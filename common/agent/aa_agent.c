/* aa-sdk/common/agent/aa_agent.c */

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
#include <time.h>   /* struct timespec */
#include <stdio.h>

#include "aa_types.h"
#include "aa_defs.h"
#include "aa_sysdefs.h"
#include "aa_agent.h"
#include "aa_agent_private.h"

#include "aasdk_errno.h"
#include "aasdk_msgif_comm.h"
#include "aasdk_osif_comm.h"
#include "aasdk_lldpif_comm.h"


/* Fabric Attach Agent message queue definitions */
#define FA_AGT_TASK_QUEUE_SIZE 1024
aasdk_msgque_id_t faAgentMsgQ = 0;

/* Fabric Attach Agent general use mutex */
aasdk_mutex_id_t faAgentMut = 0;

/* Fabric Attach (FA) Agent configuration data */
faAgentConfigData_t faAgentConfigData;

/* Fabric Attach (FA) Agent statistics data */
faAgentStatsData_t faAgentStatsData;

aasdk_stats_data_t aaGlobalStats;

/* Timer expiration values that may be manipulated */
/* through the backdoor menu for testing purposes */

int faAgentPrimaryServerExpiration;
int faAgentRemoteIsidVlanAsgnExpiration;

FaNotificationEntry_t faCallbackTable[FA_NOTIFICATION_CALLBACKS];

/*** External declarations ***/

#ifdef SYSDEF_INCLUDE_MLT

#endif

extern void faAgentProcessStackOpsData ();

/*** Forward declarations ***/


#ifdef SYSDEF_INCLUDE_MLT








#endif

#ifdef SYSDEF_INCLUDE_FA_SMLT




#endif /* SYSDEF_INCLUDE_FA_SMLT */

/********************************************************************
Fabric Attach Agent support routines 
********************************************************************/

unsigned
faAgentTimeGet(void)
{
  struct timespec current;

  aasdku_time_get(&current);

  return current.tv_sec;
}



void
faAgentDelay(unsigned sec)
{
    struct timespec delay;

    delay.tv_sec = sec;
    delay.tv_nsec = 0;

    aasdkx_time_sleep(&delay);
}


int
faAgentServerCapable(void)
{
    int rc = 1;

    return (rc);
}


int
faAgentSetFaPortStatus (int faPortStatus)
{
    int rc = 1;

    bool aaPortsEna;

    aaPortsEna = (faPortStatus==FA_AGENT_FA_PORT_STATE_ENABLED) ? true : false;
    
    rc = (aaPortsStateSet(aaPortsEna) == AA_SDK_ENONE) ? SUCCESS : FAIL;

    return (rc);
}


/**************************************************************
* Name: faAgentPortFaEnabled
* Description:
*    Determines the FA status of an interface by querying
*    the FA LLDP TLV settings that indicate the status of
*    the FA support on that interface.
* Input Parameters:
*    ifIndex - target interface for update
* Output Parameters:
*    none
* Return Values:
*    0 - target interface is disabled or state unknown
*    1 - target interface is enabled based on TLV settings
**************************************************************/

int
faAgentPortFaEnabled (unsigned int ifIndex)
{
    int portEnabled = 0;
    aasdk_port_status_t aaPortStatus;

    if (aasdki_port_status_get(ifIndex,&aaPortStatus) == AA_SDK_ENONE)
    {
        if (aaPortStatus.enabled == true)
	{
            portEnabled = 1;
	}
    }

    return (portEnabled);
}

int
faAgentSetElementType (int elementType,
	       int saveToNv,
	       int distributeData,
	       int elementReset)
{
    int rc = 0;

    int faServiceEnableReqd = 0;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_2, FA_AGENT_INFO_MSG_4,
                    "faAgentSetElementType", elementType, saveToNv, 0, NULL);

    if (faAgentConfigData.faElementType == elementType)
    {
        return (1);
    }  

    if (INRANGE(FA_AGENT_ELEMENT_SERVER, elementType, FA_AGENT_ELEMENT_HOST))
    {
        /* If necessary, disable FA service and clear database prior */
        /* to updating the element type */

        if (faAgentGetServiceStatus() == FA_AGENT_STATE_ENABLED)
        {
            faAgentSetServiceStatus(FA_AGENT_STATE_DISABLED,
                                    FA_NO_NV_SAVE,
                                    FA_NO_DISTRIBUTE);
            faLocalIsidVlanAsgnsClearAll(FA_NOTIFY_LLDP);
            faServiceEnableReqd = 1;
        }

        faAgentConfigData.faElementType = elementType;

        aasdkx_disc_elem_type_set(elementType);

        if (elementReset == FA_RESET)
        {
            if (elementType == FA_AGENT_ELEMENT_HOST)
            {
	        faAgentSetHostProxyStatus(FA_AGENT_HOST_PROXY_ENABLED, FA_NO_NV_SAVE);
	        faAgentSetAutoAttachStatus(FA_AGENT_AA_STATE_ENABLED, FA_NO_NV_SAVE);
	        faAgentSetFaPortStatus(FA_AGENT_FA_PORT_STATE_ENABLED);
	        faAgentSetServiceStatus(FA_AGENT_STATE_ENABLED,FA_NO_NV_SAVE, FA_NO_DISTRIBUTE);
            }
            else
            {
	        faAgentSetHostProxyStatus(FA_AGENT_HOST_PROXY_DISABLED, FA_NO_NV_SAVE);
	        faAgentSetAutoAttachStatus(FA_AGENT_AA_STATE_DISABLED, FA_NO_NV_SAVE);
	        faAgentSetFaPortStatus(FA_AGENT_FA_PORT_STATE_DISABLED);
            }
        }
        else if (faServiceEnableReqd)
        {
            faAgentSetServiceStatus(FA_AGENT_STATE_ENABLED, FA_NO_NV_SAVE, FA_NO_DISTRIBUTE);
        }

        rc = 1;
    } 

    return (rc);
}

void
faAgentSetMgmtVlan (unsigned short mgmtVlan)
{
    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
		    "faAgentSetMgmtVlan", mgmtVlan, 0, 0, NULL);

    aasdkx_disc_mgmt_vlan_set(mgmtVlan);

    return;
}


void
faAgentProcessElementDiscovery (aasdk_msg_t *msg)
{
    unsigned int ifIndex;
    unsigned short elemType;
    unsigned short elemVlan;
    faDiscoveredElementsEntry_t *faDiscElem = NULL;

    unsigned char chassisId[FA_AGENT_FA_ELEMENT_TLV_CHASSIS_ID];

    if (msg == NULL)
    {
        return;
    }

    ifIndex = msg->arg0 & 0x0000ffff;
    elemType = (msg->arg0 >> 28) & 0x0000000f;
    elemVlan = (msg->arg0 >> 16) & 0x00000fff;

    /* FA Element TLV Chassis ID is 10 octets */
    memcpy(&chassisId[0], &msg->arg1, sizeof(unsigned int));
    memcpy(&chassisId[4], &msg->arg2, sizeof(unsigned int));
    memcpy(&chassisId[8], &msg->arg3, sizeof(unsigned short));

    aasdkx_mutex_lock(faAgentMut);

    /* Determine if the discovered FA element is already */
    /* known or not */
    faDiscElem = faDiscoveredElementsFind(ifIndex, FA_AGENT_GET_EXACT);

    if (faDiscElem != NULL)
    {
        /* Update */
        faDiscElem->elemType = elemType;
        faDiscElem->elemVlan = elemVlan;
        faDiscElem->lastUpdated = faAgentTimeGet();

        faDiscElem->chassisIdLen = FA_AGENT_FA_ELEMENT_TLV_CHASSIS_ID;
        memcpy(faDiscElem->chassisId, chassisId, FA_AGENT_FA_ELEMENT_TLV_CHASSIS_ID);
    }
    else
    {
        /* Add element to the discovered FA elements list */
        faDiscoveredElementsCreate(ifIndex, elemType, elemVlan, 
                                   chassisId, FA_AGENT_FA_ELEMENT_TLV_CHASSIS_ID);
    } 
    aasdkx_mutex_unlock(faAgentMut);

    return;
}

/**************************************************************
* Name: faAgentSyncLldpFaData
* Description:
*    Walks the LLDP FA database and checks that all elements
*    have a corresponding entry in the local FA database. If
*    not, the LLDP FA entry potentially represents obsolete 
*    data and it is deleted.
* Input Parameters:
*    none
* Output Parameters:
*    none
* Return Values:
*    none
**************************************************************/

void
faAgentSyncLldpFaData (void)
{
    return;
}

/**************************************************************
* Name: faAgentPerformGarbageCollection
* Description:
*    The garbage collection routine is invoked every 30
*    seconds to perform whatever tasks are necessary.
*    Called during FA maintenance timer event processing.
* Input Parameters:
*    none
* Output Parameters:
*    none
* Return Values:
*    none
**************************************************************/

void
faAgentPerformGarbageCollection ()
{
#if SYSDEF_PLATFORM == SYSDEF_PLATFORM_ERS

static int thirtySecondTimer = 0;
unsigned short mgmtVlan;

thirtySecondTimer++;

if (! (thirtySecondTimer % 10))
{
/* Force a refresh of the discovered elements */
faDiscoveredElementsClearAll();

/* Clean-up LLDP FA database if necessary */
if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
{
    faAgentSyncLldpFaData();
}
}

/* Poll management VLAN data at regular intervals if AA is enabled */
/* and update the FA LLDP info if necessary */
if (faAgentConfigData.faAutoAttachStatus == FA_AGENT_AA_STATE_ENABLED)
{
faAgentGetMgmtVlan(&mgmtVlan);

if (mgmtVlan != faAgentConfigData.faMgmtVlan)
{
    faAgentConfigData.faMgmtVlan = mgmtVlan;
    faAgentSetMgmtVlan(mgmtVlan);
}
}
#endif /* PLATFORM == ERS */

return;
}

