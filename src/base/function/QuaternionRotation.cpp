//------------------------------------------------------------------------------
//                               QuaternionRotation
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
 *  Vector rotations using a unit quaternion.
 */
 //------------------------------------------------------------------------------

#include "QuaternionRotation.hpp"
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
//  QuaternionRotation(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the QuaternionRotation object (default constructor).
 *
 * @param typeStr String text identifying the object type
 * @param name    Name for the object
 */
 //------------------------------------------------------------------------------
QuaternionRotation::QuaternionRotation(const std::string& typeStr, const std::string& name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("QuaternionRotation");

   // Build input and output arrays. Function interface is:
   // QuaternionRotation(time)

   // Add dummy input names
   inputNames.push_back("__QuaternionRot_input_1_quaternion__");
   inputArgMap.insert(std::make_pair("__QuaternionRot_input_1_quaternion__", (ElementWrapper*)NULL));
   inputNames.push_back("__QuaternionRot_input_2_vector__");
   inputArgMap.insert(std::make_pair("__QuaternionRot_input_2_vector__", (ElementWrapper*)NULL));

   // Add dummy output names
   outputNames.push_back("__QuaternionRot_output_1_rotatedVec__");
   outputArgMap.insert(std::make_pair("__QuaternionRot_output_1_rotatedVec__", (ElementWrapper*)NULL));
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);
}

//------------------------------------------------------------------------------
//  ~QuaternionRotation(void)
//------------------------------------------------------------------------------
/**
 * Destroys the QuaternionRotation object (destructor).
 */
 //------------------------------------------------------------------------------
QuaternionRotation::~QuaternionRotation()
{
}


//------------------------------------------------------------------------------
//  QuaternionRotation(const QuaternionRotation &f)
//------------------------------------------------------------------------------
/**
 * Constructs the QuaternionRotation object (copy constructor).
 *
 * @param f Object that is copied
 */
 //------------------------------------------------------------------------------
QuaternionRotation::QuaternionRotation(const QuaternionRotation& f) :
   BuiltinGmatFunction(f)
{

}


