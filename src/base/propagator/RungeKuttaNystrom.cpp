//$Id$
//------------------------------------------------------------------------------
//                              RungeKuttaNystrom
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
// *** File Name : rungekuttanystrom.cpp
// *** Created   : January 21, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
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
// Modification History      : 1/21/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                                    Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************

#include "gmatdefs.hpp"
#include "RungeKuttaNystrom.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INITIALIZATION

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// RungeKuttaNystrom::RungeKuttaNystrom()
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
//RungeKuttaNystrom::RungeKuttaNystrom() :
//RungeKutta (16, 9, NULL, NULL, NULL)
//{
//}

//------------------------------------------------------------------------------
// RungeKuttaNystrom(Integer st, Integer order, const std::string &typeStr,
//                   const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * The constructor
 *
 * @param st Number of stages in the specific algorithm implemented
 * @param order Order of the expansion used for the integrator
 * @param typeStr The integrator type
 * @param nomme The name of the object being created
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom::RungeKuttaNystrom(Integer st, Integer order,
      const std::string &typeStr, const std::string &nomme) :
    RungeKutta          (st, order, typeStr, nomme),
    cdotj               (NULL),
    derivativeMap       (NULL),
    inverseMap          (NULL),
    derivativeError     (false),
    eeDeriv             (NULL)
{
   derivativeOrder = 2;       // Use second derivatives here
}

//------------------------------------------------------------------------------
// ~RungeKuttaNystrom()
//------------------------------------------------------------------------------
/**
 * The destructor
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom::~RungeKuttaNystrom()
{
    if (cdotj)
        delete [] cdotj;
    if (derivativeMap)
        delete [] derivativeMap;
    if (inverseMap)
        delete [] inverseMap;
    if (eeDeriv)
        delete [] eeDeriv;
}

//------------------------------------------------------------------------------
// RungeKuttaNystrom(const RungeKuttaNystrom& rk)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 *
 * @param rk The integrator being copied
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom::RungeKuttaNystrom(const RungeKuttaNystrom& rk) :
    RungeKutta          (rk),
    cdotj               (NULL),
    derivativeMap       (NULL),
    inverseMap          (NULL),
    derivativeError     (rk.derivativeError),
    eeDeriv             (NULL)
{
   isInitialized = false;
}

//------------------------------------------------------------------------------
// RungeKuttaNystrom & operator=(const RungeKuttaNystrom& rk)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param rk The integrator being copied
 */
//------------------------------------------------------------------------------
RungeKuttaNystrom & RungeKuttaNystrom::operator=(const RungeKuttaNystrom& rk)
{
    if (this == &rk)
        return *this;

    RungeKutta::operator=(rk);
    isInitialized = false;
    derivativeError = rk.derivativeError;

    return *this;
}

//------------------------------------------------------------------------------
// bool RungeKuttaNystrom::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialized the Nystrom elements of the integrator & calls the RK initializer
 *
 * This method allocates the data arrays for the Runge-Kutta-Nystrom integrators
 * by calling the Runge-Kutta initialization method (RungeKutta::Initialize()) 
 * and then initializing the Nystrom specific data.
 */
