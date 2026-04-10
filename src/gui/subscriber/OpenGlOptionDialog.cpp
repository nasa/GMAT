//$Id$
//------------------------------------------------------------------------------
//                              OpenGlOptionDialog
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
// Author: Linda Jun
// Created: 2004/12/20
//
/**
 * Implements OpenGlOptionDialog class. This dialog allows user to view and
 * select various OpenGL plot options.
 */
//------------------------------------------------------------------------------

#include "OpenGlOptionDialog.hpp"
#include "MdiChildTrajFrame.hpp"
#include "CelesBodySelectDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "ColorTypes.hpp"           // for GmatColor::
#include "RgbColor.hpp"
#include "GmatAppData.hpp"
#include "StringUtil.hpp"           // for GmatStringUtil::ToInteger()
#include "MessageInterface.hpp"

#include "wx/colordlg.h"            // for wxColourDialog

// If we want to see plot in different coordiante system from the list
#define __SHOW_COORD_SYSTEM__

//#define DEBUG_GLOPTION_CREATE 1
//#define DEBUG_GLOPTION_LOAD 1
//#define DEBUG_GLOPTION 2
//#define DEBUG_GLOPTION_OBJECT 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OpenGlOptionDialog, wxDialog)
   EVT_TEXT(ID_TEXTCTRL, OpenGlOptionDialog::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, OpenGlOptionDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, OpenGlOptionDialog::OnComboBoxChange)
   EVT_LISTBOX(ID_LISTBOX, OpenGlOptionDialog::OnSelectObject)
   EVT_BUTTON(ID_BUTTON, OpenGlOptionDialog::OnButtonClick)
   EVT_BUTTON(ID_BUTTON_APPLY, OpenGlOptionDialog::OnApplyButtonClick)
   EVT_BUTTON(ID_EQPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_ECPLANE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_SUNLINE_COLOR_BUTTON, OpenGlOptionDialog::OnColorButtonClick)
   EVT_BUTTON(ID_OBJECT_COLOR_BUTTON, OpenGlOptionDialog::OnObjectColorButtonClick)
   EVT_CLOSE(OpenGlOptionDialog::OnClose)
END_EVENT_TABLE()

using namespace GmatPlot;

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// OpenGlOptionDialog(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs OpenGlOptionDialog object.
 *
 * @param <parent> parent window
 *
 */
//------------------------------------------------------------------------------
OpenGlOptionDialog::OpenGlOptionDialog(wxWindow *parent, const wxString &title,
                                       const wxArrayString &objectNames,
                                       const UnsignedIntArray &objectColors)
   : wxDialog(parent, -1, title)
{
   theGuiManager = GuiItemManager::GetInstance();
   mTrajFrame = (MdiChildTrajFrame*)parent;
   
   mHasUseViewPointSpecChanged = false;
   mHasUsePerspModeChanged = false;
   mHasDistanceChanged = false;
   mHasGotoObjectChanged = false;
   mHasCoordSysChanged = false;
   
   mHasDrawWireFrameChanged = false;
   mHasDrawXyPlaneChanged = false;
   mHasDrawAxesChanged = false;
   mHasDrawSunLineChanged = false;
   
   mHasXyPlaneColorChanged = false;
   mHasEcPlaneColorChanged = false;
   mHasSunLineColorChanged = false;
   mHasObjectColorChanged = false;
   
   mHasShowObjectChanged = false;
   mHasShowOrbitNormalChanged = false;
   
   mObjectCount = objectNames.GetCount();
   
   mObjectColorMap.clear();
   mShowObjectMap.clear();
   mShowOrbitNormalMap.clear();
   
   for (int i=0; i<mObjectCount; i++)
   {
      mObjectNames.Add(objectNames[i]);
      mObjectIntColors.push_back(objectColors[i]);

      mObjectColorMap[objectNames[i]] = RgbColor(objectColors[i]);
      mShowObjectMap[objectNames[i]] = true;
      mShowOrbitNormalMap[objectNames[i]] = false;
      
      #ifdef DEBUG_GLOPTION
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog() object=%s, color=%d\n",
          mObjectNames[i].c_str(), mObjectIntColors[i]);
      #endif
   }
   
   mCoordSysName = "";
   mXyPlaneColor = wxColor("GREY");
   mEcPlaneColor = wxColor("DARK SLATE BLUE");
   mSunLineColor = wxColor("BROWN");
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~OpenGlOptionDialog()
//------------------------------------------------------------------------------
OpenGlOptionDialog::~OpenGlOptionDialog()
{
}


