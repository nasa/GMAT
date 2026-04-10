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
// Modified: 2003/09/16 Linda Jun - Added new methods and member data to return
//           data descriptions and values.
//
/**
 * Declares linear algebra operations for 3x3 matrices.
 */
//------------------------------------------------------------------------------
#ifndef Rmatrix33_hpp
#define Rmatrix33_hpp

#include "utildefs.hpp"
#include "Rmatrix.hpp"

// Forward declarations for operations with vector
class Rvector3;

class GMATUTIL_API Rmatrix33 : public Rmatrix
{
public:
   Rmatrix33(bool IsIdentityMatrix = true);
   Rmatrix33(Real a00, Real a01, Real a02,
             Real a10, Real a11, Real a12,
             Real a20, Real a21, Real a22);
   Rmatrix33(const Rmatrix33 &m);
   Rmatrix33(const Rmatrix &m);
   virtual ~Rmatrix33();
   const Rmatrix33& operator=(const Rmatrix33 &m);
   bool operator==(const Rmatrix33 &m)const;
   bool operator!=(const Rmatrix33 &m)const;
   
   void Set(Real a00, Real a01, Real a02,
            Real a10, Real a11, Real a12,
            Real a20, Real a21, Real a22);
   
   bool IsOrthogonal(Real accuracyRequired = 
                     GmatRealConstants::REAL_EPSILON) const;
   bool IsOrthonormal(Real accuracyRequired = 
                      GmatRealConstants::REAL_EPSILON) const;
   
   Rmatrix33 operator+(const Rmatrix33& m) const;
   const Rmatrix33& operator+=(const Rmatrix33& m);
   
   Rmatrix33 operator-(const Rmatrix33& m) const;
   const Rmatrix33& operator-=(const Rmatrix33& m);
   
   Rmatrix33 operator*(const Rmatrix33& m) const;
   const Rmatrix33& operator*=(const Rmatrix33& m);
   
   Rmatrix33 operator/(const Rmatrix33& m) const;
   const Rmatrix33& operator/=(const Rmatrix33& m);
   
   Rmatrix33 operator*(Real scalar) const;
   const Rmatrix33& operator*=(Real scalar);
   
   Rmatrix33 operator/(Real scalar) const;
   const Rmatrix33& operator/=(Real scalar);
   
   Rmatrix33 operator-() const;
   
   friend class Rvector3;
   Rvector3 operator*(const Rvector3& v) const;
   
   friend Rmatrix33 operator*(Real scalar, const Rmatrix33& m);
   
   Real               Trace() const;
   Real               Determinant() const;
   virtual Rmatrix33  Transpose() const;
   virtual Rmatrix33  Inverse() const;
   virtual Rmatrix33  Symmetric() const;
   Rmatrix33          AntiSymmetric() const;
   
   friend Rmatrix33  SkewSymmetric(const Rvector3& v);
   friend Rmatrix33  TransposeTimesMatrix(const Rmatrix33& m1,
                                          const Rmatrix33& m2);
   friend Rmatrix33  MatrixTimesTranspose(const Rmatrix33& m1,
                                          const Rmatrix33& m2); 
   friend Rmatrix33  TransposeTimesTranspose(const Rmatrix33& m1,
                                             const Rmatrix33& m2); 
   
   const std::string* GetDataDescriptions() const;
         
private:
   static const std::string descs[9];
};
#endif // Rmatrix33_hpp
