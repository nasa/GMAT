//$Id$
//------------------------------------------------------------------------------
//                           APIFunctions
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 16, 2018
/**
 * Functions that provide the top level GMAT API interfaces.
 */
//------------------------------------------------------------------------------

#include "APIFunctions.hpp"
#include "HelpSystem.hpp"
#include "ObjectInitializer.hpp"
#include "APIException.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "APIMessageReceiver.hpp"
#include "Validator.hpp"
#include "FileUtil.hpp"
#include "ApiInterpreter.hpp"

//------------------------------------------------------------------------------
// std::string Help(std::string forItem)
//------------------------------------------------------------------------------
/**
 * Entry point for the GMAT help system
 *
 * @param forItem Help topic requested.  If forItem == "" then top level help is
 *        returned.
 *
 * @return A string containing the help text
 */
//------------------------------------------------------------------------------
std::string Help(std::string forItem)
{
   std::string theHelp;

   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   if (GetObject(forItem) != nullptr)
      theHelp = GetObject(forItem)->Help();   // @todo Subitem help
   else
   {
      HelpSystem *helpsys = HelpSystem::Instance();
      theHelp = helpsys->Help(forItem);
   }

   return theHelp;
}

//------------------------------------------------------------------------------
// void Setup(const std::string &theStartupFile)
//------------------------------------------------------------------------------
/**
 * Prepares the GMAT engine for use
 *
 * @param theStartupFile The startup files used for initialization.  If empty,
 *        "gmat_startup_file.txt" is used.
 */
//------------------------------------------------------------------------------
void Setup(const std::string &theStartupFile)
{
   Moderator *moderator = Moderator::Instance();
   if (!moderator->IsInitialized())
      moderator->Initialize(theStartupFile);
}


//------------------------------------------------------------------------------
// GmatBase* Construct(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Entry point to object creation in the GMAT engine.
 *
 * If name identifies an existing object of the specified type, that object is
 * returned.  On type mismatch, an exception is thrown.  If no object of the
 * requested name exists and teh type is valid, a new object is created and
 * passed to the configuration manager, and then returned.
 *
 * @param type The type of object requested
 * @param name The name to be assigned to the object.
 * @param extraData1 Additional data used for some objects.
 * @param extraData2 Additional data used for some objects.
 * @param extraData3 Additional data used for some objects.
 * @param extraData4 Additional data used for some objects.
 *
 * @return The requested object
 */
//------------------------------------------------------------------------------
GmatBase* Construct(const std::string &type, const std::string &name,
      const std::string &extraData1, const std::string &extraData2,
      const std::string &extraData3, const std::string &extraData4)
{
   GmatBase *theObject = nullptr;
   std::string theType = type;

   // Add a few aliases for users that don't know what GMAT calls things
   if (theType == "Propagator")
      theType = "PropSetup";

   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   FactoryManager *factman = FactoryManager::Instance();
   UnsignedInt typeID = factman->GetBaseTypeOf(theType);
   if (typeID != Gmat::UNKNOWN_OBJECT)
   {
      theObject = theModerator->CreateObject(typeID, theType, name);
     
      if (!theObject)
         throw APIException("The object " + name + " with type " + theType +
               " was not constructed.");

      if (!theObject->IsOfType(theType)  && !theObject->IsOfType(typeID))
         throw APIException("The object " + name + " has the type " +
               theObject->GetTypeName() + ", rather than the requested " +
               theType + " type.");

      // Special case: No default forces
      if (theObject->IsOfType("ODEModel"))
         theObject->TakeAction("ClearDefaultForce");

      // Special case: Don't allow ODE models to delete constructed forces
      if (theObject->IsOfType("PhysicalModel"))
         ((PhysicalModel*) theObject)->SetAllowODEDelete(false);

      // Place celestial bodies in the solar system
      if (theObject->IsOfType("CelestialBody"))
      {
         SolarSystem *ss = theModerator->GetSolarSystemInUse();
         if (ss->GetBody(name) == nullptr)
            ss->AddBody((CelestialBody*)theObject);
      }

      theObject->TakeAction("FromAPI");

      // Handle type specific settings
      if (extraData1 != "")
         ProcessParameters(theObject, extraData1, extraData2, extraData3, extraData4);
   }

   return theObject;
}


