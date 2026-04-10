//$Id$
//------------------------------------------------------------------------------
//                                   BuiltinGmatFunction
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
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.05.02
//
/**
 * Implements BuiltinGmatFunction class.
 */
//------------------------------------------------------------------------------

#include "BuiltinGmatFunction.hpp"
#include "RealUtilities.hpp"       // for Rand()
#include "StringUtil.hpp"          // for ToString()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_INIT


//---------------------------------
// public
//---------------------------------
//------------------------------------------------------------------------------
//  BuiltinGmatFunction(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinGmatFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::BuiltinGmatFunction(const std::string &typeStr, const std::string &name) :
   ObjectManagedFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("BuiltinGmatFunction");
}


//------------------------------------------------------------------------------
//  ~BuiltinGmatFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the BuiltinGmatFunction object (destructor).
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::~BuiltinGmatFunction()
{
}


//------------------------------------------------------------------------------
//  BuiltinGmatFunction(const BuiltinGmatFunction &f)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinGmatFunction object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::BuiltinGmatFunction(const BuiltinGmatFunction &f) :
   ObjectManagedFunction(f)
{
}


//------------------------------------------------------------------------------
//  BuiltinGmatFunction& operator=(const BuiltinGmatFunction &f)
//------------------------------------------------------------------------------
/**
 * Sets one BuiltinGmatFunction object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction& BuiltinGmatFunction::operator=(const BuiltinGmatFunction &f)
{
   if (this == &f)
      return *this;
   
   ObjectManagedFunction::operator=(f);
   
   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool BuiltinGmatFunction::Initialize(ObjectInitializer *objInit,
                                     bool reinitialize, bool resetLocalObjects)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("BuiltinGmatFunction::Initialize() entered, callCount=%d\n", callCount);
   ShowTrace(callCount, t1, "BuiltinGmatFunction::Initialize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "BuiltinGmatFunction::Initialize() entered for function '%s'\n   reinitialize=%d\n",
       functionName.c_str(), reinitialize);
   MessageInterface::ShowMessage("   internalCS is %p\n", internalCoordSys);
   #endif
   
   ObjectManagedFunction::Initialize(objInit, reinitialize, resetLocalObjects);
   isInitialized = true;
   return true;
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool BuiltinGmatFunction::SetStringParameter(const Integer id, const std::string &value)
{
   return ObjectManagedFunction::SetStringParameter(id, value);
}

