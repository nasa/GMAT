//$Id$
//------------------------------------------------------------------------------
//                              GuiInterpreter
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/25
//
/**
 * Implements the operations between GUI subsystem and the Moderator.
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "ResourceTree.hpp"
#include "MissionTree.hpp"
#include "OutputTree.hpp"
#endif

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Moderator.hpp"

//#define DEBUG_SYNC_STATUS
//#define DEBUG_GUI_PLUGIN_CREATE

GuiInterpreter* GuiInterpreter::instance = NULL;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
GuiInterpreter* GuiInterpreter::Instance()
{
   if (instance == NULL)
      instance = new GuiInterpreter();
   return instance;
}


//------------------------------------------------------------------------------
// ~GuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter::~GuiInterpreter()
{
}


//------------------------------------------------------------------------------
// bool Interpret(GmatCommand *inCmd, std::istringstream *ss)
//------------------------------------------------------------------------------
bool GuiInterpreter::Interpret(GmatCommand *inCmd, std::istringstream *ss)
{   
   SetInStream(ss);
   inScriptEvent = true;
   // We don't want parse first comment as header, so set skipHeader to true.
   bool retval = ScriptInterpreter::Interpret(inCmd, true);
   inScriptEvent = false;
   return retval;
}


//------------------------------------------------------------------------------
// std::vector<Integer> GetErrorLines()
//------------------------------------------------------------------------------
/**
 * Accessor for the error line numbers found during interpreting
 *
 * If the interpreting was performed in this interpreter, the local data member
 * is returned.  If a different (script) interpreter was used, it is accessed
 * using the Moderator.
 *
 * @return A vector of line numbers
 */
//------------------------------------------------------------------------------
std::vector<Integer> GuiInterpreter::GetErrorLines()
{
   Interpreter *interp = theModerator->GetScriptInterpreter();
   if ((interp != this) && (interp != NULL))
      return interp->GetErrorLines();
   return errorLines;
}

//------------------------------------------------------------------------------
// std::vector<Integer> GetWarningLines()
//------------------------------------------------------------------------------
/**
 * Accessor for the warning line numbers found during interpreting
 *
 * If the interpreting was performed in this interpreter, the local data member
 * is returned.  If a different (script) interpreter was used, it is accessed
 * using the Moderator.
 *
 * @return A vector of line numbers
 */
