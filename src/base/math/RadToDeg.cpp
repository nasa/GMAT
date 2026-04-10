//$Id$
//------------------------------------------------------------------------------
//                                  RadToDeg
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
 * Implements RadToDeg class.
 */
//------------------------------------------------------------------------------

#include "RadToDeg.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RadToDeg()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
RadToDeg::RadToDeg(const std::string &nomme)
   : MathFunction("RadToDeg", nomme)
{
}


//------------------------------------------------------------------------------
// ~RadToDeg()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RadToDeg::~RadToDeg()
{
}


//------------------------------------------------------------------------------
//  RadToDeg(const RadToDeg &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the RadToDeg object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
RadToDeg::RadToDeg(const RadToDeg &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the RadToDeg operation.
 *
 * @return clone of the RadToDeg operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* RadToDeg::Clone() const
{
   return (new RadToDeg(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void RadToDeg::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   GetScalarOutputInfo(type, rowCount, colCount);
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool RadToDeg::ValidateInputs()
{
   return ValidateScalarInputs();
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the RadToDeg of left node
 */
//------------------------------------------------------------------------------
Real RadToDeg::Evaluate()
{
   return GmatMathUtil::RadToDeg(leftNode->Evaluate());
}

