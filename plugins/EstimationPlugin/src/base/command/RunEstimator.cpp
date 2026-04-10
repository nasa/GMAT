//$Id: RunEstimator.cpp 1398 2011-04-21 20:39:37Z  $
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
 * Implementation of the Mission Control Sequence command that drives estimation
 */
//------------------------------------------------------------------------------


#include "RunEstimator.hpp"
#include "EstimatorException.hpp"
#include "MessageInterface.hpp"
#include "ODEModel.hpp"
#include "Plate.hpp"
#include <map>

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_STATE
//#define DEBUG_STATE_RESETS
//#define DEBUG_EVENT_STATE
//#define DEBUG_LOAD_SOLVEFORS
//#define DEBUG_INITIAL_STATE
//#define DEBUG_STEP_CONTROL
//#define DEBUG_PUBLISH_STATE_DATA


//------------------------------------------------------------------------------
// RunEstimator()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator(const std::string &typeStr) :
   RunSolver               (typeStr),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (0.0),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   needToResetSTM		= true;
   overridePropInit		= true;

   // modify publishOnStep from "true" (as defined in PropagationEanbledCommand) to "false".
   publishOnStep		= false;

   delayInitialization  = true;
   hasPrecisionTime		= true;
   solveMode			= "Solve";
}


//------------------------------------------------------------------------------
// ~RunEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunEstimator::~RunEstimator()
{
   if (theEstimator)
      delete theEstimator;

   // clean up ObjectArray eventList;
   // eventList contains a list of pointers which point to objects in MeasurementManager. The task to delete those objects have to be handled by MeasurementManager
   eventList.clear();

   // clean up Event *currentEvent;
   currentEvent = NULL;                  // This class does not create Event object, so it does not to delete it.
   // clean up EventManager *eventMan;
   eventMan = NULL;                      // This class does not create EventManager object, so it does not to delete it.

}


//------------------------------------------------------------------------------
// RunEstimator(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator(const RunEstimator & rs) :
   RunSolver               (rs),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (rs.estimationOffset),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   overridePropInit     = true;
   
   // It would be publishOnStep = rs.publishOnStep.
   // This was done in PropagationEnableCommand copy constructor.
   //publishOnStep        = false;

   delayInitialization  = true;
   solveMode            = rs.solveMode;
}

//------------------------------------------------------------------------------
// RunEstimator& operator=(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunEstimator object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunEstimator& RunEstimator::operator=(const RunEstimator & rs)
{
   if (&rs != this)
   {
      RunSolver::operator=(rs);

      if (theEstimator)
      {
         delete theEstimator;
         theEstimator = NULL;
      }

      commandRunning       = false;
      commandComplete      = false;
      overridePropInit     = true;

      // It would be publishOnStep = rs.publishOnStep.
      // This was done in PropagationEnableCommand copy constructor.
      //publishOnStep        = false;

      propPrepared         = false;
      estimationOffset     = rs.estimationOffset;
      bufferFilled         = false;
      currentEvent         = NULL;
      eventProcessComplete = false;
	  solveMode		       = rs.solveMode;

      if (eventMan)
      {
         eventMan->CleanUp();
         // use delete eventMan causes a crash
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunEstimator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase *RunEstimator::Clone() const
{
   return new RunEstimator(*this);
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
std::string RunEstimator::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_Estimator
            MessageInterface::ShowMessage
               ("Getting RunEstimator reference solver name\n");
         #endif
         return solverName;

      default:
         break;
   }

   return RunSolver::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name
 *
 * @param type The object's type
 * @param name The object's name
 *
 * @return The pointer to the associated object
 */
 //------------------------------------------------------------------------------
GmatBase* RunEstimator::GetRefObject(const UnsignedInt type,
   const std::string & name)
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_Estimator
            MessageInterface::ShowMessage
               ("Getting RunEstimator reference solver\n");
         #endif
         return theEstimator;

      default:
         break;
   }

   return RunSolver::GetRefObject(type, name);
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
bool RunEstimator::SetRefObjectName(const UnsignedInt type,
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
bool RunEstimator::RenameRefObject(const UnsignedInt type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   // RunEstimator needs to know about Solver name only
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
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//        const std::string &prefix, const std::string &useName)
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
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name. (Used for
 *                indentation)
 * @param useName Name that replaces the object's name (Not yet used
 *                in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& RunEstimator::GetGeneratingString(Gmat::WriteMode mode,
      const std::string &prefix, const std::string &useName)
{
   std::string solverModeStr = "";
   if (solveMode != "Solve")
   {
      solverModeStr = " {SolveMode = " + solveMode + "}";
   }
   generatingString = prefix + "RunEstimator " + solverName + solverModeStr + ";";

   return RunSolver::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the Estimator and associated measurement manager and
 * measurements for the simulation process.  Referenced objects are cloned or
 * set as needed in this method.
 *
 * The propagation subsystem is prepared in the base class components of the
 * command.  RunEstimator generaqtes teh PropSetup clones at this level, but
 * leaves the rest of the initialization process for the PropSetups in the base
 * class method, which is called from this method.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start RunEstimator::Initialize()\n");
#endif

   // This step is used to delay to initialize until it runs Execute() function.
   // It is needed due to no observation data available before simulation. As a result,
   // no tracking configurations are auto generated for estimation. After simulation 
   // step is completed, based on simulation data, GMAT generates tracking 
   // configuration automatically for estimation step. 
   if (delayInitialization)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Exit RunEstimator::Initialize():   delay initialization to the next time\n"); 
      #endif
      return true;
   }

   // if it is initialized, does not need to do it again
   if (isInitialized)
      return true;

   bool retval = false;

   // First set the Estimator object
   if (solverName == "")
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator name is not specified.");

   // Clear the old clone if it was set
   if (theEstimator != NULL)
   {
      delete theEstimator;
      theEstimator = NULL;
   }

   GmatBase *estObj = FindObject(solverName);
   if (estObj == NULL)
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator named " + solverName + " cannot be found.");

   if (!estObj->IsOfType("Estimator"))
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "object named " + solverName + " is not a Estimator.");

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1: create an Estimator and set value to the Estimator\n"); 
   #endif
   theEstimator = (Estimator*)(estObj->Clone());
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1.1: reset delay flag\n"); 
   #endif
   theEstimator->SetDelayInitialization(false);
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 1.2: initialize the Estimator\n"); 
   #endif
   theEstimator->Initialize();

   theEstimator->TakeAction("ResetInstanceCount");
   theEstimator->TakeAction("IncrementInstanceCount"); 
   estObj->TakeAction("ResetInstanceCount");                // does it need to do it???
   estObj->TakeAction("IncrementInstanceCount");            // does it need to do it???

   
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("RunEstimator::Initialize():   step 2: set DATASTREAM objects to measurement manager\n");
#endif
   // Set the observation data streams for the measurement manager
   MeasurementManager *measman = theEstimator->GetMeasurementManager();
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

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Initialize():   step 3: set ramped table to measurement manager\n");
   #endif
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
            MessageInterface::ShowMessage("Object '%s' is not Gmat::DATASTREAM\n", obj->GetName().c_str());
      }
      else
      {
         throw CommandException("Error: Did not find the object named " + 
               streamList[ms]);
      }
   }

   if (solveMode != "Solve")
      theEstimator->SetStringParameter("SolverMode", solveMode);

   // All the code of initialize was moved to PreExcecution() function

   retval = true;
   isInitialized = retval;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Exit RunEstimator::Initialize()\n");
#endif
   return retval;
}



