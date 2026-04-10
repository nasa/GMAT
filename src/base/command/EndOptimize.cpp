//$Id$
//------------------------------------------------------------------------------
//                                EndOptimize 
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
// Author:  Daniel Hunter/GSFC/MAB (CoOp)
// Created: 2006.07.20
//
/**
 * Implementation for the EndOptimize command class
 */
//------------------------------------------------------------------------------

#include "EndOptimize.hpp"
#include "BranchCommand.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_OPTIMIZER_COMMANDS

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//VC++ error C2466: cannot allocate an array of constant size 0
// so commented out for possible later use
//const std::string
//EndOptimize::PARAMETER_TEXT[EndOptimizeParamCount - GmatCommandParamCount] = {};
//const Gmat::ParameterType
//EndOptimize::PARAMETER_TYPE[EndOptimizeParamCount - GmatCommandParamCount] = {};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
EndOptimize::EndOptimize() :
   GmatCommand         ("EndOptimize")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
   parameterCount = EndOptimizeParamCount;
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
EndOptimize::EndOptimize(const EndOptimize& eo) :
   GmatCommand         (eo)
{
}

//------------------------------------------------------------------------------
// operator =
//------------------------------------------------------------------------------
EndOptimize& EndOptimize::operator=(const EndOptimize& eo)
{
   if (this == &eo)
      return *this;
    
   return *this;
}
    

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
EndOptimize::~EndOptimize()
{
   // nothing to do here at the moment ...... la de da de da
}
    
//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
bool EndOptimize::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning Optimize command
   if (!next)
      throw CommandException("EndOptimize Command not properly reconnected");
    
   if (next->GetTypeName() != "Optimize")
      throw CommandException("EndOptimize Command not connected to Optimize Command");
    
   return true;
}

//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
bool EndOptimize::Execute()
{
   #ifdef DEBUG_OPTIMIZER_COMMANDS
      if (next)
         MessageInterface::ShowMessage(
            "End Optimize points to a %s command\n", next->GetTypeName().c_str());
      else
         MessageInterface::ShowMessage(
            "EndOptimize does not reconnect to Optimize comamnd\n");
   #endif
   
   BuildCommandSummary(true);
   return true;
}

//------------------------------------------------------------------------------
// Insert
//------------------------------------------------------------------------------
bool EndOptimize::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire Optimize command
   if (this == prev)
      return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}

//------------------------------------------------------------------------------
// Clone
//------------------------------------------------------------------------------
GmatBase* EndOptimize::Clone() const
{
   return (new EndOptimize(*this));
}

//------------------------------------------------------------------------------
// GetGeneratingString
//------------------------------------------------------------------------------
const std::string& EndOptimize::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   generatingString = prefix + "EndOptimize;";
   if (mode == Gmat::NO_COMMENTS)
   {
	  InsertCommandName(generatingString);
      return generatingString;
   }
   
   if ((next) && (next->GetTypeName() == "Optimize"))
   {
      if (showInlineComment)
      {
         // To avoid keep appending, check for empty inline comment
         if (GetInlineComment() == "")
         {
            generatingString += "  % For optimizer ";
            generatingString += next->GetRefObjectName(Gmat::SOLVER);
         }
      }
   }
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
// bool RenameRefObject()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool EndOptimize::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   // There are no renamealbe objects
   return true;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

