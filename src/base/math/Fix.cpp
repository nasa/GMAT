//$Id$
//------------------------------------------------------------------------------
//                                  Fix
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
// Author: Linda Jun
// Created: 2016.03.08
//
/**
 * Implements Fix class.
 */
//------------------------------------------------------------------------------

#include "Fix.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Fix()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Fix::Fix(const std::string &nomme)
   : MathFunction("Fix", nomme)
{
}


//------------------------------------------------------------------------------
// ~Fix()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Fix::~Fix()
{
}


//------------------------------------------------------------------------------
//  Fix(const Fix &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Fix object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Fix::Fix(const Fix &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Fix operation.
 *
 * @return clone of the Fix operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Fix::Clone() const
{
   return (new Fix(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Fix::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Fix::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the Fix of left node
 *
 */
//------------------------------------------------------------------------------
Real Fix::Evaluate()
{
   return GmatMathUtil::Fix(leftNode->Evaluate());
}

