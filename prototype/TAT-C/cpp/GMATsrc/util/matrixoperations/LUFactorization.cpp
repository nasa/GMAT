//$Id$
//------------------------------------------------------------------------------
//                                LUFactorization
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
// Created: 2017/08/04, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines LU factorization methods.  This class strictly uses Rvector and
* Rmatrix from Gmat code for vector and matrix operations.
*/
//------------------------------------------------------------------------------

#include "LUFactorization.hpp"
#include "QRFactorization.hpp"
#include "UtilityException.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// LUFactorization(bool pivotOption)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param pivotOption True or false setting to determine whether to use or not
*        use partial pivoting in calculations, defaulted to False.  Note that
*        pivoting should only be used when a diagonal value close or equal to
*        zero is expected.  Otherwise, it is a waste of computation time.
*/
//------------------------------------------------------------------------------
LUFactorization::LUFactorization(bool pivotOption)
{
   usePivot = pivotOption;
}

//------------------------------------------------------------------------------
// LUFactorization(const LUFactorization &lufactorization)
//------------------------------------------------------------------------------
/**
* Copy Constructor
*/
//------------------------------------------------------------------------------
LUFactorization::LUFactorization(const LUFactorization &lufactorization)
{
}


//------------------------------------------------------------------------------
// ~LUFactorizationGmat()
//------------------------------------------------------------------------------
/**
* Class destructor
*/
//------------------------------------------------------------------------------
LUFactorization::~LUFactorization()
{
}

//------------------------------------------------------------------------------
// LUFactorization& operator=(const LUFactorization &lufactorization)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
LUFactorization& LUFactorization::operator=(const LUFactorization
                                            &lufactorization)
{
   if (this != &lufactorization)
   {
      //Call base class method
      MatrixFactorization::operator=(lufactorization);
   }

   return *this;
}

//------------------------------------------------------------------------------
// void Factor(const Rmatrix inputMatrix, Rmatrix &L, Rmatrix &U)
//------------------------------------------------------------------------------
/**
* Method used to factor the matrix with LU factorization (A = LU).  This method
* is based off algorithm 3.4.1 from Gene H. Golub and Charles F. Van Loan.
*
* @param inputMatrix The matrix that will be factored
* @param &L The lower triangular matrix
* @param &U The upper triangular matrix
*/
//------------------------------------------------------------------------------
void LUFactorization::Factor(const Rmatrix inputMatrix, Rmatrix &L, Rmatrix &U)
{
   //Initialize the required parameters
   Rmatrix A = inputMatrix;
   rowCount = A.GetNumRows();
   colCount = A.GetNumColumns();
   Integer kMax = 0;
   Integer rowNum = 0;
   Integer colNum = 0;
   double maxElement = 0.0;
   Integer muMax = 0;
   double oldElement = 0.0;

   Integer LRowCount = L.GetNumRows();
   Integer LColCount = L.GetNumColumns();
   Integer URowCount = U.GetNumRows();
   Integer UColCount = U.GetNumColumns();

   //Check for "shape" of matrix
   if (colCount == rowCount)
   {
      kMax = colCount - 1;
      permuVector.SetSize(colCount - 1);
   }

   else
   {
      if (colCount < rowCount)
      {
         kMax = colCount;
         permuVector.SetSize(colCount);
      }

      else
      {
         kMax = rowCount;
         permuVector.SetSize(rowCount);
      }
   }

   Real *AData = (Real*)A.GetDataVector();

   // Begin algorithm 3.4.1
   for (Integer k = 0; k < kMax; ++k)
   {
      if (usePivot == true)
      {
         maxElement = std::abs(AData[k * colCount + k]);
         muMax = k;
         for (Integer mu = k; mu < rowCount; ++mu)
         {
            if (std::abs(AData[mu * colCount + k]) > maxElement)
            {
               muMax = mu;
               maxElement = std::abs(AData[mu * colCount + k]);
            }
         }
         //Switching rows so that diagonal contains largest element in column
         for (Integer i = k; i < colCount; ++i)
         {
            oldElement = AData[k * colCount + i];
            AData[k *colCount + i] = AData[muMax * colCount + i];
            AData[muMax * colCount + i] = oldElement;
         }
         permuVector[k] = muMax;
      }

      if (AData[k * colCount + k] != 0)
      {
         Real* rows = new Real[rowCount - k - 1];
         for (Integer i = 0; i < (rowCount - k - 1); ++i)
         {
            rows[i] = k + i + 1;
            rowNum = (Integer)rows[i];
            AData[rowNum * colCount + k] =
               AData[rowNum * colCount + k] / AData[k * colCount + k];
         }
         if (k < kMax)
         {
            for (Integer ii = 0; ii < (rowCount - k - 1); ++ii)
            {
               for (Integer jj = 0; jj < (colCount - k - 1); ++jj)
               {
                  colNum = k + jj + 1;
                  rowNum = (Integer)rows[ii];
                  AData[rowNum * colCount + colNum] =
                     AData[rowNum * colCount + colNum] - AData[rowNum * colCount + k] * AData[k * colCount + colNum];
               }
            }
         }
      }
   }

   //Get L and U

   if (colCount > rowCount)
   {
      for (Integer i = 0; i < rowCount; ++i)
      {
         for (Integer j = 0; j < rowCount; ++j)
            L(i, j) = A(i, j);
      }

      U = A;
   }
   
   else
   {
      if (colCount < rowCount)
      {
         for (Integer i = 0; i < colCount; ++i)
         {
            for (Integer j = 0; j < colCount; ++j)
               U(i, j) = A(i, j);
         }

         L = A;
      }

      else
      {
         U = A;
         L = A;
      }
   }

   for (Integer j = 0; j < LColCount; ++j)
   {
      for (Integer i = 0; i <= j && i < LRowCount; ++i)
      {
         if (i == j)
            L(i, j) = 1.0;
         else
            L(i, j) = 0.0;
      }
   }

   Real *UData = (Real*)U.GetDataVector();

   for (Integer j = 0; j < UColCount; ++j)
   {
      for (Integer i = URowCount - 1; i > j; --i)
      {
         UData[i * UColCount + j] = 0.0;
      }
   }
}


