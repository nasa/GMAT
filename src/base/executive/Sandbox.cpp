//$Id$
//------------------------------------------------------------------------------
//                                 Sandbox
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
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/10/08
//
/**
 * Implementation for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------

#include "Sandbox.hpp"
#include "Moderator.hpp"
#include "SandboxException.hpp"
#include "EquationInitializer.hpp"
#include "MathTree.hpp"
#include "Parameter.hpp"
#include "FiniteThrust.hpp"
#include "Function.hpp"
#include "UserDefinedFunction.hpp"
#include "CallFunction.hpp"
#include "Assignment.hpp"
#include "BranchCommand.hpp"
#include "Propagate.hpp"           // For suppression of CommandEcho message repeats
#include "SubscriberException.hpp"
#include "CommandUtil.hpp"         // for GetCommandSeqString()
#include "MessageInterface.hpp"

#include <algorithm>       // for find

//#define DISABLE_SOLAR_SYSTEM_CLONING

//#define DISALLOW_NESTED_GMAT_FUNCTIONS

//#define DEBUG_SANDBOX_INIT
//#define DEBUG_MODERATOR_CALLBACK
//#define DEBUG_SANDBOX_GMATFUNCTION
//#define DEBUG_SANDBOX_OBJ_INIT
//#define DEBUG_SANDBOX_OBJ_ADD
//#define DEBUG_SANDBOX_OBJECT_MAPS
//#define DBGLVL_SANDBOX_RUN 2
//#define DEBUG_SANDBOX_CLEAR
//#define DEBUG_SANDBOX_CLONING
//#define DEBUG_SS_CLONING
//#define DEBUG_EVENTLOCATION
//#define DEBUG_CLONE_UPDATES
//#define REPORT_CLONE_UPDATE_STATUS

//#define DEBUG_COMMAND_LOCATION

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#ifdef DEBUG_SANDBOX_INIT
   ObjectMap::iterator omIter;
#endif


//------------------------------------------------------------------------------
// Sandbox::Sandbox()
//------------------------------------------------------------------------------
/**
 *  Default constructor.
 */
//------------------------------------------------------------------------------
Sandbox::Sandbox() :
   solarSys          (NULL),
   internalCoordSys  (NULL),
   publisher         (NULL),
   sequence          (NULL),
   current           (NULL),
   moderator         (NULL),
   state             (IDLE),
   interruptCount    (45),
   pollFrequency     (50),
   objInit           (NULL),
//   cloneUpdateStyle  (SKIP_UPDATES)
   cloneUpdateStyle  (PASS_TO_ALL),
   errorInPreviousFcs (false),
   warnPyInterface   (0),
   pCreateWidget     (NULL)
{
}


//------------------------------------------------------------------------------
// ~Sandbox()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
Sandbox::~Sandbox()
{
   #ifndef DISABLE_SOLAR_SYSTEM_CLONING   
      if (solarSys)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (solarSys, solarSys->GetName(), "Sandbox::~Sandbox()",
             " deleting cloned solarSys");
         #endif
         delete solarSys;
      }
   #endif
   
   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];

   if (sequence)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (sequence, "sequence", "Sandbox::~Sandbox()",
          " deleting mission sequence");
      #endif
      delete sequence;
   }
   
   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "Sandbox::~Sandbox()", " deleting objInit");
      #endif
      delete objInit;
   }
   
   // Delete the local objects
   Clear();
}


//------------------------------------------------------------------------------
// Setup methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// GmatBase* AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Adds an object to the Sandbox's object container.
 *
 *  Objects are added to the Sandbox by cloning the objects.  That way local
 *  copies can be manipulated without affecting the objects managed by the
 *  ConfigurationManager.
 *
 *  @param <obj> The object that needs to be included in the Sandbox.
 *
 *  @return Cloned object pointer if the object was added to the Sandbox's
 *          container, NULL if it was not.
 */
