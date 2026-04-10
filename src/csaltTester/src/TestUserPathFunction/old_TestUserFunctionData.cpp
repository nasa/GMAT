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
 * Test driver for UserFunctionData classes.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
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
#include "UserPathFunctionData.hpp"
#include "UserPointFunctionData.hpp"

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
   
   IntegerArray sIdxs;
   IntegerArray cIdxs;
   
   for (Integer ii = 0; ii < 9; ii++)
      sIdxs.push_back(ii);
   for (Integer ii = 0; ii < 12; ii++)
      cIdxs.push_back(ii * 2);
   
   UserPathFunctionData  *ufdPath;
   UserPointFunctionData *ufdPoint;
   try
   {
      ufdPath = new UserPathFunctionData();
      MessageInterface::ShowMessage(
                  "SUCCESSfully created a UserPathFunctionData object!!!!\n");
      bool hasSJ = ufdPath->HasUserStateJacobian();
      bool hasCJ = ufdPath->HasUserControlJacobian();
      bool hasTJ = ufdPath->HasUserTimeJacobian();
      MessageInterface::ShowMessage("   hasUserStateJacobian   = %s\n",
                                    (hasSJ? "true":"false"));
      MessageInterface::ShowMessage("   hasUserControlJacobian = %s\n",
                                    (hasCJ? "true":"false"));
      MessageInterface::ShowMessage("   hasUserTimeJacobian    = %s\n",
                                    (hasTJ? "true":"false"));
      
      ufdPath->SetNLPData(2,3,sIdxs, cIdxs);
      
      IntegerArray sIdxsOut;
      IntegerArray cIdxsOut;
      
      MessageInterface::ShowMessage("   meshIndex is: %d\n",
                                    ufdPath->GetMeshIdx());
      MessageInterface::ShowMessage("   stageIndex is: %d\n",
                                    ufdPath->GetStageIdx());
      sIdxsOut = ufdPath->GetStateIdxs();
      cIdxsOut = ufdPath->GetControlIdxs();
      
      for (Integer ii = 0; ii < sIdxsOut.size(); ii++)
         MessageInterface::ShowMessage("state index %d = %d\n",
                                       ii, sIdxsOut.at(ii));
      for (Integer ii = 0; ii < cIdxsOut.size(); ii++)
         MessageInterface::ShowMessage("control index %d = %d\n",
                                       ii, cIdxsOut.at(ii));
      Rmatrix testMatrix(3,4,
                         0.0, 1.0, 2.0, 3.0,
                         0.1, 1.1, 2.1, 3.1,
                         0.2, 1.2, 2.2, 3.2);
      
      ufdPath->SetStateJacobian(testMatrix);
      testMatrix *= 0.5;
      ufdPath->SetControlJacobian(testMatrix);
      testMatrix *= 4.0;
      ufdPath->SetTimeJacobian(testMatrix);
      
      Rmatrix outMatrix;
      outMatrix = ufdPath->GetStateJacobian();
      MessageInterface::ShowMessage("  state jacobian = \n%s\n",
                                    outMatrix.ToString().c_str());
      outMatrix = ufdPath->GetControlJacobian();
      MessageInterface::ShowMessage("  control jacobian = \n%s\n",
                                    outMatrix.ToString().c_str());
      outMatrix = ufdPath->GetTimeJacobian();
      MessageInterface::ShowMessage("  time jacobian = \n%s\n",
                                    outMatrix.ToString().c_str());
      hasSJ = ufdPath->HasUserStateJacobian();
      hasCJ = ufdPath->HasUserControlJacobian();
      hasTJ = ufdPath->HasUserTimeJacobian();
      MessageInterface::ShowMessage("   NOW, hasUserStateJacobian   = %s\n",
                                    (hasSJ? "true":"false"));
      MessageInterface::ShowMessage("   NOW, hasUserControlJacobian = %s\n",
                                    (hasCJ? "true":"false"));
      MessageInterface::ShowMessage("   NOW, hasUserTimeJacobian    = %s\n",
                                    (hasTJ? "true":"false"));
      
      ufdPoint = new UserPointFunctionData();
      
      MessageInterface::ShowMessage(
                  "SUCCESSfully created a UserPointFunctionData object!!!!\n");
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
   if (ufdPath)  delete ufdPath;
   if (ufdPoint) delete ufdPoint;
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
