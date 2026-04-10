//------------------------------------------------------------------------------
//                           PluginWidget
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
// Created: Oct 6, 2017
/**
 * 
 */
//------------------------------------------------------------------------------

#include "PluginWidget.hpp"
#include "MessageInterface.hpp"
#include "PluginItemManager.hpp"

PluginWidget::PluginWidget() :
   minimizeOnRun     (true),
   theIcon           (NULL)
{
   #ifdef DEBUG_WIDGET_SCOPE
      MessageInterface::ShowMessage("Creating PluginWidget at %p\n", this);
   #endif
}

PluginWidget::~PluginWidget()
{
   #ifdef DEBUG_WIDGET_SCOPE
      MessageInterface::ShowMessage("Destroying PluginWidget at %p\n", this);
   #endif
   PluginItemManager::Instance()->RemoveWidget(this);
}

void PluginWidget::RenameObject(const std::string &oldName, const std::string newName,
                     const UnsignedInt ofType)
{
   return;
}

//------------------------------------------------------------------------------
// void PluginWidget::UpdateObjects(const UnsignedInt ofType)
//------------------------------------------------------------------------------
/**
 * Notifies a plugin widget that there has been a change in an object type
 *
 * @param ofType The object type that has changed (typically vis a new object)
 */
//------------------------------------------------------------------------------
void PluginWidget::UpdateObjectList(const UnsignedInt ofType)
{
   #ifdef DEBUG_GUIUPDATES
      MessageInterface::ShowMessage("Updating type %d -- No action\n", ofType);
   #endif
}

//------------------------------------------------------------------------------
// bool MinimizeOnRun()
//------------------------------------------------------------------------------
/**
 * Access method indicating the widget should be minimized when script executes
 *
 * @retval true (the default) if the widget should minimize, false if not
 */
bool PluginWidget::MinimizeOnRun()
{
   return minimizeOnRun;
}

//------------------------------------------------------------------------------
// const char* GetIcon()
//------------------------------------------------------------------------------
/**
 * Retrieves the icon used in the GUI tree for the object
 *
 * @return The icon, imported from an XPM formatted file, or NULL if not set
 */
//------------------------------------------------------------------------------
const char* PluginWidget::GetIcon()
{
   return theIcon;
}
