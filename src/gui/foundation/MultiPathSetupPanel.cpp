//$Id$
//------------------------------------------------------------------------------
//                              MultiPathSetupPanel
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

#include "MultiPathSetupPanel.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>           // for wxDir::Exists()

#include "bitmaps/up.xpm"
#include "bitmaps/down.xpm"
#include "bitmaps/OpenFolder.xpm"

//#define DEBUG_SETPATH 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MultiPathSetupPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON, MultiPathSetupPanel::OnButtonClick)
   EVT_BUTTON(ID_UP_BUTTON, MultiPathSetupPanel::OnUpButtonClick)
   EVT_BUTTON(ID_DOWN_BUTTON, MultiPathSetupPanel::OnDownButtonClick)
   EVT_BUTTON(ID_REMOVE_BUTTON, MultiPathSetupPanel::OnRemoveButtonClick)
   EVT_BUTTON(ID_BROWSE_BUTTON, MultiPathSetupPanel::OnBrowseButtonClick)
   EVT_LISTBOX(ID_LISTBOX, MultiPathSetupPanel::OnListBoxSelect)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// MultiPathSetupPanel(wxWindow *parent, const StringArray &pathNames)
//------------------------------------------------------------------------------
MultiPathSetupPanel::MultiPathSetupPanel(wxWindow *parent,
                                         const StringArray &pathNames)
   : wxPanel(parent)
{
   for (UnsignedInt i=0; i<pathNames.size(); i++)
      mPathNames.Add(pathNames[i].c_str());

   Create();
   LoadData();
   
   mHasDataChanged = false;
}


//------------------------------------------------------------------------------
// ~MultiPathSetupPanel()
//------------------------------------------------------------------------------
MultiPathSetupPanel::~MultiPathSetupPanel()
{
}


//------------------------------------------------------------------------------
// bool HasDataChanged()
//------------------------------------------------------------------------------
bool MultiPathSetupPanel::HasDataChanged()
{
   return mHasDataChanged;
}


//------------------------------------------------------------------------------
// const wxArrayString& GetPathNames()
//------------------------------------------------------------------------------
const wxArrayString& MultiPathSetupPanel::GetPathNames()
{
   mPathNames = mPathListBox->GetStrings();
   return mPathNames;
}


