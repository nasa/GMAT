//------------------------------------------------------------------------------
//                               TestRmatrix66
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
// Created: 2009.03.30
//
/**
 * Test driver for Rmatrix66 operations.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "Linear.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Rmatrix66.hpp"
#include "Rmatrix33.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   std::string matStr;
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test Constructor()");
   Rmatrix66 mat1;
   matStr = mat1.ToString(12);
   out.Put(matStr);
   
   Rmatrix66 mat2(18,
                  1.1, 1.2, 1.3, 1.4, 1.5, 1.6,
                  2.1, 2.2, 2.3, 2.4, 2.5, 2.6,
                  3.1, 3.2, 3.3, 3.4, 3.5, 3.6);   
   matStr = mat2.ToString(12);
   out.Put(matStr);
   
   Rmatrix66 mat3(36,
                  1.1, 1.2, 1.3, 1.4, 1.5, 1.6,
                  2.1, 2.2, 2.3, 2.4, 2.5, 2.6,
                  3.1, 3.2, 3.3, 3.4, 3.5, 3.6,
                  4.1, 4.2, 4.3, 4.4, 4.5, 4.6,
                  5.1, 5.2, 5.3, 5.4, 5.5, 5.6,
                  6.1, 6.2, 6.3, 6.4, 6.5, 6.6);
   matStr = mat3.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test Set()");
   mat2.Set(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   matStr = mat2.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test UpperLeft()");
   Rmatrix33 matA = mat3.UpperLeft();
   matStr = matA.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test UpperRight()");
   Rmatrix33 matB = mat3.UpperRight();
   matStr = matB.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test LowerLeft()");
   Rmatrix33 matC = mat3.LowerLeft();
   matStr = matC.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test LowerRight()");
   Rmatrix33 matD = mat3.LowerRight();
   matStr = matD.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test Inverse()");
   Rmatrix66 mat1Inv = mat1.Inverse();
   matStr = mat1Inv.ToString(12);
   out.Put(matStr);
   
   out.Put("========================= Test Symmetric()");
   Rmatrix66 mat3Sym = mat3.Symmetric();
   matStr = mat3Sym.ToString(false, false, true, 3, 4, false);
   out.Put(matStr);
   
   //---------------------------------------------------------------------------

}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestRmatrix66/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestRmatrix66Out.txt";
   TestOutput out(outFile);
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Rmatrix66!!");
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
