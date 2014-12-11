/* aa-sdk/common/common/aasdk_lldpif_comm.c */

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
#include "aasdk_lldpif_comm.h"
#include "aasdk_errno.h"

/*
 * enable and disable
 */
int
aasdki_ports_data_ena(void)
{
    return AA_SDK_ENONE;
}


int
aasdki_ports_data_dis(void)
{
    return AA_SDK_ENONE;
}


int
aasdki_port_data_ena(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}

int
aasdki_port_data_dis(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}


/*
 * authentication
 */
int
aasdki_auth_err(aasdk_port_id_t port_id)
{
    return AA_SDK_ENONE;
}

