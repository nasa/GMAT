//$Id$
//------------------------------------------------------------------------------
//                                  BallisticMassReal
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares BallisticMass real data class.
 */
//------------------------------------------------------------------------------

#include "BallisticMassReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// BallisticMassReal(const std::string &name, const std::string &typeStr, 
//              GmatBase *obj, const std::string &desc, const std::string &unit,
//              bool isSettable = true)
//------------------------------------------------------------------------------
BallisticMassReal::BallisticMassReal(const std::string &name, const std::string &typeStr, 
                                     GmatBase *obj, const std::string &desc,
                                     const std::string &unit, bool isSettable)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             GmatParam::NO_DEP, Gmat::SPACECRAFT, false, isSettable),
     SpacecraftData(name)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// BallisticMassReal(const BallisticMassReal &copy)
//------------------------------------------------------------------------------
BallisticMassReal::BallisticMassReal(const BallisticMassReal &copy)
   : RealVar(copy), SpacecraftData(copy)
{
}


//------------------------------------------------------------------------------
// BallisticMassReal& operator=(const BallisticMassReal &right)
//------------------------------------------------------------------------------
BallisticMassReal& BallisticMassReal::operator=(const BallisticMassReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      SpacecraftData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BallisticMassReal()
//------------------------------------------------------------------------------
BallisticMassReal::~BallisticMassReal()
{
}


//------------------------------------------------------------------------------
// Real EvaluateReal()
//------------------------------------------------------------------------------
Real BallisticMassReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer BallisticMassReal::GetNumRefObjects() const
{
   return SpacecraftData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool AddRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool BallisticMassReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ATTITUDEREAL
      MessageInterface::ShowMessage
         ("BallisticMassReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return SpacecraftData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                          replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool BallisticMassReal::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool BallisticMassReal::Initialize()
{
   try
   {
      InitializeRefObjects();  // NOTE - as of 2012.08.16, InitializeRefObejcts
                               // does not throw an exception
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("BallisticMassReal::Initialize() failed to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool BallisticMassReal::RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName)
{
   return SpacecraftData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string BallisticMassReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = SpacecraftData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("BallisticMassReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not a valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& BallisticMassReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return SpacecraftData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool BallisticMassReal::SetRefObjectName(const UnsignedInt type,
                                         const std::string &name)
{
   bool ret = SpacecraftData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** BallisticMassReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* BallisticMassReal::GetRefObject(const UnsignedInt type,
                                          const std::string &name)
{
   GmatBase *obj = SpacecraftData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("BallisticMassReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool BallisticMassReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name)
{
   #if DEBUG_ATTITUDEREAL
   MessageInterface::ShowMessage
      ("BallisticMassReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return SpacecraftData::SetRefObject(obj, type, name);
}

