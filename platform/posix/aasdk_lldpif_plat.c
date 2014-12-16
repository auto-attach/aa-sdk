/* aa-sdk/platform/stubs/aasdk_lldpif_plat.c */

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
#include "aasdk_lldpif_comm.h"
#include "aasdk_errno.h"
#include "sys/queue.h"
#include "lldp-const.h"
#include "aatrans_comm.h"
#include "aatrans_port_comm.h"

/*
 * called when AA is globally enabled or disabled
 */
int
aasdkx_ports_data_ena_set(bool enable)
{
    return (aatransi_ports_data_ena_set(enable));
}


/* 
 * called when AA is enabled or disabled on a port
 */
int
aasdkx_port_data_ena_set(aasdk_port_id_t port_id, bool enable)
{
    return (aatransi_port_data_ena_set(port_id, enable));
}


/*
 * assignment data
 */
int
aasdkx_asgn_data_set(aasdk_port_id_t port_id, int status,
                     uint32_t isid, uint16_t vlan,
                     bool enable)
{
    return (aatransi_asgn_data_set(port_id, status, isid, vlan, enable));
}


int
aasdkx_disc_elem_type_set(int elem_type)
{
    return (aatransi_disc_elem_type_set(elem_type));
}


int
aasdkx_disc_mgmt_vlan_set(uint16_t mgmt_vlan)
{
    return (aatransi_disc_mgmt_vlan_set(mgmt_vlan));
}


int
aasdkx_disc_sys_id_set(aasdk_port_id_t port_id, uint8_t *sys_id)
{
    return (aatransi_disc_sys_id_set(port_id, sys_id));
}


/*
 * authentication
 */
int
aasdkx_auth_key_set(aasdk_port_id_t port_id,
                    uint8_t *key, size_t key_len)
{
    return AA_SDK_ENONE;
}


int
aasdkx_auth_ena_set(aasdk_port_id_t port_id, bool enable)
{
    return AA_SDK_ENONE;
}




