//$Id$
//------------------------------------------------------------------------------
//                                  Exp
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
// Created: 2006/04/14
//
/**
 * Implements Exp class.
 */
//------------------------------------------------------------------------------

#include "Exp.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Exp()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Exp::Exp(const std::string &nomme)
   : MathFunction("Exp", nomme)
{
}


//------------------------------------------------------------------------------
// ~Exp()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Exp::~Exp()
{
}


//------------------------------------------------------------------------------
//  Exp(const Exp &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Exp object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Exp::Exp(const Exp &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Exp operation.
 *
 * @return clone of the Exp operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Exp::Clone() const
{
   return (new Exp(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Exp::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
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
bool Exp::ValidateInputs()
{
   return ValidateScalarInputs();
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the exponential value
 *
 */
//------------------------------------------------------------------------------
Real Exp::Evaluate()
{
   return GmatMathUtil::Exp(leftNode->Evaluate());
}