//-----------------------------------------------------------------------------
// void RunEstimator::DifferenceOfCentralBodiesWarningMessage(GmatBase *obj)
//-----------------------------------------------------------------------------
/**
* This function displays warning message about the using spacecraft's state in 
* solve-for when the central bodies of spacecraft and force model are different.
*
* @param obj   spacecraft object which may contain "CartesianState" or 
*              "KeplerianState" in the SolveFors parameter
*/
//-----------------------------------------------------------------------------
void RunEstimator::DifferenceOfCentralBodiesWarningMessage(GmatBase *obj)
{
   if (obj->IsOfType(Gmat::SPACECRAFT))
   {
      Spacecraft *sc = (Spacecraft*)obj;
      std::string scName = sc->GetName();

      // Find spacecraft's state in solve-for list
      StringArray sfList = sc->GetStringArrayParameter("SolveFors");
      bool found = false;
      for (Integer idx = 0; idx < sfList.size(); ++idx)
      {
         if ((sfList[idx] == "CartesianState") || (sfList[idx] == "KeplerianState"))
         {
            found = true;
            break;
         }
      }
      
      bool diff = false;
      if (found)
      {
         // Get spacecraft central body
         std::string scCoordName = sc->GetStringParameter("CoordinateSystem");
         std::string scCBName = "";
         GmatBase *coordsys = FindObject(scCoordName);
         // Compare with the name of spacecraft central body
         if (coordsys && coordsys->IsOfType(Gmat::COORDINATE_SYSTEM))
         {
            scCBName = ((CoordinateSystem*)coordsys)->GetOriginName();
         }

         // Display warning message when the central bodies of spacecraft and force model are different
         bool foundObj = false;
         for (Integer idx1 = 0; idx1 < propObjectNames.size(); ++idx1)
         {
            for (Integer idx2 = 0; idx2 < propObjectNames[idx1].size(); ++idx2)
            {
               if (propObjectNames[idx1].at(idx2) == scName)
               {
                  // found spacecraft name in the list
                  foundObj = true;

                  if (fm[idx1] != NULL)
                  {
                     // Get name of the force cetral body
                     std::string forceCBName = fm[idx1]->GetForceOrigin()->GetName();

                     // if both central bodies of spacecraft and force model are different then generate an error message
                     if (forceCBName != scCBName)
                     {
                        std::stringstream ss;
                        ss << "Warning: Attempting to solve for the spacecraft state when the central body ("
                           << scCBName << ") of the Spacecraft ("
                           << scName << ") differs from the central body ("
                           << forceCBName << ") of the ForceModel ("
                           << fm[idx1]->GetName() 
                           << ") can result in numerical difficulties, including solution divergence.\n";
                        MessageInterface::ShowMessage(ss.str());
                     }
                  }
                  break;
               }
            }

            if (foundObj)
               break;
         }

      }
   }
}


//--------------------------------------------------------------------------------
// void LoadSolveForsToESM()
//--------------------------------------------------------------------------------
/**
* This function is used to load all solve-for variables and store them into 
* EstimationStateManager object.
*/
//--------------------------------------------------------------------------------
void RunEstimator::LoadSolveForsToESM()
{
   #ifdef DEBUG_LOAD_SOLVEFORS
      MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS()  enter\n");
   #endif

   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

   // Set solve-for for all participants used in this estimator only. Solve-fors for participants in other estimator and similator are not set to ESM 
   StringArray names = theEstimator->GetMeasurementManager()->GetParticipantList();

   #ifdef DEBUG_LOAD_SOLVEFORS
      for (UnsignedInt j = 0; j < names.size(); ++j)
         MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS() "
               "Participants   %s\n", names[j].c_str());
   #endif

   ObjectMap objectmap = GetConfiguredObjectMap();

   for (UnsignedInt i = 0; i < names.size(); ++i)
   {
      GmatBase *obj = FindObject(names[i]);
      if (obj != NULL)
      {
         // Display warning message of using spacecraft's state to solve-for when the
         // central body of spacecraft and force model are different.
         DifferenceOfCentralBodiesWarningMessage(obj);

         // Set solve-for parameters for the object
         esm->SetProperty(obj);
      }
   }

   // Need to test extensively with this change
//   for (ObjectMap::iterator i = objectmap.begin(); i != objectmap.end(); ++i)
//   {
//      if (find(names.begin(), names.end(), (*i).first) != names.end())
//      {
//         #ifdef DEBUG_LOAD_SOLVEFORS
//            MessageInterface::ShowMessage("   Loading %s solve-fors\n",
//                  i->first.c_str());
//         #endif
//         // Set solve-for for all participants. If participant does not have solve-for defined in it,
//         // esm->SetProperty rejects the set request and return false otherwise return true
//         esm->SetProperty(i->second);
//      }
//   }

   // Scan the force model for solve-for parameters
   if (fm.size() > 0)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         #ifdef DEBUG_LOAD_SOLVEFORS
         MessageInterface::ShowMessage("   Scanning the force model fm[%d] <%p>\n", i, fm[i]);
         if (fm[i])
         {
            MessageInterface::ShowMessage("       the force model %s <%p> for "
               "solve-fors\n", fm[i]->GetName().c_str(), fm[i]);
         }
         #endif

         if (fm[i])
         {
            StringArray solforNames = fm[i]->GetSolveForList();
            if (solforNames.size() > 0)
            {
               #ifdef DEBUG_LOAD_SOLVEFORS
                  MessageInterface::ShowMessage("   %s has %d solve-fors:\n",
                         fm[i]->GetName().c_str(), solforNames.size());
               #endif

               for (UnsignedInt j = 0; j < solforNames.size(); ++j)
               {
                  #ifdef DEBUG_LOAD_SOLVEFORS
                     MessageInterface::ShowMessage("      %s\n",
                           solforNames[j].c_str());
                  #endif
                  esm->SetProperty(solforNames[j], fm[i]);
               }
            }
         }
      }
   }

   #ifdef DEBUG_LOAD_SOLVEFORS
      MessageInterface::ShowMessage("RunEstimator::LoadSolveForsToEMS()  exit\n");
   #endif
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
void RunEstimator::SetPropagationProperties(PropagationStateManager *psm)
{
   ObjectArray pObjects;
   psm->GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);

   for (ObjectArray::iterator p = pObjects.begin(); p != pObjects.end(); ++p)
   {
      if ((*p)->IsOfType(Gmat::SPACEOBJECT))
      {
         psm->SetProperty("STM", *p);

         // Add properties for EstimatedParameters
         ObjectArray estParamObjs = (*p)->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));
         for (ObjectArray::iterator ep = estParamObjs.begin(); ep != estParamObjs.end(); ++ep)
         {
            if ((*ep)->GetStringParameter("SolveFor") == "Cd")
               psm->SetProperty("Cd_Epsilon", *p);
            else if ((*ep)->GetStringParameter("SolveFor") == "AtmosDensityScaleFactor")
               psm->SetProperty("AtmosDensity_Epsilon", *p);
         }
      }
   }
}


//------------------------------------------------------------------------------
// bool PreExecution()
//------------------------------------------------------------------------------
/**
* This function plays the role as Initialize() function due to RunEstimator 
* delays initialization til before execution step.
*
* Note that: code in the old Initialize function was moved here
*/
//------------------------------------------------------------------------------
bool RunEstimator::PreExecution()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start RunEstimator::PreExecution()\n");
#endif

   bool retval = false;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("PreExecution(): Step 1: initialize RunEstimator\n");
#endif
   if (Initialize())
   {
#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 2: re-initialize the Estimator\n");
#endif
      retval = theEstimator->Reinitialize();

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 3: set up event manager\n");
#endif
      // Find the event manager and store its pointer
      if (triggerManagers == NULL)
          throw CommandException("The Event Manager pointer was not set on the "
            "RunEstimator command");

      for (UnsignedInt i = 0; i < triggerManagers->size(); ++i)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("RunEstimator has an TriggerManager of "
               "type %s, id %d\n",
               (*triggerManagers)[i]->GetTriggerTypeString().c_str(),
               (*triggerManagers)[i]->GetTriggerType());
         #endif
         if ((*triggerManagers)[i]->GetTriggerType() == Gmat::EVENT)
         {
            eventMan = (EventManager*)(*triggerManagers)[i];
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("RunEstimator has an EventManager of "
                  "type %s\n", eventMan->GetTriggerTypeString().c_str());
            #endif
         }
      }
      if (eventMan == NULL)
         throw CommandException("The EventManager pointer was not set on the "
            "RunEstimator command");


