/* aa-sdk/common/agent/aa_port.c */

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

#include <stdbool.h> /* bool, true, false */
#include <string.h>  /* NULL, mem*() */

#include "aasdk_errno.h" /* AA_SDK_E* */
#include "aasdk_lldpif_comm.h"

#include "aa_port.h"
#include "aa_agent.h"
#include "aa_agent_private.h"


/*
 * internal list utilities
 */

/* sanity consistency check list head and count */

static bool 
_aaPortsListCheck(void)
{
    aaPort_t *pPortsList;
    unsigned  iPortsCount;

    pPortsList  = faAgentConfigData.aaPortsList;
    iPortsCount = faAgentConfigData.aaPortsCount;

    if ( ( (pPortsList == NULL) && (iPortsCount != 0) ) ||
         ( (pPortsList != NULL) && (iPortsCount == 0) ) )
    {
        return false;
    }
    else
    {
	return true;
    }
}



/* search for an entry in the list */
/* != NULL if found, == NULL if not */

static aaPort_t *
_aaPortListFind(aasdk_port_id_t portId)
{
    aaPort_t *pPortsList;
    unsigned  iPortsCount;

    aaPort_t *pListPort;
    unsigned  iPortCount;

    pPortsList  = faAgentConfigData.aaPortsList;
    iPortsCount = faAgentConfigData.aaPortsCount;

    if (pPortsList == NULL)
        return NULL;

    for ( pListPort = pPortsList,      iPortCount = 0;
         (pListPort != NULL) &&       (iPortCount < iPortsCount);
          pListPort = pListPort->next, iPortCount++)
        {
	    if ( (pListPort != NULL ) && (pListPort->id == portId) )
	      return pListPort;
        }
     
    return NULL;
}


/* add a port to the tail of the list */

static void
_aaPortListAdd(aaPort_t *pPort)
{
    aaPort_t  *pPortsList;


    pPortsList = faAgentConfigData.aaPortsList;

    if (pPortsList == NULL)
    {
        /* list empty, make new port the head */

        pPort->next = NULL;
        pPort->prev = NULL;
        faAgentConfigData.aaPortsList = pPort;
    }
    else
    {
        /* list not empty, find the tail */

        while (pPortsList->next != NULL)
	{
	  pPortsList = pPortsList->next;
	}

	/* add new port to the tail */

	pPort->next = NULL;
        pPort->prev = pPortsList;
        pPortsList->next = pPort;
    }

    faAgentConfigData.aaPortsCount++;    
    return;
}



/* remove a port from the list */

static void
_aaPortsListRem(aaPort_t *pPort)
{
    if (pPort == NULL)
    {
        return;
    }

    if (faAgentConfigData.aaPortsList == pPort)
    {
        faAgentConfigData.aaPortsList  = pPort->next;
    }

    if (pPort->next != NULL)
    {
        pPort->next->prev = pPort->prev;
    }

    if (pPort->prev != NULL)
    {
        pPort->prev->next = pPort->next;
    }
 
    faAgentConfigData.aaPortsCount--;
    return;
}



/*
 * external functions
 */

int
aaPortCreate(aasdk_port_id_t portId)
{
    aaPort_t  *pPort;

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now check for duplicate ID */

    if (_aaPortListFind(portId) != NULL)
    {
        return AA_SDK_EADDRINUSE;
    }

    /* allocate port memory */
    
    pPort = aasdkx_mem_alloc(sizeof(aaPort_t));

    if (pPort == NULL)
    {
        return AA_SDK_ENOMEM;
    }

    /* init port entry */

    pPort->ena   = false;
    pPort->id    = portId;
    pPort->next  = NULL;
    pPort->prev  = NULL;
    memset( pPort->locSysId, 0, sizeof(pPort->locSysId));
    memset(&pPort->stats,    0, sizeof(pPort->stats));
    pPort->stats.aasdk_port_id = portId;

    /* insert into list */

    _aaPortListAdd(pPort);

    return AA_SDK_ENONE;
}



int
aaPortDestroy(aasdk_port_id_t portId)
{
    aaPort_t  *pPort;

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }
    
    /* search for the port */

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }

    /* remove from the list */

    _aaPortsListRem(pPort);

    /* free the memory */

    aasdkx_mem_free(pPort);

    return AA_SDK_ENONE;
}



