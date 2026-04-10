//$Id$
//------------------------------------------------------------------------------
//                                 UndockedMissionPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2011/08/22
//
/**
 * Shows MissionTree panel
 */
//------------------------------------------------------------------------------
#include "UndockedMissionPanel.hpp"
#include "MessageInterface.hpp"
#include "TreeViewOptionDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatNotebook.hpp"
#include "MissionTree.hpp"
#include "MissionTreeToolBar.hpp"

//#define DEBUG_CREATE
//#define DEBUG_LOAD_DATA


BEGIN_EVENT_TABLE(UndockedMissionPanel, GmatPanel)
   EVT_CLOSE (UndockedMissionPanel::OnClose)
END_EVENT_TABLE()

   
//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// UndockedMissionPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
UndockedMissionPanel::UndockedMissionPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, false, false)
{
   mParent = parent;
   mGmatNotebook = NULL;
   
   mMissionTree = NULL;
   mMissionTreeToolBar = NULL;
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~UndockedMissionPanel()
//------------------------------------------------------------------------------
UndockedMissionPanel::~UndockedMissionPanel()
{
   #ifdef DEBUG_DESTRUCTOR
   MessageInterface::ShowMessage
      ("UndockedMissionPanel destructor entered, restoring mGmatNotebook=<%p>\n",
       mGmatNotebook);
   #endif
   
   // This dialog is deleted from the GmatNotebook::RestoreMissionPage()
   if (mGmatNotebook != NULL)
      mGmatNotebook->RestoreMissionPage();
}


//------------------------------------------------------------------------------
// MissionTree* GetMissionTree()
//------------------------------------------------------------------------------
MissionTree* UndockedMissionPanel::GetMissionTree()
{
   #ifdef DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("UndockedMissionPanel::GetMissionTree() returning <%p>\n", mMissionTree);
   #endif
   return mMissionTree;
}


//------------------------------------------------------------------------------
// MissionTreeToolBar* GetMissionToolBar()
//------------------------------------------------------------------------------
MissionTreeToolBar* UndockedMissionPanel::GetMissionToolBar()
{
   return mMissionTreeToolBar;
}


//------------------------------------------------------------------------------
// void SetGmatNotebook(GmatNotebook *notebook)
//------------------------------------------------------------------------------
/**
 * Sets GmatNotebook pointer so that when this panel is closed mission page
 * can be restored in the notebook
 */
//------------------------------------------------------------------------------
void UndockedMissionPanel::SetGmatNotebook(GmatNotebook *notebook)
{
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage
      ("UndockedMissionPanel::SetGmatNotebook() this<%p> entered, "
       "notebook=<%p>\n", this, notebook);
   #endif
   mGmatNotebook = notebook;
}


//------------------------------------------------------------------------------
// virtual void Create();
//------------------------------------------------------------------------------
void UndockedMissionPanel::Create()
{  
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("UndockedMissionPanel::Create() entered\n");
   #endif
   
   //---------------------------------------------
   // Create mission tree
   //---------------------------------------------
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;
   // If we allow the the user to be able to edit labels in the tree control. 
   // add wxTR_EDIT_LABELS;
   // @note - This option confuses with double click on the active node.
   // It opens the panel and tried to rename the node. So getting warning message.
   mMissionTree = new MissionTree(this, -1, wxDefaultPosition,
                                  wxSize(180,300), style);
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("   mMissionTree<%p> created\n", mMissionTree);
   #endif
   
   //---------------------------------------------
   // Create tool bar
   //---------------------------------------------
   mMissionTreeToolBar = 
      new MissionTreeToolBar(this, wxTB_VERTICAL | wxTB_FLAT);
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("   mMissionTreeToolBar<%p> created\n", mMissionTreeToolBar);
   #endif
   
   //---------------------------------------------
   // add to sizer
   //---------------------------------------------
   int bsize = 2;
   #ifndef __WXMAC__
      GmatStaticBoxSizer *treeSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
      treeSizer->Add(mMissionTree, 1, wxGROW|wxALL, bsize);
      GmatStaticBoxSizer *treeCtrlSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
      treeCtrlSizer->Add(mMissionTreeToolBar, 0, wxGROW|wxALL, bsize);
   #endif
   
   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   #ifndef __WXMAC__
      panelSizer->Add( treeSizer, 1, wxGROW|wxALIGN_CENTER|wxALL, 0 );
      panelSizer->Add( treeCtrlSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   #else
      panelSizer->Add( mMissionTree, 1, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   #endif
      
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(panelSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Fit(this);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void UndockedMissionPanel::LoadData()
{
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage("UndockedMissionPanel::LoadData() entered\n");
   MessageInterface::ShowMessage
      ("   Setting MissionTree<%p> to toolbar<%p> and appData<%p>\n", mMissionTree,
       mMissionTreeToolBar, GmatAppData::Instance());
   #endif
   
   mMissionTreeToolBar->SetMissionTree(mMissionTree);
   GmatAppData::Instance()->SetMissionTree(mMissionTree);
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage("   Adding default mission to tree\n");
   #endif
   
   mMissionTree->AddDefaultMission();
   mMissionTreeToolBar->SetMissionTreeExpandLevel(10);
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage("UndockedMissionPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void UndockedMissionPanel::SaveData()
{
   // Do nothing here
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void UndockedMissionPanel::OnClose(wxCloseEvent& event)
{
   #ifdef DEBUG_CLOSE
   MessageInterface::ShowMessage
      ("UndockedMissionPanel::OnClose() entered, mGmatNotebook=<%p>\n",
       mGmatNotebook);
   #endif
   
   // This panel is deleted from the GmatNotebook::RestoreMissionPage()
   if (mGmatNotebook != NULL)
      mGmatNotebook->RestoreMissionPage();
   
   event.Skip();
}