//------------------------------------------------------------------------------
// void Invert(Rmatrix &inputMatrix)
//------------------------------------------------------------------------------
/**
* Method used to invert the matrix using LU factorization
*
* @param &inputMatrix The matrix that will be inverted
*/
//------------------------------------------------------------------------------
void LUFactorization::Invert(Rmatrix &inputMatrix)
{
   if (inputMatrix.GetNumRows() != inputMatrix.GetNumColumns())
   {
      std::string errMessage = "The matrix must be square to invert.\n";
      throw UtilityException(errMessage);
   }

   determinant = Determinant(inputMatrix);

   if (determinant == 0)
   {
      std::string errMessage =
         "The matrix is singular, inverse cannot be computed.\n";
      throw UtilityException(errMessage);
   }

   Rmatrix L(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());
   Rmatrix U(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());
   Factor(inputMatrix, L, U);

   Rvector y(colCount); //Intermediate column vector
   Real yHold;

   for (Integer colNum = 0; colNum < colCount; ++colNum)
   {
      for (Integer i = 0; i < colCount; ++i)
      {
         if (i != colNum)
            y[i] = 0;
         else
            y[i] = 1;
      }

      if (usePivot)
      {
         for (Integer k = 0; k < inputMatrix.GetNumRows() - 1; ++k)
         {
            yHold = y[k];
            y[k] = y[(Integer)permuVector[k]];
            y[(Integer)permuVector[k]] = yHold;
            for (Integer i = k + 1; i < inputMatrix.GetNumRows(); ++i)
            {
               if (i <= k)
                  y[i] -= y[k] * U(i, k);
               else
                  y[i] -= y[k] * L(i, k);
            }
         }
      }

      if (!usePivot)
      {
         for (Integer i = 0; i < colCount; ++i)
         {
            if (i != 0)
            {
               for (Integer j = 0; j < i; ++j)
                  y[i] -= L(i, j) * y[j];
            }
         }
      }

      for (Integer i = 0; i < colCount; ++i)
      {
         inputMatrix(i, colNum) = y[i];
      }

      for (Integer i = colCount - 1; i >= 0; --i)
      {
         for (Integer j = i + 1; j < colCount; ++j)
            inputMatrix(i, colNum) -= U(i, j) * inputMatrix(j, colNum);

         inputMatrix(i, colNum) = inputMatrix(i, colNum) / U(i, i);
      }
   }

}


