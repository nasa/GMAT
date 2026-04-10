//$Id$
//------------------------------------------------------------------------------
//                              MatlabFunctionSetupPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2005/02/11
//
/**
 * Implements MatlabFunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "MatlabFunctionSetupPanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MatlabFunctionSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, MatlabFunctionSetupPanel::OnTextUpdate)
   EVT_BUTTON(ID_BUTTON, MatlabFunctionSetupPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MatlabFunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
MatlabFunctionSetupPanel::MatlabFunctionSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   theMatlabFunction = (Function *)
      theGuiInterpreter->GetConfiguredObject(name.c_str());

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::Create()
{
   int bsize = 2; // border size
   
   wxStaticBox *topStaticBox = new wxStaticBox(this, -1, wxT(""));

    // create sizers
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mTopSizer = new wxStaticBoxSizer(topStaticBox, wxHORIZONTAL);
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);

   //wxStaticText
   wxStaticText *pathStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Path: "), wxDefaultPosition, 
         wxDefaultSize, 0);
   // wxTextCtrl
   mPathTextCtrl = 
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
         wxSize(250,-1), 0);
   // create buttons
   mBrowseButton =
      new wxButton(this, ID_BUTTON, "Browse", wxDefaultPosition, wxDefaultSize, 0);

   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   fileSizer->Add(pathStaticText, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mPathTextCtrl, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mBrowseButton, 0, wxALIGN_CENTER  | wxALL, bsize);
   mTopSizer->Add(fileSizer, 0, wxALIGN_CENTER, bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mTopSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theMatlabFunction;

   int pathId = theMatlabFunction->GetParameterID("FunctionPath");
   std::string path = theMatlabFunction->GetStringParameter(pathId);
   mPathTextCtrl->SetValue(path.c_str());
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::SaveData()
{
   int pathId = theMatlabFunction->GetParameterID("FunctionPath");
   theMatlabFunction->SetStringParameter(pathId,
         mPathTextCtrl->GetValue().c_str());
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MatlabFunctionSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//---------------------------------
// private methods
//---------------------------------
void MatlabFunctionSetupPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBrowseButton)
   {
      wxDirDialog dialog(this);

      if (dialog.ShowModal() == wxID_OK)
      {
         wxString filename;
         filename = dialog.GetPath().c_str();
         mPathTextCtrl->SetValue(filename);
      }
   }
   else
   {
      //Error - unknown object
   }
}



