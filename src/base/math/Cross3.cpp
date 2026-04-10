//$Id$
//------------------------------------------------------------------------------
//                                   Cross3
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
// Created: 2016.03.08
//
/**
 * Implements Cross3 function class.
 */
//------------------------------------------------------------------------------

#include "Cross3.hpp"
#include "ElementWrapper.hpp"
#include "Rvector3.hpp"             // for friend function Cross()
#include "MessageInterface.hpp"

//#define DEBUG_EVALUATE

//------------------------------------------------------------------------------
//  Cross3(std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Cross3 object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Cross3::Cross3(const std::string &name)
   : NumericFunctionNode("Cross3", name)
{
}

//------------------------------------------------------------------------------
//  ~Cross3(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Cross3 object (destructor).
 */
//------------------------------------------------------------------------------
Cross3::~Cross3()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  Cross3(const Cross3 &func)
//------------------------------------------------------------------------------
/**
 * Constructs the Cross3 object (copy constructor).
 * 
 * @param <func> Object that is copied
 */
//------------------------------------------------------------------------------
Cross3::Cross3(const Cross3 &func) :
   NumericFunctionNode   (func)
{
}

//------------------------------------------------------------------------------
//  Cross3& operator=(const Cross3 &func)
//------------------------------------------------------------------------------
/**
 * Sets one Cross3 object to match another (assignment operator).
 * 
 * @param <func> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Cross3& Cross3::operator=(const Cross3 &func)
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
 * Clone of the Cross3 operation.
 *
 * @return clone of the Cross3 operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Cross3::Clone() const
{
   return (new Cross3(*this));
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Cross3::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Cross3::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   Integer type1 = Gmat::RMATRIX_TYPE;
   Integer row1 = 1;
   Integer col1 = 3;
   bool retval = true;   
   
   // Any more validation to do?
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("Cross3::ValidateInputs() <%p><%s> returning %s, "
       "type=%d, row=%d, col=%d\n", this, GetTypeName().c_str(),
       retval ? "true" : "false", type1, row1, col1);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Cross3::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Cross3::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::RMATRIX_TYPE;
   // Output can be 1x3 or 3x1 matrix
   rowCount = 1;
   colCount = 3;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Cross3::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return the cross product of two 3-element vectors
 *
 * cross.gmf
 * function [crossProd] = cross(vec1,vec2)
 * Create Array crossProd[3,1] vec1[3,1] vec2[3,1]
 * BeginMissionSequence
 * crossProd(1,1) =   vec1(2,1)*vec2(3,1) - vec2(2,1)*vec1(3,1)
 * crossProd(2,1) = -(vec1(1,1)*vec2(3,1) - vec2(1,1)*vec1(3,1))
 * crossProd(3,1) =   vec1(1,1)*vec2(2,1) - vec2(1,1)*vec1(2,1)
 *
 */
//------------------------------------------------------------------------------
Rmatrix Cross3::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Cross3::Evaluate() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   Rmatrix result(1,3);
   
   if (inputArgWrappers.size() != 2)
      throw MathException("cross() function requires two input arguments");
   
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
   
   Rmatrix mat1 = inputArgWrappers[0]->EvaluateArray();
   Rmatrix mat2 = inputArgWrappers[1]->EvaluateArray();
   Rvector3 vec1, vec2, vec3;
   Integer row1 = mat1.GetNumRows();
   Integer col1 = mat1.GetNumColumns();
   Integer row2 = mat2.GetNumRows();
   Integer col2 = mat2.GetNumColumns();
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("   row1=%d, col1=%d, row2=%d, col2=%d\n", row1, col1, row2, col2);
   #endif
   
   if (((row1 == 3 && col1 == 1) || (row1 == 1 && col1 == 3)) &&
       ((row2 == 3 && col2 == 1) || (row2 == 1 && col2 == 3)))
   {
      if (row1 == 3)
         vec1.Set(mat1(0,0), mat1(1,0), mat1(2,0));
      else if (col1 == 3)
         vec1.Set(mat1(0,0), mat1(0,1), mat1(0,2));
      if (row2 == 3)
         vec2.Set(mat2(0,0), mat2(1,0), mat2(2,0));
      else if (col2 == 3)
         vec2.Set(mat2(0,0), mat2(0,1), mat2(0,2));
      
      // Cross is friend function in Rvector3
      vec3 = Cross(vec1, vec2);
      
      result.SetElement(0, 0, vec3(0));
      result.SetElement(0, 1, vec3(1));
      result.SetElement(0, 2, vec3(2));
   }
   else
   {
      throw MathException("Error evaluating \"" + GetName() +
                          "\"; Cross product requires two 3-element vectors");
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Cross3::Evaluate() <%p>'%s' returning:   [%s]\n", this, GetName().c_str(),
       result.ToString(12, 1, true).c_str());
   #endif
   return result;
}

