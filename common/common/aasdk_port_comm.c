/* aa-sdk/common/common/aasdk_port_comm.c */

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

#include <stdbool.h>

#include "aasdk_mgmtif_comm.h"
#include "aasdk_osif_comm.h"

#include "aa_agent.h"
#include "aa_port.h"


int
aasdki_port_create(aasdk_port_id_t port_id)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortCreate(port_id);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}



int
aasdki_port_destroy(aasdk_port_id_t port_id)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortDestroy(port_id);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}



int
aasdki_port_status_get(aasdk_port_id_t      port_id,
                       aasdk_port_status_t *port_status)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortStatusGet(port_id, port_status);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}


int
aasdki_port_stats_get(aasdk_port_id_t     port_id,
                      aasdk_stats_data_t *port_stats)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortStatsGet(port_id, port_stats);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}



int
aasdki_port_stats_clr(aasdk_port_id_t     port_id)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortStatsClr(port_id);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}


int
aasdki_port_aa_ena(aasdk_port_id_t port_id)
{

    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortStateSet(port_id, true);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;

}


int
aasdki_port_aa_dis(aasdk_port_id_t port_id)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortStateSet(port_id, false);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}


int
aasdki_port_sys_id_cfg(aasdk_port_id_t port_id,
                       uint8_t        *sys_id)
{
    int rc;

    aasdkx_mutex_lock(faAgentMut);

    rc = aaPortSysIdCfg(port_id, sys_id);

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}


