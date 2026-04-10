//$Id$
//------------------------------------------------------------------------------
//                                  GmatCommand
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
// Created: 2003/09/23
//
/**
 * Definition for the GmatCommand base class
 */
//------------------------------------------------------------------------------


#ifndef Command_hpp
#define Command_hpp


#include <map>              // for mapping between object names and types
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CommandException.hpp"

// Headers used by commands that override InterpretAction
#include "TextParser.hpp"
#include "ElementWrapper.hpp"

// Headers for the referenced classes
#include "SolarSystem.hpp"    // For SolarSystem
#include "Publisher.hpp"      // For the Publisher and ...
#include "SpaceObject.hpp"    // For SpaceObjects
#include "TriggerManager.hpp" // Trigger managers, usually from a plugin

// Forward reference for the transient force vector
class PhysicalModel;

// Forward reference for the calling FunctionManager
class FunctionManager;

// Forward reference for the function containing this command
class Function;

// Other forward references
class Parameter;

/**
 * GmatCommand Base Class, used for Mission Control Sequence elements in scripts
 *
 * The GmatCommands all follow a "late-binding" philosophy, in that they do
 * not set object associations until the Sandbox has been populated with both
 * the objects that are used in the model and with the complete GmatCommand 
 * sequence.  Once the Sandbox is populated, it initializes the GmatCommand 
 * sequence by calling Initialize() on each GmatCommand, and then runs the
 * sequence by calling Execute() on the first GmatCommand in the sequence.
 */
class GMAT_API GmatCommand : public GmatBase
{
public:
   // class constructor
   GmatCommand(const std::string &typeStr);
   // class destructor
   virtual ~GmatCommand();
   // Copy constructor
   GmatCommand(const GmatCommand &c);
   // Assignment operator
   GmatCommand&         operator=(const GmatCommand &c);
   
   void                 SetGeneratingString(const std::string &gs);
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   
   // Methods for function
   virtual void         SetCurrentFunction(Function *function);
   virtual Function*    GetCurrentFunction();
   
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   // other methods for setting up the object w.r.t the elements needed
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                        const std::string &withName);
   virtual void         ClearWrappers();
   virtual void         CheckDataType(ElementWrapper* forWrapper,
                                      Gmat::ParameterType needType,
                                      const std::string &cmdName,
                                      bool ignoreUnsetReference = false,
                                      bool checkUnsetValue = false,
                                      Real unsetValue = 9.999999e300,
                                      const std::string &unsetValueErrMsg = "");
   
   // Methods used to setup objects
   virtual bool         SetObject(const std::string &name,
                                  const UnsignedInt type,
                                  const std::string &associate = "",
                                  const UnsignedInt associateType =
                                       Gmat::UNKNOWN_OBJECT);
   virtual bool         SetObject(GmatBase *obj,
                                  const UnsignedInt type);
   virtual GmatBase*    GetGmatObject(const UnsignedInt type,
                                  const std::string objName = "");
   
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetupSummary(const std::string &csName,
                                     bool entireMission = false,
                                     bool physicsOnly = false);
   virtual void         SetSummaryName(const char *sumName);
   virtual void         SetSummaryName(const std::string &sumName);
   virtual std::string  GetSummaryName();
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetTriggerManagers(std::vector<TriggerManager*> *trigs);
   virtual void         SetConfiguredObjectMap(ObjectMap *objMap);
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual ObjectMap*   GetObjectMap();
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetPublisher(Publisher *p);
   virtual Publisher*   GetPublisher();
   virtual bool         GetPropStatus();

   // Methods used in validation
   virtual const StringArray& GetObjectList();
   virtual bool         AcceptsObjectType(UnsignedInt theType);
   virtual bool         Validate();

   // Access methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string &label, 
                                          const char *value);
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index);
   
   // methods to set conditions and operators between multiple conditions;
   // atIndex = -999 means add it to the end of the list; otherwise, add/replace
   // the condition at the specified index
   virtual bool         SetCondition(const std::string &lhs, 
                                     const std::string &operation, 
                                     const std::string &rhs, 
                                     Integer atIndex = -999);
   virtual bool         SetConditionOperator(const std::string &op, 
                                             Integer atIndex = -999);
   
   virtual bool         RemoveCondition(Integer atIndex);
   virtual bool         RemoveConditionOperator(Integer atIndex);
   
   // Sequence methods
   virtual bool         Initialize();
   virtual GmatCommand* GetNext();
   virtual GmatCommand* GetPrevious();
   virtual bool         ForceSetNext(GmatCommand *toCmd);     // dangerous!
   virtual bool         ForceSetPrevious(GmatCommand *toCmd); // dangerous!
   virtual bool         Append(GmatCommand *cmd);
   virtual bool         Insert(GmatCommand *cmd, GmatCommand *prev);
   virtual GmatCommand* Remove(GmatCommand *cmd);
   
   virtual GmatCommand* GetChildCommand(Integer whichOne = 0);
   virtual Integer      GetVariableCount();
   virtual Integer      GetGoalCount();
   
   virtual bool         InterpretAction();
   virtual bool         VerifyObjects();
   
   Integer              DepthIncrement();
   bool                 HasPropStateChanged();
   
   //---------------------------------------------------------------------------
   //  bool GmatCommand::Execute()
   //---------------------------------------------------------------------------
   /**
    * The method that is fired to perform the GmatCommand.
    *
    * Derived classes implement this method to perform their actions on
    * GMAT objects.
    *
    * @return true if the GmatCommand runs to completion, false if an error
    *         occurs.
    */
   //---------------------------------------------------------------------------
   virtual bool        Execute() = 0;
   virtual bool        SkipInterrupt();
   virtual void        RunComplete(bool isHead);
   virtual void        RunComplete();
   virtual void        SetRunState(Gmat::RunState newState);
   
   bool                HasConfigurationChanged();
   virtual void        ConfigurationChanged(bool tf);
   virtual bool        HasAFunction();
   virtual bool        NeedsServerStartup();
   virtual bool        IsExecuting();
   
   virtual Integer     GetCloneCount();
   virtual GmatBase*   GetClone(Integer cloneIndex = 0);

   virtual bool        AffectsClones();
   virtual GmatBase*   GetUpdatedObject();
   virtual Integer     GetUpdatedObjectParameterIndex();

   virtual void        SetDataPublishingFlag(bool allow) { isAllowDataPublishing = allow; };
   virtual bool        GetDataPublishingFlag() { return isAllowDataPublishing; };

