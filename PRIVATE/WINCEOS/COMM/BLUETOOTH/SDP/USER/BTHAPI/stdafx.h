//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft shared
// source or premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the SOURCE.RTF on your install media or the root of your tools installation.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__A8AA627B_1D36_4984_844D_04662498E050__INCLUDED_)
#define AFX_STDAFX_H__A8AA627B_1D36_4984_844D_04662498E050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



// STRICT is defined by the default WinCE build environment.
#if defined (UNDER_CE) && !defined (STRICT)
#define STRICT
#endif

#if defined (WINCE_EMULATION)
// atlbase.h (2.1) on WinCE includes these in by default, not on NT.
// #include <ntstatus.h>  // ntstatus must come before windows.h
#include <windows.h>
#include <winnls.h>
#include <ole2.h>
#include "ceport.h"
#else
#define _OLEAUT32_
#define _OLE32_
#endif


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#if ! defined (WINCE_EMULATION)
#define WIN32_NO_STATUS
#endif

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#if ! defined (WINCE_EMULATION)
#undef WIN32_NO_STATUS
// #include <ntstatus.h>
#include "ceport.h"
#endif

#include "bt_sdp.h"

#endif // !defined(AFX_STDAFX_H__A8AA627B_1D36_4984_844D_04662498E050__INCLUDED)
