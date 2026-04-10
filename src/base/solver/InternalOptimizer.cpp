//$Id$
//------------------------------------------------------------------------------
//                             InternalOptimizer
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2007/03/27
//
/**
 * Implementation of the internal optimizer. 
 */
//------------------------------------------------------------------------------


#include "InternalOptimizer.hpp"

//-----------------------------------------
// public methods
//-----------------------------------------


//------------------------------------------------------------------------------
// InternalOptimizer()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::InternalOptimizer(std::string type, std::string name) :
   Optimizer      (type, name)
{
   objectTypeNames.push_back("InternalOptimizer");
}

//------------------------------------------------------------------------------
// InternalOptimizer()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::~InternalOptimizer()
{
}

//------------------------------------------------------------------------------
// InternalOptimizer(const InternalOptimizer &opt)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::InternalOptimizer(const InternalOptimizer &opt) :
   Optimizer      (opt)
{
}

//------------------------------------------------------------------------------
// InternalOptimizer& operator=(const InternalOptimizer &jac)
//------------------------------------------------------------------------------
/**
 * InternalOptimizer assignment operator
 * 
 * Copies the data from another InternalOptimizer instance into this one.
 * 
 * @param jac The supplier of the InternalOptimizer data.
 * 
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
InternalOptimizer& InternalOptimizer::operator=(const InternalOptimizer& opt)
{
   if (&opt != this)
   {
      Optimizer::operator=(opt);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method used to set up the InternalOptimizer data structures prior to use.
 * 
 * @return true if initialization succeeds
 */
//------------------------------------------------------------------------------
bool InternalOptimizer::Initialize()
{
   bool retval = Optimizer::Initialize();
   
   return retval;
}

//------------------------------------------------------------------------------
//  void ReportProgress()
//------------------------------------------------------------------------------
/**
 * Shows the progress string to the user.
 *
 * This default version just passes the progress string to the MessageInterface.
 */
//------------------------------------------------------------------------------
void InternalOptimizer::ReportProgress(const SolverState forState)
{
   SolverState stateBuffer = currentState;

   if (forState != UNDEFINED_STATE)
      currentState = forState;

   if (currentState == CHECKINGRUN)
   {
      // Update achieved values
   }

   currentState = stateBuffer;

   Solver::ReportProgress(forState);
}

