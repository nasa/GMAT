//$Id$
//------------------------------------------------------------------------------
//                             SchurFactorization
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
* Declares SchurFactorization class.
*/
//------------------------------------------------------------------------------

#ifndef SchurFactorization_hpp
#define SchurFactorization_hpp

#include "utildefs.hpp"
#include "MatrixFactorization.hpp"

class GMATUTIL_API SchurFactorization : public MatrixFactorization
{
public:
   SchurFactorization();
   SchurFactorization(const SchurFactorization &schurfactorization);
   ~SchurFactorization();
   SchurFactorization& operator=(const SchurFactorization &schurfactorization);

   virtual void Factor(const Rmatrix inputMatrix, Rmatrix &sdUnitary,
                       Rmatrix &sdUpper);
   virtual void Invert(Rmatrix &inputMatrix);
   virtual void Invert(Real *sum1, Integer array_size);

   virtual void RemoveRowCol(Real *sum1, Integer num_rows,
      Integer row_to_remove, IntegerArray& removed_rows);
   virtual void RestoreAllRowCols(Real *sum1, Integer num_rows,
      IntegerArray& removed_rows);
};


#endif
