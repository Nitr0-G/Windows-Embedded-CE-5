//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//
#include <windows.h>
#include "QAMath.h"
#include <math.h>

#define _M(a) D3DM_MAKE_D3DMVALUE(a)

//
// RotateXMatrix
//
//   Creates a matrix suitable for rotation about the X axis; according to
//   the angle, in radians, specified by the caller.
//
D3DMMATRIX RotateXMatrix(const float rads)
{
	D3DMMATRIX ret = IdentityMatrix();
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);

	ret.m[1][1] = _M(cosine);
	ret.m[2][2] = _M(cosine);
	ret.m[1][2] = _M(-sine);
	ret.m[2][1] = _M(sine);
    return ret;
}

//
// RotateYMatrix
//
//   Creates a matrix suitable for rotation about the Y axis; according to
//   the angle, in radians, specified by the caller.
//
D3DMMATRIX RotateYMatrix(const float rads)
{
	float	cosine, sine;
	D3DMMATRIX ret = IdentityMatrix();

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
   
    ret.m[0][0] = _M(cosine);
	ret.m[2][2] = _M(cosine);
	ret.m[0][2] = _M(sine);
	ret.m[2][0] = _M(-sine);
    return ret;
}
  
//
// RotateZMatrix
//
//   Creates a matrix suitable for rotation about the Z axis; according to
//   the angle, in radians, specified by the caller.
//
D3DMMATRIX RotateZMatrix(const float rads)
{
	float	cosine, sine;
    D3DMMATRIX ret = IdentityMatrix();

	cosine = (float) cos(rads);
	sine = (float) sin(rads);

    ret.m[0][0] = _M(cosine);
	ret.m[1][1] = _M(cosine);
	ret.m[0][1] = _M(-sine);
	ret.m[1][0] = _M(sine);
    return ret;
}


