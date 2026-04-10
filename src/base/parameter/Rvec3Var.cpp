//$Id$
//------------------------------------------------------------------------------
//                                  Rvec3Var
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
// Created: 2004/01/09
//
/**
 * Defines base class of Rvector3 parameters.
 */
 //------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Rvec3Var.hpp"
#include "ParameterException.hpp"
#include <sstream>


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rvec3Var::Rvec3Var(const std::string &name, const std::string &typeStr,
//                    GmatParam::ParameterKey key, GmatBase *obj,
//                    const std::string &desc, const std::string &unit,
//                    GmatParam::DepObject depObj, UnsignedInt ownerType)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <typeStr>  parameter type string
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc)
 * @param <obj>  reference object pointer
 * @param <desc> parameter description
 * @param <unit> parameter unit
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
 //------------------------------------------------------------------------------
Rvec3Var::Rvec3Var(const std::string &name, const std::string &typeStr,
   GmatParam::ParameterKey key, GmatBase *obj,
   const std::string &desc, const std::string &unit,
   GmatParam::DepObject depObj, UnsignedInt ownerType)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, false,
      false, false, true)
{
   mRvec3Value = Rvector3::RVECTOR3_UNDEFINED;
   mReturnType = Gmat::RVECTOR_TYPE;
}


//------------------------------------------------------------------------------
// Rvec3Var(const Rvec3Var &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
 //------------------------------------------------------------------------------
Rvec3Var::Rvec3Var(const Rvec3Var &copy)
   : Parameter(copy)
{
   mRvec3Value = copy.mRvec3Value;
}

//------------------------------------------------------------------------------
// Rvec3Var& operator= (const Rvec3Var& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
 //------------------------------------------------------------------------------
Rvec3Var& Rvec3Var::operator= (const Rvec3Var& right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mRvec3Value = right.mRvec3Value;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~Rvec3Var()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
Rvec3Var::~Rvec3Var()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Rvec3Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
 //------------------------------------------------------------------------------
bool Rvec3Var::operator==(const Rvec3Var &right) const
{
   return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Rvec3Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
 //------------------------------------------------------------------------------
bool Rvec3Var::operator!=(const Rvec3Var &right) const
{
   return Parameter::operator!=(right);
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
 //------------------------------------------------------------------------------
std::string Rvec3Var::ToString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   return mRvec3Value.ToString();
}

//------------------------------------------------------------------------------
// const Rvector3& GetRvector3() const
//------------------------------------------------------------------------------
/**
 * @return Rvector3 value of parameter without evaluating.
 */
 //------------------------------------------------------------------------------
const Rvector3& Rvec3Var::GetRvector3() const
{
   return mRvec3Value;
}

//------------------------------------------------------------------------------
// virtual void SetRvector3(const Rvector3 &val)
//------------------------------------------------------------------------------
void Rvec3Var::SetRvector3(const Rvector3 &val)
{
   mRvec3Value = val;
}

//------------------------------------------------------------------------------
// const Rvector3& EvaluateRvector3()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
 //------------------------------------------------------------------------------
const Rvector3& Rvec3Var::EvaluateRvector3()
{
   if (mKey == GmatParam::SYSTEM_PARAM)
   {
      throw ParameterException("Parameter: EvaluateRvector3() should be implemented "
         "for Parameter Type:" + GetTypeName());
   }
   else
   {
      return mRvec3Value;
   }
}

