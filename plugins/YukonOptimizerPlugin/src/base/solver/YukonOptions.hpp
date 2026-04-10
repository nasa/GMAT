//$Id$
//------------------------------------------------------------------------------
//                              YukonOptions
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
// contract, Task Order 08.
//
// Created: 2017, Steven Hughes
//
// Converted: 2017/10/12, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Declares YukonOptions, a struct used to store option data for the optimizer
 */
//------------------------------------------------------------------------------

#ifndef YukonOptions_hpp
#define YukonOptions_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "UtilityException.hpp"

struct YUKON_API OptionsList
{
   /// Method to update the Hessian matrix
   std::string hessUpdateMethod;
   /// Type of merit function to use
   std::string meritFunction;
   /// Vector to store perturbation size
   Rvector finiteDiffVector;
   /// String of how derivative is calculated
   std::string derivativeMethod;
   /// Max number of iterations allowed before termination
   Integer maxIter;
   /// Max number of function evaluations allowed before termination
   Integer maxFunEvals;
   /// Tolerance on the constraints
   Real tolCon;
   /// Tolerance on the change in the cost function value
   Real tolF;
   /// Tolerance on the chance in the gradient
   Real tolGrad;
   /// Max allowable step size each variable may take
   Rvector maxVarStepSize;
   /// Type of quadratic programming method to be used
   std::string QPMethod;
   /// String determining what data is displayed during optimization
   std::string display;
   /// Max elastic weight to be used should the optimizer switch to elatic mode
   Integer maxElasticWeight;
};

#endif