//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
/*****************************************************************************
* 
*
*   @doc
*   @module fsm.c | PPP NCP Layer and Internal Event Engines
*
*   Date:   1-10-96
*
*   @comm   This file contains the layer related Finite State Machine
*   @comm   events and event handling functions.
*/


//  Include Files

#include "windows.h"
#include "cclib.h"
#include "cxport.h"

// VJ Compression Include Files

#include "ndis.h"
#include "tcpip.h"
#include "vjcomp.h"


// PPP Include Files

#include "protocol.h"
#include "ppp.h"
#include "crypt.h"
#include "auth.h"
#include "lcp.h"
#include "ipcp.h"
#include "ccp.h"
#include "ncp.h"
#include "ip_intf.h"
#include "mac.h"
#include "pppserver.h"

/*****************************************************************************
* 
*   @func   void | NcpLowerLayerUp | The lower layer has come up.
*
*   @parm   void * | context | Instance context pointer.
*               
*   @comm 
*/

void
NcpLowerLayerUp( ncpCntxt_t *c_p )
{
	DWORD	ncpProto;

	if (FALSE == c_p->bLowerLayerUp)
	{
        c_p->bLowerLayerUp = TRUE;

		// Indicate lower layer up to all enabled protocols

		for (ncpProto = 0; ncpProto < NCP_MAX_PROTOCOLS; ncpProto++)
		{
			if( c_p->protocol[ncpProto].enabled )
			{
       			(c_p->protocol[ncpProto].LowerLayerUp)(c_p->protocol[ncpProto].context);
			}
		}
	}
}

/*****************************************************************************
* 
*   @func   void | NcpLowerLayerDown | The lower layer has gone down.
*
*   @parm   void * | context | Instance context pointer.
*               
*   @comm 
*/

void
NcpLowerLayerDown( ncpCntxt_t *c_p )
{
	DWORD	ncpProto;

    ASSERT( c_p );

    c_p->bLowerLayerUp = FALSE;

	ncpIdleDisconnectTimerStop(c_p);

    // Reset all protocols

	for (ncpProto = 0; ncpProto < NCP_MAX_PROTOCOLS; ncpProto++)
	{
		if( c_p->protocol[ncpProto].enabled )
		{
       		(c_p->protocol[ncpProto].LowerLayerDown)(c_p->protocol[ncpProto].context);
		}
	}
}


