//$Id$
//------------------------------------------------------------------------------
//                           RungeKutta4
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 26
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Dec 15, 2015
/**
 * A basic Runge-Kutta 4 integrator with no step size control
 */
//------------------------------------------------------------------------------

#include "../../../plugins/EstimationPlugin/src/base/propagator/RungeKutta4.hpp"

#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// RungeKutta4(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the propoagator
 */
//------------------------------------------------------------------------------
RungeKutta4::RungeKutta4(const std::string &name) :
   RungeKutta      (4, 4, "RungeKutta4", name)
{
   hasErrorControl = false;
}

//------------------------------------------------------------------------------
// ~RungeKutta4()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RungeKutta4::~RungeKutta4()
{
}

//------------------------------------------------------------------------------
// RungeKutta4(const RungeKutta4& rk)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param The integrator copied to this one
 */
//------------------------------------------------------------------------------
RungeKutta4::RungeKutta4(const RungeKutta4& rk) :
   RungeKutta        (rk)
{
   hasErrorControl = rk.hasErrorControl;
}

//------------------------------------------------------------------------------
// RungeKutta4& operator =(const RungeKutta4& rk)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param The integrator passing data to thnis one
 *
 * @return This integrator, reconfigured to match rk
 */
//------------------------------------------------------------------------------
RungeKutta4& RungeKutta4::operator =(const RungeKutta4& rk)
{
   if (this != &rk)
   {
      RungeKutta::operator =(rk);
   }
   hasErrorControl = rk.hasErrorControl;
   return *this;
}

//------------------------------------------------------------------------------
// Propagator* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to make a type-specific copy of the integrator
 *
 * @return A new integrator matching this one
 */
//------------------------------------------------------------------------------
Propagator* RungeKutta4::Clone() const
{
   return new RungeKutta4(*this);
}


//------------------------------------------------------------------------------
// Real EstimateError()
//------------------------------------------------------------------------------
/**
 * Override for the error estimator.
 *
 * The RK4 integrator does not perform error control.  Since the Propagate
 * command monitors error estimates, a return value is needed that prevents
 * attempts at step size control.  In order to accomplish this, the RK4 claims
 * to always step at exactly the tolerance in the propagator settings.
 *
 * @return The step tolerance, always, for this integrator
 */
//------------------------------------------------------------------------------
Real RungeKutta4::EstimateError()
{
   return tolerance;
}

//------------------------------------------------------------------------------
// bool AdaptStep(Real maxerror)
//------------------------------------------------------------------------------
/**
 * Applies the propagation step
 *
 * The RK4 propagator just accepts the step taken in the call to propagate, and
 * never adjusts the step size.
 *
 * @param maxerror Maximum error detected in the step.  For the RK4 integrator,
 *        this value is always the scripted tolerance, and it has no effect.
 *
 * @return true, always
 */
//------------------------------------------------------------------------------
bool RungeKutta4::AdaptStep(Real maxerror)
{
   memcpy(outState, candidateState, dimension*sizeof(Real));
   stepAttempts = 0;
   return true;
}

//------------------------------------------------------------------------------
// void RungeKutta4::SetCoefficients()
//------------------------------------------------------------------------------
/**
 * Sets up the RK4 coefficients used in propagation
 */
//------------------------------------------------------------------------------
void RungeKutta4::SetCoefficients()
{
   if ((ai == NULL) || (bij == NULL) || (cj == NULL) || (ee == NULL))
   {
      isInitialized = false;
      return;
   }

   ai[0] = 0.0;
   ai[1] = 0.5;
   ai[2] = 0.5;
   ai[3] = 1.0;

   bij[0][0] = 0.0;

   bij[1][0] = 0.5;

   bij[2][0] = 0.0;
   bij[2][1] = 0.5;

   bij[3][0] = 0.0;
   bij[3][1] = 0.0;
   bij[3][2] = 1.0;

   cj[0] = 1.0 / 6.0;
   cj[1] = 1.0 / 3.0;
   cj[2] = 1.0 / 3.0;
   cj[3] = 1.0 / 6.0;

   for (int i = 0; i < stages; i++)
      ee[i] = 0.0;
}
