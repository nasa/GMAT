//$Id$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
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
// Author: Waka Waktola
// Created: 2003/08/29
//
/**
 * This class contains the Propagation configuration window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "CelesBodySelectDialog.hpp"
#include "DragInputsDialog.hpp"
#include "PropagationConfigPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"               // for GmatStringUtil::ToUpper()
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"
#include "bitmaps/saveobject.xpm"
#include "TimeSystemConverter.hpp"
#include "DateUtil.hpp"
#include "GmatDefaults.hpp"
#include "GmatGlobal.hpp"
#include "FileManager.hpp"
#include "EopFile.hpp"

#include "wx/platform.h"
#include <wx/config.h>
#include <wx/filename.h>


//#define DEBUG_PROP_PANEL_DISPLAY
//#define DEBUG_PROP_PANEL_SETUP
//#define DEBUG_PROP_PANEL_LOAD
//#define DEBUG_PROP_PANEL_SAVE
//#define DEBUG_PROP_PANEL_GRAV
//#define DEBUG_PROP_PANEL_ATMOS
//#define DEBUG_PROP_PANEL_ERROR
//#define DEBUG_PROP_INTEGRATOR
//#define DEBUG_PROP_PROPAGATOR
//#define DEBUG_PROP_PANEL_SAVE_SRP
//#define DEBUG_PROP_DRAG


// Hard coded drag enumeration replaced by dynamic version, so set NONE_DM here
#define NONE_DM 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PropagationConfigPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   EVT_BUTTON(ID_BUTTON_ADD_BODY, PropagationConfigPanel::OnAddBodyButton)
   EVT_BUTTON(ID_BUTTON_GRAV_SEARCH, PropagationConfigPanel::OnGravSearchButton)
   EVT_BUTTON(ID_BUTTON_GRAV_SAVE, PropagationConfigPanel::OnGravSaveButton)
   EVT_BUTTON(ID_BUTTON_TIDE_SEARCH, PropagationConfigPanel::OnTideSearchButton)
   EVT_BUTTON(ID_BUTTON_SETUP, PropagationConfigPanel::OnSetupButton)
   EVT_BUTTON(ID_BUTTON_MAG_SEARCH, PropagationConfigPanel::OnMagSearchButton)
   EVT_BUTTON(ID_BUTTON_PM_EDIT, PropagationConfigPanel::OnPMEditButton)
   EVT_BUTTON(ID_BUTTON_SRP_EDIT, PropagationConfigPanel::OnSRPEditButton)
   EVT_TEXT(ID_TEXTCTRL_PROP, PropagationConfigPanel::OnIntegratorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_GRAV, PropagationConfigPanel::OnGravityTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_TIDE, PropagationConfigPanel::OnTideTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_MAGF, PropagationConfigPanel::OnMagneticTextUpdate)
   EVT_COMBOBOX(ID_CB_INTGR, PropagationConfigPanel::OnIntegratorComboBox)
   EVT_COMBOBOX(ID_CB_BODY, PropagationConfigPanel::OnPrimaryBodyComboBox)
   EVT_COMBOBOX(ID_CB_ORIGIN, PropagationConfigPanel::OnOriginComboBox)
   EVT_COMBOBOX(ID_CB_GRAV, PropagationConfigPanel::OnGravityModelComboBox)
   EVT_COMBOBOX(ID_CB_TIDE_DATA, PropagationConfigPanel::OnTideDataComboBox)
   EVT_COMBOBOX(ID_CB_TIDE, PropagationConfigPanel::OnTideModelComboBox)
   EVT_COMBOBOX(ID_CB_ATMOS, PropagationConfigPanel::OnAtmosphereModelComboBox)
   EVT_COMBOBOX(ID_CB_SRP_MODEL, PropagationConfigPanel::OnSRPModelComboBox)
   EVT_COMBOBOX(ID_CB_DRAG_MODEL, PropagationConfigPanel::OnDragModelComboBox)
   EVT_CHECKBOX(ID_SRP_CHECKBOX, PropagationConfigPanel::OnSRPCheckBoxChange)
   EVT_CHECKBOX(ID_REL_CORRECTION_CHECKBOX, PropagationConfigPanel::OnRelativisticCorrectionCheckBoxChange)
   EVT_CHECKBOX(ID_STOP_CHECKBOX, PropagationConfigPanel::OnStopCheckBoxChange)
   EVT_COMBOBOX(ID_CB_ERROR, PropagationConfigPanel::OnErrorControlComboBox)
   EVT_COMBOBOX(ID_CB_PROP_ORIGIN, PropagationConfigPanel::OnPropOriginComboBox)
   EVT_COMBOBOX(ID_CB_PROP_EPOCHFORMAT, PropagationConfigPanel::OnPropEpochComboBox)
   EVT_COMBOBOX(ID_CB_PROP_EPOCHSTART, PropagationConfigPanel::OnStartEpochComboBox)
   EVT_TEXT(ID_CB_PROP_EPOCHSTART, PropagationConfigPanel::OnStartEpochTextChange)
END_EVENT_TABLE()


//------------------------------------------------------------------------------
// PropagationConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagationConfigPanel::PropagationConfigPanel(wxWindow *parent,
                                               const wxString &propName)
   : GmatPanel(parent)
{
   mObjectName = propName.WX_TO_STD_STRING;
   propSetupName = propName.WX_TO_STD_STRING;
   theForceModelName = propSetupName + "_ForceModel";
   primaryBodyData = NULL;
   primaryBody = "None";

   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel() entered propSetupName='%s'\n",
       propSetupName.c_str());
   #endif

   Initialize();

   // Get configured SolarSystem and PropSetup
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();

   if (theSolarSystem == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot populate the panel, the SolarSystem is NULL");
      return;
   }

   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(propSetupName);
   if (SetObject(obj))
   {
      thePropSetup = (PropSetup*)obj;
      #ifdef DEBUG_PROP_PANEL_SETUP
      MessageInterface::ShowMessage("   thePropSetup=<%p>\n", thePropSetup);
      #endif
      Create();
      Show();
   }

   canClose = true;
   EnableUpdate(false);

}


//------------------------------------------------------------------------------
// ~PropagationConfigPanel()
//------------------------------------------------------------------------------
PropagationConfigPanel::~PropagationConfigPanel()
{
//   for (Integer i=0; i<(Integer)primaryBodyList.size(); i++)
//      delete primaryBodyList[i];
   if (primaryBodyData != NULL)
      delete primaryBodyData;
   for (Integer i=0; i<(Integer)pointMassBodyList.size(); i++)
      delete pointMassBodyList[i];

   // Unregister GUI components
   theGuiManager->UnregisterComboBox("CoordinateSystem", theOriginComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", theOriginComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", thePrimaryBodyComboBox);

   theGuiManager->UnregisterComboBox("CoordinateSystem", propCentralBodyComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", propCentralBodyComboBox);
}

//------------------------------------------
// Protected methods inherited from GmatPanel
//------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Create()
{
   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() entered\n");
   #endif

   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   Integer bsize = 2; // border size
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   wxBitmap saveBitmap = wxBitmap(saveobject_xpm);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Propagator"));

   //-----------------------------------------------------------------
   // Integrator
   //-----------------------------------------------------------------
   // Type
   wxStaticText *integratorStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Type",
                        wxDefaultPosition, wxDefaultSize);

   wxString *intgArray = new wxString[IntegratorCount];
   for (Integer i=0; i<IntegratorCount; i++)
      intgArray[i] = integratorArray[i];

   theIntegratorComboBox =
      new wxComboBox( this, ID_CB_INTGR, integratorString,
                      wxDefaultPosition, wxDefaultSize, IntegratorCount,
                      intgArray, wxCB_DROPDOWN|wxCB_READONLY );
   theIntegratorComboBox->SetToolTip(pConfig->Read(_T("IntegratorTypeHint")));

   // Initial Step Size
   initialStepSizeStaticText =
      new wxStaticText( this, ID_TEXT, "Initial " GUI_ACCEL_KEY "Step Size",
                        wxDefaultPosition, wxDefaultSize);

   initialStepSizeTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   initialStepSizeTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorInitialStepSizeHint")));

   unitsInitStepSizeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxDefaultSize );
   // Accuracy
   accuracyStaticText =
      new wxStaticText( this, ID_TEXT, "A" GUI_ACCEL_KEY "ccuracy",
                        wxDefaultPosition, wxDefaultSize );
   accuracyTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   accuracyTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorAccuracyHint")));

   // Minimum Step Size
   minStepStaticText =
      new wxStaticText( this, ID_TEXT, "Mi" GUI_ACCEL_KEY "n Step Size",
                        wxDefaultPosition, wxDefaultSize );
   minStepTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   minStepTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorMinStepSizeHint")));
   unitsMinStepStaticText =
      new wxStaticText( this, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxDefaultSize );

   // Maximum Step Size
   maxStepStaticText =
      new wxStaticText( this, ID_TEXT, "Ma" GUI_ACCEL_KEY "x Step Size",
                        wxDefaultPosition, wxDefaultSize );
   maxStepTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   maxStepTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorMaxStepSizeHint")));
   unitsMaxStepStaticText =
      new wxStaticText( this, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxDefaultSize );

   // Maximum Step Attempt
   maxStepAttemptStaticText =
      new wxStaticText( this, ID_TEXT, "Max Step " GUI_ACCEL_KEY "Attempts",
                        wxDefaultPosition, wxDefaultSize );
   maxStepAttemptTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   maxStepAttemptTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorMaxStepAttemptsHint")));

   // Minimum Integration Error
   minIntErrorStaticText =
      new wxStaticText( this, ID_TEXT, "Min " GUI_ACCEL_KEY "Integration Error",
                        wxDefaultPosition, wxDefaultSize );
   minIntErrorTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC)  );
   minIntErrorTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorMinIntegrationErrorHint")));

   // Nominal Integration Error
   nomIntErrorStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Nominal Integration Error",
                        wxDefaultPosition, wxDefaultSize );
   nomIntErrorTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   nomIntErrorTextCtrl->SetToolTip(pConfig->Read(_T("IntegratorNominalIntegrationErrorHint")));

   //-----------------------------------------------------------------
   // StopOnAccuracyViolation
   //-----------------------------------------------------------------
   theStopCheckBox =
      new wxCheckBox( this, ID_STOP_CHECKBOX, GUI_ACCEL_KEY"Stop If Accuracy Is Violated",
                      wxDefaultPosition, wxDefaultSize, 0 );
   theStopCheckBox->SetToolTip(pConfig->Read(_T("IntegratorStopOnAccuracyViolationHint")));

   //-----------------------------------------------------------------
   // SPK Settings
   // (todo: generalize to other propagators that are not integrators)
   //-----------------------------------------------------------------
   //StepSize = 60;
   propagatorStepSizeStaticText =
         new wxStaticText( this, ID_TEXT, wxT("Step Size"),
                           wxDefaultPosition, wxDefaultSize );
   propagatorStepSizeTextCtrl =
         new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                         wxDefaultPosition, wxSize(160,-1), 0,
                         wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unitsPropagatorStepSizeStaticText =
         new wxStaticText( this, ID_TEXT, wxT("sec"), wxDefaultPosition,
               wxDefaultSize );
   propCentralBodyStaticText =
         new wxStaticText( this, ID_TEXT, wxT("Central Body"),
                           wxDefaultPosition, wxDefaultSize );
   propCentralBodyComboBox=
         theGuiManager->GetCelestialBodyComboBox(this, ID_CB_PROP_ORIGIN, wxSize(100,-1));
   propagatorEpochFormatStaticText =
         new wxStaticText( this, ID_TEXT, wxT("Epoch Format"),
                           wxDefaultPosition, wxDefaultSize );
   wxArrayString emptyList;
   propagatorEpochFormatComboBox = new wxComboBox
      ( this, ID_CB_PROP_EPOCHFORMAT, wxT(""), wxDefaultPosition, wxSize(150,-1), //0,
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   propagatorEpochFormatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));
   StringArray reps = TimeSystemConverter::Instance()->GetValidTimeRepresentations();
   for (unsigned int i = 0; i < reps.size(); i++)
      propagatorEpochFormatComboBox->Append(reps[i].c_str());
   startEpochStaticText =
         new wxStaticText( this, ID_TEXT, wxT("Start Epoch"),
                           wxDefaultPosition, wxDefaultSize );

   // Kludge to check if the control indexing broke the test complete tests
   startEpochTextCtrl =
         new wxTextCtrl( this, ID_TEXTCTRL_PROP, wxT(""),
                         wxDefaultPosition, wxSize(160,-1), 0);
   startEpochTextCtrl->Hide();

   startEpochCombobox = new wxComboBox( this, ID_CB_PROP_EPOCHSTART, wxT(""),
         wxDefaultPosition, wxSize(160,-1), startEpochChoices, wxCB_DROPDOWN );
   startEpochCombobox->SetToolTip(pConfig->Read(_T("StartEpochHint")));

   intFlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   intFlexGridSizer->AddGrowableCol(1);
   intFlexGridSizer->Add( integratorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( theIntegratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( initialStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( initialStepSizeTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsInitStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( accuracyStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( accuracyTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minStepTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsMinStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsMaxStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepAttemptStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepAttemptTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minIntErrorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minIntErrorTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( nomIntErrorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( nomIntErrorTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   intFlexGridSizer->Add( propagatorStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( propagatorStepSizeTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsPropagatorStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( propCentralBodyStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( propCentralBodyComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( propagatorEpochFormatStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( propagatorEpochFormatComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( startEpochStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( startEpochCombobox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   // Kludge to check if the control indexing broke the test complete tests
   intFlexGridSizer->Add( startEpochTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   intFlexGridSizer->Add( 20, 0, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   intFlexGridSizer->Add( theStopCheckBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   // define minimum size for the integrator labels
   int minLabelSize;
   minLabelSize = integratorStaticText->GetBestSize().x;
   minLabelSize = (minLabelSize < initialStepSizeStaticText->GetBestSize().x) ? initialStepSizeStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < accuracyStaticText->GetBestSize().x) ? accuracyStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < minStepStaticText->GetBestSize().x) ? minStepStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < maxStepStaticText->GetBestSize().x) ? maxStepStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < maxStepAttemptStaticText->GetBestSize().x) ? maxStepAttemptStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < minIntErrorStaticText->GetBestSize().x) ? minIntErrorStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < nomIntErrorStaticText->GetBestSize().x) ? nomIntErrorStaticText->GetBestSize().x : minLabelSize;

   integratorStaticText->SetMinSize(wxSize(minLabelSize, integratorStaticText->GetMinHeight()));

   intStaticSizer =
      new GmatStaticBoxSizer( wxVERTICAL, this, "Integrator");
   intStaticSizer->Add( intFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   //-----------------------------------------------------------------
   // Force Model
   //-----------------------------------------------------------------
   // Error Control
   Integer w, h, ecWidth, cbWidth;
   wxString text = "Error Control";
   GetTextExtent(text, &w, &h);
   ecWidth = (Integer)(w * 1.2);
   text = "Central Body";
   GetTextExtent(text, &w, &h);
   cbWidth = (Integer)(w * 1.2);

   w = (ecWidth > cbWidth ? ecWidth : cbWidth);

   wxStaticText *errorCtrlStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Error Control",
                        wxDefaultPosition, wxSize(w,-1), wxST_NO_AUTORESIZE );

   theErrorComboBox =
      new wxComboBox( this, ID_CB_ERROR, errorControlArray[0],
                      wxDefaultPosition, wxSize(100,-1),
                      errorControlArray, wxCB_DROPDOWN|wxCB_READONLY );
   theErrorComboBox->SetToolTip(pConfig->Read(_T("ForceModelErrorControlHint")));

   wxFlexGridSizer *errorFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   errorFlexGridSizer->Add( errorCtrlStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   errorFlexGridSizer->Add( theErrorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);

   // Central Body
   wxStaticText *centralBodyStaticText =
      new wxStaticText( this, ID_TEXT, "Central " GUI_ACCEL_KEY "Body",
                        wxDefaultPosition, wxSize(w,-1), wxST_NO_AUTORESIZE);
   theOriginComboBox  =
      theGuiManager->GetCelestialBodyComboBox(this, ID_CB_ORIGIN, wxSize(100,-1));
   theOriginComboBox->SetToolTip(pConfig->Read(_T("ForceModelCentralBodyHint")));

   wxFlexGridSizer *centralBodySizer = new wxFlexGridSizer( 2, 0, 2 );
   centralBodySizer->Add( centralBodyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   centralBodySizer->Add( theOriginComboBox, 0, wxALIGN_LEFT|wxALL, bsize);

   // Primary Bodies -- for 1.0, changed to allow only one body; commented code
   // is a start on allowing multiple bodies
   GmatStaticBoxSizer *primaryStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Primary " GUI_ACCEL_KEY "Body");
//   GmatStaticBoxSizer *primaryStaticSizer =
//      new GmatStaticBoxSizer(wxVERTICAL, this, "Primary " GUI_ACCEL_KEY "Bodies");
   wxArrayString bodyArray;

   thePrimaryBodyComboBox = theGuiManager->GetCelestialBodyComboBox(this,
         ID_CB_BODY, wxSize(100, -1));

//   thePrimaryBodyComboBox =
//      new wxComboBox( this, ID_CB_BODY, wxT(primaryBodyString),
//                      wxDefaultPosition,  wxSize(100,-1),// 0,
//                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   thePrimaryBodyComboBox->SetToolTip(pConfig->Read(_T("ForceModelPrimaryBodiesComboHint")));

   // Not used; left in place for later use
//   bodyTextCtrl =
//      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
//                      wxDefaultPosition, wxSize(160,-1), wxTE_READONLY );
//   bodyTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelPrimaryBodiesEditHint")));
   // Not used; left in place for later use
//   wxButton *primaryBodySelectButton =
//      new wxButton( this, ID_BUTTON_ADD_BODY, wxT("Select"),
//                    wxDefaultPosition, wxDefaultSize, 0 );
//   primaryBodySelectButton->SetToolTip(pConfig->Read(_T("ForceModelPrimaryBodiesSelectHint")));

   wxBoxSizer *bodySizer = new wxBoxSizer( wxHORIZONTAL );
   bodySizer->Add( thePrimaryBodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
//   bodySizer->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
//   bodySizer->Add( primaryBodySelectButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

   // Gravity
   //----------- Gravity Line 1
   wxStaticText *type1StaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Model",
                        wxDefaultPosition, wxDefaultSize, 0 );

   wxArrayString gravArray;
   theGravModelComboBox =
      new wxComboBox( this, ID_CB_GRAV, wxT(""),
                      wxDefaultPosition, wxSize(70,-1), // 0,
                      gravArray, wxCB_DROPDOWN|wxCB_READONLY );
   theGravModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelGravityModelHint")));

   wxBoxSizer *gravModelSizer = new wxBoxSizer( wxHORIZONTAL );
   gravModelSizer->Add( type1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   gravModelSizer->Add( theGravModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   //----------- Gravity Line 2
   wxStaticText *degreeStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Degree",
                        wxDefaultPosition, wxDefaultSize, 0 );
   gravityDegreeTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   gravityDegreeTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityDegreeHint")));

   wxStaticText *gravityDegreeSlashStaticText =
      new wxStaticText( this, ID_TEXT, "/",
                        wxDefaultPosition, wxDefaultSize, 0 );

   gravityMaxDegreeTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), wxTE_READONLY );
   gravityMaxDegreeTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityMaxDegreeHint")));

   wxStaticText *orderStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Order",
                        wxDefaultPosition, wxDefaultSize, 0 );
   gravityOrderTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   gravityOrderTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityOrderHint")));

   wxStaticText *gravityOrderSlashStaticText =
      new wxStaticText( this, ID_TEXT, "/",
                        wxDefaultPosition, wxDefaultSize, 0 );

   gravityMaxOrderTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), wxTE_READONLY );
   gravityMaxOrderTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityMaxOrderHint")));

   wxStaticText *stmlimitStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"STM Limit",
                        wxDefaultPosition, wxDefaultSize, 0 );
   gravityStmLimitTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   gravityStmLimitTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityStmLimitHint")));

   wxBoxSizer *degOrdSizer = new wxBoxSizer( wxHORIZONTAL );
   degOrdSizer->Add( degreeStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityDegreeSlashStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityMaxDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( orderStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityOrderSlashStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityMaxOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( stmlimitStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityStmLimitTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   //----------- Gravity Line 3
   potFileStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Gravity File",
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(272,-1), 0 );
   potFileTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityPotentialFileHint")));

   theGravModelSearchButton =
      new wxBitmapButton(this, ID_BUTTON_GRAV_SEARCH, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   theGravModelSearchButton->SetToolTip(pConfig->Read(_T("ForceModelGravitySearchHint")));

   theGravModelSaveButton =
      new wxBitmapButton(this, ID_BUTTON_GRAV_SAVE, saveBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   theGravModelSaveButton->SetToolTip(pConfig->Read(_T("ForceModelGravitySaveHint")));

   wxBoxSizer *potFileSizer = new wxBoxSizer( wxHORIZONTAL );
   potFileSizer->Add( potFileStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   potFileSizer->Add( potFileTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   potFileSizer->Add( theGravModelSearchButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   potFileSizer->Add( theGravModelSaveButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   //----------- Tide Box
   GmatStaticBoxSizer *tideStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Tide");
   //----------- Tide Line 1
   tideDataStaticText =
       new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY"Data Source",
       wxDefaultPosition, wxDefaultSize, 0);
   wxArrayString tideDataArray;
   theTideDataComboBox =
      new wxComboBox( this, ID_CB_TIDE_DATA, wxT(""),
                      wxDefaultPosition, wxSize(70,-1), // 0,
                      tideDataArray, wxCB_DROPDOWN|wxCB_READONLY );
   theTideDataComboBox->SetToolTip(pConfig->Read(_T("ForceModelGravityTideDataSourceHint")));

   tideModelStaticText =
      new wxStaticText( this, ID_TEXT,"Model",
                        wxDefaultPosition, wxDefaultSize, 0 );
   wxArrayString tideArray;
   theTideModelComboBox =
      new wxComboBox( this, ID_CB_TIDE, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), // 0,
                      tideArray, wxCB_DROPDOWN|wxCB_READONLY );
   theTideModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelTideModelHint")));

   tideTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition,
                      wxSize(120,-1), wxTE_READONLY );
   tideTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelTideHint")));

   tideFileStaticText =
       new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY"Tide File",
       wxDefaultPosition, wxDefaultSize, 0);
   tideFileTextCtrl =
       new wxTextCtrl(this, ID_TEXTCTRL_TIDE, wxT(""), wxDefaultPosition,
       wxSize(286, -1), 0);
   tideFileTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelGravityTideFileHint")));

   theTideModelSearchButton =
       new wxBitmapButton(this, ID_BUTTON_TIDE_SEARCH, openBitmap, wxDefaultPosition,
       wxSize(buttonWidth, -1));
   theTideModelSearchButton->SetToolTip(pConfig->Read(_T("ForceModelTideSearchHint")));

   wxBoxSizer *tideDataSizer = new wxBoxSizer(wxHORIZONTAL);
   tideDataSizer->Add (tideDataStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideDataSizer->Add (theTideDataComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideDataSizer->Add (tideModelStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideDataSizer->Add (theTideModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideDataSizer->Add (tideTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   //----------- Tide Line 2
   wxBoxSizer *tideFileSizer = new wxBoxSizer(wxHORIZONTAL);
   tideFileSizer->Add (tideFileStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideFileSizer->Add (tideFileTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   tideFileSizer->Add (theTideModelSearchButton, 0, wxALIGN_CENTRE | wxALL, bsize);
   //----------- Gravity Box
   GmatStaticBoxSizer *gravStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Gravity");
   gravStaticSizer->Add (gravModelSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   gravStaticSizer->Add (degOrdSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   gravStaticSizer->Add (potFileSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   tideStaticSizer->Add (tideDataSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   tideStaticSizer->Add (tideFileSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   gravStaticSizer->Add (tideStaticSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   //-----------

   // Drag
   wxStaticText *type2StaticText =
      new wxStaticText( this, ID_TEXT, "Atmosphere " GUI_ACCEL_KEY "Model",
                        wxDefaultPosition, wxDefaultSize, 0 );

   theAtmosModelComboBox =
      new wxComboBox( this, ID_CB_ATMOS, dragModelArray[0],
                      wxDefaultPosition, wxDefaultSize,
                      dragModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   theAtmosModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelDragAtmosphereModelHint")));
   theDragSetupButton =
      new wxButton( this, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   theDragSetupButton->SetToolTip(pConfig->Read(_T("ForceModelDragSetupHint")));

   dragModelStaticText = new wxStaticText( this, ID_TEXT,
                                          GUI_ACCEL_KEY"Drag Model",
                                          wxDefaultPosition, wxDefaultSize, 0 );
   theDragModelComboBox =
      new wxComboBox( this, ID_CB_DRAG_MODEL, dragModelListArray[0],
                     wxDefaultPosition, wxSize(100,-1),
                     dragModelListArray, wxCB_DROPDOWN|wxCB_READONLY );
   theDragModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelDragModelHint")));
   

   wxFlexGridSizer *atmosSizer = new wxFlexGridSizer( 3, 0, 2 );
//   wxBoxSizer *atmosSizer = new wxBoxSizer( wxHORIZONTAL );
   atmosSizer->Add( type2StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   atmosSizer->Add( theAtmosModelComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   atmosSizer->Add( theDragSetupButton, 0, wxALIGN_LEFT|wxALL, bsize);
   atmosSizer->Add( dragModelStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   atmosSizer->Add( theDragModelComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   atmosSizer->Add( 20,20,0, wxGROW|wxALIGN_LEFT|wxALL, 5);
   
   GmatStaticBoxSizer *atmosStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drag");
   atmosStaticSizer->Add( atmosSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   // Magnetic Field
   wxStaticText *type3StaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Model",
                        wxDefaultPosition, wxDefaultSize, 0 );

   theMagfModelComboBox =
      new wxComboBox( this, ID_CB_MAG, magfModelArray[0],
                      wxDefaultPosition, wxDefaultSize,
                      magfModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   theMagfModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelMagneticFieldModelHint")));

   wxStaticText *degree2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   magneticDegreeTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   magneticDegreeTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelMagneticDegreeHint")));
   wxStaticText *order2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   magneticOrderTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   magneticOrderTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelMagneticOrderHint")));
   theMagModelSearchButton =
      new wxButton( this, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   theMagModelSearchButton->SetToolTip(pConfig->Read(_T("ForceModelMagneticSearchHint")));

   wxBoxSizer *magfSizer = new wxBoxSizer( wxHORIZONTAL );
   magfSizer->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( theMagfModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( theMagModelSearchButton, 0, wxALIGN_CENTRE|wxALL, bsize);

   magfStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Magnetic Field");
   magfStaticSizer->Add( magfSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   //-----------------------------------------------------------------
   // Point Masses
   //-----------------------------------------------------------------
   wxStaticText *pointMassStaticText =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Point Masses",
                        wxDefaultPosition, wxDefaultSize, 0 );
   pmEditTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition,
                      wxSize(235,-1), wxTE_READONLY );
   pmEditTextCtrl->SetToolTip(pConfig->Read(_T("ForceModelPointMassesHint")));
   wxButton *editPmfButton =
      new wxButton( this, ID_BUTTON_PM_EDIT, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editPmfButton->SetToolTip(pConfig->Read(_T("ForceModelSelectPointMassesHint")));

   wxFlexGridSizer *pointMassSizer = new wxFlexGridSizer( 3, 0, 2 );
   pointMassSizer->Add( pointMassStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   pointMassSizer->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   pointMassSizer->Add( editPmfButton, 0, wxALIGN_CENTRE|wxALL, bsize);

   //-----------------------------------------------------------------
   // SRP
   //-----------------------------------------------------------------
   theSrpCheckBox =
      new wxCheckBox( this, ID_SRP_CHECKBOX, GUI_ACCEL_KEY"Use Solar Radiation Pressure",
                      wxDefaultPosition, wxDefaultSize, 0 );
   theSrpCheckBox->SetToolTip(pConfig->Read(_T("ForceModelUseSolarRadiationPressureHint")));

   srpModelStaticText = new wxStaticText( this, ID_TEXT,
                        GUI_ACCEL_KEY"SRP Model",
                        wxDefaultPosition, wxDefaultSize, 0 );
   theSRPModelComboBox =
      new wxComboBox( this, ID_CB_SRP_MODEL, srpModelArray[0],
                      wxDefaultPosition, wxSize(100,-1),
                      srpModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   theSRPModelComboBox->SetToolTip(pConfig->Read(_T("ForceModelSRPModelHint")));

   wxFlexGridSizer *theSRPSizer = new wxFlexGridSizer( 2, 0, 2 );
   theSRPSizer->Add( srpModelStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   theSRPSizer->Add( theSRPModelComboBox, 0, wxALIGN_LEFT|wxALL, bsize);

   //-----------------------------------------------------------------
   // Relativistic Correction
   //-----------------------------------------------------------------
   theRelativisticCorrectionCheckBox =
      new wxCheckBox( this, ID_REL_CORRECTION_CHECKBOX, GUI_ACCEL_KEY"Relativistic Correction",
                      wxDefaultPosition, wxDefaultSize, 0 );
   theRelativisticCorrectionCheckBox->SetToolTip(pConfig->Read(_T("ForceModelUseRelativisticCorrectionHint")));

   //-----------------------------------------------------------------
   // put SRP and RC stuff together
   //-----------------------------------------------------------------
   srpRCFlexSizer = new wxFlexGridSizer( 2, 0, 0 );
   srpRCFlexSizer->AddGrowableCol(1);
   srpRCFlexSizer->Add( theSrpCheckBox, 0, wxALIGN_LEFT|wxALL, 5 );
   srpRCFlexSizer->Add( theSRPSizer, 0, wxALIGN_LEFT|wxALL, 5 );
   srpRCFlexSizer->Add( theRelativisticCorrectionCheckBox, 0, wxALIGN_LEFT|wxALL, 5 );
   srpRCFlexSizer->Add( 80, 10, 0, wxGROW|wxALIGN_LEFT|wxALL, 5);

   //-----------------------------------------------------------------
   // Primary Bodies
   //-----------------------------------------------------------------
   primaryStaticSizer->Add( bodySizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( gravStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( atmosStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( magfStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Hide( magfStaticSizer, true ); // hide, not using magnetic right now, TGG, 04/09/10

   //-----------------------------------------------------------------
   // Force Model
   //-----------------------------------------------------------------
   fmStaticSizer =
         new GmatStaticBoxSizer(wxVERTICAL, this, "Force Model");
   fmStaticSizer->Add( errorFlexGridSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   fmStaticSizer->Add( centralBodySizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   fmStaticSizer->Add( primaryStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   fmStaticSizer->Add( pointMassSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   fmStaticSizer->Add( srpRCFlexSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
//   fmStaticSizer->Add( theSrpCheckBox, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
//   fmStaticSizer->Add( theRelativisticCorrectionCheckBox, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);

   //-----------------------------------------------------------------
   // Add panelSizer
   //-----------------------------------------------------------------
   wxBoxSizer *intBoxSizer = new wxBoxSizer( wxVERTICAL );
   intBoxSizer->Add( intStaticSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   intBoxSizer->Add( 0, 0, 1);

   leftBoxSizer = new wxBoxSizer( wxVERTICAL );
   leftBoxSizer->Add( intBoxSizer, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);

   wxBoxSizer *pageSizer = new wxBoxSizer( wxHORIZONTAL );
   pageSizer->Add( leftBoxSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   pageSizer->Add( fmStaticSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);

   wxBoxSizer *panelSizer = new wxBoxSizer( wxVERTICAL );
   panelSizer->Add( pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   theMiddleSizer->Add(panelSizer, 0, wxGROW, bsize);

   if ((thePropagator == NULL) || (thePropagator->IsOfType("Integrator")))
   {
      ShowIntegratorLayout(true);
   }
   else
   {
      ShowIntegratorLayout(false);
   }

   //-----------------------------------------------------------------
   // disable components for future implementation
   //-----------------------------------------------------------------
   theMagfModelComboBox->Enable(false);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   theMagModelSearchButton->Enable(false);
   type3StaticText->Enable(false);
   degree2StaticText->Enable(false);
   order2StaticText->Enable(false);

   //-----------------------------------------------------------------
   // initially disable components
   //-----------------------------------------------------------------
   theDragSetupButton->Enable(false);
   theDragModelComboBox->Enable(false);

   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::LoadData()
{
   // Enable the "Show Script" button
   mObject = thePropSetup;
   dragStringBuffer.resize(6);

   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("PropConfigPanel::LoadData() entered\n");
   #endif

   thePropagator = thePropSetup->GetPropagator();
   if (thePropagator->UsesODEModel())
   {
      theForceModel = thePropSetup->GetODEModel();
      theForceModelName = theForceModel->GetName();
      numOfForces   = thePropSetup->GetNumForces();
   }
   else
   {
      theForceModel = NULL;
      // Load the name in case the user changes to an integrator
      theForceModelName = thePropSetup->GetName() + "_ForceModel";
      numOfForces   = 0;

      // Set the data buffers used in the dialog
      Real ss     = thePropagator->GetRealParameter("StepSize");
      spkStep     = ToString(ss);
      spkBody     = thePropagator->GetStringParameter("CentralBody").c_str();
      spkEpFormat = thePropagator->GetStringParameter("EpochFormat").c_str();
      spkEpoch    = thePropagator->GetStringParameter("StartEpoch").c_str();

      // Set the flags so that dialog populates correctly
      isSpkStepChanged     = true;
      isSpkBodyChanged     = true;
      isSpkEpFormatChanged = true;
      isSpkEpochChanged    = true;

      try
      {
         StringArray choices =
               thePropagator->GetStringArrayParameter("StartOptions");
         startEpochChoices.Clear();
         for (UnsignedInt i = 0; i < choices.size(); ++i)
            startEpochChoices.Add(choices[i].c_str());

         startEpochCombobox->Clear();
         startEpochCombobox->Append(startEpochChoices);
      }
      catch (BaseException &)  // Ignore exceptions here
      {
      }
   }

   PopulateForces();

   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   Getting Primary bodies array.\n");
   #endif

//   if (!primaryBodiesArray.IsEmpty())
   if (primaryBody != "None")
   {
//      primaryBodyString = primaryBodiesArray.Item(0).c_str();
      primaryBodyString = primaryBody;
      currentBodyName = primaryBodyString;
      currentBodyId = FindPrimaryBody(currentBodyName);
   }

   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   primaryBodyString=%s\n", primaryBodyString.WX_TO_C_STRING);
   #endif

//   numOfBodies = (Integer)primaryBodiesArray.GetCount();

   std::string propType = thePropagator->GetTypeName();

   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   propType=%s\n", propType.c_str());
   #endif

   Integer typeId = 0;
   for (size_t i = 0; i < integratorArray.GetCount(); ++i)
      if (integratorArray[i] == propType.c_str())
         typeId = i;

   // Display primary bodies
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   Updating primary body combo box\n");
   #endif
   UpdatePrimaryBodyComboBoxList();
//   if ( !primaryBodiesArray.IsEmpty() )
//      thePrimaryBodyComboBox->SetValue(primaryBodyList[0]->bodyName.c_str());
   thePrimaryBodyComboBox->SetValue(primaryBody);
//      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//         thePrimaryBodyComboBox->Append(primaryBodiesArray[i]);

   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   Setting integrator combo box\n");
   #endif
   theIntegratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId];

   // Display the force model origin (central body)
   theOriginComboBox->SetValue(propOriginName);

   DisplayIntegratorData(false);
   DisplayForceData();

//   Integer count = (Integer)primaryBodyList.size();
   Integer count = (primaryBodyData == NULL ? 0 : 1);

   if (count == 0)
      EnablePrimaryBodyItems(false, false);
   else
      EnablePrimaryBodyItems(true);

   if ((thePropagator == NULL) || (thePropagator->IsOfType("Integrator")))
   {
      ShowIntegratorLayout(true);
   }
   else
   {
      ShowIntegratorLayout(false);
   }


   #ifdef DEBUG_PROP_PANEL_LOAD
   ShowPropData("LoadData() PropData on exit");
   if (thePropagator->IsOfType("Integrator"))
      ShowForceList("LoadData() ForceList on exit");
   #endif

}

//------------------------------------------------------------------------------
// void PropagationConfigPanel::PopulateForces()
//------------------------------------------------------------------------------
void PropagationConfigPanel::PopulateForces()
{
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("PropagationConfigPanel::PopulateForces() entered\n");
   #endif
   
   try
   {
      if (theForceModel != NULL)
      {
         mFmPrefaceComment = theForceModel->GetCommentLine();
         propOriginName = theForceModel->GetStringParameter("CentralBody").c_str();
         errorControlTypeName = theForceModel->GetStringParameter("ErrorControl").c_str();

         PhysicalModel *force;
         Integer paramId;
         wxString wxBodyName;
         wxString wxForceType;
         wxString tempStr;
         wxString useSRP;
         wxString useRC;

         paramId = theForceModel->GetParameterID("SRP");
         useSRP = theForceModel->GetOnOffParameter(paramId).c_str();
         usePropOriginForSrp = (useSRP == "On") ? true : false;

         paramId = theForceModel->GetParameterID("RelativisticCorrection");
         useRC = theForceModel->GetOnOffParameter(paramId).c_str();
         addRelativisticCorrection = (useRC == "On") ? true : false;

         for (Integer i = 0; i < numOfForces; i++)
         {
            force = theForceModel->GetForce(i);
            if (force == NULL)
            {
               MessageInterface::ShowMessage
                  ("**** INTERNAL ERROR *** PropagationConfigPanel::LoadData() "
                   "Cannot continue, force pointer is NULL for index %d\n", i);
               return;
            }

            wxForceType = force->GetTypeName().c_str();
            wxBodyName = force->GetStringParameter("BodyName").c_str();

            #ifdef DEBUG_PROP_PANEL_LOAD
            MessageInterface::ShowMessage
               ("   Getting %s for body %s\n", wxForceType.WX_TO_C_STRING, wxBodyName.WX_TO_C_STRING);
            #endif

            if (wxForceType == "PointMassForce")
            {
               thePMF = (PointMassForce *)force;
               secondaryBodiesArray.Add(wxBodyName);
               pointMassBodyList.push_back(new ForceType(wxBodyName, "None","None","None",
                                                         dragModelArray[NONE_DM],
                                                         magfModelArray[NONE_MM],
                                                         thePMF));

               //Warn user about bodies already added as Primary body
//               Integer fmSize = (Integer)primaryBodyList.size();
               Integer fmSize = (primaryBodyData == NULL ? 0 : 1);
               Integer last = (Integer)pointMassBodyList.size() - 1;

               for (Integer i = 0; i < fmSize; i++)
               {
                  if (strcmp(pointMassBodyList[last]->bodyName.c_str(),
//                        primaryBodyList[i]->bodyName.c_str()) == 0)
                        primaryBodyData->bodyName.c_str()) == 0)
                  {
                     MessageInterface::PopupMessage
                        (Gmat::WARNING_,
                         "Cannot set %s both as Primary body and Point Mass",
                         pointMassBodyList[last]->bodyName.WX_TO_C_STRING);
                  }
               }
            }
            else if (wxForceType == "GravityField")
            {
               theGravForce = (GravityField*)force;
               wxString potFilename =
                  theGravForce->GetStringParameter("PotentialFile").c_str();
               wxString tideFilename =
                  theGravForce->GetStringParameter("TideFile").c_str();
               
               #ifdef DEBUG_PROP_PANEL_GRAV
               MessageInterface::ShowMessage("   potFilename = '%s'\n", potFilename.WX_TO_C_STRING);
               MessageInterface::ShowMessage("   tideFilename = '%s'\n", tideFilename.WX_TO_C_STRING);
               #endif
               
//               currentBodyId = FindPrimaryBody(wxBodyName);
//               primaryBodyList[currentBodyId]->bodyName = wxBodyName;
//               primaryBodyList[currentBodyId]->potFile = potFilename;
               if (primaryBodyData == NULL)
               {
                  #ifdef DEBUG_PROP_PANEL_GRAV
                     MessageInterface::ShowMessage("Creating primaryBodyData for "
                           "%s\n", wxBodyName.WX_TO_C_STRING);
                  #endif
                  primaryBodyData = new ForceType(wxBodyName);
               }
               
               primaryBody = wxBodyName;

               primaryBodyData->bodyName = wxBodyName;
               primaryBodyData->potFile = potFilename;
               primaryBodyData->potFileFullPath =
                  theGravForce->GetStringParameter("PotentialFileFullPath").c_str();
               primaryBodyData->tideFile = tideFilename;
               primaryBodyData->tideFileFullPath =
                  theGravForce->GetStringParameter("TideFileFullPath").c_str();
               
               #ifdef DEBUG_PROP_PANEL_GRAV
               MessageInterface::ShowMessage
                  ("   Getting gravity model type for %s, potFilename=%s\n   tideFilename=%s\n",
                   wxBodyName.WX_TO_C_STRING, potFilename.WX_TO_C_STRING, tideFilename.WX_TO_C_STRING);
               #endif
               
               // Use full path potential file from the base as per new file path design (LOJ: 2014.06.30)
               
               //==========================================================
               #if 0
               std::ifstream inStream(potFilename.c_str());
               if (!inStream)
               {
                  std::string pathType = wxBodyName.c_str();
                  pathType += "_POT_PATH";
                  #ifdef DEBUG_PROP_PANEL_GRAV
                     MessageInterface::ShowMessage("   pathType =  \"%s\"\n", pathType.WX_TO_C_STRING);
                  #endif
                  FileManager *fm = FileManager::Instance();
                  std::string gravPath = fm->GetPathname(pathType);
                  //primaryBodyList[currentBodyId]->potFile = (gravPath + potFilename.c_str()).c_str();
                  primaryBodyData->potFile = (gravPath + potFilename.c_str()).c_str();
                  #ifdef DEBUG_PROP_PANEL_GRAV
                     MessageInterface::ShowMessage("   Could not open %s, so full path = %s\n",
                                                   potFilename.c_str(), (primaryBodyData->potFile).c_str());
                  #endif
               }
               else
               {
                  #ifdef DEBUG_PROP_PANEL_GRAV
                     MessageInterface::ShowMessage("   Successfully opened %s\n", potFilename.WX_TO_C_STRING);
                  #endif
                  inStream.close();
               }
               #endif
               //==========================================================
               
               GravityField::GravityModelType bodyGravModelType =
//               GravityFileUtil::GetModelType((primaryBodyList[currentBodyId]->potFile).c_str(), wxBodyName.c_str());
                  // GravityFileUtil::GetModelType((primaryBodyData->potFile).c_str(), wxBodyName.c_str());
                  GravityField::GetModelType((primaryBodyData->potFileFullPath).c_str(), wxBodyName.c_str());
               #ifdef DEBUG_PROP_PANEL_GRAV
               MessageInterface::ShowMessage("   bodyGravModelType = %d\n", bodyGravModelType);
               #endif
               primaryBodyData->gravType = wxString((GravityField::GRAVITY_MODEL_NAMES[bodyGravModelType]).c_str());
               
               #ifdef DEBUG_PROP_PANEL_GRAV
               MessageInterface::ShowMessage("   Getting the gravity force\n");
               #endif

//               primaryBodyList[currentBodyId]->gravf = theGravForce;
               primaryBodyData->gravf = theGravForce;


               // Warn user about bodies already added as Primary body
               Integer pmSize = (Integer)pointMassBodyList.size();
//               Integer last = (Integer)primaryBodyList.size() - 1;

               for (Integer i = 0; i < pmSize; i++)
               {
//                  if (strcmp(primaryBodyList[last]->bodyName.c_str(),
//                             pointMassBodyList[i]->bodyName.c_str()) == 0)
                  if (strcmp(primaryBodyData->bodyName.c_str(),
                             pointMassBodyList[i]->bodyName.c_str()) == 0)
                  {
                     MessageInterface::PopupMessage
                        (Gmat::WARNING_, "Cannot set %s both as Primary body "
                         "and Point Mass",
//                         pointMassBodyList[last]->bodyName.c_str());
                         primaryBodyData->bodyName.WX_TO_C_STRING);
                  }
               }

//               if (primaryBodyList[currentBodyId]->potFile == "")
               if (primaryBodyData->potFile == "")
               {
                  MessageInterface::PopupMessage
                     (Gmat::WARNING_, "Cannot find Potential File for %s.\n",
                      wxBodyName.WX_TO_C_STRING);
               }

               tempStr = "";
               tempStr << theGravForce->GetIntegerParameter("Degree");
//               primaryBodyList[currentBodyId]->gravDegree = tempStr;
               primaryBodyData->gravDegree = tempStr;

               tempStr = "";
               tempStr << theGravForce->GetIntegerParameter("Order");
//               primaryBodyList[currentBodyId]->gravOrder = tempStr;
               primaryBodyData->gravOrrder = tempStr;

               tempStr = "";
               tempStr << theGravForce->GetIntegerParameter("StmLimit");
//               primaryBodyList[currentBodyId]->gravOrder = tempStr;
               primaryBodyData->gravStmLimit = tempStr;

               tempStr = "";
               tempStr << theGravForce->GetStringParameter("TideModel");
//               primaryBodyList[currentBodyId]->gravOrder = tempStr;
               primaryBodyData->tideModel = tempStr;

               bool haveTideModel = primaryBodyData->tideModel != HarmonicGravity::ETideString[0];
               bool haveTideFile = primaryBodyData->tideFile != "";
               bool haveGrvFile = GmatStringUtil::ToLower(wxFileName(primaryBodyData->potFile).GetExt().WX_TO_STD_STRING) == "grv";

               if (haveTideFile)
               {
                  primaryBodyData->tideData = "Tide File";
               }
               else
               {
                  if (haveTideModel && (primaryBody == "Earth" || haveGrvFile))
                     primaryBodyData->tideData = "Inherited";
                  else
                     primaryBodyData->tideData = "None";
               }

//               bool found = false;
//               for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//               {
//                  if ( primaryBodiesArray[i].CmpNoCase(wxBodyName) == 0 )
//                     found = true;
//               }
//
//               if (!found)
//                  primaryBodiesArray.Add(wxBodyName);
            }
            else if (wxForceType == "DragForce")
            {
               theDragForce = (DragForce*)force;
               paramId = theDragForce->GetParameterID("AtmosphereModel");
               atmosModelString =
                     theDragForce->GetStringParameter(paramId).c_str();
               std::string theDragModel = theDragForce->GetStringParameter("DragModel");
               dragModelName = theDragModel.c_str();

               currentBodyId = FindPrimaryBody(wxBodyName);
//               primaryBodyList[currentBodyId]->bodyName = wxBodyName;
//               primaryBodyList[currentBodyId]->dragType = atmosModelString;
//               primaryBodyList[currentBodyId]->dragf = theDragForce;
               primaryBodyData->bodyName = wxBodyName;
               primaryBodyData->dragType = atmosModelString;
               primaryBodyData->dragf = theDragForce;
                    
               // get the info for the drag panel
               dragParameterBuffer[0] = theDragForce->GetRealParameter("F107");
               dragParameterBuffer[1] = theDragForce->GetRealParameter("F107A");
               dragParameterBuffer[2] = theDragForce->GetRealParameter("MagneticIndex");
               dragStringBuffer[0] = theDragForce->GetStringParameter("PredictedWeatherSource").c_str();
               dragStringBuffer[1] = theDragForce->GetStringParameter("HistoricWeatherSource").c_str();
               dragStringBuffer[2]= theDragForce->GetStringParameter("CSSISpaceWeatherFile").c_str();
               dragStringBuffer[3] = theDragForce->GetStringParameter("SchattenFile").c_str();
               dragStringBuffer[4] = theDragForce->GetStringParameter("SchattenErrorModel").c_str();
               dragStringBuffer[5] = theDragForce->GetStringParameter("SchattenTimingModel").c_str();
               dragBufferReady = true;
               

               //Warn user about bodies already added as Primary body
               Integer pmSize = (Integer)pointMassBodyList.size();
//               Integer last = (Integer)primaryBodyList.size() - 1;

               for (Integer i = 0; i < pmSize; i++)
               {
//                  if (strcmp(primaryBodyList[last]->bodyName.c_str(),
//                             pointMassBodyList[i]->bodyName.c_str()) == 0)
                  if (strcmp(primaryBodyData->bodyName.c_str(),
                             pointMassBodyList[i]->bodyName.c_str()) == 0)
                  {
                     MessageInterface::PopupMessage
                        (Gmat::WARNING_, "Cannot set %s both as Primary body "
                              "and Point Mass",
//                              pointMassBodyList[last]->bodyName.c_str());
                              primaryBodyData->bodyName.WX_TO_C_STRING);
                  }
               }

//               bool found = false;
//               for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//               {
//                  if ( primaryBodiesArray[i].CmpNoCase(wxBodyName) == 0 )
//                     found = true;
//               }
//
//               if (!found)
//                  primaryBodiesArray.Add(wxBodyName.c_str());
            }
            else if (wxForceType == "SolarRadiationPressure")
            {
               theSRP = (SolarRadiationPressure*) force;
               std::string theSRPModel = "Spherical";
               if (usePropOriginForSrp)
                  theSRPModel = theSRP->GetStringParameter("SRPModel");
               srpModelName = theSRPModel.c_str();

               // Currently SRP can only be applied to force model central body,
               // so we don't need to set to primary body list (loj:2007.10.19)
               //currentBodyId = FindPrimaryBody(wxBodyName);
               //primaryBodyList[currentBodyId]->useSrp = true;
               //primaryBodyList[currentBodyId]->bodyName = wxBodyName;
               //primaryBodyList[currentBodyId]->srpf = theSRP;
            }
         }
      }
      #ifdef DEBUG_PROP_PANEL_LOAD
      MessageInterface::ShowMessage("PropagationConfigPanel::PopulateForces() leaving\n");
      #endif
   }
   catch (BaseException &e)
   {
      #ifdef DEBUG_PROP_PANEL_LOAD
      MessageInterface::ShowMessage("PropagationConfigPanel::PopulateForces() caught an exception\n");
      #endif
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveData()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("SaveData() thePropagatorName=%s\n", thePropagator->GetTypeName().c_str());
   MessageInterface::ShowMessage("   isIntegratorChanged=%d\n", isIntegratorChanged);
   MessageInterface::ShowMessage("   isIntegratorDataChanged=%d\n",isIntegratorDataChanged);
   MessageInterface::ShowMessage("   isForceModelChanged=%d\n", isForceModelChanged);
   MessageInterface::ShowMessage("   isDegOrderChanged=%d\n", isDegOrderChanged);
   MessageInterface::ShowMessage("   isPotFileChanged=%d\n", isPotFileChanged);
   MessageInterface::ShowMessage("   isTideFileChanged=%d\n", isTideFileChanged);
   MessageInterface::ShowMessage("   isAtmosChanged=%d\n", isAtmosChanged);
   MessageInterface::ShowMessage("   isOriginChanged=%d\n", isOriginChanged);
   MessageInterface::ShowMessage("   isErrControlChanged=%d\n", isErrControlChanged);
   MessageInterface::ShowMessage("   isSRPModelChanged=%d\n", isSRPModelChanged);
   MessageInterface::ShowMessage("   isDragModelChanged=%d\n", isDragModelChanged);
   #endif

   canClose = true;

   if (thePropagator->IsOfType("Integrator"))
   {
      //-----------------------------------------------------------------
      // check for valid gravity model before saving
      //-----------------------------------------------------------------
//      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)

      if (primaryBodyData != NULL)
      {
         if (primaryBodyData->bodyName != "None")
         {
   //         if (primaryBodyList[i]->gravType == "None")
            if ((primaryBodyData->gravType == "None") &&
                (primaryBodyData->bodyName != "None"))
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "Please select Gravity Field Model for %s\n",
   //                     primaryBodyList[i]->bodyName.c_str());
                        primaryBodyData->bodyName.WX_TO_C_STRING);
               canClose = false;
               return;
            }
            //loj: 2007.10.26
            // Do we need to check empty potential file?
            // For now allow to use default coefficients from the body.
//            else if (primaryBodyList[i]->gravType == "Other" &&
//                     primaryBodyList[i]->potFile == "")
            else if (primaryBodyData->gravType == "Other" &&
                     primaryBodyData->potFile == "")
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "Please select a potential file for %s\n",
                   primaryBodyData->bodyName.WX_TO_C_STRING);
//               primaryBodyList[i]->bodyName.c_str());
               canClose = false;
               return;
            }
         }
         else
         {
            delete primaryBodyData;
            primaryBodyData = NULL;
         }
      }

      //-----------------------------------------------------------------
      // check for empty primary bodies or point mass
      //-----------------------------------------------------------------
//      if (primaryBodyList.size() == 0 && pointMassBodyList.size() == 0)
      if (primaryBodyData == NULL && pointMassBodyList.size() == 0)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Please select primary bodies or point mass "
                  "bodies\n");
         canClose = false;
         return;
      }

      //-----------------------------------------------------------------
      // save values to base, base code should do the range checking
      //-----------------------------------------------------------------

      //-------------------------------------------------------
      // Saving the Integrator
      //-------------------------------------------------------
      if (isIntegratorChanged)
      {
         #ifdef DEBUG_PROP_PANEL_SAVE
         ShowPropData("SaveData() BEFORE saving Integrator");
         #endif

         isIntegratorChanged = false;

         // Check if integrator data or the force model degree and order
         // values have changed.  If they are invalid entries, return to
         // avoid saving other sections of the propagator's settings
         if (isIntegratorDataChanged)
         {
            if (SaveIntegratorData())
               isIntegratorDataChanged = false;
            else
            {
               isIntegratorChanged = true;
               return;
            }
         }

         if (isDegOrderChanged)
            SaveDegOrder();

         if (!canClose)
         {
            isIntegratorChanged = true;
            return;
         }

         thePropSetup->SetPropagator(thePropagator, true);
         // Since the propagator is cloned in the base code, get new pointer
         thePropagator = thePropSetup->GetPropagator();
         if (!thePropagator->IsOfType("Integrator"))
         {
            thePropSetup->SetODEModel(NULL);
            theForceModel = thePropSetup->GetODEModel();
         }
      }
      else if (isIntegratorDataChanged)
      {
         if (SaveIntegratorData())
            isIntegratorDataChanged = false;
      }
      //-------------------------------------------------------
      // Saving the force model
      //-------------------------------------------------------
      if ((theForceModel != NULL) && isForceModelChanged)
      {
         #ifdef DEBUG_PROP_PANEL_SAVE
         ShowForceList("SaveData() BEFORE saving ForceModel");
         #endif

         if (isDegOrderChanged)
            SaveDegOrder();

         // If the force model's degree and/or order values are invalid
         // entries, return to avoid saving other sections of the
         // propagator's settings
         if (!canClose)
            return;

         // save force model name for later use
         std::string fmName = theForceModel->GetName();

         isForceModelChanged = false;
         ODEModel *newFm = new ODEModel(fmName, "ForceModel");
         newFm->SetCommentLine(mFmPrefaceComment);
         std::string bodyName;

         //----------------------------------------------------
         // save point mass force model
         //----------------------------------------------------
         for (Integer i=0; i < (Integer)pointMassBodyList.size(); i++)
         {
            thePMF = new PointMassForce();
            bodyName = pointMassBodyList[i]->bodyName.c_str();
            thePMF->SetBodyName(bodyName);
            pointMassBodyList[i]->pmf = thePMF;
            newFm->AddForce(thePMF);
         }

         //----------------------------------------------------
         // save gravity force model
         //----------------------------------------------------
         try
         {
//            for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
            if (primaryBodyData != NULL)
            {
//               GravityField *gf = primaryBodyList[i]->gravf;
               GravityField *gf = primaryBodyData->gravf;

               Integer deg = -999;
               Integer ord = -999;

               // save deg and order for later use
               if (gf != NULL)
               {
                  deg = gf->GetIntegerParameter("Degree");
                  ord = gf->GetIntegerParameter("Order");
               }

               // Create new GravityField since ForceModel destructor will
               // delete all PhysicalModel
//               bodyName = primaryBodyList[i]->bodyName.c_str();
               bodyName = primaryBodyData->bodyName.c_str();
               #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage("SaveData() Creating "
                        "GravityField for %s\n", bodyName.c_str());
               #endif
               theGravForce = (GravityField*)theGuiInterpreter->CreateObject("GravityField","");

//               theGravForce = new GravityField("", bodyName);
               theGravForce->SetSolarSystem(theSolarSystem);
               theGravForce->SetStringParameter("BodyName", bodyName);
               theGravForce->SetStringParameter("PotentialFile",
//                     primaryBodyList[i]->potFile.c_str());
                    primaryBodyData->potFile.c_str());
               theGravForce->SetStringParameter("TideFile",
                    primaryBodyData->tideFile.c_str());

               if (deg != -999)
               {
                  theGravForce->SetIntegerParameter("Degree", deg);
                  theGravForce->SetIntegerParameter("Order", ord);
               }

//               primaryBodyList[i]->gravf = theGravForce;
               primaryBodyData->gravf = theGravForce;
               newFm->AddForce(theGravForce);
            }
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         }

         if (isPotFileChanged || isTideFileChanged)
            DisplayGravityFieldData(primaryBodyData->bodyName);

         if (isDegOrderChanged)
            SaveDegOrder();

         if (isPotFileChanged)
            SavePotFile();

         if (isTideFileChanged)
            SaveTideFile();

         if (isTideModelChanged)
            SaveTideModel();

         //----------------------------------------------------
         // save drag force model
         //----------------------------------------------------
         Integer paramId;

         try
         {
//            for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
            if (primaryBodyData != NULL)
            {
//               if (primaryBodyList[i]->dragType == dragModelArray[NONE_DM])
//               {
//                  primaryBodyList[i]->dragf = NULL;
               if (primaryBodyData->dragType == dragModelArray[NONE_DM])
               {
                  primaryBodyData->dragf = NULL;
               }
               else
               {
                  #ifdef DEBUG_PROP_PANEL_SAVE
                  ShowForceList("SaveData() BEFORE  saving DragForce");
                  #endif

                  DragForce *df = primaryBodyData->dragf;

                  // save drag flux info for later use
                  if ((df != NULL) && (!dragBufferReady))
                  {
                     dragParameterBuffer[0] = df->GetRealParameter("F107");
                     dragParameterBuffer[1] = df->GetRealParameter("F107A");
                     dragParameterBuffer[2] = df->GetRealParameter("MagneticIndex");

                     dragBufferReady = true;
                  }

                  // create new DragForce
//                  bodyName = primaryBodyList[i]->bodyName.c_str();
                  bodyName = primaryBodyData->bodyName.c_str();
//                  theDragForce = new DragForce(primaryBodyList[i]->dragType.c_str());
                  theDragForce = new DragForce(
                     std::string(primaryBodyData->dragType.c_str()));
                  theCelestialBody = theSolarSystem->GetBody(bodyName);
                  theAtmosphereModel = theCelestialBody->GetAtmosphereModel();

                  #ifdef DEBUG_PROP_PANEL_SAVE
                  ShowForceList("Entering if (theAtmosphereModel == NULL)");
                  #endif

                  if (theAtmosphereModel == NULL)
                  {
                     theAtmosphereModel = (AtmosphereModel*)theGuiInterpreter->CreateObject
//                           (primaryBodyList[i]->dragType.c_str(), primaryBodyList[i]->dragType.c_str());
                           (primaryBodyData->dragType.c_str(),
                            primaryBodyData->dragType.c_str());

                     #ifdef DEBUG_PROP_PANEL_SAVE
                     ShowForceList("Exiting if (theAtmosphereModel == NULL)");
                     #endif
                  }

                  theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
                  theDragForce->SetStringParameter("DragModel", dragModelName.c_str());

                  #ifdef DEBUG_PROP_PANEL_SAVE
                  ShowForceList("theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);");
                  #endif

                  paramId = theDragForce->GetParameterID("AtmosphereModel");
//                  bodyName = primaryBodyList[i]->bodyName.c_str();
                  bodyName = primaryBodyData->bodyName.c_str();
//                  theDragForce->SetStringParameter(paramId, primaryBodyList[i]->dragType.c_str());
                  theDragForce->SetStringParameter(paramId,
                        primaryBodyData->dragType.c_str());
                  theDragForce->SetStringParameter("BodyName", bodyName);

                  // Save drag settings, if the user has changed things
                  if (dragBufferReady)
                  {
                       theDragForce->SetStringParameter("PredictedWeatherSource", dragStringBuffer[0].c_str());
                       theDragForce->SetStringParameter("HistoricWeatherSource", dragStringBuffer[1].c_str());
                       theDragForce->SetRealParameter("F107", dragParameterBuffer[0]);
                       theDragForce->SetRealParameter("F107A", dragParameterBuffer[1]);
                       theDragForce->SetRealParameter("MagneticIndex", dragParameterBuffer[2]);
                       theDragForce->SetStringParameter("CSSISpaceWeatherFile", dragStringBuffer[2].c_str());
                       theDragForce->SetStringParameter("SchattenFile", dragStringBuffer[3].c_str());
                       theDragForce->SetStringParameter("SchattenErrorModel", dragStringBuffer[4].c_str());
                       theDragForce->SetStringParameter("SchattenTimingModel", dragStringBuffer[5].c_str());

                       dragBufferReady = false;

                  }

//                  primaryBodyList[i]->dragf = theDragForce;
                  primaryBodyData->dragf = theDragForce;
                  newFm->AddForce(theDragForce);

                  #ifdef DEBUG_PROP_PANEL_SAVE
                  ShowForceList("SaveData() AFTER  saving DragForce");
                  #endif
               }
            }
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            // we need to set canClose, reset the Changed flag, and then return,
            // so that the Apply button remains highlighted and SaveData
            // correctly executes on the next OK/Apply.
            canClose = false;
            isForceModelChanged = true;
            return;
         }

         //----------------------------------------------------
         // save RelativisticCorrection and SRP data
         //----------------------------------------------------
         try
         {
            #ifdef DEBUG_PROP_PANEL_SAVE
               MessageInterface::ShowMessage("SaveData() saving RelativisticCorrection: addRC = %s\n", (addRelativisticCorrection? "true" : "false"));
            #endif
            paramId = newFm->GetParameterID("RelativisticCorrection");
            if (addRelativisticCorrection)
            {
               theRC = new RelativisticCorrection();
               bodyName = propOriginName;
               theRC->SetStringParameter("BodyName", bodyName);
               GmatGlobal *gg = GmatGlobal::Instance();
               theRC->SetEopFile(gg->GetEopFile());
               newFm->AddForce(theRC);
               newFm->SetOnOffParameter(paramId, "On");
            }
            else
               newFm->SetOnOffParameter(paramId, "Off");

            paramId= newFm->GetParameterID("SRP");

            #ifdef DEBUG_PROP_PANEL_SAVE_SRP
               MessageInterface::ShowMessage("SaveData() saving SRP: usePropOriginForSrp = %s\n", (usePropOriginForSrp? "true" : "false"));
            #endif
            if (usePropOriginForSrp)
            {
               theSRP = new SolarRadiationPressure();
               bodyName = propOriginName;
               theSRP->SetStringParameter("BodyName", bodyName);
               theSRP->SetStringParameter("SRPModel", srpModelName.c_str());
               newFm->AddForce(theSRP);
               newFm->SetOnOffParameter(paramId, "On");
            }
            else
            {
               newFm->SetOnOffParameter(paramId, "Off");
            }

            #ifdef DEBUG_PROP_PANEL_SAVE
            ShowForceList("SaveData() AFTER  saving SRP");
            #endif

            // Since SRP is only applied to force model central body,
            // we don't need to go through primary body list (loj: 2007.10.19)
            //for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
            //{
            //   if (primaryBodyList[i]->useSrp)
            //   {
            //      theSRP = new SolarRadiationPressure();
            //      primaryBodyList[i]->srpf = theSRP;
            //      bodyName = primaryBodyList[i]->bodyName.c_str();
            //      theSRP->SetStringParameter("BodyName", bodyName);
            //      newFm->AddForce(theSRP);
            //
            //      paramId= newFm->GetParameterID("SRP");
            //      newFm->SetOnOffParameter(paramId, "On");
            //
            //      #ifdef DEBUG_PROP_PANEL_SAVE
            //      ShowForceList("SaveData() AFTER  saving SRP");
            //      #endif
            //   }
            //}
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         }

         //----------------------------------------------------
         // Saving the error control and Origin
         // Always set this to new forcemodel
         //----------------------------------------------------
         try
         {
            newFm->SetStringParameter("ErrorControl", errorControlTypeName.c_str());
            newFm->SetStringParameter("CentralBody", propOriginName.c_str());
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         }

         //----------------------------------------------------
         // Saving forces to the prop setup
         //----------------------------------------------------
         thePropSetup->SetODEModel(newFm);
         numOfForces = thePropSetup->GetNumForces();

         // ForceModel is deleted in PropSetup::SetForceModel()
         //theForceModel = newFm;

         // Since the force model and its physical models are cloned in the
         // base code, get new pointers
         theForceModel = thePropSetup->GetODEModel();
         if (theForceModel != NULL)
         {
            theForceModel->SetName(fmName);

            PhysicalModel *pm;
//            int size = primaryBodyList.size();
            int size = (primaryBodyData == NULL ? 0 : 1);
            for (int i=0; i<numOfForces; i++)
            {
               pm = theForceModel->GetForce(i);

               #ifdef DEBUG_PROP_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   pm=<%p><%s>'%s', Body='%s'\n", pm, pm->GetTypeName().c_str(),
                   pm->GetName().c_str(), pm->GetBodyName().c_str());
               #endif

               for (int j=0; j<size; j++)
               {
//                  if ((primaryBodyList[j]->bodyName).IsSameAs(pm->GetBodyName().c_str()))
                  if ((primaryBodyData->bodyName).IsSameAs(
                        pm->GetBodyName().c_str()))
                  {
                     if (pm->GetTypeName() == "PointMassForce")
//                        primaryBodyList[j]->pmf = (PointMassForce*)pm;
                        primaryBodyData->pmf = (PointMassForce*)pm;
                     else if (pm->GetTypeName() == "GravityField")
//                        primaryBodyList[j]->gravf = (GravityField*)pm;
                        primaryBodyData->gravf = (GravityField*)pm;
                     else if (pm->GetTypeName() == "DragForce")
//                        primaryBodyList[j]->dragf = (DragForce*)pm;
                        primaryBodyData->dragf = (DragForce*)pm;
                     else if (pm->GetTypeName() == "SolarRadiationPressure")
//                        primaryBodyList[j]->srpf = (SolarRadiationPressure*)pm;
                        primaryBodyData->srpf = (SolarRadiationPressure*)pm;
                  }
               }
            }
         }
         #ifdef DEBUG_PROP_PANEL_SAVE
         ShowForceList("SaveData() AFTER  saving ForceModel");
         #endif

         // If there is a configured ForceModel that matches this updated one, update it
         GmatBase *configFM = theGuiInterpreter->GetConfiguredObject(fmName);
         if (configFM->IsOfType(Gmat::ODE_MODEL))
         {
            (*((ODEModel*)(configFM))) = (*theForceModel);
         }

      } // end if(isForceModelChange)
      else
      {
         //----------------------------------------------------
         // Saving Error Control and the Origin (Central Body)
         //----------------------------------------------------
         if (theForceModel != NULL)
         {
            if (isDegOrderChanged)
               SaveDegOrder();

            // If the force model's degree and/or order values are invalid
            // entries, return to avoid saving other sections of the
            // propagator's settings
            if (!canClose)
               return;

            try
            {
               if (isErrControlChanged)
               {
                  theForceModel->SetStringParameter
                     ("ErrorControl", theErrorComboBox->GetStringSelection().c_str());
                  isErrControlChanged = false;
               }

               if (isOriginChanged)
               {
                  theForceModel->SetStringParameter("CentralBody", propOriginName.c_str());
                  isOriginChanged = false;
               }
            }
            catch (BaseException &e)
            {
               MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            }

            // Save only GravComboBox or PotFileText is changed
            if (isPotFileChanged || isTideFileChanged)
               DisplayGravityFieldData(primaryBodyData->bodyName);

            if (isDegOrderChanged)
               SaveDegOrder();

            if (isPotFileChanged)
               SavePotFile();
            
            if (isTideFileChanged)
               SaveTideFile();

            if (isTideModelChanged)
               SaveTideModel();

            if (isAtmosChanged)
               SaveAtmosModel();
         }
      }
   }
   else // It's an analytic propagator -- Just SPK for now
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
         MessageInterface::ShowMessage("Saving a force-free propagator\n");
      #endif

      if (isIntegratorChanged)
      {
         #ifdef DEBUG_PROP_PANEL_SAVE
         ShowPropData("SaveData() BEFORE saving Propagator");
         #endif

         // First validate the data
         if (isIntegratorDataChanged)
         {
            if (SavePropagatorData())
               isIntegratorDataChanged = false;
            else
               // Invalid data, so do not update the configured objects
               return;
         }
         
         thePropSetup->SetPropagator(thePropagator, true);
         thePropSetup->SetODEModel(NULL);  // No force model for these puppies

         // Since the propagator is cloned in the base code, get new pointer
         thePropagator = thePropSetup->GetPropagator();
         theForceModel = thePropSetup->GetODEModel();

         isIntegratorChanged = false;
      }
      else if (isIntegratorDataChanged)
      {
         if (SavePropagatorData())
            isIntegratorDataChanged = false;
      }
   }

   #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage("SaveData complete; prop = %p, ODE = %p\n",
            thePropagator, theForceModel);
   #endif
}


//------------------------------------------------------------------------------
// Integer FindPrimaryBody(const wxString &bodyName, bool create = true,
//                         const wxString &gravType, const wxString &dragType,
//                         const wxString &magfType)
//------------------------------------------------------------------------------
/*
 * Finds the primary body from the primaryBodyList and returns index if body
 * was found return index. If body was not found and creaet is false,
 * it returns -1, otherwise create a new ForceType and return new index
 *
 * @param  bodyName  Name of the body to look for
 * @param  create    If true, it creates new ForceType
 * @param  gravType  The type name of GravityForce
 * @param  dragType  The type name of DragForce
 * @param  magfType  The type name of MagneticForce
 *
 * @return body index or -1 if body not found and create is false
 *
 */
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindPrimaryBody(const wxString &bodyName,
                                                bool create,
                                                const wxString &gravType,
                                                const wxString &dragType,
                                                const wxString &magfType)
{
//   for (Integer i=0; i<(Integer)primaryBodyList.size(); i++)
   if (primaryBodyData != NULL)
   {
//      if (primaryBodyList[i]->bodyName == bodyName)
      if (primaryBodyData->bodyName == bodyName)
         return 0;
   }

   if (!create)
      return -1;

//   primaryBodyList.push_back(new ForceType(bodyName, gravType, dragType, magfType));
   primaryBodyData = new ForceType(bodyName, gravType, "None", "None", dragType, magfType);

   // Set gravity model file
   if (theFileMap.find(gravType) != theFileMap.end())
   {
      std::string potFileType = theFileMap[gravType].WX_TO_STD_STRING;
      wxString wxPotFileName = theGuiInterpreter->GetFileName(potFileType).c_str();
      #ifdef DEBUG_PROP_PANEL_FIND_BODY
      MessageInterface::ShowMessage("FindPrimaryBody() potFileType=%s\n", potFileType.c_str());
      #endif
//      primaryBodyList.back()->potFile = wxPotFileName;
      primaryBodyData->potFile = wxPotFileName;
   }

   #ifdef DEBUG_PROP_PANEL_FIND_BODY
   ShowForceList("FindPrimaryBody() after add body to primaryBodyList");
   #endif

//   return (Integer)(primaryBodyList.size() - 1);
   return (primaryBodyData == NULL ? 0 : 1);
}


