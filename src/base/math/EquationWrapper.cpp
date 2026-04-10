//------------------------------------------------------------------------------
//                           EquationWrapper
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jun 14, 2018
/**
 * Wrapper for (RHS) equations
 */
//------------------------------------------------------------------------------

#include "EquationWrapper.hpp"
#include "MathTree.hpp"
#include "MessageInterface.hpp"


#define DEBUG_INITALIZE
#define DEBUG_EVALUATE


EquationWrapper::EquationWrapper(const std::string &eq) :
   ElementWrapper    (),
   theEquation       (eq),
   resultant         (NULL),
   configObjectMap   (NULL),
   hasEvaluated      (false)
{
   #ifdef DEBUG_INITALIZE
      MessageInterface::ShowMessage("Creating the EquationWrapper at "
            "address %p\n", this);
   #endif

   wrapperType = Gmat::EQUATION_WT;
   if (theEquation == "")
      // Default setting if no input equation string
      dataType = Gmat::UNKNOWN_PARAMETER_TYPE;
   else
   {
//      dataType = Gmat::REAL_TYPE;
//      dataType = Gmat::RVECTOR_TYPE;
//      dataType = Gmat::RMATRIX_TYPE;
//      dataType = Gmat::STRING_TYPE;
   }
}


EquationWrapper::~EquationWrapper()
{
   if (resultant)
      delete resultant;
}


EquationWrapper::EquationWrapper(const EquationWrapper& ew) :
   ElementWrapper    (),
   theEquation       (ew.theEquation),
   resultant         (NULL),
   configObjectMap   (NULL),
   hasEvaluated      (false)
{
}


EquationWrapper& EquationWrapper::operator=(const EquationWrapper& ew)
{
   if (this != &ew)
   {
      ElementWrapper::operator=(ew);

      theEquation     = ew.theEquation;
      configObjectMap = NULL;
      hasEvaluated    = false;
      if (resultant)
      {
         delete resultant;
         resultant = NULL;
      }
   }

   return *this;
}


ElementWrapper* EquationWrapper::Clone() const
{
   return new EquationWrapper(*this);
}


void EquationWrapper::SetEquation(const std::string &eq)
{
   theEquation = eq;
}

//------------------------------------------------------------------------------
//  void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
/**
 * Called by the Interpreter to set the local resource store used by the
 * GmatCommand for InterpretAction()
 *
 * @param map Pointer to the local object map
 */
//------------------------------------------------------------------------------
void EquationWrapper::SetConfiguredObjectMap(ObjectMap *map)
{
   configObjectMap = map;
}


void EquationWrapper::ClearRefObjectNames()
{

}

const StringArray& EquationWrapper::GetRefObjectNames()
{
   refObjects.clear();

   refObjects = theTree.GetWrapperObjectNames();

   return refObjects;
}

bool EquationWrapper::SetRefObjectName(const std::string &name, Integer index)
{
   bool retval = false;
   return retval;
}

GmatBase* EquationWrapper::GetRefObject(const std::string &name)
{
   GmatBase *retval = NULL;
   return retval;
}

bool EquationWrapper::SetRefObject(GmatBase *obj)
{
   bool retval = false;

   if (obj)
   {
      std::string name = obj->GetName();
      if (equationObjectMap.find(name) == equationObjectMap.end())
         equationObjectMap[name] = obj;
      retval = true;
   }

   return retval;
}

bool EquationWrapper::RenameObject(const std::string &oldName, const std::string &newName)
{
   bool retval = false;
   return retval;
}



bool EquationWrapper::Initialize(ObjectMap *objectMap, ObjectMap *globalObjectMap)
{
   bool retval = false;

   if (theEquation != "")
   {
      retval = theTree.Initialize(objectMap, globalObjectMap);
      Integer type, rows, cols;
      theTree.GetMathTree(false)->GetOutputInfo(type, rows, cols);
      dataType = (Gmat::ParameterType)type;
   }

   return retval;
}

