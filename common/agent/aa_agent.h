/* aa-sdk/common/agent/aa_agent.h */

#ifndef AA_AGENT_H
#define AA_AGENT_H

#include "aa_types.h"
#include "aa_port.h"
#include "aa_defs.h"

#include "aasdk_osif_plat.h"
#include "aasdk_msgif_comm.h"
#include "aasdk_osif_comm.h"


/* FA agent operational options */
#define FA_AGENT_SERVER_IMMEDIATE_DELETE
#undef FA_AGENT_SRVR_SIMULATION
#define FA_HOST_VLAN_AUTO_CREATE
#define FA_HOST_VLAN_AUTO_DELETE
#undef FA_AGENT_HOST_PROXY_SUPPORT
#undef FA_AGENT_DEMO_SUPPORT_ENABLED

#define INRANGE(a,b,c) (((a) <= (b)) && ((b) <= (c)))

#define FA_MAX_ISID        16777214

/*******************************************************
 Fabric Attach Agent ISID/VLAN Assignment Table Support
 *******************************************************/

#define FA_AGENT_GET_EXACT 1
#define FA_AGENT_GET_NEXT  2

#define FA_AGENT_LEXI_LT  -1
#define FA_AGENT_LEXI_EQ   0
#define FA_AGENT_LEXI_GT   1

/* Component flags */
#define FA_COMP_NONE         0x00
#define FA_COMP_VLAN         0x01
#define FA_COMP_VLAN_MEMBER  0x02
#define FA_COMP_VLAN_TAG     0x04
#define FA_COMP_VLAN_ISID    0x08
#define FA_COMP_PORT_PVID    0x10

/* Element state flags */
#define FA_ELEM_STATE_NONE          0x00
#define FA_ELEM_STATE_REJECT_MSG    0x01
#define FA_ELEM_STATE_DEACTIVATION  0x02

#define FA_ELEM_STATE_UPDATE_SET    1
#define FA_ELEM_STATE_UPDATE_CLEAR  2

/* Origin flags */
#define FA_ELEM_ORIGIN_PROXY        1
#define FA_ELEM_ORIGIN_CLIENT       2

typedef struct faLocalIsidVlanAsgnsEntry_s 
{
    unsigned int   isid;
    unsigned short vlan;
    unsigned char  status;
    unsigned char  components;

    unsigned short ifIndex;
    unsigned char  avail1;
    unsigned char  avail2;

    unsigned int   lastUpdated;

    unsigned char  elemState;
    unsigned char  origin;
    unsigned char  referenceCount;
    unsigned char  trunkId;

    struct faLocalIsidVlanAsgnsEntry_s *next;
    struct faLocalIsidVlanAsgnsEntry_s *prev;

} faLocalIsidVlanAsgnsEntry_t;

typedef struct faRemoteIsidVlanAsgnsEntry_s 
{
    unsigned int   isid;
    unsigned short vlan;
    unsigned char  status;
    unsigned char  components;

    unsigned short ifIndex;
    unsigned char  trunkId;
    unsigned char  avail1;

    unsigned int   lastUpdated;

    unsigned char  elemState;
    unsigned char  elemType;
    unsigned short clientVlan;

    struct faRemoteIsidVlanAsgnsEntry_s *next;
    struct faRemoteIsidVlanAsgnsEntry_s *prev;

} faRemoteIsidVlanAsgnsEntry_t;

/* Remote ISID/VLAN asgn expiration - 240 seconds */
#define FA_REMOTE_ISID_VLAN_ASGN_EXPIRATION     240

/* Immediate remote ISID/VLAN asgn delete - 10 seconds */
#define FA_REMOTE_ISID_VLAN_ASGN_IMMEDIATE_DEL  10

