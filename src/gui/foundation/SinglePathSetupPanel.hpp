//$Id$
//------------------------------------------------------------------------------
//                              SinglePathSetupPanel
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
#ifndef SinglePathSetupPanel_hpp
#define SinglePathSetupPanel_hpp

#include "gmatwxdefs.hpp"

class SinglePathSetupPanel : public wxPanel
{
public:
   
   SinglePathSetupPanel(wxWindow *parent, const wxString &filepath);
   ~SinglePathSetupPanel();
   
   bool HasDataChanged();
   wxString GetFullPathName();
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   wxTextCtrl *mFileTextCtrl;   
   
   // event handling
   void OnBrowseButtonClick(wxCommandEvent& event);   
   void OnTextChange(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BROWSE_BUTTON,
   };
   
private:
   
   /// file path and name
   wxString mFullPathName;
   bool mHasDataChanged;
};

#endif