#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 4: set up Propagator\n");
#endif
      // Load participant names to estimation state manager
      MeasurementManager *measman = theEstimator->GetMeasurementManager();
      EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

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
      std::map<std::string, PropSetup*> knownProps;
      
      // Setup new ones
      StringArray participants = measman->GetParticipantList();
      for (UnsignedInt i = 0; i < participants.size(); ++i)
      {
         GmatBase *sc = FindObject(participants[i]);
         if ((sc) && (sc->IsOfType("Spacecraft")))
         {
            
            ((Spacecraft*)(sc))->SetRunningCommandFlag(3);    // input value 3 for running estimation command

            PropSetup *prop = theEstimator->GetPropagator(participants[i]);
            if (prop == nullptr)
               throw CommandException("Cannot initialize RunEstimator command; the "
                     "propagator pointer requested from the Estimator " +
                     theEstimator->GetName() + " for the spacecraft " + participants[i] +
                     " is NULL.");

            std::string propName = prop->GetName();
            PropSetup *ps = nullptr;

            if (knownProps.find(propName) == knownProps.end())
            {
               ps = (PropSetup*)prop->Clone();
               ps->SetPrecisionTimeFlag(true);
               propagators.push_back(ps);

               StringArray satNames;
               satNames.push_back(participants[i]);
               satList[ps] = satNames;
               knownProps[propName] = ps;

			   if (prop->GetPropagator()->IsOfType("EphemerisPropagator"))
			   {
               if (prop->GetPropagator()->IsOfType("Code500Propagator"))
               {
                  if (!GmatGlobal::Instance()->InTestingMode())
                     throw CommandException("Error: The use of a Code 500 propagator in an Estimator is deprecated and no longer allowed. Please use a different propagator.\n");
               }
				   prop->GetPropagator()->TakeAction("RunPastStartEnd");
				   theEstimator->SetPropagatorSatMap(propName, participants[i]);
			   }

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
                  satNames.push_back(participants[i]);
                  satList[ps] = satNames;
                  knownProps[propName] = ps;
                  ps->GetPropagator()->TakeAction("RunPastStartEnd");
				      p.push_back(ps->GetPropagator());
                  fm.push_back(ps->GetODEModel());
                  eventMan->SetObject(ps);

				      theEstimator->SetPropagatorSatMap(propName, participants[i]);
                  retval = true;
               }
               else
               {
                  ps = knownProps[propName];
                  StringArray satNames = satList[ps];
                  if (find(satNames.begin(), satNames.end(), participants[i]) == satNames.end())
                     (satList[ps]).push_back(participants[i]);
               }
            }

            if (ps->GetPropagator()->UsesODEModel())
               sc->TakeAction("UseSTM");
            else
               sc->TakeAction("SkipSTM");
            

            //Disables use of EKF and EKFS with ephemeris SPK and OEM ephem files
            if (theEstimator->IsOfType("ExtendedKalmanFilter") || theEstimator->IsOfType("Smoother"))
            {
               
               std::vector<std::string> et = ((Spacecraft*)sc)->GetEphemerisFileTypes();
               for (int j = 0; j < et.size(); j++)
               {
                  Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
                  if (et[j] == "CCSDS-OEM")
                     if (runmode != GmatGlobal::TESTING)
                        throw CommandException("Writing to a CCSDS-OEM ephemeris file is disallowed in the ExtendedKalmanFilter and the Smoother\n");
                  if (et[j] == "SPK")
                     throw CommandException("Writing to an SPK ephemeris file is disallowed in the ExtendedKalmanFilter and the Smoother\n");
               }
            }
         }
      }

      for (UnsignedInt i = 0; i < propagators.size(); ++i)
      {
         propObjectNames.push_back(satList[propagators[i]]);
         if (propagators[i]->GetPropagator()->IsOfType("EphemerisPropagator"))
            propagators[i]->GetPropagator()->SetStringParameter("StartEpoch", "FromSpacecraft");
      }

      propPrepared = false;

      measman->SetTransientForces(transientForces);
      theEstimator->SetTransientForces(transientForces);

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 5: call AddExternalStmSetting("", NULL, -1) on participant spacecrafts\n");
#endif
      #ifdef DEBUG_LOAD_SOLVEFORS
         MessageInterface::ShowMessage("Marching through the %d entry "
               "transient list\n", transientForces->size());
      #endif
      // Build spacecraft list and clear external STM settings
      std::vector<Spacecraft*> scs;
      for (UnsignedInt k = 0; k < participants.size(); ++k)
      {
         GmatBase *party = FindObject(participants[k]);
         if (party != NULL)
         {
            if (party->IsOfType(Gmat::SPACECRAFT))
            {
               scs.push_back((Spacecraft*)party);
            }
         }
      }

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 6: call AddExternalStmSetting(<solveForName>, <transientForce>, <parmID>) on participant spacecrafts\n");
#endif
      // March through transient forces and load up SC STM with their entries
      for (UnsignedInt i = 0; i < transientForces->size(); ++i)
      {
         StringArray sfl = transientForces->at(i)->GetSolveForList();
         #ifdef DEBUG_LOAD_SOLVEFORS
            MessageInterface::ShowMessage("   transientForces[%d] with name <%s> has %d solve-fors\n", i, transientForces->at(i)->GetName().c_str(),
                  sfl.size());
         #endif
         for (UnsignedInt j = 0; j < sfl.size(); ++j)
         {
            // Tell the spacecraft about STM entries needed for the STM
            #ifdef DEBUG_LOAD_SOLVEFORS
               MessageInterface::ShowMessage("      Adding %s to %d "
                     "paricipants\n", sfl[j].c_str(), participants.size());
            #endif
            for (UnsignedInt k = 0; k < scs.size(); ++k)
            {
               #ifdef DEBUG_LOAD_SOLVEFORS
                  MessageInterface::ShowMessage("         Adding to %s\n",
                        scs[k]->GetName().c_str());
               #endif
               Integer paramID = transientForces->at(i)->GetParameterID(sfl[j]);
               bool found = false;
               Integer estID = transientForces->at(i)->GetEstimationParameterID(sfl[j]);
               Integer size = transientForces->at(i)->GetEstimationParameterSize(estID);
               Integer id = scs[k]->AddExternalStmSetting(
                               sfl[j], transientForces->at(i),
                               paramID, found, size);

               // Add size of external entries to STM
               if (!found)
               {
                  scs[k]->SetIntegerParameter("FullSTMRowCount", 
                     scs[k]->GetIntegerParameter("FullSTMRowCount") + size);
               }
               transientForces->at(i)->SetStmIndex(id, paramID);
            }
         }
      }

      // Tell the spacecraft about STM entries needed for the STM
      for (UnsignedInt i = 0; i < scs.size(); ++i)
      {
         #ifdef DEBUG_LOAD_SOLVEFORS
            MessageInterface::ShowMessage("  Set STM external entries for Spacecraft %s\n",
               scs[i]->GetName().c_str());
         #endif

         // Processing all plates on the spacecraft
         Spacecraft* sc = scs[i];
         std::vector<StringArray> constraints = sc->GetEqualConstrains();
         ObjectArray plates = sc->GetRefObjectArray("Plate");
         for (Integer j = 0; j < plates.size(); ++j)
         {
            // Resize STM for adding plate's solve-for variables
            Plate* pl = (Plate*)plates[j];                   // fix this bug. Index j is used for plates
            StringArray sfList = pl->GetStringArrayParameter("SolveFors");
            for (Integer k = 0; k < sfList.size(); ++k)
            {
               // Only add external entries and size to STM when solve-for is on the first entry in the constraint list
               std::string sfFullName = pl->GetName() + "." + sfList[k];
               bool addToList = true;
               for (Integer k1 = 0; k1 < constraints.size(); ++k1)
               {
                  for (Integer k2 = 1; k2 < constraints[k1].size(); ++k2)
                  {
                     if (sfFullName == constraints[k1][k2])
                     {
                        addToList = false;
                        break;
                     }
                  }

                  if (addToList == false)
                     break;
               }

               if (addToList)
               {
                  // Add external STM setting
                  bool found = false;
                  Integer id = pl->GetParameterID(sfList[k]);
                  Integer estID = pl->GetEstimationParameterID(sfList[k]);
                  Integer size = pl->GetEstimationParameterSize(estID);

                  sc->AddExternalStmSetting(
                     sfFullName,                // Full name: ex: set solve-for "Plate1.DiffuseFraction" to external STM list in the spacecraft
                     pl, id, found, size);

                  if (!found)
                  {
                     // Add size of external entries to STM
                     //Integer size = pl->GetEstimationParameterSize(id);
                     sc->SetIntegerParameter("FullSTMRowCount", 
                        sc->GetIntegerParameter("FullSTMRowCount") + size);
                  }
               }
            }
         }

         // Processing all Hardware objects on the spacecraft
         ObjectArray hardwareObjs = sc->GetRefObjectArray("Hardware");
         for (Integer j = 0; j < hardwareObjs.size(); ++j)
         {
            GmatBase* hw = hardwareObjs[j];
            StringArray sfList;
            try
            {
               sfList = hw->GetStringArrayParameter("SolveFors");
            }
            catch (...)
            {
               sfList.clear();
            }

            for (Integer k = 0; k < sfList.size(); ++k)
            {
               // Add external STM setting
               std::string sfFullName = hw->GetName() + "." + sfList[k];
               bool found = false;
               Integer id = hw->GetParameterID(sfList[k]);
               Integer estID = hw->GetEstimationParameterID(sfList[k]);
               Integer size = hw->GetEstimationParameterSize(estID);

               sc->AddExternalStmSetting(sfFullName, hw, id, found, size);

               if (!found)
               {
                  // Add size of external entries to STM
                  //Integer size = hw->GetEstimationParameterSize(id);
                  sc->SetIntegerParameter("FullSTMRowCount",
                     sc->GetIntegerParameter("FullSTMRowCount") + size);
               }
            }
            
         }
      }