void
faAgentInit ()
{
    /* Initialize default data */
    memset(&faAgentConfigData, 0, sizeof(faAgentConfigData_t));

    /* Initialize FA callback table */
    memset(faCallbackTable, 0, (sizeof(FaNotificationEntry_t) * FA_NOTIFICATION_CALLBACKS));

    /* Service is disabled at initialization in AA SDK */

    faAgentConfigData.faServiceStatus = FA_AGENT_STATE_DISABLED;
    faAgentConfigData.faAutoAttachStatus = FA_AGENT_AA_STATE_DISABLED;
    faAgentConfigData.faHostProxyStatus = FA_AGENT_HOST_PROXY_DISABLED;

    faAgentConfigData.faAgentPrimaryServerIdLen = 0;
    faAgentConfigData.faAgentPrimaryServerDescrLen = 0;
    faAgentConfigData.faAgentPrimaryServerLastUpdated = 0;
    memset(faAgentConfigData.faAgentPrimaryServerId, 0, FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN);
    memset(faAgentConfigData.faAgentPrimaryServerDescr, 0, FA_AGENT_MAX_PRIMARY_SERVER_DESCR_LEN);

    faAgentConfigData.faMgmtVlan = FA_AGENT_AA_VLAN_NOOP;
    faAgentConfigData.faCleanupMode = FA_AGENT_CLEANUP_MODE_ENABLED;

#ifdef SYSDEF_INCLUDE_SSH

    faAgentConfigData.faMsgAuthStatus = FA_AGENT_MSG_AUTH_ENABLED;
    faAgentConfigData.faMsgAuthKeyLength = strlen(FA_AGENT_MSG_AUTH_KEY_DEFAULT);
    memcpy(faAgentConfigData.faMsgAuthKey, FA_AGENT_MSG_AUTH_KEY_DEFAULT,
                                    strlen(FA_AGENT_MSG_AUTH_KEY_DEFAULT));

#else  /* SYSDEF_INCLUDE_SSH */

    faAgentConfigData.faMsgAuthStatus = FA_AGENT_MSG_AUTH_DISABLED;
    faAgentConfigData.faMsgAuthKeyLength = 0;

#endif /* SYSDEF_INCLUDE_SSH */

    faAgentConfigData.faLocalIsidVlanAsgnsCount = 0;
    faAgentConfigData.faLocalIsidVlanAsgnsTable = NULL;
    faAgentConfigData.faRemoteIsidVlanAsgnsCount = 0;
    faAgentConfigData.faRemoteIsidVlanAsgnsTable = NULL;

    faAgentConfigData.faDiscoveredElementsCount = 0;
    faAgentConfigData.faDiscoveredElements = NULL;

    faAgentConfigData.faAgentDisplayInfoMsgs = 0;   /* Info msgs off */

    faAgentConfigData.faAgentState = 0;

    faAgentConfigData.aaPortsList = NULL;
    faAgentConfigData.aaPortsCount = 0;


#ifdef FA_AGENT_DEMO_SUPPORT_ENABLED

/********************************************************/
/* Demo feature support                                 */
/*   -- Hide CLI commands and restrict LLDP options     */
/*   -- Start with FA per-port settings disabled        */
/********************************************************/

    faAgentSetFaPortStatus(FA_AGENT_FA_PORT_STATE_DISABLED);

#endif /* FA_AGENT_DEMO_SUPPORT_ENABLED */

    /* Allow external applications time to settle down (LLDP, SPBM) */
    faAgentDelay(TIME_5S);


    /*  use default data */
    /* and SPBM data to initialize the LLDP FA data */

#ifdef SYSDEF_INCLUDE_SPBM















#endif /* SYSDEF_INCLUDE_SPBM */

    faAgentPrimaryServerExpiration = FA_AGENT_PRIMARY_SERVER_EXPIRATION;
    faAgentRemoteIsidVlanAsgnExpiration = FA_REMOTE_ISID_VLAN_ASGN_EXPIRATION;


    /* Retrieve management VLAN data if AA is enabled. Initialize FA LLDP */
    /* with current management VLAN or "disabled" (0) value */
    if (faAgentConfigData.faAutoAttachStatus == FA_AGENT_AA_STATE_ENABLED)
    {
        faAgentGetMgmtVlan(&faAgentConfigData.faMgmtVlan);
    }

    faAgentSetMgmtVlan(faAgentConfigData.faMgmtVlan);

    memset(&faAgentStatsData, 0, sizeof(faAgentStatsData_t));

    memset(&aaGlobalStats, 0, sizeof(aaGlobalStats));
    aaGlobalStats.aasdk_port_id = -1;


#ifdef SYSDEF_INCLUDE_SPBM
















#endif /* SYSDEF_INCLUDE_SPBM */


#ifdef SYSDEF_INCLUDE_MLT
/* Register for MLT events */
#endif

    return;

} /* faAgentInit() */

/**************************************************************
* Name: faAgent_main
* Description:
*    Initiates Fabric Attach Agent initialization and 
*    processes messages generated by and related to the 
*    agent. Task spawned during system start-up by 
*    FA_AGT_init().
* Input Parameters:
*    none
* Output Parameters:
*    none
* Return Values:
*    0 - Fabric Attach Agent initialization/processing failure
*    Otherwise, never returns
**************************************************************/


void
faAgent_main_init()
{
    /* Configure default settings that may impact initial cleanup */
    faAgentConfigData.faCleanupMode = FA_AGENT_CLEANUP_MODE_ENABLED;

    faAgentInit();
}



int
faAgent_main_proc(aasdk_msg_t *pmsg)
{
    aasdk_msg_t msg = *pmsg;
    unsigned short mgmtVlan;
    unsigned int   elemType;
    faMinIsidVlanBindingEntry_t minAsgn;


    int rc = SUCCESS;


    faAgentErrorMsg(FA_AGENT_INFO_MSG_RECEIVED, FA_AGENT_INFO_MSG_5,
                    NULL, msg.msgId, 0, 0, NULL);

    switch (msg.msgId)
    {


    case FA_AGENT_MSG_LIST_MAINT_TIMER:

	/* Timer updates should only be processed by the master */
	/* agent (standalone agent or BU agent when devices are */
	/* stacked) for consistency */

        if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
	{ 
#ifdef FA_AGENT_HOST_PROXY_SUPPORT

	    faAgentPerformServerMaintenance(FALSE);

	    /* Proxy support utilizes the remote assignment database */
	    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
	    {
	        faAgentPerformProxyListMaintenance(faAgentRemoteIsidVlanAsgnExpiration, 0);
	    }

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

	}
	else if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER)
	{
	    faAgentPerformRemoteListMaintenance(faAgentRemoteIsidVlanAsgnExpiration, 0);
	}

	faAgentPerformGarbageCollection();

	break;


#if FA_DISCOVERY

    case FA_AGENT_MSG_LLDP_DISCOVERY_UPDATE:

	/* Ignore LLDP updates if the service is disabled */
	if (! faAgentServiceEnabled())
	{
	    break;
	}

	/* Ignore LLDP updates if the port is FA disabled */
	if (! faAgentPortFaEnabled(msg.arg0 & 0x0000ffff))
	{
	    break;
	}

	elemType = ((msg.arg0 >> 28) & 0x0000000f);

	/* Element discovery processing differs based on */
	/* operational state */
	if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
	{

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

	    if (elemType == FA_AGENT_ELEMENT_SERVER)
	    {
	        faAgentProcessServerDiscovery(&msg);
	    }

	    if ((FA_ELEM_EXT_CLIENT(elemType)) &&
	        (faAgentConfigData.faAutoAttachStatus == FA_AGENT_AA_STATE_ENABLED) &&
		    (faAgentConfigData.faHostProxyStatus == FA_AGENT_HOST_PROXY_ENABLED))
	    {
	        faAgentGetMgmtVlan(&mgmtVlan);
		faDownlinkAutoAttachProcessing(mgmtVlan, elemType, (msg.arg0 & 0x0000ffff));
	    }

	    faAgentProcessElementDiscovery(&msg);

#endif /* def HOST_PROXY */
	}
	else  /* faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER */
	{
	   if (faAgentConfigData.faAutoAttachStatus == FA_AGENT_AA_STATE_ENABLED)
	   {
	       faAgentGetMgmtVlan(&mgmtVlan);
	       faDownlinkAutoAttachProcessing(mgmtVlan, elemType, (msg.arg0 & 0x0000ffff));
	    }

	    faAgentProcessElementDiscovery(&msg);
	}
	break;

#endif /* FA_DISCOVERY */


#if FA_ASSIGNMENT
    case FA_AGENT_MSG_LLDP_BINDING_UPDATE:

	/* Ignore LLDP updates if the service is disabled */
	if (! faAgentServiceEnabled())
	{
	    break;
	}


        /* Ignore LLDP updates if the port is FA disabled */
	if (! faAgentPortFaEnabled(msg.arg0 & 0x0000ffff))
	{
	    break;
	}

	minAsgn.isid   = msg.arg1;
	minAsgn.vlan   = msg.arg2;
	minAsgn.status = msg.arg4;

	faUpdateRemoteIsidVlanAsgns(msg.arg0, 1, &minAsgn);

	break;
#endif /* def FA_ASSIGNMENT */


#ifdef SYSDEF_INCLUDE_SPBM





#endif /* INCLUDE_SPBM */


#ifdef SYSDEF_INCLUDE_FA_SMLT











#endif /* INCLUDE_FA_SMLT */

#ifdef SYSDEF_INCLUDE_FA_MLT






#endif /* INCLUDE_FA_MLT */


    default:

	faAgentErrorMsg(FA_AGENT_INFO_UNK_MSG_RECEIVED, FA_AGENT_INFO_MSG_1,
                        NULL, msg.msgId, 0, 0, NULL);

	rc = FAIL;
	break;

    } /* switch (msgId) */

    return rc;

} /* faAgent_main_proc */


int
faAgent_main ()
{
    aasdk_msg_t msg;

    faAgent_main_init();

    while (1)
    {
        if (aasdkx_msg_recv(faAgentMsgQ, &msg) == AA_SDK_ENONE)
        {
            faAgent_main_proc(&msg);
        }
    }
    return 0;
}


int
FA_AGT_init (task_spawn_t *tsInfo)
{
    int faAgentTid = 0;

    /* Create the Fabric Attach Agent message processing queue */
    faAgentMsgQ = aasdkx_msgque_create(FA_AGT_TASK_QUEUE_SIZE);

    /* Create the Fabric Attach Agent general use mutex */
    aasdkx_mutex_create(&faAgentMut);

    return (faAgentTid);
}

/**************************************************************
* Name: faAgentProcessTimer
* Description:
*    Generates a message for the FA agent based on a
*    timer expiration. Timer expiration may signal 
*    several events at one time, depending on the timer
*    being processed.
* Input Parameters:
*    timerId - timer_t structure
*    usrData - provided timer descriptor
* Output Parameters:
*    none
* Return Values:
*    none
**************************************************************/

void
faAgentProcessTimer (int timerId, int usrData)
{
    aasdk_msg_t msg;

    /* Compiler happiness */
    (void ) timerId;

    memset(&msg, 0, sizeof(msg));

    msg.msgId = usrData;
    msg.arg0 = (unsigned int)0;
    msg.arg1 = (unsigned int)0;
    msg.arg2 = (unsigned int)0;

    aasdkx_msg_send(faAgentMsgQ, &msg);

    return;
}

/**************************************************************
* Name: faAgentSignalEvent
* Description:
*    Generates a message for the FA agent.
* Input Parameters:
*    eventId - event indicator
*    eventParam1 - event parameter
*    eventParam2 - event parameter
*    eventParam3 - event parameter
* Output Parameters:
*    none
* Return Values:
*    none
**************************************************************/

void
faAgentSignalEvent (unsigned int eventId,
	    unsigned int eventParam1,
	    unsigned int eventParam2,
	    unsigned int eventParam3)
{
    aasdk_msg_t msg;

    memset(&msg, 0, sizeof(msg));

    msg.msgId = eventId;
    msg.arg0 = eventParam1;
    msg.arg1 = eventParam2;
    msg.arg2 = eventParam3;

    aasdkx_msg_send(faAgentMsgQ, &msg);

    return;
}



/********************************************************************
 Fabric Attach Agent I-SID/VLAN Assignment Table support routines
 ********************************************************************/

