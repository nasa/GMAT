//------------------------------------------------------------------------------
//                           TestGuessDriver
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 22, 2017
/**
 * Driver code for the Schwartz test problem.
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
#include "FunctionOutputData.hpp"
#include "PathFunctionContainer.hpp"
#include "PointFunctionContainer.hpp"
#include "UserFunctionManager.hpp"
#include "UserPointFunctionManager.hpp"
#include "UserFunctionProperties.hpp"
#include "RadauPhase.hpp"
#include "Trajectory.hpp"
//#include "Phase.hpp"
#include "SchwartzPointObject.hpp"
#include "SchwartzPathObject.hpp"
#include "boost/config.hpp"  // BOOST
#include "ExampleUserGuessClass.hpp"

#include "TestGuessDriver.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

//using namespace GmatMathConstants;


TestGuessDriver::TestGuessDriver()
{
}


TestGuessDriver::~TestGuessDriver()
{
}


Real TestGuessDriver::GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ++ii)
      if (vec(ii) > max)
         max = vec(ii);
   return max;
}


Real TestGuessDriver::GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ++ii)
      for (Integer jj = 0; jj < c; ++jj)
         if (mat(ii,jj) > max)
            max = mat(ii,jj);

   return max;
}



/**
 * Test problem driver code
 */
int TestGuessDriver::Run()
{
   std::string outFormat = "%16.9f ";

   // ********** FROM HERE *******************
   // Shouldn't this be done in main()?
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());

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

   // *********** TO HERE ********************


   MessageInterface::ShowMessage("*** START TEST ***\n");
   MessageInterface::ShowMessage("*** TESTing Schwartz optimization problem ***\n");

   // Create trajectory and configure user function names
   Trajectory *traj                = new Trajectory();

   // Create path and point objects
   MessageInterface::ShowMessage("*** TEST *** creating path and point objects\n");
   SchwartzPathObject *pathFunctionObject   = new SchwartzPathObject();
   SchwartzPointObject *pointFunctionObject = new SchwartzPointObject();

   Real    costLowerBound         = -INF;
   Real    costUpperBound         =  INF;
   Integer maxMeshRefinementCount = 25; // Change to 25 once working

   // Configure core trajectory settings
   MessageInterface::ShowMessage("*** TEST *** configuring the trajectory\n");
   traj->SetUserPathFunction(pathFunctionObject);
   traj->SetUserPointFunction(pointFunctionObject);
   traj->SetCostLowerBound(costLowerBound);
   traj->SetCostUpperBound(costUpperBound);
   traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);

   // ==========================================================================
   // =====  Define Properties for Phases
   // ==========================================================================
   // Create phases
   MessageInterface::ShowMessage("*** Creating the first phase\n");

   RadauPhase *phase1           = new RadauPhase();
