//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
////////////////////////////////////////////////////////////////////////////////
//
//  CDDATEST TUX DLL
//
//  Module: main.h
//          Header for all files in the project.
//
//  Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_H__
#define __MAIN_H__

////////////////////////////////////////////////////////////////////////////////
// Included files

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <tux.h>
#include <kato.h>
#include <clparse.h>
#include <devutils.h>
#include <atapi2.h>
#include <cdioctl.h>
#include <storemgr.h>
#include <diskio.h>

////////////////////////////////////////////////////////////////////////////////
// Suggested log verbosities

#define LOG_EXCEPTION          0
#define LOG_FAIL               2
#define LOG_ABORT              4
#define LOG_SKIP               6
#define LOG_NOT_IMPLEMENTED    8
#define LOG_PASS              10
#define LOG_DETAIL            12
#define LOG_COMMENT           14

////////////////////////////////////////////////////////////////////////////////

// kato logging macros
#define FAIL(x)     g_pKato->Log( LOG_FAIL, \
                        TEXT("FAIL in %s at line %d: %s"), \
                        TEXT(__FILE__), __LINE__, TEXT(x) )

// same as FAIL, but also logs GetLastError value
#define ERRFAIL(x)  g_pKato->Log( LOG_FAIL, \
                        TEXT("FAIL in %s at line %d: %s; error %u"), \
                        TEXT(__FILE__), __LINE__, TEXT(x), GetLastError() )

#define ABORT(x)     g_pKato->Log( LOG_ABORT, \
                        TEXT("ABORT in %s at line %d: %s"), \
                        TEXT(__FILE__), __LINE__, TEXT(x) )

// same as ABORT, but also logs GetLastError value
#define ERRABORT(x)  g_pKato->Log( LOG_ABORT, \
                        TEXT("ABORT in %s at line %d: %s; error %u"), \
                        TEXT(__FILE__), __LINE__, TEXT(x), GetLastError() )

#define WARN(x)     g_pKato->Log( LOG_DETAIL, \
                        TEXT("WARNING in %s at line %d: %s"), \
                        TEXT(__FILE__), __LINE__, TEXT(x) )

// same as WARN, but also logs GetLastError value
#define ERRWARN(x)  g_pKato->Log( LOG_DETAIL, \
                        TEXT("WARNING in %s at line %d: %s; error %u"), \
                        TEXT(__FILE__), __LINE__, TEXT(x), GetLastError() )

// log's an error, but doesn't log a failure
#define ERR(x)      g_pKato->Log( LOG_DETAIL, \
                        TEXT("ERROR in %s at line %d: %s"), \
                        TEXT(__FILE__), __LINE__, TEXT(x) )

#define DETAIL(x)   g_pKato->Log( LOG_DETAIL, TEXT(x) )

#define COMMENT(x)  g_pKato->Log( LOG_COMMENT, TEXT(x) )

#define SKIP(x)     g_pKato->Log( LOG_SKIP, TEXT(x) )

#define UNIMPL(x)   g_pKato->Log( LOG_NOT_IMPLEMENTED, TEXT(x) )

TESTPROCAPI GetTestResult(void);

void LOG(LPWSTR szFmt, ...);

HANDLE OpenCDDAHandle(void);


#endif // __MAIN_H__