typedef enum
{
    FA_AGENT_ISID_VLAN_ASGN_UNKNOWN = 0,
    FA_AGENT_ISID_VLAN_ASGN_PENDING,      /* D_avFabricAttachIsidVlanAsgnsState_pending - 1 */
    FA_AGENT_ISID_VLAN_ASGN_ACTIVE,       /* D_avFabricAttachIsidVlanAsgnsState_active - 1 */
    FA_AGENT_ISID_VLAN_ASGN_REJECTED,     /* D_avFabricAttachIsidVlanAsgnsState_rejected - 1 */

    FA_ASGN_REJECTION_RESOURCE_ERR,
    FA_ASGN_REJECTION_DUPLICATE_ERR,
    FA_ASGN_REJECTION_INVALID_VLAN,
    FA_ASGN_REJECTION_UNKNOWN_VLAN,
    FA_ASGN_REJECTION_RESOURCE_VLAN,
    FA_ASGN_REJECTION_APP_INTERACTION,
    FA_ASGN_REJECTION_PEER_ACCEPTANCE

} faIsidVlanAsgnStates;

typedef struct faMinIsidVlanBindingEntry_s
{
    unsigned int   isid;
    unsigned short vlan;
    unsigned char  status;
    unsigned char  available1;

} faMinIsidVlanBindingEntry_t;

/*******************************************************
 Fabric Attach Agent Platform Support
 *******************************************************/

#define FA_AGENT_STATE_ENABLED                1  /* D_avFabricAttachService_enabled */
#define FA_AGENT_STATE_DISABLED               2  /* D_avFabricAttachService_disabled */

#define FA_AGENT_AA_STATE_ENABLED             1  /* D_avFabricAttachAutoAttachService_enabled */
#define FA_AGENT_AA_STATE_DISABLED            2  /* D_avFabricAttachAutoAttachService_disabled */

#define FA_AGENT_AA_VLAN_NOOP                 0  /* No-op indication used with AA */

#define FA_AGENT_FA_PORT_STATE_ENABLED        1  /* D_avFabricAttachPortState_enabled */
#define FA_AGENT_FA_PORT_STATE_DISABLED       2  /* D_avFabricAttachPortState_disabled */

#define FA_AGENT_HOST_PROXY_ENABLED           1  /* D_avFabricAttachHostProxy_enabled */
#define FA_AGENT_HOST_PROXY_DISABLED          2  /* D_avFabricAttachHostProxy_disabled */

#define FA_AGENT_CLEANUP_MODE_ENABLED         1  /* D_avFabricAttachCleanupMode_enabled */
#define FA_AGENT_CLEANUP_MODE_DISABLED        2  /* D_avFabricAttachCleanupMode_disabled */

#define FA_AGENT_MSG_AUTH_ENABLED             1  /* D_avFabricAttachMessageAuthentication_enabled */
#define FA_AGENT_MSG_AUTH_DISABLED            2  /* D_avFabricAttachMessageAuthentication_disabled */

#define FA_AGENT_ELEMENT_NOT_SUPPORTED        1  /* D_avFabricAttachElementType_notSupported */
#define FA_AGENT_ELEMENT_SERVER               2  /* D_avFabricAttachElementType_faServer */
#define FA_AGENT_ELEMENT_HOST                 3  /* D_avFabricAttachElementType_faHost */
#define FA_AGENT_ELEMENT_CLIENT_UNTAGGED      4  /* D_avFabricAttachElementType_faClientUntagged */
#define FA_AGENT_ELEMENT_CLIENT_TAGGED        5  /* D_avFabricAttachElementType_faClientTagged */

#define FA_AGENT_ELEMENT_INT_CLIENT_UNTAGGED  16  /* Perform untagged client link conditioning */
#define FA_AGENT_ELEMENT_INT_CLIENT_TAGGED    17  /* Perform tagged client link conditioning */
#define FA_AGENT_ELEMENT_INT_CLIENT_UPLINK    18  /* Perform uplink conditioning only */

#define FA_ELEM_CLIENT(elemType)        ((((elemType) >= FA_AGENT_ELEMENT_CLIENT_UNTAGGED) && \
                                          ((elemType) <= FA_AGENT_ELEMENT_CLIENT_TAGGED)) || \
                                         (((elemType) >= FA_AGENT_ELEMENT_INT_CLIENT_UNTAGGED) && \
                                          ((elemType) <= FA_AGENT_ELEMENT_INT_CLIENT_UPLINK)))

#define FA_ELEM_INT_CLIENT(elemType)    (((elemType) >= FA_AGENT_ELEMENT_INT_CLIENT_UNTAGGED) && \
                                         ((elemType) <= FA_AGENT_ELEMENT_INT_CLIENT_UPLINK))

