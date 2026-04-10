//$Id$
//------------------------------------------------------------------------------
//                              OrbitDesignerDialog
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
// Author: Evelyn Hull
// Created: 2011/07/18
//
/*
 * Implements OrbitDesignerDialog Class. This Dialog allows users to generate
 * Keplerian elements for specific orbit types.
 */
//------------------------------------------------------------------------------

#include "MessageInterface.hpp"
#include "OrbitDesignerDialog.hpp"
#include "GmatBaseException.hpp"
#include "GmatAppData.hpp"
#include <wx/config.h>
#include "OrbitDesignerTime.hpp"
#include "OrbitSummaryDialog.hpp"

BEGIN_EVENT_TABLE(OrbitDesignerDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON_HELP, GmatDialog::OnHelp)
   EVT_BUTTON(ID_BUTTON_SUMMARY, OrbitDesignerDialog::OnSummary)
   EVT_COMBOBOX(ID_COMBO, OrbitDesignerDialog::OnComboBoxChange)
   EVT_CHECKBOX(ID_CB_P1, OrbitDesignerDialog::OnP1CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P2, OrbitDesignerDialog::OnP2CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P3, OrbitDesignerDialog::OnP3CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P4, OrbitDesignerDialog::OnP4CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P5, OrbitDesignerDialog::OnP5CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P6, OrbitDesignerDialog::OnP6CheckBoxChange)
   EVT_CHECKBOX(ID_CB_P7, OrbitDesignerDialog::OnP7CheckBoxChange)
   EVT_CHECKBOX(ID_CB_TP1, OrbitDesignerDialog::OnTP1CheckBoxChange)
   EVT_CHECKBOX(ID_CB_TP2, OrbitDesignerDialog::OnTP2CheckBoxChange)
   EVT_CHECKBOX(ID_CB_TP3, OrbitDesignerDialog::OnTP3CheckBoxChange)
   EVT_BUTTON(ID_BUTTON_FINDORBIT, OrbitDesignerDialog::OnFindOrbit)
   EVT_TEXT(ID_TEXTCTRL_PARAMS, OrbitDesignerDialog::OnParamChange)
END_EVENT_TABLE()



