//$Id$
//------------------------------------------------------------------------------
//                             EndIf
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
// Author:  Joey Gurganus
// Created: 2004/01/29
//
/**
 * Implementation for the closing line of the ENDIF Statement
 */
//------------------------------------------------------------------------------


#include "EndIf.hpp"
#include "BranchCommand.hpp"


//------------------------------------------------------------------------------
//  EndIf()
//------------------------------------------------------------------------------
/**
 * Creates an EndIf command.  (default constructor)
 */
//------------------------------------------------------------------------------
EndIf::EndIf() :
    GmatCommand         ("EndIf")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}

//------------------------------------------------------------------------------
//  EndIf(const EndIf& ec)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an EndIf command.  (Copy constructor)
 *
 * @param ec EndIf to use to construct 'this' EndIf.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
EndIf::EndIf(const EndIf& ic) :
    GmatCommand         (ic)
{}

//------------------------------------------------------------------------------
//  EndIf& operator=(const EndIf& ec)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the EndIf command.
 *
 * @param ec EndIf whose values to use to construct 'this' EndIf.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
EndIf& EndIf::operator=(const EndIf& ic)
{
    if (this == &ic)
        return *this;
    GmatCommand::operator=(ic);
    
    return *this;
}
    
//------------------------------------------------------------------------------
//  ~EndIf()
//------------------------------------------------------------------------------
/**
 * Destroys the EndIf command.  (destructor)
 */
//------------------------------------------------------------------------------
EndIf::~EndIf()
{}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the EndIf statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool EndIf::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning If command
   if (!next)
      throw CommandException("EndIf Command not properly reconnected");
   
   // @todo remove references to Else and ElseIf, if EndIf can only be connected
   //       to If.
   if (next->GetTypeName() != "If" && next->GetTypeName() != "Else" &&
       next->GetTypeName() != "ElseIf")
      throw CommandException("EndIf Command not connected to IF Command");
   
   return true;    
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the EndIf statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool EndIf::Execute()
{
   BuildCommandSummary(true);
   return true;
}

//------------------------------------------------------------------------------
//  bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts the command cmd after the command prev.
 *
 * @param cmd Command to insert.
 *
 * @param prev Command after which to insert the command cmd.
 *
 * @return true if the cmd is inserted, false otherwise.
 */
//------------------------------------------------------------------------------
bool EndIf::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
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
bool EndIf::RenameRefObject(const UnsignedInt type,
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
 * This method returns a clone of the EndIf.
 *
 * @return clone of the EndIf.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndIf::Clone() const
{
   return (new EndIf(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
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
const std::string& EndIf::GetGeneratingString(Gmat::WriteMode mode,
                                              const std::string &prefix,
                                              const std::string &useName)
{
   generatingString = prefix + "EndIf;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}
