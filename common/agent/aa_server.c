/* aa-sdk/common/agent/aa_server.c */

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
#include <stdio.h>  /* sprintf() */

#include "aa_types.h"
#include "aa_defs.h"
#include "aa_agent.h"
#include "aa_agent_private.h"
#include "aa_sysdefs.h"

#include "aasdk_errno.h"
#include "aasdk_osif_comm.h"
#include "aasdk_lldpif_comm.h"

/* Workspace */
static faRemoteIsidVlanAsgnsEntry_t faRemIsidVlanAsgnList[FA_AGENT_REM_MAX_ISID_VLAN_ASGNS];

/*** External declarations ***/

extern int
faRemoteIsidVlanAsgnVlanSource (unsigned int vlan);

extern int
faRemoteIsidVlanAsgnVlanActive (unsigned int vlan, unsigned int excludedIfc);

extern int
faActiveUntaggedClient (unsigned int ifIndex);

extern int
faActiveUntaggedClientPvid (unsigned int ifIndex);


/*** Forward declarations ***/

/**************************************************************
 * Name: faDownlinkAutoAttachProcessing
 * Description:
 *    FA downlink Auto Attach processing state machine support. 
 * Input Parameters:
 *    elemVlan - identifies the downlink VLAN for the update
 *    elemType - identifies the downlink element type
 *    ifIndex - identifies the host/client downlink ifIndex
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Auto Attach processing failure
 *    1 - Auto Attach processing success
 **************************************************************/

int
faDownlinkAutoAttachProcessing (unsigned int elemVlan,
                                unsigned int elemType,
                                unsigned int ifIndex)
{
    int rc = 0;

    rc = 1;

faDownlinkAutoAttachProcessingRet:

    return (rc);
}

/**************************************************************
 * Name: faAgentPerformRemoteListMaintenance
 * Description:
 *    Handles list maintenance activities for the remote
 *    I-SID/VLAN assignment list.
 * Input Parameters:
 *    expiration - time in seconds to be used for assignment
 *                 expiration determination.
 *    tgtIfc - target interface for maintenance operation
 *             (0 = ignore interface data)
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faAgentPerformRemoteListMaintenance (unsigned int expiration,
                                     unsigned int tgtIfc)
{
    int asgnIssues = 0;
    int saveToNv = 0;
    int i, elemCount = 0;
    int peerMsgCount = 0;
    unsigned int currentTime;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_5,
                    "faAgentPerformRemoteListMaintenance", expiration, tgtIfc, 0, NULL);

    if ((faAgentConfigData.faRemoteIsidVlanAsgnsCount == 0) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL) ||
        (faAgentConfigData.faAgentState & FA_AGENT_STATE_PROCESSING_BLOCKED))
    {
        /* Nothing to do */
        return;
    }

    aasdkx_mutex_lock(faAgentMut);

    currentTime = faAgentTimeGet();

    faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
    while (faIsidVlanAsgn != NULL)
    {
        /* Internal client assignment are not aged */
        if (FA_ELEM_INT_CLIENT(faIsidVlanAsgn->elemType))
        {
            /* Skip entry */
            faIsidVlanAsgn = faIsidVlanAsgn->next;
            continue;
        }

        /* If this an interface-specific maintenance request, confirm */
        /* that the element is associated with the target interface. */
        /* If not, skip it */
        if ((tgtIfc) && (tgtIfc != faIsidVlanAsgn->ifIndex))
        {
            /* Skip entry */
            faIsidVlanAsgn = faIsidVlanAsgn->next;
            continue;
        }

        /* Check the lastUpdated value for all entries and initiate */
        /* I-SID/VLAN assignment expiration tasks. Handle clock wrap */
        /* (lastUpdated > currentTime) as well. Differentiate between */
        /* entries representing actual versus virtual interfaces */
        if (((faIsidVlanAsgn->lastUpdated + expiration) <= currentTime) ||
            (faIsidVlanAsgn->lastUpdated > currentTime))
        {

#ifdef SYSDEF_INCLUDE_FA_SMLT









#endif /* SYSDEF_INCLUDE_FA_SMLT */

            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_EXPIRE, FA_AGENT_INFO_MSG_3,
                            NULL,
                            faIsidVlanAsgn->ifIndex,
                            faIsidVlanAsgn->isid, 
                            faIsidVlanAsgn->vlan,
                            NULL);

            aaPortStatInc(faIsidVlanAsgn->ifIndex,
                          offsetof(aasdk_stats_data_t, aasdk_asgn_expired));

            /* Store elements for later processing */
            if (elemCount < FA_AGENT_REM_MAX_ISID_VLAN_ASGNS)
            {
                memcpy((char *)&faRemIsidVlanAsgnList[elemCount],
                       (char *)faIsidVlanAsgn,
                       sizeof(faRemoteIsidVlanAsgnsEntry_t));

#ifdef SYSDEF_INCLUDE_FA_SMLT






















#endif /* SYSDEF_INCLUDE_FA_SMLT */

                elemCount++;
            }

            /* If active entries are being aged, need to update NV */
            if ((faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE) &&
                (faIsidVlanAsgn->components != FA_COMP_NONE))
            {
                saveToNv = 1;
            }

            /* Initiate settings cleanup - deactivate assignments */
            if (! faServerAsgnRejectionProcessing(faIsidVlanAsgn,
                                                  FA_AGENT_ISID_VLAN_ASGN_REJECTED))
            {
                asgnIssues++;
            }
        }

        faIsidVlanAsgn = faIsidVlanAsgn->next;
    }

    aasdkx_mutex_unlock(faAgentMut);

    /* Delete FA database and FA LLDP database assignments */
    for (i = 0; i < elemCount; i++)
    {
        faRemoteIsidVlanAsgnDelete(&faRemIsidVlanAsgnList[i], FA_NOTIFY_LLDP);
    }

