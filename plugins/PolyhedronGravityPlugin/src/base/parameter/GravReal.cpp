//$Id$
//------------------------------------------------------------------------------
//                                GravReal
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/24
//
/**
 * Implements GravReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------

#include "GravReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GravReal 1
//#define DEBUG_RENAME


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// GravReal(const std::string &name, const std::string &typeStr,
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
GravReal::GravReal(const std::string &name, const std::string &typeStr,
                 GmatBase *obj, const std::string &desc,
                 const std::string &unit, UnsignedInt ownerType,
                 GmatParam::DepObject depObj)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, depObj,
             ownerType, false, false)
{
   objectTypeNames.push_back("ODEData");
   mNeedCoordSystem = false;
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// GravReal(const GravReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
GravReal::GravReal(const GravReal &copy) :
   RealVar        (copy),
   GravData        (copy)
{
}


//------------------------------------------------------------------------------
// GravReal& operator=(const GravReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
GravReal& GravReal::operator=(const GravReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      GravData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~GravReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GravReal::~GravReal()
{
   //MessageInterface::ShowMessage("==> GravReal::~GravReal()\n");
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
Real GravReal::EvaluateReal()
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
void GravReal::SetSolarSystem(SolarSystem *ss)
{
#if DEBUG_GravReal
   MessageInterface::ShowMessage
      ("GravReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
       this->GetName().c_str());
#endif
   
   if (GravData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      GravData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      GravData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer GravReal::GetNumRefObjects() const
{
   return GravData::GetNumRefObjects();
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
bool GravReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return GravData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool GravReal::Validate()
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
bool GravReal::Initialize()
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
bool GravReal::RenameRefObject(const UnsignedInt type,
                              const std::string &oldName,
                              const std::string &newName)
{
   #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("GravReal::RenameRefObject(%d, %s, %s) "
            "called\n", type, oldName.c_str(), newName.c_str());
   #endif

   isInitialized = false;
   if (type == Gmat::ODE_MODEL)
      mModel = NULL;

   return GravData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls GravData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string GravReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = GravData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("GravReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + GetTypeName() + "\n");
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
const StringArray& GravReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return GravData::GetRefObjectNameArray(type);
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
bool GravReal::SetRefObjectName(const UnsignedInt type,
                               const std::string &name)
{
   if (type == Gmat::ODE_MODEL)
      mModel = NULL;
   return GravData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls GravData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* GravReal::GetRefObject(const UnsignedInt type,
                                const std::string &name)
{
   return GravData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls GravData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool GravReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                           const std::string &name)
{
   return GravData::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool NeedsForces() const
//------------------------------------------------------------------------------
/**
 * Checks for force model usage, so the Transient force table can be added
 *
 * @return true if the transient forces are needed, false (the default) if not
 */
//------------------------------------------------------------------------------
bool GravReal::NeedsForces() const
{
   return true;
}
