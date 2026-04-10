//$Id$
//------------------------------------------------------------------------------
//                                NLPFunctionGenerator
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
 * Defines NLPFunctionGenerator methods
 */
//------------------------------------------------------------------------------

#include "NLPFunctionGenerator.hpp"
#include "UtilityException.hpp"
#include <iostream>
#include <cfloat>                // For FLT_MAX

//------------------------------------------------------------------------------
// NLPFunctionGenerator(YukonUserProblem *inputUserFunction)
//------------------------------------------------------------------------------
/**
* Constructor, not in elastic mode
*
* @param inputUserProblem Pointer to object that user function provides
*/
//------------------------------------------------------------------------------
NLPFunctionGenerator::NLPFunctionGenerator(YukonUserProblem *inputUserFunction)
{
   // Set infinity parameter
   inf = std::numeric_limits<double>::infinity();

   isModeElastic = false;

   // Set pointer to the user function
   userFunction = inputUserFunction;

   // Call the user function to get infor needed for initialization
   try
   {
      userFunction->GetNLPInfo(numUserDecisionVars, numUserConstraints);
   }
   catch (const std::exception &e)
   {
      std::string errmsg = "Call to ::GetNLPInfo() failed";
      throw UtilityException(errmsg);
   }

   // Perform basic checks on the user implementation
   ValidateUserProblem();

   // Set up the array sizes based on user inputs, mode, and bounds constraints
   PrepareArrays();
}

//------------------------------------------------------------------------------
// NLPFunctionGenerator(YukonUserProblem *inputUserFunction,
//                      bool isModeElastic, Real inputElasticWeight)
//------------------------------------------------------------------------------
/**
* Constructor, using elastic mode
*
* @param inputUserProblem Pointer to object that user function provides
* @param isModeElastic Boolean determining whether elastic mode is being used
* @param inputElasticWeight Elastic weight parameter
*/
//------------------------------------------------------------------------------
NLPFunctionGenerator::NLPFunctionGenerator(YukonUserProblem *inputUserFunction,
                                           bool inputIsModeElastic,
                                           Real inputElasticWeight)
{
   isModeElastic = inputIsModeElastic;

   // Set infinity parameter
   inf = std::numeric_limits<double>::infinity();

   // Set pointer to the user function
   userFunction = inputUserFunction;

   // Call the user function to get infor needed for initialization
   try
   {
      userFunction->GetNLPInfo(numUserDecisionVars, numUserConstraints);
   }
   catch (const std::exception &e)
   {
      std::string errmsg = "Call to ::GetNLPInfo() failed";
      throw UtilityException(errmsg);
   }

   // Perform basic checks on the user implementation
   ValidateUserProblem();

   if (isModeElastic)
      PrepareElasticMode(inputElasticWeight);

   // Set up the array sizes based on user inputs, mode, and bounds constraints
   PrepareArrays();
}

//------------------------------------------------------------------------------
// ~NLPFunctionGenerator()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
NLPFunctionGenerator::~NLPFunctionGenerator()
{}

