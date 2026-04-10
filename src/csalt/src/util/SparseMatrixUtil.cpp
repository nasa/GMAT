//------------------------------------------------------------------------------
//                      SparseMatrixUtil1
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Youngkwang Kim
// Created: 2015/06/23
//
/**
 * Implementation for the BOOST class
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "SparseMatrixUtil.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SPARSE_MATRIX_UTIL
//#define DEBUG_SPARSE_MATRIX_SET


//------------------------------------------------------------------------------
// void SetElement(RSMatrix  &spMat, Integer rowIdx,
//                 Integer   colIdx, Real    value)
//------------------------------------------------------------------------------
/**
 * Set the specified element of the sparse matrix
 *
 * @param <spMat>    the sparse matrix
 * @param <rowIdx>   the row index of the element
 * @param <colIdx>   the column index of the element
 * @param <value>    the value of the element
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetElement(RSMatrix  &spMat, Integer rowIdx,
                                  Integer   colIdx, Real    value)
{
   spMat(rowIdx, colIdx) = value;
};

//------------------------------------------------------------------------------
// void SetSize(RSMatrix &spMat, Integer rowSize, Integer colSize)
//------------------------------------------------------------------------------
/**
 * Set or reset the size of the sparse matrix
 *
 * @param <spMat>    the sparse matrix
 * @param <rowSize>  the row size of the matrix
 * @param <colSize>  the column size of the matrix
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSize(RSMatrix &spMat, Integer rowSize,
                               Integer  colSize)
{
   spMat.resize(rowSize, colSize, false);
};

//------------------------------------------------------------------------------
// void SetSparsityPattern(RSMatrix     &spMat,
//                         Integer      rowSize,
//                         Integer      colSize,
//                         IntegerArray *rowIdxVec,
//                         IntegerArray *colIdxVec,
//                         bool         hasZeros)
//------------------------------------------------------------------------------
/**
 * Initialize RSMatrix with sparsity pattern.
 * Fill the matrix with zeros or ones based on the boolean.
 *
 * @param <spMat>      the sparse matrix, which is the target
 * @param <rowSize>    the number of rows of the sparse matrix
 * @param <colSize>    the number of columns of the sparse matrix
 * @param <rowIdxVec>  STL vector where the row indices of nonzeros saved
 * @param <colIdxVec>  STL vector where the column indices of nonzeros saved
 * @param <hasZeros>   a boolean indicating whether to fill with zeros or ones
 *
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparsityPattern(RSMatrix &spMat,
                                          Integer  rowSize, Integer colSize,
                                          const IntegerArray *rowIdxVec,
                                          const IntegerArray *colIdxVec,
                                          bool  hasZeros)
{
   bool preserve = false;
   spMat.resize(rowSize, colSize, preserve);


   UnsignedInt numNonZeros = (*rowIdxVec).size();
   if (numNonZeros > (UnsignedInt)(rowSize*colSize))
   {
      std::stringstream errmsg("");
      errmsg << "To many nonzero elements!; return an empty matrix";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if ((numNonZeros != (*rowIdxVec).size())
      || (numNonZeros != (*colIdxVec).size()))
   {
      std::stringstream errmsg("");
      errmsg << "input vectors do not have the same size!; ";
      errmsg << "return an empty matrix" << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // create iterator for std::vector
   IntegerArray::const_iterator stdConstIterator;

   /// find the maximum row idx
   stdConstIterator = max_element((*rowIdxVec).begin(), (*rowIdxVec).end());
   UnsignedInt maxRow = *stdConstIterator;

   /// find the maximum column idx
   stdConstIterator = max_element((*colIdxVec).begin(), (*colIdxVec).end());
   UnsignedInt maxCol = *stdConstIterator;

   if ((maxRow > (UnsignedInt)(rowSize - 1)) ||
       (maxCol > (UnsignedInt)(colSize - 1)))
   {
      std::stringstream errmsg("");
      errmsg << "a row or column index is out of range; ";
      errmsg << "return an empty matrix." << std::endl;
      throw LowThrustException(errmsg.str());
   }

   for (UnsignedInt idx = 0; idx < numNonZeros; ++idx)
   {
      if (hasZeros == true)
         spMat((*rowIdxVec)[idx], (*colIdxVec)[idx]) = 0.0;
      else
         spMat((*rowIdxVec)[idx], (*colIdxVec)[idx]) = 1.0;
   }
}

//------------------------------------------------------------------------------
// void     SetSparseMatrix(RSMatrix &spMat,
//                         Integer   rowSize, Integer colSize,
//                         IntegerArray *rowIdxVec, 
//                         IntegerArray *colIdxVec,
//                         std::vector<Real> *valueVec)
//------------------------------------------------------------------------------
/**
 * Convert the three vector form into RSMatrix;
 *
 * @param <spMat>     the sparse matrix, which is the target.
 * @param <rowSize>   the number of rows of the sparse matrix.
 * @param <colSize>   the number of columns of the sparse matrix.
 * @param <rowIdxVec> STL vector where the row indices of nonzeros saved.
 * @param <colIdxVec> STL vector where the column indices of nonzeros saved.
 * @param <valueVec>  STL vector where the nonzeros values saved.
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseMatrix(RSMatrix &spMat,
                                       Integer  rowSize, Integer colSize,
                                       const IntegerArray *rowIdxVec,
                                       const IntegerArray *colIdxVec,
                                       const std::vector<Real> *valueVec)
{
   bool preserve = false;
   spMat.resize(rowSize, colSize, preserve);

   // if valueVec is empty, do nothing
   if ((*valueVec).empty() == true)
   {
      return;
   }

   UnsignedInt numNonZeros = (*valueVec).size();
   if (numNonZeros > (UnsignedInt)(rowSize*colSize))
   {
      std::stringstream errmsg("");
      errmsg << "To many nonzero elements!; return an empty matrix";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if ((numNonZeros != (*rowIdxVec).size())
      || (numNonZeros != (*colIdxVec).size()))
   {
      std::stringstream errmsg("");
      errmsg << "input vectors do not have the same size!; ";
      errmsg << "return an empty matrix" << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // create iterator for std::vector
   IntegerArray::const_iterator stdConstIterator;

   /// find the maximum row idx
   stdConstIterator = max_element((*rowIdxVec).begin(), (*rowIdxVec).end());
   UnsignedInt maxRow = *stdConstIterator;

   /// find the maximum column idx
   stdConstIterator = max_element((*colIdxVec).begin(), (*colIdxVec).end());
   UnsignedInt maxCol = *stdConstIterator;

   if ((maxRow > (UnsignedInt)(rowSize - 1)) ||
       (maxCol > (UnsignedInt)(colSize - 1)))
   {
      std::stringstream errmsg("");
      errmsg << "a row or column index is out of range; ";
      errmsg << "return an empty matrix." << std::endl;
      throw LowThrustException(errmsg.str());
   }

   for (UnsignedInt idx = 0; idx < numNonZeros; ++idx)
   {
      spMat((*rowIdxVec)[idx], (*colIdxVec)[idx]) = (*valueVec)[idx];
   }
}


//------------------------------------------------------------------------------
// void     SetSparseMatrix(RSMatrix &spMat,
//                         Integer   rowSize, Integer colSize,
//                         IntegerArray *rowIdxVec, 
//                         IntegerArray *colIdxVec,
//                         rvector *valueVec)
//------------------------------------------------------------------------------
/**
 * Convert the three vector form into RSMatrix;
 *
 * @param <spMat>     the sparse matrix, which is the target.
 * @param <rowSize>   the number of rows of the sparse matrix.
 * @param <colSize>   the number of columns of the sparse matrix.
 * @param <rowIdxVec> STL vector where the row indices of nonzeros saved.
 * @param <colIdxVec> STL vector where the column indices of nonzeros saved.
 * @param <valueVec>  Rvector where the nonzeros values saved.
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseMatrix(RSMatrix &spMat,
                                       Integer  rowSize, Integer colSize,
                                       const IntegerArray *rowIdxVec,
                                       const IntegerArray *colIdxVec,
                                       const Rvector *valueVec)
{
   bool preserve = false;
   spMat.resize(rowSize, colSize, preserve);

   // if valueVec is empty, do nothing
   if ((*valueVec).IsSized() == false)
   {
      return;
   }


   UnsignedInt numNonZeros = (*valueVec).GetSize();
   if (numNonZeros > (UnsignedInt)(rowSize*colSize))
   {
      std::stringstream errmsg("");
      errmsg << "To many nonzero elements!; return an empty matrix";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if ((numNonZeros != (*rowIdxVec).size())
      || (numNonZeros != (*colIdxVec).size()))
   {
      std::stringstream errmsg("");
      errmsg << "input vectors do not have the same size!; ";
      errmsg << "return an empty matrix" << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // create iterator for std::vector
   IntegerArray::const_iterator stdConstIterator;

   /// find the maximum row idx
   stdConstIterator = max_element((*rowIdxVec).begin(), (*rowIdxVec).end());
   UnsignedInt maxRow = *stdConstIterator;

   /// find the maximum column idx
   stdConstIterator = max_element((*colIdxVec).begin(), (*colIdxVec).end());
   UnsignedInt maxCol = *stdConstIterator;

   if ((maxRow > (UnsignedInt)(rowSize - 1)) ||
       (maxCol > (UnsignedInt)(colSize - 1)))
   {
      std::stringstream errmsg("");
      errmsg << "a row or column index is out of range; ";
      errmsg << "return an empty matrix." << std::endl;
      throw LowThrustException(errmsg.str());
   }

   for (UnsignedInt idx = 0; idx < numNonZeros; ++idx)
   {
      spMat((*rowIdxVec)[idx], (*colIdxVec)[idx]) = (*valueVec)[idx];
   }
}

//------------------------------------------------------------------------------
// bool SparseMatrixUtil::SetSparseBLockMatrix(RSMatrix &spMat,
//                                           Integer rowOffSet, 
//                                           Integer colOffSet, 
//                                           IntegerArray &rowIdxVec, 
//                                           IntegerArray &colIdxVec,
//                                           std::vector<Real> &valueVec,
//                                           bool initialize)
//------------------------------------------------------------------------------
/**
 * Set a sparse block matrix or add it to the larger sparse matrix.
 *
 * @param <spMat>      the larger sparse matrix, which is the object of 
 *                     operation.
 * @param <rowOffSet>  the row offset of the sparse block.
 * @param <rowOffSet>  the column offset of the sparse block.
 * @param <rowIdxVec>  the row indices of non-zero elements in the sparse block.
 * @param <colIdxVec>  the col indices of non-zero elements in the sparse block.
 * @param <valueVec>   the values of non-zero elements in the sparse block.
 * @param <isNotAdding> indicating whether setting or addition will happen;
 *                     if true, the information of spMat will be ignored;
 *                     otherwise, spMat += sparseBlockMatrix will happen.
 *
 * @return If the operation is successful, return true;
 *                 otherwise, return false.
 * IMPORTANT! Note that although 'isNotAdding == true', it does not remove
 *            the existing nonzero elements.
 *            For instance, inserting [1 0; 0 2] to [1 0 1; 0 0 1] with
 *            rowOffSet = 0 and colOffSet = 1 will result in [1 1 1; 0 0 2].
 *            So it is observed that '1' at (1,3) does not affected but
 *            '1' at (2,3) is overrided with '2'. This is based on the
 *            assumption that the sparsity pattern does not change throught
 *            NLP solving process. Of course, if mesh points are refined,
 *            sparsity pattern will be changed but we are gonna isNotAdding
 *            the sparsity pattern once again.
 *            On the other hand, setting 'isNotAdding = false' will result in
 *            [1 1 1; 0 0 3] because '2' is added to the existing '1' at (2,3).
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseBLockMatrix(
                                       RSMatrix  &spMat,
                                       Integer   rowOffSet, Integer colOffSet,
                                       const IntegerArray *rowIdxVec,
                                       const IntegerArray *colIdxVec,
                                       const std::vector<Real> *valueVec,
                                       bool isNotAdding)
{
   Integer rowSize = spMat.size1();
   Integer colSize = spMat.size2();

   // if valueVec is empty, do nothing
   if ((*valueVec).empty() == true)
   {
      return;
   }

   UnsignedInt numNonZeros = (*valueVec).size();
   if (numNonZeros > (UnsignedInt)(rowSize*colSize))
   {
      std::stringstream errmsg("");
      errmsg << "To many nonzero elements!; do nothing" << std::endl;
      throw LowThrustException(errmsg.str());
   }
   // should this be && OR the product of the sizes?
   if ((numNonZeros != (*rowIdxVec).size())
      && (numNonZeros != (*colIdxVec).size()))
   {
      std::stringstream errmsg("");
      errmsg << "input vectors do not have the same size!; ";
      errmsg << "return an empty matrix" << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // create iterator for std::vector
   IntegerArray::const_iterator stdConstIterator;

   /// find the maximum row idx
   stdConstIterator = max_element((*rowIdxVec).begin(), (*rowIdxVec).end());
   UnsignedInt maxBlockRow = *stdConstIterator;

   /// find the maximum column idx
   stdConstIterator = max_element((*colIdxVec).begin(), (*colIdxVec).end());
   UnsignedInt maxBlockCol = *stdConstIterator;

   if ((maxBlockRow + rowOffSet > (UnsignedInt)(rowSize))
      || (maxBlockCol + colOffSet > (UnsignedInt)(colSize)))
   {
      std::stringstream errmsg("");
      errmsg << "a row or column index is out of range; do nothing.";
      throw LowThrustException(errmsg.str());
   }

   Integer rowIdx, colIdx;
   for (UnsignedInt idx = 0; idx < numNonZeros; ++idx)
   {
      rowIdx = rowOffSet + (*rowIdxVec)[idx];
      colIdx = colOffSet + (*colIdxVec)[idx];
      if (isNotAdding == true)
         spMat(rowIdx, colIdx) = (*valueVec)[idx];
      else
         spMat(rowIdx, colIdx) += (*valueVec)[idx];
   }
}


//------------------------------------------------------------------------------
// bool SparseMatrixUtil::SetSparseBLockMatrix(RSMatrix &spMat, 
//                               Integer rowOffSet,
//                               Integer colOffSet, 
//                               const RSMatrix  *spBlockMat,
//                               bool initialize)
//------------------------------------------------------------------------------
/**
 * Set a sparse block matrix or add it to the larger sparse matrix.
 *
 * @param <spMat> the larger sparse matrix, which is the object of operation.
 * @param <rowOffSet> the row offset of the sparse block.
 * @param <rowOffSet> the column offset of the sparse block.
 * @param <spBlockMat> the sparse block matrix to be inserted.
 * @param <isNotAdding> indicating whether setting or addition will happen;
 *                     if true, the information of spMat will be ignored;
 *                     otherwise, spMat += sparseBlockMatrix will happen.
 *
 * @return If the operation is successful, return true;
 *                 otherwise, return false.
 *
 * IMPORTANT! Note that although 'isNotAdding == true', it does not remove
 *            the existing nonzero elements.
 *            For instance, inserting [1 0; 0 2] to [1 0 1; 0 0 1] with
 *            rowOffSet = 0 and colOffSet = 1 will result in [1 1 1; 0 0 2].
 *            So it is observed that '1' at (1,3) does not affected but
 *            '1' at (2,3) is overrided with '2'. This is based on the
 *            assumption that the sparsity pattern does not change throught
 *            NLP solving process. Of course, if mesh points are refined,
 *            sparsity pattern will be changed but we are gonna isNotAdding
 *            the sparsity pattern once again.
 *            On the other hand, setting 'isNotAdding = false' will result in
 *            [1 1 1; 0 0 3] because '2' is added to the existing '1' at (2,3).
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseBLockMatrix(
                              RSMatrix  &spMat,    Integer         rowOffSet,
                              Integer   colOffSet, const RSMatrix  *spBlockMat,
                              bool      isNotAdding)
{
   UnsignedInt rowSize = spMat.size1();
   UnsignedInt colSize = spMat.size2();
   
   #ifdef DEBUG_SPARSE_MATRIX_UTIL
      MessageInterface::ShowMessage("rowSize   = %d\n", rowSize);
      MessageInterface::ShowMessage("colSize   = %d\n", colSize);
      MessageInterface::ShowMessage("rowOffSet = %d\n", rowOffSet);
      MessageInterface::ShowMessage("colOffSet = %d\n", colOffSet);
      MessageInterface::ShowMessage("size of spBlockMat (row)  = %d\n",
                                    (Integer) (*spBlockMat).size1());
      MessageInterface::ShowMessage("size of spBlockMat (col)  = %d\n",
                                    (Integer) (*spBlockMat).size2());
   #endif

   if ((rowSize < (*spBlockMat).size1() + rowOffSet)
      || (colSize < (*spBlockMat).size2() + colOffSet))
   {
      std::stringstream errmsg("");
      errmsg << "Error: dimension mismatch!; do nothing";
      throw LowThrustException(errmsg.str());
   }

   UnsignedInt rowIdx, colIdx;
   for (RSMatrix::const_iterator1 i1 = (*spBlockMat).begin1();
        i1 != (*spBlockMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         rowIdx = rowOffSet + i2.index1();
         colIdx = colOffSet + i2.index2();
         if (isNotAdding == true)
            spMat(rowIdx, colIdx) = *i2;
         else
            spMat(rowIdx, colIdx) += *i2;
      }
   }
}

//------------------------------------------------------------------------------
// bool SparseMatrixUtil::SetSparseBLockMatrix(RSMatrix &spMat, 
//                                           Integer rowOffSet, 
//                                           Integer colOffSet, 
//                                           IntegerArray &rowIdxVec, 
//                                           IntegerArray &colIdxVec,
//                                           Rvector &valueVec,
//                                           bool isNotAdding)
//------------------------------------------------------------------------------
/**
 * Set a sparse block matrix or add it to the larger sparse matrix.
 *
 * @param <spMat>      the larger sparse matrix, which is the object of 
 *                     operation.
 * @param <rowOffSet>  the row offset of the sparse block.
 * @param <rowOffSet>  the column offset of the sparse block.
 * @param <rowIdxVec>  the row indices of non-zero elements in the sparse block.
 * @param <colIdxVec>  the col indices of non-zero elements in the sparse block.
 * @param <valueVec>   the values of non-zero elements in the sparse block.
 * @param <isNotAdding> indicating whether setting or addition will happen;
 *                     if true, the information of spMat will be ignored;
 *                     otherwise, spMat += sparseBlockMatrix will happen.
 *
 * @return If the operation is successful, return true;
 *                 otherwise, return false.
 * IMPORTANT! Note that although 'isNotAdding == true', it does not remove
 *            the existing nonzero elements.
 *            For instance, inserting [1 0; 0 2] to [1 0 1; 0 0 1] with
 *            rowOffSet = 0 and colOffSet = 1 will result in [1 1 1; 0 0 2].
 *            So it is observed that '1' at (1,3) does not affected but
 *            '1' at (2,3) is overrided with '2'. This is based on the
 *            assumption that the sparsity pattern does not change throught
 *            NLP solving process. Of course, if mesh points are refined,
 *            sparsity pattern will be changed but we are gonna isNotAdding
 *            the sparsity pattern once again.
 *            On the other hand, setting 'isNotAdding = false' will result in
 *            [1 1 1; 0 0 3] because '2' is added to the existing '1' at (2,3).
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseBLockMatrix(
                                       RSMatrix  &spMat,
                                       Integer   rowOffSet, Integer colOffSet,
                                       const IntegerArray *rowIdxVec,
                                       const IntegerArray *colIdxVec,
                                       const Rvector *valueVec,
                                       bool isNotAdding)
{
   UnsignedInt rowSize = spMat.size1();
   UnsignedInt colSize = spMat.size2();

   // if valueVec is empty, do nothing
   if ((*valueVec).GetSize() == 0)
   {
      return;
   }

   UnsignedInt numNonZeros = (*valueVec).GetSize();
   if (numNonZeros > rowSize*colSize)
   {
      std::stringstream errmsg("");
      errmsg << "To many nonzero elements!; return an empty matrix";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if ((numNonZeros != (*rowIdxVec).size())
      || (numNonZeros != (*colIdxVec).size()))
   {
      std::stringstream errmsg("");
      errmsg << "vector dimension mismatch!; return an empty matrix";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // create iterator for std::vector
   IntegerArray::const_iterator stdConstIterator;

   /// find the maximum row idx
   stdConstIterator = max_element((*rowIdxVec).begin(), (*rowIdxVec).end());
   UnsignedInt maxBlockRow = *stdConstIterator;

   /// find the maximum column idx
   stdConstIterator = max_element((*colIdxVec).begin(), (*colIdxVec).end());
   UnsignedInt maxBlockCol = *stdConstIterator;

   if ((maxBlockRow + rowOffSet > rowSize)
      || (maxBlockCol + colOffSet > colSize))
   {
      std::stringstream errmsg("");
      errmsg << "a row or column index is out of range; ";
      errmsg << "return an empty matrix." << std::endl;
      throw LowThrustException(errmsg.str());
   }

   Integer rowIdx, colIdx;
   for (UnsignedInt idx = 0; idx < numNonZeros; ++idx)
   {
      rowIdx = rowOffSet + (*rowIdxVec)[idx];
      colIdx = colOffSet + (*colIdxVec)[idx];
      if (isNotAdding == true)
         spMat(rowIdx, colIdx) = (*valueVec)(idx);
      else
         spMat(rowIdx, colIdx) += (*valueVec)(idx);
   }
}



//------------------------------------------------------------------------------
// bool SparseMatrixUtil::SetSparseBLockMatrix(RSMatrix &spMat, 
//                                           Integer rowOffSet, 
//                                           Integer colOffSet, 
//                                           const Rmatrix *spBlockMat,
//                                           bool isNotAdding)
//------------------------------------------------------------------------------
/**
 * Set a sparse block matrix or add it to the larger sparse matrix.
 *
 * @param <spMat>      the larger sparse matrix, which is the object of 
 *                     operation.
 * @param <rowOffSet>  the row offset of the sparse block.
 * @param <rowOffSet>  the column offset of the sparse block.
 * @param <spBlockMat> the sparse block matrix to be inserted.
 * @param <isNotAdding> indicating whether setting or addition will happen;
 *                     if true, the information of spMat will be ignored;
 *                     otherwise, spMat += sparseBlockMatrix will happen.
 *
 * @return If the operation is successful, return true;
 *                 otherwise, return false.
 *
 * IMPORTANT! Note that although 'isNotAdding == true', it does not remove
 *            the existing nonzero elements.
 *            For instance, inserting [1 0; 0 2] to [1 0 1; 0 0 1] with
 *            rowOffSet = 0 and colOffSet = 1 will result in [1 1 1; 0 0 2].
 *            So it is observed that '1' at (1,3) does not affected but
 *            '1' at (2,3) is overrided with '2'. This is based on the
 *            assumption that the sparsity pattern does not change throught
 *            NLP solving process. Of course, if mesh points are refined,
 *            sparsity pattern will be changed but we are gonna isNotAdding
 *            the sparsity pattern once again.
 *            On the other hand, setting 'isNotAdding = false' will result in
 *            [1 1 1; 0 0 3] because '2' is added to the existing '1' at (2,3).
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::SetSparseBLockMatrix(
                                       RSMatrix &spMat,
                                       Integer  rowOffSet, Integer colOffSet,
                                       const Rmatrix *blockMat,
                                       bool isNotAdding)
{
   #ifdef DEBUG_SPARSE_MATRIX_SET
      MessageInterface::ShowMessage(
         "Entering SetSparseBLockMatrix with rowOffSet = %d, colOffSet = %d\n",
         rowOffSet, colOffSet);
      Integer r, c;
      blockMat->GetSize(r, c);
      MessageInterface::ShowMessage("      blockMat size = (%d, %d)\n", r, c);
   #endif
   UnsignedInt rowSize = spMat.size1();
   UnsignedInt colSize = spMat.size2();

   UnsignedInt numRowsBlock = blockMat->GetNumRows();
   UnsignedInt numColumnsBlock = blockMat->GetNumColumns();
   #ifdef DEBUG_SPARSE_MATRIX_SET
      MessageInterface::ShowMessage(
                           "   --- rowSize      = %d, colSize         = %d\n",
                           rowSize, colSize);
      MessageInterface::ShowMessage(
                           "   --- numRowsBlock = %d, numColumnsBlock = %d\n",
                           numRowsBlock, numColumnsBlock);
   #endif
   if ((rowSize < numRowsBlock + rowOffSet)
      || (colSize < numColumnsBlock + colOffSet))
   {
      std::stringstream errmsg("");
      errmsg << "dimension mismatch!; do nothing.";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }

   UnsignedInt rowIdx, colIdx;
   for (UnsignedInt idx1 = 0; idx1 < numRowsBlock; ++idx1)
   {
      for (UnsignedInt idx2 = 0; idx2 < numColumnsBlock; ++idx2)
      {
         if (blockMat->GetElement(idx1, idx2) != 0.0)
         {
            rowIdx = rowOffSet + idx1;
            colIdx = colOffSet + idx2;
            if (isNotAdding == true)
               spMat(rowIdx, colIdx) = blockMat->GetElement(idx1, idx2);
            else
               spMat(rowIdx, colIdx) += blockMat->GetElement(idx1, idx2);
         }

      }
   }
   #ifdef DEBUG_SPARSE_MATRIX_SET
      MessageInterface::ShowMessage("LEAVING SetSparseBLockMatrix\n");
   #endif
}

//------------------------------------------------------------------------------
// Real GetElement(const RSMatrix  *spMat,
//                 Integer rowIdx, Integer colIdx)
//------------------------------------------------------------------------------
/**
 * Returns the specified element of the input matrix
 *
 * @param <spMat>    the sparse matrix
 * @param <rowIdx>   the row index
 * @param <colIdx>   the column index
 *
 * @return the requested element
 */
