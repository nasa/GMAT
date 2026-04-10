//$Id$
//------------------------------------------------------------------------------
//                                   Mod
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
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.02.29
//
/**
 * Implements Mod function class.
 */
//------------------------------------------------------------------------------

#include "Mod.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//------------------------------------------------------------------------------
//  Mod(std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Mod object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Mod::Mod(const std::string &name)
   : NumericFunctionNode("Mod", name)
{
}

//------------------------------------------------------------------------------
//  ~Mod(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Mod object (destructor).
 */
//------------------------------------------------------------------------------
Mod::~Mod()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  Mod(const Mod &func)
//------------------------------------------------------------------------------
/**
 * Constructs the Mod object (copy constructor).
 * 
 * @param <func> Object that is copied
 */
//------------------------------------------------------------------------------
Mod::Mod(const Mod &func) :
   NumericFunctionNode   (func)
{
}

//------------------------------------------------------------------------------
//  Mod& operator=(const Mod &func)
//------------------------------------------------------------------------------
/**
 * Sets one Mod object to match another (assignment operator).
 * 
 * @param <func> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Mod& Mod::operator=(const Mod &func)
{
   if (this == &func)
      return *this;
   
   NumericFunctionNode::operator=(func);
   return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Mod operation.
 *
 * @return clone of the Mod operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Mod::Clone() const
{
   return (new Mod(*this));
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the result of modulo operation. Mod(a,b) returns the remainder after
 * division of a by b, where a is the dividend and b is the divisor.
 */
//------------------------------------------------------------------------------
Real Mod::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Mod::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   Real result;

   if (inputArgWrappers.size() != 2)
      throw MathException("mod() function requires two input arguments");
   
   for (unsigned int i = 0; i < inputArgWrappers.size(); i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s'\n", i, wrapper,
          wrapper ? wrapper->GetDescription().c_str() : "NULL");
      #endif
      
      if (wrapper == NULL)
         throw MathException("Error evaluating \"" + GetName() + "\"");
   }
   
   Real a = inputArgWrappers[0]->EvaluateReal();
   Real b = inputArgWrappers[1]->EvaluateReal();
   
   result = GmatMathUtil::Mod(a, b);
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Mod::Evaluate() <%p>'%s' returning %.12f\n", this, GetName().c_str(), result);
   #endif
   return result;
}

