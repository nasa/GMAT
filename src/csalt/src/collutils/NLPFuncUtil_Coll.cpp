//------------------------------------------------------------------------------
//                         NLPFuncUtil_Coll Class
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
// Created: 2016/02/12
//
/**
 * Definition for the NLPFuncUtil_Coll class
 */
//------------------------------------------------------------------------------

#include "NLPFuncUtil_Coll.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_COLL
//#define DEBUG


//------------------------------------------------------------------------------
//   NLPFuncUtil_Coll()
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtil_Coll::NLPFuncUtil_Coll() :
   NLPFuncUtil(),
   numPathConstraintPoints       (0),
   numControlPoints              (0),
   numMeshPoints                 (0),
   numStateStagePointsPerMesh    (0),
   numControlStagePointsPerMesh  (0),
   numStatePoints                (0),
   numStagePoints                (0),
   numStagePointsPerMesh         (0),
   numTimePoints                 (0),
   deltaTime                     (0.0),
   phaseNum                      (-1),
   relErrorTol                   (1.0e-5),
   quadratureType                (0),
   ptrPathFunctionManager        (NULL),
   ptrConfig                     (NULL),
   isFinalized                   (false),
   isConMatInitialized           (false),
   isCostMatInitialized          (false)
{
   
}

//------------------------------------------------------------------------------
//   NLPFuncUtil_Coll(const  NLPFuncUtil_Coll &col)
//------------------------------------------------------------------------------
/**
 * Copy Constructor of trans util; 
 * 
 */
//------------------------------------------------------------------------------
NLPFuncUtil_Coll::NLPFuncUtil_Coll(const NLPFuncUtil_Coll &col) :
   NLPFuncUtil(col),
   numPathConstraintPoints       (col.numPathConstraintPoints),
   numControlPoints              (col.numControlPoints),
   numMeshPoints                 (col.numMeshPoints),
   numStateStagePointsPerMesh    (col.numStateStagePointsPerMesh),
   numControlStagePointsPerMesh  (col.numControlStagePointsPerMesh),
   numStatePoints                (col.numStatePoints),
   numStagePoints                (col.numStagePoints),
   numStagePointsPerMesh         (col.numStagePointsPerMesh),
   numTimePoints                 (col.numTimePoints),
   deltaTime                     (col.deltaTime),
   phaseNum                      (col.phaseNum),
   relErrorTol                   (col.relErrorTol),
   quadratureType                (col.quadratureType),
   ptrPathFunctionManager        (col.ptrPathFunctionManager),
   ptrConfig                     (col.ptrConfig),
   isFinalized                   (col.isFinalized),
   isConMatInitialized           (col.isConMatInitialized),
   isCostMatInitialized          (col.isCostMatInitialized)
{
   // ==== Data defining dimensions of transcription properties
        
   /// Unscaled discretization points for transcription
   discretizationPoints.SetSize(col.discretizationPoints.GetSize());
   discretizationPoints = col.discretizationPoints;

   /// Vector of discretization times.
   if (col.timeVector.IsSized() == true)
   {
      timeVector.SetSize( col.timeVector.GetSize());
      timeVector = col.timeVector;   
   }
   if (col.quadratureWeights.IsSized() == true)
   {
      quadratureWeights.SetSize(col.quadratureWeights.GetSize());
      quadratureWeights = col.quadratureWeights;
   }

   /// An array of flags indicating the type of data at
   /// each discretization point.  State, control, or state+control
   timeVectorType      = col.timeVectorType;
        
   /// ==== Helper classes

   ///  Utility class to manage defect constraints
   DefectNLPData             = col.DefectNLPData;
   ///  Utility class to manage cost function quadrature
   CostNLPData               = col.CostNLPData;
   /// Pointer to FunctionInputData object.
   pathFuncInputData         = col.pathFuncInputData;
   /// for mesh refinement
   userPathFunctionContainer = col.userPathFunctionContainer;
   /// PathFunctionManager object.
   ptrPathFunctionManager    = col.ptrPathFunctionManager;  // need to CLONE?

   /// Contains data used accross utility classes
   ptrConfig                 = col.ptrConfig;
        
   // Initialize matrices that get re-used
   if (ptrConfig->HasDefectCons() == true)
      dynFuncProps = col.dynFuncProps;

   if (ptrConfig->HasIntegralCost() == true)
      costFuncProps = col.costFuncProps;
}

