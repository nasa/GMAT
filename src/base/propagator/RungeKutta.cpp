//$Id$
//------------------------------------------------------------------------------
//                              RungeKutta
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
// *** File Name : RungeKutta.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/19/2003 - D. Conway, Thinking Systems, Inc.
//                             Added code to manage min and max allowed
//                             stepsizes
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Fixed bug found with final step while stepping
//                             backwards
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/21/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                              Removals:
//                              Additions:
// **************************************************************************

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"
#include "MessageInterface.hpp"
#include "ODEModel.hpp"

//#define DEBUG_PROPAGATOR_FLOW
//#define DEBUG_RAW_STEP_STATE
//#define DEBUG_STEPSIZE

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// RungeKutta::RungeKutta(Integer st, Integer order)
//------------------------------------------------------------------------------
/**
 * The RK Constructor
 *
 * @param st    Number of stages in the specific algorithm implemented
 * @param order Order of the expansion used for the integrator
 */
//------------------------------------------------------------------------------
RungeKutta::RungeKutta(Integer st, Integer order, const std::string &typeStr,
                                           const std::string &nomme) :
    Integrator      (typeStr, nomme),
    stages          (st),
    ki              (NULL),
    ai              (NULL),
    bij             (NULL),
    cj              (NULL),
    ee              (NULL),
    sigma           (0.9),
    incPower        (1.0/order),
    decPower        (1.0/(order-1)),
    stageState      (NULL),
    candidateState  (NULL)
{
}

//------------------------------------------------------------------------------
// RungeKutta::~RungeKutta(void)
//------------------------------------------------------------------------------
/**
 * The RK destructor
 */
//------------------------------------------------------------------------------
RungeKutta::~RungeKutta()
{
    ClearArrays();
}

//------------------------------------------------------------------------------
// RungeKutta::RungeKutta(const RungeKutta& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
//------------------------------------------------------------------------------
RungeKutta::RungeKutta(const RungeKutta& rk) :
    Integrator      (rk),
    stages          (rk.stages),
    ki              (NULL),
    ai              (NULL),
    bij             (NULL),
    cj              (NULL),
    ee              (NULL),
    sigma           (rk.sigma),
    incPower        (rk.incPower),
    decPower        (rk.decPower),
    stageState      (NULL),
    candidateState  (NULL)
{
}

//------------------------------------------------------------------------------
// RungeKutta& RungeKutta::operator=(const RungeKutta& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
RungeKutta& RungeKutta::operator=(const RungeKutta& rk)
{
    if (this == &rk)
        return *this;

    Integrator::operator=(rk);
    stages = rk.stages;
    sigma = rk.sigma;
    incPower = rk.incPower;
    decPower = rk.decPower;

    ClearArrays();

    ki = NULL;
    ai = NULL;
    bij = NULL;
    cj = NULL;
    ee = NULL;
    stageState = NULL;
    candidateState = NULL;

    isInitialized = false;

    return *this;
}

//------------------------------------------------------------------------------
// void RungeKutta::SetPhysicalModel(PhysicalModel *pPhysicalModel)
//------------------------------------------------------------------------------
/**
 * Sets up the derivative source
 *
 * This method calls the Integrator::SetDerivatives() method.  If that method
 * succeeds and the system reports that it is initialized, the accumulator
 * arrays are also setup.
 */
//------------------------------------------------------------------------------
void RungeKutta::SetPhysicalModel(PhysicalModel *pPhysicalModel)
{
    Integrator::SetPhysicalModel(pPhysicalModel);
    if (isInitialized)
       SetupAccumulator();
}

//------------------------------------------------------------------------------
// bool RungeKutta::Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the internal data structures
 *
 * This method allocates the data arrays and calls the functions that set the
 * RK coefficients and accumulator array (ki) memory allocation.  If any of the
 * allocation fails, the internal initialized flag is set to false.
 *
 * @return true upon successful initialization
 */
