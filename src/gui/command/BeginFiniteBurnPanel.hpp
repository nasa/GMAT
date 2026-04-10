//$Id$
//------------------------------------------------------------------------------
//                              BeginFiniteBurnPanel
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
// Author: Linda Jun
// Created: 2006/07/14
//
/**
 * This class contains the BeginFiniteBurn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef BeginFiniteBurnPanel_hpp
#define BeginFiniteBurnPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"

class BeginFiniteBurnPanel : public GmatPanel
{
public:
   // constructors
   BeginFiniteBurnPanel(wxWindow *parent, GmatCommand *cmd);
   ~BeginFiniteBurnPanel();
   
protected:
   // member data
   GmatCommand *theCommand;
   wxArrayString mObjectTypeList;
   wxArrayString mSpacecraftList;
   
   wxComboBox *mFiniteBurnComboBox;
   wxTextCtrl *mSatTextCtrl;
   
   wxArrayString ToWxArrayString(const StringArray &array);
   wxString ToWxString(const wxArrayString &names);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnButtonClicked(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
   };
};

#endif // BeginFiniteBurnPanel_hpp
