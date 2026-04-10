//$Id$
//------------------------------------------------------------------------------
//                                Rvector6
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
// Author: L. Jun
// Created: 2003/09/15
//
/**
 * Provides definitions for the Rvector6 class, providing linear algebra
 * operations for the 6-element Real vector
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "utildefs.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//---------------------------------
//  static data
//---------------------------------
const Real Rvector6::UTIL_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED;
const Integer Rvector6::NUM_DATA = NUM_DATA_INIT;

const Rvector6
Rvector6::RVECTOR6_UNDEFINED = Rvector6(UTIL_REAL_UNDEFINED, UTIL_REAL_UNDEFINED,
                                        UTIL_REAL_UNDEFINED, UTIL_REAL_UNDEFINED,
                                        UTIL_REAL_UNDEFINED, UTIL_REAL_UNDEFINED);

const std::string Rvector6::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Element 1", "Element 2", "Element 3",
   "Element 4", "Element 5", "Element 6"
};

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rvector6()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 * Initializes elements to 0.0.
 */
//------------------------------------------------------------------------------
Rvector6::Rvector6()
   : Rvector(6)
{
}

//------------------------------------------------------------------------------
// Rvector6::Rvector6(const Real e1, const Real e2, const Real e3,
//                    const Real e4, const Real e5, const Real e6)
//------------------------------------------------------------------------------
/**
 * Constructor.
 * Creates an object from 6 Real elements.
 */
//------------------------------------------------------------------------------
Rvector6::Rvector6(const Real e1, const Real e2, const Real e3,
                   const Real e4, const Real e5, const Real e6)
   : Rvector(6, e1, e2, e3, e4, e5, e6) 
{
}

//------------------------------------------------------------------------------
//  Rvector6(const Rvector3 &r, const Rvector3 &v)
//------------------------------------------------------------------------------
/**
 * Constructor.
 * Creates an object from two Rvector3 object.
 */
//------------------------------------------------------------------------------
Rvector6::Rvector6(const Rvector3 &r, const Rvector3 &v)
   : Rvector(6, r[0], r[1], r[2], v[0], v[1], v[2])
{
}

//loj: 4/20/04 added
//------------------------------------------------------------------------------
//  Rvector6(const Real vec[6])
//------------------------------------------------------------------------------
/**
 * Constructor.
 * Creates an object from Real array
 */
//------------------------------------------------------------------------------
Rvector6::Rvector6(const Real vec[6])
   : Rvector(6, vec[0], vec[1], vec[2], vec[3], vec[4], vec[5])
{
}

//------------------------------------------------------------------------------
//  Rvector6(const Rvector6 &v)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
Rvector6::Rvector6(const Rvector6 &v)
   : Rvector(v)
{
}

//------------------------------------------------------------------------------
//  Rvector6(const RealArray &ra)
//------------------------------------------------------------------------------
Rvector6::Rvector6(const RealArray &ra)
: Rvector(ra)
{
   // This check must be made here or the resulting Rvector6 may not
   // be of size 6!!
   if (ra.size() != 6)
      throw ArrayTemplateExceptions::DimensionError();
}


//------------------------------------------------------------------------------
//  Rvector6& operator=(const Rvector6 &v)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
Rvector6& Rvector6::operator=(const Rvector6 &v)
{
    elementD[0] = v.elementD[0];
    elementD[1] = v.elementD[1];
    elementD[2] = v.elementD[2];
    elementD[3] = v.elementD[3];
    elementD[4] = v.elementD[4];
    elementD[5] = v.elementD[5];
    return *this;
}

//------------------------------------------------------------------------------
// Rvector6* Clone() const
//------------------------------------------------------------------------------
Rvector6* Rvector6::Clone() const
{
   return (new Rvector6(*this));
}

//------------------------------------------------------------------------------
//  ~Rvector6()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Rvector6::~Rvector6()
{
}

//------------------------------------------------------------------------------
// Real Get(const Integer index) const
//------------------------------------------------------------------------------
Real Rvector6::Get(const Integer index) const
{
   return elementD[index];
}

//------------------------------------------------------------------------------
// Rvector3 GetR() const
//------------------------------------------------------------------------------
/**
 * @return Rvector3 object created from first three elements.
 */
//------------------------------------------------------------------------------
Rvector3 Rvector6::GetR() const
{
   return Rvector3(elementD[0], elementD[1],elementD[2]);
}

