/* aa-sdk/common/common/aasdk_osif_comm.h */

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

#ifndef AASDK_OSIF_COMM_H
#define AASDK_OSIF_COMM_H

/* timer */

#include "aasdk_time_comm.h"

/* mutex */

#include "aasdk_osif_plat.h" /* aasdk_mutex_id_t */

int  aasdkx_mutex_create( aasdk_mutex_id_t *pmutex_id);
void aasdkx_mutex_destroy(aasdk_mutex_id_t   mutex_id);

int  aasdkx_mutex_lock(   aasdk_mutex_id_t   mutex_id);
int  aasdkx_mutex_unlock( aasdk_mutex_id_t   mutex_id);


/* memory */

void *aasdkx_mem_alloc(size_t size);
void  aasdkx_mem_free(void *ptr);


#endif /* AASDK_OSIF_COMM_H */