#ifdef SYSDEF_INCLUDE_FA_SMLT









#endif /* SYSDEF_INCLUDE_FA_SMLT */

    if (asgnIssues)
    {
        faAgentErrorMsg(FA_AGENT_ERR_ASGN_EXPIRE_FAIL, FA_AGENT_INFO_MSG_1,
                        NULL, expiration, 0, 0, NULL);

        if (expiration == FA_REMOTE_ISID_VLAN_ASGN_IMMEDIATE_DEL)
            faAgentStatsData.faAgentServerImmedDeleteIssues += asgnIssues;
        else
            faAgentStatsData.faAgentServerAsgnExpireIssues += asgnIssues;
    }

    if (expiration != FA_REMOTE_ISID_VLAN_ASGN_IMMEDIATE_DEL)
    {
        faAgentStatsData.faAgentIsidVlanAsgnExpirations += elemCount;
    }

    return;
}

/**************************************************************
 * Name: faAgentPerformRemoteListHostRemoval
 * Description:
 *    Handles list maintenance activities for the remote
 *    I-SID/VLAN assignment list related to a host downlink
 *    removal/host service termination. Host entries are
 *    identified by unit and port (port = 0 causes all
 *    entries for the unit to be processed).
 * Input Parameters:
 *    unit - unit associated with target host
 *    port - port associated with target host (1-based)
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faAgentPerformRemoteListHostRemoval (unsigned int unit, unsigned int port)
{
    int asgnIssues = 0;
    int saveToNv = 0;
    int elemCount = 0;
    unsigned int tmpIfIndex = 0;
    unsigned int elemUnit;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;
    faRemoteIsidVlanAsgnsEntry_t *tmpFaIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faAgentPerformRemoteListHostRemoval", 0, 0, 0, NULL);

    if ((faAgentConfigData.faRemoteIsidVlanAsgnsCount == 0) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL) ||
        (unit == 0))
    {
        /* Nothing to do */
        return;
    }

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
    while (faIsidVlanAsgn != NULL)
    {
        /* Search for entries associated with the target unit and */
        /* port (if non-zero) and initiate list removal actions */

        if (port == 0)
        {
            tmpFaIsidVlanAsgn = faIsidVlanAsgn->next;

            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_HOST_REMOVE, FA_AGENT_INFO_MSG_3,
                            NULL,
                            faIsidVlanAsgn->ifIndex,
                            faIsidVlanAsgn->isid, 
                            faIsidVlanAsgn->vlan,
                            NULL);

            /* If removing elements for an entire unit, the cause is */
            /* the unit leaving the stack. In this case, the interface */
            /* should be ignored for component cleanup purposes */

            if (port == 0)
            {
                tmpIfIndex = faIsidVlanAsgn->ifIndex;
                faIsidVlanAsgn->ifIndex = 0;
            }

            /* Store elements for later processing */
            if (elemCount < FA_AGENT_REM_MAX_ISID_VLAN_ASGNS)
            {
                memcpy((char *)&faRemIsidVlanAsgnList[elemCount],
                       (char *)faIsidVlanAsgn,
                       sizeof(faRemoteIsidVlanAsgnsEntry_t));

                /* Save unit/port data using available fields */
                faRemIsidVlanAsgnList[elemCount].next = (void *)unit;
                faRemIsidVlanAsgnList[elemCount].prev = (void *)port;

                elemCount++;
            }

            if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
            {
                saveToNv = 1;
            }

            if (port == 0)
            {
                /* Restore original interface data for next cleanup stage */
                faIsidVlanAsgn->ifIndex = tmpIfIndex;
            }

            faRemoteIsidVlanAsgnDelete(faIsidVlanAsgn, FA_NOTIFY_LLDP);

            faIsidVlanAsgn = tmpFaIsidVlanAsgn;
        }
        else
        {
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

#ifdef NOT_NEEDED

    /* Initiate settings cleanup */
    for (i = 0; i < elemCount; i++)
    {
        /* Deactivate assignment settings */
        if (! faServerAsgnRejectionExternalAppAccess(&faRemIsidVlanAsgnList[i], 
                                                     (unsigned int)faRemIsidVlanAsgnList[i].next, 
                                                     (unsigned int)faRemIsidVlanAsgnList[i].prev))
        {
            asgnIssues++;
        }
    }

#endif /* NOT_NEEDED */

    if (asgnIssues)
    {
        faAgentErrorMsg(FA_AGENT_ERR_HOST_REMOVE_FAIL, FA_AGENT_INFO_MSG_1,
                        NULL, unit, port, 0, NULL);

        faAgentStatsData.faAgentServerCleanupIssues += asgnIssues;
    }

    return;
}

