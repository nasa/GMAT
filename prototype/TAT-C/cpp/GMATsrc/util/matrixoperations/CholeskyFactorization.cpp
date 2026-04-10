//$Id$
//------------------------------------------------------------------------------
//                             CholeskyFactorization
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond
// Created: 2017/07/27
//
/**
* Declares CholeskyFactorization class.
*/
//------------------------------------------------------------------------------

#include "CholeskyFactorization.hpp"
#include "RealUtilities.hpp"
#include "UtilityException.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// CholeskyFactorization()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
CholeskyFactorization::CholeskyFactorization()
{
}

//------------------------------------------------------------------------------
// CholeskyFactorization(const CholeskyFactorization &choleskyfactorization)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
CholeskyFactorization::CholeskyFactorization(const CholeskyFactorization
                                             &choleskyfactorization)
{
}

//------------------------------------------------------------------------------
// ~CholeskyFactorization()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
CholeskyFactorization::~CholeskyFactorization()
{
}

//------------------------------------------------------------------------------
// CholeskyFactorization& operator=(const CholeskyFactorization
//                                  &choleskyfactorization)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
CholeskyFactorization& CholeskyFactorization::operator=(const 
                                                        CholeskyFactorization
                                                        &choleskyfactorization)
{
   if (this != &choleskyfactorization)
   {
      //Call base class method
      MatrixFactorization::operator=(choleskyfactorization);
   }

   return *this;
}

//------------------------------------------------------------------------------
// void Factor(const Rmatrix inputMatrix, Rmatrix &R, Rmatrix &w)
//------------------------------------------------------------------------------
/**
* Matrix factorization routine using Cholesky decomposition
*
* @param inputMatrix The matrix to be factored, packed in upper triangular form
* @param R The matrix the factored result will be stored in
* @param blankMatrix This is just to have the required third argument for the
*        function due to class structure, this matrix is not used for anything
*/
//------------------------------------------------------------------------------
void CholeskyFactorization::Factor(const Rmatrix inputMatrix, Rmatrix &R,
                                   Rmatrix &blankMatrix)
{
   rowCount = inputMatrix.GetNumRows();
   iError = 0;

   if (rowCount != inputMatrix.GetNumColumns())
   {
      std::string errMessage =
         "Matrix must be square for Cholesky decomposition.";
      throw UtilityException(errMessage);
   }

   Integer array_size = rowCount * (rowCount + 1) / 2;
   sum1 = new Real[array_size];
   Integer index = 0;
   for (Integer ii = 0; ii < inputMatrix.GetNumRows(); ii++)
   {
      for (Integer jj = ii; jj < inputMatrix.GetNumColumns(); jj++)
      {
         sum1[index] = inputMatrix(ii, jj);
         ++index;
      }
   }

   Real dPivot, dsum, tolerance;

   const Real epsilon = 1.0e-8;
   rowCountIf = 0;
   j = 1;

   for (k = 1; k <= rowCount; ++k)
   {
      iLeRowCount = k - 1;
      tolerance = GmatMathUtil::Abs(epsilon * sum1[j - 1]);
      for (i = k; i <= rowCount; ++i)
      {
         dsum = 0.0;
         if (k != 1)
         {
            for (il = 1; il <= iLeRowCount; ++il)
            {
               kl = k - il;
               il1 = (kl - 1) * rowCount - (kl - 1) * kl / 2;
               dsum = dsum + sum1[il1 + k - 1] * sum1[il1 + i - 1];
            }
         }
         dsum = sum1[j - 1] - dsum;
         if (i > k)
            sum1[j - 1] = dsum * dPivot;
         else if (dsum > tolerance)
         {
            dPivot = sqrt(dsum);
            sum1[j - 1] = dPivot;
            dPivot = 1.0 / dPivot;
         }
         else if (iError < 0)
         {
            iError = k - 1;
            dPivot = GmatMathUtil::Sqrt(dsum);
            sum1[j - 1] = dPivot;
            dPivot = 1.0 / dPivot;
         }
         else if (dsum < 0.0)
         {
            std::string errMessage =
               "Matrix must be positive definite for Cholesky decomposition.";
            throw UtilityException(errMessage);
         }
         j = j + 1;
      }
      j = j + rowCountIf;
   }

   // Put elements into R matrix
   Integer elementCount = 0;

   for (Integer ii = 0; ii < rowCount; ++ii)
   {
      for (Integer jj = ii; jj < rowCount; ++jj)
      {
         R(ii, jj) = sum1[elementCount];
         elementCount++;
      }
   }
}


