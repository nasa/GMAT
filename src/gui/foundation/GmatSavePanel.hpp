//$Id$
//------------------------------------------------------------------------------
//                              GmatSavePanel
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
 * Declares GmatSavePanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatSavePanel_hpp
#define GmatSavePanel_hpp

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

#include "GmatPanel.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatSavePanel : public GmatPanel
{
public:
   // constructors
   GmatSavePanel(wxWindow *parent, bool showScriptButton = true,
                 wxString filename = "", bool showScriptActiveStatus = false,
                 bool isScriptActive = false);
   
   virtual void OnSave(wxCommandEvent &event);
   virtual void OnSaveAs(wxCommandEvent &event);
   virtual void OnClosePanel(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);
   
   virtual void SetEditorModified(bool modified, bool updateSyncStatus = true);
   void UpdateScriptActiveStatus(bool isActive);
   bool UpdateStatusOnClose();
   void ReloadFile();
   
protected:
   // member functions
   virtual void Create() = 0;
   virtual void Show();
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   
   bool DoesFileExist(const char *scriptFilename);
   bool DoesFileExist(const std::string &scriptFilename);
   void MakeScriptActive(wxCommandEvent &event, bool isScriptModified);
   void RefreshScriptActiveStatus(bool isActive);
   void SaveScript();
   void SaveAndBuildScript(wxCommandEvent &event);
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowScriptButton;
   bool hasFileLoaded;
   bool mShowScriptActiveStatus;
   bool mIsScriptActive;
   bool mSyncGui;
   bool mDelayBuild;
   bool mSaveCanceled;
   
   wxString mFilename;
   wxString mScriptFilename;
   
   wxWindow *theParent;
   
   wxBoxSizer       *thePanelSizer;
   wxStaticBoxSizer *theTopSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   wxBoxSizer       *theButtonSizer;
   
   wxButton *theSaveButton;
   wxButton *theSaveAsButton;
   wxButton *theCloseButton;
   wxButton *theScriptButton;
   wxButton *mSaveSyncButton;
   wxButton *mSaveSyncRunButton;
   
   wxStaticText *mScriptActiveLabel;
   wxStaticText *mScriptDirtyLabel;
   
   GmatBase *mObject;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_SAVE = 8050,
      ID_BUTTON_SAVE_AS,
      ID_BUTTON_CLOSE,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
   };
   
};

#endif // GmatSavePanel_hpp