//------------------------------------------------------------------------------
// Integer FindPointMassBody(const wxString &bodyName)
//------------------------------------------------------------------------------
/*
 * Finds the point mass body from the pointMassBodyList and returns index if body
 * was found return index. If body was not found, it returns -1,
 *
 * @param  bodyName  Name of the body to look for
 *
 * @return body index or -1 if body was not found
 *
 */
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindPointMassBody(const wxString &bodyName)
{
   for (Integer i=0; i<(Integer)pointMassBodyList.size(); i++)
   {
      if (pointMassBodyList[i]->bodyName == bodyName)
         return i;
   }

   return -1;
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Initialize()
{
   #ifdef DEBUG_PROP_PANEL_INIT
   MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() entered\n");
   #endif

   thePropagator      = NULL;
   thePropSetup       = NULL;
   theForceModel      = NULL;
   thePMF             = NULL;
   theDragForce       = NULL;
   theGravForce       = NULL;
   theSRP             = NULL;
   theRC              = NULL;
   theSolarSystem     = NULL;
   theCelestialBody   = NULL;
   theAtmosphereModel = NULL;

   // Default integrator values
   thePropagatorName = "";
   integratorString  = "RKV 8(9)";

   // Default force model values
   useDragForce        = false;
   usePropOriginForSrp = false;
   addRelativisticCorrection = false;
   numOfForces         = 0;

   // Changed flags
   isForceModelChanged = false;
   isAtmosChanged = false;
   isDegOrderChanged = false;
   isPotFileChanged = false;
   isTideFileChanged = false;
   isTideModelChanged = false;
   isMagfTextChanged = false;
   isIntegratorChanged = false;
   isIntegratorDataChanged = false;
   isOriginChanged = false;
   isErrControlChanged = false;

   isSpkStepChanged     = false;
   isSpkBodyChanged     = false;
   isSpkEpFormatChanged = false;
   isSpkEpochChanged    = false;
   isSRPModelChanged    = false;
   isDragModelChanged   = false;

   //Note: All the settings should match enum types in the header.

   StringArray propTypes = theGuiInterpreter->GetListOfFactoryItems(Gmat::PROPAGATOR);
   IntegratorCount = (Integer)propTypes.size();
   for (UnsignedInt i = 0; i < propTypes.size(); ++i)
   {
      integratorArray.Add(propTypes[i].c_str());
      integratorTypeArray.Add(propTypes[i].c_str());
   }

   // initialize drag model type array
   dragModelArray.Clear();
   dragModelArray.Add("None");
//   dragModelArray.Add("Exponential");
   StringArray models = theGuiInterpreter->GetListOfFactoryItems(Gmat::ATMOSPHERE, "Earth");
   for (UnsignedInt i = 0; i < models.size(); ++i)
      dragModelArray.Add(models[i].c_str());
   
   // initialize SRP Model type array
   dragModelListArray.Add("Spherical");
   dragModelListArray.Add("SPADFile");
   
   dragModelName = "Spherical";
   

//   dragModelArray.Add("MSISE90");
//   dragModelArray.Add("JacchiaRoberts");

   // initialize error control type array
   errorControlArray.Add("None");
   errorControlArray.Add("RSSStep");
   errorControlArray.Add("RSSState");
   errorControlArray.Add("LargestStep");
   errorControlArray.Add("LargestState");

   // initialize SRP Model type array
   srpModelArray.Add("Spherical");
   srpModelArray.Add("SPADFile");

   srpModelName = "Spherical";

   // for actual file keyword used in FileManager
   theFileMap["JGM-2"] = "JGM2_FILE";
   theFileMap["JGM-3"] = "JGM3_FILE";
   theFileMap["EGM-96"] = "EGM96_FILE";
   theFileMap["LP-165"] = "LP165P_FILE";
   theFileMap["MGNP-180U"] = "MGNP180U_FILE";
   theFileMap["Mars-50C"] = "MARS50C_FILE";

   #ifdef DEBUG_PROP_PANEL_INIT
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Initialize() Initialized local arrays.\n");
   #endif

   // initialize mag. filed model type array
   magfModelArray.Add("None");

   // Initialize the data for drag modeling
   dragParameterBuffer[0] = dragParameterBuffer[1] = 150.0;
   dragParameterBuffer[2] = 3.0;
   dragBufferReady = false;
}


//------------------------------------------------------------------------------
// void DisplayIntegratorData(bool integratorChanged)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{
   int propIndex = theIntegratorComboBox->GetSelection();

   #ifdef DEBUG_PROP_INTEGRATOR
   MessageInterface::ShowMessage
      ("DisplayIntegratorData() entered, integratorChanged=%d, integratorString=<%s>\n",
       integratorChanged, integratorString.WX_TO_C_STRING);
   #endif

   if (integratorChanged)
   {
      std::string integratorType = integratorTypeArray[propIndex].WX_TO_STD_STRING;
      thePropagatorName = integratorType;
      for (Integer i = 0; i < thePropagatorName.size(); ++i)
         if (thePropagatorName[i] == '-')
            thePropagatorName[i] = '_';

      thePropagator = (Propagator*)theGuiInterpreter->GetConfiguredObject(thePropagatorName);
      if (thePropagator == NULL)
      {
         thePropagator = (Propagator*)
            theGuiInterpreter->CreateObject(integratorType, thePropagatorName);
      }
      
      if (!thePropagator->IsOfType("Integrator"))
      {
         Real ss     = thePropagator->GetRealParameter("StepSize");
         spkStep     = ToString(ss);
         spkBody     = thePropagator->GetStringParameter("CentralBody").c_str();
         spkEpFormat = thePropagator->GetStringParameter("EpochFormat").c_str();
         spkEpoch    = thePropagator->GetStringParameter("StartEpoch").c_str();

         try
         {
            StringArray choices =
                  thePropagator->GetStringArrayParameter("StartOptions");
            startEpochChoices.Clear();
            for (UnsignedInt i = 0; i < choices.size(); ++i)
               startEpochChoices.Add(choices[i].c_str());

            startEpochCombobox->Clear();
            startEpochCombobox->Append(startEpochChoices);
         }
         catch (BaseException &)  // Ignore exceptions here
         {
         }

         isSpkStepChanged     = true;
         isSpkBodyChanged     = true;
         isSpkEpFormatChanged = true;
         isSpkEpochChanged    = true;
      }
      
      if (theForceModel == NULL)    // Switched from an ODE-model free propagator
      {
         theForceModelName = thePropSetup->GetStringParameter("FM");
         if ((theForceModelName == "UndefinedODEModel") ||
             (theForceModelName == ""))
            theForceModelName = thePropSetup->GetName() + "_ForceModel";

         theForceModel = (ODEModel*)theGuiInterpreter->GetConfiguredObject(theForceModelName);

         if (theForceModel == NULL)
         {
            theForceModel = (ODEModel*)theGuiInterpreter->CreateNewODEModel(theForceModelName);
         }

         // Load the panel with the force model data
         numOfForces   = theForceModel->GetNumForces();

         PopulateForces();
         // Display the force model origin (central body)
         theOriginComboBox->SetValue(propOriginName);

//         if (!primaryBodiesArray.IsEmpty())
         if (primaryBody != "None")
         {
//            primaryBodyString = primaryBodiesArray.Item(0).c_str();
            primaryBodyString = primaryBody;
            currentBodyName = primaryBodyString;
            currentBodyId = FindPrimaryBody(currentBodyName);
         }

//         numOfBodies = (Integer)primaryBodiesArray.GetCount();
//         thePrimaryBodyComboBox->Clear();
//         if (numOfBodies > 0)
//         {
//            for (Integer index = 0; index < numOfBodies; ++index)
//               thePrimaryBodyComboBox->Append(primaryBodiesArray[index].c_str());
//            thePrimaryBodyComboBox->SetSelection(0);
//         }
         UpdatePrimaryBodyComboBoxList();
//         thePrimaryBodyComboBox->SetValue(primaryBodyList[0]->bodyName.c_str());
         if (primaryBodyData != NULL)
            thePrimaryBodyComboBox->SetValue(primaryBodyData->bodyName.c_str());
         else
            thePrimaryBodyComboBox->SetValue("None");
//         if (numOfBodies == 0)
         if (primaryBody == "None")
            EnablePrimaryBodyItems(false, false);
         else
            EnablePrimaryBodyItems(true);

         DisplayForceData();
         isForceModelChanged = true;
      }
      
      #ifdef DEBUG_PROP_INTEGRATOR
      MessageInterface::ShowMessage
         ("   integratorType='%s', thePropagatorName='%s'\n   thePropagator=<%p>'%s'\n",
          integratorType.c_str(), thePropagatorName.c_str(), thePropagator,
          thePropagator ? thePropagator->GetName().c_str() : "NULL");
      #endif
   }

   if (thePropagator->IsOfType("Integrator"))
   {
      #ifdef __WXMAC__    // TBD: Is this distinction still needed?
      //   if (integratorString.IsSameAs(integratorArray[ABM]))
         if (thePropagator->IsOfType("PredictorCorrector"))
         {
            minIntErrorStaticText->Show(true);
            nomIntErrorStaticText->Show(true);
            minIntErrorTextCtrl->Show(true);
            nomIntErrorTextCtrl->Show(true);
            minIntErrorStaticText->Enable(true);
            nomIntErrorStaticText->Enable(true);
            minIntErrorTextCtrl->Enable(true);
            nomIntErrorTextCtrl->Enable(true);
         }
         else
         {
            minIntErrorStaticText->Enable(false);
            nomIntErrorStaticText->Enable(false);
            minIntErrorTextCtrl->Enable(false);
            nomIntErrorTextCtrl->Enable(false);
            minIntErrorStaticText->Show(false);
            nomIntErrorStaticText->Show(false);
            minIntErrorTextCtrl->Show(false);
            nomIntErrorTextCtrl->Show(false);
         }
      #else
      //   if (integratorString.IsSameAs(integratorArray[ABM]))
         if (thePropagator->IsOfType("PredictorCorrector"))
         {
            minIntErrorStaticText->Show(true);
            nomIntErrorStaticText->Show(true);
            minIntErrorTextCtrl->Show(true);
            nomIntErrorTextCtrl->Show(true);
         }
         else
         {
            minIntErrorStaticText->Show(false);
            nomIntErrorStaticText->Show(false);
            minIntErrorTextCtrl->Show(false);
            nomIntErrorTextCtrl->Show(false);
         }
      #endif

      Integer stopId     = thePropagator->GetParameterID("StopIfAccuracyIsViolated");
      stopOnAccViolation = thePropagator->GetBooleanParameter(stopId);
      theStopCheckBox->SetValue(stopOnAccViolation);
      theStopCheckBox->Show(true);
      theStopCheckBox->Enable(true);

      leftBoxSizer->Layout();

      Real i1 = thePropagator->GetRealParameter("InitialStepSize");
      Real i2 = thePropagator->GetRealParameter("Accuracy");
      Real i3 = thePropagator->GetRealParameter("MinStep");
      Real i4 = thePropagator->GetRealParameter("MaxStep");
      Integer i5 = (long)thePropagator->GetIntegerParameter("MaxStepAttempts");

      initialStepSizeTextCtrl->SetValue(ToString(i1));
      accuracyTextCtrl->SetValue(ToString(i2));
      minStepTextCtrl->SetValue(ToString(i3));
      maxStepTextCtrl->SetValue(ToString(i4));
      maxStepAttemptTextCtrl->SetValue(ToString(i5));

//      if (integratorString.IsSameAs(integratorArray[ABM]))
      if (thePropagator->IsOfType("PredictorCorrector"))
      {
         Real i6 = thePropagator->GetRealParameter("LowerError");
         Real i7 = thePropagator->GetRealParameter("TargetError");

         minIntErrorTextCtrl->SetValue(ToString(i6));
         nomIntErrorTextCtrl->SetValue(ToString(i7));
      }
   }
   else
   {
      // Fill in Propagator data
      if (isSpkStepChanged)
      {
         propagatorStepSizeTextCtrl->SetValue(spkStep);
      }

      if (isSpkBodyChanged)
      {
         Integer cbIndex = propCentralBodyComboBox->FindString(spkBody);
         propCentralBodyComboBox->SetSelection(cbIndex);
      }
      if (isSpkEpFormatChanged)
      {
         Integer epIndex = propagatorEpochFormatComboBox->FindString(spkEpFormat);
         propagatorEpochFormatComboBox->SetSelection(epIndex);
      }
      if (isSpkEpochChanged)
      {
         startEpochCombobox->SetValue(spkEpoch);
      }

//      spkStep = propagatorStepSizeTextCtrl->GetValue();
//      spkBody = propCentralBodyComboBox->GetValue();
//      spkEpFormat = propagatorEpochFormatComboBox->GetValue();
//      spkEpoch = startEpochTextCtrl->GetValue();

      isSpkStepChanged     = false;
      isSpkBodyChanged     = false;
      isSpkEpFormatChanged = false;
      isSpkEpochChanged    = false;

      theStopCheckBox->Show(false);
      theStopCheckBox->Enable(false);
   }

   if ((thePropagator == NULL) || (thePropagator->IsOfType("Integrator")))
   {
      ShowIntegratorLayout(true);
   }
   else
   {
      ShowIntegratorLayout(false);
   }

   #ifdef DEBUG_PROP_INTEGRATOR
   ShowPropData("DisplayIntegratorData() exiting...");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayForceData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayForceData()
{
   #ifdef DEBUG_FORCE_DATA
   MessageInterface::ShowMessage("DisplayForceData() entered\n");
   #endif
   
   DisplayErrorControlData();
   DisplaySRPData();
   theRelativisticCorrectionCheckBox->SetValue(addRelativisticCorrection);

   if (!pointMassBodyList.empty())
      DisplayPointMassData();

//   if (primaryBodyList.empty())
   DisplayPrimaryBodyData();
   if (primaryBodyData == NULL)
      return;

   DisplayGravityFieldData(currentBodyName);
   DisplayAtmosphereModelData();
   DisplayMagneticFieldData();
   
   #ifdef DEBUG_FORCE_DATA
   MessageInterface::ShowMessage("DisplayForceData() leaving\n");
   #endif
}
//------------------------------------------------------------------------------
// void DisplayPrimaryBodyData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayPrimaryBodyData()
{
//   Integer bodyIndex = 0;
//
//   bodyTextCtrl->Clear();
//
//   for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//   {
//      bodyTextCtrl->AppendText(primaryBodiesArray.Item(i) + " ");
//      if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
//         bodyIndex = i;
//   }
//
//   thePrimaryBodyComboBox->SetSelection(bodyIndex);

   // todo If none, should we check (and delete) primaryBodyData pointer here?
   thePrimaryBodyComboBox->SetValue(primaryBody);
}

//------------------------------------------------------------------------------
// void DisplayGravityFieldData(const wxString& bodyName, bool textValueChanged = false)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayGravityFieldData(const wxString& bodyName,
                                                     bool potValueChanged,
                                                     bool tideValueChanged)
{
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage
      ("DisplayGravityFieldData() entered, bodyName='%s', potValueChanged=%d\n"
       "   potValueChanged=%d, currentBodyName=%s gravType=%s\n", bodyName.WX_TO_C_STRING,
       potValueChanged, tideValueChanged,
//       currentBodyName.c_str(), primaryBodyList[currentBodyId]->gravType.c_str());
       currentBodyName.WX_TO_C_STRING, primaryBodyData->gravType.WX_TO_C_STRING);
   ShowForceList("DisplayGravityFieldData() entered");
   #endif

   if (primaryBodyData == NULL)
      return;

   theGravModelComboBox->Clear();

   wxString gravType = primaryBodyData->gravType;
   wxString tideData = primaryBodyData->tideData;
   wxString tideModel = primaryBodyData->tideModel;

   // for gravity model ComboBox
   theGravModelComboBox->Append
      (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_NONE].c_str());
   if (bodyName == "Earth")
      {
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_JGM2].c_str());
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_JGM3].c_str());
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_EGM96].c_str());
      }
   else if (bodyName == "Luna")
      {
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_LP165P].c_str());
      }
   else if (bodyName == "Venus")
      {
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_MGNP180U].c_str());
      }
   else if (bodyName == "Mars")
      {
      theGravModelComboBox->Append
         (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_MARS50C].c_str());
      }
   theGravModelComboBox->Append
      (GravityField::GRAVITY_MODEL_NAMES[GravityField::GFM_OTHER].c_str());

   theTideDataComboBox->Clear();
   theTideModelComboBox->Clear();
   theTideModelComboBox->Append(HarmonicGravity::ETideString[0]);
   std::string filename = primaryBodyData->potFileFullPath.WX_TO_STD_STRING;
   std::string tideFilename = primaryBodyData->tideFileFullPath.WX_TO_STD_STRING;
   gravityMaxDegreeTextCtrl->SetValue("UNK");
   gravityMaxOrderTextCtrl->SetValue("UNK");
   tideTextCtrl->SetValue("UNK");

   if (filename != "")
      {
      try
         {
         theCelestialBody = theSolarSystem->GetBody(primaryBodyData->bodyName);
         HarmonicGravity* hg = GravityField::GetHarmonicGravity
              (std::string(filename.c_str()),
               std::string(tideFilename.c_str()),
               theCelestialBody->GetGravitationalConstant(),
               theCelestialBody->GetEquatorialRadius(),
               std::string(primaryBodyData->bodyName),false);
         if (hg == NULL)
            {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format.");
            return;
            }

         gravityMaxDegreeTextCtrl->SetValue(GmatStringUtil::ToString(hg->GetNN()));
         gravityMaxOrderTextCtrl->SetValue(GmatStringUtil::ToString(hg->GetMM()));

         theTideModelComboBox->Clear();
         for (int i = 0; i <= HarmonicGravity::ETideCount - 1; ++i)
         {
            if (hg->HaveTideModel(i))
               theTideModelComboBox->Append(HarmonicGravity::ETideString[i]);
            else
            {
               if (tideModel == HarmonicGravity::ETideString[i])
               {
                  // Set to None if previous tide setting is not in new tide model
                  tideModel = HarmonicGravity::ETideString[0];
                  primaryBodyData->tideModel = tideModel;
                  isTideModelChanged = true;
               }
            }
         }
         tideTextCtrl->SetValue(hg->TideString());

         delete hg;
         }
      catch (BaseException &be)
         {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "File \"" + filename + "\" is not a valid gravity file.");
         return;
         }
      }

   theGravModelSearchButton->Enable(false);
   theTideModelSearchButton->Enable(false);
   ////potFileStaticText->Enable(false);
   potFileTextCtrl->Enable(false);
   theTideDataComboBox->Enable(false);
   theTideModelComboBox->Enable(false);
   tideTextCtrl->Enable(false);
   gravityDegreeTextCtrl->Enable(true);
   gravityMaxDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
   gravityMaxOrderTextCtrl->Enable(true);
   gravityStmLimitTextCtrl->Enable(true);
