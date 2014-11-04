/* aa-sdk/common/common/aasdk_errno.h */

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

#ifndef AASDK_ERRNO_H
#define AASDK_ERRNO_H

typedef enum {
    AA_SDK_EUNSPEC       = -1,
    AA_SDK_ENONE         =  0,
    AA_SDK_EIO           =  5,
    AA_SDK_EAGAIN        =  11,
    AA_SDK_ENOMEM        =  12,
    AA_SDK_EFAULT        =  14,
    AA_SDK_EBUSY         =  16,
    AA_SDK_ENODEV        =  19,
    AA_SDK_EINVAL        =  22,
    AA_SDK_ELOOP         =  40,
    AA_SDK_ETIME         =  62,
    AA_SDK_EADDRINUSE    =  98,
    AA_SDK_EADDRNOTAVAIL =  99,
    AA_SDK_ENOTSUP       = 252
} aasdk_errno_t;

const char *aasdku_errno_string(int errno);

#endif /* AASDK_ERRNO_H */