/**************************************************************
 * Name: faLocalIsidVlanAsgnUpdateState
 * Description:
 *    Traverses the list of local I-SID/VLAN assignments looking
 *    for a match based on the provided I-SID/VLAN data. If found,
 *    the element state is updated based on the 'state' and update
 *    type (SET/CLEAR) that is specified.
 * Input Parameters:
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    state - state indicator
 *    update - operation (SET or CLEAR)
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/




void
faLocalIsidVlanAsgnUpdateState (unsigned int isid,
                                unsigned int vlan,
                                unsigned int update,
                                unsigned char state)
{
#if FA_LOC_ASSIGNMENT

    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if ((faIsidVlanAsgn->isid == isid) && (faIsidVlanAsgn->vlan == vlan))
            {
                if (update == FA_ELEM_STATE_UPDATE_SET)
                {
                    faIsidVlanAsgn->elemState |= state;
                }
                else if (update == FA_ELEM_STATE_UPDATE_CLEAR)
                {
                    faIsidVlanAsgn->elemState &= ~state;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_lock(faAgentMut);

#endif

    return;
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnLexiOrder
 * Description:
 *    Determines the lexicographical order of the elements 
 *    represented by the specified I-SID/VLAN data.
 * Input Parameters:
 *    isidA - I-SID for element A
 *    vlanA - VLAN for element A
 *    isidB - I-SID for element B
 *    vlanB - VLAN for element B
 * Output Parameters:
 *    none
 * Return Values:
 *   -1 - element A is lexi-less than element B
 *    0 - element A is lexi-equal to element B
 *    1 - element A is lexi-greater than element B
 **************************************************************/

int
faLocalIsidVlanAsgnLexiOrder (unsigned int isidA,
                              unsigned int vlanA,
                              unsigned int isidB,
                              unsigned int vlanB)
{
    int rc = 0;

#if FA_LOC_ASSIGNMENT

    if (isidA < isidB)
        rc = FA_AGENT_LEXI_LT;
    else if (isidA > isidB)
        rc = FA_AGENT_LEXI_GT;
    else
    {
        if (vlanA < vlanB)
            rc = FA_AGENT_LEXI_LT;
        else if (vlanA > vlanB)
            rc = FA_AGENT_LEXI_GT;
        else
            rc = FA_AGENT_LEXI_EQ;
    }

#endif

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnsClearAll
 * Description:
 *    Deletes all local I-SID/VLAN assignment data.
 * Input Parameters:
 *    notifyLldp - indicates if an LLDP update is required
 * Output Parameters:
 *    none
 * Return Values:
 *    Count - number of elements deleted
 **************************************************************/

int
faLocalIsidVlanAsgnsClearAll (int notifyLldp)
{
    int count = 0;

#if FA_ASSIGNMENT

    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn, *tmpFaIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                    "faLocalIsidVlanAsgnsClearAll", notifyLldp, 0, 0, NULL);

    /* Clear any lingering LLDP data (LLDP 'delete all') */

    if (notifyLldp == FA_NOTIFY_LLDP)
    {

    }

    if ((faAgentConfigData.faLocalIsidVlanAsgnsTable == NULL) ||
        (faAgentConfigData.faLocalIsidVlanAsgnsCount <= 0))
    {
        return (count);
    }

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;

    while (faIsidVlanAsgn != NULL)
    {
        tmpFaIsidVlanAsgn = faIsidVlanAsgn->next;
        aasdkx_mem_free(faIsidVlanAsgn);
        count++;
        faIsidVlanAsgn = tmpFaIsidVlanAsgn;
    }

    faAgentConfigData.faLocalIsidVlanAsgnsTable = NULL;
    faAgentConfigData.faLocalIsidVlanAsgnsCount = 0;

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return (count);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnFind
 * Description:
 *    Traverses the list of I-SID/VLAN assignments looking for
 *    a match based on the provided I-SID and VLAN data.
 * Input Parameters:
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 * Output Parameters:
 *    none
 * Return Values:
 *    NULL - target entry not found
 *    faLocalIsidVlanAsgnsEntry_t * - target entry found
 **************************************************************/

faLocalIsidVlanAsgnsEntry_t *
faLocalIsidVlanAsgnFind (unsigned int isid,
                         unsigned int vlan,
                         int searchType)
{

    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn = NULL;

#if FA_LOC_ASSIGNMENT
    int rc;


    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            rc = faLocalIsidVlanAsgnLexiOrder(faIsidVlanAsgn->isid, 
                                              faIsidVlanAsgn->vlan,
                                              isid, vlan);
    
            if ((searchType == FA_AGENT_GET_EXACT) && (rc == FA_AGENT_LEXI_EQ))
            {
                /* Entry found */
                break;
            }
            else if ((searchType == FA_AGENT_GET_NEXT) && (rc == FA_AGENT_LEXI_GT))
            {
                /* Entry found */
                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

#endif
    return (faIsidVlanAsgn);
}




/**************************************************************
 * Name: faLocalIsidVlanAsgnAdd
 * Description:
 *    Adds an element to the list of I-SID/VLAN assignments
 *    based on the element's I-SID and VLAN data.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faLocalIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to add element to list
 *    1 - element addition successful
 **************************************************************/

unsigned int
faLocalIsidVlanAsgnAdd (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
#if FA_LOC_ASSIGNMENT

    int rc;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    if ((faIsidVlanAsgnData == NULL) || 
        (faAgentConfigData.faLocalIsidVlanAsgnsCount >= FA_AGENT_MAX_ISID_VLAN_ASGNS))
    {
        return (0);
    }

    /* Locate addition position - list is sorted */

    if (faAgentConfigData.faLocalIsidVlanAsgnsTable == NULL)
    {
        /* List is empty */

        faAgentConfigData.faLocalIsidVlanAsgnsTable = faIsidVlanAsgnData;
        faIsidVlanAsgnData->next = NULL;
        faIsidVlanAsgnData->prev = NULL;

        faAgentConfigData.faLocalIsidVlanAsgnsCount++;
    }
    else
    { 
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            rc = faLocalIsidVlanAsgnLexiOrder(faIsidVlanAsgnData->isid, 
                                              faIsidVlanAsgnData->vlan,
                                              faIsidVlanAsgn->isid, 
                                              faIsidVlanAsgn->vlan);
    
            if (rc == FA_AGENT_LEXI_LT)
            {
                /* Insert before current element */

                if (faIsidVlanAsgn == faAgentConfigData.faLocalIsidVlanAsgnsTable)
                {
                    /* First element in list */

                    faAgentConfigData.faLocalIsidVlanAsgnsTable = faIsidVlanAsgnData;
                    faIsidVlanAsgnData->next = faIsidVlanAsgn;
                    faIsidVlanAsgnData->prev = NULL;
                    faIsidVlanAsgn->prev = faIsidVlanAsgnData;
                }
                else
                {
                    faIsidVlanAsgnData->prev = faIsidVlanAsgn->prev;
                    faIsidVlanAsgn->prev->next = faIsidVlanAsgnData;
                    faIsidVlanAsgnData->next = faIsidVlanAsgn;
                    faIsidVlanAsgn->prev = faIsidVlanAsgnData;
                }
    
                faAgentConfigData.faLocalIsidVlanAsgnsCount++;
    
                break;
            }
            else if (rc == FA_AGENT_LEXI_EQ)
            {
                /* Exact match found - should not occur */
                break;
            }

            /* Reached the end of the list? */
            if (faIsidVlanAsgn->next == NULL)
            {
                faIsidVlanAsgn->next = faIsidVlanAsgnData;
                faIsidVlanAsgnData->prev = faIsidVlanAsgn;
                faIsidVlanAsgnData->next = NULL;
    
                faAgentConfigData.faLocalIsidVlanAsgnsCount++;
    
                break;
            }
    
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

#endif

    return (1);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnDelete
 * Description:
 *    Deletes an element from the list of I-SID/VLAN assignments
 *    based on the element's I-SID and VLAN data. If found, the 
 *    target element memory is freed.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faLocalIsidVlanAsgnsEntry_t
 *    notifyLldp - indicates if an LLDP update is required
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to delete element from list
 *    1 - element deletion successful
 **************************************************************/

int
faLocalIsidVlanAsgnDelete (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                           int notifyLldp)
{
    int rc = 0;
#if FA_LOC_ASSIGNMENT

    faLocalIsidVlanAsgnsEntry_t tmpFaIsidVlanAsgn;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;
    faLocalIsidVlanAsgnsEntry_t *tmpFaIsidVlanAsgnPtr;
    LLDP_AVAYA_FA_ISID_VLAN_ASGNS_GROUP_T lldpFaIsidVlanAsgnData;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                    "faLocalIsidVlanAsgnDelete", notifyLldp, 0, 0, NULL);

    if ((faIsidVlanAsgnData == NULL) || 
        (faAgentConfigData.faLocalIsidVlanAsgnsTable == NULL) ||
        (faAgentConfigData.faLocalIsidVlanAsgnsCount <= 0))
    {
        return (0);
    }

    /* Search for target element */

    faIsidVlanAsgn = NULL;

    aasdkx_mutex_lock(faAgentMut);

    tmpFaIsidVlanAsgnPtr = faLocalIsidVlanAsgnFind(faIsidVlanAsgnData->isid,
                                                   faIsidVlanAsgnData->vlan,
                                                   FA_AGENT_GET_EXACT);

    if (tmpFaIsidVlanAsgnPtr != NULL)
    {
        memcpy((char *)&tmpFaIsidVlanAsgn, (char *)tmpFaIsidVlanAsgnPtr, 
               sizeof(faLocalIsidVlanAsgnsEntry_t));

        faIsidVlanAsgn = &tmpFaIsidVlanAsgn;
    }

    aasdkx_mutex_unlock(faAgentMut);

    if (faIsidVlanAsgn != NULL)
    {
        /* Take care of any cleanup that is necessary first */
        if (! faHostAsgnRejectionProcessing(faIsidVlanAsgn,
                                            faIsidVlanAsgn->ifIndex, 
                                            FA_AGENT_ISID_VLAN_ASGN_PENDING))
        {
            faAgentErrorMsg(FA_AGENT_ERR_ASGN_RESET_ISSUE, FA_AGENT_INFO_MSG_1,
                            NULL, 1, 0, 0, NULL);

            faAgentStatsData.faAgentHostCleanupIssues++;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    faIsidVlanAsgn = faLocalIsidVlanAsgnFind(faIsidVlanAsgnData->isid,
                                             faIsidVlanAsgnData->vlan,
                                             FA_AGENT_GET_EXACT);

    if (faIsidVlanAsgn != NULL)
    {
        if (faIsidVlanAsgn == faAgentConfigData.faLocalIsidVlanAsgnsTable)
        {
            /* First element in list */

            faAgentConfigData.faLocalIsidVlanAsgnsTable = faIsidVlanAsgn->next;

            if (faIsidVlanAsgn->next != NULL)
            {
                faIsidVlanAsgn->next->prev = NULL;
            }
        }
        else
        {
            faIsidVlanAsgn->prev->next = faIsidVlanAsgn->next;

            if (faIsidVlanAsgn->next != NULL)
            {
                faIsidVlanAsgn->next->prev = faIsidVlanAsgn->prev;
            }
        }
    
        faAgentConfigData.faLocalIsidVlanAsgnsCount--;

        /* Update the LLDP database if necessary */

        if (notifyLldp)
        {


        }

        faAgentErrorMsg(FA_AGENT_INFO_LOCAL_DELETE, FA_AGENT_INFO_MSG_2,
                        faIsidVlanAsgn->isid, faIsidVlanAsgn->vlan, 0, 0, NULL);

        aasdkx_mem_free(faIsidVlanAsgn);

        rc = 1;
    }
    
    aasdkx_mutex_unlock(faAgentMut);

#endif

    return (rc);
}

void
faLocalIsidVlanAsgnDump ()
{
#if FA_LOC_ASSIGNMENT
    char buffer[16];
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    printf("\n\nfaLocalIsidVlanAsgnDump: element list (count %d current time %d)\n\n",
           faAgentConfigData.faLocalIsidVlanAsgnsCount, faAgentTimeGet());

    aasdkx_mutex_lock(faAgentMut);

    if (faAgentConfigData.faLocalIsidVlanAsgnsTable == NULL)
    {
        printf("Empty\n\n");
    }
    else
    {
        printf(" I-SID    VLAN    Status    Components  IfIndex  Trunk  ElemState   Updated\n");
        printf("--------  ----  ----------  ----------  -------  -----  ---------  ---------\n");

        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_PENDING)
                sprintf(buffer, "Pending(%d)    ", faIsidVlanAsgn->status);
            else if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
                sprintf(buffer, "Active(%d)     ", faIsidVlanAsgn->status);
            else
            {
                if (faIsidVlanAsgn->status < 10)
                    sprintf(buffer, "Reject(%d)     ", faIsidVlanAsgn->status);
                else
                    sprintf(buffer, "Reject(%d)    ", faIsidVlanAsgn->status);
            }

            printf("%-8d  %-4d  %s",
                   faIsidVlanAsgn->isid, faIsidVlanAsgn->vlan, buffer);

            if (faIsidVlanAsgn->components == FA_COMP_NONE)
                printf("none ");
            else
            {
                if (faIsidVlanAsgn->components & FA_COMP_VLAN)
                    printf("V");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_MEMBER)
                    printf("M");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_TAG)
                    printf("T");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_ISID)
                    printf("I");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_PORT_PVID)
                    printf("P");
                else
                    printf(" ");
            }
 
            if (faIsidVlanAsgn->referenceCount < 10)
                printf("%9d(%d)    ", faIsidVlanAsgn->ifIndex, faIsidVlanAsgn->referenceCount);
            else
                printf("%9d(%d)   ", faIsidVlanAsgn->ifIndex, faIsidVlanAsgn->referenceCount);

            printf("%-2d     ", faIsidVlanAsgn->trunkId);

            printf("0x%02x/%s", faIsidVlanAsgn->elemState,
                   (faIsidVlanAsgn->origin == FA_ELEM_ORIGIN_PROXY) ? "P" : "C");

            printf("    %d\n", faIsidVlanAsgn->lastUpdated);

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }

        printf("\n");
        printf("Components: V-VLAN, M-Membership, T-Tagging, I-ISID, P-PVID\n");
        printf("ElemState/Origin: P-Proxy, C-Client (proxy)\n");
        printf("Note: index data includes client (proxy) instance count.\n");
        printf("\n");
    }

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return;
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnValidate
 * Description:
 *    Performs various tests to determine if element creation
 *    is allowed based on multiple factors (current element count,
 *    element data, etc.).
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faLocalIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    None
 * Return Values:
 *    FA_AGENT_ISID_VLAN_ASGN_PENDING - validation success
 *    >= FA_AGENT_ISID_VLAN_ASGN_REJECTED - validation failure
 **************************************************************/