//------------------------------------------------------------------------------
// void ValidateUserProblem()
//------------------------------------------------------------------------------
/**
* Check starting point function call and dimensions of the
* starting point
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::ValidateUserProblem()
{
   // Check starting point function call and dimensions of the
   // starting point
   Rvector startingPoint;
   std::string errmsg;

   try
   {
      startingPoint = userFunction->GetStartingPoint();
   }
   catch (const std::exception &e)
   {
      std::string errmsg = "User Function ::GetStartingPointMethod() failed to"
         " evaluate.";
      throw UtilityException(errmsg);
   }
   if (startingPoint.GetSize() != numUserDecisionVars)
   {
      errmsg = "Starting point dimension is not consistent with "
         "number of decision variables.";
      throw UtilityException(errmsg);
   }

   // Check bound vector dimensions
   Rvector varLowerBounds;
   Rvector varUpperBounds;
   Rvector conLowerBounds;
   Rvector conUpperBounds;

   try
   {
      userFunction->GetBoundsInfo(numUserDecisionVars, numUserConstraints,
         varLowerBounds, varUpperBounds, conLowerBounds, conUpperBounds);
   }
   catch (const std::exception &e)
   {
      errmsg = "User Function ::GetBoundsInfo() failed to evaluate.";
      throw UtilityException(errmsg);
   }
   if (varLowerBounds.GetSize() != numUserDecisionVars)
   {
      errmsg = "Variable lower bounds dimension is not consistent with number "
         "of decision variables.";
      throw UtilityException(errmsg);
   }
   if (varUpperBounds.GetSize() != numUserDecisionVars)
   {
      errmsg = "Variable upper bounds dimension is not consistent with number "
         "of decision variables.";
      throw UtilityException(errmsg);
   }
   if (conLowerBounds.GetSize() != numUserConstraints)
   {
      errmsg = "Constraint lower bounds dimension is not consistent with "
         "number of constraints.";
      throw UtilityException(errmsg);
   }
   if (conUpperBounds.GetSize() != numUserConstraints)
   {
      errmsg = "Constraint upper bounds dimension is not consistent with "
         "number of constraints.";
      throw UtilityException(errmsg);
   }

   // Call cost function
   bool isNewX = true;
   Real costFunction;
   try
   {
      costFunction = userFunction->EvaluateCostFunc(numUserDecisionVars,
                                                    startingPoint, isNewX);
   }
   catch (const std::exception &e)
   {
      errmsg = "User Function ::EvaluateCostFunc() failed to evaluate.";
      throw UtilityException(errmsg);
   }

   // Call cost Jacobian and test dimensions
   isNewX = false;
   Rvector costJacobian;
   try
   {
      costJacobian = userFunction->EvaluateCostJac(numUserDecisionVars,
                                                   startingPoint, isNewX);
   }
   catch (const std::exception &e)
   {
      errmsg = "User Function ::EvaluateCostJac() failed to evaluate.";
      throw UtilityException(errmsg);
   }
   if (costJacobian.GetSize() != numUserDecisionVars)
   {
      errmsg = "Cost derivative dimension is not consistent with number of "
         "decison variables";
      throw UtilityException(errmsg);
   }

   // Call con functions
   Rvector conFunctions;
   try
   {
      conFunctions = userFunction->EvaluateConFunc(numUserDecisionVars,
         startingPoint, isNewX);
   }
   catch (const std::exception &e)
   {
      errmsg = "User Function ::EvaluateConFunc() faile to evaluate.";
      throw UtilityException(errmsg);
   }
   if (conFunctions.GetSize() != numUserConstraints)
   {
      errmsg = "Constraint vector dimension is not consistent with number of "
         "constraints.";
      throw UtilityException(errmsg);
   }

   // Call con Jacobian and test dimensions
   //Rmatrix conJacobian;
   Integer conJacRows;
   Integer conJacCols;
   try
   {
      userFunction->EvaluateConJacDimensions(numUserDecisionVars,
         startingPoint, isNewX, conJacRows, conJacCols);
   }
   catch (const std::exception &e)
   {
      errmsg = "User Function ::EvaluateConJac() failed to evaluate.";
      throw UtilityException(errmsg);
   }
   if (conJacCols != numUserDecisionVars ||
      conJacRows != numUserConstraints)
   {
      errmsg = "Constraint Jacobian dimension is no consistent with number of "
         "decision variables and constraints.";
      throw UtilityException(errmsg);
   }
}

//------------------------------------------------------------------------------
// void PrepareElasticMode(Real inputElasticWeight)
//------------------------------------------------------------------------------
/**
* Some initialization for elastic mode
*
* @param inputElasticWeight Elastic weight parameter
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::PrepareElasticMode(Real inputElasticWeight)
{
   isModeElastic = true;
   elasticWeight = inputElasticWeight;
   elasticVarVStartIdx = numUserDecisionVars;
   elasticVarVStopIdx = elasticVarVStartIdx + numUserConstraints - 1;
   elasticVarWStartIdx = elasticVarVStopIdx + 1;
   elasticVarWStopIdx = elasticVarWStartIdx + numUserConstraints - 1;
}

//------------------------------------------------------------------------------
// void PrepareArrays()
//------------------------------------------------------------------------------
/**
* Set up the various vectors and matrices to be used/modified later on
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::PrepareArrays()
{
   // Call the user function to get properties
   Integer unusedValue = 0;
   Rvector userVarLowerBounds;
   Rvector userVarUpperBounds;
   Rvector userConLowerBounds;
   Rvector userConUpperBounds;

   userFunction->GetBoundsInfo(unusedValue, unusedValue, userVarLowerBounds,
      userVarUpperBounds, userConLowerBounds, userConUpperBounds);

   // Configure the lower and upper variable bounds constraints
   // For variables that have that have user provided bounds.
   // If the constraint  bounts are - inf to inf, discard to avoid
   // numerical issues in matrix factorizations etc.
   if (userVarLowerBounds.GetSize() == 0)
   {
      userVarLowerBounds.SetSize(numUserDecisionVars);
      for (Integer i = 0; i < numUserDecisionVars; ++i)
         userVarLowerBounds[i] = -inf;
   }
   if (userVarUpperBounds.GetSize() == 0)
   {
      userVarUpperBounds.SetSize(numUserDecisionVars);
      for (Integer i = 0; i < numUserDecisionVars; ++i)
         userVarUpperBounds[i] = inf;
   }

   // Only apply bounds on variables that have them to avoid numerical issues.
   // So only retain bounds that are not -inf < var < inf.
   Integer userVarBoundConSize = 0;
   userVarBoundConIdxs.SetSize(0);
   for (Integer conIdx = 0; conIdx < numUserDecisionVars; ++conIdx)
   {
      if (userVarLowerBounds[conIdx] <= -9e299 &&
         userVarUpperBounds[conIdx] >= 9e299)
      {
      }
      else
      {
         ++userVarBoundConSize;
         userVarBoundConIdxs.Resize(userVarBoundConSize);
         userVarBoundConIdxs[userVarBoundConSize - 1] = conIdx;
      }
   }
   Rmatrix numVarIdent(numUserDecisionVars, numUserDecisionVars);
   for (Integer i = 0; i < numUserDecisionVars; ++i)
      numVarIdent(i, i) = 1.0;
   Rmatrix AMatBound(userVarBoundConSize, numUserDecisionVars);
   for (Integer i = 0; i < userVarBoundConSize; ++i)
   {
      for (Integer j = 0; j < numUserDecisionVars; ++j)
         AMatBound(i, j) = numVarIdent((Integer)userVarBoundConIdxs[i], j);
   }
   Rvector userNLPVarLowerBounds(userVarBoundConSize);
   Rvector userNLPVarUpperBounds(userVarBoundConSize);
   for (Integer i = 0; i < userVarBoundConSize; ++i)
   {
      userNLPVarLowerBounds[i] =
         userVarLowerBounds[(Integer)userVarBoundConIdxs[i]];
      userNLPVarUpperBounds[i] =
         userVarUpperBounds[(Integer)userVarBoundConIdxs[i]];
   }
   if (AMatBound.GetNumRows() != 0 && AMatBound.GetNumColumns() != 0)
      numBoundCons = AMatBound.GetNumRows();
   else
      numBoundCons = 0;
   boundAMatrix.SetSize(AMatBound.GetNumRows(), AMatBound.GetNumColumns());
   boundAMatrix = AMatBound;

   // Set variables based on the mode
   Rvector elasticVarLowerBounds;
   Rvector elasticVarUpperBounds;
   if (isModeElastic)
   {
      numElasticVars = 2*numUserConstraints;
      numElasticConstraints = 2*numUserConstraints;
      elasticVarLowerBounds.SetSize(numElasticVars);
      elasticVarUpperBounds.SetSize(numElasticVars);
      for (Integer i = 0; i < numElasticVars; ++i)
      {
         elasticVarLowerBounds[i] = 0;
         elasticVarUpperBounds[i] = FLT_MAX;
      }
   }
   else
   {
      numElasticVars = 0;
      numElasticConstraints = 0;
      elasticVarLowerBounds.SetSize(0);
      elasticVarUpperBounds.SetSize(0);
   }

   // Dimension the Jacobian arrays and fill in constant terms
   // associated with bound constraints and elastic variables
   Integer numRowsInJacobian =
      numUserConstraints + numBoundCons + numElasticVars;
   Integer numColsInConJacobian = numUserDecisionVars + numElasticVars;
   costJac.SetSize(numColsInConJacobian);
   for (Integer i = 0; i < numColsInConJacobian; ++i)
      costJac[i] = 0;
   conJac.SetSize(numRowsInJacobian, numColsInConJacobian);
   userConJacRowStartIdx = 0;
   userConJacRowStopIdx = numUserConstraints;
   userConJacColStartIdx = 0;
   userConJacColStopIdx = numUserDecisionVars;
   Integer rowStart = numUserConstraints;
   Integer rowStop = rowStart + AMatBound.GetNumRows();
   Integer colStart = 0;
   Integer colStop = numUserDecisionVars;
   for (Integer i = rowStart; i < rowStop; ++i)
   {
      for (Integer j = colStart; j < colStop; ++j)
         conJac(i, j) = AMatBound(i - rowStart, j - colStart);
   }

   // If we are in elastic mode, fill in parts of jacobians associated
   // with elastic variables
   if (isModeElastic)
   {
      Integer rowStartIdx = 0;
      Integer rowStopIdx = numUserConstraints;
      for (Integer i = rowStartIdx; i < rowStopIdx; ++i)
      {
         for (Integer j = elasticVarVStartIdx; j <= elasticVarVStopIdx; ++j)
         {
            if (i - rowStartIdx == j - elasticVarVStartIdx)
               conJac(i, j) = -1;
            else
               conJac(i, j) = 0;
         }
      }
      for (Integer i = rowStartIdx; i < rowStopIdx; ++i)
      {
         for (Integer j = elasticVarWStartIdx; j <= elasticVarWStopIdx; ++j)
         {
            if (i - rowStartIdx == j - elasticVarWStartIdx)
               conJac(i, j) = 1;
            else
               conJac(i, j) = 0;
         }
      }
      rowStartIdx = numUserConstraints + numBoundCons;
      rowStopIdx = rowStartIdx + numElasticVars;
      for (Integer i = rowStartIdx; i < rowStopIdx; ++i)
      {
         for (Integer j = elasticVarVStartIdx; j <= elasticVarWStopIdx; ++j)
         {
            if (i - rowStartIdx == j - elasticVarVStartIdx)
               conJac(i, j) = 1;
            else
               conJac(i, j) = 0;
         }
      }
   }

   // Fill in the NLP bounds arrays
   nlpConLowerBounds.SetSize(userConLowerBounds.GetSize() +
      userNLPVarLowerBounds.GetSize() + elasticVarLowerBounds.GetSize());
   for (Integer i = 0; i < nlpConLowerBounds.GetSize(); ++i)
   {
      if (i < userConLowerBounds.GetSize())
         nlpConLowerBounds[i] = userConLowerBounds[i];
      else if (i >= userConLowerBounds.GetSize() &&
         i < userNLPVarLowerBounds.GetSize() + userConLowerBounds.GetSize())
      {
         nlpConLowerBounds[i] =
            userNLPVarLowerBounds[i - userConLowerBounds.GetSize()];
      }
      else
      {
         nlpConLowerBounds[i] = elasticVarLowerBounds[i -
            (userNLPVarLowerBounds.GetSize() + userConLowerBounds.GetSize())];
      }
   }

   nlpConUpperBounds.SetSize(userConUpperBounds.GetSize() +
      userNLPVarUpperBounds.GetSize() + elasticVarUpperBounds.GetSize());
   for (Integer i = 0; i < nlpConUpperBounds.GetSize(); ++i)
   {
      if (i < userConUpperBounds.GetSize())
         nlpConUpperBounds[i] = userConUpperBounds[i];
      else if (i >= userConUpperBounds.GetSize() &&
         i < userNLPVarUpperBounds.GetSize() + userConUpperBounds.GetSize())
      {
         nlpConUpperBounds[i] =
            userNLPVarUpperBounds[i - userConUpperBounds.GetSize()];
      }
      else
      {
         nlpConUpperBounds[i] = elasticVarUpperBounds[i -
            (userNLPVarUpperBounds.GetSize() + userConUpperBounds.GetSize())];
      }
   }

   nlpVarLowerBounds.SetSize(userVarLowerBounds.GetSize() +
      elasticVarLowerBounds.GetSize());
   for (Integer i = 0; i < nlpVarLowerBounds.GetSize(); ++i)
   {
      if (i < userVarLowerBounds.GetSize())
         nlpVarLowerBounds[i] = userVarLowerBounds[i];
      else
      {
         nlpVarLowerBounds[i] =
            elasticVarLowerBounds[i - userVarLowerBounds.GetSize()];
      }
   }

   nlpVarUpperBounds.SetSize(userVarUpperBounds.GetSize() +
      elasticVarUpperBounds.GetSize());
   for (Integer i = 0; i < nlpVarUpperBounds.GetSize(); ++i)
   {
      if (i < userVarUpperBounds.GetSize())
         nlpVarUpperBounds[i] = userVarUpperBounds[i];
      else
      {
         nlpVarUpperBounds[i] =
            elasticVarUpperBounds[i - userVarUpperBounds.GetSize()];
      }
   }
}

//-------------------------------------
// User Function Interfaces
//-------------------------------------

//------------------------------------------------------------------------------
// void EvaluateCostAndJac(Integer numVars, Rvector decVector, bool isNewX,
//                         Real &fun, Rvector &funJac, Integer &numFunEvals)
//------------------------------------------------------------------------------
/**
* Determine the cost function and cost Jacobian from the provided user object
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param fun The cost function value
* @param funJac The cost Jacobian
* @param numFunEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateCostAndJac(Integer numVars,
                                              Rvector decVector, bool isNewX,
                                              Real &fun, Rvector &funJac, 
                                              Integer &numFunEvals)
{
   fun = userFunction->EvaluateCostFunc(numVars, decVector, isNewX);
   funJac = userFunction->EvaluateCostJac(numVars, decVector, isNewX);
   numFunEvals = 1;
}

//------------------------------------------------------------------------------
// void EvaluateConAndJac(Integer numVars, Rvector decVector, bool isNewX,
//                        Rvector &conFunc, Rmatrix &conJac, 
//                        Integer &numFunEvals)
//------------------------------------------------------------------------------
/**
* Determine the constraint functions and constraint Jacobian from the provided
* user object
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param conFunc The constraint functions
* @param conJac The constraint Jacobian
* @param numCostEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateConAndJac(Integer numVars, 
                                             Rvector decVector, bool isNewX,
                                             Rvector &conFunc, Rmatrix &conJac, 
                                             Integer &numFunEvals)
{
   conFunc = userFunction->EvaluateConFunc(numVars, decVector, isNewX);
   conJac = userFunction->EvaluateConJac(numVars, decVector, isNewX);
   numFunEvals = 1;
}

//------------------------------------------------------------------------------
// void EvaluateAllFunJac(Integer numVars, Rvector decVector, bool isNewX,
//                        Real &fun, Rvector &funJac, Integer &numFunEvals,
//                        Rvector &conFunc, Rmatrix &conJac, Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Determine constraint and cost functions/Jacobians
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param fun The cost function value
* @param funJac The cost Jacobian
* @param numFunEvals
* @param conFunc The constraint functions
* @param conJac The constraint Jacobian
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateAllFunJac(Integer numVars, 
                                             Rvector decVector, bool isNewX,
                                             Real &fun, Rvector &funJac,
                                             Integer &numFunEvals,
                                             Rvector &conFunc, Rmatrix &conJac,
                                             Integer &numGEvals)
{
   EvaluateCostAndJac(numVars, decVector, isNewX, fun, funJac, numFunEvals);
   EvaluateConAndJac(numVars, decVector, false, conFunc, conJac, numGEvals);
}

//------------------------------------------------------------------------------
// void EvaluateFuncsOnly(Integer numVars, Rvector decVector, bool isNewX,
//                        Real &fun, Integer &numFunEvals, Rvector &conFunc, 
//                        Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Determine constraint and cost functions
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param fun The cost function value
* @param numFunEvals
* @param conFunc The constraint functions
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateFuncsOnly(Integer numVars,
                                             Rvector decVector, bool isNewX,
                                             Real &fun, Integer &numFunEvals, 
                                             Rvector &conFunc,
                                             Integer &numGEvals)
{
   fun = userFunction->EvaluateCostFunc(numVars, decVector, isNewX);
   Rvector userDecVector(numUserDecisionVars);
   for (Integer i = 0; i < userDecVector.GetSize(); ++i)
      userDecVector[i] = decVector[i];
   conFunc = userFunction->EvaluateConFunc(numVars, userDecVector, false);
   numFunEvals = 1;
   numGEvals = 1;
}

//------------------------------------------------------------------------------
// void EvaluateDerivsOnly(Integer numVars, Rvector decVector, bool isNewX,
//                         Rvector &costJac, Rmatrix &conJac, 
//                         Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Determine constraint and cost Jacobians
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param costJac The cost Jacobian
* @param numCostEvals
* @param conJac The constraint Jacobian
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateDerivsOnly(Integer numVars,
   Rvector decVector, bool isNewX, Rvector &costJac, Rmatrix &conJac, 
   Integer &numGEvals)
{
   costJac = userFunction->EvaluateCostJac(numVars, decVector, isNewX);
   conJac = userFunction->EvaluateConJac(numVars, decVector, false);
   numGEvals = 1;
}

//-------------------------------------
// NLP Function interfaces
//-------------------------------------

//------------------------------------------------------------------------------
// void GetNLPInfo(Integer &numVars, Integer &numCons)
//------------------------------------------------------------------------------
/**
* Return the number of variables and constraints
*
* @param numVars The number of variables
* @param numCons The number of constraints
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::GetNLPInfo(Integer &numVars, Integer &numCons)
{
   numVars = numUserDecisionVars + numElasticVars;
   numCons = numUserConstraints + numBoundCons + numElasticConstraints;
}

//------------------------------------------------------------------------------
// void GetNLPBoundsInfo(Rvector &varLowerBounds, Rvector &varUpperBounds,
//                       Rvector &conLowerBounds, Rvector &conUpperBounds)
//------------------------------------------------------------------------------
/**
* Returns the bounds on the variables and constraints
*
* @param varLowerBounds Variable lower bounds
* @param varUpperBounds Variable upper bounds
* @param conLowerBounds Constraint lower bounds
* @param conUpperBounds Constraint upper bounds
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::GetNLPBoundsInfo(Rvector &varLowerBounds,
                                            Rvector &varUpperBounds,
                                            Rvector &conLowerBounds,
                                            Rvector &conUpperBounds)
{
   varLowerBounds.SetSize(nlpVarLowerBounds.GetSize());
   varLowerBounds = nlpVarLowerBounds;
   varUpperBounds.SetSize(nlpVarUpperBounds.GetSize());
   varUpperBounds = nlpVarUpperBounds;
   conLowerBounds.SetSize(nlpConLowerBounds.GetSize());
   conLowerBounds = nlpConLowerBounds;
   conUpperBounds.SetSize(nlpConUpperBounds.GetSize());
   conUpperBounds = nlpConUpperBounds;
}

//------------------------------------------------------------------------------
// Rvector GetNLPStartingPoint()
//------------------------------------------------------------------------------
/**
* Get the initial guess from the chosen test case
*
* @return initGuess Initial decision vector
*/
//------------------------------------------------------------------------------
Rvector NLPFunctionGenerator::GetNLPStartingPoint()
{
   Rvector initGuess;

   if (!isModeElastic)
      initGuess = userFunction->GetStartingPoint();
   else
   {
      initGuess = userFunction->GetStartingPoint();
      initGuess.Resize(initGuess.GetSize() + numElasticVars);
      for (Integer i = initGuess.GetSize() - numElasticVars;
         i < initGuess.GetSize(); ++i)
         initGuess[i] = 1.0;
   }

   return initGuess;
}

