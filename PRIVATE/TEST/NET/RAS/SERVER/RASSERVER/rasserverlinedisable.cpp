//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#include "RasServerTest.h"

extern BOOL bExpectedFail;
extern LineToAdd lineToAdd;

TESTPROCAPI RasServerLineDisable(UINT uMsg, 
						 TPPARAM tpParam, 
						 LPFUNCTION_TABLE_ENTRY lpFTE) 
{	
	HANDLE	hHandle=NULL;
	RASCNTL_SERVERSTATUS *pStatus	= NULL;
	RASCNTL_SERVERLINE   Line;
	DWORD					dwStatus = TPR_PASS;
	LPRASDEVINFO		 pRasDevInfo= NULL;
	DWORD	dwResult=ERROR_SUCCESS, dwExpectedResult=ERROR_SUCCESS, cbStatus=ERROR_SUCCESS;
	DWORD	cDevices=0;
	DWORD TargetedLineToAdd=(lineToAdd==DefaultDevice0)? 0: (lineToAdd==OneL2TPLine)? 2: (lineToAdd==OnePPTPLine)?1:0;

    // Check our message value to see why we have been called
    if (uMsg == TPM_QUERY_THREAD_COUNT) 
	{
		((LPTPS_QUERY_THREAD_COUNT)tpParam)->dwThreadCount = 0;
		return TPR_HANDLED;
    } 
	else if (uMsg != TPM_EXECUTE) 
	{
		return TPR_NOT_HANDLED;
    }
	
	if(bExpectedFail)
	{
		//
		// Try the IOCTL here. This should fail with ERROR_NOT_SUPPORTED
		//
		dwResult = RasIOControl(NULL, RASCNTL_SERVER_LINE_DISABLE, NULL, 0, NULL, 0, &cbStatus);
		RasPrint(TEXT("RasIOControl() FAIL'ed (%d) AS EXPECTED "), dwResult);
		return (dwResult==ERROR_NOT_SUPPORTED? TPR_PASS: TPR_FAIL);
	}
	

	//
	// Remove the required line (if it is added already)
	//
	RemoveAllLines();
	
	//
	//	Get all available RAS devices
	//
	dwResult = GetLineDevices(&pRasDevInfo, &cDevices, &TargetedLineToAdd);
	if (!cDevices)
	{
		RasPrint(TEXT("cDevices == 0. No RAS Devices Available"));
		return TPR_SKIP;
	}
	
	//
	//	Copy device info to line
	//
	memcpy(&(Line.rasDevInfo), &(pRasDevInfo[TargetedLineToAdd]), sizeof(RASDEVINFO));
	dwResult = RasIOControl(NULL, RASCNTL_SERVER_LINE_ADD, (PUCHAR)&Line, sizeof(Line), NULL, 0, &cbStatus);
	if (dwResult != 0)
	{
		RasPrint(TEXT("Unable to add a line for RASCNTL_SERVER_LINE_DISABLE"));
		return TPR_SKIP;
	}	
	dwResult = RasIOControl(NULL, RASCNTL_SERVER_LINE_ENABLE, (PUCHAR)&Line, sizeof(Line), NULL, 0, &cbStatus);
	if (dwResult != 0)
	{
		RasPrint(TEXT("Unable to enable a line for RASCNTL_SERVER_LINE_DISABLE"));
		return TPR_SKIP;
	}
	
	switch(LOWORD(lpFTE->dwUserData))
	{
		case RASSERVER_ALREADY_DISABLED:
			dwResult = RasIOControl(NULL, RASCNTL_SERVER_LINE_DISABLE, (PUCHAR)&Line, sizeof(Line), NULL, 0, &cbStatus);
			if (dwResult != 0)
			{
				RasPrint(TEXT("Unable to disable a line for RASCNTL_SERVER_LINE_DISABLE"));
				return TPR_FAIL;
			}
			break;
			
		case RASSERVER_NONEXISTENT_DISABLED:
			_tcscpy(Line.rasDevInfo.szDeviceName, TEXT("No Such Device"));
			dwResult = RasIOControl(NULL, RASCNTL_SERVER_LINE_ADD, (PUCHAR)&Line, sizeof(Line), NULL, 0, &cbStatus);
			if (dwResult != 0)
			{
				RasPrint(TEXT("Unable to add a line for RASCNTL_SERVER_LINE_ENABLE"));
				return TPR_FAIL;
			}	
			break;

		case RASSERVER_VALID_DISABLED:
			break;
	}	

	//
	// Call the IOCTL
	//
	memcpy(&(Line.rasDevInfo), &(pRasDevInfo[TargetedLineToAdd]), sizeof(RASDEVINFO));
	dwResult = RasIOControl(
						hHandle, 
						RASCNTL_SERVER_LINE_DISABLE, 
						(PUCHAR)&Line, sizeof(Line),
						NULL, 0, 
						&cbStatus
						);
	RasPrint(TEXT("dwResult = %d\tdwExpectedResult = %d"), dwResult, dwExpectedResult);

	if(dwResult != dwExpectedResult)
	{
		dwStatus=TPR_FAIL;
	}

	if(pStatus)
		LocalFree(pStatus);	

	CleanupServer();
	return dwStatus;
}
