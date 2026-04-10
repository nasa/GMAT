//$Id$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
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
// Created: 2004/02/25
//
/**
 * Declares ParameterSetupPanel class. This panel is used for setting
 * values for user Variables and Strings.
 */
//------------------------------------------------------------------------------

#ifndef ParameterSetupPanel_hpp
#define ParameterSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Parameter.hpp"

class ParameterSetupPanel: public GmatPanel
{
public:
   // constructors
   ParameterSetupPanel(wxWindow *parent, const wxString &name); 
    
private:

   Parameter *mParam;
   bool mIsStringVar;
   bool mIsValueChanged;
   
   wxString mVarName;

   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mValueTextCtrl;
   
   wxBoxSizer *mPageBoxSizer;
   wxStaticBoxSizer *mVarStaticBoxSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_TEXTCTRL
   };
};

#endif
