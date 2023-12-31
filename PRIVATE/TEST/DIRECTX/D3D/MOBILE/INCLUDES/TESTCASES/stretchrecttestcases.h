//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
// To be included from ..\TestCases.h only

#pragma once

//
// Test function prototype
//
TESTPROCAPI StretchRectTest(LPTESTCASEARGS pTestCaseArgs);

//
// Number of frames generated by each call to test function
//
#define D3DMQA_STRETCHRECTTEST_FRAMECOUNT 1

//
// Test Ordinal Extents
//
#define D3DMQA_STRETCHRECTTEST_COUNT                       150
#define D3DMQA_STRETCHRECTTEST_BASE                        2100
#define D3DMQA_STRETCHRECTTEST_MAX                         (D3DMQA_STRETCHRECTTEST_BASE+D3DMQA_STRETCHRECTTEST_COUNT-1)