faIsidVlanAsgnStates
faLocalIsidVlanAsgnValidate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
#if FA_LOC_ASSIGNMENT

    if (faIsidVlanAsgnData == NULL)
    {
        return (FA_AGENT_ISID_VLAN_ASGN_REJECTED);
    }

    /* Check if resources are available */
    if (faAgentConfigData.faLocalIsidVlanAsgnsCount >= FA_AGENT_MAX_ISID_VLAN_ASGNS)
    {
        return (FA_ASGN_REJECTION_RESOURCE_ERR);
    }

    /* Check for data conflicts */
    if (! faLocalIsidVlanAsgnConflict(faIsidVlanAsgnData->ifIndex,
                                      faIsidVlanAsgnData->isid,
                                      faIsidVlanAsgnData->vlan,
                                      faIsidVlanAsgnData->origin))
    {
        return (FA_ASGN_REJECTION_DUPLICATE_ERR);
    }

#endif

    return (FA_AGENT_ISID_VLAN_ASGN_PENDING);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnCreate
 * Description:
 *    Allocates I-SID/VLAN assignment element memory and adds 
 *    the element to the list of current assignments. 
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faLocalIsidVlanAsgnsEntry_t
 *    notifyLldp - indicates if an LLDP update is required
 *    validateData - indicates if data validation is required
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - element creation failed
 *    1 - element creation successful
 **************************************************************/

