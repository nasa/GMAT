//$Id$
//------------------------------------------------------------------------------
//                              FindReplaceDialog
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
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Implements FindReplaceDialog class.
 */
//------------------------------------------------------------------------------

#include "FindReplaceDialog.hpp"
#include "ScriptEditor.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FIND_REPLACE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE (FindReplaceDialog, wxDialog)
   EVT_BUTTON (ID_BUTTON, FindReplaceDialog::OnButtonClick)
   EVT_TEXT_ENTER (ID_COMBOBOX, FindReplaceDialog::OnComboBoxEnter)
   EVT_KEY_DOWN (FindReplaceDialog::OnKeyDown)
END_EVENT_TABLE()

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// FindReplaceDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//    GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//    const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs FindReplaceDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 * @param  pos	   window position
 * @param  size    window size
 * @param  style   window style
 *
 */
//------------------------------------------------------------------------------
FindReplaceDialog::FindReplaceDialog(wxWindow *parent, wxWindowID id,
                                     const wxString& title, const wxPoint& pos,
                                     const wxSize& size, long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   mEditor = NULL;

   Create();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and initializes the controls for the FindReplaceDialog
 */
void FindReplaceDialog::Create()
{
   //---------- find
   wxStaticText *findText = new wxStaticText(this, -1, "Find What");
   // TGG: Fix for GMT-3198 Can't use Find/Replace dialog after finding very long string
   // Changed wxDefault Size to wxSize.  Note that none of the sizer options would stop
   // the combobox from growing if the mFindArray/mReplaceArray contained long strings
   mFindComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(200,-1),
                     mFindArray, 0);
   mFindNextButton =
      new wxButton(this, ID_BUTTON, wxT("Find Next"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mFindNextButton->SetDefault();
   mFindPrevButton =
      new wxButton(this, ID_BUTTON, wxT("Find Previous"),
                   wxDefaultPosition, wxDefaultSize, 0);

   //---------- replace
   wxStaticText *replaceText = new wxStaticText(this, -1, "Replace With");
   // TGG: Fix for GMT-3198 Can't use Find/Replace dialog after finding very long string
   // Changed wxDefault Size to wxSize.  Note that none of the sizer options would stop
   // the combobox from growing if the mFindArray/mReplaceArray contained long strings
   mReplaceComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(200,-1), 
					 mReplaceArray, 0);
   mReplaceButton =
      new wxButton(this, ID_BUTTON, wxT("Replace"),
                   wxDefaultPosition, wxDefaultSize, 0);
   mReplaceAllButton =
      new wxButton(this, ID_BUTTON, wxT("Replace All"),
                   wxDefaultPosition, wxDefaultSize, 0);

   //---------- close
   mCloseButton =
      new wxButton(this, ID_BUTTON, wxT("Close"),
                   wxDefaultPosition, wxDefaultSize, 0);

   int bsize = 3;

   //---------- sizer
   wxFlexGridSizer *findSizer = new wxFlexGridSizer(4, 0, 0);
   findSizer->Add(findText, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mFindComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   findSizer->Add(mFindNextButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mFindPrevButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);

   findSizer->Add(replaceText, 0, wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL, bsize);
   findSizer->Add(mReplaceComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   findSizer->Add(mReplaceButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   findSizer->Add(mReplaceAllButton, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);

   // Add to page sizer and set sizer to this dialog
   wxStaticBoxSizer *findStaticSizer = new wxStaticBoxSizer(wxVERTICAL, this);
   findStaticSizer->Add(findSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(findStaticSizer, 0, wxALIGN_CENTER|wxALL, 4);
   pageSizer->Add(mCloseButton, 0, wxALIGN_CENTER|wxALL, 4);

   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
}

//----------------------------
// protected methods
//----------------------------

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles button click event
 *
 * @param event		command event originated by the button
 */
//------------------------------------------------------------------------------
void FindReplaceDialog::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetEventObject() == mFindNextButton ||
       event.GetEventObject() == mFindPrevButton)
   {
      mFindText = mFindComboBox->GetValue();
      if (mFindText != "")
      {
         #ifdef DEBUG_FIND_REPLACE
         MessageInterface::ShowMessage
            ("FindReplaceDialog::OnButtonClick() FindButton clicked, "
             "mFindText='%s', mEditor=<%p>\n", mFindText.c_str(), mEditor);
         #endif

         if (mFindComboBox->FindString(mFindText, true) == wxNOT_FOUND)
            mFindComboBox->Append(mFindText);

         if (mEditor)
         {
            mEditor->SetFindText(mFindText);
            wxCommandEvent tempEvent;
            if (event.GetEventObject() == mFindNextButton)
               mEditor->OnFindNext(tempEvent);
            else
               mEditor->OnFindPrev(tempEvent);
         }
      }
   }
   else if (event.GetEventObject() == mReplaceButton ||
            event.GetEventObject() == mReplaceAllButton)
   {
      mFindText = mFindComboBox->GetValue();
      mReplaceText = mReplaceComboBox->GetValue();

      if (mFindText != "")
      {
         #ifdef DEBUG_FIND_REPLACE
         MessageInterface::ShowMessage
            ("FindReplaceDialog::OnButtonClick() ReplaceButtton clicked, "
             "mFindText='%s', mReplaceText='%s', mEditor=<%p>\n", mFindText.c_str(),
             mReplaceText.c_str(), mEditor);
         #endif

         if (mFindComboBox->FindString(mFindText, true) == wxNOT_FOUND)
            mFindComboBox->Append(mFindText);

         if (mReplaceComboBox->FindString(mReplaceText, true) == wxNOT_FOUND)
            mReplaceComboBox->Append(mReplaceText);

         if (mEditor)
         {
            mEditor->SetFindText(mFindText);
            mEditor->SetReplaceText(mReplaceText);
            wxCommandEvent tempEvent;
            if (event.GetEventObject() == mReplaceButton)
               mEditor->OnReplaceNext(tempEvent);
            else
               mEditor->OnReplaceAll(tempEvent);
         }
      }
   }
   else if (event.GetEventObject() == mCloseButton)
   {
      Hide();
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxEnter(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles enter key in combo box
 *
 * @param event		command event originated by the control
 */
void FindReplaceDialog::OnComboBoxEnter(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFindComboBox ||
       event.GetEventObject() == mReplaceComboBox)
   {
      #ifdef DEBUG_FIND_REPLACE
      MessageInterface::ShowMessage
         ("FindReplaceDialog::OnComboBoxEnter() Find entered\n");
      #endif

      wxCommandEvent tempEvent;
      tempEvent.SetEventObject(mFindNextButton);
      OnButtonClick(tempEvent);
   }
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles key press event
 *
 * @param event		key event originated by the control
 */
void FindReplaceDialog::OnKeyDown(wxKeyEvent &event)
{
}


