//------------------------------------------------------------------------------
//                              NLPFuncUtil_ImplicitRK
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
* ImplicitRKPhase transcription class.
*
* This class computes the defect constraints, integralconstraint, and cost
* function quadratures using implicite Runge Kutta methods of the 
* "Separated" form.
*
* Ref [1].  Betts, John, T.  "Practical Methods for Optimal Control and
* Estimation Using Nonlinear Programming", 2nd Edition.
*
* Ref [2].  Betts, John, T.  "Using Direct Transcription to Compute Optimal 
* Low Thrust Transfers Between Libration Point Orbits", Posted on his website.
*
* The comments below define mesh/step points, stage points, and
* how bookeeping is performed for IRK methods.
* An example with a 6 order IRK with 3 steps looks like this:
* | = mesh/step of the integrator
* x = internal state of integrator
* Point Type  |  x  x  |  x  x  |  x  x  |
* MeshIdx     1  1  1  2  2  2  3  3  3  4
* StageIdx    0  1  2  0  1  2  0  1  2  0
* PointIdx    1  2  3  4  5  6  7  8  9  10
* Note:  3 steps means 4 mesh points: numSteps = numMeshPoints - 1
* Total num points = numSteps*(numStages + 1) + 1;  There are two
* stages in this example.
*/
//------------------------------------------------------------------------------

#include "NLPFuncUtil_ImplicitRK.hpp"

#include "NLPFunctionData.hpp"
#include "LobattoIIIA_8Order.hpp"
#include "LobattoIIIA_6Order.hpp"
#include "LobattoIIIA_4Order.hpp"
#include "LobattoIIIA_4HSOrder.hpp"
#include "LobattoIIIA_2Order.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MR
//#define DEBUG
//#define DEBUG_Matrices

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// NLPFuncUtil_ImplicitRK()
//------------------------------------------------------------------------------
/**
 * constructor
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtil_ImplicitRK::NLPFuncUtil_ImplicitRK(std::string method) :
   NLPFuncUtil_Coll(),
   butcherTableData (NULL)
{
   // Lobatto uses all points in the mesh.
   quadratureType = 1;

   SetButcherTable(method);

};
   

//------------------------------------------------------------------------------
// NLPFuncUtil_ImplicitRK(const NLPFuncUtil_ImplicitRK &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtil_ImplicitRK::NLPFuncUtil_ImplicitRK(
                                          const NLPFuncUtil_ImplicitRK &copy) :
   NLPFuncUtil_Coll(copy),
   butcherTableData (NULL)    // or Clone here??
{
   //  Real Array: number of mesh steps for each mesh interval
   numStepsInPhase = copy.numStepsInPhase; // Not needed?
   // Int. Number of points per mesh
   numPointsPerMesh = copy.numPointsPerMesh;
   //  IRK data utility.
//   ButcherTableData = copy.ButcherTableData;   // TBD **********
   // Int.  Number of stages in the IRK method
   numStages = copy.numStages;
   // Int. Number of steps in the phase
   numStepsInPhase = copy.numStepsInPhase;
   // Bool. Indicates constraint matrices A and B are initialized
   isConMatInitialized = copy.isConMatInitialized;
   // The number of ode RHS terms.  numStates*numPoints
   numODERHS = copy.numODERHS;
   // Real array.  Vector of non-dimensional step sizes
   if (stepSizeVec.IsSized() == true)
      stepSizeVec = copy.stepSizeVec;

   pValue = copy.pValue;
   maxAddNodeNumPerIntv = copy.maxAddNodeNumPerIntv;
};


//------------------------------------------------------------------------------
// operator=(const NLPFuncUtil_ImplicitRK &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtil_ImplicitRK& NLPFuncUtil_ImplicitRK::operator=(
                                            const NLPFuncUtil_ImplicitRK &copy)
{
   if (this == &copy)
      return *this;
   
   NLPFuncUtil_Coll::operator=(copy);

   //  number of mesh steps for each mesh interval
   numStepsInPhase = copy.numStepsInPhase; // Not needed?
                                     // Int. Number of points per mesh
   numPointsPerMesh = copy.numPointsPerMesh;
   //  IRK data utility.
   if (butcherTableData)
      delete butcherTableData;
   butcherTableData = (copy.butcherTableData)->Clone();
   // Number of stages in the IRK method
   numStages = copy.numStages;
   // Number of steps in the phase
   numStepsInPhase = copy.numStepsInPhase;
   // Indicates constraint matrices A and B are initialized
   isConMatInitialized = copy.isConMatInitialized;
   // The number of ode RHS terms.  numStates*numPoints
   numODERHS = copy.numODERHS;
   // Vector of non-dimensional step sizes
   if (stepSizeVec.IsSized() == true)
      stepSizeVec = copy.stepSizeVec;


   pValue = copy.pValue;
   maxAddNodeNumPerIntv = copy.maxAddNodeNumPerIntv;

   return *this;
};

//------------------------------------------------------------------------------
// ~NLPFuncUtil_ImplicitRK()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
NLPFuncUtil_ImplicitRK::~NLPFuncUtil_ImplicitRK()
{
   // do nothing
   if (butcherTableData)
      delete butcherTableData;
};

//------------------------------------------------------------------------------
// void InitializeTranscription()
//------------------------------------------------------------------------------
/**
 * Computes Radau points, weights, and differentiation matrix
 *
 * @todo  add (string) collocationMethod
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InitializeTranscription()
{
   // Intialize the Butcher table helper class used throughout
   //SetButcherTable(); // TBD: add collocationMethod //moved to constructor

   //  Set stage properties, step size vector and bookeeping stuff
   SetStageProperties();
   Rvector meshFractions = ptrConfig->GetMeshIntervalFractions();
   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   ComputeStepSizeVector(&meshFractions, &meshNumPoints, 
                        discretizationPoints, stepSizeVec);



   Integer totalNumMeshPoints = 0;
   for (UnsignedInt idx = 0; idx < meshNumPoints.size(); idx++)
      totalNumMeshPoints += (meshNumPoints)[idx];

   Integer numMeshIntervals = meshNumPoints.size();
   Rvector stageTimes = butcherTableData->GetStageTimes();
   numStages = stageTimes.GetSize() - 1;
   numStepsInPhase = totalNumMeshPoints - meshNumPoints.size();
   numMeshPoints = numStepsInPhase*numPointsPerMesh + 1;
   numStatePoints = numStepsInPhase + 1; 
   numControlPoints = numStepsInPhase + 1; 
   numPathConstraintPoints = numMeshPoints;
   numODERHS = discretizationPoints.GetSize()*ptrConfig->GetNumStateVars();

   timeVectorType.resize(numMeshPoints);
   for (Integer idx = 0; idx < numMeshPoints; idx++)
      timeVectorType[idx] = 1;

   // Compute number of various types of parameters
   Integer numTimesNLP = 2;
   Integer numStatesNLP = ptrConfig->GetNumStateVars()*(numMeshPoints);
   Integer numControlsNLP = ptrConfig->GetNumControlVars()*(numMeshPoints);
   // YK mod static vars
   Integer numDecisionVarsNLP = numStatesNLP + numControlsNLP 
                                + numTimesNLP + ptrConfig->GetNumStaticVars();
   Integer numDefectConNLP = ptrConfig->GetNumStateVars()*(numMeshPoints - 1);

   // Set data on the configuration ect
   ptrConfig->SetNumStateVarsNLP(numStatesNLP);
   ptrConfig->SetNumControlVarsNLP(numControlsNLP);
   ptrConfig->SetNumDecisionVarsNLP(numDecisionVarsNLP);
   ptrConfig->SetNumDefectConNLP(numDefectConNLP);
      
   // the order of method
   pValue = (butcherTableData->GetNumPointsPerStep() - 1);

   // mesh-refinement related constants. 
   maxAddNodeNumPerIntv = 15;
   relErrorTol = 1e-5;
   maxTotalNodeNumPerIntv = 20;
};

//------------------------------------------------------------------------------
// void InitNLPHelpers()
//------------------------------------------------------------------------------
/**
 * Initializes NLP helper utilities for cost and defect calculations
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InitNLPHelpers()
{
   //  Initialize NLP helper utilities for cost and defect calculations

   // Configure the defect NLP data utility
   DefectNLPData = NLPFunctionData();
   DefectNLPData.Initialize(ptrConfig->GetNumDefectConNLP(), 
                           ptrConfig->GetNumDecisionVarsNLP(), numODERHS);

   // Configure the cost NLP data utility
   CostNLPData = NLPFunctionData();
   Integer numCostFunctions = 1;
   CostNLPData.Initialize(numCostFunctions,
                           ptrConfig->GetNumDecisionVarsNLP(), numODERHS);
};

//------------------------------------------------------------------------------
// void SetTimeVector(Real initialTime, Real finalTime)
//------------------------------------------------------------------------------
/**
 * Computes Radau points, weights, and differentiation matrix
 *
 * @param <initialTime>   the initial time
 * @param <finalTime>     the final time
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::SetTimeVector(Real initialTime, Real finalTime)
{
   //  Computes the vector of discretization times
   deltaTime = finalTime - initialTime;
   timeVector.SetSize(discretizationPoints.GetSize());
   for (Integer idx = 0; idx < timeVector.GetSize(); idx++)
      timeVector(idx) = discretizationPoints(idx)*deltaTime + initialTime;

   numTimePoints = numMeshPoints;
};

//------------------------------------------------------------------------------
// Integer  GetMeshIndex(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the mesh index given the input point index
 *
 * @param <pointIdx>   the point index
 *
 * @return the mesh index for the specified point
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_ImplicitRK::GetMeshIndex(Integer pointIdx)
{        
   Real idxByNumPointsPlusOne = (pointIdx + 1) / (numPointsPerMesh + 1);
   if (idxByNumPointsPlusOne < 1)
      return 0;
         
         
   Real quotient = (pointIdx + 1) / (numPointsPerMesh);
   if (remainder((pointIdx + 1), numPointsPerMesh) == 0)// end point in the mesh
      return quotient - 1;
   else
      return floor(quotient); // An internal(stage point)

};

//------------------------------------------------------------------------------
// Integer  GetStageIndex(Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Returns the stage index given the input point index
 *
 * @param <pointIdx>   the point index
 *
 * @return the stage index for the specified point
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_ImplicitRK::GetStageIndex(Integer pointIdx)
{
   if (pointIdx < numPointsPerMesh)
      return pointIdx;

   Real quotient = floor((pointIdx + 1) / numPointsPerMesh);

   if (remainder((pointIdx + 1), numPointsPerMesh) == 0)
      return numStagePoints;
   else
      return pointIdx - quotient*numPointsPerMesh;
};

//------------------------------------------------------------------------------
// Real  GetdCurrentTimedTI(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * Returns partial of time at point k with respect to initial time
 *
 * @param <meshIdx>    the mesh index
 * @param <stageIdx>   the stage index
 *
 * @return partial of time at point k with respect to initial time
 */
