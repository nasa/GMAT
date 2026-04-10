//$Id: SeqEstimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         SeqEstimator
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
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------

#include "SeqEstimator.hpp"

#include "GmatConstants.hpp"
#include "FileUtil.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Solver.hpp"
#include "ODEModel.hpp"
#include "EstimatedParameter.hpp"

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
#include <map>
// End EKF mod

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_REPORTS

// Macros for debugging of the state machine
//#define WALK_STATE_MACHINE
//#define DUMP_RESIDUALS
//#define DEBUG_EVENT
//#define DEBUG_RUNCOMPLETE

#define DEFAULT_POSITION_COVARIANCE 100.0
#define DEFAULT_VELOCITY_COVARIANCE 0.0001
#define DEFAULT_OTHER_COVARIANCE    1.0

//------------------------------------------------------------------------------
// static data - EKF mod 12/16
//------------------------------------------------------------------------------

const UnsignedInt SeqEstimator::truthStateSize =  6;
const UnsignedInt SeqEstimator::stdColLen      = 25;
const UnsignedInt SeqEstimator::minPartSize    = 18;
const UnsignedInt SeqEstimator::strSizeMin     =  4;



const std::string
SeqEstimator::PARAMETER_TEXT[] =
{
   "ProcessNoiseTimeStep",          // The time interval to apply the process noise
   "ScaledResidualThreshold",       // The scalar multiplier for measurement sigma editing
   "DelayRectifyTimeSpan",          // The duration to delay rectifying the reference trajectory
   "MeasDeweightingSigmaThreshold", // The sigma threshold to use on the sqare root of the trace of the position elements of the covariance
   "MeasDeweightingCoefficient",    // The coefficient used for underweighting during the measurement update
   "InputWarmStartFile",            // The file to read data from to initialize the SeqEstimator
   "WarmStartEpochFormat",          // The epoch format used by WarmStartEpoch
   "WarmStartEpoch",                // The epoch to initialize the SeqEstimator from based on the InputWarmStartFile
   "OutputWarmStartFile",           // The file to write SeqEstimator data to
};

const Gmat::ParameterType
SeqEstimator::PARAMETER_TYPE[] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};
// End EKF mod

//------------------------------------------------------------------------------
// SeqEstimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
SeqEstimator::SeqEstimator(const std::string &type,
      const std::string &name) :
   Estimator               (type, name),
// EKF mod 12/16
   measSize                (0),
   processNoiseStep        (0.0),
   nextNoiseUpdateGT       (-1.0),
   delayRectifySpan        (0.0),
   deweightThreshold       (1.0),
   deweightCoeff           (0.0),
   hasAnchorEpoch          (false),
   overwriteEditFlags      (true),
   constMult               (3.0),
   inputDataFile           (""),
   restartEpochFormat      ("TAIModJulian"),
   restartEpoch            ("FirstMeasurement"),
   outputDataFile          ("")

// End EKF mod
{
   //hiLowData.push_back(&sigma);
   //showErrorBars = true;

   objectTypeNames.push_back("SeqEstimator");
   parameterCount = SeqEstimatorParamCount;
}


//------------------------------------------------------------------------------
// ~SeqEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SeqEstimator::~SeqEstimator()
{
   if (dataFile.is_open())
      dataFile.close();
}


//------------------------------------------------------------------------------
// SeqEstimator::SeqEstimator(const SeqEstimator & se) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param se The estimator that provides configuration information for this one
 */
//------------------------------------------------------------------------------
SeqEstimator::SeqEstimator(const SeqEstimator & se) :
   Estimator               (se),
// EKF mod 12/16
   measSize                (se.measSize),
   processNoiseStep        (se.processNoiseStep),
   nextNoiseUpdateGT       (se.nextNoiseUpdateGT),
   delayRectifySpan        (se.delayRectifySpan),
   deweightThreshold       (se.deweightThreshold),
   deweightCoeff           (se.deweightCoeff),
   hasAnchorEpoch          (se.hasAnchorEpoch),
   overwriteEditFlags      (se.overwriteEditFlags),
   constMult               (se.constMult),
   inputDataFile           (se.inputDataFile),
   restartEpochFormat      (se.restartEpochFormat),
   restartEpoch            (se.restartEpoch),
   outputDataFile          (se.outputDataFile)
// End EKF mod
{
   hiLowData.push_back(&sigma);
}


//------------------------------------------------------------------------------
// SeqEstimator& operator=(const SeqEstimator &se)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
 *
 * @param se The estimator that provides configuration information for this one
 *
 * @return This estimator, configured to match se
 */
//------------------------------------------------------------------------------
SeqEstimator& SeqEstimator::operator=(
      const SeqEstimator &se)
{
   if (this != &se)
   {
      Estimator::operator=(se);
      processNoiseStep = se.processNoiseStep;
      nextNoiseUpdateGT = se.nextNoiseUpdateGT;
      delayRectifySpan = se.delayRectifySpan;
      deweightThreshold = se.deweightThreshold;
      deweightCoeff = se.deweightCoeff;
      hasAnchorEpoch = se.hasAnchorEpoch;
      overwriteEditFlags = se.overwriteEditFlags;
      constMult = se.constMult;
      inputDataFile = se.inputDataFile;
      restartEpochFormat = se.restartEpochFormat;
      restartEpoch = se.restartEpoch;
      outputDataFile = se.outputDataFile;
   }
   return *this;
}

