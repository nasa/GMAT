//------------------------------------------------------------------------------
//                               TestUserPathFunction
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
 * Test driver for FunctionOutputData classes.
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
#include "UserPathFunction.hpp"
#include "UserPointFunction.hpp"
#include "FunctionOutputData.hpp"
#include "PathFunctionContainer.hpp"
#include "PointFunctionContainer.hpp"
#include "UserFunctionManager.hpp"
#include "UserPathFunctionManager.hpp"
#include "DummyPathFunction.hpp"
#include "DummyPathFunction2.hpp"
#include "UserFunctionProperties.hpp"
#include "BoundData.hpp"
#include "ProblemCharacteristics.hpp"
//#include "Phase.hpp"
#include "DecVecTypeBetts.hpp"
#include "GuessGenerator.hpp"
#include "boost/config.hpp"  // BOOST

//#include "HypSenPathFunction.hpp"

using namespace std;

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
      MessageInterface::ShowMessage("Current oS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");

   try
   {
   // Test the PathFunctionContainer
   MessageInterface::ShowMessage("*** TEST*** creating and initializing a PathFunctionContainer\n");
   PathFunctionContainer *pfc = new PathFunctionContainer();
   pfc->Initialize();
   
   // Create and initialize FunctionInputData
   MessageInterface::ShowMessage("*** TEST*** creating function input data\n");
   Integer numStateVars   = 3;
   Integer numControlVars = 2;
   Rvector state(3, 0.1, 0.2, 0.3);
   Rvector control(2, 0.4, 0.5);
   Real    time = 0.5;
      
   FunctionInputData *fid = new FunctionInputData();
   fid->Initialize(numStateVars, numControlVars);
   fid->SetStateVector(state);
   fid->SetControlVector(control);
   fid->SetTime(time);
   fid->SetPhaseNum(0);
   fid->SetIsPerturbing(false);

   // Create and initialize Bounds data
   MessageInterface::ShowMessage("*** TEST*** creating bound data\n");
   BoundData *bd = new BoundData();
   Rvector stateLower(3, -5.0, -5.0, -5.0);
   Rvector stateUpper(3, 5.0, 5.0, 5.0);
   Rvector controlLower(2, -5.0, -5.0);
   Rvector controlUpper(2, 5.0, 5.0);
   Rvector timeLower(1,-10.0);
   Rvector timeUpper(1, 10.0);
   bd->SetStateLowerBound(stateLower);
   bd->SetStateUpperBound(stateUpper);
   bd->SetControlLowerBound(controlLower);
   bd->SetControlUpperBound(controlUpper);
   bd->SetTimeLowerBound(timeLower);
   bd->SetTimeUpperBound(timeUpper);

   // Create the user function object, initialize it, and test call
   // to evaluate
   DummyPathFunction *dpf = new DummyPathFunction();
   dpf->Initialize(fid, pfc);
   // DO I NEED TO SET THE ALG LOWER AND UPPER BOUNDS HERE???????
   PathFunctionContainer* pfRes = dpf->EvaluateUserFunction(fid, pfc);
   
   // test function values
   FunctionOutputData *cost = pfRes->GetCostData();
   FunctionOutputData *dyn  = pfRes->GetDynData();
   FunctionOutputData *alg  = pfRes->GetAlgData();

   MessageInterface::ShowMessage("*** TEST*** Test case results:\n\n");

   // cost:
   Rvector costVals = cost->GetFunctionValues();
   for (Integer ii = 0; ii < costVals.GetSize(); ii++)
      costVals(ii) -= 0.0006;
   if (GetMaxError(costVals) > 1.0e-14)
      MessageInterface::ShowMessage("--- ERROR in user cost function evaluation\n");
   else
      MessageInterface::ShowMessage("User cost function evaluation OK\n");

   if (cost->GetNumFunctions() != 1)
      MessageInterface::ShowMessage("--- ERROR in number of user cost functions\n");
   else
      MessageInterface::ShowMessage("Number of user cost function OK\n");
   if (!cost->HasUserFunction())
      MessageInterface::ShowMessage("--- ERROR in HasUserFunction for cost function\n");
   else
      MessageInterface::ShowMessage("HasUserFunction for user cost functions OK\n");
   if (cost->HasUserStateJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserStateJacobian for cost function\n");
   else
      MessageInterface::ShowMessage("HasUserStateJacobian for user cost functions OK\n");
   if (cost->HasUserControlJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserControlJacobian for cost function\n");
   else
      MessageInterface::ShowMessage("HasUserControlJacobian for user cost functions OK\n");
   if (cost->HasUserTimeJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserTimeJacobian for cost function\n");
   else
      MessageInterface::ShowMessage("HasUserTimeJacobian for user cost functions OK\n");

   // dynamics:
   Rvector dynTruth(2, -0.145054, 0.233982);
   Rvector dynVals = dyn->GetFunctionValues();
   for (Integer ii = 0; ii < dynVals.GetSize(); ii++)
      dynVals(ii) -= dynTruth(ii);
   if (GetMaxError(dynVals) > 1.0e-14)
      MessageInterface::ShowMessage("--- ERROR in user dynamics function evaluation\n");
   else
      MessageInterface::ShowMessage("User dynamics function evaluation OK\n");
   if (dyn->GetNumFunctions() != 2)
      MessageInterface::ShowMessage("--- ERROR in number of user dynamics functions\n");
   else
      MessageInterface::ShowMessage("Number of user dynamics function OK\n");
   if (!dyn->HasUserFunction())
      MessageInterface::ShowMessage("--- ERROR in HasUserFunction for dynamics function\n");
   else
      MessageInterface::ShowMessage("HasUserFunction for user dynamics functions OK\n");
   if (dyn->HasUserStateJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserStateJacobian for dynamics function\n");
   else
      MessageInterface::ShowMessage("HasUserStateJacobian for user dynamics functions OK\n");
   if (dyn->HasUserControlJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserControlJacobian for dynamics function\n");
   else
      MessageInterface::ShowMessage("HasUserControlJacobian for user dynamics functions OK\n");
   if (dyn->HasUserTimeJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserTimeJacobian for dynamics function\n");
   else
      MessageInterface::ShowMessage("HasUserTimeJacobian for user dynamics functions OK\n");

   // algebraic:
   Rvector algTruth(2, -0.455700580773814, 0.735076132272247);
   Rvector algVals = alg->GetFunctionValues();
   for (Integer ii = 0; ii < algVals.GetSize(); ii++)
      algVals(ii) -= algTruth(ii);
   if (GetMaxError(algVals) > 1.0e-14)
      MessageInterface::ShowMessage("--- ERROR in user algebraic function evaluation\n");
   else
      MessageInterface::ShowMessage("User algebraic function evaluation OK\n");
   if (alg->GetNumFunctions() != 2)
      MessageInterface::ShowMessage("--- ERROR in number of user algebraic functions\n");
   else
      MessageInterface::ShowMessage("Number of user algebraic function OK\n");
   if (!alg->HasUserFunction())
      MessageInterface::ShowMessage("--- ERROR in HasUserFunction for algebraic function\n");
   else
      MessageInterface::ShowMessage("HasUserFunction for user algebraic functions OK\n");
   if (alg->HasUserStateJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserStateJacobian for algebraic function\n");
   else
      MessageInterface::ShowMessage("HasUserStateJacobian for user algebraic functions OK\n");
   if (alg->HasUserControlJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserControlJacobian for algebraic function\n");
   else
      MessageInterface::ShowMessage("HasUserControlJacobian for user algebraic functions OK\n");
   if (alg->HasUserTimeJacobian())
      MessageInterface::ShowMessage("--- ERROR in HasUserTimeJacobian for algebraic function\n");
   else
      MessageInterface::ShowMessage("HasUserTimeJacobian for user algebraic functions OK\n");


   // Test initializing/calling a user path function via the function manager

   // Create a new user data container

   PathFunctionContainer *pfc2 = new PathFunctionContainer();
   pfc2->Initialize();

   FunctionInputData *fid2 = new FunctionInputData();
   // NOTE: numStateVars, numControlVars, state, control, time, bound are unchanged
   fid2->Initialize(numStateVars, numControlVars);
   fid2->SetStateVector(state);
   fid2->SetControlVector(control);
   fid2->SetTime(time);
   fid2->SetPhaseNum(0);
   fid2->SetIsPerturbing(false);

   UserPathFunctionManager *manager = new UserPathFunctionManager();
   DummyPathFunction       *dummy   = new DummyPathFunction();
   dummy->Initialize(fid2, pfc2);
   dummy->SetAlgFunctionsLowerBounds(controlLower);  // using control values for testing?
   dummy->SetAlgFunctionsUpperBounds(controlUpper);  // using control values for testing?

   if (!dummy) MessageInterface::ShowMessage("dummy is NULL <<<<< \n");
   MessageInterface::ShowMessage("*** TEST*** dummy created, now initializing UserPathFunctionManager:\n");
   MessageInterface::ShowMessage("   the FunctionInputData is <%p>, the PathFunctionContainer is <%p>, the BoundData is <%p>\n",
         fid2, pfc2, bd);

   manager->Initialize(dummy, fid2, pfc2, bd);

   // Was failing here <<<<<< fails when validating setting of bounds for alg data

//   MessageInterface::ShowMessage("*** TEST*** UserPathFunctionManager has been initialized\n");

   // Test the interface called to evaluate functions
   PathFunctionContainer *pfcRes;
   clock_t t0 = clock();
   for (Integer ii = 0; ii < 10000; ii++)
   {
      fid2->SetStateVector(state);  // ADD THESE HERE FOR TIMING TEST
      fid2->SetControlVector(control);
      fid2->SetTime(time);

      pfcRes = manager->EvaluateUserJacobian(fid2, pfc2);
   }
   Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
   MessageInterface::ShowMessage(">>>>>>>>>> CLOCK TIME (sec) for all iterations = %12.10f\n", timeSpent);

   MessageInterface::ShowMessage("*** TEST*** EvaluateUserJacobian has completed\n");

   // Truth data for partials
   Rmatrix algStateJac(2,3,
         -0.00339292006587698, -0.000848230016469244, -0.00169646003293849,
         -0.00169646003293849, -0.000282743338823081, -0.000376991118430775);
   Rmatrix algControlJac(2,2,
         -0.314159265358979,       -0.376991118430775,
         -0.376991118430775,       -0.201061929829747);
   Rmatrix algTimeJac(2,1, (-0.75 * GmatMathConstants::PI), GmatMathConstants::PI);
   Rmatrix dynStateJac(2,3,
             -0.00108,                 -0.00027,                 -0.00054,
             -0.00054,                   -9e-05,                 -0.00012);
   Rmatrix dynControlJac(2,2,
           -0.1,                    -0.12,
           -0.12,                   -0.064);
   Rmatrix dynTimeJac(2,1, -0.75, 1.0);
   Rmatrix costStateJac(1,3, 0.006, 0.003,   0.002);
   Rmatrix costControlJac(1,2,  0.0015, 0.0012);
   Rmatrix costTimeJac(1,1, 0.0012);

   MessageInterface::ShowMessage("*** TEST*** jacobian input data set up; now about to call GetStateJacobian\n");

   // Test the partials.  NOTE: need to have useAnalyticPartials set to
   // false in Dummy function to test here!
   // dynamics partials:
   Rmatrix dynState = pfcRes->GetDynData()->GetStateJacobian() - dynStateJac;
   if (GetMaxError(dynState) > 1.0e-8)
   {
      MessageInterface::ShowMessage("--------- ERROR in finite difference dyn state Jacobian:\n");
      MessageInterface::ShowMessage("--------- %s\n", (pfcRes->GetDynData()->GetStateJacobian()).ToString(12).c_str());
   }
   else
      MessageInterface::ShowMessage("Finite difference dyn state Jacobian OK\n");
   Rmatrix dynControl = pfcRes->GetDynData()->GetControlJacobian() - dynControlJac;
   if (GetMaxError(dynControl) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference dyn control Jacobian\n");
   else
      MessageInterface::ShowMessage("Finite difference dyn control Jacobian OK\n");
   Rmatrix dynTime = pfcRes->GetDynData()->GetTimeJacobian() - dynTimeJac;
   if (GetMaxError(dynTime) > 1.0e-7)
      MessageInterface::ShowMessage("--------- ERROR in finite difference dyn time Jacobian (%12.10f)\n", GetMaxError(dynTime));
   else
      MessageInterface::ShowMessage("Finite difference dyn time Jacobian OK\n");

   Rmatrix algState = pfcRes->GetAlgData()->GetStateJacobian() - algStateJac;
   if (GetMaxError(algState) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference alg state Jacobian\n");
   else
      MessageInterface::ShowMessage("Finite difference alg state Jacobian OK\n");
   Rmatrix algControl = pfcRes->GetAlgData()->GetControlJacobian() - algControlJac;
   if (GetMaxError(algControl) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference alg control Jacobian\n");
   else
      MessageInterface::ShowMessage("Finite difference alg control Jacobian OK\n");
   Rmatrix algTime = pfcRes->GetAlgData()->GetTimeJacobian() - algTimeJac;
   if (GetMaxError(algTime) > 1.0e-6)
      MessageInterface::ShowMessage("--------- ERROR in finite difference alg time Jacobian (%12.10f)\n", GetMaxError(algTime));
   else
      MessageInterface::ShowMessage("Finite difference alg time Jacobian OK\n");


   Rmatrix costState = pfcRes->GetCostData()->GetStateJacobian() - costStateJac;
   if (GetMaxError(costState) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference cost state Jacobian\n");
   else
      MessageInterface::ShowMessage("Finite difference cost state Jacobian OK\n");
   Rmatrix costControl = pfcRes->GetCostData()->GetControlJacobian() - costControlJac;
   if (GetMaxError(costControl) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference cost control Jacobian\n");
   else
      MessageInterface::ShowMessage("Finite difference cost control Jacobian OK\n");
   Rmatrix costTime = pfcRes->GetCostData()->GetTimeJacobian() - costTimeJac;
   if (GetMaxError(costTime) > 1.0e-8)
      MessageInterface::ShowMessage("--------- ERROR in finite difference cost time Jacobian(%12.10f)\n", GetMaxError(costTime));
   else
      MessageInterface::ShowMessage("Finite difference cost time Jacobian OK\n");

   // Unit test the sparsity determination
   // Create the new container
   PathFunctionContainer *pfc3 = new PathFunctionContainer();
   pfc3->Initialize();

   Rvector newState(3, 0.2, 0.4, 0.6);
   Rvector newControl(2, 0.8, 1.0);

   MessageInterface::ShowMessage("*** TEST*** creating fid3 ......\n");

   // Create the function input data
   FunctionInputData *fid3 = new FunctionInputData();
   fid3->Initialize(numStateVars, numControlVars);
   fid3->SetStateVector(newState);
   fid3->SetControlVector(newControl);
   fid3->SetTime(0.534156);
   fid3->SetPhaseNum(1);

   MessageInterface::ShowMessage("*** TEST*** creating bd2 ......\n");

   // Set bounds data
   BoundData *bd2 = new BoundData();
   Rvector sLower(3, -5.0, -5.0, -5.0);
   Rvector sUpper(3,  5.0,  5.0,  5.0);
   Rvector cLower(2, -2.5, -2.5);
   Rvector cUpper(2,  2.5,  2.5);
   Rvector tLower(1,-10.0);
   Rvector tUpper(1, 10.0);
   bd2->SetStateLowerBound(sLower);
   bd2->SetStateUpperBound(sUpper);
   bd2->SetControlLowerBound(cLower);
   bd2->SetControlUpperBound(cUpper);
   bd2->SetTimeLowerBound(tLower);
   bd2->SetTimeUpperBound(tUpper);

   MessageInterface::ShowMessage("*** TEST*** creating manager2 and dummy2 ......\n");

   UserPathFunctionManager *manager2 = new UserPathFunctionManager();
   DummyPathFunction2      *dummy2   = new DummyPathFunction2();
   dummy2->Initialize(fid3, pfc3);
   dummy2->SetAlgFunctionsLowerBounds(controlLower);  // using control values for testing?
   dummy2->SetAlgFunctionsUpperBounds(controlUpper);  // using control values for testing?

   manager2->Initialize(dummy2, fid3, pfc3, bd2);

   PathFunctionContainer *pfc3Res = manager2->EvaluateUserFunction(fid3, pfc3);

   UserFunctionProperties dynProp  = manager2->GetDynFunctionProperties();
   UserFunctionProperties algProp  = manager2->GetAlgFunctionProperties();
   UserFunctionProperties costProp = manager2->GetCostFunctionProperties();

   MessageInterface::ShowMessage("*** TEST*** got dynProp, algProp, costProp ...\n");

   // dynamics:
   Rmatrix dynStateTruth(  2,3, 1.0, 0.0, 1.0,    1.0, 1.0, 0.0);
   Rmatrix dynControlTruth(2,2, 1.0, 1.0,         0.0, 1.0);
   Rmatrix dynTimeTruth(   2,1, 0.0          ,    1.0);
   if (dynProp.GetStateJacobianPattern() != dynStateTruth)
      MessageInterface::ShowMessage("--------- ERROR in dyn func state jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Dyn func state jacobian pattern OK\n");
   if (dynProp.GetControlJacobianPattern() != dynControlTruth)
      MessageInterface::ShowMessage("--------- ERROR in dyn func control jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Dyn func control jacobian pattern OK\n");
   if (dynProp.GetTimeJacobianPattern() != dynTimeTruth)
      MessageInterface::ShowMessage("--------- ERROR in dyn func time jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Dyn func time jacobian pattern OK\n");

   // cost:
   Rmatrix costStateTruth(  1,3, 1.0, 0.0, 1.0);
   Rmatrix costControlTruth(1,2, 1.0, 0.0);
   Rmatrix costTimeTruth(   1,1, 1.0);
   if (costProp.GetStateJacobianPattern() != costStateTruth)
      MessageInterface::ShowMessage("--------- ERROR in cost func state jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Cost func state jacobian pattern OK\n");
   if (costProp.GetControlJacobianPattern() != costControlTruth)
      MessageInterface::ShowMessage("--------- ERROR in cost func control jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Cost func control jacobian pattern OK\n");
   if (costProp.GetTimeJacobianPattern() != costTimeTruth)
      MessageInterface::ShowMessage("--------- ERROR in cost func time jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Cost func time jacobian pattern OK\n");

   // Algebraic
   Rmatrix algStateTruth(  2,3, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0);
   Rmatrix algControlTruth(2,2, 0.0, 1.0, 1.0, 0.0);
   Rmatrix algTimeTruth(   2,1, 1.0,0.0);
   if (algProp.GetStateJacobianPattern() != algStateTruth)
      MessageInterface::ShowMessage("--------- ERROR in alg func state jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Alg func state jacobian pattern OK\n");
   if (algProp.GetControlJacobianPattern() != algControlTruth)
      MessageInterface::ShowMessage("--------- ERROR in alg func control jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Alg func control jacobian pattern OK\n");
   if (algProp.GetTimeJacobianPattern() != algTimeTruth)
      MessageInterface::ShowMessage("--------- ERROR in alg func time jacobian pattern\n");
   else
      MessageInterface::ShowMessage("Alg func time jacobian pattern OK\n");


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

