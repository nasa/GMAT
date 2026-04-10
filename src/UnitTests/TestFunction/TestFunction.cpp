//------------------------------------------------------------------------------
//                                  TestFunction
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
// Created: 2008/08/19
//
/**
 * Test driver for GmatFunction.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatStringUtil;


//------------------------------------------------------------------------------
// int Factorial(int n)
//------------------------------------------------------------------------------
int Factorial(int n)
{
   if (n <= 1)
      return 1;
   else
      return n * Factorial(n-1);
   
}

//------------------------------------------------------------------------------
// int FactorialA(int n)
//------------------------------------------------------------------------------
int FactorialA(int n)
{
   if (n <= 1)
   {
      return 1;
   }
   else
   {
      int tmp = Factorial(n-1);
      return n * tmp;
   }
}

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   MessageInterface::SetLogFile("../../TestFunction/GmatLog.txt");
   MessageInterface::ShowMessage("=========== TestFunction\n");
   
   Integer ival, expIval;
   
   out.Put("");
   
   out.Put("============================== test Factorial(2)");
   //---------------------------------------------
   
   ival = Factorial(2);
   expIval = 2;
   out.Validate(ival, expIval);
   
   out.Put("============================== test Factorial(6)");
   //---------------------------------------------
   
   ival = Factorial(6);
   expIval = 720;
   out.Validate(ival, expIval);
   
   out.Put("============================== test FactorialA(6)");
   //---------------------------------------------
   
   ival = FactorialA(6);
   expIval = 720;
   out.Validate(ival, expIval);
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outPath = "../../TestFunction/";
   std::string outFile = outPath + "TestFunctionOut.txt";   
   TestOutput out(outFile);
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of GMAT Function!!");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
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
