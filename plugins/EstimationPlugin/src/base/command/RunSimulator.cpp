//$Id: RunSimulator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             RunSimulator
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
// Created: 2009/06/21
//
/**
 * Mission Control Sequence Command that runs data simulation
 */
//------------------------------------------------------------------------------


#include "RunSimulator.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_SIMULATOR_EXECUTION
//#define DEBUG_EVENT_LOCATION
//#define DEBUG_STATE_BUFFERING
//#define DEBUG_EVENT_STATE
//#define DEBUG_INITIAL_STATE


//------------------------------------------------------------------------------
// RunSimulator()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunSimulator::RunSimulator() :
   RunSolver               ("RunSimulator"),
   theSimulator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL),
   includeSTMPropagation   (false)
{
   needToResetSTM = true;

   overridePropInit = true;
   hasPrecisionTime = true;
}


//------------------------------------------------------------------------------
// ~RunSimulator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunSimulator::~RunSimulator()
{
   if (theSimulator)
      delete theSimulator;

   eventList.clear();
}


//------------------------------------------------------------------------------
// RunSimulator(const RunSimulator & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunSimulator::RunSimulator(const RunSimulator & rs) :
   RunSolver               (rs),
   theSimulator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL),
   includeSTMPropagation   (rs.includeSTMPropagation)
{
   needToResetSTM = rs.needToResetSTM;

   overridePropInit = true;
}

//------------------------------------------------------------------------------
// RunSimulator& operator=(const RunSimulator & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunSimulator object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunSimulator & RunSimulator::operator=(const RunSimulator & rs)
{
   if (&rs != this)
   {
      theSimulator    = NULL;
      commandRunning  = false;
      commandComplete = false;
      
      needToResetSTM  = rs.needToResetSTM;

      currentEvent    = NULL;
      eventProcessComplete = false;
      eventMan        = NULL;
      bufferFilled    = false;
      overridePropInit = true;
      includeSTMPropagation = rs.includeSTMPropagation;

   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunSimulator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase* RunSimulator::Clone() const
{
   return new RunSimulator(*this);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 *
 * @param type Type of object requested.
 *
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string RunSimulator::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_SIMULATOR
            MessageInterface::ShowMessage
               ("Getting RunSimulator reference solver name\n");
         #endif
         return solverName;

      default:
         ;
   }

   return RunSolver::GetRefObjectName(type);
}



//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets names for referenced objects.
 *
 * @param type Type of the object.
 * @param name Name of the object.
 *
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool RunSimulator::SetRefObjectName(const UnsignedInt type,
                                     const std::string &name)
{
   if (type == Gmat::SOLVER)
   {
      solverName = name;
      return true;
   }

   return RunSolver::SetRefObjectName(type, name);
}



//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool RunSimulator::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
   // RunSimulator needs to know about Solver name only
   if (type != Gmat::SOLVER)
      return RunSolver::RenameRefObject(type, oldName, newName);

   if (solverName == oldName)
   {
      solverName = newName;
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode     Specifies the type of serialization requested.
 * @param prefix   Optional prefix appended to the object's name. (Used for
 *                 indentation)
 * @param useName  Name that replaces the object's name (Not yet used
 *                 in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& RunSimulator::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   generatingString = prefix + "RunSimulator " + solverName + ";";

   return RunSolver::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the simulator and associated measurement manager and
 * measurements for the simulation process.  Referenced objects are cloned or
 * set as needed in this method.
 *
 * The propagation subsystem is prepared in the base class components of the
 * command.  RunSimulator generates the PropSetup clones at this level, but
 * leaves the rest of the initialization process for the PropSetups in the base
 * class method, which is called from this method.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunSimulator::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start RunSimulator::Initialize()\n");
#endif
   bool retval = false;

   // First set the simulator object
   if (solverName == "")
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "simulator name is not specified.");

   // Clear the old clone if it was set
   if (theSimulator != NULL)
   {
      delete theSimulator;
      theSimulator = NULL;
   }

   GmatBase *simObj = FindObject(solverName);
   if (simObj == NULL)
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "simulator named " + solverName + " cannot be found.");

   if (!simObj->IsOfType("Simulator"))
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "object named " + solverName + " is not a simulator.");

   theSimulator = (Simulator*)(simObj->Clone());
   theSimulator->Initialize();

   // Set the observation data streams for the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   StringArray streamList = measman->GetStreamList();
   for (UnsignedInt ms = 0; ms < streamList.size(); ++ms)
   {
      GmatBase *obj = FindObject(streamList[ms]);
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::DATASTREAM))
         {
            DataFile *df = (DataFile*)obj;
            measman->SetStreamObject(df);
         }
      }
      else
         throw CommandException("Did not find the object named " +
               streamList[ms]);
   }