//------------------------------------------------------------------------------
Real NLPFuncUtil_ImplicitRK::GetdCurrentTimedTI(Integer meshIdx,
                                                Integer stageIdx)
{
   Real nonDimTime = MeshPointIdxToNonDimTime(meshIdx, stageIdx);
   return 1 - nonDimTime;
};

//------------------------------------------------------------------------------
// Real  GetdCurrentTimedTF(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * Returns partial of time at point k with respect to initial time
 *
 * @param <meshIdx>    the mesh index
 * @param <stageIdx>   the stage index
 *
 * @return partial of time at point k with respect to initial time
 */
//------------------------------------------------------------------------------
Real NLPFuncUtil_ImplicitRK::GetdCurrentTimedTF(Integer meshIdx,
                                                Integer stageIdx)
{
   return  MeshPointIdxToNonDimTime(meshIdx, stageIdx);
};

//------------------------------------------------------------------------------
// Real  MeshPointIdxToNonDimTime(Integer meshIdx, Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * Computes the scaled time value at a given mesh point index
 *
 * @param <meshIdx>    the mesh index
 * @param <stageIdx>   the stage index
 *
 * @return the scaled time value at a given mesh point index
 */
//------------------------------------------------------------------------------
Real NLPFuncUtil_ImplicitRK::MeshPointIdxToNonDimTime(Integer meshIdx,
                                                      Integer stageIdx)
{
   Integer pointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, stageIdx);
   return discretizationPoints(pointIdx);
};

//------------------------------------------------------------------------------
// Integer  GetPointIdxGivenMeshAndStageIdx(Integer meshIdx,
//                                          Integer stageIdx)
//------------------------------------------------------------------------------
/**
 * Returns the point index given the mesh and stage index
 *
 * @param <meshIdx>    the mesh index
 * @param <stageIdx>   the stage index
 *
 * @return the point index for the given mesh and stage indexes
 */
//------------------------------------------------------------------------------
Integer NLPFuncUtil_ImplicitRK::GetPointIdxGivenMeshAndStageIdx(
                                Integer meshIdx,
                                Integer stageIdx)
{
   return ((numStages)*(meshIdx) + stageIdx);
};

