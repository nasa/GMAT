//$Id$
//------------------------------------------------------------------------------
//                                YukonUserProblem
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
// Converted: 2017/09/15, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Declares YukonUserProblem base class for YukonUserProblems
 */
//------------------------------------------------------------------------------

#ifndef YukonUserProblem_hpp
#define YukonUserProblem_hpp

#include "yukon_defs.hpp"
#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

class YUKON_API YukonUserProblem
{
public:
   YukonUserProblem();

   virtual void GetNLPInfo(Integer &totalNumVars, Integer &totalNumCons) = 0;
   virtual Rvector GetStartingPoint() = 0;
   virtual void GetBoundsInfo(Integer numVars, Integer numCons, Rvector &varLowerBounds, Rvector &varUpperBounds, Rvector &conLB, Rvector &conUB) = 0;
   virtual Real EvaluateCostFunc(Integer numVars, Rvector decVector, bool isNewX) = 0;
   virtual Rvector EvaluateCostJac(Integer numVars, Rvector decVector, bool isNewX) = 0;
   virtual Rvector EvaluateConFunc(Integer numVars, Rvector decVector, bool isNewX) = 0;
   virtual Rmatrix EvaluateConJac(Integer numVars, Rvector decVector, bool isNewX) = 0;
   virtual void EvaluateConJacDimensions(Integer numVariables, Rvector decVector, bool isNewX, Integer &rowCount, Integer &colCount) = 0;
   virtual std::vector <Real> GetMaxVarStepSize() = 0;

   void HandleOneSidedVarBounds(Rvector lowerBoundVector, Rvector upperBoundVector);

protected:
   /// The bound constraint matrix
   Rmatrix AVarBound;
   /// Vector containing the constraint bounds
   Rvector bVarBound;
   /// The number of bound constraints
   Integer numBoundCon;
   /// The constraint type of each constraint
   Rvector conType;
   /// The constraint mode being used
   Integer conMode;
   /// Vector containing the constraint upper bounds
   Rvector bVarBoundUpper;
   /// Vector containing the constraint lower bounds
   Rvector bVarBoundLower;
   /// The name of the current optimization problem
   std::string problemName;
   /// The name of the cost function
   std::string costFuncName;
   /// The name of the constraint function
   std::string conFuncName;

   /// Vector of variable lower bounds
   Rvector varLowerBounds;
   /// Vector of variable upper bounds
   Rvector varUpperBounds;
   /// Vector of constraint lower bounds
   Rvector conLowerBounds;
   /// Vector of constraint upper bounds
   Rvector conUpperBounds;
   /// The number of nonlinear inequality constraints
   Integer numNonLinIneqCon;
   /// Index indicating where the starting point is for nonlinear inequality
   /// constraints in the constraint list
   Integer nonLinIneqConStartIdx;
   /// The number of nonlinear equality constraints
   Integer numNonLinEqCon;
   /// Index indicating where the starting point is for nonlinear equality
   /// constraints in the constraint list
   Integer nonLinEqConStartIdx;
   /// The number of linear inequality constraints
   Integer numLinIneqCon;
   /// Index indicating where the starting point is for linear inequality
   /// constraints in the constraint list
   Integer linIneqConStartIdx;
   /// The number of linear equality constraints
   Integer numLinEqCon;
   /// Index indicating where the starting point is for linear equality
   /// constraints in the constraint list
   Integer linEqConStartIdx;
   /// Index indicating where the starting point is for a bound constraint
   /// in the constraint list
   Integer boundConStartIdx;
   /// The constraint Jacobian matrix
   Rmatrix conJacobian;
   /// The cost Jacobian vector
   Rvector costJacobian;
   /// The total number of nonlinear constraints
   Integer numNonLinCon;
   /// The total number of linear constraints
   Integer numLinCon;
   /// The total number of variables
   Integer numVars;
   /// The total number of constraints
   Integer numCons;
   /// Vector containing the constraint function values
   Rvector conFunctions;

   /// Value to hold infinity
   Real inf;
};

#endif