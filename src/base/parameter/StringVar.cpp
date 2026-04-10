//$Id$
//------------------------------------------------------------------------------
//                                  StringVar
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
// Created: 2005/1/7
//
/**
 * Implements StringVar class which handles std::string value. The string value
 * is stored in Parameter::mExpr.
 */
//------------------------------------------------------------------------------

#include "StringVar.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_STRINGVAR
//#define DEBUG_GEN_STRING

//---------------------------------
// static data
//---------------------------------

const std::string
StringVar::PARAMETER_TEXT[StringVarParamCount - ParameterParamCount] =
{
   "Value",
};

const Gmat::ParameterType
StringVar::PARAMETER_TYPE[StringVarParamCount - ParameterParamCount] =
{
   Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// StringVar(const std::string &name, const std::string &typeStr, 
//         GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
//         const std::string &unit, GmatParam::DepObject depObj, UnsignedInt,
//         bool isTimeParam, bool isSettable)
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
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <ownerType> object type who owns this parameter as property
 * @param <isTimeParam> true if parameter is time related, false otherwise
 * @param <isSettable> true if parameter is settable, false otherwise
 * @param <ownedObjType> Type of the object which owned by owner or attached to owner
 */
//------------------------------------------------------------------------------
StringVar::StringVar(const std::string &name, const std::string &typeStr, 
                     GmatParam::ParameterKey key, GmatBase *obj, const std::string &desc,
                     const std::string &unit, GmatParam::DepObject depObj,
                     UnsignedInt ownerType, bool isTimeParam, bool isSettable,
                     UnsignedInt ownedObjType)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, isTimeParam,
               isSettable, false, true, ownedObjType)
{  
   objectTypes.push_back(Gmat::STRING);
   objectTypeNames.push_back("String");
   blockCommandModeAssignment = false;
   mStringValue = ""; //       STRING_PARAMETER_UNDEFINED;
   mReturnType = Gmat::STRING_TYPE;
   // Don't set name to expression, but leave it blank if not set (LOJ: 2010.11.29)
   //mExpr = name;
}


//------------------------------------------------------------------------------
// StringVar(const StringVar &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
StringVar::StringVar(const StringVar &copy)
   : Parameter(copy)
{
   mStringValue = copy.mStringValue;   
}


