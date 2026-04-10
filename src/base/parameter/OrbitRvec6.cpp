//$Id$
//------------------------------------------------------------------------------
//                                OrbitRvec6
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
// Created: 2004/09/08
//
/**
 * Implements OrbitRvec6 class which provides base class for orbit realated
 * Rvector6 Parameters.
 */
//------------------------------------------------------------------------------

#include "OrbitRvec6.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITRVEC6 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitRvec6(const std::string &name, const std::string &typeStr, 
//            GmatBase *obj, const std::string &desc,
//            const std::string &unit, DepObject depObj)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
OrbitRvec6::OrbitRvec6(const std::string &name, const std::string &typeStr, 
                       GmatBase *obj, const std::string &desc,
                       const std::string &unit, GmatParam::DepObject depObj,
                       UnsignedInt objType)
   : Rvec6Var(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, depObj,
              objType)
{
   mNeedCoordSystem = true;
   AddRefObject(obj);
}

//------------------------------------------------------------------------------
// OrbitRvec6(const OrbitRvec6 &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitRvec6::OrbitRvec6(const OrbitRvec6 &copy)
   : Rvec6Var(copy), OrbitData(copy)
{
}

//------------------------------------------------------------------------------
// OrbitRvec6& operator=(const OrbitRvec6 &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitRvec6& OrbitRvec6::operator=(const OrbitRvec6 &right)
{
   if (this != &right)
   {
      Rvec6Var::operator=(right);
      OrbitData::operator=(right);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitRvec6()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitRvec6::~OrbitRvec6()
{
}

//-------------------------------------
// methods inherited from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const Rvector6 EvaluateRvector6()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
const Rvector6& OrbitRvec6::EvaluateRvector6()
{
   Evaluate();
   return mRvec6Value;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer OrbitRvec6::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}

//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void OrbitRvec6::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_ORBITRVEC6
   MessageInterface::ShowMessage
      ("OrbitRvec6::SetSolarSystem() ss=%s", ss->GetTypeName().c_str());
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
void OrbitRvec6::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("OrbitRvec6::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
       this->GetName().c_str());
   #endif
   
   OrbitData::SetInternalCoordSys(cs);
}

//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool OrbitRvec6::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj);
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
bool OrbitRvec6::Validate()
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
bool OrbitRvec6::Initialize()
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
bool OrbitRvec6::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   return OrbitData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string OrbitRvec6::GetRefObjectName(const UnsignedInt type) const
{
   return OrbitData::GetRefObjectName(type);
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
bool OrbitRvec6::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   return OrbitData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* OrbitRvec6::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   return OrbitData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool OrbitRvec6::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   return OrbitData::SetRefObject(obj, type, name);
}