/**************************************************************
 * Name: faAgentSyncRemoteTrunkMembers
 * Description:
 *    Searches for and deletes I-SID/VLAN bindings that
 *    represent mirrored data on a different trunk port.
 * Input Parameters:
 *    tgtFaIsidVlanAsgn - target remote I-SID/VLAN binding
 *                        used to find other trunk members
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faAgentSyncRemoteTrunkMembers (faRemoteIsidVlanAsgnsEntry_t *tgtFaIsidVlanAsgn)
{
    int saveToNv = 0;
    int i, elemCount = 0;
    faRemoteIsidVlanAsgnsEntry_t *tmpFaIsidVlanAsgn;

    if ((tgtFaIsidVlanAsgn == NULL) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsCount == 0) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL))
    {
        /* Nothing to do */
        return;
    }

    aasdkx_mutex_lock(faAgentMut);

    tmpFaIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
    while (tmpFaIsidVlanAsgn != NULL)
    {
        /* Internal client assignments can be skipped */
        if (FA_ELEM_INT_CLIENT(tmpFaIsidVlanAsgn->elemType))
        {
            /* Skip entry */
            tmpFaIsidVlanAsgn = tmpFaIsidVlanAsgn->next;
            continue;
        }

        /* Look for elements with the same I-SID/VLAN data that are */
        /* associated with the same trunk */
        if ((tmpFaIsidVlanAsgn->trunkId == tgtFaIsidVlanAsgn->trunkId) &&
            (tmpFaIsidVlanAsgn->isid == tgtFaIsidVlanAsgn->isid) &&
            (tmpFaIsidVlanAsgn->vlan == tgtFaIsidVlanAsgn->vlan))
        {
            /* Store elements for later processing */
            if (elemCount < FA_AGENT_REM_MAX_ISID_VLAN_ASGNS)
            {
                memcpy((char *)&faRemIsidVlanAsgnList[elemCount],
                       (char *)tmpFaIsidVlanAsgn,
                       sizeof(faRemoteIsidVlanAsgnsEntry_t));

                elemCount++;
            }

            /* If active entries are being deleted, need to update NV */
            if ((tmpFaIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE) &&
                (tmpFaIsidVlanAsgn->components != FA_COMP_NONE))
            {
                saveToNv = 1;
            }
        }

        tmpFaIsidVlanAsgn = tmpFaIsidVlanAsgn->next;
    }

    aasdkx_mutex_unlock(faAgentMut);

    /* Delete FA database and FA LLDP database assignments */
    for (i = 0; i < elemCount; i++)
    {
        faRemoteIsidVlanAsgnDelete(&faRemIsidVlanAsgnList[i], FA_NOTIFY_LLDP);
    }

    return;
}

