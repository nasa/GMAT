//$Id$
//------------------------------------------------------------------------------
//                                  Power
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
// Created: 2006/04/20
//
/**
 * Implements Power class.
 */
//------------------------------------------------------------------------------

#include "Power.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Power()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Power::Power(const std::string &nomme)
   : MathFunction("Power", nomme)
{
}


//------------------------------------------------------------------------------
// ~Power()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Power::~Power()
{
}


//------------------------------------------------------------------------------
//  Power(const Power &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Power object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Power::Power(const Power &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Power operation.
 *
 * @return clone of the Power operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Power::Clone() const
{
   return (new Power(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Power::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   GetScalarOutputInfo(type, rowCount, colCount);
   
   #if 0
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);
   
   if (type1 == Gmat::REAL_TYPE)
   {
      if (type2 == Gmat::REAL_TYPE || type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1)
      {
         type = type1;
         rowCount = row1;
         colCount = col1;
         retval = true;
      }
   }
   else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
   {
      if (type2 == Gmat::REAL_TYPE || (type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1))
      {
         type = type1;
         rowCount = row1;
         colCount = col1;
         retval = true;
      }
   }
   
   if (!retval)
      throw MathException("Input is not a scalar or 1x1 matrix, so can not do Power()");
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
bool Power::ValidateInputs()
{
   if (leftNode == NULL || rightNode == NULL)
      throw MathException("Power() - Missing input arguments.\n");
   
   Integer type1, row1, col1; // left node
   Integer type2, row2, col2; // right node
   bool retval = false;
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if (type1 == Gmat::REAL_TYPE)
   {
      if (type2 == Gmat::REAL_TYPE || (type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1))
         retval = true;
   }
   else if (type1 == Gmat::RMATRIX_TYPE && row1 == 1 && col1 == 1)
   {
      if (type2 == Gmat::REAL_TYPE || (type2 == Gmat::RMATRIX_TYPE && row2 == 1 && col2 == 1))
         retval = true;
   }
   else
   {
      std::string errmsg = "Invalid operand type (";
      errmsg += PARAM_TYPE_STRING[type1] + ") for power operator: must be a scalar or 1x1 matrix.\n";
      throw MathException(errmsg);
   }
//      throw MathException("Input is not a scalar or 1x1 matrix, so can not do Power()");
   
   return retval;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return left node raised to the right node power
 *
 */
//------------------------------------------------------------------------------
Real Power::Evaluate()
{
   return GmatMathUtil::Pow(leftNode->Evaluate(), rightNode->Evaluate());
}

