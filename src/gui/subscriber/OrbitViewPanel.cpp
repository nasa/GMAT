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
 * Implements OrbitViewPanel class.
 */
//------------------------------------------------------------------------------

#include "OrbitViewPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"

#include "ColorTypes.hpp"           // for namespace GmatColor::
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

// To enable perspective mode
//#define __ENABLE_GL_PERSPECTIVE__

//#define DEBUG_OPENGL_PANEL 1
//#define DEBUG_OPENGL_PANEL_CREATE 1
//#define DEBUG_OPENGL_PANEL_LOAD 1
//#define DEBUG_OPENGL_PANEL_SAVE 1
//#define DEBUG_OPENGL_PANEL_CHECKBOX 1
//#define DEBUG_OPENGL_PANEL_SHOW 1
//#define DEBUG_ADD_SP

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OrbitViewPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ADD_SP_BUTTON, OrbitViewPanel::OnAddSpacePoint)
   EVT_BUTTON(REMOVE_SP_BUTTON, OrbitViewPanel::OnRemoveSpacePoint)
   EVT_BUTTON(CLEAR_SP_BUTTON, OrbitViewPanel::OnClearSpacePoint)
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   EVT_BUTTON(ORBIT_COLOR_BUTTON, OrbitViewPanel::OnOrbitColorClick)
   EVT_BUTTON(TARGET_COLOR_BUTTON, OrbitViewPanel::OnTargetColorClick)
   #endif
   EVT_LISTBOX(ID_LISTBOX, OrbitViewPanel::OnSelectAvailObject)
   EVT_LISTBOX(SC_SEL_LISTBOX, OrbitViewPanel::OnSelectSpacecraft)
   EVT_LISTBOX(OBJ_SEL_LISTBOX, OrbitViewPanel::OnSelectOtherObject)
   EVT_CHECKBOX(CHECKBOX, OrbitViewPanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, OrbitViewPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, OrbitViewPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// OrbitViewPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs OrbitViewPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the OrbitViewPanel GUI
 */
//------------------------------------------------------------------------------
OrbitViewPanel::OrbitViewPanel(wxWindow *parent,
                               const wxString &subscriberName)
   : GmatPanel(parent),
    mHasStarOptionChanged( false ),  // Fixed unitialized value error
    mHasViewUpInfoChanged( false )
{
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage("OrbitViewPanel() entering...\n");
   MessageInterface::ShowMessage("OrbitViewPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
   #endif
   
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(subscriberName.c_str());
   
   mOrbitView = (OrbitView*)subscriber;
   
   // Set the pointer for the "Show Script" button
   mObject = mOrbitView;
   
   InitializeData();   
   Create();
   Show();
   
   // Listen for Spacecraft name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~OrbitViewPanel()
//------------------------------------------------------------------------------
OrbitViewPanel::~OrbitViewPanel()
{
   #if DEBUG_OPENGL_PANEL
   MessageInterface::ShowMessage
      ("OrbitViewPanel::~OrbitViewPanel() unregistering mSpacecraftListBox:%d\n",
       mSpacecraftListBox);
   #endif
   
   theGuiManager->UnregisterListBox("CelestialPoint", mCelesPointListBox, &mExcludedCelesPointList);
   theGuiManager->UnregisterListBox("Spacecraft", mSpacecraftListBox, &mExcludedScList);
   
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", mViewUpCsComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewPointRefComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewPointVectorComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", mViewDirectionComboBox);
   
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool OrbitViewPanel::PrepareObjectNameChange()
{
   // Save GUI data
   wxCommandEvent event;
   OnApply(event);
   
   return GmatPanel::PrepareObjectNameChange();
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(UnsignedInt type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
/*
 * Reflects resource name change to this panel.
 * By the time this method is called, the base code already changed reference
 * object name, so all we need to do is re-load the data.
 */
//------------------------------------------------------------------------------
void OrbitViewPanel::ObjectNameChanged(UnsignedInt type,
                                       const wxString &oldName,
                                       const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("OrbitViewPanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::SPACECRAFT)
      return;
   
   // Initialize GUI data and re-load from base
   InitializeData();
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
void OrbitViewPanel::InitializeData()
{
   mHasIntegerDataChanged = false;
   mHasRealDataChanged = false;
   mHasDrawingOptionChanged = false;
   mHasSpChanged = false;
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   mHasOrbitColorChanged = false;
   mHasTargetColorChanged = false;
   #endif
   mHasShowObjectChanged = false;
   mHasCoordSysChanged = false;
   mHasViewInfoChanged = false;
   mScCount = 0;
   mNonScCount = 0;

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   #endif
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void OrbitViewPanel::Create()
{
   #if DEBUG_OPENGL_PANEL_CREATE
      MessageInterface::ShowMessage("OrbitViewPanel::Create() entered\n");
   #endif
   
   Integer bsize = 2; // border size
   
   // create axis array
   wxArrayString emptyList;
   wxString axisArray[] = {"X", "-X", "Y", "-Y", "Z", "-Z"};   
   wxArrayString empty;
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );   
   // We can't add emptyStaticText to two layouts, so I duplicated it
   // When the window is deleted, it deletes it's children: if the control is added twice, it is deleted twice
    wxStaticText *emptyStaticText2 =
       new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );   
   
   
   //-----------------------------------------------------------------
   // platform dependent button size
   //-----------------------------------------------------------------
   #ifdef __WXMAC__
   int arrowW = -1;
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   int colorW = 10;
   #endif
   #else
   int arrowW = -1;
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   int colorW = 25;
   #endif
   #endif
   
   //-----------------------------------------------------------------
   // Data collect, update frequency and max plot points
   //-----------------------------------------------------------------
   wxStaticText *dataCollectFreqLabel1 =
      new wxStaticText(this, -1, wxT("Collect data every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *dataCollectFreqLabel2 =
      new wxStaticText(this, -1, wxT("step"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *updatePlotFreqLabel1 =
      new wxStaticText(this, -1, wxT("Update plot every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *updatePlotFreqLabel2 =
      new wxStaticText(this, -1, wxT("cycle"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *maxPlottedDataPointsLabel1 =
      new wxStaticText(this, -1, wxT("Max number of data points to plot "),
      wxDefaultPosition, wxSize(-1, -1), 0);

   mDataCollectFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(40, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   mUpdatePlotFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(40, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   mMaxPlottedDataPointsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("20000"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   mEnableStarsCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Enable Stars"), wxDefaultPosition, wxDefaultSize, 0);
   mEnableConstellationsCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Enable Constellations"), wxDefaultPosition, wxDefaultSize, 0);
   //mEnableConstellationsCheckBox->Disable();

   mStarCountStaticText =
      new wxStaticText(this, -1, wxT("Number of stars "), 
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mStarCountTextCtrl = 
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("7000"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   //mStarCountTextCtrl->Disable();
   
   wxBoxSizer *colFreqSizer = new wxBoxSizer(wxHORIZONTAL);
   colFreqSizer->Add(dataCollectFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   colFreqSizer->Add(mDataCollectFreqTextCtrl, 1, wxALIGN_LEFT|wxALL, bsize);
   colFreqSizer->Add(dataCollectFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *updFreqSizer = new wxBoxSizer(wxHORIZONTAL);
   updFreqSizer->Add(updatePlotFreqLabel1, 0, wxALIGN_LEFT|wxALL, bsize);
   updFreqSizer->Add(mUpdatePlotFreqTextCtrl, 1, wxALIGN_LEFT|wxALL, bsize);
   updFreqSizer->Add(updatePlotFreqLabel2, 0, wxALIGN_LEFT|wxALL, bsize);

   wxBoxSizer *maxPlotPointsSizer = new wxBoxSizer(wxHORIZONTAL);
   maxPlotPointsSizer->Add(maxPlottedDataPointsLabel1, 0, wxALIGN_LEFT | wxALL, bsize);
   maxPlotPointsSizer->Add(mMaxPlottedDataPointsTextCtrl, 1, wxALIGN_LEFT | wxALL, bsize);

   wxBoxSizer *starOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   starOptionSizer->Add(mStarCountStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   starOptionSizer->Add(mStarCountTextCtrl, 1, wxALIGN_LEFT|wxALL, bsize);
   
   wxBoxSizer *plotOptionSizer = new wxBoxSizer(wxVERTICAL);   
   plotOptionSizer->Add(colFreqSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(updFreqSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(maxPlotPointsSizer, 0, wxALIGN_LEFT | wxALL, bsize);
   plotOptionSizer->Add(mEnableStarsCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(mEnableConstellationsCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(starOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *numPointsToRedrawLabel1 =
      new wxStaticText(this, -1, gmatwxT("Number of points to redraw\n"
                                         "(Enter 0 to redraw whole plot)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mNumPointsToRedrawTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   wxBoxSizer *numPointsSizer = new wxBoxSizer(wxHORIZONTAL);
   numPointsSizer->Add(numPointsToRedrawLabel1, 1, wxALIGN_LEFT|wxALL, 0);
   numPointsSizer->Add(mNumPointsToRedrawTextCtrl, 0, wxALIGN_LEFT|wxALL, 0);
   
   plotOptionSizer->Add(numPointsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Show plot
   //-----------------------------------------------------------------
   mShowPlotCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   mShowLabelsCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Show Labels"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   
   plotOptionSizer->AddSpacer(10);
   plotOptionSizer->Add(mShowPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   plotOptionSizer->Add(mShowLabelsCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *plotOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Plot Option");
   
   plotOptionStaticSizer->Add(plotOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   #ifdef __ENABLE_GL_PERSPECTIVE__
   
   mPerspectiveModeCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Perspective Mode"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mUseFixedFovCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Fixed FOV Angle"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   viewOptionSizer->Add(mPerspectiveModeCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   viewOptionSizer->Add(mUseFixedFovCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mFovLabel =
      new wxStaticText(this, -1, wxT("Field Of View (deg): "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mFixedFovTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35, -1), 0);
   
   wxBoxSizer *fovSizer = new wxBoxSizer(wxHORIZONTAL);
   fovSizer->Add(mFovLabel, 0, wxALIGN_LEFT|wxALL, 0);
   fovSizer->Add(mFixedFovTextCtrl, 0, wxALIGN_LEFT|wxALL, 0);
   viewOptionSizer->Add(fovSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   #endif
   
   //-----------------------------------------------------------------
   // Drawing option
   //-----------------------------------------------------------------
   mWireFrameCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw WireFrame"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mEclipticPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Ecliptic Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mXYPlaneCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw XY Plane"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mAxesCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mGridCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Grid"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mOriginSunLineCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Sun Line"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   //-----------------------------------------------------------------
   // Field of View Options
   //-----------------------------------------------------------------
   #ifdef __ENABLE_FOV__
   wxBoxSizer *fovOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   mFovStaticText = 
      new wxStaticText(this, -1, wxT("Starting FOV "),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mFovTextCtrl = 
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("45"), wxDefaultPosition, wxSize(50, -1), 0);
   fovOptionSizer->Add(mFovStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   fovOptionSizer->Add(mFovTextCtrl, 0, wxALIGN_RIGHT|wxALL, bsize);
   
   wxBoxSizer *fovMinOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   mFovMinStaticText =
      new wxStaticText(this, -1, wxT("Min FOV        "),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mFovMinTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(50, -1), 0);
   fovMinOptionSizer->Add(mFovMinStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   fovMinOptionSizer->Add(mFovMinTextCtrl, 0, wxALIGN_RIGHT|wxALL, bsize);
   
   wxBoxSizer *fovMaxOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   mFovMaxStaticText = 
      new wxStaticText(this, -1, wxT("Max FOV       "),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mFovMaxTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("90"), wxDefaultPosition, wxSize(50, -1), 0);
   fovMaxOptionSizer->Add(mFovMaxStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   fovMaxOptionSizer->Add(mFovMaxTextCtrl, 0, wxALIGN_RIGHT|wxALL, bsize);
   #endif
   
   // Solver Iteration ComboBox
   wxStaticText *solverIterLabel =
      new wxStaticText(this, -1, wxT("Solver Iterations"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mSolverIterComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize, emptyList, wxCB_READONLY);
   
   // Get Solver Iteration option list from the Subscriber
   const std::string *solverIterList = Subscriber::GetSolverIterOptionList();
   int count = Subscriber::GetSolverIterOptionCount();
   for (int i=0; i<count; i++)
      mSolverIterComboBox->Append(solverIterList[i].c_str());
   wxBoxSizer *solverIterOptionSizer = new wxBoxSizer(wxHORIZONTAL);
   solverIterOptionSizer->Add(solverIterLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   solverIterOptionSizer->Add(mSolverIterComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View option
   //-----------------------------------------------------------------
   mUseInitialViewCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Use Initial View Def."),
      wxDefaultPosition, wxSize(-1, -1), bsize);

   wxBoxSizer *viewOptionSizer = new wxBoxSizer(wxVERTICAL);
   viewOptionSizer->Add(mUseInitialViewCheckBox, 0, wxALIGN_LEFT | wxALL, bsize);

   GmatStaticBoxSizer *viewOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Option");
   viewOptionStaticSizer->Add(viewOptionSizer, 0, wxALIGN_LEFT | wxALL, bsize);


   wxBoxSizer *drawOptionSizer = new wxBoxSizer(wxVERTICAL);
   drawOptionSizer->Add(20, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mWireFrameCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mEclipticPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mXYPlaneCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mAxesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mGridCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(mOriginSunLineCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(solverIterOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   #ifdef __ENABLE_FOV__
   drawOptionSizer->Add(fovOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(fovMinOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   drawOptionSizer->Add(fovMaxOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   #endif
   
   drawOptionSizer->Add(20, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *drawOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drawing Option");
   drawOptionStaticSizer->Add(drawOptionSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Available spacecrafts and objects
   //-----------------------------------------------------------------
   wxSize fontSize = GetFont().GetPixelSize();
   int lbHeight = 10 * fontSize.GetHeight();
   wxStaticText *scAvailableLabel =
      new wxStaticText(this, -1, wxT("Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mSpacecraftListBox = theGuiManager->
      GetSpacecraftListBox(this, ID_LISTBOX, wxSize(150, lbHeight), &mExcludedScList);
   wxStaticText *coAvailableLabel =
      new wxStaticText(this, -1, wxT("Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCelesPointListBox = theGuiManager->
      GetCelestialPointListBox(this, ID_LISTBOX, wxSize(150, lbHeight), &mExcludedCelesPointList);
   
   wxBoxSizer *availObjSizer = new wxBoxSizer(wxVERTICAL);
   availObjSizer->Add(scAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(mSpacecraftListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(coAvailableLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   availObjSizer->Add(mCelesPointListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // add, remove, clear buttons
   //-----------------------------------------------------------------
   addScButton = new wxButton(this, ADD_SP_BUTTON, wxT("-->"),
                              wxDefaultPosition, wxSize(arrowW, -1), wxBU_EXACTFIT);
   removeScButton = new wxButton(this, REMOVE_SP_BUTTON, wxT("<--"),
                                 wxDefaultPosition, wxSize(arrowW ,-1), wxBU_EXACTFIT);
   clearScButton = new wxButton(this, CLEAR_SP_BUTTON, wxT("<="),
                                wxDefaultPosition, wxSize(arrowW, -1), wxBU_EXACTFIT);
   
   wxBoxSizer *arrowButtonsSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsSizer->Add(addScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(removeScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(clearScButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Selected spacecraft and objects
   //-----------------------------------------------------------------
   wxStaticText *titleSelectedSc =
      new wxStaticText(this, -1, wxT("Selected Spacecraft"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *titleSelectedObj =
      new wxStaticText(this, -1, wxT("Selected Celestial Object"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mSelectedScListBox =
      new wxListBox(this, SC_SEL_LISTBOX, wxDefaultPosition, wxSize(150, lbHeight), //0,
                    emptyList, wxLB_SINGLE);
   mSelectedObjListBox =
      new wxListBox(this, OBJ_SEL_LISTBOX, wxDefaultPosition, wxSize(150, lbHeight), //0,
                    emptyList, wxLB_SINGLE);
   
   wxBoxSizer *mObjSelectedSizer = new wxBoxSizer(wxVERTICAL);
   mObjSelectedSizer->Add(titleSelectedSc, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(mSelectedScListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(titleSelectedObj, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjSelectedSizer->Add(mSelectedObjListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Draw object, orbit and target color
   //-----------------------------------------------------------------
   mDrawObjectCheckBox =
      new wxCheckBox(this, CHECKBOX, wxT("Draw Object"),
                     wxDefaultPosition, wxSize(-1, -1), 0);

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mOrbitColorButton = new wxButton(this, ORBIT_COLOR_BUTTON, "",
                                    wxDefaultPosition, wxSize(colorW, -1), 0);
   mTargetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mTargetColorButton = new wxButton(this, TARGET_COLOR_BUTTON, "",
                                     wxDefaultPosition, wxSize(colorW, -1), 0);
   #endif
   
   wxFlexGridSizer *scOptionSizer1 = new wxFlexGridSizer(1, 0, 0);
   scOptionSizer1->Add(mDrawObjectCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(20, 10, 0, wxALIGN_LEFT|wxALL, bsize);
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   scOptionSizer1->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mOrbitColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mTargetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   scOptionSizer1->Add(mTargetColorButton, 0, wxALIGN_LEFT|wxALL, bsize);
   #endif
   mScOptionSizer = new wxBoxSizer(wxVERTICAL);
   mScOptionSizer->Add(scOptionSizer1, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mObjectSizer = new wxFlexGridSizer(5, 0, 0);
   mObjectSizer->Add(availObjSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(arrowButtonsSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(mObjSelectedSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mObjectSizer->Add(mScOptionSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *viewObjectStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Object");
   viewObjectStaticSizer->Add(mObjectSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *coordSysLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxDefaultSize);
   
   wxStaticText *viewPointRefLabel =
      new wxStaticText(this, -1, wxT("View Point Reference"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewPointRefComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxDefaultSize, true);
   
   //-----------------------------------------------------------------
   // View point reference
   //-----------------------------------------------------------------
   mViewPointRef1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewPointRef2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewPointRef3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewPointRefStaticText = new wxStaticText(this, -1, wxT("km"),
      wxDefaultPosition, wxSize(-1, -1), 0);

   mViewPointRefSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointRefSizer->Add(mViewPointRef1TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef2TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRef3TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointRefSizer->Add(mViewPointRefStaticText, 0, wxALIGN_LEFT | wxALL, bsize);

   wxStaticText *viewPointVectorLabel =
      new wxStaticText(this, -1, wxT("View Point Vector"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewPointVectorComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxDefaultSize, true);
   
   //-----------------------------------------------------------------
   // View point vector
   //-----------------------------------------------------------------
   mViewPointVec1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewPointVec2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewPointVec3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("30000"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   mViewPointVecStaticText = new wxStaticText(this, -1, wxT("km"),
      wxDefaultPosition, wxSize(-1, -1), 0);

   mViewPointVectorSizer = new wxBoxSizer(wxHORIZONTAL);
   mViewPointVectorSizer->Add(mViewPointVec1TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec2TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVec3TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewPointVectorSizer->Add(mViewPointVecStaticText, 0, wxALIGN_LEFT | wxALL, bsize);

   //-----------------------------------------------------------------
   // Add to view definition sizer
   //-----------------------------------------------------------------
   mViewDefSizer = new wxFlexGridSizer(3, 0, 0);
   mViewDefSizer->Add(coordSysLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(emptyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   mViewDefSizer->Add(viewPointRefLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointRefComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointRefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mViewDefSizer->Add(viewPointVectorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointVectorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewPointVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *viewScaleFactorLabel =
      new wxStaticText(this, -1, wxT("View Scale Factor"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewScaleFactorTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   mViewDefSizer->Add(viewScaleFactorLabel, 0, wxALIGN_RIGHT|wxALL, bsize);
   mViewDefSizer->Add(mViewScaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   // Added duplicate emptyStaticText to avoid double free
   mViewDefSizer->Add(emptyStaticText2, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxStaticText *viewDirectionLabel =
      new wxStaticText(this, -1, wxT("View Direction"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mViewDirectionComboBox =
      theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX, wxDefaultSize, true);
   
   //-----------------------------------------------------------------
   // View direction
   //-----------------------------------------------------------------
   mViewDirVectorSizer = new wxBoxSizer(wxHORIZONTAL);

   mViewDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewDir3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("-1"), wxDefaultPosition, wxSize(80, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mViewDirStaticText = new wxStaticText(this, -1, wxT("km"),
      wxDefaultPosition, wxSize(-1, -1), 0);

   mViewDirVectorSizer->Add(mViewDir1TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir2TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDir3TextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   mViewDirVectorSizer->Add(mViewDirStaticText, 0, wxALIGN_LEFT | wxALL, bsize);

   mViewDefSizer->Add(viewDirectionLabel, 0, wxALIGN_RIGHT | wxALL, bsize);
   mViewDefSizer->Add(mViewDirectionComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   mViewDefSizer->Add(mViewDirVectorSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *viewDefStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Definition");
   viewDefStaticSizer->Add(mViewDefSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // View up definition
   //-----------------------------------------------------------------
   wxStaticText *upCsLabel =
      new wxStaticText(this, -1, wxT("Coordinate System"),
            wxDefaultPosition, wxDefaultSize, 0);
   mViewUpCsComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxDefaultSize);
   wxStaticText *upAxisLabel =
      new wxStaticText(this, -1, wxT("Axis"),
            wxDefaultPosition, wxDefaultSize, 0);
   mViewUpAxisComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
                     wxSize(60,-1),6, axisArray, wxCB_READONLY);
   
   wxBoxSizer *viewUpSizer = new wxBoxSizer(wxHORIZONTAL);
   viewUpSizer->Add(upCsLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->Add(mViewUpCsComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->AddSpacer(20);
   viewUpSizer->Add(upAxisLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   viewUpSizer->Add(mViewUpAxisComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   GmatStaticBoxSizer *upDefStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "View Up Definition");
   
   upDefStaticSizer->Add(viewUpSizer, 0, wxALIGN_LEFT|wxALL, bsize);
      
   //-----------------------------------------------------------------
   // Add to page sizer
   //-----------------------------------------------------------------
   
   wxFlexGridSizer *pageSizer1 = new wxFlexGridSizer(3, 2, 0, 0);
   pageSizer1->Add(plotOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   pageSizer1->Add(viewObjectStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   pageSizer1->Add(drawOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);   
   pageSizer1->Add(viewDefStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   pageSizer1->Add(viewOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   pageSizer1->Add(upDefStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Add to middle sizer
   //-----------------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(pageSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage
      ("OrbitViewPanel::Create() Exiting sizers for Windows\n");
   #endif
   
   #if DEBUG_OPENGL_PANEL_CREATE
   MessageInterface::ShowMessage("OrbitViewPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void OrbitViewPanel::LoadData()
{
   #if DEBUG_OPENGL_PANEL_LOAD
   MessageInterface::ShowMessage("OrbitViewPanel::LoadData() entered.\n");
   #endif
   
   try
   {
      // load data from the core engine
      wxString str;
      Real rval;
      
      str.Printf("%d", mOrbitView->GetIntegerParameter("DataCollectFrequency"));
      mDataCollectFreqTextCtrl->SetValue(str);
      str.Printf("%d", mOrbitView->GetIntegerParameter("UpdatePlotFrequency"));
      mUpdatePlotFreqTextCtrl->SetValue(str);
      str.Printf("%d", mOrbitView->GetIntegerParameter("MaxPlotPoints"));
      mMaxPlottedDataPointsTextCtrl->SetValue(str);
      str.Printf("%d", mOrbitView->GetIntegerParameter("NumPointsToRedraw"));
      mNumPointsToRedrawTextCtrl->SetValue(str);
      str.Printf("%d", mOrbitView->GetIntegerParameter("StarCount"));
      mStarCountTextCtrl->SetValue(str);
      
      mShowPlotCheckBox->SetValue(mOrbitView->GetBooleanParameter("ShowPlot"));
      mShowLabelsCheckBox->SetValue(mOrbitView->GetBooleanParameter("ShowLabels"));
      mXYPlaneCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("XYPlane") == "On");
      mEclipticPlaneCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("EclipticPlane") == "On");
      mWireFrameCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("WireFrame") == "On");
      mAxesCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("Axes") == "On");
      mGridCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("Grid") == "On");
      mOriginSunLineCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("SunLine") == "On");
      mUseInitialViewCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("UseInitialView") == "On");
      mSolverIterComboBox->
         SetValue(mOrbitView->GetStringParameter("SolverIterations").c_str());
      mEnableStarsCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("EnableStars") == "On");
      mEnableConstellationsCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("EnableConstellations") == "On");
      
      // Update constellations and start count items
      if (mEnableStarsCheckBox->GetValue())
      {
         mEnableConstellationsCheckBox->Enable();
         mStarCountTextCtrl->Enable();
      }
      else
      {
         mEnableConstellationsCheckBox->Disable();
         mStarCountTextCtrl->Disable();
      }
      
      #ifdef __ENABLE_GL_PERSPECTIVE__
      mPerspectiveModeCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("PerspectiveMode") == "On");
      mUseFixedFovCheckBox->
         SetValue(mOrbitView->GetOnOffParameter("UseFixedFov") == "On");
      rval = mOrbitView->GetRealParameter("FixedFovAngle");
      mFixedFovTextCtrl->SetValue(theGuiManager->ToWxString(rval));
      #endif
      
      mCoordSysComboBox->SetStringSelection
         (mOrbitView->GetStringParameter("CoordinateSystem").c_str());
      
      //--------------------------------------------------------------
      // load view up direction info
      //--------------------------------------------------------------
      mViewUpAxisComboBox->
         SetStringSelection(mOrbitView->GetStringParameter("ViewUpAxis").c_str());
      mViewUpCsComboBox->
         SetStringSelection(mOrbitView->GetStringParameter("ViewUpCoordinateSystem").c_str());
      
      //--------------------------------------------------------------
      // load ViewPoint info
      //--------------------------------------------------------------
      wxString viewObj;
      viewObj = mOrbitView->GetStringParameter("ViewPointRefType").c_str();
      if (viewObj != "Vector")
         viewObj = mOrbitView->GetStringParameter("ViewPointReference").c_str();
      mViewPointRefComboBox->SetStringSelection(viewObj);
      
      viewObj = mOrbitView->GetStringParameter("ViewPointVectorType").c_str();
      if (viewObj != "Vector")
         viewObj = mOrbitView->GetStringParameter("ViewPointVector").c_str();
      mViewPointVectorComboBox->SetStringSelection(viewObj);
      
      viewObj = mOrbitView->GetStringParameter("ViewDirectionType").c_str();
      
      if (viewObj != "Vector")
         viewObj = mOrbitView->GetStringParameter("ViewDirection").c_str();
      mViewDirectionComboBox->SetStringSelection(viewObj);
      
      rval = mOrbitView->GetRealParameter("ViewScaleFactor");
      mViewScaleFactorTextCtrl->SetValue(theGuiManager->ToWxString(rval));
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
      {
         Rvector3 vec = mOrbitView->GetViewVector("ViewPointReference");
         
         #if DEBUG_OPENGL_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   ViewPointReference = %s\n", vec.ToString().c_str());
         #endif
         
         mViewPointRef1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewPointRef2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewPointRef3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));

         mViewDefSizer->Show(mViewPointRefSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewPointRefSizer, false);
      }
      
      // show vector if viewpoint vector name is Vector
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
      {
         Rvector3 vec = mOrbitView->GetViewVector("ViewPointVector");
         
         #if DEBUG_OPENGL_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   ViewPointVector = %s\n", vec.ToString().c_str());
         #endif
         
         mViewPointVec1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewPointVec2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewPointVec3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));
         
         mViewDefSizer->Show(mViewPointVectorSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewPointVectorSizer, false);
      }
      
      // show vector if view direction name is Vector
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
      {
         Rvector3 vec = mOrbitView->GetViewVector("ViewDirection");
         
         #if DEBUG_OPENGL_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   ViewDirector = %s\n", vec.ToString().c_str());
         #endif
         
         mViewDir1TextCtrl->SetValue(theGuiManager->ToWxString(vec[0]));
         mViewDir2TextCtrl->SetValue(theGuiManager->ToWxString(vec[1]));
         mViewDir3TextCtrl->SetValue(theGuiManager->ToWxString(vec[2]));
         
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      }
      else
      {
         mViewDefSizer->Show(mViewDirVectorSizer, false);
      }
      
      //--------------------------------------------------------------
      // get SpacePoint list to plot
      //--------------------------------------------------------------
      StringArray spNameList = mOrbitView->GetSpacePointList();
      int spCount = spNameList.size();
      
      #if DEBUG_OPENGL_PANEL_LOAD
      MessageInterface::ShowMessage
         ("OrbitViewPanel::LoadData() spCount=%d\n", spCount);
      
      for (int i=0; i<spCount; i++)
      {
         MessageInterface::ShowMessage
            ("OrbitViewPanel::LoadData() spNameList[%d]=%s\n",
             i, spNameList[i].c_str());
      }
      #endif
      
      StringArray scNameArray;
      StringArray nonScNameArray;
      
      // get spacecraft and non-spacecraft list
      for (int i=0; i<spCount; i++)
      {
         if (mSpacecraftListBox->FindString(spNameList[i].c_str()) == wxNOT_FOUND)
            nonScNameArray.push_back(spNameList[i]);
         else
            scNameArray.push_back(spNameList[i]);
      }
      
      mScCount = scNameArray.size();
      mNonScCount = nonScNameArray.size();
      
      #if DEBUG_OPENGL_PANEL_LOAD
      MessageInterface::ShowMessage
         ("OrbitViewPanel::LoadData() mScCount=%d, mNonScCount=%d\n",
          mScCount, mNonScCount);
      #endif
      
      //--------------------------------------------------------------
      // get object show, color
      //--------------------------------------------------------------
      if (mScCount > 0)
      {
         wxString *scNames = new wxString[mScCount];
         for (int i=0; i<mScCount; i++)
         {
            scNames[i] = scNameArray[i].c_str();
            
            mDrawObjectMap[scNameArray[i]] =
               mOrbitView->GetShowObject(scNameArray[i]);
            
            #ifdef __USE_COLOR_FROM_SUBSCRIBER__
            mOrbitColorMap[scNameArray[i]]
               = RgbColor(mOrbitView->GetColor("Orbit", scNameArray[i]));
            mTargetColorMap[scNameArray[i]]
               = RgbColor(mOrbitView->GetColor("Target", scNameArray[i]));
            #endif
            
            // Remove from the available ListBox
            mSpacecraftListBox->Delete(mSpacecraftListBox->FindString(scNames[i]));
            
            // Add to excluded list
            mExcludedScList.Add(scNames[i]);

            #ifdef __USE_COLOR_FROM_SUBSCRIBER__
            #if DEBUG_OPENGL_PANEL_LOAD > 1
            MessageInterface::ShowMessage
               ("OrbitViewPanel::LoadData() scName=%s, orbColor=%u, "
                "targetColor=%u\n", scNameArray[i].c_str(),
                mOrbitColorMap[scNameArray[i]].GetIntColor(),
                mTargetColorMap[scNameArray[i]].GetIntColor());
            #endif
            #endif
         }
         
         mSelectedScListBox->Set(mScCount, scNames);
         delete [] scNames;
      }
      
      if (mNonScCount > 0)
      {
         wxString *nonScNames = new wxString[mNonScCount];
         for (int i=0; i<mNonScCount; i++)
         {
            nonScNames[i] = nonScNameArray[i].c_str();
            
            mDrawObjectMap[nonScNameArray[i]] =
               mOrbitView->GetShowObject(nonScNameArray[i]);

            #ifdef __USE_COLOR_FROM_SUBSCRIBER__
            mOrbitColorMap[nonScNameArray[i]]
               = RgbColor(mOrbitView->GetColor("Orbit", nonScNameArray[i]));
            mTargetColorMap[nonScNameArray[i]]
               = RgbColor(mOrbitView->GetColor("Target", nonScNameArray[i]));
            #endif
            
            // Remove from the available ListBox
            mCelesPointListBox->Delete(mCelesPointListBox->FindString(nonScNames[i]));
            
            // Add to excluded list
            mExcludedCelesPointList.Add(nonScNames[i]);
         }
         
         mSelectedObjListBox->Set(mNonScCount, nonScNames);
         delete [] nonScNames;
      }
      
      // show spacecraft option
      mSelectedScListBox->SetSelection(0);
      ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   // deselect available object list
   mSpacecraftListBox->Deselect(mSpacecraftListBox->GetSelection());
   mCelesPointListBox->Deselect(mCelesPointListBox->GetSelection());
   
   #ifdef __ENABLE_GL_PERSPECTIVE__
   mPerspectiveModeCheckBox->Enable();
   #endif
   
   EnableUpdate(false);
   
   #ifdef __ENABLE_GL_PERSPECTIVE__
   if (!mUseFixedFovCheckBox->IsChecked())
   {
      mFovLabel->Disable();
      mFixedFovTextCtrl->Disable();
   }
   
   // if perspective mode, enalbe fov
   if (mPerspectiveModeCheckBox->IsChecked())
   {
      mUseFixedFovCheckBox->Enable();
      if (mUseFixedFovCheckBox->IsChecked())
      {
         mFovLabel->Enable();
         mFixedFovTextCtrl->Enable();
      }
      else
      {
         mFovLabel->Disable();
         mFixedFovTextCtrl->Disable();
      }
   }
   else
   {
      mUseFixedFovCheckBox->Disable();
      mFovLabel->Disable();
      mFixedFovTextCtrl->Disable();
   }
   #endif
   
   
   #if DEBUG_OPENGL_PANEL_LOAD
   MessageInterface::ShowMessage("OrbitViewPanel::LoadData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void OrbitViewPanel::SaveData()
{
   #if DEBUG_OPENGL_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitViewPanel::SaveData() entered, mHasIntegerDataChanged=%d, mHasRealDataChanged=%d\n",
       mHasIntegerDataChanged, mHasRealDataChanged);
   #endif
   
   canClose = true;
   std::string str1, str2;
   Integer collectFreq = 0, updateFreq = 0, maxPlotPoints = 0, pointsToRedraw = 0, starCount = 0;
   #ifdef __ENABLE_FOV__
   Integer initialFOV = 0, minFOV = 0, maxFOV = 0;
   #endif
   Real scaleFactor = 0.0;
   Real viewRef[3], viewVec[3], viewDir[3];
   Rvector3 vec;
   bool setVector = false;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------

   if (mHasIntegerDataChanged)
   {
      CheckInteger(collectFreq, mDataCollectFreqTextCtrl->GetValue().c_str(),
                   "DataCollectFrequency", "Integer Number > 0", false, true, true);
      
      CheckInteger(updateFreq, mUpdatePlotFreqTextCtrl->GetValue().c_str(),
                   "UpdatePlotFrequency", "Integer Number > 0", false, true, true);

      CheckInteger(maxPlotPoints, mMaxPlottedDataPointsTextCtrl->GetValue().c_str(),
                   "MaxPlotPoints", "Integer Number > 0", false, true, true);
      
      CheckInteger(pointsToRedraw, mNumPointsToRedrawTextCtrl->GetValue().c_str(),
                   "NumPointsToRedraw", "Integer Number >= 0", false, true, true, true);
      
      CheckInteger(starCount, mStarCountTextCtrl->GetValue().c_str(),
                   "StarCount", "Integer Number > 0", false, true, true);
      
      #ifdef __ENABLE_FOV__
      CheckInteger(initialFOV, mFovTextCtrl->GetValue().c_str(),
                   "InitialFOV", "");
      CheckInteger(minFOV, mFovMinTextCtrl->GetValue().c_str(),
                   "MinFOV", "");
      CheckInteger(maxFOV, mFovMaxTextCtrl->GetValue().c_str(),
                   "MaxFOV", "");
      #endif
   }
   
   if ((mViewPointRefComboBox->GetStringSelection() == "Vector")||
       (mViewPointVectorComboBox->GetStringSelection() == "Vector") ||
       (mViewDirectionComboBox->GetStringSelection() == "Vector"))
      setVector = true;
   
   #if DEBUG_OPENGL_PANEL_SAVE
   MessageInterface::ShowMessage("   setVector=%d\n", setVector);
   #endif
   
   if (setVector || mHasRealDataChanged)
   {
      CheckReal(scaleFactor, mViewScaleFactorTextCtrl->GetValue().c_str(),
                "ViewScaleFactor", "Real Number > 0", false, true, true);
      
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
      {
         CheckReal(viewRef[0], mViewPointRef1TextCtrl->GetValue().c_str(),
                   "ViewPointReference[1]", "Real Number");
         CheckReal(viewRef[1], mViewPointRef2TextCtrl->GetValue().c_str(),
                   "ViewPointReference[2]", "Real Number");
         CheckReal(viewRef[2], mViewPointRef3TextCtrl->GetValue().c_str(),
                   "ViewPointReference[3]", "Real Number");
      }
      
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
      {
         CheckReal(viewVec[0], mViewPointVec1TextCtrl->GetValue().c_str(),
                   "ViewPointVector[1]", "Real Number");
         CheckReal(viewVec[1], mViewPointVec2TextCtrl->GetValue().c_str(),
                   "ViewPointVector[2]", "Real Number");
         CheckReal(viewVec[2], mViewPointVec3TextCtrl->GetValue().c_str(),
                   "ViewPointVector[3]", "Real Number");
      }
      
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
      {
         CheckReal(viewDir[0], mViewDir1TextCtrl->GetValue().c_str(),
                   "ViewDirection[1]", "Real Number");
         CheckReal(viewDir[1], mViewDir2TextCtrl->GetValue().c_str(),
                   "ViewDirection[2]", "Real Number");
         CheckReal(viewDir[2], mViewDir3TextCtrl->GetValue().c_str(),
                   "ViewDirection[3]", "Real Number");
      }
   }

   // Since OrbitView base class automatically adds Sun for light source,
   // We don't need this part (LOJ: 2011.02.07)
   // Check if Sun was added to draw Sun line
//    if (mOriginSunLineCheckBox->IsChecked() &&
//        mSelectedObjListBox->FindString("Sun") == wxNOT_FOUND)
//    {
//       MessageInterface::PopupMessage
//          (Gmat::WARNING_, "\"Sun\" needs to be added to the view object list "
//           "to draw Sun line");
//       canClose = false;
//    }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      if (mHasIntegerDataChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving Integer data\n");
         #endif
         
         mHasIntegerDataChanged = false;
         mOrbitView->SetIntegerParameter("DataCollectFrequency", collectFreq);
         mOrbitView->SetIntegerParameter("UpdatePlotFrequency", updateFreq);
         mOrbitView->SetIntegerParameter("MaxPlotPoints", maxPlotPoints);
         mOrbitView->SetIntegerParameter("NumPointsToRedraw", pointsToRedraw);
         mOrbitView->SetIntegerParameter("StarCount", starCount);
         #ifdef __ENABLE_FOV__
         mOrbitView->SetIntegerParameter("MinFOV", minFOV);
         mOrbitView->SetIntegerParameter("MaxFOV", maxFOV);
         mOrbitView->SetIntegerParameter("InitialFOV", initialFOV);
         #endif
      }
      
      //--------------------------------------------------------------
      // save view definitions
      //--------------------------------------------------------------
      if (mHasViewInfoChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving View definition\n");
         #endif
         mHasViewInfoChanged = false;
         
         mOrbitView->SetStringParameter
            ("ViewPointReference",
             std::string(mViewPointRefComboBox->GetStringSelection().c_str()));
         mOrbitView->SetStringParameter
            ("ViewPointVector",
             std::string(mViewPointVectorComboBox->GetStringSelection().c_str()));
         mOrbitView->SetStringParameter
            ("ViewDirection",
             std::string(mViewDirectionComboBox->GetStringSelection().c_str()));
      } // end if ( mHasViewInfoChanged)
      
      
      //--------------------------------------------------------------
      // save scale factor and view definition vectors
      //--------------------------------------------------------------
      if (setVector || mHasRealDataChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving Real data\n");
         #endif
         
         mHasViewInfoChanged = false;
         mHasRealDataChanged = false;
         mOrbitView->SetRealParameter("ViewScaleFactor", scaleFactor);
         
         if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         {
            vec.Set(viewRef[0], viewRef[1], viewRef[2]);
            mOrbitView->SetStringParameter("ViewPointRefType", "Vector");
            mOrbitView->SetVector("ViewPointReference", vec);
         }
         
         if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         {
            vec.Set(viewVec[0], viewVec[1], viewVec[2]);
            mOrbitView->SetStringParameter("ViewPointVectorType", "Vector");
            mOrbitView->SetVector("ViewPointVector", vec);
         }
         
         if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         {
            vec.Set(viewDir[0], viewDir[1], viewDir[2]);
            mOrbitView->SetStringParameter("ViewDirectionType", "Vector");
            mOrbitView->SetVector("ViewDirection", vec);
         }
      }
      
      
      //--------------------------------------------------------------
      // save drawing options
      //--------------------------------------------------------------
      if (mHasDrawingOptionChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving drawing options\n");
         #endif
         mHasDrawingOptionChanged = false;
         
         mOrbitView->SetBooleanParameter("ShowPlot", mShowPlotCheckBox->IsChecked());
         //mOrbitView->Activate(mShowPlotCheckBox->IsChecked());
         mOrbitView->SetBooleanParameter("ShowLabels", mShowLabelsCheckBox->IsChecked());
         
         if (mXYPlaneCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("XYPlane", "On");
         else
            mOrbitView->SetOnOffParameter("XYPlane", "Off");
         
         if (mEclipticPlaneCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("EclipticPlane", "On");
         else
            mOrbitView->SetOnOffParameter("EclipticPlane", "Off");
         
         if (mWireFrameCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("WireFrame", "On");
         else
            mOrbitView->SetOnOffParameter("WireFrame", "Off");
         
         if (mAxesCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("Axes", "On");
         else
            mOrbitView->SetOnOffParameter("Axes", "Off");
         
         if (mGridCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("Grid", "On");
         else
            mOrbitView->SetOnOffParameter("Grid", "Off");
         
         if (mOriginSunLineCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("SunLine", "On");
         else
            mOrbitView->SetOnOffParameter("SunLine", "Off");
         
         //if (mOverlapCheckBox->IsChecked())
         //   mOrbitView->SetOnOffParameter("Overlap", "On");
         //else
         //   mOrbitView->SetOnOffParameter("Overlap", "Off");
         
         if (mUseInitialViewCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("UseInitialView", "On");
         else
            mOrbitView->SetOnOffParameter("UseInitialView", "Off");
         
         mOrbitView->SetStringParameter("SolverIterations",
                                         mSolverIterComboBox->GetValue().c_str());
      }

      //--------------------------------------------------------------
      // save star options
      //--------------------------------------------------------------
      if (mHasStarOptionChanged)
      {
         mHasStarOptionChanged = false;
         if (mEnableStarsCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("EnableStars", "On");
         else
            mOrbitView->SetOnOffParameter("EnableStars", "Off");
         
         if (mEnableConstellationsCheckBox->IsChecked())
            mOrbitView->SetOnOffParameter("EnableConstellations", "On");
         else
            mOrbitView->SetOnOffParameter("EnableConstellations", "Off");
      }
      
      
      //--------------------------------------------------------------
      // save perspective info
      //--------------------------------------------------------------
      #ifdef __ENABLE_GL_PERSPECTIVE__
      #if DEBUG_OPENGL_PANEL_SAVE
      MessageInterface::ShowMessage("   Saving perspective mode data\n");
      #endif
      
      if (mPerspectiveModeCheckBox->IsChecked())
         mOrbitView->SetOnOffParameter("PerspectiveMode", "On");
      else
         mOrbitView->SetOnOffParameter("PerspectiveMode", "Off");
      
      if (mUseFixedFovCheckBox->IsChecked())
         mOrbitView->SetOnOffParameter("UseFixedFov", "On");
      else
         mOrbitView->SetOnOffParameter("UseFixedFov", "Off");
      
      Real fov;
      std::string fovStr = mFixedFovTextCtrl->GetValue();
      if (!GmatStringUtil::ToReal(fovStr, &fov) || fov < 1)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                 inputString[0].c_str(), "FixedFovAngle", "Real Number >= 1");
         return;
      }
      mOrbitView->SetRealParameter("FixedFovAngle", fov);
      #endif
      
      
      //--------------------------------------------------------------
      // save spacecraft list
      //--------------------------------------------------------------
      if (mHasSpChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving spacecraft and body list\n");
         #endif
         
         mHasSpChanged = false;
         #ifdef __USE_COLOR_FROM_SUBSCRIBER__
         mHasOrbitColorChanged = true;
         mHasTargetColorChanged = true;
         #endif
         mScCount = mSelectedScListBox->GetCount();
         mNonScCount = mSelectedObjListBox->GetCount();
         
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   mScCount=%d, mNonScCount=%d\n", mScCount, mNonScCount);
         #endif
         
         // clear the list first
         mOrbitView->TakeAction("Clear");
         
         // add spacecraft
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage("   Sc[%d] = %s\n", i, mSelSpName.c_str());
            #endif
            
            mOrbitView->SetStringParameter("Add", mSelSpName, i);
         }
         
         // add non-spacecraft
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OrbitViewPanel::SaveData() NonSc[%d] = %s\n", i,
                mSelSpName.c_str());
            #endif
            
            mOrbitView->SetStringParameter("Add", mSelSpName, mScCount+i);
         }
      }
      
      //--------------------------------------------------------------
      // save draw object
      //--------------------------------------------------------------
      if (mHasShowObjectChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving show object flags\n");
         #endif
         
         mHasShowObjectChanged = false;
         
         // change draw spacecraft
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
         
            mOrbitView->
               SetShowObject( mSelSpName, mDrawObjectMap[mSelSpName]);
         }
         
         // change draw non-spacecraft
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
         
            mOrbitView->
               SetShowObject(mSelSpName, mDrawObjectMap[mSelSpName]);
         }
      }

      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      //--------------------------------------------------------------
      // save orbit color
      //--------------------------------------------------------------
      if (mHasOrbitColorChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving orbit colors\n");
         #endif
         
         mHasOrbitColorChanged = false;
         
         // change spacecraft orbit color
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("OrbitViewPanel::SaveData() objName=%s, orbColor=%u\n",
                mSelSpName.c_str(), mOrbitColorMap[mSelSpName].GetIntColor());
            #endif
            
            mOrbitView->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
         }
         
         // change non-spacecraft orbit color
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
            
            mOrbitView->
               SetColor("Orbit", mSelSpName,
                        mOrbitColorMap[mSelSpName].GetIntColor());
         }
      }
      
      //--------------------------------------------------------------
      // save target color
      //--------------------------------------------------------------
      if (mHasTargetColorChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving target colors\n");
         #endif
         
         mHasTargetColorChanged = false;
         
         // change spacecraft target color
         for (int i=0; i<mScCount; i++)
         {
            mSelSpName = std::string(mSelectedScListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("   objName=%s targetColor=%u\n",
                mSelSpName.c_str(), mTargetColorMap[mSelSpName].GetIntColor());
            #endif
            
            mOrbitView->
               SetColor("Target", mSelSpName,
                        mTargetColorMap[mSelSpName].GetIntColor());
         }
         
         // change non-spacecraft target color
         for (int i=0; i<mNonScCount; i++)
         {
            mSelSpName = std::string(mSelectedObjListBox->GetString(i).c_str());
            
            #if DEBUG_OPENGL_PANEL_SAVE
            MessageInterface::ShowMessage
               ("   objName=%s targetColor=%u\n",
                mSelSpName.c_str(), mTargetColorMap[mSelSpName].GetIntColor());
            #endif
            
            mOrbitView->
               SetColor("Target", mSelSpName,
                        mTargetColorMap[mSelSpName].GetIntColor());
         }
      }
      #endif
      
      //--------------------------------------------------------------
      // save coordinate system
      //--------------------------------------------------------------
      if (mHasCoordSysChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving view coordinate system\n");
         #endif
         
         mHasCoordSysChanged = false;
         mOrbitView->SetStringParameter
            ("CoordinateSystem",
             std::string(mCoordSysComboBox->GetStringSelection().c_str()));
      }
      
      //--------------------------------------------------------------
      // save view up direction info
      //--------------------------------------------------------------
      if (mHasViewUpInfoChanged)
      {
         #if DEBUG_OPENGL_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving view up coordinate system\n");
         #endif
         
         mHasViewUpInfoChanged = false;
         mOrbitView->SetStringParameter
            ("ViewUpCoordinateSystem",
             std::string(mViewUpCsComboBox->GetStringSelection().c_str()));
         mOrbitView->SetStringParameter
            ("ViewUpAxis",
             std::string(mViewUpAxisComboBox->GetStringSelection().c_str()));
      }
            
      EnableUpdate(false);
      canClose = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   #if DEBUG_OPENGL_PANEL_SAVE
   MessageInterface::ShowMessage("OrbitViewPanel::SaveData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnAddSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnAddSpacePoint(wxCommandEvent& event)
{
   #ifdef DEBUG_ADD_SP
   MessageInterface::ShowMessage("OrbitViewPanel::OnAddSpacePoint() entered\n");
   #endif
   
   if (mSpacecraftListBox->GetSelection() != -1)
   {
      // get string in first list and then search for it
      // in the second list
      wxString str = mSpacecraftListBox->GetStringSelection();
      int strId = mSpacecraftListBox->FindString(str);
      int found = mSelectedScListBox->FindString(str);
      
      #ifdef DEBUG_ADD_SP
      MessageInterface::ShowMessage
         ("   str='%s', strId=%d, found=%d\n", str.c_str(), strId, found);
      #endif
      
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         mSelectedScListBox->Append(str);
         mSelectedScListBox->SetStringSelection(str);
         
         // Removed from available list
         mSpacecraftListBox->Delete(strId);
         
         // select next available item
         if (strId == 0)
            mSpacecraftListBox->SetSelection(0);
         else if (strId > 0)
            mSpacecraftListBox->SetSelection(strId - 1);
         
         // deselect selected other object
         mSelectedObjListBox->Deselect(mSelectedObjListBox->GetSelection());
         
         // Add to excluded list
         mExcludedScList.Add(str);
         
         mDrawObjectMap[str.WX_TO_STD_STRING] = true;
         ShowSpacePointOption(str, true);
         mHasSpChanged = true;
         EnableUpdate(true);
      }
   }
   else if (mCelesPointListBox->GetSelection() != -1)
   {
      wxString str = mCelesPointListBox->GetStringSelection();
      int strId = mCelesPointListBox->FindString(str);
      int found = mSelectedObjListBox->FindString(str);
      
      #ifdef DEBUG_ADD_SP
      MessageInterface::ShowMessage
         ("   str='%s', strId=%d, found=%d\n", str.c_str(), strId, found);
      #endif
      
      // if the string wasn't found in the second list, insert it
      if (found == wxNOT_FOUND)
      {
         // Add to selected list
         mSelectedObjListBox->Append(str);
         mSelectedObjListBox->SetStringSelection(str);
         
         // Removed from available list
         mCelesPointListBox->Delete(strId);
         
         // select next available item
         if (strId == 0)
            mCelesPointListBox->SetSelection(0);
         else if (strId > 0)
            mCelesPointListBox->SetSelection(strId - 1);
         
         // deselect selected spacecraft
         mSelectedScListBox->Deselect(mSelectedScListBox->GetSelection());
         
         // Add to excluded list
         mExcludedCelesPointList.Add(str);
         
         mDrawObjectMap[str.WX_TO_STD_STRING] = true;
         ShowSpacePointOption(str, true, false);
         
         mHasSpChanged = true;
         EnableUpdate(true);
      }
   }
   
   Layout();
   Refresh();
   Update();
   
   #ifdef DEBUG_ADD_SP
   MessageInterface::ShowMessage("OrbitViewPanel::OnAddSpacePoint() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnRemoveSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnRemoveSpacePoint(wxCommandEvent& event)
{
   if (mSelectedScListBox->GetSelection() != -1)
   {
      wxString str = mSelectedScListBox->GetStringSelection();
      int sel = mSelectedScListBox->GetSelection();
      
      // Add to available list
      mSpacecraftListBox->Append(str);
      
      // Remove from selected list
      mSelectedScListBox->Delete(sel);
      
      // Remove from excluded list
      mExcludedScList.Remove(str);
      
      if (sel-1 < 0)
      {
         mSelectedScListBox->SetSelection(0);
         if (mSelectedScListBox->GetCount() == 0)
            ShowSpacePointOption("", false); // hide spacecraft color, etc
         else
            ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      }
      else
      {
         mSelectedScListBox->SetSelection(sel-1);
         ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
      }
   }
   else if (mSelectedObjListBox->GetSelection() != -1)
   {
      wxString str = mSelectedObjListBox->GetStringSelection();
      int sel = mSelectedObjListBox->GetSelection();
      
      // Add to available list
      mCelesPointListBox->Append(str);
      
      // Remove from selected list
      mSelectedObjListBox->Delete(sel);
      
      // Remove from excluded list
      mExcludedCelesPointList.Remove(str);
      
      if (sel-1 < 0)
      {
         mSelectedObjListBox->SetSelection(0);
         if (mSelectedObjListBox->GetCount() == 0)
            ShowSpacePointOption("", false); // hide spacecraft color, etc
         else
            ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true);
      }
      else
      {
         mSelectedObjListBox->SetSelection(sel-1);
         ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true);
      }
   }
   
   mHasSpChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnClearSpacePoint(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnClearSpacePoint(wxCommandEvent& event)
{
   if (mSelectedScListBox->GetSelection() != -1)
   {
      Integer count = mSelectedScListBox->GetCount();
      
      if (count == 0)
         return;
      
      for (Integer i = 0; i < count; i++)
         mSpacecraftListBox->Append(mSelectedScListBox->GetString(i));
      
      mSelectedScListBox->Clear();
      mExcludedScList.Clear();
   }
   else if (mSelectedObjListBox->GetSelection() != -1)
   {
      Integer count = mSelectedObjListBox->GetCount();
      
      if (count == 0)
         return;
      
      for (Integer i = 0; i < count; i++)
         mCelesPointListBox->Append(mSelectedObjListBox->GetString(i));
      
      mSelectedObjListBox->Clear();
      mExcludedCelesPointList.Clear();
   }
   
   ShowSpacePointOption("", false);
   mHasSpChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSelectAvailObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnSelectAvailObject(wxCommandEvent& event)
{
   if (event.GetEventObject() == mSpacecraftListBox)
      mCelesPointListBox->Deselect(mCelesPointListBox->GetSelection());
   else if (event.GetEventObject() == mCelesPointListBox)
      mSpacecraftListBox->Deselect(mSpacecraftListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnSelectSpacecraft(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnSelectSpacecraft(wxCommandEvent& event)
{
   ShowSpacePointOption(mSelectedScListBox->GetStringSelection(), true);
   mSelectedObjListBox->Deselect(mSelectedObjListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnSelectOtherObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnSelectOtherObject(wxCommandEvent& event)
{
   ShowSpacePointOption(mSelectedObjListBox->GetStringSelection(), true, false);
   mSelectedScListBox->Deselect(mSelectedScListBox->GetSelection());
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   #ifdef __ENABLE_GL_PERSPECTIVE__
   if (event.GetEventObject() == mPerspectiveModeCheckBox)
   {
      if (mPerspectiveModeCheckBox->IsChecked())
      {
         mUseFixedFovCheckBox->Enable();
         if (mUseFixedFovCheckBox->IsChecked())
         {
            mFovLabel->Enable();
            mFixedFovTextCtrl->Enable();
         }
         else
         {
            mFovLabel->Disable();
            mFixedFovTextCtrl->Disable();
         }
      }
      else
      {
          mUseFixedFovCheckBox->Disable();
          mFovLabel->Disable();
          mFixedFovTextCtrl->Disable();
      }
   }
   else if (event.GetEventObject() == mUseFixedFovCheckBox)
   {
      if (mUseFixedFovCheckBox->IsChecked())
      {
         mFovLabel->Enable();
         mFixedFovTextCtrl->Enable();
      }
      else
      {
         mFovLabel->Disable();
         mFixedFovTextCtrl->Disable();
      }
   }
   #endif
   
   
   if (event.GetEventObject() == mDrawObjectCheckBox)
   {
      if (mSelectedScListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
         mDrawObjectMap[mSelSpName] = mDrawObjectCheckBox->GetValue();
         mHasShowObjectChanged = true;
      }
      else if (mSelectedObjListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
         mDrawObjectMap[mSelSpName] = mDrawObjectCheckBox->GetValue();
         mHasShowObjectChanged = true;
      }
      
      #if DEBUG_OPENGL_PANEL_CHECKBOX
      MessageInterface::ShowMessage
         ("OrbitViewPanel::OnCheckBoxChange() mSelSpName=%s, show=%d\n",
          mSelSpName.c_str(), mDrawObjectMap[mSelSpName]);
      #endif
   }
   else if (event.GetEventObject() == mEnableStarsCheckBox)
   {
      if (mEnableStarsCheckBox->GetValue())
      {
         mEnableConstellationsCheckBox->Enable();
         mStarCountTextCtrl->Enable();
      }
      else
      {
         mEnableConstellationsCheckBox->Disable();
         //mEnableConstellationsCheckBox->SetValue(false);
         mStarCountTextCtrl->Disable();
      }
      mHasStarOptionChanged = true;
   }
   else if (event.GetEventObject() == mEnableConstellationsCheckBox)
   {
      mHasStarOptionChanged = true;
   }
   else
   {
      mHasDrawingOptionChanged = true;
   }
   
   EnableUpdate(true);
}


#ifdef __USE_COLOR_FROM_SUBSCRIBER__
//------------------------------------------------------------------------------
// void OnOrbitColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnOrbitColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mOrbitColor);

   wxColourDialog dialog(this, &data);
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      // if spacecraft is selected
      if (mSelectedScListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
         
         mOrbitColor = dialog.GetColourData().GetColour();
         mOrbitColorButton->SetBackgroundColour(mOrbitColor);
         mOrbitColorButton->Refresh();
         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue(), 0);
                                        //mOrbitColor.Alpha());
         
         #if DEBUG_OPENGL_PANEL_COLOR
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() red=%u, green=%u, blue=%u, alpha=%u\n",
             mOrbitColor.Red(), mOrbitColor.Green(), mOrbitColor.Blue(),
             mOrbitColor.Alpha());
         UnsignedInt intColor = mOrbitColorMap[mSelSpName].GetIntColor();
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() mOrbitColorMap[%s]=%u<%08x>\n",
             mSelSpName.c_str(), intColor, intColor);
         #endif
      }
      else if (mSelectedObjListBox->GetSelection() != -1)
      {
         mSelSpName = std::string(mSelectedObjListBox->GetStringSelection().c_str());
         
         mOrbitColor = dialog.GetColourData().GetColour();
         mOrbitColorButton->SetBackgroundColour(mOrbitColor);
         mOrbitColorButton->Refresh();
         
         mOrbitColorMap[mSelSpName].Set(mOrbitColor.Red(),
                                        mOrbitColor.Green(),
                                        mOrbitColor.Blue(), 0);
                                        //mOrbitColor.Alpha());
         
         #if DEBUG_OPENGL_PANEL_COLOR
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() red=%u, green=%u, blue=%u, alpha=%u\n",
             mOrbitColor.Red(), mOrbitColor.Green(), mOrbitColor.Blue(),
             mOrbitColor.Alpha());
         UnsignedInt intColor = mOrbitColorMap[mSelSpName].GetIntColor();
         MessageInterface::ShowMessage
            ("OnOrbitColorClick() mOrbitColorMap[%s]=%u<%08x>\n",
             mSelSpName.c_str(), intColor, intColor);
         #endif
      }
      
      EnableUpdate(true);
      mHasOrbitColorChanged = true;
   }
}


//------------------------------------------------------------------------------
// void OnTargetColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnTargetColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mTargetColor);

   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelSpName = std::string(mSelectedScListBox->GetStringSelection().c_str());
      
      mTargetColor = dialog.GetColourData().GetColour();
      mTargetColorButton->SetBackgroundColour(mTargetColor);
      mTargetColorButton->Refresh();
      mTargetColorMap[mSelSpName].Set(mTargetColor.Red(),
                                      mTargetColor.Green(),
                                      mTargetColor.Blue(), 0);
                                      //mTargetColor.Alpha());
      
      #if DEBUG_OPENGL_PANEL
      MessageInterface::ShowMessage
         ("OnTargetColorClick() red=%u, green=%u, blue=%u, alpha=%u\n",
          mTargetColor.Red(), mTargetColor.Green(), mTargetColor.Blue(),
          mTargetColor.Alpha());
      UnsignedInt intColor = mTargetColorMap[mSelSpName].GetIntColor();
      MessageInterface::ShowMessage
         ("OnTargetColorClick() mTargetColorMap[%s]=%u<%08x>\n",
          mSelSpName.c_str(), intColor, intColor);
      #endif
      
      EnableUpdate(true);
      mHasTargetColorChanged = true;
   }
}
#endif


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mCoordSysComboBox)
   {      
      mHasCoordSysChanged = true;
   }
   else if (event.GetEventObject() == mViewUpCsComboBox ||
            event.GetEventObject() == mViewUpAxisComboBox)
   {
      mHasViewUpInfoChanged = true;
   }
   else if (event.GetEventObject() == mViewPointRefComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointRefComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewPointRefSizer, true);
      else
         mViewDefSizer->Show(mViewPointRefSizer, false);
   }
   else if (event.GetEventObject() == mViewPointVectorComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewPointVectorComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewPointVectorSizer, true);
      else
         mViewDefSizer->Show(mViewPointVectorSizer, false);
   }
   else if (event.GetEventObject() == mViewDirectionComboBox)
   {
      mHasViewInfoChanged = true;
      
      if (mViewDirectionComboBox->GetStringSelection() == "Vector")
         mViewDefSizer->Show(mViewDirVectorSizer, true);
      else
         mViewDefSizer->Show(mViewDirVectorSizer, false);
   }
   else if (event.GetEventObject() == mSolverIterComboBox)
   {
      mHasDrawingOptionChanged = true;
   }
   // call Layout() to force layout of the children anew
   thePanelSizer->Layout();
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OrbitViewPanel::OnTextChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (((wxTextCtrl*)obj)->IsModified())
   {
      if (obj == mDataCollectFreqTextCtrl ||
          obj == mUpdatePlotFreqTextCtrl ||
          obj == mNumPointsToRedrawTextCtrl ||
          obj == mStarCountTextCtrl ||
          obj == mMaxPlottedDataPointsTextCtrl
          #ifdef __ENABLE_FOV__
          || obj == mFovTextCtrl ||
          obj == mFovMinTextCtrl ||
          obj == mFovMaxTextCtrl
          #endif
          )
      {
         mHasIntegerDataChanged = true;
      }
      else if (obj == mViewScaleFactorTextCtrl ||
               obj == mViewPointRef1TextCtrl ||
               obj == mViewPointRef2TextCtrl ||
               obj == mViewPointRef3TextCtrl ||
               obj == mViewPointVec1TextCtrl ||
               obj == mViewPointVec2TextCtrl ||
               obj == mViewPointVec3TextCtrl ||
               obj == mViewDir1TextCtrl ||
               obj == mViewDir2TextCtrl ||
               obj == mViewDir3TextCtrl)
      {
         mHasRealDataChanged = true;
      }
      
      EnableUpdate(true);
   }
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void ShowSpacePointOption(const wxString &name, bool show = true,
//                           bool isSc = true)
//------------------------------------------------------------------------------
void OrbitViewPanel::ShowSpacePointOption(const wxString &name, bool show,
                                          bool isSc)
{
   #if DEBUG_OPENGL_PANEL_SHOW
   MessageInterface::ShowMessage
      ("OrbitViewPanel::ShowSpacePointOption() name=%s, show=%d, isSc=%d\n",
       name.c_str(), show, isSc);
   #endif
   
   if (!name.IsSameAs(""))
   {
      mSelSpName = std::string(name.c_str());

      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      // if object name not found, insert
      if (mOrbitColorMap.find(mSelSpName) == mOrbitColorMap.end())
      {
         #if DEBUG_OPENGL_PANEL
         MessageInterface::ShowMessage
            ("ShowSpacePointOption() name not found, so adding it to color map\n");
         #endif
         
         mOrbitColorMap[mSelSpName] = RgbColor(GmatColor::RED);
         mTargetColorMap[mSelSpName] = RgbColor(GmatColor::ORANGE);
      }
      
      RgbColor orbColor = mOrbitColorMap[mSelSpName];
      RgbColor targColor = mTargetColorMap[mSelSpName];
      
      #if DEBUG_OPENGL_PANEL_SHOW
      MessageInterface::ShowMessage
         ("OrbitViewPanel::ShowSpacePointOption() orbColor=%u, targColor=%u\n",
          orbColor.GetIntColor(), targColor.GetIntColor());
      #endif
      #endif
      
      mDrawObjectCheckBox->SetValue(mDrawObjectMap[mSelSpName]);
      
      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      mOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
      // Set target color for all objects using targColor (LOJ: 2013.10.23)
      mTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      
      if (isSc)
         mTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      else
         mTargetColor.Set(GmatColor::GRAY); // Set target colot to gray for non-spacecraft
      
      mOrbitColorButton->SetBackgroundColour(mOrbitColor);
      mTargetColorButton->SetBackgroundColour(mTargetColor);
      mOrbitColorButton->Refresh();
      mTargetColorButton->Refresh();
      
      // Enable all objects target color (LOJ: 2013.10.23)
      mTargetColorLabel->Enable();
      mTargetColorButton->Enable();
      #endif
      
      mObjectSizer->Show(mScOptionSizer, show);
   }
   else
   {
      mObjectSizer->Show(mScOptionSizer, false);
   }
}

//--------------------------------------------------------------
// void ValidateFovValues()
// Just a helper function to make sure the Initial, Minimum, and
// Maximum FOV values make sense
//--------------------------------------------------------------
void OrbitViewPanel::ValidateFovValues()
{
   #ifdef __ENABLE_FOV__
   wxString fov = mFovTextCtrl->GetValue(),
      minFov = mFovMinTextCtrl->GetValue(),
      maxFov = mFovMaxTextCtrl->GetValue();
   double fovValue, minFovValue,   maxFovValue;
   fov.ToDouble(&fovValue); 
   minFov.ToDouble(&minFovValue);
   maxFov.ToDouble(&maxFovValue);
   if (minFovValue > maxFovValue)
      mFovMinTextCtrl->SetValue(maxFov);
   if (maxFovValue < minFovValue)
      mFovMaxTextCtrl->SetValue(minFov);
   if (minFovValue > fovValue)
      mFovMinTextCtrl->SetValue(fov);
   if (maxFovValue < fovValue)
      mFovMaxTextCtrl->SetValue(fov);
   if (fovValue < minFovValue)
      mFovTextCtrl->SetValue(minFov);
   if (fovValue > maxFovValue)
      mFovTextCtrl->SetValue(maxFov);
   #endif
}

