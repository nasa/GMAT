//$Id$
//------------------------------------------------------------------------------
//                              GroundTrackPlotPanel
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
 * Implements GroundTrackPlotPanel class.
 */
//------------------------------------------------------------------------------

#include "GroundTrackPlotPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

#ifdef __USE_COLOR_FROM_SUBSCRIBER__
#include "ColorTypes.hpp"           // for namespace GmatColor::
#include <wx/clrpicker.h>           // for wxColorPickerCtrl
#include "wx/colordlg.h"            // for wxColourDialog
#endif

#include "bitmaps/OpenFolder.xpm"   // for browse button bitmap
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PANEL 1
//#define DEBUG_PANEL_CREATE 1
//#define DEBUG_PANEL_COLOR 1
//#define DEBUG_PANEL_LOAD 1
//#define DEBUG_PANEL_SAVE 1
//#define DEBUG_PANEL_CHECKBOX 1
//#define DEBUG_CENTRAL_BODY_CHANGE
//#define DEBUG_PANEL_SHOW 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(GroundTrackPlotPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BROWSE_BUTTON, GroundTrackPlotPanel::OnBrowseButton)
   EVT_CHECKBOX(ID_CHECKBOX, GroundTrackPlotPanel::OnCheckBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, GroundTrackPlotPanel::OnCheckListBoxChange)
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   EVT_COLOURPICKER_CHANGED(ID_COLOR_CTRL, GroundTrackPlotPanel::OnColorPickerChange)
   #endif
   EVT_COMBOBOX(ID_COMBOBOX, GroundTrackPlotPanel::OnComboBoxChange)
   EVT_LISTBOX(ID_CHECKLISTBOX, GroundTrackPlotPanel::OnCheckListBoxSelect)
   EVT_TEXT(ID_TEXTCTRL, GroundTrackPlotPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// GroundTrackPlotPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs GroundTrackPlotPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the GroundTrackPlotPanel GUI
 */
//------------------------------------------------------------------------------
GroundTrackPlotPanel::GroundTrackPlotPanel(wxWindow *parent,
                               const wxString &subscriberName)
   : GmatPanel(parent)
{
   #if DEBUG_PANEL
   MessageInterface::ShowMessage("GroundTrackPlotPanel() entering...\n");
   MessageInterface::ShowMessage("GroundTrackPlotPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
   #endif
   
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(subscriberName.c_str());
   
   mGroundTrackPlot = (GroundTrackPlot*)subscriber;
   
   // Set the pointer for the "Show Script" button
   mObject = mGroundTrackPlot;
   
   InitializeData();   
   Create();
   Show();
   
   // Listen for Spacecraft name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~GroundTrackPlotPanel()
//------------------------------------------------------------------------------
GroundTrackPlotPanel::~GroundTrackPlotPanel()
{
   #if DEBUG_PANEL
   MessageInterface::ShowMessage
      ("GroundTrackPlotPanel::~GroundTrackPlotPanel() unregistering controls\n");
   #endif
   
   theGuiManager->UnregisterComboBox("CelestialBody", mCentralBodyComboBox);
   theGuiManager->UnregisterCheckListBox("SpacePoint", mObjectCheckListBox);
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool GroundTrackPlotPanel::PrepareObjectNameChange()
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
void GroundTrackPlotPanel::ObjectNameChanged(UnsignedInt type,
                                       const wxString &oldName,
                                       const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("GroundTrackPlotPanel::ObjectNameChanged() type=%d, oldName=<%s>, "
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
void GroundTrackPlotPanel::InitializeData()
{
   mHasIntegerDataChanged = false;
   mHasRealDataChanged = false;
   mHasDataOptionChanged = false;
   mHasObjectListChanged = false;
   mHasCentralBodyChanged = false;
   mHasTextureMapChanged = false;
   
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   mHasOrbitColorChanged = false;
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   #endif
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::Create()
{
   #if DEBUG_PANEL_CREATE
      MessageInterface::ShowMessage("GroundTrackPlotPanel::Create() entered\n");
   #endif
   
   Integer bsize = 3; // border size
   
   // create axis array
   wxArrayString emptyList;
   wxString axisArray[] = {"X", "-X", "Y", "-Y", "Z", "-Z"};   
   wxArrayString empty;
   wxStaticText *emptyStaticText =
      new wxStaticText( this, -1, wxT("  "), wxDefaultPosition, wxDefaultSize, 0 );   
   
   
   //-----------------------------------------------------------------
   // Platform dependent button size
   //-----------------------------------------------------------------
   #ifdef __WXMAC__
   int arrowW = 40;
   int buttonWidth = 40;
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   int colorW = 40;
   #endif
   #else
   int arrowW = 20;
   int buttonWidth = 25;
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   int colorW = 25;
   #endif
   #endif
   
   //-----------------------------------------------------------------
   // Drawing Options
   //-----------------------------------------------------------------
   
   // Central body
   wxStaticText *centralBodyLabel =
      new wxStaticText(this, -1, wxT("Central Body"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   // Central body ComboBox
   mCentralBodyComboBox =
      theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX, wxSize(200,-1));
   
   // Selected object
   wxStaticText *selectedObjectLabel =
      new wxStaticText(this, -1, wxT("Selected Objects"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   // Selected object CheckListBox
   mObjectCheckListBox =
      theGuiManager->GetSpacePointCheckListBox(this, ID_CHECKLISTBOX, wxSize(200,100),
                                               false, false, true, true);

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   //-----------------------------------
   // Drawing color
   wxStaticText *orbitColorLabel =
      new wxStaticText(this, -1, wxT("Orbit Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);
   mOrbitColorCtrl =
      new wxColourPickerCtrl(this, ID_COLOR_CTRL, *wxRED,
                             wxDefaultPosition, wxSize(colorW, -1), 0);
   wxStaticText *targetColorLabel =
      new wxStaticText(this, -1, wxT("Target Color"),
                       wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE);   
   wxColour targColor = wxTheColourDatabase->Find("STEEL BLUE");
   mTargetColorCtrl =
      new wxColourPickerCtrl(this, ID_COLOR_CTRL, targColor,
                             wxDefaultPosition, wxSize(colorW, -1), 0);
   
   //-----------------------------------
   // Drawing color sizer
   wxFlexGridSizer *colorSizer = new wxFlexGridSizer(1);
   colorSizer->Add(orbitColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   colorSizer->Add(mOrbitColorCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   colorSizer->Add(targetColorLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   colorSizer->Add(mTargetColorCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   #endif

   
   //-----------------------------------
   // Drawing options sizer
   wxFlexGridSizer *drawingOptionFlexSizer = new wxFlexGridSizer(3);
   drawingOptionFlexSizer->Add(centralBodyLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   drawingOptionFlexSizer->Add(mCentralBodyComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   drawingOptionFlexSizer->Add(10, 2, wxALIGN_LEFT|wxALL, bsize);
   drawingOptionFlexSizer->Add(selectedObjectLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   drawingOptionFlexSizer->Add(mObjectCheckListBox, 0, wxALIGN_LEFT|wxALL, bsize);
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   drawingOptionFlexSizer->Add(colorSizer, wxALIGN_LEFT|wxALL, bsize);
   #endif
   
   // Add to drawing option sizer
   GmatStaticBoxSizer *drawingOptionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drawing Options");
   drawingOptionSizer->Add(drawingOptionFlexSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Data Options
   //-----------------------------------------------------------------
   
   // Data collect frequency
   wxStaticText *dataCollectFreqLabel =
      new wxStaticText(this, -1, wxT("Collect data every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mDataCollectFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(200, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   wxStaticText *dataCollectFreqUnit =
      new wxStaticText(this, -1, wxT("step(s)"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   // Plot update frequency
   wxStaticText *updatePlotFreqLabel =
      new wxStaticText(this, -1, wxT("Update plot every "),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mUpdatePlotFreqTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(200, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   wxStaticText *updatePlotFreqUnit =
      new wxStaticText(this, -1, wxT("cycle(s)"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   // Number of maximum points to plot
   wxStaticText *maxPlottedDataPointsLabel =
      new wxStaticText(this, -1, wxT("Max number of data points to plot "),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mMaxPlottedDataPointsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("20000"), wxDefaultPosition,
                     wxSize(200, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));

   // Number of points to redraw
   wxStaticText *numPointsToRedrawLabel =
      new wxStaticText(this, -1, wxT("Num. points to redraw\n(Enter 0 to draw all)"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mNumPointsToRedrawTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(200, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   
   // Show plot option
   mShowPlotCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);

   //-----------------------------------
   // Add to data options sizer
   wxFlexGridSizer *dataOptionFlexSizer = new wxFlexGridSizer(3);
   dataOptionFlexSizer->Add(dataCollectFreqLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(mDataCollectFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(dataCollectFreqUnit, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(updatePlotFreqLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(mUpdatePlotFreqTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(updatePlotFreqUnit, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(maxPlottedDataPointsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(mMaxPlottedDataPointsTextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   dataOptionFlexSizer->Add(10, 2, 0, wxALIGN_LEFT | wxALL, bsize);
   dataOptionFlexSizer->Add(numPointsToRedrawLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(mNumPointsToRedrawTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(10, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(10, 2, 0, wxALIGN_LEFT|wxALL, bsize);
   dataOptionFlexSizer->Add(mShowPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Add to data option sizer
   GmatStaticBoxSizer *dataOptionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Data Options");
   dataOptionSizer->Add(dataOptionFlexSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Other Options
   //-----------------------------------------------------------------
   
   // Solver Iteration ComboBox
   wxStaticText *solverIterLabel =
      new wxStaticText(this, -1, wxT("Solver Iterations"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   // Changed to use emptyList (LOJ: 2014.08.07)
   mSolverIterComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition,
                     wxSize(200, -1), emptyList, wxCB_READONLY);
   
   // Get Solver Iteration option list from the Subscriber
   const std::string *solverIterList = Subscriber::GetSolverIterOptionList();
   int count = Subscriber::GetSolverIterOptionCount();
   for (int i=0; i<count; i++)
      mSolverIterComboBox->Append(solverIterList[i].c_str());
   
   // Texture map
   wxStaticText *textureMapLabel =
      new wxStaticText(this, -1, wxT("Texture Map"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mTextureMapTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250, -1), 0);
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   mTextureMapBrowseButton =
      new wxBitmapButton(this, ID_BROWSE_BUTTON, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   
   //-----------------------------------
   // Other option sizer
   wxFlexGridSizer *otherOptionFlexSizer = new wxFlexGridSizer(3);
   otherOptionFlexSizer->Add(solverIterLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   otherOptionFlexSizer->Add(mSolverIterComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   otherOptionFlexSizer->Add(10, 2,  0, wxALIGN_LEFT|wxALL, bsize);
   otherOptionFlexSizer->Add(textureMapLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   otherOptionFlexSizer->Add(mTextureMapTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   otherOptionFlexSizer->Add(mTextureMapBrowseButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Add to other option sizer
   GmatStaticBoxSizer *otherOptionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Other Options");
   otherOptionSizer->Add(otherOptionFlexSizer, 0, wxALIGN_LEFT|wxALL, bsize);   
   
   //-----------------------------------------------------------------
   // Add to page sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(1);
   pageSizer->Add(drawingOptionSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(dataOptionSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   pageSizer->Add(otherOptionSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Add to middle sizer
   //-----------------------------------------------------------------
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_PANEL_CREATE
   MessageInterface::ShowMessage("GroundTrackPlotPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::LoadData()
{
   #if DEBUG_PANEL_LOAD
   MessageInterface::ShowMessage("GroundTrackPlotPanel::LoadData() entered.\n");
   #endif
   
   try
   {
      // Load data from the core engine
      wxString str;
      Real rval;
      
      // Load central body
      wxString centralBody = mGroundTrackPlot->GetStringParameter("CentralBody").c_str();
      mCentralBody = centralBody;
      
      #ifdef DEBUG_PANEL_LOAD
      MessageInterface::ShowMessage("   centralBody = '%s'\n", centralBody.c_str());
      #endif
      mCentralBodyComboBox->SetValue(centralBody);
      
      // Load space objects to draw
      StringArray objects = mGroundTrackPlot->GetStringArrayParameter("Add");
      int count = mObjectCheckListBox->GetCount();
      
      #ifdef DEBUG_PANEL_LOAD
      MessageInterface::ShowMessage("   There are %d items in ObjectCheckListBox\n", count);
      for (int i = 0; i < count; i++)
         MessageInterface::ShowMessage("      %d = '%s'\n", i, mObjectCheckListBox->GetString(i).c_str());
      MessageInterface::ShowMessage("   There are %d objects to draw\n", objects.size());
      for (unsigned int i = 0; i < objects.size(); i++)
         MessageInterface::ShowMessage("      %d = '%s'\n", i, objects[i].c_str());
      #endif

      
      std::string objName;
      // Load object drawing option and colors
      for (UnsignedInt i = 0; i < objects.size(); i++)
      {
         objName = objects[i];
         
         #ifdef __USE_COLOR_FROM_SUBSCRIBER__
         mOrbitColorMap[objName] = RgbColor(mGroundTrackPlot->GetColor("Orbit", objName));
         mTargetColorMap[objName] = RgbColor(mGroundTrackPlot->GetColor("Target", objName));
         #endif
         
         // Put check mark in the object list
         for (int j = 0; j < count; j++)
         {
            std::string itemName = mObjectCheckListBox->GetString(j).WX_TO_STD_STRING;
            if (itemName == objName)
            {
               mObjectCheckListBox->Check(j, true);
               break;
            }
         }
      }
      
      
      // Load drawing options
      str.Printf("%d", mGroundTrackPlot->GetIntegerParameter("DataCollectFrequency"));
      mDataCollectFreqTextCtrl->SetValue(str);
      str.Printf("%d", mGroundTrackPlot->GetIntegerParameter("UpdatePlotFrequency"));
      mUpdatePlotFreqTextCtrl->SetValue(str);
      str.Printf("%d", mGroundTrackPlot->GetIntegerParameter("MaxPlotPoints"));
      mMaxPlottedDataPointsTextCtrl->SetValue(str);
      str.Printf("%d", mGroundTrackPlot->GetIntegerParameter("NumPointsToRedraw"));
      mNumPointsToRedrawTextCtrl->SetValue(str);
      
      mShowPlotCheckBox->SetValue(mGroundTrackPlot->GetBooleanParameter("ShowPlot"));
      
      // Load solver iteration and texture map file
      mSolverIterComboBox->SetValue(mGroundTrackPlot->GetStringParameter("SolverIterations").c_str());
      mTextureFile = mGroundTrackPlot->GetStringParameter("TextureMap").c_str();
      mTextureMapTextCtrl->SetValue(mTextureFile);
      mTextureMapTextCtrl->SetInsertionPointEnd();
      
      // Select first object in the list to show color
      if (objects.size() > 0)
      {
         mObjectCheckListBox->SetSelection(0);
         #ifdef __USE_COLOR_FROM_SUBSCRIBER__
         ShowSpacePointColor(mObjectCheckListBox->GetStringSelection());
         #endif
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   EnableUpdate(false);
   
   #if DEBUG_PANEL_LOAD
   MessageInterface::ShowMessage("GroundTrackPlotPanel::LoadData() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::SaveData()
{
   #if DEBUG_PANEL_SAVE
   MessageInterface::ShowMessage
      ("GroundTrackPlotPanel::SaveData() entered, mHasCentralBodyChanged=%d, "
       " mHasObjectListChanged=%d\n   mHasIntegerDataChanged=%d, "
       "mHasTextureMapChanged=%d\n", mHasCentralBodyChanged, mHasObjectListChanged,
       mHasIntegerDataChanged, mHasTextureMapChanged);
   #endif
   
   canClose = true;
   std::string str1, str2;
   Integer collectFreq = 0, updateFreq = 0, maxPlotPoints = 0, pointsToRedraw = 0;
   
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
   }
      
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   GmatBase *clonedObj = mGroundTrackPlot->Clone();
   try
   {
      if (mHasCentralBodyChanged)
      {
         std::string newCentralBody = mCentralBodyComboBox->GetValue().WX_TO_STD_STRING;
         std::string newTexture = mTextureMapTextCtrl->GetValue().WX_TO_STD_STRING;
         clonedObj->SetStringParameter("CentralBody", newCentralBody);
         mHasCentralBodyChanged = false;
         // Set mHasTextureMapChanged to true so that it can be validated below (LOJ: 2014.11.03)
         mHasTextureMapChanged = true;
      }
      
      if (mHasObjectListChanged)
      {
         #if DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving object list\n");
         #endif
         clonedObj->TakeAction("Clear");
         int count = mObjectCheckListBox->GetCount();
         for (int i = 0; i < count; i++)
         {
            if (mObjectCheckListBox->IsChecked(i))
            {
               std::string objName =  mObjectCheckListBox->GetString(i).WX_TO_STD_STRING;
               #ifdef DEBUG_PANEL_SAVE
               MessageInterface::ShowMessage("   objName = '%s'\n", objName.c_str());
               #endif
               clonedObj->SetStringParameter("Add", objName);
            }
         }
         mHasObjectListChanged = false;
      }
      
      if (mHasIntegerDataChanged)
      {
         #if DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving Integer data\n");
         #endif
         clonedObj->SetIntegerParameter("DataCollectFrequency", collectFreq);
         clonedObj->SetIntegerParameter("UpdatePlotFrequency", updateFreq);
         clonedObj->SetIntegerParameter("MaxPlotPoints", maxPlotPoints);
         clonedObj->SetIntegerParameter("NumPointsToRedraw", pointsToRedraw);
         mHasIntegerDataChanged = false;
      }
      
      if (mHasDataOptionChanged)
      {
         #if DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving drawing options\n");
         #endif
         clonedObj->SetBooleanParameter("ShowPlot", mShowPlotCheckBox->GetValue());
         clonedObj->SetStringParameter("SolverIterations",
                                       mSolverIterComboBox->GetValue().c_str());
         mHasDataOptionChanged = false;
      }
      
      // Save texture map
      if (mHasTextureMapChanged)
      {
         #if DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage("   Saving new texture map\n");
         #endif
         // Validate texture map file (LOJ: 2014.10.30)
         wxString wxTextureMap = mTextureMapTextCtrl->GetValue();
         std::string textureMap = wxTextureMap.WX_TO_STD_STRING;
         if (clonedObj->IsParameterValid("TextureMap", textureMap))
         {
            clonedObj->SetStringParameter("TextureMap", textureMap);
            // If texture map file is blank, display default one
            if (wxTextureMap == "")
               mTextureMapTextCtrl->SetValue(STD_TO_WX_STRING(clonedObj->GetStringParameter("TextureMap").c_str()));
            mHasTextureMapChanged = false;
         }
         else
         {
            // Recheck central body
            mHasCentralBodyChanged = true;
            canClose = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, clonedObj->GetLastErrorMessage());
         }
      }
      
      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      // Save orbit colors
      if (mHasOrbitColorChanged)
      {
         SaveObjectColors("Orbit", mOrbitColorMap);
         mHasOrbitColorChanged = false;
      }
      
      // Save target colors
      if (mHasTargetColorChanged)
      {
         SaveObjectColors("Target", mTargetColorMap);
         mHasTargetColorChanged = false;
      }
      #endif
      
      // Copy new values to original object (LOJ: 2014.10.30)
      if (canClose)
      {
         mGroundTrackPlot->Copy(clonedObj);
         mCentralBody = mCentralBodyComboBox->GetValue().c_str();
         mTextureFile = mTextureMapTextCtrl->GetValue().c_str();
         EnableUpdate(false);
      }
   }
   catch (BaseException &e)
   {
      canClose = false;
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   delete clonedObj;
   
   #if DEBUG_PANEL_SAVE
   MessageInterface::ShowMessage("GroundTrackPlotPanel::SaveData() exiting.\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for browse button click
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnBrowseButton(wxCommandEvent& event)
{
   wxString oldname = mTextureMapTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      
      filename = dialog.GetPath().c_str();
      
      if (!filename.IsSameAs(oldname))
      {
         mTextureMapTextCtrl->SetValue(filename);
			mTextureMapTextCtrl->SetInsertionPointEnd();
         mHasTextureMapChanged = true;
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mShowPlotCheckBox)
   {
      mHasDataOptionChanged = true;
   }
   else
   {
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   mHasObjectListChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnCheckListBoxSelect(wxCommandEvent& event)
{
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   // Show object color
   wxString selObj = mObjectCheckListBox->GetStringSelection();
   ShowSpacePointColor(selObj);
   #endif
}


#ifdef __USE_COLOR_FROM_SUBSCRIBER__
//------------------------------------------------------------------------------
// void OnColorPickerChange(wxColourPickerEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnColorPickerChange(wxColourPickerEvent& event)
{
   #ifdef DEBUG_PANEL_COLOR
   MessageInterface::ShowMessage("GroundTrackPlotPanel::OnColorPickerChange() entered\n");
   #endif
   
   wxObject *obj = event.GetEventObject();
   std::string selObjName = mObjectCheckListBox->GetStringSelection().c_str();
   
   if (obj == mOrbitColorCtrl)
   {
      mOrbitColor = mOrbitColorCtrl->GetColour();
      mOrbitColorMap[selObjName].Set(mOrbitColor.Red(),
                                     mOrbitColor.Green(),
                                     mOrbitColor.Blue(), 0);
      
      #ifdef DEBUG_PANEL_COLOR
      MessageInterface::ShowMessage("   Orbit color changed\n");
      MessageInterface::ShowMessage
         ("   red=%u, green=%u, blue=%u, alpha=%u\n", mOrbitColor.Red(),
          mOrbitColor.Green(), mOrbitColor.Blue(), mOrbitColor.Alpha());
      UnsignedInt intColor = mOrbitColorMap[selObjName].GetIntColor();
      MessageInterface::ShowMessage
         ("   mOrbitColorMap[%s]=%u<%08X>\n", selObjName.c_str(), intColor, intColor);
      #endif
      
      mHasOrbitColorChanged = true;
   }
   else if (obj == mTargetColorCtrl)
   {
      mTargetColor = mTargetColorCtrl->GetColour();
      mTargetColorMap[selObjName].Set(mTargetColor.Red(),
                                      mTargetColor.Green(),
                                      mTargetColor.Blue(), 0);
      
      #ifdef DEBUG_PANEL_COLOR
      MessageInterface::ShowMessage("   Target color changed\n");
      MessageInterface::ShowMessage
         ("   red=%u, green=%u, blue=%u, alpha=%u\n", mTargetColor.Red(),
          mTargetColor.Green(), mTargetColor.Blue(), mTargetColor.Alpha());
      UnsignedInt intColor = mTargetColorMap[selObjName].GetIntColor();
      MessageInterface::ShowMessage
         ("   mTargetColorMap[%s]=%u<%08X>\n", selObjName.c_str(), intColor, intColor);
      #endif
      
      mHasTargetColorChanged = true;
   }
   
   EnableUpdate(true);
   
   #ifdef DEBUG_PANEL_COLOR
   MessageInterface::ShowMessage("GroundTrackPlotPanel::OnColorPickerChange() entered\n");
   #endif
}
#endif


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnComboBoxChange(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mSolverIterComboBox)
   {
      mHasDataOptionChanged = true;
   }
   else if (event.GetEventObject() == mCentralBodyComboBox)
   {
      mHasCentralBodyChanged = true;
      wxString bodyTexture;
      if (mCentralBodyComboBox->GetValue() == mCentralBody)
      {
         bodyTexture = mTextureFile;
      }
      else
      {
         // Update texture map to corresponding body
         CelestialBody *body = (CelestialBody*)
            theGuiInterpreter->GetConfiguredObject(mCentralBodyComboBox->GetValue().c_str());
         Integer id = body->GetParameterID("TextureMapFileName");
         bodyTexture = body->GetStringParameter(id).c_str();
      }
      #ifdef DEBUG_CENTRAL_BODY
      MessageInterface::ShowMessage
         ("OnComboBoxChange(), bodyTexture = '%s'\n", bodyTexture.c_str());
      #endif
      mTextureMapTextCtrl->SetValue(bodyTexture);
      mTextureMapTextCtrl->SetInsertionPointEnd();
   }
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::OnTextChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (((wxTextCtrl*)obj)->IsModified())
   {
      if (obj == mDataCollectFreqTextCtrl ||
          obj == mUpdatePlotFreqTextCtrl  ||
          obj == mMaxPlottedDataPointsTextCtrl ||
          obj == mNumPointsToRedrawTextCtrl)
      {
         mHasIntegerDataChanged = true;
      }
      else if (obj == mTextureMapTextCtrl)
      {
         mHasTextureMapChanged = true;
      }
      
      EnableUpdate(true);
   }
}

//---------------------------------
// private methods
//---------------------------------

#ifdef __USE_COLOR_FROM_SUBSCRIBER__
//------------------------------------------------------------------------------
// void ShowSpacePointColor(const wxString &name, UnsignedInt color = GmatColor::RED)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::ShowSpacePointColor(const wxString &name, UnsignedInt color)
{
   #ifdef DEBUG_PANEL_COLOR
   MessageInterface::ShowMessage
      ("OrbitViewPanel::ShowSpacePointColor() name='%s'\n", name.c_str());
   #endif
   
   if (!name.IsSameAs(""))
   {
      std::string selObjName = name.c_str();
      
      // if object name not found, insert
      if (mOrbitColorMap.find(selObjName) == mOrbitColorMap.end())
      {
         #ifdef DEBUG_PANEL_COLOR
         MessageInterface::ShowMessage
            ("ShowSpacePointColor() name not found, so adding it to color map\n");
         #endif
         
         mOrbitColorMap[selObjName] = RgbColor(color);
         mTargetColorMap[selObjName] = RgbColor(GmatColor::ORANGE);
      }
      
      RgbColor orbColor = mOrbitColorMap[selObjName];
      RgbColor targColor = mTargetColorMap[selObjName];
      
      #ifdef DEBUG_PANEL_COLOR
      MessageInterface::ShowMessage
         ("OrbitViewPanel::ShowSpacePointColor() orbColor=%u, targColor=%u\n",
          orbColor.GetIntColor(), targColor.GetIntColor());
      #endif
      
      mOrbitColor.Set(orbColor.Red(), orbColor.Green(), orbColor.Blue());      
      mTargetColor.Set(targColor.Red(), targColor.Green(), targColor.Blue());
      
      mOrbitColorCtrl->SetColour(mOrbitColor);
      mTargetColorCtrl->SetColour(mTargetColor);
   }
}

//------------------------------------------------------------------------------
// void SaveObjectColors(const wxString &which,
//                       std::map<std::string, RgbColor> &colorMap)
//------------------------------------------------------------------------------
void GroundTrackPlotPanel::SaveObjectColors(const wxString &which,
                                            std::map<std::string, RgbColor> &colorMap)
{
   #if DEBUG_PANEL_SAVE
   MessageInterface::ShowMessage
      ("GroundTrackPlotPanel::SaveObjectColors() entered, which='%s'\n", which.c_str());
   #endif
   int count = mObjectCheckListBox->GetCount();
   std::string whichOne = which.c_str();
   
   for (int i = 0; i < count; i++)
   {
      if (mObjectCheckListBox->IsChecked(i))
      {
         std::string objName = mObjectCheckListBox->GetString(i).c_str();
         
         #if DEBUG_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   objName=%s, orbColor=%u\n", objName.c_str(),
             mOrbitColorMap[objName].GetIntColor());
         #endif
         
         mGroundTrackPlot->SetColor(whichOne, objName, colorMap[objName].GetIntColor());
      }
   }
   #if DEBUG_PANEL_SAVE
   MessageInterface::ShowMessage("GroundTrackPlotPanel::SaveObjectColors() leaving\n");
   #endif
}
#endif

