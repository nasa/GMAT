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
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//

#ifndef NONLINEARCONSTRAINTPANEL_HPP_
#define NONLINEARCONSTRAINTPANEL_HPP_

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "NonlinearConstraint.hpp"

class NonlinearConstraintPanel : public GmatPanel
{
public:
   NonlinearConstraintPanel(wxWindow *parent, GmatCommand *cmd);
   ~NonlinearConstraintPanel(); 
    
private:  
   GuiItemManager *theGuiManager;
           
   wxTextCtrl *mLHSTextCtrl;
   wxTextCtrl *mRHSTextCtrl;
   wxTextCtrl *mTolTextCtrl;

   wxButton *mLeftChooseButton;
   wxButton *mRightChooseButton;

   wxComboBox *mSolverComboBox;
   wxComboBox *mComparisonComboBox;
   
   wxArrayString mObjectTypeList;
   NonlinearConstraint *mNonlinearConstraintCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
  
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 53000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBO,
      ID_GRID,
   };
};

#endif /*NONLINEARCONSTRAINTPANEL_HPP_*/
