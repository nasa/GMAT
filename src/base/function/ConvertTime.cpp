//$Id$
//------------------------------------------------------------------------------
//                           ConvertTime
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
// Created: Mar 9, 2018
/**
 * 
 */
//------------------------------------------------------------------------------

#include "ConvertTime.hpp"
#include "FunctionException.hpp"
#include "RealUtilities.hpp"       // for ConvertTime()
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
//  ConvertTime(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the ConvertTime object (default constructor).
 *
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
ConvertTime::ConvertTime(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("ConvertTime");

   // Build input and output arrays. Function interface is:
   // ConvertTime(time)

   // Add dummy input names
   inputNames.push_back("__ConvertTime_input_1_startformat__");
   inputArgMap.insert(std::make_pair("__ConvertTime_input_1_startformat__", (ElementWrapper*) NULL));
   inputNames.push_back("__ConvertTime_input_2_endformat__");
   inputArgMap.insert(std::make_pair("__ConvertTime_input_2_endformat__", (ElementWrapper*) NULL));
   inputNames.push_back("__ConvertTime_input_3_time__");
   inputArgMap.insert(std::make_pair("__ConvertTime_input_3_time__", (ElementWrapper*) NULL));

   // Add dummy output names
   outputNames.push_back("__ConvertTime_output_1_time__");
   outputArgMap.insert(std::make_pair("__ConvertTime_output_1_time__", (ElementWrapper*) NULL));
   outputWrapperTypes.push_back(Gmat::STRING_OBJECT_WT);
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);

   theTimeConverter = TimeSystemConverter::Instance();
}

//------------------------------------------------------------------------------
//  ~ConvertTime(void)
//------------------------------------------------------------------------------
/**
 * Destroys the ConvertTime object (destructor).
 */
//------------------------------------------------------------------------------
ConvertTime::~ConvertTime()
{
}


