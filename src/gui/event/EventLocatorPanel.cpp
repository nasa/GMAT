//$Id$
//------------------------------------------------------------------------------
//                            EventLocatorPanel
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
// Author: Wendy Shoan
// Created: 2015.04.01
/**
 * This class sets up Event Locator parameters.
 */
//------------------------------------------------------------------------------
#include "EventLocatorPanel.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "TimeSystemConverter.hpp"
#include "GmatAppData.hpp"
#include "EventLocator.hpp"
#include "GmatDefaults.hpp"
#include <wx/config.h>   // ??
#include "bitmaps/OpenFolder.xpm"

//#define DEBUG_EVENTPANEL_CREATE
//#define DEBUG_EVENTPANEL_LOAD
//#define DEBUG_EVENTPANEL_SAVE
//#define DEBUG_EVENTPANEL_SAVE_COEFS
//#define DEBUG_EVENTPANEL_PANEL_COMBOBOX
//#define DEBUG_ECLIPSE_OBSERVERS

//------------------------------
// event tables for wxWidgets
//------------------------------
BEGIN_EVENT_TABLE(EventLocatorPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, EventLocatorPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBOBOX, EventLocatorPanel::OnComboBoxChange)
   EVT_COMBOBOX(ID_RUNMODE_COMBOBOX, EventLocatorPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, EventLocatorPanel::OnBrowseButton)
   EVT_CHECKBOX(ID_CHECKBOX, EventLocatorPanel::OnCheckBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX_BODIES, EventLocatorPanel::OnCheckListBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX_ECLIPSE, EventLocatorPanel::OnCheckListBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX_OBSERVER, EventLocatorPanel::OnCheckListBoxChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX_IMAGERS, EventLocatorPanel::OnCheckListBoxChange)
   EVT_LISTBOX(ID_CHECKLISTBOX_BODIES, EventLocatorPanel::OnCheckListBoxSelect)
   EVT_LISTBOX(ID_CHECKLISTBOX_ECLIPSE, EventLocatorPanel::OnCheckListBoxSelect)
   EVT_LISTBOX(ID_CHECKLISTBOX_OBSERVER, EventLocatorPanel::OnCheckListBoxSelect)
   EVT_LISTBOX(ID_CHECKLISTBOX_IMAGERS, EventLocatorPanel::OnCheckListBoxSelect)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// EventLocatorPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs EventLocatorPanel object.
 */
