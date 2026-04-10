//$Id$
//------------------------------------------------------------------------------
//                            PowerSystemConfigPanel
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
// Author: Wendy Shoan
// Created: 2014.05.13
/**
 * This class sets up PowerSystem parameters.
 */
//------------------------------------------------------------------------------
#include "PowerSystemConfigPanel.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "CelesBodySelectDialog.hpp"
#include "TimeSystemConverter.hpp"
#include "PowerSystem.hpp"
#include "GmatDefaults.hpp"
//#include <wx/variant.h>  // ??
#include <wx/config.h>   // ??

//#define DEBUG_POWERPANEL_CREATE
//#define DEBUG_POWERPANEL_LOAD
//#define DEBUG_POWERPANEL_SAVE
//#define DEBUG_POWERPANEL_SAVE_COEFS
//#define DEBUG_POWERSYSTEM_PANEL_COMBOBOX

//------------------------------
// event tables for wxWidgets
//------------------------------
BEGIN_EVENT_TABLE(PowerSystemConfigPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, PowerSystemConfigPanel::OnTextChange)
   EVT_COMBOBOX(ID_FORMAT_COMBOBOX, PowerSystemConfigPanel::OnComboBoxChange)
   EVT_COMBOBOX(ID_SHADOWMODEL_COMBOBOX, PowerSystemConfigPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON_BODIES, PowerSystemConfigPanel::OnBodiesEditButton)
END_EVENT_TABLE()


// add this one later:


//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// PowerSystemConfigPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs PowerSystemConfigPanel object.
 */
