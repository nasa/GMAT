//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeRvector
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
#include "AttitudeRvector.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// AttitudeRvector(const std::string &name, const std::string &typeStr, ...)
//------------------------------------------------------------------------------
AttitudeRvector::AttitudeRvector(const std::string &name, const std::string &typeStr, 
                                 GmatBase *obj, const std::string &desc,
                                 const std::string &unit, bool isSettable,
                                 Integer size)
   : RvectorVar(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
                GmatParam::OWNED_OBJ, Gmat::SPACECRAFT, false, isSettable,
                Gmat::ATTITUDE, size)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// AttitudeRvector(const AttitudeRvector &copy)
//------------------------------------------------------------------------------
AttitudeRvector::AttitudeRvector(const AttitudeRvector &copy)
   : RvectorVar(copy), AttitudeData(copy)
{
}


//------------------------------------------------------------------------------
// AttitudeRvector& operator=(const AttitudeRvector &right)
//------------------------------------------------------------------------------
AttitudeRvector& AttitudeRvector::operator=(const AttitudeRvector &right)
{
   if (this != &right)
   {
      RvectorVar::operator=(right);
      AttitudeData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeRvector()
//------------------------------------------------------------------------------
AttitudeRvector::~AttitudeRvector()
{
}


//------------------------------------------------------------------------------
// const Rvector& EvaluateRvector()
//------------------------------------------------------------------------------
const Rvector& AttitudeRvector::EvaluateRvector()
{
   Evaluate();
   return mRvectorValue;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer AttitudeRvector::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool AttitudeRvector::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ATTITUDE_RVECTOR
      MessageInterface::ShowMessage
         ("AttitudeRvector::AddRefObject() obj->GetName()=%s, type=%d\n",
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
bool AttitudeRvector::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool AttitudeRvector::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("AttitudeRvector::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool AttitudeRvector::RenameRefObject(const UnsignedInt type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string AttitudeRvector::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = AttitudeData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("AttitudeRvector::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& AttitudeRvector::GetRefObjectNameArray(const UnsignedInt type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRvector::SetRefObjectName(const UnsignedInt type,
                                       const std::string &name)
{
   bool ret = AttitudeData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** AttitudeRvector::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* AttitudeRvector::GetRefObject(const UnsignedInt type,
                                        const std::string &name)
{
   GmatBase *obj = AttitudeData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("AttitudeRvector::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRvector::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                   const std::string &name)
{
   #if DEBUG_ATTITUDE_RVECTOR
   MessageInterface::ShowMessage
      ("AttitudeRvector::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return AttitudeData::SetRefObject(obj, type, name);
}

