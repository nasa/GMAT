//$Id$
//------------------------------------------------------------------------------
//                            BurnThrusterPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// number NNG04CC06P.
//
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009.02.11
/**
 * This class sets up Thruster or ImpulsiveBurn parameters.
 */
//------------------------------------------------------------------------------
#include "BurnThrusterPanel.hpp"
#include "ChemicalThruster.hpp"
#include "ElectricThruster.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "ThrusterCoefficientDialog.hpp"
#include "TankAndMixDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "Rmatrix.hpp"
#include <wx/variant.h>
#include <wx/config.h>

//#define DEBUG_BURNPANEL_CREATE
//#define DEBUG_BURNPANEL_LOAD
//#define DEBUG_BURNPANEL_SAVE
//#define DEBUG_BURNPANEL_SAVE_COEFS


//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BurnThrusterPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, BurnThrusterPanel::OnTextChange)
   EVT_TEXT(ID_COMBOBOX, BurnThrusterPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, BurnThrusterPanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, BurnThrusterPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, BurnThrusterPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// BurnThrusterPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs BurnThrusterPanel object.
 */
//------------------------------------------------------------------------------
BurnThrusterPanel::BurnThrusterPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true, true)
{
   mObjectName = name.c_str();
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("BurnThrusterPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif
   
   isCoordSysChanged = false;
   isTankChanged     = false;
   isTankEmpty       = false;
   coordSysName      = "";

   // thruster only
   // chemical thruster
   areCCoefsChanged  = false;
   areKCoefsChanged  = false;
   cCoefs.clear();
   kCoefs.clear();
   cCoefNames.clear();
   kCoefNames.clear();
   // electric thruster
   areTCoefsChanged  = false;
   areMFCoefsChanged = false;
   tCoefs.clear();
   mfCoefs.clear();
   tCoefNames.clear();
   mfCoefNames.clear();
   thrustModel          = "";
   isThrustModelChanged = false;
   useMixRatio = true;
}


//------------------------------------------------------------------------------
// ~Burnthrusterpanel()
//------------------------------------------------------------------------------
BurnThrusterPanel::~BurnThrusterPanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", coordSysComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", originComboBox);
   theGuiManager->UnregisterComboBox("FuelTank", tankComboBox);

   if (localObject != NULL)
      delete localObject;
}

