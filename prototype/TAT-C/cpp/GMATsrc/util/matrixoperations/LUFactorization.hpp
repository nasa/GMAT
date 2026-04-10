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
* Declares LU factorization methods.  This class strictly uses Rvector and
* Rmatrix from Gmat code for vector and matrix operations.
*/
//------------------------------------------------------------------------------

#ifndef LUFactorization_hpp
#define LUFactorization_hpp

#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "utildefs.hpp"
#include "MatrixFactorization.hpp"

class GMATUTIL_API LUFactorization : public MatrixFactorization
{
public:
   LUFactorization(bool pivotOption = true);
   LUFactorization(const LUFactorization &lufactorization);
   ~LUFactorization();
   LUFactorization& operator=(const LUFactorization &lufactorization);

   void Factor(const Rmatrix inputMatrix, Rmatrix &L, Rmatrix &U);
   void Invert(Rmatrix &inputMatrix);
   void SolveSystem(const Rmatrix inputMatrix, Rvector b, Rvector &x);
   Real Determinant(Rmatrix A);

private:
   /// Number of rows in input matrix
   int rowCount;
   /// Number of columns in input matrix
   int colCount;
   /// Determinant of input matrix
   double determinant;
   /// Boolean of whether pivoting is used during calculations
   bool usePivot;
   /// Permutation vector representing which rows are switched during pivoting
   Rvector permuVector;
};
#endif
