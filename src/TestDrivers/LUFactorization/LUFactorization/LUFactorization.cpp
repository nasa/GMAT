//$Id$
//------------------------------------------------------------------------------
//                                LUFactorization
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
// Created: 2017/08/04, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines LU factorization methods. This code strictly uses std::vector
* operations to create and factor the matrix.
*/
//------------------------------------------------------------------------------

#include "LUFactorization.hpp"
#include <vector>
#include <iostream>

//------------------------------------------------------------------------------
// LUFactorization::LUFactorization(std::vector < std::vector < double > > B,
//                                  bool pivotOption)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param B The matrix that will be factored
* @param pivotOption True or false setting to determine whether to use or not
*        use partial pivoting in calculations, defaulted to False.  Note that
*        pivoting should only be used when a diagonal value close or equal to
*        zero is expected.  Otherwise, it is a waste of computation time.
*/
//------------------------------------------------------------------------------
LUFactorization::LUFactorization(std::vector < std::vector < double > > &A,
                                 bool pivotOption)
{
   usePivot = pivotOption;
   Factorize(A);
}

//------------------------------------------------------------------------------
// LUFactorization::LUFactorization(const LUFactorization &lufactorization)
//------------------------------------------------------------------------------
/**
* Copy Constructor
*/
//------------------------------------------------------------------------------
LUFactorization::LUFactorization(const LUFactorization &lufactorization)
{
}


//------------------------------------------------------------------------------
// LUFactorization::LUFactorization(const LUFactorization &lufactorization)
//------------------------------------------------------------------------------
/**
* Class destructor
*/
//------------------------------------------------------------------------------
LUFactorization::~LUFactorization()
{
}

//------------------------------------------------------------------------------
// void LUFactorization::Factorize(std::vector < std::vector < double > > &A)
//------------------------------------------------------------------------------
/**
* Method used to factor the matrix.  Based off algorithms 3.2.1 and 3.4.1 from
* Gene H. Golub and Charles F. Van Loan.
*
* @param &A The matrix that will be factored.  A will be converted to a format
*        where the elements in and above the diagonal represent the upper
*        triangle while elements below the diagonal represent the lower
*        triangle of the matrix.
*/
//------------------------------------------------------------------------------
void LUFactorization::Factorize(std::vector < std::vector < double > > &A)
{
   //Initialize the required parameters
   int m = A.size();
   int n = A[0].size();
   int kMax = 0;
   int rowMax = 0;
   int colMax = 0;
   int colNum = 0;
   double maxElement = 0.0;
   int muMax = 0;
   double oldElement = 0.0;

   //vector to hold which rows are switched if partial pivot is used
   std::vector <int> p(m); 

   //Check for dimensions of matrix
   if (n == m)
   {
      kMax = n - 1;
      rowMax = n;
      colMax = n;
      determinant = CalcDeterminant(A);

      if (determinant == 0)
      {
         std::cout << "The matrix is singular, factorization cannot be completed.\n";
         return;
      }
   }

   else
   {
      if (n < m)
      {
         kMax = n;
         rowMax = m;
         colMax = n;
      }

      else
      {
         kMax = m;
         rowMax = m;
         colMax = n;
      }
   }

   for (int k = 0; k < kMax; k++)
   {
      if (usePivot == true)
      {
         maxElement = std::abs(A[k][k]);
         for (int mu = k; mu < n; mu++)
         {
            if (std::abs(A[mu][k]) > maxElement)
            {
               muMax = mu;
               maxElement = std::abs(A[mu][k]);
            }
         }
         //Switching rows so that diagonal contains largest element in column
         for (int i = k; i < int(A.size()); i++)
         {
            oldElement = A[k][i];
            A[k][i] = A[muMax][i];
            A[muMax][i] = oldElement;
            p[k] = muMax;
         }
      }

      if (A[k][k] != 0)
      {
         std::vector <int> rows(rowMax - k - 1);
         for (int i = 0; i < int(rows.size()); i++)
         {
            rows[i] = k + i + 1;
            A[rows[i]][k] = A[rows[i]][k] / A[k][k];
         }
         if (k < kMax)
         {
            std::vector <int> cols(colMax - k - 1);
            for (int ii = 0; ii < int(rows.size()); ii++)
            {
               for (int jj = 0; jj < int(cols.size()); jj++)
               {
                  colNum = k + jj + 1;
                  A[rows[ii]][colNum] = A[rows[ii]][colNum] - A[rows[ii]][k] * A[k][colNum];
               }
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// double LUFactorization::GetDeterminant(std::vector < std::vector < double > > A)
//------------------------------------------------------------------------------
/**
* Method that determines the determinant of a square matrix
*
* @param A The square matrix a determinant will be calculated for
* 
* @return det The determinant of the matrix
*/
//------------------------------------------------------------------------------
double LUFactorization::CalcDeterminant(std::vector < std::vector < double > > A)
{
   double det = 0.0;

   if (int(A.size()) == 2)
   {
      det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
      return det;
   }

   std::vector < std::vector < double > > B;
   B.resize(A.size() - 1, std::vector <double>(A.size() - 1));
   for (int j = 0; j < int(A.size()); j++)
   {
      for (int ii = 0; ii < int(B.size()); ii++)
      {
         for (int jj = 0; jj < int(B.size()); jj++)
         {
            if (jj >= j)
               B[ii][jj] = A[ii + 1][jj + 1];
            else
               B[ii][jj] = A[ii + 1][jj];
         }

      }
      det += pow(-1, j) * A[0][j] * CalcDeterminant(B);
   }

   return det;
}

//------------------------------------------------------------------------------
// void LUFactorization::GetData()
//------------------------------------------------------------------------------
/**
* Method to print out results
*/
//------------------------------------------------------------------------------
void LUFactorization::GetData()
{
   std::cout << "The Determininant: " << determinant << "\n";
}
