//$Id$
//------------------------------------------------------------------------------
//                                  OrbitRmat33
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
// Author: Linda Jun (GSFC/NASA)
// Created: 2009.03.30
//
/**
 * Declares OrbitRmat33 class.
 */
//------------------------------------------------------------------------------

#include "OrbitRmat33.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITRMAT33_REFOBJ
//#define DEBUG_ORBITRMAT33_SET

//------------------------------------------------------------------------------
// OrbitRmat33(const std::string &name, const std::string &typeStr, 
//             GmatBase *obj, const std::string &desc, const std::string &unit,
//             GmatParam::DepObject depObj, bool isSettable)
//------------------------------------------------------------------------------
OrbitRmat33::OrbitRmat33(const std::string &name, const std::string &typeStr, 
                         GmatBase *obj, const std::string &desc,
                         const std::string &unit, GmatParam::DepObject depObj,
                         bool isSettable, UnsignedInt paramOwnerType)
   : Rmat33Var(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
               depObj, paramOwnerType, isSettable),
     OrbitData(name, typeStr, paramOwnerType)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// OrbitRmat33(const OrbitRmat33 &copy)
//------------------------------------------------------------------------------
OrbitRmat33::OrbitRmat33(const OrbitRmat33 &copy)
   : Rmat33Var(copy), OrbitData(copy)
{
}


//------------------------------------------------------------------------------
// OrbitRmat33& operator=(const OrbitRmat33 &right)
//------------------------------------------------------------------------------
OrbitRmat33& OrbitRmat33::operator=(const OrbitRmat33 &right)
{
   if (this != &right)
   {
      Rmat33Var::operator=(right);
      OrbitData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitRmat33()
//------------------------------------------------------------------------------
OrbitRmat33::~OrbitRmat33()
{
}


//------------------------------------------------------------------------------
// const Rmatrix66& EvaluateRmatrix()
//------------------------------------------------------------------------------
const Rmatrix& OrbitRmat33::EvaluateRmatrix()
{
   Evaluate();
   return mRmat33Value;
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitRmat33::GetInternalCoordSystem()
{
   return OrbitData::GetInternalCoordSys();
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void OrbitRmat33::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_ORBITRMAT33_SET
   MessageInterface::ShowMessage
      ("OrbitRmat33::SetSolarSystem() '%s' entered, ss=<%p>'%s'\n",
       GetName().c_str(), ss, ss->GetTypeName().c_str());
   #endif
   
   if (OrbitData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      OrbitData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      OrbitData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void OrbitRmat33::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_ORBITRMAT33_SET
   MessageInterface::ShowMessage
      ("OrbitRmat33::SetInternalCoordSystem() '%s' entered, cs=<%p>'%s'\n",
       GetName().c_str(), cs, cs->GetTypeName().c_str());
   #endif
   
   OrbitData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer OrbitRmat33::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// bool addRefObject(GmatBase *obj, bool replaceName)
//------------------------------------------------------------------------------
bool OrbitRmat33::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #ifdef DEBUG_ORBITRMAT33_REFOBJ
      MessageInterface::ShowMessage
         ("OrbitRmat33::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                     replaceName);
      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
bool OrbitRmat33::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool OrbitRmat33::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("OrbitRmat33::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//                      const std::string &newName)
//------------------------------------------------------------------------------
bool OrbitRmat33::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return OrbitData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string OrbitRmat33::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = OrbitData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitRmat33::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& OrbitRmat33::GetRefObjectNameArray(const UnsignedInt type)
{
   return OrbitData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
bool OrbitRmat33::SetRefObjectName(const UnsignedInt type,
                                   const std::string &name)
{
   bool ret = OrbitData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** OrbitRmat33::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OrbitRmat33::GetRefObject(const UnsignedInt type,
                                    const std::string &name)
{
   GmatBase *obj = OrbitData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("OrbitRmat33::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
bool OrbitRmat33::SetRefObject(GmatBase *obj, const UnsignedInt type,
                               const std::string &name)
{
   #ifdef DEBUG_ORBITRMAT33_REFOBJ
   MessageInterface::ShowMessage
      ("OrbitRmat33::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return OrbitData::SetRefObject(obj, type, name);
}

