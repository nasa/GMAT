//$Id$
//------------------------------------------------------------------------------
//                           Breakpoint
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Feb 12, 2021
/**
 * Breakpoint command used to stop GMAT execution immediately before a command
 * executes
 */
//------------------------------------------------------------------------------


#include "../../gui/debugger/Breakpoint.hpp" // class's header file

#include "ScriptInterpreter.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"
#include "SpaceObject.hpp"

InspectorPanel *Breakpoint::debugger = NULL;

//------------------------------------------------------------------------------
//  Breakpoint()
//------------------------------------------------------------------------------
/**
 * Constructs the Breakpoint command (default constructor).
 */
//------------------------------------------------------------------------------
Breakpoint::Breakpoint() :
    GmatCommand("Breakpoint")
{
   generatingString = "Breakpoint";
}


//------------------------------------------------------------------------------
//  ~Breakpoint()
//------------------------------------------------------------------------------
/**
 * Destroys the Breakpoint command (default constructor).
 */
//------------------------------------------------------------------------------
Breakpoint::~Breakpoint()
{
}


//------------------------------------------------------------------------------
//  Breakpoint(const Breakpoint& breakpoint)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the Breakpoint command (copy constructor).
 *
 * @param breakpoint The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
Breakpoint::Breakpoint(const Breakpoint& breakpoint) :
    GmatCommand (breakpoint)
{
   generatingString = breakpoint.generatingString;
}


//------------------------------------------------------------------------------
//  Breakpoint& operator=(const Breakpoint &breakpoint)
//------------------------------------------------------------------------------
/**
 * Sets this Breakpoint to match another one (assignment operator).
 *
 * @param breakpoint The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
Breakpoint& Breakpoint::operator=(const Breakpoint &breakpoint)
{
   if (this != &breakpoint)
   {
      GmatCommand::operator=(breakpoint);
      generatingString = breakpoint.generatingString;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the Breakpoint command (copy constructor).
 *
 * Like the name implies, Breakpoint is a null operation -- nothing is done in this
 * command.  It functions as a place holder, and as the starting command in the
 * command sequence managed by the Moderator.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool Breakpoint::Execute()
{
   // Generate the command summary before doing anything else
   BuildCommandSummary();

   if (debugger == NULL)
   {
      debugger = new InspectorPanel();
      debugger->SetCommand(GetPrevious());
      debugger->ShowModal();

      ClearDebugger();
   }
   else
      debugger->SetCommand(GetPrevious());

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Breakpoint.
 *
 * @return clone of the Breakpoint.
 */
//------------------------------------------------------------------------------
GmatBase* Breakpoint::Clone() const
{
   return (new Breakpoint(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
/**
 * We don't save breakpoints to script
 */
const std::string& Breakpoint::GetGeneratingString(Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = "";
   return generatingString;
//   generatingString = prefix + "Breakpoint;";
//   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


void Breakpoint::GetObjectData(const std::string &objName)
{
   GmatBase* theObject = Moderator::Instance()->GetInternalObject(objName);

   if (theObject)
   {
      std::string theData = theObject->GetGeneratingString(Gmat::NO_COMMENTS);
      MessageInterface::PopupMessage(Gmat::INFO_, ("%s\n", theData.c_str()));
   }
}