// EKF Mod 12/16
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
std::string SeqEstimator::GetParameterText(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SeqEstimatorParamCount)
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
std::string SeqEstimator::GetParameterUnit(const Integer id) const
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
Integer SeqEstimator::GetParameterID(const std::string &str) const
{
   // This section is used to throw an exception for unused parameters
   if (str == "MaximumIterations")
      throw SolverException("Syntax error: Sequential Estimator '" + GetName() + "' does not has parameter '" + str + "'.\n");

   for (Integer i = EstimatorParamCount; i < SeqEstimatorParamCount; i++)
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
Gmat::ParameterType SeqEstimator::GetParameterType(const Integer id) const
{
   if (id >= EstimatorParamCount && id < SeqEstimatorParamCount)
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
bool SeqEstimator::IsParameterReadOnly(const Integer id) const
{
   if (id == maxIterationsID)
      return true;

   if (id == ESTIMATION_EPOCH_FORMAT)
      return true;

   if (id == ESTIMATION_EPOCH)
      return true;
   if (id == PROCESS_NOISE_TIME_STEP)
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
bool SeqEstimator::IsParameterReadOnly(const std::string &label) const
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
std::string SeqEstimator::GetParameterTypeString(const Integer id) const
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
Real SeqEstimator::GetRealParameter(const Integer id) const
{
   if (id == PROCESS_NOISE_TIME_STEP)
      return processNoiseStep;

   if (id == CONSTANT_MULTIPLIER)
      return constMult;

   if (id == DELAY_RECTIFY_TIME)
      return delayRectifySpan;

   if (id == MEAS_DEWEIGHT_THRESHOLD)
      return deweightThreshold;

   if (id == MEAS_DEWEIGHT_COEFF)
      return deweightCoeff;

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
Real SeqEstimator::SetRealParameter(const Integer id, const Real value)
{
   if (id == PROCESS_NOISE_TIME_STEP)
   {
      if (value >= 0.0)
           processNoiseStep = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" cannot be negative\n");

      MessageInterface::ShowMessage("*** Warning *** the parameter \"ProcessNoiseTimeStep\" is "
         "deprecated and may be removed from a future build; please use \"UpdateTimeStep\" "
         "on the ProcessNoiseModel instead.\n");
      return processNoiseStep;
   }

   if (id == CONSTANT_MULTIPLIER)
   {
      if (value > 0.0)
           constMult = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not a positive number\n");

      return constMult;
   }

   if (id == DELAY_RECTIFY_TIME)
   {
      if (value >= 0.0)
         delayRectifySpan = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return delayRectifySpan;
   }

   if (id == MEAS_DEWEIGHT_THRESHOLD)
   {
      if (value >= 0.0)
         deweightThreshold = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return deweightThreshold;
   }

   if (id == MEAS_DEWEIGHT_COEFF)
   {
      if (value >= 0.0)
         deweightCoeff = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return deweightCoeff;
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
Real SeqEstimator::GetRealParameter(const std::string &label) const
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
Real SeqEstimator::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string SeqEstimator::GetStringParameter(const Integer id) const
{
   if (id == INPUT_DATA_FILE)
      return inputDataFile;

   if (id == RESTART_EPOCH_FORMAT)
      return restartEpochFormat;

   if (id == RESTART_EPOCH)
      return restartEpoch;

   if (id == OUTPUT_DATA_FILE)
      return outputDataFile;

   return Estimator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool SeqEstimator::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == INPUT_DATA_FILE)
   {
      // verify a valid file name
      if (value != "")
      {
         Integer error;
         if (!GmatStringUtil::IsValidFullFileName(value, error))
            throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".InputWarmStartFile is an invalid file name.\n");
      }

      inputDataFile = value;
      return true;
   }

   if (id == RESTART_EPOCH_FORMAT)
   {
      // verify is a valid epoch format
      if (!theTimeConverter->IsValidTimeSystem(value))
         throw EstimatorException("Error: Time system '" + value + "' set to " + GetName() + ".WarmStartEpochFormat parameter is invalid.\n");

      restartEpochFormat = value;
      return true;
   }

   if (id == RESTART_EPOCH)
   {
      // verify is a valid epoch or is "FirstMeasurement" or "LastWarmStartRecord"
      if (value != "FirstMeasurement" && value != "LastWarmStartRecord")
      {
         try
         {
            (!theTimeConverter->ValidateTimeFormat(restartEpochFormat, value));
         }
         catch (BaseException &ex)
         {
            throw EstimatorException("Error: Value '" + value + "' set to " + GetName() + ".WarmStartEpoch parameter is invalid."
               " This field must be 'FirstMeasurement', 'LastWarmStartRecord' or a valid epoch in the format set by " + GetName() + ".WarmStartEpochFormat.\n");
         }
      }

      restartEpoch = value;
      return true;
   }

   if (id == OUTPUT_DATA_FILE)
   {
      // verify a valid file name
      if (value != "")
      {
         Integer error;
         if (!GmatStringUtil::IsValidFullFileName(value, error))
            throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".OutputWarmStartFile is an invalid file name.\n");
      }

      outputDataFile = value;
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
std::string SeqEstimator::GetStringParameter(const std::string &label) const
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
bool SeqEstimator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}
// End EKF mod

//------------------------------------------------------------------------------
// Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Implements the basic sequential estimation state machine
 *
 * This method changes the SeqEstimator's finite state based on the
 * status of the elements involved in the estimation process by calling the
 * state machine to process one step in its implementation
 *
 * @return The state at the end of the state machine step.
 */
//------------------------------------------------------------------------------
Solver::SolverState SeqEstimator::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the INITIALIZING state\n");
         #endif
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the PROPAGATING state\n");
         #endif
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the CALCULATING state\n");
         #endif
         CalculateData();
         break;

      case LOCATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the LOCATING state\n");
         #endif
         ProcessEvent();
         break;

      case ESTIMATING:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the ESTIMATING state\n");
         #endif
         Estimate();
         break;

      case CHECKINGRUN:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the CHECKINGRUN state\n");
         #endif
         CheckCompletion();

         // Add .mat data
         if (matWriter != NULL)
            if (!AddMatData(matData))
               throw EstimatorException("Error adding .mat data file");
         break;

      case FINISHED:
         #ifdef WALK_STATE_MACHINE
            MessageInterface::ShowMessage("SeqEstimator::Executing the FINISHED state\n");
         #endif
         RunComplete();
         break;

      default:
         throw EstimatorException("Unknown state encountered in the " +
               instanceName + " sequential estimator.");

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
void SeqEstimator::StateCleanUp()
{
   if (currentState == CHECKINGRUN)
   {
      if ((hasAnchorEpoch || predictTimeSpan != 0.0) && !isPredicting)
      {
         // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
         currentSolveForState = esm.GetEstimationStateForReport();
         currentSolveForStateC = esm.GetEstimationCartesianStateForReport();
         currentSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

         // Get Covariance
         informationInverse = (*(stateCovariance->GetCovariance()));
         // TODO: for small covariances, this thinks it's singular
         //information = informationInverse.Inverse(COV_INV_TOL);

         /// Calculate conversion derivative matrixes
         // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
         cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
         cart2SolvMatrixPrev = cart2SolvMatrix;
         // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
         solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
         solv2KeplMatrixPrev = solv2KeplMatrix;

         WriteToTextFile(FINISHED);
         AddMatlabConfigData();
         ReportProgress(FINISHED);

         // Buffer the objects in the esm in case the smoother needs them
         esm.BufferObjects(&esmObjBuffer);

         isPredicting = true;

         if (!hasAnchorEpoch)
         {
            BeginPredicting(predictTimeSpan);
         }

         // Update the process noise if needed
         if (processNoiseStep == 0.0 ||
            GmatMathUtil::IsEqual(currentEpochGT, nextNoiseUpdateGT, ESTTIME_ROUNDOFF))
         {
            nextNoiseUpdateGT = currentEpochGT;
            if (finalEpochGT > currentEpochGT)
               nextNoiseUpdateGT.AddSeconds(processNoiseStep);
            else
               nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);
         }

         currentState = PROPAGATING;

         FindTimeStep();
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
bool SeqEstimator::TakeAction(const std::string & action,
      const std::string & actionData)
{
   if (action == "RunBackwards")
   {
      currentState = INITIALIZING;

	  isSmoothing = true;
      measManager.SetDirection(false);
      measManager.Reset();

      return true;
   }
   if (action == "RunForwards")
   {
      currentState = INITIALIZING;
	  isSmoothing = false;
      measManager.SetDirection(true);
      measManager.Reset();

      return true;
   }
   if (action == "UseProvidedFlags")
   {
      overwriteEditFlags = false;
      return true;
   }

   return Estimator::TakeAction(action, actionData);
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
bool SeqEstimator::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SeqEstimator::Initialize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Initialize())
   {
      retval = true;

	  isSmoothing = false;
	  forwardFilterInfo = nullptr;

      if (matFileName != "")
      {
         if (matWriter != NULL)
         {
            matFilterData.SetInitialRealValue(NAN);
            matFilterData.Clear();
         }
      }
   }

   return retval;
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
bool SeqEstimator::Finalize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SeqEstimator::Finalize() "
            "entered\n");
   #endif
   bool retval = false;

   if (Estimator::Finalize())
   {
      retval = true;
      measManager.Finalize();  // EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() measManager.Finalize() finished\n");
#endif
      esm.MapVectorToObjects();  // EKF mod 12/16
      esm.MapCovariancesToObjects();
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() esm.MapVectorToObjects() finished\n");
#endif
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SeqEstimator::Finalize() "
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
bool SeqEstimator::IsFinalPass()
{
   bool retval = true;
   return retval;
}


//------------------------------------------------------------------------------
// std::vector<SeqEstimator::UpdateInfoType> GetUpdateStats()
//------------------------------------------------------------------------------
/**
 * This returns the vector of UpdateInfoType
 *
 * @return the vector of UpdateInfoType
 */
 //------------------------------------------------------------------------------
std::vector<SeqEstimator::UpdateInfoType> SeqEstimator::GetUpdateStats()
{
   return updateStats;
}


//------------------------------------------------------------------------------
// void GetObjectBuffer(ObjectArray &buffer)
//------------------------------------------------------------------------------
/**
 * This gets the buffer of objects from the the EstimationStateManager at the
 * final epoch
 *
 * @param buffer The buffered objects
 */
 //------------------------------------------------------------------------------
void SeqEstimator::GetObjectBuffer(ObjectArray &buffer)
{
   buffer = esmObjBuffer;
}


//------------------------------------------------------------------------------
// void SetAnchorEpoch(const GmatTime& epoch, bool noiseBetween)
//------------------------------------------------------------------------------
/**
 * Set an anchor epoch to use as a reference for various SeqEstimator epochs.
 * This includes the epochs where process noise is applied, and the final epoch
 * to propagate to after the last measurement
 *
 * @param epoch The epoch to use as an anchor
 * @param noiseBetween True if the nextNoiseUpdate should be placed between the
 *                     anchor and current epoch, false if it should be outside
 */
 //------------------------------------------------------------------------------
void SeqEstimator::SetAnchorEpoch(const GmatTime& epoch, bool noiseBetween)
{
   hasAnchorEpoch = true;

   // Set final epoch
   finalEpochGT = epoch;

   // Set next noise epoch to be based off of multiples of the noise interval
   // instead of the initial epoch
   nextNoiseUpdateGT = epoch;
   prevUpdateEpochGT = currentEpochGT;
   GmatTime epochDiff = currentEpochGT - nextNoiseUpdateGT;
   Real quotient = epochDiff.GetTimeInSec() / processNoiseStep;
   Integer numberIntervals = Integer(quotient);
   if (!noiseBetween && GmatMathUtil::Abs(numberIntervals) <= GmatMathUtil::Abs(quotient))
      numberIntervals += GmatMathUtil::SignOf(numberIntervals);

   nextNoiseUpdateGT.AddSeconds(numberIntervals*processNoiseStep);
}


//------------------------------------------------------------------------------
// void SetCovariance(const Rmatrix& cov)
//------------------------------------------------------------------------------
/**
 * Set the covariance for the filter
 *
 * @param cov The covariance to set
 */
 //------------------------------------------------------------------------------
void SeqEstimator::SetCovariance(const Rmatrix& cov)
{
   *(esm.GetCovariance()->GetCovariance()) = cov;
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
bool SeqEstimator::UpdateInitialConditions()
{
   bool retval = false;

   // Warm restart
   if (inputDataFile != "")
   {
      GmatTime restartEpochGT, temp;
      bool onlyPriorEpochs;

      if (restartEpoch == "FirstMeasurement")
      {
         restartEpochGT = measManager.GetEpochGT();
         onlyPriorEpochs = true;
      }
      else if (restartEpoch == "LastWarmStartRecord")
      {
         restartEpochGT = -1;
         onlyPriorEpochs = false;
      }
      else
      {
         restartEpochGT = ConvertToGmatTimeEpoch(restartEpoch, restartEpochFormat);
         onlyPriorEpochs = false;
      }

      retval = RestartFromDataFile(restartEpochGT, onlyPriorEpochs);
   }

   retval = SetNominalValues() || retval;

   return retval;
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Performs final setup prior to use of the estimator
 *
 * This method is called in the INITIALIZING state
 */
//------------------------------------------------------------------------------
void SeqEstimator::CompleteInitialization()
{
// EKF mod 12/16
   #ifdef WALK_STATE_MACHINE
      MessageInterface::ShowMessage("SeqEstimator state is CompleteInitialization\n");
   #endif

   ObjectArray satArray;
   esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
// End EKF mod

   estimationState              = esm.GetState();
   stateSize = estimationState->GetSize();
// EKF mod 12/16

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("stateSize = %d ", stateSize  );
   MessageInterface::ShowMessage("measSize = %d ", measSize  );
   MessageInterface::ShowMessage("GmatState got from propagator: size = %d   epoch = %.12lf   [", estimationState->GetSize(), estimationState->GetEpoch());
   for (UnsignedInt k=0; k < stateSize; ++k)
      MessageInterface::ShowMessage("%.12lf,  ", (*estimationState)[k]);
   MessageInterface::ShowMessage("]\n");
#endif
// End EKF mod

   Estimator::CompleteInitialization();

// EKF mod 12/16
   // Set the current epoch based on the first spacecraft in the ESM
   if(satArray.size() == 0)
   {
      throw EstimatorException("Cannot initialize the estimator: there are "
                               "no Spacecraft in the estimation state manager");
   }

   estimationEpochGT = ((Spacecraft*)satArray[0])->GetEpochGT();
   currentEpochGT = ((Spacecraft*)satArray[0])->GetEpochGT();

   const std::vector<ListItem*>* stateMap = esm.GetStateMap();
   for (UnsignedInt ii = 0; ii < stateMap->size(); ii++)
   {
      if ((*stateMap)[ii]->elementName == "KeplerianState")
      {
         throw EstimatorException("Solving for Keplerian states is currently disallowed in the filter");
      }

      if ((*stateMap)[ii]->elementName == "PassBiases")
      {
         throw EstimatorException("Solving for PassBiases is currently disallowed in the filter");
      }
   }

   ObjectArray objects;                                                                     
   esm.GetStateObjects(objects);                                                            
   std::vector<TrackingDataAdapter*> adapters = measManager.GetAllTrackingDataAdapters();   
   for (UnsignedInt i = 0; i < adapters.size(); ++i)
   {
      adapters[i]->SetUsedForObjects(objects);
      std::string modelType = adapters[i]->GetModelType();
      if (modelType == "SN_Doppler_Rtn" || modelType == "SN_Doppler" || modelType == "SN_DOWD" || modelType == "SN_Range" )
      {
         if(!(GmatGlobal::Instance()->InTestingMode()))
            throw EstimatorException("ERROR: SN measurement model types are currently disallowed for use in an EKFS. \n     Measurement model type found: \"" + modelType + "\" in EKFS: \"" + GetName() + "\"\n" );
      }
   }
// End EKF mod

   // Now load up the observations
   measManager.PrepareForProcessing(false);
//   measManager.PrepareForProcessing(false); // added false, EKF mod 12/16

///// Make more generic?
   measManager.LoadRampTables();

   // Create RIC frame
   PropSetup  *ps  = GetPropagator("");
   ODEModel   *ode = ps->GetODEModel();
   std::string cb = ode->GetStringParameter("CentralBody");
   SpacePoint *body = solarSystem->GetBody(cb);

   for (UnsignedInt i = 0; i < satArray.size(); ++i)
   {
      if (vnbFrames.find(satArray[i]) != vnbFrames.end())
         if (vnbFrames[satArray[i]])
            delete vnbFrames[satArray[i]];

      vnbFrames[satArray[i]] = CoordinateSystem::CreateLocalCoordinateSystem("VNB", "VNB", body, body,
         (SpacePoint*)satArray[i], body->GetJ2000Body(), solarSystem);
   }

   //Initialize Process Noise Models
   processNoiseModels.clear();
   UnsignedInt processNoiseId = GmatType::GetTypeId("ProcessNoiseModel");
   RealArray noiseStepFromModel;
   for (ObjectArray::iterator sat = satArray.begin(); sat != satArray.end(); ++sat)
   {
      ProcessNoiseModel *noiseModel =
         (ProcessNoiseModel*) (*sat)->GetRefObject(processNoiseId, (*sat)->GetRefObjectName(processNoiseId));

      if (noiseModel)
      {
         processNoiseModels[(*sat)] = noiseModel;
         noiseStepFromModel.push_back(noiseModel->GetRealParameter("UpdateTimeStep"));
      }

   }

// EKF mod 12/16
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("GmatState got from propagator: size = %d [", stateSize);
   for (UnsignedInt zz=0; zz < stateSize; ++zz)
   MessageInterface::ShowMessage("%lf,  ", (*estimationState)[zz]);
   MessageInterface::ShowMessage("]\n");

   //MessageInterface::ShowMessage("currentEpoch = %.15lf  ", currentEpoch);
   MessageInterface::ShowMessage("estimationEpochGT = %s\n", estimationEpochGT.ToString().c_str());
#endif
// End EKF mod

   if (measManager.IsForward())
   {
      UnsignedInt numObsRemoved = TrimObsByEpoch(estimationEpochGT, inputDataFile != "");

      if (numObsRemoved > 0U)
      {
         MessageInterface::ShowMessage("Total number of measurements removed because "
            "they are prior to the warm start or initialization epoch: %d\n", numObsRemoved);
         MessageInterface::ShowMessage("Total number of remaining records: %d\n", measManager.GetObservationDataList()->size());

         if (measManager.GetMeasurementSize() == 0U)
            throw EstimatorException("No measurements remain after removing measurements "
               "prior to the warm start or initialization epoch.");
      }
   }

   nextMeasurementEpochGT = measManager.GetEpochGT();

   if (abs((nextMeasurementEpochGT - estimationEpochGT).GetDays()) > 30)
      MessageInterface::ShowMessage("WARNING: Initial Epoch is %d days away from the first estimation epoch, propagation may take longer than expected.\n", abs((nextMeasurementEpochGT - estimationEpochGT).GetDays()));

   nextNoiseUpdateGT = currentEpochGT;
   for (Integer i = 0; i < noiseStepFromModel.size(); i++)
   {
      if (noiseStepFromModel[i] != 0)
      {
         if (processNoiseStep != 0)
         {
            MessageInterface::ShowMessage("WARNING: When both the parameters of ProcessNoiseModel.UpdateTimeStep and the ExtendedKalmanFilter.ProcessNoiseTimeStep have a non-zero value, the ProcessNoiseTimeStep Parameter will be used.\n", processNoiseStep);
            noiseStepFromModel.clear();
         }
         else if (noiseStepFromModel[i] != noiseStepFromModel[0])
         {
            MessageInterface::ShowMessage("WARNING: All ProcessNoiseModel UpdateTimeStep parameters must be equal to be use with the ExtendedKalmanFilter. Using EKF.ProcessNoiseTimeStep parameter value %f.\n", processNoiseStep);
            noiseStepFromModel.clear();
         }
         else 
         {
            processNoiseStep = noiseStepFromModel[i];
         }
      }
   }
   if (measManager.IsForward())
      nextNoiseUpdateGT.AddSeconds(processNoiseStep);
   else
      nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);

   if (delayRectifySpan != 0.0)
      esm.SetHasStateOffset(true);

   pBar.SetSize(stateSize, stateSize);
   Q.SetSize(stateSize, stateSize);

   residuals.SetSize(stateSize);
   x0bar.SetSize(stateSize);
   dx.SetSize(stateSize);

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
   cart2SolvMatrixPrev = cart2SolvMatrix;
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
   solv2KeplMatrixPrev = solv2KeplMatrix;

   measStats.clear();
   updateStats.clear();
   isInitialized = true;

// EKF mod 12/16
   // Get list of signal paths and specify the length of participants' column
   pcolumnLen = 26;

   std::vector<StringArray> signalPaths = measManager.GetSignalPathList();
   for(UnsignedInt i = 0; i < signalPaths.size(); ++i)
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

   pcolumnCovLen = (Integer)GmatMathUtil::Max(pcolumnLen, 29);

   WriteToTextFile();
// End EKF mod
   ReportProgress();

   if (delayRectifySpan != 0.0)
   {
      bool handleLeapSecond;
      GmatTime utcMjdEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
         GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
      std::string utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);

      MessageInterface::ShowMessage("Entering DelayRectifyTimeSpan at %s UTCG\n", utcEpoch.c_str());
   }

   if (GmatMathUtil::IsEqual(currentEpochGT, nextMeasurementEpochGT))
   {
      currentState = CALCULATING;
   }
   else
   {
      Real timeStepMeas = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();
      Real timeStepNoise = (nextNoiseUpdateGT - currentEpochGT).GetTimeInSec();

      if (processNoiseStep != 0.0 && GmatMathUtil::Abs(timeStepNoise) < GmatMathUtil::Abs(timeStepMeas))
         timeStep = timeStepNoise;
      else
         timeStep = timeStepMeas;


      PrepareForStep();
      currentState = PROPAGATING;
   }

   if (showAllResiduals)
   {
      StringArray plotMeasurements;
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
      {
         plotMeasurements.clear();
         plotMeasurements.push_back(modelNames[i]);
         std::string plotName = instanceName + "_" + modelNames[i] +
               "_Residuals";
         BuildResidualPlot(plotName, plotMeasurements);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "CompleteInitialization complete!\n   STM = %s\n   Covariance = %s\n",
            stm->ToString().c_str(), covariance.GetCovariance()->ToString().c_str());
   #endif

      // Get data for the covariance report
      UpdateInfoType updateStat;
      FillUpdateInfo(updateStat);
      updateStat.processNoise.SetSize(0, 0);
      updateStat.isObs = false;

      WriteDataFile();
      AddMatlabFilterData(updateStat);
      updateStats.push_back(updateStat);
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
void SeqEstimator::FindTimeStep()
{
// if (GmatMathUtil::IsEqual(currentEpoch, nextMeasurementEpoch)) EKF mod 12/16 changed to next line
   if (!isPredicting && GmatMathUtil::IsEqual(currentEpochGT, nextMeasurementEpochGT, ESTTIME_ROUNDOFF))
   {
      // We're at the next measurement, so process it
      currentState = CALCULATING;
      FilterUpdate();
      timeStep = 0.0; // EKF mod 12/16 we are at the next measurement... so do not advance it further
   }
   else if (nextMeasurementEpochGT == 0.0 ||
           (isPredicting && GmatMathUtil::IsEqual(currentEpochGT, finalEpochGT, ESTTIME_ROUNDOFF)))
   {
      // Finished running through the data
      if (!GmatMathUtil::IsEqual(currentEpochGT, nextMeasurementEpochGT, ESTTIME_ROUNDOFF))
         FilterUpdate();
      currentState = CHECKINGRUN;
   }
   else
   {

      if (currentState == PROPAGATING)
      {
         // Update the process noise and store the filter info if needed
         if (processNoiseStep == 0.0 ||
            GmatMathUtil::IsEqual(currentEpochGT, nextNoiseUpdateGT, ESTTIME_ROUNDOFF))
         {

			 // Ensure back filter only uses valid forward filter epochs
			 if (isSmoothing)
			 {
				 if (!VerifySmoothTimeStep())
				 {
					 timeStep = MatchSmoothTimeStep();
					 return;
				 }
			 }
            
			FilterUpdate();
            nextNoiseUpdateGT = currentEpochGT;
            if (measManager.IsForward())
               nextNoiseUpdateGT.AddSeconds(processNoiseStep);
            else
               nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);
         }
      }

      // Calculate the time step in seconds and stay in the PROPAGATING state;
      // timeStep could be positive or negative
      if (isPredicting)
         timeStep = (finalEpochGT - currentEpochGT).GetTimeInSec();
      else
         timeStep = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();

      Real timeStepNoise = (nextNoiseUpdateGT - currentEpochGT).GetTimeInSec();

      if (processNoiseStep != 0.0 && GmatMathUtil::Abs(timeStepNoise) < GmatMathUtil::Abs(timeStep))
         timeStep = timeStepNoise;

      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("SeqEstimator::FindTimeStep   timestep = %.12lf; nextepoch = "
               "%s; current = %s\n", timeStep,
               GmatMathUtil::Abs(timeStepNoise) > GmatMathUtil::Abs(timeStep) ?
               nextMeasurementEpochGT.ToString().c_str() : nextNoiseUpdateGT.ToString().c_str(),
               currentEpochGT.ToString().c_str());
      #endif
   }
}

//------------------------------------------------------------------------------
// void UpdateProcessNoise()
//------------------------------------------------------------------------------
/**
 * This updates the process noise matrix, Q
 *
 * Author: Jamie LaPointe
 * Org:    University of Arizona - Department of Aerospace and Mechanical Engineering
 * Date:   9 May 2016
 */
//------------------------------------------------------------------------------
void SeqEstimator::UpdateProcessNoise()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing process noise update\n");
   #endif

   // Zero out last process noise
   Q = Rmatrix(stateSize, stateSize);

   Real dt = (currentEpochGT - prevUpdateEpochGT).GetTimeInSec();
   dt = GmatMathUtil::Abs(dt);

   // Process noise is prone to numerical issues leading to loss of
   // positive-definiteness at very small time steps
   Real pnTimeTolerance = 1e-6;
   if (dt >= pnTimeTolerance)
   {
      const std::vector<ListItem*> *map = esm.GetStateMap();
      ProcessNoiseModel *noiseModel;
      for (auto iter = processNoiseModels.begin(); iter != processNoiseModels.end(); iter++)
      {
         Rmatrix66 scNoise(false);
         const GmatBase *refBody = iter->first;
         noiseModel = iter->second;
         if (noiseModel)
            scNoise = noiseModel->GetProcessNoise(dt, currentEpochGT);

         if (refBody)
         {
            Integer idx;

            // Find state index of ref body
            for (idx = 0; idx < stateSize; idx++)
            {
               if ((*map)[idx]->objectName == refBody->GetName() &&
                     (((*map)[idx]->elementName == "CartesianState") ||
                     ((*map)[idx]->elementName == "KeplerianState")))
                  break;
            }

            // If ref body not found, skip this noise
            if (idx == stateSize)
               continue;

            for (UnsignedInt ii = 0U; ii < 6U; ii++)
            {
               for (UnsignedInt jj = 0U; jj < 6U; jj++)
               {
                  Q(idx + ii, idx + jj) += scNoise(ii, jj);
               }
            }
         }
      }

      AddEstimatedParameterNoise(Q, dt);

      prevUpdateEpochGT = currentEpochGT;
   }
}

//------------------------------------------------------------------------------
// void AddEstimatedParameterNoise(Rmatrix &noise, Real dt)
//------------------------------------------------------------------------------
/**
 * This adds process noise from the EstimatedParameter objects
 *
 * @param noise The process noise matrix to add the noise from the
 *              EstimatedParameters
 * @param dt The elapsed time to evaluate the process noise over.
 */
 //------------------------------------------------------------------------------
void SeqEstimator::AddEstimatedParameterNoise(Rmatrix &noise, Real dt)
{
   ObjectArray satArray;
   esm.GetStateObjects(satArray, Gmat::SPACECRAFT);

   if (needsSatPropMap || satPropMap.size() == 0)
         BuildSatPropMap();


   for (UnsignedInt satIdx = 0U; satIdx < satArray.size(); satIdx++)
   {
      Spacecraft *sat = (Spacecraft*)satArray[satIdx];
      ObjectArray estParams = sat->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));

      for (ObjectArray::iterator paramObj = estParams.begin(); paramObj < estParams.end(); ++paramObj)
      {
         std::string paramObjName = (*paramObj)->GetName();
         Integer paramId = sat->GetEstimationParameterID(paramObjName);
         std::string solveFor = sat->GetParameterNameForEstimationParameter(paramObjName);
         Gmat::StateElementId solveForID = static_cast<Gmat::StateElementId>(sat->GetPropItemID(solveFor));

         EstimatedParameter *estParam = (EstimatedParameter *)(*paramObj);

         // Find the esm index of the EstimatedParameter
         bool indexFound = false;
         UnsignedInt esmIndex = 0U;
         for (UnsignedInt ii = 0U; ii < esm.GetStateSize(); ii++)
         {
            if ((*esm.GetStateMap())[ii]->elementID == paramId && (*esm.GetStateMap())[ii]->objectName == sat->GetName())
            {
               indexFound = true;
               esmIndex = ii;
               break;
            }
         }

         if (!indexFound)
            throw EstimatorException("Error when adding process noise: "
               "Unable to find corresponding estimation state element for "
               "Estimated Parameter \"" + paramObjName + "\"");

         // Needed for conversion from actual force to nominal (epsilon = 0) force
         Real stateValue = (*esm.GetState())[esmIndex];
         Real accelVector[3] = { 0,0,0 };


         if (satPropMap[sat->GetName()])
         {
            ODEModel *ode = satPropMap[sat->GetName()]->GetODEModel();
            std::string prop = satPropMap[sat->GetName()]->GetName();
            

            for (UnsignedInt ii = 0U; ii < ode->GetNumForces(); ii++)
            {
               PhysicalModel *pm = ode->GetForce(ii);
               if (pm->SupportsDerivative(solveForID))
               {
                  accelVector[0] += forceDerivativeCache[prop + ode->GetName()][ii][3 + 6 * satIdx] / (1 + stateValue);
                  accelVector[1] += forceDerivativeCache[prop + ode->GetName()][ii][4 + 6 * satIdx] / (1 + stateValue);
                  accelVector[2] += forceDerivativeCache[prop + ode->GetName()][ii][5 + 6 * satIdx] / (1 + stateValue);
               }
            }
         }

         estParam->SetConversionFactor(GetEpsilonConversion(esmIndex));

         Rmatrix epmNoise = estParam->GetProcessNoise(dt, accelVector);

         // Add to noise matrix
         for (UnsignedInt ii = 0U; ii < 6U; ii++)
         {
            for (UnsignedInt jj = 0U; jj < 6U; jj++)
            {
               noise(ii, jj) += epmNoise(ii, jj);
            }

            // Add last column/row to correct column/row of noise matrix
            noise(ii, esmIndex) += epmNoise(ii, 6);
            noise(esmIndex, ii) += epmNoise(6, ii);
         }

         // Add final diagonal element
         noise(esmIndex, esmIndex) += epmNoise(6, 6);
      }
   }
}

//------------------------------------------------------------------------------
// void TimeUpdate()
//------------------------------------------------------------------------------
/**
 * Performs the time update of the state error covariance
 *
 * This method applies equation 4.7.1(b), and then symmetrizes the resulting
 * time updated covariance, pBar.
 */
//------------------------------------------------------------------------------
void SeqEstimator::TimeUpdate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing time update\n");
   #endif

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();

   // phi * P * phi^T + Q

   Rmatrix dX_dS = cart2SolvMatrixPrev;
   Rmatrix dS_dX = cart2SolvMatrix.Inverse();

   Rmatrix Q_S = dS_dX * Q * dS_dX.Transpose();
   Rmatrix stm_S = dS_dX * (*stm) * dX_dS;

   // Update offset from reference trajectory
   if (esm.HasStateOffset())
   {
      GmatState *offsetState = esm.GetStateOffset();
      Rvector xOffset(stateSize);
      xOffset.Set(offsetState->GetState(), stateSize);

      xOffset = (*stm) * xOffset;

      for (UnsignedInt i = 0; i < stateSize; ++i)
         (*offsetState)[i] = xOffset[i];
   }

   pBar = stm_S * (*(stateCovariance->GetCovariance())) * stm_S.Transpose() + Q_S;

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Q = \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", Q(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // make it symmetric!
   Symmetrize(pBar);
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
void SeqEstimator::CalculateData()
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


//------------------------------------------------------------------------------
// void ProcessEvent()
//------------------------------------------------------------------------------
/**
 * Performs the estimator side actions needed for event location
 *
 * This method is called in the LOCATING state
 */
//------------------------------------------------------------------------------
void SeqEstimator::ProcessEvent()
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

   // Check each active event.  If all located, move into the ESTIMATING state
   if (!locatingEvent)
   {
      currentState = ESTIMATING;
   }
}


//------------------------------------------------------------------------------
// void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Performs final checks after processing the observations
 *
 * This method is called in the CHECKINGRUN state
 *
 * @note Sequential estimators might not need this method or the CHECKINGRUN
 * state
 */
//------------------------------------------------------------------------------
void SeqEstimator::CheckCompletion()
{
   // Convert current estimation state from GMAT internal coordinate system to participants' coordinate system
   currentSolveForState = esm.GetEstimationStateForReport();
   currentSolveForStateC = esm.GetEstimationCartesianStateForReport();
   currentSolveForStateK = esm.GetEstimationKeplerianStateForReport("MA");

   // Get Covariance
   informationInverse = (*(stateCovariance->GetCovariance()));
   // TODO: for small covariances, this thinks it's singular
   //information = informationInverse.Inverse(COV_INV_TOL);

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   cart2SolvMatrixPrev = cart2SolvMatrix;
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
   solv2KeplMatrixPrev = solv2KeplMatrix;

   ++iterationsTaken;
   currentState = FINISHED;
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
void SeqEstimator::RunComplete()
{
   if (!isPredicting)
   {
      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SeqEstimator::entering WriteToTextFile()\n");
      #endif
      WriteToTextFile();

      AddMatlabConfigData();

      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SeqEstimator::Entering ReportProgress()\n");
      #endif
      ReportProgress();

      // Buffer the objects in the esm in case the smoother needs them
      esm.BufferObjects(&esmObjBuffer);

      #ifdef DEBUG_RUNCOMPLETE
         MessageInterface::ShowMessage("SeqEstimator::Completed ReportProgress()\n");
      #endif
   }

   measManager.ProcessingComplete();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed measManager.ProcessingComplete()\n");
   #endif
   esm.MapVectorToObjects();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed esm.MapVectorToObjects()\n");
   #endif
   // Clean up memory
   for (UnsignedInt i = 0; i < hTilde.size(); ++i)
   {
      hTilde[i].clear();
   }
   hTilde.clear();
   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::cleared hTilde\n");
   #endif

   measStats.clear();

   if (writeMatFile && (matWriter != NULL))
   {
      if (!WriteMatData())
         throw EstimatorException("Error writing .mat data file");

      matWriter->CloseFile();
   }

   if (dataFile.is_open())
      dataFile.close();

   #ifdef DEBUG_RUNCOMPLETE
      MessageInterface::ShowMessage("SeqEstimator::completed RunComplete()\n");
   #endif
}


//------------------------------------------------------------------------------
// void PrepareForStep()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to sequential estimators prior to a
 * propagation step.  That includes resetting the STM to the identity.
 */
//------------------------------------------------------------------------------
void SeqEstimator::PrepareForStep()
{
   ResetSTM();
   esm.MapSTMToObjects();

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrixPrev = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrixPrev = esm.SolveForStateToKeplConversionDerivativeMatrix();
}


//------------------------------------------------------------------------------
// void FilterUpdate()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to sequential estimators to update the
 * filter information and add it to the updateStats vector
 */
 //------------------------------------------------------------------------------
void SeqEstimator::FilterUpdate()
{
   // Update the STM
   esm.MapObjectsToSTM();
   esm.MapObjectsToVector();

   UpdateProcessNoise();
   TimeUpdate();
   (*(stateCovariance->GetCovariance())) = pBar;
   esm.MapCovariancesToObjects();

   if (currentState != CALCULATING)
   {
      // Get data for the covariance report
      UpdateInfoType updateStat;
      FillUpdateInfo(updateStat);
      updateStat.isObs = false;

      // Don't want to add data to vector if predicting to an epoch that's not an anchor epoch
      if (!(isPredicting && !hasAnchorEpoch))
      {
         WriteDataFile();
         AddMatlabFilterData(updateStat);
         updateStats.push_back(updateStat);
      }
      else if (addPredictToMatlab)
      {
         AddMatlabFilterData(updateStat);
      }

      // Reset the STM
      PrepareForStep();
      esm.MapObjectsToVector();
      PropagationStateManager *psm = propagators[0]->GetPropStateManager();
      psm->MapObjectsToVector();
      resetState = true;
   }
}

//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string SeqEstimator::GetProgressString()
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
            progress << "\nFinal Covariance Matrix:\n\n";
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
            progress << "Final Correlation Matrix:\n\n";
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
               "Solver state not supported for the sequential estimator");
      }
   }
   else
      return Estimator::GetProgressString();

   return progress.str();
}

