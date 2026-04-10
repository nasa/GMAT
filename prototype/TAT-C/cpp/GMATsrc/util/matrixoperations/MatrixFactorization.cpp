//$Id$
//------------------------------------------------------------------------------
//                             MatrixFactorization
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
* Declares MatrixFactorization class.
*/
//------------------------------------------------------------------------------

#include "MatrixFactorization.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_VERBOSE 1

//------------------------------------------------------------------------------
// MatrixFactorization()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
MatrixFactorization::MatrixFactorization()
{
}

//------------------------------------------------------------------------------
// MatrixFactorization(const MatrixFactorization &matrixfactorization)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
MatrixFactorization::MatrixFactorization(const MatrixFactorization
                                         &matrixfactorization)
{
}

//------------------------------------------------------------------------------
// ~MatrixFactorization()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
MatrixFactorization::~MatrixFactorization()
{
}

//------------------------------------------------------------------------------
// MatrixFactorization& operator=(const MatrixFactorization
//                                &matrixfactorization)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
MatrixFactorization& MatrixFactorization::operator=(const MatrixFactorization
                                                    &matrixfactorization)
{
   return *this;
}


//------------------------------------------------------------------------------
// Rmatrix CompressNormalMatrix(const Rmatrix &infMatrix, IntegerArray
//    &removedIndexes, IntegerArray &auxVector, Integer &numRemoved)
//------------------------------------------------------------------------------
/**
* Reduce the dimensionality of an N x N symmetric matrix by 1 for each row /
* column of zeros in the matrix.  If no reduction is done, numRemoved is set
* to 0, and a copy of infMatrix is returned.  If reduction is done, numRemoved
* is set to the number of dimensions removed from the matrix, and an
* N-numRemoved x N-numRemoved matrix is returned, with the zero rows and columns
* of infMatrix removed.  auxVector is an array of additional data which is used
* by ExpandNormalMatrixInverse() to restore the infMatrix, after inversion, back
* to N x N dimensions, by filling in rows and columns of 0s, where they were
* removed during the reduction.  removedIndexes is set to contain the indexes
* of the rows/columns that were removed.
*
* auxVector - an N element integer array.  Each element i of auxVector which
* corresponds to a row/column which was removed is set to -1.  Otherwise
* auxVector[i] contains the number of row/columns removed with index less
* than i.
*
* @param infMatrix The square symmetric matrix for which rows/columns of
*        zeros are to be removed
* @param removedIndexes The indexes of the rows/columns that were removed
* @param auxVector A vector of additional data, used to restore the inverse
*        of the reduced matrix back to its original dimensions, by adding
*        rows/columns of 0s back where they were removed
* @param numRemoved the number of rows/columns removed from infMatrix
*
* @return The infMatrix with rows/columns of zeros removed.
*/
//------------------------------------------------------------------------------
Rmatrix MatrixFactorization::CompressNormalMatrix(const Rmatrix &infMatrix,
   IntegerArray &removedIndexes, IntegerArray &auxVector, Integer &numRemoved)
{
#ifdef DEBUG_VERBOSE
   MessageInterface::ShowMessage("   Information matrix:\n");
   for (UnsignedInt i = 0; i < infMatrix.GetNumRows(); ++i)
   {
      MessageInterface::ShowMessage("      [");
      for (UnsignedInt j = 0; j < infMatrix.GetNumColumns(); ++j)
      {
         MessageInterface::ShowMessage(" %.12lf ", infMatrix(i, j));
      }
      MessageInterface::ShowMessage("]\n");
   }
#endif

   int iSize = infMatrix.GetNumRows();
   Rmatrix reducedMatrix;

   numRemoved = 0;
   removedIndexes.clear();
   auxVector.resize(iSize);

   for (int index = 0; index < iSize; index++)
   {
      bool isRowOfZeros = true, isColOfZeros = true;

      for (int j = 0; j < iSize; j++)
      {
         if (isRowOfZeros && infMatrix(index, j) != 0)
            isRowOfZeros = false;
         if (isColOfZeros && infMatrix(j, index) != 0)
            isColOfZeros = false;
         if (!isRowOfZeros && !isColOfZeros)
            break;
      }

      // matrix should be symmetric, if row is 0s, col should be 0s and
      // vice versa
      if (isRowOfZeros || isColOfZeros) {
         numRemoved++;
         auxVector.at(index) = -1;
         removedIndexes.push_back(index);
      }
      else {
         auxVector.at(index) = numRemoved;
      }
   }

   // compress matrix (if numRemoved == 0, this is a matrix copy)
   reducedMatrix.SetSize(iSize - numRemoved, iSize - numRemoved);

   for (int row = 0; row < iSize; row++)
   {
      for (int col = 0; col < iSize; col++)
      {
         // only move element if not part of a zero row or column
         if (auxVector.at(row) > -1 && auxVector.at(col) > -1)
         {
            reducedMatrix(row - auxVector.at(row), col - auxVector.at(col))
               = infMatrix(row, col);
         }
      }
   }

#ifdef DEBUG_VERBOSE
   MessageInterface::ShowMessage("   numRemoved: %d\n", numRemoved);
   MessageInterface::ShowMessage("   removedIndexes:\n");
   MessageInterface::ShowMessage("      [");
   for (UnsignedInt i = 0; i < removedIndexes.size(); ++i)
      MessageInterface::ShowMessage(" %d ", removedIndexes.at(i));
   MessageInterface::ShowMessage("]\n");
   MessageInterface::ShowMessage("   auxVector:\n");
   MessageInterface::ShowMessage("      [");
   for (UnsignedInt i = 0; i < auxVector.size(); ++i)
      MessageInterface::ShowMessage(" %d ", auxVector.at(i));
   MessageInterface::ShowMessage("]\n");
   MessageInterface::ShowMessage("   Compressed information matrix:\n");
   for (UnsignedInt i = 0; i < reducedMatrix.GetNumRows(); ++i)
   {
      MessageInterface::ShowMessage("      [");
      for (UnsignedInt j = 0; j < reducedMatrix.GetNumColumns(); ++j)
      {
         MessageInterface::ShowMessage(" %.12lf ", reducedMatrix(i, j));
      }
      MessageInterface::ShowMessage("]\n");
   }
#endif

   return reducedMatrix;
}


