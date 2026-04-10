//$Id$
//------------------------------------------------------------------------------
//                              OrbitViewPanel
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
// Author: Linda Jun
// Created: 2010/06/18
/**
 * Declares OrbitViewPanel class. This class allows user to setup OrbitView.
 */
//------------------------------------------------------------------------------
#ifndef OrbitViewPanel_hpp
#define OrbitViewPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "OrbitView.hpp"
#include "OrbitView.hpp"
#ifdef __USE_COLOR_FROM_SUBSCRIBER__
#include "RgbColor.hpp"
#include "ColorTypes.hpp"
#endif

class OrbitViewPanel: public GmatPanel
{
public:
   OrbitViewPanel(wxWindow *parent, const wxString &subscriberName);
   ~OrbitViewPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(UnsignedInt type,
                                  const wxString &oldName,
                                  const wxString &newName);
protected:
   OrbitView *mOrbitView;
   
   bool mHasIntegerDataChanged;
   bool mHasRealDataChanged;
   bool mHasDrawingOptionChanged;
   bool mHasSpChanged;

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   bool mHasOrbitColorChanged;
   bool mHasTargetColorChanged;
   #endif
   
   bool mHasShowObjectChanged;
   bool mHasCoordSysChanged;
   bool mHasViewInfoChanged;
   bool mHasViewUpInfoChanged;
   bool mHasStarOptionChanged;
   
   int  mScCount;
   int  mNonScCount;
   
   std::string mSelSpName;

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   wxStaticText *mTargetColorLabel;
   wxButton *mOrbitColorButton;
   wxButton *mTargetColorButton;
   #endif
   
   std::map<std::string, bool> mDrawObjectMap;
   
   wxArrayString mExcludedScList;
   wxArrayString mExcludedCelesPointList;
   
   wxStaticText *mFovLabel;
   wxStaticText *mViewPointRefStaticText;
   wxStaticText *mViewPointVecStaticText;
   wxStaticText *mViewDirStaticText;
   wxStaticText *mStarCountStaticText;
   wxStaticText *mFovStaticText;
   wxStaticText *mFovMinStaticText;
   wxStaticText *mFovMaxStaticText;
   
   wxCheckBox *mShowPlotCheckBox;
   wxCheckBox *mShowLabelsCheckBox;
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mEclipticPlaneCheckBox;
   wxCheckBox *mXYPlaneCheckBox;
   wxCheckBox *mUseInitialViewCheckBox;
   wxCheckBox *mPerspectiveModeCheckBox;
   wxCheckBox *mUseFixedFovCheckBox;
   wxCheckBox *mAxesCheckBox;
   wxCheckBox *mGridCheckBox;
   wxCheckBox *mOriginSunLineCheckBox;
   wxCheckBox *mDrawObjectCheckBox;
   wxCheckBox *mEnableStarsCheckBox;
   wxCheckBox *mEnableConstellationsCheckBox;
   
   wxTextCtrl *mDataCollectFreqTextCtrl;
   wxTextCtrl *mUpdatePlotFreqTextCtrl;
   wxTextCtrl *mMaxPlottedDataPointsTextCtrl;
   wxTextCtrl *mNumPointsToRedrawTextCtrl;
   wxTextCtrl *mFixedFovTextCtrl;
   wxTextCtrl *mViewScaleFactorTextCtrl;
   wxTextCtrl *mViewPointRef1TextCtrl;
   wxTextCtrl *mViewPointRef2TextCtrl;
   wxTextCtrl *mViewPointRef3TextCtrl;
   wxTextCtrl *mViewPointVec1TextCtrl;
   wxTextCtrl *mViewPointVec2TextCtrl;
   wxTextCtrl *mViewPointVec3TextCtrl;
   wxTextCtrl *mViewDir1TextCtrl;
   wxTextCtrl *mViewDir2TextCtrl;
   wxTextCtrl *mViewDir3TextCtrl;
   wxTextCtrl *mStarCountTextCtrl;
   wxTextCtrl *mFovTextCtrl;
   wxTextCtrl *mFovMinTextCtrl;
   wxTextCtrl *mFovMaxTextCtrl;
   
   wxListBox *mSpacecraftListBox;
   wxListBox *mCelesPointListBox;
   wxListBox *mSelectedScListBox;
   wxListBox *mSelectedObjListBox;
   
   wxButton *addScButton;
   wxButton *removeScButton;
   wxButton *clearScButton;
   
   wxComboBox *mSolverIterComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mViewPointRefComboBox;
   wxComboBox *mViewPointVectorComboBox;
   wxComboBox *mViewDirectionComboBox;
   wxComboBox *mViewUpCsComboBox;
   wxComboBox *mViewUpAxisComboBox;
   
   wxFlexGridSizer *mObjectSizer;
   wxFlexGridSizer *mViewDefSizer;
   wxBoxSizer *mScOptionSizer;
   wxBoxSizer *mViewPointRefSizer;
   wxBoxSizer *mViewPointVectorSizer;
   wxBoxSizer *mViewDirVectorSizer;
   
   // for initializing data
   void InitializeData();
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handler
   void OnAddSpacePoint(wxCommandEvent& event);
   void OnRemoveSpacePoint(wxCommandEvent& event);
   void OnClearSpacePoint(wxCommandEvent& event);
   void OnSelectAvailObject(wxCommandEvent& event);
   void OnSelectSpacecraft(wxCommandEvent& event);
   void OnSelectOtherObject(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   void OnOrbitColorClick(wxCommandEvent& event);
   void OnTargetColorClick(wxCommandEvent& event);
   #endif
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   
   // Support Function
   void ValidateFovValues();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 93000,
      ID_COMBOBOX,
      ID_LISTBOX,
      SC_SEL_LISTBOX,
      OBJ_SEL_LISTBOX,
      CHECKBOX,
      ADD_SP_BUTTON,
      REMOVE_SP_BUTTON,
      CLEAR_SP_BUTTON,
      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      ORBIT_COLOR_BUTTON,
      TARGET_COLOR_BUTTON
      #endif
   };
   
private:

   void ShowSpacePointOption(const wxString &name, bool show = true, bool isSc = true);
};
#endif


