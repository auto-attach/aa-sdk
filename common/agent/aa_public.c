/* aa-sdk/common/agent/aa_public.c */

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

#include "aa_defs.h"
#include "aa_agent_private.h"

#include "aasdk_osif_comm.h"

int aasdkx_ports_data_ena_set(bool enable); // compiler warning

/**************************************************************
 * Name: faAgentServiceEnabled
 * Description:
 *    Used to query the FA service status.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - FA service is not enabled      
 *    1 - FA service is enabled
 **************************************************************/

int
faAgentServiceEnabled ()
{
    return (faAgentConfigData.faServiceStatus == FA_AGENT_STATE_ENABLED);
}

/**************************************************************
 * Name: faAgentGetServiceStatus
 * Description:
 *    Used to query the FA service status.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    FA_AGENT_STATE_ENABLED - FA service is enabled
 *    FA_AGENT_STATE_DISABLED - FA service is disabled      
 **************************************************************/

int
faAgentGetServiceStatus ()
{
    return (faAgentConfigData.faServiceStatus);
}

/**************************************************************
 * Name: faAgentGetElementType
 * Description:
 *    Used to query the current FA element type.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    FA_AGENT_ELEMENT_HOST - FA host operation supported
 *    FA_AGENT_ELEMENT_SERVER - FA server operation supported 
 **************************************************************/

int
faAgentGetElementType ()
{
    return (faAgentConfigData.faElementType);
}


/**************************************************************
 * Name: faAgentMgmtVlan
 * Description:
 *    Used to query the current FA management VLAN
 * Input Parameters:
 *    pMgmtVlan - ptr to caller's variable
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faAgentGetMgmtVlan(uint16_t *pMgmtVlan)
{
    if (pMgmtVlan != NULL)
    {
        *pMgmtVlan = faAgentConfigData.faMgmtVlan;
    }
}


int
faAgentGetAutoAttachStatus ()
{
    return (faAgentConfigData.faAutoAttachStatus);
}

int
faAgentGetMsgAuthStatus ()
{
    return (faAgentConfigData.faMsgAuthStatus);
}

int
faAgentGetMsgAuthKey (char *authKey, int *authKeyLen)
{
    int rc = 0;  /* 0 = failure, 1 = success */

    if ((authKey != NULL) && (authKeyLen != NULL) &&
        (*authKeyLen >= faAgentConfigData.faMsgAuthKeyLength))
    {
        *authKeyLen = faAgentConfigData.faMsgAuthKeyLength;
        memcpy(authKey, faAgentConfigData.faMsgAuthKey,
               faAgentConfigData.faMsgAuthKeyLength);

        rc = 1;
    }
    
    return (rc);
}

/**************************************************************
 * Name: faAgentGetHostProxyStatus
 * Description:
 *    Used to query the FA agent host proxy operation status.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    FA_AGENT_HOST_PROXY_ENABLED - FA host proxy is enabled
 *    FA_AGENT_HOST_PROXY_DISABLED - FA host proxy is disabled      
 **************************************************************/

int
faAgentGetHostProxyStatus ()
{
    return (faAgentConfigData.faHostProxyStatus);
}

