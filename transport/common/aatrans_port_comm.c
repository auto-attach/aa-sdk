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
#include "aatrans_port_comm.h"
#include "aatrans_trace_comm.h"
#include "aasdk_lldpif_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_port

/* port_id is the ifindex associated with the ingress port.  In case of MLT
   the ifindex associated with the first MLT port member should be passed in.
 */
int
aatransi_port_create(uint32_t port_id,
                     aasdk_transport_port_config_t *p_cfg,
                     uint32_t mtu, uint8_t *system_id, 
                     uint8_t *iface_macaddr)
{
    struct lldpd_hardware *hardware;
    char ifname[2048]={0};
    char *p = ifname;
    uint32_t i;

    if ((p_cfg == NULL) || (system_id == NULL) || (iface_macaddr == NULL)) {
        aasdk_log("Error: null argument, %p %p %p", 
                  p_cfg, system_id, iface_macaddr);
        return AA_SDK_EINVAL;
    }

    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    for(i = 0; i < p_cfg->port_id_len; i++) {
        if(p_cfg->port_id_len >= (AASDK_TLV_VALUE_LEN_MAX-1)) {
            break;
        }
        p += sprintf(p, "%02x:", p_cfg->port_id[i]);
    }

    hardware = lldpd_alloc_hardware(cfg, ifname, port_id);
    if (hardware == NULL) {
        aasdk_log("Error: failed to allocate memory for hardware interface %d", port_id);
        return AA_SDK_ENOMEM;
    }

    hardware->h_flags |= IFF_RUNNING;
    hardware->h_mtu = mtu;
    hardware->h_lport.p_id_subtype = p_cfg->port_id_subtype;
    if ((hardware->h_lport.p_id = (char *) 
         calloc(1, p_cfg->port_id_len)) == NULL) {
        aasdk_log("Error: failed to allocate memory for port_id");
        return AA_SDK_ENOMEM;
    }
    memcpy(hardware->h_lport.p_id, p_cfg->port_id, p_cfg->port_id_len);
    hardware->h_lport.p_id_len = p_cfg->port_id_len;
    
    /* Auto Attach element tlv */
    hardware->h_lport.p_element.type = cfg_ext.aa_element_type;;
    hardware->h_lport.p_element.mgmt_vlan = cfg_ext.aa_element_mgmt_vlan;
    memcpy(hardware->h_lladdr, iface_macaddr, AASDK_MAC_ADDR_LEN);
    memcpy(&hardware->h_lport.p_element.system_id, system_id, 
           AASDK_ELEMENT_SYSTEM_ID_LEN);
    TAILQ_INIT(&hardware->h_lport.p_isid_vlan_maps);
    TAILQ_INSERT_TAIL(&cfg->g_hardware, hardware, h_entries);
    
    /* notify AA */
    aasdki_port_data_ena(port_id);

#if 0   //...TBD
    HMAP_FOR_EACH (m, hmap_node, cfg_ext.mappings_by_isid) {
        struct mapping_internal *p;
        
        p = xzalloc(sizeof *p);
        memcpy(p, m, sizeof *p);
        hmap_insert(&lldp->mappings, &p->hmap_node, 
                    hash_bytes((const void *) &p->isid, sizeof(p->isid), 0));
        
        update_mapping_on_lldp(lldp, hardware, p);
    }
#endif
    
    return AA_SDK_ENONE;
}

int
aatransi_port_delete(uint32_t port_id)
{
    struct lldpd_hardware *hardware;
    uint8_t sys_id[AASDK_ELEMENT_SYSTEM_ID_LEN] = {0};
    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);
    int ret = AA_SDK_ENONE;

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {
#if AA_TBD
            /* notify AA */
            aasdki_port_data_dis(port_id);
#else
            //...TBD take this out once aasdki_port_data_dis is implemented
            if ((ret = aasdki_disc_data_set(hardware->h_ifindex, 0, 0, sys_id))
                != AA_SDK_ENONE) {
                log_warn("aatransi_port_delete","asdki_disc_data_set failed!");
                return ret;
            }
            log_info("aatransi_port_delete",
                     "aasdki_disc_data_set port_id %d to clear discovery data <<<<<<<<<<<",
                     hardware->h_ifindex);
#endif
            TAILQ_REMOVE(&cfg->g_hardware, hardware, h_entries);
            
            return AA_SDK_ENONE;
        }
    }

    aasdk_log("Error: port_id %d not found!", port_id);
    
    return AA_SDK_EINVAL;
}

