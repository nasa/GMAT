//$Id$
//------------------------------------------------------------------------------
//                              BeginScript
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
 * Script tag used to indicate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------



#ifndef BeginScript_hpp
#define BeginScript_hpp

#include "GmatCommand.hpp" // inherited class's header file

/**
 * Script tag used to indicate the start of a block of script that shows up
 * verbatim in a ScriptEvent panel on the GUI.
 */
class GMAT_API BeginScript : public GmatCommand
{
public:
   BeginScript();
   virtual ~BeginScript();
   BeginScript(const BeginScript& copy);
   BeginScript&         operator=(const BeginScript& copy);
   
   bool                 Execute();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   DEFAULT_TO_NO_CLONES
protected:
   
   void IndentChildString(std::stringstream &gen, GmatCommand* cmd, 
                          std::string &indent, Gmat::WriteMode mode,
                          const std::string &prefix, const std::string &useName,
                          bool indentCommentOnly);
   void IndentComment(std::stringstream &gen, std::string &comment,
                      const std::string &prefix);
};

#endif // BeginScript_hpp
