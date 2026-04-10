//$Id$
//------------------------------------------------------------------------------
//                                BurnReal
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
// Created: 2004/03/29
//
/**
 * Implements BurnReal class which provides base class for orbit related Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "BurnReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURNREAL 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BurnReal(const std::string &name, const std::string &typeStr, 
//          GmatBase *obj, const std::string &desc,
//          const std::string &unit, GmatParam::DepObject depObj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NO_DEP)
 */
//------------------------------------------------------------------------------
BurnReal::BurnReal(const std::string &name, const std::string &typeStr,
                   UnsignedInt ownerType, GmatBase *obj,
                   const std::string &desc, const std::string &unit,
                   GmatParam::DepObject depObj, bool isSettable)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             depObj, ownerType, false, isSettable),
     BurnData(name, typeStr, ownerType)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// BurnReal(const BurnReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BurnReal::BurnReal(const BurnReal &copy)
   : RealVar(copy), BurnData(copy)
{
}


//------------------------------------------------------------------------------
// BurnReal& operator=(const BurnReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BurnReal& BurnReal::operator=(const BurnReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      BurnData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BurnReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BurnReal::~BurnReal()
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
Real BurnReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* BurnReal::GetInternalCoordSystem()
{
   return BurnData::GetInternalCoordSys();
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void BurnReal::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_BURNREAL
      MessageInterface::ShowMessage
         ("BurnReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
          this->GetName().c_str());
   #endif
   
   if (BurnData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      BurnData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      BurnData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void BurnReal::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_BURNREAL
      MessageInterface::ShowMessage
         ("BurnReal::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
          this->GetName().c_str());
   #endif
   
   BurnData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer BurnReal::GetNumRefObjects() const
{
   return BurnData::GetNumRefObjects();
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
bool BurnReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      //loj: 4/12/05 if obj->GetType() is CELESTIAL_BODY, set as SPACE_POINT
      // since CelestialBody subtypes are not set as SPACE_POINT
      ///@todo Use IsOfType(Gmat::SPACE_POINT) when GmatBase provides this method.

      #if DEBUG_BURNREAL
      MessageInterface::ShowMessage
         ("BurnReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      //if (obj->GetType() == Gmat::CELESTIAL_BODY)
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         return BurnData::AddRefObject(Gmat::SPACE_POINT, obj->GetName(), obj,
                                        replaceName);
      else
         return BurnData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
      
   }
   
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
bool BurnReal::Validate()
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
bool BurnReal::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw GmatBaseException
         ("BurnReal::Initialize() Fail to initialize Parameter:" +
          this->GetTypeName() + "\n" + e.GetFullMessage());
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
bool BurnReal::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   return BurnData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls BurnData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string BurnReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = BurnData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("BurnReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
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
const StringArray& BurnReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return BurnData::GetRefObjectNameArray(type);
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
bool BurnReal::SetRefObjectName(const UnsignedInt type,
                                const std::string &name)
{
   bool ret = BurnData::SetRefObjectName(type, name);
   
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** BurnReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls BurnData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* BurnReal::GetRefObject(const UnsignedInt type,
                                 const std::string &name)
{
   GmatBase *obj = BurnData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("BurnReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   return obj;
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls BurnData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool BurnReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
   #if DEBUG_BURNREAL
   MessageInterface::ShowMessage
      ("BurnReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return BurnData::SetRefObject(obj, type, name);
}

