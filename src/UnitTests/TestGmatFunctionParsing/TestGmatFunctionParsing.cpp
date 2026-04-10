//------------------------------------------------------------------------------
//                          TestGmatFunctionParsing
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
// Created: 2008/03/07
//
/**
 * Purpose:
 * Test driver for base/interpreter/ScriptInterpreter::InterpretGmatFunction().
 *
 * Input file:
 * 1. Create a test input file containing GmatFunction file full pathnames.
 * 2. Put this file in the directory where executable is.
 *    Some sample input file look like:
 *    Note: It skips line with # in the first column
 *       c:/projects/gmat/files/GmatFunctions/NonExistentFunctionFile.gmf
 *       c:/projects/gmat/files/GmatFunctions/EmptyFunction.gmf
 *       #c:/projects/gmat/files/GmatFunctions/NonExistentFunctionFile.gmf
 *       c:/projects/gmat/files/GmatFunctions/WrongFunctionName.gmf
 *
 * Output file:
 * TestGmatFunctionParsingOut.txt in test driver directory
 * GmatLog.txt in test driver directory
 *
 * Test Procedure:
 * 1. It gets test input file name from the screen.
 * 2. It reads the line containg GmatFunction file name into array
 * 3. If line has # in the first column, it reads the next line.
 * 4. It creates an instance of GmatFunction class.
 * 5. It sets the GmatFunction file name to GmatFunction pointer.
 * 6. It calls Moderator::InterpretGmatFunction(function)
 * 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "Moderator.hpp"
#include "CommandUtil.hpp"
#include "GmatFunction.hpp"
#include "TestOutput.hpp"
#include "FileUtil.hpp"          // for GetFileListFromDirectory(), GetTextLines()
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// int ParseGmatFunction(TestOutput &out, Moderator *mod, Function *function,
//                       const std::string &filename)
//------------------------------------------------------------------------------
int ParseGmatFunction(TestOutput &out, Moderator *mod, Function *function,
                      const std::string &filename)
{
   function->SetStringParameter("FunctionPath", filename);
   MessageInterface::ShowMessage("==================== Testing %s\n", filename.c_str());
   out.Put("==================== Testing ", filename);
   GmatCommand *fcs = NULL;
   
   // clear funtin's input and output argument list
   MessageInterface::ShowMessage("Clearing function input and output argument list\n");
   function->TakeAction("Clear");
   try
   {
      fcs = mod->InterpretGmatFunction(function);
      std::string fcsStr = "***** No function control sequence has been built.\n";
      
      if (fcs != NULL)
         fcsStr = GmatCommandUtil::GetCommandSeqString(fcs);
      
      out.Put(fcsStr);
      MessageInterface::ShowMessage(fcsStr);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
      MessageInterface::ShowMessage(e.GetFullMessage() + "\n");
   }
   
   if (fcs)
      delete fcs;

   return 1;
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(const StringArray &fileNames, TestOutput &out)
{
   Integer count = 0;
   bool moderatorInitialized = false;
   Moderator *mod = Moderator::Instance();
   
   if (!moderatorInitialized)
   {
      out.Put("Initializing Moderator... ");
      
      // note: all message output goes into output directory from startup file
      if (!mod->Initialize())
         throw GmatBaseException("Moderator failed to initialize!");
      
      moderatorInitialized = true;
   }
   
   std::string outPath =  out.GetOutPathName();
   out.Put("outPath = ", outPath);
   std::string logFileName = out.GetOutPathName() + "GmatLog.txt";
   out.Put("logFile = ", logFileName);
   
   MessageInterface::SetLogFile(logFileName);
   
   out.Put("");
   
   GmatFunction *myFunction = new GmatFunction("MyFunction");
   MessageInterface::ShowMessage
      ("GmatFunction created: <%p>%s\n", myFunction, myFunction->GetName().c_str());
   
   out.Put("============================== test ScriptInterpreter::InterpretGMATFunction()");
   
   std::string filename;
   for (unsigned int i=0; i<fileNames.size(); i++)
   {
      filename = fileNames[i];
      
      if (filename[0] == '#')
      {
         out.Put("Skippking file: ", filename);
         continue;
      }
      
      ++count;      
      ParseGmatFunction(out, mod, myFunction, filename);
   }
   
   // clean up
   delete myFunction;
   mod->Finalize();
   
   return count;
   
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestGmatFunctionParsing/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestGmatFunctionParsingOut.txt";
   TestOutput out(outFile);
   
   out.Put("");
   out.Put("=================================================");
   out.Put("You can have a file containing GmatFunction files or ");
   out.Put("directory containing GmatFunction files to run this unit testing.");
   out.Put("If you want to use a file, just list full path file names.\n");
   out.Put("It will skip the line if \"#\" in the first column.");
   out.Put("For example:");
   out.Put("  #c:/Projects/gmat/files/GmatFunctions/GmatFunction1.gmf");
   out.Put("  c:/Projects/gmat/files/GmatFunctions/GmatFunction2.gmf");
   out.Put("  c:/Projects/gmat/files/GmatFunctions/GmatFunction3.gmf");
   out.Put("=================================================");
   out.Put("");
   
   bool moreToRun = true;
   std::string inputFileName;
   int option = 0;
   StringArray fileNames;
   
   while (moreToRun)
   {
      cout << "Enter input file option:\n"
         "1. By input file name\n"
         "2. By directory name.\n> ";
      cin >> option;
      
      if (option == 1)
      {
         cout << "Enter file containing GmatFunction file names: ";
         cin >> inputFileName;
         out.Put("batchfile: ", inputFileName);
         fileNames = GmatFileUtil::GetTextLines(inputFileName);
      }
      else
      {
         cout << "Enter directory containing GmatFunction files followed by /*.gmf: \n"
            "for example, c:/MyFunctions/*.gmf\n> ";
         cin >> inputFileName;
         out.Put("directory: ", inputFileName);
         fileNames = GmatFileUtil::GetFileListFromDirectory(inputFileName, true);
      }
      
      int numFiles = fileNames.size();
      out.Put("running ", numFiles, " files from the input file: " + inputFileName);
      
      if (numFiles > 0)
      {
         try
         {
            Integer count = RunTest(fileNames, out);
            out.Put("Number of GmatFunction files ran: ", count);
            out.Put("\nSuccessfully ran unit testing of GmatFunction parsing!!");
         }
         catch (BaseException &e)
         {
            out.Put("**** ERROR **** " + e.GetFullMessage());
         }
         catch (...)
         {
            out.Put("Unknown error occurred\n");
         }
      }
      
      char answer;
      
      cout << endl;
      cout << "Do you want to run another batch? (y/n) ";
      cin >> answer;
      
      if (answer == 'y' || answer == 'Y')
         moreToRun = true;
      else
         moreToRun = false;
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
