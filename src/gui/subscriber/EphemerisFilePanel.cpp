//$Id$
//------------------------------------------------------------------------------
//                           EphemerisFilePanel
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen
// Created: 2010/02/02
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#include "EphemerisFilePanel.hpp"
#include "EphemerisFile.hpp"       // for GetProperFileName()
#include "TimeSystemConverter.hpp"
#include "DateUtil.hpp"
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>

/// wxWidget event mappings for the panel
BEGIN_EVENT_TABLE(EphemerisFilePanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBOBOX, EphemerisFilePanel::OnComboBoxChange)
   EVT_TEXT(ID_COMBOBOX, EphemerisFilePanel::OnComboBoxTextChange)
   EVT_TEXT(ID_TEXTCTRL, EphemerisFilePanel::OnTextChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, EphemerisFilePanel::OnBrowse)
   EVT_CHECKBOX(ID_CHECKBOX, EphemerisFilePanel::OnCheck)
END_EVENT_TABLE()

//#define DEBUG_BUILD_CONTROL
//#define DEBUG_LOAD_DATA
//#define DEBUG_SAVE_DATA
//#define DEBUG_COMBOBOX

//-----------------------------------------
// public methods
//-----------------------------------------

//------------------------------------------------------------------------------
// EphemerisFilePanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Panel constructor
 * 
 * @param parent Owner for this panel
 * @param name Name of the object that is to be configured 
 */
//------------------------------------------------------------------------------
EphemerisFilePanel::EphemerisFilePanel(wxWindow *parent, const wxString &name)
   : GmatPanel                (parent),
     previousEpochFormat      (_T(""))
{
   clonedObj = NULL;
   fileDialog = NULL;
   code500ComboBox = NULL;
   onlyMj2000EqComboBox = NULL;
   
   mObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   if (mObject != NULL)
   {
      Create();
      Show();
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The object named \"%s\" does not exist\n",
               name.WX_TO_C_STRING);
   }
}


