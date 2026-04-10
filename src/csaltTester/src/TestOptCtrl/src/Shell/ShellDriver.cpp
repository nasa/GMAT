//------------------------------------------------------------------------------
//                           ShellDriver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Mar 13, 2017
/**
 * INSTRUCTIONS:
 *  - Find & replace "Shell" with the actual name for your driver.
 *  - Follow the comments in "Run" to customize it.
 *  - Check that the path and point .cpp and .hpp files are in the
 *      TestOptCtrl/src/pointpath directory.
 *  - Add "src/ShellDriver.o" to the Driver list in the Makefile.
 *  - Add the following lines to the Objects list in the Makefile, below the
 *      existing path and point objects:
 *        $(TEST_ROOT)/TestOptCtrl/src/pointpath/ShellPathObject.o \
 *        $(TEST_ROOT)/TestOptCtrl/src/pointpath/ShellPointObject.o \
 *
 */
//------------------------------------------------------------------------------

#include "ShellDriver.hpp"
//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

ShellDriver::ShellDriver()
{
   // TODO Auto-generated constructor stub

}

ShellDriver::~ShellDriver()
{
   // TODO Auto-generated destructor stub
}

Real ShellDriver::GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ii++)
      if (vec(ii) > max) max = vec(ii);
   return max;
}

Real ShellDriver::GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ii++)
      for (Integer jj = 0; jj < c; jj++)
         if (mat(ii,jj) > max) max = mat(ii,jj);
   return max;
}