//   potFileTextCtrl->SetValue(primaryBodyList[currentBodyId]->potFile);
   // If text value not changed, use ChangeValue() so that
   // wxEVT_COMMAND_TEXT_UPDATED event will not be generated (LOJ: 2014.08.11)
   if (potValueChanged)
      potFileTextCtrl->SetValue(primaryBodyData->potFile);
   else
      potFileTextCtrl->ChangeValue(primaryBodyData->potFile);

   if (tideValueChanged)
      tideFileTextCtrl->SetValue(primaryBodyData->tideFile);
   else
      tideFileTextCtrl->ChangeValue(primaryBodyData->tideFile);

   bool isGravFile = GmatStringUtil::ToLower(wxFileName(filename).GetExt().WX_TO_STD_STRING) == "grv";

   theTideDataComboBox->Append("None");
   if (bodyName == "Earth" || isGravFile)
      theTideDataComboBox->Append("Inherited");
   theTideDataComboBox->Append("Tide File");

   if (potValueChanged && isGravFile && tideData.WX_TO_STD_STRING != "Tide File")
   {
      tideData = "Inherited";
      primaryBodyData->tideData = tideData;
   }

   if (gravType == "None")
   {
      gravityDegreeTextCtrl->Enable(false);
      gravityMaxDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
      gravityMaxOrderTextCtrl->Enable(false);
      gravityStmLimitTextCtrl->Enable(false);
      tideFileTextCtrl->Enable(false);
   }
   else
   {
//      if (primaryBodyList[currentBodyId]->gravType == "Other")
         theGravModelSearchButton->Enable(true);
         potFileStaticText->Enable(true);
         potFileTextCtrl->Enable(true);
         theTideDataComboBox->Enable(true);

         bool hasTideData = tideData != "None";

         theTideModelComboBox->Enable(hasTideData);
         tideTextCtrl->Enable(hasTideData);
         tideFileTextCtrl->Enable(hasTideData);
         theTideModelSearchButton->Enable(hasTideData);
   }

   theGravModelComboBox->SetValue(gravType);
   theTideDataComboBox->SetValue(tideData);
   theTideModelComboBox->SetValue(tideModel);