//------------------------------------------------------------------------------
Real SparseMatrixUtil::GetElement(const RSMatrix  *spMat,
                                  Integer rowIdx, Integer colIdx)
{
   return (*spMat)(rowIdx, colIdx);
};

//------------------------------------------------------------------------------
// Integer GetNumRows(const RSMatrix  *spMat)
//------------------------------------------------------------------------------
/**
 * Returns the number of rows of the input matrix
 *
 * @param <spMat>    the sparse matrix
 *
 * @return the number of rows
 */
//------------------------------------------------------------------------------
Integer SparseMatrixUtil::GetNumRows(const RSMatrix  *spMat)
{
   return spMat->size1();
};

//------------------------------------------------------------------------------
// Integer GetNumColumns(const RSMatrix  *spMat)
//------------------------------------------------------------------------------
/**
 * Returns the number of columns of the input matrix
 *
 * @param <spMat>    the sparse matrix
 *
 * @return the number of columns
 */
//------------------------------------------------------------------------------
Integer SparseMatrixUtil::GetNumColumns(const RSMatrix  *spMat)
{
   return spMat->size2();
};


//------------------------------------------------------------------------------
// void RSMatrix GetSparsityPattern(RSMatrix  spMat,
//                         bool      hasZeros)
//------------------------------------------------------------------------------
/**
 * Get sparsity pattern matrix from sparse matrix.
 * A sparisty pattern matrix has the same sparsity pattern as given spMat
 * but all the elements are replaced with the unity.
 *
 * @param <spMat>           the object of sparsity pattern investigation.
 * @param <hasZeros>        a boolean indicating whether fill with zeros/ones.
 *
 * return <sparsityPattern> the sparsity pattern matrix; all the elements are
 *                          either zero or one.
 */
