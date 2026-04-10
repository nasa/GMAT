//$Id$
//------------------------------------------------------------------------------
//                                   NumericFunctionNode
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
// Created: 2016.02.17
//
/**
 * Implements the string functions class.
 */
//------------------------------------------------------------------------------

#include "NumericFunctionNode.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INSTANCE
//#define DEBUG_VALIDATE_INPUT
//#define DEBUG_INPUT_OUTPUT

//------------------------------------------------------------------------------
//  NumericFunctionNode(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the NumericFunctionNode object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
NumericFunctionNode::NumericFunctionNode(const std::string &typeStr, const std::string &name) :
   BuiltinFunctionNode    (typeStr, name)
{
   objectTypeNames.push_back("NumericFunctionNode");
   
}

//------------------------------------------------------------------------------
//  ~NumericFunctionNode(void)
//------------------------------------------------------------------------------
/**
 * Destroys the NumericFunctionNode object (destructor).
 */
//------------------------------------------------------------------------------
NumericFunctionNode::~NumericFunctionNode()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  NumericFunctionNode(const NumericFunctionNode &nfn)
//------------------------------------------------------------------------------
/**
 * Constructs the NumericFunctionNode object (copy constructor).
 * 
 * @param <nfn> Object that is copied
 */
//------------------------------------------------------------------------------
NumericFunctionNode::NumericFunctionNode(const NumericFunctionNode &nfn) :
   BuiltinFunctionNode   (nfn)
{
}

//------------------------------------------------------------------------------
//  NumericFunctionNode& operator=(const NumericFunctionNode &nfn)
//------------------------------------------------------------------------------
/**
 * Sets one NumericFunctionNode object to match another (assignment operator).
 * 
 * @param <nfn> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
NumericFunctionNode& NumericFunctionNode::operator=(const NumericFunctionNode &nfn)
{
   if (this == &nfn)
      return *this;
   
   GmatBase::operator=(nfn);
   return *this;
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool NumericFunctionNode::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("NumericFunctionNode::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   Integer type1 = Gmat::REAL_TYPE;
   Integer row1 = 1;
   Integer col1 = 1;
   bool retval = true;   
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("NumericFunctionNode::ValidateInputs() <%p><%s> returning %s, "
       "type=%d, row=%d, col=%d\n", this, GetTypeName().c_str(),
       retval ? "true" : "false", type1, row1, col1);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void NumericFunctionNode::GetOutputInfo(Integer &type, Integer &rowCount,
                                   Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("NumericFunctionNode::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::REAL_TYPE;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("NumericFunctionNode::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}

