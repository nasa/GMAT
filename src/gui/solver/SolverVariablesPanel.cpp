//$Id$
//------------------------------------------------------------------------------
//                           SolverVariablesPanel
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
// Author: Waka Waktola
// Created: 2004/21/01
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Variables window.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "SolverVariablesPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SolverVariablesPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON, SolverVariablesPanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, SolverVariablesPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, SolverVariablesPanel::OnSolverSelection)
    EVT_GRID_CELL_CHANGE(SolverVariablesPanel::OnCellValueChanged)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverVariablesPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
SolverVariablesPanel::SolverVariablesPanel(wxWindow *parent)
    : GmatPanel(parent)
{
    Create();
    Show();
}

SolverVariablesPanel::~SolverVariablesPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void SolverVariablesPanel::Create()
{
    Initialize();
    Setup(this);
}

void SolverVariablesPanel::LoadData()
{
    // explicitly disable apply button
    // it is turned on in each of the panels
    EnableUpdate(false);
}

void SolverVariablesPanel::SaveData()
{   
    // explicitly disable apply button
    // it is turned on in each of the panels
    EnableUpdate(false);
}


void SolverVariablesPanel::Initialize()
{
    numOfVars = 20;    // TBD
    nextRow = 0;
}

void SolverVariablesPanel::Setup( wxWindow *parent)
{  
    // wxGrid
    varsGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(600,160), 
         wxWANTS_CHARS );
    varsGrid->CreateGrid( numOfVars, 3, wxGrid::wxGridSelectCells );
    varsGrid->SetColSize(0, 150);
    varsGrid->SetColSize(1, 585);
    varsGrid->SetColSize(2, 585);
    varsGrid->SetColLabelValue(0, _T("Solver"));
    varsGrid->SetColLabelValue(1, _T("Property"));
    varsGrid->SetColLabelValue(2, _T("Description"));
    varsGrid->SetRowLabelSize(0);
       
    // wxStaticText
    descStaticText = new wxStaticText( parent, ID_TEXT, wxT("Description"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    solverStaticText = new wxStaticText( parent, ID_TEXT, wxT("Solver"),
         wxDefaultPosition, wxDefaultSize, 0 );
    varStaticText = new wxStaticText( parent, ID_TEXT, wxT("Variable"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    pertStaticText = new wxStaticText( parent, ID_TEXT, wxT("Perturbation"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    maxStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Step"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    lowerStaticText = new wxStaticText( parent, ID_TEXT, wxT("Lower Bound"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    upperStaticText = new wxStaticText( parent, ID_TEXT, wxT("Upper Bound"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    descTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(400,-1), 0 );
    varTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(400,-1), 0 );
    pertTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
    maxTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
    lowerTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
    upperTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
         wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wxString
    wxString strArray1[] = 
    {
        wxT("")
    };
    
    // wxComboBox
    solverComboBox =
       new wxComboBox( parent, ID_COMBO, strArray1[0], wxDefaultPosition,
                       wxSize(200,-1), 0, strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    
    // wxButton
    editButton = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    updateButton = new wxButton( parent, ID_BUTTON, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
   
    // wx*Sizers
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("Variable List") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
    wxStaticBox *item5 = new wxStaticBox( parent, -1, wxT("Variable Setup") );
    wxStaticBoxSizer *item4 = new wxStaticBoxSizer( item5, wxVERTICAL );
    wxFlexGridSizer *item6 = new wxFlexGridSizer( 3, 0, 0 );
    item6->AddGrowableCol( 1 );
    wxStaticBox *item15 = new wxStaticBox( parent, -1, wxT("Settings") );
    wxStaticBoxSizer *item14 = new wxStaticBoxSizer( item15, wxHORIZONTAL );
    wxGridSizer *item16 = new wxGridSizer( 4, 0, 0 );
  
    // Add to wx*Sizers
    item1->Add( varsGrid, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item6->Add( descStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( descTextCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( solverComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item6->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( varTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item6->Add( editButton, 0, wxALIGN_CENTER|wxALL, 5 );    
    
    item16->Add( pertStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( maxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( lowerStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( upperStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( pertTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( maxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( lowerTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    item16->Add( upperTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    item14->Add( item16, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );
    item4->Add( updateButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);

}

void SolverVariablesPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == descTextCtrl )  
    {       
        EnableUpdate(true);
    }
    else if ( event.GetEventObject() == varTextCtrl )
    {      
        EnableUpdate(true);
    }
    else if ( event.GetEventObject() == pertTextCtrl )  
    {
        EnableUpdate(true);       
    }
    else if ( event.GetEventObject() == maxTextCtrl )
    {
        EnableUpdate(true);  
    } 
    else if ( event.GetEventObject() == lowerTextCtrl )
    {
        EnableUpdate(true);  
    }      
    else if ( event.GetEventObject() == upperTextCtrl )       
    {    
        EnableUpdate(true); 
    }
    else
        event.Skip();
}

void SolverVariablesPanel::OnSolverSelection(wxCommandEvent &event)
{
    EnableUpdate(true);
}

void SolverVariablesPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == editButton )  
    {       
        // Bring up the VariableCreatePanel
        EnableUpdate(true);
    }
    else if ( event.GetEventObject() == updateButton )
    {      
        solverString = solverComboBox->GetStringSelection();
        propertyString = varTextCtrl->GetValue();
        descriptionString = descTextCtrl->GetValue();
        
        varsGrid->SetCellValue(nextRow, SOL_COL, solverString);
        varsGrid->SetCellValue(nextRow, PRO_COL, propertyString);
        varsGrid->SetCellValue(nextRow, DES_COL, descriptionString);
        
        nextRow++;
       
        EnableUpdate(true);
    }
    else
        event.Skip();
}

void SolverVariablesPanel::OnCellValueChanged(wxGridEvent &event)
{ 
    EnableUpdate(true);
}
