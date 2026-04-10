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
// contract, Task 08
//
// Created: 2017, Steven Hughes
//
// Converted: 2017/10/12, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Defines Yukon methods
 */
//------------------------------------------------------------------------------

#include "Yukon.hpp"
#include <iostream>
#include <iomanip>

//#define DEBUG_ITERATION_DATA
//#define DEBUG_PROGRESS
//#define DEBUG_SEARCHDIR
//#define DEBUG_SCALING


//---------------------------
// public
//---------------------------

//------------------------------------------------------------------------------
// Yukon(MiNLPUserProblem *inputUserProblem)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param inputUserProblem Pointer to the user problem
*/
//------------------------------------------------------------------------------
Yukon::Yukon(YukonUserProblem *inputUserProblem,
               std::string hessianUpdateMethod, Integer maximumIterations,
               Integer maximumFunctionEvals, Real feasibilityTolerance,
               Real optimalityTolerance, Real functionTolerance,
               Integer maximumElasticWeight)
{
   isModeElastic = false;
   firstElasticStep = false;
   elasticWeight = 1.0;

   // Initialize various parameters
   totalNumCon = 0;
   method = "";
   isConverged = 0;
   numNLPIterations = 0;
   iterSinceHeaderWrite = 0;
   foundStep = false;
   usePreviousMerits = false;
   stepAttempts = 0;
   fold = 0;
   sigma = 1.0;
   tau = 0.1;
   eta = 0.1;
   alpha = 1.0;
   qpIter = 0;
   qpExitFlag = -1;
   srchCount = 0;
   failedSrchCount = 0;
   isFinished = false;

   // Initialize merit function values
   costFunction = 1e300;
   maxConViolation = 1e300;
   stepScalingFactor = 1.0;
   meritFalpha = 1e300;
   meritF = 1e300;
   minMeritFAlpha = 1e300;
   minDecreaseCond = 1e300;
   minCost = 1e300;
   minAlpha = 1e300;

   // Set options
   options.hessUpdateMethod = hessianUpdateMethod;
   options.meritFunction = "NocWright";
   options.finiteDiffVector.SetSize(5);
   for (Integer i = 0; i < 5; ++i)
      options.finiteDiffVector[i] = 1.0e-9;
   options.derivativeMethod = "Analytic";
   options.maxIter = maximumIterations;
   options.maxFunEvals = maximumFunctionEvals;
   options.tolCon = feasibilityTolerance;
   options.tolF = functionTolerance;
   options.tolGrad = optimalityTolerance;
   options.QPMethod = "minQP";
   options.display = "iter";
   options.maxElasticWeight = maximumElasticWeight;

   // Instantiate function manager
   userFuncManager = new NLPFunctionGenerator(inputUserProblem);
   userProblem = inputUserProblem;

   SetNLPAndBoundsInfo();

   // Call user problem and configure initial guess
   PrepareInitialGuess();

   // Initialize counters
   numFuncEvals = 0;
   allowSkippedReduction = false;
   testSkippedReduction = false;
   skipsTaken = 0;
   forceReductionSteps = 10;
   numDecisionVars = decVec.GetSize();
   currentState = "Instantiated";

   // Initialize max allowable step size for variables
   options.maxVarStepSize = userFuncManager->EvaluateMaxVarStep();
}

//------------------------------------------------------------------------------
// Yukon(const Yukon& obj)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
Yukon::Yukon(const Yukon& obj)
{
   isModeElastic = obj.isModeElastic;
   firstElasticStep = obj.firstElasticStep;
   elasticWeight = obj.elasticWeight;
   totalNumCon = obj.totalNumCon;
   method = obj.method;
   isConverged = obj.isConverged;
   numNLPIterations = obj.numNLPIterations;
   iterSinceHeaderWrite = obj.iterSinceHeaderWrite;
   foundStep = obj.foundStep;
   usePreviousMerits = obj.usePreviousMerits;
   stepAttempts = obj.stepAttempts;
   fold = obj.fold;
   currentState = obj.currentState;
   sigma = obj.sigma;
   tau = obj.tau;
   eta = obj.eta;
   alpha = obj.alpha;
   qpIter = obj.qpIter;
   qpExitFlag = obj.qpExitFlag;
   srchCount = obj.srchCount;
   failedSrchCount = obj.failedSrchCount;
   isFinished = obj.isFinished;
   costFunction = obj.costFunction;
   maxConViolation = obj.maxConViolation;
   stepScalingFactor = obj.stepScalingFactor;
   meritFalpha = obj.meritFalpha;
   meritF = obj.meritF;
   minMeritFAlpha = obj.minMeritFAlpha;
   minDecreaseCond = obj.minDecreaseCond;
   minCost = obj.minCost;
   minAlpha = obj.minAlpha;
   options.hessUpdateMethod = obj.options.hessUpdateMethod;
   options.meritFunction = obj.options.hessUpdateMethod;
   options.finiteDiffVector = obj.options.finiteDiffVector;
   options.derivativeMethod = obj.options.derivativeMethod;
   options.maxIter = obj.options.maxIter;
   options.maxFunEvals = obj.options.maxFunEvals;
   options.tolCon = obj.options.tolCon;
   options.tolF = obj.options.tolF;
   options.tolGrad = obj.options.tolGrad;
   options.QPMethod = obj.options.QPMethod;
   options.display = obj.options.display;
   options.maxElasticWeight = obj.options.maxElasticWeight;
   userFuncManager = obj.userFuncManager;
   userProblem = obj.userProblem;
   SetNLPAndBoundsInfo();
   PrepareInitialGuess();
   numFuncEvals = obj.numFuncEvals;
   allowSkippedReduction = obj.allowSkippedReduction;
   testSkippedReduction = obj.testSkippedReduction;
   skipsTaken = obj.skipsTaken;
   forceReductionSteps = obj.forceReductionSteps;
   numDecisionVars = obj.numDecisionVars;
   currentState = obj.currentState;
   options.maxVarStepSize = obj.options.maxVarStepSize;
}

