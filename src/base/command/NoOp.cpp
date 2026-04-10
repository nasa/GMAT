//$Id$
//------------------------------------------------------------------------------
//                                  NoOp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Null operator for the command sequence -- typically used as a place holder
 */
//------------------------------------------------------------------------------


#include "NoOp.hpp" // class's header file


//------------------------------------------------------------------------------
//  NoOp()
//------------------------------------------------------------------------------
/**
 * Constructs the NoOp command (default constructor).
 */
//------------------------------------------------------------------------------
NoOp::NoOp() :
    GmatCommand("NoOp")
{
   includeInSummary = false;
}


//------------------------------------------------------------------------------
//  ~NoOp()
//------------------------------------------------------------------------------
/**
 * Destroys the NoOp command (default constructor).
 */
//------------------------------------------------------------------------------
NoOp::~NoOp()
{
}


//------------------------------------------------------------------------------
//  NoOp(const NoOp& noop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the NoOp command (copy constructor).
 *
 * @param noop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
NoOp::NoOp(const NoOp& noop) :
    GmatCommand (noop)
{
}


//------------------------------------------------------------------------------
//  NoOp& operator=(const NoOp&)
//------------------------------------------------------------------------------
/**
 * Sets this NoOp to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
NoOp& NoOp::operator=(const NoOp& noop)
{
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the NoOp command (copy constructor).
 * 
 * Like the name implies, NoOp is a null operation -- nothing is done in this 
 * command.  It functions as a place holder, and as the starting command in the 
 * command sequence managed by the Moderator.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool NoOp::Execute()
{
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NoOp.
 *
 * @return clone of the NoOp.
 */
//------------------------------------------------------------------------------
GmatBase* NoOp::Clone() const
{
   return (new NoOp(*this));
}
