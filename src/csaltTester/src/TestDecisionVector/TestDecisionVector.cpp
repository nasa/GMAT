//------------------------------------------------------------------------------
//                               TestDecisionVector
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
// Author: Linda Jun
// Created: 2015.06.30
//
/**
 * Test driver for DecisionVector class.
 * Adapted from DecisionVectorTestBetts.m
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <cstdlib>
#include "gmatdefs.hpp"
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
#include "DecVecTypeBetts.hpp"

using namespace std;

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
//   std::string outFile = outPath + "TestRmatrixOut.txt";
//   TestOutput out(outFile);
//   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
//      printf("Current OS is %s\n", buffer);
      MessageInterface::ShowMessage("Current oS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }
   
   // Configure the test vector
   // Properties of the state/controls for continuous optimal control
   // problem
   Integer numStates     = 3;
   Integer numControls   = 2;
   Integer numIntegrals  = 10;
   Integer numStaticsNLP = 12;
   Real    time0         = -12345;
   Real    timef         = 12345;

   // Properties of the discretization of the state and controls
   Integer numStateMeshPoints    = 4;
   Integer numControlMeshPoints  = 3;
   Integer numStateStagePoints   = 2;
   Integer numControlStagePoints = 2;
   Integer numControlPoints      = -999;
   Integer numStatePoints   = (numStateMeshPoints-1)*(1+numStateStagePoints)+1;
   if (numStateMeshPoints  == numControlMeshPoints)
      numControlPoints = (numControlMeshPoints-1)*(1+numControlStagePoints)+1;
   else
      numControlPoints = (numControlMeshPoints)*(1+numControlStagePoints);

   Real value = 0;
   Integer vecIdx = 0;

   // Build a dummy V matrix in Eq. 40.
   // V = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
   //     [X2 Y2 Z2 VX2 VY2 VZ2 M2]
   //     [XN YN ZN VXN VYN VZN M1]
   Rmatrix V(numStatePoints,numStates);
   Integer szState = numStatePoints * numStates;
   Rvector zState(szState);
   vecIdx = 0;
   for (Integer colIdx = 0; colIdx < numStates; colIdx++)
   {
      for (Integer rowIdx = 0; rowIdx < numStatePoints; rowIdx++)
      {
         V(rowIdx,colIdx)   = rowIdx + (0.1*colIdx);
         zState(vecIdx++)   = V(rowIdx,colIdx);  // want column vector
      }
   }
   MessageInterface::ShowMessage("V = %s\n", V.ToString().c_str());
   MessageInterface::ShowMessage("zState = %s\n",
                                 (zState.ToString(outFormat, 1)).c_str());
   
   // Build a dummy W matrix in Eq. 41.
   // W = [Ux1 Uy1 Uz1]
   //     [Ux2 Uy2 Uz2]
   //     [UxN UyN UzN]
//   Real value = 200.0;
   Rmatrix W(numControlPoints, numControls);
   Integer szControl = numControlPoints * numControls;
   // Need this to be a column vector
   Rvector zControl(szControl);
   vecIdx = 0;
   for (Integer colIdx = 0; colIdx < numControls; colIdx++)
   {
      for (Integer rowIdx = 0; rowIdx < numControlPoints; rowIdx++)
      {
//         value = value + 1;
         W(rowIdx,colIdx)   = rowIdx + (0.1*colIdx) + 1000;
         zControl(vecIdx++) = W(rowIdx,colIdx);  // want column vector
      }
   }
   MessageInterface::ShowMessage("W = %s\n", W.ToString().c_str());
   MessageInterface::ShowMessage("zControl = %s\n",
                                 (zControl.ToString(outFormat, 1)).c_str());

   // Build a dummy static Vector
   value = 101;
   Rvector zStatic(numStaticsNLP);
   for (Integer rowIdx = 0; rowIdx < numStaticsNLP; rowIdx++)
   {
      zStatic(rowIdx) = value;
      value++;
   }
   MessageInterface::ShowMessage("zStatic = %s\n",
                                 (zStatic.ToString(outFormat, 1)).c_str());
   
   // Build a dummy integral Vector
   value = 501;
   Rvector zIntegral(numIntegrals);
   for (Integer rowIdx = 0; rowIdx < numIntegrals; rowIdx++)
   {
      zIntegral(rowIdx) = value;
      value++;
   }
   MessageInterface::ShowMessage("zIntegral = %s\n",
                                 (zIntegral.ToString(outFormat, 1)).c_str());
   
   // Build a dummy time vector
   Rvector zTime(2, time0, timef);
   
   // Pack it all into a big ol' vector
   Rvector stateVector(2 + szState + szControl + numStaticsNLP + numIntegrals);
   Integer theIdx           = 0;
   stateVector(theIdx++)    = zTime(0);
   stateVector(theIdx++)    = zTime(1);
   for (Integer ii = 0; ii < szState; ii++)
      stateVector(theIdx++) = zState(ii);
   for (Integer ii = 0; ii < szControl; ii++)
      stateVector(theIdx++) = zControl(ii);
   for (Integer ii = 0; ii < numStaticsNLP; ii++)
      stateVector(theIdx++) = zStatic(ii);
   for (Integer ii = 0; ii < numIntegrals; ii++)
      stateVector(theIdx++) = zIntegral(ii);
   
   DecVecTypeBetts *myVector;
   try
   {
//      RunTest(out);
      myVector = new DecVecTypeBetts();
      myVector->Initialize(numStates, numControls, numIntegrals,
                           numStaticsNLP, numStateMeshPoints,
                           numControlMeshPoints, numStateStagePoints,
                           numControlStagePoints);
      MessageInterface::ShowMessage(
                        "Successfully created and initialized "
                        "a DecVecTypeBetts!!!\n");
      
      /*
       %% Test setting decision parameters by type
       disp(' The following tests failed ')
       %  Define interpolation during initialization
       myVector.SetInterplotionType('Lagrange')
       %  Send in the time vector for the nominal points, add new data member
       %  called nominalTimeVector and set it during initialization.
       myVector.SetTimeVector(nominalTimeVector)
       %  Interplate state to new times
       stateOut = myVector.InterpolateState(newTimeVector);
       %  Interplate control to new times
       controlOut = myVector.InterpolateControl(newTimeVector);
       */
      myVector->SetTimeVector(zTime);
      myVector->SetStaticVector(zStatic);
      myVector->SetIntegralVector(zIntegral);
      
      // Time parameters
      Rvector testTime = myVector->GetTimeVector();
      if (testTime.GetSize() != zTime.GetSize())
         MessageInterface::ShowMessage(
                           "Time received is of wrong dimension!!!\n");
      for (Integer ii = 0; ii < zTime.GetSize(); ii++)
      {
         if (abs(testTime(ii) - zTime(ii)) > tolerance)
         {
            MessageInterface::ShowMessage("*** ERROR *** time get/set failed!!!\n");
            break;
         }
      }
      
      // Static parameters
      Rvector testStatic = myVector->GetStaticVector();
      if (testStatic.GetSize() != zStatic.GetSize())
         MessageInterface::ShowMessage(
                           "*** ERROR *** Static received is of wrong dimension!!!\n");
      for (Integer ii = 0; ii < zStatic.GetSize(); ii++)
      {
         if (abs(testStatic(ii) - zStatic(ii)) > tolerance)
         {
            MessageInterface::ShowMessage("*** ERROR *** static get/set failed!!!\n");
            break;
         }
      }
      
      // Integral parameters
      Rvector testIntegral = myVector->GetIntegralVector();
      if (testIntegral.GetSize() != zIntegral.GetSize())
         MessageInterface::ShowMessage(
                           "*** ERROR *** Integral received is of wrong dimension!!!\n");
      for (Integer ii = 0; ii < zIntegral.GetSize(); ii++)
      {
         if (abs(testIntegral(ii) - zIntegral(ii)) > tolerance)
         {
            MessageInterface::ShowMessage("*** ERROR *** integral get/set failed!!!\n");
            break;
         }
      }
      
      // state vector
      Rvector testSVin(3, 10.0, 11.0, 12.0);
      myVector->SetStateVector(0,2,testSVin);
      MessageInterface::ShowMessage("State Vector has been set\n");
      Rvector decVec = myVector->GetDecisionVector();
      if (decVec.GetSize() < 15)
         MessageInterface::ShowMessage(
                           "*** ERROR *** decision vec received is of wrong dimension!!!\n");
      else
         MessageInterface::ShowMessage(
                           "decision vector is of correct dimension (%d)\n",
                           decVec.GetSize());
      for (Integer ii = 0; ii < 3; ii++)
      {
         MessageInterface::ShowMessage(
                           "Comparing decVec(%12.10f) with testSVin(%12.10f)\n",
                           decVec(ii+12), testSVin(ii));
         if (abs(decVec(ii+12) - testSVin(ii)) > tolerance)
         {
            MessageInterface::ShowMessage(
                              "*** ERROR *** decision vector get/set failed!!!\n");
            break;
         }
      }
      
      for (Integer ii = 0; ii < decVec.GetSize(); ii++)
      {
         MessageInterface::ShowMessage(" decVec[%d] = %12.10f\n",
                                       ii, decVec(ii));
      }
      
      // This causes an Abort for some reason ...