//------------------------------------------------------------------------------
//  NLPFuncUtil_Coll&   operator= (const NLPFuncUtil_Coll &col)
//------------------------------------------------------------------------------
/**
 * Assignment operator of NLPFuncUtil_Coll.
 * 
 */
//------------------------------------------------------------------------------
NLPFuncUtil_Coll&   NLPFuncUtil_Coll::operator=(const NLPFuncUtil_Coll &col)
{
   if (this == &col)
      return *this;
   
   NLPFuncUtil::operator=(col);
   
   numPathConstraintPoints        = col.numPathConstraintPoints;
   numControlPoints               = col.numControlPoints;
   numMeshPoints                  = col.numMeshPoints;
   numStateStagePointsPerMesh     = col.numStateStagePointsPerMesh;
   numControlStagePointsPerMesh   = col.numControlStagePointsPerMesh;
   numStatePoints                 = col.numStatePoints;
   numStagePoints                 = col.numStagePoints;
   numStagePointsPerMesh          = col.numStagePointsPerMesh;
   numTimePoints                  = col.numTimePoints;
   quadratureType                 = col.quadratureType;
   deltaTime                      = col.deltaTime;
   phaseNum                       = col.phaseNum;
   relErrorTol                    = col.relErrorTol;
   ptrPathFunctionManager         = col.ptrPathFunctionManager;
   ptrConfig                      = col.ptrConfig;
   isFinalized                    = col.isFinalized;
   isConMatInitialized            = col.isConMatInitialized;
   isCostMatInitialized           = col.isCostMatInitialized;
   
   // NOTE: we could make a CopyData method that both the copy constructor and
   // operator= could call to do all this
   // ==== Data defining dimensions of transcription properties
        
   /// Unscaled discretization points for transcription
   discretizationPoints.SetSize(col.discretizationPoints.GetSize());
   discretizationPoints = col.discretizationPoints;
   
   /// Vector of discretization times.
   if (col.timeVector.IsSized() == true)
   {
      timeVector.SetSize( col.timeVector.GetSize());
      timeVector = col.timeVector;   
   }

   if (col.quadratureWeights.IsSized() == true)
   {
      quadratureWeights.SetSize(col.quadratureWeights.GetSize());
      quadratureWeights = col.quadratureWeights;
   }

   /// An array of flags indicating the type of data at
   /// each discretization point.  State, control, or state+control
   timeVectorType                   = col.timeVectorType;
        
   /// ==== Helper classes

   ///  Utility class to manage defect constraints
   DefectNLPData             = col.DefectNLPData;
   ///  Utility class to manage cost function quadrature
   CostNLPData               = col.CostNLPData;
   /// Pointer to FunctionInputData object.
   pathFuncInputData         = col.pathFuncInputData;
   /// for mesh refinement
   userPathFunctionContainer = col.userPathFunctionContainer;
   /// Pointer to PathFunctionManager object.
   ptrPathFunctionManager    = col.ptrPathFunctionManager;

   /// Contains data used accross utility classes
   ptrConfig                 = col.ptrConfig;
        
   // Initialize matrices that get re-used
   if (ptrConfig->HasDefectCons() == true)
      dynFuncProps = col.dynFuncProps;

   if (ptrConfig->HasIntegralCost() == true)
      costFuncProps = col.costFuncProps;

   return *this;
}

NLPFuncUtil_Coll::~NLPFuncUtil_Coll()
{
   /* do nothing */
}

