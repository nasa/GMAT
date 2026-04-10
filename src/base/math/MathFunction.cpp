//$Id$
//------------------------------------------------------------------------------
//                                   MathFunction
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
 * Defines the Math functions class for Math in scripts.
 */
//------------------------------------------------------------------------------

#include "MathFunction.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_VALIDATE_INPUT
//#define DEBUG_INPUT_OUTPUT

//------------------------------------------------------------------------------
//  MathFunction(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathFunction::MathFunction(const std::string &typeStr, const std::string &nomme) :
   MathNode    (typeStr, nomme),
   leftNode    (NULL),
   rightNode   (NULL)
{
   isFunction = true;
   objectTypeNames.push_back("MathFunction");   
}

//------------------------------------------------------------------------------
//  ~MathFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathFunction object (destructor).
 */
//------------------------------------------------------------------------------
MathFunction::~MathFunction()
{
   // all math nodes are deleted in MathTree destructor (loj: 2008.11.03)
   //MessageInterface::ShowMessage
   //   ("==> MathFunction::~MathFunction() deleting %s, %s\n", GetTypeName().c_str(),
   //    GetName().c_str());
   
//    MathNode *left = GetLeft();
//    MathNode *right = GetRight();
   
//    if (left != NULL)
//       delete left;
   
//    if (right != NULL)
//       delete right;
}

//------------------------------------------------------------------------------
//  MathFunction(const MathFunction &mf)
//------------------------------------------------------------------------------
/**
 * Constructs the MathFunction object (copy constructor).
 * 
 * @param <mf> Object that is copied
 */
//------------------------------------------------------------------------------
MathFunction::MathFunction(const MathFunction &mf) :
   MathNode   (mf),
   leftNode   (mf.leftNode),
   rightNode  (mf.rightNode)
{
}

//------------------------------------------------------------------------------
//  MathFunction& operator=(const MathFunction &mf)
//------------------------------------------------------------------------------
/**
 * Sets one MathFunction object to match another (assignment operator).
 * 
 * @param <mf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathFunction& MathFunction::operator=(const MathFunction &mf)
{
   if (this == &mf)
      return *this;
   
   GmatBase::operator=(mf);
   leftNode   = mf.leftNode;
   rightNode  = mf.rightNode;
   
   return *this;
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathFunction::Evaluate()
{
   throw MathException(GetTypeName() + " cannot return Real");
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathFunction::MatrixEvaluate()
{
   throw MathException(GetTypeName() + " cannot return Matrix");
}


//------------------------------------------------------------------------------
// bool SetChildren(MathNode *leftChild, MathNode *rightChild)()
//------------------------------------------------------------------------------
bool MathFunction::SetChildren(MathNode *leftChild, MathNode *rightChild)
{
   if (leftChild)
      leftNode = leftChild;
   else
      leftNode = NULL;
   
   if (rightChild)
      rightNode = rightChild;
   else
      rightNode = NULL;
   
   return true;
}


//------------------------------------------------------------------------------
// bool GetLeft()
//------------------------------------------------------------------------------
MathNode* MathFunction::GetLeft()
{
   return leftNode;
}


//------------------------------------------------------------------------------
// bool GetRight()
//------------------------------------------------------------------------------
MathNode* MathFunction::GetRight()
{
   return rightNode;
}


//------------------------------------------------------------------------------
// void GetScalarOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void MathFunction::GetScalarOutputInfo(Integer &type, Integer &rowCount,
                                       Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::GetScalarOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   ///@note It is OK to set type to REAL_TYPE for 1x1 matrix since
   /// ArrayWrapper::EvaluateReal() returns 1x1 matrix as real number
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
      
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::GetScalarOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// void GetMatrixOutputInfo(Integer &type, Integer &rowCount, Integer &colCount,
//                          bool allowScalarInput)
//------------------------------------------------------------------------------
void MathFunction::GetMatrixOutputInfo(Integer &type, Integer &rowCount,
                                       Integer &colCount, bool allowScalarInput)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::GetMatrixOutputInfo() <%p><%s> entered, allowScalarInput=%d\n"
       "   leftNode=<%p><%s>\n   rightNode=<%p><%s>\n", this, GetTypeName().c_str(),
       allowScalarInput,
       leftNode, leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       rightNode, rightNode ? rightNode->GetTypeName().c_str() : "NULL");
   #endif
   
   if (!leftNode)
      throw MathException(GetTypeName() + "() - Missing input arguments");
   
   Integer type1, row1, col1; // Left node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (!allowScalarInput && type1 != Gmat::RMATRIX_TYPE)
      throw MathException("Left is not a matrix, so cannot do " + GetTypeName() + "()");
   
   // output row and col is transpose of leftNode's row and col
   type = type1;
   rowCount = col1;
   colCount = row1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::GetMatrixOutputInfo() <%p><%s> returning type=%d, rowCount=%d, "
       "colCount=%d\n", this, GetTypeName().c_str(), type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateScalarInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool MathFunction::ValidateScalarInputs()
{
   if (!leftNode)
      throw MathException(GetTypeName() + "() - Missing input arguments");
   
   Integer type1, row1, col1; // Left node
   bool retval = false;
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("MathFunction::ValidateScalarInputs() <%p><%s> entered\n   left=%s, %s\n",
       this, GetTypeName().c_str(), leftNode->GetTypeName().c_str(),
       leftNode->GetName().c_str());
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 == Gmat::REAL_TYPE)
      retval = true;
   else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
      retval = true;
   else
      throw MathException("Input to " + GetTypeName() + "() is not a scalar or "
                          "1x1 matrix, so cannot do " +  GetTypeName() + "()");
   
   if (rightNode)
   {
      leftNode->GetOutputInfo(type1, row1, col1);
      
      if (type1 == Gmat::REAL_TYPE)
         retval = true;
      else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
         retval = true;
      else
         throw MathException("Input to " + GetTypeName() + "() is not a scalar or "
                             "1x1 matrix, so cannot do " +  GetTypeName() + "()");
   }
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("MathFunction::ValidateScalarInputs() <%p><%s> returning %s, "
       "type=%d, row=%d, col=%d\n", this, GetTypeName().c_str(),
       retval ? "true" : "false", type1, row1, col1);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateMatrixInputs(bool allowScalarInput)
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool MathFunction::ValidateMatrixInputs(bool allowScalarInput)
{
   Integer type1, row1, col1; // Left node
   bool retval = false;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::ValidateMatrixInputs() <%p><%s>'%s' entered, leftNode=%s, %s\n",
       this, GetTypeName().c_str(), GetName().c_str(),
       leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       leftNode ? leftNode->GetName().c_str() : "NULL");
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if (type1 == Gmat::RMATRIX_TYPE)
      retval = true;
   else if (allowScalarInput && type1 == Gmat::REAL_TYPE)
      retval = true;
   else
   {
      std::stringstream errorMsg;
      errorMsg << "Input is not a matrix ";

      if (allowScalarInput)
         errorMsg << "or scalar, ";
      else
         errorMsg << "and scalar input is not allowed, ";
      errorMsg << "so the " << GetTypeName()
               << "() function cannot be evaluated.";

      throw MathException(errorMsg.str());
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathFunction::ValidateMatrixInputs() <%p><%s>'%s' returning %d\n",
       this, GetTypeName().c_str(), GetName().c_str(), retval);
   #endif
   
   return retval;
}

