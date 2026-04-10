//$Id$
//------------------------------------------------------------------------------
//                                  Strfind
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
 * Implements Strfind class for string concatenation.
 */
//------------------------------------------------------------------------------

#include "Strfind.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Strfind()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Strfind::Strfind(const std::string &name)
   : StringFunctionNode("Strfind", name)
{
}


//------------------------------------------------------------------------------
// ~Strfind()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Strfind::~Strfind()
{
}


//------------------------------------------------------------------------------
//  Strfind(const Strfind &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Strfind object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Strfind::Strfind(const Strfind &copy) :
   StringFunctionNode      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Strfind operation.
 *
 * @return clone of the Strfind operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Strfind::Clone() const
{
   return (new Strfind(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Strfind::GetOutputInfo(Integer &type, Integer &rowCount,
                            Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Strfind::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Strfind::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}

//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 *  k = strfind(str, pattern)
 *  Searches str for first occurrences of pattern and returns starting index
 *  of pattern. If pattern is not found it returns -1.
 *
 */
//------------------------------------------------------------------------------
Real Strfind::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strfind::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   if (inputArgWrappers.size() != 2)
      throw MathException(GetTypeName() + "() requires two input arguments");
   
   ValidateWrappers();
   
   std::string str = inputArgWrappers[0]->EvaluateString();
   std::string pattern = inputArgWrappers[1]->EvaluateString();
   
   std::string::size_type patternPos = str.find(pattern);
   Real result;
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("   str = '%s', pattern = '%s'\n", str.c_str(), pattern.c_str());
   MessageInterface::ShowMessage("   patternPos = %u\n", patternPos);
   #endif
   
   // If pattern found, set starting index to result
   if (patternPos != str.npos)
   {
      result = Real(patternPos);
      // Fix index to start from 1
      result = result + 1;
   }
   else
   {
      result = -1;
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strfind::Evaluate() <%p>'%s' returning %f\n", this,
       GetName().c_str(), result);
   #endif
   return result;
}

