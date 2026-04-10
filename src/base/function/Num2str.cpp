//$Id$
//------------------------------------------------------------------------------
//                           Num2str
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
// Created: Apr 13, 2018
/**
 * 
 */
//------------------------------------------------------------------------------

#include "Num2str.hpp"
#include "FunctionException.hpp"
#include "RealUtilities.hpp"       // for Num2str()
#include "MessageInterface.hpp"
#include "StringObjectWrapper.hpp"

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
//  Num2str(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Num2str object (default constructor).
 *
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Num2str::Num2str(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("Num2str");

   // Build input and output arrays. Function interface is:
   // Num2str(time)

   // Add dummy input names
   inputNames.push_back("__Num2str_input_1_num__");
   inputArgMap.insert(std::make_pair("__Num2str_input_1_num__", (ElementWrapper*) NULL));

   // Add dummy output names
   outputNames.push_back("__Num2str_output_1_str__");
   outputArgMap.insert(std::make_pair("__Num2str_output_1_str__", (ElementWrapper*) NULL));
   outputWrapperTypes.push_back(Gmat::STRING_OBJECT_WT);
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);
}

//------------------------------------------------------------------------------
//  ~Num2str(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Num2str object (destructor).
 */
//------------------------------------------------------------------------------
Num2str::~Num2str()
{
}


//------------------------------------------------------------------------------
//  Num2str(const Num2str &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Num2str object (copy constructor).
 *
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Num2str::Num2str(const Num2str &f) :
   BuiltinGmatFunction(f)
{
}


//------------------------------------------------------------------------------
//  Num2str& operator=(const Num2str &f)
//------------------------------------------------------------------------------
/**
 * Sets one Num2str object to match another (assignment operator).
 *
 * @param <f> The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Num2str& Num2str::operator=(const Num2str &f)
{
   if (this == &f)
      return *this;

   BuiltinGmatFunction::operator=(f);

   return *this;
}


//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray Num2str::GetOutputTypes(IntegerArray &rowCounts,
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
void Num2str::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Num2str::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for Num2str
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Num2str::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Num2str::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Num2str::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Num2str::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 1 input
   if (inputArgMap.size() != 1)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Num2str::Execute() returning false, size of inputArgMap:(%d) "
          "is not 1\n", inputArgMap.size());
      #endif
      return false;
   }

   // Check for output info, there shoud be 1 output
   // It is an internal coding error if not 1
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 1)
   {
      if (outputArgMap.size() != 1)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("Num2str::Execute() returning false, size of outputArgMap: %d or "
             "outputWrapperTypes: %d are not 1\n",outputArgMap.size(),  outputWrapperTypes.size());
         #endif
      }
      return false;
   }

   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Num2str::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   std::string msg;
   GmatBase *obj = NULL;
   GmatBase *input1_variable = NULL;

   msg = "";
   for (unsigned int i = 0; i < inputNames.size(); i++)
   {
      std::string objName = inputNames[i];
      ObjectMap::iterator objIter = objectStore->find(objName);
      if (objIter != objectStore->end())
      {
         obj = objIter->second;
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("   input[%d] = <%p><%s>'%s'\n", i, obj, obj ? obj->GetTypeName().c_str() : "NULL",
             obj ? obj->GetName().c_str() : "NULL");
         #endif
         if (obj == NULL)
         {
            msg = msg + "Cannot find the object '" + objName + "' in the objectStore\n";
         }
         else
         {
            Gmat::ParameterType retType = ((Parameter*)obj)->GetReturnType();
            if (retType == Gmat::INTEGER_TYPE || retType == Gmat::UNSIGNED_INT_TYPE ||
                retType == Gmat::REAL_TYPE || retType == Gmat::REAL_ELEMENT_TYPE)
            {
               input1_variable = obj;
            }
            else
            {
               msg = msg + "The object '" + objName + "' is not a valid input type; "
                     "It is expecting a numeric type\n";
            }
         }
      }
   }

   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Num2str::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_variable == NULL)
   {
      throw FunctionException
         ("Num2str::Execute()  input1_variable remains unset\n");
   }

   Real inputNum = 0;
   // Set objects to passed in input values
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif
   try
   {
      inputNum = inputArgMap[input1_variable->GetName()]->EvaluateReal();

      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   inputStr = %lf, checking valid input\n", inputNum);
      #endif
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
   MessageInterface::ShowMessage
      ("inputNum: %lf\n", inputNum);
   #endif

   std::string outputStr = std::to_string(inputNum);

   // Output
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();

   ElementWrapper *outWrapper;

   outWrapper = CreateOutputVariableWrapper(outputStr, ewi->first);

   if (!outWrapper)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif

   //Set the output.
   ewi->second = outWrapper;



   return true;
}

ElementWrapper* Num2str::CreateOutputVariableWrapper(std::string outputStr, const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Num2str::CreateOutputVariableWrapper() entered, a1MjdEpoch=%.12f, outName='%s'\n",
       a1MjdEpoch, outName.c_str());
   #endif

   // Find StringVar object with outName
   ObjectMap::iterator objIter = objectStore->find(outName);
   GmatBase *obj = NULL;
   StringVar *outStringVar = NULL;
   if (objIter != objectStore->end())
   {
      obj = objIter->second;
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   outName = <%p><%s>'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      #endif
      outStringVar = (StringVar*)obj;
      outStringVar->SetString(outputStr);
   }

   // Create StringObjectWrapper
   ElementWrapper *outWrapper = new StringObjectWrapper();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "Num2str::CreateOutputVariableWrapper()",
       "outWrapper = new VariableWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outString to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outStringVar);

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Num2str::CreateOutputVariableWrapper() returning warpper <%p>\n",
       outWrapper);
   #endif

   return outWrapper;
}


//------------------------------------------------------------------------------
// void Num2str::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void Num2str::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Num2str::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Num2str.
 *
 * @return clone of the Num2str.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Num2str::Clone() const
{
   return (new Num2str(*this));
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
void Num2str::Copy(const GmatBase* orig)
{
   operator=(*((Num2str *)(orig)));
}













