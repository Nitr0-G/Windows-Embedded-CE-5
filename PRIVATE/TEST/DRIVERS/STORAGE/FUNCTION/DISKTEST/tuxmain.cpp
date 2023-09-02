//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#include "main.h"

#undef __FILE_NAME__
#define __FILE_NAME__   _T("TUXMAIN.CPP")

// debugging
#define Debug NKDbgPrintfW

// Global CKato logging object.  Set while processing SPM_LOAD_DLL message.
CKato *g_pKato = NULL;

// Global shell info structure.  Set while processing SPM_SHELL_INFO message.
SPS_SHELL_INFO *g_pShellInfo;

// Global critical section to be used by threaded tests if necessary.
CRITICAL_SECTION g_csProcess;

extern TCHAR optArg[];

#define BASE 4000

// extern "C"
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Tux testproc function table
//
FUNCTION_TABLE_ENTRY g_lpFTE[] = {
    _T("basic mass storage device functionality tests"),  0,                0,            0,  NULL,
    _T( "Format mass storage disk"                    ),  1,                0,    BASE +  2,  TestFormatMedia,    
    _T( "Write, Read, Verify 1 sector at at time"     ),  1,                1,    BASE +  3,  TestReadWriteSeq,
    _T( "Write, Read, Verify 8 sectors at a time"     ),  1,                8,    BASE +  4,  TestReadWriteSeq,
    _T( "Write, Read, Verify 32 sectors at a time"    ),  1,               32,    BASE +  5,  TestReadWriteSeq,   
    _T( "Write, Read, Verify increasing buffer sizes" ),  1,                0,    BASE +  6,  TestReadWriteSize,

    // these tests will only work if driver supports multiple sg buffers
    _T("multiple SG buffer read/write tests"          ),  0,                0,            0,  NULL,
    _T( "Write, read multi-SG 2 sectors at a time"    ),  1,                2,    BASE +  7,  TestReadWriteMulti,
    _T( "Write, read multi-SG 8 sectors at a time"    ),  1,                8,    BASE +  8,  TestReadWriteMulti,
    _T( "Write using 2-8 SG buffers, read + verify"   ),  1,                0,    BASE +  9,  TestSGWrite,
    _T( "Write, read using 2-8 SG buffers + verify"   ),  1,                0,    BASE + 10,  TestSGRead,
    _T( "Read + write with 2 SG buffers, varying size"),  1,                0,    BASE + 11,  TestSGBoundsCheck,

    _T("out-of-bounds read/write tests"               ),  0,                0,            0,  NULL,
    _T( "Read buffer extends past last sector"        ),  1,                0,    BASE + 12,  TestReadPastEnd,
    _T( "Read starts past final sector"               ),  1,                0,    BASE + 13,  TestReadOffDisk,    
    _T( "Write buffer extends past last sector"       ),  1,                0,    BASE + 14,  TestWritePastEnd,
    _T( "Write starts past final sector"              ),  1,                0,    BASE + 15,  TestWriteOffDisk,  

    _T("misc. additional mass storage device tests"   ),  0,                0,            0,  NULL,
    _T( "Delete sector test cases"                    ),  1,                0,    BASE + 16,  TestInvalidDeleteSectors,  
    _T( "Write using various buffer types"            ),  1,                0,    BASE + 17,  TestWriteBufferTypes,  
    _T( "Read using various buffer types"             ),  1,                0,    BASE + 18,  TestReadBufferTypes,  



    NULL,                                           0,  0,                  0,          NULL
};
// --------------------------------------------------------------------

// --------------------------------------------------------------------
BOOL WINAPI 
DllMain(
    HANDLE hInstance, 
    ULONG dwReason, 
    LPVOID lpReserved ) 
// --------------------------------------------------------------------    
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);
    
    switch( dwReason )
      {
      case DLL_PROCESS_ATTACH:         
            Debug(TEXT("%s: DLL_PROCESS_ATTACH\r\n"), __FILE_NAME__);           
            return TRUE;

      case DLL_PROCESS_DETACH:
            Debug(TEXT("%s: DLL_PROCESS_DETACH\r\n"), __FILE_NAME__);
            return TRUE;

      }
    return FALSE;
}

#ifdef __cplusplus
}
#endif
// end extern "C"

static void ProcessCmdLine( LPCTSTR );

