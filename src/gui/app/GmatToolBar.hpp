//$Id$
//------------------------------------------------------------------------------
//                             GmatToolBar
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
// Author: Linda Jun
// Created: 2008/11/13
/**
 * This class provides the tool bar for the main frame.
 */
//------------------------------------------------------------------------------
#ifndef GmatToolBar_hpp
#define GmatToolBar_hpp

#include "gmatwxdefs.hpp"
//#include "GmatTreeItemData.hpp"  // for GmatTree::

class GmatToolBar : public wxToolBar
{
public:
   GmatToolBar(wxWindow* parent, long style = wxTB_HORIZONTAL | wxNO_BORDER,
               wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               const wxString& name = wxPanelNameStr);
   
   void CreateToolBar(wxToolBar *toolBar);
   void AddAnimationTools(wxToolBar* toolBar);
   void AddGuiScriptSyncStatus(wxToolBar* toolBar);
   void AddAdvancedStatusField(wxToolBar* toolBar);
   void UpdateGuiScriptSyncStatus(wxToolBar* toolBar, int guiStat, int scriptStat);
   void UpdateAdvancedField(wxToolBar* toolBar, int status);

protected:
   wxStaticText *theSyncStatus;
   wxStaticText *theAdvancedField;

   void LoadIcon(const wxString &filename, long bitmapType, wxBitmap **bitmap, const char* xpm[]);
};

#endif

