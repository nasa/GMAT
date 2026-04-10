//$Id$
//------------------------------------------------------------------------------
//                             GmatNotebook
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
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the notebook for the left side of the main frame.
 */
//------------------------------------------------------------------------------
#include "GmatNotebook.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "UndockedMissionPanel.hpp"
#include "MessageInterface.hpp"

// Libpng-1.6 is more stringent about checking ICC profiles than previous versions.
// You can ignore the warning. To get rid of it, remove the iCCP chunk from the PNG image.
// For now just ignore warning (LOJ: 2014.09.23)
#define __IGNORE_PNG_WARNING__

//#define DEBUG_NOTEBOOK
//#define DEBUG_RESTORE
//#define DEBUG_UNDOCK_MISSION_PAGE

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatNotebook, wxNotebook)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatNotebook, wxNotebook)
   EVT_NOTEBOOK_PAGE_CHANGED (-1, GmatNotebook::OnNotebookSelChange)
   //EVT_NOTEBOOK_PAGE_CHANGING(-1, GmatNotebook::OnNotebookSelChanging)
   EVT_MOUSE_EVENTS(GmatNotebook::OnMouse)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatNotebook(wxWindow *parent, wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
GmatNotebook::GmatNotebook(wxWindow *parent, wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxNotebook(parent, id, pos, size, style)
{
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("GmatNotebook() this=<%p> entered\n", this);
   #endif
   mParent = parent;
   resourceTree = NULL;
   missionTree = NULL;
   outputTree = NULL;
   mMissionPagePanel = NULL;
   mUndockedMissionPanel = NULL;
   mMissionTreeToolBar = NULL;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
   // Create and add Resource, Mission, and Output Tabs
   wxPanel *panel = (wxPanel *)NULL;
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("   Creating Resource page\n");
   #endif
   panel = CreateResourcePage( );
   AddPage( panel, wxT("Resources") );
   
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("   Creating Mission page\n");
   #endif
   mMissionPagePanel = CreateMissionPage();
   AddPage( mMissionPagePanel, wxT("Mission") );
   // Set MissionTree to GmatAppData
   GmatAppData::Instance()->SetMissionTree(missionTree);
   missionTree->SetNotebook(this);
   // Add default mission to tree
   missionTree->AddDefaultMission();
   
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("   Creating Output page\n");
   #endif
   panel = CreateOutputPage( );
   AddPage( panel, wxT("Output") );
   
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("GmatNotebook() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual ~GmatNotebook()
//------------------------------------------------------------------------------
GmatNotebook::~GmatNotebook()
{
}


//------------------------------------------------------------------------------
// MissionTreeToolBar* GetMissionTreeToolBar()
//------------------------------------------------------------------------------
MissionTreeToolBar* GmatNotebook::GetMissionTreeToolBar()
{
   return mMissionTreeToolBar;
}


//------------------------------------------------------------------------------
// void SetMissionTreeExpandLevel(int level)
//------------------------------------------------------------------------------
void GmatNotebook::SetMissionTreeExpandLevel(int level)
{
   mMissionTreeToolBar->SetMissionTreeExpandLevel(level);
}


//------------------------------------------------------------------------------
// void CreateUndockedMissionPanel()
//------------------------------------------------------------------------------
void GmatNotebook::CreateUndockedMissionPanel()
{
   #ifdef DEBUG_UNDOCK_MISSION_PAGE
   MessageInterface::ShowMessage
      ("GmatNotebook::CreateUndockedMissionPanel() entered, creating MDI child "
       "UndockedMissionPanel through GmatMainFrame\n");
   MessageInterface::ShowMessage
      ("   mUndockedMissionPanel=<%p>\n", mUndockedMissionPanel);
   #endif
   
   #ifdef __IGNORE_PNG_WARNING__
   wxLogLevel logLevel = wxLog::GetLogLevel();
   wxLog::SetLogLevel(0);
   #endif
   
   // Create panel as MDIChildFrame
   GmatTreeItemData item("Mission", GmatTree::MISSION_TREE_UNDOCKED);
   item.SetTitle("Mission");
   mUndockedMissionPanel = (UndockedMissionPanel*)
      (GmatAppData::Instance()->GetMainFrame()->CreateChild(&item, true));
   
   // Set selection to resource page
   SetSelection(0);
   
   // Delete Mission page and reset pointers
   DeletePage(1);             
   mMissionPagePanel = NULL;
   missionTree = NULL;
   mMissionTreeToolBar = NULL;
   
   #ifdef __IGNORE_PNG_WARNING__
   wxLog::SetLogLevel(logLevel);
   #endif
   
   #ifdef DEBUG_UNDOCK_MISSION_PAGE
   MessageInterface::ShowMessage
      ("GmatNotebook::CreateUndockedMissionPanel() leaving, mUndockedMissionPanel=<%p>\n",
       mUndockedMissionPanel);
   #endif
}


//------------------------------------------------------------------------------
// void RestoreMissionPage()
//------------------------------------------------------------------------------
void GmatNotebook::RestoreMissionPage()
{
   #ifdef DEBUG_RESTORE
   MessageInterface::ShowMessage("GmatNotebook::RestoreMissionPage() entered\n");
   #endif
   
   #ifdef __IGNORE_PNG_WARNING__
   wxLogLevel logLevel = wxLog::GetLogLevel();
   wxLog::SetLogLevel(0);
   #endif
   
   if (mMissionPagePanel)
      delete mMissionPagePanel;
   if (missionTree)
      delete missionTree;
   if (mMissionTreeToolBar)
      delete mMissionTreeToolBar;
   
   // Create new mission tree, panel, and tool bar
   mMissionPagePanel = CreateMissionPage();
   InsertPage( 1, mMissionPagePanel, wxT("Mission") );
   SetSelection(1);
   GmatAppData *appData = GmatAppData::Instance();
   appData->SetMissionTree(missionTree);
   missionTree->SetNotebook(this);
   missionTree->SetMainFrame(appData->GetMainFrame());
   // Add default mission to tree
   //@todo show mission tree that matches user view control setting
   missionTree->AddDefaultMission();
   
   #ifdef DEBUG_RESTORE
   MessageInterface::ShowMessage
      ("   deleting mUndockedMissionPanel<%p>\n", mUndockedMissionPanel);
   #endif
   mUndockedMissionPanel = NULL;
   
   #ifdef __IGNORE_PNG_WARNING__
   wxLog::SetLogLevel(logLevel);
   #endif
   
   #ifdef DEBUG_RESTORE
   MessageInterface::ShowMessage("GmatNotebook::RestoreMissionPage() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// wxPanel *CreateResourcePage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with resource information to notebook.
 *
 * @return wxPanel object with resource information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateResourcePage()
{
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   wxPanel *panel = new wxPanel(this);
   
   //We don't want to edit labels
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;
   
   resourceTree = new ResourceTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);
      
   // set to GmatAppData
   GmatAppData::Instance()->SetResourceTree(resourceTree);
   
   sizer->Add( resourceTree, 0, wxGROW|wxALL, 0 );
   
   panel->SetAutoLayout( TRUE );
   panel->SetSizer( sizer );
   sizer->Fit( panel );
   sizer->SetSizeHints( panel );
   
   return panel;
}

//------------------------------------------------------------------------------
// wxPanel* CreateMissionPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with missions information to notebook.
 *
 * @return wxPanel object with missions information.
 */
//------------------------------------------------------------------------------
wxPanel* GmatNotebook::CreateMissionPage()
{
   //set the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("/GmatNotebook MissionPage"));
   
   mMissionPagePanel = new wxPanel(this);
   
   //---------------------------------------------
   // Create mission tree
   //---------------------------------------------
   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
         wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT | wxTR_ROW_LINES;
//         wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT |wxTR_EXTENDED | wxTR_ROW_LINES;
   // If we allow the the user to be able to edit labels in the tree control. 
   // add wxTR_EDIT_LABELS;
   // @note - This option confuses with double click on the active node.
   // It opens the panel and tried to rename the node. So getting warning message.
   missionTree = new MissionTree(mMissionPagePanel, -1, wxDefaultPosition,
                                 wxDefaultSize, style);
   
   //---------------------------------------------
   // Create tool bar
   //---------------------------------------------
   mMissionTreeToolBar = 
      new MissionTreeToolBar(mMissionPagePanel, wxTB_VERTICAL | wxTB_FLAT);
   mMissionTreeToolBar->SetMissionTree(missionTree);
   
   //---------------------------------------------
   // add to sizer
   //---------------------------------------------
   int bsize = 5;
   #ifndef __WXMAC__
      GmatStaticBoxSizer *treeCtrlSizer = new GmatStaticBoxSizer( wxVERTICAL, mMissionPagePanel, "" );
      treeCtrlSizer->Add(mMissionTreeToolBar, 0, wxGROW|wxBOTTOM, bsize);
   #endif
   
   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   panelSizer->Add( missionTree, 1, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   #ifndef __WXMAC__
      panelSizer->Add( treeCtrlSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 0 );
   #endif
   
   mMissionPagePanel->SetAutoLayout( TRUE );
   mMissionPagePanel->SetSizer( panelSizer );
   panelSizer->SetSizeHints( mMissionPagePanel );
   
   return mMissionPagePanel;
}


//------------------------------------------------------------------------------
// wxPanel *CreateOutputPage()
//------------------------------------------------------------------------------
/**
 * Adds a tree with output information to notebook.
 *
 * @return wxPanel object with output information.
 */
//------------------------------------------------------------------------------
wxPanel *GmatNotebook::CreateOutputPage()
{
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   wxPanel *panel = new wxPanel(this);

   long style = wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|
      wxSUNKEN_BORDER|wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT;

   outputTree = new OutputTree(panel, -1, wxDefaultPosition,
                                   wxDefaultSize, style);

   // set to GmatAppData
   GmatAppData::Instance()->SetOutputTree(outputTree);

   sizer->Add( outputTree, 0, wxGROW|wxALL, 0 );

   panel->SetAutoLayout( TRUE );
   panel->SetSizer( sizer );
   sizer->Fit( panel );
   sizer->SetSizeHints( panel );

   return panel;
}


//------------------------------------------------------------------------------
// void OnNoetbookSelChange(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChange(wxNotebookEvent &event)
{
   int sel = event.GetSelection();
   
   #ifdef DEBUG_NOTEBOOK
   MessageInterface::ShowMessage("GmatNotebook::OnNotebookSelChange sel=%d\n", sel);
   #endif
   
   if (sel == 0)
   {
      resourceTree->UpdateResource(false); //loj: 6/29/04 added false
   }
   
   event.Skip(); // need this

//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->MinimizeChildren(sel);
//      theMainFrame->Cascade();
//   }
}

//------------------------------------------------------------------------------
// void OnNoetbookSelChanging(wxNotebookEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles notebook page change
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnNotebookSelChanging(wxNotebookEvent &event)
{
//   int sel = event.GetSelection();
//
//   GmatMainFrame *theMainFrame = GmatAppData::GetMainFrame();
//
//   if (theMainFrame != NULL)
//   {
//      theMainFrame->Cascade();
//      theMainFrame->MinimizeChildren(sel);
//   }
//   else
      event.Skip(); // need this
}


//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxMouseEvent.
 */
//------------------------------------------------------------------------------
void GmatNotebook::OnMouse(wxMouseEvent& event)
{
   //if left mouse dragging
   if (event.Dragging() && event.RightIsDown())
   {
      // If current page is mission page, show dialog
      if (GetSelection() == 1)
      {
         if (mUndockedMissionPanel == NULL)
         {
            #ifdef DEBUG_UNDOCK_MISSION_PAGE
            MessageInterface::ShowMessage
               ("GmatNotebook::OnMouse() creating UndockedMissionPanel, current page = %d\n",
                GetSelection());
            #endif
            
            CreateUndockedMissionPanel();            
         }
         else
         {
            #ifdef DEBUG_UNDOCK_MISSION_PAGE
            MessageInterface::ShowMessage
               ("GmatNotebook::OnMouse() showing UndockedMissionPanel, current page = %d\n",
                GetSelection());
            #endif
         }
      }
   }
   
   event.Skip();
}

