//$Id$
//------------------------------------------------------------------------------
//                                  Determinant
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
// Author: Allison Greene
// Created: 2006/04/20
//
/**
 * Implements Determinant class.
 */
//------------------------------------------------------------------------------

#include "Determinant.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_DETERMINANT 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Determinant()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const std::string &nomme)
   : MathFunction("Determinant", nomme)
{
}


//------------------------------------------------------------------------------
// ~Determinant()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Determinant::~Determinant()
{
}


//------------------------------------------------------------------------------
//  Determinant(const Determinant &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Determinant object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Determinant::Determinant(const Determinant &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Determinant operation.
 *
 * @return clone of the Determinant operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Determinant::Clone() const
{
   return (new Determinant(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Determinant::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;

   #if DEBUG_DETERMINANT
   MessageInterface::ShowMessage
      ("Determinant::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
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
bool Determinant::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Determinant() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // Left node

   #if DEBUG_DETERMINANT
   MessageInterface::ShowMessage
      ("Determinant::ValidateInputs() left=%s, %s\n",
       leftNode->GetTypeName().c_str(), leftNode->GetName().c_str());
   #endif
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   if ((type1 != Gmat::REAL_TYPE) && (type1 != Gmat::RMATRIX_TYPE))
   {
      std::string errmsg = "Invalid operand type (";
      errmsg += PARAM_TYPE_STRING[type1] + ") for determinant operator.\n";
      throw MathException(errmsg);
   }

   if ((type1 == Gmat::RMATRIX_TYPE) && (row1 != col1))
      throw MathException("Determinant only supports a square matrix.\n");

   return true;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Determinant of left node
 *
 */
//------------------------------------------------------------------------------
Real Determinant::Evaluate()
{
   Integer type, rowCount, colCount;
   leftNode->GetOutputInfo(type, rowCount, colCount);
   
   if (type == Gmat::RMATRIX_TYPE)
   {
      return (leftNode->MatrixEvaluate()).Determinant();
   }
   else
   {
      return leftNode->Evaluate();
   }
}


