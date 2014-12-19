/*
 * Copyright (c) 2014, Avaya Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include "sys/queue.h"
#include "lldp-const.h"
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_trace_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_elem


int
aatrans_asgn_data_set(aasdk_port_id_t port_id, int status,
                      uint32_t isid, uint16_t vlan,
                      bool enable, bool local)
{
    struct lldpd_hardware *hardware;
    struct lldpd_aa_isid_vlan_maps_tlv *isid_vlan_map = NULL;
    struct lldpd_port *port = NULL;
    struct lldpd_port *rport = NULL;
    uint32_t map_isid = 0;
    bool found = false;

    aasdk_trace(aa_verbose, 
                "port_id=%d status=%d isid=%d vlan=%d enable=%d", 
                port_id, status, isid, vlan, enable);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {      
            /* local port */
            if (local) {
                port = &hardware->h_lport;
                bool found = false;
                
                /* search the list for an existing mapping */
                TAILQ_FOREACH(isid_vlan_map, &port->p_isid_vlan_maps, m_entries) {
                    map_isid = array_to_int(isid_vlan_map->isid_vlan_data.isid, 
                                            sizeof(isid_vlan_map->isid_vlan_data.isid)); 
                    if ((map_isid == isid) && 
                        (isid_vlan_map->isid_vlan_data.vlan == vlan)) {
                        aasdk_trace(aa_verbose, 
                                    "match found for port_id=%d isid=%d vlan=%d",
                                    port_id, isid, vlan);
                        found = true;
                        break;
                    }
                }
                
                if (enable) {
                    /* if not found add mapping */
                    if (!found) {
                        if ((isid_vlan_map = (struct lldpd_aa_isid_vlan_maps_tlv *) 
                             calloc(1, sizeof(struct lldpd_aa_isid_vlan_maps_tlv)))
                            == NULL ) {
                            aasdk_log("Error: unable to allocate memory for aa_isid_vlan_maps_tlv struct");
                            return AA_SDK_ENOMEM;
                        }
                        /* vlan is last 12 bits */
                        isid_vlan_map->isid_vlan_data.vlan = vlan & 0x0FFF;
                        
                        /* isid is 24 bits */
                        int_to_array(isid_vlan_map->isid_vlan_data.isid, 
                                     sizeof(isid_vlan_map->isid_vlan_data.isid), isid);
                        
                        /* status is first 4 most-significant bits */
                        isid_vlan_map->isid_vlan_data.status = status & 0x000F;
                        
                        aasdk_trace(aa_verbose,
                                    "Adding mapping isid=%d vlan=%d status=%d to port_id %d",
                                    isid, vlan, status, port_id);

                        TAILQ_INSERT_TAIL(&port->p_isid_vlan_maps, isid_vlan_map, m_entries);
                    }
                    /* if found, just update the status */
                    else {
                        aasdk_trace(aa_verbose,
                                    "Updating mapping isid=%d vlan=%d status=%d for port_id %d",
                                    isid, vlan, status, port_id);
                        
                        /* status is first 4 most-significant bits */
                        isid_vlan_map->isid_vlan_data.status = status & 0x000F;

                    }

                }
                else {
                    if (found) {
                        aasdk_trace(aa_verbose,
                                    "deleting mapping isid=%d vlan=%d from port_id %d",
                                    isid, vlan, port_id);
                        TAILQ_REMOVE(&port->p_isid_vlan_maps, isid_vlan_map, m_entries);
                    }
                    else {
                        aasdk_log("Error: mapping isid=%d vlan=%d not found for port_id=%d",
                                  isid, vlan, port_id);
                        return AA_SDK_EINVAL;
                    }
                }

                return AA_SDK_ENONE;

            } /* if (local) */

            /* remote port */
            else {
                TAILQ_FOREACH (rport, &hardware->h_rports, p_entries) {
                
                    /* search the list for an existing mapping */
                    if ( !TAILQ_EMPTY(&rport->p_isid_vlan_maps) ) {
                        TAILQ_FOREACH (isid_vlan_map, &rport->p_isid_vlan_maps, m_entries) {
                            found = false;
                            
                            map_isid = array_to_int(isid_vlan_map->isid_vlan_data.isid, 
                                                    sizeof(isid_vlan_map->isid_vlan_data.isid)); 
                            if ((map_isid == isid) && 
                                (isid_vlan_map->isid_vlan_data.vlan == vlan)) {
                                aasdk_trace(aa_verbose, 
                                            "match found for port_id=%d isid=%d vlan=%d",
                                            port_id, isid, vlan);
                                found = true;
                                break;
                            }
                        }
                        
                        if (enable) {
                            /* if not found, cannot add through this API return error */
                            if (!found) {
                                aasdk_log("Error: Remote mapping not found isid=%d vlan=%d status=%d for port_id %d",
                                          isid, vlan, status, port_id);
                                return AA_SDK_EINVAL;
                            }
                            /* if found, just update the status */
                            else {
                                aasdk_trace(aa_verbose,
                                            "Updating mapping isid=%d vlan=%d status=%d for port_id %d",
                                            isid, vlan, status, port_id);
                                
                                /* status is first 4 most-significant bits */
                                isid_vlan_map->isid_vlan_data.status = status & 0x000F;
                            }
                        }
                        else {
                            if (found) {
                                aasdk_trace(aa_verbose,
                                            "deleting mapping isid=%d vlan=%d from port_id %d",
                                            isid, vlan, port_id);
                                TAILQ_REMOVE(&rport->p_isid_vlan_maps, isid_vlan_map, m_entries);
                            }
                            else {
                                aasdk_log("Error: Unable to delete remote mapping isid=%d vlan=%d port_id=%d, not found",
                                          isid, vlan, port_id);
                                return AA_SDK_EINVAL;
                            }
                        }

                        return AA_SDK_ENONE;

                    } /* if ( !TAILQ_EMPTY(&rport->p_isid_vlan_maps) ) */
                    
                } /* TAILQ_FOREACH (rport, &hardware->h_rports, p_entries) */
                
            } /* remote */
            
            return AA_SDK_ENONE;

        } /* if (hardware->h_ifindex == port_id) */
        
    } /* TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) */

    aasdk_trace(aa_terse, "Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_asgn_data_set(aasdk_port_id_t port_id, int status,
                     uint32_t isid, uint16_t vlan,
		     bool enable)
{
    return (aatrans_asgn_data_set(port_id, status, isid, vlan, enable, false));
}