//------------------------------------------------------------------------------
//Changed to return GmatBase* (loj: 2008.11.06)
GmatBase* Sandbox::AddObject(GmatBase *obj)
{
   if (obj == NULL)
      return NULL;
   
   #ifdef DEBUG_SANDBOX_OBJ_ADD
      MessageInterface::ShowMessage
         ("Sandbox::AddObject() objTypeName=%s, objName=%s",
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
      
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state = IDLE;
   
   std::string name = obj->GetName();
   if (name == "")
      return NULL;  // No unnamed objects in the Sandbox tables
   
   GmatBase *cloned = obj;
   
   // Check to see if the object is already in the map
   if (FindObject(name) == NULL)
   {
      // If not, store the new object pointer
      #ifdef DEBUG_SANDBOX_CLONING
         MessageInterface::ShowMessage("Cloning %s <%p> -> ",
               obj->GetName().c_str(), obj);
      #endif
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage(
         "Cloning object %s of type %s\n", obj->GetName().c_str(),
         obj->GetTypeName().c_str());
      #endif
      
      cloned = obj->Clone();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (cloned, obj->GetName(), "Sandbox::AddObject()",
          "*cloned = obj->Clone()");
      #endif
      #ifdef DEBUG_SANDBOX_CLONING
      MessageInterface::ShowMessage("cloned=<%p>'%s'\n", cloned, cloned->GetName().c_str());
      #endif
      
      SetObjectByNameInMap(name, cloned);
      
      #ifdef DEBUG_SANDBOX_CLONING
         if (cloned->IsOfType(Gmat::PROP_SETUP))
            MessageInterface::ShowMessage("   PropSetup propagator <%p> -> "
                  "<%p>\n", ((PropSetup*)(obj))->GetPropagator(),
                  ((PropSetup*)(cloned))->GetPropagator());
      #endif
      
      // Register owned Subscribers with the Publisher
      Integer count = cloned->GetOwnedObjectCount();

      for (Integer i = 0; i < count; ++i)
      {
         GmatBase* oo = cloned->GetOwnedObject(i);
         if (oo->IsOfType(Gmat::SUBSCRIBER))
         {
            AddOwnedSubscriber((Subscriber*)oo);
         }
      }

      #ifdef DEBUG_SANDBOX_OBJ_ADD
         MessageInterface::ShowMessage(";  Original:  %p    Sandbox:  %p",
               obj, cloned);
      #endif
   }
   else
   {
      MessageInterface::ShowMessage
         ("in Sandbox::AddObject() %s is already in the map\n", name.c_str());
   }
   
   #ifdef DEBUG_SANDBOX_OBJ_ADD
      MessageInterface::ShowMessage("\n");
   #endif

   return cloned;
}

//------------------------------------------------------------------------------
// bool AddFunctionToGlobalObjectMap(Function *func)
//------------------------------------------------------------------------------
/**
 * This method is called from the Moderator when Sandbox triggers calling
 * Function during the Sandbox initialization, so that function created
 * inside a function can be added to Sanbox global object map
 */
//------------------------------------------------------------------------------
bool Sandbox::AddFunctionToGlobalObjectMap(Function *func)
{
   if (func == NULL)
      return false;
   
   if (func->GetName() == "")
      return false;
   
   std::string funcName = func->GetName();
   if (globalObjectMap.find(funcName) == globalObjectMap.end())
   {
      #ifdef DEBUG_SANDBOX_GMATFUNCTION
      MessageInterface::ShowMessage
         ("==> Sandbox::AddFunctionToGlobalObjectMap() adding function <%p>'%s' "
          "to global object map\n", func, funcName.c_str());
      #endif
      globalObjectMap.insert(make_pair(funcName, func));
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool AddCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 *  Adds a command to the Sandbox's command sequence.
 *
 *  Command are added to the command srquence by appending them ti the command
 *  list, using the GmatCommand::Append() method.
 *
 *  @param <cmd> The command that needs to be added to this Sandbox's sequence.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddCommand(GmatCommand *cmd)
{

   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

  state = IDLE;


   if (!cmd)
      return false;

   if (cmd == sequence)
      return true;

   if (sequence)
      return sequence->Append(cmd);


   sequence = cmd;
   return true;
}


//------------------------------------------------------------------------------
// bool AddSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 *  Sets the SolarSystem for this Sandbox by cloning the input solar system.
 *
 *  @param <ss> The SolarSystem this Sandbox's will use.
 *
 *  @return true if the solar system was added to the Sandbox, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSolarSystem(SolarSystem *ss)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;
   
   if (!ss)
      return false;
   
#ifdef DISABLE_SOLAR_SYSTEM_CLONING
   solarSys = ss;
#else
   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)            // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)       // made changes by TUAN NGUYEN
      MessageInterface::LogMessage("Cloning the solar system in the Sandbox\n");
   
   solarSys = (SolarSystem*)(ss->Clone());
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (solarSys, solarSys->GetName(), "Sandbox::AddSolarSystem()",
       "solarSys = (SolarSystem*)(ss->Clone())");
   #endif
   
   #ifdef DEBUG_SS_CLONING
   MessageInterface::ShowMessage("Sandbox cloned the solar system: %p\n", solarSys);
   #endif
#endif
   return true;
}


//------------------------------------------------------------------------------
// bool AddTriggerManagers(const std::vector<TriggerManager*> *trigs)
//------------------------------------------------------------------------------
/**
 * This method...
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool Sandbox::AddTriggerManagers(const std::vector<TriggerManager*> *trigs)
{
   bool retval = true;

   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];
   triggerManagers.clear();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Sandbox received %d trigger managers\n",
            trigs->size());
   #endif
   
   for (UnsignedInt i = 0; i < trigs->size(); ++i)
   {
      TriggerManager *trigMan = (*trigs)[i]->Clone();
      if (trigMan != NULL)
         triggerManagers.push_back(trigMan);
      else
      {
         MessageInterface::ShowMessage("Unable to clone a TriggerManager -- "
               "please check the copy constructor and assignment operator");
         retval = false;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state = IDLE;

   if (!cs)
      return false;

   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
   return true;
}


//------------------------------------------------------------------------------
// bool SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
/**
 *  Sets the Publisher so the Sandbox can pipe data to the rest of GMAT.
 *
 *  @param <pub> The GMAT Publisher.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetPublisher(Publisher *pub)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;
   
   if (pub)
   {
      publisher = pub;
      // Now publisher needs internal coordinate system
      publisher->SetInternalCoordSystem(internalCoordSys);
      return true;
   }
   
   if (!publisher)
      return false;
   
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(std::string name, UnsignedInt type)
//------------------------------------------------------------------------------
/**
 *  Accesses objects managed by this Sandbox.
 *
 *  @param <name> The name of the object.
 *  @param <name> type of object requested.
 *
 *  @return The pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* Sandbox::GetInternalObject(std::string name, UnsignedInt type)
{
   #ifdef DEBUG_INTERNAL_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::GetInternalObject() name=%s, type=%d\n", name.c_str(), type);
   #endif
   
   GmatBase* obj = NULL;
   
   if ((obj = FindObject(name)) != NULL) 
   {
      if (type != Gmat::UNKNOWN_OBJECT)
      {
         if (obj->GetType() != type) 
         {
            std::string errorStr = "GetInternalObject type mismatch for ";
            errorStr += name;
            throw SandboxException(errorStr);
         }
      }
   }
   else 
   {
      std::string errorStr = "Sandbox::GetInternalObject(" + name +
                             "...) Could not find \"";
      errorStr += name;
      errorStr += "\" in the Sandbox.";
      
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      ShowObjectMap(objectMap, "Here is the current object map:\n");
      ShowObjectMap(globalObjectMap, "Here is the current global object map:\n");
      #endif
      
      throw SandboxException(errorStr);
   }

   return obj;
}


//------------------------------------------------------------------------------
// std::map<std::string, GmatBase *> GetObjectMap()
//------------------------------------------------------------------------------
/**
* This function is used to get object map 
*
* @return The map of locally defined objects
*/
//------------------------------------------------------------------------------
std::map<std::string, GmatBase *> Sandbox::GetObjectMap()
{
   return objectMap;
}


//------------------------------------------------------------------------------
// std::map<std::string, GmatBase *> GetGlobalObjectMap()
//------------------------------------------------------------------------------
/**
 * Accessor for the global object map
 *
 * @return The map of globally defined objects
 */
//------------------------------------------------------------------------------
std::map<std::string, GmatBase *> Sandbox::GetGlobalObjectMap()
{
   return globalObjectMap;
}


//------------------------------------------------------------------------------
// Execution methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 *  Established the internal linkages between objects needed prior to running a
 *  mission sequence.
 *
 *  @return true if everything was connected properly, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Initialize()
{
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("\n==================== Sandbox::Initialize() entered\n");
      MessageInterface::ShowMessage("Initializing the Sandbox\n");
      MessageInterface::ShowMessage("Error in previous FCS = %d\n", errorInPreviousFcs);
      ShowObjectMap(objectMap, "Sandbox::Initialize() at the start, objectMap\n");
      ShowObjectMap(globalObjectMap, "Sandbox::Initialize() at the start, globalObjectMap\n");
      MessageInterface::ShowMessage("........\n");
   #endif

   bool rv = false;


   if (moderator == NULL)
      moderator = Moderator::Instance();
   
   transientForces.clear();
   events.clear();
   
   // Set transient force vector on Parameters that need it
   for (std::map<std::string,GmatBase*>::iterator i = objectMap.begin();
         i != objectMap.end(); ++i)
   {
      GmatBase *obj = (*i).second;
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         if (((Parameter*)obj)->NeedsForces())
            ((Parameter*)obj)->SetTransientForces(&transientForces);
      }
   }
   // Set transient force vector on Parameters in the GOM that need it
   for (std::map<std::string,GmatBase*>::iterator i = globalObjectMap.begin();
         i != globalObjectMap.end(); ++i)
   {
      GmatBase* obj = (*i).second;
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         if (((Parameter*)obj)->NeedsForces())
            ((Parameter*)obj)->SetTransientForces(&transientForces);
      }
   }

   // Already initialized
   if (state == INITIALIZED)
      return true;


   current = sequence;
   if (!current)
      throw SandboxException("No mission sequence defined in the Sandbox!");


   if (!internalCoordSys)
      throw SandboxException(
         "No reference (internal) coordinate system defined in the Sandbox!");


   ObjectMap::iterator omi;
   GmatBase *obj = NULL;
   std::string oName;
   std::string j2kName;


   // Set the solar system references
   if (solarSys == NULL)
      throw SandboxException("No solar system defined in the Sandbox!");
   
   // Initialize the solar system, internal coord system, etc.

   // Set J2000 Body for all SpacePoint derivatives before anything else
   // NOTE - at this point, everything should be in the SandboxObjectMap,
   // and the GlobalObjectMap should be empty
   #ifdef DEBUG_SANDBOX_OBJ_INIT
      MessageInterface::ShowMessage("About to create the ObjectInitializer ... \n");
      MessageInterface::ShowMessage(" and the objInit pointer is %s\n",
            (objInit? "NOT NULL" : "NULL!!!"));
   #endif

   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "Sandbox::Initialize()", " deleting objInit");
      #endif
      delete objInit;  // if Initialize is called more than once, delete 'old' objInit
   }
   
   objInit = new ObjectInitializer(solarSys, &objectMap, &globalObjectMap,
         internalCoordSys);
   
   objInit->SetWidgetCreator(pCreateWidget);

   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (objInit, "objInit", "Sandbox::Initialize()", "objInit = new ObjectInitializer");
   #endif
   try
   {
      #ifdef DEBUG_SANDBOX_OBJ_INIT
         MessageInterface::ShowMessage(
               "About to call the ObjectInitializer::InitializeObjects ... \n");
      #endif
      objInit->InitializeObjects();
   }
   catch (BaseException &be)
   {
      SandboxException se("");
      se.SetDetails("Error initializing objects in Sandbox.\n%s\n",
                    be.GetFullMessage().c_str());
      throw se;
      //throw SandboxException("Error initializing objects in Sandbox");
   }
   
   // Move global objects to the Global Object Store
   combinedObjectMap = objectMap;
   StringArray movedObjects;

   Real baseEpoch = -1.0;

   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;

      if (obj->IsOfType(Gmat::SPACECRAFT) && (baseEpoch == -1.0))
         baseEpoch = ((Spacecraft*)obj)->GetEpoch();

      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "Sandbox::checking object %s (of type %s) IsGlobal=%d\n",
            (omi->first).c_str(), (obj->GetTypeName()).c_str(), obj->IsGlobal());
      #endif
      // Check the isGlobal flag
      if (obj->IsGlobal())
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Sandbox::Initialie() Moving object <%p>'%s' to the Global Object Store\n",
               omi->second, (omi->first).c_str());
         #endif
         globalObjectMap.insert(*omi);
         movedObjects.push_back(omi->first);
         
         // Since CoordinateSystem is an automatic global object the origin also
         // should be in the global object map for GmatFunction.
         // So if an object is a coordinate system, add the clone of origin to global
         // object map if it is not found in the objectMap or in the solar system. If
         // it is found in the objectMap just move it to global object map.
         // Hopefully this will fix DeveloperTests/FUNCTION_Bug1688_UnusedCsInFunction (LOJ: 2015.09.23)
         if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
         {
            std::string originName = obj->GetStringParameter("Origin");
            GmatBase *origin = obj->GetRefObject(Gmat::SPACE_POINT, originName);
            #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage
               ("==> Object is CoordinateSystem and originName='%s', origin=<%p>[%s]'%s'\n",
                originName.c_str(), origin, origin ? origin->GetTypeName().c_str() : "NULL",
                origin ? origin->GetName().c_str() : "NULL");
            #endif
            if (origin && !origin->IsOfType(Gmat::CELESTIAL_BODY))
            {
               // If origin found in the objectMap then move it to globalObjectMap, otherwise
               // add clone of origin to globalObjectMap (2015.09.22)
               bool cloneOrigin = false;
               if (globalObjectMap.find(originName) == globalObjectMap.end())
               {
                  if (objectMap.find(originName) == objectMap.end())
                     cloneOrigin = true;
                  
                  // If not found in the SolarSystem
                  if (solarSys->GetBody(originName) == NULL)
                  {
                     origin->SetIsGlobal(true);
                     if (cloneOrigin)
                     {
                        #ifdef DEBUG_SANDBOX_INIT
                        MessageInterface::ShowMessage
                           ("Sandbox::Initialize() Copying origin <%p>'%s' of CS <%p>'%s' to global object map\n",
                            origin, originName.c_str(), obj, obj->GetName().c_str());
                        #endif
                        globalObjectMap.insert(make_pair(originName, origin->Clone()));
                     }
                     else
                     {
                        #ifdef DEBUG_SANDBOX_INIT
                        MessageInterface::ShowMessage
                           ("Sandbox::Initialize() Moving origin <%p>'%s' of CS <%p>'%s' to global object map\n",
                            origin, originName.c_str(), obj, obj->GetName().c_str());
                        #endif
                        globalObjectMap.insert(make_pair(originName, origin));
                        movedObjects.push_back(originName);
                     }
                  }
               }
            }
         }
      }
   }
   
   // Set global epoch to first one found (or -1.0) so that a previous run doesn't interfere
   GmatGlobal::Instance()->SetCurrentEpoch(baseEpoch);

   for (unsigned int ii = 0; ii < movedObjects.size(); ii++)
      objectMap.erase(movedObjects.at(ii));
   movedObjects.clear();  
   
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("--- Right AFTER moving things to the GOS --- \n");
      ShowObjectMap(objectMap, "Sandbox Object Map contains\n");
      ShowObjectMap(globalObjectMap, "Sandbox Global Object Map contains\n");
   #endif
   
   StringArray exceptions;
   IntegerArray exceptionTypes;
   UnsignedInt exceptionCount = 0;

   // Construct equations inside of resources -- move up so globals are set?
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      if (omi->second->HasEquation())
      {
         obj = omi->second;
         obj->SetupEquation(&objectMap);

         std::vector<RHSEquation*> equations;
         obj->GetEquations(equations);
         for (Integer j = 0; j < equations.size(); ++j)
         {
            std::string equationString;
            RHSEquation *equation = equations[j];
            Validator::Instance()->ValidateEquation(equation);
            equation->GetMathTree(false)->SetObjectMap(&objectMap);
            equation->GetMathTree(false)->SetGlobalObjectMap(&globalObjectMap);

//            // Reset object pointers to the Sandbox clones
//            StringArray references = equation->GetWrapperObjectNames();
//            for (UnsignedInt i = 0; i < references.size(); ++i)
//            {
//               GmatBase *obj = GetObject(references[i]);
//               if (obj)
//                  equation->
//            }
         }

         if (!obj->InitializeEquations(&objectMap, &globalObjectMap))
            MessageInterface::ShowMessage("Equation initialization failed "
                  "for %s\n", obj->GetName().c_str());
      }
   }

   
   // Set the EventLocators
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::EVENT_LOCATOR))
      {
         #ifdef DEBUG_EVENTLOCATION
            MessageInterface::ShowMessage("Testing event locator %s...\n",
                  obj->GetName().c_str());
         #endif
         if (find(events.begin(), events.end(), (EventLocator*)obj) == events.end())
         {
            #ifdef DEBUG_EVENTLOCATION
               MessageInterface::ShowMessage("Adding event locator %s to the "
                     "event list\n", obj->GetName().c_str());
            #endif
            events.push_back((EventLocator*)obj);
         }
      }
   }
   for (omi = globalObjectMap.begin(); omi != globalObjectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::EVENT_LOCATOR))
      {
         if (find(events.begin(), events.end(), (EventLocator*)obj) == events.end())
         {
            #ifdef DEBUG_EVENTLOCATION
               MessageInterface::ShowMessage("Adding event locator %s to the "
                     "event list\n", obj->GetName().c_str());
            #endif
            events.push_back((EventLocator*)obj);
         }
      }
   }

   #ifdef DEBUG_SANDBOX_INIT
   MessageInterface::ShowMessage
      ("Sandbox::Initialize() Initializing Commands...\n");
   #endif
   
   // Initialize commands
   warnPyInterface = 0;
   while (current)
   {
      try
      {
         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() Setting TriggerManager to [%s] command\n   \"%s\"\n",
             current->GetTypeName().c_str(),
             current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         
         current->SetTriggerManagers(&triggerManagers);
         
         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() Setting objectMap, globalObjectMap to [%s] command\n",
             current->GetTypeName().c_str());
         #endif
         
         current->SetObjectMap(&objectMap);
         current->SetGlobalObjectMap(&globalObjectMap);
         SetGlobalRefObject(current);

         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() Checking if [%s] is CallFunction or Assignment command, "
             "so that GmatFunction can be built if needed\n", current->GetTypeName().c_str());
         #endif
         // Handle GmatFunctions
         if ((current->IsOfType("CallFunction")) ||
             (current->IsOfType("Assignment")))
         {
            #ifdef DEBUG_SANDBOX_GMATFUNCTION
               MessageInterface::ShowMessage(
                  "CallFunction or Assignment found in MCS: calling HandleGmatFunction \n");
            #endif
            if (!current->IsOfType("CallPythonFunction"))
               HandleGmatFunction(current, &combinedObjectMap);
         }
         
         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() Checking if [%s] is BranchCommand, so that GmatFunction "
             "inside branch command can be built if needed\n", current->GetTypeName().c_str());
         #endif
         if (current->IsOfType("BranchCommand"))
         {
            std::vector<GmatCommand*> cmdList = ((BranchCommand*) current)->GetCommandsWithGmatFunctions();
            Integer sz = (Integer) cmdList.size();
            #ifdef DEBUG_SANDBOX_GMATFUNCTION
               MessageInterface::ShowMessage("... returning %d functions with GmatFunctions\n", sz);
            #endif
            for (Integer jj = 0; jj < sz; jj++)
            {
               HandleGmatFunction(cmdList.at(jj), &combinedObjectMap);
               (cmdList.at(jj))->SetInternalCoordSystem(internalCoordSys);
            }
         }
         
         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Sandbox::Initialize() Initializing [%s] command\n", current->GetTypeName().c_str());
         #endif
         try
         {
            rv = current->Initialize();
            if (!rv)
               throw SandboxException("The Mission Control Sequence command\n\n" +
                     current->GetGeneratingString(Gmat::SCRIPTING, "   ") +
                     "\n\nfailed to initialize correctly.  Please correct the error "
                     "and try again.");
         }
         catch (BaseException &)
         {
            // Call ValidateCommand to create wrappers and Initialize.(LOJ: 2010.08.24)
            // This will fix bug 1918 for the following scenario in ScriptEvent.
            // In ScriptEvent, x = 1 where x is undefined, save it.
            // Add x from the ResourceTree and run the mission.
            moderator->ValidateCommand(current);
            rv = current->Initialize();
         }
         
         // Find the last Vary command in the SolverBranchCommand to chunk
         // the ephemeris data. It is done here since this needs to be done
         // only once. I was going to do this in BranchCommand::Initialize()
         // but Initialize() is called many times during the run.
         // (For GMT-4097 fix LOJ: 2014.04.04)
         if (current->IsOfType("SolverBranchCommand"))
         {
            #ifdef DEBUG_SANDBOX_SOLVERBRANCH
            MessageInterface::ShowMessage
               ("Sandbox::Initialize() calling TakeAction() for action 'SetLastVaryCommand "
                "after BranchCommand <%p>[%s] is initialized\n",
                current, current->GetTypeName().c_str());
            #endif
            current->TakeAction("SetLastVaryCommand");
         }
         
         // Check to see if the command needs a server startup
         if (current->NeedsServerStartup())
            if (moderator->StartMatlabServer() == false)
               throw SandboxException("Unable to start the server needed by the " +
                        (current->GetTypeName()) + " command");
      }
      catch (BaseException &be)
      {
         // Added trap for the Python exception so it only fires one time
         if (ReportError(be))
         {
            ++exceptionCount;
            exceptionTypes.push_back(be.GetMessageType());
            exceptions.push_back(be.GetFullMessage());
         }
      }
      current = current->GetNext();
   }
   
   // Initialize equations found in the object stores
   EquationInitializer eqInitializer(solarSys, &objectMap, &globalObjectMap,
         internalCoordSys);
   if (!eqInitializer.PrepareEquationsForMapObjects())
   {
      // Add equation exceptions here; for now, just set
      rv = false;
   }

   if (exceptionCount > 0)
   {
      for (UnsignedInt i = 0; i < exceptionCount; ++i)
      {
         // Add error count only if message type is Gmat::ERROR_ (Bug 2272 fix)
         if (exceptionTypes[i] == Gmat::ERROR_)
             MessageInterface::ShowMessage("%d: %s\n", i+1, exceptions[i].c_str());
         else
            MessageInterface::ShowMessage("%s\n", exceptions[i].c_str());
      }
      throw SandboxException("Errors were found in the mission control "
            "sequence; please correct the errors listed in the message window");
   }

   #ifdef DEBUG_SANDBOX_INIT
   MessageInterface::ShowMessage
      ("==================== Sandbox::Initialize() Successfully initialized\n");
   #endif

   state = INITIALIZED;
   
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 *  Runs the mission sequence.
 *
 *  This method walks through the command linked list, firing each GmatCommand
 *  as it is encountered by calling Execute() on the commands.  Between command
 *  executions, the method check with the Moderator to see if the user has
 *  requested that the sequence be paused or halted.
 *
 *  @return true if the mission sequence was executed, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Execute()
{
   #if DBGLVL_SANDBOX_RUN > 0
   MessageInterface::ShowMessage("\n==================== Sandbox::Execute() entered\n");
   #if DBGLVL_SANDBOX_RUN > 1
   ShowObjectMap(objectMap, "Sandbox::Execute() Here is the current object map:\n");
   ShowObjectMap(globalObjectMap, "Sandbox::Execute() Here is the current global object map:\n");
   MessageInterface::ShowMessage("Sandbox::Execute() Here is the mission sequence:\n");
   std::string seq = GmatCommandUtil::GetCommandSeqString(sequence);
   MessageInterface::ShowMessage(seq);
   #endif
   #endif
   
   bool rv = true;
   Integer cloneIndex;

   state = RUNNING;
   Gmat::RunState runState = Gmat::IDLE, currentState = Gmat::RUNNING;
   
   current = sequence;
   if (!current)
      return false;

   try
   {
      while (current)
      {
         // First check to see if the run should be interrupted
         if (Interrupt())
         {
            #ifdef DEBUG_MODERATOR_CALLBACK
            MessageInterface::ShowMessage("   Interrupted in %s command\n",
                                          current->GetTypeName().c_str());
            #endif
            
            
            if (state == PAUSED)
            {
               publisher->Ping();
               continue;
            }
            else
            {
               //MessageInterface::ShowMessage("Sandbox::Execute() interrupted.\n");
               sequence->RunComplete(true);
               
               // notify subscribers end of run
               currentState = Gmat::IDLE;
               publisher->SetRunState(currentState);
               publisher->NotifyEndOfRun();
               
               // Write out event data, if any, and if we are writing it in
               // "Automatic mode"
               for (UnsignedInt i = 0; i < events.size(); ++i)
               {
                  if (events[i]->IsInAutomaticMode())
                     events[i]->LocateEvents("Execution was interrupted; the "
                                "event list may be incomplete");
               }
               throw SandboxException("Execution interrupted");
               //return rv;
            }
         }
         
         #if DBGLVL_SANDBOX_RUN
         if (current != NULL)
         {
            MessageInterface::ShowMessage
               ("Sandbox::Execute() .................... RUNNING [%s]\n", current->GetTypeName().c_str());
            
            #if DBGLVL_SANDBOX_RUN > 1
            MessageInterface::ShowMessage
               ("command = '%s'\n", current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
         }
         #endif
         
         if (currentState != runState)
         {
            publisher->SetRunState(currentState);
            runState = currentState;
         }
         
         if (current->AffectsClones())
         {
            // Manage owned clones
//            GmatBase *obj = current->GetUpdatedObject();

            cloneIndex = current->GetUpdatedObjectParameterIndex();

            #ifdef DEBUG_CLONE_UPDATES
               MessageInterface::ShowMessage("   Index %d retrieved from %s "
                     "command\n", cloneIndex, current->GetTypeName().c_str());
            #endif
         }
         else
            cloneIndex = -1;
         
         //================================================================
         // @note: This section is just testing out how the run can be
         // paused by user when branch command is running, such as Optimize.
         // Catch exception when BranchCommand throws with "RUN PAUSED"
         // so that catch block can just continue.
         // Need more work on this (LOJ: 2014.09.10)
         // Added try/catch block to catch exception when user paused the run
         try
         {
            // Update here if we add another runmode to show command location

            // Printing of command is skipped for propagate after it has 
            // already been printed once
            bool skipPrint = false;
            if (GmatGlobal::Instance()->EchoCommands())
            {
               if (current->IsOfType("Propagate"))
               {
                  bool currentPropStatus = current->GetPropStatus();
                  if (currentPropStatus)
                     skipPrint = true;
               }

               else
               {
                  if (current->IsOfType("BranchCommand"))
                  {
                     bool currentPropStatus = current->GetPropStatus();
                     if (currentPropStatus)
                        skipPrint = true;
                  }
               }

               if (!skipPrint)
               {
                  std::string currentCommand = "CurrentCommand: " +
                     current->GetGeneratingString(Gmat::NO_COMMENTS);
                  if (current->IsOfType("BranchCommand"))
                  {
                     std::string substring = ((BranchCommand*)current)->
                        GetCurrentGeneratingString(Gmat::NO_COMMENTS, "  --> ");
                     if (substring != "")
                        currentCommand = substring;
                  }
                  MessageInterface::ShowMessage("%s\n", currentCommand.c_str());
               }
            }

            rv = current->Execute();
         
            if (!rv)
            {
               std::string str = "\"" + current->GetTypeName() +
                  "\" Command failed to run to completion\n";
               
               #if DBGLVL_SANDBOX_RUN > 1
               MessageInterface::ShowMessage
                  ("%sCommand Text is\n\"%s\n", str.c_str(),
                   current->GetGeneratingString().c_str());
               #endif
               
               throw SandboxException(str);
            }
            
            if (current->AffectsClones())
            {
               // Manage owned clones
               GmatBase *obj = current->GetUpdatedObject();
               cloneIndex = current->GetUpdatedObjectParameterIndex();
               
               if (obj != NULL)
               {
                  runMode retState = state;
                  state = REFRESH;
                  UpdateClones(obj, cloneIndex);
                  state = retState;
                  
                  if (obj->IsInitialized() == false)
                  {
                     #ifdef DEBUG_CLONE_UPDATES
                     bool retval =
                     #endif
                        
                     obj->Initialize();
                     
                     #ifdef DEBUG_CLONE_UPDATES
                         if (retval == false)
                            MessageInterface::ShowMessage("%s needs initialization"
                                  ", but initialization fails\n", 
                                  obj->GetName().c_str());
                     #endif
                  }
               }
            }
            
            current = current->GetNext();
         }
         catch (BaseException &e)
         {
            #ifdef __ENABLE_PAUSE_IN_BRANCH_COMMAND__
            //================================================================
            // Just continue when BranchCommand throws with "RUN PAUSED"
            if (e.GetFullMessage().find("RUN PAUSED") != std::string::npos)
               continue;
            
            #else

            //Adds in line number and script to error
            std::string errormsg = e.GetDetails();
            errormsg = current->AddFileInfoToErrorMessage(errormsg);
            e.SetDetails(errormsg);
            throw;
            
            #endif
         }
      } // while (current)
   }
   catch (BaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      sequence->RunComplete(true);
      state = STOPPED;
      
      #if DBGLVL_SANDBOX_RUN
      MessageInterface::ShowMessage
         ("Sandbox::Execute() rethrowing %s\n", e.GetFullMessage().c_str());
      #endif
      
      // Set FCS eror flag (LOJ: 2015.10.07)
      errorInPreviousFcs = true;
      throw;
   }
   
   sequence->RunComplete(true);
   state = STOPPED;
   
   // notify subscribers end of run
   currentState = Gmat::IDLE;
   publisher->SetRunState(currentState);
   publisher->NotifyEndOfRun();
   
   // Write out event data, if any, and if we are writing in
   // "Automatic mode"
   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if (events[i]->IsInAutomaticMode())
         events[i]->LocateEvents();
   }
   
   #if DBGLVL_SANDBOX_RUN > 0
   MessageInterface::ShowMessage("==================== Sandbox::Execute() leaving\n");
   #endif
   
   return rv;
}



