//------------------------------------------------------------------------------
//                      NLPFuncUtilRadau
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Created: 2015/06/23
//
/**
 * Implementation of NLPFuncUtilRadau.m
 *
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"
#include "NLPFuncUtilRadau.hpp"

//#define DEBUG
//#define DEBUG_RADAU
//#define DEBUG_RADAU_D
//#define DEBUG_RADAU_MR
//#define DEBUG_RADAU_MATRIX

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer NLPFuncUtilRadau::minPolynomialDegreeLB = 3;
const Integer NLPFuncUtilRadau::maxPolynomialDegreeUB = 15;


//------------------------------------------------------------------------------
//public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// NLPFuncUtilRadau(Integer minPolyDeg = 3, Integer maxPolyDeg = 14,
//                  Real RelTol = 1E-6)
//------------------------------------------------------------------------------
/**
 * Default constructor with default values
 *
 * @param <minPolyDeg> the minimum polynomial degree of the LGR collocation
 * @param <maxPolyDeg> the maximum polynomial degree of the LGR collocation
 * @param <RelTol>     the relative tolerance of the mesh-refienment
 */
//------------------------------------------------------------------------------
NLPFuncUtilRadau::NLPFuncUtilRadau(Integer minPolyDeg, Integer maxPolyDeg,
                                   Real RelTol)
{
   SetPolynomialDegreeRange(minPolyDeg, maxPolyDeg);
   relErrorTol = RelTol;

   // Radau does not use the last point.
   quadratureType = 2;

   SparseMatrixUtil::SetSize(radauDiffSMatrix, 1, 1);
}

//------------------------------------------------------------------------------
//   NLPFuncUtilRadau(const  NLPFuncUtilRadau &copy)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 *
 * @parm <copy>  NPFuncUtilRadau to copy
 * 
 */
//------------------------------------------------------------------------------
NLPFuncUtilRadau::NLPFuncUtilRadau(const NLPFuncUtilRadau &copy)
   : NLPFuncUtil_Coll(copy)
{
   // scaled discretization points for transcription
   radauPoints = copy.radauPoints;
   // radau transcription related
   radauDiffSMatrix = copy.radauDiffSMatrix;

   // Indicates if constraint matrices have been initialized
   isConMatInitialized = copy.isConMatInitialized;
   // Indicates if cost matrices have been initialized
   isCostMatInitialized = copy.isCostMatInitialized;

   // polynomial degree related
   minPolynomialDegree = copy.minPolynomialDegree; // default value is 3
   maxPolynomialDegree = copy.maxPolynomialDegree; // default value is 14

   stateInterpolatorArray = copy.stateInterpolatorArray;
   controlInterpolatorArray = copy.controlInterpolatorArray;
   isInterpolatorsInitialized = copy.isInterpolatorsInitialized;
}

//------------------------------------------------------------------------------
//  NLPFuncUtilRadau&   operator=(const NLPFuncUtilRadau &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator of NLPFuncUtilRadau.
 * 
 * @parm <copy>  NPFuncUtilRadau whose values to assign to this object
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtilRadau&   NLPFuncUtilRadau::operator=(const NLPFuncUtilRadau &copy)
{
   if (this == &copy)
      return *this;

   NLPFuncUtil_Coll::operator=(copy);
   /// scaled discretization points for transcription
   radauPoints.SetSize( copy.radauPoints.GetSize());
   radauPoints = copy.radauPoints;
   /// radau transcription related 
   radauDiffSMatrix = copy.radauDiffSMatrix;

   ///  Bool. Indicates if constraint matrices have been initialized
   isConMatInitialized  = copy.isConMatInitialized;
   ///  Bool. Indicates if cost matrices have been initialized
   isCostMatInitialized = copy.isCostMatInitialized;

   /// polynomial degree related
   minPolynomialDegree  = copy.minPolynomialDegree; // default value is 3
   maxPolynomialDegree  = copy.maxPolynomialDegree; // default value is 14

   stateInterpolatorArray     = copy.stateInterpolatorArray;
   controlInterpolatorArray   = copy.controlInterpolatorArray;
   isInterpolatorsInitialized = copy.isInterpolatorsInitialized;
   
   return *this;
}

//------------------------------------------------------------------------------
//  ~NLPFuncUtilRadau()
//------------------------------------------------------------------------------
/**
 * Destructor of NLPFuncUtilRadau.
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtilRadau::~NLPFuncUtilRadau()
{
   // do nothing 
};


//------------------------------------------------------------------------------
//  Integer GetMeshIndex(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the mesh index for the given pointIdx
 *
 * @param pointIdx  the specified point index
 *
 * @return the mesh index
 *
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtilRadau::GetMeshIndex(Integer pointIdx)
{
   return pointIdx;
};

//------------------------------------------------------------------------------
//  Integer GetStageIndex(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the stage index for the given pointIdx
 *
 * @param pointIdx  the specified point index
 *
 * @return the stage index
 *
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtilRadau::GetStageIndex(Integer pointIdx)
{
   return 0;
};

//------------------------------------------------------------------------------
//  Real GetdCurrentTimedTI(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the TimedTI value for the given pointIdx
 *
 * @param pointIdx  the specified point index
 * @param stageIdx  the stage index always zero in Radau Class; ignore the input
 *
 * @return the TimedTI value
 *
 */
//------------------------------------------------------------------------------
Real NLPFuncUtilRadau::GetdCurrentTimedTI(Integer pointIdx, Integer stageIdx)
{
   /// @todo add validation of pointIdx
   return (1 - discretizationPoints(pointIdx))/2;
};

//------------------------------------------------------------------------------
//  Real GetdCurrentTimedTF(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the TimedTF value for the given pointIdx
 *
 * @param pointIdx  the specified point index
 * @param stageIdx  the stage index always zero in Radau Class; ignore the input
 *
 * @return the TimedTF value
 *
 */
//------------------------------------------------------------------------------
Real NLPFuncUtilRadau::GetdCurrentTimedTF(Integer pointIdx, Integer stageIdx)
{
   /// @todo add validation of pointIdx
   return (1 + discretizationPoints(pointIdx))/2;
};

