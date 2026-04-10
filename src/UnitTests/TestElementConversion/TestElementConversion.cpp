//------------------------------------------------------------------------------
//                              TestElementConversion
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
//
/**
 * Test driver for converting between different element types.
 * (Cartesian, Keplerian, SphericalRADEC, SphericalAZFPA)
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "Rvector6.hpp"
#include "CoordUtil.hpp"
#include "SphericalRADEC.hpp"
#include "TestOutput.hpp"

using namespace std;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   out.Put("\n============================== test CoordUtil");
   
   Rvector6 cartState(7100.0, 0.0, 1300.0, 0.0, 7.35, 1.0);
   Real grav = 398600.4415;
   Real ma;
   out.Put("========================= CartesianToKeplerian()");
   out.Put("cartState = ", cartState.ToString());
   
   Rvector6 keplState = CartesianToKeplerian(cartState, grav, &ma);
   out.Put("keplState = ", keplState.ToString());
   
   cout << "=============== Convert back to Cartesian()" << endl;
   Rvector6 cartState1 = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
   out.Put("cartState1 = ", cartState1.ToString());
   
   out.Put("=============== Change ecc to 1.5 and convert to Cartesian()");
   keplState[1] = 1.5;
   cartState1 = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
   out.Put("cartState1 = ", cartState1.ToString());
   
   out.Put("========================= CartesianToSphericalRADEC()");
   out.Put("cartState = ", cartState.ToString());
   
   Rvector6 radecState = CartesianToSphericalRADEC(cartState);
   out.Put("radecState = ", radecState.ToString());
   
   out.Put("=============== Convert back to Cartesian()");
   Rvector6 radecState1 = SphericalRADECToCartesian(radecState);
   out.Put("cartState1 = ", radecState1.ToString());

}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestUtil\\TestElementConversion.out");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of element conversion!!");
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