///// Check for generic approach here   
   // Set the ramp table data streams for the measurement manager
   streamList = measman->GetRampTableDataStreamList();
   for (UnsignedInt ms = 0; ms < streamList.size(); ++ms)
   {
      GmatBase *obj = FindObject(streamList[ms]);
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::DATASTREAM))
         {
            DataFile *df = (DataFile*)obj;
            measman->SetRampTableDataStreamObject(df);
         }
         else
            MessageInterface::ShowMessage(" Object '%s' is not Gmat::DATASTREAM\n", obj->GetName().c_str());
      }
      else
      {
         throw CommandException("Error: Did not find the object named " + 
               streamList[ms]);
      }
   }

   // Find the event manager and store its pointer
   if (triggerManagers == NULL)
      throw CommandException("The Event Manager pointer was not set on the "
            "RunSimulator command");

   for (UnsignedInt i = 0; i < triggerManagers->size(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunSimulator has an TriggerManager of "
               "type %s, id %d\n",
               (*triggerManagers)[i]->GetTriggerTypeString().c_str(),
               (*triggerManagers)[i]->GetTriggerType());
      #endif
      if ((*triggerManagers)[i]->GetTriggerType() == Gmat::EVENT)
      {
         eventMan = (EventManager*)(*triggerManagers)[i];
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("RunSimulator has an EventManager of "
                  "type %s\n", eventMan->GetTriggerTypeString().c_str());
         #endif
      }
   }
   if (eventMan == NULL)
      throw CommandException("The EventManager pointer was not set on the "
            "RunSimulator command");

   // Next comes the propagators
   // Clear old ones
   if (propagators.size() > 0)
   {
      for (std::vector<PropSetup*>::iterator pp = propagators.begin();
            pp != propagators.end(); ++pp)
      {
         delete (*pp);
      }
      propagators.clear();
      p.clear();
      fm.clear();
   }

   propObjectNames.clear();
   std::map<PropSetup*, StringArray> satList;
   std::map<std::string,PropSetup*> knownProps;

   // Setup new ones
   StringArray scraft = measman->GetParticipantList();
   for (UnsignedInt i = 0; i < scraft.size(); ++i)
   {
      GmatBase *sc = FindObject(scraft[i]);
      if ((sc) && (sc->IsOfType("Spacecraft")))
      {

         ((Spacecraft*)(sc))->SetRunningCommandFlag(1);    // init value 1 for running simulation command 

         PropSetup *prop = theSimulator->GetPropagator(scraft[i]);

         if (prop == nullptr)
            throw CommandException("Cannot initialize RunSimulator command; the "
                  "propagator pointer requested from the Simulator " +
                  theSimulator->GetName() + " for the spacecraft " + scraft[i] +
                  " is NULL.");

         std::string propName = prop->GetName();
         PropSetup *ps = nullptr;

         if (knownProps.find(propName) == knownProps.end())
         {
            ps = (PropSetup*)prop->Clone();
            ps->SetPrecisionTimeFlag(true);
            propagators.push_back(ps);

            StringArray satNames;
            satNames.push_back(scraft[i]);
            satList[ps] = satNames;
            knownProps[propName] = ps;

            p.push_back(ps->GetPropagator());
            fm.push_back(ps->GetODEModel());
            eventMan->SetObject(ps);

            retval = true;
         }
         else
         {
            // One Ephem Prop per spacecraft
            if (prop->GetPropagator()->IsOfType("EphemerisPropagator"))
            {
               ps = (PropSetup*)prop->Clone();
               ps->SetPrecisionTimeFlag(true);
               propagators.push_back(ps);

               StringArray satNames;
               satNames.push_back(scraft[i]);
               satList[ps] = satNames;
               knownProps[propName] = ps;

               Propagator *prop = ps->GetPropagator();
               p.push_back(prop);
               fm.push_back(ps->GetODEModel());
               eventMan->SetObject(ps);
               prop->TakeAction("RunPastStartEnd");

               retval = true;
            }
            else
            {
               ps = knownProps[propName];
               StringArray satNames = satList[ps];
               if (find(satNames.begin(), satNames.end(), scraft[i]) == satNames.end())
                  (satList[ps]).push_back(scraft[i]);
            }
         }

         if (ps->GetPropagator()->UsesODEModel())
            sc->TakeAction("UseSTM");
         else
            sc->TakeAction("SkipSTM");
      }
   }

   for (UnsignedInt i = 0; i < propagators.size(); ++i)
   {
      propObjectNames.push_back(satList[propagators[i]]);
      if (propagators[i]->GetPropagator()->IsOfType("EphemerisPropagator"))
         propagators[i]->GetPropagator()->SetStringParameter("StartEpoch", "FromSpacecraft");
   }

   // Now we can initialize the propagation subsystem by calling up the
   // inheritance tree.
   if (retval)
      retval = RunSolver::Initialize();

   #ifdef DEBUG_INITIALIZATION
      if (retval == false)
         MessageInterface::ShowMessage("RunSimulator command failed to "
               "initialize; RunSolver::Initialize() call failed.\n");
      MessageInterface::ShowMessage("Exit RunSimulator::Initialize()\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void SetPropagationProperties(PropagationStateManager *psm)
//------------------------------------------------------------------------------
/**
 * An interface used to set specific additional propagation properties as
 * needed.
 *
 * @param psm A PropagationStateManager that controls the propagation state
 *            vector
 */
//------------------------------------------------------------------------------
void RunSimulator::SetPropagationProperties(PropagationStateManager *psm)
{
   ObjectArray pObjects;
   psm->GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);

   for (ObjectArray::iterator p = pObjects.begin(); p != pObjects.end(); ++p)
   {
      if ((*p)->IsOfType(Gmat::SPACEOBJECT))
      {
         // Always include the STM so that simulation and estimation steps are
         // (nearly) coincident

         psm->SetProperty("STM", *p);
      }
   }
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Performs the command side processing for the Simulation
 *
 * This method calls the Simulator to determine the state of the Simulation
 * state machine and responds to that state as needed.  Typical command side
 * responses are to propagate as needed, to clean up memory, or to reset flags
 * based on the state machine.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunSimulator::Execute()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
     MessageInterface::ShowMessage("\nEntered RunSimulator::Execute()\n");
      MessageInterface::ShowMessage("\n\nThe \"%s\" command is running...\n",
            GetTypeName().c_str());
   #endif

   // Reset the command if called after it has completed execution
   // todo: Debug this piece; reentrance in a For loop doesn't work yet
//   if (commandComplete)
//      TakeAction("Reset");


   // Here we should check to see if the command is currently propagating and
   // finish that first...

   // Respond to the state in the state machine
   Solver::SolverState state = theSimulator->GetState();

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("\nSimulator state is %d\n", state);
   #endif
   switch (state)
   {
      case Solver::INITIALIZING:
      {
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): INITIALIZING state\n");
         #endif
         PrepareToSimulate();
         {
            // Register to publishing data
            covStartIndex = 0;
            expandedDim = PrepareElementInfoToPublish(pubdataOwners, pubdataElements, covStartIndex, attStartIndex);
            publisher->UnregisterPublishedData(this);
            streamID = publisher->RegisterPublishedData(this, streamID, pubdataOwners, pubdataElements);
            pubdataSize = expandedDim;
            if (pubdata)
            {
               delete[] pubdata;
               //pubdataSize = 0;        // It resets size to 0. This cause incorrect size. 
            }
            pubdata = new Real[pubdataSize];

            // Publish data
            PublishState();
         }
         break;
      }
      case Solver::PROPAGATING:
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): PROPAGATING state\n");
         #endif
         Propagate();
         break;

      case Solver::CALCULATING:
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): CALCULATING state\n");
         #endif
         Calculate();
         break;

      case Solver::LOCATING:
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): LOCATING state\n");
         #endif
         LocateEvent();
         break;

      case Solver::SIMULATING:
      {
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): SIMULATING state\n");
         #endif
         Simulate();

         // Publish data
         PublishState();
         break;
      }
      case Solver::FINISHED:
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("RunSimulator::Execute(): FINSIHED state\n");
         #endif
         Finalize();
         break;

      default:
         throw CommandException("Unknown state "
               " encountered in the RunSimulator command");
   }

   state = theSimulator->AdvanceState();

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::Execute()\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Completes processing so that subsequent commands can be run.
 */
