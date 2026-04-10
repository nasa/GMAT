//$Id$
//------------------------------------------------------------------------------
//                              AttitudeUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/09/27
//
/**
 * This file provides conversion between different attitude representation.
 * (The code is reused from GSS.)
 */
//------------------------------------------------------------------------------
#ifndef AttitudeUtil_hpp
#define AttitudeUtil_hpp

#include "BaseException.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

namespace GmatAttUtil
{
   Rvector3 GMATUTIL_API ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                          Integer seq1, Integer seq2, Integer seq3);
   Rvector3 GMATUTIL_API ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                          Integer seq2, Integer seq3);
   Rmatrix33 GMATUTIL_API ToCosineMatrix(const Rvector3 &qVec, Real qScalar);
   Rmatrix33 GMATUTIL_API ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis);

}

namespace FloatAttUtil
{
   // for float quaternion
   void GMATUTIL_API ToQuat(float q[4], float p1x, float p1y, float p2x, float p2y);
   void GMATUTIL_API ToQuat(float q[4], float mat[16]);
   void GMATUTIL_API IdentityMat(float m[16]);
   void GMATUTIL_API MultMat(float mat1[16], float mat2[16], float mat3[16]);
   void GMATUTIL_API AddQuats(float *q1, float *q2, float *dest);
   void GMATUTIL_API NormalizeQuat(float q[4]);
   void GMATUTIL_API BuildRotMatrix(float m[4][4], float q[4]);
   void GMATUTIL_API AxisToQuat(float a[3], float phi, float q[4]);
   float GMATUTIL_API ToSphereProjection(float r, float x, float y);

   // for float vector util
   void GMATUTIL_API SetZero(float *v);
   void GMATUTIL_API Set(float *v, float x, float y, float z);
   void GMATUTIL_API Subtract(const float *src1, const float *src2, float *dst);
   void GMATUTIL_API Copy(const float *v1, float *v2);
   void GMATUTIL_API Cross(const float *v1, const float *v2, float *cross);
   void GMATUTIL_API Scale(float *v, float div);
   void GMATUTIL_API Normal(float *v);
   void GMATUTIL_API Add(const float *src1, const float *src2, float *dst);
   float GMATUTIL_API Length(const float *v);
   float GMATUTIL_API Dot(const float *v1, const float *v2);
}

#endif // AttitudeUtil_hpp
