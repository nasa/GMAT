//$Id$
//------------------------------------------------------------------------------
//                                   BuiltinFunctionNode
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
 * Defines GMAT builtin function base class.
 */
//------------------------------------------------------------------------------

#ifndef BuiltinFunctionNode_hpp
#define BuiltinFunctionNode_hpp

#include "MathNode.hpp"

class GMAT_API BuiltinFunctionNode : public MathNode
{
public:
   BuiltinFunctionNode(const std::string &typeStr, const std::string &name);
   virtual ~BuiltinFunctionNode();
   BuiltinFunctionNode(const BuiltinFunctionNode &func);
   BuiltinFunctionNode& operator=(const BuiltinFunctionNode &func);
   
   // for function argument wrappers
   virtual void         SetMathWrappers(WrapperMap *wrapperMap);
   
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   
   virtual bool SetChildren(MathNode *leftChild, MathNode *rightChild);
   virtual MathNode*    GetLeft();
   virtual MathNode*    GetRight();
   
   // Inherited from GmatBase
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   
protected:
   std::string desc;
   
   /// The list of input names
   StringArray inputNames;
   /// The list of output names
   StringArray outputNames;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap *theWrapperMap;
   WrapperArray inputArgWrappers;
   WrapperArray outputArgWrappers;
   
   virtual void ValidateWrappers();
};

#endif //BuiltinFunctionNode_hpp