#define FA_ELEM_EXT_CLIENT(elemType)    (((elemType) >= FA_AGENT_ELEMENT_CLIENT_UNTAGGED) && \
                                         ((elemType) <= FA_AGENT_ELEMENT_CLIENT_TAGGED))

#define FA_AGENT_GLOBAL_SPBM_ENABLED          1
#define FA_AGENT_GLOBAL_SPBM_DISABLED         2

#define FA_AGENT_MAX_ISID_VLAN_ASGNS          94     /* Limited by max LLDP TLV size */
#define FA_AGENT_REM_MAX_ISID_VLAN_ASGNS      1000

#define FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN    32
#define FA_AGENT_MAX_PRIMARY_SERVER_DESCR_LEN 128

#define FA_AGENT_FA_ELEMENT_TLV_CHASSIS_ID    10     /* Aligns with AVAYA_FA_ELEMENT_TLV_CHASSIS_ID_LEN */

#define FA_AGENT_MAX_TRUNK_LINKS              16

#define FA_AGENT_LOCAL_ASGN_INDICATOR         0

#define FA_AGENT_MSG_AUTH_KEY_MIN_LEN         1
#define FA_AGENT_MSG_AUTH_KEY_MAX_LEN         32

#define FA_AGENT_MSG_AUTH_KEY_DEFAULT         "Avaya Fabric Connect Is The Way"

/* Primary server expiration - 240 seconds */
#define FA_AGENT_PRIMARY_SERVER_EXPIRATION    240

#define FA_AGENT_STATE_MULT_SERVERS_SEEN      0x01
#define FA_AGENT_STATE_AA_FAILURE_SEEN        0x02
#define FA_AGENT_STATE_PROCESSING_BLOCKED     0x04

typedef struct faDiscoveredElementsEntry_s 
{
    unsigned char  elemType;
    unsigned char  available;
    unsigned short elemVlan;

    unsigned int   ifIndex;
    unsigned int   lastUpdated;

    unsigned char  chassisIdLen;
    unsigned char  avail1;
    unsigned short avail2;
    unsigned char  chassisId[FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN];

    struct faDiscoveredElementsEntry_s *next;
    struct faDiscoveredElementsEntry_s *prev;

} faDiscoveredElementsEntry_t;

typedef enum 
{
    FA_AGENT_MSG_UNDEFINED = 0,
    FA_AGENT_MSG_DATA_DIST,
    FA_AGENT_MSG_DATABASE_XCHG,
    FA_AGENT_MSG_CONFIG_DATA,
    FA_AGENT_MSG_LOCAL_ASGN_DATA,
    FA_AGENT_MSG_REMOTE_ASGN_DATA,
    FA_AGENT_MSG_STKMGR_EVENT,
    FA_AGENT_MSG_LIST_MAINT_TIMER,
    FA_AGENT_MSG_LLDP_DISCOVERY_UPDATE,
    FA_AGENT_MSG_LLDP_BINDING_UPDATE,
    FA_AGENT_MSG_BU_SIGNAL,
    FA_AGENT_MSG_PRIMARY_SERVER_DATA,
    FA_AGENT_MSG_NV_SAVE_TIMER,
    FA_AGENT_MSG_RESCHEDULE_NV_SAVE,
    FA_AGENT_MSG_GLOBAL_SPBM_UPDATE,
    FA_AGENT_MSG_SMLT_PEER,
    FA_AGENT_MSG_MLT_EVENT,
    FA_AGENT_MSG_SMLT_EVENT,
    FA_AGENT_MSG_LOCAL_PEND_DATA,
    FA_AGENT_MSG_REMOTE_PEND_DATA,
    FA_AGENT_MSG_PEND_DATA_XCHG,

} faAgentMsgTypes;

typedef enum 
{
    FA_AGENT_MSG_SUB_UNDEFINED = 0,
    FA_AGENT_MSG_SUB_CREATE,
    FA_AGENT_MSG_SUB_DELETE,
    FA_AGENT_MSG_SUB_UPDATE,
    FA_AGENT_MSG_SUB_DELETE_ALL

} faAgentMsgSubTypes;

