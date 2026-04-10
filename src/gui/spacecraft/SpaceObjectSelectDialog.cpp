//$Id$
//------------------------------------------------------------------------------
//                              SpaceObjectSelectDialog
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
 * Implements SpaceObjectSelectDialog class. This class shows dialog window where
 * spacecraft object can be selected.
 * 
 */
//------------------------------------------------------------------------------

#include "SpaceObjectSelectDialog.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SO_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SpaceObjectSelectDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, SpaceObjectSelectDialog::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SpaceObjectSelectDialog(wxWindow *parent, wxArrayString &soSelList,
//                         wxArrayString &soExcList)
//------------------------------------------------------------------------------
SpaceObjectSelectDialog::SpaceObjectSelectDialog(wxWindow *parent,
                                                 wxArrayString &soSelList,
                                                 wxArrayString &soExcList)
   : GmatDialog(parent, -1, wxString(_T("SpaceObjectSelectDialog")))
{
   mSoNameList.Clear();
   mHasSelectionChanged = false;
   mSoSelList = soSelList;
   mSoExcList = soExcList;
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~SpaceObjectSelectDialog()
//------------------------------------------------------------------------------
SpaceObjectSelectDialog::~SpaceObjectSelectDialog()
{
   // Unregister GUI components
   #ifdef DEBUG_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("SpaceObjectSelectDialog::~SpaceObjectSelectDialog() unregistering "
       "SpaceObjectListBox:%d, ExcList:%d\n", spaceObjAvailableListBox, &mSoExcList);
   #endif
   
   theGuiManager->UnregisterListBox("SpaceObject", spaceObjAvailableListBox,
                                    &mSoExcList);
}


//------------------------------------------------------------------------------
// wxArrayString& GetSpaceObjectNames()
//------------------------------------------------------------------------------
wxArrayString& SpaceObjectSelectDialog::GetSpaceObjectNames()
{
   return mSoNameList;
}