//------------------------------------------------------------------------------
// GmatBase* Copy(const GmatBase *theObject, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Copies an object and, if needed, adds that object to the configuration manager
 *
 * If name identifies an existing object, that object has the source object
 * copied to it and returned.  If no object of the requested name exists,
 * a new object is created and passed to the configuration manager,
 * and then returned.
 *
 * @param theObject The object to copy
 * @param name The name to be assigned to the object.
 *
 * @return The copied object
 */
 //------------------------------------------------------------------------------
GmatBase* Copy(const GmatBase *theObject, const std::string &name)
{
   GmatBase *newObject = nullptr;
   std::string type = theObject->GetTypeName();

   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   newObject = Construct(type, name);
   newObject->Copy(theObject);

   return newObject;
}


//------------------------------------------------------------------------------
// std::string ShowObjectsForID(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Shows the list of objects in the configuration
 *
 * @param type The type of object requested.  UNKNOWN_OBJECT returns the
 *        complete list.
 *
 * @return The list of objects
 */
//------------------------------------------------------------------------------
std::string ShowObjectsForID(UnsignedInt type)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   bool includeSolarSystem = false;

   StringArray objects = theModerator->GetListOfObjects(type);

   std::string olist = "Current GMAT Objects";
   if (type != Gmat::UNKNOWN_OBJECT)
      olist += " of type " + GmatType::GetTypeName(type);
   else
      includeSolarSystem = true;
   olist += "\n\n";

   // @todo: Reformat into columns so that long lists have a chance to stay on
   // screen
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      olist += "   " + objects[i] + "\n";
   }

   if (includeSolarSystem)
   {
      olist += "\nThe SolarSystem contains the following bodies:\n\n   [";

      SolarSystem *ss = theModerator->GetSolarSystemInUse();
      StringArray bodies = ss->GetBodiesInUse();
      for (UnsignedInt i = 0; i < bodies.size(); ++i)
      {
         if (i > 0)
            olist += ", ";
         olist += bodies[i];
      }
      olist += "]\n";
   }

   return olist;
}


//------------------------------------------------------------------------------
// std::string ShowObjects(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Shows the list of objects in the configuration
 *
 * @param type The type of object requested.
 *
 * @return The list of objects
 */
//------------------------------------------------------------------------------
std::string ShowObjects(const std::string &type)
{
   std::string theType = type;
   if ((type == "") || (type == "UnknownObject"))
      theType = "Unknown";
   return ShowObjectsForID(GmatType::GetTypeId(theType));
}


//------------------------------------------------------------------------------
// std::string ShowClassesForID(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns a list of object types that can be created
 *
 * @param type The ID of the core type.  If set to UNKNOWN_OBJECT, all
 *        createable types are listed.
 *
 * @return The types that can be created that match the core type
 */
//------------------------------------------------------------------------------
std::string ShowClassesForID(UnsignedInt type)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   StringArray items;
   if (type == Gmat::UNKNOWN_OBJECT)
      items = theModerator->GetListOfAllFactoryItems();
   else
      items = theModerator->GetListOfFactoryItems(type);

   std::string clist = "\n";

   // @todo: Reformat into columns so that long lists have a change to stay on
   // screen
   for (UnsignedInt i = 0; i < items.size(); ++i)
      clist += "   " + items[i] + "\n";

   return clist;
}

//------------------------------------------------------------------------------
// std::string ShowClasses(std::string type)
//------------------------------------------------------------------------------
/**
 * Returns a list of object types that can be created
 *
 * @param type The core type.  If set to "Unknown" or "UnknownObject", all
 *        createable types are listed.
 *
 * @return The types that can be created that match the core type
 */
//------------------------------------------------------------------------------
std::string ShowClasses(const std::string &type)
{
   UnsignedInt typeID;

   if ((type == "") || (type == "Unknown") || (type == "UnknownObject"))
      typeID = Gmat::UNKNOWN_OBJECT;
   else
   {
      typeID = GmatBase::GetObjectType(type);
      if (typeID == Gmat::UNKNOWN_OBJECT)
      {
         // Remove a trailing "s" and retry
         char ch = type.back();
         if (ch == 's')
         {
            std::string type2 = type.substr(0, type.length()-1);
            typeID = GmatBase::GetObjectType(type2);
         }

         // Still unknown?  Time to punt.
         if (typeID == Gmat::UNKNOWN_OBJECT)
         {
            std::string retval = "Class help is not available for the requested "
                  "type, " + type + "; please check spelling.\n\nAll available "
                  "classes can be listed by calling the ShowClasses command "
                  "without a class type.";
            return retval;
         }
      }
   }

   return ShowClassesForID(typeID);
}


