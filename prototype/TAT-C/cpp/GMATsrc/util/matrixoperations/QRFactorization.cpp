//$Id$
//------------------------------------------------------------------------------
//                                QRFactorization
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
* Defines QR factorization methods.
*/
//------------------------------------------------------------------------------

#include "QRFactorization.hpp"
#include "UtilityException.hpp"
#include "LUFactorization.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// QRFactorization()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
QRFactorization::QRFactorization(bool pivotOption)
{
   usePivot = pivotOption;
   c = 0;
   s = 0;
   permuMatrix.SetSize(0, 0);
}

//------------------------------------------------------------------------------
// QRFactorization(const QRFactorization &qrfactorizationgmat)
//------------------------------------------------------------------------------
/**
* Copy Constructor
*/
//------------------------------------------------------------------------------
QRFactorization::QRFactorization(const QRFactorization &qrfactorizationgmat)
{
}


//------------------------------------------------------------------------------
// ~QRFactorization()
//------------------------------------------------------------------------------
/**
* Class destructor
*/
//------------------------------------------------------------------------------
QRFactorization::~QRFactorization()
{
}

//------------------------------------------------------------------------------
// QRFactorization& operator=(const QRFactorization &qrfactorization)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
QRFactorization& QRFactorization::operator=(const QRFactorization 
                                            &qrfactorization)
{
   if (this != &qrfactorization)
   {
      //Call base class method
      MatrixFactorization::operator=(qrfactorization);
   }

   return *this;
}