/* caller is responsible for allocating and freeing the supplied data storage */
int
aatransi_port_data_get(uint32_t port_id,
                       aasdk_transport_port_config_t *p_cfg,
                       uint32_t *mtu, uint8_t *system_id, 
                       uint8_t *iface_macaddr)
{
    struct lldpd_hardware *hardware;

    if ((p_cfg == NULL) | (mtu == NULL) || 
        (system_id == NULL) || (iface_macaddr == NULL)) {
        aasdk_log("Error: null argument(s)");
        return AA_SDK_EINVAL;
    }

    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {      
            p_cfg->port_id_subtype = hardware->h_lport.p_id_subtype;
            memcpy(p_cfg->port_id, hardware->h_lport.p_id,  hardware->h_lport.p_id_len);
            p_cfg->port_id_len = hardware->h_lport.p_id_len;
            *mtu = hardware->h_mtu;

            /* Auto Attach element tlv */
            cfg_ext.aa_element_type = hardware->h_lport.p_element.type;
            cfg_ext.aa_element_mgmt_vlan = hardware->h_lport.p_element.mgmt_vlan;
            memcpy(iface_macaddr, hardware->h_lladdr, AASDK_MAC_ADDR_LEN);
            memcpy(system_id, &hardware->h_lport.p_element.system_id,
                   AASDK_ELEMENT_SYSTEM_ID_LEN);

            return AA_SDK_ENONE;
        }
    }

    aasdk_trace(aa_terse, "Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_port_admin_get(uint32_t port_id)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {      
            return AA_SDK_ENONE;
        }
    }

    aasdk_log("Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_port_stats_get(aasdk_port_id_t port_id, 
                        aasdk_transport_port_stats_t *stats)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    if (stats == NULL) {
        aasdk_log("Error: null argument");
        return AA_SDK_EINVAL;
    }

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {
            stats->rx_cnt = hardware->h_rx_cnt;
            stats->tx_cnt = hardware->h_tx_cnt;
            stats->rx_discard_cnt = hardware->h_rx_discarded_cnt;
            stats->rx_unrecognized_tlv_cnt = hardware->h_rx_unrecognized_cnt;
            stats->ageout_cnt = hardware->h_ageout_cnt;
            return AA_SDK_ENONE;
        }
    }

    aasdk_log("Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_port_mtu_set(aasdk_port_id_t port_id, uint32_t value)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "port_id=%d value=%d", port_id, value);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id) {
            hardware->h_mtu = value;
            return AA_SDK_ENONE;
        }
    }

    aasdk_log("Error: port_id %d not found!", port_id);
    
    return AA_SDK_EINVAL;
}

int
aatransi_port_mtu_get(aasdk_port_id_t port_id, uint32_t *ret_val)
{
    struct lldpd_hardware *hardware;

    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id) {
            *ret_val = hardware->h_mtu;
            aasdk_trace(aa_verbose, "port_id=%d ret_val=%d", port_id, *ret_val);
            return AA_SDK_ENONE;
        }
    }


    return AA_SDK_ENONE;
}

int
aatransi_notify_port_up(aasdk_port_id_t port_id)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d - to be implemented", port_id);

    return AA_SDK_ENONE;
}

int
aatransi_notify_port_down(aasdk_port_id_t port_id)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d - to be implemented", port_id);

    /* cleanup lldpd database of all stale port and chassis objects */
    lldpd_cleanup(cfg);

    return AA_SDK_ENONE;
}

