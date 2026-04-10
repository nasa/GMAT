//$Id$
//------------------------------------------------------------------------------
//                              TreeViewOptionDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2011/07/27
//
/**
 * Declares TreeViewOptionDialog class.
 */
//------------------------------------------------------------------------------
#ifndef TreeViewOptionDialog_hpp
#define TreeViewOptionDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/button.h>
#include "wx/treectrl.h"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"


class TreeViewOptionDialog : public wxDialog
{
public:

   // constructors
   TreeViewOptionDialog(wxWindow *parent, wxTreeCtrl *treeCtrl = NULL,
                        const wxString& title = "",
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_DIALOG_STYLE);
   
protected:

   wxWindow   *mParent;
   wxTreeCtrl *mTreeCtrl;
   
   wxRadioBox *mViewRadioBox;
   wxCheckListBox *mViewCheckListBox;
   wxButton   *mCheckAllButton;
   wxButton   *mUncheckAllButton;
   wxButton   *mViewByCmdApplyButton;
   
   void Create();
   
   void OnButtonClick(wxCommandEvent &event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_BUTTON = 500,
      ID_LEVEL_BUTTON,
      ID_CATEGORY_BUTTON,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_RADIOBOX
   };

};

#endif // TreeViewOptionDialog_hpp
