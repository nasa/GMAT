//$Id$
//------------------------------------------------------------------------------
//                                   MathElement
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
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math elements class for Math in scripts.
 */
//------------------------------------------------------------------------------

#ifndef MathElement_hpp
#define MathElement_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "MathNode.hpp"
#include "MathException.hpp"
#include "Parameter.hpp"
#include "Array.hpp"
#include "ElementWrapper.hpp"
#include <map>

class GMAT_API MathElement : public MathNode
{
public:
   MathElement(const std::string &typeStr, const std::string &nomme);
   virtual ~MathElement();
   MathElement(const MathElement &me);
   MathElement& operator=(const MathElement &me);
   
   // for math elemement wrappers
   virtual void         SetMathWrappers(WrapperMap *wrapperMap);
   
   // Inherited (MathNode) methods
   virtual void         SetMatrixValue(const Rmatrix &mat);
   virtual bool         ValidateInputs();
   virtual void         GetOutputInfo(Integer &type, Integer &rowCount,
                                      Integer &colCount);
   virtual bool         SetChildren(MathNode *leftChild, MathNode *rightChild);
   virtual MathNode*    GetLeft();
   virtual MathNode*    GetRight();
   
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   
   // Inherited (GmatBase) methods
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);   
   virtual GmatBase*    Clone(void) const; 
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
protected:
   
   /// A pointer to the referenced object (i.e. the leaf node or element).  
   /// This pointer is set when the MathTree is initialized in the Sandbox.
   Parameter* refObject;
   
   /// Holds the name of the GMAT object that is accessed by this node
   std::string refObjectName;    
   std::string refObjectType;
   
   /// The list of names of Wrapper objects
   StringArray wrapperObjectNames;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap *theWrapperMap;
   
   void SetWrapperObjectNames(const std::string &name);
   void SetWrapperObject(GmatBase *obj, const std::string &name);
   ElementWrapper* FindWrapper(const std::string &name);
};

#endif //MathElement_hpp