//------------------------------------------------------------------------------
// void SetDrawXyPlane(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawXyPlane(bool flag)
{
   mXyPlaneCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawWireFrame(bool flag)
{
   mWireFrameCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawAxes(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawAxes(bool flag)
{
   mAxesCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetDrawSunLine(bool flag)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetDrawSunLine(bool flag)
{
   mSunLineCheckBox->SetValue(flag);
}


//------------------------------------------------------------------------------
// void SetCoordSysName(const wxString &csName)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetCoordSysName(const wxString &csName)
{
   mCoordSysComboBox->SetStringSelection(csName);
   mCoordSysName = csName;
   
   // We don't want to enable this
   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void SetAnimationFrameInc(int inc)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SetAnimationFrameInc(int inc)
{
   wxString str;
   str.Printf("%d", inc);
   mFrameIncTextCtrl->SetValue(str);
   
   // We don't want to enable this
   //theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(const wxArrayString &objNames, ...
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateObjectList(const wxArrayString &objNames,
                                          const wxArrayString &validCSNames,
                                          const wxStringBoolMap &showObjects,
                                          const wxStringColorMap &objColors)
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateObjectList() mObjectNames.size=%d, "
       "mObjectIntColors.size=%d\n", mObjectNames.GetCount(), mObjectIntColors.size());
   #endif
   
   mObjectNames = objNames;
   mObjectCount = objNames.GetCount();
   mValidCSNames = validCSNames;
   mValidCSCount = validCSNames.GetCount();
   mInitialShowObjectMap = showObjects;
   mShowObjectMap = showObjects;
   mObjectColorMap = objColors;
   mObjectIntColors.clear();
   int intColor;
   
   for (int i=0; i<mObjectCount; i++)
   {
      intColor = mObjectColorMap[mObjectNames[i]].GetIntColor();
      mObjectIntColors.push_back(intColor);
      
      #ifdef DEBUG_GLOPTION_OBJECT
      MessageInterface::ShowMessage
         ("OpenGlOptionDialog::UpdateObjectList() object=%s, color=%d\n",
          mObjectNames[i].c_str(), mObjectIntColors[i]);
      #endif
   }

   UpdateObjectListBox();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::Create()
{
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage("OpenGlOptionDialog::Create() entered.\n");
   #endif
   int borderSize = 2;
   
   // emptyStaticText
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );
   
   //-----------------------------------------------------------------
   // animation
   //-----------------------------------------------------------------
   
   mUseInitialViewDefCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Use Initial View Definition"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *animationStaticText =
      new wxStaticText(this, -1, wxT("Update Interval (msec)\n<Esc> to interrupt"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mUpdateIntTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(60, -1), 0);
   
   wxStaticText *frameStaticText =
      new wxStaticText(this, -1, wxT("Frame Increment"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mFrameIncTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(60, -1), 0);
   
   mViewAnimationButton =
      new wxButton(this, ID_BUTTON, "View Animation", wxDefaultPosition,
                   wxSize(-1, -1), 0);
   
   wxFlexGridSizer *updateSizer = new wxFlexGridSizer(2, 0, 0);
   updateSizer->Add(animationStaticText, 0, wxALIGN_LEFT|wxALL, borderSize);
   updateSizer->Add(mUpdateIntTextCtrl, 0, wxALIGN_LEFT|wxALL, borderSize);
   updateSizer->Add(frameStaticText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL,
                    borderSize);
   updateSizer->Add(mFrameIncTextCtrl, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   wxBoxSizer *animationBoxSizer = new wxBoxSizer(wxVERTICAL);
   animationBoxSizer->Add(mUseInitialViewDefCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   animationBoxSizer->Add(updateSizer, 0, wxALIGN_LEFT|wxALL, borderSize);
   animationBoxSizer->Add(mViewAnimationButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   GmatStaticBoxSizer *animationStaticSizer
      = new GmatStaticBoxSizer(wxVERTICAL, this, "View Animation");
   
   animationStaticSizer->Add(animationBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() animationSizer created.\n");
   #endif

   
   #ifdef __SHOW_COORD_SYSTEM__
   //-----------------------------------------------------------------
   // view in other coordinate system
   //-----------------------------------------------------------------
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120, -1));
   mCoordSysGoButton =
      new wxButton(this, ID_BUTTON, "Go", wxDefaultPosition, wxSize(25, -1), 0);
   
   //wxFlexGridSizer *coordSysSizer = new wxFlexGridSizer(3, 0, 0);
   //coordSysSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   //coordSysSizer->Add(10, 10);
   //coordSysSizer->Add(mCoordSysGoButton, 0, wxALIGN_RIGHT|wxALL, borderSize);
   
   GmatStaticBoxSizer *coordSysStaticSizer
      = new GmatStaticBoxSizer(wxHORIZONTAL, this, "Coordinate System");
   
   //coordSysStaticSizer->Add(coordSysSizer, 0, wxALIGN_LEFT|wxALL, borderSize);
   coordSysStaticSizer->Add(mCoordSysComboBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   coordSysStaticSizer->Add(20, 10);
   coordSysStaticSizer->Add(mCoordSysGoButton, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() coordSysSizer created.\n");
   #endif
   #endif
   
   //-----------------------------------------------------------------
   // drawing option
   //-----------------------------------------------------------------
   mWireFrameCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Wire Frame"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mXyPlaneCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw XY Plane"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mAxesCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Axes"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   mSunLineCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Sun Lines"),
                     wxDefaultPosition, wxSize(150, -1), 0);
   
   // equatorial plane color
   mXyPlaneColorButton =
      new wxButton(this, ID_EQPLANE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20, 15), 0);
   mXyPlaneColorButton->SetBackgroundColour(mXyPlaneColor);
   
   // Sun line color
   mSunLineColorButton =
      new wxButton(this, ID_SUNLINE_COLOR_BUTTON, "", wxDefaultPosition,
                   wxSize(20, 15), 0);
   mSunLineColorButton->SetBackgroundColour(mSunLineColor);
   
   borderSize = 2;
   wxFlexGridSizer *drawGridSizer = new wxFlexGridSizer(2, 0, 0);
   drawGridSizer->Add(mWireFrameCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mAxesCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mXyPlaneCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mXyPlaneColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mSunLineCheckBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   drawGridSizer->Add(mSunLineColorButton, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::Create() drawGridSizer created.\n");
   #endif
   
   GmatStaticBoxSizer *drawingOptionStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drawing Options");
   
   drawingOptionStaticSizer->Add(drawGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // view bodies options
   //-----------------------------------------------------------------

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   mObjectListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(75,60), //0,
                    emptyList, wxLB_SINGLE);
   
   mObjectColorButton = new wxButton(this, ID_OBJECT_COLOR_BUTTON, "",
                                    wxDefaultPosition, wxSize(20,15), 0);
   mShowObjectCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Show"),
                     wxDefaultPosition, wxSize(90, -1), 0);
   
   mShowOrbitNormalCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Draw Orb Norm"),
                     wxDefaultPosition, wxSize(90, -1), 0);
   
   wxBoxSizer *colorSizer = new wxBoxSizer(wxVERTICAL);
   colorSizer->Add(mObjectColorButton, 0, wxALIGN_LEFT|wxALL, borderSize);
   colorSizer->Add(mShowObjectCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   colorSizer->Add(mShowOrbitNormalCheckBox, 0, wxALIGN_LEFT|wxALL, borderSize);
   
   RgbColor rgbcolor;
   
   for (int i=0; i<mObjectCount; i++)
   {
      mObjectListBox->Append(mObjectNames[i]);
   }
   
   wxBoxSizer *objectSizer = new wxBoxSizer(wxHORIZONTAL);
   objectSizer->Add(mObjectListBox, 0, wxALIGN_CENTRE|wxALL, borderSize);
   objectSizer->Add(colorSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //-----------------------------------------------------------------
   // create object sizers
   //-----------------------------------------------------------------
   wxStaticBox *viewObjectStaticBox =
      new wxStaticBox(this, -1, wxT("View Object"));
   
   mViewObjectSizer = new wxStaticBoxSizer(viewObjectStaticBox, wxVERTICAL);
   mViewObjectSizer->Add(objectSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);   
   
   //-----------------------------------------------------------------
   // create page sizers
   //-----------------------------------------------------------------
   wxBoxSizer *topViewSizer = new wxBoxSizer(wxVERTICAL);
   topViewSizer->Add(animationStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, borderSize);
   
   #ifdef __SHOW_COORD_SYSTEM__
   topViewSizer->Add(coordSysStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, borderSize);
   #endif
   
   topViewSizer->Add(drawingOptionStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, borderSize);
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(1, 0, 0);
   
   pageSizer->Add(topViewSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   pageSizer->Add(mViewObjectSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   //------------------------------------------------------
   // create dialog sizer
   //------------------------------------------------------   
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   
   theApplyButton =
      new wxButton(this, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
   
   theDialogSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   theDialogSizer->Add(theApplyButton, 0, wxALIGN_CENTRE|wxALL, 5);
   
   #if DEBUG_GLOPTION_CREATE
   MessageInterface::ShowMessage("OpenGlOptionDialog::Create() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loades data into GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::LoadData()
{
   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage("OpenGlOptionDialog::LoadData() entered.\n");
   #endif

   wxString strVal;
   
   // view mode
   mUseInitialViewDefCheckBox->SetValue(mTrajFrame->GetUseViewPointInfo());

   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::LoadData() setting mUpdateIntTextCtrl.\n");
   #endif
   
   // animiation
   strVal.Printf("%d", mTrajFrame->GetAnimationUpdateInterval());
   mUpdateIntTextCtrl->SetValue(strVal);
   
   strVal.Printf("%d", mTrajFrame->GetAnimationFrameIncrement());
   mFrameIncTextCtrl->SetValue(strVal);
   
   // equatorial plane, Sun line
   mXyPlaneCheckBox->SetValue(mTrajFrame->GetDrawXyPlane());
   RgbColor rgb(mTrajFrame->GetXyPlaneColor());
   mXyPlaneColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mXyPlaneColorButton->SetBackgroundColour(mXyPlaneColor);
   
   mSunLineCheckBox->SetValue(mTrajFrame->GetDrawSunLine());
   rgb.Set(mTrajFrame->GetSunLineColor());
   mSunLineColor.Set(rgb.Red(), rgb.Green(), rgb.Blue());
   mSunLineColorButton->SetBackgroundColour(mSunLineColor);
   
   // wire frame, axes, rotation axis
   mWireFrameCheckBox->SetValue(mTrajFrame->GetDrawWireFrame());
   mAxesCheckBox->SetValue(mTrajFrame->GetDrawAxes());
   
   // view object
   mObjectListBox->SetSelection(0);
   ShowSpacePointOption(mObjectListBox->GetStringSelection());

   #if DEBUG_GLOPTION_LOAD
   MessageInterface::ShowMessage("OpenGlOptionDialog::LoadData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// void ShowData()
//------------------------------------------------------------------------------
/**
 * Loades data into GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ShowData()
{
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer);
   theDialogSizer->Fit(this);
   theDialogSizer->SetSizeHints(this);

   CenterOnScreen(wxBOTH);
   
   LoadData();

   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves data from GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::SaveData()
{
   if (mHasUseViewPointSpecChanged)
   {
      mHasUseViewPointSpecChanged = false;
      mTrajFrame->SetUseInitialViewDef(mUseInitialViewDefCheckBox->GetValue());
   }
   
   if (mHasDrawXyPlaneChanged)
   {
      mHasDrawXyPlaneChanged = false;
      mTrajFrame->SetDrawXyPlane(mXyPlaneCheckBox->GetValue());
   }
      
   if (mHasDrawSunLineChanged)
   {
      mHasDrawSunLineChanged = false;
      mTrajFrame->SetDrawSunLine(mSunLineCheckBox->GetValue());
   }
   
   if (mHasDrawWireFrameChanged)
   {
      mHasDrawWireFrameChanged = false;
      mTrajFrame->SetDrawWireFrame(mWireFrameCheckBox->GetValue());
   }
   
   if (mHasDrawAxesChanged)
   {
      mHasDrawAxesChanged = false;
      mTrajFrame->SetDrawAxes(mAxesCheckBox->GetValue());
   }
   
   if (mHasXyPlaneColorChanged)
   {
      mHasXyPlaneColorChanged = false;
      RgbColor rgb(mXyPlaneColor.Red(), mXyPlaneColor.Green(), mXyPlaneColor.Blue());
      mTrajFrame->SetXyPlaneColor(rgb.GetIntColor());      
   }
   
   if (mHasSunLineColorChanged)
   {
      mHasSunLineColorChanged = false;
      RgbColor rgb(mSunLineColor.Red(), mSunLineColor.Green(), mSunLineColor.Blue());
      mTrajFrame->SetSunLineColor(rgb.GetIntColor());      
   }
   
   if (mHasShowObjectChanged)
   {
      mHasShowObjectChanged = false;
      mTrajFrame->SetShowObjects(mShowObjectMap);    
   }
   
   if (mHasShowOrbitNormalChanged)
   {
      mHasShowOrbitNormalChanged = false;
      mTrajFrame->SetShowOrbitNormals(mShowOrbitNormalMap);    
   }
   
   if (mHasObjectColorChanged)
   {
      mHasObjectColorChanged = false;
      RgbColor rgb(mSunLineColor.Red(), mSunLineColor.Green(), mSunLineColor.Blue());
      mTrajFrame->SetObjectColors(mObjectColorMap);      
   }
   
   mTrajFrame->RedrawPlot(false);

   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * Saves data from GUI compoments.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ResetData()
{
}


//------------------------------------------------------------------------------
// void UpdateObjectListBox()
//------------------------------------------------------------------------------
void OpenGlOptionDialog::UpdateObjectListBox()
{
   #ifdef DEBUG_GLOPTION_OBJECT
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::UpdateObjectListBox() mObjectCount=%d\n", mObjectCount);
   #endif
   
   mObjectListBox->Clear();
   
   for (int i=0; i<mObjectCount; i++)
      mObjectListBox->Append(mObjectNames[i]);
   
   mObjectListBox->SetSelection(0);
   ShowSpacePointOption(mObjectListBox->GetStringSelection());
   
}


//------------------------------------------------------------------------------
// bool ShowColorDialog(wxColor &oldColor, wxButton *button)
//------------------------------------------------------------------------------
/**
 * Shows color dialog and set color to button background.
 */
//------------------------------------------------------------------------------
bool OpenGlOptionDialog::ShowColorDialog(wxColor &oldColor, wxButton *button)
{
   #ifdef DEBUG_GLOPTION
   MessageInterface::ShowMessage
      ("ShowColorDialog() old color: R=%d G=%d B=%d\n",
       oldColor.Red(), oldColor.Green(), oldColor.Blue());
   #endif
   
   wxColourData data;
   data.SetColour(oldColor);
   
   wxColourDialog dlg(this, &data);
   dlg.Center();
   
   if (dlg.ShowModal() == wxID_OK)
   {
      oldColor = dlg.GetColourData().GetColour();
      button->SetBackgroundColour(oldColor);
      theApplyButton->Enable();
      
      #ifdef DEBUG_GLOPTION
      MessageInterface::ShowMessage
         ("ShowColorDialog() new color: R=%d G=%d B=%d\n",
          oldColor.Red(), oldColor.Green(), oldColor.Blue());
      #endif
      
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles TextCtrl event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnTextChange(wxCommandEvent& event)
{
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles CheckBox event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mUseInitialViewDefCheckBox)
      mHasUseViewPointSpecChanged = true;
   else if (event.GetEventObject() == mXyPlaneCheckBox)
      mHasDrawXyPlaneChanged = true;
   else if (event.GetEventObject() == mSunLineCheckBox)
      mHasDrawSunLineChanged = true;
   else if (event.GetEventObject() == mWireFrameCheckBox)
      mHasDrawWireFrameChanged = true;
   else if (event.GetEventObject() == mAxesCheckBox)
      mHasDrawAxesChanged = true;
   else if (event.GetEventObject() == mShowObjectCheckBox)
   {
      mShowObjectMap[mObjectListBox->GetStringSelection()] =
         mShowObjectCheckBox->GetValue();
      mHasShowObjectChanged = true;
   }
   else if (event.GetEventObject() == mShowOrbitNormalCheckBox)
   {
      mShowOrbitNormalMap[mObjectListBox->GetStringSelection()] =
         mShowOrbitNormalCheckBox->GetValue();
      mHasShowOrbitNormalChanged = true;
   }
   
   if (event.GetEventObject() != mUseInitialViewDefCheckBox)
      theApplyButton->Enable();
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles ComboBox event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      if (!mCoordSysName.IsSameAs(mCoordSysComboBox->GetStringSelection()))
      {
         mHasCoordSysChanged = true;         
         mCoordSysName = mCoordSysComboBox->GetStringSelection();
         theApplyButton->Enable();
      }
   }
}


//------------------------------------------------------------------------------
// void OnColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnColorButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mXyPlaneColorButton)
   {
      mHasXyPlaneColorChanged = ShowColorDialog(mXyPlaneColor, mXyPlaneColorButton);
   }
   else if (event.GetEventObject() == mSunLineColorButton)
   {
      mHasSunLineColorChanged = ShowColorDialog(mSunLineColor, mSunLineColorButton);
   }
}


//------------------------------------------------------------------------------
// void OnObjectColorButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles button click event.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnObjectColorButtonClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mObjectColor);
         
   wxColourDialog dlg(this, &data);
   dlg.Center();
   
   if (dlg.ShowModal() == wxID_OK)
   {
      mHasObjectColorChanged = true;
      wxString name = mObjectListBox->GetStringSelection();
      mObjectColor = dlg.GetColourData().GetColour();
      mObjectColorButton->SetBackgroundColour(mObjectColor);
      
      mObjectColorMap[name].Set(mObjectColor.Red(),
                                mObjectColor.Green(),
                                mObjectColor.Blue());
      
      theApplyButton->Enable();
   }
}


//------------------------------------------------------------------------------
// void OnApplyButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnApplyButtonClick(wxCommandEvent& event)
{
   theApplyButton->Disable();
   SaveData();
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewAnimationButton)
   {
      mTrajFrame->SetUseInitialViewDef(mUseInitialViewDefCheckBox->GetValue());
      bool valid = true;
      
      // Check animation update interval and frame increment
      std::string str = mUpdateIntTextCtrl->GetValue().c_str();
      if (!GmatStringUtil::ToInteger(str, mAnimationUpdInt))
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "\"%s\" is invalid value for \"Update Interval\"."
             "\nPlease enter Integer value >= 0", str.c_str());
         valid = false;
      }
      
      str = mFrameIncTextCtrl->GetValue().c_str();
      bool validInc = GmatStringUtil::ToInteger(str, mAnimationFrameInc);
      
      if (!validInc || mAnimationFrameInc < 1)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "\"%s\" is invalid value for \"Frame Increment\"."
             "\nPlease enter Integer value > 0", str.c_str());
         valid = false;
      }
      
      if (valid)
      {
         mTrajFrame->SetAnimationUpdateInterval(mAnimationUpdInt);
         mTrajFrame->SetAnimationFrameIncrement(mAnimationFrameInc);
         mTrajFrame->RedrawPlot(true);
      }
   }
   #ifdef __SHOW_COORD_SYSTEM__
   else if (event.GetEventObject() == mCoordSysGoButton)
   {
      if (mCoordSysName != "")
      {
         //MessageInterface::ShowMessage("===> mCoordSysName=%s\n", mCoordSysName.c_str());
         mHasCoordSysChanged = false;
         mTrajFrame->DrawInOtherCoordSystem(mCoordSysName);
      }
   }
   #endif
}


//------------------------------------------------------------------------------
// void OnSelectObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnSelectObject(wxCommandEvent& event)
{
   ShowSpacePointOption(mObjectListBox->GetStringSelection());
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles close event. The dialog is not closed here but the parent should
 * hide this dialog.
 */
//------------------------------------------------------------------------------
void OpenGlOptionDialog::OnClose(wxCloseEvent& event)
{
   Hide();
   mTrajFrame->ResetShowViewOption();
}


//------------------------------------------------------------------------------
// void ShowSpacePointOption(const wxString &name)
//------------------------------------------------------------------------------
void OpenGlOptionDialog::ShowSpacePointOption(const wxString &name)
{
   
   // if object name not found, insert
   if (mObjectColorMap.find(name) == mObjectColorMap.end())
   {
      mObjectColorMap[name] = RgbColor(GmatColor::L_BROWN32);
   }
   
   RgbColor orbColor = mObjectColorMap[name];
   
   #if DEBUG_GLOPTION
   MessageInterface::ShowMessage
      ("OpenGlOptionDialog::ShowSpacePointOption() name=%s, initialshow=%d, "
       "show=%d, orbColor=%08x\n", name.c_str(), mInitialShowObjectMap[name],
       mShowObjectMap[name], orbColor.GetIntColor());
   #endif
   
   mObjectColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());
   mObjectColorButton->SetBackgroundColour(mObjectColor);
   mShowObjectCheckBox->SetValue(mShowObjectMap[name]);
   mShowOrbitNormalCheckBox->SetValue(mShowOrbitNormalMap[name]);

   if (mInitialShowObjectMap[name])
   {
      mObjectColorButton->Enable();
      mShowObjectCheckBox->Enable();
      mShowOrbitNormalCheckBox->Enable();
   }
   else
   {
      mObjectColorButton->Disable();
      mShowObjectCheckBox->Disable();
      mShowOrbitNormalCheckBox->Disable();
   }
}