/* Data message header. Payload follows header */
/* in data distribution and other IMC messages */
typedef struct faAgentDistDataMsg_s {

    UINT8  faAgentDataMsgType;
    UINT8  faAgentDataMsgSubType;
    UINT8  faDataMsgParam1;
    UINT8  faDataMsgParam2;
    UINT32 faDataMsgParam3;

    struct faAgentDistDataMsg_s *faAgentDataMsgNext;

} faAgentDataDistMsg_t;

/* Temporary agent state values */
#define FA_AGENT_TEMP_STATE_NONE        0x00
#define FA_AGENT_TEMP_STATE_REJECT_ALL  0x01

typedef struct faAgentConfigData_s {

    UINT8  faServiceStatus;
    UINT8  faElementType;
    UINT8  faAutoAttachStatus;
    UINT8  faCleanupMode;

    UINT8  faMsgAuthStatus;
    UINT8  faMsgAuthKeyLength;
    UINT8  faHostProxyStatus;
    UINT8  faAgentPrimaryServerTrunk;

    UINT8  faMsgAuthKey[FA_AGENT_MSG_AUTH_KEY_MAX_LEN];

    UINT8  faAgentPrimaryServerId[FA_AGENT_MAX_PRIMARY_SERVER_ID_LEN];
    UINT8  faAgentPrimaryServerDescr[FA_AGENT_MAX_PRIMARY_SERVER_DESCR_LEN];

    UINT16 faMgmtVlan;
    UINT8  faAgentPrimaryServerIdLen;
    UINT8  faAgentPrimaryServerDescrLen;

    UINT8  faAgentDisplayInfoMsgs;
    UINT16 faDiscoveredElementsCount;

    UINT8  faAgentPrimaryServerUnit;
    UINT8  faAgentPrimaryServerPort;
    UINT8  faAgentState;
    UINT8  faAgentTemporaryState;

    UINT32 faAgentPrimaryServerLastUpdated;

    UINT32 faAgentServerTrunkIfIndex[FA_AGENT_MAX_TRUNK_LINKS];
    UINT32 faAgentServerTrunkLastUpdated[FA_AGENT_MAX_TRUNK_LINKS];

    UINT16 faLocalIsidVlanAsgnsCount;
    UINT16 faRemoteIsidVlanAsgnsCount;
    
    faLocalIsidVlanAsgnsEntry_t *faLocalIsidVlanAsgnsTable;
    faRemoteIsidVlanAsgnsEntry_t *faRemoteIsidVlanAsgnsTable;

    faLocalIsidVlanAsgnsEntry_t *pendingFaLocalIsidVlanAsgns;
    faRemoteIsidVlanAsgnsEntry_t *pendingFaRemoteIsidVlanAsgns;

    faDiscoveredElementsEntry_t *faDiscoveredElements;

    faAgentDataDistMsg_t *pendingMsgData;

    UINT16           aaPortsCount;
    struct aaPort_s *aaPortsList;

} faAgentConfigData_t;

extern faAgentConfigData_t faAgentConfigData;

extern aasdk_mutex_id_t  faAgentMut;

extern aasdk_msgque_id_t faAgentMsgQ;

#define FA_NV_SAVE           FALSE
#define FA_NO_NV_SAVE        TRUE
#define FA_NOTIFY_LLDP       TRUE
#define FA_NO_NOTIFY_LLDP    FALSE
#define FA_VALIDATE_DATA     FALSE
#define FA_NO_VALIDATE_DATA  TRUE
#define FA_DISTRIBUTE        FALSE
#define FA_NO_DISTRIBUTE     TRUE
#define FA_RESET             TRUE
#define FA_NO_RESET          FALSE
#define FA_DISCOVERY         TRUE
#define FA_NO_DISCOVERY      FALSE
#define FA_ASSIGNMENT        FALSE
#define FA_NO_ASSIGNMENT     TRUE