//-------------------------------------------------------------------------
// void GetEstimationState(GmatState& outputState)
//-------------------------------------------------------------------------
/**
 * This Method used to convert result of estimation state to participants'
 * coordinate system
 *
 * @param outputState estimation state in participants' coordinate systems
 *
*/
//-------------------------------------------------------------------------
void SeqEstimator::GetEstimationState(GmatState& outputState)
{
    const std::vector<ListItem*> *map = esm.GetStateMap();


    Real outputStateElement;
    outputState.SetSize(map->size());

    for (UnsignedInt i = 0; i < map->size(); ++i)
    {
        ConvertToPartCoordSys((*map)[i], estimationEpochGT, (*estimationState)[i], &outputStateElement); 
        outputState[i] = outputStateElement;
    }
   #ifdef DEBUG_RESIDUALS
      // Dump the data to screen
      MessageInterface::ShowMessage("Estimator::GetEstimationState map->size = %d\n", map->size());
   #endif
}


//------------------------------------------------------------------------------
// bool ConvertToPartCoordSys(ListItem* infor, Real epoch,
//       Real inputStateElement, Real* outputStateElement)
//------------------------------------------------------------------------------
/**
 * Method used to convert result of a state's element in A1mjd to participant's
 * coordinate system
 *
 * @param infor                 information about state's element
 * @param epoch                 the epoch at which the state is converted it's
 *                              coordinate system
 * @param inputStateElement     state's element in GMAT internal coordinate system
 *                              (A1Mjd)
 * @param outputStateElement    state's element in participant's coordinate system
 *
*/
//------------------------------------------------------------------------------
//bool SeqEstimator::ConvertToPartCoordSys(ListItem* infor, Real epoch,
//      Real inputStateElement, Real* outputStateElement)
bool SeqEstimator::ConvertToPartCoordSys(ListItem* infor, GmatTime epoch,
      Real inputStateElement, Real* outputStateElement)
{

   (*outputStateElement) = inputStateElement;

   if (infor->object->IsOfType(Gmat::SPACEOBJECT))
   {
      if ((infor->elementName == "CartesianState")||(infor->elementName == "Position")||(infor->elementName == "Velocity"))
      {
         SpaceObject* obj = (SpaceObject*) (infor->object);
         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         CoordinateSystem* cs = (CoordinateSystem*) obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
         if (cs == NULL)
            throw EstimatorException("Coordinate system for "+obj->GetName()+" is not set\n");

         SpacePoint* sp = obj->GetJ2000Body();
         CoordinateSystem* gmatcs = CoordinateSystem::CreateLocalCoordinateSystem("bodyInertial",
            "MJ2000Eq", sp, NULL, NULL, sp, cs->GetSolarSystem());

         CoordinateConverter* cv = new CoordinateConverter();
         Rvector6 inState(0.0,0.0,0.0,0.0,0.0,0.0);
         Integer index;
         if ((infor->elementName == "CartesianState")||(infor->elementName == "Position"))
            index = infor->subelement-1;
         else if (infor->elementName == "Velocity")
            index = infor->subelement+2;
         else
            throw EstimatorException("Error in Estimator object: Parameter %s has not defined in GMAT\n");

         inState.SetElement(index, inputStateElement);
         Rvector6 outState;

         cv->Convert(epoch, inState, gmatcs, outState, cs); 

         (*outputStateElement) = outState[index];
         delete cv;
         delete gmatcs;
      }
   }

   return true;
}


