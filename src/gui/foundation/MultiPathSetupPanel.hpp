//$Id$
//------------------------------------------------------------------------------
//                              MultiPathSetupPanel
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
// Created: 2008/04/16
//
/**
 * Shows dialog for setting path for various files used in the system.
 * 
 */
//------------------------------------------------------------------------------
#ifndef MultiPathSetupPanel_hpp
#define MultiPathSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"

class MultiPathSetupPanel : public wxPanel
{
public:
   
   MultiPathSetupPanel(wxWindow *parent, const StringArray &pathNames);
   ~MultiPathSetupPanel();
   
   const wxArrayString& GetPathNames();
   void UpdatePathNames(const StringArray &pathNames);
   bool HasDataChanged();
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   wxTextCtrl *mFileTextCtrl;
   wxListBox  *mPathListBox;
   wxButton   *mReplaceButton;
   wxButton   *mRemoveButton;
   wxButton   *mAddButton;
   
   // event handling
   void OnListBoxSelect(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);   
   void OnUpButtonClick(wxCommandEvent& event);   
   void OnDownButtonClick(wxCommandEvent& event);   
   void OnRemoveButtonClick(wxCommandEvent& event);   
   void OnBrowseButtonClick(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_LISTBOX,
      ID_UP_BUTTON,
      ID_DOWN_BUTTON,
      ID_REMOVE_BUTTON,
      ID_BROWSE_BUTTON,
   };
   
private:
   
   /// file path names
   wxArrayString mPathNames;
   bool mHasDataChanged;
};

#endif