//------------------------------------------------------------------------------
// GmatBase*   GetObject(const std::string &objectname)
//------------------------------------------------------------------------------
/**
 * Retrieves an object by name from the configuration
 *
 * @param objectname The name of the object
 *
 * @return The object (as a GmatBase pointer), or the NULL pointer if not found.
 */
//------------------------------------------------------------------------------
GmatBase* GetObject(const std::string &objectname)
{
   GmatBase *retval = nullptr;

   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();
   retval = ConfigManager::Instance()->GetItem(objectname);
   if (!retval)
   {
      // May be a solar system object
      SolarSystem *ss = theModerator->GetSolarSystemInUse();
      retval = ss->GetBody(objectname);
      if (!retval)
         retval = ss->GetSpecialPoint(objectname);
   }

   return retval;
}


//------------------------------------------------------------------------------
// SolarSystem*   GetSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves the current solar system object
 *
 * @return The solar system object
 */
//------------------------------------------------------------------------------
SolarSystem* GetSolarSystem()
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();
   return theModerator->GetSolarSystemInUse();
}

//------------------------------------------------------------------------------
// bool Exists(const std::string &objectName)
//------------------------------------------------------------------------------
/**
 * Tests to see if an object is in the GMAT configuration
 *
 * @return true if the object is in the config, false if not
 */
//------------------------------------------------------------------------------
bool Exists(const std::string &objectName)
{
   return GetObject(objectName) != nullptr;
}


//------------------------------------------------------------------------------
// std::vector<GmatCommand*> GetCommands(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Shows the list of objects in the configuration
 *
 * @param type The type of object requested.
 *
 * @return The list of objects
 */
 //------------------------------------------------------------------------------
std::vector<GmatCommand*> GetCommands(const std::string &type)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   std::vector<GmatCommand*> commands;

   GmatCommand* cmd = theModerator->GetFirstCommand();

   // Loop through linked list of commands
   while (cmd != NULL)
   {
      if (type == "" || cmd->IsOfType(type))
         commands.push_back(cmd);

      // Get next command
      if (cmd->IsOfType("BranchCommand"))
      {
         std::vector<GmatCommand*> nestedCommands;
         nestedCommands = GetNestedCommands(cmd, type);

         commands.insert(commands.end(), nestedCommands.begin(), nestedCommands.end());
      }

      cmd = cmd->GetNext();
   }

   return commands;
}


//------------------------------------------------------------------------------
// std::vector<GmatCommand*> GetNestedCommands(GmatCommand *cmd, const std::string &type)
//------------------------------------------------------------------------------
/**
 * Shows the list of objects in the configuration
 *
 * @param type The type of object requested.
 *
 * @return The list of objects
 */
 //------------------------------------------------------------------------------
std::vector<GmatCommand*> GetNestedCommands(GmatCommand *cmd, const std::string &type)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   std::vector<GmatCommand*> commands;

   GmatCommand* nestCmd = cmd->GetChildCommand();

   // Loop through linked list of commands
   while (nestCmd != NULL && nestCmd != cmd)
   {
      if (type == "" || nestCmd->IsOfType(type))
         commands.push_back(nestCmd);

      // Get next command
      if (nestCmd->IsOfType("BranchCommand"))
      {
         std::vector<GmatCommand*> nestedCommands;
         nestedCommands = GetNestedCommands(nestCmd, type);

         commands.insert(commands.end(), nestedCommands.begin(), nestedCommands.end());
      }

      nestCmd = nestCmd->GetNext();
   }

   return commands;
}


// -----------------------------------------------------------------------------
// Functions used for script driven work flows
// -----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool LoadScript(const std::string &filename)
//------------------------------------------------------------------------------
/**
 * Loads a GMAT script into the system
 *
 * @param filename The (relative or absolute) path to the script
 *
 * @return true if the script loaded, false if loading failed
 */
