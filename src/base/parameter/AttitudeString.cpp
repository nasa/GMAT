//$Id$
//------------------------------------------------------------------------------
//                                AttitudeString
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
// Created: 2006/03/09
//
/**
 * Implements AttitudeString class which provides base class for time realated String
 * Parameters
 */
//------------------------------------------------------------------------------

#include "AttitudeString.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"               // for ParseParameter()
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// AttitudeString(const std::string &name, const std::string &typeStr, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <isSettable> true if parameter is settable
 */
//------------------------------------------------------------------------------
AttitudeString::AttitudeString(const std::string &name, const std::string &typeStr, 
                               GmatBase *obj, const std::string &desc,
                               const std::string &unit, bool isSettable)
   : StringVar(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
               GmatParam::OWNED_OBJ, Gmat::SPACECRAFT, false, isSettable,
               Gmat::ATTITUDE),
     AttitudeData(name)
{
   std::string type, ownerName, depObj;
   GmatStringUtil::ParseParameter(name, type, ownerName, depObj);
   mOwnerName = ownerName;
   mExpr = name;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// AttitudeString(const AttitudeString &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
AttitudeString::AttitudeString(const AttitudeString &copy)
   : StringVar(copy), AttitudeData(copy)
{
}


//------------------------------------------------------------------------------
// AttitudeString& operator=(const AttitudeString &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
AttitudeString& AttitudeString::operator=(const AttitudeString &right)
{
   if (this != &right)
   {
      StringVar::operator=(right);
      AttitudeData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeString()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
AttitudeString::~AttitudeString()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// const virtual std::string& EvaluateString()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
const std::string& AttitudeString::EvaluateString()
{
   Evaluate();
   return mStringValue;
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer AttitudeString::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *obj, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool AttitudeString::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return AttitudeData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                    replaceName);
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool AttitudeString::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool AttitudeString::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch (BaseException &e)
   {
      #if DEBUG_TIMESTRING
      MessageInterface::ShowMessage
         ("AttitudeString::Initialize() Fail to initialize Parameter '%s'\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("WARNING:  " + e.GetFullMessage() + " in " + GetName() + "\n");
   }
   
   return true;
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool AttitudeString::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls AttitudeData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string AttitudeString::GetRefObjectName(const UnsignedInt type) const
{
   return AttitudeData::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray& AttitudeString::GetRefObjectNameArray(const UnsignedInt type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const UnsignedInt type,
//                               const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference object name to given object type.
 *
 * @param <type> object type
 * @param <name> object name
 *
 */
//------------------------------------------------------------------------------
bool AttitudeString::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   return AttitudeData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls AttitudeData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* AttitudeString::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   return AttitudeData::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls AttitudeData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool AttitudeString::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   return AttitudeData::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(...)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const std::string& AttitudeString::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("AttitudeString::GetGeneratingString() this=<%p>'%s' entered, mode=%d, prefix='%s', "
       "useName='%s'\n", this, GetName().c_str(), mode, prefix.c_str(), useName.c_str());
   MessageInterface::ShowMessage
      ("   mExpr='%s', mDepObjectName='%s'\n", mExpr.c_str(), mDepObjectName.c_str());
   #endif

   // We want to skip StringVar::GetGeneratingString() since it is handled specially
   // for String
   return Parameter::GetGeneratingString(mode, prefix, useName);
}