//------------------------------------------------------------------------------
EventLocatorPanel::EventLocatorPanel(wxWindow *parent,
                                     const wxString &name)
   : GmatPanel(parent, true, true)
{
   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("Entering ELP::Constructor, name = %s\n",
                                    name.WX_TO_STD_STRING.c_str());
   #endif
   mObjectName = name.c_str();
   theObject   = theGuiInterpreter->GetConfiguredObject(name.c_str());

   #ifdef DEBUG_EVENTPANEL_CREATE
      if (!theObject)
         MessageInterface::ShowMessage("In ELP::Constructor, theObject is NULL!!!\n");
   #endif

   if (theObject->IsOfType("ContactLocator"))
      locatorType = CONTACT;
   else if (theObject->IsOfType("EclipseLocator"))
      locatorType = ECLIPSE;
   else if (theObject->IsOfType("IntrusionLocator"))
      locatorType = INTRUSION;
   else
      locatorType = UNKNOWN;

   #ifdef DEBUG_EVENTPANEL_CREATE
   MessageInterface::ShowMessage
      ("EventLocatorPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif

   ResetChangedFlags();

//   theGuiManager       = GuiItemManager::GetInstance();
//   theGuiInterpreter   = GmatAppData::Instance()->GetGuiInterpreter();
   ss                  = theGuiInterpreter->GetSolarSystemInUse();

   // To set panel object and show warning if object is NULL
   if (SetObject(theObject))
   {
      Create();
      Show();
   }

}


//------------------------------------------------------------------------------
// ~EventLocatorPanel()
//------------------------------------------------------------------------------
EventLocatorPanel::~EventLocatorPanel()
{
   if (localObject != NULL)
	 delete localObject;

   theGuiManager->UnregisterCheckListBox("SpacePoint", bodiesCheckListBox);
   theGuiManager->UnregisterComboBox("Spacecraft", scTargetComboBox);
   if (locatorType == CONTACT)
      theGuiManager->UnregisterCheckListBox("GroundStation", observersCheckListBox);
   else if (locatorType == INTRUSION)
      theGuiManager->UnregisterCheckListBox("Imager", imagersCheckListBox);
}

//-------------------------------
// private methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void EventLocatorPanel::Create()
{
   #ifdef DEBUG_EVENTPANEL_CREATE
   MessageInterface::ShowMessage("EventLocatorPanel::Create() entered\n");
   #endif

   localObject = NULL;

   bool centeredCheckboxes = false;  // set to true for checkboxes centered horizontally

   Integer staticTextWidth = 110;

   if (centeredCheckboxes)
      staticTextWidth = 140;

   Integer staticTextLarger = staticTextWidth + 12;

   Integer bsize = 2; // border size
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Event Locator"));

   wxArrayString emptyList;

   int epochWidth  = 170;
   int buttonWidth = 25;
   #ifdef __WXMAC__
      epochWidth = 178;
      buttonWidth = 40;
   #endif

   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   //-----------------------------------------------------------------
   //  create sizers
   //-----------------------------------------------------------------
   wxBoxSizer         *eventSizer            = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer    *leftGridSizer         = new wxFlexGridSizer( 3, 0, 0 );
//   wxFlexGridSizer    *rightGridSizer        = new wxFlexGridSizer( 1, 0, 0 );
   wxBoxSizer         *leftSizer             = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer         *rightSizer            = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer    *upperRightGridSizer   = new wxFlexGridSizer( 3, 0, 0 );
   wxFlexGridSizer    *lowerRightGridSizer   = new wxFlexGridSizer( 3, 0, 0 );
   GmatStaticBoxSizer *sizerUpperRight       = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   GmatStaticBoxSizer *sizerLowerRight       = new GmatStaticBoxSizer( wxVERTICAL, this, "" );

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() sizers created ...\n");
   #endif
   //-----------------------------------------------------------------
   // Spacecraft/Target
   //-----------------------------------------------------------------
   // label for spacecraft/target
   if (locatorType == CONTACT)
      scTargetTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Target", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   else
      scTargetTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Spacecraft", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );

   scTargetComboBox = theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX,
                                                           wxSize(150,-1));
   scTargetComboBox->SetToolTip(pConfig->Read(_T("SpacecraftOrTargetHint")));

   //-----------------------------------------------------------------
   // Intruding/Occulting Bodies
   //-----------------------------------------------------------------
   if (locatorType != INTRUSION)
   {
      bodiesTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Occulting Bodies", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      bodiesCheckListBox =
         theGuiManager->GetSpacePointCheckListBox(this, ID_CHECKLISTBOX_BODIES, wxSize(200, -1),
            true, false, false, false);
   }
   else
   {
      bodiesTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Intruding Bodies", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      bodiesCheckListBox =
         theGuiManager->GetSpacePointCheckListBox(this, ID_CHECKLISTBOX_BODIES, wxSize(200, -1),
            true, false, true, false);

      // Remove spacecraft holding the imagers
      for (unsigned int ii = 0; ii < bodiesCheckListBox->GetCount(); ++ii)
      {
         if (bodiesCheckListBox->GetString(ii) == theObject->GetStringParameter("Spacecraft"))
         {
            bodiesCheckListBox->Delete(ii);
            break;
         }
      }
   }

   //-----------------------------------------------------------------
   // Central Body
   //-----------------------------------------------------------------
   if (locatorType == INTRUSION)
   {
      cenBodyTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Central Body", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      cenBodyComboBox = theGuiManager->GetCelestialPointComboBox(this, ID_COMBOBOX,
         wxSize(150, -1));
   }

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() spacecraft and bodies widgets created ...\n");
   #endif

   //-----------------------------------------------------------------
   // Eclipse/Observers/Imagers
   //-----------------------------------------------------------------
   #ifdef DEBUG_ECLIPSE_OBSERVERS
      MessageInterface::ShowMessage("-- About to create eclipseTypesCLB OR observersCLB and isEclipse = %s\n",
            (isEclipse? "true":"false"));
   #endif

   switch (locatorType)
   {
   case CONTACT:
      observersTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Observers", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      observersCheckListBox =
         theGuiManager->GetGroundStationCheckListBox(this, ID_CHECKLISTBOX_OBSERVER, wxSize(200, -1));
      //        theGuiManager->GetSpacePointCheckListBox(this, ID_CHECKLISTBOX, wxSize(200,100),
      //              false, false, false, true);
      #ifdef DEBUG_EVENTPANEL_CREATE
               MessageInterface::ShowMessage("EventLocatorPanel::Create() observers widgets created!!!!! ...\n");
      #endif
      break;

   case ECLIPSE:
      eclipseTypesTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Eclipse Types", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      eclipseTypesCheckListBox = new wxCheckListBox(this, ID_CHECKLISTBOX_ECLIPSE, wxDefaultPosition,
         wxSize(200, -1), emptyList, wxLB_SINGLE | wxLB_SORT | wxLB_HSCROLL);
      break;

   case INTRUSION:
      imagersTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Sensors", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      imagersCheckListBox = theGuiManager->GetImagerCheckListBox(this, ID_CHECKLISTBOX_IMAGERS, wxSize(200, -1));
      break;

   default:
      break;
   }

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() eclipse/observers widgets created ...\n");
   #endif

   //-----------------------------------------------------------------
   // Filename/Report
   //-----------------------------------------------------------------
   fileNameTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Filename", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   fileNameTxtCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(200, -1), 0);
   fileNameBrowseButton =
      new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));

   runModeTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Run Mode", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );

   runModeComboBox = new wxComboBox
      ( this, ID_RUNMODE_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
            emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   runModeComboBox->SetToolTip(pConfig->Read(_T("RunMode")));

   writeReportCheckBox = new wxCheckBox(this, ID_CHECKBOX, wxT("Write Report"),
                                  wxDefaultPosition, wxDefaultSize, 0);

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() report widgets created ...\n");
   #endif

   //-----------------------------------------------------------------
   // Epoch
   //-----------------------------------------------------------------
   // check box
   entireIntervalCheckBox = new wxCheckBox(this, ID_CHECKBOX, wxT("Use Entire Interval"),
                                  wxDefaultPosition, wxDefaultSize, 0);

   // label for epoch format
   epochFormatTxt = new wxStaticText( this, ID_TEXT,
      "Epoch " GUI_ACCEL_KEY "Format", wxDefaultPosition, wxSize(staticTextLarger,-1), 0 );

   // combo box for the epoch format
   epochFormatComboBox = new wxComboBox
      ( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   epochFormatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));

   // label for epoch
   initialEpochTxt = new wxStaticText( this, ID_TEXT,
      "" GUI_ACCEL_KEY "Initial Epoch", wxDefaultPosition, wxSize(staticTextLarger,-1), 0 );

   // textfield for the initial epoch value
   initialEpochTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   initialEpochTxtCtrl->SetToolTip(pConfig->Read(_T("EpochHint")));

   // label for epoch
   finalEpochTxt = new wxStaticText( this, ID_TEXT,
      "" GUI_ACCEL_KEY "Final Epoch", wxDefaultPosition, wxSize(staticTextLarger,-1), 0 );

   // textfield for the final epoch value
   finalEpochTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   finalEpochTxtCtrl->SetToolTip(pConfig->Read(_T("EpochHint")));

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() epoch widgets created ...\n");
   #endif

   //-----------------------------------------------------------------
   // Light-time and stellar aberration
   //-----------------------------------------------------------------
   // check boxes
   lightTimeDelayCheckBox = new wxCheckBox(this, ID_CHECKBOX, wxT("Use light-time delay"),
                                wxDefaultPosition, wxDefaultSize, 0);

   stellarAberrationCheckBox = new wxCheckBox(this, ID_CHECKBOX, wxT("Use stellar aberration"),
                                   wxDefaultPosition, wxDefaultSize, 0);
   if (locatorType == CONTACT)
   {
      lightTimeDirectionTxt = new wxStaticText( this, ID_TEXT,
         "" GUI_ACCEL_KEY "Light-time direction", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
      lightTimeDirectionComboBox = new wxComboBox
         ( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
               emptyList, wxCB_DROPDOWN | wxCB_READONLY );
      lightTimeDirectionComboBox->SetToolTip(pConfig->Read(_T("LightTimeDirection")));
      #ifdef DEBUG_EVENTPANEL_CREATE
         MessageInterface::ShowMessage("EventLocatorPanel::Create() light-time direction widgets created!!!!! ...\n");
      #endif
   }

   stepSizeTxt = new wxStaticText( this, ID_TEXT,
      "" GUI_ACCEL_KEY "Step size", wxDefaultPosition, wxSize(staticTextWidth,-1), 0 );
   stepSizeTxtCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   stepSizeUnitsTxt = new wxStaticText( this, ID_TEXT,
      "s", wxDefaultPosition, wxSize(10,-1), 0 );

   //-----------------------------------------------------------------
   // Intrusion Parameters: Minimum phase, report coordinates
   //-----------------------------------------------------------------
   if (locatorType == INTRUSION)
   {
      // Minimum phase
      minPhaseTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Minimum Phase", wxDefaultPosition,
         wxSize(staticTextWidth, -1), 0);
      minPhaseTxtCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth, -1), 0,
         wxTextValidator(wxGMAT_FILTER_NUMERIC));

      // Report coordinates
      reportCoordTxt = new wxStaticText(this, ID_TEXT,
         "" GUI_ACCEL_KEY "Report Coordinates",
         wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      reportCoordComboBox = new wxComboBox
      (this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth, -1),
         emptyList, wxCB_DROPDOWN | wxCB_READONLY);

      // Spice grid file
      gridFrameFileTxtCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
         wxDefaultPosition, wxSize(epochWidth, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      gridFrameFileTxt = new wxStaticText(this, ID_TEXT,
         "Spice Grid Frame File", wxDefaultPosition, wxSize(staticTextWidth, -1), 0);
      gridFileBrowseButton =
         new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap,
            wxDefaultPosition, wxSize(buttonWidth, -1));
   }

   // Make s small blank string to match up the static box sizers
   wxStaticText *blankTxt = new wxStaticText( this, ID_TEXT,
      " ", wxDefaultPosition, wxSize(10,-1), 0 );


   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() ALL widgets created ...\n");
   #endif

   leftGridSizer->Add( scTargetTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add( scTargetComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add(20,20);
   if (locatorType == INTRUSION)
   {
      leftGridSizer->Add(cenBodyTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      leftGridSizer->Add(cenBodyComboBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      leftGridSizer->Add(20, 20);
   }
   leftGridSizer->Add( bodiesTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add( bodiesCheckListBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add(20,20);
   switch (locatorType)
   {
   case CONTACT:
      leftGridSizer->Add(observersTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      leftGridSizer->Add(observersCheckListBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      break;
   case ECLIPSE:
      leftGridSizer->Add(eclipseTypesTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      leftGridSizer->Add(eclipseTypesCheckListBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      break;
   case INTRUSION:
      leftGridSizer->Add(imagersTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      leftGridSizer->Add(imagersCheckListBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      break;
   default:
      break;
   }
   leftGridSizer->Add(20,20);
   leftGridSizer->Add( fileNameTxt,          0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add( fileNameTxtCtrl,      0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add( fileNameBrowseButton, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );


   leftGridSizer->Add( runModeTxt,          0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add( runModeComboBox,     0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   leftGridSizer->Add(20,20);

   if (centeredCheckboxes)
   {
      leftGridSizer->Add(20,20);
      leftGridSizer->Add( writeReportCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      leftGridSizer->Add(20,20);
   }
   else
   {
      leftGridSizer->Add( writeReportCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      leftGridSizer->Add(20,20);
      leftGridSizer->Add(20,20);
   }

   leftSizer->Add(leftGridSizer, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() leftGridSizer set up ...\n");
   #endif

   if (centeredCheckboxes)
   {
      upperRightGridSizer->Add(20,20);
      upperRightGridSizer->Add( entireIntervalCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   }
   else
   {
      upperRightGridSizer->Add( entireIntervalCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      upperRightGridSizer->Add(20,20);
   }
   upperRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   upperRightGridSizer->Add( epochFormatTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( epochFormatComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   upperRightGridSizer->Add( initialEpochTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( initialEpochTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   upperRightGridSizer->Add( finalEpochTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( finalEpochTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   upperRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   if (centeredCheckboxes)
   {
      lowerRightGridSizer->Add(20,20);
      lowerRightGridSizer->Add( lightTimeDelayCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add(20,20);
      lowerRightGridSizer->Add( stellarAberrationCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   }
   else
   {
      lowerRightGridSizer->Add( lightTimeDelayCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add(20,20);
      lowerRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add( stellarAberrationCheckBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add(20,20);
      lowerRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   }
   if (locatorType == CONTACT)
   {
      lowerRightGridSizer->Add( lightTimeDirectionTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add( lightTimeDirectionComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      lowerRightGridSizer->Add( blankTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   }
   lowerRightGridSizer->Add( stepSizeTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   lowerRightGridSizer->Add( stepSizeTxtCtrl, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   lowerRightGridSizer->Add( stepSizeUnitsTxt, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   if (locatorType == INTRUSION)
   {
      lowerRightGridSizer->Add(minPhaseTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(minPhaseTxtCtrl, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(blankTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(reportCoordTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(reportCoordComboBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(blankTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(gridFrameFileTxt, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(gridFrameFileTxtCtrl, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
      lowerRightGridSizer->Add(gridFileBrowseButton, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);
   }

   #ifdef DEBUG_EVENTPANEL_CREATE
      MessageInterface::ShowMessage("EventLocatorPanel::Create() right sizers set up ...\n");
   #endif

   // Upper right
   sizerUpperRight->Add(upperRightGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   // LowerRight
   sizerLowerRight->Add(lowerRightGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   rightSizer->Add(sizerUpperRight, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   rightSizer->Add(sizerLowerRight, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   eventSizer->Add( leftSizer, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   eventSizer->Add( rightSizer, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );

   theMiddleSizer->Add(eventSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   #ifdef DEBUG_EVENTPANEL_CREATE
   MessageInterface::ShowMessage("EventLocatorPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void EventLocatorPanel::LoadData()
{
   #ifdef DEBUG_EVENTPANEL_LOAD
   MessageInterface::ShowMessage("EventLocatorPanel::LoadData() entered\n");
   #endif

   // Set object pointer for "Show Script"
   mObject = theObject;

   Integer  paramID;
   wxString valueString;

   EventLocator *theLocator = (EventLocator*) theObject;

   // Load the epoch formats
   StringArray reps = TimeSystemConverter::Instance()->GetValidTimeRepresentations();
   for (unsigned int i = 0; i < reps.size(); i++)
      epochFormatComboBox->Append(reps[i].c_str());

   #ifdef DEBUG_EVENTPANEL_LOAD
   MessageInterface::ShowMessage
      ("   epochFormatComboBox has loaded its data ...\n");
   #endif

   // Load the spacecraft/target data
   std::string scTargetName;
   if (locatorType == CONTACT)
      scTargetName = theLocator->GetStringParameter("Target");
   else
      scTargetName = theLocator->GetStringParameter("Spacecraft");

   if (scTargetName == "")
   {
      scTargetComboBox->SetSelection(0);
   }
   else
   {
      valueString = wxString(scTargetName.c_str());
      scTargetComboBox->SetValue(valueString);
   }
   prevSCTargetStr = scTargetName;

   // load the occulting bodies
   wxString wxBody;
   StringArray bodies;
   if (locatorType != INTRUSION)
   {
      paramID = theLocator->GetParameterID("OccultingBodies");
      bodies = theLocator->GetStringArrayParameter(paramID);
   }
   else
   {
      paramID = theLocator->GetParameterID("IntrudingBodies");
      bodies = theLocator->GetStringArrayParameter(paramID);
   }

   #ifdef DEBUG_EVENTPANEL_LOAD
      MessageInterface::ShowMessage
         ("   occulting bodies are: ...\n");
      for (Integer tt = 0; tt < bodies.size(); tt++)
         MessageInterface::ShowMessage("    %d     %s\n", tt, bodies.at(tt).c_str());
   #endif

   unsigned int bodiesSz   = bodies.size();
   for (unsigned int ii = 0; ii < bodiesSz; ii++)
   {
      wxBody = bodies.at(ii).c_str();
      int itsPos = bodiesCheckListBox->FindString(wxBody);
      if (itsPos != wxNOT_FOUND)
      {
         bodiesCheckListBox->Check(itsPos);
      }
   }

   // load the eclipse types or observers
   wxString wxEcObs;
   int itsPos = -1;

   switch (locatorType)
   {
   case ECLIPSE:
   {
      paramID = theLocator->GetParameterID("EclipseTypes");
      StringArray possibleEclipseTypes = theLocator->GetPropertyEnumStrings(paramID);
      unsigned int posEcSz = possibleEclipseTypes.size();
      #ifdef DEBUG_ECLIPSE_OBSERVERS
         MessageInterface::ShowMessage("-- The possible EclipseTypes are:\n");
         for (Integer ll = 0; ll < posEcSz; ll++)
            MessageInterface::ShowMessage("   %d   %s\n",
               ll, possibleEclipseTypes.at(ll).c_str());
      #endif

      wxString *wxPossibleEcTypes = new wxString[posEcSz];
      for (unsigned int kk = 0; kk < posEcSz; kk++)
      {
         wxString asWx = STD_TO_WX_STRING(possibleEclipseTypes.at(kk));
         wxPossibleEcTypes[kk] = asWx;
         #ifdef DEBUG_ECLIPSE_OBSERVERS
            MessageInterface::ShowMessage("-- Adding to Eclipse Types CBL:\n");
            MessageInterface::ShowMessage("   %s\n",
               asWx.WX_TO_C_STRING);
         #endif
      }
      eclipseTypesCheckListBox->InsertItems(posEcSz, wxPossibleEcTypes, 0);

      StringArray eclipseTypes = theLocator->GetStringArrayParameter(paramID);
      unsigned int ecSz = eclipseTypes.size();
      #ifdef DEBUG_ECLIPSE_OBSERVERS
         MessageInterface::ShowMessage("-- And these are the selected ones:\n");
         for (Integer ll = 0; ll < ecSz; ll++)
            MessageInterface::ShowMessage("   %d   %s\n",
               ll, eclipseTypes.at(ll).c_str());
      #endif
      for (unsigned int kk = 0; kk < ecSz; kk++)
      {
         wxEcObs = eclipseTypes.at(kk).c_str();
         itsPos = eclipseTypesCheckListBox->FindString(wxEcObs);
         if (itsPos != wxNOT_FOUND)
         {
            eclipseTypesCheckListBox->Check(itsPos);
            #ifdef DEBUG_ECLIPSE_OBSERVERS
               MessageInterface::ShowMessage("-- Checking the box ON for: ");
               MessageInterface::ShowMessage("   %s\n",
                  wxEcObs.WX_TO_C_STRING);
            #endif
         }
      }
      break;
   }
   case CONTACT:
   {
      paramID = theLocator->GetParameterID("Observers");
      StringArray observers = theLocator->GetStringArrayParameter(paramID);
      unsigned int obsSz = observers.size();
      #ifdef DEBUG_ECLIPSE_OBSERVERS
         MessageInterface::ShowMessage("-- The observers are:\n");
         for (Integer ll = 0; ll < obsSz; ll++)
            MessageInterface::ShowMessage("   %d   %s\n",
               ll, observers.at(ll).c_str());
      #endif
      for (unsigned int ii = 0; ii < obsSz; ii++)
      {
         wxEcObs = observers.at(ii).c_str();
         itsPos = observersCheckListBox->FindString(wxEcObs);
         if (itsPos != wxNOT_FOUND)
         {
            observersCheckListBox->Check(itsPos);
         }
      }
      break;
   }
   case INTRUSION:
   {
      paramID = theLocator->GetParameterID("Sensors");
      StringArray imagers = theLocator->GetStringArrayParameter(paramID);
      unsigned int obsSz = imagers.size();
      #ifdef DEBUG_ECLIPSE_OBSERVERS
         MessageInterface::ShowMessage("-- The imagers are:\n");
         for (Integer ll = 0; ll < obsSz; ll++)
            MessageInterface::ShowMessage("   %d   %s\n",
               ll, imagers.at(ll).c_str());
      #endif
      for (unsigned int ii = 0; ii < obsSz; ii++)
      {
         wxEcObs = imagers.at(ii).c_str();
         itsPos = imagersCheckListBox->FindString(wxEcObs);
         if (itsPos != wxNOT_FOUND)
         {
            imagersCheckListBox->Check(itsPos);
         }
      }
      break;
   }
   default:
      break;
   }

   // load the filename
   paramID = theLocator->GetParameterID("Filename");
   std::string filename = theLocator->GetStringParameter(paramID);
   fileNameTxtCtrl->SetValue(wxString(filename.c_str()));

   // load the write report flag
   paramID = theLocator->GetParameterID("WriteReport");
   bool writeReport = theLocator->GetBooleanParameter(paramID);
   writeReportCheckBox->SetValue(writeReport);

   // load the run mode
   paramID = theLocator->GetParameterID("RunMode");
   StringArray theModes = theLocator->GetPropertyEnumStrings(paramID);
   unsigned int modeSz = theModes.size();
   for (unsigned int kk = 0; kk < modeSz; kk++)
   {
      runModeComboBox->Append(theModes.at(kk).c_str());
   }
   std::string currentMode = theLocator->GetStringParameter(paramID);
   runModeComboBox->SetValue(wxString(currentMode.c_str()));


   // load the entire interval flag
   paramID = theLocator->GetParameterID("UseEntireInterval");
   bool useEntireInterval = theLocator->GetBooleanParameter(paramID);
   entireIntervalCheckBox->SetValue(useEntireInterval);

   // load the epoch
   std::string epochFormat   = theLocator->GetStringParameter("InputEpochFormat");
   std::string initEpochStr  = theLocator->GetStringParameter("InitialEpoch");
   std::string finalEpochStr = theLocator->GetStringParameter("FinalEpoch");

   epochFormatComboBox->SetValue(wxString(epochFormat.c_str()));
   fromEpochFormat = epochFormat;

   #ifdef DEBUG_EVENTPANEL_LOAD
   MessageInterface::ShowMessage
      ("   epochFormat=%s, initEpochStr=%s, finalEpochStr=%s\n", epochFormat.c_str(),
            initEpochStr.c_str(), finalEpochStr.c_str());
   #endif

   #ifdef DEBUG_EVENTPANEL_LOAD
   MessageInterface::ShowMessage
      ("   loaded the epoch format ...\n");
   #endif
   // Save to TAIModJulian string to avoid keep reading the field
   // and convert to proper format when ComboBox is changed.
   if (epochFormat == "TAIModJulian")
   {
      taiMjdInitialEpochStr = initEpochStr;
      taiMjdFinalEpochStr   = finalEpochStr;
   }
   else
   {
      Real fromMjd = -999.999;
      Real outMjd;
      std::string outStr;
      #ifdef DEBUG_EVENTPANEL_LOAD
      MessageInterface::ShowMessage
         ("  about to convert from the epoch format %s to TAIModJulian ...\n",
               fromEpochFormat.c_str());
      #endif
      TimeSystemConverter::Instance()->Convert(fromEpochFormat, fromMjd, initEpochStr,
                                 "TAIModJulian", outMjd, outStr);
      taiMjdInitialEpochStr = outStr;

      fromMjd = -999.999;
      TimeSystemConverter::Instance()->Convert(fromEpochFormat, fromMjd, finalEpochStr,
                                 "TAIModJulian", outMjd, outStr);
      taiMjdFinalEpochStr = outStr;

      #ifdef DEBUG_EVENTPANEL_LOAD
      MessageInterface::ShowMessage("   taiMjdInitialEpochStr=%s\n", taiMjdInitialEpochStr.c_str());
      MessageInterface::ShowMessage("   taiMjdFinalEpochStr  =%s\n", taiMjdFinalEpochStr.c_str());
      #endif
   }
   theInitEpochStr  = initEpochStr;
   initialEpochTxtCtrl->SetValue(initEpochStr.c_str());
   theFinalEpochStr = finalEpochStr;
   finalEpochTxtCtrl->SetValue(finalEpochStr.c_str());

   // load the light-time delay flag
   paramID = theLocator->GetParameterID("UseLightTimeDelay");
   bool useLightTime = theLocator->GetBooleanParameter(paramID);
   lightTimeDelayCheckBox->SetValue(useLightTime);

   // load the stellar aberration flag
   paramID = theLocator->GetParameterID("UseStellarAberration");
   bool useStellarAberration = theLocator->GetBooleanParameter(paramID);
   stellarAberrationCheckBox->SetValue(useStellarAberration);

   if (locatorType == CONTACT)
   {
      paramID = theLocator->GetParameterID("LightTimeDirection");
      StringArray possibleDirections = theLocator->GetPropertyEnumStrings(paramID);
      unsigned int dirSz = possibleDirections.size();
      for (unsigned int kk = 0; kk < dirSz; kk++)
      {
         lightTimeDirectionComboBox->Append(possibleDirections.at(kk).c_str());
      }
      std::string currentDir = theLocator->GetStringParameter(paramID);
      lightTimeDirectionComboBox->SetValue(wxString(currentDir.c_str()));
   }

   // load the step size
   paramID = theLocator->GetParameterID("StepSize");
   Real step = theLocator->GetRealParameter(paramID);
//   std::string stepString = GmatStringUtil::Trim(GmatStringUtil::ToString(step));
   stepSizeTxtCtrl->SetValue(ToString(step));

   #ifdef DEBUG_EVENTPANEL_LOAD
      MessageInterface::ShowMessage
         ("  about to enable or disable widgets as needed\n");
      MessageInterface::ShowMessage("is useEntireInterval box checked? %s\n",
            (entireIntervalCheckBox->IsChecked()? "true": "false"));
      MessageInterface::ShowMessage("is lightTimeDelay box checked? %s\n",
            (lightTimeDelayCheckBox->IsChecked()? "true": "false"));
   #endif
   // Enable/disable as needed
   if (entireIntervalCheckBox->IsChecked())
   {
//      epochFormatTxt->Disable();
      epochFormatComboBox->Disable();
//      initialEpochTxt->Disable();
      initialEpochTxtCtrl->Disable();
//      finalEpochTxt->Disable();
      finalEpochTxtCtrl->Disable();
      #ifdef DEBUG_EVENTPANEL_LOAD
         MessageInterface::ShowMessage
            ("  time-related ones disabled\n");
      #endif
   }
   else
   {
//      epochFormatTxt->Enable();
      epochFormatComboBox->Enable();
//      initialEpochTxt->Enable();
      initialEpochTxtCtrl->Enable();
//      finalEpochTxt->Enable();
      finalEpochTxtCtrl->Enable();
      #ifdef DEBUG_EVENTPANEL_LOAD
         MessageInterface::ShowMessage
            ("  time-related ones enabled\n");
      #endif
   }
   if (lightTimeDelayCheckBox->IsChecked())
   {
      stellarAberrationCheckBox->Enable();
      if (locatorType == CONTACT)
      {
//         lightTimeDirectionTxt->Enable();
         lightTimeDirectionComboBox->Enable();
      }
      #ifdef DEBUG_EVENTPANEL_LOAD
         MessageInterface::ShowMessage
            ("  light-time-related ones enabled\n");
      #endif
   }
   else
   {
      stellarAberrationCheckBox->Disable();
      stellarAberrationCheckBox->SetValue(false);
      #ifdef DEBUG_EVENTPANEL_LOAD
         MessageInterface::ShowMessage
            ("  stellar-aberration-related ones disabled\n");
      #endif
      if (locatorType == CONTACT)
      {
//         lightTimeDirectionTxt->Disable();
         lightTimeDirectionComboBox->Disable();
      }
      #ifdef DEBUG_EVENTPANEL_LOAD
         MessageInterface::ShowMessage
            ("  light-time-related ones disabled\n");
      #endif
   }
   if (locatorType == INTRUSION)
   {
      //Central Body
      paramID = theLocator->GetParameterID("CentralBody");
      std::string centralBody = theLocator->GetStringParameter(paramID);
      cenBodyComboBox->SetValue(wxString(centralBody.c_str()));

      // Minimum Phase
      paramID = theLocator->GetParameterID("MinimumPhase");
      Real minPhase = theLocator->GetRealParameter(paramID);
      minPhaseTxtCtrl->SetValue(ToString(minPhase));

      // Report Coordinates
      paramID = theLocator->GetParameterID("ReportCoordinates");
      StringArray theCoords = theLocator->GetPropertyEnumStrings(paramID);
      UnsignedInt coordSz = theCoords.size();
      for (UnsignedInt kk = 0; kk < coordSz; kk++)
      {
         reportCoordComboBox->Append(theCoords.at(kk).c_str());
      }
      std::string repCoord = theLocator->GetStringParameter(paramID);
      reportCoordComboBox->SetValue(wxString(repCoord.c_str()));

      // Spice Grid Frame File
      paramID = theLocator->GetParameterID("SpiceGridFrameFile");
      std::string gridFile = theLocator->GetStringParameter(paramID);
      gridFrameFileTxtCtrl->SetValue(wxString(gridFile.c_str()));

      // Check if a grid frame file is required
      if (reportCoordComboBox->GetValue() == "FixedGrid")
      {
         gridFrameFileTxtCtrl->Enable();
         gridFileBrowseButton->Enable();
      }
      else
      {
         gridFrameFileTxtCtrl->Disable();
         gridFileBrowseButton->Disable();
      }
   }

   #ifdef DEBUG_EVENTPANEL_LOAD
   MessageInterface::ShowMessage("EventLocatorPanel::LoadData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void EventLocatorPanel::SaveData()
{
   #ifdef DEBUG_EVENTPANEL_SAVE
   MessageInterface::ShowMessage("EventLocatorPanel::SaveData() entered\n");
   #endif
   // create local copy of mObject
   if (localObject != NULL)
   {
	   delete localObject;
   }
   localObject = mObject->Clone();
   SaveData(localObject);
   #ifdef DEBUG_EVENTPANEL_SAVE
   MessageInterface::ShowMessage("EventLocatorPanel::SaveData() done setting local object\n");
   #endif

   // if no errors, save again
   if (canClose)
   {
      #ifdef DEBUG_EVENTPANEL_SAVE
      MessageInterface::ShowMessage("EventLocatorPanel::SaveData() copying data back to object\n");
      #endif
//      mObject->Copy(localObject);
      theObject->Copy(localObject);

      #ifdef DEBUG_EVENTPANEL_SAVE
      MessageInterface::ShowMessage("EventLocatorPanel::SaveData() about to reset changed flags\n");
      #endif
//      // reset changed flags
//	   ResetChangedFlags();
   }

   #ifdef DEBUG_EVENTPANEL_SAVE
   MessageInterface::ShowMessage("EventLocatorPanel::SaveData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void EventLocatorPanel::SaveData(GmatBase *forObject)
{
   #ifdef DEBUG_EVENTPANEL_SAVE
   MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) entered\n");
   #endif

   canClose = true;
   std::string str;
   wxString    wxStr;
   Real theStepSize;
   bool theWriteReport, theEntireInterval, theLightTimeDelay;

   bool realDataChanged = false;
   bool isValid = true;

   //-----------------------------------------------------------------
   // check values from text fields
   //-----------------------------------------------------------------
   if (isStepSizeChanged)
   {
      #ifdef DEBUG_EVENTPANEL_SAVE
      MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) - there has been a text mod!!\n");
      #endif
      str = stepSizeTxtCtrl->GetValue();
      isValid = CheckReal(theStepSize, str, "StepSize", "Real Number >= 0", false,
            true, true, true);
      realDataChanged = true;
   }

   if (!canClose)
      return;

   try
   {
      Integer paramID;
      std::string epochFormat     = epochFormatComboBox->GetValue().WX_TO_STD_STRING;

      std::string newInitEpoch    = initialEpochTxtCtrl->GetValue().WX_TO_STD_STRING;
      std::string newFinalEpoch   = finalEpochTxtCtrl->GetValue().WX_TO_STD_STRING;
      Real        fromMjd         = -999.999;
      Real        a1mjd           = -999.999;
      std::string outStr;

      #ifdef DEBUG_EVENTPANEL_SAVE
      MessageInterface::ShowMessage
         ("   newInitEpoch=%s, newFinalEpoch=%s, epochFormat=%s\n",
               newInitEpoch.c_str(), newFinalEpoch.c_str(), epochFormat.c_str());
      #endif

      if (isFileNameChanged || isFileNameTextChanged)
      {
         str = fileNameTxtCtrl->GetValue().WX_TO_STD_STRING;
         canClose = CheckFileName(str, "Filename");
         if (!canClose)
            return;
         paramID = forObject->GetParameterID("Filename");
         forObject->SetStringParameter(paramID, str);
         isFileNameChanged     = false;
         isFileNameTextChanged = false;
      }

      // Only save the epoch information if the UseEntireInterval box is NOT checked
      if (!entireIntervalCheckBox->IsChecked())
      {
         // Save epoch format and epoch
         if (isEpochFormatChanged      || isInitialEpochChanged ||
             isInitialEpochTextChanged || isFinalEpochChanged   ||
             isFinalEpochTextChanged)
         {
            bool timeOK = CheckTimeFormatAndValue(epochFormat, newInitEpoch,
                  "InitialEpoch", true);
            timeOK = timeOK && CheckTimeFormatAndValue(epochFormat, newFinalEpoch,
                  "FinalEpoch", true);

            #ifdef DEBUG_EVENTPANEL_SAVE
            MessageInterface::ShowMessage
               ("   timeOK = %s\n", (timeOK? "YES!" : "no"));
            MessageInterface::ShowMessage("    new init epoch  = %s\n",
                  newInitEpoch.c_str());
            MessageInterface::ShowMessage("    new final epoch = %s\n",
                  newFinalEpoch.c_str());
            #endif
            if (timeOK)
            {
               #ifdef DEBUG_EVENTPANEL_SAVE
               MessageInterface::ShowMessage
                  ("   About to set format and time on the object ---------\n");
               #endif
               paramID = forObject->GetParameterID("InputEpochFormat");
               forObject->SetStringParameter(paramID, epochFormat);
               paramID = forObject->GetParameterID("InitialEpoch");
               forObject->SetStringParameter(paramID, newInitEpoch);
               paramID = forObject->GetParameterID("FinalEpoch");
               forObject->SetStringParameter(paramID, newFinalEpoch);
               #ifdef DEBUG_EVENTPANEL_SAVE
               MessageInterface::ShowMessage
                  ("   DONE setting format and time on the object ---------\n");
               #endif
               isEpochFormatChanged        = false;
               isInitialEpochChanged       = false;
               isInitialEpochTextChanged   = false;
               isFinalEpochChanged         = false;
               isFinalEpochTextChanged     = false;
            }
            else
            {
               #ifdef DEBUG_EVENTPANEL_SAVE
                  MessageInterface::ShowMessage("EventLocatorPanel::SaveData() setting canClose to false inside (epoch) try\n");
               #endif
               canClose = false;
            }
         }
      }
      // Save Real Data
      if (realDataChanged)
      {
         #ifdef DEBUG_EVENTPANEL_SAVE
         MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) - attempting to real data!!\n");
         #endif
         paramID = forObject->GetParameterID("StepSize");
         forObject->SetRealParameter(paramID, theStepSize);
         realDataChanged = false;
      }
      // SC/Target
      if (isSCTargetChanged)
      {
         #ifdef DEBUG_EVENTPANEL_SAVE
         MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) - saving SC/Target!!\n");
         #endif
         str     = scTargetComboBox->GetValue().WX_TO_STD_STRING;
         if (locatorType == CONTACT)
            paramID = forObject->GetParameterID("Target");
         else
            paramID = forObject->GetParameterID("Spacecraft");
         forObject->SetStringParameter(paramID, str);
         isSCTargetChanged = false;
      }

      Integer count = 0;
      // Occulting/Intruding bodies
      if (isBodyListChanged)
      {
         #ifdef DEBUG_EVENTPANEL_SAVE
         MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) - saving occulting bodies!!\n");
         #endif
         count     = bodiesCheckListBox->GetCount();
         if (locatorType != INTRUSION)
         {
            forObject->TakeAction("Clear", "OccultingBodies");
            paramID = forObject->GetParameterID("OccultingBodies");
            for (int i = 0; i < count; i++)
            {
               if (bodiesCheckListBox->IsChecked(i))
               {
                  std::string bodyName = bodiesCheckListBox->GetString(i).WX_TO_STD_STRING;
                  #ifdef DEBUG_EVENTPANEL_SAVE
                     MessageInterface::ShowMessage("--- adding %s to the list of bodies\n", bodyName.c_str());
                  #endif
                  forObject->SetStringParameter(paramID, bodyName);
               }
            }
         }
         else
         {
            forObject->TakeAction("Clear", "IntrudingBodies");
            paramID = forObject->GetParameterID("IntrudingBodies");
            for (int i = 0; i < count; i++)
            {
               if (bodiesCheckListBox->IsChecked(i))
               {
                  std::string bodyName = bodiesCheckListBox->GetString(i).WX_TO_STD_STRING;
                  #ifdef DEBUG_EVENTPANEL_SAVE
                     MessageInterface::ShowMessage("--- adding %s to the list of bodies\n", bodyName.c_str());
                  #endif
                  forObject->SetStringParameter(paramID, bodyName);
               }
            }
         }
         isBodyListChanged = false;
      }

      // Eclipse types OR Observers
      if ((locatorType == ECLIPSE) && isEclipseTypesChanged)
      {
         count     = eclipseTypesCheckListBox->GetCount();
         forObject->TakeAction("Clear", "EclipseTypes");
         paramID   = forObject->GetParameterID("EclipseTypes");
         for (int i = 0; i < count; i++)
         {
            if (eclipseTypesCheckListBox->IsChecked(i))
            {
               std::string str =  eclipseTypesCheckListBox->GetString(i).WX_TO_STD_STRING;
               #ifdef DEBUG_EVENTPANEL_SAVE
               MessageInterface::ShowMessage("--- adding %s to the list of eclObs\n", str.c_str());
               #endif
               forObject->SetStringParameter(paramID, str);
            }
         }
         isEclipseTypesChanged = false;
      }
      if ((locatorType == CONTACT) && isObserverListChanged)
      {
         #ifdef DEBUG_EVENTPANEL_SAVE
         MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) - saving eclipse types or observers!!\n");
         #endif
         count     = observersCheckListBox->GetCount();
         forObject->TakeAction("Clear", "Observers");
         paramID   = forObject->GetParameterID("Observers");
         for (int i = 0; i < count; i++)
         {
            if (observersCheckListBox->IsChecked(i))
            {
               std::string str =  observersCheckListBox->GetString(i).WX_TO_STD_STRING;
               #ifdef DEBUG_EVENTPANEL_SAVE
               MessageInterface::ShowMessage("--- adding %s to the list of eclObs\n", str.c_str());
               #endif
               forObject->SetStringParameter(paramID, str);
            }
         }
         isObserverListChanged = false;
      }
      // run mode change
      if (isRunModeChanged)
      {
         str     = runModeComboBox->GetValue().WX_TO_STD_STRING;
         paramID = forObject->GetParameterID("RunMode");
         forObject->SetStringParameter(paramID, str);
         isRunModeChanged = false;
      }

      // Checkbox values
      if (isWriteReportChanged)
      {
         paramID = forObject->GetParameterID("WriteReport");
         if (writeReportCheckBox->IsChecked())
             forObject->SetBooleanParameter(paramID, true);
         else
            forObject->SetBooleanParameter(paramID, false);
         isWriteReportChanged = false;
      }
      if (isEntireIntervalChanged)
      {
         paramID = forObject->GetParameterID("UseEntireInterval");
         if (entireIntervalCheckBox->IsChecked())
             forObject->SetBooleanParameter(paramID, true);
         else
            forObject->SetBooleanParameter(paramID, false);
         isEntireIntervalChanged = false;
      }
      if (isLightTimeDelayChanged)
      {
         paramID = forObject->GetParameterID("UseLightTimeDelay");
         if (lightTimeDelayCheckBox->IsChecked())
            forObject->SetBooleanParameter(paramID, true);
         else
            forObject->SetBooleanParameter(paramID, false);
         isLightTimeDelayChanged = false;
      }
      // Stellar aberration could be set to false if the light time delay is turned off
      if (isStellarAberrationChanged)
      {
         paramID = forObject->GetParameterID("UseStellarAberration");
         if (stellarAberrationCheckBox->IsChecked())
            forObject->SetBooleanParameter(paramID, true);
         else
            forObject->SetBooleanParameter(paramID, false);
         isStellarAberrationChanged = false;
      }

      if (lightTimeDelayCheckBox->IsChecked())
      {
//         if (isStellarAberrationChanged)
//         {
//            paramID = forObject->GetParameterID("UseStellarAberration");
//            if (stellarAberrationCheckBox->IsChecked())
//               forObject->SetBooleanParameter(paramID, true);
//            else
//               forObject->SetBooleanParameter(paramID, false);
//            isStellarAberrationChanged = false;
//         }

         // light time direction
         // SC/Target
         if ((locatorType == CONTACT) && isLightTimeDirectionChanged)
         {
            str     = lightTimeDirectionComboBox->GetValue().WX_TO_STD_STRING;
            paramID = forObject->GetParameterID("LightTimeDirection");
            forObject->SetStringParameter(paramID, str);
            isLightTimeDirectionChanged = false;
         }
      }

      // Intrusion parameter changes
      if (locatorType == INTRUSION)
      {
         // Central Body
         if (isCenBodyChanged)
         {
            str = cenBodyComboBox->GetValue().WX_TO_STD_STRING;
            paramID = forObject->GetParameterID("CentralBody");
            forObject->SetStringParameter(paramID, str);
            isCenBodyChanged = false;
         }
         // Sensors
         if (isImagerListChanged)
         {
            forObject->TakeAction("Clear", "Sensors");
            paramID = forObject->GetParameterID("Sensors");
            count = imagersCheckListBox->GetCount();
            for (int i = 0; i < count; i++)
            {
               if (imagersCheckListBox->IsChecked(i))
               {
                  std::string imagerName = imagersCheckListBox->GetString(i).WX_TO_STD_STRING;
                  #ifdef DEBUG_EVENTPANEL_SAVE
                     MessageInterface::ShowMessage("--- adding %s to the list of imagers\n", bodyName.c_str());
                  #endif
                  forObject->SetStringParameter(paramID, imagerName);
               }
            }
            isImagerListChanged = false;
         }
         // Minimum Phase
         if (isMinPhaseTextChanged)
         {
            Real minPhase;
            str = minPhaseTxtCtrl->GetValue().WX_TO_STD_STRING;
            isValid = CheckReal(minPhase, str, "MinimumPhase", "0 <= Real Number <= 1", false,
               true, true, true);
            paramID = forObject->GetParameterID("MinimumPhase");
            forObject->SetRealParameter(paramID, minPhase);
            isMinPhaseTextChanged = false;
         }
         // Report Coordinates
         if (isReportCoordChanged)
         {
            str = reportCoordComboBox->GetValue().WX_TO_STD_STRING;
            paramID = forObject->GetParameterID("ReportCoordinates");
            forObject->SetStringParameter(paramID, str);
            isReportCoordChanged = false;
         }
         // Fixed Grid Spice File Name
         if (isGridFileNameTextChanged)
         {
            str = gridFrameFileTxtCtrl->GetValue().WX_TO_STD_STRING;
            paramID = forObject->GetParameterID("SpiceGridFrameFile");
            forObject->SetStringParameter(paramID, str);
            isGridFileNameTextChanged = false;
         }
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }

   #ifdef DEBUG_EVENTPANEL_SAVE
   MessageInterface::ShowMessage("EventLocatorPanel::SaveData(obj) exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void EventLocatorPanel::OnTextChange(wxCommandEvent &event)
{
   if (fileNameTxtCtrl->IsModified())
      isFileNameTextChanged     = true;
   if (initialEpochTxtCtrl->IsModified())
      isInitialEpochTextChanged = true;
   if (finalEpochTxtCtrl->IsModified())
      isFinalEpochTextChanged   = true;
   if (stepSizeTxtCtrl->IsModified())
      isStepSizeChanged         = true;
   if (locatorType == INTRUSION)
   {
      if (minPhaseTxtCtrl->IsModified())
         isMinPhaseTextChanged = true;
      if (gridFrameFileTxtCtrl->IsModified())
         isGridFileNameTextChanged = true;
   }

   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void EventLocatorPanel::OnComboBoxChange(wxCommandEvent &event)
{
   std::string toEpochFormat  = epochFormatComboBox->GetValue().WX_TO_STD_STRING;

   #ifdef DEBUG_EVENTPANEL_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("\nEventLocatorPanel::OnComboBoxChange() toEpochFormat=%s\n",
            toEpochFormat.c_str());
   #endif

   //-----------------------------------------------------------------
   // epoch format change
   //-----------------------------------------------------------------
   if (event.GetEventObject() == epochFormatComboBox)
   {
      isInitialEpochChanged = true;
      isFinalEpochChanged = true;

      TimeSystemConverter *theTimeConverter = TimeSystemConverter::Instance();

      #ifdef DEBUG_EVENT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("\nEventLocatorPanel::OnComboBoxChange() attempting to change epoch format ...\n");
      #endif
      try
      {
         Real fromMjd = -999.999;
         Real outMjd;
         std::string outStr;

         // if modified by user, check if initial epoch is valid first
         if (isInitialEpochTextChanged)
         {
            std::string theEpochStr = initialEpochTxtCtrl->GetValue().WX_TO_STD_STRING;

            // Save to TAIModJulian string to avoid keep reading the field
            // and convert to proper format when ComboBox is changed.
            if (fromEpochFormat == "TAIModJulian")
            {
               taiMjdInitialEpochStr = theEpochStr;
            }
            else
            {
               theTimeConverter->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                          "TAIModJulian", outMjd, outStr);
               taiMjdInitialEpochStr = outStr;
            }

            // Convert to desired format with new date
            theTimeConverter->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                       toEpochFormat, outMjd, outStr);

            initialEpochTxtCtrl->SetValue(outStr.c_str());
//            isInitialEpochChanged = false;
//            fromEpochFormat       = toEpochFormat;
         }
         else
         {
            #ifdef DEBUG_EVENTPANEL_PANEL_COMBOBOX
            MessageInterface::ShowMessage
               ("\nEventLocatorPanel::OnComboBoxChange() converting from %s to %s\n",
                     "TAIModJulian", toEpochFormat.c_str());
            #endif
            // Convert to desired format using TAIModJulian date
            theTimeConverter->Convert("TAIModJulian", fromMjd, taiMjdInitialEpochStr,
                                       toEpochFormat, outMjd, outStr);

            initialEpochTxtCtrl->SetValue(outStr.c_str());
//            fromEpochFormat = toEpochFormat;
         }
         // if modified by user, check if final epoch is valid first
         if (isFinalEpochTextChanged)
         {
            std::string theEpochStr = finalEpochTxtCtrl->GetValue().WX_TO_STD_STRING;

            // Save to TAIModJulian string to avoid keep reading the field
            // and convert to proper format when ComboBox is changed.
            if (fromEpochFormat == "TAIModJulian")
            {
               taiMjdFinalEpochStr = theEpochStr;
            }
            else
            {
               theTimeConverter->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                          "TAIModJulian", outMjd, outStr);
               taiMjdFinalEpochStr = outStr;
            }

            // Convert to desired format with new date
            theTimeConverter->Convert(fromEpochFormat, fromMjd, theEpochStr,
                                       toEpochFormat, outMjd, outStr);

            finalEpochTxtCtrl->SetValue(outStr.c_str());
//            isFinalEpochChanged   = false;
//            fromEpochFormat       = toEpochFormat;
         }
         else
         {
            #ifdef DEBUG_EVENTPANEL_PANEL_COMBOBOX
            MessageInterface::ShowMessage
               ("\nEventLocatorPanel::OnComboBoxChange() converting from %s to %s\n",
                     "TAIModJulian", toEpochFormat.c_str());
            #endif
            // Convert to desired format using TAIModJulian date
            theTimeConverter->Convert("TAIModJulian", fromMjd, taiMjdFinalEpochStr,
                                       toEpochFormat, outMjd, outStr);

            finalEpochTxtCtrl->SetValue(outStr.c_str());
//            fromEpochFormat = toEpochFormat;
         }
         fromEpochFormat       = toEpochFormat;
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(fromEpochFormat.c_str());
         MessageInterface::PopupMessage
            (Gmat::ERROR_, e.GetFullMessage() +
             "\nPlease enter valid Epoch before changing the Epoch Format\n");
      }
   }
   else if (event.GetEventObject() == scTargetComboBox)
   {
      scTargetStr       = scTargetComboBox->GetValue().WX_TO_STD_STRING;
      isSCTargetChanged = true;

      if (locatorType == INTRUSION)
      {
         // Remove new spacecraft and add in previous
         for (unsigned int ii = 0; ii < bodiesCheckListBox->GetCount(); ++ii)
         {
            if (bodiesCheckListBox->GetString(ii) == scTargetStr)
            {
               bodiesCheckListBox->Delete(ii);
               break;
            }
         }
         bodiesCheckListBox->Append(prevSCTargetStr);

         isBodyListChanged = true;
      }
      prevSCTargetStr = scTargetStr;
   }
   else if (event.GetEventObject() == runModeComboBox)
   {
      runModeStr       = runModeComboBox->GetValue().WX_TO_STD_STRING;
      isRunModeChanged = true;
   }
   else if ((locatorType == CONTACT) && event.GetEventObject() == lightTimeDirectionComboBox)
   {
      lightTimeDirectionStr       = lightTimeDirectionComboBox->GetValue().WX_TO_STD_STRING;
      isLightTimeDirectionChanged = true;
   }
   else if (locatorType == INTRUSION)
   {
      if (event.GetEventObject() == cenBodyComboBox)
         isCenBodyChanged = true;
      else if (event.GetEventObject() == reportCoordComboBox)
      {
         isReportCoordChanged = true;
         if (reportCoordComboBox->GetValue() == "FixedGrid")
         {
            gridFrameFileTxtCtrl->Enable();
            gridFileBrowseButton->Enable();
         }
         else
         {
            gridFrameFileTxtCtrl->Disable();
            gridFileBrowseButton->Disable();
         }
      }
   }

   EnableUpdate(true);
   #ifdef DEBUG_EVENTPANEL_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("\nEventLocatorPanel::OnComboBoxChange() EXITing\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnBrowseButton()
//------------------------------------------------------------------------------
void EventLocatorPanel::OnBrowseButton(wxCommandEvent &event)
{
   wxString prevFilename;
   if (event.GetEventObject() == fileNameBrowseButton)
      prevFilename = fileNameTxtCtrl->GetValue();
   else
      prevFilename = gridFrameFileTxtCtrl->GetValue();

   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;

      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(prevFilename))
      {
         if (event.GetEventObject() == fileNameBrowseButton)
         {
            fileNameTxtCtrl->SetValue(filename);
            isFileNameChanged = true;
            isFileNameTextChanged = true;
         }
         else
         {
            gridFrameFileTxtCtrl->SetValue(filename);
            isGridFileNameTextChanged = true;
         }
         EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void EventLocatorPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == writeReportCheckBox)
   {
      isWriteReportChanged = true;
   }
   else if (event.GetEventObject() == entireIntervalCheckBox)
   {
      isEntireIntervalChanged = true;
      if (entireIntervalCheckBox->IsChecked())
      {
//         epochFormatTxt->Disable();
         epochFormatComboBox->Disable();
//         initialEpochTxt->Disable();
         initialEpochTxtCtrl->Disable();
//         finalEpochTxt->Disable();
         finalEpochTxtCtrl->Disable();
      }
      else
      {
//         epochFormatTxt->Enable();
         epochFormatComboBox->Enable();
//         initialEpochTxt->Enable();
         initialEpochTxtCtrl->Enable();
//         finalEpochTxt->Enable();
         finalEpochTxtCtrl->Enable();
      }
   }
   else if (event.GetEventObject() == lightTimeDelayCheckBox)
   {
      isLightTimeDelayChanged = true;
      if (lightTimeDelayCheckBox->IsChecked())
      {
         stellarAberrationCheckBox->Enable();
         if (locatorType == CONTACT)
         {
//            lightTimeDirectionTxt->Enable();
            lightTimeDirectionComboBox->Enable();
         }
      }
      else
      {
         stellarAberrationCheckBox->Disable();
         stellarAberrationCheckBox->SetValue(false);
         isStellarAberrationChanged = true;
         if (locatorType == CONTACT)
         {
//            lightTimeDirectionTxt->Disable();
            lightTimeDirectionComboBox->Disable();
         }
      }
   }
   else if (event.GetEventObject() == stellarAberrationCheckBox)
   {
      isStellarAberrationChanged = true;
   }

   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void EventLocatorPanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == bodiesCheckListBox)
   {
      isBodyListChanged = true;
   }
   else if ((locatorType == ECLIPSE) && event.GetEventObject() == eclipseTypesCheckListBox)
   {
      isEclipseTypesChanged = true;
   }
   else if ((locatorType == CONTACT) && event.GetEventObject() == observersCheckListBox)
   {
      isObserverListChanged = true;
   }
   else if ((locatorType == INTRUSION) && event.GetEventObject() == imagersCheckListBox)
   {
      isImagerListChanged = true;
   }
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
void EventLocatorPanel::OnCheckListBoxSelect(wxCommandEvent& event)
{
   EnableUpdate(true);
}



//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts a real number to a wxString.
 *
 * @param <rval> real number to convert
 *
 * @return wxString representation of the input real number
 */
//------------------------------------------------------------------------------
wxString EventLocatorPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}

//------------------------------------------------------------------------------
// void ResetChangedFlags()
//------------------------------------------------------------------------------
void EventLocatorPanel::ResetChangedFlags()
{
   isSCTargetChanged           = false;
   isBodyListChanged           = false;
   isEclipseTypesChanged       = false;
   isObserverListChanged       = false;
   isFileNameChanged           = false;
   isFileNameTextChanged       = false;
   isWriteReportChanged        = false;
   isRunModeChanged            = false;

   isEntireIntervalChanged     = false;
   isEpochFormatChanged        = false;
   isInitialEpochChanged       = false;
   isFinalEpochChanged         = false;
   isInitialEpochTextChanged   = false;
   isFinalEpochTextChanged     = false;

   isLightTimeDelayChanged     = false;
   isStellarAberrationChanged  = false;
   isCheckForTransitsChanged   = false;
   isLightTimeDirectionChanged = false;
   isStepSizeChanged           = false;

   isCenBodyChanged            = false;
   isImagerListChanged         = false;
   isReportCoordChanged        = false;
   isMinPhaseTextChanged       = false;
   isGridFileNameTextChanged   = false;
}
