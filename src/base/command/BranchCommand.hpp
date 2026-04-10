//$Id$
//------------------------------------------------------------------------------
//                               BranchCommand
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
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Command classes that branch (Target, If, While, etc).
 */
//------------------------------------------------------------------------------


#ifndef BranchCommand_hpp
#define BranchCommand_hpp


#include <vector>
#include "GmatCommand.hpp"


class GMAT_API BranchCommand : public GmatCommand
{
public:
   BranchCommand(const std::string &typeStr);
   BranchCommand(const BranchCommand& bc);
   BranchCommand&          operator=(const BranchCommand& bc);
   virtual ~BranchCommand();
   
   void                    AddBranch(GmatCommand *cmd, Integer which = 0);
   void                    AddToFrontOfBranch(GmatCommand *cmd,
                                              Integer which = 0);
   virtual bool            ExecuteBranch(Integer which = 0);
   
   // Inherited methods that need refinements to handle the branching
   virtual bool            Append(GmatCommand *cmd);
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);
   virtual GmatCommand*    Remove(GmatCommand *cmd);
   virtual void            BuildCommandSummaryString(bool commandCompleted = true);
   // Insert into the main sequence, not into a branch
   virtual bool            InsertRightAfter(GmatCommand *cmd);
   
   virtual void            SetSolarSystem(SolarSystem *ss);
   virtual void            SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void            SetObjectMap(std::map<std::string, GmatBase*> *map);
   virtual void            SetGlobalObjectMap(std::map<std::string, GmatBase*> *map);
   
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode =
                                               Gmat::SCRIPTING,
                                               const std::string &prefix = "",
                                               const std::string &useName = "");
   
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);
   
   virtual GmatCommand*    GetNext();
   virtual GmatCommand*    GetChildCommand(Integer whichOne = 0);
   GmatCommand*            GetNextWhileExecuting();
   virtual void            SetTransientForces(std::vector<PhysicalModel*> *tf);

   virtual bool            Initialize();
   virtual bool            TakeAction(const std::string &action, 
                                      const std::string &actionData = "");
   virtual bool            Execute();
   virtual void            RunComplete();
   // method to handle GmatFunctions
   const std::vector<GmatCommand*>
                           GetCommandsWithGmatFunctions();
   virtual bool            HasAFunction();
   virtual void            SetCallingFunction(FunctionManager *fm);
   virtual bool            IsExecuting();
   virtual bool            NeedsServerStartup();
   virtual void            ResetExecutionState();

   virtual Integer         GetCloneCount();
   virtual GmatBase*       GetClone(Integer cloneIndex);

   virtual bool            AffectsClones();
   virtual GmatBase*       GetUpdatedObject();
   virtual Integer         GetUpdatedObjectParameterIndex();

   virtual std::string     GetCurrentGeneratingString(Gmat::WriteMode mode,
                                 const std::string &prefix = "", Integer nestCount = 0);

   virtual bool            GetPropStatus();

   virtual std::string AddFileInfoToErrorMessage(std::string msg);

protected:
      
   // no additional parameters to add at this time
   enum
   {
      BranchCommandParamCount = GmatCommandParamCount,
   };
   
   /// The managed branch(es)
   std::vector <GmatCommand *> branch;
   /// Flag used to indicate if the command is finished executing
   bool                        commandComplete;
   /// Flag used to indicate a run is being executed
   bool                        commandExecuting;
   /// Flag used to indicate a branch is being executed
   bool                        branchExecuting;
   /// the branch that is executing
   Integer                     branchToExecute;
   /// The branch that is being filled while the command sequence is being built
   Integer                     branchToFill;
   /// Local container used to return the full sequence from the branches
   std::string                 fullString;
   /// Counter to track how deep the nesting is
   Integer                     nestLevel;
   /// Currently executing member of the branch.  NULL if branch not executing.
   GmatCommand                 *current;
   /// Most recently executed member of the branch.  NULL if branch not executed.
   GmatCommand                 *lastFired;
   /// Current nest depth of the mission sequence being executed.
   Integer                         nestCountDepth;
   
   std::vector<GmatCommand*>             
                               cmdsWithFunctions;
   
   bool  ShiftBranches(GmatCommand *startWith, Integer ofBranchNumber);
   void  SetPreviousCommand(GmatCommand *cmd, GmatCommand *prev, bool skipBranchEnd);
};

#endif // BranchCommand_hpp
