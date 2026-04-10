//------------------------------------------------------------------------------
//                           QuaternionProduct
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract, Task Order 05
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Aug 16, 2024
/**
 *  Product of two quaternions.
 */
 //------------------------------------------------------------------------------

#include "QuaternionProduct.hpp"
#include "FunctionException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ArrayWrapper.hpp"

//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
//  QuaternionProduct(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the QuaternionProduct object (default constructor).
 *
 * @param typeStr String text identifying the object type
 * @param name    Name for the object
 */
 //------------------------------------------------------------------------------
QuaternionProduct::QuaternionProduct(const std::string& typeStr, const std::string& name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("QuaternionProduct");

   // Build input and output arrays. Function interface is:
   // QuaternionProduct(time)

   // Add dummy input names
   inputNames.push_back("__QuaternionProd_input_1_quaternion1__");
   inputArgMap.insert(std::make_pair("__QuaternionProd_input_1_quaternion1__", (ElementWrapper*)NULL));
   inputNames.push_back("__QuaternionProd_input_2_quaternion2__");
   inputArgMap.insert(std::make_pair("__QuaternionProd_input_2_quaternion2__", (ElementWrapper*)NULL));

   // Add dummy output names
   outputNames.push_back("__QuaternionProd_output_1_product__");
   outputArgMap.insert(std::make_pair("__QuaternionProd_output_1_product__", (ElementWrapper*)NULL));
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);
}

//------------------------------------------------------------------------------
//  ~QuaternionProduct(void)
//------------------------------------------------------------------------------
/**
 * Destroys the QuaternionProduct object (destructor).
 */
 //------------------------------------------------------------------------------
QuaternionProduct::~QuaternionProduct()
{
}


//------------------------------------------------------------------------------
//  QuaternionProduct(const QuaternionProduct &f)
//------------------------------------------------------------------------------
/**
 * Constructs the QuaternionProduct object (copy constructor).
 *
 * @param f Object that is copied
 */
 //------------------------------------------------------------------------------
QuaternionProduct::QuaternionProduct(const QuaternionProduct& f) :
   BuiltinGmatFunction(f)
{

}


//------------------------------------------------------------------------------
//  QuaternionProduct& operator=(const QuaternionProduct &f)
//------------------------------------------------------------------------------
/**
 * Sets one QuaternionProduct object to match another (assignment operator).
 *
 * @param f The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
 //------------------------------------------------------------------------------
QuaternionProduct& QuaternionProduct::operator=(const QuaternionProduct& f)
{
   if (this == &f)
      return *this;

   BuiltinGmatFunction::operator=(f);

   return *this;
}


//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray QuaternionProduct::GetOutputTypes(IntegerArray& rowCounts,
   IntegerArray& colCounts) const
{
   rowCounts = outputRowCounts;
   colCounts = outputColCounts;
   return outputWrapperTypes;
}

//------------------------------------------------------------------------------
// virtual void SetOutputTypes(WrapperTypeArray &outputTypes,
//                             IntegerArray &rowCounts, IntegerArray &colCounts)
//------------------------------------------------------------------------------
/*
 * Sets function output types. This method is called when parsing the function
 * file from the Interpreter.
 */
