//$Id$
//------------------------------------------------------------------------------
//                           PropagationConfigPanel
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
// Author: Waka Waktola
// Created: 2003/08/29
//
/**
 * This class contains the Propagation configuration window.
 */
//------------------------------------------------------------------------------
#ifndef PropagationConfigPanel_hpp
#define PropagationConfigPanel_hpp

// gui includes
#include "GmatStaticBoxSizer.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Propagator.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "DragForce.hpp"
#include "GravityField.hpp"
#include "HarmonicField.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "RelativisticCorrection.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "AtmosphereModel.hpp"
#include "MessageInterface.hpp"
#include "GmatPanel.hpp"
#include "gmatwxdefs.hpp"

class PropagationConfigPanel : public GmatPanel
{
public:

   PropagationConfigPanel(wxWindow *parent, const wxString &propName);
   virtual ~PropagationConfigPanel();

private:
   
   Integer IntegratorCount;

//   // Earth drag model
//   enum EarthDragModelType
//   {
//      NONE_DM = 0,
////      EXPONENTIAL,
//      MSISE90,
//      JR,
//      EarthDragModelCount,
//   };
   
   // Magnetic force drag model
   enum MagfModelType
   {
      NONE_MM = 0,
      MagfModelCount,
   };

   // Error Control
   enum ErrorControlType
   {
      NONE_EC = 0,
      RSSSTEP,
      RSSSTATE,
      LARGESTSTEP,
      LARGESTSTATE,
      ErrorControlCount,
   };
   
   struct ForceType
   {
      wxString bodyName;
      wxString gravType;
      wxString tideData;
      wxString tideModel;
      wxString dragType;
      wxString magfType;
      wxString gravDegree;
      wxString gravOrrder;
      wxString gravStmLimit;
      wxString magfDegree;
      wxString magfOrder;
      wxString potFile;
      wxString potFileFullPath;
      wxString tideFile;
      wxString tideFileFullPath;
      PointMassForce *pmf;
      GravityField *gravf;
      DragForce *dragf;
      SolarRadiationPressure *srpf;
      bool useSrp; // for future use (SRP on individual body is future implementation)
      bool relativisticCorrection;
      
      ForceType(const wxString &body, const wxString &grav = "None",
                const wxString &tData = "None", const wxString &tide = "None",
                const wxString &drag = "None", const wxString &mag = "None",
                PointMassForce *pf = NULL, GravityField *gf = NULL,
                DragForce *df = NULL)
         {
            bodyName = body; 
            gravType = grav; 
            tideData = tData; 
            tideModel = tide; 
            dragType = drag; 
            magfType = mag;
            gravDegree = "4"; 
            gravOrrder = "4"; 
            gravStmLimit = "100"; 
            magfDegree = "0"; 
            magfOrder = "0"; 
            potFile = ""; 
            potFileFullPath = ""; 
            tideFile = "";
            tideFileFullPath = "";
            pmf = pf; 
            gravf = gf; 
            dragf = df; 
            srpf = NULL; 
            useSrp = false; 
            relativisticCorrection = false;
         }
      
         ForceType& operator= (const ForceType& right)
         {
            if (this == &right)
               return *this;
            
            bodyName = right.bodyName; 
            gravType = right.gravType;
            tideData = right.tideData;
            tideModel = right.tideModel;
            dragType = right.dragType; 
            magfType = right.magfType; 
            gravDegree = right.gravDegree; 
            gravOrrder = right.gravOrrder;
            gravStmLimit = right.gravStmLimit;
            magfDegree = right.magfDegree; 
            magfOrder = right.magfOrder;
            potFile = right.potFile; 
            potFileFullPath = right.potFileFullPath;
            tideFile = right.tideFile;
            tideFileFullPath = right.tideFileFullPath;
            pmf = right.pmf; 
            gravf = right.gravf; 
            dragf = right.dragf; 
            srpf = right.srpf;
            useSrp = right.useSrp; 
            relativisticCorrection = right.relativisticCorrection;
            return *this;
         }
   };
   
