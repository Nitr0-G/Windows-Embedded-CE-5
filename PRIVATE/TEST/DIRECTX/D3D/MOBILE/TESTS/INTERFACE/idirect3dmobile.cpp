//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#define INITGUID

#include "IDirect3DMobile.h"
#include "DebugOutput.h"
#include "KatoUtils.h"
#include "BufferTools.h"
#include "D3DMStrings.h"
#include <tux.h>
#include <tchar.h>
#include <stdio.h>

//
// Allow prefast pragmas
//
#pragma warning(disable:4068)  // warning 4068: unknown pragma

//
// GUID for invalid parameter tests
//
DEFINE_GUID(IID_D3DQAInvalidInterface, 
0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

//
//  IDirect3DMobileTest
//
//    Constructor for IDirect3DMobileTest class; however, most initialization is deferred.
//
//  Arguments:
//
//    none
//
//  Return Value:
//
//    none
//
IDirect3DMobileTest::IDirect3DMobileTest()
{
	m_bInitSuccess = FALSE;
}


//
// IDirect3DMobileTest
//
//   Initialize IDirect3DMobileTest object
//
// Arguments:
//
//   UINT uiAdapterOrdinal: D3DM Adapter ordinal
//   WCHAR *pSoftwareDevice: Pointer to wide character string identifying a D3DM software device to instantiate
//   UINT uiTestCase:  Test case ordinal (to be displayed in window)
//
// Return Value
//
//   HRESULT indicates success or failure
//
HRESULT IDirect3DMobileTest::Init(LPTEST_CASE_ARGS pTestCaseArgs, LPWINDOW_ARGS pWindowArgs, BOOL bDebug, UINT uiTestCase)
{
	if (FAILED(D3DMInitializer::Init(D3DQA_PURPOSE_RAW_TEST,                      // D3DQA_PURPOSE Purpose
	                                 pWindowArgs,                                 // LPWINDOW_ARGS pWindowArgs
	                                 pTestCaseArgs->pwchSoftwareDeviceFilename,   // TCHAR *ptchDriver
	                                 uiTestCase)))                                // UINT uiTestCase
	{
		DebugOut(_T("Aborting initialization, due to prior initialization failure.\n"));
		return E_FAIL;
	}

	m_bDebugMiddleware = bDebug;

	m_bInitSuccess = TRUE;
	return S_OK;
}

//
//  IsReady
//
//    Accessor method for "initialization state" of IDirect3DMobileTest object.  "Half
//    initialized" states should not occur.
//  
//  Return Value:
//  
//    BOOL:  TRUE if the object is initialized; FALSE if it is not.
//
BOOL IDirect3DMobileTest::IsReady()
{
	if (FALSE == m_bInitSuccess)
	{
		DebugOut(_T("IDirect3DMobileTest is not ready.\n"));
		return FALSE;
	}

	DebugOut(_T("IDirect3DMobileTest is ready.\n"));
	return D3DMInitializer::IsReady();
}


// 
// ~IDirect3DMobileTest
//
//  Destructor for IDirect3DMobileTest.  Currently; there is nothing
//  to do here.
//
IDirect3DMobileTest::~IDirect3DMobileTest()
{
	return; // Success
}



//
// ExecuteQueryInterfaceTest
//
//   Verify IDirect3DMobile::QueryInterface
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteQueryInterfaceTest()
{
	DebugOut(_T("Beginning ExecuteQueryInterfaceTest.\n"));

	//
	// Interface pointer to retrieve from QueryInterface
	//
	IDirect3DMobile *pQID3D;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}


	// 	
	// Bad parameter tests
	// 	

	//
	// Test case #1:  Invalid REFIID
	//
	if (SUCCEEDED(m_pD3D->QueryInterface(IID_D3DQAInvalidInterface, // REFIID riid
	                                     (void**)(&pQID3D))))       // void** ppvObj
	{
		DebugOut(_T("QueryInterface succeeded; failure expected.\n"));
		return TPR_FAIL;
	}

	//
	// Test case #2:  Valid REFIID, invalid PPVOID
	//
	if (SUCCEEDED(m_pD3D->QueryInterface(IID_IDirect3DMobile, // REFIID riid
	                                     (void**)0)))         // void** ppvObj
	{
		DebugOut(_T("QueryInterface succeeded; failure expected.\n"));
		return TPR_FAIL;
	}


	//
	// Valid parameter tests
	//

	//
	// Retrieve IID_IDirect3DMobile
	//
	if (FAILED(m_pD3D->QueryInterface(IID_IDirect3DMobile, // REFIID riid
	                                  (void**)(&pQID3D)))) // void** ppvObj
	{
		DebugOut(_T("IDirect3DMobile::QueryInterface failed for IID_IDirect3DMobile.\n"));
		return TPR_FAIL;
	}
	else
	{
		if (0 == pQID3D->Release())
		{
			DebugOut(_T("IDirect3DMobile reference count dropped to zero unexpectedly.\n"));
			return TPR_FAIL;
		}
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}


//
// ExecuteAddRefTest
//
//   Verify IDirect3DMobile::AddRef
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteAddRefTest()
{
	DebugOut(_T("Beginning ExecuteAddRefTest.\n"));

	ULONG ulRefs1, ulRefs2;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Returns the value of the new reference count
	//
	ulRefs1 = m_pD3D->AddRef();

	//
	// Returns the value of the new reference count
	//
	ulRefs2 = m_pD3D->Release();

	//
	// Verify that the reference ops incremented/decremented as expected
	//
	if ((ulRefs1 - 1) != ulRefs2)
	{
		DebugOut(_T("IDirect3DMobile::AddRef / IDirect3DMobile::Release test failure.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}


//
// ExecuteReleaseTest
//
//   Verify IDirect3DMobile::Release
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteReleaseTest()
{
	DebugOut(_T("Beginning ExecuteReleaseTest.\n"));

	//
	// IDirect3DMobile::Release testing is covered in a closely-related test
	//
	return ExecuteAddRefTest();
}

//
// ExecuteRegisterSoftwareDeviceTest
//
//   Verify IDirect3DMobile::RegisterSoftwareDevice
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteRegisterSoftwareDeviceTest()
{
	DebugOut(_T("Beginning ExecuteRegisterSoftwareDeviceTest.\n"));

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// If the current device is a registered software device, release it as a prerequisite to
	// this test.
	// 
	if (D3DMADAPTER_REGISTERED_DEVICE == m_uiAdapterOrdinal)
	{
		if (0 != m_pd3dDevice->Release())
		{
			DebugOut(_T("IDirect3DMobileDevice::Release returned non-zero reference count; fatal error.\n"));
			m_pd3dDevice = NULL;
			return TPR_FAIL;
		}
		m_pd3dDevice = NULL;
	}

	//
	// Verify that RegisterSoftwareDevice can be used to clear out the current registration
	//
	if (FAILED(m_pD3D->RegisterSoftwareDevice(NULL)))
	{
		DebugOut(_T("Fatal error at IDirect3DMobile::RegisterSoftwareDevice.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteGetAdapterCountTest
//
//   Verify IDirect3DMobile::GetAdapterCount
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
#pragma prefast(disable: 262, "temporarily ignore stack size guideline\n") 
INT IDirect3DMobileTest::ExecuteGetAdapterCountTest()
{
	DebugOut(_T("Beginning ExecuteGetAdapterCountTest.\n"));

	//
	// Adapter identifiers
	//
	D3DMADAPTER_IDENTIFIER AdaptDefault, AdaptSoftware;

	//
	// API result status codes
	//
	HRESULT hr1, hr2;

	//
	// Number of adapters expected from GetAdapterCount
	//
	UINT uiNumExpectedAdapters = 0;

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Retrieve adapter info for default device
	//
	hr1 = m_pD3D->GetAdapterIdentifier(D3DMADAPTER_DEFAULT,// UINT Adapter
	                                   0,                  // ULONG Flags
	                                  &AdaptDefault);      // D3DMADAPTER_IDENTIFIER* pIdentifier
	
	//
	// Retrieve adapter info for software device
	//
	hr2 = m_pD3D->GetAdapterIdentifier(D3DMADAPTER_REGISTERED_DEVICE,// UINT Adapter
	                                   0,                            // ULONG Flags
	                                  &AdaptSoftware);               // D3DMADAPTER_IDENTIFIER* pIdentifier

	if (SUCCEEDED(hr1))
		uiNumExpectedAdapters++;

	if (SUCCEEDED(hr2))
		uiNumExpectedAdapters++;

	uiActualAdapters = m_pD3D->GetAdapterCount();

	if (uiActualAdapters != uiNumExpectedAdapters)
	{
		DebugOut(_T("Adapter count mismatch.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}
#pragma prefast(pop)


//
// ExecuteGetAdapterIdentifierTest
//
//   Verify IDirect3DMobile::GetAdapterIdentifier
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
#pragma prefast(disable: 262, "temporarily ignore stack size guideline\n") 
INT IDirect3DMobileTest::ExecuteGetAdapterIdentifierTest()
{
	DebugOut(_T("Beginning ExecuteGetAdapterIdentifierTest.\n"));

	//
	// Adapter identifiers
	//
	D3DMADAPTER_IDENTIFIER AdaptDefault, AdaptSoftware, AdaptInvalid;

	//
	// API result status codes
	//
	HRESULT hr1, hr2;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Retrieve adapter info for default device
	//
	hr1 = m_pD3D->GetAdapterIdentifier(D3DMADAPTER_DEFAULT,// UINT Adapter
	                                   0,                  // ULONG Flags
	                                  &AdaptDefault);      // D3DMADAPTER_IDENTIFIER* pIdentifier
	
	//
	// Retrieve adapter info for software device
	//
	hr2 = m_pD3D->GetAdapterIdentifier(D3DMADAPTER_REGISTERED_DEVICE,// UINT Adapter
	                                   0,                            // ULONG Flags
	                                  &AdaptSoftware);               // D3DMADAPTER_IDENTIFIER* pIdentifier
	
	//
	// Verify that query succeeded for required device
	//
	if (FAILED(hr1))
	{
		DebugOut(_T("Fatal error at GetAdapterIdentifier.\n"));
		return TPR_FAIL;
	}

	//
	// Retrieve adapter info for an invalid device
	//
	if (SUCCEEDED(m_pD3D->GetAdapterIdentifier(3,                    // UINT Adapter
	                                           0,                    // ULONG Flags
	                                           &AdaptInvalid    )))  // D3DMADAPTER_IDENTIFIER* pIdentifier
	{
		DebugOut(_T("Fatal error at GetAdapterIdentifier.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}
#pragma prefast(pop)

//
// ExecuteGetAdapterModeCountTest
//
//   Verify IDirect3DMobile::GetAdapterModeCount
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteGetAdapterModeCountTest()
{
	DebugOut(_T("Beginning ExecuteGetAdapterModeCountTest.\n"));

	//
	// Count of modes
	//
	UINT uiModeCount;

	//
	// Iterator for individual modes
	//
	UINT uiModeIter;
	
	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// Queried mode
	//
	D3DMDISPLAYMODE CurrentMode;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex <= D3DMADAPTER_REGISTERED_DEVICE; uiAdapterIndex++)
	{
		uiModeCount = m_pD3D->GetAdapterModeCount(uiAdapterIndex); // UINT Adapter

		for (uiModeIter = 0; uiModeIter < uiModeCount; uiModeIter++)
		{

			if (FAILED(m_pD3D->EnumAdapterModes(uiAdapterIndex, // UINT Adapter,
			                                    uiModeIter,     // UINT Mode,
			                                   &CurrentMode)))  // D3DDISPLAYMODE* pMode
			{
				DebugOut(_T("EnumAdapterModes failed.\n"));
				return TPR_FAIL;
			}
		}
	}

	//
	// Bad parameter test #1:  Invalid adapter index
	//
	if (SUCCEEDED(m_pD3D->EnumAdapterModes(0xFFFFFFFF,     // UINT Adapter,
	                                       uiModeIter,     // UINT Mode,
	                                      &CurrentMode)))  // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("EnumAdapterModes succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #2:  Invalid mode index
	//
	if (SUCCEEDED(m_pD3D->EnumAdapterModes(D3DMADAPTER_DEFAULT, // UINT Adapter,
	                                       0xFFFFFFFF,          // UINT Mode,
	                                      &CurrentMode)))       // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("EnumAdapterModes succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #3:  Invalid adapter index
	//
	if (0 != m_pD3D->GetAdapterModeCount(0xFFFFFFFF)) // UINT Adapter
	{
		DebugOut(_T("GetAdapterModeCount succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Be sure to cover code paths for both valid adapter indices, regardless of
	// existence of such an adapter
	//
	m_pD3D->EnumAdapterModes(D3DMADAPTER_DEFAULT,     // UINT Adapter,
	                         0,                       // UINT Mode,
	                         &CurrentMode);           // D3DDISPLAYMODE* pMode

	m_pD3D->EnumAdapterModes(D3DMADAPTER_REGISTERED_DEVICE, // UINT Adapter,
	                         0,                             // UINT Mode,
	                         &CurrentMode);                 // D3DDISPLAYMODE* pMode
	
	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteEnumAdapterModesTest
//
//   Verify IDirect3DMobile::EnumAdapterModes
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteEnumAdapterModesTest()
{
	DebugOut(_T("Beginning ExecuteEnumAdapterModesTest.\n"));

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Bad parameter tests
	//

	//
	// Test #1:  NULL D3DDISPLAYMODE pointer
	//
	if (SUCCEEDED(m_pD3D->EnumAdapterModes(D3DMADAPTER_DEFAULT,// UINT Adapter,
	                                       0,                  // UINT Mode,
	                                       NULL)))             // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("EnumAdapterModes succeeded; failure expected.\n"));
		return TPR_FAIL;
	}

	//
	// More thorough testing occurs in a closely-related test:
	//
	return ExecuteGetAdapterModeCountTest();
}

//
// ExecuteGetAdapterDisplayModeTest
//
//   Verify IDirect3DMobile::GetAdapterDisplayMode
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteGetAdapterDisplayModeTest()
{
	DebugOut(_T("Beginning ExecuteGetAdapterDisplayModeTest.\n"));

	//
	// Count of modes
	//
	UINT uiModeCount;

	//
	// Iterator for individual modes
	//
	UINT uiModeIter;
	
	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;

	//
	// Queried mode
	//
	D3DMDISPLAYMODE CurrentMode;
	D3DMDISPLAYMODE AdapterDisplay;

	//
	// Indicates whether or not a match has been found for the mode
	// reported by GetAdapterDisplayMode, in EnumAdapterModes
	// 
	BOOL bFoundMatch = FALSE;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	uiActualAdapters = m_pD3D->GetAdapterCount();

	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex < uiActualAdapters; uiAdapterIndex++)
	{

		if (FAILED(m_pD3D->GetAdapterDisplayMode(uiAdapterIndex,  // UINT Adapter
		                                         &AdapterDisplay)))// D3DDISPLAYMODE* pMode
		{
			DebugOut(_T("GetAdapterDisplayMode failed.\n"));
			return TPR_FAIL;
		}

		uiModeCount = m_pD3D->GetAdapterModeCount(uiAdapterIndex); // UINT Adapter

		for (uiModeIter = 0; uiModeIter < uiModeCount; uiModeIter++)
		{
			if (FAILED(m_pD3D->EnumAdapterModes(uiAdapterIndex, // UINT Adapter,
			                                    uiModeIter,     // UINT Mode,
			                                   &CurrentMode)))  // D3DMDISPLAYMODE* pMode
			{
				DebugOut(_T("EnumAdapterModes failed.\n"));
				return TPR_FAIL;
			}

			if (0 == memcmp(&CurrentMode, &AdapterDisplay, sizeof(D3DMDISPLAYMODE)))
				bFoundMatch = TRUE;
		}

		if (FALSE == bFoundMatch)
		{
			//
			// If no match was found, for this adapter, fail the test
			//
			return TPR_FAIL;
		}
		else
		{
			//
			// Proceed with the next adapter; reset the "found match" BOOL
			//
			bFoundMatch = FALSE;
		}
	}

	//
	// Bad parameter test #1:  Invalid adapter index
	//
	if (SUCCEEDED(m_pD3D->GetAdapterDisplayMode(0xFFFFFFFF,           // UINT Adapter
	                                            &AdapterDisplay)))    // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("GetAdapterDisplayMode succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #2:  Invalid D3DDISPLAYMODE*
	//
	if (SUCCEEDED(m_pD3D->GetAdapterDisplayMode(D3DMADAPTER_DEFAULT,  // UINT Adapter
	                                            NULL)))               // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("GetAdapterDisplayMode succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}


	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCheckDeviceTypeTest
//
//   Verify IDirect3DMobile::CheckDeviceType
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckDeviceTypeTest()
{
	DebugOut(_T("Beginning ExecuteCheckDeviceTypeTest.\n"));

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	if (E_NOTIMPL != m_pD3D->CheckDeviceType(D3DMADAPTER_DEFAULT, // UINT Adapter,
	                                         D3DMDEVTYPE_DEFAULT, // D3DMDEVTYPE CheckType,
	                                         D3DMFMT_UNKNOWN,     // D3DMFORMAT DisplayFormat,
	                                         D3DMFMT_UNKNOWN,     // D3DMFORMAT BackBufferFormat,
	                                         FALSE))              // BOOL Windowed
	{
		DebugOut(_T("Unexpected result for CheckDeviceType.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCheckDeviceFormatTest
//
//   Verify IDirect3DMobile::CheckDeviceFormat
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckDeviceFormatTest()
{
	DebugOut(_T("Beginning ExecuteCheckDeviceFormatTest.\n"));

	//
	// Iterators for formats, resource types
	//
	D3DMFORMAT FormatIter;
	UINT ResTypeIter;
	D3DMRESOURCETYPE ResType;
	CONST UINT uiNumResTypes = 4;

	//
	// API Results
	//
	HRESULT hr;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	for (FormatIter = (D3DMFORMAT)0; FormatIter < D3DMFMT_NUMFORMAT; (*(ULONG*)(&FormatIter))++)
	{
		for (ResTypeIter = 0; ResTypeIter < uiNumResTypes; ResTypeIter++)
		{
			switch(ResTypeIter)
			{
			case 0:
				ResType = D3DMRTYPE_SURFACE;
				break;
			case 1:
				ResType = D3DMRTYPE_TEXTURE;
				break;
			case 2:
				ResType = D3DMRTYPE_VERTEXBUFFER;
				break;
			case 3:
				ResType = D3DMRTYPE_INDEXBUFFER;
				break;
			}

			hr = m_pD3D->CheckDeviceFormat(m_uiAdapterOrdinal, // UINT Adapter
			                               m_DevType,          // D3DMDEVTYPE DeviceType
			                               FormatIter,         // D3DMFORMAT AdapterFormat
			                               0,                  // ULONG Usage
			                               ResType,            // D3DMRESOURCETYPE RType
			                               FormatIter);        // D3DMFORMAT CheckFormat

			
			//
			// If CheckDeviceFormat reported support; make sure that the inputs were sensible.
			// If not, there must be an error.
			//
			if (SUCCEEDED(hr))
			{
				//
				// Verify that CheckDeviceFormat output, for D3DMRTYPE_SURFACE, is possible
				//
				if (D3DMRTYPE_SURFACE == ResType)
				{
					if (!((FormatIter > D3DMFMT_UNKNOWN) && (FormatIter < D3DMFMT_INDEX16)))
					{
						DebugOut(_T("CheckDeviceFormat succeeded; failure expected.\n"));
						return TPR_FAIL;
					}
				}

				//
				// Verify that CheckDeviceFormat output, for D3DMRTYPE_TEXTURE, is possible
				//
				if (D3DMRTYPE_TEXTURE == ResType)
				{
					if (!((FormatIter > D3DMFMT_UNKNOWN) && (FormatIter < D3DMFMT_INDEX16)))
					{
						DebugOut(_T("CheckDeviceFormat succeeded; failure expected.\n"));
						return TPR_FAIL;
					}
				}

				//
				// If middleware is debug, it screen for disallowed restypes
				//
				if (m_bDebugMiddleware)
				{
					//
					// Verify that CheckDeviceFormat output, for D3DMRTYPE_VERTEXBUFFER, is possible
					//
					if (D3DMRTYPE_VERTEXBUFFER == ResType)
					{
						//
						// CheckDeviceFormat is not for use with the vertex buffer resource type
						//
						DebugOut(_T("CheckDeviceFormat succeeded; failure expected.\n"));
						return TPR_FAIL;
					}

					//
					// Verify that CheckDeviceFormat output, for D3DMRTYPE_INDEXBUFFER, is possible
					//
					if (D3DMRTYPE_INDEXBUFFER == ResType)
					{
						//
						// CheckDeviceFormat is not for use with the index buffer resource type
						//
						DebugOut(_T("CheckDeviceFormat succeeded; failure expected.\n"));
						return TPR_FAIL;
					}
				}
			}
		}
	}
		
	//
	// Bad parameter test #1:  Invalid adapter index
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(0xFFFFFFFF,         // UINT Adapter
	                                        m_DevType,          // D3DMDEVTYPE DeviceType
	                                        D3DMFMT_R8G8B8,     // D3DMFORMAT AdapterFormat
	                                        0,                  // ULONG Usage
	                                        D3DMRTYPE_TEXTURE,  // D3DMRESOURCETYPE RType
	                                        D3DMFMT_R8G8B8)))   // D3DMFORMAT CheckFormat
	{
		DebugOut(_T("CheckDeviceFormat succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #2:  Invalid device type index
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(m_uiAdapterOrdinal,      // UINT Adapter
	                                        (D3DMDEVTYPE)0xFFFFFFFF, // D3DMDEVTYPE DeviceType
	                                        D3DMFMT_R8G8B8,          // D3DMFORMAT AdapterFormat
	                                        0,                       // ULONG Usage
	                                        D3DMRTYPE_TEXTURE,       // D3DMRESOURCETYPE RType
	                                        D3DMFMT_R8G8B8)))        // D3DMFORMAT CheckFormat
	{
		DebugOut(_T("CheckDeviceFormat succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #3:  Invalid usage
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(m_uiAdapterOrdinal, // UINT Adapter
	                                        m_DevType,          // D3DMDEVTYPE DeviceType
	                                        D3DMFMT_R8G8B8,     // D3DMFORMAT AdapterFormat
	                                        0xFFFFFFFF,         // ULONG Usage
	                                        D3DMRTYPE_TEXTURE,  // D3DMRESOURCETYPE RType
	                                        D3DMFMT_R8G8B8)))   // D3DMFORMAT CheckFormat
	{
		DebugOut(_T("CheckDeviceFormat succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}


	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCheckDeviceMultiSampleTypeTest
//
//   Verify IDirect3DMobile::CheckDeviceMultiSampleType
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckDeviceMultiSampleTypeTest()
{
	DebugOut(_T("Beginning ExecuteCheckDeviceMultiSampleTypeTest.\n"));
	
	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// API result status codes
	//
	HRESULT hr1, hr2;

	//
	// Iterator for multisample types
	//
	D3DMMULTISAMPLE_TYPE MultiSampleIter;

	//
	// Display mode, used to determine a format to use
	//
	D3DMDISPLAYMODE Mode;

	//
	// Presentation parameters for device resets
	//
	D3DMPRESENT_PARAMETERS PresentParms;

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Retrieve current display mode
	//
	if (FAILED(m_pd3dDevice->GetDisplayMode(&Mode)))// D3DMDISPLAYMODE* pMode
	{
		DebugOut(_T("Fatal error at GetDisplayMode.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #1: Invalid adapter
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(0xFFFFFFFF,            // UINT Adapter
	                                                 m_DevType,             // D3DMDEVTYPE DeviceType
	                                                 Mode.Format,           // D3DMFORMAT SurfaceFormat
	                                                 FALSE,                 // BOOL Windowed
	                                                 D3DMMULTISAMPLE_NONE)))// D3DMMULTISAMPLE_TYPE MultiSampleType
	{
		DebugOut(_T("CheckDeviceMultiSampleType succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #2: Invalid device ordinal
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_uiAdapterOrdinal,      // UINT Adapter
	                                                 (D3DMDEVTYPE)0xFFFFFFFF, // D3DMDEVTYPE DeviceType
	                                                 Mode.Format,             // D3DMFORMAT SurfaceFormat
	                                                 FALSE,                   // BOOL Windowed
	                                                 D3DMMULTISAMPLE_NONE)))  // D3DMMULTISAMPLE_TYPE MultiSampleType
	{
		DebugOut(_T("CheckDeviceMultiSampleType succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #3: Invalid format
	//
	if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_uiAdapterOrdinal,      // UINT Adapter
	                                                 m_DevType,               // D3DMDEVTYPE DeviceType
	                                                 D3DMFMT_UNKNOWN,         // D3DMFORMAT SurfaceFormat
	                                                 FALSE,                   // BOOL Windowed
	                                                 D3DMMULTISAMPLE_NONE)))  // D3DMMULTISAMPLE_TYPE MultiSampleType
	{
		DebugOut(_T("CheckDeviceMultiSampleType succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Clear out presentation parameters structure
	//
	ZeroMemory( &PresentParms, sizeof(PresentParms) );

	//
	// Prepare presentation intervals for use with IDirect3DMobileDevice::Reset
	//
	// Note: Multisampling is supported only if the swap effect is D3DMSWAPEFFECT_DISCARD. 
	//
	PresentParms.Windowed   = FALSE;
	PresentParms.SwapEffect = D3DMSWAPEFFECT_DISCARD;
	PresentParms.BackBufferFormat = Mode.Format;
	PresentParms.BackBufferCount = 1;
	PresentParms.BackBufferWidth = Mode.Width;
	PresentParms.BackBufferHeight = Mode.Height;
	PresentParms.FullScreen_PresentationInterval = D3DMPRESENT_INTERVAL_DEFAULT;

	//
	// For this test, we must create a device specifically in fullscreen mode.  Release & create
	// to be sure.
	//
	if (0 != m_pd3dDevice->Release())
	{
		DebugOut(_T("IDirect3DMobileDevice::Release returned non-zero reference count; fatal error.\n"));
		return TPR_FAIL;
	}

	//
	// In case of unexpected failure, ensure that device interface pointer is cleaned up
	//
	m_pd3dDevice = NULL;

	//
	// Create a device with D3DMSWAPEFFECT_DISCARD; other swap effects prevent rigorous testing
	// of IDirect3DMobile::CheckDeviceMultiSampleType with IDirect3DMobileDevice::Reset
	//
	if( FAILED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,// UINT Adapter,
	                                  m_DevType,         // D3DMDEVTYPE DeviceType,
	                                  m_hWnd,            // HWND hFocusWindow,
	                                  m_dwBehaviorFlags, // ULONG BehaviorFlags,
	                                 &PresentParms,      // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                 &m_pd3dDevice )))   // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("IDirect3DMobile::CreateDevice failed.\n"));
		return TPR_FAIL;
	}

	uiActualAdapters = m_pD3D->GetAdapterCount();

	//
	// Verify, for each valid adapter index, that D3DMMULTISAMPLE_NONE is reported as supported
	//
	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex < uiActualAdapters; uiAdapterIndex++)
	{

		if (FAILED(m_pD3D->CheckDeviceMultiSampleType(uiAdapterIndex,        // UINT Adapter
		                                              D3DMDEVTYPE_DEFAULT,   // D3DMDEVTYPE DeviceType
		                                              D3DMFMT_R8G8B8,        // UNUSED - D3DMFORMAT SurfaceFormat
		                                              FALSE,                 // UNUSED - BOOL Windowed
		                                              D3DMMULTISAMPLE_NONE)))// D3DMMULTISAMPLE_TYPE MultiSampleType
		{
			DebugOut(_T("CheckDeviceMultiSampleType failed with D3DMMULTISAMPLE_NONE; fatal error.\n"));
			return TPR_FAIL;
		}
	}

	//
	// For currently instantiated adapter index, verify consistency between:
	//
	//    * Result of IDirect3DMobile::CheckDeviceMultiSampleType
	//    * Result of IDirect3DMobileDevice::Reset with multisample type
	//
	for (MultiSampleIter = D3DMMULTISAMPLE_2_SAMPLES; MultiSampleIter < D3DMMULTISAMPLE_16_SAMPLES; (*(ULONG*)(&MultiSampleIter))++)
	{

		hr1 = m_pD3D->CheckDeviceMultiSampleType(m_uiAdapterOrdinal, // UINT Adapter
		                                         D3DMDEVTYPE_DEFAULT,// D3DMDEVTYPE DeviceType
		                                         D3DMFMT_R8G8B8,     // UNUSED - D3DMFORMAT SurfaceFormat
		                                         FALSE,              // UNUSED - BOOL Windowed
		                                         MultiSampleIter);   // D3DMMULTISAMPLE_TYPE MultiSampleType

		//
		// Attempt a device reset with the specified multisample type
		//
		PresentParms.MultiSampleType = MultiSampleIter;

		hr2 = m_pd3dDevice->Reset(&PresentParms);

		//
		// Analyze all four of the possible outcomes
		//
		if ((FAILED(hr1)) && (FAILED(hr2)))
		{
			DebugOut(_T("CheckDeviceMultiSampleType reported the multisample type as unsupported; IDirect3DMobileDevice::Reset failed as expected.\n"));
		}
		else if ((SUCCEEDED(hr1)) && (SUCCEEDED(hr2)))
		{
			DebugOut(_T("CheckDeviceMultiSampleType reported the multisample type as supported; IDirect3DMobileDevice::Reset succeeded as expected.\n"));
		}
		else if ((FAILED(hr1)) && (SUCCEEDED(hr2)))
		{
			DebugOut(_T("CheckDeviceMultiSampleType reported the multisample type as unsupported; but IDirect3DMobileDevice::Reset succeeded (unexpected).\n"));
			return TPR_FAIL;
		}
		else if ((SUCCEEDED(hr1)) && (FAILED(hr2)))
		{
			DebugOut(_T("CheckDeviceMultiSampleType reported the multisample type as supported; but IDirect3DMobileDevice::Reset failed (unexpected).\n"));
			return TPR_FAIL;
		}

	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCheckDepthStencilMatchTest
//
//   Verify IDirect3DMobile::CheckDepthStencilMatch
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckDepthStencilMatchTest()
{
	DebugOut(_T("Beginning ExecuteCheckDepthStencilMatchTest.\n"));

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;

	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// Display modes for adapters
	//
	D3DMDISPLAYMODE Mode1, Mode2;

	//
	// Iterators for surface formats
	//
	D3DMFORMAT FormatIterRT, FormatIterDS;

	//
	// API result status codes
	//
	HRESULT hr;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	uiActualAdapters = m_pD3D->GetAdapterCount();

	//
	// Retrieve current display mode for default adapter
	//
	if (FAILED(m_pD3D->GetAdapterDisplayMode( D3DMADAPTER_DEFAULT,// UINT Adapter
	                                         &Mode1)))            // D3DDISPLAYMODE* pMode
	{
		DebugOut(_T("Fatal error at GetDisplayMode.\n"));
		return TPR_FAIL;
	}

	//
	// Retrieve current display mode for registered device, if present
	//
	if (2 == uiActualAdapters)
	{
		if (FAILED(m_pD3D->GetAdapterDisplayMode( D3DMADAPTER_REGISTERED_DEVICE,// UINT Adapter
		                                         &Mode2)))                      // D3DDISPLAYMODE* pMode
		{
			DebugOut(_T("Fatal error at GetDisplayMode.\n"));
			return TPR_FAIL;
		}
	}

	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex < uiActualAdapters; uiAdapterIndex++)
	{
		for (FormatIterRT = (D3DMFORMAT)0; FormatIterRT < D3DMFMT_NUMFORMAT; (*(ULONG*)(&FormatIterRT))++)
		{
			for (FormatIterDS = (D3DMFORMAT)0; FormatIterDS < D3DMFMT_NUMFORMAT; (*(ULONG*)(&FormatIterDS))++)
			{

				hr = m_pD3D->CheckDepthStencilMatch( uiAdapterIndex,                              // UINT Adapter
				                                     D3DMDEVTYPE_DEFAULT,                         // D3DMDEVTYPE DeviceType
				                                     uiAdapterIndex ? Mode2.Format : Mode1.Format,// D3DMFORMAT AdapterFormat
				                                     FormatIterRT,                                // D3DMFORMAT RenderTargetFormat
				                                     FormatIterDS);                               // D3DMFORMAT DepthStencilFormat


				//
				// If CheckDepthStencilMatch reported support; make sure that the inputs were sensible.
				// If not, there must be an error.
				//
				if (SUCCEEDED(hr))
				{
					//
					// Verify that CheckDepthStencilMatch output is possible
					//
					if (!((FormatIterRT >= D3DMFMT_R8G8B8) && (FormatIterRT <= D3DMFMT_DXT5)))
					{
						DebugOut(_T("CheckDepthStencilMatch succeeded; failure expected.\n"));
						return TPR_FAIL;
					}
					if (!((FormatIterDS >= D3DMFMT_D32) && (FormatIterDS <= D3DMFMT_D24X4S4)))
					{
						DebugOut(_T("CheckDepthStencilMatch succeeded; failure expected.\n"));
						return TPR_FAIL;
					}
				}


				//
				// Bad parameter test #1: Invalid device type
				//
				if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch( uiAdapterIndex,                              // UINT Adapter
				                                              (D3DMDEVTYPE)0xFFFFFFFF,                     // D3DMDEVTYPE DeviceType
				                                              uiAdapterIndex ? Mode2.Format : Mode1.Format,// D3DMFORMAT AdapterFormat
				                                              FormatIterRT,                                // D3DMFORMAT RenderTargetFormat
				                                              FormatIterDS)))                              // D3DMFORMAT DepthStencilFormat
				{
					DebugOut(_T("CheckDepthStencilMatch succeeded unexpectedly.\n"));
					return TPR_FAIL;
				}

				//
				// Bad parameter test #2: Invalid adapter index
				//
				if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch( 0xFFFFFFFF,                                  // UINT Adapter
				                                              D3DMDEVTYPE_DEFAULT,                         // D3DMDEVTYPE DeviceType
				                                              uiAdapterIndex ? Mode2.Format : Mode1.Format,// D3DMFORMAT AdapterFormat
				                                              FormatIterRT,                                // D3DMFORMAT RenderTargetFormat
				                                              FormatIterDS)))                              // D3DMFORMAT DepthStencilFormat
				{
					DebugOut(_T("CheckDepthStencilMatch succeeded unexpectedly.\n"));
					return TPR_FAIL;
				}

			}

		}
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCheckProfileTest
//
//   Verify IDirect3DMobile::CheckProfile
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckProfileTest()
{
	DebugOut(_T("Beginning ExecuteCheckProfileTest.\n"));

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;
	
	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// Device capabilities
	//
	D3DMCAPS Caps;

	//
	// Test result for profile conformance
	//
	INT BaseProfileTestResult;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	uiActualAdapters = m_pD3D->GetAdapterCount();

	//
	// Verify that the base profile is supported on all valid adapters; and that a non-existent
	// profile is reported as not supported.
	//
	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex < uiActualAdapters; uiAdapterIndex++)
	{

		if (FAILED(m_pD3D->GetDeviceCaps( uiAdapterIndex,      // UINT Adapter,
										  D3DMDEVTYPE_DEFAULT, // D3DMDEVTYPE DeviceType
										 &Caps)))              // D3DMCAPS* pCaps
		{
			DebugOut(_T("GetDeviceCaps failed.\n"));
			return TPR_FAIL;
		}

		//
		// Confirm that device adheres to D3DMPROFILE_BASE requirements.
		//
		// Return value is a TPR_* code, not an HRESULT
		//
		BaseProfileTestResult = ExecuteConformsToBaseProfileTest(Caps);

		//
		// If non-passing, return
		//
		if (TPR_PASS != BaseProfileTestResult)
			return BaseProfileTestResult;

		if (FAILED(m_pD3D->CheckProfile(uiAdapterIndex,       // UINT Adapter
		                                D3DMDEVTYPE_DEFAULT, // D3DMDEVTYPE DeviceType
		                                D3DMPROFILE_BASE)))  // D3DMPROFILE Profile
		{
			DebugOut(_T("Device does not support base profile.  Fatal error.\n"));
			return TPR_FAIL;
		}

		//
		// Bad parameter test #1:  Invalid profile identifier
		//
		if (SUCCEEDED(m_pD3D->CheckProfile( uiAdapterIndex,           // UINT Adapter
		                                    D3DMDEVTYPE_DEFAULT,      // D3DMDEVTYPE DeviceType
		                                   (D3DMPROFILE)0xFFFFFFFF))) // D3DMPROFILE Profile
		{
			DebugOut(_T("Device claims to support non-existent profile.  Fatal error.\n"));
			return TPR_FAIL;
		}
	}


	//
	// Bad parameter test #2:  Invalid adapter index
	//
	if (SUCCEEDED(m_pD3D->CheckProfile(0xFFFFFFFF,               // UINT Adapter
	                                   D3DMDEVTYPE_DEFAULT,      // D3DMDEVTYPE DeviceType
	                                   D3DMPROFILE_BASE)))       // D3DMPROFILE Profile
	{
		DebugOut(_T("CheckProfile succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #3:  Invalid device type index
	//
	if (SUCCEEDED(m_pD3D->CheckProfile( D3DMADAPTER_DEFAULT,     // UINT Adapter
	                                   (D3DMDEVTYPE)0xFFFFFFFF,  // D3DMDEVTYPE DeviceType
	                                    D3DMPROFILE_BASE)))      // D3DMPROFILE Profile
	{
		DebugOut(_T("CheckProfile succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteGetDeviceCapsTest
//
//   Verify IDirect3DMobile::GetDeviceCaps
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteGetDeviceCapsTest()
{
	DebugOut(_T("Beginning ExecuteGetDeviceCapsTest.\n"));

	//
	// Device capabilities
	//
	D3DMCAPS Caps1, Caps2;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	if (FAILED(m_pD3D->GetDeviceCaps( m_uiAdapterOrdinal,  // UINT Adapter,
	                                  D3DMDEVTYPE_DEFAULT, // D3DMDEVTYPE DeviceType
	                                 &Caps1)))             // D3DMCAPS* pCaps
	{
		DebugOut(_T("GetDeviceCaps failed.\n"));
		return TPR_FAIL;
	}

	if (FAILED(m_pd3dDevice->GetDeviceCaps(&Caps2)))  // D3DMCAPS* pCaps
	{
		DebugOut(_T("GetDeviceCaps failed.\n"));
		return TPR_FAIL;
	}

	if (0 != memcmp(&Caps1, &Caps2, sizeof(D3DMCAPS)))
	{
		DebugOut(_T("D3DMCAPS mismatch.\n"));
		return TPR_FAIL;
	}

	if (D3DMDEVTYPE_DEFAULT != Caps1.DeviceType)
	{
		DebugOut(_T("Invalid data: D3DMCAPS.DeviceType.\n"));
		return TPR_FAIL;
	}

	if (m_uiAdapterOrdinal != Caps1.AdapterOrdinal)
	{
		DebugOut(_T("Invalid data: D3DMCAPS.AdapterOrdinal.\n"));
		return TPR_FAIL;
	}


	//
	// Bad parameter test #1:  Invalid adapter index
	//
	if (SUCCEEDED(m_pD3D->GetDeviceCaps(0xFFFFFFFF,          // UINT Adapter,
	                                    D3DMDEVTYPE_DEFAULT, // D3DMDEVTYPE DeviceType
	                                    &Caps1)))            // D3DMCAPS* pCaps
	{
		DebugOut(_T("GetDeviceCaps succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Bad parameter test #2:  Invalid device type index
	//
	if (SUCCEEDED(m_pD3D->GetDeviceCaps(m_uiAdapterOrdinal,      // UINT Adapter,
	                                    (D3DMDEVTYPE)0xFFFFFFFF, // D3DMDEVTYPE DeviceType
	                                    &Caps1)))                // D3DMCAPS* pCaps
	{
		DebugOut(_T("GetDeviceCaps succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}

//
// ExecuteCreateDeviceTest
//
//   Verify IDirect3DMobile::CreateDevice
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCreateDeviceTest()
{
	DebugOut(_T("Beginning ExecuteCreateDeviceTest.\n"));

	//
	// Display mode, used to determine a format to use
	//
	D3DMDISPLAYMODE Mode;

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		return TPR_SKIP;
	}

	//
	// Retrieve current display mode
	//
	if (FAILED(m_pd3dDevice->GetDisplayMode(&Mode)))// D3DMDISPLAYMODE* pMode
	{
		DebugOut(_T("Fatal error at GetDisplayMode.\n"));
		return TPR_FAIL;
	}

	//
	// Release existing device to prepare for CreateDevice testing
	//
	if (0 != m_pd3dDevice->Release())
	{
		DebugOut(_T("IDirect3DMobileDevice::Release returned non-zero reference count; fatal error.\n"));
		return TPR_FAIL;
	}
	m_pd3dDevice = NULL;

	//
	// Bad parameter tests
	//

	//
	// Test #1:  Invalid IDirect3DMobileDevice**
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,// UINT Adapter,
	                                     m_DevType,         // D3DMDEVTYPE DeviceType,
	                                     m_hWnd,            // HWND hFocusWindow,
	                                     m_dwBehaviorFlags, // ULONG BehaviorFlags,
	                                    &m_d3dpp,           // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                     NULL )))           // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Test #2:  Invalid D3DMPRESENT_PARAMETERS*
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,// UINT Adapter,
	                                     m_DevType,         // D3DMDEVTYPE DeviceType,
	                                     m_hWnd,            // HWND hFocusWindow,
	                                     m_dwBehaviorFlags, // ULONG BehaviorFlags,
	                                     NULL,              // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                    &m_pd3dDevice)))    // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}


	//
	// Test #3:  Invalid HWND
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,// UINT Adapter,
	                                     m_DevType,         // D3DMDEVTYPE DeviceType,
	                                     (HWND)0xFFFFFFFF,  // HWND hFocusWindow,
	                                     m_dwBehaviorFlags, // ULONG BehaviorFlags,
	                                    &m_d3dpp,           // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                    &m_pd3dDevice)))    // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Test #4:  Invalid adapter ordinal
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( 0xFFFFFFFF,        // UINT Adapter,
	                                     m_DevType,         // D3DMDEVTYPE DeviceType,
	                                     m_hWnd,            // HWND hFocusWindow,
	                                     m_dwBehaviorFlags, // ULONG BehaviorFlags,
	                                    &m_d3dpp,           // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                    &m_pd3dDevice)))    // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Test #5:  Invalid behavior flags
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,// UINT Adapter,
	                                     m_DevType,         // D3DMDEVTYPE DeviceType,
	                                     m_hWnd,            // HWND hFocusWindow,
	                                     0xFFFFFFFF,        // ULONG BehaviorFlags,
	                                    &m_d3dpp,           // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                    &m_pd3dDevice)))    // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// Test #6:  Invalid D3DMDEVTYPE
	//
	if( SUCCEEDED( m_pD3D->CreateDevice( m_uiAdapterOrdinal,      // UINT Adapter,
	                                     (D3DMDEVTYPE)0xFFFFFFFF, // D3DMDEVTYPE DeviceType,
	                                     m_hWnd,                  // HWND hFocusWindow,
	                                     m_dwBehaviorFlags,       // ULONG BehaviorFlags,
	                                    &m_d3dpp,                 // D3DMPRESENT_PARAMETERS* pPresentationParameters,
	                                    &m_pd3dDevice)))          // IDirect3DMobileDevice** ppReturnedDeviceInterface
	{
		DebugOut(_T("CreateDevice succeeded unexpectedly.\n"));
		return TPR_FAIL;
	}

	//
	// All failure conditions have already returned.
	//
	return TPR_PASS;
}



//
// ExecuteCheckDeviceFormatConversionTest
//
//   Verify IDirect3DMobile::CheckDeviceFormatConversion
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteCheckDeviceFormatConversionTest()
{
	DebugOut(_T("Beginning ExecuteCheckDeviceFormatConversionTest.\n"));

	//
	// Actual number of adapters reported by GetAdapterCount
	//
	UINT uiActualAdapters;

	//
	// Iterator for adapter indices
	//
	UINT uiAdapterIndex;

	//
	// Tux Test Result
	//
	INT Result = TPR_PASS;

	//
	// Source Format and Destination Format for CheckDeviceFormatConversion
	//
	D3DMFORMAT SourceFormat, DestFormat;

	//
	// Adapter display mode
	//
	D3DMDISPLAYMODE DisplayMode;

	//
	// Data points for test to cross-reference
	//
	BOOL bConversionSupported, bSourceSupported, bDestSupported;

	//
	// Special source format cases to track for debug output
	//
	BOOL bSrcFailReasonChkDevFmt;     
	BOOL bSrcFailReasonFmtIndex;      
	BOOL bSrcFailReasonFmtFixedFloat; 
	BOOL bSrcFailReasonFmtVertexData; 
	BOOL bSrcFailReasonFmtUnknown;    
	BOOL bSrcFailReasonFmtInvalid;    

	//
	// Special dest format cases to track for debug output
	//
	BOOL bDestFailReasonYUV;          
	BOOL bDestFailReasonChkDevFmt;    
	BOOL bDestFailReasonFmtIndex;     
	BOOL bDestFailReasonFmtFixedFloat;
	BOOL bDestFailReasonFmtVertexData;
	BOOL bDestFailReasonFmtUnknown;   
	BOOL bDestFailReasonFmtInvalid;   

	if (!IsReady())
	{
		DebugOut(_T("Due to failed initialization, the test must be aborted.\n"));
		Result = TPR_SKIP;
		goto cleanup;
	}

	uiActualAdapters = m_pD3D->GetAdapterCount();

	for (uiAdapterIndex = D3DMADAPTER_DEFAULT; uiAdapterIndex < uiActualAdapters; uiAdapterIndex++)
	{

		DebugOut(_T(""));
		DebugOut(_T("======================================================================================"));
		DebugOut(_T("Now beginning CheckDeviceFormatConversion tests for adapter index:  %s"),
			(D3DMADAPTER_DEFAULT == uiAdapterIndex) ? _T("D3DMADAPTER_DEFAULT") : _T("D3DMADAPTER_REGISTERED_DEVICE"));
		DebugOut(_T("======================================================================================"));
		DebugOut(_T(""));

		if (FAILED(m_pD3D->GetAdapterDisplayMode(uiAdapterIndex, // UINT Adapter
		                                         &DisplayMode))) // D3DMDISPLAYMODE* pMode
		{
			DebugOut(_T("GetAdapterDisplayMode failed.\n"));
			Result = TPR_ABORT;
			goto cleanup;
		}

		for (SourceFormat = (D3DMFORMAT)0; SourceFormat < D3DMFMT_NUMFORMAT; (*(ULONG*)(&SourceFormat))++)
		{
			for (DestFormat = (D3DMFORMAT)0; DestFormat < D3DMFMT_NUMFORMAT; (*(ULONG*)(&DestFormat))++)
			{
				bSrcFailReasonChkDevFmt       = FALSE;
				bSrcFailReasonFmtIndex        = FALSE;
				bSrcFailReasonFmtFixedFloat   = FALSE;
				bSrcFailReasonFmtVertexData   = FALSE;
				bSrcFailReasonFmtUnknown      = FALSE;
				bSrcFailReasonFmtInvalid      = FALSE;

				bDestFailReasonYUV            = FALSE;
				bDestFailReasonChkDevFmt      = FALSE;
				bDestFailReasonFmtIndex       = FALSE;
				bDestFailReasonFmtFixedFloat  = FALSE;
				bDestFailReasonFmtVertexData  = FALSE;
				bDestFailReasonFmtUnknown     = FALSE;
				bDestFailReasonFmtInvalid     = FALSE;

				//
				// Can the device perform the format conversion?
				//
				if (FAILED(m_pD3D->CheckDeviceFormatConversion( uiAdapterIndex,                              // UINT Adapter
				                                                D3DMDEVTYPE_DEFAULT,                         // D3DMDEVTYPE DeviceType
				                                                SourceFormat,                                // D3DMFORMAT SourceFormat
				                                                DestFormat)))                                // D3DMFORMAT DestFormat
				{
					bConversionSupported = FALSE;
				}
				else
				{
					bConversionSupported = TRUE;
				}

				//
				// Does the device support creating the proposed source at all?
				//
				switch(SourceFormat)
				{
				case D3DMFMT_UNKNOWN:
					bSrcFailReasonFmtUnknown = TRUE;
					break;

				case D3DMFMT_R8G8B8:  
				case D3DMFMT_A8R8G8B8:
				case D3DMFMT_X8R8G8B8:
				case D3DMFMT_R5G6B5:  
				case D3DMFMT_X1R5G5B5:
				case D3DMFMT_A1R5G5B5:
				case D3DMFMT_A4R4G4B4:
				case D3DMFMT_R3G3B2:  
				case D3DMFMT_A8R3G3B2:
				case D3DMFMT_X4R4G4B4:
				case D3DMFMT_A8P8:    
				case D3DMFMT_P8:      
				case D3DMFMT_A8:      
				case D3DMFMT_UYVY:
				case D3DMFMT_YUY2:
				case D3DMFMT_DXT1:
				case D3DMFMT_DXT2:
				case D3DMFMT_DXT3:
				case D3DMFMT_DXT4:
				case D3DMFMT_DXT5:

					//
					// Does the device support the D3DMRTYPE_SURFACE format at all?
					//
					if (FAILED(m_pD3D->CheckDeviceFormat(uiAdapterIndex,         // UINT Adapter
				                                         D3DMDEVTYPE_DEFAULT,    // D3DMDEVTYPE DeviceType
				                                         DisplayMode.Format,     // D3DMFORMAT AdapterFormat
				                                         0,                      // ULONG Usage
				                                         D3DMRTYPE_SURFACE,      // D3DMRESOURCETYPE RType
				                                         SourceFormat)))         // D3DMFORMAT CheckFormat
					{
						bSrcFailReasonChkDevFmt = TRUE;
					}

					break;

				case D3DMFMT_D32:    
				case D3DMFMT_D15S1:  
				case D3DMFMT_D24S8:  
				case D3DMFMT_D16:    
				case D3DMFMT_D24X8:
				case D3DMFMT_D24X4S4:

					//
					// Does the device support the D3DMRTYPE_DEPTHSTENCIL format at all?
					//
					if (FAILED(m_pD3D->CheckDeviceFormat(uiAdapterIndex,         // UINT Adapter
				                                         D3DMDEVTYPE_DEFAULT,    // D3DMDEVTYPE DeviceType
				                                         DisplayMode.Format,     // D3DMFORMAT AdapterFormat
				                                         0,                      // ULONG Usage
				                                         D3DMRTYPE_DEPTHSTENCIL, // D3DMRESOURCETYPE RType
				                                         SourceFormat)))         // D3DMFORMAT CheckFormat
					{
						bSrcFailReasonChkDevFmt = TRUE;
					}

					break;

				case D3DMFMT_INDEX16:        
				case D3DMFMT_INDEX32:  

					bSrcFailReasonFmtIndex = TRUE;
					break;

				case D3DMFMT_VERTEXDATA:  

					bSrcFailReasonFmtVertexData = TRUE;
					break;

				case D3DMFMT_D3DMVALUE_FLOAT:
				case D3DMFMT_D3DMVALUE_FIXED:

					bSrcFailReasonFmtFixedFloat = TRUE;
					break;

				default:

					bSrcFailReasonFmtInvalid = TRUE;
					break;
				}

				//
				// Does the device support creating the proposed dest at all?
				//
				switch(DestFormat)
				{
				case D3DMFMT_UNKNOWN:
					bDestFailReasonFmtUnknown = TRUE;
					break;

				case D3DMFMT_R8G8B8:  
				case D3DMFMT_A8R8G8B8:
				case D3DMFMT_X8R8G8B8:
				case D3DMFMT_R5G6B5:  
				case D3DMFMT_X1R5G5B5:
				case D3DMFMT_A1R5G5B5:
				case D3DMFMT_A4R4G4B4:
				case D3DMFMT_R3G3B2:  
				case D3DMFMT_A8R3G3B2:
				case D3DMFMT_X4R4G4B4:
				case D3DMFMT_A8P8:    
				case D3DMFMT_P8:      
				case D3DMFMT_A8:
				case D3DMFMT_DXT1:
				case D3DMFMT_DXT2:
				case D3DMFMT_DXT3:
				case D3DMFMT_DXT4:
				case D3DMFMT_DXT5:

					//
					// Does the device support the D3DMRTYPE_SURFACE format at all?
					//
					if (FAILED(m_pD3D->CheckDeviceFormat(uiAdapterIndex,         // UINT Adapter
				                                         D3DMDEVTYPE_DEFAULT,    // D3DMDEVTYPE DeviceType
				                                         DisplayMode.Format,     // D3DMFORMAT AdapterFormat
				                                         0,                      // ULONG Usage
				                                         D3DMRTYPE_SURFACE,      // D3DMRESOURCETYPE RType
				                                         DestFormat)))         // D3DMFORMAT CheckFormat
					{
						bDestFailReasonChkDevFmt = TRUE;
					}

					break;
					
				case D3DMFMT_UYVY:
				case D3DMFMT_YUY2:

					bDestFailReasonYUV = TRUE;
					break;

				case D3DMFMT_D32:    
				case D3DMFMT_D15S1:  
				case D3DMFMT_D24S8:  
				case D3DMFMT_D16:    
				case D3DMFMT_D24X8:
				case D3DMFMT_D24X4S4:

					//
					// Does the device support the D3DMRTYPE_DEPTHSTENCIL format at all?
					//
					if (FAILED(m_pD3D->CheckDeviceFormat(uiAdapterIndex,         // UINT Adapter
				                                         D3DMDEVTYPE_DEFAULT,    // D3DMDEVTYPE DeviceType
				                                         DisplayMode.Format,     // D3DMFORMAT AdapterFormat
				                                         0,                      // ULONG Usage
				                                         D3DMRTYPE_DEPTHSTENCIL, // D3DMRESOURCETYPE RType
				                                         DestFormat)))           // D3DMFORMAT CheckFormat
					{
						bDestFailReasonChkDevFmt = TRUE;
					}

					break;

				case D3DMFMT_INDEX16:        
				case D3DMFMT_INDEX32:  
					bDestFailReasonFmtIndex = TRUE;
					break;

				case D3DMFMT_VERTEXDATA: 
					bDestFailReasonFmtVertexData = TRUE;
					break;

				case D3DMFMT_D3DMVALUE_FLOAT:
				case D3DMFMT_D3DMVALUE_FIXED:
					bDestFailReasonFmtFixedFloat = TRUE;
					break;

				default:
					bDestFailReasonFmtInvalid = TRUE;
					break;
				}

				//
				// Is there any constraint that disqualifies the source format from being useful for StretchRect?
				//
				if ((bSrcFailReasonChkDevFmt    ) ||
					(bSrcFailReasonFmtIndex     ) ||
					(bSrcFailReasonFmtFixedFloat) ||
					(bSrcFailReasonFmtVertexData) ||
					(bSrcFailReasonFmtUnknown   ) ||
					(bSrcFailReasonFmtInvalid   ))
					bSourceSupported = FALSE;
				else
					bSourceSupported = TRUE;

				//
				// Is there any constraint that disqualifies the dest format from being useful for StretchRect?
				//
				if ((bDestFailReasonYUV          ) ||           
					(bDestFailReasonChkDevFmt    ) ||     
					(bDestFailReasonFmtIndex     ) ||      
					(bDestFailReasonFmtFixedFloat) || 
					(bDestFailReasonFmtVertexData) || 
					(bDestFailReasonFmtUnknown   ) ||  
					(bDestFailReasonFmtInvalid   ))
					bDestSupported = FALSE;
				else
					bDestSupported = TRUE;

				//
				// Degenerate conversion should always report as valid, with CheckDeviceFormatConversion, 
				// regardless of whether or not src/dst formats are actually supported
				//
				if (DestFormat == SourceFormat)
				{
					if (FALSE == bConversionSupported)
					{
						DebugOut(_T("\n"));
						DebugOut(_T("CheckDeviceFormatConversion failed unexpectedly [Source: %s; Dest: %s]  Success expected because:\n"),
							(SourceFormat < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[SourceFormat] : _T("?"),
							(DestFormat   < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[DestFormat]   : _T("?"));

						DebugOut(_T("  * CheckDeviceFormatConversion should always report success if src and dest formats are identical (regardless of whether or not formats are supported)."));
						DebugOut(_T("\n"));
						//
						// Record failure for return value; but continue execution of remaining permutations
						//
						Result = TPR_FAIL;
					}
				}
				else
				{
					if (bConversionSupported)
					{
						if (FALSE == bSourceSupported)
						{
							DebugOut(_T("\n"));
							DebugOut(_T("CheckDeviceFormatConversion succeeded unexpectedly [Source: %s; Dest: %s].  Failure expected because:\n"),
								(SourceFormat < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[SourceFormat] : _T("?"),
								(DestFormat   < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[DestFormat]   : _T("?"));

							if (bSrcFailReasonChkDevFmt    )
								DebugOut(_T("    * CheckDeviceFormat failed for this format.\n"));

							if (bSrcFailReasonFmtIndex     )
								DebugOut(_T("    * D3DMFMT_INDEX16 and D3DMFMT_INDEX32 are never a valid formats for use with StretchRect.\n"));

							if (bSrcFailReasonFmtFixedFloat)
								DebugOut(_T("    * D3DMFMT_D3DMVALUE_FLOAT and D3DMFMT_D3DMVALUE_FIXED are never a valid formats for use with StretchRect.\n"));

							if (bSrcFailReasonFmtVertexData)
								DebugOut(_T("    * D3DMFMT_VERTEXDATA is never a valid format for use with StretchRect.\n"));

							if (bSrcFailReasonFmtUnknown   )
								DebugOut(_T("    * D3DMFMT_UNKNOWN is never a valid format for use with StretchRect.\n"));

							if (bSrcFailReasonFmtInvalid   )
								DebugOut(_T("    * The specified destination format has no corresponding D3DMFORMAT enumerator.\n"));

							//
							// Record failure for return value; but continue execution of remaining permutations
							//
							Result = TPR_FAIL;
							continue;
						}

						if (FALSE == bDestSupported)
						{
							DebugOut(_T("\n"));
							DebugOut(_T("CheckDeviceFormatConversion succeeded unexpectedly [Source: %s; Dest: %s].  Failure expected because:\n"),
								(SourceFormat < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[SourceFormat] : _T("?"),
								(DestFormat   < D3DMFMT_NUMFORMAT) ? D3DQA_D3DMFMT_NAMES[DestFormat]   : _T("?"));

							if (bDestFailReasonYUV          )
								DebugOut(_T("    * YUV formats are invalid StretchRect destination formats.\n"));

							if (bDestFailReasonChkDevFmt    )
								DebugOut(_T("    * CheckDeviceFormat failed for this format.\n"));

							if (bDestFailReasonFmtIndex     )
								DebugOut(_T("    * D3DMFMT_INDEX16 and D3DMFMT_INDEX32 are never a valid formats for use with StretchRect.\n"));

							if (bDestFailReasonFmtFixedFloat)
								DebugOut(_T("    * D3DMFMT_D3DMVALUE_FLOAT and D3DMFMT_D3DMVALUE_FIXED are never a valid formats for use with StretchRect.\n"));

							if (bDestFailReasonFmtVertexData)
								DebugOut(_T("    * D3DMFMT_VERTEXDATA is never a valid format for use with StretchRect.\n"));

							if (bDestFailReasonFmtUnknown   )
								DebugOut(_T("    * D3DMFMT_UNKNOWN is never a valid format for use with StretchRect.\n"));

							if (bDestFailReasonFmtInvalid   )
								DebugOut(_T("    * The specified destination format has no corresponding D3DMFORMAT enumerator.\n"));

							//
							// Record failure for return value; but continue execution of remaining permutations
							//
							Result = TPR_FAIL;
							continue;

						}
					}
				}

				//
				// Note:  Even if both formats are supported, and CheckDeviceFormatConversion succeeds, it still might not
				//        be possible to achieve a successful StretchRect call for certain situations.  Example reasons:
				//
				//            * If your app intents to stretch to a render target, an additional check is required to
				//              ensure that the format is supported for a render target
				//            * If calling StretchRect for depth buffers, source and dest must be identical sizes
				//

			}
		}
	}

cleanup:

	return Result;
}

//
// ExecuteConformsToBaseProfileTest
//
//   Verify D3DMPROFILE_BASE
//
// Arguments:
//  
//  <none>
//
// Return Value:
//
//  TPR_PASS, TPR_FAIL, or TPR_SKIP, depending on test result
//
INT IDirect3DMobileTest::ExecuteConformsToBaseProfileTest(D3DMCAPS Caps)
{
	INT Result = TPR_PASS;

	DebugOut(_T("Beginning ExecuteConformsToBaseProfileTest.\n"));

	if (!(Caps.PresentationIntervals & D3DMPRESENT_INTERVAL_ONE))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement:  D3DMPRESENT_INTERVAL_ONE (PresentationIntervals)\n"));
		Result = TPR_FAIL;
	}

	/*
	// D3DMPRESENT_INTERVAL_DEFAULT is defined as zero, and thus cannot be verified programmatically

	if (!(Caps.PresentationIntervals & D3DMPRESENT_INTERVAL_DEFAULT))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement:  D3DMPRESENT_INTERVAL_ONE\n"));
		Result = TPR_FAIL;
	}
	*/

	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSFRONTBUFFER)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDFRONTBUFFER)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement:  D3DMSURFCAPS_SYSFRONTBUFFER or D3DMSURFCAPS_VIDFRONTBUFFER (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}

	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSBACKBUFFER)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDBACKBUFFER)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMSURFCAPS_SYSBACKBUFFER or D3DMSURFCAPS_VIDBACKBUFFER (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}


	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSDEPTHBUFFER)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDDEPTHBUFFER)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMSURFCAPS_SYSDEPTHBUFFER or D3DMSURFCAPS_VIDDEPTHBUFFER (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}


	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSVERTEXBUFFER)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDVERTEXBUFFER)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMSURFCAPS_SYSVERTEXBUFFER or D3DMSURFCAPS_VIDVERTEXBUFFER (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSINDEXBUFFER)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDINDEXBUFFER)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMSURFCAPS_SYSINDEXBUFFER or D3DMSURFCAPS_VIDINDEXBUFFER (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if ((!(Caps.SurfaceCaps & D3DMSURFCAPS_SYSIMAGESURFACE)) &&
		(!(Caps.SurfaceCaps & D3DMSURFCAPS_VIDIMAGESURFACE)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMSURFCAPS_SYSIMAGESURFACE or D3DMSURFCAPS_VIDIMAGESURFACE (SurfaceCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.PrimitiveMiscCaps & D3DMPMISCCAPS_CULLNONE))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement:  D3DMPMISCCAPS_CULLNONE (PrimitiveMiscCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.PrimitiveMiscCaps & D3DMPMISCCAPS_CULLCCW))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement:  D3DMPMISCCAPS_CULLCCW (PrimitiveMiscCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if ((Caps.RasterCaps & D3DMPRASTERCAPS_FOGTABLE) &&
	    (!(Caps.RasterCaps & D3DMPRASTERCAPS_WFOG)) &&
		(!(Caps.RasterCaps & D3DMPRASTERCAPS_ZFOG)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPRASTERCAPS_WFOG or D3DMPRASTERCAPS_ZFOG (RasterCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if ((!(Caps.RasterCaps & D3DMPRASTERCAPS_FOGVERTEX)) &&
		(!(Caps.RasterCaps & D3DMPRASTERCAPS_FOGTABLE)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPRASTERCAPS_FOGVERTEX or D3DMPRASTERCAPS_FOGTABLE (RasterCaps)\n"));
		Result = TPR_FAIL;
	}

	if (!(Caps.ZCmpCaps & D3DMPCMPCAPS_NEVER))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPCMPCAPS_NEVER  (ZCmpCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if ((!(Caps.ZCmpCaps & D3DMPCMPCAPS_LESS)) &&
		(!(Caps.ZCmpCaps & D3DMPCMPCAPS_LESSEQUAL)))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPCMPCAPS_LESS or D3DMPCMPCAPS_LESSEQUAL (ZCmpCaps)\n"));
		Result = TPR_FAIL;
	}

	if (!(Caps.ZCmpCaps & D3DMPCMPCAPS_ALWAYS))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPCMPCAPS_ALWAYS  (ZCmpCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.BlendOpCaps & D3DMBLENDOPCAPS_ADD))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMBLENDOPCAPS_ADD  (BlendOpCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.ShadeCaps & D3DMPSHADECAPS_COLORGOURAUDRGB))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMPSHADECAPS_COLORGOURAUDRGB  (ShadeCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.VertexProcessingCaps & D3DMVTXPCAPS_DIRECTIONALLIGHTS))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMVTXPCAPS_DIRECTIONALLIGHTS  (VertexProcessingCaps)\n"));
		Result = TPR_FAIL;
	}
    
	if (!(Caps.VertexProcessingCaps & D3DMVTXPCAPS_POSITIONALLIGHTS))
	{
		DebugOut(_T("Device does not conform to Base Profile requirement: D3DMVTXPCAPS_POSITIONALLIGHTS (VertexProcessingCaps)\n"));
		Result = TPR_FAIL;
	}

	if (!((Caps.StretchRectFilterCaps & D3DMPTFILTERCAPS_MINFPOINT )   ||
	      (Caps.StretchRectFilterCaps & D3DMPTFILTERCAPS_MINFLINEAR)))
	{
		DebugOut(_T("Device does not expose any D3DMPTFILTERCAPS_MIN* bit in StretchRectFilterCaps.  Provided that StretchRect works with\n"));
		DebugOut(_T("D3DMTEXF_NONE, this is acceptable.\n"));
	}

	if (!((Caps.StretchRectFilterCaps & D3DMPTFILTERCAPS_MAGFPOINT )   ||
	      (Caps.StretchRectFilterCaps & D3DMPTFILTERCAPS_MAGFLINEAR)))
	{
		DebugOut(_T("Device does not expose any D3DMPTFILTERCAPS_MAG* bit in StretchRectFilterCaps.  Provided that StretchRect works with\n"));
		DebugOut(_T("D3DMTEXF_NONE, this is acceptable.\n"));
	}

	return Result;
}