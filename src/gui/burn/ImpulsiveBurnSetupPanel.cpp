//$Id$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
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
// Author: LaMont Ruley
// Created: 2004/02/04
//
// Modifications:
//    2009.02.10  Linda Jun - Derived from the BurnThrusterPanel
//
/**
 * This class sets up the ImpulsiveBurn parameters
 */
//------------------------------------------------------------------------------

#include "ImpulsiveBurnSetupPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>


//#define DEBUG_BURNPANEL_LOAD

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs ImpulsiveBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <name> input burn name.
 */
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::ImpulsiveBurnSetupPanel(wxWindow *parent,
                                                 const wxString &name)
   : BurnThrusterPanel(parent, name)
{
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("ImpulsiveBurnSetupPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif
   
   useMixRatio = false;

   // To set panel object and show warning if object is NULL
   if (SetObject(theObject))
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~ImpulsiveBurnSetupPanel()
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::~ImpulsiveBurnSetupPanel()
{
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::Create()
{
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::Create() entered\n");
   #endif
   
   BurnThrusterPanel::Create();

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Impulsive Burn"));
   //-----------------------------------------------------------------
   // thrust vector items
   //-----------------------------------------------------------------
   XLabel->SetLabel("Element" GUI_ACCEL_KEY "1");
   YLabel->SetLabel("Element" GUI_ACCEL_KEY "2");
   ZLabel->SetLabel("Element" GUI_ACCEL_KEY "3");
   XUnitLabel->SetLabel(wxT("km/s"));
   YUnitLabel->SetLabel(wxT("km/s"));
   ZUnitLabel->SetLabel(wxT("km/s"));
   vectorBoxSizer->SetLabel(wxT("Delta-V Vector"));
   elem1TextCtrl->SetToolTip(pConfig->Read(_T("Element1Hint")));
   elem2TextCtrl->SetToolTip(pConfig->Read(_T("Element2Hint")));
   elem3TextCtrl->SetToolTip(pConfig->Read(_T("Element3Hint")));
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::LoadData() entered\n");
   #endif
   
   try
   {
      Integer paramID;
      paramID = theObject->GetParameterID("Isp");
      ispTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   BurnThrusterPanel::LoadData();
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::LoadData() entered\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData(GmatBase *theObject)
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::SaveData(GmatBase *theObject)
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   Real isp;
   Integer paramID;
   bool ispChanged = false;
   
   try 
   {
      if (ispTextCtrl->IsModified())
      {
         str = ispTextCtrl->GetValue();      
         CheckReal(isp, str, "Isp", "Real Number >= 0", false, true, true, true);
         ispChanged = true;
      }
      
      if (!canClose)
         return;
      
      if (ispChanged)
      {
         paramID = theObject->GetParameterID("Isp");
         theObject->SetRealParameter(paramID, isp);
      }

      BurnThrusterPanel::SaveData(theObject);
      
      // Validation checks mass depletion settings
      theObject->Validate();
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::SaveData() exiting\n");
   #endif
}
