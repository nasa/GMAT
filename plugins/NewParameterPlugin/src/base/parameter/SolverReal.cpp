//$Id$
//------------------------------------------------------------------------------
//                                SolverReal
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
 * Implements SolverReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------

#include "SolverReal.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SolverReal 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SolverReal(const std::string &name, const std::string &typeStr,
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
SolverReal::SolverReal(const std::string &name, const std::string &typeStr,
                 GmatBase *obj, const std::string &desc,
                 const std::string &unit, UnsignedInt ownerType,
                 GmatParam::DepObject depObj)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, depObj,
             ownerType, false, false)
{
   objectTypeNames.push_back("SolverData");
   mNeedCoordSystem = false;
   AddRefObject(obj);

   std::string type, ownerName, dep;
   GmatStringUtil::ParseParameter(name, type, ownerName, dep);
   mOwnerName = ownerName;

   mNeedExternalClone = true;
}


//------------------------------------------------------------------------------
// SolverReal(const SolverReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SolverReal::SolverReal(const SolverReal &copy) :
   RealVar        (copy),
   SolverData     (copy)
{
}


//------------------------------------------------------------------------------
// SolverReal& operator=(const SolverReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SolverReal& SolverReal::operator=(const SolverReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      SolverData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~SolverReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SolverReal::~SolverReal()
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
Real SolverReal::EvaluateReal()
{
   Evaluate();
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer SolverReal::GetNumRefObjects() const
{
   return SolverData::GetNumRefObjects();
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
bool SolverReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return SolverData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool SolverReal::Validate()
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
bool SolverReal::Initialize()
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
bool SolverReal::RenameRefObject(const UnsignedInt type,
                              const std::string &oldName,
                              const std::string &newName)
{
   return SolverData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls SolverData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string SolverReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = SolverData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("SolverReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
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
const StringArray& SolverReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return SolverData::GetRefObjectNameArray(type);
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
bool SolverReal::SetRefObjectName(const UnsignedInt type,
                               const std::string &name)
{
   return SolverData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls SolverData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* SolverReal::GetRefObject(const UnsignedInt type,
                                const std::string &name)
{
   return SolverData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls SolverData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool SolverReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                           const std::string &name)
{
   return SolverData::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// const std::string& Parameter::GetExternalCloneName(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of an external clone if one is needed
 *
 * @param whichOne Index of the clone needed; the default is 0.
 *
 * @return The name of the needed clone, or the empty string if one is not
 *         needed.
 */
//------------------------------------------------------------------------------
const std::string SolverReal::GetExternalCloneName(Integer whichOne)
{
   return mOwnerName;
}

//------------------------------------------------------------------------------
// void Parameter::SetExternalClone(GmatBase *clone)
//------------------------------------------------------------------------------
/**
 * Sets the reference to an external clone
 *
 * @param clone The external clone
 */
//------------------------------------------------------------------------------
void SolverReal::SetExternalClone(GmatBase *clone)
{
   if (clone == NULL)
      throw ParameterException("External clone pointer is NULL");

   SolverData::AddRefObject(clone->GetType(), clone->GetName(), clone, true);
}