//------------------------------------------------------------------------------
// OrbitDesignerDialog()
//--------------------------------------------------------------------------------
/**
 * This method creates an object of the OrbitDesignerDialog class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
OrbitDesignerDialog::OrbitDesignerDialog(wxWindow *parent, Spacecraft *spacecraft)
   : GmatDialog(parent, -1, wxString(_T("OrbitDesignerDialog")))
{
   orbitType = wxT("Sun Sync");

   epochType = wxT("UTCGregorian");

   theSpacecraft = spacecraft;

   isEpochChanged = false;
   updateOrbit = false;

   canClose = false;

   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// ~OrbitDesignerDialog()
//------------------------------------------------------------------------------
/**
 * This method destroys the object of the OrbitDesignerDialog class
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
OrbitDesignerDialog::~OrbitDesignerDialog()
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
void OrbitDesignerDialog::Create()
{
   int bsize = 3; // border size
   wxString orbitTypesArr[] = {"Sun Sync", "Repeat Sun Sync", "Repeat Ground Track", "Geostationary", 
                                   "Molniya", "Frozen"};
   wxString epochTypesArr[] = {"A1ModJulian", "TAIModJulian", "UTCModJulian", "TTModJulian", 
                                   "A1Gregorian", "TAIGregorian", "UTCGregorian", "TTGregorian"};

   //orbit selection combo box and label
   wxStaticText *orbitTypeStaticText = new wxStaticText( this, ID_TEXT, wxT("Orbit Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   orbitTypeComboBox =  new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxDefaultSize,
         6, orbitTypesArr, wxCB_DROPDOWN|wxCB_READONLY);

   //epoch combo box
   epochComboBox =  new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxDefaultSize,
         8, epochTypesArr, wxCB_DROPDOWN|wxCB_READONLY);

   //orbit calculate button
   findOrbitButton = 
      new wxButton(this, ID_BUTTON_FINDORBIT, wxT("Find Orbit"), wxDefaultPosition, wxDefaultSize, 0);

   //summary button
   showSummary = 
       new wxButton(this, ID_BUTTON_SUMMARY, wxT("Summary"), wxDefaultPosition, wxDefaultSize, 0);

   orbitTypeFlexGridSizer = new wxFlexGridSizer(2, 0, 10);
   orbitTypeFlexGridSizer->Add(orbitTypeStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   orbitTypeFlexGridSizer->Add(orbitTypeComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //orbit parameter check boxes   
   paramOneCheckBox = 
      new wxCheckBox(this, ID_CB_P1, wxT("SMA"), wxDefaultPosition, wxSize(150,-1),0);
   paramTwoCheckBox = 
          new wxCheckBox(this, ID_CB_P2, wxT("Altitude"), wxDefaultPosition, wxSize(150,-1),0);
   paramThreeCheckBox = 
          new wxCheckBox(this, ID_CB_P3, wxT("ECC"), wxDefaultPosition, wxSize(150,-1),0);
   paramFourCheckBox = 
          new wxCheckBox(this, ID_CB_P4, wxT("INC"), wxDefaultPosition, wxSize(150,-1),0);
   paramFiveCheckBox = 
          new wxCheckBox(this, ID_CB_P5, wxT("ROP"), wxDefaultPosition, wxSize(150,-1),0);
   paramSixCheckBox = 
          new wxCheckBox(this, ID_CB_P6, wxT("ROA"), wxDefaultPosition, wxSize(150,-1),0);
   paramSevenCheckBox = 
          new wxCheckBox(this, ID_CB_P7, wxT("P"), wxDefaultPosition, wxSize(150,-1),0);
   timeParamOneCheckBox = 
          new wxCheckBox(this, ID_CB_TP1, wxT("Epoch"), wxDefaultPosition, wxSize(150,-1),0);
   timeParamTwoCheckBox = 
          new wxCheckBox(this, ID_CB_TP2, wxT("RAAN"), wxDefaultPosition, wxSize(150,-1),0);
   timeParamThreeCheckBox = 
          new wxCheckBox(this, ID_CB_TP3, wxT("Initial Local Sidereal Time"), 
          wxDefaultPosition, wxSize(150,-1),0);
   
   //orbit parameter inputs
   param1TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param2TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param3TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param4TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param5TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param6TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   param7TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
       wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   time1TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
       wxDefaultPosition, wxSize(150,-1), 0);
   time2TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
       wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   time3TextCtrl = new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""),
       wxDefaultPosition, wxSize(150,-1), 0);

   //orbit parameter units
   units1StaticText = 
      new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   units2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   units3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   units4StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   units5StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   units6StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   units7StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   timeUnits1StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   timeUnits2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   timeUnits3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);

   orbitParamsFlexGridSizer = new wxFlexGridSizer(7,3,0,10);

   orbitParamsFlexGridSizer->Add(paramOneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units1StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramTwoCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units2StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramThreeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units3StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramFourCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param4TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units4StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramFiveCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param5TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units5StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramSixCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param6TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units6StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   orbitParamsFlexGridSizer->Add(paramSevenCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(param7TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   orbitParamsFlexGridSizer->Add(units7StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   //time inputs
   timeParamsFlexGridSizer = new wxFlexGridSizer(4,3,0,10);
   timeSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Optional" );
   
   timeParamsFlexGridSizer->Add(epochComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->AddSpacer(10);
   timeParamsFlexGridSizer->AddSpacer(10);

   timeParamsFlexGridSizer->Add(timeParamOneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(time1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(timeUnits1StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
      
   timeParamsFlexGridSizer->Add(timeParamTwoCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(time2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(timeUnits2StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
      
   timeParamsFlexGridSizer->Add(timeParamThreeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(time3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   timeParamsFlexGridSizer->Add(timeUnits3StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   timeSizer->Add(timeParamsFlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);


   //user input box
   GmatStaticBoxSizer *inputSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Inputs" );
   inputSizer->Add(orbitTypeFlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(orbitParamsFlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(timeSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(findOrbitButton, 0, wxALIGN_CENTRE|wxALL, bsize);

   
   //output parameter text boxes
   paramOut1StaticText = 
      new wxStaticText( this, ID_TEXT, wxT("SMA"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("Altitude"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("ECC"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut4StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("INC"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut5StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("ROP"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut6StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("ROA"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut7StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("P"), wxDefaultPosition, wxDefaultSize, 0);
   paramOut8StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   paramOut9StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   timeParamOut1StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0);
   timeParamOut2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("RAAN"), wxDefaultPosition, wxDefaultSize, 0);
   timeParamOut3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("Initial Local Time"), wxDefaultPosition, wxDefaultSize, 0);
   timeParamOut4StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);

   //output values boxes
   output1TextCtrl = 
      new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output1TextCtrl->Disable();
   output2TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output2TextCtrl->Disable();
   output3TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output3TextCtrl->Disable();
   output4TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output4TextCtrl->Disable();
   output5TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output5TextCtrl->Disable();
   output6TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output6TextCtrl->Disable();
   output7TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output7TextCtrl->Disable();
   output8TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output8TextCtrl->Disable();
   output9TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   output9TextCtrl->Disable();
   timeOut1TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   timeOut1TextCtrl->Disable();
   timeOut2TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   timeOut2TextCtrl->Disable();
   timeOut3TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   timeOut3TextCtrl->Disable();
   timeOut4TextCtrl = 
          new wxTextCtrl( this, ID_TEXTCTRL_PARAMS, wxT(""), wxDefaultPosition, wxSize(150,-1), 0);
   timeOut4TextCtrl->Disable();

   //output units text boxes
   unitsOut1StaticText = 
      new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut4StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut5StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut6StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut7StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("km"), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut8StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   unitsOut9StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   timeUnitsOut1StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   timeUnitsOut2StaticText = 
          new wxStaticText( this, ID_TEXT, wxT("deg"), wxDefaultPosition, wxDefaultSize, 0);
   timeUnitsOut3StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
   timeUnitsOut4StaticText = 
          new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, 0);

   outputsFlexGridSizer = new wxFlexGridSizer(13,3,0,10);

   outputsFlexGridSizer->Add(paramOut1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut1StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut2StaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output5TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output6TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output7TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut8StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output8TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut8StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(paramOut9StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(output9TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(unitsOut9StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(timeParamOut1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeOut1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeUnitsOut1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   outputsFlexGridSizer->Add(timeParamOut2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeOut2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeUnitsOut2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   outputsFlexGridSizer->Add(timeParamOut3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeOut3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeUnitsOut3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   outputsFlexGridSizer->Add(timeParamOut4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeOut4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   outputsFlexGridSizer->Add(timeUnitsOut4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);


   //output box
   GmatStaticBoxSizer *outputSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Outputs" );
   outputSizer->Add(outputsFlexGridSizer, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(showSummary, 0, wxALIGN_RIGHT|wxALL, bsize);

   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(1,2,0,0);
   pageSizer->Add(inputSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outputSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the attitude object into the widgets for the panel.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::LoadData()
{
   //default = sun sync
   orbitTypeComboBox->SetSelection(0);
   epochComboBox->SetSelection(6);
   DisplaySunSync();
   DisplayTime();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the widgets to the object, if applicable.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::SaveData()
{
   //canClose = true;

   if (mIsOrbitChanged)
      updateOrbit = true;
   else
          updateOrbit = false;

   mIsOrbitChanged = false;
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
/**
 * Resets the data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::ResetData()
{
   updateOrbit = false;
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from a combo box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == orbitTypeComboBox)
   {
      orbitType = orbitTypeComboBox->GetStringSelection().c_str();
      if (orbitType == "Sun Sync")
      {
         DisplaySunSync();
         DisplayTime();
      }
      else if (orbitType == "Repeat Sun Sync")
      {
         DisplayRepeatSunSync();
         DisplayTime();
      }
      else if (orbitType == "Repeat Ground Track")
      {
         DisplayRepeatGroundTrack();
         DisplayTime();
      }
      else if (orbitType == "Geostationary")
      {
         DisplayGeostationary();
      }
      else if (orbitType == "Molniya")
      {
         DisplayMolniya();
         DisplayTime();
      }
      else if (orbitType == "Frozen")
      {
         DisplayFrozen();
         DisplayTime();
      }
      //Refresh();
      outputsFlexGridSizer->Layout();
   }
   else if ((event.GetEventObject() == epochComboBox) )
   {
      epochType = epochComboBox->GetStringSelection().c_str();
      if (epochType == "A1ModJulian")
         timeIn1Val = "21545.00000039794";
      else if (epochType == "TAIModJulian")
         timeIn1Val = "21545";
      else if (epochType == "UTCModJulian")
         timeIn1Val = "21544.99962962963";
      else if (epochType == "TTModJulian")
         timeIn1Val = "21545.0003725";
      else if (epochType == "A1Gregorian")
         timeIn1Val = "01 Jan 2000 12:00:00.034";
      else if (epochType == "TAIGregorian")
         timeIn1Val = "01 Jan 2000 12:00:00.000";
      else if (epochType == "UTCGregorian")
         timeIn1Val = "01 Jan 2000 11:59:28.000";
      else if (epochType == "TTGregorian")
         timeIn1Val = "01 Jan 2000 12:00:32.184";
      time1TextCtrl->SetValue(timeIn1Val);
      time3TextCtrl->SetValue("12:00:00.000");
   }
}

//------------------------------------------------------------------------------
// void OnP1CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P1 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP1CheckBoxChange(wxCommandEvent& event)
{
   paramOneVal = paramOneCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramOneVal)
      {
         param1TextCtrl->Enable();
         paramTwoCheckBox->Disable();
         param2TextCtrl->Disable();
         units2StaticText->Disable();
         if (paramThreeVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
      }
      else
      {
         paramOneCheckBox->Enable();
         param1TextCtrl->Enable();
         units1StaticText->Enable();
   
         paramTwoCheckBox->Enable();
         param2TextCtrl->Enable();
         units2StaticText->Enable();

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();
         
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();
         
         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();
         
         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();
         
         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP2CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P2 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP2CheckBoxChange(wxCommandEvent& event)
{
   paramTwoVal = paramTwoCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramTwoVal)
      {
         param2TextCtrl->Enable();
         paramOneCheckBox->Disable();
         param1TextCtrl->Disable();
         units1StaticText->Disable();
         if (paramThreeVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
            
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
            
            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
            
            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
            
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
            
            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
            
            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
            
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
            
            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
            
            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
            
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
            
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
            
            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
            
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
            
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
            
            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
      }
      else
      {
         paramOneCheckBox->Enable();
         param1TextCtrl->Enable();
         units1StaticText->Enable();
         
         paramTwoCheckBox->Enable();
         param2TextCtrl->Enable();
         units2StaticText->Enable();
         
         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();
         
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();
         
         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();
         
         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();
         
         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Sun Sync")
   {
      if (paramTwoVal)
      {
         if (paramFourVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
         }
         else
         {
            paramFourCheckBox->Enable();
            param4TextCtrl->Enable();
            units4StaticText->Enable();

            paramThreeCheckBox->Enable();
            param3TextCtrl->Enable();
            units3StaticText->Enable();
         }
      }
      else
      {
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP3CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P3 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP3CheckBoxChange(wxCommandEvent& event)
{
   paramThreeVal = paramThreeCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramThreeVal)
      {
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
      }
      else
      {
         if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();
         }
         else
         {
            paramOneCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
   
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else
         {
            paramTwoCheckBox->Enable();
            param2TextCtrl->Enable();
            units2StaticText->Enable();
         }

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();

         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();

         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Sun Sync")
   {
      if (paramThreeVal)
      {
         if (paramFourVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
         }
         else
         {
            paramFourCheckBox->Enable();
            param4TextCtrl->Enable();
            units4StaticText->Enable();

            paramTwoCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
      }
      else
      {
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramTwoCheckBox->Enable();
         param2TextCtrl->Enable();
         units2StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Ground Track")
   {
      if (paramThreeVal)
      {
         if (paramFourVal)
         {
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
         }
         else
         {
            paramFourCheckBox->Enable();
            param4TextCtrl->Enable();
            units4StaticText->Enable();

            paramFiveCheckBox->Enable();
            param5TextCtrl->Enable();
            units5StaticText->Enable();
         }
      }
      else
      {
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP4CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P4 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP4CheckBoxChange(wxCommandEvent& event)
{
   paramFourVal = paramFourCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramFourVal)
      {
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
      }
      else
      {
         if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();
         }
         else
         {
            paramOneCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
   
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else
         {
            paramTwoCheckBox->Enable();
            param2TextCtrl->Enable();
            units2StaticText->Enable();
         }

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();

         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();

         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Sun Sync")
   {
      if (paramFourVal)
      {
         if (paramThreeVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
         }
         else
         {
            paramThreeCheckBox->Enable();
            param3TextCtrl->Enable();
            units3StaticText->Enable();

            paramTwoCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
      }
      else
      {
         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramTwoCheckBox->Enable();
         param2TextCtrl->Enable();
         units2StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Ground Track")
   {
      if (paramFourVal)
      {
         if (paramFiveVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
         }
         else
         {
            paramThreeCheckBox->Enable();
            param3TextCtrl->Enable();
            units3StaticText->Enable();

            paramFiveCheckBox->Enable();
            param5TextCtrl->Enable();
            units5StaticText->Enable();
         }
      }
      else
      {
         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP5CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P5 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP5CheckBoxChange(wxCommandEvent& event)
{
   paramFiveVal = paramFiveCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramFiveVal)
      {
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
      }
      else
      {
         if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();
         }
         else
         {
            paramOneCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
   
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else
         {
            paramTwoCheckBox->Enable();
            param2TextCtrl->Enable();
            units2StaticText->Enable();
         }

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();

         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();

         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
   else if (orbitType == "Repeat Ground Track")
   {
      if (paramFiveVal)
      {
         if (paramThreeVal)
         {
            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();
         }
         else
         {
            paramFourCheckBox->Enable();
            param4TextCtrl->Enable();
            units4StaticText->Enable();

            paramThreeCheckBox->Enable();
            param3TextCtrl->Enable();
            units3StaticText->Enable();
         }
      }
      else
      {
         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP6CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P6 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP6CheckBoxChange(wxCommandEvent& event)
{
   paramSixVal = paramSixCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramSixVal)
      {
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSevenCheckBox->Disable();
            param7TextCtrl->Disable();
            units7StaticText->Disable();
         }
         else if (paramSevenVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
         }
      }
      else
      {
         if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();
         }
         else
         {
            paramOneCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
   
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else
         {
            paramTwoCheckBox->Enable();
            param2TextCtrl->Enable();
            units2StaticText->Enable();
         }

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();

         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();

         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnP7CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the P7 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnP7CheckBoxChange(wxCommandEvent& event)
{
   paramSevenVal = paramSevenCheckBox->GetValue();
   if (orbitType == "Sun Sync")
   {
      if (paramSevenVal)
      {
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
         else if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
         else if (paramThreeVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
         else if (paramFourVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
         else if (paramFiveVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramSixCheckBox->Disable();
            param6TextCtrl->Disable();
            units6StaticText->Disable();
         }
         else if (paramSixVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();

            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();

            paramThreeCheckBox->Disable();
            param3TextCtrl->Disable();
            units3StaticText->Disable();

            paramFourCheckBox->Disable();
            param4TextCtrl->Disable();
            units4StaticText->Disable();

            paramFiveCheckBox->Disable();
            param5TextCtrl->Disable();
            units5StaticText->Disable();
         }
      }
      else
      {
         if (paramTwoVal)
         {
            paramOneCheckBox->Disable();
            param1TextCtrl->Disable();
            units1StaticText->Disable();
         }
         else
         {
            paramOneCheckBox->Enable();
            param1TextCtrl->Enable();
            units1StaticText->Enable();
         }
   
         if (paramOneVal)
         {
            paramTwoCheckBox->Disable();
            param2TextCtrl->Disable();
            units2StaticText->Disable();
         }
         else
         {
            paramTwoCheckBox->Enable();
            param2TextCtrl->Enable();
            units2StaticText->Enable();
         }

         paramThreeCheckBox->Enable();
         param3TextCtrl->Enable();
         units3StaticText->Enable();

         paramFourCheckBox->Enable();
         param4TextCtrl->Enable();
         units4StaticText->Enable();

         paramFiveCheckBox->Enable();
         param5TextCtrl->Enable();
         units5StaticText->Enable();

         paramSixCheckBox->Enable();
         param6TextCtrl->Enable();
         units6StaticText->Enable();

         paramSevenCheckBox->Enable();
         param7TextCtrl->Enable();
         units7StaticText->Enable();
      }
   }
}

//------------------------------------------------------------------------------
// void OnTP1CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the TP1 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnTP1CheckBoxChange(wxCommandEvent& event)
{
   timeParamOneVal = timeParamOneCheckBox->GetValue();
   if (timeParamOneVal)
   {
      time1TextCtrl->Enable();
      timeUnits1StaticText->Enable();

      timeParamTwoCheckBox->Enable();
      time2TextCtrl->Enable();
      timeUnits2StaticText->Enable();

      timeParamThreeCheckBox->Enable();
      time3TextCtrl->Enable();
      timeUnits3StaticText->Enable();
   }
   else
   {
      time1TextCtrl->Disable();
      timeUnits1StaticText->Disable();

      timeParamTwoCheckBox->Disable();
      timeParamTwoCheckBox->SetValue(false);
      time2TextCtrl->Disable();
      timeUnits2StaticText->Disable();

      timeParamThreeCheckBox->Disable();
      timeParamThreeCheckBox->SetValue(false);
      time3TextCtrl->Disable();
      timeUnits3StaticText->Disable();
   }
}

//------------------------------------------------------------------------------
// void OnTP2CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the TP2 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnTP2CheckBoxChange(wxCommandEvent& event)
{
   timeParamTwoVal = timeParamTwoCheckBox->GetValue();
   if (timeParamTwoVal)
   {
      timeParamThreeCheckBox->Disable();
      time3TextCtrl->Disable();
      timeUnits3StaticText->Disable();
   }
   else
   {
      timeParamThreeCheckBox->Enable();
      time3TextCtrl->Enable();
      timeUnits3StaticText->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnTP3CheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a selection from the TP3 check box.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnTP3CheckBoxChange(wxCommandEvent& event)
{
   timeParamThreeVal = timeParamThreeCheckBox->GetValue();
   if (timeParamThreeVal)
   {
      timeParamTwoCheckBox->Disable();
      time2TextCtrl->Disable();
      timeUnits2StaticText->Disable();
   }
   else
   {
      timeParamTwoCheckBox->Enable();
      time2TextCtrl->Enable();
      timeUnits2StaticText->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnParamChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user makes a change to the parameter.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnParamChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == param1TextCtrl)
   {
      wxString in1 = param1TextCtrl->GetValue();
      in1.ToDouble(&input1Val);
   }
   else if (event.GetEventObject() == param2TextCtrl)
   {
      wxString in2 = param2TextCtrl->GetValue();
      in2.ToDouble(&input2Val);
   }
   else if (event.GetEventObject() == param3TextCtrl)
   {
      wxString in3 = param3TextCtrl->GetValue();
      in3.ToDouble(&input3Val);
   }
   else if (event.GetEventObject() == param4TextCtrl)
   {
      wxString in4 = param4TextCtrl->GetValue();
      in4.ToDouble(&input4Val);
   }
   else if (event.GetEventObject() == param5TextCtrl)
   {
      wxString in5 = param5TextCtrl->GetValue();
      in5.ToDouble(&input5Val);
   }
   else if (event.GetEventObject() == param6TextCtrl)
   {
      wxString in6 = param6TextCtrl->GetValue();
      in6.ToDouble(&input6Val);
   }
   else if (event.GetEventObject() == param7TextCtrl)
   {
      wxString in7 = param7TextCtrl->GetValue();
      in7.ToDouble(&input7Val);
   }
   else if (event.GetEventObject() == time1TextCtrl)
   {
      timeIn1Val = time1TextCtrl->GetValue();
   }
   else if (event.GetEventObject() == time2TextCtrl)
   {
      wxString tIn2 = time2TextCtrl->GetValue();
      tIn2.ToDouble(&timeIn2Val);
   }
   else if (event.GetEventObject() == time3TextCtrl)
   {
      timeIn3Val=time3TextCtrl->GetValue();
   }

}

//------------------------------------------------------------------------------
// void OnFindOrbit(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user selects the Find Orbit button.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnFindOrbit(wxCommandEvent& event)
{
   canClose = true;

   if (orbitType == "Sun Sync")
   {
      orbitSS.CalculateSunSync(paramOneVal, input1Val, paramTwoVal, input2Val, paramThreeVal, 
                               input3Val, paramFourVal, input4Val, paramFiveVal, input5Val, paramSixVal, input6Val, 
                               paramSevenVal, input7Val);
      if (orbitSS.IsError())
      {
         mIsOrbitChanged = false;
         canClose = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, orbitSS.GetError());
         return;
      }
      wxString sma;
      sma.Printf("%.16f", orbitSS.GetSMA());
      output1TextCtrl->SetValue(sma);
      wxString alt;
      alt.Printf("%.16f", orbitSS.GetALT());
      output2TextCtrl->SetValue(alt);
      wxString ecc;
      ecc.Printf("%.16f", orbitSS.GetECC());
      output3TextCtrl->SetValue(ecc);
      wxString inc;
      inc.Printf("%.16f", orbitSS.GetINC());
      output4TextCtrl->SetValue(inc);
      wxString rop;
      rop.Printf("%.16f", orbitSS.GetROP());
      output5TextCtrl->SetValue(rop);
      wxString roa;
      roa.Printf("%.16f", orbitSS.GetROA());
      output6TextCtrl->SetValue(roa);
      wxString p;
      p.Printf("%.16f", orbitSS.GetP());
      output7TextCtrl->SetValue(p);

      mIsOrbitChanged = true;
   }
   else if (orbitType == "Repeat Sun Sync")
   {
      orbitRSS.CalculateRepeatSunSync(paramOneVal, input1Val, paramTwoVal, input2Val, paramThreeVal,
                                      input3Val, paramFourVal, input4Val);
      if (orbitRSS.IsError())
      {
         canClose = false;
         mIsOrbitChanged = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, orbitRSS.GetError());
         return;
      }
      wxString sma;
      sma.Printf("%.16f", orbitRSS.GetSMA());
      output1TextCtrl->SetValue(sma);
      wxString alt;
      alt.Printf("%.16f", orbitRSS.GetALT());
      output2TextCtrl->SetValue(alt);
      wxString ecc;
      ecc.Printf("%.16f", orbitRSS.GetECC());
      output3TextCtrl->SetValue(ecc);
      wxString inc;
      inc.Printf("%.16f", orbitRSS.GetINC());
      output4TextCtrl->SetValue(inc);
      wxString rop;
      rop.Printf("%.16f", orbitRSS.GetROP());
      output5TextCtrl->SetValue(rop);
      wxString roa;
      roa.Printf("%.16f", orbitRSS.GetROA());
      output6TextCtrl->SetValue(roa);
      wxString p;
      p.Printf("%.16f", orbitRSS.GetP());
      output7TextCtrl->SetValue(p);

      mIsOrbitChanged = true;
   }
   else if (orbitType == "Repeat Ground Track")
   {
      orbitRGT.CalculateRepeatGroundTrack(paramOneVal, input1Val, paramTwoVal, input2Val, 
                                          paramThreeVal, input3Val, paramFourVal, input4Val, 
                                          paramFiveVal, input5Val);
      if (orbitRGT.IsError())
      {
         canClose = false;
         mIsOrbitChanged = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, orbitRGT.GetError());
         return;
      }
      wxString sma;
      sma.Printf("%.16f", orbitRGT.GetSMA());
      output1TextCtrl->SetValue(sma);
      wxString alt;
      alt.Printf("%.16f", orbitRGT.GetALT());
      output2TextCtrl->SetValue(alt);
      wxString ecc;
      ecc.Printf("%.16f", orbitRGT.GetECC());
      output3TextCtrl->SetValue(ecc);
      wxString inc;
      inc.Printf("%.16f", orbitRGT.GetINC());
      output4TextCtrl->SetValue(inc);
      wxString rop;
      rop.Printf("%.16f", orbitRGT.GetROP());
      output5TextCtrl->SetValue(rop);
      wxString roa;
      roa.Printf("%.16f", orbitRGT.GetROA());
      output6TextCtrl->SetValue(roa);
      wxString p;
      p.Printf("%.16f", orbitRGT.GetP());
      output7TextCtrl->SetValue(p);

      mIsOrbitChanged = true;
   }
   else if (orbitType == "Frozen")
   {
      orbitFZN.CalculateFrozen(input1Val, paramOneVal, input2Val, paramTwoVal);
      if (orbitFZN.IsError())
      {
         canClose = false;
         mIsOrbitChanged = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, orbitFZN.GetError());
         return;
      }
      wxString sma;
      sma.Printf("%.16f", orbitFZN.GetSMA());
      output1TextCtrl->SetValue(sma);
      wxString aalt;
      aalt.Printf("%.16f", orbitFZN.GetAALT());
      output2TextCtrl->SetValue(aalt);
      wxString palt;
      palt.Printf("%.16f", orbitFZN.GetPALT());
      output3TextCtrl->SetValue(palt);
      wxString ecc;
      ecc.Printf("%.16f", orbitFZN.GetECC());
      output4TextCtrl->SetValue(ecc);

      mIsOrbitChanged = true;
   }

   if (timeParamOneVal)
   {
      std::string str = time1TextCtrl->GetValue().WX_TO_STD_STRING;
      orbitTime.SetEpoch(str);
      if ((epochType == "UTCGregorian") || (epochType == "A1Gregorian") || 
          (epochType == "TAIGregorian") || (epochType == "TTGregorian"))
      {
         if(!CheckTimeFormatAndValue("UTCGregorian", str, "Epoch"))
            return;
      }
      else
      {
         if(!CheckTimeFormatAndValue("UTCModJulian", str, "Epoch"))
            return;
      }
      if (orbitType == "Geostationary")
      {
         orbitTime = OrbitDesignerTime(std::string(timeIn1Val), std::string(epochType), 
                                       timeParamTwoVal, timeIn2Val, timeParamThreeVal, 
                                       std::string(timeIn3Val));
         if (!timeParamTwoVal && timeParamThreeVal)
         {
            str = "01 Jan 2000 " + time3TextCtrl->GetValue();
            if(!CheckTimeFormatAndValue("UTCGregorian", str, "Initial Local Sidereal Time"))
               return;
            orbitTime.FindRAAN();
            if (orbitTime.IsError())
            {
               isEpochChanged = false;
               mIsOrbitChanged = false;
               canClose = false;
               MessageInterface::PopupMessage(Gmat::ERROR_, orbitTime.GetError());
               return;
            }
            Real lonVal = orbitTime.GetRAAN();
            if (lonVal>180)
               lonVal = lonVal-360;
            wxString lon;
            lon.Printf("%.16f", lonVal);//use value of raan for value of lon in geosynchronous case
            timeOut2TextCtrl->SetValue(lon);//TA
            timeOut3TextCtrl->SetValue(time3TextCtrl->GetValue());//start time
            timeOut4TextCtrl->SetValue(lon);//longitude
         }
         else if (timeParamTwoVal && !timeParamThreeVal)
         {
            timeOut2TextCtrl->SetValue(time2TextCtrl->GetValue());//TA
            Real lon;
            std::string s = time2TextCtrl->GetValue().WX_TO_STD_STRING;
            GmatStringUtil::ToReal(s, lon);
            std::string start = orbitTime.FindStartTime(true, lon);
            if (orbitTime.IsError())
            {
               isEpochChanged = false;
               canClose = false;
               mIsOrbitChanged = false;
               MessageInterface::PopupMessage(Gmat::ERROR_, orbitTime.GetError());
               return;
            }
            timeOut3TextCtrl->SetValue(start.c_str());//start time
            timeOut4TextCtrl->SetValue(time2TextCtrl->GetValue());//longitude
         }
         else
         {
            timeOut2TextCtrl->SetValue("");//TA
            timeOut3TextCtrl->SetValue("");//start time
            timeOut4TextCtrl->SetValue("");//longitude
         }
         timeOut1TextCtrl->SetValue(time1TextCtrl->GetValue());//epoch
         isEpochChanged = true;
         mIsOrbitChanged = true;
      }
      else
      {
         orbitTime = OrbitDesignerTime(std::string(timeIn1Val), std::string(epochType), 
                                       timeParamTwoVal, timeIn2Val, timeParamThreeVal, 
                                       std::string(timeIn3Val));
         timeOut1TextCtrl->SetValue(orbitTime.GetEpoch().c_str());
         std::string start = orbitTime.GetStartTime();
         if (!timeParamTwoVal && timeParamThreeVal)
         {
            orbitTime.FindRAAN();
            if (orbitTime.IsError())
            {
               isEpochChanged = false;
               canClose = false;
               mIsOrbitChanged = false;
               MessageInterface::PopupMessage(Gmat::ERROR_, orbitTime.GetError());
               return;
            }
            wxString raan;
            raan.Printf("%.16f", orbitTime.GetRAAN());
            timeOut2TextCtrl->SetValue(raan);
            timeOut3TextCtrl->SetValue(start.c_str());
         }
         else if (timeParamTwoVal && !timeParamThreeVal)
         {
            start = orbitTime.FindStartTime();
            if (orbitTime.IsError())
            {
               isEpochChanged = false;
               canClose = false;
               mIsOrbitChanged = false;
               MessageInterface::PopupMessage(Gmat::ERROR_, orbitTime.GetError());
               return;
            }
            wxString raan;
            raan.Printf("%.16f", orbitTime.GetRAAN());
            timeOut2TextCtrl->SetValue(raan);
            timeOut3TextCtrl->SetValue(start.c_str());
         }
         else
         {
            orbitTime.SetRAAN(theSpacecraft->GetRealParameter(10009));
            timeOut2TextCtrl->SetValue("");
            timeOut3TextCtrl->SetValue("");
         }
                  
         isEpochChanged = true;
      }
   }
   else
   {
      orbitTime.SetRAAN(theSpacecraft->GetRealParameter(10009));
      isEpochChanged = false;
   }

}

//------------------------------------------------------------------------------
// void OnSummary(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles case when user selects the Summary button.
 *
 * @param event  the wxCommandEvent to be handled
 *
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::OnSummary(wxCommandEvent& event)
{
   summaryString = "";
   summaryString += "SMA = ";
   summaryString += output1TextCtrl->GetValue();
   summaryString += "\n";

   if (orbitType == "Frozen")
   {
      summaryString += "Apogee ALT = ";
      summaryString += output2TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "Perigee ALT = ";
      summaryString += output3TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "ECC = ";
      summaryString += output4TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "INC = ";
      summaryString += param2TextCtrl->GetValue();
      summaryString += "\n";
   }
   else
   {
      summaryString += "ALT = ";
      summaryString += output2TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "ECC = ";
      summaryString += output3TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "INC = ";
      summaryString += output4TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "RP = ";
      summaryString += output5TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "RA = ";
      summaryString += output6TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "P = ";
      summaryString += output7TextCtrl->GetValue();
      summaryString += "\n";
   }

   if (orbitType == "Molniya")
   {
      summaryString += "AOP = ";
      summaryString += output8TextCtrl->GetValue();
      summaryString += "\n";
   }

   summaryString += "Epoch = ";
   summaryString += timeOut1TextCtrl->GetValue();
   summaryString += "\n";

   if (orbitType == "Geostationary")
   {
      summaryString += "RAAN = ";
      summaryString += output8TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "AOP = ";
      summaryString += output9TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "TA = ";
      summaryString += timeOut2TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "Longitude = ";
      summaryString += timeOut4TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "Initial Local Sidereal Time = ";
      summaryString += timeOut3TextCtrl->GetValue();
      summaryString += "\n";
   }
   else
   {
      summaryString += "RAAN = ";
      summaryString += timeOut2TextCtrl->GetValue();
      summaryString += "\n";
      summaryString += "Initial Local Sidereal Time = ";
      summaryString += timeOut3TextCtrl->GetValue();
      summaryString += "\n";
   }

   OrbitSummaryDialog summaryDlg(this, summaryString);
   summaryDlg.ShowModal();
}

//------------------------------------------------------------------------------
// void DisplaySunSync()
//------------------------------------------------------------------------------
/**
 * Displays the SunSync data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplaySunSync()
{
   //show inputs
   paramOneCheckBox->Show(true);
   paramOneCheckBox->SetLabel(wxT("SMA"));
   paramOneCheckBox->Enable();
   paramOneCheckBox->SetValue(true);
   paramOneVal = true;
   param1TextCtrl->Show(true);
   param1TextCtrl->Enable();
   param1TextCtrl->SetValue(wxT("7000.0000000000000000"));
   param1TextCtrl->SetToolTip("6478.1363<SMA<8378.1363");
   std::string in1 = param1TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in1, input1Val);
   units1StaticText->Show(true);
   units1StaticText->SetLabel(wxT("km"));
   units1StaticText->Enable();
   
   paramTwoCheckBox->Show(true);
   paramTwoCheckBox->Disable();
   paramTwoCheckBox->SetLabel(wxT("Mean ALT"));
   paramTwoCheckBox->SetValue(false);
   paramTwoVal = false;
   param2TextCtrl->Show(true);
   param2TextCtrl->Disable();
   param2TextCtrl->SetValue(wxT("621.863699999999880"));
   param2TextCtrl->SetToolTip("100<ALT<2000");
   std::string in2 = param2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in2, input2Val);
   units2StaticText->Show(true);
   units2StaticText->SetLabel(wxT("km"));
   units2StaticText->Disable();

   paramThreeCheckBox->Show(true);
   paramThreeCheckBox->Enable();
   paramThreeCheckBox->SetLabel(wxT("ECC"));
   paramThreeCheckBox->SetValue(true);
   paramThreeVal = true;
   param3TextCtrl->Show(true);
   param3TextCtrl->Enable();
   param3TextCtrl->SetValue(wxT("0.0010000000000000"));
   param3TextCtrl->SetToolTip("0<=ECC<1");
   std::string in3 = param3TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in3, input3Val);
   units3StaticText->Show(true);
   units3StaticText->SetLabel(wxT(""));
   units3StaticText->Enable();

   paramFourCheckBox->Show(true);
   paramFourCheckBox->Disable();
   paramFourCheckBox->SetLabel(wxT("INC"));
   paramFourCheckBox->SetValue(false);
   paramFourVal = false;
   param4TextCtrl->Show(true);
   param4TextCtrl->Disable();
   param4TextCtrl->SetValue(wxT("97.8739286731882600"));
   param4TextCtrl->SetToolTip("90<INC<110");
   std::string in4 = param4TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in4, input4Val);
   units4StaticText->Show(true);
   units4StaticText->SetLabel(wxT("deg"));
   units4StaticText->Disable();

   paramFiveCheckBox->Show(true);
   paramFiveCheckBox->Disable();
   paramFiveCheckBox->SetLabel(wxT("RP"));
   paramFiveCheckBox->SetValue(false);
   paramFiveVal = false;
   param5TextCtrl->Show(true);
   param5TextCtrl->Disable();
   param5TextCtrl->SetValue(wxT("6993.0000000000000000"));
   param5TextCtrl->SetToolTip("6478.1363<RP<8378.1363");
   std::string in5 = param5TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in5, input5Val);
   units5StaticText->Show(true);
   units5StaticText->SetLabel(wxT("km"));
   units5StaticText->Disable();

   paramSixCheckBox->Show(true);
   paramSixCheckBox->Disable();
   paramSixCheckBox->SetLabel(wxT("RA"));
   paramSixCheckBox->SetValue(false);
   paramSixVal = false;
   param6TextCtrl->Show(true);
   param6TextCtrl->Disable();
   param6TextCtrl->SetValue(wxT("7006.9999999999991000"));
   param6TextCtrl->SetToolTip("6478.1363<RA<10278.1363");
   std::string in6 = param6TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in6, input6Val);
   units6StaticText->Show(true);
   units6StaticText->SetLabel(wxT("km"));
   units6StaticText->Disable();

   paramSevenCheckBox->Show(true);
   paramSevenCheckBox->Disable();
   paramSevenCheckBox->SetLabel(wxT("P"));
   paramSevenCheckBox->SetValue(false);
   paramSevenVal = false;
   param7TextCtrl->Show(true);
   param7TextCtrl->Disable();
   param7TextCtrl->SetValue(wxT("6999.9929999999995000"));
   param7TextCtrl->SetToolTip("0<P<8378.1363");
   std::string in7 = param7TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in7, input7Val);
   units7StaticText->Show(true);
   units7StaticText->SetLabel(wxT("km"));
   units7StaticText->Disable();

   //ShowOutputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxT(""));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT("km"));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Altitude"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxT(""));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("ECC"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT(""));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT(""));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("INC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT(""));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("deg"));
   unitsOut4StaticText->Show(true);

   paramOut5StaticText->SetLabel(wxT("RP"));
   paramOut5StaticText->Show(true);
   output5TextCtrl->SetValue(wxT(""));
   output5TextCtrl->Show(true);
   unitsOut5StaticText->SetLabel(wxT("km"));
   unitsOut5StaticText->Show(true);

   paramOut6StaticText->SetLabel(wxT("RA"));
   paramOut6StaticText->Show(true);
   output6TextCtrl->SetValue(wxT(""));
   output6TextCtrl->Show(true);
   unitsOut6StaticText->SetLabel(wxT("km"));
   unitsOut6StaticText->Show(true);

   paramOut7StaticText->SetLabel(wxT("P"));
   paramOut7StaticText->Show(true);
   output7TextCtrl->SetValue(wxT(""));
   output7TextCtrl->Show(true);
   unitsOut7StaticText->SetLabel(wxT("km"));
   unitsOut7StaticText->Show(true);

   //hide unused
   paramOut8StaticText->Show(false);
   output8TextCtrl->Show(false);
   unitsOut8StaticText->Show(false);
   paramOut9StaticText->Show(false);
   output9TextCtrl->Show(false);
   unitsOut9StaticText->Show(false);
}

//------------------------------------------------------------------------------
// void DisplayRepeatSunSync()
//------------------------------------------------------------------------------
/**
 * Displays the RepeatSunSync data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayRepeatSunSync()
{
   //show inputs
   paramOneCheckBox->Show(true);
   paramOneCheckBox->Enable();
   paramOneCheckBox->SetLabel(wxT("ECC"));
   paramOneCheckBox->SetValue(true);
   paramOneVal = true;
   param1TextCtrl->Show(true);
   param1TextCtrl->Enable();
   param1TextCtrl->SetValue(wxT("0.001"));
   param1TextCtrl->SetToolTip("0<=ECC<1");
   std::string in1 = param1TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in1, input1Val);
   units1StaticText->Show(true);
   units1StaticText->SetLabel(wxT(""));
   units1StaticText->Enable();

   paramTwoCheckBox->Show(true);
   paramTwoCheckBox->Enable();
   paramTwoCheckBox->SetLabel(wxT("Days To Repeat"));
   paramTwoCheckBox->SetValue(false);
   paramTwoVal = false;
   param2TextCtrl->Show(true);
   param2TextCtrl->Enable();
   param2TextCtrl->SetValue(wxT("16"));
   param2TextCtrl->SetToolTip("");
   std::string in2 = param2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in2, input2Val);
   units2StaticText->Show(true);
   units2StaticText->SetLabel(wxT(""));
   units2StaticText->Enable();
   
   paramThreeCheckBox->Show(true);
   paramThreeCheckBox->Enable();
   paramThreeCheckBox->SetLabel(wxT("Revs To Repeat"));
   paramThreeCheckBox->SetValue(false);
   paramThreeVal = false;
   param3TextCtrl->Show(true);
   param3TextCtrl->Enable();
   param3TextCtrl->SetValue(wxT("232"));
   param3TextCtrl->SetToolTip("");
   std::string in3 = param3TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in3, input3Val);
   units3StaticText->Show(true);
   units3StaticText->SetLabel(wxT(""));
   units3StaticText->Enable();

   paramFourCheckBox->Show(true);
   paramFourCheckBox->Enable();
   paramFourCheckBox->SetLabel(wxT("Revs Per Day"));
   paramFourCheckBox->SetValue(false);
   paramFourVal = false;
   param4TextCtrl->Show(true);
   param4TextCtrl->Enable();
   param4TextCtrl->SetValue(wxT("14.5"));
   param4TextCtrl->SetToolTip("");
   std::string in4 = param4TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in4, input4Val);
   units4StaticText->Show(true);
   units4StaticText->SetLabel(wxT(""));
   units4StaticText->Enable();

   //hide unused
   paramFiveCheckBox->Show(false);
   paramSixCheckBox->Show(false);
   paramSevenCheckBox->Show(false);
   param5TextCtrl->Show(false);
   param6TextCtrl->Show(false);
   param7TextCtrl->Show(false);
   units5StaticText->Show(false);
   units6StaticText->Show(false);
   units7StaticText->Show(false);

   //show outputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxT(""));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT("km"));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Altitude"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxT(""));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("ECC"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT(""));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT(""));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("INC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT(""));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("deg"));
   unitsOut4StaticText->Show(true);

   paramOut5StaticText->SetLabel(wxT("RP"));
   paramOut5StaticText->Show(true);
   output5TextCtrl->SetValue(wxT(""));
   output5TextCtrl->Show(true);
   unitsOut5StaticText->SetLabel(wxT("km"));
   unitsOut5StaticText->Show(true);

   paramOut6StaticText->SetLabel(wxT("RA"));
   paramOut6StaticText->Show(true);
   output6TextCtrl->SetValue(wxT(""));
   output6TextCtrl->Show(true);
   unitsOut6StaticText->SetLabel(wxT("km"));
   unitsOut6StaticText->Show(true);

   paramOut7StaticText->SetLabel(wxT("P"));
   paramOut7StaticText->Show(true);
   output7TextCtrl->SetValue(wxT(""));
   output7TextCtrl->Show(true);
   unitsOut7StaticText->SetLabel(wxT("km"));
   unitsOut7StaticText->Show(true);

   //hide unused
   paramOut8StaticText->Show(false);
   output8TextCtrl->Show(false);
   unitsOut8StaticText->Show(false);
   paramOut9StaticText->Show(false);
   output9TextCtrl->Show(false);
   unitsOut9StaticText->Show(false);
}

//------------------------------------------------------------------------------
// void DisplayRepeatGroundTrack()
//------------------------------------------------------------------------------
/**
 * Displays the RepeatGroundTrack data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayRepeatGroundTrack()
{
   //show inputs
   paramOneCheckBox->Show(true);
   paramOneCheckBox->Enable();
   paramOneCheckBox->SetLabel(wxT("ECC"));
   paramOneCheckBox->SetValue(true);
   paramOneVal = true;
   param1TextCtrl->Show(true);
   param1TextCtrl->Enable();
   param1TextCtrl->SetValue(wxT("0.00117604"));
   param1TextCtrl->SetToolTip("0<=ECC<1");
   std::string in1 = param1TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in1, input1Val);
   units1StaticText->Show(true);
   units1StaticText->SetLabel(wxT(""));
   units1StaticText->Enable();

   paramTwoCheckBox->Show(true);
   paramTwoCheckBox->Enable();
   paramTwoCheckBox->SetLabel(wxT("INC"));
   paramTwoCheckBox->SetValue(true);
   paramTwoVal = true;
   param2TextCtrl->Show(true);
   param2TextCtrl->Enable();
   param2TextCtrl->SetValue(wxT("98.2"));
   param2TextCtrl->SetToolTip("0<INC<180");
   std::string in2 = param2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in2, input2Val);
   units2StaticText->Show(true);
   units2StaticText->SetLabel(wxT("deg"));
   units2StaticText->Enable();

   paramThreeCheckBox->Show(true);
   paramThreeCheckBox->Enable();
   paramThreeCheckBox->SetLabel(wxT("Revs To Repeat"));
   paramThreeCheckBox->SetValue(false);
   paramThreeVal = false;
   param3TextCtrl->Show(true);
   param3TextCtrl->Enable();
   param3TextCtrl->SetValue(wxT("232"));
   param3TextCtrl->SetToolTip("");
   std::string in3 = param3TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in3, input3Val);
   units3StaticText->Show(true);
   units3StaticText->SetLabel(wxT(""));
   units3StaticText->Enable();

   paramFourCheckBox->Show(true);
   paramFourCheckBox->Enable();
   paramFourCheckBox->SetLabel(wxT("Days To Repeat"));
   paramFourCheckBox->SetValue(false);
   paramFourVal = false;
   param4TextCtrl->Show(true);
   param4TextCtrl->Enable();
   param4TextCtrl->SetValue(wxT("16"));
   param4TextCtrl->SetToolTip("");
   std::string in4 = param4TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in4, input4Val);
   units4StaticText->Show(true);
   units4StaticText->SetLabel(wxT(""));
   units4StaticText->Enable();

   paramFiveCheckBox->Show(true);
   paramFiveCheckBox->Enable();
   paramFiveCheckBox->SetLabel(wxT("Revs Per Day"));
   paramFiveCheckBox->SetValue(false);
   paramFiveVal = false;
   param5TextCtrl->Show(true);
   param5TextCtrl->Enable();
   param5TextCtrl->SetValue(wxT("14.5"));
   param5TextCtrl->SetToolTip("");
   std::string in5 = param5TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in5, input5Val);
   units5StaticText->Show(true);
   units5StaticText->SetLabel(wxT(""));
   units5StaticText->Enable();

   //hide unused
   paramSixCheckBox->Show(false);
   paramSevenCheckBox->Show(false);
   param6TextCtrl->Show(false);
   param7TextCtrl->Show(false);
   units6StaticText->Show(false);
   units7StaticText->Show(false);

   //show outputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxT(""));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT("km"));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Altitude"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxT(""));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("ECC"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT(""));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT(""));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("INC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT(""));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("deg"));
   unitsOut4StaticText->Show(true);

   paramOut5StaticText->SetLabel(wxT("RP"));
   paramOut5StaticText->Show(true);
   output5TextCtrl->SetValue(wxT(""));
   output5TextCtrl->Show(true);
   unitsOut5StaticText->SetLabel(wxT("km"));
   unitsOut5StaticText->Show(true);

   paramOut6StaticText->SetLabel(wxT("RA"));
   paramOut6StaticText->Show(true);
   output6TextCtrl->SetValue(wxT(""));
   output6TextCtrl->Show(true);
   unitsOut6StaticText->SetLabel(wxT("km"));
   unitsOut6StaticText->Show(true);

   paramOut7StaticText->SetLabel(wxT("P"));
   paramOut7StaticText->Show(true);
   output7TextCtrl->SetValue(wxT(""));
   output7TextCtrl->Show(true);
   unitsOut7StaticText->SetLabel(wxT("km"));
   unitsOut7StaticText->Show(true);

   //hide unused
   paramOut8StaticText->Show(false);
   output8TextCtrl->Show(false);
   unitsOut8StaticText->Show(false);
   paramOut9StaticText->Show(false);
   output9TextCtrl->Show(false);
   unitsOut9StaticText->Show(false);
}

//------------------------------------------------------------------------------
// void DisplayGeostationary()
//------------------------------------------------------------------------------
/**
 * Displays the Geostationary data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayGeostationary()
{
   Real SMA = 
      pow(GmatSolarSystemDefaults::PLANET_MU[2]*pow(86164.09054/(2*GmatMathConstants::PI),2),1/3.0);
   //show inputs
   timeSizer->Show(true);
   timeParamOneCheckBox->Show(true);
   timeParamOneCheckBox->Enable();
   timeParamOneCheckBox->SetLabel(wxT("Epoch"));
   timeParamOneCheckBox->SetValue(false);
   timeParamOneVal = false;
   time1TextCtrl->Show(true);
   time1TextCtrl->Disable();
   time1TextCtrl->SetValue(wxT("01 Jan 2000 11:59:28.000"));
   time1TextCtrl->SetToolTip("DD MM YYYY HH:MM:SS.s");
   timeIn1Val = time1TextCtrl->GetValue();
   timeUnits1StaticText->Show(true);
   timeUnits1StaticText->SetLabel(wxT(""));
   timeUnits1StaticText->Disable();

   timeParamTwoCheckBox->Show(true);
   timeParamTwoCheckBox->Disable();
   timeParamTwoCheckBox->SetLabel(wxT("Longitude"));
   timeParamTwoCheckBox->SetValue(false);
   timeParamTwoVal = false;
   time2TextCtrl->Show(true);
   time2TextCtrl->Disable();
   time2TextCtrl->SetValue(wxT("-79.5390165757071940"));
   time1TextCtrl->SetToolTip("-180<Lon<180");
   std::string tIn2 = time2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(tIn2, timeIn2Val);
   timeUnits2StaticText->Show(true);
   timeUnits2StaticText->SetLabel(wxT("deg"));
   timeUnits2StaticText->Disable();

   timeParamThreeCheckBox->Show(true);
   timeParamThreeCheckBox->Disable();
   timeParamThreeCheckBox->SetLabel(wxT("Initial Local Sidereal Time"));
   timeParamThreeCheckBox->SetValue(false);
   timeParamThreeVal = false;
   time3TextCtrl->Show(true);
   time3TextCtrl->Disable();
   time3TextCtrl->SetValue(wxT("12:00:00.0"));
   time3TextCtrl->SetToolTip("HH:MM:SS.s");
   timeIn3Val = time3TextCtrl->GetValue();
   timeUnits3StaticText->Show(true);
   timeUnits3StaticText->SetLabel(wxT(""));
   timeUnits3StaticText->Disable();

   //hide unused
   paramOneCheckBox->Show(false);
   paramTwoCheckBox->Show(false);
   paramThreeCheckBox->Show(false);
   paramFourCheckBox->Show(false);
   paramFiveCheckBox->Show(false);
   paramSixCheckBox->Show(false);
   paramSevenCheckBox->Show(false);
   param1TextCtrl->Show(false);
   param2TextCtrl->Show(false);
   param3TextCtrl->Show(false);
   param4TextCtrl->Show(false);
   param5TextCtrl->Show(false);
   param6TextCtrl->Show(false);
   param7TextCtrl->Show(false);
   units1StaticText->Show(false);
   units2StaticText->Show(false);
   units3StaticText->Show(false);
   units4StaticText->Show(false);
   units5StaticText->Show(false);
   units6StaticText->Show(false);
   units7StaticText->Show(false);

   //show outputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxString(GmatStringUtil::ToString(SMA).c_str()));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT("km"));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Altitude"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxString(GmatStringUtil::ToString(SMA - 
                                      GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]).c_str()));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("ECC"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT("0"));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT(""));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("INC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT("0"));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("deg"));
   unitsOut4StaticText->Show(true);

   paramOut5StaticText->SetLabel(wxT("RP"));
   paramOut5StaticText->Show(true);
   output5TextCtrl->SetValue(wxString(GmatStringUtil::ToString(SMA).c_str()));
   output5TextCtrl->Show(true);
   unitsOut5StaticText->SetLabel(wxT("km"));
   unitsOut5StaticText->Show(true);

   paramOut6StaticText->SetLabel(wxT("RA"));
   paramOut6StaticText->Show(true);
   output6TextCtrl->SetValue(wxString(GmatStringUtil::ToString(SMA).c_str()));
   output6TextCtrl->Show(true);
   unitsOut6StaticText->SetLabel(wxT("km"));
   unitsOut6StaticText->Show(true);

   paramOut7StaticText->SetLabel("P");
   paramOut7StaticText->Show(true);
   output7TextCtrl->SetValue(wxString(GmatStringUtil::ToString(SMA).c_str()));
   output7TextCtrl->Show(true);
   unitsOut7StaticText->SetLabel(wxT("km"));
   unitsOut7StaticText->Show(true);

   paramOut8StaticText->SetLabel("RAAN");
   paramOut8StaticText->Show(true);
   output8TextCtrl->SetValue(wxT("0"));
   output8TextCtrl->Show(true);
   unitsOut8StaticText->SetLabel(wxT("deg"));
   unitsOut8StaticText->Show(true);

   paramOut9StaticText->SetLabel("AOP");
   paramOut9StaticText->Show(true);
   output9TextCtrl->SetValue(wxT("0"));
   output9TextCtrl->Show(true);
   unitsOut9StaticText->SetLabel(wxT("deg"));
   unitsOut9StaticText->Show(true);

   timeParamOut1StaticText->SetLabel(wxT("Epoch"));
   timeParamOut1StaticText->Show(true);
   timeOut1TextCtrl->SetValue(wxT(""));
   timeOut1TextCtrl->Show(true);
   timeUnitsOut1StaticText->SetLabel(wxT(""));
   timeUnitsOut1StaticText->Show(true);

   timeParamOut2StaticText->SetLabel(wxT("TA"));
   timeParamOut2StaticText->Show(true);
   timeOut2TextCtrl->SetValue(wxT(""));
   timeOut2TextCtrl->Show(true);
   timeUnitsOut2StaticText->SetLabel(wxT("deg"));
   timeUnitsOut2StaticText->Show(true);

   timeParamOut3StaticText->SetLabel(wxT("Initial Local Time"));
   timeParamOut3StaticText->Show(true);
   timeOut3TextCtrl->SetValue(wxT(""));
   timeOut3TextCtrl->Show(true);
   timeUnitsOut3StaticText->SetLabel(wxT(""));
   timeUnitsOut3StaticText->Show(true);

   timeParamOut4StaticText->SetLabel(wxT("Longitude"));
   timeParamOut4StaticText->Show(true);
   timeOut4TextCtrl->SetValue(wxT(""));
   timeOut4TextCtrl->Show(true);
   timeUnitsOut4StaticText->SetLabel(wxT("deg"));
   timeUnitsOut4StaticText->Show(true);

   canClose = true;
   mIsOrbitChanged = true;
}

//------------------------------------------------------------------------------
// void DisplayMolniya()
//------------------------------------------------------------------------------
/**
 * Displays the Molniya data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayMolniya()
{
   //show inputs

   //hide unused
   paramOneCheckBox->Show(false);
   paramTwoCheckBox->Show(false);
   paramThreeCheckBox->Show(false);
   paramFourCheckBox->Show(false);
   paramFiveCheckBox->Show(false);
   paramSixCheckBox->Show(false);
   paramSevenCheckBox->Show(false);
   param1TextCtrl->Show(false);
   param2TextCtrl->Show(false);
   param3TextCtrl->Show(false);
   param4TextCtrl->Show(false);
   param5TextCtrl->Show(false);
   param6TextCtrl->Show(false);
   param7TextCtrl->Show(false);
   units1StaticText->Show(false);
   units2StaticText->Show(false);
   units3StaticText->Show(false);
   units4StaticText->Show(false);
   units5StaticText->Show(false);
   units6StaticText->Show(false);
   units7StaticText->Show(false);

   //show outputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxT("26554"));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT("km"));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Altitude"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxString(GmatStringUtil::ToString(26554 - 
                                     GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]).c_str()));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("ECC"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT("0.72"));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT(""));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("INC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT("63.4"));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("deg"));
   unitsOut4StaticText->Show(true);

   paramOut5StaticText->SetLabel(wxT("RP"));
   paramOut5StaticText->Show(true);
   output5TextCtrl->SetValue(wxString(GmatStringUtil::ToString(26554*(1+.72)).c_str()));
   output5TextCtrl->Show(true);
   unitsOut5StaticText->SetLabel(wxT("km"));
   unitsOut5StaticText->Show(true);

   paramOut6StaticText->SetLabel(wxT("RA"));
   paramOut6StaticText->Show(true);
   output6TextCtrl->SetValue(wxString(GmatStringUtil::ToString(26554*(1-.72)).c_str()));
   output6TextCtrl->Show(true);
   unitsOut6StaticText->SetLabel(wxT("km"));
   unitsOut6StaticText->Show(true);

   paramOut7StaticText->SetLabel("P");
   paramOut7StaticText->Show(true);
   output7TextCtrl->SetValue(wxString(GmatStringUtil::ToString(26554*(1-pow(.72,2))).c_str()));
   output7TextCtrl->Show(true);
   unitsOut7StaticText->SetLabel(wxT("km"));
   unitsOut7StaticText->Show(true);

   paramOut8StaticText->SetLabel(wxT("AOP"));
   paramOut8StaticText->Show(true);
   output8TextCtrl->SetValue(wxT("-90"));
   output8TextCtrl->Show(true);
   unitsOut8StaticText->SetLabel(wxT("deg"));
   unitsOut8StaticText->Show(true);

   //hide unused
   paramOut9StaticText->Show(false);
   output9TextCtrl->Show(false);
   unitsOut9StaticText->Show(false);

   canClose = true;
   mIsOrbitChanged = true;
}

//------------------------------------------------------------------------------
// void DisplayFrozen()
//------------------------------------------------------------------------------
/**
 * Displays the Frozen data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayFrozen()
{
   //show inputs
   paramOneCheckBox->Show(true);
   paramOneCheckBox->Enable();
   paramOneCheckBox->SetLabel(wxT("Mean ALT"));
   paramOneCheckBox->SetValue(true);
   paramOneVal = true;
   param1TextCtrl->Show(true);
   param1TextCtrl->Enable();
   param1TextCtrl->SetValue(wxT("790"));
   param1TextCtrl->SetToolTip("0<ALT");
   std::string in1 = param1TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in1, input1Val);
   units1StaticText->Show(true);
   units1StaticText->SetLabel(wxT("km"));
   units1StaticText->Enable();

   paramTwoCheckBox->Show(true);
   paramTwoCheckBox->Enable();
   paramTwoCheckBox->SetLabel(wxT("INC"));
   paramTwoCheckBox->SetValue(true);
   paramTwoVal = true;
   param2TextCtrl->Show(true);
   param2TextCtrl->Enable();
   param2TextCtrl->SetValue(wxT("98.6"));
   param2TextCtrl->SetToolTip("0<=INC<180");
   std::string in2 = param2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(in2, input2Val);
   units2StaticText->Show(true);
   units2StaticText->SetLabel(wxT("deg"));
   units2StaticText->Enable();

   //hide unused
   paramThreeCheckBox->Show(false);
   paramFourCheckBox->Show(false);
   paramFiveCheckBox->Show(false);
   paramSixCheckBox->Show(false);
   paramSevenCheckBox->Show(false);
   param3TextCtrl->Show(false);
   param4TextCtrl->Show(false);
   param5TextCtrl->Show(false);
   param6TextCtrl->Show(false);
   param7TextCtrl->Show(false);
   units3StaticText->Show(false);
   units4StaticText->Show(false);
   units5StaticText->Show(false);
   units6StaticText->Show(false);
   units7StaticText->Show(false);

   //show outputs
   paramOut1StaticText->SetLabel(wxT("SMA"));
   paramOut1StaticText->Show(true);
   output1TextCtrl->SetValue(wxT(""));
   output1TextCtrl->Show(true);
   unitsOut1StaticText->SetLabel(wxT(""));
   unitsOut1StaticText->Show(true);

   paramOut2StaticText->SetLabel(wxT("Apogee Alt"));
   paramOut2StaticText->Show(true);
   output2TextCtrl->SetValue(wxT(""));
   output2TextCtrl->Show(true);
   unitsOut2StaticText->SetLabel(wxT("km"));
   unitsOut2StaticText->Show(true);

   paramOut3StaticText->SetLabel(wxT("Perigee Alt"));
   paramOut3StaticText->Show(true);
   output3TextCtrl->SetValue(wxT(""));
   output3TextCtrl->Show(true);
   unitsOut3StaticText->SetLabel(wxT("km"));
   unitsOut3StaticText->Show(true);

   paramOut4StaticText->SetLabel(wxT("ECC"));
   paramOut4StaticText->Show(true);
   output4TextCtrl->SetValue(wxT(""));
   output4TextCtrl->Show(true);
   unitsOut4StaticText->SetLabel(wxT("km"));
   unitsOut4StaticText->Show(true);

   //hide unused
   paramOut5StaticText->Show(false);
   paramOut6StaticText->Show(false);
   paramOut7StaticText->Show(false);
   paramOut8StaticText->Show(false);
   paramOut9StaticText->Show(false);
   output5TextCtrl->Show(false);
   output6TextCtrl->Show(false);
   output7TextCtrl->Show(false);
   output8TextCtrl->Show(false);
   output9TextCtrl->Show(false);
   unitsOut5StaticText->Show(false);
   unitsOut6StaticText->Show(false);
   unitsOut7StaticText->Show(false);
   unitsOut8StaticText->Show(false);
   unitsOut9StaticText->Show(false);
}

//------------------------------------------------------------------------------
// void DisplayTime()
//------------------------------------------------------------------------------
/**
 * Displays the Time data.
 */
