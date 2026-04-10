//------------------------------------------------------------------------------
//                           UserFunctionManager
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
// Author: Wendy Shoan / NASA
// Created: 2015.08.14
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
#include <cstdlib>   // for rand
#include "UserFunctionManager.hpp"
#include "MessageInterface.hpp"
#include "LowThrustException.hpp"

//#define DEBUG_FUNCTION_RANDOM

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
UserFunctionManager::UserFunctionManager() :
   userData              (NULL),
   phaseNum              (0),
   hasFunction           (false),
   numStateVars          (0),
   numControlVars        (0),
   numTimeVars           (0),
   numStaticVars         (0),
   forceFiniteDiff       (false),
   hasCostFunction       (false),
   isInitializing        (false),
   realDistribution      (0, 1)
{
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserFunctionManager::UserFunctionManager(const UserFunctionManager &copy) :
   userData              (NULL),
   phaseNum              (copy.phaseNum),
   hasFunction           (copy.hasFunction),
   numStateVars          (copy.numStateVars),
   numControlVars        (copy.numControlVars),
   numTimeVars           (copy.numTimeVars),
   numStaticVars         (copy.numStaticVars),
   forceFiniteDiff       (copy.forceFiniteDiff),
   hasCostFunction       (copy.hasCostFunction),
   isInitializing        (copy.isInitializing),
   randGen               (copy.randGen),
   realDistribution      (copy.realDistribution)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserFunctionManager& UserFunctionManager::operator=(
                                          const UserFunctionManager &copy)
{
   if (&copy == this)
      return *this;
   
   userData         = copy.userData; // or NULL? or clone/copy constructor?
   phaseNum         = copy.phaseNum;
   hasFunction      = copy.hasFunction;
   numStateVars     = copy.numStateVars;
   numControlVars   = copy.numControlVars;
   numTimeVars      = copy.numTimeVars;
   forceFiniteDiff  = copy.forceFiniteDiff;
   hasCostFunction  = copy.hasCostFunction;
   isInitializing   = copy.isInitializing;
   numStaticVars    = copy.numStaticVars;
   randGen          = copy.randGen;
   realDistribution = copy.realDistribution;
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserFunctionManager::~UserFunctionManager()
{
   // This is provided elsewhere so it is deleted from the source instead of
   // here
   // if (userData) delete userData;
}


//------------------------------------------------------------------------------
// Rvector GetRandomVector(const Rvector &lowerBound,
//                         const Rvector &upperBound)
//------------------------------------------------------------------------------
/**
 * This method returns a random vector using the input lower and upper bounds
 *
 * @param <lowerBound>   the lower bound vector
 * @param <upperBound>   the upper bound vector
 *
 * @return a random vector of the same size as the upper and lower bounds inputs
 */
//------------------------------------------------------------------------------
Rvector UserFunctionManager::GetRandomVector(const Rvector &lowerBound,
                                             const Rvector &upperBound)
{
   #ifdef DEBUG_FUNCTION_RANDOM
      MessageInterface::ShowMessage("-=-=-=-= in GetRandomVector:\n");
      MessageInterface::ShowMessage("-=-=-=-= lowerBound = %s\n",
                                    lowerBound.ToString(12).c_str());
      MessageInterface::ShowMessage("-=-=-=-= upperBound = %s\n",
                                    upperBound.ToString(12).c_str());
   #endif
   Integer sz = upperBound.GetSize();
   Rvector random(sz);

   for (Integer ii = 0; ii < sz; ii++)
   {
      Real randf = realDistribution(randGen);
      #ifdef DEBUG_FUNCTION_RANDOM
         MessageInterface::ShowMessage(
                     "-=-=-=-=-=-=-= rand output           = %d\n", rndm);
         MessageInterface::ShowMessage(
                     "-=-=-=-=-=-=-= rand (as Real) output = %12.10f\n", randf);
      #endif
      // do we need to bound rand?  Yes, we do
      random[ii] = lowerBound[ii] +
                   (Real) (randf * (upperBound[ii] - lowerBound[ii]));
   }
   return random;
}

//------------------------------------------------------------------------------
// void SetIsInitializing(bool isInit)
//------------------------------------------------------------------------------
/**
 * This method sets a flag indicating whether or not this function manager
 * is initializing
 *
 * @param <isInit>  input flag indicating whether this manager is initializing
 */
//------------------------------------------------------------------------------
void UserFunctionManager::SetIsInitializing(bool isInit)
{
   isInitializing = isInit;
}

//------------------------------------------------------------------------------
// bool IsInitializing()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this function manager
 * is initializing
 *
 * @return true if initializing; false otherwise
 */
//------------------------------------------------------------------------------
bool UserFunctionManager::IsInitializing()
{
   return isInitializing;
}


//------------------------------------------------------------------------------
// bool HasCostFunction()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this function manager
 * has a cost function
 *
 * @return true if there is a cost function; false otherwise
 */
//------------------------------------------------------------------------------
bool UserFunctionManager::HasCostFunction()
{
   return hasCostFunction;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
