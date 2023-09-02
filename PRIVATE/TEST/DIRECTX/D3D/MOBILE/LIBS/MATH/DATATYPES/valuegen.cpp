//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#include "qamath.h"
#include "winerror.h"

//
// GetValuesCommonToFixedAndFloat
//
//   Produces a set of numbers that have an exact representation in both IEEE754-1985
//   single-precision floating point, and in s15.16 fixed point.
//
//   Because the numbers can be represented in either format, the type that this function
//   generates them in is an arbitrary decision; the data will be returned in float.
//   
// Arguments:
//   
//   float **ppfValues  [out]: An array of flt values that have exact flt and fixed representations
//   UINT *puiNumValues [out]: Number of values in array
//   
// Return Value:
//   
//   HRESULT:  Indicates success or failure.
//
HRESULT GetValuesCommonToFixedAndFloat(float **ppfValues, UINT *puiNumValues)
{
    //
    // Iterator for walking through pre-defined data table.
    //
    UINT uiCurSet;

    //
    // Iterator for walking through each individual generated coordinate
    //
    UINT uiStepNum;

    //
    // Per-set, the step size is generated and stored here.
    //
    float fStepSize;   

    //
    // floating point representation for number generated by current iteration
    //
    float fCurVal; 

    //
    // Index into resultant data
    //
    DWORD dwIndex = 0;

    //
    // Number of values expected to be generated.
    //
    DWORD dwNumValues;

    //
    // Bad-parameter checks
    //
    if ((NULL == puiNumValues) || (NULL == ppfValues))
        return E_FAIL;

    //
    // Count the number of coordinates that will be generated, per the table.
    //
    for (uiCurSet=0, dwNumValues=0; uiCurSet < D3DQA_NUMFIXEDFLOAT_RANGES; uiCurSet++)
    {
        dwNumValues+=ValidFixedAndFloat[uiCurSet].uiNumSteps;
    }

    //
    // Allocate exactly enough memory to store all of the to-be-generated
    // data.
    //
    (*ppfValues) = (float*)malloc(sizeof(float)*dwNumValues);
    if (NULL == (*ppfValues))
        return E_FAIL;

    //
    // The function can successfully execute, so it is OK to touch the arg now.
    // 
    *puiNumValues = dwNumValues;

    //
    // Iterate through each "set", generating coordinates
    //
    for (uiCurSet=0; uiCurSet < D3DQA_NUMFIXEDFLOAT_RANGES; uiCurSet++)
    {
        //
        // Set the "starting" value as specified in the table.
        //
        // The cast here should not cause any data loss.
        //
        fCurVal = (float)ValidFixedAndFloat[uiCurSet].iStart;


        //
        // Step size is 1/(2^(ValidFixedAndFloat[uiCurSet].iStep))
        //
        fStepSize = (float)pow(2, (-1)*ValidFixedAndFloat[uiCurSet].iStep);

        //
        // Apply the "direction" directly to the step
        // 
        fStepSize *= ValidFixedAndFloat[uiCurSet].StepDirection;

        //
        // Iterate through the number of steps that are specified in the table
        //
        for(uiStepNum=0; uiStepNum < ValidFixedAndFloat[uiCurSet].uiNumSteps; uiStepNum++)
        {
            //
            // Store the value for this iteration, then increment for next iteration
            //
            (*ppfValues)[dwIndex] = fCurVal;
            fCurVal += fStepSize;
            dwIndex++;
        }
    }

    //
    // All failure conditions have already returned
    //
    return S_OK;
}

//
// Defines that are useful when analysing IEEE 754-1985 single precision floating point numbers.
//
// For more information, see the IEEE standard.
//

//
// Extract the exponent field (bits 23..30); result has these bits in 0..7
//
#define D3DQA_GETFLOAT_EXPONENT(_f) ((_f & 0x7F800000) >> 23)

//
// Same as D3DQA_GETFLOAT_EXPONENT, except without the shift.
//
#define D3DQA_GETFLOAT_EXPONENT_RAW(_f) (_f & 0x7F800000)

