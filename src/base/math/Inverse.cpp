//$Id$
//------------------------------------------------------------------------------
//                                  Inverse
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
 * Implements Inverse class.
 */
//------------------------------------------------------------------------------

#include "Inverse.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Inverse()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Inverse::Inverse(const std::string &nomme)
   : MathFunction("Inverse", nomme)
{
}


//------------------------------------------------------------------------------
// ~Inverse()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Inverse::~Inverse()
{
}


//------------------------------------------------------------------------------
//  Inverse(const Inverse &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Inverse object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Inverse::Inverse(const Inverse &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Inverse operation.
 *
 * @return clone of the Inverse operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Inverse::Clone() const
{
   return (new Inverse(*this));
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Inverse::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::RMATRIX_TYPE;
   
   leftNode->GetOutputInfo(type, rowCount, colCount);

   if (rowCount == 1 && colCount == 1)
      type = Gmat::REAL_TYPE;

}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Inverse::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Inverse() - Missing input arguments.\n");

   ValidateMatrixInputs(true);

   Integer type, row, col;
   
   GetOutputInfo(type, row, col);
   
   // Only Real type and Matrix are valid. Vector is not a valid input
   if (type == Gmat::RMATRIX_TYPE && row != col)
      throw MathException("Cannot compute inverse of non-square matrix");

   if ((type == Gmat::REAL_TYPE) || (type == Gmat::RMATRIX_TYPE))
      return true;

   return false;
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Inverse of left node
 *
 */
//------------------------------------------------------------------------------
Real Inverse::Evaluate()
{
   return GmatMathUtil::Pow(leftNode->Evaluate(), -1.0);
}

//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the product of left and right nodes
 *
 */
//------------------------------------------------------------------------------
Rmatrix Inverse::MatrixEvaluate()
{
   //=======================================================
   #ifdef DEBUG_EVALUATE
   //=======================================================
   
   Rmatrix rmat = leftNode->MatrixEvaluate();
   MessageInterface::ShowMessage
      ("Inverse::MatrixEvaluate() left node =\n%s\n", rmat.ToString(12).c_str());
   try
   {
      Rmatrix result = rmat.Inverse();
      MessageInterface::ShowMessage
         ("Inverse::MatrixEvaluate() returning\n%s\n", result.ToString(12).c_str());
      return result;
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage
         ("Inverse::MatrixEvaluate() %s for %s\n", be.GetFullMessage().c_str(),
          GetName().c_str());
      throw;
   }
   
   //=======================================================
   #else
   //=======================================================
   
   return (leftNode->MatrixEvaluate()).Inverse();
   
   //=======================================================
   #endif
   //=======================================================
}


