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
#ifndef AATRANS_PORT_COMM_H
#define AATRANS_PORT_COMM_H

typedef struct {
    /* PORT ID TLV */
    uint32_t port_id_subtype;
    uint8_t port_id[AASDK_TLV_VALUE_LEN_MAX];
    uint32_t port_id_len;

} aasdk_transport_port_config_t;

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t rx_discard_cnt;
    uint32_t rx_unrecognized_tlv_cnt;
    uint32_t ageout_cnt;
} aasdk_transport_port_stats_t;

typedef struct {
    uint32_t time_mark; //...TBD
    uint32_t index; //...TBD

    /* Chassis/System */
    uint32_t chassis_id_len;
    uint32_t chassis_id_subtype;
    char chassis_id[AASDK_TLV_VALUE_LEN_MAX];
    char chassis_name[AASDK_TLV_STRING_LEN_MAX];  /* sys_name */
    char chassis_descr[AASDK_TLV_STRING_LEN_MAX]; /* sys_descr */
    uint32_t chassis_cap_supported;
    uint32_t chassis_cap_enabled;

    /* Port */ 
    uint32_t port_id_len;
    uint32_t port_id_subtype;
    char port_id[AASDK_TLV_VALUE_LEN_MAX];
    char port_descr[AASDK_TLV_STRING_LEN_MAX];
                            
    /* Auto Attach: Element */
    uint8_t     elem_system_id[AASDK_TLV_SYSTEM_ID_LEN];
    uint32_t    elem_type;
    uint32_t    elem_mgmt_vlan;

} aatrans_disc_neighbor_entry_t;

int aatransi_ports_data_ena_set(bool enable);
int aatransi_port_data_ena_set(aasdk_port_id_t port_id, bool enable);
int aatransi_port_admin_get(uint32_t port_id);
int aatransi_notify_port_up(aasdk_port_id_t port_id);
int aatransi_notify_port_down(aasdk_port_id_t port_id);
int aatransi_port_stats_get(aasdk_port_id_t port_id, 
                                    aasdk_transport_port_stats_t *stats);
int aatransi_port_create(uint32_t port_id, 
                                 aasdk_transport_port_config_t *p_cfg,
                                 uint32_t mtu, 
                                 uint8_t *system_id, 
                                 uint8_t *iface_macaddr);
int aatransi_port_delete(uint32_t port_id);

#endif /* AATRANS_PORT_COMM_H */
