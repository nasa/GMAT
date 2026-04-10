//------------------------------------------------------------------------------
//                               TestEphemerisFile
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
 *   Unit test driver of EphemerisFile class.
 *
 * Output file:
 *   TestEphemerisFileOut.txt
 *
 * Description of EphemerisFile:
 *   The EphemerisFile class allows the user to write orbit or attitude ephmeris
 *   to a specified file in CCSDS or SPK file format.  The user can configure the
 *   EphemerisFile by defining its output file type, file format, start and end epoch.
 * 
 * Test Procedure:
 *   1. Create EphemerisFile.
 *   2. Validate setting values by calling generic Set*Parameter().
 *   3. Create script to test EphemerisFile in various epoch and file format.
 *
 * Validation method:
 *   1. The test driver is used for user input error checking.
 *   2. Launch GMAT and read the script and verify the results by using spacit utility
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "EphemerisFile.hpp"
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
   out.Put("========================= Test Constructor()");
   EphemerisFile ephem1("MyEphem");
   Integer id;
   
   out.Put("========================= Test SetStringParameter()");
   try
   {
      ephem1.SetStringParameter("FileFormat", "dummy");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   try
   {
      ephem1.SetStringParameter("EpochFormat", "dummy");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      ephem1.SetStringParameter("EpochFormat", "UTCGregorian");
      ephem1.SetStringParameter("InitialEpoch", "dummy");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      ephem1.SetStringParameter("EpochFormat", "UTCModJulian");
      ephem1.SetStringParameter("InitialEpoch", "21545");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      id = ephem1.GetParameterID("InterpolationOrder");
      ephem1.SetIntegerParameter(id, 5);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      ephem1.SetStringParameter("StateType", "Cartesian");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //---------------------------------------------------------------------------
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestEphemerisFile/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestEphemerisFileOut.txt";
   TestOutput out(outFile);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of EphemerisFile!!");
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