bool EquationWrapper::Initialize()
{
   #ifdef DEBUG_INITALIZE
      MessageInterface::ShowMessage("Initializing the EquationWrapper at "
            "address %p\n", this);
   #endif

   bool retval = false;

   // Validate that all references are set on the RHSEquation
   if (theEquation != "")
   {
      retval = theTree.Initialize(&equationObjectMap, NULL);
      Integer type, rows, cols;
      theTree.GetMathTree(false)->GetOutputInfo(type, rows, cols);
      dataType = (Gmat::ParameterType)type;

      if (resultant)
      {
         delete resultant;
         resultant = NULL;
      }
   }

//*****   TODO: Analog of this code in Assignment:
//*****   if (rhsEquation)
//*****      rhsEquation->GetMathTree(false)->SetMathWrappers(&mathWrapperMap);

   return retval;
}

//------------------------------------------------------------------------------
// bool ConstructTree()
//------------------------------------------------------------------------------
/**
 * Parse the equation string and build the tree
 *
 * @return true is an equation was set up
 */
//------------------------------------------------------------------------------
bool EquationWrapper::ConstructTree()
{
   return theTree.BuildExpression(theEquation, configObjectMap, true);
}

//------------------------------------------------------------------------------
// RHSEquation *GetTree()
//------------------------------------------------------------------------------
/**
 * Access the equation tree container
 *
 * @return The RHS pointer
 */
//------------------------------------------------------------------------------
RHSEquation *EquationWrapper::GetTree()
{
   return &theTree;
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the ElementWrapper object.
 *
 * @return value type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EquationWrapper::GetDataType() const
{
   return dataType;
}

//------------------------------------------------------------------------------
// bool EvaluateEquation()
//------------------------------------------------------------------------------
/**
 * Evaluate the equation to generate the data for teh wrapper to return
 *
 * @return true if the equation fires successfully
 */
//------------------------------------------------------------------------------
bool EquationWrapper::EvaluateEquation()
{
   #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage("Evaluating the EquationWrapper at "
            "address %p\n", this);
   #endif

   bool retval = false;

   resultant = theTree.RunMathTree(NULL);
   Gmat::ParameterType returnType = Gmat::UNKNOWN_PARAMETER_TYPE;
   if (resultant)
   {
      returnType = resultant->GetDataType();

      switch (dataType)
      {
      case Gmat::REAL_TYPE:
         if (returnType == dataType)
         {
            hasEvaluated = true;
            retval = true;
         }
         break;

      default:
         MessageInterface::ShowMessage("The equation \"%s\" could not be "
               "evaluated\n", theEquation.c_str());
         break;
      }
   }

   return retval;
}


//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped object.
 *
 * @note  This is a pure (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
Real EquationWrapper::EvaluateReal() const
{
   Real retval = 0.0;

   #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage("Evaluating the Real value for %s\n",
            theEquation.c_str());
   #endif

   if (theEquation != "")
   {
      if (resultant != NULL)
      {
         switch (dataType)
         {
         case Gmat::REAL_TYPE:
            retval = resultant->EvaluateReal();
            break;

         default:
            MessageInterface::ShowMessage("The equation \"%s\" does not "
                  "evaluate to a real number\n", theEquation.c_str());
            break;
         }
      }
   }

   return retval;
}

//---------------------------------------------------------------------------
//  bool SetReal(const Real value)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 *
 * @note  This is a pure (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
bool EquationWrapper::SetReal(const Real toValue)
{
   bool retval = false;
   return retval;
}

const Rmatrix& EquationWrapper::EvaluateArray() const
{
   return retMat;
}

bool EquationWrapper::SetArray(const Rmatrix &toValue)
{
   bool retval = false;
   return retval;
}


const Rvector& EquationWrapper::EvaluateRvector() const
{
   if (dataType != Gmat::RVECTOR_TYPE)
   {

   }

   return retVec;
}


bool EquationWrapper::SetRvector(const Rvector &toValue)
{
   bool retval = false;
   return retval;
}


//bool EquationWrapper::SetString(const std::string &toValue)
//{
//   bool retval = false;
//   return retval;
//}
//
//
//std::string EquationWrapper::EvaluateString() const
//{
//}



//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method that parses the description string so that the reference
 * objects and related properties are identified.
 *
 * @return true if successful; false otherwise.
 *
 * @note  This is a pure virtual (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
void EquationWrapper::SetupWrapper()
{

}
