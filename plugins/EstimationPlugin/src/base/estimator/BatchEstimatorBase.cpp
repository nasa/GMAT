//$Id: BatchEstimatorBase.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimatorBase
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
// Created: 2009/08/04
//
/**
 * Implementation of the BatchEstimatorBase class
 */
//------------------------------------------------------------------------------


#include "BatchEstimatorBase.hpp"
//#include "GmatState.hpp"
//#include "PropagationStateManager.hpp"
//#include "EstimatorException.hpp"
//#include "GmatConstants.hpp"
//#include "RealUtilities.hpp"
#include "TimeTypes.hpp"
//#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include <sstream>
//#include "SpaceObject.hpp"    // To access epoch data
#include "Spacecraft.hpp"
#include "StringUtil.hpp"
#include "GroundstationInterface.hpp"
#include "EstimatorException.hpp"
#include "Propagator.hpp"

//#define DEBUG_STATE_MACHINE
//#define DEBUG_SIMULATOR_WRITE
//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_EVENT
//#define DEBUG_ACCUMULATION_RESULTS
//#define DEBUG_PROPAGATION
//#define DEBUG_DATA_FILTER

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DEBUG_VERBOSE
//#define RUN_SINGLE_PASS
//#define DUMP_FINAL_RESIDUALS


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
BatchEstimatorBase::PARAMETER_TEXT[] =
{
   "AbsoluteTol",
   "RelativeTol",
   "UseInitialCovariance",
   "InversionAlgorithm",
   "MaxConsecutiveDivergences",
   "ResetBestRMSIfDiverging",
   "FreezeMeasurementEditing",
   "FreezeIteration",
   "ConvergentStatus",
   // todo Add useApriori here
};

const Gmat::ParameterType
BatchEstimatorBase::PARAMETER_TYPE[] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,         // "UseInitialCovariance"
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,         // FREEZE_MEASUREMENT_EDITING
   Gmat::INTEGER_TYPE,         // FREEZE_ITERATION
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// BatchEstimatorBase(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subclass type name
 * @param name The name of the instance that is being created
 */
//------------------------------------------------------------------------------
BatchEstimatorBase::BatchEstimatorBase(const std::string &type,
      const std::string &name) :
   Estimator                  (type, name),
   absoluteTolerance          (1.0e-3),
   relativeTolerance          (1.0e-4),
   oldResidualRMS             (0.0),
   newResidualRMS             (1.0e12),
   resetBestRMSFlag           (false),
   useApriori                 (false),                  // second term of Equation Eq8-184 in GTDS MathSpec is not used   
   advanceToEstimationEpoch   (false),
//   converged                  (false),
   estimationStatus           (UNKNOWN),
   maxConsDivergences         (3),
   freezeEditing              (false),                   // measurement editing is not frozen
   freezeIteration            (4),                       // number of iteration to freeze measurement editing
   inversionType              ("Internal"),
   hasInitialized			  (false)
{
   objectTypeNames.push_back("BatchEstimatorBase");
   parameterCount = BatchEstimatorBaseParamCount;
}


//------------------------------------------------------------------------------
// ~BatchEstimatorBase()
//------------------------------------------------------------------------------
/**
 * BatchEstimatorBase destructor
 */
//------------------------------------------------------------------------------
BatchEstimatorBase::~BatchEstimatorBase()
{
   dx.clear();

   for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
   {
      if (outerLoopBuffer[i])
         delete outerLoopBuffer[i];
   }
   outerLoopBuffer.clear();
}


//------------------------------------------------------------------------------
// BatchEstimatorBase(const BatchEstimatorBase& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The BatchEstimatorBase that is being copied
 */
//------------------------------------------------------------------------------
BatchEstimatorBase::BatchEstimatorBase(const BatchEstimatorBase& est) :
   Estimator                  (est),
   absoluteTolerance          (est.absoluteTolerance),
   relativeTolerance          (est.relativeTolerance),
   oldResidualRMS             (0.0),
   newResidualRMS             (1.0e12),
   resetBestRMSFlag           (est.resetBestRMSFlag),
   useApriori                 (est.useApriori),
   advanceToEstimationEpoch   (false),
   estimationStatus           (UNKNOWN),
   maxConsDivergences         (est.maxConsDivergences),
   freezeEditing              (est.freezeEditing),
   freezeIteration            (est.freezeIteration),
   inversionType              (est.inversionType),
   hasInitialized             (est.hasInitialized)
{
}


//------------------------------------------------------------------------------
// BatchEstimatorBase& operator=(const BatchEstimatorBase& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The BatchEstimatorBase that is being copied
 *
 * @return This BatchEstimatorBase, configured to match est
 */