#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunEstimator command found %d "
            "participants\n", participants.size());
      #endif

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 7: load participant names to EstimationStateManager\n");
#endif
#ifdef DEBUG_LOAD_SOLVEFORS
      for (UnsignedInt j = 0; j < participants.size(); ++j)
        MessageInterface::ShowMessage("RunEstimator::PreExecution() Participants   %s\n", participants[j].c_str());
#endif
      // Load participant names to estimation state manager
      esm->SetParticipantList(participants);

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PreExecution(): Step 8: initialize RunSolver\n");
#endif
      // Now we can initialize the propagation subsystem by calling up the
      // inheritance tree.
      try
      {
#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunSolver::Initialize()\n");
#endif
         isInitialized = RunSolver::Initialize();
      } catch(GmatBaseException e)
      {
         MessageInterface::ShowMessage(" *** message: %s\n", e.GetDetails().c_str());
      }
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End RunEstimator::PreExecution()\n");
#endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Performs the command side processing for the Simulation
 *
 * This method calls the Estimator to determine the state of the Simulation
 * state machine and responds to that state as needed.  Typical command side
 * responses are to propagate as needed, to clean up memory, or to reset flags
 * based on the state machine.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Execute()
{
   #ifdef DEBUG_STATE
     MessageInterface::ShowMessage("*** Enter RunEstimator:Execute()\n");
   #endif

   //--------------------------------------------------------------------
   // Steps to run before running Execute()
   //--------------------------------------------------------------------
   // Initialization step is moved here:
   if (delayInitialization)
   {
      // It needs to run initialization now
      delayInitialization = false;
      PreExecution();
   }


   #ifdef DEBUG_EXECUTION
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
   Solver::SolverState state = theEstimator->GetState();
   
   // Set run state here (for fixing GMT-5101 LOJ: 2015.06.16)
   if(theEstimator->IsFinalPass())
      publisher->SetRunState(Gmat::SOLVEDPASS);
   else
      publisher->SetRunState(Gmat::SOLVING);
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("\nEstimator state is %d\n", state);
   #endif
   
   try
   {
      switch (state)
      {
      case Solver::INITIALIZING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): INITIALIZING state\n");
         #endif
			
         PrepareToEstimate();
         {
            EstimationStateManager *esm = theEstimator->GetEstimationStateManager();
#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator::Execute(): Solver::INITIALIZING: load solve-fors to EstimationStateManager\n");
#endif
            // Load solve for objects to esm
            LoadSolveForsToESM();

            // Reset the propagation state vector because the STM may have resized
            // TBD: Do we need to do this?
            // From HERE --------------------------------------
            hasFired = false;
            for (UnsignedInt i = 0; i < p.size(); ++i)
            {
               if (p[i]->IsOfType("EphemerisPropagator"))
                  p[i]->TakeAction("RunPastStartEnd");
               p[i]->Initialize();
               psm[i]->MapObjectsToVector();
               p[i]->Update(true);
            }
            // To HERE --------------------------------------

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: set solver-for objects to EstimationStateManager\n");
#endif
            // Pass in the objects
            StringArray objList = esm->GetObjectList("");
            for (UnsignedInt i = 0; i < objList.size(); ++i)
            {
               std::string propName = objList[i];
               StringArray objName; 
               std::string refObjectName = "";
               size_t loc = propName.find('.');              // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
               objName.push_back(propName.substr(0, loc));
               if (loc != propName.npos)
               {
                  refObjectName = (propName.substr(loc + 1));
               }
                  
               while (loc != propName.npos)
               {
                  loc = refObjectName.find('.');
                  objName.push_back(refObjectName.substr(0, loc));
                  refObjectName = (refObjectName.substr(loc + 1));
               }

               GmatBase* obj = FindObject(objName[0]);
               // if referent object is used, set referent object to be solve-for object
               // ex: propName = "CAN.ErrorModel1". Referent object is "ErrorModel1". It needs to set object ErrorModel1 to estimation state mananger
               if (objName.size() != 1)
               {
                  refObjectName = objName[0] + "." + objName[1];
                  GmatBase* refObj = obj->GetRefObject(Gmat::UNKNOWN_OBJECT, refObjectName);
                  int k = 1;
                  while (k < objName.size() - 1)
                  {
                     refObjectName = objName[k] + "." + objName[k+1];
                     refObj = refObj->GetRefObject(Gmat::UNKNOWN_OBJECT, refObjectName);
                     k++;
                  }
                  obj = refObj;
                  obj->SetFullName(propName);
               }

               if (obj != NULL)
               {
                  if (obj->IsOfType(Gmat::ODE_MODEL))
                  {
                     //Is this right? Loop through fm and set to last? 
					      for (int i = 0; i < fm.size(); i++)
					      {
						      if (fm[i] != nullptr)
						      {
                           if (fm[i]->GetName() == obj->GetName())
                           {
                              // Use the internal ODEModel
                              esm->SetObject(fm[i]);
                              // Refresh its solvefor buffer
                              fm[i]->GetSolveForList();
                           }
						      }
					      }
                  }
#ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: object <%s,%p> set to EstimationStateManager\n", obj->GetName().c_str(), obj);
#endif            
                  else
                     esm->SetObject(obj);
               }
            }
            if(objList.size() > 0)
               esm->CheckObjects();
#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Execute(): Solver::INITIALIZING: build state in EstimationStateManager\n");
#endif
            esm->BuildState();
            //esm->MapObjectsToVector();

            UpdateInitialConditions();


            // Calculate state dot at initial epoch
            CalculateStateDotAtInitialEpoch();
            // copy acceleration from spacecraft to esimation gmat state vector
            esm->MapObjectsToVector();

            // Re-register publisher
            //StringArray owners, elements;
            pubdataOwners.clear();
            pubdataElements.clear();
            pubdataOwners.push_back("All");
            pubdataElements.push_back("All.epoch");

            //esm->PrepareStateInfoToPublish(pubdataOwners, pubdataElements);
            // Get element's info from propagation state manager instead of estimation
            // state manager. 
            for (Integer idx = 0; idx < psm.size(); ++idx)
               psm[idx]->PrepareStateInfoToPublish(pubdataOwners, pubdataElements);

            covStartIndex = pubdataElements.size();

            //esm->PrepareCovAndAccelerationInfoToPublish(pubdataOwners, pubdataElements);
            // Get element's info from propagation state manager instead of estimation
            // state manager.
            for (Integer idx = 0; idx < psm.size(); ++idx)
               psm[idx]->PrepareCovAndAccelerationInfoToPublish(pubdataOwners, pubdataElements);

            attStartIndex = pubdataElements.size();

            for (Integer idx = 0; idx < psm.size(); ++idx)
               psm[idx]->PrepareAttitudeInfoToPublish(pubdataOwners, pubdataElements);
            
            #ifdef DEBUG_PUBLISH_STATE_DATA
               MessageInterface::ShowMessage("  pubdataElements.size() = %d\n", pubdataElements.size());
               for (Integer idx = 0; idx < pubdataElements.size(); ++idx)
                  MessageInterface::ShowMessage("pubdataOwners[%d] = <%s>    pubdataElements[%d] = <%s>\n", 
                     idx, pubdataOwners[idx].c_str(), idx, pubdataElements[idx].c_str());

            #endif

            publisher->UnregisterPublishedData(this);
            streamID = -1;
            streamID = publisher->RegisterPublishedData(this, streamID, pubdataOwners, pubdataElements);


            // Publish data
            if (pubdata)
               delete[] pubdata;


            pubdataSize = expandedDim = pubdataElements.size();
            pubdata = new Real[pubdataSize];

            PublishState();
         }
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): INITIALIZING state\n");
         #endif
         break;

      case Solver::PROPAGATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): PROPAGATING state\n");
         #endif
         Propagate();

         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): PROPAGATING state\n");
         #endif

         break;

      case Solver::CALCULATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): CALCULATING state\n");
         #endif
         Calculate();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): CALCULATING state\n");
         #endif

         break;

      case Solver::LOCATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): LOCATING state\n");
         #endif
         LocateEvent();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): LOCATING state\n");
         #endif
         break;

      case Solver::ACCUMULATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): ACCUMULATING state\n");
         #endif
         Accumulate();
			
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): ACCUMULATING state\n");
         #endif
         break;

      case Solver::ESTIMATING:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): ESTIMATING state\n");
         #endif
         Estimate();
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): ESTIMATING state\n");
         #endif
         break;

      case Solver::CHECKINGRUN:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): CHECKINGRUN state\n");
         #endif
         CheckConvergence();
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): CHECKINGRUN state\n");
         #endif
         break;

      case Solver::FINISHED:
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Entered RunEstimator::Execute(): FINISHED state\n");
         #endif

         // Set run state to SOLVEDPASS here (for fixing GMT-5101 LOJ: 2015.06.16)
         publisher->SetRunState(Gmat::SOLVEDPASS);

         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i])
            {
               fm[i]->UpdateFromSpaceObject();
               fm[i]->TakeAction("UpdateSpacecraftParameters");
            }
            else
            {
               p[i]->UpdateFromSpaceObject();
               p[i]->TakeAction("UpdateSpacecraftParameters");
            }
         }

         // Publish the final state
         PublishState();

         // Why is Finalize commented out???  There is no command summary because of this change.
         // Finalize();
         // Adding in for now.
         BuildCommandSummary(true);
         #ifdef DEBUG_STATE
            MessageInterface::ShowMessage("Exit RunEstimator::Execute(): FINISHED state\n");
         #endif
         break;

      default:
         throw CommandException("Unknown state "
               " encountered in the RunEstimator command");
      }

      #ifdef DEBUG_STATE
         MessageInterface::ShowMessage("*** Start AdvanceState ... RunEstimator:Execute()\n");
      #endif

      if (state != Solver::FINISHED)
         state = theEstimator->AdvanceState(); // ekf mod 12/16
      else
      {
         // It has to run all work in AdvanceState() before Finalize()
         state = theEstimator->AdvanceState();  // ekf mod 12/16
         Finalize();
      }
   } catch (...)//(EstimatorException ex1)
   {
      Finalize();
      throw; // ex1;
   }

   #ifdef DEBUG_STATE
      MessageInterface::ShowMessage("*** Exit RunEstimator:Execute()\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Completes processing so that subsequent mission runs can be executed.
 */
//------------------------------------------------------------------------------
void RunEstimator::RunComplete()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::RunComplete()\n");
   #endif
   //commandRunning = false;                        // Fix bug GMT-5818 Batch estimation stop and start error

   RunSolver::RunComplete();
   commandComplete = true;                          // Fix bug GMT-5818 Batch estimation stop and start error
   commandRunning = false;                          // Fix bug GMT-5818 Batch estimation stop and start error
   propPrepared = false;                            // Fix bug GMT-5818 Batch estimation stop and start error

   overridePropInit = true;                         // Fix bug GMT-5818 Batch estimation stop and start error
   delayInitialization = true;                      // Fix bug GMT-5818 Batch estimation stop and start error

   try
   {
      if (eventMan)
      {
         eventMan->CleanUp();

         // This causes crash. Use CleanUp() instead
         //delete eventMan;
         eventMan = NULL;
      }
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_, "Error: EventManager::CleanUp() has error.\n");
   }

   
   try
   {
      if (theEstimator)
      {
         delete theEstimator;
         theEstimator = NULL;
      }
   }
   catch (...)
   {
      MessageInterface::PopupMessage(Gmat::WARNING_, "Error: ~Estimator() has error.\n");
   }

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::RunComplete()\n");
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
bool RunEstimator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   if (action == "Reset")
   {
      theEstimator->TakeAction("Reset");
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
 * This method returns a pointer to the current RunEstimator command while the
 * simulation state machine is running.  It returns the next pointer after the
 * simulation has finished execution.
 *
 * @return The next command that should Execute()
 */
//------------------------------------------------------------------------------
GmatCommand* RunEstimator::GetNext()
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
bool RunEstimator::HasLocalClones()
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
void RunEstimator::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("Spacecraft"))
      return;
   if (obj->IsOfType("Parameter"))
      return;
   throw CommandException("To do: implement RunEstimator::UpdateClonedObject "
         "for " + obj->GetTypeName() + " objects");
}

