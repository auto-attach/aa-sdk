/* aa-sdk/common/common/aasdk_comm.h */
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


#ifndef AASDK_COMM_H
#define AASDK_COMM_H

#include <stdint.h>

/* opaque port identifier */
typedef uint32_t aasdk_port_id_t;

/* AA SDK init */
int aasdki_global_init(void);

/* AA SDK enable and disable */
int aasdki_global_ena(void);
int aasdki_global_dis(void);

/* AA SDK status output */
#define AASDK_OUTPUT_FLAG_CUST_MSG   1
#define AASDK_OUTPUT_FLAG_ENG_MSG    2
#define AASDK_OUTPUT_FLAG_INFO_1_MSG 3
#define AASDK_OUTPUT_FLAG_INFO_2_MSG 4
#define AASDK_OUTPUT_FLAG_INFO_3_MSG 5
#define AASDK_OUTPUT_FLAG_INFO_4_MSG 6
#define AASDK_OUTPUT_FLAG_INFO_5_MSG 6
  

typedef int (*aasdk_output_func_t)(const char *, ...);

int aasdki_output_set(unsigned int         output_flags,
                      aasdk_output_func_t  output_func);

extern unsigned int        aasdk_output_flags;
extern aasdk_output_func_t aasdk_output_func;

int aasdk_output(char *output_string, unsigned int output_flag_num);

#endif /* ndef AASDK_COMM_H */

