//$Id$
//------------------------------------------------------------------------------
//                            GroundStationPanel
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
// Author: Thomas Grubb
// Created: 2010/03/11
// Modified: 
//    2010.03.26 Thomas Grubb 
//      - Fixed positive real number check in SaveData for Longitude
/**
 * This class contains information needed to set up users ground station
 * parameters.
 */
//------------------------------------------------------------------------------
#include "GroundStationPanel.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatColorPanel.hpp"
#include "BodyFixedStateConverter.hpp"
#include "CelestialBody.hpp"
#include "GuiInterpreter.hpp"
#include <wx/config.h>
#include "StringUtil.hpp"
#include <wx/variant.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(GroundStationPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_LOCATION_TEXTCTRL, GroundStationPanel::OnLocationTextChange)
   EVT_TEXT(ID_STATION_ID_TEXTCTRL, GroundStationPanel::OnStationIDTextChange)
//   EVT_TEXT(ID_HARDWARE_TEXTCTRL, GroundStationPanel::OnHardwareTextChange)
   EVT_TEXT(ID_ELEVATION_TEXTCTRL, GroundStationPanel::OnElevationTextChange)
   EVT_COMBOBOX(ID_COMBOBOX, GroundStationPanel::OnComboBoxChange)
   EVT_COMBOBOX(ID_STATE_TYPE_COMBOBOX, GroundStationPanel::OnStateTypeComboBoxChange)
   EVT_COMBOBOX(ID_HORIZON_REFERENCE_COMBOBOX, GroundStationPanel::OnHorizonReferenceComboBoxChange)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GroundStationPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs GroundStationPanel object.
 */
//------------------------------------------------------------------------------
GroundStationPanel::GroundStationPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{           
   std::string groundName = std::string(name.c_str());
   theGroundStation = (GroundstationInterface*)theGuiInterpreter->GetConfiguredObject(groundName);
   guiManager     = GuiItemManager::GetInstance();
   guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   ss             = guiInterpreter->GetSolarSystemInUse();
   
   if (theGroundStation)
   {
      Create();
      Show();
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot find the GroundStation object named " + groundName);
   }
}

