//$Id$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
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
// Author: Wendy C. Shoan (original author: Allison Greene 2005.07.06)
// Created: 2009.01.26
//
/**
 * Implements CelestialBodyPanel class.
 */
//------------------------------------------------------------------------------

#include "CelestialBodyPanel.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include <stdlib.h>


//#define DEBUG_PARAM_PANEL 1
//#define DEBUG_CELESBODY_SAVE 1

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelestialBodyPanel, GmatPanel)
   EVT_NOTEBOOK_PAGE_CHANGED (-1, CelestialBodyPanel::OnPageChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelestialBodyPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Creates the panel (default constructor).
 *
 * @param <parent>   pointer to the parent window
 * @param <name>     name for the panel
 */
//------------------------------------------------------------------------------
CelestialBodyPanel::CelestialBodyPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true)
{
   origCelestialBody = (CelestialBody*)
      theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   bodyName = name.c_str();
   
   if (origCelestialBody)
   {
      isUserDefined = origCelestialBody->IsUserDefined();
      
      Create();
      Show();
   }
}

//------------------------------------------------------------------------------
// ~CelestialBodyPanel()
//------------------------------------------------------------------------------
/**
 * The destructor.
 */
//------------------------------------------------------------------------------
CelestialBodyPanel::~CelestialBodyPanel()
{
   delete theCelestialBody;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and arranges the widgets for the panel.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::Create()
{
   // use a clone here
   theCelestialBody = (CelestialBody*) (origCelestialBody->Clone());

   try
   {
      cbNotebook = new wxNotebook(this, ID_NOTEBOOK, wxDefaultPosition, 
                   wxDefaultSize, wxGROW);
      cbNotebook->SetBackgroundColour(GetBackgroundColour());
      cbNotebook->SetForegroundColour(GetForegroundColour());
      
      // create properties, orbit, orientation, and visualization panels
      properties       = new CelestialBodyPropertiesPanel(this, cbNotebook, theCelestialBody);
      orbit            = new CelestialBodyOrbitPanel(this, cbNotebook, theCelestialBody);
      orientation      = new CelestialBodyOrientationPanel(this, cbNotebook, theCelestialBody);
      visualization    = new CelestialBodyVisualizationPanel(this, cbNotebook, theCelestialBody);
      
      // add panels to notebook
      cbNotebook->AddPage(properties, wxT("Properties"));
      cbNotebook->AddPage(orbit, wxT("Orbit"));
      cbNotebook->AddPage(orientation, wxT("Orientation"));
      cbNotebook->AddPage(visualization, wxT("Visualization"));
      
      theMiddleSizer->Add(cbNotebook, 1, wxGROW, 1);  // 3?
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Error creating the CelestialBodyPanel: %s\n",
                                   (be.GetFullMessage()).c_str());
   }
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the Celestial Body object onto the widgets.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::LoadData()
{
   try
   {
      properties->LoadData();
      orbit->LoadData();
      orientation->LoadData();
      visualization->LoadData();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CelestialBodyPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }

   mObject = origCelestialBody;
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves data from the widgets back to the celestial body object.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::SaveData()
{
   #if DEBUG_CELESBODY_SAVE
      MessageInterface::ShowMessage("in CBPanel, origBody = %p, theBody = %p\n",
            origCelestialBody, theCelestialBody);
   #endif
   
   canClose = true;
   
   if (properties->IsDataChanged())
   {
      properties->SaveData();
      canClose = canClose && properties->CanClosePanel();
   }
   if (orbit->IsDataChanged())
   {
      orbit->SaveData();
      canClose = canClose && orbit->CanClosePanel();
   }
   if (orientation->IsDataChanged())
   {
      orientation->SaveData();
      canClose = canClose && orientation->CanClosePanel();
   }
   if (visualization->IsDataChanged())
   {
      visualization->SaveData();
      canClose = canClose && visualization->CanClosePanel();
   }
   
   if (!canClose)   // why do this???? spacecraft panel did this ....
   {
      EnableUpdate(true);
      return;
   }
   // copy the current info into origCelestialBody
   origCelestialBody->Copy(theCelestialBody);
   EnableUpdate(false);

}

//------------------------------------------------------------------------------
// void OnPageChange(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user change the page.
 *
 * @param <event>  the handled event
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnPageChange(wxNotebookEvent &event)
{
   // Changed wxCommandEvent to wxNotebookEvent so that this method can be
   // called when user changes the page. (LOJ: 2014.10.03)
   
   int selectedPage = event.GetSelection();
   #ifdef DEBUG_PAGE_CHANGE
   MessageInterface::ShowMessage
      ("CelestialBodyPanel::OnPageChange() called, selection = %d\n", selectedPage);
   MessageInterface::ShowMessage
      ("   page: %s\n", cbNotebook->GetPageText(selectedPage).WX_TO_C_STRING);
   #endif
   
   // Commented out calling LoadData() since it is only needed when this panel is loaded.
   // Added Navigate() so that the first editable item is not highlighed with wx3.0
   // (Fix for GMT-4723 LOJ:2014.10.03)
   if (selectedPage == 0)
   {
      //properties->LoadData();
      properties->Navigate();
   }
   else if (selectedPage == 1)
   {
      //orbit->LoadData();
      orbit->Navigate();
   }
   else if (selectedPage == 2)
   {
      //orientation->LoadData();
      orientation->Navigate();
   }
   else if (selectedPage == 3)
   {
      visualization->Navigate();
   }
}

