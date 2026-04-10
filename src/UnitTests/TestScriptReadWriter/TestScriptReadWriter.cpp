//------------------------------------------------------------------------------
//                                  TestScriptReadWriter
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
// Created: 2006/08/21
//
/**
 * Test driver for ScriptReadWriter.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "StringUtil.hpp"
#include "GmatBaseException.hpp"
#include "TestOutput.hpp"
#include "ScriptReadWriter.hpp"
#include "MessageInterface.hpp"

#include <iostream>
#include <string>
#include <fstream>         // for std::ifstream used in GMAT functions

using namespace std;
using namespace GmatStringUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out);
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   MessageInterface::SetLogFile("../../test/TestScriptReadWriter/GmatLog.txt");
   MessageInterface::ShowMessage
      ("================================================== TestScriptReadWriter\n");
   
   ScriptReadWriter *theReadWriter = ScriptReadWriter::Instance();
   std::string scriptFilename = "../../test/TestScriptReadWriter/TestScriptReadWriterIn.txt";
   std::ifstream inFile(scriptFilename.c_str());
   std::istream *inStream;
   inStream = &inFile;
   theReadWriter->SetInStream(inStream);
   
   std::string header;
   std::string block;
   int blockCounter = 0;
   
   out.Put("");
   out.Put("======================================== test ReadHeaderComment()\n");
   
   out.Put("----------------------------------------\n");
   theReadWriter->ReadFirstBlock(header, block);
   out.Put(header);
   
   // Write first block
   if (block != "")
   {
      blockCounter++;
      out.Put(blockCounter);
      out.Put(block);
   }
   
   while (block != "")
   {
      blockCounter++;
      block = theReadWriter->ReadLogicalBlock();
      out.Put(blockCounter);
      out.Put(block);
   }
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestScriptReadWriter\\TestScriptReadWriterOut.txt");
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   
   // do not add new line
   out.SetAddNewLine(false);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of ScriptReadWriter!!\n");
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
