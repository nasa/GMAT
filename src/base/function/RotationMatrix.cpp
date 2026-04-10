//$Id$
//------------------------------------------------------------------------------
//                           RotationMatrix
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
// Created: Apr 16, 2018
/**
 * 
 */
//------------------------------------------------------------------------------

#include "RotationMatrix.hpp"
#include "FunctionException.hpp"
#include "MessageInterface.hpp"
#include "ArrayWrapper.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "StringUtil.hpp"

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
//  RotationMatrix(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the RotationMatrix object (default constructor).
 *
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
RotationMatrix::RotationMatrix(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("RotationMatrix");

   // Build input and output arrays. Function interface is:
   // RotationMatrix(time)

   // Add dummy input names
   inputNames.push_back("__RotationMatrix_input_1_coordsys__");
   inputArgMap.insert(std::make_pair("__RotationMatrix_input_1_coordsys__", (ElementWrapper*) NULL));
   inputNames.push_back("__RotationMatrix_input_2_epoch__");
   inputArgMap.insert(std::make_pair("__RotationMatrix_input_2_epoch__", (ElementWrapper*) NULL));
   inputNames.push_back("__RotationMatrix_input_3_epochFormat__");
   inputArgMap.insert(std::make_pair("__RotationMatrix_input_3_epochFormat__", (ElementWrapper*) NULL));

   // Add dummy output names
   outputNames.push_back("__RotationMatrix_output_1_matrix__");
   outputArgMap.insert(std::make_pair("__RotationMatrix_output_1_matrix__", (ElementWrapper*) NULL));
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);
   outputRowCounts.push_back(3);
   outputColCounts.push_back(3);

   outputNames.push_back("__RotationMatrix_output_2_timederivative__");
   outputArgMap.insert(std::make_pair("__RotationMatrix_output_2_timederivative__", (ElementWrapper*) NULL));
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);
   outputRowCounts.push_back(3);
   outputColCounts.push_back(3);

   theTimeConverter = TimeSystemConverter::Instance();
}

//------------------------------------------------------------------------------
//  ~RotationMatrix(void)
//------------------------------------------------------------------------------
/**
 * Destroys the RotationMatrix object (destructor).
 */
//------------------------------------------------------------------------------
RotationMatrix::~RotationMatrix()
{
}


