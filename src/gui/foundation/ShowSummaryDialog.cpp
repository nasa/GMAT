//$Id$
//------------------------------------------------------------------------------
//                         ShowSummaryDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show the command summary for individual
 * components.
 */
//------------------------------------------------------------------------------

#include <wx/filedlg.h>
#include "ShowSummaryDialog.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_CMD_SUMMARY_COMBOBOX
//#define DEBUG_CMD_SUMMARY_DIALOG

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ShowSummaryDialog, GmatDialog)
   EVT_COMBOBOX(ID_COMBOBOX, ShowSummaryDialog::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON_SAVEAS, ShowSummaryDialog::OnSaveAs)
END_EVENT_TABLE()



//------------------------------------------------------------------------------
//  ShowSummaryDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
//                   GmatBase *obj, bool summaryForMission, bool physicsOnly)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param parent Parent wxWindow for the dialog.
 * @param id wxWidget identifier for the dialog.
 * @param title Title for the dialog.
 * @param obj The object that provides the script text.
 */
//------------------------------------------------------------------------------
ShowSummaryDialog::ShowSummaryDialog(wxWindow *parent, wxWindowID id, 
                                     const wxString& title, GmatCommand *obj,
                                     bool summaryForMission, bool physicsOnly) :
   GmatDialog(parent, id, title, NULL, wxDefaultPosition, wxDefaultSize, 
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
   theObject              (obj),
   summaryForEntireMission(summaryForMission),
   physicsBasedOnly       (physicsOnly)
{
   #ifdef DEBUG_CMD_SUMMARY_DIALOG
      MessageInterface::ShowMessage("Creating ShowSummaryDialog with obj = %s\n", obj->GetTypeName().c_str());
   #endif
   isCoordSysModified = false;
   theGuiManager      = GuiItemManager::GetInstance();

   Create();
   ShowData();
   theOkButton->Enable(true);
   theDialogSizer->Hide(theCancelButton, true);
   theDialogSizer->Layout();
}

ShowSummaryDialog::~ShowSummaryDialog()
{
   coordSysComboBox->Clear();
   theGuiManager->UnregisterComboBox("CoordinateSystem", coordSysComboBox);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and populates the dialog.
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::Create()
{
   Integer bsize = 2; // border size

   Integer w, h;
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   if (summaryForEntireMission)
   {
      pConfig->SetPath(wxT("/Mission Summary"));
   }
   else
   {
      pConfig->SetPath(wxT("/Command Summary"));
   }
   wxString text = "Summary not yet available for this panel";
   // Find the height of a line of test, to use when sizing the text control
   GetTextExtent(text, &w, &h);

   // label for coordinate system
   wxStaticText *coordSysStaticText = new wxStaticText( this, ID_CS_TEXT,
      GUI_ACCEL_KEY"Coordinate System", wxDefaultPosition, wxDefaultSize, 0 );
   coordSysComboBox =theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   coordSysComboBox->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));

   /// the coordinate system must have a celestial body as the origin and must
   /// not contain a reference to a spacecraft (e.g. primary, etc.)
   BuildValidCoordinateSystemList();

   wxSize scriptPanelSize(500, 32);
   
   try
   {
      if (theObject != NULL)
      {
         theObject->SetupSummary("EarthMJ2000Eq", summaryForEntireMission, physicsBasedOnly);
         if (summaryForEntireMission)
            text = theObject->GetStringParameter("MissionSummary").c_str();
         else
            text = theObject->GetStringParameter("Summary").c_str();
         
         // This code is flaky -- text width is height dependent??? -- on Linux:
         //GetTextExtent(text, &w, &h);
         //w = (w + 200 > 1000 ? 1000 : w + 200);
         //h = (h > 700 ? 700 : h);
         #ifdef __WXMAC__
            scriptPanelSize.Set(658, 735);
         #else
            scriptPanelSize.Set(775, 735);
         #endif
            
         SetSize(wxDefaultCoord, wxDefaultCoord, w, h);
      }
      
      wxFlexGridSizer *coordSizer = new wxFlexGridSizer(2);
      coordSizer->Add(coordSysStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
      coordSizer->Add(coordSysComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
      
      // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
      // increase text size on Windows(loj: 2009.02.05)
      theSummary = new wxTextCtrl(this, -1, text, wxPoint(0,0), scriptPanelSize, 
                                  wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);
      theSummary->SetFont(GmatAppData::Instance()->GetScriptFont() );
      theMiddleSizer->Add(coordSizer, 0, wxGROW|wxALL, 3);
      theMiddleSizer->Add(theSummary, 1, wxGROW|wxALL, 3);

      theSaveAsButton = new wxButton(this, ID_BUTTON_SAVEAS, "Save As...",
                                     wxDefaultPosition, wxDefaultSize, 0);
      int borderSize = 2;
      theButtonSizer->Insert(3, theSaveAsButton, 0, wxALIGN_CENTER | wxALL,
                             borderSize);
   }
   catch (BaseException &e)
   {
      #ifdef DEBUG_CMD_SUMMARY_DIALOG
      std::string errmsg = e.GetFullMessage();
      errmsg += "Current Coordinate System is " + currentCoordSysName;
      MessageInterface::ShowMessage("%s\n", errmsg.c_str());      
      #endif
      
      // Unregister ComboBox and rethrow the exception
      theGuiManager->UnregisterComboBox("CoordinateSystem", coordSysComboBox);
      throw;
   }
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::LoadData()
{
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::SaveData()
{
   isCoordSysModified = false;
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * For the ShowSummaryDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::ResetData()
{
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Converts state to ComboBox selection by sending the selection to the command.
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::OnComboBoxChange(wxCommandEvent& event)
{
   std::string coordSysStr  = coordSysComboBox->GetValue().WX_TO_STD_STRING;
   wxString    text         = "Summary not yet available for this panel";

   #ifdef DEBUG_CMD_SUMMARY_COMBOBOX
   MessageInterface::ShowMessage
      ("ShowSummaryDialog::OnComboBoxChange() coordSysStr=%s\n", coordSysStr.c_str());
   #endif

   //-----------------------------------------------------------------
   // coordinate system change
   //-----------------------------------------------------------------
   if (event.GetEventObject() == coordSysComboBox)
   {
      isCoordSysModified          = true;
      std::string lastCSName      = currentCoordSysName;
      currentCoordSysName         = coordSysComboBox->GetValue().c_str();
      theObject->SetupSummary(currentCoordSysName, summaryForEntireMission, physicsBasedOnly);
      try
      {
         if (summaryForEntireMission)
            text = theObject->GetStringParameter("MissionSummary").c_str();
         else
            text = theObject->GetStringParameter("Summary").c_str();
         theSummary->ChangeValue(text);
      }
      catch (BaseException &e)
      {
         isCoordSysModified  = false;
         currentCoordSysName = lastCSName;
         std::string errmsg = e.GetFullMessage();
         errmsg += "Resetting to last valid value: " + currentCoordSysName + "\n";
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
         coordSysComboBox->SetValue(currentCoordSysName.c_str());
//         canClose = false;
      }
   }

//   dataChanged = true;
//   theScPanel->EnableUpdate(true);

   #ifdef DEBUG_CMD_SUMMARY_COMBOBOX
   MessageInterface::ShowMessage
      ("ShowSummaryDialog::OnComboBoxChange() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void BuildValidCoordinateSystemList()
//------------------------------------------------------------------------------
void ShowSummaryDialog::BuildValidCoordinateSystemList()
{
   CoordinateSystem *tmpCS = NULL;
   SpacePoint       *origin = NULL;
   std::string      currentCS = coordSysComboBox->GetValue().WX_TO_STD_STRING;
   std::string      newCS     = currentCS;
   Integer          sz;

   // The only valid coordinate system for use here:
   // 1) have a celestial body origin, and
   // 2) do not have a spacecraft as the origin or the primary or the secondary

   // get the names of the coordinate systems
   StringArray coordSystemNames;
   wxArrayString csNames = coordSysComboBox->GetStrings();
   for (Integer ii = 0; ii < (Integer) csNames.GetCount(); ii++)
      coordSystemNames.push_back(std::string((csNames.Item(ii)).c_str()));
   sz = (Integer) coordSystemNames.size();

   coordSysComboBox->Clear();
   for (Integer ii = 0; ii < sz; ii++)
   {
      if (ii == 0)                                   newCS = coordSystemNames.at(ii);
      else if (currentCS == coordSystemNames.at(ii)) newCS = currentCS;
      tmpCS      = (CoordinateSystem*) theGuiInterpreter->GetConfiguredObject(coordSystemNames.at(ii));
      origin     = tmpCS->GetOrigin();
      if ((origin->IsOfType("SpacePoint")) && (!tmpCS->UsesSpacecraft()))  // add it to the list
         coordSysComboBox->Append(wxString(coordSystemNames[ii].c_str()));
   }
   coordSysComboBox->SetValue(newCS.c_str());
   currentCoordSysName = coordSysComboBox->GetValue().c_str();
}


//------------------------------------------------------------------------------
// void OnSaveAs(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Saves the command summary to the user-specified file.
 */
//------------------------------------------------------------------------------
void ShowSummaryDialog::OnSaveAs(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Save summary"), _T(""), _T(""),
                       "Text files (*.txt, *.text)|*.txt;*.text|"
                       "All files (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

   if (dialog.ShowModal() == wxID_CANCEL)
      return;

   if (!theSummary->SaveFile(dialog.GetPath()))
   {
      MessageInterface::PopupMessage(
         Gmat::ERROR_, "Unable to save to %s", dialog.GetPath().WX_TO_C_STRING);
         
   }
}