//------------------------------------------------------------------------------
// void CholeskyFactorization::Invert(Rmatrix &inputMatrix)
//------------------------------------------------------------------------------
/**
* Matrix inversion routine using Cholesky decomposition
*
* This method is a port of the inversion code from GEODYN, as ported by Angel
* Wang of Thinking Systems and then integrated into GMAT by D. Conway.
*
* @param inputMatrix The matrix to be inverted
*/
//------------------------------------------------------------------------------
void CholeskyFactorization::Invert(Rmatrix &inputMatrix)
{
   Real work, din;
   Rmatrix R(inputMatrix.GetNumRows(), inputMatrix.GetNumRows());
   Rmatrix w;
   Integer index;
   Factor(inputMatrix, R, w);

   // Invert R
   j = (rowCount - 1) * rowCount + (3 - rowCount) * rowCount / 2;

   sum1[j - 1] = 1.0 / sum1[j - 1];
   iPivot = j;

   for (i = 2; i <= rowCount; ++i)
   {
      j = iPivot - rowCountIf;
      iPivot = j - i;
      din = 1.0 / sum1[iPivot - 1];
      sum1[iPivot - 1] = din;

      i1 = rowCount + 2 - i;
      i2 = i - 1;
      i3 = i1 - 1;
      il1 = (i3 - 1) * rowCount - (i3 - 1) * i3 / 2;
      for (k1 = 1; k1 <= i2; ++k1)
      {
         k = rowCount + 1 - k1;
         j = j - 1;
         work = 0.0;
         for (il = i1; il <= k; ++il)
         {
            il2 = (il - 1) * rowCount - (il - 1) * il / 2 + k;
            work = work + sum1[il1 + il - 1] * sum1[il2 - 1];
         }
         sum1[j - 1] = -din * work;
      }
   }

   // Inverse(A) = INV(R) * TRN(INV(R));
   il = 1;
   for (i = 1; i <= rowCount; ++i)
   {
      il1 = (i - 1) * rowCount - (i - 1) * i / 2;
      for (j = i; j <= rowCount; ++j)
      {
         il2 = (j - 1) * rowCount - (j - 1) * j / 2;
         work = 0.0;
         for (k = j; k <= rowCount; ++k)
            work = work + sum1[il1 + k - 1] * sum1[il2 + k - 1];

         sum1[il - 1] = work;
         il = il + 1;
      }
      il = il + rowCountIf;
   }

   index = 0;
   for (Integer i = 0; i < inputMatrix.GetNumRows(); ++i)
   {
      for (Integer j = i; j < inputMatrix.GetNumColumns(); ++j)
      {
         inputMatrix(i, j) = sum1[index];
         ++index;
      }

      for (Integer j = 0; j < i; ++j)
         inputMatrix(i, j) = inputMatrix(j, i);
   }

}


//------------------------------------------------------------------------------
// Integer Invert(Real* SUM1, Integer array_size)
//------------------------------------------------------------------------------
/**
* Matrix inversion routine using Cholesky decomposition
*
* This method is a port of the inversion code from GEODYN, as ported by Angel
* Wang of Thinking Systems and then integrated into GMAT by D. Conway.
*
* @param sum1 The matrix to be inverted, packed in upper triangular form
* @param array_size The size of the sum1 array
*
* @return 0 on success, anything else indicates a problem
*/
//------------------------------------------------------------------------------
Integer CholeskyFactorization::Invert(Real* sum1, Integer array_size)
{
   Integer retval = -1;
   Integer rowCount = (Integer)((GmatMathUtil::Sqrt(1 + array_size * 8) - 1) / 2);
   Integer i, i1, i2, i3, ist, iERowCount, iError = 0, il, il1, il2;
   Integer j, k, k1, kl, iLeRowCount, rowCountIf, iPivot;
   Real dPivot, din, dsum, tolerance;
   Real work;

   const Real epsilon = 1.0e-8;

   rowCountIf = 0;
   j = 1;

   for (k = 1; k <= rowCount; ++k)
   {
      iLeRowCount = k - 1;
      tolerance = GmatMathUtil::Abs(epsilon * sum1[j - 1]);
      for (i = k; i <= rowCount; ++i)
      {
         dsum = 0.0;
         if (k != 1)
         {
            for (il = 1; il <= iLeRowCount; ++il)
            {
               kl = k - il;
               il1 = (kl - 1) * rowCount - (kl - 1) * kl / 2;
               dsum = dsum + sum1[il1 + k - 1] * sum1[il1 + i - 1];
            }
         }
         dsum = sum1[j - 1] - dsum;
         if (i > k)
            sum1[j - 1] = dsum * dPivot;
         else if (dsum > tolerance)
         {
            dPivot = sqrt(dsum);
            sum1[j - 1] = dPivot;
            dPivot = 1.0 / dPivot;
         }
         else if (iError < 0)
         {
            iError = k - 1;
            dPivot = GmatMathUtil::Sqrt(dsum);
            sum1[j - 1] = dPivot;
            dPivot = 1.0 / dPivot;
         }
         else if (dsum < 0.0)
         {
            retval = 1;
            break;            // Throw here?
         }

         j = j + 1;
      }
      j = j + rowCountIf;
   }

   if (retval == -1)
   {
      // Invert R
      j = (rowCount - 1) * rowCount + (3 - rowCount) * rowCount / 2;

      sum1[j - 1] = 1.0 / sum1[j - 1];
      iPivot = j;

      for (i = 2; i <= rowCount; ++i)
      {
         j = iPivot - rowCountIf;
         iPivot = j - i;
         din = 1.0 / sum1[iPivot - 1];
         sum1[iPivot - 1] = din;

         i1 = rowCount + 2 - i;
         i2 = i - 1;
         i3 = i1 - 1;
         il1 = (i3 - 1) * rowCount - (i3 - 1) * i3 / 2;
         for (k1 = 1; k1 <= i2; ++k1)
         {
            k = rowCount + 1 - k1;
            j = j - 1;
            work = 0.0;
            for (il = i1; il <= k; ++il)
            {
               il2 = (il - 1) * rowCount - (il - 1) * il / 2 + k;
               work = work + sum1[il1 + il - 1] * sum1[il2 - 1];
            }
            sum1[j - 1] = -din * work;
         }
      }

      // Inverse(A) = INV(R) * TRN(INV(R));
      il = 1;
      for (i = 1; i <= rowCount; ++i)
      {
         il1 = (i - 1) * rowCount - (i - 1) * i / 2;
         for (j = i; j <= rowCount; ++j)
         {
            il2 = (j - 1) * rowCount - (j - 1) * j / 2;
            work = 0.0;
            for (k = j; k <= rowCount; ++k)
               work = work + sum1[il1 + k - 1] * sum1[il2 + k - 1];

            sum1[il - 1] = work;
            il = il + 1;
         }
         il = il + rowCountIf;
      }
      retval = 0;
   }

   return retval;
}