int
faAgentIsDeviceActiveFabricAttachHost ()
{
    if ((faAgentConfigData.faServiceStatus == FA_AGENT_STATE_ENABLED) &&
        (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

int
faAgentIsDeviceActiveFabricAttachServer ()
{
    if ((faAgentConfigData.faServiceStatus == FA_AGENT_STATE_ENABLED) &&
        (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

int
faAgentGetPrimaryServerId (int *serverIdLen,
                           unsigned char *serverId)
{
    if ((serverIdLen == NULL) || (serverId == NULL))
        return (0);

    *serverIdLen = faAgentConfigData.faAgentPrimaryServerIdLen;
    memcpy(serverId, faAgentConfigData.faAgentPrimaryServerId,
           faAgentConfigData.faAgentPrimaryServerIdLen);

    return (faAgentConfigData.faAgentPrimaryServerIdLen);
}

int
faAgentGetPrimaryServerDescr (int *serverDescrLen,
                              unsigned char *serverDescr)
{
    if ((serverDescrLen == NULL) || (serverDescr == NULL))
        return (0);

    *serverDescrLen = faAgentConfigData.faAgentPrimaryServerDescrLen;
    memcpy(serverDescr, faAgentConfigData.faAgentPrimaryServerDescr,
           faAgentConfigData.faAgentPrimaryServerDescrLen);

    return (faAgentConfigData.faAgentPrimaryServerDescrLen);
}

int
faAgentGetLocalIsidVlanAsgnsCount ()
{
    return (faAgentConfigData.faLocalIsidVlanAsgnsCount);
}

int
faAgentGetRemoteIsidVlanAsgnsCount ()
{
    return (faAgentConfigData.faRemoteIsidVlanAsgnsCount);
}


int
faAgentSetServiceStatus (int status,
                         int saveToNv,
                         int distributeData)
{
    int rc = 0;
    bool faServiceEnable;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetServiceStatus", status, 0, 0, NULL);
    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetServiceStatus", saveToNv, distributeData, 0, NULL);

    if (INRANGE(FA_AGENT_STATE_ENABLED, status, FA_AGENT_STATE_DISABLED))
    {
        /* Update LLDP FA settings */

        faServiceEnable = (status == FA_AGENT_STATE_ENABLED) ? true : false;

        aasdkx_ports_data_ena_set(faServiceEnable);

        if (faAgentConfigData.faServiceStatus != status)
        {
            faAgentConfigData.faServiceStatus = status;
    
            if (status == FA_AGENT_STATE_ENABLED)
            {
                faAgentErrorMsg(FA_AGENT_INFO_SERVICE_ENABLED, FA_AGENT_INFO_MSG_2, 0, 0, 0, 0, NULL);
            }
            else
            {
                faAgentErrorMsg(FA_AGENT_INFO_SERVICE_DISABLED, FA_AGENT_INFO_MSG_2, 0, 0, 0, 0, NULL);

                if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
                {
#ifdef FA_AGENT_HOST_PROXY_SUPPORT

                    /* Reset FA host assignments to 'pending', cleaning up previously */
                    /* established settings as necessary. Resync the local LLDP FA data */
                    /* and clear any remote data */
                    faHostAsgnReset(FALSE);
                    faAgentPerformServerMaintenance(TRUE);
#endif

                }
                else if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER)
                {
                    /* Delete FA server assignments, cleaning up previously established */
                    /* settings as necessary. Clear the LLDP FA data */
                    faServerAsgnReset(FA_NOTIFY_LLDP);
                }
            }
        }

        rc = 1;
   } 

    return (rc);
}



#ifdef SYSDEF_INCLUDE_SPBM






















































#endif /* SYSDEF_INCLUDE_SPBM */



int
faAgentSetAutoAttachStatus (int status,
                            int saveToNv)
{
    int rc = 0;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetAutoAttachStatus", status, saveToNv, 0, NULL);

    if (INRANGE(FA_AGENT_AA_STATE_ENABLED, status, FA_AGENT_AA_STATE_DISABLED))
    {
        /* Update LLDP FA settings */

        if (faAgentConfigData.faAutoAttachStatus != status)
        {
            faAgentConfigData.faAutoAttachStatus = status;

            if (status == FA_AGENT_AA_STATE_ENABLED)
            {
                faAgentErrorMsg(FA_AGENT_INFO_AUTOATCH_ENABLED, FA_AGENT_INFO_MSG_2, 0, 0, 0, 0, NULL);

                /* Set management VLAN data */ 
                faAgentSetMgmtVlan(faAgentConfigData.faMgmtVlan);
            }
            else
            {
                faAgentErrorMsg(FA_AGENT_INFO_AUTOATCH_DISABLED, FA_AGENT_INFO_MSG_2, 0, 0, 0, 0, NULL);

                /* Clear management VLAN data */ 
                faAgentConfigData.faMgmtVlan = FA_AGENT_AA_VLAN_NOOP;
                faAgentSetMgmtVlan(faAgentConfigData.faMgmtVlan);
            }

        }
 
        rc = 1;
    }

    return (rc);
}

int
faAgentSetCleanupMode (int status,
                       int saveToNv)
{
    int rc = 0;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetCleanupMode", status, saveToNv, 0, NULL);

    if (INRANGE(FA_AGENT_CLEANUP_MODE_ENABLED, status, FA_AGENT_CLEANUP_MODE_DISABLED))
    {
        if (faAgentConfigData.faCleanupMode != status)
        {
            faAgentConfigData.faCleanupMode = status;
        }
 
        rc = 1;
    }

    return (rc);
}

int
faAgentSetMsgAuthStatus (int status,
                         int saveToNv)
{
    int rc = 0;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetMsgAuthStatus", status, saveToNv, 0, NULL);

#ifdef SYSDEF_INCLUDE_SSH

    if (INRANGE(FA_AGENT_MSG_AUTH_ENABLED, status, FA_AGENT_MSG_AUTH_DISABLED))
    {
        /* Update LLDP FA settings */

        if (faAgentConfigData.faMsgAuthStatus != status)
        {
            faAgentConfigData.faMsgAuthStatus = status;
        }
 
        rc = 1;
    }

#endif /* SYSDEF_INCLUDE_SSH */

    return (rc);
}

int
faAgentSetMsgAuthKey (char *authKey,
                      int authKeyLen,
                      int saveToNv)
{
    int rc = 0;


    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetMsgAuthKey", authKeyLen, saveToNv, 0, NULL);

#ifdef SYSDEF_INCLUDE_SSH

    if ((authKey != NULL) && 
        (INRANGE(FA_AGENT_MSG_AUTH_KEY_MIN_LEN, authKeyLen, FA_AGENT_MSG_AUTH_KEY_MAX_LEN)))
    {
        memcpy(faAgentConfigData.faMsgAuthKey, authKey, authKeyLen);
        faAgentConfigData.faMsgAuthKeyLength = authKeyLen;

        /* Update LLDP FA settings */

        rc = 1;
    }

#endif /* SYSDEF_INCLUDE_SSH */

    return (rc);
}

int
faAgentSetHostProxyStatus (int status,
                           int saveToNv)
{
    int rc = 0;

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4, 
                    "faAgentSetHostProxyStatus", status, saveToNv, 0, NULL);

    if (INRANGE(FA_AGENT_HOST_PROXY_ENABLED, status, FA_AGENT_HOST_PROXY_DISABLED))
    {
        /* Update LLDP FA settings */

        if (faAgentConfigData.faHostProxyStatus != status)
        {
            faAgentConfigData.faHostProxyStatus = status;

            if (faAgentConfigData.faHostProxyStatus == FA_AGENT_HOST_PROXY_DISABLED)
            {
                faAgentPerformProxyListHostRemoval(0, 0);

                /* Notify interested applications */
                faNotifyApps(FA_EVENT_PROXY_DISABLED);
            }
            else
            {
                /* Notify interested applications */
                faNotifyApps(FA_EVENT_PROXY_ENABLED);
            }

        }
 
        rc = 1;
    }

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

    return (rc);
}



#if FA_ASSIGNMENT


/**************************************************************
 * Name: faLocalIsidVlanAsgnQuery
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided I-SID and VLAN data. If
 *    found, the data is returned in the provided parameter.
 *    Externally accessible routine.
 * Input Parameters:
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 *    faIsidVlanData - pointer to faLocalIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    faIsidVlanData - requested I-SID/VLAN assignment data
 * Return Values:
 *    0 - target entry not found
 *    1 - target entry found
 **************************************************************/

int
faLocalIsidVlanAsgnQuery (unsigned int isid,
                          unsigned int vlan,
                          int searchType,
                          faLocalIsidVlanAsgnsEntry_t *faIsidVlanData)
{
    int rc = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanEntry;

    if (faIsidVlanData != NULL)
    {
        aasdkx_mutex_lock(faAgentMut);

        faIsidVlanEntry = faLocalIsidVlanAsgnFind(isid, vlan, searchType);

        if (faIsidVlanEntry != NULL)
        {
            faIsidVlanData->isid = faIsidVlanEntry->isid;
            faIsidVlanData->vlan = faIsidVlanEntry->vlan;
            faIsidVlanData->status = faIsidVlanEntry->status;
            faIsidVlanData->origin = faIsidVlanEntry->origin;
            faIsidVlanData->referenceCount = faIsidVlanEntry->referenceCount;

            rc = 1;
        }

        aasdkx_mutex_unlock(faAgentMut);
    }

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnVlanPresent
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data only. If found, 
 *    the associated I-SID is returned. Externally accessible 
 *    routine.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found
 *    isid - target entry found: I-SID data
 **************************************************************/

int
faLocalIsidVlanAsgnVlanPresent (unsigned int vlan)
{
    int rc = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->vlan == vlan)
            {
                rc = faIsidVlanAsgn->isid;
                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnConflict
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data only. If found, 
 *    several checks are made related to the current operational
 *    context to ensure that the data is acceptable for installation.
 * Input Parameters:
 *    interface - target interface data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    elemSrc - assignment origin indicator
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Data conflict exists
 *    1 - Data conflict does not exist
 **************************************************************/

int
faLocalIsidVlanAsgnConflict (unsigned int interface,
                             unsigned int isid,
                             unsigned int vlan,
                             unsigned int elemSrc)
{
    int rc = 1;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            /* If proxy support is enabled, multiple I-SID/VLAN */
            /* assignments with the same index data are allowed */
            /* but only a single host entry can be defined. If */
            /* proxy support is disabled, a simple duplicate */
            /* VLAN check is all that's needed */

            if (faIsidVlanAsgn->vlan == vlan)
            {
                if (faAgentConfigData.faHostProxyStatus == FA_AGENT_HOST_PROXY_ENABLED)
                {
                    if (faIsidVlanAsgn->isid != isid)
                    {
                        /* I-SID conflict - failure */
                        rc = 0;
                        break;
                    }

                    if ((elemSrc == FA_ELEM_ORIGIN_PROXY) &&
                        (faIsidVlanAsgn->origin == FA_ELEM_ORIGIN_PROXY))
                    {
                        /* Origin conflict - failure */
                        rc = 0;
                        break;
                    }

                    if ((interface != FA_AGENT_LOCAL_ASGN_INDICATOR) &&
                        (faIsidVlanAsgn->ifIndex == interface))
                    {
                        /* Client binding conflict - failure */
                        rc = 0;
                        break;
                    }
                }
                else
                {
                    /* Duplicate VLAN - failure */
                    rc = 0;
                    break;
                }
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnVlanFaCreated
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data only. If found, 
 *    entry state data is consulted to determine if the FA agent
 *    dynamically created the VLAN. Externally accessible routine.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found or found but not FA created
 *    1 - target entry found and VLAN FA created
 **************************************************************/

int
faLocalIsidVlanAsgnVlanFaCreated (unsigned int vlan)
{
    int rc = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->vlan == vlan)
            {
                if (faIsidVlanAsgn->components & FA_COMP_VLAN)
                {
                    rc = 1;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnVlanRequired
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data and whether
 *    the assignment is required. If found and required by the
 *    agent, the associated I-SID is returned. Externally 
 *    accessible routine.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found
 *    isid - target entry found: I-SID data
 **************************************************************/

int
faLocalIsidVlanAsgnVlanRequired (unsigned int vlan)
{
    int rc = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->vlan == vlan)
            {

#ifdef FA_HOST_VLAN_AUTO_CREATE

                if ((faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE) &&
                    (! (faIsidVlanAsgn->elemState & FA_ELEM_STATE_DEACTIVATION)))
                {
                    rc = faIsidVlanAsgn->isid;
                }

#else  /* FA_HOST_VLAN_AUTO_CREATE */

                rc = faIsidVlanAsgn->isid;

#endif /* FA_HOST_VLAN_AUTO_CREATE */

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnVlanActive
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data and an 'active'
 *    assignment state. If found, the associated I-SID is returned.
 *    Externally accessible routine.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found
 *    isid - target entry found: I-SID data
 **************************************************************/

int
faLocalIsidVlanAsgnVlanActive (unsigned int vlan)
{
    int rc = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->vlan == vlan)
            {
                if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
                {
                    rc = faIsidVlanAsgn->isid;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnVlanActive
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data, an 'active'
 *    assignment state, a VLAN ownership indication and ifIndex
 *    data that is different than that provided by the caller.
 *    Externally accessible routine.
 * Input Parameters:
 *    vlan - target VLAN data
 *    excludedIfc - excluded ifIndex for search
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - VLAN data is not "active".
 *    1 - VLAN data is "active" (passes test criteria).
 **************************************************************/

int
faRemoteIsidVlanAsgnVlanActive (unsigned int vlan,
                                unsigned int excludedIfc)
{
    int rc = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if ((faIsidVlanAsgn->vlan == vlan) &&
                (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE) &&
                (faIsidVlanAsgn->components & FA_COMP_VLAN) &&
                (faIsidVlanAsgn->ifIndex != excludedIfc))
            {
                rc = 1;
                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnConflict
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided Interface/ISID/VLAN data. 
 *    If found, several checks are made related to the current 
 *    operational context to ensure that the data is acceptable 
 *    for installation.
 * Input Parameters:
 *    interface - target interface data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    elemSrc - assignment origin indicator
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Data conflict exists
 *    1 - Data conflict does not exist
 **************************************************************/

int
faRemoteIsidVlanAsgnConflict (unsigned int interface,
                              unsigned int isid,
                              unsigned int vlan,
                              unsigned int elemSrc)
{
    int rc = 1;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            /* Entry duplication is all that is checked currently */

            if ((faIsidVlanAsgn->ifIndex == interface) &&
                (faIsidVlanAsgn->isid == isid) &&
                (faIsidVlanAsgn->vlan == vlan))
            {
                if (faIsidVlanAsgn->elemType != elemSrc)
                {
                    /* Duplicate - failure */
                    rc = 0;
                    break;
                }
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnQuery
 * Description:
 *    Traverses the list of I-SID/VLAN assignments looking for
 *    a match based on the provided ifIndex, I-SID and VLAN 
 *    data. If found, the data is returned in the provided 
 *    parameter. Externally accessible routine.
 * Input Parameters:
 *    ifIndex - target ifIndex data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 *    faIsidVlanData - pointer to faRemoteIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    faIsidVlanData - requested I-SID/VLAN assignment data
 * Return Values:
 *    0 - target entry not found
 *    1 - target entry found
 **************************************************************/

int
faRemoteIsidVlanAsgnQuery (unsigned int ifIndex,
                           unsigned int isid,
                           unsigned int vlan,
                           int searchType,
                           faRemoteIsidVlanAsgnsEntry_t *faIsidVlanData)
{
    int rc = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanEntry;

    if (faIsidVlanData != NULL)
    {
        aasdkx_mutex_lock(faAgentMut);

        faIsidVlanEntry = faRemoteIsidVlanAsgnFind(ifIndex, isid, vlan, searchType);

        if (faIsidVlanEntry != NULL)
        {
            faIsidVlanData->ifIndex = faIsidVlanEntry->ifIndex;
            faIsidVlanData->isid = faIsidVlanEntry->isid;
            faIsidVlanData->vlan = faIsidVlanEntry->vlan;
            faIsidVlanData->status = faIsidVlanEntry->status;

            rc = 1;
        }

        aasdkx_mutex_unlock(faAgentMut);
    }

    return (rc);
}

/**************************************************************
 * Name: faAgentGetRemoteIsidVlanAsgnsCountByIfc
 * Description:
 *    Returns the number of remote I-SID/VLAN assignment
 *    entries that are associtaed with the target interface
 *    value.
 * Input Parameters:
 *    ifIndex - target ifIndex data
 * Output Parameters:
 *    none
 * Return Values:
 *    Count of matching entries
 **************************************************************/

int
faAgentGetRemoteIsidVlanAsgnsCountByIfc (unsigned int tgtIfIndex)
{
    int count = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanEntry;

    aasdkx_mutex_unlock(faAgentMut);

    faIsidVlanEntry = faRemoteIsidVlanAsgnFind(tgtIfIndex, 0, 1, FA_AGENT_GET_NEXT);

    while ((faIsidVlanEntry != NULL) &&
           (faIsidVlanEntry->ifIndex == tgtIfIndex))
    {
        count++;

        faIsidVlanEntry = faRemoteIsidVlanAsgnFind(faIsidVlanEntry->ifIndex, 
                                                   faIsidVlanEntry->isid, 
                                                   faIsidVlanEntry->vlan, 
                                                   FA_AGENT_GET_NEXT);
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (count);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnVlanFaCreated
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided VLAN data only. If found, 
 *    entry state data is consulted to determine if the FA agent
 *    dynamically created the VLAN. Externally accessible routine.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found or found but not FA created
 *    1 - target entry found and VLAN FA created
 **************************************************************/

int
faRemoteIsidVlanAsgnVlanFaCreated (unsigned int vlan)
{
    int rc = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->vlan == vlan)
            {
                if (faIsidVlanAsgn->components & FA_COMP_VLAN)
                {
                    rc = 1;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faRemoteSwitchedUniFaCreated
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided ifIndex/I-SID/VLAN data. 
 *    If found, entry state data is consulted to determine if the 
 *    FA agent dynamically created the switched UNI. Externally 
 *    accessible routine.
 * Input Parameters:
 *    interface - target interface data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found or found but not FA created
 *    1 - target entry found and switched UNI FA created
 **************************************************************/

int
faRemoteSwitchedUniFaCreated (unsigned int interface, 
                              unsigned int isid,
                              unsigned int vlan)
{
    int rc = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid,
                                              vlan, FA_AGENT_GET_EXACT);

    if ((faIsidVlanAsgn != NULL) &&
        (faIsidVlanAsgn->components & FA_COMP_VLAN_ISID))
    {
        rc = 1;
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

#endif /* FA_ASSIGNMENT */


#ifdef FA_AGENT_HOST_PROXY_SUPPORT

/**************************************************************
 * Name: faInternalClientCreate
 * Description:
 *    Initiates creation of an internal FA client assignment 
 *    using the specified ifIndex/I-SID/VLAN data for client 
 *    assignment creation. Clients are either tagged or untagged.
 *    Externally accessible routine.
 * Input Parameters:
 *    interface - ingress interface associated with assignment
 *    isid - I-SID value of assignment
 *    vlan - VLAN value of assignment
 *    elemType - type of assignment originator
 *    elemVlan - VLAN associated with assignment originator
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Request to create internal FA client failed
 *    1 - Request to create internal FA client succeeded
 **************************************************************/

int
faInternalClientCreate (unsigned int interface,
                        unsigned int isid,
                        unsigned int vlan,
                        unsigned int elemType,
                        unsigned int elemVlan)
{
    int rc = 1;
    unsigned int unit, port;
    faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn;
    faIsidVlanAsgnStates asgnState;

    /* If currently a stack member, operations should only */
    /* be initiated on the base unit */
    if ((faAgentConfigData.faAgentInStackMode) && (! SMGR_isBaseUnit()))
    {
        rc = 0;
        goto faInternalClientCreateRet;
    }

    /* Validate parameters */

    /* Only internal clients (tagged/untagged) are supported */
    if (! FA_ELEM_INT_CLIENT(elemType))
    {
        rc = 0;
        goto faInternalClientCreateRet;
    }

    /* Perform a cursory isid/vlan check */
    if ((! INRANGE(1, isid, FA_MAX_ISID)) ||
        (! INRANGE(1, vlan, Q_MAX_VID)))
    {
        rc = 0;
        goto faInternalClientCreateRet;
    }

    aasdkx_mutex_lock(faAgentMut);

    faRemIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid, 
                                                 vlan, FA_AGENT_GET_EXACT);

    aasdkx_mutex_unlock(faAgentMut);

    /* Check for previously configured data - not allowed */
    if (faRemIsidVlanAsgn != NULL)
    {
        rc = 0;
        goto faInternalClientCreateRet;
    }

    if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
    {
        asgnState = faProxyProcessRemoteElem(interface, isid, vlan, 
                                             FA_AGENT_ISID_VLAN_ASGN_PENDING, 
                                             elemType, elemVlan);

        if ((asgnState != FA_AGENT_ISID_VLAN_ASGN_PENDING) &&
            (asgnState != FA_AGENT_ISID_VLAN_ASGN_ACTIVE))
        {
            rc = 0;
        }
    }
    else  /* FA_AGENT_ELEMENT_SERVER */
    {
        asgnState = faServerProcessRemoteElem(interface, isid, vlan, 
                                              FA_AGENT_ISID_VLAN_ASGN_PENDING, 
                                              elemType, elemVlan);

        if (asgnState != FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
        {
            rc = 0;
        }
    }


faInternalClientCreateRet:

    if (rc)
    {
        faAgentErrorMsg(FA_AGENT_INFO_INT_CREATE_SUCCESS, FA_AGENT_INFO_MSG_2,
                        NULL, interface, isid, vlan, NULL);
    }
    else
    {
        faAgentErrorMsg(FA_AGENT_INFO_INT_CREATE_FAIL, FA_AGENT_INFO_MSG_1,
                        NULL, interface, isid, vlan, NULL);
    }

    return (rc);
}

/**************************************************************
 * Name: faInternalClientDelete
 * Description:
 *    Initiates deletion of an internal FA client assignment 
 *    using the specified ifIndex/I-SID/VLAN data for client 
 *    assignment deletion. Externally accessible routine.
 * Input Parameters:
 *    interface - ingress interface associated with assignment
 *    isid - I-SID value of assignment
 *    vlan - VLAN value of assignment
 *    elemType - type of assignment originator
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Request to delete internal FA client failed
 *    1 - Request to delete internal FA client succeeded
 **************************************************************/

int
faInternalClientDelete (unsigned int interface,
                        unsigned int isid,
                        unsigned int vlan,
                        unsigned int elemType)
{
    int rc = 1;
    int saveToNv = 0;
    unsigned int unit, port;
    faLocalIsidVlanAsgnsEntry_t *faLocIsidVlanAsgn;
    faLocalIsidVlanAsgnsEntry_t tmpFaLocIsidVlanAsgn;
    faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn;
    faRemoteIsidVlanAsgnsEntry_t tmpFaRemIsidVlanAsgn;

    /* Validate parameters */

    /* Only internal clients (tagged/untagged) are supported */
    if (! FA_ELEM_INT_CLIENT(elemType))
    {
        rc = 0;
        goto faInternalClientDeleteRet;
    }

    /* Perform a cursory isid/vlan check */
    if ((! INRANGE(1, isid, FA_MAX_ISID)) ||
        (! INRANGE(1, vlan, Q_MAX_VID)))
    {
        rc = 0;
        goto faInternalClientDeleteRet;
    }

    aasdkx_mutex_lock(faAgentMut);

    faRemIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid, 
                                                 vlan, FA_AGENT_GET_EXACT);

    if ((faRemIsidVlanAsgn == NULL) ||
        (! FA_ELEM_INT_CLIENT(faRemIsidVlanAsgn->elemType)))
    {
        rc = 0;
        aasdkx_mutex_unlock(faAgentMut);
        goto faInternalClientDeleteRet;
    }

    memcpy(&tmpFaRemIsidVlanAsgn, faRemIsidVlanAsgn, sizeof(faRemoteIsidVlanAsgnsEntry_t));

    aasdkx_mutex_unlock(faAgentMut);

    /* If active entries with component data are being deleted, need to update NV */
    if ((tmpFaRemIsidVlanAsgn.status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE) &&
        (tmpFaRemIsidVlanAsgn.components != FA_COMP_NONE))
    {
        saveToNv = 1;
    }

    if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
    {
        /* Initiate settings cleanup - deactivate assignments */
        if (tmpFaRemIsidVlanAsgn.components != FA_COMP_NONE)
        {
            if (! faProxyAsgnRejectionProcessing(&tmpFaRemIsidVlanAsgn,
                                                 FA_AGENT_ISID_VLAN_ASGN_REJECTED))
            {
                rc = 0;
            }
        }
    
        /* Delete the proxy assignment from the local host database */
        /* if there aren't multiple references to the service */
        faLocIsidVlanAsgn = faLocalIsidVlanAsgnFind(tmpFaRemIsidVlanAsgn.isid,
                                                    tmpFaRemIsidVlanAsgn.vlan,
                                                    FA_AGENT_GET_EXACT);

        if (faLocIsidVlanAsgn != NULL)
        {
            /* Are there multiple service instantiations? */
            if (faLocIsidVlanAsgn->referenceCount > 1)
            {
                /* Decrement instance count */
                faLocIsidVlanAsgn->referenceCount--;

                faAgentDistributeData(FA_AGENT_MSG_LOCAL_ASGN_DATA,
                                      FA_AGENT_MSG_SUB_UPDATE,
                                      0, 0, 0, (void *)faLocIsidVlanAsgn);
            }
            else
            {
                /* If the base element owner is a client (proxy), the */
                /* local representation needs to be deleted */
                if (faLocIsidVlanAsgn->origin == FA_ELEM_ORIGIN_CLIENT)
                {
                    memset(&tmpFaLocIsidVlanAsgn, 0, sizeof(faLocalIsidVlanAsgnsEntry_t));
                    tmpFaLocIsidVlanAsgn.isid = faLocIsidVlanAsgn->isid;
                    tmpFaLocIsidVlanAsgn.vlan = faLocIsidVlanAsgn->vlan;

                    if (! faLocalIsidVlanAsgnDelete(faLocIsidVlanAsgn, FA_NOTIFY_LLDP))
                    {
                        rc = 0;
                    }
                    else
                    {
                        faAgentDistributeData(FA_AGENT_MSG_LOCAL_ASGN_DATA, FA_AGENT_MSG_SUB_DELETE,
                                              0, 0, 0, (void *)&tmpFaLocIsidVlanAsgn);
                    }
                }
                else
                {
                    /* Decrement instance count */
                    faLocIsidVlanAsgn->referenceCount--;

                    faAgentDistributeData(FA_AGENT_MSG_LOCAL_ASGN_DATA,
                                          FA_AGENT_MSG_SUB_UPDATE,
                                          0, 0, 0, (void *)faLocIsidVlanAsgn);
                }
            }
        }

        if (! faRemoteIsidVlanAsgnDelete(&tmpFaRemIsidVlanAsgn, FA_NO_NOTIFY_LLDP))
        {
            rc = 0;
        }
    }
    else  /* FA_AGENT_ELEMENT_SERVER */
    {
        /* Initiate settings cleanup - deactivate assignments */
        if (tmpFaRemIsidVlanAsgn.components != FA_COMP_NONE)
        {
            if (! faServerAsgnRejectionProcessing(&tmpFaRemIsidVlanAsgn,
                                                  FA_AGENT_ISID_VLAN_ASGN_REJECTED))
            {
                rc = 0;
            }
        }

        if (! faRemoteIsidVlanAsgnDelete(&tmpFaRemIsidVlanAsgn, FA_NO_NOTIFY_LLDP))
        {
            rc = 0;
        }
    }

faInternalClientDeleteRet:

    if (rc)
    {
        faAgentErrorMsg(FA_AGENT_INFO_INT_DELETE_SUCCESS, FA_AGENT_INFO_MSG_2,
                        NULL, interface, isid, vlan, NULL);
    }
    else
    {
        faAgentErrorMsg(FA_AGENT_INFO_INT_DELETE_FAIL, FA_AGENT_INFO_MSG_1,
                        NULL, interface, isid, vlan, NULL);
    }

    return (rc);
}

/**************************************************************
 * Name: faInternalClientQuery
 * Description:
 *    Supports verifying internal FA client assignment
 *    existence and status retrieval based on the
 *    specified ifIndex/I-SID/VLAN. Externally accessible 
 *    routine.
 * Input Parameters:
 *    interface - ingress interface associated with assignment
 *    isid - I-SID value of assignment
 *    vlan - VLAN value of assignment
 *    status - assignment status return value placeholder
 *    clientVlan - client VLAN return value placeholder
 * Output Parameters:
 *    status - current assignment status
 *    clientVlan - client VLAN data
 * Return Values:
 *    0 - Internal FA client assignment query failed
 *    1 - Internal FA client assignment query succeeded
 **************************************************************/

int
faInternalClientQuery (unsigned int interface,
                       unsigned int isid,
                       unsigned int vlan,
                       unsigned int *status,
                       unsigned int *clientVlan)
{
    int rc = 1;
    unsigned int unit, port;
    faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn;

    /* Validate parameters */

    if ((status == NULL) || (clientVlan == NULL))
    {
        rc = 0;
        goto faInternalClientQueryRet;
    }

    /* Perform a cursory interface/isid/vlan check */
    if ((! INRANGE(1, isid, FA_MAX_ISID)) ||
        (! INRANGE(1, vlan, Q_MAX_VID)))
    {
        rc = 0;
        goto faInternalClientQueryRet;
    }

    aasdkx_mutex_lock(faAgentMut);

    faRemIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid, 
                                                 vlan, FA_AGENT_GET_EXACT);

    if ((faRemIsidVlanAsgn == NULL) ||
        (! FA_ELEM_INT_CLIENT(faRemIsidVlanAsgn->elemType)))
    {
        rc = 0;
    }
    else
    {
        *status = faRemIsidVlanAsgn->status;
        *clientVlan = faRemIsidVlanAsgn->clientVlan;
    }

    aasdkx_mutex_unlock(faAgentMut);

faInternalClientQueryRet:

    return (rc);
}

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

/**************************************************************
 * Name: faRegisterCallback
 * Description:
 *    Registers a callback routine with the FA agent. Routine
 *    is called when events that it has registered for are
 *    detected by the FA agent. Externally accessible routine.
 * Input Parameters:
 *    tgtEvents - FA events monitored for callback
 *    callback - pointer to callback routine (FaNotificationHandler)
 *    context - pointer to context data passed as-is to
 *              callback during invocation
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Callback routine not successfully registered
 *   >0 - Callback routine identifier (used for deregistration)
 **************************************************************/

int
faRegisterCallback (unsigned int tgtEvents,
                    FaNotificationHandler callback,
                    void *context)
{
    int i, rc = 0;

    if ((tgtEvents == 0) || (callback == NULL))
    {
        return (rc);
    }

    aasdkx_mutex_lock(faAgentMut);

    for (i = 0; i < FA_NOTIFICATION_CALLBACKS; i++)
    {
        if ((faCallbackTable[i].tgtEvents == 0) && (faCallbackTable[i].callback == NULL))
        {
            faCallbackTable[i].callback = callback;
            faCallbackTable[i].tgtEvents = tgtEvents;
            faCallbackTable[i].context = context;
            break;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    if (i < FA_NOTIFICATION_CALLBACKS)
    {
        rc = i + 1;
    }

    return (rc);
}

/**************************************************************
 * Name: faUnregisterCallback
 * Description:
 *    Deregisters a callback routine with the FA agent based 
 *    on the callback ID returned following a successful 
 *    registration. Externally accessible routine.
 * Input Parameters:
 *    faCallbackId - callback routine ID returned during 
 *                   registration
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faUnregisterCallback (int faCallbackId)
{
    if (! INRANGE(1, faCallbackId, FA_NOTIFICATION_CALLBACKS))
    {
        return;
    }

    aasdkx_mutex_lock(faAgentMut);

    faCallbackTable[faCallbackId - 1].callback = NULL;
    faCallbackTable[faCallbackId - 1].tgtEvents = 0;
    faCallbackTable[faCallbackId - 1].context = NULL;

    aasdkx_mutex_unlock(faAgentMut);

    return;
}

/**************************************************************
 * Name: faGetRemoteServerInfo
 * Description:
 *    Returns current server data maintained by the local host.
 *    Externally accessible routine.
 * Input Parameters:
 *    serverInfo - pointer to remote server data info structure
 * Output Parameters:
 *    serverInfo - current remote server data
 * Return Values:
 *    0 - Server information not returned
 *    1 - Server information returned
 **************************************************************/

int
faGetRemoteServerInfo (faRemoteServerInfo_t *serverInfo)
{
    int rc = 0;
    unsigned int ifIndex=0; // TBD

    if (serverInfo != NULL) 
    {
        if (faAgentConfigData.faAgentPrimaryServerIdLen)
        {
            serverInfo->serverStatus = FA_REM_SERVER_STATUS_ACTIVE;

            if (faAgentConfigData.faAgentPrimaryServerTrunk)
            {
                serverInfo->serverUplinkType = FA_REM_SERVER_UPLINK_TRUNKID;
                serverInfo->serverUplinkId = faAgentConfigData.faAgentPrimaryServerTrunk;
            }
            else if ((faAgentConfigData.faAgentPrimaryServerPort - 1) == ifIndex)
             {
                serverInfo->serverUplinkType = FA_REM_SERVER_UPLINK_IFINDEX;
                serverInfo->serverUplinkId = ifIndex;
            }
            else
            {
                serverInfo->serverUplinkType = FA_REM_SERVER_UPLINK_UNKNOWN;
                serverInfo->serverUplinkId = 0;
            }
        }
        else
        {
            serverInfo->serverStatus = FA_REM_SERVER_STATUS_INACTIVE;
        }

        rc = 1;
    }

    return (rc);
}