//   gravityDegreeTextCtrl->SetValue(primaryBodyList[currentBodyId]->gravDegree);
//   gravityOrderTextCtrl->SetValue(primaryBodyList[currentBodyId]->gravOrder);
   // If text value not changed, use ChangeValue() so that
   // wxEVT_COMMAND_TEXT_UPDATED event will not be generated (LOJ: 2014.08.11)
   if (potValueChanged)
   {
      gravityDegreeTextCtrl->SetValue(primaryBodyData->gravDegree);
      gravityOrderTextCtrl->SetValue(primaryBodyData->gravOrrder);
      gravityStmLimitTextCtrl->SetValue(primaryBodyData->gravStmLimit);
   }
   else
   {
      gravityDegreeTextCtrl->ChangeValue(primaryBodyData->gravDegree);
      gravityOrderTextCtrl->ChangeValue(primaryBodyData->gravOrrder);
      gravityStmLimitTextCtrl->ChangeValue(primaryBodyData->gravStmLimit);
   }
   
   #ifdef DEBUG_PROP_PANEL_GRAV
   ShowForceList("DisplayGravityFieldData() exiting");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayAtmosphereModelData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayAtmosphereModelData()
{
   #ifdef DEBUG_PROP_PANEL_DISPLAY
   MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() currentBodyName=%s\n", // dragType=%s\n",
       currentBodyName.WX_TO_C_STRING);
       //, primaryBodyList[currentBodyId]->dragType.c_str()
   MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() dragModelName=%s\n", // dragType=%s\n",
       dragModelName.WX_TO_C_STRING);
   #endif

   // Enable atmosphere model only for bodies that have models
   // initialize drag model type array
   dragModelArray.Clear();
   dragModelArray.Add("None");
   StringArray models = theGuiInterpreter->
         GetListOfFactoryItems(Gmat::ATMOSPHERE, currentBodyName.c_str());

   theDragModelComboBox->SetValue(dragModelName);
   
   if (models.size() > 0)
   {
      for (UnsignedInt i = 0; i < models.size(); ++i)
         dragModelArray.Add(models[i].c_str());
      theAtmosModelComboBox->Clear();
      for (UnsignedInt i = 0; i < dragModelArray.size(); ++i)
         theAtmosModelComboBox->Append(dragModelArray[i]);
      // Set the value to either the passed in value or "None"
      theAtmosModelComboBox->SetValue(atmosModelString); //dragModelArray[0]);
      theAtmosModelComboBox->Enable(true);

      theDragModelComboBox->Enable(true);
      
#ifdef DEBUG_PROP_PANEL_DISPLAY
      MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() models size > 0\n");
#endif

      // Current code always enables Setup for any model; disabled for "None"
//      if (currentBodyName == "Earth")
      if (currentBodyName == "Mars")
         theDragSetupButton->Enable(false);  // Is this right?
      else
         theDragSetupButton->Enable(true);  // Is this right?
   }
   else
   {
#ifdef DEBUG_PROP_PANEL_DISPLAY
      MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() models size <= 0\n");
#endif
      theAtmosModelComboBox->Clear();
      theAtmosModelComboBox->Append("None");
      theAtmosModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
      theDragModelComboBox->Enable(false);
   }

   // Set current drag force pointer