//------------------------------------------------------------------------------
//void InitializeTranscription();
//------------------------------------------------------------------------------
/**
 * Initialize NLPFuncUtilRadau, and mainly compute LGR nodes,
 * weigthVec, & diffMat
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::InitializeTranscription()
{
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage("In Radau::InitializeTranscription\n");
   #endif
   // Compute Radau points, weights, and differentiation matrix
   CheckMeshIntervalNumPoints();
   Rvector      fractions = ptrConfig->GetMeshIntervalFractions();
   IntegerArray numPts    = ptrConfig->GetMeshIntervalNumPoints();

   

	bool isSuccess = RadauMathUtil::ComputeMultiSegmentLGRNodes(
		                             &fractions,
                                   &numPts,
                                   discretizationPoints,
                                   quadratureWeights,
                                   radauDiffSMatrix);
   if (!isSuccess)
   {
      std::stringstream errmsg("");
      errmsg << " NLPFuncUtilRadau::InitializeTranscription, ";
      errmsg << "failed to generate mesh points.";
      errmsg << " For the Radau collocation method, the first and last segment ";
	  errmsg << "points must be -1 and 1 respectively, and the points ";
	  errmsg << "must be monotically increasing. \n";
      throw LowThrustException(errmsg.str());
   }
   numMeshPoints    = quadratureWeights.GetSize();
   numStatePoints   = numMeshPoints + 1;
   numControlPoints = numMeshPoints;
   numPathConstraintPoints = numMeshPoints;

   radauPoints.SetSize(discretizationPoints.GetSize());
   radauPoints = discretizationPoints;

   Integer numDefectConNLP; 
   numDefectConNLP = (ptrConfig->GetNumStateVars())*numMeshPoints;
   ptrConfig->SetNumDefectConNLP(numDefectConNLP );
   SetStageProperties();

   Integer numStateVarsNLP = ptrConfig->GetNumStateVars()*numStatePoints;
   ptrConfig->SetNumStateVarsNLP(numStateVarsNLP);

   Integer numControlVarsNLP = ptrConfig->GetNumControlVars()*numControlPoints;
   ptrConfig->SetNumControlVarsNLP(numControlVarsNLP);
   
   
   Integer numDecisionVarsNLP = ptrConfig->GetNumStateVarsNLP() +
                                ptrConfig->GetNumControlVarsNLP() +
                                ptrConfig->GetNumTimeVarsNLP() + 
                                ptrConfig->GetNumStaticVars();// YK mod static vars

   ptrConfig->SetNumDecisionVarsNLP(numDecisionVarsNLP);

   pathFuncInputData.Initialize(ptrConfig->GetNumStateVars(), 
                                ptrConfig->GetNumControlVars(),
                                ptrConfig->GetNumStaticVars());

   timeVectorType.resize(numStatePoints);
   for (Integer idx = 0; idx < numStatePoints -1; ++idx)
   {
      // have all the state and control
      timeVectorType[idx] = 1; 
   }
   // last point has state but does not have control
   timeVectorType[numStatePoints - 1] = 2; 

   isConMatInitialized = false;   
   isCostMatInitialized = false;

   InitStateAndControlInterpolator();
   
   return;
}

//------------------------------------------------------------------------------
//void SetTimeVector(Real initialTime, Real finalTime)
//------------------------------------------------------------------------------
/**
 * Compute discretization points with initial and final times and radau points
 *
 * @param <initialTime>  initial time of the phase
 * @param <finalTime>    final time of the phase
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::SetTimeVector(Real initialTime, Real finalTime)
{
   deltaTime = finalTime - initialTime;
//   Rvector timeVector(radauPoints.GetSize());
   timeVector.SetSize(radauPoints.GetSize());
   for (Integer idx = 0; idx < radauPoints.GetSize(); ++idx)
   {
      timeVector(idx) = (deltaTime)*(radauPoints(idx) + 1) / 2 + initialTime;
   }
   numTimePoints = numMeshPoints + 1;
}

//------------------------------------------------------------------------------
//void SetPolynomialDegreeRange(Integer minPolynomialDegree,
//                              Integer maxPolynomialDegree)
//------------------------------------------------------------------------------
/**
 * Set polynomial degrees for NLPFuncUtilRadau (not used in MATLAB)
 *
 * @param <minPolynomialDegree>   minimum polynomial degree used in collocation
 * @param <maxPolynomialDegree>   maximum polynomial degree used in collocation
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::SetPolynomialDegreeRange(Integer minDegree,
   												         Integer maxDegree)
{
//   minPolynomialDegree = minDegree;

   if ((maxPolynomialDegreeUB < maxDegree) ||
      (minPolynomialDegreeLB > minDegree))
   {
      std::stringstream errmsg("");
      errmsg << "For NLPFuncUtilRadau::SetPolynomialDegreeRange,";
      errmsg << " the given min. and/or max. polynomial degree is invalid.";
      errmsg << " The possible polynomial degree range is [ ";
      errmsg << minPolynomialDegreeLB;
      errmsg << ", " << maxPolynomialDegreeUB << " ]";
      throw LowThrustException(errmsg.str());
   }

   minPolynomialDegree = minDegree;
   maxPolynomialDegree = maxDegree;

}

//------------------------------------------------------------------------------
// void RefineMesh(DecVecTypeBetts        *ptrDecVector,
//                 UserPathFunctionManager *ptrPathFunctionManager,
//                 bool                    &isMeshRefined,
//                 IntegerArray            &newMeshIntervalNumPoints,
//                 Rvector                 &newMeshIntervalFractions,
//                 IntegerArray            &maxRelErrorInMesh,
//                 Rmatrix                 &newStateGuess,
//                 Rmatrix                 &newControlGuess)
//------------------------------------------------------------------------------
/**
 * Compute new mesh points based on estimated transcription errors.
 *
 * @param <isMeshRefined>              output: indicating whether mesh has been
 *                                    changed
 * @param <newMeshIntervalNumPoints>   output: new mesh points
 * @param <newMeshIntervalFractions>   output: new mesh intervals
 * @param <maxRelErrorInMesh>          output: the current maximum relative error
 * @param <ptrDecVector>               input: the pointer of the current
 *                                     decision vector
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::RefineMesh(DecVecTypeBetts        *ptrDecVector,
                                 UserPathFunctionManager *ptrFunctionManager,
                                 Rvector                 *oldRelErrorArray,
                                 bool                 &isMeshRefined,
                                 IntegerArray         &newMeshIntervalNumPoints,
                                 Rvector              &newMeshIntervalFractions,
                                 Rvector              &maxRelErrorArray,
                                 Rmatrix              &newStateGuess,
                                 Rmatrix              &newControlGuess)
{
   // TBD: set new state and control guesses 
   PrepareToRefineMesh(ptrFunctionManager);

   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   Rvector meshIntervalFractions = ptrConfig->GetMeshIntervalFractions();
   std::vector<Real> maxRelErrorStdArray, intnlNewMeshIntvFractions;
   intnlNewMeshIntvFractions.push_back(-1);

   // assume mesh is not refined.
   bool IsNotMeshRefined = true;

   newMeshIntervalNumPoints.clear();   
   
   
   Real logNq, logErrorRatio,
      newMeshPoints, meshIntvLength;
   Integer polynomialDegreeDiff, newPolynomialDegreeRatio,
           polynomialDegree, newPolynomialDegree;

   // accumulate state and control; first index is meshIntvIdx,
   /// the second index is state/control var idx
   std::vector<std::vector<Rvector> > stateAcc, controlAcc;

   for (Integer meshIntvIdx = 0; meshIntvIdx < ptrConfig->GetNumMeshIntervals();
        ++meshIntvIdx)
   {
      // Compute new state and control in the new mesh points
      std::vector<Rvector> stateColVecArray, controlColVecArray;
      GetStateAndControlInMesh(meshIntvIdx, ptrDecVector,
                              stateColVecArray, controlColVecArray, false, false);
      polynomialDegree = meshIntervalNumPoints[meshIntvIdx];

      //  Compute error in proposed new mesh and state and control
      Real maxRelErrorInMesh = GetMaxRelErrorInMesh(meshIntvIdx, ptrDecVector);
      maxRelErrorStdArray.push_back(maxRelErrorInMesh);
      
      Rvector normNewMeshIntvFractions;
      IntegerArray normNewMeshNumPts;
      Integer numSubIntervals = 1;

      // Compute new mesh points in the interval
      if (maxRelErrorInMesh <= relErrorTol)
      {
         //  If relative error is small enough given previous mesh,
         //  dont refine this mesh interval.
         newMeshIntervalNumPoints.push_back(polynomialDegree);
         intnlNewMeshIntvFractions.push_back(
            meshIntervalFractions(meshIntvIdx + 1));     

         normNewMeshIntvFractions.SetSize(2);
         normNewMeshIntvFractions(0) = -1;
         normNewMeshIntvFractions(1) = 1;
         normNewMeshNumPts.push_back(polynomialDegree);

         stateAcc.push_back(stateColVecArray);
         controlAcc.push_back(controlColVecArray);
      }
      else
      {
         //  Copute the new polynomial degree for the mesh interval
         
         logNq = log(double(polynomialDegree));

         logErrorRatio = log(maxRelErrorInMesh / relErrorTol);
         polynomialDegreeDiff = ceil(logErrorRatio / logNq);

         newPolynomialDegree = polynomialDegree + polynomialDegreeDiff;

         //  If polynomial degree is ok, accept it, otherwise divide
         //  the interval
         if (newPolynomialDegree <= maxPolynomialDegree)
         {
            newMeshIntervalNumPoints.push_back(newPolynomialDegree);
            intnlNewMeshIntvFractions.push_back(
               meshIntervalFractions(meshIntvIdx + 1));

            normNewMeshIntvFractions.SetSize(2);
            normNewMeshIntvFractions(0) = -1;
            normNewMeshIntvFractions(1) = 1;
            normNewMeshNumPts.push_back(newPolynomialDegree);
            IsNotMeshRefined = false;
         }
         else
         {
            newPolynomialDegreeRatio =
               ceil(double(newPolynomialDegree) / double(minPolynomialDegree));
            numSubIntervals = GmatMathUtil::Max(newPolynomialDegreeRatio, 2);
            meshIntvLength = meshIntervalFractions(meshIntvIdx + 1)
                             - meshIntervalFractions(meshIntvIdx);


            normNewMeshIntvFractions.SetSize(numSubIntervals + 1);
            normNewMeshIntvFractions(0) = -1;

            for (Integer subIntervalIdx = 0; subIntervalIdx < numSubIntervals;
               ++subIntervalIdx)
            {
               normNewMeshNumPts.push_back(minPolynomialDegree);
               newMeshIntervalNumPoints.push_back(minPolynomialDegree);

               normNewMeshIntvFractions(subIntervalIdx + 1) = 
                                 -1.0 + (subIntervalIdx + 1.0)*2.0 / double(numSubIntervals);
               newMeshPoints =
                  meshIntervalFractions(meshIntvIdx)
                  + (subIntervalIdx + 1.0)*meshIntvLength / double(numSubIntervals);
               intnlNewMeshIntvFractions.push_back(newMeshPoints);

            }
            IsNotMeshRefined = false;
         }

         BaryLagrangeInterpolator interp;
         std::vector<Rvector> stateInterpColVec, controlInterpColVec;

         Rvector newNormMeshPts, interpPts, normMeshPts, weightVec;
         RSMatrix diffMat;
         bool flag = RadauMathUtil::ComputeMultiSegmentLGRNodes(&normNewMeshIntvFractions,
            &normNewMeshNumPts, newNormMeshPts, weightVec, diffMat);
         RadauMathUtil::ComputeSingleSegLGRNodes(polynomialDegree - 1, normMeshPts, weightVec);
         Real tmp = normMeshPts(normMeshPts.GetSize() - 1);

         // eliminate the terminal points for interpolation.
         interpPts.SetSize(newNormMeshPts.GetSize() - 2);
         for (Integer idx = 1; idx < newNormMeshPts.GetSize() - 1; ++idx)
         {
            interpPts(idx - 1) = newNormMeshPts(idx);
         }

         interp.SetIndVarVec(&normMeshPts);
         interp.SetInterpPointVec(&interpPts);

         // interpolate!
         for (Integer stateIdx = 0; stateIdx < stateColVecArray.size(); stateIdx++)
         {
            // Barycentric Lagrange Interpolator does not allow 
            // coincedence of the independent and interpolation points
            // So, use the initial point as given by decision vector

            Rvector tmpVec, stateVec;
            interp.Interpolate(&stateColVecArray[stateIdx], tmpVec);
            stateVec.SetSize(tmpVec.GetSize() + 1);
            stateVec(0) = stateColVecArray[stateIdx](0);
            for (Integer idx  = 0 ; idx < tmpVec.GetSize(); idx++)
            {
               stateVec(idx + 1) = tmpVec(idx);
            }
            stateInterpColVec.push_back(stateVec);
         }
         for (Integer controlIdx = 0; controlIdx < controlColVecArray.size(); controlIdx++)
         {
            // Barycentric Lagrange Interpolator does not allow 
            /// coincedence of the independent and interpolation points
            /// So, use the initial point as given by decision vector

            Rvector tmpVec, controlVec;
            interp.Interpolate(&controlColVecArray[controlIdx], tmpVec);
            controlVec.SetSize(tmpVec.GetSize() + 1);
            controlVec(0) = controlColVecArray[controlIdx](0);
            for (Integer idx = 0; idx < tmpVec.GetSize(); idx++)
            {
               controlVec(idx + 1) = tmpVec(idx);
            }
            controlInterpColVec.push_back(controlVec);
         }
         stateAcc.push_back(stateInterpColVec);
         controlAcc.push_back(controlInterpColVec);
      }
   }

   
   Integer numNewMeshIntvFractions = intnlNewMeshIntvFractions.size(); 
   Integer numNewMeshPts = 0;
   newMeshIntervalFractions.SetSize(numNewMeshIntvFractions);
   for (Integer idx = 0; idx < newMeshIntervalNumPoints.size(); ++idx)
   {
      // exclude the end point of the interval
      numNewMeshPts += newMeshIntervalNumPoints[idx];
      newMeshIntervalFractions(idx) = intnlNewMeshIntvFractions[idx];
   }
   // include the final point
   numNewMeshPts += 1;
   newMeshIntervalFractions(numNewMeshIntvFractions - 1)
      = intnlNewMeshIntvFractions[numNewMeshIntvFractions - 1];


   newStateGuess.SetSize(numNewMeshPts,ptrConfig->GetNumStateVars());
   newControlGuess.SetSize(numNewMeshPts - 1, ptrConfig->GetNumControlVars());

   Integer idxCounter = 0;
   // loop over the previous mesh intervals
   for (Integer idx = 0; idx < stateAcc.size(); ++idx)
   {
      // loop over mesh points
      for (Integer idx2 = 0; idx2 < stateAcc[idx][0].GetSize(); ++idx2)
      {
         // loop over state variables
         for (Integer idx3 = 0; idx3 < ptrConfig->GetNumStateVars(); ++idx3)
         {
            newStateGuess(idxCounter + idx2, idx3) = stateAcc[idx][idx3](idx2);
         }
         for (Integer idx3 = 0; idx3 < ptrConfig->GetNumControlVars(); ++idx3)
         {
            newControlGuess(idxCounter + idx2, idx3) = controlAcc[idx][idx3](idx2);            
         }
      }
      idxCounter += stateAcc[idx][0].GetSize();
   }   

   // now handle the final state vector
   Rvector finalState = ptrDecVector->GetStateAtMeshPoint(numStatePoints - 1);
   for (Integer idx3 = 0; idx3 < ptrConfig->GetNumStateVars(); ++idx3)
   {
      newStateGuess(numNewMeshPts - 1, idx3) = finalState(idx3);
   }

   maxRelErrorArray.SetSize(maxRelErrorStdArray.size());
   for (Integer idx = 0; idx < maxRelErrorStdArray.size(); ++idx)
   {
      maxRelErrorArray(idx) = maxRelErrorStdArray[idx];
   }
   isMeshRefined = !IsNotMeshRefined;
}

//------------------------------------------------------------------------------
// Rvector ComputeMaxMeshError(DecVecTypeBetts *ptrDecVector,
//                            UserPathFunctionManager *ptrFunctionManager)
//------------------------------------------------------------------------------
/**
 * Computes the max mesh error of each interval
 *
 * @param <ptrDecVector>       The pointer to the decision vector object that 
 *                             contains the mesh to compute the error for
 * @param <ptrFunctionManager> The pointer to the function manager used to 
 *                             initialize data
 *
 * @return The vector of max mesh error from each interval
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtilRadau::ComputeMaxMeshError(
                                   DecVecTypeBetts *ptrDecVector,
                                   UserPathFunctionManager *ptrFunctionManager)
{
   PrepareToRefineMesh(ptrFunctionManager);
   Rvector maxRelErrorsInMesh(ptrConfig->GetNumMeshIntervals());
   for (Integer meshIntvIdx = 0; meshIntvIdx < ptrConfig->GetNumMeshIntervals();
      ++meshIntvIdx)
   {
      //  Compute error in received mesh
      maxRelErrorsInMesh(meshIntvIdx) =
         GetMaxRelErrorInMesh(meshIntvIdx, ptrDecVector, false);
   }

   return maxRelErrorsInMesh;
}

//------------------------------------------------------------------------------
//protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void SetStageProperties()
//------------------------------------------------------------------------------
/**
 * Sets the stage properties to default values.
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::SetStageProperties()
{
   numControlStagePointsPerMesh = 0;
   numStagePointsPerMesh        = 0;
   numStateStagePointsPerMesh   = 0;
   numStagePoints               = 0;
}


//------------------------------------------------------------------------------
//void InitNLPHelpers()
//------------------------------------------------------------------------------
/**
 * Initializes NLP helper utilities for cost and defect calculations
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::InitNLPHelpers()
{
   // Configure the defect NLP data utility
   Integer numDependencies = 1;
   DefectNLPData.Initialize(ptrConfig->GetNumDefectConNLP(),
      ptrConfig->GetNumDecisionVarsNLP(),
      numDependencies * ptrConfig->GetNumDefectConNLP());

   // Configure the cost NLP data utility
   Integer numCostFunctions = 1;
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   numDependencies = numMeshPoints;

   CostNLPData.Initialize(numCostFunctions,
      ptrConfig->GetNumDecisionVarsNLP(),
      numDependencies * numCostFunctions);
}


//------------------------------------------------------------------------------
// void InitializeConstantCostMatrices(UserFunctionProperties *costFuncProps,
//                            std::vector<FunctionOutputData> *costFuncValues)
//------------------------------------------------------------------------------
/**
* Fills in the A, B, and D matrices for cost function during initialization
*
* @param <costFuncProps>   the user function properties of cost
* @param <costFuncValues>  the cost function values
*
*/
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::InitializeConstantCostMatrices(
                        UserFunctionProperties                 *costFuncProps,
                        const std::vector<FunctionOutputData*> &costFuncValues)
{

   if ((ptrConfig->HasIntegralCost() == false)
      && (isConMatInitialized == false))
      return;

   //  For cost function, there is only one function!
   Integer meshIdx;
   //  Note that A matrix is all zeros for Radau cost quadrature.

   Rmatrix stateJacPattern(1, ptrConfig->GetNumStateVars());
   stateJacPattern = costFuncProps->GetStateJacobianPattern();
   Rmatrix controlJacPattern(1, ptrConfig->GetNumControlVars());
   controlJacPattern = costFuncProps->GetControlJacobianPattern();

   IntegerArray stateIdxs, controlIdxs;
   IntegerArray staticIdxs = costFuncValues.at(0)->GetStaticIdxs();
   
   Rmatrix staticJacPattern;
   if (ptrConfig->HasStaticVars() == true)
   {
      staticJacPattern.SetSize(1, ptrConfig->GetNumStaticVars());
      staticJacPattern = costFuncProps->GetStaticJacobianPattern();
   }
   
   Integer numInterations = costFuncValues.size() - 1;

   //  Loop over the number of points
   for (Integer funcIdx = 0; funcIdx < numInterations; ++funcIdx)
   {
      //  Increment counters
      meshIdx = costFuncValues.at(funcIdx)->GetMeshIdx();

      CostNLPData.InsertBMatElement(0, funcIdx, -quadratureWeights(meshIdx));

      //  Initial time terms
      CostNLPData.InsertDMatElement(funcIdx, 0, 1);

      //  Final time terms
      CostNLPData.InsertDMatElement(funcIdx, 1, 1);

      //  Fill in state Jacobian elements
      if (ptrConfig->HasStateVars() == true)
      {
         stateIdxs = costFuncValues.at(funcIdx)->GetStateIdxs();
         for (Integer colIdx = 0; colIdx < ptrConfig->GetNumStateVars();
            ++colIdx)
         {
            if (stateJacPattern(0, colIdx) != 0)
               CostNLPData.InsertDMatElement(funcIdx,
               stateIdxs[colIdx], stateJacPattern(0, colIdx));
         }
      }

      //  Fill in control Jacobian terms
      if (ptrConfig->HasControlVars() == true)
      {
         controlIdxs = costFuncValues.at(funcIdx)->GetControlIdxs();
         for (Integer colIdx = 0; colIdx < ptrConfig->GetNumControlVars();
            ++colIdx)
         {
            if (controlJacPattern(0, colIdx) != 0)
               CostNLPData.InsertDMatElement(funcIdx,
               controlIdxs[colIdx], controlJacPattern(0, colIdx));
         }
      }

      //  Fill in static Jacobian terms
      if (ptrConfig->HasStaticVars() == true)
      {
         for (Integer colIdx = 0; colIdx < ptrConfig->GetNumStaticVars();
            ++colIdx)
         {
            if (staticJacPattern(0, colIdx) != 0)
               CostNLPData.InsertDMatElement(funcIdx,
               staticIdxs[colIdx], staticJacPattern(0, colIdx));
         }
      }
   }
   
   isCostMatInitialized = true;
}


//------------------------------------------------------------------------------
// void InitializeConstantDefectMatrices(UserFunctionProperties *dynFuncProps,
//                              std::vector<FunctionOutputData> *dynFuncValues)
//------------------------------------------------------------------------------
/**
 * Fills in the A, B, and D matrices for defect cons during initialization
 *
 * @param <dynFuncProps>    the user function properties of defect
 * @param <dynFuncValues>   the defect function values
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::InitializeConstantDefectMatrices(
                       UserFunctionProperties                 *dynFuncProps,
                       const std::vector<FunctionOutputData*> &dynFuncValues)
{

   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(
         "Entering NLPFuncUtilRadau::InitializeConstantDefectMatrices ...\n");
      MessageInterface::ShowMessage("   numMeshIntervals = %d\n",
                                    ptrConfig->GetNumMeshIntervals());
   #endif
   if ((ptrConfig->HasDefectCons() == false) && (!isConMatInitialized))
      return;

   //  Fill the A and B matrices
   int conIdx = -1;
   Integer lowIdx;
   IntegerArray stateIdxs, controlIdxs;
   Integer numIntvl = ptrConfig->GetNumMeshIntervals();
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   for (Integer intervalIdx = 0; intervalIdx < numIntvl; intervalIdx++)
   {
      #ifdef DEBUG_RADAU
         MessageInterface::ShowMessage("   intervalIdx = %d, numIntvl = %d\n",
                                       intervalIdx, numIntvl);
      #endif
      //  Extract Radau differentiation matrix for current mesh
      lowIdx = 0;
      #ifdef DEBUG_RADAU
         MessageInterface::ShowMessage(
                           "   meshIntervalNumPoints for this interval = %d\n",
                           meshIntervalNumPoints.at(intervalIdx));
         MessageInterface::ShowMessage(
                     "   num pts IN the mesh interval for this interval = %d\n",
                     ptrConfig->GetNumPointsInMeshInterval(intervalIdx));
      #endif
      if (intervalIdx != 0)
      {
         //  Count the number of mesh points up to and including
         //  this interval 
         for (Integer ii = 0; ii < intervalIdx; ii++)
            lowIdx += ptrConfig->GetNumPointsInMeshInterval(ii);
      }
      else
      {
         #ifdef DEBUG_RADAU
            MessageInterface::ShowMessage(
                              "   intervalidx = 0, so not computing lowIdx\n");
         #endif
      }
       
      #ifdef DEBUG_RADAU
         MessageInterface::ShowMessage("   lowIdx = %d\n", lowIdx);
      #endif

      // Loop over number of mesh points in the interval
      for (Integer rowIdx = 0; rowIdx < meshIntervalNumPoints[intervalIdx];
         ++rowIdx)
      {

         //  Loop over the number of states in the problem
         for (Integer stateIdx = 0; stateIdx < ptrConfig->GetNumStateVars();
              ++stateIdx)
         {

            //  Row * Col multiplication and A matrix config
            conIdx += 1;

            for (Integer pointIdx = 0;
                 pointIdx < meshIntervalNumPoints[intervalIdx] + 1;
                 ++pointIdx)
            {

               #ifdef DEBUG_RADAU
                  MessageInterface::ShowMessage("   pointIdx = %d\n", pointIdx);
                  MessageInterface::ShowMessage("   lowIdx + pointIdx = %d\n",
                                                (lowIdx + pointIdx));
                  MessageInterface::ShowMessage(
                                       " ---> with dynFuncValues size = %d\n",
                                       (Integer) dynFuncValues.size());
               #endif
               stateIdxs = dynFuncValues.at(lowIdx + pointIdx)->GetStateIdxs();
               #ifdef DEBUG_RADAU
                  MessageInterface::ShowMessage("   stateIdxs = \n");
                  for (Integer ii = 0; ii < stateIdxs.size(); ii++)
                     MessageInterface::ShowMessage("  %d    %d \n",
                                                   ii, stateIdxs.at(ii));
                  MessageInterface::ShowMessage(
                                 " --- about to call InsertAMatElement ...\n");
               #endif

               #ifdef DEBUG_RADAU_MATRIX
                  MessageInterface::ShowMessage(
                  " --- about to call InsertAMatElement with conIdx = %d...\n",
                  conIdx);
                  MessageInterface::ShowMessage("------ radauDiffSMatrix = \n");
                  Integer rJac = radauDiffSMatrix.size1();
                  Integer cJac = radauDiffSMatrix.size2();
                  for (Integer cc = 0; cc < cJac; cc++)
                  {
                     for (Integer rr = 0; rr < rJac; rr++)
                     {
                        Real jacTmp = radauDiffSMatrix(rr,cc);
                        if (jacTmp != 0.0)
                           MessageInterface::ShowMessage(
                                    " radauDiffSMatrix(%d, %d) =  %12.10f\n",
                                    rr+1, cc+1, jacTmp);
                     }
                  }
               #endif
               DefectNLPData.InsertAMatElement(
                  conIdx,
                  stateIdxs[stateIdx],
                  radauDiffSMatrix(lowIdx + rowIdx, lowIdx + pointIdx));
            }

            #ifdef DEBUG_RADAU
               MessageInterface::ShowMessage(
                  " --- about to call InsertBMatElement with conIdx = %d...\n",
                  conIdx);
            #endif
            //  Insert the B matrix element
            DefectNLPData.InsertBMatElement(conIdx, conIdx, 1);
         }
      }
   }

   Integer tmp1 = ptrConfig->GetNumStateVars();
   Integer tmp2 = dynFuncProps->GetNumberOfFunctions();
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(" --- tmp1 = %d, tmp2 = %d\n", tmp1, tmp2);
   #endif
   Rmatrix stateJacPattern(tmp2, tmp1);
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(" --- tmp1 = %d, tmp2 = %d\n", tmp1, tmp2);
      Rmatrix tmpMat = dynFuncProps->GetStateJacobianPattern();
      Integer r, c;
      tmpMat.GetSize(r,c);
      MessageInterface::ShowMessage(
                               " BUT size of state jac pattern = %d x %d\n",
                               (Integer) r, (Integer) c);
   #endif
   stateJacPattern = dynFuncProps->GetStateJacobianPattern();
   

   tmp1 = ptrConfig->GetNumControlVars();
   Rmatrix controlJacPattern(tmp2, tmp1);
   controlJacPattern = dynFuncProps->GetControlJacobianPattern();
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(" --- jacobian patterns set\n");
   #endif


   IntegerArray staticIdxs = dynFuncValues.at(0)->GetStaticIdxs();

   Rmatrix staticJacPattern;
   if (ptrConfig->HasStaticVars() == true)
   {
      staticJacPattern.SetSize(tmp2, ptrConfig->GetNumStaticVars());
      staticJacPattern = dynFuncProps->GetStaticJacobianPattern();
   }

   Integer conStartIdx;
   Real tmpValue;
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(
                  " --- about to fill D matrices... with numMeshPoints = %d\n",
                  numMeshPoints);
   #endif
   //  Fill the D matrices
   for (Integer funcIdx = 0; funcIdx < numMeshPoints; ++funcIdx)
   {
      //  Increment counters
      conStartIdx = (dynFuncValues.at(funcIdx)->GetMeshIdx()) *
                     ptrConfig->GetNumStateVars();

      for (Integer rowIdx = 0; rowIdx < ptrConfig->GetNumStateVars(); ++rowIdx)
      {
         #ifdef DEBUG_RADAU
            MessageInterface::ShowMessage(
                           " --- --- funcIdx = %d, rowIdx = %d\n",
                           funcIdx, rowIdx);
         #endif
         //  Initial time terms
         DefectNLPData.InsertDMatElement(rowIdx + conStartIdx, 0, 1);
         //  Final time terms
         DefectNLPData.InsertDMatElement(rowIdx + conStartIdx, 1, 1);
         //  Fill in state Jacobian elements
         if (ptrConfig->HasStateVars() == true)
         {
            #ifdef DEBUG_RADAU
               MessageInterface::ShowMessage(" --- has state vars\n");
            #endif
            stateIdxs = dynFuncValues.at(funcIdx)->GetStateIdxs();
            for (int colIdx = 0; colIdx < ptrConfig->GetNumStateVars();
                 ++colIdx)
            {
               tmpValue = stateJacPattern(rowIdx, colIdx);
               if (tmpValue != 0)
                  DefectNLPData.InsertDMatElement(rowIdx + conStartIdx,
                                                  stateIdxs[colIdx], tmpValue);
            }
         }
         //  Fill in control Jacobian terms
         if (ptrConfig->HasControlVars() == true)
         {
            #ifdef DEBUG_RADAU
               MessageInterface::ShowMessage(" --- has control vars\n");
            #endif
            controlIdxs = dynFuncValues.at(funcIdx)->GetControlIdxs();
            for (Integer colIdx = 0; colIdx < ptrConfig->GetNumControlVars();
                 ++colIdx)
            {
               tmpValue = controlJacPattern(rowIdx, colIdx);
               if (tmpValue != 0)
                  DefectNLPData.InsertDMatElement(rowIdx + conStartIdx,
                                controlIdxs[colIdx], tmpValue);
            }
         }

         //  Fill in static Jacobian terms
         if (ptrConfig->HasStaticVars() == true)
         {
            for (Integer colIdx = 0; colIdx < ptrConfig->GetNumStaticVars();
               ++colIdx)
            {
               if (staticJacPattern(rowIdx, colIdx) != 0)
                  DefectNLPData.InsertDMatElement(rowIdx + conStartIdx,
                  staticIdxs[colIdx], staticJacPattern(rowIdx, colIdx));
            }
         }
      }
   }
   #ifdef DEBUG_RADAU_D
      // print out in column order with index adjustment (to match MATLAB)
      RSMatrix getD = DefectNLPData.GetDMatrix();
      Integer sz1 = getD.size1();
      Integer sz2 = getD.size2();
      MessageInterface::ShowMessage(" ----------->>>>> DMatrix (%d x %d) : \n",
                                    sz1, sz2);
//      for (Integer ii = 0; ii < sz2; ii++)
//      {
//         for (Integer jj = 0; jj < sz1; jj++)
//         {
//            Real tmpD = getD(ii,jj);
//            if (tmpD != 0.0)
//               MessageInterface::ShowMessage("  (%d, %d)  = %12.6f\n",
//                                 jj+1, ii+1, tmpD);
//         }
//      }
      SparseMatrixUtil::PrintNonZeroElements(&getD);
   #endif

   //  Set flag indiciating the matrices are initialized
   isConMatInitialized = true;
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(
            "LEAVING NLPFuncUtilRadau::InitializeConstantDefectMatrices ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void InitStateAndControlInterpolator()
//------------------------------------------------------------------------------
/**
 * Initializes state and control interpolators in the form of
 * std::vector<BaryLagrangeInterpolator>.
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::InitStateAndControlInterpolator()
{
   BaryLagrangeInterpolator interpolator;
   Rvector lgrNodes, lgrNodesPlusOne, augLgrNodes, lgrWeightVec, interpPointVec;
   for (Integer polynomialDegree = minPolynomialDegree;
      polynomialDegree < maxPolynomialDegree + 1;
      ++polynomialDegree)
   {
      RadauMathUtil::ComputeSingleSegLGRNodes(
         polynomialDegree - 1, lgrNodes, lgrWeightVec);
      RadauMathUtil::ComputeSingleSegLGRNodes(
         polynomialDegree, augLgrNodes, lgrWeightVec);

      lgrNodesPlusOne.SetSize(polynomialDegree + 1);
      interpPointVec.SetSize(polynomialDegree);

      for (Integer idx = 0; idx < polynomialDegree; ++idx)
      {
         lgrNodesPlusOne(idx) = lgrNodes(idx);
         interpPointVec(idx) = augLgrNodes(idx + 1);
      }
      lgrNodesPlusOne(polynomialDegree) = 1;

      interpolator.SetIndVarVec(&lgrNodes);
      interpolator.SetInterpPointVec(&interpPointVec);
      controlInterpolatorArray.push_back(interpolator);

      interpolator.SetIndVarVec(&lgrNodesPlusOne);
      interpolator.SetInterpPointVec(&interpPointVec);
      stateInterpolatorArray.push_back(interpolator);
   }
   isInterpolatorsInitialized = true;
}

//------------------------------------------------------------------------------
// void GetStateAndControlInMesh(std::vector<Rvector> &stateVecRvector,
//                               std::vector<Rvector> &controlVecRvector,
//                               Integer              meshIntvIdx,
//                               DecVecTypeBetts      *ptrDecVector)
//------------------------------------------------------------------------------
/**
 * Get state and control vector in the given mesh interval in array form.
 * In an array of std::vector<Rvector>, the first index is state variable idx,
 * and the second index is mesh point idx.
 *
 * @param <stateVecRvector>   output: state vector in Rvector form
 * @param <controlVecRvector> output: control vector in Rvetor form
 * @param <meshIntvIdx>       input : the index of the mesh interval of interest
 * @param <ptrDecVector>      input : the pointer of the decision vector of 
 *                            phase
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::GetStateAndControlInMesh(
                                 Integer              meshIntvIdx,
                                 DecVecTypeBetts      *ptrDecVector,
                                 std::vector<Rvector> &stateVecRvector,
                                 std::vector<Rvector> &controlVecRvector, 
                                 bool                 hasFinalState,
                                 bool                 hasFinalControl)
{
   IntegerArray stateIdxs, controlIdxs;

   // accumulate the number of mesh points in the previous intervals 
   // DecisionVector class assumes mesh idx begins with 1 but not 0.
   Integer pointSum = 0;
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   if (meshIntvIdx != 0)
   {
      for (Integer idx = 0; idx < meshIntvIdx; ++idx)
      {
         pointSum += meshIntervalNumPoints[idx];
      }
   }
  
   Integer numStateIdxs, numControlIdxs;
   Integer numPointsInMesh = meshIntervalNumPoints[meshIntvIdx];
   if (hasFinalState == true)
      numStateIdxs = numPointsInMesh + 1;
   else 
      numStateIdxs = numPointsInMesh;

   if (hasFinalControl == true)
   {
      if (meshIntvIdx == meshIntervalNumPoints.size() - 1)
      {
         std::stringstream errmsg("");
         errmsg << "For NLPFuncUtilRadau::GetStateAndControlInMesh,";
         errmsg << " at the final interval, it is impossible to " ;
         errmsg << " obtain control vector at the final mesh point.";
         throw LowThrustException(errmsg.str());
      }
      else
         numControlIdxs = numPointsInMesh + 1;
   }
   else
      numControlIdxs = numPointsInMesh;

   stateIdxs.resize(numStateIdxs);
   controlIdxs.resize(numControlIdxs);

   //  Create the vector for the requested mesh interval
   for (Integer idx = 0; idx < numStateIdxs; ++idx)
   {
      stateIdxs[idx] = pointSum + idx;
   }
   for (Integer idx = 0; idx < numControlIdxs; ++idx)
   {
      controlIdxs[idx] = pointSum + idx;
   } 
 
   // pre-allocating   
   Rvector tmpVec;
   for (Integer idx1 = 0; idx1 < ptrConfig->GetNumStateVars(); ++idx1)
   {
      tmpVec.SetSize(numStateIdxs);
      stateVecRvector.push_back(tmpVec);      
   }
   for (Integer idx1 = 0; idx1 < ptrConfig->GetNumControlVars(); ++idx1)
   {
      tmpVec.SetSize(numControlIdxs);
      controlVecRvector.push_back(tmpVec);   
   }   

   // inserting data
   for (Integer idx1 = 0; idx1 < numStateIdxs; ++idx1)
   {
      
      tmpVec.SetSize(ptrConfig->GetNumStateVars());
      tmpVec = (*ptrDecVector).GetStateVector(
                              stateIdxs[idx1],0);      
      
      for (Integer idx2 = 0; idx2 < ptrConfig->GetNumStateVars(); ++idx2)
      {
         stateVecRvector[idx2](idx1) = tmpVec(idx2);
      }      
   }
   for (Integer idx1 = 0; idx1 < numControlIdxs; ++idx1)
   {
      tmpVec.SetSize(ptrConfig->GetNumControlVars());
      tmpVec = (*ptrDecVector).GetControlVector(
                              controlIdxs[idx1],0);      
      for (Integer idx2 = 0; idx2 < ptrConfig->GetNumControlVars(); ++idx2)
      {
         controlVecRvector[idx2](idx1) = tmpVec(idx2);
      }      
   }   
}

//------------------------------------------------------------------------------
// Real GetMaxRelErrorInMesh(Integer meshIntvIdx)
//------------------------------------------------------------------------------
/**
 * Estimates transcription error in a mesh interval.
 *
 * @param <meshIntvIdx> the mesh interval index of interest
 *
 */