//------------------------------------------------------------------------------
// void EvaluateNLPFunctionsOnly(Integer numVars, Rvector decVector,
//                               bool isNewX, Real &fun, Integer &numFunEvals,
//                               Rvector &conFunc, Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Evaluate the NLP cost function and constraint functions
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param fun The cost function value
* @param numFunEvals
* @param conFunc The constraint functions
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateNLPFunctionsOnly(Integer numVars,
                                                    Rvector decVector,
                                                    bool isNewX, Real &fun,
                                                    Integer &numFunEvals, 
                                                    Rvector &conFunc,
                                                    Integer &numGEvals)
{
   Rvector userDecVector(numUserDecisionVars);
   for (Integer i = 0; i < userDecVector.GetSize(); ++i)
      userDecVector[i] = decVector[i];
   Rvector userConFunc;
   EvaluateFuncsOnly(numVars, userDecVector, isNewX, fun, numFunEvals,
      userConFunc, numGEvals);

   if (isModeElastic)
   {
      userConFunc =
         userConFunc - GetElasticV(decVector) + GetElasticW(decVector);
      Rvector sumVector = GetElasticV(decVector) + GetElasticW(decVector);
      Real sum = 0;
      for (Integer i = 0; i < sumVector.GetSize(); ++i)
         sum += sumVector[i];
      fun += elasticWeight*sum;
   }
   conFunc.SetSize(userConFunc.GetSize() + userVarBoundConIdxs.GetSize());
   for (Integer i = 0; i < conFunc.GetSize(); ++i)
   {
      if (i < userConFunc.GetSize())
         conFunc[i] = userConFunc[i];
      else
      {
         conFunc[i] =
            decVector[(Integer)userVarBoundConIdxs[i - userConFunc.GetSize()]];
      }
   }
   if (isModeElastic)
   {
      Integer currentConSize = conFunc.GetSize();
      conFunc.Resize(conFunc.GetSize() + elasticVarWStopIdx -
         elasticVarVStartIdx + 1);
      Integer decVecIdx = elasticVarVStartIdx;
      for (Integer i = currentConSize; i < conFunc.GetSize(); ++i)
      {
         conFunc[i] = decVector[decVecIdx];
         ++decVecIdx;
      }
   }
}

