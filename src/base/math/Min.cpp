//$Id$
//------------------------------------------------------------------------------
//                                   Min
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
 * Implements Min function class.
 */
//------------------------------------------------------------------------------

#include "Min.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//------------------------------------------------------------------------------
//  Min(std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Min object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Min::Min(const std::string &name)
   : NumericFunctionNode("Min", name)
{
}

//------------------------------------------------------------------------------
//  ~Min(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Min object (destructor).
 */
//------------------------------------------------------------------------------
Min::~Min()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  Min(const Min &func)
//------------------------------------------------------------------------------
/**
 * Constructs the Min object (copy constructor).
 * 
 * @param <func> Object that is copied
 */
//------------------------------------------------------------------------------
Min::Min(const Min &func) :
   NumericFunctionNode   (func)
{
}

//------------------------------------------------------------------------------
//  Min& operator=(const Min &func)
//------------------------------------------------------------------------------
/**
 * Sets one Min object to match another (assignment operator).
 * 
 * @param <func> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Min& Min::operator=(const Min &func)
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
 * Clone of the Min operation.
 *
 * @return clone of the Min operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Min::Clone() const
{
   return (new Min(*this));
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the minimum number.
 */
//------------------------------------------------------------------------------
Real Min::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Min::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   Real result;
   
   unsigned int inputSz = inputArgWrappers.size();
   
   if (inputSz < 1)
         throw MathException("Error evaluating \"" + GetName());
   
   
   for (unsigned int i = 0; i < inputSz; i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s'\n", i, wrapper,
          wrapper ? wrapper->GetDescription().c_str() : "NULL");
      #endif
      
      if (wrapper == NULL)
         throw MathException("Error evaluating \"" + GetName());

      if (i == 0)
         result = wrapper->EvaluateReal();
      else
      {
         Real newVal = wrapper->EvaluateReal();
         if (newVal < result)
            result = newVal;
      }
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Min::Evaluate() <%p>'%s' returning %.12f\n", this, GetName().c_str(), result);
   #endif
   return result;
}

