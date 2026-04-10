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
// Author: Linda Jun
// Created: 2009.02.11
/**
 * This class sets up Thruster or ImpulsiveBurn parameters.
 */
//------------------------------------------------------------------------------
#ifndef BurnThrusterPanel_hpp
#define BurnThrusterPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "gmatdefs.hpp"
#include "GmatStaticBoxSizer.hpp"

class BurnThrusterPanel: public GmatPanel
{
public:
   BurnThrusterPanel(wxWindow *parent, const wxString &name);
   virtual ~BurnThrusterPanel();
   
   // Event Handling
   void OnTextChange(wxCommandEvent &event);
   void OnCheckBoxChange(wxCommandEvent &event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent &event);
   void UpdateOriginAxes();
   
protected:
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void SaveData(GmatBase *theObject);
   
   void         EnableDataForThrustModel(const std::string &tModel);

   // local copy of object for verifying changes before commit/apply
   GmatBase *localObject;
   GmatBase* theObject;

   // Chemical Thruster
   RealArray   cCoefs;
   RealArray   kCoefs;
   StringArray cCoefNames;
   StringArray kCoefNames;
   // Electric Thruster
   RealArray   tCoefs;
   RealArray   mfCoefs;
   StringArray tCoefNames;
   StringArray mfCoefNames;
   // Tank settings
   RealArray   mixRatio;
   StringArray tankNames;
   
   std::string coordSysName;
   std::string thrustDir1;
   std::string thrustDir2;
   std::string thrustDir3;
   
   std::string thrustModel;

   bool isCoordSysChanged;
   bool isTankChanged;
   bool isTankEmpty;

   bool isThrustModelChanged;

   bool useMixRatio;

   // Chemical Thruster
   bool areCCoefsChanged;
   bool areKCoefsChanged;
   // Electric Thruster
   bool areTCoefsChanged;
   bool areMFCoefsChanged;
   
   wxStaticText *originLabel;
   wxStaticText *axisLabel;
   wxStaticText *tankLabel;
   wxStaticText *mixRatioLabel;
   wxStaticText *gravityAccelLabel;
   wxStaticText *gravityAccelUnit;
   wxStaticText *ispLabel;
   wxStaticText *ispUnit;

   // Electric Thruster Additions
   wxStaticText  *thrustModelTxt;
   wxStaticText  *minPowerTxt;
   wxStaticText  *maxPowerTxt;
   wxStaticText  *efficiencyTxt;
   wxStaticText  *ispTxt;
   wxStaticText  *constantThrustTxt;

   wxStaticText  *minPowerUnits;
   wxStaticText  *maxPowerUnits;
   wxStaticText  *efficiencyUnits;
   wxStaticText  *ispUnits;
   wxStaticText  *constantThrustUnits;

   wxComboBox    *thrustModelCB;

   wxTextCtrl    *minPowerTxtCtrl;
   wxTextCtrl    *maxPowerTxtCtrl;
   wxTextCtrl    *efficiencyTxtCtrl;
   wxTextCtrl    *ispTxtCtrl;
   wxTextCtrl    *constantThrustTxtCtrl;
   wxTextCtrl    *tankTxtCtrl;
   wxTextCtrl    *mixRatioTxtCtrl;

   wxString      *thrustModelArray;

//   wxButton *cCoefButton;
//   wxButton *kCoefButton;
   
   wxButton *configButton;
   wxButton *tankSelectorButton;

   wxComboBox *coordSysComboBox;
   wxComboBox *originComboBox;
   wxComboBox *axesComboBox;
   wxComboBox *tankComboBox;

   wxStaticText *XLabel;
   wxStaticText *YLabel;
   wxStaticText *ZLabel;
   wxStaticText *XUnitLabel;
   wxStaticText *YUnitLabel;
   wxStaticText *ZUnitLabel;
   GmatStaticBoxSizer *vectorBoxSizer;
   wxTextCtrl *elem1TextCtrl;
   wxTextCtrl *elem2TextCtrl;
   wxTextCtrl *elem3TextCtrl;
   wxTextCtrl *dutyCycleTextCtrl;
   wxTextCtrl *scaleFactorTextCtrl;
   wxTextCtrl *ispTextCtrl;
   wxTextCtrl *gravityAccelTextCtrl;
   
   wxCheckBox *decMassCheckBox;
   
   void LoadTankAndMixControl();
   void EnableMixRatio(bool activate);

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30250,
      ID_TEXTCTRL,
      ID_CHECKBOX,
      ID_BUTTON,
      ID_COMBOBOX,
   };
};
#endif

