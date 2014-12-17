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
#ifdef ENABLE_AASERVER
aatransi_packet_process(char *buffer, uint32_t buf_size, 
                        aasdk_port_id_t port_id, void *cfg_param)
#else
aatransi_packet_process(char *buffer, uint32_t buf_size, 
                        aasdk_port_id_t port_id)
#endif
{
    struct lldpd_hardware *hardware = NULL;
#ifdef ENABLE_AASERVER
    struct lldpd *cfg = (struct lldpd *) cfg_param;
#endif

    aasdk_trace(aa_verbose, "processing packet received on port_id %d", port_id);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id) {
            lldpd_recv(cfg, hardware, buffer, buf_size);
            return AA_SDK_ENONE;
        }
    }

    aasdk_trace(aa_verbose, "Error: port_id %d not found, no packet processed", port_id);

    return AA_SDK_EINVAL;
}


/*
 * compose packet
 */
int
#ifdef ENABLE_AASERVER
aatransi_packet_compose(char *buffer, uint32_t buf_size, 
                        aasdk_port_id_t port_id, void *cfg_param)
#else
aatransi_packet_compose(char *buffer, uint32_t buf_size, aasdk_port_id_t port_id)
#endif
{
    struct lldpd_hardware *hardware = NULL;
    uint32_t lldp_size = 0;
#ifdef ENABLE_AASERVER
    struct lldpd *cfg = (struct lldpd *) cfg_param;
#endif

    aasdk_trace(aa_verbose, "composing packet for port_id %d", port_id);

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        if (hardware->h_ifindex == port_id) {
            lldp_size = lldpd_send(hardware, buffer);

            if (lldp_size + ETH_HEADER_LEN < MINIMUM_ETH_PACKET_SIZE) {
                lldp_size = MINIMUM_ETH_PACKET_SIZE ;
            }
            return lldp_size;
       }
    }

    aasdk_trace(aa_verbose, "Error: port_id %d not found, no packet composed", port_id);

    return 0;
}


/*
 * Run through all active ports within the box and for each port that has
 * LLDP enabled, compose an LLDPDU packet and send it out on that port.
 */
int
aatransi_send_pdu(void)
{
    struct lldpd_hardware *hardware;
#ifndef ENABLE_AASERVER
    uint32_t len;
#endif
    char *buf = NULL;

    if ((buf = (char *) calloc(1, AASDK_TRANSPORT_PACKET_MAX_SIZE)) == NULL) {
        aasdk_log("Error: unable to allocate memory for pdu");
        return AA_SDK_ENOMEM;
    }
    
    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        // construct the TLVs
#ifndef ENABLE_AASERVER
        len = aatransi_packet_compose(buf, 
                                      AASDK_TRANSPORT_PACKET_MAX_SIZE, 
                                      (aasdk_port_id_t)hardware->h_ifindex);
        if (cfg_ext.send) {
            (cfg_ext.send)(hardware->h_ifindex, buf, len);
        }
#endif
        /* upon return from send, clear the buffer for the next interface */
        memset(buf, 0, AASDK_TRANSPORT_PACKET_MAX_SIZE);
    }

    free(buf);

    return AA_SDK_ENONE;
}


