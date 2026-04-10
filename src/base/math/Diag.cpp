//$Id$
//------------------------------------------------------------------------------
//                                   Diag
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
// Created: 2016.03.09
//
/**
 * Implements Diag function class.
 */
//------------------------------------------------------------------------------

#include "Diag.hpp"
#include "ElementWrapper.hpp"
#include "Rmatrix33.hpp"
#include "StringUtil.hpp"          // for ParseFunctionCall()
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE
//#define DEBUG_INPUT_OUTPUT

//------------------------------------------------------------------------------
//  Diag(std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Diag object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Diag::Diag(const std::string &name)
   : NumericFunctionNode("Diag", name),
     numRows(0),
     numCols(0)
{
}

//------------------------------------------------------------------------------
//  ~Diag(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Diag object (destructor).
 */
//------------------------------------------------------------------------------
Diag::~Diag()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  Diag(const Diag &func)
//------------------------------------------------------------------------------
/**
 * Constructs the Diag object (copy constructor).
 * 
 * @param <func> Object that is copied
 */
//------------------------------------------------------------------------------
Diag::Diag(const Diag &func) :
   NumericFunctionNode (func),
   numRows (func.numRows),
   numCols (func.numCols)
{
}

//------------------------------------------------------------------------------
//  Diag& operator=(const Diag &func)
//------------------------------------------------------------------------------
/**
 * Sets one Diag object to match another (assignment operator).
 * 
 * @param <func> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Diag& Diag::operator=(const Diag &func)
{
   if (this == &func)
      return *this;
   
   NumericFunctionNode::operator=(func);
   numRows = func.numRows;
   numCols = func.numCols;
   return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Diag operation.
 *
 * @return clone of the Diag operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Diag::Clone() const
{
   return (new Diag(*this));
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Diag::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Diag::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   Integer type1 = Gmat::RMATRIX_TYPE;
   bool retval = true;   
   
   // Is there any validation to do?
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Diag::ValidateInputs() <%p><%s> returning %s, "
       "type=%d\n", this, GetTypeName().c_str(), retval ? "true" : "false", type1);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Diag::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Diag::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   // Since inputNames are built in the constructor, check for the size
   if (inputNames.empty())
      throw MathException("The math function " + GetTypeName() + "() requires input arguments");
   
   type = Gmat::RMATRIX_TYPE;
   
   rowCount = inputNames.size();
   colCount = rowCount;
   numRows = rowCount;
   numCols = colCount;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Diag::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the diagonal matarix.
 * diag([1 2 3]) will generate:
 *    1     0     0
 *    0     2     0
 *    0     0     3
 *
 */
//------------------------------------------------------------------------------
Rmatrix Diag::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Diag::Evaluate() <%p>'%s' entered, numRows=%d, numCols=%d\n",
       this, GetName().c_str(), numRows, numCols);
   #endif
   
   Rmatrix result(numRows, numCols);

   for (unsigned int i = 0; i < inputArgWrappers.size(); i++)
   {
      ElementWrapper *wrapper = inputArgWrappers[i];
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   inputArgWrappers[%d] = <%p>, desc = '%s'\n", i, wrapper,
          wrapper ? wrapper->GetDescription().c_str() : "NULL");
      #endif
      
      if (wrapper == NULL)
         throw MathException("Error evaluating \"" + GetName());
      
      for (int j = 0; j < numCols; j++)
      {
         if (i == j)
            result(i,j) = wrapper->EvaluateReal();
      }
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Diag::Evaluate() <%p>'%s' returning:\n%s\n", this, GetName().c_str(),
       result.ToString(12, 1, false, "   ").c_str());
   #endif
   return result;
}