//------------------------------------------------------------------------------
// void EvaluateNLPDerivsOnly(Integer numVars, Rvector decVector, bool isNewX,
//                            Rvector &nlpCostJac, Rmatrix &nlpConJac,
//                            Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Evaluate the NLP cost function and constraint Jacobians
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param nlpCostJac The cost Jacobian
* @param nlpConJac The constraint Jacobian
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateNLPDerivsOnly(Integer numVars,
                                                 Rvector decVector,
                                                 bool isNewX,
                                                 Rvector &nlpCostJac,
                                                 Rmatrix &nlpConJac,
                                                 Integer &numGEvals)
{
   Rvector userDecVector(numUserDecisionVars);
   for (Integer i = 0; i < userDecVector.GetSize(); ++i)
      userDecVector[i] = decVector[i];
   Rvector userCostJac =
      userFunction->EvaluateCostJac(numVars, userDecVector, isNewX);
   Rmatrix userConJac =
      userFunction->EvaluateConJac(numVars, userDecVector, false);
   InsertUserConJac(userConJac);
   if (isModeElastic)
   {
      InsertUserCostJac(userCostJac);
      UpdateElasticCostJacobian();
      nlpCostJac.SetSize(costJac.GetSize());
      nlpCostJac = costJac;
   }
   else
   {
      nlpCostJac.SetSize(userCostJac.GetSize());
      nlpCostJac = userCostJac;
   }
   nlpConJac.SetSize(conJac.GetNumRows(), conJac.GetNumColumns());
   nlpConJac = conJac;
   numGEvals = 1;
}

