//$Id$
//------------------------------------------------------------------------------
//                              RealUtilities
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
// Created: 2005/11/09
//
//------------------------------------------------------------------------------
#include "RealUtilities.hpp"
#include "AttitudeUtil.hpp"
#include <cmath>

//#define DEBUG_ATT_UTIL 1
#if DEBUG_ATT_UTIL
#include "MessageInterface.hpp"
#endif

using namespace GmatMathUtil;
using namespace GmatAttUtil;


//------------------------------------------------------------------------------
//  Rvector3 ToEuler(Real rotAngle, Rvector3 rotAxis, Integer seq1, ...)
//------------------------------------------------------------------------------
/**
 * @return Euler angles for input sequence
 */
//------------------------------------------------------------------------------
Rvector3 GmatAttUtil::ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                                    Integer seq1, Integer seq2, Integer seq3)
{
   // convert to Quaternion
   Rvector3 unitVector = rotAxis.GetUnitVector();
   const Real sinThetaOverTwo = Sin(rotAngle / 2.0);
   
   Rvector3 qvec;
   qvec = unitVector * sinThetaOverTwo;
   Real qscalar = Cos(rotAngle / 2.0);
   
   // convert it to CosineMatrix
   Rmatrix33 mat = ToCosineMatrix(qvec, qscalar);
   
   // convert it to Euler angles
   Rvector3 eulerAngles = ToEulerAngles(mat, seq1, seq2, seq3);
   return eulerAngles;
}


//------------------------------------------------------------------------------
//  Rvector3 ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
//                         Integer seq2, Integer seq3)
//------------------------------------------------------------------------------
/**
 * @return Euler angles converted from the CosineMatrix and sequence
 */
//------------------------------------------------------------------------------
Rvector3 GmatAttUtil::ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                    Integer seq2, Integer seq3)
{
   const Integer i = seq1;
   const Integer j = seq2;
   const Integer k = seq3;
   
   Integer l;
   Rvector3 retVector(0.0, 0.0, 0.0);
   
//    if (((i == j) || (j == k)) && (j != 0))
//    {
//       throw AttitudeExceptions::InvalidEulerSequence();
//    }
   
   if (k != i)           // first and last axis different
   {
      if (j != 0)
      {
         retVector(1) = ASin(cosMat(k-1,i-1) * Real (((k-i+3) % 3) * 2 -3));
      }
      
      if (i != 0)
      {
         if (cosMat(k-1,j-1) != 0.0 || cosMat(k-1,k-1) != 0.0)
         {
            retVector(0) = ATan(cosMat(k-1,j-1) * 
                                Real (((k-j+3) % 3) * 2 - 3),
                                cosMat(k-1,k-1));
         }
      }
      if (k != 0)
      {
         if (cosMat(j-1,i-1) != 0.0 || cosMat(i-1,i-1) != 0.0)
         {
            retVector(2) = ATan(cosMat(j-1,i-1) * 
                                Real (((j-i+3) % 3) * 2 - 3),
                                cosMat(i-1,i-1));
         }
      }
   }
   else     //  First and last axes are the same
   {
      l = 6 - i - j; 
      if (j != 0)
      {
         retVector(1) = ACos(cosMat(k-1,k-1));
      }
      if (i != 0)
      {
         if (cosMat(k-1,j-1) != 0.0 || cosMat(k-1,l-1) != 0.0)
         {
            retVector(0) = ATan(cosMat(k-1,j-1),
                                cosMat(k-1,l-1) * Real(((k-l+3) % 3) * 2 -3));
         }
      }
      if (k != 0)
      {
         if (cosMat(j-1,i-1) != 0.0 || cosMat(l-1,i-1) != 0.0)
         {
            retVector(2) = ATan(cosMat(j-1,i-1),
                                cosMat(l-1,i-1) * Real(((l-i+3) % 3) * 2 -3));
         }
      }
   }
   
   return retVector;
}


//------------------------------------------------------------------------------
//  Rmatrix33 ToCosineMatrix(const Rvector3 &qVec, Real qScalar)
//------------------------------------------------------------------------------
/**
 * @return CosineMatrix converted form the Quaternion
 */
