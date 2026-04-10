//$Id$
//------------------------------------------------------------------------------
//                                  Strcat
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
 * Implements Strcat class for string concatenation.
 */
//------------------------------------------------------------------------------

#include "Strcat.hpp"
#include "MathException.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Strcat()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Strcat::Strcat(const std::string &name)
   : StringFunctionNode("Strcat", name)
{
}


//------------------------------------------------------------------------------
// ~Strcat()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Strcat::~Strcat()
{
}


//------------------------------------------------------------------------------
//  Strcat(const Strcat &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Strcat object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Strcat::Strcat(const Strcat &copy) :
   StringFunctionNode      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Strcat operation.
 *
 * @return clone of the Strcat operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Strcat::Clone() const
{
   return (new Strcat(*this));
}

//------------------------------------------------------------------------------
// std;:string EvaluateString()
//------------------------------------------------------------------------------
/**
 * @return the Strcat of left node
 *
 */
//------------------------------------------------------------------------------
std::string Strcat::EvaluateString()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strcat::EvaluateString() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   std::string result;

   ValidateWrappers();
   
   for (unsigned int i = 0; i < inputArgWrappers.size(); i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s'\n", i, wrapper,
          wrapper ? wrapper->GetDescription().c_str() : "NULL");
      #endif
      
      result = result + wrapper->EvaluateString();
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Strcat::EvaluateString() <%p>'%s' returning '%s'\n", this,
       GetName().c_str(), result.c_str());
   #endif
   return result;
}

