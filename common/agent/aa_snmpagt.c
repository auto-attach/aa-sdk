/* aa-sdk/common/agent/aa_snmpagt.c */

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
#include <string.h>

#include "aa_sysdefs.h"
#include "aa_agent.h"

#include "aasdk_comm.h"


/********************************************************************
 Fabric Attach Agent Log/Error Message support routines
 ********************************************************************/

#define AA_SDK_MSG_LEN_MAX 120


char *aa_agt_log_messages[] = {

/* FA_AGENT_INFO_SERVICE_ENABLED */    "aasdk: service enabled",
/* FA_AGENT_INFO_SERVICE_DISABLED */   "aasdk: service disabled",
/* FA_AGENT_INFO_REMOTE_ACCEPT */      "aasdk: remote asgn - accepted (%d/%d/%d)",
/* FA_AGENT_INFO_REMOTE_DELETE */      "aasdk: remote asgn - deleted (%d/%d/%d)",
/* FA_AGENT_INFO_REMOTE_UPDATE */      "aasdk: remote asgn - updated (%d/%d/%d)",
/* FA_AGENT_ERR_REMOTE_ACCEPT_FAIL */  "aasdk: remote asgn - acceptance failure (%d/%d/%d)",
/* FA_AGENT_ERR_TIMER_UNAVAILABLE */   "aasdk: timer-related services unavailable",
/* FA_AGENT_ERR_NV_SAVE_FAILURE */     "aasdk: NVRAM save failure detected",
/* FA_AGENT_ERR_NV_RESTORE_FAILURE */  "aasdk: NVRAM restore failure detected",
/* FA_AGENT_INFO_NV_SAVE_SUCCESS */    "aasdk: NVRAM save success (%d local %d remote)",
/* FA_AGENT_INFO_LLDP_DELETE */        "aasdk: LLDP sync issue - delete (%d/%d/%d)",
/* FA_AGENT_INFO_LLDP_ADD */           "aasdk: LLDP sync issue - add (%d/%d/%d)",
/* FA_AGENT_INFO_LLDP_UPDATE */        "aasdk: LLDP sync issue - update (%d/%d/%d)",
/* FA_AGENT_ERR_SERVICE_NOT_AVAIL */   "aasdk: operation rejected - service not enabled",
/* FA_AGENT_ERR_SERVICE_LIMITED */     "aasdk: operation rejected - not supported by element type",
/* FA_AGENT_ERR_RESOURCES_EXCEEDED */  "aasdk: operation rejected - table entry limits reached",      
/* FA_AGENT_ERR_ASGN_DUPLICATION */    "aasdk: operation rejected - CVLAN already exists",
/* FA_AGENT_ERR_GENERAL_FAILURE */     "aasdk: operation failure detected",
/* FA_AGENT_ERR_ASGN_UNKNOWN */        "aasdk: operation rejected - entry does not exist",
/* FA_AGENT_ERR_ASGN_CLIENT_CTRL */    "aasdk: operation rejected - client-owned entry",
/* FA_AGENT_ERR_ASGN_CREATION_FAIL */  "aasdk: I-SID/VLAN assignment creation failure (unexpected)",
/* FA_AGENT_ERR_ASGN_DELETION_FAIL */  "aasdk: I-SID/VLAN assignment deletion failure (unexpected)",
/* FA_AGENT_ERR_ASGN_UPDATE_UNK */     "aasdk: unknown I-SID/VLAN asgn (update %d/%d)",
/* FA_AGENT_ERR_ASGN_UPDATE_STATE */   "aasdk: rcvd invalid I-SID/VLAN asgn state (%d - %d/%d)",
/* FA_AGENT_ERR_ASGN_IFC_INVALID */    "aasdk: interface-specific I-SID/VLAN assignments not allowed",
/* FA_AGENT_INFO_MSG_RECEIVED */       "aasdk: message processing (msgId %d)",
/* FA_AGENT_INFO_UNK_MSG_RECEIVED */   "aasdk: unknown message rcvd (msgId %d)",
/* FA_AGENT_INFO_LOCAL_CREATE */       "aasdk: local asgn - created (%d/%d)",
/* FA_AGENT_INFO_LOCAL_DELETE */       "aasdk: local asgn - deleted (%d/%d)",
/* FA_AGENT_INFO_LOCAL_UPDATE */       "aasdk: local asgn - updated (%d/%d)",
/* FA_AGENT_ERR_ASGN_INVALID_VLAN */   "aasdk: invalid VLAN data (unknown or incompatible type)",
/* FA_AGENT_INFO_MULTIPLE_SERVERS */   "aasdk: multiple FA servers detected",
/* FA_AGENT_INFO_PRIM_SERVER_LOST */   "aasdk: primary FA server lost",
/* FA_AGENT_ERR_ASGN_ACTIVE_FAIL */    "aasdk: activation failure (%d/%d ifc %d)",
/* FA_AGENT_ERR_ASGN_ACPT_VLAN */      "aasdk: asgn activation failure (VLAN creation)",
/* FA_AGENT_ERR_ASGN_ACPT_MEMBER */    "aasdk: asgn activation failure (VLAN membership)",
/* FA_AGENT_ERR_ASGN_ACPT_TAGGING */   "aasdk: asgn activation failure (port tagging)",
/* FA_AGENT_ERR_ASGN_ACPT_PVID */      "aasdk: asgn activation failure (PVID update)",
/* FA_AGENT_ERR_ASGN_ACPT_SW_UNI */    "aasdk: asgn activation failure (switched UNI creation)",
/* FA_AGENT_ERR_ASGN_REJT_VLAN */      "aasdk: asgn rejection failure (VLAN deletion)",
/* FA_AGENT_ERR_ASGN_REJT_MEMBER */    "aasdk: asgn rejection failure (VLAN membership)",
/* FA_AGENT_ERR_ASGN_REJT_SW_UNI */    "aasdk: asgn rejection failure (switched UNI deletion)",
/* FA_AGENT_ERR_ASGN_REJT_PVID */      "aasdk: asgn rejection failure (port PVID update)",
/* FA_AGENT_ERR_ASGN_DEACTIVE_FAIL */  "aasdk: deactivation failure (%d/%d ifc %d)",
/* FA_AGENT_ERR_ASGN_RESET_ISSUE */    "aasdk: asgn reset issues (%d asgns)",
/* FA_AGENT_ERR_ASGN_EXPIRE_FAIL */    "aasdk: expiration cleanup issue detected (%d secs)",
/* FA_AGENT_INFO_REMOTE_EXPIRE */      "aasdk: remote asgn - expired (%d/%d/%d)",
/* FA_AGENT_INFO_ELEM_TYPE_UPDATE */   "aasdk: element type updated",
/* FA_AGENT_ERR_ELEM_TYPE_UPDATE */    "aasdk: specified element type not supported",
/* FA_AGENT_ERR_UNSUPPORTED_OP_HOST */ "aasdk: operation not supported (switch-only operation)",
/* FA_AGENT_ERR_UNSUPPORTED_OP_SRVR */ "aasdk: operation not supported (server-only operation)",
/* FA_AGENT_ERR_NV_CLEANUP_FAILURE */  "aasdk: agent settings reset incomplete",
/* FA_AGENT_INFO_REMOTE_HOST_REMOVE */ "aasdk: remote asgn - switch removal (%d/%d/%d)",
/* FA_AGENT_ERR_HOST_REMOVE_FAIL */    "aasdk: switch removal issue detected (switch %d/%d)",
/* FA_AGENT_ERR_SRVR_REJT_GEN */       "aasdk: server asgn (%d/%d) rejection",
/* FA_AGENT_ERR_SRVR_REJT_RES */       "aasdk: server asgn (%d/%d) rejection - no FA resources",
/* FA_AGENT_ERR_SRVR_REJT_INV_VLAN */  "aasdk: server asgn (%d/%d) rejection - invalid VLAN",
/* FA_AGENT_ERR_SRVR_REJT_RES_VLAN */  "aasdk: server asgn (%d/%d) rejection - no VLAN resources",
/* FA_AGENT_ERR_SRVR_REJT_APP */       "aasdk: server asgn (%d/%d) rejection - application interactions",
/* FA_AGENT_ERR_SRVR_REJT_PEER */      "aasdk: server asgn (%d/%d) rejection - peer acceptance",
/* FA_AGENT_INFO_AUTOATCH_ENABLED */   "aasdk: aasdk capabilities enabled",
/* FA_AGENT_INFO_AUTOATCH_DISABLED */  "aasdk: aasdk capabilities disabled",
/* FA_AGENT_ERR_AUTOATCH_LIMITED */    "aasdk: aasdk not supported by element type",
/* FA_AGENT_ERR_AUTOATCH_VLAN */       "aasdk: auto attach failure (VLAN creation)",
/* FA_AGENT_ERR_AUTOATCH_MEMBER */     "aasdk: auto attach failure (VLAN membership)",
/* FA_AGENT_ERR_AUTOATCH_TAGGING */    "aasdk: auto attach failure (port tagging)",
/* FA_AGENT_ERR_AUTOATCH_DHCP */       "aasdk: auto attach failure (DHCP initiation)",
/* FA_AGENT_ERR_AUTOATCH_MGMT */       "aasdk: auto attach failure (mgmt VLAN update)",
/* FA_AGENT_ERR_AUTOATCH_PVID */       "aasdk: auto attach failure (PVID update)",
/* FA_AGENT_ERR_AUTOATCH_FAIL */       "aasdk: auto attach operations not completed (mgmt VLAN %d)",
/* FA_AGENT_ERR_AUTOATCH_UPDATE */     "aasdk: management VLAN updated (was %d, now %d) by aasdk",
/* FA_AGENT_ERR_MSGAUTH_LIMITED */     "aasdk: message authentication support not available",
/* FA_AGENT_INFO_REMOTE_SMLT_ACCEPT */ "aasdk: remote smlt peer asgn - accepted (%d/%d/%d)",
/* FA_AGENT_INFO_REMOTE_SMLT_DELETE */ "aasdk: remote smlt peer asgn - deleted (%d/%d/%d)",
/* FA_AGENT_INFO_REMOTE_SMLT_UPDATE */ "aasdk: remote smlt peer asgn - updated (%d/%d/%d)",
/* FA_AGENT_ERR_REM_SMLT_ACPT_FAIL */  "aasdk: remote smlt peer asgn - acceptance failure (%d/%d/%d)",
/* FA_AGENT_INFO_REMOTE_PEER_REMOVE */ "aasdk: remote smlt peer removal (%d/%d/%d)",
/* FA_AGENT_ERR_REMOVE_PEER_FAIL */    "aasdk: remote smlt peer removal issue detected",
/* FA_AGENT_INFO_PROXY_UPDATE */       "aasdk: proxy sync issue - update (%d/%d - %d)",
/* FA_AGENT_INFO_INT_CREATE_SUCCESS */ "aasdk: internal client (%d/%d/%d) creation success",
/* FA_AGENT_INFO_INT_DELETE_SUCCESS */ "aasdk: internal client (%d/%d/%d) deletion success",
/* FA_AGENT_INFO_INT_CREATE_FAIL */    "aasdk: internal client (%d/%d/%d) creation failure",
/* FA_AGENT_INFO_INT_DELETE_FAIL */    "aasdk: internal client (%d/%d/%d) deletion failure",
/* FA_AGENT_ERR_HOSTPROXY_LIMITED */   "aasdk: proxy not supported by element type",
/* FA_AGENT_ERR_CLNT_BIND_CONFLICT */  "aasdk: client binding conflict detected (%d/%d/%d)",
/* FA_AGENT_INFO_CALL_TRACE_0 */       "RTN: %s",
/* FA_AGENT_INFO_CALL_TRACE_1 */       "RTN: %s  PARAM: %d",
/* FA_AGENT_INFO_CALL_TRACE_2 */       "RTN: %s  PARAM: %d %d",
/* End marker */ NULL

};

