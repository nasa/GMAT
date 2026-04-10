//------------------------------------------------------------------------------
//                                  TestItrf
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
// Created: 2004/08/31
//
/**
 * Test driver for Solar System stuff (testing updates for AtmosphereFactory)
 */
//------------------------------------------------------------------------------
#include <map>
#include <iostream>
#include <string>
#include <list>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

using namespace std;


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   
   cout.setf(ios::fixed);
   cout.precision(16);

   cout << "============================== Test ItrfCoefficientsFile =========================" << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

   std::string nutFileName  = "/GMAT/dev/datafiles/ITRF/NUTATION.DAT";
   std::string planFileName = "/GMAT/dev/datafiles/ITRF/NUT85.DAT";
   ItrfCoefficientsFile*    itrf = new ItrfCoefficientsFile(nutFileName,planFileName);

   cout << "ITRF File has been created! .............." << endl;

   cout << "NOW trying to initialize ..........." << endl;

   try
   {
      itrf->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "ERROR - " << be.GetMessage() << endl;
   }
   
   Integer numNut  = itrf->GetNumberOfNutationTerms();
   Integer numPlan = itrf->GetNumberOfPlanetaryTerms();

   std::vector<IntegerArray> the_a;
   std::vector<IntegerArray> the_ap;
   
   Rvector the_A(numNut);
   Rvector the_B(numNut);
   Rvector the_C(numNut);
   Rvector the_D(numNut);
   Rvector the_E(numNut);
   Rvector the_F(numNut);

   Rvector the_Ap(numPlan);
   Rvector the_Bp(numPlan);
   Rvector the_Cp(numPlan);
   Rvector the_Dp(numPlan);
   
   itrf->GetNutationTerms(the_a, the_A, the_B, the_C, the_D, the_E, the_F);
   itrf->GetPlanetaryTerms(the_ap, the_Ap, the_Bp, the_Cp, the_Dp);
   
   cout << "The size of a = " << the_a.size() << endl;
   Integer s;
   for(s = 0; s < (Integer) the_a.size(); s++)
   {
      cout << "The size of a(" << s << ") is " << (the_a.at(s)).size() << endl;
      cout << "And the first 6 rows' worth are " << (the_a.at(s)).at(0) << " " 
         << (the_a.at(s)).at(1) << " " << (the_a.at(s)).at(2) << " " 
         << (the_a.at(s)).at(3) << " "  << (the_a.at(s)).at(4) << " " 
         << (the_a.at(s)).at(5) << endl;
   }
   
   cout << "The size of A is " << the_A.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_A(0) << " " 
      << the_A(1) << " " << the_A(2) << " " 
      << the_A(3) << " "  << the_A(4) << " " 
      << the_A(5) << endl;
   cout << "The LAST A is " << the_A(the_A.GetSize() - 1) << endl;
   cout << "The size of B is " << the_B.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_B(0) << " " 
      << the_B(1) << " " << the_B(2) << " " 
      << the_B(3) << " "  << the_B(4) << " " 
      << the_B(5) << endl;
   cout << "The LAST B is " << the_B(the_B.GetSize() - 1) << endl;
   cout << "The size of C is " << the_C.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_C(0) << " " 
      << the_C(1) << " " << the_C(2) << " " 
      << the_C(3) << " "  << the_C(4) << " " 
      << the_C(5) << endl;
   cout << "The LAST C is " << the_C(the_C.GetSize() - 1) << endl;
   cout << "The size of D is " << the_D.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_D(0) << " " 
      << the_D(1) << " " << the_D(2) << " " 
      << the_D(3) << " "  << the_D(4) << " " 
      << the_D(5) << endl;
   cout << "The LAST D is " << the_D(the_D.GetSize() - 1) << endl;
   cout << "The size of E is " << the_E.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_E(0) << " " 
      << the_E(1) << " " << the_E(2) << " " 
      << the_E(3) << " "  << the_E(4) << " " 
      << the_E(5) << endl;
   cout << "The LAST E is " << the_E(the_E.GetSize() - 1) << endl;
   cout << "The size of F is " << the_F.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_F(0) << " " 
      << the_F(1) << " " << the_F(2) << " " 
      << the_F(3) << " "  << the_F(4) << " " 
      << the_F(5) << endl;
   cout << "The LAST F is " << the_F(the_F.GetSize() - 1) << endl;
   
   cout << "The size of ap = " << the_ap.size() << endl;
   for(s = 0; s < (Integer) the_ap.size(); s++)
   {
      cout << "The size of ap(" << s << ") is " << (the_ap.at(s)).size() << endl;
      cout << "And the first 6 rows' worth are " << (the_ap.at(s)).at(0) << " " 
         << (the_ap.at(s)).at(1) << " " << (the_ap.at(s)).at(2) << " " 
         << (the_ap.at(s)).at(3) << " "  << (the_ap.at(s)).at(4) << " " 
         << (the_ap.at(s)).at(5) << endl;
   }
   cout << "The size of Ap is " << the_Ap.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_Ap(0) << " " 
      << the_Ap(1) << " " << the_Ap(2) << " " 
      << the_Ap(3) << " "  << the_Ap(4) << " " 
      << the_Ap(5) << endl;
   cout << "The LAST Ap is " << the_Ap(the_Ap.GetSize() - 1) << endl;
   cout << "The size of Bp is " << the_Bp.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_Bp(0) << " " 
      << the_Bp(1) << " " << the_Bp(2) << " " 
      << the_Bp(3) << " "  << the_Bp(4) << " " 
      << the_Bp(5) << endl;
   cout << "The LAST Bp is " << the_Bp(the_Bp.GetSize() - 1) << endl;
   cout << "The size of Cp is " << the_Cp.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_Cp(0) << " " 
      << the_Cp(1) << " " << the_Cp(2) << " " 
      << the_Cp(3) << " "  << the_Cp(4) << " " 
      << the_Cp(5) << endl;
   cout << "The LAST Cp is " << the_Cp(the_Cp.GetSize() - 1) << endl;
   cout << "The size of Dp is " << the_Dp.GetSize() << endl;
   cout << "And the first 6 rows' worth are " << the_Dp(0) << " " 
      << the_Dp(1) << " " << the_Dp(2) << " " 
      << the_Dp(3) << " "  << the_Dp(4) << " " 
      << the_Dp(5) << endl;
   cout << "The LAST Dp is " << the_Dp(the_Dp.GetSize() - 1) << endl;
  
   
   cout << "....................................................." << endl;
   delete itrf;

   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   cout << "========================== End Test ItrfCoefficientsFile =========================" << endl;
   
}