   // SPK parameter buffers
   wxString spkStep;
   wxString spkBody;
   wxString spkEpFormat;
   wxString spkEpoch;

   bool isSpkStepChanged;
   bool isSpkBodyChanged;
   bool isSpkEpFormatChanged;
   bool isSpkEpochChanged;
   bool isSRPModelChanged;
   bool isDragModelChanged;

   wxFlexGridSizer *intFlexGridSizer;
   wxFlexGridSizer *srpRCFlexSizer;
   GmatStaticBoxSizer *intStaticSizer;
   GmatStaticBoxSizer *fmStaticSizer;
   GmatStaticBoxSizer *magfStaticSizer;      // So it can be hidden

   wxStaticText *minIntErrorStaticText;
   wxStaticText *nomIntErrorStaticText;
   wxStaticText *potFileStaticText;
   wxStaticText *tideDataStaticText;
   wxStaticText *tideFileStaticText;
   wxStaticText *tideModelStaticText;
   
   wxStaticText *initialStepSizeStaticText;
   wxStaticText *unitsInitStepSizeStaticText;
   wxStaticText *accuracyStaticText;
   wxStaticText *minStepStaticText;
   wxStaticText *unitsMinStepStaticText;
   wxStaticText *maxStepStaticText;
   wxStaticText *unitsMaxStepStaticText;
   wxStaticText *maxStepAttemptStaticText;
   wxStaticText *srpModelStaticText;
   wxStaticText *dragModelStaticText;


   wxTextCtrl *initialStepSizeTextCtrl;
   wxTextCtrl *accuracyTextCtrl;
   wxTextCtrl *minStepTextCtrl;
   wxTextCtrl *maxStepTextCtrl;
   wxTextCtrl *maxStepAttemptTextCtrl;
   wxTextCtrl *minIntErrorTextCtrl;
   wxTextCtrl *nomIntErrorTextCtrl;
//   wxTextCtrl *bodyTextCtrl;
   wxTextCtrl *gravityDegreeTextCtrl;
   wxTextCtrl *gravityMaxDegreeTextCtrl;
   wxTextCtrl *gravityOrderTextCtrl;
   wxTextCtrl *gravityMaxOrderTextCtrl;
   wxTextCtrl *gravityStmLimitTextCtrl;
   wxTextCtrl *potFileTextCtrl;
   wxTextCtrl *tideFileTextCtrl;
   wxTextCtrl *tideTextCtrl;
   wxTextCtrl *magneticDegreeTextCtrl;
   wxTextCtrl *magneticOrderTextCtrl;
   wxTextCtrl *pmEditTextCtrl;
   
   wxComboBox *theIntegratorComboBox;
   wxComboBox *theOriginComboBox;
   wxComboBox *thePrimaryBodyComboBox;
   wxComboBox *theGravModelComboBox;
   wxComboBox *theTideDataComboBox;
   wxComboBox *theTideModelComboBox;
   wxComboBox *theAtmosModelComboBox;
   wxComboBox *theMagfModelComboBox;
   wxComboBox *theErrorComboBox;
   wxComboBox *theSRPModelComboBox;
   wxComboBox *theDragModelComboBox;
   
   wxBitmapButton *theGravModelSearchButton;
   wxBitmapButton *theGravModelSaveButton;
   wxBitmapButton *theTideModelSearchButton;
   wxButton *theDragSetupButton;
   wxButton *theMagModelSearchButton;
   
   wxCheckBox *theSrpCheckBox;
   wxCheckBox *theRelativisticCorrectionCheckBox;
   wxCheckBox *theStopCheckBox;
   
   // Controls used by Propagators that aren't Integrators (SPK for now)
   wxStaticText *propagatorStepSizeStaticText;
   wxTextCtrl *propagatorStepSizeTextCtrl;
   wxStaticText *unitsPropagatorStepSizeStaticText;
   wxStaticText *propCentralBodyStaticText;
   wxComboBox *propCentralBodyComboBox;
   wxStaticText *propagatorEpochFormatStaticText;
   wxComboBox *propagatorEpochFormatComboBox;
   wxStaticText *startEpochStaticText;

