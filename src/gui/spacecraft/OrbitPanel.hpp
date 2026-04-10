//$Id$
//------------------------------------------------------------------------------
//                            OrbitPanel
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
// Author: Allison Greene
// Created: 2004/04/08
/**
 * This class contains information needed to setup spacecraft orbit data.
 * 
 */
//------------------------------------------------------------------------------
#ifndef OrbitPanel_hpp
#define OrbitPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

#include "Spacecraft.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateConverter.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector6.hpp"
#include "OrbitDesignerDialog.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton


class OrbitPanel: public wxPanel
{
public:
   OrbitPanel(GmatPanel *scPanel, wxWindow *parent,
              Spacecraft *spacecraft, SolarSystem *solarsystem);
   ~OrbitPanel();
   
   void LoadData();
   void SaveData();
   
   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }
   
   bool RefreshComponents();

private:

   /// valid state type in the StateConversionUtil
   StringArray mStateTypeNames;
   /// valid anomaly type in the Anomaly
   StringArray mAnomalyTypeNames;
   /// names of the available coordinate systems
   StringArray coordSystemNames;
   
   Real        mAnomaly;
   Real        mTrueAnomaly;
   
   bool dataChanged;
   bool canClose;

   Spacecraft     *theSpacecraft;
   SolarSystem    *theSolarSystem;
   GuiItemManager *theGuiManager;
   GuiInterpreter *theGuiInterpreter;
   CoordinateSystem *mInternalCoord;
   CoordinateSystem *mOutCoord;
   CoordinateSystem *mFromCoord;
   TimeSystemConverter *theTimeConverter;
   
   Real mEpoch;
   
   bool mIsEpochFormatChanged;
   bool mIsCoordSysChanged;
   bool mIsStateTypeChanged;
   bool mIsAnomalyTypeChanged;
   bool mIsStateChanged;
   bool mIsStateModified[6]; // user typed in number
   bool mIsEpochChanged;
   bool mIsEpochModified;    // user typed in number
   bool mShowFullStateTypeList;
   
   /// The spacecraft state in the internal coordinate system
   Rvector6 mCartState;
   Rvector6 mTempCartState;
   /// The spacecraft state on the display
   Rvector6 mOutState;
   
   CoordinateConverter mCoordConverter;
   std::string mFromCoordSysStr;
   std::string mFromStateTypeStr;
   std::string mFromAnomalyTypeStr;
   std::string mFromEpochFormat;
   
   std::string mElementStrs[6];
   std::string mEpochStr;
   std::string mTaiMjdStr;
   std::string mAnomalyType;
   
   std::string errMsgFormat;
   GmatGlobal  *gg;

   void Create();
   void AddElements(wxWindow *parent);
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);

   void InitializeCoordinateSystem(CoordinateSystem *cs);
   void SetLabelsUnits(const std::string &stateType);

   void DisplayState();
   void BuildValidStateTypes(const std::string &forCS = "mOutCoord");
   void BuildValidCoordinateSystemList(const std::string &forStateType);
   void BuildState(const Rvector6 &inputState, bool isInternal = false);
   
   wxString ToString(Real rval);
   
   bool IsStateModified();
   void ResetStateFlags(bool discardEdits = false);
   
   bool CheckState(Rvector6 &outState);
   bool CheckCartesian(Rvector6 &outState);
   bool CheckKeplerian(Rvector6 &outState);
   bool CheckModKeplerian(Rvector6 &outState);
   //bool CheckSpherical(Rvector6 &outState, const wxString &stateType);
   bool CheckSphericalAZFPA(Rvector6 &outState);
   bool CheckSphericalRADEC(Rvector6 &outState);
   bool CheckEquinoctial(Rvector6 &outState);
   bool CheckModifiedEquinoctial(Rvector6 &outState);
   bool CheckAlternateEquinoctial(Rvector6 &outState);
   bool CheckDelaunay(Rvector6 &outState);
   bool CheckPlanetodetic(Rvector6 &outState);
   bool CheckOutgoingAsymptote(Rvector6 &outState); // mod by YK
   bool CheckIncomingAsymptote(Rvector6 &outState);
   bool CheckBrouwerMeanShort(Rvector6 &outState); 
   bool CheckBrouwerMeanLong(Rvector6 &outState);
   
   bool CheckAnomaly();
   bool ComputeTrueAnomaly(Rvector6 &inState, const std::string &stateType);
   
   Real GetOriginData(GmatBase *fromObject, const std::string &whichData = "mu");

   GmatPanel *theScPanel;
   
   wxStaticText *description1;
   wxStaticText *description2;
   wxStaticText *description3;
   wxStaticText *description4;
   wxStaticText *description5;
   wxStaticText *description6;
   
   wxStaticText *unit1;
   wxStaticText *unit2;
   wxStaticText *unit3;
   wxStaticText *unit4;
   wxStaticText *unit5;
   wxStaticText *unit6;
   
   wxButton *orbitDesignerButton;

   wxStaticText *anomalyStaticText;

   wxTextCtrl *textCtrl[6];   
   wxTextCtrl *epochValue;

   wxPanel *elementsPanel;

   wxComboBox *anomalyComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *epochFormatComboBox;
   wxComboBox *stateTypeComboBox;

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30200,
      ID_TEXTCTRL,
      ID_COMBOBOX,
	   ID_BUTTON,
        
      ID_STATIC_ELEMENT     
   };
};
#endif

