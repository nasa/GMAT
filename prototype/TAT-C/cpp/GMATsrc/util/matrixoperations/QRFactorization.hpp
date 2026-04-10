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
* Declares QR factorization methods.  This class strictly uses std::vector
* operations to create and factor the matrix.
*/
//------------------------------------------------------------------------------
#ifndef QRFactorization_hpp
#define QRFactorization_hpp

#include <vector>
#include "MatrixFactorization.hpp"
#include "utildefs.hpp"

class GMATUTIL_API QRFactorization : public MatrixFactorization
{
public:
   QRFactorization(bool pivotOption = true);
   QRFactorization(const QRFactorization &qrfactorization);
   ~QRFactorization();
   QRFactorization& operator=(const QRFactorization &qrfactorization);

   void Factor(const Rmatrix A, Rmatrix &R, Rmatrix &Q);
   void RemoveFromQR(Rmatrix R, Rmatrix Q,
      std::string dimensionToRemove, Integer locationToRemove, Rmatrix &R1, Rmatrix &Q1);
   void AddToQR(Rmatrix R, Rmatrix Q,
      std::string dimensionToInsert, Integer locationToInsert, Rvector newElements, Rmatrix &R1, Rmatrix &Q1);
   void Invert(Rmatrix &inputMatrix);
   Real Determinant(Rmatrix A);
   Rmatrix GetParameterMatrix();

private:
   void Givens(const double el1, const double el2);

   /// The determinant of the matrix
   double determinant;
   /// Cosine of Given rotation
   double c;
   /// Sine of Given rotation
   double s;
   /// Shows whether there are less rows than columns in input matrix
   bool hasLessRows = false;
   /// Number of zero rows that were added so m = n
   int rowsAdded = 0;
   /// Number of rows in input matrix
   int m = 0;
   /// Number of columns in input matrix
   int n = 0;
   /// Boolean representing whether pivoting is used for the factoring
   bool usePivot;
   /// Permutation matrix representing which columns have been switched when
   /// pivoting is used
   Rmatrix permuMatrix;
};
#endif