//------------------------------------------------------------------------------
// Rvector3 GetV() const
//------------------------------------------------------------------------------
/**
 * @return Rvector3 object created from last three elements.
 */
//------------------------------------------------------------------------------
Rvector3 Rvector6::GetV() const
{
   return Rvector3(elementD[3], elementD[4],elementD[5]);
}

//------------------------------------------------------------------------------
// void GetR(Real *r)
//------------------------------------------------------------------------------
void Rvector6::GetR(Real *r) const
{
   r[0] = elementD[0];
   r[1] = elementD[1];
   r[2] = elementD[2];
}

//------------------------------------------------------------------------------
// void GetV(Real *v)
//------------------------------------------------------------------------------
void Rvector6::GetV(Real *v) const
{
   v[0] = elementD[3];
   v[1] = elementD[4];
   v[2] = elementD[5];
}

//------------------------------------------------------------------------------
// void Set(const Real e1, const Real e2, const Real e3,
//          const Real e4, const Real e5, const Real e6)
//------------------------------------------------------------------------------
void Rvector6::Set(const Real e1, const Real e2, const Real e3,
                   const Real e4, const Real e5, const Real e6)
{
   elementD[0] = e1;
   elementD[1] = e2;
   elementD[2] = e3;
   elementD[3] = e4;
   elementD[4] = e5;
   elementD[5] = e6;
}

//------------------------------------------------------------------------------
// void Set(const Real v[6]) 
//------------------------------------------------------------------------------
void Rvector6::Set(const Real v[6])
{
   for (UnsignedInt i=0; i < 6; i++)
      elementD[i] = v[i];
}

//------------------------------------------------------------------------------
// void SetR(const Rvector3 &v)
//------------------------------------------------------------------------------
/**
 * Sets first three elements.
 */
//------------------------------------------------------------------------------
void Rvector6::SetR(const Rvector3 &v)
{
   elementD[0] = v.Get(0);
   elementD[1] = v.Get(1);
   elementD[2] = v.Get(2);
}

//------------------------------------------------------------------------------
// void SetV(const Rvector3 &v)
//------------------------------------------------------------------------------
/**
 * Sets last three elements.
 */
//------------------------------------------------------------------------------
void Rvector6::SetV(const Rvector3 &v)
{
   elementD[3] = v.Get(3);
   elementD[4] = v.Get(4);
   elementD[5] = v.Get(5);
}

//------------------------------------------------------------------------------
//  bool operator==(const Rvector6 &v) const
//------------------------------------------------------------------------------
/**
 * @return true if all 6 elements of v are equal to this object.
 */
