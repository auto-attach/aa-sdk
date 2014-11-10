/* aa-sdk/common/common/aasdk_elem_comm.c */

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

#include <string.h> /* mem*() */

#include "aasdk_mgmtif_comm.h"
#include "aasdk_errno.h"

#include "aa_defs.h"
#include "aa_agent.h"
#include "aa_agent_private.h"


int
aasdki_elem_type_cfg(int elem_type)
{
    int fa_rc;
    int aa_rc = AA_SDK_EINVAL;

    switch( elem_type )
    {
      case AA_SDK_ELEM_SERVER:
	aa_rc = AA_SDK_ENONE;
	break;
      default:
        aa_rc = AA_SDK_EINVAL;
	break;
    }

    if (aa_rc != AA_SDK_ENONE)
    {
        return aa_rc;
    }

    aasdkx_mutex_lock(faAgentMut);

    fa_rc = faAgentSetElementType(elem_type,
                                  FA_NO_NV_SAVE,
                                  FA_NO_DISTRIBUTE,
                                  FA_NO_RESET);

    aasdkx_mutex_unlock(faAgentMut);

    aa_rc = (fa_rc == SUCCESS) ? AA_SDK_ENONE : AA_SDK_EUNSPEC;

    return aa_rc;
}



int
aasdki_elem_mgmt_vlan_cfg(uint16_t mgmt_vlan)
{
    aasdkx_mutex_lock(faAgentMut);

    faAgentConfigData.faMgmtVlan = mgmt_vlan;

    faAgentSetMgmtVlan(mgmt_vlan);

    aasdkx_mutex_unlock(faAgentMut);

    return AA_SDK_ENONE;
}



/* per-agent sys ID deprecated
 * now per-port sys ID
 * aasdki_port_sys_id_cfg(port, sysid) */

int
aasdki_elem_sys_id_cfg(uint8_t *sys_id)
{
    return AA_SDK_ENOTSUP;
}


int
aasdki_glbl_rem_asgn_exp_cfg(int rem_asgn_exp)
{
    aasdkx_mutex_lock(faAgentMut);

    faAgentRemoteIsidVlanAsgnExpiration = rem_asgn_exp;

    aasdkx_mutex_unlock(faAgentMut);

    return AA_SDK_ENONE;
}



int
aasdki_glbl_status_get(aasdk_glbl_status_t *p_glbl_sts)
{
    aasdkx_mutex_lock(faAgentMut);

    p_glbl_sts->enabled = faAgentServiceEnabled() ? true : false;

    p_glbl_sts->elem_type = faAgentGetElementType();

    faAgentGetMgmtVlan(&p_glbl_sts->mgmt_vlan);

    p_glbl_sts->rem_asgn_exp = faAgentRemoteIsidVlanAsgnExpiration;

    aasdkx_mutex_unlock(faAgentMut);

    return AA_SDK_ENONE;
}



int
aasdki_disc_data_set(aasdk_port_id_t port_id,
                     int elem_type, uint16_t mgmt_vlan, uint8_t *sys_id)
{
    aasdk_msg_t msg;
    int rc;

    msg.msgId = AA_SDK_LLDP_DISCOVERY_UPDATE;

    msg.arg0  = 0;
    msg.arg0 |= ((uint32_t)port_id   & 0x0000ffff) <<  0;
    msg.arg0 |= ((uint32_t)mgmt_vlan & 0x00000fff) << 16;
    msg.arg0 |= ((uint32_t)elem_type & 0x0000000f) << 28;

    memcpy(&msg.arg1, &sys_id[0], sizeof(uint32_t));
    memcpy(&msg.arg2, &sys_id[4], sizeof(uint32_t));
    memcpy(&msg.arg3, &sys_id[8], sizeof(uint16_t));

    rc = aasdki_msg_proc(faAgentMsgQ, &msg);

    aasdkx_mutex_lock(faAgentMut);

    aaPortStatInc(port_id, offsetof(aasdk_stats_data_t, aasdk_disc_received));

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}


