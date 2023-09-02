//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//

/**********************************************************************/
/**                        Microsoft Windows                         **/
/**********************************************************************/

/*
	helper.h
	
	info needed by afd's helper dll's

*/

#ifndef _HELPER_H_
#define _HELPER_H_

#include "..\pm\lib\afdws2.h"

#ifndef VXDAPI
#define VXDAPI		__cdecl
#endif

//	Helper Function Indices into the AFD VTable

#define	AFD_NETBIOS_IDX		24
#define AFD_DHCP_IDX		25


//	Afd Function Typedefs

typedef SOCKHAND (VXDAPI *AFN_Socket)(
	DWORD AddressFamily,
	DWORD SocketType,
	DWORD Protocol,
	DWORD CatId,
	GUID *ProvId);

typedef DWORD (VXDAPI *AFN_Control)(
	DWORD		Protocol,
	DWORD       Action,
	LPVOID      InputBuffer,
	LPDWORD     InputBufferLength,
	LPVOID      OutputBuffer,
	LPDWORD     OutputBufferLength);

typedef DWORD (VXDAPI *AFN_EnumProtocolsW)(
	LPINT           lpiProtocols,
	LPVOID          lpProtocolBuffer,
	LPDWORD         lpdwBufferLength);

typedef DWORD (*AFN_GetHostentByAttr)(
	LPSOCK_THREAD 			pThread,
	LPSTR					Name,		
	LPBYTE					Address);	

typedef DWORD (*AFN_AddIPHostent)(
	LPWSTR		 			Hostname,		
	DWORD					IpAddr,			
	LPWSTR					Aliases,			
	LPFILETIME				lpFileTime);	
  
typedef DWORD (VXDAPI *AFN_Select)(
	UINT				ReadCount,
	LPSOCK_LIST			ReadList,
	UINT				WriteCount,
	LPSOCK_LIST			WriteList,
	UINT				ExceptCount,
	LPSOCK_LIST			ExceptList,
	const struct timeval * timeout,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (*AFN_AddResolver)(
	DWORD	CONTEXT,
	BOOL	fDNS,
	BOOL	fDelete,
	DWORD	IpAddr);

typedef DWORD(*AFN_AddInterface)(
	PTSTR	pAdapter,
	VOID	*Nte,
	DWORD	Context,
	INT		Flags,
	UINT	IpAddr,
	UINT	SubnetMask,
	int		cDns,
	UINT	*pDns,
	int		cWins,
	UINT	*pWins);

typedef BYTE (VXDAPI *AFN_ProxyNameQuery)(
    UCHAR *pDnsQuery, 
    ULONG ulQueryLen, 
    BOOL fIsDnsQuery,
    UCHAR *pDnsResponse, 
    ULONG *pulResponseLen,
    BOOL *pfInitiateAutodial);


//	Socket Function Typedefs

typedef DWORD (VXDAPI *ASF_Close) (
    LPSOCK_INFO Socket);

typedef DWORD (VXDAPI *ASF_Accept)(
    LPSOCK_INFO   ListeningSocket,
    PSOCKHAND	  ConnectedSocket,
    LPSOCKADDR    Address,
    LPDWORD       AddressLength,
    LPCONDITIONPROC pfnCondition,
    DWORD dwCallbackData,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_Bind)(
    LPSOCK_INFO   Socket,
    LPSOCKADDR    Address,
    DWORD         AddressLength,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_Connect)(
    LPSOCK_INFO   Socket,
    LPSOCKADDR    Address,
    DWORD         AddressLength,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_Ioctl)(
    LPSOCK_INFO Socket,
    DWORD Code, 
    VOID *pInBuf, 
    DWORD cInBuf,
    VOID *pOutBuf, 
    DWORD cOutBuf, 
    DWORD *pcRet, 
    WSAOVERLAPPED *pOv,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE pCompRtn, 
    LPWSATHREADID pThreadId,
	CRITICAL_SECTION *pDllCS);

typedef DWORD (VXDAPI *ASF_Listen)(
	LPSOCK_INFO			Socket,
	DWORD				Backlog,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI * ASF_RecvImpl)(
    LPSOCK_INFO   Socket,
    WSABUF        *pWsaBufs,
    DWORD         cWsaBufs,
    DWORD         *pcRcvd,
    WSABUF        *pControl,
    DWORD         *pFlags,
    LPSOCKADDR    pAddr,
    LPDWORD       pcAddr,
    void          *pOv,
    void          *pCompRtn,
    void          *pThreadId,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI * ASF_Send)(
    LPSOCK_INFO   Socket,
    WSABUF        *pWsaBufs,
    DWORD         cWsaBufs,
    DWORD         *pcSent,
    DWORD         Flags,
    LPSOCKADDR    pAddr,
    DWORD         cAddr,
    void          *pOv,
    void          *pCompRtn,
    void          *pThreadId,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_Shutdown)(
    LPSOCK_INFO Socket,
    DWORD       How,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (*ASF_Getsockname)(
			   LPSOCK_INFO	Socket,
			   LPSOCKADDR	Address,
			   LPDWORD		pAddressLength,
				CRITICAL_SECTION	*pDllCS);

typedef DWORD (*ASF_Getpeername)(
			   LPSOCK_INFO	Socket,
			   LPSOCKADDR	Address,
			   LPDWORD		pAddressLength,
			   CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_GetOption)(
    LPSOCK_INFO Socket,
    DWORD       Level,
    DWORD       OptionName,
    LPVOID      Buffer,
    LPDWORD     BufferLength,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_SetOption)(
    LPSOCK_INFO Socket,
    DWORD       Level,
    DWORD       OptionName,
    LPVOID      Buffer,
    DWORD       BufferLength,
	CRITICAL_SECTION	*pDllCS);

typedef DWORD (VXDAPI *ASF_Wakeup)(
    LPSOCK_INFO   Socket,
    DWORD         Event,
    DWORD         Status,
	CRITICAL_SECTION	*pDllCS);


//	AFI: Afd Function Index - Helper Dll index
#define AFI_Socket			2
#define AFI_Control			3
#define AFI_EnumProtocolsW	4
#define AFI_GetHostenByAddr	7
#define AFI_AddIPHostent	8
#define AFI_Select			10
#define AFI_AddResolver		19
#define AFI_AddInterface	19
#define AFI_ProxyNameQuery  36

#define CallAfd(FnName)		(*((AFN_##FnName) (v_apAfdFns[AFI_##FnName]))) 

//	ASI: Afd Socket Index - Helper Dll index

#define ASI_Close			0
#define ASI_Wait			1
#define ASI_Accept			2
#define ASI_Bind			3
#define ASI_Connect			4
#define ASI_Ioctl			5
#define ASI_Listen			6
#define ASI_RecvImpl		7
#define ASI_Send			8
#define ASI_Shutdown		9
#define ASI_Getsockname		10
#define ASI_Getpeername		11
#define ASI_GetOption		12
#define ASI_SetOption		13
#define ASI_Wakeup			14


#define CallSock(FnName) (*((ASF_##FnName)(v_apSocketFns[ASI_##FnName])))

// AFDAddInterface Flags
#define ADD_INTF_DELETE_FL		0x0001

#endif	/*	_HELPER_H_	*/