typedef struct faAgentStatsData_s {

    UINT32 faAgentPrimaryServerLost;
    UINT32 faAgentMultipleServersDetected;
    UINT32 faAgentMsgAuthFailures;
    UINT32 faAgentAutoAttachFailures;
    UINT32 faAgentIsidVlanAsgnUpdates;
    UINT32 faAgentIsidVlanAsgnExpirations;
    UINT32 faAgentHostAcceptanceIssues;
    UINT32 faAgentHostRejectionIssues;
    UINT32 faAgentHostCleanupIssues;
    UINT32 faAgentServerAcceptanceIssues;
    UINT32 faAgentServerCleanupIssues;
    UINT32 faAgentServerImmedDeleteIssues;
    UINT32 faAgentServerAsgnExpireIssues;
    UINT32 faAgentServerSmltPeerAcceptanceIssues;
    UINT32 faAgentServerSmltPeerRejectionIssues;
    UINT32 faAgentServerSmltPeerCleanupIssues;
    UINT32 faAgentLldpSyncIssues;
    UINT32 faAgentProxyAcceptanceIssues;
    UINT32 faAgentProxyRejectionIssues;
    UINT32 faAgentProxyCleanupIssues;
    UINT32 faAgentProxyImmedDeleteIssues;
    UINT32 faAgentProxyAsgnExpireIssues;
    UINT32 faAgentServersInterfaceIssues;

} faAgentStatsData_t;

extern faAgentStatsData_t faAgentStatsData;

extern aasdk_stats_data_t aaGlobalStats;


#ifdef SYSDEF_INCLUDE_FA_SMLT













#endif

#define FA_AGENT_VIRTUAL_IFC_OFFSET  10000

#define FA_AGENT_ACTUAL_IFC(x)        ((x >= 1) && (x < FA_AGENT_VIRTUAL_IFC_OFFSET))
#define FA_AGENT_VIRTUAL_IFC(x)       (x >= FA_AGENT_VIRTUAL_IFC_OFFSET)
#define FA_AGENT_VIRTUALIZE_IFC(x)    (x + FA_AGENT_VIRTUAL_IFC_OFFSET)
#define FA_AGENT_DEVIRTUALIZE_IFC(x)  (x - FA_AGENT_VIRTUAL_IFC_OFFSET)

#ifdef SYSDEF_INCLUDE_FA_SMLT


































#endif /* SYSDEF_INCLUDE_FA_SMLT */

/*******************************************************
 Log/Error Message Support
 *******************************************************/

typedef struct faAgentErrorMessageStr_s {

    UINT32 msgTxt;
    UINT32 msgParam1;
    UINT32 msgParam2;
    UINT32 msgParam3;
    UINT32 msgType;

} faAgentErrorMessageStr_t;

#define FA_AGENT_CUSTOMER_MSG     1
#define FA_AGENT_ENGINEERING_MSG  2
#define FA_AGENT_INFO_MSG_1       3
#define FA_AGENT_INFO_MSG_2       4
#define FA_AGENT_INFO_MSG_3       5
#define FA_AGENT_INFO_MSG_4       6
#define FA_AGENT_INFO_MSG_5       7
#define FA_AGENT_INFO_MSG_MAX     7

