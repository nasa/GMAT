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
 * Provides definitions for the Rvector3 class, providing linear algebra
 * operations for the 3-element Real vector.
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "utildefs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "Linear.hpp"
#include "MessageInterface.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string Rvector3::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Element 1",
   "Element 2",
   "Element 3"
};

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rvector3()
//------------------------------------------------------------------------------
Rvector3::Rvector3()
   : Rvector(3)
{
}

//------------------------------------------------------------------------------
//  Rvector3(const Real e1, const Real e2, const Real e3)
//------------------------------------------------------------------------------
Rvector3::Rvector3(const Real e1, const Real e2, const Real e3)
   : Rvector(3, e1, e2, e3) 
{
}

//------------------------------------------------------------------------------
//  Rvector3(const Rvector3 &v)
//------------------------------------------------------------------------------
Rvector3::Rvector3(const Rvector3 &v)
   : Rvector(v)
{
}

//------------------------------------------------------------------------------
//  Rvector3(const RealArray &ra)
//------------------------------------------------------------------------------
Rvector3::Rvector3(const RealArray &ra)
   : Rvector(ra)
{
   // This check must be made here or the resulting Rvector3 may not
   // be of size 3!!
   if (ra.size() != 3)
      throw ArrayTemplateExceptions::DimensionError();
}

//------------------------------------------------------------------------------
//  ~Rvector3()
//------------------------------------------------------------------------------
Rvector3::~Rvector3()
{
}

//------------------------------------------------------------------------------
// Real Get(const Integer index) const
//------------------------------------------------------------------------------
Real Rvector3::Get(const Integer index) const
{
   return elementD[index];
}

//------------------------------------------------------------------------------
// void Set(const Real e1, const Real e2, const Real e3)
//------------------------------------------------------------------------------
void Rvector3::Set(const Real e1, const Real e2, const Real e3)
{
   elementD[0] = e1;
   elementD[1] = e2;
   elementD[2] = e3;
}

//------------------------------------------------------------------------------
//  Real GetMagnitude() const
//------------------------------------------------------------------------------
Real Rvector3::GetMagnitude() const
{
    return GmatMathUtil::Sqrt(elementD[0]*elementD[0] +
                              elementD[1]*elementD[1] +
                              elementD[2]*elementD[2]);
}


//------------------------------------------------------------------------------
//  Rvector3 GetUnitVector() const
//------------------------------------------------------------------------------
Rvector3 Rvector3::GetUnitVector() const
{
   Real mag = GetMagnitude();

   if (GmatMathUtil::IsZero(mag))
      throw ZeroVector(" from Rvector3::GetUnitVector()\n");
   
   return Rvector3(elementD[0]/mag, elementD[1]/mag, elementD[2]/mag);
}


//------------------------------------------------------------------------------
//  const Rvector3& Normalize()
//------------------------------------------------------------------------------
const Rvector3& Rvector3::Normalize()
{
   Real mag = GetMagnitude();
   
   if (GmatMathUtil::IsZero(mag))
      throw ZeroVector(" from Rvector3::Normalize()\n");
   
   elementD[0] /= mag;
   elementD[1] /= mag;
   elementD[2] /= mag;
   return *this;
}


//------------------------------------------------------------------------------
// void ComputeLongitudeLatitude(Real &lon, Real &lat)
//------------------------------------------------------------------------------
/**
 * Computes longitude and latitude in radians.
 * Longitude returns value between -PI and +PI.
 * Latitude returns value between -PI/2 and +PI/2
 *
 * @note - Consider using BodyFixedStateConverter::CartesianToSpherical()
 */
//------------------------------------------------------------------------------
void Rvector3::ComputeLongitudeLatitude(Real &lon, Real &lat)
{
   lon = GmatMathUtil::ATan2(elementD[1], elementD[0]);
   lat = GmatMathUtil::ASin(elementD[2] / GetMagnitude());
}


//------------------------------------------------------------------------------
//  Rvector3& operator=(const Rvector3 &v)
//------------------------------------------------------------------------------
Rvector3& Rvector3::operator=(const Rvector3 &v)
{
    elementD[0] = v.elementD[0];
    elementD[1] = v.elementD[1];
    elementD[2] = v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rvector3 &v) const