//------------------------------------------------------------------------------
void OrbitDesignerDialog::DisplayTime()
{
   timeParamOneCheckBox->Show(true);
   timeParamOneCheckBox->Enable();
   timeParamOneCheckBox->SetLabel(wxT("Epoch"));
   timeParamOneCheckBox->SetValue(false);
   timeParamOneVal = false;
   time1TextCtrl->Show(true);
   time1TextCtrl->Disable();
   time1TextCtrl->SetValue(wxT("01 Jan 2000 11:59:28.000"));
   time1TextCtrl->SetToolTip("DD MM YYYY HH:MM:SS.s");
   timeIn1Val = time1TextCtrl->GetValue();
   timeUnits1StaticText->Show(true);
   timeUnits1StaticText->SetLabel(wxT(""));
   timeUnits1StaticText->Disable();

   timeParamTwoCheckBox->Show(true);
   timeParamTwoCheckBox->Disable();
   timeParamTwoCheckBox->SetLabel(wxT("RAAN"));
   timeParamTwoCheckBox->SetValue(false);
   timeParamTwoVal = false;
   time2TextCtrl->Show(true);
   time2TextCtrl->Disable();
   time2TextCtrl->SetValue(wxT("280.4609834242928100"));
   std::string tIn2 = time2TextCtrl->GetValue().WX_TO_STD_STRING;
   GmatStringUtil::ToReal(tIn2, timeIn2Val);
   time2TextCtrl->SetToolTip("0<=RAAN<360");
   timeUnits2StaticText->Show(true);
   timeUnits2StaticText->SetLabel(wxT("deg"));
   timeUnits2StaticText->Disable();

   timeParamThreeCheckBox->Show(true);
   timeParamThreeCheckBox->Disable();
   timeParamThreeCheckBox->SetLabel(wxT("Initial Local Sidereal Time"));
   timeParamThreeCheckBox->SetValue(false);
   timeParamThreeVal = false;
   time3TextCtrl->Show(true);
   time3TextCtrl->Disable();
   time3TextCtrl->SetValue(wxT("12:00:00.0"));
   time3TextCtrl->SetToolTip("ascending node");
   timeIn3Val = time3TextCtrl->GetValue();
   timeUnits3StaticText->Show(true);
   timeUnits3StaticText->SetLabel(wxT(""));
   timeUnits3StaticText->Disable();

   timeSizer->Show(true);

   timeParamOut1StaticText->SetLabel(wxT("Epoch"));
   timeParamOut1StaticText->Show(true);
   timeOut1TextCtrl->SetValue(wxT(""));
   timeOut1TextCtrl->Show(true);
   timeUnitsOut1StaticText->SetLabel(wxT(""));
   timeUnitsOut1StaticText->Show(true);

   timeParamOut2StaticText->SetLabel(wxT("RAAN"));
   timeParamOut2StaticText->Show(true);
   timeOut2TextCtrl->SetValue(wxT(""));
   timeOut2TextCtrl->Show(true);
   timeUnitsOut2StaticText->SetLabel(wxT("deg"));
   timeUnitsOut2StaticText->Show(true);

   timeParamOut3StaticText->SetLabel(wxT("Initial Local Time"));
   timeParamOut3StaticText->Show(true);
   timeOut3TextCtrl->SetValue(wxT(""));
   timeOut3TextCtrl->Show(true);
   timeUnitsOut3StaticText->SetLabel(wxT(""));
   timeUnitsOut3StaticText->Show(true);

   timeParamOut4StaticText->Show(false);
   timeOut4TextCtrl->Show(false);
   timeUnitsOut4StaticText->Show(false);
}