/**************************************************************
 * Name: faServerAsgnAcceptanceProcessing
 * Description:
 *    FA server I-SID/VLAN assignment acceptance processing 
 *    state machine support. Supports 'pending'/'rejected'
 *    to 'active' transition. 
 * Input Parameters:
 *    faRemIsidVlanData - pointer to faRemoteIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - acceptance processing failure
 *    1 - acceptance processing success
 **************************************************************/

int
faServerAsgnAcceptanceProcessing (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn)
{
    int rc = 0;
#if FA_REM_ASSIGNMENT
    int aa_rc = AA_SDK_ENONE;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faServerAsgnAcceptanceProcessing", 0, 0, 0, NULL);

    if (faRemIsidVlanAsgn == NULL)
    {
        rc = 0;
        goto faServerAsgnAcceptanceProcessingRet;
    }

    /* Check temporary agent state to see if all requests */
    /* should be rejected by default */
    if (faAgentConfigData.faAgentTemporaryState & FA_AGENT_TEMP_STATE_REJECT_ALL)
    {
        faRemIsidVlanAsgn->status = FA_AGENT_ISID_VLAN_ASGN_REJECTED;
        rc = 0;
        goto faServerAsgnAcceptanceProcessingRet;
    }
    
    /* Update remote asgn status */

#ifdef SYSDEF_INCLUDE_SPBM





#endif

    if (aa_rc == AA_SDK_ENONE)
    {
        faRemIsidVlanAsgn->status = FA_AGENT_ISID_VLAN_ASGN_ACTIVE;
        rc = 1;
    }
    else
    {
        faRemIsidVlanAsgn->status = FA_AGENT_ISID_VLAN_ASGN_REJECTED;
        rc = 0;
    }

    /* send the asgn status to the remote element */

    aa_rc = aasdkx_asgn_data_set(faRemIsidVlanAsgn->ifIndex,
                                 faRemIsidVlanAsgn->status,
                                 faRemIsidVlanAsgn->isid,
                                 faRemIsidVlanAsgn->vlan,
                                 true);

    /* cleanup if necessary */

    if ((aa_rc != AA_SDK_ENONE) &&
        (faRemIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE))
    {
#ifdef SYSDEF_INCLUDE_SPBM



#endif
        faRemIsidVlanAsgn->status = FA_AGENT_ISID_VLAN_ASGN_REJECTED;
        rc = 0;
    }
        
    faRemoteIsidVlanAsgnUpdate(faRemIsidVlanAsgn, FA_DISTRIBUTE);
                                 
faServerAsgnAcceptanceProcessingRet:
#else
    rc = 1;
#endif

    return (rc);
}

/**************************************************************
 * Name: faServerAsgnRejectionExternalAppAccess
 * Description:
 *    FA server I-SID/VLAN assignment rejection processing
 *    state machine support. Supports access to external
 *    applications used in conjunction with FA server processing.
 * Input Parameters:
 *    faRemIsidVlanAsgn - pointer to faRemoteIsidVlanAsgnsEntry_t
 *    unit - interface identifier component
 *    port - interface identifier component (1-based value)
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - rejection processing failure
 *    1 - rejection processing success
 **************************************************************/

int
faServerAsgnRejectionExternalAppAccess (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                        unsigned int unit,
                                        unsigned int port)
{
    int rc = 0;

#if FA_REM_ASSIGNMENT
    int failureSeen = 0;
    char vlanStr[32];
    char portStr[32];
    int tmpUnit, vlanMembersPresent = 0;

    if (faRemIsidVlanAsgn == NULL)
    {
        return (rc);
    }

    /* If ifIndex data represents a potentially valid UNI interface, */
    /* interface-based operations are to be performed. Check value and */
    /* proceed accordingly */
    if (FA_AGENT_ACTUAL_IFC(faRemIsidVlanAsgn->ifIndex))
    {
        sprintf(vlanStr, "%d", faRemIsidVlanAsgn->vlan);
    
        unit = 0;
        sprintf(portStr, "%d", port);
    }

#ifdef SYSDEF_INCLUDE_SPBM

























#endif /* SYSDEF_INCLUDE_SPBM */

    /* Update port PVID data, if necessary */
    if ((FA_AGENT_ACTUAL_IFC(faRemIsidVlanAsgn->ifIndex)) &&
        (faRemIsidVlanAsgn->components & FA_COMP_PORT_PVID))
    {
    }

#ifdef SYSDEF_INCLUDE_FA_SMLT











#else  /* SYSDEF_INCLUDE_FA_SMLT */

    /* Delete SPBM switched UNI VLAN, if necessary */
    if (faRemIsidVlanAsgn->components & FA_COMP_VLAN)
    {

#endif /* SYSDEF_INCLUDE_FA_SMLT */

    }

    if (! failureSeen)
    {
        rc = 1;
    }

#endif /* FA_REM_ASSIGNMENT */

    return (rc);
}