//------------------------------------------------------------------------------
// Methods triggered by the finite state machine
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void PrepareToEstimate()
//------------------------------------------------------------------------------
/**
 * Responds to the INITIALIZING state of the finite state machine
 *
 * Performs the final stages of initialization that need to be performed prior
 * to running the estimation.  This includes the final ODEModel preparation and
 * the setting for the flags that indicate that an estimation is in process.
 */
//------------------------------------------------------------------------------
void RunEstimator::PrepareToEstimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "Entered RunEstimator::PrepareToEstimate()\n");
   #endif

   if (!propPrepared)
   {
      // The code to register publised data should be to near the code to
      // publish data. Therefore, this code section is moved to
      // RunEstimator::Execute() function.

      //// Re-register publisher now that STM is added to ForceModel
      //publisher->UnregisterPublishedData(this);
      //streamID = -1;

      //StringArray owners, elements;
      ///// @todo Check to see if All and All.Epoch belong for all modes.
      //owners.push_back("All");
      //elements.push_back("All.epoch");

      //for (UnsignedInt ii = 0U; ii < sats.size(); ii++)
      //   if (sats[ii]->GetType() != Gmat::FORMATION)
      //      SetNames(sats[ii]->GetName(), owners, elements);

      //// Fill out remaining elements before covariance elements
      //Integer fdim = 0;
      //for (UnsignedInt i = 0; i < fm.size(); ++i)
      //{
      //   if (fm[i])
      //      fdim += fm[i]->GetDimension();
      //   else
      //      fdim += p[i]->GetDimension();
      //}

      //while (owners.size() <= fdim)
      //{
      //   owners.push_back("");
      //   elements.push_back("");
      //}

      //for (UnsignedInt ii = 0U; ii < sats.size(); ii++)
      //   if (sats[ii]->GetType() != Gmat::FORMATION)
      //      SetCovNames(sats[ii]->GetName(), owners, elements);
      
      //streamID = publisher->RegisterPublishedData(this, streamID, owners, elements);
      
      //Ensures all multisat epochs match. Ensures ephem props are on the same epoch
      bool currentEpochSet = false;
      GmatTime ce;
      // For Multisat, check that all epochs are the same
      if (sats.size() > 0)
      {
         for (int cyc = 0; cyc < sats.size() - 1; cyc++)
         {
            if (!currentEpochSet)
            {
               if (!theEstimator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName()))
               {
                  ce = ((Spacecraft *)sats[cyc])->GetEpoch();
                  theEstimator->UpdateCurrentEpoch(ce);
                  currentEpochSet = true;
               }
            }
            for (int cyc2 = cyc + 1; cyc2 < sats.size(); cyc2++)
            {
               if (!currentEpochSet)
               {
                  if (!(theEstimator->DoesSatUseEphemerisPropagator(sats[cyc2]->GetName())))
                  {
                     ce = ((Spacecraft *)sats[cyc2])->GetEpoch();
                     theEstimator->UpdateCurrentEpoch(ce);
                     currentEpochSet = true;
                  }
               }
               if (((Spacecraft *)sats[cyc2])->GetEpoch() != ((Spacecraft *)sats[cyc])->GetEpoch())
               {
                  //Will not throw if either sat is from an ephemeris file. 
                  if (!(theEstimator->DoesSatUseEphemerisPropagator(sats[cyc2]->GetName())) && !(theEstimator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName())))
                     throw CommandException("All participating Spacecraft must be at the same epoch prior to simulation. Use the Propagate command to advance all Spacecraft to a common epoch before running the simulator or estimator.");
               }
            }
         }
         if (currentEpochSet == false)
         {
            ce = theEstimator->GetMeasurementManager()->GetEpochGT();
            theEstimator->UpdateCurrentEpoch(ce);
            currentEpochSet = true;
         }

         for (int cyc = 0; cyc < sats.size(); cyc++)
         {
            if (theEstimator->DoesSatUseEphemerisPropagator(sats[cyc]->GetName()))
            {
               ((Spacecraft *)sats[cyc])->SetEpochGT(ce);
            }
         }

         for (UnsignedInt n = 0U; n < propagators.size(); n++)
         {
            if (propagators[n]->GetPropagator()->IsOfType("EphemerisPropagator"))
            {
               propagators[n]->GetPropagator()->SetStringParameter("StartEpoch", "FromSpacecraft");
            }
         }
      }

      PrepareToPropagate(false);  // ?? Test return value here?

      //Integer covDim = 21 * sats.size();
      //if (pubdata)
      //   delete[] pubdata;
      //pubdata = new Real[dim + covDim + 1];

      commandRunning  = true;
      commandComplete = false;
      propPrepared    = true;



      // Warn that the attitude is not updated at each propagation intermediate step [GMT-4398]
      // FIXME: Fix this when PropagationEnabledCommand is refactored
      // Check if the attitude affects the dynamics of any force model
      bool attAffectDyn = false;
      for (UnsignedInt n = 0U; n < propagators.size(); n++)
      {
         ODEModel *odem = propagators[n]->GetODEModel();
         if (odem)
         {
            for (UnsignedInt ii = 0U; ii < odem->GetNumForces(); ii++)
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
         MessageInterface::ShowMessage("Warning: A Spacecraft in the Estimator is using NadirPointing or "
            "an ObjectReferenced axes for its attitude with a force model that has dynamics affected by the attitude. "
            "The attitude is not updated at intermediate steps in the integrator, so the result may be inaccurate.\n");
      // End PropagationEnabledCommand FIXME
   }

   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         estimationOffset = fm[i]->GetTime();
         // @todo Temporary -- Turn off range check for Cr.  This needs to be made conditional,
         // and only active if Cr is a solve-for
         fm[i]->TakeAction("SolveForCr");
      }
   }

   CacheForceDerivatives();

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "Exit RunEstimator::PrepareToEstimate()\n");
   #endif

   #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("Object states at close of PrepareToEstimate:\n");
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
               MessageInterface::ShowMessage("      EpochGT: [%s]\n",
                  ((SpaceObject*)poa->at(j))->GetEpochGT().ToString().c_str());
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("X"),
                     poa->at(j)->GetRealParameter("Y"),
                     poa->at(j)->GetRealParameter("Z"));
               MessageInterface::ShowMessage("      [%16.14lf, %16.14lf, %16.14lf]:\n",
                     poa->at(j)->GetRealParameter("VX"),
                     poa->at(j)->GetRealParameter("VY"),
                     poa->at(j)->GetRealParameter("VZ"));
               
               
               Rmatrix stm = poa->at(j)->GetRmatrixParameter("STM");
               MessageInterface::ShowMessage("STM = [\n");
               for (Integer row = 0; row < stm.GetNumRows(); ++row)
               {
                  for (Integer col = 0; col < stm.GetNumColumns(); ++col)
                     MessageInterface::ShowMessage("%.15le   ", stm(row,col));
                  MessageInterface::ShowMessage("]\n");
               }
               MessageInterface::ShowMessage("]\n");
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
 * Propagation from the current epoch to the next estimation epoch is performed
 * in this method.
 */
