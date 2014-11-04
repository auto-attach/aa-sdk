/* aa-sdk/common/stubs/aasdk_mgmtif_comm.c */

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
#include "aasdk_errno.h"
#include "aasdk_mgmtif_comm.h"


/*
 * Configuration
 */

int
aasdki_port_create(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}

int
aasdki_port_destroy(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}


int
aasdki_elem_type_cfg(int elem_type)
{
    return AA_SDK_ENONE;
}

int
aasdki_elem_mgmt_vlan_cfg(uint16_t mgmt_vlan)
{
    return AA_SDK_ENONE;
}

int
aasdki_elem_sys_id_cfg(uint8_t *sys_id)
{
    return AA_SDK_ENOTSUP; /* depricated */
}

int
aasdki_glbl_rem_asgn_exp_cfg(int rem_asgn_exp)
{
    return AA_SDK_ENONE;
}


int
aasdki_port_aa_ena(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}

int
aasdki_port_aa_dis(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}


int
aasdki_port_sys_id_cfg(aasdk_port_id_t port_id,
		       uint8_t *sys_id)
{
    return AA_SDK_ENONE;
}


int
intaasdki_port_auth_cfg(aasdk_port_id_t port_id,
                         uint8_t *key, size_t key_len)
{
    return AA_SDK_ENONE;
}

int
aasdki_port_auth_ena(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}

int
aasdki_port_auth_dis(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}


int aasdki_port_asgn_add(aasdk_port_id_t port_id,
                         uint32_t isid, uint16_t vlan)
{
    return AA_SDK_ENONE;
}

int aasdki_port_asgn_rem(aasdk_port_id_t port_id,
                         uint32_t isid, uint16_t vlan)
{
    return AA_SDK_ENONE;
}


/*
 * Status
 */

int
aasdki_glbl_status_get(aasdk_glbl_status_t *p_glbl_sts)
{
    return AA_SDK_ENONE;
}


int
aasdki_port_status_get( aasdk_port_id_t port_id,
                        aasdk_port_status_t *pportsts)
{
    return AA_SDK_ENONE;
}

int
aasdki_ports_status_get(size_t  max_num_ports,
                        size_t *act_num_ports,
                        aasdk_port_status_t *pportsts)
{
    return AA_SDK_ENONE;
}


int
aasdki_asgn_status_get( aasdk_port_id_t port_id,
                        uint32_t isid,
                        uint16_t vlan,
                        aasdk_asgn_status_t *pasgnsts)
{
    return AA_SDK_ENONE;
}


int
aasdki_asgns_status_get(size_t  max_num_asgns,
                        size_t *act_num_asgns,
                        aasdk_asgn_status_t *pasgnsts)
{
    return AA_SDK_ENONE;
}


/*
 * Statistics
 */

int
aasdki_glbl_stats_get(aasdk_stats_data_t *pstats)
{
    return AA_SDK_ENONE;
}



/*
 * Events
 */

int
aasdki_event_register(unsigned int events,
                     aasdk_event_func_t event_handler,
                     void *context)
{
    return AA_SDK_ENONE;
}

void
aasdki_event_unregister(int callback_id)
{
    return;
}


