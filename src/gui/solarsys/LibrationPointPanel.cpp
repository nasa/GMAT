//$Id$
//------------------------------------------------------------------------------
//                              LibrationPointPanel
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
// Author: LaMont Ruley
// Created: 2005/04/27
/**
 * This class allows user to configure a LibrationPoint.
 */
//------------------------------------------------------------------------------

#include "LibrationPointPanel.hpp"
#include "GmatColorPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PANEL_LOAD
//#define DEBUG_PANEL_SAVE

//------------------------------
// event tables for wxWindows
//------------------------------

BEGIN_EVENT_TABLE(LibrationPointPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, LibrationPointPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// LibrationPointPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs a LibrationPointPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the LibrationPoint GUI
 */
//------------------------------------------------------------------------------
LibrationPointPanel::LibrationPointPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   theLibrationPt =
      (LibrationPoint*)theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   if (theLibrationPt != NULL)
   {
      theClonedLibPoint = (LibrationPoint*)(theLibrationPt->Clone());
      SetName("LibrationPointPanel");
      
      Create();
      Show();
      
      EnableUpdate(false);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("LibrationPointPanel:Create() the LibrationPoint is NULL\n");
   }
}


//------------------------------------------------------------------------------
// ~LibrationPointPanel()
//------------------------------------------------------------------------------
/**
 * Destructs a LibrationPointPanel object.
 *
 */