//------------------------------------------------------------------------------
//  QuaternionRotation& operator=(const QuaternionRotation &f)
//------------------------------------------------------------------------------
/**
 * Sets one QuaternionRotation object to match another (assignment operator).
 *
 * @param f The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
 //------------------------------------------------------------------------------
QuaternionRotation& QuaternionRotation::operator=(const QuaternionRotation& f)
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
WrapperTypeArray QuaternionRotation::GetOutputTypes(IntegerArray& rowCounts,
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
void QuaternionRotation::SetOutputTypes(WrapperTypeArray& outputTypes,
   IntegerArray& rowCounts, 
   IntegerArray& colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
      MessageInterface::ShowMessage
      ("QuaternionRotation::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for QuaternionRotation
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool QuaternionRotation::Initialize(ObjectInitializer* objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
      ("QuaternionRotation::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
      ("QuaternionRotation::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool QuaternionRotation::Execute(ObjectInitializer* objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 2 inputs
   if (inputArgMap.size() != 2)
   {
      #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
         ("QuaternionRotation::Execute() returning false, size of inputArgMap:(%d) "
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
            ("QuaternionRotation::Execute() returning false, size of outputArgMap: %d or "
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
         ("QuaternionRotation::Execute() returning false, size of outputRowCounts: &d or "
            "outputColCounts: %d are zero\n", outputRowCounts.size(), outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   std::string msg;
   GmatBase* obj = NULL;
   Array* input1_quaternion = NULL;
   Array* input2_vector = NULL;

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
                     input1_quaternion = (Array*)obj;
                  else
                     input2_vector = (Array*)obj;
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
         ("QuaternionRotation::Execute() returning false, one or more inputs "
            "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_quaternion == NULL) // if inputNames.size() == 0
   {
      throw FunctionException
      ("QuaternionRotation::Execute()  input1_quatVec1 remains unset\n");
   }

   if (input2_vector == NULL)
   {
      throw FunctionException
      ("QuaternionRotation::Execute()  input2_quatVec2 remains unset\n");
   }

   //Check size of arrays
   Integer numRows, numCols;
   input1_quaternion->GetSize(numRows, numCols);
   if (!(numRows == 1 && numCols == 4))
      throw FunctionException("QuaternionRotation::Execute()  input1_quaternion "
         "has an incorrect size. The quaternion must be a 4 element row "
         "vector\n");

   input2_vector->GetSize(numRows, numCols);
   if (!(numRows == 1 && numCols == 3))
      throw FunctionException("QuaternionRotation::Execute()  input2_vector "
         "has an incorrect size. The vector must be a 3 element row "
         "vector\n");

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif

   Rvector3 outVec;
   try
   {
      Rvector3 qVec, inVec;
      qVec(0) = input1_quaternion->GetRealParameter("SingleValue", 0);
      qVec(1) = input1_quaternion->GetRealParameter("SingleValue", 1);
      qVec(2) = input1_quaternion->GetRealParameter("SingleValue", 2);
      Real qScalar = input1_quaternion->GetRealParameter("SingleValue", 3);

      Real qNorm = GmatMathUtil::Sqrt(qVec(0) * qVec(0) + qVec(1) * qVec(1) +
         qVec(2) * qVec(2) + qScalar * qScalar);
      if (qNorm == 0)
         throw FunctionException("The input quaternion \"" +
            input1_quaternion->GetFullName() + "\" to built-in function"
            " QuaternionRotation has a magnitude of zero");

      // Normalize the quaternion
      if (GmatMathUtil::Abs(qNorm - 1.0) > 1.0e-12)
      {
         std::string wrnMsg = "**** WARNING **** The quaternion input \"" +
            input1_quaternion->GetFullName() + "\" to QuaternionRotation has"
            " magnitude %.12f with elements [%.12f %.12f %.12f %.12f]."
            " The quaternion will be normalized.\n";
         MessageInterface::ShowMessage(wrnMsg.c_str(), qNorm, qVec(0),
            qVec(1), qVec(2), qScalar);

         qVec /= qNorm;
         qScalar /= qNorm;
      }

      inVec(0) = input2_vector->GetRealParameter("SingleValue", 0);
      inVec(1) = input2_vector->GetRealParameter("SingleValue", 1);
      inVec(2) = input2_vector->GetRealParameter("SingleValue", 2);

      // Rotation calculated using equation 5.14 of Kuipers. Note that GMAT 
      // places the quaternion scalar as the last element rather than the first.

      outVec = (qScalar * qScalar - qVec.Norm() * qVec.Norm()) * inVec +
         2 * (qVec * inVec) * qVec +
         2 * qScalar * Cross(qVec, inVec);
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
   outWrapper = CreateOutputArrayWrapper(outVec, ewi->first);

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
// ElementWrapper* CreateOutputArrayWrapper(Rvector3 outVec,
//                                       const std::string& outName)
//------------------------------------------------------------------------------
/*
 * Generates an output wrapper object for the array output provided by this
 * built in function.
 * 
 * @param outVec  The vector containing the data to be provided to the output
 *                object
 * @param outName The name of the object the wrapper will be provided
 * 
 * @return The output element wrapper
 */
//------------------------------------------------------------------------------
ElementWrapper* QuaternionRotation::CreateOutputArrayWrapper(Rvector3 outVec,
   const std::string& outName)
{
   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
      ("QuaternionRotation::CreateOutputArrayWrapper() entered, outVec=[%.12f %.12f %.12f], outName='%s'\n",
         outVec(0), outVec(1), outVec(2), outName.c_str());
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
      outArray->SetSize(1,3);
      outArray->SetRvectorParameter("RowValue", outVec, 0);
   }

   // Create StringObjectWrapper
   ElementWrapper* outWrapper = new ArrayWrapper();
   #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "QuaternionRotation::CreateOutputArrayWrapper()",
         "outWrapper = new ArrayWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("   Setting outArray to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outArray);

   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
      ("QuaternionRotation::CreateOutputEpochWrapper() returning wrpper <%p>\n",
         outWrapper);
   #endif

   return outWrapper;
}


//------------------------------------------------------------------------------
// void QuaternionRotation::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void QuaternionRotation::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
      ("QuaternionRotation::Finalize() <%p>'%s' entered, nothing to do here?\n",
         this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the QuaternionRotation.
 *
 * @return clone of the QuaternionRotation.
 *
 */
 //------------------------------------------------------------------------------
GmatBase* QuaternionRotation::Clone() const
{
   return (new QuaternionRotation(*this));
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
void QuaternionRotation::Copy(const GmatBase* orig)
{
   operator=(*((QuaternionRotation*)(orig)));
}
