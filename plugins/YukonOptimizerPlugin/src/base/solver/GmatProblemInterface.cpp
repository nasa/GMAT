//$Id$
//------------------------------------------------------------------------------
//                              GmatProblemInterface
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
// Created: 2017/10/30, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Defines GmatProblemInterface methods, derived from MiNLPUserProblem.
 * Interface is used to collect required optimizer data from other sources in
 * Gmat.
 */
//------------------------------------------------------------------------------

#include "GmatProblemInterface.hpp"
#include "Yukonad.hpp"

//------------------------------------------------------------------------------
// GmatProblemInterface()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
GmatProblemInterface::GmatProblemInterface()
{
   conFunctions.SetSize(0);
}

//------------------------------------------------------------------------------
// void GetNLPInfo()
//------------------------------------------------------------------------------
/**
* Method to get the total number of variables and constraints being used
*
* @param totalNumVars Total number of variables
* @param totalNumCons Total number of constraints
*/
//------------------------------------------------------------------------------
void GmatProblemInterface::GetNLPInfo(Integer &totalNumVars,
                                      Integer &totalNumCons)
{
   totalNumVars = optimizerData->registeredVariableCount;
   totalNumCons = optimizerData->registeredComponentCount;
}

//------------------------------------------------------------------------------
// Rvector GetStartingPoint()
//------------------------------------------------------------------------------
/**
* Returns the initial variables
*
* @return initalDecVec The initial decision vector
*/
//------------------------------------------------------------------------------
Rvector GmatProblemInterface::GetStartingPoint()
{
   Rvector initialDecVec = optimizerData->variable;
   return initialDecVec;
}

//------------------------------------------------------------------------------
// void GetBoundsInfo()
//------------------------------------------------------------------------------
/**
* Returns the bounds info for variables and constraints
*
* @param numVars Number of variables
* @param numCons Number of constraints
* @param varLowerBounds Vector of lower bounds for variables
* @param varUpperBounds Vector of upper bounds for variables
* @param conLB Vector of lower bounds for constraints
* @param conUB Vector of upper bounds for constraints
*/
//------------------------------------------------------------------------------
void GmatProblemInterface::GetBoundsInfo(Integer numVars, Integer numCons,
                                         Rvector &varLowerBounds,
                                         Rvector &varUpperBounds,
                                         Rvector &conLB, Rvector &conUB)
{
   varLowerBounds.SetSize(optimizerData->variableMinimum.size());
   varLowerBounds = optimizerData->variableMinimum;
   varUpperBounds.SetSize(optimizerData->variableMaximum.size());
   varUpperBounds = optimizerData->variableMaximum;
   conLB.SetSize(optimizerData->eqConstraintCount +
      optimizerData->ineqConstraintCount);
   conUB.SetSize(optimizerData->eqConstraintCount +
      optimizerData->ineqConstraintCount);
   for (Integer i = 0; i < conLB.GetSize(); ++i)
   {
      if (i < optimizerData->eqConstraintCount)
      {
         conLB[i] = optimizerData->eqConstraintDesiredValues[i];
         conUB[i] = optimizerData->eqConstraintDesiredValues[i];
      }
      else
      {
         conLB[i] = -inf;
         conUB[i] = inf;
         if (optimizerData->ineqConstraintOp[i -
            optimizerData->eqConstraintCount] == 1)
         {
            conLB[i] = optimizerData->ineqConstraintDesiredValues[i -
               optimizerData->eqConstraintCount];
         }
         else if (optimizerData->ineqConstraintOp[i -
            optimizerData->eqConstraintCount] == -1)
         {
            conUB[i] = optimizerData->ineqConstraintDesiredValues[i -
               optimizerData->eqConstraintCount];
         }
      }
   }

   conLowerBounds = conLB;
   conUpperBounds = conUB;
}

//------------------------------------------------------------------------------
// Real EvaluateCostFunc()
//------------------------------------------------------------------------------
/**
* Returns the value of the cost function
*
* @param numVars Number of variables
* @param decVector The decision vector
* @param isNewX Boolian representing whether a new decision vector is being 
*        used
*
* @return cost Current cost value
*/
//------------------------------------------------------------------------------
Real GmatProblemInterface::EvaluateCostFunc(Integer numVars, Rvector decVector,
                                            bool isNewX)
{
   return optimizerData->cost;
}

//------------------------------------------------------------------------------
// Rvector EvaluateCostJac()
//------------------------------------------------------------------------------
/**
* Returns the gradient of the cost function as a vector
*
* @param numVars Number of variables
* @param decVector The decision vector
* @param isNewX Boolian representing whether a new decision vector is being
*        used
*
* @return costJacobian Current cost gradient
*/
//------------------------------------------------------------------------------
Rvector GmatProblemInterface::EvaluateCostJac(Integer numVars,
                                              Rvector decVector, bool isNewX)
{
   Rvector costJacobian(optimizerData->variableCount);
   for (Integer i = 0; i < costJacobian.GetSize(); ++i)
   {
      costJacobian[i] = optimizerData->gradient[i];
   }


   return costJacobian;
}

