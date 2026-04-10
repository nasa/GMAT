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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//

#include "LUFactorization.hpp"
#include <chrono>
#include <iostream>

int main()
{
   std::vector < std::vector < double > > A;
   A.resize(3, std::vector<double>(3));
   A[0][0] = 1;
   A[0][1] = 2;
   A[0][2] = 2;
   A[1][0] = 2;
   A[1][1] = 8;
   A[1][2] = 0;
   A[2][0] = 2;
   A[2][1] = 0;
   A[2][2] = 24;

   //Chrono lines used for calculating computation time

   //std::chrono::time_point<std::chrono::system_clock> start, end;
   //start = std::chrono::system_clock::now();

   //for (int i = 0; i < 100000; i++)
      //LUFactorization test(A, false);

   //end = std::chrono::system_clock::now();

   //std::chrono::duration<double> elapsedTime = end - start;

   //std::cout << "Total time: " << elapsedTime.count() << "\n";

   LUFactorization test(A, false);

   std::cout << "L and U of matrix: \n";
   for (int ii = 0; ii < int(A.size()); ii++)
   {
      for (int jj = 0; jj < int(A[0].size()); jj++)
         std::cout << A[ii][jj] << "     ";
      std::cout << "\n";
   }

   test.GetData(); //Used to view determinant

   return 0;
}