//--------------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
//--------------------------------------------------------------------------------------------
/**
* This function is use to calculate derivative state conversion matrix for a spacecraft state.
* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
*
* @param obj      it is a spacecraft object
* @param state    Cartesian state of the spacecraft
*
* return          6x6 derivative state conversion matrix
*/
//--------------------------------------------------------------------------------------------
Rmatrix66 SeqEstimator::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
{
   // 1. Get mu value
   Spacecraft* spacecraft = (Spacecraft*)obj;
   CoordinateSystem* cs = (CoordinateSystem*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   CelestialBody* body = (CelestialBody*)(cs->GetOrigin());
   Real mu = body->GetRealParameter(body->GetParameterID("Mu"));

   // 2. Specify conversion matrix
   Rmatrix66 convMatrix = StateConversionUtil::CartesianToKeplerianDerivativeConversion(mu, state);

   return convMatrix;
}


//--------------------------------------------------------------------------------------------
// Rmatrix Estimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
//--------------------------------------------------------------------------------------------
/**
* This function is use to calculate derivative state conversion matrix for all solve-for variables.
* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
*
* @param stateMap a map of spacecrafts and their Cartisian state
*
* return          derivative state conversion matrix
*/
//--------------------------------------------------------------------------------------------
Rmatrix SeqEstimator::CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();

   // 1. Specify conversion sub matrixes for all spacecrafts
   std::map<GmatBase*, Rmatrix66> matrixMap;
   for (std::map<GmatBase*, Rvector6>::iterator i = stateMap.begin(); i != stateMap.end(); ++i)
      matrixMap[i->first] = CartesianToKeplerianCoverianceConvertionMatrix(i->first, i->second);

   // 2. Assemly whole conversion matrix
   Rmatrix conversionMatrix(map->size(),map->size());        // It is a zero matrix
   for(UnsignedInt i = 0; i < map->size(); )
   {
      if ((*map)[i]->elementName == "CartesianState")
      {
         // fill in conversion submatrix
         Rmatrix66 m = matrixMap[(*map)[i]->object];
         for(UnsignedInt row = 0; row < 6; ++row)
         {
            for(UnsignedInt col = 0; col < 6; ++col)
            {
               conversionMatrix.SetElement(i+row, i+col, m(row,col));
            }
         }
         // skip to next
         i = i + 6;
      }
      else
      {
         conversionMatrix(i,i) = 1.0;
         ++i;
      }
   }

   return conversionMatrix;
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
bool SeqEstimator::OverwriteEditFlag(const std::string &editFlag)
{
   return overwriteEditFlags;
}


//------------------------------------------------------------------------------
// void WriteEditFlag()
//------------------------------------------------------------------------------
/**
 * This method indicates if a new edit flag is to be written
 */
 //------------------------------------------------------------------------------
bool SeqEstimator::WriteEditFlag()
{
   return overwriteEditFlags;
}


//-------------------------------------------------------------------------
// bool EstimationPartials(std::vector<RealArray> &hMeas)
//-------------------------------------------------------------------------
/**
* Calculates the estimation partials for the current measurement in the
* solve-for states.
*/
//-------------------------------------------------------------------------
void SeqEstimator::EstimationPartials(std::vector<RealArray> &hMeas)
{
   Estimator::EstimationPartials(hMeas);

   // Now populate H
   for (UnsignedInt ii = 0; ii < hMeas.size(); ii++)
      for (UnsignedInt jj = 0; jj < hMeas[ii].size(); jj++)
         H(ii,jj) = hMeas[ii][jj];

}


//-------------------------------------------------------------------------
// bool DataFilter()
//-------------------------------------------------------------------------
/**
* This function is used to filter bad observation data records. It has
*   1. Data filter based on Sigma Editing
*/
//-------------------------------------------------------------------------
bool SeqEstimator::DataFilter()
{
   const ObservationData *currentObs =  measManager.GetObsData();                              // Get observation measurement data O
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);      // Get calculated measurement data C

   Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());
   Rmatrix Rbar = H * pBar * H.Transpose() + R;

   // Adjust computed and residual based on value of xOffset
   Rvector H_x(measSize);

   if (esm.HasStateOffset())
   {
      Rvector xOffset(stateSize);
      xOffset.Set(esm.GetEstimationStateOffset().GetState(), stateSize);
      H_x = H * xOffset;
   }

   for (Integer i=0; i < currentObs->value.size(); ++i)
   {

      // Filter based on scaled residual
      Real sigmaVal = GmatMathUtil::Sqrt(Rbar(i,i));
      if (GmatMathUtil::Abs(currentObs->value[i] - calculatedMeas->value[i] - H_x[i]) >= (constMult*sigmaVal))
      {
         measManager.GetObsDataObject()->inUsed = false;
         measManager.GetObsDataObject()->removedReason = "SIG";
         break;
      }
   }

   return measManager.GetObsDataObject()->inUsed;
}