//------------------------------------------------------------------------------
Real NLPFuncUtilRadau::GetMaxRelErrorInMesh(Integer meshIntvIdx,
                                              DecVecTypeBetts* DecVector,
                                              bool printErr)
{
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   Rvector meshIntervalFractions = ptrConfig->GetMeshIntervalFractions();

   Integer numNewRadauPoints = meshIntervalNumPoints[meshIntvIdx] + 1;
   Rvector newNormalRadauPoints, newRadauPoints, 
           augNewRadauPoints, lgrWeightVec, newTimeVector;

   RadauMathUtil::ComputeSingleSegLGRNodes(
                        numNewRadauPoints -1,
                        newNormalRadauPoints,lgrWeightVec);
            
   Real meshIntervalLength = meshIntervalFractions(meshIntvIdx+1)   
                             - meshIntervalFractions(meshIntvIdx);
   Real meshIntervalCenter = (meshIntervalFractions(meshIntvIdx+1) 
                             +  meshIntervalFractions(meshIntvIdx))/2;
                     
   newRadauPoints.SetSize(numNewRadauPoints);
   augNewRadauPoints.SetSize(numNewRadauPoints + 1);
   newTimeVector.SetSize(numNewRadauPoints + 1);
   for (Integer idx = 0; idx < numNewRadauPoints; ++idx)
   {
      newRadauPoints(idx) = 
                  1/2.0*meshIntervalLength*newNormalRadauPoints(idx);
      newRadauPoints(idx) += meshIntervalCenter;
      augNewRadauPoints(idx) = newRadauPoints(idx);
      newTimeVector(idx) = 
                  (timeVector(timeVector.GetSize() - 1) - timeVector(0))
                  *(newRadauPoints(idx) + 1) / 2 + timeVector(0);
   }
   augNewRadauPoints(numNewRadauPoints) = 
                  meshIntervalFractions(meshIntvIdx+1);
   newTimeVector(numNewRadauPoints) = timeVector(timeVector.GetSize() - 1);
   #ifdef DEBUG_RADAU_MR
      MessageInterface::ShowMessage(" --- newRadauPoints = \n");
      for (Integer idx = 0; idx < newRadauPoints.GetSize(); ++idx)
         MessageInterface::ShowMessage("%le, ", newRadauPoints(idx));
      MessageInterface::ShowMessage("\n");
   #endif
   Rmatrix dynFuncArray;
   dynFuncArray.SetSize(numNewRadauPoints, ptrConfig->GetNumStateVars());

   Rvector tmpVec, intStateVec, intControlVec;
   intStateVec.SetSize(ptrConfig->GetNumStateVars());
   intControlVec.SetSize(ptrConfig->GetNumControlVars());

   std::vector<Rvector> stateColVecArray, controlColVecArray;
   std::vector<Rvector> intStateColVecArray, intControlColVecArray;

   GetStateAndControlInMesh(meshIntvIdx, DecVector,
                           stateColVecArray, controlColVecArray);


   #ifdef DEBUG_RADAU_MR
      MessageInterface::ShowMessage(" --- stateColVecArray = \n");
      for (Integer idx = 0; idx < newRadauPoints.GetSize(); ++idx)
         MessageInterface::ShowMessage("%le, ",stateColVecArray[0](idx));
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage(" --- controlColVecArray = \n");
      for (Integer idx = 0; idx < newRadauPoints.GetSize() - 1; ++idx)
         MessageInterface::ShowMessage("%le, ",controlColVecArray[0](idx));
      MessageInterface::ShowMessage("\n");
   #endif
   // to do : get interpolated State/control in new mesh
   // using interpolators.

   Integer interpolatorIdx = numNewRadauPoints - minPolynomialDegree - 1;
   for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); ++idx)
   {      
      stateInterpolatorArray[interpolatorIdx].Interpolate(
                             &(stateColVecArray[idx]),tmpVec);
      intStateColVecArray.push_back(tmpVec);


      #ifdef DEBUG_RADAU_MR
         MessageInterface::ShowMessage(" --- intStateColVecArray = \n");
         for (Integer idx = 0; idx < tmpVec.GetSize(); ++idx)
            MessageInterface::ShowMessage("%le, ",tmpVec(idx));
         MessageInterface::ShowMessage("\n");
      #endif
   }

   for (Integer idx = 0; idx < ptrConfig->GetNumControlVars(); ++idx)
   {
      controlInterpolatorArray[interpolatorIdx].Interpolate(
                               &controlColVecArray[idx],tmpVec);
      intControlColVecArray.push_back(tmpVec);


      #ifdef DEBUG_RADAU_MR
         MessageInterface::ShowMessage(" --- intControlColVecArray = \n");
         for (Integer idx = 0; idx < tmpVec.GetSize(); ++idx)
            MessageInterface::ShowMessage("%le, ",tmpVec(idx));
         MessageInterface::ShowMessage("\n");
      #endif
   }

   FunctionOutputData *dynFuncData;
   // re-initialization for the next for-loop
   tmpVec.SetSize(ptrConfig->GetNumStateVars());
   for (Integer meshPointIdx = 0; meshPointIdx < numNewRadauPoints;
        ++meshPointIdx)
   {      
      if (meshPointIdx == 0)
      {
         // For the first state or control
         for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); ++idx)
         {
      
            intStateVec(idx) = stateColVecArray[idx](meshPointIdx);
         }
         // For the first control
         for (Integer idx = 0; idx < ptrConfig->GetNumControlVars(); ++idx)
         {
            intControlVec(idx) = controlColVecArray[idx](meshPointIdx);
         }
      }
      else
      {
         for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); ++idx)
         {
            // interpolated state does not include the first state
            intStateVec(idx) = intStateColVecArray[idx](meshPointIdx - 1);
         }
         for (Integer idx = 0; idx < ptrConfig->GetNumControlVars(); ++idx)
         {
            // interpolated state does not include the first control
            intControlVec(idx) = intControlColVecArray[idx](meshPointIdx - 1);
         }
      }            
      dynFuncData = NLPFuncUtil_Coll::GetUserDynFuncData(
                                      newTimeVector(meshPointIdx),
                                      intStateVec, intControlVec);

      tmpVec = dynFuncData->GetFunctionValues();
      
      for (Integer idx2 = 0; idx2 < ptrConfig->GetNumStateVars(); ++idx2)
      {
         // dynFuncArray does not include the first state
         dynFuncArray(meshPointIdx,idx2) = tmpVec(idx2);
      }
   }
   #ifdef DEBUG_RADAU_MR
   MessageInterface::ShowMessage(" --- dynFuncArray = \n");
   for (Integer idx1 = 0; idx1 < dynFuncArray.GetNumRows(); ++idx1)
   {
      for (Integer idx2 = 0; idx2 < dynFuncArray.GetNumColumns(); ++idx2)
      {
         MessageInterface::ShowMessage("%le, ",dynFuncArray(idx1,idx2));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   }
   #endif

   Real phaseLength = deltaTime;
            

   RSMatrix newLagDiffMat = 
                  RadauMathUtil::GetLagrangeDiffMatrix(&augNewRadauPoints);


   Rmatrix newLagDiffMat2 = 
                  SparseMatrixUtil::RSMatrixToRmatrix(&newLagDiffMat,
                  0, SparseMatrixUtil::GetNumRows(&newLagDiffMat) - 2, 
                  1, SparseMatrixUtil::GetNumColumns(&newLagDiffMat) - 1);

   Rmatrix integrationMatrix = newLagDiffMat2.Inverse();
   #ifdef DEBUG_RADAU_MR
      MessageInterface::ShowMessage(" --- integrationMatrix = \n");
      for (Integer idx1 = 0; idx1 < integrationMatrix.GetNumRows(); ++idx1)
      {
         for (Integer idx2 = 0; idx2 < integrationMatrix.GetNumColumns();
              ++idx2)
            MessageInterface::ShowMessage("%le, ",
                                          integrationMatrix(idx1, idx2));
         MessageInterface::ShowMessage("\n");
      }
   #endif

   Rmatrix integralTermArray; 
   integralTermArray.SetSize(numNewRadauPoints,ptrConfig->GetNumStateVars());
   for (Integer idx = 0; idx < numNewRadauPoints; ++idx)
   {
      
      for (Integer colIdx = 0; colIdx < ptrConfig->GetNumStateVars(); ++colIdx)
      {
         integralTermArray(idx, colIdx) =0;
         for (Integer idx2 = 0; idx2 < numNewRadauPoints; ++idx2)
         {
             integralTermArray(idx, colIdx) += 
                   phaseLength/2*integrationMatrix(idx,idx2)
                   *dynFuncArray(idx2,colIdx);             
         }
      }
   }

   Real maxAbsError, error;
   Real maxAbsState, state;
   Real maxStateRelError = 0;
   for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); ++idx)
   {
      maxAbsError = 0;
      maxAbsState = GmatMathUtil::Abs(stateColVecArray[idx](0));


      for (Integer idx2 = 0; idx2 < numNewRadauPoints; ++idx2)
      {
         if (idx2 != numNewRadauPoints - 1)
         {
            state = intStateColVecArray[idx](idx2);
         }
         else
         {
            // no interpolation at last point
            state = stateColVecArray[idx](numNewRadauPoints - 1); 
         }

         error = state - stateColVecArray[idx](0) 
                 - integralTermArray(idx2, idx);

         if (GmatMathUtil::Abs(error) > maxAbsError)         
            maxAbsError = GmatMathUtil::Abs(error);
         
         if (GmatMathUtil::Abs(state) > maxAbsState)        
            maxAbsState = GmatMathUtil::Abs(state);         
      }
      if (maxStateRelError < maxAbsError/(1+maxAbsState))
         maxStateRelError = maxAbsError/(1+maxAbsState);
   }
   if (printErr)
   {
      MessageInterface::ShowMessage(" Max. Mesh Relative Error = ");
      MessageInterface::ShowMessage("%le \n", maxStateRelError);
   }
   return maxStateRelError;
}

