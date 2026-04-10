//$Id$
//------------------------------------------------------------------------------
//                            OrbitDesignerDialog
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
// Author: Evelyn Hull
// Created: 2011/07/18
/**
 * Declares OrbitDesignerDialog class. This Dialog allows users to generate
 * Keplerian elements for specific orbit types.
 */
//------------------------------------------------------------------------------
#ifndef OrbitDesignerDialog_hpp
#define OrbitDesignerDialog_hpp

#include "GmatDialog.hpp"
#include "gmatdefs.hpp"
#include "gmatwxdefs.hpp"
#include <wx/string.h> 
#include "SunSync.hpp"
#include "Spacecraft.hpp"
#include "StringUtil.hpp"
#include "Rvector6.hpp"
#include "OrbitDesignerTime.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "RepeatSunSync.hpp"
#include "RepeatGroundTrack.hpp"
#include "Frozen.hpp"
#include "UserInputValidator.hpp"

class OrbitDesignerDialog: public GmatDialog
{
public:
   // constructors
   OrbitDesignerDialog(wxWindow *parent, Spacecraft *spacecraft); 
   
   ~OrbitDesignerDialog(); 
   
   wxArrayString GetElementsString();
   
   //accessor functions
   Rvector6 GetElementsDouble();
   std::string GetEpochFormat();
   std::string GetEpoch();
   
   bool isEpochChanged;
   bool updateOrbit;
   
private:

   bool mIsOrbitChanged;

   bool paramOneVal;
   bool paramTwoVal;
   bool paramThreeVal;
   bool paramFourVal;
   bool paramFiveVal;
   bool paramSixVal;
   bool paramSevenVal;
   bool timeParamOneVal;
   bool timeParamTwoVal;
   bool timeParamThreeVal;

   Real input1Val;
   Real input2Val;
   Real input3Val;
   Real input4Val;
   Real input5Val;
   Real input6Val;
   Real input7Val;
   wxString timeIn1Val;
   Real timeIn2Val;
   wxString timeIn3Val;

   struct sunElements 
   {
      Real SMA;
      Real ALT;
      Real ECC;
      Real INC;
      Real ROP;
      Real ROA;
      Real P;
      wxString epoch;
      wxString epochFormat;
      wxString startTime;
      Real RAAN;
      wxString errormsg;
   };

   wxString orbitType;
   wxString epochType;
   wxArrayString elements;
   wxString summaryString;

   //orbit type objects
   /// @todo   these should all be pointers and not set until the constructor
   /// and then deleted in the destructor;
   /// then the unneeded default constructors for all of these items can be
   /// removed
   Spacecraft *theSpacecraft;
   SunSync orbitSS;
   RepeatSunSync orbitRSS;
   RepeatGroundTrack orbitRGT;
   Frozen orbitFZN;
   OrbitDesignerTime orbitTime;

   wxComboBox *orbitTypeComboBox;
   wxComboBox *epochComboBox;
   wxButton *findOrbitButton;
   wxButton *showSummary;

   //input parameter check boxes
   wxCheckBox *paramOneCheckBox;
   wxCheckBox *paramTwoCheckBox;
   wxCheckBox *paramThreeCheckBox;
   wxCheckBox *paramFourCheckBox;
   wxCheckBox *paramFiveCheckBox;
   wxCheckBox *paramSixCheckBox;
   wxCheckBox *paramSevenCheckBox;

   //user input boxes
   wxTextCtrl *param1TextCtrl;
   wxTextCtrl *param2TextCtrl;
   wxTextCtrl *param3TextCtrl;
   wxTextCtrl *param4TextCtrl;
   wxTextCtrl *param5TextCtrl;
   wxTextCtrl *param6TextCtrl;
   wxTextCtrl *param7TextCtrl;

   //input units text
   wxStaticText *units1StaticText;
   wxStaticText *units2StaticText;
   wxStaticText *units3StaticText;
   wxStaticText *units4StaticText;
   wxStaticText *units5StaticText;
   wxStaticText *units6StaticText;
   wxStaticText *units7StaticText;

   //time input check boxes
   wxCheckBox *timeParamOneCheckBox;
   wxCheckBox *timeParamTwoCheckBox;
   wxCheckBox *timeParamThreeCheckBox;

   //time input boxes
   wxTextCtrl *time1TextCtrl;
   wxTextCtrl *time2TextCtrl;
   wxTextCtrl *time3TextCtrl;

