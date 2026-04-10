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

#ifndef MatrixFactorization_hpp
#define MatrixFactorization_hpp

#include "utildefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

class GMATUTIL_API MatrixFactorization
{
public:
   MatrixFactorization();
   MatrixFactorization(const MatrixFactorization &matrixfactorization);
   ~MatrixFactorization();
   MatrixFactorization& operator=(const MatrixFactorization 
                                  &matrixfactorization);

   virtual void Invert(Rmatrix &inputMatrix) = 0;
   virtual void Factor(const Rmatrix inputMatrix, Rmatrix &output1,
                       Rmatrix &output2) = 0;

   static Rmatrix CompressNormalMatrix(const Rmatrix &infMatrix,
      IntegerArray &removedIndexes, IntegerArray &auxVector,
      Integer &numRemoved);
   static Rmatrix ExpandNormalMatrixInverse(const Rmatrix &covMatrix,
      const IntegerArray &auxVector, const Integer &numRemoved);
   static Integer PackedArrayIndex(Integer n, Integer i, Integer j);
};


#endif
