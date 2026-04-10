//------------------------------------------------------------------------------
//                           GmatWidget
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
// Created: Feb 20, 2017
/**
 * Base class for GUI components added to GMAT
 */
//------------------------------------------------------------------------------

#ifndef GmatWidget_hpp
#define GmatWidget_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PluginWidget.hpp"

class PluginItemManager;


/**
 * Base class for GUI addins.
 *
 * This class is overridden in the GUI code to add base toolkit specific
 * features.
 */
class GMAT_API GmatWidget
{
public:
   GmatWidget(const std::string &type, GmatBase *forObject = NULL);
   virtual ~GmatWidget();
   void SetWidget(PluginWidget* widget, const std::string &mode);
   PluginWidget* GetWidget();
   PluginWidget* GetGuiWidget();
   std::string GetType();
   std::string GetName();
   std::string GetMode();
   void GetGeometry(Integer &l, Integer &t, Integer &w, Integer &h);
   void GetGeometry(Real &l, Real &t, Real &w, Real &h);

   void GetMinimumSize(Integer &wid, Integer &ht);

   enum WidgetMode {
      DIALOG = 700,
      PANEL,
//      MDI_CHILD,
      UNKNOWN_MODE
   };

protected:
   // Default methods protected -- not usually needed in the GUI, but they are
   // provided in case they are needed
   GmatWidget(const GmatWidget &gw);
   GmatWidget& operator=(const GmatWidget &gw);

   /// The object associated with the widget, if needed
   GmatBase *theObject;
   /// The widget, as a void* that can be recast in the GUI
   PluginWidget *theWidget;

   // A minimal set of attributes for the widgets
   /// Type of GUI element
   std::string widgetType;
   /// Name of the widget
   std::string widgetName;
   /// Upper edge location
   Integer top;
   /// Lower edge location
   Integer bottom;
   /// Left edge location
   Integer left;
   /// Right edge location
   Integer right;
   /// Minimum width
   Integer minWidth;
   /// Minimum height
   Integer minHeight;

   // Geometry settings for GMAT's wxWidgets window layout
   /// Proportional upper edge location
   Real ptop;
   /// Proportional lower edge location
   Real pheight;
   /// Proportional left edge location
   Real pleft;
   /// Proportional right edge location
   Real pwidth;

   /// Type of widget control, so that it can open correctly
   WidgetMode displayMode;

   /// The ItemManager used to update widget data structures
   PluginItemManager *theItemManager;
};

#endif /* GmatWidget_hpp */
