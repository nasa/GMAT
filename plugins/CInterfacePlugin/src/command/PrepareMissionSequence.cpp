//------------------------------------------------------------------------------
//                           PrepareMissionSequence
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
// Author: Darrel J. Conway
// Created: 2010/03/30
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------


#include "PrepareMissionSequence.hpp" // class's header file
#include "Propagate.hpp"


//------------------------------------------------------------------------------
//  PrepareMissionSequence()
//------------------------------------------------------------------------------
/**
 * Constructs the PrepareMissionSequence command (default constructor).
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::PrepareMissionSequence() :
    GmatCommand("PrepareMissionSequence")
{
   objectTypeNames.push_back("PrepareMissionSequence");
}


//------------------------------------------------------------------------------
//  ~PrepareMissionSequence()
//------------------------------------------------------------------------------
/**
 * Destroys the PrepareMissionSequence command.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::~PrepareMissionSequence()
{
}


//------------------------------------------------------------------------------
//  PrepareMissionSequence(const PrepareMissionSequence& bms)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the PrepareMissionSequence command (copy constructor).
 *
 * @param bms The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::PrepareMissionSequence(const PrepareMissionSequence& bms) :
    GmatCommand (bms)
{
}


//------------------------------------------------------------------------------
//  PrepareMissionSequence& operator=(const PrepareMissionSequence& bms)
//------------------------------------------------------------------------------
/**
 * Sets this PrepareMissionSequence to match another one (assignment operator).
 * 
 * @param bms The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence& PrepareMissionSequence::operator=(
      const PrepareMissionSequence& bms)
{
   if (this != &bms)
   {
      GmatCommand::operator=(bms);
   }
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the PrepareMissionSequence command (copy constructor).
 * 
 * PrepareMissionSequence is a null operation -- nothing is done in this
 * command.  It functions to toggle GMAT into command mode when needed.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool PrepareMissionSequence::Execute()
{
   BuildCommandSummary(true);
//   GmatCommand *cmd = GetNext();
//   while (cmd != NULL)
//   {
//      if (cmd->GetTypeName() == "Propagate")
//      {
//         ((Propagate*)cmd)->PrepareToPropagate();
//         break;
//      }
//      cmd = cmd->GetNext();
//   }
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PrepareMissionSequence.
 *
 * @return clone of the PrepareMissionSequence.
 */
//------------------------------------------------------------------------------
GmatBase* PrepareMissionSequence::Clone() const
{
   return (new PrepareMissionSequence(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
bool PrepareMissionSequence::RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName)
{
   return true;
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& PrepareMissionSequence::GetGeneratingString(
                                             Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = prefix + "PrepareMissionSequence;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