//------------------------------------------------------------------------------
void RunEstimator::Propagate()
{
   /// Warning: code related to force mode fm or ode model in this function may
   /// cause a crash when ephemeris propagator is used instead of ode model.

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Propagate()\n");
   #endif

   // If state reset at current epoch - e.g. during sequential estimation -
   // reload the prop vector
   if (theEstimator->ResetState())
   {
      #ifdef DEBUG_STATE_RESETS
         MessageInterface::ShowMessage("Calling UpdateFromSpaceObject()\n");
         Real* oldState = fm[0]->GetState();
         MessageInterface::ShowMessage("   Old state:  (%.12lf   %.12lf   %.12lf)\n", oldState[0], oldState[1], oldState[2]);
      #endif

      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->TakeAction("UpdateSpacecraftParameters");
            fm[i]->UpdateInitialData(false, false);
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->SetTime(estimationOffset);
            p[i]->TakeAction("UpdateSpacecraftParameters");
         }
      }

      CacheForceDerivatives();

      // Publish the new state
      PublishState();
      
      #ifdef DEBUG_STATE_RESETS
         Real* newState = fm[0]->GetState();
         MessageInterface::ShowMessage("   New state:  (%.12lf   %.12lf   %.12lf\n", newState[0], newState[1], newState[2]);
      #endif
   }

   // If state reset to initial epoch - e.g. during batch estimation -
   // reload prop vector and reset the epoch information
   if (startNewPass == true)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->SetTime(estimationOffset);
            fm[i]->TakeAction("UpdateSpacecraftParameters");
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->SetTime(estimationOffset);
            p[i]->TakeAction("UpdateSpacecraftParameters");
         }
      }
      startNewPass = false;
   }
   
   Real dt = theEstimator->GetTimeStep();

   /// Updated for Multi-spacecraft prop
   Real maxStep = fabs(maxSteps[0]);
   for (Integer chker = 0; chker < maxSteps.size(); chker++)
   {
      if (maxStep < fabs(maxSteps[chker]))
      {
         maxStep = fabs(maxSteps[chker]);
      }
   }
   #ifdef DEBUG_STEP_CONTROL
      MessageInterface::ShowMessage("Estimator step = %.12lf; \n", dt);
   #endif

   if (fabs(dt) > maxStep)
      dt = (dt > 0.0 ? maxStep : -maxStep);

   #ifdef DEBUG_STEP_CONTROL
      MessageInterface::ShowMessage("Stepping by %.12lf; could be %lf\n", dt, p[0]->GetStepSize());
   #endif

   #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("Stepping by %.12le seconds\n", dt);
   #endif

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      Real *b4State = fm[0]->GetState();

		MessageInterface::ShowMessage("State before prop: dim = %d   fm[0]->GetState() = <%p>\n  ", dim, b4State);
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12le", b4State[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   // Ignore ephemeris gaps exceptions for TDRS measurements
   skipEphemerisProp = true;
   Step(dt);
   skipEphemerisProp = false;

   bufferFilled = false;

   GmatTime currentIntegratedEpoch;
   // Fix bug related to current epoch. Without this line, current epoch is always set to 
   // the default value of GmatTime when a ephemeris propagator is used for spacecraft.
   currentIntegratedEpoch = currEpochGT[0];
   for (UnsignedInt i = 0; i < currEpochGT.size(); ++i)
   {
      #ifdef DEBUG_INITIAL_STATE
      MessageInterface::ShowMessage("propagators[%d] <%s>: currEpoch[%d] = %.15lf    currEpochGT[%d] = %s\n", 
         i, propagators[i]->GetName().c_str(), i, currEpoch[i], i, currEpochGT[i].ToString().c_str());
      #endif

      if (fm[i] != NULL)
         currentIntegratedEpoch = currEpochGT[i];
   }
   theEstimator->UpdateCurrentEpoch(currentIntegratedEpoch);
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Propagate()\n");
   #endif

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      Real *odeState = fm[0]->GetState();

		MessageInterface::ShowMessage("State after prop: epoch = %s   fm[0]->GetState() = <%p>\n", currEpochGT[0].ToString().c_str(), fm[0]->GetState());
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12le", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif

}


//------------------------------------------------------------------------------
// void Calculate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Calculate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Calculate() "
            "epoch = %.12lf   epochGT = %s\n", currEpoch[0], currEpochGT[0].ToString().c_str());
   #endif

   bufferFilled = false;

   if (IsOfType("RunSmoother") == false)
   {
      // This code is moved from ESTIMATING state.
      PublishState();

      // At this point, all spacecrafts' epoch have the same value of
      // measurement time tag tm. New segment is start at (the first
      // spacecarft's epoch) measurement time tag.
      publisher->FlushBuffers(true);
      for (ObjectArray::iterator sat = sats.begin(); sat != sats.end(); sat++)
      {
         if (createNewSegmentsNow == true)
         {
            //if (HasPrecisionTime())
            //   CreateNewSegments(currEpochGT[0].GetMjd());
            //else
               CreateNewSegments(currEpoch[0]);
         }
         publisher->SetSpacecraftPropertyChanged(this, currEpochGT[0].GetMjd(), 
            (*sat)->GetName(), "RunEstimator");
      }
   }

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Calculate()\n");
   #endif
}


//------------------------------------------------------------------------------
// void LocateEvent()
//------------------------------------------------------------------------------
/**
 * Responds to the LOCATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::LocateEvent()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::LocateEvent()\n");
   #endif

//   bool newEvent = false;
   EventStatus status;

   // First time through, buffer the objects that get propagated
   if (!bufferFilled)
   {
      if (fm[0])
         dt = fm[0]->GetTime();
      else
         dt = p[0]->GetTime();

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
      eventList = theEstimator->GetRefObjectArray(Gmat::EVENT);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Event list has %d events\n",
               eventList.size());
      #endif
      // Set status of the events to ITERATING
      for (UnsignedInt i = 0; i < eventList.size(); ++i)
      {
         ((Event*)eventList[i])->CheckStatus(ITERATING);
         ((Event*)eventList[i])->Initialize();
      }

      if (eventList.size() == 0)
      {
         currentEvent = NULL;
         #ifdef DEBUG_EXECUTION
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

   if (currentEvent == NULL)
      return;

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
            MessageInterface::ShowMessage("   Event %d, of type %s\n", eventIndex,
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

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::LocateEvent()\n");
   #endif

}



//------------------------------------------------------------------------------
// void Accumulate()
//------------------------------------------------------------------------------
/**
 * Performs command side actions taken during accumulation
 */
