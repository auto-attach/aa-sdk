/* aa-sdk/platform/stubs/aasdk_msgif_plat.c */

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

#include "aasdk_msgif_comm.h"
#include "aasdk_errno.h"


aasdk_msgque_id_t
aasdkx_msgque_create(size_t msgque_size)
{
    return 0;
}


void
aasdkx_msgque_destroy(aasdk_msgque_id_t msgque_id)
{
    return;
}


int
aasdkx_msg_send(aasdk_msgque_id_t msgque_id,
                aasdk_msg_t     *pmsg)
{
    return AA_SDK_ENONE;
}


int
aasdkx_msg_recv(aasdk_msgque_id_t msgque_id,
                aasdk_msg_t     *pmsg)
{
    return AA_SDK_EAGAIN;
}

