//$Id$
//------------------------------------------------------------------------------
//                           DCSetupPanel
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
// Author: Waka Waktola
// Created: 2004/01/21
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------

#ifndef DCSetupPanel_hpp
#define DCSetupPanel_hpp

#include "gmatwxdefs.hpp"

#include <wx/variant.h>

// base includes
#include "GuiInterpreter.hpp"
#include "GmatPanel.hpp"
#include "Solver.hpp"
#include "DifferentialCorrector.hpp"

class DCSetupPanel : public GmatPanel
{
public:
   // constructors
   DCSetupPanel(wxWindow *parent, const wxString &name);
   ~DCSetupPanel();  
   
private:     
   Solver *theSolver;
   DifferentialCorrector *theDC;
   bool isTextModified;
   
//   wxString reportStyle;
   
   wxStaticText *maxStaticText;
   wxStaticText *reportfileStaticText;  // wxStaticText *textfileStaticText;
   wxStaticText *reportStyleStaticText;
   wxStaticText *algorithmStaticText;
   wxStaticText *derivativeMethodStaticText;
   
   wxTextCtrl *maxTextCtrl;
   wxTextCtrl *reportfileTextCtrl;      //   wxTextCtrl *textfileTextCtrl;
   
   wxCheckBox *showProgressCheckBox;
//   wxCheckBox *centralDifferencesCheckBox;
      
   wxComboBox *styleComboBox;
   wxComboBox *algorithmComboBox;
   wxComboBox *derivativeMethodComboBox;
         
   wxBitmapButton       *browseButton;
   
   wxFileDialog *fileDialog;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout 
   void Setup(wxWindow *parent);
   
   // Event Methods
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnCheckBoxChange(wxCommandEvent &event);
   void OnBrowse(wxCommandEvent &event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 55000,
      ID_TEXTCTRL,
      ID_CHECKBOX,
      ID_COMBOBOX,
      ID_BUTTON_BROWSE
   };
};

#endif // SolverEvent_hpp
