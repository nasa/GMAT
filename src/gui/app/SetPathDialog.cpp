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
// Created: 2006/03/29
//
/**
 * Shows dialog for setting path for various files used in the system.
 */
//------------------------------------------------------------------------------

#include "SetPathDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MultiPathSetupPanel.hpp"
#include "SinglePathSetupPanel.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>           // for wxDir::Exists()

//#define DEBUG_SETPATH_DIALOG
//#define DEBUG_SETPATH_DIALOG_SAVE

BEGIN_EVENT_TABLE(SetPathDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_READ, SetPathDialog::OnReadButtonClick)
   EVT_BUTTON(ID_BUTTON_SAVE, SetPathDialog::OnSaveButtonClick)
END_EVENT_TABLE()
   
//------------------------------------------------------------------------------
// SetPathDialog(wxWindow *parent)
//------------------------------------------------------------------------------
SetPathDialog::SetPathDialog(wxWindow *parent)
   : GmatDialog(parent, -1, "SetPathDialog")
{
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~SetPathDialog()
//------------------------------------------------------------------------------
SetPathDialog::~SetPathDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void SetPathDialog::Create()
{
   #ifdef DEBUG_SETPATH_DIALOG_CREATE
   MessageInterface::ShowMessage("SetPathDialog::Create() entered.\n");
   #endif
   
   FileManager *fm = FileManager::Instance();
   mStartupFilePath = (fm->GetFullStartupFilePath()).c_str();
   
   //----- read startup file
   wxButton *readButton =
      new wxButton(this, ID_BUTTON_READ, wxT("Read Other Startup File"),
                   wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *currFileText =
      new wxStaticText(this, -1, wxT("Current Startup File:"), 
                       wxDefaultPosition, wxDefaultSize, 0 );
   mReadFileTextCtrl =
      new wxTextCtrl(this, -1, wxT(""),
                     wxDefaultPosition, wxSize(350, -1), 0);
   mReadFileTextCtrl->Disable();
   
   //----- write startup file
   wxButton *saveButton =
      new wxButton(this, ID_BUTTON_SAVE, wxT("Save Current Startup File"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mSaveFileTextCtrl =
      new wxTextCtrl(this, -1, wxT(""),
                     wxDefaultPosition, wxSize(350, -1), 0);
   mSaveFileTextCtrl->Disable();
   
   //----- add to sizer
   GmatStaticBoxSizer *startupSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Startup File");
   startupSizer->Add(readButton, 0, wxALIGN_LEFT|wxALL, 2);
   startupSizer->Add(currFileText, 0, wxALIGN_LEFT|wxALL, 2);
   startupSizer->Add(mReadFileTextCtrl, 0, wxALIGN_CENTER|wxGROW|wxALL, 2);
   startupSizer->Add(saveButton, 0, wxALIGN_LEFT|wxALL, 2);
   startupSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_CENTER|wxGROW|wxALL, 2);
   
   //----- create Notebook
   mPathNotebook =
      new wxNotebook(this, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxGROW);
   
   StringArray gmatPaths = fm->GetAllGmatFunctionPaths();
   
   //----- add panels to notebook   
   mGmatFunPathPanel = new MultiPathSetupPanel(mPathNotebook, gmatPaths);
   mPathNotebook->AddPage(mGmatFunPathPanel, "GMAT Function");
   
   StringArray matlabPaths = fm->GetAllMatlabFunctionPaths();
   mMatlabPathPanel = new MultiPathSetupPanel(mPathNotebook, matlabPaths);
   mPathNotebook->AddPage(mMatlabPathPanel, "MATLAB Function");
   
   std::string outputPath = fm->GetFullPathname("OUTPUT_PATH");
   
   #ifdef DEBUG_SETPATH_DIALOG_CREATE
   MessageInterface::ShowMessage("   outputPath='%s'\n", outputPath.c_str());
   #endif
   
   mOutputPathPanel = new SinglePathSetupPanel(mPathNotebook, outputPath.c_str());
   mPathNotebook->AddPage(mOutputPathPanel, "Output");
   
   theMiddleSizer->Add(startupSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, 5);
   theMiddleSizer->Add(mPathNotebook, 1, wxALIGN_CENTER|wxGROW|wxALL, 5);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SetPathDialog::LoadData()
{
   FileManager *fm = FileManager::Instance();
   wxString startupPath = (fm->GetFullStartupFilePath()).c_str();
   mReadFileTextCtrl->SetValue(startupPath.c_str());
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SetPathDialog::SaveData()
{
   #ifdef DEBUG_SETPATH_DIALOG_SAVE
   MessageInterface::ShowMessage("SetPathDialog::SaveData() entered.\n");
   #endif
   
   canClose = true;
   
   FileManager *fm = FileManager::Instance();
   wxArrayString pathNames;
   
   // Gmat Function paths
   if (mGmatFunPathPanel->HasDataChanged())
   {
      #ifdef DEBUG_SETPATH_DIALOG_SAVE
      MessageInterface::ShowMessage("   Saving GmatFunction paths...\n");
      #endif
      
      pathNames = mGmatFunPathPanel->GetPathNames();
      
      #ifdef DEBUG_SETPATH_DIALOG_SAVE
      MessageInterface::ShowMessage("   ...Adding %d paths\n", pathNames.GetCount());
      #endif
      
      fm->ClearGmatFunctionPath();
      for (UnsignedInt i=0; i<pathNames.GetCount(); i++)
      {
         #ifdef DEBUG_SETPATH_DIALOG_SAVE
         MessageInterface::ShowMessage("   ...Adding '%s'\n", pathNames[i].c_str());
         #endif
         
         fm->AddGmatFunctionPath(pathNames[i].c_str(), false);
      }
   }
   
   // Matlab Function paths
   if (mMatlabPathPanel->HasDataChanged())
   {
      #ifdef DEBUG_SETPATH_DIALOG_SAVE
      MessageInterface::ShowMessage("   Saving MatlabFunction paths...\n");
      #endif
      
      pathNames = mMatlabPathPanel->GetPathNames();
      fm->ClearMatlabFunctionPath();
      for (UnsignedInt i=0; i<pathNames.GetCount(); i++)
         fm->AddMatlabFunctionPath(pathNames[i].c_str(), false);
   }
   
   // Log file path
   if (mOutputPathPanel->HasDataChanged())
   {
      std::string pathName = mOutputPathPanel->GetFullPathName().WX_TO_STD_STRING;
      if (wxDir::Exists(pathName.c_str()))
      {
         #ifdef DEBUG_SETPATH_DIALOG_SAVE
         MessageInterface::ShowMessage("   Saving Log path to '%s'\n", pathName.c_str());
         #endif
         
         fm->SetAbsPathname("OUTPUT_PATH", pathName);
         MessageInterface::SetLogPath(pathName);
      }
      else
      {
         wxString str;
         str.Printf("The directory \" %s \" does not exist.\n", pathName.c_str());
         wxMessageBox(str,
                      wxT("Directory Error"));
         
         // find page
         int numPage = mPathNotebook->GetPageCount();
         for (int i=0; i<numPage; i++)
         {
            if (mPathNotebook->GetPageText(i) == "Output")
            {
               mPathNotebook->SetSelection(i);
               break;
            }
         }
         canClose = false;
      }
   }
   
   #ifdef DEBUG_SETPATH_DIALOG_SAVE
   MessageInterface::ShowMessage("SetPathDialog::SaveData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void SetPathDialog::ResetData()
{
   canClose = true;
}


//------------------------------------------------------------------------------
// void OnReadButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SetPathDialog::OnReadButtonClick(wxCommandEvent& event)
{
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage("SetPathDialog::OnReadButtonClick() entered\n");
   #endif
   
   FileManager *fm = FileManager::Instance();
   // Do we want default directory to be where executable is?
   // Set it blank, so that system can show current working directory for now.
   //wxString defDir = fm->GetStartupFileDir();
   wxString defDir;
   
   #ifdef DEBUG_SETPATH
   MessageInterface::ShowMessage(" defDir='%s'\n", defDir.c_str());
   #endif
   
   wxFileDialog dialog(this, _T("Choose a file"), defDir, _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;      
      filename = dialog.GetPath().c_str();
      
      if (!filename.IsSameAs(mStartupFilePath))
      {
         mReadFileTextCtrl->SetValue(filename);
         try
         {
            fm->ReadStartupFile(filename.WX_TO_STD_STRING);
            StringArray paths = fm->GetAllGmatFunctionPaths();
            mGmatFunPathPanel->UpdatePathNames(paths);
            paths = fm->GetAllMatlabFunctionPaths();
            mMatlabPathPanel->UpdatePathNames(paths);
            mStartupFilePath = filename;
         }
         catch(BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnSaveButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SetPathDialog::OnSaveButtonClick(wxCommandEvent& event)
{
   wxFileDialog dialog(this, _T("Choose a file"), "", _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;      
      filename = dialog.GetPath().c_str();
      
      mSaveFileTextCtrl->SetValue(filename);
      try
      {
         FileManager *fm = FileManager::Instance();
         fm->WriteStartupFile(filename.WX_TO_STD_STRING);            
      }
      catch(BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      }
   }
}