   // Kludge to check if the control indexing broke the test complete tests
   wxTextCtrl *startEpochTextCtrl;

   wxComboBox *startEpochCombobox;
   wxArrayString startEpochChoices;

   wxString integratorString;
   wxString primaryBodyString;
   wxString gravityFieldString;
   wxString atmosModelString;
   
   wxBoxSizer *leftBoxSizer;
   
   std::string propSetupName;
   std::string thePropagatorName;
   std::string theForceModelName;
   std::string mFmPrefaceComment;
   
   wxString currentBodyName;
   wxString gravTypeName;
   wxString tideDataName;
   wxString tideModelName;
   wxString dragTypeName;
   wxString propOriginName;
   wxString errorControlTypeName;
   wxString srpModelName;
   wxString dragModelName;
   
   wxArrayString integratorTypeArray;
   wxArrayString dragModelArray;
   wxArrayString magfModelArray;
   wxArrayString errorControlArray;
   wxArrayString srpModelArray;
   wxArrayString dragModelListArray;
   
   std::map<wxString, wxString> theFileMap;
   
   // Restrict to one primary body:
   wxString primaryBody;
//   wxArrayString primaryBodiesArray;
   wxArrayString secondaryBodiesArray;
   wxArrayString integratorArray;
   
   Integer numOfForces;
   Integer currentBodyId;
   
   /// Flag indicating that the drag buffer has been filled
   bool               dragBufferReady;
   /// Buffer for the drag data (F10.7, F10.7a, geomagnetic index)
   Real               dragParameterBuffer[3];
   std::vector<std::string> dragStringBuffer;

   bool useDragForce;
   bool usePropOriginForSrp;
   bool addRelativisticCorrection;
   bool stopOnAccViolation;
   bool isForceModelChanged;
   bool isAtmosChanged;
   bool isDegOrderChanged;
   bool isPotFileChanged;
   bool isTideFileChanged;
   bool isTideModelChanged;
   bool isMagfTextChanged;
   bool isIntegratorChanged;
   bool isIntegratorDataChanged;
   bool isOriginChanged;
   bool isErrControlChanged;
   
   Propagator                     *thePropagator;
   PropSetup                      *thePropSetup;
   ODEModel                       *theForceModel;
   PointMassForce                 *thePMF;
   DragForce                      *theDragForce;
   GravityField                   *theGravForce;
   SolarRadiationPressure         *theSRP;
   RelativisticCorrection         *theRC;
   SolarSystem                    *theSolarSystem;
   CelestialBody                  *theCelestialBody;
   AtmosphereModel                *theAtmosphereModel;
//   std::vector<PointMassForce *>  thePMForces;
//   std::vector<ForceType*>        primaryBodyList;
   std::vector<ForceType*>        pointMassBodyList;
   
   // Restricted to one primary, so using a single ForceType rather than array
   ForceType                      *primaryBodyData;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   Integer FindPrimaryBody(const wxString& bodyName, bool create = true,
                           const wxString& gravType = "None",
                           const wxString& dragType = "None",
                           const wxString& magfType = "None");
   
   Integer FindPointMassBody(const wxString& bodyName);
   
   void Initialize();
   void DisplayIntegratorData(bool integratorChanged);
   void DisplayPrimaryBodyData();
   void DisplayForceData();
   void DisplayGravityFieldData (const wxString& bodyName, bool potValueChanged = false,
       bool tideValueChanged = false);
   void DisplayAtmosphereModelData();
   void DisplayPointMassData();
   void DisplayMagneticFieldData();
   void DisplaySRPData();
   void DisplayErrorControlData();
   void EnablePrimaryBodyItems(bool enable = true, bool clear = false);
   void UpdatePrimaryBodyItems();
   void UpdatePrimaryBodyComboBoxList();
   bool ValidatePropEpochComboBoxes();
   
