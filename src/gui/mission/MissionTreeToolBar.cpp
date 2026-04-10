//$Id$
//------------------------------------------------------------------------------
//                                 MissionTreeToolBar
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
// Created: 2011/08/24
//
/**
 * Shows MissionTree view control tool bar
 */
//------------------------------------------------------------------------------
#include "MissionTreeToolBar.hpp"
#include "MessageInterface.hpp"
#include "TreeViewOptionDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatNotebook.hpp"
#include "MissionTree.hpp"

#include "bitmaps/mtc_LA.xpm"
#include "bitmaps/mtc_L1.xpm"
#include "bitmaps/mtc_L2.xpm"
#include "bitmaps/mtc_L3.xpm"
#include "bitmaps/mtc_IncPhysics.xpm"
#include "bitmaps/mtc_IncSolver.xpm"
#include "bitmaps/mtc_IncScriptEvent.xpm"
#include "bitmaps/mtc_IncControlFlow.xpm"
#include "bitmaps/mtc_ExcReport.xpm"
#include "bitmaps/mtc_ExcEquation.xpm"
#include "bitmaps/mtc_ExcPlot.xpm"
#include "bitmaps/mtc_ExcCall.xpm"
#include "bitmaps/mtc_CustomView.xpm"
#include "bitmaps/mtc_ClearFilters.xpm"

//#define DEBUG_MORE_VIEW_OPTIONS
//#define DEBUG_MISSION_VIEW_CONTROL

BEGIN_EVENT_TABLE(MissionTreeToolBar, wxToolBar)
   EVT_TOOL (TOOL_CLEAR_FILTERS, MissionTreeToolBar::OnViewByCategory)
   EVT_TOOL_RANGE (TOOL_LEVEL_ALL, TOOL_LEVEL_3, MissionTreeToolBar::OnViewByLevel)
   EVT_TOOL_RANGE (TOOL_INC_PHYSICS, TOOL_EXC_CALL, MissionTreeToolBar::OnViewByCategory)
   EVT_TOOL (TOOL_CUSTOM_VIEW, MissionTreeToolBar::OnCustomView)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTreeToolBar(wxWindow *parent, long style, wxWindowID id, ...)
//------------------------------------------------------------------------------
MissionTreeToolBar::MissionTreeToolBar(wxWindow *parent, long style,
                                       wxWindowID id, const wxPoint& pos,
                                       const wxSize& size, const wxString& name)
   : wxToolBar(parent, id, pos, size, style, name)
{
   mParent = parent;   
   mMissionTree = NULL;
   
   CreateMissionTreeToolBar();
}


//------------------------------------------------------------------------------
// ~MissionTreeToolBar()
//------------------------------------------------------------------------------
MissionTreeToolBar::~MissionTreeToolBar()
{
}


//------------------------------------------------------------------------------
// MissionTree* GetMissionTree()
//------------------------------------------------------------------------------
MissionTree* MissionTreeToolBar::GetMissionTree()
{
   return mMissionTree;
}


//------------------------------------------------------------------------------
// void SetMissionTree(MissionTree *missionTree)
//------------------------------------------------------------------------------
void MissionTreeToolBar::SetMissionTree(MissionTree *missionTree)
{
   mMissionTree = missionTree;
}


//------------------------------------------------------------------------------
// void ClearFilters()
//------------------------------------------------------------------------------
void MissionTreeToolBar::ClearFilters()
{
   ResetMissionTreeTools();
   mMissionTree->SetViewLevel(10);
}


//------------------------------------------------------------------------------
// void ResetMissionTreeTools()
//------------------------------------------------------------------------------
void MissionTreeToolBar::ResetMissionTreeTools()
{
   ResetMissionTreeLevel();
   ResetMissionTreeIncludeCategory();
   ResetMissionTreeExcludeCategory();
   ToggleTool(TOOL_CLEAR_FILTERS, true);
}


//------------------------------------------------------------------------------
// void SetMissionTreeExpandLevel(int level)
//------------------------------------------------------------------------------
void MissionTreeToolBar::SetMissionTreeExpandLevel(int level)
{
   ResetMissionTreeTools();
   
   if (level == 1)
      ToggleTool(TOOL_LEVEL_1, true);
   else if (level == 2)
      ToggleTool(TOOL_LEVEL_2, true);
   else if (level == 3)
      ToggleTool(TOOL_LEVEL_3, true);
   //else
   //   ToggleTool(TOOL_LEVEL_ALL, true);
}


