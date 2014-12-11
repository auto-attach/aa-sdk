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

#include "aasdk_comm.h"
#include "aasdk_lldpif_comm.h"
#include "aasdk_errno.h"

/*
 * enable and disable
 */
int
aasdkx_ports_data_ena_set(bool enable)
{
    return AA_SDK_ENONE;
}


int
aasdkx_port_data_ena_set(aasdk_port_id_t port_id, bool enable)
{
    return AA_SDK_ENONE;
}



/*
 * discovery data
 */
int
aasdkx_disc_elem_type_set(int elem_type)
{
    return AA_SDK_ENONE;
}

int
aasdkx_disc_mgmt_vlan_set(uint16_t mgmt_vlan)
{
    return AA_SDK_ENONE;
}

int
aasdkx_disc_sys_id_set(aasdk_port_id_t port_id,
                       uint8_t        *sys_id)
{
    return AA_SDK_ENONE;
}





/*
 * assignment data
 */
int
aasdkx_asgn_data_set(aasdk_port_id_t port_id, int status,
                     uint32_t isid, uint16_t vlan,
                     bool enable)
{
    return AA_SDK_ENONE;
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
