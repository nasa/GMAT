//$Id$
//------------------------------------------------------------------------------
//                            TankPanel
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
// Author: Waka Waktola
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "TankPanel.hpp"
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"
#include <wx/config.h>

//#define DEBUG_TANK_PANEL

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(TankPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON, TankPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankPanel(GmatPanel *scPanel, wwxWindow *parent, Spacecraft *spacecraft)
//------------------------------------------------------------------------------
/**
 * Constructs TankPanel object.
 */
//------------------------------------------------------------------------------
TankPanel::TankPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft)
   : wxPanel(parent)
{
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::TankPanel() entered\n");
   #endif
   theScPanel = scPanel;
   theSpacecraft = spacecraft;
   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();

   dataChanged = false;
   Create();
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::TankPanel() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// ~TankPanel()
//------------------------------------------------------------------------------
TankPanel::~TankPanel()
{
   theGuiManager->UnregisterListBox("FuelTank", availableTankListBox,
                                    &mExcludedTankList);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TankPanel::Create()
{
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::Create() entered\n");
   #endif
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Tanks"));

   // wxButton
   selectButton = new wxButton( this, ID_BUTTON, wxString("-" GUI_ACCEL_KEY ">"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   selectButton->SetToolTip(pConfig->Read(_T("AddTankHint")));

   removeButton = new wxButton( this, ID_BUTTON, wxString(GUI_ACCEL_KEY"<-"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeButton->SetToolTip(pConfig->Read(_T("RemoveTankHint")));

   selectAllButton = new wxButton( this, ID_BUTTON, wxString("=>"),
                              wxDefaultPosition, wxDefaultSize, 0 );
   selectAllButton->SetToolTip(pConfig->Read(_T("AddAllTanksHint")));

   removeAllButton = new wxButton( this, ID_BUTTON, wxString("<" GUI_ACCEL_KEY "="),
                              wxDefaultPosition, wxDefaultSize, 0 );
   removeAllButton->SetToolTip(pConfig->Read(_T("ClearTanksHint")));

                              
   // wxString
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   if (theSpacecraft != NULL)
   {
      Integer paramID = theSpacecraft->GetParameterID("Tanks");
      StringArray tankNames = theSpacecraft->GetStringArrayParameter(paramID);
      
      int count = tankNames.size();
      #ifdef DEBUG_TANK_PANEL
      MessageInterface::ShowMessage("   Spacecraft attached with %d tanks\n", count);
      #endif
      for (int i=0; i<count; i++)
         mExcludedTankList.Add(tankNames[i].c_str());
   }
   
   availableTankListBox =
      theGuiManager->GetFuelTankListBox(this, ID_LISTBOX, wxSize(150,200),
                                        &mExcludedTankList);
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage
      ("   There are %d available tanks\n", availableTankListBox->GetCount());
   #endif
   availableTankListBox->SetToolTip(pConfig->Read(_T("AvailableTanksHint")));
   
   selectedTankListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150,200), //0,
                    emptyList, wxLB_SINGLE);
   selectedTankListBox->SetToolTip(pConfig->Read(_T("SelectedTanksHint")));
   
   Integer bsize = 3; // border size
   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Available Tanks") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Selected Tanks") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxHORIZONTAL );
   
   // Add to wx*Sizers   
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);    
   boxSizer2->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   staticBoxSizer1->Add(availableTankListBox, 1, wxALIGN_CENTER | wxEXPAND | wxALL, bsize);
   staticBoxSizer2->Add(selectedTankListBox, 1, wxALIGN_CENTER | wxEXPAND | wxALL, bsize);
   
   boxSizer3->Add(staticBoxSizer1, 1, wxALIGN_CENTER | wxEXPAND | wxALL, bsize);
   boxSizer3->Add( boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add(staticBoxSizer2, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, bsize);
   
   boxSizer1->Add(boxSizer3, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, bsize);
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   
   selectButton->Enable(true);
   removeButton->Enable(true);
   selectAllButton->Enable(true);
   removeAllButton->Enable(true);
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::Create() leaving\n");
   #endif
}    

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TankPanel::LoadData()
{
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::LoadData() entered\n");
   #endif
   
   if (theSpacecraft == NULL)
      return;
   
   // Load list of selected tanks
   Integer paramID = theSpacecraft->GetParameterID("Tanks");
   StringArray tankNames = theSpacecraft->GetStringArrayParameter(paramID);
   
   int count = tankNames.size();
   for (Integer i = 0; i < count; i++) 
      selectedTankListBox->Append(tankNames[i].c_str());
   
   dataChanged = false;
   #ifdef DEBUG_TANK_PANEL
   MessageInterface::ShowMessage("TankPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankPanel::SaveData()
{
   
   dataChanged = false;
   Integer paramID = 0;
    
   theSpacecraft->TakeAction("RemoveTank", "");
   int count = selectedTankListBox->GetCount();
    
   for (Integer i = 0; i < count; i++) 
   {        
      paramID = theSpacecraft->GetParameterID("Tanks");
      theSpacecraft->SetStringParameter(paramID,
         std::string(selectedTankListBox->GetString(i).c_str()));       
   }
}

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void TankPanel::OnButtonClick(wxCommandEvent &event)
{   
    if (event.GetEventObject() == selectButton)
    {
       wxString str = availableTankListBox->GetStringSelection();
       
       if (str.IsEmpty())
          return;
          
       int sel = availableTankListBox->GetSelection();
       int found = selectedTankListBox->FindString(str);
       
       if (found == wxNOT_FOUND)
       {
          selectedTankListBox->Append(str);
          availableTankListBox->Delete(sel);
          selectedTankListBox->SetStringSelection(str);
          mExcludedTankList.Add(str);
          
          if (sel-1 < 0)
             availableTankListBox->SetSelection(0);
          else
             availableTankListBox->SetSelection(sel-1);
      
       }
       
       dataChanged = true;
       theScPanel->EnableUpdate(true);
    }
    else if (event.GetEventObject() == removeButton)
    {
       wxString str = selectedTankListBox->GetStringSelection();
       
       if (str.IsEmpty())
          return;
          
       int sel = selectedTankListBox->GetSelection();
       
       selectedTankListBox->Delete(sel);
       availableTankListBox->Append(str);
       availableTankListBox->SetStringSelection(str);
       mExcludedTankList.Remove(str.c_str());
       
       if (sel-1 < 0)
          selectedTankListBox->SetSelection(0);
       else
          selectedTankListBox->SetSelection(sel-1);
   
       dataChanged = true;
       theScPanel->EnableUpdate(true);
    }   
    else if (event.GetEventObject() == selectAllButton)
    {
       Integer count = availableTankListBox->GetCount();
       
       if (count == 0)
          return;
       
       for (Integer i=0; i<count; i++)
       {
          selectedTankListBox->Append(availableTankListBox->GetString(i));
          mExcludedTankList.Add(availableTankListBox->GetString(i));
       }
       
       availableTankListBox->Clear();
       selectedTankListBox->SetSelection(0);
    
       dataChanged = true;
       theScPanel->EnableUpdate(true);
    } 
    else if (event.GetEventObject() == removeAllButton)
    {
       Integer count = selectedTankListBox->GetCount();
       
       if (count == 0)
          return;
       
       for (Integer i=0; i<count; i++)
       {
          availableTankListBox->Append(selectedTankListBox->GetString(i));
       }
       
       selectedTankListBox->Clear();
       mExcludedTankList.Clear();
       availableTankListBox->SetSelection(0);
   
       dataChanged = true;
       theScPanel->EnableUpdate(true);
    }
}     
    
