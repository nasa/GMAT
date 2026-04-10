//$Id$
//------------------------------------------------------------------------------
//                                  Rvec6Var
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
 * Defines base class of Rvector6 parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Rvec6Var.hpp"
#include "ParameterException.hpp"
#include <sstream>


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rvec6Var::Rvec6Var(const std::string &name, const std::string &typeStr,
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
Rvec6Var::Rvec6Var(const std::string &name, const std::string &typeStr,
                   GmatParam::ParameterKey key, GmatBase *obj,
                   const std::string &desc, const std::string &unit,
                   GmatParam::DepObject depObj, UnsignedInt ownerType)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, false,
               false, false, false)
{
   mRvec6Value = Rvector6::RVECTOR6_UNDEFINED;
   mReturnType = Gmat::RVECTOR_TYPE;
}


//------------------------------------------------------------------------------
// Rvec6Var(const Rvec6Var &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Rvec6Var::Rvec6Var(const Rvec6Var &copy)
   : Parameter(copy)
{
   mRvec6Value = copy.mRvec6Value;
}

//------------------------------------------------------------------------------
// Rvec6Var& operator= (const Rvec6Var& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Rvec6Var& Rvec6Var::operator= (const Rvec6Var& right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mRvec6Value = right.mRvec6Value;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~Rvec6Var()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Rvec6Var::~Rvec6Var()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Rvec6Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvec6Var::operator==(const Rvec6Var &right) const
{
   return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Rvec6Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Rvec6Var::operator!=(const Rvec6Var &right) const
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
std::string Rvec6Var::ToString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   return mRvec6Value.ToString();
}

//------------------------------------------------------------------------------
// const Rvector6& GetRvector6() const
//------------------------------------------------------------------------------
/**
 * @return Rvector6 value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvec6Var::GetRvector6() const
{
   return mRvec6Value;
}

//------------------------------------------------------------------------------
// virtual void SetRvector6(const Rvector6 &val)
//------------------------------------------------------------------------------
void Rvec6Var::SetRvector6(const Rvector6 &val)
{
   mRvec6Value = val;
}

//------------------------------------------------------------------------------
// const Rvector6& EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
//------------------------------------------------------------------------------
const Rvector6& Rvec6Var::EvaluateRvector6()
{
   if (mKey == GmatParam::SYSTEM_PARAM)
   {
      throw ParameterException("Parameter: EvaluateRvector6() should be implemented "
                               "for Parameter Type:" + GetTypeName());
   }
   else
   {
      return mRvec6Value;
   }
}

