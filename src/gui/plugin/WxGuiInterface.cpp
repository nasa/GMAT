//------------------------------------------------------------------------------
//                           WxGuiInterface
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
 * Interface code used for GUI plugin components, wxWidgets implementation
 */
//------------------------------------------------------------------------------

#include "WxGuiInterface.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// GuiInterface *Instance()
//------------------------------------------------------------------------------
/**
 * Singleton accessor for the interface
 *
 * @return The singleton pointer
 */
//------------------------------------------------------------------------------
GuiInterface *WxGuiInterface::Instance()
{
   if (instance == NULL)
      instance = new WxGuiInterface();
   return instance;
}


//------------------------------------------------------------------------------
// GmatBase* WxGuiInterface::CreateObject(const std::string& ofType,
//       const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Object creation interface
 *
 * @param ofType   Type name used in the factory to create the object
 * @param withname Name of the object that is created
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* WxGuiInterface::CreateObject(const std::string& ofType,
      const std::string& withName)
{
   #ifdef DEBUG_WXGUI_CREATE
      MessageInterface::ShowMessage("WxGuiInterface::CreateObject(%s, %s) "
            "called\n", ofType.c_str(), withName.c_str());
   #endif

   GmatBase *retval = GuiInterface::CreateObject(ofType, withName);

   if (retval)
   {
      if (resourceTree)
      {
         resourceTree->AddObjectToTree(retval);
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void* CreateGuiElement(const std::string& ofType, const std::string& withName)
//------------------------------------------------------------------------------
/**
 * GUI widget creation method
 *
 * @param ofType   The panel type needed
 * @param withName The name associated with teh widget.  This is a title for
 *                 windows, etc
 *
 * @return The object pointer, returned as a void* pointer that must be recast
 *         on the receiver side
 */
//------------------------------------------------------------------------------
void* WxGuiInterface::CreateGuiElement(const std::string& ofType,
      const std::string& withName)
{
   void *retval = NULL;

   return retval;
}

//------------------------------------------------------------------------------
// void SetResourceTree(ResourceTree* rt)
//------------------------------------------------------------------------------
/**
 * Sets the pointer to the ResourceTree in the GUI
 *
 * @param rt The ResourceTree pointer
 */
//------------------------------------------------------------------------------
void WxGuiInterface::SetResourceTree(ResourceTree* rt)
{
   resourceTree = rt;
}

// Hidden elements: this is a singleton

//------------------------------------------------------------------------------
// WxGuiInterface()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
WxGuiInterface::WxGuiInterface() :
   GuiInterface      (),
   resourceTree      (NULL)
{
   theInterpreter = GuiInterpreter::Instance();
}

//------------------------------------------------------------------------------
// ~WxGuiInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
WxGuiInterface::~WxGuiInterface()
{
}
