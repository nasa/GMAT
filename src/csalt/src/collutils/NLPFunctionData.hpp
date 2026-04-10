//------------------------------------------------------------------------------
//                         NLPFunctionData  
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
// Created: 2015/06/22
//
/**
 * Definition for NLPFunctionData
 * Note that all the compressed_matrix is assumed to be row-major matrices. 
 * Although, all the matrices are sparse, all the vectors are dense.
 * The interface assumes that all the input data is given as STL vectors,
 * and all the output data is given as STL vectors.
 */
//------------------------------------------------------------------------------
#ifndef NLPFunctionData_hpp
#define NLPFunctionData_hpp

#include "csaltdefs.hpp"
#include "SparseMatrixUtil.hpp"

/**
 * NLPFunctionData class
 */
class NLPFunctionData
{
public:
    
   //  The nlp functions are computed using
   //       nlpFuncs = conAMat*DecVector + conBMat*userFuns
   //  The sparsity pattern is determined using
   //       sparsePattern = conAMat + conBMat*conDMat
   //  The Jacobian is calculated using 
   //       nlpJac = conAMat + conBMat*conQMat
    
   /// default constructor; 'size_t' is equal to 'unsigned int'
   NLPFunctionData();
   
   /// copy constructor
   NLPFunctionData(const NLPFunctionData &nlpFuncData);

   /// assignment operator
   NLPFunctionData&   operator=(const  NLPFunctionData &nlpFuncData);
   
   /// default destructor
   virtual ~NLPFunctionData();


   void Initialize(Integer numFuncs, Integer numVars, 
                   Integer numFuncDependencies); 

   /// methods for setting matrices and vectors

   void InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
                            const IntegerArray  *rowIdxVec,
                            const IntegerArray  *colIdxVec,
                            const Rvector *valueVec);

   void InsertAMatPartition(Integer rowOffSet, Integer colOffSet,
                            const Rmatrix *blockMatrix);

   void InsertAMatElement(Integer rowIdx, Integer colIdx, Real value);

   void InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
                            const IntegerArray  *rowIdxVec,
                            const IntegerArray  *colIdxVec,
                            const Rvector *valueVec);

   void InsertBMatPartition(Integer rowOffSet, Integer colOffSet,
                            const Rmatrix *blockMatrix);

   void InsertBMatElement(Integer rowIdx, Integer colIdx, Real value);

   void InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
                            const IntegerArray  *rowIdxVec,
                            const IntegerArray  *colIdxVec,
                            const Rvector *valueVec);

   void InsertDMatPartition(Integer rowOffSet, Integer colOffSet,
                            const Rmatrix *blockMatrix);

   void InsertDMatElement(Integer rowIdx, Integer colIdx, Real value);


   /// methods for access to the internal data
   RSMatrix        GetAMatrix();
   RSMatrix        GetBMatrix();
   RSMatrix        GetDMatrix();
   const RSMatrix* GetAMatrixPtr();
   const RSMatrix* GetBMatrixPtr();
   const RSMatrix* GetDMatrixPtr();

   RSMatrix*       GetJacSparsityPatternPointer();

   void            GetAMatrixThreeVectors(IntegerArray  &rowIdxVec,
                                          IntegerArray  &colIdxVec,
                                          Rvector       &valueVec);
   void            GetBMatrixThreeVectors(IntegerArray  &rowIdxVec,
                                          IntegerArray  &colIdxVec,
                                          Rvector       &valueVec);
 
   /// methods for computing results
   void            ComputeFunctions(const Rvector *QVector,
                                    Rvector       &funcValueVec);
   void            ComputeFunctions(const Rvector *QVector,
                                    const Rvector *DecVector,
                                    Rvector       &funcValueVec);

   void            ComputeJacSparsityPattern();
   void            ComputeJacobian(RSMatrix *parQMat, RSMatrix &jacobian);
                                                          
   /// other methods
   IntegerArray    GetMatrixNumNonZeros();

   void            SumBMatPartition(Integer             rowOffSet,
                                    Integer             colOffSet,
                                    const IntegerArray  *rowIdxVec,
                                    const IntegerArray  *colIdxVec,
                                    const Rvector       *valueVec);

protected:      
   /// Parameters

   /// A, B, and D matrices
   RSMatrix AMatrix;
   RSMatrix BMatrix;
   RSMatrix DMatrix;

   /// the jacobian sparsity pattern
   RSMatrix jacSparsityPattern;
   /// has the jacobian sparsity pattern has been computed?
   bool     isJacSparsityPatternComputed;

};

#endif // NLPFunctionData_hpp
