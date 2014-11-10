/* aa-sdk/common/common/aasdk_msg_comm.c */

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

#include "aa_agent.h"

#include "aasdk_msgif_comm.h"
#include "aasdk_errno.h"


/* initiate message processing */

int
aasdki_msg_proc(aasdk_msgque_id_t msgque_id, aasdk_msg_t *pmsg)
{
  int aa_rc;

#ifdef AA_SDK_USE_MSG_QUEUE
    /* queue the message to be processed by the faAgent_main() thread */
    aa_rc = aasdkx_msg_send(msgque_id, pmsg)
#else
    /* process the message directly */
    int fa_rc;

    fa_rc = faAgent_main_proc(pmsg);

    aa_rc = (fa_rc == 1) ? AA_SDK_ENONE : AA_SDK_EUNSPEC;
#endif

  return aa_rc;
}

