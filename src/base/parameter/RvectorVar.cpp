//$Id$
//------------------------------------------------------------------------------
//                                  RvectorVar
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
// Created: 2014/02/10
//
/**
 * Defines base class of Rvector parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "RvectorVar.hpp"
#include "ParameterException.hpp"
#include <sstream>

//---------------------------------
// static data
//---------------------------------
const std::string
RvectorVar::PARAMETER_TEXT[RvectorVarParamCount - ParameterParamCount] =
{
   "VectorSize",
};

const Gmat::ParameterType
RvectorVar::PARAMETER_TYPE[RvectorVarParamCount - ParameterParamCount] =
{
   Gmat::INTEGER_TYPE,
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RvectorVar::RvectorVar(const std::string &name, const std::string &typeStr, ...)
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
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE) [GmatParam::NO_DEP]
 * @param <ownerType> object type who owns this parameter as property [Gmat::UNKNOWN_OBJECT]
 * @param <size> Size of the vector
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
RvectorVar::RvectorVar(const std::string &name, const std::string &typeStr,
                       GmatParam::ParameterKey key, GmatBase *obj,
                       const std::string &desc, const std::string &unit,
                       GmatParam::DepObject depObj, UnsignedInt ownerType,
                       bool isTimeParam, bool isSettable, UnsignedInt ownedObjType,
                       Integer size)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, false, isSettable, 
               false, true, ownedObjType)
{
   mVectorSize = size;
   mRvectorValue.SetSize(size);
   mReturnType = Gmat::RVECTOR_TYPE;
}


//------------------------------------------------------------------------------
// RvectorVar(const RvectorVar &param)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <param> the object being copied.
 */
//------------------------------------------------------------------------------
RvectorVar::RvectorVar(const RvectorVar &param)
   : Parameter(param)
{
   mVectorSize = param.mVectorSize;
   mRvectorValue = param.mRvectorValue;
}

//------------------------------------------------------------------------------
// RvectorVar& operator= (const RvectorVar& param)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <param> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RvectorVar& RvectorVar::operator= (const RvectorVar& param)
{
   if (this != &param)
   {
      Parameter::operator=(param);
      mVectorSize = param.mVectorSize;
      mRvectorValue = param.mRvectorValue;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~RvectorVar()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RvectorVar::~RvectorVar()
{
}

//------------------------------------------------------------------------------
// bool operator==(const RvectorVar &param) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool RvectorVar::operator==(const RvectorVar &param) const
{
   return Parameter::operator==(param);
}

//------------------------------------------------------------------------------
// bool operator!=(const RvectorVar &param) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool RvectorVar::operator!=(const RvectorVar &param) const
{
   return Parameter::operator!=(param);
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
std::string RvectorVar::ToString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   return mRvectorValue.ToString();
}

//------------------------------------------------------------------------------
// const Rvector& GetRvector() const
//------------------------------------------------------------------------------
/**
 * @return Rvector value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
const Rvector& RvectorVar::GetRvector() const
{
   return mRvectorValue;
}

//------------------------------------------------------------------------------
// virtual void SetRvector(const Rvector &val)
//------------------------------------------------------------------------------
void RvectorVar::SetRvector(const Rvector &val)
{
   // Check for the size
   if (val.GetSize() != mVectorSize)
   {
      ParameterException pe;
      pe.SetDetails("Cannot set Rvector value to the Parameter \"%s\" due to "
                    "different size. It has %d elements, expecting %d elements\n",
                    GetName().c_str(), val.GetSize(), mVectorSize);
      throw pe;
   }
   
   mRvectorValue = val;
}

//------------------------------------------------------------------------------
// const Rvector& EvaluateRvector()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
//------------------------------------------------------------------------------
const Rvector& RvectorVar::EvaluateRvector()
{
   if (mKey == GmatParam::SYSTEM_PARAM)
   {
      throw ParameterException("Parameter: EvaluateRvector() should be implemented "
                               "for Parameter Type:" + GetTypeName());
   }
   else
   {
      // Check for 0 size or unsized rvector
      if (mVectorSize == 0 || !mRvectorValue.IsSized())
      {
         ParameterException pe;
         pe.SetDetails("Cannot evalulate the Parameter \"%s\". It has zero size or "
                       "size has not been set\n", GetName().c_str());
         throw pe;
      }
      
      return mRvectorValue;
   }
}

//---------------------------------
// Methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer RvectorVar::GetParameterID(const std::string &str) const
{
   for (int i = ParameterParamCount; i < RvectorVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string RvectorVar::GetParameterText(const Integer id) const
{
   if (id >= ParameterParamCount && id < RvectorVarParamCount)
      return PARAMETER_TEXT[id - ParameterParamCount];
   else
      return Parameter::GetParameterText(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool RvectorVar::IsParameterReadOnly(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return true;
   
   return Parameter::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool RvectorVar::IsParameterCommandModeSettable(const Integer id) const
{
   // For now, turn them all on
   return true;
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer RvectorVar::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case VECTOR_SIZE:
      return mVectorSize;
   default:
      return Parameter::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer RvectorVar::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


