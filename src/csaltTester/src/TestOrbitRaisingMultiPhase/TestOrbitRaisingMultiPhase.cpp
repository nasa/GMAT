//------------------------------------------------------------------------------
//                         TestTrajectory
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
// Created: 2016.09.21
//
/**
 * Test driver for Trajectory, etc. classes.
 */
//------------------------------------------------------------------------------
// global igrid iGfun jGvar traj
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
#include "ImplicitRKPhase.hpp"
#include "Trajectory.hpp"
//#include "Phase.hpp"
#include "DecVecTypeBetts.hpp"
#include "GuessGenerator.hpp"
#include "OrbitRaisingMultiPhasePointObject.hpp"
#include "OrbitRaisingMultiPhasePathObject.hpp"
#include "OrbitRaisingPointObject.hpp"
#include "OrbitRaisingPathObject.hpp"
#include "boost/config.hpp"  // BOOST
#include <iostream>
#include <stack>
#include <ctime>

std::stack<clock_t> tictoc_stack;

void tic() {
   tictoc_stack.push(clock());
}

void toc() {
   std::cout << "Time elapsed: "
      << ((double)(clock() - tictoc_stack.top())) / CLOCKS_PER_SEC
      << std::endl;
   tictoc_stack.pop();
}

static const Real INF = std::numeric_limits<Real>::infinity();

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
   tic();

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
      // ==========================================================================
      // =====  Define Properties for the Trajectory
      // Create trajectory and configure user function names
      Trajectory *traj                = new Trajectory();
      // Create path and point objects
      MessageInterface::ShowMessage("*** TEST *** creating path and point objects\n");
      OrbitRaisingMultiPhasePathObject *pathFunctionObject  =
                                       new OrbitRaisingMultiPhasePathObject();
      OrbitRaisingMultiPhasePointObject *pointFunctionObject  =
                                        new OrbitRaisingMultiPhasePointObject();
      
      // not doing plotting for now
      //std::string plotFunctionName   = "OrbitRaisingMultiPhasePlotFunction";
      //bool    showPlot               = false;
      //Integer plotUpdateRate         = 3;
      Real    costLowerBound         = -INF;
      Real    costUpperBound         = INF;
      Integer maxMeshRefinementCount = 10;

      // Create trajectory
      MessageInterface::ShowMessage("*** TEST *** creating trajectory\n");
      traj->SetUserPathFunction(pathFunctionObject);
      traj->SetUserPointFunction(pointFunctionObject);
      traj->SetCostLowerBound(costLowerBound);
      traj->SetCostUpperBound(costUpperBound);
      traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);

      // ==========================================================================
      // =====  Define Properties for Phases
      // ==========================================================================
      // Create phases
      MessageInterface::ShowMessage("*** TEST *** creating phases\n");

      //ImplicitRKPhase *phase1           = new ImplicitRKPhase();
      ImplicitRKPhase *phase1 = new ImplicitRKPhase();
      phase1->SetTranscription("RungeKutta8");

      std::string initialGuessMode = "LinearNoControl";
      //Rvector meshIntervalFractions(5, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0);
      Rvector meshIntervalFractions(2, 0.0, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(10);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints.push_back(10);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints.push_back(10);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints.push_back(10);   // was - 50*ones(1,1) - commented out

      // Set time properties
      Real timeLowerBound   = 0.0;
      Real timeUpperBound   = 3.32;
      Real initialGuessTime = 0.0;
      Real finalGuessTime   = 1.0;
      // Set state properties
      Integer numStateVars              = 5;
      Rvector stateLowerBound(5, 0.5, 0.0,   -10.0, -10.0, 0.1);
      Rvector stateUpperBound(5, 5.0, 4.0*PI, 10.0,  10.0, 3.0);
      Rvector initialGuessState(5, 1.0, 0.0, 0.0, 1.0, 1.0); // [r0 theta0 vr0 vtheta0 m0]
      Rvector finalGuessState(5,   1.0, 0.5, 0.0, 1.0, 1.0); // [rf thetaf vrf vthetaf mf]
      // Set control properties
      Integer numControlVars  = 2;
      Rvector controlUpperBound(2,  10.0,  10.0);
      Rvector controlLowerBound(2, -10.0, -10.0);

      phase1->SetInitialGuessMode(initialGuessMode);

      phase1->SetNumStateVars(numStateVars);
      phase1->SetNumControlVars(numControlVars);
      phase1->SetMeshIntervalFractions(meshIntervalFractions);
      phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints);
      phase1->SetStateLowerBound(stateLowerBound);
      phase1->SetStateUpperBound(stateUpperBound);
      phase1->SetStateInitialGuess(initialGuessState);
      phase1->SetStateFinalGuess(finalGuessState);
      phase1->SetTimeLowerBound(timeLowerBound);
      phase1->SetTimeUpperBound(timeUpperBound);
      phase1->SetTimeInitialGuess(initialGuessTime);
      phase1->SetTimeFinalGuess(finalGuessTime);
      phase1->SetControlLowerBound(controlLowerBound);
      phase1->SetControlUpperBound(controlUpperBound);

      ImplicitRKPhase *phase2        = new ImplicitRKPhase();
      phase2->SetTranscription("RungeKutta8");
      phase2->SetInitialGuessMode("LinearUnityControl");
      //Rvector meshIntervalFractions2(7, -1.0, -0.65, -0.3, 0.0, 0.3, 0.65, 1.0);
      Rvector meshIntervalFractions2(2, 0.0, 1.0);
      //%phase2.meshIntervalNumPoints     = 50*ones(1,1);
      IntegerArray meshIntervalNumPoints2;
      meshIntervalNumPoints2.push_back(10);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
      //meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
