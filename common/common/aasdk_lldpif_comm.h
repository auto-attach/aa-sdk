/* aa-sdk/common/common/aasdk_lldpif_comm.h */

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

#ifndef AASDK_LLDPIF_COMM_H
#define AASDK_LLDPIF_COMM_H

#include <stdbool.h> /* bool */
#include <stddef.h>  /* size_t */

#include "aasdk_comm.h"


/*
 * enable and disable
 */
int aasdki_ports_data_ena(void);
int aasdki_ports_data_dis(void);

int aasdki_port_data_ena(aasdk_port_id_t port_id);
int aasdki_port_data_dis(aasdk_port_id_t port_id);

int aasdkx_ports_data_ena_set(bool enabled);
int aasdkx_port_data_ena_set(aasdk_port_id_t port_id, bool enable);


/*
 * discovery data
 */
int aasdki_disc_data_set(aasdk_port_id_t port_id, int elem_type,
                         uint16_t mgmt_vlan, uint8_t *sys_id);

int aasdkx_disc_elem_type_set(int elem_type);
int aasdkx_disc_mgmt_vlan_set(uint16_t mgmt_vlan);
int aasdkx_disc_sys_id_set(aasdk_port_id_t port_id,
                           uint8_t *sys_id);


/*
 * assignment data
 */
int aasdkx_asgn_data_set(aasdk_port_id_t port_id, int status,
                         uint32_t isid, uint16_t vlan,
                         int origin, bool enable);
int aasdki_asgn_data_set(aasdk_port_id_t port_id, int status,
                         uint32_t isid, uint16_t vlan);

/*
 * authentication
 */
int aasdkx_auth_key_set(aasdk_port_id_t port_id,
                        uint8_t *key, size_t key_len);
int aasdkx_auth_ena_set(aasdk_port_id_t port_id, bool enable);
int aasdki_auth_err(aasdk_port_id_t port_id);


#endif /* AASDK_LLDPIF_COMM_H */