//------------------------------------------------------------------------------
// void Factor(const Rmatrix A, Rmatrix &R, Rmatrix &Q)
//------------------------------------------------------------------------------
/**
* Method used to factor the matrix with QR factorization (A = QR) based off
* the third option in algorithm 5.2.2 from Gene H. Golub and Chales F. Van
* Loan with modifications added for column pivoting similar to what is used in
* algorithm 5.4.1.
*
* @param A The matrix that will be factored
* @param &R The upper triangular matrix, same dimensions as A
* @param &Q The orthogonal matrix, has dimensions mxm where m is the number of
*        rows in A
*/
//------------------------------------------------------------------------------
void QRFactorization::Factor(const Rmatrix A, Rmatrix &R, Rmatrix &Q)
{
   m = A.GetNumRows();
   n = A.GetNumColumns();

   R = A;

   double elemHold; // Holds an element from Givens calculation to be placed 
                    // into original matrix later

   bool lessRows = false; // Boolian to keep track of whether m < n initially
   Integer oldRowCount = m;

   // Add rows of zeros to R and both rows and columns of zeros to Q to be
   // removed later
   if (m < n)
   {
      lessRows = true;
      R.ChangeSize(n, n, false);
      Q.SetSize(n, n);
      m = n;
   }

   for (Integer i = 0; i < Q.GetNumRows(); ++i)
      Q(i, i) = 1;

   if (n == 0)
      return;

   // Set up neccessary Real* arrays for faster matrix manipulation
   Real *qrOrthogData = (Real*)Q.GetDataVector(); // Q
   //Real *qrOrthogOld = new Real[m * m]; // Q from previous step
   Real *qrOrthogNext = new Real[m * m]; // Current rotation matrix
   Real *qrUpperData = (Real*)R.GetDataVector(); // R

   for (Integer i = 0; i < m; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         if (i == j)
         {
            //qrOrthogOld[i * m + j] = 1;
            qrOrthogNext[i * m + j] = 1;
         }
         else
         {
            //qrOrthogOld[i * m + j] = 0;
            qrOrthogNext[i * m + j] = 0;
         }
      }
   }

   // Define pivot parameters
   Rvector cPivot(n);
   Integer rank;
   Real tau;
   Integer colToSwitch;
   Rvector paramVector(n - 1);
   Real oldColumn;
   Real oldPivotElem;

   // Begin algorithm 5.2.2

   // If using pivot, prepare column pivot vector
   if (usePivot)
   {

      for (Integer pivotElem = 0; pivotElem < n; ++pivotElem)
      {
         for (Integer matrixRowNum = 0; matrixRowNum < m; ++matrixRowNum)
         {
            cPivot[pivotElem] +=
               pow(qrUpperData[matrixRowNum * n + pivotElem], 2);
         }
      }

      rank = -1;
      tau = cPivot[0];

      for (Integer pivotElem = 1; pivotElem < n; ++pivotElem)
      {
         if (cPivot[pivotElem] > cPivot[pivotElem - 1])
            tau = cPivot[pivotElem];
      }

      for (Integer k = 0; k < n; ++k)
      {
         if (cPivot[k] == tau)
         {
            colToSwitch = k;
            break;
         }
      }
   }

   Integer index;
   Real qElemHold;
   //Begin solving for elements in qrOrthog and qrUpper
   for (Integer j = 0; j < n; ++j)
   {
      if (usePivot)
      {
         rank++;
         if (rank < n - 1)
            paramVector[rank] = colToSwitch;

         // Pivot columns
         for (Integer rowNum = 0; rowNum < m; ++rowNum)
         {
            oldColumn = qrUpperData[rowNum * n + rank];
            qrUpperData[rowNum * n + rank] =
               qrUpperData[rowNum * n + colToSwitch];
            qrUpperData[rowNum * n + colToSwitch] = oldColumn;

            oldPivotElem = cPivot[rank];
            cPivot[rank] = cPivot[colToSwitch];
            cPivot[colToSwitch] = oldPivotElem;
         }
      }

      for (Integer i = m - 1; i > j; --i)
      {
         Givens(qrUpperData[j * n + j], qrUpperData[i * n + j]);

         for (Integer jj = 0; jj < m; ++jj)
         {
            index = jj * m;
            if (jj < n && jj >= j)
            {
               elemHold = c * qrUpperData[j * n + jj] - s *
                  qrUpperData[i * n + jj];
               qrUpperData[i * n + jj] = s * qrUpperData[j * n + jj] +
                  c * qrUpperData[i * n + jj];
               qrUpperData[j * n + jj] = elemHold;
            }

            qElemHold = qrOrthogData[index + j];
            qrOrthogData[index + j] =
               qElemHold * c + qrOrthogData[index + i] * -s;
            qrOrthogData[index + i] =
               qElemHold * s + qrOrthogData[index + i] * c;
         }
      }

      // Update pivot parameters
      if (usePivot && (rank < n - 1))
      {
         for (Integer pivotElem = rank + 1; pivotElem < n; ++pivotElem)
            cPivot[pivotElem] -= pow(qrUpperData[rank * n + pivotElem], 2);

         tau = cPivot[rank + 1];

         for (Integer pivotElem = rank + 2; pivotElem < n; ++pivotElem)
         {
            if (cPivot[pivotElem] > cPivot[pivotElem - 1])
               tau = cPivot[pivotElem];
         }

         for (Integer k = rank + 1; k < n; ++k)
         {
            if (cPivot[k] == tau)
            {
               colToSwitch = k;
               break;
            }
         }
      }
   }
   
   if (lessRows)
   {
      R.ChangeSize(oldRowCount, n, false);
      Q.ChangeSize(oldRowCount, oldRowCount, false);
   }
   
   m = R.GetNumRows();

   // Set parameter matrix if pivoting was used
   permuMatrix.SetSize(n, n);
   permuMatrix = Rmatrix::Identity(n);
   if (usePivot)
   {
      Real oldRow;
      for (Integer i = 0; i < paramVector.GetSize(); ++i)
      {
         for (Integer j = 0; j < n; ++j)
         {
            oldRow = permuMatrix((Integer)paramVector[i], j);
            permuMatrix((Integer)paramVector[i], j) = permuMatrix(i, j);
            permuMatrix(i, j) = oldRow;
         }
      }
   }
}