   // Saving data
   bool SaveIntegratorData();
   bool SavePropagatorData();
   bool SaveDegOrder();
   bool SavePotFile();
   bool SaveTideFile();
   bool SaveTideModel();
   bool SaveAtmosModel();
   
   // Converting Data
   wxString ToString(Real rval);
   
   // Text control event method
   void OnIntegratorTextUpdate(wxCommandEvent &event);
   void OnGravityTextUpdate(wxCommandEvent& event);
   void OnTideTextUpdate(wxCommandEvent& event);
   void OnMagneticTextUpdate(wxCommandEvent& event);

   // Checkbox event method
   void OnSRPCheckBoxChange(wxCommandEvent &event);
   void OnStopCheckBoxChange(wxCommandEvent &event);
   void OnRelativisticCorrectionCheckBoxChange(wxCommandEvent &event);
   
   // Combobox event method
   void OnIntegratorComboBox(wxCommandEvent &event);
   void OnPrimaryBodyComboBox(wxCommandEvent &event);
   void OnOriginComboBox(wxCommandEvent &event);
   void OnGravityModelComboBox(wxCommandEvent &event);
   void OnTideDataComboBox(wxCommandEvent &event);
   void OnTideModelComboBox(wxCommandEvent &event);
   void OnAtmosphereModelComboBox(wxCommandEvent &event);
   void OnErrorControlComboBox(wxCommandEvent &event);
   void OnPropOriginComboBox(wxCommandEvent &);
   void OnPropEpochComboBox(wxCommandEvent &);
   void OnStartEpochComboBox(wxCommandEvent &);
   void OnSRPModelComboBox(wxCommandEvent &event);
   void OnDragModelComboBox(wxCommandEvent &event);

   void OnStartEpochTextChange(wxCommandEvent &);

   
   // Button event methods
   void OnAddBodyButton(wxCommandEvent &event);
   void OnGravSearchButton(wxCommandEvent &event);
   void OnGravSaveButton(wxCommandEvent &event);
   void OnTideSearchButton(wxCommandEvent &event);
   void OnSetupButton(wxCommandEvent &event);
   void OnMagSearchButton(wxCommandEvent &event);
   void OnPMEditButton(wxCommandEvent &event);
   void OnSRPEditButton(wxCommandEvent &event);
   
   // for Debug
   void ShowPropData(const std::string &header);
   void ShowForceList(const std::string &header);
   void ShowForceModel(const std::string &header);
   
   void ShowIntegratorLayout(bool isIntegrator = true, bool isEphem = true);

   void PopulateForces();

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 42000,
      ID_TEXTCTRL,
      ID_TEXTCTRL_PROP,
      ID_TEXTCTRL_GRAV,
      ID_TEXTCTRL_TIDE,
      ID_TEXTCTRL_MAGF,
      ID_SRP_CHECKBOX,
      ID_REL_CORRECTION_CHECKBOX,
      ID_STOP_CHECKBOX,
      ID_CB_INTGR,
      ID_CB_BODY,
      ID_CB_ORIGIN,
      ID_CB_GRAV,
      ID_CB_TIDE,
      ID_CB_TIDE_DATA,
      ID_CB_ATMOS,
      ID_CB_MAG,
      ID_CB_ERROR,
      ID_CB_SRP_MODEL,
      ID_CB_DRAG_MODEL,
      ID_BUTTON_ADD_BODY,
      ID_BUTTON_GRAV_SEARCH,
      ID_BUTTON_GRAV_SAVE,
      ID_BUTTON_TIDE_SEARCH,
      ID_BUTTON_SETUP,
      ID_BUTTON_MAG_SEARCH,
      ID_BUTTON_PM_EDIT,
      ID_BUTTON_SRP_EDIT,
      ID_CB_PROP_ORIGIN,
      ID_CB_PROP_EPOCHFORMAT,
      ID_CB_PROP_EPOCHSTART
   };
};
//------------------------------------------------------------------------------
#endif // PropagationConfigPanel_hpp

