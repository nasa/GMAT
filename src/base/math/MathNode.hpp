//$Id$
//------------------------------------------------------------------------------
//                                   MathNode
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
// Author: Allison Greene
// Created: 2006/03/27
//
/**
 * Defines the MathNode base class for Math in scripts.
 */
//------------------------------------------------------------------------------
#ifndef MathNode_hpp
#define MathNode_hpp

#include "GmatBase.hpp"
#include "MathException.hpp"

/**
 * All function classes are derived from this base class.
 */
class GMAT_API MathNode : public GmatBase
{
public:
   
   MathNode(const std::string &typeStr, const std::string &nomme);
   virtual ~MathNode();
   MathNode(const MathNode &mn);
   MathNode& operator=(const MathNode &mn);
   
   bool                 IsFunction() { return isFunction; }
   bool                 IsNumber() { return isNumber; }
   bool                 IsFunctionInput() { return isFunctionInput; }
   void                 SetNumberFlag(bool flag) { isNumber = flag; }
   void                 SetFunctionInputFlag(bool flag) { isFunctionInput = flag; }
   
   Integer              GetElementType() { return elementType; }
   
   Real                 GetRealValue() { return realValue; }
   const Rmatrix&       GetMatrixValue() { return matrix; }
   
   void                 SetRealValue(Real val);
   virtual void         SetMatrixValue(const Rmatrix &mat);
   
   // for math elemement wrappers
   virtual void         SetMathWrappers(WrapperMap *wrapperMap);

   // for function node returning object
   virtual GmatBase*    EvaluateObject();

   // for function node returning string
   virtual std::string  EvaluateString();
   
   // abstract methods
   virtual bool         ValidateInputs() = 0;
   virtual void         GetOutputInfo(Integer &type, Integer &rowCount,
                                      Integer &colCount) = 0;
   virtual Real         Evaluate() = 0;
   virtual Rmatrix      MatrixEvaluate() = 0;
   virtual bool         SetChildren(MathNode *leftChild, MathNode *rightChild) = 0;
   virtual MathNode*    GetLeft() = 0;
   virtual MathNode*    GetRight() = 0;
   
   // Inherited (GmatBase) methods
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   DEFAULT_TO_NO_CLONES

protected:
   
   bool isNumber;
   bool isFunction;
   bool isFunctionInput;
   
   /// Element type (is the leaf node a real number or a matrix
   Integer elementType;
   
   Real realValue;
   Rmatrix matrix;
   std::string stringValue;
   
};


#endif //MathNode_hpp