//------------------------------------------------------------------------------
// void SetStageProperties()
//------------------------------------------------------------------------------
/**
 * Sets data on state and control at stage points
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::SetStageProperties()
{
   numStateStagePointsPerMesh   =
                            butcherTableData->GetNumStateStagePointsPerMesh();
   numControlStagePointsPerMesh =
                            butcherTableData->GetNumControlStagePointsPerMesh();
   numStagePoints               = butcherTableData->GetNumStagePointsPerMesh();
   numPointsPerMesh             = 1 + numStagePoints;
};

//------------------------------------------------------------------------------
// void ComputeStepSizeVector()
//------------------------------------------------------------------------------
/**
 * Computes the step size vector
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::ComputeStepSizeVector(Rvector *ptrMeshFractions, 
                                                   IntegerArray *meshNumPoints, 
                                                   Rvector &discretizationPts,
                                                   Rvector &stepSizeVec)
{
   // Note this is generalized for later support of non-constant
   // step sizes.

   Rvector meshFractions = NormalizeMeshFractions(ptrMeshFractions);
   ptrConfig->SetMeshIntervalFractions(meshFractions);
   
   Integer totalNumMeshPoints = 0;
   for (UnsignedInt idx = 0; idx < meshNumPoints->size(); idx++)
      totalNumMeshPoints += (*meshNumPoints)[idx];

   Integer numMeshIntervals = meshNumPoints->size();
   Rvector stageTimes = butcherTableData->GetStageTimes();
   Integer numStages = stageTimes.GetSize() - 1;  
   Integer numStepsInPhase = totalNumMeshPoints - meshNumPoints->size();
   Integer numMeshPoints = numStepsInPhase*numPointsPerMesh + 1;
   discretizationPts.SetSize(numMeshPoints);
   discretizationPts(0) = 0;

   Integer pointIdx = 0;

   stepSizeVec.SetSize(numMeshIntervals);
   for (Integer intervalIdx = 0; intervalIdx < numMeshIntervals; intervalIdx++)
   {
      
      Real meshIntervalDuration = meshFractions(intervalIdx + 1) -
                                  meshFractions(intervalIdx);
      #ifdef DEBUG
         MessageInterface::ShowMessage(
                           "meshIntervalDuration is given as follows:\n%le",
                           meshIntervalDuration);
      #endif
      Real stepSize = meshIntervalDuration / ((*meshNumPoints)[intervalIdx] - 1.0);
      stepSizeVec(intervalIdx) = stepSize;

      for (Integer stepIdx = 0; stepIdx < (*meshNumPoints)[intervalIdx] - 1;
           stepIdx++)
      {
         for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
         {
            pointIdx = pointIdx + 1;
            discretizationPts(pointIdx) =  discretizationPts(pointIdx - 1) 
               +  stepSize*(stageTimes(stageIdx + 1) - stageTimes(stageIdx));
            
            #ifdef DEBUG
               Real deltaT = stepSize*(stageTimes(stageIdx + 1) -
                             stageTimes(stageIdx));
               MessageInterface::ShowMessage(
                             "delta times are given as follows:\n%le",
                             deltaT);
            #endif
         }
      }
   }
   // to remove numerical noise, set the final point directly.
   discretizationPts(numMeshPoints - 1) = 1;

   #ifdef DEBUG
      MessageInterface::ShowMessage(
                        "discretizationPts are given as follows:\n%s\n",
                        discretizationPts.ToString(12).c_str());
   #endif
};



//------------------------------------------------------------------------------
// Rvector NormalizeMeshFractions(Rvector meshIntvFractions)
//------------------------------------------------------------------------------
/**
 * Normalizes mesh interval fractions to fall in [0 1]
 *
 * @param <meshIntvFractions> mesh interval fractions
 *
 * @return the mesh interval fractions normalized to [0 1]
 *
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_ImplicitRK::NormalizeMeshFractions(
                                                      Rvector *meshIntvFractions)
{
   Integer numPoints = meshIntvFractions->GetSize();
   if ((*meshIntvFractions)(0) < 0.0 ||
      (*meshIntvFractions)(numPoints - 1) > 1.0)
      MessageInterface::ShowMessage("**** WARNING **** "
         "NLPFuncUtil_ImplicitRK::NormalizeMeshFractions detected mesh "
         "fractions less than zero and/or greater than one, fractions will "
         "be normalized to the range [0 1]\n");

   Real meshSpan = (*meshIntvFractions)(numPoints - 1) -
                   (*meshIntvFractions)(0);
   Rvector newNeshIntervalFractions(numPoints);
   for (Integer meshIdx = 1; meshIdx < numPoints; meshIdx++)
   {
      newNeshIntervalFractions(meshIdx) = ((*meshIntvFractions)(meshIdx) - 
                                           (*meshIntvFractions)(0)) / meshSpan;
   }
   newNeshIntervalFractions(numPoints - 1) = 1;
   return newNeshIntervalFractions;
};

//------------------------------------------------------------------------------
// void SetButcherTable(std::string collocationMethod)
//------------------------------------------------------------------------------
/**
 * Initializes Butcher table helper class for chosen method
 *
 * @param <collocationMethod> the specified collocation method
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::SetButcherTable(std::string collocationMethod)
{
   // delete the butcher table if it already exists
   if (butcherTableData)
      delete butcherTableData;
   
   // Initializes Butcher table helper class for chosen method
   
   if (collocationMethod == "RungeKutta8")
   {
      butcherTableData   = new LobattoIIIA_8Order();
   }
   else if (collocationMethod == "RungeKutta6")
   {
      butcherTableData   = new LobattoIIIA_6Order();
   }
   else if (collocationMethod == "RungeKutta4")
   {
      butcherTableData   = new LobattoIIIA_4Order();
   }
   else if (collocationMethod == "HermiteSimpson")
   {
      // TBD
      butcherTableData   = new LobattoIIIA_4HSOrder();
   }
   else if (collocationMethod == "Trapezoid")
   {
      // TBD
      butcherTableData   = new LobattoIIIA_2Order();
   }
   else
   {
      std::stringstream errmsg;
      errmsg << "NLPFuncUtil_ImplicitRK::SetButcherTable,";
      errmsg << "Unknown collocation method. Please set a proper collocation ";
      errmsg << "method before optimization.";
      throw LowThrustException(errmsg.str());
   }
   
   quadratureWeights  = butcherTableData->GetQuadratureWeights();
};

//------------------------------------------------------------------------------
// void InitStateAndControlInterpolator()
//------------------------------------------------------------------------------
/**
 * Do nothing here. This is for Radau collocation
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InitStateAndControlInterpolator()
{
   // do nothing
};

//------------------------------------------------------------------------------
// void InitializeConstantDefectMatrices(
//                      UserFunctionProperties                 *dynFunProps,
//                      const std::vector<FunctionOutputData*> &dynFunVector)
//------------------------------------------------------------------------------
/**
 * Populates the A and B matrices
 *
 * @param <dynFunProps>  dynamic function properties
 * @param <dynFunVector> function ouptut data vector
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InitializeConstantDefectMatrices(
                      UserFunctionProperties                 *dynFunProps,
                      const std::vector<FunctionOutputData*> &dynFunVector)
{
   //  If nothing to do return
   if (!ptrConfig->HasDefectCons() && !isConMatInitialized)
      return;

   //// Populate the A and B matrices
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer defectStartIdx = 0, pointIdx;
   //defectEndIdx = numStateVars;
   // Loop over the steps in the phase
   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   Integer stepIdx = 0;
   //  Loop over the function data and concatenate Q and parQ matrix terms
   for (UnsignedInt intervalIdx = 0; intervalIdx < meshNumPoints.size();
        intervalIdx++)
   {
      for (Integer meshIdx = 0; meshIdx < meshNumPoints[intervalIdx] - 1;
           meshIdx++)
      {
         //  Loop over defect constraints for current step
         for (Integer defectIdx = 0; defectIdx < numStages; defectIdx++)
         {
            // Loop over points used in current step
            for (Integer subStepIdx = 0; subStepIdx < numStages + 1;
                 subStepIdx++)
            {
               if (subStepIdx < numStages)
                  pointIdx = GetPointIdxGivenMeshAndStageIdx(stepIdx,
                                                             subStepIdx);
               else
                  pointIdx = GetPointIdxGivenMeshAndStageIdx(stepIdx + 1, 0);

               Rmatrix aChunk, bChunk, negBChunk;
               butcherTableData->GetDependencyChunk(defectIdx, subStepIdx,
                                 numStateVars, aChunk, bChunk);
               IntegerArray stateIdxs =
                            dynFunVector.at(pointIdx)->GetStateIdxs();
               DefectNLPData.InsertAMatPartition(defectStartIdx, stateIdxs[0],
                                                 &aChunk);
               Integer odeStartIdx = pointIdx*numStateVars;
            
               negBChunk = (-bChunk)*stepSizeVec(intervalIdx);
               DefectNLPData.InsertBMatPartition(defectStartIdx, odeStartIdx,
                                                 &negBChunk);
            }
            defectStartIdx = defectStartIdx + numStateVars;            
         }
         stepIdx += 1;
     
      }
   }
   
   #ifdef DEBUG_Matrices
      MessageInterface::ShowMessage("AMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(DefectNLPData.GetAMatrixPtr());
      MessageInterface::ShowMessage("BMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(DefectNLPData.GetBMatrixPtr());
   #endif

   //  Fill the D matrices
   RSMatrix conDMatRD;
      
   SparseMatrixUtil::SetSize(conDMatRD, numODERHS,
                             ptrConfig->GetNumDecisionVarsNLP());
   for (UnsignedInt funcIdx = 0; funcIdx < dynFunVector.size(); funcIdx++)
   {
      //  Increment counters
      Integer funIdxStart = (funcIdx)*ptrConfig->GetNumStateVars();
      //  Fill in state Jacobian elements
      if (ptrConfig->HasStateVars())
      {
         IntegerArray stateIdx = dynFunVector.at(funcIdx)->GetStateIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(conDMatRD, funIdxStart,
                           stateIdx[0],
                           &(dynFunProps->GetStateJacobianPattern()));
         #ifdef DEBUG_Matrices
            MessageInterface::ShowMessage(
               "StateJacobianPattern is given as follows:\n%s\n",
               (dynFunProps->GetStateJacobianPattern()).ToString(12).c_str());
         #endif
      }
      //  Fill in control Jacobian terms
      if (ptrConfig->HasControlVars())
      {
         IntegerArray controlIdx = dynFunVector.at(funcIdx)->GetControlIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(conDMatRD, funIdxStart,
                           controlIdx[0],
                           &(dynFunProps->GetControlJacobianPattern()));

         #ifdef DEBUG_Matrices
            MessageInterface::ShowMessage(
               "ControlJacobianPattern is given as follows:\n%s\n",
               (dynFunProps->GetControlJacobianPattern()).ToString(12).c_str());
         #endif
      }

      //  Fill in static Jacobian terms
      if (ptrConfig->HasStaticVars() == true)
      {
         IntegerArray staticIdxs = dynFunVector.at(0)->GetStaticIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(conDMatRD, funIdxStart,
                     staticIdxs[0], &(dynFunProps->GetStaticJacobianPattern()));
      }

      for (Integer idx = 0; idx < ptrConfig->GetNumStateVars(); idx++)
      { 
         //  Initial time terms
         conDMatRD(funIdxStart + idx, 0) = 1.0;
         //  Final time terms
         conDMatRD(funIdxStart + idx, 1) = 1.0;
      }
   }
   Rmatrix conDMatRD_rmatrix = SparseMatrixUtil::RSMatrixToRmatrix(&conDMatRD);
   DefectNLPData.InsertDMatPartition(0,0, &conDMatRD_rmatrix);

   #ifdef DEBUG_Matrices
      MessageInterface::ShowMessage("DMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(DefectNLPData.GetDMatrixPtr());
   #endif
   //  Set flag indiciating the matrices are initialized
   isConMatInitialized = true;
};

//------------------------------------------------------------------------------
// void FillDynamicDefectConMatrices(
//                             const std::vector<FunctionOutputData*> &funcData,
//                             Rvector                                &valueVec,
//                             RSMatrix                               &jacobian)
//------------------------------------------------------------------------------
/**
 * Populates the dynamic constraint matrices
 *
 * @param <funcData>     function ouptut data vector
 * @param <valueVec>     [output] vector of values
 * @param <dynFunVector> [output] jacobian matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::FillDynamicDefectConMatrices(
                             const std::vector<FunctionOutputData*> &funcData,
                             Rvector                                &valueVec,
                             RSMatrix                               &jacobian)
{

   //  Loop initializations           
   // set the sparsity pattern of the parQMatrix before setting values
   // to save time.
   const RSMatrix* ptrDMatrix = DefectNLPData.GetDMatrixPtr();
   bool hasZeros = true;
   jacobian = SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   Integer numValues = SparseMatrixUtil::GetNumRows(ptrDMatrix);
   valueVec.SetSize(numValues);
   
   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   //  Loop over the function data and concatenate Q and parQ matrix terms
   for (UnsignedInt funcIdx = 0; funcIdx < funcData.size(); funcIdx++)
   {
      // Increment counters
      Integer conStartIdx = (funcIdx)*ptrConfig->GetNumStateVars();
      Integer meshIdx = funcData.at(funcIdx)->GetMeshIdx();
      Integer stageIdx = funcData.at(funcIdx)->GetStageIdx();
      Integer rowStart = conStartIdx;

      // Compute dimensionalized time step multiplier for this
      // function evaluation
      Real timeStep = deltaTime;

      // Fill in the function data
      Rvector tmpVec = -timeStep*funcData.at(funcIdx)->GetFunctionValues();
      for (Integer idx = 0; idx < tmpVec.GetSize(); idx++)
      { 
         // keep adding things 
         valueVec(conStartIdx + idx) = tmpVec(idx);
      }
      // Fill in state Jacobian elements
      if (ptrConfig->HasStateVars())
      {
         Rmatrix denseMat = -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::STATE);
         IntegerArray stateIdxs = funcData.at(funcIdx)->GetStateIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
                                                stateIdxs[0], &denseMat);
      }

      // Fill in control Jacobian terms
      if (ptrConfig->HasControlVars())
      {
         Rmatrix denseMat =
                          -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::CONTROL);
         IntegerArray controlIdxs = funcData.at(funcIdx)->GetControlIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
                                                controlIdxs[0], &denseMat);
      }

      if (ptrConfig->HasStaticVars())
      {
         Rmatrix denseMat = -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::STATIC);
         IntegerArray staticIdxs = funcData.at(funcIdx)->GetStaticIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
            staticIdxs[0], &denseMat);
      }


      // Initial time terms

      /// type cast Rvector to Rmatrix 
      tmpVec = funcData.at(funcIdx)->GetFunctionValues();
      Rmatrix tmpMat(tmpVec.GetSize(), 1);
      for (Integer idx = 0; idx < tmpVec.GetSize(); idx++)
         tmpMat(idx, 0) = tmpVec(idx);

      Rmatrix denseMat = tmpMat -
                         timeStep*GetdCurrentTimedTI(meshIdx,
                         stageIdx)*funcData.at(funcIdx)->GetJacobian(UserFunction::TIME);
      SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart, 0, &denseMat);

      // Final time terms
      denseMat = -tmpMat - timeStep*GetdCurrentTimedTF(meshIdx, stageIdx) *
                 funcData.at(funcIdx)->GetJacobian(UserFunction::TIME);
      SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart, 1, &denseMat);
 
   }
   #ifdef DEBUG_Matrices
      MessageInterface::ShowMessage(
                        "function vector is given as follows:\n%s\n",
                        valueVec.ToString(12).c_str());
      MessageInterface::ShowMessage("    getting size of jacobian\n");
      Integer r = SparseMatrixUtil::GetNumRows(&jacobian);
      Integer c = SparseMatrixUtil::GetNumColumns(&jacobian);
      MessageInterface::ShowMessage(" jacobian size = (%d, %d)\n", r, c);
      MessageInterface::ShowMessage("parQMatrix is given as follows:\n");
//      SparseMatrixUtil::PrintNonZeroElements(&jacobian); // this line causes a
                                                           // bad_alloc error
   #endif
};


//---------------------------------------------------------------------------
// void InitializeConstantCostMatrices(UserFunctionProperties *costFuncProps,
//                            std::vector<FunctionOutputData> *costFuncValues)
//---------------------------------------------------------------------------
/**
 * Fills in the A, B, and D matrices for cost function during initialization
 *
 * @param <costFuncProps>   the user function properties of cost
 * @param <costFuncValues>  the cost function values
 *
 */
