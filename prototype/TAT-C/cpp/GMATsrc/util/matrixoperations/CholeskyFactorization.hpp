//$Id$
//------------------------------------------------------------------------------
//                             CholeskyFactorization
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
* Declares CholeskyFactorization class.
*/
//------------------------------------------------------------------------------

#ifndef CholeskyFactorization_hpp
#define CholeskyFactorization_hpp

#include "utildefs.hpp"
#include "MatrixFactorization.hpp"

class GMATUTIL_API CholeskyFactorization : public MatrixFactorization
{
public:
   CholeskyFactorization();
   CholeskyFactorization(const CholeskyFactorization &choleskyfactorization);
   ~CholeskyFactorization();
   CholeskyFactorization& operator=(const CholeskyFactorization
                                    &choleskyfactorization);

   virtual void Factor(const Rmatrix inputMatrix, Rmatrix &R,
                       Rmatrix &blankMatrix);
   virtual void Invert(Rmatrix &inputMatrix);
   virtual Integer Invert(Real* sum1, Integer array_size);

private:
   /// Number of rows in input matrix
   Integer rowCount;
   /// Various indexes used in arrays along with error counters
   Integer i, i1, i2, i3, ist, iERowCount, iError, il, il1, il2;
   /// Various indexes used in arrays along with an index for pivot positions
   Integer j, k, k1, kl, iLeRowCount, rowCountIf, iPivot;
   /// Pointer used to store the input matrix in packed upper triangular form
   Real *sum1;
};


#endif
