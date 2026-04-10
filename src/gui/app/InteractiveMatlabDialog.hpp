//$Id$
//------------------------------------------------------------------------------
//                              InteractiveMatlabDialog
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
// Author: Allison Greene
// Created: 2004/11/12
//
/**
 * Declares InteractiveMatlabDialog class. This class shows dialog window where 
 * parameters can be passed to/from matlab.
 * 
 */
//------------------------------------------------------------------------------
#ifndef InteractiveMatlabDialog_hpp
#define InteractiveMatlabDialog_hpp

#include "gmatwxdefs.hpp"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "CallFunction.hpp"

class InteractiveMatlabDialog : public wxDialog
{
public:
    
   InteractiveMatlabDialog(wxWindow *parent);
   ~InteractiveMatlabDialog();

protected:
    // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxArrayString mObjectTypeList;
   
   wxWindow *theParent;

   wxBoxSizer *theDialogSizer;
   wxStaticBoxSizer *theTopSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   wxBoxSizer *theButtonSizer; //loj: 10/19/04 added

   wxButton *theEvaluateButton;
   wxButton *theClearButton;
   wxButton *theCloseButton;

   wxGrid *inputGrid;
   wxGrid *outputGrid;

   wxArrayString inputStrings;
   wxArrayString outputStrings;

   wxStaticText *nameStaticText;
   wxComboBox *functionComboBox;

   wxTextCtrl *inputTextCtrl;
   wxTextCtrl *outputTextCtrl;

private:
   CallFunction *theCmd;

   // abstract methods from GmatDialog
   void Create();
   void Show();

   void SetupCommand();
   void SetResults();
   void OnClear();

   // event handling
   void OnButton(wxCommandEvent& event);
   void OnCellClick(wxGridEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 9300,
      ID_COMBOBOX,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif
