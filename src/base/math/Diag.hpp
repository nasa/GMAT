//$Id$
//------------------------------------------------------------------------------
//                                   Diag
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.03.09
//
/**
 * Defines the Diag function class. Diag function generates diagonal matrix.
 */
//------------------------------------------------------------------------------

#ifndef Diag_hpp
#define Diag_hpp

#include "GmatBase.hpp"
#include "NumericFunctionNode.hpp"
#include "MathException.hpp"

class GMAT_API Diag : public NumericFunctionNode
{
public:
   Diag(const std::string &name);
   virtual ~Diag();
   Diag(const Diag &nf);
   Diag& operator=(const Diag &nf);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // inherited from MathNode
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual Rmatrix MatrixEvaluate();
   
protected:
   RealArray diagValues;
   Integer numRows;
   Integer numCols;
};

#endif //Diag_hpp
