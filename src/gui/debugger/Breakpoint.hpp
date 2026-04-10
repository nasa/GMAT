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




#ifndef Breakpoint_hpp
#define Breakpoint_hpp

#include "GmatCommand.hpp" // inheriting class's header file
#include "InspectorPanel.hpp"

/**
 * Default command used to initialize the command sequence lists in the
 * Moderator
 */
class Breakpoint : public GmatCommand
{
public:
   Breakpoint();
   virtual ~Breakpoint();
   Breakpoint(const Breakpoint& noop);
   Breakpoint&             operator=(const Breakpoint &noop);

   bool                    Execute();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual const std::string&
                     GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                         const std::string &prefix = "",
                                         const std::string &useName = "");
   void GetObjectData(const std::string &objName);

   static void ClearDebugger()
   {
      debugger = NULL;
   }

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

private:
   static InspectorPanel *debugger;
};


#endif // Breakpoint_hpp
