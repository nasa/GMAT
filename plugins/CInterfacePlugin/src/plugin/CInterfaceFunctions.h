//------------------------------------------------------------------------------
//                         GmatCInterfaceFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
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
// Developed jointly by NASA/GSFC, Emergent Space Technologies, Inc.
// and Thinking Systems, Inc. under the FDSS contract, Task 28
//
// Author: Darrel J. Conway (Thinking Systems)
// Created: 2011/05/17
//
// Note: Since this is pure C code, the file header excludes the Doxygen comment
//       tag usually part of GMAT file prefaces.
/* *
 * Functions called on the client (e.g. ODTBX) side of the interface
 *
 * This file is parsed by MATLAB to define the functions that are accessed using 
 * MATLAB's loadlibrary/calllib functions. These are all pure C functions; the 
 * file is loaded by CInterfacePluginFunctions.hpp when building the interface
 * library.
 */
//------------------------------------------------------------------------------

#ifndef GmatCInterfaceFunctions_hpp
#define GmatCInterfaceFunctions_hpp

#include "GmatCFunc_defs.hpp"

// Interfaces used by the client
const char CINTERFACE_API *getLastMessage();
int CINTERFACE_API StartGmat();

int CINTERFACE_API LoadScript(const char* scriptName);
int CINTERFACE_API RunScript();
int CINTERFACE_API LoadAndRunScript(const char* scriptName);

int CINTERFACE_API FindOdeModel(const char* modelName);
int CINTERFACE_API SetModel(int modelID);
int CINTERFACE_API SetModelByName(const char* modelName);

int CINTERFACE_API GetStateSize();
const char CINTERFACE_API *GetStateDescription();
int CINTERFACE_API SetState(double epoch, double state[], int stateDim);
double CINTERFACE_API *GetState();

double CINTERFACE_API *GetDerivativesForState(double epoch, double state[], 
      int stateDim, double dt, int order, int *pdim);
double CINTERFACE_API *GetDerivatives(double dt, int order, int *pdim);
//double CINTERFACE_API *GetDerivatives(double dt, int order);

int CINTERFACE_API CountObjects();
const char CINTERFACE_API *GetObjectName(int which);
const char CINTERFACE_API *GetRunSummary();

void CINTERFACE_API (*GetPluginFunction(const char* funName, const char* libraryName))();

#endif /*GmatCInterfaceFunctions_hpp*/
