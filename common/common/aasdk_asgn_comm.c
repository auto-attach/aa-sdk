/* aa-sdk/common/common/aasdk_asgn_comm.c */
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

#include <stdint.h>

#include "aa_agent.h"
#include "aa_agent_private.h"

#include "aasdk_errno.h"
#include "aasdk_comm.h"
#include "aasdk_lldpif_comm.h"
#include "aasdk_mgmtif_comm.h"



int
aasdki_asgn_data_set(aasdk_port_id_t port_id, int status,
                     uint32_t isid, uint16_t vlan)
{
    aasdk_msg_t msg;
    uint16_t    mgmt_vlan;
    uint32_t    elem_type;

    int         aa_rc;

    /* format the assignment message */

    msg.msgId = AA_SDK_MSG_LLDP_BINDING_UPDATE;

    faAgentGetMgmtVlan(&mgmt_vlan);
    elem_type = faAgentGetElementType();

    msg.arg0  = 0;
    msg.arg0 |= ((uint32_t)port_id   & 0x0000ffff) <<  0;
    msg.arg0 |= ((uint32_t)mgmt_vlan & 0x00000fff) << 16;
    msg.arg0 |= ((uint32_t)elem_type & 0x0000000f) << 28;

    msg.arg1 = isid;
    msg.arg2 = vlan;
    msg.arg3 = AA_SDK_ISID_VLAN_ORIG_REMOTE_CLIENT;
    msg.arg4 = status;


    /* process the message */

    aasdkx_mutex_lock(faAgentMut);

    aaPortStatInc(port_id,offsetof(aasdk_stats_data_t, aasdk_asgn_received));

    aa_rc = aasdki_msg_proc(faAgentMsgQ, &msg);

    aasdkx_mutex_unlock(faAgentMut);

    return aa_rc;
}



int
aasdki_asgn_status_get(aasdk_port_id_t port_id,
                       uint32_t isid, uint16_t vlan,
                       aasdk_asgn_status_t *p_asgn_sts)
{
    faRemoteIsidVlanAsgnsEntry_t *p_rem_asgn;
    int rc;

    if (p_asgn_sts == NULL) {
        return AA_SDK_EINVAL;
    }

    aasdkx_mutex_lock(faAgentMut);

    p_rem_asgn = faRemoteIsidVlanAsgnFind(port_id,isid,vlan,FA_AGENT_GET_EXACT);

    if (p_rem_asgn == NULL) {
        rc = AA_SDK_EADDRNOTAVAIL;
    }
    else  {
        p_asgn_sts->port_id = p_rem_asgn->ifIndex;
        p_asgn_sts->isid    = p_rem_asgn->isid;
        p_asgn_sts->vlan    = p_rem_asgn->vlan;
        p_asgn_sts->status  = p_rem_asgn->status;
        p_asgn_sts->origin  =(p_rem_asgn->elemType ==
                              FA_AGENT_ELEMENT_HOST) ?
                              AA_SDK_ISID_VLAN_ORIG_REMOTE_PROXY :
                              AA_SDK_ISID_VLAN_ORIG_REMOTE_CLIENT;
        rc = AA_SDK_ENONE;
    }

    aasdkx_mutex_unlock(faAgentMut);

    return rc;
}



int
aasdki_port_asgns_status_get(aasdk_port_id_t port_id,
                             size_t  max_num_asgns,
                             size_t *act_num_asgns,
                             aasdk_asgn_status_t *p_asgns_sts)
{
    aasdk_asgn_status_t *p_asgn_sts;
    unsigned             num_asgns_sts = 0;

    faRemoteIsidVlanAsgnsEntry_t *p_rem_asgn = NULL;
    faRemoteIsidVlanAsgnsEntry_t *p_rem_asgns = NULL;
    unsigned                     num_rem_asgns = 0;
 
    int fa_rc;


    /* validate input parameters */

    if (p_asgns_sts == NULL) {
        return AA_SDK_EINVAL;
    }

    if (act_num_asgns == NULL) {
        return AA_SDK_EINVAL;
    }

    if (max_num_asgns == 0) {
        return AA_SDK_EINVAL;
    }


    /* try to allocate a buffer for the max number of rem asgns */

    p_rem_asgns = aasdkx_mem_alloc(FA_AGENT_MAX_ISID_VLAN_ASGNS *
                                   sizeof(*p_rem_asgns));
    if (p_rem_asgns == NULL) {
        return AA_SDK_ENOMEM;
    }


    /* get the remote assignmets for the specified port */
                                  
    aasdkx_mutex_lock(faAgentMut);

    fa_rc = faRemoteIsidVlanAsgnPortFind(port_id,
                                         p_rem_asgns,
					 &num_rem_asgns);
    aasdkx_mutex_unlock(faAgentMut);


    /* copy from the private assignment to the public status */

    if (fa_rc) {
        p_asgn_sts = p_asgns_sts;
        num_asgns_sts = 0;
        p_rem_asgn = p_rem_asgns;
        while ( (num_asgns_sts < num_rem_asgns) &&
                (num_asgns_sts < max_num_asgns))
        {
            p_asgn_sts->port_id = p_rem_asgn->ifIndex;
            p_asgn_sts->isid    = p_rem_asgn->isid;
            p_asgn_sts->vlan    = p_rem_asgn->vlan;
            p_asgn_sts->status  = p_rem_asgn->status;
            p_asgn_sts->origin  =(p_rem_asgn->elemType ==
                                  FA_AGENT_ELEMENT_HOST) ?
                                  AA_SDK_ISID_VLAN_ORIG_REMOTE_PROXY :
                                  AA_SDK_ISID_VLAN_ORIG_REMOTE_CLIENT;
            p_asgn_sts++;
            num_asgns_sts++;
            p_rem_asgn++;
        }
    }

    *act_num_asgns = num_asgns_sts;

    aasdkx_mem_free(p_rem_asgns);

    return AA_SDK_ENONE;
}
