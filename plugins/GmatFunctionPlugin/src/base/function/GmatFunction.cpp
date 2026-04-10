//$Id$
//------------------------------------------------------------------------------
//                                  GmatFunction
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
// Author: Allison Greene
// Created: 2004/12/16
//
/**
 * Implementation for the GmatFunction class.
 */
//------------------------------------------------------------------------------

#include "GmatFunction.hpp"
#include "FunctionException.hpp"
#include "Assignment.hpp"        // for Assignment::GetMathTree()
#include "FileManager.hpp"       // for GetGmatFunctionPath()
#include "FileUtil.hpp"          // for ParseFileName(), GetCurrentWorkingDirectory()
#include "StringUtil.hpp"        // for Trim()
#include "CommandUtil.hpp"       // for ClearCommandSeq()
#include "Parameter.hpp"         // for GetReturnType()
#include "BranchCommand.hpp"     // for ResetExecutionState()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_CONSTRUCT
//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_SET_PATH
//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC
//#define DEBUG_FUNCTION_FINALIZE
//#define DEBUG_UNUSED_GOL
//#define DEBUG_OBJECT_MAP

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
//const std::string
//GmatFunction::PARAMETER_TEXT[GmatFunctionParamCount - FunctionParamCount] =
//{
//};
//
//const Gmat::ParameterType
//GmatFunction::PARAMETER_TYPE[GmatFunctionParamCount - FunctionParamCount] =
//{
//};

