/* aa-sdk/common/common/asdk_time_comm.h */

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

#ifndef AASDK_TIME_COMM_H
#define AASDK_TIME_COMM_H

#include <time.h> /* struct timespec */

extern const struct timespec ts_0_sec;
extern const struct timespec ts_1_sec;

void aasdku_time_add(        struct timespec *result,
                       const struct timespec *time1,
                       const struct timespec *time0);

void aasdku_time_sub(        struct timespec *result,
                       const struct timespec *time1,
                       const struct timespec *time0);

int  aasdku_time_cmp(  const struct timespec *time1,
                       const struct timespec *time0);

void aasdki_time_tick(       struct timespec *curr);

void aasdku_time_get(        struct timespec *curr);

int  aasdkx_time_sleep(      struct timespec *time);


#endif /* ndef AASDK_TIME_COMM_H */