//------------------------------------------------------------------------------
// void CreateMissionTreeToolBar()
//------------------------------------------------------------------------------
void MissionTreeToolBar::CreateMissionTreeToolBar()
{
   // Load toolbar icons
   GuiItemManager *guiManager = GuiItemManager::GetInstance();
   const int NUM_ICONS = 14;
   wxBitmap* bitmaps[NUM_ICONS];
   int index = -1;
   long bitmapType = wxBITMAP_TYPE_PNG;
   
   // Do not change the order, this order is how it appears in the toolbar
   
   guiManager->LoadIcon("mtc_ClearFilters", bitmapType, &bitmaps[++index], mtc_ClearFilters_xpm);
   guiManager->LoadIcon("mtc_LA", bitmapType, &bitmaps[++index], mtc_LA_xpm);
   guiManager->LoadIcon("mtc_L1", bitmapType, &bitmaps[++index], mtc_L1_xpm);
   guiManager->LoadIcon("mtc_L2", bitmapType, &bitmaps[++index], mtc_L2_xpm);
   guiManager->LoadIcon("mtc_L3", bitmapType, &bitmaps[++index], mtc_L3_xpm);
   guiManager->LoadIcon("mtc_IncPhysics", bitmapType, &bitmaps[++index], mtc_IncPhysics_xpm);
   guiManager->LoadIcon("mtc_IncSolver", bitmapType, &bitmaps[++index], mtc_IncSolver_xpm);
   guiManager->LoadIcon("mtc_IncScriptEvent", bitmapType, &bitmaps[++index], mtc_IncScriptEvent_xpm);
   guiManager->LoadIcon("mtc_IncControlFlow", bitmapType, &bitmaps[++index], mtc_IncControlFlow_xpm);
   guiManager->LoadIcon("mtc_ExcReport", bitmapType, &bitmaps[++index], mtc_ExcReport_xpm);
   guiManager->LoadIcon("mtc_ExcEquation", bitmapType, &bitmaps[++index], mtc_ExcEquation_xpm);
   guiManager->LoadIcon("mtc_ExcPlot", bitmapType, &bitmaps[++index], mtc_ExcPlot_xpm);
   guiManager->LoadIcon("mtc_ExcCall", bitmapType, &bitmaps[++index], mtc_ExcCall_xpm);
   guiManager->LoadIcon("mtc_CustomView", bitmapType, &bitmaps[++index], mtc_CustomView_xpm);
   
   index = 0;
   AddCheckTool(TOOL_CLEAR_FILTERS, wxT("ClearFilters"), *bitmaps[index],
                                 *bitmaps[index], wxT("Show All"));
   AddSeparator();
   ++index;
   AddCheckTool(TOOL_LEVEL_ALL, wxT("LevelA"), *bitmaps[index],
                                 *bitmaps[index], wxT("Expand All Levels"));
   ++index;
   AddCheckTool(TOOL_LEVEL_1, wxT("Level1"), *bitmaps[index],
                                 *bitmaps[index], wxT("Expand One Level"));
   ++index;
   AddCheckTool(TOOL_LEVEL_2, wxT("Level2"), *bitmaps[index],
                                 *bitmaps[index], wxT("Expand Two Levels"));
   ++index;
   AddCheckTool(TOOL_LEVEL_3, wxT("Level3"), *bitmaps[index],
                                 *bitmaps[index], wxT("Expand Three Levels"));
   AddSeparator();
   ++index;
   AddCheckTool(TOOL_INC_PHYSICS, wxT("IncPhysics"), *bitmaps[index],
                                 *bitmaps[index], wxT("Include Physics Related commands"));
   ++index;
   AddCheckTool(TOOL_INC_SOLVER, wxT("IncSolver"), *bitmaps[index],
                                 *bitmaps[index], wxT("Include Solver Related commands"));
   ++index;
   AddCheckTool(TOOL_INC_SCRIPT, wxT("IncScript"), *bitmaps[index],
                                 *bitmaps[index], wxT("Include ScriptEvent"));
   ++index;
   AddCheckTool(TOOL_INC_CONTROL, wxT("IncControl"), *bitmaps[index],
                                 *bitmaps[index], wxT("Include Control Flow"));
   AddSeparator();
   ++index;
   AddCheckTool(TOOL_EXC_REPORT, wxT("ExcReport"), *bitmaps[index],
                                 *bitmaps[index], wxT("Exclude Report"));
   ++index;
   AddCheckTool(TOOL_EXC_EQUATION, wxT("ExcEquation"), *bitmaps[index],
                                 *bitmaps[index], wxT("Exclude Equation"));
   ++index;
   AddCheckTool(TOOL_EXC_PLOT, wxT("ExcPlot"), *bitmaps[index],
                                 *bitmaps[index], wxT("Exclude Toggle, PenUp/Down, ClearPlot, and MarkPoint commands"));
   ++index;
   AddCheckTool(TOOL_EXC_CALL, wxT("ExcCall"), *bitmaps[index],
                                 *bitmaps[index], wxT("Exclude CallFunction, Save commands"));
   // Commented out until GMT-2855 is resolved
   // Commented out button create code until Customize View Window is completely fixed
   // TGG: 2012-07-31
   //AddSeparator();
   //++index;
   //AddTool(TOOL_CUSTOM_VIEW, wxT("CustomizeView"), *bitmaps[index],
   //                         wxT("Customize Mission Tree View"));
   Realize();
   
   for (int i = 0; i < NUM_ICONS; i++)
      delete bitmaps[i];
}