//
// GetRotationInverses
//
//   Generates a rotation matrix, of the specified type and angles.
//
//   Depending on the rotation type, some rotation angle arguments may be unused.
//
bool GetRotation(D3DQA_ROTTYPE rType,
	 		     const float fXRot,
				 const float fYRot,
				 const float fZRot,
				 D3DMMATRIX* const RotMatrix)
{
	D3DMMATRIX TempMatrix;
	unsigned int uiRotNumber;

	if ((NULL == RotMatrix) || (rType > D3DQA_LAST_ROTTYPE))
	{
		OutputDebugString(L"GetRotationInverses:  Invalid argument(s).\n");
		return false;
	}

	*RotMatrix = IdentityMatrix();

	//
	// Maximum of three rotations
	//
	for (uiRotNumber = 0; uiRotNumber < D3DQA_NUM_AXIS; uiRotNumber++)
	{
		//
		// Rotation matrix
		//
		switch(ROTORDER[rType][uiRotNumber])
		{
		case D3DQA_ROTAXIS_X: 
			TempMatrix = RotateXMatrix(fXRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Y: 
			TempMatrix = RotateYMatrix(fYRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Z: 
			TempMatrix = RotateZMatrix(fZRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		};

	}

	return true;
}
	
//
// GetRotationInverses
//
//   Generates a rotation matrix, of the specified type and angles, and also
//   generates the inverse matrix for the rotation.
//
//   Depending on the rotation type, some rotation angle arguments may be unused.
//
bool GetRotationInverses(D3DQA_ROTTYPE rType,
	 		             const float fXRot,
					     const float fYRot,
					     const float fZRot,
						 D3DMMATRIX* const RotMatrix,
						 D3DMMATRIX* const RotMatrixInverse)
{
	D3DMMATRIX TempMatrix;
	D3DQA_ROTTYPE rInverseType;
	unsigned int uiRotNumber;

	if ((NULL == RotMatrix) || (NULL == RotMatrixInverse) || (rType > D3DQA_LAST_ROTTYPE))
	{
		OutputDebugString(L"GetRotationInverses:  Invalid argument(s).\n");
		return false;
	}

	rInverseType = ROTINVERSE[rType];

	*RotMatrix = *RotMatrixInverse = IdentityMatrix();

	//
	// Maximum of three rotations
	//
	for (uiRotNumber = 0; uiRotNumber < D3DQA_NUM_AXIS; uiRotNumber++)
	{
		//
		// Rotation matrix
		//
		switch(ROTORDER[rType][uiRotNumber])
		{
		case D3DQA_ROTAXIS_X: 
			TempMatrix = RotateXMatrix(fXRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Y: 
			TempMatrix = RotateYMatrix(fYRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Z: 
			TempMatrix = RotateZMatrix(fZRot);
			*RotMatrix = MatrixMult(RotMatrix, &TempMatrix);
			break;
		};

		//
		// Inverse of rotation matrix; generated by
		// rotating by ((-1)*Angle); with opposite "order of
		// rotations"
		//
		switch(ROTORDER[rInverseType][uiRotNumber])
		{
		case D3DQA_ROTAXIS_X: 
			TempMatrix = RotateXMatrix((-1)*fXRot);
			*RotMatrixInverse = MatrixMult(RotMatrixInverse, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Y: 
			TempMatrix = RotateYMatrix((-1)*fYRot);
			*RotMatrixInverse = MatrixMult(RotMatrixInverse, &TempMatrix);
			break;
		case D3DQA_ROTAXIS_Z: 
			TempMatrix = RotateZMatrix((-1)*fZRot);
			*RotMatrixInverse = MatrixMult(RotMatrixInverse, &TempMatrix);
			break;
		};
	}

	return true;
}

//
// UsesXRot
//
//   This is a utility function for use in conjunction with D3DQA_ROTTYPE.  Given
//   any valid rotation type; this function will determine whether or not the
//   specified rotation type performs a rotation about the X Axis.
//
// Return Value:
//
//   bool:  true if the specified rotation includes a rotation about the X Axis
//
bool UsesXRot(D3DQA_ROTTYPE Rot)
{
	if (D3DQA_ROTAXIS_X == ROTORDER[Rot][0])
		return true; // First axis of rotation is X-Axis

	if (D3DQA_ROTAXIS_X == ROTORDER[Rot][1])
		return true; // Second axis of rotation is X-Axis
	
	if (D3DQA_ROTAXIS_X == ROTORDER[Rot][2])
		return true; // Third axis of rotation is X-Axis

	return false;
}


//
// UsesAxis
//
//   This is a utility function for use in conjunction with D3DQA_ROTTYPE.  Given
//   any valid rotation type; this function will determine whether or not the
//   specified rotation type performs a rotation about the specified axis.
//
// Return Value:
//
//   bool:  true if the specified rotation includes a rotation about the specified axis
//
bool UsesAxis(D3DQA_ROTAXIS RotAxis, D3DQA_ROTTYPE RotType)
{
	switch(RotAxis)
	{
	case D3DQA_ROTAXIS_X:
		switch(RotType)
		{
		case D3DQA_ROTTYPE_X:
		case D3DQA_ROTTYPE_XY:
		case D3DQA_ROTTYPE_XZ:
		case D3DQA_ROTTYPE_YX:
		case D3DQA_ROTTYPE_ZX:
		case D3DQA_ROTTYPE_XYZ:
		case D3DQA_ROTTYPE_XZY:
		case D3DQA_ROTTYPE_YXZ:
		case D3DQA_ROTTYPE_YZX:
		case D3DQA_ROTTYPE_ZXY:
		case D3DQA_ROTTYPE_ZYX:
			return true;

		case D3DQA_ROTTYPE_Y:
		case D3DQA_ROTTYPE_Z:
		case D3DQA_ROTTYPE_YZ:
		case D3DQA_ROTTYPE_ZY:
		default:
			return false;
		}
	case D3DQA_ROTAXIS_Y:
		switch(RotType)
		{
		case D3DQA_ROTTYPE_Y:
		case D3DQA_ROTTYPE_XY:
		case D3DQA_ROTTYPE_YX:
		case D3DQA_ROTTYPE_YZ:
		case D3DQA_ROTTYPE_ZY:
		case D3DQA_ROTTYPE_XYZ:
		case D3DQA_ROTTYPE_XZY:
		case D3DQA_ROTTYPE_YXZ:
		case D3DQA_ROTTYPE_YZX:
		case D3DQA_ROTTYPE_ZXY:
		case D3DQA_ROTTYPE_ZYX:
			return true;

		case D3DQA_ROTTYPE_X:
		case D3DQA_ROTTYPE_Z:
		case D3DQA_ROTTYPE_XZ:
		case D3DQA_ROTTYPE_ZX:
		default:
			return false;
		}
	case D3DQA_ROTAXIS_Z:
		switch(RotType)
		{
		case D3DQA_ROTTYPE_Z:
		case D3DQA_ROTTYPE_XZ:
		case D3DQA_ROTTYPE_YZ:
		case D3DQA_ROTTYPE_ZX:
		case D3DQA_ROTTYPE_ZY:
		case D3DQA_ROTTYPE_XYZ:
		case D3DQA_ROTTYPE_XZY:
		case D3DQA_ROTTYPE_YXZ:
		case D3DQA_ROTTYPE_YZX:
		case D3DQA_ROTTYPE_ZXY:
		case D3DQA_ROTTYPE_ZYX:
			return true;

		case D3DQA_ROTTYPE_X:
		case D3DQA_ROTTYPE_Y:
		case D3DQA_ROTTYPE_XY:
		case D3DQA_ROTTYPE_YX:
		default:
			return false;
		}
	default:
		return false;
	}
}

//
// UsesYRot
//
//   This is a utility function for use in conjunction with D3DQA_ROTTYPE.  Given
//   any valid rotation type; this function will determine whether or not the
//   specified rotation type performs a rotation about the Y Axis.
//
// Return Value:
//
//   bool:  true if the specified rotation includes a rotation about the Y Axis
//
bool UsesYRot(D3DQA_ROTTYPE Rot)
{
	if (D3DQA_ROTAXIS_Y == ROTORDER[Rot][0])
		return true; // First axis of rotation is Y-Axis

	if (D3DQA_ROTAXIS_Y == ROTORDER[Rot][1])
		return true; // Second axis of rotation is Y-Axis
	
	if (D3DQA_ROTAXIS_Y == ROTORDER[Rot][2])
		return true; // Third axis of rotation is Y-Axis

	return false;
}

//
// UsesZRot
//
//   This is a utility function for use in conjunction with D3DQA_ROTTYPE.  Given
//   any valid rotation type; this function will determine whether or not the
//   specified rotation type performs a rotation about the Z Axis.
//
// Return Value:
//
//   bool:  true if the specified rotation includes a rotation about the Z Axis
//
bool UsesZRot(D3DQA_ROTTYPE Rot)
{
	if (D3DQA_ROTAXIS_Z == ROTORDER[Rot][0])
		return true; // First axis of rotation is Z-Axis

	if (D3DQA_ROTAXIS_Z == ROTORDER[Rot][1])
		return true; // Second axis of rotation is Z-Axis
	
	if (D3DQA_ROTAXIS_Z == ROTORDER[Rot][2])
		return true; // Third axis of rotation is Z-Axis

	return false;
}



//
// GetAxisCount
//
//   This is a utility function for use in conjunction with D3DQA_ROTTYPE.  Given
//   any valid rotation type; this function will determine the number of axis that
//   are involved.
//
// Return Value:
//
//   HRESULT indicates success or failure
//
HRESULT GetAxisCount(PUINT puiAxis, D3DQA_ROTTYPE RotType)
{
	if (NULL == puiAxis)
		return E_FAIL;

	*puiAxis = 0;

	if (UsesAxis(D3DQA_ROTAXIS_X, RotType))
		(*puiAxis)++;

	if (UsesAxis(D3DQA_ROTAXIS_Y, RotType))
		(*puiAxis)++;

	if (UsesAxis(D3DQA_ROTAXIS_Z, RotType))
		(*puiAxis)++;

	return S_OK;
}