typedef enum 
{
    FA_AGENT_INFO_SERVICE_ENABLED = 0,
    FA_AGENT_INFO_SERVICE_DISABLED,
    FA_AGENT_INFO_REMOTE_ACCEPT,
    FA_AGENT_INFO_REMOTE_DELETE,
    FA_AGENT_INFO_REMOTE_UPDATE,
    FA_AGENT_ERR_REMOTE_ACCEPT_FAIL,
    FA_AGENT_ERR_TIMER_UNAVAILABLE,
    FA_AGENT_ERR_NV_SAVE_FAILURE,
    FA_AGENT_ERR_NV_RESTORE_FAILURE,
    FA_AGENT_INFO_NV_SAVE_SUCCESS,
    FA_AGENT_INFO_LLDP_DELETE,
    FA_AGENT_INFO_LLDP_ADD,
    FA_AGENT_INFO_LLDP_UPDATE,
    FA_AGENT_ERR_SERVICE_NOT_AVAIL,
    FA_AGENT_ERR_SERVICE_LIMITED,
    FA_AGENT_ERR_RESOURCES_EXCEEDED,
    FA_AGENT_ERR_ASGN_DUPLICATION,
    FA_AGENT_ERR_GENERAL_FAILURE,
    FA_AGENT_ERR_ASGN_UNKNOWN,
    FA_AGENT_ERR_ASGN_CLIENT_CTRL,
    FA_AGENT_ERR_ASGN_CREATION_FAIL,
    FA_AGENT_ERR_ASGN_DELETION_FAIL,
    FA_AGENT_ERR_ASGN_UPDATE_UNK,
    FA_AGENT_ERR_ASGN_UPDATE_STATE,
    FA_AGENT_ERR_ASGN_IFC_INVALID,
    FA_AGENT_INFO_MSG_RECEIVED,
    FA_AGENT_INFO_UNK_MSG_RECEIVED,
    FA_AGENT_INFO_LOCAL_CREATE,
    FA_AGENT_INFO_LOCAL_DELETE,
    FA_AGENT_INFO_LOCAL_UPDATE,
    FA_AGENT_ERR_ASGN_INVALID_VLAN,
    FA_AGENT_INFO_MULTIPLE_SERVERS,
    FA_AGENT_INFO_PRIM_SERVER_LOST,
    FA_AGENT_ERR_ASGN_ACTIVE_FAIL,
    FA_AGENT_ERR_ASGN_ACPT_VLAN,
    FA_AGENT_ERR_ASGN_ACPT_MEMBER,
    FA_AGENT_ERR_ASGN_ACPT_TAGGING,
    FA_AGENT_ERR_ASGN_ACPT_PVID,
    FA_AGENT_ERR_ASGN_ACPT_SW_UNI,
    FA_AGENT_ERR_ASGN_REJT_VLAN,
    FA_AGENT_ERR_ASGN_REJT_MEMBER,
    FA_AGENT_ERR_ASGN_REJT_SW_UNI,
    FA_AGENT_ERR_ASGN_REJT_PVID,
    FA_AGENT_ERR_ASGN_DEACTIVE_FAIL,
    FA_AGENT_ERR_ASGN_RESET_ISSUE,
    FA_AGENT_ERR_ASGN_EXPIRE_FAIL,
    FA_AGENT_INFO_REMOTE_EXPIRE,
    FA_AGENT_INFO_ELEM_TYPE_UPDATE,
    FA_AGENT_ERR_ELEM_TYPE_UPDATE,
    FA_AGENT_ERR_UNSUPPORTED_OP_HOST,
    FA_AGENT_ERR_UNSUPPORTED_OP_SRVR,
    FA_AGENT_ERR_NV_CLEANUP_FAILURE,
    FA_AGENT_INFO_REMOTE_HOST_REMOVE,
    FA_AGENT_ERR_HOST_REMOVE_FAIL,
    FA_AGENT_ERR_SRVR_REJT_GEN,
    FA_AGENT_ERR_SRVR_REJT_RES,
    FA_AGENT_ERR_SRVR_REJT_INV_VLAN,
    FA_AGENT_ERR_SRVR_REJT_RES_VLAN,
    FA_AGENT_ERR_SRVR_REJT_APP,
    FA_AGENT_ERR_SRVR_REJT_PEER,
    FA_AGENT_INFO_AUTOATCH_ENABLED,
    FA_AGENT_INFO_AUTOATCH_DISABLED,
    FA_AGENT_ERR_AUTOATCH_LIMITED,
    FA_AGENT_ERR_AUTOATCH_VLAN,
    FA_AGENT_ERR_AUTOATCH_MEMBER,
    FA_AGENT_ERR_AUTOATCH_TAGGING,
    FA_AGENT_ERR_AUTOATCH_DHCP,
    FA_AGENT_ERR_AUTOATCH_MGMT,
    FA_AGENT_ERR_AUTOATCH_PVID,
    FA_AGENT_ERR_AUTOATCH_FAIL,
    FA_AGENT_ERR_AUTOATCH_UPDATE,
    FA_AGENT_ERR_MSGAUTH_LIMITED,
    FA_AGENT_INFO_REMOTE_SMLT_ACCEPT,
    FA_AGENT_INFO_REMOTE_SMLT_DELETE,
    FA_AGENT_INFO_REMOTE_SMLT_UPDATE,
    FA_AGENT_ERR_REM_SMLT_ACPT_FAIL,
    FA_AGENT_INFO_REMOTE_PEER_REMOVE,
    FA_AGENT_ERR_REMOVE_PEER_FAIL,
    FA_AGENT_INFO_PROXY_UPDATE,
    FA_AGENT_INFO_INT_CREATE_SUCCESS,
    FA_AGENT_INFO_INT_DELETE_SUCCESS,
    FA_AGENT_INFO_INT_CREATE_FAIL,
    FA_AGENT_INFO_INT_DELETE_FAIL,
    FA_AGENT_ERR_HOSTPROXY_LIMITED,
    FA_AGENT_ERR_CLNT_BIND_CONFLICT,
    FA_AGENT_INFO_CALL_TRACE_0,
    FA_AGENT_INFO_CALL_TRACE_1,
    FA_AGENT_INFO_CALL_TRACE_2,
    FA_AGENT_NULL_LOG

} faAgentLogMsgData;