//------------------------------------------------------------------------------
bool RungeKuttaNystrom::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Entered RungeKuttaNystrom::"
            "Initialize()\n");
   #endif

   if (RungeKutta::Initialize())
   {
      isInitialized = false;

      if (physicalModel == NULL)
         throw PropagatorException("PhysicalModel was not set");

      if (physicalModel)
         dimension = physicalModel->GetDimension();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Dimension: %d\n", dimension);
      #endif

      if (cdotj) {
         delete [] cdotj;
         cdotj = NULL;
      }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Stages\n");
      #endif
      // Set the Nystrom-specific structures
      if ((cdotj = new double [stages]) == NULL) {
         RungeKutta::ClearArrays();
         throw PropagatorException("Could not allocate cdotj");
      }

      if (derivativeMap) {
         delete [] derivativeMap;
         derivativeMap = NULL;
      }

      if (inverseMap) {
         delete [] inverseMap;
         inverseMap = NULL;
      }

      if (eeDeriv) {
         delete [] eeDeriv;
         eeDeriv = NULL;
      }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Derivative map\n");
      #endif
      if ((derivativeMap = new int [dimension]) == NULL) {
         RungeKutta::ClearArrays();
         delete [] cdotj;
         cdotj = NULL;
         throw PropagatorException("Could not allocate derivativeMap");
      }

      if ((inverseMap = new int [dimension]) == NULL) {
         RungeKutta::ClearArrays();
         delete [] cdotj;
         cdotj = NULL;
         delete [] derivativeMap;
         derivativeMap = NULL;
         throw PropagatorException("Could not allocate inverseMap");
      }

      // Init the derivative and inverse map
      for (int i = 0; i < dimension; ++i)
      {
         derivativeMap[i] = -1;
         inverseMap[i] = -1;
      }

      if (!physicalModel->GetComponentMap(derivativeMap)) {
         RungeKutta::ClearArrays();
         delete [] cdotj;
         cdotj = NULL;
         delete [] derivativeMap;
         derivativeMap = NULL;
         throw PropagatorException("Could not set the component map");
      }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Derivative error = %s\n",
               (derivativeError ? "true" : "false"));
      #endif

      if (derivativeError) {
         if ((eeDeriv = new double[stages]) == NULL) {
            RungeKutta::ClearArrays();
            delete [] cdotj;
            cdotj = NULL;
            delete [] derivativeMap;
            derivativeMap = NULL;
            delete [] inverseMap;
            inverseMap = NULL;
            throw PropagatorException("Encountered derivative error");
         }
      }

      // Fill the inverse map from the derivative map
      for (int i = 0; i < dimension; ++i)
         if (derivativeMap[i] != -1)
             inverseMap[derivativeMap[i]] = i;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Setting coefficients\n");
      #endif
      isInitialized = true;      // Flag that allows coefficient filling
      SetCoefficients();
      isInitialized = SetupAccumulator();  // Final init check and setup
   }
   else
      throw PropagatorException("RungeKutta base did not initialize for the "
            "RKN class");

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Completed RungeKuttaNystrom::Initialize();"
            " returning %s\n", (isInitialized ? "true" : "false"));
   #endif
   return isInitialized;
}


//------------------------------------------------------------------------------
// bool Step()
//------------------------------------------------------------------------------
/**
 * Evolves the state vector by one step
 *
 * @return true if a successful step was taken, false if not
 */
//------------------------------------------------------------------------------
bool RungeKuttaNystrom::Step()
{
   if (!isInitialized) {
      Initialize();
      if (!isInitialized)
         throw PropagatorException("Cannot Step: RKN is not initialized");
   }
    
   if (!finalStep)
      if (fabs(stepSize) < minimumStep)
         stepSize = ((stepSize > 0.0) ? minimumStep : -minimumStep);
    
   if (fabs(stepSize) > maximumStep)
      stepSize = ((stepSize > 0.0) ? maximumStep : -maximumStep);
        
   bool goodStepTaken = false;
   double maxerror;
    
   do {
      if (!RawStep()) {
         throw PropagatorException("RKN::RawStep() failed");
         return false;
      }

      maxerror = EstimateError();
      stepTaken = stepSize;
      if (AdaptStep(maxerror))
         goodStepTaken = true;

      if (stepAttempts >= maxStepAttempts) {
         throw PropagatorException("Too many step attempts in RKN Propagator");
         return false;
      }
   } while (!goodStepTaken);

   physicalModel->IncrementTime(stepTaken);
   return true;
}


//------------------------------------------------------------------------------
// bool Step(double dt)
//------------------------------------------------------------------------------
/**
 * Evolves the state vector by a specified step
 *
 * @param dt The step desired
 *
 * @return true if the step was taken, false if not
 */
