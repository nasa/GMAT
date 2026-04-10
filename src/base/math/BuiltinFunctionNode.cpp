//$Id$
//------------------------------------------------------------------------------
//                                   BuiltinFunctionNode
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
 * Implements the GMAT builtin function class.
 */
//------------------------------------------------------------------------------

#include "BuiltinFunctionNode.hpp"
#include "MathException.hpp"
#include "TextParser.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_INSTANCE
//#define DEBUG_WRAPPERS

//------------------------------------------------------------------------------
//  BuiltinFunctionNode(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinFunctionNode object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
BuiltinFunctionNode::BuiltinFunctionNode(const std::string &typeStr, const std::string &name) :
   MathNode    (typeStr, name)
{
   desc = name;
   isFunction = true;
   objectTypeNames.push_back("BuiltinFunctionNode");
   
   #ifdef DEBUG_INSTANCE
   MessageInterface::ShowMessage
      ("BuiltinFunctionNode() constructor entered, typeStr='%s', name='%s'\n",
       typeStr.c_str(), name.c_str());
   #endif
   
   // Parse input arguments
   StringArray items = GmatStringUtil::DecomposeBy(desc, "(");
   if (items.size() == 2)
   {
      std::string args = GmatStringUtil::RemoveLastString(items[1], ")");
      // Trim both for correct parsing ex) MyFunc( [a b c])
      args = GmatStringUtil::Trim(args);
      
      // Check if input arg is in the vector form [1 2 3]
      if (GmatStringUtil::IsEnclosedWithBrackets(args))
      {
         StringArray argList = GmatStringUtil::SeparateBrackets(args, "[]", " ,;", true);
         for (unsigned int i = 0; i < argList.size(); i++)
         {
            #ifdef DEBUG_INSTANCE
            MessageInterface::ShowMessage
               ("   argList[%d] = '%s'\n", i, argList[i].c_str());
            #endif
            inputNames.push_back(argList[i]);
            inputArgWrappers.push_back(NULL);
         }
      }
      else
      {
         StringArray argList = GmatStringUtil::SeparateByComma(args);
         
         // Add to wrapper object names
         for (unsigned int i = 0; i < argList.size(); i++)
         {
            std::string arg = argList[i];
            #ifdef DEBUG_INSTANCE
            MessageInterface::ShowMessage("   argList[%d] = '%s'\n", i, argList[i].c_str());
            #endif
            
            inputNames.push_back(argList[i]);
            inputArgWrappers.push_back(NULL);
         }
      }
   }
   #ifdef DEBUG_INSTANCE
   MessageInterface::ShowMessage
      ("BuiltinFunctionNode() constructor leaving, typeStr='%s', name='%s'\n",
       typeStr.c_str(), name.c_str());
   #endif
}

//------------------------------------------------------------------------------
//  ~BuiltinFunctionNode(void)
//------------------------------------------------------------------------------
/**
 * Destroys the BuiltinFunctionNode object (destructor).
 */
//------------------------------------------------------------------------------
BuiltinFunctionNode::~BuiltinFunctionNode()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  BuiltinFunctionNode(const BuiltinFunctionNode &func)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinFunctionNode object (copy constructor).
 * 
 * @param <func> Object that is copied
 */
//------------------------------------------------------------------------------
BuiltinFunctionNode::BuiltinFunctionNode(const BuiltinFunctionNode &func) :
   MathNode   (func)
{
   desc = func.desc;
}

//------------------------------------------------------------------------------
//  BuiltinFunctionNode& operator=(const BuiltinFunctionNode &func)
//------------------------------------------------------------------------------
/**
 * Sets one BuiltinFunctionNode object to match another (assignment operator).
 * 
 * @param <func> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
BuiltinFunctionNode& BuiltinFunctionNode::operator=(const BuiltinFunctionNode &func)
{
   if (this == &func)
      return *this;
   
   GmatBase::operator=(func);
   desc = func.desc;
   return *this;
}


//------------------------------------------------------------------------------
// void SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
//------------------------------------------------------------------------------
void BuiltinFunctionNode::SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
{
   theWrapperMap = wrapperMap;
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("BuiltinFunctionNode::SetMathWrappers() entered, theWrapperMap=<%p>, Node=%s\n",
       theWrapperMap, GetName().c_str());
   #endif
   
   if (theWrapperMap == NULL)
   {
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("MathElement::RenameRefObject() theWrapperMap is NULL, returning true\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("   now checking %d objects in theWrapperMap\n", theWrapperMap->size());
   #endif
   
   // Rename wrapper objects
   std::map<std::string, ElementWrapper *> tempMap;
   std::map<std::string, ElementWrapper *>::iterator ewi;
   bool needSwap = false;
   
   for (ewi = theWrapperMap->begin(); ewi != theWrapperMap->end(); ++ewi)
   {
      std::string name = ewi->first;
      ElementWrapper *wrapper = ewi->second;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage("   name=<%s>, wrapper=%p\n", name.c_str(), wrapper);
      #endif
      
      for (unsigned int i = 0; i < inputNames.size(); i++)
      {
         if (name == inputNames[i])
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Setting wrapper<%p> to '%s'\n", wrapper, name.c_str());
            #endif
            inputArgWrappers[i] = wrapper;
         }
      }
   }
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real BuiltinFunctionNode::Evaluate()
{
   throw MathException(GetTypeName() + " cannot return Real");
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix BuiltinFunctionNode::MatrixEvaluate()
{
   throw MathException(GetTypeName() + " cannot return Matrix");
}


//------------------------------------------------------------------------------
// bool SetChildren(MathNode *leftChild, MathNode *rightChild)()
//------------------------------------------------------------------------------
bool BuiltinFunctionNode::SetChildren(MathNode *leftChild, MathNode *rightChild)
{
   throw MathException("SetChildren() is not valid for BuiltinFunctionNode");
}


//------------------------------------------------------------------------------
// bool GetLeft()
//------------------------------------------------------------------------------
MathNode* BuiltinFunctionNode::GetLeft()
{
   return NULL;
}


//------------------------------------------------------------------------------
// bool GetRight()
//------------------------------------------------------------------------------
MathNode* BuiltinFunctionNode::GetRight()
{
   return NULL;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object.
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& BuiltinFunctionNode::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::GetRefObjectNameArray() '%s' entered, type=%d\n",
       GetName().c_str(), type);
   MessageInterface::ShowMessage
      ("There are %d wrapper object names:\n", inputNames.size());
   for (UnsignedInt i = 0; i < inputNames.size(); i++)
      MessageInterface::ShowMessage("   [%d] %s\n", i, inputNames[i].c_str());
   #endif
   
   if (Gmat::UNKNOWN_OBJECT)
   {      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("MathElement::GetRefObjectNameArray() returning %d wrapper object names:\n",
          inputNames.size());
      for (UnsignedInt i = 0; i < inputNames.size(); i++)
         MessageInterface::ShowMessage("   [%d] %s\n", i, inputNames[i].c_str());
      #endif
      
      return inputNames;
   }
   
   return MathNode::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
/*
 * Returns wrapper object names used for creating wrappers in Validator and
 * used to determine if an object can be deleted.
 *
 * @param completeSet   return names that would not ordinarily be returned
 *
 * @return list of names of wrappers
 *
 */
//------------------------------------------------------------------------------
const StringArray& BuiltinFunctionNode::GetWrapperObjectNameArray(bool completeSet)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("BuiltinFunctionNode::GetWrapperObjectNameArray() returning %d wrapper names\n",
       inputNames.size());
   #endif
   return inputNames;
}

//=================================
// protected
//=================================

//------------------------------------------------------------------------------
// void ValidateWrappers()
//------------------------------------------------------------------------------
void BuiltinFunctionNode::ValidateWrappers()
{
   for (unsigned int i = 0; i < inputArgWrappers.size(); i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s'\n", i, wrapper,
          wrapper ? wrapper->GetDescription().c_str() : "NULL");
      #endif
      
      if (wrapper == NULL)
         throw MathException("Error evaluating \"" + GetName() + "() function");
   }
}

