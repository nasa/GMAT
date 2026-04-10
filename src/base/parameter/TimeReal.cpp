//$Id$
//------------------------------------------------------------------------------
//                                TimeReal
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
// Created: 2004/04/28
//
/**
 * Implements TimeReal class which provides base class for time realated Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "TimeReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TIMEREAL 1
//#define DEBUG_REF_OBJECT 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// TimeReal(const std::string &name, const std::string &typeStr, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the Parameter
 * @param <typeStr> type of the Parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the Parameter
 * @param <unit> unit of the Parameter
 * @param <isSettable> true if this is settable Parameter
 */
//------------------------------------------------------------------------------
TimeReal::TimeReal(const std::string &name, const std::string &typeStr, 
                   GmatBase *obj, const std::string &desc,
                   const std::string &unit, bool isSettable,
                   UnsignedInt paramOwnerType)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
         GmatParam::NO_DEP, paramOwnerType, true, isSettable),
     TimeData(name, typeStr, paramOwnerType)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// TimeReal(const TimeReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TimeReal::TimeReal(const TimeReal &copy)
   : RealVar(copy), TimeData(copy)
{
}


//------------------------------------------------------------------------------
// TimeReal& operator=(const TimeReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
TimeReal& TimeReal::operator=(const TimeReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      TimeData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TimeReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
TimeReal::~TimeReal()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real TimeReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//------------------------------------------------------------------------------
// bool IsOptionalField(const std::string &field) const
//------------------------------------------------------------------------------
/**
 * @return true if input field name is optional field, false otherwise
 */
//------------------------------------------------------------------------------
bool TimeReal::IsOptionalField(const std::string &field) const
{
   if (field == "Epoch")
      return true;
   
   return false;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer TimeReal::GetNumRefObjects() const
{
   return TimeData::GetNumRefObjects();
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
bool TimeReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return TimeData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool TimeReal::Validate()
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
bool TimeReal::Initialize()
{
   #if DEBUG_TIMEREAL
   MessageInterface::ShowMessage
      ("TimeReal::Initialize() <%p>'%s' entered, IsGlobal=%d\n", this,
       GetName().c_str(), IsGlobal());
   #endif
   
   RealVar::Initialize();
   
   // Set Parameter pointer (LOJ: 2015.07.24)
   SetParameter(this);
   
   // Reset initial epoch and flag unless it is global
   // for fix of GMT5160 LOJ: 2015.07.24)
   if (!IsGlobal())
   {
      mInitialEpoch = 0.0;
      mIsInitialEpochSet = false;
   }
   
   try
   {
      InitializeRefObjects();
   }
   catch (BaseException &e)
   {
      #if DEBUG_TIMEREAL
      MessageInterface::ShowMessage
         ("TimeReal::Initialize() Fail to initialize Parameter:%s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("WARNING:  " + e.GetFullMessage() + " in " + GetName() + "\n");
   }
   
   #if DEBUG_TIMEREAL
   MessageInterface::ShowMessage
      ("TimeReal::Initialize() <%p>'%s' returning true", this, GetName().c_str());
   #endif
   return true;
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool TimeReal::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   return TimeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls TimeData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string TimeReal::GetRefObjectName(const UnsignedInt type) const
{
   return TimeData::GetRefObjectName(type);
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
const StringArray& TimeReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return TimeData::GetRefObjectNameArray(type);
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
bool TimeReal::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   return TimeData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls TimeData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* TimeReal::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   return TimeData::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls TimeData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool TimeReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("TimeReal::SetRefObject() <%p>'%s' entered, obj=<%p><%s>'%s', type=%d, name='%s'\n",
       this, this->GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
   {
      #if DEBUG_REF_OBJECT
      MessageInterface::ShowMessage
         ("TimeReal::SetRefObject() <%p>'%s' just returning false, obj is NULL\n",
          this, this->GetName().c_str());
      #endif
      return false;
   }
   
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("   mParamOwnerName='%s', Is%sGlobal=%d, Is%sLocal=%d\n", mParamOwnerName.c_str(),
       name.c_str(), obj->IsGlobal(), name.c_str(), obj->IsLocal());
   #endif
   
   // Set owner object for Parameter here (LOJ: 2015.08.05)
   if (obj->GetName() == mParamOwnerName)
      SetOwner(obj);
   
   bool setOk = TimeData::SetRefObject(obj, type, name);
   
   #if DEBUG_REF_OBJECT
   GmatBase *paramOwner = GetOwner();
   MessageInterface::ShowMessage(WriteObjectInfo("paramOwner=", paramOwner);
   MessageInterface::ShowMessage
      ("TimeReal::SetRefObject() <%p>'%s' returning %d\n", this,
       this->GetName().c_str(), setOk);
   #endif
   
   return setOk;
}

