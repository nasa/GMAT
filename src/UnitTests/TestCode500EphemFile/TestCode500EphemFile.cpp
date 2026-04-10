//------------------------------------------------------------------------------
//                               TestCode500EphemFile
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
// Created: 2013.05.09
//
/**
 * Purpose:
 *   Unit test driver of Code500EphemerisFile utility class
 *
 * Output file:
 *   TestCode500EphemFileOut.txt
 *
 * Description of EphemerisFile:
 *   The Code500EphemerisFile utility class allows the user to write orbit ephmeris
 *   to a specified file in Code 500 Ephemeirs File format.
 * 
 * Test Procedure:
 *   1. Create Code500EphemerisFile instance.
 *   2. Read header and dValidate setting values by calling generic Set*Parameter().
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
#include "Code500EphemerisFile.hpp"
#include "LeapSecsFileReader.hpp"
#include "TimeSystemConverter.hpp"
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
   // Create leap seconds file reader
   std::string taiUtcFile = "tai-utc.dat";
   LeapSecsFileReader lsfr(taiUtcFile);
   TimeConverterUtil::SetLeapSecsFileReader(&lsfr);
   
   Code500EphemerisFile ephem1;

   //======================================================================
   #if 1
   //======================================================================
   
   //std::string inFileName = "ACE_vec424.eph";
   //std::string outFileName = "ACE_vec424_out.eph";
   //std::string inFileName = "aqua_20020504_20020530_defin.eph";
   //std::string outFileName = "aqua_20020504_20020530_defin..out.eph";
   //std::string inFileName = "gmat.eph";
   //std::string outFileName = "gmat_out.eph";
   //std::string inFileName = "EphemStressTests.eph";
   //std::string outFileName = "EphemStressTests_out.eph";   
   //std::string inFileName = "EPHEM_GMT3995-EphemStressTests.eph";
   //std::string outFileName = "EPHEM-GMT3995-EphemStressTests_out.eph";   
   //std::string inFileName = "Ephem0701.eph";
   //std::string outFileName = "Ephem0701_out.eph";   
   //std::string inFileName = "Ephem0709.eph";
   //std::string outFileName = "Ephem0709_out.eph";   
   std::string inFileName = "ephem1sec.eph";
   std::string outFileName = "ephem1sec_out.eph";
   
   // Test reading ephem file
   out.Put("========================= Test read ephem file");
   if (ephem1.OpenForRead(inFileName))
   {
      ephem1.ReadHeader1(1);
      ephem1.ReadHeader2(1);
      ephem1.ReadDataRecords(-999, 2);
      //ephem1.ReadDataRecords(10, 2);
   }
   
   // Test writing ephem file
   out.Put("========================= Test write ephem file");
   if (ephem1.OpenForWrite(outFileName))
   {
      ephem1.WriteHeader1();
      ephem1.WriteHeader2();
      ephem1.WriteDataAt(1);
   }
   
   // Read back output ephem file
   out.Put("========================= Test read back in");
   ephem1.CloseForRead();
   if (ephem1.OpenForRead(outFileName))
   {
      ephem1.ReadHeader1();
      ephem1.ReadHeader2();
      ephem1.ReadDataRecords(1000, 1);
   }
   //======================================================================
   #endif
   //======================================================================
   
   
   //======================================================================
   #if 0
   //======================================================================
   //---------------------------------------------------------------------------
   // Test double endianness swapping
   out.Put("========================= Test double byte swapping");
   double r_value = 123.456;
   MessageInterface::ShowMessage("=> r_value        = %f, %X\n", r_value, r_value);
   double r_swapped = ephem1.SwapDoubleEndian(r_value);
   MessageInterface::ShowMessage("=> r_swapped      = %X\n", r_swapped);
   double r_swappedAgain = ephem1.SwapDoubleEndian(r_swapped);
   MessageInterface::ShowMessage("=> r_swappedAgain = %f, %X\n", r_swappedAgain, r_swappedAgain);
   
   // Test int endianness swapping
   out.Put("========================= Test int byte swapping");
   int i_value = 1234;
   MessageInterface::ShowMessage("=> i_value        = %d, %X\n", i_value, i_value);
   int i_swapped = ephem1.SwapIntegerEndian(i_value);
   MessageInterface::ShowMessage("=> i_swapped      = %X\n", i_swapped);
   int i_swappedAgain = ephem1.SwapIntegerEndian(i_swapped);
   MessageInterface::ShowMessage("=> i_swappedAgain = %d, %X\n", i_swappedAgain, i_swappedAgain);
   
   //---------------------------------------------------------------------------
   // Test convert ascii to ebcdic
   out.Put("========================= Test convert ascii to ebcdic");
   char asciiA[] = "Test my ascii string";
   char ebcdicA[25] = {'\0'};
   ephem1.ConvertAsciiToEbcdic(asciiA, ebcdicA, 20);
   out.Put("ascii  = ", std::string(asciiA));
   out.Put("ebcdic = ", std::string(ebcdicA));
   
   // Test convert ebcdic to ascii
   out.Put("========================= Test convert ebcdic to ascii");
   char asciiB[25] = {'\0'};
   ephem1.ConvertEbcdicToAscii(ebcdicA, asciiB, 20);
   out.Put("ebcdic = ", std::string(ebcdicA));
   out.Put("ascii  = ", std::string(asciiB));
   //======================================================================
   #endif
   //======================================================================
   
   return 1;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./TestCode500EphemFile/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestCode500EphemFileOut.txt";
   TestOutput out(outFile);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Code500EphemFile!!");
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