//------------------------------------------------------------------------------
void RunSimulator::RunComplete()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::RunComplete()\n");
   #endif
   commandRunning = false;

   RunSolver::RunComplete();

   if (eventMan)
   {
      eventMan->CleanUp();
      eventMan = NULL;
   }

   if (theSimulator)
   {
      delete theSimulator;
      theSimulator = NULL;
   }



   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::RunComplete()\n");
   #endif
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Performs actions at prompting from higher level structures
 *
 * @param action The action that needs to be taken
 * @param actionData Optional additional data the action needs
 *
 * @return true if an action was taken, false if not
 */
//------------------------------------------------------------------------------
bool RunSimulator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   if (action == "Reset")
   {
      theSimulator->TakeAction("Reset");
      commandRunning = false;
      commandComplete = false;
      return true;
   }

   return RunSolver::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Retrieves the pointer to the next command that the Sandbox needs to run
 *
 * This method returns a pointer to the current RunSimulator command while the
 * simulation state machine is running.  It returns the next pointer after the
 * simulation has finished execution.
 *
 * @return The next comamnd that should Execute()
 */
//------------------------------------------------------------------------------
GmatCommand* RunSimulator::GetNext()
{
   if (commandRunning)
      return this;
   return next;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool RunSimulator::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void RunSimulator::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("Spacecraft"))
      return;
   if (obj->IsOfType("Parameter"))
      return;
   throw CommandException("To do: implement RunSimulator::UpdateClonedObject "
         "for " + obj->GetTypeName() + " objects");
}