//------------------------------------------------------------------------------
RSMatrix SparseMatrixUtil::GetSparsityPattern(const RSMatrix *spMat,
                                              bool hasZeros)
{
   RSMatrix sparsityPattern = (*spMat);

   for (RSMatrix::iterator1 i1 = sparsityPattern.begin1();
        i1 != sparsityPattern.end1(); ++i1) 
   {
      for (RSMatrix::iterator2 i2 = i1.begin(); i2 != i1.end();
           ++i2)
     {
        if (hasZeros == true)
            *i2 = 0.0;
        else
            *i2 = 1.0;
     }    
   }
   return sparsityPattern;
}

//------------------------------------------------------------------------------
// bool GetSparsityPattern(RSMatrix     spMat,
//                         IntegerArray &rowIdxVec, 
//                         IntegerArray &colIdxVec)
//------------------------------------------------------------------------------
/**
 * Get sparsity pattern matrix from sparse matrix.
 * Return row and column indices of nonzeros.
 *
 * @param <spMat> the object of sparsity pattern investigation.
 * @param <rowIdxVec> the row indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <colIdxVec> the column indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 *
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::GetSparsityPattern(
                                       const RSMatrix *spMat,
                                       IntegerArray &rowIdxVec, 
                                       IntegerArray &colIdxVec)
{
   // now spMat is a pointer to sparse matrix;
   Integer numNonZeros = GetNumNonZeroElements(spMat);

   // initialization and preallocation
   rowIdxVec.clear();
   rowIdxVec.resize(numNonZeros);
   colIdxVec.clear();
   colIdxVec.resize(numNonZeros);
   
   Integer idx = 0;
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         rowIdxVec[idx] = i2.index1();
         colIdxVec[idx] = i2.index2();
         ++idx;
      }    
   }
}


//------------------------------------------------------------------------------
// void SparseMatrixUtil::GetThreeVectorForm(RSMatrix spMat,
//                                          IntegerArray &rowIdxVec, 
//                                          IntegerArray &colIdxVec,
//                                          std::vector<Real> &valueVec)
//------------------------------------------------------------------------------
/**
 * Convert sparsity matrix into the three vector form; Return boolean.
 *
 * @param <spMat> the sparse matrix, which is the object of conversion.
 * @param <rowIdxVec> the row indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <colIdxVec> the column indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <valueVec>  the nonzero values will be saved here;
 *                    note that the previous data will be lost.
 *
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::GetThreeVectorForm(
                                    const RSMatrix  *spMat,
                                    IntegerArray &rowIdxVec,
                                    IntegerArray &colIdxVec,
                                    std::vector<Real> &valueVec)
{
   // now spMat is a pointer to sparse matrix;
   Integer numNonZeros = GetNumNonZeroElements(spMat);

   // initialization and preallocation
   rowIdxVec.clear();
   rowIdxVec.resize(numNonZeros);
   colIdxVec.clear();
   colIdxVec.resize(numNonZeros);
   valueVec.clear();
   valueVec.resize(numNonZeros);

   Integer idx = 0;
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         rowIdxVec[idx] = (i2.index1());
         colIdxVec[idx] = (i2.index2());
         valueVec[idx] = (*i2);
         ++idx;
      }
   }
}

//------------------------------------------------------------------------------
// void SparseMatrixUtil::GetThreeVectorForm(RSMatrix spMat,
//                                          Integer rowIdxLB, Integer rowIdxUB,
//                                          Integer colIdxLB, Integer colIdxUB,
//                                          IntegerArray &rowIdxVec, 
//                                          IntegerArray &colIdxVec,
//                                          std::vector<Real> &valueVec)
//------------------------------------------------------------------------------
/**
 * Convert a sub-block of the sparsity matrix into the three vector form;
 * Return boolian.
 *
 * @param <spMat>     the sparse matrix, which is the object of conversion.
 * @param <rowIdxLB>  defines the lower bound of row idx of the sub-block.
 * @param <rowIdxUB>  defines the upper bound of row idx of the sub-block.
 * @param <colIdxLB>  defines the lower bound of column idx of the sub-block.
 * @param <rowIdxLB>  defines the upper bound of column idx of the sub-block.
 * @param <rowIdxVec> the row indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <colIdxVec> the column indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <valueVec>  the nonzero values will be saved here;
 *                    note that the previous data will be lost.
 *
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::GetThreeVectorForm(
                                    const RSMatrix  *spMat,
                                    Integer rowIdxLB, Integer rowIdxUB,
                                    Integer colIdxLB, Integer colIdxUB,
                                    IntegerArray &rowIdxVec,
                                    IntegerArray &colIdxVec,
                                    std::vector<Real> &valueVec)
{

   Integer rowIdxMax = (*spMat).size1() - 1;
   Integer colIdxMax = (*spMat).size2() - 1;

   if (((rowIdxUB - rowIdxLB) < 0) || (rowIdxUB > rowIdxMax))
   {
      std::stringstream errmsg("");
      errmsg << "invalid row index boundary values";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if (((colIdxUB - colIdxLB) < 0) || (colIdxUB > colIdxMax))
   {
      std::stringstream errmsg("");
      errmsg << "invalid column index boundary values";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // initialization and preallocation
   rowIdxVec.clear();

   colIdxVec.clear();

   valueVec.clear();

   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         if (((UnsignedInt)rowIdxLB <= i2.index1())
            && (i2.index1() <= (UnsignedInt)rowIdxUB)
            && ((UnsignedInt)colIdxLB <= i2.index2())
            && (i2.index2() <= (UnsignedInt)colIdxUB))
         {
            rowIdxVec.push_back(i2.index1());
            colIdxVec.push_back(i2.index2());
            valueVec.push_back(*i2);
         }
      }
   }
}


//------------------------------------------------------------------------------
// bool SparseMatrixUtil::GetThreeVectorForm(RSMatrix spMat, 
//                                          IntegerArray &rowIdxVec, 
//                                          IntegerArray &colIdxVec,
//                                          Rvector &valueVec)
//------------------------------------------------------------------------------
/**
 * Convert sparsity matrix into the three vector form; Return boolian.
 *
 * @param <spMat>     the sparse matrix, which is the object of conversion.
 * @param <rowIdxVec> the row indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <colIdxVec> the column indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <valueVec>  the nonzero values will be saved here;
 *                    note that the previous data will be lost.
 *
 * @return if the operation is successful, return true;
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::GetThreeVectorForm(
                                    const RSMatrix  *spMat,
                                    IntegerArray &rowIdxVec,
                                    IntegerArray &colIdxVec,
                                    Rvector &valueVec)
{
   // now spMat is a pointer to sparse matrix;
   Integer numNonZeros = GetNumNonZeroElements(spMat);

   // initialization and preallocation
   rowIdxVec.clear();
   rowIdxVec.resize(numNonZeros);
   colIdxVec.clear();
   colIdxVec.resize(numNonZeros);

   valueVec.SetSize(numNonZeros);

   Integer idx = 0;
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         rowIdxVec[idx] = (i2.index1());
         colIdxVec[idx] = (i2.index2());
         valueVec(idx) = (*i2);
         ++idx;
      }
   }
}

//------------------------------------------------------------------------------
// bool SparseMatrixUtil::GetThreeVectorForm(RSMatrix spMat,
//                                          Integer rowIdxLB, Integer rowIdxUB,
//                                          Integer colIdxLB, Integer colIdxUB,
//                                          IntegerArray &rowIdxVec, 
//                                          IntegerArray &colIdxVec,
//                                          Rvector &valueVec)
//------------------------------------------------------------------------------
/**
 * Convert a sub-block of the sparsity matrix into the three vector form;
 * Return boolian.
 *
 * @param <spMat>     the sparse matrix, which is the object of conversion.
 * @param <rowIdxLB>  defines the lower bound of row idx of the sub-block.
 * @param <rowIdxUB>  defines the upper bound of row idx of the sub-block.
 * @param <colIdxLB>  defines the lower bound of column idx of the sub-block.
 * @param <colIdxUB>  defines the upper bound of column idx of the sub-block.
 * @param <rowIdxVec> the row indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <colIdxVec> the column indices of nonzeros will be saved here;
 *                    note that the previous data will be lost.
 * @param <valueVec>  the nonzero values will be saved here;
 *                    note that the previous data will be lost.
 *
 * @return if the operation is successful, return true;
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::GetThreeVectorForm(
                                    const RSMatrix *spMat,
                                    Integer rowIdxLB, Integer rowIdxUB,
                                    Integer colIdxLB, Integer colIdxUB,
                                    IntegerArray &rowIdxVec,
                                    IntegerArray &colIdxVec,
                                    Rvector &valueVec)
{

   Integer rowIdxMax = (*spMat).size1() - 1;
   Integer colIdxMax = (*spMat).size2() - 1;


   if (((rowIdxUB - rowIdxLB) < 1) || (rowIdxUB > rowIdxMax))
   {
      std::stringstream errmsg("");
      errmsg << "invalid row index boundary values";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if (((colIdxUB - colIdxLB) < 1) || (colIdxUB > colIdxMax))
   {
      std::stringstream errmsg("");
      errmsg << "invalid column index boundary values";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }

   // initialization and preallocation
   rowIdxVec.clear();
   colIdxVec.clear();
   std::vector<Real> stdValueVec;

   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         if (((UnsignedInt)rowIdxLB <= i2.index1())
            && (i2.index1() <= (UnsignedInt)rowIdxUB)
            && ((UnsignedInt)colIdxLB <= i2.index2())
            && (i2.index2() <= (UnsignedInt)colIdxUB))
         {
            rowIdxVec.push_back(i2.index1());
            colIdxVec.push_back(i2.index2());
            stdValueVec.push_back(*i2);
         }
      }
   }
   valueVec.SetSize(stdValueVec.size());

   UnsignedInt idx = 0;
   for (std::vector<Real>::iterator it = stdValueVec.begin();
      it != stdValueVec.end(); ++it)
   {
      valueVec(idx) = *it;
      ++idx;
   }
}

//------------------------------------------------------------------------------
// Integer SparseMatrixUtil::GetNumNonZeroElements(RSMatrix spMat)
//------------------------------------------------------------------------------
/**
 * Get the number of nonzero elements.
 *
 * @param <spMat> the object of the investigation on
 *                the number of nonzero elements.
 *
 * @return <numNonZeroElements>  the number of nonzero elements.
 */
