//------------------------------------------------------------------------------
//                               TestUserPointFunction
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
// Author: Wendy Shoan
// Created: 2015.06.30
//
/**
 * Test driver for UserPointFunction classes.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
//#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"
#include "TimeTypes.hpp"
#include "UserPointFunction.hpp"
#include "UserPointFunction.hpp"
#include "FunctionOutputData.hpp"
#include "PathFunctionContainer.hpp"
#include "PointFunctionContainer.hpp"
#include "UserFunctionManager.hpp"
#include "UserPointFunctionManager.hpp"
#include "DummyPathFunction.hpp"
#include "DummyPathFunction2.hpp"
#include "UserFunctionProperties.hpp"
#include "BoundData.hpp"
#include "ProblemCharacteristics.hpp"
#include "RadauPhase.hpp"
//#include "Phase.hpp"
#include "DecVecTypeBetts.hpp"
#include "GuessGenerator.hpp"
#include "OrbitRaisingMultiPhasePointObject.hpp"
#include "OrbitRaisingPointObject.hpp"
#include "OrbitRaisingPathObject.hpp"
#include "boost/config.hpp"  // BOOST

//#include "HypSenPathFunction.hpp"

using namespace std;
using namespace GmatMathConstants;

Real GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ii++)
      if (vec(ii) > max) max = vec(ii);
   return max;
}

Real GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ii++)
      for (Integer jj = 0; jj < c; jj++)
         if (mat(ii,jj) > max) max = mat(ii,jj);
   return max;
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outFormat = "%16.9f ";
   Real        tolerance = 1e-15;
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");

   try
   {
      
      // ---------------------------------------------------------------------
      // Orbit Raising MultiPhase specific set-up starts here
      // ---------------------------------------------------------------------
      RadauPhase *phase1 = new RadauPhase();
      phase1->SetInitialGuessMode("LinearUnityControl");
      Rvector meshIntervalFractions1(2, -1.0, 1.0);
      IntegerArray meshIntervalNumPoints1;
      meshIntervalNumPoints1.push_back(7);
//      IntegerArray meshIntervalNumPoints1;
//      for (Integer ii = 0; ii < 7; ii++)
//         meshIntervalNumPoints1.push_back(1);

      OrbitRaisingPathObject *pathFuncObj = new OrbitRaisingPathObject();
      // Set time properties
      Real timeLowerBound1   = 0.0;
      Real initialGuessTime1 = 0.0;
      Real finalGuessTime1   = 1.0;
      Real timeUpperBound1   = 3.32;
      // Set state properties
      Integer numStateVars1  = 5;
      Rvector stateLowerBound1(5, 0.5, 0.0, -10.0, -10.0, 0.1);
      Rvector initialGuessState1(5, 1.0, 0.0, 0.0,  1.0, 1.0); // [r0 theta0 vr0 vtheta0 m0]
      Rvector finalGuessState1(5, 1.0, PI, 0.0, 0.5, 0.5); // [rf thetaf vrf vthetaf mf]
      Rvector stateUpperBound1(5, 5.0, 4.0*PI, 10.0, 10.0, 3.0);
      // Set control properties
      Integer numControlVars1 = 2;
      Rvector controlUpperBound1(2,  10.0,  10.0);
      Rvector controlLowerBound1(2, -10.0, -10.0);

      RadauPhase *phase2  = new RadauPhase();
      phase2->SetInitialGuessMode("LinearUnityControl");
      Rvector meshIntervalFractions2(2, -1.0, 1.0);
      //  %phase2.meshIntervalNumPoints     = 50*ones(1,1);
      IntegerArray meshIntervalNumPoints2;
      meshIntervalNumPoints2.push_back(7);
//      for (Integer ii = 0; ii < 7; ii++)
//         meshIntervalNumPoints2.push_back(1);

      // Set time properties
      Real timeLowerBound2   = 0.0;
      Real initialGuessTime2 = 0.0;
      Real finalGuessTime2   = 1.0;
      Real timeUpperBound2   = 3.32;
      // Set state properties
      Integer numStateVars2  = 5;
      Rvector stateLowerBound2(5, 0.5, 0.0, -10.0, -10.0, 0.1);
      Rvector initialGuessState2(5, 1.0, 0.5, 0.0, 1.0, 1.0); // [r0 theta0 vr0 vtheta0 m0]
      Rvector finalGuessState2(5, 1.0, PI, 0.0, 1.0, 1.0); // [rf thetaf vrf vthetaf mf]
      Rvector stateUpperBound2(5, 5.0, 4.0*PI, 10.0, 10.0, 3.0);
      // Set control properties
      Integer numControlVars2 = 2;
      Rvector controlUpperBound2(2, 10.0, 10.0);
      Rvector controlLowerBound2(2, -10.0, -10.0);
      // ---------------------------------------------------------------------
      // Orbit Raising Path specific set-up ends here
      // ---------------------------------------------------------------------

      // Test the PathFunctionContainer
      MessageInterface::ShowMessage("*** TEST*** creating and initializing a RadauPhase\n");
      
      // Configure the phase according to setup
      phase1->SetNumStateVars(numStateVars1);
      phase1->SetNumControlVars(numControlVars1);
      phase1->SetMeshIntervalFractions(meshIntervalFractions1);
      phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints1);
      phase1->SetStateLowerBound(stateLowerBound1);
      phase1->SetStateUpperBound(stateUpperBound1);
      phase1->SetStateInitialGuess(initialGuessState1);
      phase1->SetStateFinalGuess(finalGuessState1);
      phase1->SetTimeLowerBound(timeLowerBound1);
      phase1->SetTimeUpperBound(timeUpperBound1);
      phase1->SetTimeInitialGuess(initialGuessTime1);
      phase1->SetTimeFinalGuess(finalGuessTime1);
      phase1->SetControlLowerBound(controlLowerBound1);
      phase1->SetControlUpperBound(controlUpperBound1);
   
      // Set phase number, this is done by trajectory
      phase1->SetPhaseNumber(0);
      phase1->SetPathFunction(pathFuncObj);
      
      MessageInterface::ShowMessage("*** TEST*** creating and initializing a second RadauPhase\n");
      
      // Configure the phase according to setup
      phase2->SetNumStateVars(numStateVars2);
      phase2->SetNumControlVars(numControlVars2);
      phase2->SetMeshIntervalFractions(meshIntervalFractions2);
      phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints2);
      phase2->SetStateLowerBound(stateLowerBound2);
      phase2->SetStateUpperBound(stateUpperBound2);
      phase2->SetStateInitialGuess(initialGuessState2);
      phase2->SetStateFinalGuess(finalGuessState2);
      phase2->SetTimeLowerBound(timeLowerBound2);
      phase2->SetTimeUpperBound(timeUpperBound2);
      phase2->SetTimeInitialGuess(initialGuessTime2);
      phase2->SetTimeFinalGuess(finalGuessTime2);
      phase2->SetControlLowerBound(controlLowerBound2);
      phase2->SetControlUpperBound(controlUpperBound2);
   
      // Set phase number, this is done by trajectory
      phase2->SetPhaseNumber(1);
      phase2->SetPathFunction(pathFuncObj); // do we need a separate function here??
   
   
      // This is code from Trajectory.InitializePhases()
   
      //==========================================================================
      // =====  Testing starts here
      //==========================================================================
      MessageInterface::ShowMessage("*** TEST*** initializing the first Phase ...\n");
      
      phase1->Initialize();
      MessageInterface::ShowMessage("*** TEST*** initializing the second Phase ...\n");
      phase2->Initialize();
   
      MessageInterface::ShowMessage("*** TEST*** creating the userPointFunction\n");

      OrbitRaisingMultiPhasePointObject *userPointFunction = 
            new OrbitRaisingMultiPhasePointObject();
      UserPointFunctionManager *pointFuncManager = new UserPointFunctionManager();
      Integer      totalnumDecisionParams = 112;
      IntegerArray decVecStartIdx;
      decVecStartIdx.push_back(0);
      decVecStartIdx.push_back(56);
      
      MessageInterface::ShowMessage("*** TEST *** decVecStartIdx = \n");
      for (Integer ii = 0; ii < decVecStartIdx.size(); ii++)
         MessageInterface::ShowMessage("   (%d) =  %d\n", ii, decVecStartIdx.at(ii));
      
      MessageInterface::ShowMessage("*** TEST*** setting up the Phase list\n");
      if (!userPointFunction)
         MessageInterface::ShowMessage("Uh-oh! userPointFunction is NULL!!\n");

      std::vector<Phase*> plist;
      plist.push_back(phase1);
      plist.push_back(phase2);
      MessageInterface::ShowMessage("*** TEST*** initializing the Point Function Manager\n");
      pointFuncManager->Initialize(userPointFunction,plist,
                                   totalnumDecisionParams,decVecStartIdx);
   
      Integer numBF = pointFuncManager->GetNumberBoundaryFunctions();
   
      MessageInterface::ShowMessage("*** TEST*** Number of boundary functions = %d\n", numBF);
      MessageInterface::ShowMessage("*** TEST*** evaluating user jacobian\n");
      
      // Runs up to here so far  WCS 2016.07.25

      pointFuncManager->EvaluateUserJacobian();
   
      MessageInterface::ShowMessage("*** TEST*** EvaluateUserJacobian is finished\n");

      if (pointFuncManager->HasBoundaryFunctions())
      {
         MessageInterface::ShowMessage("*** TEST*** calling ComputeBoundNLPJacobian\n");
         RSMatrix bnlp = pointFuncManager->ComputeBoundNLPJacobian();
         
         MessageInterface::ShowMessage("bound NLP Jacobian (in MATLAB indexes and order!):\n");
         Integer rJac = bnlp.size1();
         Integer cJac = bnlp.size2();
         for (Integer cc = 0; cc < cJac; cc++)
         {
            for (Integer rr = 0; rr < rJac; rr++)
            {
               Real jacTmp = bnlp(rr,cc);
               if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
            }
         }
      }
   
      if (pointFuncManager->HasCostFunction())
      {
         MessageInterface::ShowMessage("*** TEST*** calling ComputeCostNLPJacobian\n");
         RSMatrix cnlp = pointFuncManager->ComputeCostNLPJacobian();
         
         MessageInterface::ShowMessage("cost NLP Jacobian (in MATLAB indexes and order!):\n");
         Integer rJac = cnlp.size1();
         Integer cJac = cnlp.size2();
         for (Integer cc = 0; cc < cJac; cc++)
         {
            for (Integer rr = 0; rr < rJac; rr++)
            {
               Real jacTmp = cnlp(rr,cc);
               if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
            }
         }
      }
   
      if (pointFuncManager->HasBoundaryFunctions())
      {
         MessageInterface::ShowMessage("*** TEST*** calling ComputeBoundNLPFunctions\n");
         Rvector nlpFunc = pointFuncManager->ComputeBoundNLPFunctions();
         MessageInterface::ShowMessage("--- nlpFunc = %s\n", nlpFunc.ToString(12).c_str());
      }
   
      MessageInterface::ShowMessage("*** TEST*** getting bounds and sparsity data\n");

      Rvector conLower       = pointFuncManager->GetConLowerBound();
      Rvector conUpper       = pointFuncManager->GetConUpperBound();
      RSMatrix* boundSparsity = pointFuncManager->ComputeBoundNLPSparsityPattern(); // YK mod; change it to use pointer instead of RSMatrix
      RSMatrix* costSparsity  = pointFuncManager->ComputeCostNLPSparsityPattern();
      
      
      // Now write stuff out TBD
      MessageInterface::ShowMessage("--- conLower = %s\n", conLower.ToString(12).c_str());
      MessageInterface::ShowMessage("--- conUpper = %s\n", conUpper.ToString(12).c_str());

      MessageInterface::ShowMessage("boundSparsity (in MATLAB indexes and order!):\n");
      Integer rJac = boundSparsity->size1();
      Integer cJac = boundSparsity->size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = (*boundSparsity)(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
         }
      }
      
      MessageInterface::ShowMessage("costSparsity (in MATLAB indexes and order!):\n");
      rJac = costSparsity->size1();
      cJac = costSparsity->size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = (*costSparsity)(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
         }
      }
      

   
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();

      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }


   // TODO: why don't these deletes work? - cause segmentation violation
//   if (buffer) delete buffer;
//   if (pfc)    delete pfc;
//   if (fid)    delete fid;
//   if (bd)     delete bd;
//   if (dpf)    delete dpf;
//   if (pfRes)  delete pf2;

//   if (cost)   delete cost;
//   if (dyn)    delete dyn;
//   if (alg)    delete alg;
}