//      MessageInterface::ShowMessage("DecisionVector = %s\n",
//                                    decVec.ToString(outFormat, 3).c_str());
      Rvector testSVout = myVector->GetStateVector(0,2);
      if (testSVout.GetSize() != testSVin.GetSize())
         MessageInterface::ShowMessage(
                           "*** ERROR *** state vec received is of wrong dimension!!!\n");
      for (Integer ii = 0; ii < testSVout.GetSize(); ii++)
      {
         if (abs(testSVout(ii) - testSVin(ii)) > tolerance)
         {
            MessageInterface::ShowMessage(
                              "*** ERROR *** state vector get/set failed!!!\n");
            break;
         }
      }
      
      MessageInterface::ShowMessage("testSVout = %s\n",
                                    testSVout.ToString(outFormat, 1).c_str());

      // state Array
      myVector->SetStateArray(V);
      MessageInterface::ShowMessage("We have set the state array --------\n");
      MessageInterface::ShowMessage("We will now get the state array --------\n");
      Rmatrix testM = myVector->GetStateArray();
      Integer r, c, rV, cV;
      testM.GetSize(r,c);
      V.GetSize(rV,cV);
      MessageInterface::ShowMessage("For state array, r = %d, and c = %d\n",
                                    r, c);
      MessageInterface::ShowMessage("For state array, r(V) = %d, and c(V) = %d\n",
                                    rV, cV);
      if ((r != numStatePoints) || (c != numStates))
      {
         MessageInterface::ShowMessage(
                           "*** ERROR *** state array received is of wrong dimension!!!\n");
      }
      else
      {
         MessageInterface::ShowMessage(
                           "state array received is of correct dimension!!!\n");
      }
      for (Integer rr = 0; rr < r; rr++)
      {
         for (Integer cc = 0; cc < c; cc++)
         {
            if (abs(V(rr,cc) - testM(rr,cc)) > tolerance)
            {
               MessageInterface::ShowMessage(
                                 "*** ERROR *** state array get/set failed!!!\n");
               break;
            }
         }
      }
      MessageInterface::ShowMessage("We have checked the state array --------\n");
      MessageInterface::ShowMessage("We will set the control vector ----\n");

      // control vector
      Rvector testCVin(2,555.1, 555.2);
      myVector->SetControlVector(0,2,testCVin);
      MessageInterface::ShowMessage("Control Vector has been set\n");
      Rvector decVec2 = myVector->GetDecisionVector();
      if (decVec2.GetSize() < 17)
         MessageInterface::ShowMessage(
                           "*** ERROR *** decision vec received is of wrong dimension!!!\n");
      else
         MessageInterface::ShowMessage(
                           "decision vector is of correct dimension (%d)\n",
                           decVec2.GetSize());
      for (Integer ii = 0; ii < 2; ii++)
      {
         MessageInterface::ShowMessage(
                           "Comparing decVec(%12.10f) with testCVin(%12.10f)\n",
                           decVec2(ii+15), testCVin(ii));
         if (abs(decVec2(ii+15) - testCVin(ii)) > tolerance)
         {
            MessageInterface::ShowMessage(
                                          "*** ERROR *** decision vector get/set failed!!!\n");
            break;
         }
      }
      MessageInterface::ShowMessage("We will get the control vector ----\n");
      Rvector testCVout = myVector->GetControlVector(0,2);
      if (testCVout.GetSize() != testCVin.GetSize())
         MessageInterface::ShowMessage(
                           "*** ERROR *** state vec received is of wrong dimension!!!\n");
      for (Integer ii = 0; ii < testCVout.GetSize(); ii++)
      {
         if (abs(testCVout(ii) - testCVin(ii)) > tolerance)
         {
            MessageInterface::ShowMessage(
                              "*** ERROR *** control vector get/set failed!!!\n");
            break;
         }
      }
      
      MessageInterface::ShowMessage("We will set the control array ----\n");
      // control Array
      myVector->SetControlArray(W);
      MessageInterface::ShowMessage("We will get the control vector ----\n");
      Rmatrix testM2 = myVector->GetControlArray();
      testM2.GetSize(r,c);
      MessageInterface::ShowMessage("For control array, r = %d, and c = %d\n",
                             r, c);
      if ((r != numControlPoints) || (c != numControls))
      {
         MessageInterface::ShowMessage(
                           "*** ERROR *** control array received is of wrong dimension!!!\n");
      }
      for (Integer rr = 0; rr < r; rr++)
      {
         for (Integer cc = 0; cc < c; cc++)
         {
            if (abs(W(rr,cc) - testM2(rr,cc)) > tolerance)
            {
               MessageInterface::ShowMessage(
                                 "*** ERROR *** control array get/set failed!!!\n");
               break;
            }
         }
      }
      MessageInterface::ShowMessage("Done with control vector ----\n");
   
      // Check getting state at mesh points and stage points
      Integer meshIdx  = 4;
      Integer stageIdx = 0;
      Rvector stateAtMeshPoint = myVector->GetStateAtMeshPoint(meshIdx,stageIdx);
      Rmatrix stateArray = myVector->GetStateArray();
      Integer stateRows, stateCols;
      stateArray.GetSize(stateRows, stateCols);
