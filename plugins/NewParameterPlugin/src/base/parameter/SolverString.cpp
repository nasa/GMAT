//$Id$
//------------------------------------------------------------------------------
//                                SolverString
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/03/09
//
/**
 * Implements SolverString class which provides base class for time realated String
 * Parameters
 */
//------------------------------------------------------------------------------

#include "SolverString.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"               // for ParseParameter()
#include "MessageInterface.hpp"

//#define DEBUG_SOLVER_STRING_ADD
//#define DEBUG_SOLVER_STRING

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SolverString(const std::string &name, const std::string &typeStr, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <isSettable> true if parameter is settable
 */
//------------------------------------------------------------------------------
SolverString::SolverString(const std::string &name, const std::string &typeStr,
                       GmatBase *obj, const std::string &desc,
                       const std::string &unit, bool isSettable,
                       UnsignedInt paramOwnerType)
   : StringVar(name, typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
               GmatParam::NO_DEP, paramOwnerType, true, isSettable),
     SolverData()
{
   objectTypeNames.push_back("Solver");
   std::string type, ownerName, depObj;
   GmatStringUtil::ParseParameter(name, type, ownerName, depObj);
   mOwnerName = ownerName;
   mExpr = name;
   mNeedExternalClone = true;
   #ifdef DEBUG_SOLVER_STRING
      MessageInterface::ShowMessage("Creating a solver string %s\n",
            name.c_str());
   #endif
   AddRefObject(obj);
}


//------------------------------------------------------------------------------
// SolverString(const SolverString &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SolverString::SolverString(const SolverString &copy) :
   StringVar(copy),
   SolverData(copy)
{
}


//------------------------------------------------------------------------------
// SolverString& operator=(const SolverString &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
SolverString& SolverString::operator=(const SolverString &right)
{
   if (this != &right)
   {
      StringVar::operator=(right);
      SolverData::operator=(right);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~SolverString()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SolverString::~SolverString()
{
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// const virtual std::string& EvaluateString()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
const std::string& SolverString::EvaluateString()
{
   Evaluate();
   return mStringValue;
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer SolverString::GetNumRefObjects() const
{
   return SolverData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// virtual bool (GmatBase *obj, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool SolverString::AddRefObject(GmatBase *obj, bool replaceName)
{
   #ifdef DEBUG_SOLVER_STRING_ADD
      MessageInterface::ShowMessage("In SolverString::AddRefObject - obj is %s\n",
            (obj? " NOT NULL" : "NULL!"));
      MessageInterface::ShowMessage("... replaceName = %s\n",
            (replaceName? "true" : "false"));
      if (obj)
      {
         MessageInterface::ShowMessage("... obj is %s\n", obj->GetName().c_str());
         MessageInterface::ShowMessage("... type is %d\n", obj->GetType());
      }
   #endif

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
bool SolverString::Validate()
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
bool SolverString::Initialize()
{
   try
   {
      InitializeRefObjects();
   }
   catch (BaseException &e)
   {
      #if DEBUG_SolverString
      MessageInterface::ShowMessage
         ("SolverString::Initialize() Failed to initialize Parameter '%s'\n",
          GetName().c_str());
      #endif
      
      throw ParameterException
         ("WARNING:  " + e.GetFullMessage() + " in " + GetName() + "\n");
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
bool SolverString::RenameRefObject(const UnsignedInt type,
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
std::string SolverString::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = SolverData::GetRefObjectName(type);

   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("SolverString::GetRefObjectName() " +
          GmatBase::GetObjectTypeString(type) +
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
 * @param type object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray& SolverString::GetRefObjectNameArray(const UnsignedInt type)
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
 * @param type object type
 * @param name object name
 *
 */
//------------------------------------------------------------------------------
bool SolverString::SetRefObjectName(const UnsignedInt type,
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
GmatBase* SolverString::GetRefObject(const UnsignedInt type,
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
 * @param obj  reference object pointer
 * @param type object type
 * @param name object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool SolverString::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   return SolverData::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(...)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const std::string& SolverString::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("SolverString::GetGeneratingString() this=<%p>'%s' entered, mode=%d, prefix='%s', "
       "useName='%s'\n", this, GetName().c_str(), mode, prefix.c_str(), useName.c_str());
   MessageInterface::ShowMessage
      ("   mExpr='%s', mDepObjectName='%s'\n", mExpr.c_str(), mDepObjectName.c_str());
   #endif

   // We want to skip StringVar::GetGeneratingString() since it is handled specially
   // for String
   return Parameter::GetGeneratingString(mode, prefix, useName);
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
const std::string SolverString::GetExternalCloneName(Integer whichOne)
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
void SolverString::SetExternalClone(GmatBase *clone)
{
   if (clone == NULL)
      throw ParameterException("External clone pointer is NULL");

   SolverData::AddRefObject(clone->GetType(), clone->GetName(), clone, true);
}