//------------------------------------------------------------------------------
// wxArrayString GetElementsString()
//------------------------------------------------------------------------------
/**
 * Returns the elements as a string.
 *
 * @return orbital elements, as a string
 */
//------------------------------------------------------------------------------
wxArrayString OrbitDesignerDialog::GetElementsString()
{
   wxString str;
   if (orbitType == "Sun Sync")
   {
      str.Printf("%.16f", orbitSS.GetSMA());
      elements.Add(str);   
      str.Printf("%.16f", orbitSS.GetECC());
      elements.Add(str);
      str.Printf("%.16f", orbitSS.GetINC());
      elements.Add(str);
      str.Printf("%.16f", orbitTime.GetRAAN());
      elements.Add(str);
      //aop
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10010));
      elements.Add(str);
      //ta
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10011));
      elements.Add(str);
   }
   else if (orbitType == "Repeat Sun Sync")
   {
          str.Printf("%.16f", orbitRSS.GetSMA());
      elements.Add(str);   
      str.Printf("%.16f", orbitRSS.GetECC());
      elements.Add(str);
      str.Printf("%.16f", orbitRSS.GetINC());
      elements.Add(str);
      str.Printf("%.16f", orbitTime.GetRAAN());
      elements.Add(str);
      //aop
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10010));
      elements.Add(str);
      //ta
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10011));
      elements.Add(str);
   }
   else if (orbitType == "Repeat Ground Track")
   {
          str.Printf("%.16f", orbitRGT.GetSMA());
      elements.Add(str);   
      str.Printf("%.16f", orbitRGT.GetECC());
      elements.Add(str);
      str.Printf("%.16f", orbitRGT.GetINC());
      elements.Add(str);                
      str.Printf("%.16f", orbitTime.GetRAAN());
      elements.Add(str);
      //aop
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10010));
      elements.Add(str);
      //ta
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10011));
      elements.Add(str);
   }
   else if (orbitType == "Geostationary")
   {
      Real SMA = 
         pow(GmatSolarSystemDefaults::PLANET_MU[2]*pow(86164.09054/(2*GmatMathConstants::PI),2),1/3.0);
      elements.Add(GmatStringUtil::ToString(SMA).c_str());
      elements.Add("0");
      elements.Add("0");                
      elements.Add("0");
      elements.Add("0");
      elements.Add("0.6931030628395508");
   }
   else if (orbitType == "Molniya")
   {
      elements.Add("26554");   
      elements.Add("0.72");
      elements.Add("63.4");             
      str.Printf("%.16f", orbitTime.GetRAAN());
      elements.Add(str);
      elements.Add("270");
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10011));
      elements.Add(str);
   }
   else if (orbitType == "Frozen")
   {
          str.Printf("%.16f", orbitFZN.GetSMA());
      elements.Add(str);   
      str.Printf("%.16f", orbitFZN.GetECC());
      elements.Add(str);
      str.Printf("%.16f", orbitFZN.GetINC());
      elements.Add(str);                
      str.Printf("%.16f", orbitTime.GetRAAN());
      elements.Add(str);
      //aop
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10010));
      elements.Add(str);
      //ta
      str.Printf("%.16f", theSpacecraft->GetRealParameter(10011));
      elements.Add(str);
   }

   return elements;
}

