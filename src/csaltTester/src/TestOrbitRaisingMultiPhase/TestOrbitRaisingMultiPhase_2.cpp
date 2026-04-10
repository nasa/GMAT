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
#include "Trajectory.hpp"
//#include "Phase.hpp"
#include "DecVecTypeBetts.hpp"
#include "GuessGenerator.hpp"
#include "OrbitRaisingMultiPhasePointObject.hpp"
#include "OrbitRaisingMultiPhasePathObject.hpp"
#include "OrbitRaisingPointObject.hpp"
#include "OrbitRaisingPathObject.hpp"
#include "boost/config.hpp"  // BOOST


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
      Integer maxMeshRefinementCount = 0;

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

      RadauPhase *phase1           = new RadauPhase();
      std::string initialGuessMode = "LinearUnityControl";
      Rvector meshIntervalFractions(2, -1.0, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(7);   // was - 50*ones(1,1) - commented out
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

      RadauPhase *phase2        = new RadauPhase();
      phase2->SetInitialGuessMode("LinearUnityControl");
      Rvector meshIntervalFractions2(2, -1.0, 1.0);
      //%phase2.meshIntervalNumPoints     = 50*ones(1,1);
      IntegerArray meshIntervalNumPoints2;
      meshIntervalNumPoints2.push_back(7);   // was - 50*ones(1,1) - commented out
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
//      %traj.linkageConfig{1} = {phase1,phase2};
//      [z,F,xmul,Fmul] = traj.Optimize();
      
      
//      Rvector decVec(112,
//                        0.0,
//                        0.874788287660863,
//                        1.0,
//                        0.0,
//                        0.0,
//                        1.0,
//                        1.0,
//                        0.418638285944116,
//                        0.908153391165818,
//                        1.00013375697725,
//                        0.0641613456529463,
//                        0.00439863938650356,
//                        1.00798623414871,
//                        0.995213360375178,
//                        0.443701883277405,
//                        0.896174479678608,
//                        1.0016258489303,
//                        0.204239455356736,
//                        0.0182034676593185,
//                        1.0237262328372,
//                        0.984879823550106,
//                        0.50105977398946,
//                        0.865412683370421,
//                        1.00733623224445,
//                        0.39363873017592,
//                        0.0454005048932221,
//                        1.04006086118639,
//                        0.971083432919404,
//                        0.58587561842451,
//                        0.810401994164376,
//                        1.01969697973108,
//                        0.593790348218696,
//                        0.0835285719672894,
//                        1.04914855743659,
//                        0.956558147741607,
//                        0.682463691333357,
//                        0.730919512612957,
//                        1.03661082773538,
//                        0.76258046287112,
//                        0.121905543183325,
//                        1.04868430172207,
//                        0.944180710359978,
//                        0.767542759938876,
//                        0.640997948997497,
//                        1.05061590629451,
//                        0.866800377247734,
//                        0.147880214751275,
//                        1.04406866613842,
//                        0.936399217721148,
//                        0.819991530052321,
//                        0.572375742990062,
//                        1.05449234253983,
//                        0.892219743261639,
//                        0.154432242040947,
//                        1.04239600693235,
//                        0.934478357254202,
//                        0.874788287660863,
//                        3.32,
//                        1.05449234253983,
//                        0.892219743261639,
//                        0.154432242040947,
//                        1.04239600693235,
//                        0.934478357254202,
//                        0.831036158559443,
//                        0.55622169139149,
//                        1.08579916310088,
//                        1.06513277655561,
//                        0.198926577757929,
//                        1.02551976232233,
//                        0.921098723074761,
//                        0.913775642035913,
//                        0.406219342662396,
//                        1.18339719580798,
//                        1.40343312196817,
//                        0.30005559782219,
//                        0.949566320511046,
//                        0.892214379707385,
//                        0.997234258300699,
//                        -0.0743276779280805,
//                        1.34531611756982,
//                        1.76441272435955,
//                        0.296311793869297,
//                        0.821551139949751,
//                        0.853650653564682,
//                        -0.991193385207691,
//                        -0.132423287064524,
//                        1.46867561615421,
//                        2.06487118736734,
//                        0.15188384155421,
//                        0.762061473993129,
//                        0.813049518475429,
//                        -0.940278637154706,
//                        0.34040578572212,
//                        1.51617516744929,
//                        2.30149993570472,
//                        0.065361949282618,
//                        0.775311707846435,
//                        0.778452056116586,
//                        -0.841187858133557,
//                        0.540743003503342,
//                        1.52826219147421,
//                        2.45127061011572,
//                        0.00763761705147656,
//                        0.800081197797525,
//                        0.756701196842023,
//                        -0.772647280860197,
//                        0.634835662953065,
//                        1.52779256874015,
//                        2.48905934883927,
//                        1.93870456067116e-26,
//                        0.809035918682472,
//                        0.751332);

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

      
      bool reDim  = true;
      bool nonDim = true;

      MessageInterface::ShowMessage("*** TEST *** setting decision vector on Trajectory\n");
//      traj->SetDecisionVector(decVec);   // ,reDim);
      Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);
      RSMatrix J        = traj->GetJacobian(); // nonDim);
