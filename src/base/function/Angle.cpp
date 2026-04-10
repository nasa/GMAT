//$Id$
//------------------------------------------------------------------------------
//                           Angle
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Nov 8, 2021
/**
 * 
 */
//------------------------------------------------------------------------------

#include "Angle.hpp"
#include "FunctionException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "VariableWrapper.hpp"
#include "SpacePoint.hpp"
#include <cmath>

//#define DEBUG_FUNCTION_EXEC



//------------------------------------------------------------------------------
//  Pause(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Pause object (default constructor).
 *
 * @param typeStr String text identifying the object type
 * @param name    Name for the object
 */
//------------------------------------------------------------------------------
Angle::Angle(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("Angle");

   // Build input and output arrays. Function interface is:
   // Angle(time)

   // Add dummy input names
   inputNames.push_back("__Angle_input_1_vertex__");
   inputArgMap.insert(std::make_pair("__Angle_input_1_vertex__", (ElementWrapper*) NULL));
   inputNames.push_back("__Angle_input_2_end1__");
   inputArgMap.insert(std::make_pair("__Angle_input_2_end1__", (ElementWrapper*) NULL));
   inputNames.push_back("__Angle_input_3_end2__");
   inputArgMap.insert(std::make_pair("__Angle_input_3_end2__", (ElementWrapper*) NULL));

   outputNames.push_back("__Angle_output_1_num__");
   outputArgMap.insert(std::make_pair("__Angle_output_1_num__", (ElementWrapper*) NULL));
   outputWrapperTypes.push_back(Gmat::VARIABLE_WT);
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);
}

//------------------------------------------------------------------------------
//  ~Angle(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Angle object (destructor).
 */
//------------------------------------------------------------------------------
Angle::~Angle()
{
}


//------------------------------------------------------------------------------
//  Angle(const Angle &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Angle object (copy constructor).
 *
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Angle::Angle(const Angle &f) :
   BuiltinGmatFunction(f)
{
}