//------------------------------------------------------------------------------
bool LoadScript(const std::string &filename)
{
   Moderator *theModerator = Moderator::Instance();
   FileManager *theFileManager = FileManager::Instance(); 
   theFileManager->SetGmatWorkingDirectory(GmatFileUtil::GetCurrentWorkingDirectory());

   if (!theModerator->IsInitialized())
      Setup();
   return theModerator->InterpretScript(filename);
}

//------------------------------------------------------------------------------
// bool LoadInclude(const std::string &filename)
//------------------------------------------------------------------------------
/**
 * Loads a GMAT include script into the system. Existing objects are not cleared
 * when this function is called.
 *
 * @param filename The (relative or absolute) path to the script
 *
 * @return true if the include script loaded, false if loading failed
 */
 //------------------------------------------------------------------------------
bool LoadInclude(const std::string &filename)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   std::string includeLine;
   includeLine = "#Include '" + filename + "'";

   std::istringstream ss(includeLine);

   return theModerator->InterpretScript(&ss, false);
}

//------------------------------------------------------------------------------
// bool RunScript()
//------------------------------------------------------------------------------
/**
 * Runs a GMAT script that was loaded using LoadScript()
 *
 * @return true if the script ran successfully, false if running failed
 */
//------------------------------------------------------------------------------
bool RunScript()
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   bool retval = false;

   Integer flag = theModerator->RunMission();
   if (flag == 1)
      retval = true;
//   else

   return retval;
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &filename);
//------------------------------------------------------------------------------
/**
 * Writes out a script file containing everything in the engine.
 *
 * @param filename The name of the script to be written
 *
 * @return true if the script was written, false if writing failed
 */
//------------------------------------------------------------------------------
bool SaveScript(const std::string &filename)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   return theModerator->SaveScript(filename);
}

//------------------------------------------------------------------------------
// GmatBase*   GetRuntimeObject(const std::string &objectname)
//------------------------------------------------------------------------------
/**
 * Access method for objects in the GMAT Sandbox following a run.
 *
 * GetObject returns objects that are in the GMAT configuration.  Those objects
 * exist in their "pre-execution" state -- that is, if part of a script that is
 * to be run, the objects accessed using GetObject contain the data at the start
 * of the run.  This method accesses GMAT objects from the GMAT Sandbox.  If a
 * script has been executed, the data in these objects is the data at the end of
 * the run.
 *
 * @param objectname The name of the object
 *
 * @return The object requested from the Sandbox
 */
//------------------------------------------------------------------------------
GmatBase*   GetRuntimeObject(const std::string &objectname)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   return theModerator->GetInternalObject(objectname);
}


//------------------------------------------------------------------------------
// std::string  GetRunSummary()
//------------------------------------------------------------------------------
/**
 * Returns the summary results found when running a script
 *
 * @return A C-style string, packed into an std::string, with run results.
 */
//------------------------------------------------------------------------------
std::string  GetRunSummary()
{
   std::string theMsg;

   Moderator *theModerator = Moderator::Instance();
   if (theModerator == NULL)
   {
      theMsg = "Cannot find the Moderator";
      return "Error accessing the Moderator";
   }

   GmatCommand *current = theModerator->GetFirstCommand(1);
   theMsg = "";
   while (current != NULL)
   {
      if (current->GetTypeName() != "NoOp")
      {
         theMsg += current->GetStringParameter("Summary");
         theMsg += "\n-----------------------------------\n";
      }
      current = current->GetNext();
   }

   return theMsg;

}

//------------------------------------------------------------------------------
// void Initialize(std::string forObject)
//------------------------------------------------------------------------------
/**
 * Performs object interconnection and initialization
 *
 * @param forObject Optional name of a specific object to initialize.  If empty,
 *                  all configured objects are set up.
 */