//------------------------------------------------------------------------------
BatchEstimatorBase& BatchEstimatorBase::operator=(const BatchEstimatorBase& est)
{
   if (this != &est)
   {
      Estimator::operator=(est);

      absoluteTolerance		   = est.absoluteTolerance;
      relativeTolerance        = est.relativeTolerance;
      oldResidualRMS           = 0.0;
      newResidualRMS           = 0.0;
      resetBestRMSFlag         = est.resetBestResidualRMS;
      useApriori			   = est.useApriori;

      advanceToEstimationEpoch = false;
      estimationStatus         = UNKNOWN;

      maxConsDivergences       = est.maxConsDivergences;
      freezeEditing            = est.freezeEditing;
      freezeIteration          = est.freezeIteration;

      // Clear the loop buffer
      for (UnsignedInt i = 0; i < outerLoopBuffer.size(); ++i)
      {
         if (outerLoopBuffer[i])
            delete outerLoopBuffer[i];
      }
      outerLoopBuffer.clear();

      inversionType            = est.inversionType;
	  hasInitialized           = est.hasInitialized;
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
std::string BatchEstimatorBase::GetParameterText(const Integer id) const
{
   if (id >= EstimatorParamCount && id < BatchEstimatorBaseParamCount)
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
std::string BatchEstimatorBase::GetParameterUnit(const Integer id) const
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
Integer BatchEstimatorBase::GetParameterID(const std::string &str) const
{
   for (Integer i = EstimatorParamCount; i < BatchEstimatorBaseParamCount; i++)
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
Gmat::ParameterType BatchEstimatorBase::GetParameterType(const Integer id) const
{
   if (id >= EstimatorParamCount && id < BatchEstimatorBaseParamCount)
      return PARAMETER_TYPE[id - EstimatorParamCount];

   return Estimator::GetParameterType(id);
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
std::string BatchEstimatorBase::GetParameterTypeString(const Integer id) const
{
   return Estimator::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool BatchEstimatorBase::IsParameterReadOnly(const Integer id) const
{
   if (id == CONVERGENT_STATUS)
      return true;

   return Estimator::IsParameterReadOnly(id);
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
Real BatchEstimatorBase::GetRealParameter(const Integer id) const
{
   if (id == ABSOLUTETOLERANCE)
      return absoluteTolerance;
   if (id == RELATIVETOLERANCE)
      return relativeTolerance;

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
Real BatchEstimatorBase::SetRealParameter(const Integer id, const Real value)
{
   if (id == ABSOLUTETOLERANCE)
   {
      if (value > 0.0)
         absoluteTolerance = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not a positive number\n");

      return absoluteTolerance;
   }

   if (id == RELATIVETOLERANCE)
   {
      if ((value > 0.0) && (value <= 1.0))
         relativeTolerance = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not in range (0,1]\n");

      return relativeTolerance;
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
Real BatchEstimatorBase::GetRealParameter(const std::string &label) const
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
Real BatchEstimatorBase::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns value of an integer parameter given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return value of the requested parameter.
 */
//------------------------------------------------------------------------------
Integer BatchEstimatorBase::GetIntegerParameter(const Integer id) const
{
   if (id == MAX_CONSECUTIVE_DIVERGENCES)
      return maxConsDivergences;
   else if (id == FREEZE_ITERATION)
      return freezeIteration;

   return Estimator::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets value to an integer parameter specified by the input parameter ID.
 *
 * @param id       ID for the requested parameter.
 * @param value    integer value used to set to the request parameter. 
 *
 * @return value set to the requested parameter.
 */
//------------------------------------------------------------------------------
Integer BatchEstimatorBase::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == MAX_CONSECUTIVE_DIVERGENCES)
   {
      if (value < 1)
      {
         std::stringstream ss;
         ss << "Error: " << GetName() << ".MaxConsecutiveDivergences has invalid value (" << value << "). It has to be a positive integer greater than 0.\n";
         throw EstimatorException(ss.str());
         return value;
      }

      maxConsDivergences = value;
      return maxConsDivergences;
   }
   else if (id == FREEZE_ITERATION)
   {
      if (value < 1)
      {
         std::stringstream ss;
         ss << "Error: " << GetName() << ".FreezeIteration has invalid value (" << value << "). It has to be a positive integer greater than 0.\n";
         throw EstimatorException(ss.str());
         return value;
      }

      freezeIteration = value;
      return freezeIteration;
   }

   return Estimator::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value of an integer parameter specified by parameter name.
*
* @param label    name of parameter.
*
* @return         value of an integer parameter.
*/
//------------------------------------------------------------------------------
Integer BatchEstimatorBase::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets value to an integer parameter specified by the input parameter name.
*
* @param label    name for the requested parameter.
* @param value    integer value used to set to the request parameter.
*
* @return value set to the requested parameter.
*/
//------------------------------------------------------------------------------
Integer BatchEstimatorBase::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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
std::string BatchEstimatorBase::GetStringParameter(const Integer id) const
{
   if (id == INVERSION_ALGORITHM)
   {
      return inversionType;
   }

   if (id == CONVERGENT_STATUS)
   {
      switch (estimationStatus)
      {
      case UNKNOWN:
         return "Unknown";
         break;
      case ABSOLUTETOL_CONVERGED:
         return "Meet Absolute Tolerance convergence criteria";
         break;
      case RELATIVETOL_CONVERGED:
         return "Meet Relative Tolerance convergence criteria";
         break;
      case ABS_AND_REL_TOL_CONVERGED:
         return "Meet Absolute and Relative Tolerance convergence criteria";
         break;
      case MAX_CONSECUTIVE_DIVERGED:
         return "Exceed maximum number of consecutive diverged iterations";
         break;
      case MAX_ITERATIONS_DIVERGED:
         return "Exceed maximum number of iterations";
         break;
      case CONVERGING:
         return "Converging";
         break;
      case DIVERGING:
         return "Diverging";
         break;
	  case RUN_INITIAL_GUESS:
		 return "Ran Initial Guess";
		 break;
      }
   }

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
bool BatchEstimatorBase::SetStringParameter(const Integer id,
         const std::string &value)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("BatchEstimatorBase::SetStringParameter(%d, %s)\n",
            id, value.c_str());
   #endif

   if (id == INVERSION_ALGORITHM)
   {
      if ((value == "Internal") || (value == "Schur") || (value == "Cholesky"))
      {
         inversionType = value;
         return true;
      }
      else
         throw EstimatorException("The requested inversion routine is not an "
               "allowed value for the field \"InversionAlgorithm\"; allowed "
               "values are \"Internal\", \"Schur\" and \"Cholesky\"");
   }

   return Estimator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param id The integer ID for the parameter
 * @param index The index into the StringArray
 *
 * @return The value
 */
//------------------------------------------------------------------------------
std::string BatchEstimatorBase::GetStringParameter(const Integer id,
      const Integer index) const
{
   return Estimator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter in a StringArray
 *
 * @param id The integer ID for the parameter
 * @param value The new value
 * @param index The index into the StringArray
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   return Estimator::SetStringParameter(id, value, index);
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
std::string BatchEstimatorBase::GetStringParameter(const std::string &label) const
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
bool BatchEstimatorBase::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The text label for the parameter
 * @param index The index into the StringArray
 *
 * @return The value
 */
//------------------------------------------------------------------------------
std::string BatchEstimatorBase::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter in a StringArray
 *
 * @param label The text label for the parameter
 * @param value The new value
 * @param index The index into the StringArray
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// bool BatchEstimatorBase::GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets value from a boolean parameter 
 *
 * @param id   The id number of a parameter
 *
 * @return     a boolean value
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::GetBooleanParameter(const Integer id) const
{
   if (id == USE_INITIAL_COVARIANCE)
      return useApriori;
   else if (id == RESET_BEST_RMS)
      return resetBestRMSFlag;
   else if (id == FREEZE_MEASUREMENT_EDITING)
      return freezeEditing;

   return Estimator::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 *
 */
 //------------------------------------------------------------------------------
bool BatchEstimatorBase::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_INITIAL_COVARIANCE)
   {
      useApriori = value;
      return useApriori;
   }
   else if (id == RESET_BEST_RMS)
   {
      resetBestRMSFlag = value;
      return resetBestRMSFlag;
   }
   else if (id == FREEZE_MEASUREMENT_EDITING)
   {
      freezeEditing = value;
      return freezeEditing;
   }

   return Estimator::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool BatchEstimatorBase::GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value from a boolean parameter
*
* @param label   The name of a parameter
*
* @return        value of the parameter
*/
//------------------------------------------------------------------------------
bool BatchEstimatorBase::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool BatchEstimatorBase::SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
* This method sets value to a boolean parameter
*
* @param label      The name of a parameter
* @param value      value used to set to parameter
*
* @return true value when it successfully sets the value, false otherwise.
*/
//------------------------------------------------------------------------------
bool BatchEstimatorBase::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs an action on the instance.
 *
 * TakeAction is a method overridden from GmatBase.  The only action defined for
 * a Simulator is "Reset" which resets the state to INITIALIZING
 *
 * @param <action>      Text label for the action.
 * @param <actionData>  Related action data, if needed.
 *
 * @return  flag indicating successful completion or not.
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   // @todo  Complete Reset action (?) and add others if needed
   if (action == "Reset")
   {
      currentState = INITIALIZING;
      isInitialized = false;
      estimationStatus = UNKNOWN;

      return true;
   }

   return Estimator::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the estimator - checks for unset references and does some
 * validation checking.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::Initialize()
{
   bool retval = true;

   plotCount = 1;

   return retval;
}


//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Advances the simulator to the next state.
 *
 * @return The state machine's finite state at the end of the method.
 */
//------------------------------------------------------------------------------
Solver::SolverState BatchEstimatorBase::AdvanceState()
{
#ifdef DEBUG_STATE_MACHINE
   MessageInterface::ShowMessage("BatchEstimatorBase::AdvanceState():  entered: currentState = %d\n", currentState);
#endif
   try
   {
      switch (currentState)
      {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "INITIALIZING\n");
         #endif
         // ReportProgress();
         CompleteInitialization();
         break;
      case PROPAGATING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "PROPAGATING\n");
         #endif
         // ReportProgress();
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "CALCULATING\n");
         #endif
         // ReportProgress();
         CalculateData();
         break;

      case LOCATING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "LOCATING\n");
         #endif
         // ReportProgress();
         ProcessEvent();
         break;

      case ACCUMULATING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "ACCUMULATING\n");
         #endif
         // ReportProgress();
         Accumulate();
         break;

      case ESTIMATING:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "ESTIMATING\n");
         #endif
         // ReportProgress();
         Estimate();

         // Add .mat and .json data TODO
         AddMatlabIterationData();
         if (writeJsonFile)
            AddJsonIterationData();
         if (writeMatFile)
         {
            if (matWriter != NULL)
               if (!AddMatData(matData, iterationsTaken))
                  throw EstimatorException("Error adding .mat data file");
         }
         break;

      case CHECKINGRUN:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "CHECKINGRUN\n");
         #endif
         // ReportProgress();
         CheckCompletion();
         break;

      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "FINISHED\n");
         #endif
         RunComplete();
         // ReportProgress();
         break;

      default:
         #ifdef DEBUG_STATE_MACHINE
         MessageInterface::ShowMessage("Entered Estimator state machine: "
            "Bad state for an estimator.\n");
         #endif
         /* throw EstimatorException("Solver state not supported for the simulator")*/;
      }
   }
   catch (...) //(EstimatorException ex)
   {
      currentState = FINISHED;
      throw; // ex;
   }

#ifdef DEBUG_STATE_MACHINE
   MessageInterface::ShowMessage("BatchEstimatorBase::AdvanceState():  exit\n");
#endif
   return currentState;
}

//------------------------------------------------------------------------------
//  bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the simulator.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::Finalize()
{
   bool retval = false;

   if (Estimator::Finalize())
   {
      retval = true;
      measManager.Finalize();
      esm.MapVectorToObjects();
      esm.MapCovariancesToObjects();
   }

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
bool BatchEstimatorBase::IsFinalPass()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Methods used in the finite state machine
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * This method completes initialization for the Simulator object, initializing
 * its MeasurementManager, retrieving the epoch and setting the state.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::CompleteInitialization()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimatorBase state is INITIALIZING\n");
      MessageInterface::ShowMessage("advanceToEstimationEpoch = %s\n", (advanceToEstimationEpoch?"true":"false"));
   #endif
   
   Estimator::Initialize();
   
   if (esm.HasElementsOfType("EstimatedParameter"))
      throw EstimatorException("The Estimator '" + GetName() + "' does not allow "
         "solving for EstimatedParameter resources");
   
   ObjectArray satArray;
   if (advanceToEstimationEpoch == false)
   {
      //ObjectArray satArray;
      esm.MapVectorToObjects();
      esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
      estimationState = esm.GetState();
      stateSize       = estimationState->GetSize();

      Estimator::CompleteInitialization();
      
      // If estimation epoch not set, use the epoch from the prop state
      if ((estEpochFormat == "FromParticipants") || (estimationEpochGT <= 0.0))
      {
         ObjectArray participants;
         esm.GetStateObjects(participants, Gmat::SPACEOBJECT);
		 
         if (needsSatPropMap)
         {
            BuildSatPropMap();
         }

		 std::string currentSatName;
         for (UnsignedInt i = 0; i < participants.size(); ++i)
         {
            if(participants[i]->IsOfType("Spacecraft"))
            {
               currentSatName = ((Spacecraft *)(participants[i]))->GetName();
               estimationEpochGT = ((Spacecraft *)(participants[i]))->GetEpochGT();
               if (satPropMap.find(currentSatName) != satPropMap.end())
                  if (!(satPropMap[currentSatName]->GetPropagator()->IsOfType("EphemerisPropagator")))
                     break;
            }
         }
		 
         if(estimationEpochGT <= 0.0) 
            estimationEpochGT = ((SpaceObject *)(participants[participants.size()]))->GetEpochGT();
		
      }
      
      // Set the current epoch based on the first spacecraft in the ESM
      if(satArray.size() == 0)
         throw EstimatorException("Cannot initialized the estimator: there are "
               "no Spacecraft in the estimation state manager");
      
      currentEpochGT = ((Spacecraft*)satArray[0])->GetEpochGT();
      
      // Set all solve-for and consider objects to tracking data adapters
      // Note that: it only sets for tracking data adapters. For measurement models, 
      // it does not has this option due to old GMAT Nav syntax will be removed, 
      // so we do not need to implement this option.
      ObjectArray objects;
      esm.GetStateObjects(objects);
      std::vector<TrackingDataAdapter*> adapters = measManager.GetAllTrackingDataAdapters();
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
         adapters[i]->SetUsedForObjects(objects);
      
      // Now load up the observations
      measManager.PrepareForProcessing(false);
      
///// Check for more generic approach
      measManager.LoadRampTables();      
      
      if (!(fabs((currentEpochGT - estimationEpochGT).GetTimeInSec()) <= ESTTIME_ROUNDOFF))
      {
		  if (hasInitialized) {
			  advanceToEstimationEpoch = true;
			  nextMeasurementEpochGT = estimationEpochGT;
			  currentState = PROPAGATING;
			  return;
		  }
      }
   }
   
   // Show all residuals plots
   if (showAllResiduals)
   {
      // Remove all existing residual plots
      for (UnsignedInt i = 0; i < residualPlots.size(); ++i)
         delete residualPlots[i];
      residualPlots.clear();
      
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
      {
         StringArray plotMeasurements;
         plotMeasurements.push_back(modelNames[i]);
         std::string plotName = instanceName + "_" + modelNames[i] +
               "_Residuals";
         BuildResidualPlot(plotName, plotMeasurements);
      }
   }
   
   advanceToEstimationEpoch = false;

   // First measurement epoch is the epoch of the first measurement.  Duh.
   nextMeasurementEpochGT = measManager.GetEpochGT();
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "Init complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), stateCovariance->GetCovariance()->ToString().c_str());
   #endif

   if (abs((nextMeasurementEpochGT - estimationEpochGT).GetDays()) > 30)
      MessageInterface::ShowMessage("WARNING: Initial Epoch is %d days away from the first estimation epoch, propagation may take longer than expected.\n", abs((nextMeasurementEpochGT - estimationEpochGT).GetDays()));

   // [Lambda] = [0] 
   information.SetSize(stateSize, stateSize);
   for (UnsignedInt i = 0; i <  stateSize; ++i)
      for (UnsignedInt j = 0; j <  stateSize; ++j)
         information(i,j) = 0.0;
   
   residuals.SetSize(stateSize);
   x0bar.SetSize(stateSize);

   for (Integer i = 0; i < information.GetNumRows(); ++i)
   {
      residuals[i] = 0.0;
         x0bar[i] = 0.0;                                              // it is delta_XTile(i) in equation 8-22 in GTDS MathSpec. Initialy its value is zero-vector 
   }
   
	
   esm.BufferObjects(&outerLoopBuffer);
   esm.MapObjectsToVector();
	
   estimationStateS = esm.GetEstimationState();

   estimationStatus = UNKNOWN;
   // Convert estimation state from GMAT internal coordinate system to participants' coordinate system
   aprioriMJ2000EqSolveForState = esm.GetEstimationState();

   aprioriSolveForState = esm.GetEstimationStateForReport();
   aprioriSolveForStateMA = esm.GetEstimationStateForReport("MA");
   aprioriSolveForStateC = esm.GetEstimationCartesianStateForReport();
   aprioriSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

   isInitialized = true;
   numDivIterations = 0;                       // It need to reset its value when starting estimatimation calculation

   
   // Get list of signal paths and specify the length of participants' column
   std::vector<StringArray> signalPaths = measManager.GetSignalPathList();
   for(UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      Integer len = 0;
      for (UnsignedInt j = 0; j < signalPaths[i].size(); ++j)
      {
         GmatBase* obj = GetConfiguredObject(signalPaths[i].at(j));
         std::string id = "";
         if (obj->IsOfType(Gmat::SPACECRAFT))
            id = ((Spacecraft*)obj)->GetStringParameter("Id");
         else if (obj->IsOfType(Gmat::GROUND_STATION))
            id = ((GroundstationInterface*)obj)->GetStringParameter("Id");
         
         len = len + id.size() + 1;
      }
      if (pcolumnLen < len)
         pcolumnLen = len;
   }
	
   WriteToTextFile();
   ReportProgress();
   
   numRemovedRecords["U"] = 0;
   numRemovedRecords["R"] = 0;
   numRemovedRecords["B"] = 0;
   numRemovedRecords["OLSE"] = 0;
   numRemovedRecords["ILSE"] = 0;
   numRemovedRecords["IRMS"] = 0;
   numRemovedRecords["USER"] = 0;
   numRemovedRecords["HORP"] = 0;
   numRemovedRecords["EGAP"] = 0;
   numRemovedRecords["N"]    = 0;           // Edit status "-"

   if (fabs((currentEpochGT - nextMeasurementEpochGT).GetTimeInSec()) <= ESTTIME_ROUNDOFF)
      currentState = CALCULATING;
   else
   {
      timeStep = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();
      currentState = PROPAGATING;
   }
   
   // Clear warning message's count
   warningCount = 0;


   // Verify no two different ground station having the same Id
   std::string errMsg = "";
   if (GetMeasurementManager()->ValidateDuplicationOfGroundStationID(errMsg) == false)
   {
      errMsg = "Error: " + errMsg + " in batch estimator '" + GetName() + "'.\n";
      throw EstimatorException(errMsg);
   }

   
   /// Recalculate all conversion derivative matrixes for the new estimation state
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
   
   hasInitialized = true;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("BatchEstimatorBase::CompleteInitialization "
            "process complete\n");
      MessageInterface::ShowMessage("   Estimation state = [");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Information Matrix = \n");
      for (Integer i = 0; i < information.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (Integer j = 0; j < information.GetNumColumns(); ++j)
         {
            MessageInterface::ShowMessage(" %.12lf ", information(i, j));
         }
         MessageInterface::ShowMessage("]\n");
      }
      MessageInterface::ShowMessage("   Residuals = [");
      for (Integer i = 0; i < residuals.GetSize(); ++i)
         MessageInterface::ShowMessage(" %.12lf ", residuals[i]);
      MessageInterface::ShowMessage("]\n");
   #endif
}


//------------------------------------------------------------------------------
//  void FindTimeStep()
//------------------------------------------------------------------------------
/**
 * This method determines whether the simulation is finished or still
 * calculating, and if neither, computes the timeStep.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::FindTimeStep()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("BatchEstimatorBase::FindTimeStep() "
            "current epoch = %s, next epoch = %s\n",
            currentEpochGT.ToString().c_str(), nextMeasurementEpochGT.ToString().c_str());
   #endif

   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimatorBase state is PROPAGATING\n");
   #endif

   if (advanceToEstimationEpoch == true)
   {
      if (fabs((currentEpochGT - estimationEpochGT).GetTimeInSec()) <= ESTTIME_ROUNDOFF)
      {
         timeStep = 0.0;
         currentState = INITIALIZING;
         return;
      }
      timeStep = (estimationEpochGT - currentEpochGT).GetTimeInSec();

      return;
   }

   if (nextMeasurementEpochGT == 0.0)
   {
      // Estimate and check for convergence after processing measurements
      currentState = ESTIMATING;
      #ifdef WALK_STATE_MACHINE
         MessageInterface::ShowMessage("Next state will be ESTIMATING\n");
      #endif
   }
   //else if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch))       // value of accuracy is set to 5.0e-12 due to the accuracy limit of double
   else if (fabs((currentEpochGT - nextMeasurementEpochGT).GetTimeInSec()) <= ESTTIME_ROUNDOFF)
   {
      // We're at the next measurement, so process it
      currentState = CALCULATING;
      #ifdef WALK_STATE_MACHINE
         MessageInterface::ShowMessage("Next state will be CALCULATING\n");
      #endif
   }
   else
   {
      // Calculate the time step in seconds and stay in the PROPAGATING state;
      timeStep = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();
      currentState = PROPAGATING;
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   timestep = %.12lf; nextepoch = "
               "%s; current = %s\n", timeStep, nextMeasurementEpochGT.ToString().c_str(),
               currentEpochGT.ToString().c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
//  void CalculateData()
//------------------------------------------------------------------------------
/**
 * This method checks for valid measurements and changes state based on the
 * results.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::CalculateData()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("Entered BatchEstimatorBase::CalculateData()\n");
   #endif

   // Update the STM
   esm.MapObjectsToSTM();
   
   // We need to run CalculateMeasurements() with event in order to get correct result in signal block status
   //if (measManager.CalculateMeasurements() == false)
   if (measManager.CalculateMeasurements(false, true, false) == false)
   {
      currentState = ACCUMULATING;
   }
   else if (measManager.GetEventCount() > 0)
   {
      currentState = LOCATING;
      locatingEvent = true;
   }
   else
      currentState = ACCUMULATING;

   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("Exit BatchEstimatorBase::CalculateData()\n");
   #endif

}


//------------------------------------------------------------------------------
//  void ProcessEvent()
//------------------------------------------------------------------------------
/**
 * This method manages the state machine operations while processing events.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::ProcessEvent()
{
   locatingEvent = false;

   for (UnsignedInt i = 0; i < activeEvents.size(); ++i)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("*** Checking event %d\n", i);
      #endif
      if (((Event*)activeEvents[i])->CheckStatus() != LOCATED)
      {
         locatingEvent = true;
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d not yet located\n", i);
         #endif
      }
      else
      {
         if (measManager.ProcessEvent((Event*)activeEvents[i]) == false)
            MessageInterface::ShowMessage("Event %d located but not "
                  "processed!\n", i);
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d located!\n", i);
         #endif
      }
   }

   // Check each active event.  If all located, move into the ACCUMULATING state
   if (!locatingEvent)
   {
      currentState = ACCUMULATING;
   }
}


//------------------------------------------------------------------------------
//  void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * This method tests to see if the estimation algorithm has converged.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::CheckCompletion()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimatorBase state is CHECKINGRUN\n");
   #endif

   #ifdef DEBUG_VERBOSE
      MessageInterface::ShowMessage("\nCompleted iteration %d\n\n",
            iterationsTaken+1);
   #endif

   convergenceReason = "";
   estimationStatus = TestForConvergence(convergenceReason);
   
   // Reset best RMS as needed                           // fix bug GMT-5711
   if (resetBestRMSFlag)                                 // fix bug GMT-5711
   {                                                     // fix bug GMT-5711
      if (estimationStatus == DIVERGING)                 // fix bug GMT-5711
         resetBestResidualRMS = newResidualRMS;          // fix bug GMT-5711
   }                                                     // fix bug GMT-5711

   #ifdef RUN_SINGLE_PASS
      converged = true;
   #endif
   
   ++iterationsTaken;

   // clear cache after each iteration
   measManager.ClearIonosphereCache();

   if ((estimationStatus == ABSOLUTETOL_CONVERGED) ||
      (estimationStatus == RELATIVETOL_CONVERGED) ||
      (estimationStatus == ABS_AND_REL_TOL_CONVERGED) ||
      (estimationStatus == MAX_CONSECUTIVE_DIVERGED) ||
      (estimationStatus == MAX_ITERATIONS_DIVERGED) ||
	  (estimationStatus == RUN_INITIAL_GUESS))
   {
      if ((estimationStatus == ABSOLUTETOL_CONVERGED) ||
         (estimationStatus == RELATIVETOL_CONVERGED) ||
         (estimationStatus == ABS_AND_REL_TOL_CONVERGED))
         status = Gmat::CONVERGED;
      else
         status = Gmat::EXCEEDED_ITERATIONS;
      
      GmatGlobal::Instance()->SetSolverStatus(instanceName, status);

      currentState = FINISHED;
   }
   else
   {
      if (showAllResiduals)
         PlotResiduals();

      currentEpochGT = estimationEpochGT;
      measManager.Reset();                                            // set current observation data to be the first one in observation data table
      nextMeasurementEpochGT = measManager.GetEpochGT();
      
      information.SetSize(stateSize, stateSize);
      for (UnsignedInt i = 0; i <  stateSize; ++i)
         for (UnsignedInt j = 0; j <  stateSize; ++j)
            information(i,j) = 0.0;
      
      ResetSTM();
      esm.MapSTMToObjects();
      
      for (UnsignedInt i = 0; i < information.GetNumRows(); ++i)
         residuals[i] = 0.0;
      

      // Recalculate x0bar for each iteration
      GmatState currState = esm.GetEstimationState();
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         /// delta_XTile(i+1) = X[0] - X[i+1] = (X[0] - X[i]) - (X[i+1] - X[i]) = delta_X_Tile(i) - dx
         //x0bar[j] -= dx[j];
         x0bar[j] = initialEstimationStateS[j] - currState[j];        // state  type could be Cartesian or Keplerian depend on solve-for variable
      }

      #ifdef DEBUG_VERBOSE
         MessageInterface::ShowMessage("Starting iteration %d\n\n",
               iterationsTaken+1);
      #endif

      #ifdef DEBUG_ITERATIONS
         MessageInterface::ShowMessage(
               "Init complete!\n   STM = %s\n   Covariance = %s\n",
               stm->ToString().c_str(), covariance->ToString().c_str());
      #endif
      
      WriteToTextFile();
      ReportProgress();
      
      // After writing to GmatLog.txt file, bestResidualRMS is set to resetBestResdualRMS    // fix bug GMT-5711
      if ((resetBestRMSFlag) && (estimationStatus == DIVERGING))                             // fix bug GMT-5711
         bestResidualRMS = resetBestResidualRMS;                                             // fix bug GMT-5711

      numRemovedRecords["U"] = 0;
      numRemovedRecords["R"] = 0;
      numRemovedRecords["B"] = 0;
      numRemovedRecords["OLSE"] = 0;
      numRemovedRecords["ILSE"] = 0;
      numRemovedRecords["IRMS"] = 0;
      numRemovedRecords["USER"] = 0;
      numRemovedRecords["HORP"] = 0;
      numRemovedRecords["EGAP"] = 0;
      numRemovedRecords["N"]    = 0;

      measStats.clear();
      stationsList.clear();
      measTypesList.clear();


      // Clear all media correct warning lists
      ionoWarningList.clear();
      tropoWarningList.clear();

      // Get new estimationStateS after it reset all Cr_Epsilon and Cd_Epsilon
      // Note that: All epsilon parameters such as Cr_Epsilon and Cd_Epsilon are 0 at the starting point of the next iteration
      estimationStateS = esm.GetEstimationState();

      if (fabs((currentEpochGT - nextMeasurementEpochGT).GetTimeInSec()) <= ESTTIME_ROUNDOFF)
         currentState = CALCULATING;
      else
      {
         timeStep = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();
         currentState = PROPAGATING;
      }
   }
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * This method updates the simulator text file at the end of a simulator run.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::RunComplete()
{
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("BatchEstimatorBase state is FINALIZING\n");
   #endif

   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         (*stateCovariance)(i, j) = informationInverse(i, j);
   }
   esm.MapCovariancesToObjects();

   measManager.ProcessingComplete();

   // Report the results
   WriteToTextFile();
   AddMatlabConfigData();
   ReportProgress();

   if (showAllResiduals)
      PlotResiduals();


   // Clean up memory
   //for (UnsignedInt i = 0; i < hTilde.size(); ++i)
   //   hTilde[i].clear();
   //hTilde.clear();

   //measStats.clear();
   //stationsList.clear();
   //measTypesList.clear();

   if (writeMatFile && (matWriter != NULL))
   {
      if (!WriteMatData())
         throw EstimatorException("Error writing .mat data file");

      matWriter->CloseFile();
   }

   if (writeJsonFile)
      WriteJsonData();
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string BatchEstimatorBase::GetProgressString()
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
                        << "    Performing Estimation "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "    " ;
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

         case CHECKINGRUN:
            progress << "\n   WeightedRMS residuals for this iteration : "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals                        : "
                     << bestResidualRMS;
            if ((resetBestRMSFlag) && (estimationStatus == DIVERGING))                 // fix bug GMT-5711
            {                                                                          // fix bug GMT-5711
            progress << "\n   Reset value of BestRMS residuals         : "             // fix bug GMT-5711
                        << resetBestResidualRMS;                                       // fix bug GMT-5711
            }                                                                          // fix bug GMT-5711
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n";
         
            switch(estimationStatus)
            {
            case ABSOLUTETOL_CONVERGED:
               progress << "This iteration is converged due to absolute tolerance convergence criteria\n";
               break;
            case RELATIVETOL_CONVERGED:
               progress << "This iteration is converged due to relative convergence criteria \n";
               break;
            case ABS_AND_REL_TOL_CONVERGED:
               progress << "This iteration is converged due to boths: absolute and relative convergence criteria\n";
               break;
            case MAX_CONSECUTIVE_DIVERGED:
               progress << "This iteration is diverged due to maximum consecutive diverged criteria\n";
               break;
            case CONVERGING:
               progress << "This iteration is converging\n";
               break;
            case DIVERGING:
               progress << "This iteration is diverging\n";
               break;
            }
            progress << "\n";

            progress << "------------------------------"
                     << "------------------------\n"
                     << "Iteration " << iterationsTaken
                     << "\n\nCurrent estimated state:\n";
            taiMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD);
            utcMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                  GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
            utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
            progress << "   Estimation Epoch:\n";
            progress << "   " << estimationEpochGT.ToString() << " A.1 modified Julian\n";
            progress << "   " << taiMjdEpoch.ToString() << " TAI modified Julian\n";
            progress << "   " << utcEpoch << " UTCG\n";


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

            break;

         case FINISHED:
            progress << "\n   WeightedRMS residuals for this iteration : "
                     << newResidualRMS;
            progress << "\n   BestRMS residuals                        : "
                     << bestResidualRMS;
            progress << "\n   PredictedRMS residuals for next iteration: "
                     << predictedRMS << "\n";

            switch(estimationStatus)
            {
            case ABSOLUTETOL_CONVERGED:
               progress << "This iteration is converged due to absolute tolerance convergence criteria.\n";
               break;
            case RELATIVETOL_CONVERGED:
               progress << "This iteration is converged due to relative convergence criteria.\n";
               break;
            case ABS_AND_REL_TOL_CONVERGED:
               progress << "This iteration is converged due to boths: absolute and relative convergence criteria.\n";
               break;
            case MAX_CONSECUTIVE_DIVERGED:
               progress << "This iteration is diverged due to maximum consecutive diverged criteria.\n";
               break;
            case MAX_ITERATIONS_DIVERGED:
               progress << "This iteration is diverged due to exceeding the maximum iterations.\n";
               break;
            case CONVERGING:
               progress << "This iteration is converging.\n";
               break;
            case DIVERGING:
               progress << "This iteration is diverging.\n";
               break;
			case RUN_INITIAL_GUESS:
			   progress << "This iteration ended due to Run Initial Guess being set.\n";
			   break;
            }
            progress << "\n";

            progress << "\n****************************"
                     << "****************************\n"
                     << "*** Estimation Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n\n"
                     << "Estimation ";
            switch(estimationStatus)
            {
            case ABSOLUTETOL_CONVERGED:
            case RELATIVETOL_CONVERGED:
            case ABS_AND_REL_TOL_CONVERGED:
               progress << "converged!\n";
               break;
            case MAX_CONSECUTIVE_DIVERGED:
            case MAX_ITERATIONS_DIVERGED:
            case CONVERGING:
            case DIVERGING:
               progress << "did not converge!\n";
               break;
			case RUN_INITIAL_GUESS:
			   progress << "ran initial guess only\n";
			   return progress.str();
            case UNKNOWN:
               break;
            };
            
            progress   << "   " << convergenceReason << "\n"
                       << "Final Estimated State:\n\n";

            if (estEpochFormat != "FromParticipants")
               progress << "   Estimation Epoch (" << estEpochFormat
                        << "): " << estEpoch << "\n";
            else
            {
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

            }

            if (textFileMode == "Verbose")
            {
               progress << "\n   WeightedRMS residuals for previous iteration: "
                        << oldResidualRMS;
               progress << "\n   WeightedRMS residuals                       : "
                        << newResidualRMS;
               progress << "\n   BestRMS residuals for this iteration        : "
                        << bestResidualRMS << "\n\n";
            }

            //finalCovariance = information.Inverse();
            finalCovariance = informationInverse;
            CovarianceEpsilonConversion(finalCovariance);

            MessageInterface::ShowMessage("%s\n", progress.str().c_str());
            progress.str("");
            
            // Display final covariance matrix
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
               MessageInterface::ShowMessage("%s", progress.str().c_str());
               progress.str("");
            }

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
               MessageInterface::ShowMessage("%s", progress.str().c_str());
               progress.str("");
            }

            progress << "\n****************************"
                     << "****************************\n"
                     << std::endl;
            MessageInterface::ShowMessage("%s", progress.str().c_str());
            progress.str("");

            #ifdef DUMP_FINAL_RESIDUALS
            {
               std::fstream residFile;
               residFile.open("FinalResiduals.csv", std::ios::out);

               for (UnsignedInt i = 0; i < measurementResiduals.size(); ++i)
               {
                  residFile << i << "   " << measurementEpochs[i]
                            << "   " << measurementResiduals[i] << "\n";
               }

               residFile.close();
            }
            #endif

            break;

         default:
            throw EstimatorException(
               "Solver state not supported for the simulator");
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
bool BatchEstimatorBase::OverwriteEditFlag(const std::string &editFlag)
{
   bool iterationFrozen = freezeEditing && (iterationsTaken >= freezeIteration);
   bool flagIsFrozen = ((editFlag == "IRMS") || (editFlag == "OLSE") || (editFlag == "ILSE"));
   return !(iterationFrozen && flagIsFrozen);
}


//------------------------------------------------------------------------------
// void WriteEditFlag()
//------------------------------------------------------------------------------
/**
 * This method indicates if a new edit flag is to be written
 */
//------------------------------------------------------------------------------
bool BatchEstimatorBase::WriteEditFlag()
{
   bool iterationFrozen = freezeEditing && (iterationsTaken >= freezeIteration);
   return !iterationFrozen;
}


//------------------------------------------------------------------------------
// Integer TestForConvergence(std::string &reason)
//------------------------------------------------------------------------------
/**
 * Provides the default convergence test for the BatchEstimators.
 *
 * GMAT's batch estimator report a converged solution when any of the following
 * criteria are met:
 *
 * 1. Aboslute Tolerance test: |RMS| <= Absolute Tol
 *
 * 2. Relative Tolerance test: |1 - RMSP/RMSB| <= Relative Tol
 *
 * 3. Maximum consecutive divergence test
 * 
 * 4. Converging and diverging tests
 *
 * Note that: tests 2, 3, and 4 only perform after iteration 0
 *
 * @param reason A string that is set to indicate the criterion that was met
 *               when convergence is detected.  The string contains text for all
 *               criteria that are satisfied.
 *
 * @return true if the estimator has converged, false if not
 */
//------------------------------------------------------------------------------
Integer BatchEstimatorBase::TestForConvergence(std::string &reason)
{
   Integer retval = UNKNOWN;
   std::stringstream why;
   
   if (currentMode == INITIAL_GUESS)
   {
	   retval = RUN_INITIAL_GUESS;
	   why << "";
	   reason = why.str();
	   return retval;
   }

   // AbsoluteTol test
   if (newResidualRMS <= absoluteTolerance)
   {
      why << "   WeightedRMS residual, " << newResidualRMS
          << " is within the AbsoluteTol, " << absoluteTolerance
          << "\n";

      reason = why.str();
      retval = ABSOLUTETOL_CONVERGED;
   }

   // RelativeTol test
   if (GmatMathUtil::Abs((predictedRMS - bestResidualRMS)/bestResidualRMS) <= relativeTolerance)
   {
      why << "   |1 - RMSP/RMSB| = | 1- " << predictedRMS << " / " << bestResidualRMS << "| = " 
          << GmatMathUtil::Abs(1 - predictedRMS/bestResidualRMS)
          << " is less than RelativeTol, "
          << relativeTolerance << "\n";

      reason = why.str();
      if (retval == ABSOLUTETOL_CONVERGED)
         retval = ABS_AND_REL_TOL_CONVERGED;
      else
         retval = RELATIVETOL_CONVERGED;
   }
   if (retval != UNKNOWN)
      return retval;


   if (iterationsTaken == (maxIterations-1))
   {
      retval = MAX_ITERATIONS_DIVERGED;
      why << "Number of iterations reached its maximum value (" << maxIterations << ").\n";
      reason = why.str();
      return retval;
   }

   if (iterationsTaken >= 1)
   {
      // Maximmum consecutive divergence test
      if (newResidualRMS > oldResidualRMS)
      {
         numDivIterations++;
         if (numDivIterations >= maxConsDivergences)
         {
            why << "Number of consecutive divergences reached its maximum value (" << maxConsDivergences << ").\n";
            reason = why.str();
            retval = MAX_CONSECUTIVE_DIVERGED;
         }
         else
            retval = DIVERGING;
      }
      else
      {
         numDivIterations = 0;
         retval = CONVERGING;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2b()
//------------------------------------------------------------------------------
/**
* This function is used to write apriori covariance matrix to report file.
*/
//------------------------------------------------------------------------------
void BatchEstimatorBase::WriteReportFileHeaderPart2b()
{
   if (useApriori)
      Estimator::WriteReportFileHeaderPart2b();
}


//----------------------------------------------------------------------
// void WriteIterationHeader()
//----------------------------------------------------------------------
/**
* Write iteration header
*/
//----------------------------------------------------------------------
void BatchEstimatorBase::WriteIterationHeader()
{
   /// 1. Write iteration header
   textFile
      << "************************************************************  ITERATION " << GmatStringUtil::ToString(iterationsTaken, 3) << ":  MEASUREMENT RESIDUALS  ***********************************************************\n"
      << "\n";
   
   if ((freezeEditing) && (iterationsTaken >= freezeIteration ))
   {
      textFile
      << "                                                               *** Residual Editing is Frozen ***\n"
      << "\n";
   }

   textFile
      << "                                                                  Notations Used In Report File\n"
      << "\n"
      << "                         -    : Not edited                                                    IRMS : Edited by initial RMS sigma filter\n"
      << "                         U    : Unused because no computed value configuration available      OLSE : Edited by outer-loop sigma editor\n"
      << "                         R    : Out of ramp table range                                       ILSE : Edited by inner-loop sigma editor\n"
      << "                         BXY  : Blocked, X = Path index, Y = Count index(Doppler)             USER : Edited by second-level data editor\n"
      << "                         HORP : Blocked by Height of Ray Path criteria                        EGAP : Edited due to ephemeris gap\n";
   
   
   textFile
      << "\n"
      << "                                                                  Measurement and Residual Units\n"
      << "\n";
   
   WriteMeasurementHeader();

   textFile.flush();

   WritePageHeader();
}


//------------------------------------------------------------------------------
// bool IsIterative()
//------------------------------------------------------------------------------
/**
* This function indicates if this estimator is iterative.
*/
//------------------------------------------------------------------------------
bool BatchEstimatorBase::IsIterative()
{
   return true;
}


//----------------------------------------------------------------------
// std::string GetHeaderName()
//----------------------------------------------------------------------
/**
* Write the name of the estimator type in upper case for report headers
*/
//----------------------------------------------------------------------
std::string BatchEstimatorBase::GetHeaderName()
{
   return "ITERATION " + GmatStringUtil::ToString(iterationsTaken, 3) + ":";
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const MeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::AddMatlabData(const MeasurementInfoType &measStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabData(measStat, matData, matIndex);
}

//------------------------------------------------------------------------------
//  void AddJsonData(const MeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method adds Observation and Iteration data to the JSON output file
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::AddJsonData(const MeasurementInfoType &measStat)
{
   Estimator::AddJsonObservationData(measStat);
   Estimator::AddJsonIterationData(measStat);
}

//------------------------------------------------------------------------------
//  void AddMatlabData(const UpdateInfoType &measStat,
//                     DataBucket &matData, IntegerMap &matIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void BatchEstimatorBase::AddMatlabData(const MeasurementInfoType &measStat,
                                       DataBucket &matData, IntegerMap &matIndex)
{
   Estimator::AddMatlabData(measStat, matData, matIndex);

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   Integer matMeasIndex = matData.GetContainerSize() - 1;

   if (matIndex.count("Epoch") == 0)
   {
      matConfigIndex["Epoch"] = matConfigData.AddReal2DArray("EstimationEpochUTC");
   }

   Real utcEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD).GetMjd();
   matConfigData.real2DArrayValues[matIndex["Epoch"]] = { { utcEpoch + MATLAB_DATE_CONVERSION }, { utcEpoch } };
}


//------------------------------------------------------------------------------
//  void AddMatlabConfigData()
//------------------------------------------------------------------------------
/**
 * This method adds the estimation configuration data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::AddMatlabConfigData()
{
   Estimator::AddMatlabConfigData();

   if (writeMatFile && (matWriter != NULL))
   {
      const std::vector<ListItem*> *stateMap = esm.GetStateMap();

      matConfigIndex["CartNames"] = matConfigData.AddString2DArray("CartesianStateNames");
      matConfigIndex["KeplNames"] = matConfigData.AddString2DArray("KeplerianStateNames");

      for (Integer ii = 0; ii < stateMap->size(); ii++)
      {
         matConfigData.string2DArrayValues[matConfigIndex["CartNames"]].push_back({ GetElementFullName((*stateMap)[ii], false, "Cartesian") });
         matConfigData.string2DArrayValues[matConfigIndex["KeplNames"]].push_back({ GetElementFullNameMJ2000Eq((*stateMap)[ii], false, "Keplerian", "MA") });
      }
   }
}


////--------------------------------------------------------------------------------------------
//// Rmatrix66 CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
////--------------------------------------------------------------------------------------------
///**
//* This function is use to calculate derivative state conversion matrix for a spacecraft state. 
//* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
//*
//* @param obj      it is a spacecraft object
//* @param state    Cartesian state of the spacecraft 
//*
//* return          6x6 derivative state conversion matrix [dX/dK]
//*/
////--------------------------------------------------------------------------------------------
//Rmatrix66 BatchEstimatorBase::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
//{
//   // 1. Get mu value 
//   Spacecraft* spacecraft = (Spacecraft*)obj;
//   CoordinateSystem* cs = (CoordinateSystem*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
//   CelestialBody* body = (CelestialBody*)(cs->GetOrigin());
//   Real mu = body->GetRealParameter(body->GetParameterID("Mu"));
//   
//   // 2. Specify conversion matrix
//   Rmatrix66 convMatrix = StateConversionUtil::CartesianToKeplerianDerivativeConversion(mu, state);
//   
//   return convMatrix;
//}


////--------------------------------------------------------------------------------------------
//// Rmatrix BatchEstimatorBase::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
////--------------------------------------------------------------------------------------------
///**
//* This function is use to calculate derivative state conversion matrix for all solve-for variables. 
//* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
//*
//* @param stateMap a map of spacecrafts and their Cartisian state
//*
//* return          derivative state conversion matrix
//*/
////--------------------------------------------------------------------------------------------
//Rmatrix BatchEstimatorBase::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
//{
//   const std::vector<ListItem*> *map = esm.GetStateMap();
//
//   // 1. Specify conversion sub matrixes for all spacecrafts
//   std::map<GmatBase*, Rmatrix66> matrixMap;
//   for (std::map<GmatBase*, Rvector6>::iterator i = stateMap.begin(); i != stateMap.end(); ++i)
//      matrixMap[i->first] = CartesianToKeplerianCoverianceConvertionMatrix(i->first, i->second).Inverse();   // It is [dK/dX] matrix
//
//   // 2. Assemly whole conversion matrix
//   Rmatrix conversionMatrix(map->size(),map->size());        // It is a zero matrix
//   for(UnsignedInt i = 0; i < map->size(); )
//   {
//      if ((*map)[i]->elementName == "CartesianState")
//      {
//         // fill in conversion submatrix
//         Rmatrix66 m = matrixMap[(*map)[i]->object];
//         for(UnsignedInt row = 0; row < 6; ++row)
//         {
//            for(UnsignedInt col = 0; col < 6; ++col)
//            {
//               conversionMatrix.SetElement(i+row, i+col, m(row,col));
//            }
//         }
//         // skip to next
//         i = i + 6;
//      }
//      else
//      {
//         conversionMatrix(i,i) = 1.0;
//         ++i;
//      }
//   }
//   
//   return conversionMatrix;      // conversionMatrix contains identity submatrixes and [dK/dX] submatrixes
//}


//------------------------------------------------------------------------------
// void InvertApriori(Rmatrix &Pdx0_inv)
//------------------------------------------------------------------------------
/**
 * This method solves the normal equations using the selected inversionType
 */
//------------------------------------------------------------------------------
void BatchEstimatorBase::InvertApriori(Rmatrix &Pdx0_inv) const
{
   try
   {
      Pdx0_inv = stateCovariance->GetCovariance()->Inverse();              // inverse of the initial estimation error covariance matrix
   }
   catch (...)
   {
      MessageInterface::ShowMessage("Apriori covariance matrix:\n[");
      for (Integer row = 0; row < stateCovariance->GetDimension(); ++row)
      {
         for (Integer col = 0; col < stateCovariance->GetDimension(); ++col)
            MessageInterface::ShowMessage("%le   ", stateCovariance->GetCovariance()->GetElement(row, col));
         if (row < stateCovariance->GetDimension() - 1)
            MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("]\n");

      throw EstimatorException("Error: Apriori covariance matrix is singular. GMAT cannot take inverse of that matrix.\n");
   }
}

