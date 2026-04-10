//$Id$
//------------------------------------------------------------------------------
//                       PropagationEnabledCommand
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/20
//
/**
 * Definition of the PropagationEnabledCommand base class
 */
//------------------------------------------------------------------------------


#ifndef PropagationEnabledCommand_hpp
#define PropagationEnabledCommand_hpp

#include "GmatCommand.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"

#include "Spacecraft.hpp"
#include "FormationInterface.hpp"


/// A convenient typedef used in this code
typedef std::vector<GmatBase*> PropObjectArray;


#define TIME_ROUNDOFF 1.0e-6
#define DEFAULT_STOP_TOLERANCE 1.0e-7



/**
 * PropagationEnabledCommand is a base class used for commands that perform
 * propagation.  It provides the methods and interfaces needed perform basic
 * time-based propagation.  It does not provide interfaces for more complicated
 * stopping conditions; derived classes provide those interfaces.
 */
class GMAT_API PropagationEnabledCommand : public GmatCommand
{
public:
   PropagationEnabledCommand(const std::string &typeStr);
   // Abstract to force a derived class to instantiate
   virtual ~PropagationEnabledCommand() = 0;
   PropagationEnabledCommand(const PropagationEnabledCommand& pec);
   PropagationEnabledCommand& operator=(const PropagationEnabledCommand& pec);

   virtual void SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool Initialize();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);
   virtual bool CreateNewSegments(Real epochNow);

protected:
   // todo: Merge the propagator objects in the Propagate command into this code

   // Check if Need to make Finite Burn Segments

   bool createNewSegmentsNow = false;

   /// Names of the PropSetups used by this command, as set in a derived class
   StringArray                   propagatorNames;
   /// Step direction multipliers used to switch btwn forwards & backwards prop
   Real                          direction;
   /// The PropSetup used by this command, as set in a derived class
   std::vector<PropSetup*>       propagators;
   /// Flag used to indicate that the PropSetups were built in a derived class
   bool                          overridePropInit;
   /// The objects that are propagated; one StringArray per PropSetup
   std::vector<StringArray>      propObjectNames;
   /// The objects that are propagated; one PropObjectArray per PropSetup
   std::vector<PropObjectArray*> propObjects;

   /// Flag to indicate that it needs to reset spacecrafts' STM to identity matrix.
   //  Spacecrafts' STM needs to reset when running RunSimulation or RunEstimation commands.
   //  But for Propagation command, it does not need.
   //@todo: In Propate command, we should have option to reset or keep the current value of STM in the script syntax
   bool needToResetSTM;

   /// The complete set of spacecraft and formations that are propagated
   ObjectArray                   sats;

   /// Flag indicating that the command has been executed once, so that some
   /// pieces of initialization can be skipped
   bool hasFired;
   /// Flag indicating the command is currently executing; used for reentrance
   bool inProgress;
   /// Flag used to skip ephemeris propagators, used for the TDRS gap special case
   bool skipEphemerisProp;

   /// The size of the propagation state vector
   Integer dim;
   Integer expandedDim;

   /// ID for the spacecraft epoch parameter
   Integer                      epochID;
   /// Starting epoch for the propagation
   std::vector<GmatEpoch>       baseEpoch;
   std::vector<GmatTime>        baseEpochGT;

   /// Epoch where a derivative zero was located
   GmatEpoch                    derivativeEpoch;

   /// Time elapsed during this Propagate
   RealArray                    elapsedTime;
   /// Start epoch for the step
   RealArray                    currEpoch;
   std::vector<GmatTime>        currEpochGT;

   /// The Propagators
   std::vector<Propagator*>     p;
   /// The ForceModels
   std::vector<ODEModel*>       fm;
   /// The Propagation State Managers
   std::vector<PropagationStateManager*>  psm;
   /// List of forces that can be turned on or off by other commands
   std::vector<PhysicalModel*>  *transientForces;
   /// Number of transient forces
   Integer                      transientCount;

   /// MaxStep settings for the propagators
   RealArray                    maxSteps;

   ///// The Mean-of-J2000 propagation state vector data
   //Real                         *j2kState;
   /// Data sent to the Publisher
   Real                         *pubdata;
   /// Size of pubdata array. It needs to keep track the size
   /// to prevent data overflow
   Integer                       pubdataSize;

   /// Stopping condition evaluation requires propagation; the satBuffer and
   /// formBuffer let us restore the Spacecraft and Formations to the state
   /// needed for the last step
   std::vector<Spacecraft *>    satBuffer;
   std::vector<FormationInterface *>     formBuffer;

   /// Flag used to turn off publishing during event location
   bool                 publishOnStep;

   bool                 PrepareToPropagate(bool preparePublisher = true);
   bool                 AssemblePropagators();
   bool                 Step(Real dt);
   virtual bool         TakeAStep(Real propStep = 0.0);

   virtual void         AddToBuffer(SpaceObject *so);
   virtual void         EmptyBuffer();
   virtual void         BufferSatelliteStates(bool fillingBuffer = true);

   virtual void         SetPropagationProperties(PropagationStateManager *psm);
   
   virtual void         SetNames(const std::string& name, 
                                 StringArray& owners, StringArray& elements);

   void                 AddTransientForce(StringArray *sats, ODEModel *p,
                              PropagationStateManager *propMan);
   void                 ClearTransientForces();

   virtual bool         IsSmootherPredicting();

   virtual void         UpdateCov();
   Integer              PrepareElementInfoToPublish(StringArray &ownerNames,
                                         StringArray &elementNames,
                                         Integer& covStartIndex,
                                         Integer& attStartIndex);

   bool                 PrepareElementDataToPublish(Real* publicData,
                                             Integer publishDataSize,
                                             StringArray &elementNames,
                                             Integer covStartIndex,
                                             Integer attStartIndex);

   void                 CalculateStateDotAtInitialEpoch();
};

#endif /* PropagationEnabledCommand_hpp */
