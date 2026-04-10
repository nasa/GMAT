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


#include "NLPFuncUtilRadau.hpp"
#include <iostream>
#include "Rvector.hpp"
#include "ProblemCharacteristics.hpp"
#include "FunctionOutputData.hpp"
#include "UserFunctionProperties.hpp"
#include "DecVecTypeBetts.hpp"
#include "UserPathFunction.hpp"

class DecVecBrysonMax  : public DecVecTypeBetts
{
public:
   // class methods
   DecVecBrysonMax() {
      numParamsPerMesh = 28  ;
      numStateAndControlVars = 5  ;
      numStagePoints = 0  ;
      hasControlAtFinalMesh = false  ;
      numStateVars = 3 ;
      numControlVars = 2  ;
      numStateMeshPoints = 6  ;
      numControlMeshPoints = 5  ;
      numStateStagePoints = 0  ;
      numControlStagePoints = 0  ;
      numStatePoints = 6  ;
      numControlPoints = 5  ;
      numStatePointsPerMesh = 1  ;
      numControlPointsPerMesh = 1  ;
      numDecisionParams = 30  ;
      decisionVector.SetSize(30);
      decisionVector[0] = 0;
      decisionVector[1] = 2.000000000000000;
      decisionVector[2] = 0;
      decisionVector[3] = 0;
      decisionVector[4] = 0;
      decisionVector[5] = -0.000000076604530;
      decisionVector[6] = -1.000000000021982;
      decisionVector[7] = 0.011203713932112;
      decisionVector[8] = -0.060092106971108;
      decisionVector[9] = 0.414941720985215;
      decisionVector[10] = 0.371931438744632;
      decisionVector[11] = -0.928260203224040;
      decisionVector[12] = 0.328993933209084;
      decisionVector[13] = -0.312590960539967;
      decisionVector[14] = 1.058007166968678;
      decisionVector[15] = 0.939464115603286;
      decisionVector[16] = -0.342647304810254;
      decisionVector[17] = 1.077589103360819;
      decisionVector[18] = -0.301403382447943;
      decisionVector[19] = 1.353685287222132;
      decisionVector[20] = 0.941325152410502;
      decisionVector[21] = 0.337501048064107;
      decisionVector[22] = 1.591627798495716;
      decisionVector[23] = 0.005700227057782;
      decisionVector[24] = 1.356474785664222;
      decisionVector[25] = 0.786340489937462;
      decisionVector[26] = 0.617793358588485;
      decisionVector[27] = 1.712373981051012;
      decisionVector[28] = 0.100000000000000;
      decisionVector[29] = 1.340403513677551;
      numStateParams = 18  ;
      numControlParams = 10  ;
      numIntegralParams = 0  ;
      numStaticParams = 0  ;
      integralStartIdx = 31 ;
      integralStopIdx = 30  ;
      timeStartIdx = 0  ;
      timeStopIdx = 1  ;
      staticStartIdx = 31 ;
      staticStopIdx = 30  ;    
   };                            
};  


class DynFuncPropsBrysonMax  : public UserFunctionProperties
{
public:
   // class methods
   DynFuncPropsBrysonMax() {
      stateJacobianPattern.SetSize(3,3); 
      timeJacobianPattern.SetSize(3,1); 
      controlJacobianPattern.SetSize(3,2); 
      for (int idx1 = 0; idx1 < 3; ++idx1)
      {
         timeJacobianPattern(idx1,0) = 0;
         for (int idx2 = 0; idx2 < 2; ++idx2)
         {
            stateJacobianPattern(idx1,idx2) = 0;
            controlJacobianPattern(idx1,idx2) = 0;
         }
         stateJacobianPattern(idx1,2) = 0;
      }
      stateJacobianPattern(0,2) = 1;
      stateJacobianPattern(1,2) = 1;
      controlJacobianPattern(0,0) = 1;
      controlJacobianPattern(1,1) = 1;
      controlJacobianPattern(2,1) = 1;
      
      numFunctions = 3  ;
      hasStateVars = true ;
      hasControlVars = true ;
   };                           
};                      

class ProblemCharacteristicsBrysonMax : public ProblemCharacteristics
{
public:
   // class methods
   ProblemCharacteristicsBrysonMax() {
         numStateVars = 3; 
         numControlVars = 2; 
         numIntegralVars = 0; 
         numStaticVars = 0; 
         numTimeVars = 2; 
         numStateVarsNLP = 18; 
         numTimeVarsNLP = 2; 
         numControlVarsNLP = 10; 
         numDecisionVarsNLP = 30; 
         numDefectConNLP = 15; 
         numTotalConNLP = 15; 
         hasStateVars = true; 
         hasControlVars = true; 
         hasIntegralVars = false; 
         hasStaticVars= false; 
         hasTimeVars= false; 
         hasDefectCons= true; 
         hasIntegralCons= false; 
         hasAlgPathCons= true; 
         hasIntegralCost= false; 
         hasAlgebraicCost= false; 
         meshIntervalFractions.SetSize(2);
         meshIntervalFractions(0) = -1;
         meshIntervalFractions(1) = 1;
         meshIntervalNumPoints.resize(1);
         meshIntervalNumPoints[0] = 5; 
         numMeshIntervals= 1; 
         stateLowerBound.SetSize(3);
         stateLowerBound(0)= -10; 
         stateLowerBound(1)= -10; 
         stateLowerBound(2)= -10; 
         stateUpperBound.SetSize(3);
         stateUpperBound(0) = 10;
         stateUpperBound(1) = 10;
         stateUpperBound(2) = 10;

         stateInitialGuess.SetSize(3);
         stateInitialGuess(0)= 0; 
         stateInitialGuess(1)= 0; 
         stateInitialGuess(2)= 0; 
         stateFinalGuess.SetSize(3);
         stateFinalGuess(0)= 1;
         stateFinalGuess(1)= 1;
         stateFinalGuess(2)= 1;
         timeInitialGuess= 0; 
         timeFinalGuess= 2; 
         timeLowerBound= 0; 
         timeUpperBound= 2; 
         controlUpperBound.SetSize(2);
         controlUpperBound(0)= 2; 
         controlUpperBound(1)= 2; 
         controlLowerBound.SetSize(2);
         controlLowerBound(0)= -2; 
         controlLowerBound(1)= -2; 
   }   
};

