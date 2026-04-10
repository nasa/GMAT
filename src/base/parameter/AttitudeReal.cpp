//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeReal
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
// Author: Daniel Hunter
// Created: 2006/6/26
//
/**
 * Declares Attitude real data class.
 */
//------------------------------------------------------------------------------
#include "AttitudeReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// AttitudeReal(const std::string &name, const std::string &typeStr, 
//              GmatBase *obj, const std::string &desc, const std::string &unit)
//------------------------------------------------------------------------------
AttitudeReal::AttitudeReal(const std::string &name, const std::string &typeStr, 
                           GmatBase *obj, const std::string &desc,
                           const std::string &unit, bool isSettable)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             GmatParam::OWNED_OBJ, Gmat::SPACECRAFT, false, isSettable, true,
             true, Gmat::ATTITUDE)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// AttitudeReal(const AttitudeReal &copy)
//------------------------------------------------------------------------------
AttitudeReal::AttitudeReal(const AttitudeReal &copy)
   : RealVar(copy), AttitudeData(copy)
{
}


//------------------------------------------------------------------------------
// AttitudeReal& operator=(const AttitudeReal &right)
//------------------------------------------------------------------------------
AttitudeReal& AttitudeReal::operator=(const AttitudeReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      AttitudeData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeReal()
//------------------------------------------------------------------------------
AttitudeReal::~AttitudeReal()
{
}


//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
Real AttitudeReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer AttitudeReal::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool AttitudeReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ATTITUDEREAL
      MessageInterface::ShowMessage
         ("AttitudeReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return AttitudeData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool AttitudeReal::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool AttitudeReal::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("AttitudeReal::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool AttitudeReal::RenameRefObject(const UnsignedInt type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string AttitudeReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = AttitudeData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("AttitudeReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& AttitudeReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeReal::SetRefObjectName(const UnsignedInt type,
                                    const std::string &name)
{
   bool ret = AttitudeData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** AttitudeReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* AttitudeReal::GetRefObject(const UnsignedInt type,
                                     const std::string &name)
{
   GmatBase *obj = AttitudeData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("AttitudeReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                const std::string &name)
{
   #if DEBUG_ATTITUDEREAL
   MessageInterface::ShowMessage
      ("AttitudeReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return AttitudeData::SetRefObject(obj, type, name);
}

