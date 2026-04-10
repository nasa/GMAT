//$Id$
//------------------------------------------------------------------------------
//                           Pause
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Jan 19, 2018
/**
 * Function to pause execution in GMAT for a set time interval
 */
//------------------------------------------------------------------------------

#include "Pause.hpp"
#include "FunctionException.hpp"
#include "RealUtilities.hpp"       // for Pause()
#include "MessageInterface.hpp"

#include <iostream>
#include <chrono>
#include <thread>

//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
//  Pause(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Pause object (default constructor).
 *
 * @param typeStr String text identifying the object type
 * @param name    Name for the object
 */
//------------------------------------------------------------------------------
Pause::Pause(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("Pause");

   // Build input and output arrays. Function interface is:
   // Pause(time)

   // Add dummy input names
   inputNames.push_back("__Pause_input_1_time__");
   inputArgMap.insert(std::make_pair("__Pause_input_1_time__", (ElementWrapper*) NULL));

   // Pause has no output
}

//------------------------------------------------------------------------------
//  ~Pause()
//------------------------------------------------------------------------------
/**
 * Destroys the Pause object (destructor).
 */
//------------------------------------------------------------------------------
Pause::~Pause()
{
}


//------------------------------------------------------------------------------
//  Pause(const Pause &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Pause object (copy constructor).
 *
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Pause::Pause(const Pause &f) :
   BuiltinGmatFunction  (f),
   pauseValue           (f.pauseValue),
   pauseReal            (f.pauseReal)
{
}


//------------------------------------------------------------------------------
//  Pause& operator=(const Pause &f)
//------------------------------------------------------------------------------
/**
 * Sets one Pause object to match another (assignment operator).
 *
 * @param f The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Pause& Pause::operator=(const Pause &f)
{
   if (this != &f)
   {
      BuiltinGmatFunction::operator=(f);
      pauseValue = f.pauseValue;
      pauseReal  = f.pauseReal;
   }

   return *this;
}


//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray Pause::GetOutputTypes(IntegerArray &rowCounts,
                                       IntegerArray &colCounts) const
{
   rowCounts = outputRowCounts;
   colCounts = outputColCounts;
   return outputWrapperTypes;
}

//------------------------------------------------------------------------------
// virtual void SetOutputTypes(WrapperTypeArray &outputTypes,
//                             IntegerArray &rowCounts, IntegerArray &colCounts)
//------------------------------------------------------------------------------
/*
 * Sets function output types. This method is called when parsing the function
 * file from the Interpreter.
 */
//------------------------------------------------------------------------------
void Pause::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Pause::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for Pause
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Pause::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Pause::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Pause::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Pause::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("Pause::Execute() <%p>'%s' entered\n", this, GetName().c_str());
   ShowObjects("");
   MessageInterface::ShowMessage("   inputArgMap.size() = %d\n", inputArgMap.size());
   MessageInterface::ShowMessage
      ("   outputArgMap.size() = %d\n   outputWrapperTypes.size() = %d\n",
       outputArgMap.size(), outputWrapperTypes.size());
   MessageInterface::ShowMessage
      ("   outputRowCounts.size() = %d\n   outputColCounts() = %d\n",
       outputRowCounts.size(), outputColCounts.size());
   #endif

   // Check for input info, there should be 1 inputs
   if (inputArgMap.size() != 1)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetEphemStates::Execute() returning false, size of inputArgMap:(%d) "
          "is not 1\n", inputArgMap.size());
      #endif
      return false;
   }

   // Check for output info (it has no output)
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 0)
   {
      if (outputArgMap.size() != 0)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("Pause::Execute() returning false, size of outputArgMap: &d or "
             "outputWrapperTypes: %d are not zero, Pause requires no output\n",
             outputArgMap.size(), outputWrapperTypes.size());
         #endif
      }
      return false;
   }

   // Check for output row and col counts, both should be empty
   if (!outputRowCounts.empty() && !outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Pause::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are not zero, Pause requires no output\n",
          outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Checking if inputs exist in the objectStore\n");
   #endif
   std::string msg;
   GmatBase *obj = NULL;
   GmatBase *input1_pauseValue = NULL;
   for (unsigned int i = 0; i < inputNames.size(); i++)
   {
      std::string objName = inputNames[i];
      ObjectMap::iterator objIter = objectStore->find(objName);
      if (objIter != objectStore->end())
      {
         obj = objIter->second;
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("   input[%d] = <%p>'%s'\n", i, obj, obj ? obj->GetName().c_str() : "NULL");
         #endif
         if (obj == NULL)
         {
            msg = msg + "Cannot find the object '" + objName + "' in the objectStore\n";
         }
         else
         {
            // Check for input type
            if (!obj->IsOfType(Gmat::PARAMETER))
            {
               msg = "Input to Pause() function requires unsigned integer value";
            }
            else
            {
               #ifdef DEBUG_FUNCTION_EXEC
               MessageInterface::ShowMessage
                  ("   '%s' is a Parameter, checking for Array\n", objName.c_str());
               #endif
               Gmat::ParameterType retType = ((Parameter*)obj)->GetReturnType();
               if (retType == Gmat::INTEGER_TYPE || retType == Gmat::UNSIGNED_INT_TYPE ||
                   retType == Gmat::REAL_TYPE || retType == Gmat::REAL_ELEMENT_TYPE)
               {
                  input1_pauseValue = obj;
               }
               else
               {
                  msg = "Input to Pause() function requires unsigned integer value";
               }
            }
         }
      }
   }

   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Pause::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }
   if (input1_pauseValue == NULL) // if inputNames.size() == 0
   {
         throw FunctionException
            ("Pause::Execute()  input1_pauseValues remains unset\n");
   }

   // Set objects to passed in input values
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif
   try
   {
      pauseReal = inputArgMap[input1_pauseValue->GetName()]->EvaluateReal() * 1000.0;
      pauseValue = int(pauseReal);

      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   pauseValue = %f, checking valid input\n", pauseValue);
      #endif
      if (pauseValue >= 0)
      {
         std::chrono::milliseconds dura(pauseValue);
         std::this_thread::sleep_for(dura);
      }
      else
      {
         throw FunctionException
            ("Input to Pause() function requires unsigned integer value");
      }
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("==> Caught Exception: '%s'\n", be.GetFullMessage().c_str());
      #endif
      std::string msg = be.GetFullMessage();
      be.SetDetails("");
      be.SetMessage(msg + " in \"" + callDescription + "\"");
      throw;
   }

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("Pause::Execute() returning true\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void Pause::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void Pause::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Pause::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Pause.
 *
 * @return clone of the Pause.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Pause::Clone() const
{
   return (new Pause(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Pause::Copy(const GmatBase* orig)
{
   operator=(*((Pause *)(orig)));
}
