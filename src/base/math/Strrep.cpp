//$Id$
//------------------------------------------------------------------------------
//                                  Strrep
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
 * Implements Strrep class for string concatenation.
 */
//------------------------------------------------------------------------------

#include "Strrep.hpp"
#include "ElementWrapper.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Strrep()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Strrep::Strrep(const std::string &name)
   : StringFunctionNode("Strrep", name)
{
}


//------------------------------------------------------------------------------
// ~Strrep()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Strrep::~Strrep()
{
}


//------------------------------------------------------------------------------
//  Strrep(const Strrep &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Strrep object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Strrep::Strrep(const Strrep &copy) :
   StringFunctionNode      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Strrep operation.
 *
 * @return clone of the Strrep operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Strrep::Clone() const
{
   return (new Strrep(*this));
}

//------------------------------------------------------------------------------
// std;:string EvaluateString()
//------------------------------------------------------------------------------
/**
 * modifiedStr = strrep(origStr, oldSubstr, newSubstr)
 * Replaces all occurrences of the string oldSubstr within string origStr with
 * the string newSubstr. Returns modified string.
 *
 */
//------------------------------------------------------------------------------
std::string Strrep::EvaluateString()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strrep::EvaluateString() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   if (inputArgWrappers.size() != 3)
      throw MathException(GetTypeName() + "() function requires three input arguments");

   ValidateWrappers();
   
   std::string str = inputArgWrappers[0]->EvaluateString();
   std::string oldStr = inputArgWrappers[1]->EvaluateString();
   std::string newStr = inputArgWrappers[2]->EvaluateString();
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("   str='%s', oldStr='%s', newStr='%s'\n", str.c_str(), oldStr.c_str(),
       newStr.c_str());
   #endif
   
   std::string result = GmatStringUtil::Replace(str, oldStr, newStr, 0);
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strrep::EvaluateString() <%p>'%s' returning '%s'\n", this,
       GetName().c_str(), result.c_str());
   #endif
   return result;
}

