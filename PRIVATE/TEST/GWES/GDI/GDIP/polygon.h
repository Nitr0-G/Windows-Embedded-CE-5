//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#include "bencheng.h"
#include "otak.h"
#include "surface.h"
#include "rop2.h"
#include "pen.h"
#include "brush.h"
#include "polycoordinates.h"
#include "rgn.h"
#include "dispperfdata.h"

#ifndef POLYGON_H
#define POLYGON_H
class CPolygonTestSuite : public CTestSuite
{
    public:
        CPolygonTestSuite(CSection * Section) : CTestSuite(Section), 
                                            m_Dest(Section), m_hdcDest(NULL), m_Coordinates(Section), 
                                            m_Rop2(Section), m_Pen(Section), m_Brush(Section), m_Rgn(Section), m_DispPerfData()
                                            { g_pCOtakLog->Log(OTAK_VERBOSE, TEXT("In CPolygonTestSuite overloaded constructor.")); }
         ~CPolygonTestSuite() { g_pCOtakLog->Log(OTAK_VERBOSE, TEXT("In CPolygonTestSuite destructor.")); }

        virtual BOOL Initialize(TestSuiteInfo *);
        virtual BOOL PreRun(TestInfo *);
        virtual BOOL Run();
        virtual BOOL AddPostRunData(TestInfo *);
        virtual BOOL PostRun();
        virtual BOOL Cleanup();

private:
        CPolygonTestSuite() { g_pCOtakLog->Log(OTAK_VERBOSE, TEXT("In CPolygonTestSuite base constructor.")); }

        int m_nIterationCount;
        class CSurface m_Dest;
        class CPolyCoordinates m_Coordinates;
        class CRop2 m_Rop2;
        class CPen m_Pen;
        class CBrush m_Brush;
        class CRgn m_Rgn;
        class CDispPerfData m_DispPerfData;

        HDC m_hdcDest;

        POINT *m_sPointsInUse;
        int m_nCoordinateCountInUse;

#ifndef UNDER_CE
        DWORD m_dwOldBatchLimit;
#endif

};
#endif
