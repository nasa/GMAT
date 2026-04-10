//$Id$
//------------------------------------------------------------------------------
//                              CelesBodySelectDialog
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
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Implements CelesBodySelectDialog class. This class shows a dialog window on
 * which celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "CelesBodySelectDialog.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>

//#define DEBUG_CELESBODY_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelesBodySelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CelesBodySelectDialog::OnButton)
   EVT_LISTBOX(ID_LISTBOX, CelesBodySelectDialog::OnSelectBody)
   EVT_LISTBOX_DCLICK(ID_LISTBOX, CelesBodySelectDialog::OnListBoxDoubleClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude, ...
//------------------------------------------------------------------------------
/**
 * Creates the dialog (default constructor).
 *
 * @param <parent>              pointer to the parent window
 * @param <bodiesToExclude>     list of bodies to exclude when asking for
 *                              the celestial body list
 * @param <bodiesToHide>        list of bodies to hide on the list
 * @param <showCalPoints>       flag specifying whether or not to show the
 *                              calculated points (default is false)
 */
//------------------------------------------------------------------------------
CelesBodySelectDialog::CelesBodySelectDialog(wxWindow *parent,
                                             wxArrayString &bodiesToExclude,
                                             wxArrayString &bodiesToHide,
                                             bool showCalPoints)
   : GmatDialog(parent, -1, wxString(_T("CelesBodySelectDialog")))
{
   mBodyNames.Clear();
   mBodiesToHide.Clear();
   
   mBodiesToExclude = bodiesToExclude;
   mBodiesToHide = bodiesToHide;
   
   mIsBodySelected = false;
   mShowCalPoints = showCalPoints;
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~CelesBodySelectDialog()
//------------------------------------------------------------------------------
/**
 * Destroys the dialog (destructor).
 *
 */
//------------------------------------------------------------------------------
CelesBodySelectDialog::~CelesBodySelectDialog()
{
   #if DEBUG_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("CelesBodySelectDialog::~CelesBodySelectDialog() Unregistering GUI items\n");
   #endif
   
   // Unregister GUI components
   if (mShowCalPoints)
      theGuiManager->UnregisterListBox("CelestialPoint", mBodyListBox);
   else
      theGuiManager->UnregisterListBox("CelestialBody", mBodyListBox);
   
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and arranges the widgets for the dialog.
 *
 */
//------------------------------------------------------------------------------
void CelesBodySelectDialog::Create()
{
   int borderSize = 2;
   wxArrayString emptyList;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Celestial Body"));

   // body GridSizer
   wxBoxSizer *bodyGridSizer = new wxBoxSizer(wxHORIZONTAL);
   
   GmatStaticBoxSizer *bodyAvailableSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, GUI_ACCEL_KEY"Available Bodies");
   
   // Available celestial body ListBox
   if (mShowCalPoints)
   {
      mBodyListBox =
         theGuiManager->GetCelestialPointListBox(this, ID_LISTBOX, wxSize(150, 200),
                                                 &mBodiesToExclude);
   }
   else
   {
      mBodyListBox =
         theGuiManager->GetCelestialBodyListBox(this, ID_LISTBOX, wxSize(150, 200),
                                                &mBodiesToExclude);
   }
   mBodyListBox->SetToolTip(pConfig->Read(_T("AvailableBodiesHint")));
   bodyAvailableSizer->Add(mBodyListBox, 0, wxALIGN_CENTER|wxGROW, borderSize);
   
   // arrow buttons
   mAddBodyButton =
      new wxButton( this, ID_BUTTON, wxString("-" GUI_ACCEL_KEY ">"), wxDefaultPosition, wxDefaultSize, 0 );
   mAddBodyButton->SetToolTip(pConfig->Read(_T("AddBodyHint")));
   mRemoveBodyButton =
      new wxButton( this, ID_BUTTON, wxString(GUI_ACCEL_KEY"<-"), wxDefaultPosition, wxDefaultSize, 0 );
   mRemoveBodyButton->SetToolTip(pConfig->Read(_T("RemoveBodyHint")));
   mClearBodyButton =
      new wxButton( this, ID_BUTTON, wxString("<" GUI_ACCEL_KEY "="), wxDefaultPosition, wxDefaultSize, 0 );
   mClearBodyButton->SetToolTip(pConfig->Read(_T("ClearBodiesHint")));
   
   // add buttons to sizer
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   buttonsBoxSizer->Add(mAddBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(mRemoveBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   buttonsBoxSizer->Add(mClearBodyButton, 0, wxALIGN_CENTER|wxALL, borderSize);
   
   GmatStaticBoxSizer *bodySelectedSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, GUI_ACCEL_KEY"Selected Bodies");
   // selected celetestial body ListBox
   if (! mBodiesToExclude.IsEmpty())
   {
      Integer count = mBodiesToExclude.GetCount();
      wxArrayString selectedBodyList;
      
      for (Integer i = 0; i < count; i++)
         selectedBodyList.Add(mBodiesToExclude[i]);
      
      mBodySelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 200), selectedBodyList, wxLB_SINGLE | wxLB_SORT);
   }
   else
   {
      mBodySelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150, 200),
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   mBodySelectedListBox->SetToolTip(pConfig->Read(_T("SelectedBodiesHint")));
   bodySelectedSizer->Add(mBodySelectedListBox,0, wxALIGN_CENTER|wxGROW, borderSize);
   
   bodyGridSizer->Add(bodyAvailableSizer, 1, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, borderSize);
   bodyGridSizer->Add(bodySelectedSizer, 1, wxALIGN_CENTER|wxALL, borderSize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add( bodyGridSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
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
void CelesBodySelectDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == mAddBodyButton )  
   {
      wxString s = mBodyListBox->GetStringSelection();
      
      if (s.IsEmpty())
         return;
      
      int strId1 = mBodyListBox->FindString(s);
      int strId2 = mBodySelectedListBox->FindString(s);
      
      // if the string wasn't found in the second list, insert it
      if (strId2 == wxNOT_FOUND)
      {
         mBodySelectedListBox->Append(s);
         mBodyListBox->Delete(strId1);
         mBodySelectedListBox->SetStringSelection(s);
         
         // select first available body
         mBodyListBox->SetSelection(0);
         ShowBodyOption(s, true);
      }
   }
   else if ( event.GetEventObject() == mRemoveBodyButton )  
   {
      wxString s = mBodySelectedListBox->GetStringSelection();
        
      if (s.IsEmpty())
         return;

      #ifdef DEBUG_BODY_REMOVE
      MessageInterface::ShowMessage("Removing body: %s\n", s.c_str());
      #endif
      
      mBodyListBox->Append(s);
      int sel = mBodySelectedListBox->GetSelection();
      mBodySelectedListBox->Delete(sel);
   }
   else if ( event.GetEventObject() == mClearBodyButton )  
   {
      Integer count = mBodySelectedListBox->GetCount();
      
      if (count == 0)
         return;
      
      for (Integer i = 0; i < count; i++)
         mBodyListBox->Append(mBodySelectedListBox->GetString(i));
      
      mBodySelectedListBox->Clear();
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
void CelesBodySelectDialog::OnSelectBody(wxCommandEvent& event)
{
   ShowBodyOption(mBodySelectedListBox->GetStringSelection(), true);
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
void CelesBodySelectDialog::OnListBoxDoubleClick(wxCommandEvent& event)
{
   wxCommandEvent evt;
   if (event.GetEventObject() == mBodyListBox)
   {
      evt.SetEventObject(mAddBodyButton);
      OnButton(evt);
   }
   else if (event.GetEventObject() == mBodySelectedListBox)
   {
      evt.SetEventObject(mRemoveBodyButton);
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
void CelesBodySelectDialog::LoadData()
{
   if (! mBodiesToHide.IsEmpty())
   {
      // Check mBodyListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)mBodyListBox->GetCount(); j++)
         {
            std::string bName = mBodyListBox->GetString(j).WX_TO_STD_STRING;
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               mBodyListBox->Delete(j);
         }
      }
      
      // Check mBodySelectedListBox
      for (Integer i = 0; i < (Integer)mBodiesToHide.GetCount(); i++)
      {
         for (Integer j = 0; j < (Integer)mBodySelectedListBox->GetCount(); j++)
         {
            std::string bName = mBodySelectedListBox->GetString(j).WX_TO_STD_STRING;
            if (strcmp(mBodiesToHide[i].c_str(), bName.c_str()) == 0)
               mBodySelectedListBox->Delete(j);
         }    
      }
   }
   
   // Show body color
   mBodySelectedListBox->SetSelection(0);
   ShowBodyOption(mBodySelectedListBox->GetStringSelection(), true);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the widgets of the dialog back to the object.
 *
 */
//------------------------------------------------------------------------------
void CelesBodySelectDialog::SaveData()
{
   mBodyNames.Clear();
   for(unsigned int i=0; i<mBodySelectedListBox->GetCount(); i++)
      mBodyNames.Add(mBodySelectedListBox->GetString(i));
   
   mIsBodySelected = true;
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
/**
 * Resets flags for the dialog.
 *
 */
//------------------------------------------------------------------------------
void CelesBodySelectDialog::ResetData()
{
   mIsBodySelected = false;
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
void CelesBodySelectDialog::ShowBodyOption(const wxString &name, bool show)
{
   #if DEBUG_CELESBODY_DIALOG
   MessageInterface::ShowMessage
      ("CelesBodySelectDialog::ShowBodyOption() name=%s\n",
       name.c_str());
   #endif
   
   if (!name.IsSameAs(""))
      mSelBodyName = name;
}