//------------------------------------------------------------------------------
// bool Interrupt()
//------------------------------------------------------------------------------
/**
 *  Tests to see if the mission sequence should be interrupted.
 *
 *  @return true if the Moderator wants to interrupt execution, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Interrupt()
{
   // Ask the moderator for the current RunState; only check at fixed frequency
   if (++interruptCount == pollFrequency)
   {
      Gmat::RunState interruptType =  moderator->GetUserInterrupt();
   
      switch (interruptType)
      {
         case Gmat::PAUSED:   // Pause
            state = PAUSED;
            break;
   
         case Gmat::IDLE:     // Stop puts GMAT into the Idle state
            state = STOPPED;
            break;
   
         case Gmat::RUNNING:   // MCS is running
            state = RUNNING;
            break;
   
         default:
            break;
      }
      interruptCount = 0;
   }
   
   if ((state == PAUSED) || (state == STOPPED))
      return true;

   return false;
}


//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 *  Cleans up the local object store.
 */
//------------------------------------------------------------------------------
void Sandbox::Clear()
{
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("Sandbox::Clear() entered, errorInPreviousFcs=%d\n", errorInPreviousFcs);
   #endif
   
   sequence  = NULL;
   current   = NULL;
   
   // Delete the all cloned objects
   ObjectMap::iterator omi;
   
   #ifdef DEBUG_SANDBOX_CLEAR
   ShowObjectMap(objectMap, "Sandbox::Clear() =====> Before clearing objectMap\n");
   #endif
   
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Sandbox clearing <%p>'%s' from objectMap\n", omi->second,
            (omi->first).c_str());
      #endif

      if (omi->second != NULL)
      {
         // if object is a SUBSCRIBER, let's unsubscribe it first
         if ((omi->second != NULL) && (omi->second)->GetType() == Gmat::SUBSCRIBER)
            publisher->Unsubscribe((Subscriber*)(omi->second));
         
         // If not PropSetup since PropSetup::GetOwnedObject() starts from count 1
         // @note How should we handle this in other object?
         if ((omi->second)->GetType() != Gmat::PROP_SETUP)
         {
            // Unsubscribe owned Subscribers too
            Integer count = (omi->second)->GetOwnedObjectCount();
            for (Integer i = 0; i < count; ++i)
            {
               if ((omi->second)->GetOwnedObject(i)->IsOfType(Gmat::SUBSCRIBER))
                  publisher->Unsubscribe((Subscriber*)
                                         ((omi->second)->GetOwnedObject(i)));
            }
         }
         
         #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("   Deleting <%p>'%s'\n", omi->second,
            (omi->second)->GetName().c_str());
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (omi->second, omi->first, "Sandbox::Clear()",
             " deleting cloned obj from objectMap");
         #endif

         // Check to see if object already stored in global
         std::string localName = omi->first;
         ObjectMap::iterator globalIt = globalObjectMap.find(localName);
         if (globalIt != globalObjectMap.end())
         {
            // If the same pointer is being used, let global handle deletion
            if (globalIt->second == omi->second)
            {
               omi->second = NULL;
               continue;
            }
         }

         delete omi->second;
         omi->second = NULL;

         #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("   Deleting completed\n");
         #endif

         // Commented out since this causes crash when re-run or exit GMAT (LOJ: 2015.03.26)
         //objectMap.erase(omi++);
      }
   }
   objectMap.clear();
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("=====> Sandbox::Clear() deleting objects from objectMap done\n");
   ShowObjectMap(objectMap, "Sandbox::Clear() After clearing objectMap\n");
   ShowObjectMap(globalObjectMap, "Sandbox::Clear() Before clearing globalObjectMap\n");
   #endif
   for (omi = globalObjectMap.begin(); omi != globalObjectMap.end(); omi++)
   {
      #ifdef DEBUG_SANDBOX_CLEAR
         MessageInterface::ShowMessage("Sandbox deleting <%p>'%s' from globalObjectMap\n",
            omi->second, (omi->first).c_str());
      #endif
      
      if (omi->second != NULL)
      {
         GmatBase *globalObj = omi->second;
         
         #ifdef DEBUG_SANDBOX_CLEAR
         MessageInterface::ShowMessage
            ("   globalObj=<%p>[%s]'%s'\n", globalObj, globalObj->GetTypeName().c_str(),
             globalObj->GetName().c_str());
         #endif
         
         // If object is a function and failed to create FCS, skip
         // So that after function is fixed, it can run without a crash
         //@todo Need to revisit clearing failed function object here (LOJ: 2015.09.28)
         if (globalObj->IsOfType(Gmat::FUNCTION) && errorInPreviousFcs)
         {
            #ifdef DEBUG_SANDBOX_CLEAR
            MessageInterface::ShowMessage
               ("   ==> Skip deleting '%s', since it failed to create FCS\n", globalObj->GetName().c_str());
            #endif
            continue;
         }
         
         #ifdef DEBUG_SANDBOX_CLEAR
         MessageInterface::ShowMessage("   ==> Continue with deleting\n");
         #endif
         
         // if object is a SUBSCRIBER, let's unsubscribe it first
         if ((globalObj)->GetType() == Gmat::SUBSCRIBER)
            publisher->Unsubscribe((Subscriber*)(globalObj));
         
         // Unsubscribe owned Subscribers too
         // If not PropSetup since PropSetup::GetOwnedObject() starts from count 1
         // @note How should we handle this in other object?
         //if (!((globalObj)->IsOfType(Gmat::PROP_SETUP)))
         if ((globalObj)->GetType() != Gmat::PROP_SETUP)
         {
            Integer count = (globalObj)->GetOwnedObjectCount();
            #ifdef DEBUG_SANDBOX_CLEAR
            MessageInterface::ShowMessage
               ("   ownedObjectCount of '%s' = %d\n", (globalObj)->GetName().c_str(), count);
            #endif
            for (Integer i = 0; i < count; ++i)
            {
               if ((globalObj)->GetOwnedObject(i)->IsOfType(Gmat::SUBSCRIBER))
                  publisher->Unsubscribe((Subscriber*)
                                         ((globalObj)->GetOwnedObject(i)));
            }
         }
         
         #ifdef DEBUG_SANDBOX_CLEAR
         MessageInterface::ShowMessage("   Deleting <%p>'%s'\n", globalObj,
            (globalObj)->GetName().c_str());
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (globalObj, omi->first, "Sandbox::Clear()",
             " deleting cloned obj from globalObjectMap");
         #endif
         delete globalObj;
         globalObj = NULL;
         // Commented out since this causes crash when re-run or exit GMAT (LOJ: 2015.03.26)
         //globalObjectMap.erase(omi++);
      }
   }
   
   globalObjectMap.clear();
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("=====> Sandbox::Clear() deleting objects from globalObjectMap done\n");
   ShowObjectMap(globalObjectMap, "Sandbox::Clear() After clearing globalObjectMap\n");
   #endif
   
   // Clear published data
   if (publisher)
   {
      publisher->ClearPublishedData();
      #ifdef DEBUG_SANDBOX_CLEAR
      MessageInterface::ShowMessage
         ("--- Sandbox::Clear() publisher cleared published data\n");
      #endif
   }
   
   // Set publisher to NULL. The publisher is set before the run and this method
   // Sandbox::Clear() can be called multiple times from the Moderator
   publisher = NULL;
   
