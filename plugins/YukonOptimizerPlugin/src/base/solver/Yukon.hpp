//$Id$
//------------------------------------------------------------------------------
//                                Yukon
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
 * Declares Yukon methods
 */
//------------------------------------------------------------------------------

#ifndef Yukon_hpp
#define Yukon_hpp

#include "yukon_defs.hpp"
#include "gmatdefs.hpp"
#include "NLPFunctionGenerator.hpp"
#include "MinQP.hpp"
#include "YukonUserProblem.hpp"
#include "GmatProblemInterface.hpp"
#include "YukonOptions.hpp"
#include "YukonOutput.hpp"
#include "MessageInterface.hpp"

class YUKON_API Yukon
{
public:
   Yukon(YukonUserProblem *inputUserProblem, std::string hessianUpdateMethod,
          Integer maximumIterations, Integer maximumFunctionEvals,
          Real feasibilityTolerance, Real optimalityTolerance,
          Real functionTolerance, Integer maximumElasticWeight);
   Yukon(const Yukon& obj);
   Yukon& operator=(const Yukon& obj);
   ~Yukon();
   void PrepareToOptimize();
   void PrepareFailedRunOutput(Rvector &decVector, Real &costOut,
      Integer &exitFlag, OutputData &output);
   void RespondToData();
   void CheckStatus(Integer &status, Integer &funTypes, Integer &optimizerIter,
      Rvector &decVector, bool &isNewX, YukonUserProblem *userFunPointer);
   void Optimize(Rvector &decVector, Real &costOut, Integer &exitFlag,
      OutputData &output);
   void PrepareOutput(Rvector &decVector, Real &costOut, Integer &exitFlag,
      OutputData &output);
   void PrepareLineSearch();
   void TakeStep();
   void TestStepTaken();
   void PrepareForNextIteration();
   void CheckIfFinished();
   void SetNLPAndBoundsInfo();

private:
   void ComputeSearchDirection(Rvector &px, Real &f, Rvector &plam,
      Integer &exitFlag, Rvector &activeSet,
      Integer &qpIter);
   void PrepareElasticMode();
   std::string UpdateHessian();
   void PrepareInitialGuess();
   void SetConstraintTypes();
   Real CalcMeritFunction(Real f, Rvector cviol);
   Rvector CalcConViolations();
   Integer CheckConvergence(Rvector gradLag, Real f, Real fnew, Rvector x,
      Rvector xnew, Real alpha, Real maxConViolation);
   void RemoveLinearlyDependentCons(std::string typeToRemove);
   void SetMinimumMeritValues(Real meritFun, Real decreaseCond);
   void ReportNLPConfiguration();
   void ReportProblemSpecification();
   void WriteIterationHeader();
   void PrintSearchDirDebug(Rmatrix Aqp, Rvector bqplow, Rvector bqphigh);
   Real GetMax(Rvector inputVector);
   Real GetMin(Rvector inputVector);
   Real InfNorm(Rvector inputVector);
   Rvector MultiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector);
   Rvector MultiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector);
   Rmatrix MultiColToRowVector(Rvector vec1, Rvector vec2);

   /// Struct object containing the options for the optimizer to use
   OptionsList options;
   /// Pointer to NLPFunctionGenerator
   NLPFunctionGenerator *userFuncManager;
   /// Total number of constraints
   Integer totalNumCon;
   /// Number of user function evaluations
   Integer numFuncEvals;
   /// Number of decision variables
   Integer numDecisionVars;
   /// The decision vector
   Rvector decVec;
   /// Lower bounds on decision variables
   Rvector varLowerBounds;
   /// Upper bounds on decision variables
   Rvector varUpperBounds;
   /// Lower bounds on constraints
   Rvector conLowerBounds;
   /// Upper bounds on constraints
   Rvector conUpperBounds;
   /// Pointer to user problem
   YukonUserProblem *userProblem;
   /// The cost function value
   Real costFunction;
   /// The user constraint functions
   Rvector conFunctions;
   /// The cost function Jacobian
   Rvector costJac;
   /// The constraint function Jacobian
   Rmatrix conJac;
   /// Gradient of the Lagrangian
   Rvector gradLagrangian;
   /// Change in the Lagrangian after last step
   Rvector deltaGradLagrangian;
   /// The change in the decision variables after line
   Rvector stepTaken;
   /// Hessian of the Lagrangian
   Rmatrix hessLagrangian;
   /// Lagrange multiplieres
   Rvector lagMultipliers;
   /// Equality constraint indeces for conFunctions vector
   Rvector eqInd;
   /// Inequality constraint indeces for conFunctions vector
   Rvector ineqInd;
   /// Indicates if constraints are equality or inequality. 
   /// 1 = equality, 3 = inequality
   Rvector constraintTypes;
   /// Boolean represnting whether optimizer is in elastic mode
   bool isModeElastic;
   bool firstElasticStep;
   /// The weight used in elastic mode if elastic mode is required
   Real elasticWeight;

   /// From refactoring for reverse communication
   std::string method;
   /// Integer representing whether optimizer converged and how convergence 
   /// was achieved
   Integer isConverged;
   /// Number of iterations taken through the optimizer
   Integer numNLPIterations;
   /// Scaling factor used to increase mu elements
   Real sigma;
   /// Minimum reduction factor applied to alpha in a step iteration
   Real tau;
   /// Scaling factor used when applying the decrese condition to the merit
   /// function
   Real eta;
   /// Scaling vector for the merit function
   Rvector mu;
   /// Iterations since header was written when printing iteration data
   Integer iterSinceHeaderWrite;

   /// From prepare line search
   Real alpha;
   /// Value of merit function from previous decision vector
   Real meritF;
   /// Boolean showing whether current calculated step has converged
   bool foundStep;
   /// Parameter dictating whether nonmonotone line search can be used
   bool usePreviousMerits;
   /// Counter of how many attempts have been made to converge the current step
   Integer stepAttempts;
   /// Boolean determining whether relaxed step can be used
   bool allowSkippedReduction;
   /// Number of relaxed steps taken without reducing the merit function
   Integer skipsTaken;
   /// Number of steps taken with the normal step requirements being forced
   Integer forceReductionSteps;
   /// Boolean determining whether a test if a relaxed step has pushed the
   /// optimizer to reduce the merit function is required
   bool testSkippedReduction;
   /// The value of the cost function from the previous iteration
   Real fold;
   /// The value of the decision vector from the previous iteration
   Rvector xold;
   /// The constraint Jacobian from the previous iteration
   Rmatrix Jold;
   /// The gradient of the decision vector from the previous iteration
   Rvector gradFold;
   /// Vector containing the constraint violations from the previous converged
   /// step
   Rvector cViolOld;
   /// The number of times a step direction is attempted to produce a
   /// successful step
   Integer srchCount;
   /// The number of successive step directions that have failed to converge
   Integer failedSrchCount;
   /// Placeholder for the current decision vector, used to modify the decision
   /// vector when attemping to converge a step
   Rvector xk;
   /// Integer from the quadratic programming code representing whether 
   /// convergence was successful or if an error occured
   Integer qpExitFlag;
   /// The step direction calculated from MinQP
   Rvector px;
   /// Lagrange multipliers calculated from the MinQP subproblem
   Rvector plam;
   /// The number of iterations MinQP required for the current step calculation
   Integer qpIter;
   /// The current active constraints
   Rvector activeSet;
   /// Vector storing which constraints must be removed when taking fresh data
   /// from Gmat
   Rvector removeConIdx;
   /// Matrix storing which constraints were removed and which inequality
   /// bounds were merged from MinQP
   Rmatrix modifiedConIdxs;

   /// From line search
   Real meritFalpha;
   /// Maximum constraint violation achieved
   Real maxConViolation;
   /// A variable scaling factor to keep step size below specified maximum
   Real stepScalingFactor;

   /// Minimum merit function data
   /// Minimum merit function value
   Real minMeritFAlpha;
   /// Minimum meritf function with decrease condition included
   Real minDecreaseCond;
   /// Corresponding decision vector to minimum merit function
   Rvector minDecVec;
   /// Corresponding cost value to minimum merit function
   Real minCost;
   /// Corresponding constraint values to minimum merit function
   Rvector minConFuncs;
   /// Corresponding constraint violations to minimum merit function
   Rvector minConViol;
   /// Corresponding alpha value to minimum merit function
   Real minAlpha;
   /// Corresponding cost Jacobian to minimum merit function
   Rvector minCostJac;
   /// Corresponding constraint Jacobian to minimum merit function
   Rmatrix minConJac;
   /// Corresponding Hessian matrix to minimum merit function
   Rmatrix minHessian;

   /// From CheckIfFinished
   /// Boolean representing whether optimizer has converged and is finished
   bool isFinished;

   /// For state machine
   /// The current state the optimizer is in
   std::string currentState;
};
#endif