//------------------------------------------------------------------------------
//  ConvertTime(const ConvertTime &f)
//------------------------------------------------------------------------------
/**
 * Constructs the ConvertTime object (copy constructor).
 *
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
ConvertTime::ConvertTime(const ConvertTime &f) :
   BuiltinGmatFunction(f)
{
   theTimeConverter = f.theTimeConverter;
}


//------------------------------------------------------------------------------
//  ConvertTime& operator=(const ConvertTime &f)
//------------------------------------------------------------------------------
/**
 * Sets one ConvertTime object to match another (assignment operator).
 *
 * @param <f> The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
ConvertTime& ConvertTime::operator=(const ConvertTime &f)
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
WrapperTypeArray ConvertTime::GetOutputTypes(IntegerArray &rowCounts,
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
void ConvertTime::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("ConvertTime::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for ConvertTime
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool ConvertTime::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("ConvertTime::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("ConvertTime::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool ConvertTime::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 3 inputs
   if (inputArgMap.size() != 3)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("ConvertTime::Execute() returning false, size of inputArgMap:(%d) "
          "is not 1\n", inputArgMap.size());
      #endif
      return false;
   }

   // Check for output info, there should be 1 output
   // It is an internal coding error if not 1
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 1)
   {
      if (outputArgMap.size() != 1)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("ConvertTime::Execute() returning false, size of outputArgMap: %d or "
             "outputWrapperTypes: %d are not 4\n",outputArgMap.size(),  outputWrapperTypes.size());
         #endif
      }
      return false;
   }

   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("ConvertTime::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   std::string msg;
   GmatBase *obj = NULL;
   GmatBase *input1_startFormat = NULL;
   GmatBase *input2_endFormat = NULL;
   GmatBase *input3_timeNum = NULL;
   GmatBase *input3_timeString = NULL;

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
            if (i == 0 || i == 1) {
               if (obj->IsOfType(Gmat::STRING))
               {
                  if (i == 0)
                  {
                     input1_startFormat = obj;
                  }
                  else
                  {
                     input2_endFormat = obj;
                  }
               }
               else
               {
                  msg = msg + "The object '" + objName + "' is not valid input type; "
                        "It is expecting a String\n";
               }
            }
            else if (i == 2)
            {
               Gmat::ParameterType retType = ((Parameter*)obj)->GetReturnType();
               if (retType == Gmat::INTEGER_TYPE || retType == Gmat::UNSIGNED_INT_TYPE ||
                   retType == Gmat::REAL_TYPE || retType == Gmat::REAL_ELEMENT_TYPE)
               {
                  input3_timeNum = obj;
               }
               else if (obj -> IsOfType(Gmat::STRING))
               //else if (retType == Gmat::STRING)
               {
                  input3_timeString = obj;
               }
               else
               {
                  msg = msg + "The object '" + objName + "' is not valid input type; "
                        "It is expecting a numeric or string type\n";
               }
            }
         }
      }
   }


   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("ConvertTime::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_startFormat == NULL) // if inputNames.size() == 0
   {
         throw FunctionException
            ("ConvertTime::Execute()  input1_startFormat remains unset\n");
   }

   if (input2_endFormat == NULL)
   {
      throw FunctionException
         ("ConvertTime::Execute()  input2_endFormat remains unset\n");
   }

   if (input3_timeNum == NULL && input3_timeString == NULL)
   {
      throw FunctionException
         ("ConvertTime::Execute()  input3_time remains unset\n");
   }

   timeNum = -999.999;
   timeString = "";
   // Set objects to passed in input values
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif
   try
   {
      startFormat = inputArgMap[input1_startFormat->GetName()]->EvaluateString();
      endFormat = inputArgMap[input2_endFormat->GetName()]->EvaluateString();
      if (input3_timeNum != NULL)
         timeNum = inputArgMap[input3_timeNum->GetName()]->EvaluateReal();
      else
         timeString = inputArgMap[input3_timeString->GetName()]->EvaluateString();


      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
      //   ("   formatString = %f, checking valid input\n", formatString);
         ("   formatString = %.12f, checking valid input\n", timeNum);
      #endif

      std::list<std::string> validFormats{"A1ModJulian", "TAIModJulian",
         "UTCModJulian", "TDBModJulian", "TTModJulian", "A1Gregorian",
         "TAIGregorian", "UTCGregorian", "TDBGregorian", "TTGregorian"};

      //Error if the format string is not in the list of valid formats.
      if (std::find(validFormats.begin(), validFormats.end(), startFormat) == validFormats.end())
      {
         throw FunctionException
            ("First input to ConvertTime() function is not a recognized format");
      }
      if (std::find(validFormats.begin(), validFormats.end(), endFormat) == validFormats.end())
      {
         throw FunctionException
            ("Second input to ConvertTime() function is not a recognized format");
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
   MessageInterface::ShowMessage
      ("timeNum: %.12lf\ntimeString: %s\n", timeNum, timeString.c_str());
   #endif

   // Output
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();

   Real toNum;
   std::string toString;
   ElementWrapper *outWrapper;

   theTimeConverter->Convert(startFormat, timeNum, timeString, endFormat, toNum, toString, 1);
   outWrapper = CreateOutputEpochWrapper(toNum, toString, ewi->first);

   if (!outWrapper)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif

   //Set the output.
   ewi->second = outWrapper;

   return true;
}

ElementWrapper* ConvertTime::CreateOutputEpochWrapper(Real MjdEpoch, std::string epochStr,
                                                         const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("ConvertTime::CreateOutputEpochWrapper() entered, a1MjdEpoch=%.12f, outName='%s'\n",
       a1MjdEpoch, outName.c_str());
   #endif

   // Find StringVar object with outName
   ObjectMap::iterator objIter = objectStore->find(outName);
   GmatBase *obj = NULL;
   StringVar *outString = NULL;
   if (objIter != objectStore->end())
   {
      obj = objIter->second;
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   outName = <%p><%s>'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      #endif
      outString = (StringVar*)obj;
      outString->SetString(epochStr);
   }

   // Create StringObjectWrapper
   ElementWrapper *outWrapper = new StringObjectWrapper();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "ConvertTime::CreateOutputEpochWrapper()",
       "outWrapper = new StringObjectWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outString to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outString);

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("ConvertTime::CreateOutputEpochWrapper() returning wrpper <%p>\n",
       outWrapper);
   #endif

   return outWrapper;
}


//------------------------------------------------------------------------------
// void ConvertTime::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void ConvertTime::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("ConvertTime::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the ConvertTime.
 *
 * @return clone of the ConvertTime.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ConvertTime::Clone() const
{
   return (new ConvertTime(*this));
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
void ConvertTime::Copy(const GmatBase* orig)
{
   operator=(*((ConvertTime *)(orig)));
}
