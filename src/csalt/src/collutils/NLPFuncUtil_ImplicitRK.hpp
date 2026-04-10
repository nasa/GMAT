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
* StageIdx    0  1  2  0  1  1  0  1  2  0
* PointIdx    1  2  3  4  5  6  7  8  9  10
* Note:  3 steps means 4 mesh points: numSteps = numMeshPoints - 1
* Total num points = numSteps*(numStages + 1) + 1;  There are two
* stages in this example.
*/
//------------------------------------------------------------------------------

#ifndef NLPFuncUtil_ImplicitRK_hpp
#define NLPFuncUtil_ImplicitRK_hpp

#include <algorithm>
#include <math.h>
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"  // for Abs etc.

#include "NLPFuncUtil_Coll.hpp"
#include "ImplicitRungeKutta.hpp"

#include "LobattoIIIaMathUtil.hpp"
#include "BaryLagrangeInterpolator.hpp"

class NLPFuncUtil_ImplicitRK : public NLPFuncUtil_Coll
{
public:
   // class methods
   //NLPFuncUtil_ImplicitRK();
   
   /// constructor
   NLPFuncUtil_ImplicitRK(std::string method);
   /// copy constructor
   NLPFuncUtil_ImplicitRK(const NLPFuncUtil_ImplicitRK &copy);
   /// operator =
   NLPFuncUtil_ImplicitRK& operator=(const NLPFuncUtil_ImplicitRK &copy);

   /// destructor
   virtual ~NLPFuncUtil_ImplicitRK();

   ///  Compute Radau points, weights, and differentiation matrix
   void    InitializeTranscription();

   void    InitNLPHelpers();

   void    SetTimeVector(Real initialTime, Real finalTime);

   Integer GetMeshIndex(Integer pointIdx);

   Integer GetStageIndex(Integer pointIdx);

   Real    GetdCurrentTimedTI(Integer meshIdx, Integer stageIdx);

   Real    GetdCurrentTimedTF(Integer meshIdx, Integer stageIdx);

   void    RefineMesh(DecVecTypeBetts         *ptrDecVector,
                      UserPathFunctionManager *ptrFunctionManager,
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
   ///  Compute the scaled time value at a given mesh point index
   Real    MeshPointIdxToNonDimTime(Integer meshIdx, Integer stageIdx);

   Integer GetPointIdxGivenMeshAndStageIdx(Integer meshIdx, Integer stageIdx);

   void    SetStageProperties();

   void    ComputeStepSizeVector(Rvector *fractions, IntegerArray *meshNumPoints,
                                 Rvector &discretizationPts, Rvector &stepSizeVec);

   Rvector NormalizeMeshFractions(Rvector *meshIntervalFractions);

   void    SetButcherTable(std::string collocationMethod);

   void    InitStateAndControlInterpolator();

   void    InitializeConstantDefectMatrices(
                        UserFunctionProperties                 *dynFunProps,
                        const std::vector<FunctionOutputData*> &dynFunVector);

   void    InitializeConstantCostMatrices(
                        UserFunctionProperties                 *dynFunProps,
                        const std::vector<FunctionOutputData*> &dynFunVector);
                                       
   void    FillDynamicDefectConMatrices(
                        const std::vector<FunctionOutputData*> &funcData,
                        Rvector                                &valueVec,
                        RSMatrix                               &jacobian);

   void    FillDynamicCostFuncMatrices(
                        const std::vector<FunctionOutputData*> &funcData,
                        Rvector                                &valueVec,
                        RSMatrix                               &jacobian);   
   /// get state and control in the step
   void    GetStateAndControlInMesh(Integer              meshIntvIdx,
                                    DecVecTypeBetts      *ptrDecVector,
                                    std::vector<Rvector> &stateVecRvector,
                                    std::vector<Rvector> &controlVecRvector,
                                    bool                 hasFinalState = true,
                                    bool                 hasFinalControl = true);
   /// calculate weight vector for mesh-refinement
   Rvector CalWeightVec(DecVecTypeBetts* ptrDecVector, Integer meshIdx);
   /// calculate order reduction (muted)
   IntegerArray GetOrderReduction(Rvector                  *oldMeshFractions,
                                 IntegerArray              *oldMeshNumPoints,
                                 Rvector                   *oldCollocErrorVec,
                                 std::vector<IntegerArray> *addNodeNumVec,
                                 std::vector<Real>         *newCollocErrorVec);
   /// calculate collocation error in the mesh interval
   Rvector GetCollocError(DecVecTypeBetts* ptrDecVector, Integer meshIdx);
   /// calculate collocation error throughout the mesh points
   void    GetCollocErrorVec(DecVecTypeBetts* ptrDecVector, Rvector &discErrorVec,
                             std::vector<std::vector<Real> > &discErrorArray,
                             std::vector<Real>  &maxDiscErrorPerIntv);
   /// a method to utilize Romberg integrator 
   Rmatrix RombFuncWrapper(const Rvector *interpPoints, 
                           const Rvector *dataPoints,
                           const std::vector< Rvector>     &stateArray,
                           const std::vector< Rvector>     &controlArray,
                           BaryLagrangeInterpolator        *controller,
                           const std::vector< Rvector>     &dynArray);
   /// conduct romberg integration 
   Rvector GetRombergIntegration(Integer numFuncs, Real tInit, Real tFin,
                                 const Rvector*             timePoints,
                                 const std::vector<Rvector> &stateArray,
                                 const std::vector<Rvector> &controlArray,
                                 BaryLagrangeInterpolator* controller,
                                 const std::vector<Rvector> &dynArray,
                                 Integer decimalDigits = 8);
   /// conduct interpolation in the step 
   void    InterpolateInMesh(DecVecTypeBetts         *ptrDecVector,
                             const std::vector<Real> *interpPts, 
                             const Integer meshIdx,
                             Rmatrix &stateArray, Rmatrix &controlArray);
   /// generate new mesh points in mesh-refinement
   void    GetNewMeshPoints(DecVecTypeBetts*   ptrDecVector,
                            const Integer      orderReduction,
                            const Rvector      *newCollocError,
                            const Integer      intvIdx,
                            Integer            &numNewIntv,
                            IntegerArray       &addNodeNum);
   /// Number of points per mesh
   Integer numPointsPerMesh;
   ///  IRK data utility.
   ImplicitRungeKutta *butcherTableData;
   /// Number of stages in the IRK method
   Integer numStages;
   /// Number of steps in the phase
   Integer numStepsInPhase;
   /// Indicates constraint matrices A and B are initialized
   bool    isConMatInitialized;
   /// The number of ode RHS terms.  numStates*numPoints
   Integer numODERHS;
   /// Vector of non-dimensional step sizes
   Rvector stepSizeVec;
   /// polynomial order == number of points used in collocation
   Integer pValue;      
   /// array indicating number of nodes that is added in the previous simulation
   std::vector<IntegerArray> addNodeNumVecArray;
   /// maximum number of points per interval. fixed as 20
   Integer maxTotalNodeNumPerIntv; 
   /// maximum number of points can be added in an interval. fixed as 15
   Integer maxAddNodeNumPerIntv;

   // mute order reduction related things. 
   /// order reduction indicating collocation performance degrade
   ///IntegerArray orderReductionVec;   
   //Rvector oldCollocErrorVec;
   //Rvector oldMeshFractions;
   //std::vector<Real> oldMaxCollocErrorPerIntv;
   //IntegerArray oldMeshNumPoints;
};



#endif // NLPFuncUtil_ImplicitRK_hpp        
    
        
     
        
