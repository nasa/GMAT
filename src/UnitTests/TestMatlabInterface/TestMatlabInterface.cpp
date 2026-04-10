//------------------------------------------------------------------------------
//                                  TestMatlabInterface
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
// Created: 2010.10.05
//
/**
 * Test driver for MatlabInterface utility.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "TestOutput.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "MatlabInterface.hpp"

using namespace std;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   MatlabInterface *mf = MatlabInterface::Instance();
   out.Put("\n============================== test MatlabInterface::Open()");
   mf->Open();
   out.Put("\n============================== test MatlabInterface::Close()");
   mf->Close();
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string startupFile = "gmat_startup_file.txt";
   FileManager *fm = FileManager::Instance();
   fm->ReadStartupFile(startupFile);
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestMatlabInterface/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestMatlabInterfaceOut.txt";
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of time classes!!");
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