//------------------------------------------------------------------------------
Rmatrix33 GmatAttUtil::ToCosineMatrix(const Rvector3 &qVec, Real qScalar)
{
   Rmatrix33 mat;
   Rvector3 vec = qVec;
   
   Real q00, q01, q02, q03;
   Real      q11, q12, q13;
   Real           q22, q23;
   Real                q33;
   Real factor;
   
   // Set up terms to create matrix elements
   q00 = vec(0) * vec(0);
   q01 = vec(0) * vec(1);
   q02 = vec(0) * vec(2);
   q03 = vec(0) * qScalar;
   q11 = vec(1) * vec(1);
   q12 = vec(1) * vec(2);
   q13 = vec(1) * qScalar;
   q22 = vec(2) * vec(2);
   q23 = vec(2) * qScalar;
   q33 = qScalar * qScalar;
   factor = 1.0 / (q00 + q11 + q22 + q33);
   
   // Compute matrix elements
   mat(0,0) = (q00 - q11 - q22 + q33) * factor;
   mat(0,1) = 2.0 * (q01 + q23) * factor;
   mat(0,2) = 2.0 * (q02 - q13) * factor;
   mat(1,0) = 2.0 * (q01 - q23) * factor;
   mat(1,1) = (-q00 + q11 - q22 + q33) * factor;
   mat(1,2) = 2.0 * (q12 + q03) * factor;
   mat(2,0) = 2.0 * (q02 + q13) * factor;
   mat(2,1) = 2.0 * (q12 - q03) * factor;
   mat(2,2) = (-q00 - q11 + q22 + q33) * factor;

   return mat;
   
}


//------------------------------------------------------------------------------
//  Rmatrix33 ToCosineMatrix(eal rotAngle, const Rvector3 &rotAxis)
//------------------------------------------------------------------------------
/**
 * @return CosineMatrix converted form rotation angle and axis
 */
//------------------------------------------------------------------------------
Rmatrix33 GmatAttUtil::ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis)
{
   // convert to Quaternion
   Rvector3 unitVector = rotAxis.GetUnitVector();
   const Real sinThetaOverTwo = Sin(rotAngle / 2.0);
   
   Rvector3 qVec;
   qVec = unitVector * sinThetaOverTwo;
   Real qScalar = Cos(rotAngle / 2.0);
   
   // convert it to CosineMatrix
   Rmatrix33 mat = ToCosineMatrix(qVec, qScalar);

   return mat;
}


#define TRACKBALLSIZE  (0.8f)
//------------------------------------------------------------------------------
// void ToQuat(float q[4], float p1x, float p1y, float p2x, float p2y)
//------------------------------------------------------------------------------
/*
 * Simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
//------------------------------------------------------------------------------
void FloatAttUtil::ToQuat(float q[4], float p1x, float p1y, float p2x, float p2y)
{
   float a[3]; /* Axis of rotation */
   float phi;  /* how much to rotate about axis */
   float p1[3], p2[3], d[3];
   float t;

   if (p1x == p2x && p1y == p2y)
   {
      // Zero rotation
      SetZero(q);
      q[3] = 1.0;
      return;
   }

   // First, figure out z-coordinates for projection of P1 and P2 to
   // deformed sphere

   Set(p1, p1x, p1y, ToSphereProjection(TRACKBALLSIZE, p1x, p1y));
   Set(p2, p2x, p2y, ToSphereProjection(TRACKBALLSIZE, p2x, p2y));

   //  Now, we want the cross product of P1 and P2

   Cross(p2, p1, a);

   //  Figure out how much to rotate around that axis.

   Subtract(p1, p2, d);
   t = Length(d) / (2.0f*TRACKBALLSIZE);

   // Avoid problems with out-of-control values...

   if (t > 1.0) t = 1.0;
   if (t < -1.0) t = -1.0;
   phi = 2.0f * (float) asin(t);

   AxisToQuat(a, phi, q);
}


