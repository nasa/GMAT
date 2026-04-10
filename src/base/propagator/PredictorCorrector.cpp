//$Id$
//------------------------------------------------------------------------------
//                              PredictorCorrector
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
// *** File Name : predictorcorrector.cpp
// *** Created   : October 1, 2002
// *****************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)        ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                     ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                                ***
// ***                                                                       ***
// ***  This software is subject to the Software Usage Agreement described   ***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement       ***
// ***  must be included in any distribution.  Removal of this header is     ***
// ***  strictly prohibited.                                                 ***
// ***                                                                       ***
// ***                                                                       ***
// ***  Header Version: July 12, 2002                                        ***
// *****************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/7/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/18/2003 - D. Conway, Thinking Systems, Inc.
//                             Reworked step control for fixed step mode
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Moved time step granularity to Integrator
//                             class
//
//                           : 6/17/2004 - A. Greene, Missions Applications
//                              Changes:
//                                - Updated style using GMAT cpp style guide
// **************************************************************************


#include "PredictorCorrector.hpp"
#include "MessageInterface.hpp"
#include <fstream>

#include "MessageInterface.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
PredictorCorrector::PARAMETER_TEXT[PredictorCorrectorParamCount - IntegratorParamCount] =
{
   "StepCount",
   "MaximumError",
   "LowerError",
   "TargetError",
   "StepSign",
   "StartupComplete",
   "StartupCount",
   "InvOrder"
};

