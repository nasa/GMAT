//------------------------------------------------------------------------------
//                           GuiInterfaces
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// THIS COMPONENT Copyright (c) 2022, Emergent Space Technologies, Inc.
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
// Developed jointly by Emergent Space Technologies, Inc. and
// Thinking Systems, Inc. under SBIR contract NNX16CG16C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 30, 2017
/**
 * Interface code used for GUI plugin components, Base class
 */
//------------------------------------------------------------------------------

#ifndef GuiInterface_hpp
#define GuiInterface_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Interpreter.hpp"


/**
 * Base class defining interfaces plugins can access to add GUI elements to GMAT
 *
 * This class provides a pass through interface to GUI specific components.  The
 * GUI code overrides this code for GUI specific libraries.  The GuiInterface
 * component is a singleton.  There is one and inly one GuiInterface object in
 * GMAT.  That means that the GUI is assumed to implement plugins for only one
 * GUI toolkit -- e.g. wxWidgets or Qt -- and does not support dual GUI toolkits
 * on a single application.
 *
 * The derived class sets the singleton pointer.
 */
class GMAT_API GuiInterface
{
public:
   static GuiInterface *Instance();
   virtual GmatBase *CreateObject(const std::string &ofType,
         const std::string &withName);

   /// GUI element creation interface
   virtual void* CreateGuiElement(const std::string &ofType,
         const std::string &withName = "") = 0;

protected:
   static GuiInterface *instance;
   Interpreter *theInterpreter;

   GuiInterface();
   virtual ~GuiInterface();
};

#endif /* GuiInterfaces_hpp */