//    std::string initialGuessMode = "OCHFile";
//    std::string initialGuessMode = "GuessArrays";
   std::string initialGuessMode = "UserGuessClass";
   std::string initialGuessFile = "/Users/jknittel/Collocation/LowThrust/lowthrust/CPlusPlus/test/TestGuess/SchwartzInitialGuess.och";
   ExampleUserGuessClass *userClass = new ExampleUserGuessClass();
   Rvector meshIntervalFractions(2,-1.0,1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(10);

   Rvector timeArray;
   timeArray.SetSize(7);
   timeArray(0) = 0;
   timeArray(1) = .5;
   timeArray(2) = 1.0;
   timeArray(3) = 1.5;
   timeArray(4) = 2.0;
   timeArray(5) = 2.5;
   timeArray(6) = 3.0;

   Rmatrix stateArray;
   stateArray.SetSize(7,2);
   stateArray(0,0) = 1.0;
   stateArray(1,0) = 1.1;
   stateArray(2,0) = 1.2;
   stateArray(3,0) = 1.3;
   stateArray(4,0) = 1.4;
   stateArray(5,0) = 1.5;
   stateArray(6,0) = 1.6;
   stateArray(0,1) = 1.0;
   stateArray(1,1) = .9;
   stateArray(2,1) = .8;
   stateArray(3,1) = .7;
   stateArray(4,1) = .6;
   stateArray(5,1) = .5;
   stateArray(6,1) = .4;

   Rmatrix controlArray;
   controlArray.SetSize(7,1);
   controlArray(0,0) = 0;
   controlArray(1,0) = .1;
   controlArray(2,0) = .2;
   controlArray(3,0) = .3;
   controlArray(4,0) = .4;
   controlArray(5,0) = .5;
   controlArray(6,0) = .6;

   // Set time properties
   Real initialGuessTime = 0.0;
   Real finalGuessTime   = 1.0;
   Real timeLowerBound   = 0.0;
   Real timeUpperBound   = 1.0;

   // Set state properties
   Integer numStateVars  = 2;
   Rvector stateLowerBound  (2, -2.0, -2.0);
   Rvector stateUpperBound  (2,  2.0,  2.0);
   Rvector initialGuessState(2,  1.0,  1.0);
   Rvector finalGuessState  (2,  1.0,  1.0);

   // Set control properties
   Integer numControlVars  = 1;
   Rvector controlUpperBound(1,  0.5);
   Rvector controlLowerBound(1, -0.5);

   phase1->SetInitialGuessMode(initialGuessMode);
   phase1->SetGuessFileName(initialGuessFile);
   phase1->SetInitialGuessArrays(timeArray,stateArray,controlArray);
   phase1->SetUserGuessClass(userClass);

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

   MessageInterface::ShowMessage("*** Creating the second phase\n");

   RadauPhase *phase2           = new RadauPhase();

   // Time properties
   initialGuessTime = 1.0;
   finalGuessTime   = 2.9;
   timeLowerBound   = 1.0;
   timeUpperBound   = 3.0;

   // State properties match phase 1
   // Update control properties
   numControlVars  = 1;
   controlUpperBound(0) =  5.0;
   controlLowerBound(0) = -5.0;

   phase2->SetInitialGuessMode(initialGuessMode);
   phase2->SetGuessFileName(initialGuessFile);
   phase2->SetInitialGuessArrays(timeArray,stateArray,controlArray);
   phase2->SetUserGuessClass(userClass);

   phase2->SetNumStateVars(numStateVars);
   phase2->SetNumControlVars(numControlVars);
   phase2->SetMeshIntervalFractions(meshIntervalFractions);
   phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase2->SetStateLowerBound(stateLowerBound);
   phase2->SetStateUpperBound(stateUpperBound);
   phase2->SetStateInitialGuess(initialGuessState);
   phase2->SetStateFinalGuess(finalGuessState);
   phase2->SetTimeLowerBound(timeLowerBound);
   phase2->SetTimeUpperBound(timeUpperBound);
   phase2->SetTimeInitialGuess(initialGuessTime);
   phase2->SetTimeFinalGuess(finalGuessTime);
   phase2->SetControlLowerBound(controlLowerBound);
   phase2->SetControlUpperBound(controlUpperBound);


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

   MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");

   Rvector  dv2      = traj->GetDecisionVector();
   Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);

   RSMatrix conSp    = phase1->GetConSparsityPattern();

   // ------------------ Optimizing --------------------------------------------
   Rvector z = dv2;
   Rvector F(C.GetSize());
   Rvector xmul(dv2.GetSize());
   Rvector Fmul(C.GetSize());

   MessageInterface::ShowMessage("*** TEST *** Starting Optimization!\n");
   traj->Optimize(z, F, xmul, Fmul);

   traj->WriteToFile("/Users/jknittel/Collocation/LowThrust/lowthrust/CPlusPlus/test/TestGuess/SchwartzOptimizationOutput.och");

   #ifdef DEBUG_SHOW_DATA
      MessageInterface::ShowMessage("*** TEST *** z:\n%s\n",
                                    z.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** F:\n%s\n",
                                    F.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** xmul:\n%s\n",
                                    xmul.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** Fmul:\n%s\n",
                                    Fmul.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** Optimization complete!!\n");

      Rvector dvP1 = phase1->GetDecVector();
      MessageInterface::ShowMessage("*** TEST *** dvP1:\n%s\n",
                                    dvP1.ToString(12).c_str());

      // Interpolate solution
      Rvector timeVector = phase1->GetTimeVector();
      DecisionVector *dv = phase1->GetDecisionVector();
      Rmatrix stateSol   = dv->GetStateArray();
      Rmatrix controlSol = dv->GetControlArray();
   #endif

   MessageInterface::ShowMessage("*** END Guess Generator TEST ***\n");

   return 0;
}
