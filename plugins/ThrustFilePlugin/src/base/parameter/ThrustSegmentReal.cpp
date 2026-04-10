//------------------------------------------------------------------------------
//                                ThrustSegmentReal
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
// Author: Peter Candell
// Created: 2022/09/06
//
/**
 * Implements ThrustSegmentReal class which provides base class for environment realated
 * Real Parameters
 */
 //------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ThrustSegmentReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ThrustSegmentReal 1
//#define DEBUG_RENAME


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ThrustSegmentReal(const std::string &name, const std::string &typeStr,
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
ThrustSegmentReal::ThrustSegmentReal(const std::string &name, const std::string &typeStr,
   GmatBase *obj, const std::string &desc, 
   const std::string &unit, bool isSettable)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit, GmatParam::NO_DEP,
      Gmat::THRUST_SEGMENT, false, isSettable, true, true, Gmat::THRUST_SEGMENT),
   ThrustSegmentData(name)

{
   objectTypeNames.push_back("ThrustSegmentData");
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// ThrustSegmentReal(const ThrustSegmentReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
ThrustSegmentReal::ThrustSegmentReal(const ThrustSegmentReal &copy) :
   RealVar(copy),
   ThrustSegmentData(copy)
{
}


//------------------------------------------------------------------------------
// ThrustSegmentReal& operator=(const ThrustSegmentReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
ThrustSegmentReal& ThrustSegmentReal::operator=(const ThrustSegmentReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      ThrustSegmentData::operator=(right);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustSegmentReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
ThrustSegmentReal::~ThrustSegmentReal()
{
   //MessageInterface::ShowMessage("==> ThrustSegmentReal::~ThrustSegmentReal()\n");
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
Real ThrustSegmentReal::EvaluateReal()
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
Integer ThrustSegmentReal::GetNumRefObjects() const
{
   return ThrustSegmentData::GetNumRefObjects();
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
bool ThrustSegmentReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
      return ThrustSegmentData::AddRefObject(obj->GetType(), obj->GetName(), obj,
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
bool ThrustSegmentReal::Validate()
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
bool ThrustSegmentReal::Initialize()
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
bool ThrustSegmentReal::RenameRefObject(const UnsignedInt type,
   const std::string &oldName,
   const std::string &newName)
{
   return ThrustSegmentData::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls ThrustSegmentData to get reference object name for given type.
 *
 * @return reference object name.
 */
 //------------------------------------------------------------------------------
std::string ThrustSegmentReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = ThrustSegmentData::GetRefObjectName(type);

   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
      ("ThrustSegmentReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
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
const StringArray& ThrustSegmentReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return ThrustSegmentData::GetRefObjectNameArray(type);
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
bool ThrustSegmentReal::SetRefObjectName(const UnsignedInt type,
   const std::string &name)
{
   return ThrustSegmentData::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls ThrustSegmentData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
 //------------------------------------------------------------------------------
GmatBase* ThrustSegmentReal::GetRefObject(const UnsignedInt type,
   const std::string &name)
{
   return ThrustSegmentData::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls ThrustSegmentData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
 //------------------------------------------------------------------------------
bool ThrustSegmentReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string &name)
{
   return ThrustSegmentData::SetRefObject(obj, type, name);
}