/**************************************************************
 * Name: faAgentErrorMsg
 * Description:
 *    Generates a log message and potentially saves error
 *    message data for later processing.
 * Input Parameters:
 *    msgId - message identifier in fa_agt_log_messages
 *    msgType - msg destined for customer or engineering log
 *    msgParam1 - message parameter
 *    msgParam2 - message parameter
 *    msgParam3 - message parameter
 *    faAgentErrMsgStr - pointer to error message structure
 * Output Parameters:
 *    faAgentErrMsgStr - error message structure contains
 *                       error text and data if appropriate
 * Return Values:
 *    none
 **************************************************************/

void
faAgentErrorMsg (UINT16 msgId,
                 UINT16 msgType,
		 char  *msgGenerator,
                 UINT32 msgParam1,
                 UINT32 msgParam2,
                 UINT32 msgParam3,
                 faAgentErrorMessageStr_t *faAgentErrMsgStr)
{
    char output_string[AA_SDK_MSG_LEN_MAX];


    /* Range test the msgId value for safety purposes */
    if (msgId > (FA_AGENT_ERR_MSG_COUNT_TOTAL - 1))
        return;

    /* Log/display the message */
    switch (msgType)
    {

#if SYSDEF_PLATFORM == SYSDEF_PLATFORM_ERS
        case FA_AGENT_CUSTOMER_MSG:

            Log_Error(FA_AGT_ID, msgId, ERR_CUST_INFO,
                      NO_PRINT, msgParam1, msgParam2, msgParam3);

            break;

        case FA_AGENT_ENGINEERING_MSG:

            Log_Error(FA_AGT_ID, msgId, ERR_INFO,
                      NO_PRINT, msgParam1, msgParam2, msgParam3);

            break;
#endif


        case FA_AGENT_INFO_MSG_1:
        case FA_AGENT_INFO_MSG_2:
        case FA_AGENT_INFO_MSG_3:
        case FA_AGENT_INFO_MSG_4:
        case FA_AGENT_INFO_MSG_5:

            /* Check for 'special' display levels that only display msg subsets */
            if (faAgentConfigData.faAgentDisplayInfoMsgs == 6)
            {
                if (msgType == FA_AGENT_INFO_MSG_4)
                {
                    if ((msgId == FA_AGENT_INFO_CALL_TRACE_0) ||
                        (msgId == FA_AGENT_INFO_CALL_TRACE_1) ||
                        (msgId == FA_AGENT_INFO_CALL_TRACE_2))
                    {
		        snprintf(output_string, AA_SDK_MSG_LEN_MAX,
			         "\n%d ", faAgentTimeGet());
		        aasdk_output(output_string, msgType);
			
			snprintf(output_string, AA_SDK_MSG_LEN_MAX,
				 aa_agt_log_messages[msgId],
				 msgGenerator, msgParam1, msgParam2);				 
                    }
                    else
                    {
		        aasdk_output("\n",msgType);

		        snprintf(output_string, AA_SDK_MSG_LEN_MAX,
				 aa_agt_log_messages[msgId],
				 msgParam1, msgParam2, msgParam3);
                    }

		    aasdk_output(output_string, msgType);
                    aasdk_output("\n", msgType);

                }

                break;
            }

            if (((msgType == FA_AGENT_INFO_MSG_1) && (faAgentConfigData.faAgentDisplayInfoMsgs)) ||
                ((msgType == FA_AGENT_INFO_MSG_2) && (faAgentConfigData.faAgentDisplayInfoMsgs > 1)) ||
                ((msgType == FA_AGENT_INFO_MSG_3) && (faAgentConfigData.faAgentDisplayInfoMsgs > 2)) ||
                ((msgType == FA_AGENT_INFO_MSG_4) && (faAgentConfigData.faAgentDisplayInfoMsgs > 3)) ||
                ((msgType == FA_AGENT_INFO_MSG_5) && (faAgentConfigData.faAgentDisplayInfoMsgs > 4)))
            {


                aasdk_output("\n", msgType);

                snprintf(output_string, AA_SDK_MSG_LEN_MAX,
                         aa_agt_log_messages[msgId],
                         msgParam1, msgParam2, msgParam3);
                aasdk_output(output_string, msgType);
                aasdk_output("\n", msgType);

            }

            break;

        default:

            break;
    }

    /* Return error message data in error structure */
    /* if it is available */
    if (faAgentErrMsgStr != NULL)
    {
        faAgentErrMsgStr->msgTxt = msgId;
        faAgentErrMsgStr->msgParam1 = msgParam1;
        faAgentErrMsgStr->msgParam2 = msgParam2;
        faAgentErrMsgStr->msgParam3 = msgParam3;
        faAgentErrMsgStr->msgType = msgType;
    }

    return;
}

