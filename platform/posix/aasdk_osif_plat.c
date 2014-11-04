/* aa-sdk/platform/posix/aasdk_osif_plat.c */

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

#include "stdlib.h"
#include "pthread.h"

#include "aasdk_osif_plat.h"
#include "aasdk_osif_comm.h"
#include "aasdk_errno.h"


/*
 * mutexes
 */

int
aasdkx_mutex_create(aasdk_mutex_id_t *pmutex_id)
{
    pthread_mutex_t   *pmutex;
    pthread_mutexattr_t mutexattr_recursive;
    int rc;

    pmutex = aasdkx_mem_alloc(sizeof(pthread_mutex_t));
    if (pmutex != NULL) {
       *pmutex_id = pmutex;
        pthread_mutexattr_init(&mutexattr_recursive);
        pthread_mutexattr_settype(&mutexattr_recursive,PTHREAD_MUTEX_RECURSIVE);
        rc = pthread_mutex_init(pmutex, &mutexattr_recursive);
    }
    else {
        rc = AA_SDK_ENOMEM;
    }
    return rc;
};


void
aasdkx_mutex_destroy(aasdk_mutex_id_t mutex_id)
{
  pthread_mutex_destroy(mutex_id);

  aasdkx_mem_free( mutex_id );
}


int
aasdkx_mutex_lock(aasdk_mutex_id_t mutex_id)
{
    int rc;

    rc = pthread_mutex_lock(mutex_id);

    return rc;
}


int
aasdkx_mutex_unlock(aasdk_mutex_id_t mutex_id)
{
    int rc;

    rc = pthread_mutex_unlock(mutex_id);

    return rc;
}


/*
 * memory
 */
void *
aasdkx_mem_alloc(size_t size)
{
    return malloc(size);
}

void
aasdkx_mem_free(void *ptr)
{
    free(ptr);
}