#define FA_AGENT_ERR_MSG_COUNT_TOTAL FA_AGENT_NULL_LOG

extern char *fa_agt_log_messages[];

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

extern void
faAgentErrorMsg (UINT16 msgId,
                 UINT16 msgType,
                 char  *msgGenerator,
                 UINT32 msgParam1,
                 UINT32 msgParam2,
                 UINT32 msgParam3,
                 faAgentErrorMessageStr_t *faAgentErrMsgStr);

/**************************************************************
 Internal agent routines
 *************************************************************/

extern faIsidVlanAsgnStates
faLocalIsidVlanAsgnValidate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData);

extern faLocalIsidVlanAsgnsEntry_t *
faLocalIsidVlanAsgnFind (unsigned int isid, unsigned int vlan, int searchType);

extern int
faLocalIsidVlanAsgnCreate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                           int notifyLldp, int validateData);

extern int
faLocalIsidVlanAsgnDelete (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                           int notifyLldp);

/*******************************************************
 External application notification support             
 *******************************************************/

typedef void (* FaNotificationHandler)(unsigned int events, void *context);

#define FA_EVENT_SERVER_DISCOVERED  0x00000001
#define FA_EVENT_SERVER_LOST        0x00000002
#define FA_EVENT_PROXY_ENABLED      0x00000004
#define FA_EVENT_PROXY_DISABLED     0x00000008

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

extern int
faRegisterCallback (unsigned int tgtEvents, FaNotificationHandler callback, void *context);

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

extern void
faUnregisterCallback (int faCallbackId);

#define FA_REM_SERVER_STATUS_UNKNOWN   1
#define FA_REM_SERVER_STATUS_ACTIVE    2  /* Server available/discovered */
#define FA_REM_SERVER_STATUS_INACTIVE  3  /* Server not available */

#define FA_REM_SERVER_UPLINK_UNKNOWN   1  /* Server not available */
#define FA_REM_SERVER_UPLINK_IFINDEX   2  /* Uplink ID is an ifIndex */
#define FA_REM_SERVER_UPLINK_TRUNKID   3  /* Uplink ID is a Trunk ID */

typedef struct faRemoteServerInfo_s
{
    int serverStatus;
    int serverUplinkType;
    int serverUplinkId;

} faRemoteServerInfo_t;

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

extern int
faGetRemoteServerInfo (faRemoteServerInfo_t *serverInfo);


/**************************************************************
 * Name: faAgentTimeGet
 * Description:
 *    Gets the current time
 * Input Parameters:
 *    none
 * Output Parameters:
 *    none
 * Return Values:
 *    time in seconds
 **************************************************************/
unsigned
faAgentTimeGet(void);


/**************************************************************
 * Name: faAgentDelay
 * Description:
 *    Delays for a specified time
 * Input Parameters:
 *    sec - delay in seconds
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/
void
faAgentDelay(unsigned sec);


/**************************************************************
 Public routines
 *************************************************************/

int FA_AGT_init(task_spawn_t *tsInfo);

void faAgent_main_init(void);

int  faAgent_main_proc(aasdk_msg_t *pmsg);


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

extern int faAgentServiceEnabled ();

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

extern int faAgentGetServiceStatus ();

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

extern int faAgentGetElementType ();


/**************************************************************
 * Name: faAgentGetMgmtVlan
 * Description:
 *    Used to query the current FA management VLAN
 * Input Parameters:
 *    pMgmtVlan - ptr to caller's variable
 * Output Parameters:
 *    none
 * Return Values:
 *    none
 **************************************************************/

extern void faAgentGetMgmtVlan(uint16_t *pMgmtVlan);

