//$Id$
//------------------------------------------------------------------------------
//                         SmootherBase
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 * Base class for smoother
 */
//------------------------------------------------------------------------------

#include "SmootherBase.hpp"

#include "GmatConstants.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "FileUtil.hpp"
#include "EstimatorException.hpp"
#include "Solver.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"

// EKF mod 12/16
#include "GroundstationInterface.hpp"
#include "Spacecraft.hpp" 
#include "SpaceObject.hpp"
#include "StringUtil.hpp"
#include "Rmatrix66.hpp"
#include "StateConversionUtil.hpp"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_REPORTS

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DUMP_RESIDUALS
//#define DEBUG_EVENT
//#define DEBUG_RUNCOMPLETE


const std::string
SmootherBase::PARAMETER_TEXT[] =
{
   "Filter",                  // The filter to use
   "DelayRectifyTimeSpan",    // The duration to delay rectifying the reference trajectory in the filter
   "MeasDeweightingSigmaThreshold", // The sigma threshold to use in the filter on the sqare root of the trace of the position elements of the covariance
   "MeasDeweightingCoefficient",    // The coefficient used in the filter for underweighting during the measurement update
};

const Gmat::ParameterType
SmootherBase::PARAMETER_TYPE[] =
{
   Gmat::OBJECT_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};

//------------------------------------------------------------------------------
// SmootherBase(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
SmootherBase::SmootherBase(const std::string &type, const std::string &name) :
   Estimator               (type, name),
   filter                  (NULL),
   filterName              (""),
   filterIndex             (0),
   delayFilterRectifySpan  (0.0),
   deweightThresholdFilter (1.0),
   deweightCoeffFilter     (0.0),
   smootherState           (FILTERING)
{
   objectTypeNames.push_back("SmootherBase");
   parameterCount = SmootherBaseParamCount;
}


//------------------------------------------------------------------------------
// ~SmootherBase()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SmootherBase::~SmootherBase()
{
   if (filter)
      delete filter;
}


//------------------------------------------------------------------------------
// SmootherBase::SmootherBase(const SmootherBase & sb) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param se The base smoother that provides configuration information for this one
 */
//------------------------------------------------------------------------------
SmootherBase::SmootherBase(const SmootherBase & sb) :
   Estimator               (sb),
   filter                  (sb.filter),
   filterName              (sb.filterName),
   filterIndex             (sb.filterIndex),
   delayFilterRectifySpan  (sb.delayFilterRectifySpan),
   deweightThresholdFilter (sb.deweightThresholdFilter),
   deweightCoeffFilter     (sb.deweightCoeffFilter),
   smootherState           (sb.smootherState)
{
   if (sb.filter)
      filter = (SeqEstimator*)sb.filter->Clone();
   else
      filter = NULL;
}


//------------------------------------------------------------------------------
// SmootherBase& operator=(const SmootherBase &sb)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sb The base smoother that provides configuration information for this one
 *
 * @return This base smoother, configured to match sb
 */