//------------------------------------------------------------------------------
void QuaternionProduct::SetOutputTypes(WrapperTypeArray& outputTypes,
   IntegerArray& rowCounts, 
   IntegerArray& colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
      MessageInterface::ShowMessage
      ("QuaternionProduct::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for QuaternionProduct
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool QuaternionProduct::Initialize(ObjectInitializer* objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
      ("QuaternionProduct::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
      ("QuaternionProduct::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool QuaternionProduct::Execute(ObjectInitializer* objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 2 inputs
   if (inputArgMap.size() != 2)
   {
      #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
         ("QuaternionProduct::Execute() returning false, size of inputArgMap:(%d) "
            "is not 1\n", inputArgMap.size());
      #endif
      return false;
   }

   // Check for output info, there should be 1 output
   // It is an internal coding error if not 1
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
      outputWrapperTypes.size() != 1)
   {
      if (outputArgMap.size() != 1)
      {
         #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
            ("QuaternionProduct::Execute() returning false, size of outputArgMap: %d or "
               "outputWrapperTypes: %d are not 4\n", outputArgMap.size(), outputWrapperTypes.size());
         #endif
      }
      return false;
   }

   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
         ("QuaternionProduct::Execute() returning false, size of outputRowCounts: &d or "
            "outputColCounts: %d are zero\n", outputRowCounts.size(), outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   std::string msg;
   GmatBase* obj = NULL;
   Array* input1_quatVec1 = NULL;
   Array* input2_quatVec2 = NULL;

   msg = "";
   for (unsigned int i = 0; i < inputNames.size(); i++)
   {
      std::string objName = inputNames[i];
      ObjectMap::iterator objIter = objectStore->find(objName);
      if (objIter != objectStore->end())
      {
         obj = objIter->second;
         #ifdef DEBUG_FUNCTION_EXEC
            MessageInterface::ShowMessage
            ("   input[%d] = <%p><%s>'%s'\n", i, obj, obj ? obj->GetTypeName().c_str() : "NULL",
               obj ? obj->GetName().c_str() : "NULL");
         #endif
         if (obj == NULL)
         {
            msg = msg + "Cannot find the object '" + objName + "' in the objectStore\n";
         }
         else
         {
            if (i == 0 || i == 1) 
            {
               if (obj->IsOfType(Gmat::ARRAY))
               {
                  if (i == 0)
                     input1_quatVec1 = (Array*)obj;
                  else
                     input2_quatVec2 = (Array*)obj;
               }
               else
               {
                  msg = msg + "The object '" + objName + "' is not valid input type; "
                     "It is expecting an Array\n";
               }
            }
         }
      }
   }


   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
         ("QuaternionProduct::Execute() returning false, one or more inputs "
            "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_quatVec1 == NULL) // if inputNames.size() == 0
   {
      throw FunctionException
      ("QuaternionProduct::Execute()  input1_quatVec1 remains unset\n");
   }

   if (input2_quatVec2 == NULL)
   {
      throw FunctionException
      ("QuaternionProduct::Execute()  input2_quatVec2 remains unset\n");
   }

   //Check size of arrays
   Integer numRows, numCols;
   input1_quatVec1->GetSize(numRows, numCols);
   if (!(numRows == 1 && numCols == 4))
      throw FunctionException("QuaternionProduct::Execute()  input1_quatVec1 "
         "has an incorrect size. The quaternion must be a 4 element row "
         "vector\n");

   input2_quatVec2->GetSize(numRows, numCols);
   if (!(numRows == 1 && numCols == 4))
      throw FunctionException("QuaternionProduct::Execute()  input2_quatVec2 "
         "has an incorrect size. The quaternion must be a 4 element row "
         "vector\n");

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif

   Rvector qOut(4);
   try
   {
      Rvector3 q1Vec, q2Vec;
      q1Vec(0) = input1_quatVec1->GetRealParameter("SingleValue", 0);
      q1Vec(1) = input1_quatVec1->GetRealParameter("SingleValue", 1);
      q1Vec(2) = input1_quatVec1->GetRealParameter("SingleValue", 2);
      Real q1Scalar = input1_quatVec1->GetRealParameter("SingleValue", 3);

      q2Vec(0) = input2_quatVec2->GetRealParameter("SingleValue", 0);
      q2Vec(1) = input2_quatVec2->GetRealParameter("SingleValue", 1);
      q2Vec(2) = input2_quatVec2->GetRealParameter("SingleValue", 2);
      Real q2Scalar = input2_quatVec2->GetRealParameter("SingleValue", 3);

      Real q1Mag = GmatMathUtil::Sqrt(q1Vec(0) * q1Vec(0) + q1Vec(1) * q1Vec(1) +
         q1Vec(2) * q1Vec(2) + q1Scalar * q1Scalar);
      if (q1Mag == 0)
         throw FunctionException("The input quaternion \"" +
            input1_quatVec1->GetFullName() + "\" to built-in function"
            " QuaternionProduct has a magnitude of zero");
      if (GmatMathUtil::Abs(q1Mag - 1.0) > 1.0e-12)
      {
         std::string wrnMsg = "**** WARNING **** The quaternion input \"" +
            input1_quatVec1->GetFullName() + "\" to QuaternionProduct has"
            " magnitude %.12f with elements [%.12f %.12f %.12f %.12f]."
            " The quaternion will be normalized.\n";
         MessageInterface::ShowMessage(wrnMsg.c_str(), q1Mag, q1Vec(0),
            q1Vec(1), q1Vec(2), q1Scalar);

         q1Vec /= q1Mag;
         q1Scalar /= q1Mag;
      }

      Real q2Mag = GmatMathUtil::Sqrt(q2Vec(0) * q2Vec(0) + q2Vec(1) * q2Vec(1) +
         q2Vec(2) * q2Vec(2) + q2Scalar * q2Scalar);
      if (q2Mag == 0)
         throw FunctionException("The input quaternion \"" +
            input2_quatVec2->GetFullName() + "\" to built-in function"
            " QuaternionProduct has a magnitude of zero");
      if (GmatMathUtil::Abs(q2Mag - 1.0) > 1.0e-12)
      {
         std::string wrnMsg = "**** WARNING **** The quaternion input \"" +
            input2_quatVec2->GetFullName() + "\" to QuaternionProduct has"
            " magnitude %.12f with elements [%.12f %.12f %.12f %.12f]."
            " The quaternion will be normalized.\n";
         MessageInterface::ShowMessage(wrnMsg.c_str(), q2Mag, q2Vec(0),
            q2Vec(1), q2Vec(2), q2Scalar);

         q2Vec /= q2Mag;
         q2Scalar /= q2Mag;
      }

      // Product calculated using equation 5.1 of Kuipers. Note that GMAT places
      // the quaternion scalar as the last element rather than the first.

      // First calculate scalar element
      qOut(3) = q1Scalar * q2Scalar - (q1Vec * q2Vec);

      // Now the vector portion
      Rvector3 outVec = q1Scalar * q2Vec + q2Scalar * q1Vec + Cross(q1Vec, q2Vec);

      qOut(0) = outVec(0); qOut(1) = outVec(1); qOut(2) = outVec(2);
      if (qOut.GetMagnitude() != 0)
         qOut /= qOut.GetMagnitude();
      if (qOut(3) < 0)
         qOut = -qOut;
   }
   catch (BaseException& be)
   {
      #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
         ("==> Caught Exception: '%s'\n", be.GetFullMessage().c_str());
      #endif
      std::string msg = be.GetFullMessage();
      be.SetDetails("");
      be.SetMessage(msg + " in \"" + callDescription + "\"");
      throw;
   }

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
      ("timeNum: %.12lf\ntimeString: %s\n", timeNum, timeString.c_str());
   #endif

   // Output
   std::map<std::string, ElementWrapper*>::iterator ewi = outputArgMap.begin();

   ElementWrapper* outWrapper;
   outWrapper = CreateOutputArrayWrapper(qOut, ewi->first);

   if (!outWrapper)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif

   //Set the output.
   ewi->second = outWrapper;

   return true;
}

//------------------------------------------------------------------------------
// ElementWrapper* CreateOutputArrayWrapper(Rvector outQuat,
//                                       const std::string& outName)
//------------------------------------------------------------------------------
/*
 * Generates an output wrapper object for the array output provided by this
 * built in function.
 * 
 * @param outQuat The vector containing the data to be provided to the output
 *                object
 * @param outName The name of the object the wrapper will be provided
 * 
 * @return The output element wrapper
 */
//------------------------------------------------------------------------------
ElementWrapper* QuaternionProduct::CreateOutputArrayWrapper(Rvector outQuat,
   const std::string& outName)
{
   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
      ("QuaternionProduct::CreateOutputArrayWrapper() entered, outQuat=[%.12f %.12f %.12f %.12f], outName='%s'\n",
         outQuat(0), outQuat(1), outQuat(2), outQuat(3), outName.c_str());
   #endif

   // Find StringVar object with outName
   ObjectMap::iterator objIter = objectStore->find(outName);
   GmatBase* obj = NULL;
   Array* outArray = NULL;
   if (objIter != objectStore->end())
   {
      obj = objIter->second;
      #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
         ("   outName = <%p><%s>'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
            obj ? obj->GetName().c_str() : "NULL");
      #endif
      outArray = (Array*)obj;
      outArray->SetSize(1,4);
      outArray->SetRvectorParameter("RowValue", outQuat, 0);
   }

   // Create StringObjectWrapper
   ElementWrapper* outWrapper = new ArrayWrapper();
   #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "QuaternionProduct::CreateOutputArrayWrapper()",
         "outWrapper = new ArrayWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Setting outArray to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outArray);

   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
      ("QuaternionProduct::CreateOutputEpochWrapper() returning wrpper <%p>\n",
         outWrapper);
   #endif

   return outWrapper;
}


//------------------------------------------------------------------------------
// void QuaternionProduct::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void QuaternionProduct::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
      ("QuaternionProduct::Finalize() <%p>'%s' entered, nothing to do here?\n",
         this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the QuaternionProduct.
 *
 * @return clone of the QuaternionProduct.
 *
 */
 //------------------------------------------------------------------------------
GmatBase* QuaternionProduct::Clone() const
{
   return (new QuaternionProduct(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
 //---------------------------------------------------------------------------
void QuaternionProduct::Copy(const GmatBase* orig)
{
   operator=(*((QuaternionProduct*)(orig)));
}
