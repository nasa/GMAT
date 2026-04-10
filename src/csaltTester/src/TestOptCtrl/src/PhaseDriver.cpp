//------------------------------------------------------------------------------
//                           PhaseDriver
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 15, 2017
/**
 * Phase class unit test driver
 */
//------------------------------------------------------------------------------

#include "csaltdefs.hpp"
#include "PhaseDriver.hpp"

#include "OrbitRaisingPathObject.hpp"

//#define DEBUG_SHOWRESULTS

PhaseDriver::PhaseDriver()
{
   // TODO Auto-generated constructor stub

}

PhaseDriver::~PhaseDriver()
{
   // TODO Auto-generated destructor stub
}

//$Id$
//------------------------------------------------------------------------------
//                               TestPhase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.06.035
//
/**
 * Test driver for Phase class and its interfaces.
 */
//------------------------------------------------------------------------------




using namespace std;
using namespace GmatMathConstants;
using namespace GmatMathUtil;

Real PhaseDriver::GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ii++)
      if (vec(ii) > max) max = vec(ii);
   return max;
}

Real PhaseDriver::GetMaxError(const Rmatrix &mat)
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
int PhaseDriver::Run()
{
   std::string outFormat = "%16.9f ";
//   Real        tolerance = 1e-15;

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
      MessageInterface::ShowMessage("Current oS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");

   try
   {
      // Test the Phase
      MessageInterface::ShowMessage("*** TEST*** creating and initializing a Phase\n");

      // ---------------------------------------------------------------------
      // Orbit Raising Path specific set-up starts here
      // ---------------------------------------------------------------------
      RadauPhase *phase1 = new RadauPhase();
      phase1->SetInitialGuessMode("LinearUnityControl");
      Rvector meshIntervalFractions(2, -1.0, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(7);

      MessageInterface::ShowMessage("TESTPHASE --- Phase created <%p>.\n", phase1);

      OrbitRaisingPathObject *pathFuncObj = new OrbitRaisingPathObject();
      MessageInterface::ShowMessage("TESTPHASE --- OrbitRisingPathObject created <%p>.\n", pathFuncObj);
      // Set time properties
      Real timeLowerBound      = 0.0;
      Real initialGuessTime    = 0.0;
      Real finalGuessTime      = 1.0;
      Real timeUpperBound      = 3.32;
      // Set state properties
      Integer numStateVars     = 5;
      Rvector stateLowerBound(5, 0.5, 0.0, -10.0, -10.0, 0.1);
      Rvector initialGuessState(5, 1.0, 0.0, 0.0, 1.0, 1.0); // [r0 theta0 vr0 vtheta0 m0]
      Rvector finalGuessState(5, 1.0, PI, 0.0, 0.5, 0.5);     // rf thetaf vrf vthetaf mf]
      Rvector stateUpperBound(5, 5.0, 4.0*PI, 10.0, 10.0, 3.0);
      // Set control properties
      Integer numControlVars   = 2;
      Rvector controlUpperBound(2,  10.0,  10.0);
      Rvector controlLowerBound(2, -10.0, -10.0);
      Rvector decVector(56,
                        0.0,
                        3.32,
                        1.0,
                        2.84101063191129e-13,
                        0.0,
                        1.00000000000002,
                        1.0,
                        0.46564207244944,
                        0.884974100541562,
                        1.00292150959124,
                        0.24510014177824,
                        0.0248656190820442,
                        1.02556394151943,
                        0.981833726195737,
                        0.566713541657015,
                        0.823914988214297,
                        1.03953838754381,
                        0.783146974241609,
                        0.128385089613886,
                        1.04637271043109,
                        0.942615846003289,
                        0.826929089699722,
                        0.562306508250403,
                        1.19252136085221,
                        1.41233382715236,
                        0.293096855201965,
                        0.914835511777526,
                        0.890255729229884,
                        0.923208252346056,
                        -0.384300120276895,
                        1.40796689256,
                        1.88052713429625,
                        0.245429245261179,
                        0.755727892844444,
                        0.835129308962834,
                        -0.952951568440912,
                        0.30312263963231,
                        1.50799680500876,
                        2.20156392370062,
                        0.0743757524753398,
                        0.766816704594532,
                        0.788154409222363,
                        -0.823874663720378,
                        0.566772060138446,
                        1.5238694762769,
                        2.40497639294557,
                        0.018970008002483,
                        0.794803522925167,
                        0.758622057308969,
                        -0.734971943498651,
                        0.678098021376783,
                        1.52560750596174,
                        2.4557570010183,
                        5.54801539173474e-18,
                        0.809615085255086,
                        0.751332);

      MessageInterface::ShowMessage("TESTPHASE --- Orbit Raising Path specific set-up complete.\n");

      // ---------------------------------------------------------------------
      // Orbit Raising Path specific set-up ends here
      // ---------------------------------------------------------------------


      //==========================================================================
      // =====  Mimic how trajectory drives the phase class
      //==========================================================================

      // Configure the phase according to setup
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

      // Set phase number, this is done by trajectory
      phase1->SetPhaseNumber(0);    // was 1

      MessageInterface::ShowMessage("TESTPHASE --- About to set path function ....\n");

      // This is code from Trajectory.InitializePhases()
      phase1->SetPathFunction(pathFuncObj);

      MessageInterface::ShowMessage("TESTPHASE --- About to initialize Phase ....\n");

      phase1->Initialize();

      Integer numPhaseConstraints    = phase1->GetNumTotalConNLP();
      Integer numPhaseDecisionParams = phase1->GetNumDecisionVarsNLP();
      Integer totalnumConstraints    = phase1->GetNumTotalConNLP();

      MessageInterface::ShowMessage(" ---> numPhaseConstraints    = %d \n", numPhaseConstraints);
      MessageInterface::ShowMessage(" ---> numPhaseDecisionParams = %d \n", numPhaseDecisionParams);
      MessageInterface::ShowMessage(" ---> totalnumConstraints    = %d \n", totalnumConstraints);

      MessageInterface::ShowMessage("TESTPHASE --- Calling PrepareToOptimize ....\n");

      // This is code from Trajectory.PrepareToOptimize()
      phase1->PrepareToOptimize();
      MessageInterface::ShowMessage("TESTPHASE --- Done with PrepareToOptimize ....\n");

      MessageInterface::ShowMessage("TESTPHASE --- Getting the Decision vector ....\n");
      DecisionVector *decvec = phase1->GetDecisionVector();

      MessageInterface::ShowMessage("TESTPHASE --- Getting the State and Control arrays ....\n");
      Rmatrix stateGuess   = decvec->GetStateArray();
      MessageInterface::ShowMessage(" ---> stateGuess: \n");
      Integer r, c;
      stateGuess.GetSize(r,c);
      for (Integer rr = 0; rr < r; rr++)
         for (Integer cc = 0; cc < c; cc++)
            MessageInterface::ShowMessage(" (%d, %d)  = %12.10f\n", rr, cc, stateGuess(rr, cc));

      Rmatrix controlGuess = decvec->GetControlArray(); // NOTE - segmentation fault is happening while writing out controlGuess!!!!!
      MessageInterface::ShowMessage(" ---> controlGuess: \n");
      controlGuess.GetSize(r,c);
      for (Integer rr = 0; rr < r; rr++)
         for (Integer cc = 0; cc < c; cc++)
            MessageInterface::ShowMessage(" (%d, %d)  = %12.10f\n", rr, cc, controlGuess(rr, cc));

      //  Set the decision vector
      phase1->SetDecisionVector(decVector);

      // This tests only computation of algebraic path constraints
      if (phase1->HasAlgPathCons())
      {
         MessageInterface::ShowMessage("TESTPHASE --- Computing algebraic constraints ....\n");
         phase1->ComputeAlgebraicPathConstraints();
         // @todo write some output here
         Rvector f;
         RSMatrix jac;
         phase1->ComputeAlgFuncAndJac(f, jac);
         MessageInterface::ShowMessage("funcValues = \n");
         for (Integer ii = 0; ii < f.GetSize(); ii++)
            MessageInterface::ShowMessage("  %d      %12.10f\n", ii, f(ii));
         MessageInterface::ShowMessage("jacArray = \n");
         Integer rJac = jac.size1();
         Integer cJac = jac.size2();
         for (Integer rr = 0; rr < rJac; rr++)
         {
            for (Integer cc = 0; cc < cJac; cc++)
            {
               Real jacTmp = jac(rr,cc);
               if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr, cc, jacTmp);
            }
         }
      }

      // Now request the data that trajectory requests from phase

      // Get function values
      Integer totalnumCons = phase1->GetNumTotalConNLP();
      MessageInterface::ShowMessage(" ---> totalnumCons    = %d \n",      totalnumCons);

      Real costFunc        = phase1->GetCostFunction();
      MessageInterface::ShowMessage(" ---> costFunc        = %12.10f \n", costFunc);

      Rvector conVec       = phase1->GetConstraintVector();
      MessageInterface::ShowMessage(" ---> conVec:\n");
      for (Integer ii = 0; ii < conVec.GetSize(); ii++)
         MessageInterface::ShowMessage(" (%d)  = %12.10f\n", ii, conVec(ii));

      // Get the Jacobians and sparsity patterns
      RSMatrix costJacobian = phase1->GetCostJacobian();
      MessageInterface::ShowMessage("costJacobian (matching MATLAB indexes!!!!!)::\n");
      SparseMatrixUtil::PrintNonZeroElements(&costJacobian);

      RSMatrix conJacobian  = phase1->GetConJacobian();
      MessageInterface::ShowMessage("conJacobian (matching MATLAB indexes!!!!!)::\n");
      // To match the MATLAB output, print it out in column order and match indexes
      for (unsigned cc = 0; cc < conJacobian.size2(); cc++)
      {
         for (unsigned rr = 0; rr < conJacobian.size1(); rr++)
         {
            Real tmp = conJacobian(rr,cc);
            if (tmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d) = %12.10f\n", rr+1, cc+1, tmp);
         }
      }


      RSMatrix costSparsity = phase1->GetCostSparsityPattern();
      MessageInterface::ShowMessage("costSparsity::\n");
      SparseMatrixUtil::PrintNonZeroElements(&costSparsity);

      RSMatrix conSparsity  = phase1->GetConSparsityPattern();
      MessageInterface::ShowMessage("conSparsity::\n");
      // To match the MATLAB output, print it out in column order and match indexes
      for (UnsignedInt cc = 0; cc < conSparsity.size2(); cc++)
      {
         for (UnsignedInt rr = 0; rr < conSparsity.size1(); rr++)
         {
            Real tmp = conSparsity(rr,cc);
            if (tmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d) = %12.10f\n", rr+1, cc+1, tmp);
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

   return 0;
}

