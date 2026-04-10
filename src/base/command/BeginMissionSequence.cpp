//$Id$
//------------------------------------------------------------------------------
//                           BeginMissionSequence
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
// Created: 2010/05/21
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------


#include "BeginMissionSequence.hpp" // class's header file


//------------------------------------------------------------------------------
//  BeginMissionSequence()
//------------------------------------------------------------------------------
/**
 * Constructs the BeginMissionSequence command (default constructor).
 */
//------------------------------------------------------------------------------
BeginMissionSequence::BeginMissionSequence() :
    GmatCommand("BeginMissionSequence")
{
   objectTypeNames.push_back("BeginMissionSequence");
   includeInSummary = false;
}


//------------------------------------------------------------------------------
//  ~BeginMissionSequence()
//------------------------------------------------------------------------------
/**
 * Destroys the BeginMissionSequence command.
 */
//------------------------------------------------------------------------------
BeginMissionSequence::~BeginMissionSequence()
{
}


//------------------------------------------------------------------------------
//  BeginMissionSequence(const BeginMissionSequence& BeginMissionSequence)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the BeginMissionSequence command (copy constructor).
 *
 * @param BeginMissionSequence The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
BeginMissionSequence::BeginMissionSequence(const BeginMissionSequence& bms) :
    GmatCommand (bms)
{
}


//------------------------------------------------------------------------------
//  BeginMissionSequence& operator=(const BeginMissionSequence&)
//------------------------------------------------------------------------------
/**
 * Sets this BeginMissionSequence to match another one (assignment operator).
 * 
 * @param BeginMissionSequence The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
BeginMissionSequence& BeginMissionSequence::operator=(
      const BeginMissionSequence& bms)
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
 * Executes the BeginMissionSequence command (copy constructor).
 * 
 * BeginMissionSequence is a null operation -- nothing is done in this
 * command.  It functions to toggle GMAT into command mode when needed.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool BeginMissionSequence::Execute()
{
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginMissionSequence.
 *
 * @return clone of the BeginMissionSequence.
 */
//------------------------------------------------------------------------------
GmatBase* BeginMissionSequence::Clone() const
{
   return (new BeginMissionSequence(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
bool BeginMissionSequence::RenameRefObject(const UnsignedInt type,
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
const std::string& BeginMissionSequence::GetGeneratingString(
                                             Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = prefix + "BeginMissionSequence;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