//------------------------------------------------------------------------------
// Rvector EvaluateConFunc()
//------------------------------------------------------------------------------
/**
* Returns the value of the constraint functions
*
* @param numVars Number of variables
* @param decVector The decision vector
* @param isNewX Boolian representing whether a new decision vector is being
*        used
*
* @return conFuncs Current constraint values
*/
//------------------------------------------------------------------------------
Rvector GmatProblemInterface::EvaluateConFunc(Integer numVars,
                                              Rvector decVector, bool isNewX)
{   
   return conFunctions;
}

//------------------------------------------------------------------------------
// Rmatrix EvaluateConJac()
//------------------------------------------------------------------------------
/**
* Returns the value of the constraint Jacobian as a matrix
*
* @param numVars Number of variables
* @param decVector The decision vector
* @param isNewX Boolian representing whether a new decision vector is being
*        used
*
* @return cost Current constraint Jacobian
*/
//------------------------------------------------------------------------------
Rmatrix GmatProblemInterface::EvaluateConJac(Integer numVars,
                                             Rvector decVector, bool isNewX)
{
   Rmatrix conJac(optimizerData->eqConstraintCount +
      optimizerData->ineqConstraintCount, optimizerData->variableCount);

   for (Integer i = 0; i < optimizerData->variableCount; ++i)
   {
      for (Integer j = 0; j < optimizerData->eqConstraintCount +
         optimizerData->ineqConstraintCount; ++j)
      {
         if (j < optimizerData->eqConstraintCount)
         {
            conJac(j, i) =
               optimizerData->jacobian.at(i + optimizerData->variableCount * j);
         }
         else
         {
            if (conLowerBounds[j] > -inf)
            {
               conJac(j, i) =
                  -optimizerData->jacobian.at(i + optimizerData->variableCount * j);
            }
            else
            {
               conJac(j, i) =
                  optimizerData->jacobian.at(i + optimizerData->variableCount * j);
            }
         }
      }
   }

   return conJac;
}

//------------------------------------------------------------------------------
// void EvaluateConJacDimensions()
//------------------------------------------------------------------------------
/**
* Returns the dimensions of the constraint Jacobian
*
* @param numVariables Number of variables
* @param decVector The decision vector
* @param isNewX Boolian representing whether a new decision vector is being
*        used
* @param rowCount The number of rows in constraint Jacobian
* @param colCount The number of columns in constraint Jacobian
*/
//------------------------------------------------------------------------------
void GmatProblemInterface::EvaluateConJacDimensions(Integer numVariables,
                                                    Rvector decVector,
                                                    bool isNewX,
                                                    Integer &rowCount,
                                                    Integer &colCount)
{
   rowCount = optimizerData->eqConstraintCount + optimizerData->ineqConstraintCount;
   colCount = optimizerData->variableCount;
}

//------------------------------------------------------------------------------
// std::vector <Real> GetMaxVarStepSize()
//------------------------------------------------------------------------------
/**
* Returns the max step size a variable can take during the optimization
*
* @return variableMaximumStep The maximum step a variable can take
*/
//------------------------------------------------------------------------------
std::vector <Real> GmatProblemInterface::GetMaxVarStepSize()
{
   return optimizerData->variableMaximumStep;
}

//------------------------------------------------------------------------------
// void SetPointerToOptimizer()
//------------------------------------------------------------------------------
/**
* Sets the optimizerData pointer to point to the current optimizer so its
* parameters can be accessed for evalautions to be sent to other pieces of the
* optimizer
*/
//------------------------------------------------------------------------------
void GmatProblemInterface::SetPointerToOptimizer(Yukonad *inputDataPointer)
{
   optimizerData = inputDataPointer;
}

//------------------------------------------------------------------------------
// void SetConFunction()
//------------------------------------------------------------------------------
/**
* Updates the conFunctions vector to the current values of the constraint
* functions
*/
//------------------------------------------------------------------------------
void GmatProblemInterface::SetConFunction(Integer conIdx, Real value,
                                       const std::string &conType)
{
   if (conFunctions.GetSize() == 0)
   {
      conFunctions.SetSize(optimizerData->eqConstraintCount +
         optimizerData->ineqConstraintCount);
   }

   if (conType == "EqCon")
      conFunctions[conIdx] = value;
   else
      conFunctions[conIdx + optimizerData->eqConstraintCount] = value;
}