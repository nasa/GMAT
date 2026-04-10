//$Id$
//------------------------------------------------------------------------------
//                                BplaneReal
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
// Created: 2005/06/16
//
/**
 * Implements BplaneReal class which provides base class for B-Plane realated Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "BplaneReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BPLANE_REAL 1
//#define DEBUG_BPLANE_REAL_INIT 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BplaneReal(const std::string &name, const std::string &typeStr, 
//            GmatBase *obj, const std::string &desc,
//            const std::string &unit, GmatParam::DepObject depObj)
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
BplaneReal::BplaneReal(const std::string &name, const std::string &typeStr, 
                       GmatBase *obj, const std::string &desc,
                       const std::string &unit, GmatParam::DepObject depObj)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             depObj, Gmat::SPACECRAFT, false, false)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// BplaneReal(const BplaneReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BplaneReal::BplaneReal(const BplaneReal &copy)
   : RealVar(copy), BplaneData(copy)
{
   #if DEBUG_BPLANE_REAL
   MessageInterface::ShowMessage
      ("===> BplaneReal::BplaneReal() copy constructor called on %s\n",
       instanceName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// BplaneReal& operator=(const BplaneReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
BplaneReal& BplaneReal::operator=(const BplaneReal &right)
{
   if (this != &right)
      return *this;
   
   RealVar::operator=(right);
   BplaneData::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BplaneReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BplaneReal::~BplaneReal()
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
Real BplaneReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* BplaneReal::GetInternalCoordSystem()
{
   return BplaneData::GetInternalCoordSys();
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void BplaneReal::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_BPLANE_REAL
   MessageInterface::ShowMessage
      ("==> BplaneReal::SetSolarSystem() ss=%s addr=%p to %s\n", ss->GetTypeName().c_str(),
       ss, this->GetName().c_str());
   #endif
   
   if (BplaneData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      BplaneData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      BplaneData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void BplaneReal::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_BPLANE_REAL
      MessageInterface::ShowMessage
         ("BplaneReal::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
          this->GetName().c_str());
   #endif
   
   BplaneData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer BplaneReal::GetNumRefObjects() const
{
   return BplaneData::GetNumRefObjects();
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
bool BplaneReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_BPLANE_REAL
      MessageInterface::ShowMessage
         ("BplaneReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         return BplaneData::AddRefObject(Gmat::SPACE_POINT, obj->GetName(), obj,
                                        replaceName);
      else
         return BplaneData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool BplaneReal::Validate()
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
bool BplaneReal::Initialize()
{
   #if DEBUG_BPLANE_REAL_INIT
   MessageInterface::ShowMessage
      ("BplaneReal::Initialize() %s\n", this->GetTypeName().c_str());
   #endif
   
   try
   {
      InitializeRefObjects();
   }
   catch(BaseException &e)
   {
      throw ParameterException
         ("BplaneReal::Initialize() Fail to initialize Parameter:" +
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
bool BplaneReal::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   return BplaneData::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls BplaneData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string BplaneReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = BplaneData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("BplaneReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
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
const StringArray& BplaneReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return BplaneData::GetRefObjectNameArray(type);
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
bool BplaneReal::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   return BplaneData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls BplaneData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* BplaneReal::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   GmatBase *obj = BplaneData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("BplaneReal::GetRefObject() Cannot find ref. object of type:" +
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
 * Calls BplaneData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool BplaneReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #if DEBUG_BPLANE_REAL
   MessageInterface::ShowMessage
      ("BplaneReal::SetRefObject() setting type=%d, name=%s to %s\n",
       type, name.c_str(), this->GetName().c_str());
   #endif
   
   return BplaneData::SetRefObject(obj, type, name);
}

