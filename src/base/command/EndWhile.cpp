//$Id$
//------------------------------------------------------------------------------
//                             EndWhile
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
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/11
//
/**
 * Definition for the closing line of the While Statement
 */
//------------------------------------------------------------------------------
#include "EndWhile.hpp"
#include "BranchCommand.hpp"

//#define DEBUG_ENDWHILE_EXECUTE

#ifdef DEBUG_ENDWHILE_EXECUTE
   #include "MessageInterface.hpp"
#endif


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EndWhile() :
//------------------------------------------------------------------------------
/*
 * Creates an EndWhile object (default constructor)
 */
//---------------------------------------------------------------------------
EndWhile::EndWhile() :
GmatCommand         ("EndWhile")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}

//------------------------------------------------------------------------------
// EndWhile(const EndWhile& ew)
//------------------------------------------------------------------------------
/*
 * Creates an EndWhile object from the input object (copy constructor)
 *
 * @param ef  the EndFor to copy
 */
//---------------------------------------------------------------------------
EndWhile::EndWhile(const EndWhile& ew) :
GmatCommand         (ew)
{
}

//------------------------------------------------------------------------------
// EndWhile& operator=(const EndWhile& ew)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EndWhile command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
EndWhile& EndWhile::operator=(const EndWhile& ew)
{
   if (this == &ew)
      return *this;
   GmatCommand::operator=(ew);

   return *this;
}

//------------------------------------------------------------------------------
// ~EndWhile()
//------------------------------------------------------------------------------
/**
 * Destroys this EndWhile instance (destructor)
 */
//------------------------------------------------------------------------------
EndWhile::~EndWhile()
{
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes this EndWhile command.
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndWhile::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning for command
   if (!next)
      throw CommandException("EndWhile Command not properly reconnected");

   if (next->GetTypeName() != "While")
      throw CommandException("EndWhile Command not connected to While Command");

   return true;
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes this EndWhile command.
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndWhile::Execute()
{
   BuildCommandSummary(true);
   #ifdef DEBUG_ENDWHILE_EXECUTE
      MessageInterface::ShowMessage("In EndWhile::Execute, next is a %s\n",
      (next->GetTypeName()).c_str());
      MessageInterface::ShowMessage("---- that is, in EndWhile %p, object While is %p\n",
      this, next);
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the input command into the command sequences after the specified
 * command.
 *
 * @param   cmd   command to insert
 * @param   prev  command after which to insert the command cmd
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool EndWhile::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   #ifdef DEBUG_ENDWHILE_EXECUTE
      MessageInterface::ShowMessage(
      "In EndWhile::Insert, calling InsertRightAfter(), next is a %s\n",
      (next->GetTypeName()).c_str());
   #endif
   if (this == prev) return ((BranchCommand*)next)->InsertRightAfter(cmd);
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
bool EndWhile::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   // There are no renameable objects
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndWhile.
 *
 * @return clone of the EndWhile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndWhile::Clone() const
{
   return (new EndWhile(*this));
}

//------------------------------------------------------------------------------
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useNam)
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
const std::string& EndWhile::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "EndWhile;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
