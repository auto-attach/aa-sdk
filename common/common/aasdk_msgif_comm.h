/* aa-sdk/common/common/aasdk_msgif_comm.h */

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

#ifndef AASDK_MSGIF_COMM_H
#define AASDK_MSGIF_COMM_H

#include <stdint.h>  /* uint*_t */
#include <stddef.h>  /* size_t  */

#include "aasdk_msgif_plat.h"

#if 1
#undef  AA_SDK_USE_MSG_QUEUE
#else
#define AA_SDK_USE_MSG_QUEUE
#endif

typedef struct aasdk_msg_s
{
    int      msgId;  /* AA_SDK_MSG_* */
    uint32_t arg0;
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
    uint32_t arg5;
} aasdk_msg_t;

enum {
    AA_SDK_MSG_UNDEFINDED = 0,
    AA_SDK_MSG_LIST_MAINT_TIMER = 7,
    /* arg0 = struct timespec.sec
     * arg1 = struct timespec.nsec
     */
    AA_SDK_LLDP_DISCOVERY_UPDATE = 8,
    /* arg0 = port ID
     * arg1 = type
     * arg2 = mgmt VLAN
     * arg3 = chassis ID 0-3
     * arg4 = chassis ID 4-7
     * arg5 = chassis ID 8-9
     */
    AA_SDK_MSG_LLDP_BINDING_UPDATE = 9,
    /* arg0 = port ID
     * arg1 = ISID
     * arg2 = VLAN
     * arg3 = origin
     * arg4 = status
     */
    AA_SDK_MSG_GLOBAL_SPBM_UPDATE = 14,
    /* arg1 = AA_SDK_GLOBAL_SPBM_*
     */
    AA_SDK_MSG_SMLT_PERR = 15,
    /* arg 0 = msg tpe, msg subtype
     */
    AA_SDK_MSG_SMLT_EVENT = 16
    /* arg 0 = AA_SDK_MSG_SMLT_EVENT_*
     */
};

enum {
    AA_SDK_GLOBAL_SPBM_ENABLED = 1,
    AA_SDK_GLOBAL_SPBM_DISABLED = 2
};

enum {
    AA_SDK_MSG_SMLT_EVENT_IST_UP,
    AA_SDK_MSG_SMLT_EVENT_IST_DOWN,
    AA_SDK_MSG_SMLT_EVENT_SMLT_ENABLE,
    AA_SDK_MSG_SMLT_EVENT_SMLT_DISABLE
};


aasdk_msgque_id_t aasdkx_msgque_create(size_t msgque_size);
void  aasdkx_msgque_destroy(aasdk_msgque_id_t msgque_id);

int   aasdkx_msg_send(aasdk_msgque_id_t msgque_id, aasdk_msg_t *pmsg);
int   aasdkx_msg_recv(aasdk_msgque_id_t msgque_id, aasdk_msg_t *pmsg);

int   aasdki_msg_proc(aasdk_msgque_id_t msgque_id, aasdk_msg_t *pmsg);

#endif /* ndef AASDK_MSGIF_COMM_H */