int
faLocalIsidVlanAsgnCreate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                           int notifyLldp, int validateData)
{
    int rc = 0;

#if FA_LOC_ASSIGNMENT

    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                    "faLocalIsidVlanAsgnCreate", notifyLldp, 0, 0, NULL);

    if ((validateData == FA_VALIDATE_DATA) &&
        (faLocalIsidVlanAsgnValidate(faIsidVlanAsgnData) != FA_AGENT_ISID_VLAN_ASGN_PENDING))
    {
        return (0);
    }

    aasdkx_mutex_lock(faAgentMut);

    /* Allocate memory for element tracking */
    faIsidVlanAsgn = (faLocalIsidVlanAsgnsEntry_t *) aasdkx_mem_alloc(
               sizeof(faLocalIsidVlanAsgnsEntry_t));

    if (faIsidVlanAsgn != NULL)
    {
        memset(faIsidVlanAsgn, 0, sizeof(faLocalIsidVlanAsgnsEntry_t));

        /* Set-up element data */
        faIsidVlanAsgn->isid = faIsidVlanAsgnData->isid;
        faIsidVlanAsgn->vlan = faIsidVlanAsgnData->vlan;
        faIsidVlanAsgn->status = FA_AGENT_ISID_VLAN_ASGN_PENDING;
        faIsidVlanAsgn->components = FA_COMP_NONE;
        faIsidVlanAsgn->elemState = FA_ELEM_STATE_NONE;
        faIsidVlanAsgn->origin = ((faIsidVlanAsgnData->origin == FA_ELEM_ORIGIN_CLIENT) ? 
                                  FA_ELEM_ORIGIN_CLIENT : FA_ELEM_ORIGIN_PROXY);
        faIsidVlanAsgn->referenceCount = faIsidVlanAsgnData->referenceCount;

        /* Add the element to the list */
        if (faLocalIsidVlanAsgnAdd(faIsidVlanAsgn))
        {
            /* Update the LLDP database if necessary */
            if (notifyLldp)
            {
            }

            faAgentErrorMsg(FA_AGENT_INFO_LOCAL_CREATE, FA_AGENT_INFO_MSG_2,
                            faIsidVlanAsgnData->isid, faIsidVlanAsgnData->vlan, 0, 0, NULL);

            rc = 1;
        }
        else
        {
            /* Addition failure - free allocated memory */
            aasdkx_mem_free(faIsidVlanAsgn);
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnUpdate
 * Description:
 *    Updates LLDP FA data for a single local I-SID/VLAN
 *    assignment entry.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faLocalIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - element update failed
 *    1 - element update successful
 **************************************************************/

int
faLocalIsidVlanAsgnUpdate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
    int rc = 0;

#if FA_LOC_ASSIGNMENT

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faLocalIsidVlanAsgnUpdate", 0, 0, 0, NULL);

    if (faIsidVlanAsgnData == NULL)
    {
        return (rc);
    }
#endif

    return (rc);
}

/**************************************************************
 * Name: faLocalIsidVlanAsgnSync
 * Description:
 *    Synchronizes the local I-SID/VLAN databases (FA --> LLDP).
 * Input Parameters:
 *    syncState - indicates if agent state data should be synced 
 *                as well.
 *    clearLldpData - indicates if LLDP data should be cleared
 *                    before synchronization is performed
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faLocalIsidVlanAsgnSync (int syncState,
                         int clearLldpData)
{
#if FA_LOC_ASSIGNMENT

    int failureSeen = 0;
    faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faLocalIsidVlanAsgnSync", 0, 0, 0, NULL);

    aasdkx_mutex_lock(faAgentMut);

    /* Start by clearing all LLDP entries, if necessary */

    if (clearLldpData)
    {
            faAgentErrorMsg(FA_AGENT_INFO_LLDP_DELETE, FA_AGENT_INFO_MSG_1, 0, 0, 0, 0, NULL);
        }
    }

    if (faAgentConfigData.faLocalIsidVlanAsgnsTable != NULL)
    {
        faIsidVlanAsgn = faAgentConfigData.faLocalIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    /* Update agent state attributes, if necessary */

    if (syncState)
    {

    }

    aasdkx_mutex_unlock(faAgentMut);

    if (failureSeen)
    {
        faAgentErrorMsg(FA_AGENT_INFO_LLDP_ADD, FA_AGENT_INFO_MSG_1, 0, 0, 0, 0, NULL);
    }

#endif

    return;
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnLexiOrder
 * Description:
 *    Determines the lexicographical order of the elements 
 *    represented by the specified ifIndex/I-SID/VLAN data.
 * Input Parameters:
 *    ifIndexA - ifIndex for element A
 *    isidA - I-SID for element A
 *    vlanA - VLAN for element A
 *    ifIndexB - ifIndex for element B
 *    isidB - I-SID for element B
 *    vlanB - VLAN for element B
 * Output Parameters:
 *    none
 * Return Values:
 *   -1 - element A is lexi-less than element B
 *    0 - element A is lexi-equal to element B
 *    1 - element A is lexi-greater than element B
 **************************************************************/

int
faRemoteIsidVlanAsgnLexiOrder (unsigned int ifIndexA,
                               unsigned int isidA,
                               unsigned int vlanA,
                               unsigned int ifIndexB,
                               unsigned int isidB,
                               unsigned int vlanB)
{
    int rc;

    if (ifIndexA < ifIndexB)
        rc = FA_AGENT_LEXI_LT;
    else if (ifIndexA > ifIndexB)
        rc = FA_AGENT_LEXI_GT;
    else
    {
        if (isidA < isidB)
            rc = FA_AGENT_LEXI_LT;
        else if (isidA > isidB)
            rc = FA_AGENT_LEXI_GT;
        else
        {
            if (vlanA < vlanB)
                rc = FA_AGENT_LEXI_LT;
            else if (vlanA > vlanB)
                rc = FA_AGENT_LEXI_GT;
            else
                rc = FA_AGENT_LEXI_EQ;
        }
    }

    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnsClearAll
 * Description:
 *    Deletes all remote I-SID/VLAN assignment data.
 * Input Parameters:
 *    notifyLldp - indicates if an LLDP update is required
 * Output Parameters:
 *    none
 * Return Values:
 *    Count - number of elements deleted
 **************************************************************/

int
faRemoteIsidVlanAsgnsClearAll (int notifyLldp)
{
    int count = 0;

#if FA_LOC_ASSIGNMENT

    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn, *tmpFaIsidVlanAsgn;
    LLDP_AVAYA_FA_ISID_VLAN_ASGNS_GROUP_T lldpFaIsidVlanAsgnData;

    /* Update the LLDP database - 'delete all' request */

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                    "faRemoteIsidVlanAsgnsClearAll", notifyLldp, 0, 0, NULL);

    if (notifyLldp == FA_NOTIFY_LLDP)
    {
        /* to resync the local settings on an FA host */
        if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
        {
            faLocalIsidVlanAsgnSync(FALSE, FALSE);
        }
    }

    if ((faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsCount <= 0))
    {
        return (count);
    }

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;

    while (faIsidVlanAsgn != NULL)
    {
        tmpFaIsidVlanAsgn = faIsidVlanAsgn->next;
        aasdkx_mem_free(faIsidVlanAsgn);
        count++;
        faIsidVlanAsgn = tmpFaIsidVlanAsgn;
    }

    faAgentConfigData.faRemoteIsidVlanAsgnsTable = NULL;
    faAgentConfigData.faRemoteIsidVlanAsgnsCount = 0;

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return (count);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnsClearLldpData
 * Description:
 *    Deletes all remote I-SID/VLAN assignment LLDP data.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faRemoteIsidVlanAsgnsClearLldpData ()
{

#if FA_REM_ASSIGNMENT

    if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
    {
        faLocalIsidVlanAsgnSync(FALSE, FALSE);
    }

#endif

    return;
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnFind
 * Description:
 *    Traverses the list of I-SID/VLAN assignments looking for
 *    a match based on the provided ifIndex, I-SID and VLAN data.
 * Input Parameters:
 *    ifIndex - target ifIndex data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 * Output Parameters:
 *    none
 * Return Values:
 *    NULL - target entry not found
 *    faRemoteIsidVlanAsgnsEntry_t * - target entry found
 **************************************************************/

faRemoteIsidVlanAsgnsEntry_t *
faRemoteIsidVlanAsgnFind (unsigned int ifIndex,
                          unsigned int isid,
                          unsigned int vlan,
                          int searchType)
{

    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn = NULL;
#if FA_REM_ASSIGNMENT
    int rc;

    /* Traverse the list of ifIndex/I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            rc = faRemoteIsidVlanAsgnLexiOrder(faIsidVlanAsgn->ifIndex,
                                               faIsidVlanAsgn->isid, 
                                               faIsidVlanAsgn->vlan,
                                               ifIndex, isid, vlan);
    
            if ((searchType == FA_AGENT_GET_EXACT) && (rc == FA_AGENT_LEXI_EQ))
            {
                /* Entry found */
                break;
            }
            else if ((searchType == FA_AGENT_GET_NEXT) && (rc == FA_AGENT_LEXI_GT))
            {
                /* Entry found */
                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }
#endif
    return (faIsidVlanAsgn);
}


/**************************************************************
 * Name: faRemoteIsidVlanAsgnPortFind
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided ifIndex value
 * Input Parameters:
 *    ifIndex - target ifIndex value
 *    pRemAsgnEnts - storage for matching entries
 * Output Parameters:
 *    pRemAsgCnt - number of entries found
 *    none
 * Return Values:
 *    0 - no entries found
 *    1 - entries found
 *    
 **************************************************************/

int
faRemoteIsidVlanAsgnPortFind (unsigned int ifIndex,
                              faRemoteIsidVlanAsgnsEntry_t *pRemAsgnEnts,
                              unsigned int *pRemAsgnCnt)
{
    int faRc = 0;

    *pRemAsgnCnt = 0;

#if FA_REM_ASSIGNMENT
    faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn = NULL;
    unsigned                      faRemIsidVlanAsgnPortCnt = 0;

    /* Traverse the list of ifIndex/I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faRemIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while ((faRemIsidVlanAsgn != NULL) &&
               (*pRemAsgnCnt < FA_AGENT_MAX_ISID_VLAN_ASGNS) &&
               (faRemIsidVlanAsgnPortCnt <
                faAgentConfigData.faRemoteIsidVlanAsgnsCount))
        {
	    if (faRemIsidVlanAsgn->ifIndex == ifIndex)
	    {
	        *pRemAsgnEnts = *faRemIsidVlanAsgn;
		 pRemAsgnEnts++;
		*pRemAsgnCnt += 1;
                 faRc = 1;
            };
            faRemIsidVlanAsgnPortCnt++;
            faRemIsidVlanAsgn = faRemIsidVlanAsgn->next;
        }
    }
#endif

    return (faRc);
}



/**************************************************************
 * Name: faRemoteIsidVlanAsgnAdd
 * Description:
 *    Adds an element to the list of ifIndex/I-SID/VLAN 
 *    assignments based on the element's ifIndex, I-SID 
 *    and VLAN data.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faRemoteIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to add element to list
 *    1 - element addition successful
 **************************************************************/

unsigned int
faRemoteIsidVlanAsgnAdd (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
#if FA_REM_ASSIGNMENT

    int rc, maxCount;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    maxCount = ((faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER) ?
                FA_AGENT_REM_MAX_ISID_VLAN_ASGNS : FA_AGENT_MAX_ISID_VLAN_ASGNS);

    if ((faIsidVlanAsgnData == NULL) || 
        (faAgentConfigData.faRemoteIsidVlanAsgnsCount >= maxCount))
    {
        return (0);
    }

    /* Locate addition position - list is sorted */

    if (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL)
    {
        /* List is empty */

        faAgentConfigData.faRemoteIsidVlanAsgnsTable = faIsidVlanAsgnData;
        faIsidVlanAsgnData->next = NULL;
        faIsidVlanAsgnData->prev = NULL;

        faAgentConfigData.faRemoteIsidVlanAsgnsCount++;
    }
    else
    { 
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            rc = faRemoteIsidVlanAsgnLexiOrder(faIsidVlanAsgnData->ifIndex,
                                               faIsidVlanAsgnData->isid, 
                                               faIsidVlanAsgnData->vlan,
                                               faIsidVlanAsgn->ifIndex, 
                                               faIsidVlanAsgn->isid, 
                                               faIsidVlanAsgn->vlan);
    
            if (rc == FA_AGENT_LEXI_LT)
            {
                /* Insert before current element */

                if (faIsidVlanAsgn == faAgentConfigData.faRemoteIsidVlanAsgnsTable)
                {
                    /* First element in list */

                    faAgentConfigData.faRemoteIsidVlanAsgnsTable = faIsidVlanAsgnData;
                    faIsidVlanAsgnData->next = faIsidVlanAsgn;
                    faIsidVlanAsgnData->prev = NULL;
                    faIsidVlanAsgn->prev = faIsidVlanAsgnData;
                }
                else
                {
                    faIsidVlanAsgnData->prev = faIsidVlanAsgn->prev;
                    faIsidVlanAsgn->prev->next = faIsidVlanAsgnData;
                    faIsidVlanAsgnData->next = faIsidVlanAsgn;
                    faIsidVlanAsgn->prev = faIsidVlanAsgnData;
                }
    
                faAgentConfigData.faRemoteIsidVlanAsgnsCount++;
    
                break;
            }
            else if (rc == FA_AGENT_LEXI_EQ)
            {
                /* Exact match found - should not occur */
                break;
            }

            /* Reached the end of the list? */
            if (faIsidVlanAsgn->next == NULL)
            {
                faIsidVlanAsgn->next = faIsidVlanAsgnData;
                faIsidVlanAsgnData->prev = faIsidVlanAsgn;
                faIsidVlanAsgnData->next = NULL;
    
                faAgentConfigData.faRemoteIsidVlanAsgnsCount++;
    
                break;
            }
    
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

#endif

    return (1);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnDelete
 * Description:
 *    Deletes an element from the list of ifIndex/I-SID/VLAN 
 *    assignments based on the element's ifIndex, I-SID and 
 *    VLAN data. If found, the target element memory is freed.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faRemoteIsidVlanAsgnsEntry_t
 *    notifyLldp - indicates if an LLDP update is required
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to delete element from list
 *    1 - element deletion successful
 **************************************************************/

int
faRemoteIsidVlanAsgnDelete (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                            int notifyLldp)
{
    int rc = 0;
#if FA_REM_ASSIGNMENT

    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                    "faRemoteIsidVlanAsgnDelete", notifyLldp, 0, 0, NULL);

    if ((faIsidVlanAsgnData == NULL) || 
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsCount <= 0))
    {
        return (0);
    }

    /* Search for target element */

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faRemoteIsidVlanAsgnFind(faIsidVlanAsgnData->ifIndex,
                                              faIsidVlanAsgnData->isid,
                                              faIsidVlanAsgnData->vlan,
                                              FA_AGENT_GET_EXACT);

    if (faIsidVlanAsgn != NULL)
    {
        if (faIsidVlanAsgn == faAgentConfigData.faRemoteIsidVlanAsgnsTable)
        {
            /* First element in list */

            faAgentConfigData.faRemoteIsidVlanAsgnsTable = faIsidVlanAsgn->next;

            if (faIsidVlanAsgn->next != NULL)
            {
                faIsidVlanAsgn->next->prev = NULL;
            }
        }
        else
        {
            faIsidVlanAsgn->prev->next = faIsidVlanAsgn->next;

            if (faIsidVlanAsgn->next != NULL)
            {
                faIsidVlanAsgn->next->prev = faIsidVlanAsgn->prev;
            }
        }
    
        faAgentConfigData.faRemoteIsidVlanAsgnsCount--;

        /* Update the LLDP database */

        if (notifyLldp)
        {
        }

        faAgentErrorMsg(FA_AGENT_INFO_REMOTE_DELETE, FA_AGENT_INFO_MSG_2,
                        "faRemoteIsidVlanAsgnDelete",
                        faIsidVlanAsgn->ifIndex, faIsidVlanAsgn->isid, 
                        faIsidVlanAsgn->vlan, NULL);


        aasdkx_mem_free(faIsidVlanAsgn);

        rc = 1;
    }

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnUpdate
 * Description:
 *    Updates LLDP FA data for a single remote I-SID/VLAN
 *    assignment entry. Syncs FA -> LLDP using an LLDP_SET 
 *    (update) operation.
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faRemoteIsidVlanAsgnsEntry_t
 *    distributeData - indicates local or stack-wide update
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - element update failed
 *    1 - element update successful
 **************************************************************/

int
faRemoteIsidVlanAsgnUpdate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                            int distributeData)
{
    int rc = 1;
#if FA_REM_ASSIGNMENT

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faRemoteIsidVlanAsgnUpdate", 0, 0, 0, NULL);

    if (faIsidVlanAsgnData == NULL)
    {
        return (0);
    }

    /* Update LLDP data only if interface is non-virtual */
    /* and the element type support utilizes LLDP exchanges */
    if ((FA_AGENT_ACTUAL_IFC(faIsidVlanAsgnData->ifIndex)) &&
        (! FA_ELEM_INT_CLIENT(faIsidVlanAsgnData->elemType)))
    {

    }
#endif
    return (rc);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnSync
 * Description:
 *    Synchronizes the remote I-SID/VLAN databases (FA --> LLDP).
 *    Optionally can sync agent state data, if requested.
 * Input Parameters:
 *    syncState - indicates if agent state data should be synced 
 *                as well.
 *    clearLldpData - indicates if LLDP data should be cleared
 *                    before synchronization is performed
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faRemoteIsidVlanAsgnSync (int syncState,
                          int clearLldpData) 
{
#if FA_REM_ASSIGNMENT
    int failureSeen = 0;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faRemoteIsidVlanAsgnSync", 0, 0, 0, NULL);

    aasdkx_mutex_lock(faAgentMut);

    /* Start by clearing all LLDP entries, if necessary */

    if (clearLldpData)
    {

    }

    if (faAgentConfigData.faRemoteIsidVlanAsgnsTable != NULL)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    /* Update agent state attributes, if necessary */

    if (syncState)
    {

    }

    aasdkx_mutex_unlock(faAgentMut);

    if (failureSeen)
    {
        faAgentErrorMsg(FA_AGENT_INFO_LLDP_ADD, FA_AGENT_INFO_MSG_1, 0, 0, 0, 0, NULL);
    }
#endif /* FA_REM_ASSIGNMENT */
    return;
}




void
faRemoteGetElemTypeString (unsigned int elemType,
                           unsigned char *elemTypeString)
{
    if (elemTypeString != NULL)
    {
        switch (elemType)
        {
            case FA_AGENT_ELEMENT_SERVER:
                strcpy((char *)elemTypeString, "S ");
                break;
            case FA_AGENT_ELEMENT_HOST:
                strcpy((char *)elemTypeString, "P ");
                break;
            case FA_AGENT_ELEMENT_CLIENT_UNTAGGED:
                strcpy((char *)elemTypeString, "CU");
                break;
            case FA_AGENT_ELEMENT_CLIENT_TAGGED:
                strcpy((char *)elemTypeString, "CT");
                break;
            case FA_AGENT_ELEMENT_INT_CLIENT_UNTAGGED:
                strcpy((char *)elemTypeString, "IU");
                break;
            case FA_AGENT_ELEMENT_INT_CLIENT_TAGGED:
                strcpy((char *)elemTypeString, "IT");
                break;
            case FA_AGENT_ELEMENT_INT_CLIENT_UPLINK:
                strcpy((char *)elemTypeString, "IL");
                break;
            case FA_AGENT_ELEMENT_NOT_SUPPORTED:
            default:
                strcpy((char *)elemTypeString, "UN");
                break;
        }
    }

    return;
}




void
faRemoteIsidVlanAsgnDump (int elementTypeStart, int elementTypeEnd)
{
#if FA_REM_ASSIGNMENT

    char buffer[16];
    char buffer1[16];
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    if ((elementTypeStart == 0) || (elementTypeEnd == 0))
    {
        printf("\n\nfaRemoteIsidVlanAsgnDump: element list (count %d current time %d)\n\n",
               faAgentConfigData.faRemoteIsidVlanAsgnsCount, faAgentTimeGet());
    }
    else
    {
        faRemoteGetElemTypeString(elementTypeStart, buffer);
        faRemoteGetElemTypeString(elementTypeEnd, buffer1);

        if (elementTypeStart == elementTypeEnd)
        {
            printf("\n\nfaRemoteIsidVlanAsgnDump: element type - %s (current time %d)\n\n",
                   buffer, faAgentTimeGet());
        }
        else
        {
            printf("\n\nfaRemoteIsidVlanAsgnDump: element types - %s/%s (current time %d)\n\n",
                   buffer, buffer1, faAgentTimeGet());
        }
    }

    aasdkx_mutex_lock(faAgentMut);

    if (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL)
    {
        printf("Empty\n\n");
    }
    else
    {
        printf("ifIndex  Trunk   I-SID    VLAN    Status    Components  ElemState   Updated\n");
        printf("-------  -----  --------  ----  ----------  ----------  ---------  ---------\n");

        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if ((elementTypeStart != 0) && (elementTypeEnd != 0) &&
                (! INRANGE(elementTypeStart, faIsidVlanAsgn->elemType, elementTypeEnd)))
            {
                faIsidVlanAsgn = faIsidVlanAsgn->next;
                continue;
            }

            if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_PENDING)
                sprintf(buffer, "Pending(%d)    ", faIsidVlanAsgn->status);
            else if (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE)
                sprintf(buffer, "Active(%d)     ", faIsidVlanAsgn->status);
            else
            {
                if (faIsidVlanAsgn->status < 10)
                    sprintf(buffer, "Reject(%d)     ", faIsidVlanAsgn->status);
                else
                    sprintf(buffer, "Reject(%d)    ", faIsidVlanAsgn->status);
            }

            printf("%-9d  %-3d  %-8d  %-4d  %s",
                   faIsidVlanAsgn->ifIndex, faIsidVlanAsgn->trunkId, 
                   faIsidVlanAsgn->isid, faIsidVlanAsgn->vlan, buffer);
 
            if (faIsidVlanAsgn->components == FA_COMP_NONE)
                printf("none ");
            else
            {
                if (faIsidVlanAsgn->components & FA_COMP_VLAN)
                    printf("V");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_MEMBER)
                    printf("M");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_TAG)
                    printf("T");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_VLAN_ISID)
                    printf("I");
                else
                    printf(" ");
                if (faIsidVlanAsgn->components & FA_COMP_PORT_PVID)
                    printf("P");
                else
                    printf(" ");
            }

            faRemoteGetElemTypeString(faIsidVlanAsgn->elemType, buffer);

            printf("     %02x/%s%4d", faIsidVlanAsgn->elemState,
                   buffer, faIsidVlanAsgn->clientVlan);

            printf("   %d\n", faIsidVlanAsgn->lastUpdated);

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }

        printf("\n");
        printf("Components: V-VLAN, M-Membership, T-Tagging, I-ISID, P-PVID\n");
        printf("ElemState: P-Proxy, CT-Client(Tagged), CU-Client(Untagged),\n");
        printf("           IT-Internal Client(Tagged), IU-Internal Client(Untagged),\n");
        printf("           IL-Internal Client(Uplink Setup Only)\n");
        printf("Note: element state info includes client VLAN data.\n");
        printf("\n");
    }

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return;
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnValidate
 * Description:
 *    Performs various tests to determine if element creation
 *    is allowed based on multiple factors (current element count,
 *    element data, etc.).
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faRemoteIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    None
 * Return Values:
 *    FA_AGENT_ISID_VLAN_ASGN_PENDING - validation success
 *    >= FA_AGENT_ISID_VLAN_ASGN_REJECTED - validation failure
 **************************************************************/