//------------------------------------------------------------------------------
// void EvaluateAllNLPFuncJac(Integer numVars, Rvector decVector, bool isNewX,
//                            Real &nlpCost, Rvector &nlpCostJac,
//                            Integer &numCostEvals, Rvector &nlpConFunc,
//                            Rmatrix &nlpConJac, Integer &numGEvals)
//------------------------------------------------------------------------------
/**
* Evaluate all the NLP cost and constraint functions/Jacobians
*
* @param numVars The number of variables
* @param decVector The decision vector
* @param isNewX Boolean showing whether or not new variables are being input
* @param nlpCost The cost function value
* @param nlpCostJac The cost Jacobian
* @param numCostEvals
* @param nlpConFunc The constraint functions
* @param nlpConJac The constraint Jacobian
* @param numGEvals
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::EvaluateAllNLPFuncJac(Integer numVars,
                                                 Rvector decVector,
                                                 bool isNewX, Real &nlpCost,
                                                 Rvector &nlpCostJac,
                                                 Integer &numCostEvals,
                                                 Rvector &nlpConFunc,
                                                 Rmatrix &nlpConJac,
                                                 Integer &numGEvals)
{
   Integer unusedValue;
   EvaluateNLPFunctionsOnly(numVars, decVector, isNewX, nlpCost, numCostEvals,
      nlpConFunc, unusedValue);
   EvaluateNLPDerivsOnly(numVars, decVector, isNewX, nlpCostJac, nlpConJac,
      numGEvals);
}

//------------------------------------------------------------------------------
// void InsertUserConJac(Rmatrix userConJac)
//------------------------------------------------------------------------------
/**
* Inserts the users constraint Jacobian into the total NLP con Jacobian
* 
* @param userConJac The user constraint Jacobian
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::InsertUserConJac(Rmatrix userConJac)
{
   for (Integer i = userConJacRowStartIdx; i < userConJacRowStopIdx; ++i)
   {
      for (Integer j = userConJacColStartIdx; j < userConJacColStopIdx; ++j)
      {
         conJac(i, j) =
            userConJac(i - userConJacRowStartIdx, j - userConJacColStartIdx);
      }
   }
}

//------------------------------------------------------------------------------
// void InsertUserCostJac(Rvector userCostJac)
//------------------------------------------------------------------------------
/**
* Inserts the user cost Jacobian into the total NLP cost Jacobian
*
* @param userConJac The user cost Jacobian
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::InsertUserCostJac(Rvector userCostJac)
{
   for (Integer i = 0; i < numUserDecisionVars; ++i)
      costJac[i] = userCostJac[i];
}

//------------------------------------------------------------------------------
// void UpdateElasticCostJacobian()
//------------------------------------------------------------------------------
/**
* Adds in the portion of the cost Jacobian due to elastic vars and weight
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::UpdateElasticCostJacobian()
{
   for (Integer i = elasticVarVStartIdx; i <= elasticVarWStopIdx; ++i)
      costJac[i] = elasticWeight;
}

//------------------------------------------------------------------------------
// Integer GetNumUserDecisionVars()
//------------------------------------------------------------------------------
/**
* Get the number of user decision variables
*
* @return numUserDecisionVars The number of user decision variables
*/
//------------------------------------------------------------------------------
Integer NLPFunctionGenerator::GetNumUserDecisionVars()
{
   return numUserDecisionVars;
}

