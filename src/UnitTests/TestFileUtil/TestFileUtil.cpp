//------------------------------------------------------------------------------
//                                  TestFileUtil
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
 * Test driver for StringUtil.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "FileUtil.hpp"
#include "UtilityException.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatFileUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   MessageInterface::ShowMessage("=========== TestFileUtil\n");
   
   std::string str, str1;
   Real rval, expRval;
   Integer ival, expIval;
   bool bval;
   StringArray itemNames;
   
   out.Put("");
   
   out.Put("============================== test GmatFileUtil::IsValidFileName()");
   //---------------------------------------------
   str = "validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = "withpath/validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = ".\\withpath/validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = "in*validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "invalidfile?name.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "\"invalidfile\\name.txt\"";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   out.Put("============================== test GmatFileUtil::IsPathAbsolute()");
   //---------------------------------------------
   str = "filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "c:/filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "c:/mydir/filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "..\\filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   //std::string outPath = "../../TestFileUtil/";
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestFileUtilOut.txt";   
   TestOutput out(outFile);
   
   #if 0
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   #endif
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of StingUtil!!");
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
