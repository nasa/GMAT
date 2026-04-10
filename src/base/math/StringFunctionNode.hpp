//$Id$
//------------------------------------------------------------------------------
//                                   StringFunctionNode
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
 * Defines the string functions base class.
 */
//------------------------------------------------------------------------------

#ifndef StringFunctionNode_hpp
#define StringFunctionNode_hpp

#include "BuiltinFunctionNode.hpp"

class GMAT_API StringFunctionNode : public BuiltinFunctionNode
{
public:
   StringFunctionNode(const std::string &typeStr, const std::string &name);
   virtual ~StringFunctionNode();
   StringFunctionNode(const StringFunctionNode &sf);
   StringFunctionNode& operator=(const StringFunctionNode &sf);
   
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   
protected:
   virtual void ValidateWrappers();
};

#endif //StringFunctionNode_hpp