//
// Extract the fraction field (bits 0..22); result has these bits in 0..22
//
#define D3DQA_GETFLOAT_FRACTION(_f) (_f & 0x007FFFFF)

//
// Same as D3DQA_GETFLOAT_FRACTION
//
#define D3DQA_GETFLOAT_FRACTION_RAW(_f) (_f & 0x007FFFFF)


//
// Extract the fraction field (bits 0..22); result has bits at position 0
//
#define D3DQA_GETFLOAT_SIGN(_f) ((_f & 0x80000000) >> 31)

//
// Same as D3DQA_GETFLOAT_SIGN, except without the shift.
//
#define D3DQA_GETFLOAT_SIGN_RAW(_f) (_f & 0x80000000)


//
// FloatToSFIX
//
//   Enable the ifdef to get specific diagnostic information about the single-precision float.
//   Otherwise, the conversion will proceed.
//
// Arguments:
//
//   float fIn:  floating point input?
//
// Return Value:
//
//   DWORD:  A s15.16 fixed point result.
//
inline __int32 FloatToSFIX(float fIn)
{

    __int32 iFixed = ((__int32)((double)(fIn)*(double)(1<<16)));

#ifdef D3DQA_FLOAT_DIAGNOSE

    DWORD dwExp = D3DQA_GETFLOAT_EXPONENT(fIn);
    DWORD dwFrac = D3DQA_GETFLOAT_FRACTION(fIn);

    //
    // Is this a normalized float?
    //
    // For more info, see IEEE Std 754-1985, section 3.2.1, numeral 3
    //
    if ((dwExp != 0) &&
        (dwExp != 255))
    {
        //
        // Yes, it is a normalized float.
        //
        DebugBreak();
    }

    //
    // Is this a denormalized float?
    //
    // For more info, see IEEE Std 754-1985, section 3.2.1, numeral 4
    //
    if ((dwExp == 0) &&
        (dwFrac != 0))
    {
        //
        // Yes, it is a denormalized float.
        //
        DebugBreak();
    }

    //
    // Is this a zero-valued float?
    //
    // For more info, see IEEE Std 754-1985, section 3.2.1, numeral 5
    //
    if ((dwExp == 0) &&
        (dwFrac == 0))
    {
        //
        // Yes, it is a zero-valued float.
        //
        DebugBreak();
    }

    //
    // Is this a NaN float?
    //
    // For more info, see IEEE Std 754-1985, section 3.2.1, numeral 1
    //
    if ((dwExp == 255) &&
        (dwFrac != 0))
    {
        //
        // Yes, it is a NaN float.
        //
        // No similar value exists in the fixed-point data type.
        //
        DebugBreak();
    }


    //
    // Is this an infinite float?
    //
    // For more info, see IEEE Std 754-1985, section 3.2.1, numeral 2
    //
    if ((dwExp == 255) &&
        (dwFrac == 0))
    {
        //
        // Yes, it is an infinite float.
        //

        //
        // Return a value that is clamped to min/max of fixed point, retaining
        // the sign from float.
        //
        DebugBreak();
    }

#endif // D3DQA_FLOAT_DIAGNOSE

    return iFixed;
}

//
// FloatToFixedArray
//
//   Convert an array of (x) floats to s15.16.  No special handling for
//   overflow, rounding, or special cases.  Conversion is approximate, unless
//   caller has specifically crafted the input to be suitable for exact
//   conversion.
//
// Arguments:
//
//   DWORD *pdwValues  [in,out]:  array of values (in as float, out as s15.16)
//   UINT uiNumEntries: [in]:  number of entries in array.
//
// Return Value:
//  
//   None:  This is used in some inner loops where special-casing may be
//   prohibitively expensive.  Callers are expected to craft their inputs
//   to be suitable (e.g., don't pass a NULL pointer)
//
void FloatToFixedArray(DWORD *pdwValues, UINT uiNumEntries)
{
    //
    // Iterator for touching each element of the array.
    //
    UINT uiIter;

    for(uiIter=0; uiIter < uiNumEntries; uiIter++)
    {
        
        pdwValues[uiIter] = FloatToSFIX(*(float*)&pdwValues[uiIter]);
    }
}
