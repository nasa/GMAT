//$Id$
//------------------------------------------------------------------------------
//                                  VariableWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.18
//
/**
 * Implementation of the VariableWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "VariableWrapper.hpp"
#include "ParameterException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_VAR_WRAPPER_GET
//#define DEBUG_VAR_WRAPPER_SET
//#define DEBUG_VAR_WRAPPER_EVAL
//#define DEBUG_REF_OBJ

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  VariableWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs VariableWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
VariableWrapper::VariableWrapper() :
   ElementWrapper(),
   var           (NULL)
{
   wrapperType = Gmat::VARIABLE_WT;
}

//---------------------------------------------------------------------------
//  VariableWrapper(const VariableWrapper &vw);
//---------------------------------------------------------------------------
/**
 * Constructs base VariableWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <vw>  VariableWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
VariableWrapper::VariableWrapper(const VariableWrapper &vw) :
   ElementWrapper(vw),
   var           (NULL)
{
   if (vw.var)
   {
      var = (Variable*)((vw.var)->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (var, var->GetName(), "VariableWrapper copy constructor",
          "var = (Variable*)((vw.var)->Clone())");
      #endif
   }
}

//---------------------------------------------------------------------------
//  VariableWrapper& operator=(const VariableWrapper &vw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for VariableWrapper structures.
 *
 * @param <vw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const VariableWrapper& VariableWrapper::operator=(const VariableWrapper &vw)
{
   if (&vw == this)
      return *this;
   
   ElementWrapper::operator=(vw);
   
   if (var)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (var, var->GetName(), "VariableWrapper operator=", "deleting old var");
      #endif
      delete var;
   }
   
   var = NULL;
   
   if (vw.var)
   {
      var = (Variable*)((vw.var)->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (var, var->GetName(), "VariableWrapper operator=",
          "var = (Variable*)((vw.var)->Clone())");
      #endif
   }
   
   return *this;
}


//---------------------------------------------------------------------------
//  ~VariableWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
VariableWrapper::~VariableWrapper()
{
   ///@todo deleting var causes crash
   #ifdef __ENABLE_CLONING_REFOBJ__
   if (var)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (var, var->GetName(), "VariableWrapper destructor", "deleting var");
      #endif
      delete var;
   }
   #endif
}


//------------------------------------------------------------------------------
// virtual ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
ElementWrapper* VariableWrapper::Clone() const
{
   return new VariableWrapper(*this);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the VariableWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType VariableWrapper::GetDataType() const
{
   return Gmat::REAL_TYPE;
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return VariableWrapper value converted to std::string.
 *
 * @exception <GmatBaseException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string VariableWrapper::ToString()
{
   return var->ToString();
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * This method retrives a reference object for the wrapper name
 * 
 * @param <name> name of the wrapper
 *
 * @return reference for success; NULL if name not found
 *
 */
//------------------------------------------------------------------------------
GmatBase* VariableWrapper::GetRefObject(const std::string &name)
{
   return var;
}


//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (Variable) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool VariableWrapper::SetRefObject(GmatBase *obj)
{
   if (obj == NULL)
      return false;
   
   #ifdef DEBUG_REF_OBJ
   MessageInterface::ShowMessage
      ("VariableWrapper::SetRefObject() <%p>'%s' entered, obj=<%p><%s>'%s', "
       "var=<%p>\n", this, description.c_str(), obj, obj->GetTypeName().c_str(),
       obj->GetName().c_str(), var);
   #endif
   
//   if ( (obj->GetName() != refObjectNames[0]) ||
//        (obj->GetTypeName() != "Variable") )
//   {
//      std::string errmsg = "Referenced variable \"";
//      errmsg += obj->GetName();
//      errmsg += ("\" of type \"" + obj->GetTypeName());
//      errmsg += "\" was passed into the variable wrapper \"";
//      errmsg += description;
//      errmsg += "\", but an object named \"";
//      errmsg += refObjectNames[0];
//      errmsg += "\" was expected.\n";
//      throw ParameterException(errmsg);
//   }
   
   if ((obj->GetName() == refObjectNames[0]) && (obj->IsOfType("Variable")) )
   {
      ///@todo if we clone the ref. Variable, math in script does not work,
      // such as RouttineTests/APT_Cart2KepMathTest.script.
      // This script creates Variables and set values in command mode, that
      // means Assignment command's lhs wrappers' ref object should be the same
      // as this VariableWrappers's ref. Variable, so we cannot clone the
      // ref. Variable here. (LOJ: 2009.03.12)
      
#ifdef __ENABLE_CLONING_REFOBJ__
      if (var)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (var, var->GetName(), "VariableWrapper::SetRefObject()",
             "deleting old var");
         #endif
         delete var;
      }
      
      #ifdef DEBUG_REF_OBJ
      MessageInterface::ShowMessage("   Now cloning obj and setting to var\n");
      #endif
      
      var = (Variable*)(obj->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (var, var->GetName(), "VariableWrapper::SetRefObject()",
          "var = (Variable*)(obj->Clone()");
      #endif

#else
      var = (Variable*) obj;
#endif
      
      #ifdef DEBUG_REF_OBJ
      MessageInterface::ShowMessage
         ("VariableWrapper::SetRefObject() Set var to <%p>, returning true\n", var);
      #endif
      return true;
   }
   else  return false;
}

//---------------------------------------------------------------------------
//  bool RenameObject(const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Method to rename a reference object for the wrapper.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool VariableWrapper::RenameObject(const std::string &oldName, 
                                   const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   description = refObjectNames[0];
   return true;
}


//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real VariableWrapper::EvaluateReal() const
{
   if (var == NULL)
      throw ParameterException(
      "Cannot return value of Variable - pointer is NULL\n");
   
   #ifdef DEBUG_VAR_WRAPPER_EVAL
   MessageInterface::ShowMessage
      ("In VarWrapper::EvalReal() <%p>'%s', var=<%p>'%s' - value is %.12f\n",
       this, description.c_str(), var, var->GetName().c_str(), (var->EvaluateReal()));
   #endif
   
   return var->EvaluateReal();
}

//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool VariableWrapper::SetReal(const Real toValue)
{
   #ifdef DEBUG_VAR_WRAPPER_SET
   MessageInterface::ShowMessage
      ("In VarWrapper::SetReal() <%p>'%s', var=<%p>'%s' - setting to %.12f\n",
       this, description.c_str(), var, var ? var->GetName().c_str() : "NULL", toValue);
   #endif
   if (var == NULL)
      throw ParameterException(
      "Cannot set value of Variable - pointer is NULL\n");
   var->SetReal(toValue);
   return true;
}

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Variable Wrapper.
 *
 */
//---------------------------------------------------------------------------
void VariableWrapper::SetupWrapper()
{
   refObjectNames.push_back(description);
}
