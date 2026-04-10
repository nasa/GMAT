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
 * Declares the operations between GUI subsystem and the Moderator.
 */
//------------------------------------------------------------------------------
#ifndef GuiInterpreter_hpp
#define GuiInterpreter_hpp

#include "ScriptInterpreter.hpp"
#include "GuiFactory.hpp"
#include "GmatWidget.hpp"

#include "gmatdefs.hpp"
#include <sstream>      // for std::istringstream

// GMAT_API not used here because the GuiInterpreter is not exported from a dll
class GuiInterpreter : public ScriptInterpreter
{
public:

   static GuiInterpreter* Instance();

   // Interpreter abstract methods
   virtual bool Interpret(GmatCommand *inCmd, std::istringstream *ss);
   virtual std::vector<Integer> GetErrorLines();
   virtual std::vector<Integer> GetWarningLines();
   
   void Finalize();
   
   //----- running object
   GmatBase* GetRunningObject(const char *name);
   GmatBase* GetRunningObject(const std::string &name);
   
   //----- factory
   const StringArray& GetListOfAllFactoryItems();
   const StringArray& GetListOfFactoryItems(UnsignedInt type, const char *qualifier);
   const StringArray& GetListOfFactoryItems(UnsignedInt type, const std::string &qualifier = "");
   const StringArray& GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types);
   std::string GetStringOfAllFactoryItems(UnsignedInt type);
   std::string GetStringOfAllFactoryItemsExcept(const ObjectTypeArray &types);
   
   //----- configuration
   std::string GetNewName(const std::string &name, Integer startCount);
   GmatBase* AddClone(const std::string &name, std::string &newName);
   bool RenameObject(UnsignedInt type, const char *oldName,
                     const char *newName);
   bool RenameObject(UnsignedInt type, const std::string &oldName,
                     const std::string &newName);
   bool RemoveObject(UnsignedInt type, const std::string &name);
   bool RemoveObjectIfNotUsed(UnsignedInt type, const char *name);
   bool RemoveObjectIfNotUsed(UnsignedInt type, const std::string &name);
   bool HasConfigurationChanged(Integer sandboxNum = 1);
   void ConfigurationChanged(GmatBase *obj, bool tf);
   void ResetConfigurationChanged(bool resetResource = true,
                                  bool resetCommands = true,
                                  Integer sandboxNum = 1);
   
   // General Object
   GmatBase* CreateObject(const char *type, const std::string &name,
                          Integer manage = 1, bool createDefault = false);
   GmatBase* CreateObject(const char *type, const char *name,
                          Integer manage = 1, bool createDefault = false);
   virtual GmatBase* CreateObject(const std::string &type, const std::string &name,
                          Integer manage = 1, bool createDefault = false,
                          bool includeLineOnError = false, bool showWarning = false);

   
   // SolarSystem
   SolarSystem* GetDefaultSolarSystem();
   SolarSystem* GetSolarSystemInUse();
   CoordinateSystem* GetInternalCoordinateSystem();
   bool IsDefaultCoordinateSystem(const std::string &name);
   
   // Parameter
   bool IsParameter(const std::string &type);
   Parameter* GetParameter(const char *name);
   Parameter* GetParameter(const std::string &name);
   virtual Parameter* CreateParameter(const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName = "",
                                      const std::string &depName = "",
                                      bool manage = true);
   
   // Subscriber
   Subscriber* CreateSubscriber(const std::string &type,
                                const std::string &name,
                                bool createDefault = true);
   Integer GetNumberOfActivePlots();
   
   GmatBase* CreateDefaultPropSetup(const std::string &name);
   GmatBase* CreateNewODEModel(const std::string &name);
   
   // Planetary source
   const StringArray& GetPlanetarySourceTypes();
   const StringArray& GetPlanetarySourceNames();
   const StringArray& GetPlanetarySourceTypesInUse();
//   const StringArray& GetAnalyticModelNames();
//   bool SetAnalyticModelToUse(const std::string &modelName);
   bool SetPlanetarySourceName(const std::string &sourceType,
                               const std::string &filename);
   Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes);
   std::string GetPlanetarySourceName(const std::string &sourceType);
   
   // Potential field files
   std::string GetPotentialFileName(const std::string &fileType);
   
   // Getting file names
   std::string GetFileName(const std::string &fileType, bool getFullpath = false,
                           bool forInput = true, bool writeWarning = false,
                           bool writeInfo = false);
   
   // StopCondition
   GmatBase* CreateStopCondition(const std::string &type,
                                 const std::string &name);
   
   // Command
   GmatCommand* CreateDefaultCommand(const char *type,
                                     const std::string &name = "",
                                     GmatCommand *refCmd = NULL);
   GmatCommand* CreateDefaultCommand(const std::string &type,
                                     const std::string &name = "",
                                     GmatCommand *refCmd = NULL);
   GmatCommand* AppendCommand(const std::string &type, const std::string &name,
                              bool &retFlag, Integer sandboxNum = 1);
   GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   GmatCommand* GetFirstCommand(Integer sandboxNum = 1);
   bool AppendCommand(GmatCommand *cmd, Integer sandboxNum = 1);
   bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                      Integer sandboxNum = 1);
   
   // Resource
   bool ClearResource();
   bool LoadMinimumResource();
   
   // Command sequence
   bool LoadDefaultMission();
   bool ClearCommandSeq(Integer sandboxNum = 1);
   
   // Sandbox
   void ClearAllSandboxes();
   Integer RunMission(Integer sandboxNum = 1);
   virtual Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);
   Gmat::RunState GetDetailedRunState(Integer sandboxNum = 1);
   
   // Script
   void ClearScript();
   bool InterpretScript(const char *filename, bool readBack = false,
                        const char *newPath = "");
   bool InterpretScript(const std::string &filename, bool readBack = false,
                        const std::string &newPath = "");
   bool SaveScript(const std::string &filename,
                   Gmat::WriteMode mode = Gmat::SCRIPTING);
   std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING);
   
   Integer RunScript(Integer sandboxNum = 1);

   // GUI control
   virtual void SetInputFocus();
   virtual void NotifyRunCompleted();
   virtual void UpdateView(Integer type = 7);
   virtual void CloseCurrentProject();
   virtual void ResetIconFile();
   
   void UpdateResourceTree();
   void UpdateMissionTree();
   void UpdateOutputTree();
   virtual void StartMatlabServer();
   
   // GUI Plugins
   std::vector<GuiFactory*> RetrieveGuiFactories();
   std::vector<Gmat::PluginResource*> *GetUserResources();
   static GmatWidget* CreateWidget(const std::string &ofType, GmatBase *forObject,
         void *withParent);

private:

   GuiInterpreter();
   virtual ~GuiInterpreter();
   GuiInterpreter(const GuiInterpreter&);
   GuiInterpreter& operator=(const GuiInterpreter &guiInterpreter);

   // member data
   bool isInitialized;
   Integer focusCounter;

   static GuiInterpreter *instance;
};


#endif // GuiInterpreter_hpp

