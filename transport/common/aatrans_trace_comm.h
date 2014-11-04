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

#ifndef AATRANS_TRACE_PLAT_H
#define AATRANS_TRACE_PLAT_H

#include <stdarg.h>
#include <stdbool.h>

typedef enum
{
    aa_none          = 0x0,
    aa_terse         = 0x1,
    aa_verbose       = 0x2,
    aa_levels        = 0x3
} aasdk_trace_level_t;

typedef enum {
    aasdk_mod_none=0,           /* No modules (no tracing) */
    aasdk_mod_all,              /* All modules (max tracing) */
    aasdk_mod_aatrans_common,
    aasdk_mod_aatrans_port,
    aasdk_mod_aatrans_time,
    aasdk_mod_aatrans_packet,
    aasdk_mod_aatrans_elem,
    aasdk_mod_aatrans_auth,
    aasdk_mod_max               /* Last entry -- Do not add entries after this line! */
} aasdk_trc_mod_id_t;

typedef struct {
    aasdk_trc_mod_id_t trc_mod_id;
    char trc_mod_name[128];
    aasdk_trace_level_t trc_level;
} aasdk_trc_mod_t;

extern bool aasdk_trace_enabled;
extern aasdk_trc_mod_t aasdk_trc_mod_list[];
void aasdku_report(aasdk_trc_mod_id_t mod_id, const char *file, uint32_t line, 
                   const char *function, const char *format, ...);

#define aasdk_trace(TRClevel, args...) do { \
    if (aasdk_trace_enabled && \
        ((aasdk_trc_mod_list[AASDK_TRC_MOD_ID].trc_level >= (TRClevel)) || \
         (aasdk_trc_mod_list[aasdk_mod_all].trc_level >= (TRClevel)))) \
    { \
        aasdku_report(AASDK_TRC_MOD_ID, __FILE__, __LINE__, __FUNCTION__, ##args); \
    } \
} while (0)

#define aasdk_log(args...) do { \
    aasdku_report(AASDK_TRC_MOD_ID, __FILE__, __LINE__, __FUNCTION__, ##args); \
} while (0)

void aasdkx_puts(const char *strbuf);

#define AA_TRACE_MAX_LINE_LENGTH 1024          // Max 1024 bytes per trace line

#endif /* AATRANS_TRACE_PLAT_H */
