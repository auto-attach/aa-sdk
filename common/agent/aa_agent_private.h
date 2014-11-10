/* aa-sdk/common/agent/aa_agent_private.h */

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

#ifndef AA_AGENT_PRIVATE_H
#define AA_AGENT_PRIVATE_H

#include <stdint.h>

#include "aa_sysdefs.h"
#include "aa_agent.h"

#include "aasdk_msgif_comm.h"


#ifdef SYSDEF_INCLUDE_SPBM

#endif /* SYSDEF_INCLUDE_SPBM */



/***** External declarations *****/

/** Platform **/

typedef struct FaNotificationEntry_s
{
    FaNotificationHandler callback;
    unsigned int          tgtEvents;
    void                  *context;

} FaNotificationEntry_t;

#define FA_NOTIFICATION_CALLBACKS  4

extern FaNotificationEntry_t faCallbackTable[FA_NOTIFICATION_CALLBACKS];

extern int faAgentPrimaryServerExpiration;
extern int faAgentRemoteIsidVlanAsgnExpiration;

extern int faWriteAgentNvData (int newUnitNumber);

extern void 
faAgentSignalEvent (unsigned int eventId,
                    unsigned int eventParam1,
                    unsigned int eventParam2,
                    unsigned int eventParam3);

extern void faAgentNvDataDump ();

extern void 
faAgentProcessTimer (int timerId, int usrData);

extern void faDiscoveredElementsDump ();

extern int
faDiscoveredElementsCreate (unsigned int ifIndex,
                            unsigned int elemType,
                            unsigned int elemVlan,
                            unsigned char *chassisId,
                            unsigned int chassisIdLen);

extern faDiscoveredElementsEntry_t *
faDiscoveredElementsFind (unsigned int ifIndex,
                          int searchType);

extern int faDiscoveredElementsClearAll ();

extern void faNotifyApps (int currentEvent);

/** Host/Local **/

extern void 
faAgentProcessServerDiscovery (aasdk_msg_t *msg);

extern void 
faHostProcessRemoteElem (unsigned int interface,
                         unsigned int isid,
                         unsigned int vlan,
                         unsigned int status,
                         unsigned int elemType,
                         unsigned int elemVlan);

extern int 
faHostAsgnRejectionProcessing (faLocalIsidVlanAsgnsEntry_t *faLocIsidVlanAsgn,
                               unsigned int interface, 
                               int newStatus);

extern int
faHostAsgnRejectionExternalAppAccess (faLocalIsidVlanAsgnsEntry_t *faLocIsidVlanAsgn,
                                      unsigned int unit,
                                      unsigned int port);

extern void 
faHostAsgnReset (int fullReset);

extern void faHostAsgnPartialReset ();

extern int
faLocalIsidVlanAsgnUpdate (faLocalIsidVlanAsgnsEntry_t *faIsidVlanAsgnData);

extern void faLocalIsidVlanAsgnDump ();

extern void 
faAgentPerformServerMaintenance (int forcedExpiration);

extern int 
faLocalIsidVlanAsgnsClearAll (int notifyLldp);

extern void
faLocalIsidVlanAsgnSync (int syncState, int clearLldpData);

extern void
faLocalIsidVlanAsgnUpdateState (unsigned int isid,
                                unsigned int vlan,
                                unsigned int update,
                                unsigned char state);

#ifdef FA_AGENT_HOST_PROXY_SUPPORT

faIsidVlanAsgnStates
faProxyProcessRemoteElem (unsigned int interface,
                          unsigned int isid,
                          unsigned int vlan,
                          unsigned int status,
                          unsigned int elemSrc,
                          unsigned int elemVlan);

extern void
faAgentPerformProxyListMaintenance (unsigned int expiration,
                                    unsigned int tgtIfc);

extern void 
faProxyPostUpdateProcess (unsigned int interface);

extern int
faProxyAsgnRejectionProcessing (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                unsigned int rejectionStatus);

extern int 
faProxyAsgnRejectionExternalAppAccess (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                       unsigned int unit,
                                       unsigned int port);

extern void
faAgentPerformProxyListHostRemoval (unsigned int unit, unsigned int port);

#endif /* FA_AGENT_HOST_PROXY_SUPPORT */

/** Server/Remote **/

extern void 
faUpdateRemoteIsidVlanAsgns (unsigned int updateData,
                             unsigned int faBindingCount,
                             faMinIsidVlanBindingEntry_t *faBindingList);

extern void faRemoteIsidVlanAsgnDump (int elementTypeStart, int elementTypeEnd);

extern faIsidVlanAsgnStates
faServerProcessRemoteElem (unsigned int interface,
                           unsigned int isid,
                           unsigned int vlan,
                           unsigned int status,
                           unsigned int elemType,
                           unsigned int elemVlan);

extern int 
faServerAsgnRejectionExternalAppAccess (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                        unsigned int unit,
                                        unsigned int port);

extern int 
faServerAsgnRejectionProcessing (faRemoteIsidVlanAsgnsEntry_t *faRemIsidVlanAsgn,
                                 unsigned int rejectionStatus);

extern void 
faServerAsgnReset (int notifyLldp);

extern void 
faServerPostUpdateProcess (unsigned int interface);

extern int 
faRemoteIsidVlanAsgnUpdate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                            int distributeData);

extern faIsidVlanAsgnStates 
faRemoteIsidVlanAsgnValidate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData);

extern void 
faAgentPerformRemoteListMaintenance (unsigned int expiration, unsigned int tgtIfc);

extern faRemoteIsidVlanAsgnsEntry_t *
faRemoteIsidVlanAsgnFind (unsigned int ifIndex, unsigned int isid, unsigned int vlan, int searchType);

extern int
faRemoteIsidVlanAsgnCreate (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData);

extern int
faRemoteIsidVlanAsgnDelete (faRemoteIsidVlanAsgnsEntry_t *faIsidVlanAsgnData,
                            int notifyLldp);

extern int 
faRemoteIsidVlanAsgnsClearAll (int notifyLldp);

extern void 
faRemoteIsidVlanAsgnsClearLldpData ();

extern void
faRemoteIsidVlanAsgnSync (int syncState, int clearLldpData);

extern void
faAgentPerformRemoteListHostRemoval (unsigned int unit, unsigned int port);

extern void
faRemoteIsidVlanAsgnUpdateState (unsigned int interface,
                                 unsigned int isid,
                                 unsigned int vlan,
                                 unsigned int timestamp,
                                 unsigned int update,
                                 unsigned char state);

extern int
faAgentPerformRemoteElemRemoval (unsigned int interface,
                                 unsigned int isid,
                                 unsigned int vlan);

extern int
faDownlinkAutoAttachProcessing (unsigned int elemVlan,
                                unsigned int elemType,
                                unsigned int ifIndex);

/** Misc FA **/

#ifdef SYSDEF_INCLUDE_SPBM






#endif

extern int 
GDS_fa_lldpEvent (unsigned int events, void *info, void *context);

extern void 
faAgentSetMgmtVlan (unsigned short mgmtVlan);

extern void
faAgentSetSysId(uint8_t *sysId);

/** External Apps **/


#ifdef SYSDEF_INCLUDE_MLT




#endif

#endif /* ndef AA_AGENT_PRIVATE_H */