//------------------------------------------------------------------------------
//void Initialize(ProblemCharacteristics *ptrCfg)
//------------------------------------------------------------------------------
/**
* Thsi method performs the first half of the initialization.
*
* @param <ptrCfg> the problem characteristc of optimization problem
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::Initialize(ProblemCharacteristics *ptrCfg)
{     
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage("In Util_Coll::Initialize ...\n");
   #endif
   //  Set pointer to config object
   ptrConfig = ptrCfg;

   //  Computes dimensions of various items based on the
   //  transcription details
   InitializeTranscription();
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
               "In Util_Coll::Initialize, done with InitializeTranscription\n");
   #endif

   //  Initialize utilities used to compute function values and
   //  Jacobians given appropriate sparse matrices in Bett's
   //  formulation
   InitNLPHelpers();
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage("Leaving Util_Coll::Initialize ...\n");
   #endif
}

//------------------------------------------------------------------------------
//void PrepareToOptimize(UserFunctionProperties          dynFunProp,
//  				          std::vector<FunctionOutputData> *dynFunValues,
//  					       UserFunctionProperties          costFunProps,
//  					       std::vector<FunctionOutputData> *costFunValues)
//------------------------------------------------------------------------------
/**
* Initializations that must be done after other subsystems intialized
* (after Initialize() above is done)
*
* @param <dynFuncProp>    a UserFunProperties Objects of dynamics
* @param <dynFunValues>   FunctionOutputData array of dyn function
* @param <costFuncProps>  a UserFunProperties Objects of cost
* @param <costFunValues>  FunctionOutputData array of cost function
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::PrepareToOptimize(UserFunctionProperties dynFunProp,
					        const std::vector<FunctionOutputData*>   &dynFunValues,
						     UserFunctionProperties                   costFunProp,
						     const std::vector<FunctionOutputData*>   &costFunValues)
{
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                              "Entering Util_Coll::PrepareToOptimize ...\n");
   #endif
   dynFuncProps = dynFunProp;
   InitializeConstantDefectMatrices(&dynFuncProps,dynFunValues);   

   costFuncProps = costFunProp;
   InitializeConstantCostMatrices(&costFuncProps,costFunValues);
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                              "LEAVING Util_Coll::PrepareToOptimize ...\n");
   #endif
}

//------------------------------------------------------------------------------
//void PrepareToOptimize(UserFunctionProperties                 usrFunProp,
//                       const std::vector<FunctionOutputData*> &usrFunValues)
//------------------------------------------------------------------------------
/**
* Initializations that must be done after other subsystems intialized
* (after Initialize() above is done)
*
* @param <usrFunProp>    the user funciton properties
* @param <usrFunValues>  FunctionOutputData array of user functions
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::PrepareToOptimize(UserFunctionProperties usrFunProp,
                        const std::vector<FunctionOutputData*>  &usrFunValues)
{
      #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                           "Entering Util_Coll::PrepareToOptimize (2) ...\n");
      MessageInterface::ShowMessage(
                        "   has defect = %s\n",
                        (ptrConfig->HasDefectCons() ? "true" : "false"));
      MessageInterface::ShowMessage("   has cost   = %s\n",
                        (ptrConfig->HasIntegralCost() ? "true" : "false"));
      #endif
   // Initialize matrices that get re-used
   if (ptrConfig->HasDefectCons() == true)
   {
      dynFuncProps = usrFunProp;
      #ifdef DEBUG_COLL
         MessageInterface::ShowMessage(
                  "--- About to call InitializeConstantDefectMatrices ...\n");
      #endif
      InitializeConstantDefectMatrices(&dynFuncProps, usrFunValues);
   }
   if (ptrConfig->HasIntegralCost() == true)
   {
      costFuncProps = usrFunProp;
      #ifdef DEBUG_COLL
         MessageInterface::ShowMessage(
                     "--- About to call InitializeConstantCostMatrices ...\n");
      #endif
      InitializeConstantCostMatrices(&costFuncProps, usrFunValues);
   }
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                           "LEAVING Util_Coll::PrepareToOptimize (2) ...\n");
   #endif
}

//------------------------------------------------------------------------------
// FunctionOutputData* NLPFuncUtil_Coll::GetUserDynFuncData(Real time,
//                                                          Rvector stateVec,
//                                                          Rvector controlVec)
//------------------------------------------------------------------------------
/**
* Given time, state, and control: calls user function and returns
* dynamics function at the given point.
*
* @param <time>       the time point of interest
* @param <stateVec>   the state vector at the given time
* @param <controlVec> the control vector at the given time
*
* @return the pointer of userPathFunctionData at the given point
*/
//------------------------------------------------------------------------------
FunctionOutputData* NLPFuncUtil_Coll::GetUserDynFuncData(Real time,
														               Rvector stateVec,
														               Rvector controlVec)
{
   PreparePathFunctionOffMesh(time, stateVec, controlVec);
   // Call the phase and ask it to evaluate the function   

   try
   {
      ptrPathFunctionManager->EvaluateUserFunction(&pathFuncInputData,
                                                   &userPathFunctionContainer);
   }
   catch (BaseException &be)
   {
      std::stringstream errmsg;
      errmsg << "For NLPFuncUtil_Coll::GetUserDynFuncData,";
      errmsg << "fail to evalute user path function.";
      throw LowThrustException(errmsg.str());
   }

   // Now get the data
   return userPathFunctionContainer.GetDynData();
}