//   theDragForce = primaryBodyList[currentBodyId]->dragf;
   theDragForce = primaryBodyData->dragf;

//   if (primaryBodyList[currentBodyId]->dragType == dragModelArray[NONE_DM])
   if (primaryBodyData->dragType == dragModelArray[NONE_DM])
   {
#ifdef DEBUG_PROP_PANEL_DISPLAY
      MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() NONE\n");
#endif
      theAtmosModelComboBox->SetSelection(NONE_DM);
      theDragSetupButton->Enable(false);
      theDragModelComboBox->Enable(false);
   }
   else
   {
#ifdef DEBUG_PROP_PANEL_DISPLAY
      MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() NOT NONE\n");
#endif
      Integer set = 0;
      for (UnsignedInt i = 0; i < dragModelArray.size(); ++i)
         if (primaryBodyData->dragType == dragModelArray[i])
            set = i;
      theAtmosModelComboBox->SetSelection(set);
      if (primaryBodyData->bodyName == "Mars")
      {
         theDragSetupButton->Enable(false);
//         theDragModelComboBox->Enable(false);
      }
      else
      {
         theDragSetupButton->Enable(true);
//         theDragModelComboBox->Enable(true);
      }
   }
}

//------------------------------------------------------------------------------
// void DisplayPointMassData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayPointMassData()
{
   pmEditTextCtrl->Clear();
   if (!secondaryBodiesArray.IsEmpty())
   {
      for (Integer i = 0; i < (Integer)secondaryBodiesArray.GetCount(); i++)
         pmEditTextCtrl->AppendText(secondaryBodiesArray.Item(i) + " ");
   }

   UpdatePrimaryBodyComboBoxList();
}

//------------------------------------------------------------------------------
// void DisplayMagneticFieldData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayMagneticFieldData()
{
//   if (primaryBodyList[currentBodyId]->magfType == magfModelArray[NONE_MM])
   if (primaryBodyData->magfType == magfModelArray[NONE_MM])
   {
      theMagfModelComboBox->SetSelection(NONE_MM);
   }
}


//------------------------------------------------------------------------------
// void DisplaySRPData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplaySRPData()
{
   theSrpCheckBox->SetValue(usePropOriginForSrp);
   theSRPModelComboBox->SetValue(srpModelName);
   if (usePropOriginForSrp)
      theSRPModelComboBox->Enable(true);
   else
      theSRPModelComboBox->Enable(false);
}


//------------------------------------------------------------------------------
// void DisplayErrorControlData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayErrorControlData()
{
   #ifdef DEBUG_PROP_PANEL_DISPLAY
   MessageInterface::ShowMessage("On DisplayErrorControlData()\n");
   #endif

   wxString wxEcTypeName = errorControlTypeName.c_str();

   if (wxEcTypeName == errorControlArray[NONE_EC])
      theErrorComboBox->SetSelection(NONE_EC);
   else if (wxEcTypeName == errorControlArray[RSSSTEP])
      theErrorComboBox->SetSelection(RSSSTEP);
   else if (wxEcTypeName == errorControlArray[RSSSTATE])
      theErrorComboBox->SetSelection(RSSSTATE);
   else if (wxEcTypeName == errorControlArray[LARGESTSTEP])
      theErrorComboBox->SetSelection(LARGESTSTEP);
   else if (wxEcTypeName == errorControlArray[LARGESTSTATE])
      theErrorComboBox->SetSelection(LARGESTSTATE);
}


//------------------------------------------------------------------------------
// void EnablePrimaryBodyItems(bool enable = true, bool clear = false)
//------------------------------------------------------------------------------
void PropagationConfigPanel::EnablePrimaryBodyItems(bool enable, bool clear)
{
   if (enable)
   {
      theGravModelComboBox->Enable(true);
      gravityDegreeTextCtrl->Enable(true);
      gravityMaxDegreeTextCtrl->Enable(true);
      gravityOrderTextCtrl->Enable(true);
      gravityMaxOrderTextCtrl->Enable(true);
      gravityStmLimitTextCtrl->Enable(true);

      if (theGravModelComboBox->GetStringSelection() == "None")
      {
         theGravModelSearchButton->Enable(false);
         potFileTextCtrl->Enable(false);
         theTideDataComboBox->Enable(false);
         theTideModelComboBox->Enable(false);
         tideTextCtrl->Enable(false);
         tideFileTextCtrl->Enable(false);
         theTideModelSearchButton->Enable(false);
      }
      else
      {
         theGravModelSearchButton->Enable(true);
         potFileTextCtrl->Enable(true);
         theTideDataComboBox->Enable(true);

         if (theTideDataComboBox->GetStringSelection() != "None")
         {
            theTideModelComboBox->Enable(true);
            tideTextCtrl->Enable(true);
            tideFileTextCtrl->Enable(true);
            theTideModelSearchButton->Enable(true);
         }
         else
         {
            theTideModelComboBox->Enable(false);
            tideTextCtrl->Enable(false);
            tideFileTextCtrl->Enable(false);
            theTideModelSearchButton->Enable(false);
         }
      }

      StringArray models = theGuiInterpreter->GetListOfFactoryItems(
            Gmat::ATMOSPHERE, thePrimaryBodyComboBox->GetValue().c_str());
      Integer atmosCount = models.size();
      if (atmosCount > 0)
      {
         theAtmosModelComboBox->Enable(true);
         if ((primaryBodyData->bodyName == "Mars") ||
             (theAtmosModelComboBox->GetValue() == dragModelArray[NONE_DM] //||
             //theAtmosModelComboBox->GetValue() == dragModelArray[EXPONENTIAL]
             ))
         {
            theDragSetupButton->Enable(false);
            theDragModelComboBox->Enable(false);
         }
         else
         {
            theDragSetupButton->Enable(true);
            theDragModelComboBox->Enable(true);
         }
      }
      else
      {
         theAtmosModelComboBox->Enable(false);
         theDragSetupButton->Enable(false);
         theDragModelComboBox->Enable(false);
      }

      //theMagfModelComboBox->Enable(true);
      //theSrpCheckBox->Enable(true);
   }
   else
   {
      if (clear)
      {
//         primaryBodyList.clear();
         if (primaryBodyData != NULL)
         {
            delete primaryBodyData;
            primaryBodyData = NULL;
         }
//         primaryBodiesArray.Clear();
         primaryBody = "None";
//         thePrimaryBodyComboBox->Clear();
//         bodyTextCtrl->Clear();
      }

      theGravModelComboBox->Enable(false);
      gravityDegreeTextCtrl->Enable(false);
      gravityMaxDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
      gravityMaxOrderTextCtrl->Enable(false);
      gravityStmLimitTextCtrl->Enable(false);
      potFileTextCtrl->Enable(false);
      theTideDataComboBox->Enable(false);
      theTideModelComboBox->Enable(false);
      tideTextCtrl->Enable(false);
      tideFileTextCtrl->Enable(false);
      theGravModelSearchButton->Enable(false);
      theTideModelSearchButton->Enable(false);
      theAtmosModelComboBox->Enable(false);
      theDragModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
      //theMagfModelComboBox->Enable(false);
      //theSrpCheckBox->Enable(false);
   }

   // Only enable atmosphere settings if the primary body is selected
   if (primaryBodyString == "None")
   {
      theAtmosModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
      theDragModelComboBox->Enable(false);
   }
   if (primaryBodyString == "Mars")
      theDragSetupButton->Enable(false);
}


