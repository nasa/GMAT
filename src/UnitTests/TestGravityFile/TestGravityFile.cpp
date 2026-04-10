//------------------------------------------------------------------------------
//                                  TestGravityFile
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
// Created: 2007/08/16
//
/**
 * Purpose:
 * Test driver for base/util/GravityFile.
 *
 * Input file:
 * 1. Create a test input file containing gravity file full pathnames.
 * 2. Put this file in the directory where executable is.
 *    Some sample input file look like:
 *    Note: It skips line with # in the first column
 *       c:/Projects/gmat/files/gravity/earth/JGM2.cof
 *       c:/Projects/gmat/files/gravity/earth/JGM3.cof
 *       #c:/Projects/gmat/files/gravity/earth/EGM96.cof
 *       c:/Projects/gmat/files/gravity/earth/EGM96low.cof
 *       c:/Projects/gmat/files/gravity/earth/EGM96low.dat
 *       c:/Projects/gmat/files/gravity/luna/LP165P.cof
 *       c:/Projects/gmat/files/gravity/luna/LP165P.grv
 *       c:/Projects/gmat/files/gravity/mars/Mars50C.cof
 *       #c:/Projects/gmat/files/gravity/mars/Mars50C.grv
 *       c:/Projects/gmat/files/gravity/venus/MGNP180U.cof
 *       c:/Projects/gmat/files/gravity/venus/MGNP180U.grv
 *
 * Output file:
 * TestGravityFileOut.txt in test driver directory
 * GmatLog.txt in test driver directory
 *
 * Test Procedure:
 * 1. It gets test input file name from the screen.
 * 2. It reads the line containg gravity file name.
 * 3. If line has # in the first column, it reads the next line.
 * 4. It creates an instance of GravityFile class.
 * 5. It gets gravity file type by calling GetFileType() and writes out.
 * 6. It gets file degree, order, mu, radius by calling GetFileInfo() and writes out.
 * 7. It gets file coefficients by calling ReadFile() and writes out.
 * 8. If gravity file is for Earth, it validates a few coefficients.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "GravityFile.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"

using namespace std;

//------------------------------------------------------------------------------
// void RunGravityFileTest(std::string &filename, TestOutput &out)
//------------------------------------------------------------------------------
void RunGravityFileTest(std::string &filename, TestOutput &out)
{
   out.SetPrecision(12);
   out.SetWidth(20);
   GravityFile gf;
   
   Integer m, n;
   static Real Cbar[361][361];
   static Real Sbar[361][361];
   static Real dCbar[17][17];
   static Real dSbar[17][17];
   
   for (n=0; n <= 360; ++n)
      for ( m=0; m <= 360; ++m)
      {
         Cbar[n][m] = 0.0;
         Sbar[n][m] = 0.0;
      }
   
   for (n=0; n <= 16; ++n)
      for ( m=0; m <= 16; ++m)
      {
         dCbar[n][m] = 0.0;
         dSbar[n][m] = 0.0;
      }
   
   try
   {
      if (filename[0] == '#')
      {
         out.Put("Skippking file: ", filename);
         return;
      }
      
      Integer deg, order;
      Real mu, radius;
      bool retval;
      
      out.Put("========== Reading Gravity File Type: ", filename);
      GmatFM::GravityFileType gft = gf.GetFileType(filename);
      out.Put("file type = ", gft);
      
      out.Put("========== Reading Gravity File Info: ", filename);
      gf.GetFileInfo(filename, deg, order, mu, radius);
      
      out.Put("degree = ", deg);
      out.Put("order  = ", order);
      out.Put("mu     = ", mu);
      out.Put("radius = ", radius);
      
      out.Put("========== Reading Gravity File Coef: ", filename);
      
      retval = gf.ReadFile(filename, deg, order, mu, radius, true,
                           Cbar, Sbar, dCbar, dSbar, 360, 360, 2);
      
      out.Put(" Cbar[ 2][ 0] = ", Cbar[2][0]);
      out.Put(" Cbar[20][20] = ", Cbar[20][20]);
      out.Put(" Sbar[ 2][ 1] = ", Cbar[2][1]);
      out.Put(" Sbar[20][20] = ", Cbar[20][20]);
      
      if (gft == GmatFM::GFT_DAT)
      {
         out.Put("dCbar[ 2][ 0] = ", dCbar[2][0]);
         out.Put("dCbar[ 2][ 1] = ", dCbar[2][1]);
         out.Put("dSbar[ 2][ 0] = ", dSbar[2][0]);
         out.Put("dSbar[ 2][ 1] = ", dSbar[2][1]);
      }
      
      // Do some validation here for Earth
      Real cbar_2_0 = -0.000484165;
      Real cbar_20_20 = 4.01448e-009;
      Real sbar_2_1 = -2.31792e-010;
      Real sbar_20_20 = 4.01448e-009;
      
      if (filename.find("earth") != filename.npos)
      {
         out.Validate(cbar_2_0, Cbar[2][0]);
         out.Validate(cbar_20_20, Cbar[20][20]);
         out.Validate(sbar_2_1, Sbar[2][1]);
         out.Validate(sbar_20_20, Sbar[20][20]);
      }
      
   }
   catch (BaseException &e)
   {
      throw;
   }
   
}


//------------------------------------------------------------------------------
//int RunTest(std::string& batchfilename, TestOutput &out)
//------------------------------------------------------------------------------
Integer RunTest(std::string& batchfilename, TestOutput &out)
{
   Integer count = 0;
   
   MessageInterface::ShowMessage
      ("================================================== GravityFile\n");

   out.Put("running batchfile: ", batchfilename);
   
   // check if batch file exist
   std::ifstream batchfile(batchfilename.c_str());
   
   if (!(batchfile))
   {
      std::string errstr = "Batch file \"" + batchfilename + "\" does not exist";
      out.Put(errstr);
      return 0;
   }
   
   std::string filename;
   batchfile >> filename;
   
   while (!batchfile.eof())
   {
      ++count;
      RunGravityFileTest(filename, out);
      
      batchfile >> filename;
   }

   return count;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // Assuming executable is in:  Dev/build/unit_test/exe
   // Assuming this file is in:   Dev/test/TestUtil
   // Create log and output file in this test directory
   
   MessageInterface::SetLogFile("../../../test/TestUtil/GmatLog.txt");
   TestOutput out("../../../test/TestUtil/TestGravityFileOut.txt");
   
   out.Put("");
   out.Put("=================================================");
   out.Put("You must have a file containing gravity files to run ");
   out.Put("unit testing in this directory.\n");
   out.Put("Put \"#\" in the first column if you want to skip the line.");
   out.Put("For example:");
   out.Put("  #c:/Projects/gmat/files/gravity/earth/EGM96.cof");
   out.Put("  c:/Projects/gmat/files/gravity/earth/EGM96.dat");
   out.Put("  c:/Projects/gmat/files/gravity/luna/LP165P.cof");
   out.Put("=================================================");
   out.Put("");
   
   bool moreToRun = true;
   std::string batchfilename;
   
   while (moreToRun)
   {
      cout << "Enter file containing gravity file names: ";
      cin >> batchfilename;
      out.Put("file: ", batchfilename);

      try
      {
         Integer count = RunTest(batchfilename, out);
         out.Put("Number of gravity files in the batch file: ", count);
         out.Put("\nSuccessfully ran unit testing of GravityFile!!");
      }
      catch (BaseException &e)
      {
         out.Put("**** ERROR **** " + e.GetFullMessage());
      }
      catch (...)
      {
         out.Put("Unknown error occurred\n");
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
