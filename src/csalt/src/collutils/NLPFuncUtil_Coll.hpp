//------------------------------------------------------------------------------
//                         NLPFuncUtil_Coll
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
// Created: 2016/03/01
//
/**
 * Definition for the NLPFuncUtil_Coll class
 * ==== Important Behavior Notes.
 *
 * This class has a two step intialization process.  You must first call
 * Initialize(), which will populate data on the input
 * ProbCharacteristics class.  Then you must evaluate user functions
 * according to the problem characteristics, and send user function info
 * into PrepareToOptimize() to complete initialization.
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_Coll_hpp
#define NLPFuncUtil_Coll_hpp

//#include "NLPFuncUtil.hpp" // this class is now empty class

#include "csaltdefs.hpp"
#include "NLPFuncUtil.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "SparseMatrixUtil.hpp"
#include "ProblemCharacteristics.hpp"
#include "PathFunctionContainer.hpp"
#include "NLPFunctionData.hpp"
#include "UserFunctionProperties.hpp"
#include "UserPathFunction.hpp"
#include "UserPathFunctionManager.hpp"
#include "FunctionOutputData.hpp"
#include "FunctionInputData.hpp"
#include "DecVecTypeBetts.hpp"
#include "UserPathFunction.hpp"

/**
 * NLPFuncUtil_Coll class
 */
class NLPFuncUtil_Coll : public NLPFuncUtil
{
public:
   // default constructors and destructors =====================================
   /// default constructor
   NLPFuncUtil_Coll();
   
   /// copy constructor
   NLPFuncUtil_Coll(const NLPFuncUtil_Coll &col);
   
   /// assignment operator
   NLPFuncUtil_Coll&   operator=(const NLPFuncUtil_Coll &col);

   /// virtual destructor
   virtual ~NLPFuncUtil_Coll();

   // methods ==================================================================

   /// Performs the first half of the initialization.
   virtual void Initialize(ProblemCharacteristics *ptrCfg);


   ///  Initializations that must be done after other subsystems are
   ///  initialized (after Initialize() above)
   ///  the case that the problem has both cost and defect
   void PrepareToOptimize(UserFunctionProperties           dynFunProps,
					     const std::vector<FunctionOutputData*> &dynFunValues,
						  UserFunctionProperties                 costFunProps,
						  const std::vector<FunctionOutputData*> &costFunValues);

   ///  Initializations that must be done after other subsystems
   ///  intialized (after Initialize() above)
   ///  the case that the problem has only cost 
   void PrepareToOptimize(UserFunctionProperties            functionProps,
                     const std::vector<FunctionOutputData*> &functionValues);

   void PrepareToRefineMesh(UserPathFunctionManager *pathFunctionManager);

   /// Given time, state, and control: calls user function and returns
   /// dynamics functions at the point.
   FunctionOutputData* GetUserDynFuncData(Real time, Rvector stateVec, 
										            Rvector controlVec);
     
   ///  Prepares user path function evaluation at a specific point
   void PreparePathFunctionOffMesh(Real time, Rvector stateVec, 
                                   Rvector ControlVec);
   
   /// Computes defect constraints and cost functions using Bett's formulation
   void ComputeDefectFunAndJac(
                     const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                     DecVecTypeBetts                        *DecVector,
                     Rvector                                &funcValues,
                     RSMatrix                               &jacArray);
   void ComputeCostFunAndJac(
                     const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                     DecVecTypeBetts                        *DecVector,
                     Rvector                                &funcValue,
                     RSMatrix                               &jacArray);

   void SetPhaseNum(Integer inputNum);
   void SetRelativeErrorTol(Real toNum);

   // Get methods ==============================================================
   
   ///  Returns time as mesh points given mesh index, real
   Real         GetTimeAtMeshPoint(Integer meshIdx);


   /// Returns sparsity pattern of defect constraints   
   RSMatrix*    ComputeDefectSparsityPattern();
   /// Returns sparsity pattern of integral cost component
   /// should we rename it as GetCostSparsityPattern()?
   RSMatrix*    ComputeCostSparsityPattern();

   /// IntegerArray = [numAZeros,numBZeros,numQZeros];
   IntegerArray GetDefectMatrixNumNonZeros();

   ///  Returns discretization points <inline>
   Rvector      GetDiscretizationPoints();
   ///  Returns total number of discretization points that have state
   Integer      GetNumStatePoints();
   ///  Returns number of mesh points that have control
   Integer      GetNumControlPoints();
   ///  Returns numer of state stage points in a mesh
   Integer      GetNumStateStagePointsPerMesh();
   ///  Returns the number of control state points in a mesh
   Integer      GetNumControlStagePointsPerMesh();
   ///  Returns total number of discretization points with path contraints
   Integer      GetNumPathConstraintPoints();
   ///  Returns array of times in the discrization 
   Rvector      GetTimeVector();
   ///  Returns the number of times in the time vector, integer
   Integer      GetNumTimePoints();
   ///  Returns vector of bools defining discretization point type
   IntegerArray GetTimeVectorType();

   // virtual methods ==========================================================
   
   /// Partial of time at point k with respect to initial time
   virtual  Real GetdCurrentTimedTI(Integer pointIdx, Integer stageIdx = 0) = 0;

