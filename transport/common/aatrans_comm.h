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
#ifndef AATRANS_COMM_H
#define AATRANS_COMM_H

#include <stddef.h>

#define AASDK_MAC_ADDR_LEN              6
#define AASDK_TLV_STRING_LEN_MAX        256
#define AASDK_TLV_VALUE_LEN_MAX         256
#define AASDK_TLV_SYSTEM_ID_LEN         10

#define ETH_HEADER_LEN                  14
#define MINIMUM_ETH_PACKET_SIZE         68
#define AASDK_ELEMENT_SYSTEM_ID_LEN     10
#define AASDK_TRANSPORT_PACKET_MAX_SIZE 1500 //...TBD revisit this 

typedef int (*aasdk_transport_send_func_t) (uint32_t, char *, uint32_t);
typedef void (*print_func_t) (void *, const char *format, ...);

typedef struct {
    /* Chassis ID TLV */
    uint32_t chassis_id_subtype;
    char chassis_id[AASDK_TLV_VALUE_LEN_MAX];
    uint32_t chassis_id_len;

    /* System Name TLV */
    char chassis_name[AASDK_TLV_STRING_LEN_MAX];

    /* System Description */
    char chassis_descr[AASDK_TLV_STRING_LEN_MAX];
                            
    /* System Capability TLV */
    uint32_t chassis_cap_available;
    uint32_t chassis_cap_enabled;

    /* Auto Attach: Element System ID TLV */
    uint8_t system_id[AASDK_TLV_SYSTEM_ID_LEN];

    /* Misc */
    uint32_t tx_interval;           /* Transmit interval */
    uint32_t tx_hold;               /* Transmit hold */
    uint32_t max_neighbors;         /* Maximum number of neighbors (per protocol) */
    uint32_t advertise_version;     /* Should the system description TLV be advertised */
    bool paused;                    /* lldpd is paused */
    bool receiveonly;               /* Receive only mode */
    aasdk_transport_send_func_t send; /* function to send a frame */

} aasdk_transport_config_t;

typedef struct {
    uint32_t inserts;
    uint32_t deletes;
    uint32_t drops;
    uint32_t ageouts;
} aasdk_transport_global_stats_t;

int aatransi_global_init(aasdk_transport_config_t *config);
int aatransi_global_configure(void);
int aatransi_global_stats_get(aasdk_transport_global_stats_t *stats);
int aatransi_tx_interval_set(uint32_t value);
int aatransi_tx_interval_get(uint32_t *ret_val);
int aatransi_tx_hold_multiplier_set(uint32_t value);
int aatransi_tx_hold_multiplier_get(uint32_t *ret_val);
int aatransi_reinit_delay_set(uint32_t value);
int aatransi_reinit_delay_get(uint32_t *ret_val);
int aatransi_tx_delay_set(uint32_t value);
int aatransi_tx_delay_get(uint32_t *ret_val);
int aatransi_notification_interval_set(uint32_t value);
int aatransi_notification_interval_get(uint32_t *ret_val);

int aatransi_chassis_macaddr_set(uint8_t *macaddr);
int aatransi_mtu_set(uint32_t mtu);
int aatransi_send_pdu();
int aatransi_time_tick(void);
int aatransi_asgn_data_set(aasdk_port_id_t port_id, int status,
                           uint32_t isid, uint16_t vlan,
                           int origin, bool enable);
int aatransi_disc_elem_type_set(int elem_type);
int aatransi_disc_mgmt_vlan_set(uint16_t mgmt_vlan);
int aatransi_disc_sys_id_set(aasdk_port_id_t port_id, uint8_t *sys_id);
int aatransi_packet_process(char *buffer, uint32_t buf_size, aasdk_port_id_t port_id);
int aatransi_packet_compose(char *buffer, uint32_t buf_size, aasdk_port_id_t port_id);
int aatransi_send_pdu(void);
int aatransx_auth_key_set(aasdk_port_id_t port_id,
                          uint8_t *key, 
                          size_t key_len);
int aatransx_auth_ena_set(aasdk_port_id_t port_id, bool enable);

void aatransi_print_lldp_and_aa_stats(print_func_t print_func, void *arg);
void aatransi_print_element_status(print_func_t print_func, void *arg);
void aatransi_print_isid_status(print_func_t print_func, void *arg);


#endif /* AATRANS_COMM_H */