static void Usage()
{    
    Debug(TEXT(""));
    Debug(TEXT("DISKTEST: Usage: tux -o -d disktest -c\"/disk <disk> /profile <profile> /maxsectors <count> /oldioctls\""));
    Debug(TEXT("       /disk <disk>        : name of the disk to test (e.g. DSK1:); default = first detected disk"));
    Debug(TEXT("       /profile <profile>  : limit to devices of the specified storage profile; default = all profiles"));
    Debug(TEXT("       /maxsectors <count> : maximum number of sectors per operation; default = %u"), DEF_MAX_SECTORS);
    Debug(TEXT("       /oldioctls          : use legacy DISK_IOCTL_* codes"));
    Debug(TEXT("       /store              : open the disk using the OpenStore() API"));
    Debug(TEXT(""));

}


// --------------------------------------------------------------------
SHELLPROCAPI 
ShellProc(
    UINT uMsg, 
    SPPARAM spParam ) 
// --------------------------------------------------------------------    
{
    LPSPS_BEGIN_TEST pBT = {0};
    LPSPS_END_TEST pET = {0};

    switch (uMsg) {
    
        // --------------------------------------------------------------------
        // Message: SPM_LOAD_DLL
        //
        case SPM_LOAD_DLL: 
            Debug(_T("ShellProc(SPM_LOAD_DLL, ...) called"));

            // If we are UNICODE, then tell Tux this by setting the following flag.
            #ifdef UNICODE
                ((LPSPS_LOAD_DLL)spParam)->fUnicode = TRUE;
            #endif

            // turn on kato debugging
            KatoDebug(1, KATO_MAX_VERBOSITY,KATO_MAX_VERBOSITY,KATO_MAX_LEVEL);

            // Get/Create our global logging object.
            g_pKato = (CKato*)KatoGetDefaultObject();

            // Initialize our global critical section.
            InitializeCriticalSection(&g_csProcess);

            Usage();

            return SPR_HANDLED;        

        // --------------------------------------------------------------------
        // Message: SPM_UNLOAD_DLL
        //
        case SPM_UNLOAD_DLL: 
            Debug(_T("ShellProc(SPM_UNLOAD_DLL, ...) called"));

            // This is a good place to delete our global critical section.
            DeleteCriticalSection(&g_csProcess);

            return SPR_HANDLED;      

        // --------------------------------------------------------------------
        // Message: SPM_SHELL_INFO
        //
        case SPM_SHELL_INFO:
            Debug(_T("ShellProc(SPM_SHELL_INFO, ...) called"));
      
            // Store a pointer to our shell info for later use.
            g_pShellInfo = (LPSPS_SHELL_INFO)spParam;
        
            if( g_pShellInfo->szDllCmdLine && *g_pShellInfo->szDllCmdLine )
            {
                // Display our Dlls command line if we have one.
                g_pKato->Log( LOG_DETAIL, 
                    _T("Command Line: \"%s\"."), g_pShellInfo->szDllCmdLine);

                ProcessCmdLine(g_pShellInfo->szDllCmdLine);
            } 

        return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_REGISTER
        //
        case SPM_REGISTER:
            Debug(_T("ShellProc(SPM_REGISTER, ...) called"));            
            
            ((LPSPS_REGISTER)spParam)->lpFunctionTable = g_lpFTE;
            #ifdef UNICODE
                return SPR_HANDLED | SPF_UNICODE;
            #else
                return SPR_HANDLED;
            #endif
            
        // --------------------------------------------------------------------
        // Message: SPM_START_SCRIPT
        //
        case SPM_START_SCRIPT:
            Debug(_T("ShellProc(SPM_START_SCRIPT, ...) called")); 

            g_hDisk = OpenDiskHandle();

            if(INVALID_HANDLE(g_hDisk))
            {
                g_pKato->Log(LOG_FAIL, _T("FAIL: unable to open Hard Disk media"));
            }
            
            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_STOP_SCRIPT
        //
        case SPM_STOP_SCRIPT:

            if(VALID_HANDLE(g_hDisk))
            {
                g_pKato->Log(LOG_DETAIL, _T("Closing disk handle..."));
                CloseHandle(g_hDisk);
            }
            
            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_BEGIN_GROUP
        //
        case SPM_BEGIN_GROUP:
            Debug(_T("ShellProc(SPM_BEGIN_GROUP, ...) called"));
            g_pKato->BeginLevel(0, _T("BEGIN GROUP: FSDTEST.DLL"));
            
            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_END_GROUP
        //
        case SPM_END_GROUP:
            Debug(_T("ShellProc(SPM_END_GROUP, ...) called"));
            g_pKato->EndLevel(_T("END GROUP: TUXDEMO.DLL"));
            
            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_BEGIN_TEST
        //
        case SPM_BEGIN_TEST:
            Debug(_T("ShellProc(SPM_BEGIN_TEST, ...) called"));

            // Start our logging level.
            pBT = (LPSPS_BEGIN_TEST)spParam;
            g_pKato->BeginLevel(pBT->lpFTE->dwUniqueID, 
                                _T("BEGIN TEST: \"%s\", Threads=%u, Seed=%u"),
                                pBT->lpFTE->lpDescription, pBT->dwThreadCount,
                                pBT->dwRandomSeed);

            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_END_TEST
        //
        case SPM_END_TEST:
            Debug(_T("ShellProc(SPM_END_TEST, ...) called"));

            // End our logging level.
            pET = (LPSPS_END_TEST)spParam;
            g_pKato->EndLevel(_T("END TEST: \"%s\", %s, Time=%u.%03u"),
                              pET->lpFTE->lpDescription,
                              pET->dwResult == TPR_SKIP ? _T("SKIPPED") :
                              pET->dwResult == TPR_PASS ? _T("PASSED") :
                              pET->dwResult == TPR_FAIL ? _T("FAILED") : _T("ABORTED"),
                              pET->dwExecutionTime / 1000, pET->dwExecutionTime % 1000);

            return SPR_HANDLED;

        // --------------------------------------------------------------------
        // Message: SPM_EXCEPTION
        //
        case SPM_EXCEPTION:
            Debug(_T("ShellProc(SPM_EXCEPTION, ...) called"));
            g_pKato->Log(LOG_EXCEPTION, _T("Exception occurred!"));
            return SPR_HANDLED;

        default:
            Debug(_T("ShellProc received bad message: 0x%X"), uMsg);
            ASSERT(!"Default case reached in ShellProc!");
            return SPR_NOT_HANDLED;
    }
}