int
aaPortStatusGet(aasdk_port_id_t portId,
                aasdk_port_status_t *portStatus)
{
                       aaPort_t *pPort;
    faDiscoveredElementsEntry_t *pDiscElem;

    /* check the parameters */

    if (portStatus == NULL)
    {
        return AA_SDK_EINVAL;
    }

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now find the entry*/


    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }


    /*
     * fill in the caller's status struct
     */

    /* fill in the AA port data */

    memset(portStatus, 0, sizeof(*portStatus));
    
    portStatus->port_id = pPort->id;
    portStatus->enabled = pPort->ena;
    memcpy(portStatus->loc_sys_id, pPort->locSysId,
    sizeof(portStatus->loc_sys_id));



    /* fill in the FA discovered element data */

    pDiscElem = faDiscoveredElementsFind((unsigned)portId, FA_AGENT_GET_EXACT);

    if (pDiscElem == NULL)
    {
        /* no element discovered */
        portStatus->elem_type = AA_SDK_ELEM_NOT_SUPPORTED;
    }
    else
    {
        /* element discovered */
        portStatus->elem_type = pDiscElem->elemType;
        portStatus->mgmt_vlan = pDiscElem->elemVlan;
        memcpy(portStatus->rem_sys_id, pDiscElem->chassisId,
	sizeof(portStatus->rem_sys_id));
    }
        
    return AA_SDK_ENONE;
}


int
aaPortStatsGet(aasdk_port_id_t     portId,
               aasdk_stats_data_t *portStats)
{
                       aaPort_t *pPort;

    /* check the parameters */

    if (portStats == NULL)
    {
        return AA_SDK_EINVAL;
    }

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now find the entry*/

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }

     /* fill in the caller's stats struct */

    *portStats                = pPort->stats;
     portStats->aasdk_port_id = pPort->id;
        
    return AA_SDK_ENONE;
}


int
aaPortStatsClr(aasdk_port_id_t     portId)
{
    aaPort_t *pPort;

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now find the entry*/

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }

     /* clear the port stats */

    memset(&pPort->stats, 0, sizeof(pPort->stats));
    pPort->stats.aasdk_port_id = pPort->id;
        
    return AA_SDK_ENONE;
}


int
aaPortStatInc(aasdk_port_id_t portId,
              size_t          offset)
{
    aaPort_t *pPort;
    void *port_stats_addr;
    void *glbl_stats_addr;
    uint32_t  *stat_addr;


    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now find the entry*/

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }


    /* increment the per-port stats */

    port_stats_addr = &pPort->stats;
    stat_addr = (uint32_t *) (port_stats_addr + offset);
    *stat_addr += 1;

    /* increment the global stats */

    glbl_stats_addr = &aaGlobalStats;
    stat_addr = (uint32_t *)(glbl_stats_addr + offset);
    *stat_addr += 1;
        
    return AA_SDK_ENONE;
}



int 
aaPortStateSet(aasdk_port_id_t portId,
               bool            portEna)
{
    aaPort_t *pPort;
    int       rc;

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* now find the entry*/

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }


    /* see if we need to change state */

    if (pPort->ena == portEna)
    {
	rc = AA_SDK_ENONE;
    }
    else
    {
        pPort->ena = portEna;
        rc = aasdkx_port_data_ena_set(portId, portEna);
    }

    return rc;
}


int 
aaPortsStateSet(bool portEna)
{
    aaPort_t *pListPort;
    unsigned  iListCount;
    int       rc;

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }

    /* loop init */

    pListPort  = faAgentConfigData.aaPortsList;
    iListCount = faAgentConfigData.aaPortsCount;
    rc = AA_SDK_ENONE;

    /* loop to process each port in the list */

    while ((pListPort != NULL) && (iListCount != 0))
    {
        /* change the state if needed */

        if (pListPort->ena == portEna)
        {
	    rc = AA_SDK_ENONE;
        }
        else
        {
            pListPort->ena = portEna;
            rc = aasdkx_port_data_ena_set(pListPort->id, portEna);
        }

	/* move to the next port */

	pListPort = pListPort->next;
	iListCount--;
    }

    /* if the count is wrong the list is corrupt */

    if ((pListPort != NULL) || (iListCount !=0))
    {
	rc = AA_SDK_ELOOP;
    }

    return rc;
}

int 
aaPortSysIdCfg(aasdk_port_id_t portId,
               uint8_t        *sysId)
{
    aaPort_t *pPort;
    int       rc;

    /* check the parameters */

    if (sysId == NULL)
    {
        return AA_SDK_EINVAL;
    }

    /* first sanity check the list */

    if (_aaPortsListCheck() == false)
    {
        return AA_SDK_ELOOP;
    }


    /* now find the entry*/

    pPort = _aaPortListFind(portId);

    if (pPort == NULL)
    {
        return AA_SDK_EADDRNOTAVAIL;
    }


    /* see if we need to change the sys ID */

    if ( memcmp(pPort->locSysId, sysId, AA_SDK_SYS_ID_LEN) == 0 )
    {
	rc = AA_SDK_ENONE;
    }
    else
    {
        memcpy(pPort->locSysId, sysId, AA_SDK_SYS_ID_LEN);
        rc = aasdkx_disc_sys_id_set(portId, sysId);
    }

    return rc;
}