//---------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InitializeConstantCostMatrices(
                     UserFunctionProperties                 *costFuncProps,
                     const std::vector<FunctionOutputData*> &costFuncValues)
{

   if ((ptrConfig->HasIntegralCost() == false)
      && (isConMatInitialized == false))
      return;

   //  For cost function, there is only one function!
   //Integer meshIdx;
   Real tmpValue;
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

   Integer numInterations = costFuncValues.size();

   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   Integer funcIdx = 0;
   Rmatrix tmpMat(1, numPathConstraintPoints);   
   //  Loop over the function data and concatenate Q and parQ matrix terms
   for (UnsignedInt intervalIdx = 0; intervalIdx < meshNumPoints.size();
        intervalIdx++)
   {
      // loop over the steps in the current interval
      for (Integer stepIdx = 0; stepIdx < meshNumPoints[intervalIdx] - 1;
           stepIdx++)
      {
         //  Loop over substeps (stage points) for current step
         for (Integer subStepIdx = 0; subStepIdx < numStages + 1; subStepIdx++)
         {
            tmpMat(0, funcIdx) +=  quadratureWeights(subStepIdx) *
                                   (-stepSizeVec(intervalIdx));
            if (subStepIdx == numStages)
            {
               // do nothing 
            }
            else
            {
               funcIdx += 1;
            }
            
         }
      }
   }
   CostNLPData.InsertBMatPartition(0, 0, &(tmpMat));
   #ifdef DEBUG_Matrices
      MessageInterface::ShowMessage("B Matrix is given as follows:\n%s\n",
                                    tmpMat.ToString(12).c_str());
   #endif

   //  Loop over the number of points
   for (Integer funcIdx = 0; funcIdx < numInterations; ++funcIdx)
   {
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
            tmpValue = stateJacPattern(0, colIdx);
            if (tmpValue != 0)
               CostNLPData.InsertDMatElement(funcIdx,
                  stateIdxs[colIdx], tmpValue);
         }
      }

      //  Fill in control Jacobian terms
      if (ptrConfig->HasControlVars() == true)
      {
         controlIdxs = costFuncValues.at(funcIdx)->GetControlIdxs();
         for (Integer colIdx = 0; colIdx < ptrConfig->GetNumControlVars();
            ++colIdx)
         {
            tmpValue = controlJacPattern(0, colIdx);
            if (tmpValue != 0)
               CostNLPData.InsertDMatElement(funcIdx,
                  controlIdxs[colIdx], tmpValue);
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
// void FillDynamicCostFuncMatrices(
//                             const std::vector<FunctionOutputData*> &funcData,
//                             Rvector                                &valueVec,
//                             RSMatrix                               &jacobian)
//------------------------------------------------------------------------------
/**
 * Populates the dynamic cost function matrices
 *
 * @param <funcData>     function ouptut data vector
 * @param <valueVec>     [output] vector of values
 * @param <dynFunVector> [output] jacobian matrix
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::FillDynamicCostFuncMatrices(
                  const std::vector<FunctionOutputData*> &funcData,
                  Rvector                                &valueVec,
                  RSMatrix                               &jacobian)
{      

   //  Loop initializations           
   // set the sparsity pattern of the parQMatrix before setting values
   // to save time.
   const RSMatrix* ptrDMatrix = CostNLPData.GetDMatrixPtr();
   bool hasZeros = true;
   jacobian = SparseMatrixUtil::GetSparsityPattern(ptrDMatrix, hasZeros);

   Integer numValues = SparseMatrixUtil::GetNumRows(ptrDMatrix);
   valueVec.SetSize(numValues);

   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();


   //  Loop over the function data and concatenate Q and parQ matrix terms
   for (UnsignedInt funcIdx = 0; funcIdx < funcData.size(); funcIdx++)
   {
      // Increment counters
      Integer conStartIdx = (funcIdx)*1;
      Integer meshIdx = funcData.at(funcIdx)->GetMeshIdx();
      Integer stageIdx = funcData.at(funcIdx)->GetStageIdx();
      Integer rowStart = conStartIdx;

      // Compute dimensionalized time step multiplier for this
      // function evaluation
      Real timeStep = deltaTime;

      // Fill in the function data
      Rvector tmpVec = -timeStep*funcData.at(funcIdx)->GetFunctionValues();
      valueVec(conStartIdx) = tmpVec(0);

      // Fill in state Jacobian elements
      if (ptrConfig->HasStateVars())
      {
         Rmatrix denseMat = -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::STATE);
         IntegerArray stateIdxs = funcData.at(funcIdx)->GetStateIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
                                                stateIdxs[0], &denseMat);
      }

      // Fill in control Jacobian terms
      if (ptrConfig->HasControlVars())
      {
         Rmatrix denseMat =
                          -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::CONTROL);
         IntegerArray controlIdxs = funcData.at(funcIdx)->GetControlIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
                                                controlIdxs[0], &denseMat);
      }

      // Fill in state Jacobian elements
      if (ptrConfig->HasStaticVars())
      {
         Rmatrix denseMat = -timeStep*funcData.at(funcIdx)->GetJacobian(UserFunction::STATIC);
         IntegerArray staticIdxs = funcData.at(funcIdx)->GetStaticIdxs();
         SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart,
            staticIdxs[0], &denseMat);
      }

      // Initial time terms

      /// type cast Rvector to Rmatrix 
      tmpVec = funcData.at(funcIdx)->GetFunctionValues();
      Rmatrix tmpMat(tmpVec.GetSize(), 1);
      for (Integer idx = 0; idx < tmpVec.GetSize(); idx++)
         tmpMat(idx, 0) = tmpVec(idx);

      Rmatrix denseMat = tmpMat -
                         timeStep*GetdCurrentTimedTI(meshIdx, stageIdx) *
                         funcData.at(funcIdx)->GetJacobian(UserFunction::TIME);
      SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart, 0, &denseMat);

      // Final time terms
      denseMat = -tmpMat -
                  timeStep*GetdCurrentTimedTF(meshIdx, stageIdx) *
                  funcData.at(funcIdx)->GetJacobian(UserFunction::TIME);
      SparseMatrixUtil::SetSparseBLockMatrix(jacobian, rowStart, 1, &denseMat);

   }
   #ifdef DEBUG_Matrices
      MessageInterface::ShowMessage(
                        "function vector is given as follows:\n%s\n",
                        valueVec.ToString(12).c_str());
      MessageInterface::ShowMessage("parQMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(&jacobian);
   #endif
};

//------------------------------------------------------------------------------
// void RefineMesh(DecVecTypeBetts         *ptrDecVector,
//                UserPathFunctionManager *ptrFunctionManager,
//                Rvector                 *oldRelErrorArray,
//                bool                    &isMeshRefined,
//                IntegerArray            &newMeshIntervalNumPoints,
//                Rvector                 &newMeshIntervalFractions,
//                Rvector                 &maxRelErrorArray,
//                Rmatrix                 &newStateGuess,
//                Rmatrix                 &newControlGuess)
//------------------------------------------------------------------------------
/**
* Compute new mesh points based on estimated transcription errors.
*
* @param <ptrDecVector>               input: the pointer of the current
*                                     decision vector
* @param <ptrFunctionManager>         input: the pointer of the current
*                                     path function manager
* @param <oldRelErrorArray>           input: the pointer of the old error vector
* @param <isMeshRefined>              output: indicating whether mesh has been
*                                     changed
* @param <newMeshIntervalNumPoints>   output: new mesh points
* @param <newMeshIntervalFractions>   output: new mesh intervals
* @param <maxRelErrorInMesh>          output: the current maximum relative
*                                     error
* @param <newStateGuess>              output: state guess for next mesh points
* @param <newControlGuess>            output: control guess for next mesh points
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::RefineMesh(
                              DecVecTypeBetts         *ptrDecVector,
                              UserPathFunctionManager *ptrFunctionManager,
                              Rvector                 *oldRelErrorArray,
                              bool                    &isMeshRefined,
                              IntegerArray            &newMeshIntervalNumPoints,
                              Rvector                 &newMeshIntervalFractions,
                              Rvector                 &maxRelErrorArray,
                              Rmatrix                 &newStateGuess,
                              Rmatrix                 &newControlGuess)
{
   PrepareToRefineMesh(ptrFunctionManager);
   Integer numMeshIntervals = ptrConfig->GetNumMeshIntervals();

   Rvector newCollocErrorVec, weightVec;
   std::vector<std::vector<Real> > newCollocErrorArray;
   std::vector<Rvector> weightArray;
   std::vector<Real> maxCollocErrorPerIntv;

   GetCollocErrorVec(ptrDecVector, newCollocErrorVec, newCollocErrorArray,
                     maxCollocErrorPerIntv);

   // NOTE: Now order reduction is not activated due to interface issue.
   //       We need to deliver the previous mesh iteration information
   //       (e.g., addNodeNumVecArray, oldMeshNumPoints, oldMeshFractions,
   //       and oldRelErrorArray) to the current NLP collocation utility.
   //       YK. 2017.08.17

   // mute order reduction
   IntegerArray orderReductionVec;
   if (true) //if (oldRelErrorArray->IsSized() == false)   
   {

      orderReductionVec.resize(numMeshIntervals);
      for (Integer idx = 0; idx < numMeshIntervals; idx++)
         orderReductionVec[idx] = 0;
   }
   else
   {
      // mute order reduction
      //orderReductionVec = GetOrderReduction(&oldMeshFractions, &oldMeshNumPoints, 
      //                              oldRelErrorArray, 
      //                              &addNodeNumVecArray, &maxCollocErrorPerIntv);
   }

   Rvector newMeshPoints;
   std::vector<Integer> newNumMeshIntv;
   addNodeNumVecArray.resize(numMeshIntervals);
   // update mesh points, error vector, add node number vector
   for (Integer idx = 0; idx < numMeshIntervals; idx++)
   {
      Integer newNumIntv;
      IntegerArray addNodeNum;
      GetNewMeshPoints(ptrDecVector, orderReductionVec[idx], &newCollocErrorVec,
         idx, newNumIntv, addNodeNum);
      newNumMeshIntv.push_back(newNumIntv);
      addNodeNumVecArray[idx] = addNodeNum;
   }

   //// Create New Mesh Points
   Integer totAddNumNode = 0;
   for (Integer idx = 0; idx < numMeshIntervals; idx++)
   {
      for (Integer subIdx = 0; subIdx < addNodeNumVecArray[idx].size(); subIdx++)
         totAddNumNode += addNodeNumVecArray[idx][subIdx];
   }
   if (totAddNumNode == 0)
   {
      // there is no mesh points to add
      isMeshRefined = false;

      Rmatrix tmpState = ptrDecVector->GetStateArray();
      Rmatrix tmpControl = ptrDecVector->GetControlArray();

      newStateGuess.SetSize(tmpState.GetNumRows(), tmpState.GetNumColumns());
      newControlGuess.SetSize(tmpControl.GetNumRows(), tmpControl.GetNumColumns());
      newStateGuess = tmpState;
      newControlGuess = tmpControl;
      return;
   }

   std::vector<Real> stdNewMeshFractions;
   newMeshIntervalNumPoints.clear();
   Rvector meshIntvFractions = ptrConfig->GetMeshIntervalFractions();
   IntegerArray meshIntervalNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   for (Integer idx = 0; idx < numMeshIntervals; idx++)
   {
      // split mesh interval into multiple intervals.

      Real stepSize = (meshIntvFractions(idx + 1) -
         meshIntvFractions(idx)) / double(newNumMeshIntv[idx]);
      Real currPoint = meshIntvFractions(idx);
      Integer numNodes = meshIntervalNumPoints[idx] / newNumMeshIntv[idx];

      for (Integer idx2 = 0; idx2 < newNumMeshIntv[idx]; idx2++)
      {
         Real tmp = addNodeNumVecArray[idx][idx2] +
            meshIntervalNumPoints[idx] / newNumMeshIntv[idx];

         // there must be at least three points per interval for mesh refinement. 
         if (tmp < 3)
            numNodes = 3;
         else
            numNodes = tmp;

         stdNewMeshFractions.push_back(currPoint);
         newMeshIntervalNumPoints.push_back(numNodes);
         currPoint += stepSize;
      }
   }
   // take care of the last point
   stdNewMeshFractions.push_back(meshIntvFractions(numMeshIntervals));

   Integer numNewMeshIntervals = stdNewMeshFractions.size();
   newMeshIntervalFractions.SetSize(numNewMeshIntervals);
   for (Integer idx = 0; idx < numNewMeshIntervals; idx++)
   {
      newMeshIntervalFractions(idx) = stdNewMeshFractions[idx];
   }

   Rvector newDiscrtPts, newStepSizeVec;
   ComputeStepSizeVector(&newMeshIntervalFractions, &newMeshIntervalNumPoints,
                         newDiscrtPts, newStepSizeVec);
   Integer numNewDiscrtPts = newDiscrtPts.GetSize();
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer numControlVars = ptrConfig->GetNumControlVars();
   newStateGuess.SetSize(numNewDiscrtPts, numStateVars);
   newControlGuess.SetSize(numNewDiscrtPts, numControlVars);

   // idx counter for new dicretization points
   Integer idxCounter = 0;
   for (Integer meshIdx = 0; meshIdx < numStepsInPhase; meshIdx++)
   {
      Integer stageIdx = 0;
      Real startTime = MeshPointIdxToNonDimTime(meshIdx, stageIdx);
      Real endTime = MeshPointIdxToNonDimTime(meshIdx + 1, stageIdx);

      std::vector<Real> interpPts;
      Integer numPtsInMesh = 0;
      while (newDiscrtPts(idxCounter + numPtsInMesh) < endTime)
      {
         interpPts.push_back(newDiscrtPts(idxCounter + numPtsInMesh));
         numPtsInMesh += 1;
      }
      // take care the final point
      if (endTime == 1.0)
      {
         interpPts.push_back(1.0);
         numPtsInMesh += 1;
      }
      Rmatrix stateArray, controlArray;

      InterpolateInMesh(ptrDecVector,
                        &interpPts, meshIdx,
                        stateArray, controlArray);

      for (Integer idx = 0; idx < numPtsInMesh; idx++)
      {
         for (Integer idx2 = 0; idx2 < numStateVars; idx2++)
         {
            newStateGuess(idxCounter + idx, idx2) = stateArray(idx, idx2);
         }
         for (Integer idx2 = 0; idx2 < numControlVars; idx2++)
         {
            newControlGuess(idxCounter + idx, idx2) = controlArray(idx, idx2);
         }
      }
      idxCounter += numPtsInMesh;
   }


#ifdef DEBUG_MR
   MessageInterface::ShowMessage(
      "discretizationPts are given as follows:\n%s\n",
      discretizationPoints.ToString(12).c_str());
#endif
#ifdef DEBUG_MR
   Rmatrix oldStateArray = ptrDecVector->GetStateArray();
   MessageInterface::ShowMessage(
      "stateArray is given as follows:\n%s\n",
      oldStateArray.ToString(12).c_str());
#endif
#ifdef DEBUG_MR
   Rmatrix oldControlArray = ptrDecVector->GetControlArray();
   MessageInterface::ShowMessage(
      "controlArray is given as follows:\n%s\n",
      oldControlArray.ToString(12).c_str());
#endif

#ifdef DEBUG_MR
   MessageInterface::ShowMessage(
      "New DiscretizationPts are given as follows:\n%s\n",
      newDiscrtPts.ToString(12).c_str());
#endif
#ifdef DEBUG_MR
   MessageInterface::ShowMessage(
      "newStateGuess is given as follows:\n%s\n",
      newStateGuess.ToString(12).c_str());
#endif
#ifdef DEBUG_MR
   MessageInterface::ShowMessage(
      "newControlGuess is given as follows:\n%s\n",
      newControlGuess.ToString(12).c_str());
#endif

   isMeshRefined = true;

};

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
Rvector NLPFuncUtil_ImplicitRK::ComputeMaxMeshError(
                              DecVecTypeBetts *ptrDecVector,
                              UserPathFunctionManager *ptrFunctionManager)
{
   PrepareToRefineMesh(ptrFunctionManager);
   Rvector maxRelErrorsInMesh(ptrConfig->GetNumMeshIntervals());
   Rvector newCollocErrorVec;
   std::vector<std::vector<Real> > newCollocErrorArray;
   std::vector<Rvector> weightArray;
   std::vector<Real> maxCollocErrorPerIntv;
   GetCollocErrorVec(ptrDecVector, newCollocErrorVec, newCollocErrorArray,
      maxCollocErrorPerIntv);

   for (UnsignedInt ii = 0; ii < ptrConfig->GetNumMeshIntervals(); ++ii)
      maxRelErrorsInMesh(ii) = maxCollocErrorPerIntv.at(ii);
   return maxRelErrorsInMesh;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void GetStateAndControlInMesh(
//                                     Integer              meshIdx,
//                                     DecVecTypeBetts      *ptrDecVector,
//                                     std::vector<Rvector> &stateVecRvector,
//                                     std::vector<Rvector> &controlVecRvector,
//                                     bool                 hasFinalState,
//                                     bool                 hasFinalControl)
//------------------------------------------------------------------------------
/**
* Get state and control vector in the given mesh interval in array form.
* In an array of std::vector<Rvector>, the first index is state variable idx,
* and the second index is mesh point idx.
* TBD: needs to be verified for multi-dimensional problems
*
* @param <stateVecRvector>   output: state vector in Rvector form
* @param <controlVecRvector> output: control vector in Rvetor form
* @param <meshIntvIdx>       input : the index of the mesh interval of interest
* @param <ptrDecVector>      input : the pointer of the decision vector of
*                            phase
* @param <hasFinalState>     input : flag for need final state or not
* @param <hasFinalControl>   input : flag for need final control or not
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::GetStateAndControlInMesh(
                                       Integer              meshIdx,
                                       DecVecTypeBetts      *ptrDecVector,
                                       std::vector<Rvector> &stateVecRvector,
                                       std::vector<Rvector> &controlVecRvector,
                                       bool                 hasFinalState,
                                       bool                 hasFinalControl)
{

   //   operate for each mesh interval
   stateVecRvector.clear();
   controlVecRvector.clear();
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer numControlVars = ptrConfig->GetNumControlVars();
   Integer numStateIdxs, numControlIdxs;

   if (hasFinalState == false)
      numStateIdxs = numStages - 1;
   else
      numStateIdxs = numStages;

   if (hasFinalControl == false)
      numControlIdxs = numStages - 1;
   else
      numControlIdxs = numStages;

   Rmatrix stateArray(numStateIdxs, numStateVars);
   Rmatrix controlArray(numControlIdxs, numControlVars);

   // Loop over stage points used in current interval
   for (Integer stageIdx = 0; stageIdx < numStateIdxs; stageIdx++)
   {
      Rvector stateVec = ptrDecVector->GetStateVector(meshIdx, stageIdx);

      for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
         stateArray(stageIdx, stateIdx) = stateVec(stateIdx);
   }

   for (Integer stageIdx = 0; stageIdx < numControlIdxs; stageIdx++)
   {
      Rvector controlVec = ptrDecVector->GetControlVector(meshIdx, stageIdx);

      for (Integer ctrIdx = 0; ctrIdx < numControlVars; ctrIdx++)
         controlArray(stageIdx, ctrIdx) = controlVec(ctrIdx);
   }

   for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
   {
      Rvector tmp(numStateIdxs);
      for (Integer stageIdx = 0; stageIdx < numStateIdxs; stageIdx++)
         tmp(stageIdx) = stateArray(stageIdx, stateIdx);

      stateVecRvector.push_back(tmp);
   }
   for (Integer ctrIdx = 0; ctrIdx < numControlVars; ctrIdx++)
   {
      Rvector tmp(numControlIdxs);
      for (Integer stageIdx = 0; stageIdx < numControlIdxs; stageIdx++)
         tmp(stageIdx) = controlArray(stageIdx, ctrIdx);

      controlVecRvector.push_back(tmp);
   }
}
//------------------------------------------------------------------------------
// Rvector CalWeightVec(DecVecTypeBetts *ptrDecVector, Integer meshIdx)
//------------------------------------------------------------------------------
/**
 * calculate weight vector to normalize collocation error w.r.t. state magnitude
 *
 * @param <ptrDecVector>     the pointer of the decision vector of phase                            
 * @param <meshIdx>          mesh index
 *
 * @return weight vector
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_ImplicitRK::CalWeightVec(DecVecTypeBetts *ptrDecVector,
                                             Integer          meshIdx)
{
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Rvector weightVec(numStateVars);
   
   // Loop over the steps in the phase
   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   std::vector<std::vector<Real> > absStateHistory, absDynHistory;
    
   // Loop over points used in current step
   for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
   {               
      Integer pointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, stageIdx);

      Real time = discretizationPoints(pointIdx);
      Rvector stateVec = ptrDecVector->GetStateVector(meshIdx, stageIdx);
      Rvector controlVec = ptrDecVector->GetControlVector(meshIdx, stageIdx);

      FunctionOutputData *dynFuncData;
      dynFuncData = NLPFuncUtil_Coll::GetUserDynFuncData(time, stateVec,
                                                         controlVec);
      Rvector dynVec = dynFuncData->GetFunctionValues();

      if (stageIdx == 0)
      {
         for (Integer idx2 = 0; idx2 < ptrConfig->GetNumStateVars(); ++idx2)
         {
            // dynFuncArray does not include the first state
            std::vector<Real> tmpVec;
            tmpVec.push_back(GmatMathUtil::Abs(dynVec(idx2)));
            absDynHistory.push_back(tmpVec);

            tmpVec.clear();
            tmpVec.push_back(GmatMathUtil::Abs(stateVec(idx2)));
            absStateHistory.push_back(tmpVec);
         }
      }
      else
      {
         for (Integer idx2 = 0; idx2 < ptrConfig->GetNumStateVars(); ++idx2)
         {
            // dynFuncArray does not include the first state
            absDynHistory[idx2].push_back(GmatMathUtil::Abs(dynVec(idx2)));
            absStateHistory[idx2].push_back(
                                  GmatMathUtil::Abs(stateVec(idx2)));
         }
      }
   }            


   for (Integer idx=0; idx< numStateVars; idx++)
   {
      std::vector<Real>::iterator result;
      result = std::max_element(absDynHistory[idx].begin(),
                                absDynHistory[idx].end());
      Real maxDynValue = *result;

      result = std::max_element(absStateHistory[idx].begin(),
                                absStateHistory[idx].end());
      Real maxStateValue = *result;
      weightVec(idx)= GmatMathUtil::Max(maxDynValue,maxStateValue);
   }
   return weightVec;
};
//------------------------------------------------------------------------------
//IntegerArray GetOrderReduction(
//                                    Rvector *oldMeshFractions,
//                                    IntegerArray *oldMeshNumPoints,
//                                    Rvector  *oldCollocErrorVec,
//                                    std::vector<IntegerArray> *addNodeNumVec,
//                                    std::vector<Real> *newCollocErrorVec)
//------------------------------------------------------------------------------
/**
* Returns the order reduction vector indicating the gap between the expected 
*         mesh-refinement performance and the real one. 
*         Now muted for interface issue.
*
* @param <oldMeshFractions>  input: the previous-previous mesh fractions 
* @param <oldMeshNumPoints>  input: the previous-previous mesh number points
* @param <oldCollocErrorVec> input: the previous-previous collocation error vector
* @param <addNodeNumVec>     input: the previous add node number vector 
* @param <newCollocErrorVec> input: the previous collocation error vector 
*
* @return the order reduction vector
*/
//------------------------------------------------------------------------------
IntegerArray NLPFuncUtil_ImplicitRK::GetOrderReduction(
                                       Rvector *oldMeshFractions,
                                       IntegerArray *oldMeshNumPoints,
                                       Rvector  *oldCollocErrorVec,
                                       std::vector<IntegerArray> *addNodeNumVec,
                                       std::vector<Real> *newCollocErrorVec)
{
   Integer numMeshIntv = oldMeshNumPoints->size();
   IntegerArray orderReductionVec;
   Integer counter = 0;
   Real rk, r;

   // loop over old mesh intervals.
   for (Integer i = 0; i < numMeshIntv; i++)
   {
      Real orderReduction = 0.0, oldNumPts = double((*oldMeshNumPoints)[i]);
      // loop over subintervals 
      for (Integer j = 0; j < addNodeNumVec[i].size(); j++)
      {
         if (addNodeNumVec[i].size() == 1)
         {
            // no sub intervals, just adding points
            rk = double(pValue) + 1.0
               - (log((*oldCollocErrorVec)(i)
                  / (*newCollocErrorVec)[counter + j]))
               / (log(oldNumPts
                  / double(oldNumPts + (*addNodeNumVec)[i][j])));
            r = GmatMathUtil::Max(0.0, GmatMathUtil::Min(rk, pValue));
            orderReductionVec.push_back(round(r));
         }

         // at the last subinterval, update counter; so that 
         // next iteration begins with new mesh interval.
         if (j + 1 == addNodeNumVec[i].size())
            counter += j + 1;
      }
   }
   return orderReductionVec;
};


//------------------------------------------------------------------------------
// Rvector GetCollocError(DecVecTypeBetts* ptrDecVector,
//                        Integer          meshIdx)
//------------------------------------------------------------------------------
/**
 * Returns the collocation error
 *
 * @param <ptrDecVector>        input: decision vector
 * @param <meshIdx>             input: mesh index
 *
 * @return the collocation error
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_ImplicitRK::GetCollocError(DecVecTypeBetts* ptrDecVector,
                                               Integer          meshIdx)
{
   std::vector<Rvector> stateVecRvector, controlVecRvector;
   bool hasStateAtFinal = true, hasControlAtFinal = true;
   GetStateAndControlInMesh(meshIdx, ptrDecVector, stateVecRvector, 
                            controlVecRvector, hasStateAtFinal, 
                            hasControlAtFinal);

   //   operate for each mesh interval
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer startPointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, 0);
   Integer endPointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx+1, 0);

   Rvector timeVec(numStages);   
   Rmatrix dynArray(numStages, numStateVars);

   std::vector<Rvector> dynArray_wrt_var;
   FunctionOutputData *dynFuncData;

   // Loop over stage points used in current interval
   for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
   {
      Integer pointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, stageIdx);

      Real time = discretizationPoints(pointIdx);
      timeVec(stageIdx) = time;
      Rvector stateVec = ptrDecVector->GetStateVector(meshIdx, stageIdx);
      Rvector controlVec = ptrDecVector->GetControlVector(meshIdx, stageIdx);
      dynFuncData = NLPFuncUtil_Coll::GetUserDynFuncData(time, stateVec,
                                                         controlVec);
      Rvector dynVec = dynFuncData->GetFunctionValues();

      for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
         dynArray(stageIdx, stateIdx) = dynVec(stateIdx);
      
   }
   // TBD: compute wrt_var for state, control, dyn
   for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
   {
      Rvector tmp(numStages);      
      for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
      {
         // we need to consider time normalization effect
         tmp(stageIdx) = deltaTime*dynArray(stageIdx, stateIdx);
      }
      dynArray_wrt_var.push_back(tmp);
   }

   BaryLagrangeInterpolator controller;

   return GetRombergIntegration(numStateVars,
                                discretizationPoints(startPointIdx),
                                discretizationPoints(endPointIdx),
                                &timeVec, stateVecRvector, 
                                controlVecRvector, &controller,
                                dynArray_wrt_var);
};
//------------------------------------------------------------------------------
// void GetCollocErrorVec(
//                         DecVecTypeBetts* ptrDecVector,
//                         Rvector          &discErrorVec,
//                         std::vector<std::vector<Real> > &discErrorArray,
//                         std::vector<Real> &discErrorPerIntv)
//------------------------------------------------------------------------------
/**
 * Returns the collocation error vector
 *
 * @param <ptrDecVector>    input: decision vector
 * @param <discErrorVec>    output: discretization error vector per steps
 * @param <discErrorArray>  output: discretization errors of states per steps
 * @param <discErrorPerIntv>output:discretization error vector per mesh fraction
 *
 * @return the collocation error vector
 *
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::GetCollocErrorVec(
                                DecVecTypeBetts* ptrDecVector,
                                Rvector          &discErrorVec,
                                std::vector<std::vector<Real> > &discErrorArray, 
                                std::vector<Real> &discErrorPerIntv)
{
   discErrorArray.clear();
   
   discErrorVec.SetSize(numStepsInPhase);
   
   for (Integer meshIdx = 0; meshIdx < numStepsInPhase; meshIdx++)
   {
      Rvector weightVec = CalWeightVec(ptrDecVector, meshIdx);
      std::vector<Real> tmp;
      Rvector discError = GetCollocError(ptrDecVector, meshIdx);
      for (Integer stateIdx = 0; stateIdx < weightVec.GetSize(); stateIdx++)
         tmp.push_back(1.0 / (1.0 + weightVec(stateIdx))*discError(stateIdx));

      std::vector<Real>::iterator result;
      result = max_element(tmp.begin(), tmp.end());
      discErrorVec(meshIdx) = *result;
      discErrorArray.push_back(tmp);
   }
#ifdef DEBUG_MR
   MessageInterface::ShowMessage(
      "Discretization error per each step is given as follows:\n%s\n",
      discErrorVec.ToString(12).c_str());
#endif

   // start point of the mesh point of the given interval index
   

   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();
   discErrorPerIntv.clear();
   for (Integer intvIdx = 0; intvIdx < meshNumPoints.size(); intvIdx++)
   {
      Integer pointIdx = 0;

      for (Integer intervalIdx = 0; intervalIdx < intvIdx; intervalIdx++)
      {
         for (Integer stepIdx = 0; stepIdx < meshNumPoints[intervalIdx] - 1;
            stepIdx++)
         {
            pointIdx = pointIdx + 1;
         }
      }
      Real errorPerIntv = 0.0;
      for (Integer idx = pointIdx; idx < pointIdx + meshNumPoints[intvIdx]-1;
         idx++)
      {
         // pick the maximum discretization error
         errorPerIntv = GmatMathUtil::Max(errorPerIntv, 
                                          GmatMathUtil::Abs(discErrorVec(idx))); 
      }
      discErrorPerIntv.push_back(errorPerIntv);
   }   
};

//------------------------------------------------------------------------------
// Rvector RombFuncWrapper(const Real time, const Rvector* timePoints,
//                         const std::vector< Rvector>      &stateArray,
//                         const std::vector< Rvector>      &controlArray,
//                         BaryLagrangeInterpolator* controller,
//                         const std::vector< Rvector>      &dynArray)
//------------------------------------------------------------------------------
/**
* Computes pseudoDynVec-dynFunc(t,stateVec,controlVec)
* deliver "timePoints, stateArray, controller, dynVector"
* in order to increase speed (remove unnecessary computation for data points)
*
* @param <interpPoints> input: when the pseudo dynamics vector required
* @param <dataPoints>   input: the collocation points (stage points)
* @param <stateArray>   input: the state vectors of stage points
* @param <controlArray> input: the control vectors of stage points
* @param <controller>   input: the Lagrange interpolator to interpolate control
* @param <dynArray>     input: the dynamics vectors of stage points
*
% Returns pseudo dynamics vector
*/
//------------------------------------------------------------------------------
Rmatrix NLPFuncUtil_ImplicitRK::RombFuncWrapper(
                                 const Rvector                    *interpPoints,
                                 const Rvector                    *dataPoints,
                                 const std::vector< Rvector>      &stateArray,
                                 const std::vector< Rvector>      &controlArray,
                                 BaryLagrangeInterpolator         *controller,
                                 const std::vector< Rvector>      &dynArray)
{
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer numControlVars = ptrConfig->GetNumControlVars();
   Integer numDataPoints = dataPoints->GetSize();
   Integer numInterpPoints = interpPoints->GetSize();

   std::vector<Rvector> pseudoStateVec, pseudoDynVec;
   Rmatrix integrand(numInterpPoints, numStateVars);

   bool noInterpFlag = false;
   Rvector normDataPoints(numDataPoints), normInterpPoints(numInterpPoints);

   for (Integer idx = 0; idx < numDataPoints; idx++)
      normDataPoints(idx) = (*dataPoints)(idx) - (*dataPoints)(0);
   for (Integer idx = 0; idx < numInterpPoints; idx++)
      normInterpPoints(idx) = (*interpPoints)(idx) - (*dataPoints)(0);

   LobattoIIIaMathUtil::HermiteInterpolation(&normInterpPoints, &normDataPoints,
                                             stateArray, dynArray,
                                             pseudoStateVec, pseudoDynVec);
   
   FunctionOutputData *dynFuncData;
   for (Integer j = 0; j < numInterpPoints; j++)
   {
      Rvector controlVec(ptrConfig->GetNumControlVars());
      for (Integer i = 0; i < numControlVars; i++)
      {
         Rvector tmpInterpPts(1, normInterpPoints(j));
         bool interpFlag = true;
         for (Integer k = 0; k < numDataPoints; k++)
         {
            if (tmpInterpPts(0) == normDataPoints(k))
            {            
               interpFlag = false;
               controlVec(i) = controlArray[i](k);
               break;
            }
         }
         if (interpFlag == true)
         {
            Rvector tmpVec;
            controller->Interpolate(&normDataPoints, &controlArray[i],
                                    &tmpInterpPts, tmpVec);
            controlVec(i) = tmpVec(0);
         }
      }
   
      dynFuncData = NLPFuncUtil_Coll::GetUserDynFuncData((*interpPoints)(j), 
                                                         pseudoStateVec[j],
                                                         controlVec);
      Rvector dynVec = dynFuncData->GetFunctionValues();

      Rvector tmpIntegrand(numStateVars);
      tmpIntegrand = (pseudoDynVec[j] - deltaTime*dynVec);
      for (Integer i = 0; i < numStateVars; i++)
      {
         Real tmp = GmatMathUtil::Abs(tmpIntegrand(i));
         integrand(j,i) = tmp / deltaTime;
      }
   }

   return integrand;
};
//------------------------------------------------------------------------------
// Rvector GetRombergIntegration(Integer        numFuncs,   Real tInit,
//                               Real           tFin,
//                               const Rvector* timePoints,
//                               const std::vector<Rvector> &stateArray,
//                               const std::vector<Rvector> &controlArray,
//                               BaryLagrangeInterpolator *controller,
//                               const std::vector<Rvector> &dynArray,
//                               Integer decimalDigits)
//------------------------------------------------------------------------------
/**
 * Returns the Romberg integration
 *
 * @param <numFuncs>        number of functions
 * @param <tInit>           initial time
 * @param <tIFin>           final time
 * @param <timePoints>      array of time points
 * @param <stateArray>      state array
 * @param <controlArray>    control array
 * @param <controller>      interpolator
 * @param <dynArray>        dynamics array
 * @param <decimalDigits>   number of digits for decimal
 *
 * @return the Romberg integration
 *
 * @note :
 * original author: Martin Kacenak from Faculty of BERG, Technical University of
 * Kosice, Slovak Republic; modified by Youngkwang Kim from Yonsei Univ., 
 * South Korea.
 */
//------------------------------------------------------------------------------
Rvector NLPFuncUtil_ImplicitRK::GetRombergIntegration(Integer numFuncs,
                    Real tInit, Real tFin, const Rvector *timePoints,
                    const std::vector<Rvector> &stateArray,
                    const std::vector<Rvector> &controlArray,
                    BaryLagrangeInterpolator *controller,
                    const std::vector<Rvector> &dynArray, Integer decimalDigits)
{

   Rmatrix rom(2, decimalDigits);
   Rmatrix romall(pow(2, (decimalDigits - 1)) + 1, numFuncs);
   Rvector timeVec(pow(2, (decimalDigits - 1)) + 1);

   for (Integer i = 0; i < (pow(2, (decimalDigits - 1)) + 1); i++)
   {
      Real multiplier = (double)i;
      timeVec(i) = tInit + multiplier*(tFin - tInit) / pow(2.0,
                   (decimalDigits - 1));
   }

   #ifdef DEBUG
      MessageInterface::ShowMessage("timeVector is:\n%s\n",
         timeVec.ToString(16).c_str());
   #endif // DEBUG

   Rmatrix result((pow(2, (decimalDigits - 1)) + 1),numFuncs);
      // call method pointer
   romall = RombFuncWrapper(&timeVec, timePoints, stateArray,
                            controlArray, controller, dynArray);

   #ifdef DEBUG
      MessageInterface::ShowMessage("romall is:\n%s\n",
         romall.ToString(16).c_str());
   #endif // DEBUG

   Real h;
   Rvector output(numFuncs);

   for (Integer j = 0; j <numFuncs; j++)
   {
      h = tFin - tInit;
      #ifdef DEBUG
         MessageInterface::ShowMessage("h is:\n%le", h);
      #endif // DEBUG

      rom(0, 0) = h*(romall(0, j) + romall(pow(2, (decimalDigits - 1)), j)) /
                  2.0;

      for (Integer i = 2; i <= decimalDigits; i++)
      {
         Integer st = pow(2, (decimalDigits - i + 1));
         Real sumValue = 0.0;
         for (Integer index = st / 2; index < pow(2, (decimalDigits - 1));
              index += st)
            sumValue += romall(index, j);

         #ifdef DEBUG
            MessageInterface::ShowMessage("sumValue is:\nle", sumValue);
         #endif // DEBUG

         rom(1, 0) = (rom(0, 0) + h*sumValue) / 2.0;
         for (Integer k = 1; k <= i - 1; k++)
         {
            rom(1, k) = (pow(4.0, k)*rom(1, k - 1) - rom(0, k - 1)) /
                        (pow(4.0, k) - 1.0);
         }
         #ifdef DEBUG
            MessageInterface::ShowMessage("rom is:\n%s\n",
            rom.ToString(16).c_str());
         #endif // DEBUG
         for (Integer idx = 0; idx < i; idx++)
            rom(0, idx) = rom(1, idx);

         h = h / 2.0;
         #ifdef DEBUG
            MessageInterface::ShowMessage("h is:\n%le", h);
         #endif // DEBUG
      }
      output(j) = rom(0, decimalDigits - 1);
      for (Integer idx1 = 0; idx1 < 2; idx1++)
      {
         for (Integer idx2 = 0; idx2 < decimalDigits; idx2++)
            rom(idx1, idx2) = 0.0;
      }
   }
   #ifdef DEBUG
      MessageInterface::ShowMessage("rom is:\n%s\n",
         rom.ToString(16).c_str());
   #endif // DEBUG
   return output;
};


//------------------------------------------------------------------------------
// void InterpolateInMesh(
//                         DecVecTypeBetts         *ptrDecVector,
//                         const std::vector<Real> *interpPts,
//                         const Integer           meshIdx,
//                         Rmatrix                 &stateArray,
//                         Rmatrix                 &controlArray)
//------------------------------------------------------------------------------
/**
* Computes pseudoDynVec-dynFunc(t,stateVec,controlVec)
* deliver "timePoints, stateArray, controller, dynVector"
* in order to increase speed
* (no unnecessary computation for each Romberg data points)
*
* @param <ptrDecVector> input: the pointer to the decision vector of phase
* @param <interpPts>    input: the interpolation points that need interpolation 
* @param <meshIdx>      input: the mesh index of interest
* @param <stateArray>   output: the state array interpolated at interpPts 
* @param <controlArray> output: the control array interpolated at interpPts 
*
*/
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::InterpolateInMesh(
                                          DecVecTypeBetts         *ptrDecVector,
                                          const std::vector<Real> *interpPts, 
                                          const Integer           meshIdx,
                                          Rmatrix                 &stateArray, 
                                          Rmatrix                 &controlArray)
{
   std::vector<Rvector> stateVecRvector, controlVecRvector;
   bool hasStateAtFinal = true, hasControlAtFinal = true;
   GetStateAndControlInMesh(meshIdx, ptrDecVector, stateVecRvector,
      controlVecRvector, hasStateAtFinal,
      hasControlAtFinal);

   //   operate for each mesh interval
   Integer numStateVars = ptrConfig->GetNumStateVars();
   Integer numControlVars = ptrConfig->GetNumControlVars();
   Integer startPointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, 0);
   Integer endPointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx + 1, 0);

   Rvector timeVec(numStages);
   Rmatrix dynArray(numStages, numStateVars);

   std::vector<Rvector> dynVecRvector;
   FunctionOutputData *dynFuncData;

   // Loop over stage points used in current interval
   for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
   {
      Integer pointIdx = GetPointIdxGivenMeshAndStageIdx(meshIdx, stageIdx);

      Real time = discretizationPoints(pointIdx);
      timeVec(stageIdx) = time;
      Rvector stateVec = ptrDecVector->GetStateVector(meshIdx, stageIdx);
      Rvector controlVec = ptrDecVector->GetControlVector(meshIdx, stageIdx);
      dynFuncData = NLPFuncUtil_Coll::GetUserDynFuncData(time, stateVec,
         controlVec);
      Rvector dynVec = dynFuncData->GetFunctionValues();

      for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
         dynArray(stageIdx, stateIdx) = dynVec(stateIdx);

   }
   // TBD: compute wrt_var for state, control, dyn
   for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
   {
      Rvector tmp(numStages);
      for (Integer stageIdx = 0; stageIdx < numStages; stageIdx++)
      {
         // we need to consider time normalization effect
         tmp(stageIdx) = deltaTime*dynArray(stageIdx, stateIdx);
      }
      dynVecRvector.push_back(tmp);
   }

   // interpolation is necessary
   Rvector normTimeVec(numStages);

   // normalize time for better Hermite interpolation results
   for (Integer stgIdx = 0; stgIdx < numStages; stgIdx++)
   {
      normTimeVec(stgIdx) = timeVec(stgIdx) - timeVec(0);
   }
   Rvector normInterpPts(interpPts->size());
   for (Integer idx = 0; idx < interpPts->size(); idx++)
   {
      normInterpPts(idx) = (*interpPts)[idx] - timeVec(0);
   }

   LobattoIIIaMathUtil::HermiteInterpolation(&normInterpPts, &normTimeVec,
                                             stateVecRvector, dynVecRvector, 
                                             stateArray, dynArray);


   BaryLagrangeInterpolator controller;
   controlArray.SetSize(normInterpPts.GetSize(), numControlVars);
   for (Integer idx = 0; idx < numControlVars; idx++)
   {      
      for (Integer idx2 = 0; idx2 < normInterpPts.GetSize(); idx2++)
      {
         bool noInterpFlag = false;
         Integer timeValIdx;
         for (Integer idx3 = 0; idx3 < normTimeVec.GetSize(); idx3++)
         {
            if (normInterpPts(idx2) == normTimeVec(idx3))
            {
               noInterpFlag = true;
               timeValIdx = idx3;
               break;
            }
         }
         if (!noInterpFlag)
         {
            // do interpolation
            Rvector tmpVec(1, normInterpPts(idx2)), result;
            controller.Interpolate(&normTimeVec, &controlVecRvector[idx],
                                   &tmpVec, result);
            controlArray(idx2, idx) = result(0);
         }
         else
         {
            // use the vale itself
            controlArray(idx2, idx) = controlVecRvector[idx](timeValIdx);
         }
      }         
   }
};


