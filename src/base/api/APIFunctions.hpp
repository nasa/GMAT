//$Id$
//------------------------------------------------------------------------------
//                           APIFunctions
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 16, 2018
/**
 * Functions that provide the top level GMAT API interfaces.
 */
//------------------------------------------------------------------------------


#ifndef APIFunctions_hpp
#define APIFunctions_hpp

#include <string>

#include "gmatdefs.hpp"
#include "HelpSystem.hpp"
#include "Moderator.hpp"
#include "TimeSystemConverter.hpp"
#include "GmatBase.hpp"

// API specific functions
GMAT_API std::string    Help(std::string forItem = "");
GMAT_API std::string    ShowObjectsForID(UnsignedInt type = Gmat::UNKNOWN_OBJECT);
GMAT_API std::string    ShowClassesForID(UnsignedInt type = Gmat::UNKNOWN_OBJECT);
GMAT_API std::string    ShowObjects(const std::string &type = "");
GMAT_API std::string    ShowClasses(const std::string &type = "UnknownObject");
GMAT_API bool           Exists(const std::string &objectName);
GMAT_API GmatBase*      GetObject(const std::string &objectname);
GMAT_API SolarSystem*   GetSolarSystem();
GMAT_API std::vector<GmatCommand*>   GetCommands(const std::string &type = "");
GMAT_API std::vector<GmatCommand*>   GetNestedCommands(GmatCommand *cmd, const std::string &type = "");

GMAT_API Integer Execute(Integer sandboxNum=1);

// Functions used for script driven work flows
GMAT_API bool           LoadScript(const std::string &filename);
GMAT_API bool           LoadInclude(const std::string &filename);
GMAT_API bool           RunScript();
GMAT_API bool           SaveScript(const std::string &filename);
GMAT_API GmatBase*      GetRuntimeObject(const std::string &objectname);
GMAT_API std::string    GetRunSummary();

// Engine access functions
GMAT_API void           Setup(const std::string &theStartupFile = "gmat_startup_file.txt");
GMAT_API GmatBase*      Construct(const std::string &type, const std::string &name = "",
      const std::string &extraData1 = "", const std::string &extraData2 = "",
      const std::string &extraData3 = "", const std::string &extraData4 = "");
GMAT_API GmatBase*      Copy(const GmatBase *theObject, const std::string &name = "");
GMAT_API void           Initialize(std::string forObject = "");
GMAT_API void           Update(const std::string &name = "",
      const std::string &extraData1 = "", const std::string &extraData2 = "",
      const std::string &extraData3 = "", const std::string &extraData4 = "");
GMAT_API void           Update(GmatBase *theObject,
      const std::string &extraData1 = "", const std::string &extraData2 = "",
      const std::string &extraData3 = "", const std::string &extraData4 = "");
GMAT_API std::string    Clear(std::string forObject = "");
GMAT_API void           UseLogFile(std::string logFile = "GmatAPILog.txt");
GMAT_API void           EchoLogFile(bool echo = true);

// Interpreter access functions
GMAT_API GmatCommand*      Command(const std::string &type, const std::string &desc = "");
GMAT_API GmatCommand*      DeleteCommand(GmatCommand *cmd, Integer sandboxNum = 1);


// Internal functions - not (yet) exported on Windows, so no GMAT_API macro
void ProcessParameters(GmatBase *theObject, const std::string &extraData1,
      const std::string &extraData2 = "", const std::string &extraData3 = "",
      const std::string &extraData4 = "");
void ProcessUpdateParameters(GmatBase *theObject, const std::string &extraData1,
   const std::string &extraData2 = "", const std::string &extraData3 = "",
   const std::string &extraData4 = "");

#endif /* APIFunctions_hpp */
