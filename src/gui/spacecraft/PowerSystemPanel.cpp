//$Id$
//------------------------------------------------------------------------------
//                            PowerSystemPanel
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
// Author: Wendy Shoan
// Created: 2014.05.07
/**
 * This class contains information needed to setup users spacecraft power
 * system through GUI
 */
//------------------------------------------------------------------------------
#include <wx/config.h>
#include "PowerSystemPanel.hpp"
#include "GuiItemManager.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "bitmaps/OpenFolder.xpm"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(PowerSystemPanel, wxPanel)
   EVT_TEXT(ID_COMBOBOX,  PowerSystemPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PowerSystemPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs PowerSystemPanel object.
 *
 * @param <parent> Window parent.
 */
//------------------------------------------------------------------------------
PowerSystemPanel::PowerSystemPanel(GmatPanel *scPanel, wxWindow *parent,
                                   Spacecraft *spacecraft)
   : wxPanel(parent)
{
   theScPanel         = scPanel;
   theSpacecraft      = spacecraft;
   theGuiInterpreter  = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager      = GuiItemManager::GetInstance();

   canClose           = true;
   dataChanged        = false;
   powerSystemChanged = false;

   Create();
}

//------------------------------------------------------------------------------
// ~PowerSystemPanel()
//------------------------------------------------------------------------------
/**
 * Destructs a  PowerSystemPanel object.
 *
 */
//------------------------------------------------------------------------------
PowerSystemPanel::~PowerSystemPanel()
{
   theGuiManager->UnregisterComboBox("PowerSystem", powerSystemComboBox);
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// methods inherited from GmatPanel
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the page for ballistics and mass information.
 */
//------------------------------------------------------------------------------
void PowerSystemPanel::Create()
{

//   #if __WXMAC__
//   int buttonWidth = 40;
//   #else
//   int buttonWidth = 25;
//   #endif
//
//   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   Integer bsize = 2; // border size

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Power System"));

    //wxStaticBox *item2 = new wxStaticBox( this, -1, wxT("") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer(wxVERTICAL, this, "" );
   //wxBoxSizer      *item1   = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *item0   = //new wxStaticBoxSizer( wxHORIZONTAL );
      new wxBoxSizer(wxHORIZONTAL);

   // label for power system
   wxStaticText *powerSystemStaticText = new wxStaticText( this, ID_TEXT,
      "" GUI_ACCEL_KEY "Power System", wxDefaultPosition, wxDefaultSize, 0 );

   int pwrSysWidth = 170;
   #ifdef __WXMAC__
      pwrSysWidth = 178;
   #endif
   // combo box for the power system
   powerSystemComboBox = theGuiManager->GetPowerSystemComboBox(this, ID_COMBOBOX,
         wxDefaultSize);
   powerSystemComboBox->SetToolTip(pConfig->Read(_T("PowerSystemHint")));

   item0->Add( powerSystemStaticText, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   item0->Add( powerSystemComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   item1->Add( item0, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );

   this->SetAutoLayout( TRUE );
   this->SetSizer( item1 );
   //item0->Fit( this );
   //item0->SetSizeHints( this );
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data for the ballistics and mass panel.
 */
//------------------------------------------------------------------------------
void PowerSystemPanel::LoadData()
{
   try
   {
      std::string pwrSys = theSpacecraft->GetStringParameter("PowerSystem");
      powerSystemComboBox->SetValue(wxString(pwrSys.c_str()));
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }

   dataChanged        = false;
   powerSystemChanged = false;
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the ballistics and mass panel to the Spacecraft.
 */
//------------------------------------------------------------------------------
void PowerSystemPanel::SaveData()
{
   try
   {
      canClose    = true;
      std::string inputString;
      if (powerSystemChanged)
      {
         try
         {
            wxString pwrSysVal = powerSystemComboBox->GetValue();

            theSpacecraft->SetStringParameter("PowerSystem", pwrSysVal.c_str());
            powerSystemChanged = false;
         }
         catch (BaseException &ex)
         {
            canClose    = false;
            dataChanged = true;
            MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
         }
      }
      if (canClose)
         dataChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("PowerSystemPanel::SaveData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
      canClose = false;
      return;
   }
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Converts state to ComboBox selection using utility.
 *
 * @param <event> the wxCommandEvent to handle
 */
//------------------------------------------------------------------------------
void PowerSystemPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == powerSystemComboBox)
   {
      std::string pwrSysStr  = powerSystemComboBox->GetValue().WX_TO_STD_STRING;
      #ifdef DEBUG_POWER_SYSTEM_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("PowerSystemPanel::OnComboBoxChange() pwrSysStr=%s\n",
               pwrSysStr.c_str());
      #endif

      wxString pwrSysSelected = powerSystemComboBox->GetStringSelection().c_str();
      if (pwrSysSelected == "No Power System Selected")
         pwrSysSelected = "";

      // remove "No Power System Selected" once power system is selected
      int pos = powerSystemComboBox->FindString("No Power System Selected");
      if (pos != wxNOT_FOUND)
         powerSystemComboBox->Delete(pos);
      powerSystemChanged = true;
   }

   dataChanged = true;
   theScPanel->EnableUpdate(true);

   #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("PowerSystemPanel::OnComboBoxChange() leaving\n");
   #endif
}

