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
 */
//------------------------------------------------------------------------------

#include "CompareFilesDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_COMPARE_FILES_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareFilesDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CompareFilesDialog::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, CompareFilesDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, CompareFilesDialog::OnComboBoxChange)
   EVT_TEXT_ENTER(ID_TEXTCTRL, CompareFilesDialog::OnTextEnterPress)
   EVT_RADIOBOX(ID_RADIOBOX, CompareFilesDialog::OnRadioButtonClick)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// CompareFilesDialog(wxWindow *parent)
//------------------------------------------------------------------------------
CompareFilesDialog::CompareFilesDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("CompareFilesDialog")))
{
   mCompareFiles = false;
   mSkipBlankLinesForTextCompare = false;
   mSaveCompareResults = false;
   mHasDir1 = false;
   mHasDir2 = false;
   mHasDir3 = false;
   
   mNumFilesToCompare = 0;
   mNumDirsToCompare = 1;
   mTolerance = 1.0e-6;
   mCompareOption = 1;
   
   mBaseDirectory = "";
   mBaseString = "";
   
   // we can have up to 3 directories to compare
   mCompareStrings.Add("");
   mCompareStrings.Add("");
   mCompareStrings.Add("");
   
   mCompareDirs.Add("");
   mCompareDirs.Add("");
   mCompareDirs.Add("");
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~CompareFilesDialog()
//------------------------------------------------------------------------------
CompareFilesDialog::~CompareFilesDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void CompareFilesDialog::Create()
{
   #if DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage("CompareFilesDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   wxArrayString options;
   options.Add("Compare lines as text");
   options.Add("Compare lines numerically (skips strings and blank lines)");
   options.Add("Compare data columns numerically");
   
   //------------------------------------------------------
   // compare type
   //------------------------------------------------------
   mCompareOptionRadioBox =
      new wxRadioBox(this, ID_RADIOBOX, wxT("Compare Option"), wxDefaultPosition,
                     wxDefaultSize, options, 1, wxRA_SPECIFY_COLS);
   
   //------------------------------------------------------
   // compare directory
   //------------------------------------------------------
   
   mBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(400,-1), 0);
   
   mBaseDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *baseStringLabel =
      new wxStaticText(this, ID_TEXT, wxT("Compare Files Contain:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mBaseStrTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, mBaseString,
                     wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
   
   wxStaticText *numFilesBaseDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   mBaseUpdateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   wxFlexGridSizer *baseDirGridSizer = new wxFlexGridSizer(2, 0, 0);
   baseDirGridSizer->Add(mBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirGridSizer->Add(mBaseDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
 
   wxFlexGridSizer *baseFileGridSizer = new wxFlexGridSizer(3, 0, 0);
   baseFileGridSizer->Add(baseStringLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   baseFileGridSizer->Add(mBaseStrTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileGridSizer->Add(20, 20, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileGridSizer->Add(numFilesBaseDirLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   baseFileGridSizer->Add(mNumFilesInBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileGridSizer->Add(mBaseUpdateButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *baseDirSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Base Directory");
   
   baseDirSizer->Add(baseDirGridSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirSizer->Add(baseFileGridSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   
   //------------------------------------------------------
   // compare file names
   //------------------------------------------------------
   wxString dirArray[] = {"Directory1", "Directory2", "Directory3"};
   
   mCompareDirsComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT("Compare Directories"), wxDefaultPosition,
                     wxDefaultSize, 3, dirArray, wxCB_READONLY);
   mCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(400,-1), 0);
   
   mCompareDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *compareStringLabel =
      new wxStaticText(this, ID_TEXT, wxT("Compare Files Contain:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mCompareStrTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, mCompareStrings[0],
                     wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
   
   wxStaticText *numFilesInCompareDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   mCompareUpdateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxDefaultSize, 0);
   

   //---------- sizer
   wxFlexGridSizer *compareDirGridSizer = new wxFlexGridSizer(2, 0, 0);   
   compareDirGridSizer->Add(mCompareDirsComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(mCompareDirButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxFlexGridSizer *compareFileGridSizer = new wxFlexGridSizer(3, 0, 0);   
   compareFileGridSizer->Add(compareStringLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareFileGridSizer->Add(mCompareStrTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareFileGridSizer->Add(20, 20, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareFileGridSizer->Add(numFilesInCompareDirLabel, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareFileGridSizer->Add(mNumFilesInCompareDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareFileGridSizer->Add(mCompareUpdateButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *compareDirsSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Compare Directories");
   
   compareDirsSizer->Add(compareDirGridSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareDirsSizer->Add(compareFileGridSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   
   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   wxStaticText *numDirsToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Directories to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumDirsToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *numFilesToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *toleranceLabel =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance to be Used in Flagging:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mToleranceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, ToWxString(mTolerance),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   //---------- sizer
   wxFlexGridSizer *numFilesGridSizer = new wxFlexGridSizer(2, 0, 0);
   numFilesGridSizer->Add(numDirsToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumDirsToCompareTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   numFilesGridSizer->Add(numFilesToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumFilesToCompareTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);   
   numFilesGridSizer->Add(toleranceLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mToleranceTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   
   // Inner parentheses were added below to turn off warnings
   // need to verify intent was not "if (!(mCompareOption == 1)"
   // the concern is that since mCompareOption isn't Bool the # of possible
   // options is unknown. As written, this is true only if mCompareOption is 0,
   // because if mCompareOption !=0 then !nCompareOption == 0.
   //if (!(mCompareOption) == 1)
   //warnings persisted even with parentheses around mCompareOption, so rewrote
   //  using "!="
   if (mCompareOption != 1)
   {
      numFilesGridSizer->Hide(toleranceLabel);
      numFilesGridSizer->Hide(mToleranceTextCtrl);
   }
   
   mSkipBlankLinesCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Skip Blank Lines for Text Compare"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   mSaveResultCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Save Compare Results to File"),
                     wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *saveFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("File Name to Save:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   mSaveFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(400,-1), 0);
   
   mSaveBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);

   //---------- sizer
   wxFlexGridSizer *saveGridSizer = new wxFlexGridSizer(2, 0, 0);
   saveGridSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   saveGridSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *compareSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Compare");
   
   compareSizer->Add(numFilesGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mSkipBlankLinesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   pageBoxSizer->Add(mCompareOptionRadioBox, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(baseDirSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareDirsSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);

   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CompareFilesDialog::LoadData()
{
   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::LoadData() entered, mNumFilesToCompare = %d\n",
       mNumFilesToCompare);
   #endif
   wxString str;
   str.Printf("%d", mNumFilesToCompare);
   mNumFilesToCompareTextCtrl->SetValue(str);
   
   str.Printf("%d", mNumDirsToCompare);
   mNumDirsToCompareTextCtrl->SetValue(str);

   str.Printf("%g", mTolerance);
   mToleranceTextCtrl->SetValue(str);

   FileManager *fm = FileManager::Instance();
   mBaseDirectory = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[0] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[1] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[2] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirsComboBox->SetSelection(0);
   mSaveFileName = mBaseDirectory + "CompareNumericResults.out";
   mBaseDirTextCtrl->SetValue(mBaseDirectory);
   mCompareDirTextCtrl->SetValue(mCompareDirs[0]);
   mSaveFileTextCtrl->SetValue(mSaveFileName);
   
   // update file info in directory 1 and 2
   UpdateFileInfo(0, true);
   UpdateFileInfo(1, false);
   
   mSaveResultCheckBox->Enable();
   mSaveFileTextCtrl->Disable();
   mSaveBrowseButton->Disable();
   
   theOkButton->Enable();

   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::LoadData() leaving, mNumFilesToCompare = %d\n",
       mNumFilesToCompare);
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CompareFilesDialog::SaveData()
{
   long longNum;
   canClose = true;
      
   if (!mNumFilesToCompareTextCtrl->GetValue().ToLong(&longNum))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   mNumFilesToCompare = longNum;
   
   if (!mNumDirsToCompareTextCtrl->GetValue().ToLong(&longNum))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   mNumDirsToCompare = longNum;
   
   if (!mToleranceTextCtrl->GetValue().ToDouble(&mTolerance))
   {
      wxMessageBox("Invalid tolerance entered.");
      canClose = false;
      return;
   }
   
   
   mSaveFileName = mSaveFileTextCtrl->GetValue();

   mCompareFiles = true;
   if (mNumFilesToCompare <= 0)
   {
      wxMessageBox("There are no specific report files to compare.\n"
                   "Please check file names to compare.",
                   "GMAT Warning");
      canClose = false;
      mCompareFiles = false;
   }
   
   mSkipBlankLinesForTextCompare = mSkipBlankLinesCheckBox->GetValue();
   mSaveCompareResults = mSaveResultCheckBox->GetValue();
   
   #if DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::SaveData() mNumFilesToCompare=%d, "
       "mCompareFiles=%d, mTolerance=%e\n   mBaseDirectory=%s, mCompareDirs[0]=%s, "
       "mBaseString=%s, mCompareStrings[0]=%s\n", mNumFilesToCompare, mCompareFiles,
       mTolerance, mBaseDirectory.WX_TO_C_STRING, mCompareDirs[0].WX_TO_C_STRING,
       mBaseString.WX_TO_C_STRING, mCompareStrings[0].WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CompareFilesDialog::ResetData()
{
   canClose = true;
   mCompareFiles = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBaseDirButton)
   {
      wxDirDialog dialog(this, "Select a base directory", mBaseDirectory);
   
      if (dialog.ShowModal() == wxID_OK)
      {
         mBaseDirectory = dialog.GetPath();
         mBaseDirTextCtrl->SetValue(mBaseDirectory);
         mSaveFileName = mBaseDirectory + "/CompareNumericResults.out";
         mSaveFileTextCtrl->SetValue(mSaveFileName);
         //mSaveFileTextCtrl->SetValue(mBaseDirectory + "/CompareNumericResults.out");
         UpdateFileInfo(0, true);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() mBaseDirectory=%s\n",
             mBaseDirectory.WX_TO_C_STRING);
         #endif
      }
   }
   else if (event.GetEventObject() == mCompareDirButton)
   {
      int dirIndex = mCompareDirsComboBox->GetSelection();
      wxDirDialog dialog(this, "Select a compare dierctory", mCompareDirs[dirIndex]);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         if (dirIndex == 0)
            mHasDir1 = true;
         else if (dirIndex == 1)
            mHasDir2 = true;
         else if (dirIndex == 2)
            mHasDir3 = true;
         
         mCompareDirs[dirIndex] = dialog.GetPath();
         mCompareDirTextCtrl->SetValue(mCompareDirs[dirIndex]);
         UpdateFileInfo(dirIndex, false);

         // update number of directories to compare
         int numDirs = 0;
         if (mHasDir1) numDirs++;
         if (mHasDir2) numDirs++;
         if (mHasDir3) numDirs++;
         mNumDirsToCompare = numDirs;
         
         wxString str;
         str.Printf("%d", mNumDirsToCompare);
         mNumDirsToCompareTextCtrl->SetValue(str);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() mCompareDirs[%d]=%s\n",
             dirIndex, mCompareDirs[dirIndex].WX_TO_C_STRING);
         #endif
      }
   }
   else if (event.GetEventObject() == mBaseUpdateButton)
   {
      // update file info in base directory
      mBaseDirectory = mBaseDirTextCtrl->GetValue();
      mBaseString = mBaseStrTextCtrl->GetValue();
      mSaveFileName = mBaseDirectory + "/CompareNumericResults.out";
      mSaveFileTextCtrl->SetValue(mSaveFileName);
      UpdateFileInfo(0, true);
   }
   else if (event.GetEventObject() == mCompareUpdateButton)
   {
      int dirIndex = mCompareDirsComboBox->GetSelection();
      
      // update file info in compare directory
      mCompareDirs[dirIndex] = mCompareDirTextCtrl->GetValue();
      mCompareStrings[dirIndex] = mCompareStrTextCtrl->GetValue();
      UpdateFileInfo(dirIndex, false);
   }
   else if (event.GetEventObject() == mSaveBrowseButton)
   {
      wxString filename =
         wxFileSelector("Choose a file to save", mBaseDirectory, "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        wxFD_SAVE); //wxSAVE);
      
      if (!filename.empty())
      {
         mSaveFileTextCtrl->SetValue(filename);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() savefile=%s\n", filename.WX_TO_C_STRING);
         #endif
      }
      
   }
}

//------------------------------------------------------------------------------
// void OnRadioButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnRadioButtonClick(wxCommandEvent& event)
{
   mCompareOption = mCompareOptionRadioBox->GetSelection() + 1;
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   if (mSaveResultCheckBox->IsChecked())
   {
      mSaveFileTextCtrl->Enable();
      mSaveBrowseButton->Enable();
   }
   else
   {
      mSaveFileTextCtrl->Disable();
      mSaveBrowseButton->Disable();
   }
}


// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCompareDirsComboBox)
   {
      int currDir = mCompareDirsComboBox->GetSelection();
      mCompareDirTextCtrl->SetValue(mCompareDirs[currDir]);
   }
}


//------------------------------------------------------------------------------
// void OnTextEnterPress(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnTextEnterPress(wxCommandEvent& event)
{
   MessageInterface::ShowMessage("==> CompareFilesDialog::OnTextEnterPress() entered\n");
   
   wxObject *eventObj = event.GetEventObject();
   int dirIndex = mCompareDirsComboBox->GetSelection();
   
   if (eventObj == mBaseDirTextCtrl)
   {
      mBaseDirectory = mBaseDirTextCtrl->GetValue();
      mSaveFileName = mBaseDirectory + "/CompareNumericResults.out";
      mSaveFileTextCtrl->SetValue(mSaveFileName);
      UpdateFileInfo(0, true);
   }
   else if (eventObj == mCompareDirTextCtrl)
   {
      mCompareDirs[dirIndex] = mCompareDirTextCtrl->GetValue();
      UpdateFileInfo(0, false);
   }
   else if (eventObj == mBaseStrTextCtrl)
   {
      mBaseString = mBaseStrTextCtrl->GetValue();
      UpdateFileInfo(0, true);
   }
   else if (eventObj == mCompareStrTextCtrl)
   {
      mCompareStrings[dirIndex] = mCompareStrTextCtrl->GetValue();
      UpdateFileInfo(dirIndex, false);
   }
}


//------------------------------------------------------------------------------
// void UpdateFileInfo(Integer dir)
//------------------------------------------------------------------------------
void CompareFilesDialog::UpdateFileInfo(Integer dir, bool isBaseDir)
{
   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::UpdateFileInfo() entered, dir = %d, isBaseDir = %d\n",
       dir, isBaseDir);
   #endif
   
   if (isBaseDir)
   {
      mFileNamesInBaseDir = GetFilenamesContain(mBaseDirectory, mBaseString);
      mNumFilesInBaseDir = mFileNamesInBaseDir.GetCount();
      mNumFilesInBaseDirTextCtrl->SetValue("");
      *mNumFilesInBaseDirTextCtrl << mNumFilesInBaseDir;
   }
   else
   {
      mFileNamesInCompareDir = GetFilenamesContain(mCompareDirs[dir], mCompareStrings[dir]);
      mNumFilesInCompareDir = mFileNamesInCompareDir.GetCount();
      mNumFilesInCompareDirTextCtrl->SetValue("");
      *mNumFilesInCompareDirTextCtrl << mNumFilesInCompareDir;
   }
   
   // number of files to compare
   mNumFilesToCompareTextCtrl->SetValue("");
   if (mNumFilesInBaseDir == 0 || mNumFilesInCompareDir == 0)
      *mNumFilesToCompareTextCtrl << 0;
   else
      *mNumFilesToCompareTextCtrl << mNumFilesInBaseDir;
   
   mNumFilesToCompare = mNumFilesInBaseDir;
   
   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::UpdateFileInfo() leaving, mNumFilesInBaseDir = %d, "
       "mNumFilesToCompare = %d\n", mNumFilesInBaseDir, mNumFilesToCompare);
   #endif
}


//------------------------------------------------------------------------------
// wxArrayString GetFilenamesContain(const wxString &dirname,
//                                   const wxString &str)
//------------------------------------------------------------------------------
wxArrayString CompareFilesDialog::GetFilenamesContain(const wxString &dirname,
                                                      const wxString &str)
{
   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::GetFilenamesContain() entered, dirname = '%s', str = '%s'\n",
       dirname.WX_TO_C_STRING, str.WX_TO_C_STRING);
   #endif
   
   wxDir dir(dirname);
   wxString filename;
   wxString filepath;
   wxArrayString fileNames;
   
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt") ||
          filename.Contains(".data") || filename.Contains(".script") ||
          filename.Contains(".eph") || filename.Contains(".oem") ||
          filename.Contains(".e") || filename.Contains(".truth"))
      {
         // If not backup files
         // Add files ending 't' for report, txt, and script
         // 'a' for data, 'h' for eph and truth, 'm' for .oem, 'e' for .e
         if (filename.Last() == 't' || filename.Last() == 'a' ||
             filename.Last() == 'h' || filename.Last() == 'm' ||
             filename.Last() == 'e')
         {
            if (filename.Contains(str))
            {
               filepath = dirname + "/" + filename;
               fileNames.Add(filepath);
            }
         }
      }
      
      cont = dir.GetNext(&filename);
   }

   #ifdef DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::GetFilenamesContain() returning %d files\n", fileNames.size());
   #endif
   
   return fileNames;
}

