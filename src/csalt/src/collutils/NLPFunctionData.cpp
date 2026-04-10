//$Id$
//------------------------------------------------------------------------------
//                      NLPFunctionData
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
 * Implementation for the NLPFunctionData class
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "NLPFunctionData.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_NLP_FUNCTION_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
NLPFunctionData::NLPFunctionData() :
   isJacSparsityPatternComputed (false)
{
   SparseMatrixUtil::SetSize(AMatrix, 1, 1);
   SparseMatrixUtil::SetSize(BMatrix, 1, 1);
   SparseMatrixUtil::SetSize(DMatrix, 1, 1);
   SparseMatrixUtil::SetSize(jacSparsityPattern, 1, 1);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
NLPFunctionData::NLPFunctionData(const NLPFunctionData &nlpFuncData)
{
   AMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.AMatrix);
   BMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.BMatrix);
   DMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.DMatrix);
   jacSparsityPattern = SparseMatrixUtil::CopySparseMatrix(&jacSparsityPattern);

   isJacSparsityPatternComputed = nlpFuncData.isJacSparsityPatternComputed;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
NLPFunctionData& NLPFunctionData::operator = (const NLPFunctionData &nlpFuncData)
{
   if (this == &nlpFuncData)
      return *this;

   AMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.AMatrix);
   BMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.BMatrix);
   DMatrix = SparseMatrixUtil::CopySparseMatrix(&nlpFuncData.DMatrix);
   jacSparsityPattern = SparseMatrixUtil::CopySparseMatrix(&jacSparsityPattern);

   isJacSparsityPatternComputed = nlpFuncData.isJacSparsityPatternComputed;

   return *this;
}

//------------------------------------------------------------------------------
// ~NLPFunctionData()
//------------------------------------------------------------------------------
/**
 * The default destructor. It does not do anything.
 */
//------------------------------------------------------------------------------
NLPFunctionData::~NLPFunctionData()
{
   // do nothing
}

//------------------------------------------------------------------------------
// void Initialize(Integer numFuncs, Integer numVars,
//                 Integer numFuncDependencies)
//------------------------------------------------------------------------------
/**
 * The initializer for NLPFunctionData class.
 *
 * @param <numFuncs>            number of functions
 * @param <numVars>             number of variables
 * @param <numFuncDependencies> number of function dependencies
 *
 * All the dimensions of the matrices are determined here.
 * They do not change hereafter.
 */
//------------------------------------------------------------------------------
void NLPFunctionData::Initialize(Integer numFuncs, Integer numVars,
                                 Integer numFuncDependencies)
{
   //Integer numCols = numFuncDependencies*numFuncs;
   isJacSparsityPatternComputed = false;

   SparseMatrixUtil::SetSize(AMatrix,numFuncs, numVars);
   SparseMatrixUtil::SetSize(BMatrix,numFuncs, numFuncDependencies);
   SparseMatrixUtil::SetSize(DMatrix, numFuncDependencies, numVars);
   SparseMatrixUtil::SetSize(jacSparsityPattern,numFuncs, numVars);
}