//-------------------------------
// private methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void BurnThrusterPanel::Create()
{
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("BurnThrusterPanel::Create() entered\n");
   #endif

   #if __WXMAC__
   int otherTextWidth = 150;
   #else
   int otherTextWidth = 150;
   #endif

   localObject = NULL;
   
   Integer bsize = 2; // border size
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(gmatwxT("/Burn Thruster"));
   
   //-----------------------------------------------------------------
   // coordinate system items
   //-----------------------------------------------------------------
   // Coordinate Systems 
   wxStaticText *coordSysLabel =
      new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Coordinate System"));
   coordSysComboBox  =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   coordSysComboBox->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));
   
   // Addd Local to CoordinateSystem list
   coordSysComboBox->Insert("Local", 0);
   
   // Origin
   originLabel = new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Origin"));
   originComboBox =
      theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                              wxSize(150,-1));
   originComboBox->SetToolTip(pConfig->Read(_T("OriginHint")));
   
   // Axes 
   StringArray axesLabels = theObject->GetPropertyEnumStrings("Axes");
   wxArrayString wxAxesLabels = ToWxArrayString(axesLabels);
   
   axisLabel = new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Axes"));
   
   axesComboBox = 
      new wxComboBox(this, ID_COMBOBOX, gmatwxT(""), wxDefaultPosition, 
                     wxSize(150,-1), wxAxesLabels, wxCB_DROPDOWN|wxCB_READONLY);
   axesComboBox->SetSelection(0);
   axesComboBox->SetToolTip(pConfig->Read(_T("AxesHint")));
   
   //----- Add to sizer
   wxFlexGridSizer *coordSysSizer = new wxFlexGridSizer(3, 0, 0);
   coordSysSizer->Add(coordSysLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(coordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   coordSysSizer->Add(originLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(originComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   coordSysSizer->Add(axisLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(axesComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   GmatStaticBoxSizer *coordSysBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Coordinate System");
   coordSysBoxSizer->Add(coordSysSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   //-----------------------------------------------------------------
   // thrust vector items
   //-----------------------------------------------------------------
   
   // ThrustDirection1
   XUnitLabel = new wxStaticText(this, ID_TEXT, gmatwxT(""));
   XLabel = new wxStaticText(this, ID_TEXT, gmatwxT("ThrustDirection" GUI_ACCEL_KEY "1"));
   elem1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem1TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection1Hint")));
   
   // ThrustDirection2
   YUnitLabel =
      new wxStaticText(this, ID_TEXT, gmatwxT(""));
   YLabel =
      new wxStaticText(this, ID_TEXT, gmatwxT("ThrustDirection" GUI_ACCEL_KEY "2"),
                        wxDefaultPosition,wxDefaultSize, 0);
   elem2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem2TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection2Hint")));
   
   // ThrustDirection3
   ZUnitLabel = new wxStaticText(this, ID_TEXT, gmatwxT(""));
   ZLabel = new wxStaticText(this, ID_TEXT, gmatwxT("ThrustDirection" GUI_ACCEL_KEY "3"));
   elem3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem3TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection3Hint")));
   
   wxStaticText *dutyCycleLabel = NULL;
   wxStaticText *scaleFactorLabel = NULL;
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      // Thruster Duty Cycle
      dutyCycleLabel =
         new wxStaticText(this, ID_TEXT, gmatwxT("Duty " GUI_ACCEL_KEY "Cycle"));
      dutyCycleTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      dutyCycleTextCtrl->SetToolTip(pConfig->Read(_T("DutyCycleHint")));
      
      // Thruster Scale Factor
      scaleFactorLabel =
         new wxStaticText(this, ID_TEXT, gmatwxT("Thrust " GUI_ACCEL_KEY "Scale Factor"));
      scaleFactorTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""),
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      scaleFactorTextCtrl->SetToolTip(pConfig->Read(_T("ThrustScaleFactorHint")));
   }
   
   //----- Add to sizer   
   wxFlexGridSizer *vectorSizer = new wxFlexGridSizer(3, 0, 0);
   vectorSizer->Add(XLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(XUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   vectorSizer->Add(YLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(YUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   vectorSizer->Add(ZLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(ZUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      vectorSizer->Add(dutyCycleLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      vectorSizer->Add(dutyCycleTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      vectorSizer->Add(20,20);
      
      vectorSizer->Add(scaleFactorLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      vectorSizer->Add(scaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      vectorSizer->Add(20,20);
   }
   
   vectorBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Thrust Vector");
   vectorBoxSizer->Add(vectorSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   //-----------------------------------------------------------------
   // mass change items
   //-----------------------------------------------------------------
   // Decrement mass
   decMassCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, gmatwxT(GUI_ACCEL_KEY"Decrement Mass"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   decMassCheckBox->SetToolTip(pConfig->Read(_T("DecrementMassHint")));
   
   //Tank
   tankLabel =
      new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Tanks"));

   // Use one or the other of these, depending on if mix ratios are active
   tankTxtCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""),
            wxDefaultPosition, wxSize(150,-1), 0);
   tankTxtCtrl->SetToolTip(pConfig->Read(_T("TankHint")));
   
   tankComboBox =
      theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   tankComboBox->SetToolTip(pConfig->Read(_T("TankHint")));

   // Sizer for the tank widgets
   wxBoxSizer *tankSizer = new wxBoxSizer(wxVERTICAL);
   tankSizer->Add(tankTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL,
         bsize);
   tankSizer->Add(tankComboBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL,
         bsize);
   // Default to using mix ratio
   tankComboBox->Hide();

   mixRatioLabel =
         new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Mix Ratio"));
   mixRatioTxtCtrl = new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""),
         wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mixRatioTxtCtrl->SetToolTip(pConfig->Read(_T("DutyCycleHint")));

   tankTxtCtrl->SetEditable(false);
   mixRatioTxtCtrl->SetEditable(false);

   tankSelectorButton = new wxButton(this, ID_BUTTON, gmatwxT(GUI_ACCEL_KEY"Select Tanks"));
   tankSelectorButton->SetToolTip(pConfig->Read(_T("TankHint")));

   ispLabel = NULL;
   ispTextCtrl = NULL;
   ispUnit = NULL;
   // Isp for ImpulsiveBurn only
   if (theObject->IsOfType(Gmat::IMPULSIVE_BURN))
   {
      ispLabel =
         new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"Isp"));
      ispTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      ispTextCtrl->SetToolTip(pConfig->Read(_T("IspHint")));
      ispUnit =
         new wxStaticText(this, ID_TEXT, gmatwxT(" s"));
   }
   
   // Gravitational Acceleration
   gravityAccelLabel =
      new wxStaticText(this, ID_TEXT, gmatwxT(GUI_ACCEL_KEY"GravitationalAccel"));
   gravityAccelTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   gravityAccelTextCtrl->SetToolTip(pConfig->Read(_T("GravitationalAccelHint")));
   gravityAccelUnit =
      new wxStaticText(this, ID_TEXT, gmatwxT(" m/s^2"));
   
   // Coefficients for Thruster only
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
//      cCoefButton = new wxButton(this, ID_BUTTON, gmatwxT("Edit " GUI_ACCEL_KEY "Thruster Coef."));
//      cCoefButton->SetToolTip(pConfig->Read(_T("EditThrusterCoefficientHint")));
//      kCoefButton = new wxButton(this, ID_BUTTON, gmatwxT("Edit " GUI_ACCEL_KEY "Impulse Coef."));
//      kCoefButton->SetToolTip(pConfig->Read(_T("EditImpulseCoefficientHint")));
      configButton = new wxButton(this, ID_BUTTON, gmatwxT("" GUI_ACCEL_KEY "Configure Polynomials"));
      configButton->SetToolTip(pConfig->Read(_T("ConfigPolynomialsHint")));
   }
   
   wxBoxSizer *coefSizer = new wxBoxSizer(wxHORIZONTAL);
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
//      coefSizer->Add(cCoefButton, 0, wxALIGN_CENTER|wxALL, 5);
//      coefSizer->Add(kCoefButton, 0, wxALIGN_CENTER|wxALL, 5);
      coefSizer->Add(configButton, 0, wxALIGN_CENTER|wxALL, 5);  // need anther one here?
   }
   
   //----- Add to sizer   
   wxFlexGridSizer *massSizer = new wxFlexGridSizer(3, 0, 0);
   massSizer->Add(decMassCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   massSizer->Add(20,20);
   massSizer->Add(20,20);
   
   massSizer->Add(tankLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);

   // Add in the sizer for the tank controls here
   massSizer->Add(tankSizer, 0, wxALIGN_LEFT|wxALL, bsize);
//   massSizer->Add(tankTxtCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
//   massSizer->Add(tankComboBox, 0, wxALIGN_LEFT|wxALL, bsize);

   massSizer->Add(tankSelectorButton, 0, wxALIGN_LEFT | wxALL, bsize);
   massSizer->Add(mixRatioLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   massSizer->Add(mixRatioTxtCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   massSizer->Add(20, 0);
   

   if (theObject->IsOfType(Gmat::IMPULSIVE_BURN))
   {
      massSizer->Add(ispLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      massSizer->Add(ispTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(ispUnit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   }
   
   massSizer->Add(gravityAccelLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   massSizer->Add(gravityAccelTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   massSizer->Add(gravityAccelUnit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {      
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   }
   
   GmatStaticBoxSizer *massBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Mass Change");
   massBoxSizer->Add(massSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   // Handle ElectricThruster items
   GmatStaticBoxSizer *electricBoxSizer = NULL;
   if (theObject->IsOfType("ElectricThruster"))
   {
      electricBoxSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Thrust Config.");
      // Thrust Model
      thrustModelTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("Thrust " GUI_ACCEL_KEY "Model"),
                           wxDefaultPosition, wxDefaultSize, 0);
      StringArray thrustModels = theObject->GetPropertyEnumStrings("ThrustModel");
      Integer numModels = thrustModels.size();
      thrustModelArray = new wxString[numModels];
      for (Integer ii=0; ii<numModels; ii++)
         thrustModelArray[ii] = thrustModels[ii].c_str();

      thrustModelCB =
         new wxComboBox( this, ID_COMBOBOX, gmatwxT(thrustModelArray[0]),
            wxDefaultPosition, wxSize(180,-1), numModels, thrustModelArray,
            wxCB_DROPDOWN|wxCB_READONLY );
      thrustModelCB->SetToolTip(pConfig->Read(_T("ThrustModelHint")));

      // Minimum and Maximum Usable Power
      minPowerTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("Mi" GUI_ACCEL_KEY "nimum Usable Power"),
                           wxDefaultPosition, wxDefaultSize, 0);
      maxPowerTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("Ma" GUI_ACCEL_KEY "ximum Usable Power"),
                           wxDefaultPosition, wxDefaultSize, 0);
      minPowerTxtCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), wxDefaultPosition,
                        wxSize(otherTextWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      maxPowerTxtCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), wxDefaultPosition,
                        wxSize(otherTextWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

      minPowerUnits =
         new wxStaticText(this, ID_TEXT, gmatwxT("kW"), wxDefaultPosition, wxDefaultSize, 0);
      maxPowerUnits =
         new wxStaticText(this, ID_TEXT, gmatwxT("kW"), wxDefaultPosition, wxDefaultSize, 0);

      // Efficiency and ISP and Constant Thrust
      efficiencyTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("" GUI_ACCEL_KEY "Fixed Efficiency"),
                           wxDefaultPosition, wxDefaultSize, 0);
      ispTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("" GUI_ACCEL_KEY "Isp"),
                           wxDefaultPosition, wxDefaultSize, 0);
      constantThrustTxt =
         new wxStaticText( this, ID_TEXT, gmatwxT("" GUI_ACCEL_KEY "Constant Thrust"),
                           wxDefaultPosition, wxDefaultSize, 0);
      efficiencyTxtCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), wxDefaultPosition,
                        wxSize(otherTextWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      ispTxtCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), wxDefaultPosition,
                        wxSize(otherTextWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      constantThrustTxtCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, gmatwxT(""), wxDefaultPosition,
                        wxSize(otherTextWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

      efficiencyUnits =
         new wxStaticText(this, ID_TEXT, gmatwxT(""), wxDefaultPosition, wxDefaultSize, 0);
      ispUnits =
         new wxStaticText(this, ID_TEXT, gmatwxT("s"), wxDefaultPosition, wxDefaultSize, 0);
      constantThrustUnits =
         new wxStaticText(this, ID_TEXT, gmatwxT("N"), wxDefaultPosition, wxDefaultSize, 0);

      wxFlexGridSizer *electricSizer = new wxFlexGridSizer(3, 0, 0);
      electricSizer->Add(thrustModelTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(thrustModelCB, 0, wxALIGN_LEFT|wxALL, bsize);
      electricSizer->Add(20,20);
      electricSizer->Add(minPowerTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(minPowerTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(minPowerUnits, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(maxPowerTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(maxPowerTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(maxPowerUnits, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(efficiencyTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(efficiencyTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(efficiencyUnits, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(ispTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(ispTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(ispUnits, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(constantThrustTxt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(constantThrustTxtCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      electricSizer->Add(constantThrustUnits, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);

      electricBoxSizer->Add(electricSizer, 0, wxALIGN_CENTER|wxALL, 0);
   }

   //-----------------------------------------------------------------
   // add to page sizer
   //-----------------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(coordSysBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   pageSizer->Add(vectorBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   pageSizer->Add(massBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      if (theObject->IsOfType("ElectricThruster"))
      {
         pageSizer->Add(electricBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
      }
      pageSizer->Add(coefSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   }
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("BurnThrusterPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void BurnThrusterPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("BurnThrusterPanel::LoadData() entered\n");
   #endif
   
   // Set object pointer for "Show Script"
   mObject = theObject;
   bool isImpBurn = false;
   if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      isImpBurn = true;
   
   if (isImpBurn)
   {
      thrustDir1 = "Element1";
      thrustDir2 = "Element2";
      thrustDir3 = "Element3";
   }
   else
   {
      thrustDir1 = "ThrustDirection1";
      thrustDir2 = "ThrustDirection2";
      thrustDir3 = "ThrustDirection3";
   }
   
   Integer paramID;
   
   try
   {
      paramID = theObject->GetParameterID("CoordinateSystem");
      coordSysName = theObject->GetStringParameter(paramID);
      coordSysComboBox->SetValue(coordSysName.c_str());
      
      paramID = theObject->GetParameterID("Origin");
      std::string objName = theObject->GetStringParameter(paramID);
      originComboBox->SetValue(objName.c_str());
      
      paramID = theObject->GetParameterID("Axes");
      objName = theObject->GetStringParameter(paramID);
      axesComboBox->SetValue(objName.c_str());
      
      paramID = theObject->GetParameterID(thrustDir1);
      elem1TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID(thrustDir2);
      elem2TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID(thrustDir3);
      elem3TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID("DecrementMass");
      decMassCheckBox->SetValue((wxVariant(theObject->GetBooleanParameter(paramID))));
      
      paramID = theObject->GetParameterID("GravitationalAccel");
      gravityAccelTextCtrl->SetValue((wxVariant(theObject->GetRealParameter(paramID))));
      
      paramID = theObject->GetParameterID("Tank");
      tankNames = theObject->GetStringArrayParameter(paramID);

      if (useMixRatio)
      {
         paramID = theObject->GetParameterID("MixRatio");
         Rvector theMix = theObject->GetRvectorParameter(paramID);
         for (Integer i = 0; i < theMix.GetSize(); ++i)
            if (i < tankNames.size())
               mixRatio.push_back(theMix[i]);
            if (theMix.GetSize() < tankNames.size())
               for (Integer i = theMix.GetSize(); i < tankNames.size(); ++i)
                  mixRatio.push_back(1.0);

         LoadTankAndMixControl();
      }
      else
      {
         if (tankNames.empty())
         {
            if (theGuiManager->GetNumFuelTank() > 0)
            {
               tankComboBox->Insert("No Fuel Tank Selected", 0);
               tankComboBox->SetSelection(0);
            }
         }
         else
         {
            tankComboBox->SetValue(STD_TO_WX_STRING(tankNames[0].c_str()));
            isTankEmpty = false;
         }

         tankTxtCtrl->SetEditable(true);
         if (tankNames.size() > 0)
         {
            if (tankNames.size() > 1)
               MessageInterface::ShowMessage("Only one tank name is allowed");
            tankTxtCtrl->SetValue(tankNames[0].c_str());
         }
         mixRatioTxtCtrl->Hide();
         tankTxtCtrl->Hide();
         tankComboBox->Show();
         tankSelectorButton->Hide();
         mixRatioLabel->Hide();
      }
      
      // Disable tank combo box if decrement mass is not checked
      if (!decMassCheckBox->IsChecked())
      {
         // Tanks needed to apply nontrivial coefficients, so don't disable
         tankLabel->Disable();
         tankTxtCtrl->Disable();
//         tankComboBox->Disable();
         mixRatioTxtCtrl->Disable();
         mixRatioLabel->Disable();

         // g is only used to decrement mass
         gravityAccelLabel->Disable();
         gravityAccelTextCtrl->Disable();
         gravityAccelUnit->Disable();

         if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
         {
            ispLabel->Disable();
            ispTextCtrl->Disable();
            ispUnit->Disable();
         }
      }
      else
      {
         // g is required to decrement mass
         gravityAccelLabel->Enable();
         gravityAccelTextCtrl->Enable();
         gravityAccelUnit->Enable();
      }

      if (theObject->IsOfType(Gmat::THRUSTER))
      {
         if (theObject->IsOfType("ChemicalThruster"))
         {
            // Get the initial values for the coefficients
            Integer cParamID  = 0;
            Integer kParamID  = 0;
            Integer coefCount = ChemicalThruster::COEFFICIENT_COUNT;
            std::stringstream cStrings("");
            std::stringstream kStrings("");
            Real cVal, kVal;
            for (Integer ii = 0; ii < coefCount; ii++)
            {
               cStrings << "C" << ii + 1;
               cParamID = theObject->GetParameterID(cStrings.str());
               cVal     = theObject->GetRealParameter(cParamID);
               #ifdef DEBUG_BURNPANEL_LOAD
                  MessageInterface::ShowMessage("Loading: %s =  %lf\n", cStrings.str().c_str(), cVal);
               #endif
               cCoefs.push_back(cVal);
               cCoefNames.push_back(cStrings.str());
               cStrings.str("");
            }
            for (Integer ii = 0; ii < coefCount; ii++)
            {
               kStrings << "K" << ii + 1;
               kParamID = theObject->GetParameterID(kStrings.str());
               kVal     = theObject->GetRealParameter(kParamID);
               #ifdef DEBUG_BURNPANEL_LOAD
                  MessageInterface::ShowMessage("Loading: %s =  %lf\n", kStrings.str().c_str(), kVal);
               #endif
               kCoefs.push_back(kVal);
               kCoefNames.push_back(kStrings.str());
               kStrings.str("");
            }
         }
         else // Electric Thruster
         {
            // Get the initial values for the coefficients
            Integer tParamID  = 0;
            Integer mfParamID = 0;
            Integer coefCount = ElectricThruster::ELECTRIC_COEFF_COUNT;
            std::stringstream tStrings("");
            std::stringstream mfStrings("");
            Real tVal, mfVal;
            for (Integer ii = 0; ii < coefCount; ii++)
            {
               tStrings << "ThrustCoeff" << ii + 1;
               tParamID = theObject->GetParameterID(tStrings.str());
               tVal     = theObject->GetRealParameter(tParamID);
               #ifdef DEBUG_BURNPANEL_LOAD
                  MessageInterface::ShowMessage("Loading: %s =  %lf\n", tStrings.str().c_str(), tVal);
               #endif
               tCoefs.push_back(tVal);
               tCoefNames.push_back(tStrings.str());
               tStrings.str("");
            }
            for (Integer ii = 0; ii < coefCount; ii++)
            {
               mfStrings << "MassFlowCoeff" << ii + 1;
               mfParamID = theObject->GetParameterID(mfStrings.str());
               mfVal     = theObject->GetRealParameter(mfParamID);
               #ifdef DEBUG_BURNPANEL_LOAD
                  MessageInterface::ShowMessage("Loading: %s =  %lf\n", mfStrings.str().c_str(), mfVal);
               #endif
               mfCoefs.push_back(mfVal);
               mfCoefNames.push_back(mfStrings.str());
               mfStrings.str("");
            }
         }
      }

      // Update Origin and Axes
      UpdateOriginAxes();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("BurnThrusterPanel::LoadData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void BurnThrusterPanel::SaveData()
{
#ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() entered\n");
#endif
   // create local copy of mObject
   if (localObject != NULL)
   {
      delete localObject;
   }
   localObject = mObject->Clone();
   SaveData(localObject);
   localObject->TakeAction("ClearTanks");

   if (useMixRatio)
   {
      for (UnsignedInt i = 0; i < tankNames.size(); ++i)
         localObject->SetStringParameter("Tank", tankNames[i]);
      for (UnsignedInt i = 0; i < mixRatio.size(); ++i)
         localObject->SetRealParameter("MixRatio", mixRatio[i], i);
   }
   else
   {
      #ifdef DEBUG_BURNPANEL_SAVE
         MessageInterface::ShowMessage("Setting tank to '%s'\n",
               tankTxtCtrl->GetValue().mb_str().data());
      #endif
      std::string tankName = tankComboBox->GetValue().mb_str().data();
      if ((tankName != "No Fuel Tank Selected") &&
          (tankName != "No Fuel Tanks Available"))
         localObject->SetStringParameter("Tank", tankName);
   }

   // if no errors, save again
   if (canClose)
	   theObject->Copy(localObject);

   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() exiting\n");
   #endif
}

   
//------------------------------------------------------------------------------
// void SaveData(GmatBase *theObject)
//------------------------------------------------------------------------------
void BurnThrusterPanel::SaveData(GmatBase *theObject)
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   Real elem1, elem2, elem3, gravityAccel;
   bool realDataChanged = false;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (elem1TextCtrl->IsModified() || elem2TextCtrl->IsModified() ||
       elem3TextCtrl->IsModified() || gravityAccelTextCtrl->IsModified())
   {
      str = elem1TextCtrl->GetValue();
      CheckReal(elem1, str, thrustDir1, "Real Number");
      
      str = elem2TextCtrl->GetValue();
      CheckReal(elem2, str, thrustDir2, "Real Number");
      
      str = elem3TextCtrl->GetValue();
      CheckReal(elem3, str, thrustDir3, "Real Number");
      
      str = gravityAccelTextCtrl->GetValue();
      CheckReal(gravityAccel, str, "GravitationalAccel", "Real Number > 0", false,
                true, true);
      
      realDataChanged = true;      
   }
   
   if (!canClose)
      return;
   
   try 
   {
      Integer paramID;
      
      // Coordinate System      
      if (isCoordSysChanged)
      {
         paramID = theObject->GetParameterID("CoordinateSystem");
         theObject->SetStringParameter(paramID, coordSysName);
         isCoordSysChanged = false;
      }
      
      if (coordSysName == "Local")
      {
         // Origin
         paramID = theObject->GetParameterID("Origin");
         theObject->SetStringParameter(paramID, originComboBox->GetValue().WX_TO_STD_STRING);
         
         // Axes
         paramID = theObject->GetParameterID("Axes");
         theObject->SetStringParameter(paramID, axesComboBox->GetValue().WX_TO_STD_STRING);

         std::string axisValue = axesComboBox->GetValue().WX_TO_STD_STRING;
         if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
         {
            originLabel->Disable();
            originComboBox->Disable();
         }
         else
         {
            originLabel->Enable();
            originComboBox->Enable();
         }
      }
      
      // Save ThrustDirections
      if (realDataChanged)
      {
         paramID = theObject->GetParameterID(thrustDir1);
         theObject->SetRealParameter(paramID, elem1);
         
         paramID = theObject->GetParameterID(thrustDir2);
         theObject->SetRealParameter(paramID, elem2);
         
         paramID = theObject->GetParameterID(thrustDir3);
         theObject->SetRealParameter(paramID, elem3);
         
         paramID = theObject->GetParameterID("GravitationalAccel");
         theObject->SetRealParameter(paramID, gravityAccel);
      }
      
      // Always save DecrementMass
      // @todo If some base code computation involved, have separate flag (LOJ)
      paramID = theObject->GetParameterID("DecrementMass");
      if (decMassCheckBox->IsChecked())
         theObject->SetBooleanParameter(paramID, true);
      else
         theObject->SetBooleanParameter(paramID, false);
      
      // Save Tank
      if (isTankChanged)
      {
         isTankChanged = false;
         paramID = theObject->GetParameterID("Tank");
         if (theObject->TakeAction("ClearTanks", ""))
         {
            if (useMixRatio)
            {
               Integer mixID   = theObject->GetParameterID("MixRatio");
               for (UnsignedInt i = 0; i < tankNames.size(); ++i)
               {
                  theObject->SetStringParameter(paramID, tankNames[i]);
                  theObject->SetRealParameter(mixID,
                        (mixRatio.size() < i ? mixRatio[i] : 1.0), i);
               }
            }
            else
            {
               #ifdef DEBUG_BURNPANEL_SAVE
                  MessageInterface::ShowMessage("Setting tank to '%s'\n",
                        tankTxtCtrl->GetValue().mb_str().data());
               #endif
               std::string tankName = tankComboBox->GetValue().mb_str().data();
               if ((tankName != "No Fuel Tank Selected") &&
                   (tankName != "No Fuel Tanks Available"))
                  localObject->SetStringParameter("Tank", tankName);
            }
         }
      }

      if (theObject->IsOfType(Gmat::THRUSTER))
      {
         if (theObject->IsOfType("ChemicalThruster"))
         {
            // Save C Coefficients
            if (areCCoefsChanged)
            {
               unsigned int coefSize = cCoefs.size();
               for (unsigned int i = 0; i < coefSize; i++)
               {
                  #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                     MessageInterface::ShowMessage("Saving %s with value %lf\n", cCoefNames[i].c_str(), cCoefs[i]);
                  #endif
                  paramID = theObject->GetParameterID(cCoefNames[i]);
                  theObject->SetRealParameter(paramID, cCoefs[i]);
               }
            }

            // Save K Coefficients
            if (areKCoefsChanged)
            {
               unsigned int coefSize = kCoefs.size();
               for (unsigned int i = 0; i < coefSize; i++)
               {
                  #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                     MessageInterface::ShowMessage("Saving %s with value %lf\n", kCoefNames[i].c_str(), kCoefs[i]);
                  #endif
                  paramID = theObject->GetParameterID(kCoefNames[i]);
                  theObject->SetRealParameter(paramID, kCoefs[i]);
               }
            }
         }
         else // ElectricThruster
         {
            // Save T Coefficients
            if (areTCoefsChanged)
            {
               unsigned int coefSize = tCoefs.size();
               for (unsigned int i = 0; i < coefSize; i++)
               {
                  #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                     MessageInterface::ShowMessage("Saving %s with value %lf\n", tCoefNames[i].c_str(), tCoefs[i]);
                  #endif
                  paramID = theObject->GetParameterID(tCoefNames[i]);
                  theObject->SetRealParameter(paramID, tCoefs[i]);
               }
            }

            // Save MF Coefficients
            if (areMFCoefsChanged)
            {
               unsigned int coefSize = mfCoefs.size();
               for (unsigned int i = 0; i < coefSize; i++)
               {
                  #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                     MessageInterface::ShowMessage("Saving %s with value %lf\n", mfCoefNames[i].c_str(), mfCoefs[i]);
                  #endif
                  paramID = theObject->GetParameterID(mfCoefNames[i]);
                  theObject->SetRealParameter(paramID, mfCoefs[i]);
               }
            }
         }
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnTextChange(wxCommandEvent &event)
{
   if (event.GetEventObject() == tankTxtCtrl)
      isTankChanged = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   if (decMassCheckBox->IsChecked())
   {
      // Disabling disabled so no need to enable
      tankLabel->Enable();
      tankTxtCtrl->Enable();
//      tankComboBox->Enable();
      mixRatioTxtCtrl->Enable();
      mixRatioLabel->Enable();

      // g is only used to decrement mass
      gravityAccelLabel->Enable();
      gravityAccelTextCtrl->Enable();
      gravityAccelUnit->Enable();

      if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      {
         ispLabel->Enable();
         ispTextCtrl->Enable();
         ispUnit->Enable();
      }
   }
   else
   {
      // Tanks needed to apply nontrivial coefficients, so don't disable
      tankLabel->Disable();
      tankTxtCtrl->Disable();
//      tankComboBox->Disable();
      mixRatioTxtCtrl->Disable();
      mixRatioLabel->Disable();

      // g is only used to decrement mass
      gravityAccelLabel->Disable();
      gravityAccelTextCtrl->Disable();
      gravityAccelUnit->Disable();

      if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      {
         ispLabel->Disable();
         ispTextCtrl->Disable();
         ispUnit->Disable();
      }
   }
   
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnComboBoxChange(wxCommandEvent &event)
{
   if (event.GetEventObject() == coordSysComboBox)
   {
      UpdateOriginAxes();      
      isCoordSysChanged =  true;
      coordSysName = coordSysComboBox->GetStringSelection().c_str();
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == tankComboBox)
   {
      isTankChanged = true;
      std::string tankName = tankComboBox->GetStringSelection().WX_TO_STD_STRING;
      if (tankName == "No Fuel Tank Selected")
         tankNames.clear();
      
      // remove "No Tank Selected" once tank is selected
      int pos = tankComboBox->FindString("No Fuel Tank Selected");
      if (pos != wxNOT_FOUND)
         tankComboBox->Delete(pos);
      
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == axesComboBox)
   {
      std::string csName = coordSysComboBox->GetStringSelection().WX_TO_STD_STRING;
      
      if (csName == "Local")
      {
         std::string axisValue = axesComboBox->GetValue().WX_TO_STD_STRING;
         if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
         {
            originLabel->Disable();
            originComboBox->Disable();
         }
         else
         {
            originLabel->Enable();
            originComboBox->Enable();
         }
      }
   }
   // thrustModelCB will be NULL if the thruster is not Electric
   else if (event.GetEventObject() == thrustModelCB)
   {
      thrustModel = thrustModelCB->GetStringSelection().c_str();
      EnableDataForThrustModel(thrustModel);
      isThrustModelChanged =  true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnButtonClick(wxCommandEvent &event)
{  
   if (event.GetEventObject() == tankSelectorButton)
   {
      if (mixRatio.size() < tankNames.size())
      {
         RealArray newRatio;
         for (Integer i = 0; i < tankNames.size(); ++i)
         {
            if (i < mixRatio.size())
               newRatio.push_back(mixRatio[i]);
            else
               newRatio.push_back(1.0);
         }
         mixRatio = newRatio;
      }
      wxArrayString tankNamesWx;
      wxArrayDouble mixRatioWx;

      for (UnsignedInt i = 0; i < tankNames.size(); ++i)
      {
         tankNamesWx.Add(tankNames[i].c_str());
         mixRatioWx.Add(mixRatio[i]);
      }
      TankAndMixDialog tmDlg(this, tankNamesWx, mixRatioWx);
      tmDlg.ShowModal();
      // Pop the data back off
      if (tmDlg.UpdateTankAndMixArrays(tankNamesWx, mixRatioWx))
      {
         tankNames.clear();
         mixRatio.clear();
         for (UnsignedInt i = 0; i < tankNamesWx.size(); ++i)
         {
            tankNames.push_back(tankNamesWx[i].mb_str().data());
            mixRatio.push_back(mixRatioWx[i]);
         }
         LoadTankAndMixControl();
      }
   }
   else
   {
      if (theObject->IsOfType("ChemicalThruster"))
      {
         ThrusterCoefficientDialog tcDlg(this, -1, "Chemical Thruster Configuration",
               theObject, ChemicalThruster::COEFFICIENT_COUNT, cCoefs, kCoefs);
         tcDlg.ShowModal();
         bool cSaved = tcDlg.AreCoefs1Saved();
         bool kSaved = tcDlg.AreCoefs2Saved();

         if (cSaved)
         {
            cCoefs.clear();
            cCoefs      = tcDlg.GetCoefs1Values();

            EnableUpdate(true);
         }
         areCCoefsChanged = areCCoefsChanged || cSaved;

         if (kSaved)
         {
            kCoefs.clear();
            kCoefs      = tcDlg.GetCoefs2Values();

            EnableUpdate(true);
         }
         areKCoefsChanged = areKCoefsChanged || kSaved;
      }
      else // ElectricThruster
      {
   //      MessageInterface::ShowMessage("Electric Thruster Configuration not yet implemented.\n");
         ThrusterCoefficientDialog tcDlg(this, -1, "Electric Thruster Configuration",
               theObject, ElectricThruster::ELECTRIC_COEFF_COUNT, tCoefs, mfCoefs);
         tcDlg.ShowModal();

         bool tSaved  = tcDlg.AreCoefs1Saved();
         bool mfSaved = tcDlg.AreCoefs2Saved();

         if (tSaved)
         {
            tCoefs.clear();
            tCoefs      = tcDlg.GetCoefs1Values();

            EnableUpdate(true);
         }
         areTCoefsChanged = areTCoefsChanged || tSaved;

         if (mfSaved)
         {
            mfCoefs.clear();
            mfCoefs      = tcDlg.GetCoefs2Values();

            EnableUpdate(true);
         }
         areMFCoefsChanged = areMFCoefsChanged || mfSaved;
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateOriginAxes()
//------------------------------------------------------------------------------
void BurnThrusterPanel::UpdateOriginAxes()
{
   if (coordSysComboBox->GetValue() == "Local")
   {
      axisLabel->Enable();
      axesComboBox->Enable();

      std::string axisValue = axesComboBox->GetValue().WX_TO_STD_STRING;

      if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
      {
         originLabel->Disable();
         originComboBox->Disable();
      }
      else
      {
         originLabel->Enable();
         originComboBox->Enable();
      }
   }
   else
   {
      originLabel->Disable();
      originComboBox->Disable();
      axisLabel->Disable();
      axesComboBox->Disable();
   }
}

//------------------------------------------------------------------------------
// void EnableDataForThrustModel()
//------------------------------------------------------------------------------
void BurnThrusterPanel::EnableDataForThrustModel(const std::string &tModel)
{
   ispTxt->Enable();
   ispTxtCtrl->Enable();
   ispUnits->Enable();
   constantThrustTxt->Enable();
   constantThrustTxtCtrl->Enable();
   constantThrustUnits->Enable();
   efficiencyTxt->Enable();
   efficiencyTxtCtrl->Enable();
   efficiencyUnits->Enable();
   configButton->Enable();

   if (tModel != "ThrustMassPolynomial")
      configButton->Disable();


   if (tModel != "FixedEfficiency")
   {
      efficiencyTxt->Disable();
      efficiencyTxtCtrl->Disable();
      efficiencyUnits->Disable();
   }

   if (tModel != "ConstantThrustAndIsp")
   {
      constantThrustTxt->Disable();
      constantThrustTxtCtrl->Disable();
      constantThrustUnits->Disable();
   }

   if ((tModel != "FixedEfficiency") && (tModel != "ConstantThrustAndIsp"))
   {
      ispTxt->Disable();
      ispTxtCtrl->Disable();
      ispUnits->Disable();
   }
}


//------------------------------------------------------------------------------
// void LoadTankAndMixControl()
//------------------------------------------------------------------------------
/**
 * Updates the tank and mix text controls
 */
//------------------------------------------------------------------------------
void BurnThrusterPanel::LoadTankAndMixControl()
{
   std::string tankList;
   for (UnsignedInt i = 0; i < tankNames.size(); ++i)
   {
      if (i > 0)
         tankList += ", ";
      tankList += tankNames[i];
   }
   tankTxtCtrl->SetValue(tankList.c_str());

   std::stringstream ratio;
   ratio.precision(14);
   for (Integer i = 0; i < mixRatio.size(); ++i)
   {
      if (i > 0)
         ratio << " : ";
      ratio << mixRatio[i];
   }
   mixRatioTxtCtrl->SetValue(ratio.str().c_str());
}


//------------------------------------------------------------------------------
// void BurnThrusterPanel::EnableMixRatio(bool activate)
//------------------------------------------------------------------------------
/**
 * Method used to turn the mix ration controls on or off
 *
 * @param activate true to use a mix ratio, false to disable it.
 */
//------------------------------------------------------------------------------
void BurnThrusterPanel::EnableMixRatio(bool activate)
{
   useMixRatio = activate;
}
