//$Id$
//------------------------------------------------------------------------------
//                                Rvector3
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2003/09/16
//
/**
 * Provides linear algebra operations for 3-element Real vectors.
 */
//------------------------------------------------------------------------------
#ifndef Rvector3_hpp
#define Rvector3_hpp

#include "utildefs.hpp"
#include "Rvector.hpp"
#include "GmatGlobal.hpp"      // for DATA_PRECISION

//forward declaration for matrix operations
class Rmatrix33;

class GMATUTIL_API Rvector3 : public Rvector
{
public:

   Rvector3();
   Rvector3(const Real e1, const Real e2, const Real e3);
   Rvector3(const Rvector3 &v);
   Rvector3(const RealArray &ra);
   virtual ~Rvector3();

   Real Get(Integer index) const;
   void Set(const Real e1, const Real e2, const Real e3);

   virtual Real GetMagnitude() const; 
   Rvector3 GetUnitVector() const;
   const Rvector3& Normalize();
   void ComputeLongitudeLatitude(Real &lon, Real &lat);
   
   Rvector3& operator=(const Rvector3 &v);
   bool operator==(const Rvector3 &v)const;
   bool operator!=(const Rvector3 &v)const;
   Rvector3 operator-() const;                    // negation 
   Rvector3 operator+(const Rvector3 &v) const;
   const Rvector3& operator+=(const Rvector3 &v);
   Rvector3 operator-(const Rvector3 &v) const; 
   const Rvector3& operator-=(const Rvector3 &v);
   Rvector3 operator*(Real s) const;
   const Rvector3& operator*=(Real s);
   Real operator*(const Rvector3 &v) const;       // dot product
   Rvector3 operator/(Real s) const;
   const Rvector3& operator/=(Real s);
   
   friend class Rmatrix33;
   Rvector3 operator*(const Rmatrix33 &m) const; 
   const Rvector3& operator*=(const Rmatrix33 &m);
   Rvector3 operator/(const Rmatrix33 &m) const;
   const Rvector3& operator/=(const Rmatrix33 &m);
   
   friend Rvector3 GMATUTIL_API operator*(Real s, const Rvector3 &v);
   friend Rmatrix33 GMATUTIL_API Outerproduct(const Rvector3 &v1, const Rvector3 &v2);
   friend Rvector3 GMATUTIL_API Cross(const Rvector3 &v1, const Rvector3 &v2);
   
   static Real Normalize(const Real from[3], Real to[3]);
   static void Copy(const Real from[3], Real to[3]);
   
   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   
private:
   static const Integer NUM_DATA = 3;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
};
#endif // Rvector3_hpp