//---------------------------------
// public
//---------------------------------
//------------------------------------------------------------------------------
// GmatFunction(std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> function name
 */
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const std::string &name) :
   UserDefinedFunction("GmatFunction", name)
{
   #ifdef DEBUG_FUNCTION_CONSTRUCT
   MessageInterface::ShowMessage
      ("GmatFunction::GmatFunction() constructor <%p> entered, name='%s'\n",
       this, name.c_str());
   #endif
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
   
   // for initial function path, use FileManager
   FileManager *fm = FileManager::Instance();
   std::string pathname;
   
   try
   {
      // if there is a function name, try to locate it
      if (name != "")
      {
         #ifdef DEBUG_FUNCTION_CONSTRUCT
         MessageInterface::ShowMessage
            ("   Getting path for '%s' from the FileManager\n", name.c_str());
         #endif
         // Get path of first it is located
         pathname = fm->GetGmatFunctionPath(name + ".gmf");
         // gmat function uses whole path name
         pathname = pathname + name + ".gmf";         
         functionPath = pathname;
         #ifdef DEBUG_FUNCTION_CONSTRUCT
         MessageInterface::ShowMessage
            ("   functionPath now is '%s'\n", functionPath.c_str());
         #endif
         functionName = GmatFileUtil::ParseFileName(functionPath);
         
         // Remove path and .gmf
         functionName = GmatFileUtil::ParseFileName(functionPath);
         std::string::size_type dotIndex = functionName.find(".gmf");
         functionName = functionName.substr(0, dotIndex);
      }
      else
      {
         // gmat function uses whole path name
         functionPath = fm->GetFullPathname("GMAT_FUNCTION_PATH");    
      }
   }
   catch (GmatBaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      
      // see if there is FUNCTION_PATH
      try
      {
         pathname = fm->GetFullPathname("FUNCTION_PATH");
         functionPath = pathname;
      }
      catch (GmatBaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #ifdef DEBUG_FUNCTION_CONSTRUCT
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   objectTypeNames.push_back("GmatFunction");

   #ifdef DEBUG_FUNCTION_CONSTRUCT
   MessageInterface::ShowMessage
      ("   Gmat functionPath=<%s>\n", functionPath.c_str());
   MessageInterface::ShowMessage
      ("   Gmat functionName=<%s>\n", functionName.c_str());
   MessageInterface::ShowMessage
      ("GmatFunction::GmatFunction() constructor <%p> leaving, name='%s'\n",
       this, name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~GmatFunction()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatFunction::~GmatFunction()
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("GmatFunction() destructor entered, this=<%p> '%s', fcs=<%p>\n", this,
       GetName().c_str(), fcs);
   #endif
   
   // delete function sequence including NoOp (loj: 2008.12.22)
   if (fcs != NULL)
      GmatCommandUtil::ClearCommandSeq(fcs, false);
   
   if (unusedGlobalObjectList != NULL)
   {
      delete unusedGlobalObjectList;
      unusedGlobalObjectList = NULL;
   }
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("GmatFunction() destructor exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// GmatFunction(const GmatFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> object being copied
 */
//------------------------------------------------------------------------------
GmatFunction::GmatFunction(const GmatFunction &copy) :
   UserDefinedFunction(copy)
{
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
}


//------------------------------------------------------------------------------
// GmatFunction& GmatFunction::operator=(const GmatFunction& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param <right> object being copied
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
GmatFunction& GmatFunction::operator=(const GmatFunction& right)
{
   if (this == &right)
      return *this;
   
   Function::operator=(right);
   mIsNewFunction = false;
   unusedGlobalObjectList = NULL;
   
   return *this;
}


//------------------------------------------------------------------------------
// bool IsNewFunction()
//------------------------------------------------------------------------------
/**
 * Return true if function was created but not saved to file.
 * FunctionSetupPanel uses this flag to open new editor or load existing function.
 */
//------------------------------------------------------------------------------
bool GmatFunction::IsNewFunction()
{
   return mIsNewFunction;
}

//------------------------------------------------------------------------------
// void SetNewFunction(bool flag)
//------------------------------------------------------------------------------
void GmatFunction::SetNewFunction(bool flag)
{
   mIsNewFunction = flag;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool GmatFunction::Initialize(ObjectInitializer *objInit, bool reinitialize,
                              bool resetLocalObjects)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage("\n");
   ShowTrace(callCount, t1, "GmatFunction::Initialize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("======================================================================\n"
          "GmatFunction::Initialize() entered for function '%s'\n   reinitialize=%d, "
          "objectsInitialized=%d\n", functionName.c_str(), reinitialize, objectsInitialized);
      MessageInterface::ShowMessage("   FCS is %s set.\n", (fcs? "correctly" : "NOT"));
      MessageInterface::ShowMessage("   Pointer for FCS is %p\n", fcs);
      MessageInterface::ShowMessage("   First command in fcs is %s\n",
            (fcs->GetTypeName()).c_str());
      MessageInterface::ShowMessage("   internalCS is %p\n", internalCoordSys);
   #endif
   if (!fcs) return false;
   
   // Override input to initialize if new types of inputs were received
   if (changingInputType)
      reinitialize = true;

   UserDefinedFunction::Initialize(objInit, reinitialize, resetLocalObjects);
   
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator->SetFunction(this);
   validator->SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("   functionObjectMap.size()  = %d\n   automaticObjectMap.size() = %d\n",
       functionObjectMap.size(), automaticObjectMap.size());
   #endif
   
   // Add clones of objects created in the function to the FOS (LOJ: 2014.12.09)
   for (omi = functionObjectMap.begin(); omi != functionObjectMap.end(); ++omi)
   {
      std::string funcObjName = omi->first;
      GmatBase *funcObj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Add clone to objectStore if funcObj <%p>[%s]'%s' is not already in objectStore\n",
          funcObj, funcObj ? funcObj->GetTypeName().c_str() : "NULL", funcObjName.c_str());
      MessageInterface::ShowMessage
         ("   funcObj->IsGlobal=%d, funcObj->IsLocal=%d\n", funcObj->IsGlobal(),
          funcObj->IsLocal());
      #endif
      
      // if name not found, clone it and add to map (loj: 2008.12.15)
      if (objectStore->find(funcObjName) == objectStore->end())
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   About to clone <%p>[%s]'%s'\n", (omi->second),
             (omi->second)->GetTypeName().c_str(), (omi->second)->GetName().c_str());
         #endif
         GmatBase *funcObjClone = (omi->second)->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (funcObjClone, funcObjName, "GmatFunction::Initialize()",
             "funcObj = (omi->second)->Clone()");
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT_MORE
         try
         {
            MessageInterface::ShowMessage
               ("   funcObj(%s)->EvaluateReal() = %f\n", funcObjName.c_str(),
                funcObjClone->GetRealParameter("Value"));
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage("%s\n", e.GetFullMessage().c_str());             
         }
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> Adding funcObj clone <%p>'%s' to objectStore\n", funcObjClone,
             funcObjName.c_str());
         #endif
         funcObjClone->SetIsLocal(true);
         objectStore->insert(std::make_pair(funcObjName, funcObjClone));
      }
      else
      {
         std::map<std::string, GmatBase *>::iterator mapi;
         mapi = objectStore->find(funcObjName);
         if (mapi != objectStore->end())
         {
            GmatBase *mapObj = (mapi->second);
            #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("   '%s' already exist: <%p>[%s]'%s'\n", funcObjName.c_str(), mapObj,
                mapObj->GetTypeName().c_str(), mapObj->GetName().c_str());
            MessageInterface::ShowMessage
               ("   Now checking if input parameter is redefined to different type\n");
            #endif

            // Check if input parameter is redefined in a function
            if (funcObj->GetTypeName() != mapObj->GetTypeName())
            {
               // Check for return type if object type is Parameter?
               // (fix for GMT-5262 LOJ: 2015.09.04)
               if (funcObj->IsOfType(Gmat::PARAMETER) && mapObj->IsOfType(Gmat::PARAMETER))
               {
                  if (((Parameter*)funcObj)->GetReturnType() != ((Parameter*)mapObj)->GetReturnType())
                  {
                     throw FunctionException
                        ("Redefinition of formal input parameter '" + funcObjName +
                         "' to different type is not allowed in GMAT function '" +
                         functionPath + "'.  It's expected type is '" +
                         mapObj->GetTypeName() + "'.\n");
                  }
               }
            }

            // Copy data of fresh local objects
            if (resetLocalObjects)
               objectStore->at(funcObjName)->Copy(omi->second);
         }
      }
   }
   
   // add automatic objects such as sat.X to the FOS (well, actually, clones of them)
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      std::string autoObjName = omi->first;
      GmatBase *autoObj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Add clone to objectStore if autoObj <%p>[%s]'%s' is not already in objectStore\n",
          autoObj, autoObj ? autoObj->GetTypeName().c_str() : "NULL", autoObjName.c_str());
      MessageInterface::ShowMessage
         ("   autoObj->IsGlobal=%d, autoObj->IsLocal=%d\n", autoObj->IsGlobal(),
          autoObj->IsLocal());
      #endif
      
      // If automatic object owner (i.e Parameter owner) is global, set it global
      GmatBase *owner = NULL;
      if (IsAutomaticObjectGlobal(autoObjName, &owner))
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   autoObj <%p>'%s' is global, so setting isGlobal to true\n", autoObj,
             autoObjName.c_str());
         MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   owner=", owner));
         #endif
         autoObj->SetIsGlobal(true);
         
         // Set IsLocal to false to indicate it is not created inside a function (LOJ: 2015.10.03)
         owner->SetIsLocal(false);
         autoObj->SetIsLocal(false);
      }
      else
      {
         #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
            ("   autoObj <%p>'%s' or owner is local, "
               "so setting isGlobal to false\n", autoObj,
               autoObjName.c_str());
            MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   owner=", owner));
         #endif

         // Object created in function
         autoObj->SetIsGlobal(false);
         autoObj->SetIsLocal(true);
      }
      
      // if name not found, clone it and add to map (loj: 2008.12.15)
      std::map<std::string, GmatBase *>::iterator foundIter = objectStore->find(autoObjName);
      if (foundIter == objectStore->end())
      {
         GmatBase *clonedAutoObj = autoObj->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (clonedAutoObj, autoObjName, "GmatFunction::Initialize()",
             "clonedAutoObj = (omi->second)->Clone()");
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT_MORE
         try
         {
            MessageInterface::ShowMessage
               ("   clonedAutoObj(%s)->EvaluateReal() = %f\n", autoObjName.c_str(),
                clonedAutoObj->GetRealParameter("Value"));
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage("%s\n", e.GetFullMessage().c_str());             
         }
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> Adding clonedAutoObj <%p>'%s' to objectStore, isGlobal=%d, "
             "isLocal=%d\n", clonedAutoObj, autoObjName.c_str(), clonedAutoObj->IsGlobal(),
             clonedAutoObj->IsLocal());
         if (owner)
         {
            GmatBase *paramOwner = clonedAutoObj->GetRefObject(owner->GetType(), owner->GetName());
            MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   paramOwner=", paramOwner));
         }
         #endif
         
         // Do not set IsLocal to true since it is cloned above (LOJ: 2015.10.03)
         //clonedAutoObj->SetIsLocal(true);
         objectStore->insert(std::make_pair(autoObjName, clonedAutoObj));
      }
   }
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("   If object is global, move it to globalObjectStore\n");
   #endif
   
   // If object is global, move it to globalObjectStore
   
   // Save the ones that need to be removed from the objectStore here so as not to
   // corrupt the objectStore in the middle of iterating!!
   StringArray removeFromObjectStore;
   
   for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
   {
      std::string objName = omi->first;
      GmatBase *obj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   obj<%p>'%s' IsGlobal:%s, IsLocal:%s\n", obj, objName.c_str(),
          obj->IsGlobal() ? "Yes" : "No", obj->IsLocal() ? "Yes" : "No");
      #endif
      // Check if it is global but not a local (means passed or locally created object)
      if (obj->IsGlobal() && !obj->IsLocal())
      {
         if (globalObjectStore->find(objName) == globalObjectStore->end())
         {
            #ifdef DEBUG_FUNCTION_INIT
               MessageInterface::ShowMessage
               ("   ==> obj<%p>'%s' is global and not local, so moving it from "
                "objectStore to globalObjectStore\n", obj, objName.c_str());
            #endif
            globalObjectStore->insert(std::make_pair(objName, obj));
            removeFromObjectStore.push_back(objName); /// save the name to remove it later
//            objectStore->erase(objName); // do NOT remove it here while iterating!
         }
      }
      else
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> obj<%p>'%s' is global and local, so it stays in objectStore\n",
             obj, objName.c_str());
         #endif
      }
   }
   // Now remove the ones that were moved to the global object store
   for (UnsignedInt ii = 0; ii < removeFromObjectStore.size(); ii++)
      objectStore->erase(removeFromObjectStore.at(ii));
   
   GmatCommand *current = fcs;

   // Set object map on Validator (moved here from below) LOJ: 2015.04.09
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("   Now about to set combined object map to Validator\n");
   ShowObjectMap(objectStore, "In GmatFunction::Initialize()", "objectStore");
   ShowObjectMap(globalObjectStore, "In GmatFunction::Initialize()", "globalObjectStore");
   #endif
   validatorStore.clear();
   for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
      validatorStore.insert(std::make_pair(omi->first, omi->second));
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
      validatorStore.insert(std::make_pair(omi->first, omi->second));
   #ifdef DEBUG_FUNCTION_INIT
   ShowObjectMap(&validatorStore, "In GmatFunction::Initialize()", "validatorStore");
   #endif
   validator->SetObjectMap(&validatorStore);
   
   // Create wrappers for local subscribers (fix for GMT1552 LOJ: 2015.06.24)
   // This must be done before initialization
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("   Now about to create subscriber wrappers\n");
   #endif
   CreateSubscriberWrappers();
   
   
   // Initialize function objects here. Moved from Execute() (LOJ: 2015.02.27)
   if (!objectsInitialized ||
       (objectsInitialized && reinitialize))
   {
      objectsInitialized = true;
      validator->HandleCcsdsEphemerisFile(objectStore, true);
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("GmatFunction::Initialize() Calling InitializeLocalObjects()\n");
      #endif
      InitializeLocalObjects(objInit, current, true);
   }
   
   // Set object store, solar system, etc to all the commands and
   // validate command to create necessary wrappers
   while (current)
   {
      #ifdef DEBUG_FUNCTION_INIT
         if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
         else MessageInterface::ShowMessage("   =====> Current command is %s <%s>\n",
                 (current->GetTypeName()).c_str(),
                 current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      current->SetObjectMap(objectStore);
      current->SetGlobalObjectMap(globalObjectStore);
      current->SetSolarSystem(solarSys);
      current->SetInternalCoordSystem(internalCoordSys);
      current->SetTransientForces(forces);      
      
      // Commands are only validated and intiialized the first execution
      // of the function for runtime speed
      if (fcsInitialized && !reinitialize)
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   fcs already validated, so skipping validation\n");
         #endif
      }
      else
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   Now about to call Validator->ValidateCommand() of type %s\n",
             current->GetTypeName().c_str());
         #endif
         
         // Let's try to ValidateCommand here, this will validate the command
         // and create wrappers also
         if (!validator->ValidateCommand(current, false, 2))
         {
            // get error message (loj: 2008.06.04)
            StringArray errList = validator->GetErrorList();
            std::string msg; // Check for empty errList (loj: 2009.03.17)
            if (errList.empty())
               msg = "Error occurred";
            else
               msg = errList[0];
            
            throw FunctionException(msg + " in the function \"" + functionPath + "\"");
         }

         #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
            ("   Now about to initialize command of type %s\n", current->GetTypeName().c_str());
         #endif

         // catch exception and add function name to message (loj: 2008.09.23)
         try
         {
            if (!(current->Initialize()))
            {
               #ifdef DEBUG_FUNCTION_INIT
                  MessageInterface::ShowMessage
                  ("Exiting  GmatFunction::Initialize for function '%s' with false\n",
                     functionName.c_str());
               #endif
               return false;
            }
         }
         catch (BaseException &e)
         {
            throw FunctionException("Cannot continue due to " + e.GetFullMessage() +
               " in the function \"" + functionPath + "\"");
         }
      }
      
      if (current->IsOfType("BranchCommand"))
         ((BranchCommand*)current)->ResetExecutionState();

      // Check to see if the command needs a server startup (loj: 2008.07.25)
      if (current->NeedsServerStartup())
         if (validator->StartMatlabServer(current) == false)
            throw FunctionException("Unable to start the server needed by the " +
                                   (current->GetTypeName()) + " command");

      current = current->GetNext();
   }
   
   
   // Get automatic global object list and check if they are used in the function
   // command sequence so that when any global object is declared in the main script
   // but not used in the function, they can be ignored during function local object
   // initialization. (LOJ: 2009.12.18)
   BuildUnusedGlobalObjectList();
   
   fcsInitialized = true;
   fcsFinalized = false;
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GmatFunction::Initialize() exiting for function '%s' with true\n\n",
       functionName.c_str());
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Initialize() exiting", true);
   #endif
   
   isInitialized = true;
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize = true)
//------------------------------------------------------------------------------
bool GmatFunction::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   if (!fcs) return false;
   if (!objInit) return false;
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage("\n");
   ShowTrace(callCount, t1, "GmatFunction::Execute() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "GmatFunction::Execute() entered for '%s'\n   internalCS is <%p>, "
       "reinitialize = %d, fcs=<%p><%s>\n", functionName.c_str(), internalCoordSys,
       reinitialize, fcs, fcs ? fcs->GetTypeName().c_str() : "NULL");
   #endif
   
   GmatCommand *current = fcs;
   GmatCommand *last = NULL;
   
   // We want to initialize local objects with new object map,
   // so do it everytime (loj: 2008.09.26)
   // This causes to slow down function execution, so initialize if necessary
   if (reinitialize)
      objectsInitialized = false;
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   objectsInitialized = %d\n", objectsInitialized);
   #endif
   
   // For two modes function parsing, some ref objects are not set without reinitialization
   // Reinitialize Spacecrafts (LOJ: 2015.01.21) - /MonteCarlo/TestCalcOfGSEtime.script
   // Reinitialize Burns (LOJ:2015.01.09) - Function_TargetCheck.script
   // Reinitialize CalculatedPoints (LOJ:2015.01.08)
   // Reinitialize CoordinateSystems to fix bug 1599 (LOJ: 2009.11.05)
   // Reinitialize Parameters to fix bug 1519 (LOJ: 2009.09.16)
   if (objectsInitialized)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   Reinitializing CoordinateSystems, CalculatedPointes, Spacecrafts, Burns, Solvers, and Parameters\n");
      #endif
      
      #ifdef DEBUG_TRACE
      ShowTrace(callCount, t1, "GmatFunction::Execute() BEGIN object re-initialization");
      #endif
      
      if (!objInit->InitializeObjects(true, Gmat::COORDINATE_SYSTEM))
         throw FunctionException
            ("Failed to re-initialize CoordinateSystems in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::CALCULATED_POINT))
         throw FunctionException
            ("Failed to re-initialize CalculatedPoints in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::SPACECRAFT))
         throw FunctionException
            ("Failed to re-initialize Spacecrafts in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::BURN))
         throw FunctionException
            ("Failed to re-initialize Burns in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::SOLVER))
         throw FunctionException
            ("Failed to re-initialize Solvers in the \"" + functionName + "\"");
      if (!objInit->InitializeObjects(true, Gmat::PARAMETER))
         throw FunctionException
            ("Failed to re-initialize Parameters in the \"" + functionName + "\"");
      
      #ifdef DEBUG_TRACE
      ShowTrace(callCount, t1, "GmatFunction::Execute() END   object re-initialization");
      #endif
   }
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjects("In GmatFunction::Execute()");
   #endif
   
   // Go through each command in the sequence and execute.
   // Once it gets to a real command, initialize local and automatic objects.
   while (current)
   {
      // Call to IsNextAFunction is necessary for branch commands in particular
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("......Function executing <%p><%s> [%s]\n", current, current->GetTypeName().c_str(),
          current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("      objectsInitialized=%d\n", objectsInitialized);
      #endif
      
      last = current;
      
      if (!objectsInitialized)
      {
         // Since we don't know where actual mission sequence starts, just check
         // for command that is not NoOp, Create, Global, and GMAT with equation.
         // Can we have simple command indicating beginning of the sequence,
         // such as BeginSequence? (loj: 2008.06.19)
         // @todo: Now we have BeginMissionSequence, but not all functions have it,
         // so check it first otherwise do in the old way. (loj: 2010.07.16)
         Function *func = current->GetCurrentFunction();
         bool isEquation = false;
         std::string cmdType = current->GetTypeName();
         if (func && cmdType == "GMAT")
            if (((Assignment*)current)->GetMathTree() != NULL)
               isEquation = true;
         
         if (cmdType != "NoOp" && cmdType != "Create" && cmdType != "Global")
         {
            bool beginInit = true;            
            if (cmdType == "GMAT" && !isEquation)
               beginInit = false;

            if (cmdType == "BeginMissionSequence" || cmdType == "BeginScript")
               beginInit = true;
            
            if (beginInit)
            {
               objectsInitialized = true;
               validator->HandleCcsdsEphemerisFile(objectStore, true);
               #ifdef DEBUG_FUNCTION_EXEC
               MessageInterface::ShowMessage
                  ("============================ Initializing LocalObjects at current\n"
                   "%s\n", current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
               MessageInterface::ShowMessage
                  ("GmatFunction::Execute() Calling InitializeLocalObjects()\n");
               #endif
                              
               InitializeLocalObjects(objInit, current, true);
            }
         }
      }
      
      // Now execute the function sequence
      try
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("Now calling <%p>[%s]->Execute()\n", current->GetTypeName().c_str(),
             current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         
         if (!(current->Execute()))
            return false;
      }
      catch (BaseException &e)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("Caught CommandException: IsFatal()=%d\n%s\n", e.IsFatal(),
             e.GetFullMessage().c_str());
         #endif
         
         // If it is user interrupt, rethrow (loj: 2008.10.16)
         // How can we tell if it is thrown by Stop command?
         // For now just find the phrase "interrupted by Stop command"
         std::string msg = e.GetFullMessage();
         if (msg.find("interrupted by Stop command") != msg.npos)
         {
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("*** Interrupted by Stop commaned, so re-throwing...\n");
            #endif
            throw;
         }
         
         if (e.IsFatal())
         {
            #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
               ("*** The exception is fatal, so re-throwing...\n");
            #endif
            // Add command line to error message (LOJ: 2010.04.13)
            throw FunctionException
               ("In " + current->GetGeneratingString(Gmat::NO_COMMENTS) + ", " +
                e.GetFullMessage());
            //throw;
         }

         // Should we still re-throw the exception here? (LOJ: 2015.02.19)
         throw FunctionException
            ("In " + current->GetGeneratingString(Gmat::NO_COMMENTS) + ", " +
             e.GetFullMessage());
      }
      
      // If current command is BranchCommand and still executing, continue to next
      // command in the branch (LOJ: 2009.03.24)
      if (current->IsOfType("BranchCommand") && current->IsExecuting())
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("In Function '%s', still executing current command is <%p><%s>\n",
             functionName.c_str(), current, current ? current->GetTypeName().c_str() : "NULL");
         #endif
         
         continue;
      }
      
      current = current->GetNext();
      
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("In Function '%s', the next command is <%p><%s>\n", functionName.c_str(),
          current, current ? current->GetTypeName().c_str() : "NULL");
      #endif
   }
   
   // This block no longer needed since two-mode parsing already filled
   // object map for the Validator.
   // (LOJ: 2015.10.15 Fixes GMT-5336 Passing global objects in and out to a fucction)
   // Set ObjectMap from the last command to Validator in order to create
   // valid output wrappers (loj: 2008.11.12)
   // #ifdef DEBUG_FUNCTION_EXEC
   // MessageInterface::ShowMessage
   //    ("GmatFunction()::Execute() Setting object map from the last command <%p>[%s]\n",
   //     last, last->GetTypeName().c_str());
   // ShowObjectMap(last->GetObjectMap());
   // #endif
   //validator->SetObjectMap(last->GetObjectMap());
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GmatFunction()::Execute()  Now about to create %d output wrapper(s) to "
       "set results, objectsInitialized=%d\n", outputNames.size(), objectsInitialized);
   #endif
   
   // create output wrappers and put into map
   GmatBase *obj = NULL;
   wrappersToDelete.clear();
   for (unsigned int jj = 0; jj < outputNames.size(); jj++)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   Trying to find ouputNames[%d]='%s'\n", jj, outputNames[jj].c_str());
      #endif
      if (!(obj = FindObject(outputNames.at(jj))))
      {
         std::string errMsg = "Function: Output \"" + outputNames.at(jj);
         errMsg += " not found for function \"" + functionName + "\"";
         throw FunctionException(errMsg);
      }
      
      std::string outName = outputNames.at(jj);
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   The object '%s' found\n   Trying to create wrapper for '%s'\n",
          GmatBase::WriteObjectInfo("", obj, false).c_str(), outName.c_str());
      #endif
      
      ElementWrapper *outWrapper =
         validator->CreateElementWrapper(outName, false, 0);
      
      #ifdef DEBUG_MORE_MEMORY
      MessageInterface::ShowMessage
         ("+++ GmatFunction::Execute() *outWrapper = validator->"
          "CreateElementWrapper(%s), <%p> '%s'\n", outName.c_str(), outWrapper,
          outWrapper->GetDescription().c_str());
      #endif
      
      outWrapper->SetRefObject(obj);
      
      // nested CallFunction crashes if old outWrappers are deleted here. (loj: 2008.11.24)
      // so collect here and delete when FunctionRunner completes.
      wrappersToDelete.push_back(outWrapper);         
      
      // Set new outWrapper
      outputArgMap[outName] = outWrapper;
      #ifdef DEBUG_FUNCTION_EXEC // --------------------------------------------------- debug ---
         MessageInterface::ShowMessage("GmatFunction: Output wrapper created for %s\n",
                                       (outputNames.at(jj)).c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GmatFunction::Execute() exiting true for '%s'\n", functionName.c_str());
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Execute() exiting", true);
   #endif
   
   return true; 
}


//------------------------------------------------------------------------------
// void Finalize(bool cleanUp = false)
//------------------------------------------------------------------------------
void GmatFunction::Finalize(bool cleanUp)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   ShowTrace(callCount, t1, "GmatFunction::Finalize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "GmatFunction::Finalize() entered for '%s', FCS %s\n",
       functionName.c_str(), fcsFinalized ? "already finalized, so skip fcs" :
       "NOT finalized, so call fcs->RunComplete");
   #endif
   
   // Call RunComplete on each command in fcs
   if (!fcsFinalized)
   {
      fcsFinalized = true;
      GmatCommand *current = fcs;
      while (current)
      {
         #ifdef DEBUG_FUNCTION_FINALIZE
            if (!current)
               MessageInterface::ShowMessage
                  ("   GmatFunction:Finalize() Current is NULL!!!\n");
            else
               MessageInterface::ShowMessage
                  ("   GmatFunction:Finalize() Now about to finalize "
                   "(call RunComplete on) command %s\n",
                   (current->GetTypeName()).c_str());
         #endif
         current->RunComplete();
         current = current->GetNext();
      }
   }
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("   Calling Function::Finalize()\n");
   #endif
   
   UserDefinedFunction::Finalize(cleanUp);
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("GmatFunction::Finalize() leaving\n");
   #endif
   
   #ifdef DEBUG_TRACE
   ShowTrace(callCount, t1, "GmatFunction::Finalize() exiting", true, true);
   #endif
   
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the GmatFunction.
 *
 * @return clone of the GmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GmatFunction::Clone() const
{
   return (new GmatFunction(*this));
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
void GmatFunction::Copy(const GmatBase* orig)
{
   operator=(*((GmatFunction *)(orig)));
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
bool GmatFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("GmatFunction::SetStringParameter() entered, id=%d, value=%s\n", id, value.c_str());
   #endif

   bool retval = false;
   switch (id)
   {
   case FUNCTION_PATH:
      {
         retval = SetGmatFunctionPath(value);
         break;
      }
   case FUNCTION_NAME:
      {
         // Remove path if it has one
         functionName = GmatFileUtil::ParseFileName(functionPath);
         
         // Remove .gmf for function name
         std::string::size_type dotIndex = functionName.find(".gmf");
         functionName = functionName.substr(0, dotIndex);
         
         retval = true;
         break;
      }
   default:
      retval = UserDefinedFunction::SetStringParameter(id, value);
      break;
   }
   
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("GmatFunction::SetStringParameter() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool GmatFunction::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// void ShowTrace(Integer count, Integer t1, const std::string &label = "",
//                bool showMemoryTracks = false, bool addEol = false)
//------------------------------------------------------------------------------
void GmatFunction::ShowTrace(Integer count, Integer t1, const std::string &label,
                             bool showMemoryTracks, bool addEol)
{
   // To locally control debug output
   bool showTrace = true;
   bool showTracks = true;
   
   showTracks = showTracks & showMemoryTracks;
   
   if (showTrace)
   {
      #ifdef DEBUG_TRACE
      clock_t t2 = clock();
      MessageInterface::ShowMessage
         (">>>>> CALL TRACE: %s, '%s' Count = %d, elapsed time: %f sec\n", label.c_str(),
          functionName.c_str(), count, (Real)(t2-t1)/CLOCKS_PER_SEC);
      #endif
   }
   
   if (showTracks)
   {
      #ifdef DEBUG_MEMORY
      StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
      if (showTrace)
         MessageInterface::ShowMessage
            ("    ==> There are %d memory tracks\n", tracks.size());
      else
         MessageInterface::ShowMessage
            (">>>>> MEMORY TRACK: There are %d memory tracks when %s, '%s'\n", tracks.size(),
             label.c_str(), functionName.c_str());
      
      if (addEol)
         MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// bool InitializeLocalObjects(ObjectInitializer *objInit,
//                             GmatCommand *current, bool ignoreException)
//------------------------------------------------------------------------------
bool GmatFunction::InitializeLocalObjects(ObjectInitializer *objInit,
                                          GmatCommand *current,
                                          bool ignoreException)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("\n============================ BEGIN INITIALIZATION of local objects in '%s'\n",
       functionName.c_str());
   MessageInterface::ShowMessage
      ("GmatFunction::InitializeLocalObjects() entered, ignoreException=%d\n",
       ignoreException);
   MessageInterface::ShowMessage
      ("Now at command <%p><%s> \"%s\"\n", current, current->GetTypeName().c_str(),
       current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   ShowObjectMap(&functionObjectMap, "In GmatFunction::InitializeLocalObjects()",
                 "functionObjectMap");
   #endif
   
   // Why internal coordinate system is empty in ObjectInitializer?
   // Set internal coordinate system (added (loj: 2008.10.07)
   objInit->SetInternalCoordinateSystem(internalCoordSys);
   
   // Let's try initializing local objects using ObjectInitializer (2008.06.19)
   // We need to add subscribers to publisher, so pass true
   try
   {
      if (!objInit->InitializeObjects(true, Gmat::UNKNOWN_OBJECT,
                                      unusedGlobalObjectList))
      {
         // Should we throw an exception instead?
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("GmatFunction::InitializeLocalObjects() returning false, failed to initialize\n");
         MessageInterface::ShowMessage
            ("============================ END INITIALIZATION of local objects\n");
         #endif
         return false;
      }
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("==> Caught exception:%s, IsFatal=%d\n", be.GetFullMessage().c_str());
      #endif
      // We need to ignore exception thrown for the case Object is
      // created after it is used, such as
      // GMAT DefaultOpenGL.ViewPointReference = EarthSunL1;
      // Create LibrationPoint EarthSunL1;
      if (!ignoreException || (ignoreException && be.IsFatal()))
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("objInit->InitializeObjects() threw a fatal exception:\n'%s'\n"
             "   So rethrow...\n", be.GetFullMessage().c_str());
         #endif
         throw;
      }
      else
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("objInit->InitializeObjects() threw an exception:\n'%s'\n"
             "   So ignoring...\n", be.GetFullMessage().c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("============================ END INITIALIZATION of local objects\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void CreateSubscriberWrappers()
//------------------------------------------------------------------------------
void GmatFunction::CreateSubscriberWrappers()
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("GmatFunction::CreateSubscriberWrappers() entered\n");
   MessageInterface::ShowMessage("   Create wrappers for subscribers if any\n");
   MessageInterface::ShowMessage("   objectStore->size()=%d\n", objectStore->size());
   #endif
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
   {
      std::string funcObjName = omi->first;
      GmatBase *funcObj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Create wrappers if funcObj [%s]'%s' is a subscriber\n",
          funcObjName.c_str(), funcObj ? funcObj->GetTypeName().c_str() : "NULL");
      #endif
      
      if (funcObj->IsOfType(Gmat::SUBSCRIBER))
      {
         Subscriber *sub = (Subscriber*)funcObj;
         const StringArray wrapperNames = sub->GetWrapperObjectNameArray();
         
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   '%s' has %d wrapper names:\n", funcObjName.c_str(), wrapperNames.size());
         for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
            MessageInterface::ShowMessage("   '%s'\n", wrapperNames[ii].c_str());
         #endif
         
         for (StringArray::const_iterator i = wrapperNames.begin();
              i != wrapperNames.end(); ++i)
         {
            std::string wname = (*i);
            // Skip blank name
            if (wname == "")
               continue;
            
            try
            {
               #ifdef DEBUG_FUNCTION_INIT
               MessageInterface::ShowMessage
                  ("   About to create wrapper for '%s'\n", wname.c_str());
               #endif
               ElementWrapper *ew = validator->CreateElementWrapper(wname, true, 2);
               
               if (sub->SetElementWrapper(ew, wname) == false)
               {
                  #ifdef DEBUG_FUNCTION_INIT
                  MessageInterface::ShowMessage
                     ("Throwing exception: cannot create an item wrapper '%s' for the "
                      "%s '%s' in the function '%s'\n", wname.c_str(),
                      funcObj->GetTypeName().c_str(), funcObj->GetName().c_str(),
                      functionPath.c_str());
                  #endif
                  FunctionException fe;
                  fe.SetDetails("Error occurred during validation of '%s' for the "
                                "%s '%s' in the function \"%s\"",
                                wname.c_str(), funcObj->GetTypeName().c_str(),
                                funcObj->GetName().c_str(), functionPath.c_str());
               }
            }
            catch (BaseException &ex)
            {
               #ifdef DEBUG_FUNCTION_INIT
               MessageInterface::ShowMessage
                  ("GmatFunction::InitializeLocalObjects() returning false, "
                   "failed to create a wrapper for '%s'\n%s\n", wname.c_str(),
                   ex.GetFullMessage().c_str());
               #endif
               throw FunctionException(ex.GetFullMessage() + " in the function \"" + functionPath + "\"");
            }
         }
      }
   }
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("GmatFunction::CreateSubscriberWrappers() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// bool SetGmatFunctionPath(const std::string &path)
//------------------------------------------------------------------------------
bool GmatFunction::SetGmatFunctionPath(const std::string &path)
{
   #ifdef DEBUG_FUNCTION_SET_PATH
   MessageInterface::ShowMessage
      ("GmatFunction::SetGmatFunctionPath() <%p> entered, path='%s'\n", this,
       path.c_str());
   #endif
   
   FileManager *fm = FileManager::Instance();
   
   // Compose full path if it has relative path.
   // Assuming if first char has '.', it has relative path.
   std::string thePath  = GmatStringUtil::Trim(path);
   std::string tempPath = thePath;
   #ifdef DEBUG_FUNCTION_SET_PATH
   MessageInterface::ShowMessage("   tempPath='%s'\n", tempPath.c_str());
   #endif
   if (tempPath[0] == '.')
   {
      #ifdef DEBUG_FUNCTION_SET_PATH
      MessageInterface::ShowMessage("   It has relative path\n");
      #endif
      // Follow new file path implementation (LOJ: 2015.01.13)
      //std::string currPath = fm->GetCurrentWorkingDirectory();
      std::string currPath = fm->GetGmatWorkingDirectory();
      
      #ifdef DEBUG_FUNCTION_SET_PATH
      MessageInterface::ShowMessage("   currPath='%s'\n", currPath.c_str());
      #endif
            
      tempPath = currPath + thePath;
      tempPath = GmatFileUtil::ConvertToOsFileName(tempPath);

      // if it is not relative to the script, maybe it's relative to the
      // bin directory
      if (!fm->DoesDirectoryExist(tempPath))
      {
         std::string binDir   = fm->GetBinDirectory();
         tempPath             = binDir + thePath;
         tempPath             = GmatFileUtil::ConvertToOsFileName(tempPath);
      }
      
      #ifdef DEBUG_FUNCTION_SET_PATH
      MessageInterface::ShowMessage("   tempPath='%s'\n", tempPath.c_str());
      #endif
      
      // If path has only a path name without a function name, append function name
      // This will make FunctionPath to work without specifying function name (LOJ: 2015.03.16)
      if (tempPath.find(".gmf") == tempPath.npos)
      {
         #ifdef DEBUG_FUNCTION_SET_PATH
         MessageInterface::ShowMessage("   It has relative path only\n");
         #endif
         if ((tempPath[tempPath.size()-1] != '/') &&
             (tempPath[tempPath.size()-1] != '\\'))
            tempPath = tempPath + "/";
         functionPath = tempPath + functionName + ".gmf";
      }
      else
      {
         #ifdef DEBUG_FUNCTION_SET_PATH
         MessageInterface::ShowMessage("   It has relative path and name\n");
         #endif
         functionPath = tempPath;
      }
   }
   else
   {
      // Check if it has only absolute path
      if (tempPath.find(".gmf") == tempPath.npos)
      {
         #ifdef DEBUG_FUNCTION_SET_PATH
         MessageInterface::ShowMessage("   It has absolute path only\n");
         #endif
         if ((tempPath[tempPath.size()-1] != '/') &&
             (tempPath[tempPath.size()-1] != '\\'))
            tempPath = tempPath + '/';
         functionPath = tempPath + functionName + ".gmf";
      }
      else
      {
         #ifdef DEBUG_FUNCTION_SET_PATH
         MessageInterface::ShowMessage("   It has absolute path and name\n");
         #endif
         functionPath = tempPath;
      }
   }
   
   // Add to GmatFunction path so that nested function can be found
   // Do we need to add to FileManager function path? exclude it (LOJ: 2015.03.17)
   std::string funcPathOnly = GmatFileUtil::ParsePathName(functionPath);
   #ifdef DEBUG_FUNCTION_SET_PATH
   MessageInterface::ShowMessage
      ("   ===> Adding GmatFunction path '%s' to FileManager for function '%s'\n",
       funcPathOnly.c_str(), GetName().c_str());
   #endif
   fm->AddGmatFunctionPath(funcPathOnly);
   
   // Remove path for function name
   functionName = GmatFileUtil::ParseFileName(functionPath);
   
   // Remove .gmf for GmatFunction name
   std::string::size_type dotIndex = functionName.find(".gmf");
   functionName = functionName.substr(0, dotIndex);
   
   #ifdef DEBUG_FUNCTION_SET_PATH
   MessageInterface::ShowMessage("   functionPath='%s'\n", functionPath.c_str());
   MessageInterface::ShowMessage("   functionName='%s'\n", functionName.c_str());
   #endif
   
   #ifdef DEBUG_FUNCTION_SET_PATH
   MessageInterface::ShowMessage
      ("GmatFunction::SetGmatFunctionPath() <%p> returning true\n", this);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void BuildUnusedGlobalObjectList()
//------------------------------------------------------------------------------
/*
 * Builds unused global object list which is used in the ObjectInitializer
 * for ignoring undefined ref objects. For now it adds automatic global
 * CoordinateSystem if it's ref objects is Spacecraft and it is not used in
 * the function sequence. Since Spacecraft is not an automatic object it is
 * not automatically added to GOS.
 */
//------------------------------------------------------------------------------
void GmatFunction::BuildUnusedGlobalObjectList()
{
   #ifdef DEBUG_UNUSED_GOL
   MessageInterface::ShowMessage
      ("BuildUnusedGlobalObjectList() entered. There are %d global objects\n",
       globalObjectStore->size());
   #endif
   
   if (unusedGlobalObjectList != NULL)
      delete unusedGlobalObjectList;
   
   unusedGlobalObjectList = new StringArray;
   
   // Check global object store
   std::string cmdName;
   GmatCommand *cmdUsing = NULL;
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      if (!GmatCommandUtil::FindObject(fcs, (omi->second)->GetType(), omi->first,
                                       cmdName, &cmdUsing))
      {
         // Add unused global CoordinateSystem with Spacecraft origin,  primary,
         // or secondary, since Spacecraft is not an automatic global object and
         // we don't want to throw an exception for unexisting Spacecraft in the GOS.
         if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
         {
            GmatBase *origin = obj->GetRefObject(Gmat::SPACE_POINT, "_GFOrigin_");
            GmatBase *primary = obj->GetRefObject(Gmat::SPACE_POINT, "_GFPrimary_");
            GmatBase *secondary = obj->GetRefObject(Gmat::SPACE_POINT, "_GFSecondary_");
            
            if ((origin != NULL && origin->IsOfType(Gmat::SPACECRAFT)) ||
                (primary != NULL && primary->IsOfType(Gmat::SPACECRAFT)) ||
                (secondary != NULL && secondary->IsOfType(Gmat::SPACECRAFT)))
            {
               #ifdef DEBUG_UNUSED_GOL
               MessageInterface::ShowMessage
                  ("==> Adding '%s' to unusedGOL\n", (omi->first).c_str());
               #endif
               unusedGlobalObjectList->push_back(omi->first);
            }
         }
      }
   }
   #ifdef DEBUG_UNUSED_GOL
   MessageInterface::ShowMessage
      ("BuildUnusedGlobalObjectList() leaving, There are %d unused global objects\n",
       unusedGlobalObjectList->size());
   #endif
}
