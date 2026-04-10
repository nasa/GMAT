//$Id$
//------------------------------------------------------------------------------
//                                  Subtract
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
 * Implements Subtract class.
 */
//------------------------------------------------------------------------------

#include "Subtract.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SUBTRACT 1
//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Subtract()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Subtract::Subtract(const std::string &nomme)
   : MathFunction("Subtract", nomme)
{
}


//------------------------------------------------------------------------------
// ~Subtract()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Subtract::~Subtract()
{
}


//------------------------------------------------------------------------------
//  Subtract(const Subtract &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Subtract object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Subtract::Subtract(const Subtract &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Subtract operation.
 *
 * @return clone of the Subtract operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Subtract::Clone() const
{
   return (new Subtract(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Subtract::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Subtract::GetOutputInfo() this=<%p><%s><%s> entered\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   if (!leftNode || !rightNode)
   {
      #ifdef DEBUG_INPUT_OUTPUT
      MessageInterface::ShowMessage
         ("Subtract::GetOutputInfo() throwing exception in '%s', leftNode=<%p>, "
          "rightNode=<%p>\n", GetName().c_str(), leftNode, rightNode);
      #endif
      throw MathException("Subtract() - Missing input arguments");
   }
   
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Subtract::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
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
         throw MathException("Matrixes are not the same can not subtract.\n");
   }
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Subtract::GetOutputInfo() this=<%p><%s><%s> leaving\n", this, GetTypeName().c_str(),
       GetName().c_str());
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
bool Subtract::ValidateInputs()
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("\nSubtract::ValidateInputs() '%s' entered\n", GetName().c_str());
   #endif
   
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   bool retval = false;
   
   if (!leftNode || !rightNode)
   {
      #ifdef DEBUG_INPUT_OUTPUT
      MessageInterface::ShowMessage
         ("Subtract::ValidateInputs() throwing exception in '%s', leftNode=<%p>, "
          "rightNode=<%p>\n", GetName().c_str(), leftNode, rightNode);
      #endif
      throw MathException("Subtract() - Missing input arguments");
   }
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Subtract::ValidateInputs() type1=%d, row1=%d, col1=%d, "
       "type2=%d, row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
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
      ("Subtract::ValidateInputs() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the difference of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Subtract::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("Subtract::Evaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   if (ValidateInputs())
      return leftNode->Evaluate() - rightNode->Evaluate();
   else
      throw MathException("Subtract::Both operands should be of type Real.\n");    
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the difference of left node(matrix) and right node(matrix)
 *
 */
//------------------------------------------------------------------------------
Rmatrix Subtract::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("Subtract::MatrixEvaluate() '%s' entered\n", GetName().c_str());
   #endif
      
   Integer type1 = -1, row1 = -1, col1 = -1; // Left node
   Integer type2 = -1, row2 = -1, col2 = -1; // Right node
   
   if (leftNode)
   {
      leftNode->GetOutputInfo(type1, row1, col1);
      rightNode->GetOutputInfo(type2, row2, col2);
      
      if (type1 == Gmat::RMATRIX_TYPE && type2 ==  Gmat::RMATRIX_TYPE)
         return leftNode->MatrixEvaluate() - rightNode->MatrixEvaluate();
      else if (type1 == Gmat::RMATRIX_TYPE && type2 ==  Gmat::REAL_TYPE)
         return leftNode->MatrixEvaluate() - rightNode->Evaluate();
      else if (type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
         return leftNode->Evaluate() - rightNode->MatrixEvaluate();
      else
         return MathFunction::MatrixEvaluate();
   }
   else
      return rightNode->MatrixEvaluate();
}