//------------------------------------------------------------------------------
// void FloatAttUtil::ToQuat(float q[4], float mat[16])
//------------------------------------------------------------------------------
void FloatAttUtil::ToQuat(float q[4], float mat[16])
{ 
   float a[4][4];
   static const float ACCURACY = (float)(1.19209290E-07);

   for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
         a[j][i] = mat[i*4+j];
   
   if ((1.0 + a[0][0] + a[1][1] + a[2][2] ) >= ACCURACY)
   {
      q[3] = (float)(Sqrt (1.0 + a[0][0] + a[1][1] + a[2][2] ) / 2.0);
      q[0] = (float)(( a[1][2] - a[2][1] ) / (4.0 * q[3] ));
      q[1] = (float)(( a[2][0] - a[0][2] ) / (4.0 * q[3] ));
      q[2] = (float)(( a[0][1] - a[1][0] ) / (4.0 * q[3] ));
   }
   else if ((1.0 + a[0][0] - a[1][1] - a[2][2] ) >= ACCURACY)
   {
      q[0] = (float)(Sqrt (1.0 + a[0][0] - a[1][1] - a[2][2] ) / 2.0);
      q[1] = (float)(( a[0][1] + a[1][0] ) / (4.0 * q[0] ));
      q[2] = (float)(( a[2][0] + a[0][2] ) / (4.0 * q[0] ));
      q[3] = (float)(( a[1][2] - a[2][1] ) / (4.0 * q[0] ));
   }
   else if ((1.0 - a[0][0] + a[1][1] - a[2][2] ) >= ACCURACY)
   {
      q[1] = (float)(Sqrt (1.0 - a[0][0] + a[1][1] - a[2][2] ) / 2.0);
      q[0] = (float)(( a[0][1] + a[1][0] ) / (4.0 * q[1] ));
      q[2] = (float)(( a[1][2] + a[2][1] ) / (4.0 * q[1] ));
      q[3] = (float)(( a[2][0] - a[0][2] ) / (4.0 * q[1] ));
   }
   else
   {
      q[2] = (float)(Sqrt (1.0 - a[0][0] - a[1][1] + a[2][2] ) / 2.0);
      q[0] = (float)(( a[2][0] + a[0][2] ) / (4.0 * q[2] ));
      q[1] = (float)(( a[1][2] + a[2][1] ) / (4.0 * q[2] ));
      q[3] = (float)(( a[0][1] - a[1][0] ) / (4.0 * q[2] ));
   }

   NormalizeQuat(q);

   #if DEBUG_ATT_UTIL
   MessageInterface::ShowMessage
      ("FloatAttUtil::ToQuat() mat=\n%f, %f, %f, %f\n%f, %f, %f, %f\n"
       "%f, %f, %f, %f\n%f, %f, %f, %f\n",
       mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[8],
       mat[8], mat[9], mat[10], mat[11], mat[12], mat[13], mat[14], mat[15]);
   
   for (int i=0; i<4; i++)
      MessageInterface::ShowMessage
         ("FloatAttUtil::ToQuat() a=%f, %f, %f, %f\n", a[i][0], a[i][1],
          a[i][2], a[i][3]);
   
   MessageInterface::ShowMessage
      ("FloatAttUtil::ToQuat() q=%f, %f, %f, %f\n", q[0], q[1], q[2], q[3]);
   
   #endif
}


//------------------------------------------------------------------------------
// void IdentityMat(float m[16])
//------------------------------------------------------------------------------
void FloatAttUtil::IdentityMat(float m[16])
{
   m[0] = 1.0;
   m[1] = 0.0;
   m[2] = 0.0;
   m[3] = 0.0;
   
   m[4] = 0.0;
   m[5] = 1.0;
   m[6] = 0.0;
   m[7] = 0.0;
   
   m[8] = 0.0;
   m[9] = 0.0;
   m[10] = 1.0;
   m[11] = 0.0;
   
   m[12] = 0.0;
   m[13] = 0.0;
   m[14] = 0.0;
   m[15] = 1.0;
   
}


//------------------------------------------------------------------------------
// void MultMat(float m1[16], float m2[16], float m3[16]
//------------------------------------------------------------------------------
void FloatAttUtil::MultMat(float mat1[16], float mat2[16], float mat3[16])
{   
   float m1[4][4], m2[4][4];

   for (int i=0; i<4; i++)
   {
      for (int j=0; j<4; j++)
      {
         m1[j][i] = mat1[i*4+j];
         m2[j][i] = mat2[i*4+j];
      }
   }
   
   mat3[0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0];
   mat3[1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1];
   mat3[2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2];
   mat3[4] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0];
   mat3[5] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1];
   mat3[6] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2];
   mat3[8] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0];
   mat3[9] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1];
   mat3[10] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2];
}


//------------------------------------------------------------------------------
// void AxisToQuat(float a[3], float phi, float q[4])
//------------------------------------------------------------------------------
/*
 *  Given an axis and angle, compute quaternion.
 */
//------------------------------------------------------------------------------
void FloatAttUtil::AxisToQuat(float a[3], float phi, float q[4])
{
   Normal(a);
   Copy(a, q);
   Scale(q, (float) sin(phi/2.0));
   q[3] = (float) cos(phi/2.0);
}


//------------------------------------------------------------------------------
// float ToSphereProjection(float r, float x, float y)
//------------------------------------------------------------------------------
/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
//------------------------------------------------------------------------------
float FloatAttUtil::ToSphereProjection(float r, float x, float y)
{
   float d, t, z;

   d = (float) sqrt(x*x + y*y);
   if (d < r * 0.70710678118654752440)
   {
      // Inside sphere
      z = (float) sqrt(r*r - d*d);
   }
   else
   {
      // On hyperbola 
      t = r / 1.41421356237309504880f;
      z = t*t / d;
   }
   
   return z;
}


