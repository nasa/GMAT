//------------------------------------------------------------------------------
//                                  TestRealUtil
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
// Created: 2005/02/14
//
/**
 * Test driver for RealUtilities.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <math.h>
#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"             // for ReadStartupFile()

using namespace std;
using namespace GmatMathConstants;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   Real x = 0.0;
   Real result = 0.0;
   Real expResult = 0.0;
   Real tol = 1.0e-11;
   bool bval = false;
   
   out.Put("");

   out.Put("============================== test GmatTimeConstants:: constants");
   Real rval = GmatTimeConstants::SECS_PER_DAY;
   out.Put("GmatTimeConstants::SECS_PER_DAY = ", rval);
   out.Put("");
   
   out.Put("============================== test GmatRealConstants constants");
   rval = GmatRealConstants::REAL_TOL;
   out.Put("GmatRealConstants::REAL_TOL = ", rval);
   out.Put("");
   
   out.Put("============================== test GmatPhysicalConstants constants");
   rval = GmatPhysicalConstants::c;
   out.Put("GmatPhysicalConstants::c = ", rval);
   out.Put("");
   
   out.Put("============================== test RealUtilities");
   out.Put("");
   
   out.Put("========================= Tan(90)");
   out.Put(Tan(90.0 * RAD_PER_DEG));
   out.Put("");
   
   out.Put("========================= Ln(e)");
   out.Put(Ln(E));
   out.Put("");
   
   out.Put("========================= Log(e)");
   out.Put(Log(E));
   out.Put("");
   
   out.Put("========================= Ln(10)");
   out.Put(Ln(10.0));
   out.Put("");
   
   out.Put("========================= Log(10)");
   out.Put(Log(10.0));
   out.Put("");
   
   out.Put("========================= Log(10, E)");
   out.Put(Log(10.0, E));
   out.Put("");
   
   out.Put("========================= Log10(10)");
   out.Put(Log10(10.0));
   out.Put("");
   
   out.Put("========================= Log(10, 10)");
   out.Put(Log(10.0, 10.0));
   out.Put("");
   
   out.Put("========================= Log(100, 10)");
   out.Put(Log(100.0, 10.0));
   out.Put("");
   
   out.Put("========================= Exp(2)");
   out.Put(Exp(2.0));
   out.Put("");
   
   out.Put("========================= Exp10(2)");
   out.Put(Exp10(2.0));
   out.Put("");
   
   out.Put("========================= Pow(2, 3)");
   out.Put(Pow(2.0, 3.0));
   out.Put("");
   
   x = 1.0000000000000002;
   out.Put("========================= Acos(1.0000000000000002)");
   out.Put(ACos(x, tol));
   out.Put("");
   
   x = -1.0000000000000002;
   out.Put("========================= Acos(-1.0000000000000002)");
   out.Put(ACos(x, tol));
   out.Put("");
   
   bval = IsNaN(sqrt(-1.0));
   out.Put("========================= IsNaN(Sqrt(-1))");
   out.Validate(bval, true);
   
   bval = IsInf(1e+1000);
   out.Put("========================= IsInf(1e+1000)");
   out.Validate(bval, true);
   
   try
   {
      x = -1.0000000000000002;
      out.Put("========================= Acos(-1.0000000000000002)");
      out.Put(ACos(x));
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   
   try
   {
      x = 1.00000000002;
      out.Put("========================= Acos(1.00000000002)");
      out.Put(ACos(x, tol));
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   try
   {
      x = -1.00000000002;
      out.Put("========================= Acos(-1.00000000002)");
      out.Put(ACos(x, tol));
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   try
   {
      x = -1.00000000002;
      out.Put("========================= acos(-1.00000000002)");
      out.Put(acos(x));
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      x = 2.2;
      expResult = 1.425416943070613;
      out.Put("========================= Acosh(2.2)");
      result = ACosh(x);
      out.Validate(result, expResult);
      out.Put("");
      
      x = 1.0;
      expResult = 0.0;
      out.Put("========================= Acosh(1.0)");
      result = ACosh(x);
      out.Validate(result, expResult);
      out.Put("");
      
      x = 0.999999999999999;
      expResult = 0.0;
      out.Put("========================= Acosh(0.999999999999999) should throw an exception with MSVC++, nan with GCC");
      result = ACosh(x);
      out.Validate(result, expResult);
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      //===== Test invalid cycle in radians
      x = 2.2;
      expResult = 1.425416943070613;
      out.Put("========================= Acosh(2.2, 0.0)");
      result = ACosh(x, 0.0);
      out.Validate(result, expResult);
      out.Put("");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   
   return 1;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   #if 0
   std::string startupFile = "gmat_startup_file.txt";
   FileManager *fm = FileManager::Instance();
   fm->ReadStartupFile(startupFile);
   #endif
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   //std::string outPath = "../../TestRealUtil/";
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestRealUtilOut.txt";
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of RealUtilities!!");
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