class BrysonMaxPathObject : public UserPathFunction
{
public:

   /// default constructor
   BrysonMaxPathObject() {};
   /// default destructor
   ~BrysonMaxPathObject() {};


   /// EvaluateFunctions
   void EvaluateFunctions()
   {

      Rvector stateVec = GetStateVector();
      Rvector controlVec = GetControlVector();
      // @TODO add validation of vector size here!

      Real x = stateVec(0);
      Real y = stateVec(1);
      Real v = stateVec(2);
      Real u1 = controlVec(0);
      Real u2 = controlVec(1);

      Real g = 1.0;
      Real a = 0.5*g;
      Rvector dynFunctions(3);
   
      dynFunctions(0) = v*u1;
      dynFunctions(1) = v*u2;
      dynFunctions(2) = a*g - u2;
      
      SetDynFunctions(dynFunctions);

      Rvector algFunction(1), algBounds(1);
      
      algFunction(0) = u1*u1 + u2*u2;
      algBounds(0) = 1.0;

      SetAlgFunctions(algFunction); 
      SetAlgFunctionsUpperBounds(algBounds);
      SetAlgFunctionsLowerBounds(algBounds);
   }

   /// EvaluateJacobians
   void EvaluateJacobians() {};


protected:
};

class TestNLPFuncUtilRadauDefect
{
public:
   static void RunDefectTests()
   {
       
      ProblemCharacteristicsBrysonMax *ptrConfig = new ProblemCharacteristicsBrysonMax();
      
      DecVecBrysonMax DecVector;
      DynFuncPropsBrysonMax dynFuncProps;

      std::vector<FunctionOutputData*> ptrUserDynFuncData;
          
      std::vector<FunctionOutputData> userDynFuncData = GetUserDynFuncData();

      for (Integer idx = 0; idx < userDynFuncData.size(); ++idx)
      {
         ptrUserDynFuncData.push_back(&userDynFuncData[idx]);
      }

      NLPFuncUtilRadau trans(3,10);

      trans.Initialize(ptrConfig);
      trans.SetTimeVector(0,2);
      trans.PrepareToOptimize(dynFuncProps, ptrUserDynFuncData);

      Rvector defectConVec;
      RSMatrix defectConJacobian;

      trans.ComputeDefectFunAndJac(ptrUserDynFuncData, &DecVector, defectConVec, defectConJacobian);

      Rvector trueFuncValues;
      RSMatrix trueJacArray;
      GetTruthData(trueFuncValues, trueJacArray);
      
      std::cout << "NLPFuncUtilRadau Test Begins.\n" << std::endl;

      double error = 0;
      for (int idx = 0; idx < defectConVec.GetSize(); ++idx)
      {
         error += defectConVec(idx) - trueFuncValues(idx);
      }
      std::cout << "The resultant defect constraint function errors are:      " 
                << error << std::endl;

      RSMatrix errorMat = defectConJacobian -trueJacArray;

      std::cout << "The resultant defect constraint Jacobian errors are:      "
                << SparseMatrixUtil::GetAbsTotalSum(&errorMat) << std::endl;



      // test assign, copy constructor operators
      NLPFuncUtilRadau trans2;
      trans2 = trans;      
      NLPFuncUtilRadau trans3 = trans2;

      // test for execution speed!
      clock_t total_start_time, total_end_time, start_time, end_time;
      start_time = clock();
      total_start_time = start_time;
      int numRepetition = 100;
      for (int idx = 0; idx < numRepetition; ++idx)
      {
         trans3.ComputeDefectFunAndJac(ptrUserDynFuncData, &DecVector, defectConVec, defectConJacobian);
      }
      end_time = clock();

      printf("Time consumption for %d defect function and jacobian  computation is: %f.", 
             numRepetition, ((double)(end_time - start_time))/CLOCKS_PER_SEC);

      std::cout << "\n" << std::endl;

      Real maxRelErrorInMesh;
      bool isMeshRefined;
      Rvector newMeshIntervalFractions;
      IntegerArray newMeshIntervalNumPoints;


      BrysonMaxPathObject *ptrUserPathFunction = new BrysonMaxPathObject();
      FunctionInputData *ptrFunctionInputData = new FunctionInputData();
      ptrFunctionInputData->Initialize(3, 2);
      PathFunctionContainer *ptrPathFunctionContainer = new PathFunctionContainer();
      ptrPathFunctionContainer->Initialize();

      BoundData *ptrBoundData = new BoundData();
      ptrBoundData->SetStateLowerBound(ptrConfig->GetStateLowerBound());
      ptrBoundData->SetStateUpperBound(ptrConfig->GetStateUpperBound());
      ptrBoundData->SetControlLowerBound(ptrConfig->GetControlLowerBound());
      ptrBoundData->SetControlUpperBound(ptrConfig->GetControlUpperBound());
      Rvector timeUpper(1, ptrConfig->GetTimeUpperBound());
      Rvector timeLower(1, ptrConfig->GetTimeLowerBound());
      ptrBoundData->SetTimeUpperBound(timeUpper);
      ptrBoundData->SetTimeLowerBound(timeLower);

      UserPathFunctionManager *ptrUserPathFunctionManager = new UserPathFunctionManager();
      ptrUserPathFunctionManager->Initialize(ptrUserPathFunction, ptrFunctionInputData, ptrPathFunctionContainer, ptrBoundData);

      trans3.RefineMesh(&DecVector, ptrUserPathFunctionManager, isMeshRefined, newMeshIntervalNumPoints,
                        newMeshIntervalFractions, maxRelErrorInMesh);

      std::cout << "The resultant new mesh fractions are:      " << std::endl;
      for (int idx = 0; idx < newMeshIntervalFractions.GetSize(); ++idx)
      {
         std::cout << "newMeshIntervalFractions(" << idx << ") = " 
                   << newMeshIntervalFractions(idx) << std::endl;
      }
      std::cout << "The resultant new mesh numbers of intervals are:      " << std::endl;
      for (int idx = 0; idx < newMeshIntervalNumPoints.size(); ++idx)
      {
         std::cout << "newMeshIntervalNumPoints(" << idx << ") = "
            << newMeshIntervalNumPoints[idx] << std::endl;
      }
      std::cout << "The resultant maximum relative error in mesh = " << maxRelErrorInMesh << std::endl;

      std::cout << "The relative error of thee maximum relative error compared to MATLAB result = ";
      std::cout << (maxRelErrorInMesh - 0.003588128329767) / 0.003588128329767 << std::endl;

   };

