//$Id$
//------------------------------------------------------------------------------
//                              RunScriptFolderDialog
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
// Created: 2005/12/22
//
/**
 * Shows dialog where various run script folder option can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef RunScriptFolderDialog_hpp
#define RunScriptFolderDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class RunScriptFolderDialog : public GmatDialog
{
public:
   
   RunScriptFolderDialog(wxWindow *parent, int numScripts, Real absTol,
                         const wxString &compareDir);
   ~RunScriptFolderDialog();
   
   wxString GetFilterString(bool &exclude);
   wxString Get2ndFilterString(bool &exclude);
   
   bool RunScripts() { return mRunScripts; }
   bool RunFromSavedScripts() { return mRunFromSavedScripts; }
   bool CompareResults() { return mCompareResults; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   bool HasOutDirChanged() { return mOutDirChanged; }
   bool CreateRunFolder() { return mCreateRunFolder; }
   Real GetAbsTolerance() { return mAbsTol; }
   Integer  GetStartingScriptNumber() { return mNumStartingScript; }
   Integer  GetNumScriptsToRun() { return mNumScriptsToRun; }
   Integer  GetNumTimesToRun() { return mNumTimesToRun; }
   wxString GetReplaceString() { return mReplaceString; }
   wxString GetCompareDirectory() { return mCompareDir; }
   wxString GetSaveFilename() { return mSaveFilename; }
   wxString GetCurrentOutDir() { return mCurrOutDir; }
   wxString GetSaveScriptsDir() { return mSaveScriptsDir; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mStartingScriptTextCtrl;
   wxTextCtrl *mNumScriptsToRunTextCtrl;
   wxTextCtrl *mFilterStringTextCtrl;
   wxTextCtrl *m2ndFilterStringTextCtrl;
   wxTextCtrl *mNumTimesToRunTextCtrl;
   wxTextCtrl *mCompareDirTextCtrl;
   wxTextCtrl *mReplaceTextCtrl;
   wxTextCtrl *mAbsTolTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   wxTextCtrl *mCurrOutDirTextCtrl;
   wxTextCtrl *mSaveScriptsDirTextCtrl;
   
   wxCheckBox *mRunFromSavedCheckBox;
   wxCheckBox *mExcludeScriptsCheckBox;
   wxCheckBox *m2ndExcludeScriptsCheckBox;
   wxCheckBox *mCreateRunFolderCheckBox;
   wxCheckBox *mCompareCheckBox;
   wxCheckBox *mSaveResultCheckBox;
   
   wxButton *mDirBrowseButton;
   wxButton *mSaveBrowseButton;
   wxButton *mChangeCurrOutDirButton;
   wxButton *mChangeSaveScriptsDirButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnCheckBoxChange(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_CHECKBOX,
   };
   
private:

   bool mRunScripts;
   bool mRunFromSavedScripts;
   bool mCompareResults;
   bool mSaveCompareResults;
   bool mOutDirChanged;
   bool mExcludeScripts;
   bool m2ndExcludeScripts;
   bool mCreateRunFolder;
   Integer mNumStartingScript;
   Integer mNumScriptsToRun;
   Integer mNumTimesToRun;
   Real mAbsTol;
   wxString mFilterString;
   wxString m2ndFilterString;
   wxString mReplaceString;
   wxString mCurrOutDir;
   wxString mSaveScriptsDir;
   wxString mCompareDir;
   wxString mSaveFilename;
};

#endif