// Placeholder implementation of pure virtual functions
void SeqEstimator::WriteReportFileHeaderPart6()
{ }


//----------------------------------------------------------------------
// void WriteNotationHeader()
//----------------------------------------------------------------------
/**
* Write notation header
*/
//----------------------------------------------------------------------
void SeqEstimator::WriteNotationHeader()
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
void SeqEstimator::WriteReportFileSummaryPart1(Solver::SolverState sState)
{
   // 1. Write summary part 1 header:
   textFile0 << "\n";
   textFile0 << CenterAndPadHeader(GetHeaderName() + " MEASUREMENT STATISTICS") << "\n";
   textFile0 << "\n";
   textFile0.flush();

   if (sState == FINISHED)
   {
      std::stringstream ss;

      ss << "     Filter run completed!     \n";
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
void SeqEstimator::WriteReportFileSummaryPart5(Solver::SolverState sState)
{
   textFile5 << "******************************************************************  FILTER COVARIANCE REPORT  ******************************************************************\n";
   textFile5 << "\n";

   for (UnsignedInt ii = 0U; ii < updateStats.size(); ii++)
   {
      if (GmatMathUtil::Mod(ii, 80) < 0.001)
         WriteCovariancePageHeader();
      BuildCovarianceLine(updateStats[ii]);
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
void SeqEstimator::WriteCovariancePageHeader()
{
   /// 4.1. Write page header
   textFile5 << "\n";

   if (IsIterative())
      textFile5 << "Iter ";

   if (textFileMode == "Normal")
   {
      textFile5 << "RecNum  UTCGregorian-Epoch        Obs-Type            " << GmatStringUtil::GetAlignmentString("Participants", pcolumnCovLen) << " Edit       Scaled Residual      Sigma-V (km)    Sigma-N (km)    Sigma-B (km)\n";
   }
   else
   {
      textFile5 << "RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " << GmatStringUtil::GetAlignmentString("Participants", pcolumnCovLen) << " Edit       Scaled Residual      Sigma-V (km)    Sigma-N (km)    Sigma-B (km)\n";
   }
   textFile5 << "\n";
}


//------------------------------------------------------------------------------
// void FillUpdateInfo(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * This method fills the UpdateInfoType struct with the relevant data,
 * except for the observation data.
 *
 * @param updateStat The struct to populate
 */
 //------------------------------------------------------------------------------
void SeqEstimator::FillUpdateInfo(UpdateInfoType &updateStat)
{
   updateStat.epoch = currentEpochGT;

   GmatState currentState = esm.GetEstimationMJ2000EqCartesianStateForReport();
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      updateStat.state.push_back(currentState[ii]);

   // Add state offset if not rectified
   if (esm.HasStateOffset())
   {
      GmatState xOffset = *esm.GetStateOffset();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
      {
         Real conv = GetEpsilonConversion(ii);
         updateStat.state[ii] += xOffset[ii] * conv;
      }
   }

   // fill sigmas
   updateStat.processNoise.SetSize(Q.GetNumRows(), Q.GetNumColumns());
   updateStat.processNoise = Q;
   CovarianceEpsilonConversion(updateStat.processNoise);

   Rmatrix cov = *(stateCovariance->GetCovariance());
   CovarianceEpsilonConversion(cov);
   updateStat.cov.SetSize(cov.GetNumRows(), cov.GetNumColumns());
   updateStat.cov = cov;
   Rmatrix covVNB = GetCovarianceVNB(cov);
   updateStat.covVNB.SetSize(covVNB.GetNumRows(), covVNB.GetNumColumns());
   updateStat.covVNB = covVNB;
   updateStat.stm.SetSize(stm->GetNumRows(), stm->GetNumColumns());
   updateStat.stm = *stm;
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      Real iiConv = GetEpsilonConversion(ii);
      for (UnsignedInt jj = 0; jj < stateSize; jj++)
      {
         Real jjConv = GetEpsilonConversion(jj);
         updateStat.stm(ii, jj) *= iiConv / jjConv;
      }
   }
}


//------------------------------------------------------------------------------
// void BuildCovarianceLine(const UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * This method builds the line for each measurement and time-update to the text file.
 *
 * @note The contents of the text file are TBD
 */
//------------------------------------------------------------------------------
void SeqEstimator::BuildCovarianceLine(const UpdateInfoType &updateStat)
{
   const ObservationData *currentObs;
   if (updateStat.isObs)
      currentObs = measManager.GetObsData(updateStat.measStat.recNum); // Get current observation data

   char s[1000];
   std::string timeString;
   std::string ss;
   Real temp;
   std::string linePrefix;
   std::string lineSuffix;

   // Write to report file iteration number, record number, and time:
   bool handleLeapSecond;
   theTimeConverter->Convert("A1ModJulian", updateStat.epoch.GetMjd(), "", "UTCGregorian", temp, timeString, 1, &handleLeapSecond);
   if (updateStat.isObs)
   {
      if (IsIterative())
         sprintf(&s[0], "%4d %6d  ", iterationsTaken, updateStat.measStat.recNum);
      else
         sprintf(&s[0], "%6d  ", updateStat.measStat.recNum);
   }
   else
   {
      sprintf(&s[0], "        ");
   }
   linePrefix = s;

   linePrefix += GmatStringUtil::GetAlignmentString(timeString.c_str(), 26, GmatStringUtil::LEFT);

   if (textFileMode != "Normal")
   {
      if (updateStat.isObs)
      {
         Real timeTAI = theTimeConverter->Convert(currentObs->epoch, currentObs->epochSystem, TimeSystemConverter::TAIMJD);
         sprintf(&s[0], "%.12lf ", timeTAI);
         linePrefix += s;
      }
      else
      {
         linePrefix += GmatStringUtil::GetAlignmentString("", 19);
      }
   }

   // Write to report file the measurement type

   // Write to report file measurement type name:
   if (updateStat.isObs)
      linePrefix += GmatStringUtil::GetAlignmentString(updateStat.measStat.type, 19) + " ";
   else
      linePrefix += GmatStringUtil::GetAlignmentString("", 19) + " ";

   // Write to report file measurement type unit:
   if (textFileMode != "Normal")
   {
      if (updateStat.isObs && currentObs)
         linePrefix += GmatStringUtil::GetAlignmentString(currentObs->unit, 6) + " ";
      else
         linePrefix += GmatStringUtil::GetAlignmentString("", 6) + " ";
   }

   // Write to report file all participants in signal path:
   ss = "";
   if (updateStat.isObs)
      for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
         ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
   linePrefix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(ss), pcolumnLen) + " ";


   // Write sigma values to suffix
   lineSuffix = "";
   for (UnsignedInt ii = 0; ii < 3; ii++)
   {
      Real val = updateStat.covVNB(ii, ii);
      if (val < 0.0)
         lineSuffix += GmatStringUtil::GetAlignmentString("N/A", 16, GmatStringUtil::RIGHT);
      else if ((1.0e-6 <= val) && (val < 1.0e16))
         lineSuffix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(val), false, false, true, 6)), 16, GmatStringUtil::RIGHT);
      else
         lineSuffix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(val), false, true, true, 6)), 16, GmatStringUtil::RIGHT);
   }


   std::string removedReason = updateStat.measStat.removedReason;

   if (updateStat.measStat.isCalculated && removedReason == "N")
   {
      if (textFileMode == "Normal")
         removedReason = "";
      else
         removedReason = "-";
   }

   if (updateStat.isObs)
   {
      for (Integer k = 0; k < updateStat.measStat.residual.size(); ++k)
      {
         textFile5 << linePrefix;   // write line prefix for each element. The GPS Point Solution has three elements.
         textFile5 << GmatStringUtil::GetAlignmentString(removedReason, 4, GmatStringUtil::LEFT) + " ";

         // Write C-value, and O-C
         if (!updateStat.measStat.isCalculated)
         {
            textFile5 << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << "  "; // Scaled Residual
         }
         else
         {
            sprintf(&s[0], "%21.6lf", updateStat.measStat.scaledResid[k]);
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
bool SeqEstimator::IsIterative()
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
std::string SeqEstimator::GetHeaderName()
{
   return "FILTER";
}


//------------------------------------------------------------------------------
// void OpenDataFile();
//------------------------------------------------------------------------------
/**
 * Method used to open the output data file
 */
 //------------------------------------------------------------------------------
void SeqEstimator::OpenDataFile()
{
   // Don't write file if file name is an empty string
   if (outputDataFile == "")
      return;

   std::string fnNoPath;
   std::string dataFileFullPath =
      GmatBase::GetFullPathFileName(fnNoPath, GetName(), outputDataFile, "OUTPUT_PATH",
         false, "data", false, true);

   if (dataFile.is_open())
      dataFile.close();

   dataFile.open(dataFileFullPath.c_str());

   if (!dataFile.is_open())
      throw EstimatorException("Error opening data file " +
         dataFileFullPath);

   dataFile.precision(16);

   // Write the data file header
   dataFile << "Epoch.UTCGregorian";

   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   // Write state names
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      dataFile << "," << DataFileStateHeader(ii);

   // Write covariance names
   std::string covHeaderName = DataFileCovHeader();
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      for (UnsignedInt jj = 0; jj <= ii; jj++)
         dataFile << "," << covHeaderName << "_" << ii+1 << "_" << jj+1;

   dataFile << "\n";
}


//------------------------------------------------------------------------------
// std::string DataFileStateHeader(Integer index)
//----------------------------------------------------------------------
/**
 * Write the name of the state type in header of the data file
 */
 //------------------------------------------------------------------------------
std::string SeqEstimator::DataFileStateHeader(Integer index)
{
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   std::string stateHeader;
   std::string elementName = (*stateMap)[index]->elementName;

   if ((elementName == "CartesianState") || (elementName == "KeplerianState"))
   {
      GmatBase* obj = (*stateMap)[index]->object;
      std::stringstream ss;
      ss << obj->GetStringParameter("Id") << ".";

      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      SpacePoint* origin = cs->GetOrigin();

      ss << origin->GetName() << "MJ2000Eq.";

      switch ((*stateMap)[index]->subelement)
      {
      case 1:
         ss << "X";
         break;
      case 2:
         ss << "Y";
         break;
      case 3:
         ss << "Z";
         break;
      case 4:
         ss << "VX";
         break;
      case 5:
         ss << "VY";
         break;
      case 6:
         ss << "VZ";
         break;
      }

      stateHeader = ss.str();
   }
   else
   {
      stateHeader = GetElementFullNameID((*stateMap)[index], false, "Cartesian");
   }

   return stateHeader;
}


//------------------------------------------------------------------------------
// std::string DataFileCovHeader() const
//----------------------------------------------------------------------
/**
 * Write the name of the covariance type in header of the data file
 */
 //------------------------------------------------------------------------------
std::string SeqEstimator::DataFileCovHeader() const
{
   return "Covariance";
}


//------------------------------------------------------------------------------
// bool WriteDataFile()
//------------------------------------------------------------------------------
/**
 * Method used to write to the output data file
 */
 //------------------------------------------------------------------------------
void SeqEstimator::WriteDataFile()
{
   if (outputDataFile == "")
      return;

   if (!dataFile.is_open())
      OpenDataFile();

   // Don't write line if in filter is performing delayed rectification
   if (esm.HasStateOffset())
      return;

   // Write epoch
   GmatTime temp;
   std::string timeString;
   bool handleLeapSecond;
   theTimeConverter->Convert("A1ModJulian", currentEpochGT, "", "UTCGregorian", temp, timeString, 1, &handleLeapSecond);

   dataFile << timeString;

   // Write state
   GmatState outState = esm.GetEstimationMJ2000EqCartesianStateForReport();

   // Add state offset if not rectified
   if (esm.HasStateOffset())
   {
      GmatState xOffset = *esm.GetStateOffset();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
      {
         Real conv = GetEpsilonConversion(ii);
         outState[ii] += xOffset[ii] * conv;
      }
   }
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      std::string value = GmatStringUtil::RealToString(outState[ii], false, true, true);
      dataFile << "," << value;
   }

   WriteCovarianceToDataFile();

   dataFile << "\n";
}


//------------------------------------------------------------------------------
// void WriteCovarianceToDataFile()
//----------------------------------------------------------------------
/**
 * Writes the covariance to the data file
 */
 //------------------------------------------------------------------------------
void SeqEstimator::WriteCovarianceToDataFile()
{
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   Rmatrix dX_dS = esm.CartToSolveForStateConversionDerivativeMatrix();

   Rmatrix cov = *(stateCovariance->GetCovariance());
   cov = dX_dS * cov * dX_dS.Transpose();

   // Write covariance lower triangle
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      Real iiConv = GetEpsilonConversion(ii);
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         Real jjConv = GetEpsilonConversion(jj);
         Real value = cov(ii, jj) * iiConv * jjConv;
         std::string valueStr = GmatStringUtil::RealToString(value, false, true, true);
         dataFile << "," << valueStr;
      }
   }
}


