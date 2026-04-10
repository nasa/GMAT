//$Id$
//------------------------------------------------------------------------------
//                              GmatDialog
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
// Created: 2004/02/02
//
/**
 * Declares GmatDialog class. This class provides base class of GMAT Dialogs.
 */
//------------------------------------------------------------------------------

#ifndef GmatDialog_hpp
#define GmatDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "UserInputValidator.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatDialog : public wxDialog, public UserInputValidator
{
public:
   
   // constructors
   GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
              GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
   
   virtual void EnableUpdate(bool enable = true);
   virtual bool HasDataUpdated();
   virtual void SetCanClose(bool flag);
   
   virtual void OnOK(wxCommandEvent &event);
   virtual void OnCancel(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnClose(wxCloseEvent &event);
      
protected:
   
   // member functions
   // Changed Show() to ShowDate(), because it conflicts with wxDialog::Show()
   virtual void ShowData();   
   virtual void Create() = 0;
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   virtual void ResetData() = 0;
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   GmatBase *mObject;
   bool canClose;
   bool mDataChanged;
   bool mDataUpdated;
   
   wxWindow *theParent;
    
   wxBoxSizer *theDialogSizer;
   wxBoxSizer *theButtonSizer;
   
   #if __WXMAC__
   wxBoxSizer *theMiddleSizer;
   wxBoxSizer *theBottomSizer;
   #else
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   #endif
   
   wxButton *theOkButton;
   wxButton *theCancelButton;
   wxButton *theHelpButton;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8100,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_GMAT_DIALOG_LAST
   };

};

#endif // GmatDialog_hpp
