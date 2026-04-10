//$Id$
//------------------------------------------------------------------------------
//                           SolverStatus
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

#include "SolverStatus.hpp"

//------------------------------------------------------------------------------
// SolverStatus(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name name of the parameter
 * @param obj reference object pointer
 */
//------------------------------------------------------------------------------
SolverStatus::SolverStatus(const std::string &name, GmatBase *obj) :
   SolverString(name, "SolverStatus", obj, "SolverStatus", "")
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// ~SolverStatus()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolverStatus::~SolverStatus()
{
}

//------------------------------------------------------------------------------
// SolverStatus(const SolverStatus& ss)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ss The parameter that is copied to make this new one
 */
//------------------------------------------------------------------------------
SolverStatus::SolverStatus(const SolverStatus& ss) :
   SolverString(ss)
{
}

//------------------------------------------------------------------------------
// SolverStatus& operator=(const SolverStatus& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ss The parameter that is copied into this one
 *
 * @return Thos parameter, set to look like ss
 */
//------------------------------------------------------------------------------
SolverStatus& SolverStatus::operator =(const SolverStatus& ss)
{
   if (this != &ss)
   {
      SolverString::operator=(ss);
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
bool SolverStatus::Evaluate()
{
//   mStringValue = SolverData::GetSolverString("Status");
   mStringValue = GmatGlobal::Instance()->GetSolverStatusString(mOwnerName);

   if (mStringValue == SOLVER_STRING_UNDEFINED)
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
GmatBase* SolverStatus::Clone() const
{
   return new SolverStatus(*this);
}
