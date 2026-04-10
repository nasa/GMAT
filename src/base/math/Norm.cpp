//$Id$
//------------------------------------------------------------------------------
//                                  Norm
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
 * Implements Norm class.
 */
//------------------------------------------------------------------------------

#include "Norm.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_NORM
//#define DEBUG_INPUT_OUTPUT 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Norm()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Norm::Norm(const std::string &nomme)
   : MathFunction("Norm", nomme)
{
}


//------------------------------------------------------------------------------
// ~Norm()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Norm::~Norm()
{
}


//------------------------------------------------------------------------------
//  Norm(const Norm &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Norm object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Norm::Norm(const Norm &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Norm operation.
 *
 * @return clone of the Norm operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Norm::Clone() const
{
   return (new Norm(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Norm::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;

   #ifdef DEBUG_NORM
   MessageInterface::ShowMessage
      ("Norm::GetOutputInfo() type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
   //GetMatrixOutputInfo(type, rowCount, colCount, true);
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Norm::ValidateInputs()
{
   return ValidateMatrixInputs(true);
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Norm of left node
 *
 */
//------------------------------------------------------------------------------
Real Norm::Evaluate()
{
   Integer type, rowCount, colCount;
   leftNode->GetOutputInfo(type, rowCount, colCount);
   
   if(type == Gmat::RMATRIX_TYPE)
   {
      if (rowCount == 1)
      {
        return (leftNode->MatrixEvaluate()).GetRow(0).Norm();
      }
      else if (colCount == 1)
      {
        return (leftNode->MatrixEvaluate()).GetColumn(0).Norm();
      }
      else
         // Norm() function works for a vector or scalar.
         // Norm() for matrix may be implemented in the future upon the
         // user request
         throw MathException(
            "Norm::Evaluate():: Can only be done on a vector or a scalar.  "
            "This is a matrix");
   }
   else
   {
      // Norm of a scalar should be the absolute value of the scalar.
      Real result = leftNode->Evaluate();
      return GmatMathUtil::Abs(result);
   }
}