//------------------------------------------------------------------------------
// void ResetMissionTreeLevel()
//------------------------------------------------------------------------------
void MissionTreeToolBar::ResetMissionTreeLevel()
{
   ToggleTool(TOOL_LEVEL_ALL, false);
   ToggleTool(TOOL_LEVEL_1, false);
   ToggleTool(TOOL_LEVEL_2, false);
   ToggleTool(TOOL_LEVEL_3, false);
   mLastLevelClicked = -1;
}


//------------------------------------------------------------------------------
// void ResetMissionTreeIncludeCategory()
//------------------------------------------------------------------------------
void MissionTreeToolBar::ResetMissionTreeIncludeCategory()
{
   ToggleTool(TOOL_INC_PHYSICS, false);
   ToggleTool(TOOL_INC_SOLVER, false);
   ToggleTool(TOOL_INC_SCRIPT, false);
   ToggleTool(TOOL_INC_CONTROL, false);
   mLastIncCategoryClicked = -1;
}


//------------------------------------------------------------------------------
// void ResetMissionTreeExcludeCategory()
//------------------------------------------------------------------------------
void MissionTreeToolBar::ResetMissionTreeExcludeCategory()
{
   ToggleTool(TOOL_EXC_REPORT, false);
   ToggleTool(TOOL_EXC_EQUATION, false);
   ToggleTool(TOOL_EXC_PLOT, false);
   ToggleTool(TOOL_EXC_CALL, false);
   mLastExcCategoryClicked = -1;
}


//------------------------------------------------------------------------------
// int GetOnlyOneClicked(int category)
//------------------------------------------------------------------------------
int MissionTreeToolBar::GetOnlyOneClicked(int category)
{
   int numClicked = 0;
   int onlyOneClicked = -1;
   
   if (category == 1)
   {
      if (GetToolState(TOOL_INC_PHYSICS))
      {
         onlyOneClicked = TOOL_INC_PHYSICS;
         numClicked++;
      }
      if (GetToolState(TOOL_INC_SOLVER))
      {
         onlyOneClicked = TOOL_INC_SOLVER;
         numClicked++;
      }
      if (GetToolState(TOOL_INC_SCRIPT))
      {
         onlyOneClicked = TOOL_INC_SCRIPT;
         numClicked++;
      }
      if (GetToolState(TOOL_INC_CONTROL))
      {
         onlyOneClicked = TOOL_INC_CONTROL;
         numClicked++;
      }
   }
   else
   {
      if (GetToolState(TOOL_EXC_REPORT))
      {
         onlyOneClicked = TOOL_EXC_REPORT;
         numClicked++;
      }
      if (GetToolState(TOOL_EXC_EQUATION))
      {
         onlyOneClicked = TOOL_EXC_EQUATION;
         numClicked++;
      }
      if (GetToolState(TOOL_EXC_PLOT))
      {
         onlyOneClicked = TOOL_EXC_PLOT;
         numClicked++;
      }
      if (GetToolState(TOOL_EXC_CALL))
      {
         onlyOneClicked = TOOL_EXC_CALL;
         numClicked++;
      }
   }
   
   #ifdef DEBUG_MISSION_VIEW_CONTROL
   MessageInterface::ShowMessage
      ("There are %d tools clicked, onlyOneClicked = %d\n", numClicked,
       onlyOneClicked);
   #endif
   
   if (numClicked == 1)
      return onlyOneClicked;
   else
      return -1;
}



