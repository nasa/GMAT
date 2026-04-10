//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeRmat33
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
/**
 * Declares AttitudeRmat33 class.
 */
//------------------------------------------------------------------------------

#include "AttitudeRmat33.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ATTITUDERMAT33_REFOBJ

//------------------------------------------------------------------------------
// AttitudeRmat33(const std::string &name, const std::string &typeStr,
//                GmatBase *obj, const std::string &desc,
//                const std::string &unit,GmatParam::DepObject depObj,
//                bool isSettable)
//------------------------------------------------------------------------------
AttitudeRmat33::AttitudeRmat33(const std::string &name,
                               const std::string &typeStr,
                               GmatBase *obj, const std::string &desc,
                               const std::string &unit,
                               GmatParam::DepObject depObj,
                               bool isSettable, UnsignedInt paramOwnerType)
   : Rmat33Var(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
               depObj, paramOwnerType, isSettable)
{
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// AttitudeRmat33(const AttitudeRmat33 &copy)
//------------------------------------------------------------------------------
AttitudeRmat33::AttitudeRmat33(const AttitudeRmat33 &copy)
   : Rmat33Var(copy), AttitudeData(copy)
{
}


//------------------------------------------------------------------------------
// AttitudeRmat33& operator=(const AttitudeRmat33 &right)
//------------------------------------------------------------------------------
AttitudeRmat33& AttitudeRmat33::operator=(const AttitudeRmat33 &right)
{
   if (this != &right)
   {
      Rmat33Var::operator=(right);
      AttitudeData::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~AttitudeRmat33()
//------------------------------------------------------------------------------
AttitudeRmat33::~AttitudeRmat33()
{
}


//------------------------------------------------------------------------------
// const Rmatrix66& EvaluateRmatrix()
//------------------------------------------------------------------------------
const Rmatrix& AttitudeRmat33::EvaluateRmatrix()
{
   Evaluate();
   return mRmat33Value;
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer AttitudeRmat33::GetNumRefObjects() const
{
   return AttitudeData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool AttitudeRmat33::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #ifdef DEBUG_ATTITUDERMAT33_REFOBJ
      MessageInterface::ShowMessage
         ("AttitudeRmat33::AddRefObject() obj->GetName()=%s, type=%d\n",
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
bool AttitudeRmat33::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool AttitudeRmat33::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("AttitudeRmat33::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }

   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool AttitudeRmat33::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return AttitudeData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string AttitudeRmat33::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = AttitudeData::GetRefObjectName(type);

   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("AttitudeRmat33::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }

   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& AttitudeRmat33::GetRefObjectNameArray(const UnsignedInt type)
{
   return AttitudeData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRmat33::SetRefObjectName(const UnsignedInt type,
                                   const std::string &name)
{
   bool ret = AttitudeData::SetRefObjectName(type, name);

   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** AttitudeRmat33::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());

   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* AttitudeRmat33::GetRefObject(const UnsignedInt type,
                                    const std::string &name)
{
   #ifdef DEBUG_ATTITUDERMAT33_REFOBJ
   MessageInterface::ShowMessage
      ("AttitudeRmat33::GetRefObject() with type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif

   GmatBase *obj = AttitudeData::GetRefObject(type, name);

   if (obj == NULL)
   {
      throw ParameterException
         ("AttitudeRmat33::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }

   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool AttitudeRmat33::SetRefObject(GmatBase *obj, const UnsignedInt type,
                               const std::string &name)
{
   #ifdef DEBUG_ATTITUDERMAT33_REFOBJ
   MessageInterface::ShowMessage
      ("AttitudeRmat33::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif

   return AttitudeData::SetRefObject(obj, type, name);
}
