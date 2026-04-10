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
/**
 * This class contains information needed to set up ground station 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef GroundStationPanel_hpp
#define GroundStationPanel_hpp

// This panel is derived from GmatBaseSetupPanel
// 
#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "gmatdefs.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GroundstationInterface.hpp"
#include "BodyFixedPoint.hpp"
#include "GmatAppData.hpp"
#include "GuiItemManager.hpp"
#include "GuiInterpreter.hpp"
#include "SolarSystem.hpp"

class GroundStationPanel: public GmatPanel
{
public:
   GroundStationPanel(wxWindow *parent, const wxString &name);
   ~GroundStationPanel();
   
protected:
   virtual void UpdateControls();

private: 
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   GuiItemManager *guiManager;
   GuiInterpreter *guiInterpreter;
   SolarSystem    *ss;
   // object data
   std::string currentStateType;
   std::string currentHorizonReference;
   Real        location1, location2, location3;
   Real        minElevation;

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnLocationTextChange(wxCommandEvent &event);
   void OnStationIDTextChange(wxCommandEvent &event);
   void OnHardwareTextChange(wxCommandEvent &event);
   void OnElevationTextChange(wxCommandEvent &event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnStateTypeComboBoxChange(wxCommandEvent &event);
   void OnHorizonReferenceComboBoxChange(wxCommandEvent &event);
   
   GroundstationInterface* theGroundStation;
   GroundstationInterface* localGroundStation;
   
   wxTextCtrl *stationIDTextCtrl;
   wxTextCtrl *hardwareTextCtrl;
   wxStaticText *location1Label;
   wxTextCtrl *location1TextCtrl;
   wxStaticText *location1Unit;
   wxStaticText *location2Label;
   wxTextCtrl *location2TextCtrl;
   wxStaticText *location2Unit;
   wxStaticText *location3Label;
   wxTextCtrl *location3TextCtrl;
   wxStaticText *location3Unit;

   wxTextCtrl *minElevationCtrl;
   
   wxComboBox *centralBodyComboBox;
   wxComboBox *stateTypeComboBox;
   wxComboBox *horizonReferenceComboBox;
   GmatStaticBoxSizer *locationPropertiesSizer;
   GmatStaticBoxSizer *hardwarePropertiesSizer;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 39010,
      ID_LOCATION_TEXTCTRL,
      ID_STATION_ID_TEXTCTRL,
	  ID_HARDWARE_TEXTCTRL,
	  ID_ELEVATION_TEXTCTRL,
	  ID_BUTTON,
      ID_COMBOBOX,
      ID_STATE_TYPE_COMBOBOX,
      ID_HORIZON_REFERENCE_COMBOBOX
   };
};

#endif // GroundStationPanel_hpp
