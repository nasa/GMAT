//------------------------------------------------------------------------------
//                               TestLinearAlbegra
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
 * Test driver for linear algebra operations.
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

using namespace std;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   const std::string *descs;
   std::string *vals;
   Integer size;

   out.Put("");
   out.Put("============================== test Rvector3()");
   Rvector3 r31 = Rvector3(1.123, 2.345, 3.456);
   size = r31.GetNumData();
   descs = r31.GetDataDescriptions();
   vals = r31.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);

   Rvector3 r32 = Rvector3(1.123, 2.345, 3.456);
   out.Put("---------- test operator ==(r3==r32) should be true");
   out.PutBool(r31==r32);
   out.Validate(r31==r32, true);
   
   out.Put("---------- test operator !=(r3!=r32) should be false");
   out.PutBool(r31!=r32);
   out.Validate(r31!=r32, false);
   
   Rvector3 r33 = Rvector3(1.123, 2.345, 3.789);
   out.Put("---------- test operator ==(r3==r33) should be false");
   out.PutBool(r31==r33);
   out.Validate(r31==r33, false);
   
   out.Put("---------- test operator !=(r3!=r33) should be true");
   out.PutBool(r31!=r33);
   out.Validate(r31!=r33, true);
   
   out.Put("\n============================== test v3 = Cross(v1, v2)");
   Rvector3 v1(1.0, 0.0, 0.0);
   Rvector3 v2(0.0, 1.0, 0.0);
   Rvector3 v3 = Cross(v1, v2);
   out.Put("v1=", v1.ToString());
   out.Put("v2=", v2.ToString());
   out.Put("v3=", v3.ToString());
   out.Put("");
   
   v1.Set(1.0, 0.0, 0.0);
   v2.Set(0.0, 0.0, 1.0);
   v3 = Cross(v1, v2);
   out.Put("v1=", v1.ToString());
   out.Put("v2=", v2.ToString());
   out.Put("v3=", v3.ToString());
   out.Put("");
   
   v1.Set(-1.0, 0.0, 0.0);
   v2.Set( 0.0, 0.0, 1.0);
   v3 = Cross(v1, v2);
   out.Put("v1=", v1.ToString());
   out.Put("v2=", v2.ToString());
   out.Put("v3=", v3.ToString());
   
   out.Put("\n============================== test Rvector6()");
   Rvector6 r61 = Rvector6(1.123, 2.345, 3.456, 4.567, 5.678, 6.789);
   size = r61.GetNumData();
   descs = r61.GetDataDescriptions();
   vals = r61.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);
   
   Rvector6 r62 = Rvector6(1.123, 2.345, 3.456, 4.567, 5.678, 6.789);
   out.Put("r62 = ", r62.ToString());
   out.Put("---------- test operator ==(r6==r62) should be true");
   out.PutBool(r61==r62);
   out.Validate(r61==r62, true);
   
   out.Put("---------- test operator !=(r6!=r62) should be false");
   out.PutBool(r61!=r62);
   out.Validate(r61!=r62, false);
   
   Rvector6 r63 = Rvector6(1.123, 2.345, 3.456, 4.567, 5.678, 7.777);
   out.Put("r63 = ", r63.ToString());
   out.Put("---------- test operator ==(r6==r63) should be false");
   out.PutBool(r61==r63);
   out.Validate(r61==r63, false);
   
   out.Put("---------- test operator !=(r6!=r63) should be true");
   out.PutBool(r61!=r63);
   out.Validate(r61!=r63, true);

   
   //---------------------------------------------------------------------------
   out.Put("=============== Test Linear IO formating");
   Rvector vec1(5, 20040921.103020123, 2222.2222, 33333.33333,
                444444.444444, 5555555.5555555);
   out.Put("default vec1 = "); out.Put(vec1);
   
   GmatRealUtil::SetHorizontal(true);
   out.Put("horizontal vec1 = "); out.Put(vec1);
   
   GmatRealUtil::SetPrecision(18);
   GmatRealUtil::SetWidth(18);
   out.Put("precision=18 width=18 vec1 = "); out.Put(vec1);
   
   Rmatrix mat1(5, 3,
                1.1, 1.2, 1.3,
                2.1, 2.2, 2.3,
                3.1, 3.2, 3.3,
                4.1, 4.2, 4.3,
                5.1, 5.2, 5.3);

   out.Put("default mat1 = "); out.Put(mat1);
   
   GmatRealUtil::SetHorizontal(true);
   GmatRealUtil::SetWidth(3);
   out.Put("horizontal width=3 mat1 = "); out.Put(mat1);
   
   Real r1 = 1234.1234;
   out.Put("ToString(r1) =        ", GmatRealUtil::ToString(r1), ";");
   out.Put("ToString(r1, 8, 5) = ", GmatRealUtil::ToString(r1, 8, 5), ";");
   
   Integer i1 = 987654321;
   out.Put("ToString(i1) = ", GmatRealUtil::ToString(i1), ";");
   out.Put("");

}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\test\\TestUtil\\TestLinearAlgebraOut.txt");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of linear algebra!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
