/* aa-sdk/common/agent/aa_types.h */

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

#ifndef AA_TYPES_H
#define AA_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t   INT8;
typedef int16_t  INT16;
typedef int32_t  INT32;

typedef unsigned UINT;
typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

typedef uint16_t WORD;
typedef uint32_t LONG;

typedef int32_t  GT_Int32;
typedef uint32_t GT_Uint32;
typedef int      GT_Status;

typedef _Bool BOOL;
typedef _Bool boolean;
typedef _Bool BOOLEAN;

typedef uint8_t *OctetString;

typedef void *task_spawn_t;

#endif /* ndef AA_TYPES_H */