//------------------------------------------------------------------------------
// void InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const IntegerArray  *rowIdxVec,
//                          const IntegerArray  *colIdxVec,
//                          const Rvector *valueVec)
//------------------------------------------------------------------------------
/**
 * This method inserts a vector of values into the AMatrix, with the input
 * row and column offsets at the input row and column indexes.
 *
 * @param <rowOffSet>            row offset for beginning of input data
 * @param <colOffSet>            column offset for beginning of input data
 * @param <rowIdxVec>            vector of row indexes for the data
 * @param <colIdxVec>            vector of column indexes for the data
 * @param <valueVec>             vector of values to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
                                          const IntegerArray  *rowIdxVec,
                                          const IntegerArray  *colIdxVec,
                                          const Rvector *valueVec)
{
   SparseMatrixUtil::SetSparseBLockMatrix(AMatrix, rowOffSet, colOffSet,
                                          rowIdxVec, colIdxVec, valueVec);
}


//------------------------------------------------------------------------------
// void InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const Rmatrix *blockMatrix)
//------------------------------------------------------------------------------
/**
 * This method inserts a matrix of values into the AMatrix, with the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <blockMatrix>    matrix of row indexes for the data
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
                                          const Rmatrix *blockMatrix)
{
   SparseMatrixUtil::SetSparseBLockMatrix(AMatrix, rowOffSet,
                                          colOffSet,
                                          blockMatrix);
}

//------------------------------------------------------------------------------
// void InsertAMatElement(Integer rowOffSet, Integer colOffSet,
//                        Real value)
//------------------------------------------------------------------------------
/**
 * This method inserts a value into the AMatrix, at the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <value>          value to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertAMatElement(Integer rowIdx, Integer colIdx,
                                        Real value)
{
   SparseMatrixUtil::SetElement(AMatrix, rowIdx, colIdx, value);
}


//------------------------------------------------------------------------------
// void InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const IntegerArray  *rowIdxVec,
//                          const IntegerArray  *colIdxVec,
//                          const Rvector *valueVec)
//------------------------------------------------------------------------------
/**
 * This method inserts a vector of values into the BMatrix, with the input
 * row and column offsets at the input row and column indexes.
 *
 * @param <rowOffSet>            row offset for beginning of input data
 * @param <colOffSet>            column offset for beginning of input data
 * @param <rowIdxVec>            vector of row indexes for the data
 * @param <colIdxVec>            vector of column indexes for the data
 * @param <valueVec>             vector of values to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
                         const IntegerArray  *rowIdxVec,
                         const IntegerArray  *colIdxVec,
                         const Rvector *valueVec)
{
   SparseMatrixUtil::SetSparseBLockMatrix(BMatrix, rowOffSet, colOffSet,
                                          rowIdxVec, colIdxVec, valueVec);
}

//------------------------------------------------------------------------------
// void InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const Rmatrix *blockMatrix)
//------------------------------------------------------------------------------
/**
 * This method inserts a matrix of values into the BMatrix, with the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <blockMatrix>    matrix of row indexes for the data
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
                                          const Rmatrix *blockMatrix)
{
   SparseMatrixUtil::SetSparseBLockMatrix(BMatrix, rowOffSet,
                                          colOffSet,
                                          blockMatrix);
}

//------------------------------------------------------------------------------
// void InsertBMatElement(Integer rowOffSet, Integer colOffSet,
//                        Real value)
//------------------------------------------------------------------------------
/**
 * This method inserts a value into the BMatrix, at the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <value>          value to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertBMatElement(Integer rowIdx, Integer colIdx,
                                        Real value)
{
   SparseMatrixUtil::SetElement(BMatrix,rowIdx, colIdx, value);
}


//------------------------------------------------------------------------------
// void InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const IntegerArray  *rowIdxVec,
//                          const IntegerArray  *colIdxVec,
//                          const Rvector *valueVec)
//------------------------------------------------------------------------------
/**
 * This method inserts a vector of values into the DMatrix, with the input
 * row and column offsets at the input row and column indexes.
 *
 * @param <rowOffSet>            row offset for beginning of input data
 * @param <colOffSet>            column offset for beginning of input data
 * @param <rowIdxVec>            vector of row indexes for the data
 * @param <colIdxVec>            vector of column indexes for the data
 * @param <valueVec>             vector of values to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
                                          const IntegerArray  *rowIdxVec,
                                          const IntegerArray  *colIdxVec,
                                          const Rvector *valueVec)
{
   SparseMatrixUtil::SetSparseBLockMatrix(DMatrix, rowOffSet, colOffSet,
                                          rowIdxVec, colIdxVec, valueVec);
}

//------------------------------------------------------------------------------
// void InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
//                          const Rmatrix *blockMatrix)
//------------------------------------------------------------------------------
/**
 * This method inserts a matrix of values into the DMatrix, with the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <blockMatrix>    matrix of row indexes for the data
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
                         const Rmatrix *blockMatrix)
{
   SparseMatrixUtil::SetSparseBLockMatrix(DMatrix, rowOffSet,
                                          colOffSet,
                                          blockMatrix);
}

//------------------------------------------------------------------------------
// void InsertDMatElement(Integer rowOffSet, Integer colOffSet,
//                        Real value)
//------------------------------------------------------------------------------
/**
 * This method inserts a value into the DMatrix, at the input
 * row and column offsets.
 *
 * @param <rowOffSet>      row offset for beginning of input data
 * @param <colOffSet>      column offset for beginning of input data
 * @param <value>          value to insert
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::InsertDMatElement(Integer rowIdx, Integer colIdx,
                                        Real value)
{
   SparseMatrixUtil::SetElement(DMatrix,rowIdx, colIdx, value);
}

//------------------------------------------------------------------------------
// RSMatrix GetAMatrix()
//------------------------------------------------------------------------------
/**
 * This method returns the entire A matrix.
 *
 * @return the A matrix
 *
 */