const Gmat::ParameterType
PredictorCorrector::PARAMETER_TYPE[PredictorCorrectorParamCount - IntegratorParamCount] =
{
        Gmat::INTEGER_TYPE,
        Gmat::REAL_TYPE,
        Gmat::REAL_TYPE,
        Gmat::REAL_TYPE,
        Gmat::REAL_TYPE,
        Gmat::BOOLEAN_TYPE,
        Gmat::INTEGER_TYPE,
        Gmat::REAL_TYPE,
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PredictorCorrector(Integer sc, Integer order, const std::string &typeStr,
//       const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The Predictor-Corrector Constructor 
 *
 * @param sc    Size of the Predictor-Corrector buffer needed to advance
 *                   the state (basically the amount of history that needs to be
 *                   preserved from step to step)
 *
 * @param order Truncation order of the highest order term kept in the
 *                   series expansion used to construct the algorithm.  This
 *                   value, inverted, is used in the stepsize control algorithm
 *                   to determine new step sizes if the estimated error falls
 *                   outside of the target error bounds.
 * @param typeStr The scripted type of the integrator
 * @param nomme The name of the new object
 */
//------------------------------------------------------------------------------
PredictorCorrector::PredictorCorrector(Integer sc, Integer order, 
                  const std::string &typeStr, const std::string &nomme) :
   Integrator                      (typeStr, nomme),
   stepCount                       (sc),
   ddt                             (NULL),
   history                         (NULL),
   pweights                        (NULL),
   cweights                        (NULL),
   predictorState                  (NULL),
   correctorState                  (NULL),
   maxError                        (0.0),
   lowerError                      (1.0e-13),
   targetError                     (1.0e-11),
   stepSign                        (1.0),
   startupComplete                 (false),
   startupCount                    (0),
   starter                         (NULL),
   invOrder                        (1.0/order)
{
   objectTypeNames.push_back("PredictorCorrector");
   parameterCount = PredictorCorrectorParamCount;
   tolerance = 1.0e-10;
}


//------------------------------------------------------------------------------
// ~PredictorCorrector(void)
//------------------------------------------------------------------------------
/**
 * The PredictorCorrector destructor
 */
//------------------------------------------------------------------------------
PredictorCorrector::~PredictorCorrector(void)
{
   if (history)
   {
      for (int i = 0; i < stepCount; i++)
         if (history[i])
            delete [] history[i];
      delete [] history;
   }

   if (pweights)
      delete [] pweights;

   if (cweights)
      delete [] cweights;

   if (predictorState)
      delete [] predictorState;

   if (correctorState)
      delete [] correctorState;

   if (starter)
      delete starter;
}


//------------------------------------------------------------------------------
// PredictorCorrector(const PredictorCorrector& pc)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 *
 * @param pc The Predictor-Corrector used as a template for this new one
 */
//------------------------------------------------------------------------------
PredictorCorrector::PredictorCorrector(const PredictorCorrector& pc) :
   Integrator                      (pc),
   stepCount                       (pc.stepCount),
   ddt                             (NULL),
   history                         (NULL),
   historyBuffer                   (pc.historyBuffer),
   pweights                        (NULL),
   cweights                        (NULL),
   predictorState                  (NULL),
   correctorState                  (NULL),
   maxError                        (pc.maxError),
   lowerError                      (pc.lowerError),
   targetError                     (pc.targetError),
   stepSign                        (pc.stepSign),
   startupComplete                 (false),
   startupCount                    (0),
   starter                         (NULL),
   invOrder                        (pc.invOrder)
{
   parameterCount = PredictorCorrectorParamCount;
   tolerance = pc.tolerance;
   isInitialized = false;
}


//------------------------------------------------------------------------------
// PredictorCorrector& operator=(const PredictorCorrector& pc)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param pc The Predictor-Corrector used as a template for the new settings on
 *           this one
 */
//------------------------------------------------------------------------------
PredictorCorrector& PredictorCorrector::operator=(const PredictorCorrector& pc)
{
   if (this == &pc)
      return *this;

   Integrator::operator=(pc);
   stepCount = pc.stepCount;
   maxError = pc.maxError;
   lowerError = pc.lowerError;
   targetError = pc.targetError;
   stepSign = pc.stepSign;
   startupComplete = false;
   startupCount = 0;
   invOrder = pc.invOrder;
   isInitialized = false;
   historyBuffer = pc.historyBuffer;
    
   ddt = NULL;
   history = NULL;
   pweights = NULL;
   cweights = NULL;
   predictorState = NULL;
   correctorState = NULL;
   starter = NULL;

   return *this;
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Method used to setup the data structures for the algorithm 
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Initialize()
{
//    Propagator::Initialize();

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Initializing %s\n", typeName.c_str());
   #endif

   isInitialized = false;
   if (stepCount <= 0)
      return isInitialized;

   // Check the P-C error control tolerances
   if ((lowerError >= targetError) || (targetError >= tolerance))
      throw PropagatorException("Setup error in the " + typeName +
             " Propagator; Predictor-Corrector integrators require that the "
             "error control settings satisfy the relationship\n\n"
             "   LowerError < TargetError < Accuracy\n\n"
             "and work best if the settings differ from one another by at "
             "least a factor of 10", Gmat::ERROR_);

   if (physicalModel)
   {
      dimension = physicalModel->GetDimension();
      historyBuffer.resize(dimension);

      if (history)
      {
         for (int i = 0; i < stepCount; i++)
            if (history[i])
               delete [] history[i];
         delete [] history;
         history = NULL;
      }

      if (pweights)
      {
         delete [] pweights;
         pweights = NULL;
      }

      if (cweights)
      {
         delete [] cweights;
         cweights = NULL;
      }

      if (predictorState)
      {
         delete [] predictorState;
         predictorState = NULL;
      }

      if (correctorState)
      {
         delete [] correctorState;
         correctorState = NULL;
      }

      if (errorEstimates)
      {
         delete [] errorEstimates;
         errorEstimates = NULL;
      }

      predictorState = new Real[dimension];
      if (!predictorState)
      {
         return isInitialized;
      }

      correctorState = new Real[dimension];
      if (!correctorState)
      {
         delete [] predictorState;
         predictorState = NULL;
         return isInitialized;
      }

      errorEstimates = new Real[dimension];
      if (!errorEstimates)
      {
         delete [] predictorState;
         predictorState = NULL;
         delete [] correctorState;
         correctorState = NULL;
         return isInitialized;
      }

      pweights = new Real[stepCount];
      if (!pweights)
      {
         delete [] predictorState;
         predictorState = NULL;
         delete [] correctorState;
         correctorState = NULL;
         delete [] errorEstimates;
         errorEstimates = NULL;
         return isInitialized;
      }

      cweights = new Real[stepCount];
      if (!cweights)
      {
         delete [] pweights;
         pweights = NULL;
         delete [] predictorState;
         predictorState = NULL;
         delete [] correctorState;
         correctorState = NULL;
         delete [] errorEstimates;
         errorEstimates = NULL;
         return isInitialized;
      }

      history = new Real*[stepCount];
      if (history)
      {
         for (int i = 0; i < stepCount; i++)
         {
            history[i] = new Real[dimension];
            if (!history[i])
            {
               for (int j = 0; j < i; j++)
               {
                  delete [] history[j];
               }
               delete [] history;
               history = NULL;
               delete [] pweights;
               pweights = NULL;
               delete [] cweights;
               cweights = NULL;
               delete [] predictorState;
               predictorState = NULL;
               delete [] correctorState;
               correctorState = NULL;
               delete [] errorEstimates;
               errorEstimates = NULL;
               return isInitialized;
            }
            if (SetWeights())
            {
               isInitialized = true;
            }
         }
      }
      else
      {
         delete [] pweights;
         pweights = NULL;
         delete [] cweights;
         cweights = NULL;
         delete [] predictorState;
         predictorState = NULL;
         delete [] correctorState;
         correctorState = NULL;
         delete [] errorEstimates;
         errorEstimates = NULL;
      }

      // Setup the starter
      if (starter == NULL)
         starter = new RungeKutta89;

      starter->SetPhysicalModel(physicalModel);

      if (fabs(stepSizeBuffer) < minimumStep)
         stepSizeBuffer = minimumStep * stepSign;
      if (fabs(stepSizeBuffer) > maximumStep)
         stepSizeBuffer = maximumStep * stepSign;

      starter->SetRealParameter("InitialStepSize", stepSize);
      starter->SetBooleanParameter(STOP_IF_ACCURACY_VIOLATED,
               stopIfAccuracyViolated);
      starter->SetRealParameter(ACCURACY, tolerance);
      starter->SetRealParameter(MIN_STEP, minimumStep);
      starter->SetRealParameter(MAX_STEP, maximumStep);
      starter->TakeAction("ChangeTypeSourceString",
            "Starter for Predictor-Corrector");
      starter->Initialize();

      inState  = physicalModel->GetState();
      outState = physicalModel->GetState();
   }
    
   accuracyWarningTriggered = false;

   Reset();

   return isInitialized;
}

//------------------------------------------------------------------------------
// bool Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Method to advance the system by a fixed interval
 *
 * @param dt The interval
 *
 * @return true on success, false if the step failed
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Step(Real dt)
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Called PredictorCorrector::Step(%lf)\n",
            dt);
   #endif

   timeleft = dt;

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Stepsize: %lf dt: %lf ", stepSize, dt);
   #endif
   if (fabs(dt) != fabs(stepSize))
   {
      // Processing for case: |dt| <> |stepSize|
      if (fabs(dt) < fabs(stepSize))
      {
         // Processing for case: |dt| < |stepSize|
         stepSize = dt;
         Reset();
      }
      else
      {
         // Processing for case: |dt| > |stepSize|
         // ss is number of steps in real number in order to complete a period of time dt 
         // in this case: ss > 1.0
         Real ss = fabs(dt / stepSize);
         
         // value of ss is greater than 1 
         // value of ss-int(ss) is inside of the range [0 , 1)
         if (fabs(ss - int(ss)) > smallestTime)
         {
            stepSize = dt / ((int)(ss+1));
            Reset();
         }
         else
         {
            // what is value of stepSize for this case? 

         }
      }
   }
   else
   {
      // Processing for case: |dt| == |stepSize|
      // What is the value of stepSize for this case?
      // for dt == - stepSize and dt == stepSize ?

   }

   bool fixed = fixedStep;
   fixedStep = true;

   // Is this line below should be:   stepSign = ((stepSize > 0.0) ? 1.0 : -1.0);
   // Using stepSign variable in Interator.hpp instead
   Real stepsign = ((stepSize > 0.0) ? 1.0 : -1.0);
   do
   {
      if (!Step())
         return false;
   }
   while (timeleft * stepsign > smallestTime);
   // Is the line above should be?:   while (fabs(timeleft) > smallestTime);
   fixedStep = fixed;

   return true;
}

//------------------------------------------------------------------------------
// bool BufferHistory()
//------------------------------------------------------------------------------
/**
 * Method used to revert predictor-correct history by one-step for LastStep
 * clean-up
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::BufferHistory()
{
   bool retval = false;
   if (historyBuffer.size() > 0)
   {
      for (Integer i = stepCount -1; i > 0 ; i--)
      {
         memcpy(history[i], history[i - 1], dimension * sizeof(double));

      }
      memcpy(history[0] + 0 * dimension, &historyBuffer, dimension * sizeof(double));
      retval = true;
   }
   return retval;
}


//------------------------------------------------------------------------------
// bool Step()
//------------------------------------------------------------------------------
/**
 * Method used to advance the Predictor-Corrector one step
 *
 * @return true on success, false if the step failed
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Called PredictorCorrector::Step()\n");
   #endif

   if (!isInitialized)
      return false;

//    if (physicalModel->StateChanged(true))
//    {
//       MessageInterface::ShowMessage("   Resetting in Step()\n");
//       Reset();
//    }

   bool wasStartup = false;

   do
   {
      if (!startupComplete)
      {
         wasStartup = true;
         if (ddt == NULL)
         {
            ddt = physicalModel->GetDerivativeArray();
            if (ddt == NULL)
               return false;
         }
         // First load up the derivatives for the current state
         physicalModel->GetDerivatives(inState);
         memcpy(history[startupCount+1], ddt,
            dimension * sizeof(Real));
         // Ensure first row has assigned values in case where history doesn't update on first step
         if (startupCount == 0)
         {
            memcpy(history[startupCount], ddt,
               dimension * sizeof(Real));
         }
         #ifdef DEBUG_PROPAGATION
            MessageInterface::ShowMessage("startup ddt = [%le %le %le...]\n", ddt[0], ddt[1], ddt[2]);
         #endif
         bool retval = FireStartupStep();
         if (retval)
            timeleft -= stepTaken;
         else
            return false;

         // Omit any error from the starter code -- we assume it is good
         // enough for startup purposes
         maxError = 0.0;
      }
      else
      {
         #ifdef DEBUG_PROPAGATION
            MessageInterface::ShowMessage("Pred-Corr step ");
         #endif
         ++stepAttempts;

         if (!Predict())
            return false;
         if (!Correct())
            return false;
         if (EstimateError() < 0.0)
            return false;
         if (maxError <= tolerance)
         {
            memcpy(outState, correctorState, dimension * sizeof(Real));
            physicalModel->IncrementTime(stepSize);
            stepTaken = stepSize;
            timeleft -= stepTaken;
         }

         #ifdef DEBUG_PROPAGATION
            MessageInterface::ShowMessage("Max error = %le, tolerance = %le\n",
                  maxError, tolerance);
         #endif

         if ((maxError > tolerance) || (maxError < lowerError))
            if (maxError != 0.0)
               if (!AdaptStep(maxError))
                  return false;
      }

      if (stepAttempts >= maxStepAttempts)
         return false;
   } while (maxError > tolerance);

   #ifdef DEBUG_PROPAGATION
       MessageInterface::ShowMessage("Good step\n");
   #endif

   if ((startupComplete) && (wasStartup == false))
   {
      stepAttempts = 0;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * For the P-C integrators, this method just returns false.
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::RawStep()
{
   return false;
}

//------------------------------------------------------------------------------
// bool AdaptStep(Real maxError)
//------------------------------------------------------------------------------
/**
 * Method used to change the stepsize
 *
 * The default implementation halves the stepsize, and then resets the algorithm
 * to try again.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::AdaptStep(Real maxError)
{
   Real newStep = stepSize * pow(targetError / maxError, invOrder);

   // The following code is in place to omit error control for steps
   // at the minimum stepsize.  See the note above for more
   // information.  Remove this block if the issue gets resolved.
   if (GmatMathUtil::Abs(stepSize) == minimumStep)
   {
      if (stopIfAccuracyViolated)
      {
         throw PropagatorException(typeSource + ": Accuracy settings will be "
               "violated with current step size values.\n");
      }
      else
      {
         if (!accuracyWarningTriggered)
         {
            accuracyWarningTriggered = true;
            MessageInterface::ShowMessage("**** Warning **** %s: Accuracy "
                  "settings will be violated with current step size values.\n",
                  typeSource.c_str());
         }
         return true;
      }
   }

   // Make sure new step is in the accepted range
   if (fabs(newStep) < minimumStep)
      newStep = minimumStep * stepSign;
   if (fabs(newStep) > maximumStep)
      newStep = maximumStep * stepSign;

   if (!fixedStep)
   {
      // Variable step mode
      if (maxError > tolerance)
      {
         // Tried and failed at the minimum stepsize
         if (GmatMathUtil::IsEqual(GmatMathUtil::Abs(stepSize),minimumStep))
         {
            if (stopIfAccuracyViolated)
            {
               throw PropagatorException(
                     "PredictorCorrector: Accuracy settings will be violated "
                     "with current step size values.\n");
            }
            else
            {
               // Only write the warning once per propagation command
               if (!accuracyWarningTriggered)
               {
                  accuracyWarningTriggered = true;
                  MessageInterface::PopupMessage(Gmat::WARNING_,
                     "PredictorCorrector: Accuracy settings will be violated "
                     "with current step size values.\n");
               }
               return false;
            }
         }
         stepSize = newStep;
         ++stepAttempts;
      }
      else
      {
         // Step can be "safely" increased -- but only up to twice old value
         if (newStep >= 2.0 * stepSize)
            stepSize *= 2.0;
         else
            stepSize = newStep;

         stepAttempts = 0;
      }
   }
   else
   {
      // Fixed step mode: Adjust newStep to be a power of 2 different from
      // the current step
      Real twoStep = stepSize;
      if (fabs(newStep) < fabs(stepSize))
      {
         do
         {
            twoStep /= 2.0;
         } while (fabs(twoStep) > fabs(newStep));
         stepSize = twoStep;
      }
      else if (fabs(newStep) >= fabs(2.0 * stepSize))
      {
         // Check to see if the step can safely be Reald, given the
         // remaining interval
         Real stepsToGo = timeleft / (2.0 * stepSize);
         if (stepsToGo == int(stepsToGo))
            stepSize *= 2.0;
         else
            return true;
      }
      else        // Cannot increase this stepsize in fixed step mode
         return true;

      while (fabs(stepSize) < minimumStep)
         stepSize *= 2.0;
   }

   return Reset();
}
//------------------------------------------------------------------------------
// std::string GetParameterText(const int id) const
//------------------------------------------------------------------------------
/**
 * Helper method used to find the text name for the class parameters
 *
 * This static method is used to obtain text descriptions for the parameters 
 * defined by the class.  These strings can be used to fill in the descriptors
 * for user interface elements, so that those elements have labels that match 
 * the intent of each of the user selectable parameters.
 *
 * @param parm ID of the parameter that needs the text string
 *
 * @return The text
 */
//------------------------------------------------------------------------------
std::string PredictorCorrector::GetParameterText(const Integer id) const
{
   if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
      return PARAMETER_TEXT[id - IntegratorParamCount];
   else
      return Integrator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::GetParameterID(const std::string &str) const
{
   for (Integer i = STEP_COUNT; i < PredictorCorrectorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - IntegratorParamCount])
         return i;
   }

   return Integrator::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PredictorCorrector::GetParameterType(const Integer id) const
{
   if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
      return PARAMETER_TYPE[id - IntegratorParamCount];
   else
      return Integrator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PredictorCorrector::GetParameterTypeString(const Integer id) const
{
   if (id >= STEP_COUNT && id < PredictorCorrectorParamCount)
      return Integrator::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Integrator::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Method used to determine is a parameter should be hidden when writing scripts
 * 
 * @param <id> The parameter id.
 * 
 * @return true is the parameter should be hidden, false if it is visible.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::IsParameterReadOnly(const Integer id) const
{
   if ((id == STEP_COUNT) || (id == MAXIMUM_ERROR) || (id == STEP_SIGN) || 
       (id == STARTUP_COMPLETE) || (id == STARTUP_COUNT) || (id == INV_ORDER))
      return true;
      
   return Integrator::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Method used to determine is a parameter should be hidden when writing scripts
 * 
 * @param <label> The parameter name.
 * 
 * @return true is the parameter should be hidden, false if it is visible.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::GetRealParameter(const Integer id) const
{
   if (id == MAXIMUM_ERROR)
      return maxError;
   else if (id == LOWEVER_ERROR)
      return lowerError;
   else if (id == TARGET_ERROR)
      return targetError;
   else if (id == STEP_SIGN)
      return stepSign;
   else if (id == INV_ORDER)
      return invOrder;

   return Integrator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real PredictorCorrector::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);

   return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Method used to Set parameters for the BS Integrator.
 *
 * This method sets the tolerance and tolerance limit for the Bulirsch-Stoer 
 * integrator, and calls Integrator::SetParameter() for other parameters.  The
 * implementation here prevents setting tolerances of more than 100%.  If the
 * minimum tolerance is set to a value larger than the current tolerance, the
 * current value is raised to the minimum value.
 * 
 * @param id  ID for the parameter being set
 * @param value  New value for the parameter
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::SetRealParameter(const Integer id, const Real value)
{
   if (id == MAXIMUM_ERROR)
   {
      maxError = value;
      return maxError;
   }
   else if (id == LOWEVER_ERROR)
   {
      lowerError = value;
      return lowerError;
   }
   else if (id == TARGET_ERROR)
   {
      targetError = value;
      return targetError;
   }
   else if (id == STEP_SIGN)
   {
      stepSign = value;
      return stepSign;
   }
   else if (id == INV_ORDER)
   {
      invOrder = value;
      return invOrder;
   }

   return Integrator::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real PredictorCorrector::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 * @see GmatBase
 * @param id ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::GetIntegerParameter(const Integer id) const
{
   if (id == STEP_COUNT)
      return stepCount;
   else if (id == STARTUP_COUNT)
      return startupCount;

   return Integrator::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter ID.
 * @see GmatBase
 * @param id ID for the requested parameter.
 * @param value Integer value for the requested parameter.
 *
 * @return Integer value of the requested parameter.
 */
//------------------------------------------------------------------------------
Integer PredictorCorrector::SetIntegerParameter(const Integer id,
      const Integer value) // const?
{
   if (id == STEP_COUNT)
   {
      stepCount = value;
      return stepCount;
   }
   else if (id == STARTUP_COUNT)
   {
      startupCount = value;
      return startupCount;
   }
   return Integrator::SetIntegerParameter(id,value);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the bool parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  bool value of the requested parameter.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::GetBooleanParameter(const Integer id) const
{
   if (id == STARTUP_COMPLETE)
      return startupComplete;

   return Integrator::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool PredictorCorrector::SetBooleanParameter(const Integer id, const bool value)
{
   if (id ==  STARTUP_COMPLETE)
   {
      startupComplete = value;
      return startupComplete;
   }

   return Integrator::SetBooleanParameter(id,value);
}