//------------------------------------------------------------------------------
Integer SparseMatrixUtil::GetNumNonZeroElements(
                                      const RSMatrix *spMat)
{
   Integer numNonZeroElements = 0;

   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         ++numNonZeroElements;
      }    
   }
   return numNonZeroElements;
}

//------------------------------------------------------------------------------
// Real GetAbsTotalSum(RSMatrix spMat)
//------------------------------------------------------------------------------
/**
 * Get the total sum of absolute values of the elements of the input
 * sparse matrix
 *
 * @param <spMat> the object of the investigation on
 *                the number of nonzero elements.
 *
 * @return   the total sum of absolute values
 */
//------------------------------------------------------------------------------
Real SparseMatrixUtil::GetAbsTotalSum(const RSMatrix *spMat)
{
   Real error = 0;

   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1) 
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         error += GmatMathUtil::Abs(*i2);
      }    
   }
   return error;
}

//------------------------------------------------------------------------------
// void fast_prod(RSMatrix spMat, std::vector<Real> vec,
//                std::vector<Real> result, bool initialize = true)
//------------------------------------------------------------------------------
/**
 * Calculate product between sparse matrix and dense Real vector.
 * Similar to BOOST's fast_prod. This method is just a vector version of it.
 *
 * @param <spMat>   the sparse matrix
 * @param <vec>     the target of multiplication. 
 * @param <result>  output - result = spMat*vec  (if initialize = true),
 *                           result += spMat*vec (if initialize = false).
 * @param <initialize>  Flag determine opertation to be 
 *                      setting (=) or addition (+=)
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::fast_prod(const RSMatrix *spMat,
                           const std::vector<Real> *vec, 
                           std::vector<Real> &result, bool initialize)
{
   if (initialize == true)
   {
      // empty result vector
      result.clear();
      // preallocate and initialize as 0
      result.resize((*spMat).size1());
   }
   else
   {
      // check for the size of result vector
      if (result.size() != (*spMat).size1())
      {
         std::stringstream errmsg("");
         errmsg << "result vector size is" << result.size() ;
         errmsg << "the number of rows of matrix is" << (*spMat).size1();
         errmsg << "Error: the length of the result vector is not correct!";
         errmsg << std::endl;
         throw LowThrustException(errmsg.str());
      }
   }
   if ((*spMat).size2() != (*vec).size())
   {

     std::stringstream errmsg("");
      errmsg << "vector size is" << (*vec).size();
      errmsg << "the number of columns of matrix is" << (*spMat).size2();
      errmsg << "Error: dimension mismatch between matrix and vector.";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());

   }
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         result[i1.index1()] += (*i2)*(*vec)[i2.index2()];
      }    
   }
}


//------------------------------------------------------------------------------
// void fast_prod(RSMatrix spMat, std::vector<Real> vec,
//                std::vector<Real> result, bool initialize = true)
//------------------------------------------------------------------------------
/**
 * Calculate product between sparse matrix and dense Real vector.
 * Similar to BOOST's fast_prod. This method is just a vector version of it.
 *
 * @param <spMat>   the sparse matrix
 * @param <vec>     the target of multiplication.
 * @param <result>  output - result = spMat*vec  (if initialize = true),
 *                           result += spMat*vec (if initialize = false).
 * @param <initialize>  Flag determine opertation to be
 *                      setting (=) or addition (+=)
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::fast_prod(const RSMatrix *spMat,
                                 const Rvector *vec,
                                 Rvector &result, bool initialize)
{
   if (initialize == true)
   {
      // preallocate and initialize as 0
      result.SetSize((*spMat).size1());
      for (int idx = 0; idx < result.GetSize(); ++idx)
         result(idx) = 0;
   }
   else
   {
      // check for the size of result vector
      if ((UnsignedInt)result.GetSize() != (*spMat).size1())
      {

         std::stringstream errmsg("");
         errmsg << "result vector size is" << result.GetSize();
         errmsg << "the number of rows of matrix is" << (*spMat).size1();
         errmsg << "Error: the length of the result vector is not correct!";
         errmsg << std::endl;
         throw LowThrustException(errmsg.str());
      }
   }
   if ((*spMat).size2() != (UnsignedInt)(*vec).GetSize())
   {
      std::stringstream errmsg("");
      errmsg << "vector size is" << (*vec).GetSize();
      errmsg << "the number of columns of matrix is" << (*spMat).size2();
      errmsg << "Error: dimension mismatch between matrix and vector.";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {

      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         result(i1.index1()) += (*i2)*(*vec)(i2.index2());
      }
   }
}

//------------------------------------------------------------------------------
// void fast_prod(RSMatrix *spMat1,
//                RSMatrix *spMat2,
//                RSMatrix &resultMat,
//                bool     initialize = true)
//------------------------------------------------------------------------------
/**
 * Calculate product between sparse matrices. Using BOOST's fast_prod.
 *
 * @param <spMat1>      the first sparse matrix
 * @param <spMat2>      the second sparse matrix 
 * @param <resultMat>   output:resultMat = spMat1*spMat2 (if initialize = true)
 *                             resultMat += spMat*spMat2 (if initialize = false)
 * @param <initialize>  Flag determine opertation to be
 *                      setting (=) or addition (+=)
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::fast_prod(const RSMatrix *spMat1,
                                 const RSMatrix *spMat2,
                                 RSMatrix &resultMat,
                                 bool initialize)
{
   if ((*spMat1).size2() != (*spMat2).size1())
   {
      std::stringstream errmsg("");
      errmsg << "Error: dimension mismatch between matrices.";
      errmsg << std::endl;
      throw LowThrustException(errmsg.str());
   }
   if (initialize != true)
   {
      if ((resultMat.size1() != (*spMat1).size1())
         || (resultMat.size2() != (*spMat2).size2()))
      {
         std::stringstream errmsg("");
         errmsg << "Error: the dimensions of the result matrix ";
         errmsg << "are not correct!" << std::endl;
         throw LowThrustException(errmsg.str());
      }
   }
   axpy_prod((*spMat1),(*spMat2),resultMat,initialize);      
}

//------------------------------------------------------------------------------
//  void PrintNonZeroElements(RSMatrix spMat,
//                            bool rowOrder = true)
//------------------------------------------------------------------------------
/**
 * Print all the nonzero elements to the screen.
 *
 * @param <spMat>    a sparse matrix, which is the object of operation.
 * @param <rowOrder> print in row order?
 *
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::PrintNonZeroElements(
                       const RSMatrix *spMat, bool rowOrder)
{   
   std::stringstream msg("");

   if (rowOrder)
   {
      for (RSMatrix::const_iterator1 i1 = spMat->begin1();
           i1 != spMat->end1(); ++i1)
      {
         for (RSMatrix::const_iterator2 i2 = i1.begin();
              i2 != i1.end(); ++i2)
         {
            msg << "(" << i2.index1() << "," << i2.index2()
               << ") = " << *i2 << std::endl;
         }
      }
   }
   else
   {
      // TBD
   }

   // the below line is temporarily muted
   MessageInterface::ShowMessage(msg.str());
}

//------------------------------------------------------------------------------
// RSMatrix ReplicateSparseMatrix(RSMatrix spBlockMat,
//                                Integer  numRowRepetition,
//                                Integer  numColRepetition)
//------------------------------------------------------------------------------
/**
 * Create a sparse matrix based on a sparse block matrix and repetition.
 *
 * @param <spBlockMat> a sparse block matrix, which is the object of operation.
 * @param <numRowRepetition> the number of repetition of the sparse block in
 *                           row direction.
 * @param <numColRepetition> the number of repetition of the sparse block in
 *                           column direction.
 *
 * @return The sparse matrix filled the repetition of the sparse block matrix.
 *
 * IMPORTANT! Because compressed_matrix is row major data storage, a row-wise
 *            operation is performed here. Otherwise, compressed_matrix needs
 *            to be rearranged everytime and it makes algorithm extremely slow.
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::ReplicateSparseMatrix(
                                 const RSMatrix *spBlockMat,
                                 Integer        numRowRepetition,
                                 Integer        numColRepetition,
                                 RSMatrix       &spMat)
{
   Integer rowBlockSize = (*spBlockMat).size1();
   Integer colBlockSize = (*spBlockMat).size2();
   Integer rowSize = rowBlockSize*numRowRepetition;
   Integer colSize = colBlockSize*numColRepetition;
   Integer rowIdx, colIdx;

   bool preserve = false;
   spMat.resize(rowSize,colSize, preserve);

   for (Integer rowBlockIdx = 0; rowBlockIdx < numRowRepetition; rowBlockIdx++)
   {
      for (RSMatrix::const_iterator1 it1 =
           (*spBlockMat).begin1(); it1 != (*spBlockMat).end1(); it1++)
      {
         rowIdx = it1.index1() + rowBlockIdx*rowBlockSize;
         // filling the gap; insertion must be made by each row; 
         for (Integer colBlockIdx = 0; colBlockIdx < numColRepetition;
              colBlockIdx++)
         {
            for (RSMatrix::const_iterator2 it2 = it1.begin();
                 it2 != it1.end(); it2++)
            {
               colIdx = it2.index2() + colBlockIdx*colBlockSize;
               spMat(rowIdx, colIdx) = *it2;
            }
         }
      }
   }
   return;
}


//------------------------------------------------------------------------------
// bool IsZeroMatrix(const RSMatrix *spMat)
//------------------------------------------------------------------------------
/**
 * Check sparse matrix is zero matrix.
 *
 * @param <spMat> the object of the investigation on
 *                the zero-matrix-ness.
 *
 * @return if spMat is zero matrix, return true; otherwise return false.
 */
