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

#include <time.h>
#include <stdbool.h>
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_trace_comm.h"
#include "aasdk_time_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_time

static struct timespec aatrans_zero = {0,0};
static struct timespec aatrans_wait = {0,0};

static struct timespec aatrans_prev = {0,0};
static struct timespec aatrans_sum  = {0,0};

void
aatransi_time_get(struct timespec *curr)
{
    *curr = aatrans_prev;
}

void
aatransi_time_interval_set(int interval)
{
    aatrans_wait.tv_sec = interval;
}

void
aatransi_time_tick(struct timespec *curr)
{
    struct timespec delta, old_sum, new_sum;

    aasdku_time_sub(&delta, curr, &aatrans_prev);
    aatrans_prev = *curr;
    old_sum = aatrans_sum;
    aasdku_time_add(&new_sum, &old_sum, &delta);

    if (aasdku_time_cmp(&new_sum, &aatrans_wait) < 0) {
        aatrans_sum = new_sum;
    }
    else {
        aatrans_sum = aatrans_zero;
        aasdk_trace(aa_verbose, "ticking every %d seconds", (int)aatrans_wait.tv_sec);
	aatransi_send_pdu();
    }

}

/* this function must be called once every second */
void
aatransi_time_onesec_ticker(void)
{
  struct timespec cur_time;
  struct timespec new_time;
  struct timespec one_sec = {1,0};

    aatransi_time_get(&cur_time);

    aasdku_time_add(&new_time, &cur_time, &one_sec);

    aatransi_time_tick(&new_time);
}