//------------------------------------------------------------------------------
void Initialize(std::string forObject)
{
   StringArray theObjects;

   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   if (forObject != "")
      theObjects.push_back(forObject);
   else
   {
      theObjects = ConfigManager::Instance()->GetListOfAllItems();
   }

   SolarSystem *solarSys = theModerator->GetSolarSystemInUse();
   CoordinateSystem *internalCoordSys =
         theModerator->GetInternalCoordinateSystem();
   ObjectMap theObjectMap;

   if (solarSys && internalCoordSys)
   {
      for (UnsignedInt i = 0; i < theObjects.size(); ++i)
      {
         GmatBase *obj = GetObject(theObjects[i]);
         if (obj)
         {
            theObjectMap[theObjects[i]] = obj;
            obj->SetSolarSystem(solarSys);
            obj->SetInternalCoordSystem(internalCoordSys);
            // Tell the object that it was built from the API, so init can avoid
            // actions that are API unfriendly
            obj->TakeAction("FromAPI");
         }
      }

      ObjectInitializer *objInit = new ObjectInitializer(solarSys, &theObjectMap,
            nullptr, internalCoordSys);
      objInit->InitializeObjects();

      // March through the objects and report the ones that are not initialized
      std::stringstream uninitialized;
      std::stringstream warnUninitialized;
      std::stringstream notFound;

      for (Integer i = 0; i < theObjects.size(); ++i)
      {
         GmatBase *obj = GetObject(theObjects[i]);

         if (obj)
         {
            // Some GMAT objects don't get initialized by the object initializer.
            // Initialize them if possible.
            if (obj->IsInitialized() == false)
               // We don't care if some object types initialized
               if (
                    // Skip physical models except ODEModel container
                    ( !(
						(obj->IsOfType(Gmat::SOLVER)) || 
						((obj->IsOfType(Gmat::PHYSICAL_MODEL)) && (!obj->IsOfType(Gmat::ODE_MODEL))) ||
						(obj->IsOfType(Gmat::BURN))
						) 
					)
                  )
               {
                  if (obj->Initialize() == false)
                  {
                     if (obj->IsOfType(Gmat::ODE_MODEL))
                        warnUninitialized << "   " << theObjects[i] << "\n";
                     else
                        uninitialized << "   " << theObjects[i] << "\n";
                  }
               }
         }
         else
            notFound << "   " << theObjects[i] << "\n";
      }

      if ((uninitialized.str() != "") || (notFound.str() != ""))
      {
         std::string message;
         if (uninitialized.str() != "")
            message = "The following objects were not initialized:\n" +
                  uninitialized.str();
         if (notFound.str() != "")
            message = "The following objects were not found:\n" +
                  notFound.str();
         throw APIException(message);
      }
   }
}


//------------------------------------------------------------------------------
// GmatBase* Update(const std::string &name,
//                  const std::string &extraData1, const std::string &extraData2,
//                  const std::string &extraData3, const std::string &extraData4)
//------------------------------------------------------------------------------
/**
 * Update an existing object in the GMAT engine.
 *
 * If name identifies an existing object of the specified type, that object is
 * returned.  On type mismatch, an exception is thrown.  If no object of the
 * requested name exists and teh type is valid, a new object is created and
 * passed to the configuration manager, and then returned.
 *
 * @param name The name of the object to update.
 * @param extraData1 Additional data used for some objects.
 * @param extraData2 Additional data used for some objects.
 * @param extraData3 Additional data used for some objects.
 * @param extraData4 Additional data used for some objects.
 *
 * @return The requested object
 */
 //------------------------------------------------------------------------------
void Update(const std::string &name,
            const std::string &extraData1, const std::string &extraData2,
            const std::string &extraData3, const std::string &extraData4)
{
   GmatBase *obj = GetObject(name);
   Update(obj, extraData1, extraData2, extraData3, extraData4);
}


//------------------------------------------------------------------------------
// GmatBase* Update(const GmatBase *theObject,
//                  const std::string &extraData1, const std::string &extraData2,
//                  const std::string &extraData3, const std::string &extraData4)
//------------------------------------------------------------------------------
/**
 * Update an existing object in the GMAT engine.
 *
 * If name identifies an existing object of the specified type, that object is
 * returned.  On type mismatch, an exception is thrown.  If no object of the
 * requested name exists and teh type is valid, a new object is created and
 * passed to the configuration manager, and then returned.
 *
 * @param theObject The GMAT object to update.
 * @param extraData1 Additional data used for some objects.
 * @param extraData2 Additional data used for some objects.
 * @param extraData3 Additional data used for some objects.
 * @param extraData4 Additional data used for some objects.
 *
 * @return The requested object
 */
 //------------------------------------------------------------------------------
void Update(GmatBase *theObject,
            const std::string &extraData1, const std::string &extraData2,
            const std::string &extraData3, const std::string &extraData4)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   // Handle type specific settings
   if (extraData1 != "")
      ProcessUpdateParameters(theObject, extraData1, extraData2, extraData3, extraData4);
}


