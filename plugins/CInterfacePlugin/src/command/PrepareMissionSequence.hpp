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
// Created: 2011/03/30
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------



#ifndef PrepareMissionSequence_hpp
#define PrepareMissionSequence_hpp

#include "GmatCFunc_defs.hpp"
#include "GmatCommand.hpp"

/**
 * Command used to toggle GMAT into command mode
 *
 * When run, scripts that start the Mission Control Sequence using the
 * PrepareMissionSequence command will populate the GMAT Sandbox with the
 * objects identified in the script, and then initialize the objects and
 * commands in the Sandbox.  The script is not actually executed.  Replacing
 * BeginMissionSequence with PrepareMissionSequence may save time when
 * preparing objects for use through the C interface.
 */
class CINTERFACE_API PrepareMissionSequence : public GmatCommand
{
public:
   PrepareMissionSequence();
   virtual ~PrepareMissionSequence();
   PrepareMissionSequence(const PrepareMissionSequence& PrepareMissionSequence);
   PrepareMissionSequence& operator=(const PrepareMissionSequence &bms);
   
   bool                 Execute();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

   DEFAULT_TO_NO_CLONES
};


#endif // PrepareMissionSequence_hpp
