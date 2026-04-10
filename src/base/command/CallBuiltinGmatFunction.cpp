//$Id$
//------------------------------------------------------------------------------
//                                 CallBuiltinGmatFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Linda Jun
// Created: 2016.05.03
//
/**
 * Implementation for the CallBuiltinGmatFunction command class
 */
//------------------------------------------------------------------------------
#include "CallBuiltinGmatFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_CALL_FUNCTION_INIT
//#define DEBUG_CALL_FUNCTION_EXEC
//#define DEBUG_RUN_COMPLETE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
// CallBuiltinGmatFunction::CallBuiltinGmatFunction()
//------------------------------------------------------------------------------
CallBuiltinGmatFunction::CallBuiltinGmatFunction() :
   CallFunction("CallBuiltinGmatFunction")
{
   objectTypeNames.push_back("CallBuiltinGmatFunction");
}


//------------------------------------------------------------------------------
// ~CallBuiltinGmatFunction()
//------------------------------------------------------------------------------
CallBuiltinGmatFunction::~CallBuiltinGmatFunction()
{
}


//------------------------------------------------------------------------------
// CallBuiltinGmatFunction(const CallBuiltinGmatFunction& cf)
//------------------------------------------------------------------------------
CallBuiltinGmatFunction::CallBuiltinGmatFunction(const CallBuiltinGmatFunction& cf) :
   CallFunction (cf)
{
}


//------------------------------------------------------------------------------
// CallBuiltinGmatFunction& operator=(const CallBuiltinGmatFunction& cf)
//------------------------------------------------------------------------------
CallBuiltinGmatFunction& CallBuiltinGmatFunction::operator=(const CallBuiltinGmatFunction& cf)
{
   if (this == &cf)
      return *this;
   
   CallFunction::operator=(cf);
   
   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallBuiltinGmatFunction.
 *
 * @return clone of the CallBuiltinGmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallBuiltinGmatFunction::Clone() const
{
   return (new CallBuiltinGmatFunction(*this));
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallBuiltinGmatFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::Initialize() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   isGmatFunction = %d, isBuiltinGmatFunction = %d\n", isGmatFunction,
       isBuiltinGmatFunction);
   #endif
   
   bool rv = CallFunction::Initialize();
   
   // Handle additional initialization for BuiltinGmatFunctions
   if (isBuiltinGmatFunction)
   {
      #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("CallBuiltinGmatFunction::Initialize() Setting solarSys, forces, gom to "
          "function manager '%s'\n", mFunction->GetName().c_str());
      #endif
      fm.SetSolarSystem(solarSys);
      fm.SetTransientForces(forces);
      fm.SetGlobalObjectMap(globalObjectMap);

      for (UnsignedInt i = 0; i < mOutputNames.size(); ++i)
         mFunction->SetOutputName(mOutputNames[i]);
   }
   
   #ifdef DEBUG_CALL_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::Initialize() this=<%p> returning %d\n", this, rv);
   #endif
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallBuiltinGmatFunction::Execute()
{
   bool status = false;
   
   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallBuiltinGmatFunction");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: CallBuiltinGmatFunction::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::Execute() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the start");
      #endif
   #endif

   if (isBuiltinGmatFunction)
   {
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Function is BuiltinGmatFunction\n");
      #endif
      status = fm.Execute(callingFunction);
   }
   else
   {
      // The function is not a BuiltinGmatFunction so throw an exception
      throw CommandException
         ("*** INTERNAL ERROR *** in CallBuiltinGmatFunction. " +
          GetGeneratingString(Gmat::NO_COMMENTS) + " cannot be executed, "
          "the function type should be GmatFunction or BuiltinGmatFunction, but it is " +
          mFunction->GetTypeName() + ".\n");
   }
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   Executed command, about to build command summery\n");
   #endif
   
   BuildCommandSummary(true);
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::Execute() GmatFunction exiting with %d\n", status);
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the end");
      #endif
   #endif
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: CallBuiltinGmatFunction::Execute() exiting, '%s' Count = %d, Run Time: %f seconds\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void CallBuiltinGmatFunction::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::RunComplete() entered, this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
   
   // if (!fm.IsFinalized())
   // {
   //    #ifdef DEBUG_RUN_COMPLETE
   //    MessageInterface::ShowMessage("   Calling FunctionManager::Finalize()\n");
   //    #endif
   //    fm.Finalize();
   // }
   
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage("   Calling CallFunction::RunComplete()\n");
   #endif
   
   CallFunction::RunComplete();
   
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallBuiltinGmatFunction::RunComplete() leaving, this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
}