//------------------------------------------------------------------------------
// ~EphemerisFilePanel()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EphemerisFilePanel::~EphemerisFilePanel()
{
   // Unregister automatically registered ComboBoxes
   std::multimap<wxString, wxComboBox *>::iterator iter;
   for (iter = managedComboBoxMap.begin(); iter != managedComboBoxMap.end();
         ++iter)
      theGuiManager->UnregisterComboBox(iter->first, iter->second);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Inherited function that is called to create the panel.
 */
//------------------------------------------------------------------------------

void EphemerisFilePanel::Create()
{
   Integer bsize = 2;              // border size
   Integer id;  
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Ephemeris File"));
   
   // 1. Create Options box:
   wxStaticBoxSizer *optionsStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL,
         this, "Options");
   
   grid1 = new wxFlexGridSizer( 2, 0, 0 );
   
   id = mObject->GetParameterID("Spacecraft");
   wxStaticText * spacecraftStaticText =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Spacecraft"),
            wxDefaultPosition, wxDefaultSize, 0 );
   spacecraftComboBox = (wxComboBox*)BuildControl(this, id);
   spacecraftComboBox->SetToolTip(pConfig->Read(_T("SpacecraftHint")));
   grid1->Add(spacecraftStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add(spacecraftComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   
//   id = mObject->GetParameterID("StateType");
//   wxStaticText * stateTypeStaticText =
//      new wxStaticText(this, ID_TEXT, wxString("State T" GUI_ACCEL_KEY "ype"),
//            wxDefaultPosition, wxDefaultSize, 0 );
//   stateTypeComboBox = (wxComboBox*) BuildControl(this, id);
//   stateTypeComboBox->SetToolTip(pConfig->Read(_T("StateTypeHint")));
//   grid1->Add(stateTypeStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
//   grid1->Add(stateTypeComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   
   id = mObject->GetParameterID("CoordinateSystem");
   allCoordSystemStaticText =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Coordinate System"),
            wxDefaultPosition, wxDefaultSize, 0 );
   // BuildControl creates two coordinate systems, one showing all coordiante systems
   // another showing coordinate system with MJ2000Eq axis only
   allCoordSystemComboBox = (wxComboBox*) BuildControl(this, id);
   allCoordSystemComboBox->SetToolTip(pConfig->Read(
         _T("CoordinateSystemHint")));
   grid1->Add(allCoordSystemStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add(allCoordSystemComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   // Add code500ComboBox and hide it
   code500StaticText =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Coordinate System"),
      wxDefaultPosition, wxDefaultSize, 0);
   grid1->Add(code500StaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add(code500ComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Hide(code500StaticText);
   grid1->Hide(code500ComboBox);
   // Add onlyMj2000EqComboBox and hide it
   onlyMJ2000EqStaticText =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Coordinate System"),
            wxDefaultPosition, wxDefaultSize, 0 );
   grid1->Add(onlyMJ2000EqStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add(onlyMj2000EqComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   grid1->Hide(onlyMJ2000EqStaticText);
   grid1->Hide(onlyMj2000EqComboBox);

   id = mObject->GetParameterID("WriteEphemeris");
   writeEphemerisCheckBox = (wxCheckBox*) BuildControl(this, id);
   writeEphemerisCheckBox->SetToolTip(pConfig->Read(_T("WriteEphemerisHint")));
   grid1->Add(writeEphemerisCheckBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add(0, 0, wxALIGN_LEFT|wxALL, bsize );
   
   optionsStaticBoxSizer->Add( grid1, 0, wxALIGN_LEFT|wxALL, bsize );
   
   
   // 2. Create File Settings box:
   fileSettingsStaticBoxSizer =
      new wxStaticBoxSizer(wxHORIZONTAL, this, "File Settings");
   grid2 = new wxFlexGridSizer( 3, 0, 0 );
   
   id = mObject->GetParameterID("FileFormat");
   wxStaticText * fileFormatStaticText =
      new  wxStaticText(this, ID_TEXT, wxString("File For" GUI_ACCEL_KEY "mat"),
            wxDefaultPosition, wxDefaultSize, 0 );
   fileFormatComboBox = (wxComboBox*) BuildControl(this, id);
   fileFormatComboBox->SetToolTip(pConfig->Read(_T("FileFormatHint")));
   grid2->Add(fileFormatStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(fileFormatComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_CENTER|wxALL, bsize );
   
   id = mObject->GetParameterID("Filename");
   wxStaticText * fileNameStaticText =
      new  wxStaticText(this, ID_TEXT, wxString("File " GUI_ACCEL_KEY "Name"),
            wxDefaultPosition, wxDefaultSize, 0 );
   fileNameTextCtrl = (wxTextCtrl*) BuildControl(this, id);
   fileNameTextCtrl->SetToolTip(pConfig->Read(_T("FilenameHint")));
   browseButton =
      new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   browseButton->SetToolTip(pConfig->Read(_T("BrowseEphemerisFilenameHint")));
   grid2->Add(fileNameStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(fileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(browseButton, 0, wxALIGN_LEFT|wxALL, bsize );
   
   id = mObject->GetParameterID("Interpolator");
   wxStaticText * interpolatorStaticText =
      new  wxStaticText(this, ID_TEXT, wxString("Interpolato" GUI_ACCEL_KEY "r"),
            wxDefaultPosition, wxDefaultSize, 0 );
   interpolatorComboBox = (wxComboBox*) BuildControl(this, id);
   interpolatorComboBox->SetToolTip(pConfig->Read(_T("InterpolatorHint")));
   interpolatorComboBox->Enable(false);
   grid2->Add(interpolatorStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(interpolatorComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_CENTER|wxALL, bsize );
   
   id = mObject->GetParameterID("InterpolationOrder");
   wxStaticText * interpolationOrderStaticText =
      new  wxStaticText(this, ID_TEXT, wxString("Interpolation " GUI_ACCEL_KEY
            "Order"), wxDefaultPosition, wxDefaultSize, 0 );
   interpolationOrderTextCtrl = (wxTextCtrl*) BuildControl(this, id);
   interpolationOrderTextCtrl->SetToolTip(pConfig->Read(
         _T("InterpolationOrderHint")));
   grid2->Add(interpolationOrderStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(interpolationOrderTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_CENTER|wxALL, bsize );
   
   // StepSize
   id = mObject->GetParameterID("StepSize");
   allStepSizeStaticText =
      new wxStaticText(this, ID_TEXT, wxString("S" GUI_ACCEL_KEY "tep Size"),
                       wxDefaultPosition, wxDefaultSize, 0 );
   allStepSizeComboBox = (wxComboBox*) BuildControl(this, id);
   allStepSizeComboBox->SetToolTip(pConfig->Read(_T("StepSizeHint")));
   allStepSizeUnit =
      new wxStaticText(this, ID_TEXT, wxT("sec"),
                       wxDefaultPosition, wxDefaultSize, 0 );
   // StepSize for code 500 takes real numbers only, so create TextCtrl,
   // add to sizer and hide
   numericStepSizeStaticText =
      new wxStaticText(this, ID_TEXT, wxString("S" GUI_ACCEL_KEY "tep Size"),
                       wxDefaultPosition, wxDefaultSize, 0);
   numericStepSizeTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(180,-1),
                     0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   numericStepSizeUnit =
      new wxStaticText(this, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   grid2->Add(allStepSizeStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(allStepSizeComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(allStepSizeUnit, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(numericStepSizeStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(numericStepSizeTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(numericStepSizeUnit, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Hide(numericStepSizeStaticText);
   grid2->Hide(numericStepSizeTextCtrl);
   grid2->Hide(numericStepSizeUnit);
   
   // OutputFormat
   id = mObject->GetParameterID("OutputFormat");
   wxStaticText *outputFormatStaticText =
      new wxStaticText(this, ID_TEXT, wxString("O" GUI_ACCEL_KEY "utput Format"),
            wxDefaultPosition, wxDefaultSize, 0 );
   outputFormatComboBox = (wxComboBox*) BuildControl(this, id);
   outputFormatComboBox->SetToolTip(pConfig->Read(_T("OutputFormatHint")));
   grid2->Add(outputFormatStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(outputFormatComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_CENTER|wxALL, bsize );

   //DistanceUnit
   id = mObject->GetParameterID("DistanceUnit");
   distanceUnitStaticText =
      new wxStaticText(this, ID_TEXT, wxString("D" GUI_ACCEL_KEY "istance Unit"),
            wxDefaultPosition, wxDefaultSize, 0 );
   distanceUnitComboBox = (wxComboBox*) BuildControl(this, id);
   distanceUnitComboBox->SetToolTip(pConfig->Read(_T("OutputFormatHint")));
   grid2->Add(distanceUnitStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(distanceUnitComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_CENTER|wxALL, bsize );
   
   //includeEventBoundaries
   id = mObject->GetParameterID("IncludeEventBoundaries");
   eventBoundariesCheckBox = (wxCheckBox*) BuildControl(this, id);
   eventBoundariesCheckBox->SetToolTip(pConfig->Read(_T("EventBoundariesHint")));
   grid2->Add(eventBoundariesCheckBox, 0, wxALIGN_LEFT|wxALL, bsize );
   grid2->Add(0, 0, wxALIGN_LEFT|wxALL, bsize );

   fileSettingsStaticBoxSizer->Add( grid2, 0, wxALIGN_LEFT|wxALL, bsize );
   
   // 3. Create Epoch box:
   wxStaticBoxSizer *epochStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL,this, "Epoch");
   wxFlexGridSizer *grid3 = new wxFlexGridSizer( 2, 0, 0 );
   //grid3->AddGrowableCol(1);
   
   id = mObject->GetParameterID("EpochFormat");
   wxStaticText * epochFormatStaticText =
      new  wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Epoch Format"),
            wxDefaultPosition, wxDefaultSize, 0 );
   epochFormatComboBox = (wxComboBox*) BuildControl(this, id);
   epochFormatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));
   grid3->Add(epochFormatStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid3->Add(epochFormatComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   previousEpochFormat = epochFormatComboBox->GetValue();
   
   id = mObject->GetParameterID("InitialEpoch");
   wxStaticText * initialEpochStaticText =
      new  wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Initial Epoch"),
            wxDefaultPosition, wxDefaultSize, 0 );
   initialEpochComboBox = (wxComboBox*) BuildControl(this, id);
   initialEpochComboBox->SetToolTip(pConfig->Read(_T("InitialEpochHint")));
   grid3->Add(initialEpochStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid3->Add(initialEpochComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   
   id = mObject->GetParameterID("FinalEpoch");
   wxStaticText * finalEpochStaticText =
      new  wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Final Epoch"),
            wxDefaultPosition, wxDefaultSize, 0 );
   finalEpochComboBox = (wxComboBox*) BuildControl(this, id);
   finalEpochComboBox->SetToolTip(pConfig->Read(_T("FinalEpochHint")));
   grid3->Add(finalEpochStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid3->Add(finalEpochComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   
   epochStaticBoxSizer->Add( grid3, 0, wxALIGN_LEFT|wxALL, bsize );
   
   // 4. Create theMiddleSizer:
   theMiddleSizer->Add(optionsStaticBoxSizer, 0, wxALIGN_CENTRE|wxGROW, bsize);
   theMiddleSizer->Add(fileSettingsStaticBoxSizer, 0, wxALIGN_CENTRE|wxGROW, bsize);
   theMiddleSizer->Add(epochStaticBoxSizer, 0, wxALIGN_CENTRE|wxGROW, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Populates the panel with the configurable property data in the Solver 
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::LoadData()
{
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage("EphemerisFilePanel::LoadData() entered\n");
   #endif
   
   // load data from the core engine
   try
   {
      LoadControl("Spacecraft");
      LoadControl("StateType");
      LoadControl("CoordinateSystem");
      LoadControl("WriteEphemeris");
      LoadControl("FileFormat"); // Load file format before step size or output format
      LoadControl("Filename");
      LoadControl("Interpolator");
      LoadControl("InterpolationOrder");
      LoadControl("StepSize");
      LoadControl("OutputFormat");
      LoadControl("EpochFormat");
      LoadControl("InitialEpoch");
      LoadControl("FinalEpoch");
      LoadControl("DistanceUnit");
      LoadControl("IncludeEventBoundaries");
      
      // Show all or only MJ2000Eq coordinate system
      ShowCoordSystems(fileFormat);
      
      // Show or hide output format
      ShowCode500Items(fileFormat);
      
      // Show default interpolator; show or hide step size
      ShowInterpolatorAndStepSize(fileFormat);
      
      outputFormat = outputFormatComboBox->GetValue();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("EphemerisFilePanel:LoadData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
   }
   
   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage("EphemerisFilePanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Passes configuration data from the panel to the EphemerisFile object
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::SaveData()
{
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage("EphemerisFilePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   clonedObj = mObject->Clone();
   try
   {
      SaveControl("Spacecraft");
      SaveControl("StateType");
      SaveControl("CoordinateSystem");
      SaveControl("WriteEphemeris");
      SaveControl("FileFormat");
      SaveControl("Filename");
      SaveControl("Interpolator");
      SaveControl("InterpolationOrder");
      SaveControl("StepSize");
      SaveControl("OutputFormat");
      SaveControl("DistanceUnit");
      SaveControl("IncludeEventBoundaries");
      SaveControl("EpochFormat");
      try
      {
         SaveControl("InitialEpoch");
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
      
      try
      {
         SaveControl("FinalEpoch");
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   if (canClose)
      mObject->Copy(clonedObj);
   
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage("EphemerisFilePanel::SaveData() leaving\n");
   #endif

}


//------------------------------------------------------------------------------
// wxControl* BuildControl(wxWindow *parent, Integer index)
//------------------------------------------------------------------------------
/**
 * Builds a wxWidget control for an object property
 * 
 * @param parent The window that owns the control
 * @param index The index for the property that the constructed control
 *              represents
 * 
 * @return The new control
 */
//------------------------------------------------------------------------------
wxControl *EphemerisFilePanel::BuildControl(wxWindow *parent, Integer index)
{
   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage("EphemerisFilePanel::BuildControl() entered, index=%d\n", index);
   #endif
   
   wxControl *control = NULL;
   
   Gmat::ParameterType type = mObject->GetParameterType(index);
   
   switch (type)
   {
   case Gmat::BOOLEAN_TYPE:
      {
         wxCheckBox *cbControl;
         if (mObject->GetParameterText(index) == "WriteEphemeris")
            cbControl = new wxCheckBox(parent, ID_CHECKBOX,
                  GUI_ACCEL_KEY"Write Ephemeris");
         else if (mObject->GetParameterText(index) == "IncludeEventBoundaries")
            cbControl = new wxCheckBox(parent, ID_CHECKBOX,
                  GUI_ACCEL_KEY"Include Event Boundaries");
         else
            cbControl = new wxCheckBox(parent, ID_CHECKBOX,
                  (mObject->GetParameterText(index)).c_str());
         
         cbControl->SetValue(mObject->GetBooleanParameter(index));
         control = cbControl;
      }
      break;
   case Gmat::OBJECT_TYPE:
      {
         UnsignedInt type = mObject->GetPropertyObjectType(index);
         if (type == Gmat::SPACE_POINT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1), false);
            managedComboBoxMap.insert(std::make_pair("SpacePoint", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::CELESTIAL_BODY)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
            theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("CelestialBody",
                  cbControl));
            control = cbControl;
         }
         else if (type == Gmat::SPACECRAFT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("Spacecraft", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::COORDINATE_SYSTEM)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetCoordSystemComboBox(this, ID_COMBOBOX, wxSize(180,-1));
            // Create ComboBox showing only CS that Code-500 ephemeris files support
            if (code500ComboBox == NULL)
               code500ComboBox =
               theGuiManager->GetCoordSystemComboBox(this, ID_COMBOBOX, wxSize(180,-1), false, true);
            // Create ComboBox showing only CS with MJ2000Eq axis
            if (onlyMj2000EqComboBox == NULL)
               onlyMj2000EqComboBox =
                  theGuiManager->GetCoordSystemComboBox(this, ID_COMBOBOX, wxSize(180,-1), true);
            managedComboBoxMap.insert(std::make_pair("CoordinateSystem", cbControl));
            managedComboBoxMap.insert(std::make_pair("CoordinateSystem", onlyMj2000EqComboBox));
            onlyMj2000EqComboBox->SetName("_MJ2000EqOnly_");
            managedComboBoxMap.insert(std::make_pair("CoordinateSystem", code500ComboBox));
            code500ComboBox->SetName("_Code500Only_");
            #ifdef DEBUG_BUILD_CONTROL
            MessageInterface::ShowMessage
               ("cbControl->GetName()='%s', onlyMj2000EqComboBox->GetName()='%s, code500ComboBox->GetName()='%s'\n",
                cbControl->GetName().c_str(), onlyMj2000EqComboBox->GetName().c_str(), code500ComboBox->GetName().c_str());
            #endif
            control = cbControl;
         }
         else
         {
            // Check if enumeration strings available for owned object types
            wxArrayString enumList;
            StringArray enumStrings = mObject->GetPropertyEnumStrings(index);
            for (UnsignedInt i=0; i<enumStrings.size(); i++)
               enumList.Add(enumStrings[i].c_str());
            
            control = new wxComboBox(parent, ID_COMBOBOX, wxT(""),
                                     wxDefaultPosition, wxSize(180,-1),
                                     enumList, wxCB_READONLY);
         }
      }
      break;
   case Gmat::ENUMERATION_TYPE:
      {
         StringArray enumStrings = mObject->GetPropertyEnumStrings(index);
         wxArrayString enumList;
         for (UnsignedInt i=0; i<enumStrings.size(); i++)
            enumList.Add(enumStrings[i].c_str());
         
         wxComboBox *cbControl = NULL;
         if (enumStrings.size() == 1)
         {
            // Show the value even if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(180,-1), enumList, 0);
         }
         else
         {
            // Do not show the value if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(180,-1), enumList, wxCB_READONLY);
         }
         
         control = cbControl;
      }
      break;
      
   case Gmat::INTEGER_TYPE:
   case Gmat::REAL_TYPE:      
      control = new wxTextCtrl(parent, ID_TEXTCTRL, 
                               wxT(""), wxDefaultPosition, wxSize(180,-1), 0,
                               wxTextValidator(wxGMAT_FILTER_NUMERIC));
      break;
      
   case Gmat::STRING_TYPE:
   default:
      control = new wxTextCtrl(parent, ID_TEXTCTRL, 
                               wxT(""), wxDefaultPosition, wxSize(180,-1));
      break;
   }
   
   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::BuildControl() returning, <%p>'%s' for index=%d\n",
       control, control->GetName().c_str(), index);
   #endif
   
   return control;
}


//------------------------------------------------------------------------------
// void LoadControl(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Sets the data for a control
 * 
 * @param label The control's label
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::LoadControl(const std::string &label)
{
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::LoadControl() entered, label='%s'\n", label.c_str());
   #endif
   
   wxString valueString;
   bool valueBool;
   Integer valueInteger;
   
   if (label == "Spacecraft")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      spacecraftComboBox->SetValue(valueString);
   }
   else if (label == "CoordinateSystem")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());

      #ifdef DEBUG_LOAD_DATA
      MessageInterface::ShowMessage
         ("   Setting CoordinateSystem CB to '%s'\n", valueString.c_str());
      #endif
      
      allCoordSystemComboBox->SetValue(valueString);
      // keep 3 comboboxes in sync
      if (onlyMj2000EqComboBox->FindString(allCoordSystemComboBox->GetValue()) != wxNOT_FOUND)
         onlyMj2000EqComboBox->SetValue(allCoordSystemComboBox->GetValue());
      if (code500ComboBox->FindString(allCoordSystemComboBox->GetValue()) != wxNOT_FOUND)
         code500ComboBox->SetValue(allCoordSystemComboBox->GetValue());
   }
   else if (label == "WriteEphemeris")
   {
      valueBool = mObject->GetBooleanParameter(mObject->GetParameterID(label));
      writeEphemerisCheckBox->SetValue(valueBool);
   }
   else if (label == "FileFormat")
   {
      fileFormat = (mObject->GetStringParameter(label)).c_str();
      valueString = fileFormat.c_str();
      fileFormatComboBox->SetValue(valueString);
      
      // Set interpolator based on the format, per bug 2219
      valueString = wxString(mObject->GetStringParameter("Interpolator").c_str());
      interpolatorComboBox->SetValue(valueString);
      // Use the order from the object as well
      valueInteger = mObject->GetIntegerParameter("InterpolationOrder");
      interpolationOrderTextCtrl->SetValue(wxString::Format("%d",valueInteger));

      if (fileFormat == "STK-TimePosVel")
      {
         distanceUnitStaticText->Enable();
         distanceUnitComboBox->Enable();
         eventBoundariesCheckBox->Enable();
      }
      else
      {
         distanceUnitStaticText->Disable();
         distanceUnitComboBox->Disable();
         eventBoundariesCheckBox->Disable();
      }
   }
   else if (label == "Filename")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      fileNameTextCtrl->SetValue(valueString);
   }
   else if (label == "InterpolationOrder")
   {
      valueInteger = mObject->GetIntegerParameter(label.c_str());
      interpolationOrderTextCtrl->SetValue(wxString::Format("%d",valueInteger));
   }
   else if (label == "StepSize")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      Real stepSizeReal = ToReal(valueString);
      if (stepSizeReal != 0.0 && stepSizeReal != GmatRealConstants::REAL_UNDEFINED)
         numericStepSizeTextCtrl->SetValue(valueString);
      else
         numericStepSizeTextCtrl->SetValue("60"); // Set 60 as default
      allStepSizeComboBox->SetValue(valueString);
   }
   else if (label == "OutputFormat")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      outputFormatComboBox->SetValue(valueString);
   }
   else if (label == "EpochFormat")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      epochFormatComboBox->SetValue(valueString);
      previousEpochFormat = epochFormatComboBox->GetValue();
   }
   else if (label == "InitialEpoch")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      initialEpochComboBox->SetValue(valueString);
   }
   else if (label == "FinalEpoch")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      finalEpochComboBox->SetValue(valueString);
   }
   else if (label == "DistanceUnit")
   {
      valueString = wxString(mObject->GetStringParameter(label).c_str());
      distanceUnitComboBox->SetValue(valueString);
   }
   else if (label == "IncludeEventBoundaries")
   {
      valueBool = mObject->GetBooleanParameter(mObject->GetParameterID(label));
      eventBoundariesCheckBox->SetValue(valueBool);
   }
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::LoadControl() leaving, label='%s'\n", label.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SaveControl(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Passes a control's data to the EphemerisFile.
 * 
 * @param label The string associated with the control.
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::SaveControl(const std::string &label)
{
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::SaveControl() entered, label='%s'\n", label.c_str());
   #endif
   
   std::string valueString;
   bool valueBool;
   Integer valueInteger;
   Real stepSize;
   
   Integer paramId = clonedObj->GetParameterID(label);
   
   if (label == "Spacecraft")
   {
      valueString = spacecraftComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   // Only allowed StateType is Cartesian for the 2010 release, per bug 2219
//   else if (label == "StateType")
//   {
//      valueString = stateTypeComboBox->GetValue();
//      clonedObj->SetStringParameter(paramId, valueString);
//   }
   else if (label == "CoordinateSystem")
   {
      // Added check for fileFormat since only coordinate system with MJ2000Eq axis
      // should be shown for format other than CCSDS-OEM (for GMT-4452 fix - LOJ: 2014.04.01)
      if (fileFormat == "CCSDS-OEM" || fileFormat == "STK-TimePosVel")
         valueString = allCoordSystemComboBox->GetValue();
      else if (fileFormat == "Code-500")
         valueString = code500ComboBox->GetValue();
      else
         valueString = onlyMj2000EqComboBox->GetValue();
      #ifdef DEBUG_SAVE_DATA
      MessageInterface::ShowMessage
         ("   Setting CoordinateSystem '%s' to object\n", valueString.c_str());
      #endif
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "WriteEphemeris")
   {
      valueBool =  writeEphemerisCheckBox->GetValue();
      clonedObj->SetBooleanParameter(paramId, valueBool);
   }
   else if (label == "FileFormat")
   {
      valueString = fileFormatComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "Filename")
   {
      valueString = fileNameTextCtrl->GetValue();
      if (CheckFileName(valueString, "Filename"))
         clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "InterpolationOrder")
   {
      valueString = interpolationOrderTextCtrl->GetValue().c_str();
      if (CheckInteger(valueInteger, valueString, "InterpolationOrder",
                       "Integer Number > 0.0", false, false, true, false))
         clonedObj->SetIntegerParameter(paramId, valueInteger);
   }
   else if (label == "StepSize")
   {
      if (fileFormat == "Code-500")
      {
         valueString = numericStepSizeTextCtrl->GetValue();
         clonedObj->SetStringParameter(paramId, valueString);
      }
      else
      {
         valueString = allStepSizeComboBox->GetValue();
         if ((valueString == "IntegratorSteps") ||
             (CheckReal(stepSize, valueString, "StepSize", "Real Number > 0.0 or "
                        "equals 'IntegratorSteps'", false, true, true, false)))
            clonedObj->SetStringParameter(paramId, valueString);
      }
   }
   else if (label == "OutputFormat")
   {
      valueString = outputFormatComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "EpochFormat")
   {
      valueString = epochFormatComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "InitialEpoch")
   {
      valueString = initialEpochComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "FinalEpoch")
   {
      valueString = finalEpochComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "DistanceUnit")
   {
      valueString = distanceUnitComboBox->GetValue();
      clonedObj->SetStringParameter(paramId, valueString);
   }
   else if (label == "IncludeEventBoundaries")
   {
      valueBool =  eventBoundariesCheckBox->GetValue();
      clonedObj->SetBooleanParameter(paramId, valueBool);
   }
   
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::SaveControl() leaving, label='%s'\n", label.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for comboboxes
 * 
 * Activates the Apply button when selection is changed.
 * 
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::OnComboBoxChange(wxCommandEvent& event)
{
   // Make the file type and interpolator compatible
   if (event.GetEventObject() == fileFormatComboBox)
   {
      wxString newFileFormat = fileFormatComboBox->GetValue();
      #ifdef DEBUG_COMBOBOX
      MessageInterface::ShowMessage
         ("fileFormat=%s, newFileFormat=%s\n", fileFormat.WX_TO_C_STRING,
          newFileFormat.WX_TO_C_STRING);
      #endif
      if (fileFormat != newFileFormat)
      {
         fileFormat = newFileFormat;
         
         // Show proper file extension
         std::string fileName = fileNameTextCtrl->GetValue().WX_TO_STD_STRING;
         std::string stdFileFormat = fileFormat.WX_TO_STD_STRING;
         std::string properFileName =
            ((EphemerisFile*)mObject)->GetProperFileName(fileName, stdFileFormat, false);
         #ifdef DEBUG_COMBOBOX
         MessageInterface::ShowMessage
            ("fileName='%s', stdFileFormat='%s', properFileName='%s'\n", fileName.c_str(),
             stdFileFormat.c_str(), properFileName.c_str());
         #endif
         fileNameTextCtrl->SetValue(wxString(properFileName));
         
         // Show proper coordinate systems based on the format
         ShowCoordSystems(fileFormat);
         
         // Show proper fields for Code500 format
         ShowCode500Items(fileFormat);
         
         // Show default interpolator; show or hide step size
         ShowInterpolatorAndStepSize(fileFormat);
         
         if (newFileFormat == "STK-TimePosVel")
         {
            distanceUnitStaticText->Enable();
            distanceUnitComboBox->Enable();
            eventBoundariesCheckBox->Enable();
         }
         else
         {
            distanceUnitStaticText->Disable();
            distanceUnitComboBox->Disable();
            eventBoundariesCheckBox->Disable();
         }

         EnableUpdate(true);
      }
   }
   else if (event.GetEventObject() == outputFormatComboBox)
   {
      wxString newOutputFormat = outputFormatComboBox->GetValue();
      #ifdef DEBUG_COMBOBOX
      MessageInterface::ShowMessage
         ("outputFormat=%s, newOutputFormat=%s\n", outputFormat.c_str(), newOutputFormat.c_str());
      #endif
      if (outputFormat != newOutputFormat)
      {
         outputFormat = newOutputFormat;
         EnableUpdate(true);
      }
   }
   else if (event.GetEventObject() == distanceUnitComboBox)
   {
      wxString newDistanceUnit = distanceUnitComboBox->GetValue();
      #ifdef DEBUG_COMBOBOX
      MessageInterface::ShowMessage
         ("distanceUnit=%s, newDistanceUnit=%s\n", distanceUnit.c_str(), newDistanceUnit.c_str())
      #endif
      if (distanceUnit != newDistanceUnit)
      {
         distanceUnit = newDistanceUnit;
         EnableUpdate(true);
      }
   }
   // Update the epoch data fields if the format changes and they are times
   else if (event.GetEventObject() == epochFormatComboBox)
   {
      #ifdef DEBUG_TIME_CONVERSIONS
         MessageInterface::ShowMessage("Epoch format changed from '%s' to "
               "'%s'\n", previousEpochFormat.c_str(),
               epochFormatComboBox->GetValue().c_str());
      #endif
      if (previousEpochFormat == _T(""))
         return;
      
      wxString shownEpoch;
      bool epochError = false;
      std::string oldEpoch, newInitialEpoch, epFormat, newFinalEpoch;
      Real fromVal;
      Real toVal = -999.999;
      epFormat = epochFormatComboBox->GetValue().c_str();

      // Start epoch
      shownEpoch = initialEpochComboBox->GetValue();

      #ifdef DEBUG_TIME_CONVERSIONS
         MessageInterface::ShowMessage("   Displayed initial epoch is %s\n",
               shownEpoch.c_str());
      #endif

      Integer id = mObject->GetParameterID("InitialEpoch");
      StringArray enumStrings = mObject->GetPropertyEnumStrings(id);
      if (find(enumStrings.begin(), enumStrings.end(), shownEpoch.c_str()) ==
            enumStrings.end())
      {
         // Not in the list, so must be an epoch string
         oldEpoch = shownEpoch.c_str();

         try
         {
            std::string prevFmt = previousEpochFormat.WX_TO_STD_STRING;
            TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt,oldEpoch, true);

            if (previousEpochFormat.Find("ModJulian") == wxNOT_FOUND)
               fromVal = -999.999;
            else
            {
               shownEpoch.ToDouble(&fromVal);
            }

            TimeSystemConverter::Instance()->Convert(previousEpochFormat.c_str(), fromVal,
                  oldEpoch, epFormat, toVal, newInitialEpoch);

         }
         catch (BaseException &be)
         {
            epochFormatComboBox->SetValue(previousEpochFormat.c_str());
            epochError = true;
            MessageInterface::PopupMessage
               (Gmat::ERROR_, be.GetFullMessage() +
                "\nPlease enter valid Initial Epoch before changing the Epoch Format\n");
         }
      }
      else
         newInitialEpoch = shownEpoch.c_str();

      // End epoch
      shownEpoch = finalEpochComboBox->GetValue();

      #ifdef DEBUG_TIME_CONVERSIONS
         MessageInterface::ShowMessage("   Displayed final epoch is %s\n",
               shownEpoch.c_str());
      #endif

      id = mObject->GetParameterID("FinalEpoch");
      enumStrings = mObject->GetPropertyEnumStrings(id);
      if (find(enumStrings.begin(), enumStrings.end(), shownEpoch.c_str()) ==
            enumStrings.end())
      {
         // Not in the list, so must be an epoch string
         oldEpoch = shownEpoch.c_str();

         try
         {
            std::string prevFmt = previousEpochFormat.WX_TO_STD_STRING;
            TimeSystemConverter::Instance()->ValidateTimeFormat(prevFmt,oldEpoch, true);

            if (previousEpochFormat.Find("ModJulian") == wxNOT_FOUND)
            {
               fromVal = -999.999;
            }
            else
            {
               shownEpoch.ToDouble(&fromVal);
            }

            TimeSystemConverter::Instance()->Convert(previousEpochFormat.c_str(), fromVal,
                  oldEpoch, epFormat, toVal, newFinalEpoch);

         }
         catch (BaseException &be)
         {
            epochFormatComboBox->SetValue(previousEpochFormat.c_str());
            epochError = true;
            MessageInterface::PopupMessage
               (Gmat::ERROR_, be.GetFullMessage() +
                "\nPlease enter valid Final Epoch before changing the Epoch Format\n");
         }
       }
      else
         newFinalEpoch = shownEpoch.c_str();
      
      if (!epochError)
      {
         previousEpochFormat = epochFormatComboBox->GetValue();
         initialEpochComboBox->SetValue(newInitialEpoch.c_str());
         finalEpochComboBox->SetValue(newFinalEpoch.c_str());
      }
      
      EnableUpdate(true);
   }
   // keep allCoordSystemComboBox, code500ComboBox, and onlyMj2000EqComboBox in sync if possible 
   else if (event.GetEventObject() == allCoordSystemComboBox)
   {
      if (code500ComboBox->FindString(allCoordSystemComboBox->GetValue()) != wxNOT_FOUND)
         code500ComboBox->SetValue(allCoordSystemComboBox->GetValue());
      if (onlyMj2000EqComboBox->FindString(allCoordSystemComboBox->GetValue()) != wxNOT_FOUND)
         onlyMj2000EqComboBox->SetValue(allCoordSystemComboBox->GetValue());
   }
   else if (event.GetEventObject() == code500ComboBox)
   {
      if (allCoordSystemComboBox->FindString(code500ComboBox->GetValue()) != wxNOT_FOUND)
         allCoordSystemComboBox->SetValue(code500ComboBox->GetValue());
      if (onlyMj2000EqComboBox->FindString(code500ComboBox->GetValue()) != wxNOT_FOUND)
         onlyMj2000EqComboBox->SetValue(code500ComboBox->GetValue());
   }
   else if (event.GetEventObject() == onlyMj2000EqComboBox)
   {
      if (allCoordSystemComboBox->FindString(onlyMj2000EqComboBox->GetValue()) != wxNOT_FOUND)
         allCoordSystemComboBox->SetValue(onlyMj2000EqComboBox->GetValue());
      if (code500ComboBox->FindString(onlyMj2000EqComboBox->GetValue()) != wxNOT_FOUND)
         code500ComboBox->SetValue(onlyMj2000EqComboBox->GetValue());
   }
   
   //if (theApplyButton != NULL)
   //   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboBoxTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for ComboBox text change
 * 
 * Activates the Apply button when selection is changed.
 * 
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::OnComboBoxTextChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for text boxes
 * 
 * Activates the Apply button when text is changed.
 * 
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void EphemerisFilePanel::OnTextChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnWriteEphemerisChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void EphemerisFilePanel::OnCheck(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnBrowse(wxCommandEvent &event)
// This function is used to open file dialog.
//------------------------------------------------------------------------------
void EphemerisFilePanel::OnBrowse(wxCommandEvent &event)
{
   if (fileDialog == NULL)
   {
      // create fileDialog object when it does not exist
      wxString caption = wxT("Choose a File");
      wxString defaultDir = wxT("./output/");
      wxString defaultFile = wxEmptyString;
      wxString wildcard = wxT("*.*");
      
      fileDialog = new wxFileDialog(this, caption, 
                                    defaultDir, defaultFile, 
                                    wildcard,
                                    gmatFD_OPEN);
   }
   else
   {
      // show fileDialog when it exists 
      fileDialog->Show();
   }
   
   if (fileDialog->ShowModal() == wxID_OK)
   {
      // change file name when a new file is chosen
      wxString filename = fileDialog->GetPath().c_str();
      fileNameTextCtrl->SetValue(filename);
   }
   else
   {
      // only hide fileDialog when click on Cancel button
      fileDialog->Hide();
   }
}


//------------------------------------------------------------------------------
// void ShowCoordSystems(const wxString &fileType)
//------------------------------------------------------------------------------
void EphemerisFilePanel::ShowCoordSystems(const wxString &fileType)
{
   bool showAll = (fileType == "CCSDS-OEM" || fileType == "STK-TimePosVel");
   bool show500 = (fileType == "Code-500");
   bool showOther = !showAll && !show500;
   
   #ifdef DEBUG_CS
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::ShowCoordSystems() entered, showAll=%d, fileType='%s'\n",
       showAll, fileType.c_str());
   #endif
   
   grid1->Show(allCoordSystemStaticText, showAll);
   grid1->Show(allCoordSystemComboBox, showAll);
   grid1->Show(code500StaticText, show500);
   grid1->Show(code500ComboBox, show500);
   grid1->Show(onlyMJ2000EqStaticText, showOther);
   grid1->Show(onlyMj2000EqComboBox, showOther);
   
   // Commented out for GMT-4452 fix (LOJ: 2014.04.01)
   #if 0
   if (allCoordSystemComboBox->GetValue() != onlyMj2000EqComboBox->GetValue())
   {
	   // if changing to allCoordSystemComboBox or onlyMj2000 does not have value
	   if (show || (onlyMj2000EqComboBox->FindString(allCoordSystemComboBox->GetValue()) == wxNOT_FOUND))
		   allCoordSystemComboBox->SetValue( onlyMj2000EqComboBox->GetValue() );
	   else 
		   onlyMj2000EqComboBox->SetValue( allCoordSystemComboBox->GetValue() );
   }
   #endif
   
   grid1->Layout();
   theMiddleSizer->Layout();
   
   #ifdef DEBUG_CS
   MessageInterface::ShowMessage
      ("EphemerisFilePanel::ShowCoordSystems() leaving, showAll=%d, fileFormat='%s'\n",
       show, fileFormat.c_str());
   #endif
}

//------------------------------------------------------------------------------
// void ShowCode500Items(const wxString &fileType)
//------------------------------------------------------------------------------
void EphemerisFilePanel::ShowCode500Items(const wxString &fileType)
{
   bool show = false;
   if (fileType == "Code-500")
      show = true;
   
   // Show or hide all step size
   // Show or hide numeric only step size
   grid2->Show(allStepSizeStaticText, !show);
   grid2->Show(allStepSizeComboBox, !show);
   grid2->Show(allStepSizeUnit, !show);
   grid2->Show(numericStepSizeStaticText, show);
   grid2->Show(numericStepSizeTextCtrl, show);
   grid2->Show(numericStepSizeUnit, show);
   grid2->Layout();
   
   // If not showing code 500, set output format to "LittleEndian"
   if (!show)
      outputFormatComboBox->SetValue("LittleEndian");
   
   // Enable or disable output format
   outputFormatComboBox->Enable(show);
   theMiddleSizer->Layout();
}

//------------------------------------------------------------------------------
// void ShowInterpolatorAndStepSize(const wxString &fileType)
//------------------------------------------------------------------------------
void EphemerisFilePanel::ShowInterpolatorAndStepSize(const wxString &fileType)
{
   if (fileType == "SPK")
   {
      interpolatorComboBox->SetValue("Hermite");
      allStepSizeComboBox->Enable(false);
   }
   else if (fileType == "CCSDS-OEM" || fileType == "Code-500" ||
            fileType == "STK-TimePosVel")
   {
      interpolatorComboBox->SetValue("Lagrange");
      allStepSizeComboBox->Enable(true);
   }
}