//------------------------------------------------------------------------------
// void RemoveFromQR(const Rmatrix R, const Rmatrix Q,
// std::string dimensionToRemove, Integer locationToRemove, Rmatrix &R1,
// Rmatrix &Q1)
//------------------------------------------------------------------------------
/**
* Method used to update the QR factorization of a matrix by removing a row or
* column from the original matrix.  Algorithms are based on section 12.5 of
* Matrix Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan.
*
* @param R Original upper triangular matrix from initial QR factorization
* @param Q Original orthogonal matrix from initial QR factorization
* @param dimensionToRemove String entry determining whether a row (entered as
*        "row") or column (entered as "col") should be removed
* @param locationToRemove Index of which row/column from the original
*        factored matrix should be removed
* @param &R1 New upper triangular matrix after removing row/column from A
* @param &Q1 New orthogonal matrix after removing row/column from A
*/
//------------------------------------------------------------------------------
void QRFactorization::RemoveFromQR(Rmatrix R, Rmatrix Q,
   std::string dimensionToRemove, Integer locationToRemove, Rmatrix &R1,
   Rmatrix &Q1)
{
   Rmatrix H;
   m = R.GetNumRows();
   n = R.GetNumColumns();
   Real* RData = (Real*)R.GetDataVector();

   if (locationToRemove < 0)
   {
      std::string errMessage = "The row or column number to remove must be a"
         " positive integer or zero.";
      throw UtilityException(errMessage);
   }

   if (dimensionToRemove == "col")
   {
      // Remove a column using the steps detailed in section 12.5.2 in Matrix
      // Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan

      if (locationToRemove > R.GetNumColumns() - 1)
      {
         std::string errMessage = "The column index to remove is out of"
            " bounds.";
         throw UtilityException(errMessage);
      }

      R1.SetSize(m, n - 1, true);
      Real* R1Data = (Real*)R1.GetDataVector();

      for (Integer i = 0; i < m; ++i)
      {
         for (Integer j = 0; j < n - 1; ++j)
         {
            if (i < m && j < locationToRemove)
               R1Data[i * (n - 1) + j] = RData[i * n + j];
            if (i < m && j >= locationToRemove)
               R1Data[i * (n - 1) + j] = RData[i * n + j + 1];
            if (i >= m)
               R1Data[i * (n - 1) + j] = 0;
         }
      }

      Q1 = Q;
      Real* Q1Data = (Real*)Q1.GetDataVector();
      Real elemHold = 0;
      Real qElemHold = 0;
      Integer jMax = 0;
      if (m <= n - 1)
         jMax = m - 1;
      else
         jMax = n - 1;

      for (Integer j = 0; j < jMax; ++j)
      {
         Givens(R1Data[j * (n - 1) + j], R1Data[(j + 1) * (n - 1) + j]);
         for (Integer jj = j; jj < n - 1; ++jj)
         {
            elemHold = R1Data[j * (n - 1) + jj];
            R1Data[j * (n - 1) + jj] =
               c * elemHold - s * R1Data[(j + 1) * (n - 1) + jj];
            R1Data[(j + 1) * (n - 1) + jj] =
               s * elemHold + c * R1Data[(j + 1) * (n - 1) + jj];
         }

         for (Integer jj = 0; jj < m; ++jj)
         {
            qElemHold = Q1Data[jj * m + j];
            Q1Data[jj * m + j] =
               qElemHold * c + Q1Data[jj * m + j + 1] * -s;
            Q1Data[jj * m + j + 1] =
               qElemHold * s + Q1Data[jj * m + j + 1] * c;
         }
      }
   }

   else
   {
      if (dimensionToRemove == "row")
      {
         // Remove a row using the steps detailed in section 12.5.3 in Matrix
         // Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan

         if (locationToRemove > R.GetNumRows() - 1)
         {
            std::string errMessage = "The row index to remove is out of"
               " bounds.";
            throw UtilityException(errMessage);
         }

         H = R;
         Rvector rowToDelete(Q.GetNumColumns());
         for (Integer i = 0; i < m; ++i)
         {
            rowToDelete[i] = Q(locationToRemove, i);
         }
         
         Real elemHold = 0;
         Real qElemHold = 0;
         Rmatrix QHold = Q;
         Real* HData = (Real*)H.GetDataVector();
         Real* QHoldData = (Real*)QHold.GetDataVector();
         for (Integer i = rowToDelete.GetSize() - 1; i > 0; --i)
         {
            Givens(rowToDelete[i - 1], rowToDelete[i]);
            elemHold = rowToDelete[i - 1];
            rowToDelete[i - 1] = c * rowToDelete[i - 1] - s * rowToDelete[i];
            rowToDelete[i] = s * elemHold + c * rowToDelete[i];

            for (Integer jj = 0; jj < H.GetNumColumns(); ++jj)
            {
                  elemHold = HData[(i - 1) * n + jj];
                  HData[(i - 1) * n + jj] = c * elemHold -
                     s * HData[i * n + jj];
                  HData[i * n + jj] = s * elemHold + c * HData[i * n + jj];
            }

            for (Integer jj = 0; jj < QHold.GetNumColumns(); jj++)
            {
                  qElemHold = QHoldData[jj * m + i - 1];
                  QHoldData[jj * m + i - 1] = qElemHold * c +
                     QHoldData[jj * m + i] * -s;
                  QHoldData[jj * m + i] = qElemHold * s +
                     QHoldData[jj * m + i] * c;
            }
         }

         R1.SetSize(m - 1, n);
         Real* R1Data = (Real*)R1.GetDataVector();
         for (Integer i = 0; i < m - 1; ++i)
         {
            for (Integer j = 0; j < n; ++j)
            {
               R1Data[i * n + j] = HData[(i + 1) * n + j];
            }
         }

         Q1.SetSize(m - 1, m - 1);
         Real* Q1Data = (Real*)Q1.GetDataVector();
         for (Integer i = 0; i < m - 1; ++i)
         {
            for (Integer j = 0; j < m - 1; ++j)
            {
               if (i < locationToRemove && j < 0)
                  Q1Data[i * (m -1) + j] = QHoldData[i * m + j];
               if (i < locationToRemove && j >= 0)
                  Q1Data[i * (m - 1) + j] = QHoldData[i * m + j + 1];
               if (i >= locationToRemove && j < 0)
                  Q1Data[i * (m - 1) + j] = QHoldData[(i + 1) * m + j];
               if (i >= locationToRemove && j >= 0)
                  Q1Data[i * (m - 1) + j] = QHoldData[(i + 1) * m + j + 1];
            }
         }
      }

      else
      {
         std::string errMessage = "Dimension type must be either \"row\" or"
            " \"col\"";
         throw UtilityException(errMessage);
      }
   }
}