//------------------------------------------------------------------------------
RSMatrix NLPFunctionData::GetAMatrix()
{
   return AMatrix;
};

//------------------------------------------------------------------------------
// RSMatrix GetBMatrix()
//------------------------------------------------------------------------------
/**
 * This method returns the entire B matrix.
 *
 * @return the B matrix
 *
 */
//------------------------------------------------------------------------------
RSMatrix NLPFunctionData::GetBMatrix()
{
   return BMatrix;
};

//------------------------------------------------------------------------------
// RSMatrix GetDMatrix()
//------------------------------------------------------------------------------
/**
 * This method returns the entire D matrix.
 *
 * @return the D matrix
 *
 */
//------------------------------------------------------------------------------
RSMatrix NLPFunctionData::GetDMatrix()
{
   return DMatrix;
};

//------------------------------------------------------------------------------
// const RSMatrix* GetAMatrixPtr()
//------------------------------------------------------------------------------
/**
 * This method returns a constant pointer to the A matrix.
 *
 * @return pointer to the A matrix
 *
 */
//------------------------------------------------------------------------------
const RSMatrix* NLPFunctionData::GetAMatrixPtr()
{
   return &AMatrix;
};

//------------------------------------------------------------------------------
// const RSMatrix* GetBMatrixPtr()
//------------------------------------------------------------------------------
/**
 * This method returns a constant pointer to the B matrix.
 *
 * @return pointer to the B matrix
 *
 */
//------------------------------------------------------------------------------
const RSMatrix* NLPFunctionData::GetBMatrixPtr()
{
   return &BMatrix;
};

//------------------------------------------------------------------------------
// const RSMatrix* GetDMatrixPtr()
//------------------------------------------------------------------------------
/**
 * This method returns a constant pointer to the D matrix.
 *
 * @return pointer to the D matrix
 *
 */
//------------------------------------------------------------------------------
const RSMatrix* NLPFunctionData::GetDMatrixPtr()
{
   return &DMatrix;
};


//------------------------------------------------------------------------------
// RSMatrix* GetJacSparsityPatternPointer()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the Jacobian Sparsity pattern.
 *
 * @return a pointer to the jacobian sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix* NLPFunctionData::GetJacSparsityPatternPointer()
{
   if (isJacSparsityPatternComputed == false)
      ComputeJacSparsityPattern();
   
   return &jacSparsityPattern;
}