#define RENORMCOUNT 97

//------------------------------------------------------------------------------
// AddQuats(float q1[4], float q2[4], float dest[4])
//------------------------------------------------------------------------------
void FloatAttUtil::AddQuats(float q1[4], float q2[4], float dest[4])
{
   static int count=0;
   float t1[4], t2[4], t3[4];
   float tf[4];

   Copy(q1, t1);
   Scale(t1, q2[3]);

   Copy(q2, t2);
   Scale(t2, q1[3]);

   Cross(q2, q1, t3);
   Add(t1, t2, tf);
   Add(t3, tf, tf);
   tf[3] = q1[3] * q2[3] - Dot(q1,q2);

   dest[0] = tf[0];
   dest[1] = tf[1];
   dest[2] = tf[2];
   dest[3] = tf[3];

   if (++count > RENORMCOUNT)
   {
      count = 0;
      NormalizeQuat(dest);
   }
}


//------------------------------------------------------------------------------
// void NormalizeQuat(float q[4])
//------------------------------------------------------------------------------
void FloatAttUtil::NormalizeQuat(float q[4])
{
   int i;
   float mag;

   mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
   for (i = 0; i < 4; i++) q[i] /= mag;
}


//------------------------------------------------------------------------------
// void BuildRotMatrix(float m[4][4], float q[4])
//------------------------------------------------------------------------------
void FloatAttUtil::BuildRotMatrix(float m[4][4], float q[4])
{
   m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
   m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
   m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
   m[0][3] = 0.0f;

   m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
   m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
   m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
   m[1][3] = 0.0f;

   m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
   m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
   m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
   m[2][3] = 0.0f;

   m[3][0] = 0.0f;
   m[3][1] = 0.0f;
   m[3][2] = 0.0f;
   m[3][3] = 1.0f;
}


//------------------------------------------------------------------------------
// void SetZero(float *v)
//------------------------------------------------------------------------------
void FloatAttUtil::SetZero(float *v)
{
   v[0] = 0.0;
   v[1] = 0.0;
   v[2] = 0.0;
}


//------------------------------------------------------------------------------
// void Set(float *v, float x, float y, float z)
//------------------------------------------------------------------------------
void FloatAttUtil::Set(float *v, float x, float y, float z)
{
   v[0] = x;
   v[1] = y;
   v[2] = z;
}


//------------------------------------------------------------------------------
// void Subtract(const float *src1, const float *src2, float *dst)
//------------------------------------------------------------------------------
void FloatAttUtil::Subtract(const float *src1, const float *src2, float *dst)
{
   dst[0] = src1[0] - src2[0];
   dst[1] = src1[1] - src2[1];
   dst[2] = src1[2] - src2[2];
}


//------------------------------------------------------------------------------
// void Copy(const float *v1, float *v2)
//------------------------------------------------------------------------------
void FloatAttUtil::Copy(const float *v1, float *v2)
{
   int i;
   for (i = 0 ; i < 3 ; i++)
      v2[i] = v1[i];
}


//------------------------------------------------------------------------------
// void Cross(const float *v1, const float *v2, float *cross)
//------------------------------------------------------------------------------
void FloatAttUtil::Cross(const float *v1, const float *v2, float *cross)
{
   float temp[3];

   temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
   temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
   temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
   Copy(temp, cross);
}


//------------------------------------------------------------------------------
// float Length(const float *v)
//------------------------------------------------------------------------------
float FloatAttUtil::Length(const float *v)
{
   return (float) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}


//------------------------------------------------------------------------------
// void Scale(float *v, float div)
//------------------------------------------------------------------------------
void FloatAttUtil::Scale(float *v, float div)
{
   v[0] *= div;
   v[1] *= div;
   v[2] *= div;
}


//------------------------------------------------------------------------------
// void Normal(float *v)
//------------------------------------------------------------------------------
void FloatAttUtil::Normal(float *v)
{
   Scale(v, 1.0f/Length(v));
}


//------------------------------------------------------------------------------
// float Dot(const float *v1, const float *v2)
//------------------------------------------------------------------------------
float FloatAttUtil::Dot(const float *v1, const float *v2)
{
   return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}


//------------------------------------------------------------------------------
// void Add(const float *src1, const float *src2, float *dst)
//------------------------------------------------------------------------------
void FloatAttUtil::Add(const float *src1, const float *src2, float *dst)
{
   dst[0] = src1[0] + src2[0];
   dst[1] = src1[1] + src2[1];
   dst[2] = src1[2] + src2[2];
}


