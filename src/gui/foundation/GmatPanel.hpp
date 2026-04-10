//$Id$
//------------------------------------------------------------------------------
//                              GmatPanel
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
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatPanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatPanel_hpp
#define GmatPanel_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "UserInputValidator.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatPanel : public wxPanel, public UserInputValidator
{
public:
   
   // constructors
   GmatPanel(wxWindow *parent, bool showBottomSizer = true,
             bool showScriptButton = true);

   virtual wxPanel* GetPanel();
   virtual GuiInterpreter* GetGuiInterpreter();
   
   virtual bool TakeAction(const wxString &action);
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(UnsignedInt type,
                                  const wxString &oldName,
                                  const wxString &newName);
   virtual bool RefreshObjects(UnsignedInt type = Gmat::UNKNOWN_OBJECT);
   virtual void EnableUpdate(bool enable = true);
   virtual void SetCanClose(bool flag);
   virtual void SetEditorModified(bool isModified);
   virtual bool IsEditorModified();
   
   virtual void OnApply(wxCommandEvent &event);
   virtual void OnOK(wxCommandEvent &event);
   virtual void OnCancel(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);
   virtual void OnSummary(wxCommandEvent &event);

   void SetFunction(bool isFun) { mIsFunction = isFun; }
   bool IsFunction() { return mIsFunction; }
   
protected:
   
   // member functions
   virtual bool SetObject(GmatBase *obj);
   virtual void Show();
   virtual void Create() = 0;
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowBottomSizer;
   bool mShowScriptButton;
   bool mDataChanged;
   bool mEditorModified;
   bool mIsFunction;
   
   wxWindow *theParent;
   
   wxBoxSizer *thePanelSizer;
   #ifdef __SHOW_TOP_SIZER__
   wxStaticBoxSizer *theTopSizer;
   #endif
   //wxStaticBoxSizer *theMiddleSizer;
   wxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   
   wxButton *theOkButton;
   wxButton *theApplyButton;
   wxButton *theCancelButton;
   wxButton *theHelpButton;
   wxButton *theScriptButton;
   wxButton *theSummaryButton;
   
   std::string mObjectName;
   GmatBase *mObject;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8000,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
      ID_BUTTON_SUMMARY,
   };
   
};

#endif // GmatPanel_hpp