   static void GetTruthData(Rvector &funcValues, RSMatrix &jacArray)
   {
      funcValues.SetSize(15);
      
      funcValues[0] =      3.552713678800501e-15;
      funcValues[1] =      2.081668171172169e-15;
      funcValues[2] =      1.998401444325282e-15;
      funcValues[3] =      1.672204319458359e-10;
      funcValues[4] =      6.628542159603512e-11;
      funcValues[5] =      1.310063169057685e-14;
      funcValues[6] =     -1.378752667591243e-11;
      funcValues[7] =     -3.792910430178154e-11;
      funcValues[8] =     -1.998401444325282e-15;
      funcValues[9] =      6.038058941726376e-12;
      funcValues[10] =     -1.674160809983505e-11;
      funcValues[11] =     -8.326672684688674e-15;
      funcValues[12] =     -1.247713043994736e-11;
      funcValues[13] =      1.603561727847591e-11;
      funcValues[14] =      1.720845688168993e-14;

      jacArray.resize(15,30);

      RSMatrix rowData(16,31);
      rowData(3,1) = 7.500000001137400e-01;
      rowData(4,1) = 7.716504847615936e-02;
      rowData(5,1) =-1.925868621293861e-01;
      rowData(6,1) = 7.141299813221544e-01;
      rowData(7,1) = 4.969798066018744e-01;
      rowData(8,1) =-1.812619409764532e-01;
      rowData(9,1) = 4.213239168356230e-01;
      rowData(10,1) = 6.371289071787324e-01;
      rowData(11,1) = 2.284355232582131e-01;
      rowData(12,1) = 8.124917707487750e-02;
      rowData(13,1) = 5.333257079421525e-01;
      rowData(14,1) = 4.190102323448718e-01;
      rowData(15,1) =-5.889646542392252e-02;
      rowData(3,2) =-7.500000001137400e-01;
      rowData(4,2) =-7.716504847615936e-02;
      rowData(5,2) = 1.925868621293861e-01;
      rowData(6,2) =-7.141299813221544e-01;
      rowData(7,2) =-4.969798066018744e-01;
      rowData(8,2) = 1.812619409764532e-01;
      rowData(9,2) =-4.213239168356230e-01;
      rowData(10,2) =-6.371289071787324e-01;
      rowData(11,2) =-2.284355232582131e-01;
      rowData(12,2) =-8.124917707487750e-02;
      rowData(13,2) =-5.333257079421525e-01;
      rowData(14,2) =-4.190102323448718e-01;
      rowData(15,2) = 5.889646542392252e-02;
      rowData(1,3) =-6.500000000000000e+00;
      rowData(4,3) =-1.404991827639855e+00;
      rowData(7,3) = 3.285313785671800e-01;
      rowData(10,3) =-1.371690388875971e-01;
      rowData(13,3) = 6.685308192460790e-02;
      rowData(2,4) =-6.500000000000000e+00;
      rowData(5,4) =-1.404991827639855e+00;
      rowData(8,4) = 3.285313785671800e-01;
      rowData(11,4) =-1.371690388875971e-01;
      rowData(14,4) = 6.685308192460790e-02;
      rowData(1,5) =-2.026854473646720e-06;
      rowData(2,5) = 1.000000000227480e+00;
      rowData(3,5) =-6.500000000000000e+00;
      rowData(6,5) =-1.404991827639855e+00;
      rowData(9,5) = 3.285313785671800e-01;
      rowData(12,5) =-1.371690388875971e-01;
      rowData(15,5) = 6.685308192460790e-02;
      rowData(3,7) = 1.000000000583867e+00;
      rowData(1,8) = 9.109641155544052e+00;
      rowData(4,8) =-2.906165262904104e-01;
      rowData(7,8) =-1.259160474605532e+00;
      rowData(10,8) = 4.329503901415673e-01;
      rowData(13,8) =-1.998526039699828e-01;
      rowData(2,9) = 9.109641155544052e+00;
      rowData(5,9) =-2.906165262904104e-01;
      rowData(8,9) =-1.259160474605532e+00;
      rowData(11,9) = 4.329503901415673e-01;
      rowData(14,9) =-1.998526039699828e-01;
      rowData(3,10)= 9.109641155544052e+00;
      rowData(4,10)=-3.719320426642803e-01;
      rowData(5,10)= 9.282599627846722e-01;
      rowData(6,10)=-2.906165262904104e-01;
      rowData(9,10)=-1.259160474605532e+00;
      rowData(12,10)= 4.329503901415673e-01;
      rowData(15,10)=-1.998526039699828e-01;
      rowData(4,11)=-4.149416540921713e-01;
      rowData(5,12)=-4.149416538146156e-01;
      rowData(6,12)= 9.999999983634211e-01;
      rowData(1,13)=-4.388557102075239e+00;
      rowData(4,13)= 2.594170453203594e+00;
      rowData(7,13)=-4.283826226985887e-01;
      rowData(10,13)=-1.181898588034305e+00;
      rowData(13,13)= 4.375931981001326e-01;
      rowData(2,14)=-4.388557102075239e+00;
      rowData(5,14)= 2.594170453203594e+00;
      rowData(8,14)=-4.283826226985887e-01;
      rowData(11,14)=-1.181898588034305e+00;
      rowData(14,14)= 4.375931981001326e-01;
      rowData(3,15)=-4.388557102075239e+00;
      rowData(6,15)= 2.594170453203594e+00;
      rowData(7,15)=-9.394639233217816e-01;
      rowData(8,15)= 3.426478339951800e-01;
      rowData(9,15)=-4.283826226985887e-01;
      rowData(12,15)=-1.181898588034305e+00;
      rowData(15,15)= 4.375931981001326e-01;
      rowData(7,16)=-1.058007219612023e+00;
      rowData(8,17)=-1.058007220167134e+00;
      rowData(9,17)= 9.999999994736442e-01;
      rowData(1,18)= 3.485128058328331e+00;
      rowData(4,18)=-1.696575959032477e+00;
      rowData(7,18)= 2.248008562906697e+00;
      rowData(10,18)=-9.030388620418219e-01;
      rowData(13,18)=-1.445971307690058e+00;
      rowData(2,19)= 3.485128058328331e+00;
      rowData(5,19)=-1.696575959032477e+00;
      rowData(8,19)= 2.248008562906697e+00;
      rowData(11,19)=-9.030388620418219e-01;
      rowData(14,19)=-1.445971307690058e+00;
      rowData(3,20)= 3.485128058328331e+00;
      rowData(6,20)=-1.696575959032477e+00;
      rowData(9,20)= 2.248008562906697e+00;
      rowData(10,20)=-9.413249402179247e-01;
      rowData(11,20)=-3.375016460749336e-01;
      rowData(12,20)=-9.030388620418219e-01;
      rowData(15,20)=-1.445971307690058e+00;
      rowData(10,21)=-1.353685388227888e+00;
      rowData(11,22)=-1.353685388782999e+00;
      rowData(12,22)= 1.000000000028756e+00;
      rowData(1,23)=-4.206212111797146e+00;
      rowData(4,23)= 1.939331609862006e+00;
      rowData(7,23)=-2.061082623121687e+00;
      rowData(10,23)= 3.580690360072694e+00;
      rowData(13,23)=-4.377961988969181e+00;
      rowData(2,24)=-4.206212111797146e+00;
      rowData(5,24)= 1.939331609862006e+00;
      rowData(8,24)=-2.061082623121687e+00;
      rowData(11,24)= 3.580690360072694e+00;
      rowData(14,24)=-4.377961988969181e+00;
      rowData(3,25)=-4.206212111797146e+00;
      rowData(6,25)= 1.939331609862006e+00;
      rowData(9,25)=-2.061082623121687e+00;
      rowData(12,25)= 3.580690360072694e+00;
      rowData(13,25)=-7.863408257868798e-01;
      rowData(14,25)=-6.177929312567443e-01;
      rowData(15,25)=-4.377961988969181e+00;
      rowData(13,26)=-1.356474672586927e+00;
      rowData(14,27)=-1.356474673697150e+00;
      rowData(15,27)= 9.999999994736442e-01;
      rowData(1,28)= 2.500000000000000e+00;
      rowData(4,28)=-1.141317750102857e+00;
      rowData(7,28)= 1.172085778951930e+00;
      rowData(10,28)=-1.791534261250537e+00;
      rowData(13,28)= 5.519339620604482e+00;
      rowData(2,29)= 2.500000000000000e+00;
      rowData(5,29)=-1.141317750102857e+00;
      rowData(8,29)= 1.172085778951930e+00;
      rowData(11,29)=-1.791534261250537e+00;
      rowData(14,29)= 5.519339620604482e+00;
      rowData(3,30)= 2.500000000000000e+00;
      rowData(6,30)=-1.141317750102857e+00;
      rowData(9,30)= 1.172085778951930e+00;
      rowData(12,30)=-1.791534261250537e+00;
      rowData(15,30)= 5.519339620604482e+00;

      for (RSMatrix::const_iterator1 i1 = rowData.begin1(); i1 != rowData.end1(); ++i1) 
      {
         for (RSMatrix::const_iterator2 i2 = i1.begin(); i2 != i1.end(); ++i2)
         {
            jacArray(i2.index1() - 1, i2.index2() - 1) =  *i2;
         }    
      }
   };
   static std::vector<FunctionOutputData> GetUserDynFuncData() 
   {
      bool hasUserFunction = true, hasUserStateJacobian = true, 
           hasUserControlJacobian = true, hasUserTimeJacobian = true,
           isInitializing = true;
      Integer numData = 6, meshIdx, stageIdx = 0;
      IntegerArray controlIdx, stateIdx;
      controlIdx.resize(1);
      stateIdx.resize(1);
      Rmatrix stateJacobian(3,3), controlJacobian(3,2), timeJacobian(3,1);
      Rvector functionValues(3);

      std::vector<FunctionOutputData> userDynFuncData;
      userDynFuncData.resize(numData);
      userDynFuncData[0].SetIsInitializing(true);      
      userDynFuncData[0].SetNumFunctions(3);

      
      userDynFuncData[1] = userDynFuncData[0];
      userDynFuncData[2] = userDynFuncData[0];
      userDynFuncData[3] = userDynFuncData[0];
      userDynFuncData[4] = userDynFuncData[0];
      userDynFuncData[5] = userDynFuncData[0];
      

      meshIdx = 0; 
      stateIdx.resize(3); stateIdx[0] = 2; stateIdx[1] = 3; stateIdx[2] = 4;
      controlIdx.resize(2); controlIdx[0] = 5; controlIdx[1] = 6; 

      functionValues(0) = 0;
      functionValues(1) = 0;
      functionValues(2) = 1.500000000227480;

      stateJacobian.SetSize(3,3); 
      timeJacobian.SetSize(3,1); 
      controlJacobian.SetSize(3,2); 
      for (int idx1 = 0; idx1 < 3; ++idx1)
      {
         timeJacobian(idx1,0) = 0;
         for (int idx2 = 0; idx2 < 2; ++idx2)
         {
            stateJacobian(idx1,idx2) = 0;
            controlJacobian(idx1,idx2) = 0;
         }
         stateJacobian(idx1,2) = 0;
      }
      stateJacobian(0,2) = 2.026854473646720e-06;
      stateJacobian(1,2) = -1.000000000227480;
      controlJacobian(0,0) = 0;
      controlJacobian(1,1) = 0;
      controlJacobian(2,1) = -1.000000000583867;

      userDynFuncData[0].SetFunctions(functionValues);      
      userDynFuncData[0].SetControlJacobian(controlJacobian); 
      userDynFuncData[0].SetStateJacobian(stateJacobian); 
      userDynFuncData[0].SetTimeJacobian(timeJacobian); 
      userDynFuncData[0].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 1; 
      stateIdx.resize(3); stateIdx[0] = 7; stateIdx[1] = 8; stateIdx[2] = 9;
      controlIdx.resize(2); controlIdx[0] = 10; controlIdx[1] = 11; 

      functionValues(0) = 1.543300969523187e-01;
      functionValues(1) = -3.851737242587722e-01;
      functionValues(2) = 1.428259962644309e+00;

      stateJacobian(0,2) = 3.719320426642803e-01;
      stateJacobian(1,2) = -9.282599627846722e-01;
      controlJacobian(0,0) = 4.149416540921713e-01;
      controlJacobian(1,1) = 4.149416538146156e-01;
      controlJacobian(2,1) = -9.999999983634211e-01;

      userDynFuncData[1].SetFunctions(functionValues);      
      userDynFuncData[1].SetControlJacobian(controlJacobian); 
      userDynFuncData[1].SetStateJacobian(stateJacobian); 
      userDynFuncData[1].SetTimeJacobian(timeJacobian); 
      userDynFuncData[1].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 2; 
      stateIdx.resize(3); stateIdx[0] = 12; stateIdx[1] = 13; stateIdx[2] = 14;
      controlIdx.resize(2); controlIdx[0] = 15; controlIdx[1] = 16; 

      functionValues(0) = 9.939596132037488e-01;
      functionValues(1) = -3.625238819529065e-01;
      functionValues(2) = 8.426478336712460e-01;

      stateJacobian(0,2) = 9.394639233217816e-01;
      stateJacobian(1,2) = -3.426478339951800e-01;
      controlJacobian(0,0) = 1.058007219612023e+00;
      controlJacobian(1,1) = 1.058007220167134e+00;
      controlJacobian(2,1) = -9.999999994736442e-01;

      userDynFuncData[2].SetFunctions(functionValues);      
      userDynFuncData[2].SetControlJacobian(controlJacobian); 
      userDynFuncData[2].SetStateJacobian(stateJacobian); 
      userDynFuncData[2].SetTimeJacobian(timeJacobian); 
      userDynFuncData[2].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 3; 
      stateIdx.resize(3); stateIdx[0] = 17; stateIdx[1] = 18; stateIdx[2] = 19;
      controlIdx.resize(2); controlIdx[0] = 20; controlIdx[1] = 21; 

      functionValues(0) = 1.274257814357465e+00;
      functionValues(1) = 4.568710465164262e-01;
      functionValues(2) = 1.624983541497550e-01;

      stateJacobian(0,2) = 9.413249402179247e-01;
      stateJacobian(1,2) = 3.375016460749336e-01;
      controlJacobian(0,0) = 1.353685388227888e+00;
      controlJacobian(1,1) = 1.353685388782999e+00;
      controlJacobian(2,1) = -1.000000000028756e+00;

      userDynFuncData[3].SetFunctions(functionValues);      
      userDynFuncData[3].SetControlJacobian(controlJacobian); 
      userDynFuncData[3].SetStateJacobian(stateJacobian); 
      userDynFuncData[3].SetTimeJacobian(timeJacobian); 
      userDynFuncData[3].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 4; 
      stateIdx.resize(3); stateIdx[0] = 22; stateIdx[1] = 23; stateIdx[2] = 24;
      controlIdx.resize(2); controlIdx[0] = 25; controlIdx[1] = 26; 

      functionValues(0) = 1.066651415884305e+00;
      functionValues(1) = 8.380204646897436e-01;
      functionValues(2) = -1.177929308478450e-01;

      stateJacobian(0,2) = 7.863408257868798e-01;
      stateJacobian(1,2) = 6.177929312567443e-01;
      controlJacobian(0,0) = 1.356474672586927e+00;
      controlJacobian(1,1) = 1.356474673697150e+00;
      controlJacobian(2,1) = -9.999999994736442e-01;

      userDynFuncData[4].SetFunctions(functionValues);      
      userDynFuncData[4].SetControlJacobian(controlJacobian); 
      userDynFuncData[4].SetStateJacobian(stateJacobian); 
      userDynFuncData[4].SetTimeJacobian(timeJacobian); 
      userDynFuncData[4].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 5; 
      stateIdx.resize(3); stateIdx[0] = 27; stateIdx[1] = 28; stateIdx[2] = 29;
      controlIdx.resize(2); controlIdx[0] = 30; controlIdx[1] = 31; 

      functionValues(0) = 0;
      functionValues(1) = 0;
      functionValues(2) = 0;

      stateJacobian(0,2) = 0;
      stateJacobian(1,2) = 0;
      controlJacobian(0,0) = 0;
      controlJacobian(1,1) = 0;
      controlJacobian(2,1) = 0;

      userDynFuncData[5].SetFunctions(functionValues);      
      userDynFuncData[5].SetControlJacobian(controlJacobian); 
      userDynFuncData[5].SetStateJacobian(stateJacobian); 
      userDynFuncData[5].SetTimeJacobian(timeJacobian); 
      userDynFuncData[5].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      return userDynFuncData;
   };  
};