int ShellDriver::Run()
{
   std::string outFormat = "%16.9f ";

   //If this test uses ImplicitRungeKutta phases, set this to false.
   bool useRadau = true;

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
      MessageInterface::ShowMessage("The Operating System was not detected\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");
   MessageInterface::ShowMessage("*** TESTing Shell optimization problem ***\n");

   try
   {
      // ---------------------------------------------------------------------
      // ==========================================================================
      // =====  Define Properties for the Trajectory
      // Create trajectory and configure user function names

      MessageInterface::ShowMessage("*** TEST *** Creating & configuring trajectory\n");
      ShellPathObject *pathFunctionObject = new ShellPathObject();
      ShellPointObject *pointFunctionObject = new ShellPointObject();

      //Adjust these values based on your test problem.
      Real costLowerBound = -INF;
      Real costUpperBound = INF;
      Integer maxMeshRefinementCount = 8;

      Trajectory *traj = new Trajectory();

      traj->SetUserPathFunction(pathFunctionObject);
      traj->SetUserPointFunction(pointFunctionObject);
      traj->SetCostLowerBound(costLowerBound);
      traj->SetCostUpperBound(costUpperBound);
      traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);

      //PHASE 1
      MessageInterface::ShowMessage("*** Creating the first phase\n");
      MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 mesh properties\n");

      //Set mesh properties
      Rvector meshIntervalFractions;
      IntegerArray meshIntervalNumPoints;
      Phase *phase1;
      //Uncomment and modify as needed for multiphase problems.
      //Phase *phase2, *phase3;
      if (useRadau)
      {
         //If using Radau, adjust these values based on your test problem.
         phase1 = new RadauPhase();

         meshIntervalFractions.SetSize(9);
         meshIntervalFractions[0] = -1.0;
         meshIntervalFractions[1] = -0.75;
         meshIntervalFractions[2] = -0.5;
         meshIntervalFractions[3] = -0.25;
         meshIntervalFractions[4] =  0.0;
         meshIntervalFractions[5] =  0.25;
         meshIntervalFractions[6] =  0.5;
         meshIntervalFractions[7] =  0.75;
         meshIntervalFractions[8] =  1.0;

         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
         meshIntervalNumPoints.push_back(4);
      }
      else
      {
         //If using ImplicitRungeKutta, adjust these values based on your test problem.
         ImplicitRKPhase *rkphase = new ImplicitRKPhase();
         rkphase->SetTranscription("RungeKutta8");
         phase1 = rkphase;

         meshIntervalFractions.SetSize(2);
         meshIntervalFractions[0] = 0.0;
         meshIntervalFractions[1] = 1.0;
         meshIntervalNumPoints.push_back(5);
      }

      //Adjust this value based on your test problem.
      std::string initialGuessMode = "LinearUnityControl";

      //Adjust these values based on your test problem.
      MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 time properties\n");
      //Set time properties
      Real timeLowerBound = 0.0;
      Real timeUpperBound = 0.0;
      Real initialGuessTime = 0.0;
      Real finalGuessTime = 0.0;

      //Adjust these values based on your test problem.
      MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 state properties\n");
      //Set state properties
      Integer numStateVars = 1;
      Rvector stateLowerBound(1, 0.0);
      Rvector initialGuessState(1, 0.0);
      Rvector finalGuessState(1, 0.0);
      Rvector stateUpperBound(1, 0.0);

      //Adjust these values based on your test problem.
      MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 control properties\n");
      //Set control properties
      Integer numControlVars = 1;
      Rvector controlUpperBound(1, 0.0);
      Rvector controlLowerBound(1, 0.0);

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

      //If your problem has more than 1 phase, uncomment the following block and adjust
      //the variables to create the second phase. Make and adjust a copy for each
      //additional phase.
      /*
      //PHASE 2
      MessageInterface::ShowMessage("*** Creating the second phase\n");
      MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 mesh properties\n");
      //Set mesh properties
      if (useRadau)
      {
         phase2 = new RadauPhase();
      }
      else
      {
         ImplicitRKPhase *rkphase = new ImplicitRKPhase();
         rkphase->SetTranscription("RungeKutta8");
         phase2 = rkphase;
      }

      MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 time properties\n");
      //Set time properties
      timeLowerBound = 0.0;
      timeUpperBound = 0.0;
      initialGuessTime = 0.0;
      finalGuessTime = 0.0;

      MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 state properties\n");
      //Set state properties
      numStateVars = 1;
      Rvector stateLowerBound2(1, 0.0);
      Rvector initialGuessState2(1, 0.0);
      Rvector finalGuessState2(1, 0.0);
      Rvector stateUpperBound2(1, 0.0);

      MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 control properties\n");
      //Set control properties
      Rvector controlUpperBound2(1, 0.0);
      Rvector controlLowerBound2(1, 0.0);

      phase2->SetInitialGuessMode(initialGuessMode);

      phase2->SetNumStateVars(numStateVars);
      phase2->SetNumControlVars(numControlVars);
      phase2->SetMeshIntervalFractions(meshIntervalFractions);
      phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints);
      phase2->SetStateLowerBound(stateLowerBound2);
      phase2->SetStateUpperBound(stateUpperBound2);
      phase2->SetStateInitialGuess(initialGuessState2);
      phase2->SetStateFinalGuess(finalGuessState2);
      phase2->SetTimeLowerBound(timeLowerBound);
      phase2->SetTimeUpperBound(timeUpperBound);
      phase2->SetTimeInitialGuess(initialGuessTime);
      phase2->SetTimeFinalGuess(finalGuessTime);
      phase2->SetControlLowerBound(controlLowerBound2);
      phase2->SetControlUpperBound(controlUpperBound2);
      */

      std::vector<Phase*> pList;
      pList.push_back(phase1);
      //If there are multiple phases, push each one.
      //pList.push_back(phase2);
      MessageInterface::ShowMessage("Setting phase list\n");
      traj->SetPhaseList(pList);

      //Nothing below this point typically needs to be customized.

      MessageInterface::ShowMessage("*** TEST *** initializing the Trajectory\n");
      // Initialize the Trajectory
      traj->Initialize();

      MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");

      Rvector  dv2      = traj->GetDecisionVector();
      Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);

      RSMatrix conSp    = phase1->GetConSparsityPattern();

      // ------------------ Optimizing -----------------------------------------------
      // TRY it first without optimizing
      Rvector z = dv2;
      Rvector F(C.GetSize());
      Rvector xmul(dv2.GetSize());
      Rvector Fmul(C.GetSize());
      MessageInterface::ShowMessage("*** TEST *** Optimizing!!\n");
      traj->Optimize(z, F, xmul, Fmul);

   }
   catch (LowThrustException &ex)
   {
      MessageInterface::ShowMessage("%s\n", ex.GetDetails().c_str());
   }

   #ifdef DEBUG_SHOWRESULTS
   MessageInterface::ShowMessage("*** TEST *** z:\n%s\n",
                                       z.ToString(12).c_str());
   MessageInterface::ShowMessage("*** TEST *** F:\n%s\n",
                                       F.ToString(12).c_str());
   MessageInterface::ShowMessage("*** TEST *** xmul:\n%s\n",
                                       xmul.ToString(12).c_str());
   MessageInterface::ShowMessage("*** TEST *** Fmul:\n%s\n",
                                       Fmul.ToString(12).c_str());
   MessageInterface::ShowMessage("*** TEST *** Optimization complete!!\n");
   // ------------------ Optimizing -----------------------------------------------

   Rvector dvP1 = phase1->GetDecVector();
   MessageInterface::ShowMessage("*** TEST *** dvP1:\n%s\n",
                                 dvP1.ToString(12).c_str());

   // Interpolate solution
   Rvector timeVector = phase1->GetTimeVector();
   DecisionVector *dv = phase1->GetDecisionVector();
   Rmatrix stateSol   = dv->GetStateArray();
   Rmatrix controlSol = dv->GetControlArray();
   #endif

   MessageInterface::ShowMessage("*** END Shell TEST ***\n");

   return 0;
}

