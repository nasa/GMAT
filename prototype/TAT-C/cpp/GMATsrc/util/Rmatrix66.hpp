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
 * Declares linear algebra operations for 6x6 matrices.
 */
//------------------------------------------------------------------------------
#ifndef Rmatrix66_hpp
#define Rmatrix66_hpp

#include "Rmatrix.hpp"

// Forward declarations for operations with vector
class Rvector6;
class Rmatrix33;

class GMATUTIL_API Rmatrix66 : public Rmatrix
{
public:
   Rmatrix66(bool IsIdentityMatrix = true);
   Rmatrix66(int nArgs, Real a1, ...);
   Rmatrix66(const Rmatrix66 &m);
   Rmatrix66(const Rmatrix &m);
   virtual ~Rmatrix66();
   const Rmatrix66& operator=(const Rmatrix66 &m);
   bool operator==(const Rmatrix66 &m)const;
   bool operator!=(const Rmatrix66 &m)const;
   
   void Set(int nArgs, Real a1, ...);
   void SetUndefined();
   
   // Subset of matrix
   Rmatrix33 UpperLeft();
   Rmatrix33 UpperRight();
   Rmatrix33 LowerLeft();
   Rmatrix33 LowerRight();
   
   bool IsOrthogonal(Real accuracyRequired = 
                     GmatRealConstants::REAL_EPSILON) const;
   bool IsOrthonormal(Real accuracyRequired = 
                      GmatRealConstants::REAL_EPSILON) const;
   
   Rmatrix66 operator+(const Rmatrix66& m) const;
   const Rmatrix66& operator+=(const Rmatrix66& m);
   
   Rmatrix66 operator-(const Rmatrix66& m) const;
   const Rmatrix66& operator-=(const Rmatrix66& m);
   
   Rmatrix66 operator*(const Rmatrix66& m) const;
   const Rmatrix66& operator*=(const Rmatrix66& m);
   
   Rmatrix66 operator/(const Rmatrix66& m) const;
   const Rmatrix66& operator/=(const Rmatrix66& m);
   
   Rmatrix66 operator*(Real scalar) const;
   const Rmatrix66& operator*=(Real scalar);
   
   Rmatrix66 operator/(Real scalar) const;
   const Rmatrix66& operator/=(Real scalar);
   
   Rmatrix66 operator-() const;
   
   friend class Rvector6;
   Rvector6 operator*(const Rvector6& v) const;
   
   friend Rmatrix66 operator*(Real scalar, const Rmatrix66& m);
   
   Real               Trace() const;
   Real               Determinant() const;
   virtual Rmatrix66  Transpose() const;
   virtual Rmatrix66  Inverse() const;
   virtual Rmatrix66  Symmetric() const;
   Rmatrix66          AntiSymmetric() const;
   
   friend Rmatrix66  SkewSymmetric(const Rvector6& v);
   friend Rmatrix66  TransposeTimesMatrix(const Rmatrix66& m1,
                                          const Rmatrix66& m2);
   friend Rmatrix66  MatrixTimesTranspose(const Rmatrix66& m1,
                                          const Rmatrix66& m2); 
   friend Rmatrix66  TransposeTimesTranspose(const Rmatrix66& m1,
                                             const Rmatrix66& m2); 
   
   
private:
};
#endif // Rmatrix66_hpp
