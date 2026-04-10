//------------------------------------------------------------------------------
//                           ConwaySpiralDriver
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
// Created: Feb 13, 2017
/**
 * Friver for the Conway spiral problem (not yet implemented)
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "ConwaySpiralDriver.hpp"
#include "ConwaySpiralPointObject.hpp"
#include "ConwaySpiralPathObject.hpp"

//#define DEBUG_SHOWRESULTS

//static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

ConwaySpiralDriver::ConwaySpiralDriver()
{
   // TODO Auto-generated constructor stub

}

ConwaySpiralDriver::~ConwaySpiralDriver()
{
   // TODO Auto-generated destructor stub
}

Real ConwaySpiralDriver::GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ii++)
      if (vec(ii) > max) max = vec(ii);
   return max;
}

Real ConwaySpiralDriver::GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ii++)
      for (Integer jj = 0; jj < c; jj++)
         if (mat(ii,jj) > max) max = mat(ii,jj);
   return max;
}

int ConwaySpiralDriver::Run()
{
   //TODO: Fill out. Currently just a copy of ConwayOrbitExample.
   /*
   std::string outFormat = "%16.9f ";

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
   MessageInterface::ShowMessage("*** TESTing ConwaySpiral optimization problem ***\n");

   // try
   //   {
      // ---------------------------------------------------------------------
      // ==========================================================================
      // =====  Define Properties for the Trajectory
      // Create trajectory and configure user function names

   MessageInterface::ShowMessage("*** TEST *** Creating & configuring trajectory\n");
   ConwaySpiralPathObject *pathFunctionObject = new ConwaySpiralPathObject();
   ConwaySpiralPointObject *pointFunctionObject = new ConwaySpiralPointObject();

   Real costLowerBound = -INF;
   Real costUpperBound = INF;
   Integer maxMeshRefinementCount = 0;
   Real costScaling = 0.1;

   Trajectory *traj = new Trajectory();

   traj->SetUserPathFunction(pathFunctionObject);
   traj->SetUserPointFunction(pointFunctionObject);
   traj->SetCostLowerBound(costLowerBound);
   traj->SetCostUpperBound(costUpperBound);
   traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);
   traj->SetCostScaling(costScaling);





   MessageInterface::ShowMessage("*** TEST *** Setting mesh properties\n");
   //Set mesh properties
   RadauPhase *phase1 = new RadauPhase();
   std::string initialGuessMode = "LinearUnityControl";
   Rvector meshIntervalFractions(5);
   meshIntervalFractions[0] = -1.0;
   meshIntervalFractions[1] = -0.5;
   meshIntervalFractions[2] =  0.0;
   meshIntervalFractions[3] =  0.5;
   meshIntervalFractions[4] =  1.0;

   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(6);
   meshIntervalNumPoints.push_back(6);
   meshIntervalNumPoints.push_back(6);
   meshIntervalNumPoints.push_back(6);

   MessageInterface::ShowMessage("*** TEST *** Setting time properties\n");
   //Set time properties
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 100.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 50.0;

   MessageInterface::ShowMessage("*** TEST *** Setting state properties\n");
   //Set state properties
   Integer numStateVars = 4;
   Rvector stateLowerBound(4,-10.0, -6.0*2.0*GmatMathConstants::PI, -10.0, -10.0);
   Rvector initialGuessState(4,1.1, 0.0, 0.0, 1.0/sqrt(1.1));
   Rvector finalGuessState(4,5.0, 3*2*GmatMathConstants::PI, 1.0, 1.0);
   Rvector stateUpperBound(4,10.0, 6*2*GmatMathConstants::PI, 10.0, 10.0);

   MessageInterface::ShowMessage("*** TEST *** Setting control properties\n");
   //Set control properties
   Integer numControlVars = 1;
   Rvector controlUpperBound(1,10.0);
   Rvector controlLowerBound(1,-10.0);

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

   std::vector<Phase*> pList;
   pList.push_back(phase1);
   MessageInterface::ShowMessage("*** TEST *** Setting phase list\n");
   traj->SetPhaseList(pList);

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

   MessageInterface::ShowMessage("*** END ConwaySpiral TEST ***\n");

   */
   return 0;
}