//------------------------------------------------------------------------------
SmootherBase& SmootherBase::operator=(
      const SmootherBase &sb)
{
   if (this != &sb)
   {
      Estimator::operator=(sb);

      if (sb.filter)
         filter = (SeqEstimator*)sb.filter->Clone();
      else
         filter = NULL;

      filterName        = sb.filterName;
      filterIndex       = sb.filterIndex;
      delayFilterRectifySpan = sb.delayFilterRectifySpan;
      deweightThresholdFilter = sb.deweightThresholdFilter;
      deweightCoeffFilter = sb.deweightCoeffFilter;
      smootherState     = sb.smootherState;
   }
   return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetParameterText(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SmootherBaseParamCount)
      return PARAMETER_TEXT[id - EstimatorParamCount];
   return Estimator::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetParameterUnit(const Integer id) const
{
   return Estimator::GetParameterUnit(id); // TBD
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str String for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer SmootherBase::GetParameterID(const std::string &str) const
{
   // This section is used to throw an exception for unused parameters
   if (str == "MaximumIterations")
      throw SolverException("Syntax error: '" + GetName() + "' does not has parameter '" + str + "'.\n");

   for (Integer i = EstimatorParamCount; i < SmootherBaseParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EstimatorParamCount])
         return i;
   }

   return Estimator::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SmootherBase::GetParameterType(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SmootherBaseParamCount)
      return PARAMETER_TYPE[id - EstimatorParamCount];

   return Estimator::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method is used to specify a parameter is read only or not.
 *
 * @param id ID for the requested parameter.
 *
 * @return   true if parameter read only, false otherwise.
 */
//------------------------------------------------------------------------------
bool SmootherBase::IsParameterReadOnly(const Integer id) const
{
   if (id == maxIterationsID)
      return true;

   if (id == ESTIMATION_EPOCH_FORMAT)
      return true;

   if (id == ESTIMATION_EPOCH)
      return true;

   if (id == PREDICT_TIME_SPAN)
      return false;

   if (id == ADD_PREDICT_TO_MATLAB)
      return false;

   return Estimator::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
 //---------------------------------------------------------------------------
bool SmootherBase::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetParameterTypeString(const Integer id) const
{
   return Estimator::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
Real SmootherBase::GetRealParameter(const Integer id) const
{
   if (id == DELAY_FILTER_RECTIFY_TIME)
      return delayFilterRectifySpan;

   if (id == MEAS_DEWEIGHT_THRESHOLD_FILTER)
      return deweightThresholdFilter;

   if (id == MEAS_DEWEIGHT_COEFF_FILTER)
      return deweightCoeffFilter;

   return Estimator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the parameter whose value to change.
 * @param value Value for the parameter.
 *
 * @return  Real value of the requested parameter.
 */
 //------------------------------------------------------------------------------
Real SmootherBase::SetRealParameter(const Integer id, const Real value)
{
   if (id == DELAY_FILTER_RECTIFY_TIME)
   {
      if (value >= 0.0)
         delayFilterRectifySpan = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return delayFilterRectifySpan;
   }

   if (id == MEAS_DEWEIGHT_THRESHOLD_FILTER)
   {
      if (value >= 0.0)
         deweightThresholdFilter = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return deweightThresholdFilter;
   }

   if (id == MEAS_DEWEIGHT_COEFF_FILTER)
   {
      if (value >= 0.0)
         deweightCoeffFilter = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return deweightCoeffFilter;
   }

   return Estimator::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  Integer GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value of an real parameter specified by parameter name.
*
* @param label    name of parameter.
*
* @return         value of a real parameter.
*/
//------------------------------------------------------------------------------
Real SmootherBase::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer SetRealParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets value to a real parameter specified by the input parameter name.
*
* @param label    name for the requested parameter.
* @param value    real value used to set to the request parameter.
*
* @return value set to the requested parameter.
*/
//------------------------------------------------------------------------------
Real SmootherBase::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The integer ID for the parameter
 *
 * @return The string assigned to the parameter
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetStringParameter(const Integer id) const
{
   if (id == FILTER)
      return filterName;

   return Estimator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param id The integer ID for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SmootherBase::SetStringParameter(const Integer id,
         const std::string &value)
{
   if (id == FILTER)
   {
      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Filter is an invalid GMAT object name.\n");

      filterName = value;
      return true;
   }

   return Estimator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The text label for the parameter
 *
 * @return The string assigned to the parameter
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The text label for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SmootherBase::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param id ID for the parameter.
 *
 * @return parameter ObjectType
 */
 //---------------------------------------------------------------------------
UnsignedInt SmootherBase::GetPropertyObjectType(const Integer id) const
{
   if (id == FILTER)
      return GmatType::GetTypeId("SeqEstimator");

   return Solver::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool SmootherBase::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
 //------------------------------------------------------------------------------
const ObjectTypeArray& SmootherBase::GetRefObjectTypeArray()
{
   static ObjectTypeArray objTypes = Estimator::GetRefObjectTypeArray();
   objTypes.push_back(GmatType::GetTypeId("SeqEstimator"));
   return objTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Initialization method that identifies the reference objects needed
 *
 * @param type The ObjectType for the references; UNKNOWN_OBJECT retrieves all
 *
 * @return A StringArray with all of the object names.
 */
 //------------------------------------------------------------------------------
const StringArray& SmootherBase::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == GmatType::GetTypeId("SeqEstimator")))
   {
      if (find(refObjectList.begin(), refObjectList.end(),
         filterName) == refObjectList.end())
         refObjectList.push_back(filterName);
   }

   return refObjectList;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//------------------------------------------------------------------------------
/**
 * Renames references objects
 *
 * @param type The type of object that is renamed
 * @param oldName The name of the object that is changing
 * @param newName the new object name
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool SmootherBase::RenameRefObject(const UnsignedInt type,
   const std::string & oldName, const std::string & newName)
{
   if (type == GmatType::GetTypeId("SeqEstimator"))
   {
      if (filterName == oldName)
         filterName = newName;
   }

   return Estimator::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object's name
 *
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool SmootherBase::SetRefObjectName(const UnsignedInt type,
   const std::string & name)
{
   if (type == GmatType::GetTypeId("SeqEstimator"))
   {
      filterName = name;
      return true;
   }

   return Estimator::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a referenced object of a given type
 *
 * @param type The object's type
 *
 * @return The name of the associated object
 */
 //------------------------------------------------------------------------------
std::string SmootherBase::GetRefObjectName(const UnsignedInt type) const
{
   if (type == GmatType::GetTypeId("SeqEstimator"))
      return filterName;

   return Estimator::GetRefObjectName(type);
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
GmatBase* SmootherBase::GetRefObject(const UnsignedInt type,
   const std::string & name)
{
   if (type == GmatType::GetTypeId("SeqEstimator"))
      return filter;

   return Estimator::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool SmootherBase::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string & name)
{
   if (name == filterName)
   {
      if (obj->IsOfType("SeqEstimator"))
      {
         if (filter)
            delete filter;

         filter = (SeqEstimator*) obj->Clone();
         return true;
      }
   }

   return Estimator::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Implements the basic smoother state machine
 *
 * This method changes the SmootherBase's finite state based on the
 * status of the elements involved in the estimation process by calling the
 * state machine to process one step in its implementation
 *
 * @return The state at the end of the state machine step.
 */
//------------------------------------------------------------------------------
Solver::SolverState SmootherBase::AdvanceState()
{
   filter->UpdateCurrentEpoch(currentEpochGT);

   switch (currentState)
   {
      case INITIALIZING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SmootherBase::Executing the INITIALIZING state\n");
         #endif
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SmootherBase::Executing the PROPAGATING state\n");
         #endif
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the CALCULATING state\n");
         #endif
         CalculateData();
         break;

      case ESTIMATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SmootherBase::Executing the ESTIMATING state\n");
         #endif
         Estimate();
         break;

      case CHECKINGRUN:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SmootherBase::Executing the CHECKINGRUN state\n");
         #endif
         CheckCompletion();

         // Add .mat data
         if (matWriter != NULL)
            if (!AddMatData(matData))
               throw EstimatorException("Error adding .mat data file");
         break;

      case FINISHED:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SmootherBase::Executing the FINISHED state\n");
         #endif
         RunComplete();
         break;

      default:
         throw EstimatorException("Unknown state encountered in the " +
               instanceName + " smoother.");

   }

   StateCleanUp();

   return currentState;
}

//------------------------------------------------------------------------------
// void StateCleanUp()
//------------------------------------------------------------------------------
/**
 * Performs some clean up actions for AdvanceState()
 */
 //------------------------------------------------------------------------------
void SmootherBase::StateCleanUp()
{
   if (smootherState == PREDICTING)
   {
      (*(stateCovariance->GetCovariance())) = (*(filter->stateCovariance->GetCovariance()));
      informationInverse = (*(stateCovariance->GetCovariance()));
      //information = informationInverse.Inverse(std::numeric_limits<double>::epsilon());
   }

   if (smootherState == SMOOTHING && currentState == CHECKINGRUN)
   {
      if (predictTimeSpan != 0.0 && !isPredicting)
      {
         // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
         currentSolveForState = esm.GetEstimationStateForReport();
         currentSolveForStateC = esm.GetEstimationCartesianStateForReport();
         currentSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

         // Get Covariance
         informationInverse = (*(stateCovariance->GetCovariance()));
         // TODO: for small covariances, this thinks it's singular
         //information = informationInverse.Inverse(std::numeric_limits<double>::epsilon());

         /// Calculate conversion derivative matrixes
         // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
         cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
         // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
         solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();

         WriteToTextFile(FINISHED);
         AddMatlabConfigData();
         ReportProgress(FINISHED);

         PrepareFilter();

         BeginPredicting(predictTimeSpan);
         filter->TakeAction("RunForwards");
         filter->UpdateCurrentEpoch(currentEpochGT);
         filter->SetAnchorEpoch(forwardFilterInfo[0].epoch, false);
         filter->BeginPredicting(predictTimeSpan);
         currentState = PROPAGATING;
         smootherState = PREDICTING;

         filter->FindTimeStep();
         filter->SetCovariance(*(stateCovariance->GetCovariance()));
         filter->SetNominalValues();
      }
   }
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string & action, const std::string & actionData)
//------------------------------------------------------------------------------
/**
 * This method implements a custom action
 *
 * @param action The action requested
 * @param actionData Optional additional data required by the action
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SmootherBase::TakeAction(const std::string & action,
      const std::string & actionData)
{
   return Estimator::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the Estimator
*
* The transient force vector is a set of models used in GMAT's ODEModel for
* effects that are turned on and off over the course of a mission.  An example
* of a transient force is a finite burn, which is toggled by the
* BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
* by commands that need them.  Typical usage is found in the propagation
* enabled commands.
*
* @param tf The vector of transient forces
*/
//------------------------------------------------------------------------------
void SmootherBase::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   Estimator::SetTransientForces(tf);
   filter->SetTransientForces(tf);

   filter->GetMeasurementManager()->SetTransientForces(tf);
}


//------------------------------------------------------------------------------
//  std::vector<Real *>* GetForceDerivativeCache()
//------------------------------------------------------------------------------
/**
* Get the vector containing the derivatives of each force in the ODEModel
*
* This vector is used by SeqEstimators for calculating the process noise for
* EstimatedParameter objects, which need to know the accelerations due to
* their related force models
*
* @return The vector that contains the force derivatives
*/
//------------------------------------------------------------------------------
std::map<std::string, std::vector<Real *>>* SmootherBase::GetForceDerivativeCache()
{
   if (smootherState == FILTERING || smootherState == PREDICTING)
      return filter->GetForceDerivativeCache();

   return &forceDerivativeCache;
}


//------------------------------------------------------------------------------
//  SeqEstimator* GetFilter()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the SeqEstimator object.
 *
 * @return The SeqEstimator pointer
 */
 //------------------------------------------------------------------------------
SeqEstimator* SmootherBase::GetFilter()
{
   return filter;
}


//------------------------------------------------------------------------------
//  void PrepareFilter()
//------------------------------------------------------------------------------
/**
 * Prepares the filter used by the smoother.
 */
 //------------------------------------------------------------------------------
void SmootherBase::PrepareFilter()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SmootherBase::PrepareFilter() enter\n");
#endif

   EstimationStateManager *esmFilter = filter->GetEstimationStateManager();
   GmatState estimationStateFilterS = esmFilter->GetEstimationMJ2000EqCartesianStateForReport();

   // Reset state to estimation epoch
   if (forwardFilterInfo.size() == 0)
      throw EstimatorException("Error: There is no forward filter information"
         " because no EKF estimation has been run before running RunSmoother\n");
   SeqEstimator::UpdateInfoType lastState = forwardFilterInfo.back();

   estimationEpochGT = lastState.epoch;
   currentEpochGT = lastState.epoch;
   estimationStateFilterS.SetPrecisionTimeFlag(true);
   estimationStateFilterS.SetEpoch(currentEpochGT.GetMjd());
   estimationStateFilterS.SetEpochGT(currentEpochGT);

   UnsignedInt stateSizeFilter = esmFilter->GetState()->GetSize();
   for (UnsignedInt i = 0; i < stateSizeFilter; ++i)
      estimationStateFilterS[i] = lastState.state[i];
   
   // Convert estimation state from Keplerian to Cartesian if needed
   esmFilter->SetEstimationMJ2000EqCartesianStateParticipant(estimationStateFilterS);
   
   for (UnsignedInt i = 0; i < propagators.size(); ++i)
   {
      PropagationStateManager *psm = propagators[i]->GetPropStateManager();
      psm->MapObjectsToVector();
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SmootherBase::PrepareFilter() exit\n");
#endif
}


//------------------------------------------------------------------------------
//  void SetForwardFilterInfo(std::vector<SeqEstimator::UpdateInfoType> filterInfo)
//------------------------------------------------------------------------------
/**
 * Passes the filter info from the forward filter pass to the smoother
 *
 * @param filterInfo The filter info to set
 */
 //------------------------------------------------------------------------------
void SmootherBase::SetForwardFilterInfo(std::vector<SeqEstimator::UpdateInfoType> filterInfo)
{
   forwardFilterInfo = filterInfo;
   filter->forwardFilterInfo = &forwardFilterInfo;
}


//------------------------------------------------------------------------------
//  void SetObjectBuffer(const ObjectArray &buffer)
//------------------------------------------------------------------------------
/**
 * Loads the buffer of objects from the filter's EstimationStateManager at
 * the final epoch. THis will then be used to restore the buffer during
 * CompleteInitialization()
 *
 * @param buffer The buffered objects
 */
 //------------------------------------------------------------------------------
void SmootherBase::SetObjectBuffer(const ObjectArray &buffer)
{
   esmObjBuffer = buffer;
}


//------------------------------------------------------------------------------
//  Real GetTimeStep()
//------------------------------------------------------------------------------
/**
 * Returns the time step of the estimator.
 *
 * @return The time step
 */
 //------------------------------------------------------------------------------
Real SmootherBase::GetTimeStep()
{
   if (smootherState == FILTERING || smootherState == PREDICTING)
   {
      try
      {
         return filter->GetTimeStep();
      }
      catch (BaseException &ex)
      {
         if (smootherState == FILTERING)
            throw BackFilterExceptionMessage(ex);
         else
            throw;
      }
   }
   else
      return Estimator::GetTimeStep();
}


//------------------------------------------------------------------------------
// bool ResetState()
//------------------------------------------------------------------------------
/**
 * This method returns the resetState flag, turning it off in the process
 *
 * @return The resetState flag
 */
 //------------------------------------------------------------------------------
bool SmootherBase::ResetState()
{
   // Need to evaluate both of the following functions, cannot short-circuit or
   bool filterReset = filter->ResetState();
   bool thisReset = Estimator::ResetState();

   bool retval = filterReset || thisReset;

   return retval;
}


//------------------------------------------------------------------------------
// void MoveToNext(bool includeUpdate)
//------------------------------------------------------------------------------
/**
 * Moves the spacecraft to the next smoothing point and update the state and
 * covariance
 *
 * @param includeUpdate Flag to indicate if the measurement update information
 *                      should be used when moving to the next smoothing point,
 *                      if false, the pre-update values for state and
 *                      covariance are used
 */
 //------------------------------------------------------------------------------
void SmootherBase::MoveToNext(bool includeUpdate)
{
   SmootherInfoType smootherStat;
   SmoothState(smootherStat, includeUpdate);

   // Update the epoch
   estimationStateS = esm.GetEstimationMJ2000EqCartesianStateForReport();
   currentEpochGT = smootherStat.epoch;
   estimationStateS.SetPrecisionTimeFlag(true);
   estimationStateS.SetEpoch(currentEpochGT.GetMjd());
   estimationStateS.SetEpochGT(currentEpochGT);

   // Update the state, covariances, and so forth
   for (UnsignedInt i = 0; i < stateSize; ++i)
      estimationStateS[i] = smootherStat.state[i];

   // Convert estimation state from Keplerian to Cartesian
   esm.SetEstimationMJ2000EqCartesianStateParticipant(estimationStateS);

   Rmatrix cov = smootherStat.cov;
   CovarianceEpsilonConversionReverse(cov);
   (*(stateCovariance->GetCovariance())) = cov;

   Symmetrize(*stateCovariance);
   informationInverse = (*(stateCovariance->GetCovariance()));
   //information = informationInverse.Inverse(COV_INV_TOL);

   esm.MapVectorToObjects();
   esm.MapCovariancesToObjects();

   for (UnsignedInt i = 0; i < propagators.size(); ++i)
   {
      PropagationStateManager *psm = propagators[i]->GetPropStateManager();
      psm->MapObjectsToVector();
   }
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
bool SmootherBase::HasLocalClones()
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
void SmootherBase::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("SeqEstimator"))
   {
      if (filter && (obj->GetName() == filterName))
      {
         filter->operator=((*(SeqEstimator*)obj));
         if (isInitialized)
            isInitialized = filter->IsInitialized();
      }

      return;
   }

   Estimator::UpdateClonedObject(obj);
}


//------------------------------------------------------------------------------
// SmootherBase::SmootherState GetSmootherState()
//------------------------------------------------------------------------------
/**
 * Determine the smoother state of this instance of the SmootherBase.
 *
 * @return current smoother state 
 */
 //------------------------------------------------------------------------------
SmootherBase::SmootherState SmootherBase::GetSmootherState()
{
   return smootherState;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the estimator for use by setting and checking reference object
 * pointers
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SmootherBase::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SmootherBase::Initialize() "
            "entered\n");
   #endif
   bool retval = false;

   // Pre-initialize data from filter for the smoother

   if (filter == NULL)
      throw EstimatorException("Error: Filter for \"" + GetName() + "\" is not set\n");

   // Copy some parameters from smoother to filter
   filter->SetRealParameter("MeasDeweightingSigmaThreshold", deweightThresholdFilter);
   filter->SetRealParameter("MeasDeweightingCoefficient", deweightCoeffFilter);

   // Set file names for the backward filter
   filter->SetStringParameter("OutputWarmStartFile", "");
   if (solverTextFile != "")
   {
      std::string filterFile = GmatFileUtil::ParsePathName(solverTextFile) + GmatFileUtil::ParseFileName(solverTextFile, true) +
                               "_backfilter" + GmatFileUtil::ParseFileExtension(solverTextFile, true);
      filter->SetStringParameter("ReportFile", filterFile);
   }
   if (matFileName != "")
   {
      std::string filterMatFile = GmatFileUtil::ParsePathName(matFileName) + GmatFileUtil::ParseFileName(matFileName, true) +
                                  "_backfilter" + GmatFileUtil::ParseFileExtension(matFileName, true);
      filter->SetStringParameter("MatlabFile", filterMatFile);
   }

   // Don't write these files if not in Verbose or Debug
   if (textFileMode != "Verbose" && textFileMode != "Debug")
      filter->TakeAction("NoOutput");

   filter->Initialize();

   if (filter->IsInitialized())
   {
      filter->TakeAction("ResetInstanceCount");
      filter->TakeAction("IncrementInstanceCount");

      propagatorNames = filter->GetStringArrayParameter("Propagator");
      propagatorSatMap = filter->GetPropSatMap();

      // Propagators are set/cloned in Estimator base class; why clone here?
      /// @todo  Smoothers don't use propagators right now, but if they do in the
      /// future, an update will be necessary here.
      std::map<std::string, PropSetup*> filterSatPropMap = filter->GetSatPropMap();

      if (filterSatPropMap.size() == 0)
      {
         filter->BuildSatPropMap();
         filterSatPropMap = filter->GetSatPropMap();
      }

      bool foundInList = false;
      std::string propName;
      for (std::map<std::string, PropSetup*>::iterator it = filterSatPropMap.begin(); it != filterSatPropMap.end(); ++it)
      {
         for (int j = 0; j < propagators.size(); j++)
         {
            propName = ((PropSetup*)filter->GetPropagator(it->first))->GetName();
            if (propName == propagators[j]->GetName())
               foundInList = true;
         }
         if (!foundInList)
            propagators.push_back((PropSetup*)(filter->GetPropagator(it->first)->Clone()));
         foundInList = false;
      }

      BuildSatPropMap();

      measurementNames = filter->GetStringArrayParameter("Measurements");
      measManager = *(filter->GetMeasurementManager());
      esm = *(filter->GetEstimationStateManager());
      dataFilterStrings = filter->GetStringArrayParameter("DataFilters");
   }

   if (Estimator::Initialize())
   {
      retval = true;

      if (matFileName != "")
      {
         if (matWriter != NULL)
         {
            matSmootherData.SetInitialRealValue(NAN);
            matSmootherData.Clear();
         }
      }
   }

   return retval;
}


bool SmootherBase::Reinitialize()
{
   Estimator::Reinitialize();
   filter->Reinitialize();

   return true;
}


void SmootherBase::SetDelayInitialization(bool delay)
{
   Estimator::SetDelayInitialization(delay);
   filter->SetDelayInitialization(delay);
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Cleans up the estimator after a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SmootherBase::Finalize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SmootherBase::Finalize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Finalize())
   {
      retval = true;
      measManager.Finalize();  // EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SmootherBase::Finalize() measManager.Finalize() finished\n");
#endif
      esm.MapVectorToObjects();  // EKF mod 12/16
      esm.MapCovariancesToObjects();
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SmootherBase::Finalize() esm.MapVectorToObjects() finished\n");
#endif
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SmootherBase::Finalize() "
         "finished\n");
#endif
   return retval;
}


//------------------------------------------------------------------------------
//  bool IsFinalPass()
//------------------------------------------------------------------------------
/**
 * Returns if this is the final propagation pass through the Estimator
 *
 * @return true if this is the final pass, false otherwise
 */
 //------------------------------------------------------------------------------
bool SmootherBase::IsFinalPass()
{
   bool retval = smootherState == SMOOTHING || smootherState == PREDICTING;
   return retval;
}


//------------------------------------------------------------------------------
// bool UpdateInitialConditions()
//------------------------------------------------------------------------------
/**
 * Check if the initial conditions of the estimator need to be updated,
 * and update them if needed.
 *
 * @return true if the initial conditions were changed, false otherwise
 */
 //------------------------------------------------------------------------------
bool SmootherBase::UpdateInitialConditions()
{
   bool retval = false;

   if (forwardFilterInfo.size() == 0U)
   {
      throw EstimatorException("The Smoother does not have any data from a forward filter. "
         "This may be a result of not using the \"RunSmoother\" command.");
   }

   Real covarianceIncrease = 1e10;

   // Set covariance to match the filter's final covariance
   Rmatrix cov = forwardFilterInfo.back().cov;
   CovarianceEpsilonConversionReverse(cov);
   *(esm.GetCovariance()->GetCovariance()) = cov;
   SetNominalValues();

   // Grab covariance again (this time for backward filter) and scale
   cov = forwardFilterInfo.back().cov * covarianceIncrease;

   for (UnsignedInt ii = 0; ii < cov.GetNumRows(); ii++)
      for (UnsignedInt jj = 0; jj < cov.GetNumColumns(); jj++)
         if (ii != jj)
            cov(ii, jj) = 0;

   covarianceIncrease = 1e4;

   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   for (UnsignedInt ii = 0; ii < cov.GetNumRows(); ii++)
      if (((*stateMap)[ii]->elementName != "CartesianState") &&
            ((*stateMap)[ii]->elementName != "KeplerianState") &&
            ((*stateMap)[ii]->elementName != "Position"))
         cov(ii, ii) = forwardFilterInfo.back().cov(ii, ii) * covarianceIncrease;

   filter->CovarianceEpsilonConversionReverse(cov);
   filter->SetCovariance(cov);

   retval = filter->SetNominalValues();

   return retval;
}

//------------------------------------------------------------------------------
// void SmootherBase::UpdateCov()
//------------------------------------------------------------------------------
/**
 *  Updates covariance without updating the state or publishing
 */
 //------------------------------------------------------------------------------
void SmootherBase::UpdateCov()
{
   filter->UpdateCov();
   (*(stateCovariance->GetCovariance())) = (*(filter->stateCovariance->GetCovariance()));
   informationInverse = (*(stateCovariance->GetCovariance()));

   esm.MapCovariancesToObjects();
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Performs final setup prior to use of the smoother
 *
 * This method is called in the INITIALIZING state
 */
 //------------------------------------------------------------------------------
void SmootherBase::CompleteInitialization()
{
   ObjectArray satArray;
   esm.GetStateObjects(satArray, Gmat::SPACECRAFT);

   estimationState = esm.GetState();
   stateSize = estimationState->GetSize();

   Estimator::CompleteInitialization();

   // Set the current epoch based on the first spacecraft in the ESM
   if (satArray.size() == 0)
   {
      throw EstimatorException("Cannot initialize the estimator: there are "
         "no Spacecraft in the estimation state manager");
   }

   estimationEpochGT = ((Spacecraft*)satArray[0])->GetEpochGT();
   currentEpochGT = ((Spacecraft*)satArray[0])->GetEpochGT();

   // Disallow some solve-for variables which are not work in Smoother
   const std::vector<ListItem*>* stateMap = esm.GetStateMap();
   for (UnsignedInt ii = 0; ii < stateMap->size(); ii++)
   {
      if ((*stateMap)[ii]->elementName == "PassBiases")
      {
         throw EstimatorException("Solving for PassBiases is currently disallowed in the smoother");
      }
   }

   // Now load up the observations
   measManager.PrepareForProcessing(false);

   ///// Make more generic?
   measManager.LoadRampTables();

   // Create RIC frame; for now use cb from the first propagator
   PropSetup  *ps = GetPropagator("");
   std::string cb = "Earth";
   if (ps->GetPropagator()->UsesODEModel())
   {
      ODEModel *ode = ps->GetODEModel();
      cb = ode->GetStringParameter("CentralBody");
   }
   else
      cb = ps->GetPropagator()->GetPropOriginName();
   SpacePoint *body = solarSystem->GetBody(cb);

   for (UnsignedInt i = 0; i < satArray.size(); ++i)
   {
      if (vnbFrames.find(satArray[i]) != vnbFrames.end())
         if (vnbFrames[satArray[i]])
            delete vnbFrames[satArray[i]];

      vnbFrames[satArray[i]] = CoordinateSystem::CreateLocalCoordinateSystem("VNB", "VNB", body, body,
         (SpacePoint*)satArray[i], body->GetJ2000Body(), solarSystem);
   }

   // Trim observations prior to start epoch
   bool obsAtFirstEpoch = false;
   bool atFirstEpoch = true;
   UnsignedInt ii = 0U;

   if (forwardFilterInfo.size() == 0U)
   {
      throw EstimatorException("The Smoother does not have any data from a forward filter. "
         "This may be a result of not using the \"RunSmoother\" command.");
   }

   while (atFirstEpoch)
   {
      obsAtFirstEpoch = obsAtFirstEpoch || forwardFilterInfo[ii].isObs;

      if (obsAtFirstEpoch)
         break; // Don't need to keep checking

      ii++; // Go to next item

      if (ii == forwardFilterInfo.size())
         break; // Exit, we've reached the end of the vector

      atFirstEpoch = GmatMathUtil::IsEqual(forwardFilterInfo[0].epoch, forwardFilterInfo[ii].epoch, ESTTIME_ROUNDOFF);
   }

   TrimObsByEpoch(forwardFilterInfo[0].epoch, !obsAtFirstEpoch);

   // Restore the objects from the buffer
   if (esmObjBuffer.size() > 0U)
   {
      esm.RestoreObjects(&esmObjBuffer);

      // Overwrite buffer solve-for states with those from the filter info
      PrepareFilter();
      UpdateInitialConditions();
   }

   esm.MapObjectsToVector();
   esm.MapCovariancesToObjects();

   estimationStateS = esm.GetEstimationState();

   aprioriSolveForState = esm.GetEstimationStateForReport();  // EKF mod 12/16
   aprioriSolveForStateMA = esm.GetEstimationStateForReport("MA");
   aprioriSolveForStateC = esm.GetEstimationCartesianStateForReport();
   aprioriSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   //cart2SolvMatrixPrev = cart2SolvMatrix;
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
   //solv2KeplMatrixPrev = solv2KeplMatrix;


   //Mod to match filter
   pcolumnLen = 26;

   std::vector<StringArray> signalPaths = measManager.GetSignalPathList();
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      Integer len = 0;
      for (UnsignedInt j = 0; j < signalPaths[i].size(); ++j)
      {
         try
         {
            GmatBase* obj = GetConfiguredObject(signalPaths[i].at(j));
            std::string id = "";
            if (obj->IsOfType(Gmat::SPACECRAFT))
               id = ((Spacecraft*)obj)->GetStringParameter("Id");
            else if (obj->IsOfType(Gmat::GROUND_STATION))
               id = ((GroundstationInterface*)obj)->GetStringParameter("Id");

            len = len + id.size() + 1;
         }
         catch (...)
         {
            // swallow the exception
         }
      }
      if (pcolumnLen < len)
      {
         pcolumnLen = len;
      }
   }
   pcolumnLen += 3;



   WriteToTextFile();
   ReportProgress();

   // Some special configuration for backwards filter
   filter->TakeAction("RunBackwards");
   filter->TakeAction("UseProvidedFlags");
   filter->SetRealParameter("DelayRectifyTimeSpan", delayFilterRectifySpan);

   bool isTrimmed = false;
   // Use edit flags from forward filter for backward filter and smoother
   for (UnsignedInt ii = 0U; ii < forwardFilterInfo.size(); ii++)
   {
      if (forwardFilterInfo[ii].isObs)
      {
         if (!isTrimmed)
         {
            filter->TrimObsByEpoch(forwardFilterInfo[ii].epoch, false);
            TrimObsByEpoch(forwardFilterInfo[ii].epoch, false);
            isTrimmed = true;
         }

         Integer recNum = forwardFilterInfo[ii].measStat.recNum;
         Integer editFlag = forwardFilterInfo[ii].measStat.editFlag;
         std::string removedReason = forwardFilterInfo[ii].measStat.removedReason;

         filter->GetMeasurementManager()->GetObsDataObject(recNum)->inUsed = (editFlag == NORMAL_FLAG);
         filter->GetMeasurementManager()->GetObsDataObject(recNum)->removedReason = removedReason;

         GetMeasurementManager()->GetObsDataObject(recNum)->inUsed = (editFlag == NORMAL_FLAG);
         GetMeasurementManager()->GetObsDataObject(recNum)->removedReason = removedReason;
      }
   }

   // Complete initialization of backwards filter
   filter->CompleteInitialization();
   filter->SetAnchorEpoch(forwardFilterInfo[0].epoch, true);
   filter->TrimObsByEpoch(forwardFilterInfo[0].epoch, false);
   filter->StateCleanUp();
   currentState = filter->GetState();
}


//------------------------------------------------------------------------------
// void FindTimeStep()
//------------------------------------------------------------------------------
/**
 * Calculates the time step to the next measurement
 *
 * This method is called in the PROPAGATING state
 */
 //------------------------------------------------------------------------------
void SmootherBase::FindTimeStep()
{
   if (smootherState == FILTERING || smootherState == PREDICTING)
   {
      try
      {
         filter->FindTimeStep();
      }
      catch (BaseException &ex)
      {
         if (smootherState == FILTERING)
            throw BackFilterExceptionMessage(ex);
         else
            throw;
      }

      if (addPredictToMatlab && smootherState == PREDICTING)
      {
         if (filter->ResetState())
         {
            SeqEstimator::UpdateInfoType updateStat;
            filter->FillUpdateInfo(updateStat);

            SmootherInfoType smootherStat;
            smootherStat.epoch = updateStat.epoch;
            smootherStat.isObs = false;
            smootherStat.state = updateStat.state;
            smootherStat.cov.SetSize(updateStat.cov.GetNumRows(), updateStat.cov.GetNumColumns());
            smootherStat.cov = updateStat.cov;
            smootherStat.covVNB.SetSize(updateStat.covVNB.GetNumRows(), updateStat.covVNB.GetNumColumns());
            smootherStat.covVNB = updateStat.covVNB;
            AddMatlabSmootherData(smootherStat);

            resetState = true;
         }
      }

      try
      {
         filter->StateCleanUp();
         currentState = filter->GetState();

         timeStep = filter->GetTimeStep();
      }
      catch (BaseException &ex)
      {
         if (smootherState == FILTERING)
            throw BackFilterExceptionMessage(ex);
         else
            throw;
      }
   }
   else
   {
      if (filterIndex == forwardFilterInfo.size())
      {
         currentState = CHECKINGRUN;
         return;
      }

      if (currentEpochGT == forwardFilterInfo[filterIndex].epoch)
      {
         timeStep = 0;

         if (forwardFilterInfo[filterIndex].isObs)
            currentState = CALCULATING;
         else
         {
            SmootherUpdate();
            filterIndex++;
            timeStep = (forwardFilterInfo[filterIndex].epoch - currentEpochGT).GetTimeInSec();
            currentState = PROPAGATING;
         }
      }
      else
      {
         timeStep = (forwardFilterInfo[filterIndex].epoch - currentEpochGT).GetTimeInSec();
         currentState = PROPAGATING;
      }
   }
}


//------------------------------------------------------------------------------
// void CalculateData()
//------------------------------------------------------------------------------
/**
 * Determines if a measurement is feasible, and if so, prepares the data for use
 * while estimating
 *
 * This method is called in the CALCULATING state
 */
 //------------------------------------------------------------------------------
void SmootherBase::CalculateData()
{
   if (smootherState == FILTERING)
   {
      try
      {
         filter->CalculateData();
         filter->StateCleanUp();
         currentState = filter->GetState();
      }
      catch (BaseException &ex)
      {
         throw BackFilterExceptionMessage(ex);
      }
   }
   else
   {
      // Update the STM
      esm.MapObjectsToSTM();
      esm.MapObjectsToVector();

      // Tell the measurement manager to calculate the simulation data
      if (measManager.CalculateMeasurements() == false)
      {
         currentState = ESTIMATING;
      }
      else if (measManager.GetEventCount() > 0)
      {
         currentState = LOCATING;
         locatingEvent = true;
      }
      else
      {
         currentState = ESTIMATING;
      }
   }
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Performs estimation of the backwards filter pass
 */
 //------------------------------------------------------------------------------
void SmootherBase::Estimate()
{
   if (smootherState == FILTERING)
   {
      try
      {
         filter->Estimate();
         filter->StateCleanUp();
         currentState = filter->GetState();
      }
      catch (BaseException &ex)
      {
         throw BackFilterExceptionMessage(ex);
      }
   }
   else
   {
      SmootherInfoType smootherStat;
      smootherStat.epoch = currentEpochGT;

      // Compute the O-C, Htilde, and Kalman gain

      // Populate measurement statistics
      SmootherMeasurementInfoType measStat;
      CalculateResiduals(measStat);

      
      // Get scaled residuals
      if (modelsToAccess.size() > 0)
      {
         if (measManager.CountFeasibleMeasurements(modelsToAccess[0]) > 0)
         {
             Rmatrix pBar = informationInverse;
             Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());

             // Now populate H
             Rmatrix H(measStat.hAccum.size(), measStat.hAccum[0].size());
             for (UnsignedInt ii = 0; ii < measStat.hAccum.size(); ++ii)
                 for (UnsignedInt jj = 0; jj < measStat.hAccum[ii].size(); ++jj)
                     H(ii, jj) = measStat.hAccum[ii][jj];

             // Keep this line for when we implement the scaled residual for the entire measurement
             // instead of for each element of the measurement:
             // measStat.scaledResid = GmatMathUtil::Sqrt(yi * (H * pBar * H.Transpose() + R).Inverse() * yi);

             // The element-by-element scaled residual calculation:
             for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
             {
                 Rmatrix Rbar = H * pBar * H.Transpose() + R;
                 Real sigmaVal = GmatMathUtil::Sqrt(Rbar(k, k));
                 Real scaledResid = measStat.residual[k] / sigmaVal;
                 measStat.scaledResid.push_back(scaledResid);
             }
         }
      }  // end of if (modelsToAccess.size() > 0)



      BuildMeasurementLine(measStat);
      WriteToTextFile();

      measStats.push_back(measStat);

      smootherStat.isObs = true;
      smootherStat.measStat = measStat;
      SmoothState(smootherStat, true);
      AddMatlabData(smootherStat.measStat);

      // Advance to next measurement
      //measManager.AdvanceObservation();

      // Update the state, covariances, and so forth
      estimationStateS = esm.GetEstimationMJ2000EqCartesianStateForReport();
      currentEpochGT = smootherStat.epoch;
      estimationStateS.SetPrecisionTimeFlag(true);
      estimationStateS.SetEpoch(currentEpochGT.GetMjd());
      estimationStateS.SetEpochGT(currentEpochGT);

      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         estimationStateS[i] = smootherStat.state[i];
      }

      // Convert estimation state from Keplerian to Cartesian
      esm.SetEstimationMJ2000EqCartesianStateParticipant(estimationStateS);

      Rmatrix cov = smootherStat.cov;
      CovarianceEpsilonConversionReverse(cov);
      (*(stateCovariance->GetCovariance())) = cov;
      informationInverse = cov;
      //information = informationInverse.Inverse(COV_INV_TOL);

      smootherStats.push_back(smootherStat);
      AddMatlabSmootherData(smootherStat);

      // Increment to next index
      AdvanceEpoch();
   }
}


//------------------------------------------------------------------------------
// void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Performs final checks after processing the observations
 *
 * This method is called in the CHECKINGRUN state
 */
 //------------------------------------------------------------------------------
void SmootherBase::CheckCompletion()
{
   if (smootherState == FILTERING)
   {
      try
      {
         filter->CheckCompletion();

         // Add .mat data
         if (filter->matWriter != NULL)
            if (!filter->AddMatData(filter->matData))
               throw EstimatorException("Error adding .mat data file");

         filter->StateCleanUp();
         currentState = filter->GetState();
      }
      catch (BaseException &ex)
      {
         throw BackFilterExceptionMessage(ex);
      }

      if (currentState == FINISHED)
      {
         try
         {
            filter->RunComplete();
            filter->StateCleanUp();
         }
         catch (BaseException &ex)
         {
            throw BackFilterExceptionMessage(ex);
         }

         backwardFilterInfo = filter->GetUpdateStats();

         estimationEpochGT = forwardFilterInfo[0].epoch;
         filterIndex = 0;
         MoveToNext(false);

         smootherState = SMOOTHING;
         currentState = PROPAGATING;
      }
   }
   else
   {
      // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
      currentSolveForState = esm.GetEstimationStateForReport();
      currentSolveForStateC = esm.GetEstimationCartesianStateForReport();
      currentSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

      // Get Covariance
      informationInverse = (*(stateCovariance->GetCovariance()));
      // TODO: for small covariances, this thinks it's singular
      //information = informationInverse.Inverse(std::numeric_limits<double>::epsilon());

      /// Calculate conversion derivative matrixes
      // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
      cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
      //cart2SolvMatrixPrev = cart2SolvMatrix;
      // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
      solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
      //solv2KeplMatrixPrev = solv2KeplMatrix;

      ++iterationsTaken;
      currentState = FINISHED;
   }
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Finishes the estimation process and reports results
 *
 * This method is called in the FINISHED state
 */
//------------------------------------------------------------------------------
void SmootherBase::RunComplete()
{
   if (!isPredicting)
   {
      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SmootherBase::entering WriteToTextFile()\n");
      #endif
      WriteToTextFile();

      AddMatlabConfigData();

      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SmootherBase::Entering ReportProgress()\n");
      #endif
      ReportProgress();
      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SmootherBase::Completed ReportProgress()\n");
      #endif
   }
   measManager.ProcessingComplete();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SmootherBase::completed measManager.ProcessingComplete()\n");
   #endif
   esm.MapVectorToObjects();
   esm.MapCovariancesToObjects();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SmootherBase::completed esm.MapVectorToObjects()\n");
   #endif
   // Clean up memory
   for (UnsignedInt i = 0; i < hTilde.size(); ++i)
   {
      hTilde[i].clear();
   }
   hTilde.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SmootherBase::cleared hTilde\n");
   #endif

   measStats.clear();

   if (writeMatFile && (matWriter != NULL))
   {
      if (!WriteMatData())
         throw EstimatorException("Error writing .mat data file");

      matWriter->CloseFile();
   }

   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SmootherBase::completed RunComplete()\n");
   #endif
}


//------------------------------------------------------------------------------
// void SmootherUpdate()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to smoothers to update the
 * smoother information and add it to the smootherStats vector
 */
 //------------------------------------------------------------------------------
void SmootherBase::SmootherUpdate()
{
   if (currentState != CALCULATING)
   {
      // Get data for the covariance report
      SmootherInfoType smootherStat;
      smootherStat.epoch = currentEpochGT;
      smootherStat.isObs = false;

      currentSolveForState = esm.GetEstimationStateForReport();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
         smootherStat.state.push_back(currentSolveForState[ii]);

      // fill sigmas
      Rmatrix cov = *(stateCovariance->GetCovariance());
      CovarianceEpsilonConversion(cov);
      smootherStat.cov.SetSize(cov.GetNumRows(), cov.GetNumColumns());
      smootherStat.cov = cov;
      Rmatrix covVNB = GetCovarianceVNB(cov);
      smootherStat.covVNB.SetSize(covVNB.GetNumRows(), covVNB.GetNumColumns());
      smootherStat.covVNB = covVNB;

      AddMatlabSmootherData(smootherStat);
      smootherStats.push_back(smootherStat);

      //resetState = true;
   }
}


//------------------------------------------------------------------------------
// void AdvanceEpoch()
//------------------------------------------------------------------------------
/**
 * Advances the next epoch to match the filter data point currently being processed 
 */
 //------------------------------------------------------------------------------
void SmootherBase::AdvanceEpoch()
{
   filterIndex++;
   resetState = true;

   if (filterIndex == forwardFilterInfo.size())
   {
      currentState = CHECKINGRUN;
      return;
   }
   else
   {
      // Advance to next measurement
      measManager.AdvanceObservation();

      FindTimeStep();
      currentState = PROPAGATING;
   }
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string SmootherBase::GetProgressString()
{
   GmatTime taiMjdEpoch, utcMjdEpoch;
   std::string utcEpoch;
   Rmatrix finalCovariance;

   StringArray::iterator current;

   std::stringstream progress;
   progress.str("");
   progress.precision(12);
   const std::vector<ListItem*> *map = esm.GetStateMap();

   bool handleLeapSecond;

   GmatState outputEstimationState;

   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Target
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing Estimation "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " ;
               progress << "\n****************************"
                        << "****************************\n\na priori state:\n";

               if (estEpochFormat != "FromParticipants")
                  progress << "   Estimation Epoch (" << estEpochFormat
                           << "): " << estEpoch << "\n";
               else
               {
                  char s[100];
                  progress << "   Estimation Epoch:\n";
                  progress << "   " << estimationEpochGT.ToString() << " A.1 modified Julian\n";
                  taiMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD);
                  utcMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                        GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
                  progress << "   " << taiMjdEpoch.ToString() << " TAI modified Julian\n";
                  utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
                  progress << "   " << utcEpoch << " UTCG\n";
               }

               outputEstimationState = esm.GetEstimationStateForReport();
               
               for (UnsignedInt i = 0; i < map->size(); ++i)
               {
                  progress << "   " << GetElementFullName((*map)[i], false) << " = "
                           << outputEstimationState[i];

                  if ((*map)[i]->elementName == "PassBiases")
                  {
                     Integer index = (*map)[i]->subelement - 1;
                     progress << "    in time range " << ((ErrorModel*)(*map)[i]->object)->GetTimeRangeInUTCGregorian(index);
                  }
                  progress << "\n";

               };
            }
            break;

         case ESTIMATING:
            progress << "Current estimated state:\n";
            progress << "   Estimation Epoch: "
                     << currentEpochGT.ToString() << "\n"; 

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   "
                        << (*estimationState)[i];
            }

            progress << "\n   Current Residual Value: ";
            if (measStats.back().residual.size() > 0)
               progress << measStats.back().residual.back() << "      ";
            else
               progress << "N/A      ";
            progress << "   Trace of the State Covariance:  "
                     << stateCovariance->GetCovariance()->Trace() << "\n";
            break;

         case FINISHED:
            progress << "\n****************************"
                     << "****************************\n"
                     << "*** Estimation Completed"
                     << "\n****************************"
                     << "****************************\n\n"
                     << "Final Estimated State:\n\n";

            if (estEpochFormat != "FromParticipants")
               progress << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n";
            else
            {
               progress << "   Estimation Epoch:\n";
               progress << "   " << currentEpochGT.ToString() << " A.1 modified Julian\n";
               taiMjdEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD);
               utcMjdEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                     GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
               progress << "   " << taiMjdEpoch.ToString() << " TAI modified Julian\n";
               utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
               progress << "   " << utcEpoch << " UTCG\n";
            }

            outputEstimationState = esm.GetEstimationStateForReport();

            for (UnsignedInt i = 0; i < map->size(); ++i)
            {
               progress << "   " << GetElementFullName((*map)[i], false) << " = "
                     << outputEstimationState[i];
            
               if ((*map)[i]->elementName == "PassBiases")
               {
                  Integer index = (*map)[i]->subelement - 1;
                  progress << "    in time range " << ((ErrorModel*)(*map)[i]->object)->GetTimeRangeInUTCGregorian(index);
               }
               progress << "\n";

            }

            MessageInterface::ShowMessage("%s\n", progress.str().c_str());
            progress.str("");
            //finalCovariance = information.Inverse();
            finalCovariance = informationInverse;
            CovarianceEpsilonConversion(finalCovariance);

            // Display final coveriance matrix
            progress << "Final Covariance Matrix:\n\n";
            for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
               {
                  char s[100];
                  sprintf(&s[0], "   %22.12le", finalCovariance(i, j));
                  std::string ss(s);
                  progress << "   " << ss.substr(ss.size()-24); //finalCovariance(i, j);
               }
               progress << "\n";
            }

            MessageInterface::ShowMessage("%s", progress.str().c_str());
            progress.str("");
            // Display final correlation matrix
            progress << "\nFinal Correlation Matrix:\n\n";
            for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
               {
                  char s[100];
                  sprintf(&s[0], "   %22.12lf", finalCovariance(i, j)/ sqrt(finalCovariance(i, i)*finalCovariance(j, j)));
                  std::string ss(s);
                  progress << "   " << ss.substr(ss.size()-24);
               }
               progress << "\n";
            }

            MessageInterface::ShowMessage("%s", progress.str().c_str());
            progress.str("");

            #ifdef DUMP_RESIDUALS
               MessageInterface::ShowMessage("\nMeasurement Residuals: \n");
               for (UnsignedInt i = 0; i < measurementEpochs.size(); ++i)
               {
                  MessageInterface::ShowMessage("   %.12lf   %.12lf\n",
                        measurementEpochs[i], measurementResiduals[i]);
               }
            #endif

            progress << "\n****************************"
                     << "****************************\n"
                     << std::endl;

            MessageInterface::ShowMessage("%s", progress.str().c_str());
            progress.str("");

            break;

         default:
            throw SolverException(
               "Solver state not supported for the smoother");
      }
   }
   else
      return Estimator::GetProgressString();

   return progress.str();
}


