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
 * Interface code used for GUI plugin components, Base class.
 */
//------------------------------------------------------------------------------

#include "GuiInterface.hpp"


/// Singleton instance, set by the derived GUI specific code
GuiInterface *GuiInterface::instance = NULL;



//------------------------------------------------------------------------------
// GuiInterface* GuiInterface::Instance()
//------------------------------------------------------------------------------
/**
 * Singleton access (static) method
 *
 * @return The singleton pointer
 */
//------------------------------------------------------------------------------
GuiInterface* GuiInterface::Instance()
{
   return instance;
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string& ofType, const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Object creation method
 *
 * This method calls the GMAT Interpreter to create the requested object.
 * Derived GuiInterfaces call this base class method, and then use the returned
 * pointer to process the new object for the GUI.  One example: in wxWidgets,
 * the WxGuiInterface takes the returned pointer and uses it to update the
 * GMAT ResourceTree with an entry for the new object.
 *
 * @param ofType   The object type to be created.  This string matches the type
 *                 string from the supporting factory.
 * @param withName The name of the object that gets created.
 *
 * @return The new object pointer
 */
//------------------------------------------------------------------------------
GmatBase* GuiInterface::CreateObject(const std::string& ofType,
      const std::string& withName)
{
   GmatBase *retval = NULL;

   if (theInterpreter)
      retval = theInterpreter->CreateObject(ofType, withName);

   return retval;
}

// Hidden methods to enforce the singleton behavior

//------------------------------------------------------------------------------
// GuiInterface()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
GuiInterface::GuiInterface() :
      theInterpreter       (NULL)
{
}

//------------------------------------------------------------------------------
// ~GuiInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GuiInterface::~GuiInterface()
{
}