//------------------------------------------------------------------------------
// bool HasSelectionChanged()
//------------------------------------------------------------------------------
bool SpaceObjectSelectDialog::HasSelectionChanged()
{
   return mHasSelectionChanged;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::Create()
{
   int bsize = 2; // border size
   int soSelCount = mSoSelList.Count();
   wxArrayString emptyList;
   wxString *tempList = NULL;
   
   #ifdef DEBUG_SO_DIALOG
   for (int i=0; i<soSelCount; i++)
   {
      MessageInterface::ShowMessage
         ("SpaceObjectSelectDialog::Create() mSoSelList[%d]=%s\n",
          i, mSoSelList[i].c_str());
   }
   #endif
   
   if (soSelCount > 0)
   {
      tempList = new wxString[soSelCount];
      for (int i=0; i<soSelCount; i++)
      {
         tempList[i] = mSoSelList[i];
      }
   }
   
   //wxStaticText
   wxStaticText *spaceObjStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Available SpaceObject"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *spaceObjSelectStaticText =
      new wxStaticText( this, ID_TEXT, wxT("SpaceObject Selected"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *emptyStaticText =
      new wxStaticText( this, ID_TEXT, wxT("  "),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxButton
   addSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("->"),
                                       wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
   removeSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("<-"),
                                          wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
   clearSpaceObjectButton = new wxButton( this, ID_BUTTON, wxT("<="),
       wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );

   #ifdef DEBUG_SO_DIALOG
   MessageInterface::ShowMessage
      ("SpaceObjectSelectDialog::Create() Calling GetSpaceObjectListBox\n");
   #endif
   
   // wxListBox
   spaceObjAvailableListBox =
      theGuiManager->GetSpaceObjectListBox(this, -1, wxSize(150, 100), &mSoExcList);
   
   if (soSelCount > 0)
   {
      spaceObjSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                       wxSize(150, 100), soSelCount, tempList, wxLB_SINGLE);
   }
   else
   {
      spaceObjSelectedListBox =
         new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(150, 100), //0,
                       emptyList, wxLB_SINGLE);
   }
   
   // wxSizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *spaceObjGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *buttonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   // add buttons to sizer
   buttonsBoxSizer->Add(addSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(removeSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   buttonsBoxSizer->Add(clearSpaceObjectButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 1st row
   spaceObjGridSizer->Add(spaceObjStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spaceObjGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   spaceObjGridSizer->Add(spaceObjSelectStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 2nd row
   spaceObjGridSizer->Add(spaceObjAvailableListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   spaceObjGridSizer->Add(buttonsBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   spaceObjGridSizer->Add(spaceObjSelectedListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   pageBoxSizer->Add( spaceObjGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   delete [] tempList;
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::OnButton(wxCommandEvent& event)
{
   if ( event.GetEventObject() == addSpaceObjectButton )  
   {
      //-----------------------------------
      // add spacecraft object
      //-----------------------------------
      int sel = spaceObjAvailableListBox->GetSelection();

      if (sel == wxNOT_FOUND)
         return;
      
      wxString str = spaceObjAvailableListBox->GetStringSelection();
      int strId = spaceObjSelectedListBox->FindString(str);
      
      #ifdef DEBUG_SO_DIALOG
      MessageInterface::ShowMessage
         ( "SpaceObjectSelectDialog::OnButton() sel=%d, strId=%d, adding '%s'\n",
           sel, strId, str.c_str() );
      #endif
      
      // if the string wasn't found in the second list, insert it
      if (strId == wxNOT_FOUND)
      {
         spaceObjSelectedListBox->Append(str);
         spaceObjAvailableListBox->Delete(sel);
         spaceObjSelectedListBox->SetStringSelection(str);
         
         // select next available item 
         if (sel-1 < 0)
            spaceObjAvailableListBox->SetSelection(0);
         else
            spaceObjAvailableListBox->SetSelection(sel-1);
      }
      
      EnableUpdate(true);
   }
   else if ( event.GetEventObject() == removeSpaceObjectButton )  
   {
      //-----------------------------------
      // remove spacecraft object
      //-----------------------------------
      int sel = spaceObjSelectedListBox->GetSelection();
      wxString str = spaceObjSelectedListBox->GetStringSelection();
      
      #ifdef DEBUG_SO_DIALOG
      MessageInterface::ShowMessage
         ( "SpaceObjectSelectDialog::OnButton() removing <%s>\n", str.c_str() );
      #endif
      
      // if item selected
      if (sel != wxNOT_FOUND)
      {
         spaceObjSelectedListBox->Delete(sel);
         spaceObjAvailableListBox->Append(str);
         spaceObjAvailableListBox->SetStringSelection(str);
         
         if (sel-1 < 0)
            spaceObjSelectedListBox->SetSelection(0);
         else
            spaceObjSelectedListBox->SetSelection(sel-1);
         
         EnableUpdate(true);
      }
   }
   else if ( event.GetEventObject() == clearSpaceObjectButton )  
   {
      #ifdef DEBUG_SO_DIALOG
      MessageInterface::ShowMessage
         ( "SpaceObjectSelectDialog::OnButton() clearing objects\n" );
      #endif
      
      //-----------------------------------
      // clear spacecraft object
      //-----------------------------------
      for(unsigned int i=0; i<spaceObjSelectedListBox->GetCount(); i++)
         spaceObjAvailableListBox->Append(spaceObjSelectedListBox->GetString(i));
      
      spaceObjSelectedListBox->Clear();
      spaceObjAvailableListBox->SetSelection(0);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::LoadData()
{
   spaceObjSelectedListBox->SetSelection(0);
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::SaveData()
{
   #ifdef DEBUG_SO_DIALOG
   MessageInterface::ShowMessage
      ("SpaceObjectSelectDialog::SaveData() selected item count = %d\n",
       spaceObjSelectedListBox->GetCount());
   #endif
   
   mSoNameList.Clear();
   for(unsigned int i=0; i<spaceObjSelectedListBox->GetCount(); i++)
   {
      mSoNameList.Add(spaceObjSelectedListBox->GetString(i));
      
      #ifdef DEBUG_SO_DIALOG
      MessageInterface::ShowMessage
         ("SpaceObjectSelectDialog::SaveData() item = <%s>\n", mSoNameList[i].c_str());
      #endif
   }
   
   mHasSelectionChanged = true;
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void SpaceObjectSelectDialog::ResetData()
{
   mHasSelectionChanged = false;
}