//------------------------------------------------------------------------------
// void Clear(std::string forObject)
//------------------------------------------------------------------------------
/**
 * Remove managed GMAT object(s) from the configuration.
 *
 * When called without a parameter, all managed objects are deleted.  Call with
 * an object name to delete just that object.
 *
 * @param forObject Optional name of a specific object to delete
 *
 * @return A confirmation string
 */
//------------------------------------------------------------------------------
std::string Clear(std::string forObject)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   std::string retval = "No objects were removed from the configuration.";

   if (forObject == "")
   {
      theModerator->ClearResource();
      theModerator->LoadMinimumResource();
      retval = "All configured objects have been removed from GMAT.";
   }
   else
   {
      GmatBase *obj = GetObject(forObject);
      if (obj)
      {
         UnsignedInt type = obj->GetType();
         if (theModerator->RemoveObject(type, forObject, true))
            retval = "The object " + forObject + " has been removed from GMAT.";
         else
            retval = "The object " + forObject + " could not be removed.  "
                  "Is another object using it?";
      }
      else
         retval += "  The object " + forObject + " was not found.";
   }

   return retval;
}


//------------------------------------------------------------------------------
// void UseLogFile(std::string logFile)
//------------------------------------------------------------------------------
/**
 * Turns on the GMAT log file
 *
 * @param logFile The name of the log file.  If there is no path information,
 *                the file is written to the output folder specified in the
 *                startup file.
 */
//------------------------------------------------------------------------------
void UseLogFile(std::string logFile)
{
   Moderator *theModerator = Moderator::Instance();
   if (!theModerator->IsInitialized())
      Setup();

   APIMessageReceiver *theReceiver = APIMessageReceiver::Instance();
   FileManager *theFileManager = FileManager::Instance();

   MessageInterface::SetMessageReceiver(theReceiver);

   // Setup the name and path
   std::string path = theFileManager->GetAbsPathname("LOG_FILE");

   if (path.find("/") != path.npos)
      path = path.substr(0, path.find_last_of("/") + 1);
   if (path.find("\\") != path.npos)
      path = path.substr(0, path.find_last_of("\\") + 1);

   if (logFile == "")
      logFile = "GmatAPILog.txt";

   if ((logFile.find("/") != logFile.npos) ||
         (logFile.find("\\") != logFile.npos))
      path = logFile;
   else
      path += logFile;

   MessageInterface::SetLogFile(path);
   MessageInterface::SetLogEnable(true);
   MessageInterface::ShowMessage("Logging to %s\n", path.c_str());
}


//------------------------------------------------------------------------------
// void        EchoLogFile(bool echo)
//------------------------------------------------------------------------------
/**
 * Turn on echoing of the log to the user's terminal
 *
 * @param echo The new mode setting
 */
//------------------------------------------------------------------------------
void        EchoLogFile(bool echo)
{
	APIMessageReceiver *theReceiver = APIMessageReceiver::Instance();
	MessageInterface::SetMessageReceiver(theReceiver);
	MessageInterface::SetLogEnable(true);
	theReceiver->SetEchoMode(echo);
}


//------------------------------------------------------------------------------
// GmatCommand* Command(const std::string &type, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Creates a command
 *
 * @param type The type of command
 * @param desc The description defining the command
 *
 * @return The created command
 */
 //------------------------------------------------------------------------------
GmatCommand* Command(const std::string &type, const std::string &desc)
{
	GmatCommand *obj = NULL;
	GmatCommand *inCmd = NULL;

	ApiInterpreter *theApiInterpreter = ApiInterpreter::Instance();	

	obj = theApiInterpreter->ParseCommandBlock(type, desc, inCmd);

	return obj;
}

//------------------------------------------------------------------------------
// Integer Execute(Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Calls Moderator RunMission
 *
 * @param  sandboxNum  The sandbox number (1 to Gmat::MAX_SANDBOX)
 *
 * @return  1 if run was successful
 *         -1 if sandbox number is invalid
 *         -2 if exception thrown during sandbox initialization
 *         -3 if unknown error occurred during sandbox initialization
 *         -4 if execution interrupted by user
 *         -5 if exception thrown during the sandbox execution
 *         -6 if unknown error occurred during sandbox execution
 *		   -7 if unkown error at the API level
 */
 //------------------------------------------------------------------------------