//------------------------------------------------------------------------------
// Methods triggered by the finite state machine
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void PrepareToSimulate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 *
 * Performs the final stages of initialization that need to be performed prior
 * to running the simulation.  This includes the final ODEModel preparation and
 * the setting for the flags that indicate that a simulation is in process.
 */
//------------------------------------------------------------------------------
void RunSimulator::PrepareToSimulate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::PrepareToSimulate()\n");
   #endif

   // Prep the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   
   // Should RunEstimator do this too?  Or is it superfluous?
   std::vector<PropSetup*> *thePropagators = theSimulator->GetPropagators();
   for (UnsignedInt i = 0; i < thePropagators->size(); ++i)
      thePropagators->at(i)->SetPrecisionTimeFlag(true);

   if (measman->PrepareForProcessing(true) == false)
      throw CommandException(
            "Measurement Manager was unable to prepare for processing");

   //Ensures all multisat epochs match. Ensures ephem props are on the same epoch
   bool currentEpochSet = false;
   GmatTime ce;
   // For Multisat, check that all epochs are the same
   if (sats.size() > 0)
   {
      for (int cyc = 0; cyc < sats.size() - 1; ++cyc)
      {
         if (!currentEpochSet)
         {
            if (!theSimulator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName()))
            {
               ce = ((Spacecraft *)sats[cyc])->GetEpoch();
               theSimulator->UpdateCurrentEpoch(ce);
               currentEpochSet = true;
            }
         }
         for (int cyc2 = cyc + 1; cyc2 < sats.size(); ++cyc2)
         {
            if (!currentEpochSet)
            {
               if (!theSimulator->DoesSatUseEphemerisPropagator(sats[cyc2]->GetName()))
               {
                  ce = ((Spacecraft *)sats[cyc2])->GetEpoch();
                  theSimulator->UpdateCurrentEpoch(ce);
                  currentEpochSet = true;
               }
            }
            if (((Spacecraft *)sats[cyc2])->GetEpoch() != ((Spacecraft *)sats[cyc])->GetEpoch())
            {
               //Will not throw if either sat is from an ephemeris file. 
               if (!(theSimulator->DoesSatUseEphemerisPropagator(sats[cyc2]->GetName())) && !(theSimulator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName())))
                  throw CommandException("All participating Spacecraft must be at the same epoch prior to simulation. Use the Propagate command to advance all Spacecraft to a common epoch before running the simulator or estimator.");
            }
         }
      }
      if (currentEpochSet == false)
      {
         ce = theSimulator->GetSimulationStartEpochGT();
         theSimulator->UpdateCurrentEpoch(ce);
         currentEpochSet = true;
      }

      for (int cyc = 0; cyc < sats.size(); ++cyc)
      {
         if (theSimulator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName()))
         {
            ((Spacecraft *)sats[cyc])->SetEpochGT(ce);
         }
      }

      for (UnsignedInt n = 0U; n < propagators.size(); ++n)
      {
         if (propagators[n]->GetPropagator()->IsOfType("EphemerisPropagator"))
         {
            propagators[n]->GetPropagator()->SetStringParameter("StartEpoch", "FromSpacecraft");
         }
      }
   }

   // Prepare to propagate without preparation for publisher
   // (false: to tell without prepareation for publisher)
   PrepareToPropagate(false);  // ?? Test return value here?

   // measman->LoadRampTables();                     // This command is moved to Simulator::CompleteInitialization()

   // Warn that the attitude is not updated at each propagation intermediate step [GMT-4398]
   // FIXME: Fix this when PropagationEnabledCommand is refactored
   // Check if the attitude affects the dynamics of any force model
   bool attAffectDyn = false;
   for (UnsignedInt n = 0U; n < propagators.size(); ++n)
   {
      ODEModel *odem = propagators[n]->GetODEModel();
      if (odem)
      {
         for (UnsignedInt ii = 0U; ii < odem->GetNumForces(); ++ii)
         {
            PhysicalModel* f = odem->GetForce(ii);

            if (f->AttitudeAffectsDynamics())
            {
               attAffectDyn = true;
               break;
            }
         }
      }

      if (attAffectDyn)
         break;
   }

   

   

   // If the attitude of the Spacecraft affects the force model,
   // check if the Spacecraft uses an ObjectReferencedAxes
   bool objRefAxes = false;
   if (attAffectDyn)
   {
      for (std::vector<GmatBase *>::const_iterator obj = sats.begin();
         obj != sats.end(); ++obj)
      {
         if ((*obj)->IsOfType("Spacecraft"))
         {
            GmatBase* att = (*obj)->GetRefObject(Gmat::ATTITUDE, "");

            if (att->IsOfType("NadirPointing"))
            {
               objRefAxes = true;
            }
            else
            {
               std::string refCSName = att->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
               CoordinateSystem* cs = (CoordinateSystem*) att->GetRefObject(Gmat::COORDINATE_SYSTEM, refCSName);
               AxisSystem* ax = cs->GetAxisSystem();

               if (ax->IsOfType("ObjectReferencedAxes"))
                  objRefAxes = true;
            }
         }
      }
   }

   if (objRefAxes)
      MessageInterface::ShowMessage("Warning: A Spacecraft in the Simulator is using NadirPointing or "
         "an ObjectReferenced axes for its attitude with a force model that has dynamics affected by the attitude. "
         "The attitude is not updated at intermediate steps in the integrator, so the result may be inaccurate.\n");
   // End PropagationEnabledCommand FIXME

   // TBD
   theSimulator->GetMeasurementManager()->SetTransientForces(transientForces);
   commandRunning  = true;
   commandComplete = false;

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::PrepareToSimulate()\n");
   #endif

   #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("Object states at close of PrepareToSimulate:\n");
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
      {
         PropObjectArray *poa = propObjects[i];
         for (UnsignedInt j = 0; j < poa->size(); ++j)
         {
            MessageInterface::ShowMessage("   %s:\n", poa->at(j)->GetName().c_str());
            if (poa->at(j)->IsOfType(Gmat::SPACEOBJECT))
            {
               MessageInterface::ShowMessage("      Epoch: [%s]\n",
                     poa->at(j)->GetStringParameter("Epoch").c_str());
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("X"),
                     poa->at(j)->GetRealParameter("Y"),
                     poa->at(j)->GetRealParameter("Z"));
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("VX"),
                     poa->at(j)->GetRealParameter("VY"),
                     poa->at(j)->GetRealParameter("VZ"));
            }
            else
               MessageInterface::ShowMessage("      Not a SpaceObject\n");
         }
      }
   #endif
}


