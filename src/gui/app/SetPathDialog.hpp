//$Id$
//------------------------------------------------------------------------------
//                              SetPathDialog
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
#ifndef SetPathDialog_hpp
#define SetPathDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "MultiPathSetupPanel.hpp"
#include "SinglePathSetupPanel.hpp"

class SetPathDialog : public GmatDialog
{
public:
   
   SetPathDialog(wxWindow *parent);
   ~SetPathDialog();
   
protected:
   
   wxTextCtrl *mReadFileTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   wxNotebook *mPathNotebook;
   MultiPathSetupPanel  *mGmatFunPathPanel;
   MultiPathSetupPanel  *mMatlabPathPanel;
   SinglePathSetupPanel *mOutputPathPanel;
   wxString mStartupFilePath;
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnReadButtonClick(wxCommandEvent& event);   
   void OnSaveButtonClick(wxCommandEvent& event);   
   void OnPageChange(wxCommandEvent &event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_BUTTON_READ,
      ID_BUTTON_SAVE,
      ID_NOTEBOOK,
   };
   
};

#endif
