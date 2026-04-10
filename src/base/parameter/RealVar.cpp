//$Id$
//------------------------------------------------------------------------------
//                                  RealVar
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
// Created: 2004/01/08
//
/**
 * Defines base class of Real parameters.
 * All Parameters returning Real number are derived from this class.
 */
//------------------------------------------------------------------------------

#include "RealVar.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // for GmatStringUtil::ToReal()
#include "GmatGlobal.hpp"          // for Global settings
#include "MessageInterface.hpp"

//#define DEBUG_REAL_VAR 1

//---------------------------------
// static data
//---------------------------------
const std::string
RealVar::PARAMETER_TEXT[RealVarParamCount - ParameterParamCount] =
{
   "Value"
};

const Gmat::ParameterType
RealVar::PARAMETER_TYPE[RealVarParamCount - ParameterParamCount] =
{
   Gmat::REAL_TYPE
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RealVar(const std::string &name, const std::string &valStr, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name [""]
 * @param <valStr>  value string of parameter [""]
 * @param <typeStr>  parameter type string ["RealVar"]
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc) [GmatParam::USER_PARAM]
 * @param <obj>  reference object pointer [NULL]
 * @param <desc> parameter description [""]
 * @param <unit> parameter unit [""]
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE) [GmatParam::NO_DEP]
 * @param <ownerType> object type who owns this parameter as property [Gmat::UNKNOWN_OBJECT]
 * @param <isTimeParam> true if parameter is time related, false otherwise [false]
 * @param <isSettable> true if parameter is settable [false]
 * @param <isPlottable> true if parameter is plottable, false otherwise [true]
 * @param <isReportable> true if parameter is reportable, false otherwise [true]
 * @param <ownedObjType> Type of the object which owned by owner or attached to owner [Gmat::UNKNOWN_OBJECT]
 */
//------------------------------------------------------------------------------
RealVar::RealVar(const std::string &name, const std::string &valStr,
                 const std::string &typeStr, GmatParam::ParameterKey key,
                 GmatBase *obj, const std::string &desc, const std::string &unit,
                 GmatParam::DepObject depObj, UnsignedInt ownerType,
                 bool isTimeParam, bool isSettable, bool isPlottable,
                 bool isReportable,UnsignedInt ownedObjType)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, isTimeParam,
               isSettable, isPlottable, isReportable, ownedObjType)
{
   mValueSet = false;
   mIsNumber = true;
   mRealValue = 0.0;
   mExpr = "0";
   
   Real rval;
   if (GmatStringUtil::ToReal(valStr, &rval))
   {
      mRealValue = rval;
      mExpr = valStr;
   }
   else
   {
      mIsNumber = false;
   }
   
   parameterCount = RealVarParamCount;
   mReturnType = Gmat::REAL_TYPE;
}


//------------------------------------------------------------------------------
// RealVar(const RealVar &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
RealVar::RealVar(const RealVar &copy)
   : Parameter(copy)
{
   mValueSet = copy.mValueSet;
   mIsNumber = copy.mIsNumber;
   mRealValue = copy.mRealValue;
}


//------------------------------------------------------------------------------
// RealVar& operator= (const RealVar& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RealVar& RealVar::operator= (const RealVar& right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mValueSet = right.mValueSet;
      mIsNumber = right.mIsNumber;
      mRealValue = right.mRealValue;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// ~RealVar()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RealVar::~RealVar()
{
   //MessageInterface::ShowMessage("==> RealVar::~RealVar()\n");
}

//------------------------------------------------------------------------------
// bool operator==(const RealVar &right) const
//------------------------------------------------------------------------------
/**
 * Equal operator.
 *
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool RealVar::operator==(const RealVar &right) const
{
   return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const RealVar &right) const
//------------------------------------------------------------------------------
/**
 * Not equal operator.
 *
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool RealVar::operator!=(const RealVar &right) const
{
   return Parameter::operator!=(right);
}

//------------------------------------
// methods inherited from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool RealVar::Initialize()
{
   //mRealValue = REAL_PARAMETER_UNDEFINED;
   return true;
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * Converts parameter real value to string.
 *
 * @return parameter value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string RealVar::ToString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   // use default global precision to convert to string (loj: 2008.03.05)
   return GmatStringUtil::ToString(mRealValue, false, false, false,
                                   GmatGlobal::DATA_PRECISION, 1);
   
   //std::stringstream ss("");
   //ss.precision(10);
   //ss << mRealValue;
   //return std::string(ss.str());
}


//------------------------------------------------------------------------------
// Real GetReal() const
//------------------------------------------------------------------------------
/**
 * Retrives real value of parameter.
 *
 * @return Real value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
Real RealVar::GetReal() const
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::GetReal() name=%s, returning val=%.18f\n", GetName().c_str(), 
      mRealValue);
   #endif
   return mRealValue;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets real value of parameter.
 */
//------------------------------------------------------------------------------
void RealVar::SetReal(Real val)
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::SetReal(Real val) name=%s, mExpr='%s', mIsNumber=%d, val=%.18f\n",
       GetName().c_str(), mExpr.c_str(), mIsNumber, val);
   #endif
   
   mRealValue = val;
   
   if (mIsNumber)
   {
      // use default global precision to convert to string (loj: 2008.03.05)
      mExpr = GmatStringUtil::ToString(mRealValue, false, false, false,
                                       GmatGlobal::DATA_PRECISION, 1);
      
      #if DEBUG_REAL_VAR
      MessageInterface::ShowMessage
         ("RealVar::SetReal(Real val) mExpr=%s\n", mExpr.c_str());
      #endif
   }
}

//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer RealVar::GetParameterID(const std::string &str) const
{
   for (int i=ParameterParamCount; i<RealVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string RealVar::GetParameterTypeString(const Integer id) const
{
   if (id >= ParameterParamCount && id < RealVarParamCount)
      return Parameter::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Parameter::GetParameterTypeString(id);
}



//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType RealVar::GetParameterType(const Integer id) const
{
   if (id >= ParameterParamCount && id < RealVarParamCount)
      return PARAMETER_TYPE[id - ParameterParamCount];
   else
      return Parameter::GetParameterType(id);
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
bool RealVar::IsParameterReadOnly(const Integer id) const
{
   if (id == VALUE)
      return true;

   return Parameter::IsParameterReadOnly(id);
}




//-------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//-------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case VALUE:
      return mRealValue;
   default:
      return Parameter::GetRealParameter(id);
   }
}


//-------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//-------------------------------------------------------------------------------
Real RealVar::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//-------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//-------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case VALUE:
      mValueSet = true;
      SetReal(value);
      return value;
   default:
      return Parameter::GetRealParameter(id);
   }
}


//-------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//-------------------------------------------------------------------------------
Real RealVar::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage
      ("RealVar::SetStringParameter() name='%s', id=%d, value='%s'\n",
       GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case EXPRESSION:
      {
         mValueSet = true;
         // if value is blank, initialize expression to 0
         if (value != "")
         {
            // if value is just a number, convert and set to real value
            Real rval;
            if (GmatStringUtil::ToReal(value, &rval))
            {
               mRealValue = rval;
               
               #if DEBUG_REAL_VAR
               MessageInterface::ShowMessage("mRealValue set to %f\n", mRealValue);
               #endif
            }
            
            mExpr = value;
            return true;
         }
      }
   default:
      return Parameter::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool RealVar::SetStringParameter(const std::string &label,
                                 const std::string &value)
{
   #if DEBUG_REAL_VAR
   MessageInterface::ShowMessage("RealVar::SetStringParameter() label=%s value=%s\n",
                                 label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}

