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
 * ItemManager for plugin GUI components
 */
//------------------------------------------------------------------------------

#ifndef PluginItemManager_hpp
#define PluginItemManager_hpp

#include "gmatdefs.hpp"
#include "ItemManager.hpp"
#include "GmatWidget.hpp"
#include "PluginWidget.hpp"

#include <vector>

class GmatWidget;

/**
 * Singleton class that serves, for GUI plugin components, the GuiItemManager role
 */
class GMAT_API PluginItemManager : public ItemManager
{
public:
   static PluginItemManager *Instance();

   // Widget registration
   void AddWidget(GmatWidget *widget);
   void RemoveWidget(PluginWidget *widget);

   // Object management methods responding to GUI actions
   virtual void UpdateObjectList(const UnsignedInt ofType = Gmat::UNKNOWN_OBJECT);
   virtual void RenameObject(const std::string &oldName,
                             const std::string newName,
                             const UnsignedInt ofType = Gmat::UNKNOWN_OBJECT);
   void ClearAllPluginItems();

   // Methods plugin component call to access data updates
   virtual StringArray GetListOfObjects(const UnsignedInt ofType);

private:
   /// The managed widgets
   std::vector<GmatWidget*> widgets;

   /// The Singleton
   static PluginItemManager *instance;

   PluginItemManager();
   virtual ~PluginItemManager();
};

#endif /* PluginItemManager_hpp */
