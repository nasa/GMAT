//$Id$
//------------------------------------------------------------------------------
//                        AdamsBashforthMoulton
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
// *** File Name : AdamsBashforthMoulton.cpp
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
//                           : 6/17/2004 - A. Greene, Mission Applications, GSFC
//                             - Updated style using GMAT cpp style guide
//                             - All double types to Real types, int to Integer
// *****************************************************************************


#include "AdamsBashforthMoulton.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATION


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// AdamsBashforthMoulton(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor for the ABM integrator
 *
 * @param nomme The name of the new ABM propagator
 */
//------------------------------------------------------------------------------
//AdamsBashforthMoulton::AdamsBashforthMoulton() :
//    PredictorCorrector      (4, 4),
AdamsBashforthMoulton::AdamsBashforthMoulton(const std::string &nomme) :
   PredictorCorrector      (4, 4, "AdamsBashforthMoulton", nomme),
   eeFactor                (19.0/270.0)
{
   starter = new RungeKutta89;
}

//------------------------------------------------------------------------------
// ~AdamsBashforthMoulton()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
AdamsBashforthMoulton::~AdamsBashforthMoulton()
{
}

//------------------------------------------------------------------------------
// AdamsBashforthMoulton(const AdamsBashforthMoulton& abm)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 *
 * @param abm The propagator that supplies data for this one
 */
//------------------------------------------------------------------------------
AdamsBashforthMoulton::AdamsBashforthMoulton(const AdamsBashforthMoulton& abm) :
   PredictorCorrector      (abm),
   eeFactor                (abm.eeFactor)
{
}

//------------------------------------------------------------------------------
// AdamsBashforthMoulton operator=(const AdamsBashforthMoulton& abm)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param abm The propagator that supplies data for this one
 */