//------------------------------------------------------------------------------
// void NLPFuncUtil_Coll::PreparePathFunctionOffMesh(Real time,
//                                                   Rvector stateVec,
//                                                   Rvector controlVec)
//------------------------------------------------------------------------------
/**
* Prepare path function object out of any mesh point
*
* @param <time>       the time point of interest
* @param <stateVec>   the state vector at the given time
* @param <controlVec> the control vector at the given time
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::PreparePathFunctionOffMesh(Real time,
												              Rvector stateVec,
												              Rvector controlVec)
{
   pathFuncInputData.SetStateVector(stateVec);
   pathFuncInputData.SetControlVector(controlVec);
   // YK mod static vars
   if (ptrConfig->GetNumStaticVars() > 0)
      pathFuncInputData.SetStaticVector(ptrConfig->GetStaticVector());

   pathFuncInputData.SetTime(time);
}


//------------------------------------------------------------------------------
// void ComputeDefectFunAndJac(
//                   std::vector<FunctionOutputData> *ptrFuncDataArray,
//                   DecVecTypeBetts                 *DecVector,
//                   Rvector                         &funcValues,
//                   RSMatrix                        &jacArray)
//------------------------------------------------------------------------------
/**
* Computes defect constraints using Bett's formulation
*
* @param <ptrFuncDataArray>   path function data array
* @param <DecVector>          the decision vector of the phase
* @param <funcValues>         output: the defect function vector
* @param <jacArray>           output: the defect jacobian matrix
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::ComputeDefectFunAndJac(
                       const std::vector<FunctionOutputData*> &ptrFuncDataArray,
                       DecVecTypeBetts                        *DecVector,
                       Rvector                                &funcValues,
                       RSMatrix                               &jacArray)
{
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                              "Entering NLPColl::ComputeDefectFunAndJac ...\n");
      MessageInterface::ShowMessage(
                              "   isConMatInitialized = %s\n",
                              (isConMatInitialized? "true" :"false"));
   #endif
   if (isConMatInitialized == false)
   {
      InitializeConstantDefectMatrices(&dynFuncProps, ptrFuncDataArray);
   }

   if (ptrConfig->HasDefectCons())
   {
      #ifdef DEBUG_COLL
         MessageInterface::ShowMessage(
                        "In NLPColl::ComputeDefectFunAndJac, hasDefectCons\n");
      #endif
      Rvector QVector;
      const Rvector *ptrDecVectorData;
      
      ptrDecVectorData = DecVector->GetDecisionVectorPointer();
      RSMatrix parQMatrix;
      #ifdef DEBUG_COLL
         MessageInterface::ShowMessage(
                  "In NLPColl::ComputeDefectFunAndJac, calling FillDynamic\n");
      #endif
      FillDynamicDefectConMatrices(ptrFuncDataArray, QVector, parQMatrix);


      #ifdef DEBUG
         MessageInterface::ShowMessage(
                        "the parQMatrix of defect constraints is given as \n");
         SparseMatrixUtil::PrintNonZeroElements(&parQMatrix);
         Rvector decisionVector = DecVector->GetDecisionVector();
         MessageInterface::ShowMessage("DecisionVector is given as \n%s\n",
                           decisionVector.ToString(12).c_str());

         MessageInterface::ShowMessage(
                           "the QVector of defect constraints is given as \n");
            for (Integer idx = 0; idx < QVector.GetSize(); ++idx)
               MessageInterface::ShowMessage("%le\n", QVector(idx));
      #endif

      DefectNLPData.ComputeFunctions(&QVector, ptrDecVectorData, funcValues);
      DefectNLPData.ComputeJacobian(&parQMatrix, jacArray);
   }
   #ifdef DEBUG_COLL
      MessageInterface::ShowMessage(
                              "LEAVING NLPColl::ComputeDefectFunAndJac ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeCostFunAndJac(std::vector<FunctionOutputData> *ptrFuncDataArray,
//                           DecVecTypeBetts                 *DecVector,
//                           Rvector                         &costValue,
//                           RSMatrix                        &jacArray)
//------------------------------------------------------------------------------
/**
* Computes integral cost using Bett's formulation
*
* @param <ptrFuncDataArray>   cost function data array
* @param <DecVector>          the decision vector of the phase
* @param <costValues>         output: the integral cost value (scalar)
* @param <jacArray>           output: the cost jacobian matrix (gradient)
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::ComputeCostFunAndJac(
                     const std::vector<FunctionOutputData*>  &ptrFuncDataArray,
                     DecVecTypeBetts                         *DecVector,
                     Rvector                                 &costValue,
                     RSMatrix                                &jacArray)
{
   if (isCostMatInitialized == false)
      InitializeConstantCostMatrices(&costFuncProps, ptrFuncDataArray);

   if (ptrConfig->HasIntegralCost())
   {
      Rvector QVector;
      //const Rvector *ptrDecVectorData;
      //ptrDecVectorData = DecVector->GetDecisionVectorPointer();
      RSMatrix parQMatrix;
      FillDynamicCostFuncMatrices(ptrFuncDataArray, QVector, parQMatrix);


      #ifdef DEBUG
         MessageInterface::ShowMessage("the parQMatrix of cost is given as \n");
         SparseMatrixUtil::PrintNonZeroElements(&parQMatrix);
   
         MessageInterface::ShowMessage("the QVector of cost is given as \n");
         for (Integer idx = 0; idx < QVector.GetSize(); ++idx)
            MessageInterface::ShowMessage("%le\n", QVector(idx));
      #endif

      CostNLPData.ComputeFunctions(&QVector, costValue);
      CostNLPData.ComputeJacobian(&parQMatrix, jacArray);
   }
}

//------------------------------------------------------------------------------
// void SetPhaseNum(Integer inputNum)
//------------------------------------------------------------------------------
/**
* Sets the phase index that this NLP function util is associated with
*
* @param <inputNum> the phase index
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::SetPhaseNum(Integer inputNum)
{
   phaseNum = inputNum;
}


//------------------------------------------------------------------------------
// void SetRelativeErrorTol(Integer inputNum)
//------------------------------------------------------------------------------
/**
 * Sets the mesh points size increasement control parameter
 *
 * @param <toNum> the index of the phase
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::SetRelativeErrorTol(Real toNum)
{
   relErrorTol = toNum;  // @todo add validation here
}

//------------------------------------------------------------------------------
// Real NLPFuncUtil_Coll::GetTimeAtMeshPoint(Integer meshIdx)
//------------------------------------------------------------------------------
/**
* Get time of given mesh point
*
* @param <meshIdx> the index of the mesh point for which to return the time
*
* @return the time of the specified mesh point
*/
//------------------------------------------------------------------------------
Real NLPFuncUtil_Coll::GetTimeAtMeshPoint(Integer meshIdx)
{
   if (meshIdx > timeVector.GetSize())
   {
      std::stringstream errmsg;
	  errmsg << "For NLPFuncUtil_Coll::GetTimeAtMeshPoint,";
      errmsg << "meshIdx must be equal to or less than";
	  errmsg << " the number of elements in timeVector";
      throw LowThrustException(errmsg.str());      
   }
   return timeVector(meshIdx);
}     