//      RSMatrix sparsity = traj->GetSparsityPattern();
//
//      MessageInterface::ShowMessage("*** TEST *** sparsity (with MATLAB-style indexes):\n");
//      Integer rSparse = sparsity.size1();
//      Integer cSparse = sparsity.size2();
//      for (Integer cc = 0; cc < cSparse; cc++)
//      {
//         for (Integer rr = 0; rr < rSparse; rr++)
//         {
//            Real jacTmp = sparsity(rr,cc);
//            if (jacTmp != 0.0)
//            MessageInterface::ShowMessage("      sparsity(%d, %d) =  %12.10f\n", rr+1, cc+1, jacTmp);
//         }
//      }

      Rvector dvP1 = phase1->GetDecVector();
      Rvector dvP2 = phase2->GetDecVector();
      MessageInterface::ShowMessage("*** TEST *** dvP1 (Before optimization):\n%s\n",
                                    dvP1.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** dvP2 (Before optimization):\n%s\n",
                                    dvP2.ToString(12).c_str());

      MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");
      
      RSMatrix conSp    = phase1->GetConSparsityPattern();
//      MessageInterface::ShowMessage("*** TEST *** Con sparsity pattern from Phase1:\n");
//      Integer rJac = conSp.size1();
//      Integer cJac = conSp.size2();
//      for (Integer cc = 0; cc < cJac; cc++)
//      {
//         for (Integer rr = 0; rr < rJac; rr++)
//         {
//            Real jacTmp = conSp(rr,cc);
//            if (jacTmp != 0.0)
//            MessageInterface::ShowMessage(" Phase1ConSparsity(%d, %d) =  %12.10f\n", rr+1, cc+1, jacTmp);
//         }
//      }
//      RSMatrix sparsity = traj->GetSparsityPattern();
//      
//      MessageInterface::ShowMessage("*** TEST *** sparsity BEFORE Optimize (with MATLAB-style indexes):\n");
//      Integer rSparse = sparsity.size1();
//      Integer cSparse = sparsity.size2();
//      for (Integer cc = 0; cc < cSparse; cc++)
//      {
//         for (Integer rr = 0; rr < rSparse; rr++)
//         {
//            Real jacTmp = sparsity(rr,cc);
//            if (jacTmp != 0.0)
//            MessageInterface::ShowMessage("      sparsity(%d, %d) =  %12.10f\n", rr+1, cc+1, jacTmp);
//         }
//      }

// ------------------ Optimizing -----------------------------------------------
// TRY it first without optimizing
      Rvector z = decVec;
      Rvector F(C.GetSize());
      Rvector xmul(decVec.GetSize());
      Rvector Fmul(C.GetSize());
      MessageInterface::ShowMessage("*** TEST *** Optimizing!!\n");
      traj->Optimize(z, F, xmul, Fmul);

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

