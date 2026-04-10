//$Id$
//------------------------------------------------------------------------------
//                                PlanetReal
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
// Created: 2004/12/13
//
/**
 * Implements PlanetReal class which provides base class for the planet
 * realated Real Parameters
 */
//------------------------------------------------------------------------------

#include "PlanetReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PLANET_REAL 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PlanetReal(const std::string &name, const std::string &typeStr, 
//            GmatBase *obj, const std::string &desc,
//            const std::string &unit, UnsignedInt ownerType,
//            GmatParam::DepObject depObj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <ownerType> object type who owns this parameter as property
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NO_DEP)
 */
//------------------------------------------------------------------------------
PlanetReal::PlanetReal(const std::string &name, const std::string &typeStr, 
                       GmatBase *obj, const std::string &desc,
                       const std::string &unit, UnsignedInt ownerType,
                       GmatParam::DepObject depObj)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, depObj,
             ownerType, false, false)
{
   mNeedCoordSystem = false;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// PlanetReal(const PlanetReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
PlanetReal::PlanetReal(const PlanetReal &copy)
   : RealVar(copy), PlanetData(copy)
{
}


//------------------------------------------------------------------------------
// PlanetReal& operator=(const PlanetReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
PlanetReal& PlanetReal::operator=(const PlanetReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      PlanetData::operator=(right);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// ~PlanetReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PlanetReal::~PlanetReal()
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
Real PlanetReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void PlanetReal::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_PLANET_REAL
   MessageInterface::ShowMessage
      ("PlanetReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
       this->GetName().c_str());
   #endif
   
   if (PlanetData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      PlanetData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      PlanetData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void PlanetReal::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_PLANET_REAL
   MessageInterface::ShowMessage
      ("PlanetReal::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
       this->GetName().c_str());
   #endif
   
   PlanetData::SetInternalCoordSystem(cs);
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer PlanetReal::GetNumRefObjects() const
{
   return PlanetData::GetNumRefObjects();
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
bool PlanetReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return PlanetData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool PlanetReal::Validate()
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
bool PlanetReal::Initialize()
{
   try
   {
      #if DEBUG_PLANET_REAL
      MessageInterface::ShowMessage
         ("===> PlanetReal::Initialize() calling InitializeRefObjects() on %s\n",
          instanceName.c_str());
      #endif
      
      InitializeRefObjects();
   }
   catch(InvalidDependencyException &e)
   {
      #if DEBUG_PLANET_REAL
      MessageInterface::ShowMessage
         ("PlanetReal::Initialize() Fail to initialize Parameter: %s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("Incorrect parameter dependency: " + GetName() + ".\n" +
          this->GetTypeName() + e.GetFullMessage() + "\n");
   }
   catch(BaseException &e)
   {
      #if DEBUG_PLANET_REAL
      MessageInterface::ShowMessage
         ("OrbitReal::Initialize() Fail to initialize Parameter: %s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         (e.GetFullMessage() + " in " + GetName() + "\n");
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
bool PlanetReal::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   return PlanetData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string PlanetReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = PlanetData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("PlanetReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
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
const StringArray& PlanetReal::GetRefObjectNameArray(const UnsignedInt type)
{
   #if DEBUG_PLANET_REAL
   MessageInterface::ShowMessage
      ("===> PlanetReal::GetRefObjectNameArray() calling PlanetData::GetRefObjectNameArray() on %s\n",
       instanceName.c_str());
   #endif
   
   return PlanetData::GetRefObjectNameArray(type);
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
bool PlanetReal::SetRefObjectName(const UnsignedInt type,
                                  const std::string &name)
{
   #ifdef DEBUG_PLANET_REAL
   MessageInterface::ShowMessage
      ("PlanetReal::SetRefObjectName() this='%s', type=%d, name='%s'\n",
       GetName().c_str(), type, name.c_str());
   #endif
   
   //loj: 5/27/05 write parameter name, if failed to set
   bool ret = PlanetData::SetRefObjectName(type, name);
   if (!ret)
      MessageInterface::ShowMessage
         ("*** Warning *** PlanetReal::SetRefObjectName() RefObjType:%s is not valid "
          "for ParameterName:%s\n", GmatBase::GetObjectTypeString(type).c_str(),
          this->GetName().c_str());
   
   return ret;
   //return PlanetData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* PlanetReal::GetRefObject(const UnsignedInt type,
                                const std::string &name)
{
   return PlanetData::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls PlanetData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool PlanetReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                const std::string &name)
{
   return PlanetData::SetRefObject(obj, type, name);
}