/**************************************************************
 * Name: faServerAsgnRejectionProcessing
 * Description:
 *    FA server I-SID/VLAN assignment rejection processing 
 *    state machine support. Supports 'active' to 'rejected'
 *    transition. 
 * Input Parameters:
 *    faRemIsidVlanAsgn - pointer to faRemoteIsidVlanAsgnsEntry_t
 *    rejectionStatus - rejection reason for status update
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - rejection processing failure
 *    1 - rejection processing success
 **************************************************************/

int
faServerAsgnRejectionProcessing (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                 unsigned int rejectionStatus)
{
    int rc = 0;

#if FA_ASSIGNMENT

    unsigned int unit, port;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faServerAsgnRejectionProcessing", 0, 0, 0, NULL);

    if (faRemIsidVlanAsgn == NULL)
    {
        goto faServerAsgnRejectionProcessingRet;
    }


    /* If ifIndex data represents a potentially valid UNI interface, */
    /* interface-based operations are to be performed. Check value and */
    /* proceed accordingly */
    if (FA_AGENT_ACTUAL_IFC(faRemIsidVlanAsgn->ifIndex))
    {
        if (faRemIsidVlanAsgn->ifIndex != port)
        {
            goto faServerAsgnRejectionProcessingRet;
        }

        /* Work with 1-based port value */
        port++;
    }

    /* Tasks:                                                  */
    /*  a) Delete switched UNI data, if necessary              */
    /*  b) Update downlink port VLAN membership, if necessary  */
    /*  c) Update downlink port PVID data, if necessary        */
    /*  d) Delete SPBM switched UNI VLAN, if necessary         */
    /*  e) Update remote asgn state and status appropriately   */

    if (! faServerAsgnRejectionExternalAppAccess(faRemIsidVlanAsgn, unit, port))
    {
        goto faServerAsgnRejectionProcessingRet;
    }

    /* Update remote asgn status */
    faRemIsidVlanAsgn->components = FA_COMP_NONE;
    faRemIsidVlanAsgn->status = rejectionStatus;

    aasdkx_asgn_data_set(faRemIsidVlanAsgn->ifIndex,
                         faRemIsidVlanAsgn->status,
                         faRemIsidVlanAsgn->isid,
                         faRemIsidVlanAsgn->vlan,
                         false);

#ifdef SYSDEF_INCLUDE_SPBM                         



#endif

    faRemoteIsidVlanAsgnUpdate(faRemIsidVlanAsgn, FA_DISTRIBUTE);


#endif /* FA_ASSIGNMENT */

    rc = 1;

faServerAsgnRejectionProcessingRet:

    return (rc);
}

/**************************************************************
 * Name: faServerProcessRemoteElem
 * Description:
 *    Searches the list of current remote ifIndex/I-SID/VLAN
 *    assignments for a match based on the provided parameters.
 *    If found, the assignment is updated. If not found, a
 *    new entry is allocated and added to the remote list.
 *    Externally accessible routine.
 * Input Parameters:
 *    interface - ingress interface value associated with assignment
 *    isid - I-SID value of assignment
 *    vlan - VLAN value of assignment
 *    status - current status for the assignment
 *    elemType - type of assignment originator
 *    elemVlan - VLAN data associated with assignment originator
 * Output Parameters:
 *    none
 * Return Values:
 *    asgnState - operation/entry status (faIsidVlanAsgnStates)
 **************************************************************/

