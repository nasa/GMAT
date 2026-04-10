//$Id$
//------------------------------------------------------------------------------
//                                  Rmatrix33
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: M. Weippert, T. McRoberts, E. Corderman
// Created: 1996/01/03 for GSS project (Originally Matrix33)
// Modified: 2003/09/15 Linda Jun - See Rmatrix33.hpp
//
/**
 * Defines linear algebra operations for 3x3 matrices.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>

#include "utildefs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"

//---------------------------------
//  static data
//---------------------------------

const std::string Rmatrix33::descs[9] =
{
   "Element 1,1", "Element 1,2", "Element 1,3",
   "Element 2,1", "Element 2,2", "Element 2,3",
   "Element 3,1", "Element 3,2", "Element 3,3"
};


//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rmatrix33(bool IsIdentityRmatrix = true)
//------------------------------------------------------------------------------
Rmatrix33::Rmatrix33(bool IdentityRmatrix)  
   : Rmatrix(3,3) 
{ 
    if(IdentityRmatrix == true)
    {
        elementD[0] = 1.0;
        elementD[4] = 1.0;
        elementD[8] = 1.0;
    }
}

//------------------------------------------------------------------------------
//  Rmatrix33::Rmatrix33(Real a00, Real a01, Real a02,
//                     Real a10, Real a11, Real a12,
//                     Real a20, Real a21, Real a22)
//------------------------------------------------------------------------------
Rmatrix33::Rmatrix33(Real a00, Real a01, Real a02,
           Real a10, Real a11, Real a12,
           Real a20, Real a21, Real a22)
   : Rmatrix(3,3)
{
    elementD[0] = a00;
    elementD[1] = a01;
    elementD[2] = a02;
    elementD[3] = a10;
    elementD[4] = a11;
    elementD[5] = a12;
    elementD[6] = a20;
    elementD[7] = a21;
    elementD[8] = a22;
}


//------------------------------------------------------------------------------
//  const Rmatrix33(const Rmatrix33 &m)
//------------------------------------------------------------------------------
Rmatrix33::Rmatrix33(const Rmatrix33 &m) 
  : Rmatrix(m)
{
}

//------------------------------------------------------------------------------
//  const Rmatrix33(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix33::Rmatrix33(const Rmatrix &m) 
  : Rmatrix(m)
{
}

//------------------------------------------------------------------------------
//  virtual ~Rmatrix33()
//------------------------------------------------------------------------------
Rmatrix33::~Rmatrix33() 
{
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator=(const Rmatrix33 &matrix)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator=(const Rmatrix33 &m) 
{
    Rmatrix::operator=(m);
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rmatrix33 &m) const
//------------------------------------------------------------------------------
bool Rmatrix33::operator==(const Rmatrix33 &m)const 
{
    return Rmatrix::operator==(m);
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rmatrix33 &m) const
//------------------------------------------------------------------------------
bool Rmatrix33::operator!=(const Rmatrix33 &m)const 
{
    return Rmatrix::operator!=(m);
}

//------------------------------------------------------------------------------
// void Set(Real a00, Real a01, Real a02,
//          Real a10, Real a11, Real a12,
//          Real a20, Real a21, Real a22)
//------------------------------------------------------------------------------
void Rmatrix33::Set(Real a00, Real a01, Real a02,
                    Real a10, Real a11, Real a12,
                    Real a20, Real a21, Real a22)
{
    elementD[0] = a00;
    elementD[1] = a01;
    elementD[2] = a02;
    elementD[3] = a10;
    elementD[4] = a11;
    elementD[5] = a12;
    elementD[6] = a20;
    elementD[7] = a21;
    elementD[8] = a22;
}

//------------------------------------------------------------------------------
//  bool IsOrthogonal(Real accuracyRequired = Real_Constants::REAL_EPSILON)
//       const
//------------------------------------------------------------------------------
bool Rmatrix33::IsOrthogonal(Real accuracyRequired) const 
{
    // make column vectors from matrix elements
    Rvector3 cVect0(elementD[0], elementD[3], elementD[6]);
    Rvector3 cVect1(elementD[1], elementD[4], elementD[7]);
    Rvector3 cVect2(elementD[2], elementD[5], elementD[8]);

    return bool(GmatMathUtil::IsZero(cVect0*cVect1, accuracyRequired)
        && GmatMathUtil::IsZero(cVect0*cVect2, accuracyRequired)
        && GmatMathUtil::IsZero(cVect1*cVect2, accuracyRequired));
}

//------------------------------------------------------------------------------
//  bool IsOrthonormal(Real accuracyRequired= Real_Constants::REAL_EPSILON)
//       const
//------------------------------------------------------------------------------
bool Rmatrix33::IsOrthonormal(Real accuracyRequired) const 
{
    // make column vectors from matrix elements
    Rvector3 cVect0(elementD[0], elementD[3], elementD[6]);
    Rvector3 cVect1(elementD[1], elementD[4], elementD[7]);
    Rvector3 cVect2(elementD[2], elementD[5], elementD[8]);

    return bool(GmatMathUtil::IsZero(cVect0.GetMagnitude() - 1, accuracyRequired)
        && GmatMathUtil::IsZero(cVect1.GetMagnitude() - 1, accuracyRequired)
        && GmatMathUtil::IsZero(cVect2.GetMagnitude() - 1, accuracyRequired)
        && IsOrthogonal(accuracyRequired));
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator+(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator+(const Rmatrix33& m) const 
{
    return Rmatrix33(elementD[0] + m.elementD[0],    
                    elementD[1] + m.elementD[1],
                    elementD[2] + m.elementD[2],
                    elementD[3] + m.elementD[3],
                    elementD[4] + m.elementD[4],
                    elementD[5] + m.elementD[5],
                    elementD[6] + m.elementD[6],
                    elementD[7] + m.elementD[7],
                    elementD[8] + m.elementD[8]);
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator+=(const Rmatrix33& m)
//
//  Notes: none
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator+=(const Rmatrix33& m) 
{
   elementD[0] = elementD[0] + m.elementD[0];    
   elementD[1] = elementD[1] + m.elementD[1];
   elementD[2] = elementD[2] + m.elementD[2];
   elementD[3] = elementD[3] + m.elementD[3];
   elementD[4] = elementD[4] + m.elementD[4];
   elementD[5] = elementD[5] + m.elementD[5];
   elementD[6] = elementD[6] + m.elementD[6];
   elementD[7] = elementD[7] + m.elementD[7];
   elementD[8] = elementD[8] + m.elementD[8];

   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator-(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator-(const Rmatrix33& m) const 
{
  return Rmatrix33(elementD[0] - m.elementD[0],    
          elementD[1] - m.elementD[1],
          elementD[2] - m.elementD[2],
          elementD[3] - m.elementD[3],
          elementD[4] - m.elementD[4],
          elementD[5] - m.elementD[5],
          elementD[6] - m.elementD[6],
          elementD[7] - m.elementD[7],
          elementD[8] - m.elementD[8]);
}


//------------------------------------------------------------------------------
//  const Rmatrix33& operator-=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator-=(const Rmatrix33& m) 
{
   elementD[0] = elementD[0] - m.elementD[0];    
   elementD[1] = elementD[1] - m.elementD[1];
   elementD[2] = elementD[2] - m.elementD[2];
   elementD[3] = elementD[3] - m.elementD[3];
   elementD[4] = elementD[4] - m.elementD[4];
   elementD[5] = elementD[5] - m.elementD[5];
   elementD[6] = elementD[6] - m.elementD[6];
   elementD[7] = elementD[7] - m.elementD[7];
   elementD[8] = elementD[8] - m.elementD[8];
   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator*(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator*(const Rmatrix33& m) const 
{
    return
    Rmatrix33((*this)(0,0)*m(0,0) + (*this)(0,1)*m(1,0) + (*this)(0,2)*m(2,0),
         (*this)(0,0)*m(0,1) + (*this)(0,1)*m(1,1) + (*this)(0,2)*m(2,1),
         (*this)(0,0)*m(0,2) + (*this)(0,1)*m(1,2) + (*this)(0,2)*m(2,2),
         (*this)(1,0)*m(0,0) + (*this)(1,1)*m(1,0) + (*this)(1,2)*m(2,0),
         (*this)(1,0)*m(0,1) + (*this)(1,1)*m(1,1) + (*this)(1,2)*m(2,1),
         (*this)(1,0)*m(0,2) + (*this)(1,1)*m(1,2) + (*this)(1,2)*m(2,2),
         (*this)(2,0)*m(0,0) + (*this)(2,1)*m(1,0) + (*this)(2,2)*m(2,0),
         (*this)(2,0)*m(0,1) + (*this)(2,1)*m(1,1) + (*this)(2,2)*m(2,1),
         (*this)(2,0)*m(0,2) + (*this)(2,1)*m(1,2) + (*this)(2,2)*m(2,2));
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator*=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator*=(const Rmatrix33& m) 
{
    Rmatrix33& a = *this;  // rename the object on the left
    
    *this = Rmatrix33(a(0,0)*m(0,0) + a(0,1)*m(1,0) + a(0,2)*m(2,0),
             a(0,0)*m(0,1) + a(0,1)*m(1,1) + a(0,2)*m(2,1),
             a(0,0)*m(0,2) + a(0,1)*m(1,2) + a(0,2)*m(2,2),
             a(1,0)*m(0,0) + a(1,1)*m(1,0) + a(1,2)*m(2,0),
             a(1,0)*m(0,1) + a(1,1)*m(1,1) + a(1,2)*m(2,1),
             a(1,0)*m(0,2) + a(1,1)*m(1,2) + a(1,2)*m(2,2),
             a(2,0)*m(0,0) + a(2,1)*m(1,0) + a(2,2)*m(2,0),
             a(2,0)*m(0,1) + a(2,1)*m(1,1) + a(2,2)*m(2,1),
             a(2,0)*m(0,2) + a(2,1)*m(1,2) + a(2,2)*m(2,2));
    return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator/(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator/(const Rmatrix33& m) const 
{
    return (*this)*m.Inverse();
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator/=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator/=(const Rmatrix33& m) 
{
    return (*this) *= m.Inverse();
}

//------------------------------------------------------------------------------
//  Rmatrix 33 operator*(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator*(Real scalar) const 
{
    return Rmatrix33(elementD[0]*scalar,
            elementD[1]*scalar,
            elementD[2]*scalar, 
            elementD[3]*scalar, 
            elementD[4]*scalar, 
            elementD[5]*scalar, 
            elementD[6]*scalar, 
            elementD[7]*scalar, 
            elementD[8]*scalar);
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator*=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator*=(Real scalar) 
{
    elementD[0] = elementD[0]*scalar;
    elementD[1] = elementD[1]*scalar;
    elementD[2] = elementD[2]*scalar; 
    elementD[3] = elementD[3]*scalar; 
    elementD[4] = elementD[4]*scalar;
    elementD[5] = elementD[5]*scalar; 
    elementD[6] = elementD[6]*scalar; 
    elementD[7] = elementD[7]*scalar; 
    elementD[8] = elementD[8]*scalar;
    
    return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator/(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator/(Real scalar) const 
{
   // Why checking for less than 0.0001? Replaced with IsZero() (LOJ: 2009.03.30)
   //if(scalar < 0.0001) throw Rmatrix::DivideByZero();
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   return Rmatrix33(elementD[0]/scalar, elementD[1]/scalar, elementD[2]/scalar, 
                    elementD[3]/scalar, elementD[4]/scalar, elementD[5]/scalar, 
                    elementD[6]/scalar, elementD[7]/scalar, elementD[8]/scalar);
}

//------------------------------------------------------------------------------
//  const Rmatrix33& operator/=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix33& Rmatrix33::operator/=(Real scalar) 
{
   // Replaced with IsZero() (LOJ: 2009.03.30)
   //if(scalar == 0) throw Rmatrix::DivideByZero();
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   elementD[0] = elementD[0]/scalar;
   elementD[1] = elementD[1]/scalar;
   elementD[2] = elementD[2]/scalar; 
   elementD[3] = elementD[3]/scalar; 
   elementD[4] = elementD[4]/scalar;
   elementD[5] = elementD[5]/scalar; 
   elementD[6] = elementD[6]/scalar; 
   elementD[7] = elementD[7]/scalar; 
   elementD[8] = elementD[8]/scalar;
   
   return *this;
}

//------------------------------------------------------------------------------
//  Rmatrix33 operator-() const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::operator-() const 
{
    return Rmatrix33(-elementD[0], -elementD[1], -elementD[2],
            -elementD[3], -elementD[4], -elementD[5],
            -elementD[6], -elementD[7], -elementD[8]);
}

//------------------------------------------------------------------------------
//  Rvector3 operator*(const Rvector3& v) const
//------------------------------------------------------------------------------
Rvector3 Rmatrix33::operator*(const Rvector3& v) const 
{
    return Rvector3((*this)(0,0)*v(0) + (*this)(0,1)*v(1) + (*this)(0,2)*v(2),
           (*this)(1,0)*v(0) + (*this)(1,1)*v(1) + (*this)(1,2)*v(2),
           (*this)(2,0)*v(0) + (*this)(2,1)*v(1) + (*this)(2,2)*v(2));
           
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 operator*(Real scalar, const Rmatrix33& m)
//------------------------------------------------------------------------------
Rmatrix33 operator*(Real scalar, const Rmatrix33& m) 
{
    return Rmatrix33(scalar*m.elementD[0],
            scalar*m.elementD[1],
            scalar*m.elementD[2],
            scalar*m.elementD[3],
            scalar*m.elementD[4],
            scalar*m.elementD[5],
            scalar*m.elementD[6],
            scalar*m.elementD[7],
            scalar*m.elementD[8]);
    
}

//------------------------------------------------------------------------------
//  Real Trace() const
//------------------------------------------------------------------------------
Real Rmatrix33::Trace() const
{
    return elementD[0] + elementD[4] + elementD[8];
}

//------------------------------------------------------------------------------
//  Real Determinant() const
//------------------------------------------------------------------------------
Real Rmatrix33::Determinant() const
{
    return  (elementD[0]*(elementD[4]*elementD[8] - 
    elementD[5]*elementD[7]) - elementD[1]*(elementD[3]
    *elementD[8] - elementD[5]*elementD[6]) + 
    elementD[2]*(elementD[3]*elementD[7] - elementD[4]*
        elementD[6]));
}

//------------------------------------------------------------------------------
//  virtual Rmatrix33 Transpose() const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::Transpose() const
{
    return Rmatrix33 (elementD[0], elementD[3], elementD[6],
             elementD[1], elementD[4], elementD[7],
             elementD[2], elementD[5], elementD[8]);
    
}

//------------------------------------------------------------------------------
//  virtual Rmatrix33 Inverse() const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::Inverse() const 
{
    // Inverse(M) = 1/det(M) * MCT
    // MCT is formed by computing the cofactors of M(i, j)
    // and storing the ijth cofactor at M(j,i).  The computations of
    // the cofactors where i + j is odd are optimized by removing the
    // unary minus operator and reversing the terms in computing the
    // determinant of the ijth 2x2 minor matrix
    Real D = Determinant();

    if (GmatMathUtil::IsZero(D))
    throw Rmatrix::IsSingular();
    return Rmatrix33(
            (elementD[4]*elementD[8] - elementD[7]*elementD[5]) / D,
            (elementD[7]*elementD[2] - elementD[1]*elementD[8]) / D,
            (elementD[1]*elementD[5] - elementD[4]*elementD[2]) / D,
            (elementD[6]*elementD[5] - elementD[3]*elementD[8]) / D,
            (elementD[0]*elementD[8] - elementD[6]*elementD[2]) / D,
            (elementD[3]*elementD[2] - elementD[0]*elementD[5]) / D,
            (elementD[3]*elementD[7] - elementD[6]*elementD[4]) / D,
            (elementD[6]*elementD[1] - elementD[0]*elementD[7]) / D,
            (elementD[0]*elementD[4] - elementD[1]*elementD[3]) / D);
    
}

//------------------------------------------------------------------------------
//  virtual Rmatrix33 Symmetric() const
//------------------------------------------------------------------------------   
Rmatrix33 Rmatrix33::Symmetric() const
{
    return Rmatrix33 (
    elementD[0],0.5*(elementD[1]+elementD[3]),0.5*(elementD[2]+elementD[6]),
    0.5*(elementD[3]+elementD[1]),elementD[4],0.5*(elementD[5]+elementD[7]),
    0.5*(elementD[6]+elementD[2]),0.5*(elementD[7]+elementD[5]),elementD[8]);
}

//------------------------------------------------------------------------------
//  virtual Rmatrix33 AntiSymmetric() const
//------------------------------------------------------------------------------
Rmatrix33 Rmatrix33::AntiSymmetric() const
{
    return Rmatrix33 (
      0.0, 0.5*(elementD[1] - elementD[3]), 0.5*(elementD[2] - elementD[6]),
      0.5*(elementD[3] - elementD[1]), 0.0, 0.5*(elementD[5] - elementD[7]), 
      0.5*(elementD[6] - elementD[2]), 0.5*(elementD[7] - elementD[5]), 0.0); 
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 SkewSymmetric(const Rvector3& v)
//------------------------------------------------------------------------------
Rmatrix33 SkewSymmetric(const Rvector3& v)
{
    Rmatrix33 m;
    m.elementD[0] = 0.0;
    m.elementD[1] = -v[2];
    m.elementD[2] = v[1];
    m.elementD[3] = v[2];
    m.elementD[4] = 0.0;
    m.elementD[5] = -v[0];
    m.elementD[6] = -v[1];
    m.elementD[7] = v[0];
    m.elementD[8] = 0.0;
    return m;

} 


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 TransposeTimesRmatrix(const Rmatrix33& m1,  const Rmatrix33& m2)
//------------------------------------------------------------------------------
Rmatrix33 TransposeTimesRmatrix(const Rmatrix33& m1, const Rmatrix33& m2) 
{
    return Rmatrix33(m1(0,0)*m2(0,0) + m1(1,0)*m2(1,0) + m1(2,0)*m2(2,0),
                     m1(0,0)*m2(0,1) + m1(1,0)*m2(1,1) + m1(2,0)*m2(2,1),
                     m1(0,0)*m2(0,2) + m1(1,0)*m2(1,2) + m1(2,0)*m2(2,2),
                     m1(0,1)*m2(0,0) + m1(1,1)*m2(1,0) + m1(2,1)*m2(2,0),
                     m1(0,1)*m2(0,1) + m1(1,1)*m2(1,1) + m1(2,1)*m2(2,1),
                     m1(0,1)*m2(0,2) + m1(1,1)*m2(1,2) + m1(2,1)*m2(2,2),
                     m1(0,2)*m2(0,0) + m1(1,2)*m2(1,0) + m1(2,2)*m2(2,0),
                     m1(0,2)*m2(0,1) + m1(1,2)*m2(1,1) + m1(2,2)*m2(2,1),
                     m1(0,2)*m2(0,2) + m1(1,2)*m2(1,2) + m1(2,2)*m2(2,2));
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 MatrixTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2)
//------------------------------------------------------------------------------
Rmatrix33 MatrixTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2)
{
    return Rmatrix33(m1(0,0)*m2(0,0) + m1(0,1)*m2(0,1) + m1(0,2)*m2(0,2),
                     m1(0,0)*m2(1,0) + m1(0,1)*m2(1,1) + m1(0,2)*m2(1,2),
                     m1(0,0)*m2(2,0) + m1(0,1)*m2(2,1) + m1(0,2)*m2(2,2),
                     m1(1,0)*m2(0,0) + m1(1,1)*m2(0,1) + m1(1,2)*m2(0,2),
                     m1(1,0)*m2(1,0) + m1(1,1)*m2(1,1) + m1(1,2)*m2(1,2),
                     m1(1,0)*m2(2,0) + m1(1,1)*m2(2,1) + m1(1,2)*m2(2,2),
                     m1(2,0)*m2(0,0) + m1(2,1)*m2(0,1) + m1(2,2)*m2(0,2),
                     m1(2,0)*m2(1,0) + m1(2,1)*m2(1,1) + m1(2,2)*m2(1,2),
                     m1(2,0)*m2(2,0) + m1(2,1)*m2(2,1) + m1(2,2)*m2(2,2));
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 TransposeTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2)
//------------------------------------------------------------------------------
Rmatrix33 TransposeTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2) 
{
    return Rmatrix33(m1(0,0)*m2(0,0) + m1(1,0)*m2(0,1) + m1(2,0)*m2(0,2),
                     m1(0,0)*m2(1,0) + m1(1,0)*m2(1,1) + m1(2,0)*m2(1,2),
                     m1(0,0)*m2(2,0) + m1(1,0)*m2(2,1) + m1(2,0)*m2(2,2),
                     m1(0,1)*m2(0,0) + m1(1,1)*m2(0,1) + m1(2,1)*m2(0,2),
                     m1(0,1)*m2(1,0) + m1(1,1)*m2(1,1) + m1(2,1)*m2(1,2),
                     m1(0,1)*m2(2,0) + m1(1,1)*m2(2,1) + m1(2,1)*m2(2,2),
                     m1(0,2)*m2(0,0) + m1(1,2)*m2(0,1) + m1(2,2)*m2(0,2),
                     m1(0,2)*m2(1,0) + m1(1,2)*m2(1,1) + m1(2,2)*m2(1,2),
                     m1(0,2)*m2(2,0) + m1(1,2)*m2(2,1) + m1(2,2)*m2(2,2));
}


//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Rmatrix33::GetDataDescriptions() const
{
   return descs;
}