//------------------------------------------------------------------------------
// void Propagate()
//------------------------------------------------------------------------------
/**
 * Responds to the PROPAGATING state of the finite state machine.
 *
 * Propagation from the current epoch to the next simulation epoch is performed
 * in this method.
 */
//------------------------------------------------------------------------------
void RunSimulator::Propagate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::Propagate()\n");
   #endif
   
   Real dt = theSimulator->GetTimeStep(currEpochGT[0]);
      

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("dt = %.15lf\n", dt);
   #endif

   #ifdef DEBUG_EVENT_STATE
      Integer dim = fm[0]->GetDimension();
      Real *odeState = fm[0]->GetState();

      MessageInterface::ShowMessage("State before prop\n");
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12lf", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   /// Updated for Multi-spacecraft prop
   Real maxStep = fabs(maxSteps[0]);
   for (Integer chker = 0; chker < maxSteps.size(); chker++)
   {
      if (maxStep < fabs(maxSteps[chker]))
      {
         maxStep = fabs(maxSteps[chker]);
      }
   }
   if (fabs(dt) > maxStep)
      dt = (dt > 0.0 ? maxStep : -maxStep);

   Step(dt);
   bufferFilled = false;

   theSimulator->UpdateCurrentEpoch(currEpochGT[0]);

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      odeState = fm[0]->GetState();

      MessageInterface::ShowMessage("State after prop:\n  ");
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12lf", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::Propagate()\n");
   #endif
}


