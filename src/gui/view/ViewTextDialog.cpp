//$Id$
//------------------------------------------------------------------------------
//                         ViewTextDialog
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
 * Implements the dialog used to show text.
 */
//------------------------------------------------------------------------------
#include "ViewTextDialog.hpp"
#include "FileManager.hpp"
#include "GmatAppData.hpp"         // SetIcon(), etc.
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(ViewTextDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON, ViewTextDialog::OnButtonClick)
   EVT_TEXT_ENTER(-1, ViewTextDialog::OnEnterPressed)
END_EVENT_TABLE()

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
//  ViewTextDialog(wxWindow *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param parent		 Parent wxWindow for the dialog.
 * @param title       Title for the dialog.
 * @param isEditable	 Is text in dialog editable or read-only
 * @param pos			 window position
 * @param size			 window size
 * @param style	    window style
 */
//------------------------------------------------------------------------------
ViewTextDialog::ViewTextDialog(wxWindow *parent, const wxString& title,
                               bool isEditable, const wxPoint &pos,
                               const wxSize &size, const wxFont &font, long style)
   : wxDialog(parent, -1, title, pos, size, style, title)
{
   isTextEditable = isEditable;
   
   // create bottom buttons
   theOkButton =
      new wxButton(this, ID_BUTTON, "OK", wxDefaultPosition, wxDefaultSize, 0);
   
   if (isTextEditable)
      theCancelButton =
         new wxButton(this, ID_BUTTON, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
   
   // adds the buttons to button sizer
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, 2);
   if (isTextEditable)
      theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, 2);
   
   theBottomSizer = new wxBoxSizer(wxVERTICAL);
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, 2);
   
   // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
   // increase text size(loj: 2009.02.05)
   if (isTextEditable)
      theText = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, size,
                               wxTE_PROCESS_ENTER);
   else
      theText = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, size,
                               wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);
   
   if (!isTextEditable)
      theText->SetMaxLength(320000);
   //theText->SetFont( GmatAppData::Instance()->GetFont());
   theText->SetFont(font);
   
   // add items to middle sizer
   theMiddleSizer = new wxBoxSizer(wxVERTICAL);
   if (isTextEditable)
      theMiddleSizer->Add(theText, 0, wxGROW|wxALL, 2);
   else
      theMiddleSizer->Add(theText, 1, wxGROW|wxALL, 2);
   
   // add items to dialog sizer
   wxBoxSizer *dialogSizer = new wxBoxSizer(wxVERTICAL);
   if (isTextEditable)
   {
      dialogSizer->Add(theMiddleSizer, 0, wxGROW | wxALL, 1);
      dialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   }
   else
   {
      dialogSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
      dialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   }
   
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(dialogSizer);
   dialogSizer->Fit(this);
   dialogSizer->SetSizeHints(this);
   
   // Set GMAT main icon
   GmatAppData::Instance()->SetIcon(this, "ViewTextDialog");
   
   if (!isTextEditable)
      CenterOnScreen(wxBOTH);
}


//------------------------------------------------------------------------------
// void AppendText(const wxString& text)
//------------------------------------------------------------------------------
/**
 * Append text to view dialog
 *
 * param text	text to append
 */
void ViewTextDialog::AppendText(const wxString& text)
{
   if (isTextEditable)
   {
      oldText = text;
      theText->SetValue(text);
      theText->SetInsertionPointEnd();
      // Why this doesn't set selection to all text?
      theText->SetSelection(-1, -1);
   }
   else
      theText->AppendText(text);
   
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles button click event to close the dialog
 *
 * @param event		command event originated by the control
 */
//------------------------------------------------------------------------------
void ViewTextDialog::OnButtonClick(wxCommandEvent &event)
{
   hasTextChanged = false;
   if (event.GetEventObject() == theOkButton)
   {
      if (theText->GetValue() != oldText)
         hasTextChanged = true;
   }
   
   Close(true);
}

//----------------------------
// protected methods
//----------------------------

//------------------------------------------------------------------------------
// void OnEnterPressed(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles enter key pressed event to close the dialog
 *
 * @param event		command event originated by the control
 */
//------------------------------------------------------------------------------
void ViewTextDialog::OnEnterPressed(wxCommandEvent &event)
{
   hasTextChanged = false;
   if (theText->GetValue() != oldText)
      hasTextChanged = true;
   
   Close(true);
}