//------------------------------------------------------------------------------
// void SolveSystem(const Rmatrix inputMatrix, Rvector b, Rvector &x)
//------------------------------------------------------------------------------
/**
* Method used to solve system of equations using LU factorization (Ax = b).  For
* square systems, a combination of algorthims 3.1.1 and 3.1.2 from Gene H. Golub
* and Charles F. Van Loan are used with matrices L and U. When an
* underdetermined system is input, algorithm 5.7.2 from Gene H. Golub and 
* Charles F. Van Loan is used to find the minimum 2-norm solution.
*
* @param inputMatrix The matrix resembling coefficients of equations
* @param b Column vector representing RHS values of equations from inputMatrix
* @param x Column vector containing solution values to solve system of equations
*/
//------------------------------------------------------------------------------
void LUFactorization::SolveSystem(const Rmatrix inputMatrix, Rvector b, Rvector &x)
{
   if (inputMatrix.GetNumRows() == inputMatrix.GetNumColumns())
   {
      // Use algorithms 3.1.1 and 3.1.2
      Rmatrix L(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());
      Rmatrix U(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());
      Factor(inputMatrix, L, U);
      Real bHold;

      Rvector y(colCount); //Intermediate column vector
      Real* UData = (Real*)U.GetDataVector();
      Real* LData = (Real*)L.GetDataVector();

      if (usePivot)
      {
         for (Integer k = 0; k < rowCount - 1; ++k)
         {
            bHold = b[k];
            b[k] = b[(Integer)permuVector[k]];
            b[(Integer)permuVector[k]] = bHold;
            for (Integer i = k + 1; i < rowCount; ++i)
            {
               if (i <= k)
                  b[i] -= b[k] * UData[i * colCount + k];
               else
                  b[i] -= b[k] * LData[i * colCount + k];
            }
         }
      }

      for (Integer i = 0; i < colCount; ++i)
         y[i] = b[i];

      if (!usePivot)
      {
         for (Integer i = 0; i < colCount; ++i)
         {
            if (i != 0)
            {
               for (Integer j = 0; j < i; ++j)
                  y[i] -= LData[i * colCount + j] * y[j];
            }
         }
      }

      for (Integer i = 0; i < colCount; ++i)
      {
         x[i] = y[i];
      }

      for (Integer i = colCount - 1; i >= 0; --i)
      {
         for (Integer j = i + 1; j < colCount; ++j)
            x[i] -= UData[i * colCount + j] * x[j];

         x[i] = x[i] / U(i, i);
      }
   }

   else
   {
      if (inputMatrix.GetNumRows() > inputMatrix.GetNumColumns())
      {
         Rmatrix Q(inputMatrix.GetNumRows(), inputMatrix.GetNumRows());
         Rmatrix R(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());

         QRFactorization *qr = new QRFactorization(usePivot);
         qr->Factor(inputMatrix, R, Q);

         Rvector y(b.GetSize());
         Rmatrix QTrans = Q.Transpose();
         Real* QTransData = (Real*)QTrans.GetDataVector();
         Integer QRowCount = Q.GetNumRows();
         Real* RData = (Real*)R.GetDataVector();
         Integer RColCount = R.GetNumColumns();

         for (Integer i = 0; i < QRowCount; ++i)
         {
            y[i] = 0;
            for (Integer j = 0; j < QRowCount; ++j)
            {
               y[i] += QTransData[i * QRowCount + j] * b[j];
            }
         }

         for (Integer i = 0; i < RColCount; ++i)
         {
            x[i] = y[i];
         }

         for (Integer i = RColCount - 1; i >= 0; --i)
         {
            for (Integer j = i + 1; j < RColCount; ++j)
               x[i] -= RData[i * RColCount + j] * x[j];

            x[i] = x[i] / RData[i *RColCount + i];
         }

         Rmatrix paramMatrix = qr->GetParameterMatrix();
         Rvector xcopy = x;

         for (Integer i = 0; i < paramMatrix.GetNumRows(); ++i)
         {
            for (Integer j = 0; j < paramMatrix.GetNumColumns(); ++j)
            {
               if (paramMatrix(i, j) == 1)
                  x[j] = xcopy[i];
            }
         }
      }

      else
      {
         // Use algorithm 5.7.2

         Rmatrix Q(inputMatrix.GetNumColumns(), inputMatrix.GetNumColumns());
         Rmatrix R(inputMatrix.GetNumColumns(), inputMatrix.GetNumRows());

         QRFactorization *qr = new QRFactorization(usePivot);
         qr->Factor(inputMatrix.Transpose(), R, Q);

         Rvector y(inputMatrix.GetNumRows());
         Rmatrix partR(inputMatrix.GetNumRows(), inputMatrix.GetNumRows());

         for (Integer rowIndx = 0; rowIndx < partR.GetNumRows(); ++rowIndx)
         {
            for (Integer colIndx = 0; colIndx < partR.GetNumColumns();
                 ++colIndx)
               partR(rowIndx, colIndx) = R(rowIndx, colIndx);
         }

         partR = partR.Transpose();
         y = b;

         for (Integer i = 0; i < y.GetSize(); ++i)
         {
            for (Integer j = i - 1; j >= 0; --j)
               y[i] -= partR(i, j) * y[j];
            y[i] = y[i] / partR(i, i);
         }

         x.SetSize(inputMatrix.GetNumColumns());

         for (Integer i = 0; i < Q.GetNumRows(); ++i)
         {
            x[i] = 0;
            for (Integer j = 0; j < y.GetSize(); ++j)
               x[i] += Q(i, j) * y[j];
         }

         Rmatrix paramMatrix = qr->GetParameterMatrix();
         Rvector xcopy = x;

         for (Integer i = 0; i < paramMatrix.GetNumRows(); ++i)
         {
            for (Integer j = 0; j < paramMatrix.GetNumColumns(); ++j)
            {
               if (paramMatrix(i, j) == 1)
                  x[j] = xcopy[i];
            }
         }

      }
   }

}

//------------------------------------------------------------------------------
// Real GetDeterminant(Rmatrix A)
//------------------------------------------------------------------------------
/**
* Method that determines the determinant of a square matrix
*
* @param A The square matrix a determinant will be calculated for
*
* @return det The determinant of the matrix
*/
//------------------------------------------------------------------------------
Real LUFactorization::Determinant(Rmatrix A)
{
   Rmatrix L(A.GetNumRows(), A.GetNumColumns());
   Rmatrix U(A.GetNumRows(), A.GetNumColumns());

   Factor(A, L, U);
   Real det = U(0,0);

   for (Integer i = 1; i < U.GetNumRows(); ++i)
      det = det*U(i, i);
   det = det*pow(-1, U.GetNumRows());

   return det;
}