//------------------------------------------------------------------------------
// void AddToQR(const Rmatrix R, const Rmatrix Q,
// std::string dimensionToRemove, Integer locationToRemove, Rmatrix &R1,
// Rmatrix &Q1)
//------------------------------------------------------------------------------
/**
* Method used to update the QR factorization of a matrix by adding a row or
* column from the original matrix.  Algorithms are based on section 12.5 of
* Matrix Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan.
*
* @param R Original upper triangular matrix from initial QR factorization
* @param Q Original orthogonal matrix from initial QR factorization
* @param dimensionToInsert String entry determining whether a row (entered as
*        "row") or column (entered as "col") should be added
* @param locationToInsert Index of which row/column from the original
*        factored matrix should be added
* @param &R1 New upper triangular matrix after removing row/column from A
* @param &Q1 New orthogonal matrix after removing row/column from A
*/
//------------------------------------------------------------------------------
void QRFactorization::AddToQR(Rmatrix R, Rmatrix Q,
   std::string dimensionToInsert, Integer locationToInsert,
   Rvector newElements, Rmatrix &R1, Rmatrix &Q1)
{
   if (locationToInsert < 0)
   {
      std::string errMessage = "The row or column number to remove must be a"
         " positive integer or zero.";
      throw UtilityException(errMessage);
   }

   m = R.GetNumRows();
   n = R.GetNumColumns();
   Real elemHold = 0;
   Real qElemHold = 0;
   Q1 = Q;

   Real* RData = (Real*)R.GetDataVector();
   Real* QData = (Real*)Q.GetDataVector();


   if (dimensionToInsert == "col")
   {
      // Insert a column using the steps detailed in section 12.5.2 in Matrix
      // Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan

      if (locationToInsert > n)
      {
         std::string errMessage = "The column index to remove is out of"
            " bounds.";
         throw UtilityException(errMessage);
      }

      if (newElements.GetSize() != m)
      {
         std::string errMessage = "The new column does not have the correct"
            " dimensions.";
         throw UtilityException(errMessage);
      }

      Real* Q1Data = (Real*)Q1.GetDataVector();
      R1.SetSize(m, n + 1);
      Real* R1Data = (Real*)R1.GetDataVector();

      for (Integer i = 0; i < m; ++i)
      {
         for (Integer j = 0; j < n + 1; ++j)
         {
            if (j < locationToInsert)
               R1Data[i * (n + 1) + j] = RData[i * n + j];
            if (j == locationToInsert)
            {
               for (Integer jj = 0; jj < m; ++jj)
               {
                  R1Data[i * (n + 1) + j] +=
                     QData[jj * m + i] * newElements[jj];
               }
            }
            if (j > locationToInsert)
               R1Data[i * (n + 1) + j] = RData[i * n + j - 1];
         }
      }

      for (Integer i = m - 1; i > locationToInsert; --i)
      {
         Givens(R1(i - 1, locationToInsert), R1(i, locationToInsert));
         for (Integer j = 0; j < n + 1; ++j)
         {
            elemHold = R1Data[(i - 1) * (n + 1) + j];
            R1Data[(i - 1) * (n + 1) + j] =
               c * elemHold - s * R1Data[i * (n + 1) + j];
            R1Data[i * (n + 1) + j] =
               s * elemHold + c * R1Data[i * (n + 1) + j];
         }

         for (Integer j = 0; j < m; ++j)
         {
            qElemHold = Q1Data[j * m + i - 1];
            Q1Data[j * m + i - 1] = qElemHold * c + Q1Data[j * m + i] * -s;
            Q1Data[j * m + i] = qElemHold * s + Q1Data[j * m + i] * c;
         }
      }
   }

   else
   {
      if (dimensionToInsert == "row")
      {
         // Insert a row using the steps detailed in section 12.5.3 in Matrix
         // Computations (3rd ed.) by Gene H. Golub and Charles F. Van Loan

         if (locationToInsert > m - 1)
         {
            std::string errMessage = "The row index to remove is out of"
               " bounds.";
            throw UtilityException(errMessage);
         }

         if (newElements.GetSize() != n)
         {
            std::string errMessage = "The new row does not have the correct"
               " dimensions.";
            throw UtilityException(errMessage);
         }

         R1.SetSize(m + 1, n);
         Real* R1Data = (Real*)R1.GetDataVector();
         for (Integer i = 0; i < m + 1; ++i)
         {
            for (Integer j = 0; j < n; ++j)
            {
               if (i == 0)
                  R1Data[i * n + j] = newElements[j];
               else
                  R1Data[i * n + j] = RData[(i - 1) * n + j];
            }
         }

         Q1.SetSize(m + 1, m + 1);
         Real* Q1Data = (Real*)Q1.GetDataVector();
         for (Integer i = 0; i < m + 1; ++i)
         {
            for (Integer j = 0; j < m + 1; ++j)
            {
               if (i == 0 && j == 0)
                  Q1Data[i * (m + 1) + j] = 1;
               if (i == 0 && j != 0)
                  Q1Data[i * (m + 1) + j] = 0;
               if (i != 0 && j == 0)
                  Q1Data[i * (m + 1) + j] = 0;
               if (i != 0 && j != 0)
                  Q1Data[i * (m + 1) + j] = QData[(i - 1) * m + j - 1];
            }
         }

         Real maxDiag;
         if (m + 1 > n)
            maxDiag = n;
         else
            maxDiag = m;

         for (Integer i = 1; i <= maxDiag; ++i)
         {
            Givens(R1Data[(i - 1) * n + i - 1], R1Data[i * n + i - 1]);
            for (Integer j = i - 1; j < n; ++j)
            {
               elemHold = R1Data[(i - 1) * n + j];
               R1Data[(i - 1) * n + j] = c * elemHold - s * R1Data[i * n + j];
               R1Data[i * n + j] = s * elemHold + c * R1Data[i* n + j];
            }

            for (Integer j = 0; j < m + 1; ++j)
            {
               qElemHold = Q1Data[j * (m + 1) + i - 1];
               Q1Data[j * (m + 1) + i - 1] = c * qElemHold -
                  s * Q1Data[j * (m + 1) + i];
               Q1Data[j * (m + 1) + i] = s * qElemHold +
                  c * Q1Data[j * (m + 1) + i];
            }
         }
         
         Rmatrix QOld = Q1;
         Real* QOldData = (Real*)QOld.GetDataVector();
         for (Integer i = 0; i < m + 1; ++i)
         {
            for (Integer j = 0; j < m + 1; ++j)
            {
               if (i < locationToInsert)
                  Q1Data[i * (m + 1) + j] = QOldData[(i + 1) * (m + 1) + j];
               else
               {
                  if (i > locationToInsert)
                     Q1Data[i * (m + 1) + j] = QOldData[i * (m + 1) + j];
                  else
                     Q1Data[i * (m + 1) + j] = QOldData[j];
               }
            }     
         }
      }

      else
      {
         std::string errMessage = "Dimension type must be either \"row\" or"
            " \"col\"";
         throw UtilityException(errMessage);
      }
   }
}