//------------------------------------------------------------------------------
bool SparseMatrixUtil::IsZeroMatrix(const RSMatrix *spMat)
{
   Integer numNonZeroElements = 0;
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         ++numNonZeroElements;
         if (numNonZeroElements == 1)
            return false;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// Rmatrix  RSMatrixToRmatrix(const RSMatrix  *spMat,
//                            Integer rowLB, Integer rowUB,
//                            Integer colLB, Integer colUB)
//------------------------------------------------------------------------------
/**
 * Check sparse matrix is zero matrix.
 *
 * @param <spMat>  the object needs to be copied.
 * @param <rowLB>  defines the lower bound of row idx of the sub-block.
 * @param <rowUB>  defines the upper bound of row idx of the sub-block.
 * @param <colLB>  defines the lower bound of column idx of the sub-block.
 * @param <colUB>  defines the upper bound of column idx of the sub-block.
 *
 * @return Rmatrix having the data of spMat.
 */
//------------------------------------------------------------------------------
Rmatrix  SparseMatrixUtil::RSMatrixToRmatrix(
                              const RSMatrix  *spMat,
                              Integer rowLB, Integer rowUB, 
                              Integer colLB, Integer colUB)
{
   if ((rowLB > rowUB)
       || (colLB > colUB))
   {
      std::string errmsg = "For SparseMatrixUtil::RSMatrixToRmatrix, ";
      errmsg += "invalid row or column ranges.\n";
      throw LowThrustException(errmsg);
   }
   Rmatrix tmpMat;
   tmpMat.SetSize(rowUB - rowLB + 1, colUB - colLB + 1);

   for (int rowIdx = rowLB; rowIdx < rowUB + 1; ++rowIdx)
   {
      for (int colIdx = colLB; colIdx < colUB + 1; ++colIdx)
      {
         tmpMat(rowIdx - rowLB, colIdx - colLB) 
            = (*spMat)(rowIdx,colIdx);
      }
   }
   return tmpMat;
}

//------------------------------------------------------------------------------
// Rmatrix  RSMatrixToRmatrix(const RSMatrix  *spMat)
//------------------------------------------------------------------------------
/**
* Copy RSMatrix to Rmatrix.
*
* @param <spMat>  the matrix needs to be copied.
*
* @return Rmatrix having the same data as spMat.
*/
//------------------------------------------------------------------------------
Rmatrix  SparseMatrixUtil::RSMatrixToRmatrix(const RSMatrix  *spMat)
{

   Rmatrix tmpMat;
   tmpMat.SetSize(spMat->size1(), spMat->size2());

   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         tmpMat(i1.index1(), i2.index2()) = *i2;
      }
   }

   return tmpMat;
}

