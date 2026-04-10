//$Id$
//------------------------------------------------------------------------------
//                              TextEphemFileDialog
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
// Created: 2006/03/29
//
/**
 * Shows dialog for setting parameters for ephemeris file generation.
 * 
 */
//------------------------------------------------------------------------------
#ifndef TextEphemFileDialog_hpp
#define TextEphemFileDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class TextEphemFileDialog : public GmatDialog
{
public:
   
   TextEphemFileDialog(wxWindow *parent);
   ~TextEphemFileDialog();
   bool CreateEphemFile() { return mCreateEphemFile; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mEphemFileTextCtrl;
   wxTextCtrl *mIntervalTextCtrl;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mEpochFormatComboBox;

   wxListBox *mSpacecraftListBox;
   wxListBox *mSelectedScListBox;
   
   wxButton *mAddScButton;
   wxButton *mRemoveScButton;
   wxButton *mClearScButton;
   wxButton *mEphemFileBrowseButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_LISTBOX,
   };
   
private:

   wxString mEphemDirectory;
   bool mCreateEphemFile;
   
   bool CreateTextEphem();
};

#endif