int
aatransi_ports_data_ena_set(bool enable)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "global AA %s", (enable) ? "enabled" : "disabled");

    /* no change, do nothing */
    if (cfg->g_aa_global_enabled == enable) {
        return AA_SDK_ENONE;
    }
    cfg->g_aa_global_enabled = enable;

    if (enable) {
        aasdk_trace(aa_verbose, "global AA enabled, notify AA");

        TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
            if (hardware->h_aa_enabled) {
                aasdk_trace(aa_verbose, "global AA enabled, set aa_notify flag for %d", hardware->h_ifindex);
                hardware->h_aa_notify = true;
            }
        }
    }
    else {
        cfg->g_aa_global_enabled = false;
        aasdk_trace(aa_verbose, "global AA disabled");
    }
    
    return AA_SDK_ENONE;
}

/* 
 * called when AA is enabled or disabled on a port
 */
int
aatransi_port_data_ena_set(aasdk_port_id_t port_id, bool enable)
{
    struct lldpd_hardware *hardware;

    aasdk_trace(aa_verbose, "AA %s on port %d", (enable) ? "enabled" : "disabled", port_id);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id) {
            /* no change, do nothing */
            if (hardware->h_aa_enabled == enable) {
                return AA_SDK_ENONE;
            }
            hardware->h_aa_enabled = enable;
            if (enable) {
                if (cfg->g_aa_global_enabled) {
                    aasdk_trace(aa_verbose, "AA enabled on port_id %d, set aa_notify flag for %d", port_id);                
                    hardware->h_aa_notify = true;
                }
            }
            else {
                aasdk_trace(aa_verbose, "AA disabled on port_id %d - no action", port_id);
            }
            return AA_SDK_ENONE;
        }
    }    

    aasdk_trace(aa_terse, "Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}

/* caller is responsible for allocating and freeing the supplied data storage */
int
aatransi_port_neighbors_get(aasdk_port_id_t port_id, char *buf, uint32_t *retcount)
{
    struct lldpd_hardware *hardware;
    struct lldpd_port *port;
    aatrans_disc_neighbor_entry_t *neighbor = NULL;
    int count = 0;

    //...TBD sanity check input params here
    if ((buf == NULL) || (retcount == NULL)) {
        aasdk_log("Error: null argument(s)\n");
        return AA_SDK_EINVAL;
    }

    *retcount = 0;

    aasdk_trace(aa_verbose, "getting discovered neighbors for port_id %d buf=%p", port_id, buf);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id)
        {
            neighbor = (aatrans_disc_neighbor_entry_t *) buf;
            
            /* populate the buffer */
            TAILQ_FOREACH(port, &hardware->h_rports, p_entries) {
                /* Chassis */
                if (port->p_chassis->c_name) {
                    memcpy(neighbor->chassis_name, port->p_chassis->c_name, strlen(port->p_chassis->c_name));
                }
                if (port->p_chassis->c_id) {
                    memcpy(neighbor->chassis_id, port->p_chassis->c_id, port->p_chassis->c_id_len);
                }
                neighbor->chassis_id_subtype = port->p_chassis->c_id_subtype;
                neighbor->chassis_id_len = port->p_chassis->c_id_len;
                neighbor->chassis_cap_supported = port->p_chassis->c_cap_available;
                neighbor->chassis_cap_enabled = port->p_chassis->c_cap_enabled;
                if (port->p_chassis->c_descr) {
                    memcpy(neighbor->chassis_descr, port->p_chassis->c_descr, strlen(port->p_chassis->c_descr));
                }
                /* Port */
                if (port->p_id) {
                    memcpy(neighbor->port_id, port->p_id, port->p_id_len);
                }
                neighbor->port_id_subtype = port->p_id_subtype;
                neighbor->port_id_len = port->p_id_len;
                if (port->p_descr) {
                    memcpy(neighbor->port_descr, port->p_descr, strlen(port->p_descr));
                }
                /* AA Element */
                memcpy(neighbor->elem_system_id, &port->p_element.system_id, sizeof(port->p_element.system_id));
                neighbor->elem_mgmt_vlan = port->p_element.mgmt_vlan;
                neighbor->elem_type = port->p_element.type;

                neighbor++;
                count++;
            }

            *retcount = count;
            return AA_SDK_ENONE;
        }
    }    

    aasdk_trace(aa_terse, "Error: port_id %d not found!", port_id);

    return AA_SDK_EINVAL;
}
