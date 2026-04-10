//------------------------------------------------------------------------------
//                                 Randn
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2016.04.20
//
/**
 * Declares Randn class.
 */
//------------------------------------------------------------------------------
#ifndef _Randn_hpp
#define _Randn_hpp

#include "MathFunction.hpp"

class GMAT_API Randn : public MathFunction
{
public:
   Randn(const std::string &nomme);
   virtual ~Randn();
   Randn(const Randn &copy);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();

protected:
   Integer GetOutputDimension();
   
private:
   
};

#endif // _Randn_hpp