//------------------------------------------------------------------------------
// void GetAMatrixThreeVectors(IntegerArray  &rowIdxVec,
//                             IntegerArray  &colIdxVec,
//                             Rvector       &valueVec)
//------------------------------------------------------------------------------
/**
 * Returns an array of row indexes, an array of column indexes, and
 * a vector of values - a 30vector representation of the AMatrix.
 *
 * @param <rowIdxVec>   output - array of row indexes (non-zero elements)
 * @param <colIdxVec>   output - array of column indexes (non-zero elements)
 * @param <valueVec>    output - vector of non-zero values
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::GetAMatrixThreeVectors(IntegerArray  &rowIdxVec,
                                             IntegerArray  &colIdxVec,
                                             Rvector &valueVec)
{
   SparseMatrixUtil::GetThreeVectorForm(&AMatrix, rowIdxVec, colIdxVec,
                                        valueVec);
}

//------------------------------------------------------------------------------
// void GetBMatrixThreeVectors(IntegerArray  &rowIdxVec,
//                             IntegerArray  &colIdxVec,
//                             Rvector       &valueVec)
//------------------------------------------------------------------------------
/**
 * Returns an array of row indexes, an array of column indexes, and
 * a vector of values - a 30vector representation of the BMatrix.
 *
 * @param <rowIdxVec>   output - array of row indexes (non-zero elements)
 * @param <colIdxVec>   output - array of column indexes (non-zero elements)
 * @param <valueVec>    output - vector of non-zero values
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::GetBMatrixThreeVectors(IntegerArray  &rowIdxVec,
                                             IntegerArray  &colIdxVec,
                                             Rvector       &valueVec)
{
   SparseMatrixUtil::GetThreeVectorForm(&BMatrix, rowIdxVec, colIdxVec,
                                        valueVec);
}


//------------------------------------------------------------------------------
// std::vector<double> ComputeFunctions(const Rvector *QVector,
//                                      const Rvector *DecVector,
//                                      Rvector       &funcValueVec)
//------------------------------------------------------------------------------
/**
 * Compute function values based on AMatrix, Decision Vector, BMatrix, QVector
 *
 * @param <QVector>      input Q-vector
 * @param <DecVector>    input decision vector
 * @param <funcValueVec> output function values
 *
 * Note that for performance improvement, 'fast_prod' is used here
 * instead of the ordinary 'prod'.
 *
 * @return STL vector contains function values.
 */
