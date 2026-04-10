//------------------------------------------------------------------------------
//                               TestLagrangeInterpolator
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
// Created: 2009.09.28
//
/**
/**
 * Purpose:
 *   Unit test driver of LagrangeInterpolator class.
 *
 * Output file:
 *   TestLagrangeInterpolatorOut.txt
 *
 * Description of LagrangeInterpolator:
 *   The LagrangeInterpolator class interpolates data at the desired
 *   interpolation point. (See GMAT Math Spec on Lagrange Interpolation)
 * 
 * Test Procedure:
 *   + Create LagrangeInterpolator with order of 20.
 *       - buffer size should be set to maximum of 22
 *   + Create LagrangeInterpolator with order of 7.
 *   + Test exceptions:
 *       - non-monotonic dada
 *       - too few data points
 *       - desired point not within the range
 *   + Test interpolate for points less than buffer size.
 *   + Test interpolate for points more than buffer size.
 *   + Test with some realistic data
 *
 * Validation method:
 *   The test driver code knows expected results and throws an exception if the
 *   result is not within the tolerance.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "LagrangeInterpolator.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   //---------------------------------------------------------------------------
   out.Put("========================= Test LagrangeInterpolator constructor");
   out.Put("========================= Test order of 20, buffer size should set to maximum of 22");
   LagrangeInterpolator lagrange20("MyLagrange", 3, 20);
   Integer bufSize = lagrange20.GetBufferSize();
   out.Put("buffer size is ", bufSize);
   
   out.Put("");
   out.Put("========================= Now create LagrangeInterpolator of order of 7 and continue");
   LagrangeInterpolator lagrangeInterp("MyLagrange", 3, 7);
   Real xReq, yIn[3], yOut[3];
   yIn[0] = 1.0;
   yIn[1] = 2.0;
   yIn[2] = 3.0;
   yOut[0] = -999.999;
   yOut[1] = -999.999;
   yOut[2] = -999.999;
   
   //-----------------------------------------------------------------
   // Test non-monotonic datda
   //-----------------------------------------------------------------
   out.Put("========================= Test non-monotonic dada");
   try
   {
      for (int i = 1; i <= 3; i++)
         lagrangeInterp.AddPoint(Real(i), yIn);
      
      lagrangeInterp.AddPoint(1.0, yIn);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //-----------------------------------------------------------------
   // Test interpolation feasiblility, data points less than required
   //-----------------------------------------------------------------
   out.Put("");
   out.Put("========================= Test data points less than required");
   
   lagrangeInterp.Clear();
   
   try
   {
      for (int i = 1; i <= 3; i++)
         lagrangeInterp.AddPoint(Real(i), yIn);
      
      lagrangeInterp.Interpolate(2.5, yOut);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //-----------------------------------------------------------------
   // Test interpolation feasiblility, data points not within range
   //-----------------------------------------------------------------
   out.Put("");
   out.Put("========================= Test desired point not within range");
   
   lagrangeInterp.Clear();
   
   try
   {
      for (int i = 1; i <= 8; i++)
         lagrangeInterp.AddPoint(Real(i), yIn);
      
      xReq = 9;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("*** ERROR *** We shouldn't get this\n"
              "Interpolated value of ", xReq, " are ", yOut[0]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //-----------------------------------------------------------------
   // Now interpolate for points less than buffer size
   //-----------------------------------------------------------------
   out.Put("");
   out.Put("========================= Test interpolate for points less than buffer size");
   
   lagrangeInterp.Clear();
   
   try
   {
      for (int i = 1; i <= 8; i++)
      {
         for (int j=0; j<3; j++)
            yIn[j] = Real(j + i);
         
         lagrangeInterp.AddPoint(Real(i), yIn);
      }
      
      xReq = 3.5;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("Interpolated value of ", xReq, " are ", yOut[0], yOut[1], yOut[2]);
      
      xReq = 7.5;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("Interpolated value of ", xReq, " are ", yOut[0], yOut[1], yOut[2]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //-----------------------------------------------------------------
   // Now add some more points so that it buffers from the beginning
   //-----------------------------------------------------------------
   out.Put("");
   out.Put("========================= Test interpolate for points more than buffer size");
   try
   {
      for (int i = 9; i <= 23; i++)
      {
         for (int j=0; j<3; j++)
            yIn[j] = Real(j + i);
         
         lagrangeInterp.AddPoint(Real(i), yIn);
      }
      
      xReq = 8.5;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("Interpolated value of ", xReq, " are ", yOut[0], yOut[1], yOut[2]);
      
      xReq = 15.5;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("Interpolated value of ", xReq, " are ", yOut[0], yOut[1], yOut[2]);
      
      xReq = 20.5;
      lagrangeInterp.Interpolate(xReq, yOut);
      out.Put("Interpolated value of ", xReq, " are ", yOut[0], yOut[1], yOut[2]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //-----------------------------------------------------------------
   // Now try with more meaningful data given from Steve
   //-----------------------------------------------------------------
   out.Put("");
   out.Put("========================= Test interpolate with some realistic data");
   
   lagrangeInterp.Clear();
   
   try
   {
      //-----  Create the functions table to be interpolated
      Real x[8];
      Real f[8][3];
      Real xi[3], fe[3][3], fi[3];
      
      for (int i = 0; i < 8; i++)
      {
         x[i] = Real(i + 1);
         f[i][0] = Pow(x[i],2) - 2.0*x[i] + 3.0;
         f[i][1] = Pow(x[i],3) + Exp(x[i]/100);
         f[i][2] = Sin(x[i]/30);
      }
      
      //----- Create the exact solutions at the interpolation points
      xi[0] = 3.3415;
      xi[1] = 5.3333333333333333;
      xi[2] = 7.3426;
      
      for (int i = 0; i < 3; i++)
      {
         fe[i][0] = Pow(xi[i],2) - 2*xi[i] + 3;
         fe[i][1] = Pow(xi[i],3) + Exp(xi[i]/100);
         fe[i][2] = Sin(xi[i]/30);
      }
      
      //------  Add points
      for (int i = 0; i < 8; i++)
         lagrangeInterp.AddPoint(x[i], f[i]);
      
      //------  Perform the interpolation
      for (int i = 0; i < 3; i++)
      {
         lagrangeInterp.Interpolate(xi[i], fi);
         out.Put("Interpolated value of ", xi[i], " are ", fi[0], fi[1], fi[2]);
         out.Validate(fi[0], fi[1], fi[2], fe[i][0], fe[i][1], fe[i][2]);
      }
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
   std::string outPath = "../../TestLagrangeInterpolator/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestLagrangeInterpolatorOut.txt";
   TestOutput out(outFile);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of LagrangeInterpolator!!");
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
