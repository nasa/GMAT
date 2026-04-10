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

#include "SimpleMathNode.hpp"
#include "MathException.hpp"
#include "RealUtilities.hpp"    // for GmatMathUtil::
#include "StringUtil.hpp"       // for GmatStringUtil::
#include "Array.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_SIMPLE_NODE 1
//#define DEBUG_SIMPLE_NODE_OBJ 1
//#define DEBUG_REAL_NODE 1
//#define DEBUG_MATRIX_NODE 1

//------------------------------------------------------------------------------
// SimpleMathNode(std::string typeStr, std::string name = "")
//------------------------------------------------------------------------------
/**
 * Constructs the SimpleMathNode object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
SimpleMathNode::SimpleMathNode(const std::string &typeStr, const std::string &name)
{
   realValue = 0.0;
   theType = typeStr;
   theName = name;
   theLeft = NULL;
   theRight = NULL;
   
   theReturnType = Gmat::REAL_TYPE;
   theRowCount = 0;
   theColCount = 0;

   isParameter = false;
   theParameter = NULL;
   
   if (typeStr == "MathElement")
   {
      theArgCount = 0;
      Real rval;
      if (!GmatStringUtil::ToDouble(name, &rval))
         isParameter = true;
   }
   else if (typeStr == "Add")
      theArgCount = 2;
   else if (typeStr == "Subtract")
      theArgCount = 2;
   else if (typeStr == "Multiply")
      theArgCount = 2;
   else if (typeStr == "Divide")
      theArgCount = 2;
   else if (typeStr == "sin")
      theArgCount = 1;
   else if (typeStr == "cos")
      theArgCount = 1;
   else if (typeStr == "tan")
      theArgCount = 1;
   else if (typeStr == "Negate")
      theArgCount = 1;
   else if (typeStr == "Power")
      theArgCount = 1;
   else if (typeStr == "sqrt")
      theArgCount = 1;
   else
      throw MathException(typeStr + " is undefined.\n");

   #if DEBUG_SIMPLE_NODE
   MessageInterface::ShowMessage
      ("SimpleMathNode::SimpleMathNode() typeStr=%s, name=%s, isParameter=%d\n",
       typeStr.c_str(), name.c_str(), isParameter);
   #endif

}


//------------------------------------------------------------------------------
//  ~SimpleMathNode(void)
//------------------------------------------------------------------------------
/**
 * Destroys the SimpleMathNode object (destructor).
 */
