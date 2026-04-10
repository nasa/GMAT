//$Id$
//------------------------------------------------------------------------------
//                              FindReplaceDialog
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
// Created: 2009/01/09
//
/**
 * Declares FindReplaceDialog class.
 */
//------------------------------------------------------------------------------
#ifndef FindReplaceDialog_hpp
#define FindReplaceDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/button.h>

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ScriptEditor;

class FindReplaceDialog : public wxDialog
{
public:

   // default constructor
   FindReplaceDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_DIALOG_STYLE);

   void SetEditor(ScriptEditor *editor) { mEditor = editor; };
   wxString GetFindText() { return mFindText; };

protected:

   ScriptEditor *mEditor;
   wxArrayString mFindArray;
   wxArrayString mReplaceArray;
   wxString   mFindText;
   wxString   mReplaceText;

   wxComboBox *mFindComboBox;
   wxComboBox *mReplaceComboBox;
   wxButton   *mFindNextButton;
   wxButton   *mFindPrevButton;
   wxButton   *mReplaceButton;
   wxButton   *mReplaceAllButton;
   wxButton   *mCloseButton;

   void Create();

   void OnButtonClick(wxCommandEvent &event);
   void OnComboBoxEnter(wxCommandEvent &event);
   void OnKeyDown(wxKeyEvent &event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_BUTTON = 500,
      ID_COMBOBOX,
   };

};

#endif // FindReplaceDialog_hpp
