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
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_trace_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_packet

/*
 * process packet
 */
int
aatransi_packet_process(char *buffer, uint32_t buf_size, aasdk_port_id_t port_id)
{
    aasdk_trace(aa_verbose, "processing packet received on port_id %d", port_id);

    return AA_SDK_ENONE;
}


/*
 * compose packet
 */
int
aatransi_packet_compose(char *buffer, uint32_t buf_size, aasdk_port_id_t port_id)
{
    aasdk_trace(aa_verbose, "composing packet for port_id %d", port_id);

    return AA_SDK_ENONE;
}


/*
 * Run through all active ports within the box and for each port that has
 * LLDP enabled, compose an LLDPDU packet and send it out on that port.
 */
int
aatransi_send_pdu(void)
{
    aasdk_trace(aa_verbose, "sending a pdu per active interface");

    return AA_SDK_ENONE;
}