//------------------------------------------------------------------------------
// ~GroundStationPanel()
//------------------------------------------------------------------------------
GroundStationPanel::~GroundStationPanel()
{
   GuiItemManager::GetInstance()->UnregisterComboBox("CelestialBody", centralBodyComboBox);
   delete localGroundStation;
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
void GroundStationPanel::Create()
{
   // create local copy of ground station
   localGroundStation = (GroundstationInterface*)theGroundStation->Clone();
   
   // Border size
   int minLabelSize;
   Integer bsize = 2;
   Integer labelSizeProportion = 0;
   Integer ctrlSizeProportion = 1;
   Integer unitSizeProportion = 0;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Ground Station"));

   //-----------------------------------------------------------------
   // Create controls in tab order
   //-----------------------------------------------------------------
   // Station ID
   wxStaticText *stationIDLabel =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"ID" );
   stationIDTextCtrl =
      new wxTextCtrl( this, ID_STATION_ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
   // Since Groundstation is now in plugin, this no longer works:
//   stationIDTextCtrl->SetToolTip(pConfig->Read(_T((GroundStation::PARAMETER_TEXT[GroundStation::STATION_ID-GroundStation::STATION_ID]+"Hint").c_str())));
   stationIDTextCtrl->SetToolTip(pConfig->Read(_T("IdHint")));
   
   // Central Body
   wxStaticText *centralBodyLabel =
      new wxStaticText( this, ID_TEXT, "Central " GUI_ACCEL_KEY "Body");
   StringArray centralBodyList;
   centralBodyList.push_back("Earth");
   wxArrayString wxCentralBodyLabels = ToWxArrayString(centralBodyList);
//   centralBodyComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(120,-1),
//         wxCentralBodyLabels, wxCB_DROPDOWN|wxCB_READONLY);
// for now, we are only allowing Earth as the central body; when code is updated to use any body
// as central body, un-comment the following code:
   centralBodyComboBox = GuiItemManager::GetInstance()->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                                              wxSize(150,-1));
   std::string toolTip = BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::CENTRAL_BODY-BodyFixedPoint::CENTRAL_BODY]+"Hint";
   centralBodyComboBox->SetToolTip(pConfig->Read(wxString(toolTip.c_str())));
   // centralBodyComboBox->SetToolTip(pConfig->Read(_T((BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::CENTRAL_BODY-BodyFixedPoint::CENTRAL_BODY]+"Hint").c_str())));
   
   // State Type
   wxStaticText *stateTypeLabel =
      new wxStaticText( this, ID_TEXT, "State " GUI_ACCEL_KEY "Type");
   StringArray stateTypeList =
       localGroundStation->GetPropertyEnumStrings(BodyFixedPoint::STATE_TYPE);
   wxArrayString wxStateTypeLabels = ToWxArrayString(stateTypeList);
   stateTypeComboBox = 
      new wxComboBox( this, ID_STATE_TYPE_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(120,-1),
                      wxStateTypeLabels, wxCB_DROPDOWN|wxCB_READONLY);
   toolTip = BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::STATE_TYPE-BodyFixedPoint::CENTRAL_BODY]+"Hint";
   stateTypeComboBox->SetToolTip(pConfig->Read(wxString(toolTip.c_str())));
   //stateTypeComboBox->SetToolTip(pConfig->Read(_T((BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::STATE_TYPE-BodyFixedPoint::CENTRAL_BODY]+"Hint").c_str())));
   
   // Horizon Reference
   wxStaticText *horizonReferenceLabel =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Horizon Reference");
   StringArray horizonReferenceList =
       localGroundStation->GetPropertyEnumStrings(BodyFixedPoint::HORIZON_REFERENCE);
   wxArrayString wxHorizonReferenceLabels = ToWxArrayString(horizonReferenceList);
   horizonReferenceComboBox = 
      new wxComboBox( this, ID_HORIZON_REFERENCE_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(120,-1),
                      wxHorizonReferenceLabels, wxCB_DROPDOWN|wxCB_READONLY);
   toolTip = BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::HORIZON_REFERENCE-BodyFixedPoint::CENTRAL_BODY]+"Hint";
   horizonReferenceComboBox->SetToolTip(pConfig->Read(wxString(toolTip.c_str())));
   // horizonReferenceComboBox->SetToolTip(pConfig->Read(_T((BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::HORIZON_REFERENCE-BodyFixedPoint::CENTRAL_BODY]+"Hint").c_str())));
   
   // Location 1
   location1Label =
      new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1).c_str());
   location1TextCtrl =
      new wxTextCtrl( this, ID_LOCATION_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   //location1TextCtrl->SetToolTip(pConfig->Read(wxT(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1)+"Hint")));
   location1Unit =
      new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_1).c_str());
   
   // Location 2
   location2Label =
       new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2).c_str());
   location2TextCtrl =
      new wxTextCtrl( this, ID_LOCATION_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   //location2TextCtrl->SetToolTip(pConfig->Read(wxT(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2)+"Hint")));
   location2Unit =
       new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_2).c_str());
   
   // Location 3
   location3Label =
       new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3).c_str());
   location3TextCtrl =
      new wxTextCtrl( this, ID_LOCATION_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   //location3TextCtrl->SetToolTip(pConfig->Read(_T(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3)+"Hint")));
   location3Unit =
       new wxStaticText( this, ID_TEXT, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_3).c_str());
   
   // Hardware
   //hardwareLabel =
   //    new wxStaticText( this, ID_TEXT, wxT(localGroundStation->GetStringParameter(BodyFixedPoint::HARDWARE)));
