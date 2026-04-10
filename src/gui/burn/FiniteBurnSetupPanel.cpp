//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
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
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "FiniteBurnSetupPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>

//#define DEBUG_FINITEBURN_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FiniteBurnSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON, FiniteBurnSetupPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FiniteBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs FiniteBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the Finite burn GUI
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::FiniteBurnSetupPanel(wxWindow *parent, 
   const wxString &burnName):GmatPanel(parent)
{
   theBurn = 
      (FiniteBurn*)theGuiInterpreter->GetConfiguredObject(burnName.c_str());
   
   canClose = true;

   Create();
   Show();
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// ~FiniteBurnSetupPanel()
//------------------------------------------------------------------------------
/**
 * Destroys FiniteBurnSetupPanel object.
 *
 */
//------------------------------------------------------------------------------
FiniteBurnSetupPanel::~FiniteBurnSetupPanel()
{
   theGuiManager->UnregisterListBox("Thruster", availableThrusterListBox,
                                    &mExcludedThrusterList);
}

//-------------------------------
// protected methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel for the Finite burn data
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::Create()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::Create() \n" );
   #endif

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Finite Burn Setup"));

   if (theBurn != NULL)
   {
      // wx*Sizers
      wxBoxSizer *thrusterSelectionSizer = new wxBoxSizer( wxHORIZONTAL );

      wxBoxSizer *availableSizer = new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *buttonSizer = new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *selectedSizer = new wxBoxSizer( wxVERTICAL );

      Integer paramID = theBurn->GetParameterID("Thrusters");
      StringArray thrusterNames = theBurn->GetStringArrayParameter(paramID);

      Integer count = thrusterNames.size();
      for (Integer i = 0; i < count; i++)
         mExcludedThrusterList.Add(thrusterNames[i].c_str());

      // Thruster list boxes
      wxSize fontSize = GetFont().GetPixelSize();
      availableThrusterListBox =
         theGuiManager->GetThrusterListBox(this, ID_LISTBOX, wxSize(150,200),
                                           &mExcludedThrusterList);
      availableThrusterListBox->SetToolTip(pConfig->Read(_T("AvailableThrustersHint")));
      availableSizer->Add(availableThrusterListBox, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 5);

      wxArrayString selectedList;
      for (Integer i = 0; i < count; i++)
         selectedList.Add(thrusterNames[i].c_str());
      selectedThrusterListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150,200), //0,
               selectedList, wxLB_SINGLE);
      selectedThrusterListBox->SetToolTip(pConfig->Read(_T("SelectedThrustersHint")));
      selectedSizer->Add(selectedThrusterListBox, 1, wxALIGN_CENTRE|wxEXPAND|wxALL, 5);


      // wxButton
      selectButton = new wxButton( this, ID_BUTTON, wxString("-" GUI_ACCEL_KEY ">"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      selectButton->SetToolTip(pConfig->Read(_T("AddThrusterHint")));

      removeButton = new wxButton( this, ID_BUTTON, wxString(GUI_ACCEL_KEY"<-"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      removeButton->SetToolTip(pConfig->Read(_T("RemoveThrusterHint")));

      selectAllButton = new wxButton( this, ID_BUTTON, wxString("=>"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      selectAllButton->SetToolTip(pConfig->Read(_T("AddAllThrustersHint")));

      removeAllButton = new wxButton( this, ID_BUTTON, wxString("<" GUI_ACCEL_KEY "="),
                                 wxDefaultPosition, wxDefaultSize, 0 );
      removeAllButton->SetToolTip(pConfig->Read(_T("ClearThrustersHint")));

      buttonSizer->Add(selectButton, 0, wxALIGN_CENTRE|wxALL, 5);
      buttonSizer->Add(removeButton, 0, wxALIGN_CENTRE|wxALL, 5);
      buttonSizer->Add(selectAllButton, 0, wxALIGN_CENTRE|wxALL, 5);
      buttonSizer->Add(removeAllButton, 0, wxALIGN_CENTRE|wxALL, 5);

      // Put all the sizers together
      thrusterSelectionSizer->Add(availableSizer, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 5);
      thrusterSelectionSizer->Add(buttonSizer, 0, wxALIGN_CENTRE|wxALL, 5);
      thrusterSelectionSizer->Add(selectedSizer, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 5);
      theMiddleSizer->Add(thrusterSelectionSizer, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 5);
   }
   else
   {
      // show error message
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:Create() theBurn is NULL\n");
   }
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::LoadData()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::LoadData() \n" );
   #endif
   
   // load data from the core engine 
   try
   {
      // Set object pointer for "Show Script"
      mObject = theBurn;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("FiniteBurnSetupPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::SaveData()
{
   #if DEBUG_FINITEBURN_PANEL
      MessageInterface::ShowMessage( "FiniteBurnSetupPanel::SaveData() \n" );
   #endif
      
   canClose = true;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save thrusters
      Integer id = theBurn->GetParameterID("Thrusters");
      theBurn->TakeAction("ClearThrusterList");
      Integer count = selectedThrusterListBox->GetCount();
      for (Integer i = 0; i < count; i++)
      {
         theBurn->SetStringParameter(id,
               selectedThrusterListBox->GetString(i).c_str(), i);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for the selection buttons
 *
 * @param event The triggerign event
 */
//------------------------------------------------------------------------------
void FiniteBurnSetupPanel::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetEventObject() == selectButton)
   {
      wxString str = availableThrusterListBox->GetStringSelection();

      if (str.IsEmpty())
         return;

      int sel = availableThrusterListBox->GetSelection();
      int found = selectedThrusterListBox->FindString(str);

      if (found == wxNOT_FOUND)
      {
         selectedThrusterListBox->Append(str);
         availableThrusterListBox->Delete(sel);
         selectedThrusterListBox->SetStringSelection(str);
         mExcludedThrusterList.Add(str);

         if (sel-1 < 0)
            availableThrusterListBox->SetSelection(0);
         else
            availableThrusterListBox->SetSelection(sel-1);

      }

      dataChanged = true;
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == removeButton)
   {
      wxString str = selectedThrusterListBox->GetStringSelection();

      if (str.IsEmpty())
         return;

      int sel = selectedThrusterListBox->GetSelection();

      selectedThrusterListBox->Delete(sel);
      availableThrusterListBox->Append(str);
      availableThrusterListBox->SetStringSelection(str);
      mExcludedThrusterList.Remove(str.c_str());

      if (sel-1 < 0)
         selectedThrusterListBox->SetSelection(0);
      else
         selectedThrusterListBox->SetSelection(sel-1);

      dataChanged = true;
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == selectAllButton)
   {
      Integer count = availableThrusterListBox->GetCount();

      if (count == 0)
         return;

      for (Integer i=0; i<count; i++)
      {
         selectedThrusterListBox->Append(availableThrusterListBox->GetString(i));
         mExcludedThrusterList.Add(availableThrusterListBox->GetString(i));
      }

      availableThrusterListBox->Clear();
      selectedThrusterListBox->SetSelection(0);

      dataChanged = true;
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == removeAllButton)
   {
      Integer count = selectedThrusterListBox->GetCount();

      if (count == 0)
         return;

      for (Integer i=0; i<count; i++)
      {
         availableThrusterListBox->Append(selectedThrusterListBox->GetString(i));
      }

      selectedThrusterListBox->Clear();
      mExcludedThrusterList.Clear();
      availableThrusterListBox->SetSelection(0);

      dataChanged = true;
      EnableUpdate(true);
   }
}