//------------------------------------------------------------------------------
//void CheckMeshIntervalNumPoints()
//------------------------------------------------------------------------------
/**
 * check whether user-defined polynomial degree satisfies min/max constarints
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::CheckMeshIntervalNumPoints()
{
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   for (UnsignedInt idx = 0; idx < meshIntervalNumPoints.size(); ++idx)
   {
      if ((meshIntervalNumPoints[idx] - 1 > maxPolynomialDegreeUB) ||
         (meshIntervalNumPoints[idx]   < minPolynomialDegreeLB) ||
		 (meshIntervalNumPoints[idx] <= 1)
		  )
      {
         std::stringstream errmsg("");
         errmsg << "For NLPFuncUtilRadau::CheckMeshIntervalNumPoints,";
         errmsg << " at interval " << idx + 1;
         errmsg << ", the current number of mesh points (";
         errmsg << meshIntervalNumPoints[idx];
         errmsg << ") is out of the allowed range [ ";
         errmsg << minPolynomialDegree << ", " << maxPolynomialDegree + 1;
         errmsg << " ].";
         throw LowThrustException(errmsg.str());
      }
   }
}

//------------------------------------------------------------------------------
// void FillDynamicDefectConMatrices(
//                         Rvector &QVector, RSMatrix &parQMatrix,
//                         std::vector<FunctionOutputData> *ptrFuncDataArray)
//------------------------------------------------------------------------------
/**
 * Computes Q and partial Q Matrix for defect constraints.
 *
 * @param <QVector>          output: the defect Q vector
 * @param <parQMatrix>       output: the defect jacobian matrix
 * @param <ptrFuncDataArray> input:  user defect function data array
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::FillDynamicDefectConMatrices(
                       const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                       Rvector &QVector, RSMatrix &parQMatrix)
{
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(
                           "Entering Radau::FillDynamicDefectConMatrices\n");
   #endif
   Integer idxLB = 0;
   // -1 from except the last point
   Integer idxUB = ptrFuncDataArray.size() - 1;
   //  Loop initializations
   Real dtBy2 = 0.5*(deltaTime);
   Real tempValue, dCurrentTimedTI, dCurrentTimedTF;

   // set the size of QVector
   QVector.SetSize(ptrConfig->GetNumDefectConNLP());

   // set the sparsity pattern of the parQMatrix before setting values
   // to save time.
   const RSMatrix* ptrDMatrix = DefectNLPData.GetDMatrixPtr();
   bool hasZeros = true;   
   parQMatrix = SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage("size of QVector set to %d\n",
                                    ptrConfig->GetNumDefectConNLP());
      MessageInterface::ShowMessage("size of parQMatrix set to %d x %d\n",
                                    ptrConfig->GetNumDefectConNLP(),
                                    ptrConfig->GetNumDecisionVarsNLP());
   #endif

   //  Loop over the function data and concatenate Q and parQ
   //  matrix terms
   Integer conStartIdx, meshIdx;
   Rvector funcValueVec;
   Rmatrix timeJac, stateJac, controlJac, staticJac;
   IntegerArray stateIdxs, controlIdxs, staticIdxs;
   
   if (ptrConfig->HasStaticVars() == true)
   {
      // static data idxs is same for all points; YK
      staticIdxs = ptrFuncDataArray.at(0)->GetStaticIdxs();
   }

   for (Integer funcIdx = idxLB; funcIdx < idxUB; ++funcIdx)
   {
      #ifdef DEBUG_RADAU
         MessageInterface::ShowMessage(
                     "at start of loop, funcIdx = %d, idxLB = %d, idxUB = %d\n",
                     funcIdx, idxLB, idxUB);
      #endif
      meshIdx = ptrFuncDataArray.at(funcIdx)->GetMeshIdx();
      //  Increment counters
      conStartIdx = (meshIdx) * ptrConfig->GetNumStateVars();
      #ifdef DEBUG_RADAU
         MessageInterface::ShowMessage(
                                 "           meshIdx = %d, conStartIdx = %d\n",
                                 meshIdx, conStartIdx);
      #endif

      // the second input is just meaningless
      dCurrentTimedTI = GetdCurrentTimedTI(meshIdx);
      dCurrentTimedTF = GetdCurrentTimedTF(meshIdx);

      funcValueVec = ptrFuncDataArray.at(funcIdx)->GetFunctionValues();
      timeJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::TIME);
      stateJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::STATE);
      controlJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::CONTROL);
      staticJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::STATIC);

      if (ptrConfig->HasStateVars() == true)
      {
         stateIdxs = ptrFuncDataArray.at(funcIdx)->GetStateIdxs();
      }
      if (ptrConfig->HasControlVars() == true)
      {
         controlIdxs = ptrFuncDataArray.at(funcIdx)->GetControlIdxs();
      }

      for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); ++idx)
      {

         //  Fill in the function data; QVector
         QVector(conStartIdx + idx) = -dtBy2*funcValueVec(idx);
         //  Initial time terms
         tempValue = 0.5*funcValueVec(idx) -
                     dtBy2*dCurrentTimedTI*timeJac(idx, 0);

         if (tempValue != 0.0)
            SparseMatrixUtil::SetElement(parQMatrix, conStartIdx + idx, 
                                         0, tempValue);

         //  Final time terms
         tempValue = -0.5*funcValueVec(idx) -
                      dtBy2*dCurrentTimedTF*timeJac(idx, 0);

         if (tempValue != 0.0)
            SparseMatrixUtil::SetElement(parQMatrix, conStartIdx + idx, 
                                         1, tempValue);

         //  Fill in state Jacobian elements
         if (ptrConfig->HasStateVars() == true)
         {
            for (UnsignedInt colIdx = 0; colIdx < stateIdxs.size(); ++colIdx)
            {
               tempValue = stateJac(idx, colIdx);
               if (tempValue != 0.0)
               {
                  SparseMatrixUtil::SetElement(parQMatrix, conStartIdx + idx,
                     stateIdxs[colIdx], -dtBy2*tempValue);
               }
            }
         }

         //  Fill in control Jacobian terms
         if (ptrConfig->HasControlVars() == true)
         {
            for (UnsignedInt colIdx = 0; colIdx < controlIdxs.size(); ++colIdx)
            {

               tempValue = controlJac(idx, colIdx);
               if (tempValue != 0.0)
               {
                  SparseMatrixUtil::SetElement(parQMatrix, conStartIdx + idx, 
                                       controlIdxs[colIdx], -dtBy2*tempValue);
               }
            }
         }

         //  Fill in static Jacobian elements
         if (ptrConfig->HasStaticVars() == true)
         {
            for (UnsignedInt colIdx = 0; colIdx < staticIdxs.size(); ++colIdx)
            {
               tempValue = staticJac(idx, colIdx);
               if (tempValue != 0.0)
               {
                  SparseMatrixUtil::SetElement(parQMatrix, conStartIdx + idx,
                     staticIdxs[colIdx], -dtBy2*tempValue);
               }
            }
         }
      }
   }
   #ifdef DEBUG_RADAU
      MessageInterface::ShowMessage(
                        "LEAVING Radau::FillDynamicDefectConMatrices\n");
   #endif
}

//------------------------------------------------------------------------------
// void FillDynamicCostMatrices(
//                         Rvector &QVector, RSMatrix &parQMatrix,
//                         std::vector<FunctionOutputData> *ptrFuncDataArray)
//------------------------------------------------------------------------------
/**
 * Computes Q and partial Q Matrix for integral cost.
 *
 * @param <QVector>          output: the defect Q vector
 * @param <parQMatrix>       output: the defect jacobian matrix
 * @param <ptrFuncDataArray> input:  user defect function data array
 */