//------------------------------------------------------------------------------
// void OverwriteEditFlag(const std::string &editFlag)
//------------------------------------------------------------------------------
/**
 * This method indicates if the edit flag is to be overwritten
 *
 * @param editFlag The edit flag that might be overwitten
 */
 //------------------------------------------------------------------------------
bool SmootherBase::OverwriteEditFlag(const std::string &editFlag)
{
   return false;
}


//------------------------------------------------------------------------------
// void WriteEditFlag()
//------------------------------------------------------------------------------
/**
 * This method indicates if a new edit flag is to be written
 */
 //------------------------------------------------------------------------------
bool SmootherBase::WriteEditFlag()
{
   return false;
}


//-------------------------------------------------------------------------
// bool DataFilter()
//-------------------------------------------------------------------------
/**
* The smoother does not edit measurements itself, it relies on the filter
* to perform measurement editing. This function is only here because it
* is pure virtual in the Estimator class
*/
//-------------------------------------------------------------------------
bool SmootherBase::DataFilter()
{
   return measManager.GetObsDataObject()->inUsed;
}


// Placeholder implementation of pure virtual functions
void SmootherBase::WriteReportFileHeaderPart6()
{ }


//----------------------------------------------------------------------
// void WriteNotationHeader()
//----------------------------------------------------------------------
/**
* Write notation header
*/
//----------------------------------------------------------------------
void SmootherBase::WriteNotationHeader()
{
   textFile
      << "                         -    : Not edited                                                    BXY  : Blocked, X = Path index, Y = Count index(Doppler)\n"
      << "                         U    : Unused because no computed value configuration available      SIG  : Edited by sigma editor\n"
      << "                         R    : Out of ramp table range                                       USER : Edited by second-level data editor\n"
      << "                         HORP : Blocked by Height of Ray Path criteria                        EGAP : Edited due to ephemeris gap\n";
}