class CostFuncPropsHypSen  : public UserFunctionProperties
{
public:
   // class methods
   CostFuncPropsHypSen() {
      stateJacobianPattern.SetSize(1,1); 
      stateJacobianPattern(0,0) = 1  ;
      timeJacobianPattern.SetSize(1,1); 
      timeJacobianPattern(0,0) = 0  ;
      controlJacobianPattern.SetSize(1,1); 
      controlJacobianPattern(0,0) = 1  ;
      numFunctions = 1  ;
      hasStateVars = true ;
      hasControlVars = true ;
   };                           
};                    

class DecVecHypSen  : public DecVecTypeBetts
{
public:
   // class methods
   DecVecHypSen() {
      numParamsPerMesh = 7  ;
      numStateAndControlVars = 2  ;
      numStagePoints = 0  ;
      hasControlAtFinalMesh = false  ;
      numStateVars = 1  ;
      numControlVars = 1  ;
      numStateMeshPoints = 4  ;
      numControlMeshPoints = 3  ;
      numStateStagePoints = 0  ;
      numControlStagePoints = 0  ;
      numStatePoints = 4  ;
      numControlPoints = 3  ;
      numStatePointsPerMesh = 1  ;
      numControlPointsPerMesh = 1  ;
      numDecisionParams = 9  ;
      decisionVector.SetSize(9);
      decisionVector[0]=                 0;
      decisionVector[1]=50.000000000000000;
      decisionVector[2]= 1.000000000000000;
      decisionVector[3]= 0.950878358620457;
      decisionVector[4]=-0.030250049754212;
      decisionVector[5]=-0.052466292811721;
      decisionVector[6]= 0.041258252018226;
      decisionVector[7]= 0.085967528179871;
      decisionVector[8]= 1.000000000000000;             
      numStateParams = 4  ;
      numControlParams = 3  ;
      numIntegralParams = 0  ;
      numStaticParams = 0  ;
      integralStartIdx = 10 ;
      integralStopIdx = 9  ;
      timeStartIdx = 0  ;
      timeStopIdx = 1  ;
      staticStartIdx = 10 ;
      staticStopIdx = 9  ;    
   };                            
};  