//------------------------------------------------------------------------------
// void UpdatePrimaryBodyItems()
//------------------------------------------------------------------------------
void PropagationConfigPanel::UpdatePrimaryBodyItems()
{
   // Reworked for only one primary allowed; the multiple-body code is
   // commented out below
   mDataChanged = true;

   wxString selBody = thePrimaryBodyComboBox->GetStringSelection();

   if (currentBodyName != selBody)
   {
      primaryBodyString = thePrimaryBodyComboBox->GetStringSelection();
      currentBodyName = selBody;
      currentBodyId = FindPrimaryBody(currentBodyName);
      primaryBody = selBody;

      isForceModelChanged = true;

      if (currentBodyName == "None")
      {
         EnablePrimaryBodyItems(false, true);
         // Delete primary body data
         if (primaryBodyData != NULL)
         {
            delete primaryBodyData;
            primaryBodyData = NULL;
         }
      }
      else
      {
         EnablePrimaryBodyItems();
         DisplayGravityFieldData(currentBodyName);
         DisplayAtmosphereModelData();
         DisplayMagneticFieldData();
         DisplaySRPData();
         isForceModelChanged = true;
      }
   }

//   if (primaryBodiesArray.IsEmpty())
//      return;
//
//   wxString selBody = thePrimaryBodyComboBox->GetStringSelection();
//
//   if (currentBodyName != selBody)
//   {
//      primaryBodyString = thePrimaryBodyComboBox->GetStringSelection();
//      currentBodyName = selBody;
//      currentBodyId = FindPrimaryBody(currentBodyName);
//
//      DisplayGravityFieldData(currentBodyName);
//      DisplayAtmosphereModelData();
//      DisplayMagneticFieldData();
//      DisplaySRPData();
//   }
}
//------------------------------------------------------------------------------
// void PropagationConfigPanel::UpdatePrimaryBodyComboBoxList()
//------------------------------------------------------------------------------
/**
 * This method updates the list of bodies available for use as the primary body.
 *
 * The thePrimaryBodyComboBox widget for release 1.0 of GMAT is used to select
 * a primary body for the single allowed source of full field gravity (a 1.0
 * restriction that might be removed in a later release).  This method populates
 * the combobox with the available bodies, omitting any that are used as point
 * masses and adding an option to select "None", indicating that there is no
 * ful field gravity in teh force model.
 */
//------------------------------------------------------------------------------
void PropagationConfigPanel::UpdatePrimaryBodyComboBoxList()
{
//   wxArrayString cBodies = theGuiManager->GetConfigBodyList();
   thePrimaryBodyComboBox->Clear();
   thePrimaryBodyComboBox->Append("None");

   // wcs 2012.04.11 - display only None and current central body name (if not on point mass list) for now
   if (FindPointMassBody(propOriginName) == -1)
      thePrimaryBodyComboBox->Append(propOriginName);

   // #ifdef DEBUG_PROP_PANEL_GRAV
   //    MessageInterface::ShowMessage("Celestial bodies:\n");
   // #endif

   // for (UnsignedInt i = 0; i < cBodies.GetCount(); ++i)
   // {
   //    if (secondaryBodiesArray.Index(cBodies[i]) == wxNOT_FOUND)
   //    {
   //       #ifdef DEBUG_PROP_PANEL_GRAV
   //          MessageInterface::ShowMessage("   %s\n", cBodies[i].c_str());
   //       #endif
   //       thePrimaryBodyComboBox->Append(cBodies[i].c_str());
   //    }
   // }
}


//------------------------------------------------------------------------------
// bool ValidatePropEpochComboBoxes()
//------------------------------------------------------------------------------
/**
 * Ensures consistency in propagatorEpochFormatComboBox and startEpochCombobox
 *
 * This method checks the setting in the startEpochComboBox.  If the field isn't
 * a fixed value like "FromSpacecraft" or "EphemStart", it validates that the
 * text in the combobox is formatted consistently with the format selected in
 * the propagatorEpochFormatComboBox.
 *
 * @return true is the fields are consistent, false if not.
 */
//------------------------------------------------------------------------------
bool PropagationConfigPanel::ValidatePropEpochComboBoxes()
{
   bool retval = false;

   wxString startEpochValue = startEpochCombobox->GetValue();
   Real fromVal, toVal;
   std::string newStr;

   try
   {
      if (startEpochChoices.Index(startEpochValue) == wxNOT_FOUND)
      {
         #ifdef DEBUG_PROP_PROPAGATOR
            MessageInterface::ShowMessage("Checking user set epoch: %s\n",
                  startEpochValue.c_str());
         #endif
         std::string prevFmt = spkEpFormat.WX_TO_STD_STRING;
         std::string prevVal = startEpochValue.WX_TO_STD_STRING;
         TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt,prevVal, true);

         if (spkEpFormat.Find("ModJulian") == wxNOT_FOUND)
         {
            fromVal = -999.999;
            TimeSystemConverter::Instance()->Convert(spkEpFormat.c_str(), fromVal,
                  startEpochValue.c_str(), "A1ModJulian", toVal,
                  newStr);
            retval = true;
         }
         else
         {
            Real val = startEpochValue.ToDouble(&fromVal);
            if (val > 0.0)
               retval = true;
         }
      }
      else
      {
         retval = true;
      }
      #ifdef DEBUG_PROP_PROPAGATOR
         MessageInterface::ShowMessage("Ready to save epoch setting: %s\n",
               startEpochValue.c_str());
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }


   return retval;
}

//------------------------------------------------------------------------------
// bool SaveIntegratorData()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveIntegratorData()
{
   bool retval = false;

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveIntegratorData() entered\n");
   #endif

   Integer maxAttempts;
   Real initStep, accuracy, minStep, maxStep, minError, nomError;
   std::string str;

   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   str = initialStepSizeTextCtrl->GetValue();
   CheckReal(initStep, str, "InitialStepSize", "Real Number");

   str = accuracyTextCtrl->GetValue();
   CheckReal(accuracy, str, "Accuracy", "Real Number > 0", false, true, true);

   str = minStepTextCtrl->GetValue();
   CheckReal(minStep, str, "Min Step Size", "Real Number >= 0, MinStep <= MaxStep",
             false, true, true, true);

   str = maxStepTextCtrl->GetValue();
   CheckReal(maxStep, str, "Max Step Size", "Real Number > 0, MinStep <= MaxStep",
             false, true, true);

   str = maxStepAttemptTextCtrl->GetValue();
   CheckInteger(maxAttempts, str, "Max Step Attempts", "Integer Number > 0",
                false, true, true);

   if (thePropagator->IsOfType("PredictorCorrector"))
   {
      str = minIntErrorTextCtrl->GetValue();
      CheckReal(minError, str, "Min Integration Error", "Real Number > 0",
                false, true, true);

      str = nomIntErrorTextCtrl->GetValue();
      CheckReal(nomError, str, "Nominal Integration Error", "Real Number > 0",
                false, true, true);
   }

   try
   {
      if (minStep > maxStep)
         throw GmatBaseException("The step control values are invalid; "
               "Integrators require 0.0 <= Min Step Size <= Max Step Size");

      if (thePropagator->IsOfType("PredictorCorrector"))
      {
         // Check the error control settings
         if ((minError > nomError) ||
             (nomError > accuracy) ||
             (minError > accuracy))
            throw GmatBaseException("The step control values are invalid; "
                  "Predictor-Corrector Integrators require 0.0 < Min "
                  "Integration Error < Nominal Integration Error < Accuracy");
      }

   }
   catch (GmatBaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }

   if (!canClose)
      return false;

   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;

      id = thePropagator->GetParameterID("InitialStepSize");
      thePropagator->SetRealParameter(id, initStep);

      id = thePropagator->GetParameterID("Accuracy");
      thePropagator->SetRealParameter(id, accuracy);

      id = thePropagator->GetParameterID("MinStep");
      thePropagator->SetRealParameter(id, minStep);

      id = thePropagator->GetParameterID("MaxStep");
      thePropagator->SetRealParameter(id, maxStep);

      id = thePropagator->GetParameterID("MaxStepAttempts");
      thePropagator->SetIntegerParameter(id, maxAttempts);

//      if (integratorString.IsSameAs(integratorArray[ABM]))
      if (thePropagator->IsOfType("PredictorCorrector"))
      {
         id = thePropagator->GetParameterID("LowerError");
         thePropagator->SetRealParameter(id, minError);

         id = thePropagator->GetParameterID("TargetError");
         thePropagator->SetRealParameter(id, nomError);
      }

      if (thePropagator->IsOfType("Integrator"))
      {
         id = thePropagator->GetParameterID("StopIfAccuracyIsViolated");
         thePropagator->SetBooleanParameter(id, stopOnAccViolation);
      }

      #ifdef DEBUG_PROP_PANEL_SAVE
      ShowPropData("SaveData() AFTER  saving Integrator");
      #endif

      retval = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SavePropagatorData()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SavePropagatorData()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::SavePropagatorData() entered\n");
   #endif

   Real step;
   std::string str;

   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   str = propagatorStepSizeTextCtrl->GetValue().c_str();
   CheckReal(step, str, "StepSize", "Real Number");

   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;

      id = thePropagator->GetParameterID("StepSize");
      if (step <= 0.0)
         throw GmatBaseException("Step size must be a real positive number");
      thePropagator->SetRealParameter(id, step);

      str = propCentralBodyComboBox->GetValue().c_str();
      id = thePropagator->GetParameterID("CentralBody");
      thePropagator->SetStringParameter(id, str);

      // Range check the epoch value

      std::string prevFmt = spkEpFormat.WX_TO_STD_STRING;
      std::string prevVal = startEpochCombobox->GetValue().WX_TO_STD_STRING;

      if (startEpochChoices.Index(prevVal.c_str()) == wxNOT_FOUND)
         TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt, prevVal, true);

      wxString startEpochValue = startEpochCombobox->GetValue();

      if (ValidatePropEpochComboBoxes())
      {
         str = propagatorEpochFormatComboBox->GetValue().c_str();
         id = thePropagator->GetParameterID("EpochFormat");
         thePropagator->SetStringParameter(id, str);

         str = startEpochValue.c_str();
         id = thePropagator->GetParameterID("StartEpoch");
         thePropagator->SetStringParameter(id, str);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return false;
   }

   #ifdef DEBUG_PROP_PANEL_SAVE
   ShowPropData("SaveData() AFTER  saving Propagator");

   MessageInterface::ShowMessage("<%p> and <%p>\n", thePropagator, theForceModel);

   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool SaveDegOrder()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveDegOrder()
{
   bool retval = false;

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveDegOrder() entered\n");
   #endif

   Integer degree, order, stmlimit;
   std::string str;

   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   
   str = gravityDegreeTextCtrl->GetValue();
   CheckInteger(degree, str, "Degree", "Integer Number >= 0 "
            "and < the maximum specified by the model, Order <= Degree].");

   str = gravityOrderTextCtrl->GetValue();
   CheckInteger(order, str, "Order", "Integer Number >= 0"
            " and < the maximum specified by the model, Order <= Degree].");

   str = gravityStmLimitTextCtrl->GetValue();
   CheckInteger(stmlimit, str, "StmLimit", "Integer Number >= 0 "
            "and < Infinity");

   if (!canClose)
      return false;

   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage("   degree=%d, order=%d\n", degree, order);
      #endif

      // check to see if degree is less than order
      if (degree < order)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Degree can not be less than Order.\n"
             "The allowed values are: [Integer >= 0 "
             "and < the maximum specified by the model, "
             "Order <= Degree].");
         canClose = false;
         return false;
      }

      // save degree and order
      wxString bodyName = thePrimaryBodyComboBox->GetValue();

      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage("   bodyName=%s\n", bodyName.WX_TO_C_STRING);
      #endif

      // find gravity force pointer
