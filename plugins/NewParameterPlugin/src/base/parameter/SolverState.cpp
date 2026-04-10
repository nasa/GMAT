//$Id$
//------------------------------------------------------------------------------
//                           SolverState
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
// Created: Oct 9, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#include "SolverState.hpp"

//------------------------------------------------------------------------------
// SolverState(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
SolverState::SolverState(const std::string &name, GmatBase *obj) :
   SolverReal(name, "SolverState", obj, "SolverState", "", Gmat::SOLVER,
         GmatParam::NO_DEP)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ~SolverState()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolverState::~SolverState()
{
}

//------------------------------------------------------------------------------
// SolverState(const SolverState& ss)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ss The parameter that is copied to make this new one
 */
//------------------------------------------------------------------------------
SolverState::SolverState(const SolverState& ss) :
   SolverReal(ss)
{
}

//------------------------------------------------------------------------------
// SolverState& operator=(const SolverState& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ss The parameter that is copied into this one
 *
 * @return Thos parameter, set to look like ss
 */
//------------------------------------------------------------------------------
SolverState& SolverState::operator =(const SolverState& ss)
{
   if (this != &ss)
   {
      SolverReal::operator=(ss);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates the parameter
 *
 * @return true is the evaluation succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool SolverState::Evaluate()
{
   mRealValue = GmatGlobal::Instance()->GetSolverStatus(mOwnerName);

   if (mRealValue == SOLVER_REAL_UNDEFINED)
      return false;

   return true;
}

//-------------------------------------
// methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* SolverState::Clone() const
{
   return new SolverState(*this);
}
