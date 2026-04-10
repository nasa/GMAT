//$Id$
//------------------------------------------------------------------------------
//                                  Rmatrix66
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.30
//
/**
 * Defines linear algebra operations for 6x6 matrices.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>

#include "Rmatrix66.hpp"
#include "utildefs.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include <stdarg.h>           // for va_list, va_start, va_arg, va_end

//---------------------------------
//  static data
//---------------------------------


//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rmatrix66(bool isIdentityMatrix = true)
//------------------------------------------------------------------------------
Rmatrix66::Rmatrix66(bool isIdentityMatrix)
   : Rmatrix(6,6) 
{ 
   if (isIdentityMatrix)
   {
      elementD[0]  = 1.0;  elementD[7]  = 1.0;  elementD[14] = 1.0;
      elementD[21] = 1.0;  elementD[28] = 1.0;  elementD[35] = 1.0;
   }
}

//------------------------------------------------------------------------------
//  Rmatrix66(int nArgs, Real a1,...)
//------------------------------------------------------------------------------
Rmatrix66::Rmatrix66(int nArgs, Real a1, ...)
   : Rmatrix(6,6)
{
   va_list ap;
   elementD[0] = a1;
   va_start(ap, a1);
   
   int numInput = nArgs <= rowsD*colsD ? nArgs : rowsD*colsD;
   
   for (int i = 1; i < numInput; i++)
      elementD[i] = va_arg(ap, Real);
   
   va_end(ap);
}

//------------------------------------------------------------------------------
//  const Rmatrix66(const Rmatrix66 &m)
//------------------------------------------------------------------------------
Rmatrix66::Rmatrix66(const Rmatrix66 &m) 
  : Rmatrix(m)
{
}

//------------------------------------------------------------------------------
//  const Rmatrix66(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix66::Rmatrix66(const Rmatrix &m) 
  : Rmatrix(m)
{
}

//------------------------------------------------------------------------------
//  virtual ~Rmatrix66()
//------------------------------------------------------------------------------
Rmatrix66::~Rmatrix66() 
{
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator=(const Rmatrix66 &matrix)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator=(const Rmatrix66 &m) 
{
    Rmatrix::operator=(m);
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rmatrix66 &m) const
//------------------------------------------------------------------------------
bool Rmatrix66::operator==(const Rmatrix66 &m)const 
{
    return Rmatrix::operator==(m);
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rmatrix66 &m) const
//------------------------------------------------------------------------------
bool Rmatrix66::operator!=(const Rmatrix66 &m)const 
{
    return Rmatrix::operator!=(m);
}

//------------------------------------------------------------------------------
// void Set(int nArgs, Real a1, ...)
//------------------------------------------------------------------------------
void Rmatrix66::Set(int nArgs, Real a1, ...)
{
   va_list ap;
   elementD[0] = a1;
   va_start(ap, a1);
   
   int numInput = nArgs <= rowsD*colsD ? nArgs : rowsD*colsD;
   
   for (int i = 1; i < numInput; i++)
      elementD[i] = va_arg(ap, Real);
   
   va_end(ap);
}

//------------------------------------------------------------------------------
// void SetUndefined()
//------------------------------------------------------------------------------
void Rmatrix66::SetUndefined()
{
   for (int i=0; i<rowsD*colsD; i++)
      elementD[i] = GmatRealConstants::REAL_UNDEFINED;
}

//------------------------------------------------------------------------------
// Rmatrix33 UpperLeft()
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix66::UpperLeft()
{
   Rmatrix33 mat33(elementD[0],  elementD[1],  elementD[2], 
                   elementD[6],  elementD[7],  elementD[8],
                   elementD[12], elementD[13], elementD[14]);
   return mat33;
}


//------------------------------------------------------------------------------
// Rmatrix33 UpperRight()
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix66::UpperRight()
{
   Rmatrix33 mat33(elementD[3],  elementD[4],  elementD[5], 
                   elementD[9],  elementD[10], elementD[11],
                   elementD[15], elementD[16], elementD[17]);
   return mat33;
}


//------------------------------------------------------------------------------
// Rmatrix33 LowerLeft()
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix66::LowerLeft()
{
   Rmatrix33 mat33(elementD[18], elementD[19], elementD[20], 
                   elementD[24], elementD[25], elementD[26],
                   elementD[30], elementD[31], elementD[32]);
   return mat33;
}


//------------------------------------------------------------------------------
// Rmatrix33 LowerRight()
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix66::LowerRight()
{
   Rmatrix33 mat33(elementD[21], elementD[22], elementD[23], 
                   elementD[27], elementD[28], elementD[29],
                   elementD[33], elementD[34], elementD[35]);
   return mat33;
}


//------------------------------------------------------------------------------
// bool IsOrthogonal(Real accuracyRequired = Real_Constants::REAL_EPSILON) const
//------------------------------------------------------------------------------
bool Rmatrix66::IsOrthogonal(Real accuracyRequired) const 
{
   bool orthogonal = true;  // assume it's orthogonal and try to prove it's not
   
   // create an array of pointers to column vectors
   Rvector6 colVec[6];
   
   // copy from matrix
   for (int i = 0; i < colsD; i++)
   {
      for (int j = 0; j < rowsD; j++)
         (colVec[i])(j) = elementD[j*colsD + i];
   }
   
   // are they mutually orthogonal
   for (int i = 0; i < colsD && orthogonal; i++)
   {
      for (int j = i + 1; j < colsD; j++)
      {
         if (!GmatMathUtil::IsZero((colVec[i])*(colVec[j]), accuracyRequired)) 
            orthogonal = false;
      }
   }
   
   return orthogonal;
   
}

//------------------------------------------------------------------------------
// bool IsOrthonormal(Real accuracyRequired= Real_Constants::REAL_EPSILON) const
//------------------------------------------------------------------------------
bool Rmatrix66::IsOrthonormal(Real accuracyRequired) const 
{
   bool normal = true;  // assume it's normal, try to prove it's not
   
   // create an array of pointers to column vectors
   Rvector6 colVec[6];
   
   // copy from matrix
   for (int i = 0; i < colsD; i++)
   {
      for (int j = 0; j < rowsD; j++)
         (colVec[i])(j) = elementD[j*colsD + i];
   }
   
   // see if each magnitude of each colVec is equal to one
   for (int i = 0; i < colsD && normal; i++)
   {
      if (!GmatMathUtil::IsZero(colVec[i].GetMagnitude() - 1, accuracyRequired))
         normal = false;
   }
   
   return ((bool) (normal && IsOrthogonal(accuracyRequired)));
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator+(const Rmatrix66& m) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator+(const Rmatrix66& m) const 
{
   Rmatrix66 sum(false);
   
   for (int i = 0; i < rowsD*colsD; i++)
      sum.elementD[i] = elementD[i] + m.elementD[i];
   
   return sum;
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator+=(const Rmatrix66& m)
//
//  Notes: none
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator+=(const Rmatrix66& m) 
{
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i] + m.elementD[i];

   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator-(const Rmatrix66& m) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator-(const Rmatrix66& m) const 
{
   Rmatrix66 diff(false);
   
   for (int i = 0; i < rowsD*colsD; i++)
      diff.elementD[i] = elementD[i] + m.elementD[i];
   
   return diff;
}


//------------------------------------------------------------------------------
//  const Rmatrix66& operator-=(const Rmatrix66& m)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator-=(const Rmatrix66& m) 
{
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i] - m.elementD[i];

   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator*(const Rmatrix66& m) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator*(const Rmatrix66& m) const 
{
   Rmatrix66 prod(false);
   
   for (int i = 0; i < rowsD; i++)
   {
      for (int j = 0; j < m.colsD; j++)
      {
         for (int k = 0; k < colsD; k++)
            prod(i, j) += elementD[i*colsD + k]*m(k, j);
      }
   }
   
   return prod;
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator*=(const Rmatrix66& m)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator*=(const Rmatrix66& m) 
{
   *this = *this * m;  
   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator/(const Rmatrix66& m) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator/(const Rmatrix66& m) const 
{
    return (*this)*m.Inverse();
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator/=(const Rmatrix66& m)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator/=(const Rmatrix66& m) 
{
    return (*this) *= m.Inverse();
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator*(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator*(Real scalar) const 
{
   Rmatrix66 prod(false);
   
   for (int i = 0; i < rowsD*colsD; i++)
      prod.elementD[i] = elementD[i]*scalar;
   
   return prod;
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator*=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator*=(Real scalar) 
{
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i]*scalar;
   
   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator/(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator/(Real scalar) const 
{
   Rmatrix66 quot(false);
   
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   for (int i = 0; i < rowsD*colsD; i++)
      quot.elementD[i] = elementD[i]/scalar;
   
   return quot;
}

//------------------------------------------------------------------------------
//  const Rmatrix66& operator/=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix66& Rmatrix66::operator/=(Real scalar) 
{
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i]/scalar;
   
   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix66 operator-() const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::operator-() const 
{
   Rmatrix66 neg(false);
   
   for (int i = 0; i < rowsD*colsD; i++)
      neg.elementD[i] = -elementD[i];
   
   return neg;
}

//------------------------------------------------------------------------------
//  Rvector6 operator*(const Rvector6& v) const
//------------------------------------------------------------------------------
Rvector6 Rmatrix66::operator*(const Rvector6& v) const 
{
   Rvector6 prod;
   
   Real var;
   for (int i = 0; i < rowsD; i++)
   { 
      var = 0.0;
      for (int j = 0; j < colsD; j++)
      {
         prod.elementD[i] = elementD[i*colsD + j]*v.elementD[j];
         var = var + prod.elementD[i];
      }
      prod.elementD[i] = var;
   }
   
   return prod;
}

//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix66 operator*(Real scalar, const Rmatrix66& m)
//------------------------------------------------------------------------------
Rmatrix66 operator*(Real scalar, const Rmatrix66& m) 
{
   Rmatrix66 prod(m);
   
   for (int i = 0; i < m.rowsD*m.colsD; i++)
      prod.elementD[i] *= scalar;
   
   return prod;    
}

//------------------------------------------------------------------------------
//  Real Trace() const
//------------------------------------------------------------------------------
Real Rmatrix66::Trace() const
{
   Real sum = 0;
   
   for (int i = 0; i < rowsD; i++)
      sum += elementD[i*colsD + i];
   
   return sum;
}

//------------------------------------------------------------------------------
//  Real Determinant() const
//------------------------------------------------------------------------------
Real Rmatrix66::Determinant() const
{
   Real D;
   
   if (rowsD == 1)
      D = elementD[0];
   else if (rowsD == 2)
      D = elementD[0]*elementD[3] - elementD[1]*elementD[2];
   else if (rowsD == 3)
   {
      D = elementD[0]*elementD[4]*elementD[8] + 
         elementD[1]*elementD[5]*elementD[6] + 
         elementD[2]*elementD[3]*elementD[7] - 
         elementD[0]*elementD[5]*elementD[7] - 
         elementD[1]*elementD[3]*elementD[8] -
         elementD[2]*elementD[4]*elementD[6];
   }
   else
   {
      D = 0.0;
      for (int i = 0; i < colsD; i++)
         D += elementD[i]*Cofactor(0,i);
   }
   
   return D;
}

//------------------------------------------------------------------------------
//  virtual Rmatrix66 Transpose() const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::Transpose() const
{
   Rmatrix66 tran(false);
   
   for (int i = 0; i < rowsD; i++)
   {
      for (int j = 0; j < colsD; j++)
         tran(j, i) = elementD[i*colsD + j]; 
   }
   
   return tran;
}

//------------------------------------------------------------------------------
//  virtual Rmatrix66 Inverse() const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::Inverse() const 
{
   return Rmatrix66(Rmatrix::Inverse());
}

//------------------------------------------------------------------------------
//  virtual Rmatrix66 Symmetric() const
//------------------------------------------------------------------------------   
Rmatrix66 Rmatrix66::Symmetric() const
{
   return (*this + Transpose())/2;
}

//------------------------------------------------------------------------------
//  virtual Rmatrix66 AntiSymmetric() const
//------------------------------------------------------------------------------
Rmatrix66 Rmatrix66::AntiSymmetric() const
{
   return (*this - Transpose())/2;
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix66 SkewSymmetric(const Rvector6& v)
//------------------------------------------------------------------------------
Rmatrix66 SkewSymmetric(const Rvector6& v)
{
    Rmatrix66 m;
    //@todo - needs work
//     m.elementD[0] =  0.0;
//     m.elementD[1] = -v[2];
//     m.elementD[2] =  v[1];
//     m.elementD[3] =  v[2];
//     m.elementD[4] =  0.0;
//     m.elementD[5] = -v[0];
//     m.elementD[6] = -v[1];
//     m.elementD[7] =  v[0];
//     m.elementD[8] =  0.0;
    return m;

} 


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix66 TransposeTimesMatrix(const Rmatrix66& m1, const Rmatrix66& m2)
//------------------------------------------------------------------------------
Rmatrix66 TransposeTimesMatrix(const Rmatrix66& m1, const Rmatrix66& m2) 
{
   Rmatrix66 m(false);
   
   for (int i = 0; i < m1.colsD; i++)
   {
      for (int j = 0; j < m2.rowsD; j++)
      {
         for (int k = 0; k < m1.rowsD; k++)
            m(i, j) += m1(k, i)*m2(j, k);
      }
   }
   
   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix66 MatrixTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2)
//------------------------------------------------------------------------------
Rmatrix66 MatrixTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2)
{
   Rmatrix66 m(false);
   
   for (int i = 0; i < m1.rowsD; i++)
   {
      for (int j = 0; j < m2.rowsD; j++)
      {
         for (int k = 0; k < m1.colsD; k++)
            m(i, j) += m1(i, k)*m2(j, k);
      }
   }
   
   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix66 TransposeTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2)
//------------------------------------------------------------------------------
Rmatrix66 TransposeTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2) 
{
   Rmatrix66 m(false);
   
   for (int i = 0; i < m1.colsD; i++)
   {
      for (int j = 0; j < m2.rowsD; j++)
      {
         for (int k = 0; k < m1.rowsD; k++)
            m(i, j) += m1(k, i)*m2(j, k);
      }
   }
   
   return m;
}