protected:
   enum
   {
      COMMENT = GmatBaseParamCount,
      SUMMARY,
      MISSION_SUMMARY,
      GmatCommandParamCount
   };
   
   /// Command parameter labels
   static const std::string 
                     PARAMETER_TEXT[GmatCommandParamCount - GmatBaseParamCount];
   /// Command parameter types
   static const Gmat::ParameterType 
                     PARAMETER_TYPE[GmatCommandParamCount - GmatBaseParamCount];
   
   static const Integer MAX_NUM_TANKS;

   static const std::string MISSION_CHANGE_MESSAGE;
   static const std::string UNDEFINED_KEPLERIAN_ELEMENTS;
   static const std::string UNDEFINED_SPHERICAL_ELEMENTS;

   /// Flag to allow publishing data or not in this command
   bool                 isAllowDataPublishing;

   /// Map containing names and associated types
   ObjectTypeMap        association;
   /// List of the associated objects
   StringArray          objects;
   // pointer to the function that contains this command
   Function             *currentFunction;
   // pointer to the function that is calling this command (ignored for all but
   // CallFunction and Assignment)
   FunctionManager      *callingFunction;
   /// Pointer to the next GmatCommand in the sequence; NULL at the end
   GmatCommand          *next;
   /// Pointer to the previous GmatCommand in the sequence; NULL if at the start
   GmatCommand          *previous;
   /// Indicator of the current nesting level
   Integer              level;
   /// Object store for resource configuration. This is used for checking
   /// resource name is the same as built-in function name
   ObjectMap            *configObjectMap;
   /// Object store obtained from the Sandbox
   ObjectMap            *objectMap;
   /// Object store obtained from the Sandbox
   ////std::map<std::string, GmatBase *> *globalObjectMap;
   ObjectMap            *globalObjectMap;
   /// Solar System, set by the local Sandbox
   SolarSystem          *solarSys;
   /// Trigger managers, set by the local Sandbox
   std::vector<TriggerManager*> *triggerManagers;
   /// Internal coordinate system, set by the local Sandbox
   CoordinateSystem     *internalCoordSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// Publisher for data generated by this GmatCommand
   Publisher            *publisher;
   /// Stream ID issued by the Publisher to identify which Command is publishing
   Integer              streamID;
   /// Change in branch depth caused by this command
   Integer              depthChange;
   /// Flag indicating that the command changes the state vector for propagation
   bool                 commandChangedState;
   /// String used for the command summary data
   std::string          commandSummary;
   /// Coordinate System used for the Command Summary display
   std::string          summaryCoordSysName;
   /// Current coordinate system for Command Summary
   CoordinateSystem     *summaryCoordSys;
   /// flag indicating whether or not the summary for the command is part of
   /// a requested summary for the entire misison (or branch)
   bool                 summaryForEntireMission;
   /// flag indicating whether or not the entire-mission summary should only include
   /// physics-based commands
   bool                 missionPhysicsBasedOnly;
   /// flag indicating whether or not this command is a physics-based command
   bool                 physicsBasedCommand;
   /// flag indicating whether or not to include this type of command in a command summary
   bool                 includeInSummary;
   /// node name (on the GUI) for the command
   std::string          summaryName;