//------------------------------------------------------------------------------
LibrationPointPanel::~LibrationPointPanel()
{
   // Unregister GUI components
   theGuiManager->UnregisterComboBox("CelestialBody", primaryBodyCB);
   theGuiManager->UnregisterComboBox("CelestialBody", secondaryBodyCB);
   theGuiManager->RemoveFromResourceUpdateListeners(this);
   
   // Delete cloned LibrationPoint
   delete theClonedLibPoint;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles a ComboBoxChange event.
 *
 * @param <event> the handled event.
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the LibrationPoint panel's GUI components.
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::Create()
{
   int bsize = 2; // border size
   
   wxString librationList[] = {"L1", "L2", "L3", "L4", "L5"};
   // create sizers
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 2, bsize, bsize);
   
   // label for primary body combobox
   wxStaticText *primaryBodyLabel =
      new wxStaticText(this, ID_TEXT, wxT("Primary Body:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // register for updates, in order to refresh
   theGuiManager->AddToResourceUpdateListeners(this);
   
   // combo box for avaliable bodies 
   primaryBodyCB = 
      theGuiManager->GetCelestialPointComboBox(this, ID_COMBOBOX, wxSize(100,-1));
   
   // label for secondary body combobox
   wxStaticText *secondaryBodyLabel =
      new wxStaticText(this, ID_TEXT, wxT("Secondary Body:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // combo box for avaliable bodies 
   secondaryBodyCB = 
      theGuiManager->GetCelestialPointComboBox(this, ID_COMBOBOX, wxSize(100,-1));
   
   // label for libration point combobox
   wxStaticText *librationPointLabel =
      new wxStaticText(this, ID_TEXT, wxT("Libration Point:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // combo box for libration points 
   librationPtCB =
      new wxComboBox(this, ID_COMBOBOX, wxT(""),
                     wxDefaultPosition, wxSize(100,-1), 5, librationList, wxCB_READONLY);
   
   // add labels and comboboxes to page sizer    
   pageSizer->Add(primaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   pageSizer->Add(primaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
   pageSizer->Add(secondaryBodyLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   pageSizer->Add(secondaryBodyCB, 0, wxALIGN_LEFT | wxALL, bsize);
   pageSizer->Add(librationPointLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   pageSizer->Add(librationPtCB, 0, wxALIGN_LEFT | wxALL, bsize);
   
   // create Options box:
   GmatStaticBoxSizer *optionsStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionsStaticBoxSizer->Add(pageSizer, 0, wxALIGN_LEFT|wxALL,bsize);
   
   // Create color panel
   GmatColorPanel *colorPanel = new GmatColorPanel(this, this, theClonedLibPoint);
   
   // add page sizer to middle sizer
   theMiddleSizer->Add(optionsStaticBoxSizer, 0, wxEXPAND|wxALL, bsize);
   theMiddleSizer->Add(colorPanel, 0, wxEXPAND|wxALL, bsize);   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads data for the LibrationPoint.
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::LoadData()
{
   // load data from the core engine 
   try
   {
      UpdateComboBoxes();

      // load primary body
      std::string primaryBody = theClonedLibPoint->GetStringParameter("Primary");
      primaryBodyCB->SetValue(primaryBody.c_str());

      // load secondary body
      std::string secondaryBody = theClonedLibPoint->GetStringParameter("Secondary");
      secondaryBodyCB->SetValue(secondaryBody.c_str());
      
      #ifdef DEBUG_PANEL_LOAD
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() primary body = %s\n", 
             primaryBody.c_str());
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() secondary body = %s\n", 
             secondaryBody.c_str());
      #endif

      // load libration
      std::string librationPoint = theClonedLibPoint->GetStringParameter("Point");
      librationPtCB->SetValue(librationPoint.c_str());

      #ifdef DEBUG_PANEL_LOAD
         MessageInterface::ShowMessage
            ("LibrationPointPanel::LoadData() libration point = %s\n",
             librationPoint.c_str());
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
   }
   
   // Activate "ShowScript"
   mObject = theLibrationPt;
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves data to the LibrationPoint object.
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::SaveData()
{
   canClose = true;
   
   //-----------------------------------------------------------------
   // check user input
   //-----------------------------------------------------------------
   wxString primaryBodyString = primaryBodyCB->GetValue().Trim();
   wxString secondaryBodyString = secondaryBodyCB->GetValue().Trim();

   // Check to make sure the primary body and secondary body are different        
   if (primaryBodyString == secondaryBodyString)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Primary and Secondary bodies can not be the same");
      canClose = false;
      
      return;
   }
   
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      std::string spName;
      // get Earth pointer as j2000body
      CelestialBody *j2000body =
         (CelestialBody*)theGuiInterpreter->GetConfiguredObject("Earth");      
      
      // Primary body
      spName = primaryBodyString.c_str();
      int primaryBodyID = theClonedLibPoint->GetParameterID("Primary");
      theClonedLibPoint->SetStringParameter(primaryBodyID, spName);
      SpacePoint *primary = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(spName);
      theClonedLibPoint->SetRefObject(primary, Gmat::SPACE_POINT, spName);
      
      // set Earth as J000Body of primary body if NULL
      if (primary->GetJ2000Body() == NULL)
         primary->SetJ2000Body(j2000body);
      
      #ifdef DEBUG_PANEL_SAVE
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() primary body ID = %d\n", 
          primaryBodyID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() primary body = %s\n", 
          primaryBodyString.c_str());
      #endif
      
      // Secondary body
      spName = secondaryBodyString.c_str();
      int secondaryBodyID = theClonedLibPoint->GetParameterID("Secondary");
      theClonedLibPoint->SetStringParameter(secondaryBodyID, spName);
      SpacePoint *secondary = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(spName);
      theClonedLibPoint->SetRefObject(secondary, Gmat::SPACE_POINT, spName);
      
      // set Earth as J000Body of secondary body if NULL
      if (secondary->GetJ2000Body() == NULL)
         secondary->SetJ2000Body(j2000body);
      
      #ifdef DEBUG_PANEL_SAVE
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() secondary body ID = %d\n", 
          secondaryBodyID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() secondary body = %s\n", 
          secondaryBodyString.c_str());
      #endif

      
      // Libration point
      wxString librationPointString = librationPtCB->GetValue().Trim();
      int librationPointID = theClonedLibPoint->GetParameterID("Point");
      
      #ifdef DEBUG_PANEL_SAVE
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() libration point ID = %d\n", 
          librationPointID);
      MessageInterface::ShowMessage
         ("LibrationPointPanel::SaveData() libration point = %s\n", 
          librationPointString.c_str());
      #endif
      
      theClonedLibPoint->SetStringParameter(librationPointID, librationPointString.c_str());
      
      // Copy cloned object to actual object
      theLibrationPt->Copy(theClonedLibPoint);
      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage().c_str());
      canClose = false;
   }
}


//------------------------------------------------------------------------------
// virtual void UpdateComboBoxes()
//------------------------------------------------------------------------------
/**
 * Updates the comboboxes by removing libration points and solar system barycenter
 */
//------------------------------------------------------------------------------
void LibrationPointPanel::UpdateComboBoxes()
{
   unsigned int i;
   wxString pIndex = primaryBodyCB->GetStringSelection(); 
   wxString sIndex = secondaryBodyCB->GetStringSelection();

   primaryBodyCB->Clear();
   secondaryBodyCB->Clear();
   StringArray items;

   // list of celestial bodies points
   items = theGuiInterpreter->GetListOfObjects(Gmat::CELESTIAL_BODY);
   for (i=0; i<items.size(); i++)
   {
	   primaryBodyCB->Append(items[i].c_str());
	   secondaryBodyCB->Append(items[i].c_str());
   }

   // list of calculated points
   items = theGuiInterpreter->GetListOfObjects(Gmat::CALCULATED_POINT);
   for (i=0; i<items.size(); i++)
   {
		CalculatedPoint *calpt
			= (CalculatedPoint*)theGuiInterpreter->GetConfiguredObject(items[i]);
		wxString objName = wxString(items[i].c_str());
		wxString objTypeName = wxString(calpt->GetTypeName().c_str());

		// append barycenters to the primary and secondary body lists   
		if ((objTypeName == "Barycenter") && (objName != "SolarSystemBarycenter"))
		{
			primaryBodyCB->Append(objName);
			secondaryBodyCB->Append(objName);
		}
   }

   // reselect current selection if still there
   primaryBodyCB->SetStringSelection(pIndex);
   secondaryBodyCB->SetStringSelection(sIndex);
}


//------------------------------------------------------------------------------
// bool RefreshObjects(UnsignedInt type = Gmat::UNKNOWN_OBJECT)
//------------------------------------------------------------------------------
/**
 * Refreshes the comboboxes.  This is necessary because the comboboxes
 * must hold celestial bodies and barycenters
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool LibrationPointPanel::RefreshObjects(UnsignedInt type)
{
	if ((type == Gmat::CALCULATED_POINT) || (type == Gmat::CELESTIAL_BODY) ||
       (type == Gmat::SOLAR_SYSTEM))
   {
      // Update comboboxes with Barycenters
      UpdateComboBoxes();
      return true;
   }
   else
   {
      return GmatPanel::RefreshObjects(type);
   }
}

