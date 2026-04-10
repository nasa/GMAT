//$Id$
//------------------------------------------------------------------------------
//                              UniversePanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#ifndef UniversePanel_hpp
#define UniversePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "SolarSystem.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <map>

class UniversePanel: public GmatPanel
{
public:
   UniversePanel(wxWindow *parent);
   ~UniversePanel();
   
   virtual void OnScript(wxCommandEvent &event);
    
   
private:
   StringArray mAllFileTypes;
   StringArray mFileTypesInUse;
   
   std::map<wxString, wxString> mFileTypeNameMap;
   
   StringArray spiceKernelNames;

   bool        mHasFileTypesInUseChanged;
   bool        mHasFileNameChanged;
   bool        mHasSPKFileNameChanged;
   bool        mHasPCKFileNameChanged;
   bool        mHasTextModified;
   
   wxComboBox *mFileTypeComboBox;
   
   wxTextCtrl *mIntervalTextCtrl;
   wxTextCtrl *mDEFileNameTextCtrl;
   wxTextCtrl *mSPKFileNameTextCtrl;
   wxTextCtrl *mPCKFileNameTextCtrl;
   
   wxBitmapButton *mBrowseButton;
   wxBitmapButton *mSPKBrowseButton;
   wxBitmapButton *mPCKBrowseButton;

   wxCheckBox     *mOverrideCheckBox;

   GmatStaticBoxSizer *mPageSizer;
   
   SolarSystem  *theSolarSystem;
   
   wxStaticText *deFileNameLabel;
   wxStaticText *spkNameLabel;
   wxStaticText *pckNameLabel;

   std::map<wxString, wxString> mPreviousFileTypeNameMap;
   wxString      previousSpkFile;
   wxString      previousPckFile;


   void OnBrowseButton(wxCommandEvent& event);
   void OnSPKBrowseButton(wxCommandEvent& event);
   void OnPCKBrowseButton(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnTextCtrlChange(wxCommandEvent& event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 6100,
      ID_COMBOBOX,
      ID_TEXT_CTRL,
      ID_BUTTON_BROWSE,
      ID_SPK_BUTTON_BROWSE,
      ID_PCK_BUTTON_BROWSE,
      ID_CHECKBOX,
   };
};
#endif
