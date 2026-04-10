//$Id$
//------------------------------------------------------------------------------
//                            ThrusterPanel
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
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ThrusterPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON, ThrusterPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterPanel(GmatPanel *scPanel, wwxWindow *parent, Spacecraft *spacecraft)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterPanel object.
 */
//------------------------------------------------------------------------------
ThrusterPanel::ThrusterPanel(GmatPanel *scPanel, wxWindow *parent,
                                Spacecraft *spacecraft)
   : wxPanel(parent)
{
   theScPanel = scPanel;
   theSpacecraft = spacecraft;
   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   Create();
}

//------------------------------------------------------------------------------
// ~ThrusterPanel()
//------------------------------------------------------------------------------
ThrusterPanel::~ThrusterPanel()
{
   theGuiManager->UnregisterListBox("Thruster", availableThrusterListBox,
                                    &mExcludedThrusterList);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterPanel::Create()
{
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxString("/Spacecraft Thrusters"));

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

                              
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   Integer paramID = theSpacecraft->GetParameterID("Thrusters");
   StringArray thrusterNames = theSpacecraft->GetStringArrayParameter(paramID);
   
   Integer count = thrusterNames.size();
   for (Integer i = 0; i < count; i++)
      mExcludedThrusterList.Add(thrusterNames[i].c_str());
   
   availableThrusterListBox =
      theGuiManager->GetThrusterListBox(this, ID_LISTBOX, wxSize(150,200),
                                        &mExcludedThrusterList);
   availableThrusterListBox->SetToolTip(pConfig->Read(_T("AvailableThrustersHint")));
   
   selectedThrusterListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150,200), //0,
                    emptyList, wxLB_SINGLE);
   selectedThrusterListBox->SetToolTip(pConfig->Read(_T("SelectedThrustersHint")));
                    
   Integer bsize = 3; // border size
   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Available Thrusters") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxString(GUI_ACCEL_KEY"Selected Thrusters") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxHORIZONTAL );

   // Add to wx*Sizers   
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);    
   boxSizer2->Add(selectButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer2->Add(selectAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   staticBoxSizer1->Add(availableThrusterListBox, 1, wxALIGN_CENTER | wxEXPAND | wxALL, bsize);
   staticBoxSizer2->Add(selectedThrusterListBox, 1, wxALIGN_CENTER | wxEXPAND | wxALL, bsize);
   
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
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterPanel::LoadData()
{
    if (theSpacecraft == NULL)
       return;

    // Load list of selected thrusters
    Integer paramID = theSpacecraft->GetParameterID("Thrusters");
    StringArray thrustersNames = theSpacecraft->GetStringArrayParameter(paramID);
     
    Integer count = thrustersNames.size();
    for (Integer i = 0; i < count; i++) 
        selectedThrusterListBox->Append(thrustersNames[i].c_str());

    dataChanged = false;
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterPanel::SaveData()
{
   dataChanged = false;
   Integer paramID = 0;
   
   theSpacecraft->TakeAction("RemoveThruster", "");
   Integer count = selectedThrusterListBox->GetCount();
   
   for (Integer i = 0; i < count; i++) 
   {        
      paramID = theSpacecraft->GetParameterID("Thrusters");
      theSpacecraft->SetStringParameter(paramID,
         std::string(selectedThrusterListBox->GetString(i).c_str()));       
   }  
}

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ThrusterPanel::OnButtonClick(wxCommandEvent &event)
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
       theScPanel->EnableUpdate(true);
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
       theScPanel->EnableUpdate(true);
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
       theScPanel->EnableUpdate(true);
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
       theScPanel->EnableUpdate(true);
    }   
}     
    
