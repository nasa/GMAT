//------------------------------------------------------------------------------
//                           TestConsoleApp
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
// Created: 2007/03/05
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
/**
 * Purpose:
 *   Test driver for GMAT console version.
 *
 * Output file:
 *   TestConsoleAppOut.txt
 *   GmatLog.txt as specified in the gmat_startup_file.txt
 *
 * Description of Console version:
 *   The console version of GMAT application is built without GUI components using
 *   the compiler flag __CONSOLE_APP__.
 *
 * Test Procedure:
 *   1. Get file names containing script names from the console.
 *      The sample file of run.txt contains the following scripts
 *      (# in the first column indicates to ignore the line)
 *      #c:/Projects/gmat/Scripts/PerformanceTests10Day/Earth00MSISE90_10Day.m
 *      c:/Projects/gmat/Scripts/PerformanceTests10Day/Earth00_10Day.m
 *   2. Get run repeat count from the console.
 *   3. It clocks for begin time.
 *   4. It builds scripts specified in the input files names ignores line with #.
 *   5. I runs built mission for the specified number of times.
 *   6. It clocks for end time.
 *   7. It reports total elapsed time.
 */
//------------------------------------------------------------------------------


#include <fstream>
#include <ctime>                   // for clock()

#include "ConsoleAppException.hpp"
#include "Moderator.hpp"
#include "StringUtil.hpp"         // for GmatStringUtil::
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;

//------------------------------------------------------------------------------
// void RunScriptInterpreter(std::string script, , TestOutput &out, int runCount)
//------------------------------------------------------------------------------
/**
 * Executes a script.
 * 
 * @param <script> The script file that is run.
 */
//------------------------------------------------------------------------------
void RunScriptInterpreter(std::string script, TestOutput &out, int runCount)
{
   static bool moderatorInitialized = false;
   
   Moderator *mod = Moderator::Instance();
   
   if (!moderatorInitialized)
   {
      out.Put("Initialize Moderator... ");
      
      if (!mod->Initialize())
         throw ConsoleAppException("Moderator failed to initialize!");
      
      moderatorInitialized = true;
   }
   
   try
   {
      if (script[0] == '#')
      {
         out.Put("Skippking script: ", script);
         return;
      }
      
      out.Put("Reading script: ", script);
      if (!mod->InterpretScript(script))
      {
         MessageInterface::ShowMessage("**** Error Reading the script\n");
         out.Put("   Error Reading the script");
         return;
      }
   }
   catch (BaseException &oops)
   {
      std::cout << "ERROR!!!!!! ---- " << oops.GetFullMessage();
   }
   
   
   // And now run it
   
   for (int i=0; i<runCount; i++)
   {
      out.Put("Running script: ", script);
      out.Put("Running count: ", i+1);
      
      if (mod->RunMission() != 1)
         throw ConsoleAppException("Moderator::RunMission failed");
   }
   
}


//------------------------------------------------------------------------------
// Integer RunBatch(std::string& batchfilename, TestOutput &out, int runCount)
//------------------------------------------------------------------------------
/**
 * Executes a collection of scripts.
 * 
 * @param <batchfilename> The file containing the list of script files to run.
 * 
 * @return The number of lines parsed from teh batch file.
 */
//------------------------------------------------------------------------------
Integer RunBatch(std::string& batchfilename, TestOutput &out, int runCount)
{
   Integer count = 0;
   
   MessageInterface::ShowMessage
      ("================================================== TestConsoleApp\n");

   out.Put("running batchfile: ", batchfilename);
   
   // check if batch file exist
   std::ifstream batchfile(batchfilename.c_str());
   
   if (!(batchfile))
   {
      std::string errstr = "Batch file \"" + batchfilename + "\" does not exist";
      out.Put(errstr);
      return 0;
   }
   
   std::string script;
   batchfile >> script;
   
   clock_t t1 = clock();
   
   while (!batchfile.eof())
   {
      ++count;
      RunScriptInterpreter(script, out, runCount);
      
      batchfile >> script;
   }
   
   clock_t t2 = clock();
   
   MessageInterface::ShowMessage
      ("===> Total Batch Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);
   
   return count;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
/**
 * The program entry point.
 * 
 * @param <argc> The count of the input arguments.
 * @param <argv> The input arguments.
 * 
 * @return 0 on success.
 */
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestConsoleApp/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestConsoleAppOut.txt";
   TestOutput out(outFile);
   
   std::string msg = "Console Based Version ";

   msg += "Build Date: ";
   msg += __DATE__;
   msg += "  ";
   msg += __TIME__;
   
   out.Put("");
   out.Put(msg);
   out.Put("");
   out.Put("=================================================");
   out.Put("You must have a gmat_startup_file.txt and ");
   out.Put("a file containing scripts to run in this directory.\n");
   out.Put("Put \"#\" in the first column if you want to skip the line.");
   out.Put("For example:");
   out.Put("  #c:/Projects/gmat/Earth00MSISE90_10Day.m");
   out.Put("  c:/Projects/gmat/Earth00_10Day.m");
   out.Put("  #c:/Projects/gmat/Scripts/Earth20by20_10Day.m");
   out.Put("=================================================");
   out.Put("");
   
   
   int runCount = 0;
   bool moreToRun = true;
   std::string batchfilename;
   std::string runCountStr;
   
   while (moreToRun)
   {
      cout << "Enter file containing script names: ";
      cin >> batchfilename;
      cout << "Enter number of times to repeat run: ";
      cin >> runCountStr;
      
      if (!GmatStringUtil::ToInteger(runCountStr, runCount))
      {
         out.Put(runCountStr, " is invalid number, it is set to 0.");
         runCount = 0;
      }
      
      out.Put("file: ", batchfilename);
      out.Put("repeat run: ", runCount);

      if (runCount > 0)
      {
         try
         {
            Integer count = RunBatch(batchfilename, out, runCount);
            out.Put("Number of scripts in the file: ", count);
         }
         catch (BaseException &e)
         {
            out.Put(e.GetFullMessage());
         }
         catch (...)
         {
            out.Put("Unknown error occurred\n");
         }
      }
      
      char answer;
      runCount = 0;
      
      cout << endl;
      cout << "Do you want to run another batch? (y/n) ";
      cin >> answer;
      
      if (answer == 'y' || answer == 'Y')
         moreToRun = true;
      else
         moreToRun = false;
      
   }
   
   
}

