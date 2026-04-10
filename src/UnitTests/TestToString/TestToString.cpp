//------------------------------------------------------------------------------
//                                  TestToString
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
// Created: 2007/01/16
//
/**
 * This test driver tests ToString().
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "GmatGlobal.hpp"
#include "StringUtil.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;

//------------------------------------------------------------------------------
// void WriteString(TestOutput &out, Real rval, int testCase)
//------------------------------------------------------------------------------
void WriteString(TestOutput &out, Real rval, int testCase)
{
   GmatGlobal *global = GmatGlobal::Instance();
   std::string str;
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::ToString()\n");
   out.Put("---------- using precision = 25");
   str = GmatStringUtil::ToString(rval, 25);
   if (testCase == 1)
      out.Validate(str, "123456789.1234567900000000");
   else if (testCase == 2)
      out.Validate(str, "0.1234567890123456800000000");
   else if (testCase == 3)
      out.Validate(str, "1.234567890123000000000000e-010");
   
   out.Put("---------- using current format");
   str = GmatStringUtil::ToString(rval);
   if (testCase == 1)
      out.Validate(str, "123456789.1234568");
   else if (testCase == 2)
      out.Validate(str, "0.1234567890123457");
   else if (testCase == 3)
      out.Validate(str, "1.234567890123000e-010");
   
   out.Put("---------- using default format");
   str = GmatStringUtil::ToString(rval, false);
   if (testCase == 1)
      out.Validate(str, "123456789.1234568");
   else if (testCase == 2)
      out.Validate(str, "0.1234567890123457");
   else if (testCase == 3)
      out.Validate(str, "1.234567890123000e-010");

   out.Put("---------- using scientific format");
   str = GmatStringUtil::ToString(rval, false, true);
   if (testCase == 1)
      out.Validate(str, "1.2345678912345679e+008");
   else if (testCase == 2)
      out.Validate(str, "1.2345678901234568e-001");
   else if (testCase == 3)
      out.Validate(str, "1.2345678901230000e-010");
   
   out.Put("---------- using own format p=20, w=20");
   str = GmatStringUtil::ToString(rval, false, false, 20, 20);
   if (testCase == 1)
      out.Validate(str, "123456789.12345679000");
   else if (testCase == 2)
      out.Validate(str, "0.12345678901234568000");
   else if (testCase == 3)
      out.Validate(str, "1.2345678901230000000e-010");
   
   out.Put("---------- using own format p=30, w=25");
   str = GmatStringUtil::ToString(rval, false, false, 30, 25);
   if (testCase == 1)
      out.Validate(str, "123456789.123456790000000000000");
   else if (testCase == 2)
      out.Validate(str, "0.123456789012345680000000000000");
   else if (testCase == 3)
      out.Validate(str, "1.23456789012300000000000000000e-010");
   
   out.Put("============================== test Rvector3::ToString()");
   Rvector3 r1(rval, rval, rval);
   
   cout << r1;
   
   out.Put("---------- using precision = 25");
   out.Put(r1.ToString(25));
   out.Put("");
   
   out.Put("---------- using current format");
   out.Put(r1.ToString());
   out.Put("");
   
   out.Put("---------- set global format to scientific, spacing to 2");
   global->SetScientific(true);
   global->SetSpacing(3);
   
   out.Put("---------- using current format");
   out.Put(r1.ToString());
   out.Put("");
   
   out.Put("---------- using default format");
   out.Put(r1.ToString(false));
   out.Put("");
   
   out.Put("---------- using scientific format");
   out.Put(r1.ToString(false, true));
   out.Put("");
   
   out.Put("---------- using own format p=20, w=20");
   out.Put(r1.ToString(false, false, 20, 20));
   out.Put("");
   
   out.Put("---------- using own format p=30, w=25");
   out.Put(r1.ToString(false, false, 30, 25));
   out.Put("");
   
   out.Put("---------- using current format, vertical");
   global->SetHorizontal(false);
   out.Put(r1.ToString());
   out.Put("");
   
   out.Put("---------- using own format, scientific, 15, 15, vertical");
   out.Put(r1.ToString(false, true, 15, 15, false));
   out.Put("");
   
   out.Put("---------- using own format, scientific, 20, 15, vertical");
   out.Put(r1.ToString(false, true, 20, 15, false));
   out.Put("");
   
   out.Put("---------- using own format, scientific, 15, 25, vertical");
   out.Put(r1.ToString(false, true, 15, 25, false));
   out.Put("");
   
   out.Put("============================== test Rvector6::ToString()");
   Rvector6 r2(rval, rval, rval, rval, rval, rval);
   
   cout << r2;
   
   out.Put("---------- using precision = 25");
   out.Put(r2.ToString(25));
   out.Put("");
   
   out.Put("---------- using current format");
   out.Put(r2.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical");
   global->SetHorizontal(false);
   out.Put(r2.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical, prefix $$$");
   global->SetPrefix("$$$");
   out.Put(r2.ToString());
   out.Put("");
   
   out.Put("============================== test Rvector::ToString()");
   Rvector r3(4, rval, rval, rval, rval);
   
   cout << r3;
   
   out.Put("---------- using precision = 25");
   out.Put(r3.ToString(25));
   out.Put("");
   
   out.Put("---------- using current format");
   out.Put(r3.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical");
   global->SetHorizontal(false);
   out.Put(r3.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical, prefix ===>");
   global->SetPrefix("===>");
   out.Put(r3.ToString());
   out.Put("");
   
   out.Put("============================== test Rmatrix33::ToString()");
   Rmatrix33 m1(rval, rval, rval, rval, rval, rval, rval, rval, rval);
   
   cout << m1;
   
   out.Put("---------- using precision = 25");
   out.Put(m1.ToString(25));
   out.Put("");
   
   out.Put("---------- using current format");
   out.Put(m1.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical");
   global->SetHorizontal(false);
   out.Put(m1.ToString());
   out.Put("");
   
   out.Put("============================== test Rmatrix::ToString()");
   Rmatrix m2(2, 2, rval, rval, rval, rval);
   
   cout << m2;
   
   out.Put("---------- using precision = 25");
   out.Put(m2.ToString(25));
   out.Put("");
   
   out.Put("---------- using current format");
   out.Put(m2.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical");
   global->SetHorizontal(false);
   out.Put(m2.ToString());
   out.Put("");
   
   out.Put("---------- using current format, vertical, prefix ...");
   global->SetPrefix("...");
   out.Put(m2.ToString());
   out.Put("");
   
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   GmatGlobal *global = GmatGlobal::Instance();
   std::string str;
   
   MessageInterface::SetLogFile("../../test/TestUtil/GmatLog.txt");
   
   out.Put("======================================== ival = 1234");
   Integer ival = 1234;
   out.Put("---------- using current format");
   str = GmatStringUtil::ToString(ival);
   out.Validate(str, "1234");
   
   out.Put("---------- using own format, width=10");
   str = GmatStringUtil::ToString(ival, false , 10);
   out.Validate(str, "      1234");
   
   out.Put("---------- using current format");
   str = GmatStringUtil::ToString(ival);
   out.Validate(str, "1234");
   
   out.Put("---------- using own format, width=2");
   str = GmatStringUtil::ToString(ival, false , 2);
   out.Validate(str, "1234");
   
   out.Put("======================================== rval1 = 123456789.123456789012");
   Real rval1 = 123456789.123456789012;
   WriteString(out, rval1, 1);
   
   out.Put("======================================== rval2 = 0.1234567890123456789");
   Real rval2 = 0.1234567890123456789;
   WriteString(out, rval2, 2);
   
   out.Put("======================================== rval2 = 0.0000000001234567890123");
   Real rval3 = 0.0000000001234567890123;
   WriteString(out, rval3, 3);
   
   Rvector3 r1(rval1, rval2, rval3);
   global->SetPrefix("   ");
   out.Put("rval3 =\n", r1.ToString());
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestToString/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestToStringOut.txt";
   TestOutput out(outFile);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of ToString()!!");
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