static
void
ProcessCmdLine( 
    LPCTSTR szCmdLine )
{
    CClParse cmdLine(szCmdLine);

    if(!cmdLine.GetOptString(_T("disk"), g_szDiskName, MAX_PATH))
    {
        StringCchCopyEx(g_szDiskName, MAX_PATH, _T(""), NULL, NULL, STRSAFE_NULL_ON_FAILURE);
    }
    if(!cmdLine.GetOptString(_T("profile"), g_szProfile, PROFILENAMESIZE))
    {
        StringCchCopyEx(g_szDiskName, PROFILENAMESIZE, _T(""), NULL, NULL, STRSAFE_NULL_ON_FAILURE);
    }
    if(!cmdLine.GetOptVal(_T("maxsectors"), &g_dwMaxSectorsPerOp))
    {
        g_dwMaxSectorsPerOp = DEF_MAX_SECTORS;
    }
    g_fOldIoctls = cmdLine.GetOpt(_T("oldioctls"));
    g_fOpenAsStore = cmdLine.GetOpt(_T("store"));

    // a user specified disk name will override a user specified profile
    if(g_szDiskName[0])
    {
        g_pKato->Log(LOG_DETAIL, _T("DISKTEST: Disk Device Name = %s"), g_szDiskName);
    }
    else if(g_szProfile[0])
    {
        g_pKato->Log(LOG_DETAIL, _T("DISKTEST: Storage Profile = %s"), g_szProfile);
    }
    
    if(g_fOldIoctls)
    {
        g_pKato->Log(LOG_DETAIL, _T("DISKTEST: Forcing use of legacy DISK_IOCTL* style control codes"));
    }
    
    if(g_fOpenAsStore)
    {
        g_pKato->Log(LOG_DETAIL, _T("DISKTEST: Will open disk as a store (using OpenStore())"));
    }
    
    g_pKato->Log(LOG_DETAIL, _T("DISKTEST: Max Sectors per operation = %u"), g_dwMaxSectorsPerOp);
}