//------------------------------------------------------------------------------
// Yukon& operator=(const Yukon& obj)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
Yukon& Yukon::operator=(const Yukon& obj)
{
   if (&obj != this)
   {
      isModeElastic = obj.isModeElastic;
      firstElasticStep = obj.firstElasticStep;
      elasticWeight = obj.elasticWeight;
      totalNumCon = obj.totalNumCon;
      method = obj.method;
      isConverged = obj.isConverged;
      numNLPIterations = obj.numNLPIterations;
      iterSinceHeaderWrite = obj.iterSinceHeaderWrite;
      foundStep = obj.foundStep;
      usePreviousMerits = obj.usePreviousMerits;
      stepAttempts = obj.stepAttempts;
      fold = obj.fold;
      currentState = obj.currentState;
      sigma = obj.sigma;
      tau = obj.tau;
      eta = obj.eta;
      alpha = obj.alpha;
      qpIter = obj.qpIter;
      qpExitFlag = obj.qpExitFlag;
      srchCount = obj.srchCount;
      failedSrchCount = obj.failedSrchCount;
      isFinished = obj.isFinished;
      costFunction = obj.costFunction;
      maxConViolation = obj.maxConViolation;
      stepScalingFactor = obj.stepScalingFactor;
      meritFalpha = obj.meritFalpha;
      meritF = obj.meritF;
      minMeritFAlpha = obj.minMeritFAlpha;
      minDecreaseCond = obj.minDecreaseCond;
      minCost = obj.minCost;
      minAlpha = obj.minAlpha;
      options.hessUpdateMethod = obj.options.hessUpdateMethod;
      options.meritFunction = obj.options.hessUpdateMethod;
      options.finiteDiffVector = obj.options.finiteDiffVector;
      options.derivativeMethod = obj.options.derivativeMethod;
      options.maxIter = obj.options.maxIter;
      options.maxFunEvals = obj.options.maxFunEvals;
      options.tolCon = obj.options.tolCon;
      options.tolF = obj.options.tolF;
      options.tolGrad = obj.options.tolGrad;
      options.QPMethod = obj.options.QPMethod;
      options.display = obj.options.display;
      options.maxElasticWeight = obj.options.maxElasticWeight;
      userFuncManager = obj.userFuncManager;
      userProblem = obj.userProblem;
      SetNLPAndBoundsInfo();
      PrepareInitialGuess();
      numFuncEvals = obj.numFuncEvals;
      allowSkippedReduction = obj.allowSkippedReduction;
      testSkippedReduction = obj.testSkippedReduction;
      skipsTaken = obj.skipsTaken;
      forceReductionSteps = obj.forceReductionSteps;
      numDecisionVars = obj.numDecisionVars;
      currentState = obj.currentState;
      options.maxVarStepSize = obj.options.maxVarStepSize;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~Yukon()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
Yukon::~Yukon()
{}

//------------------------------------------------------------------------------
// void SetNLPAndBoundsInfo()
//------------------------------------------------------------------------------
/**
* Method to set number of variables and constraints along with their bounds
* through the function manager
*/
//------------------------------------------------------------------------------
void Yukon::SetNLPAndBoundsInfo()
{
   // Call user problem and configure basic problem info
   userFuncManager->GetNLPInfo(numDecisionVars, totalNumCon);

   // Call user problem and configure bounds info
   userFuncManager->GetNLPBoundsInfo(varLowerBounds, varUpperBounds,
      conLowerBounds, conUpperBounds);
}

//------------------------------------------------------------------------------
// void PrepareToOptimize()
//------------------------------------------------------------------------------
/**
* Perform last minute initilization before iteratiing
*/
//------------------------------------------------------------------------------
void Yukon::PrepareToOptimize()
{
   // Evaluate all functions and derivatives
   Integer numFEval;
   Integer numGEval;
   userFuncManager->EvaluateAllNLPFuncJac(numDecisionVars, decVec, true,
      costFunction, costJac, numFEval, conFunctions, conJac, numGEval);
   numFuncEvals += numFEval;


   // Write data on the config and problem specs to file and display
   #ifdef DEBUG_ITERATION_DATA
      ReportNLPConfiguration();
      ReportProblemSpecification();
      WriteIterationHeader();
   #endif

   // Initializations
   method = "";
   isConverged = 0;
   numNLPIterations = 0;

   //----- Guess for Hessian of the Lagrangian
   hessLagrangian.SetSize(numDecisionVars, numDecisionVars);
   for (Integer i = 0; i < numDecisionVars; ++i)
      hessLagrangian(i, i) = 1.0;
   lagMultipliers.SetSize(totalNumCon);
   for (Integer i = 0; i < totalNumCon; ++i)
      lagMultipliers[i] = 0;
   SetConstraintTypes();
   removeConIdx.SetSize(0);

   // Line search parameters
   sigma = 1.0;
   tau = 0.1;
   eta = 0.1;
   mu.SetSize(totalNumCon);
   for (Integer i = 0; i < mu.GetSize(); ++i)
      mu[i] = 1.0e-20;
   minMeritFAlpha = 1e300;
   iterSinceHeaderWrite = 0;
   testSkippedReduction = false;
   currentState = "ReadyToOptimize";
}

//------------------------------------------------------------------------------
// void PrepareFailedRunOutput(Rvector &decVector, Real &costOut,
//                             Integer &exitFlag, OutputData &output)
//------------------------------------------------------------------------------
/**
* Prepare outputs for a failed run
*
* @param decVector The decision vector
* @param costOut The value of the cost function
* @param exitFlag Integer representing whether optimization successfully
*        completed
* @param output Output struct containing iteration and number of function
*        evaluate counts
*/
//------------------------------------------------------------------------------
void Yukon::PrepareFailedRunOutput(Rvector &decVector, Real &costOut,
                                   Integer &exitFlag, OutputData &output)
{
   Integer unusedFlag;
   PrepareOutput(decVector, costOut, unusedFlag, output);
   if (options.display == "iter")
   {
      MessageInterface::ShowMessage("\n Optimization Failed due to failure in "
         "QP subproblem \n");
   }
}

//------------------------------------------------------------------------------
// void RespondToData()
//------------------------------------------------------------------------------
/**
* Have the optimizer respond to the current state of the state machine
*/
//------------------------------------------------------------------------------
void Yukon::RespondToData()
{
   if (currentState == "Initialized")
   {
      PrepareToOptimize();
      return;
   }
   if (currentState == "ReadyToOptimize")
   {
      PrepareLineSearch();
      return;
   }
   if (currentState == "ReadyForLineSearch" ||
      currentState == "LineSearchIteration")
   {
      TakeStep();
      return;
   }
   if (currentState == "StepTaken")
   {
      TestStepTaken();
      if (currentState == "LineSearchConverged")
      {
         PrepareForNextIteration();
         CheckIfFinished();
         if (currentState == "StepTooSmall")
            return;
         if (!isFinished)
         {
            // Check if the max number of function evaluations will be exceeded
            if (numFuncEvals >= options.maxFunEvals)
            {
               currentState = "MaxFuncEvalsReached";
               return;
            }
            PrepareLineSearch();
         }
      }
      return;
   }
   if (currentState == "LineSearchConverged" || currentState == "Finished")
   {
      PrepareForNextIteration();
      CheckIfFinished();
      if (currentState == "StepTooSmall")
         return;
      if (!isFinished)
         PrepareLineSearch();
      return;
   }
}

//------------------------------------------------------------------------------
// void CheckStatus(Integer &status, Integer &funTypes, Rvector &decVector,
//                  bool &isNewX, bool isOptimized)
//------------------------------------------------------------------------------
/**
* Have the optimizer respond to the current state of the state machine
*
* @param status Integer representing the current status of the optimizer
* @param funTypes
* @param decVector The decision vector
* @param isNewX Boolean representing whether a new set of variables are being
*        used
* @param isOptimized Boolean representing whether current problem is optimized
*/
//------------------------------------------------------------------------------
void Yukon::CheckStatus(Integer &status, Integer &funTypes,
                        Integer &optimizerIter, Rvector &decVector,
                        bool &isNewX, YukonUserProblem *userFunPointer)
{
   userFunPointer = userProblem;
   if (currentState == "Instantiated")
   {
      status = 0;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "ReadyToOptimize")
   {
      status = 0;
      funTypes = 2;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = true;
      return;
   }
   if (currentState == "ReadyForLineSearch" ||
       currentState == "LineSearchIteration")
   {
      status = -1;
      funTypes = 1;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = true;
      return;
   }
   if (currentState == "StepTaken")
   {
      status = 0;
      funTypes = 1;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = true;
      return;
   }
   if (currentState == "LineSearchConverged")
   {
      status = 0;
      funTypes = 2;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = true;
      return;
   }
   if (currentState == "Finished")
   {
      status = 1;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "MaxFuncEvalsReached")
   {
      status = 3;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "MaxIterCountReached")
   {
      status = 2;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "StepTooSmall")
   {
      status = 4;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "FailedStepDirection")
   {
      status = 5;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
   if (currentState == "InfeasibleProblem")
   {
      status = 6;
      funTypes = 0;
      optimizerIter = numNLPIterations;
      decVector.SetSize(decVec.GetSize());
      decVector = decVec;
      isNewX = false;
      return;
   }
}

//------------------------------------------------------------------------------
// void Optimize(Rvector &decVector, Real &costOut, Integer &exitFlag, 
//               Integer &outputIter, Integer &outputFEvals)
//------------------------------------------------------------------------------
/**
* Optimize the problem
*
* @param decVector The decision vector
* @param costOut The output cost function value
* @param exitFlag Flag showing whether optimization completed successfully or
*        if some error occured to cause early termination
* @param outputIter The output number of NLP iterations completed
* @param outputFEvel The output number of function evaluates completed
*/
//------------------------------------------------------------------------------
void Yukon::Optimize(Rvector &decVector, Real &costOut, Integer &exitFlag,
   OutputData &output)
{
   // Perform the last minute initializations
   PrepareToOptimize();

   // Perform the iteration
   isFinished = false;
   while (!isFinished)
   {
      PrepareLineSearch();

      // Test to see if QP failed after elastic mode was tried
      if (qpExitFlag <= 0)
      {
         PrepareFailedRunOutput(decVector, costOut, exitFlag, output);
         return;
      }

      // Keep testing the step until converged or too many iterations
      while (!foundStep && srchCount < 10)
         TakeStep();

      // Updates for next iteration
      PrepareForNextIteration();

      // Check to see if the run is finished (converged, failed, etc.)
      CheckIfFinished();
   }

   // Prepare data for return. Run is complete.
   PrepareOutput(decVector, costOut, exitFlag, output);
}

//------------------------------------------------------------------------------
// void PrepareOutput(Rvector &decVector, Real &costOut, Integer &exitFlag,
//                    OutputData &output)
//------------------------------------------------------------------------------
/**
* Prepare output values to be written and/or displayed
*
* @param decVector The decision vector
* @param costOut The current value of the cost function
* @param exitFlag Integer representing whether optimizer successfully converged
* @param output Struct containing iteration and function evaluation counts
*/
//------------------------------------------------------------------------------
void Yukon::PrepareOutput(Rvector &decVector, Real &costOut, Integer &exitFlag,
   OutputData &output)
{
   if (isConverged == 0)
   {
      if (numFuncEvals >= options.maxFunEvals)
      {
         MessageInterface::ShowMessage("\n Optimization Failed \n"
            " Solution was not found within the maximum number \n"
            " of allowed function evaluations \n");
      }
      if (numNLPIterations >= options.maxIter)
      {
         MessageInterface::ShowMessage("\n Optimization Failed \n"
            " Solution was not found within maximum number \n"
            " of allowed iterations \n");
      }
   }

   exitFlag = isConverged;
   costOut = costFunction;
   output.iter = numNLPIterations;
   output.fevals = numFuncEvals;
   decVector.SetSize(userFuncManager->GetNumUserDecisionVars());
   for (Integer i = 0; i < decVector.GetSize(); ++i)
      decVector[i] = decVec[i];
   MessageInterface::ShowMessage("\n");
   currentState = "Finished";
}

//------------------------------------------------------------------------------
// void PrepareLineSearch()
//------------------------------------------------------------------------------
/**
* Prepare to find search direction
*/
//------------------------------------------------------------------------------
void Yukon::PrepareLineSearch()
{
   #ifdef DEBUG_PROGRESS
   {
      MessageInterface::ShowMessage("Entering PrepareLineSearch(),"
         " current state is: " + currentState + "\n");
   }
   #endif

   //Check if maximum number of iterations would be exceeded
   if (numNLPIterations >= options.maxIter)
   {
      currentState = "MaxIterCountReached";
      return;
   }

   // ----- Increment counters
   numNLPIterations++;
   iterSinceHeaderWrite++;
   stepAttempts = 0;
   Integer numStepReductions = 0;

   ++forceReductionSteps;

   // If the relaxed iteration has failed to reduce the merit function, return
   // to the decision vector with the lowest merit function and force the next
   // 10 iterations to use normal line search convergence requirements
   if (skipsTaken >= 3)
   {
      if (minConFuncs.GetSize() != totalNumCon)
         RemoveLinearlyDependentCons("minConFunc");
      if (minConJac.GetNumRows() != totalNumCon)
         RemoveLinearlyDependentCons("minConJac");

      allowSkippedReduction = false;
      forceReductionSteps = 0;
      meritFalpha = minMeritFAlpha;
      decVec = minDecVec;
      costFunction = minCost;
      conFunctions = minConFuncs;
      costJac = minCostJac;
      conJac = minConJac;
      alpha = minAlpha;
      skipsTaken = 0;
      testSkippedReduction = false;
      hessLagrangian = minHessian;
   }

   // Call QP problem to solve the QP problem, the set data to get ready for
   // the line search

   // Solve the QP subproblem to determine the search direction
   Real unusedValue;
   ComputeSearchDirection(px, unusedValue, plam, qpExitFlag, activeSet, qpIter);

   Rvector cviol = CalcConViolations();
   
   // Test if a change to mu is required based on the calculated search
   // direction
   Real testMuReduction = px*costJac +
      0.5*MultiRowToMatrix(hessLagrangian, px)*px;

   if (testMuReduction > 0)
   {
      Real muSum = 0;
      Rvector predConViolReduction(mu.GetSize());
      sigma = 2 * std::abs(px*costJac);
      Real sigmaScale = 0;
      Real muSumMin = 1.5*std::abs(px*costJac);
      for (Integer i = 0; i < cviol.GetSize(); ++i)
      {
         if (constraintTypes[i] == 1 || constraintTypes[i] == 2)
         {
            predConViolReduction[i] = std::abs(cviol[i]) - std::abs(cviol[i] +
               px*conJac.GetRow(i));
            muSum += mu[i] * predConViolReduction[i];
         }
         else
         {
            predConViolReduction[i] = std::abs(cviol[i]) - std::abs(cviol[i] -
               px*conJac.GetRow(i));
            muSum += mu[i] * predConViolReduction[i];
         }
      }

      if (muSum < muSumMin)
      {
         sigma -= muSum;

         // Scale sigma
         for (Integer i = 0; i < mu.GetSize(); ++i)
            sigmaScale += std::abs(plam[i] * predConViolReduction[i]);
         sigma = sigma / sigmaScale;

         // Update mu[i] only if it is larger than its previous value
         for (Integer i = 0; i < mu.GetSize(); ++i)
         {
            if (mu[i] < sigma*std::abs(plam[i]))
               mu[i] = sigma*std::abs(plam[i]);
         }

         if (numNLPIterations != 1 && !firstElasticStep)
         {
            minMeritFAlpha = minCost;
            for (Integer i = 0; i < totalNumCon; ++i)
               minMeritFAlpha += mu[i] * minConViol[i];
            minDecreaseCond = minMeritFAlpha;
         }
      }
   }

   meritF = CalcMeritFunction(costFunction, cviol);
   alpha = 1.0;

   if (numNLPIterations == 1)
   {
      stepTaken.SetSize(decVec.GetSize());
      for (Integer i = 0; i < stepTaken.GetSize(); ++i)
         stepTaken[i] = 0;
   }
   foundStep = false;
   fold = costFunction;
   xold.SetSize(numDecisionVars);
   xold = decVec;
   Jold.SetSize(totalNumCon, numDecisionVars);
   Jold = conJac;
   gradFold.SetSize(numDecisionVars);
   gradFold = costJac;
   cViolOld.SetSize(totalNumCon);
   cViolOld = cviol;
   srchCount = 0;
   xk.SetSize(numDecisionVars);
   xk = decVec;
   if (numNLPIterations == 1 || firstElasticStep)
   {
      SetMinimumMeritValues(meritF, 0);
      minHessian.SetSize(numDecisionVars, numDecisionVars);
      minHessian = hessLagrangian;
      if (firstElasticStep)
         firstElasticStep = false;
   }

   // Adjust variable step size to remain within max allowable step size and
   // variable bounds
   if (options.maxVarStepSize.GetSize() != numDecisionVars)
   {
      Integer oldNumDecisionVars = options.maxVarStepSize.GetSize();
      options.maxVarStepSize.Resize(numDecisionVars);
      for (Integer i = oldNumDecisionVars; i < numDecisionVars; ++i)
         options.maxVarStepSize[i] = 1e300;
   }

   stepScalingFactor = 1.0;
   Rvector stepSizeComparison(numDecisionVars);
   Rvector testDecVecBounds(numDecisionVars);
   for (Integer i = 0; i < px.GetSize(); ++i)
   {
      stepSizeComparison[i] = 1.0;
      testDecVecBounds[i] = 1.0;
      if (std::abs(options.maxVarStepSize[i]) < std::abs(px[i]))
         stepSizeComparison[i] = std::abs(options.maxVarStepSize[i] /
         px[i]);

      if ((xk + px)[i] < varLowerBounds[i])
         testDecVecBounds[i] = std::abs((varLowerBounds[i] - xk[i]) / px[i]);
      else if ((xk + px)[i] > varUpperBounds[i])
         testDecVecBounds[i] = std::abs((varUpperBounds[i] - xk[i]) / px[i]);
   }

   // Take the smallest scaling factor calculated to avoid all violations
   // that would occur
   if (GetMin(stepSizeComparison) < GetMin(testDecVecBounds))
      stepScalingFactor = GetMin(stepSizeComparison);
   else
      stepScalingFactor = GetMin(testDecVecBounds);

   // If a step size of zero is the only way to avoid breaking a bound,
   // the problem is infeasable
   if (stepScalingFactor == 0)
   {
      currentState = "InfeasibleProblem";
      return;
   }

   currentState = "ReadyForLineSearch";

   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Exiting PrepareLineSearch(),"
         " current state is: " + currentState + "\n");
   #endif
}

//------------------------------------------------------------------------------
// void TakeStep()
//------------------------------------------------------------------------------
/**
* Update the decision vector with the step, reducing the step size if a
* component of the step is larger than the user defined maximum step size
*/
//------------------------------------------------------------------------------
void Yukon::TakeStep()
{
   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Entering TakeStep(),"
         " current state is: " + currentState + "\n");
   #endif

   // Increment counter and value of x
   srchCount++;
   if (srchCount > 20)
   {
      srchCount = 0;
      failedSrchCount++;
      if (failedSrchCount >= 2)
      {
         currentState = "FailedStepDirection";
         return;
      }
      decVec = xk;
      skipsTaken = 0;
      for (Integer i = 0; i < numDecisionVars; ++i)
      {
         for (Integer j = 0; j < numDecisionVars; ++j)
         {
            if (i != j)
               hessLagrangian(i, j) = 0;
            else
               hessLagrangian(i, j) = 1.0;
         }
      }
      testSkippedReduction = false;
      allowSkippedReduction = false;
      currentState = "ReadyToOptimize";
      return;
   }
   stepTaken.SetSize(numDecisionVars);
   stepTaken = alpha*px;

   // Take step
   stepTaken = stepScalingFactor*alpha*px;
   decVec = xk + stepScalingFactor*alpha*px;

   currentState = "StepTaken";

   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Exiting TakeStep(),"
         " current state is: " + currentState + "\n");
   #endif
}

