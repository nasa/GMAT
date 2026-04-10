//------------------------------------------------------------------------------
//                              ImplicitRK
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
// Author: Youngkwang Kim
// Created: 2017.01.24
//
/**
* From original MATLAB prototype:
*  Author: S. Hughes.  steven.p.hughes@nasa.gov
*
* Explanation goes here.
*
*/
//------------------------------------------------------------------------------
#include "ImplicitRungeKutta.hpp"
//#define DEBUG

// the followings are of cpp
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// ImplicitRungeKutta()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta::ImplicitRungeKutta() :
   numDefectCons                (0),
   numPointsPerStep             (0),
   numStagePointsPerMesh        (0),
   numStateStagePointsPerMesh   (0),
   numControlStagePointsPerMesh (0)
{
   // vectors and matrices are zeros be default
}

//------------------------------------------------------------------------------
// ImplicitRungeKutta(const ImplicitRungeKutta &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 * @param <copy> object to copy
 *
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta::ImplicitRungeKutta(const ImplicitRungeKutta &copy)
{
   numDefectCons = copy.numDefectCons;
   numPointsPerStep = copy.numPointsPerStep;
   numStagePointsPerMesh = copy.numStagePointsPerMesh;
   numStateStagePointsPerMesh = copy.numStateStagePointsPerMesh;
   numControlStagePointsPerMesh = copy.numControlStagePointsPerMesh;

   Integer sz = (Integer)copy.rhoVec.GetSize();
   rhoVec.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      rhoVec(ii) = copy.rhoVec(ii);

   sz = (Integer)copy.betaVec.GetSize();
   betaVec.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      betaVec(ii) = copy.betaVec(ii);

   sz = (Integer)copy.stageTimes.GetSize();
   stageTimes.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      stageTimes(ii) = copy.stageTimes(ii);

   Integer sz1 = copy.sigmaMatrix.GetNumRows();
   Integer sz2 = copy.sigmaMatrix.GetNumColumns();
   sigmaMatrix.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         sigmaMatrix(ii, jj) = copy.sigmaMatrix(ii, jj);
   }

   sz1 = copy.paramDepArray.GetNumRows();
   sz2 = copy.paramDepArray.GetNumColumns();
   paramDepArray.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         paramDepArray(ii, jj) = copy.paramDepArray(ii, jj);
   }


   sz1 = copy.funcConstArray.GetNumRows();
   sz2 = copy.funcConstArray.GetNumColumns();
   funcConstArray.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         funcConstArray(ii, jj) = copy.funcConstArray(ii, jj);
   }

   sz1 = copy.patternAMat.GetNumRows();
   sz2 = copy.patternAMat.GetNumColumns();
   patternAMat.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         patternAMat(ii, jj) = copy.patternAMat(ii, jj);
   }

   sz1 = copy.patternBMat.GetNumRows();
   sz2 = copy.patternBMat.GetNumColumns();
   patternBMat.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         patternBMat(ii, jj) = copy.patternBMat(ii, jj);
   }
}

//------------------------------------------------------------------------------
// ImplicitRungeKutta& operator=(const ImplicitRungeKutta &copy)
//------------------------------------------------------------------------------
/**
 * operator =
 *
 * @param <copy> object to copy
 *
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta& ImplicitRungeKutta::operator=(
                                        const ImplicitRungeKutta &copy)
{
   if (&copy == this)
      return *this;

   numDefectCons = copy.numDefectCons;
   numPointsPerStep = copy.numPointsPerStep;
   numStagePointsPerMesh = copy.numStagePointsPerMesh;
   numStateStagePointsPerMesh = copy.numStateStagePointsPerMesh;
   numControlStagePointsPerMesh = copy.numControlStagePointsPerMesh;

   Integer sz = (Integer)copy.rhoVec.GetSize();
   rhoVec.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      rhoVec(ii) = copy.rhoVec(ii);

   sz = (Integer)copy.betaVec.GetSize();
   betaVec.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      betaVec(ii) = copy.betaVec(ii);

   sz = (Integer)copy.stageTimes.GetSize();
   stageTimes.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
      stageTimes(ii) = copy.stageTimes(ii);

   Integer sz1 = copy.sigmaMatrix.GetNumRows();
   Integer sz2 = copy.sigmaMatrix.GetNumColumns();
   sigmaMatrix.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         sigmaMatrix(ii, jj) = copy.sigmaMatrix(ii, jj);
   }

   sz1 = copy.paramDepArray.GetNumRows();
   sz2 = copy.paramDepArray.GetNumColumns();
   paramDepArray.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         paramDepArray(ii, jj) = copy.paramDepArray(ii, jj);
   }


   sz1 = copy.funcConstArray.GetNumRows();
   sz2 = copy.funcConstArray.GetNumColumns();
   funcConstArray.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         funcConstArray(ii, jj) = copy.funcConstArray(ii, jj);
   }

   sz1 = copy.patternAMat.GetNumRows();
   sz2 = copy.patternAMat.GetNumColumns();
   patternAMat.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         patternAMat(ii, jj) = copy.patternAMat(ii, jj);
   }

   sz1 = copy.patternBMat.GetNumRows();
   sz2 = copy.patternBMat.GetNumColumns();
   patternBMat.SetSize(sz1, sz2);
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
         patternBMat(ii, jj) = copy.patternBMat(ii, jj);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~ImplicitRungeKutta()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta::~ImplicitRungeKutta()
{
   // do nothing
}

//------------------------------------------------------------------------------
// Rvector GetQuadratureWeights()
//------------------------------------------------------------------------------
/**
 * This method returns the vector of quadrature weights
 *
 * @return  vector of quadrature weights
 *
 */
