//------------------------------------------------------------------------------
//                         SparseMatrixUtil1
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
// Created: 2015/07/08
//
// NOTE: This is a static utility class.  The constructors, destructor, and
// operator= are not implemented.  No instances of this class can be
// instantiated.
/**
 */
//------------------------------------------------------------------------------
#ifndef SparseMatrixUtil_hpp
#define SparseMatrixUtil_hpp

/// for std::vector's min_element, max_element
#include <algorithm> 
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

// ublas related preprocessor starts
/// we are not using any BOOST libraries
#define BOOST_ALL_NO_LIB 
/// turning off unnecessary checks in debug mode.
#define BOOST_UBLAS_NDEBUG
/// include ublas related headers
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
// ublas related inclusion ends


using boost::numeric::ublas::compressed_matrix;

// define RSMatrix; they can be replaced with other
// data types.
typedef compressed_matrix<Real> RSMatrix;

// it is recommended not to use Iterators in the 
// outside of sparse matrix utility!
// because using Iterators are BOOST dependent!

//typedef compressed_matrix<Real>::iterator1 Iterator1;
//typedef compressed_matrix<Real>::iterator2 Iterator2;
//typedef compressed_matrix<Real>::const_iterator1 ConstIterator1;
//typedef compressed_matrix<Real>::const_iterator2 ConstIterator2;

class CSALT_API SparseMatrixUtil
{ 
public:

   // set methods

   /// set the element of sparse matrix
   static void SetElement(RSMatrix  &spMat,
                          Integer rowIdx, Integer colIdx, Real value);

   /// set the size/resize of the sparse matrix
   static void SetSize(RSMatrix &spMat,
                       Integer rowSize, Integer colSize);
   
   /// set the sparsity pattern of the entire sparse matrix with zeros or ones
   /// NOTE that: changing sparsity pattern takes some time;
   ///      so it is desirable to set sparsity pattern with zeros, first;
   ///      then fill the values at the last.
   ///      this approach it useful for user jacobian and hessian matrices.
   static void SetSparsityPattern(RSMatrix &spMat,
                                  Integer  rowSize, Integer colSize,
                                  const IntegerArray *rowIdxVec,
                                  const IntegerArray *colIdxVec,
                                  bool  hasZeros = false);

   /// set the data of the entire sparse matrix
   static void SetSparseMatrix(RSMatrix &spMat,
                               Integer rowSize, Integer colSize,
                               const IntegerArray *rowIdxVec,
                               const IntegerArray *colIdxVec,
                               const std::vector<Real> *valueVec);
   
   /// set the data of the entire sparse matrix
   static void SetSparseMatrix(RSMatrix &spMat,
                               Integer rowSize, Integer colSize,
                               const IntegerArray *rowIdxVec,
                               const IntegerArray *colIdxVec,
                               const Rvector *valueVec);

   /// set the data of a part of the sparse matrix
   /// if the isNotAdding option is off, then 
   /// the addition to the existing sparse matrix is done.
   static void SetSparseBLockMatrix(RSMatrix  &spMat,
                                    Integer   rowOffSet, Integer colOffSet,
                                    const IntegerArray *rowIdxVec,
                                    const IntegerArray *colIdxVec,
                                    const std::vector<Real> *valueVec,
                                    bool isNotAdding = true);

   /// set the data of a part of the sparse matrix
   /// if the isNotAdding option is off, then 
   /// the addition to the existing sparse matrix is done.
   static void SetSparseBLockMatrix(RSMatrix  &spMat,
                                    Integer rowOffSet, Integer colOffSet,
                                    const IntegerArray *rowIdxVec,
                                    const IntegerArray *colIdxVec,
                                    const Rvector *valueVec,
                                    bool isNotAdding = true);

   /// set the data of a part of the sparse matrix
   /// if the isNotAdding option is off, then 
   /// the addition to the existing sparse matrix is done.
   static void SetSparseBLockMatrix(RSMatrix  &spMat,
                                    Integer rowOffSet, Integer colOffSet,
                                    const RSMatrix  *spBlockMat,
                                    bool isNotAdding = true);

   /// set the data of a part of the sparse matrix
   /// if the isNotAdding option is off, then 
   /// the addition to the existing sparse matrix is done.
   static void SetSparseBLockMatrix(RSMatrix  &spMat,
                                    Integer rowOffSet, Integer colOffSet,
                                    const Rmatrix  *blockMat,
                                    bool isNotAdding = true);


   /// get methods
   static Real GetElement(const RSMatrix  *spMat,
                          Integer rowIdx, Integer colIdx);

   static Integer GetNumRows(const RSMatrix  *spMat);
   static Integer GetNumColumns(const RSMatrix  *spMat);