//------------------------------------------------------------------------------
// Integer GetNumUserConstraints()
//------------------------------------------------------------------------------
/**
* Get the number of user constraints
*
* @return numUserConstraints The number of user constraints
*/
//------------------------------------------------------------------------------
Integer NLPFunctionGenerator::GetNumUserConstraints()
{
   return numUserConstraints;
}

//------------------------------------------------------------------------------
// Integer GetNumElasticVars()
//------------------------------------------------------------------------------
/**
* Get the number of elastic variables
*
* @return numElasticVars The number of elastic variables
*/
//------------------------------------------------------------------------------
Integer NLPFunctionGenerator::GetNumElasticVars()
{
   return numElasticVars;
}

//------------------------------------------------------------------------------
// Rvector GetElasticV(Rvector decVector)
//------------------------------------------------------------------------------
/**
* Returns the "V" elastic mode variables
*
* @param decVector The decision vector
*
* @return elasticV The "V" elastic mode variables
*/
//------------------------------------------------------------------------------
Rvector NLPFunctionGenerator::GetElasticV(Rvector decVector)
{
   Rvector elasticV;
   if (isModeElastic)
   {
      elasticV.SetSize(elasticVarVStopIdx - elasticVarVStartIdx + 1);
      for (Integer i = 0; i < elasticV.GetSize(); ++i)
         elasticV[i] = decVector[i + elasticVarVStartIdx];
   }
   else
   {
      std::string errmsg = "Cannot compute elasticV because the problem is not"
         " in elastic mode";
      throw UtilityException(errmsg);
   }

   return elasticV;
}

