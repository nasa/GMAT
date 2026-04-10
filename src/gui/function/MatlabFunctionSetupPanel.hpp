//$Id$
//------------------------------------------------------------------------------
//                              MatlabFunctionSetupPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2005/02/11
//
/**
 * Declares MatlabFunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef MatlabFunctionSetupPanel_hpp
#define MatlabFunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Function.hpp"

class MatlabFunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   MatlabFunctionSetupPanel(wxWindow *parent, const wxString &name);
    
private:

   Function *theMatlabFunction;

   wxStaticBoxSizer *mTopSizer;
   wxBoxSizer *mPageSizer;
   wxTextCtrl *mPathTextCtrl;
   wxButton *mBrowseButton;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