//------------------------------------------------------------------------------
void NLPFunctionData::ComputeFunctions(const Rvector *QVector,
                                       const Rvector *DecVector,
                                       Rvector       &funcValueVec)
{
   #ifdef DEBUG_NLP_FUNCTION_DATA
      MessageInterface::ShowMessage("NLPFunctionData entered.\n");
      MessageInterface::ShowMessage("   DecVector size is %d\n",
                                    DecVector->GetSize());
      MessageInterface::ShowMessage("   A matrix row size is %d\n",
                                    (Integer) AMatrix.size1());
      MessageInterface::ShowMessage("   A matrix column size is %d\n",
                                    (Integer) AMatrix.size2());
      MessageInterface::ShowMessage("AMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(&AMatrix);
   #endif
   
   // initialize funcValueVec
   bool isInitializing = true; // do not add previous results
   SparseMatrixUtil::fast_prod(&AMatrix,DecVector,funcValueVec, 
                              isInitializing);

   #ifdef DEBUG_NLP_FUNCTION_DATA
      MessageInterface::ShowMessage("BMatrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(&BMatrix);
   #endif
   // sum up the previous result
   isInitializing = false;  // add the previous values
   SparseMatrixUtil::fast_prod(&BMatrix, QVector, funcValueVec,
                               isInitializing);

}

//------------------------------------------------------------------------------
// std::vector<double> ComputeFunctions(const Rvector *QVector,
//                                      Rvector       &funcValueVec)
//------------------------------------------------------------------------------
/**
 * Compute function values based on BMatrix, QVector
 *
 * @param <QVector>      input Q-vector
 * @param <funcValueVec> output function values
 *
 * Note that for performance improvement, 'fast_prod' is used here
 * instead of the ordinary 'prod'.
 *
 * @return STL vector contains function values.
 */
//------------------------------------------------------------------------------
void NLPFunctionData::ComputeFunctions(const Rvector *QVector,
                                       Rvector       &funcValueVec)
{
   bool isInitializing = true; // do not add previous results
   SparseMatrixUtil::fast_prod(&BMatrix, QVector, funcValueVec, isInitializing);
}

//------------------------------------------------------------------------------
// bool ComputeJacobian(RSMatrix *parQMat, RSMatrix &funcJacobianMatrix)
//------------------------------------------------------------------------------
/**
 * Compute function Jacobian based on parQMatrix.
 * Note that for performance improvement, 'fast_prod' is used here
 * instead of the ordinary 'prod'.
 *
 * @param <parQMat>            Q matrix
 * @param <funcJacobianMatrix> output Jacobian matrix
 *
 * @return if the operation is successful, return true;
 */
//------------------------------------------------------------------------------
void NLPFunctionData::ComputeJacobian(RSMatrix *parQMat,
                                      RSMatrix &funcJacobianMatrix)
{
   funcJacobianMatrix  = AMatrix; // initialize funcJacobianMatrix
   bool isInitializing = false;  // add the previous values
   SparseMatrixUtil::fast_prod(&BMatrix,parQMat, funcJacobianMatrix, 
                              isInitializing);

   #ifdef DEBUG_NLP_FUNCTION_DATA
      MessageInterface::ShowMessage("jacobian matrix is given as follows:\n");
      SparseMatrixUtil::PrintNonZeroElements(&funcJacobianMatrix);
   #endif
}

//------------------------------------------------------------------------------
// void ComputeJacSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Compute the sparsity pattern of function Jacobian based on
 * AMatrix, BMatrix, parQMatrix.
 *
 */
//------------------------------------------------------------------------------
void NLPFunctionData::ComputeJacSparsityPattern()
{
   // initialize jacSparsityPattern
   RSMatrix localJacSPattern = SparseMatrixUtil::GetSparsityPattern(&AMatrix);
   
   // get sparsity patterns of B and parQ 
   RSMatrix BSPattern = SparseMatrixUtil::GetSparsityPattern(&BMatrix);
   RSMatrix DSPattern = SparseMatrixUtil::GetSparsityPattern(&DMatrix);
   
   // compute B*D and add it to jacSparsityPattern
   SparseMatrixUtil::fast_prod(&BSPattern, &DSPattern, localJacSPattern, false);

   jacSparsityPattern = SparseMatrixUtil::GetSparsityPattern(&localJacSPattern);

   isJacSparsityPatternComputed = true;
}

//------------------------------------------------------------------------------
// IntegerArray GetMatrixNumNonZeros()
//------------------------------------------------------------------------------
/**
 * Returns an array containing the number of non-zero numbers in the matrices
 * AMatrix and BMatrix.
 *
 * @return integer array containing number of non-zeros in AMatrix and BMatrix
 *
 */
//------------------------------------------------------------------------------
IntegerArray NLPFunctionData::GetMatrixNumNonZeros()
{
   IntegerArray numNonZerosVec;
   numNonZerosVec.resize(3);
   numNonZerosVec[0] = SparseMatrixUtil::GetNumNonZeroElements(&AMatrix);
   numNonZerosVec[1] = SparseMatrixUtil::GetNumNonZeroElements(&BMatrix);

   return numNonZerosVec;
}

//------------------------------------------------------------------------------
// bool SumBMatPartition(Integer rowOffSet, Integer colOffSet,
//                       IntegerArray  rowIdxVec,
//                       IntegerArray  colIdxVec,
//                       IntegerArray  valueVec)
//------------------------------------------------------------------------------
/**
 * Based on three vectors and row/column offset, it adds a sparse block
 * matrix to BMatrix.
 *
 * @param <rowOffSet> the row offset of the sparse block.
 * @param <rowOffSet> the column offset of the sparse block.
 * @param <rowIdxVec> the row indices of non-zero elements in the sparse block.
 * @param <colIdxVec> the col indices of non-zero elements in the sparse block.
 * @param <valueVec>  the values of non-zero elements in the sparse block.
 *
 * @return if the addition is successful, return true;
 *         otherwise return false.
 */
//------------------------------------------------------------------------------
void NLPFunctionData::SumBMatPartition(Integer rowOffSet, Integer colOffSet,
                                       const IntegerArray  *rowIdxVec,
                                       const IntegerArray  *colIdxVec,
                                       const Rvector       *valueVec)
{
   #ifdef DEBUG_NLP_FUNCTION_DATA
      MessageInterface::ShowMessage(" Entering SumBMatParition\n");
   #endif
   // SetSparseBlockMatrix without initialization
   // So, the addition is conducted.
   SparseMatrixUtil::SetSparseBLockMatrix(BMatrix, rowOffSet,
                                          colOffSet, rowIdxVec,
                                          colIdxVec, valueVec, false);
}