extern int faAgentGetAutoAttachStatus ();
extern int faAgentGetMsgAuthStatus ();
extern int faAgentGetMsgAuthKey (char *authKey, int *authKeyLen);

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

extern int faAgentGetHostProxyStatus ();

extern int faAgentGetPrimaryServerId (int *serverIdLen,
                                      unsigned char *serverId);
extern int faAgentGetPrimaryServerDescr (int *serverDescrLen,
                                         unsigned char *serverDescr);

extern int faAgentGetLocalIsidVlanAsgnsCount ();
extern int faAgentGetRemoteIsidVlanAsgnsCount ();
extern int faAgentGetRemoteIsidVlanAsgnsCountByIfc (unsigned int tgtIfIndex);

extern int faAgentSetServiceStatus (int status, int saveToNv, int distributeData);
extern int faAgentSetAutoAttachStatus (int status, int saveToNv);
extern int faAgentSetCleanupMode (int status, int saveToNv);
extern int faAgentSetMsgAuthStatus (int status, int saveToNv);
extern int faAgentSetMsgAuthKey (char *authKey, int authKeyLen, int saveToNv);
extern int faAgentSetHostProxyStatus (int status, int saveToNv);

extern int faAgentSetElementType (int elementType, int saveToNv, 
                                  int distributeData, int elementReset);
extern void faAgentSetMgmtVlan (unsigned short mgmtVlan);
extern void faAgentSetSysId (uint8_t *sysId);

extern void faAgentGlobalSpbmUpdate (unsigned int status);

extern int faLocalIsidVlanAsgnQuery (unsigned int isid,
                                     unsigned int vlan,
                                     int searchType,
                                     faLocalIsidVlanAsgnsEntry_t *faIsidVlanData);

extern int faRemoteIsidVlanAsgnQuery (unsigned int ifIndex,
                                      unsigned int isid,
                                      unsigned int vlan,
                                      int searchType,
                                      faRemoteIsidVlanAsgnsEntry_t *faIsidVlanData);

extern int faAgentIsDeviceActiveFabricAttachHost ();
extern int faAgentIsDeviceActiveFabricAttachServer ();

extern int faLocalIsidVlanAsgnVlanPresent (unsigned int vlan);
extern int faLocalIsidVlanAsgnVlanRequired (unsigned int vlan);
extern int faLocalIsidVlanAsgnVlanActive (unsigned int vlan);
extern int faLocalIsidVlanAsgnVlanFaCreated (unsigned int vlan);

extern int faLocalIsidVlanAsgnConflict (unsigned int interface,
                                        unsigned int isid,
                                        unsigned int vlan,
                                        unsigned int elemSrc);

extern int faRemoteIsidVlanAsgnConflict (unsigned int interface,
                                         unsigned int isid,
                                         unsigned int vlan,
                                         unsigned int elemSrc);

extern int faRemoteIsidVlanAsgnVlanFaCreated (unsigned int vlan);
extern int faRemoteSwitchedUniFaCreated (unsigned int interface, 
                                         unsigned int isid,
                                         unsigned int vlan);

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

extern int
faDiscoveredElementsQuery (int ifIndex,
                           int searchType,
                           int *elemType,
                           int *elemVlan,
                           int *elemDescrLen,
                           unsigned char *elemDescr);

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

/**************************************************************
 * Name: faInternalClientCreate
 * Description:
 *    Initiates creation of an internal FA client using the
 *    specified ifIndex/I-SID/VLAN data for client assignment
 *    creation. Clients are either tagged or untagged.
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

extern int
faInternalClientCreate (unsigned int interface,
                        unsigned int isid,
                        unsigned int vlan,
                        unsigned int elemType,
                        unsigned int elemVlan);

/**************************************************************
 * Name: faInternalClientDelete
 * Description:
 *    Initiates deletion of an internal FA client using the
 *    specified ifIndex/I-SID/VLAN data for client assignment
 *    deletion. Externally accessible routine.
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

extern int
faInternalClientDelete (unsigned int interface,
                        unsigned int isid,
                        unsigned int vlan,
                        unsigned int elemType);

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

extern int
faInternalClientQuery (unsigned int interface,
                       unsigned int isid,
                       unsigned int vlan,
                       unsigned int *status,
                       unsigned int *clientVlan);

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

#endif /* AA_AGENT_H */

