//------------------------------------------------------------------------------
//                              OptimizerBase
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
// Author: Youngkwang Kim / Yonsei University
// Created: 2017.09.28
//
/**
 * the base class for SNOPT/IPOPT optimizer classes
 */
//------------------------------------------------------------------------------

#include "OptimizerBase.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_OPTIMIZER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
OptimizerBase::OptimizerBase(Trajectory* trajectory_in) :
	traj    (trajectory_in)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OptimizerBase::OptimizerBase(const OptimizerBase &copy) :
   traj    (copy.traj)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OptimizerBase& OptimizerBase::operator=(const OptimizerBase &copy)
{
   if (&copy == this)
      return *this;
   
   traj    = copy.traj;

   return *this;   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OptimizerBase::~OptimizerBase()
{
   // nothing to do here 
}