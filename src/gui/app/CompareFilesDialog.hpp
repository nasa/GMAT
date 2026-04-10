//$Id$
//------------------------------------------------------------------------------
//                              CompareFilesDialog
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
#ifndef CompareFilesDialog_hpp
#define CompareFilesDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class CompareFilesDialog : public GmatDialog
{
public:
   
   CompareFilesDialog(wxWindow *parent);
   ~CompareFilesDialog();
   
   bool CompareFiles() { return mCompareFiles; }
   bool SkipBlankLines() { return mSkipBlankLinesForTextCompare; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   Real GetCompareTolerance() { return mTolerance; }
   Integer GetCompareOption() { return mCompareOption; }
   Integer GetNumDirsToCompare() { return mNumDirsToCompare; }
   Integer GetNumFilesToCompare() { return mNumFilesToCompare; }
   wxString GetBaseDirectory() { return mBaseDirectory; }
   wxString GetBaseString() { return mBaseString; }
   wxString GetSaveFilename() { return mSaveFileName; }
   wxArrayString GetCompareDirectories() { return mCompareDirs; }
   wxArrayString GetCompareStrings() { return mCompareStrings; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mBaseDirTextCtrl;
   wxTextCtrl *mCompareDirTextCtrl;
   wxTextCtrl *mNumFilesInBaseDirTextCtrl;
   wxTextCtrl *mNumFilesInCompareDirTextCtrl;
   wxTextCtrl *mNumDirsToCompareTextCtrl;
   wxTextCtrl *mNumFilesToCompareTextCtrl;
   wxTextCtrl *mBaseStrTextCtrl;
   wxTextCtrl *mCompareStrTextCtrl;
   wxTextCtrl *mToleranceTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   
   wxRadioBox *mCompareOptionRadioBox;
   wxComboBox *mCompareDirsComboBox;
   wxCheckBox *mSkipBlankLinesCheckBox;
   wxCheckBox *mSaveResultCheckBox;
   
   wxButton *mBaseDirButton;
   wxButton *mCompareDirButton;
   wxButton *mBaseUpdateButton;
   wxButton *mCompareUpdateButton;
   wxButton *mSaveBrowseButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnRadioButtonClick(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextEnterPress(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_RADIOBOX,
   };
   
private:

   bool mCompareFiles;
   bool mSkipBlankLinesForTextCompare;
   bool mSaveCompareResults;
   bool mHasDir1;
   bool mHasDir2;
   bool mHasDir3;
   Real mTolerance;
   Integer mCompareOption;
   Integer mNumFilesInBaseDir;
   Integer mNumFilesInCompareDir;
   Integer mNumFilesToCompare;
   Integer mNumDirsToCompare;
   wxString mBaseString;
   wxString mBaseDirectory;
   wxString mSaveFileName;
   wxArrayString mCompareStrings;
   wxArrayString mCompareDirs;
   wxArrayString mFileNamesInBaseDir;
   wxArrayString mFileNamesInCompareDir;

   void UpdateFileInfo(Integer dir, bool isBaseDir);
   wxArrayString GetFilenamesContain(const wxString &dirname,
                                     const wxString &str);
};

#endif
