//$Id$
//------------------------------------------------------------------------------
//                              EndScript
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
// Created: 2004/02/25
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Script tag used to terminate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------


#include "EndScript.hpp" // class's header file


//------------------------------------------------------------------------------
//  EndScript()
//------------------------------------------------------------------------------
/**
 * Constructs the EndScript command (default constructor).
 */
//------------------------------------------------------------------------------
EndScript::EndScript() :
   GmatCommand("EndScript")
{
   generatingString = "EndScript;";
}


//------------------------------------------------------------------------------
//  ~EndScript()
//------------------------------------------------------------------------------
/**
 * Destroys the EndScript command (destructor).
 */
//------------------------------------------------------------------------------
EndScript::~EndScript()
{
}


//------------------------------------------------------------------------------
//  EndScript(const EndScript& copy)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the EndScript command (copy constructor).
 *
 * @param copy The original used this one.
 */
//------------------------------------------------------------------------------
EndScript::EndScript(const EndScript& copy) :
    GmatCommand (copy)
{
}


//------------------------------------------------------------------------------
//  EndScript& operator=(const EndScript& copy)
//------------------------------------------------------------------------------
/**
 * Sets this EndScript to match another one (assignment operator).
 * 
 * @param copy The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
EndScript& EndScript::operator=(const EndScript& copy)
{
   if (this != &copy)
      GmatCommand::operator=(copy);
      
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the EndScript command.
 * 
 * During mission execution, EndScript is a null operation -- nothing is done
 * in this command.  It functions as a marker in the script, indicating to the
 * GUI the end of a block of commands that should all be grouped together on a
 * ScriptEvent panel.
 */
//------------------------------------------------------------------------------
bool EndScript::Execute()
{
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndScript.
 *
 * @return clone of the EndScript.
 */
//------------------------------------------------------------------------------
GmatBase* EndScript::Clone() const
{
   return (new EndScript(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type    Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool EndScript::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return true;
}
