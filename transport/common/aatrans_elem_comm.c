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
//#include "aasdk_plat/aasdk_lldpif_plat.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_elem

static struct lldp_aa_element_system_id system_id_null;

int
aatransi_asgn_data_set(aasdk_port_id_t port_id, int status,
                     uint32_t isid, uint16_t vlan,
                     int origin, bool enable)
{
    aasdk_trace(aa_verbose, 
                "port_id=%d status=%d isid=%d vlan=%d origin=%d, enable=%d", 
                port_id, status, isid, vlan, origin, enable);

    return AA_SDK_ENONE;
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

inline static uint32_t
array_to_int(uint8_t *array, size_t len)
{
    uint32_t res = 0;
    unsigned int i = 0;

    for (i = 0; i < len; i++) {
        res = res | (array[len - i - 1] << (i * 8));
    }

    return res;
}

void
aatransi_print_isid_status(print_func_t print_func, void *arg)
{
    if(!print_func || !arg) {
        return;
    }

    (print_func)(arg, "LLDP ISID-VLAN Mappings:\n");

    if (cfg) {
        struct lldpd_hardware *hardware;
//        struct mapping_internal *m;

        /* TODO Who enforces that the I-SID are unique per ports ?  This triple loop is wicked ... */
        TAILQ_FOREACH (hardware, &cfg->g_hardware, h_entries) {
            struct lldpd_port *port;

            TAILQ_FOREACH (port, &hardware->h_rports, p_entries) {
                struct lldpd_aa_isid_vlan_maps_tlv *mapping;

                TAILQ_FOREACH (mapping, &port->p_isid_vlan_maps, m_entries) {
                    uint32_t isid = array_to_int(mapping->isid_vlan_data.isid, 
                        sizeof(mapping->isid_vlan_data.isid));
//                    struct mapping_internal *m = mapping_find_by_isid(lldp, isid);

                    aasdk_trace(aa_verbose, "h_rport: isid=%u, vlan=%u, status=%d", 
                                isid, 
                                mapping->isid_vlan_data.vlan,
                                mapping->isid_vlan_data.status);

                    /* Update the status of our internal state for the mapping */
//                    if (m) {
//                        aasdk_trace(aa_verbose, "setting status for ISID=%u to %u", 
//                                 isid, 
//                                 mapping->isid_vlan_data.status);
//                        m->status = mapping->isid_vlan_data.status;
//                    } else {
//                        aasdk_log("Error: couldn't find mapping for I-SID=%u", isid);
//                    }
                }
            }
        }

        (print_func)(arg, "%-8s %-4s %-11s %-8s\n", 
                          "I-SID",
                          "VLAN",
                          "Source",
                          "Status");
        (print_func)(arg, "-------- ---- ----------- --------\n");

//        HMAP_FOR_EACH (m, hmap_node, &lldp->mappings) {
//            /* TODO Make more validation (for e.g. verify that VLAN is the same, etc.) */
//            (print_func)(arg, "%-8ld %-4ld %-11s %-11s\n",
//                              (long int) m->isid,
//                              (long int) m->vlan,
//                              "Switch",
//                              fa_status_to_str(m->status));
//        }
    }
}