faIsidVlanAsgnStates
faServerProcessRemoteElem (unsigned int interface,
                           unsigned int isid,
                           unsigned int vlan,
                           unsigned int status,
                           unsigned int elemType,
                           unsigned int elemVlan)
{
    int saveToNv = 0;
    faRemoteIsidVlanAsgnsEntry_t tmpFaIsidVlanAsgn;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;
    faIsidVlanAsgnStates asgnState = FA_AGENT_ISID_VLAN_ASGN_UNKNOWN;

    int trunkId = 0;
    unsigned int unit = 0, port = 0;

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid, 
                                              vlan, FA_AGENT_GET_EXACT);

    if (faIsidVlanAsgn != NULL)
    {
        faAgentErrorMsg(FA_AGENT_INFO_REMOTE_UPDATE, FA_AGENT_INFO_MSG_3,
                        NULL, interface, isid, vlan, NULL);

        faIsidVlanAsgn->lastUpdated = faAgentTimeGet();

        if (faIsidVlanAsgn->status != status)
        {
            /* Server assignment status is either 'active' or 'rejected' */
            if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
            {
                /* Nothing to do but possibly update the LLDP FA data */ 
                faRemoteIsidVlanAsgnUpdate(faIsidVlanAsgn, FA_NO_DISTRIBUTE);
                asgnState = FA_AGENT_ISID_VLAN_ASGN_ACTIVE;
            }
            else  /* faIsidVlanAsgn->status >= FA_AGENT_ISID_VLAN_ASGN_REJECTED */
            {
                /* Attempt to activate the assignment */
                if (faServerAsgnAcceptanceProcessing(faIsidVlanAsgn))
                {
                    faAgentErrorMsg(FA_AGENT_INFO_REMOTE_ACCEPT, FA_AGENT_INFO_MSG_3,
                                    NULL, interface, isid, vlan, NULL);

                    asgnState = FA_AGENT_ISID_VLAN_ASGN_ACTIVE;
                    saveToNv = 1;
                }
                else
                {
                    faAgentErrorMsg(FA_AGENT_ERR_REMOTE_ACCEPT_FAIL, FA_AGENT_INFO_MSG_1,
                                    NULL, interface, isid, vlan, NULL);

                    faIsidVlanAsgn->status = FA_ASGN_REJECTION_APP_INTERACTION;
                    faRemoteIsidVlanAsgnUpdate(faIsidVlanAsgn, FA_DISTRIBUTE);
                    asgnState = FA_ASGN_REJECTION_APP_INTERACTION;

                    faAgentStatsData.faAgentServerAcceptanceIssues++;
                }
            }
        }
        else
        {
            asgnState = faIsidVlanAsgn->status;
        }
    }
    else
    {
        /* New assignment processing */

        tmpFaIsidVlanAsgn.ifIndex = interface;
        tmpFaIsidVlanAsgn.isid = isid;
        tmpFaIsidVlanAsgn.vlan = vlan;
        tmpFaIsidVlanAsgn.status = FA_AGENT_ISID_VLAN_ASGN_PENDING;
        tmpFaIsidVlanAsgn.components = FA_COMP_NONE;
        tmpFaIsidVlanAsgn.elemType = elemType;
        tmpFaIsidVlanAsgn.clientVlan = ((INRANGE(1, elemVlan, 4094)) ? elemVlan : 0);

        if ((interface ==  port) && (trunkId))
        {
            tmpFaIsidVlanAsgn.trunkId = trunkId;
        }
        else
        {
            tmpFaIsidVlanAsgn.trunkId = 0;
        }

        /* Perform any validation that is necessary prior */
        /* to accepting a new assignment */
        asgnState = faRemoteIsidVlanAsgnValidate(&tmpFaIsidVlanAsgn);

        if (asgnState != FA_AGENT_ISID_VLAN_ASGN_PENDING)
        {
            faAgentErrorMsg(FA_AGENT_ERR_REMOTE_ACCEPT_FAIL,FA_AGENT_INFO_MSG_1,
                            NULL, interface, isid, vlan, NULL);

            /* Update associated LLDP FA data so that the host */
            /* is notified about the rejection */
            tmpFaIsidVlanAsgn.status = asgnState; 
            faRemoteIsidVlanAsgnUpdate(&tmpFaIsidVlanAsgn, FA_NO_DISTRIBUTE);
            aaPortStatInc(tmpFaIsidVlanAsgn.ifIndex,
                          offsetof(aasdk_stats_data_t, aasdk_asgn_rejected));

            goto faServerProcessRemoteElemRet;
        }

        /* Create tracking entry, update settings and state */

        if (faRemoteIsidVlanAsgnCreate(&tmpFaIsidVlanAsgn))
        {
            faIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid, 
                                                      vlan, FA_AGENT_GET_EXACT);
        }

        if (faIsidVlanAsgn == NULL)
        {
            faAgentErrorMsg(FA_AGENT_ERR_REMOTE_ACCEPT_FAIL,FA_AGENT_INFO_MSG_1,
                            NULL, interface, isid, vlan, NULL);

            /* Update associated LLDP FA data so that the host */
            /* is notified about the rejection */
            tmpFaIsidVlanAsgn.status = FA_ASGN_REJECTION_RESOURCE_ERR; 
            faRemoteIsidVlanAsgnUpdate(&tmpFaIsidVlanAsgn, FA_NO_DISTRIBUTE);
            asgnState = FA_ASGN_REJECTION_RESOURCE_ERR;

            goto faServerProcessRemoteElemRet;
        }

        if (faServerAsgnAcceptanceProcessing(faIsidVlanAsgn))
        {
            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_ACCEPT, FA_AGENT_INFO_MSG_3,
                            NULL, interface, isid, vlan, NULL);
   
            asgnState = FA_AGENT_ISID_VLAN_ASGN_ACTIVE;

            aaPortStatInc(tmpFaIsidVlanAsgn.ifIndex,
                           offsetof(aasdk_stats_data_t, aasdk_asgn_accepted));
            saveToNv = 1;
        }
        else
        {
            faAgentErrorMsg(FA_AGENT_ERR_REMOTE_ACCEPT_FAIL, FA_AGENT_INFO_MSG_1,
                            NULL, interface, isid, vlan, NULL);
  
            faIsidVlanAsgn->status = FA_ASGN_REJECTION_APP_INTERACTION;
            faRemoteIsidVlanAsgnUpdate(faIsidVlanAsgn, FA_DISTRIBUTE);
            asgnState = FA_ASGN_REJECTION_APP_INTERACTION;
 
            faAgentStatsData.faAgentServerAcceptanceIssues++;
        }
    }

