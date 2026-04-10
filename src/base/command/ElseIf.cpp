//$Id$
//------------------------------------------------------------------------------
//                                ElseIf
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan
// Created: 2004/10/21
//
/**
 * Implementation for the ElseIf command class
 */
//------------------------------------------------------------------------------

//******************************************************************************
//******************************************************************************
//@todo
// ElseIf does not work yet. (2008.08.29)
// The workaround is to use nested If-Else statements.
// The work that needs to be done concerns the conditions, IIRC - WCS
//******************************************************************************
//******************************************************************************

#include "ElseIf.hpp"

//------------------------------------------------------------------------------
//  ElseIf()
//------------------------------------------------------------------------------
/**
 * Creates a ElseIf command.  (default constructor)
 */
//------------------------------------------------------------------------------
ElseIf::ElseIf() :
    GmatCommand      ("ElseIf")
{
}

//------------------------------------------------------------------------------
//  ElseIf(const ElseIf& ec)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an ElseIf command.  (Copy constructor)
 *
 * @param ec Elseif to use to construct 'this' ElseIf.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseIf::ElseIf(const ElseIf& ec) :
    GmatCommand   (ec)
{
}

//------------------------------------------------------------------------------
//  ElseIf& operator=(const ElseIf& ec)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Elseif command.
 *
 * @param ec Elseif whose values to use to construct 'this' ElseIf.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ElseIf& ElseIf::operator=(const ElseIf& ec)
{
    if (this == &ec)
        return *this;

    GmatCommand::operator=(ec);
    return *this;
}

//------------------------------------------------------------------------------
//  ~ElseIf()
//------------------------------------------------------------------------------
/**
 * Destroys the ElseIf command.  (destructor)
 */
//------------------------------------------------------------------------------
ElseIf::~ElseIf()
{}

//------------------------------------------------------------------------------
//  bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the command cmd after the command prev.
 *
 * @param cmd  Command to insert.
 *
 * @param prev Command after which to insert the command cmd.
 *
 * @return     true if the cmd is inserted, false otherwise.
 */
//------------------------------------------------------------------------------
bool ElseIf::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if we've gotten to this point, we should have inserted it into the front
   // of the next branch
   
   return false;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the ElseIf statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ElseIf::Initialize()
{
    bool retval = GmatCommand::Initialize();

    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the ElseIf statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool ElseIf::Execute()
{
    BuildCommandSummary(true);
    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ElseIf.
 *
 * @return clone of the ElseIf.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ElseIf::Clone() const
{
   return (new ElseIf(*this));
}
