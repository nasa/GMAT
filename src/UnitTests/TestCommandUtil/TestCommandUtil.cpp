//------------------------------------------------------------------------------
//                               TestCommandUtil
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
// Created: 2009.09.03
//
/**
 * Purpose:
 *   Unit test driver of CommandUtil class.
 *
 * Output file:
 *   TestCommandUtilOut.txt
 *
 * Description of CommandUtil:
 *   The CommandUtil provides various functions to access command sequence, such
 *   as getting first command or last command in the sequence, finding matching
 *   End command, etc.
 * 
 * Test Procedure:
 *   1. Create various commands and append it to the first command NoOp.
 *
 * Validation method:
 *   1. The test driver is used for validating the functions
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "NoOp.hpp"
#include "Propagate.hpp"
#include "Optimize.hpp"
#include "EndOptimize.hpp"
#include "Vary.hpp"
#include "Stop.hpp"
#include "Report.hpp"
#include "CommandUtil.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   //---------------------------------------------------------------------------
   out.Put("========================= Test creating command");
   GmatCommand *top = new NoOp();
   GmatCommand *propagate = new Propagate();
   GmatCommand *optimize = new Optimize();
   GmatCommand *vary1 = new Vary();
   GmatCommand *vary2 = new Vary();
   GmatCommand *stop = new Stop();
   GmatCommand *endOptimize = new EndOptimize();
   GmatCommand *report = new Report();
   
   GmatCommand *cmd = NULL;
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test appending to NoOp");
   top->Append(propagate);
   top->Append(optimize);
   optimize->Append(vary1);
   optimize->Append(vary2);
   optimize->Append(stop);
   optimize->Append(endOptimize);
   top->Append(report);
   
   Integer id;
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetCommandSeqString()");
   std::string cmdStr = GmatCommandUtil::GetCommandSeqString(top);
   out.Put(cmdStr);
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetFirstCommand()");
   cmd = GmatCommandUtil::GetFirstCommand(report);
   out.Validate(cmd->GetTypeName(), "NoOp");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetLastCommand()");
   cmd = GmatCommandUtil::GetLastCommand(top);
   out.Validate(cmd->GetTypeName(), "Report");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetLastCommand()");
   cmd = GmatCommandUtil::GetLastCommand(optimize);
   out.Validate(cmd->GetTypeName(), "Report");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetParentCommand()");
   cmd = GmatCommandUtil::GetParentCommand(top, stop);
   out.Validate(cmd->GetTypeName(), "Optimize");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetParentCommand()");
   cmd = GmatCommandUtil::GetParentCommand(top, vary1);
   out.Validate(cmd->GetTypeName(), "Optimize");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetParentCommand()");
   cmd = GmatCommandUtil::GetParentCommand(top, vary2);
   out.Validate(cmd->GetTypeName(), "Optimize");
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test deleting command sequence");
   out.Put("========================= Deleting top will delete whole sequence");
   out.Put("deleting top");
   delete top;
   
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestCommandUtil/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestCommandUtilOut.txt";
   TestOutput out(outFile);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of CommandUtil!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
