//$Id$
//------------------------------------------------------------------------------
//                              TankAndMixDialog
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
// Author: Darrel Conway
// Created: 2015/12/16
//
/**
 * Implements TankAndMixDialog class. This class shows a dialog window on
 * which celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "TankAndMixDialog.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>

//#define DEBUG_CELESBODY_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TankAndMixDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, TankAndMixDialog::OnButton)
   EVT_LISTBOX(ID_LISTBOX, TankAndMixDialog::OnSelectTank)
   EVT_LISTBOX_DCLICK(ID_LISTBOX, TankAndMixDialog::OnListBoxDoubleClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// TankAndMixDialog(wxWindow *parent, wxArrayString &selectedTanks, ...
//------------------------------------------------------------------------------
/**
 * Creates the dialog (default constructor).
 *
 * @param <parent>              pointer to the parent window
 * @param <selectedTanks>     list of bodies to exclude when asking for
 *                              the celestial body list
 */
//------------------------------------------------------------------------------
TankAndMixDialog::TankAndMixDialog(wxWindow *parent,
      wxArrayString &currentTanks, wxArrayDouble &currentMixes)
   : GmatDialog(parent, -1, wxString(_T("TankAndMixDialog")))
{
   selectedTanks = currentTanks;
   mixValues = currentMixes;
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~TankAndMixDialog()
//------------------------------------------------------------------------------
/**
 * Destroys the dialog (destructor).
 *
 */
//------------------------------------------------------------------------------
TankAndMixDialog::~TankAndMixDialog()
{
   #if DEBUG_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("TankAndMixDialog::~TankAndMixDialog() Unregistering GUI items\n");
   #endif
   
   // Unregister GUI components
   theGuiManager->UnregisterListBox("FuelTank", tankListBox, &selectedTanks);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and arranges the widgets for the dialog.
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::Create()
{
   int borderSize = 2;
   wxArrayString emptyList;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Tanks"));

   // body GridSizer
   wxBoxSizer *tankGridSizer = new wxBoxSizer(wxHORIZONTAL);
   
   GmatStaticBoxSizer *tankAvailableSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, GUI_ACCEL_KEY"Available Tanks");
   
   // Available celestial body ListBox
   tankListBox = theGuiManager->GetFuelTankListBox(this, ID_LISTBOX,
         wxSize(150, 200), &selectedTanks);
   tankListBox->SetToolTip(pConfig->Read(_T("AvailableTanksHint")));
   tankAvailableSizer->Add(tankListBox, 0, wxALIGN_CENTER|wxGROW, borderSize);
   
   // arrow buttons
   addTankButton =
      new wxButton( this, ID_BUTTON, wxString("-" GUI_ACCEL_KEY ">"),
            wxDefaultPosition, wxDefaultSize, 0 );
   addTankButton->SetToolTip(pConfig->Read(_T("AddTankHint")));

   removeTankButton =
      new wxButton( this, ID_BUTTON, wxString(GUI_ACCEL_KEY"<-"),
            wxDefaultPosition, wxDefaultSize, 0 );
   removeTankButton->SetToolTip(pConfig->Read(_T("RemoveTankHint")));

   clearTankButton =
      new wxButton( this, ID_BUTTON, wxString("<" GUI_ACCEL_KEY "="),
            wxDefaultPosition, wxDefaultSize, 0 );
   clearTankButton->SetToolTip(pConfig->Read(_T("ClearTanksHint")));
   
   // add buttons to sizer
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   buttonsBoxSizer->Add(addTankButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(removeTankButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(clearTankButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   GmatStaticBoxSizer *tankSelectedSizer =
      new GmatStaticBoxSizer(wxHORIZONTAL, this, GUI_ACCEL_KEY"Selected Tanks and Mixes");

   tankMixGrid = new wxGrid(this, ID_LISTBOX, wxDefaultPosition,
         wxSize(250, 200));
   tankMixGrid->SetToolTip(pConfig->Read(_T("TankMixValueHint")));
   tankMixGrid->CreateGrid(100, 2);
   tankMixGrid->SetColLabelValue(0,"Tank");
   tankMixGrid->SetColLabelValue(1,"Mix Factor");
   tankMixGrid->SetColSize(0, 150);
   tankMixGrid->SetColSize(1, 99);
   tankMixGrid->HideRowLabels();
   theRenderer = new wxGridCellFloatRenderer();
   theMixEditor = new wxGridCellFloatEditor();
   for (Integer i = 0; i < 100; ++i)
   {
      tankMixGrid->SetReadOnly(i, 0);
      tankMixGrid->SetCellRenderer(i, 1, theRenderer);
      tankMixGrid->SetCellEditor(i, 1, theMixEditor);
   }

   tankSelectedSizer->Add(tankMixGrid,0, wxALIGN_CENTER|wxGROW, borderSize);
   
   tankGridSizer->Add(tankAvailableSizer, 1, wxALIGN_CENTER|wxALL, borderSize);
   tankGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   tankGridSizer->Add(tankSelectedSizer, 1, wxALIGN_CENTER|wxALL, borderSize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add( tankGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user pushes a button on the dialog.
 *
 * @param <event>   the handled event
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == addTankButton )
   {
      wxString s = tankListBox->GetStringSelection();
      wxString t;
      
      if (s.IsEmpty())
         return;
      
      int strId1 = tankListBox->FindString(s);
      
      int strId2 = wxNOT_FOUND;
      int nextEmptyRow = wxNOT_FOUND;
      for (Integer i = 0; i < 100; ++i)
      {
         t = tankMixGrid->GetCellValue(i, 0);
         if (t == s)
         {
            strId2 = i;
            break;
         }
         if (t == "")
         {
            nextEmptyRow = i;
            break;
         }
      }

      // if the string wasn't found in the second list, insert it
      if (strId2 == wxNOT_FOUND)
      {
         tankMixGrid->SetCellValue(nextEmptyRow, 0, s);
         tankMixGrid->SetCellValue(nextEmptyRow, 1, "1.000000");
         tankMixGrid->SelectBlock(nextEmptyRow, 1, nextEmptyRow, 1);

         tankListBox->Delete(strId1);
         tankListBox->SetSelection(0);
         ShowTankOption(s, true);
      }
   }
   else if ( event.GetEventObject() == removeTankButton )
   {
      Integer row = tankMixGrid->GetGridCursorRow();
      MessageInterface::ShowMessage("Selected row %d\n", row);

      wxString s = tankMixGrid->GetCellValue(row, 0), t;
        
      if (s.IsEmpty())
         return;

      #ifdef DEBUG_BODY_REMOVE
         MessageInterface::ShowMessage("Removing tank: %s\n", s.mb_str().data());
      #endif

      tankListBox->Append(s);
      // DeleteRows fails, so move the subsequent rows up until we hit a blank
      do
      {
         if (row < 99)
         {
            s = tankMixGrid->GetCellValue(row+1, 0);
            t = tankMixGrid->GetCellValue(row+1, 1);
         }
         else
         {
            s = "";
            t = tankMixGrid->GetCellValue(row, 1);
         }

         tankMixGrid->SetCellValue(row, 0, s);
         tankMixGrid->SetCellValue(row, 1, t);
         ++row;
      } while (s != "");
   }
   else if ( event.GetEventObject() == clearTankButton )
   {
      for (Integer i = 0; i < 100; ++i)
      {
         wxString val = tankMixGrid->GetCellValue(i, 0);
         if (val == "")
            break;
         tankListBox->Append(val);
         tankMixGrid->SetCellValue(i, 0, "");
         tankMixGrid->SetCellValue(i, 1, "");
      }
   }
   
   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// void OnSelectBody(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects a body on the dialog.
 *
 * @param <event>   the handled event
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::OnSelectTank(wxCommandEvent& event)
{
   wxGridCellCoordsArray cells = tankMixGrid->GetSelectedCells();
   if (cells.size() > 0)
   {
      wxString s = tankMixGrid->GetCellValue(cells[0].GetRow(), cells[0].GetCol());
      ShowTankOption(s, true);
   }
}


//------------------------------------------------------------------------------
// void OnListBoxDoubleClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user double-clicks on a list box
 * on the dialog.
 *
 * @param <event>   the handled event
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::OnListBoxDoubleClick(wxCommandEvent& event)
{
   wxCommandEvent evt;
   if (event.GetEventObject() == tankListBox)
   {
      evt.SetEventObject(addTankButton);
      OnButton(evt);
   }
   else if (event.GetEventObject() == tankMixGrid)
   {
      evt.SetEventObject(removeTankButton);
      OnButton(evt);
   }
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the object onto the widgets of the dialog.
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::LoadData()
{
   if (!selectedTanks.IsEmpty())
   {
      // Check tankListBox
      for (Integer i = 0; i < (Integer)selectedTanks.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)tankListBox->GetCount(); j++)
         {
            std::string bName = tankListBox->GetString(j).WX_TO_STD_STRING;
            if (strcmp(selectedTanks[i].c_str(), bName.c_str()) == 0)
               tankListBox->Delete(j);
         }
         // Fill the grid
         tankMixGrid->SetCellValue(i, 0, selectedTanks[i]);
         tankMixGrid->SetCellValue(i, 1, wxString::Format(wxT("%f"), mixValues[i]));
      }
   }
   

   tankMixGrid->SelectBlock(0, 1, 0, 1);
   ShowTankOption(tankMixGrid->GetCellValue(0, 0), true);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the widgets of the dialog back to the object.
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::SaveData()
{
   tankNames.Clear();
   mixValues.clear();
   for(unsigned int i = 0; i < 100; ++i)
   {
      wxString s = tankMixGrid->GetCellValue(i, 0);
      if (s == "")
         break;
      tankNames.Add(s);

      Real value;
      if (!(tankMixGrid->GetCellValue(i, 1).ToDouble(&value)))
      {
         MessageInterface::ShowMessage("WARNING: Mix value was not read "
               "correctly for tank %s; using 1.0\n", s.mb_str().data());
         value = 1.0;
      }
      mixValues.Add(value);
   }
   
   hasTankChanged = true;
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
/**
 * Resets flags for the dialog.
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::ResetData()
{
   hasTankChanged = false;
}


//------------------------------------------------------------------------------
// void ShowBodyOption(const wxString &name, bool show = true)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating whether or not to show a body with the input name.
 *
 * @param <name>    name of the body
 * @param <show>    show this body?  (default is true)
 *
 */
//------------------------------------------------------------------------------
void TankAndMixDialog::ShowTankOption(const wxString &name, bool show)
{
   #if DEBUG_CELESBODY_DIALOG
   MessageInterface::ShowMessage
      ("TankAndMixDialog::ShowBodyOption() name=%s\n",
       name.c_str());
   #endif
   
   if (!name.IsSameAs(""))
      selectedTankName = name;
}


//------------------------------------------------------------------------------
// bool UpdateTankAndMixArrays(wxArrayString &currentTanks,
//       wxArrayDouble &currentMixes)
//------------------------------------------------------------------------------
/**
 * Retrieves the settings made on the dialog
 *
 * @param currentTanks List used for the tanks
 * @param currentMixes List used for the mix ration factors
 *
 * @return true on success, false if there was a size mismatch
 */
//------------------------------------------------------------------------------
bool TankAndMixDialog::UpdateTankAndMixArrays(wxArrayString &currentTanks,
      wxArrayDouble &currentMixes)
{
   bool retval = true;
   currentTanks = tankNames;
   currentMixes = mixValues;

   if (currentTanks.size() != currentMixes.size())
      retval = false;

   return retval;
}