//------------------------------------------------------------------------------
// Rvector6 GetElementsDouble()
//------------------------------------------------------------------------------
/**
 * Returns the elements as a Real array.
 *
 * @return orbital elements, as a real array.
 */
//------------------------------------------------------------------------------
Rvector6 OrbitDesignerDialog::GetElementsDouble()
{
   Rvector6 elementVector;
   if (orbitType == "Sun Sync")
   {
      elementVector = 
         Rvector6(orbitSS.GetSMA(), orbitSS.GetECC(), orbitSS.GetINC(), orbitTime.GetRAAN(),
                  theSpacecraft->GetRealParameter(10010), theSpacecraft->GetRealParameter(10011));
   }
   else if (orbitType == "Repeat Sun Sync")
   {
      elementVector = 
         Rvector6(orbitRSS.GetSMA(), orbitRSS.GetECC(), orbitRSS.GetINC(),
                  theSpacecraft->GetRealParameter(10009), theSpacecraft->GetRealParameter(10010), 
                  theSpacecraft->GetRealParameter(10011));
   }
   else if (orbitType == "Repeat Ground Track")
   {
      elementVector = 
         Rvector6(orbitRGT.GetSMA(), orbitRGT.GetECC(), orbitRGT.GetINC(), 
                  theSpacecraft->GetRealParameter(10009), theSpacecraft->GetRealParameter(10010), 
                  theSpacecraft->GetRealParameter(10011));
   }
   else if (orbitType == "Geostationary")
   {
          Real SMA = 
         pow(GmatSolarSystemDefaults::PLANET_MU[2]*pow(86164.09054/(2*GmatMathConstants::PI),2),1/3.0);
          elementVector = 
             Rvector6(SMA, 0, 0, 0, 0, 0.6931030628395508);
   }
   else if (orbitType == "Molniya")
   {
      elementVector = 
         Rvector6(26554, 0.72, 63.4, orbitTime.GetRAAN(), 270, 
                  theSpacecraft->GetRealParameter(10011));
   }
   else if (orbitType == "Frozen")
   {
      elementVector = 
         Rvector6(orbitFZN.GetSMA(), orbitFZN.GetECC(), orbitFZN.GetINC(), 
                  theSpacecraft->GetRealParameter(10009), theSpacecraft->GetRealParameter(10010), 
                  theSpacecraft->GetRealParameter(10011));
   }
   return elementVector;
}

//------------------------------------------------------------------------------
// std::string GetEpochFormat()
//------------------------------------------------------------------------------
/**
 * Returns the epoch format.
 *
 * @return epoch format
 */
//------------------------------------------------------------------------------
std::string OrbitDesignerDialog::GetEpochFormat()
{
   return orbitTime.GetEpochFormat();
}

//------------------------------------------------------------------------------
// std::string GetEpoch()
//------------------------------------------------------------------------------
/**
 * Returns the epoch as a string
 *
 * @return epoch
 */
//------------------------------------------------------------------------------
std::string OrbitDesignerDialog::GetEpoch()
{
   return orbitTime.GetEpoch();
}
