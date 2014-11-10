/* aa-sdk/common/agent/aa_port.h */

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

#ifndef AA_PORT_H
#define AA_PORT_H

#include <stdbool.h> /* bool */

#include "aasdk_comm.h"
#include "aasdk_mgmtif_comm.h"


typedef struct aaPort_s {
  struct aaPort_s                    *next;
  struct aaPort_s                    *prev;

  aasdk_port_id_t                     id;
  bool                                ena;
  uint8_t                             locSysId[AA_SDK_SYS_ID_LEN];
  aasdk_stats_data_t                  stats;
} aaPort_t;


int aaPortCreate(   aasdk_port_id_t      portId);
int aaPortDestroy(  aasdk_port_id_t      portId);
int aaPortStatusGet(aasdk_port_id_t      portId,
                    aasdk_port_status_t *portStatus);
int aaPortStateSet( aasdk_port_id_t      portId,
                    bool                 portEna);
int aaPortsStateSet(bool                 portsEna);
int aaPortSysIdCfg( aasdk_port_id_t      portId,
                    uint8_t             *sysId);

int aaPortStatInc(  aasdk_port_id_t      portId,
                    size_t               offset);
int aaPortStatsClr( aasdk_port_id_t      portId);
int aaPortStatsGet( aasdk_port_id_t      portId,
                    aasdk_stats_data_t  *portStats);

int aaPortsStatsClr(void);
int aaPortsStatsGet(aasdk_stats_data_t  *portsStats);

#endif /* ndef AA_PORT_H */