//------------------------------------------------------------------------------
// RSMatrix CopySparseMatrix(const RSMatrix *spMat)
//------------------------------------------------------------------------------
/**
 * copy the input data into the output sparse matrix
 *
 * @param <spMat> the object of copy operation.
 *
 * @return the copy RSMatrix .
 */
//------------------------------------------------------------------------------
RSMatrix SparseMatrixUtil::CopySparseMatrix(const RSMatrix *spMat)
{
   RSMatrix newSparseMatrix;
   SetSize(newSparseMatrix, spMat->size1(), spMat->size2());
   for (RSMatrix::const_iterator1 i1 = (*spMat).begin1();
        i1 != (*spMat).end1(); ++i1)
   {
      for (RSMatrix::const_iterator2 i2 = i1.begin();
           i2 != i1.end(); ++i2)
      {
         newSparseMatrix(i1.index1(), i2.index2()) = *i2;
      }
   }

   return newSparseMatrix;
}

//------------------------------------------------------------------------------
// void CopySparseMatrix(const RSMatrix &copyFrom, RSMatrix &copyTo)
//------------------------------------------------------------------------------
/**
 * copy the input data into the output sparse matrix
 *
 * @param <spMat> the object of copy operation.
 *
 * @return the copy RSMatrix .
 */
//------------------------------------------------------------------------------
void SparseMatrixUtil::CopySparseMatrix(const RSMatrix &copyFrom,
                                        RSMatrix       &copyTo)
{
   SetSize(copyTo, copyFrom.size1(), copyFrom.size2());
   
   Integer sz1 = copyFrom.size1();
   Integer sz2 = copyFrom.size2();
   
   for (Integer ii = 0; ii < sz1; ii++)
   {
      for (Integer jj = 0; jj < sz2; jj++)
      {
         copyTo(ii,jj) = copyFrom(ii,jj);
      }
   }

}
