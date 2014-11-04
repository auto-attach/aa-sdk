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
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_trace_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_elem

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
    aasdk_trace(aa_verbose, "elem_type=%d", elem_type);

    return AA_SDK_ENONE;
}


int
aatransi_disc_mgmt_vlan_set(uint16_t mgmt_vlan)
{
    aasdk_trace(aa_verbose, "mgmt_vlan=%d", mgmt_vlan);

    return AA_SDK_ENONE;
}


int
aatransi_disc_sys_id_set(aasdk_port_id_t port_id, uint8_t *sys_id)
{
    aasdk_trace(aa_verbose, "port_id=%d", port_id);

    return AA_SDK_ENONE;
}

