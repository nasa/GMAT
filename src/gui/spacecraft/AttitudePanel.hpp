//$Id$
//------------------------------------------------------------------------------
//                            AttitudePanel
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
// Author: Waka Waktola (and heavily modified by Wendy C. Shoan)
// Created: 2006/03/01 (2007.06.12)
/**
 * This class contains information needed to setup users' spacecraft attitude
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef AttitudePanel_hpp
#define AttitudePanel_hpp

#include "gmatdefs.hpp"
#include "Attitude.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "CoordinateConverter.hpp"
#include "CoordinateSystem.hpp"
#include "CelestialBody.hpp"

#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class AttitudePanel: public wxPanel
{
public:
   AttitudePanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~AttitudePanel();
   void SaveData();
   void LoadData();

   bool IsDataChanged() { return dataChanged; };
   bool CanClosePanel() { return canClose;    };
   
private:    

   GmatStaticBoxSizer *rightSizer;

   wxFlexGridSizer    *flexGridSizer2;

   GmatStaticBoxSizer *attitudeSizer;
   GmatStaticBoxSizer *attRateSizer;

   wxBoxSizer   *precessingSpinnerSizer; //LOJ: Added
   wxBoxSizer   *precessingSpinnerSizer2; //LOJ: Added
   wxBoxSizer   *nadirPointingSizer;
   wxBoxSizer   *nadirPointingSizer2;
   wxBoxSizer   *ccsdsAttitudeSizer;
   
   wxStaticText *config1StaticText;
   wxStaticText *config2StaticText;
   wxStaticText *config3StaticText;
   wxStaticText *config4StaticText;
   
   wxStaticText *stateTypeStaticText;
   wxStaticText *stateRateTypeStaticText;
   
   wxStaticText *st1StaticText;
   wxStaticText *st2StaticText;
   wxStaticText *st3StaticText;
   wxStaticText *st4StaticText;
   
   wxStaticText *str1StaticText;
   wxStaticText *str2StaticText;
   wxStaticText *str3StaticText;
   
   wxStaticText *spiceMessage;

   // precessing spinner
   wxStaticText *spinAxisLabel;
   wxStaticText *nutRefVecLabel;
   wxStaticText *initPrecAngleLabel;
   wxStaticText *precessionRateLabel;
   wxStaticText *nutationAngleLabel;
   wxStaticText *initSpinAngleLabel;
   wxStaticText *spinRateLabel;
   // nadir-pointing
   wxStaticText *attRefBodyLabel;
   wxStaticText *constraintTypeLabel;
   wxStaticText *bodyAlignVectorLabel;
   wxStaticText *bodyConstraintVectorLabel;
   // CCSDS attitude
   wxStaticText *aemFileLabel;

   wxTextCtrl   *st1TextCtrl;
   wxTextCtrl   *st2TextCtrl;
   wxTextCtrl   *st3TextCtrl;
   wxTextCtrl   *st4TextCtrl;
   wxTextCtrl   *st5TextCtrl;
   wxTextCtrl   *st6TextCtrl;
   wxTextCtrl   *st7TextCtrl;
   wxTextCtrl   *st8TextCtrl;
   wxTextCtrl   *st9TextCtrl;
   wxTextCtrl   *st10TextCtrl;

   wxTextCtrl   *str1TextCtrl;
   wxTextCtrl   *str2TextCtrl;
   wxTextCtrl   *str3TextCtrl;

   wxStaticText *attUnits1;
   wxStaticText *attUnits2;
   wxStaticText *attUnits3;

   wxStaticText *rateUnits1;
   wxStaticText *rateUnits2;
   wxStaticText *rateUnits3;

   wxStaticText *initPrecAngleUnits;
   wxStaticText *precessionRateUnits;
   wxStaticText *nutationAngleUnits;
   wxStaticText *initSpinAngleUnits;
   wxStaticText *spinRateUnits;

   wxComboBox   *config1ComboBox;
   wxComboBox   *coordSysComboBox;
   wxComboBox   *config3ComboBox;
   wxComboBox   *config4ComboBox;
   wxComboBox   *stateTypeComboBox;
   wxComboBox   *stateRateTypeComboBox;
   wxComboBox   *referenceBodyComboBox;
   wxComboBox   *constraintTypeComboBox;

   wxBitmapButton *aemBrowseButton;


   // for precessing spinner data
   wxTextCtrl *spinAxis1TextCtrl;
   wxTextCtrl *spinAxis2TextCtrl;
   wxTextCtrl *spinAxis3TextCtrl;
   wxTextCtrl *nutRefVec1TextCtrl;
   wxTextCtrl *nutRefVec2TextCtrl;
   wxTextCtrl *nutRefVec3TextCtrl;
   wxTextCtrl *initPrecAngleTextCtrl;
   wxTextCtrl *precessionRateTextCtrl;
   wxTextCtrl *nutationAngleTextCtrl;
   wxTextCtrl *initSpinAngleTextCtrl;
   wxTextCtrl *spinRateTextCtrl;
   
   //for nadir-pointing data
   wxTextCtrl *bodyAlignVectorXTextCtrl;
   wxTextCtrl *bodyAlignVectorYTextCtrl;
   wxTextCtrl *bodyAlignVectorZTextCtrl;
   wxTextCtrl *bodyConstraintVectorXTextCtrl;
   wxTextCtrl *bodyConstraintVectorYTextCtrl;
   wxTextCtrl *bodyConstraintVectorZTextCtrl;

   // CCSDS attitude
   wxTextCtrl *aemFileTextCtrl;

   /// objects needed
   GmatPanel      *theScPanel;
   Spacecraft     *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   Attitude       *theAttitude;
   
   /// coordinate system, reference body, and converter data
   CoordinateConverter cc;
   CoordinateSystem    *attCS;
   CoordinateSystem    *toCS;
   CoordinateSystem    *fromCS;
   CelestialBody       *attRefBody;
   
   /// arrays of strings needed as input to the combo boxes
   StringArray         modelArray;
   StringArray         eulerSeqArray;
   StringArray         stateTypeArray;
   StringArray         stateRateTypeArray;
   StringArray         constraintTypeArray;

   /// those strings as wxString arrays for the combo boxes
   wxString            *attitudeModelArray;
   wxString            *eulerSequenceArray;
   wxString            *stateArray;
   wxString            *stateRateArray;
   wxString            *constraintArray;
   
   /// string versions of values in text boxes
   wxString            *cosineMatrix[9];
   wxString            *quaternion[4];
   wxString            *eulerAngles[3];
   wxString            *MRPs[3]; // Dunn Added
   wxString            *angVel[3];
   wxString            *eulerAngleRates[3];
   
   /// string versions of current data on combo boxes
   std::string         attitudeModel;
   std::string         attCoordSystem;
   std::string         eulerSequence;
   std::string         attStateType;
   std::string         attRateStateType;

   std::string         attRefBodyName;
   std::string         constraintType;

   std::string         aemFile;
   std::string         previousAemFile;

   /// flags for data modification
   bool                dataChanged;
   bool                canClose;

   // local values of attitude data
   // NOTE - only currently-displayed representations will be up-to-date;
   // conversions occur on state (or state rate) type changes
   UnsignedIntArray seq;
   Rmatrix33        dcmat; // Dunn renamed to dcmat
   Rvector          q;
   Rvector3         ea;
   Rvector3         mrp; // Dunn Added
   Rvector3         av;
   Rvector3         ear;
   Real             epoch;
   
   /// modification flags
   bool             stateTypeModified;
   bool             rateStateTypeModified;
   bool             stateModified;
   bool             stateRateModified;
   bool             csModified;
   bool             seqModified;
   bool             modelModified;

   bool             dcmatModified[9]; // Dunn renamed to dcmatModified
   bool             qModified[4];
   bool             eaModified[3];
   bool             mrpModified[3];   // Dunn Added
   bool             avModified[3];
   bool             earModified[3];
   
   bool             precessingSpinnerDataLoaded; // LOJ: Added, this flag tells whether data is loaded or not
   bool             spinAxisModified;  // LOJ: Added, this flag is for all 3 components
   bool             nutRefVecModified; // LOJ: Added, this flag is for all 3 components
   bool             initPrecAngleModified;
   bool             precessionRateModified;
   bool             nutationAngleModified;
   bool             initSpinAngleModified;
   bool             spinRateModified;
   
   bool             nadirPointingDataLoaded;
   bool             attRefBodyModified;
   bool             constraintTypeModified;
   bool             bodyAlignVectorModified;
   bool             bodyConstraintVectorModified;

   bool             ccsdsDataLoaded;
   bool             aemFileModified;

   /// has the attitude and attitude rate data been loaded?
   bool             attDataLoaded;

   void Create();
   
   void LoadPrecessingSpinnerData(); //LOJ: Added
   void SavePrecessingSpinnerData(Attitude *useAttitude); //LOJ: Added
   
   void LoadNadirPointingData(); //WCS: Added
   void SaveNadirPointingData(Attitude *useAttitude); //WCS: Added

   void LoadCCSDSAttitudeData();
   void SaveCCSDSAttitudeData(Attitude *useAttitude);

   void LoadAttitudeAndRateData(Attitude* forAtt);

   bool DisplayEulerAngles();
   bool DisplayQuaternion();
   bool DisplayDCM();
   bool DisplayMRPs();  // Dunn Added
   bool DisplayEulerAngleRates();
   bool DisplayAngularVelocity();
   
   bool UpdateEulerAngles();
   bool UpdateQuaternion();
   bool UpdateCosineMatrix();
   bool UpdateMRPs();   // Dunn Added
   bool UpdateEulerAngleRates();
   bool UpdateAngularVelocity();
      
   bool IsStateModified(const std::string which = "Both");
   void ResetStateFlags(const std::string which = "Both",
                        bool discardEdits = false);
   bool ValidateState(const std::string which = "Both", bool checkForSingularity = false);
   
   void HideInitialAttitudeAndRate();
   void ShowInitialAttitudeAndRate();
   void ShowPrecessingSpinnerData(); //LOJ: Added
   void ShowNadirPointingData();
   void ShowCCSDSAttitudeData();
   void DisableAll();
   void EnableAll();
   void DisplayDataForModel(const std::string &modelType);

   void ResizeTextCtrl1234(bool forQuaternion = false);

   wxString ToString(Real rval); // ??
   
   /// Event Handling
   DECLARE_EVENT_TABLE();
   /// when user types in a new state value
   void OnStateTextUpdate(wxCommandEvent &event);
   /// when user types in a new state rate value
   void OnStateRateTextUpdate(wxCommandEvent &event);
   /// when user types in a new spin axis value
   void OnSpinAxisTextUpdate(wxCommandEvent &event);               //LOJ: Added to trigger new value
   /// when user types in a new nutation ref vector value
   void OnNutationRefVectorTextUpdate(wxCommandEvent &event);      //LOJ: Added to trigger new value
   /// when user types in a new initial precession angle
   void OnInitialPrecessionAngleTextUpdate(wxCommandEvent &event); //WCS: Added to trigger new value
   /// when user types in a new precession rate
   void OnPrecessionRateTextUpdate(wxCommandEvent &event);         //WCS: Added to trigger new value
   /// when user types in a new nutation rate
   void OnNutationAngleTextUpdate(wxCommandEvent &event);          //WCS: Added to trigger new value
   /// when user types in a new initial spin angle
   void OnInitialSpinAngleTextUpdate(wxCommandEvent &event);       //WCS: Added to trigger new value
   /// when user types in a new spin rate
   void OnSpinRateTextUpdate(wxCommandEvent &event);               //WCS: Added to trigger new value
   /// when user types in a new body alignment vector value
   void OnBodyAlignmentVectorTextUpdate(wxCommandEvent &event);    //WCS: Added to trigger new value
   /// when user types in a new body constraint vector value
   void OnBodyConstraintVectorTextUpdate(wxCommandEvent &event);   //WCS: Added to trigger new value
   /// when user types in a new AEM file name
   void OnAEMFileTextUpdate(wxCommandEvent &event);
   // when the user selects the Browse button for the AEM file
   void OnBrowseButton(wxCommandEvent& event);
   /// when user selects state type from combo box
   void OnStateTypeSelection(wxCommandEvent &event);
   /// when user selects state rate type from combo box
   void OnStateTypeRateSelection(wxCommandEvent &event);
   /// when user selects an euler sequence from comb box
   void OnEulerSequenceSelection(wxCommandEvent &event);
   /// when user selects a reference coordinate system from combo box
   void OnCoordinateSystemSelection(wxCommandEvent &event);
   /// when user selects a attitude model from combo box
   void OnAttitudeModelSelection(wxCommandEvent &event);
   /// when user selects a attitude model from combo box
   void OnReferenceBodySelection(wxCommandEvent &event);           //WCS: Added to trigger new value
   /// when user selects a attitude model from combo box
   void OnConstraintTypeSelection(wxCommandEvent &event);        //WCS: Added to trigger new value
   
   
   /// IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 50000,
      ID_TEXTCTRL_STATE,
      ID_TEXTCTRL_STATE_RATE,
      // Added for PrecessingSpinner
      ID_TEXTCTRL_SPIN_AXIS,       //LOJ: Added
      ID_TEXTCTRL_NUT_REF_VEC,     //LOJ: Added
      ID_TEXTCTRL_INIT_PREC_ANGLE, //WCS: Added
      ID_TEXTCTRL_PRECESSION_RATE, //WCS: Added
      ID_TEXTCTRL_NUTATION_ANGLE,  //WCS: Added
      ID_TEXTCTRL_INIT_SPIN_ANGLE, //WCS: Added
      ID_TEXTCTRL_SPIN_RATE,       //WCS: Added
      // Added for NadirPointing
      ID_TEXTCTRL_BODY_ALIGNMENT_VECTOR,  //WCS: Added
      ID_TEXTCTRL_BODY_CONSTRAINT_VECTOR, //WCS: Added
      ID_CB_STATE,
      ID_CB_STATE_RATE,
      ID_CB_SEQ,
      ID_CB_COORDSYS,
      ID_CB_MODEL,
      // Added for NadirPointing
      ID_CB_REFERENCE_BODY,
      ID_CB_CONSTRAINT_TYPE,
      // Added for CCSDS attitude
      ID_TEXTCTRL_AEM_FILE,
      ID_BUTTON_BROWSE,
   };
   
   // IDs for state type
   // Dunn added a new enumeration, "MRPs", and changed the name of the count
   // variable at the bottom from StateTypeCount, which shows up for OrbitState
   // as well, to attStateTypeCount.
   enum StateType
   {
      EULER_ANGLES = 0,
      QUATERNION,
      DCM,
      MRPS,
      AttStateTypeCount,
   };
   
   // IDs for state rate type
   enum RateStateType
   {
      EULER_ANGLE_RATES = 0,
      ANGULAR_VELOCITY,
      AttStateRateTypeCount,  // Dunn renamed from StateRateTypeCount
   };
   
   static const std::string STATE_TEXT[AttStateTypeCount];
   static const std::string STATE_RATE_TEXT[AttStateRateTypeCount];
   
   static const Integer STARTUP_STATE_TYPE_SELECTION;
   static const Integer STARTUP_RATE_STATE_TYPE_SELECTION;
   
   static const Integer ATTITUDE_TEXT_CTRL_WIDTH;
   static const Integer QUATERNION_TEXT_CTRL_WIDTH;

   /// @todo these should come from the AttitudeConversionUtility or
   /// GmatConstants
   static const Real    EULER_ANGLE_TOLERANCE;
   static const Real    DCM_ORTHONORMALITY_TOLERANCE;

};
#endif

