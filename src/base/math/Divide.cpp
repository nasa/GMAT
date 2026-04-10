//$Id$
//------------------------------------------------------------------------------
//                                  Divide
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
 * Implements Divide class.
 */
//------------------------------------------------------------------------------

#include "Divide.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INPUT_OUTPUT 1
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Divide()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Divide::Divide(const std::string &nomme)
   : MathFunction("Divide", nomme)
{
}


//------------------------------------------------------------------------------
// ~Divide()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Divide::~Divide()
{
}


//------------------------------------------------------------------------------
//  Divide(const Divide &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Divide object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Divide::Divide(const Divide &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Divide operation.
 *
 * @return clone of the Divide operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Divide::Clone() const
{
   return (new Divide(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Divide::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #if DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() entered, this=<%p><%s><%s>\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   if (leftNode)
      leftNode->GetOutputInfo(type1, row1, col1);
   else
      throw MathException("Left node is NULL in " + GetTypeName() +
                          "::GetOutputInfo()\n");
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   if (rightNode)
      rightNode->GetOutputInfo(type2, row2, col2);
   else
      throw MathException("Right node is NULL in " + GetTypeName() +
                          "::GetOutputInfo()\n");
   
   #if DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() type1=%d, row1=%d, col1=%d, type2=%d, "
       "row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   type = type1;
   rowCount = row1;
   colCount = col1;
   
   if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
   {
      // Check for 1x1
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
   }
   else if (type2 == Gmat::RMATRIX_TYPE)
   {
      type = type2;
      rowCount = row2;
      colCount = col2;
   }
   
   #if DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Divide::GetOutputInfo() leaving, type=%d, rowCount=%d, colCount=%d\n",
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
bool Divide::ValidateInputs()
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("\nDivide::ValidateInputs() '%s' entered\n", GetName().c_str());
   #endif
   
   if (leftNode == NULL)
      throw MathException("Divide() - Missing input arguments");
   
   if (rightNode == NULL)
      throw MathException("Divide() - Not enough input arguments");
   
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Divide::ValidateInputs() type1=%d, row1=%d, col1=%d, "
       "type2=%d, row2=%d, col2=%d\n", type1, row1, col1, type2, row2, col2);
   #endif
   
   bool leftNumeric  = (type1 == Gmat::REAL_TYPE) || (type1 == Gmat::RMATRIX_TYPE);
   bool rightNumeric = (type2 == Gmat::REAL_TYPE) || (type2 == Gmat::RMATRIX_TYPE);

   if (!leftNumeric && (!rightNumeric))
   {
      std::string errmsg = "Invalid operand types (";
      errmsg += PARAM_TYPE_STRING[type1] + ", ";
      errmsg += PARAM_TYPE_STRING[type2] + ") for division operator.\n";
      throw MathException(errmsg);
   }
   else if (!leftNumeric)
   {
      std::string errmsg = "Invalid operand type (";
      errmsg += PARAM_TYPE_STRING[type1] + ") for division operator.\n";
      throw MathException(errmsg);
   }
   else if (!rightNumeric)
   {
      std::string errmsg = "Invalid operand type (";
      errmsg += PARAM_TYPE_STRING[type2] + ") for division operator.\n";
      throw MathException(errmsg);
   }

   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
      retval = true;
   else if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
   {
      if ((row1 == row2) && (col1 == col2))
         retval = true;
      else if ((row1 == 1 && col1 == 1) || (row2 == 1 && col2 == 1))
         retval = true;
      else
         retval = false; 
   }
   else if ((type1 == Gmat::RMATRIX_TYPE) && (type2 == Gmat::REAL_TYPE))
      return true;
   else if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::RMATRIX_TYPE))
   {
      if ((row2 == 1) && (col2 == 1))
         retval = true;
      else
         throw MathException("Cannot divide a scalar by a matrix.\n");
   }
   else
      retval = false;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Divide::ValidateInputs() '%s' returning %d\n", GetName().c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Real Divide::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("\nDivide::Evaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   Real div;
   div = leftNode->Evaluate() / rightNode->Evaluate();
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Divide::Evaluate() '%s' returning %f\n", GetName().c_str(), div);
   #endif
   return div;
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the division of left node by the right node
 *
 */
//------------------------------------------------------------------------------
Rmatrix Divide::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("\nDivide::MatrixEvaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   Integer type1, row1, col1; // Left node matrix
   Integer type2, row2, col2; // Right node matrix
   Rmatrix div;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   // Divide matrix by matrix
   if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::RMATRIX_TYPE)
      div = leftNode->MatrixEvaluate() / rightNode->MatrixEvaluate();
   
   // Divide scalar by matrix
   else if( type1 == Gmat::REAL_TYPE && type2 == Gmat::RMATRIX_TYPE)
      div = leftNode->Evaluate() / rightNode->MatrixEvaluate();
   
   // Divide matrix by scalar
   else if( type1 == Gmat::RMATRIX_TYPE && type2 == Gmat::REAL_TYPE)
      div = leftNode->MatrixEvaluate() / rightNode->Evaluate();
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Divide::MatrixEvaluate() '%s' returning %s\n", GetName().c_str(), div.ToString().c_str());
   #endif
   
   return div;
}