   /// Partial of time at point k with respect to initial time
   virtual  Real GetdCurrentTimedTF(Integer pointIdx, Integer stageIdx = 0) = 0;

   virtual void    SetTimeVector(Real initialTime, Real finalTime) = 0;

   virtual Integer GetMeshIndex(Integer pointIdx) = 0;

   virtual Integer GetStageIndex(Integer pointIdx) = 0;

   virtual void    RefineMesh(DecVecTypeBetts         *ptrDecVector,
                              UserPathFunctionManager *ptrFunctionManager,
                              Rvector                 *oldRelErrorArray,
                              bool                    &isMeshRefined,
                              IntegerArray            &newMeshIntervalNumPoints,
                              Rvector                 &newMeshIntervalFractions,
                              Rvector                 &maxRelErrorArray,
                              Rmatrix                 &newStateGuess,
                              Rmatrix                 &newControlGuess) = 0;

   ///  Initialize the transcription
   virtual void InitializeTranscription() = 0;

   ///  Initialize NLP helper utilities for cost and defect calculations
   virtual void InitNLPHelpers() = 0;

   ///  Initialize interpolators for mesh-refinements
   virtual void InitStateAndControlInterpolator() = 0;

   ///  fill defect and cost matrices (A, B, and D matrices)
   virtual void InitializeConstantDefectMatrices(
                     UserFunctionProperties                 *dynFuncProps,
                     const std::vector<FunctionOutputData*> &dynFuncValues) = 0;
   virtual void InitializeConstantCostMatrices(
                  UserFunctionProperties                 *costFuncProps,
                  const std::vector<FunctionOutputData*> &costFuncValues) = 0;

   ///  fill defect and cost matrices (Q vector and partial Q matrix)
   virtual void FillDynamicCostFuncMatrices(
                     const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                     Rvector                                &valueVec,
                     RSMatrix                               &jacobian) = 0;
   virtual void FillDynamicDefectConMatrices(
                     const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                     Rvector                                &valueVec,
                     RSMatrix                               &jacobian) = 0;

   virtual void GetStateAndControlInMesh(
                     Integer                       meshIntvIdx,
                     DecVecTypeBetts               *ptrDecVector,
                     std::vector<Rvector>          &stateVecRvector,
                     std::vector<Rvector>          &controlVecRvector, 
                     bool                          hasFinalState,
                     bool                          hasFinalControl) = 0;

   virtual Rvector ComputeMaxMeshError(
                     DecVecTypeBetts               *ptrDecVector,
                     UserPathFunctionManager       *ptrFunctionManager) = 0;

protected:      
   // ==== Data defining dimensions of transcription properties
        
   /// Unscaled discretization points for transcription
   Rvector      discretizationPoints;
   /// Number of points where path constraints are applied
   Integer      numPathConstraintPoints;
   /// The number of discretization points that have controls
   Integer      numControlPoints;
   /// Number of mesh points in the transcription
   Integer      numMeshPoints;
   /// Number stage points that have states
   Integer      numStateStagePointsPerMesh;
   /// Number of stages that have control
   Integer      numControlStagePointsPerMesh;
   /// The number of discretization points that have states
   Integer      numStatePoints;
   /// The number of stage points in the transcription
   Integer      numStagePoints;
   /// Number of stages between mesh points
   Integer      numStagePointsPerMesh;
   /// Vector of discretization times.
   Rvector      timeVector;
   /// Number of points in the time vector
   Integer      numTimePoints;
   /// The time change from; the first time to last time in phase.
   Real         deltaTime;
   /// An array of flags indicating the type of data at
   /// each discretization point.  State, control, or state+control
   IntegerArray timeVectorType;
   /// The phase index that this NLP function util is associated with
   Integer      phaseNum;
   
   /// mesh points size increasement control parameter
   Real     relErrorTol; // default value is 1E-5

   
   Integer      quadratureType;
   Rvector      quadratureWeights;
   /// ==== Helper classes

   ///  Utility class to manage defect constraints
   NLPFunctionData         DefectNLPData;
   ///  Utility class to manage cost function quadrature
   NLPFunctionData         CostNLPData;
   /// Pointer to FunctionInputData object.
   FunctionInputData       pathFuncInputData;
   /// for mesh refinement
   PathFunctionContainer   userPathFunctionContainer;
   /// Pointer to PathFunctionManager object.
   ///// PathFunctionManager *PathFunctionManager;
   UserPathFunctionManager *ptrPathFunctionManager; 

   /// Contains data used accross utility classes
   ProblemCharacteristics  *ptrConfig;
        
   /// ==== New for three vector handling of NLP data
   /// UserFunProperties Object. Contains info on user functions
   /// they look redundant, are not they?
   UserFunctionProperties  dynFuncProps;
   UserFunctionProperties  costFuncProps;
   bool                    isFinalized; // it seems not used here

   ///  Indicates if constraint matrices have been initialized
   bool                    isConMatInitialized;
   ///  Indicates if cost matrices have been initialized
   bool                    isCostMatInitialized;
};
#endif // NLPFuncUtil_Coll_hpp
