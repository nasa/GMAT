//------------------------------------------------------------------------------
//                              DummyPathFunction
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
// Created: 2015.08.15
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "DummyPathFunction.hpp"
#include "GmatConstants.hpp"
#include "Rvector.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_DUMMY_PATH_FUNCTION

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
DummyPathFunction::DummyPathFunction() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
DummyPathFunction::DummyPathFunction(const DummyPathFunction &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
DummyPathFunction& DummyPathFunction::operator=(const DummyPathFunction &copy)
{
   
   if (&copy == this)
      return *this;

   UserPathFunction::operator=(copy);

   return *this;

}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
DummyPathFunction::~DummyPathFunction()
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
void DummyPathFunction::EvaluateFunctions()
{
   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      MessageInterface::ShowMessage(
            "In DummyPathFunction::EvaluateFunctions  ... \n");
      MessageInterface::ShowMessage("   paramData = <%p>, pfContainer = <%p>\n",
            paramData, pfContainer);
   #endif
   // Extract paramater data
   Rvector y = GetStateVector();
   Rvector u = GetControlVector();
   
   Real time = GetTime();

   Integer phaseNum = GetPhaseNumber();

   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      MessageInterface::ShowMessage("--- state vector   = %s  ... \n",
                                    y.ToString(12).c_str());
      MessageInterface::ShowMessage("--- control vector = %s  ... \n",
                                    u.ToString(12).c_str());
      MessageInterface::ShowMessage("--- time           = %12.10f\n", time);
      MessageInterface::ShowMessage("--- phaseNum       = %d\n", phaseNum);
   #endif
   
   if (phaseNum == 0)   // was 1 in matlab
   {
      // Test functions used for dynamics and algebraic functions
      Rvector f(2);
      f[0] = -(y[0] * y[0]) * y[1] * (y[2] * y[2] * y[2]) +
      (-(u[0] * u[0]) * (u[1] * u[1] * u[1]) ) - (time * time * time);
      f[1] = -(y[2] * y[2]) * y[1] * (y[0] * y[0] * y[0]) +
      (-(u[1] * u[1]) * (u[0] * u[0] * u[0]) ) + (time * time);
      
      Rvector cost(1);
      cost[0] = y(0)*y(1)*y(2)*u(0)*u(1)*time;

      #ifdef DEBUG_DUMMY_PATH_FUNCTION
         Rvector algTmp = GmatMathConstants::PI * f;
         MessageInterface::ShowMessage(
               "   Setting Alg Functions  ...\n     %s\n",
               algTmp.ToString(12).c_str());
      #endif
      // Evaluate Algebraic Function
      SetFunctions(ALGEBRAIC, GmatMathConstants::PI * f);
      
      #ifdef DEBUG_DUMMY_PATH_FUNCTION
         MessageInterface::ShowMessage(
               "   Setting Cost Functions  ...\n     %s\n",
               cost.ToString(12).c_str());
      #endif
      // Evaluate Cost Function
      SetFunctions(COST, cost);
      
      #ifdef DEBUG_DUMMY_PATH_FUNCTION
         MessageInterface::ShowMessage(
               "   Setting Dyn Functions  ...\n     %s\n",
               f.ToString(12).c_str());
      #endif
      // Evaluate dynamics and compute Jacobians
      SetFunctions(DYNAMICS, f);

      #ifdef DEBUG_DUMMY_PATH_FUNCTION
         MessageInterface::ShowMessage(
               "   DONE Setting Functions  ... \n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void EvaluateJacobians()
//------------------------------------------------------------------------------
/**
 * This method valuates the jacobians
 *
 */
//------------------------------------------------------------------------------
void DummyPathFunction::EvaluateJacobians()
{
   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      MessageInterface::ShowMessage(
            "Entering DummyPathFunction::EvaluateJacobians  ... \n");
      MessageInterface::ShowMessage("   paramData = <%p>, pfContainer = <%p>\n",
            paramData, pfContainer);
   #endif
   // Extract paramater data
   Rvector y = GetStateVector();    // assume size 3
   Rvector u = GetControlVector();  // assume size 2
   
   Real time = GetTime();  // is this in the correct units for what we need?
   bool useAnalyticPartials = false;
   
   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      MessageInterface::ShowMessage("--- state vector        = %s  ... \n",
                                    y.ToString(12).c_str());
      MessageInterface::ShowMessage("--- control vector      = %s  ... \n",
                                    u.ToString(12).c_str());
      MessageInterface::ShowMessage("--- time                = %12.10f\n",
                                    time);
      MessageInterface::ShowMessage("--- useAnalyticPartials = %s\n",
                                    (useAnalyticPartials? "true" : "false"));
   #endif
   Rmatrix fStateJacobian(2,3);
   
   fStateJacobian.SetElement(0,0, -2*y(0)*y(1)*(y(2)*y(2)*y(2)));
   fStateJacobian.SetElement(0,1, -(y(0)*y(0))*(y(2)*y(2)*y(2)));
   fStateJacobian.SetElement(0,2, -3*(y(0)*y(0))*y(1)*(y(2)*y(2)));
   fStateJacobian.SetElement(1,0, -3*(y(2)*y(2))*y(1)*(y(0)*y(0)));
   fStateJacobian.SetElement(1,1, -(y(2)*y(2))*(y(0)*y(0)*y(0)));
   fStateJacobian.SetElement(1,2, -2*y(2)*y(1)*(y(0)*y(0)*y(0)));
   
   Rmatrix fControlJacobian(2,2);
   fControlJacobian.SetElement(0,0, -2*u(0)*(u(1)*u(1)*u(1)));
   fControlJacobian.SetElement(0,1, -3*(u(0)*u(0))*(u(1)*u(1)));
   fControlJacobian.SetElement(1,0, -3*(u(1)*u(1))*(u(0)*u(0)));
   fControlJacobian.SetElement(1,1, -2*u(1)*(u(0)*u(0)*u(0)));

   Rmatrix fTimeJacobian(1,2);
   fTimeJacobian.SetElement(0,0,-3*time*time);
   fTimeJacobian.SetElement(0,1,2*time);
   
   Rmatrix cStateJacobian(1,3);
   cStateJacobian.SetElement(0,0,y(1)*y(2)*u(0)*u(1)*time);
   cStateJacobian.SetElement(0,1,y(0)*y(2)*u(0)*u(1)*time);
   cStateJacobian.SetElement(0,2,y(0)*y(1)*u(0)*u(1)*time);
   
   Rmatrix cControlJacobian(1,2);
   cControlJacobian.SetElement(0,0,y(0)*y(1)*y(2)*u(1)*time);
   cControlJacobian.SetElement(0,1,y(0)*y(1)*y(2)*u(0)*time);
   
   Rmatrix cTimeJacobian(1,1,y(0)*y(1)*y(2)*u(0)*u(1));
   
   if (useAnalyticPartials)
   {
      #ifdef DEBUG_DUMMY_PATH_FUNCTION
         MessageInterface::ShowMessage(
                           "EvaluateJacobians:: About to set functions\n");
      #endif
      // Set Algebraic
      SetJacobian(ALGEBRAIC, STATE, GmatMathConstants::PI*fStateJacobian);
      SetJacobian(ALGEBRAIC, CONTROL, GmatMathConstants::PI*fControlJacobian);
      SetJacobian(ALGEBRAIC, TIME, GmatMathConstants::PI*fTimeJacobian);

      // Set Cost
      SetJacobian(COST, STATE, cStateJacobian);
      SetJacobian(COST, CONTROL, cControlJacobian);
      SetJacobian(COST, TIME, cTimeJacobian);

      // Set dynamics
      SetJacobian(DYNAMICS, STATE, fStateJacobian);
      SetJacobian(DYNAMICS, CONTROL, fControlJacobian);
      SetJacobian(DYNAMICS, TIME, fTimeJacobian);
   }
   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      else
      {
      MessageInterface::ShowMessage(
            "Jacobians NOT SET - useAnalyticPartials is false!!! \n");
      }
   #endif
   #ifdef DEBUG_DUMMY_PATH_FUNCTION
      MessageInterface::ShowMessage(
            "EXITING DummyPathFunction::EvaluateJacobians  ... \n");
   #endif
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
