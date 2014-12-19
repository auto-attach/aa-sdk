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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "aasdk_comm.h"
#include "aatrans_trace_comm.h"

bool aasdk_trace_enabled = false;

/* keep this table in-sync with aasdk_trc_mod_t */
aasdk_trc_mod_t aasdk_trc_mod_list[aasdk_mod_max] = {
    {aasdk_mod_none,            "none",             aa_none},
    {aasdk_mod_all,             "all",              aa_none},
    {aasdk_mod_aatrans_common,  "aatrans_common",   aa_none},
    {aasdk_mod_aatrans_port,    "aatrans_port",     aa_none},
    {aasdk_mod_aatrans_time,    "aatrans_time",     aa_none},
    {aasdk_mod_aatrans_packet,  "aatrans_packet",   aa_none},
    {aasdk_mod_aatrans_elem,    "aatrans_elem",     aa_none},
    {aasdk_mod_aatrans_auth,    "aatrans_auth",     aa_none},
    {aasdk_mod_lldp_debug,      "lldp_debug",       aa_none},
};

void aasdku_report(aasdk_trc_mod_id_t mod_id, const char *file, uint32_t line, 
                   const char *function, const char *format, ...) {

    va_list args;
    char tempBuf[AA_TRACE_MAX_LINE_LENGTH];
    char buf[AA_TRACE_MAX_LINE_LENGTH];
    char *pBuf = buf;
    int prefix_len = 0;

    if(format == NULL) {
        return;
    }

    snprintf(tempBuf, sizeof(tempBuf), "[%s:%d]", function, line);
    prefix_len = snprintf(pBuf, sizeof(buf), "%s ", tempBuf);
    pBuf += prefix_len;

    va_start(args, format);
    vsnprintf(pBuf, AA_TRACE_MAX_LINE_LENGTH-1-prefix_len, format, args); /* this excludes '\0' */
    va_end(args);

    if(aasdk_output_func) {
        (*aasdk_output_func)(buf);
        (*aasdk_output_func)("\n");
    }
}