//------------------------------------------------------------------------------
// StringVar& operator= (const StringVar& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
StringVar& StringVar::operator= (const StringVar& right)
{
   if (this != &right)
   {
      // We don't want to change the name when copy
      std::string thisName = instanceName;
      
      Parameter::operator=(right);
      mStringValue = right.mStringValue;
      // Set expression so that we can preserve string value when we write (loj: 2008.08.13)
      // Set expression to name of right side since expression is used for
      // writing in GetGeneratingString() (loj: 2008.08.13)
      // For example:
      // str1 = 'this is str1'
      // str2 = str1;
      // We want to write "str2 = str1" instead of "str2 = 'this is str1'
      mExpr = right.GetName();
      // Set depObjectName so that we can check whether to add quotes when we write (loj: 2008.08.13)
      mDepObjectName = right.GetName();
      SetName(thisName);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~StringVar()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StringVar::~StringVar()
{
}


//------------------------------------
// methods inherited from Parameter
//------------------------------------

//------------------------------------------------------------------------------
// bool operator==(const StringVar &right) const
//------------------------------------------------------------------------------
/**
 * Equal operator.
 *
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool StringVar::operator==(const StringVar &right) const
{
   return Parameter::operator==(right);
}


//------------------------------------------------------------------------------
// bool operator!=(const StringVar &right) const
//------------------------------------------------------------------------------
/**
 * Not equal operator.
 *
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool StringVar::operator!=(const StringVar &right) const
{
   return Parameter::operator!=(right);
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value of parameter.
 */
//------------------------------------------------------------------------------
std::string StringVar::ToString()
{
   Evaluate();
   return mStringValue;
}


//------------------------------------------------------------------------------
// const std::string& GetString() const
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value.
 */
//------------------------------------------------------------------------------
const std::string& StringVar::GetString() const
{
   return mStringValue;
}


//------------------------------------------------------------------------------
// void SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Sets string value of parameter.
 */
//------------------------------------------------------------------------------
void StringVar::SetString(const std::string &val)
{
   mStringValue = val;
}


//------------------------------------------------------------------------------
// const std::string& EvaluateString()
//------------------------------------------------------------------------------
/**
 * Retrieves string value of parameter.
 *
 * @return string value.
 */
//------------------------------------------------------------------------------
const std::string& StringVar::EvaluateString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   return mStringValue;
}


//------------------------------------
// methods inherited from GmatBase
//------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object.
 *
 * @return cloned object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* StringVar::Clone() const
{
   return new StringVar(*this);
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void StringVar::Copy(const GmatBase* orig)
{
   operator=(*((StringVar *)(orig)));
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer StringVar::GetParameterID(const std::string &str) const
{
   for (int i=ParameterParamCount; i<StringVarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ParameterParamCount])
         return i;
   }
   
   return Parameter::GetParameterID(str);
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
bool StringVar::IsParameterCommandModeSettable(const Integer id) const
{
   // For now, turn them all on
   return true;
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string StringVar::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case VALUE:
      #ifdef DEBUG_STRINGVAR
      MessageInterface::ShowMessage
         ("StringVar::GetStringParameter(%d) returning '%s'\n", id,
          mStringValue.c_str());
      #endif
      return mStringValue;
   default:
      return Parameter::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string StringVar::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool StringVar::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_STRINGVAR
   MessageInterface::ShowMessage
      ("StringVar::SetStringParameter() this=<%p>, id=%d, value='%s'\n", this,
       id, value.c_str());
   #endif
   
   switch (id)
   {
   case EXPRESSION:
      mExpr = value;
      mStringValue = value; // set as initial value
      #ifdef DEBUG_STRINGVAR
      MessageInterface::ShowMessage
         ("StringVar::SetStringParameter() returning true, "
          "both mExpr and mStringValue are set to '%s'\n", value.c_str());
      #endif
      return true;
   case VALUE:
      mStringValue = value;
      #ifdef DEBUG_STRINGVAR
      MessageInterface::ShowMessage
         ("StringVar::SetStringParameter() returning true, "
          "mStringValue is set to '%s'\n", value.c_str());
      #endif
      return true;
   default:
      return Parameter::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool StringVar::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   #ifdef DEBUG_STRINGVAR
   MessageInterface::ShowMessage
      ("StringVar::SetStringParameter() label=%s value='%s'\n",
       label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(...)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 * 
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 * 
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& StringVar::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("StringVar::GetGeneratingString() this=<%p>'%s' entered, mode=%d, prefix='%s', "
       "useName='%s'\n", this, GetName().c_str(), mode, prefix.c_str(), useName.c_str());
   MessageInterface::ShowMessage
      ("   mExpr='%s', mDepObjectName='%s'\n", mExpr.c_str(), mDepObjectName.c_str());
   #endif
   
   // @note
   // Do not write "Create name" since multiple Strings per line will be written from
   // the ScriptInterpreter
   
   // Write value if it is not blank or blank and SHOW_SCRIPT mode
   if ( mExpr != "" ||
       (mExpr == "" && mode == Gmat::SHOW_SCRIPT))
   {
      // if value is other StringVar object, do not put quotes
      if (mExpr != "" && mExpr == mDepObjectName)
         generatingString = GetName() + " = " + mExpr;
      else
         generatingString = GetName() + " = '" + mExpr + "'";
      
      bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
      if(writeGmatKeyword)
        generatingString = "GMAT " + generatingString;

      if (mode == Gmat::NO_COMMENTS)
         generatingString = generatingString + ";\n";
      else
         generatingString = generatingString + ";" + inlineComment + "\n";
   }
   else
   {
      generatingString = "";
   }
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("StringVar::GetGeneratingString() returning\n   <%s>\n", generatingString.c_str());
   #endif
   
   return generatingString;
}

