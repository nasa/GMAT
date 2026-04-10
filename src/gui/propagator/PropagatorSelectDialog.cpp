//$Id$
//------------------------------------------------------------------------------
//                              PropagatorSelectDialog
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
// Created: 2004/10/19
//
/**
 * Implements PropagatorSelectDialog class. This class shows dialog window where
 * propagator can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "PropagatorSelectDialog.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROP_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatorSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON, PropagatorSelectDialog::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagatorSelectDialog(wxWindow *parent, const wxString &propName)
//------------------------------------------------------------------------------
PropagatorSelectDialog::PropagatorSelectDialog(wxWindow *parent,
                                               const wxString &propName)
   : GmatDialog(parent, -1, wxString(_T("PropagatorSelectDialog")))
{
   mHasSelectionChanged = false;
   mPropName = propName;
   mNewPropName = "";
   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// bool HasSelectionChanged()
//------------------------------------------------------------------------------
bool PropagatorSelectDialog::HasSelectionChanged()
{
   return mHasSelectionChanged;
}

//------------------------------------------------------------------------------
// wxString GetPropagatorName()
//------------------------------------------------------------------------------
wxString PropagatorSelectDialog::GetPropagatorName()
{
   return mNewPropName;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagatorSelectDialog::Create()
{
   int bsize = 2; // border size
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   //wxStaticText
   wxStaticText *mPropagatorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Available Propagator"),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   // wxButton
   mAddButton = new wxButton(this, ID_BUTTON, wxT("OK"),
                             wxDefaultPosition, wxDefaultSize, 0);
   mCancelButton = new wxButton(this, ID_BUTTON, wxT("Cancel"),
                                wxDefaultPosition, wxDefaultSize, 0);
   
   StringArray propList =
      theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
   int propCount = propList.size();
   
   if (propCount > 0)
   {
      wxString *propNameList = new wxString[propCount];

      for (int i=0; i<propCount; i++)
         propNameList[i] = propList[i].c_str();
      
      mPropagatorListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), propCount, propNameList, wxLB_SINGLE);
      delete [] propNameList;
   }
   else
   {
      mPropagatorListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150, 100),
                       emptyList, wxLB_SINGLE);
   }
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   // add buttons to sizer
   buttonsBoxSizer->Add(mAddButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(mCancelButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   pageBoxSizer->Add(mPropagatorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageBoxSizer->Add(mPropagatorListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   pageBoxSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

   // Hide the bottom size
   theBottomSizer->Show(theButtonSizer, false);
   theDialogSizer->Show(theBottomSizer, false);
   theDialogSizer->Layout();
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatorSelectDialog::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mAddButton)  
   {
      if (mPropagatorListBox->GetStringSelection() != mPropName)
      {
         mHasSelectionChanged = true;
         mNewPropName = mPropagatorListBox->GetStringSelection();
      }
   }
   else if (event.GetEventObject() == mCancelButton)  
   {
   }

   Close();
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void PropagatorSelectDialog::LoadData()
{
   mPropagatorListBox->SetStringSelection(mPropName);
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void PropagatorSelectDialog::SaveData()
{
}

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void PropagatorSelectDialog::ResetData()
{
}