//------------------------------------------------------------------------------
// void UpdatePathNames(const StringArray &pathNames)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::UpdatePathNames(const StringArray &pathNames)
{
   #if DEBUG_SETPATH
   MessageInterface::ShowMessage
      ("MultiPathSetupPanel::UpdatePathNames() entered. There are %d input paths\n",
       pathNames.size());
   #endif
   
   mPathNames.Clear();
   for (UnsignedInt i=0; i<pathNames.size(); i++)
      mPathNames.Add(pathNames[i].c_str());
   
   LoadData();
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void MultiPathSetupPanel::Create()
{
   #if DEBUG_SETPATH
   MessageInterface::ShowMessage("MultiPathSetupPanel::Create() entered.\n");
   #endif
   
   int bsize = 3;
   wxArrayString emptyList;
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   
   wxBitmap upBitmap = wxBitmap(up_xpm);
   wxBitmap downBitmap = wxBitmap(down_xpm);
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   
   //------------------------------------------------------
   // create components
   //------------------------------------------------------
   
   //----- path listbox
   mPathListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(350, 100),
                    emptyList, wxLB_SINGLE);
   
   //----- up and down buttons
   wxBitmapButton *upButton =
      new wxBitmapButton(this, ID_UP_BUTTON, upBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   wxBitmapButton *downButton =
      new wxBitmapButton(this, ID_DOWN_BUTTON, downBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   
   wxBoxSizer *upDownSizer = new wxBoxSizer(wxVERTICAL);
   upDownSizer->Add(upButton, 0, wxALIGN_CENTER|wxALL, bsize);
   upDownSizer->Add(downButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //----- file name and browse button
   mFileTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(350, -1), 0);
   wxBitmapButton *browseButton =
      new wxBitmapButton(this, ID_BROWSE_BUTTON, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   
   //----- bottom buttons
   mAddButton = new wxButton(this, ID_BUTTON, wxT("Add"),
                             wxDefaultPosition, wxDefaultSize, 0);
   mReplaceButton = new wxButton(this, ID_BUTTON, wxT("Replace"),
                                 wxDefaultPosition, wxDefaultSize, 0);
   mRemoveButton = new wxButton(this, ID_REMOVE_BUTTON, wxT("Remove"),
                                wxDefaultPosition, wxDefaultSize, 0);
   
   wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
   bottomSizer->Add(mAddButton, 0, wxALIGN_CENTER|wxALL, bsize);
   bottomSizer->Add(mReplaceButton, 0, wxALIGN_CENTER|wxALL, bsize);
   bottomSizer->Add(mRemoveButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //----- add to the sizer
   wxFlexGridSizer *pathSizer = new wxFlexGridSizer(2, 0, 0);
   pathSizer->Add(mPathListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   pathSizer->Add(upDownSizer, 0, wxALIGN_CENTER|wxALL, bsize);   
   pathSizer->Add(mFileTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   pathSizer->Add(browseButton, 0, wxALIGN_CENTER|wxALL, bsize);   
   pathSizer->Add(bottomSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   pathSizer->Add(20, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);   
   pageSizer->Add(pathSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   SetSizer(pageSizer);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void MultiPathSetupPanel::LoadData()
{
   #if DEBUG_SETPATH
   MessageInterface::ShowMessage
      ("MultiPathSetupPanel::LoadData() entered. There are %d paths\n",
       mPathNames.size());
   #endif
   
   mPathListBox->Clear();
   mPathListBox->Set(mPathNames);
   
   // populate FileTextCtrl with first item by creating event
   mPathListBox->SetSelection(0);
   wxCommandEvent tempEvent;
   tempEvent.SetEventObject(mPathListBox);
   OnListBoxSelect(tempEvent);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void MultiPathSetupPanel::SaveData()
{
}


//------------------------------------------------------------------------------
// void OnListBoxSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnListBoxSelect(wxCommandEvent& event)
{
   wxString selStr = mPathListBox->GetStringSelection();
   mFileTextCtrl->SetValue(selStr);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnButtonClick(wxCommandEvent& event)
{
   wxString pathname = mFileTextCtrl->GetValue();
   int selected = mPathListBox->GetSelection();
   wxString pathSelected = mPathListBox->GetString(selected);
   
   // check if directory exist
   if (!wxDir::Exists(pathname))
   {
      wxMessageBox(wxT("The directory \"" + pathname + "\" does not exist.\n"),
                   wxT("Directory Error"));
      return;
   }
   
   pathname = pathname.Strip(wxString::both);
   if (!pathname.EndsWith("/"))
      pathname = pathname + "/";
   
   if (event.GetEventObject() == mAddButton)
   {
      if (pathname != "" && mPathListBox->FindString(pathname) == wxNOT_FOUND)
      {
         // Add to top of the list (loj: 2008.10.20)
         //mPathListBox->Append(pathname);
         mPathListBox->Insert(pathname, 0);
         mPathListBox->SetStringSelection(pathname);
      }
   }
   else if (event.GetEventObject() == mReplaceButton)
   {
      if (pathSelected != "")
         mPathListBox->SetString(selected, pathname);
   }
   
   mFileTextCtrl->SetValue("");
   mHasDataChanged = true;
}


//------------------------------------------------------------------------------
// void OnUpButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnUpButtonClick(wxCommandEvent& event)
{
   int sel = mPathListBox->GetSelection();
   
   if (sel-1 >= 0)
   {
      wxString frontString = mPathListBox->GetString(sel-1);
      mPathListBox->SetString(sel-1, mPathListBox->GetStringSelection());
      mPathListBox->SetString(sel, frontString);
      mPathListBox->SetSelection(sel-1);
      mHasDataChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnDownButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnDownButtonClick(wxCommandEvent& event)
{
   unsigned int sel = mPathListBox->GetSelection();
   
   if (sel+1 >= 1 && (sel+1) < mPathListBox->GetCount())
   {
      wxString rearString = mPathListBox->GetString(sel+1);
      mPathListBox->SetString(sel+1, mPathListBox->GetStringSelection());
      mPathListBox->SetString(sel, rearString);      
      mPathListBox->SetSelection(sel+1);
      mHasDataChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnRemoveButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnRemoveButtonClick(wxCommandEvent& event)
{
   int selected = mPathListBox->GetSelection();
   mPathListBox->Delete(selected);
   mFileTextCtrl->SetValue("");
   mHasDataChanged = true;
}


//------------------------------------------------------------------------------
// void OnBrowseButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MultiPathSetupPanel::OnBrowseButtonClick(wxCommandEvent& event)
{
   wxString defPath = mFileTextCtrl->GetValue();
   
   wxDirDialog dialog(this, "Choose a directory", defPath);
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString pathname;
      pathname = dialog.GetPath().c_str();
      pathname.Replace("\\", "/");
      mFileTextCtrl->SetValue(pathname + "/");
   }
}