//------------------------------------------------------------------------------
bool RungeKuttaNystrom::Step(double dt)
{
   bool stepFinished = false;
   timeleft = dt;
   do {
      if (!Propagator::Step(timeleft)) {
         throw PropagatorException("Propagator::Step(timeleft) failed in RKN");
         return false;
      }
      if (timeleft == stepTaken)
         stepFinished = true;
      timeleft -= stepTaken;
   } while (stepFinished == false);

   return true;
}


//------------------------------------------------------------------------------
// bool RungeKuttaNystrom::RawStep()
//------------------------------------------------------------------------------
/**
 * Applies the Nystrom algorithm to evolve the state vector
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RungeKuttaNystrom::RawStep()
{
    int i, j, k;
    double accum, h2 = stepSize * stepSize;

    // Calculate the stages
    for (i = 0; i < stages; i++) {
        // Built the accumulated state if this is not the initial step
        if (i > 0) {
            for (k = 0; k < dimension; ++k) {
                // Only do the dependents, not their derivatives
                if (derivativeMap[k] >= 0) {
                    accum = 0.0;
                    for (j = 0; j < i; ++j) {
                        accum += bij[i][j] * ki[j][k];
                    }
                    stageState[k] = inState[k] + ai[i] * stepSize * 
                        inState[derivativeMap[k]] + h2 * accum;
                }
            }
        }
        else {
            memcpy(stageState, inState, dimension * sizeof(double));
        }

        if (!physicalModel->GetDerivatives(stageState, stepSize * ai[i], 2)) {
            throw PropagatorException("Call to GetDerivatives() Failed for RKN Propagator");
            return false;
        }
        for (j = 0; j < dimension; j++)
            // Only the dependents...
            if (derivativeMap[j] >= 0)
                ki[i][j] = ddt[j];
            else {
                if (derivativeError) {
                    if (inverseMap[j] >= 0)
                        ki[i][j] = ddt[inverseMap[j]];
                    else
                        ki[i][j] = 0.0; 
                }
                else
                    ki[i][j] = 0.0; 
            }
    }

    // Accumulate the stages for the RK step
    memcpy(candidateState, inState, dimension*sizeof(double));
    for (i = 0; i < dimension; i++) {
        if (derivativeMap[i] >= 0) {
            candidateState[i] += stepSize * inState[derivativeMap[i]];
            for (j = 0; j < stages; j++) {
                if (cj[j] != 0.0)
                    candidateState[i] += h2 * cj[j] * ki[j][i];
            }
        }
        else {
            for (j = 0; j < stages; j++) {
                if (cdotj[j] != 0.0)
                    candidateState[i] += stepSize * cdotj[j] * ki[j][inverseMap[i]];
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Integer GetPropagatorOrder() const
//------------------------------------------------------------------------------
/**
 * Identifies the derivative order needed by this integrator
 *
 * @return 2, since Nystrom integrators are second order integrators.
 */
//------------------------------------------------------------------------------
Integer RungeKuttaNystrom::GetPropagatorOrder() const
{
    return 2;
}


//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void RungeKuttaNystrom::EstimateError()
//------------------------------------------------------------------------------
/**
 * Provides the element by element error estimate for the evolved state vector
 *
 * This method takes the state vector and calculates the error in each 
 * component.  The error is then divided by the change in the component.  The
 * function returns the largest of the resulting relative errors.

 * The Nystrom implementation of this code is similar to the implementation for
 * the first order Runge-Kutta, except that the derivative terms use special
 * coefficients when error estimates are available for those terms.
 */
//------------------------------------------------------------------------------
Real RungeKuttaNystrom::EstimateError()
{
    int i, j;

    double h2 = stepSize * stepSize;

    for (i = 0; i < dimension; i++) {
        errorEstimates[i] = 0.0;
        for (j = 0; j < stages; j++) {
            if (derivativeMap[i] >= 0) {
                errorEstimates[i] += h2 * ee[j] * ki[j][i];
            }
            else if (derivativeError && (inverseMap[i] >= 0)) {
                errorEstimates[i] += stepSize * eeDeriv[j] * ki[j][i];
            }
        }
    }

    // Find the maximum error
    return physicalModel->EstimateError(errorEstimates, candidateState);
}
