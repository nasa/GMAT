//$Id$
//------------------------------------------------------------------------------
//                              OutputTree
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Allison Greene
// Created: 2005/04/11
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#ifndef OutputTree_hpp
#define OutputTree_hpp

#include "gmatwxdefs.hpp"

#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatTreeItemData.hpp"

class OutputTree : public wxTreeCtrl
{
public:

   // constructor
   OutputTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
              const wxSize& size, long style);
   
   void RemoveItem(GmatTree::ItemType type, const wxString &name, bool forceRemove = false);
   void UpdateOutput(bool resetTree, bool removeReports, bool removePlots);
   
private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxString theSubscriberName;
   
   wxTreeItemId mReportItem;
   wxTreeItemId mEphemFileItem;
   wxTreeItemId mOrbitViewItem;
   wxTreeItemId mGroundTrackItem;
   wxTreeItemId mXyPlotItem;
   wxTreeItemId mEventsItem;
   
   void AddDefaultResources();
   wxTreeItemId FindItem(wxTreeItemId parentId, const wxString &name);
   
   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void OnItemActivated(wxTreeEvent &event);
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   void OnEndLabelEdit(wxTreeEvent &event);
   void OnBeginLabelEdit(wxTreeEvent &event);
   
   void AddIcons();   
   void OnAddReportFile(wxCommandEvent &event);
   void OnAddXyPlot(wxCommandEvent &event);
   void OnAddOrbitView(wxCommandEvent &event);
   
   void OnCompareTextLines(wxCommandEvent &event);
   void OnCompareNumericLines(wxCommandEvent &event);
   void OnCompareNumericColumns(wxCommandEvent &event);
   
   DECLARE_EVENT_TABLE();
   
   // for popup menu
   enum
   {
      POPUP_COMPARE_TEXT_LINES = 200,
      POPUP_COMPARE_NUMERIC_LINES,
      POPUP_COMPARE_NUMERIC_COLUMNS,
   };
};

#endif // OutputTree_hpp