   //time units text
   wxStaticText *timeUnits1StaticText;
   wxStaticText *timeUnits2StaticText;
   wxStaticText *timeUnits3StaticText;

   //output parameters text boxes
   wxStaticText *paramOut1StaticText;
   wxStaticText *paramOut2StaticText;
   wxStaticText *paramOut3StaticText;
   wxStaticText *paramOut4StaticText;
   wxStaticText *paramOut5StaticText;
   wxStaticText *paramOut6StaticText;
   wxStaticText *paramOut7StaticText;
   wxStaticText *paramOut8StaticText;
   wxStaticText *paramOut9StaticText;

   //output values boxes
   wxTextCtrl *output1TextCtrl;
   wxTextCtrl *output2TextCtrl;
   wxTextCtrl *output3TextCtrl;
   wxTextCtrl *output4TextCtrl;
   wxTextCtrl *output5TextCtrl;
   wxTextCtrl *output6TextCtrl;
   wxTextCtrl *output7TextCtrl;
   wxTextCtrl *output8TextCtrl;
   wxTextCtrl *output9TextCtrl;

   //output units text boxes
   wxStaticText *unitsOut1StaticText;
   wxStaticText *unitsOut2StaticText;
   wxStaticText *unitsOut3StaticText;
   wxStaticText *unitsOut4StaticText;
   wxStaticText *unitsOut5StaticText;
   wxStaticText *unitsOut6StaticText;
   wxStaticText *unitsOut7StaticText;
   wxStaticText *unitsOut8StaticText;
   wxStaticText *unitsOut9StaticText;

   //time output parameters text boxes
   wxStaticText *timeParamOut1StaticText;
   wxStaticText *timeParamOut2StaticText;
   wxStaticText *timeParamOut3StaticText;
   wxStaticText *timeParamOut4StaticText;

   //time output values boxes
   wxTextCtrl *timeOut1TextCtrl;
   wxTextCtrl *timeOut2TextCtrl;
   wxTextCtrl *timeOut3TextCtrl;
   wxTextCtrl *timeOut4TextCtrl;

   //time output units text boxes
   wxStaticText *timeUnitsOut1StaticText;
   wxStaticText *timeUnitsOut2StaticText;
   wxStaticText *timeUnitsOut3StaticText;
   wxStaticText *timeUnitsOut4StaticText;

   //sizers
   wxFlexGridSizer *orbitTypeFlexGridSizer;
   wxFlexGridSizer *orbitParamsFlexGridSizer;
   wxFlexGridSizer *timeParamsFlexGridSizer;
   wxFlexGridSizer *outputsFlexGridSizer;
   wxFlexGridSizer *timeOutputsFlexGridSizer;
   GmatStaticBoxSizer *timeSizer;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnSummary(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnP1CheckBoxChange(wxCommandEvent& event);
   void OnP2CheckBoxChange(wxCommandEvent& event);
   void OnP3CheckBoxChange(wxCommandEvent& event);
   void OnP4CheckBoxChange(wxCommandEvent& event);
   void OnP5CheckBoxChange(wxCommandEvent& event);
   void OnP6CheckBoxChange(wxCommandEvent& event);
   void OnP7CheckBoxChange(wxCommandEvent& event);
   void OnTP1CheckBoxChange(wxCommandEvent& event);
   void OnTP2CheckBoxChange(wxCommandEvent& event);
   void OnTP3CheckBoxChange(wxCommandEvent& event);
   void OnFindOrbit(wxCommandEvent& event);
   void OnParamChange(wxCommandEvent& event);

   //display functions
   void DisplaySunSync();
   void DisplayRepeatSunSync();
   void DisplayRepeatGroundTrack();
   void DisplayGeostationary();
   void DisplayMolniya();
   void DisplayFrozen();
   void DisplayTime();
        
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 81000,
      ID_BUTTON_SUMMARY,
      ID_COMBO,
      ID_TEXTCTRL_PARAMS,
      ID_BUTTON_FINDORBIT,
      ID_CB_P1,
      ID_CB_P2,
      ID_CB_P3,
      ID_CB_P4,
      ID_CB_P5,
      ID_CB_P6,
      ID_CB_P7,
      ID_CB_TP1,
      ID_CB_TP2,
      ID_CB_TP3
   };
};

#endif