//------------------------------------------------------------------------------
Rvector ImplicitRungeKutta::GetQuadratureWeights()
{
   return betaVec;
}


//------------------------------------------------------------------------------
// Integer GetNumStagePointsPerMesh()
//------------------------------------------------------------------------------
/**
 * This method returns the number of stage points per mesh
 *
 * @return  number of stage points per mesh
 *
 */
//------------------------------------------------------------------------------
Integer ImplicitRungeKutta::GetNumStagePointsPerMesh()
{
   return numStagePointsPerMesh;
}

//------------------------------------------------------------------------------
// Integer GetNumStateStagePointsPerMesh()
//------------------------------------------------------------------------------
/**
 * This method returns the number of state stage points per mesh
 *
 * @return  number of state stage points per mesh
 *
 */
//------------------------------------------------------------------------------
Integer ImplicitRungeKutta::GetNumStateStagePointsPerMesh()
{
   return numStateStagePointsPerMesh;
}

//------------------------------------------------------------------------------
// Integer GetNumControlStagePointsPerMesh()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control stage points per mesh
 *
 * @return  number of control stage points per mesh
 *
 */
//------------------------------------------------------------------------------
Integer ImplicitRungeKutta::GetNumControlStagePointsPerMesh()
{
   return numControlStagePointsPerMesh;
}

//------------------------------------------------------------------------------
// Rvector GetStageTimes()
//------------------------------------------------------------------------------
/**
 * This method returns the array of stage times
 *
 * @return  array of stage times
 *
 */
//------------------------------------------------------------------------------
Rvector ImplicitRungeKutta::GetStageTimes()
{
   return stageTimes;
}

//------------------------------------------------------------------------------
// Rmatrix GetParamDependArray()
//------------------------------------------------------------------------------
/**
 * This method returns the matrix of parameter dependencies
 *
 * @return  matrix of parameter dependencies
 *
 */
//------------------------------------------------------------------------------
Rmatrix ImplicitRungeKutta::GetParamDependArray()
{
   return paramDepArray;
}

//------------------------------------------------------------------------------
// Rmatrix GetFuncConstArray()
//------------------------------------------------------------------------------
/**
 * This method returns the function constraint matrix
 *
 * @return  matrix of fucntion constraints
 *
 */
//------------------------------------------------------------------------------
Rmatrix ImplicitRungeKutta::GetFuncConstArray()
{
   return funcConstArray;
}

//------------------------------------------------------------------------------
// Integer GetNumDefectCons()
//------------------------------------------------------------------------------
/**
 * This method returns the number of defect contraints
 *
 * @return  the number of defect contraints
 *
 */
//------------------------------------------------------------------------------
Integer ImplicitRungeKutta::GetNumDefectCons()
{
   return numDefectCons;
}

//------------------------------------------------------------------------------
// Integer GetNumPointsPerStep()
//------------------------------------------------------------------------------
/**
 * This method returns the number of points per step
 *
 * @return  the number of points per step
 *
 */
//------------------------------------------------------------------------------
Integer ImplicitRungeKutta::GetNumPointsPerStep()
{
   return numPointsPerStep;
}

//------------------------------------------------------------------------------
// void GetDependencyChunk(Integer defectIdx, Integer pointIdx,
//                         Integer numVars, Rmatrix &aChunk,
//                         Rmatrix &bChunk)
//------------------------------------------------------------------------------
/**
 * This method returns the dependency chunk(s) for the input defect and point
 * indexes
 *
 * @param  <defectIdx>      defect index
 * @param  <pointIdx>       point index
 * @param  <numVars>        number of variables
 * @param  <aChunk>         [output] dependency chunk A
 * @param  <bChunk>         [output] dependency chunk B
 *
 */
