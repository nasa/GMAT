//------------------------------------------------------------------------------
//                              DummyPathFunction2
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
// Author: Wendy Shoan / NASA
// Created: 2015.11.02
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "DummyPathFunction2.hpp"
#include "GmatConstants.hpp"
#include "Rvector.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
DummyPathFunction2::DummyPathFunction2() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
DummyPathFunction2::DummyPathFunction2(const DummyPathFunction2 &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
DummyPathFunction2& DummyPathFunction2::operator=(const DummyPathFunction2 &copy)
{
   
   if (&copy == this)
      return *this;

   UserPathFunction::operator=(copy);
   
   return *this;

}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
DummyPathFunction2::~DummyPathFunction2()
{
   // nothing to do here
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//------------------------------------------------------------------------------
/**
 * This method valuates the functions
 *
 */
//------------------------------------------------------------------------------
void DummyPathFunction2::EvaluateFunctions()
{
   // Extract paramater data
   Rvector y = GetStateVector();    // Assume size 3
   Rvector u = GetControlVector();  // Assume size 2
   
   Real time = GetTime();  // is this in the correct units for what we need?
   Integer phaseNum = GetPhaseNumber();
   
   if (phaseNum == 1)   // was 2 in matlab
   {
      // Test functions used for dynamics and algebraic functions
      Rvector f(2);
      f[0] = -(y[0]*y[0]) * y[2] + (u[0]*u[0]) * (u[1]*u[1]*u[1]);
      f[1] =  y[1] * (y[0]*y[0]*y[0]) + (u[1]*u[1]) * time*time;
      
      Rvector cost(1);
      cost[0] = y(0)*y(2)*u(0)*time;
      
      Rvector f2(2);
      f2[0] = -(y[1]*y[1]) * y[2] + (u[1]*u[1]) * (u[1]*u[1]*u[1]) +
              time * time;
      f2[1] = y[2] * (y[0]*y[0]*y[0]) + (u[0]*u[0]);


      // Evaluate Algebraic Function
      SetFunctions(ALGEBRAIC, f2);  // GmatMathConstants::PI * ??
      
      // Evaluate Cost Function
      SetFunctions(COST, cost);
      
      // Evaluate dynamics and compute Jacobians
      SetFunctions(DYNAMICS, f);
   }
}


//------------------------------------------------------------------------------
// void EvaluateJacobians()
//------------------------------------------------------------------------------
/**
 * This method valuates the functions
 *
 */
//------------------------------------------------------------------------------
void DummyPathFunction2::EvaluateJacobians()
{
   // Extract paramater data
   Rvector y = GetStateVector();    // assume size 3
   Rvector u = GetControlVector();  // assume size 2
   
   Real time = GetTime();  // is this in the correct units for what we need?

   // where should this actually be set?  not here
   bool useAnalyticJacobians = true;
   
   
   if (useAnalyticJacobians)
   {
      Rmatrix dsj(2,3, -2*y(0)*y(2),      0.0,           -y(0)*y(0),
                        3*y(1)*y(0)*y(0), y(0)*y(0)*y(0), 0.0);
      SetJacobian(DYNAMICS, STATE, dsj);
      
      Rmatrix csj(1,3, y(2)*u(0)*time, 0.0, y(0)*u(0)*time);
      SetJacobian(COST, STATE, csj);
      
      Rmatrix asj(2,3, 0.0 ,            -2*y(1)*y(2), -y(1)*y(1),
                       3*y(2)*y(0)*y(0), 0.0 ,         y(0)*y(0)*y(0));
      SetJacobian(ALGEBRAIC, STATE, asj);
   }
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