//      for (Integer rr = 0; rr < stateRows; rr++)
//      {
         for (Integer cc= 0; cc < stateCols; cc++)
         {
            MessageInterface::ShowMessage(
                                           "stateAtMeshPoint(%d) = %12.10f AND ",
                                           cc, stateAtMeshPoint(cc));
            MessageInterface::ShowMessage(
                                           "stateArray(9,%d) = %12.10f\n",
                                           cc, stateArray(9,cc));
         }
//      }
//      stateAtMeshPoint - stateArray(10,:)'
      
      meshIdx = 3;
      stageIdx = 2;
      stateAtMeshPoint = myVector->GetStateAtMeshPoint(meshIdx,stageIdx);
      stateArray = myVector->GetStateArray();
      stateArray.GetSize(stateRows, stateCols);
//      stateAtMeshPoint - stateArray(9,:)'
      for (Integer cc= 0; cc < stateCols; cc++)
      {
         MessageInterface::ShowMessage(
                                       "stateAtMeshPoint(%d) = %12.10f AND ",
                                       cc, stateAtMeshPoint(cc));
         MessageInterface::ShowMessage(
                                       "stateArray(8,%d) = %12.10f\n",
                                       cc, stateArray(8,cc));
      }
      // Check getting control at mesh points and stage points
      meshIdx  = 3;
      stageIdx = 0;
      Integer controlRows, controlCols;
      Rvector controlAtMeshPoint = myVector->GetControlAtMeshPoint(
                                             meshIdx,stageIdx);
      Rmatrix controlArray = myVector->GetControlArray();
      controlArray.GetSize(controlRows, controlCols);
      for (Integer cc= 0; cc < controlCols; cc++)
      {
         MessageInterface::ShowMessage(
                                       "controlAtMeshPoint(%d) = %12.10f AND ",
                                       cc, controlAtMeshPoint(cc));
         MessageInterface::ShowMessage(
                                       "controlArray(6,%d) = %12.10f\n",
                                       cc, controlArray(6,cc));
      }
