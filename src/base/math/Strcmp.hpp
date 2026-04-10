//------------------------------------------------------------------------------
//                              Strcmp
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
// Created: 2016.02.17
//
/**
 * Declares Strcmp class.
 */
//------------------------------------------------------------------------------
#ifndef Strcmp_hpp
#define Strcmp_hpp

#include "StringFunctionNode.hpp"

class GMAT_API Strcmp : public StringFunctionNode
{
public:
   Strcmp(const std::string &name);
   virtual ~Strcmp();
   Strcmp(const Strcmp &copy);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // inherited from MathNode
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual Real Evaluate();
};

#endif // Strcmp_hpp
