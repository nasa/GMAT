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
 * Shows dialog where various run script folder option can be selected.
 */
//------------------------------------------------------------------------------

#include "TextEphemFileDialog.hpp"
#include "FileManager.hpp"
#include "TextEphemFile.hpp"
#include "TimeSystemConverter.hpp"  // for TimeSystemConverter::
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_EPHEM_FILE_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TextEphemFileDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, TextEphemFileDialog::OnButtonClick)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// TextEphemFileDialog(wxWindow *parent)
//------------------------------------------------------------------------------
TextEphemFileDialog::TextEphemFileDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("TextEphemFileDialog")))
{
   mCreateEphemFile = false;
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~TextEphemFileDialog()
//------------------------------------------------------------------------------
TextEphemFileDialog::~TextEphemFileDialog()
{
   theGuiManager->UnregisterListBox("Spacecraft", mSpacecraftListBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void TextEphemFileDialog::Create()
{
   #if DEBUG_EPHEM_FILE_DIALOG
   MessageInterface::ShowMessage("TextEphemFileDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   //------------------------------------------------------
   // Ephemeris file parameters
   //------------------------------------------------------
   
   //----- Ephemeris filename
   wxStaticText *headerFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("Header File:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mEphemFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(200,-1), 0);
   
   mEphemFileBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,-1), 0);

   //----- Interval
   wxStaticText *intervalLabel =
      new wxStaticText(this, ID_TEXT, wxT("Interval:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mIntervalTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("60"),
                     wxDefaultPosition, wxSize(80,-1), 0);
   
   wxStaticText *intervalUnitLabel =
      new wxStaticText(this, ID_TEXT, wxT("Seconds"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   //----- Output CoordinateSystem
   wxStaticText *coordSysLabel =
      new wxStaticText(this, ID_TEXT, wxT("Coordinate System:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));

   //----- Output epoch format
   wxStaticText *epochFormatLabel =
      new wxStaticText(this, ID_TEXT, wxT("Epoch Format:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mEpochFormatComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(150,-1), //0,
                     emptyList, wxCB_DROPDOWN | wxCB_READONLY);
   
   wxFlexGridSizer *fileSizer = new wxFlexGridSizer(3, 0, 0);
   fileSizer->Add(headerFileLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mEphemFileTextCtrl, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(mEphemFileBrowseButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(intervalLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mIntervalTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   fileSizer->Add(intervalUnitLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mCoordSysComboBox, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   
   fileSizer->Add(epochFormatLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(mEpochFormatComboBox, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);

   fileSizer->Add(20, 5, 0, wxALIGN_RIGHT|wxALL, bsize);
   fileSizer->Add(20, 5, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   fileSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);

   
   //----- Spacecraft selection
   wxStaticText *scLabel =
      new wxStaticText(this, ID_TEXT, wxT("Spacecraft"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *selScLabel =
      new wxStaticText(this, ID_TEXT, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mSpacecraftListBox = theGuiManager->
      GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150,65));
   
   mSelectedScListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150,65), //0,
                    emptyList, wxLB_SINGLE);
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   
   mAddScButton = new wxButton(this, ID_BUTTON, wxT("-->"),
                               wxDefaultPosition, wxSize(buttonWidth,-1), 0);
   mRemoveScButton = new wxButton(this, ID_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(buttonWidth,-1), 0);
   mClearScButton = new wxButton(this, ID_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(buttonWidth,-1), 0);

   
   wxBoxSizer *arrowsSizer = new wxBoxSizer(wxVERTICAL);
   arrowsSizer->Add(mAddScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   arrowsSizer->Add(mRemoveScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   arrowsSizer->Add(mClearScButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxFlexGridSizer *scGridSizer = new wxFlexGridSizer(3, 0, 0);
   scGridSizer->Add(scLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(20, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(selScLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   
   scGridSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(arrowsSizer, wxALIGN_CENTER|wxALL, bsize);
   scGridSizer->Add(mSelectedScListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxBoxSizer *scSizer = new wxBoxSizer(wxVERTICAL);
   scSizer->Add(scGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // Run
   //------------------------------------------------------
   
   wxBoxSizer *runSizer = new wxBoxSizer(wxVERTICAL);
   runSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   pageBoxSizer->Add(fileSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(scSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(runSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);

   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTER|wxALL|wxGROW, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::LoadData()
{
   // Get default path for ephemeris file
   FileManager *fm = FileManager::Instance();
   mEphemDirectory = fm->GetAbsPathname(FileManager::OUTPUT_PATH).c_str();
   
   // Append first spacecraft to selected listbox
   int scCount = mSpacecraftListBox->GetCount();
   if (scCount > 0)
   {
      wxString scName = mSpacecraftListBox->GetStringSelection();
      mSelectedScListBox->Append(scName);
      mSelectedScListBox->SetStringSelection(scName);
      
      // set default ephemeris  file using spacecraft name
      wxString fname = scName + "_Ephem.txt";
      mEphemFileTextCtrl->SetValue(mEphemDirectory + fname);
      
      theOkButton->Enable();
   }
   else
   {
      mEphemFileTextCtrl->SetValue(mEphemDirectory +
                                   wxString("/TextEphemHeader.txt"));
      
      theOkButton->Disable();
   }

   // Show last position
   mEphemFileTextCtrl->SetInsertionPointEnd();
   
   mCoordSysComboBox->SetSelection(0);
   
   // Load the epoch formats
   StringArray reps = TimeSystemConverter::Instance()->GetValidTimeRepresentations();
   for (unsigned int i = 0; i < reps.size(); i++)
      mEpochFormatComboBox->Append(reps[i].c_str());
   
   mEpochFormatComboBox->SetSelection(0);
   
   // Change label
   theOkButton->SetLabel("Run and Create Ephemeris File");
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::SaveData()
{
   if (CreateTextEphem())
   {
      mCreateEphemFile = true;
   }
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void TextEphemFileDialog::ResetData()
{
   canClose = true;
   mCreateEphemFile = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TextEphemFileDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mEphemFileBrowseButton)
   {
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
      if (dialog.ShowModal() == wxID_OK)
      {
         mEphemDirectory = dialog.GetPath();
         mEphemFileTextCtrl->SetValue(mEphemDirectory);
         mEphemFileTextCtrl->SetInsertionPointEnd();
         #if DEBUG_EPHEM_FILE_DIALOG
         MessageInterface::ShowMessage
            ("TextEphemFileDialog::OnButtonClick() mEphemDirectory=%s\n",
             mEphemDirectory.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mAddScButton)
   {
      wxString s = mSpacecraftListBox->GetStringSelection();
      int found = mSelectedScListBox->FindString(s);
      
      if (found == wxNOT_FOUND)
      {
         mSelectedScListBox->Append(s);
         mSelectedScListBox->SetStringSelection(s);
      
         // select next available item
         mSpacecraftListBox->
            SetSelection(mSpacecraftListBox->GetSelection()+1);

         theOkButton->Enable();
      }
   }
   else if (event.GetEventObject() == mRemoveScButton)
   {
      int sel = mSelectedScListBox->GetSelection();
      mSelectedScListBox->Delete(sel);
   
      if (sel-1 < 0)
         mSelectedScListBox->SetSelection(0);
      else
         mSelectedScListBox->SetSelection(sel-1);

      theOkButton->Enable();
      if (mSelectedScListBox->GetCount() == 0)
         theOkButton->Disable();

   }
   else if (event.GetEventObject() == mClearScButton)
   {
      mSelectedScListBox->Clear();
      theOkButton->Disable();
   }
}


//------------------------------------------------------------------------------
// bool CreateTextEphem()
//------------------------------------------------------------------------------
bool TextEphemFileDialog::CreateTextEphem()
{
   std::string ephemFileName = mEphemFileTextCtrl->GetValue().WX_TO_STD_STRING;
   
   TextEphemFile *ephemFile = (TextEphemFile*)(theGuiInterpreter->
      //CreateSubscriber("TextEphemFile", "TextEphemFile", ephemFileName, false));
      CreateSubscriber("TextEphemFile", "TextEphemFile", false));
   
   // get first spacecraft from the list
   std::string scName = mSelectedScListBox->GetString(0).WX_TO_STD_STRING;
   std::string epochFormat = mEpochFormatComboBox->GetValue().WX_TO_STD_STRING;
   std::string coordSys = mCoordSysComboBox->GetValue().WX_TO_STD_STRING;

   // check interval
   Real interval;
   if (!mIntervalTextCtrl->GetValue().ToDouble(&interval))
   {
      wxMessageBox("Invalid Interval Entered.");
      return false;
   }
   
   // Add parameters
   std::string time = scName + "." + epochFormat;
   std::string xpos = scName + "." + coordSys + ".X";
   std::string ypos = scName + "." + coordSys + ".Y";
   std::string zpos = scName + "." + coordSys + ".Z";
   std::string xvel = scName + "." + coordSys + ".VX";
   std::string yvel = scName + "." + coordSys + ".VY";
   std::string zvel = scName + "." + coordSys + ".VZ";
   
   // Create parameters
   try
   {
      theGuiInterpreter->CreateParameter(epochFormat, time, scName);
      theGuiInterpreter->CreateParameter("X", xpos, scName, coordSys);
      theGuiInterpreter->CreateParameter("Y", ypos, scName, coordSys);
      theGuiInterpreter->CreateParameter("Z", zpos, scName, coordSys);
      theGuiInterpreter->CreateParameter("VX", xvel, scName, coordSys);
      theGuiInterpreter->CreateParameter("VY", yvel, scName, coordSys);
      theGuiInterpreter->CreateParameter("VZ", zvel, scName, coordSys);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("TextEphemFileDialog:CreateTextEphem() error occurred!\n%s\n",
          e.GetFullMessage().c_str());

      return false;
   }

   // Set parameters to ephemeris file
   try
   {
      ephemFile->SetStringParameter("filename", ephemFileName);
      ephemFile->SetStringParameter("Add", time);
      ephemFile->SetStringParameter("Add", xpos);
      ephemFile->SetStringParameter("Add", ypos);
      ephemFile->SetStringParameter("Add", zpos);
      ephemFile->SetStringParameter("Add", xvel);
      ephemFile->SetStringParameter("Add", yvel);
      ephemFile->SetStringParameter("Add", zvel);
      
      ephemFile->SetStringParameter("EpochFormat", epochFormat);
      ephemFile->SetStringParameter("CoordinateSystem", coordSys);
      ephemFile->SetRealParameter("Interval", interval);
      
      #if DEBUG_EPHEM_FILE_DIALOG
      StringArray params = ephemFile->GetRefObjectNameArray(Gmat::PARAMETER);
      MessageInterface::ShowMessage
         ("TextEphemFileDialog:CreateTextEphem() params=\n");
      for (UnsignedInt i=0; i<params.size(); i++)
         MessageInterface::ShowMessage("%s\n", params[i].c_str());
      #endif
      
      // Now create element wrappers through ValidateSubscriber() (loj: 2008.03.19)
      theGuiInterpreter->ValidateSubscriber(ephemFile);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot generate text ephemeris file. "
          "Error occurred!\n%s\n", e.GetFullMessage().c_str());
      
      return false;
   }
   
   return true;
}