//------------------------------------------------------------------------------
// void Calculate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Calculate()
{
#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Entered RunSimulator::Calculate()\n");
#endif

   bufferFilled = false;

#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Exit RunSimulator::Calculate()\n");
#endif
}


//------------------------------------------------------------------------------
// void LocateEvent()
//------------------------------------------------------------------------------
/**
 * Responds to the LOCATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::LocateEvent()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::LocateEvent()\n   "
            "bufferFilled is %s\n", (bufferFilled ? "true" : "false"));
   #endif

//   bool newEvent = false;
   EventStatus status;

   // First time through, buffer the objects that get propagated
   if (!bufferFilled)
   {
      dt = fm[0]->GetTime();
      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State at event location start:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  dt = %.12lf\n  ", dt);
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
      BufferSatelliteStates(true);
      eventMan->ClearObject(NULL);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("   Buffer has %d objects; contents:\n",
               satBuffer.size());

         for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
              i != satBuffer.end(); ++i)
         {
            MessageInterface::ShowMessage(
                  "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
                  (*i)->GetRealParameter("A1Epoch"));
            MessageInterface::ShowMessage(
                  "   Position is [%.12lf  %.12lf  %.12lf]\n",
                  (*i)->GetRealParameter("X"), (*i)->GetRealParameter("Y"),
                  (*i)->GetRealParameter("Z"));
         }
      #endif

      bufferFilled = true;
      eventList.clear();
      eventList = theSimulator->GetRefObjectArray(Gmat::EVENT);

      // Set status of the events to ITERATING
      for (UnsignedInt i = 0; i < eventList.size(); ++i)
      {
         ((Event*)eventList[i])->CheckStatus(ITERATING);
         ((Event*)eventList[i])->Initialize();
      }

      if (eventList.size() == 0)
      {
         currentEvent = NULL;
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("   No events were found\n");
         #endif
         return;
      }
      else
      {
         currentEvent = ((Event*)eventList[0]);
//         newEvent = true;
         eventIndex = 0;
         // Will need to be updated when multiple propagators are enabled:
         for (int i = 0; i < propagators.size(); i++)
         {
            eventMan->SetObject(propagators[i]);
         }

         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);

         // Reset the state data to the starting states
         BufferSatelliteStates(false);
         for (int i = 0; i < propagators.size(); i++)
         {
            propagators[i]->GetODEModel()->UpdateFromSpaceObject();
         }

         for (int i = 0; i < fm.size(); i++)
         {
            fm[i]->SetTime(dt);
         }

         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif
      }

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Setting flag so state data will "
               "reset\n");
      #endif

      eventProcessComplete = true;
   }

   status = currentEvent->CheckStatus();

   if (status == LOCATED)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("Located the event %s (index %d)\n",
               currentEvent->GetTypeName().c_str(), eventIndex);
      #endif

      ++eventIndex;
      if (eventIndex < eventList.size())
      {
         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif

         // Prepare to work with the next active event
         eventMan->ClearObject(currentEvent);
         currentEvent = (Event*)eventList[eventIndex];
//         newEvent = true;

         // Eventually, set propagators associated with specific events here
         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);
         // Reset the state data to the starting states
         BufferSatelliteStates(false);
         for (int i = 0; i < propagators.size(); i++)
         {
            propagators[i]->GetODEModel()->UpdateFromSpaceObject();
         }

         for (int i = 0; i < fm.size(); i++)
         {
            fm[i]->SetTime(dt);
         }

         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("   Event %d, of type %s\n", j,
                  currentEvent->GetTypeName().c_str());
         #endif
      }
      else
      {
         eventMan->ClearObject(currentEvent);
         currentEvent = NULL;
         #ifdef DEBUG_EVENT_STATE
            Integer dim = fm[0]->GetDimension();
            Real *odeState = fm[0]->GetState();

            MessageInterface::ShowMessage("State at event location end:\n  ");
            fm[0]->ReportEpochData();
            MessageInterface::ShowMessage("  ");
            for (Integer i = 0; i < dim; ++i)
               MessageInterface::ShowMessage("   %.12lf", odeState[i]);
            MessageInterface::ShowMessage("\n");
         #endif
      }
   }

   if (currentEvent != NULL)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("   Finding root for the event\n");
      #endif
      eventMan->FindRoot(0);
   }

   // Reset the state data to the starting states
   BufferSatelliteStates(false);
   for (int i = 0; i < propagators.size(); i++)
   {
      propagators[i]->GetODEModel()->UpdateFromSpaceObject();
   }

   for (int i = 0; i < fm.size(); i++)
   {
      fm[i]->SetTime(dt);
   }

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::LocateEvent()\n   ");
   #endif

}


//------------------------------------------------------------------------------
// void Simulate()
//------------------------------------------------------------------------------
/**
 * Responds to the SIMULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Simulate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::Simulate()\n");
   #endif

   // If events were processed, we need to reset the state data
   if (eventProcessComplete == true)
   {
      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Resetting state data after event "
               "location\n");
      #endif

      // Reset the state data to the starting states
      BufferSatelliteStates(false);
      propagators[0]->GetODEModel()->UpdateFromSpaceObject();
      eventProcessComplete = false;

      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State in the Simulate method:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  ");
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }

   #ifdef DEBUG_STATE_BUFFERING
      MessageInterface::ShowMessage("State changed to Simulate\n"
            "   Buffer has %d objects; contents:\n", satBuffer.size());

      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
           i != satBuffer.end(); ++i)
      {
         MessageInterface::ShowMessage(
               "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
               (*i)->GetRealParameter("A1Epoch"));
         MessageInterface::ShowMessage(
               "   Position is [%.12lf  %.12lf  %.12lf]\n",
               (*i)->GetRealParameter("X"), (*i)->GetRealParameter("Y"),
               (*i)->GetRealParameter("Z"));
      }
   #endif

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Exit RunSimulator::Simulate()\n");
   #endif

}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Responds to the FINALIZING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Finalize()
{
#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Entered RunSimulator::Finalize()\n");
#endif
   // Do cleanup here

   // Finalize the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   if (measman->ProcessingComplete() == false)
      MessageInterface::ShowMessage(
            "Measurement Manager reported a problem completing processing\n");

   commandComplete = true;
   commandRunning  = false;

   BuildCommandSummary(true);

#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Exit RunSimulator::Finalize()\n");
#endif
}



//------------------------------------------------------------------------------
// void PublishState()
//------------------------------------------------------------------------------
/**
 * Publishes the state to the publisher if ready.
 */
 //------------------------------------------------------------------------------
void RunSimulator::PublishState()
{
   // Clear pubdata
   for (Integer idx = 0; idx < pubdataSize; ++idx)
      pubdata[idx] = GmatMathConstants::QUIET_NAN;

   // Publish epoch
   if (HasPrecisionTime())
      pubdata[0] = currEpochGT[0].GetMjd();
   else
      pubdata[0] = currEpoch[0];

   // Publish all elements in estimation state manager
   PrepareElementDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, attStartIndex);
   publisher->Publish(this, streamID, pubdata, expandedDim, direction);
}       

