//------------------------------------------------------------------------------
//                               TestRmatrix
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
// Created: 2005/02/14
// Modification:
//   2005/02/14  L. Jun - Moved linear algebra related test cases from TestUtil.cpp
//
/**
 * Test driver for Rmatrix operations.
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
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"
#include "TimeTypes.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int OutputRmatrix(const Rmatrix &rmat, const Real *array, TestOutput &out)
//------------------------------------------------------------------------------
int OutputRmatrix(const Rmatrix &rmat, const Real *array, TestOutput &out)
{
   out.Put("=============== In OutputRmatrix()");
   std::string matStr = rmat.ToString();
   out.Put(matStr);
   
   Integer row, col;
   rmat.GetSize(row, col);
   out.Put("row = ", row, "col = ", col);
   
   Real *newArray = new Real(row*col);
   memcpy(newArray, array, sizeof(Real)*row*col);
   out.Put("=============== after memcpy");
   
   Rmatrix newRmat = Rmatrix(2, 2, newArray[0], newArray[1], newArray[3], newArray[4]);
   matStr = newRmat.ToString();
   out.Put(matStr);
   
   // Why crash when newArray is deleted here?
   //delete newArray;
   
   return 1;
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   Rmatrix mat53(5, 3,
                 1.1, 1.2, 1.3,
                 2.1, 2.2, 2.3,
                 3.1, 3.2, 3.3,
                 4.1, 4.2, 4.3,
                 5.1, 5.2, 5.3);
   out.Put("========================= mat53");
   out.Put(mat53);
   
   #if 0
   //---------------------------------------------------------------------------
   out.Put("========================= Test GetDataVector()");
   const Real *mat53Data = mat53.GetDataVector();
   
   OutputRmatrix(mat53, mat53Data, out);
   #endif
   
   Rmatrix mat53a(5, 3);
   Rmatrix mat53b(5, 3);
   Rmatrix mat53c(5, 3);
   Rmatrix mat53d(5, 3);
   Rmatrix mat11(1, 1);
   mat11(0, 0) = 10.0;
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test Rmatrix * Scalar");
   mat53a = mat53 * 10.0;
   out.Put(mat53a);
   
   out.Put("========================= Test Scalar * Rmatrix");
   mat53b = 10.0 * mat53;
   out.Put(mat53b);
   
   out.Put("========================= Test Rmatrix * Rmatrix[1,1]");
   mat53a = mat53 * mat11;
   out.Put(mat53a);
   
   out.Put("========================= Test Rmatrix[1,1] * Rmatrix");
   mat53b = mat11 * mat53;
   out.Put(mat53b);
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test Rmatrix / Scalar");
   mat53a = mat53 / 10.0;
   out.Put(mat53a);
   
   out.Put("========================= Test Scalar / Rmatrix");
   mat53b = 10.0 / mat53;
   out.Put(mat53b);
   
   out.Put("========================= Test Rmatrix / Rmatrix[1,1]");
   mat53a = mat53 / mat11;
   out.Put(mat53a);
   
   out.Put("========================= Test  Rmatrix[1,1] / Rmatrix");
   mat53b = mat11 / mat53;
   out.Put(mat53b);
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test Rmatrix + Rmatrix[1,1]");
   mat53c = mat53 + mat11;
   out.Put(mat53c);
   
   out.Put("========================= Test Rmatrix[1,1] + Rmatrix");
   mat53d = mat11 + mat53;
   out.Put(mat53d);
   
   //---------------------------------------------------------------------------
   out.Put("========================= Test Rmatrix - Rmatrix[1,1]");
   mat53c = mat53 - mat11;
   out.Put(mat53c);
   
   out.Put("========================= Test Rmatrix[1,1] - Rmatrix");
   mat53d = mat11 - mat53;
   out.Put(mat53d);
   
   //---------------------------------------------------------------------------

   return 1;
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
   std::string outPath = "../../TestRmatrix/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestRmatrixOut.txt";
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Rmatrix!!");
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
