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
TESTPROCAPI TexWrapTest(LPTESTCASEARGS pTestCaseArgs);

//
// Number of frames generated by each call to test function
//
#define D3DMQA_TEXWRAPTEST_FRAMECOUNT 1

//
// Test Ordinal Extents
//
#define D3DMQA_TEXWRAPTEST_BASE 3000
#define D3DMQA_TEXWRAPTEST_COUNT 42
#define D3DMQA_TEXWRAPTEST_MAX  (D3DMQA_TEXWRAPTEST_BASE + D3DMQA_TEXWRAPTEST_COUNT - 1)