//------------------------------------------------------------------------------
void RunEstimator::Accumulate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Accumulate()\n");
   #endif

   CleanUpEvents();

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Accumulate()\n");
   #endif
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Responds to the ESTIMATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Estimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Estimate()\n");
   #endif

   CleanUpEvents();
   
   // No data is allowed to publish in ESTIMATING state because
   // after solving light-time solution, states of spacecrafts are get at 
   // different time stamps.
   //PublishState();


   // Creating a new segment is not allowed here. It is allowed before
   // the step to solve light-time solution where epoches of all spacecrafts
   // have the same value of measurement time tag tm.
   
   //publisher->FlushBuffers(true);
   //for (ObjectArray::iterator sat = sats.begin(); sat != sats.end(); sat++)
   //{
   //   if (createNewSegmentsNow == true)
   //   {
   //      CreateNewSegments(currEpoch[0]);
   //   }
   //   publisher->SetSpacecraftPropertyChanged(this, currEpochGT[0].GetMjd(), (*sat)->GetName(), "RunEstimator");
   //}

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Estimate()\n");
   #endif
}

//------------------------------------------------------------------------------
// void CheckConvergence()
//------------------------------------------------------------------------------
/**
 * Performs command actions needed when testing for convergence
 */
//------------------------------------------------------------------------------
void RunEstimator::CheckConvergence()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::CheckConvergence() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   startNewPass = true;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::CheckConvergence()\n");
   #endif
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Responds to the FINALIZING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Finalize()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Finalize() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   // Finalize the Estimator
   if (theEstimator->Finalize() == false)
      MessageInterface::ShowMessage(
            "The Estimator %s reported a problem completing processing\n",
            theEstimator->GetName().c_str());
   
   BuildCommandSummary(true);
   
   commandComplete = true;
   commandRunning  = false;
   propPrepared    = false;

   overridePropInit = true;
   delayInitialization = true;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Exit RunEstimator::Finalize()\n");
   #endif
}


//------------------------------------------------------------------------------
// void CleanUpEvents()
//------------------------------------------------------------------------------
/**
 * Restores the buffered satellite states after event processing
 */
//------------------------------------------------------------------------------
void RunEstimator::CleanUpEvents()
{
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

         MessageInterface::ShowMessage("State in the Accumulate method:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  ");
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void PublishState()
//------------------------------------------------------------------------------
/**
 * Publishes the state to the publisher if ready.
 */
 //------------------------------------------------------------------------------
void RunEstimator::PublishState()
{
   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();

   if (theEstimator->IsOfType("SeqEstimator") && esm->HasStateOffset())
   {
      // Include the state offset in the subscribers
      esm->MapObjectsToVector();
      esm->MapFullVectorToObjects();

      //fm[0]->UpdateFromSpaceObject();
      // This code is used for multiple propagators instead of a single
      for (Integer i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
            fm[i]->UpdateFromSpaceObject();
      }
   }

   // 0. Clear pubdata 
   for (Integer idx = 0; idx < pubdataSize; ++idx)
      pubdata[idx] = GmatMathConstants::QUIET_NAN;

   // 1. Publish epoch 
   // This has assumption that all propagated objects has the same epoch
   // (time synchonized). That means currEpochGT[i] = currEpoch[0].
   pubdata[0] = currEpochGT[0].GetMjd();

   // 2. Publish all elements in propagation state manager
   // Use propagation state manager instead of estimation state manager
   //esm->PrepareStateDataToPublish(pubdata, pubdataSize, pubdataElements);
   for(Integer idx = 0; idx < psm.size(); ++idx)
      psm[idx]->PrepareStateDataToPublish(pubdata, pubdataSize, pubdataElements);

   Integer theDim = covStartIndex;

   #ifdef DEBUG_PUBLISH_STATE_DATA
      MessageInterface::ShowMessage(" theDim = %d\n", theDim);
   #endif

   // 3. Publish covariance and acceleration for each spacecraft
   // We have to use propagation state manager instead of estimation state manager.
   // Note that: estimation state manager only contains the estimation of
   // the spacecraft's initial state and all other solve-for variable. It does not 
   // contains spacecraft's state during the propagation.
   // Propagation state manager contains all propagated variables at the current epoch
   // it propagtes to. Therefore, we needs to get propagated elements there.
   for (Integer idx = 0; idx < psm.size(); ++idx)
      psm[idx]->PrepareCovAndAccelerationDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, hasPrecisionTime);

   // 4. Publish attitude for each spacecraft
   for (Integer idx = 0; idx < psm.size(); ++idx)
      psm[idx]->PrepareAttitudeDataToPublish(pubdata, pubdataSize, pubdataElements, attStartIndex, hasPrecisionTime);


#ifdef DEBUG_PUBLISH_STATE
   Solver::SolverState state = theEstimator->GetState();
   std::string stateS = "UNKNOWN     ";
   switch (state)
   {
   case Solver::INITIALIZING:
      stateS = "state = INITIALIZING";
      break;
   case Solver::PROPAGATING:
      stateS = "state = PROPAGATING ";
      break;
   case Solver::CALCULATING:
      stateS = "state = CALCULATING ";
      break;
   case Solver::LOCATING:
      stateS = "state = LOCATING    ";
      break;
   case Solver::ACCUMULATING:
      stateS = "state = ACCUMULATING";
      break;
   case Solver::ESTIMATING:
      stateS = "state = ESTIMATING  ";
      break;
   case Solver::CHECKINGRUN:
      stateS = "state = CHECKINGRUN ";
      break;
   case Solver::FINISHED:
      stateS = "state = FINISHED    ";
      break;
   }

   MessageInterface::ShowMessage("%s: time tag t = %.12lf  data = [", stateS.c_str(), pubdata[0]);
   for (Integer idx = 1; idx <= dim + covDim + 1; idx++)
      MessageInterface::ShowMessage("%.16le   ", pubdata[idx]);
   MessageInterface::ShowMessage("]\n");
#endif 


   //publisher->Publish(this, streamID, pubdata, theDim +idx, direction);
   publisher->Publish(this, streamID, pubdata, pubdataElements.size(), direction);
   if (theEstimator->IsOfType("SeqEstimator") && esm->HasStateOffset())
   {
      // Remove the state offset from the reference trajectory
      esm->MapVectorToObjects();

      // update state of spacecraft inside all force models
      //fm[0]->UpdateFromSpaceObject();
      // This code is used for multiple propagators instead of a single
      for (Integer i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
            fm[i]->UpdateFromSpaceObject();
      }
   }
}


//------------------------------------------------------------------------------
// void SetNames(const std::string& name, StringArray& owners,
//               StringArray& elements)
//------------------------------------------------------------------------------
/**
 * Sets the parameter names used when publishing Spacecraft data.
 *
 * @param <name>     Name of the Spacecraft that is referenced.
 * @param <owners>   Array of published data identifiers.
 * @param <elements> Individual elements of the published data.
 */
 //------------------------------------------------------------------------------
void RunEstimator::SetCovNames(const std::string& name, StringArray& owners,
   StringArray& elements)
{
   std::string names[6] = { "X", "Y", "Z", "Vx", "Vy", "Vz" };

   for (UnsignedInt ii = 0; ii < 6; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         owners.push_back(name);
         std::string elementName = name + ".C" + names[ii] + names[jj];
         elements.push_back(elementName);
      }
   }

#ifdef DEBUG_PUBLISH_DATA
   MessageInterface::ShowMessage
   ("RunEstimator::SetCovNames() Setting data labels:\n");
   for (unsigned int i = 0; i < elements.size(); i++)
      MessageInterface::ShowMessage("%s ", elements[i].c_str());
   MessageInterface::ShowMessage("\n");
#endif
}


//------------------------------------------------------------------------------
// void UpdateInitialConditions()
//------------------------------------------------------------------------------
/**
 * Updates the initial conditions for the estimator.
 */
 //------------------------------------------------------------------------------
void RunEstimator::UpdateInitialConditions()
{
   if (theEstimator->UpdateInitialConditions())
   {
      EstimationStateManager *esm = theEstimator->GetEstimationStateManager();
      GmatTime epochGT = esm->GetEstimationEpochGT();

      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         baseEpochGT[i] = epochGT;
         elapsedTime[i] = 0.0;
         currEpochGT[i] = epochGT;

         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->TakeAction("UpdateSpacecraftParameters");
            fm[i]->UpdateInitialData(false, false);
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->TakeAction("UpdateSpacecraftParameters");
         }
      }

      CacheForceDerivatives();
   }
}


//------------------------------------------------------------------------------
// void CacheForceDerivatives()
//------------------------------------------------------------------------------
/**
 * Cache the force derivatives in the estimator.
 */
 //------------------------------------------------------------------------------