//      controlAtMeshPoint - controlArray(5,:)'
      
      meshIdx  = 2;
      stageIdx = 1;
      controlAtMeshPoint = myVector->GetControlAtMeshPoint(meshIdx,stageIdx);
      controlArray = myVector->GetControlArray();
      controlArray.GetSize(controlRows, controlCols);
      for (Integer cc= 0; cc < controlCols; cc++)
      {
         MessageInterface::ShowMessage(
                                       "controlAtMeshPoint(%d) = %12.10f AND ",
                                       cc, controlAtMeshPoint(cc));
         MessageInterface::ShowMessage(
                                       "controlArray(4,%d) = %12.10f\n",
                                       cc, controlArray(4,cc));
      }
//      controlAtMeshPoint - controlArray(4,:)'
      
//      // Test getting components of decision vector
//      stateSubVector    = myVector.GetStateSubVector() - zState
//      controlSubVector  = myVector.GetControlSubVector() - zControl
//      integralSubVector = myVector.GetIntegralSubVector() - zIntegral
//      staticSubVector   = myVector.GetStaticSubVector() - zStatic
//      timeSubVector     = myVector.GetTimeSubVector() - [time0;timef]
//      stateArray        = myVector.GetStateArray() - V
//      controlArray      = myVector.GetControlArray() - W
      
      // Test setting components of decision vector
      myVector->SetStateArray(V);
      Rvector dv = myVector->GetDecisionVector();
      Integer szSV = stateVector.GetSize();
      Integer szDV = dv.GetSize();
      if (szSV != szDV)
         MessageInterface::ShowMessage("*** ERROR *** SIZES not the SAME!!!\n");
      else
         MessageInterface::ShowMessage("SIZES ARE the SAME!!!\n");
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting state array:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
      