//------------------------------------------------------------------------------
// bool RestartFromDataFile(const GmatTime &epoch, bool onlyPriorEpochs);
//------------------------------------------------------------------------------
/**
 * Method used to open the output data file
 *
 * @param epoch The epoch to search for in the data file.
 * @param onlyPriorEpochs Flag if only epochs in the data file prior to the
                          input epoch are accepted.
 *
 * @return true if data was loaded from the file, false otherwise
 */
 //------------------------------------------------------------------------------
bool SeqEstimator::RestartFromDataFile(const GmatTime &epoch, bool onlyPriorEpochs)
{
   // Exit if file is an empty string
   if (inputDataFile == "")
      return false;

   std::string fnNoPath;
   std::string dataFileFullPath =
      GetFullPathFileName(fnNoPath, GetName(), inputDataFile, "OUTPUT_PATH",
         false, "data", false, true);

   std::ifstream dataInFile;

   dataInFile.open(dataFileFullPath.c_str());

   if (!dataInFile.is_open())
      throw EstimatorException("Error opening data file " +
         dataFileFullPath);

   std::string line;

   // Read the data file header
   GmatFileUtil::GetLine(&dataInFile, line);
   StringArray header = GmatStringUtil::SeparateByComma(line);

   UnsignedInt epochIndex;
   std::string epochFormat;
   bool epochFound = false;
   for (UnsignedInt ii = 0U; ii < header.size(); ii++)
   {
      StringArray headerData = GmatStringUtil::SeparateBy(header[ii], ".");

      if (GmatStringUtil::ToLower(headerData[0]) == "epoch")
      {
         if (headerData.size() >= 2)
         {
            epochFound = true;
            epochIndex = ii;
            epochFormat = headerData[1];

            break;
         }
      }
   }

   if (!epochFound)
   {
      dataInFile.close();
      throw EstimatorException("Error reading InputWarmStartFile, no epoch column found");
   }

   // Find the line containing the epoch closest to the input epoch, but not after the input epoch
   epochFound = false;
   StringArray restartData;
   GmatTime bestEpoch;

   while (!dataInFile.eof())
   {
      GmatFileUtil::GetLine(&dataInFile, line);

      if (GmatStringUtil::IsBlank(line))
         continue;

      StringArray data = GmatStringUtil::SeparateByComma(line);
      GmatTime lineEpoch = ConvertToGmatTimeEpoch(data[epochIndex], epochFormat);

      if (restartEpoch == "LastWarmStartRecord")
      {
         epochFound = true;
         bestEpoch = lineEpoch;
         restartData = data;

         continue;
      }

      // Compare epochs
      Real epochDiff = (epoch - lineEpoch).GetMjd();

      // If line epoch is before input epoch
      bool goodEpoch = epochDiff > ESTTIME_ROUNDOFF;

      // If line epoch is equal to input epoch (within tolerance)
      if (!onlyPriorEpochs)
         goodEpoch = goodEpoch || GmatMathUtil::Abs(epochDiff) < ESTTIME_ROUNDOFF;

      if (goodEpoch && (!epochFound || lineEpoch >= bestEpoch))
      {
         epochFound = true;
         bestEpoch = lineEpoch;
         restartData = data;
      }
   }

   // Done with the file now
   dataInFile.close();

   // Validation checks
   if (!epochFound)
      throw EstimatorException("Error reading InputWarmStartFile, no epoch found in file before WarmStartEpoch.");

   if (header.size() != restartData.size())
      throw EstimatorException("Error reading InputWarmStartFile, header row and data row have a different number of elements.");


   // Map file columns to states
   IntegerArray stateColumnNum(esm.GetStateSize(), -1);
   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      std::string stateName = DataFileStateHeader(ii);

      for (UnsignedInt jj = 0; jj < header.size(); jj++)
      {
         if (stateName == header[jj])
         {
            stateColumnNum[ii] = jj;
            break;
         }
      }
   }

   UnsignedInt firstStateIndex = epochIndex + 1U;

   // Set epoch
   esm.MapObjectsToVector();
   GmatState estimationCartState = esm.GetEstimationMJ2000EqCartesianStateForReport();
   estimationCartState.SetPrecisionTimeFlag(true);
   estimationCartState.SetEpoch(bestEpoch.GetMjd());
   estimationCartState.SetEpochGT(bestEpoch);

   // Set state
   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      if (stateColumnNum[ii] >= 0)
      {
         Real value;
         GmatStringUtil::ToReal(restartData[stateColumnNum[ii]], value);
         estimationCartState[ii] = value;
      }
   }
   esm.SetEstimationMJ2000EqCartesianStateParticipant(estimationCartState);

   ReadCovarianceFromDataFile(header, restartData, firstStateIndex, stateColumnNum);

   MessageInterface::ShowMessage("The following states were loaded from the InputWarmStartFile:\n");
   for (UnsignedInt ii = 0U; ii < stateColumnNum.size(); ii++)
   {
      if (stateColumnNum[ii] != -1)
         MessageInterface::ShowMessage("   %s\n", header[stateColumnNum[ii]].c_str());
   }
   MessageInterface::ShowMessage("\n");

   esm.MapVectorToObjects();
   esm.MapCovariancesToObjects();
   PropagationStateManager *psm = propagators[0]->GetPropStateManager();
   psm->MapObjectsToVector();
   return true;
}