#ifndef DISABLE_SOLAR_SYSTEM_CLONING
   if (solarSys != NULL)
   {
      #ifdef DEBUG_SS_CLONING
      MessageInterface::ShowMessage
         ("--- Sandbox::Clear() deleting the solar system clone: %p\n", solarSys);
      #endif
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (solarSys, solarSys->GetName(), "Sandbox::Clear()", " deleting solarSys");
      #endif
      delete solarSys;
   }
   
   solarSys = NULL;
#endif
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() now about to delete triggerManagers\n");
   #endif
   // Remove the TriggerManager clones
   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];
   triggerManagers.clear();
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() triggerManagers are cleared\n");
   #endif
   
   // Don't delete objects.  ConfigManager::RemoveAllItems() deletes them
   objectMap.clear();
   globalObjectMap.clear();
   
   #ifdef DEBUG_TRANSIENT_FORCES
   MessageInterface::ShowMessage
      ("Sandbox::Clear() transientForces<%p> has %d transient forces\n",
       &transientForces, transientForces.size());
   #endif

   // BeginFiniteBurn pushes burn forces onto the transient force vector.
   // We don't delete transient forces here; the burn forces are deleted in
   // the BeginFiniteBurn destructor.
   #if 0
   for (std::vector<PhysicalModel*>::iterator tf = transientForces.begin();
        tf != transientForces.end(); ++tf)
   {
      #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage("   tf=<%p>\n", (*tf));
      #endif
      
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove((*tf), (*tf)->GetName(),
               "Sandbox::Clear()", "deleting transient force");
      #endif
      delete (*tf);
   }
   #endif
   
   transientForces.clear();
   
   events.clear();

   // Update the sandbox state
   if ((state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state     = IDLE;
   errorInPreviousFcs = false;
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage("Sandbox::Clear() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool AddSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
/**
 *  Add Subcribers to the Sandbox and registers them with the Publisher.
 *
 *  @param sub The subscriber.
 *
 *  @return true if the Subscriber was registered.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSubscriber(Subscriber *sub)
{
   // add subscriber to sandbox by AddObject() so that cloned subscribers
   // can be deleted when clear (loj: 2008.11.06)
   Subscriber *newSub = (Subscriber*)AddObject(sub);
   if (newSub != NULL)
   {
      publisher->Subscribe(newSub);
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
//------------------------------------------------------------------------------
/**
 * Sets the callback function used to build plugin widgets
 *
 * @param creatorFun The callback function
 */
//------------------------------------------------------------------------------
void Sandbox::SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
{
   pCreateWidget = creatorFun;
}

//------------------------------------------------------------------------------
// bool AddOwnedSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
/**
 *  Registers owned Subcribers with the Publisher.
 *
 *  @param <sub> The subscriber.
 *
 *  @return true if the Subscriber was registered.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddOwnedSubscriber(Subscriber *sub)
{
   publisher->Subscribe(sub);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Sandbox::FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 *  Finds an object by name, searching through the SandboxObjectMap first,
 *  then the GlobalObjectMap
 *
 *  @param spname The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          Sandbox.
 */
//------------------------------------------------------------------------------
GmatBase* Sandbox::FindObject(const std::string &name)
{
   if (objectMap.find(name) == objectMap.end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (globalObjectMap.find(name) == globalObjectMap.end())
         return NULL;
      else return globalObjectMap[name];
   }
   else
      return objectMap[name];
}


//------------------------------------------------------------------------------
// bool Sandbox::SetObjectByNameInMap(const std::string &name,
//                                    GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Sets the object pointer for the given name in the object map(s).  NOTE that
 *  an object should only exist in one of the object maps, so both IFs should
 *  not evaluate to TRUE.
 *
 *  @param <name> The name of the object.
 *  @param <obj>  The object pointer.
 *
 *  @return true if successful; flase otherwise
 */
//------------------------------------------------------------------------------
bool Sandbox::SetObjectByNameInMap(const std::string &name,
                                   GmatBase *obj)
{
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
   MessageInterface::ShowMessage
      ("Sandbox::SetObjectByNameInMap() entered, name = %s\n", name.c_str());
   #endif
   bool found = false;
   // if it's already in a map, set the object pointer for the name
   if (objectMap.find(name) != objectMap.end())
   {
      objectMap[name] = obj;
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage
         ("Sandbox::SetObjectByNameInMap() set object name = %s in objectMap\n",
          name.c_str());
      #endif
      found = true;
   }
   else if (globalObjectMap.find(name) != globalObjectMap.end())
   {
      globalObjectMap[name] = obj;
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage
         ("Sandbox::SetObjectByNameInMap() set object name = %s in globalObjectMap\n",
          name.c_str());
      #endif      
      found = true;
   }
   // if not already in the map, add it to the objectMap
   // (globals are added to the globalObjectMap later)
   if (!found)
   {
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage
         ("Sandbox::SetObjectByNameInMap() '%s' not in objectMap nor in globalObjectMap "
          "so adding to objectMap\n", name.c_str());
      #endif
      objectMap.insert(std::make_pair(name,obj));
   }
   
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
   MessageInterface::ShowMessage
      ("Sandbox::SetObjectByNameInMap() returning found = %s\n",
       (found? "TRUE" : "FALSE"));
   #endif   
   return found;
}

//------------------------------------------------------------------------------
// bool Sandbox::HandleGmatFunction(GmatCommand *cmd,
//                                  ObjectMap *usingMap)
//------------------------------------------------------------------------------
/**
 *  Handles any GmatFunctions included in the sequence.  The input cmd is the 
 *  CallFunction or Assignment command to process - it may itself contain a nested
 *  GmatFunction.  If it does, this method will be called recursively to process
 *  the nested GmatFunctions.
 *
 *  @param <name>     The cmd.
 *  @param <usingMap> The map to send to the Interpreter (via the Moderator).
 *
 *  @return true if successful; flase otherwise
 */
//------------------------------------------------------------------------------
bool Sandbox::HandleGmatFunction(GmatCommand *cmd,
                                 std::map<std::string,  GmatBase *> *usingMap)
{
   #ifdef DEBUG_SANDBOX_GMATFUNCTION
      MessageInterface::ShowMessage(
         "Sandbox::HandleGmatFunction() entered, with command of type %s,\n   '%s'\n",
         (cmd->GetTypeName()).c_str(), cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
      
   bool OK = false;
   GmatGlobal *global = GmatGlobal::Instance();
   std::string matlabExt = global->GetMatlabFuncNameExt();
   StringArray gfList;
   bool        isMatlabFunction = false;
   
   SetGlobalRefObject(cmd);
   
   // Check for CallGmatFunction
   if (cmd->IsOfType("CallGmatFunction"))
   {
      std::string cfName = cmd->GetStringParameter("FunctionName");
      gfList.push_back(cfName);
   }
   else if (cmd->IsOfType("Assignment"))  
   {
      gfList = ((Assignment*) cmd)->GetGmatFunctionNames();
   }
   
   #ifdef DEBUG_SANDBOX_GMATFUNCTION
   MessageInterface::ShowMessage("   Has %d GmatFunctions\n", gfList.size());
   #endif
   
   for (unsigned int ii = 0; ii < gfList.size(); ii++)
   {
      std::string fName = gfList.at(ii);
      Function *f;
      isMatlabFunction = false;
      // if it's a Matlab Function, remove the extension from the name before looking in the GOS
      // (as Matlab function names are placed into the GOS without the extension)
      if (fName.find(matlabExt) != fName.npos)
      {
         fName = fName.substr(0, fName.find(matlabExt));
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage
            ("   actual matlab function name='%s'\n", fName.c_str());
         #endif
         isMatlabFunction = true;
      }
      #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage("Now searching GOS for object %s\n",
            (gfList.at(ii)).c_str());
      #endif
      // if there is not already a function of that name, create it
      if (globalObjectMap.find(fName) == globalObjectMap.end())
      {
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage("Sandbox creating function '%s'\n", fName.c_str());
         #endif
         if (isMatlabFunction)
            f = moderator->CreateFunction("MatlabFunction",fName, 0);
         else
            f = moderator->CreateFunction("GmatFunction",fName, 0);
         if (!f) 
            throw SandboxException("Sandbox::HandleGmatFunction - error creating new function\n");
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage
            ("Adding function <%p>'%s' to the Global Object Store\n", f, fName.c_str());
         #endif
         globalObjectMap.insert(std::make_pair(fName,f));
      }
      else // it's already in the GOS, so just grab it
         f = (Function*) globalObjectMap[fName];
      
      if (cmd->IsOfType("CallFunction"))
      {
         ((CallFunction*)cmd)->SetRefObject(f,Gmat::FUNCTION,fName);
         cmd->SetStringParameter("FunctionName", fName);
      }
      else if (cmd->IsOfType("Assignment"))
         ((Assignment*) cmd)->SetFunction(f);
      
      #ifdef DEBUG_SANDBOX_GMATFUNCTION
      MessageInterface::ShowMessage
         ("Function type: %s, name: %s\n", f->GetTypeName().c_str(),
          f->GetStringParameter("FunctionName").c_str());
      if (f->IsOfType("UserDefinedFunction"))
      {
         UserDefinedFunction *udf = (UserDefinedFunction*)f;
         MessageInterface::ShowMessage(
            "Now handling function whose fcs is %s set, ",
            ((udf->IsFunctionControlSequenceSet())? "already" : "NOT"));
         MessageInterface::ShowMessage
            ("Errors were %sfound.\n", udf->ErrorFound() ? "" : "not ");
      }
      #endif
      
      // if function is GmatFunction and no FCS has built and no script error found,
      // build FCS
      if (f->IsOfType("UserDefinedFunction"))
      {
         UserDefinedFunction* udf = (UserDefinedFunction*)f;
         // if ((f->GetTypeName() == "GmatFunction") &&
         //     (!(f->IsFunctionControlSequenceSet())) &&
         //     (!(f->ScriptErrorFound())))
         if (!(udf->IsFunctionControlSequenceSet()) &&
             (!(udf->ErrorFound())))
         {
            //// Use udf instead of f for GMT-5942 (LOJ:2016.10.25)
            // Set SolarSystem first for parsing in two modes (LOJ: 2015.02.06)
            udf->SetSolarSystem(solarSys);
            
            #ifdef DEBUG_SANDBOX_GMATFUNCTION
            MessageInterface::ShowMessage(
               "About to call InterpretGmatFunction for function '%s'\n",
               (udf->GetStringParameter("FunctionPath")).c_str());
            #endif
            GmatCommand* fcs = moderator->InterpretGmatFunction(udf, usingMap, solarSys);
            
            // If FCS not created, throw an exception with Gmat::GENERAL_ so that it will not
            // write error count again for function in Initialize()(Bug 2272 fix)
            if (fcs == NULL)
            {
               errorInPreviousFcs = true;
               throw SandboxException("Sandbox::HandleGmatFunction - error creating FCS\n",
                                      Gmat::GENERAL_);
            }
            
            errorInPreviousFcs = false;
            udf->SetFunctionControlSequence(fcs);
            GmatCommand* fcsCmd = fcs;
            while (fcsCmd)
            {
               #ifdef DISALLOW_NESTED_GMAT_FUNCTIONS
               if (fcsCmd->HasAFunction())
               {
                  std::string errMsg = "Sandbox::HandleGmatFunction (";
                  errMsg += fName + ") - nested or recursive GmatFunctions not yet supported.\n";
                  throw SandboxException(errMsg);
               }
               #endif
               
               // todo: Figure out what makes sense for the "events" container and functions
               
               if ((fcsCmd->IsOfType("CallFunction")) ||
                   (fcsCmd->IsOfType("Assignment")))
               {
                  #ifdef DEBUG_SANDBOX_GMATFUNCTION
                  MessageInterface::ShowMessage(
                     "CallFunction or Assignment (%s)'%s' detected in FCS... now processing\n",
                     (fcsCmd->GetTypeName()).c_str(),
                     fcsCmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
                  #endif
                  
                  //Let's handle GmatFunction first
                  //compiler warning: '+=' : unsafe use of type 'bool' in operation
                  //OK += HandleGmatFunction(fcsCmd, &combinedObjectMap);
                  OK = HandleGmatFunction(fcsCmd, &combinedObjectMap) && OK;
                  // do not set the non-global object map here; it will need to be
                  // setup by the FunctionManager at execution
                  fcsCmd->SetGlobalObjectMap(&globalObjectMap);
               }
               if (fcsCmd->IsOfType("BranchCommand"))
               {
                  std::vector<GmatCommand*> cmdList =
                     ((BranchCommand*) fcsCmd)->GetCommandsWithGmatFunctions();
                  Integer sz = (Integer) cmdList.size();
                  for (Integer jj = 0; jj < sz; jj++)
                  {
                     GmatCommand *childCmd = cmdList.at(jj);
                     HandleGmatFunction(childCmd, &combinedObjectMap);
                  }
               }
               fcsCmd = fcsCmd->GetNext();
            }
         }
      }
   }
   #ifdef DEBUG_SANDBOX_GMATFUNCTION
   MessageInterface::ShowMessage("Sandbox::HandleGmatFunction() returning %d\n", OK);
   #endif
   return OK;
}


//------------------------------------------------------------------------------
// void SetGlobalRefObject(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Sets globally used object pointers to command
 *
 * @param cmd The command to set global object pointers to
 */
//------------------------------------------------------------------------------
void Sandbox::SetGlobalRefObject(GmatCommand *cmd)
{
   #ifdef DEBUG_SANDBOX_GLOBAL_REF_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::SetGlobalRefObject() Setting solarSystem <%p>, transientForces <%p>\n   "
       "internalCoordSystem <%p>, publisher <%p>, to <%p>'%s'\n", solarSys,
       &transientForces, internalCoordSys, publisher, cmd, cmd->GetTypeName().c_str());
   #endif
   cmd->SetSolarSystem(solarSys);
   cmd->SetTransientForces(&transientForces);
   cmd->SetInternalCoordSystem(internalCoordSys);
   cmd->SetPublisher(publisher);
}


//------------------------------------------------------------------------------
// void ShowObjectMap(ObjectMap &om, const std::string &title)
//------------------------------------------------------------------------------
void Sandbox::ShowObjectMap(ObjectMap &om, const std::string &title)
{   
   MessageInterface::ShowMessage(title);
   MessageInterface::ShowMessage
      ("object map = <%p> and contains %d objects\n", &om, om.size());
   if (om.size() > 0)
   {
      std::string objName;
      GmatBase *obj = NULL;
      std::string isGlobal;
      std::string isLocal;
      GmatBase *paramOwner = NULL;
      bool isParameter = false;
      std::string paramOwnerType;
      std::string paramOwnerName;
      
      for (ObjectMap::iterator i = om.begin(); i != om.end(); ++i)
      {
         objName = i->first;
         obj = i->second;
         isGlobal = "No";
         isLocal = "No";
         paramOwner = NULL;
         isParameter = false;
         paramOwnerType = "";
         paramOwnerName = "";
         
         if (obj)
         {
            if (obj->IsGlobal())
               isGlobal = "Yes";
            if (obj->IsLocal())
               isLocal = "Yes";
            if (obj->IsOfType(Gmat::PARAMETER))
            {
               isParameter = true;
               paramOwner = ((Parameter*)obj)->GetOwner();
               if (paramOwner)
               {
                  paramOwnerType = paramOwner->GetTypeName();
                  paramOwnerName = paramOwner->GetName();
               }
            }
         }
         MessageInterface::ShowMessage
            ("   %50s  <%p>  %-16s  IsGlobal:%-3s  IsLocal:%-3s", objName.c_str(), obj,
             obj == NULL ? "NULL" : (obj)->GetTypeName().c_str(), isGlobal.c_str(),
             isLocal.c_str());
         if (isParameter)
            MessageInterface::ShowMessage
               ("  ParameterOwner: <%p>[%s]'%s'\n", paramOwner, paramOwnerType.c_str(),
                paramOwnerName.c_str());
         else
            MessageInterface::ShowMessage("\n");
      }
   }
   else
   {
      MessageInterface::ShowMessage("The object map is empty\n");
   }
}


//------------------------------------------------------------------------------
// void UpdateClones(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Entry method for updates to owned clone objects
 *
 * @param obj The updates object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::UpdateClones(GmatBase *obj, Integer updatedParameterIndex)
{
   switch (cloneUpdateStyle)
   {
      case PASS_TO_ALL:
         PassToAll(obj, updatedParameterIndex);
         break;

      case PASS_TO_REGISTERED:
         PassToRegisteredClones(obj, updatedParameterIndex);
         break;

      case SKIP_UPDATES:
         #ifdef REPORT_CLONE_UPDATE_STATUS
            MessageInterface::ShowMessage("Clone update requested for %s; "
                  "skipping because the update method is set to "
                  "SKIP_UPDATES.\n", obj->GetName().c_str());
         #endif
         break;

      default:
         throw SandboxException("The requested clone update method is not "
               "recognized");
         break;
   }
}


//------------------------------------------------------------------------------
// void PassToAll(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Brute force method for updating owned clones
 *
 * @param obj The updates object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToAll(GmatBase *obj, Integer updatedParameterIndex)
{
   #ifdef REPORT_CLONE_UPDATE_STATUS
      MessageInterface::ShowMessage("Clone update requested for %s index %d; "
            "the update method is set to PASS_TO_ALL.\n, updatedParameterIndex",
            obj->GetName().c_str());
   #endif

   // Walk the global object store
   GmatBase *listObj;
   ObjectMap::iterator current = globalObjectMap.begin();
   
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the global "
            "object store\n");
   #endif

   while (current != globalObjectMap.end())
   {
      listObj = current->second;

      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   <%p> %s %s\n", listObj,
               listObj->GetName().c_str(), (listObj->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif

      if (listObj->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, listObj, updatedParameterIndex);

      ++current;
   }

   // Walk the local object store
   current = objectMap.begin();

   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the local "
            "object store\n");
   #endif

   while (current != objectMap.end())
   {
      listObj = current->second;

      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   <%p> %s %s\n", listObj,
               listObj->GetName().c_str(), (listObj->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif

      if (listObj->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, listObj, updatedParameterIndex);

      ++current;
   }


   // Walk the command list
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the "
            "command list\n");
   #endif
   GmatCommand *currentCmd = sequence;
   while (currentCmd != NULL)
   {
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   %s %s\n",
               currentCmd->GetTypeName().c_str(),
               (currentCmd->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif
      if (currentCmd->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, currentCmd, updatedParameterIndex);

      if (currentCmd->IsOfType("BranchCommand"))
      {
         PassToBranchCommand(obj, (BranchCommand*)currentCmd,
               updatedParameterIndex);
         currentCmd = ((BranchCommand*)currentCmd)->GetNextWhileExecuting();
      }
      else
         currentCmd = currentCmd->GetNext();
   }
}


//------------------------------------------------------------------------------
// void PassToRegisteredClones(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Owned clone update method that only updates registered clones
 *
 * @param obj The updated object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToRegisteredClones(GmatBase *obj,
      Integer updatedParameterIndex)
{
   #ifdef REPORT_CLONE_UPDATE_STATUS
      MessageInterface::ShowMessage("Clone update requested for %s; "
            "the update method is set to PASS_TO_REGISTERED.\n",
            obj->GetName().c_str());
   #endif

}


//------------------------------------------------------------------------------
// void PassToBranchCommand(GmatBase *theClone, GmatCommand* theBranchCommand,
//       Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Passes potential owned clones into branch command branches.
 *
 * This method calls into branches of branches recursively.
 *
 * @param theClone The object that may be an owned clone
 * @param theBranchCommand The branch command that that manages the branch
 *                         control sequence
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToBranchCommand(GmatBase *theClone,
      BranchCommand* theBranchCommand, Integer updatedParameterIndex)
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("Clone management for a %s branch "
            "command\n", theBranchCommand->GetTypeName().c_str());
   #endif

   // Loop through the branch command, updating each branch member
   Integer branch = 0;
   GmatCommand *cmd = NULL;
   do
   {
      cmd = theBranchCommand->GetChildCommand(branch);
      while ((cmd != NULL) && (cmd != theBranchCommand))
      {
         #ifdef DEBUG_CLONE_UPDATES
            MessageInterface::ShowMessage("   Branch[%d] member: a %s "
                  "command\n", branch, cmd->GetTypeName().c_str());
         #endif
         if (cmd->HasLocalClones())
            UpdateAndInitializeCloneOwner(theClone, cmd, updatedParameterIndex);

         if ((cmd->IsOfType("BranchCommand")) && (cmd != theBranchCommand))
         {
            #ifdef DEBUG_CLONE_UPDATES
               MessageInterface::ShowMessage("Nesting PassToBranchCommands; "
                     "passing %p from %p\n", cmd, this);
            #endif
            PassToBranchCommand(theClone, (BranchCommand*)cmd,
                  updatedParameterIndex);
         }

         if (cmd->IsOfType("BranchCommand"))
            cmd = ((BranchCommand*)cmd)->GetNextWhileExecuting();
         else
            cmd = cmd->GetNext();
      }
      ++branch;
   } while (cmd != NULL);
}


//------------------------------------------------------------------------------
// void UpdateAndInitializeCloneOwner(GmatBase *theClone, GmatBase* theOwner)
//------------------------------------------------------------------------------
/**
 * Gives an owned clone owner the opportunity to update its clone
 *
 * @param theClone The object that may affect an owned clone
 * @param theOwner The object that might own the clone
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::UpdateAndInitializeCloneOwner(GmatBase *theClone,
         GmatBase* theOwner, Integer updatedParameterIndex)
{
   bool incomingInitState = theOwner->IsInitialized();

   if (updatedParameterIndex >= 0)
   {
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("In Sandbox::UpdateAndInitializeClone"
               "Owner(%s, %s, %d), %s may not need reinitialization\n",
               theClone->GetName().c_str(), theOwner->GetName().c_str(),
               updatedParameterIndex, theOwner->GetName().c_str());
      #endif

      theOwner->UpdateClonedObjectParameter(theClone, updatedParameterIndex);
   }
   else
   {
      theOwner->UpdateClonedObject(theClone);

      if ((theOwner->IsInitialized() == false) && incomingInitState)
      {
         #ifdef DEBUG_CLONE_UPDATES
            MessageInterface::ShowMessage("In Sandbox::UpdateAndInitializeClone"
                  "Owner(%s, %s, %d), %s must be reinitialized\n",
                  theClone->GetName().c_str(), theOwner->GetName().c_str(),
                  updatedParameterIndex, theOwner->GetName().c_str());
         #endif

         ObjectInitializer *objInit = new ObjectInitializer(solarSys, &objectMap, &globalObjectMap,
                  internalCoordSys, true);
         objInit->BuildReferences(theOwner);
         objInit->BuildAssociations(theOwner);
         delete objInit;

         #ifdef DEBUG_CLONE_UPDATES
            bool retval =
         #endif

         theOwner->Initialize();

         #ifdef DEBUG_CLONE_UPDATES
            if (retval == false)
               MessageInterface::ShowMessage("%s needs initialization, but "
                     "initialization fails\n", theOwner->GetName().c_str());
         #endif
      }
   }
}


//----------------------------------------------------------------------------
// bool ReportError(BaseException &be)
//----------------------------------------------------------------------------
/**
 * Method used to suppress multiple warnings
 *
 * @note The current implementation only traps multiple Python iinitialization
 * failures.
 *
 * @param be The exception checked for suppression
 *
 * @retval true to report the exception, false to suppress it
 */
 //----------------------------------------------------------------------------
bool Sandbox::ReportError(BaseException &be)
{
   bool retval = true;

   if (be.GetDetails().find("The Pythoninterface cannot be accessed") != std::string::npos)
      ++warnPyInterface;

   if (warnPyInterface > 1)
      retval = false;

   return retval;
}