//------------------------------------------------------------------------------
void ImplicitRungeKutta::GetDependencyChunk(Integer defectIdx, Integer pointIdx,
                                            Integer numVars, Rmatrix &aChunk,
                                            Rmatrix &bChunk)
{
   if (defectIdx < 0 || defectIdx >= numDefectCons)
   {
      std::string errmsg = "ImplicitRungeKutta::GetDependencyChunk: ";
      errmsg += "Invalid defect constraint index.\n";
      throw LowThrustException(errmsg);
   }

   if (pointIdx < 0 || pointIdx >= numPointsPerStep)
   {
      std::string errmsg = "ImplicitRungeKutta::GetDependencyChunk: ";
      errmsg += "Invalid point index.\n";
      throw LowThrustException(errmsg);
   }

   aChunk.SetSize(numVars, numVars);
   bChunk.SetSize(numVars, numVars);

   for (Integer rowIdx = 0; rowIdx < numVars; rowIdx++)
   {
      for (Integer colIdx = 0; colIdx < numVars; colIdx++)
      {
         if (rowIdx == colIdx)
         {
            aChunk(rowIdx, colIdx) = patternAMat(defectIdx, pointIdx);
            bChunk(rowIdx, colIdx) = patternBMat(defectIdx, pointIdx);
         }
         else
         {
            aChunk(rowIdx, colIdx) = 0.0;
            bChunk(rowIdx, colIdx) = 0.0;
         }
      }
   }
   #ifdef DEBUG
         MessageInterface::ShowMessage("aChunk is given as follows:\n%s\n",
                                       aChunk.ToString(12).c_str());
         MessageInterface::ShowMessage("bChunk is given as follows:\n%s\n",
                                       bChunk.ToString(12).c_str());
   #endif
}

//------------------------------------------------------------------------------
// void ComputeDependencies()
//------------------------------------------------------------------------------
/**
 * This method computes the dependencies
 *
 */
//------------------------------------------------------------------------------
void ImplicitRungeKutta::ComputeDependencies()
{

   // Loop over the functions
   patternAMat.SetSize(numDefectCons, numPointsPerStep);
   patternBMat.SetSize(numDefectCons, numPointsPerStep);
   for (Integer funIdx  = 0; funIdx < paramDepArray.GetNumRows(); funIdx++)
   {
      // Loop over the variables
      for (Integer pointIdx = 0;  pointIdx < paramDepArray.GetNumColumns();
           pointIdx++)
      {
         patternAMat(funIdx, pointIdx) = paramDepArray(funIdx, pointIdx);
         patternBMat(funIdx, pointIdx) = funcConstArray(funIdx, pointIdx);
      }
   }
   #ifdef DEBUG
         MessageInterface::ShowMessage(
                           "funcConstArray is given as follows:\n%s\n",
                           funcConstArray.ToString(12).c_str());
         MessageInterface::ShowMessage(
                           "paramDepArray is given as follows:\n%s\n",
                           paramDepArray.ToString(12).c_str());
   #endif
}

//------------------------------------------------------------------------------
// void ComputeAandB(Integer numVars, Rmatrix &aMat,
//                   Rmatrix &bMat)
//------------------------------------------------------------------------------
/**
 * This method computes the A and B matrices
 *
 * @param  <aMat>   [output] A matrix
 * @param  <bMat>   [output] B matrix
 *
 */
//------------------------------------------------------------------------------
void ImplicitRungeKutta::ComputeAandB(Integer numVars, Rmatrix &aMat,
                                      Rmatrix &bMat)
{            
   // Loop over the functions
   Integer numRows = paramDepArray.GetNumRows()*numVars;
   Integer numCols = paramDepArray.GetNumColumns()*numVars;
   aMat.SetSize(numRows,numCols);
   bMat.SetSize(numRows,numCols);
   Integer rowStartIdx, colStartIdx;
   //Integer nvM1 = numVars - 1;
   for (Integer funIdx = 0; funIdx < paramDepArray.GetNumRows(); funIdx++)
   {
         // Loop over the variables
         rowStartIdx = numVars*funIdx;
         for (Integer varIdx = 0; varIdx < paramDepArray.GetNumColumns();
              varIdx++)
         {
            colStartIdx = numVars*varIdx;

            for (Integer idx = 0; idx < numVars; idx++)
            {                  
               aMat(rowStartIdx + idx, colStartIdx + idx) =
                                                 paramDepArray(funIdx,varIdx);
               bMat(rowStartIdx + idx, colStartIdx + idx) =
                                                 funcConstArray(funIdx,varIdx);
            }
         }
   }            
   #ifdef DEBUG
      MessageInterface::ShowMessage("aMat is given as follows:\n%s\n",
                                    aMat.ToString(12).c_str());
      MessageInterface::ShowMessage("bMat is given as follows:\n%s\n",
                                    bMat.ToString(12).c_str());
   #endif
}