void RunEstimator::CacheForceDerivatives()
{

   std::map<std::string, std::vector<Real *>> *derivs = theEstimator->GetForceDerivativeCache();
   for (int i = 0; i < propagators.size(); i++)
   {
      PropagationStateManager *propPsm = propagators[i]->GetPropStateManager();
      Real * state = propPsm->GetState()->GetState();
      Integer stateSize = propPsm->GetState()->GetSize();

      Real * stateDot = propPsm->GetState()->GetStateDot();
      for (Integer i = 0; i < stateSize; ++i)
         stateDot[i] = 0.0;
      
      ODEModel *propODE = propagators[i]->GetODEModel(); 
      if (propODE)
      {
         std::string odeName = propagators[i]->GetName() + propODE->GetName();
         for (UnsignedInt ii = 0U; ii < propODE->GetNumForces(); ii++)
         {
            PhysicalModel *pm = propODE->GetForce(ii);
            pm->GetDerivatives(state);
            const Real *deriv = pm->GetDerivativeArray();

            // Accumulate all forces
            for (Integer i = 0; i < stateSize; ++i)
               stateDot[i] += deriv[i];

            //derivs->insert(std::pair<std::string, std::vector<Real *>>(odeName, std::vector<Real *>()));
            //(*derivs)[odeName] = std::vector<Real *>(propODE->GetNumForces());

            if ((*derivs)[odeName].size() <= ii)
               ((*derivs)[odeName]).push_back(new Real[stateSize]);

            if (((*derivs)[odeName])[ii] == NULL)
               ((*derivs)[odeName])[ii] = new Real[stateSize];

            memcpy(((*derivs)[odeName])[ii], deriv, sizeof(Real) * stateSize);
         }
      }
   }
}

//------------------------------------------------------------------------------
// void RunEstimator::UpdateCov()
//------------------------------------------------------------------------------
/**
 * This method updates covariance on a non-step time. 
 * Initially written for use in just Finite burn cases. 
 * Use with caution
 */
 //------------------------------------------------------------------------------
void RunEstimator::UpdateCov()
{
   theEstimator->UpdateCov();

   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();


   Covariance *stateCovariance = esm->GetCovariance();
   Rmatrix cov = *(stateCovariance->GetCovariance());
   Rmatrix dX_dS = esm->CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix covCart = dX_dS * cov * dX_dS.Transpose();
   const std::vector<ListItem*> *map = esm->GetStateMap();

   Integer theDim = covStartIndex;
   UnsignedInt idx = 0;
   for (UnsignedInt satIdx = 0U; satIdx < sats.size(); satIdx++)
   {
      if (sats[satIdx]->GetType() != Gmat::FORMATION)
      {
         // Publish covariance
         // Find esm index of S/C
         UnsignedInt esmIdx = 0;
         for (esmIdx = 0; esmIdx < map->size(); esmIdx++)
         {
            if ((*map)[esmIdx]->objectName == sats[satIdx]->GetName() &&
               (((*map)[esmIdx]->elementName == "CartesianState") ||
               ((*map)[esmIdx]->elementName == "KeplerianState")))
               break;
         }

         if (esmIdx < map->size())
         {
            for (UnsignedInt ii = 0; ii < 6; ii++)
            {
               for (UnsignedInt jj = 0; jj <= ii; jj++)
               {
                  //pubdata[dim + 1 + idx] = covCart(esmIdx + ii, esmIdx + jj);

                  if (theDim + idx >= pubdataSize)
                  {
                     std::stringstream ss;
                     ss << "Error: data overflow in RunEstimator::PublishState() function."
                        << " When GMAT fills covariance data to pubdata.\n";
                     throw GmatBaseException(ss.str());
                  }
                  pubdata[theDim + idx] = covCart(esmIdx + ii, esmIdx + jj);
                  idx++;
               }
            }
         }
         // Only publish info of spacecrafts in estimation state manager only
         //else
         //{
         //   // Fill data for missing S/C with zeros
         //   for (UnsignedInt ii = 0; ii < 21; ii++)
         //   {
         //      pubdata[dim + 1 + idx] = 0;
         //      idx++;
         //   }
         //}
      }
   }

   for (UnsignedInt satIdx = 0U; satIdx < sats.size(); satIdx++)
   {
      if (sats[satIdx]->GetType() != Gmat::FORMATION)
      {
         // 3.1. Publish covariance
         // Find esm index of S/C
         UnsignedInt esmIdx = 0;
         for (esmIdx = 0; esmIdx < map->size(); esmIdx++)
         {
            if ((*map)[esmIdx]->objectName == sats[satIdx]->GetName() && 
               (((*map)[esmIdx]->elementName == "CartesianState") || 
               ((*map)[esmIdx]->elementName == "KeplerianState")))
               break;
         }

         if (esmIdx < map->size())
         {
            // Publish acceleration
            // Specify acceleration in spacecraft's coordinate origin MJ2000Eq coordinate system
            Rvector3 accel = esm->GetAccelerationOfSpacecraft(sats[satIdx]);

            #ifdef DEBUG_PUBLISH_STATE_DATA
            MessageInterface::ShowMessage(" accel = [%.15le   %.15le   %.15le]\n",
               accel[0], accel[1], accel[2]);
            #endif

            for (Integer k = 0; k < 3; ++k)
            {
               if (theDim + idx >= pubdataSize)
               {
                  std::stringstream ss;
                  ss << "Error: data overflow in RunEstimator::PublishState() function."
                     << " When GMAT fills acceleration data to pubdata.\n";
                  throw GmatBaseException(ss.str());
               }

               pubdata[theDim + idx] = accel[k];
               ++idx;
            }
         }
      }
   }

}



//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Solver commands have one of the following syntaxes:
 *
 *    RunEstimator Bat {SolveMode = Solve}
 *	  
 *	  RunEstimator Bat {SolveMode = RunInitialGuess}
 *    
 *
 * This method breaks the script line into the corresponding pieces, and stores
 * the name of the Solver and all specified solver options.
 *
 * @return true on successful parsing of the command.
 */
 //------------------------------------------------------------------------------
bool RunEstimator::InterpretAction()
{
		StringArray blocks = parser.DecomposeBlock(generatingString);

		StringArray chunks = parser.SeparateBrackets(blocks[0], "{}", " ", false);

	#ifdef DEBUG_PARSING
		MessageInterface::ShowMessage("Chunks from \"%s\":\n",
			blocks[0].c_str());
		for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
			MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
	#endif

		if (chunks.size() < 2)
			throw CommandException(typeName + "::InterpretAction() cannot identify "
				"the Solver -- is it missing? -- in line\n" + generatingString);

		if (chunks.size() > 3)
			throw CommandException(typeName +
				"::InterpretAction() found too many components to parse in the "
				"line\n" + generatingString);

		if (chunks[0] != typeName)
			throw CommandException(typeName + "::InterpretAction() does not identify "
				"the correct Solver type in line\n" + generatingString);

		solverName = chunks[1];

		if (chunks.size() == 3)
			CheckForOptions(chunks[2]);

	#ifdef DEBUG_PARSING
		MessageInterface::ShowMessage("%s::InterpretAction for \"%s\", type = %s\n",
			typeName.c_str(), generatingString.c_str(), typeName.c_str());
		MessageInterface::ShowMessage("   Solver name:     \"%s\"\n",
			solverName.c_str());
		MessageInterface::ShowMessage("   SolveMode:          %d\n", startMode);
		MessageInterface::ShowMessage("   ExitMode:           %d\n", exitMode);
	#endif

		return true;
}


//------------------------------------------------------------------------------
// void CheckForOptions(std::string &opts)
//------------------------------------------------------------------------------
void RunEstimator::CheckForOptions(std::string &opts)
{
	StringArray chunks = parser.SeparateBrackets(opts, "{}", ", ", true);

	#ifdef DEBUG_PARSING
		MessageInterface::ShowMessage("Chunks from \"%s\":\n", opts.c_str());
		for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
			MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
	#endif

	for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
	{
		StringArray option = parser.SeparateBy(*i, "= ");

		#ifdef DEBUG_PARSING
				MessageInterface::ShowMessage("Options from \"%s\":\n", i->c_str());
				for (StringArray::iterator i = option.begin(); i != option.end(); ++i)
					MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
		#endif

		if (option.size() != 2)
			throw CommandException(typeName + "::InterpretAction() Solver option "
				"is not in the form option = value in line\n" +
				generatingString);

		if (option[0] == "SolveMode")
		{
			if (option[1] == "Solve")
				solveMode = "Solve";
			else if (option[1] == "RunInitialGuess")
				solveMode = "RunInitialGuess";
			else
				throw CommandException(typeName + "::InterpretAction() Solver "
					"SolveMode option " + option[1] +
					" is not a recognized value on line\n" + generatingString +
					"\nAllowed values are \"Solve\" and \"RunInitialGuess\"\n");
		}
		else
		{
			throw CommandException(typeName +
				"::InterpretAction() Solver option " + option[0] +
				" is not a recognized option on line\n" + generatingString +
				"\nAllowed options are \"SolveMode\" and "
				"\"ExitMode\"\n");
		}
	}
}