//------------------------------------------------------------------------------
std::vector<Integer> GuiInterpreter::GetWarningLines()
{
   Interpreter *interp = theModerator->GetScriptInterpreter();
   if ((interp != this) && (interp != NULL))
      return interp->GetWarningLines();
   return warningLines;
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
void GuiInterpreter::Finalize()
{
   theModerator->Finalize();
}


//------------------------------------------------------------------------------
// GmatBase* GetRunningObject(const char *name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::GetRunningObject(const char *name)
{
   return theModerator->GetInternalObject(std::string(name));
}


//------------------------------------------------------------------------------
// GmatBase* GetRunningObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::GetRunningObject(const std::string &name)
{
   return theModerator->GetInternalObject(name);
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItems()
//------------------------------------------------------------------------------
/**
 * Return a list of all items that can be created.
 *
 * @return list of all creatable items.
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetListOfAllFactoryItems()
{
   return theModerator->GetListOfAllFactoryItems();
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfFactoryItems(UnsignedInt type, const char *qualifier)
//------------------------------------------------------------------------------
/**
 * Returns names of all creatable items of object type.
 *
 * @param <type> object type
 *
 * @return array of item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetListOfFactoryItems(UnsignedInt type,
                                                         const char *qualifier)
{
   return theModerator->GetListOfFactoryItems(type, std::string(qualifier));
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfFactoryItems(UnsignedInt type,
//                                          const std::string &qualifier = "")
//------------------------------------------------------------------------------
/**
 * Returns names of all creatable items of object type.
 *
 * @param <type> object type
 *
 * @return array of item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetListOfFactoryItems(UnsignedInt type,
                                                         const std::string &qualifier)
{
   return theModerator->GetListOfFactoryItems(type, qualifier);
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
//------------------------------------------------------------------------------
/**
 * Return a std::string of all items that can be created except input object types
 *
 * @param <type> object types to be excluded
 *
 * @return list of all creatable items.
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
{
   return theModerator->GetListOfAllFactoryItemsExcept(types);
}


//------------------------------------------------------------------------------
// std::string GetStringOfAllFactoryItem(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Return a std::string of all items that can be created of input object types
 *
 * @param <type> requested object types
 *
 * @return list of all creatable items in string.
 */
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetStringOfAllFactoryItems(UnsignedInt type)
{
   StringArray creatables = theModerator->GetListOfFactoryItems(type);
   std::string str;
   
   for (UnsignedInt i = 0; i < creatables.size(); i++)
      str = str + creatables[i] + " ";
   
   return str;
}


//------------------------------------------------------------------------------
// std::string GetStringOfAllFactoryItemsExcept(const ObjectTypeArray &types)
//------------------------------------------------------------------------------
/**
 * Return a std::string of all items that can be created except input object types
 *
 * @param <type> object types to be excluded
 *
 * @return list of all creatable items in string.
 */
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetStringOfAllFactoryItemsExcept(const ObjectTypeArray &types)
{
   StringArray creatables = theModerator->GetListOfAllFactoryItemsExcept(types);
   std::string str;
   
   for (UnsignedInt i = 0; i < creatables.size(); i++)
      str = str + creatables[i] + " ";
   
   return str;
}


//------------------------------------------------------------------------------
// std::string GetNewName(const std::string &name, Integer startCount)
//------------------------------------------------------------------------------
/*
 * It gives new name by adding counter to the input name.
 *
 * @param <name> Base name to used to generate new name
 * @param <startCount> Starting counter
 * @return new name
 */
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetNewName(const std::string &name, Integer startCount)
{
   return theModerator->GetNewName(name, startCount);
}


//------------------------------------------------------------------------------
// GmatBase* AddClone(const std::string &name, std::string &cloneName)
//------------------------------------------------------------------------------
/*
 * Adds the clone of the named object to configuration.
 * It gives new name by adding counter to the name to be cloned.
 *
 * @parameter  name  The name of the object to be cloned
 * @parameter  cloneName  Name of the cloned object if object was cloned and
 *                added to configuration, blank otherwise
 *
 * @return  Cloned object pointer, NULL if it was not cloned
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::AddClone(const std::string &name, std::string &cloneName)
{
   return theModerator->AddClone(name, cloneName);
}


//------------------------------------------------------------------------------
// bool RenameObject(UnsignedInt type, const char *oldName
//                   const char *newName)
//------------------------------------------------------------------------------
/**
 * Renames item from the configured list.
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RenameObject(UnsignedInt type, const char *oldName,
                                  const char *newName)
{
   return theModerator->RenameObject(type, std::string(oldName), std::string(newName));
}


//------------------------------------------------------------------------------
// bool RenameObject(UnsignedInt type, const std::string &oldName
//                   const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames item from the configured list.
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RenameObject(UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return theModerator->RenameObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool RemoveObject(UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveObject(UnsignedInt type,
                                  const std::string &name)
{
   return theModerator->RemoveObject(type, name, false);
}

//------------------------------------------------------------------------------
// bool RemoveItemIfNotUsed(UnsignedInt type, const char *name)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list if it is not used in the mission
 * sequence.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveObjectIfNotUsed(UnsignedInt type, const char *name)
{
   return theModerator->RemoveObject(type, std::string(name), true);
}

//------------------------------------------------------------------------------
// bool RemoveItemIfNotUsed(UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list if it is not used in the mission
 * sequence.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::RemoveObjectIfNotUsed(UnsignedInt type,
                                          const std::string &name)
{
   return theModerator->RemoveObject(type, name, true);
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool GuiInterpreter::HasConfigurationChanged(Integer sandboxNum)
{
   return theModerator->HasConfigurationChanged(sandboxNum);
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(GmatBase *obj, bool tf)
//------------------------------------------------------------------------------
void GuiInterpreter::ConfigurationChanged(GmatBase *obj, bool tf)
{
   #if !defined __CONSOLE_APP__
   
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   theModerator->ConfigurationChanged(obj, tf);
   
   if (tf == true)
   {
      #ifdef DEBUG_SYNC_STATUS
      MessageInterface::ShowMessage
         ("GuiInterpreter::ConfigurationChanged() Setting GUI dirty\n");
      #endif
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   }
   
   #else
   
   theModerator->ConfigurationChanged(obj, tf);
   
   #endif
}


//------------------------------------------------------------------------------
// void ResetConfigurationChanged(bool resetResource = true,
//                                bool resetCommands = true,
//                                Integer sandboxNum = 1)
//------------------------------------------------------------------------------
void GuiInterpreter::ResetConfigurationChanged(bool resetResource,
                                               bool resetCommands,
                                               Integer sandboxNum)
{
   theModerator->ResetConfigurationChanged(resetResource, resetCommands, sandboxNum);
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const char *type, const std::string &name,
//                        Integer manage, bool createDefault)
//------------------------------------------------------------------------------
/**
 * @see CreateObject(const std::string &type, ...)
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateObject(const char *type,
                                       const std::string &name,
                                       Integer manage, bool createDefault)
{
   return CreateObject(std::string(type), name, manage, createDefault);
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const char *type, const char *name,
//                        Integer manage, bool createDefault)
//------------------------------------------------------------------------------
/**
 * @see CreateObject(const std::string &type, ...)
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateObject(const char *type,
                                       const char *name,
                                       Integer manage, bool createDefault)
{
   return CreateObject(std::string(type), name, manage, createDefault);
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &type, const std::string &name,
//                        Integer manage, bool createDefault)
//------------------------------------------------------------------------------
/**
 * Creates an object by calling Interpreter::CreateObject()
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateObject(const std::string &type,
                                       const std::string &name,
                                       Integer manage, bool createDefault,
                                       bool includeLineOnError, bool showWarning)
{
   #if !defined __CONSOLE_APP__
   
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   continueOnError = false;
   GmatBase *obj = Interpreter::CreateObject(type, name, manage, createDefault,
         includeLineOnError, showWarning);
   
   if (obj == NULL)
   {
      #ifdef DEBUG_SYNC_STATUS
      MessageInterface::ShowMessage
         ("GuiInterpreter::CreateObject() type='%s', name='%s', Setting GUI error\n",
          type.c_str(), name.c_str());
      #endif
      mainFrame->UpdateGuiScriptSyncStatus(3, 0); // Set GUI error
   }
   else if (name != "")
   {
      #ifdef DEBUG_SYNC_STATUS
      MessageInterface::ShowMessage
         ("GuiInterpreter::CreateObject() type='%s', name='%s', Setting GUI dirty\n",
          type.c_str(), name.c_str());
      #endif
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   }
   
   return obj;
   
   #else
   
   return Interpreter::CreateObject(type, name, manage, createDefault);
   
   #endif
}


//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves a default solar system object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* GuiInterpreter::GetDefaultSolarSystem()
{
   return theModerator->GetDefaultSolarSystem();
}

//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Retrieves the solar system is use object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* GuiInterpreter::GetSolarSystemInUse()
{
   return theModerator->GetSolarSystemInUse();
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const char *name)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a parameter object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::GetParameter(const char *name)
{
   return theModerator->GetParameter(std::string(name));
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a parameter object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::GetParameter(const std::string &name)
{
   return theModerator->GetParameter(name);
}


//------------------------------------------------------------------------------
// bool Moderator::IsParameter(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks to see if a given type is a Parameter. If str has '.', it parses
 * string to get type before checking.
 *
 * @param <str> object type string
 *
 * @return true if the type is a registered parameter, false if not.
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::IsParameter(const std::string &str)
{
   return theModerator->IsParameter(str);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName
//                            bool manage = true)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter. We need this to create an array
 * first and set size later when we create an array from the GUI.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * @param  manage     true if created object to be added to configuration (true)
 * 
 * @return Pointer to the constructed Parameter.
 *
 * @Note We need this to create an array first and set size later when an array
 *       is created the GUI.
 */
//------------------------------------------------------------------------------
Parameter* GuiInterpreter::CreateParameter(const std::string &type, 
                                           const std::string &name,
                                           const std::string &ownerName,
                                           const std::string &depName,
                                           bool manage)
{
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   Parameter *obj =
      theModerator->CreateParameter(type, name, ownerName, depName, manage);
   
   #ifdef DEBUG_SYNC_STATUS
   MessageInterface::ShowMessage
      ("GuiInterpreter::CreateParameter() type='%s', name='%s', Setting GUI %s\n",
       type.c_str(), name.c_str(), obj ? "dirty" : "error");
   #endif
   
   if (obj == NULL)
      mainFrame->UpdateGuiScriptSyncStatus(3, 0); // Set GUI error
   else
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   
   return obj;
}


//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type,
//                              const const std::string &name,
//                              bool createDefault = true)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* GuiInterpreter::CreateSubscriber(const std::string &type,
                                             const std::string &name,
                                             bool createDefault)
{
   // Set object manage option to configuration object
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   theModerator->SetObjectManageOption(1);
   Subscriber *obj =
      theModerator->CreateSubscriber(type, name, createDefault);
   
   #ifdef DEBUG_SYNC_STATUS
   MessageInterface::ShowMessage
      ("GuiInterpreter::CreateSubscriber() type='%s', name='%s', Setting GUI %s\n",
       type.c_str(), name.c_str(), obj ? "dirty" : "error");
   #endif
   
   if (obj == NULL)
      mainFrame->UpdateGuiScriptSyncStatus(3, 0); // Set GUI error
   else
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   
   return obj;
}


//------------------------------------------------------------------------------
// Integer GetNumberOfActivePlots()
//------------------------------------------------------------------------------
Integer GuiInterpreter::GetNumberOfActivePlots()
{
   return theModerator->GetNumberOfActivePlots();
}


//------------------------------------------------------------------------------
// GmatBase* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a default PropSetup object.
 *
 * @param <name> PropSetup name
 *
 * @return a PropSetup object pointer
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateDefaultPropSetup(const std::string &name)
{
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   theModerator->SetObjectManageOption(1);
   GmatBase *obj = theModerator->CreateDefaultPropSetup(name);
   
   #ifdef DEBUG_SYNC_STATUS
   MessageInterface::ShowMessage
      ("GuiInterpreter::CreateDefaultPropSetup() type='PropSetup', name='%s', "
       "Setting GUI %s\n", name.c_str(), obj ? "dirty" : "error");
   #endif
   
   if (obj == NULL)
      mainFrame->UpdateGuiScriptSyncStatus(3, 0); // Set GUI error
   else
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   
   return obj;
}

//------------------------------------------------------------------------------
// GmatBase* CreateNewODEModel(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateNewODEModel(const std::string &name)
{
   return theModerator->CreateODEModel("ODEModel", name);
}

//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * @return a internal coordinate system pointer
 */
//------------------------------------------------------------------------------
CoordinateSystem* GuiInterpreter::GetInternalCoordinateSystem()
{
   return theModerator->GetInternalCoordinateSystem();
}


//------------------------------------------------------------------------------
// bool IsDefaultCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
bool GuiInterpreter::IsDefaultCoordinateSystem(const std::string &name)
{
   return theModerator->IsDefaultCoordinateSystem(name);
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types for the solar system in use.
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetPlanetarySourceTypes()
{
   return theModerator->GetPlanetarySourceTypes();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names of the solar system in use.
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetPlanetarySourceNames()
{
   return theModerator->GetPlanetarySourceNames();
}


//------------------------------------------------------------------------------
// StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
/*
 * @return planetary source type in use of the solar system in use.
 */
//------------------------------------------------------------------------------
const StringArray& GuiInterpreter::GetPlanetarySourceTypesInUse()
{
   return theModerator->GetPlanetarySourceTypesInUse();
}


////------------------------------------------------------------------------------
//// const StringArray& GetAnalyticModelNames()
////------------------------------------------------------------------------------
///*
// * @return analytic model name used of the solar system in use.
// */
////------------------------------------------------------------------------------
//const StringArray& GuiInterpreter::GetAnalyticModelNames()
//{
//   return theModerator->GetAnalyticModelNames();
//}
//
//
////------------------------------------------------------------------------------
//// bool SetAnalyticModelToUse(const std::string &modelName)
////------------------------------------------------------------------------------
//bool GuiInterpreter::SetAnalyticModelToUse(const std::string &modelName)
//{
//   return theModerator->SetAnalyticModelToUse(modelName);
//}
//

//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &filename)
//------------------------------------------------------------------------------
bool GuiInterpreter::SetPlanetarySourceName(const std::string &sourceType,
                                          const std::string &filename)
{
   return theModerator->SetPlanetarySourceName(sourceType, filename);
}


//------------------------------------------------------------------------------
// Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
//------------------------------------------------------------------------------
/*
 * @param <sourceTypes> list of file type in the priority order of use
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
{
   return theModerator->SetPlanetarySourceTypesInUse(sourceTypes);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPlanetarySourceName(const std::string &sourceType)
{
   return theModerator->GetPlanetarySourceName(sourceType);
}


//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetPotentialFileName(const std::string &fileType)
{
   return theModerator->GetPotentialFileName(fileType);
}


//------------------------------------------------------------------------------
// std::string GetFileName(const std::string &fileType, bool getFullpath = false,
//                         bool forInput = true, bool writeWarning = false,
//                         bool writeInfo = false)
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetFileName(const std::string &fileType, bool getFullpath,
                                        bool forInput, bool writeWarning, bool writeInfo)
{
   return theModerator->GetFileName(fileType, getFullpath, forInput, writeWarning, writeInfo);
}


//------------------------------------------------------------------------------
// GmatBase* CreateStopCondition(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GuiInterpreter::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   return (GmatBase*)theModerator->CreateStopCondition(type, name);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const char *type,
//                                   const std::string &name = "",
//                                   const GmatCommand *refCmd = NULL)
//------------------------------------------------------------------------------
/**
 * Creates a default command object by given type and name.
 *
 * @param <type> command type
 * @param <name> command name
 *
 * @return a command object pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::CreateDefaultCommand(const char *type,
                                                  const std::string &name,
                                                  GmatCommand *refCmd)
{
   return CreateDefaultCommand(std::string(type), name, refCmd);
}

//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name = "",
//                                   const GmatCommand *refCmd = NULL)
//------------------------------------------------------------------------------
/**
 * Creates a default command object by given type and name.
 *
 * @param <type> command type
 * @param <name> command name
 *
 * @return a command object pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::CreateDefaultCommand(const std::string &type,
                                                  const std::string &name,
                                                  GmatCommand *refCmd)
{
   GmatCommand *cmd = theModerator->CreateDefaultCommand(type, name, refCmd);
   
   #ifdef DEBUG_SYNC_STATUS
   MessageInterface::ShowMessage
      ("GuiInterpreter::CreateDefaultCommand() type='%s', name='%s', Setting GUI dirty\n",
       type.c_str(), name.c_str());
   #endif
   
   #if !defined __CONSOLE_APP__
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   if (cmd == NULL)
      mainFrame->UpdateGuiScriptSyncStatus(3, 0); // Set GUI error
   else
      mainFrame->UpdateGuiScriptSyncStatus(2, 0); // Set GUI dirty
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                        Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::AppendCommand(const std::string &type,
                                           const std::string &name, bool &retFlag,
                                           Integer sandboxNum)
{
   return theModerator->AppendCommand(type, name, retFlag, sandboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::DeleteCommand(GmatCommand *cmd, Integer snadboxNum)
{
   return theModerator->DeleteCommand(cmd, snadboxNum);
}


//------------------------------------------------------------------------------
// GmatCommand* GetFirstCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Retrieves a first command in the sequence.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a next command object pointer, return null if no command found
 */
//------------------------------------------------------------------------------
GmatCommand* GuiInterpreter::GetFirstCommand(Integer sandboxNum)
{
   return theModerator->GetFirstCommand(sandboxNum);
}


//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Appends a command.
 *
 * @param <cmd> command to append
 *
 * @return a command object pointer, return null if not appended
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
   return theModerator->AppendCommand(cmd, sandboxNum);
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
bool GuiInterpreter::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                                   Integer sandboxNum)
{
   return theModerator->InsertCommand(cmd, prevCmd, sandboxNum);
}


//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearResource()
{
   return theModerator->ClearResource();
}


//------------------------------------------------------------------------------
// bool LoadMinimumResource()
//------------------------------------------------------------------------------
bool GuiInterpreter::LoadMinimumResource()
{
   return theModerator->LoadMinimumResource();
}


//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool GuiInterpreter::LoadDefaultMission()
{
   return theModerator->LoadDefaultMission();
}


//------------------------------------------------------------------------------
// bool ClearCommandSeq(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool GuiInterpreter::ClearCommandSeq(Integer sandboxNum)
{
   return theModerator->ClearCommandSeq(true, true, sandboxNum);
}


//------------------------------------------------------------------------------
// void ClearAllSandboxes()
//------------------------------------------------------------------------------
void GuiInterpreter::ClearAllSandboxes()
{
   theModerator->ClearAllSandboxes();
}


//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to run mission sequence.
 *
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::RunMission(Integer sandboxNum)
{
   theModerator->SetWidgetCreator(&GuiInterpreter::CreateWidget);
   return theModerator->RunMission(sandboxNum);
}


//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to change run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    1 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::ChangeRunState(const std::string &state, Integer sandboxNum)
{
   Integer retval = theModerator->ChangeRunState(state, sandboxNum);
   if (state == "Pause")
   {
      GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
      mainFrame->OnPause();
   }
   return retval;
}


//------------------------------------------------------------------------------
// void ClearScript()
//------------------------------------------------------------------------------
/**
 * Clears scirpt related data
 */
//------------------------------------------------------------------------------
void GuiInterpreter::ClearScript()
{
   // Reset script data for ScriptInterpreter instance
   theModerator->ClearScript();
   // Reset script data for GuiInterpreter instance
   ScriptInterpreter::Clear();
}


//------------------------------------------------------------------------------
// bool InterpretScript(const char *filename, bool readBack = false,
//                      const char *newPath = "")
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::InterpretScript(const char *filename, bool readBack,
                                     const char *newPath)
{
   return theModerator->InterpretScript(std::string(filename), readBack,
                                        std::string(newPath));
}


//------------------------------------------------------------------------------
// Gmat::RunState GetDetailedRunState(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to to get detailed run state.
 *
 * @return detailed run state
 */
//------------------------------------------------------------------------------
Gmat::RunState GuiInterpreter::GetDetailedRunState(Integer sandboxNum)
{
   return theModerator->GetDetailedRunState(sandboxNum);
}


//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &filename, bool readBack = false,
//                      const std::string &newPath = "")
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::InterpretScript(const std::string &filename, bool readBack,
                                     const std::string &newPath)
{
   return theModerator->InterpretScript(filename, readBack, newPath);
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &filename, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <filename> output script file name
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GuiInterpreter::SaveScript(const std::string &filename,
                                Gmat::WriteMode mode)
{
   return theModerator->SaveScript(filename, mode);
}


//------------------------------------------------------------------------------
// std::string GetScript(Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/**
 * Returns Built scripts from objects
 *
 * @return built scripts from objects
 */
//------------------------------------------------------------------------------
std::string GuiInterpreter::GetScript(Gmat::WriteMode mode)
{
   return theModerator->GetScript(mode);
}


//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Executes commands built from the script file.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer GuiInterpreter::RunScript(Integer sandboxNum)
{
   return theModerator->RunScript(sandboxNum);
}


//------------------------------------------------------------------------------
// void SetInputFocus()
//------------------------------------------------------------------------------
void GuiInterpreter::SetInputFocus()
{
#if !defined __CONSOLE_APP__
   GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
   mainFrame->SetFocus();

   if (++focusCounter > 1000)
   {
      wxYield();
      focusCounter = 0;
   }

   if (mainFrame->IsIconized())
      mainFrame->ProcessPendingEvent();
#endif
}


//------------------------------------------------------------------------------
// void NotifyRunCompleted()
//------------------------------------------------------------------------------
void GuiInterpreter::NotifyRunCompleted()
{
#if !defined __CONSOLE_APP__
   GmatAppData::Instance()->GetMainFrame()->NotifyRunCompleted();
#endif
}

void GuiInterpreter::UpdateView(Integer type)
{
   if (type & 0x01)
      UpdateResourceTree();
   if (type & 0x02)
      UpdateMissionTree();
   if (type & 0x04)
      UpdateOutputTree();
}


//------------------------------------------------------------------------------
// void UpdateResourceTree()
//------------------------------------------------------------------------------
void GuiInterpreter::UpdateResourceTree()
{
#if !defined __CONSOLE_APP__
   //close the open windows first
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetMainFrame()->CloseAllChildren();
   gmatAppData->GetResourceTree()->UpdateResource(true);
#endif
}


//------------------------------------------------------------------------------
// void UpdateMissionTree()
//------------------------------------------------------------------------------
void GuiInterpreter::UpdateMissionTree()
{
#if !defined __CONSOLE_APP__
   GmatAppData::Instance()->GetMissionTree()->UpdateMission(true);
#endif
}


//------------------------------------------------------------------------------
// void UpdateOutputTree()
//------------------------------------------------------------------------------
void GuiInterpreter::UpdateOutputTree()
{
#if !defined __CONSOLE_APP__
   GmatAppData::Instance()->GetOutputTree()->UpdateOutput(false, true, true);
#endif
}


//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GuiInterpreter::CloseCurrentProject()
{
#if !defined __CONSOLE_APP__
   GmatAppData::Instance()->GetMainFrame()->CloseCurrentProject();
#endif
}


//------------------------------------------------------------------------------
// void ResetIconFile()
//------------------------------------------------------------------------------
void GuiInterpreter::ResetIconFile()
{
#if !defined __CONSOLE_APP__
   GmatAppData::Instance()->ResetIconFile();
#endif
}


//------------------------------------------------------------------------------
// void StartMatlabServer()
//------------------------------------------------------------------------------
/*
 * Starts the MATLAB server.
 */
//------------------------------------------------------------------------------
void GuiInterpreter::StartMatlabServer()
{
   GmatAppData::Instance()->GetMainFrame()->StartMatlabServer();   
}


//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// GuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter::GuiInterpreter()
   : ScriptInterpreter()
{
   Initialize();
   isInitialized = false;
   focusCounter = 0;
}


//------------------------------------------------------------------------------
// GuiInterpreter(const GuiInterpreter&)
//------------------------------------------------------------------------------
GuiInterpreter::GuiInterpreter(const GuiInterpreter&)
{
   continueOnError = false;
}


//------------------------------------------------------------------------------
// GuiInterpreter& operator=(const GuiInterpreter &guiInterpreter)
//------------------------------------------------------------------------------
GuiInterpreter& GuiInterpreter::operator=(const GuiInterpreter &guiInterpreter)
{
   return *this;
}

std::vector<Gmat::PluginResource*> *GuiInterpreter::GetUserResources()
{
   return theModerator->GetPluginResourceList();
}

//------------------------------------------------------------------------------
// std::vector <GuiFactory*> GuiInterpreter::RetrieveGuiFactories()
//------------------------------------------------------------------------------
/**
 * Accesses the GUI factories so that plugin GUI elements can be built
 *
 * @return The GuiFactory pointer vector
 */
//------------------------------------------------------------------------------
std::vector <GuiFactory*> GuiInterpreter::RetrieveGuiFactories()
{
   return theModerator->RetrieveGuiFactories();
}

//------------------------------------------------------------------------------
// GmatWidget* CreateWidget(const std::string ofType, GmatBase *forObject,
//       void *withParent)
//------------------------------------------------------------------------------
/**
 * Creates a widget and returns it to the calling function
 *
 * Note that this static method is the "pass through base code" callback
 * function, so some type casting will be required on the ends of the calls.
 *
 * @param ofType The string describing the desired widget
 * @param forObject The object that uses the widget
 * @paran withParent The parent pointer for the widget
 *
 * @return A GmatWidget package that has the widget in it
 */
//------------------------------------------------------------------------------
GmatWidget* GuiInterpreter::CreateWidget(const std::string &ofType,
      GmatBase *forObject, void *withParent)
{
   GmatWidget *theWidget = NULL;

   GmatMainFrame *mainframe = GmatAppData::Instance()->GetMainFrame();

   std::string oName = forObject->GetName();
   mainframe->CreatePluginChild(oName, oName, ofType,
         GmatTree::USER_DEFINED_OBJECT, forObject, &theWidget);

   return theWidget;
}