faIsidVlanAsgnStates
faRemoteIsidVlanAsgnValidate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
#if FA_REM_ASSIGNMENT

    int maxCount;


    if (faIsidVlanAsgnData == NULL)
    {
        return (FA_AGENT_ISID_VLAN_ASGN_REJECTED);
    }

    /* Verify that the FA service is enabled */
    if (faAgentConfigData.faServiceStatus != FA_AGENT_STATE_ENABLED)
    {
        return (FA_AGENT_ISID_VLAN_ASGN_REJECTED);
    }

    /* Check if resources are available */
    maxCount = ((faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER) ?
                FA_AGENT_REM_MAX_ISID_VLAN_ASGNS : FA_AGENT_MAX_ISID_VLAN_ASGNS);

    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount >= maxCount)
    {
        return (FA_ASGN_REJECTION_RESOURCE_ERR);
    }

    /* Check for data conflicts */
    if (! faRemoteIsidVlanAsgnConflict(faIsidVlanAsgnData->ifIndex,
                                       faIsidVlanAsgnData->isid,
                                       faIsidVlanAsgnData->vlan,
                                       faIsidVlanAsgnData->elemType))
    {
        return (FA_ASGN_REJECTION_DUPLICATE_ERR);
    }

#endif

    return (FA_AGENT_ISID_VLAN_ASGN_PENDING);
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnCreate
 * Description:
 *    Allocates I-SID/VLAN assignment element memory and adds 
 *    the element to the list of current assignments. 
 * Input Parameters:
 *    faIsidVlanAsgnData - pointer to faRemoteIsidVlanAsgnsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - element creation failed
 *    1 - element creation successful
 **************************************************************/

int
faRemoteIsidVlanAsgnCreate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData)
{
    int rc = 0;
#if FA_REM_ASSIGNMENT
    int maxCount;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_4,
                    "faRemoteIsidVlanAsgnCreate", 0, 0, 0, NULL);

    maxCount = ((faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER) ?
                FA_AGENT_REM_MAX_ISID_VLAN_ASGNS : FA_AGENT_MAX_ISID_VLAN_ASGNS);

    if ((faIsidVlanAsgnData == NULL) || 
        (faAgentConfigData.faRemoteIsidVlanAsgnsCount >= maxCount))
    {
        return (0);
    }

    aasdkx_mutex_lock(faAgentMut);

    /* Allocate memory for element tracking */
    faIsidVlanAsgn = (faRemoteIsidVlanAsgnsEntry_t *) aasdkx_mem_alloc(
               sizeof(faRemoteIsidVlanAsgnsEntry_t));

    if (faIsidVlanAsgn != NULL)
    {
        memset(faIsidVlanAsgn, 0, sizeof(faRemoteIsidVlanAsgnsEntry_t));

        /* Set-up element data */
        faIsidVlanAsgn->ifIndex = faIsidVlanAsgnData->ifIndex;
        faIsidVlanAsgn->trunkId = faIsidVlanAsgnData->trunkId;
        faIsidVlanAsgn->isid = faIsidVlanAsgnData->isid;
        faIsidVlanAsgn->vlan = faIsidVlanAsgnData->vlan;
        faIsidVlanAsgn->status = faIsidVlanAsgnData->status;
        faIsidVlanAsgn->components = faIsidVlanAsgnData->components;
        faIsidVlanAsgn->elemType = faIsidVlanAsgnData->elemType;
        faIsidVlanAsgn->clientVlan = faIsidVlanAsgnData->clientVlan;

        faIsidVlanAsgn->lastUpdated = faAgentTimeGet();

        /* Save timestamp for possible later use */
        faIsidVlanAsgnData->lastUpdated = faIsidVlanAsgn->lastUpdated;

        /* Add the element to the list */
        if (faRemoteIsidVlanAsgnAdd(faIsidVlanAsgn))
        {
            rc = 1;
        }
        else
        {
            /* Addition failure - free allocated memory */
            aasdkx_mem_free(faIsidVlanAsgn);
        }
    }

    aasdkx_mutex_unlock(faAgentMut);
#endif
    return (rc);
}