////      stateVector - myVector.decisionVector
//      myVector->SetStateVector(zState);
//      dv = myVector->GetDecisionVector();
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting zState:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
////      stateVector - myVector.decisionVector
      
      myVector->SetControlArray(W);
      dv = myVector->GetDecisionVector();
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting control array:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
////      stateVector - myVector.decisionVector
//      myVector->SetControlVector(zControl);
//      dv = myVector->GetDecisionVector();
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting zControl:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
////      stateVector - myVector.decisionVector
      
      myVector->SetStaticVector(zStatic);
      dv = myVector->GetDecisionVector();
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting zStatic:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
//      stateVector - myVector.decisionVector
      
      myVector->SetIntegralVector(zIntegral);
      dv = myVector->GetDecisionVector();
//      MessageInterface::ShowMessage("(stateVector - decisionVector) AFTER setting zIntegral:\n");
//      for (Integer ii = 0; ii < szDV; ii++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       ii, stateVector(ii) - dv(ii));
//      }
//      stateVector - myVector.decisionVector
      
      MessageInterface::ShowMessage("*** stateVector::::::\n");
      for (Integer ii = 0; ii < szSV; ii++)
         MessageInterface::ShowMessage("   stateVector[%d] = %12.10f\n",
                                       ii, stateVector[ii]);
      MessageInterface::ShowMessage("*** decisionVector::::::\n");
      for (Integer ii = 0; ii < szDV; ii++)
         MessageInterface::ShowMessage("   decisionVector[%d] = %12.10f\n",
                                       ii, dv[ii]);
      
      
//      Rvector stateVec = myVector->GetFirstStateVector();
//      Rvector stateAtMP = myVector->GetStateAtMeshPoint(1);  // 0?
//      MessageInterface::ShowMessage("(stateVec - stateAtMP) for FIRST:\n");
//      for (Integer jj = 0; jj < stateVec.GetSize(); jj++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       jj, stateVec(jj) - stateAtMP(jj));
//      }
////      stateVec - stateAtMeshPoint
//      
//      stateVec = myVector->GetLastStateVector();
//      stateAtMP = myVector->GetStateAtMeshPoint(numStatePoints); // -1?
//      MessageInterface::ShowMessage("(stateVec - stateAtMP) for LAST:\n");
//      for (Integer jj = 0; jj < stateVec.GetSize(); jj++)
//      {
//         MessageInterface::ShowMessage("      %d:  %12,10f\n",
//                                       jj, stateVec(jj) - stateAtMP(jj));
//      }
////      stateVec - stateAtMeshPoint
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(
              "Oh, dear!!!  There was an exception!!!\n");
      MessageInterface::ShowMessage("Full messsage is: %s\n",
                                    e.GetFullMessage().c_str());
//      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      MessageInterface::ShowMessage(
      "Uh-oh!!!  There was an UNKNOWN exception!!!\n");
//      out.Put("Unknown error occurred\n");
   }
   if (myVector) delete myVector;
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