//------------------------------------------------------------------------------
AdamsBashforthMoulton AdamsBashforthMoulton::operator=(
      const AdamsBashforthMoulton& abm)
{
   if (this == &abm)
      return *this;

   PredictorCorrector::operator=(abm);
   eeFactor = abm.eeFactor;

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 *
 * @return A clone of this instance
 */
//------------------------------------------------------------------------------
GmatBase* AdamsBashforthMoulton::Clone() const
{
   return new AdamsBashforthMoulton(*this);
}
  
//------------------------------------------------------------------------------
// bool SetWeights()
//------------------------------------------------------------------------------
/**
 * Method used to set the ABM weights
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AdamsBashforthMoulton::SetWeights()
{
   if ((pweights == NULL) || (cweights == NULL))
      return false;

   pweights[3] =  55.0 / 24.0;
   pweights[2] = -59.0 / 24.0;
   pweights[1] =  37.0 / 24.0;
   pweights[0] = - 9.0 / 24.0;

   cweights[3] =   9.0 / 24.0;
   cweights[2] =  19.0 / 24.0;
   cweights[1] = - 5.0 / 24.0;
   cweights[0] =   1.0 / 24.0;

   return true;
}

//------------------------------------------------------------------------------
// bool FireStartupStep()
//------------------------------------------------------------------------------
/**
 * Method used to start the integrator by filling initial states
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AdamsBashforthMoulton::FireStartupStep()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   Startup Step size %lf\n", stepSize);
   #endif

   if (starter == NULL)
      return false;

   bool retval = starter->Step(stepSize);
   SetHasFiniteBurn(starter->GetHasFiniteBurn());
   SetFollowUpStep(starter->GetFollowUpStep());
   if (retval)
   {
      #ifdef DEBUG_PROPAGATION
         MessageInterface::ShowMessage("Startup %d -> ", startupCount);
      #endif
      ++startupCount;
      #ifdef DEBUG_PROPAGATION
         MessageInterface::ShowMessage("%d ", startupCount);
      #endif
      if (startupCount == 3) {
         startupComplete = true;
      }
      stepTaken = stepSize;
   }

   #ifdef DEBUG_PROPAGATION
      else
         MessageInterface::ShowMessage("Bad startup step ");
   #endif

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   Startup state %d: [", startupCount);
      for (Integer j = 0; j < dimension-1; ++j)
         MessageInterface::ShowMessage("%lf ", outState[j]);
      MessageInterface::ShowMessage("%lf]\n", outState[dimension-1]);
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// bool Predict()
//------------------------------------------------------------------------------
/**
 * Method used to fire the step extrapolation (the predictor phase)
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AdamsBashforthMoulton::Predict()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Predictor phase fired\n");
   #endif
   int i, j;

   physicalModel->GetDerivatives(inState);
   // Store step to be removed in the buffer
   for (i = 0; i < dimension; i++)
   {
      historyBuffer[i] = history[0][i];
   }

   for (i = 0; i < stepCount - 1; i++)
      memcpy(history[i], history[i + 1], dimension * sizeof(double));
   memcpy(history[stepCount - 1], ddt, dimension * sizeof(double));


   for (j = 0; j < dimension; j++)
   {
      predictorState[j] = inState[j];
      for (i = 0; i < stepCount; i++)
      {
         predictorState[j] += stepSize * pweights[i] * history[i][j];
      }
   }

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   History buffer:\n");
      for (Integer i = 0; i < stepCount; ++i)
      {
         MessageInterface::ShowMessage("      [%d] = [", i);
         for (Integer j = 0; j < dimension-1; ++j)
            MessageInterface::ShowMessage(" %.8le ", history[i][j]);
         MessageInterface::ShowMessage(" %.8le]\n", history[i][dimension-1]);
      }

      MessageInterface::ShowMessage("   Predictor state: [");
      for (j = 0; j < dimension-1; ++j)
         MessageInterface::ShowMessage("%le ", predictorState[j]);
      MessageInterface::ShowMessage("%le]\n", predictorState[dimension-1]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
// bool Correct()
//------------------------------------------------------------------------------
/**
 * Method used to fire the step refinement (the corrector phase)
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AdamsBashforthMoulton::Correct()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Corrector phase fired\n");
   #endif

   int i, j;

   physicalModel->GetDerivatives(predictorState, stepSize);

   for (j = 0; j < dimension; j++)
   {
      correctorState[j] = inState[j] + stepSize * cweights[stepCount-1] * ddt[j];
      for (i = 1; i < stepCount; i++)
      {
         correctorState[j] += stepSize * cweights[i-1] * history[i][j];
      }
   }

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   Corrector state: [");
      for (j = 0; j < dimension-1; ++j)
         MessageInterface::ShowMessage("%lf ", predictorState[j]);
      MessageInterface::ShowMessage("%lf]\n", predictorState[dimension-1]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
// Real EstimateError()
//------------------------------------------------------------------------------
/**
 * Method used to determine the error in the step
 *
 * The error estimate for the Adams-Bashforth-Moulton integrator is calculated
 * by finding the difference between the corrector and predictor states, and
 * multiplying this difference by the difference in the truncated Taylor series 
 * used to derive the algorithm.  For this 4th order implementation, the error 
 * in the \f$i^{th}\f$ component is given by
 *
 * \f[EE_i = {{19}\over{270}}\left|r_{i}^{(C)}\left(t+\delta t\right) - 
 *                          r_{i}^{(P)}\left(t+\delta t\right)\right|\f]
 *
 * The resulting error components are passed to PhysicalModel->EstimateError()
 * and processed to determine the maximum error in the system.
 *
 * @return The maximum error found
 */
//------------------------------------------------------------------------------
Real AdamsBashforthMoulton::EstimateError()
{
   int i;

   for (i = 0; i < dimension; i++)
      errorEstimates[i] = fabs(eeFactor*(correctorState[i]-predictorState[i]));
    
   maxError = physicalModel->EstimateError(errorEstimates, correctorState);

   return maxError;
}

//------------------------------------------------------------------------------
// bool Reset()
//------------------------------------------------------------------------------
/**
 * Method used to restart the integrator, when appropriate
 *
 * This method is called when the ABM integrator changes the stepsize or when 
 * the state data in the PhysicalModel is changed discontinuously, so that the
 * internal buffers can be filled with the data for the new stepsize.
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool AdamsBashforthMoulton::Reset()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("ABM Resetting\n");
   #endif

   startupCount = 0;
   startupComplete = false;
   ddt = NULL;

   return true;
}