   /// get the sparsity pattern of the sparse matrix
   static RSMatrix GetSparsityPattern(const RSMatrix *spMat,
                                      bool hasZeros = false);

   /// get the sparsity pattern of the sparse matrix
   static void GetSparsityPattern(const RSMatrix *spMat,
                                 IntegerArray &rowIdxVec, 
                                 IntegerArray &colIdxVec);  

   /// set the data of the entire sparse matrix
   static void GetThreeVectorForm(const RSMatrix  *spMat,
                                 IntegerArray &rowIdxVec, 
                                 IntegerArray &colIdxVec, 
                                 std::vector<Real> &valueVec); 

   /// set the data of the entire sparse matrix
   static void GetThreeVectorForm(const RSMatrix *spMat,
                                 IntegerArray &rowIdxVec,
                                 IntegerArray &colIdxVec,
                                 Rvector &valueVec);

   /// set the data of a part of the sparse matrix
   static void GetThreeVectorForm(const RSMatrix *spMat,
                                 Integer rowIdxLB, Integer rowIdxUB,
                                 Integer colIdxLB, Integer colIdxUB,
                                 IntegerArray &rowIdxVec, 
                                 IntegerArray &colIdxVec, 
                                 std::vector<Real> &valueVec); 

   /// set the data of a part of the sparse matrix
   static void GetThreeVectorForm(const RSMatrix *spMat,
                                 Integer rowIdxLB, Integer rowIdxUB,
                                 Integer colIdxLB, Integer colIdxUB,
                                 IntegerArray &rowIdxVec,
                                 IntegerArray &colIdxVec,
                                 Rvector &valueVec);

   /// get the number of non zero elements of the sparse matrix
   static Integer GetNumNonZeroElements(const RSMatrix *spMat);


   /// Get sum(abs(element)) throughout a sparse matrix.
   /// useful especially when calculating errors
   static Real GetAbsTotalSum(const RSMatrix *spMat);


   // various utilities


   /// perform product between sparse matrix and dense vector
   /// B = A*V (initialize = true); where A and B are sparse matrices, and
   /// V is a dense vector
   /// B += A*V (initialize = false); where A and B are sparse matrices, and
   /// V is a dense vector
   static void fast_prod(const RSMatrix *spMat,
                         const std::vector<Real> *vec,
                         std::vector<Real> &result, bool initialize = true);

   /// perform product between sparse matrix and dense vector
   /// B = A*V (initialize = true); where A and B are sparse matrices, and
   /// V is a dense vector
   /// B += A*V (initialize = false); where A and B are sparse matrices, and
   /// V is a dense vector
   static void fast_prod(const RSMatrix *spMat,
                         const Rvector  *vec,
                         Rvector &result, bool initialize = true);

   /// perform C = A*B (initialize = true); 
   /// or C+= A*B (initialize = false) using axpy_prod of
   /// SparseMatrixUtil::ublas
   static void fast_prod(const RSMatrix *spMat1,
                         const RSMatrix *spMat2,
                         RSMatrix &resultMat,
                         bool initialize = true);

   /// print all the nonzero elements to the screen
   static void PrintNonZeroElements(const RSMatrix *spMat,
                                    bool rowOrder = true);
   
   /// replicate sparse block matrix pattern with row and column repetition and
   /// return spMat
   static void ReplicateSparseMatrix(const RSMatrix *spBlockMat,
                                    Integer numRowRepetition,
                                    Integer numColRepetition,
                                    RSMatrix &spMat);
   
   /// check whether the given matrix is zero matrix or not
   static bool IsZeroMatrix(const RSMatrix *spMat);

   // convert RSMatrix to Rmatrix
   static Rmatrix RSMatrixToRmatrix(const RSMatrix *spMat,
                                    Integer rowLB, Integer rowUB,
                                    Integer colLB, Integer colUB);

   // convert RSMatrix to Rmatrix
   static Rmatrix RSMatrixToRmatrix(const RSMatrix *spMat);

   /// copy the data of the input matrix into output matrix 
   static RSMatrix CopySparseMatrix(const RSMatrix *spMat);

   /// copy the data of the input matrix into output matrix 
   static void CopySparseMatrix(const RSMatrix &copyFrom, RSMatrix &copyTo);
   
private:
   /// private constructors, destructor, operator=   *** UNIMPLEMENTED ***
   SparseMatrixUtil();
   SparseMatrixUtil(const SparseMatrixUtil &copy);
   SparseMatrixUtil& operator=(const SparseMatrixUtil &copy);
   ~SparseMatrixUtil();
};

#endif // SparseMatrixUtil_hpp