//------------------------------------------------------------------------------
// void OnViewByLevel(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MissionTreeToolBar::OnViewByLevel(wxCommandEvent& event)
{
   int eventId = event.GetId();
   
   mLastLevelClicked = eventId;
   int viewLevel = 0;
   
   switch (eventId)
   {
   case TOOL_LEVEL_ALL:
      viewLevel = 10;
      break;
   case TOOL_LEVEL_1:
      viewLevel = 1;
      break;
   case TOOL_LEVEL_2:
      viewLevel = 2;
      break;
   case TOOL_LEVEL_3:
      viewLevel = 3;
      break;
   default:
      break;
   }
   // GMT-2924 changed behavior so all level buttons do not stay pressed
   // TGG: 2012-07-31
   ToggleTool(TOOL_CLEAR_FILTERS, true);
   ToggleTool(TOOL_LEVEL_ALL, false);
   ToggleTool(TOOL_LEVEL_1, false);
   ToggleTool(TOOL_LEVEL_2, false);
   ToggleTool(TOOL_LEVEL_3, false);
   
   ResetMissionTreeIncludeCategory();
   ResetMissionTreeExcludeCategory();
   mMissionTree->SetViewLevel(viewLevel);

}


//------------------------------------------------------------------------------
// void OnViewByCategory(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles event with filter by category tools. It constructs command array
 * as filter view commands tools are selected.
 */
