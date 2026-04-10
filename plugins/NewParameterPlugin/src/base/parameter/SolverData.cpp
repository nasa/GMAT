//$Id$
//------------------------------------------------------------------------------
//                           SolverData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 4, 2013
/**
 * Parameter data base for solver Parameters
 */
//------------------------------------------------------------------------------

#include "SolverData.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"


//---------------------------------
// static data
//---------------------------------

const Real SolverData::SOLVER_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;
const std::string SolverData::SOLVER_STRING_UNDEFINED = "Invalid Solver Field";


const std::string
SolverData::VALID_OBJECT_TYPE_LIST[SolverDataObjectCount] =
{
   "Solver"
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SolverData()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SolverData::SolverData() :
   RefData     (),
   mSolver     (NULL)
{
}

//------------------------------------------------------------------------------
// ~SolverData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolverData::~SolverData()
{
}

//------------------------------------------------------------------------------
// SolverData(const SolverData& sd)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 *
 * @param sd Solver data that is copied into the new instance
 */
//------------------------------------------------------------------------------
SolverData::SolverData(const SolverData& sd) :
   RefData     (sd),
   mSolver     (sd.mSolver)
{
}

//------------------------------------------------------------------------------
// SolverData& operator=(const SolverData& sd)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sd Solver data that is copied into this one
 *
 * @return This SolverData, set to look like sd
 */
//------------------------------------------------------------------------------
SolverData& SolverData::operator=(const SolverData& sd)
{
   if (this != &sd)
   {
      RefData::operator=(sd);

      mSolver = sd.mSolver;
   }

   return *this;
}

//------------------------------------------------------------------------------
// Real GetSolverReal(const std::string& str)
//------------------------------------------------------------------------------
/**
 * Returns real data for use in parameters
 *
 * @param str String identifying the real data desired
 *
 * @return The real parameter value
 */
//------------------------------------------------------------------------------
Real SolverData::GetSolverReal(const std::string& str)
{
   Real retval = -3.0;

   if (mSolver == NULL)
      //Find the solver to use
      GetSolver();

   if (mSolver)
   {

      if (str == "State")
      {
         Integer status = mSolver->GetIntegerParameter(
               mSolver->GetParameterID("IntegerSolverStatus"));
  
         switch (status)
         {
         case Gmat::CREATED:
         case Gmat::COPIED:
         case Gmat::INITIALIZED:
         case Gmat::RUN:
            retval = 0.0;
            break;

         case Gmat::CONVERGED:
            retval = 1.0;
            break;

         case Gmat::EXCEEDED_ITERATIONS:
            retval = -1.0;
            break;

         case Gmat::FAILED:
            retval = -2.0;
            break;

         case Gmat::UNKNOWN_STATUS:
         default:
            break;
         }
      }
   }
   else
      retval = -4.0;

   return retval;
}

//------------------------------------------------------------------------------
// std::string GetSolverString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Returns string data for use in parameters
 *
 * @param str String identifying the string desired
 *
 * @return The string parameter value
 */
//------------------------------------------------------------------------------
std::string SolverData::GetSolverString(const std::string &str)
{
   std::string retval = SOLVER_STRING_UNDEFINED;

   if (mSolver == NULL)
      //throw ParameterException("Solver object not set");
      //Find the solver to use
      GetSolver();

   if (mSolver)
   {

	   if (str == "Status")
	   {
	      Integer status = mSolver->GetIntegerParameter(
	            mSolver->GetParameterID("IntegerSolverStatus"));

	      switch (status)
	      {
	      case Gmat::CREATED:
	      case Gmat::COPIED:
	         retval = "Ready";
	         break;

	      case Gmat::INITIALIZED:
	         retval = "Initialized";
	         break;

	      case Gmat::RUN:
	         retval = "Running";
	         break;

	      case Gmat::CONVERGED:
	         retval = "Converged";
	         break;

	      case Gmat::EXCEEDED_ITERATIONS:
	         retval = "ExceededIterations";
	         break;

	      case Gmat::FAILED:
	         retval = "DidNotConverge";
	         break;

	      case Gmat::UNKNOWN_STATUS:
	      default:
	         break;
	      }
	   }

   }
   else {
      retval = "SolverNotFound";
   }

   solverStatuses.push_back(retval);
   return retval;
}

//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase* param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 *
 * @return true of the objects are in place and ready, false if not
 */
//------------------------------------------------------------------------------
bool SolverData::ValidateRefObjects(GmatBase* param)
{
   bool retval = false;

   Integer objCount = 0;
   for (Integer i = 0; i < SolverDataObjectCount; ++i)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == SolverDataObjectCount)
      retval = true;

   return retval;
}

//------------------------------------------------------------------------------
// const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of valid object types
 *
 * @return The list
 */
//------------------------------------------------------------------------------
const std::string* SolverData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}

//------------------------------------------------------------------------------
// void InitializeRefObjects()
//------------------------------------------------------------------------------
/**
 * Prepares for evaluation of the data
 */
//------------------------------------------------------------------------------
void SolverData::InitializeRefObjects()
{
   mSolver = (Solver*)(FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLVER]));
   if (mSolver == NULL)
      throw ParameterException
         ("SolverData::InitializeRefObjects() Cannot find Solver object.\n"
          "Make sure Solver is set to any unnamed parameters\n");
}

//------------------------------------------------------------------------------
// bool SolverData::IsValidObjectType(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool SolverData::IsValidObjectType(UnsignedInt type)
{
   bool retval = false;

   for (int i = 0; i < SolverDataObjectCount; ++i)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool AddRefObject(const UnsignedInt type, const std::string &name,
//                   GmatBase *obj = NULL, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds object which is used in evaluation.
 *
 * @param type The type of the object
 * @param name The name of the object
 * @param obj The object
 * @param replaceName Flag indicating if this object replaces a previous one
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool SolverData::AddRefObject(const UnsignedInt type, const std::string &name,
                           GmatBase *obj, bool replaceName)
{
   if (type == Gmat::SOLVER)
   {
      //instead of having msolver be a single pointer, maybe have a collection of pointers and go to see which changed
      //mSolver = (Solver*)obj;
      mSolver = NULL;
      mSolvers.push_back((Solver*) obj);
      //solverStatuses.push_back()
      //MessageInterface::ShowMessage("In SolverData %p, adding %s at %p\n", this, name.c_str(), obj);
   }

   return RefData::AddRefObject(type, name, obj, replaceName);
}

void SolverData::GetSolver()
{
   for (int i = mSolvers.size() - 1; i>=0; --i)
   {
      if (mSolvers[i]->GetIntegerParameter(mSolvers[i]->GetParameterID("IntegerSolverStatus")) != Gmat::INITIALIZED)
      {
         mSolver = mSolvers[i];
         break;
      }
   }
   if (mSolver == NULL && mSolvers.size() > 0)
      mSolver = mSolvers[0];
   mSolvers.clear();
}