//------------------------------------------------------------------------------
// void TestStepTaken()
//------------------------------------------------------------------------------
/**
* Perform the line search to determine the step length
*
* Calculate the constraint penalty parameter, mu, to result in a descent
* direction for the merit function.  We use the L1 merit function, and
* calculate mu using N&W 2nd Ed. Eq 18.36
*/
//------------------------------------------------------------------------------
void Yukon::TestStepTaken()
{
   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Entering TestStepTaken(),"
         " current state is: " + currentState + "\n");
   #endif

   // Evaluate objective, constraints, and merit at x = x + alpha*p
   Integer numFEval;
   Integer unusedCounter;

   Real costOld = costFunction;
   Rvector conOld = CalcConViolations();
   userFuncManager->EvaluateNLPFunctionsOnly(numDecisionVars, decVec, true,
      costFunction, numFEval, conFunctions, unusedCounter);
   numFuncEvals += numFEval;

   // Remove constraints that were previously removed
   if (removeConIdx.GetSize() > 0)
      RemoveLinearlyDependentCons("conFunc");

   Rvector cviol = CalcConViolations();
   meritFalpha = CalcMeritFunction(costFunction, cviol);

   // Calculate the predicted merit reduction using a first order Taylor
   // series expansion on the previous cost and constraint values
   Real meritPred = fold + costJac*(decVec - xk);
   for (Integer i = 0; i < totalNumCon; ++i)
      meritPred += mu[i]*(cViolOld[i] + conJac.GetRow(i)*(decVec - xk));

   // Check sufficient decrease condition
   Real decreaseCond = eta*(meritF - meritPred);

   if (!allowSkippedReduction || forceReductionSteps < 10)
   {
      if (meritFalpha > meritF - decreaseCond)
      {
         // Check if the max number of function evaluations will be exceeded
         if (numFuncEvals >= options.maxFunEvals)
         {
            currentState = "MaxFuncEvalsReached";
            return;
         }
         Real alphaRed = 0.5 / (1.0 - (meritF - meritFalpha) /
            (meritF - meritPred));

         // If the calculated alpha reduction would produce too small of a step
         // reduction, use tau to reduce alpha instead
         if (alphaRed > tau && alphaRed <= 1.0)
            alpha = alpha*alphaRed;
         else
            alpha = alpha*tau;
         ++stepAttempts;
         currentState = "LineSearchIteration";
      }
      else
      {
         if (meritFalpha <= minMeritFAlpha)
         {
            SetMinimumMeritValues(meritFalpha, decreaseCond);
            if (meritFalpha <= meritF - decreaseCond)
               allowSkippedReduction = true;
         }
         else if (meritFalpha <= minDecreaseCond && numNLPIterations != 1)
         {
            allowSkippedReduction = true;
         }
         foundStep = true;
         currentState = "LineSearchConverged";
      }

      if (testSkippedReduction && meritFalpha > meritF)
         ++skipsTaken;
      else if (testSkippedReduction && meritFalpha <= meritF)
      {
         testSkippedReduction = false;
         skipsTaken = 0;
      }

   }
   else
   {
      // Check to make sure relaxed iteration has at least decreased the
      // merit function
      if (meritFalpha <= minDecreaseCond)
      {
         skipsTaken = 0;
         SetMinimumMeritValues(meritFalpha, decreaseCond);
         if (meritFalpha < meritF - decreaseCond)
            allowSkippedReduction = true;
      }
      else
      {
         ++skipsTaken;
         testSkippedReduction = true;
         allowSkippedReduction = false;
      }

      currentState = "LineSearchConverged";
   }

   if (skipsTaken >= 3)
      currentState = "LineSearchConverged";
   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Exiting TestStepTaken(),"
         " current state is: " + currentState + "\n");
   #endif
}

