//$Id$
//------------------------------------------------------------------------------
//                             NumericJacobian
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.07.10
//
/**
* Declares the NumericJacobian class.  This class is a conversion of the numjac
* function from MATLAB.
*/
//------------------------------------------------------------------------------
#ifndef NumericJacobian_hpp
#define NumericJacobian_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "GmatConstants.hpp"
#include "PhysicalModel.hpp"
#include "MessageInterface.hpp"

class GMAT_API NumericJacobianOriginal
{
public:
   NumericJacobianOriginal();
   NumericJacobianOriginal(const NumericJacobianOriginal &copy);
   NumericJacobianOriginal&         operator=(const NumericJacobianOriginal &copy);
   ~NumericJacobianOriginal();

   virtual void CalculateJacobian(PhysicalModel* F, Rvector y, Rvector Fty,
                          Rmatrix threshScal, Rvector &fac, Integer vectorized,
                          Rmatrix S, IntegerArray g, Rmatrix &dFdy,
                          Integer &nfevals, Integer &nfcalls);
};
#endif