Integer Execute(Integer sandboxNum)
{
	Integer status = -7;
	
	Moderator *theModerator = Moderator::Instance();
	if (!theModerator->IsInitialized())
		Setup();

	status = theModerator->RunMission(sandboxNum);

	return status;

}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Removes a command from the command sequence. The caller has to delete the command.
 *
 * If deleting branch command,
 * it will remove and delete all children from the branch. If deleting ScriptEvent,
 * it will remove and delete all commands including EndScrpt between BeginScrint
 * and EndScript.
 */
 //------------------------------------------------------------------------------
GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
{
	GmatCommand *remvCmd = NULL;

	Moderator *theModerator = Moderator::Instance();
	if (!theModerator->IsInitialized())
	Setup();

	remvCmd = theModerator->DeleteCommand(cmd, sandboxNum);
	return remvCmd;
}

//------------------------------------------------------------------------------
// ******                                                                 ******
// ******   Functions used by the API but not intended for external use   ******
// ******                                                                 ******
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void ProcessParameters(GmatBase *theObject, const std::string &extraData1,
//       const std::string &extraData2, const std::string &extraData3,
//       const std::string &extraData4)
//------------------------------------------------------------------------------
/**
 * Handler for special case settings used in some object creation
 *
 * @param extraData1 Additional data used for some objects.
 * @param extraData2 Additional data used for some objects.
 * @param extraData3 Additional data used for some objects.
 * @param extraData4 Additional data used for some objects.
 */
//------------------------------------------------------------------------------
void ProcessParameters(GmatBase *theObject, const std::string &extraData1,
      const std::string &extraData2, const std::string &extraData3,
      const std::string &extraData4)
{
   UnsignedInt type = theObject->GetType();
   Moderator *theModerator = Moderator::Instance();

   switch (type)
   {
   case Gmat::COORDINATE_SYSTEM:
      {
         // Order of settings for a CS: Origin, Axis system, ??, ??
         theObject->SetStringParameter("Origin", extraData1);
         std::string axissys = extraData2;
         if (axissys == "")
            axissys = "MJ2000Eq";
         AxisSystem *axes = theModerator->CreateAxisSystem(axissys, axissys);
         ((CoordinateSystem*)theObject)->SetRefObject(axes, Gmat::AXIS_SYSTEM,
               axes->GetName());
      }
      break;

   default:
      break;
   }
}

//------------------------------------------------------------------------------
// void ProcessUpdateParameters(GmatBase *theObject, const std::string &extraData1,
//       const std::string &extraData2, const std::string &extraData3,
//       const std::string &extraData4)
//------------------------------------------------------------------------------
/**
 * Handler for special case settings used in some object updating
 *
 * @param extraData1 Additional data used for some objects.
 * @param extraData2 Additional data used for some objects.
 * @param extraData3 Additional data used for some objects.
 * @param extraData4 Additional data used for some objects.
 */
//------------------------------------------------------------------------------
void ProcessUpdateParameters(GmatBase *theObject, const std::string &extraData1,
      const std::string &extraData2, const std::string &extraData3,
      const std::string &extraData4)
{
   Validator *theValidator = Validator::Instance();

   if (theObject->IsOfType(Gmat::STOP_CONDITION))
   {
      StopCondition *sc = (StopCondition*) theObject;

      sc->ClearWrappers();

      sc->SetStringParameter("StopVar", extraData1);

      bool paramCreated;
      theValidator->CreateSystemParameter(paramCreated, extraData1);
      ElementWrapper *lhsWrapper = theValidator->CreateElementWrapper(extraData1, true);
      if (!lhsWrapper)
         throw APIException("Unable to set \"" + extraData1 + "\" to \"StopVar\" when updating StopCondition");
      sc->SetLhsWrapper(lhsWrapper);

      if (extraData2 != "")
      {
         sc->SetStringParameter("Goal", extraData2);
         theValidator->CreateSystemParameter(paramCreated, extraData2);
         ElementWrapper *rhsWrapper = theValidator->CreateElementWrapper(extraData2, true);
         if (!rhsWrapper)
            throw APIException("Unable to set \"" + extraData2 + "\" to \"Goal\" when updating StopCondition");
         sc->SetRhsWrapper(rhsWrapper);
      }
   }
}