//------------------------------------------------------------------------------
void MissionTreeToolBar::OnViewByCategory(wxCommandEvent& event)
{
   wxArrayString cmdsToInclude;

   ToggleTool(TOOL_LEVEL_ALL, false);
   ToggleTool(TOOL_LEVEL_1, false);
   ToggleTool(TOOL_LEVEL_2, false);
   ToggleTool(TOOL_LEVEL_3, false);

   // Expand Level, Include category and Exclude category are mutually exclusive.
   // So for example, if an include button is clicked and then an exclude
   // button is clicked it will toggle off the includ button.
   
   int eventId = event.GetId();
   
   #ifdef DEBUG_MISSION_VIEW_CONTROL
   MessageInterface::ShowMessage
      ("MissionTreeToolBar::OnViewByCategory() entered, eventId = %d, mLastIncCategoryClicked = %d, "
       "mLastExcCategoryClicked = %d\n", eventId, mLastIncCategoryClicked,
       mLastExcCategoryClicked);
   #endif
   
   switch (eventId)
   {
   case TOOL_CLEAR_FILTERS:
      // Stay toggled on if the last one reclicked      
      if (eventId == mLastIncCategoryClicked)
      {
         ToggleTool(eventId, true);
         return;
      }
      
	  ClearFilters();

      break;
   case TOOL_INC_PHYSICS:
   case TOOL_INC_SOLVER:
   case TOOL_INC_SCRIPT:
   case TOOL_INC_CONTROL:
      // Stay toggled on if the last one reclicked
      // Actually it should be toggled off and remove all filters which shows
      // all levels (LOJ: 2012.12.10)
      if (eventId == mLastIncCategoryClicked)
      {
         //ToggleTool(eventId, true);
         ToggleTool(eventId, false);
		 ClearFilters();
         return;
      }
      
      ResetMissionTreeExcludeCategory();
      ToggleTool(TOOL_CLEAR_FILTERS, false);
      
      // Include Physics based
      if (GetToolState(TOOL_INC_PHYSICS))
      {
         cmdsToInclude.Add("Propagate");
         cmdsToInclude.Add("Maneuver");
         cmdsToInclude.Add("BeginFiniteBurn");
         cmdsToInclude.Add("EndFiniteBurn");
      }
      
      // Include Solver related
      if (GetToolState(TOOL_INC_SOLVER))
      {
         cmdsToInclude.Add("Target");
         cmdsToInclude.Add("Optimize");
         cmdsToInclude.Add("Vary");
         cmdsToInclude.Add("Achieve");
         cmdsToInclude.Add("NonlinearConstraint");
         cmdsToInclude.Add("Minimize");
      }
      
      // Include ScriptEvent
      if (GetToolState(TOOL_INC_SCRIPT))
      {
         cmdsToInclude.Add("BeginScript");
      }
      
      // Include ControlFlow
      if (GetToolState(TOOL_INC_CONTROL))
      {
         cmdsToInclude.Add("If");
         cmdsToInclude.Add("While");
         cmdsToInclude.Add("For");
      }
      
      mLastIncCategoryClicked = GetOnlyOneClicked(1);
      mMissionTree->SetViewCommands(cmdsToInclude);
      break;
      
   case TOOL_EXC_REPORT:
   case TOOL_EXC_EQUATION:
   case TOOL_EXC_PLOT:
   case TOOL_EXC_CALL:
      // Stay toggled on if the same one reclicked      
      // Actually it should be toggled off and remove all filters which shows
      // all levels (LOJ: 2012.12.10)
      if (eventId == mLastExcCategoryClicked)
      {
         //ToggleTool(eventId, true);
         ToggleTool(eventId, false);
		 ClearFilters();
         return;
      }
      
      ResetMissionTreeIncludeCategory();
      ToggleTool(TOOL_CLEAR_FILTERS, false);
      cmdsToInclude = mMissionTree->GetCommandList(true);
      
      #ifdef DEBUG_MISSION_VIEW_CONTROL
      MessageInterface::ShowMessage
         ("Before exclusion: cmdsToInclude has %d commands\n", cmdsToInclude.GetCount());
      for (unsigned int i = 0; i < cmdsToInclude.size(); i++)
         MessageInterface::ShowMessage("   '%s'\n", cmdsToInclude[i].c_str());
      #endif
      
      // Exclude Report
      if (GetToolState(TOOL_EXC_REPORT))
         cmdsToInclude.Remove("Report");
      
      // Exclude Equation
      if (GetToolState(TOOL_EXC_EQUATION))
         cmdsToInclude.Remove("GMAT");
      
      // Exclude Subscriber related
      if (GetToolState(TOOL_EXC_PLOT))
      {
         cmdsToInclude.Remove("ClearPlot");
         cmdsToInclude.Remove("MarkPoint");
         cmdsToInclude.Remove("PenUp");
         cmdsToInclude.Remove("PenDown");
         cmdsToInclude.Remove("Toggle");
      }
      
      // Exclude CallGMATFunction, CallMatlabFunction, Save, SaveMission
      if (GetToolState(TOOL_EXC_CALL))
      {
         cmdsToInclude.Remove("CallGmatFunction");
         cmdsToInclude.Remove("CallMatlabFunction");
         cmdsToInclude.Remove("Save");
         cmdsToInclude.Remove("SaveMission");
      }
      
      mLastExcCategoryClicked = GetOnlyOneClicked(2);
      
      #ifdef DEBUG_MISSION_VIEW_CONTROL
      MessageInterface::ShowMessage
         (" After exclusion: cmdsToInclude has %d commands\n", cmdsToInclude.GetCount());
      for (unsigned int i = 0; i < cmdsToInclude.size(); i++)
         MessageInterface::ShowMessage("   '%s'\n", cmdsToInclude[i].c_str());
      MessageInterface::ShowMessage("==> Calling mMissionTree->SetViewCommands()\n");
      #endif
      
      mMissionTree->SetViewCommands(cmdsToInclude);
      break;
      
   default:
      break;
   }
   
   #ifdef DEBUG_MISSION_VIEW_CONTROL
   MessageInterface::ShowMessage
      ("MissionTreeToolBar::OnViewByCategory() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnCustomView(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MissionTreeToolBar::OnCustomView(wxCommandEvent& event)
{
   #ifdef DEBUG_MORE_VIEW_OPTIONS
   MessageInterface::ShowMessage("MissionTreeToolBar::OnCustomView() entered\n");
   #endif
   
   int grandpX = 0, grandpY = 0, grandpW = 0, grandpH = 0;
   int parentX = 0, parentY = 0, parentW = 0, parentH = 0;
   int x = 0, y = 0;
   
   if (mParent->GetParent() != NULL)
   {
      mParent->GetParent()->GetScreenPosition(&grandpX, &grandpY);
      mParent->GetParent()->GetSize(&grandpW, &grandpH);
   }
   
   mParent->GetScreenPosition(&parentX, &parentY);
   mParent->GetSize(&parentW, &parentH);
   
   #ifdef DEBUG_MORE_VIEW_OPTIONS
   MessageInterface::ShowMessage
      ("   grandpX = %3d, grandpY = %3d, grandpW = %3d, grandpH = %3d\n", grandpX, grandpY, grandpW, grandpH);
   MessageInterface::ShowMessage
      ("   parentX = %3d, parentY = %3d, parentW = %3d, parentH = %3d\n", parentX, parentY, parentW, parentH);
   #endif
   
   if (parentX == grandpX)
   {
      x = parentX + parentW + 9;
      y = parentY - 30;
   }
   else
   {
      x = parentX + grandpW;
      y = grandpY;
   }
   
   
   #ifdef DEBUG_MORE_VIEW_OPTIONS
   MessageInterface::ShowMessage("         x = %3d,       y = %3d\n", x, y);
   #endif
   
   ResetMissionTreeTools();
   TreeViewOptionDialog optionDlg(this, mMissionTree,
                                  "MissionTree Customize View",
                                  wxPoint(x, y), wxDefaultSize,
                                  wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE);
   
   optionDlg.ShowModal();
}