/**************************************************************
 * Name: faUpdateRemoteIsidVlanAsgns
 * Description:
 *    Retrieves updated I-SID/VLAN assignments from LLDP
 *    and updates the data in the FA database. The data is 
 *    passed to the appropriate routines that support 
 *    host-specific and server-specific processing.
 * Input Parameters:
 *    updateData - target interface for update and source
 *                 element type/vlan
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faUpdateRemoteIsidVlanAsgns (unsigned int updateData,
                             unsigned int faBindingCount,
                             faMinIsidVlanBindingEntry_t *faBindingList)
{
#if FA_REM_ASSIGNMENT

    int count = 0;
    int activeCount = 0;
    faIsidVlanAsgnStates asgnState;
    faMinIsidVlanBindingEntry_t faIsidVlanAsgn;

    unsigned short interface;
    unsigned short elemType;
    unsigned short elemVlan;

    interface = updateData & 0x0000ffff;
    elemType = (updateData >> 28) & 0x0000000f;
    elemVlan = (updateData >> 16) & 0x00000fff;

    for (count = 0; count < faBindingCount; count++)
    {
        faIsidVlanAsgn.interface = interface;
        faIsidVlanAsgn.isid = faBindingList[count].isid;
        faIsidVlanAsgn.vlan = faBindingList[count].vlan;
        faIsidVlanAsgn.status = faBindingList[count].status;

        /* The element type determines how this data should now be */
        /* processed, i.e., whether the host or the server receive */
        /* state machine should be executed */
        if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
        {

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

            if (! count)
            {
                faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                                "faHostProcessRemoteElem", interface, 0, 0, NULL);
            }

            faHostProcessRemoteElem(faIsidVlanAsgn.interface,
                                    faIsidVlanAsgn.isid,
                                    faIsidVlanAsgn.vlan,
                                    faIsidVlanAsgn.status,
                                    elemType, elemVlan);
#endif

        }
        else if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER)
        {
            if (! count)
            {
                faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                                "faServerProcessRemoteElem", interface, 0, 0, NULL);
            }

            asgnState = faServerProcessRemoteElem(faIsidVlanAsgn.interface,
                                                  faIsidVlanAsgn.isid,
                                                  faIsidVlanAsgn.vlan,
                                                  faIsidVlanAsgn.status,
                                                  elemType, elemVlan);

#ifdef SYSDEF_INCLUDE_FA_SMLT























#endif /* SYSDEF_INCLUDE_FA_SMLT */

        }
    }

    /* Track LLDP synchronization issues */
    if (! count)
    {
        faAgentStatsData.faAgentLldpSyncIssues++;
    }

    if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_HOST)
    {
        faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                        "faHostProcessRemoteElem: elems processed", count, 0, 0, NULL);

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

        /* Skip post-update processing if an issue was detected */
        /* or if the updated wasn't from a client */
        if ((count) && (FA_ELEM_CLIENT(elemType)))
        {
            faProxyPostUpdateProcess(interface); 
        }

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

    }
    else if (faAgentConfigData.faElementType == FA_AGENT_ELEMENT_SERVER)
    {
        faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_1, FA_AGENT_INFO_MSG_4,
                        "faServerProcessRemoteElem: elems processed", count, 0, 0, NULL);

#ifdef SYSDEF_INCLUDE_FA_SMLT









#endif /* SYSDEF_INCLUDE_FA_SMLT */

        /* Skip post-update processing if an issue was detected */
        if (count)
        {
            faServerPostUpdateProcess(interface); 
        }
    }

    faAgentStatsData.faAgentIsidVlanAsgnUpdates++;

#endif

    return;
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnUpdateState
 * Description:
 *    Traverses the list of remote I-SID/VLAN assignments looking
 *    for a match based on the provided ifIndex/I-SID/VLAN data. 
 *    If found, the element state is updated based on the 'state' 
 *    and update type (SET/CLEAR) that is specified.
 * Input Parameters:
 *    interface - target interface data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 *    timestamp - timestamp value for update (ignore if 0)
 *    update - operation (SET or CLEAR)
 *    state - state indicator
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

