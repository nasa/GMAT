//------------------------------------------------------------------------------
//                           PluginItemManager
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
// Created: Oct 4, 2017
/**
 * 
 */
//------------------------------------------------------------------------------

#include "PluginItemManager.hpp"
#include "GmatWidget.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"


#include <algorithm>          // for Find

//#define DEBUG_GUIUPDATES


/// Initialize the static pointer
PluginItemManager *PluginItemManager::instance = NULL;


//------------------------------------------------------------------------------
// PluginItemManager *Instance()
//------------------------------------------------------------------------------
/**
 * Singleton access method
 *
 * Creates the singleton if it has not already been created, and then provides
 * access to it.
 *
 * @return The PluginItemManager.
 */
//------------------------------------------------------------------------------
PluginItemManager *PluginItemManager::Instance()
{
   if (instance == NULL)
      instance = new PluginItemManager();
   return instance;
}


//------------------------------------------------------------------------------
// void PluginItemManager::AddWidget(GmatWidget *widget)
//------------------------------------------------------------------------------
/**
 * Adds a GUI plugin widget for management
 *
 * @param widget The widget
 */
//------------------------------------------------------------------------------
void PluginItemManager::AddWidget(GmatWidget *widget)
{
   #ifdef DEBUG_PLUGINWIDGETS
      MessageInterface::ShowMessage("Adding widget at %p\n", widget);
   #endif

   if (find(widgets.begin(), widgets.end(), widget) == widgets.end())
      widgets.push_back(widget);
}


//------------------------------------------------------------------------------
// void RemoveWidget(GmatWidget *widget)
//------------------------------------------------------------------------------
/**
 * Removes a widget from management
 *
 * @param widget The widget to be removed
 */
//------------------------------------------------------------------------------
void PluginItemManager::RemoveWidget(PluginWidget *widget)
{
   #ifdef DEBUG_PLUGINWIDGETS
      MessageInterface::ShowMessage("Removing widget at %p\n", widget);
   #endif

   for (std::vector<GmatWidget*>::iterator item =widgets.begin();
         item != widgets.end(); ++item)
   {
      if ((*item)->GetWidget() == widget)
      {
         GmatWidget *gw = *item;
         widgets.erase(item);
         delete gw;
         break;
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateObjectList(const UnsignedInt ofType)
//------------------------------------------------------------------------------
/**
 * Method called when the GUI changes the configured objects
 *
 * @param ofType The kind of object that has changed on the GUI
 */
//------------------------------------------------------------------------------
void PluginItemManager::UpdateObjectList(const UnsignedInt ofType)
{
   #ifdef DEBUG_GUIUPDATES
      MessageInterface::ShowMessage("Object list for type %d has changed\n",
            ofType);
   #endif

   PluginWidget *component = NULL;
   for (UnsignedInt i = 0; i < widgets.size(); ++i)
   {
      component = widgets[i]->GetWidget();
      if (component)
         component->UpdateObjectList(ofType);
   }
}

//------------------------------------------------------------------------------
// void RenameObject(const std::string &oldName, const std::string newName,
//                   const UnsignedInt ofType)
//------------------------------------------------------------------------------
/**
 * Method invoked from the GUI when a configured object is renamed
 *
 * @param oldName  The old name
 * @param newName  The new name
 * @param ofType   The type of object renamed (defaults to unknown)
 */
//------------------------------------------------------------------------------
void PluginItemManager::RenameObject(const std::string &oldName, const std::string newName,
                  const UnsignedInt ofType)
{
   #ifdef DEBUG_GUIUPDATES
      MessageInterface::ShowMessage("Rename type %d from %s to %s\n",
            ofType, oldName.c_str(), newName.c_str());
   #endif

   PluginWidget *component = NULL;
   for (UnsignedInt i = 0; i < widgets.size(); ++i)
   {
      component = widgets[i]->GetWidget();
      if (component)
         component->RenameObject(oldName, newName, ofType);
   }
}


//------------------------------------------------------------------------------
// void ClearAllPluginItems()
//------------------------------------------------------------------------------
/**
 * Clean up the widget list when everything reset prior to running a new script
 */
//------------------------------------------------------------------------------
void PluginItemManager::ClearAllPluginItems()
{
   widgets.clear();
}


//------------------------------------------------------------------------------
// StringArray GetListOfObjects(const UnsignedInt ofType);
//------------------------------------------------------------------------------
/**
 * Method used to get the configured objects so that a component can refresh
 *
 * @param ofType The type of object being requested
 *
 * @return A list of all configured objects with that type
 */
//------------------------------------------------------------------------------
StringArray PluginItemManager::GetListOfObjects(const UnsignedInt ofType)
{
   StringArray theList;
   theList = Moderator::Instance()->GetListOfObjects((Gmat::ObjectType)ofType,false);
   return theList;
}


//------------------------------------------------------------------------------
// Hidden to enforce the singleton
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PluginItemManager():
//------------------------------------------------------------------------------
/**
 * (Hidden) constructor
 */
//------------------------------------------------------------------------------
PluginItemManager::PluginItemManager():
   ItemManager          ()
{
}


//------------------------------------------------------------------------------
// PluginItemManager::~PluginItemManager()
//------------------------------------------------------------------------------
/**
 * (Hidden abstract) destructor
 */
//------------------------------------------------------------------------------
PluginItemManager::~PluginItemManager()
{
}
