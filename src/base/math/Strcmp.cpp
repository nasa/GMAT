//$Id$
//------------------------------------------------------------------------------
//                                  Strcmp
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
// Created: 2016.02.17
//
/**
 * Implements Strcmp class for string concatenation.
 */
//------------------------------------------------------------------------------

#include "Strcmp.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Strcmp()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Strcmp::Strcmp(const std::string &name)
   : StringFunctionNode("Strcmp", name)
{
}


//------------------------------------------------------------------------------
// ~Strcmp()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Strcmp::~Strcmp()
{
}


//------------------------------------------------------------------------------
//  Strcmp(const Strcmp &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Strcmp object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Strcmp::Strcmp(const Strcmp &copy) :
   StringFunctionNode      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Strcmp operation.
 *
 * @return clone of the Strcmp operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Strcmp::Clone() const
{
   return (new Strcmp(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Strcmp::GetOutputInfo(Integer &type, Integer &rowCount,
                           Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Strcmp::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Strcmp::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * strcmp(s1, s2)
 * Compares strings s1 and s2 and returns 1 (true) if the two are identical.
 * Otherwise, returns 0 (false).
 *
 */
//------------------------------------------------------------------------------
Real Strcmp::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strcmp::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   if (inputArgWrappers.size() != 2)
      throw MathException(GetTypeName() + "() function requires two input arguments");

   ValidateWrappers();
   
   std::string s1 = inputArgWrappers[0]->EvaluateString();
   std::string s2 = inputArgWrappers[1]->EvaluateString();
   
   Real result;
   
   if (s1 == s2)
      result = 1.0;
   else
      result = 0.0;
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strcmp::Evaluate() <%p>'%s' returning %f\n", this,
       GetName().c_str(), result);
   #endif
   return result;
}