//------------------------------------------------------------------------------
SimpleMathNode::~SimpleMathNode()
{
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
bool SimpleMathNode::ValidateInputs()
{
   #if DEBUG_SIMPLE_NODE
   MessageInterface::ShowMessage
      ("SimpleMathNode::ValidateInputs() %s: theArgCount=%d\n",
       theType.c_str(), theArgCount);
   #endif
   
   Integer leftType, rightType;
   Integer leftRowCount, rightRowCount;
   Integer leftColCount, rightColCount;

   if (theArgCount == 0)
   {
      theReturnType = Gmat::REAL_TYPE;
      theRowCount = 0;
      theColCount = 0;
      return true;
   }
   else if (theArgCount == 1)
   {
      if (theLeft == NULL)
         throw MathException("Left node is NULL in " + GetTypeName() +
                             "::ValidateInputs()\n");
      
      theLeft->GetOutputInfo(leftType, leftRowCount, leftColCount);
      theReturnType = leftType;
      theRowCount = leftRowCount;
      theColCount = leftColCount;

      #if DEBUG_SIMPLE_NODE
      MessageInterface::ShowMessage
         ("SimpleMathNode::ValidateInputs() theReturnType=%d, theRowCount=%d, "
          "theColCount=%d\n", theReturnType, theRowCount, theColCount);
      #endif
      
      return true;
   }
   else if (theArgCount == 2)
   {
      if (theLeft == NULL || theRight == NULL)
         throw MathException("Left or Right node is NULL in " + GetTypeName() +
                             "::ValidateInputs()\n");
      
      theLeft->GetOutputInfo(leftType, leftRowCount, leftColCount);
      theRight->GetOutputInfo(rightType, rightRowCount, rightColCount);
      
      #if DEBUG_SIMPLE_NODE
      MessageInterface::ShowMessage
         ("SimpleMathNode::ValidateInputs() leftType=%d, leftRowCount=%d, "
          "leftColCount=%d\n", leftType, leftRowCount, leftColCount);
      
      MessageInterface::ShowMessage
         ("SimpleMathNode::ValidateInputs() rightType=%d, rightRowCount=%d, "
          "rightColCount=%d\n", rightType, rightRowCount, rightColCount);
      #endif
      
      if (leftType == rightType && leftRowCount == rightRowCount &&
          leftColCount == rightColCount)
      {
         theReturnType = leftType;
         theRowCount = leftRowCount;
         theColCount = leftColCount;
         return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void SimpleMathNode::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = theReturnType;
   rowCount = theRowCount;
   colCount = theColCount;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name = "")
//------------------------------------------------------------------------------
bool SimpleMathNode::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name)
{
   #if DEBUG_SIMPLE_NODE_OBJ
   MessageInterface::ShowMessage
      ("SimpleMathNode::SetRefObject() obj=%p, typeName=%s, type=%d, name=%s\n",
       obj, obj->GetTypeName().c_str(), type, name.c_str());
   #endif
   
   if (theType != "MathElement")
      throw MathException("SimpleMathNode::SetRefObject() Setting obj on non-leaf node");
   
   if (type != Gmat::PARAMETER)
      throw MathException("SimpleMathNode::SetRefObject() Setting invalid type");

   if (isParameter)
      if (theName != name)
         throw MathException("SimpleMathNode::SetRefObject() Cannot find parameter name:" +
                             name);
   
   theParameter = (Parameter*)obj;
   
   if (theParameter->GetReturnType() == Gmat::RMATRIX_TYPE)
   {
      Array *arr = (Array*)theParameter;     
      theReturnType = Gmat::RMATRIX_TYPE;
      theRowCount = arr->GetRowCount();
      theColCount = arr->GetColCount();
      
      rmat.SetSize(theRowCount, theColCount);
      rmat = arr->GetRmatrix();
      
      #if DEBUG_SIMPLE_NODE_OBJ
      std::stringstream ss("");
      ss << rmat;
      MessageInterface::ShowMessage
         ("SimpleMathNode::SetRefObject() theReturnType=%d, theRowCount=%d, "
          "theColCount=%d, rmat=\n%s\n", theReturnType, theRowCount, theColCount,
          ss.str().c_str());
      #endif
   }
   
   return true;
   
}


//------------------------------------------------------------------------------
// void Evaluate()
//------------------------------------------------------------------------------
Real SimpleMathNode::Evaluate()
{
   #if DEBUG_REAL_NODE
   MessageInterface::ShowMessage
      ("SimpleMathNode::Evaluate() theType=%s, theName=%s\n", theType.c_str(),
       theName.c_str());
   #endif
   
   if (theType == "MathElement")
   {
      #if DEBUG_REAL_NODE
      MessageInterface::ShowMessage
         ("SimpleMathNode::Evaluate() isParameter=%d\n", isParameter);
      #endif
      
      if (!isParameter)
      {
         #if DEBUG_REAL_NODE
         MessageInterface::ShowMessage
            ("SimpleMathNode::Evaluate() returning %f\n", realValue);
         #endif
         return realValue;
      }

      if (theParameter == NULL)
         throw MathException("SimpleMathNode::Evaluate() theParameter is NULL\n");
      
      #if DEBUG_REAL_NODE
      MessageInterface::ShowMessage
         ("SimpleMathNode::Evaluate() returning %f\n", theParameter->EvaluateReal());
      #endif
      
      return theParameter->EvaluateReal();
   }
   else
   {
      Real left = -999;
      Real right = -999;
      Real result = -999;
      
      if (theLeft != NULL)
         left = theLeft->Evaluate();

      if (theRight != NULL)
         right = theRight->Evaluate();

      if (theType == "Add")
         result = left + right;
      else if (theType == "Subtract")
         result = left - right;
      else if (theType == "Multiply")
         result = left * right;
      else if (theType == "Divide")
         result = left / right;
      else if (theType == "sin")
         result = GmatMathUtil::Sin(left);
      else if (theType == "cos")
         result = GmatMathUtil::Cos(left);
      else if (theType == "tan")
         result = GmatMathUtil::Tan(left);
      else if (theType == "Negate")
         result = -left;
      else if (theType == "Power")
         result = GmatMathUtil::Pow(left, right);
      else if (theType == "sqrt")
         result = GmatMathUtil::Sqrt(left);
      else
         throw MathException(theType + " is undefined.\n");    

      return result;
   }
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix SimpleMathNode::MatrixEvaluate()
{
   #if DEBUG_MATRIX_NODE
   MessageInterface::ShowMessage
      ("SimpleMathNode::MatrixEvaluate() theType=%s, theName=%s, isParameter\n",
       theType.c_str(), theName.c_str(), isParameter);
   #endif
   
   if (theType == "MathElement")
   {
      if (!isParameter)
      {
         #if DEBUG_MATRIX_NODE      
         std::stringstream ss("");
         ss << rmat;
         MessageInterface::ShowMessage
            ("SimpleMathNode::MatrixEvaluate() rmat=\n%s\n", ss.str().c_str());
         #endif
         
         return rmat;
      }
      
      if (theParameter == NULL)
         throw MathException("SimpleMathNode::MatrixEvaluate() theParameter is NULL");
      
      #if DEBUG_MATRIX_NODE
      std::stringstream ss("");
      ss << rmat;
      MessageInterface::ShowMessage
         ("SimpleMathNode::MatrixEvaluate() rmat=\n%s\n", ss.str().c_str());
      #endif
      
      return ((Array*)theParameter)->GetRmatrix();

   }
   else
   {
      Integer leftType, rightType;
      Integer leftRowCount, rightRowCount;
      Integer leftColCount, rightColCount;
      
      Rmatrix left;
      Rmatrix right;
      Rmatrix result;

      // Each math function knows the number of arguments,
      // since SimpleMathNode handles all function here, use theArgCount instead
      if (theArgCount > 0)
      {
         if (theLeft != NULL)
         {
            if (theLeft->ValidateInputs())
               left = theLeft->MatrixEvaluate();
            else
               throw MathException("Left node is NULL in " + GetTypeName() +
                                   "MatrixEvaluate()\n");
         }
      }
      
      if (theArgCount > 1)
      {
         if (theRight != NULL)
         {
            if (theRight->ValidateInputs())
               right = theRight->MatrixEvaluate();
            else
               throw MathException("Right node is NULL in " + GetTypeName() +
                                   "MatrixEvaluate()\n");
         }
      }

      #if DEBUG_MATRIX_NODE
      MessageInterface::ShowMessage
         ("SimpleMathNode::MatrixEvaluate() theReturnType=%d, theRowCount=%d, "
          "theColCount=%d\n",  theReturnType, theRowCount, theColCount);
      #endif
      
      result.SetSize(theRowCount, theColCount);
      
      if (theType == "Add")
         result = left + right;
      else if (theType == "Subtract")
         result = left - right;
      else if (theType == "Multiply")
         result = left * right;
      else if (theType == "Divide")
         result = left / right;
      else if (theType == "Transpose")
         result = left.Transpose();
      else if (theType == "Inverse")
         result = left.Inverse();
      else
         throw MathException(theType + " is undefined.\n");    

      #if DEBUG_MATRIX_NODE
      std::stringstream ss("");
      ss << result;
      MessageInterface::ShowMessage
         ("SimpleMathNode::MatrixEvaluate() result=\n%s\n", ss.str().c_str());
      #endif
      
      return result;
   }
}


// This should be in MathFunction
//------------------------------------------------------------------------------
// void SetChildren(SimpleMathNode *leftNode, SimpleMathNode *rightNode)
//------------------------------------------------------------------------------
void SimpleMathNode::SetChildren(SimpleMathNode *leftNode, SimpleMathNode *rightNode)
{
   theLeft = leftNode;
   theRight = rightNode;
}


// This should be in MathElement
//------------------------------------------------------------------------------
// void SetRealValue(Real val)
//------------------------------------------------------------------------------
void SimpleMathNode::SetRealValue(Real val)
{
   realValue = val;
}


//------------------------------------------------------------------------------
// void SetMatrixValue(const Rmatrix &mat)
//------------------------------------------------------------------------------
void SimpleMathNode::SetMatrixValue(const Rmatrix &mat)
{
   theReturnType = Gmat::RMATRIX_TYPE;
   theRowCount = mat.GetNumRows();
   theColCount = mat.GetNumColumns();
   rmat.SetSize(theRowCount, theColCount);
   rmat = mat;
   isParameter = false;
   
   #if DEBUG_MATRIX_NODE
   std::stringstream ss("");
   ss << rmat;
   MessageInterface::ShowMessage
      ("SimpleMathNode::SetMatrixValue() theReturnType=%d, theRowCount=%d, "
       "theColCount=%d, rmat=\n%s\n", theReturnType, theRowCount, theColCount,
       ss.str().c_str());
   #endif
}


