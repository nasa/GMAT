//$Id$
//------------------------------------------------------------------------------
//                           TestScriptInterpreter driver
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
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Function prototypes for TestScriptInterpreter.
 */
//------------------------------------------------------------------------------


#ifndef driver_hpp
#define driver_hpp

#include <iostream>
#include "gmatdefs.hpp"


enum {
        RUN_SCRIPT = 2001,
        RUN_BATCH,
        SAVE,
        SUMMARY,
        PARSE,
        HELP,
        VERBOSE
     };

void ShowHelp();
void RunScriptInterpreter(std::string script, int verbosity, 
                          bool batchmode = false);
Integer RunBatch(std::string& batchfilename);
void SaveScript(std::string filename = "");
void ShowVersionInfo();
void ShowCommandSummary(std::string filename = "");
void DumpDEData(double secsToStep, double spanInSecs = 86400.0);

StringArray CheckForStartupAndLogFile(int argc, char *argv[]);

int main(int argc, char *argv[]);

#endif /* driver_hpp */
