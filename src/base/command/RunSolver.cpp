//$Id$
//------------------------------------------------------------------------------
//                         ClassName
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
// Created: 2009/ /
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "RunSolver.hpp"

#include <sstream>
#include "MessageInterface.hpp"


//#define DEBUG_RUNSOLVER_PARSING


RunSolver::RunSolver(const std::string &typeStr) :
   PropagationEnabledCommand  (typeStr)
{
}

RunSolver::~RunSolver()
{
}

RunSolver::RunSolver(const RunSolver & rs) :
   PropagationEnabledCommand  (rs)
{
}

RunSolver & RunSolver::operator =(const RunSolver& rs)
{
   if (&rs != this)
   {
      PropagationEnabledCommand::operator=(rs);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool RunSolver::InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parser for the RunSolver commands
 *
 * The RunSolver commands all have a simple enough structure that the generic
 * command parsers should be able to handle them.  However, that functionality
 * no longer works as designed, so an implementation is provided here.  This
 * implementation assumes that the command syntax is
 *
 *    CommandKeyword SolverName
 *
 * If you need more specific text in the command scripting, override this method
 * and implement your command specific details.
 *
 * @return true if the command parsed the generating string successfully; false
 *         otherwise.
 */
//------------------------------------------------------------------------------
bool RunSolver::InterpretAction()
{
   bool retval = false;

   #ifdef DEBUG_RUNSOLVER_PARSING
      MessageInterface::ShowMessage(
            "RunSolver::InterpretAction() called for string \"%s\"\n",
            generatingString.c_str());
   #endif

   std::stringstream stringToParse;

   stringToParse << generatingString;

   std::string temp;

   stringToParse >> temp;

   #ifdef DEBUG_RUNSOLVER_PARSING
      MessageInterface::ShowMessage("   Cmd keyword: \"%s\"\n", temp.c_str());
   #endif
   stringToParse >> temp;

   #ifdef DEBUG_RUNSOLVER_PARSING
      MessageInterface::ShowMessage("   Solver Name: \"%s\"\n", temp.c_str());
   #endif

   if (temp != "")
   {
      solverName = temp;
      retval = true;
   }

   return retval;
}


bool RunSolver::Initialize()
{
   bool retval = PropagationEnabledCommand::Initialize();

   #ifdef DEBUG_RUNSOLVER_INITIALIZATION
      MessageInterface::ShowMessage("   PEC returned \"%s\"\n",
            (retval ? "true" : "false"));
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 *
 * @note This is here to catch calls from child RunSImulator/Estimator classes.
 * There are no other objects to rename at this level.
 */
//------------------------------------------------------------------------------
bool RunSolver::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return true;
}


