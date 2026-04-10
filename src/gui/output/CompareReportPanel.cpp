//$Id$
//------------------------------------------------------------------------------
//                              CompareReportPanel
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
// Created: 2004/02/02
//
/**
 * Implements CompareReportPanel class.
 */
//------------------------------------------------------------------------------

#include "CompareReportPanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareReportPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_CLOSE, CompareReportPanel::OnClose)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// CompareReportPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs CompareReportPanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
CompareReportPanel::CompareReportPanel(wxWindow *parent, const wxString &name)
    : wxPanel(parent)
{
   mName = name;
   
   int borderSize = 3;
   
   // create the text ctrl
   mTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                              wxDefaultPosition, wxDefaultSize,
                              wxTE_DONTWRAP | wxTE_READONLY | wxTE_MULTILINE);
   
   // set font
   mTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
   
   // create bottom buttons
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   
   // adds the buttons to button sizer
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   GmatStaticBoxSizer *theBottomSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "");
//   wxStaticBoxSizer *theBottomSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   wxBoxSizer *thePanelSizer = new wxBoxSizer(wxVERTICAL);
   thePanelSizer->Add(mTextCtrl, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this);
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
}


//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void CompareReportPanel::OnClose(wxCommandEvent &event)
{
   GmatAppData::Instance()->GetMainFrame()->OverrideActiveChildDirty(false);
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}