//------------------------------------------------------------------------------
// RSMatrix* ComputeDefectSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Returns sparsity pattern of defect constraints
 *
 * @return the sparsity pattern of defect constraints
 */
//------------------------------------------------------------------------------
RSMatrix* NLPFuncUtil_Coll::ComputeDefectSparsityPattern()
{
   return DefectNLPData.GetJacSparsityPatternPointer();
}

//------------------------------------------------------------------------------
// RSMatrix* ComputeCostSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Returns sparsity pattern of integral cost component
 *
 * @return the sparsity pattern of integral cost component
 */
//------------------------------------------------------------------------------
RSMatrix* NLPFuncUtil_Coll::ComputeCostSparsityPattern()
{
   return CostNLPData.GetJacSparsityPatternPointer();
}

//------------------------------------------------------------------------------
// IntegerArray GetDefectMatrixNumNonZeros()
//------------------------------------------------------------------------------
/**
 * Returns an array of numbers of zeros for A, B, Q
 *
 * @return an array of number of zeros [numAZeros,numBZeros,numQZeros]
 */
//------------------------------------------------------------------------------
IntegerArray NLPFuncUtil_Coll::GetDefectMatrixNumNonZeros()
{
   return DefectNLPData.GetMatrixNumNonZeros();
}

//------------------------------------------------------------------------------
// Rvector GetDiscretizationPoints()
//------------------------------------------------------------------------------
/**
 * Returns a vector of discretization points
 *
 * @return a vector of number of discretization points
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_Coll::GetDiscretizationPoints()
{
   return discretizationPoints;
};

//------------------------------------------------------------------------------
// Integer GetNumStatePoints()
//------------------------------------------------------------------------------
/**
 * Returns the number of state points
 *
 * @return the number of state points
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumStatePoints()
{
   return numStatePoints;
};

//------------------------------------------------------------------------------
// Integer GetNumControlPoints()
//------------------------------------------------------------------------------
/**
 * Returns the number of control points
 *
 * @return the number of control points
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumControlPoints()
{
   return numControlPoints;
};

//------------------------------------------------------------------------------
// Integer GetNumStateStagePointsPerMesh()
//------------------------------------------------------------------------------
/**
 * Returns the number of state stage points per mesh
 *
 * @return the number of state stage points per mesh
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumStateStagePointsPerMesh()
{
   return numStateStagePointsPerMesh;
};

//------------------------------------------------------------------------------
// Integer GetNumControlStagePointsPerMesh()
//------------------------------------------------------------------------------
/**
 * Returns the number of control stage points per mesh
 *
 * @return the number of control stage points per mesh
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumControlStagePointsPerMesh()
{
   return numControlStagePointsPerMesh;
};

//------------------------------------------------------------------------------
// Integer GetNumPathConstraintPoints()
//------------------------------------------------------------------------------
/**
 * Returns the number of path constraint points
 *
 * @return the number of path constraint points
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumPathConstraintPoints()
{
   return numPathConstraintPoints;
};

//------------------------------------------------------------------------------
// Rvector GetTimeVector()
//------------------------------------------------------------------------------
/**
 * Returns the time vector
 *
 * @return the time vector
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_Coll::GetTimeVector()
{
   return timeVector;
};

//------------------------------------------------------------------------------
// Integer GetNumTimePoints()
//------------------------------------------------------------------------------
/**
 * Returns the number of time points
 *
 * @return the number of time points
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_Coll::GetNumTimePoints()
{
   return numTimePoints;
};

//------------------------------------------------------------------------------
// IntegerArray GetTimeVectorType()
//------------------------------------------------------------------------------
/**
 * Returns an array of time vector types
 *
 * @return an arry of time vector types
 */
//------------------------------------------------------------------------------
IntegerArray NLPFuncUtil_Coll::GetTimeVectorType()
{
   return timeVectorType;
};

//------------------------------------------------------------------------------
// void NLPFuncUtil_Coll::PrepareToRefineMesh(
//                        UserPathFunctionManager *pathFunctionManager)
//------------------------------------------------------------------------------
/**
* Prepare to refine mesh points
*
* @param <pathFunctionManager> the user path function manager used for 
*                              mesh refinement 
*
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_Coll::PrepareToRefineMesh(
                       UserPathFunctionManager *pathFunctionManager)
{  
   ptrPathFunctionManager = pathFunctionManager;
   pathFuncInputData.Initialize(ptrConfig->GetNumStateVars(), 
                                ptrConfig->GetNumControlVars(),
                                ptrConfig->GetNumStaticVars());
   pathFuncInputData.SetPhaseNum(phaseNum);
   userPathFunctionContainer.Initialize();
};