//   wxStaticText *hardwareLabel =
//       new wxStaticText( this, ID_TEXT, wxT("Hardware"));
//   hardwareTextCtrl =
//      new wxTextCtrl( this, ID_HARDWARE_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
//   //hardwareTextCtrl->SetToolTip(pConfig->Read(_T(BodyFixedPoint::PARAMETER_TEXT[BodyFixedPoint::HARDWARE-BodyFixedPoint::CENTRAL_BODY]"Hint")));
//   hardwareTextCtrl->SetToolTip(pConfig->Read(_T("HardwareHint")));
   
   // update labels and tooltips based on statetype
   UpdateControls();

   // set the min width for one of the labels for each GmatStaticBoxSizer
   minLabelSize = stationIDLabel->GetBestSize().x;
   minLabelSize = (minLabelSize < centralBodyLabel->GetBestSize().x) ? centralBodyLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < stateTypeLabel->GetBestSize().x) ? stateTypeLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < horizonReferenceLabel->GetBestSize().x) ? horizonReferenceLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < location1Label->GetBestSize().x) ? location1Label->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < location2Label->GetBestSize().x) ? location2Label->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < location3Label->GetBestSize().x) ? location3Label->GetBestSize().x : minLabelSize;
   
   stationIDLabel->SetMinSize(wxSize(minLabelSize, stationIDLabel->GetMinHeight()));
   centralBodyLabel->SetMinSize(wxSize(minLabelSize, centralBodyLabel->GetMinHeight()));

   wxStaticText *minElLabel = new wxStaticText(this, ID_TEXT, "Min. Elevation:");
   minElLabel->SetMinSize(wxSize(minLabelSize, minElLabel->GetMinHeight()));
   minElevationCtrl = new wxTextCtrl(this, ID_ELEVATION_TEXTCTRL,
      wxT(""), wxDefaultPosition, wxSize(120, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   wxStaticText *minElUnitLabel = new wxStaticText(this, ID_TEXT, "deg");

   //-----------------------------------------------------------------
   // Add to Station ID sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   flexGridSizer1->Add(stationIDLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(stationIDTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   GmatStaticBoxSizer *idSizer = new GmatStaticBoxSizer(wxVERTICAL, this , "");
   idSizer->Add(flexGridSizer1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

   wxFlexGridSizer *elevationGridSizer = new wxFlexGridSizer(3, 0, 0);
   elevationGridSizer->Add(minElLabel, labelSizeProportion, wxALIGN_LEFT | wxALL, bsize);
   elevationGridSizer->Add(minElevationCtrl, ctrlSizeProportion, wxGROW | wxALL, bsize);
   elevationGridSizer->Add(minElUnitLabel, unitSizeProportion, wxGROW | wxALL, bsize);
   idSizer->Add(elevationGridSizer, wxGROW | wxALIGN_CENTRE | wxALL, bsize);

   //-----------------------------------------------------------------
   // Add to location properties sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 3, 0, 0 );
   
   flexGridSizer2->Add(centralBodyLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(centralBodyComboBox, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(0, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   flexGridSizer2->Add(stateTypeLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(stateTypeComboBox, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(0, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   flexGridSizer2->Add(horizonReferenceLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(horizonReferenceComboBox, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(0, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   flexGridSizer2->Add(location1Label, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(location1TextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(location1Unit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   flexGridSizer2->Add(location2Label, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(location2TextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(location2Unit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   flexGridSizer2->Add(location3Label, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(location3TextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(location3Unit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   // create the location Properties group box
   locationPropertiesSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Location" );
   locationPropertiesSizer->Add(flexGridSizer2, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   
   // Create color panel
   GmatColorPanel *colorPanel = new GmatColorPanel(this, this, localGroundStation);
   
   //-----------------------------------------------------------------
   // Now put id, location and color into the middle sizer
   //-----------------------------------------------------------------
   theMiddleSizer->Add(idSizer, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(locationPropertiesSizer, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add(colorPanel, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->SetSizeHints(this);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void GroundStationPanel::LoadData()
{ 
   if (localGroundStation == NULL)
      return; 
   
   // Set object pointer for "Show Script"
   mObject = theGroundStation;

   try
   {
   // Since Groundstation is now in plugin, this no longer works:
//      stationIDTextCtrl->SetValue(ToWxString(localGroundStation->GetStringParameter(GroundStation::STATION_ID).c_str()));
      stationIDTextCtrl->SetValue(ToWxString(localGroundStation->GetStringParameter(localGroundStation->GetParameterID("Id")).c_str()));
      minElevation = localGroundStation->GetRealParameter(localGroundStation->GetParameterID("MinimumElevationAngle"));
      minElevationCtrl->SetValue(ToWxString(minElevation));
      centralBodyComboBox->SetValue(ToWxString(localGroundStation->GetStringParameter(BodyFixedPoint::CENTRAL_BODY).c_str()));
      currentStateType = localGroundStation->GetStringParameter(BodyFixedPoint::STATE_TYPE);
      stateTypeComboBox->SetValue(ToWxString(currentStateType.c_str()));
      currentHorizonReference = localGroundStation->GetStringParameter(BodyFixedPoint::HORIZON_REFERENCE);
      horizonReferenceComboBox->SetValue(ToWxString(currentHorizonReference.c_str()));
      location1 = localGroundStation->GetRealParameter(BodyFixedPoint::LOCATION_1);
      location1TextCtrl->SetValue(ToWxString(location1));
      location2 = localGroundStation->GetRealParameter(BodyFixedPoint::LOCATION_2);
      location2TextCtrl->SetValue(ToWxString(location2));
      location3 = localGroundStation->GetRealParameter(BodyFixedPoint::LOCATION_3);
      location3TextCtrl->SetValue(ToWxString(location3));
      
      // update labels and tooltips based on statetype
      UpdateControls();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void GroundStationPanel::SaveData()
{
   canClose = true;
   
   std::string inputString;
   std::string text;
   
   //-----------------------------------------------------------------
   // validate user input for locations if state type is not cartesian
   //   X,Y,Z May be any real
   //   Latitude is -90 to 90, Longitude 0 to 360, Altitude is any Real
   // NOTE - we are just checking for Real or non-negative real here;
   //        base code checks for actual ranges on SetrealParameter call
   //-----------------------------------------------------------------
   try
   {
      // State Type
      text = stateTypeComboBox->GetValue().c_str();
      localGroundStation->SetStringParameter(BodyFixedPoint::STATE_TYPE, text);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }

   // Location 1 (X or Latitude)
   inputString = location1TextCtrl->GetValue();
   CheckReal(location1, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1), "Real Number");
   
   // Location 2 (Y or Longitude)
   inputString = location2TextCtrl->GetValue();
   if (text != "Cartesian")
      CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number >= 0.0", false, true, true, true);
   else
      CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number");
   
   // Location 3 (Z or Altitude)
   inputString = location3TextCtrl->GetValue();
   CheckReal(location3, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3), "Real Number");

   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // Station ID
      text = stationIDTextCtrl->GetValue().c_str();
      localGroundStation->SetStringParameter(localGroundStation->GetParameterID("Id"), text);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
	   // Minimum elevation angle
	   minElevationCtrl->GetValue().ToCDouble(&minElevation);
      if ((minElevation <= 90.0) && (minElevation >= -90.0))
         localGroundStation->SetRealParameter(localGroundStation->GetParameterID("MinimumElevationAngle"), minElevation);
      else
      {
         std::string elstr = minElevationCtrl->GetValue().WX_TO_STD_STRING;
         CheckRealRange(elstr, minElevation, "MinimumElevationAngle", -90.0, 90.0, true, true, true, true);
         canClose = false;
      }
   }
   catch (BaseException &ex)
   {
	   MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
	   canClose = false;
   }
   try
   {
      // Central Body
      text = centralBodyComboBox->GetValue().c_str();
      localGroundStation->SetStringParameter(BodyFixedPoint::CENTRAL_BODY, text);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
      // Horizon Reference
      text = horizonReferenceComboBox->GetValue().c_str();
      localGroundStation->SetStringParameter(BodyFixedPoint::HORIZON_REFERENCE, text);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
      // location 1
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_1, location1);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
      // location 2
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_2, location2);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
      // location 3
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_3, location3);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   try
   {
      // Hardware
//      text = hardwareTextCtrl->GetValue().c_str();
      //localGroundStation->SetStringParameter(GroundStation::HARDWARE, text);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }

   if (!canClose)
      return;
   // copy the current info into the ground station
   theGroundStation->Copy(localGroundStation);
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void UpdateControls()
//------------------------------------------------------------------------------
void GroundStationPanel::UpdateControls()
{
//   bool enableHorizon = localGroundStation->GetStringParameter(BodyFixedPoint::STATE_TYPE) != "Cartesian";
   bool enableHorizon = currentStateType != "Cartesian";
   horizonReferenceComboBox->Enable(enableHorizon);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Ground Station"));

   location1Label->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1).c_str());
   location1TextCtrl->SetToolTip(pConfig->Read(wxString((localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1)+"Hint").c_str())));
   location1Unit->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_1).c_str());
   location2Label->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2).c_str());
   location2TextCtrl->SetToolTip(pConfig->Read(wxString((localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2)+"Hint").c_str())));
   location2Unit->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_2).c_str());
   location3Label->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3).c_str());
   location3TextCtrl->SetToolTip(pConfig->Read(wxString((localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3)+"Hint").c_str())));
   location3Unit->SetLabel(localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_UNITS_3).c_str());
}    


//------------------------------------------------------------------------------
// void OnLocationTextChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnLocationTextChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}    


//------------------------------------------------------------------------------
// void OnStationIDTextChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnStationIDTextChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}    

void GroundStationPanel::OnElevationTextChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnHardwareTextChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnHardwareTextChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}    


//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnComboBoxChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}    


//------------------------------------------------------------------------------
// void OnStateTypeComboBoxChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnStateTypeComboBoxChange(wxCommandEvent &event)
{
   std::string sttype       = stateTypeComboBox->GetValue().WX_TO_STD_STRING;
   std::string inputString;
   Real        location1, location2, location3;
   if (sttype != currentStateType)
   {
      std::string bodyName = centralBodyComboBox->GetValue().WX_TO_STD_STRING;
      // get a pointer to the celestial body
      CelestialBody *body = ss->GetBody(bodyName);
      if (!body)
      {
         std::string errmsg = "Cannot find body ";
         errmsg += bodyName + " needed for GroundStation panel update.\n";
         throw GmatBaseException(errmsg);
      }
      Real meanRadius = body->GetRealParameter(body->GetParameterID("EquatorialRadius"));
      Real flattening = body->GetRealParameter(body->GetParameterID("Flattening"));
      // Convert location values to the appropriate values
      // Location 1 (Latitude)
      inputString = location1TextCtrl->GetValue();
      CheckReal(location1, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1), "Real Number");

      // Location 2 (Longitude)
      inputString = location2TextCtrl->GetValue();
      if (currentStateType != "Cartesian")
         CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number >= 0.0", false, true, true, true);
      else
         CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number");

      // Location 3 (Altitude)
      inputString = location3TextCtrl->GetValue();
      CheckReal(location3, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3), "Real Number");

      Rvector3 locInCurrent(location1, location2, location3);
      if (currentStateType == "Spherical") // latitude and longitude need to be passed in as radians
      {
         locInCurrent[0] *= GmatMathConstants::RAD_PER_DEG;
         locInCurrent[1] *= GmatMathConstants::RAD_PER_DEG;
      }
      Rvector3 locInNew = BodyFixedStateConverterUtil::Convert(locInCurrent, currentStateType,
                          currentHorizonReference, sttype, currentHorizonReference,
                          flattening, meanRadius);
      location1 = locInNew[0];
      location2 = locInNew[1];
      location3 = locInNew[2];
      if (sttype == "Spherical") // need to display DEGREES for latitude and longitude
      {
         location1 *= GmatMathConstants::DEG_PER_RAD;
         location2 *= GmatMathConstants::DEG_PER_RAD;
      }
      localGroundStation->SetStringParameter(BodyFixedPoint::STATE_TYPE, sttype);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_1, location1);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_2, location2);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_3, location3);
      location1TextCtrl->SetValue(ToWxString(location1));
      location2TextCtrl->SetValue(ToWxString(location2));
      location3TextCtrl->SetValue(ToWxString(location3));
      currentStateType = sttype;
   }
   UpdateControls();
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnHorizonReferenceComboBoxChange()
//------------------------------------------------------------------------------
void GroundStationPanel::OnHorizonReferenceComboBoxChange(wxCommandEvent &event)
{
   std::string horizon       = horizonReferenceComboBox->GetValue().WX_TO_STD_STRING;
   std::string inputString;
   Real        location1, location2, location3;
   if (horizon != currentHorizonReference)
   {
      std::string bodyName = centralBodyComboBox->GetValue().WX_TO_STD_STRING;
      // get a pointer to the celestial body
      CelestialBody *body = ss->GetBody(bodyName);
      if (!body)
      {
         std::string errmsg = "Cannot find body ";
         errmsg += bodyName + " needed for GroundStation panel update.\n";
         throw GmatBaseException(errmsg);
      }
      Real meanRadius = body->GetRealParameter(body->GetParameterID("EquatorialRadius"));
      Real flattening = body->GetRealParameter(body->GetParameterID("Flattening"));
      // Convert location values to the appropriate values
      // Location 1 (Latitude)
      inputString = location1TextCtrl->GetValue();
      CheckReal(location1, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_1), "Real Number");

      // Location 2 (Longitude)
      inputString = location2TextCtrl->GetValue();
      if (currentStateType != "Cartesian")
         CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number >= 0.0", false, true, true, true);
      else
         CheckReal(location2, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_2), "Real Number");

      // Location 3 (Altitude)
      inputString = location3TextCtrl->GetValue();
      CheckReal(location3, inputString, localGroundStation->GetStringParameter(BodyFixedPoint::LOCATION_LABEL_3), "Real Number");

      Rvector3 locInCurrent(location1, location2, location3);
      if (currentStateType == "Spherical") // latitude and longitude need to be passed in as radians
      {
         locInCurrent[0] *= GmatMathConstants::RAD_PER_DEG;
         locInCurrent[1] *= GmatMathConstants::RAD_PER_DEG;
      }
      //MessageInterface::ShowMessage(" ... Spherical to new horizon ... loc = %12.10f  %12.10f  %12.10f\n",
      //      locInCurrent[0], locInCurrent[1], locInCurrent[2]); // *************************
      Rvector3 locInNew = BodyFixedStateConverterUtil::Convert(locInCurrent, currentStateType,
                          currentHorizonReference, currentStateType, horizon,
                          flattening, meanRadius);
      //MessageInterface::ShowMessage(" ... result =  %12.10f  %12.10f  %12.10f\n",
      //      locInNew[0], locInNew[1], locInNew[2]); // *************************
      location1 = locInNew[0];
      location2 = locInNew[1];
      location3 = locInNew[2];
      if (currentStateType == "Spherical") // need to display DEGREES for latitude and longitude
      {
         location1 *= GmatMathConstants::DEG_PER_RAD;
         location2 *= GmatMathConstants::DEG_PER_RAD;
      }
      localGroundStation->SetStringParameter(BodyFixedPoint::HORIZON_REFERENCE, horizon);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_1, location1);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_2, location2);
      localGroundStation->SetRealParameter(BodyFixedPoint::LOCATION_3, location3);
      location1TextCtrl->SetValue(ToWxString(location1));
      location2TextCtrl->SetValue(ToWxString(location2));
      location3TextCtrl->SetValue(ToWxString(location3));
      currentHorizonReference = horizon;
   }
   UpdateControls();
   EnableUpdate(true);
}    



