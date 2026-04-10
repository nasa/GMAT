//$Id$
//------------------------------------------------------------------------------
//                              FileUpdateDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Thomas G. Grubb, NASA/GSFC
// Created: 2016/01/04
//
/**
 * Implements FileUpdateDialog class.
 */
//------------------------------------------------------------------------------

#include "FileUpdateDialog.hpp"
#include "GmatAppData.hpp"         // for SetIcon()
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "FileUpdaterSVN.hpp"
#include "IFileUpdater.hpp"


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FileUpdateDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_OK, FileUpdateDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, FileUpdateDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON_SELECT, FileUpdateDialog::OnSelect)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FileUpdateDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//            GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs FileUpdateDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 *
 */
//------------------------------------------------------------------------------
FileUpdateDialog::FileUpdateDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   #ifdef DEBUG_FILEUPATE
   MessageInterface::ShowMessage("FileUpdateDialog::FileUpdateDialog() entered\n");
   #endif
   
   mIsEmpty = false;
   // Select button
   mSelectButton = new wxButton(this, ID_BUTTON_SELECT, "Select All");
   wxBoxSizer *selectSizer = new wxBoxSizer(wxHORIZONTAL);
   selectSizer->Add(mSelectButton, 0, wxALIGN_LEFT);

   mGrid = new wxGrid(this, ID_CHECKLISTBOX, wxDefaultPosition, wxSize(640, 480));
   mGrid->CreateGrid(0, 3);
   mGrid->SetColLabelValue(0, "Update");
   mGrid->SetColLabelValue(1, "Filename");
   mGrid->SetColLabelValue(2, "Status");
   mGrid->SetColFormatBool(0);
   mGrid->UseNativeColHeader();

   // OK button
   mOkButton = new wxButton(this, ID_BUTTON_OK, "OK");
   // Cancel button
   mCancelButton = new wxButton(this, ID_BUTTON_CANCEL, "Cancel");
   wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
   buttonSizer->Add(mOkButton, 0, wxALIGN_CENTRE|wxALL);
   buttonSizer->Add(mCancelButton, 0, wxALIGN_CENTRE|wxALL);
   
   // Add to page sizer and set sizer to this dialog
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(selectSizer, 0, wxALIGN_LEFT|wxALL);
   pageSizer->Add(mGrid, 1, wxGROW|wxALIGN_CENTRE|wxALL);
   pageSizer->Add(10, 10);
   pageSizer->Add(buttonSizer, 0, wxALIGN_CENTRE|wxALL);
   
   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
   
   CenterOnScreen(wxBOTH);

   updateUtil = (IFileUpdater *) new FileUpdaterSVN("","");
   InitializeFiles();

   #ifdef DEBUG_FILEUPATE
   MessageInterface::ShowMessage("FileUpdateDialog::FileUpdateDialog() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeFiles()
//------------------------------------------------------------------------------
/**
 * Initializes the files checklistbox
 */
//------------------------------------------------------------------------------
void FileUpdateDialog::InitializeFiles()
{
   if (!updateUtil->CheckForUpdates())
   {
      mIsEmpty = true;
      return;
   }

   std::vector<IFileUpdater::repoFile> files = updateUtil->GetFiles();

   if (files.size() == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::INFO_, "All files are up to date");
      mIsEmpty = true;
      return;
   }
   std::vector<IFileUpdater::repoFile>::iterator it;
   int aRow;
   wxString a;

   for(it=files.begin() ; it < files.end(); it++ ) {
      mGrid->AppendRows();
      aRow = mGrid->GetNumberRows() - 1;
      // mGrid->SetCellValue("1", aRow, 0); // set checked to true
      mGrid->SetCellValue(aRow, 0, "1"); // set checked to true
      mGrid->SetCellValue(aRow, 1, it->name);
//      switch (it->status)
//      {
//         case IFileUpdater::NOMINAL:
//            mGrid->SetCellValue("Nominal", aRow, 2);
//            break;
//         case IFileUpdater::ADDED:
//            mGrid->SetCellValue("Added", aRow, 2);
//            break;
//         case IFileUpdater::DELETED:
//            mGrid->SetCellValue("Deleted", aRow, 2);
//            break;
//         case IFileUpdater::LOCALMODIFIED:
//            mGrid->SetCellValue("Modified (Local)", aRow, 2);
//            break;
//         case IFileUpdater::LOCALMISSING:
//            mGrid->SetCellValue("Missing (Local)", aRow, 2);
//            break;
//         case IFileUpdater::MODIFIED:
//            mGrid->SetCellValue("Modified", aRow, 2);
//            break;
//      }
      switch (it->status)
      {
         case IFileUpdater::NOMINAL:
             mGrid->SetCellValue(aRow, 2, "Nominal");
            break;
         case IFileUpdater::ADDED:
            mGrid->SetCellValue(aRow, 2, "Added");
            break;
         case IFileUpdater::DELETED:
            mGrid->SetCellValue(aRow, 2, "Deleted");
            break;
         case IFileUpdater::LOCALMODIFIED:
            mGrid->SetCellValue(aRow, 2, "Modified (Local)");
            break;
         case IFileUpdater::LOCALMISSING:
            mGrid->SetCellValue(aRow, 2, "Missing (Local)");
            break;
         case IFileUpdater::MODIFIED:
            mGrid->SetCellValue(aRow, 2, "Modified");
            break;
      }

   }
   mGrid->AutoSizeColumns();
   int aSize=0;
   for (int i=0; i < mGrid->GetNumberCols(); i++)
      aSize += mGrid->GetColSize(i);
   int w, h;
   mGrid->GetClientSize(&w, &h);
   mGrid->SetColSize(1, w-(aSize-mGrid->GetColSize(1))-mGrid->GetRowLabelSize()-mGrid->GetScrollThumb(wxVERTICAL)); 

}