//------------------------------------------------------------------------------
// void Invert(Rmatrix &inputMatrix)
//------------------------------------------------------------------------------
/**
* Method used to invert the matrix with QR factorization (inv(A) =
* inv(Q)*inv(R) = inv(R)*transpose(Q))
*
* @param A The matrix that will be factored
* @param &R The upper triangular matrix, same dimensions as A
* @param &Q The orthogonal matrix, has dimensions mxm where m is the number of
*        rows in A
*/
//------------------------------------------------------------------------------
void QRFactorization::Invert(Rmatrix &inputMatrix)
{
   if (inputMatrix.GetNumRows() != inputMatrix.GetNumColumns())
   {
      std::string errMessage = "Matrix must be square to invert. \n";
      throw UtilityException(errMessage);
   }

   determinant = Determinant(inputMatrix);

   if (determinant == 0)
   {
      std::string errMessage =
         "The matrix is singular, inverse cannot be computed.\n";
      throw UtilityException(errMessage);
   }

   Rmatrix R(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());
   Rmatrix Q(inputMatrix.GetNumRows(), inputMatrix.GetNumColumns());

   Factor(inputMatrix, R, Q);


   // Calculate inv(R)
   Rmatrix invR(m, n);

   for (Integer colNum = 0; colNum < n; ++colNum)
   {
      for (Integer i = 0; i < n; ++i)
      {
         if (i != colNum)
            invR(i, colNum) = 0;
         else
            invR(i, colNum) = 1;
      }

      for (Integer i = n - 1; i >= 0; --i)
      {
         for (Integer j = i + 1; j < n; ++j)
            invR(i, colNum) -= R(i, j) * invR(j, colNum);

         invR(i, colNum) = invR(i, colNum) / R(i, i);
      }
   }

   // Calculate inv(Q) = transpose(Q)
   Rmatrix invQ(m, m);

   for (Integer i = 0; i < m; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         invQ(i, j) = Q(j, i);
      }
   }

   // Calculate inv(inputMatrix) = inv(R) * transpose(Q)
   double newElem;

   for (Integer i = 0; i < m; ++i)
   {
      for (Integer j = 0; j < n; ++j)
      {
         newElem = 0;
         for (Integer index = 0; index < Q.GetNumRows(); ++index)
            newElem += invR(i, index) * invQ(index, j);

         inputMatrix(i, j) = newElem;
      }
   }

   // Use the parameter matrix to interchange rows of inverse matrix in order
   // to get true inverse of the orginal matrix input

   if (usePivot)
   {
      Rmatrix oldMatrix = inputMatrix;
      for (Integer i = 0; i < n; ++i)
      {
         for (Integer j = 0; j < n; ++j)
         {
            if (permuMatrix(i, j) == 1)
            {
               for (Integer jj = 0; jj < n; ++jj)
                  inputMatrix(j, jj) = oldMatrix(i, jj);
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void Givens(const double el1, const double el2)
//------------------------------------------------------------------------------
/**
* Method used to calculate Givens rotations (used for factoring method).
* These equations are taken from equation set 5.1.10 from Gene H. Golub and
* Charles F. Van Loan.
*
* @param el1 Scalar taken from R as it is being calculated
* @param el2 Scalar taken from R as it is being calculated
*/
//------------------------------------------------------------------------------
void QRFactorization::Givens(const double el1, const double el2)
{
   double tau = 0.0;
   if (el2 == 0)
   {
      c = 1;
      s = 0;
   }
   else
   {
      if (std::abs(el2) > std::abs(el1))
      {
         tau = -el1 / el2;
         s = 1.0 / sqrt(1 + pow(tau, 2));
         c = s * tau;
      }
      else
      {
         tau = -el2 / el1;
         c = 1.0 / sqrt(1 + pow(tau, 2));
         s = c * tau;
      }
   }
}

//------------------------------------------------------------------------------
// Real Determinant(Rmatrix A)
//------------------------------------------------------------------------------
/**
* Method that determines the determinant of a square matrix
*
* @param A The square matrix a determinant will be calculated for
*
* @return det The determinant of the matrix
*/
//------------------------------------------------------------------------------
Real QRFactorization::Determinant(Rmatrix A)
{
   Real det;

   LUFactorization *lu = new LUFactorization();
   det = lu->Determinant(A);

   return det;
}

//------------------------------------------------------------------------------
// Rmatrix GetParameterMatrix()
//------------------------------------------------------------------------------
/**
* Method that returns the parameter matrix
*
*/
//------------------------------------------------------------------------------
Rmatrix QRFactorization::GetParameterMatrix()
{
   return permuMatrix;
}
