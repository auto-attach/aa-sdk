/* aasdk_lldp.c */

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
#include <time.h>

#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"

#define LLDP_CAP_OTHER		0x01
#define LLDP_CAP_REPEATER	0x02
#define LLDP_CAP_BRIDGE		0x04
#define LLDP_CAP_WLAN		0x08
#define LLDP_CAP_ROUTER		0x10
#define LLDP_CAP_TELEPHONE	0x20
#define LLDP_CAP_DOCSIS		0x40
#define LLDP_CAP_STATION	0x80

#define LLDP_MAX_NUM_NEIGHBOR_DEF           16
#define LLDP_MESSAGE_TX_INTERVAL_DEF        30
#define LLDP_MESSAGE_TX_HOLD_MULTIPLIER_DEF 4

#define AASDK_MAC_ADDR_LEN              6
#define AASDK_TLV_STRING_LEN_MAX        256
#define AASDK_TLV_VALUE_LEN_MAX         256
#define AASDK_TLV_SYSTEM_ID_LEN         10

/* Chassis ID subtype */
#define LLDP_CHASSISID_SUBTYPE_CHASSIS	1
#define LLDP_CHASSISID_SUBTYPE_IFALIAS	2
#define LLDP_CHASSISID_SUBTYPE_PORT	3
#define LLDP_CHASSISID_SUBTYPE_LLADDR	4
#define LLDP_CHASSISID_SUBTYPE_ADDR	5
#define LLDP_CHASSISID_SUBTYPE_IFNAME	6
#define LLDP_CHASSISID_SUBTYPE_LOCAL	7

void aatransi_initialize(void)
{
    aasdk_transport_config_t config;
    uint8_t                  mac_addr[AASDK_MAC_ADDR_LEN];
    int                      rc;

    /* init the lldp data */
    memset(&config, 0, sizeof(aasdk_transport_config_t));
    
    /* Chassis ID TLV */
    memset(&mac_addr, 0, AASDK_MAC_ADDR_LEN);
/*    if (getChassisMac((tUINT8*)config.chassis_id) != SUCCESS)
     {
        printf("failed to init lldp properly- chassis\n");;
    }
*/
    config.chassis_id_subtype = LLDP_CHASSISID_SUBTYPE_LLADDR; /*mac*/
    config.chassis_id_len = AASDK_MAC_ADDR_LEN;

    printf("GET CHASSIS MAC %02x%02x%02x%02x%02x%02x\n", 
           config.chassis_id[0],
           config.chassis_id[1],
           config.chassis_id[2],
           config.chassis_id[3],
           config.chassis_id[4],
           config.chassis_id[5]);

/*    if (getSystemGrp(config.chassis_name,
              config.chassis_descr,AASDK_TLV_STRING_LEN_MAX )!= SUCCESS)
    {
        printf("failed to init lldp properly - system\n");;
    }
*/
    /* System Capability TLV */
    config.chassis_cap_available |= LLDP_CAP_BRIDGE;
    config.chassis_cap_available |= LLDP_CAP_ROUTER;
    
    config.chassis_cap_enabled   |= LLDP_CAP_BRIDGE;
    
    /* global config parameters */
    config.tx_interval   = LLDP_MESSAGE_TX_INTERVAL_DEF;
    config.tx_hold       = LLDP_MESSAGE_TX_HOLD_MULTIPLIER_DEF;

    config.max_neighbors = LLDP_MAX_NUM_NEIGHBOR_DEF;
    config.advertise_version = false;
    config.paused = false;
    config.receiveonly = false;

/*    config.send = (aasdk_transport_send_func_t) send_function; */

    rc = aatransi_global_init(&config);
    if (rc != AA_SDK_ENONE) 
    {
        printf("aasdk: aatransi_global_init() failed, rc=%d\n", rc);
    }
}


