/* aasdk_engtest.c */

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

#include <pthread.h>

#include "aasdk_comm.h"
#include "aasdk_time_comm.h"


void *clock_thread(void *arg);
void aatransi_initialize(void);


int
main(void)
{
  int       aa_rc; /* AASDK ret code */
  int       pt_rc; /* pthread ret code */
  pthread_t pt_id; /* pthread ID */

  /* init the SDK */

  printf("aasdk: initializing...\n");

  aa_rc = aasdki_output_set(-1, printf);

  printf("aasdk: aasdki_output_set() = %d\n", aa_rc);

  aatransi_initialize();

  aa_rc = aasdki_global_init();

  printf("aasdk: aasdki_global_init() = %d\n", aa_rc);

  /* start the clock thread */

  pt_rc = pthread_create(&pt_id, NULL, clock_thread, NULL);

  printf("aasdk: pthread_create(clock_thread) = %d\n", pt_rc);
}


void *
clock_thread(void *arg)
{
  struct timespec cur_time;
  struct timespec new_time;
  struct timespec sleep_time;
  struct timespec one_sec = {1,0};

  for (;;)
  {
    sleep_time = one_sec;

    aasdkx_time_sleep(&sleep_time);

    aasdku_time_get(&cur_time);

    aasdku_time_add(&new_time, &cur_time, &one_sec);

    aasdki_time_tick(&new_time);
  }
}

