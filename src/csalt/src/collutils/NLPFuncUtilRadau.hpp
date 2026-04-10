//------------------------------------------------------------------------------
//                         NLPFuncUtil_Radau
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
// Created: 2015/07/10
//
/**
 * Developed based on NLPFuncUtilRadau.m
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_Radau_hpp
#define NLPFuncUtil_Radau_hpp

#include <math.h>
#include "NLPFuncUtil_Coll.hpp"
#include "RadauMathUtil.hpp"
#include "BaryLagrangeInterpolator.hpp"

/**
 * NLPFuncUtil_Radau class
 */
class NLPFuncUtilRadau : public NLPFuncUtil_Coll
{
public:
   // default constructors and destructors =====================================

   /// default/detailed constructor
   NLPFuncUtilRadau(Integer minPolyDeg = 3, Integer maxPolyDeg = 14,
                    Real    RelTol     = 1E-5);
   
   /// copy constructor
   NLPFuncUtilRadau(const NLPFuncUtilRadau &copy);
   /// assignment operator
   NLPFuncUtilRadau& operator=(const NLPFuncUtilRadau &copy);

   /// default destructor
   virtual ~NLPFuncUtilRadau();
   
   // get methods ==============================================================

   /// Get the mesh index given the point index
   Integer GetMeshIndex(Integer pointIdx);

   /// Get the stage index given the point index
   Integer GetStageIndex(Integer pointIdx);

   /// Partial of time at point k with respect to initial time
   Real    GetdCurrentTimedTI(Integer pointIdx, Integer stageIdx = 0);

   /// Partial of time at point k with respect to initial time
   Real    GetdCurrentTimedTF(Integer pointIdx, Integer stageIdx = 0);

   // other methods ============================================================

   /// Initialize NLPFuncUtilRadau
   void InitializeTranscription();

   /// Computes the vector of discretization times
   void SetTimeVector(Real initialTime, Real finalTime);

   /// a method that define polynomial range (not used in MATLAB prototype)
   void SetPolynomialDegreeRange(Integer minPolynomialDegree,
                                 Integer maxPolynomialDegree);

   /// refine mesh based on error
   /// TBD: should be verified with multi-dimensional problems
   void RefineMesh(DecVecTypeBetts        *ptrDecVector,
                  UserPathFunctionManager *ptrPathFunctionManager,
                  Rvector                 *oldRelErrorArray,
                  bool                    &isMeshRefined,
                  IntegerArray            &newMeshIntervalNumPoints,
                  Rvector                 &newMeshIntervalFractions,
                  Rvector                 &maxRelErrorArray,
                  Rmatrix                 &newStateGuess,
                  Rmatrix                 &newControlGuess);

   Rvector ComputeMaxMeshError(DecVecTypeBetts *ptrDecVector,
                               UserPathFunctionManager *ptrFunctionManager);

protected:  
   // Parameters ===============================================================

   static const Integer minPolynomialDegreeLB;
   static const Integer maxPolynomialDegreeUB;
   
   /// scaled discretization points for transcription
   Rvector  radauPoints;
   /// radau transcription related 
   RSMatrix radauDiffSMatrix;

   /// polynomial degree related
   Integer  minPolynomialDegree; // default value is 3
   Integer  maxPolynomialDegree; // default value is 14

   /// data types related to mesh-refinement
   std::vector<BaryLagrangeInterpolator> stateInterpolatorArray;
   std::vector<BaryLagrangeInterpolator> controlInterpolatorArray;

   /// indicator of interpolation initialization
   bool     isInterpolatorsInitialized;

   // initialization methods ===================================================

   /// Function to set stage properties for the phase type
   void SetStageProperties();
   void InitNLPHelpers();
   /// initialize NLPFunctionData for cost and defect constraints
   void InitializeConstantDefectMatrices(
						UserFunctionProperties                 *dynFuncProps,
						const std::vector<FunctionOutputData*> &dynFuncValues);
   
   void InitializeConstantCostMatrices( 
						UserFunctionProperties                 *costFuncProps,
						const std::vector<FunctionOutputData*> &costFuncValues);
                  

   /// initialize interpolator arrays
   void InitStateAndControlInterpolator();

   // mesh-refinement methods ==================================================
   // TBD: these should be verified with multi-dimensional problems

   /// Gets state and control at all points in given mesh interval
   void GetStateAndControlInMesh(
                        Integer                       meshIntvIdx,
                        DecVecTypeBetts               *ptrDecVector,
                        std::vector<Rvector>          &stateVecRvector,
                        std::vector<Rvector>          &controlVecRvector,
                        bool                          hasFinalState = true,
                        bool                          hasFinalControl = false);

   /// obtain relative state error in a mesh interval
   Real GetMaxRelErrorInMesh(Integer          meshIntervalIdx,
                             DecVecTypeBetts *DecVector,
                             bool printErr = true);

   // other methods ============================================================
   
   void CheckMeshIntervalNumPoints();

   /// compute Q vector and partial Q matrix for defect and cost
   void FillDynamicDefectConMatrices(
						const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                  Rvector &valueVec, RSMatrix &jacobian);
   void FillDynamicCostFuncMatrices(
                  const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                  Rvector &valueVec, RSMatrix &jacobian);
};

#endif // NLPFuncUtilRadau_hpp
