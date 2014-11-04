/* aa-sdk/common/stubs/aasdk_time_comm.c */

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

#include "time.h"  /* struct timespec */

#include "aasdk_time_comm.h"


const struct timespec ts_0_sec = {0,0};
const struct timespec ts_1_sec = {1,0};


void
aasdku_time_add(      struct timespec *result,
                const struct timespec *time1,
                const struct timespec *time0)
{
    result->tv_sec  = 0;
    result->tv_nsec = 0;
}


void
aasdku_time_sub(      struct timespec *result,
                const struct timespec *time1,
                const struct timespec *time0)
{
    result->tv_sec  = 0;
    result->tv_nsec = 0;
}


int
aasdku_time_cmp(const struct timespec *time1,
                const struct timespec *time0)
{
    return 0;
}



void
aasdki_time_tick(struct timespec *curr)
{
    return;
}


void
aasdki_time_get(struct timespec *curr)
{
    curr->tv_sec  = 0;
    curr->tv_nsec = 0;

    return;
}


