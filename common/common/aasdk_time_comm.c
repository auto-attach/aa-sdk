/* aa-sdk/common/common/aasdk_time_comm.c */

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

#include <time.h>  /* struct timespec */

#include "aasdk_time_comm.h"
#include "aasdk_msgif_comm.h"

#include "aa_agent.h"


#ifndef NSECS_PER_SEC
#define NSECS_PER_SEC 1000000000
#endif
#ifndef NSECS_PER_MSEC
#define NSECS_PER_MSEC   1000000
#endif
#ifndef NSECS_PER_USEC
#define NSECS_PER_USEC      1000
#endif


const struct timespec ts_1_sec = {1,0};
const struct timespec ts_0_sec = {0,0};


void aasdk_time_msg_send(struct timespec *time);


/* result = time1 + time0 */

void
aasdku_time_add(      struct timespec *result,
                const struct timespec *time1,
                const struct timespec *time0)
{
    struct timespec temp;

    temp.tv_nsec = time1->tv_nsec + time0->tv_nsec;

    if (temp.tv_nsec < NSECS_PER_SEC) {
        result->tv_nsec = temp.tv_nsec;
        temp.tv_sec = 0;
    }
    else {
        result->tv_nsec = temp.tv_nsec - NSECS_PER_SEC;
        temp.tv_sec = 1;
    }

    result->tv_sec = time1->tv_sec + time0->tv_sec + temp.tv_sec;
}



/* result = time1 - time 0*/

void
aasdku_time_sub(      struct timespec *result,
                const struct timespec *time1,
                const struct timespec *time0)
{
    struct timespec temp;

    temp.tv_nsec = time1->tv_nsec - time0->tv_nsec;

    if (temp.tv_nsec >= 0) {
        result->tv_nsec = temp.tv_nsec;
        temp.tv_sec = 0;
    }
    else {
        result->tv_nsec = temp.tv_nsec + NSECS_PER_SEC;
        temp.tv_sec = 1;
    } 

    result->tv_sec = time1->tv_sec - time0->tv_sec - temp.tv_sec;
}



/* +1: time1 > time0
 *  0: time1 = time0
 * -1: time1 < time0
 */

int
aasdku_time_cmp(const struct timespec *time1,
                const struct timespec *time0)
{
    int rv;

    if      (time1->tv_sec  > time0->tv_sec)
        rv = +1;
    else if (time1->tv_sec  < time0->tv_sec)
        rv = -1;
    else if (time1->tv_nsec > time0->tv_nsec)
        rv = +1;
    else if (time1->tv_nsec < time0->tv_nsec)
        rv = -1;
    else
        rv = 0;

    return rv;
}

static struct timespec zero = {0,0};
static struct timespec wait = {1,0};

static struct timespec prev = {0,0};
static struct timespec sum  = {0,0};


void
aasdku_time_get(struct timespec *curr)
{
    *curr = prev;
}


void
aasdki_time_tick(struct timespec *curr)
{
    struct timespec delta, old_sum, new_sum;

    aasdku_time_sub(&delta, curr, &prev);
    prev = *curr;
    old_sum = sum;
    aasdku_time_add(&new_sum, &old_sum, &delta);

    if (aasdku_time_cmp(&new_sum, &wait) < 0) {
        sum = new_sum;
    }
    else {
        sum = zero;
	aasdk_time_msg_send(curr);
    }
}


void
aasdk_time_msg_send(struct timespec *time)
{
    aasdk_msg_t  msg;

    msg.msgId = AA_SDK_MSG_LIST_MAINT_TIMER;
    msg.arg0  = time->tv_sec;
    msg.arg1  = time->tv_nsec;

    aasdki_msg_proc(faAgentMsgQ, &msg);
}