faServerProcessRemoteElemRet:

    aasdkx_mutex_unlock(faAgentMut);

    return (asgnState);
}

/**************************************************************
 * Name: faServerPostUpdateProcess
 * Description:
 *    Performs actions that may be required following the 
 *    processing of an I-SID/VLAN assignment advertisement
 *    from a FA host.
 * Input Parameters:
 *    interface - identifies the interface associated with the
 *                update being completed.
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faServerPostUpdateProcess (unsigned int interface)
{

#ifdef FA_AGENT_SERVER_IMMEDIATE_DELETE

    faAgentPerformRemoteListMaintenance(FA_REMOTE_ISID_VLAN_ASGN_IMMEDIATE_DEL, interface);

#endif /* FA_AGENT_SERVER_IMMEDIATE_DELETE */

    return;
}

/**************************************************************
 * Name: faServerAsgnReset
 * Description:
 *    Reset the data that is associated with FA server I-SID/VLAN 
 *    assignments.
 * Input Parameters:
 *    notifyLldp - indicates if an LLDP update is required
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faServerAsgnReset (int notifyLldp)
{
#if FA_ASSIGNMENT
    int asgnIssues = 0;
    int saveToNv = 0;
    int i, elemCount = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faServerAsgnReset", 0, 0, 0, NULL);

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
    while (faIsidVlanAsgn != NULL)
    {
        if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
        {
            /* Store elements for later processing */
            if (elemCount < FA_AGENT_REM_MAX_ISID_VLAN_ASGNS)
            {
                memcpy((char *)&faRemIsidVlanAsgnList[elemCount],
                       (char *)faIsidVlanAsgn,
                       sizeof(faRemoteIsidVlanAsgnsEntry_t));

                elemCount++;
            }

            saveToNv = 1;
        }

        faIsidVlanAsgn = faIsidVlanAsgn->next;
    }

    aasdkx_mutex_unlock(faAgentMut);

    /* Initiate settings cleanup */
    for (i = 0; i < elemCount; i++)
    {
        /* Deactivate assignment settings */
        if (! faServerAsgnRejectionProcessing(&faRemIsidVlanAsgnList[i],
                                              FA_AGENT_ISID_VLAN_ASGN_REJECTED))
        {
            asgnIssues++;
        }
    }

    /* Clear all remote data and reinitialize FA LLDP database */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsTable != NULL)
    {
        faRemoteIsidVlanAsgnsClearAll(notifyLldp);
    }

    if (asgnIssues)
    {
        faAgentErrorMsg(FA_AGENT_ERR_ASGN_RESET_ISSUE, FA_AGENT_INFO_MSG_1,
                        "faServerAsgnReset", asgnIssues, 0, 0, NULL);

        faAgentStatsData.faAgentServerCleanupIssues += asgnIssues;
    }
