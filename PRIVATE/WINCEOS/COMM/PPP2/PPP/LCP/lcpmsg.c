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
*   @module lcpmsg.c | PPP LCP Layer Message Processing
*
*   Date:   7-17-95
*
*   @comm
*/


//  Include Files

#   include "windows.h"
#   include "types.h"

#include "cxport.h"
#include "crypt.h"

// VJ Compression Include Files

#include "ndis.h"
#include "tcpip.h"
#include "vjcomp.h"

// PPP Include Files

#include "protocol.h"
#include "ppp.h"
#include "auth.h"
#include "lcp.h"
#include "ncp.h"
#include "mac.h"
#include "pppserver.h"

void
pppLcpRcvData(
	IN	PVOID	  context,
	IN	pppMsg_t *pMsg )
{
	PLCPContext  pContext;
	
	pContext = (PLCPContext)context;

	ASSERT(PPP_PROTOCOL_LCP == pMsg->ProtocolType);

	PppFsmProcessRxPacket(pContext->pFsm, pMsg->data, pMsg->len);
}

void
pppLcpSendProtocolReject(
	IN	PVOID	      context,
	IN	pppMsg_t      *pMsg )
//
//  Send an LCP protocol reject packet for a protocol that
//  we do not recognize or have enabled.
//
{
	PLCPContext  pContext  = (PLCPContext)context;
	pppSession_t *pSession = pContext->session;
	DWORD        cbResponsePacket;
	BYTE         abResponsePacket[PPP_DEFAULT_MTU];
	DWORD        dwResult;
	PBYTE        pPacket =  pMsg->pPPPFrame;
	DWORD        cbPacket = pMsg->cbPPPFrame;

	//
	// Per RFC1661 5.7
	//  "If the LCP automaton is in the Opened state, then this must
	//   be reported back by transmitting a Protocol-Reject"
	// 
	if (pContext->pFsm->state == PFS_Opened)
	{
		//
		// Per RFC1661 5.7
		//	"The Rejected-Information MUST be truncated to comply with
		//   the peer's established MRU."
		//
		// Currently, for simplicity, assume default MRU
		//
		// Also, we currently just send the protocol field as it was
		// received, so it could be sent in 1 byte compressed form.

		if ((PPP_PACKET_HEADER_SIZE + cbPacket) > PPP_DEFAULT_MTU)
			cbPacket = PPP_DEFAULT_MTU - PPP_PACKET_HEADER_SIZE;

		cbResponsePacket = PPP_PACKET_HEADER_SIZE + cbPacket;

		ASSERT((PPP_PACKET_HEADER_SIZE + cbPacket) <= PPP_DEFAULT_MTU);
		ASSERT(cbResponsePacket <= PPP_DEFAULT_MTU);

		PPP_SET_PACKET_HEADER(abResponsePacket, LCP_PROTOCOL_REJECT, pContext->idNextProtocolReject, cbResponsePacket);
		pContext->idNextProtocolReject++;
		memcpy(&abResponsePacket[PPP_PACKET_HEADER_SIZE], pPacket, cbPacket);

		dwResult = pppSendData(pSession, PPP_PROTOCOL_LCP, abResponsePacket, cbResponsePacket);
	}
}