//------------------------------------------------------------------------------
// Rvector GetElasticW(Rvector decVector)
//------------------------------------------------------------------------------
/**
* Returns the "W" elastic mode variables
*
* @param decVector The decision vector
*
* @return elasticW The "W" elastic mode variables
*/
//------------------------------------------------------------------------------
Rvector NLPFunctionGenerator::GetElasticW(Rvector decVector)
{
   Rvector elasticW;
   if (isModeElastic)
   {
      elasticW.SetSize(elasticVarWStopIdx - elasticVarWStartIdx + 1);
      for (Integer i = 0; i < elasticW.GetSize(); ++i)
         elasticW[i] = decVector[i + elasticVarWStartIdx];
   }
   else
   {
      std::string errmsg = "Cannot compute elasticW because the problem is not"
         " in elastic mode";
      throw UtilityException(errmsg);
   }

   return elasticW;
}

//------------------------------------------------------------------------------
// Real GetMaxElasticVar(Rvector decVector)
//------------------------------------------------------------------------------
/**
* Returns the maximum elastic variable
*
* @param decVector The decision vector
*
* @return maxElasticVar The maximum elastic variable
*/
//------------------------------------------------------------------------------
Real NLPFunctionGenerator::GetMaxElasticVar(Rvector decVector)
{
   Real maxElasticVar = GmatMathUtil::Abs(decVector[elasticVarVStartIdx]);
   for (Integer i = elasticVarVStartIdx + 1; i < elasticVarWStopIdx; ++i)
   {
      if (GmatMathUtil::Abs(decVector[i]) > maxElasticVar)
         maxElasticVar = GmatMathUtil::Abs(decVector[i]);
   }

   return maxElasticVar;
}