class DynFuncPropsHypSen  : public UserFunctionProperties
{
public:
   // class methods
   DynFuncPropsHypSen() {
      stateJacobianPattern.SetSize(1,1); 
      stateJacobianPattern(0,0) = 1  ;
      timeJacobianPattern.SetSize(1,1); 
      timeJacobianPattern(0,0) = 0  ;
      controlJacobianPattern.SetSize(1,1); 
      controlJacobianPattern(0,0) = 1  ;
      numFunctions = 1  ;
      hasStateVars = true;
      hasControlVars = true;
   };                           
};                      

class ProblemCharacteristicsHypSen : public ProblemCharacteristics
{
public:
   // class methods
   ProblemCharacteristicsHypSen() {
         numStateVars = 1; 
         numControlVars = 1; 
         numIntegralVars = 0; 
         numStaticVars = 0; 
         numTimeVars = 2; 
         numStateVarsNLP = 4; 
         numTimeVarsNLP = 2; 
         numControlVarsNLP = 3; 
         numDecisionVarsNLP = 9; 
         numDefectConNLP = 3; 
         numTotalConNLP = 3; 
         hasStateVars = true; 
         hasControlVars = true; 
         hasIntegralVars = false; 
         hasStaticVars= false; 
         hasTimeVars= false; 
         hasDefectCons= true; 
         hasIntegralCons= false; 
         hasAlgPathCons= false; 
         hasIntegralCost= true; 
         hasAlgebraicCost= false; 
         meshIntervalFractions.SetSize(2);
         meshIntervalFractions(0) = -1;
         meshIntervalFractions(1) = 1;
         meshIntervalNumPoints.resize(1);
         meshIntervalNumPoints[0] = 3; 
         numMeshIntervals= 1; 
         stateLowerBound.SetSize(1);
         stateLowerBound(0)= -50; 
         stateUpperBound.SetSize(1);
         stateUpperBound(0) = 50;
         stateInitialGuess.SetSize(1);
         stateInitialGuess(0)= 1; 
         stateFinalGuess.SetSize(1);
         stateFinalGuess(0)= 1; 
         timeInitialGuess= 0; 
         timeFinalGuess= 50; 
         timeLowerBound= 0; 
         timeUpperBound= 50; 
         controlUpperBound.SetSize(1);
         controlUpperBound(0)= 50; 
         controlLowerBound.SetSize(1);
         controlLowerBound(0)= -50; 
   }   
};


