//$Id: RunEstimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         RunEstimator
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/03
//
/**
 * Definition of the MissionControlSequence command that drives estimation
 */
//------------------------------------------------------------------------------


#ifndef RunEstimator_hpp
#define RunEstimator_hpp

#include <RunSolver.hpp>

#include "PropSetup.hpp"
#include "Estimator.hpp"
#include "EventManager.hpp"


/**
 * Mission Control Sequence Command that runs data estimation
 *
 * This command interacts with an estimator to perform the estimation process.
 * Each GMAT estimator performs estimation by running a finite state machine
 * that implements an estimation algorithm.  The RunEstimator command performs
 * command side actions required by the state machine during this process.
 */
class ESTIMATION_API RunEstimator : public RunSolver
{
public:
   RunEstimator(const std::string &typeStr = "RunEstimator");
   virtual ~RunEstimator();
   RunEstimator(const RunEstimator& rs);
   RunEstimator& operator=(const RunEstimator& rs);

   virtual GmatBase* Clone() const;

   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual GmatBase* GetRefObject(const UnsignedInt type,
         const std::string &name);
   virtual bool SetRefObjectName(const UnsignedInt type,
         const std::string &name);
   virtual bool RenameRefObject(const UnsignedInt type,
         const std::string &oldName, const std::string &newName);
   virtual const std::string& GetGeneratingString(Gmat::WriteMode mode,
         const std::string &prefix, const std::string &useName);

   virtual bool Initialize();
   virtual bool PreExecution();
   virtual bool Execute();
   virtual void RunComplete();
   virtual bool TakeAction(const std::string &action,
                           const std::string &actionData = "");

   virtual GmatCommand* GetNext();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);

   //Parses commands
   virtual bool InterpretAction();
   /// Parsing function for SolveMode and ExitMode
   void                CheckForOptions(std::string &opts);

protected:
   /// The estimator that drives this process
   Estimator      *theEstimator;
   /// Flag indicating if command execution is started
   bool commandRunning;
//   /// Flag indicating if propagation is running and needs reentrance
//   bool commandPropagating;
   /// Flag indicating if command execution is done
   bool commandComplete;
   /// Flag indicating a new pass is occurring
   bool startNewPass;
   /// Flag indicating current estimate has prepped the propagator
   bool propPrepared;
   /// Time offset matching the estimation epoch
   Real estimationOffset;

   /// Flag indicating is the buffers have been filled
   bool bufferFilled;
   /// List of Event objects to process
   ObjectArray eventList;
   /// The current Event that is getting processed
   Event *currentEvent;
   /// Index of the current Event in the eventList
   UnsignedInt eventIndex;
   /// Flag indicating when all Events have been processed
   bool eventProcessComplete;

   /// The event manager
   EventManager *eventMan;

   /// Time different used while running the event code
   Real dt;

   //Solve mode state when command is used
   std::string solveMode;

   // Methods called by specific states of the finite state machine
   virtual void PrepareToEstimate();
   virtual void Propagate();
   virtual void Calculate();
   void LocateEvent();
   virtual void Estimate();
   void Accumulate();
   void CheckConvergence();
   void Finalize();

   // Helper methods
   virtual void SetPropagationProperties(PropagationStateManager *psm);
   virtual void CleanUpEvents();

   virtual void PublishState();

   virtual void SetCovNames(const std::string& name,
      StringArray& owners, StringArray& elements);

   virtual void UpdateInitialConditions();
   virtual void CacheForceDerivatives();

   virtual void UpdateCov();

private:
   bool delayInitialization;

   void LoadSolveForsToESM();

   /// This function displays warning message when solve-for spacecraft state when 
   /// central body of both spacecraft differs from the central body of force model 
   void DifferenceOfCentralBodiesWarningMessage(GmatBase *obj);
};

#endif /* RunEstimator_hpp */