int
aatransi_disc_elem_type_set(int elem_type)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "elem_type=%d >>>>>>>>>>>>>", elem_type);

    cfg_ext.aa_element_type = elem_type;

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        hardware->h_lport.p_element.type = (elem_type & 0xF);
    }

    return AA_SDK_ENONE;
}


int
aatransi_disc_mgmt_vlan_set(uint16_t mgmt_vlan)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "mgmt_vlan=%d >>>>>>>>>>>>>", mgmt_vlan);

    cfg_ext.aa_element_mgmt_vlan = mgmt_vlan;

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        hardware->h_lport.p_element.mgmt_vlan = (mgmt_vlan & 0xFFF);
    }

    return AA_SDK_ENONE;
}

int
aatransi_disc_sys_id_set(aasdk_port_id_t port_id, uint8_t *sys_id)
{
    struct lldpd_hardware *hardware;

    if(sys_id == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    aasdk_trace(aa_verbose, 
                "port_id=%d sys_id %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                port_id,
                sys_id[0],
                sys_id[1],
                sys_id[2],
                sys_id[3],
                sys_id[4],
                sys_id[5],
                sys_id[6],
                sys_id[7],
                sys_id[8],
                sys_id[9]);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if(port_id == hardware->h_ifindex) {
            memcpy(&hardware->h_lport.p_element.system_id, sys_id, AASDK_ELEMENT_SYSTEM_ID_LEN);
        }
    }

    return AA_SDK_ENONE;
}

void
aatransi_print_element_status(print_func_t print_func, void *arg)
{
    if(!print_func || !arg) {
        return;
    }
}

//static struct mapping_internal *
//mapping_find_by_isid(struct lldp *lldp, const uint64_t isid)
//{
////    struct mapping_internal *m;
//
////    HMAP_FOR_EACH_IN_BUCKET (m, hmap_node, hash_bytes(&isid, sizeof(isid), 0), &lldp->mappings) {
////        if (isid == m->isid) {
////            return m;
////        }
////    }
//
//    return NULL;
//}