class HypSenPathObject : public UserPathFunction
{
public:

   /// default constructor
   HypSenPathObject() {};
   /// default destructor
   ~HypSenPathObject() {};


   /// EvaluateFunctions
   void EvaluateFunctions()
   {

      Rvector stateVec = GetStateVector();
      Rvector controlVec = GetControlVector();
      Real    t = GetTime();
      Real    timeFac = 0.0;

      // @TODO add validation of vector size here!
      
      Rvector costFunc(1);
      costFunc(0) = 0.5*(stateVec(0)*stateVec(0) + controlVec(0)*controlVec(0)) + timeFac*t / 100.0;
      SetCostFunction(costFunc);

      Rvector dynFunc(1);
      
      dynFunc(0) = -stateVec(0)*stateVec(0)*stateVec(0) + controlVec(0) + timeFac*t*t*t / 100.0;
      SetDynFunctions(dynFunc);

   }

   /// EvaluateJacobians
   void EvaluateJacobians() {};


protected:
};


class TestNLPFuncUtilRadauCost
{
public:
   static void RunCostTests()
   {
       
      ProblemCharacteristicsHypSen *ptrConfig = new ProblemCharacteristicsHypSen();
      
      DecVecHypSen DecVector;
      DynFuncPropsHypSen dynFuncProps;

      CostFuncPropsHypSen costFuncProps;
          
      std::vector<FunctionOutputData*> ptrUserDynFuncData, ptrCostIntFuncData;

      std::vector<FunctionOutputData> userDynFuncData = GetUserDynFuncData();
      std::vector<FunctionOutputData> costIntFuncData = GetCostIntFuncData();

      for (Integer idx = 0; idx < userDynFuncData.size(); ++idx)
      {
         ptrUserDynFuncData.push_back(&userDynFuncData[idx]);
         ptrCostIntFuncData.push_back(&costIntFuncData[idx]);
      }

      NLPFuncUtilRadau trans;

      trans.Initialize(ptrConfig);
      
      trans.SetTimeVector(0,50);

      trans.PrepareToOptimize(dynFuncProps, ptrUserDynFuncData,
						  costFuncProps, 
						  ptrCostIntFuncData);

      Rvector defectConVec, costIntFuncVec;
      RSMatrix defectConJacobian, costIntJacobian;
      /*
      trans.ComputeDefectFunAndJac(defectConVec, defectConJacobian,&userDynFuncData,&DecVector);
 

      std::stringstream dataLine("");
      dataLine << "The resultant defect constraint function values are ";

      dataLine <<defectConVec(0) << " ,";
      dataLine <<defectConVec(1) << " ,";
      dataLine <<defectConVec(2) << " ,";


      std::cout << dataLine.str() << std::endl;

      SparseMatrixUtil::PrintNonZeroElements(&defectConJacobian);
      */
      trans.ComputeCostFunAndJac(ptrCostIntFuncData, &DecVector, costIntFuncVec, costIntJacobian);
      
      std::stringstream dataLine2("");
      std::cout << "The resultant cost integral function error is:     "
                << costIntFuncVec(0) - 5.421914864941591 << std::endl;

      RSMatrix truthData(1,9);
      truthData(0,0)=-0.108438297298832;
      truthData(0,1)= 0.108438297298832;
      truthData(0,2)= 5.555555555555555;
      truthData(0,3)= 5.282657547891429;
      truthData(0,4)=-0.775136087026420;
      truthData(0,5)=-1.344411570932912;
      truthData(0,6)= 0.776486621066409;
      truthData(0,7)= 1.617922044984608;




      RSMatrix errorMat = costIntJacobian - truthData;
      std::cout << "The resultant defect cost Jacobian errors are:      " 
                << SparseMatrixUtil::GetAbsTotalSum(&errorMat) << std::endl;


      
      // test assign, copy constructor operators
      NLPFuncUtilRadau trans2;
      trans2 = trans;      
      NLPFuncUtilRadau trans3 = trans2;

      // test for execution speed!
      clock_t total_start_time, total_end_time, start_time, end_time;
      start_time = clock();
      total_start_time = start_time;
      int numRepetition = 100;
      for (int idx = 0; idx < numRepetition; ++idx)
      {
         trans3.ComputeCostFunAndJac(ptrCostIntFuncData, &DecVector, costIntFuncVec, costIntJacobian);
      }
      end_time = clock();

      printf("Time consumption for %d cost function and jacobian computation is: %f.", 
             numRepetition, ((double)(end_time - start_time))/CLOCKS_PER_SEC);

      std::cout << "\n" << std::endl;

      /* settings for mesh refinements
      HypSenPathObject *ptrUserPathFunction = new HypSenPathObject();
      FunctionInputData *ptrFunctionInputData = new FunctionInputData();
      ptrFunctionInputData->Initialize(3, 2);
      PathFunctionContainer *ptrPathFunctionContainer = new PathFunctionContainer();
      ptrPathFunctionContainer->Initialize();

      BoundData *ptrBoundData = new BoundData();
      ptrBoundData->SetStateLowerBound(ptrConfig->GetStateLowerBound());
      ptrBoundData->SetStateUpperBound(ptrConfig->GetStateUpperBound());
      ptrBoundData->SetControlLowerBound(ptrConfig->GetControlLowerBound());
      ptrBoundData->SetControlUpperBound(ptrConfig->GetControlUpperBound());
      Rvector timeUpper(1, ptrConfig->GetTimeUpperBound());
      Rvector timeLower(1, ptrConfig->GetTimeLowerBound());
      ptrBoundData->SetTimeUpperBound(timeUpper);
      ptrBoundData->SetTimeLowerBound(timeLower);

      UserPathFunctionManager *ptrUserPathFunctionManager = new UserPathFunctionManager();
      ptrUserPathFunctionManager->Initialize(ptrUserPathFunction, ptrFunctionInputData, ptrPathFunctionContainer, ptrBoundData);
      */

   };
   static std::vector<FunctionOutputData> GetUserDynFuncData() {
      bool hasUserFunction = true, hasUserStateJacobian = true, 
           hasUserControlJacobian = true, hasUserTimeJacobian = true,
           isInitializing = true;
      Integer numData = 4, meshIdx, stageIdx = 0;
      IntegerArray controlIdx, stateIdx;
      controlIdx.resize(1);
      stateIdx.resize(1);
      Rmatrix stateJacobian(1,1), controlJacobian(1,1), timeJacobian(1,1);
      Rvector functionValues(1);

      std::vector<FunctionOutputData> userDynFuncData;
      userDynFuncData.resize(numData);
      userDynFuncData[0].SetIsInitializing(true);      
      userDynFuncData[0].SetNumFunctions(1);

      
      userDynFuncData[1] = userDynFuncData[0];
      userDynFuncData[2] = userDynFuncData[0];
      userDynFuncData[3] = userDynFuncData[0];
      

      meshIdx = 0; 
      stateIdx.resize(1); stateIdx[0] = 2;
      controlIdx.resize(1); controlIdx[0] = 3; 

      functionValues(0) = -0.049121641379543;
      stateJacobian(0,0) = -3; controlJacobian(0,0) = 1; timeJacobian(0,0) = 0;
      userDynFuncData[0].SetFunctions(functionValues);      
      userDynFuncData[0].SetControlJacobian(controlJacobian); 
      userDynFuncData[0].SetStateJacobian(stateJacobian); 
      userDynFuncData[0].SetTimeJacobian(timeJacobian); 
      userDynFuncData[0].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 1; 
      stateIdx.resize(1); stateIdx[0] = 4;      
      controlIdx.resize(1); controlIdx[0] = 5; 

      functionValues(0) = -0.052438612034511;
      stateJacobian(0,0) = -0.002745196530397; controlJacobian(0,0) = 1; timeJacobian(0,0) = 0;
      userDynFuncData[1].SetFunctions(functionValues);      
      userDynFuncData[1].SetControlJacobian(controlJacobian); 
      userDynFuncData[1].SetStateJacobian(stateJacobian); 
      userDynFuncData[1].SetTimeJacobian(timeJacobian); 
      userDynFuncData[1].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);
      