//------------------------------------------------------------------------------
// bool IsEmpty()
//------------------------------------------------------------------------------
/**
 * Are there files to update?
 */
//------------------------------------------------------------------------------
bool FileUpdateDialog::IsEmpty()
{
   return mIsEmpty;
}


//------------------------------------------------------------------------------
// void InitializeFiles()
//------------------------------------------------------------------------------
/**
 * Initializes the files checklistbox
 */
//------------------------------------------------------------------------------
bool FileUpdateDialog::GenerateBatchFile()
{
   if (mGrid->GetNumberRows() == 0) return false;

   std::vector<IFileUpdater::repoFile> files = updateUtil->GetFiles();
   std::vector<IFileUpdater::repoFile>::iterator it;
   int aRow;
   wxString a;
   int i;

   // get the operation for files
   for(it=files.begin(), i=0; it < files.end(); it++, i++ ) 
   {
      if (mGrid->GetCellValue(i, 0) != "")
      {
         if (it->status == IFileUpdater::LOCALMODIFIED)
            it->operation = IFileUpdater::REVERT;
         else
            it->operation = IFileUpdater::UPDATE;
      }
      else
         it->operation = IFileUpdater::NONE;
   }

   updateUtil->SetFiles(files);
   FileManager *fm = FileManager::Instance();
   
   return updateUtil->SaveUpdateScript(fm->GetBinDirectory()+IFileUpdater::ShellFile);
}


//------------------------------------------------------------------------------
// void OnOK(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Closes the dialog
 */
//------------------------------------------------------------------------------
void FileUpdateDialog::OnOK(wxCommandEvent &event)
{
   if (GenerateBatchFile())
   {
      MessageInterface::PopupMessage
         (Gmat::INFO_, "You must restart GMAT to update files");
   }
   Close();
}


//------------------------------------------------------------------------------
// void OnCancel(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Closes the dialog
 */
//------------------------------------------------------------------------------
void FileUpdateDialog::OnCancel(wxCommandEvent &event)
{
   Close();
}


//------------------------------------------------------------------------------
// void OnSelect(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Select All/Deselect All
 */
//------------------------------------------------------------------------------
void FileUpdateDialog::OnSelect(wxCommandEvent &event)
{
   if (mGrid->GetNumberRows() == 0) return;

   wxString IsChecked = mGrid->GetCellValue(0,0);
   if (IsChecked == "")
      IsChecked = "1";
   else
      IsChecked = "";
   for (int i=0; i<mGrid->GetNumberRows(); i++)
   {
      mGrid->SetCellValue(i, 0, IsChecked);
   }
}

