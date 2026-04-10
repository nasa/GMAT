//$Id$
//------------------------------------------------------------------------------
//                                   Min
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
// Created: 2016.02.17
//
/**
 * Defines the Min function class.
 */
//------------------------------------------------------------------------------

#ifndef Min_hpp
#define Min_hpp

#include "GmatBase.hpp"
#include "NumericFunctionNode.hpp"
#include "MathException.hpp"

class GMAT_API Min : public NumericFunctionNode
{
public:
   Min(const std::string &name);
   virtual ~Min();
   Min(const Min &nf);
   Min& operator=(const Min &nf);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // inherited from MathNode
   virtual Real Evaluate();
   
protected:
};

#endif //Min_hpp
