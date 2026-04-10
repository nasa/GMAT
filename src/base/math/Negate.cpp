//$Id$
//------------------------------------------------------------------------------
//                                  Negate
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
// Author: Linda Jun, NASA/GSFC
// Created: 2006/05/04
//
/**
 * Implements Negate class.
 */
//------------------------------------------------------------------------------

#include "Negate.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Negate()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Negate::Negate(const std::string &nomme)
   : MathFunction("Negate", nomme)
{
}


//------------------------------------------------------------------------------
// ~Negate()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Negate::~Negate()
{
}


//------------------------------------------------------------------------------
//  Negate(const Negate &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Negate object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Negate::Negate(const Negate &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Negate operation.
 *
 * @return clone of the Negate operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Negate::Clone() const
{
   return (new Negate(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Negate::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   
   if (!leftNode)
      throw MathException("Negate::GetOutputInfo() The left node is NULL");
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   type = type1;
   rowCount = row1;
   colCount = col1;
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Negate::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Negate() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // Left node

   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);

   // it can be any numeric type
   if ((type1 != Gmat::REAL_TYPE) && (type1 != Gmat::RMATRIX_TYPE))
   {
      std::string errmsg = "Invalid operand type (";
      errmsg += PARAM_TYPE_STRING[type1] + ") for negation operator.\n";
      throw MathException(errmsg);
   }

   return true;
   //return leftNode->ValidateInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Negate of left node
 *
 */
//------------------------------------------------------------------------------
Real Negate::Evaluate()
{
   return (leftNode->Evaluate() * -1);
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix Negate::MatrixEvaluate()
{
   return (leftNode->MatrixEvaluate() * -1);
}