//------------------------------------------------------------------------------
void NLPFuncUtilRadau::FillDynamicCostFuncMatrices(
						   const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                     Rvector &QVector, RSMatrix &parQMatrix)
{
   Integer idxLB = 0, idxUB;
   idxUB = ptrFuncDataArray.size() - 1; // except the last point
   //  Loop initializations
   Real dtBy2 = 0.5*(deltaTime);
   Real tempValue, dCurrentTimedTI, dCurrentTimedTF;

   QVector.SetSize(numMeshPoints);

   // set the sparsity pattern of the parQMatrix before setting values to
   // save time.
   const RSMatrix* ptrDMatrix = CostNLPData.GetDMatrixPtr();
   bool hasZeros = true;
   parQMatrix = SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   //  Loop over the function data and concatenate Q and parQ
   //  matrix terms
   Integer meshIdx;
   Rvector funcValueVec;
   Rmatrix timeJac, stateJac, controlJac, staticJac;
   IntegerArray stateIdxs, controlIdxs, staticIdxs;

   if (ptrConfig->HasStaticVars() == true)
   {
      // static data idxs is same for all points; YK
      staticIdxs = ptrFuncDataArray.at(0)->GetStaticIdxs();
   }

   for (Integer funcIdx = idxLB; funcIdx < idxUB; ++funcIdx)
   {
      meshIdx = ptrFuncDataArray.at(funcIdx)->GetMeshIdx();

      // the second input is just meaningless
      dCurrentTimedTI = GetdCurrentTimedTI(meshIdx);
      dCurrentTimedTF = GetdCurrentTimedTF(meshIdx);

      funcValueVec = ptrFuncDataArray.at(funcIdx)->GetFunctionValues();
      timeJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::TIME);
      stateJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::STATE);
      controlJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::CONTROL);
      staticJac = ptrFuncDataArray.at(funcIdx)->GetJacobian(UserFunction::STATIC);

      if (ptrConfig->HasStateVars() == true)
      {
         stateIdxs = ptrFuncDataArray.at(funcIdx)->GetStateIdxs();
      }
      if (ptrConfig->HasControlVars() == true)
      {
         controlIdxs = ptrFuncDataArray.at(funcIdx)->GetControlIdxs();
      }

      //  Fill in the function data; QVector
      QVector(funcIdx) = -dtBy2*funcValueVec(0);

      //  Initial time terms
      
      tempValue = 0.5*funcValueVec(0) - dtBy2*dCurrentTimedTI*timeJac(0, 0);
      if (tempValue != 0.0)
         SparseMatrixUtil::SetElement(parQMatrix, funcIdx, 0, tempValue);

      //  Final time terms
      tempValue = -0.5*funcValueVec(0) - dtBy2*dCurrentTimedTF*timeJac(0, 0);
      if (tempValue != 0.0)
         SparseMatrixUtil::SetElement(parQMatrix, funcIdx, 1, tempValue);

      //  Fill in state Jacobian elements
      if (ptrConfig->HasStateVars() == true)
      {
         for (UnsignedInt colIdx = 0; colIdx < stateIdxs.size(); ++colIdx)
         {
            tempValue = stateJac(0, colIdx);
            if (tempValue != 0)
            {
               SparseMatrixUtil::SetElement(parQMatrix,funcIdx,
                                            stateIdxs[colIdx],
                                            -dtBy2*tempValue);
            }
         }
      }

      //  Fill in control Jacobian terms
      if (ptrConfig->HasControlVars() == true)
      {
         for (UnsignedInt colIdx = 0; colIdx < controlIdxs.size(); ++colIdx)
         {
            tempValue = controlJac(0, colIdx);
            if (tempValue != 0)
            {
               SparseMatrixUtil::SetElement(parQMatrix,funcIdx,
                                            controlIdxs[colIdx],
                                            -dtBy2*tempValue);
            }
         }
      }

      //  Fill in static Jacobian elements
      if (ptrConfig->HasStaticVars() == true)
      {
         for (UnsignedInt colIdx = 0; colIdx < staticIdxs.size(); ++colIdx)
         {
            tempValue = staticJac(0, colIdx);
            if (tempValue != 0.0)
            {
               SparseMatrixUtil::SetElement(parQMatrix, funcIdx,
                  staticIdxs[colIdx], -dtBy2*tempValue);
            }
         }
      }
   }
}