//   /// Optional comment string associated with the command
//   std::string          comment;
   /// Flag indicating that a command has been changed by a user
   bool                 commandChanged;
   
   /// Text parser used by commands that override InterpretAction
   TextParser           parser;
   /// The list of settable entities for the command
   StringArray          settables;
   /// List used to initialize the local TextParser
   StringArray          commandNameList;
   
   /// Count of owned objects created through cloning
   Integer              cloneCount;

   /// Field used for processes that only function in certain run states
   Gmat::RunState       currentRunState;

   // IDs used to buffer the command summary data
   static Integer       satEpochID;
   static Integer       satCdID;
   static Integer       satDragAreaID;
   static Integer       satCrID;
   static Integer       satSRPAreaID;
   static Integer       satTankID;
   static Integer       satThrusterID;
   static Integer       satDryMassID;
   static Integer       satTotalMassID;
   static Integer       satSPADDragScaleFactorID;
   static Integer       satSPADSRPScaleFactorID;


   // Command summary data buffers
   Real                 *epochData;
   Real                 *stateData;
   Real                 *parmData;
   Real                 *fuelMassData;
   StringArray          tankNames;

   std::vector <SpaceObject*>
                        satVector;
   Integer              satsInMaps;

   /// A list of object names, their elements are published
   StringArray          pubdataOwners;
   /// A list of object elements which are published
   StringArray          pubdataElements;
   /// Start index of covariance section
   Integer              covStartIndex;
   /// Start index of the attitude section
   Integer              attStartIndex;


   virtual bool         AssignObjects();
   virtual bool         ClearObjects();
   virtual void         BuildCommandSummary(bool commandCompleted = true);
   virtual void         BuildCommandSummaryString(bool commandCompleted = true);
   virtual const std::string 
                        BuildMissionSummaryString(const GmatCommand* head = NULL);
   // for command name
   virtual void         InsertCommandName(std::string &genString);
   
   // for Debug
   virtual void         ShowCommand(const std::string &prefix,
                                    const std::string &title1, GmatCommand *cmd1,
                                    const std::string &title2 = "",
                                    GmatCommand *cmd2 = NULL,
                                    bool showDetail = false);
   virtual void         ShowWrapper(const std::string &prefix,
                                    const std::string &title,
                                    ElementWrapper *wrapper);
   virtual void         ShowObjectMaps(const std::string &title = "");
   
   // for the Parameters in Commands updates
   StringArray          InterpretPreface();
   bool                 IsSettable(const std::string &setDesc);
   bool                 SeparateEquals(const std::string &description,
                                       std::string &lhs, std::string &rhs,
                                       bool checkOp = false);
   
   // For objects
   GmatBase* FindObject(const std::string &name);
   // For referencing cloned objects
   virtual void        HandleReferencesToClones(Parameter *param);
   
   // Method(s) used for ParametersInCommands
   bool                SetWrapperReferences(ElementWrapper &wrapper);
   
   // Used for deleting old ElementWrappers
   std::vector<ElementWrapper*> oldWrappers;   
   void                ClearOldWrappers();
   void                CollectOldWrappers(ElementWrapper **wrapper);
   void                DeleteOldWrappers();

   // Publish methods that are overridden as needed
   virtual void        PrepareToPublish(bool publishAll = true);
   virtual void        PublishData();

};

#endif // Command_hpp
