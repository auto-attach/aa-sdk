/* aa-sdk/platform/posix/aasdk_msgif_plat.c */

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

#include <fcntl.h>    /* O_*    */
#include <sys/stat.h> /* mode_t */
#include <mqueue.h>

#include "aasdk_msgif_plat.h"
#include "aasdk_msgif_comm.h"
#include "aasdk_errno.h"


const char aasdk_mq_name[] = "/aasdk_mq";


aasdk_msgque_id_t
aasdkx_msgque_create(size_t msgque_size)
{
    int            mq_oflags;
    struct mq_attr mq_attrs;
    mode_t         mq_mode;
    mqd_t          mqd;

    mq_oflags = O_CREAT |
                O_EXCL  |
                O_RDWR  |
                O_NONBLOCK;

    mq_mode = S_IRWXU |
              S_IRWXG |
              S_IRWXO;

    mq_attrs.mq_maxmsg = msgque_size;
    mq_attrs.mq_msgsize = (long) sizeof(aasdk_msg_t);

    mqd = mq_open(aasdk_mq_name, mq_oflags, mq_mode, &mq_attrs);

    return mqd;
}


void
aasdkx_msgque_destroy(aasdk_msgque_id_t msgque_id)
{
  mq_close(msgque_id);

  mq_unlink(aasdk_mq_name);
}



int
aasdkx_msg_send(aasdk_msgque_id_t msgque_id,
                aasdk_msg_t     *pmsg)
{
    int mqsts;
    int ret_val;

    mqsts = mq_send(msgque_id, (const char *)pmsg, sizeof(aasdk_msg_t), 0);

    if (mqsts == 0) {
        ret_val = AA_SDK_ENONE;
    }
    else {
        ret_val = AA_SDK_EUNSPEC;
    }
    return ret_val;
}


int
aasdkx_msg_recv(aasdk_msgque_id_t msgque_id,
                aasdk_msg_t     *pmsg)
{
    ssize_t      msg_size;
    unsigned int msg_prio;
    int          ret_val;

    msg_size = mq_receive(msgque_id,(char *)pmsg,sizeof(aasdk_msg_t),&msg_prio);

    if (msg_size == sizeof(aasdk_msg_t)) {
        ret_val = AA_SDK_ENONE;
    }
    else {
        ret_val = AA_SDK_EUNSPEC;
    };
    return ret_val;
}

