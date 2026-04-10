//$Id$
//------------------------------------------------------------------------------
//                                EndTarget
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
// Author: Darrel J. Conway
// Created: 2004/01/23
//
/**
 * Definition for the closing line of a Targeter loop
 */
//------------------------------------------------------------------------------


#include "EndTarget.hpp"
#include "BranchCommand.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
//  EndTarget()
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (default constructor)
 */
//------------------------------------------------------------------------------
EndTarget::EndTarget() :
   GmatCommand         ("EndTarget")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}


//------------------------------------------------------------------------------
//  ~EndTarget()
//------------------------------------------------------------------------------
/**
 * Destroys an EndTarget command.  (destructor)
 */
//------------------------------------------------------------------------------
EndTarget::~EndTarget()
{
}
    

//------------------------------------------------------------------------------
//  EndTarget(const EndTarget& et)
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (copy constructor)
 *
 * @param et The command that is copied.
 */
//------------------------------------------------------------------------------
EndTarget::EndTarget(const EndTarget& et) :
   GmatCommand         (et)
{
}


//------------------------------------------------------------------------------
//  EndTarget& operator=(const EndTarget& et)
//------------------------------------------------------------------------------
/**
 * Creates an EndTarget command.  (copy constructor)
 *
 * @param et The command that is copied.
 *
 * @return This instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
EndTarget& EndTarget::operator=(const EndTarget& et)
{
   if (this == &et)
      return *this;
    
   return *this;
}
    

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the EndTarget command.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool EndTarget::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning Target command
   if (!next)
      throw CommandException("EndTarget Command not properly reconnected");
    
   if (next->GetTypeName() != "Target")
      throw CommandException("EndTarget Command not connected to Target Command");
    
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Runs the EndTarget command.
 *
 * The EndTarget command is basically a no-op command; it just marks the end of
 * the targeting loop.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool EndTarget::Execute()
{
   #ifdef DEBUG_TARGET_COMMANDS
      if (next)
         MessageInterface::ShowMessage(
            "End Target points to a %s command\n", next->GetTypeName().c_str());
      else
         MessageInterface::ShowMessage(
            "EndTarget does not reconnect to Target comamnd\n");
   #endif
   
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts a command into the mission sequence.
 *
 * @param cmd  The command that gets inserted.
 * @param prev The command that will precede the inserted command.
 *
 * @return true.
 */
//------------------------------------------------------------------------------
bool EndTarget::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   if (this == prev)
      return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool EndTarget::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   // There are no renamealbe objects
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndTarget.
 *
 * @return clone of the EndTarget.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndTarget::Clone() const
{
   return (new EndTarget(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& EndTarget::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "EndTarget;";
	  InsertCommandName(generatingString);
      return generatingString;
   }
   
   // Build the local string
   generatingString = prefix + "EndTarget;";
   if ((next) && (next->GetTypeName() == "Target"))
   {
      // To avoid keep appending, check for empty inline comment
      if (GetInlineComment() == "")
      {
         generatingString += "  % For targeter ";
         generatingString += next->GetRefObjectName(Gmat::SOLVER);
      }
   }
   
   // Then call the base class method for preface and inline comments
   // We want preface comment to be indented
   return GmatCommand::GetGeneratingString(mode, prefix + "   ", useName);
}