//------------------------------------------------------------------------------
// void ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
//                                 UnsignedInt firstStateIndex, IntegerArray stateColumnNum)
//----------------------------------------------------------------------
/**
 * Reads the covariance from the data file
 *
 * @param header The array of header elements.
 * @param restartData The array of data elements for the restart row.
 * @param firstStateIndex The column index of the first state element in restartData.
 * @param stateColumnNum The maping of the solve for states to the restart file states.
 */
 //------------------------------------------------------------------------------
void SeqEstimator::ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
                                              UnsignedInt firstStateIndex, IntegerArray stateColumnNum)
{
   // Find column of first covariance element
   bool covFound = false;
   UnsignedInt firstCovIndex;
   for (UnsignedInt ii = 0U; ii < header.size(); ii++)
   {
      if (header[ii] == "Covariance_1_1")
      {
         covFound = true;
         firstCovIndex = ii;
         break;
      }
   }

   if (!covFound)
      throw EstimatorException("Error reading InputWarmStartFile, no covariance was found.");

   UnsignedInt fileStateSize = firstCovIndex - firstStateIndex;

   // Set covariance
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   Rmatrix dX_dS = esm.CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix dS_dX = dX_dS.Inverse();

   Rmatrix fileCov(fileStateSize, fileStateSize);
   UnsignedInt index = firstCovIndex;
   // Convert lower triangle array to lower triangle matrix
   for (UnsignedInt ii = 0; ii < fileStateSize; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         Real value;
         GmatStringUtil::ToReal(restartData[index], value);
         fileCov(ii, jj) = value;

         index++;
      }
   }

   // Map file covariance matrix to state order of the covariance in the esm
   Covariance *cov = esm.GetCovariance();
   Rmatrix covMat = *(cov->GetCovariance());

   // Convert covariance to Cartesian first
   covMat = dX_dS * covMat * dX_dS.Transpose();
   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      if (stateColumnNum[ii] >= 0)
      {
         Real iiConv = GetEpsilonConversion(ii);
         for (UnsignedInt jj = 0; jj <= ii; jj++)
         {
            if (stateColumnNum[jj] >= 0)
            {
               Real jjConv = GetEpsilonConversion(jj);

               UnsignedInt idx1 = stateColumnNum[ii] - firstStateIndex;
               UnsignedInt idx2 = stateColumnNum[jj] - firstStateIndex;
               Real value = fileCov(idx1, idx2);
               value /= iiConv * jjConv;
               covMat(ii, jj) = value;
               covMat(jj, ii) = value;
            }
         }
      }
   }

   // Convert covariance back to Keplerian
   covMat = dS_dX * covMat * dS_dX.Transpose();
   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
      for (UnsignedInt jj = 0; jj < esm.GetStateSize(); jj++)
         (*cov)(ii, jj) = covMat(ii, jj);
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
bool SeqEstimator::WriteMatData()
{
   bool retval;

   retval = Estimator::WriteMatData();
   matWriter->ClearData();

   MessageInterface::ShowMessage("Writing Filter MATLAB File...\n");

   StringArray dataDesc;

   AddMatData(matFilterData);

   dataDesc = GetMatDataDescription(matFilterData);
   matWriter->DescribeData(dataDesc, iterationsTaken);
   matWriter->WriteData("Filter");

   MessageInterface::ShowMessage("Finished Writing Filter MATLAB File.\n\n");

   return retval;
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const FilterMeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SeqEstimator::AddMatlabData(const FilterMeasurementInfoType &measStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabData(measStat, matData, matIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabData(const FilterMeasurementInfoType &measStat,
//                     DataBucket &matData, IntegerMap &matIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SeqEstimator::AddMatlabData(const FilterMeasurementInfoType &measStat,
                                 DataBucket &matData, IntegerMap &matIndex)
{
   Estimator::AddMatlabData(measStat, matData, matIndex);

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   Integer matMeasIndex = matData.GetContainerSize() - 1;

   if (matIndex.count("Cov") == 0)
   {
      matIndex["ScaledResid"] = matData.AddRealContainer("ScaledResidual", 0);
      matIndex["State"] = matData.AddRealContainer("PreUpdateState", stateSize);

      matIndex["Cov"] = matData.AddReal3DContainer("PreUpdateCovariance");
      matIndex["CovVNB"] = matData.AddReal3DContainer("PreUpdateCovarianceVNB");
      matIndex["Kalman"] = matData.AddReal3DContainer("KalmanGain");
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

   for (UnsignedInt ii = 0; ii < measStat.kalmanGain.GetNumRows(); ii++)
   {
      RealArray rowArray = measStat.kalmanGain.GetRow(ii).GetRealArray();
      matData.real3DValues[matIndex["Kalman"]][matMeasIndex].push_back(rowArray);
   }
}


//------------------------------------------------------------------------------
//  void AddMatlabFilterData(const UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SeqEstimator::AddMatlabFilterData(const UpdateInfoType &updateStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabFilterData(updateStat, matFilterData, matFilterIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabFilterData(const UpdateInfoType &updateStat,
//                           DataBucket &matFilterData, IntegerMap &matFilterIndex)
//------------------------------------------------------------------------------
/**
 * This method adds the filter data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void SeqEstimator::AddMatlabFilterData(const UpdateInfoType &updateStat,
                                       DataBucket &matFilterData, IntegerMap &matFilterIndex)
{
   Integer matMeasIndex;

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();

   bool initial = false;

   if (matFilterIndex.count("EpochTAI") == 0)
   {
      initial = true;

      matFilterData.SetInitialRealValue(NAN);
      matFilterIndex["EpochTAI"] = matFilterData.AddRealContainer("EpochTAI", 2);
      matFilterIndex["EpochUTC"] = matFilterData.AddRealContainer("EpochUTC", 2);
      matFilterIndex["MeasNum"] = matFilterData.AddRealContainer("MeasurementNumber");
      matFilterIndex["Type"] = matFilterData.AddStringContainer("UpdateType");

      matFilterIndex["State"] = matFilterData.AddRealContainer("State", stateSize);
      matFilterIndex["Cov"] = matFilterData.AddReal3DContainer("Covariance");

      matFilterIndex["CovVNB"] = matFilterData.AddReal3DContainer("CovarianceVNB");
      matFilterIndex["ProcNoise"] = matFilterData.AddReal3DContainer("ProcessNoise");
      matFilterIndex["STM"] = matFilterData.AddReal3DContainer("StateTransitionMatrix");
   }

   matMeasIndex = matFilterData.AddPoint();

   std::string temp;

   Real taiEpoch = theTimeConverter->Convert(updateStat.epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD).GetMjd();
   Real utcEpoch = theTimeConverter->Convert(updateStat.epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD).GetMjd();

   matFilterData.realValues[matFilterIndex["EpochTAI"]][matMeasIndex][0] = taiEpoch + MATLAB_DATE_CONVERSION;
   matFilterData.realValues[matFilterIndex["EpochTAI"]][matMeasIndex][1] = taiEpoch;
   matFilterData.realValues[matFilterIndex["EpochUTC"]][matMeasIndex][0] = utcEpoch + MATLAB_DATE_CONVERSION;
   matFilterData.realValues[matFilterIndex["EpochUTC"]][matMeasIndex][1] = utcEpoch;

   for (UnsignedInt ii = 0; ii < updateStat.covVNB.GetNumRows(); ii++)
   {
      RealArray rowArray = updateStat.covVNB.GetRow(ii).GetRealArray();
      matFilterData.real3DValues[matFilterIndex["CovVNB"]][matMeasIndex].push_back(rowArray);
   }

   for (UnsignedInt ii = 0; ii < updateStat.processNoise.GetNumRows(); ii++)
   {
      RealArray rowArray = updateStat.processNoise.GetRow(ii).GetRealArray();
      matFilterData.real3DValues[matFilterIndex["ProcNoise"]][matMeasIndex].push_back(rowArray);
   }

   for (UnsignedInt ii = 0; ii < updateStat.stm.GetNumRows(); ii++)
   {
      RealArray rowArray = updateStat.stm.GetRow(ii).GetRealArray();
      matFilterData.real3DValues[matFilterIndex["STM"]][matMeasIndex].push_back(rowArray);
   }

   if (updateStat.isObs)
   {
      matFilterData.stringValues[matFilterIndex["Type"]][matMeasIndex][0] = "Measurement";
      matFilterData.realValues[matFilterIndex["MeasNum"]][matMeasIndex][0] = updateStat.measStat.recNum;
   }
   else
   {
      if (initial)
         matFilterData.stringValues[matFilterIndex["Type"]][matMeasIndex][0] = "Initial";
      else if (isPredicting && !hasAnchorEpoch)
         matFilterData.stringValues[matFilterIndex["Type"]][matMeasIndex][0] = "Predict";
      else
         matFilterData.stringValues[matFilterIndex["Type"]][matMeasIndex][0] = "Time";
   }

   matFilterData.realValues[matFilterIndex["State"]][matMeasIndex] = updateStat.state;

   for (Integer ii = 0; ii < stateSize; ii++)
   {
      RealArray rowArray = updateStat.cov.GetRow(ii).GetRealArray();
      matFilterData.real3DValues[matFilterIndex["Cov"]][matMeasIndex].push_back(rowArray);
   }
}


//------------------------------------------------------------------------------
//  void AddMatlabConfigData()
//------------------------------------------------------------------------------
/**
 * This method adds the estimation configuration data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void SeqEstimator::AddMatlabConfigData()
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
//  bool VerifySmoothTimeStep()
//------------------------------------------------------------------------------
/**
 * This method checks the forward filter for epoch that the back filter 
 * is attempting to use.
 *
 * Returns true if found false if not
 */
 //------------------------------------------------------------------------------
bool SeqEstimator::VerifySmoothTimeStep()
{

	bool found = false;

	int high = forwardFilterInfo->size() - 1;
	int low = 0;
	int index = 0;

	while (high >= low)
	{
		index = low + (high - low) / 2;

		if (GmatMathUtil::IsEqual(currentEpochGT, forwardFilterInfo->at(index).epoch, ESTTIME_ROUNDOFF))
		{
			found = true;
			break;
		}

		if (forwardFilterInfo->at(index).epoch > currentEpochGT)
			high = index - 1;
		else
			low = index + 1;
	}

	return found;
}


//------------------------------------------------------------------------------
//  Real MatchSmoothTimeStep()
//------------------------------------------------------------------------------
/**
 * This method finds the next epoch in the forwad filter that the back 
 * filter should step to. 
 *
 * Returns the difference between the currentEpochGT and the epoch in the
 * next epoch in the forward filter the back filter should propagate to.
 *
 */
 //------------------------------------------------------------------------------
Real SeqEstimator::MatchSmoothTimeStep()
{
	bool found = false;

	int high = forwardFilterInfo->size() - 1;
	int low = 0;
	int index = 0;

	GmatTime lastBackEpoch = updateStats[updateStats.size() - 1].epoch;

	while (high >= low)
	{
		index = low + (high - low) / 2;

		if (GmatMathUtil::IsEqual(lastBackEpoch, forwardFilterInfo->at(index).epoch, ESTTIME_ROUNDOFF))
		{
			found = true;
			break;
		}

		if (forwardFilterInfo->at(index).epoch > lastBackEpoch)
			high = index - 1;
		else
			low = index + 1;
	}

	if (!found)
		throw "Could not find a valid epoch for back filter to use in the forward filter";

   while (true)
   {
      if (index > 0)
      {
         if (GmatMathUtil::IsEqual(forwardFilterInfo->at(index - 1).epoch, forwardFilterInfo->at(index).epoch, ESTTIME_ROUNDOFF))
            index--;
         else
            break;
      }
      else
         break;
   }
	return (forwardFilterInfo->at(index - 1).epoch - currentEpochGT).GetTimeInSec();

}

//------------------------------------------------------------------------------
// std::string GetMeasurementResidualTitles()
//------------------------------------------------------------------------------
/**
 * Returns the Mesurement residual titles to be used in report files.
 */
 //------------------------------------------------------------------------------
std::string SeqEstimator::GetMeasurementResidualTitles()
{
   if(textFileMode == "Normal")
      return "RecNum  UTCGregorian-Epoch        Obs-Type            " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit          Observed (O)          Computed (C)     Pre-Update (O-C)  Elev.\n";
   else
      return "RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit               Obs (O)    Obs-Correction (O)               Cal (C)     Pre-Update (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|    Elevation-Angle    Partial-Derivatives";
}
