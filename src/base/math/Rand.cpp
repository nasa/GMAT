//------------------------------------------------------------------------------
//                                  Rand
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
// Created: 2016.04.15
//
/**
 * Implements Rand class.
 */
//------------------------------------------------------------------------------

#include "Rand.hpp"
#include "RealUtilities.hpp"       // for Rand()
#include "MessageInterface.hpp"

//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rand()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Rand::Rand(const std::string &nomme)
   : MathFunction("Rand", nomme)
{
}


//------------------------------------------------------------------------------
// ~Rand()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Rand::~Rand()
{
}


//------------------------------------------------------------------------------
//  Rand(const Rand &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Rand object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Rand::Rand(const Rand &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Rand operation.
 *
 * @return clone of the Rand operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Rand::Clone() const
{
   return (new Rand(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Rand::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Rand::GetOutputInfo() <%p><%s><%s> entered\n", this, GetTypeName().c_str(),
       GetName().c_str());
   MessageInterface::ShowMessage
      ("   leftNode=<%p><%s>, rightNode=<%p><%s>\n",
       leftNode,  leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       rightNode, rightNode ? rightNode->GetTypeName().c_str() : "NULL");
   #endif
   
   Integer type1, row1, col1; // Left node
   type = Gmat::RMATRIX_TYPE;
   
   // If left node is empty, assume input value is 1
   if (!leftNode)
   {
      rowCount = 1;
      colCount = 1;
   }
   else
   {
      // Evaluate left node to set NxN matrix for output
      Integer outDim = GetOutputDimension();
      rowCount = outDim;
      colCount = outDim;
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Rand::GetOutputInfo() returning type=%d, rowCount=%d, colCount=%d\n",
       type, rowCount, colCount);
   #endif
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Rand::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("Rand::ValidateInputs() leftNode=<%p><%s>'%s'\n", leftNode,
       leftNode ? leftNode->GetTypeName().c_str() : "NULL",
       leftNode ? leftNode->GetName().c_str() : "NULL");
   #endif
   
   if (leftNode == NULL)
   {
      // Empty left node is allowed for rand() function. It assumes rand(1) which
      // will return Real number.
   }
   else
   {
      // Get the type(Real or Matrix), # rows and # columns of the left node
      leftNode->GetOutputInfo(type1, row1, col1);
      
      if ((type1 != Gmat::REAL_TYPE) && (type1 != Gmat::RMATRIX_TYPE))
      {
         std::string errmsg = "Invalid operand type (";
         errmsg += PARAM_TYPE_STRING[type1] + ") for rand operator.\n";
         throw MathException(errmsg);
      }
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage("Rand::ValidateInputs() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real Rand::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Rand::Evaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   Integer outDim = GetOutputDimension();
   if (outDim > 1)
   {
      throw MathException("Rand() Cannot evaluate \"" + GetName() + "\". "
                          "Left-hand-side of rand function is not an Array\n");
   }
   
   Real result = GmatMathUtil::Rand(0.0, 1.0);
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Rand::Evaluate() <%p>'%s' returning: %.12f\n", this, GetName().c_str(), result);
   #endif
   
   return result;
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
/**
 * @return Random number of uniform distribution
 *
 */
//------------------------------------------------------------------------------
Rmatrix Rand::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Rand::MatrixEvaluate() '%s' entered\n", GetName().c_str());
   #endif
   
   Integer outDim = GetOutputDimension();
   Rmatrix result(outDim, outDim);
   
   if (outDim == 1)
   {
      result(0,0) = GmatMathUtil::Rand(0.0, 1.0);
   }
   else
   {
      for (int i = 0; i < outDim; i++)
         for (int j = 0; j < outDim; j++)
            result(i,j) = GmatMathUtil::Rand(0.0, 1.0);
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("Rand::MatrixEvaluate() <%p>'%s' returning:\n%s\n", this, GetName().c_str(),
       result.ToString(12, 1, false, "   ").c_str());
   #endif
   
   return result;
}


//------------------------------------------------------------------------------
// Integer GetOutputDimension()
//------------------------------------------------------------------------------
/**
 * Evaluates left node and checks if value is a whole number. This will check
 * if input to rand() function is a whole number.
 */
//------------------------------------------------------------------------------
Integer Rand::GetOutputDimension()
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage("Rand::GetOutputDimension() entered\n");
   #endif
   Integer type, rowCount, colCount;
   Real rval = 0;
   Integer outDimension = 0;
   
   if (leftNode)
   {
      leftNode->GetOutputInfo(type, rowCount, colCount);
      
      if(type == Gmat::RMATRIX_TYPE)
      {
         // Allow 1x1 matrix
         if (rowCount == 1 && colCount == 1)
         {
            Rmatrix rmat = leftNode->MatrixEvaluate();
            rval = rmat(0,0);
         }
         else
            throw MathException("Rand() Cannot evaluate \"" + GetName() + "\"\n");
      }
      else
      {
         rval = leftNode->Evaluate();
      }
   }
   else
   {
      #ifdef DEBUG_INPUT_OUTPUT
      MessageInterface::ShowMessage("   Rand() has no input so value of 1 is assumed\n");
      #endif
      rval = 1;
   }
   
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage("   rval = %.12f\n", rval);
   #endif
   
   // Check if input is a positive whole number
   if ((rval > 0.0) &&
       (fabs(rval - round(rval)) < 0.000001))
   {
         outDimension = (Integer)rval;
   }
   else
   {
      throw MathException("Rand() Cannot evaluate \"" + GetName() + "\". "
                          "It expects positive whole number\n");
   }
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage("Rand::GetOutputDimension() returning %d\n", outDimension);
   #endif
   return outDimension;
}