      meshIdx = 2; 
      stateIdx.resize(1); stateIdx[0] = 6;      
      controlIdx.resize(1); controlIdx[0] = 7; 
      functionValues(0) = 0.085897296594344;
      stateJacobian(0,0) = -0.005106730078798; controlJacobian(0,0) = 1; timeJacobian(0,0) = 0;
      userDynFuncData[2].SetFunctions(functionValues);      
      userDynFuncData[2].SetControlJacobian(controlJacobian); 
      userDynFuncData[2].SetStateJacobian(stateJacobian); 
      userDynFuncData[2].SetTimeJacobian(timeJacobian); 
      userDynFuncData[2].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 3; 
      stateIdx.resize(1); stateIdx[0] = 8;
      controlIdx.resize(1); controlIdx[0] = 9; 

      functionValues(0) = -1000000000000;
      stateJacobian(0,0) = -3; controlJacobian(0,0) = 1; timeJacobian(0,0) = 0;
      userDynFuncData[3].SetFunctions(functionValues);      
      userDynFuncData[3].SetControlJacobian(controlJacobian); 
      userDynFuncData[3].SetStateJacobian(stateJacobian); 
      userDynFuncData[3].SetTimeJacobian(timeJacobian); 
      userDynFuncData[3].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      return userDynFuncData;
   };
   static std::vector<FunctionOutputData> GetCostIntFuncData() {
      bool hasUserFunction = true, hasUserStateJacobian = true, 
           hasUserControlJacobian = true, hasUserTimeJacobian = true,
           isInitializing = true;
      Integer numData = 4, meshIdx, stageIdx = 0;
      IntegerArray controlIdx, stateIdx;
      controlIdx.resize(1);
      stateIdx.resize(1);
      Rmatrix stateJacobian(1,1), controlJacobian(1,1), timeJacobian(1,1);
      Rvector functionValues(1);

      std::vector<FunctionOutputData> userDynFuncData;
      userDynFuncData.resize(numData);
      userDynFuncData[0].SetIsInitializing(true);      
      userDynFuncData[0].SetNumFunctions(1);

      
      userDynFuncData[1] = userDynFuncData[0];
      userDynFuncData[2] = userDynFuncData[0];
      userDynFuncData[3] = userDynFuncData[0];
      

      meshIdx = 0; 
      stateIdx.resize(1); stateIdx[0] = 2;
      controlIdx.resize(1); controlIdx[0] = 3; 

      functionValues(0) = 0.952084826446367;
      stateJacobian(0,0) = 1; controlJacobian(0 ,0) = 0.950878358620457; timeJacobian(0,0) = 0;
      userDynFuncData[0].SetFunctions(functionValues);      
      userDynFuncData[0].SetControlJacobian(controlJacobian); 
      userDynFuncData[0].SetStateJacobian(stateJacobian); 
      userDynFuncData[0].SetTimeJacobian(timeJacobian); 
      userDynFuncData[0].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 1; 
      stateIdx.resize(1); stateIdx[0] = 4;      
      controlIdx.resize(1); controlIdx[0] = 5; 

      functionValues(0) = 0.001833888695769;
      stateJacobian(0,0) = -0.030250049754212; controlJacobian(0,0) = -0.052466292811721; timeJacobian(0,0) = 0;
      userDynFuncData[1].SetFunctions(functionValues);      
      userDynFuncData[1].SetControlJacobian(controlJacobian); 
      userDynFuncData[1].SetStateJacobian(stateJacobian); 
      userDynFuncData[1].SetTimeJacobian(timeJacobian); 
      userDynFuncData[1].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);
      
      meshIdx = 2; 
      stateIdx.resize(1); stateIdx[0] = 6;      
      controlIdx.resize(1); controlIdx[0] = 7; 
      functionValues(0) = 0.004546329630478;
      stateJacobian(0,0) = 0.041258252018226; controlJacobian(0,0) = 0.085967528179871; timeJacobian(0,0) = 0;
      userDynFuncData[2].SetFunctions(functionValues);      
      userDynFuncData[2].SetControlJacobian(controlJacobian); 
      userDynFuncData[2].SetStateJacobian(stateJacobian); 
      userDynFuncData[2].SetTimeJacobian(timeJacobian); 
      userDynFuncData[2].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      meshIdx = 3; 
      stateIdx.resize(1); stateIdx[0] = 8;
      controlIdx.resize(1); controlIdx[0] = 9; 

      functionValues(0) = -1000000000000;
      stateJacobian(0,0) = 1; controlJacobian(0,0) = -1000000000000; timeJacobian(0,0) = 0;
      userDynFuncData[3].SetFunctions(functionValues);      
      userDynFuncData[3].SetControlJacobian(controlJacobian); 
      userDynFuncData[3].SetStateJacobian(stateJacobian); 
      userDynFuncData[3].SetTimeJacobian(timeJacobian); 
      userDynFuncData[3].SetNLPData(meshIdx, stageIdx,stateIdx,controlIdx);

      return userDynFuncData;
   };  
};
