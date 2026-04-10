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


//------------------------------------------------------------------------------
//                                   SimpleMathNode
//------------------------------------------------------------------------------
// This class was created to unit test MathParser.
//------------------------------------------------------------------------------

#ifndef SimpleMathNode_hpp
#define SimpleMathNode_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "Parameter.hpp"

/**
 * All function classes are derived from this base class.
 */
class GMAT_API SimpleMathNode
{
public:
   SimpleMathNode(const std::string &typeStr, const std::string &name = "");
   virtual ~SimpleMathNode();

   std::string GetTypeName() { return theType; }
   std::string GetName() { return theName; }

   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
   
   virtual bool ValidateInputs();
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   
   //*** This should be in MathFunction
   void SetChildren(SimpleMathNode *leftNode, SimpleMathNode *rightNode);
   SimpleMathNode* GetLeft() { return theLeft; }
   SimpleMathNode* GetRight() { return theRight; }
   
   //*** This should be in MathElement
   void SetRealValue(Real val);
   void SetMatrixValue(const Rmatrix &mat);

//    virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                   const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
//    virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
protected:
   
   std::string theType;
   std::string theName;
   SimpleMathNode *theLeft;
   SimpleMathNode *theRight;
   
   Real realValue;
   Rmatrix rmat;

   //*** Do we want to add this to MathNode?
   Parameter *theParameter;
   bool isParameter;
   Integer theReturnType;
   Integer theRowCount;
   Integer theColCount;
   Integer theArgCount;
};


#endif //SimpleMathNode_hpp


