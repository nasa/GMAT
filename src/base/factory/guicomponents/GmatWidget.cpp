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
 * Base class for plugin GUI components added to GMAT
 */
//------------------------------------------------------------------------------

#include "GmatWidget.hpp"
#include "PluginItemManager.hpp"
#include "MessageInterface.hpp"

#include "Subscriber.hpp"     // To get size data for subscribers


//------------------------------------------------------------------------------
// GmatWidget(const std::string &type, GmatBase *forObject)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param type Object type for the widget
 * @param forObject The object associated with the widget
 */
//------------------------------------------------------------------------------
GmatWidget::GmatWidget(const std::string &type, GmatBase *forObject) :
   theObject         (forObject),
   theWidget         (NULL),
   widgetType        (type),
   widgetName        (""),
   top               (-1),
   bottom            (-1),
   left              (-1),
   right             (-1),
   minWidth          (150),
   minHeight         (100),
   ptop              (0.1),
   pheight           (0.8),
   pleft             (0.1),
   pwidth            (0.8),
   displayMode       (PANEL)
{
   if (forObject)
      widgetName = forObject->GetName();

   theItemManager = PluginItemManager::Instance();
   theItemManager->AddWidget(this);

   #ifdef DEBUG_WIDGET
      MessageInterface::ShowMessage("GmatWidget %p constructor\n", this);
   #endif
}

//------------------------------------------------------------------------------
// ~GmatWidget()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GmatWidget::~GmatWidget()
{
   #ifdef DEBUG_WIDGET
      MessageInterface::ShowMessage("GmatWidget %p destructor\n", this);
   #endif
}

void GmatWidget::SetWidget(PluginWidget* widget, const std::string &mode)
{
   #ifdef DEBUG_ITEM_MANAGEMENT
      MessageInterface::ShowMessage("GmatWidget %p managing PluginWidget %p "
            "destructor\n", this, widget);
   #endif

   theWidget = widget;
   if (mode == "Panel")
      displayMode = PANEL;
   else if (mode == "Dialog")
      displayMode = DIALOG;
   else
      displayMode = UNKNOWN_MODE;
}

//------------------------------------------------------------------------------
// GmatWidget::GmatWidget(const GmatWidget &gw) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param gw The widget providing properties for this new one
 */
//------------------------------------------------------------------------------
GmatWidget::GmatWidget(const GmatWidget &gw) :
   widgetType        (gw.widgetType),
   widgetName        (gw.widgetName),
   top               (gw.top),
   bottom            (gw.bottom),
   left              (gw.left),
   right             (gw.right),
   minWidth          (gw.minWidth),
   minHeight         (gw.minHeight),
   ptop              (gw.ptop),
   pheight           (gw.pheight),
   pleft             (gw.pleft),
   pwidth            (gw.pwidth),
   displayMode       (gw.displayMode)
{
}

//------------------------------------------------------------------------------
// GmatWidget& GmatWidget::operator=(const GmatWidget &gw)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gw The widget assigning properties to this one
 *
 * @return
 */
//------------------------------------------------------------------------------
GmatWidget& GmatWidget::operator=(const GmatWidget &gw)
{
   if (this == &gw)
   {
      widgetType  = gw.widgetType;
      widgetName  = gw.widgetName;
      top         = gw.top;
      bottom      = gw.bottom;
      left        = gw.left;
      right       = gw.right;
      minWidth    = gw.minWidth;
      minHeight   = gw.minHeight;
      ptop        = gw.ptop;
      pheight     = gw.pheight;
      pleft       = gw.pleft;
      pwidth      = gw.pwidth;
      displayMode = gw.displayMode;
   }

   return *this;
}

//------------------------------------------------------------------------------
// PluginWidget* GmatWidget::GetWidget()
//------------------------------------------------------------------------------
/**
 * Accessor for the widget pointer
 *
 * @return The widget
 */
//------------------------------------------------------------------------------
PluginWidget* GmatWidget::GetWidget()
{
   return theWidget;
}

