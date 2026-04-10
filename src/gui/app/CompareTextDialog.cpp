//$Id$
//------------------------------------------------------------------------------
//                              CompareTextDialog
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
// Created: 2006/06/27
//
/**
 * Shows dialog for comparing text line by line.
 */
//------------------------------------------------------------------------------

#include "CompareTextDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_COMPARE_TEXT_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareTextDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CompareTextDialog::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, CompareTextDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, CompareTextDialog::OnComboBoxChange)
   EVT_TEXT_ENTER(ID_TEXTCTRL, CompareTextDialog::OnTextEnterPress)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// CompareTextDialog(wxWindow *parent, bool compareNumeric = false)
//------------------------------------------------------------------------------
CompareTextDialog::CompareTextDialog(wxWindow *parent, bool compareNumeric)
   : GmatDialog(parent, -1, wxString(_T("CompareTextDialog")))
{
   mTolerance = 1.0e-6;
   mIsNumericCompare = compareNumeric;
   mCompareFiles = false;
   mSaveCompareResults = false;
   mHasDir1 = false;
   mHasDir2 = false;
   mHasDir3 = false;
   
   mNumFilesToCompare = 0;
   mNumDirsToCompare = 1;
   mBaseDirectory = "";
   mBaseString = "";
   
   mCompareDirs.Add("");
   mCompareDirs.Add("");
   mCompareDirs.Add("");
   
   mCompareStrings.Add("");
   mCompareStrings.Add("");
   mCompareStrings.Add("");
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~CompareTextDialog()
//------------------------------------------------------------------------------
CompareTextDialog::~CompareTextDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void CompareTextDialog::Create()
{
   #if DEBUG_COMPARE_TEXT_DIALOG
   MessageInterface::ShowMessage("CompareTextDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   
   //------------------------------------------------------
   // compare base directory
   //------------------------------------------------------
   
   mBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,-1), 0);
   
   mBaseDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,-1), 0);
   
   wxStaticText *basePrefixLabel =
      new wxStaticText(this, ID_TEXT, wxT("Prefix to use for file name:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mBaseStringTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(80,-1), 0);
   mBaseStringButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxSize(60,-1), 0);
   
   wxStaticText *numFilesBaseDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files (.txt, .report, .data, .script, .eph, .oem, .e, .truth):"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   
   wxFlexGridSizer *baseDirSizer = new wxFlexGridSizer(2, 0, 0);
   baseDirSizer->Add(mBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirSizer->Add(mBaseDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxFlexGridSizer *baseFileSizer = new wxFlexGridSizer(3, 0, 0);
   baseFileSizer->Add(basePrefixLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   baseFileSizer->Add(mBaseStringTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileSizer->Add(mBaseStringButton, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileSizer->Add(numFilesBaseDirLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   baseFileSizer->Add(mNumFilesInBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseFileSizer->Add(20,20);
   
   GmatStaticBoxSizer *baseSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Base Directory");
   
   baseSizer->Add(baseDirSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseSizer->Add(baseFileSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   
   //------------------------------------------------------
   // compare file names
   //------------------------------------------------------
   wxString dirArray[] = {"Directory1", "Directory2", "Directory3"};
   
   mCompareDirsComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT("Compare Directories"),
                     wxDefaultPosition, wxDefaultSize, 3, dirArray,
                     wxCB_READONLY);
   mCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,-1), 0);
   
   mCompareDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,-1), 0);
   
   wxStaticText *comparePrefixLabel =
      new wxStaticText(this, ID_TEXT, wxT("Prefix to use for file name:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mComparePrefixTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(80,-1), 0);
   mComparePrefixButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxSize(60,-1), 0);
   
   wxStaticText *numFilesInCompareDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files (.txt, .report, .data, .script, .eph, .oem, .e, .truth):"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   //---------- sizer
   wxFlexGridSizer *compareDirSizer = new wxFlexGridSizer(2, 0, 0);   
   compareDirSizer->Add(mCompareDirsComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirSizer->Add(mCompareDirButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxFlexGridSizer *compareFileSizer = new wxFlexGridSizer(3, 0, 0);   
   compareFileSizer->Add(comparePrefixLabel, 0, wxALL, bsize);
   compareFileSizer->Add(mComparePrefixTextCtrl, 0, wxALL|wxGROW, bsize);
   compareFileSizer->Add(mComparePrefixButton, 0, wxALL|wxGROW, bsize);
   compareFileSizer->Add(numFilesInCompareDirLabel, 0, wxALL|wxGROW, bsize);
   compareFileSizer->Add(mNumFilesInCompareDirTextCtrl, 0, wxALL|wxGROW, bsize);
   compareFileSizer->Add(20, 20);
   
   GmatStaticBoxSizer *compareDirsSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Compare Directories");
   
   compareDirsSizer->Add(compareDirSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareDirsSizer->Add(compareFileSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   
   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   wxStaticText *numDirsToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Directories to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumDirsToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   wxStaticText *numFilesToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   wxStaticText *toleranceLabel =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance Used in Compare"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mToleranceTextCtrl =
      //new wxTextCtrl(this, ID_TEXTCTRL, wxT(ToWxString(mTolerance)),
      new wxTextCtrl(this, ID_TEXTCTRL, ToWxString(mTolerance),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   //---------- sizer
   wxFlexGridSizer *numFilesGridSizer = new wxFlexGridSizer(2, 0, 0);
   numFilesGridSizer->Add(numDirsToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumDirsToCompareTextCtrl, 0,
                          wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   numFilesGridSizer->Add(numFilesToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumFilesToCompareTextCtrl, 0,
                          wxALIGN_RIGHT|wxALL|wxGROW, bsize);   
   numFilesGridSizer->Add(toleranceLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mToleranceTextCtrl, 0,
                          wxALIGN_RIGHT|wxALL|wxGROW, bsize);   
   
   if (!mIsNumericCompare)
   {
      numFilesGridSizer->Hide(toleranceLabel);
      numFilesGridSizer->Hide(mToleranceTextCtrl);
   }
   
   mSaveResultCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Save Compare Results to File"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *saveFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("File Name to Save:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   mSaveFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,-1), 0);
   
   mSaveBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,-1), 0);

   //---------- sizer
   wxFlexGridSizer *saveGridSizer = new wxFlexGridSizer(2, 0, 0);
   saveGridSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   saveGridSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *compareSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Compare");
   
   compareSizer->Add(numFilesGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   pageBoxSizer->Add(baseSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareDirsSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CompareTextDialog::LoadData()
{
   wxString str;
   str.Printf("%d", mNumFilesToCompare);
   mNumFilesToCompareTextCtrl->SetValue(str);
   
   str.Printf("%d", mNumDirsToCompare);
   mNumDirsToCompareTextCtrl->SetValue(str);


   FileManager *fm = FileManager::Instance();
   mBaseDirectory = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[0] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[1] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[2] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirsComboBox->SetSelection(0);
   mSaveFileName = mBaseDirectory + "CompareTextResults.out";
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
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CompareTextDialog::SaveData()
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
   mSaveFileName = mSaveFileTextCtrl->GetValue();
   
   mCompareFiles = true;
   if (mNumFilesToCompare <= 0)
   {
      wxMessageBox("There are no specific report files to compare.\n"
                   "Please check file names to compare.",
                   wxT("GMAT Warning"));
      canClose = false;
      mCompareFiles = false;
   }
   
   mSaveCompareResults = mSaveResultCheckBox->GetValue();
   
   #if DEBUG_COMPARE_TEXT_DIALOG
   MessageInterface::ShowMessage
      ("CompareTextDialog::SaveData() mNumFilesToCompare=%d, "
       "mCompareFiles=%d\n   mBaseDirectory=%s, mCompareDirs[0]=%s\n",
       mNumFilesToCompare, mCompareFiles,
       mBaseDirectory.c_str(), mCompareDirs[0].c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CompareTextDialog::ResetData()
{
   canClose = true;
   mCompareFiles = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBaseDirButton)
   {
      wxDirDialog dialog(this, "Select a base directory", mBaseDirectory);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         mBaseDirectory = dialog.GetPath();
         mBaseDirTextCtrl->SetValue(mBaseDirectory);
         mSaveFileTextCtrl->SetValue(mBaseDirectory + "/CompareTextResults.out");
         UpdateFileInfo(0, true);
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() mBaseDirectory=%s\n",
             mBaseDirectory.c_str());
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
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() mCompareDirs[%d]=%s\n",
             dirIndex, mCompareDirs[dirIndex].c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mBaseStringButton)
   {
      mBaseString = mBaseStringTextCtrl->GetValue();
      UpdateFileInfo(0, true);
   }
   else if (event.GetEventObject() == mComparePrefixButton)
   {
      int dirIndex = mCompareDirsComboBox->GetSelection();
      mCompareStrings[dirIndex] = mComparePrefixTextCtrl->GetValue();
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
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() savefile=%s\n", filename.c_str());
         #endif
      }
      
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnCheckBoxChange(wxCommandEvent& event)
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


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCompareDirsComboBox)
   {
      int currDir = mCompareDirsComboBox->GetSelection();
      mCompareDirTextCtrl->SetValue(mCompareDirs[currDir]);
      mComparePrefixTextCtrl->SetValue(mCompareStrings[currDir]);
   }
}


//------------------------------------------------------------------------------
// void OnTextEnterPress(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnTextEnterPress(wxCommandEvent& event)
{
}


//------------------------------------------------------------------------------
// void UpdateFileInfo(Integer dir)
//------------------------------------------------------------------------------
void CompareTextDialog::UpdateFileInfo(Integer dir, bool isBaseDir)
{
   #if DEBUG_COMPARE_TEXT_DIALOG   
   MessageInterface::ShowMessage
      ("CompareTextDialog::UpdateFileInfo() entered, dir=%d, isBaseDir=%d\n",
       dir, isBaseDir);
   #endif
   
   if (isBaseDir)
   {
      mFileNamesInBaseDir = GetFilenames(mBaseDirectory, mBaseString);
      mNumFilesInBaseDir = mFileNamesInBaseDir.GetCount();
      mNumFilesInBaseDirTextCtrl->SetValue("");
      *mNumFilesInBaseDirTextCtrl << mNumFilesInBaseDir;
   }
   else
   {
      mFileNamesInCompareDir = GetFilenames(mCompareDirs[dir], mCompareStrings[dir]);
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
   
}


//------------------------------------------------------------------------------
// wxArrayString GetFilenames(const wxString &dirname, const wxString &prefix)
//------------------------------------------------------------------------------
wxArrayString CompareTextDialog::GetFilenames(const wxString &dirname,
                                              const wxString &prefix)
{
   #if DEBUG_COMPARE_TEXT_DIALOG   
   MessageInterface::ShowMessage
      ("CompareTextDialog::GetFilenames() entered, dirname='%s', "
       "prefix='%s'\n", dirname.c_str(), prefix.c_str());
   #endif
   
   wxDir dir(dirname);
   wxString filename;
   wxString filepath;
   wxArrayString fileNames;
   size_t prefixLen = prefix.Len();
   
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt") ||
          filename.Contains(".data") || filename.Contains(".script") ||
          filename.Contains(".eph") || filename.Contains(".oem") ||
          filename.Contains(".e") || filename.Contains(".truth"))
      {
         // if it has prefix
         if (filename.Left(prefixLen) == prefix)
         {
            // If not backup files
            // Add files ending 't' for report, txt, and script
            // 'a' for data, 'h' for eph and truth, 'm' for .oem, 'e' for .e
            if (filename.Last() == 't' || filename.Last() == 'a' ||
                filename.Last() == 'h' || filename.Last() == 'm' ||
                filename.Last() == 'e')
            {
               filepath = dirname + "/" + filename;
               fileNames.Add(filepath);
            }
         }
      }
      
      cont = dir.GetNext(&filename);
   }
   
   return fileNames;
}

