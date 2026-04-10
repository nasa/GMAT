//------------------------------------------------------------------------------
//                                TestSpiceKernelWriter
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
// Author: Wendy Shoan
// Created: 2009.12.29
//
/**
 * Purpose:
 *   Unit test driver for SpiceKernelWriter class.
 *
 * Output file:
 *   TestSpiceKernelWriterOut.bsp
 *
 * Description of SpiceKernelWriter:
 *   The SpiceKernelWriter class writes out a binary SPK file, given the spacecraft NAIF Id,
 *   file name, references frame, and states and epochs for the orbit data.
 * 
 * Test Procedure:
 *   1. Create SpiceKernelWriter.
 *   2. Send meta data to the SpiceKernelWriter.
 *   2. In a loop, send states and epochs to the SpiceKernelWriter.
 *   3. Finalize the kernel.
 *   4. Create a SpiceKernelReader.
 *   5. Extract data from the SPK file at selected time points.  Write out data
 *      to a text file.
 *
 * Validation method:
 *   Compare the output text file with input values.
 *   Run CSPICE utility spacit to summarize output SPK file, to check for
 *   correct data format, reference frame, etc.
*/
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "Spacecraft.hpp"
#include "TestOutput.hpp"
#include "BaseException.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "SpiceOrbitKernelReader.hpp"
#include "SpiceKernelWriter.hpp"
#include "TimeTypes.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   out.Put("======================================== test SpiceKernelWriter\n");
   
//   Integer id;
   SpiceKernelWriter *skw = NULL;
   SpiceOrbitKernelReader *skr = NULL;

   // Create SpiceKernelWriter
   std::string kernelName     = "TestSpiceKernelWriter.bsp";
   std::string scName         = "MySpacecraft";
   std::string centerName     = "Earth";
   Integer     scNaifId       = -101;   // made up, of course
   Integer     centerNaifId   = 0;    // test checking for 0   - should be 399;
   std::string referenceFrame = "J2000";
   StateArray  sa;
   EpochArray  ea;
   Rvector6    states[400];
   A1Mjd       epochs[400];
   
   try
   {
      Integer degree = 9;
      skw = new SpiceKernelWriter(scName, centerName, scNaifId, centerNaifId, kernelName, degree);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      StringArray meta;
      std::string metaData = "First line of added meta data\n";
      meta.push_back(metaData);
      metaData = "Second line of added meta data\n";
      meta.push_back(metaData);
      // try sending meta data to the SKW
      skw->AddMetaData(meta);

      metaData = "Add another line using the one-string method too!!  Woo Hoo!!\n";
      skw->AddMetaData(metaData);

      // Test sending states and epochs to the SKW
      A1Mjd      refA1(GmatTimeUtil::A1MJD_OF_J2000 + 2922.00); // Jan 1 2008?
      for (Integer ii = 0; ii < 400; ii++)
      {
         epochs[ii] = refA1 + 0.01 * ii;
         for (Integer jj= 0; jj < 6; jj++)
            states[ii][jj] = ii * 100 + jj;
      }

      // send the first 100 points as a segment
      for (Integer ii = 0; ii < 100; ii++)
      {
         sa.push_back(&(states[ii]));
         ea.push_back(&(epochs[ii]));
      }
      skw->WriteSegment(epochs[0], epochs[99], sa, ea);

      // write another segment, of 150 points
      sa.clear();
      ea.clear();
      for (Integer ii = 100; ii < 150; ii++)
      {
         sa.push_back(&(states[ii]));
         ea.push_back(&(epochs[ii]));
      }
      skw->WriteSegment(epochs[100], epochs[149], sa, ea);

      // write another segment, of 175 points
      sa.clear();
      ea.clear();
      for (Integer ii = 150; ii < 325; ii++)
      {
         sa.push_back(&(states[ii]));
         ea.push_back(&(epochs[ii]));
      }
      skw->WriteSegment(epochs[150], epochs[324], sa, ea);

      // write yet another segment, of 75 points
      sa.clear();
      ea.clear();
      for (Integer ii = 325; ii < 400; ii++)
      {
         sa.push_back(&(states[ii]));
         ea.push_back(&(epochs[ii]));
      }
      skw->WriteSegment(epochs[325], epochs[399], sa, ea);

   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   skw->FinalizeKernel();

//   skr = SpiceKernelReader::Instance();
	skr = new SpiceOrbitKernelReader();

   // first, check to see that the NAIF Id was associated with the name correctly
   Integer nid = skr->GetNaifID(scName);
   out.Put("NAIF id for the object is ");
   out.Put(nid);
   Integer obsNid = skr->GetNaifID(centerName);
   out.Put("NAIF id for the central body is ");
   out.Put(obsNid);

   try
   {
      // Now try to read the data back in, to test that the actual values put on the file are correct
      skr->LoadKernel(kernelName);
      Rvector6  outState;
      for (Integer ii = 0; ii < 400; ii++)
      {
         outState = skr->GetTargetState(scName, nid, epochs[ii], centerName);
         out.Validate(outState, states[ii], 1.0e-7, true);
//         for (Integer jj = 0; jj < 6; jj++)
//            out.Validate(outState[jj], (states[ii])[jj], 1.0e-7, true);
      }
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   if (skw) delete skw;
   if (skr) delete skr;
   
   return 1;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   MessageInterface::SetLogFile("./output/GmatLog.txt"); // ??
   
   TestOutput out("TestSpiceKernelWriterOut.txt");
   out.Put(GmatTimeUtil::FormatCurrentTime());
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of SpiceKernelWriter!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   out.Close();
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