//------------------------------------------------------------------------------
bool Rvector6::operator==(const Rvector6 &v) const
{
   if (elementD[0] == v[0] &&
       elementD[1] == v[1] &&
       elementD[2] == v[2] &&
       elementD[3] == v[3] &&
       elementD[4] == v[4] &&
       elementD[5] == v[5])
   {
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rvector6 &v) const
//------------------------------------------------------------------------------
/**
 * @return true if all 6 elements of v are not equal to this object.
 */
//------------------------------------------------------------------------------
bool Rvector6::operator!=(const Rvector6 &v) const
{
   return !(operator==(v));
}

//------------------------------------------------------------------------------
//  Rvector6 operator-() const
//------------------------------------------------------------------------------
/**
 * Negates all elements and return a new object.
 */
//------------------------------------------------------------------------------
Rvector6 Rvector6::operator-() const
{
    return Rvector6(-elementD[0], -elementD[1], -elementD[2],
                    -elementD[3], -elementD[4], -elementD[5]);
}

//------------------------------------------------------------------------------
//  Rvector6 operator+(const Rvector6 &v) const
//------------------------------------------------------------------------------
/**
 * Adds an object and return a new object.
 */
//------------------------------------------------------------------------------
Rvector6 Rvector6::operator+(const Rvector6 &v) const
{
    return Rvector6(elementD[0] + v.elementD[0],
                    elementD[1] + v.elementD[1],
                    elementD[2] + v.elementD[2],
                    elementD[3] + v.elementD[3],
                    elementD[4] + v.elementD[4],
                    elementD[5] + v.elementD[5]);
}

//------------------------------------------------------------------------------
//  const Rvector6&  operator+=(const Rvector6 &v)
//------------------------------------------------------------------------------
/**
 * Adds an object and return the same object.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvector6::operator+=(const Rvector6 &v)
{
    elementD[0] += v.elementD[0];
    elementD[1] += v.elementD[1];
    elementD[2] += v.elementD[2];
    elementD[3] += v.elementD[3];
    elementD[4] += v.elementD[4];
    elementD[5] += v.elementD[5];
    return *this;
}

//------------------------------------------------------------------------------
//  const Rvector6 operator-(const Rvector6 &v) const
//------------------------------------------------------------------------------
/**
 * Subtracts a object and return a new object.
 *
 * @return a new object.
 */
//------------------------------------------------------------------------------
Rvector6 Rvector6::operator-(const Rvector6 &v) const
{
    return Rvector6(elementD[0] - v.elementD[0],
                    elementD[1] - v.elementD[1],
                    elementD[2] - v.elementD[2],
                    elementD[3] - v.elementD[3],
                    elementD[4] - v.elementD[4],
                    elementD[5] - v.elementD[5]);
}

//------------------------------------------------------------------------------
//  const Rvector6& operator-=(const Rvector6 &v)
//------------------------------------------------------------------------------
/**
 * Subtracts an object and return the same object.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvector6::operator-=(const Rvector6 &v)
{
    elementD[0] -= v.elementD[0];
    elementD[1] -= v.elementD[1];
    elementD[2] -= v.elementD[2];
    elementD[3] -= v.elementD[3];
    elementD[4] -= v.elementD[4];
    elementD[5] -= v.elementD[5];
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector6 operator*(Real s) const
//------------------------------------------------------------------------------
/**
 * Multiplies a Real number and return a object.
 */
//------------------------------------------------------------------------------
Rvector6 Rvector6::operator*(Real s) const
{
    return Rvector6(elementD[0]*s, elementD[1]*s, elementD[2]*s,
                    elementD[3]*s, elementD[4]*s, elementD[5]*s);
}

//------------------------------------------------------------------------------
//  const Rvector6& operator*=(Real s)
//------------------------------------------------------------------------------
/**
 * Multiplies a Real number and return the same object.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvector6::operator*=(Real s)
{
    elementD[0] *= s;
    elementD[1] *= s;
    elementD[2] *= s;
    elementD[3] *= s;
    elementD[4] *= s;
    elementD[5] *= s;
    return *this;
}

//------------------------------------------------------------------------------
//  Real operator*(const Rvector6& v) const        // dot product
//------------------------------------------------------------------------------
Real Rvector6::operator*(const Rvector6& v) const 
{
   Real sum = 0.0;
   
   for (int i=0; i<sizeD; i++)
      sum += elementD[i] * v.elementD[i];
   
   return sum;
}

//------------------------------------------------------------------------------
//  Rvector6 operator/(Real s) const
//------------------------------------------------------------------------------
/**
 * Divides by a Real number and return a new object.
 *
 * @exception thrown when divider is zero.
 */
//------------------------------------------------------------------------------
Rvector6 Rvector6::operator/(Real s) const
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    return Rvector6(elementD[0]/s, elementD[1]/s, elementD[2]/s,
                    elementD[3]/s, elementD[4]/s, elementD[5]/s);
}

//------------------------------------------------------------------------------
//  const Rvector6& operator/=(Real s)
//------------------------------------------------------------------------------
/**
 * Divides by a Real number and return the same object.
 *
 * @exception thrown when divider is zero.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvector6::operator/=(Real s)
{
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    elementD[0] /= s;
    elementD[1] /= s;
    elementD[2] /= s;
    elementD[3] /= s;
    elementD[4] /= s;
    elementD[5] /= s;
    return *this;
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
/**
 * @return number of data elements.
 */
//------------------------------------------------------------------------------
Integer Rvector6::GetNumData() const
{
   return NUM_DATA;
}


//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
/**
 * @return data description pointer.
 */
//------------------------------------------------------------------------------
const std::string* Rvector6::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}


//------------------------------------------------------------------------------
// bool IsValid(const Real val)
//------------------------------------------------------------------------------
/**
 * @return true if all 6 elelmets are not equal to input value.
 */
//------------------------------------------------------------------------------
bool Rvector6::IsValid(const Real val)
{
   if (elementD[0] == val || elementD[1] == val || elementD[2] == val ||
       elementD[3] == val || elementD[4] == val || elementD[5] == val)
      return false;
   else
      return true;
}
