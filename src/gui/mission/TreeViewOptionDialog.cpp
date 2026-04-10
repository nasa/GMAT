//$Id$
//------------------------------------------------------------------------------
//                              TreeViewOptionDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2011/07/27
//
/**
 * Implements TreeViewOptionDialog class.
 */
//------------------------------------------------------------------------------

#include "TreeViewOptionDialog.hpp"
#include "MissionTree.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatNotebook.hpp"        // for ResetMissionTreeTools()
#include "wx/collpane.h"           // for wxCollapsiblePane

//#define DEBUG_DIALOG

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE (TreeViewOptionDialog, wxDialog)
   EVT_BUTTON (ID_BUTTON, TreeViewOptionDialog::OnButtonClick)
END_EVENT_TABLE()

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// TreeViewOptionDialog(wxWindow *parent, ...)
//------------------------------------------------------------------------------
/**
 * Constructs TreeViewOptionDialog object.
 *
 * @param  parent  parent window
 * @param  treeCtrl  MissionTree pointer
 * @param  id      window id
 * @param  title   window title
 *
 */
//------------------------------------------------------------------------------
TreeViewOptionDialog::TreeViewOptionDialog(wxWindow *parent, wxTreeCtrl *treeCtrl,
                                           const wxString& title, const wxPoint& pos,
                                           const wxSize& size, long style)
   : wxDialog(parent, -1, title, pos, size, style, title)
{
   mParent = parent;
   mTreeCtrl = treeCtrl;
   
   Create();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void TreeViewOptionDialog::Create()
{
   wxArrayString commands;
   if (mTreeCtrl)
      commands = ((MissionTree*)mTreeCtrl)->GetCommandList(true);
   
   //---------- View by level
   int bsize = 2;
   
   
   mCheckAllButton =
      new wxButton(this, ID_BUTTON, wxT("Check All"),
                   wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   mUncheckAllButton =
      new wxButton(this, ID_BUTTON, wxT("Uncheck All"),
                   wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   wxArrayString choices;
   choices.Add("Include");
   choices.Add("Exclude");
   
   mViewRadioBox =
      new wxRadioBox(this, ID_RADIOBOX, "Select Commands to", wxDefaultPosition, wxDefaultSize, choices);
      
   mViewCheckListBox =
      new wxCheckListBox(this, -1, wxDefaultPosition, wxSize(150, 250), commands,
                         wxLB_SINGLE|wxLB_SORT);
   
   mViewByCmdApplyButton =
      new wxButton(this, ID_BUTTON, wxT("Apply"),
                   wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   
   bsize = 5;
   wxFlexGridSizer *viewByCmdSizer = new wxFlexGridSizer(2);
   viewByCmdSizer->Add( mCheckAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   viewByCmdSizer->Add( mUncheckAllButton, 0, wxALIGN_CENTER|wxALL, bsize );
   viewByCmdSizer->Add( mViewRadioBox, 0, wxALIGN_CENTER|wxALL, bsize );
   viewByCmdSizer->AddSpacer(10);
   viewByCmdSizer->Add( mViewCheckListBox, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   viewByCmdSizer->Add( mViewByCmdApplyButton, 0, wxALIGN_CENTER|wxALL, bsize );
   
   GmatStaticBoxSizer *viewByCmdSizer1 = new GmatStaticBoxSizer( wxVERTICAL, this, "View By Commands" );
   viewByCmdSizer1->Add(viewByCmdSizer, 1, wxGROW|wxALIGN_CENTER||wxALL, bsize);
   
   //---------- add to page sizer
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   
   pageSizer->Add(viewByCmdSizer1, 1, wxGROW|wxALIGN_CENTER|wxALL, 2);
   
   pageSizer->AddSpacer(5);
   
   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles button click event
 */
//------------------------------------------------------------------------------
void TreeViewOptionDialog::OnButtonClick(wxCommandEvent &event)
{
   #ifdef DEBUG_DIALOG
   MessageInterface::ShowMessage("TreeViewOptionDialog::OnButtonClick() entered\n");
   #endif
   
   MissionTree *missionTree = (MissionTree*)mTreeCtrl;
   wxArrayString viewCmds;
   
   if (event.GetEventObject() == mCheckAllButton)
   {
      int count = mViewCheckListBox->GetCount();
      for (int i = 0; i < count; i++)
         mViewCheckListBox->Check(i, true);
   }
   else if (event.GetEventObject() == mUncheckAllButton)
   {
      int count = mViewCheckListBox->GetCount();
      for (int i = 0; i < count; i++)
         mViewCheckListBox->Check(i, false);
   }
   else if (event.GetEventObject() == mViewByCmdApplyButton)
   {
      int count = mViewCheckListBox->GetCount();
      int checkedCount = 0;
      bool exclude = false;
      if (mViewRadioBox->GetSelection() == 1)
         exclude = true;
      
      // Count for number of commands checked
      for (int i=0; i<count; i++)
         if (mViewCheckListBox->IsChecked(i))
            checkedCount++;
      
      wxString cmdStr;
      for (int i=0; i<count; i++)
      {
         cmdStr = mViewCheckListBox->GetString(i);
         if (cmdStr == "Equation")
            cmdStr = "GMAT";
         if (cmdStr == "ScriptEvent")
            cmdStr = "BeginScript";
         
         if (mViewCheckListBox->IsChecked(i))
         {
            if (!exclude)
            {
               #ifdef DEBUG_DIALOG
               MessageInterface::ShowMessage("   cmdStr='%s'\n", cmdStr.c_str());
               #endif
               
               viewCmds.Add(cmdStr);
            }
         }
         else
         {
            if (exclude)
               viewCmds.Add(cmdStr);
         }
      }
      
      if (missionTree != NULL)
         missionTree->SetViewCommands(viewCmds);
      
   }
}

