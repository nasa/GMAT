//$Id$
//------------------------------------------------------------------------------
//                                  Atan2
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
 * Implements Atan2 class.
 */
//------------------------------------------------------------------------------

#include "Atan2.hpp"
#include <math.h>          // for atan2(double y, double x)
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Atan2()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Atan2::Atan2(const std::string &nomme)
   : MathFunction("Atan2", nomme)
{
}


//------------------------------------------------------------------------------
// ~Atan2()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Atan2::~Atan2()
{
}


//------------------------------------------------------------------------------
//  Atan2(const Atan2 &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Atan2 object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Atan2::Atan2(const Atan2 &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Atan2 operation.
 *
 * @return clone of the Atan2 operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Atan2::Clone() const
{
   return (new Atan2(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Atan2::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
   
   // Input validation is done in ValidateInputs() so this code is commented out
   // LOJ: 2012.09.21
   /*
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if ((type1 != Gmat::REAL_TYPE) && (type2 != Gmat::REAL_TYPE))
      throw MathException("Left and Right is not scalar, so cannot do Atan2().\n");    
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
   }
   */
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Atan2::ValidateInputs()
{
   if (leftNode == NULL)
      throw MathException("Atan2() - Missing input arguments");
   
   if (rightNode == NULL)
      throw MathException("Atan2() - Not enough input arguments");
   
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return returns the arc tangent of right node / left node 
 * in the range [-, ] radians
 *
 */
//------------------------------------------------------------------------------
Real Atan2::Evaluate()
{
   return atan2(leftNode->Evaluate(), rightNode->Evaluate());
}