//------------------------------------------------------------------------------
// void SetElasticWeight(Real elasticWeightInput)
//------------------------------------------------------------------------------
/**
* Returns the maximum elastic variable
*
* @param elasticWeightInput The elastic weight value to set
*/
//------------------------------------------------------------------------------
void NLPFunctionGenerator::SetElasticWeight(Real elasticWeightInput)
{
   elasticWeight = elasticWeightInput;
   UpdateElasticCostJacobian();
}

//------------------------------------------------------------------------------
// Rvector MaxUserConViolation(Rvector conViolation, Rvector decVector)
//------------------------------------------------------------------------------
/**
* Takes constraints with elastic shift and removes it (results in user
* constraint values being returned)
*
* @param conViolation The current constraint functions
* @param decVector The decision vector
*
* @return conViolation The user constraint values
*/
//------------------------------------------------------------------------------
Real NLPFunctionGenerator::MaxUserConViolation(Rvector conViolation,
                                                  Rvector decVector)
{
   Rvector conViolationCopy = conViolation;
   conViolation.SetSize(numUserConstraints);
   for (Integer i = 0; i < numUserConstraints; ++i)
      conViolation[i] = conViolationCopy[i];
   conViolation += (GetElasticV(decVector) - GetElasticW(decVector));
   return conViolation[0];
}

//------------------------------------------------------------------------------
// Real ShiftCost(Real cost, Rvector decVector)
//------------------------------------------------------------------------------
/**
* Takes cost with elastic shift and removes it (results in user cost value
* being returned)
*
* @param cost The current cost function value
* @param decVector The decision vector
*
* @return cost The user cost value
*/
//------------------------------------------------------------------------------
Real NLPFunctionGenerator::ShiftCost(Real cost, Rvector decVector)
{
   Rvector elasticVW = GetElasticV(decVector) + GetElasticW(decVector);
   Real elasticSum = 0;
   for (Integer i = 0; i < elasticVW.GetSize(); ++i)
      elasticSum += elasticVW[i];
   cost -= elasticWeight*elasticSum;
   return cost;
}

//------------------------------------------------------------------------------
// Rvector EvaluateMaxVarStep()
//------------------------------------------------------------------------------
/**
* Method to generate a vector of max step sizes each variable in the decision
* vector can take.  The original real std::vector is converted to an Rvector.
*
* @return retMaxVarStep The vector of maximum step sizes
*/
//------------------------------------------------------------------------------
Rvector NLPFunctionGenerator::EvaluateMaxVarStep()
{
   std::vector <Real> maxVarStep = userFunction->GetMaxVarStepSize();
   Rvector retMaxVarStep(maxVarStep.size());
   for (Integer i = 0; i < retMaxVarStep.GetSize(); ++i)
      retMaxVarStep[i] = maxVarStep[i];

   return retMaxVarStep;
}