//      RSMatrix sparsity2 = traj->GetSparsityPattern();
//      MessageInterface::ShowMessage("*** TEST *** sparsity2 AFTER Optimize (with MATLAB-style indexes):\n");
//      rSparse = sparsity2.size1();
//      cSparse = sparsity2.size2();
//      for (Integer cc = 0; cc < cSparse; cc++)
//      {
//         for (Integer rr = 0; rr < rSparse; rr++)
//         {
//            Real jacTmp = sparsity2(rr,cc);
//            if (jacTmp != 0.0)
//            MessageInterface::ShowMessage("      sparsity2(%d, %d) =  %12.10f\n", rr+1, cc+1, jacTmp);
//         }
//      }

      dvP1 = phase1->GetDecVector();
      dvP2 = phase2->GetDecVector();
      MessageInterface::ShowMessage("*** TEST *** dvP1 (AFTER optimization):\n%s\n",
                                    dvP1.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** dvP2 (AFTER optimization):\n%s\n",
                                    dvP2.ToString(12).c_str());
      
      Rvector dv     = traj->GetDecisionVector();
      Rvector dvLow  = traj->GetDecisionVectorLowerBound();
      Rvector dvHigh = traj->GetDecisionVectorUpperBound();
      
      Real      costLow  = traj->GetCostLowerBound();
      Real      costHigh = traj->GetCostUpperBound();
      RealArray conLow   = traj->GetAllConLowerBound();
      RealArray conHigh  = traj->GetAllConUpperBound();
      
      // Output results
      MessageInterface::ShowMessage("*** TEST *** C (with MATLAB-style indexes):\n"); // %s\n",
//                                    C.ToString(12).c_str());
      for (Integer ii = 0; ii < C.GetSize(); ii++)
         MessageInterface::ShowMessage("   (%d)    %le\n", ii+1, C(ii));
      
      MessageInterface::ShowMessage("*** TEST *** J (with MATLAB-style indexes):\n");
      Integer rJ = J.size1();
      Integer cJ = J.size2();
      for (Integer cc = 0; cc < cJ; cc++)
      {
         for (Integer rr = 0; rr < rJ; rr++)
         {
            Real jacTmp = J(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage("      J(%d, %d) =  %12.10f\n", rr+1, cc+1, jacTmp);
         }
      }
      
      
      MessageInterface::ShowMessage("*** TEST *** dv:\n%s\n",
                                    dv.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** dvLow:\n%s\n",
                                    dvLow.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** dvHigh:\n%s\n",
                                    dvHigh.ToString(12).c_str());
      MessageInterface::ShowMessage("*** TEST *** costLow:\n%12.10f\n",
                                    costLow);
      MessageInterface::ShowMessage("*** TEST *** costHigh:\n%12.10f\n",
                                    costHigh);
      MessageInterface::ShowMessage("*** TEST *** conLow:\n");
      for (Integer ii = 0; ii < conLow.size(); ii++)
         MessageInterface::ShowMessage("   (%d) = %12.10f\n", conLow.at(ii));
      MessageInterface::ShowMessage("*** TEST *** conHigh:\n");
      for (Integer ii = 0; ii < conHigh.size(); ii++)
         MessageInterface::ShowMessage("   (%d) = %12.10f\n", conHigh.at(ii));
      
      Rvector  C2        = traj->GetCostConstraintFunctions(); // nonDim);
      // Output results
      MessageInterface::ShowMessage("*** TEST *** C2 (with MATLAB-style indexes):\n"); // %s\n",
      //                                    C.ToString(12).c_str());
      for (Integer ii = 0; ii < C2.GetSize(); ii++)
         MessageInterface::ShowMessage("   (%d)    %le\n", ii+1, C2(ii));
     

      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}