//------------------------------------------------------------------------------
bool RungeKutta::Initialize()
{
    Propagator::Initialize();
    if (stages <= 0)
    {
        isInitialized = false;
        return isInitialized;
    }

    ClearArrays();

    if ((ai = new Real [stages]) == NULL)
    {
        isInitialized = false;
        return isInitialized;
    }
    if ((bij = new Real*[stages]) == NULL)
    {
        delete [] ai;
        isInitialized = false;
        return isInitialized;
    }
//    else
//    {
//       // allocate memory for each row for matrix bij
//       for (UnsignedInt i = 0; i < stages; ++i)
//          bij[i] = new Real[stages];
//    }

    if ((cj = new Real [stages]) == NULL)
    {
        delete [] ai;
        delete [] bij;
        isInitialized = false;
        return isInitialized;
    }
    if ((ki = new Real*[stages]) == NULL)
    {
        delete [] ai;
        // Note: If we reach this point, the following is a leak
        delete [] bij;
        delete [] cj;
        isInitialized = false;
        return isInitialized;
    }
//    else
//    {
//       // allocate memory for each row for matrix ki
//       for (UnsignedInt i = 0; i < stages; ++i)
//          ki[i] = new Real[stages];
//    }

    if ((ee = new Real [stages]) == NULL)
    {
        delete [] ai;
        delete [] bij;
        delete [] cj;
        delete [] ki;
        isInitialized = false;
        return isInitialized;
    }

    for (int i = 0; i < stages; i++)
    {
        ki[i] = NULL;
        if ((bij[i] = new Real[i+1]) == NULL)
        {
            for (int j = 0; j < i; j++)
                delete [] bij[j];
            delete [] ai;
            delete [] bij;
            delete [] cj;
            delete [] ki;
            isInitialized = false;
            return isInitialized;
        }
    }

    if (derivativeOrder == 1)
    {
       SetCoefficients();
       SetupAccumulator();
    }

    isInitialized = true;
    return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::Step()
//------------------------------------------------------------------------------
/**
 * The RK propagation method
 *
 * This method implements the heart of the Runge-Kutta integration
 *
 * @return      true if a good step was taken, false if no good step was found or
 *              if another failure was encountered (a bad force model call, for
 *              example)
 */
//------------------------------------------------------------------------------
bool RungeKutta::Step()
{
    #ifdef DEBUG_PROPAGATOR_FLOW
       MessageInterface::ShowMessage(".");
    #endif

    // Force epoch updates at every step (a test, but left in place in case
    // it's needed later -- DJC
    // ((ForceModel*)physicalModel)->UpdateInitialData();

    if (!isInitialized)
    {
       MessageInterface::ShowMessage("RK not initialized\n");
       return false;
    }

    if ((fabs(stepSize) < minimumStep) && !finalStep && !followUpStep)
        stepSize = ((stepSize > 0.0) ? minimumStep : -minimumStep);
    if (fabs(stepSize) > maximumStep)
        stepSize = ((stepSize > 0.0) ? maximumStep : -maximumStep);

    // Store the original time and step data in case it needs to be restored
    Real originalTime = physicalModel->GetTime();
    Real originalStep = stepSize;

    bool stepLimited = false; // Is the step limited by the force model?

    lastBurn = false;

    // Get the maximum step size allowed by the force models
    Real forceMaxStep = physicalModel->GetForceMaxStep(stepSize > 0.0);
    while (fabs(stepSize) > fabs(forceMaxStep))
    {
       // Check to see if there is a Finite Burn present
       if (!finalStep && hasFiniteBurn == false)
       {
          ODEModel * checker = (ODEModel *)physicalModel;
          for (Integer indexer = 0; indexer < checker->GetNumForces(); indexer++)
          {
             if (checker->GetForce(indexer)->GetTypeName() == "FileThrust" && typeSource != "Starter for Predictor-Corrector")
             {
                hasFiniteBurn = true;
             }
          }
       }

       // Make sure forceMaxStep is not smaller than the precision
       Real stepPrecision = physicalModel->GetStepPrecision(forceMaxStep);
       if (GmatMathUtil::Abs(forceMaxStep) < GmatMathUtil::Abs(stepPrecision))
       {
          // If the forceMaxStep is smaller than the elapsedTime precision,
          // increase the elapsedTime to the next floating point representation
          // and check the forceMaxStep again
          physicalModel->SetTime(physicalModel->GetTime() + stepPrecision);
          forceMaxStep = physicalModel->GetForceMaxStep(stepSize > 0.0);
       }
       else
       {
          stepLimited = true;
          stepSize = forceMaxStep;
       }
    }

    
    if (fabs(forceMaxStep) - fabs(stepSize) < 1e-6)
    {
       // Check to see if there is a Finite Burn present
       if (!finalStep && hasFiniteBurn == false)
       {
          ODEModel * checker = (ODEModel *)physicalModel;
          for (Integer indexer = 0; indexer < checker->GetNumForces(); indexer++)
          {
             if (checker->GetForce(indexer)->GetTypeName() == "FileThrust" && typeSource != "Starter for Predictor-Corrector")
             {
                hasFiniteBurn = true;
             }
          }
       }
       if (hasFiniteBurn)
       {
          stepLimited = true;
          stepSize = forceMaxStep;
       }
    }

    // Check to deal with FuelMass calculation Errors
    if (!finalStep && hasFiniteBurn && !stepLimited && fabs(fabs(stepSize) - fabs(forceMaxStep)) < physicalModel->GetStepPrecision(forceMaxStep) && fabs(forceMaxStep) < 1.7976931348623157e+308)
    {
       if (fabs(stepSize) < fabs(forceMaxStep))
       {
          stepSize = stepSize - (physicalModel->GetStepPrecision(forceMaxStep)*0.1);
          stepLimited = true;
       }
       else if (fabs(stepSize) > fabs(forceMaxStep))
       {
          stepSize = forceMaxStep - (physicalModel->GetStepPrecision(forceMaxStep)*0.1);;
          stepLimited = true;
       }
    }

    bool goodStepTaken = false;
    Real maxerror;

    do
    {
        if (!RawStep())
           return false;

        maxerror = EstimateError();
        stepTaken = stepSize;
        if (maxerror!= 0.0)
        {
           if (AdaptStep(maxerror))
              goodStepTaken = true;
        }
        else  // 0.0 means no error control; in that case leave step alone
        {
           memcpy(outState, candidateState, dimension*sizeof(Real));
           stepAttempts = 0;
           goodStepTaken = true;
        }

        if (stepAttempts >= maxStepAttempts)
        {
           MessageInterface::ShowMessage("%d step attempts taken; max is %d\n",
                 stepAttempts, maxStepAttempts);
           return false;
        }
    } while (!goodStepTaken);

    if (debug)
    {
       MessageInterface::ShowMessage("Propagator's step taken = %.15lf   ", stepTaken);
       for (UnsignedInt i = 0; i < dimension; ++i)
          MessageInterface::ShowMessage("%.12lf,   ", outState[i]);
       MessageInterface::ShowMessage("\n");
    }

    physicalModel->IncrementTime(stepTaken);

    if (stepLimited)
    {
       if (maxerror == 0.0)
       {
          // Issue a false return if FiniteBurn present. Further checks are done on the propagate/propagationenabledcommand level
          if (!finalStep && hasFiniteBurn)
          {
             if (followUpStep)
                burning = true; 
             forceMaxStep = physicalModel->GetForceMaxStep(stepSize > 0.0);

             bool endOfSegment = physicalModel->IsEndOfSegment(stepSize > 0.0);
             if (fabs(forceMaxStep) == GmatRealConstants::REAL_MAX || endOfSegment)
                burning = false;
             stepSize = originalStep;
             followUpStep = true;
             
             return false;
          }
          else
          {
             // Finish and restore fixed step if it's size is limited by a force
             bool ret = true;

             // If the step size was limited by a force, complete the remainder of the step
             followUpStep = true;
             stepSize = originalStep - stepSize;
             if (stepSize != 0.0)
                ret = Step();

             // Restore the original values and report the correct step taken
             stepSize = stepSizeBuffer;
             stepTaken = originalStep;

             // This is necessary to have the correct value of prevElapsedTime
             physicalModel->SetTime(originalTime);
             physicalModel->IncrementTime(stepTaken);

             followUpStep = false;
             return ret;
          }
       }
       else
       {
          // Restore originial variable step size if the forceMaxStep was used
          if (stepTaken == forceMaxStep)
            stepSize = originalStep;
          if (!finalStep && hasFiniteBurn)
             return false;
       }
    }

    return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::RawStep()
//------------------------------------------------------------------------------
/**
 * Take a raw prop step without any error control
 */
//------------------------------------------------------------------------------
bool RungeKutta::RawStep()
{
   #ifdef DEBUG_PROPAGATOR_FLOW
      MessageInterface::ShowMessage("*");
   #endif

   Integer i, j, k;

//   Real *inState = new Real[dimension];

   if (inState != physicalModel->GetState())
      memcpy(inState, physicalModel->GetState(), sizeof(Real) * dimension);

   #ifdef DEBUG_RAW_STEP_STATE
      MessageInterface::ShowMessage("inState: [");
      for (Integer q = 0; q < dimension-1; ++q)
        MessageInterface::ShowMessage("%le, ", inState[q]);
      MessageInterface::ShowMessage("%le]\n", inState[dimension-1]);
   #endif

   // Calculate the stages
   for (i = 0; i < stages; i++)
   {
      memcpy(stageState, inState, sizeof(Real) * dimension);

      #ifdef DEBUG_RAW_STEP_STATE
         MessageInterface::ShowMessage("inState[%d]: [", i);
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", inState[q]);
         MessageInterface::ShowMessage("%le]\n", inState[dimension-1]);
         MessageInterface::ShowMessage("stageState: [");
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", stageState[q]);
         MessageInterface::ShowMessage("%le]\n", stageState[dimension-1]);
      #endif

      // Built the accumulated state if this is not the initial step
      if (i > 0)
      {
         for (j = 0; j < i; j++)
         {
            for (k = 0; k < dimension; k++)
               stageState[k] += bij[i][j] * ki[j][k];
         }
      }

      Real direction = 0.0;
      if (stepSize > 0.0)
         direction = 1.0;
      else if (stepSize < 0.0)
         direction = -1.0;

      physicalModel->SetDirection(direction);

      if (!physicalModel->GetDerivatives(stageState, stepSize * ai[i]))
      {
         return false;
      }

      #ifdef DEBUG_RAW_STEP_STATE
         MessageInterface::ShowMessage("ddt data: [");
         for (Integer q = 0; q < dimension-1; ++q)
                MessageInterface::ShowMessage("%le, ", ddt[q]);
         MessageInterface::ShowMessage("%le]\n", ddt[dimension-1]);
         MessageInterface::ShowMessage("++ After GetDerivatives,  stageState: [");
         for (Integer q = 0; q < dimension-1; ++q)
            MessageInterface::ShowMessage("%le, ", stageState[q]);
         MessageInterface::ShowMessage("%le]\n", stageState[dimension-1]);
      #endif

      for (j = 0; j < dimension; j++)
         ki[i][j] = stepSize * ddt[j];
   }

   // Accumulate the stages for the RK step
   memcpy(candidateState, inState, dimension*sizeof(Real));
   for (i = 0; i < stages; i++)
   {
      for (j = 0; j < dimension; j++)
      {
         candidateState[j] += cj[i] * ki[i][j];
      }
   }

   #ifdef DEBUG_RAW_STEP_STATE
      MessageInterface::ShowMessage("candidateState: [");
      for (Integer q = 0; q < dimension-1; ++q)
         MessageInterface::ShowMessage("%le, ", candidateState[q]);
      MessageInterface::ShowMessage("%le]\n", candidateState[dimension-1]);
   #endif

//   delete [] inState;

   return true;
}

//------------------------------------------------------------------------------
// bool RungeKutta::Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Method used to step a fixed time
 *
 * @param dt    The time interval to step
 */
//------------------------------------------------------------------------------
bool RungeKutta::Step(Real dt)
{
   bool stepFinished = false;
   timeleft = dt;
   Integer attemptsTaken = 0;

   #ifdef DEBUG_STEPSIZE
       MessageInterface::ShowMessage("Time left: ");
   #endif

   do
   {
      #ifdef DEBUG_STEPSIZE
         MessageInterface::ShowMessage("%.12lf  ", timeleft);
      #endif

      if (attemptsTaken > maxStepAttempts)
      {
         MessageInterface::ShowMessage(
               "    Integrator attempted too many steps! (%d attempts "
               "taken)\n", attemptsTaken);
         return false;
      }
      if (!Propagator::Step(timeleft))
         return false;

      if (fabs(timeleft - stepTaken) <= smallestTime)
         stepFinished = true;

      timeleft -= stepTaken;
      ++attemptsTaken;
   } while (stepFinished == false);

   if (debug)
      MessageInterface::ShowMessage(" stepTaken %.15lf\n", stepTaken);

   #ifdef DEBUG_STEPSIZE
      MessageInterface::ShowMessage("Done!\n");
   #endif

   return true;
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// Real RungeKutta::EstimateError(void)
//------------------------------------------------------------------------------
/**
 * Provides the greatest relative error in the state vector
 *
 * This method takes the state vector and calculates the error in each
 * component.  The error is then divided by the change in the component.  The
 * function returns the largest of the resulting relative errors.
 *
 * Override this method if you want a different error estimate for the stepsize
 * control.  For example, we are using
 *
 * \f[error_i = \left({{\Delta_i(t+\delta t)}\over
 *                  {r_i(t+\delta t) - r_i(t)}}\right)\f]
 *
 * Another popular approach is to divide the estimated error \f$\Delta_i\f$ by
 * the norm of the corresponding 3-vector; for instance, divide the error in x
 * by the magnitude of the displacement in position for the step.
 */
//------------------------------------------------------------------------------
Real RungeKutta::EstimateError()
{
    Integer i, j;

    for (i = 0; i < dimension; i++)
    {
        errorEstimates[i] = 0.0;
        for (j = 0; j < stages; j++)
        {
            errorEstimates[i] += ee[j] * ki[j][i];
        }
    }

    // Find the maximum error
    return physicalModel->EstimateError(errorEstimates, candidateState);
}

//------------------------------------------------------------------------------
// bool RungeKutta::AdaptStep(Real maxerror)
//------------------------------------------------------------------------------
/**
 * Adjust the stepsize to maintain the accuracy and step constraints
 *
 *  Note that there is a kludge in this method to force the RK integrator to
 *  accept bad steps -- steps with too much error -- if the stepsize == the
 *  minimum value.
 *
 *  This is not the correct approach; that means the system can run without error
 *  control, and without reporting the bad steps.  It has been put in place to
 *  allow propagation across the SRP discontinuity experienced by a satellite
 *  crossing into or out of shadow when using a cylindrical shadow model.
 *
 *  This issue needs to be reexamined once the system is functional.
 *
 *  @param maxerror     Maximum error found in the current attempt
 *
 *  @return             true if the step is accepted, false if not
 */
//------------------------------------------------------------------------------
bool RungeKutta::AdaptStep(Real maxerror)
{
    // Adapt the step if necessary
    if (maxerror > tolerance)  // Too much error, so need to try again
    {

        // The following code is in place to omit error control for steps
        // at the minimum stepsize.  See the note above for more
        // information.  Remove this block if the issue gets resolved.
        if (GmatMathUtil::Abs(stepSize) == minimumStep)
        {
           if (stopIfAccuracyViolated)
           {
              throw PropagatorException(typeSource + ": Accuracy settings will "
                    "be violated with current step size values.\n");
           }
           else
           {
              if (!accuracyWarningTriggered) // so only write the warning once per propagation command
              {
                 accuracyWarningTriggered = true;
                 MessageInterface::ShowMessage("**** Warning **** %s: Accuracy "
                       "settings will be violated with current step size "
                       "values.\n", typeSource.c_str());
              }
               // Do this if the step was at the minimum stepSize
              memcpy(outState, candidateState, dimension*sizeof(Real));
              stepAttempts = 0;
              return true;
           }
        }

        stepSize = sigma * stepSize * pow(tolerance/maxerror, decPower);

        if (fabs(stepSize) < minimumStep)
            stepSize = ((stepSize < 0.0) ? -minimumStep : minimumStep);
        ++stepAttempts;

        // Flag this as an unacceptable step
        return false;
    }

    // Increase the stepsize, but keep this step
    stepSize = sigma * stepSize * pow(tolerance/maxerror, incPower);
    memcpy(outState, candidateState, dimension*sizeof(Real));
    stepAttempts = 0;

    // Identify the step as accepted
    return true;
}

//------------------------------------------------------------------------------
// void RungeKutta::ClearArrays(void)
//------------------------------------------------------------------------------
/**
 * Deallocates and NULLs the array data structures
 */
//------------------------------------------------------------------------------
void RungeKutta::ClearArrays()
{
    if (ki != NULL)
    {
        for (int i = 0; i < stages; i++)
        {
            if (ki[i] != NULL)
            {
                delete [] ki[i];
                ki[i] = NULL;
            }
        }
        delete [] ki;
    }

    if (ai != NULL)
        delete [] ai;

    if (bij != NULL)
    {
        for (int i = 0; i < stages; i++)
        {
            if (bij[i] != NULL)
                delete [] bij[i];
        }
        delete [] bij;
    }

    if (cj != NULL)
        delete [] cj;

    if (ee != NULL)
        delete [] ee;

    if (stageState != NULL)
        delete [] stageState;

    if (candidateState != NULL)
    {
        delete [] candidateState;
    }

    ki = bij = NULL;
    ai = cj = ee = stageState = candidateState = NULL;
    //    ai = cj = ee = stageState = candidateState = errorEstimates = NULL;
}

//------------------------------------------------------------------------------
// bool RungeKutta::SetupAccumulator()
//------------------------------------------------------------------------------
/**
 * Sets memory for the stageState and the members of the ki array
 */
//------------------------------------------------------------------------------
bool RungeKutta::SetupAccumulator()
{
    if (physicalModel)
    {
        isInitialized = true;
        dimension = physicalModel->GetDimension();

        if (stageState)
            delete [] stageState;

        if ((stageState = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            return false;
        }

        if (candidateState)
            delete [] candidateState;

        if ((candidateState = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            delete [] stageState;
            stageState = NULL;
            return false;
        }

        ddt = physicalModel->GetDerivativeArray();

        if (errorEstimates)
            delete [] errorEstimates;

        if ((errorEstimates = new Real[dimension]) == NULL)
        {
            isInitialized = false;
            delete [] stageState;
            stageState = NULL;
            delete [] candidateState;
            candidateState = NULL;
            return false;
        }

        if (ki)
        {
            for (int i = 0; i < stages; i++)
            {
                if (ki[i])
                    delete [] ki[i];
                if ((ki[i] = new Real[dimension]) == NULL)
                {
                    isInitialized = false;
                    delete [] stageState;
                    stageState = NULL;
                    delete [] candidateState;
                    candidateState = NULL;
                    delete [] errorEstimates;
                    errorEstimates = NULL;
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}
