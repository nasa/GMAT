//$Id$
//------------------------------------------------------------------------------
//                           TargetPanel
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
// Author: Waka Waktola
// Created: 2003/12/16
//
/**
 * This class contains the Target setup window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "TargetPanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TargetPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBO, TargetPanel::OnComboBoxChange)
   EVT_BUTTON(ID_APPLYBUTTON, TargetPanel::OnApplyButtonPress)
   EVT_CHECKBOX(ID_PROGRESS_CHECKBOX, TargetPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// TargetPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
TargetPanel::TargetPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = cmd;
   
   if (theCommand != NULL)
   {
      Create();
      Show();
   }
   else
   {
      // show error message
   }
}


//------------------------------------------------------------------------------
// ~TargetPanel()
//------------------------------------------------------------------------------
TargetPanel::~TargetPanel()
{
   theGuiManager->UnregisterComboBox("BoundarySolver", mSolverComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TargetPanel::Create()
{    
   Integer bsize = 5;

   //-------------------------------------------------------
   // Solver ComboBox
   //-------------------------------------------------------
   wxStaticText *solverNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   wxStaticText *solverModeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Mode"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   wxStaticText *exitModeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Exit Mode"), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mSolverComboBox =
      theGuiManager->GetBoundarySolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   StringArray options = theCommand->GetStringArrayParameter("SolveModeOptions");
   wxArrayString theOptions;
   
   for (StringArray::iterator i = options.begin(); i != options.end(); ++i)
      theOptions.Add(i->c_str());
   
   mSolverModeComboBox =
      new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(180,-1),
                     theOptions, wxCB_READONLY);

   options = theCommand->GetStringArrayParameter("ExitModeOptions");
   theOptions.Clear();
   
   for (StringArray::iterator i = options.begin(); i != options.end(); ++i)
      theOptions.Add(i->c_str());
   
   mExitModeComboBox = 
      new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(180,-1),
            theOptions, wxCB_READONLY);;
   
   //----------------------------------------------------------------------
   // ShowProgressWindow flag
   //----------------------------------------------------------------------
   mProgressWindowCheckBox =
      new wxCheckBox(this, ID_PROGRESS_CHECKBOX, gmatwxT(GUI_ACCEL_KEY"Show Progress Window"),
      wxDefaultPosition, wxDefaultSize);
   mProgressWindowCheckBox->SetToolTip(_T("Show Progress Window during targeting"));

   mApplyCorrectionsButton = new wxButton(this, ID_APPLYBUTTON, 
         wxT("Apply Corrections"));
   
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(2);
   
   pageSizer->Add(solverNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);

   pageSizer->Add(solverModeStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mSolverModeComboBox, 0, wxALIGN_CENTER|wxALL, bsize);

   pageSizer->Add(exitModeStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(mExitModeComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
//   pageSizer->Add(NULL, 0, wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(20,20);
   pageSizer->Add(mProgressWindowCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(mApplyCorrectionsButton, 0, wxALIGN_CENTER|wxALL, bsize);

   theMiddleSizer->Add(pageSizer, 0, wxGROW, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TargetPanel::LoadData()
{
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      std::string solverName =
         theCommand->GetStringParameter(theCommand->GetParameterID("Targeter"));
      
      mSolverComboBox->SetValue(solverName.c_str());
       
      std::string solverMode =
               theCommand->GetStringParameter("SolveMode");
      mSolverModeComboBox->SetValue(solverMode.c_str());

      std::string exitMode =
               theCommand->GetStringParameter("ExitMode");
      mExitModeComboBox->SetValue(exitMode.c_str());

      mProgressWindowCheckBox->SetValue((wxVariant(theCommand->GetBooleanParameter("ShowProgressWindow"))));
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TargetPanel::SaveData()
{
   try
   {
      std::string solverName = mSolverComboBox->GetValue().WX_TO_STD_STRING;
      std::string solverMode = mSolverModeComboBox->GetValue().WX_TO_STD_STRING;
      std::string exitMode   = mExitModeComboBox->GetValue().WX_TO_STD_STRING;

      theCommand->SetStringParameter(theCommand->GetParameterID("Targeter"),
                                     solverName);
      theCommand->SetStringParameter(theCommand->GetParameterID("SolveMode"), 
            solverMode);
      theCommand->SetStringParameter(theCommand->GetParameterID("ExitMode"), 
            exitMode);

      if (mProgressWindowCheckBox->IsChecked())
         theCommand->SetBooleanParameter("ShowProgressWindow", true);
      else
         theCommand->SetBooleanParameter("ShowProgressWindow", false);

      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TargetPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnApplyButtonPress(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Method that updates variables with solution values.
 */
//------------------------------------------------------------------------------
void TargetPanel::OnApplyButtonPress(wxCommandEvent& event)
{
   // ensure all vary panels are closed
   if (GmatAppData::Instance()->GetMainFrame()->IsChildOpen(GmatTree::VARY))
   {
		MessageInterface::PopupMessage(Gmat::ERROR_, "You must close all Vary command panels before Apply Corrections");
		return;
   }
   theCommand->TakeAction("ApplyCorrections");
}