//      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      if (primaryBodyData != NULL)
      {
//         if (primaryBodyList[i]->gravType != "None")
         if (primaryBodyData->gravType != "None")
         {
//            theGravForce = primaryBodyList[i]->gravf;
            theGravForce = primaryBodyData->gravf;
//            if (theGravForce != NULL && primaryBodyList[i]->bodyName == bodyName)
            if (theGravForce != NULL && primaryBodyData->bodyName == bodyName)
            {
               theCelestialBody = theSolarSystem->GetBody(primaryBodyData->bodyName);
               HarmonicGravity* hg = GravityField::GetHarmonicGravity
                  (std::string(primaryBodyData->potFileFullPath.c_str()),
                  std::string(primaryBodyData->tideFileFullPath.c_str()),
                  theCelestialBody->GetGravitationalConstant(),
                  theCelestialBody->GetEquatorialRadius(),
                  std::string(primaryBodyData->bodyName),false);
               if (hg != NULL)
                  {
                  theGravForce->SetIntegerParameter("MaxDegree", hg->GetNN());
                  theGravForce->SetIntegerParameter("MaxOrder", hg->GetMM());
                  }
               theGravForce->SetIntegerParameter("Degree", degree);
               theGravForce->SetIntegerParameter("Order", order);
               theGravForce->SetIntegerParameter("StmLimit", stmlimit);

               isDegOrderChanged = false;
               retval = true;
               delete hg;
            }
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }

   return retval;
}
//------------------------------------------------------------------------------
// bool SavePotFile()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SavePotFile()
{
   bool retval = false;

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SavePotFile() entered\n");
   #endif

   // save data to core engine
   try
   {
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                         thePropSetup->GetName() + "\" is not an allowed value.  "
                        "\nThe allowed values are: [ %s ].";

      //      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      if (primaryBodyData != NULL)
      {
//         if (primaryBodyList[i]->gravType != "None")
         if (primaryBodyData->gravType != "None")
         {
//            theGravForce = primaryBodyList[i]->gravf;
            theGravForce = primaryBodyData->gravf;
            if (theGravForce != NULL)
            {
               #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage
                    ("SavePotFile() Saving Body:%s, potFile=%s\n   potFileFullPath=%s\n",
//                          primaryBodyList[i]->bodyName.WX_TO_C_STRING, primaryBodyList[i]->potFile.WX_TO_C_STRING);
                     primaryBodyData->bodyName.WX_TO_C_STRING,
                     primaryBodyData->potFile.WX_TO_C_STRING, primaryBodyData->potFileFullPath.WX_TO_C_STRING);
               #endif

//               inputString = primaryBodyList[i]->potFile.WX_TO_C_STRING;
               // Use full path pot file for checking for existence (LOJ: 2014.06.30)
               //inputString = primaryBodyData->potFile.WX_TO_C_STRING;
               inputString = primaryBodyData->potFileFullPath.WX_TO_C_STRING;
               #ifdef DEBUG_PROP_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   primaryBodyData->potFileFullPath = '%s'\n", inputString.c_str());
               #endif
               
               // Changed to use base code to check for potential file (LOJ: 2014.11.05)
               bool update = theGravForce->IsParameterValid("PotentialFile",
                                                            primaryBodyData->potFile.WX_TO_STD_STRING);
               
               // std::ifstream filename(inputString.WX_TO_C_STRING);              
               // bool update = true;
               // if (!filename)
               // {
               //    update = false;
               //    throw PropagatorException("The potential file \"" +
               //          inputString + "\" cannot be found; please enter a "
               //          "valid file name and path");
               // }
               // filename.close();
               
               if (update)
               {
                  #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage
                     ("   Calling theGravForce->SetStringParameter('PotentialFile')\n");
                  #endif
                  theGravForce->SetStringParameter("PotentialFile",
//                        primaryBodyList[i]->potFile.WX_TO_C_STRING);
                        primaryBodyData->potFile.WX_TO_C_STRING);
                  retval = true;
                  isPotFileChanged = false;
               }
            }
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }
   
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SavePotFile() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool SaveTideFile()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveTideFile()
{
   bool retval = false;

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveTideFile() entered\n");
   #endif

   // save data to core engine
   try
   {
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                         thePropSetup->GetName() + "\" is not an allowed value.  "
                        "\nThe allowed values are: [ %s ].";

      //      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      if (primaryBodyData != NULL)
      {
//         if (primaryBodyList[i]->gravType != "None")
         if (primaryBodyData->gravType != "None")
         {
//            theGravForce = primaryBodyList[i]->gravf;
            theGravForce = primaryBodyData->gravf;
            if (theGravForce != NULL)
            {
               #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage
                    ("SaveTideFile() Saving Body:%s, tideFile=%s\n   tideFileFullPath=%s\n",
                     primaryBodyData->bodyName.WX_TO_C_STRING,
                     primaryBodyData->tideFile.WX_TO_C_STRING, primaryBodyData->tideFileFullPath.WX_TO_C_STRING);
               #endif

               inputString = primaryBodyData->tideFileFullPath.WX_TO_C_STRING;
               #ifdef DEBUG_PROP_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   primaryBodyData->tideFileFullPath = '%s'\n", inputString.c_str());
               #endif
               
               bool update = theGravForce->IsParameterValid("TideFile",
                   primaryBodyData->tideFile.WX_TO_STD_STRING);
               
               if (update)
               {
                  #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage
                     ("   Calling theGravForce->SetStringParameter('TideFile')\n");
                  #endif
                  theGravForce->SetStringParameter("TideFile",
//                        primaryBodyList[i]->tideFile.WX_TO_C_STRING);
                        primaryBodyData->tideFile.WX_TO_C_STRING);
                  retval = true;
                  isTideFileChanged = false;

                  if (primaryBodyData->tideFile != "")
                  {
                     primaryBodyData->tideData = "Tide File";
                     theTideDataComboBox->SetValue(primaryBodyData->tideData);
                  }
               }
            }
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }
   
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveTideFile() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool SaveTideModel()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveTideModel()
   {
   bool retval = false;

   try
      {
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                         thePropSetup->GetName() + "\" is not an allowed value.  "
                        "\nThe allowed values are: [ %s ].";

      //      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      if (primaryBodyData != NULL)
         {
         inputString = primaryBodyData->potFileFullPath.WX_TO_C_STRING;
         bool update = theGravForce->IsParameterValid("TideModel",
                          primaryBodyData->tideModel.WX_TO_STD_STRING);
              
               
         if (update)
            {
            theGravForce->SetStringParameter("TideModel",
               primaryBodyData->tideModel.WX_TO_C_STRING);
            retval = true;
            isTideModelChanged = false;
            }
         }
      }
   catch (BaseException &e)
      {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
      }
   
   return retval;
   }
//------------------------------------------------------------------------------
// bool SaveAtmosModel()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveAtmosModel()
{
   bool retval = false;

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveAtmosModel() entered\n");
   #endif

   //-------------------------------------------------------
   // find drag force model
   //-------------------------------------------------------
   Integer paramId;
   bool dragForceFound = false;

   wxString bodyName = thePrimaryBodyComboBox->GetValue();
   wxString dragType = theAtmosModelComboBox->GetValue();

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("   bodyName=%s, dragType=%s\n", bodyName.WX_TO_C_STRING, dragType.WX_TO_C_STRING);
   #endif

//   for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
   if (primaryBodyData != NULL)
   {
//      if (primaryBodyList[i]->dragType != dragModelArray[NONE_DM])
      if (primaryBodyData->dragType != dragModelArray[NONE_DM])
      {
//         theDragForce = primaryBodyList[i]->dragf;
         theDragForce = primaryBodyData->dragf;
//         if (theDragForce != NULL && primaryBodyList[i]->bodyName == bodyName)
         if (theDragForce != NULL && primaryBodyData->bodyName == bodyName)
         {
            dragForceFound = true;
//            break;
         }
      }
   }

   if (!dragForceFound)
   {
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::SaveAtmosModel() Drag Force not found "
          "for body:%s\n", bodyName.WX_TO_C_STRING);

      return false;
   }

   theCelestialBody = theSolarSystem->GetBody(bodyName.c_str());
   theAtmosphereModel = theCelestialBody->GetAtmosphereModel();

   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveAtmosModel() theCelestialBody=<%p>'%s', "
       "theAtmosphereModel=<%p>'%s'\n", theCelestialBody,
       theCelestialBody->GetName().c_str(), theAtmosphereModel, theAtmosphereModel ?
       theAtmosphereModel->GetName().c_str() : "NULL");
   #endif

   if (theAtmosphereModel == NULL)
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::SaveAtmosModel() AtmosphereModel not found "
          "for body '%s'\n", bodyName.WX_TO_C_STRING);
      #endif
   }

   //-------------------------------------------------------
   // save drag force model
   //-------------------------------------------------------
   try
   {
      if (theAtmosphereModel != NULL)
         theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
      paramId = theDragForce->GetParameterID("AtmosphereModel");
      theDragForce->SetStringParameter(paramId, dragType.WX_TO_C_STRING);
      theDragForce->SetStringParameter("BodyName", bodyName.WX_TO_C_STRING);

      #ifdef DEBUG_PROP_PANEL_ATMOS
            MessageInterface::ShowMessage("Drag buffer %s\n",
                  dragBufferReady ? "Ready" : "Not Ready");
      #endif

      if (dragBufferReady)
      {
         theDragForce->SetRealParameter("F107", dragParameterBuffer[0]);
         theDragForce->SetRealParameter("F107A", dragParameterBuffer[1]);
         theDragForce->SetRealParameter("MagneticIndex", dragParameterBuffer[2]);
      }

      isAtmosChanged = false;
      canClose = true;
      retval = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      retval = false;
   }

   return retval;
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString PropagationConfigPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// void OnIntegratorComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorComboBox(wxCommandEvent &event)
{
   if (!integratorString.IsSameAs(theIntegratorComboBox->GetStringSelection()))
   {
      isIntegratorChanged = true;
      integratorString = theIntegratorComboBox->GetStringSelection();
      DisplayIntegratorData(true);
      isIntegratorDataChanged = false;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnPrimaryBodyComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPrimaryBodyComboBox(wxCommandEvent &event)
{
   if (primaryBody != thePrimaryBodyComboBox->GetStringSelection())
      EnableUpdate(true);

   UpdatePrimaryBodyItems();
}


//------------------------------------------------------------------------------
// void OnOriginComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnOriginComboBox(wxCommandEvent &event)
{
   // get the current primary body selection
   wxString selBody = thePrimaryBodyComboBox->GetStringSelection();
   // get the updated central body selection
   propOriginName = theOriginComboBox->GetValue();
   // Update the primary body list to only display None and the central body
   UpdatePrimaryBodyComboBoxList();

   // Determine whether to set None or the central body as the selection
   if ((selBody.IsSameAs(propOriginName)) && (FindPointMassBody(propOriginName) == -1))
      thePrimaryBodyComboBox->SetValue(propOriginName);
   else
      thePrimaryBodyComboBox->SetValue("None");

   UpdatePrimaryBodyItems();

   isOriginChanged = true;

   // Only enable atmosphere settings if the primary body is selected
   if (thePrimaryBodyComboBox->GetValue() == "None")
   {
      theAtmosModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
      theDragModelComboBox->Enable(false);
   }
   if (thePrimaryBodyComboBox->GetValue() == "Mars")
      theDragSetupButton->Enable(false);

   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnGravityModelComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityModelComboBox(wxCommandEvent &event)
{
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage("OnGravityModelComboBox() entered\n");
   #endif
///   if (primaryBodiesArray.IsEmpty())
   if (primaryBody == "None")
      return;

   gravTypeName = theGravModelComboBox->GetStringSelection();

//   if (primaryBodyList[currentBodyId]->gravType != gravTypeName)
   if (primaryBodyData->gravType != gravTypeName)
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("OnGravityModelComboBox() grav changed from=%s to=%s for body=%s\n",
//          primaryBodyList[currentBodyId]->gravType.WX_TO_C_STRING, gravTypeName.WX_TO_C_STRING,
//          primaryBodyList[currentBodyId]->bodyName.WX_TO_C_STRING);
            primaryBodyData->gravType.WX_TO_C_STRING, gravTypeName.WX_TO_C_STRING,
            primaryBodyData->bodyName.WX_TO_C_STRING);
      #endif

//      primaryBodyList[currentBodyId]->gravType = gravTypeName;
      primaryBodyData->gravType = gravTypeName;

      if (gravTypeName != "None" && gravTypeName != "Other")
      {
         std::string fileType = theFileMap[gravTypeName].WX_TO_STD_STRING;
         #ifdef DEBUG_PROP_PANEL_GRAV
         MessageInterface::ShowMessage("   Found fileType = %s\n", fileType.c_str());
         #endif
         try
         {
//            primaryBodyList[currentBodyId]->potFile =
            primaryBodyData->potFile =
               theGuiInterpreter->GetFileName(fileType).c_str();
            primaryBodyData->potFileFullPath =
               theGuiInterpreter->GetFileName(fileType, true).c_str();
            #ifdef DEBUG_PROP_PANEL_GRAV
            MessageInterface::ShowMessage
               ("   potFileFullPath = '%s'\n", primaryBodyData->potFileFullPath.WX_TO_C_STRING);
            #endif
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, e.GetFullMessage() +
                "\nPlease select Other and specify file name\n");
         }
      }
      else if (gravTypeName == "Other")
      {
//         primaryBodyList[currentBodyId]->potFile = potFileTextCtrl->GetValue();
         std::string potFile = potFileTextCtrl->GetValue().WX_TO_STD_STRING;
         std::string potFileFullPath = potFile;
         std::string potFileType = GmatStringUtil::ToUpper(primaryBodyData->bodyName.WX_TO_C_STRING) + "_POT_PATH";

         primaryBodyData->potFile = potFileTextCtrl->GetValue();
         // Added to fix GMT-4651 (LOJ: 2014.07.31)
         primaryBodyData->potFileFullPath =
             GmatBase::GetFullPathFileName(potFileFullPath, primaryBodyData->bodyName.WX_TO_C_STRING,
             potFile, potFileType, true, "", false, true);
      }

      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("OnGravityModelComboBox() bodyName=%s\n   potFile=%s\n   potFileFullPath=%s\n",
//          primaryBodyList[currentBodyId]->bodyName.WX_TO_C_STRING,
//          primaryBodyList[currentBodyId]->potFile.WX_TO_C_STRING);
          primaryBodyData->bodyName.WX_TO_C_STRING,
          primaryBodyData->potFile.WX_TO_C_STRING, primaryBodyData->potFile.WX_TO_C_STRING);
      MessageInterface::ShowMessage("OnGravityModelComboBox() Calling DisplayGravityFieldData()\n");
      #endif

//      DisplayGravityFieldData(primaryBodyList[currentBodyId]->bodyName);
      DisplayGravityFieldData(primaryBodyData->bodyName);

      isPotFileChanged = true;
      EnableUpdate(true);
   }
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage("OnGravityModelComboBox() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTideDataComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnTideDataComboBox(wxCommandEvent &event)
{
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage("OnTideDataComboBox() entered\n");
   #endif

   if (primaryBody == "None")
      return;

   tideDataName = theTideDataComboBox->GetStringSelection();

   if (primaryBodyData->tideData != tideDataName)
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("OnTideDataComboBox() tideData changed from=%s to=%s for body=%s\n",
            primaryBodyData->tideData.WX_TO_C_STRING, tideDataName.WX_TO_C_STRING,
            primaryBodyData->bodyName.WX_TO_C_STRING);
      #endif

      primaryBodyData->tideData = tideDataName;

      if (tideDataName == "None")
      {
         primaryBodyData->tideModel = "None";
         primaryBodyData->tideFile = "";
         primaryBodyData->tideFileFullPath = "";

         isTideModelChanged = true;
         isTideFileChanged = true;
      }
      else if (tideDataName != "Tide File")
      {
         primaryBodyData->tideFile = "";
         primaryBodyData->tideFileFullPath = "";

         isTideFileChanged = true;
      }

      DisplayGravityFieldData(primaryBodyData->bodyName);
      EnableUpdate(true);
   }
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage("OnTideDataComboBox() leaving\n");
   #endif
}
//------------------------------------------------------------------------------
// void OnTideModelComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnTideModelComboBox(wxCommandEvent &event)
   {
   if (primaryBody == "None")
      return;

   tideModelName = theTideModelComboBox->GetStringSelection();

   if (primaryBodyData->tideModel != tideModelName)
      {

      primaryBodyData->tideModel = tideModelName;

      isTideModelChanged = true;
      EnableUpdate(true);
      }
   }
//------------------------------------------------------------------------------
// void OnAtmosphereModelComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAtmosphereModelComboBox(wxCommandEvent &event)
{
//   if (primaryBodiesArray.IsEmpty())
   if (primaryBody == "None")
      return;

   #ifdef DEBUG_PROP_PANEL_ATMOS
   MessageInterface::ShowMessage("OnAtmosphereModelComboBox() body=%s\n",
                                 primaryBody.WX_TO_C_STRING);
   #endif

   dragTypeName = theAtmosModelComboBox->GetStringSelection();

   // if we are creating new DragForce, set isForceModelChanged
//   if (primaryBodyList[currentBodyId]->dragf == NULL)
   if (primaryBodyData->dragf == NULL)
      isForceModelChanged = true;

//   if (primaryBodyList[currentBodyId]->dragType != dragTypeName)
   if (primaryBodyData->dragType != dragTypeName)
   {
      #ifdef DEBUG_PROP_PANEL_ATMOS
      MessageInterface::ShowMessage
         ("OnAtmosphereModelComboBox() drag changed from=%s to=%s for body=%s\n",
          primaryBodyData->dragType.WX_TO_C_STRING, dragTypeName.WX_TO_C_STRING,
          primaryBody.WX_TO_C_STRING);
      #endif

//      primaryBodyList[currentBodyId]->dragType = dragTypeName;
      primaryBodyData->dragType = dragTypeName;
      DisplayAtmosphereModelData();

      isForceModelChanged = true;
      isAtmosChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnErrorControlComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnErrorControlComboBox(wxCommandEvent &event)
{
   #ifdef DEBUG_PROP_PANEL_ERROR
   MessageInterface::ShowMessage("OnErrorControlComboBox()\n");
   #endif

   wxString eType = theErrorComboBox->GetStringSelection();

   if (errorControlTypeName != eType)
   {
      #ifdef DEBUG_PROP_PANEL_ERROR
      MessageInterface::ShowMessage
         ("OnErrorControlComboBox() error control changed from=%s to=%s\n",
          errorControlTypeName.WX_TO_C_STRING, eType.WX_TO_C_STRING);
      #endif

      errorControlTypeName = eType;
      DisplayErrorControlData();

      // We don't want to create a new ForceModel if only Error Control is changed
      //isForceModelChanged = true;
      isErrControlChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnPropOriginComboBox(wxCommandEvent &)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPropOriginComboBox(wxCommandEvent &)
{
   if (thePropagator->IsOfType("Integrator"))
      return;

   wxString propSelection = propCentralBodyComboBox->GetStringSelection();

   if (!spkBody.IsSameAs(propSelection))
   {
      isIntegratorDataChanged = true;
      spkBody = propSelection;
      isSpkBodyChanged = true;
      Integer cbIndex = propCentralBodyComboBox->FindString(propSelection);
      propCentralBodyComboBox->SetSelection(cbIndex);
      DisplayIntegratorData(false);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnPropEpochComboBox(wxCommandEvent &)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPropEpochComboBox(wxCommandEvent &)
{
   if (thePropagator->IsOfType("Integrator"))
      return;

   wxString epochSelection = propagatorEpochFormatComboBox->GetStringSelection();
   std::string newStr;

   if (!spkEpFormat.IsSameAs(epochSelection))
   {
      if (startEpochChoices.Index(startEpochCombobox->GetValue()) == wxNOT_FOUND)
      {
         isIntegratorDataChanged = true;

         if (ValidatePropEpochComboBoxes())
         {
            spkEpoch = startEpochCombobox->GetValue();

            // Update the epoch string
            try
            {
               Real fromVal;
               Real toVal = -999.999;

               std::string prevFmt = spkEpFormat.WX_TO_STD_STRING;
               std::string prevVal = spkEpoch.WX_TO_STD_STRING;
               TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt,prevVal, true);

               if (spkEpFormat.Find("ModJulian") == wxNOT_FOUND)
                  fromVal = -999.999;
               else
               {
                  spkEpoch.ToDouble(&fromVal);
               }

               TimeSystemConverter::Instance()->Convert(spkEpFormat.c_str(), fromVal, spkEpoch.c_str(),
                     epochSelection.c_str(), toVal, newStr);
            }
            catch (BaseException &e)
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, e.GetFullMessage() +
                   "\nPlease enter valid Epoch before changing the Epoch Format\n");

               Integer epIndex = propagatorEpochFormatComboBox->FindString(spkEpFormat);
               propagatorEpochFormatComboBox->SetSelection(epIndex);
               return;
            }
         }
         else
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, "Please enter a valid "
                  "Epoch in the current format before changing the Epoch "
                  "Format\n");
            return;
         }
      }

      spkEpFormat = epochSelection;
      isSpkEpFormatChanged = true;
      Integer epIndex = propagatorEpochFormatComboBox->FindString(epochSelection);
      propagatorEpochFormatComboBox->SetSelection(epIndex);

      spkEpoch = newStr.c_str();
      if (startEpochChoices.Index(startEpochCombobox->GetValue()) == wxNOT_FOUND)
         isSpkEpochChanged = true;

      DisplayIntegratorData(false);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnStartEpochComboBox(wxCommandEvent &)
//------------------------------------------------------------------------------
/**
 * Tells the GUI that the start epoch combobox has changed
 */
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnStartEpochComboBox(wxCommandEvent &)
{
   if (thePropagator->IsOfType("Integrator"))
      return;

   if (startEpochChoices.Index(startEpochCombobox->GetValue()) == wxNOT_FOUND)
   {
      isIntegratorDataChanged = true;

      if (ValidatePropEpochComboBoxes())
      {
         wxString epochSelection = propagatorEpochFormatComboBox->GetStringSelection();
         std::string newStr;
         spkEpoch = startEpochCombobox->GetValue();

         // Update the epoch string
         try
         {
            Real fromVal;
            Real toVal = -999.999;

            std::string prevFmt = spkEpFormat.WX_TO_STD_STRING;
            std::string prevVal = spkEpoch.WX_TO_STD_STRING;
            TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt,prevVal, true);

            if (spkEpFormat.Find("ModJulian") == wxNOT_FOUND)
               fromVal = -999.999;
            else
            {
               spkEpoch.ToDouble(&fromVal);
            }

            TimeSystemConverter::Instance()->Convert(spkEpFormat.c_str(), fromVal,
                  spkEpoch.c_str(), epochSelection.c_str(), toVal, newStr);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, e.GetFullMessage() + "\nPlease enter valid Epoch "
                     "before changing the Epoch Format\n");

            Integer epIndex =
                  propagatorEpochFormatComboBox->FindString(spkEpFormat);
            propagatorEpochFormatComboBox->SetSelection(epIndex);
            return;
         }
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, "Please enter a valid "
               "Epoch in the current format before changing the Epoch "
               "Format\n");
         return;
      }
   }
   else
      spkEpoch = startEpochCombobox->GetValue();

   isSpkEpochChanged = true;
   DisplayIntegratorData(false);
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnSRPModelComboBox(wxCommandEvent &)
//------------------------------------------------------------------------------
/**
 * Tells the GUI that the SRP model combobox has changed
 */
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPModelComboBox(wxCommandEvent &)
{
   wxString modelSelection = theSRPModelComboBox->GetStringSelection();
   
   if (!srpModelName.IsSameAs(modelSelection))
   {
      isSRPModelChanged   = true;
      isForceModelChanged = true;
      srpModelName        = theSRPModelComboBox->GetValue();
      EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnDragModelComboBox(wxCommandEvent &)
//------------------------------------------------------------------------------
/**
 * Tells the GUI that the drag model combobox has changed
 */
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnDragModelComboBox(wxCommandEvent &)
{
   wxString modelSelection = theDragModelComboBox->GetStringSelection();
   
   #ifdef DEBUG_PROP_DRAG
      MessageInterface::ShowMessage("OnDragModelComboBox: selection = %s\n"
                                    "                     dragModelName = %s\n",
                                    modelSelection.WX_TO_C_STRING,
                                    dragModelName.WX_TO_C_STRING);
   #endif
   
   if (!dragModelName.IsSameAs(modelSelection))
   {
      isDragModelChanged   = true;
      isForceModelChanged  = true;
      dragModelName        = theDragModelComboBox->GetValue();
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnStartEpochTextChange(wxCommandEvent &)
//------------------------------------------------------------------------------
/**
 * Handles typed changes to the start epoch combobox
 */
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnStartEpochTextChange(wxCommandEvent &)
{
   isIntegratorDataChanged = true;
   isSpkEpochChanged = true;
   EnableUpdate(true);
}

// wxButton events
//------------------------------------------------------------------------------
// void OnAddBodyButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAddBodyButton(wxCommandEvent &event)
{
//   CelesBodySelectDialog bodyDlg(this, primaryBodiesArray, secondaryBodiesArray);
//   bodyDlg.ShowModal();

//   if (bodyDlg.IsBodySelected())
//   {
//      wxArrayString &names = bodyDlg.GetBodyNames();
//
//      if (names.IsEmpty())
//      {
//         EnablePrimaryBodyItems(false, true);
//
//         EnableUpdate(true);
//         isForceModelChanged = true;
//         return;
//      }
//      else
//      {
//         EnablePrimaryBodyItems(true);
//      }
//
//      std::vector<ForceType*> oldList;
//      wxString bodyName;
//
//      oldList = primaryBodyList;
//      primaryBodyList.clear();
//      primaryBodiesArray.Clear();
//      primaryBody = "None";
//
//      for (Integer i = 0; i < (Integer)names.GetCount(); i++)
//      {
//         bodyName = names[i];
//         primaryBodiesArray.Add(bodyName.c_str());
//
//         // Set default gravity model file for display
//         if (bodyName == "Earth")
//            currentBodyId = FindPrimaryBody(bodyName, true, earthGravModelArray[JGM2]);
//         else if (bodyName == "Luna")
//            currentBodyId = FindPrimaryBody(bodyName, true, lunaGravModelArray[LP165]);
//         else if (bodyName == "Venus")
//            currentBodyId = FindPrimaryBody(bodyName, true, venusGravModelArray[MGNP180U]);
//         else if (bodyName == "Mars")
//            currentBodyId = FindPrimaryBody(bodyName, true, marsGravModelArray[MARS50C]);
//         else
//            currentBodyId = FindPrimaryBody(bodyName, true, othersGravModelArray[O_NONE_GM]);
//
//
//         // Copy old body force model
//         for (Integer j = 0; j < (Integer)oldList.size(); j++)
//            if (bodyName == oldList[j]->bodyName)
//               primaryBodyList[currentBodyId] = oldList[j];
//      }
//   }

   //----------------------------------------------
   // Append body names to combobox and text field
   //----------------------------------------------
//   thePrimaryBodyComboBox->Clear();
//   bodyTextCtrl->Clear();
//   wxString name;
//   for (Integer i = 0; i < (Integer)primaryBodyList.size(); i++)
//   {
//      name = primaryBodyList[i]->bodyName.c_str();
//      thePrimaryBodyComboBox->Append(primaryBodyList[i]->bodyName.c_str());
//      thePrimaryBodyComboBox->SetValue(primaryBodyList[i]->bodyName.c_str());
//   }
   UpdatePrimaryBodyComboBoxList();
//   thePrimaryBodyComboBox->SetValue(primaryBodyList[0]->bodyName.c_str());
   thePrimaryBodyComboBox->SetValue(primaryBodyData->bodyName.c_str());

   UpdatePrimaryBodyItems();
   DisplayForceData();

   EnableUpdate(true);
   isForceModelChanged = true;
}


//------------------------------------------------------------------------------
// void OnGravSearchButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravSearchButton(wxCommandEvent &event)
{
   wxString fileFormats;
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();

   if (runmode == GmatGlobal::TESTING || runmode == GmatGlobal::TESTING_NO_PLOTS)
      fileFormats = _T("Gravity files (*.cof;*.grv;*.gfc;*.tab)|*.cof;*.grv;*.gfc;*.tab|All files (*.*)|*.*");
   else
      fileFormats = _T("Gravity files (*.cof;*.grv)|*.cof;*.grv|All files (*.*)|*.*");

   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), fileFormats);

   if (dialog.ShowModal() == wxID_OK)
      {
      wxString filename = dialog.GetPath();
      std::string tideFilename = primaryBodyData->tideFileFullPath.WX_TO_STD_STRING;

      try
         {
         theCelestialBody = theSolarSystem->GetBody(primaryBodyData->bodyName);
         HarmonicGravity* hg = GravityField::GetHarmonicGravity
           (std::string(filename.c_str()),
            std::string(tideFilename.c_str()),
            theCelestialBody->GetGravitationalConstant(),
            theCelestialBody->GetEquatorialRadius(),
            std::string(primaryBodyData->bodyName),false);
         if (hg == NULL)
            {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format.");
            return;
            }
//         primaryBodyData->gravDegree = GmatStringUtil::ToString(hg->GetNN(),4);
//         primaryBodyData->gravOrder = GmatStringUtil::ToString(hg->GetMM(),4);
//         gravityDegreeTextCtrl->SetValue(primaryBodyData->gravDegree);
//         gravityOrderTextCtrl->SetValue(primaryBodyData->gravOrder);
         delete hg;
         }
      catch (BaseException &be)
         {
         MessageInterface::PopupMessage
         (Gmat::WARNING_, "File \"" + filename + "\" is not a valid gravity file.");
         return;
         }
      GravityField::GravityModelType bodyGravModelType =
         GravityField::GetModelType(filename.c_str(), (primaryBodyData->bodyName).c_str());
      primaryBodyData->gravType = (GravityField::GRAVITY_MODEL_NAMES[bodyGravModelType]).c_str();
      
//      primaryBodyList[currentBodyId]->potFile = filename;
      primaryBodyData->potFile = filename;
      // Added to fix GMT-4651 (LOJ: 2014.07.31)
      primaryBodyData->potFileFullPath = filename; 
      
//      if (primaryBodyList[currentBodyId]->bodyName == "Earth")
//         primaryBodyList[currentBodyId]->gravType = earthGravModelArray[E_OTHER];
//      else if (primaryBodyList[currentBodyId]->bodyName == "Luna")
//         primaryBodyList[currentBodyId]->gravType = lunaGravModelArray[L_OTHER];
//      else if (primaryBodyList[currentBodyId]->bodyName == "Mars")
//         primaryBodyList[currentBodyId]->gravType = marsGravModelArray[M_OTHER];
//      else //other bodies
//         primaryBodyList[currentBodyId]->gravType = othersGravModelArray[O_OTHER];
//      if (primaryBodyData->bodyName == "Earth")
//         primaryBodyData->gravType = earthGravModelArray[E_OTHER];
//      else if (primaryBodyData->bodyName == "Luna")
//         primaryBodyData->gravType = lunaGravModelArray[L_OTHER];
//      else if (primaryBodyData->bodyName == "Mars")
//         primaryBodyData->gravType = marsGravModelArray[M_OTHER];
//      // todo Add Venus here?
//      else //other bodies
//         primaryBodyData->gravType = othersGravModelArray[O_OTHER];

      //loj: Do we need to show? body name didn't change
      //waw: Yes, we need to update the degree & order displays (10/17/06)
//      DisplayGravityFieldData(primaryBodyList[currentBodyId]->bodyName);
      DisplayGravityFieldData(primaryBodyData->bodyName, true, false);
      isDegOrderChanged = true;
      EnableUpdate(true);
      }
   }
//------------------------------------------------------------------------------
// void OnGravSaveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravSaveButton(wxCommandEvent &event)
{
   std::string filename = primaryBodyData->potFileFullPath.WX_TO_STD_STRING;

   wxFileDialog dialog(this, _T("Select save file name"), _T(""), filename + ".cof",
      _T(".cof file (*.cof)|*.cof|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

   if (dialog.ShowModal() == wxID_OK)
   {
      std::string tideFilename = primaryBodyData->tideFileFullPath.WX_TO_STD_STRING;
      try
      {
         theCelestialBody = theSolarSystem->GetBody(primaryBodyData->bodyName);
         HarmonicGravity* hg = GravityField::GetHarmonicGravity
            (std::string(filename.c_str()),
            std::string(tideFilename.c_str()),
            theCelestialBody->GetGravitationalConstant(),
            theCelestialBody->GetEquatorialRadius(),
            std::string(primaryBodyData->bodyName), true);
         if (hg == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format.");
            return;
         }

         std::string outputFile = dialog.GetPath().WX_TO_STD_STRING;
         hg->WriteCofFile(outputFile);
         delete hg;
      }
      catch (BaseException &be)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "File \"" + filename +
            "\" is not a valid gravity file because " + be.GetFullMessage());
         return;
      }
   }
}
//------------------------------------------------------------------------------
// void OnTideSearchButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnTideSearchButton(wxCommandEvent &event)
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
        _T("Tide file (*.tide)|*.tide|All files (*.*)|*.*"));

    if (dialog.ShowModal() == wxID_OK)
    {
        std::string filename = primaryBodyData->potFileFullPath.WX_TO_STD_STRING;
        wxString tideFilename = dialog.GetPath();

        try
        {
            theCelestialBody = theSolarSystem->GetBody(primaryBodyData->bodyName);
            HarmonicGravity* hg = GravityField::GetHarmonicGravity
                (std::string(filename.c_str()),
                std::string(tideFilename.c_str()),
                theCelestialBody->GetGravitationalConstant(),
                theCelestialBody->GetEquatorialRadius(),
                std::string(primaryBodyData->bodyName), false);
            if (hg == NULL)
            {
                MessageInterface::PopupMessage
                    (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format."
                                     "Fix before loading a tide file");
                return;
            }

            if (!hg->HaveTideModel(HarmonicGravity::ETide::Solid))
            {
                MessageInterface::PopupMessage
                    (Gmat::WARNING_, "Unable to load tide data from \"" + tideFilename + "\"");
                return;
            }

            delete hg;
        }
        catch (BaseException &be)
        {
            MessageInterface::PopupMessage
                (Gmat::WARNING_, "Encountered following error while loading gravity model:\n" +
                                 be.GetFullMessage());
            return;
        }
        GravityField::GravityModelType bodyGravModelType =
            GravityField::GetModelType(filename.c_str(), (primaryBodyData->bodyName).c_str());
        primaryBodyData->gravType = (GravityField::GRAVITY_MODEL_NAMES[bodyGravModelType]).c_str();

        primaryBodyData->tideData = "Tide File";
        primaryBodyData->tideFile = tideFilename;
        primaryBodyData->tideFileFullPath = tideFilename;

        //loj: Do we need to show? body name didn't change
        //waw: Yes, we need to update the degree & order displays (10/17/06)
        //      DisplayGravityFieldData(primaryBodyList[currentBodyId]->bodyName);
        DisplayGravityFieldData(primaryBodyData->bodyName, false, true);
        isTideModelChanged = true;
        EnableUpdate(true);
    }
}
//------------------------------------------------------------------------------
// void OnSetupButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSetupButton(wxCommandEvent &event)
{
   DragForce *dragForce = NULL;
   dragForce = primaryBodyData->dragf;

   // Per S. Hughes:
   std::string title = "Space Weather Setup";


   if ((dragForce != NULL) && !dragBufferReady)
   {
      dragParameterBuffer[0] = dragForce->GetRealParameter("F107");
      dragParameterBuffer[1] = dragForce->GetRealParameter("F107A");
      dragParameterBuffer[2] = dragForce->GetRealParameter("MagneticIndex");

      dragStringBuffer[0] = dragForce->GetStringParameter("PredictedWeatherSource");
      dragStringBuffer[1] = dragForce->GetStringParameter("HistoricWeatherSource");
      dragStringBuffer[2] = dragForce->GetStringParameter("CSSISpaceWeatherFile");
      dragStringBuffer[3] = dragForce->GetStringParameter("SchattenFile");
      dragStringBuffer[4] = dragForce->GetStringParameter("SchattenErrorModel");
      dragStringBuffer[5] = dragForce->GetStringParameter("SchattenTimingModel");

      dragBufferReady = true;
   }
   else if ( dragForce==NULL && !dragBufferReady)
   {
      dragStringBuffer[0] = "ConstantFluxAndGeoMag";
      dragStringBuffer[1] = "ConstantFluxAndGeoMag";

      // Default values
      FileManager *fm = FileManager::Instance();
      dragStringBuffer[2] = fm->GetFilename("CSSI_FLUX_FILE");
      dragStringBuffer[3] = fm->GetFilename("SCHATTEN_FILE");
      dragStringBuffer[4] = "Nominal";
      dragStringBuffer[5] = "NominalCycle";
   }

   // Buffer the settings 
   Real dragSettings[3];
   dragSettings[0] = dragParameterBuffer[0];
   dragSettings[1] = dragParameterBuffer[1];
   dragSettings[2] = dragParameterBuffer[2];

   // Buffer the string values
   std::vector<std::string> dragStringSettings;
   dragStringSettings = dragStringBuffer;

   DragInputsDialog dragDlg(this, dragForce, dragParameterBuffer,
         &dragStringBuffer, title.c_str());
   dragDlg.ShowModal();
   
   int numSettings =  dragStringBuffer.size();
   for (int i=0; i<numSettings; i++)
   {
       if (strcmp(dragStringSettings[i].c_str(),dragStringBuffer[i].c_str()))
       {
            dragBufferReady = true;
            EnableUpdate(true);
            isForceModelChanged=true;
       }
   } 

   if ((dragSettings[0] != dragParameterBuffer[0]) ||
       (dragSettings[1] != dragParameterBuffer[1]) ||
       (dragSettings[2] != dragParameterBuffer[2]) )
   {
      dragBufferReady = true;
      isForceModelChanged=true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnMagSearchButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagSearchButton(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;

      filename = dialog.GetPath().c_str();
      theMagfModelComboBox->Append(filename);
   }
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnPMEditButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPMEditButton(wxCommandEvent &event)
{
   wxArrayString primaryBodiesArray;
   if (primaryBody != "None")
      primaryBodiesArray.Add(primaryBody);

   CelesBodySelectDialog bodyDlg(this, secondaryBodiesArray, primaryBodiesArray);
   bodyDlg.ShowModal();

   if (bodyDlg.IsBodySelected())
   {
      wxArrayString &names = bodyDlg.GetBodyNames();

      for (Integer i=0; i<(Integer)pointMassBodyList.size(); i++)
         delete pointMassBodyList[i];

      if (names.IsEmpty())
      {
         pointMassBodyList.clear();
         secondaryBodiesArray.Clear();
         pmEditTextCtrl->Clear();
         EnableUpdate(true);
         isForceModelChanged = true;
         UpdatePrimaryBodyComboBoxList();
         // Redisplay primary body after combo box change
         DisplayPrimaryBodyData();
         return;
      }

      wxString bodyName;

      pointMassBodyList.clear();
      secondaryBodiesArray.Clear();
      pmEditTextCtrl->Clear();

      //--------------------------
      // Add bodies to pointMassBodyList
      //--------------------------
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         pointMassBodyList.push_back(new ForceType(bodyName));

         secondaryBodiesArray.Add(bodyName);
         pmEditTextCtrl->AppendText(names[i] + " ");
      }

      UpdatePrimaryBodyComboBoxList();

      EnableUpdate(true);
      isForceModelChanged = true;
   }
   
   // Redisplay primary body after combo box change
   DisplayPrimaryBodyData();
}

//------------------------------------------------------------------------------
// void OnSRPEditButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPEditButton(wxCommandEvent &event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnIntegratorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorTextUpdate(wxCommandEvent &event)
{
   //isIntegratorChanged = true;
   isIntegratorDataChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityTextUpdate(wxCommandEvent& event)
{
   #ifdef DEBUG_GRAVITY_TEXT
   MessageInterface::ShowMessage("OnGravityTextUpdate() entered\n");
   #endif
   EnableUpdate(true);

   if (event.GetEventObject() == gravityDegreeTextCtrl)
   {
//      primaryBodyList[currentBodyId]->gravDegree = gravityDegreeTextCtrl->GetValue();
      primaryBodyData->gravDegree = gravityDegreeTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityOrderTextCtrl)
   {
//      primaryBodyList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue();
      primaryBodyData->gravOrrder = gravityOrderTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityStmLimitTextCtrl)
   {
//      primaryBodyList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue();
      primaryBodyData->gravStmLimit = gravityStmLimitTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      std::string potFile = potFileTextCtrl->GetValue().WX_TO_STD_STRING;
      std::string potFileType = GmatStringUtil::ToUpper(primaryBodyData->bodyName.WX_TO_C_STRING) + "_POT_PATH";
//      primaryBodyList[currentBodyId]->potFile = potFileTextCtrl->GetValue();
      primaryBodyData->potFile = potFile;
      // Added to fix GMT-4651 (LOJ: 2014.07.31) 
      primaryBodyData->potFileFullPath =
            GmatBase::GetFullPathFileName(potFile, primaryBodyData->bodyName.WX_TO_C_STRING,
            potFile, potFileType, true, "", false, true);
      isPotFileChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   #ifdef DEBUG_GRAVITY_TEXT
   MessageInterface::ShowMessage("OnGravityTextUpdate() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTideTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnTideTextUpdate(wxCommandEvent& event)
{
   #ifdef DEBUG_TIDE_TEXT
   MessageInterface::ShowMessage("OnTideTextUpdate() entered\n");
   #endif
   EnableUpdate(true);

   primaryBodyData->tideFile = tideFileTextCtrl->GetValue();
   primaryBodyData->tideFileFullPath = tideFileTextCtrl->GetValue(); 
   isTideFileChanged = true;

   #ifdef DEBUG_TIDE_TEXT
   MessageInterface::ShowMessage("OnTideTextUpdate() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnMagneticTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagneticTextUpdate(wxCommandEvent& event)
{
   EnableUpdate(true);
   isMagfTextChanged = true;
}
//------------------------------------------------------------------------------
// void OnSRPCheckBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPCheckBoxChange(wxCommandEvent &event)
{
   usePropOriginForSrp = theSrpCheckBox->GetValue();
   isForceModelChanged = true;
   // enable or disable the SRP Model combo box here
   if (usePropOriginForSrp)
      theSRPModelComboBox->Enable(true);
   else
      theSRPModelComboBox->Enable(false);
   EnableUpdate(true);
}
//------------------------------------------------------------------------------
// void OnRelativisticCorrectionCheckBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnRelativisticCorrectionCheckBoxChange(wxCommandEvent &event)
{
   addRelativisticCorrection = theRelativisticCorrectionCheckBox->GetValue();
   isForceModelChanged       = true;
   EnableUpdate(true);
}
//------------------------------------------------------------------------------
// void OnStopCheckBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnStopCheckBoxChange(wxCommandEvent &event)
{
   stopOnAccViolation      = theStopCheckBox->GetValue();
   isIntegratorDataChanged = true;
   EnableUpdate(true);
}
//------------------------------------------------------------------------------
// void ShowPropData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowPropData(const std::string& header)
{
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("   thePropSetup =%p, name=%s\n",
                                 thePropSetup, thePropSetup->GetName().c_str());
   MessageInterface::ShowMessage("   thePropagator=%p, name=%s\n", thePropagator,
                                 thePropagator->GetTypeName().c_str());
   if ((theForceModel != NULL) && (thePropagator->IsOfType("Integrator")))
   {
      MessageInterface::ShowMessage("   theForceModel=%p, name=%s\n",
            theForceModel, (theForceModel == NULL ? "" :
            theForceModel->GetName().c_str()));
      MessageInterface::ShowMessage("   numOfForces=%d\n", numOfForces);

      std::string forceType;
      std::string forceBody;
      PhysicalModel *force;

      for (int i=0; i<numOfForces; i++)
      {
         force = theForceModel->GetForce(i);
         forceType = force->GetTypeName();

         forceBody = force->GetStringParameter("BodyName");

         MessageInterface::ShowMessage("   forceBody=%s, forceType=%s\n", forceBody.c_str(),
                                       forceType.c_str());
      }
   }
   MessageInterface::ShowMessage("============================================\n");
}
//------------------------------------------------------------------------------
// void ShowForceList()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowForceList(const std::string &header)
{
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("   theForceModel=%p\n", theForceModel);

   if (theForceModel != NULL)
   {
      // srp force
      MessageInterface::ShowMessage
         ("   usePropOriginForSrp=%d\n", usePropOriginForSrp);

      // relativistic correction
      MessageInterface::ShowMessage
         ("   addRelativisticCorrection=%d\n", addRelativisticCorrection);

      // primary body list
//      for (unsigned int i=0; i<primaryBodyList.size(); i++)
      if (primaryBodyData != NULL)
      {
//         MessageInterface::ShowMessage
//            ("   id=%d, body=%s, gravType=%s, dragType=%s, magfType=%s\n   potFile=%s\n"
//             "   gravf=%p, dragf=%p, srpf=%p\n", i, primaryBodyList[i]->bodyName.c_str(),
//             primaryBodyList[i]->gravType.c_str(), primaryBodyList[i]->dragType.c_str(),
//             primaryBodyList[i]->magfType.c_str(), primaryBodyList[i]->potFile.c_str(),
//             primaryBodyList[i]->gravf, primaryBodyList[i]->dragf,
//             primaryBodyList[i]->srpf);
         MessageInterface::ShowMessage
            ("   id=%d, body=%s, gravType=%s, dragType=%s, magfType=%s\n   potFile=%s\n"
             "   potFileFullPath=%s\n   tideFile=%s\n   tideFileFullPath=%s\n"
             "   gravf=%p, dragf=%p, srpf=%p\n", 0,
             primaryBodyData->bodyName.WX_TO_C_STRING,
             primaryBodyData->gravType.WX_TO_C_STRING, primaryBodyData->dragType.WX_TO_C_STRING,
             primaryBodyData->magfType.WX_TO_C_STRING, primaryBodyData->potFile.WX_TO_C_STRING,
             primaryBodyData->potFileFullPath.WX_TO_C_STRING,
             primaryBodyData->tideFile.WX_TO_C_STRING,
             primaryBodyData->tideFileFullPath.WX_TO_C_STRING,
             primaryBodyData->gravf, primaryBodyData->dragf,
             primaryBodyData->srpf);
      }

      // pointmass body list
      MessageInterface::ShowMessage("   pointMassBodyListSize=%d\n", pointMassBodyList.size());
      for (Integer i=0; i < (Integer)pointMassBodyList.size(); i++)
      {
         MessageInterface::ShowMessage
            ("   id=%d, body=%s, pmf=%p\n", i, pointMassBodyList[i]->bodyName.WX_TO_C_STRING,
             pointMassBodyList[i]->pmf);
      }
   }
   MessageInterface::ShowMessage("============================================\n");
}
//------------------------------------------------------------------------------
// void ShowForceModel()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowForceModel(const std::string &header)
{
   if (theForceModel != NULL)
   {
      MessageInterface::ShowMessage(">>>>>=======================================\n");
      MessageInterface::ShowMessage("%s%s\n", header.c_str(), " --- ForceModel");

      Integer numForces = theForceModel->GetNumForces();
      PhysicalModel *pm;

      for (int i = 0; i < numForces; i++)
      {
         pm = theForceModel->GetForce(i);
         MessageInterface::ShowMessage
            ("id=%d, body=%s, type=%s, addr=%p\n", i, pm->GetBodyName().c_str(),
             pm->GetTypeName().c_str(), pm);
      }
      MessageInterface::ShowMessage("============================================\n");
   }
}
//-----------------------------------------------------------------------------
// void PropagationConfigPanel::ShowIntegratorLayout(bool isIntegrator,
//-----------------------------------------------------------------------------
void PropagationConfigPanel::ShowIntegratorLayout(bool isIntegrator,
      bool isEphem)
{
   bool isPredictorCorrector = false;
   bool isCode500 = false;

   if (thePropagator != NULL)
   {
      if (thePropagator->IsOfType("PredictorCorrector"))
         isPredictorCorrector = true;
      if (thePropagator->IsOfType("Code500Propagator"))
         isCode500 = true;
   }

   if (isIntegrator)
   {
      fmStaticSizer->Show(true);
      fmStaticSizer->Hide( magfStaticSizer, true );

      initialStepSizeStaticText->Show(true);
      unitsInitStepSizeStaticText->Show(true);
      accuracyStaticText->Show(true);
      minStepStaticText->Show(true);
      unitsMinStepStaticText->Show(true);
      maxStepStaticText->Show(true);
      unitsMaxStepStaticText->Show(true);
      maxStepAttemptStaticText->Show(true);

      initialStepSizeTextCtrl->Show(true);
      accuracyTextCtrl->Show(true);
      minStepTextCtrl->Show(true);
      maxStepTextCtrl->Show(true);
      maxStepAttemptTextCtrl->Show(true);

      if (isPredictorCorrector)
      {
         minIntErrorStaticText->Show(true);
         nomIntErrorStaticText->Show(true);
         minIntErrorTextCtrl->Show(true);
         nomIntErrorTextCtrl->Show(true);
      }

      propagatorStepSizeStaticText->Show(false);
      propagatorStepSizeTextCtrl->Show(false);
      unitsPropagatorStepSizeStaticText->Show(false);
      propCentralBodyStaticText->Show(false);
      propCentralBodyComboBox->Show(false);
      propagatorEpochFormatStaticText->Show(false);
      propagatorEpochFormatComboBox->Show(false);
      startEpochStaticText->Show(false);
      startEpochCombobox->Show(false);

      theStopCheckBox->Show(true);
   }
   else // It's either analytic or ephem based; hide other stuff
   {
      if (isEphem)
      {
         fmStaticSizer->Show(false);

         minIntErrorStaticText->Show(false);
         nomIntErrorStaticText->Show(false);

         initialStepSizeStaticText->Show(false);
         unitsInitStepSizeStaticText->Show(false);
         accuracyStaticText->Show(false);
         minStepStaticText->Show(false);
         unitsMinStepStaticText->Show(false);
         maxStepStaticText->Show(false);
         unitsMaxStepStaticText->Show(false);
         maxStepAttemptStaticText->Show(false);

         initialStepSizeTextCtrl->Show(false);
         accuracyTextCtrl->Show(false);
         minStepTextCtrl->Show(false);
         maxStepTextCtrl->Show(false);
         maxStepAttemptTextCtrl->Show(false);
         minIntErrorTextCtrl->Show(false);
         nomIntErrorTextCtrl->Show(false);

         propagatorStepSizeStaticText->Show(true);
         propagatorStepSizeTextCtrl->Show(true);
         unitsPropagatorStepSizeStaticText->Show(true);
         propCentralBodyStaticText->Show(true);
         propCentralBodyComboBox->Show(true);
         propagatorEpochFormatStaticText->Show(true);
         propagatorEpochFormatComboBox->Show(true);
         startEpochStaticText->Show(true);
         startEpochCombobox->Show(true);

         propCentralBodyComboBox->Enable(false);


         propCentralBodyComboBox->Clear();
         propCentralBodyComboBox->Append("Provided by file");
         propCentralBodyComboBox->SetValue("Provided by file");
      }
      else
      {
         // Not yet supported
      }
   }

   // Hide some items if not in testing mode
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   theGravModelSaveButton->Show(runmode == GmatGlobal::TESTING || runmode == GmatGlobal::TESTING_NO_PLOTS);

   theMiddleSizer->Layout();
   Refresh();
}