//------------------------------------------------------------------------------
bool Rvector3::operator==(const Rvector3 &v) const
{
   if (elementD[0] == v[0] &&
       elementD[1] == v[1] &&
       elementD[2] == v[2])
   {
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rvector3 &v) const
//------------------------------------------------------------------------------
bool Rvector3::operator!=(const Rvector3 &v) const
{
   return !(operator==(v));
}

//------------------------------------------------------------------------------
//  Rvector3 operator-() const                     // negation
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator-() const
{
    return Rvector3(-elementD[0], -elementD[1], -elementD[2]);
}

//------------------------------------------------------------------------------
//  Rvector3 operator+(const Rvector3& v) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator+(const Rvector3& v) const
{
    return Rvector3(elementD[0] + v.elementD[0],
                   elementD[1] + v.elementD[1],
                   elementD[2] + v.elementD[2]);
}

//------------------------------------------------------------------------------
//  const Rvector3&  operator+=(const Rvector3& v)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator+=(const Rvector3& v)
{
    elementD[0] += v.elementD[0];
    elementD[1] += v.elementD[1];
    elementD[2] += v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  const Rvector3 operator-(const Rvector3& v) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator-(const Rvector3& v) const
{
    return Rvector3(elementD[0] - v.elementD[0],
                    elementD[1] - v.elementD[1],
                    elementD[2] - v.elementD[2]);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator-=(const Rvector3& v)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator-=(const Rvector3& v)
{
    elementD[0] -= v.elementD[0];
    elementD[1] -= v.elementD[1];
    elementD[2] -= v.elementD[2];
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator*(Real s) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator*(Real s) const
{
    return Rvector3(elementD[0]*s, elementD[1]*s, elementD[2]*s);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator*=(Real s)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator*=(Real s)
{
    elementD[0] *= s;
    elementD[1] *= s;
    elementD[2] *= s;
    return *this;
}

//------------------------------------------------------------------------------
//  Real operator*(const Rvector3& v) const        // dot product
//------------------------------------------------------------------------------
Real Rvector3::operator*(const Rvector3& v) const 
{
    return (elementD[0] * v.elementD[0] +
            elementD[1] * v.elementD[1] +
            elementD[2] * v.elementD[2]);
}

//------------------------------------------------------------------------------
//  Rvector3 operator/(Real s) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator/(Real s) const
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    return Rvector3(elementD[0]/s, elementD[1]/s, elementD[2]/s);
}

//------------------------------------------------------------------------------
//  const Rvector3& operator/=(Real s)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator/=(Real s)
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    elementD[0] /= s;
    elementD[1] /= s;
    elementD[2] /= s;
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator*(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator*(const Rmatrix33& m) const
{
    return Rvector3(elementD[0]*m(0,0) + elementD[1]*m(0,1) + elementD[2]*m(0,2),
           elementD[0]*m(1,0) + elementD[1]*m(1,1) + elementD[2]*m(1,2),
           elementD[0]*m(2,0) + elementD[1]*m(2,1) + elementD[2]*m(2,2));
}

//------------------------------------------------------------------------------
//  const Rvector3& operator*=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator*=(const Rmatrix33& m) 
{
    *this = *this * m;
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector3 operator/(const Rmatrix33& m) const
//------------------------------------------------------------------------------
Rvector3 Rvector3::operator/(const Rmatrix33& m) const
{
    Rmatrix33 invM;
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    return (*this) * invM;
}

//------------------------------------------------------------------------------
//  const Rvector3& operator/=(const Rmatrix33& m)
//------------------------------------------------------------------------------
const Rvector3& Rvector3::operator/=(const Rmatrix33& m)
{
    Rmatrix33 invM;
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    *this *= invM;
    return *this;
}

//------------------------------------------------------------------------------
//  <friend>
//  Rvector3 operator*(Real s, const Rvector3& v)
//------------------------------------------------------------------------------
Rvector3 operator*(Real s, const Rvector3& v)
{
    return Rvector3(s * v.elementD[0],
                   s * v.elementD[1],
                   s * v.elementD[2]);
}

//------------------------------------------------------------------------------
//  <friend>
//  Rvector3 Cross(const Rvector3 &v1, const Rvector3 &v2)
//------------------------------------------------------------------------------
Rvector3 Cross(const Rvector3 &v1, const Rvector3 &v2)
{
    return Rvector3(
       v1.elementD[1]*v2.elementD[2] - v1.elementD[2]*v2.elementD[1],
       v1.elementD[2]*v2.elementD[0] - v1.elementD[0]*v2.elementD[2],
       v1.elementD[0]*v2.elementD[1] - v1.elementD[1]*v2.elementD[0]);
}

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix33 Outerproduct(const Rvector3 &v1, const Rvector3 &v2)
//------------------------------------------------------------------------------
Rmatrix33 Outerproduct(const Rvector3 &v1, const Rvector3 &v2)
{
    return Rmatrix33(v1.elementD[0] * v2.elementD[0],
                    v1.elementD[0] * v2.elementD[1],
                    v1.elementD[0] * v2.elementD[2],
                    v1.elementD[1] * v2.elementD[0],
                    v1.elementD[1] * v2.elementD[1],
                    v1.elementD[1] * v2.elementD[2],
                    v1.elementD[2] * v2.elementD[0],
                    v1.elementD[2] * v2.elementD[1],
                    v1.elementD[2] * v2.elementD[2]);
}

//------------------------------------------------------------------------------
// static Real Rvector3::Normalize(const Real from[3], Real to[3])
//------------------------------------------------------------------------------
Real Rvector3::Normalize(const Real from[3], Real to[3])
{
   Real mag = GmatMathUtil::Sqrt(from[0]*from[0] + from[1]*from[1] + from[2]*from[2]);
   to[0] = from[0]/mag;
   to[1] = from[1]/mag;
   to[2] = from[2]/mag;
   return mag;
}


//------------------------------------------------------------------------------
// static void Rvector3::Copy(const Real from[3], Real to[3])
//------------------------------------------------------------------------------
void Rvector3::Copy(const Real from[3], Real to[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Rvector3::GetNumData() const
{
   return NUM_DATA;
}


//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Rvector3::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}


//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rvector3 &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rvector3 &a)
{
   return GmatRealUtil::operator>> (input, a);
}


//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rvector3 &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rvector3 &a)
{
   return GmatRealUtil::operator<< (output, a);
}