//------------------------------------------------------------------------------
//  Angle& operator=(const Angle &f)
//------------------------------------------------------------------------------
/**
 * Sets one Angle object to match another (assignment operator).
 *
 * @param <f> The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Angle& Angle::operator=(const Angle &f)
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
WrapperTypeArray Angle::GetOutputTypes(IntegerArray &rowCounts,
                                          IntegerArray &colCounts) const
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
void Angle::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Angle::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for Angle
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Angle::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Angle::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("Angle::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}





//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool Angle::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 3 inputs
   if (inputArgMap.size() != 3)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Angle::Execute() returning false, size of inputArgMap:(%d) "
          "is not 3\n", inputArgMap.size());
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
         ("Angle::Execute() returning false, size of outputArgMap: %d or "
          "outputWrapperTypes: %d are not 1\n",outputArgMap.size(),  outputWrapperTypes.size());
      #endif
   }
      return false;
   }

   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Angle::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Checking if inputs exist in the objectStore\n");
   #endif
   std::string msg;
   GmatBase *obj = NULL;
   Spacecraft *scCopy = NULL;
   SpacePoint *input1_vertex = NULL;
   SpacePoint *input2_end1 = NULL;
   SpacePoint *input3_end2 = NULL;

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

            if (obj->IsOfType(Gmat::SPACE_POINT))
            {
               if (obj->IsOfType(Gmat::SPACECRAFT) && scCopy == NULL)
               {
                  scCopy = (Spacecraft*)obj;
               }

               if (i == 0)
                  input1_vertex = (SpacePoint*)obj;
               else if (i == 1)
                  input2_end1 = (SpacePoint*)obj;
               else
                  input3_end2 = (SpacePoint*)obj;
            }
            else
            {
               msg = msg + "The object '" + objName + "' is not a valid input type; "
                     "the angle function is expecting a SpacePoint object.\n";
            }
         }
      }
   }

   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("Angle::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_vertex == NULL)
   {
      throw FunctionException
         ("Angle::Execute()  input1_vertex remains unset\n");
   }
   if (input2_end1 == NULL)
   {
      throw FunctionException
         ("Angle::Execute()  input2_end1 remains unset\n");
   }
   if (input3_end2 == NULL)
   {
      throw FunctionException
         ("Angle::Execute()  input3_end2 remains unset\n");
   }

   // Check for duplicate inputs
   std::string name1 = inputArgMap["__Angle_input_1_vertex__"]->GetDescription();
   std::string name2 = inputArgMap["__Angle_input_2_end1__"]->GetDescription();
   std::string name3 = inputArgMap["__Angle_input_3_end2__"]->GetDescription();

   if (name1 == name2 || name1 == name3 || name2 == name3)
   {
      throw FunctionException
         ("The Angle function cannot be evaluated: it uses the same object for more than one point");
   }

   Real epoch;

   //Retrieve epoch & positions. Get from Spacecraft if possible.
   if (scCopy != NULL)
   {
      epoch = scCopy->GetEpoch();
   }
   else
   {
      epoch = GmatGlobal::Instance()->GetCurrentEpoch();
   }

   Integer loc = 0;
   Rvector3 posVertex, posEnd1, posEnd2;
   try
   {
      posVertex = input1_vertex->GetMJ2000Position(epoch);
      ++loc;
      posEnd1 = input2_end1->GetMJ2000Position(epoch);
      ++loc;
      posEnd2 = input3_end2->GetMJ2000Position(epoch);
   }
   catch (BaseException *ex)
   {
      std::string point = "Vertex";
      if (loc == 1)
         point = "Endpoint 1";
      if (loc == 2)
         point = "Endpoint 2";

      std::string msg = ex->GetFullMessage() + " for " + point;
      throw FunctionException(msg);
   }

   #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage("Vertex: %s\nPoint1: %s\nPoint2: %s\n",
            posVertex.ToString().c_str(), posEnd1.ToString().c_str(),
            posEnd2.ToString().c_str());
   #endif
   //Calculate angle
   Rvector3 vector1 = Rvector3(posEnd1[0] - posVertex[0],
                               posEnd1[1] - posVertex[1],
                               posEnd1[2] - posVertex[2]);
   Rvector3 vector2 = Rvector3(posEnd2[0] - posVertex[0],
                               posEnd2[1] - posVertex[1],
                               posEnd2[2] - posVertex[2]);

   Real v1mag = sqrt(pow(vector1[0], 2) + pow(vector1[1], 2) + pow(vector1[2], 2));
   Rvector3 v1norm = Rvector3(vector1[0]/v1mag,
                              vector1[1]/v1mag,
                              vector1[2]/v1mag);

   Real v2mag = sqrt(pow(vector2[0], 2) + pow(vector2[1], 2) + pow(vector2[2], 2));
   Rvector3 v2norm = Rvector3(vector2[0]/v2mag,
                              vector2[1]/v2mag,
                              vector2[2]/v2mag);

   Real dotprod = v1norm[0]*v2norm[0] + v1norm[1]*v2norm[1] + v1norm[2]*v2norm[2];
   Real angle = acos(dotprod);
   angle = angle * GmatMathConstants::DEG_PER_RAD;

   // Output
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();

   ElementWrapper *outWrapper;

   outWrapper = CreateOutputVariableWrapper(angle, ewi->first);

   if (!outWrapper)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif

   //Set the output.
   ewi->second = outWrapper;



   return true;
}

ElementWrapper* Angle::CreateOutputVariableWrapper(Real outputNum, const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Angle::CreateOutputVariableWrapper() entered, a1MjdEpoch=%.12f, outName='%s'\n",
       a1MjdEpoch, outName.c_str());
   #endif

   // Find StringVar object with outName
   ObjectMap::iterator objIter = objectStore->find(outName);
   GmatBase *obj = NULL;
   RealVar *outRealVar = NULL;
   if (objIter != objectStore->end())
   {
      obj = objIter->second;
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   outName = <%p><%s>'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      #endif
      outRealVar = (RealVar*)obj;
      outRealVar->SetReal(outputNum);
   }

   // Create VariableWrapper
   ElementWrapper *outWrapper = new VariableWrapper();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "Angle::CreateOutputVariableWrapper()",
       "outWrapper = new VariableWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outString to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outRealVar);
   outWrapper->SetReal(outputNum);

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Angle::CreateOutputVariableWrapper() returning warpper <%p>\n",
       outWrapper);
   #endif

   return outWrapper;
}



//------------------------------------------------------------------------------
// void Angle::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void Angle::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Angle::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Angle.
 *
 * @return clone of the Angle.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Angle::Clone() const
{
   return (new Angle(*this));
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
void Angle::Copy(const GmatBase* orig)
{
   operator=(*((Angle *)(orig)));
}


