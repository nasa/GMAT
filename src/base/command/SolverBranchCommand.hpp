//$Id$
//------------------------------------------------------------------------------
//                            SolverBranchCommand
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
// number NNG06CA54C
//
// Author: Darrel J. Conway
// Created: 2006/10/20
//
/**
 * Definition for the Solver loop command base class (Target, Optimize and 
 * Iterate).
 */
//------------------------------------------------------------------------------


#ifndef SolverBranchCommand_hpp
#define SolverBranchCommand_hpp

#include "BranchCommand.hpp"
#include "Solver.hpp"
#include "ISolverListener.hpp"

class Subscriber;

class GMAT_API SolverBranchCommand : public BranchCommand
{
public:
        SolverBranchCommand(const std::string &typeStr);
        virtual ~SolverBranchCommand();
   SolverBranchCommand(const SolverBranchCommand& sbc);

   SolverBranchCommand&    operator=(const SolverBranchCommand& sbc);

   // listener (solver window) code
   virtual void        AddListener( ISolverListener* listener );
   virtual void        RemoveListener( ISolverListener* listener );
   virtual void        ClearListeners();
   virtual void        NotifyVariabledChanged(std::string name, Real value);
   virtual void        NotifyVariabledChanged(std::string name, std::string &value);
   virtual void        NotifyConstraintChanged(std::string name, Real desiredValue, Real value);

   virtual GmatCommand*    GetNext();
   virtual bool            Initialize();
   virtual bool            TakeAction(const std::string &action, 
                                      const std::string &actionData = "");
   virtual void            RunComplete();
   
   // Handle parsing internally
   virtual bool InterpretAction();
   
   // For Ref. Objects
   virtual bool        RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const UnsignedInt type);
   
   // Parameter access methods
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id) const; 
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                       GetStringArrayParameter(const std::string &label) const;
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id, 
                                           const bool value);
   virtual bool        GetBooleanParameter(const std::string &label) const;
   virtual bool        SetBooleanParameter(const std::string &label, 
                                           const bool value);

   virtual bool        NeedsServerStartup();
   virtual Integer     GetCloneCount();
   virtual GmatBase*   GetClone(Integer cloneIndex = 0);

   virtual bool        HasLocalClones();
   virtual void        UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);

protected:
   // Mode definitions for the state machine overrides
   enum solverStartMode
   {
      RUN_INITIAL_GUESS,
      RUN_AND_SOLVE,
      RUN_SOLUTION
   };
   
   // Mode definitions used on convergence
   enum solverExitMode
   {
      DISCARD_AND_CONTINUE,
      SAVE_AND_CONTINUE,
      STOP
   };
   
   std::list<ISolverListener*> listeners;
   std::string         solverName;
   Solver              *theSolver; 
   solverStartMode     startMode;
   solverExitMode      exitMode;
   bool                showProgressWindow;
   Solver::SolverState specialState;
   Integer             queryStatusID;
   
   /// Modes used in the solver, filled in the derived classes
   StringArray         solverModes;    
   /// Modes used to leave the solver, filled in the derived classes
   StringArray         exitModes;    

   /// Local store of the objects that we'll need to reset
   ObjectArray         localStore;

   /// Active subscribers (only XY plots for now) so the penups can be managed
   std::vector<Subscriber*> activeSubscribers;

   /// Parsing function for SolveMode and ExitMode
   void                CheckForOptions(std::string &opts);
   /// Returns the text for options that are not set to the default values
   std::string         GetSolverOptionText();
   
   // Methods used to save the starting point for the loops
   virtual void        StoreLoopData();
   virtual void        ResetLoopData();
   virtual void        FreeLoopData();
   
   virtual void        ApplySolution();
   
   virtual void        GetActiveSubscribers();
   virtual void        PenUpSubscribers();
   virtual void        PenDownSubscribers();
   virtual void        DarkenSubscribers(Integer denominator = 1);
   virtual void        LightenSubscribers(Integer denominator = 1);
   virtual void        SetSubscriberBreakpoint();
   virtual void        ApplySubscriberBreakpoint(Integer bp = -1);
   virtual void        PrepareToPublish(bool publishAll = true);
   virtual void        PublishData();

   virtual void        ChangeRunState(Gmat::RunState newState);

   enum
   {
      SOLVER_NAME_ID  = BranchCommandParamCount,
      SOLVER_SOLVE_MODE,
      SOLVER_EXIT_MODE,
      SOLVER_SOLVE_MODE_OPTIONS,
      SOLVER_EXIT_MODE_OPTIONS,
      SOLVER_SHOW_PROGRESS,
      SolverBranchCommandParamCount
   };
   
};

#endif /*SOLVERBRANCHCOMMAND_HPP_*/