//------------------------------------------------------------------------------
// void GetNewMeshPoints(DecVecTypeBetts *ptrDecVector,
//                       const Rvector   newDErrorVec,
//                       const Integer   intvIdx, const Real orderReduction,
//                       Integer &numNewIntv, Integer &totalAddNodeNum)
//------------------------------------------------------------------------------
/**
 * Returns new mesh points as a result of mesh-refinement
 *
 * @param <ptrDecVector>   the pointer to the decision vector of the phase
 * @param <newDErrorVec>   the discretization error vector
 * @param <intvIdx>        the interval index of interest
 * @param <orderReduction> the order reduction vector
 * @param <numNewIntv>     the number of sub-intervals needed
 * @param <addNodeNum>     the number of additional steps per sub-intervals
 *
 * @return new mesh points as a result of mesh-refinement
 */
//------------------------------------------------------------------------------
void NLPFuncUtil_ImplicitRK::GetNewMeshPoints(DecVecTypeBetts *ptrDecVector,
                                       const Integer      orderReduction,
                                       const Rvector      *newDErrorVec,
                                       const Integer      intvIdx,
                                       Integer            &numNewIntv,
                                       IntegerArray       &addNodeNumVec)
{
   IntegerArray meshNumPoints = ptrConfig->GetMeshIntervalNumPoints();

   // start point of the mesh point of the given interval index
   Integer pointIdx = 0;


   for (Integer intervalIdx = 0; intervalIdx < intvIdx; intervalIdx++)
   {
      for (Integer stepIdx = 0; stepIdx < meshNumPoints[intervalIdx] - 1;
         stepIdx++)
      {
         pointIdx = pointIdx + 1;
      }
   }

   Real maxError = 0;
   for (Integer idx = pointIdx; idx < pointIdx + meshNumPoints[intvIdx] - 1;
      idx++)
   {
      if (maxError < (*newDErrorVec)(idx))
         maxError = (*newDErrorVec)(idx);
   }
   if (maxError < relErrorTol)
   {
      numNewIntv = 1;
      addNodeNumVec.resize(1);
      addNodeNumVec[0] = 0;
      return;
   }
   

   // at least, ten mesh point can be added per each mesh intervals
   
   Integer AddNodeNum = 0;  
   Integer totalAddNodeNum = 0;
  
   // try minimize max. error without splitting the current mesh interval into several
   // as Betts book.. 
   Real newDError = relErrorTol*1000.0;
   AddNodeNum = 0;
   while (AddNodeNum < maxAddNodeNumPerIntv && newDError > relErrorTol)
   {
      AddNodeNum = AddNodeNum + 1;
      Real tmp = double(meshNumPoints[intvIdx]) / double(meshNumPoints[intvIdx] + AddNodeNum);
      newDError = maxError*(pow(tmp, (pValue - orderReduction + 1)));
   }
   totalAddNodeNum = AddNodeNum;
   

   if (totalAddNodeNum < 0)
   {
      numNewIntv = 1;
      addNodeNumVec.resize(1);
      addNodeNumVec[0] = 0;
      return;
   }
   else if (totalAddNodeNum > 50)
   {
      // The initial mesh point configuration is so sparse
      // But adding too much mesh points at once is problematic
      // So, reduce the number of the nodes to be added.
      totalAddNodeNum = 50;
   }

   if (totalAddNodeNum + meshNumPoints[intvIdx] > maxTotalNodeNumPerIntv)
   {
      // we need to split the current mesh intervals into several
      numNewIntv = (totalAddNodeNum + meshNumPoints[intvIdx])/ maxTotalNodeNumPerIntv + 1;

      if ((meshNumPoints[intvIdx] - 1) < numNewIntv)
      {
         // there must be at least one discr. point per new mesh intv.
         numNewIntv = (meshNumPoints[intvIdx] - 1);
      }
      addNodeNumVec.resize(numNewIntv);

      Integer currPoint = pointIdx, nextPoint = pointIdx + (meshNumPoints[intvIdx] - 1)/numNewIntv;

      for (Integer subIntvIdx = 0; subIntvIdx < numNewIntv; subIntvIdx++)
      {
         // note that num points per sub intv may differ at the last sub intv.
         Integer newNumPointsPerSubIntv = nextPoint - currPoint;

         // find the max. error at the current sub intv.
         maxError = 0.0;
         for (Integer i = currPoint; i < nextPoint; i++)
         {
            maxError = GmatMathUtil::Max(maxError, (*newDErrorVec)(i));
         }
         

         // find the required number of points per sub-interval
         // as Betts's book does 
         AddNodeNum = 0;
         newDError = relErrorTol*1000.0;
         while (AddNodeNum < maxAddNodeNumPerIntv && newDError > relErrorTol)
         {
            AddNodeNum = AddNodeNum + 1;
            Real tmp = double(newNumPointsPerSubIntv) / double(newNumPointsPerSubIntv + AddNodeNum);

            newDError = maxError*(pow(tmp, (pValue - orderReduction + 1)));
         }
         addNodeNumVec[subIntvIdx] = AddNodeNum;
         
         // move forward through sub-interval
         currPoint = nextPoint;

         // take care of the final sub-interval
         if (subIntvIdx == numNewIntv - 2)
            nextPoint = pointIdx + (meshNumPoints[intvIdx] - 1);
         else
            nextPoint = nextPoint + (meshNumPoints[intvIdx] - 1) / numNewIntv;
      }
      for (Integer subIntvIdx = 0; subIntvIdx < numNewIntv; subIntvIdx++)
      {
         if (addNodeNumVec[subIntvIdx] > maxAddNodeNumPerIntv)
            addNodeNumVec[subIntvIdx] = maxAddNodeNumPerIntv;
         else if (addNodeNumVec[subIntvIdx] < 0)
            addNodeNumVec[subIntvIdx] = 0;
      }
      return;
   }
   else
   {
      // we do not need to split the current mesh intervals
      numNewIntv = 1;
      addNodeNumVec.resize(1);
      addNodeNumVec[0] = totalAddNodeNum;
      return;
   }

};








