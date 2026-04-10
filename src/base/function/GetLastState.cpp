//$Id$
//------------------------------------------------------------------------------
//                                GetLastState
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
 * Implements GetLastState class.
 * [state] = GetLastState(spacecraft)
 *   Input:
 *    - spacecraft is a spacecraft object name
 *   Output:
 *    - state is 6x1 arrays of current Cartesian state of spacecraft object
 */
//------------------------------------------------------------------------------

#include "GetLastState.hpp"
#include "ArrayWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// public
//---------------------------------
//------------------------------------------------------------------------------
//  GetLastState(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the GetLastState object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
GetLastState::GetLastState(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   if (typeStr != "GetLastState")
      objectTypeNames.push_back("GetLastState");
   
   // Build input and output arrays. Function interface is:
   // [state] = GetLastState(spacecraft)
   
   // Add dummy input names
   inputNames.push_back("__BuiltinFunction_GetLastState_input_1_spacecraft__");
   inputArgMap.insert(std::make_pair("__BuiltinFunction_GetLastState_input_1_spacecraft__", (ElementWrapper*) NULL));
   
   // Add dummy output names
   outputNames.push_back("__BuiltinFunction_GetLastState_output_1_state__");
   outputArgMap.insert(std::make_pair("__BuiltinFunction_GetLastState_output_1_state__", (ElementWrapper*) NULL));
   
   // Add output types
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);
   
   // Add output row and column counts
   // state array has 6x1
   outputRowCounts.push_back(6);
   outputColCounts.push_back(1);   
}


//------------------------------------------------------------------------------
//  ~GetLastState(void)
//------------------------------------------------------------------------------
/**
 * Destroys the GetLastState object (destructor).
 */
//------------------------------------------------------------------------------
GetLastState::~GetLastState()
{
}


//------------------------------------------------------------------------------
//  GetLastState(const GetLastState &f)
//------------------------------------------------------------------------------
/**
 * Constructs the GetLastState object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
GetLastState::GetLastState(const GetLastState &f) :
   BuiltinGmatFunction(f)
{
}


//------------------------------------------------------------------------------
//  GetLastState& operator=(const GetLastState &f)
//------------------------------------------------------------------------------
/**
 * Sets one GetLastState object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
GetLastState& GetLastState::operator=(const GetLastState &f)
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
WrapperTypeArray GetLastState::GetOutputTypes(IntegerArray &rowCounts,
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
void GetLastState::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("GetLastState::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif
   
   // Set output wrapper type for GetLastState
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;   
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool GetLastState::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GetLastState::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   BuiltinGmatFunction::Initialize(objInit);
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GetLastState::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool GetLastState::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GetLastState::Execute() <%p>'%s' entered\n", this, GetName().c_str());
   ShowObjectMap(objectStore, "", "objectStore");
   MessageInterface::ShowMessage
      ("   outputArgMap.size() = %d\n   outputWrapperTypes.size() = %d\n",
       outputArgMap.size(), outputWrapperTypes.size());
   MessageInterface::ShowMessage
      ("   outputRowCounts.size() = %d\n   outputColCounts() = %d\n",
       outputRowCounts.size(), outputColCounts.size());
   #endif
   
   // Check for output info, there should be 1 output
   // It is an internal coding error if not 1
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 1)
   {
      if (outputArgMap.size() != 1)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("GetLastState::Execute() returning false, size of outputArgMap: &d or "
             "outputWrapperTypes: %d are not 2\n",outputArgMap.size(),  outputWrapperTypes.size());
         #endif
      }
      return false;
   }
   
   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetLastState::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }
   
   // Check if input names are in the objectStore
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Checking if inputs exist in the objectStore\n");
   #endif
   std::string msg;
   GmatBase *obj = NULL;
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
            // Check for spacecraft type
            if (!obj->IsOfType(Gmat::SPACECRAFT))
            {
               msg = msg + "The object '" + objName + "' is not valid type to retrieve GetLastState()\n";
            }
         }
      }
   }
   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetLastState::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      return false;
   }
   
   
   #ifdef DEBUG_FUNCTION_EXEC
   for (unsigned int i = 0; i < outputWrapperTypes.size(); i++)
      MessageInterface::ShowMessage
         ("   outputWrapperTypes[%d] = %d\n", i, outputWrapperTypes[i]);
   #endif
   
   // Get last state
   Rvector6 state = ((SpacePoint*)obj)->GetLastState();
   
   // Only 1 output
   int index = 0;
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("==================== index=%d\n", index);
   MessageInterface::ShowMessage
      ("   ewi->first  = '%s', ewi->second = <%p>\n", (ewi->first).c_str(),
       ewi->second);
   #endif
   
   // Output array type is ARRAY_WT
   int numRows = outputRowCounts[index];
   int numCols = outputColCounts[index];
   Rmatrix rmat;
   rmat.SetSize(numRows, numCols);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   It is ARRAY_WT, numRows = %d, numCols = %d\n", numRows, numCols);
   #endif
   
   // Set state
   for (int jj = 0; jj < numRows; jj++)
      rmat.SetElement(jj, 0, state(jj));
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   rmat = %s", rmat.ToString().c_str());
   #endif
   
   // Create Array, this array will be deleted when ArrayWrapper is deleted
   Array *outArray = new Array(ewi->first);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outArray, "outArray", "GetLastState::Execute()",
       "outArray = new Array()");
   #endif
   
   outArray->SetSize(numRows, numCols);
   outArray->SetRmatrix(rmat);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Creating ArrayWrapper for output\n");
   #endif
   
   // Create ArrayWrapper
   ElementWrapper *outWrapper = new ArrayWrapper();         
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "GetLastState::Execute()",
       "outWrapper = new ArrayWrapper()");
   #endif
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting size and outArray to outWrapper\n");
   #endif
   outWrapper->SetDescription(ewi->first);
   outWrapper->SetRefObject(outArray);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting ArrayWrapper to outputWrapperMap\n");
   #endif
   ewi->second = outWrapper;
  
   #ifdef DEBUG_FUNCTION_EXEC
   for (ewi = outputArgMap.begin(); ewi != outputArgMap.end(); ++ewi)
      MessageInterface::ShowMessage
         ("   ewi->first  = '%s', ewi->second = <%p>\n", (ewi->first).c_str(),
          ewi->second);
   MessageInterface::ShowMessage
      ("GetLastState::Execute() <%p>'%s' returning true\n", this,
       GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void GetLastState::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void GetLastState::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("GetLastState::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the GetLastState.
 *
 * @return clone of the GetLastState.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GetLastState::Clone() const
{
   return (new GetLastState(*this));
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
void GetLastState::Copy(const GmatBase* orig)
{
   operator=(*((GetLastState *)(orig)));
}

