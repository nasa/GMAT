//$Id$
//------------------------------------------------------------------------------
//                                   StringFunctionNode
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

#include "StringFunctionNode.hpp"
#include "ElementWrapper.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_VALIDATE_INPUT
//#define DEBUG_INPUT_OUTPUT

//------------------------------------------------------------------------------
//  StringFunctionNode(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the StringFunctionNode object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
StringFunctionNode::StringFunctionNode(const std::string &typeStr, const std::string &name) :
   BuiltinFunctionNode    (typeStr, name)
{
   objectTypeNames.push_back("StringFunctionNode");
}

//------------------------------------------------------------------------------
//  ~StringFunctionNode(void)
//------------------------------------------------------------------------------
/**
 * Destroys the StringFunctionNode object (destructor).
 */
//------------------------------------------------------------------------------
StringFunctionNode::~StringFunctionNode()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  StringFunctionNode(const StringFunctionNode &sf)
//------------------------------------------------------------------------------
/**
 * Constructs the StringFunctionNode object (copy constructor).
 * 
 * @param <sf> Object that is copied
 */
//------------------------------------------------------------------------------
StringFunctionNode::StringFunctionNode(const StringFunctionNode &sf) :
   BuiltinFunctionNode   (sf)
{
}

//------------------------------------------------------------------------------
//  StringFunctionNode& operator=(const StringFunctionNode &sf)
//------------------------------------------------------------------------------
/**
 * Sets one StringFunctionNode object to match another (assignment operator).
 * 
 * @param <sf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
StringFunctionNode& StringFunctionNode::operator=(const StringFunctionNode &sf)
{
   if (this == &sf)
      return *this;
   
   GmatBase::operator=(sf);
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
bool StringFunctionNode::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("StringFunctionNode::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   bool retval = true;
   
   // Validate inputs
   std::string noBlankDesc = GmatStringUtil::RemoveAllBlanks(desc);
   if (noBlankDesc == "")
      throw MathException("There are no Input to " + GetTypeName() + "() in " +
                          desc);
   
   StringArray items = GmatStringUtil::DecomposeBy(desc, "(");
   if (items.size() == 0)
      throw MathException("Invalid calling syntax to " + GetTypeName() + "() in " +
                          desc);
   
   #ifdef DEBUG_VALIDATE_INPUT
   for (unsigned int i = 0; i < items.size(); i++)
      MessageInterface::ShowMessage("   items[%d] = '%s'\n", i, items[i].c_str());
   #endif
   
   // Check for closing parenthesis
   if (!GmatStringUtil::EndsWith(items[1], ")"))
      throw MathException("Invalid calling syntax to " + GetTypeName() + "() in " +
                          desc);
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("StringFunctionNode::ValidateInputs() <%p><%s> returning %s\n",
       this, GetTypeName().c_str(), retval ? "true" : "false");
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void StringFunctionNode::GetOutputInfo(Integer &type, Integer &rowCount,
                                   Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("StringFunctionNode::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::STRING_TYPE;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("StringFunctionNode::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}

//------------------------------------------------------------------------------
// void ValidateWrappers()
//------------------------------------------------------------------------------
void StringFunctionNode::ValidateWrappers()
{
   // Validate wrappers are not NULL
   BuiltinFunctionNode::ValidateWrappers();
   
   // Wrappers are not NUll, now validate that first input is string
   for (unsigned int i = 0; i < inputArgWrappers.size(); i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s', dataType=%d\n", i, wrapper,
          wrapper->GetDescription().c_str(), wrapper->GetDataType());
      #endif
      
      if (wrapper->GetDataType() != Gmat::STRING_TYPE)
         throw MathException("Error evaluating \"" + GetName() + ". "
                             "Expecting inputs of String type");
      
      break;
   }
}

