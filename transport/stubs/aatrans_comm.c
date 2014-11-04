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
#include "aatrans_port_comm.h"
#include "aatrans_trace_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_common

/*
 * initialize transport protocol
 */
int
aatransi_global_init(aasdk_transport_config_t *config)
{
    aasdk_log("Initializing the transport module");

    return AA_SDK_ENONE;
}

/*
 * configure transport protocol
 */
int
aatransi_global_configure(void) /*struct lldp *lldp, const struct lldp_settings *settings)*/
{
    aasdk_trace(aa_verbose, "configuring the transport module");

    return AA_SDK_ENONE;
}

int
aatransi_tx_interval_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    return AA_SDK_ENONE;
}

int
aatransi_tx_interval_get(uint32_t *ret_val)
{
    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_tx_hold_multiplier_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    return AA_SDK_ENONE;
}

int
aatransi_tx_hold_multiplier_get(uint32_t *ret_val)
{
    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_reinit_delay_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    return AA_SDK_ENONE;
}

int
aatransi_reinit_delay_get(uint32_t *ret_val)
{
    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_tx_delay_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    return AA_SDK_ENONE;
}

int
aatransi_tx_delay_get(uint32_t *ret_val)
{
    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_notification_interval_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    return AA_SDK_ENONE;
}

int
aatransi_notification_interval_get(uint32_t *ret_val)
{
    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_global_stats_get(aasdk_transport_global_stats_t *stats)
{
    aasdk_trace(aa_verbose, "getting global stats");

    return AA_SDK_ENONE;
}

int
aatransi_chassis_macaddr_set(uint8_t *macaddr)
{
    aasdk_trace(aa_verbose, "setting chassis macaddr");

    return AA_SDK_ENONE;
}

int
aatransi_mtu_set(uint32_t mtu)
{
    aasdk_trace(aa_verbose, "mtu=%d", mtu);

    return AA_SDK_ENONE;
}