//      meshIntervalNumPoints     = 7*ones(1,1);
      // Set time properties
      Real timeLowerBound2   = 0.0;
      Real timeUpperBound2   = 3.32;
      Real initialGuessTime2 = 0.0;
      Real finalGuessTime2   = 1.0;
      // Set state properties
      Integer numStateVars2              = 5;
      Rvector stateLowerBound2(5, 0.5, 0.0,   -10.0, -10.0, 0.1);
      Rvector stateUpperBound2(5, 5.0, 4.0*PI, 10.0,  10.0, 3.0);
      Rvector initialGuessState2(5, 1.0, 0.5, 0.0, 1.0, 1.0); // [r0 theta0 vr0 vtheta0 m0]
      Rvector finalGuessState2(5,   1.0, PI,  0.0, 1.0, 1.0); // [rf thetaf vrf vthetaf mf]
      // Set control properties
      Integer numControlVars2            = 2;
      Rvector controlUpperBound2(2,  10.0,  10.0);
      Rvector controlLowerBound2(2, -10.0, -10.0);

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

      // =========================================================================
      // =====  Define the linkage configuration and optimize
      // =========================================================================

      // Add phases to Trajectory
      MessageInterface::ShowMessage("*** TEST *** adding Phase list to Trajectory\n");

      std::vector<Phase*> pList;
      pList.push_back(phase1);
      pList.push_back(phase2);
      traj->SetPhaseList(pList);
      
      MessageInterface::ShowMessage("*** TEST *** initializing the Trajectory\n");
      // Initialize the Trajectory
      traj->Initialize();
      /* deactivate the old code. YK
      Rvector decVec(112,
                     0.0000000000,1.0000000000,1.0000000000,0.0000000000,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.0714285714,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.1428571429,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.2142857143,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.2857142857,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.3571428571,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.4285714286,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.5000000000,0.0000000000,1.0000000000,1.0000000000,
                     0.0000000000,1.0000000000,1.0000000000,0.5000000000,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,0.8773703791,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,1.2547407582,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,1.6321111373,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,2.0094815163,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,2.3868518954,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,2.7642222745,0.0000000000,1.0000000000,1.0000000000,
                     1.0000000000,1.0000000000,1.0000000000,3.1415926536,0.0000000000,1.0000000000,1.0000000000);
      */
      Rvector  decVec = traj->GetDecisionVector();
      bool reDim  = true;
      bool nonDim = true;

      MessageInterface::ShowMessage("*** TEST *** setting decision vector on Trajectory\n");
//      traj->SetDecisionVector(decVec);   // ,reDim);
      Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);
      RSMatrix J        = traj->GetJacobian(); // nonDim);

      Rvector dvP1 = phase1->GetDecVector();
      Rvector dvP2 = phase2->GetDecVector();
      MessageInterface::ShowMessage("*** TEST *** dvP1 (Before optimization):\n%s\n",
                                    dvP1.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** dvP2 (Before optimization):\n%s\n",
                                    dvP2.ToString(12).c_str());

      MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");
      
      RSMatrix conSp    = phase1->GetConSparsityPattern();

// ------------------ Optimizing -----------------------------------------------
// TRY it first without optimizing
      Rvector z = decVec;
      Rvector F(C.GetSize());
      Rvector xmul(decVec.GetSize());
      Rvector Fmul(C.GetSize());
      MessageInterface::ShowMessage("*** TEST *** Optimizing!!\n");
      traj->Optimize(z, F, xmul, Fmul);
      MessageInterface::ShowMessage("*** TEST *** Optimization complete!!\n");     
      cout << endl;
      
      toc();
      
      cout << "Hit enter to end" << endl;
      cin.get();

      MessageInterface::ShowMessage("*** END TEST ***\n");
      


   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}
