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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_port_comm.h"
#include "aatrans_trace_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_port

int
aatransi_port_create(uint32_t port_id,
                     aasdk_transport_port_config_t *p_cfg,
                     uint32_t mtu, uint8_t *system_id, 
                     uint8_t *iface_macaddr)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    return AA_SDK_ENONE;
}

int
aatransi_port_delete(uint32_t port_id)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_port_admin_get(uint32_t port_id)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    return AA_SDK_EINVAL;
}

int
aatransi_port_stats_get(aasdk_port_id_t port_id, 
                        aasdk_transport_port_stats_t *stats)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d", port_id);

    return AA_SDK_EINVAL;
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

    return AA_SDK_ENONE;
}

int
aatransi_ports_data_ena_set(bool enable)
{
    aasdk_trace(aa_verbose, "entering, enable=%d - to be implemented", enable);

    return AA_SDK_ENONE;
}

int
aatransi_port_data_ena_set(aasdk_port_id_t port_id, bool enable)
{
    aasdk_trace(aa_verbose, "entering, port_id=%d enable=%d - to be implemented", port_id, enable);

    return AA_SDK_ENONE;
}