//----------------------------------------------------------------------
// void WriteReportFileSummaryPart1(Solver::SolverState sState)
//----------------------------------------------------------------------
/**
* Write statistics summary for estimation
*/
//----------------------------------------------------------------------
void SmootherBase::WriteReportFileSummaryPart1(Solver::SolverState sState)
{
   // 1. Write summary part 1 header:
   textFile0 << "\n";
   textFile0 << CenterAndPadHeader(GetHeaderName() + " MEASUREMENT STATISTICS") << "\n";
   textFile0 << "\n";
   textFile0.flush();

   if (sState == FINISHED)
   {
      std::stringstream ss;

      ss << "     Smoother run completed!     ";
      textFile0 << GmatStringUtil::GetAlignmentString(ss.str(), 160, GmatStringUtil::CENTER) << "\n";

      std::vector<ObservationData> *obsList = GetMeasurementManager()->GetObservationDataList();
      std::string firstObsEpoch, lastObsEpoch;
      Integer lastIndex = obsList->size() - 1;
      Real temp;
      theTimeConverter->Convert("A1ModJulian", (*obsList)[0].epoch, "", "UTCGregorian", temp, firstObsEpoch, 1);         // @todo: It needs to changes (*obsList)[0].epoch to (*obsList)[0].epochGT
      theTimeConverter->Convert("A1ModJulian", (*obsList)[lastIndex].epoch, "", "UTCGregorian", temp, lastObsEpoch, 1);  // @todo: It needs to changes (*obsList)[lastIndex].epoch to (*obsList)[lastIndex].epochGT

      textFile0 << "                                                      Time of First Observation : " << firstObsEpoch << "\n";
      textFile0 << "                                                      Time of Last Observation  : " << lastObsEpoch << "\n";
      textFile0 << "\n";
      textFile0.flush();
   }

   std::stringstream ss;
   ss << "Total Number Of Observations              : " << GetMeasurementManager()->GetObservationDataList()->size();
   textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";
   ss.str(""); ss << "Observations Used For Estimation          : " << numRemovedRecords["N"];
   textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   ss.str(""); ss << "No Computed Value Configuration Available : " << numRemovedRecords["U"];
   textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   ss.str(""); ss << "Out of Ramp Table Range                   : " << numRemovedRecords["R"] << " ";
   textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   ss.str("");  ss << "Signal Blocked                            : " << numRemovedRecords["B"];
   textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   ss.str(""); ss << "Sigma Editing                             : " << numRemovedRecords["SIG"];
   textFile1_1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   ss.str(""); ss << "User Editing                              : " << numRemovedRecords["USER"];
   textFile1_1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";

   if (numRemovedRecords["HORP"] > 0)
   {
      ss.str("");  ss << "Signal Blocked by HORP                   : " << numRemovedRecords["HORP"];
      textFile1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60) << "\n";
   }

   if (numRemovedRecords["EGAP"] > 0)
   {
      ss.str(""); ss << "Ephemeris Gap Editing                     : " << numRemovedRecords["EGAP"];
      textFile1_1 << GmatStringUtil::GetAlignmentString("", 56) << GmatStringUtil::GetAlignmentString(ss.str(), 60);
   }

   textFile1_1 << "\n";
   textFile1_1.flush();

   textFile1.flush();
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummaryPart5(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write additional covariance information
*/
//------------------------------------------------------------------------------------------
void SmootherBase::WriteReportFileSummaryPart5(Solver::SolverState sState)
{
   textFile5 << "*****************************************************************  SMOOTHER COVARIANCE REPORT  *****************************************************************\n";
   textFile5 << "\n";

   for (UnsignedInt ii = 0U; ii < smootherStats.size(); ii++)
   {
      if (GmatMathUtil::Mod(ii, 80) < 0.001)
         WriteCovariancePageHeader();
      BuildCovarianceLine(smootherStats[ii]);
   }
   textFile5 << "\n";
   textFile5 << "***********************************************************************  END OF REPORT  ************************************************************************\n";
   textFile5.flush();
}


//----------------------------------------------------------------------
// void WriteCovariancePageHeader()
//----------------------------------------------------------------------
/**
* Write covariance page header
*/
//----------------------------------------------------------------------
void SmootherBase::WriteCovariancePageHeader()
{
   /// 4.1. Write page header
   textFile5 << "\n";

   if (IsIterative())
      textFile5 << "Iter ";

   if (textFileMode == "Normal")
   {
      textFile5 << "RecNum  UTCGregorian-Epoch        Obs-Type            " << GmatStringUtil::GetAlignmentString("Participants", filter->pcolumnCovLen) << " Edit       Scaled Residual      Sigma-V (km)    Sigma-N (km)    Sigma-B (km)\n";
   }
   else
   {
      textFile5 << "RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " << GmatStringUtil::GetAlignmentString("Participants", filter->pcolumnCovLen) << " Edit       Scaled Residual      Sigma-V (km)    Sigma-N (km)    Sigma-B (km)\n";
   }
   textFile5 << "\n";
}


//------------------------------------------------------------------------------
// void BuildCovarianceLine(const SmootherInfoType &smootherStat)
//------------------------------------------------------------------------------
/**
 * This method builds the line for each measurement and time-update to the text file.
 *
 * @note The contents of the text file are TBD
 */
//------------------------------------------------------------------------------
void SmootherBase::BuildCovarianceLine(const SmootherInfoType &smootherStat)
{
   const ObservationData *currentObs;
   if (smootherStat.isObs)
      currentObs = measManager.GetObsData(smootherStat.measStat.recNum); // Get current observation data

   char s[1000];
   std::string timeString;
   std::string ss;
   Real temp;
   std::string linePrefix;
   std::string lineSuffix;

   // Write to report file iteration number, record number, and time:
   bool handleLeapSecond;
   theTimeConverter->Convert("A1ModJulian", smootherStat.epoch.GetMjd(), "", "UTCGregorian", temp, timeString, 1, &handleLeapSecond);
   if (smootherStat.isObs)
   {
      if (IsIterative())
         sprintf(&s[0], "%4d %6d  ", iterationsTaken, smootherStat.measStat.recNum);
      else
         sprintf(&s[0], "%6d  ", smootherStat.measStat.recNum);
   }
   else
   {
      sprintf(&s[0], "        ");
   }
   linePrefix = s;

   linePrefix += GmatStringUtil::GetAlignmentString(timeString.c_str(), 26, GmatStringUtil::LEFT);

   if (textFileMode != "Normal")
   {
      Real timeTAI = theTimeConverter->Convert(smootherStat.epoch.GetMjd(), TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD);
      sprintf(&s[0], "%.12lf ", timeTAI);
      linePrefix += s;
   }

   // Write to report file the measurement type

   // Write to report file measurement type name:
   linePrefix += GmatStringUtil::GetAlignmentString(smootherStat.measStat.type, 19) + " ";

   // Write to report file measurement type unit:
   if (textFileMode != "Normal")
   {
      if (smootherStat.isObs)
         linePrefix += GmatStringUtil::GetAlignmentString(currentObs->unit, 6) + " ";
      else
         linePrefix += GmatStringUtil::GetAlignmentString("", 6) + " ";
   }

   // Write to report file all participants in signal path:
   ss = "";
   if (smootherStat.isObs)
      for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
         ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
   linePrefix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(ss), pcolumnLen) + " ";


   // Write sigma values to suffix
   lineSuffix = "";
   for (UnsignedInt ii = 0; ii < 3; ii++)
   {
      Real val = smootherStat.covVNB(ii, ii);
      if (val < 0.0)
         lineSuffix += GmatStringUtil::GetAlignmentString("N/A", 16, GmatStringUtil::RIGHT);
      else if ((1.0e-6 <= val) && (val < 1.0e16))
         lineSuffix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(val), false, false, true, 6)), 16, GmatStringUtil::RIGHT);
      else
         lineSuffix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(val), false, true, true, 6)), 16, GmatStringUtil::RIGHT);
   }


   std::string removedReason = smootherStat.measStat.removedReason;

   if (smootherStat.measStat.isCalculated && removedReason == "N")
   {
      if (textFileMode == "Normal")
         removedReason = "";
      else
         removedReason = "-";
   }

   if (smootherStat.isObs)
   {
      for (Integer k = 0; k < smootherStat.measStat.residual.size(); ++k)
      {
         textFile5 << linePrefix;   // write line prefix for each element. The GPS Point Solution has three elements.
         textFile5 << GmatStringUtil::GetAlignmentString(removedReason, 4, GmatStringUtil::LEFT) + " ";

         // Write C-value, and O-C
         if (!smootherStat.measStat.isCalculated)
         {
            textFile5 << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << "  "; // Scaled Residual
         }
         else
         {
            sprintf(&s[0], "%21.6lf", smootherStat.measStat.scaledResid[k]);
            textFile5 << s << "  ";
         }

         textFile5 << lineSuffix << "\n";
      }
   }
   else // just a time-update
   {
      textFile5 << linePrefix;   // write line prefix for each element.
      textFile5 << GmatStringUtil::GetAlignmentString("", 4, GmatStringUtil::LEFT) << " ";
      textFile5 << GmatStringUtil::GetAlignmentString("", 21, GmatStringUtil::RIGHT) << "  "; // Scaled Residual
      textFile5 << lineSuffix << "\n";
   }
}


