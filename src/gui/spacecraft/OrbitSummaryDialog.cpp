//
//------------------------------------------------------------------------------
//                         OrbitSummaryDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Evelyn Hull
// Created: 2011/10/08
//
/**
 * Implements the dialog used to show a summary of the calculated orbit 
 * parameters.
 */
//------------------------------------------------------------------------------

#include "OrbitSummaryDialog.hpp"

BEGIN_EVENT_TABLE(OrbitSummaryDialog, GmatDialog)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// OrbitSummaryDialog()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the OrbitSummaryDialog class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
OrbitSummaryDialog::OrbitSummaryDialog(wxWindow *parent, wxString &summary)
   : GmatDialog(parent, -1, wxString(_T("Orbit Summary")))
{
   orbitSummaryString = wxString(summary);

   theCancelButton->Show(false);
   theHelpButton->Show(false);

   canClose = true;

   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// ~OrbitSummaryDialog()
//------------------------------------------------------------------------------
/**
 * This method destroys the object of the OrbitSummaryDialog class
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
OrbitSummaryDialog::~OrbitSummaryDialog()
{
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the widgets for the panel.
 */
//------------------------------------------------------------------------------
void OrbitSummaryDialog::Create()
{
   orbitSummary = new wxTextCtrl(this, -1, orbitSummaryString, wxPoint(0,0), wxSize(300,200), 
                    wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);

   theMiddleSizer->Add(orbitSummary, 1, wxGROW|wxALL, 3);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the attitude object into the widgets for the panel.
 */
//------------------------------------------------------------------------------
void OrbitSummaryDialog::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the widgets to the object, if applicable.
 */
//------------------------------------------------------------------------------
void OrbitSummaryDialog::SaveData()
{
}

//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * Resets the data.
 */
//------------------------------------------------------------------------------
void OrbitSummaryDialog::ResetData()
{
}
