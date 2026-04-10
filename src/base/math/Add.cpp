//$Id$
//------------------------------------------------------------------------------
//                                  Add
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
// Author: LaMont Ruley
// Created: 2006/03/31
//
/**
 * Implements Add class.
 */
//------------------------------------------------------------------------------

#include "Add.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ADD
//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Add()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Add::Add(const std::string &nomme)
   : MathFunction("Add", nomme)
{
}


//------------------------------------------------------------------------------
// ~Add()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Add::~Add()
{
}


//------------------------------------------------------------------------------
//  Add(const Add &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Add object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Add::Add(const Add &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Add operation.
 *
 * @return clone of the Add operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Add::Clone() const
{
   return (new Add(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Add::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() exp=%s, leftNode=<%p><%s>, rightNode=<%p><%s>\n",
       GetName().c_str(), leftNode, leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       rightNode, rightNode ? rightNode->GetTypeName().c_str() : "NULL");
   #endif
   
   // leftNode can be NULL for unaraty operator +, so commented out (LOJ: 2010.10.26)
   // if (!leftNode)
   //    throw MathException("Add::GetOutputInfo() The left node is NULL");
   
   if (!rightNode)
      throw MathException("Add::GetOutputInfo() The right node is NULL");
   
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   // If leftNode is NULL, set leftNode type same as rightNode for unary + operator
   if (leftNode == NULL)
   {
      type1 = type2;
      row1 = row2;
      col1 = col2;
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   if ((type1 != type2) || (row1 != row2) || (col1 != col2))
   {
      // We want to allow 1x1 - MxN or MxN - 1x1, so check
      if (row1 == 1 && col1 == 1)
      {
         type = type2;
         rowCount = row2;
         colCount = col2;
      }
      else if (row2 == 1 && col2 == 1)
      {
         type = type1;
         rowCount = row1;
         colCount = col1;
      }
      else
         throw MathException("Dimensions are not the same, cannot add.\n");
   }
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::GetOutputInfo() returning type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Add::ValidateInputs()
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::ValidateInputs() '%s' entered\n", GetName().c_str());
   #endif
   
   if (rightNode == NULL)
      throw MathException("Add() - Not enough input arguments");
   
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   bool retval = false;
   
   // We can do unary operator + such as, x = + var, so leftNode can be NULL
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   // If leftNode is NULL, set leftNode type same as rightNode for unary + operator
   if (leftNode == NULL)
   {
      type1 = type2;
      row1 = row2;
      col1 = col2;
   }
   
   // If any one side is 1x1, then it is valid, so check first
   if ((row1 == 1 && col1 == 1) || (row2 == 1 && col2 == 1))
   {
      retval = true;
   }
   else if (type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
   {
      if (row1 == row2 && col1 == col2)
         retval = true;
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Add::ValidateInputs() '%s' returning %s\n", GetName().c_str(),
       retval ? "true" : "false");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the sum of left node(real) and right node(real)
 *
 */
//------------------------------------------------------------------------------
Real Add::Evaluate()
{
   #if DEBUG_EVLAUATE
   MessageInterface::ShowMessage
      ("Add::Evaluate() left=%s, right=%s\n",
       leftNode ? leftNode->Evaluate().ToString().c_str() : "NULL",
       rightNode->Evaluate().ToString().c_str());
   #endif
   
   // For unary operator + , leftNode can be NULL
   if (leftNode)
      return leftNode->Evaluate() + rightNode->Evaluate();
   else
      return rightNode->Evaluate();
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the sum of left node(matrix) and right node(matrix)
 *
 */
//------------------------------------------------------------------------------
Rmatrix Add::MatrixEvaluate()
{
   #if DEBUG_EVLAUATE
   MessageInterface::ShowMessage
      ("Add::MatrixEvaluate() left=%s, right=%s\n",
       leftNode ? leftNode->MatrixEvaluate().ToString().c_str() : "NULL",
       rightNode->MatrixEvaluate().ToString().c_str());
   #endif
   
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   
   if (leftNode)
   {
      leftNode->GetOutputInfo(type1, row1, col1);
      rightNode->GetOutputInfo(type2, row2, col2);
      
      if (type1 == Gmat::RMATRIX_TYPE && type2 ==  Gmat::RMATRIX_TYPE)
         return leftNode->MatrixEvaluate() + rightNode->MatrixEvaluate();
      else if (type1 == Gmat::RMATRIX_TYPE && type2 ==  Gmat::REAL_TYPE)
         return leftNode->MatrixEvaluate() + rightNode->Evaluate();
      else if (type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
         return leftNode->Evaluate() + rightNode->MatrixEvaluate();
      else
         return MathFunction::MatrixEvaluate();
   }
   else
      return rightNode->MatrixEvaluate();
}