//------------------------------------------------------------------------------
// bool IsIterative()
//------------------------------------------------------------------------------
/**
* This function indicates if this estimator is iterative.
*/
//------------------------------------------------------------------------------
bool SmootherBase::IsIterative()
{
   return false;
}


//----------------------------------------------------------------------
// std::string GetHeaderName()
//----------------------------------------------------------------------
/**
* Write the name of the estimator type in upper case for report headers
*/
//----------------------------------------------------------------------
std::string SmootherBase::GetHeaderName()
{
   return "SMOOTHER";
}


//------------------------------------------------------------------------------
// bool WriteMatData()
//------------------------------------------------------------------------------
/**
 * Method used to write the MATLAB .mat file
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool SmootherBase::WriteMatData()
{
   bool retval;

   retval = Estimator::WriteMatData();
   matWriter->ClearData();

   MessageInterface::ShowMessage("Writing Smoother MATLAB File...\n");

   StringArray dataDesc;

   AddMatData(matSmootherData);

   dataDesc = GetMatDataDescription(matSmootherData);
   matWriter->DescribeData(dataDesc, iterationsTaken);
   matWriter->WriteData("Smoother");

   WriteAdditionalMatData();

   MessageInterface::ShowMessage("Finished Writing Smoother MATLAB File.\n\n");

   return retval;
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const SmootherMeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method adds the smoother data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SmootherBase::AddMatlabData(const SmootherMeasurementInfoType &measStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabData(measStat, matData, matIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const SmootherMeasurementInfoType &measStat,
//                     DataBucket &matData, IntegerMap &matIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the smoother data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SmootherBase::AddMatlabData(const SmootherMeasurementInfoType &measStat,
                                 DataBucket &matData, IntegerMap &matIndex)
{
   Estimator::AddMatlabData(measStat, matData, matIndex);

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   Integer matMeasIndex = matData.GetContainerSize() - 1;

   if (matIndex.count("ScaledResid") == 0)
   {
      matIndex["ScaledResid"] = matData.AddRealContainer("ScaledResidual", 0);
      matIndex["State"] = matData.AddRealContainer("PreUpdateState", stateSize);

      matIndex["Cov"] = matData.AddReal3DContainer("PreUpdateCovariance");
      matIndex["CovVNB"] = matData.AddReal3DContainer("PreUpdateCovarianceVNB");
   }

   matData.realValues[matIndex["ScaledResid"]][matMeasIndex] = measStat.scaledResid;
   matData.realValues[matIndex["State"]][matMeasIndex] = measStat.state;

   for (UnsignedInt ii = 0; ii < measStat.cov.GetNumRows(); ii++)
   {
      RealArray rowArray = measStat.cov.GetRow(ii).GetRealArray();
      matData.real3DValues[matIndex["Cov"]][matMeasIndex].push_back(rowArray);
   }

   for (UnsignedInt ii = 0; ii < measStat.covVNB.GetNumRows(); ii++)
   {
      RealArray rowArray = measStat.covVNB.GetRow(ii).GetRealArray();
      matData.real3DValues[matIndex["CovVNB"]][matMeasIndex].push_back(rowArray);
   }
}


//------------------------------------------------------------------------------
//  void AddMatlabSmootherData(const SmootherInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * This method adds the smoother data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SmootherBase::AddMatlabSmootherData(const SmootherInfoType &updateStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabSmootherData(updateStat, matSmootherData, matSmootherIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabSmootherData(const SmootherInfoType &updateStat,
//                             DataBucket &matSmootherData,
//                             IntegerMap &matSmootherIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the smoother data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SmootherBase::AddMatlabSmootherData(const SmootherInfoType &updateStat,
                                         DataBucket &matSmootherData,
                                         IntegerMap &matSmootherIndex)
{
   Integer matMeasIndex;

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   bool initial = false;

   if (matSmootherIndex.count("EpochTAI") == 0)
   {
      initial = true;

      matSmootherData.SetInitialRealValue(NAN);
      matSmootherIndex["EpochTAI"] = matSmootherData.AddRealContainer("EpochTAI", 2);
      matSmootherIndex["EpochUTC"] = matSmootherData.AddRealContainer("EpochUTC", 2);
      matSmootherIndex["MeasNum"] = matSmootherData.AddRealContainer("MeasurementNumber");
      matSmootherIndex["Type"] = matSmootherData.AddStringContainer("UpdateType");

      matSmootherIndex["State"] = matSmootherData.AddRealContainer("State", stateSize);
      matSmootherIndex["Cov"] = matSmootherData.AddReal3DContainer("Covariance");

      matSmootherIndex["CovVNB"] = matSmootherData.AddReal3DContainer("CovarianceVNB");
      //matSmootherIndex["ProcNoise"] = matSmootherData.AddReal3DContainer("ProcessNoise");
   }

   matMeasIndex = matSmootherData.AddPoint();

   std::string temp;

   Real taiEpoch = theTimeConverter->Convert(updateStat.epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD).GetMjd();
   Real utcEpoch = theTimeConverter->Convert(updateStat.epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD).GetMjd();

   matSmootherData.realValues[matSmootherIndex["EpochTAI"]][matMeasIndex][0] = taiEpoch + MATLAB_DATE_CONVERSION;
   matSmootherData.realValues[matSmootherIndex["EpochTAI"]][matMeasIndex][1] = taiEpoch;
   matSmootherData.realValues[matSmootherIndex["EpochUTC"]][matMeasIndex][0] = utcEpoch + MATLAB_DATE_CONVERSION;
   matSmootherData.realValues[matSmootherIndex["EpochUTC"]][matMeasIndex][1] = utcEpoch;

   for (UnsignedInt ii = 0; ii < updateStat.covVNB.GetNumRows(); ii++)
   {
      RealArray rowArray = updateStat.covVNB.GetRow(ii).GetRealArray();
      matSmootherData.real3DValues[matSmootherIndex["CovVNB"]][matMeasIndex].push_back(rowArray);
   }

   //for (UnsignedInt ii = 0; ii < updateStat.processNoise.GetNumRows(); ii++)
   //{
   //   RealArray rowArray = updateStat.processNoise.GetRow(ii).GetRealArray();
   //   matSmootherData.real3DValues[matSmootherIndex["ProcNoise"]][matMeasIndex].push_back(rowArray);
   //}

   if (updateStat.isObs)
   {
      matSmootherData.stringValues[matSmootherIndex["Type"]][matMeasIndex][0] = "Measurement";
      matSmootherData.realValues[matSmootherIndex["MeasNum"]][matMeasIndex][0] = updateStat.measStat.recNum;
   }
   else
   {
      if (initial)
         matSmootherData.stringValues[matSmootherIndex["Type"]][matMeasIndex][0] = "Initial";
      else if (smootherState == PREDICTING)
         matSmootherData.stringValues[matSmootherIndex["Type"]][matMeasIndex][0] = "Predict";
      else
         matSmootherData.stringValues[matSmootherIndex["Type"]][matMeasIndex][0] = "Time";
   }

   matSmootherData.realValues[matSmootherIndex["State"]][matMeasIndex] = updateStat.state;

   for (Integer ii = 0; ii < stateSize; ii++)
   {
      RealArray rowArray = updateStat.cov.GetRow(ii).GetRealArray();
      matSmootherData.real3DValues[matSmootherIndex["Cov"]][matMeasIndex].push_back(rowArray);
   }
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const SeqEstimator::FilterMeasurementInfoType &measStat,
//                     DataBucket &matData, IntegerMap &matIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SmootherBase::AddMatlabData(const SeqEstimator::FilterMeasurementInfoType &measStat,
                                 DataBucket &matData, IntegerMap &matIndex)
{
   filter->AddMatlabData(measStat, matData, matIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabFilterData(const SeqEstimator::UpdateInfoType &updateStat,
//                           DataBucket &matFilterData, IntegerMap &matFilterIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SmootherBase::AddMatlabFilterData(const SeqEstimator::UpdateInfoType &updateStat,
                                       DataBucket &matFilterData, IntegerMap &matFilterIndex)
{
   filter->AddMatlabFilterData(updateStat, matFilterData, matFilterIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabConfigData()
//------------------------------------------------------------------------------
/**
 * This method adds the estimation configuration data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SmootherBase::AddMatlabConfigData()
{
   Estimator::AddMatlabConfigData();

   if (writeMatFile && (matWriter != NULL))
   {
      const std::vector<ListItem*> *stateMap = esm.GetStateMap();

      matConfigIndex["StateNames"] = matConfigData.AddString2DArray("StateNames");

      for (Integer ii = 0; ii < stateMap->size(); ii++)
      {
         matConfigData.string2DArrayValues[matConfigIndex["StateNames"]].push_back({ GetElementFullNameMJ2000Eq((*stateMap)[ii], false, "Cartesian") });
      }

   }
}


//------------------------------------------------------------------------------
// EstimatorException BackFilterExceptionMessage(BaseException &ex)
//------------------------------------------------------------------------------
/**
 * Creates a new exception message indicating that the input exception happened
 * during the backward filter pass.
 *
 * @param ex The input exception that occurred during the backward filter
 *
 * @return The new exception message to be thrown
 */
 //------------------------------------------------------------------------------
EstimatorException SmootherBase::BackFilterExceptionMessage(BaseException &ex)
{
   EstimatorException newEx;
   newEx.SetDetails("The following error occurred during the backward filter: " + ex.GetFullMessage());

   return newEx;
}

//------------------------------------------------------------------------------
// std::string GetMeasurementResidualTitles()
//------------------------------------------------------------------------------
/**
 * Returns the Mesurement residual titles to be used in report files.
 */
 //------------------------------------------------------------------------------
std::string SmootherBase::GetMeasurementResidualTitles()
{
   if(textFileMode == "Normal")
      return "RecNum  UTCGregorian-Epoch        Obs-Type            " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit          Observed (O)          Computed (C)       Residual (O-C)  Elev.\n";
   else
      return "RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit               Obs (O)    Obs-Correction (O)               Cal (C)       Residual (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|    Elevation-Angle    Partial-Derivatives";
}