//------------------------------------------------------------------------------
// PluginWidget* GmatWidget::GetGuiWidget()
//------------------------------------------------------------------------------
/**
 * Accessor for the widget pointer
 *
 * @return The widget

 * @note Remove redundant methos
 */
//------------------------------------------------------------------------------
PluginWidget* GmatWidget::GetGuiWidget()
{
   return theWidget;
}

//------------------------------------------------------------------------------
// std::string GmatWidget::GetType()
//------------------------------------------------------------------------------
/**
 * Accesses the type of the widget: A dialog, a panel, etc
 * 
 * @return The (desired) type of widget that is returned
 */
//------------------------------------------------------------------------------
std::string GmatWidget::GetType()
{
   return widgetType;
}

//------------------------------------------------------------------------------
// std::string GmatWidget::GetName()
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the widget
 *
 * @return The Widget name
 */
//------------------------------------------------------------------------------
std::string GmatWidget::GetName()
{
   return widgetName;
}

//------------------------------------------------------------------------------
// std::string GmatWidget::GetMode()
//------------------------------------------------------------------------------
/**
 * Accesses the type of widget that is in this object
 *
 * @return The type
 */
//------------------------------------------------------------------------------
std::string GmatWidget::GetMode()
{
   std::string mode = "";

   switch (displayMode)
   {
   case PANEL:
      mode = "Panel";
      break;

   case DIALOG:
      mode = "Dialog";
      break;

   default:
      break;
   }

   return mode;
}

//------------------------------------------------------------------------------
// void GmatWidget::GetGeometry(Integer &l, Integer &t, Integer &w, Integer &h)
//------------------------------------------------------------------------------
/**
 * Accesses the geometry of the widget
 *
 * Sets the geometry in the passed-in data members.  A value of -1 indicates 
 * that the calling object should set it up.
 *
 * @param l The left edge of the widget
 * @param t The top edge of the widget
 * @param w The widget width
 * @param h The widget height
 */
//------------------------------------------------------------------------------
void GmatWidget::GetGeometry(Integer &l, Integer &t, Integer &w, Integer &h)
{
   l = left;
   t = top;
   w = (right > left ? right - left : -1);
   h = (bottom > top ? bottom - top : -1);
}


//------------------------------------------------------------------------------
// void GmatWidget::GetGeometry(Integer &l, Integer &t, Integer &w, Integer &h)
//------------------------------------------------------------------------------
/**
* Accesses the geometry of the widget
*
* Sets the geometry in the passed-in data members.  A value of -1 indicates
* that the calling object should set it up.
*
* @param l The left edge of the widget
* @param t The top edge of the widget
* @param w The widget width
* @param h The widget height
*/
//------------------------------------------------------------------------------
void GmatWidget::GetGeometry(Real &l, Real &t, Real &w, Real &h)
{
   if (theObject)
   {
      if (theObject->IsOfType(Gmat::SUBSCRIBER) && 
         (widgetType == theObject->GetGuiPanelNames("Execution")[0]))
      {
         Subscriber *subs = (Subscriber*)theObject;
         Rvector location = subs->GetRvectorParameter("UpperLeft");
         Rvector size = subs->GetRvectorParameter("Size");

         pleft   = location[0];
         ptop    = location[1];
         pwidth  = size[0];
         pheight = size[1];

         // @todo: Add z order and min/max support
      }
      else
      {
         pleft   = 0.0;
         ptop    = 0.0;
         pwidth  =  0.0;
         pheight = 0.0;
      }
   }
   l = pleft;
   t = ptop;
   w = pwidth;
   h = pheight;
}


//------------------------------------------------------------------------------
// void GetMinimumSize(Integer &wid, Integer &ht)
//------------------------------------------------------------------------------
/**
* Accesses the geometry of the widget
*
* Sets the geometry in the passed-in data members.  A value of -1 indicates
* that the calling object should set it up.
*
* @param wid The widget minimum width
* @param ht The widget minimum height
*/
//------------------------------------------------------------------------------
void GmatWidget::GetMinimumSize(Integer &wid, Integer &ht)
{
   wid = minWidth;
   ht = minHeight;
}
