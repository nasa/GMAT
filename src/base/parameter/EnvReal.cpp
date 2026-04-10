//$Id$
//------------------------------------------------------------------------------
//                                EnvReal
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
// Created: 2004/12/10
//
/**
 * Implements EnvReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------

#include "EnvReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ENVREAL 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// EnvReal(const std::string &name, const std::string &typeStr, 
//         GmatBase *obj, const std::string &desc,
//         const std::string &unit, UnsignedInt ownerType,
//         GmatParam::DepObject depObj)
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
EnvReal::EnvReal(const std::string &name, const std::string &typeStr, 
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
// EnvReal(const EnvReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
EnvReal::EnvReal(const EnvReal &copy)
   : RealVar(copy), EnvData(copy)
{
}


//------------------------------------------------------------------------------
// EnvReal& operator=(const EnvReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
EnvReal& EnvReal::operator=(const EnvReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      EnvData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~EnvReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EnvReal::~EnvReal()
{
   //MessageInterface::ShowMessage("==> EnvReal::~EnvReal()\n");
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
Real EnvReal::EvaluateReal()
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
void EnvReal::SetSolarSystem(SolarSystem *ss)
{
#if DEBUG_ENVREAL
   MessageInterface::ShowMessage
      ("EnvReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
       this->GetName().c_str());
#endif
   
   if (EnvData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      EnvData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      EnvData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer EnvReal::GetNumRefObjects() const
{
   return EnvData::GetNumRefObjects();
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
bool EnvReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return EnvData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool EnvReal::Validate()
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
bool EnvReal::Initialize()
{
   InitializeRefObjects();
   return true;
}

//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool EnvReal::RenameRefObject(const UnsignedInt type,
                              const std::string &oldName,
                              const std::string &newName)
{
   return EnvData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls EnvData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string EnvReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = EnvData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
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
const StringArray& EnvReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return EnvData::GetRefObjectNameArray(type);
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
bool EnvReal::SetRefObjectName(const UnsignedInt type,
                               const std::string &name)
{
   return EnvData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls EnvData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* EnvReal::GetRefObject(const UnsignedInt type,
                                const std::string &name)
{
   return EnvData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls EnvData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool EnvReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                           const std::string &name)
{
   return EnvData::SetRefObject(obj, type, name);
}