//------------------------------------------------------------------------------
//  RotationMatrix(const RotationMatrix &f)
//------------------------------------------------------------------------------
/**
 * Constructs the RotationMatrix object (copy constructor).
 *
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
RotationMatrix::RotationMatrix(const RotationMatrix &f) :
   BuiltinGmatFunction(f)
{
   theTimeConverter = TimeSystemConverter::Instance();
}


//------------------------------------------------------------------------------
//  RotationMatrix& operator=(const RotationMatrix &f)
//------------------------------------------------------------------------------
/**
 * Sets one RotationMatrix object to match another (assignment operator).
 *
 * @param <f> The object that is copied.
 *
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
RotationMatrix& RotationMatrix::operator=(const RotationMatrix &f)
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
WrapperTypeArray RotationMatrix::GetOutputTypes(IntegerArray &rowCounts,
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
void RotationMatrix::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("RotationMatrix::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif

   // Set output wrapper type for RotationMatrix
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool RotationMatrix::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("RotationMatrix::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif

   BuiltinGmatFunction::Initialize(objInit);

   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("RotationMatrix::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool RotationMatrix::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 3 inputs
   if (inputArgMap.size() != 3)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("RotationMatrix::Execute() returning false, size of inputArgMap:(%d) "
          "is not 3\n", inputArgMap.size());
      #endif
      return false;
   }

   // Check for output info, there should be 2 output
   // It is an internal coding error if not 2
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 2)
   {
      if (outputArgMap.size() != 2)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("RotationMatrix::Execute() returning false, size of outputArgMap: %d or "
             "outputWrapperTypes: %d are not 2\n",outputArgMap.size(),  outputWrapperTypes.size());
         #endif
      }
      return false;
   }

   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("RotationMatrix::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }

   // Check if input names are in the objectStore
   std::string msg;
   GmatBase *obj = NULL;
   GmatBase *input1_coordSys = NULL;
   GmatBase *input2_epoch = NULL;
   GmatBase *input3_epochFormat = NULL;

   msg = "";
   for (unsigned int i = 0; i < inputNames.size(); i++)
   {
      std::string objName = inputNames[i];
      if (i == 0)
         GmatStringUtil::RemoveAll(objName, "'", 0);
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
            if (i == 0)
            {
               if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
                  input1_coordSys = obj;
               else
                  msg = msg + "Error in the input parameters for the RotationMatrix "
                        "function. The function call has the format\n"
                        "RotationMatrix(CoordinateSystem, EpochString, TimeSystemString)\n"
                        "where CoordinateSystem is a defined coordinate system object, "
                        "EpochString is the time of the rotation, and TimeSystemString"
                        "is a defined GMAT time system.\n";
            }
            else if (i == 1 || i == 2)
            {
               // 1st or 3rd Input: check for String type
               if (obj->IsOfType(Gmat::STRING))
               {
                  if (i == 1)
                     input2_epoch = obj;
                  else
                     input3_epochFormat = obj;
               }
               else
               {
                  if (i == 1)
                     msg = msg + "Error in the input parameters for the RotationMatrix "
                           "function. The function call has the format\n"
                           "RotationMatrix(CoordinateSystem, EpochString, TimeSystemString)\n"
                           "where CoordinateSystem is a defined coordinate system object, "
                           "EpochString is the time of the rotation, and TimeSystemString"
                           "is a defined GMAT time system.\n";
                  else
                     msg = msg + "Error in the input parameters for the RotationMatrix "
                           "function. The function call has the format\n"
                           "RotationMatrix(CoordinateSystem, EpochString, TimeSystemString)\n"
                           "where CoordinateSystem is a defined coordinate system object, "
                           "EpochString is the time of the rotation, and TimeSystemString"
                           "is a defined GMAT time system.\n";
               }
            }
         }
      }
   }

   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("RotationMatrix::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      throw FunctionException(msg + " in \"" + callDescription + "\"");
   }

   //Check that the inputs have been set
   if (input1_coordSys == NULL)
   {
      throw FunctionException
         ("RotationMatrix::Execute()  First input is unset.\n");
   }
   if (input2_epoch == NULL)
   {
      throw FunctionException
         ("RotationMatrix::Execute()  Second input is unset.\n");
   }
   if (input3_epochFormat == NULL)
   {
      throw FunctionException
         ("RotationMatrix::Execute()  Third input is unset\n");
   }


   CoordinateSystem* inCoordSys = NULL;
   std::string inEpoch = "";
   std::string inEpochFormat = "";

   // Set objects to passed in input values
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Evaluating input value\n");
   #endif
   try
   {
      inCoordSys = (CoordinateSystem*)(inputArgMap[input1_coordSys->GetName()]->GetRefObject());
      inEpoch = inputArgMap[input2_epoch->GetName()]->EvaluateString();
      inEpochFormat = inputArgMap[input3_epochFormat->GetName()]->EvaluateString();

      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   checking valid input\n");
      #endif

      std::list<std::string> validFormats{"A1ModJulian", "TAIModJulian",
         "UTCModJulian", "TDBModJulian", "TTModJulian", "A1Gregorian",
         "TAIGregorian", "UTCGregorian", "TDBGregorian", "TTGregorian"};

      //Error if the format string is not in the list of valid formats.
      if (std::find(validFormats.begin(), validFormats.end(), inEpochFormat) == validFormats.end())
      {
         throw FunctionException
            ("Epoch format input to RotationMatrix() function is not a recognized format.");
      }
   }
   catch (BaseException &be)
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
      ("inCoordSys = <%p>'%s'\ninEpoch = %s\ninEpochFormat = %s\n", inCoordSys,
            inCoordSys->GetName().c_str(), inEpoch.c_str(), inEpochFormat.c_str());
   #endif

   //Clone the CoordinateSystem so that the original won't be affected.
   CoordinateSystem* cloneCoordSys = (CoordinateSystem*)inCoordSys->Clone();

//   std::string axesType = inCoordSys->GetStringParameter("axesType");
   SpacePoint *origin = (SpacePoint*)inCoordSys->GetRefObject(Gmat::SPACE_POINT, "_GFOrigin_");
   SpacePoint *primary = (SpacePoint*)inCoordSys->GetRefObject(Gmat::SPACE_POINT, "_GFPrimary_");
   SpacePoint *secondary = (SpacePoint*)inCoordSys->GetRefObject(Gmat::SPACE_POINT, "_GFSecondary_");
   SpacePoint *j2000Body = inCoordSys->GetJ2000Body();
   SolarSystem *solarSystem = inCoordSys->GetSolarSystem();

   cloneCoordSys->SetStringParameter("Origin", origin->GetName());
   cloneCoordSys->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
   cloneCoordSys->SetJ2000Body(j2000Body);
   cloneCoordSys->SetSolarSystem(solarSystem);
   cloneCoordSys->Initialize();

   Real epochNum = -999.999;
   Real epochMjd;
   std::string epochMjdStr;
   theTimeConverter->Convert(inEpochFormat, epochNum, inEpoch, "A1ModJulian", epochMjd, epochMjdStr, 1);

   Rvector6 pos;
   pos(0) = pos(1) = pos(2) = pos(3) = pos(4) = pos(5) = 1.0;

   cloneCoordSys->FromBaseSystem(A1Mjd(epochMjd), pos, true, true);

   Rmatrix33 lastRotation;
   Rmatrix33 lastRotationDot;
   // ICRF Coordinate systems don't update the rotation matrix correctly. so...
   if (cloneCoordSys->GetBaseSystem() == "ICRF")
   {
      // get rotation matrix R from ICRF to FK5:
      CoordinateConverter cc;
      lastRotation    = cc.GetRotationMatrixFromICRFToFK5(epochMjd);
   }
   else
      lastRotation = cloneCoordSys->GetLastRotationMatrix();

   lastRotationDot = cloneCoordSys->GetLastRotationDotMatrix();

   // Output
   int index = 0;
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();

   //Output 1: Rotation Matrix
   ElementWrapper *outWrapper1 = CreateOutputArrayWrapper(index, lastRotation, ewi->first);

   if (!outWrapper1)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif

   ewi->second = outWrapper1;

   //Output 2: Time Derivative
   ++index;
   ++ewi;

   ElementWrapper *outWrapper2 = CreateOutputArrayWrapper(index, lastRotationDot, ewi->first);

   if (!outWrapper2)
      return false;

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper2 to outputWrapperMap\n");
   #endif

   ewi->second = outWrapper2;

   return true;
}

//------------------------------------------------------------------------------
// ElementWrapper* CreateOutputArrayWrapper(Integer outIndex,
//                                          Rmatrix33 &outMatrix,
//                                          const std::string &outName)
//------------------------------------------------------------------------------
ElementWrapper* RotationMatrix::CreateOutputArrayWrapper(Integer outIndex,
                                                         Rmatrix33 &outMatrix,
                                                         const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("RotationMatrix::CreateOutputArrayWrapper() entered, outIndex=%d, outName='%s'\n",
       outIndex, outName.c_str());
   #endif

   if (outIndex != 0 && outIndex != 1)
   {
      MessageInterface::ShowMessage("**** CODING ERROR **** outIndex must be 0 or 1\n");
      return NULL;
   }

   int numRows = outputRowCounts[outIndex];
   int numCols = outputColCounts[outIndex];

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   outName  = '%s', numRows = %d, numCols = %d\n", outName.c_str(),
       numRows, numCols);
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   outMatrix = %s\n", outMatrix.ToString().c_str());
   #endif

   // Find Array object with outName
   ObjectMap::iterator objIter = objectStore->find(outName);
   GmatBase *obj = NULL;
   Array *outArray = NULL;
   if (objIter != objectStore->end())
   {
      obj = objIter->second;
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   outName = <%p><%s>'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      #endif
      outArray = (Array*)obj;
      outArray->SetSize(numRows, numCols);
      outArray->SetRmatrix(outMatrix);
   }

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Creating ArrayWrapper for output\n");
   #endif

   // Create ArrayWrapper
   ElementWrapper *outWrapper = new ArrayWrapper();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "RotationMatrix::CreateOutputArrayWrapper()",
       "outWrapper = new ArrayWrapper()");
   #endif

   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outArray to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outArray);

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("RotationMatrix::CreateOutputArrayWrapper() returning wrapper <%p>\n",
       outWrapper);
   #endif

   return outWrapper;
}

//------------------------------------------------------------------------------
// void RotationMatrix::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void RotationMatrix::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("RotationMatrix::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the RotationMatrix.
 *
 * @return clone of the RotationMatrix.
 *
 */
//------------------------------------------------------------------------------
GmatBase* RotationMatrix::Clone() const
{
   return (new RotationMatrix(*this));
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
void RotationMatrix::Copy(const GmatBase* orig)
{
   operator=(*((RotationMatrix *)(orig)));
}