void
faRemoteIsidVlanAsgnUpdateState (unsigned int interface,
                                 unsigned int isid,
                                 unsigned int vlan,
                                 unsigned int timestamp,
                                 unsigned int update,
                                 unsigned char state)
{
#if FA_REM_ASSIGNMENT

    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    aasdkx_mutex_lock(faAgentMut);

    /* Traverse the list of I-SID/VLAN asgns looking for the target */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if ((faIsidVlanAsgn->ifIndex == interface) && 
                (faIsidVlanAsgn->isid == isid) && 
                (faIsidVlanAsgn->vlan == vlan))
            {
                if (update == FA_ELEM_STATE_UPDATE_SET)
                {
                    faIsidVlanAsgn->elemState |= state;
                }
                else if (update == FA_ELEM_STATE_UPDATE_CLEAR)
                {
                    faIsidVlanAsgn->elemState &= ~state;
                }

                if (timestamp)
                {
                    faIsidVlanAsgn->lastUpdated = timestamp;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

#endif

    return;
}

/**************************************************************
 * Name: faRemoteIsidVlanAsgnVlanSource
 * Description:
 *    Traverses the list of I-SID/VLAN assignments to determine
 *    if the specified VLAN was created through FA processing.
 * Input Parameters:
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - Specified VLAN was not created through FA
 *    1 - Specified VLAN was created through FA
 **************************************************************/

int
faRemoteIsidVlanAsgnVlanSource (unsigned int vlan)
{
    int rc = 0;

#if FA_REM_ASSIGNMENT

    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn = NULL;

    /* Traverse the list of ifIndex/I-SID/VLAN asgns looking for the target VLAN */
    if (faAgentConfigData.faRemoteIsidVlanAsgnsCount)
    {
        faIsidVlanAsgn = faAgentConfigData.faRemoteIsidVlanAsgnsTable;
        while (faIsidVlanAsgn != NULL)
        {
            if ((faIsidVlanAsgn->vlan == vlan) &&
                (faIsidVlanAsgn->status == FA_AGENT_ISID_VLAN_ASGN_ACTIVE))
            {
                /* Entry found */
                if (faIsidVlanAsgn->components & FA_COMP_VLAN)
                {
                    rc = 1;
                }

                break;
            }

            faIsidVlanAsgn = faIsidVlanAsgn->next;
        }
    }

#endif

    return (rc);
}

/**************************************************************
 * Name: faAgentPerformRemoteElemRemoval
 * Description:
 *    Handles element removal activities for a remote
 *    I-SID/VLAN assignment list element.
 * Input Parameters:
 *    interface - target ifIndex data
 *    isid - target I-SID data
 *    vlan - target VLAN data
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - target entry not found or found but FA deletion issue
 *    1 - target entry found and no FA deletion issues
 **************************************************************/

int
faAgentPerformRemoteElemRemoval (unsigned int interface,
                                 unsigned int isid,
                                 unsigned int vlan)
{
    int rc = 1;
    faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgn;

    faAgentErrorMsg(FA_AGENT_INFO_CALL_TRACE_0, FA_AGENT_INFO_MSG_3,
                    "faAgentPerformRemoteElemRemoval", 0, 0, 0, NULL);

    if ((faAgentConfigData.faRemoteIsidVlanAsgnsCount == 0) ||
        (faAgentConfigData.faRemoteIsidVlanAsgnsTable == NULL))
    {
        /* Nothing to do */
        return (0);
    }

    aasdkx_mutex_lock(faAgentMut);

    faIsidVlanAsgn = faRemoteIsidVlanAsgnFind(interface, isid,
                                              vlan, FA_AGENT_GET_EXACT);

    if (faIsidVlanAsgn != NULL)
    {
        if (FA_AGENT_ACTUAL_IFC(interface))
        {
            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_DELETE, FA_AGENT_INFO_MSG_3,
                            NULL,
                            faIsidVlanAsgn->ifIndex,
                            faIsidVlanAsgn->isid, 
                            faIsidVlanAsgn->vlan, NULL);
        }
        else if (FA_AGENT_VIRTUAL_IFC(interface))
        {
            faAgentErrorMsg(FA_AGENT_INFO_REMOTE_SMLT_DELETE,
                            FA_AGENT_INFO_MSG_3,
                            NULL,
                            faIsidVlanAsgn->ifIndex, faIsidVlanAsgn->isid, 
                            faIsidVlanAsgn->vlan, NULL);
        }

        /* Initiate settings cleanup - deactivate assignment */
        if (! faServerAsgnRejectionProcessing(faIsidVlanAsgn,
                                              FA_AGENT_ISID_VLAN_ASGN_REJECTED))
        {
            if (FA_AGENT_ACTUAL_IFC(interface))
            {
                faAgentStatsData.faAgentServerCleanupIssues++;
            }
            else if (FA_AGENT_VIRTUAL_IFC(interface))
            {
                faAgentStatsData.faAgentServerSmltPeerCleanupIssues++;
            }

            rc = 0;
        }

        /* Delete FA database and FA LLDP database assignment */
        faRemoteIsidVlanAsgnDelete(faIsidVlanAsgn, ((FA_AGENT_VIRTUAL_IFC(interface)) ?  
                                                    FA_NO_NOTIFY_LLDP : FA_NOTIFY_LLDP));
    }
    else
    {
        rc = 0;
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faDiscoveredElementsClearAll
 * Description:
 *    Deletes all discovered element data.
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    Count - number of elements deleted
 **************************************************************/

int
faDiscoveredElementsClearAll ()
{
    int count = 0;
    faDiscoveredElementsEntry_t *faDiscElems, *tmpFaDiscElems;

    if ((faAgentConfigData.faDiscoveredElements == NULL) ||
        (faAgentConfigData.faDiscoveredElementsCount <= 0))
    {
        return (count);
    }

    aasdkx_mutex_lock(faAgentMut);

    faDiscElems = faAgentConfigData.faDiscoveredElements;

    while (faDiscElems != NULL)
    {
        tmpFaDiscElems = faDiscElems->next;
        aasdkx_mem_free(faDiscElems);
        count++;
        faDiscElems = tmpFaDiscElems;
    }

    faAgentConfigData.faDiscoveredElements = NULL;
    faAgentConfigData.faDiscoveredElementsCount = 0;

    aasdkx_mutex_unlock(faAgentMut);

    return (count);
}



#if FA_DISCOVERY

/**************************************************************
 * Name: faDiscoveredElementsFind
 * Description:
 *    Traverses the list of discovered FA elements looking for
 *    a match based on the provided ifIndex data.
 * Input Parameters:
 *    ifIndex - target ifIndex data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 * Output Parameters:
 *    none
 * Return Values:
 *    NULL - target entry not found
 *    faDiscoveredElementsEntry_t * - target entry found
 **************************************************************/

faDiscoveredElementsEntry_t *
faDiscoveredElementsFind (unsigned int ifIndex,
                          int searchType)
{
    faDiscoveredElementsEntry_t *faDiscElem = NULL;

    /* Traverse the list of discovered elements looking for the target */
    if (faAgentConfigData.faDiscoveredElementsCount)
    {
        faDiscElem = faAgentConfigData.faDiscoveredElements;
        while (faDiscElem != NULL)
        {
            if ((searchType == FA_AGENT_GET_EXACT) && (ifIndex == faDiscElem->ifIndex))
            {
                /* Entry found */
                break;
            }
            else if ((searchType == FA_AGENT_GET_NEXT) && (ifIndex <= faDiscElem->ifIndex))
            {
                /* Entry found */
                break;
            }

            faDiscElem = faDiscElem->next;
        }
    }

    return (faDiscElem);
}

/**************************************************************
 * Name: faDiscoveredElementsAdd
 * Description:
 *    Adds an element to the list of discoverd FA elements
 *    based on the element's ifIndex.
 * Input Parameters:
 *    faDiscElemData - pointer to faDiscoveredElementsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to add element to list
 *    1 - element addition successful
 **************************************************************/

int
faDiscoveredElementsAdd (faDiscoveredElementsEntry_t *faDiscElemData)
{
    faDiscoveredElementsEntry_t *faDiscElem;

    if (faDiscElemData == NULL) 
    {
        return (0);
    }

    /* Locate addition position - list is sorted */

    if (faAgentConfigData.faDiscoveredElements == NULL)
    {
        /* List is empty */

        faAgentConfigData.faDiscoveredElements = faDiscElemData;
        faDiscElemData->next = NULL;
        faDiscElemData->prev = NULL;

        faAgentConfigData.faDiscoveredElementsCount++;
    }
    else
    { 
        faDiscElem = faAgentConfigData.faDiscoveredElements;
        while (faDiscElem != NULL)
        {
            if (faDiscElemData->ifIndex < faDiscElem->ifIndex)
            {
                /* Insert before current element */

                if (faDiscElem == faAgentConfigData.faDiscoveredElements)
                {
                    /* First element in list */

                    faAgentConfigData.faDiscoveredElements = faDiscElemData;
                    faDiscElemData->next = faDiscElem;
                    faDiscElemData->prev = NULL;
                    faDiscElem->prev = faDiscElemData;
                }
                else
                {
                    faDiscElemData->prev = faDiscElem->prev;
                    faDiscElem->prev->next = faDiscElemData;
                    faDiscElemData->next = faDiscElem;
                    faDiscElem->prev = faDiscElemData;
                }
    
                faAgentConfigData.faDiscoveredElementsCount++;
    
                break;
            }
            else if (faDiscElemData->ifIndex == faDiscElem->ifIndex)
            {
                /* Exact match found - should not occur */
                break;
            }

            /* Reached the end of the list? */
            if (faDiscElem->next == NULL)
            {
                faDiscElem->next = faDiscElemData;
                faDiscElemData->prev = faDiscElem;
                faDiscElemData->next = NULL;
    
                faAgentConfigData.faDiscoveredElementsCount++;
    
                break;
            }
    
            faDiscElem = faDiscElem->next;
        }
    }

    return (1);
}

/**************************************************************
 * Name: faDiscoveredElementsDelete
 * Description:
 *    Deletes an element from the list of discovered FA 
 *    elements based on the element's ifIndex. If found, the 
 *    target element memory is freed.
 * Input Parameters:
 *    faDiscElemData - pointer to faDiscoveredElementsEntry_t
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - unable to delete element from list
 *    1 - element deletion successful
 **************************************************************/

int
faDiscoveredElementsDelete (faDiscoveredElementsEntry_t *faDiscElemData)
{
    int rc = 0;
    faDiscoveredElementsEntry_t *faDiscElem;

    if ((faDiscElemData == NULL) || 
        (faAgentConfigData.faDiscoveredElements == NULL) ||
        (faAgentConfigData.faDiscoveredElementsCount <= 0))
    {
        return (0);
    }

    /* Search for target element */

    aasdkx_mutex_lock(faAgentMut);

    faDiscElem = faDiscoveredElementsFind(faDiscElemData->ifIndex,
                                          FA_AGENT_GET_EXACT);

    if (faDiscElem != NULL)
    {
        if (faDiscElem == faAgentConfigData.faDiscoveredElements)
        {
            /* First element in list */

            faAgentConfigData.faDiscoveredElements = faDiscElem->next;

            if (faDiscElem->next != NULL)
            {
                faDiscElem->next->prev = NULL;
            }
        }
        else
        {
            faDiscElem->prev->next = faDiscElem->next;

            if (faDiscElem->next != NULL)
            {
                faDiscElem->next->prev = faDiscElem->prev;
            }
        }
    
        faAgentConfigData.faDiscoveredElementsCount--;

        aasdkx_mem_free(faDiscElem);

        rc = 1;
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faDiscoveredElementsCreate
 * Description:
 *    Allocates discovered FA element memory and adds 
 *    the element to the list of current elements. 
 * Input Parameters:
 *    ifIndex - interface through which element was discovered
 *    elemType - type of discovered FA element
 *    elemVlan - VLAN associated with discovered FA element
 *    chassisId - chassis ID data
 *    chassisIdLen - chassis ID data length
 * Output Parameters:
 *    none
 * Return Values:
 *    0 - element creation failed
 *    1 - element creation successful
 **************************************************************/

int
faDiscoveredElementsCreate (unsigned int ifIndex,
                            unsigned int elemType,
                            unsigned int elemVlan,
                            unsigned char *chassisId,
                            unsigned int chassisIdLen)
{
    int rc = 0;
    faDiscoveredElementsEntry_t *faDiscElem;

    aasdkx_mutex_lock(faAgentMut);

    /* Allocate memory for element tracking */
    faDiscElem = (faDiscoveredElementsEntry_t *) aasdkx_mem_alloc(
           sizeof(faDiscoveredElementsEntry_t));

    if (faDiscElem != NULL)
    {
        memset(faDiscElem, 0, sizeof(faDiscoveredElementsEntry_t));

        /* Set-up element data */
        faDiscElem->ifIndex = ifIndex;
        faDiscElem->elemType = elemType;
        faDiscElem->elemVlan = elemVlan;
        faDiscElem->lastUpdated = faAgentTimeGet();

        if (chassisId != NULL)
        {
            faDiscElem->chassisIdLen =
                (chassisIdLen < FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN) ?
                 chassisIdLen : FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN;
            memcpy(faDiscElem->chassisId, chassisId,
                   faDiscElem->chassisIdLen);
        }

        /* Add the element to the list */
        if (faDiscoveredElementsAdd(faDiscElem))
        {
            rc = 1;
        }
        else
        {
            /* Addition failure - free allocated memory */
            aasdkx_mem_free(faDiscElem);
        }
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (rc);
}

/**************************************************************
 * Name: faDiscoveredElementsQuery
 * Description:
 *    Queries the FA discovered elements data for the
 *    exact/next discovered element.
 * Input Parameters:
 *    ifIndex - target element ifIndex data
 *    searchType - specifies that an FA_AGENT_GET_EXACT or 
 *                 FA_AGENT_GET_NEXT match is required
 *    elemType - type of discovered FA element placeholder
 *    elemVlan - VLAN of discovered FA element placeholder
 *    elemDescrLen - maximum length of element description
 *    elemDescr - element description placeholder
 * Output Parameters:
 *    elemType - type of discovered FA element
 *    elemVlan - VLAN of discovered FA element
 *    elemDescrLen - length of element description
 *    elemDescr - element description
 * Return Values:
 *    0 - element query failed
 *   >0 - ifIndex of element being returned
 **************************************************************/

int
faDiscoveredElementsQuery (int ifIndex,
                           int searchType,
                           int *elemType,
                           int *elemVlan,
                           int *elemDescrLen,
                           unsigned char *elemDescr)
{
    int retIfIndex = 0;
    faDiscoveredElementsEntry_t *faDiscElem = NULL;

    if ((elemType == NULL) || (elemVlan == NULL) ||
        (elemDescrLen == NULL) || (elemDescr == NULL) ||
        (! INRANGE(FA_AGENT_GET_EXACT, searchType, FA_AGENT_GET_NEXT)))
    {
        return (0);
    }

    aasdkx_mutex_lock(faAgentMut);

    faDiscElem = faDiscoveredElementsFind(ifIndex, searchType);

    if (faDiscElem != NULL)
    {
        *elemType = faDiscElem->elemType;
        *elemVlan = faDiscElem->elemVlan;
        
        if (faDiscElem->chassisIdLen < *elemDescrLen)
            *elemDescrLen = faDiscElem->chassisIdLen;

        memcpy(elemDescr, faDiscElem->chassisId, *elemDescrLen);

        retIfIndex = faDiscElem->ifIndex;
    }

    aasdkx_mutex_unlock(faAgentMut);

    return (retIfIndex);
}

void
faDiscoveredElementsDump ()
{
    unsigned int i, unit, port;
    char buffer[16];
    faDiscoveredElementsEntry_t *faDiscElem;

    printf("\n\nDiscovered FA Elements: current element count - %d (current time %d)\n\n",
           faAgentConfigData.faDiscoveredElementsCount, faAgentTimeGet());

    aasdkx_mutex_lock(faAgentMut);

    if (faAgentConfigData.faDiscoveredElements == NULL)
    {
        printf("No Discovered Elements\n\n");
    }
    else
    {
        printf(" Interface     Element Type     Updated     Chassis ID\n");
        printf("-----------  ----------------  ---------  --------------------------------\n");

        faDiscElem = faAgentConfigData.faDiscoveredElements;
        while (faDiscElem != NULL)
        {
            port++;
            sprintf(buffer, "%d/%d (%d)", unit, port, faDiscElem->ifIndex);

            printf("%-11s  ", buffer);
 
            if (faDiscElem->elemType == FA_AGENT_ELEMENT_SERVER)
                printf("Server            ");
            else if (faDiscElem->elemType == FA_AGENT_ELEMENT_HOST)
                printf("Proxy             ");
            else if (faDiscElem->elemType == FA_AGENT_ELEMENT_CLIENT_UNTAGGED)
                printf("Client(Untagged)  ");
            else if (faDiscElem->elemType == FA_AGENT_ELEMENT_CLIENT_TAGGED)
                printf("Client(Tagged)    ");
            else
                printf("Unknown           ");

            printf("%-9d  ", faDiscElem->lastUpdated);

            if (faDiscElem->chassisIdLen)
            {
                for (i = 0; i < faDiscElem->chassisIdLen; i++)
                    if (i == (faDiscElem->chassisIdLen - 1))
                        printf("%02x", faDiscElem->chassisId[i]);
                    else
                        printf("%02x:", faDiscElem->chassisId[i]);
            }

            printf("\n");

            faDiscElem = faDiscElem->next;
        }

        printf("\n");
    }

    aasdkx_mutex_lock(faAgentMut);

    return;
}
#endif /* FA_DISCOVERY */


void
faNotifyApps (int currentEvent)
{
    int i;

    for (i = 0; i < FA_NOTIFICATION_CALLBACKS; i++)
    {
        if (faCallbackTable[i].tgtEvents & currentEvent)
        {
            faCallbackTable[i].callback(currentEvent, faCallbackTable[i].context);
        }
    }

    return;
}



#ifdef SYSDEF_INCLUDE_MLT



















  













































































































#endif /* ifdef SYSDEF_INCLUDE_MLT */