#endif /* FA_ASSIGNMENT */
    return;
}


/**************************************************************
 * Name: faAgentPerformRemoteListPeerRemoval
 * Description:
 *    Handles list maintenance activities for the remote
 *    I-SID/VLAN assignment list related to a SMLT peer IST
 *    removal/SMLT service termination. Peer entries are
 *    identified by virtual interface numbering.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faAgentPerformRemoteListPeerRemoval (void)
{
    int asgnIssues = 0;
    int saveToNv = 0;
    int i, elemCount = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;
    faRemoteIsidVlanAsgnsEntry_t *tmpFaIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faAgentPerformRemoteListPeerRemoval", 0, 0, 0, NULL);

    if ((faAgentConfigData.faRemoteIsidVlanAsgnsCount == 0) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL))
    {
        /* Nothing to do */
        return;
    }

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
    while (faIsidVlanAsgn != NULL)
    {
        /* Search for entries identifying virtual interfaces */
        /* and initiate list removal actions */

        if (FA_AGENT_VIRTUAL_IFC(faIsidVlanAsgn->ifIndex))
        {
            tmpFaIsidVlanAsgn = faIsidVlanAsgn->next;

            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_PEER_REMOVE,FA_AGENT_INFO_MSG_3,
                            NULL,
                            faIsidVlanAsgn->ifIndex,
                            faIsidVlanAsgn->isid,
                            faIsidVlanAsgn->vlan,
                            NULL);

            /* Store elements for later processing */
            if (elemCount < FA_AGENT_REM_MAX_ISID_VLAN_ASGNS)
            {
                memcpy((char *)&faRemIsidVlanAsgnList[elemCount],
                       (char *)faIsidVlanAsgn,
                       sizeof(faRemoteIsidVlanAsgnsEntry_t));

                elemCount++;
            }

            if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
            {
                saveToNv = 1;
            }

            faRemoteIsidVlanAsgnDelete(faIsidVlanAsgn, FA_NO_NOTIFY_LLDP);

            faIsidVlanAsgn = tmpFaIsidVlanAsgn;
        }
        else
        {
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    /* Initiate settings cleanup */
    for (i = 0; i < elemCount; i++)
    {
        /* Deactivate assignment settings */
        if (! faServerAsgnRejectionExternalAppAccess(&faRemIsidVlanAsgnList[i], 0, 0))
        {
            asgnIssues++;
        }
    }

    if (asgnIssues)
    {
        faAgentErrorMsg(FA_AGENT_ERR_REMOVE_PEER_FAIL, FA_AGENT_INFO_MSG_1,
                        0, 0, 0, 0, NULL);

        faAgentStatsData.faAgentServerSmltPeerCleanupIssues += asgnIssues;
    }

    return;
}

/**************************************************************
 * Name: faServerProcessSmltPeerRemoteResp
 * Description:
 *    Searches the list of current remote ifIndex/I-SID/VLAN
 *    assignments for a match based on the provided parameters.
 *    If found, the assignment state is updated to 'rejected'
 *    (peer responses are only generated in rejection scenarios).
 * Input Parameters:
 *    interface - ingress interface value associated with assignment
 *    isid - I-SID value of assignment
 *    vlan - VLAN value of assignment
 *    status - current status for the assignment
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faServerProcessSmltPeerRemoteResp (unsigned int interface,
                                   unsigned int isid,
                                   unsigned int vlan,
                                   unsigned int status)
{
#ifdef SYSDEF_INCLUDE_FA_SMLT















 
 






















#endif /* SYSDEF_INCLUDE_FA_SMLT */

    return;
}