//------------------------------------------------------------------------------
// void PrepareForNextIteration()
//------------------------------------------------------------------------------
/**
* Evaluate important values at the new step, check for
* convergence and prepare for the next iteration
*/
//------------------------------------------------------------------------------
void Yukon::PrepareForNextIteration()
{
   // A line search direction has converged, reset the failed search attempts
   // counter
   failedSrchCount = 0;

   // Evaluate important values such as F, ce, ci at the new step
   Integer unusedValue;
   userFuncManager->EvaluateNLPDerivsOnly(numDecisionVars, decVec, false,
      costJac, conJac, unusedValue);

   // Remove constraints that were previously removed
   if (removeConIdx.GetSize() > 0)
      RemoveLinearlyDependentCons("conJac");

   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("\nCost Function %.5f\n", costFunction);
      MessageInterface::ShowMessage("\nConstraint Functions\n");
      for (Integer i = 0; i < conFunctions.GetSize(); ++i)
         MessageInterface::ShowMessage("%.5f   ", conFunctions[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   lagMultipliers += alpha*plam;
   Rvector gradLagrangianXK;
   if (lagMultipliers.GetSize() != 0)
   {
      gradLagrangianXK.SetSize(gradFold.GetSize());
      gradLagrangianXK = gradFold -
         MultiMatrixToColumn(Jold.Transpose(), lagMultipliers);
      gradLagrangian.SetSize(costJac.GetSize());
      gradLagrangian = costJac -
         MultiMatrixToColumn(conJac.Transpose(), lagMultipliers);
   }
   else
   {
      gradLagrangianXK.SetSize(gradFold.GetSize());
      gradLagrangianXK = gradFold;
      gradLagrangian.SetSize(costJac.GetSize());
      gradLagrangian = costJac;
   }
   deltaGradLagrangian.SetSize(gradLagrangian.GetSize());
   deltaGradLagrangian = gradLagrangian - gradLagrangianXK;

   // Output data, Check convergence, and prepare for next iteration

   // Output Data for New Iterate if Requested
   Rvector conViolations = CalcConViolations();
   if (conViolations.GetSize() == 0)
      maxConViolation = 0;
   else
      maxConViolation = GetMax(conViolations);

   #ifdef DEBUG_ITERATION_DATA
      if (iterSinceHeaderWrite == 10)
      {
         iterSinceHeaderWrite = 0;
         WriteIterationHeader();
      }
   #endif

   // Compute user constraints and cost without elastic
   // contributions for reporting
   Real conOut;
   Real costOut;
   if (isModeElastic)
   {
      conOut = userFuncManager->MaxUserConViolation(conViolations, decVec);
      costOut = userFuncManager->ShiftCost(costFunction, decVec);
   }
   else
   {
      costOut = costFunction;
      conOut = maxConViolation;
   }

   #ifdef DEBUG_ITERATION_DATA
      MessageInterface::ShowMessage("%i       %i          %.1e    %i    "
         " %.2e  %.2e   %7e    ", numNLPIterations, qpIter, alpha,
         activeSet.GetSize(), costOut, conOut, meritFalpha);
      MessageInterface::ShowMessage(method + "\n");
   #endif

   // PrepareLineSearch();
   currentState = "ReadyForConvergenceTest";

   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Exiting PrepareForNextIteration(),"
         " current state is: " + currentState + "\n");
   #endif
}

//------------------------------------------------------------------------------
// void CheckIfFinished()
//------------------------------------------------------------------------------
/**
* Check for convergence
*/
//------------------------------------------------------------------------------
void Yukon::CheckIfFinished()
{
   isConverged = CheckConvergence(gradLagrangian, fold, costFunction,
      decVec, xold, alpha, maxConViolation);

   // Update the Hessian of the Lagrangian
   if (isConverged == 0)
      method = UpdateHessian();

   if (currentState == "StepTooSmall")
      return;

   // Test for various reasons to stop iteration and set flage to stop of one
   // occurs
   if (!isConverged && numNLPIterations <= options.maxIter &&
      numFuncEvals <= options.maxFunEvals)
   {
      isFinished = false;
      currentState = "LineSearchIteration";
   }
   else if (isConverged < 0)
   {
      isFinished = true;
      currentState = "InfeasibleProblem";
   }
   else
   {
      isFinished = true;
      currentState = "Finished";
   }


   #ifdef DEBUG_PROGRESS
      MessageInterface::ShowMessage("Exiting CheckIfFinished(),"
         " current state is: " + currentState + "\n");
   #endif
}

//---------------------------
// private
//---------------------------

//------------------------------------------------------------------------------
// void ComputeSearchDirection(Rvector &px, Real &f, Rvector &plam,
//                             Integer &exitFlag, Rvector &activeSet,
//                             Integer &qpIter)
//------------------------------------------------------------------------------
/**
* Solve the Quadraditic Programming Subproblem
*
* Define the QP subproblem according to N&W 2nd Ed., Eqs.18.11
*                min:   f + gradF'*p + 0.5*p'*W*p  (over p)
*         subject to:   gradce'*p + ce =  0  (i in Equality Set)
*                       gradci'*p + ci >= 0  (i in Inequality Set)
*
* In terms of what minQP needs, we rewrite this as
*                min:    0.5*p'*W*p + gradF'*p  (over p)
*         subject to:   gradce'*p  = -ce  (i in Equality Set)
*                       gradci'*p >= -ci  (i in Inequality Set)
*
* @param px The resulting decison vector
* @param f The resulting cost function value
* @param plam The lagrange multipliers
* @param exitFlag Flag showing whether optimization completed successfully or
*        if some error occured to cause early termination
* @param activeSet
* @param qpIter Number of iterations used in QP solver
*/
//------------------------------------------------------------------------------
void Yukon::ComputeSearchDirection(Rvector &px, Real &f, Rvector &plam,
   Integer &exitFlag, Rvector &activeSet,
   Integer &qpIter)
{
   bool checkForDuplicateCons = false;
   if (numNLPIterations == 1)
      checkForDuplicateCons = true;
   Rvector W(0);
   MinQP qpOpt(0 * decVec, hessLagrangian, costJac, conJac,
      (conLowerBounds - conFunctions), (conUpperBounds - conFunctions), W, 2,
      checkForDuplicateCons);
   #ifdef DEBUG_PROGRESS
   {
      MessageInterface::ShowMessage("Hessian of Lagrangian\n");
      for (Integer i = 0; i < hessLagrangian.GetNumRows(); ++i)
      {
         for (Integer j = 0; j < hessLagrangian.GetNumColumns(); ++j)
            MessageInterface::ShowMessage("%.5f    ", hessLagrangian(i, j));
         MessageInterface::ShowMessage("\n");
      }

      MessageInterface::ShowMessage("Cost Jacobian\n");
      for (Integer i = 0; i < costJac.GetSize(); ++i)
         MessageInterface::ShowMessage("%.5f   ", costJac[i]);

      MessageInterface::ShowMessage("\nConstraint Jacobian\n");
      for (Integer i = 0; i < conJac.GetNumRows(); ++i)
      {
         for (Integer j = 0; j < conJac.GetNumColumns(); ++j)
            MessageInterface::ShowMessage("%.5f   ", conJac(i, j));
         MessageInterface::ShowMessage("\n");
      }
   }
   #endif

   #ifdef DEBUG_SEARCHDIR
   {
      MessageInterface::ShowMessage("=========  Input to from QP subproblem"
         " ==========\n");
      PrintSearchDirDebug(conJac, conLowerBounds - conFunctions,
         conUpperBounds - conFunctions);
      MessageInterface::ShowMessage("\nConstraint Functions\n");
      for (Integer i = 0; i < conLowerBounds.GetSize(); ++i)
      {
         MessageInterface::ShowMessage("%.5f   ",
            (conLowerBounds - conFunctions)[i]);
      }
      MessageInterface::ShowMessage("\n");
   }
   #endif

   // Call the QP solver
   Rvector lambdaQP;
   qpOpt.Optimize(px, f, lambdaQP, exitFlag, qpIter, activeSet);

   if (isModeElastic && exitFlag != 1)
   {
      // Something went really wrong in QP.  MinQP is not robust enough if this
      // happens.
      plam = -lagMultipliers;
      return;
   }

   // QP Failed. Switch to elastic mode.
   if (exitFlag != 1)
   {
      PrepareElasticMode();
      firstElasticStep = true;
      RemoveLinearlyDependentCons("All");
      qpOpt.~MinQP();
      W.SetSize(0);
      new (&qpOpt) MinQP(0 * decVec, hessLagrangian, costJac, conJac,
         conLowerBounds - conFunctions, conUpperBounds - conFunctions, W, 2,
         true);
      qpOpt.Optimize(px, f, lambdaQP, exitFlag, qpIter, activeSet);
   }

   // Update constraint values if MinQP removed any
   modifiedConIdxs.SetSize(qpOpt.GetModifiedCons().GetNumRows(),
      qpOpt.GetModifiedCons().GetNumColumns());
   modifiedConIdxs = qpOpt.GetModifiedCons();
   if (modifiedConIdxs.GetNumRows() != 0)
      RemoveLinearlyDependentCons("All");

   // Compute the lagrange multipliers
   plam.SetSize(lambdaQP.GetSize());
   plam = lambdaQP - lagMultipliers;

   #ifdef DEBUG_SEARCHDIR
   {
      MessageInterface::ShowMessage("\n=========  Output to from QP subproblem"
         " ==========\n");
      MessageInterface::ShowMessage("QP Solution\n");
      for (Integer i = 0; i < px.GetSize(); ++i)
         MessageInterface::ShowMessage("%.5f   ", px[i]);
      MessageInterface::ShowMessage("\nQP Function Value\n%.5f", f);
      MessageInterface::ShowMessage("\nQP LagMult\n");
      for (Integer i = 0; i < plam.GetSize(); ++i)
         MessageInterface::ShowMessage("%.5f   ", plam[i]);
      MessageInterface::ShowMessage("\n");
   }
   #endif
}

//------------------------------------------------------------------------------
// void PrepareElasticMode()
//------------------------------------------------------------------------------
/**
* Method to set up parameters to be used in elastic mode when the QP solver has
* failed
*/
//------------------------------------------------------------------------------
void Yukon::PrepareElasticMode()
{
   // Switch the user function generator to use elastic mode
   isModeElastic = true;
   Rvector oldDecVec = decVec;
   userFuncManager->~NLPFunctionGenerator();
   userFuncManager =
      new NLPFunctionGenerator(userProblem, true, elasticWeight);

   // Update the bounds data for elastic mode
   Integer oldTotalNumCon = totalNumCon;
   SetNLPAndBoundsInfo();
   hessLagrangian.SetSize(numDecisionVars, numDecisionVars);
   for (Integer i = 0; i < numDecisionVars; ++i)
      hessLagrangian(i, i) = 1.0;

   // Set up the decision vector
   decVec.SetSize(userFuncManager->GetNLPStartingPoint().GetSize());
   decVec = userFuncManager->GetNLPStartingPoint();
   for (Integer i = 0; i < userFuncManager->GetNumUserDecisionVars(); ++i)
      decVec[i] = oldDecVec[i];

   // Evaluate functions and derivatives and determine cosntraint types
   Integer unusedResult1;
   Integer unusedResult2;
   userFuncManager->EvaluateAllNLPFuncJac(numDecisionVars, decVec, true,
      costFunction, costJac, unusedResult1, conFunctions, conJac,
      unusedResult2);

   mu.Resize(totalNumCon);
   for (Integer i = oldTotalNumCon; i < totalNumCon; ++i)
      mu[i] = 1e-20;
   SetConstraintTypes();

   removeConIdx.SetSize(0);
}

//------------------------------------------------------------------------------
// std::string UpdateHessian
//------------------------------------------------------------------------------
/**
* Method used to update the Hessian matrix
*
* @return method The method used to update the Hessian matrix, given as a
*         string
*/
//------------------------------------------------------------------------------
std::string Yukon::UpdateHessian()
{
   Real theta;
   std::string method;
   Real projHess;
   bool newMinHessian = false;

   if (meritFalpha == minMeritFAlpha)
      newMinHessian = true;

   if (options.hessUpdateMethod == "DampedBFGS")
   {
      //----- The Damped BFGS Update formula.Ref. 1, Procedure 18.2.
      //      The values of 0.2 and 0.8 in the Ref. 1 are changed to
      //      0.1 and 0.9 based on empirical evidence : they seem to
      //      work better for the test problem set.

      projHess = MultiRowToMatrix(hessLagrangian, stepTaken) * stepTaken;
      if (stepTaken*deltaGradLagrangian >= 0.1*projHess)
      {
         theta = 1.0;
         method = "   BFGS Update";
      }
      else
      {
         try
         {
            theta = (0.9*projHess) / (projHess - stepTaken*deltaGradLagrangian);
            method = "   Damped BFGS Update";
         }
         catch (Rmatrix::DivideByZero &e)
         {
            currentState = "StepTooSmall";
            method = "No Update";
            return method;
         }
      }

      // Ref 1. Eq. 18.14
      Rvector r = theta*deltaGradLagrangian +
         MultiMatrixToColumn((1 - theta)*hessLagrangian, stepTaken);
      // Ref 1. Eq. 18.16
      try
      {
         hessLagrangian = hessLagrangian - hessLagrangian*
            (MultiColToRowVector(stepTaken, stepTaken))*hessLagrangian / projHess
            + MultiColToRowVector(r, r) / (stepTaken*r);
      }
      catch (Rmatrix::DivideByZero &e)
      {
         currentState = "StepTooSmall";
         method = "No Update";
         return method;
      }
   }

   else if (options.hessUpdateMethod == "SelfScaledBFGS")
   {
      //----- The self - scaled BFGS update.See section 4.3.3 of
      //      Eldersveld.
      Real gamma;
      projHess = MultiRowToMatrix(hessLagrangian, stepTaken)*stepTaken;
      if (stepTaken*deltaGradLagrangian >= projHess)
      {
         gamma = 1.0;
         method = "   BFGS Update";
         try
         {
            // Note that the Rmatrix math has a "divide by zero" issue here, so
            // we do it by hand
            Real den1, den2;
            den1 = 1.0 / projHess;
            den2 = 1.0 / (stepTaken*deltaGradLagrangian);

            hessLagrangian = gamma*hessLagrangian - gamma*hessLagrangian*
               MultiColToRowVector(stepTaken, stepTaken)*hessLagrangian * den1 +
               MultiColToRowVector(deltaGradLagrangian, deltaGradLagrangian) *
               den2;
         }
         // If step size has become too small, optimization has failed
         catch (Rmatrix::DivideByZero &e)
         {
            currentState = "StepTooSmall";
            method = "No Update";
            return method;
         }
      }
      else if (1e-10 < stepTaken*deltaGradLagrangian &&
         stepTaken*deltaGradLagrangian <= projHess)
      {
         try
         {
            gamma = deltaGradLagrangian*stepTaken / projHess;
            method = "   Self Scaled BFGS";
            hessLagrangian = gamma*hessLagrangian - gamma*hessLagrangian*
               MultiColToRowVector(stepTaken, stepTaken)*
               hessLagrangian / projHess +
               MultiColToRowVector(deltaGradLagrangian, deltaGradLagrangian) /
               (stepTaken*deltaGradLagrangian);
         }
         // If step size has become too small, optimization has failed
         catch (Rmatrix::DivideByZero &e)
         {
            currentState = "StepTooSmall";
            method = "No Update";
            return method;
         }
      }
      else
         method = "   No Update";
   }
   hessLagrangian = (hessLagrangian.Transpose() + hessLagrangian)*0.5;

   if (newMinHessian)
   {
      minHessian.SetSize(numDecisionVars, numDecisionVars);
      minHessian = hessLagrangian;
   }

   return method;
}

//------------------------------------------------------------------------------
// void PrepareInitialGuess()
//------------------------------------------------------------------------------
/**
* Calls user problem to get guess and puts within dec vec bounds
*/
//------------------------------------------------------------------------------
void Yukon::PrepareInitialGuess()
{
   // Call user problem to get the guess
   decVec.SetSize(userFuncManager->GetNLPStartingPoint().GetSize());
   decVec = userFuncManager->GetNLPStartingPoint();

   // Put the guess within variable bounds
   for (Integer varIdx = 0; varIdx < numDecisionVars; ++varIdx)
   {
      if (decVec[varIdx] > varUpperBounds[varIdx])
         decVec[varIdx] = varUpperBounds[varIdx];
      else if (decVec[varIdx] < varLowerBounds[varIdx])
         decVec[varIdx] = varLowerBounds[varIdx];
   }
}

//------------------------------------------------------------------------------
// void SetConstraintTypes()
//------------------------------------------------------------------------------
/**
* Creates arrays indicating which elements of the constraint vector
* of eq.and ineq.contraints
*/
//------------------------------------------------------------------------------
void Yukon::SetConstraintTypes()
{

   Integer eqSize = 0;
   eqInd.SetSize(eqSize);
   Integer ineqSize = 0;
   ineqInd.SetSize(ineqSize);
   constraintTypes.SetSize(totalNumCon);
   for (Integer conIdx = 0; conIdx < totalNumCon; ++conIdx)
   {
      constraintTypes[conIdx] = 0;
      if (std::abs(conUpperBounds[conIdx] - conLowerBounds[conIdx]) >=
         options.tolCon)
      {
         ++ineqSize;
         ineqInd.Resize(ineqSize);
         ineqInd[ineqSize - 1] = conIdx;
         constraintTypes[conIdx] = 3;
      }
      else
      {
         ++eqSize;
         eqInd.Resize(eqSize);
         eqInd[eqSize - 1] = conIdx;
         constraintTypes[conIdx] = 1;
      }
   }
}

//------------------------------------------------------------------------------
// Real CalcMeritFunction(Real f, Rvector cviol, Real mu)
//------------------------------------------------------------------------------
/**
* Computes the merit function given cost and constraint violations
*
* @return phi The merit function
*/
//------------------------------------------------------------------------------
Real Yukon::CalcMeritFunction(Real f, Rvector cviol)
{  
   Real phi = 0;
   
   if (options.meritFunction == "NocWright")
   {
      Real cviolSum = 0;
      for (Integer i = 0; i < cviol.GetSize(); ++i)
         cviolSum += mu[i]*cviol[i];
      phi = f + cviolSum;
   }

   return phi;
}

//------------------------------------------------------------------------------
// Rvector CalcConViolations()
//------------------------------------------------------------------------------
/**
* This utility calculates the constraint violations for all constraints.
* If there is not a violation for a specific constraint, the violation
* is set to zero.For equality constraints, the violation is the
* absolute value of the constraint value. For inequality constraints,
* the constraint violation is nonzero only if the constraint value is
* negative.  (If an inequality constraint is positive not violated)
*
* @return conViolation The constraint violation vector
*/
//------------------------------------------------------------------------------
Rvector Yukon::CalcConViolations()
{
   Rvector conLBOld = conLowerBounds;
   Rvector conUBOld = conUpperBounds;
   userFuncManager->PrepareArrays();
   userFuncManager->GetNLPBoundsInfo(varLowerBounds, varUpperBounds,
      conLowerBounds, conUpperBounds);

   // Remove constraints that were previously removed
   if (removeConIdx.GetSize() > 0)
   {
      conLowerBounds.SetSize(totalNumCon);
      conLowerBounds = conLBOld;
      conUpperBounds.SetSize(totalNumCon);
      conUpperBounds = conUBOld;
   }

   Rvector conViolation(totalNumCon);
   Real lowerError;
   Real upperError;
   for (Integer conIdx = 0; conIdx < totalNumCon; ++conIdx)
   {
      // equality constraints so violation is just difference from bound
      // (upper and lower are the same)
      conViolation[conIdx] = 0;
      if (constraintTypes[conIdx] == 1 || constraintTypes[conIdx] == 2)
      {
         conViolation[conIdx] =
            std::abs(conFunctions[conIdx] - conLowerBounds[conIdx]);
      }
      else
      {
         lowerError = conFunctions[conIdx] - conLowerBounds[conIdx];
         upperError = conUpperBounds[conIdx] - conFunctions[conIdx];
         if (upperError < 0)
            conViolation[conIdx] = std::abs(upperError);
         else if (lowerError < 0)
            conViolation[conIdx] = std::abs(lowerError);
      }
   }

   return conViolation;
}

//------------------------------------------------------------------------------
// Integer CheckConvergence(Rvector gradLag, Real f, Real fnew, Rvector x,
//                          Rvector xnew, Real alpha, Real maxConViolation)
//------------------------------------------------------------------------------
/**
* Checks multiple convergence criteria for algorithm termination
*
* @param gradLag Gradient of lagrangian
* @param f The previous cost function value
* @param fnew The new cost function value
* @param x The previous decision vector
* @param xnew The new decision vector
* @param alpha
* @param maxConViolation The current maximum constraint violation
* @return converged Integer representing which convergence criteria was met
*         or if problem cannot be converged
*/
//------------------------------------------------------------------------------
Integer Yukon::CheckConvergence(Rvector gradLag, Real f, Real fnew, Rvector x,
   Rvector xnew, Real alpha, Real maxConViolation)
{

   Integer converged = 0;
   Integer constraintsSatisfied = 0;
   bool isElasticModeActive = false;

   // Check if constraints are satisfied for use in later convergence tests.
   // If constraints are not satisfied, and elasticWeight < maxElasticWeight,
   // then increase weight and continue to iterate
   if (isModeElastic)
   {
      Real maxElasticVar = userFuncManager->GetMaxElasticVar(decVec);
      // maximum elastic var is not zero, and we have not reached the limit
      // on maximum elastic weight so increase elastic weight and continue
      // to iterate
      if (maxElasticVar > 1.0e-10 && elasticWeight < options.maxElasticWeight)
      {
         elasticWeight *= 10.0;
         userFuncManager->SetElasticWeight(elasticWeight);
         converged = 0;
         constraintsSatisfied = 0;
         return converged;
      }
      //  We have reached maximum elastic weight, but constraints are not
      //  satisfied so set flag to indicate constraints are not satisfied
      else if (maxElasticVar > 1.0e-10 &&
         elasticWeight >= options.maxElasticWeight)
      {
         constraintsSatisfied = 0;
      }
   }
   // Not in elastic mode, check if constraints are satisfied
   else
   {
      if (!GmatMathUtil::IsNaN(maxConViolation))
      {
         if (maxConViolation < options.tolCon)
         {
            constraintsSatisfied = 1;
         }
         else
         {
            constraintsSatisfied = 0;
         }
      }
   }

   // Check convergence criteria
   std::string messageHow;
   std::string messageMode;

   // If fun is not too close to zero, use relative change
   Real funChange = 0;
   if (std::abs(f) > 1e-7)
   {
      funChange = std::abs((fnew - f) / f);
   }
   else
   {
      funChange = std::abs(fnew - f);
   }

   // Check for convergence based on norm of the gradient of the Lagrangian
   if (InfNorm(gradLagrangian) < options.tolGrad)
   {
      if (!isModeElastic && constraintsSatisfied)
      {
         // Not in elastic mode, constraints satisfied, gradient of lagragian
         // is smaller than tolerance.  Solution was found.
         converged = 1;
         messageHow = " Magnitude of gradient of Lagrangian is less than "
            "tolerance \n";
      }
      else if (isModeElastic && !constraintsSatisfied)
      {
         // In elastic mode and elasticWeight = maxElasticWeight
         // constraints are NOT satisfied, gradient of lagragian
         // is smaller than tolerance. Solution was not found, but
         // no more progress is possible.
         converged = -1;
         messageHow = " The problem appears to be infeasible.  "
            "Constraint violations minimized. \n";
      }
      else
      {
         converged = 0;
      }
   }
   // Check for convergence based on change in objective function
   else if (funChange < options.tolF)
   {
      if (!isModeElastic && constraintsSatisfied)
      { 
         // Not in elastic mode, constraints satisfied, step size
         // is smaller than tolerance.  Solution was found.
         converged = 2;
         messageHow = " Absolute value of function improvement is less than "
         "tolerance\n";
      }
      else if (isModeElastic && !constraintsSatisfied)
      {
         // In elastic mode and elasticWeight is = maxElasticWeight
         // constraints are NOT satisfied, step size
         // is smaller than tolerance. Solution was not found, but
         // no more progress is possible
         converged = -1;
         messageHow = " The problem appears to be infeasible.  "
            "Constraint violations minimized. \n";
      }
      else
      {
         converged = 0;
      }
   }
   else
   {
      converged = 0;
   }
      
   // If the step size has been set to zero, a variable bound has been reached
   // and the search direction requires passing that bound, therefore making
   // the current problem setup infeasible
   if (stepScalingFactor == 0)
   {
      converged = -1;
      messageHow = " The problem appears to be infeasible due to variable "
         "bound restrictions. \n";
   }

   // If problem converged, and user requested output.
   // Write convergence type to console
   if (converged > 0 && options.display == "iter")
   {
      messageMode = "\n Optimization Terminated Successfully \n";
      MessageInterface::ShowMessage(messageMode + messageHow);
   }
   // The problem is infeasible and no more progress is possible.
   // Write message indicating the problem is infeasible.
   else if (converged < 0 && options.display == "iter")
   { 
      messageMode = "\n Optimal Solution Not Found \n";
      MessageInterface::ShowMessage(messageMode + messageHow);
   }
   
   return converged;
}

//------------------------------------------------------------------------------
// void RemoveLinearlyDependentCons(std::string typeToRemove)
//------------------------------------------------------------------------------
/**
* Method to remove constraints determined linearly dependent by MinQP
*
* @param typeToRemove String determining which of the constraint paramaters
*        need to be updated
*/
//------------------------------------------------------------------------------
void Yukon::RemoveLinearlyDependentCons(std::string typeToRemove)
{
   if (typeToRemove == "All")
   {
      totalNumCon -= modifiedConIdxs.GetNumRows();
      Rvector conFuncCopy = conFunctions;
      conFunctions.SetSize(totalNumCon);
      Rmatrix conJacCopy = conJac;
      conJac.SetSize(totalNumCon, numDecisionVars);
      Rvector conLBCopy = conLowerBounds;
      conLowerBounds.SetSize(totalNumCon);
      Rvector conUBCopy = conUpperBounds;
      conUpperBounds.SetSize(totalNumCon);
      Rvector muCopy = mu;
      mu.SetSize(totalNumCon);
      Rvector lagMultiplierCopy = lagMultipliers;
      lagMultipliers.SetSize(totalNumCon);
      Integer numRemovedCons = 0;
      Integer currentRemoveConIdx = removeConIdx.GetSize();
      bool skipBoundSetup = false;
      bool removeCurrentCon = false;
      removeConIdx.Resize(removeConIdx.GetSize() +
         modifiedConIdxs.GetNumRows());

      for (Integer i = 0; i < totalNumCon + modifiedConIdxs.GetNumRows(); ++i)
      {
         skipBoundSetup = false;
         removeCurrentCon = false;
         for (Integer j = 0; j < modifiedConIdxs.GetNumRows(); ++j)
         {
            if (i == modifiedConIdxs(j, 1))
            {
               removeConIdx[currentRemoveConIdx + numRemovedCons] = i;
               numRemovedCons++;
               removeCurrentCon = true;
            }
            else if (i == modifiedConIdxs(j, 0))
            {
               if (conLBCopy[i] > conLBCopy[(Integer)modifiedConIdxs(j, 1)])
               {
                  conLowerBounds[(Integer)modifiedConIdxs(j, 0) - numRemovedCons] = conLBCopy[i];
                  conUpperBounds[(Integer)modifiedConIdxs(j, 0) - numRemovedCons] =
                     conUBCopy[(Integer)modifiedConIdxs(j, 1)];
               }
               if (conUBCopy[i] < conUBCopy[(Integer)modifiedConIdxs(j, 1)])
               {
                  conUpperBounds[(Integer)modifiedConIdxs(j, 0) - numRemovedCons] = conUBCopy[i];
                  conLowerBounds[(Integer)modifiedConIdxs(j, 0) - numRemovedCons] =
                     conLBCopy[(Integer)modifiedConIdxs(j, 1)];
               }
               skipBoundSetup = true;
            }
         }

         if (!removeCurrentCon)
         {
            if (!skipBoundSetup)
            {
               conLowerBounds[i - numRemovedCons] = conLBCopy[i];
               conUpperBounds[i - numRemovedCons] = conUBCopy[i];
            }
            conFunctions[i - numRemovedCons] = conFuncCopy[i];
            mu[i - numRemovedCons] = muCopy[i];
            if (i < lagMultiplierCopy.GetSize())
               lagMultipliers[i - numRemovedCons] = lagMultiplierCopy[i];
            else
               lagMultipliers[i - numRemovedCons] = 0;
            for (Integer conJacCol = 0; conJacCol < numDecisionVars;
               ++conJacCol)
            {
               conJac(i - numRemovedCons, conJacCol) =
                  conJacCopy(i, conJacCol);
            }
         }
      }

      SetConstraintTypes();
   }

   else if (typeToRemove == "conFunc")
   {
      Rvector conFuncCopy = conFunctions;
      conFunctions.SetSize(totalNumCon);
      bool removeCurrentCon = false;
      Integer numRemovedCons = 0;
      for (Integer i = 0; i < totalNumCon + removeConIdx.GetSize(); ++i)
      {
         removeCurrentCon = false;
         for (Integer j = 0; j < removeConIdx.GetSize(); ++j)
         {
            if (i == removeConIdx[j])
            {
               ++numRemovedCons;
               removeCurrentCon = true;
            }
         }
         if (!removeCurrentCon)
            conFunctions[i - numRemovedCons] = conFuncCopy[i];
      }
   }

   else if (typeToRemove == "conJac")
   {
      Rmatrix conJacCopy = conJac;
      conJac.SetSize(totalNumCon, numDecisionVars);
      bool removeCurrentCon = false;
      Integer numRemovedCons = 0;
      for (Integer i = 0; i < totalNumCon + removeConIdx.GetSize(); ++i)
      {
         removeCurrentCon = false;
         for (Integer j = 0; j < removeConIdx.GetSize(); ++j)
         {
            if (i == removeConIdx[j])
            {
               ++numRemovedCons;
               removeCurrentCon = true;
            }
         }
         if (!removeCurrentCon)
         {
            for (Integer conJacCol = 0; conJacCol < numDecisionVars;
               ++conJacCol)
            {
               conJac(i - numRemovedCons, conJacCol) =
                  conJacCopy(i, conJacCol);
            }
         }
      }
   }

   else if (typeToRemove == "minConFunc")
   {
      Rvector conFuncCopy = minConFuncs;
      minConFuncs.SetSize(totalNumCon);
      bool removeCurrentCon = false;
      Integer numRemovedCons = 0;
      for (Integer i = 0; i < totalNumCon + removeConIdx.GetSize(); ++i)
      {
         removeCurrentCon = false;
         for (Integer j = 0; j < removeConIdx.GetSize(); ++j)
         {
            if (i == removeConIdx[j])
            {
               ++numRemovedCons;
               removeCurrentCon = true;
            }
         }
         if (!removeCurrentCon)
            minConFuncs[i - numRemovedCons] = conFuncCopy[i];
      }
   }

   else if (typeToRemove == "minConJac")
   {
      Rmatrix conJacCopy = minConJac;
      minConJac.SetSize(totalNumCon, numDecisionVars);
      bool removeCurrentCon = false;
      Integer numRemovedCons = 0;
      for (Integer i = 0; i < totalNumCon + removeConIdx.GetSize(); ++i)
      {
         removeCurrentCon = false;
         for (Integer j = 0; j < removeConIdx.GetSize(); ++j)
         {
            if (i == removeConIdx[j])
            {
               ++numRemovedCons;
               removeCurrentCon = true;
            }
         }
         if (!removeCurrentCon)
         {
            for (Integer conJacCol = 0; conJacCol < numDecisionVars;
               ++conJacCol)
            {
               minConJac(i - numRemovedCons, conJacCol) =
                  conJacCopy(i, conJacCol);
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void SetMinimumMeritValues(Real minMeritFun, Real decreaseCond)
//------------------------------------------------------------------------------
/**
* Method to set what the current minimum merit function is and the parameter
* values that were calculated to achieve it
*
* @param minMeritFun The minimum merit function
* @param decreaseCond The decrease condition determined by the predicted
*        reduction in the merit function at the step the minimum was calculated
*        at.  This is used as the minimum decrease condition to check whether
*        the relaxed line search criteria can continue to be used
*/
//------------------------------------------------------------------------------
void Yukon::SetMinimumMeritValues(Real minMeritFun, Real decreaseCond)
{
   Integer unusedValue;
   minMeritFAlpha = minMeritFun;
   minDecreaseCond = meritF - decreaseCond;
   minDecVec.SetSize(numDecisionVars);
   minDecVec = decVec;
   minCost = costFunction;
   minConFuncs.SetSize(totalNumCon);
   minConFuncs = conFunctions;
   minConViol.SetSize(totalNumCon);
   minConViol = CalcConViolations();
   minAlpha = alpha;
   minConJac.SetSize(totalNumCon, numDecisionVars);
   userFuncManager->EvaluateNLPDerivsOnly(numDecisionVars, decVec, false,
      minCostJac, minConJac, unusedValue);
   if (minConFuncs.GetSize() != totalNumCon)
      RemoveLinearlyDependentCons("minConFunc");
   if (minConJac.GetNumRows() != totalNumCon)
      RemoveLinearlyDependentCons("minConJac");
}

//------------------------------------------------------------------------------
// void ReportNLPConfiguration()
//------------------------------------------------------------------------------
/**
* Write NLP Configuration to display and/or file
*/
//------------------------------------------------------------------------------
void Yukon::ReportNLPConfiguration()
{
   MessageInterface::ShowMessage(" NLP Configuration \n");
   MessageInterface::ShowMessage("-------------------\n");
   MessageInterface::ShowMessage(" Hessian Update Method: " +
      options.hessUpdateMethod);
   MessageInterface::ShowMessage("\n Merit Function       : " +
      options.meritFunction);
   MessageInterface::ShowMessage("\n MaxIter              : %i",
      options.maxIter);
   MessageInterface::ShowMessage("\n MaxFunEvals          : %i",
      options.maxFunEvals);
   MessageInterface::ShowMessage("\n TolCon               : %e",
      options.tolCon);
   MessageInterface::ShowMessage("\n TolF                 : %e",
      options.tolF);
   MessageInterface::ShowMessage("\n TolGrad              : %e",
      options.tolGrad);
   MessageInterface::ShowMessage("\n Display              : " +
      options.display);
   MessageInterface::ShowMessage("\n MaxElasticWeight     : %i\n",
      options.maxElasticWeight);
}

//------------------------------------------------------------------------------
// void ReportProblemSpecification()
//------------------------------------------------------------------------------
/**
* Write NLP problem specifications to display and/or file
*/
//------------------------------------------------------------------------------
void Yukon::ReportProblemSpecification()
{
   MessageInterface::ShowMessage(" Problem Specifications \n");
   MessageInterface::ShowMessage("------------------------\n");
   MessageInterface::ShowMessage(" Number of Variables:      %i\n",
      userFuncManager->GetNumUserDecisionVars());
   MessageInterface::ShowMessage(" Number of Constraints:    %i\n\n",
      userFuncManager->GetNumUserConstraints());
}

//------------------------------------------------------------------------------
// void ReportProblemSpecification()
//------------------------------------------------------------------------------
/**
* Write the iteration header
*/
//------------------------------------------------------------------------------
void Yukon::WriteIterationHeader()
{
   MessageInterface::ShowMessage("Major Minors         Step  nCon         Obj      MaxCon      "
      "MeritFunction        Hess. Update\n");
}

//------------------------------------------------------------------------------
// void PrintSearchDirDebug(Rmatrix Aqp, Rvector bqplow, Rvector bqphigh)
//------------------------------------------------------------------------------
/**
* Write various matrix and vector information to the display and/or file be
* used for debugging
*/
//------------------------------------------------------------------------------
void Yukon::PrintSearchDirDebug(Rmatrix Aqp, Rvector bqplow, Rvector bqphigh)
{
   MessageInterface::ShowMessage("Dec Vec\n");
   for (Integer i = 0; i < decVec.GetSize(); ++i)
      MessageInterface::ShowMessage("%.5f   ", decVec[i]);
   MessageInterface::ShowMessage("\nLag. Hess\n");
   for (Integer i = 0; i < hessLagrangian.GetNumRows(); ++i)
   {
      for (Integer j = 0; j < hessLagrangian.GetNumColumns(); ++j)
         MessageInterface::ShowMessage("%.5f   ", hessLagrangian(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("Cost Jac\n");
   for (Integer i = 0; i < costJac.GetSize(); ++i)
      MessageInterface::ShowMessage("%.5f   ", costJac[i]);
   MessageInterface::ShowMessage("\nA matrix\n");
   for (Integer i = 0; i < Aqp.GetNumRows(); ++i)
   {
      for (Integer j = 0; j < Aqp.GetNumColumns(); ++j)
         MessageInterface::ShowMessage("%.5f   ", Aqp(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("b matrix low\n");
   for (Integer i = 0; i < bqplow.GetSize(); ++i)
      MessageInterface::ShowMessage("%.5f   ", bqplow[i]);
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("b matrix high\n");
   for (Integer i = 0; i < bqphigh.GetSize(); ++i)
      MessageInterface::ShowMessage("%.5f   ", bqphigh[i]);
   MessageInterface::ShowMessage("\n");
}

//------------------------------------------------------------------------------
// Real GetMax(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to find maximum value in an Rvector
*
* @return currentMax The maximum value in the vector
*/
//------------------------------------------------------------------------------
Real Yukon::GetMax(Rvector inputVector)
{
   Real currentMax = inputVector[0];
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (inputVector[i] > currentMax)
         currentMax = inputVector[i];
   }

   return currentMax;
}

//------------------------------------------------------------------------------
// Real GetMin(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to find minimum value in an Rvector
*
* @return currentMin The minimum value in the vector
*/
//------------------------------------------------------------------------------
Real Yukon::GetMin(Rvector inputVector)
{
   Real currentMin = inputVector[0];
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (inputVector[i] < currentMin)
         currentMin = inputVector[i];
   }

   return currentMin;
}

//------------------------------------------------------------------------------
// Real InfNorm(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to calculate the infinite-norm of an Rvector
*
* @return currentMax The infinite-norm
*/
//------------------------------------------------------------------------------
Real Yukon::InfNorm(Rvector inputVector)
{
   Real currentMax = std::abs(inputVector[0]);
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (std::abs(inputVector[i]) > currentMax)
         currentMax = std::abs(inputVector[i]);
   }

   return currentMax;
}

//------------------------------------------------------------------------------
// Rvector multiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to multiply an Rmatrix to an Rvector when the Rvector is considered
* to be a column vector
*
* @return product The product of the matrix and column vector
*/
//------------------------------------------------------------------------------
Rvector Yukon::MultiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector)
{
   Rvector product(inputMatrix.GetNumRows());
   Real* productData = (Real*)product.GetDataVector();
   Real* inputMatrixData = (Real*)inputMatrix.GetDataVector();
   Real* inputVectorData = (Real*)inputVector.GetDataVector();

   Integer rowCount = inputMatrix.GetNumRows();
   Integer colCount = inputMatrix.GetNumColumns();
   Integer vectorSize = inputVector.GetSize();

   for (Integer i = 0; i < rowCount; ++i)
   {
      for (Integer j = 0; j < vectorSize; ++j)
      {
         productData[i] +=
            inputMatrixData[i * colCount + j] * inputVectorData[j];
      }
   }

   return product;
}

//------------------------------------------------------------------------------
// Rvector MultiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to multiply an Rvector to an Rmatrix when the Rvector is considered
* a row vector
*
* @return product The product of the row vector and matrix
*/
//------------------------------------------------------------------------------
Rvector Yukon::MultiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector)
{
   Rvector product(inputVector.GetSize());
   Real* productData = (Real*)product.GetDataVector();
   Real* inputMatrixData = (Real*)inputMatrix.GetDataVector();
   Real* inputVectorData = (Real*)inputVector.GetDataVector();

   Integer rowCount = inputMatrix.GetNumRows();
   Integer colCount = inputMatrix.GetNumColumns();
   Integer vectorSize = inputVector.GetSize();

   for (Integer i = 0; i < rowCount; ++i)
   {
      for (Integer j = 0; j < vectorSize; ++j)
      {
         productData[i] +=
            inputMatrixData[j * colCount + i] * inputVectorData[j];
      }
   }

   return product;
}

//------------------------------------------------------------------------------
// Rmatrix MultiColToRowVector(Rvector vec1, Rvector vec2)
//------------------------------------------------------------------------------
/**
* Method to multiply a column vector to a row vector
*
* @return product The product of the column and row vector as a matrix
*/
//------------------------------------------------------------------------------
Rmatrix Yukon::MultiColToRowVector(Rvector vec1, Rvector vec2)
{
   Integer rowCount = vec1.GetSize();
   Integer colCount = vec2.GetSize();
   Rmatrix product(vec1.GetSize(), vec2.GetSize());

   Real* productData = (Real*)product.GetDataVector();
   Real* vec1Data = (Real*)vec1.GetDataVector();
   Real* vec2Data = (Real*)vec2.GetDataVector();

   for (Integer i = 0; i < rowCount; ++i)
   {
      for (Integer j = 0; j < colCount; ++j)
         productData[i*colCount + j] = vec1Data[i] * vec2Data[j];
   }

   return product;
}
