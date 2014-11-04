/* aa-sdk/common/stubs/aasdk_comm.c */

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

#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aasdk_mgmtif_comm.h"

int
aasdki_global_init(void)
{
    return AA_SDK_ENONE;
}

int
aasdki_global_ena(void)
{
    return AA_SDK_ENONE;
}

int
aasdki_global_dis(void)
{
    return AA_SDK_ENONE;
}


int
aasdki_glbl_stats_get(aasdk_stats_data_t *p_glbl_stats)
{
    return AA_SDK_ENONE;
}


int
aasdki_glbl_stats_clr(void)
{
    return AA_SDK_ENONE;
}


/*
 * error and status output
 */
int
aasdki_output_set(unsigned int        output_flags,
                  aasdk_output_func_t output_func)
{
    return AA_SDK_ENONE;
}

int
aasdk_output(char * output_string, unsigned int output_flag_num)
{
     return 0;
}

const char *
aasdku_errno_string(int errno)
{
    return NULL;
}