//------------------------------------------------------------------------------
PowerSystemConfigPanel::PowerSystemConfigPanel(wxWindow *parent,
                                               const wxString &name)
   : GmatPanel(parent, true, true)
{
   #ifdef DEBUG_POWERPANEL_CREATE
      MessageInterface::ShowMessage("Entering PSCP::Constructor, name = %s\n",
                                    name.c_str());
   #endif
   mObjectName = name.c_str();
   theObject   = theGuiInterpreter->GetConfiguredObject(name.c_str());

   #ifdef DEBUG_POWERPANEL_CREATE
      if (!theObject)
         MessageInterface::ShowMessage("In PSCP::Constructor, theObject is NULL!!!\n");
   #endif
   if (theObject->IsOfType("SolarPowerSystem"))
      isSolar = true;
   else
      isSolar = false;

   #ifdef DEBUG_POWERPANEL_CREATE
   MessageInterface::ShowMessage
      ("PowerSystemConfigPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif

   ResetChangedFlags();

   // To set panel object and show warning if object is NULL
   if (SetObject(theObject))
   {
      Create();
      Show();
   }

}


//------------------------------------------------------------------------------
// ~PowerSystemConfigPanel()
//------------------------------------------------------------------------------
PowerSystemConfigPanel::~PowerSystemConfigPanel()
{
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
void PowerSystemConfigPanel::Create()
{
   #ifdef DEBUG_POWERPANEL_CREATE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::Create() entered\n");
   #endif

   localObject = NULL;

   Integer staticTextWidth = 120;

   Integer bsize = 2; // border size
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Power System"));

   wxArrayString emptyList;

   int epochWidth = 170;
   #ifdef __WXMAC__
      epochWidth = 178;
   #endif

   //-----------------------------------------------------------------
   //  create sizers
   //-----------------------------------------------------------------
   wxBoxSizer *powerSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer    *flexGridSizer1  = new wxFlexGridSizer( 3, 0, 0 );
   wxFlexGridSizer    *flexGridSizer2  = new wxFlexGridSizer( 3, 0, 0 );
   GmatStaticBoxSizer *generalSizer    = new GmatStaticBoxSizer( wxVERTICAL, this, "General" );
   GmatStaticBoxSizer *coeffSizer      = new GmatStaticBoxSizer( wxVERTICAL, this, "Coefficients" );

   //-----------------------------------------------------------------
   // epoch
   //-----------------------------------------------------------------
   // label for epoch format
   epochFormatTxt = new wxStaticText( this, ID_TEXT,
      "Epoch " GUI_ACCEL_KEY "Format", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );

   // combo box for the epoch format
   epochFormatComboBox = new wxComboBox
      ( this, ID_FORMAT_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   epochFormatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));

   // label for epoch
   epochTxt = new wxStaticText( this, ID_TEXT,
      "" GUI_ACCEL_KEY "Initial Epoch", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );

   // textfield for the epoch value
   epochTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   epochTxtCtrl->SetToolTip(pConfig->Read(_T("EpochHint")));

   // static text for initial max power
   initPowerTxt = new wxStaticText( this, ID_TEXT,
           "Initial " GUI_ACCEL_KEY "Max Power", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   initPowerTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   initPowerTxtCtrl->SetToolTip(pConfig->Read(_T("InitialMaxPowerHint")));
   // static text for units
   initPowerUnits = new wxStaticText( this, ID_TEXT, wxT("kW"), wxDefaultPosition,
         wxSize(staticTextWidth,-1), 0 );

   // static text for decay rate
   decayRateTxt = new wxStaticText( this, ID_TEXT,
           "" GUI_ACCEL_KEY "Decay Rate", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   decayRateTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   decayRateTxtCtrl->SetToolTip(pConfig->Read(_T("DecayRateHint")));
   // static text for units
   decayRateUnits = new wxStaticText( this, ID_TEXT, wxT("percent/year"), wxDefaultPosition,
         wxSize(staticTextWidth,-1), 0 );

   // static text for margin
   powerMarginTxt = new wxStaticText( this, ID_TEXT,
           "" GUI_ACCEL_KEY "Margin", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   powerMarginTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   powerMarginTxtCtrl->SetToolTip(pConfig->Read(_T("PowerMarginHint")));
   // static text for units
   powerMarginUnits = new wxStaticText( this, ID_TEXT, wxT("percent"), wxDefaultPosition,
         wxSize(staticTextWidth,-1), 0 );

   // static text for bus coeff1
   busCoeff1Txt = new wxStaticText( this, ID_TEXT,
           "" GUI_ACCEL_KEY "Bus Coeff1", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   busCoeff1TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   busCoeff1TxtCtrl->SetToolTip(pConfig->Read(_T("BusCoeff1Hint")));
   // static text for units
   busCoeff1Units = new wxStaticText( this, ID_TEXT, wxT("kW"));
   // static text for bus coeff2
   busCoeff2Txt = new wxStaticText( this, ID_TEXT,
           "" GUI_ACCEL_KEY "Bus Coeff2", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   busCoeff2TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   busCoeff2TxtCtrl->SetToolTip(pConfig->Read(_T("BusCoeff2Hint")));
   // static text for units
   busCoeff2Units = new wxStaticText( this, ID_TEXT, wxT("kW*AU"));
   // static text for bus coeff3
   busCoeff3Txt = new wxStaticText( this, ID_TEXT,
           "" GUI_ACCEL_KEY "Bus Coeff3", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   // textfield
   busCoeff3TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   busCoeff3TxtCtrl->SetToolTip(pConfig->Read(_T("BusCoeff3Hint")));
   // static text for units
   busCoeff3Units = new wxStaticText( this, ID_TEXT, wxT("kW*AU^2"));
//   busCoeff3Units->SetLabelText(b3Unit);  // need wxOSX 3.0 for this?

   if (isSolar)
   {
      // label for power shadow model
      shadowModelTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Shadow Model", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );

      // combo box for the shadow model
      shadowModelComboBox = new wxComboBox
         ( this, ID_SHADOWMODEL_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
           emptyList, wxCB_DROPDOWN | wxCB_READONLY );
      shadowModelComboBox->SetToolTip(pConfig->Read(_T("ShadowModelHint")));

      shadowBodiesTxt = new wxStaticText( this, ID_TEXT,
         "Shadow " GUI_ACCEL_KEY "Bodies", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      shadowBodiesTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), wxTE_READONLY  );
      shadowBodiesTxtCtrl->SetToolTip(pConfig->Read(_T("ShadowBodiesHint")));
      shadowBodiesButton =
         new wxButton( this, ID_BUTTON_BODIES, wxT("Select"),
                       wxDefaultPosition, wxDefaultSize, 0 );
      shadowBodiesButton->SetToolTip(pConfig->Read(_T("ShadowBodiesHint")));

      // static text for solar coeff1
      solarCoeff1Txt = new wxStaticText( this, ID_TEXT,
              "" GUI_ACCEL_KEY "Solar Coeff1", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      // textfield
      solarCoeff1TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), 0 );
      solarCoeff1TxtCtrl->SetToolTip(pConfig->Read(_T("SolarCoeff1Hint")));
      // static text for units
      solarCoeff1Units = new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition,
            wxSize(staticTextWidth,-1), 0 );
      // static text for solar coeff2
      solarCoeff2Txt = new wxStaticText( this, ID_TEXT,
              "" GUI_ACCEL_KEY "Solar Coeff2", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      // textfield
      solarCoeff2TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), 0 );
      solarCoeff2TxtCtrl->SetToolTip(pConfig->Read(_T("SolarCoeff2Hint")));
      // static text for units
      solarCoeff2Units = new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition,
            wxSize(staticTextWidth,-1), 0 );
      // static text for solar coeff3
      solarCoeff3Txt = new wxStaticText( this, ID_TEXT,
              "" GUI_ACCEL_KEY "Solar Coeff3", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      // textfield
      solarCoeff3TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), 0 );
      solarCoeff3TxtCtrl->SetToolTip(pConfig->Read(_T("SolarCoeff3Hint")));
      // static text for units
      solarCoeff3Units = new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition,
            wxSize(staticTextWidth,-1), 0 );
      // static text for solar coeff4
      solarCoeff4Txt = new wxStaticText( this, ID_TEXT,
              "" GUI_ACCEL_KEY "Solar Coeff4", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      // textfield
      solarCoeff4TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), 0 );
      solarCoeff4TxtCtrl->SetToolTip(pConfig->Read(_T("SolarCoeff4Hint")));
      // static text for units
      solarCoeff4Units = new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition,
            wxSize(staticTextWidth,-1), 0 );
      // static text for solar coeff5
      solarCoeff5Txt = new wxStaticText( this, ID_TEXT,
              "" GUI_ACCEL_KEY "Solar Coeff5", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      // textfield
      solarCoeff5TxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth,-1), 0 );
      solarCoeff5TxtCtrl->SetToolTip(pConfig->Read(_T("SolarCoeff5Hint")));
      // static text for units
      solarCoeff5Units = new wxStaticText( this, ID_TEXT, wxT(""), wxDefaultPosition,
            wxSize(staticTextWidth,-1), 0 );
   }


   flexGridSizer1->Add( epochFormatTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( epochFormatComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add(20,20);
   flexGridSizer1->Add( epochTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( epochTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add(20,20);
   flexGridSizer1->Add( initPowerTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( initPowerTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( initPowerUnits, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( decayRateTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( decayRateTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( decayRateUnits, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( powerMarginTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( powerMarginTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer1->Add( powerMarginUnits, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   flexGridSizer2->Add( busCoeff1Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff1TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff1Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff2Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff2TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff2Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff3Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff3TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   flexGridSizer2->Add( busCoeff3Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   if (isSolar)
   {
      flexGridSizer1->Add( shadowModelTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer1->Add( shadowModelComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer1->Add(20,20);
      flexGridSizer1->Add( shadowBodiesTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer1->Add( shadowBodiesTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer1->Add( shadowBodiesButton, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

      flexGridSizer2->Add( solarCoeff1Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff1TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff1Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff2Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff2TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff2Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff3Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff3TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff3Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff4Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff4TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff4Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff5Txt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff5TxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      flexGridSizer2->Add( solarCoeff5Units, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   }



   // Add to general sizer
   generalSizer->Add(flexGridSizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   // Add to coeff sizer
   coeffSizer->Add(flexGridSizer2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );

   powerSizer->Add( generalSizer, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   powerSizer->Add( coeffSizer, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   theMiddleSizer->Add(powerSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   #ifdef DEBUG_POWERPANEL_CREATE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::LoadData()
{
   #ifdef DEBUG_POWERPANEL_LOAD
   MessageInterface::ShowMessage("PowerSystemConfigPanel::LoadData() entered\n");
   #endif

   // Set object pointer for "Show Script"
   mObject = theObject;

   Integer paramID;

   // Load the epoch formats
   StringArray reps = TimeSystemConverter::Instance()->GetValidTimeRepresentations();
   for (unsigned int i = 0; i < reps.size(); i++)
      epochFormatComboBox->Append(reps[i].c_str());

   #ifdef DEBUG_POWERPANEL_LOAD
   MessageInterface::ShowMessage
      ("   epochFormatComboBox has loaded its data ...\n");
   #endif

   PowerSystem *thePowerSystem = (PowerSystem*) theObject;

   // load the epoch
   std::string epochFormat = theObject->GetStringParameter("EpochFormat");
   std::string theEpochStr = thePowerSystem->GetEpochString();

   #ifdef DEBUG_POWERPANEL_LOAD
   MessageInterface::ShowMessage
      ("   epochFormat=%s, theEpochStr=%s\n", epochFormat.c_str(), theEpochStr.c_str());
   #endif

   epochFormatComboBox->SetValue(wxString(epochFormat.c_str()));
   fromEpochFormat = epochFormat;

   #ifdef DEBUG_POWERPANEL_LOAD
   MessageInterface::ShowMessage
      ("   loaded the epoch format ...\n");
   #endif
   // Save to TAIModJulian string to avoid keep reading the field
   // and convert to proper format when ComboBox is changed.
   if (epochFormat == "TAIModJulian")
   {
      taiMjdStr = theEpochStr;
   }
   else
   {
      Real fromMjd = -999.999;
      Real outMjd;
      std::string outStr;
      #ifdef DEBUG_POWERPANEL_LOAD
      MessageInterface::ShowMessage
         ("  about to convert from the epoch format %s to TAIModJulian ...\n",
               fromEpochFormat.c_str());
      #endif
      TimeSystemConverter::Instance()->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                 "TAIModJulian", outMjd, outStr);
      taiMjdStr = outStr;

      #ifdef DEBUG_POWERPANEL_LOAD
      MessageInterface::ShowMessage("   taiMjdStr=%s\n", taiMjdStr.c_str());
      #endif
   }
   epochStr = theEpochStr;
   epochTxtCtrl->SetValue(theEpochStr.c_str());

   // Initial Max Power
   Real initPower = theObject->GetRealParameter("InitialMaxPower");
   initPowerTxtCtrl->SetValue(ToString(initPower));

   // Decay Rate
   Real decayRate = theObject->GetRealParameter("AnnualDecayRate");
   decayRateTxtCtrl->SetValue(ToString(decayRate));

   // Margin
   Real margin = theObject->GetRealParameter("Margin");
   powerMarginTxtCtrl->SetValue(ToString(margin));

   // BusCoeff
   Real bus1 = theObject->GetRealParameter("BusCoeff1");
   busCoeff1TxtCtrl->SetValue(ToString(bus1));
   Real bus2 = theObject->GetRealParameter("BusCoeff2");
   busCoeff2TxtCtrl->SetValue(ToString(bus2));
   Real bus3 = theObject->GetRealParameter("BusCoeff3");
   busCoeff3TxtCtrl->SetValue(ToString(bus3));

   if (isSolar)
   {
      // Load the shadow model options
      StringArray sModels;  // should get these from the PowerSystem object
      sModels.push_back("None");
      sModels.push_back("DualCone");

      for (unsigned int i = 0; i < sModels.size(); i++)
         shadowModelComboBox->Append(sModels[i].c_str());
      std::string sModelStr    = theObject->GetStringParameter("ShadowModel");
      shadowModelComboBox->SetValue(wxString(sModelStr.c_str()));

      // Shadow Bodies
      shadowBodiesList = theObject->GetStringArrayParameter("ShadowBodies");
      shadowBodiesTxtCtrl->Clear();
      if (!shadowBodiesList.empty())
      {
         for (unsigned int i = 0; i < shadowBodiesList.size(); i++)
         {
            wxString bodyName = shadowBodiesList.at(i).c_str();
            shadowBodiesTxtCtrl->AppendText(bodyName + " ");
         }
      }

      // Solar Coeffs
      Real sCoeff = theObject->GetRealParameter("SolarCoeff1");
      solarCoeff1TxtCtrl->SetValue(ToString(sCoeff));
      sCoeff = theObject->GetRealParameter("SolarCoeff2");
      solarCoeff2TxtCtrl->SetValue(ToString(sCoeff));
      sCoeff = theObject->GetRealParameter("SolarCoeff3");
      solarCoeff3TxtCtrl->SetValue(ToString(sCoeff));
      sCoeff = theObject->GetRealParameter("SolarCoeff4");
      solarCoeff4TxtCtrl->SetValue(ToString(sCoeff));
      sCoeff = theObject->GetRealParameter("SolarCoeff5");
      solarCoeff5TxtCtrl->SetValue(ToString(sCoeff));
   }


   #ifdef DEBUG_POWERPANEL_LOAD
   MessageInterface::ShowMessage("PowerSystemConfigPanel::LoadData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::SaveData()
{
   #ifdef DEBUG_POWERPANEL_SAVE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData() entered\n");
   #endif
   // create local copy of mObject
   if (localObject != NULL)
   {
	   delete localObject;
   }
   localObject = mObject->Clone();
   SaveData(localObject);

   // if no errors, save again
   if (canClose)
   {
      #ifdef DEBUG_POWERPANEL_SAVE
      MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData() copying data back to object\n");
      #endif
	   theObject->Copy(localObject);

      #ifdef DEBUG_POWERPANEL_SAVE
      MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData() about to reset changed flags\n");
      #endif
      // reset changed flags
	   ResetChangedFlags();
   }

   #ifdef DEBUG_POWERPANEL_SAVE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::SaveData(GmatBase *theObject)
{
   #ifdef DEBUG_POWERPANEL_SAVE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) entered\n");
   #endif

   canClose = true;
   std::string str;
   Real initPower, decayRate, margin, bus1, bus2, bus3;
   Real solar1, solar2, solar3, solar4, solar5;
   bool realDataChanged = false;
   bool isValid = true;

   //-----------------------------------------------------------------
   // check values from text fields
   //-----------------------------------------------------------------
   if (isInitPowerChanged || isDecayRateChanged || isMarginChanged ||
       isBusCoeffChanged || (isSolar && isSolarCoeffChanged))
   {
      #ifdef DEBUG_POWERPANEL_SAVE
      MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) - there has been a text mod!!\n");
      #endif
      str = initPowerTxtCtrl->GetValue();
      isValid = CheckReal(initPower, str, "InitialMaxPower", "Real Number >= 0", false,
            true, true, true);

      str = decayRateTxtCtrl->GetValue();
      isValid = CheckReal(decayRate, str, "AnnualDecayRate", "0 <= Real Number <= 100");
      if (isValid)
         isValid = CheckRealRange(str, decayRate, "AnnualDecayRate", 0.0, 100.0, true, true, true, true);


      str = powerMarginTxtCtrl->GetValue();
      isValid = CheckReal(margin, str, "Margin", "0 <= Real Number <= 100");
      if (isValid)
         isValid = CheckRealRange(str, margin, "Margin", 0.0, 100.0, true, true, true, true);


      str = busCoeff1TxtCtrl->GetValue();
      isValid = CheckReal(bus1, str, "BusCoeff1", "Real Number");
      str = busCoeff2TxtCtrl->GetValue();
      isValid = CheckReal(bus2, str, "BusCoeff2", "Real Number");
      str = busCoeff3TxtCtrl->GetValue();
      isValid = CheckReal(bus3, str, "BusCoeff3", "Real Number");


      #ifdef DEBUG_POWERPANEL_SAVE
      MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) - isSolar = %s!!\n",
            (isSolar? "true" : "false"));
      #endif
      if (isSolar)
      {
         str = solarCoeff1TxtCtrl->GetValue();
         isValid = CheckReal(solar1, str, "SolarCoeff1", "Real Number");
         str = solarCoeff2TxtCtrl->GetValue();
         isValid = CheckReal(solar2, str, "SolarCoeff2", "Real Number");
         str = solarCoeff3TxtCtrl->GetValue();
         isValid = CheckReal(solar3, str, "SolarCoeff3", "Real Number");
         str = solarCoeff4TxtCtrl->GetValue();
         isValid = CheckReal(solar4, str, "SolarCoeff4", "Real Number");
         str = solarCoeff5TxtCtrl->GetValue();
         isValid = CheckReal(solar5, str, "SolarCoeff5", "Real Number");
      }
      realDataChanged = true;
   }


   if (!canClose)
      return;

   try
   {
      Integer paramID;

      std::string newEpoch    = epochTxtCtrl->GetValue().WX_TO_STD_STRING;
      std::string epochFormat = epochFormatComboBox->GetValue().WX_TO_STD_STRING;
      Real        fromMjd     = -999.999;
      Real        a1mjd       = -999.999;
      std::string outStr;

      #ifdef DEBUG_POWERPANEL_SAVE
      MessageInterface::ShowMessage
         ("   newEpoch=%s, epochFormat=%s\n", newEpoch.c_str(), epochFormat.c_str());
      #endif

      // Save epoch format and epoch
      if (isEpochFormatChanged || isEpochChanged || isEpochTextChanged)
      {
            bool timeOK = CheckTimeFormatAndValue(epochFormat, newEpoch,
                  "InitialEpoch", true);
            #ifdef DEBUG_POWERPANEL_SAVE
            MessageInterface::ShowMessage
               ("   timeOK = %s\n", (timeOK? "YES!" : "no"));
            #endif
            if (timeOK)
            {
               #ifdef DEBUG_POWERPANEL_SAVE
               MessageInterface::ShowMessage
                  ("   About to set format and time on the object ---------\n");
               #endif
               paramID = theObject->GetParameterID("EpochFormat");
               theObject->SetStringParameter(paramID, epochFormat);
               paramID = theObject->GetParameterID("InitialEpoch");
               theObject->SetStringParameter(paramID, newEpoch);
               #ifdef DEBUG_POWERPANEL_SAVE
               MessageInterface::ShowMessage
                  ("   DONE setting format and time on the object ---------\n");
               #endif
               isEpochFormatChanged = false;
               isEpochChanged       = false;
               isEpochTextChanged   = false;
            }
            else
            {
               #ifdef DEBUG_POWERPANEL_SAVE
                  MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData() setting canClose to false inside (epoch) try\n");
               #endif
               canClose = false;
            }
      }
      // Save Real Data
      if (realDataChanged)
      {
         #ifdef DEBUG_POWERPANEL_SAVE
         MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) - attempting to real data!!\n");
         #endif
         paramID = theObject->GetParameterID("InitialMaxPower");
         theObject->SetRealParameter(paramID, initPower);

         paramID = theObject->GetParameterID("AnnualDecayRate");
         theObject->SetRealParameter(paramID, decayRate);

         paramID = theObject->GetParameterID("Margin");
         theObject->SetRealParameter(paramID, margin);

         paramID = theObject->GetParameterID("BusCoeff1");
         theObject->SetRealParameter(paramID, bus1);
         paramID = theObject->GetParameterID("BusCoeff2");
         theObject->SetRealParameter(paramID, bus2);
         paramID = theObject->GetParameterID("BusCoeff3");
         theObject->SetRealParameter(paramID, bus3);
         if (isSolar)
         {
            paramID = theObject->GetParameterID("SolarCoeff1");
            theObject->SetRealParameter(paramID, solar1);
            paramID = theObject->GetParameterID("SolarCoeff2");
            theObject->SetRealParameter(paramID, solar2);
            paramID = theObject->GetParameterID("SolarCoeff3");
            theObject->SetRealParameter(paramID, solar3);
            paramID = theObject->GetParameterID("SolarCoeff4");
            theObject->SetRealParameter(paramID, solar4);
            paramID = theObject->GetParameterID("SolarCoeff5");
            theObject->SetRealParameter(paramID, solar5);
         }
      }
      if (isSolar)
      {
         #ifdef DEBUG_POWERPANEL_SAVE
         MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) - attempting to set solar data!!\n");
         MessageInterface::ShowMessage("isShadowModelChanged = %s\n", (isShadowModelChanged? "true" : "false"));
         #endif
         if (isShadowModelChanged)
         {
            std::string toShadowModel  = shadowModelComboBox->GetValue().WX_TO_STD_STRING;
            paramID = theObject->GetParameterID("ShadowModel");
            theObject->SetStringParameter(paramID, toShadowModel);
         }
         if (isBodyListChanged)
         {
            paramID = theObject->GetParameterID("ShadowBodies");
            theObject->TakeAction("ClearShadowBodies");
            // An empty list is allowed
            if (shadowBodiesList.empty())
            {
               theObject->SetStringParameter(paramID, "{}", 0);
            }
            else
            {
               for (unsigned int ii = 0; ii < shadowBodiesList.size(); ii++)
                  theObject->SetStringParameter(paramID, shadowBodiesList[ii], ii);
            }
         }
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }

   #ifdef DEBUG_POWERPANEL_SAVE
   MessageInterface::ShowMessage("PowerSystemConfigPanel::SaveData(obj) exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::OnTextChange(wxCommandEvent &event)
{
   if (epochTxtCtrl->IsModified())
      isEpochTextChanged = true;
   if (initPowerTxtCtrl->IsModified())
      isInitPowerChanged = true;
   if (decayRateTxtCtrl->IsModified())
      isDecayRateChanged = true;
   if (powerMarginTxtCtrl->IsModified())
      isMarginChanged = true;
   if (busCoeff1TxtCtrl->IsModified() || busCoeff2TxtCtrl->IsModified() ||
       busCoeff1TxtCtrl->IsModified())
      isBusCoeffChanged = true;
   if (isSolar)
   {
      if (solarCoeff1TxtCtrl->IsModified() || solarCoeff2TxtCtrl->IsModified() ||
          solarCoeff3TxtCtrl->IsModified() || solarCoeff4TxtCtrl->IsModified() ||
          solarCoeff5TxtCtrl->IsModified())
         isSolarCoeffChanged = true;
   }

   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::OnComboBoxChange(wxCommandEvent &event)
{
   std::string toEpochFormat  = epochFormatComboBox->GetValue().WX_TO_STD_STRING;

   #ifdef DEBUG_POWERSYSTEM_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("\nPowerSystemConfigPanel::OnComboBoxChange() toEpochFormat=%s\n",
            toEpochFormat.c_str());
   #endif

   //-----------------------------------------------------------------
   // epoch format change
   //-----------------------------------------------------------------
   if (event.GetEventObject() == epochFormatComboBox)
   {
      isEpochChanged = true;

      #ifdef DEBUG_POWERSYSTEM_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("\nPowerSystemConfigPanel::OnComboBoxChange() attempting to change epoch format ...\n");
      #endif
      try
      {
         Real fromMjd = -999.999;
         Real outMjd;
         std::string outStr;

         // if modified by user, check if epoch is valid first
         if (isEpochTextChanged)
         {
            std::string theEpochStr = epochTxtCtrl->GetValue().WX_TO_STD_STRING;

            // Save to TAIModJulian string to avoid keep reading the field
            // and convert to proper format when ComboBox is changed.
            if (fromEpochFormat == "TAIModJulian")
            {
               taiMjdStr = theEpochStr;
            }
            else
            {
               TimeSystemConverter::Instance()->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                          "TAIModJulian", outMjd, outStr);
               taiMjdStr = outStr;
            }

            // Convert to desired format with new date
            TimeSystemConverter::Instance()->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                       toEpochFormat, outMjd, outStr);

            epochTxtCtrl->SetValue(outStr.c_str());
            isEpochChanged = false;
            fromEpochFormat = toEpochFormat;
         }
         else
         {
            #ifdef DEBUG_POWERSYSTEM_PANEL_COMBOBOX
            MessageInterface::ShowMessage
               ("\nPowerSystemConfigPanel::OnComboBoxChange() converting from %s to %s\n",
                     "TAIModJulian", toEpochFormat.c_str());
            #endif
            // Convert to desired format using TAIModJulian date
            TimeSystemConverter::Instance()->Convert("TAIModJulian", fromMjd, taiMjdStr,
                                       toEpochFormat, outMjd, outStr);

            epochTxtCtrl->SetValue(outStr.c_str());
            fromEpochFormat = toEpochFormat;
         }
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(fromEpochFormat.c_str());
         MessageInterface::PopupMessage
            (Gmat::ERROR_, e.GetFullMessage() +
             "\nPlease enter valid Epoch before changing the Epoch Format\n");
      }
   }
   if (isSolar)
   {
      if (event.GetEventObject() == shadowModelComboBox)
      {
         std::string toShadowModel  = shadowModelComboBox->GetValue().WX_TO_STD_STRING;
         isShadowModelChanged = true;
      }
   }
   EnableUpdate(true);
   #ifdef DEBUG_POWERSYSTEM_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("\nPowerSystemConfigPanel::OnComboBoxChange() EXITing\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnBodiesEditButton()
//------------------------------------------------------------------------------
void PowerSystemConfigPanel::OnBodiesEditButton(wxCommandEvent &event)
{
   bool isModified = false;

   wxArrayString toExclude;
   wxArrayString toHide;

   // don't want the sun to shadow itself
   toHide.push_back((GmatSolarSystemDefaults::SUN_NAME).c_str());

   for (unsigned int ii = 0; ii < shadowBodiesList.size(); ii++)
   {
      wxString bodyStr = shadowBodiesList.at(ii).c_str();
      toExclude.Add(bodyStr);
   }

   CelesBodySelectDialog cbDialog(this, toExclude, toHide);
   cbDialog.ShowModal();
   if (cbDialog.IsBodySelected())
   {
      wxArrayString &names = cbDialog.GetBodyNames();

      if (names.IsEmpty())
      {
         shadowBodiesList.clear();
         shadowBodiesTxtCtrl->Clear();
         EnableUpdate(true);
         isBodyListChanged = true;
         return;
      }

      wxString bodyName;

      shadowBodiesList.clear();
      shadowBodiesTxtCtrl->Clear();

      //--------------------------
      // Add bodies to shadowBodiesList
      //--------------------------
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         shadowBodiesList.push_back(std::string(bodyName.c_str()));
      }

      if (!shadowBodiesList.empty())
      {
         for (unsigned int i = 0; i < shadowBodiesList.size(); i++)
         {
            wxString bodyName = shadowBodiesList.at(i).c_str();
            shadowBodiesTxtCtrl->AppendText(bodyName + " ");
         }
      }
      EnableUpdate(true);
      isBodyListChanged = true;
   }
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts a real number to a wxString.
 *
 * @param <rval> real number to convert
 *
 * @return wxString representation of the input real number
 */
//------------------------------------------------------------------------------
wxString PowerSystemConfigPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


void PowerSystemConfigPanel::ResetChangedFlags()
{
   isEpochFormatChanged   = false;
   isEpochChanged         = false;
   isEpochTextChanged     = false;
   isInitPowerChanged     = false;
   isDecayRateChanged     = false;
   isMarginChanged        = false;
   isShadowModelChanged   = false;
   isBodyListChanged      = false;
   isBusCoeffChanged      = false;
   isSolarCoeffChanged    = false;
}