//------------------------------------------------------------------------------
// Rmatrix ExpandNormalMatrixInverse(const Rmatrix &covMatrix,
//    const IntegerArray &auxVector, const Integer &numRemoved)
//------------------------------------------------------------------------------
/**
* Given the inverse of an N x N symmetric normal matrix, which had numRemoved
* rows/columns of zeros removed before inversion, return a copy of the matrix
* which has been expanded back to size N x N by inserting rows/columns of 0s
* at the indexes where they were removed from the normal matrix.
*
* @param covMatrix The N-numRemoved x N-numRemoved inverse of the normal matrix
* @param auxVector The auxiliary vector which was returned by
*        CompressNormalMatrix() when the normal matrix was reduced
* @param numRemoved The number of rows/columns which were removed when the
*        normal matrix was reduced, and will be restored to covMatrix
*
* @return A copy of covMatrix, which has expanded back to size N x N by
*    inserting rows/columns of 0s at the indexes where they were removed from
*    the normal matrix.
*/
//------------------------------------------------------------------------------
Rmatrix MatrixFactorization::ExpandNormalMatrixInverse(const Rmatrix &covMatrix,
   const IntegerArray &auxVector, const Integer &numRemoved)
{
#ifdef DEBUG_VERBOSE
   MessageInterface::ShowMessage("   Compressed covariance matrix:\n");
   for (UnsignedInt i = 0; i < covMatrix.GetNumRows(); ++i)
   {
      MessageInterface::ShowMessage("      [");
      for (UnsignedInt j = 0; j < covMatrix.GetNumColumns(); ++j)
      {
         MessageInterface::ShowMessage(" %.12lf ", covMatrix(i, j));
      }
      MessageInterface::ShowMessage("]\n");
   }
   MessageInterface::ShowMessage("   auxVector:\n");
   MessageInterface::ShowMessage("      [");
   for (UnsignedInt i = 0; i < auxVector.size(); ++i)
      MessageInterface::ShowMessage(" %d ", auxVector.at(i));
   MessageInterface::ShowMessage("]\n");
   MessageInterface::ShowMessage("   numRemoved: %d\n", numRemoved);
#endif

   int iSize = covMatrix.GetNumRows() + numRemoved;
   Rmatrix expandedCovMatrix;

   expandedCovMatrix.SetSize(iSize, iSize, false);

   for (int row = iSize - 1; row >= 0; row--)
   {
      for (int col = iSize - 1; col >= 0; col--)
      {
         if (auxVector.at(row) == -1 || auxVector.at(col) == -1)
            expandedCovMatrix(row, col) = 0.0;
         else
            expandedCovMatrix(row, col) =
               covMatrix(row - auxVector.at(row), col - auxVector.at(col));
      }
   }

#ifdef DEBUG_VERBOSE
   MessageInterface::ShowMessage("   Uncompressed covariance matrix:\n");
   for (UnsignedInt i = 0; i < expandedCovMatrix.GetNumRows(); ++i)
   {
      MessageInterface::ShowMessage("      [");
      for (UnsignedInt j = 0; j < expandedCovMatrix.GetNumColumns(); ++j)
      {
         MessageInterface::ShowMessage(" %.12lf ", expandedCovMatrix(i, j));
      }
      MessageInterface::ShowMessage("]\n");
   }
#endif

   return expandedCovMatrix;
}


//------------------------------------------------------------------------------
// Integer PackedArrayIndex(Integer N, Integer row, Integer col)
//------------------------------------------------------------------------------
/**
* Given the upper triangle of an N x N symmetric matrix stored in a linear
* array, and given a (row, column) index in the upper triangle, 0 <= row < N,
* row <= col < N, return the index in the linear array where that element is
* stored.
*
* Note that for the array, matrix elements (0, 0) to (0, N-1) are stored in
* elements [0] to [N-1] of the array, matrix elements (1, 1) to (1, N-1) are
* store in elements [N] to [2N-2], etc.
*
* @param N The number of rows and columns in the square symmetric matrix
* @param row The row index of the element.  0 <= row < N.
* @param col The column index of the element.  row <= col < N.
*
* @return The array index for matrix element (row, col)
*/
//------------------------------------------------------------------------------
Integer MatrixFactorization::PackedArrayIndex(Integer N, Integer row,
   Integer col)
{
   // For element (i, j), the array index is the total number of elements in
   // rows 0 to i = 1, plus the number of elements in row i up to element j.
   // The number of elements in rows 0 to i - 1 is equal to the number of
   // elements in the entire upper triangle of the matrix, minus the elements
   // in rows i to N-1.  Which is the sum of the integers from 1 to N, minus
   // the sum of the integers from 1 to N-i.  Which is N(N+1)/2 minus
   // (N-i)(N-i+1)/2.  The number of elements in row i is j - i + 1.  Then
   // subtract 1 because array indexes are 0 based.

   return (2 * N * row - row * row + row) / 2 + col - row;
}
