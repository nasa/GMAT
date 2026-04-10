//$Id$
//------------------------------------------------------------------------------
//                              MissionTree
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
// Created: 2003/09/02
// Modified:
//    2011.12.13 Linda Jun
//       - Modified to automatically generate menu ids and command counter
//    
/**
 * This class provides the tree for missions.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
//#include "bitmaps/folder.xpm"
#include "bitmaps/ClosedFolder.xpm"
#include "bitmaps/OpenFolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/propagateevent.xpm"
#include "bitmaps/target.xpm"
#include "bitmaps/whileloop.xpm"
#include "bitmaps/forloop.xpm"
#include "bitmaps/if.xpm"
#include "bitmaps/scriptevent.xpm"
#include "bitmaps/varyevent.xpm"
#include "bitmaps/achieveevent.xpm"
#include "bitmaps/deltav.xpm"
// Call Function
#include "bitmaps/mt_CallGmatFunction.xpm"
#include "bitmaps/mt_CallMatlabFunction.xpm"

#include "bitmaps/nestreturn.xpm"
#include "bitmaps/saveobject.xpm"
#include "bitmaps/equalsign.xpm"
#include "bitmaps/toggle.xpm"
#include "bitmaps/beginfb.xpm"
#include "bitmaps/endfb.xpm"
#include "bitmaps/report.xpm"
#include "bitmaps/mt_WriteCommand.xpm"
#include "bitmaps/penup.xpm"
#include "bitmaps/pendown.xpm"
#include "bitmaps/optimize.xpm"
#include "bitmaps/mt_Default.xpm"
#include "bitmaps/mt_Stop.xpm"
#include "bitmaps/mt_SetCommand.xpm"
#include "bitmaps/mt_MarkPoint.xpm"
#include "bitmaps/mt_ClearPlot.xpm"
#include "bitmaps/mt_Global.xpm"
#include "bitmaps/mt_SaveMission.xpm"
#include "bitmaps/mt_Minimize.xpm"
#include "bitmaps/mt_NonlinearConstraint.xpm"
#include "bitmaps/mt_RunSimulator.xpm"
#include "bitmaps/mt_RunEstimator.xpm"
#include "bitmaps/mt_FindEvents.xpm"   

#include <wx/tipwin.h>

#include "MissionTree.hpp"
#include "MissionTreeItemData.hpp"
#include "ViewTextFrame.hpp"
#include "ShowSummaryDialog.hpp"

#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "GmatNotebook.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"         // for GetNextCommand()
#include "StringUtil.hpp"          // for GmatStringUtil::
#include "Propagate.hpp"           // for GetSegmentOrbitColor()
#include "RgbColor.hpp"

// Should we sort the command list?
#define __SORT_COMMAND_LIST__

// If we are ready to handle GUI Navigation plugins, enable this
//#define __ENABLE_NAV_GUI__

// Should we enable multiple mission sequence?
//#define__ENABLE_MULTIPLE_SEQUENCE__

// If actions to be saved and playback for testing
// Define this by setting the compiler parameter -D
//#define __TEST_MISSION_TREE_ACTIONS__

#ifdef __TEST_MISSION_TREE_ACTIONS__
#include <fstream>
#include "StringTokenizer.hpp"
//#define DEBUG_MISSION_TREE_ACTIONS
#endif

// For debug output
//#define DEBUG_MISSION_TREE_SHOW_CMD 1
//#define DEBUG_APPEND_COMMAND 1
//#define DEBUG_MISSION_TREE_APPEND 1
//#define DEBUG_MISSION_TREE_INSERT 1
//#define DEBUG_MISSION_TREE_DELETE 1
//#define DEBUG_MISSION_TREE_CHILD 1
//#define DEBUG_MISSION_TREE_FIND 2
//#define DEBUG_FIND_ITEM_PARENT 2
//#define DEBUG_MISSION_TREE_MENU 1
//#define DEBUG_MISSION_TREE 1
//#define DEBUG_ADD_ICONS
//#define DEBUG_COMMAND_LIST
//#define DEBUG_BUILD_TREE_ITEM 1
//#define DEBUG_VIEW_COMMANDS 1
//#define DEBUG_VIEW_LEVEL
//#define DEBUG_COMMAND_COUNTER
//#define DEBUG_RENAME
//#define DEBUG_NODE_NAME
//#define DEBUG_UPDATE_GUI_MANAGER
//#define DEBUG_CMD_SUMMARY

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(MissionTree, wxTreeCtrl)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MissionTree, wxTreeCtrl)
   EVT_PAINT(DecoratedTree::OnPaint)
   //EVT_UPDATE_UI(-1, DecoratedTree::OnPaint)
   //ag: 10/20/2004 Commented out so that the position of the click is not
   // checked to open up a panel from the variables/goals boxes
   //loj: 11/4/04 Uncommented so that double click on Target/If/For/While folder
   // will not collapse
   
   // wxMouseEvent event
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)
   
   // wxTreeEvent
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, MissionTree::OnBeginEditLabel)
   EVT_TREE_END_LABEL_EDIT(-1, MissionTree::OnEndEditLabel)
   
   // wxCommandEvent
   EVT_MENU(MT_OPEN, MissionTree::OnOpen)
   EVT_MENU(MT_CLOSE, MissionTree::OnClose)
   
   EVT_MENU(MT_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   EVT_MENU(MT_APPEND, MissionTree::OnPopupAppend)
   
   EVT_MENU_RANGE(MT_BEGIN_APPEND, MT_END_APPEND, MissionTree::OnAppend)
   EVT_MENU_RANGE(MT_BEGIN_INSERT_BEFORE, MT_END_INSERT_BEFORE, MissionTree::OnInsertBefore)
   EVT_MENU_RANGE(MT_BEGIN_INSERT_AFTER, MT_END_INSERT_AFTER, MissionTree::OnInsertAfter)
   
   EVT_MENU(MT_COLLAPSE, MissionTree::OnCollapse)
   EVT_MENU(MT_EXPAND, MissionTree::OnExpand)
   
   EVT_MENU(MT_RUN, MissionTree::OnRun)
   
   EVT_MENU(MT_RENAME, MissionTree::OnRename)
   EVT_MENU(MT_DELETE, MissionTree::OnDelete)

   EVT_MENU(MT_SHOW_DETAIL, MissionTree::OnShowDetail)
   EVT_MENU(MT_SHOW_SCRIPT, MissionTree::OnShowScript)
   EVT_MENU(MT_SHOW_MISSION_SEQUENCE, MissionTree::OnShowMissionSequence)
   EVT_MENU(MT_COMMAND_SUMMARY, MissionTree::OnShowCommandSummary)
   EVT_MENU(MT_MISSION_SUMMARY_ALL, MissionTree::OnShowMissionSummaryAll)
   EVT_MENU(MT_MISSION_SUMMARY_PHYSICS, MissionTree::OnShowMissionSummaryPhysics)
   
   EVT_MENU_RANGE(MT_DOCK_MISSION_TREE, MT_UNDOCK_MISSION_TREE,
                  MissionTree::OnDockUndockMissionTree)
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   EVT_MENU(MT_START_SAVE_ACTIONS, MissionTree::OnStartSaveActions)
   EVT_MENU(MT_STOP_SAVE_ACTIONS, MissionTree::OnStopSaveActions)
   EVT_MENU(MT_READ_ACTIONS, MissionTree::OnPlaybackActions)
   #endif
   
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTree(wxWindow *parent, const wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTree object.
 *
 * @param <parent> input parent.
 * @param <id> input id.
 * @param <pos> input position.
 * @param <size> input size.
 * @param <style> input style.
 *
 * @note Creates the tree for missions and adds a default mission.
 */
//------------------------------------------------------------------------------
MissionTree::MissionTree(wxWindow *parent, const wxWindowID id,
                         const wxPoint &pos, const wxSize &size, long style)
   : DecoratedTree(parent, id, pos, size, style),
     inScriptEvent(false),
     inFiniteBurn(false),
     mShowDetailedItem(false)
{
   mParent = parent;
   theNotebook = NULL;
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   SetFont(*wxNORMAL_FONT);
   mViewCommands.Add("All");
   mViewAll = true;
   mUsingViewLevel = true;
   mIsMissionTreeDocked = true;
   mEnableShowScript = true;
   mWriteMissionSeq = false;
   mViewLevel = 10;
   
   // Set mWriteMissionSeq to true if debugging or
   // DEBUG_MISSION_TREE = ON in the startup file.
   #ifdef DEBUG_MISSION_TREE_SHOW_CMD
   mWriteMissionSeq = true;
   #endif
   
   if (GmatGlobal::Instance()->IsMissionTreeDebugOn())
      mWriteMissionSeq = true;
   
   //-----------------------------------------------------------------
   // Create command id map
   //-----------------------------------------------------------------
   
   StringArray cmds = theGuiInterpreter->GetListOfViewableCommands();
   #ifdef DEBUG_COMMAND_LIST
   GmatStringUtil::WriteStringArray
      (cmds, "===> Here is the viewable command list", "   ");
   #endif
   for (unsigned int i = 0; i < cmds.size(); i++)
   {
      #ifndef __ENABLE_NAV_GUI__
      if (cmds[i] == "RunSimulator" || cmds[i] == "RunEstimator" || cmds[i] == "RunSmoother")
         continue;
      #endif
      mCommandList.Add(cmds[i].c_str());
   }
   
   // Build commands for view control since MissionTree show ControlFlow commands
   // and Vary, Achieve in sub nodes such as ControlLogic and Target node.
   mCommandListForViewControl = mCommandList;
   mCommandListForViewControl.Add("For");
   mCommandListForViewControl.Add("If");
   mCommandListForViewControl.Add("Else");
   mCommandListForViewControl.Add("While");
   mCommandListForViewControl.Add("Achieve");
   mCommandListForViewControl.Add("Vary");
   mCommandListForViewControl.Add("Minimize");
   mCommandListForViewControl.Add("NonlinearConstraint");
   
   //mCommandListForViewControl.Add("EndTarget");
   //mCommandListForViewControl.Add("EndOptimize");
   //mCommandListForViewControl.Add("EndFor");
   //mCommandListForViewControl.Add("EndIf");
   //mCommandListForViewControl.Add("EndWhile");

   
   CreateCommandIdMap();
   CreateCommandCounterMap();
   
   // Should we sort the command list?
   #ifdef __SORT_COMMAND_LIST__
   mCommandList.Sort();
   #endif
   
   SetParameter(BOXCOUNT, 0);
   SetParameter(DRAWOUTLINE, 0);
   
   InitializeCounter();
   AddIcons();

   // shortcut keys
   wxAcceleratorEntry entries[2];
   entries[0].Set(wxACCEL_NORMAL,  WXK_NUMPAD_DELETE, MT_DELETE);
   entries[1].Set(wxACCEL_NORMAL, WXK_F2, MT_RENAME);
   wxAcceleratorTable accel(2, entries);
   this->SetAcceleratorTable(accel);

   
   // Now this is called from GmatNotebook after MissionTreeToolBar is created
   //AddDefaultMission();
   
   // Set default font
   SetFont(GmatAppData::Instance()->GetFont());
   
   #ifdef DEBUG_FONT
   wxFont currFont = GetFont();
   MessageInterface::ShowMessage
      ("In MissionTree() constructor, currFont.FaceName = '%s'\ncurrFont.NativeFontInfoDesc = '%s'\n"
       "currFont.NativeFontInfoUserDesc = '%s'\ncurrFont.GetPointSize = %d\n",
       currFont.GetFaceName().WX_TO_C_STRING, currFont.GetNativeFontInfoDesc().WX_TO_C_STRING,
       currFont.GetNativeFontInfoUserDesc().WX_TO_C_STRING, currFont.GetPointSize());
   #endif
   
   // for auto-testing of MissionTree actions
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   mSaveActions = false;
   mPlaybackActions = false;
   mActionsOutFile = "MissionTreeActionsOut.txt";
   mResultsFile = "MissionTreeResults.txt";
   #endif
}


//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *gmf)
//------------------------------------------------------------------------------
void MissionTree::SetMainFrame(GmatMainFrame *gmf)
{
   theMainFrame = gmf;
}


//------------------------------------------------------------------------------
// void SetNotebook(GmatNotebook *notebook)
//------------------------------------------------------------------------------
void MissionTree::SetNotebook(GmatNotebook *notebook)
{
   theNotebook = notebook;
}


//------------------------------------------------------------------------------
// void ClearFilters()
//------------------------------------------------------------------------------
/**
 * Clears filters on Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ClearFilters()
{
	if ((theNotebook != NULL) && (theNotebook->GetMissionTreeToolBar() != NULL))
		theNotebook->GetMissionTreeToolBar()->ClearFilters();
}

//------------------------------------------------------------------------------
// void ClearMission()
//------------------------------------------------------------------------------
/**
 * Clears Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ClearMission()
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::ClearMission() entered\n");
   #endif
   
   // Collapse, so folder icon is closed
   #ifdef __WXMSW__
      Collapse(mMissionSeqSubId);
   #endif

   wxString itemText = GetItemText(mMissionSeqSubId);
   if (itemText.Find("...") != wxNOT_FOUND)
   {
      itemText.Replace("...", "");
      SetItemText(mMissionSeqSubId, itemText);
   }
   
   DeleteChildren(mMissionSeqSubId);

   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mActionsOutStream.is_open())
      mActionsOutStream.close();
   if (mResultsStream.is_open())
      mResultsStream.close();
   if (mPlaybackResultsStream.is_open())
      mPlaybackResultsStream.close();
   #endif
}


//------------------------------------------------------------------------------
// void UpdateMission(bool resetCounter, bool viewAll = true, bool collapse = false)
//------------------------------------------------------------------------------
/**
 * Updates Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMission(bool resetCounter, bool viewAll, bool collapse)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateMission() entered, resetCounter=%d, viewAll=%d, "
       "collapse=%d, mViewAll=%d, mUsingViewLevel=%d, mViewLevel=%d\n",
       resetCounter, viewAll, collapse, mViewAll, mUsingViewLevel, mViewLevel);
   #endif
   
   if (resetCounter)
      InitializeCounter();
   
   // if (mUsingViewLevel)
   // {
   //    mViewAll = viewAll;
   // }
   
   ClearMission();
   UpdateCommand();
   
   if (collapse)
   {
      CollapseAllChildren(mMissionSeqSubId);
      Expand(mMissionSeqSubId);
   }
}


//------------------------------------------------------------------------------
// void UpdateMissionForRename()
//------------------------------------------------------------------------------
/**
 * Updates MissionTree nodes if show detail is turned on since it needs to show
 * new resource name on the nodes.
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMissionForRename()
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::UpdateMissionForRename() entered\n");
   #endif
   
   if (mShowDetailedItem)
   {
      ClearMission();
      UpdateCommand();
   }
}

//#define DEBUG_CHANGE_NODE_LABEL
//------------------------------------------------------------------------------
// void ChangeNodeLabel(const wxString &oldLabel)
//------------------------------------------------------------------------------
/*
 * Sets tree node label to new label if it is different from oldLabel.
 */
//------------------------------------------------------------------------------
void MissionTree::ChangeNodeLabel(const wxString &oldLabel)
{
   #ifdef DEBUG_CHANGE_NODE_LABEL
   MessageInterface::ShowMessage
      ("MissionTree::ChangeNodeLabel() oldLabel=<%s>\n", oldLabel.c_str());
   #endif
   
   wxTreeItemId itemId = FindChild(mMissionSeqSubId, oldLabel);
   if (itemId.IsOk())
   {
      MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
      GmatCommand *cmd = item->GetCommand();
      wxString newLabel = GetCommandString(cmd, oldLabel);
      
      #ifdef DEBUG_CHANGE_NODE_LABEL
      MessageInterface::ShowMessage("   newLabel=<%s>\n", newLabel.c_str());
      #endif
      
      if (newLabel != oldLabel)
      {
         item->SetName(newLabel);
         item->SetTitle(newLabel);
         SetItemText(itemId, newLabel);
      }
   }
   else
   {
      #ifdef DEBUG_CHANGE_NODE_LABEL
      MessageInterface::ShowMessage("===> <%s> not found\n", oldLabel.c_str());
      #endif
   }
   #ifdef DEBUG_CHANGE_NODE_LABEL
   MessageInterface::ShowMessage("MissionTree::ChangeNodeLabel() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ChangeNodeColor(const wxTreeItemId &nodeId, UnsignedInt intColor)
//------------------------------------------------------------------------------
void MissionTree::ChangeNodeColor(const wxTreeItemId &nodeId, UnsignedInt intColor)
{
   // Do not change node color for Propagate command for now (LOJ: 2014.02.20)
   // @todo Need better way to change Propatate command node color since
   //       some color makes hard to read item text. The better way would be
   //       changing the background color of icon. (See GMT-4330)
   #ifdef __CHANGE_NODE_COLOR__
   
   #ifdef DEBUG_NODE_COLOR
   MessageInterface::ShowMessage
      ("MissionTree::ChangeNodeColor() entered, nodeId=<%p>, intColor=%06X\n",
       &nodeId, intColor);
   #endif
   
   if (nodeId.IsOk())
   {
      RgbColor rgb(intColor);
      wxColour textColor(rgb.Red(), rgb.Green(), rgb.Blue());
      //SetItemTextColour(nodeId, textColor);
      SetItemBackgroundColour(nodeId, textColor);
      Refresh();
      Update();
   }
   
   #endif
}


//------------------------------------------------------------------------------
// void SetMissionTreeDocked(bool docked = false)
//------------------------------------------------------------------------------
void MissionTree::SetMissionTreeDocked(bool docked)
{
   mIsMissionTreeDocked = docked;
}


//------------------------------------------------------------------------------
// void SetViewAll(bool viewAll = true)
//------------------------------------------------------------------------------
void MissionTree::SetViewAll(bool viewAll)
{
   mViewAll = viewAll;
}


//------------------------------------------------------------------------------
// void SetViewLevel(int level)
//------------------------------------------------------------------------------
void MissionTree::SetViewLevel(int level)
{
   #ifdef DEBUG_VIEW_LEVEL
   MessageInterface::ShowMessage
      ("MissionTree::SetViewLevel() entered, level = %d, mViewAll = %d, "
       "mUsingViewLevel = %d, mViewLevel = %d\n", level, mViewAll, mUsingViewLevel,
       mViewLevel);
   #endif
   
   mUsingViewLevel = true;
   mViewLevel = level;
   if (level == 0) // Set level to 10 for showing all levels
      mViewLevel = 10;
   mViewAll = false;
   if (mViewLevel == 10)
      mViewAll = true;
   
   UpdateMission(true, false, false);
   
   #ifdef DEBUG_VIEW_LEVEL
   MessageInterface::ShowMessage
      ("MissionTree::SetViewLevel() leaving, mViewAll = %d, mUsingViewLevel = %d, "
       "mViewLevel = %d\n", mViewAll, mUsingViewLevel, mViewLevel);
   #endif
}


//------------------------------------------------------------------------------
// void SetViewCommands(const wxArrayString &viewCmds)
//------------------------------------------------------------------------------
void MissionTree::SetViewCommands(const wxArrayString &viewCmds)
{
   mViewCommands = viewCmds;
   
   #ifdef DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage("\n=====> MissionTree::SetViewOption() entered\n");
   MessageInterface::ShowMessage
      ("mCommandListForViewControl has %d commands\n", mCommandListForViewControl.GetCount());
   for (unsigned int i = 0; i < mCommandListForViewControl.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", mCommandListForViewControl[i].c_str());
   MessageInterface::ShowMessage
      ("mViewCommands has %d commands\n", mViewCommands.GetCount());
   for (unsigned int i = 0; i < mViewCommands.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", mViewCommands[i].c_str());
   #endif
   
   mUsingViewLevel = false;
   mViewAll = false;
   if (mViewCommands.GetCount() == 1 && mViewCommands[0] == "All")
      mViewAll = true;
   
   UpdateMission(true, false, false);
}

//------------------------------------------------------------------------------
// void EnableShowScript(bool flag)
//------------------------------------------------------------------------------
void MissionTree::EnableShowScript(bool flag)
{
   mEnableShowScript = flag;
}

//------------------------------------------------------------------------------
// const wxArrayString& GetCommandList(bool forViewControl = false)
//------------------------------------------------------------------------------
const wxArrayString& MissionTree::GetCommandList(bool forViewControl)
{
   if (forViewControl)
      return mCommandListForViewControl;
   else
      return mCommandList;
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void InitializeCounter()
//------------------------------------------------------------------------------
/**
 * Initializes command counter.
 */
//------------------------------------------------------------------------------
void MissionTree::InitializeCounter()
{
   mScriptEventCount = 0;
   mNumMissionSeq = 0;
   inScriptEvent = false;
   inFiniteBurn = false;
   ResetCommandCounter("ALL", true);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const wxString &cmdTypeName, GmatCommand *refCmd)
//------------------------------------------------------------------------------
GmatCommand* MissionTree::CreateCommand(const wxString &cmdTypeName, GmatCommand *refCmd)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateCommand() entered, cmdTypeName='%s', refCmd=<%p><%s>\n",
       cmdTypeName.c_str(), refCmd, refCmd ? refCmd->GetTypeName().c_str() : "NULL");
   #endif
   
   GmatCommand *cmd = NULL;
   try
   {
      if (cmdTypeName == "IfElse")
         cmd = theGuiInterpreter->CreateDefaultCommand("If");
      else if (cmdTypeName == "Equation")
         cmd = theGuiInterpreter->CreateDefaultCommand("GMAT");
      else
         cmd = theGuiInterpreter->CreateDefaultCommand(cmdTypeName.c_str(), "", refCmd);
   }
   catch (BaseException &be)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateCommand() returning <%p>\n", cmd);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateEndCommand(const wxString &cmdTypeName, GmatTree::ItemType &endType)
//------------------------------------------------------------------------------
GmatCommand* MissionTree::CreateEndCommand(const wxString &cmdTypeName,
														 GmatTree::ItemType &endType)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateEndCommand() entered, cmdTypeName='%s'\n", cmdTypeName.c_str());
   #endif
	
	GmatCommand *endCmd = NULL;
	
	if (cmdTypeName == "Target")
	{
		endCmd = CreateCommand("EndTarget");
		endType = GmatTree::END_TARGET;
	}
	else if (cmdTypeName == "For")
	{
		endCmd = CreateCommand("EndFor");
		endType = GmatTree::END_FOR_CONTROL;
	}
	else if (cmdTypeName == "While")
	{
		endCmd = CreateCommand("EndWhile");
		endType = GmatTree::END_WHILE_CONTROL;
	}
	else if (cmdTypeName == "If")
	{
		endCmd = CreateCommand("EndIf");
		endType = GmatTree::END_IF_CONTROL;
	}
	else if (cmdTypeName == "ScriptEvent")
	{
		endCmd = CreateCommand("EndScript");
		endType = GmatTree::END_SCRIPT_EVENT;
	}
	else if (cmdTypeName == "Optimize")
	{
		endCmd = CreateCommand("EndOptimize");
		endType = GmatTree::END_OPTIMIZE;
	}
	
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::CreateEndCommand() returning <%p>\n", endCmd);
   #endif
   
   return endCmd;
}

//------------------------------------------------------------------------------
// bool InsertCommandToSequence(GmatCommand *currCmd, GmatCommand *prevCmd,
//                              GmatCommand *cmdToInsert)
//------------------------------------------------------------------------------
bool MissionTree::InsertCommandToSequence(GmatCommand *currCmd, GmatCommand *prevCmd,
                                          GmatCommand *cmdToInsert)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("MissionTree::InsertCommandToSequence() entered\n");
   #endif
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==> before appending/inserting: ", "previous of ", cmdToInsert, " is ",
                cmdToInsert->GetPrevious());
   #endif
   
   bool cmdAdded = false;
   if (currCmd->GetTypeName() == "NoOp" ||
       currCmd->GetTypeName() == "BeginMissionSequence")
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   ==> Calling gui->AppendCommand()\n");
      #endif
      
      // Append to base command list
      cmdAdded = theGuiInterpreter->AppendCommand(cmdToInsert);
   }
   else
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   ==> Calling gui->InsertCommand()\n");
      #endif
      
      cmdAdded = theGuiInterpreter->InsertCommand(cmdToInsert, prevCmd);
   }
   
   //------------------------------------------------------------
   // We need to set real previous command after command is
   // appended/inserted, since cmd->AppendCommand() or
   // cmd->InsertCommand() resets previous command.
   // So when ScriptEvent is modified, the old ScriptEvent is
   // deleted and new one can be inserted into correct place.
   //------------------------------------------------------------
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==>", " Resetting previous of ", cmdToInsert, "to ", prevCmd);
   #endif
   
   cmdToInsert->ForceSetPrevious(prevCmd);
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ==> after  appending/inserting: ", "previous of ", cmdToInsert,
                " is ", cmdToInsert->GetPrevious());
   #endif
   
   //MessageInterface::ShowMessage("   ==> cmdAdded = %d\n", cmdAdded);   
   // Why returning false eventhough it inserted?
   cmdAdded = true;
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommandToSequence() returning %d\n", cmdAdded);
   #endif
   
   return cmdAdded;
}


//------------------------------------------------------------------------------
// bool IsAnyViewCommandInBranch(GmatCommand *branch)
//------------------------------------------------------------------------------
/**
 * Returns true if any view command found in the branch command
 */
//------------------------------------------------------------------------------
bool MissionTree::IsAnyViewCommandInBranch(GmatCommand *branch)
{
   #if DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage
      ("MissionTree::IsAnyViewCommandInBranch() branch=<%p><%s>'%s' entered\n",
       branch, branch->GetTypeName().c_str(), branch->GetName().c_str());
   #endif
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   wxString typeName, cmdName;
   wxString branchTypeName = branch->GetTypeName().c_str();
   
   while((child = branch->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != branch))
      {
         typeName = (nextInBranch->GetTypeName()).c_str();
         cmdName = (nextInBranch->GetName()).c_str();
         
         #if DEBUG_VIEW_COMMANDS
         MessageInterface::ShowMessage
            ("   ----- <%p><%s>'%s'\n", nextInBranch, typeName.c_str(), cmdName.c_str());
         #endif
         
         if (mViewCommands.Index(typeName) != wxNOT_FOUND)
         {
            #if DEBUG_VIEW_COMMANDS
            MessageInterface::ShowMessage
               ("MissionTree::IsAnyViewCommandInBranch() returning true, found <%s>'%s' "
                "in <%s>'%s'\n", typeName.c_str(), cmdName.c_str(), branchTypeName.c_str(),
                branch->GetName().c_str());
            MessageInterface::ShowMessage("***** Should the tree item be built and return true?\n");
            #endif
            
            return true;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
         {
            if (IsAnyViewCommandInBranch(nextInBranch))
            {
               #if DEBUG_VIEW_COMMANDS
               MessageInterface::ShowMessage("***** Should the node be shown here?\n");
               #endif
            }
         }
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
   #if DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage
      ("MissionTree::IsAnyViewCommandInBranch() returning false\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// wxTreeItemId BuildTreeItemInBranch(wxTreeItemId parent, GmatCommand *branch, ...)
//------------------------------------------------------------------------------
/**
 * Returns valid wxTreeItemId if any view command found in the branch command
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::BuildTreeItemInBranch(wxTreeItemId parent, GmatCommand *branch,
                                                Integer level, bool &isLastItemHidden)
{
   #if DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage
      ("MissionTree::BuildTreeItemInBranch() branch=<%p><%s> entered\n",
       branch, branch->GetTypeName().c_str());
   #endif
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   wxString typeName, cmdName;
   wxString branchTypeName = branch->GetTypeName().c_str();
   wxTreeItemId node;
   
   while((child = branch->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != branch))
      {
         typeName = (nextInBranch->GetTypeName()).c_str();
         cmdName = (nextInBranch->GetName()).c_str();
         
         #if DEBUG_VIEW_COMMANDS
         MessageInterface::ShowMessage
            ("   ----- <%p><%s>'%s'\n", nextInBranch, typeName.c_str(), cmdName.c_str());
         #endif
         
         if (mViewCommands.Index(typeName) != wxNOT_FOUND)
         {
            #if DEBUG_VIEW_COMMANDS
            MessageInterface::ShowMessage
               ("MissionTree::BuildTreeItemInBranch() returning true, found <%s>'%s' "
                "in <%s>'%s'\n", typeName.c_str(), cmdName.c_str(), branchTypeName.c_str(),
                branch->GetName().c_str());
            #endif
            
            return node;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
         {
            //@todo Need to finish this
            //node = BuildTreeItemInBranch(nextInBranch);
         }
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
   #if DEBUG_VIEW_COMMANDS
   MessageInterface::ShowMessage
      ("MissionTree::BuildTreeItemInBranch() returning\n");
   #endif
   return node;
}


//------------------------------------------------------------------------------
// void ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node)
//------------------------------------------------------------------------------
void MissionTree::ShowEllipsisInPreviousNode(wxTreeItemId parent, wxTreeItemId node)
{
   wxString itemText;
   wxTreeItemId prevId = GetPrevVisible(node);
   if (prevId.IsOk())
   {
      itemText = GetItemText(prevId);
      if (itemText.Find("...") == wxNOT_FOUND)
      {
         itemText = itemText + "...";
         SetItemText(prevId, itemText);
      }
   }
   else
   {
      itemText = GetItemText(parent);
      if (itemText.Find("...") == wxNOT_FOUND)
      {
         itemText = itemText + "...";
         SetItemText(parent, itemText);
      }
   }
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("   previous item = '%s'\n", itemText.c_str());
   #endif
}


//------------------------------------------------------------------------------
// wxTreeItemId BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd, ...)
//------------------------------------------------------------------------------
/**
 * Appends command node to mission tree and returns valid wxTreeItemId
 * if command is visible.
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::BuildTreeItem(wxTreeItemId parent, GmatCommand *cmd,
                                        Integer level, bool &isLastItemHidden)
{
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString cmdName = cmd->GetName().c_str();
   wxTreeItemId node;
   
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("\nMissionTree::BuildTreeItem() entered, parent='%s', cmd=<%s>'%s', level=%d\n",
       GetItemText(parent).WX_TO_C_STRING, cmdTypeName.WX_TO_C_STRING,
       cmdName.WX_TO_C_STRING, level);
   MessageInterface::ShowMessage
      ("   inScriptEvent=%d, mViewAll=%d, mUsingViewLevel=%d, mViewLevel=%d\n",
       inScriptEvent, mViewAll, mUsingViewLevel, mViewLevel);
   #endif
   
   // if cmdTypeName not found in the view list and not showing all
   if (mViewCommands.Index(cmdTypeName) == wxNOT_FOUND && !mViewAll && !mUsingViewLevel)
   {
      if (cmd->GetTypeName() == "BeginScript")
         mScriptEventCount++;
      
      if (cmd->GetTypeName() == "EndScript")
         mScriptEventCount--;
      
      inScriptEvent = (mScriptEventCount == 0) ? false : true;
      
      bool viewCmdFoundInBranch = false;
      if (cmd->IsOfType("BranchCommand"))
      {
         if (IsAnyViewCommandInBranch(cmd))
            viewCmdFoundInBranch = true;
      }
      
      // Always show EndBranch command
      // Fix constant, was "EndBranch"
      // (TGG: 2012.07.17 for bug GMT-2901)
      if (!cmd->IsOfType("BranchEnd") && !viewCmdFoundInBranch)
      {
         isLastItemHidden = true;
         #if DEBUG_BUILD_TREE_ITEM
         MessageInterface::ShowMessage
            ("MissionTree::BuildTreeItem() returning '%s' node, hiding the node\n",
             node.IsOk() ? "good" : "bad");
         #endif
         return node;
      }
   }
   
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("   Creating command node for <%s>'%s'\n", cmdTypeName.WX_TO_C_STRING,
       cmdName.WX_TO_C_STRING);
   #endif
   
   node = UpdateCommandTree(parent, cmd, level);
   
   // Change command item text color (LOJ: 2013.12.11)
   ChangeCommandNodeColor(cmd, node);
   
   // If it is not a branch end, then show ellipsis
   if (isLastItemHidden && !cmd->IsOfType("BranchEnd"))
      ShowEllipsisInPreviousNode(parent, node);
   
   isLastItemHidden = false;
   
   if (cmd->GetTypeName() == "BeginScript")
      mScriptEventCount++;
   
   if (cmd->GetTypeName() == "EndScript")
      mScriptEventCount--;
   
   inScriptEvent = (mScriptEventCount == 0) ? false : true;
   
   #if DEBUG_BUILD_TREE_ITEM
   MessageInterface::ShowMessage
      ("MissionTree::BuildTreeItem() returning '%s' node, showing the node\n",
       node.IsOk() ? "good" : "bad");
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// void UpdateCommand()
//------------------------------------------------------------------------------
/**
 * Builds mission tree from commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateCommand()
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommand() entered, mViewAll=%d, mUsingViewLevel=%d, "
       "mViewLevel=%d\n", mViewAll, mUsingViewLevel, mViewLevel);
   #endif
   
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   GmatCommand *child;
   MissionTreeItemData *seqItemData =
      (MissionTreeItemData *)GetItemData(mMissionSeqSubId);
   wxTreeItemId node;
   
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);
   
   wxString typeName;
   bool isLastItemHidden = false;
   
   while (cmd != NULL)
   {
      #ifndef __ENABLE_NAV_GUI__
      if (cmd->IsOfType("RunSimulator") || cmd->IsOfType("RunEstimator"))
      {
         cmd = cmd->GetNext();
         continue;
      }
      #endif
      
      node = BuildTreeItem(mMissionSeqSubId, cmd, 0, isLastItemHidden);
      
      if (isLastItemHidden)
      {
         cmd = cmd->GetNext();
         continue;
      }
      
      child = cmd->GetChildCommand(0);
      
      if (child != NULL)
      {
         ExpandChildCommand(node, cmd, 0);
      }
      
      cmd = cmd->GetNext();
   }
   
   Expand(mMissionSeqSubId);
   ////ScrollTo(mMissionSeqSubId);
   
   if (mWriteMissionSeq)
      ShowCommands("After Updating Command Sequence");
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   MessageInterface::ShowMessage("MissionTree::UpdateCommand() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd, ...)
//------------------------------------------------------------------------------
/**
 * Appends command node to parent node.
 */
//------------------------------------------------------------------------------
wxTreeItemId& MissionTree::UpdateCommandTree(wxTreeItemId parent,
                                             GmatCommand *cmd, Integer level)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() entered, inScriptEvent=%d, cmd=<%p><%s>\n",
       inScriptEvent, cmd, cmd->GetTypeName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxTreeItemId currId;
   mNewTreeId = currId;
   
   // If ScriptEvent mode or command is NoOp or BeginMissionSequence, don't add it 
   // This is different from command unviewable list
   if (inScriptEvent || cmdTypeName == "NoOp" || cmdTypeName == "BeginMissionSequence")
   {
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage
         ("MissionTree::UpdateCommandTree() leaving, command '%s' ignored\n",
          cmdTypeName.WX_TO_C_STRING);
      #endif
      return mNewTreeId;
   }
   
   mNewTreeId = AppendCommand(parent, GetIconId(cmdTypeName),
                              GetCommandId(cmdTypeName), cmd,
                              GetCommandCounter(cmd));
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("   mUsingViewLevel=%d, mViewLevel=%d, level=%d\n", mUsingViewLevel,
       mViewLevel, level);
   #endif
   
   if (mUsingViewLevel)
   {
      if (mViewLevel > level + 1)
         Expand(parent);
   }
   else
      Expand(parent);
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() leaving, command <%s>'%s' added to tree\n",
       cmdTypeName.WX_TO_C_STRING, cmd->GetName().c_str());
   #endif
   
   return mNewTreeId;
}


//------------------------------------------------------------------------------
// void ExpandNode(wxTreeItemId node, const wxString &cmdType)
//------------------------------------------------------------------------------
/**
 * Expands the node or not based on the view level or view category.
 */
//------------------------------------------------------------------------------
void MissionTree::ExpandNode(wxTreeItemId node, const wxString &cmdType)
{
   #ifdef DEBUG_EXPAND_NODE
   MessageInterface::ShowMessage
      ("MissionTree::ExpandNode() endtered, cmdType='%s'\n", cmdType.c_str());
   #endif
   
   bool expand = false;
   int nodeLevel  = 0;
   wxTreeItemId parentId = GetItemParent(node);
   
   // Check upto 3 levels
   if (mUsingViewLevel)
   {
      if (parentId.IsOk())
      {
         nodeLevel = 1;
         wxTreeItemId gParentId = GetItemParent(parentId);
         if (gParentId.IsOk())
         {         
            nodeLevel = 2;
            wxTreeItemId ggParentId = GetItemParent(gParentId);
            if (ggParentId.IsOk())
               nodeLevel = 3;
         }
         
         if (mUsingViewLevel && mViewLevel >= nodeLevel)
            expand = true;
         
         if (expand)
            Expand(node);
      }
   }
   else
   {
      if (mViewCommands.Index(cmdType) != wxNOT_FOUND)
      {
         expand = true;
         if (parentId.IsOk())
         {
            MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId); 
            GmatCommand *parentCmd = parentItem->GetCommand();
            if (parentCmd->IsOfType("BranchCommand"))
               expand = true;
         }
      }
      
      if (expand)
         Expand(parentId);
   }
   
   #ifdef DEBUG_EXPAND_NODE
   MessageInterface::ShowMessage
      ("MissionTree::ExpandNode() nodeLevel = %d, mViewLevel = %d, expand = %d\n",
       nodeLevel, mViewLevel, expand);
   #endif
}


//------------------------------------------------------------------------------
// void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Expands child commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd,
                                     Integer level)
{
   #if DEBUG_MISSION_TREE_CHILD
   MessageInterface::ShowMessage
      ("MissionTree::ExpandChildCommand() parent='%s', cmd='%s', level=%d\n",
       GetItemText(parent).c_str(), cmd->GetTypeName().c_str(), level);
   #endif
   
   wxTreeItemId branchNode;
   wxTreeItemId node;
   wxTreeItemId elseNode;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   wxString typeName;
   bool isLastItemHidden = false;
   bool useElseAsParent = false;
   
   while((child = cmd->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      #if DEBUG_MISSION_TREE_CHILD
      MessageInterface::ShowMessage("   nextInBranch='%s'\n", nextInBranch->GetTypeName().c_str());
      MessageInterface::ShowMessage("   useElseAsParent=%d\n", useElseAsParent);
      #endif
      
      while ((nextInBranch != NULL) && (nextInBranch != cmd))
      {
         #if DEBUG_MISSION_TREE_CHILD
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         MessageInterface::ShowMessage
            ("----- (%p)'%s'\n", nextInBranch, nextInBranch->GetTypeName().c_str());
         #endif
         
         // Special handling of Else since Else command is not a branch command
         // We want indent commands after Else
         if (useElseAsParent)
            node = BuildTreeItem(elseNode, nextInBranch, level, isLastItemHidden);
         else
            node = BuildTreeItem(parent, nextInBranch, level, isLastItemHidden);
			
         if (isLastItemHidden)
         {
            // If it is not a branch end, then show ellipsis
            if (!nextInBranch->IsOfType("BranchEnd"))
               ShowEllipsisInPreviousNode(parent, node);
            
            nextInBranch = nextInBranch->GetNext();
            continue;
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
            ExpandChildCommand(node, nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}


//------------------------------------------------------------------------------
// wxTreeItemId AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType type, GmatCommand *cmd,
//                            int cmdCount)
//------------------------------------------------------------------------------
/**
 * Appends command to command list and/or command tree.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, GmatCommand *cmd,
                           int cmdCount)
{
   #if DEBUG_APPEND_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand('%s') entered, type = \"%s\" and name = \"%s\"\n",
       GetItemText(parent).WX_TO_C_STRING,  cmd->GetTypeName().c_str(), 
       cmd->GetName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   //MissionTreeItemData *parentItem = (MissionTreeItemData *) GetItemData(parent);
   //GmatCommand *parentCmd = parentItem->GetCommand();
   //wxString parentName = (parentCmd->GetName()).c_str();
   bool hasNoCommandName = true;
   if (nodeName != "")
      hasNoCommandName = false;
   
   #if DEBUG_APPEND_COMMAND
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand() cmdTypeName='%s', nodeName='%s'\n",
       cmdTypeName.WX_TO_C_STRING, nodeName.WX_TO_C_STRING);
   #endif
   // Compose node name
   // Changed node name of End and Else to append parent's name
   // (LOJ: 2010.01.27 for bug GMT-209 fix)
   // Note: Else is also BranchEnd so it will have If's counter if unnamed
   if (cmd->IsOfType("BranchEnd"))
   {
      // Actually we need to first check for command name exist or not.
      // So commented out (LOJ: 2012.12.11)
      //if (parentName.Trim() != "")
      //{
      //   nodeName.Printf("End %s", parentName.c_str());
      //}
      //else if (nodeName.Trim() == "")
      if (nodeName.Trim() == "")
      {
         // Use branch summary name for counter so that nested branch end command
         // matches with its branch command. (GMT-209 fix)(LOJ: 2012.02.27)
         GmatCommand *branch = cmd->GetNext();
         wxString branchTypeName = branch->GetTypeName().c_str();
         wxString label = branch->GetSummaryName().c_str();
		 if (label.StartsWith(branchTypeName + " "))
			label.Replace(branchTypeName, cmdTypeName, false);
         // Replace first End with End+Space
         // (TGG: 2012.07.17 for bug GMT-2901)
		 if ((!label.StartsWith("End")) && (!label.StartsWith("Else")))
			 label.Prepend("End ");
		 else
			label.Replace("End", "End ", false);
         nodeName.Printf("%s", label.c_str());
      }
      
      // If command has no name and it is Else command, Show "Else" (LOJ: 2012.12.11)
      if (hasNoCommandName && cmd->IsOfType("Else"))
         nodeName.Replace("End", "Else");
   }
   else
   {
      if (nodeName.Trim() == "" || nodeName == cmdTypeName)
         nodeName.Printf("%s%d", cmdTypeName.c_str(), cmdCount);
   }
   
   #if DEBUG_APPEND_COMMAND
   MessageInterface::ShowMessage
      ("   After composing node name, nodeName='%s'\n", nodeName.WX_TO_C_STRING);
   #endif
   
   // If a command has no name, replace for GUI name
   if (hasNoCommandName)
   {
      // Show "ScriptEvent" instead of "BeginScript" to be more clear for user if
      // command has no name
      if (hasNoCommandName && nodeName.Contains("BeginScript"))
         nodeName.Replace("BeginScript", "ScriptEvent");
      
      // Show "Equation" instead of "GMAT" to be more clear for user
      if (nodeName.Contains("GMAT"))
         nodeName.Replace("GMAT", "Equation");
   }
   
   // Show command string as node label(loj: 2007.11.13)
   nodeName = GetCommandString(cmd, nodeName);
   /// Tell the command its name
   cmd->SetSummaryName(nodeName.c_str());
   
   #if DEBUG_APPEND_COMMAND
   MessageInterface::ShowMessage
      ("   After replacing command name, nodeName='%s'\n", nodeName.WX_TO_C_STRING);
   #endif
   
   node = AppendItem(parent, nodeName, icon, -1,
                     new MissionTreeItemData(nodeName, type, nodeName, cmd));
   
   #if DEBUG_APPEND_COMMAND
   WriteNode(1, "", false, "MissionTree::AppendCommand() returning", node);
   #endif
   return node;
}


//------------------------------------------------------------------------------
// void AppendEndToBranch(wxTreeItemId parentId, ...)
//------------------------------------------------------------------------------
/**
 * Appends BranchEnd node to Branch node.
 */
//------------------------------------------------------------------------------
void MissionTree::AppendEndToBranch(wxTreeItemId parentId, GmatTree::MissionIconType icon,
                                    const wxString &cmdTypeName, const wxString &cmdName,
                                    GmatCommand *elseCmd, GmatCommand *endCmd,
                                    GmatTree::ItemType endType, int cmdCount)
{
   wxTreeItemId node = parentId;
   
   // append Else (temp code until Else is implemented)
   if (cmdName == "IfElse")
   {
      wxString elseName;
      elseName.Printf("Else%d", cmdCount);
      elseCmd->SetSummaryName(elseName.c_str());
      
      wxTreeItemId elseNode =
         InsertItem(node, 0, elseName, icon, -1,
                    new MissionTreeItemData(elseName, GmatTree::ELSE_CONTROL,
                                            elseName, elseCmd));
      
      wxString endName = "End " + cmdTypeName;
      wxString tmpName;
      tmpName.Printf("%s%d", endName.c_str(), cmdCount);
      endCmd->SetSummaryName(tmpName.c_str());
      InsertItem(node, elseNode, tmpName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                 new MissionTreeItemData(tmpName, endType, tmpName, endCmd));
   }
   else
   {
      wxString endName = "End " + cmdTypeName;
      wxString tmpName;
      tmpName.Printf("%s%d", endName.c_str(), cmdCount);
      endCmd->SetSummaryName(tmpName.c_str());
      InsertItem(node, 0, tmpName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                 new MissionTreeItemData(tmpName, endType, tmpName, endCmd));
   }
}


//------------------------------------------------------------------------------
// wxTreeItemId InsertNodeToTree(wxTreeItemId parentId, wxTreeItemId currId, ...)
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertNodeToTree(wxTreeItemId parentId, wxTreeItemId currId,
                              wxTreeItemId prevId, GmatTree::MissionIconType icon,
                              const wxString &nodeName, GmatTree::ItemType itemType,
                              GmatCommand *cmd, GmatCommand *currCmd, GmatCommand *prevCmd,
                              bool insertBefore)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("MissionTree::InsertNodeToTree() entered\n");
   #endif
   
	wxString parentName = GetItemText(parentId);
	wxString currItemName = GetItemText(currId);
   wxString prevTypeName = prevCmd->GetTypeName().c_str();
   wxTreeItemId node;
   
   if (currCmd->GetTypeName() == "NoOp" ||
       currCmd->GetTypeName() == "BeginMissionSequence")
   {
      node = AppendItem(currId, nodeName, icon, -1,
                        new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
   }
   else if (IsElseNode(prevId) && insertBefore)
   {
      // Find the position of Else and insert before position
      size_t elsePos = (size_t)FindItemPosition(parentId, prevId);
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   111 inserting '%s' before position %d\n", nodeName.c_str(), elsePos);
      #endif
      node = InsertItem(parentId, elsePos, nodeName, icon, -1,
                        new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
   }
   else if (currId == prevId && !insertBefore)
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   112 inserting '%s' after '%s' from parent '%s'\n", nodeName.c_str(),
          currItemName.c_str(), parentName.c_str());
      #endif
      node = InsertItem(parentId, currId, nodeName, icon, -1,
                        new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
   }
   else if (prevTypeName == "NoOp"     || prevTypeName == "BeginMissionSequence" ||
            prevTypeName == "Target"   || prevTypeName == "For"    ||
            prevTypeName == "While"    || prevTypeName == "If"     ||
            prevTypeName == "Optimize")
   {
      node = InsertNodeToBranch(parentId, currId, prevId, icon, nodeName, itemType, cmd,
                                currCmd, prevCmd, insertBefore);
   }
   else if (prevTypeName.Contains("End") && prevTypeName != "EndScript" &&
            prevTypeName != "EndFiniteBurn")
   {
      wxTreeItemId realParentId = parentId;
      wxTreeItemId realPrevId = prevId;			
      wxString realParentName = GetItemText(realParentId);
      wxString realPrevName = GetItemText(realPrevId);
		
      if (realParentName == "")
      {
         realParentId = parentId;
         realParentName = GetItemText(realParentId);
      }
		
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   ==> previous type contains End\n");
      MessageInterface::ShowMessage
         ("   ==> realParentId='%s'\n", realParentId.IsOk() ? realParentName.c_str() : "Bad Node");
      MessageInterface::ShowMessage
         ("   ==> realPrevId='%s'\n", realPrevId, realPrevName.c_str());
      #endif
      
      if (realParentId == realPrevId)
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   211 inserting '%s' after '%s' from parent '%s'\n", nodeName.c_str(),
             currItemName.c_str(), realParentName.c_str());
         #endif
         
         node = InsertItem(realParentId, currId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
      else if (realParentName != "")
      {
         MissionTreeItemData *realPrevItem = (MissionTreeItemData *)GetItemData(realPrevId);
         GmatCommand *realPrevCmd = realPrevItem->GetCommand();
			
         if (realPrevCmd->IsOfType("BranchEnd"))
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage
               ("===> Previous node is BranchEnd, so setting previous to its parent\n");
            #endif
            realPrevId = GetItemParent(realPrevId);					
         }
			
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   221 inserting '%s' after '%s' from parent '%s'\n", nodeName.c_str(),
             realPrevName.c_str(), realParentName.c_str());
         #endif
         
         node = InsertItem(realParentId, realPrevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
      else
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage
            ("   231 inserting '%s' after '%s', parent='%s'\n", nodeName.c_str(),
             currItemName.c_str(), realParentName.c_str());
         #endif
         node = InsertNodeAfter(realParentId, currId, prevId, icon, nodeName, itemType,
                                cmd, insertBefore);
      }
   }
   else
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   241 inserting '%s' after '%s', parent='%s'\n", nodeName.c_str(),
          currItemName.c_str(), GetItemText(parentId).c_str());
       #endif
      node = InsertNodeAfter(parentId, currId, prevId, icon, nodeName, itemType,
                             cmd, insertBefore);
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteNode(1, "MissionTree::InsertNodeToTree() returning ", true, "node", node);
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// wxTreeItemId InsertNodeToBranch(wxTreeItemId parentId, wxTreeItemId currId, ...)
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertNodeToBranch(wxTreeItemId parentId, wxTreeItemId currId,
                                wxTreeItemId prevId, GmatTree::MissionIconType icon,
                                const wxString &nodeName, GmatTree::ItemType itemType,
                                GmatCommand *cmd, GmatCommand *currCmd, GmatCommand *prevCmd,
                                bool insertBefore)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertNodeToBranch() entered, insertBefore=%d\n", insertBefore);
   WriteNode(3, "   ", false, "parentId", parentId, "currId", currId, "prevId", prevId);
   #endif
   
   wxTreeItemId node;
   wxTreeItemId realParentId = parentId;
   
   if (insertBefore)
      realParentId = prevId;
   
   wxString realParentName = GetItemText(realParentId);
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   previous type is NoOp, BeginMissionSequence, BranchCommand, or Else\n");
   WriteNode(1, "   ", false, "realParentId", realParentId);
   #endif
   
   
   if (!insertBefore)
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   411 inserting by realParentId and position 0\n");
      #endif
      node = InsertItem(realParentId, 0, nodeName, icon, -1,
                        new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
   }
   else if (IsExpanded(prevId))
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   Previous item is expanded\n");
      #endif
      if (currCmd->IsOfType("BranchEnd"))
      {
         if (prevCmd->IsOfType("BranchCommand"))
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   421 inserting by parentId and position 0\n");
            #endif
            node = InsertItem(parentId, 0, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
         }
         else
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   422 inserting by parentId and prevId\n");
            #endif
            node = InsertItem(parentId, prevId, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
         }
      }
      else
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   423 inserting by parentId and position 0\n");
         #endif
         node = InsertItem(parentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
   }
   else
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   Neither current or previous item is expanded\n");
      #endif
      if (!insertBefore)
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   431 inserting by realParentId and position 0\n");
         #endif
         node = InsertItem(realParentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
      else
      {
         if (currCmd->IsOfType("BranchEnd"))
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   432 inserting by realParentId and position 0\n");
            #endif
            node = InsertItem(realParentId, 0, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
         }
         else
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   433 inserting by parentId and prevId\n");
            #endif
            node = InsertItem(parentId, prevId, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
            
         }
      }
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("InsertNodeToBranch() leaving\n");
   #endif
   return node;
}


//------------------------------------------------------------------------------
// wxTreeItemId InsertNodeAfter(wxTreeItemId parentId, wxTreeItemId currId,
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertNodeAfter(wxTreeItemId parentId, wxTreeItemId currId,
                             wxTreeItemId prevId, GmatTree::MissionIconType icon,
                             const wxString &nodeName, GmatTree::ItemType itemType,
                             GmatCommand *cmd, bool insertBefore)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("InsertNodeAfter() entered, insertBefore = %d\n", insertBefore);
   #endif
   
   wxTreeItemId node;
   wxTreeItemId realParentId = parentId;
   wxTreeItemId realPrevId = prevId;
   
   if (insertBefore)
   {
      if (GetChildrenCount(GetItemParent(prevId)) > 0)
      {
         realParentId = GetItemParent(prevId);
      }
      else
      {
         realParentId = prevId;
         realPrevId = GetLastChild(prevId);
      }
   }
   
   wxTreeItemId prevVisId = GetPrevVisible(currId);
   wxString prevVisName = GetItemText(prevVisId);
   wxString parentName = GetItemText(realParentId);
   wxString realPrevName = GetItemText(realPrevId);
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteNode(3, "   ==> ", false, "realParentId", realParentId, "realPrevId", realPrevId,
             "prevVisId", prevVisId);
   #endif
   
   if (GetItemParent(currId) == prevVisId)
   {
      if (insertBefore)
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   311 inserting by prevVisId and position 0\n");
         #endif
         node = InsertItem(prevVisId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
      else
      {
         if (realPrevName == prevVisName)
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   312 appending to realPrevId\n");
            #endif
            node = AppendItem(prevVisId, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
         }
         else
         {
            #if DEBUG_MISSION_TREE_INSERT
            MessageInterface::ShowMessage("   313 insertnig by realParentId and realPrevId\n");
            #endif
            node = InsertItem(realParentId, realPrevId, nodeName, icon, -1,
                              new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
         }
      }
   }
   else if (realPrevId == prevVisId)
   {
      if (GetItemParent(prevVisId) == GetItemParent(currId))
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   321 inserting by parentId and realPrevId\n");
         #endif
         node = InsertItem(parentId, realPrevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
      else
      {
         #if DEBUG_MISSION_TREE_INSERT
         MessageInterface::ShowMessage("   322 inserting by parentId and realParentId\n");
         #endif
         node = InsertItem(parentId, realParentId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
      }
   }
   else
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage("   331 inserting by realParentId and realPrevId\n");
      #endif
      node = InsertItem(realParentId, realPrevId, nodeName, icon, -1,
                        new MissionTreeItemData(nodeName, itemType, nodeName, cmd));
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("InsertNodeAfter() leaving\n");
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
//                            wxTreeItemId prevId, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType itemType, const wxString &cmdName,
//                            GmatCommand *prevCmd, GmatCommand *cmd, int cmdCount,
//                            bool insertBefore)
//------------------------------------------------------------------------------
/**
 * Inserts command to mission tree.
 *
 * @param  parentId  Parent item id
 * @param  currId  Currently selected item id
 * @param  prevId  Previous item id
 * @param  icon  Icon to be used
 * @param  itemType  Item type, such as GmatTree::PROPAGATE
 * @param  cmdName  Command type name to be inserted
 * @param  prevCmd  Previous command pointer
 * @param  cmd  Command pointer to be inserted
 * @param  cmdCount  Command counter to be appended to command name
 * @param  insertBefore  true if inserting before the current item,
 *                       false if inserterting after current item
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::MissionIconType icon,
                           GmatTree::ItemType itemType, const wxString &cmdName,
                           GmatCommand *prevCmd, GmatCommand *cmd, int cmdCount,
                           bool insertBefore)
{
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() entered, insertBefore=%d\n", insertBefore);
   WriteNode(3, "   ", false, "parentId", parentId, "currId", currId, "prevId", prevId);
   #endif
   
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId);
   GmatCommand *currCmd = currItem->GetCommand();
	wxString parentName = GetItemText(parentId);
	wxString currItemName = GetItemText(currId);
   wxString currTypeName = currCmd->GetTypeName().c_str();
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString prevTypeName = prevCmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   GmatCommand *endCmd = NULL;
   GmatCommand *elseCmd = NULL;
   GmatTree::ItemType endType = GmatTree::END_TARGET;
   bool cmdAdded = false;
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   currItemName='%s', currTypeName='%s', prevTypeName='%s'\n",
		 currItemName.c_str(), currTypeName.c_str(), prevTypeName.c_str());
   #endif
	
   // Show "Equation" instead of "GMAT" to be more clear for user
   if (currTypeName == "GMAT")
      currTypeName = "Equation";
   if (cmdTypeName == "GMAT")
      cmdTypeName = "Equation";
   if (prevTypeName == "GMAT")
      prevTypeName = "Equation";
   
   // Show "ScriptEvent" instead of "BeginScript" to be more clear for user
   if (currTypeName == "BeginScript")
      currTypeName = "ScriptEvent";
   if (cmdTypeName == "BeginScript")
      cmdTypeName = "ScriptEvent";
   if (prevTypeName == "BeginScript")
      prevTypeName = "ScriptEvent";
   
   #if DEBUG_MISSION_TREE_INSERT
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId); 
   GmatCommand *parentCmd = parentItem->GetCommand();
   MessageInterface::ShowMessage
      ("   cmdName='%s', cmdTypeName='%s', cmdCount=%d\n", cmdName.c_str(),
       cmdTypeName.c_str(), cmdCount);
   MessageInterface::ShowMessage
      ("   parentCmd='%s', prevCmd='%s', prevTypeName='%s', currCmd='%s'\n",
       parentCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str(),
       prevTypeName.c_str(), currTypeName.c_str());
   #endif
   
   // If previous command is BeginScript, find matching EndScript,
   // since commands inside EndScript, including EndScript is not shown
   // on the tree.
   if (prevCmd->GetTypeName() == "BeginScript")
   {
      GmatCommand *endScript = GmatCommandUtil::GetMatchingEnd(prevCmd);
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage
         ("   setting prevCmd to '%s'\n", prevCmd->GetTypeName().c_str());
      #endif
      prevCmd = endScript;
   }
   
   
   //@Note "BeginFiniteBurn" is not a branch command but it pairs with "EndFiniteBurn"
   
   //------------------------------------------------------------
   // Create End* command if branch command
   //------------------------------------------------------------
   if (cmdTypeName == "Target" || cmdTypeName == "For"  ||
       cmdTypeName == "While"  ||  cmdTypeName == "If"  ||
       cmdTypeName == "ScriptEvent" || cmdTypeName == "Optimize")
   {
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   Creating End* for '%s'\n", cmdTypeName.c_str());
      #endif
      
		endCmd = CreateEndCommand(cmdTypeName, endType);
		
      #if DEBUG_MISSION_TREE_INSERT
      if (endCmd != NULL)
         MessageInterface::ShowMessage
            ("   '%s' created\n", endCmd->GetTypeName().c_str());
      #endif
      
      // create Else for IfElse
      if (cmdName == "IfElse")
      {
         elseCmd = CreateCommand("Else");
         cmd->Append(elseCmd);
      }
      
      #if DEBUG_MISSION_TREE_INSERT
      MessageInterface::ShowMessage
         ("   ==> Calling cmd->Append('%s')\n", endCmd->GetTypeName().c_str());
      #endif
      
      cmdAdded = cmd->Append(endCmd);
      
      #if DEBUG_MISSION_TREE_INSERT
      WriteCommand("   ", "previous of ", endCmd , " is ", endCmd->GetPrevious());
      #endif
      
   }
   
   // Compose node name
   nodeName = ComposeNodeName(cmd, cmdCount);
   
   // Insert command to mission sequence
   cmdAdded = InsertCommandToSequence(currCmd, prevCmd, cmd);
   
   if (cmdAdded)
   {
      // Insert command to mission tree
      node = InsertNodeToTree(parentId, currId, prevId, icon, nodeName, itemType, cmd,
                              currCmd, prevCmd, insertBefore);
      
      // Append End* to branch node
      if (cmdTypeName == "Target" || cmdTypeName == "For"  ||
          cmdTypeName == "While"  || cmdTypeName == "If"   ||
          cmdTypeName == "Optimize")
      {
         AppendEndToBranch(node, icon, cmdTypeName, cmdName, elseCmd, endCmd,
                           endType, cmdCount);
      }
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Command:'%s' not appended or created\n",
          cmd->GetTypeName().c_str());
   }
   
   // Change command item text color (LOJ: 2013.12.11)
   ChangeCommandNodeColor(cmd, node);
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("MissionTree::InsertCommand() leaving\n");
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// void Append(const wxString &cmdTypeName)
//------------------------------------------------------------------------------
/*
 * Appends a command to the end of the branch identified by parent.
 * The parent is the current selection, assuming Append menu item only appears
 * on branch command, such as Target, If, For, Optimize.
 * It sets parent item id, current item id, and previous item id and pass them
 * to InsertCommand() with insertBefore flag to true.
 *
 * @param  cmdName  Command type name to be appended
 */
//------------------------------------------------------------------------------
void MissionTree::Append(const wxString &cmdTypeName)
{
   wxTreeItemId currId = GetSelection();
   wxString itemText = GetItemText(currId);
   
   wxString actionStr;
   actionStr.Printf("Append '%s' to '%s'\n", cmdTypeName.c_str(), itemText.c_str());
   #if DEBUG_MISSION_TREE_APPEND
	MessageInterface::ShowMessage
		("MissionTree::Append() entered, %s", actionStr.c_str());
	#endif
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
      WriteActions(str);
   #endif
   
   wxTreeItemId lastChildId = GetLastChild(currId);
   wxTreeItemId parentId = GetItemParent(currId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId);
   GmatCommand *currCmd = currItem->GetCommand();
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Appending '" + cmdTypeName + "' to '" + itemText + "'");
   #endif
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("\nMissionTree::Append() cmdTypeName='%s'\n", cmdTypeName.c_str());
   WriteNode(2, "   ", false, "currId", currId, "lastChildId", lastChildId);
   WriteCommand("   ", "currCmd = ", currCmd);
   #endif
	
	
	// Now Else part is not indented (LOJ: 2011.12.02)
   //======================================================================
   // Note:
   // Previous command is the 2nd last visible command from the current node
   // For example:
   // Target
   //    If          <-- If appending command, previous command should be Else (2012.02.24)
   //       Maneuver
   //       Else     <-- There is no Append for Else
   //       Propagate
   //       EndIf
   //    EndTarget
   //======================================================================
   
   wxTreeItemId prevId;
   
   if (lastChildId.IsOk() && GetItemText(lastChildId) != "")
   {
      prevId = GetPrevSibling(lastChildId);
      if (!prevId.IsOk())
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   ==> There is no previous sibling, so assigning prevId to currId\n");
         #endif
         prevId = currId;
      }
   }
   else
      prevId = currId;
   
   
   #if DEBUG_MISSION_TREE_APPEND
   WriteNode(1, "   ", false, "prevId", prevId);
   #endif
   
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = prevItem->GetCommand();
   GmatCommand *cmd = NULL;
   
   #if DEBUG_MISSION_TREE_APPEND
   WriteCommand("   ", "currCmd = ", currCmd, ", prevCmd = ", prevCmd);
   #endif
   
   bool insertBefore = false;
   
   // For BranchCommand, use GmatCommandUtil::GetMatchingEnd() to get
   // previous command, since there may be commands not shown on the tree.
   // Note: Make sure this works on other than Windows also (loj: 12/15/06)
   if (currCmd->IsOfType("BranchCommand"))
   {
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   ==> current item is BranchCommand\n");
      #endif
      
      parentId = currId;
      // If selected command is BranchCommand, make currId to last child, so that
      // append item acts like inserting item just before the last child which is
      // BranchEnd command
      currId = GetLastChild(currId);
      
      GmatCommand *branchEnd = GmatCommandUtil::GetMatchingEnd(currCmd);
		GmatCommand *realPrevCmd = branchEnd->GetPrevious();
      
      // If there is Else node, append just before Else
      wxTreeItemId elseId = FindElse(parentId);
      if (elseId.IsOk())
      {
         MissionTreeItemData *elseItem = (MissionTreeItemData *) GetItemData(elseId);
         GmatCommand *elseCmd = elseItem->GetCommand();
         realPrevCmd = elseCmd->GetPrevious();
         wxTreeItemId realPrevId = GetPrevSibling(elseId);
         if (realPrevId.IsOk())
            prevId = realPrevId;
         insertBefore = true;
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   ==> Else found\n");
         WriteCommand("   ", "elseCmd = ", elseCmd, ", realPrevCmd = ", realPrevCmd);
         #endif
      }
      
      #if DEBUG_MISSION_TREE_APPEND
      WriteCommand("   ", "branchEnd = ", branchEnd, ", realPrevCmd = ", realPrevCmd);
		MessageInterface::ShowMessage("   ==> setting prevCmd to realPrevCmd\n");
      #endif
		
		prevCmd = realPrevCmd;
		
      #if DEBUG_MISSION_TREE_APPEND
      WriteCommand("   ", "prevCmd = ", prevCmd);
      #endif
      
      // If previous command is BranchCommand and not current command
      if (prevCmd->IsOfType("BranchCommand") && prevCmd != currCmd)
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   previous command is '%s' and not '%s'\n", 
             prevCmd->GetTypeName().c_str(), currCmd->GetTypeName().c_str());
         #endif
         
         prevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
         
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   so setting prevCmd to '%s'\n", prevCmd->GetTypeName().c_str(),
             currCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str());
         #endif
      }
   }
   else if (currId == mMissionSeqSubId)
   {
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage("   current item is MissionSequence\n");
      #endif
      
      parentId = currId;
      if (lastChildId.IsOk() && GetItemText(lastChildId) != "")
         currId = lastChildId;
      prevId = currId;
      prevCmd = prevItem->GetCommand();
   }
   else
   {
      #if DEBUG_MISSION_TREE_APPEND
      MessageInterface::ShowMessage
         ("   current Item is not BranckCommand, Else, or MissionSequence\n");
      #endif
      
      // handle case prevItem is NULL
      if (prevItem != NULL)
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   setting prevCmd from prevItem\n");
         #endif
         prevCmd = prevItem->GetCommand();
      }
      else
      {
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage("   setting prevCmd from currItem\n");
         #endif
         prevCmd = currItem->GetCommand();
      }
   }
   
   #if DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("   currCmd='%s', prevCmd='%s'\n",  currCmd->GetTypeName().c_str(),
       prevCmd->GetTypeName().c_str());
   #endif
   
   // Create a new command
   cmd = CreateCommand(cmdTypeName, currCmd);
   
   // Insert a node to tree
   if (cmd != NULL)
   {
      UpdateGuiManager(cmd);
      
      if (currCmd->GetTypeName() == "NoOp" ||
          currCmd->GetTypeName() == "BeginMissionSequence")
      {
         // Use GetLastCommand() to get last command since some command
         // doesn't appear on the tree, such as EndScript
         prevCmd = GmatCommandUtil::GetLastCommand(prevCmd);
         #if DEBUG_MISSION_TREE_APPEND
         MessageInterface::ShowMessage
            ("   Current command is NoOp or BeginMissionSequence\n");
         WriteCommand("   ==>", " new prevCmd = ", prevCmd);
         #endif
         
         if (prevCmd->IsOfType("BranchCommand"))
             prevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
      }
      
      // Need to set previous command of new command
      cmd->ForceSetPrevious(prevCmd);
      
      #if DEBUG_MISSION_TREE_APPEND
      WriteNode(3, "   ==> Calling InsertCommand() ", false, "parentId", parentId,
                "currId", currId, "prevId", prevId);
      #endif
      
      // Added to tree node if visible command
      wxTreeItemId node =
         InsertCommand(parentId, currId, prevId, GetIconId(cmdTypeName),
                       GetCommandId(cmdTypeName), cmdTypeName, prevCmd, cmd,
                       GetCommandCounter(cmd), insertBefore);
      
      Expand(currId);
      ////Expand(node);
      ExpandNode(node, cmdTypeName);
      SelectItem(node);
   }
   
   if (mWriteMissionSeq)
      ShowCommands("After Appending '" + cmdTypeName + "' to '" + itemText + "'");
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
}


//------------------------------------------------------------------------------
// void InsertBefore(const wxString &cmdTypeName)
//------------------------------------------------------------------------------
/*
 * Inserts a command before current selection.
 *
 * @param  cmdTypeName  Command type name to be inserted
 */
//------------------------------------------------------------------------------
void MissionTree::InsertBefore(const wxString &cmdTypeName)
{
   wxTreeItemId currId = GetSelection();
   wxString itemText = GetItemText(currId);
	
	wxString actionStr;
	actionStr.Printf("Insert '%s' before '%s'\n", cmdTypeName.c_str(), itemText.c_str());
   #if DEBUG_MISSION_TREE_INSERT
	MessageInterface::ShowMessage
		("MissionTree::InsertBefore() entered, %s", actionStr.c_str());
	#endif
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
      WriteActions(actionStr);
   #endif
	
   wxTreeItemId parentId = GetItemParent(currId);
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Inserting '" + cmdTypeName + "' before '" + itemText + "'");
   #endif
   
   wxTreeItemId prevId = GetPrevSibling(currId);
	
   // Check if no previous sibling then get parent item
   if (!prevId.IsOk())
   {
	   #if DEBUG_MISSION_TREE_INSERT
		MessageInterface::ShowMessage
			("   ==> There is no previous sibling, so assigning prevId to parentId\n");
		#endif
      if (parentId.IsOk())
         prevId = parentId;
   }
	
	#if DEBUG_MISSION_TREE_INSERT
   WriteNode(3, "   ", false, "parentId", parentId, "currId", currId, "prevId", prevId);
   #endif
	
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   
   // Do not insert anything if previous item is empty
   if (prevItem == NULL)
   {
      MessageInterface::ShowMessage
         ("\n***************  Warning ***************"
          "\nMissionTree::InsertBefore() has empty prevItem "
          "so it can't insert before this."
          "\n****************************************");
      return;
   }
   
   GmatCommand *currCmd = currItem->GetCommand();
   GmatCommand *prevCmd = currCmd->GetPrevious();
   GmatCommand *realPrevCmd = currCmd->GetPrevious();
   GmatCommand *cmd = NULL;
   
   // We want to use real previous command via cmd->GetPrevious(), not from
   // the tree, because some commands are not visible from the tree.
      
   if (currCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "Current item has empty command. Cannot insert the command.\n");
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   currCmd = '%s'(%p)\n", currCmd->GetTypeName().c_str(), currCmd);
   WriteCommand("   ", "prevCmd = ", prevCmd, ", realPrevCmd = ", realPrevCmd);
   #endif
   
   if (prevCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "The previous command is empty. Cannot insert the command.\n");
      
      ShowCommands("Before Insert: " + cmdTypeName);
      MessageInterface::ShowMessage
         ("InsertBefore('%s') currCmd='%s', addr=%p\n",
          cmdTypeName.WX_TO_C_STRING, currCmd->GetTypeName().c_str(), currCmd);
      
      return;
   }
   
   // If previous command is BranchCmmand check to see if we need to use matching
   // BranchEnd as previous command
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("   Checking to see if it needs to use matching BranchEnd as previous command\n");
   #endif
   if (prevCmd->IsOfType("BranchCommand"))
   {
      // check if first child is current command
      if (prevCmd->GetChildCommand(0) == currCmd)
         realPrevCmd = prevCmd;
      else
         realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   WriteCommand("   ", "realPrevCmd = ", realPrevCmd);
   #endif
   
	
   if (realPrevCmd != NULL)
   {
      GmatTree::ItemType itemType = currItem->GetItemType();
      GmatTree::ItemType solverItemType = itemType;
      wxTreeItemId branchId;
      GmatCommand *branchCmd = NULL;
      // Get parent branch command if exist
      IsInsideSolverBranch(currId, itemType, solverItemType, branchId, &branchCmd);
      #if DEBUG_MISSION_TREE_INSERT
      WriteCommand("   ", "branchCmd = ", branchCmd);
      #endif
      
      cmd = CreateCommand(cmdTypeName, branchCmd);
      
      if (cmd != NULL)
      {
         // Set previous command to realPrevCmd (loj: 2007.05.16)
         cmd->ForceSetPrevious(realPrevCmd);
         
         #if DEBUG_MISSION_TREE_INSERT
         WriteCommand("   ", "cmd->GetPrevious() = ", cmd->GetPrevious());
         #endif
         
         UpdateGuiManager(cmd);
         
			bool insertBefore = true;
			if (realPrevCmd->IsOfType("BranchEnd"))
				insertBefore = false;
			
         #if DEBUG_MISSION_TREE_INSERT
         WriteNode(3, "   ==> Calling InsertCommand() ", false, "parentId", parentId,
                   "currId", currId, "prevId", prevId);
         #endif
			
         wxTreeItemId node =
            InsertCommand(parentId, currId, prevId, GetIconId(cmdTypeName),
                          GetCommandId(cmdTypeName), cmdTypeName, realPrevCmd, cmd,
                          GetCommandCounter(cmd), insertBefore);
         
         ////Expand(parentId);
         ////Expand(prevId);
         Expand(node);
         SelectItem(node);
      }
   }
   
   if (mWriteMissionSeq)
      ShowCommands("After Inserting '" + cmdTypeName + "' before '" + itemText + "'");
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
} // InsertBefore()


//------------------------------------------------------------------------------
// void InsertAfter(const wxString &cmdTypeName)
//------------------------------------------------------------------------------
/*
 * Inserts a command after current selection.
 *
 * @param  cmdTypeName  Command name to be inserted
 */
//------------------------------------------------------------------------------
void MissionTree::InsertAfter(const wxString &cmdTypeName)
{
   wxTreeItemId currId = GetSelection();
   wxString itemText = GetItemText(currId);
   
	wxString actionStr;
	actionStr.Printf("Insert '%s' after '%s'\n", cmdTypeName.c_str(), itemText.c_str());
   #if DEBUG_MISSION_TREE_INSERT
	MessageInterface::ShowMessage
		("MissionTree::InsertAfter() entered, %s", actionStr.c_str());
	#endif
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
      WriteActions(actionStr);
   #endif
   
   wxTreeItemId parentId = GetItemParent(currId);
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Inserting '" + cmdTypeName + "' after '" + itemText + "'");
   #endif
   
	wxTreeItemId prevId = currId;
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId);
   GmatCommand *currCmd = currItem->GetCommand();
   
   if (currCmd == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "*** Internal Error Occurred ***\n"
          "Current item has empty command. Cannot insert the command.\n");
      return;
   }
   
   #if DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("MissionTree::InsertAfter('%s') currCmd='%s'(%p)\n", cmdTypeName.c_str(),
       currCmd->GetTypeName().c_str(), currCmd);
   #endif
   
   GmatCommand *prevCmd = currCmd;   
   GmatCommand *cmd = NULL;
   
   if (currCmd != NULL)
   {
      GmatTree::ItemType itemType = currItem->GetItemType();
      GmatTree::ItemType solverItemType = itemType;
      wxTreeItemId branchId;
      GmatCommand *branchCmd = NULL;
      // Get parent branch command if exist
      IsInsideSolverBranch(currId, itemType, solverItemType, branchId, &branchCmd);
      cmd = CreateCommand(cmdTypeName, branchCmd);
      
      // Set parentId, currId, prevId properly to pass to InsertCommand()
      // If current node is BranchCommand, it inserts after BranchEnd (2011.09.27 new Requirement)
      if (currCmd->IsOfType("BranchCommand"))
      {
         GmatCommand *branchEnd = NULL;
			branchEnd = GmatCommandUtil::GetMatchingEnd(currCmd);
         #if DEBUG_MISSION_TREE_INSERT
			MessageInterface::ShowMessage
				("   ==> Insert after BranchCommand, branchEnd=<%p><%s>\n",
				 branchEnd, branchEnd->GetTypeName().c_str());
         #endif
			cmd->ForceSetPrevious(branchEnd);
			prevCmd = branchEnd;
			prevId = currId;
			parentId = GetItemParent(currId);
      }
      else if (currCmd->IsOfType("BranchEnd"))
      {
         cmd->ForceSetPrevious(currCmd);

			// If inserting after BranchEnd, insert it after Branch command
         #if DEBUG_MISSION_TREE_INSERT
			MessageInterface::ShowMessage("   ==> Insert after BranchEnd\n");
         #endif
			
			// If it is not a Else then reassign nodes, since Else is not a really
			// BranchEnd in the tree(LOJ: 2011.12.02)
			if (!currCmd->IsOfType("Else"))
			{
				currId = parentId;
				parentId = GetItemParent(currId);
				prevId = currId;
			}
      }
      else
      {
         cmd->ForceSetPrevious(currCmd);
         prevId = currId;
      }
      
      
      if (cmd != NULL)
      {
			UpdateGuiManager(cmd);
         
         #if DEBUG_MISSION_TREE_INSERT
         WriteNode(3, "   ==> Calling InsertCommand() ", false, "parentId", parentId,
                   "currId", currId, "prevId", prevId);
         #endif
         
         wxTreeItemId node =
            InsertCommand(parentId, currId, prevId, GetIconId(cmdTypeName),
                          GetCommandId(cmdTypeName), cmdTypeName, prevCmd, cmd,
                          GetCommandCounter(cmd), false);
         
         ////Expand(parentId);
         Expand(node);
         SelectItem(node);
      }
   }
   
   if (mWriteMissionSeq)
      ShowCommands("After Inserting '" + cmdTypeName + "' after '" + itemText + "'");
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
} // InsertAfter()


//------------------------------------------------------------------------------
// void DeleteCommand(const wxString &cmdName)
//------------------------------------------------------------------------------
/*
 * Deletes a command from the tree and command sequence.
 *
 * @param  cmdName  Command name to be deleted
 */
//------------------------------------------------------------------------------
void MissionTree::DeleteCommand(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("MissionTree::DeleteCommand() entered, cmdName='%s'\n", cmdName.c_str());
   #endif
   
   // Get selected item
   wxTreeItemId currId = GetSelection();
   wxTreeItemId parentId = GetItemParent(currId);
   wxString itemText = GetItemText(currId);
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Deleting '" + cmdName + "' from '" + itemText + "'");
   #endif
   
   #if DEBUG_MISSION_TREE_DELETE
   WriteNode(2, "   ", false, "currId", currId, "parentId", parentId);
   #endif
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions)
   {
      wxString str;
      str.Printf("Delete '%s' from '%s'\n", itemText.c_str(),
                 GetItemText(parentId).c_str());
      WriteActions(str);
   }
   #endif
   
   
   // delete from gui interpreter
   MissionTreeItemData *treeItem = (MissionTreeItemData *)GetItemData(currId);
   if (treeItem == NULL)
   {
      // write error message
      MessageInterface::ShowMessage
         ("\n*** ERROR *** could not delete '%s' due to NULL item\n",
         cmdName.WX_TO_C_STRING);
      return;
   }
   
   GmatCommand *theCmd = treeItem->GetCommand();  
   if (theCmd == NULL)
   {
      // write error message
      MessageInterface::ShowMessage
         ("\n*** ERROR *** could not delete '%s' due to NULL command\n",
         cmdName.WX_TO_C_STRING);
      return;
   }
   
   // save command type to check if there is no more of this command
   std::string cmdType = theCmd->GetTypeName();
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Calling theGuiInterpreter->DeleteCommand('%s')\n", theCmd->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("   Previous of '%s' is '%s'\n", theCmd->GetTypeName().c_str(),
       theCmd->GetPrevious()->GetTypeName().c_str());
   if (theCmd->GetNext() == NULL)
      MessageInterface::ShowMessage
         ("   Next of '%s' is NULL\n", theCmd->GetTypeName().c_str());
   else
      MessageInterface::ShowMessage
         ("   Next of '%s' is '%s'\n", theCmd->GetTypeName().c_str(),
          theCmd->GetNext()->GetTypeName().c_str());
   #endif
   
   GmatCommand *tmp = theGuiInterpreter->DeleteCommand(theCmd);
   if (tmp)
   {
      #if DEBUG_MISSION_TREE_DELETE
      MessageInterface::ShowMessage
         ("   About to delete <%p>'%s'\n", tmp, tmp->GetTypeName().c_str());
      #endif
      delete tmp;
      tmp = NULL;
   }
   
   // reset counter if there is no more of this command
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Checking if the command counter needs to be reset\n");
   #endif
   
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   std::string seqString = GmatCommandUtil::GetCommandSeqString(cmd, false, false, false);
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage("seqString:%s\n", seqString.c_str());
   #endif
   
   if (seqString.find(cmdType) == seqString.npos)
   {
      #if DEBUG_MISSION_TREE_DELETE
      MessageInterface::ShowMessage
         ("   Resetting the command counter of '%s'\n", cmdType.c_str());
      #endif
      ResetCommandCounter(cmdType.c_str());
   }
   
   // delete from tree - if parent only has 1 child collapse
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   Checking if the parent item needs to be collapsed\n");
   #endif
   if (GetChildrenCount(parentId) <= 1)
   {
      #if DEBUG_MISSION_TREE_DELETE
      WriteNode(1, "   About to collapse parent tree item ", false, "parentId", parentId);
      #endif
      this->Collapse(parentId);
   }
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("   About to delete tree item '%s'\n", itemText.c_str());
   #endif
   this->Delete(currId);
   
   if (mWriteMissionSeq)
      ShowCommands("After Deleting '" + cmdName + "' from '" + itemText + "'");
   
   #ifdef __TEST_MISSION_TREE_ACTIONS__
   if (mSaveActions || mPlaybackActions)
      WriteResults();
   #endif
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("MissionTree::DeleteCommand() leaving, cmdName='%s'\n", cmdName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void UpdateGuiManager(const GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Calls GuiItemManager to update corresponding object list to dynamically
 * show new objects.
 *
 * @param  cmd  Command pointer
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateGuiManager(const GmatCommand *cmd)
{
   #ifdef DEBUG_UPDATE_GUI_MANAGER
   MessageInterface::ShowMessage
      ("UpdateGuiManager() entered, this=<%p>, mIsMissionTreeDocked=%d\n",
       this, mIsMissionTreeDocked);
   #endif
   
   // Update GuiItemManger since default commands were created with new default
   // resources if needed
   if (cmd->IsOfType("BurnCommand") || cmd->IsOfType("Vary"))
      theGuiManager->UpdateBurn();
   
   if (cmd->IsOfType("SolverCommand"))
      theGuiManager->UpdateSolver();
   
   if (cmd->IsOfType("SubscriberCommand"))
      theGuiManager->UpdateSubscriber();
   
   // Always update parameter, since it is used in many commands
   theGuiManager->UpdateParameter();
   
   // If MissionTree is undocked, update ResourceTree
   // Fix for GMT-310 (LOJ: 2012.10.04)
   if (!mIsMissionTreeDocked)
      GmatAppData::Instance()->GetResourceTree()->UpdateResource(false);
   
   #ifdef DEBUG_UPDATE_GUI_MANAGER
   MessageInterface::ShowMessage
      ("UpdateGuiManager() leaving, this=<%p>, mIsMissionTreeDocked=%d\n",
       this, mIsMissionTreeDocked);
   #endif
}


//------------------------------------------------------------------------------
// void AddDefaultMission()
//------------------------------------------------------------------------------
/**
 * Adds a default mission to tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMission()
{
   //----- Mission Sequence
   
   wxTreeItemId mission =
      AddRoot(wxT("Mission"), -1, -1,
              new MissionTreeItemData(wxT("Mission"), GmatTree::MISSIONS_FOLDER));
   
   //-----------------------------------------------------------------
   #ifdef __ENABLE_MULTIPLE_SEQUENCE__
   //-----------------------------------------------------------------
   mMissionSeqTopId =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(wxT("Mission Sequence"),
                                         GmatTree::MISSION_SEQ_TOP_FOLDER));
   
   SetItemImage(mMissionSeqTopId, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   AddDefaultMissionSeq(mMissionSeqTopId);
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   mMissionSeqSubId =
      AppendItem(mission, wxT("Mission Sequence"), GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(wxT("Mission Sequence"),
                                         GmatTree::MISSION_SEQ_SUB_FOLDER));
   
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   UpdateCommand();
   if (theNotebook)
   {
       theNotebook->SetMissionTreeExpandLevel(10); // level > 3 expands all
   }
   theGuiInterpreter->ResetConfigurationChanged(false, true);
   
}

//------------------------------------------------------------------------------
// void AddDefaultMissionSeq(wxTreeItemId item)
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMissionSeq(wxTreeItemId item)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::AddDefaultMission() entered\n");
   #endif
   
   #ifdef __ENABLE_MULTIPLE_SEQUENCE
   StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::MISSION_SEQ);
   
   int size = itemNames.size();
   for (int i = 0; i<size; i++)
   {
      wxString objName = wxString(itemNames[i].c_str());
      AppendItem(item, wxT(objName), GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(wxT(objName),
                                         GmatTree::MISSION_SEQ_COMMAND));
   };
   #endif
   
   wxString name;   
   name.Printf("Sequence%d", ++mNumMissionSeq);
   
   mMissionSeqSubId =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
   
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
   
   Expand(item);
}

//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Adds icons to a list, so that they can be used in the tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddIcons()
{
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("MissionTree::AddIcons() entered, GmatTree::MISSION_ICON_COUNT=%d\n",
       GmatTree::MISSION_ICON_COUNT);
   #endif
   
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap* bitmaps[GmatTree::MISSION_ICON_COUNT];
   int index = 0;
   long bitmapType = wxBITMAP_TYPE_PNG;
   
   // Show hourglass temporarily busy cursor
   wxBusyCursor wait;
   
   // Icons should follow the order in GmatTreeItemData::MissionIconType.
   theGuiManager->LoadIcon("propagateevent", bitmapType, &bitmaps[index], propagateevent_xpm);
   theGuiManager->LoadIcon("target", bitmapType, &bitmaps[++index], target_xpm);
   theGuiManager->LoadIcon("ClosedFolder", bitmapType, &bitmaps[++index], ClosedFolder_xpm);
   theGuiManager->LoadIcon("file", bitmapType, &bitmaps[++index], file_xpm);
   theGuiManager->LoadIcon("OpenFolder", bitmapType, &bitmaps[++index], OpenFolder_xpm);
   
   theGuiManager->LoadIcon("whileloop", bitmapType, &bitmaps[++index], whileloop_xpm);
   theGuiManager->LoadIcon("forloop", bitmapType, &bitmaps[++index], forloop_xpm);
   theGuiManager->LoadIcon("if", bitmapType, &bitmaps[++index], if_xpm);
   theGuiManager->LoadIcon("scriptevent", bitmapType, &bitmaps[++index], scriptevent_xpm);
   theGuiManager->LoadIcon("varyevent", bitmapType, &bitmaps[++index], varyevent_xpm);
   
   theGuiManager->LoadIcon("achieveevent", bitmapType, &bitmaps[++index], achieveevent_xpm);
   theGuiManager->LoadIcon("deltav", bitmapType, &bitmaps[++index], deltav_xpm);
   theGuiManager->LoadIcon("mt_CallGmatFunction", bitmapType, &bitmaps[++index], mt_CallGmatFunction_xpm);
   theGuiManager->LoadIcon("mt_CallMatlabFunction", bitmapType, &bitmaps[++index], mt_CallMatlabFunction_xpm);
   theGuiManager->LoadIcon("nestreturn", bitmapType, &bitmaps[++index], nestreturn_xpm);
   theGuiManager->LoadIcon("saveobject", bitmapType, &bitmaps[++index], saveobject_xpm);
   
   theGuiManager->LoadIcon("equalsign", bitmapType, &bitmaps[++index], equalsign_xpm);
   theGuiManager->LoadIcon("toggle", bitmapType, &bitmaps[++index], toggle_xpm);
   theGuiManager->LoadIcon("beginfb", bitmapType, &bitmaps[++index], beginfb_xpm);
   theGuiManager->LoadIcon("endfb", bitmapType, &bitmaps[++index], endfb_xpm);
   theGuiManager->LoadIcon("report", bitmapType, &bitmaps[++index], report_xpm);
   theGuiManager->LoadIcon("write", bitmapType, &bitmaps[++index], mt_WriteCommand_xpm);

   theGuiManager->LoadIcon("mt_Set", bitmapType, &bitmaps[++index], mt_SetCommand_xpm);
   theGuiManager->LoadIcon("mt_Stop", bitmapType, &bitmaps[++index], mt_Stop_xpm);
   theGuiManager->LoadIcon("penup", bitmapType, &bitmaps[++index], penup_xpm);
   theGuiManager->LoadIcon("pendown", bitmapType, &bitmaps[++index], pendown_xpm);
   theGuiManager->LoadIcon("mt_MarkPoint", bitmapType, &bitmaps[++index], mt_MarkPoint_xpm);
   theGuiManager->LoadIcon("mt_ClearPlot", bitmapType, &bitmaps[++index], mt_ClearPlot_xpm);
   
   theGuiManager->LoadIcon("mt_Global", bitmapType, &bitmaps[++index], mt_Global_xpm);   
   theGuiManager->LoadIcon("mt_SaveMission", bitmapType, &bitmaps[++index], mt_SaveMission_xpm);   
   theGuiManager->LoadIcon("optimize", bitmapType, &bitmaps[++index], optimize_xpm);
   theGuiManager->LoadIcon("mt_Minimize", bitmapType, &bitmaps[++index], mt_Minimize_xpm);
   theGuiManager->LoadIcon("mt_NonlinearConstraint", bitmapType, &bitmaps[++index], mt_NonlinearConstraint_xpm);
   
   theGuiManager->LoadIcon("mt_RunSimulator", bitmapType, &bitmaps[++index], mt_RunSimulator_xpm);
   theGuiManager->LoadIcon("mt_RunEstimator", bitmapType, &bitmaps[++index], mt_RunEstimator_xpm);
   theGuiManager->LoadIcon("mt_FindEvents",   bitmapType, &bitmaps[++index], mt_Default_xpm);  // <<<<<<< need FindEvents bitmap
   theGuiManager->LoadIcon("mt_Default", bitmapType, &bitmaps[++index], mt_Default_xpm);
   
   // Rescale if bitmap size is not 16x16 and use high quality scale (LOJ: 2011.04.22)
   int w, h;
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
   {
      w = bitmaps[i]->GetWidth();
      h = bitmaps[i]->GetHeight();
      
      #ifdef DEBUG_ADD_ICONS
      MessageInterface::ShowMessage("   bitmaps[%2d], w=%d, h=%d\n", i, w, h);
      #endif
      
      wxImage image = bitmaps[i]->ConvertToImage();
      if (w != sizeW || h != sizeH)
      {
         #ifdef DEBUG_ADD_ICONS
         MessageInterface::ShowMessage("   rescaling image to %d x %d\n", sizeW, sizeH);
         #endif
         
         image.Rescale(sizeW, sizeH, wxIMAGE_QUALITY_HIGH);
      }
      
      images->Add(image);
      //images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   }
   
   AssignImageList(images);
   
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("MissionTree::AddIcons() exiting, %d icons added\n", index + 1);
   #endif
}


//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Brings up popup menu on a right click.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemRightClick(wxTreeEvent& event)
{
   //wxWidgets-2.6.3 does not need this but wxWidgets-2.8.0 needs to SelectItem
   SelectItem(event.GetItem());
   mLastClickPoint = event.GetPoint();
   mLastRightClickItemId = event.GetItem();
   ShowMenu(event.GetItem(), event.GetPoint());
}


//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * On a double click sends the TreeItemData to GmatMainFrame to open a new
 * window.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId currId = event.GetItem();
   
   OpenItem(currId);
}


//------------------------------------------------------------------------------
// void OnDoubleClick(wxMouseEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles double click on an item.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnDoubleClick(wxMouseEvent &event)
{
   #ifdef DEBUG_DOUBLE_CLICK
   MessageInterface::ShowMessage("MissionTree::OnDoubleClick() entered\n");
   #endif
   
   //wxPoint position = event.GetPosition();
   //MessageInterface::ShowMessage("Event position is %d %d\n", position.x, position.y );
   
   wxTreeItemId currId = GetSelection();
   mLastDoubleClickItemId = currId;
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(currId);
   MissionTreeItemData *parent = (MissionTreeItemData *)GetItemData(GetItemParent(currId));
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::OnDoubleClick() item='%s', parent='%s', theMainFrame=<%p>, "
       "theMainFrame->theMdiChildren=<%p>\n", item->GetTitle().c_str(), parent->GetTitle().c_str(),
       theMainFrame);
   #endif
   
   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetItemType() == GmatTree::VARY) &&
       (parent->GetItemType() == GmatTree::OPTIMIZE))
      item->SetItemType(GmatTree::OPTIMIZE_VARY);
   
   // Show panel here. because OnItemActivated() always collapse the node.
   item->SetNodeId(mLastDoubleClickItemId);
   theMainFrame->CreateChild(item);
   
   #ifdef DEBUG_DOUBLE_CLICK
   MessageInterface::ShowMessage("MissionTree::OnDoubleClick() leaving\n");
   #endif
   //CheckClickIn(position);
}


//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId id, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Creates and shows a popup menu.
 *
 * @param <id> input TreeItemId.
 * @param <pt> input location for popup menu.
 */
//------------------------------------------------------------------------------
void MissionTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
   MissionTreeItemData *treeItem = (MissionTreeItemData *)GetItemData(id);
   wxString title = treeItem->GetTitle();
   GmatTree::ItemType itemType = treeItem->GetItemType();
   wxTreeItemId parent = GetItemParent(id);
   
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::ShowMenu() itemType=%d, itemTitle='%s', mViewAll=%d, "
       "mUsingViewLevel=%d, mViewLevel=%d\n", itemType, title.c_str(), mViewAll,
       mUsingViewLevel, mViewLevel);
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;
   
   if (itemType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(MT_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
      menu.Enable(MT_ADD_MISSION_SEQ, FALSE);
   }
   else if (itemType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(MT_COLLAPSE, wxT("Collapse All"));
      menu.Append(MT_EXPAND, wxT("Expand All"));
      
      //if (mViewAll)
      //{
         menu.AppendSeparator();
         menu.Append(MT_APPEND, wxT("Append"), CreateSubMenu(itemType, APPEND));
         //}
      
      // If multiple sequence is enabled
      #ifdef __ENABLE_MULTIPLE_SEQUENCE__
      menu.Append(MT_DELETE, wxT("Delete"));
      #endif
      
      menu.Enable(MT_RENAME, FALSE);
      menu.AppendSeparator();
      menu.Append(MT_RUN, wxT("Run"));
      menu.AppendSeparator();
      
      menu.AppendCheckItem(MT_SHOW_DETAIL, wxT("Show Detail"));
      menu.Check(MT_SHOW_DETAIL, mShowDetailedItem);
      menu.Append(MT_SHOW_MISSION_SEQUENCE, wxT("Show Mission Sequence"));
      menu.Append(MT_SHOW_SCRIPT, wxT("Show Script"));
      
      // We want to enable/disable these menu items depends on GUI savable/non-savable mode
      menu.Enable(MT_SHOW_MISSION_SEQUENCE, mEnableShowScript);
      menu.Enable(MT_SHOW_SCRIPT, mEnableShowScript);
      
      menu.AppendSeparator();
      menu.Append(MT_MISSION_SUMMARY_ALL, wxT("Mission Summary - All"));
      menu.Append(MT_MISSION_SUMMARY_PHYSICS, wxT("Mission Summary - Physics"));
      
      menu.AppendSeparator();
      menu.Append(MT_DOCK_MISSION_TREE, wxT("Dock Mission Tree"));
      menu.Append(MT_UNDOCK_MISSION_TREE, wxT("Undock Mission Tree"));
      
      //----- for auto testing actions
      #ifdef __TEST_MISSION_TREE_ACTIONS__
      menu.AppendSeparator();
      menu.Append(MT_START_SAVE_ACTIONS, wxT("Start Save Actions"));
      menu.Append(MT_STOP_SAVE_ACTIONS, wxT("Stop Save Actions"));
      menu.Append(MT_READ_ACTIONS, wxT("Playback Actions"));
      #endif
   }
   else
   {
      // add to non-EndBranch item
      if (itemType < GmatTree::BEGIN_NO_PANEL)
      {
         menu.Append(MT_OPEN, wxT("Open"));
         menu.Append(MT_CLOSE, wxT("Close"));
      }
      
      // if (mViewAll)
      // {
         menu.AppendSeparator();
         if (itemType == GmatTree::TARGET)
         {
            menu.Append(MT_APPEND, wxT("Append"),
                        CreateTargetSubMenu(itemType, APPEND));
            menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                        CreateSubMenu(itemType, INSERT_BEFORE));
            menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                        CreateSubMenu(itemType, INSERT_AFTER));
         }
         else if (itemType == GmatTree::OPTIMIZE)
         {
            menu.Append(MT_APPEND, wxT("Append"),
                        CreateOptimizeSubMenu(itemType, id, APPEND));
            menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                        CreateSubMenu(itemType, INSERT_BEFORE));
            menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                        CreateSubMenu(itemType, INSERT_AFTER));
         }
         else if (itemType == GmatTree::END_TARGET)
         {
            menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                        CreateTargetSubMenu(itemType, INSERT_BEFORE));
            menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                        CreateSubMenu(itemType, INSERT_AFTER));
         }
         else if (itemType == GmatTree::END_OPTIMIZE)
         {
            menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                        CreateOptimizeSubMenu(itemType, GetItemParent(id), INSERT_BEFORE));
            menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                        CreateSubMenu(itemType, INSERT_AFTER));
         }
         else 
         {
            GmatTree::ItemType solverItemType = itemType;
            wxTreeItemId branchId;
            GmatCommand *branchCmd = NULL;
            if (IsInsideSolverBranch(id, itemType, solverItemType, branchId, &branchCmd))
            {
               #if DEBUG_MISSION_TREE_MENU
               MessageInterface::ShowMessage("   ===> item is inside a Solver command\n");
               #endif
               if (solverItemType == GmatTree::TARGET)
               {
                  menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                              CreateTargetSubMenu(itemType, INSERT_BEFORE));
                  menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                              CreateTargetSubMenu(itemType, INSERT_AFTER));
               }
               else if (solverItemType == GmatTree::OPTIMIZE)
               {
                  menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                              CreateOptimizeSubMenu(itemType, branchId, INSERT_BEFORE));
                  menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                              CreateOptimizeSubMenu(itemType, branchId, INSERT_AFTER));
               }
            }
            else
            {
               #if DEBUG_MISSION_TREE_MENU
               MessageInterface::ShowMessage("   ===> item is outside a Solver command\n");
               #endif
               menu.Append(MT_INSERT_BEFORE, wxT("Insert Before"),
                           CreateSubMenu(itemType, INSERT_BEFORE));
               menu.Append(MT_INSERT_AFTER, wxT("Insert After"),
                           CreateSubMenu(itemType, INSERT_AFTER));
            }
         // }
         
         #if DEBUG_MISSION_TREE_MENU
         MessageInterface::ShowMessage("   Now add Append if control logic\n");
         #endif
         // Append is allowed for the control logic
         if ((itemType == GmatTree::IF_CONTROL) ||
				 // ELSE is not a BranchCommand so commented out (LOJ: 2011.12.01)
             //(itemType == GmatTree::ELSE_CONTROL) ||
             (itemType == GmatTree::FOR_CONTROL) ||
             (itemType == GmatTree::WHILE_CONTROL))
         {
            // Use menu.Insert() to make Append appear before insert before/after
            // just like other branch command
            size_t insertPos = menu.GetMenuItemCount() - 2;
            #ifdef DEBUG_MISSION_TREE_MENU
            MessageInterface::ShowMessage("   ---> insertPos = %d\n", insertPos);
            #endif
            
            GmatTree::ItemType solverItemType = itemType;
            wxTreeItemId branchId;
            GmatCommand *branchCmd = NULL;
            if (IsInsideSolverBranch(id, itemType, solverItemType, branchId, &branchCmd))
            {
               #ifdef DEBUG_MISSION_TREE_MENU
               MessageInterface::ShowMessage("   ===> Inserting Append menu inside Solver command\n");
               #endif
               if (solverItemType == GmatTree::TARGET)
               {
                  menu.Insert(insertPos, MT_APPEND, wxT("Append"),
                              CreateTargetSubMenu(itemType, APPEND));   
               }
               else if (solverItemType == GmatTree::OPTIMIZE)
               {
                  menu.Insert(insertPos, MT_APPEND, wxT("Append"),
                              CreateOptimizeSubMenu(itemType, branchId, APPEND));   
               }
            }
            else
            {
               #ifdef DEBUG_MISSION_TREE_MENU
               MessageInterface::ShowMessage("   ===> Inserting Append menu outside Solver command\n");
               #endif
               menu.Insert(insertPos, MT_APPEND, wxT("Append"),
                           CreateSubMenu(itemType, APPEND));
            }
         }
      }
      
      #if DEBUG_MISSION_TREE_MENU
      MessageInterface::ShowMessage("   Now add Rename/Delete to all items\n");
      #endif
      // Delete applies to all, except End branch
      if (itemType < GmatTree::BEGIN_NO_PANEL || itemType == GmatTree::STOP)
      {
         menu.AppendSeparator();
         menu.Append(MT_RENAME, wxT("Rename\tF2"));
         menu.Append(MT_DELETE, wxT("Delete\tDEL"));
      }
      
      menu.AppendSeparator();
      menu.Append(MT_COMMAND_SUMMARY, wxT("Command Summary"));
      
   }
   
   PopupMenu(&menu, pt);
   
   
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// void OnAddMissionSeq(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
   wxTreeItemId currId = GetSelection();
   wxString name;
   
   name.Printf("Sequence%d", ++mNumMissionSeq);
   
   mMissionSeqSubId =
      AppendItem(currId, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubId, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   Expand(currId);
}


//------------------------------------------------------------------------------
// void OnPopupAppend(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnPopupAppend(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("MissionTree::OnPopupAppend() entered, event id = %d, itemStr = '%s'\n",
       event.GetId(), event.GetString().c_str());
   #endif
   
}


//------------------------------------------------------------------------------
// void OnAppend(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAppend(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage
      ("\n==========> MissionTree::OnAppend() entered, event id = %d\n", event.GetId());
   #endif
   
   int menuId = event.GetId();
   if (idCmdMap.find(menuId) != idCmdMap.end())
      Append(idCmdMap[menuId]);
   else
      MessageInterface::ShowMessage
         ("**** OnAppend() Error appending a command, cannot find menu id\n");
   
   #ifdef DEBUG_MISSION_TREE_APPEND
   MessageInterface::ShowMessage("==========> MissionTree::OnAppend() leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnInsertBefore(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertBefore(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("\n==========> MissionTree::OnInsertBefore() entered, event id = %d\n", event.GetId());
   #endif
   
   int menuId = event.GetId();   
   if (idCmdMap.find(menuId) != idCmdMap.end())
      InsertBefore(idCmdMap[menuId]);
   else
      MessageInterface::ShowMessage
         ("**** OnInsertBefore() Error inserting a command, cannot find menu id\n");
   
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("==========> MissionTree::OnInsertBefore() leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnInsertAfter(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertAfter(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage
      ("\n==========> MissionTree::OnInsertAfter() entered, event id = %d\n", event.GetId());
   #endif
   
   wxString cmdString = event.GetString();
   int menuId = event.GetId();
   if (idCmdMap.find(menuId) != idCmdMap.end())
      InsertAfter(idCmdMap[menuId]);
   else
      MessageInterface::ShowMessage
         ("**** OnInsertBefore() Error inserting a command, cannot find menu id\n");
   
   #ifdef DEBUG_MISSION_TREE_INSERT
   MessageInterface::ShowMessage("==========> MissionTree::OnInsertAfter() leaving\n\n");
   #endif
}


//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreateSubMenu(GmatTree::ItemType type, ActionType action)
//------------------------------------------------------------------------------
/*
 * Creates popup menu. It will create proper menu id depends on the input
 * action.
 *
 * @param  type  Command type id
 * @param  action  One of APPEND, INSERT_BEFORE, INSERT_AFTER
 */
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateSubMenu(GmatTree::ItemType type, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateSubMenu() type=%d, action=%d\n", type, action);
   #endif
   
   unsigned int i;
   wxMenu *menu = new wxMenu;
   
   if (!mUsingViewLevel)
   {
      wxString actionStr = "inserted";
      if (action == APPEND) actionStr = "appended";
      wxString menuText = "New command cannot be " + actionStr + " unless Category Filter is Off";
      menu->Append(-99, menuText);
      menu->Enable(-99, false);
   }
   else
   {
      for (i=0; i<mCommandList.GetCount(); i++)
		  if (!GmatGlobal::Instance()->IsHiddenCommand(mCommandList[i].c_str()))
			menu->Append(GetMenuId(mCommandList[i], action), mCommandList[i]);
      
      menu->Append(MT_CONTROL_LOGIC, "Control Logic",
                   CreateControlLogicSubMenu(type, action));
   }
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateTargetSubMenu(GmatTree::ItemType type, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateTargetSubMenu(GmatTree::ItemType type, ActionType action)
{
   wxMenu *menu;
   
   menu = CreateSubMenu(type, action);
   
   if (mUsingViewLevel)
   {
      menu->Append(GetMenuId("Vary", action), "Vary");
      menu->Append(GetMenuId("Achieve", action), "Achieve");
   }
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateOptimizeSubMenu(GmatTree::ItemType type, wxTreeItemId solverBranchId,
//                               ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateOptimizeSubMenu(GmatTree::ItemType type, wxTreeItemId solverBranchId,
                                           ActionType action)
{
   #ifdef DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateOptimizeSubMenu() entered, type=%d, action=%d, mUsingViewLevel=%d\n",
       type, action, mUsingViewLevel);
   #endif
   
   wxMenu *menu;
   
   menu = CreateSubMenu(type, action);
   
   if (mUsingViewLevel)
   {
      // Check if Minimize is already in the Optimize branch since only one Minimize
      // is allowed.      
      wxTreeItemId minimizeId = FindChild(solverBranchId, "Minimize", true, true);
      menu->Append(GetMenuId("Vary", action), "Vary");
      if (!minimizeId.IsOk())
         menu->Append(GetMenuId("Minimize", action), "Minimize");
      menu->Append(GetMenuId("NonlinearConstraint", action), "NonlinearConstraint");
   }
   
   #ifdef DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateOptimizeSubMenu() returning menu<%p>\n", menu);
   #endif
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateControlLogicSubMenu(GmatTree::ItemType type, ActionType action)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateControlLogicSubMenu(GmatTree::ItemType type,
                                               ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateControlLogicSubMenu() type=%d, action=%d\n",
       type, action);
   #endif
   
   wxMenu *menu = new wxMenu;
   
   menu->Append(GetMenuId("If", action), wxT("If"));
   menu->Append(GetMenuId("IfElse", action), wxT("If-Else"));
   menu->Append(GetMenuId("For", action), wxT("For"));
   menu->Append(GetMenuId("While", action), wxT("While"));
   
   return menu;
}


//------------------------------------------------------------------------------
// void OnBeginEditLabel(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles EVT_TREE_BEGIN_LABEL_EDIT for beginning editing a label.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnBeginEditLabel(wxTreeEvent& event)
{
   // if panel is currently opened give warning and veto
   wxTreeItemId currId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(currId);
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be renamed while panel is opened");
      wxLog::FlushActive();
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void OnEndEditLabel(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles EVT_TREE_END_LABEL_EDIT for ending editing a label.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnEndEditLabel(wxTreeEvent& event)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnEndEditLabel() entered\n");
   #endif
   
   wxString newLabel = event.GetLabel();
   wxTreeItemId currId = event.GetItem();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(currId);
   GmatCommand *cmd = item->GetCommand();
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   old cmd name = '%s'\n", cmd->GetName().c_str());
   #endif
   
   item->SetName(newLabel);
   item->SetTitle(newLabel);
   cmd->SetName(newLabel.c_str());
   cmd->SetSummaryName(newLabel.c_str());
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   new cmd name = '%s'\n", cmd->GetName().c_str());
   #endif
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnEndEditLabel() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnRename()
//------------------------------------------------------------------------------
void MissionTree::OnRename(wxCommandEvent &event)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnRename() entered\n");
   #endif
   
   // get selected item
   wxTreeItemId currId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(currId);
   MissionTreeItemData *item = (MissionTreeItemData *)selItem;
   GmatCommand *cmd = item->GetCommand();
   wxString oldName = GetItemText(currId);
   
   // if panel is currently opened give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be renamed while panel is opened");
      wxLog::FlushActive();
      return;
   }
   
   if (cmd == NULL)
   {
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** Command for item '%s' is NULL\n", oldName.WX_TO_C_STRING);
      return;
   }
   
   // Do we want to use rename dialog here?
   //=================================================================
   #if 1
   //=================================================================
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   mLastClickPoint.x=%d, mLastClickPoint.y=%d\n",
       mLastClickPoint.x, mLastClickPoint.y);
   #endif
   
   // LOJ: 2012.01.27
   // Changed to use wxGetTextFromUser() to be consistent with ResoureTree rename
   //mLastClickPoint.y += 100;
   //ViewTextDialog renameDlg(this, wxT("Rename"), true, true, mLastClickPoint,
   //                         wxSize(100, -1), wxDEFAULT_DIALOG_STYLE);
   //renameDlg.AppendText(oldName);
   //renameDlg.ShowModal();
   
   //LOJ: 2012.02.15
   // Added a new method GetNameFromUser() and call this method
   //wxString newName =
   //   wxGetTextFromUser(wxT("Enter new command name:"), wxT("Rename"), oldName, this);
   
   wxString newName = oldName;
   wxString name;
   if (GetNameFromUser(name, oldName, "Enter new command name:", "Rename") == 1)
      newName = name;
   
   //if (newName != "")
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   Setting command name to '%s' for cmd=<%p>'%s'\n", newName.c_str(),
          cmd, cmd->GetTypeName().c_str());
      #endif
      SetItemText(currId, newName);
      item->SetName(newName);
      item->SetTitle(newName);
      cmd->SetName(newName.c_str());
      cmd->SetSummaryName(newName.c_str());
      theGuiInterpreter->ConfigurationChanged(cmd, true);
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   cmd=<%p>'%s'\n", cmd, cmd->GetName().c_str());
      #endif
      
      // If renamed Branch command find matching EndBranch and rename it also
      // (LOJ: 2010.01.27 for bug GMT-209 fix)
      if (cmd->IsOfType("BranchCommand"))
      {
         GmatCommand *branchEnd = GmatCommandUtil::GetMatchingEnd(cmd);
         if (branchEnd != NULL)
         {
            wxString endItemText =  (branchEnd->GetSummaryName()).c_str();
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   branchEnd=<%p><%s>'%s'\n", branchEnd, branchEnd->GetName().c_str(),
                endItemText);
            #endif
            
            // Use summary name for finding the child
            wxTreeItemId endId = FindChild(currId, endItemText, true);
            if (endId.IsOk())
            {
               #ifdef DEBUG_RENAME
               MessageInterface::ShowMessage("   endId='%s'\n", GetItemText(endId).c_str());
               #endif
               MissionTreeItemData *endItem = (MissionTreeItemData *) GetItemData(endId);
               wxString newEndName = "End " + newName;
               SetItemText(endId, newEndName);
               endItem->SetName(newEndName);
               endItem->SetTitle(newEndName);
               //branchEnd->SetName(newEndName.c_str());
               branchEnd->SetSummaryName(newEndName.c_str());
               
               // Handle Else node
               wxTreeItemId elseId = FindElse(currId);
               if (elseId.IsOk())
               {
                  #ifdef DEBUG_RENAME
                  MessageInterface::ShowMessage("   elseId='%s'\n", GetItemText(elseId).c_str());
                  #endif
                  // Get matching Else command
                  GmatCommand *branchElse = GmatCommandUtil::GetMatchingEnd(cmd, true);
                  MissionTreeItemData *elseItem = (MissionTreeItemData *) GetItemData(elseId);
                  wxString newElseName = "Else " + newName;
                  SetItemText(elseId, newElseName);
                  elseItem->SetName(newElseName);
                  elseItem->SetTitle(newElseName);
                  //branchElse->SetName(newElseName.c_str());
                  branchElse->SetSummaryName(newElseName.c_str());
               }
            }
            else
            {
               MessageInterface::ShowMessage
                  ("**** INTERNAL ERROR **** BranchEnd of '%s' not found\n",
                   newName.WX_TO_C_STRING);
            }
         }
      }
   }
   
   //=================================================================
   #else
   //=================================================================
   
   //@note
   // To enable this function, xTR_EDIT_LABELS style must be set when creating
   // MissionTree in GmatNotebook
   EditLabel(currId);   
   
   //=================================================================
   #endif
   //=================================================================
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("OnRename() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnDelete()
//------------------------------------------------------------------------------
void MissionTree::OnDelete(wxCommandEvent &event)
{
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("\n==========> MissionTree::OnDelete() entered, event id = %d\n", event.GetId());
   #endif
   
   // get selected item
   wxTreeItemId currId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(currId);
   wxString cmdName = GetItemText(currId);
   
   // if panel is currently opened give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItem))
   {
      wxLogWarning(selItem->GetTitle() + " cannot be deleted "
                   "while panel is opened");
      wxLog::FlushActive();
      return;
   }
   
   int answer = wxMessageBox(wxT("Are you sure you want to delete " + cmdName + "?"),
                         wxT("Please confirm"), wxYES_NO);
   if (answer == wxNO) return;

   DeleteCommand(cmdName);
   
   #if DEBUG_MISSION_TREE_DELETE
   MessageInterface::ShowMessage
      ("==========> MissionTree::OnDelete() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun(wxCommandEvent &event)
{
   theGuiInterpreter->RunMission();
}


//---------------------------------------------------------------------------
// void OnShowDetail()
//--------------------------------------------------------------------------
void MissionTree::OnShowDetail(wxCommandEvent &event)
{
   mShowDetailedItem = event.IsChecked();
   UpdateMission(true);
}


//---------------------------------------------------------------------------
// void OnShowMissionSequence()
//--------------------------------------------------------------------------
void MissionTree::OnShowMissionSequence(wxCommandEvent &event)
{
   #ifdef DEBUG_SHOW_MISSION_SEQ
   MessageInterface::ShowMessage("MissionTree::OnShowMissionSequence() entered\n");
   #endif
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   std::string str = GmatCommandUtil::GetCommandSeqString(cmd, false, false, true, "   ");
   
   #ifdef DEBUG_SHOW_MISSION_SEQ
   MessageInterface::ShowMessage("CommandSeqString =%s", str.c_str());
   #endif
   
   if (str == "")
      return;
   
   ViewTextFrame *vtf =
      new ViewTextFrame(theMainFrame, _T("Show Mission Sequence"),
       50, 50, 800, 500, "Temporary", "Mission Sequence");
   
   vtf->AppendText(str.c_str());
   vtf->Show(true);
   
   #ifdef DEBUG_SHOW_MISSION_SEQ
   MessageInterface::ShowMessage("MissionTree::OnShowMissionSequence() leaving\n");
   #endif
}

//---------------------------------------------------------------------------
// void OnShowScript()
//--------------------------------------------------------------------------
void MissionTree::OnShowScript(wxCommandEvent &event)
{
   std::string str = theGuiInterpreter->GetScript();
   
   if (str == "")
      return;
   
   ViewTextFrame *vtf =
      new ViewTextFrame(theMainFrame, _T("Show Script"),
       50, 50, 800, 500, "Temporary", "Script");
   
   vtf->AppendText(str.c_str());
   vtf->Show(true);
   
}

//------------------------------------------------------------------------------
// void OnShowCommandSummary()
//------------------------------------------------------------------------------
void MissionTree::OnShowCommandSummary(wxCommandEvent &event)
{
   #ifdef DEBUG_CMD_SUMMARY
   MessageInterface::ShowMessage("OnShowCommandSummary() entered\n");
   #endif
   
   // get selected item
   wxTreeItemId currId = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(currId);
   MissionTreeItemData *item = (MissionTreeItemData *)selItem;
   GmatCommand *cmd = item->GetCommand();
   
   // open window to show command summary
   if (cmd != NULL)
   {
      try
      {
         wxString title = "Command Summary for ";
         if (cmd->GetName() != "")
            title += cmd->GetName().c_str();
         else
         {
            std::string cmdType = cmd->GetTypeName();
            if (cmdType == "BeginScript") cmdType = "ScriptEvent";
            title += cmdType.c_str();
         }

         // Handle special case of ScriptEvent - we need the Command Summary of the EndScript
         if (cmd->GetTypeName() == "BeginScript")
         {
            GmatCommand *endCmd = GmatCommandUtil::GetMatchingEnd(cmd);
            std::string endName = endCmd->GetSummaryName();
            #ifdef DEBUG_CMD_SUMMARY
            MessageInterface::ShowMessage("... summary name for BeginScript is \"%s\"\n", (cmd->GetSummaryName()).c_str());
            #endif
            endCmd->SetSummaryName(cmd->GetSummaryName());
            ShowSummaryDialog ssd(this, -1, title, endCmd);
            ssd.ShowModal();
            // Set the EndScript command summary name back to what it was
            endCmd->SetSummaryName(endName);
         }
         else
         {
            ShowSummaryDialog ssd(this, -1, title, cmd);
            ssd.ShowModal();
         }
      }
      catch (BaseException &be)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
      }
   }
   
   #ifdef DEBUG_CMD_SUMMARY
   MessageInterface::ShowMessage("OnShowCommandSummary() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void MissionTree::OnShowMissionSummaryAll()
//--------------------------------------------------------------------------
void MissionTree::OnShowMissionSummaryAll(wxCommandEvent &event)
{
   wxString title = "Mission Summary - All Commands";

   GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();;
   if (firstCmd)
   {
      ShowSummaryDialog ssd(this, -1, title, firstCmd, true, false);
      ssd.ShowModal();
   }
   else
   {
      std::string errmsg = "\'Mission Summary\' - unable to obtain pointer to first command.\n";
      MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
   }
}

//---------------------------------------------------------------------------
// void MissionTree::OnShowMissionSummaryPhysics()
//--------------------------------------------------------------------------
void MissionTree::OnShowMissionSummaryPhysics(wxCommandEvent &event)
{
   wxString title = "Mission Summary - Physics-Based Commands";

   GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();;
   if (firstCmd)
   {
      ShowSummaryDialog ssd(this, -1, title, firstCmd, true, true);
      ssd.ShowModal();
   }
   else
   {
      std::string errmsg = "\'Mission Summary\' - unable to obtain pointer to first command.\n";
      MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
   }
}


//---------------------------------------------------------------------------
// void OnDockUndockMissionTree(wxCommandEvent &event)
//---------------------------------------------------------------------------
void MissionTree::OnDockUndockMissionTree(wxCommandEvent &event)
{
   if (event.GetId() == MT_DOCK_MISSION_TREE)
   {
      theMainFrame->CloseChild("Mission", GmatTree::MISSION_TREE_UNDOCKED);
   }
   else if (event.GetId() == MT_UNDOCK_MISSION_TREE)
   {
      theNotebook->CreateUndockedMissionPanel();
   }
}


//---------------------------------------------------------------------------
// bool CheckClickIn(wxPoint position)
//--------------------------------------------------------------------------
bool MissionTree::CheckClickIn(wxPoint position)
{
   //MessageInterface::ShowMessage("Click position is %d %d\n", position.x, position.y );
   //MissionTreeItemData *missionTreeItem = (MissionTreeItemData*) GetFirstVisibleItem();
   wxTreeItemId visibleItemId = GetFirstVisibleItem();
   MissionTreeItemData *missionTreeItemData = 
      (MissionTreeItemData*) GetItemData(visibleItemId);
   //MessageInterface::ShowMessage("Got first visible");

   // loop through all the visible items on the mission tree
   // to compare the event click with the position of the box
   while (missionTreeItemData != NULL)
   {
      GmatTree::ItemType itemType = missionTreeItemData->GetItemType();
      // don't have to open any panels for top folders
      if ((itemType != GmatTree::MISSIONS_FOLDER)       &&
          (itemType != GmatTree::MISSION_SEQ_TOP_FOLDER)&&
          (itemType != GmatTree::MISSION_SEQ_SUB_FOLDER)&&
          (itemType != GmatTree::MISSION_SEQ_COMMAND))
      {
         // get the surrounding box to compare click and commands
         wxRect bound;
         int w, h;
       
         GetBoundingRect(visibleItemId, bound, TRUE);
         GetSize(&w, &h);
       
         // compare event click to see if it is in the box or the
         // icon which is size 16
         if ((position.x >= (bound.x - 16)) &&
             (position.x <= w-offset) &&
             (position.y <= bound.y+rowHeight+1) &&
             (position.y >= bound.y-1))
         {
            //MessageInterface::ShowMessage("\nInside a rect\n");
              
            // set this item selected
            SelectItem(visibleItemId);
              
            // now that we know it is in a box, check to see
            // which box it is in
            // we only need to compare the left and the right, because
            // we already know it is within the top and the bottom
          
            // get box width
            int boxWidth = GetParameter(BOXWIDTH);
 
            // box count is 2, rightmost is for variables
            // next is goals, and the rest is the cmd panel
            int boxNum = 0;
          
            // check if in variables
            if ((position.x <= w-offset-boxWidth*boxNum) &&
                (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside variables");
               MissionTreeItemData *item =
                  new MissionTreeItemData(wxT("Variables"),
                                          GmatTree::VIEW_SOLVER_VARIABLES);
               theMainFrame->CreateChild(item);
            }
            else if ((position.x <= w-offset-boxWidth*boxNum) &&
                     (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside goals");
               MissionTreeItemData *item =
                  new MissionTreeItemData(wxT("Goals"),
                                          GmatTree::VIEW_SOLVER_GOALS);
               theMainFrame->CreateChild(item);
            }
            else
            {
               //MessageInterface::ShowMessage("\nOpen regular panel");
               theMainFrame->CreateChild(missionTreeItemData);
            }
            
            // get out of while loop
            break;
         }
      }
      //MessageInterface::ShowMessage("Not equal to null");
      visibleItemId = GetNextVisible(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible id");
      missionTreeItemData = (MissionTreeItemData*) GetItemData(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible data");
   }

   return false;
}


//------------------------------------------------------------------------------
// void OnCollapse(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * This method collapses MissionTree.
 */
//------------------------------------------------------------------------------
void MissionTree::OnCollapse(wxCommandEvent &event)
{
   // Get selected item
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId selectId = currItem->GetId();
   wxTreeItemId currId = currItem->GetId();
   
   int numChildren = GetChildrenCount(currId);
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      wxTreeItemId childId = GetFirstChild(currId, cookie);
      
      while (childId.IsOk())
      {
         Collapse(childId);
         childId = GetNextChild(currId, cookie);
      }
   }
   
   ScrollTo(selectId);
}


//------------------------------------------------------------------------------
// void OnExpand(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * This method expands MissionTree.
 */
//------------------------------------------------------------------------------
void MissionTree::OnExpand(wxCommandEvent &event)
{
   // Get selected item
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId currId = currItem->GetId();
   
   ExpandAll();
   ScrollTo(currId);
}


//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnOpen(wxCommandEvent &event)
{
   wxTreeItemId currId = GetSelection();
   OpenItem(currId);
}


//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnClose(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *currItem = (GmatTreeItemData *) GetItemData(GetSelection());
   wxTreeItemId currId = currItem->GetId();
   
   int numChildren = GetChildrenCount(currId);
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      MissionTreeItemData *item;
      wxTreeItemId childId = GetFirstChild(currId, cookie);
      
      while (childId.IsOk())
      {
         item = (MissionTreeItemData *)GetItemData(childId);
         
         #if DEBUG_MISSION_TREE_DELETE
         MessageInterface::ShowMessage
            ("MissionTree::OnClose() while-loop, item->GetTitle(): \"%s\"\n",
             item->GetTitle().c_str());
         #endif
         
         if (theMainFrame->IsChildOpen(item))
            theMainFrame->CloseActiveChild();
         
         childId = GetNextChild(currId, cookie);
         
         #if DEBUG_MISSION_TREE_DELETE
         WriteNode(1, "MissionTree::OnClose()", false, "childId", childId);
         #endif
      }
   }
   
   
   // delete selected item panel, if its open, its activated
   if (theMainFrame->IsChildOpen(currItem))
      theMainFrame->CloseActiveChild();
   else
      return;
   
}


//------------------------------------------------------------------------------
// GmatTree::MissionIconType GetIconId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::MissionIconType MissionTree::GetIconId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::MISSION_ICON_PROPAGATE;
   if (cmd == "Maneuver")
      return GmatTree::MISSION_ICON_DELTA_V;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::MISSION_ICON_BEGIN_FB;
   if (cmd == "EndFiniteBurn")
      return GmatTree::MISSION_ICON_END_FB;
   if (cmd == "Target")
      return GmatTree::MISSION_ICON_TARGET;
   if (cmd == "EndTarget")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Optimize")
      return GmatTree::MISSION_ICON_OPTIMIZE;
   if (cmd == "EndOptimize")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Achieve")
      return GmatTree::MISSION_ICON_ACHIEVE;
   if (cmd == "Minimize")
      return GmatTree::MISSION_ICON_MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::MISSION_ICON_NONLINEAR_CONSTRAINT;
   if (cmd == "Vary")
      return GmatTree::MISSION_ICON_VARY;
   if (cmd == "Save")
      return GmatTree::MISSION_ICON_SAVE_OBJECT;
   if (cmd == "GMAT")
      return GmatTree::MISSION_ICON_ASSIGNMENT;
   if (cmd == "Equation")
      return GmatTree::MISSION_ICON_ASSIGNMENT;
   if (cmd == "Report")
      return GmatTree::MISSION_ICON_REPORT;
   if (cmd == "Write")
      return GmatTree::MISSION_ICON_WRITE;
   if (cmd == "Toggle")
      return GmatTree::MISSION_ICON_TOGGLE;
   if (cmd == "For")
      return GmatTree::MISSION_ICON_FOR;
   if (cmd == "EndFor")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "If")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "IfElse")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "Else")
      return GmatTree::MISSION_ICON_IF;
   if (cmd == "EndIf")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "While")
      return GmatTree::MISSION_ICON_WHILE;
   if (cmd == "EndWhile")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "CallGmatFunction")
      return GmatTree::MISSION_ICON_CALL_GMAT_FUNCTION;
   if (cmd == "CallMatlabFunction")
      return GmatTree::MISSION_ICON_CALL_MATLAB_FUNCTION;
   if (cmd == "Set")
      return GmatTree::MISSION_ICON_SET;
   if (cmd == "Stop")
      return GmatTree::MISSION_ICON_STOP;
   if (cmd == "BeginScript")
      return GmatTree::MISSION_ICON_SCRIPTEVENT;
   if (cmd == "ScriptEvent")
      return GmatTree::MISSION_ICON_SCRIPTEVENT;
   if (cmd == "PenUp")
      return GmatTree::MISSION_ICON_PEN_UP;
   if (cmd == "PenDown")
      return GmatTree::MISSION_ICON_PEN_DOWN;
   if (cmd == "MarkPoint")
      return GmatTree::MISSION_ICON_MARK_POINT;
   if (cmd == "ClearPlot")
      return GmatTree::MISSION_ICON_CLEAR_PLOT;
   if (cmd == "Global")
      return GmatTree::MISSION_ICON_GLOBAL;
   if (cmd == "SaveMission")
      return GmatTree::MISSION_ICON_SAVE_MISSION;
   if (cmd == "Minimize")
      return GmatTree::MISSION_ICON_MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::MISSION_ICON_NONLINEAR_CONSTRAINT;
   if (cmd == "RunSimulator")
      return GmatTree::MISSION_ICON_RUN_SIMULATOR;
   if (cmd == "RunEstimator")
      return GmatTree::MISSION_ICON_RUN_ESTIMATOR;
   if (cmd == "FindEvents")
      return GmatTree::MISSION_ICON_FIND_EVENTS;
   
   return GmatTree::MISSION_ICON_DEFAULT;
}


//------------------------------------------------------------------------------
// void OpenItem(wxTreeItemId currId)
//------------------------------------------------------------------------------
void MissionTree::OpenItem(wxTreeItemId currId)
{
   // get some info about this item
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(currId);
   MissionTreeItemData *parent = (MissionTreeItemData *)GetItemData(GetItemParent(currId));
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::OpenItem() item='%s' parent='%s'\n",
       item->GetTitle().c_str(), parent->GetTitle().c_str());
   #endif
   
   // Since VaryPanel is used for both Target and Optimize,
   // set proper id indicating Optimize Vary
   if ((item->GetItemType() == GmatTree::VARY) &&
       (parent->GetItemType() == GmatTree::OPTIMIZE))
      item->SetItemType(GmatTree::OPTIMIZE_VARY);
   
   theMainFrame->CreateChild(item);
}


//------------------------------------------------------------------------------
// wxString ComposeNodeName(GmatCommand *cmd, int cmdCount)
//------------------------------------------------------------------------------
wxString MissionTree::ComposeNodeName(GmatCommand *cmd, int cmdCount)
{
   #ifdef DEBUG_NODE_NAME
   WriteCommand("MissionTree::ComposeNodeName() entered, ", "cmd = ", cmd);
   MessageInterface::ShowMessage("   cmdCount=%d\n", cmdCount);
   #endif
   
   wxString nodeName = cmd->GetName().c_str();
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   #ifdef DEBUG_NODE_NAME
   MessageInterface::ShowMessage("MissionTree::ComposeNodeName() - nodeName    = '%s'\n", nodeName.c_str());
   MessageInterface::ShowMessage("MissionTree::ComposeNodeName() - cmdTypeName = '%s'\n", cmdTypeName.c_str());
   #endif
   
   if (cmdTypeName == "GMAT")
      cmdTypeName = "Equation";
   else if (cmdTypeName == "BeginScript")
      cmdTypeName = "ScriptEvent";
   
   // if command has NO name or command name is the type name then append counter
   if (nodeName.Trim() == "" || nodeName == cmdTypeName)
      nodeName.Printf("%s%d", cmdTypeName.c_str(), cmdCount);
   
   // Get long or short label
   nodeName = GetCommandString(cmd, nodeName);
   // Set summary name after getting new nodeName (Fix for GMT-2893, LOJ: 2012.12.13)
   cmd->SetSummaryName(nodeName.c_str());
   
   #ifdef DEBUG_NODE_NAME
   MessageInterface::ShowMessage
      ("MissionTree::ComposeNodeName() returning '%s'\n", nodeName.c_str());
   #endif
   
   return nodeName;
}


//------------------------------------------------------------------------------
// wxString GetCommandString(GmatCommand *cmd, const wxString &currStr)
//------------------------------------------------------------------------------
/*
 * Returns command string if command is not a BranchCommand or Begin/EndScript.
 */
//------------------------------------------------------------------------------
wxString MissionTree::GetCommandString(GmatCommand *cmd, const wxString &currStr)
{
   if (!mShowDetailedItem)
      return currStr;
   
   if (cmd->GetTypeName() == "BeginScript" || cmd->GetTypeName() == "EndScript")
      return currStr;
   
   wxString cmdString;
   cmdString = cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str();
   
   #ifdef DEBUG_NODE_NAME
   MessageInterface::ShowMessage("GetCommandString() cmdString='%s'\n", cmdString.c_str());
   #endif
   
   if (cmdString == ";")
      return currStr;
   else
      return cmdString;
}


//------------------------------------------------------------------------------
// GmatCommand* GetCommand(wxTreeItemId nodeId)
//------------------------------------------------------------------------------
/**
 * Returns GmatCommand pointer at given tree node.
 */
//------------------------------------------------------------------------------
GmatCommand* MissionTree::GetCommand(wxTreeItemId nodeId)
{
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(nodeId);
   GmatCommand *currCmd = currItem->GetCommand();
   return currCmd;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetCommandId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::ItemType MissionTree::GetCommandId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::PROPAGATE;
   if (cmd == "Maneuver")
      return GmatTree::MANEUVER;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::BEGIN_FINITE_BURN;
   if (cmd == "EndFiniteBurn")
      return GmatTree::END_FINITE_BURN;
   if (cmd == "Target")
      return GmatTree::TARGET;
   if (cmd == "EndTarget")
      return GmatTree::END_TARGET;
   if (cmd == "Optimize")
      return GmatTree::OPTIMIZE;
   if (cmd == "EndOptimize")
      return GmatTree::END_OPTIMIZE;
   if (cmd == "Achieve")
      return GmatTree::ACHIEVE;
   if (cmd == "Minimize")
      return GmatTree::MINIMIZE;
   if (cmd == "NonlinearConstraint")
      return GmatTree::NON_LINEAR_CONSTRAINT;
   if (cmd == "Vary")
      return GmatTree::VARY;
   if (cmd == "Save")
      return GmatTree::SAVE;
   if (cmd == "FindEvents")
      return GmatTree::FIND_EVENTS;
   if (cmd == "Global")
      return GmatTree::MANAGE_OBJECT;
   if (cmd == "Report")
      return GmatTree::REPORT;
//   if (cmd == "Write")
//      return GmatTree::WRITE_COMMAND;
   if (cmd == "For")
      return GmatTree::FOR_CONTROL;
   if (cmd == "EndFor")
      return GmatTree::END_FOR_CONTROL;
   if (cmd == "If")
      return GmatTree::IF_CONTROL;
   if (cmd == "IfElse")
      return GmatTree::IF_CONTROL;
   if (cmd == "Else")
      return GmatTree::ELSE_CONTROL;
   if (cmd == "EndIf")
      return GmatTree::END_IF_CONTROL;
   if (cmd == "While")
      return GmatTree::WHILE_CONTROL;
   if (cmd == "EndWhile")
      return GmatTree::END_WHILE_CONTROL;
   if (cmd == "CallGmatFunction")
      return GmatTree::CALL_FUNCTION;
   if (cmd == "CallMatlabFunction")
      return GmatTree::CALL_FUNCTION;
   if (cmd == "Set")
      return GmatTree::SET;
   if (cmd == "Stop")
      return GmatTree::STOP;
   if (cmd == "GMAT")
      return GmatTree::ASSIGNMENT;
   if (cmd == "Equation")
      return GmatTree::ASSIGNMENT;
   if (cmd == "BeginScript")
      return GmatTree::SCRIPT_EVENT;
   if (cmd == "MarkPoint" || cmd == "ClearPlot")
      return GmatTree::XY_PLOT_ACTION;
   if (cmd == "PenUp" || cmd == "PenDown")
      return GmatTree::PLOT_ACTION;
   if (cmd == "Toggle")
      return GmatTree::TOGGLE;
   
   return GmatTree::OTHER_COMMAND;
}


//------------------------------------------------------------------------------
// void CreateCommandIdMap()
//------------------------------------------------------------------------------
void MissionTree::CreateCommandIdMap()
{
   wxString cmd;
   int numCmd = 0;
   
   for (unsigned int i=0; i<mCommandListForViewControl.size(); i++)
   {
      cmd = mCommandListForViewControl[i];
      CreateMenuIds(cmd, i);
      numCmd++;
   }
   
   // Add IfElse command
   CreateMenuIds("IfElse", numCmd);
}


//------------------------------------------------------------------------------
// void CreateCommandCounterMap()
//------------------------------------------------------------------------------
void MissionTree::CreateCommandCounterMap()
{
   wxString cmd;
   
   for (unsigned int i=0; i<mCommandListForViewControl.size(); i++)
   {
      cmd = mCommandListForViewControl[i];
      cmdCounterMap.insert(std::make_pair(cmd, 0));
   }
   
   // Display is now removed from the unviewable list in the CommandFactory
   // so commented out (LOJ: 2016.01.28)
   //cmdCounterMap.insert(std::make_pair("Display", 0));  
   
   // Add command counter that has different display name
   cmdCounterMap.insert(std::make_pair("GMAT", 0));  
   cmdCounterMap.insert(std::make_pair("BeginScript", 0));  
   cmdCounterMap.insert(std::make_pair("IfElse", 0));
   
   // Add counter for EndBranch command so that AppendCommand()
   // can add proper counter when reloading the mission tree
   cmdCounterMap.insert(std::make_pair("EndIf", 0));
   cmdCounterMap.insert(std::make_pair("EndFor", 0));
   cmdCounterMap.insert(std::make_pair("EndWhile", 0));
   cmdCounterMap.insert(std::make_pair("EndTarget", 0));
   cmdCounterMap.insert(std::make_pair("EndOptimize", 0));
   
   #ifdef DEBUG_COMMAND_COUNTER
   MessageInterface::ShowMessage("===> cmdCounterMap = \n");
   for (std::map<wxString, int>::iterator pos = cmdCounterMap.begin();
        pos != cmdCounterMap.end(); ++pos)
      MessageInterface::ShowMessage("   '%s'\n", (pos->first).WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void CreateMenuIds(const wxString &cmd, int index)
//------------------------------------------------------------------------------
void MissionTree::CreateMenuIds(const wxString &cmd, int index)
{
   #if DEBUG_MISSION_TREE_MENU > 1
   MessageInterface::ShowMessage
      ("CreateMenuIds() entered, cmd='%s', index=%d\n", cmd.c_str(), index);
   #endif
   
   int id;
   wxString str, realCmd;
   realCmd = cmd;
   
   // If command to show is ScriptEvent, we want to create BeginScript
   if (cmd == "ScriptEvent")
      realCmd = "BeginScript";
   
   // Create id for append
   str = "AP*" + cmd;
   id = index + MT_BEGIN_APPEND;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
   
   // Create id for insert before
   str = "IB*" + cmd;
   id = index + MT_BEGIN_INSERT_BEFORE;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
   
   // Create id for insert after
   str = "IA*" + cmd;
   id = index + MT_BEGIN_INSERT_AFTER;
   cmdIdMap.insert(std::make_pair(str, id));
   idCmdMap.insert(std::make_pair(id, realCmd));
   
   #if DEBUG_MISSION_TREE_MENU > 1
   MessageInterface::ShowMessage
      ("CreateMenuIds() leaving, cmd='%s', index=%d\n", cmd.c_str(), index);
   #endif
}


//------------------------------------------------------------------------------
// int GetMenuId(const wxString &cmd, ActionType action)
//------------------------------------------------------------------------------
int MissionTree::GetMenuId(const wxString &cmd, ActionType action)
{
   #if DEBUG_MISSION_TREE_MENU > 1
   wxString actionString;
   if (action == 0)
      actionString = "Append";
   else if (action == 1)
      actionString = "InsertBefore";
   else if (action == 2)
      actionString = "InsertAfter";
   else
      actionString = "Unknown";
   MessageInterface::ShowMessage
      ("MissionTree::GetMenuId() cmd='%s', action=%d(%s)\n", cmd.c_str(),
       action, actionString.c_str());
   #endif
   
   int id = -1;
   
   wxString cmdStr = cmd;
   
   // Add prefix to command string
   if (action == APPEND)
      cmdStr = "AP*" + cmdStr;
   else if (action == INSERT_BEFORE)
      cmdStr = "IB*" + cmdStr;
   else if (action == INSERT_AFTER)
      cmdStr = "IA*" + cmdStr;
   
   // check if string is known command string
   if (cmdIdMap.find(cmdStr) != cmdIdMap.end())
   {
      id = cmdIdMap[cmdStr];
   }
   else
   {
      #if DEBUG_MISSION_TREE_MENU
      MessageInterface::ShowMessage
         ("**** MissionTree::GetMenuId() The '%s' is not recognized command\n",
          cmdStr.c_str());
      #endif
   }
   
   #if DEBUG_MISSION_TREE_MENU > 1
   MessageInterface::ShowMessage("MissionTree::GetMenuId() returning %d\n", id);
   #endif
   
   return id;
}

//------------------------------------------------------------------------------
// void PanelObjectChanged( GmatBase *obj )
// Called when user clicks Ok/Apply and successful
//------------------------------------------------------------------------------
void MissionTree::PanelObjectChanged( GmatBase *obj )
{
   wxTreeItemId itemId = FindChild(mMissionSeqSubId, (GmatCommand *) obj );
   if (itemId.IsOk())
   {
      MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
	  wxString oldLabel = GetItemText(itemId);
      GmatCommand *cmd = (GmatCommand *) obj;
      wxString newLabel = GetCommandString(cmd, oldLabel);
      
      if (newLabel != oldLabel)
      {
         item->SetName(newLabel);
         item->SetTitle(newLabel);
         SetItemText(itemId, newLabel);
      }
   }
}


//------------------------------------------------------------------------------
// void ResetCommandCounter(const wxString &cmd, bool resetAll = false)
//------------------------------------------------------------------------------
void MissionTree::ResetCommandCounter(const wxString &cmd, bool resetAll)
{
   #ifdef DEBUG_COMMAND_COUNTER
   MessageInterface::ShowMessage
      ("ResetCommandCounter() entered, cmd='%s', resetAll=%s\n", cmd.c_str(),
       resetAll ? "true" : "false");
   #endif
   
   if (resetAll)
   {
      for (std::map<wxString, int>::iterator pos = cmdCounterMap.begin();
           pos != cmdCounterMap.end(); ++pos)
      {
         pos->second = 0;
      }
   }
   else
   {
      // If command found in the map then reset the counter
      if (cmdCounterMap.find(cmd) != cmdCounterMap.end())
      {
         #ifdef DEBUG_COMMAND_COUNTER
         MessageInterface::ShowMessage
            ("   Resetting command counter for cmd='%s'\n", cmd.c_str());
         #endif
         cmdCounterMap[cmd] = 0;
         if (cmd == "GMAT")
            cmdCounterMap["Equation"] = 0;
      }
      else
      {
         #ifdef DEBUG_COMMAND_COUNTER
         MessageInterface::ShowMessage
            ("**** Cannot reset the counter for the command '%s'. "
             "It is not found in the map\n", cmd.c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_COMMAND_COUNTER
   MessageInterface::ShowMessage("ResetCommandCounter() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// int GetCommandCounter(GmatCommand *cmd)
//------------------------------------------------------------------------------
int MissionTree::GetCommandCounter(GmatCommand *cmd)
{
   int cmdCounter = 99;
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   
   // If command found in the map then increment the counter first
   if (cmdCounterMap.find(cmdTypeName) != cmdCounterMap.end())
   {
      cmdCounterMap[cmdTypeName] = cmdCounterMap[cmdTypeName] + 1;
      cmdCounter = cmdCounterMap[cmdTypeName];
   }
   else
   {
      #ifdef DEBUG_COMMAND_COUNTER
      MessageInterface::ShowMessage
         ("**** Cannot increment the counter for the command '%s'. "
          "It is not found in the map.\n",  cmdTypeName.WX_TO_C_STRING);
      #endif
   }
   
   return cmdCounter;
}


//------------------------------------------------------------------------------
// int GetNameFromUser(wxString &newName, const wxString &oldName = "",
//                     const wxString &msg = "", const wxString &caption = "")
//------------------------------------------------------------------------------
/**
 * Gets command name from a user until valid name is entered or canceld
 *
 * @return  1  Valid name entered
 *          2  User canceled or no changes made
 */
//------------------------------------------------------------------------------
int MissionTree::GetNameFromUser(wxString &newName, const wxString &oldName,
                                 const wxString &msg, const wxString &caption)
{
   newName = wxGetTextFromUser(msg, caption, oldName, this);
   
   // @note
   // There is no way of kwowing whether user entered blank and clicked OK
   // or just clicked Cancel, since wxGetTextFromUser() returns blank for
   // both cases. So if blank is returned, no change is assumed. (loj: 2008.08.29)
   if (newName == "")
      return 2;
   
   // Remove leading and trailing white spaces
   newName = newName.Strip(wxString::both);
   std::string stdNewName = newName.WX_TO_STD_STRING;
   std::string::size_type index1 = stdNewName.find('\'', 0);
   
   // single quote is not allowd, so check it first
   if (index1 != stdNewName.npos)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "\"%s\" - Single quotes within a command name is not allowed.\n"
          "Please reenter without single quotes.", newName.WX_TO_C_STRING);
      
      return GetNameFromUser(newName, newName, msg, caption);
   }
   else
   {
      return 1;
   }
}


//------------------------------------------------------------------------------
// wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmdName,
//                        bool useSummaryName, bool onlyCheckCmdType)
//------------------------------------------------------------------------------
/*
 * Finds a first item from the parent node of the tree. It compares item command name
 * and cmdName for finding the item.
 *
 * @param  parentId  Parent item id
 * @param  cmdName   Comand name or type name to find. If using command type name,
 *                         set onlyCheckCmdType flag to true
 * @param  useSummaryName  If this flag is set, it will use summary name to find
 *                         the item [false]
 * @param  onlyCheckCmdType If this flag is set, it will only check for the command
 *                         type name [false]
 *
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindChild(wxTreeItemId parentId, const wxString &cmd,
                                    bool useSummaryName, bool onlyCheckCmdType)
{
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("\nMissionTree::FindChild() entered, parentId=<%s>, cmd=<%s>\n",
       GetItemText(parentId).c_str(), cmd.c_str());
   #endif
   
   int numChildren = GetChildrenCount(parentId);
   wxTreeItemId childId;
   wxString childText;
   
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      childId = GetFirstChild(parentId, cookie);
      
      while (childId.IsOk())
      {
         //MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(childId);
         //GmatCommand *currCmd = currItem->GetCommand();
         GmatCommand *currCmd = GetCommand(childId);
         wxString currCmdType = currCmd->GetTypeName().c_str();
         wxString currCmdName = currCmd->GetName().c_str();
         if (useSummaryName)
            currCmdName = currCmd->GetSummaryName().c_str();
         
         childText = GetItemText(childId);
         
         #if DEBUG_MISSION_TREE_FIND > 1
         WriteNode(1, "---> ", false, "childId", childId);
         MessageInterface::ShowMessage("     cmdType = '%s'\n", currCmdType.c_str());
         MessageInterface::ShowMessage("     cmdName = '%s'\n", currCmdName.c_str());
         #endif
         
         if (onlyCheckCmdType && currCmdType == cmd)
            break;
         
         if (currCmdName == cmd)
            break;
         
         if (GetChildrenCount(childId) > 0)
		 {
            childId = FindChild(childId, cmd, useSummaryName, onlyCheckCmdType);
			if (childId.IsOk()) break;
		 }
         
         childId = GetNextChild(parentId, cookie);
      }
   }
   
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage("MissionTree::FindChild() returning ");
   WriteNode(1, "", false, "childId", childId);
   #endif
   
   return childId;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmdName,
//                        bool useSummaryName, bool onlyCheckCmdType)
//------------------------------------------------------------------------------
/*
 * Finds a first item from the parent node of the tree. It compares item command name
 * and cmdName for finding the item.
 *
 * @param  parentId  Parent item id
 * @param  cmdName   Comand name or type name to find. If using command type name,
 *                         set onlyCheckCmdType flag to true
 * @param  useSummaryName  If this flag is set, it will use summary name to find
 *                         the item [false]
 * @param  onlyCheckCmdType If this flag is set, it will only check for the command
 *                         type name [false]
 *
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindChild(wxTreeItemId parentId, GmatCommand *cmd)
{
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("\nMissionTree::FindChild() entered, parentId=<%s>, cmd=<%s>\n",
       GetItemText(parentId).c_str(), cmd->GetName().c_str());
   #endif
   
   int numChildren = GetChildrenCount(parentId);
   wxTreeItemId childId;
   wxString childText;
   
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      childId = GetFirstChild(parentId, cookie);
      
      while (childId.IsOk())
      {
         //MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(childId);
         //GmatCommand *currCmd = currItem->GetCommand();
         GmatCommand *currCmd = GetCommand(childId);
         if (currCmd == cmd)
            break;
         
         if (GetChildrenCount(childId) > 0)
		 {
            childId = FindChild(childId, cmd);
			if (childId.IsOk()) break;
		 }
         
         childId = GetNextChild(parentId, cookie);
      }
   }
   
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage("MissionTree::FindChild() returning ");
   WriteNode(1, "", false, "childId", childId);
   #endif
   
   return childId;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindElse(wxTreeItemId parentId)
//------------------------------------------------------------------------------
/*
 * Finds a Else from the children of the parent node of the tree. It checks
 * only one level.
 *
 * @param <parentId> Parent item id
 *
 * @return  id of Else node
 * 
 */
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindElse(wxTreeItemId parentId)
{
   #if DEBUG_MISSION_TREE_FIND
   WriteNode(1, "\nMissionTree::FindElse() entered, ", false, "parentId", parentId);
   #endif
   
   int numChildren = GetChildrenCount(parentId);
   wxTreeItemId childId;
   
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      childId = GetFirstChild(parentId, cookie);
      
      while (childId.IsOk())
      {
         MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(childId);
         GmatCommand *currCmd = currItem->GetCommand();
         wxString currCmdType = currCmd->GetTypeName().c_str();
         if (currCmdType == "Else")
            break;
         
         childId = GetNextChild(parentId, cookie);
      }
   }
   
   #if DEBUG_MISSION_TREE_FIND
   WriteNode(1, "MissionTree::FindElse() returning, ", true, "childId", childId);
   #endif
   
   return childId;
}


//------------------------------------------------------------------------------
// int FindItemPosition(wxTreeItemId parentId, wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Finds the position of the child item from the parent item
 *
 * @param  parentId  Parent item id
 * @param  itemId    Child item id to find position of
 *
 * @return  Position of the child item
 */
//------------------------------------------------------------------------------
int MissionTree::FindItemPosition(wxTreeItemId parentId, wxTreeItemId itemId)
{
   #if DEBUG_MISSION_TREE_FIND
   WriteNode(2, "\nMissionTree::FindItemPosition() entered, ", false,
             "parentId", parentId, "itemId", itemId);
   #endif
   
   int numChildren = GetChildrenCount(parentId);
   wxTreeItemId childId;
   int itemPos = 0;
   
   if (numChildren > 0)
   {
      wxTreeItemIdValue cookie;
      childId = GetFirstChild(parentId, cookie);
      while (childId.IsOk())
      {
         if (childId == itemId)
            break;
         
         itemPos++;         
         childId = GetNextChild(parentId, cookie);
      }
   }
   
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("MissionTree::FindItemPosition() returning %d\n\n", itemPos);
   #endif
   
   return itemPos;
}


//------------------------------------------------------------------------------
// bool IsElseNode(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Returns true if the node is Else command node, false otherwise
 */
//------------------------------------------------------------------------------
bool MissionTree::IsElseNode(wxTreeItemId itemId)
{
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
   GmatCommand *itemCmd = item->GetCommand();
   if (itemCmd->IsOfType("Else"))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsInsideSolverBranch(wxTreeItemId currId, GmatTree::ItemType &itemType, ...)
//------------------------------------------------------------------------------
/*
 * Checks if an item is inside of solver (Target, Optimize) branch and
 * assigns solverItemType to solver branch item type if item is inside a
 * solver branch.
 *
 * @parameter  currId  Id of the current node
 * @parameter  itemType  Item type of the current node
 * @parameter  solverItemType  Item type of the solver branch returned
 * @parameter  branchId  Id of the solver branch node returned
 * @parameter  branchCmd  Pointer of the solver branch command pointer returned
 *
 * @return  true if item is inside a solver branch
 */
//------------------------------------------------------------------------------
bool MissionTree::IsInsideSolverBranch(wxTreeItemId currId, GmatTree::ItemType &itemType,
                                       GmatTree::ItemType &solverItemType,
                                       wxTreeItemId &branchId, GmatCommand **branchCmd)
{
   #if DEBUG_FIND_ITEM_PARENT
   WriteNode(1, "MissionTree::IsInsideSolverBranch() ", false, "currId", currId);
   #endif
   
   wxTreeItemId parentId = GetItemParent(currId);
   MissionTreeItemData *parentItem;
   GmatTree::ItemType parentType;
   solverItemType = itemType;
   *branchCmd = NULL;
   
   // go through parents
   while (parentId.IsOk() && GetItemText(parentId) != "")
   {
      #if DEBUG_FIND_ITEM_PARENT > 1
      WriteNode(1, "   ", false, "parentId", parentId);
      #endif
      
      parentItem = (MissionTreeItemData *)GetItemData(parentId);
      parentType = parentItem->GetItemType();
      
      if (parentType == GmatTree::TARGET || parentType == GmatTree::OPTIMIZE)
      {
         #if DEBUG_FIND_ITEM_PARENT
         WriteNode(1, "MissionTree::IsInsideSolverBranch() returning true ",
                   false, "parentId", parentId);
         #endif
         
         solverItemType = parentType;
         branchId = parentId;
         *branchCmd = parentItem->GetCommand();
         return true;
      }
      
      parentId = GetItemParent(parentId);
   }
   
   #if DEBUG_FIND_ITEM_PARENT
   MessageInterface::ShowMessage("MissionTree::IsInsideSolverBranch() returning false\n");
   #endif
   
   return false;
}

//------------------------------------------------------------------------------
// void ChangeCommandNodeColor(GmatCommand *cmd, wxTreeItemId itemId)
//------------------------------------------------------------------------------
void MissionTree::ChangeCommandNodeColor(GmatCommand *cmd, wxTreeItemId itemId)
{
   // Do not change node color for Propagate command(LOJ: 2014.02.20)
   // @todo Need better way to change propatate command node color since
   //       some color makes hard to read item text. The better way would be
   //       changing the background color of icon. (See GMT-4330)
   #ifdef __CHANGE_NODE_COLOR__
   
   std::string cmdTypeName = cmd->GetTypeName();
   
   if (cmdTypeName == "Propagate")
   {
      Propagate *propCmd = (Propagate*)cmd;
      if (propCmd->GetOverrideSegmentColor() == true)
      {
         UnsignedInt intColor = propCmd->GetSegmentOrbitColor();
         #ifdef DEBUG_COLOR
         MessageInterface::ShowMessage
            ("ChangeCommandNodeColor() Setting propagation color to %06X\n", intColor);
         #endif
         RgbColor rgb(intColor);
         wxColour propColor(rgb.Red(), rgb.Green(), rgb.Blue());
         //SetItemTextColour(itemId, propColor);
         SetItemBackgroundColour(itemId, propColor);
         Refresh();
         Update();
      }
   }
   
   #endif
}

// For Debug
//------------------------------------------------------------------------------
// void ShowCommands(const wxString &msg = "")
//------------------------------------------------------------------------------
void MissionTree::ShowCommands(const wxString &msg)
{
   MessageInterface::ShowMessage("-------------------->%s\n", msg.WX_TO_C_STRING);
   
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();;
   
   while (cmd != NULL)
   {
      #ifdef DEBUG_MISSION_TREE_SHOW_CMD
      MessageInterface::ShowMessage
         ("----- <%p> %s '%s' (%s)\n", cmd, cmd->GetTypeName().c_str(),
          cmd->GetName().c_str(), cmd->GetSummaryName().c_str());
      #else
      MessageInterface::ShowMessage
         ("----- %s '%s' (%s)\n", cmd->GetTypeName().c_str(),
          cmd->GetName().c_str(), cmd->GetSummaryName().c_str());
      #endif
      
      if ((cmd->GetChildCommand(0)) != NULL)
         ShowSubCommands(cmd, 0);
      
      cmd = cmd->GetNext();
   }
   
   MessageInterface::ShowMessage("<--------------------\n");
}


//------------------------------------------------------------------------------
// void ShowSubCommands(GmatCommand* brCmd, Integer level)
//------------------------------------------------------------------------------
void MissionTree::ShowSubCommands(GmatCommand* brCmd, Integer level)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         
         #ifdef DEBUG_MISSION_TREE_SHOW_CMD
         MessageInterface::ShowMessage
            ("----- <%p> %s '%s' (%s)\n", nextInBranch, nextInBranch->GetTypeName().c_str(),
             nextInBranch->GetName().c_str(), nextInBranch->GetSummaryName().c_str());
         #else
         MessageInterface::ShowMessage
            ("----- %s '%s' (%s)\n", nextInBranch->GetTypeName().c_str(),
             nextInBranch->GetName().c_str(), nextInBranch->GetSummaryName().c_str());
         #endif
         
         if (nextInBranch->GetChildCommand() != NULL)
            ShowSubCommands(nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}


//------------------------------------------------------------------------------
// void WriteNode(int itemCount, const std::string &prefix,  bool appendEol,
//                const std::string &title1, wxTreeItemId itemId1, ...)
//------------------------------------------------------------------------------
void MissionTree::WriteNode(int itemCount, const std::string &prefix, bool appendEol,
                            const std::string &title1, wxTreeItemId itemId1,
                            const std::string &title2, wxTreeItemId itemId2,
                            const std::string &title3, wxTreeItemId itemId3)
{
   if (itemCount == 1)
      MessageInterface::ShowMessage
         ("%s%s = '%s'\n", prefix.c_str(),
          title1.c_str(), itemId1.IsOk() ? GetItemText(itemId1).WX_TO_C_STRING : "Unknown Node");
   else if (itemCount == 2)
      MessageInterface::ShowMessage
         ("%s%s = '%s', %s = '%s'\n", prefix.c_str(),
          title1.c_str(), itemId1.IsOk() ? GetItemText(itemId1).WX_TO_C_STRING : "Unknown Node",
          title2.c_str(), itemId2.IsOk() ? GetItemText(itemId2).WX_TO_C_STRING : "Unknown Node");
   else if (itemCount == 3)
      MessageInterface::ShowMessage
         ("%s%s = '%s', %s = '%s', %s = '%s'\n", prefix.c_str(),
          title1.c_str(), itemId1.IsOk() ? GetItemText(itemId1).WX_TO_C_STRING : "Unknown Node",
          title2.c_str(), itemId2.IsOk() ? GetItemText(itemId2).WX_TO_C_STRING : "Unknown Node",
          title3.c_str(), itemId3.IsOk() ? GetItemText(itemId3).WX_TO_C_STRING : "Unknown Node");
   if (appendEol)
      MessageInterface::ShowMessage("\n");
}


//------------------------------------------------------------------------------
// void WriteCommand(const std::string &prefix = "",
//                   const std::string &title1, GmatCommand *cmd1,
//                   const std::string &title2 = "", GmatCommand *cmd2 = NULL)
//------------------------------------------------------------------------------
/*
 * Writes command info to message window.
 */
//------------------------------------------------------------------------------
void MissionTree::WriteCommand(const std::string &prefix,
                               const std::string &title1, GmatCommand *cmd1,
                               const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage
            ("%s%sNULL<%p>'%s'\n", prefix.c_str(), title1.c_str(), cmd1,
				 cmd1->GetSummaryName().c_str());
      else
         MessageInterface::ShowMessage
            ("%s%s%s<%p>'%s'\n", prefix.c_str(), title1.c_str(),
             cmd1->GetTypeName().c_str(), cmd1, cmd1->GetSummaryName().c_str());
   }
   else
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage
            ("%s%sNULL<%p>'%s'%s%s<%p>'%s'\n", prefix.c_str(), title1.c_str(),
             cmd1, cmd1->GetSummaryName().c_str(), title2.c_str(),
				 cmd2->GetTypeName().c_str(), cmd2, cmd2->GetSummaryName().c_str());
      else if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s%s%s<%p>'%s'%sNULL<%p>'%s'\n", prefix.c_str(), title1.c_str(),
             cmd1->GetTypeName().c_str(), cmd1, cmd1->GetSummaryName().c_str(),
				 title2.c_str(), cmd2, cmd2->GetSummaryName().c_str());
      else
         MessageInterface::ShowMessage
            ("%s%s%s<%p>'%s'%s%s<%p>'%s'\n", prefix.c_str(), 
             title1.c_str(), cmd1->GetTypeName().c_str(), cmd1, cmd1->GetSummaryName().c_str(),
             title2.c_str(), cmd2->GetTypeName().c_str(), cmd2, cmd2->GetSummaryName().c_str());
   }
}


#ifdef __TEST_MISSION_TREE_ACTIONS__
//------------------------------------------------------------------------------
// void OnStartSaveActions()
//------------------------------------------------------------------------------
/**
 * Start saving actions on the tree to a text file for used in testing
 */
//------------------------------------------------------------------------------
void MissionTree::OnStartSaveActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnStartSaveActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   mSaveActions = true;
   mPlaybackActions = false;
   
   if (mActionsOutStream.is_open())
      mActionsOutStream.close();
   
   if (mResultsStream.is_open())
      mResultsStream.close();
   
   mActionsOutStream.open(mActionsOutFile.c_str());
   mResultsStream.open(mResultsFile.c_str());
}


//------------------------------------------------------------------------------
// void OnStopSaveActions()
//------------------------------------------------------------------------------
/**
 * Stops saving actions on the tree to a text file.
 */
//------------------------------------------------------------------------------
void MissionTree::OnStopSaveActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnStopSaveActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   mSaveActions = false;
   mActionsOutStream.close();
   mResultsStream.close();
}


//------------------------------------------------------------------------------
// void OnPlaybackActions()
//------------------------------------------------------------------------------
/**
 * Reads actions from a text file, parses, and initiate actions.
 */
//------------------------------------------------------------------------------
void MissionTree::OnPlaybackActions(wxCommandEvent &event)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("MissionTree::OnPlaybackActions() mSaveActions=%d\n", mSaveActions);
   #endif
   
   wxString actionsInFile =
      wxFileSelector("Choose a file to open", "", "", "txt",
                     "Text files (*.txt)|*.txt", wxOPEN);
   
   if (actionsInFile.empty())
      return;
   
   // clear command sequence and mission tree first
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage("   clearing command sequence and mission tree\n");
   #endif
   
   ClearMission();
   theGuiInterpreter->ClearCommandSeq();
   InitializeCounter();
   
   wxString playbackResultsFile;
   
   // compose playback results file
   int dot = actionsInFile.Find('.');
   if (dot == wxNOT_FOUND)
      playbackResultsFile = actionsInFile + "PbResults.txt";
   else
      playbackResultsFile = actionsInFile.Mid(0, dot) + "PbResults.txt";
   
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage
      ("   playback action file = '%s'\n   playback results file = '%s'\n",
       actionsInFile.c_str(), playbackResultsFile.c_str());
   #endif
   
   mSaveActions = false;
   mPlaybackActions = true;
   
   // close output streams
   if (mPlaybackResultsStream.is_open())
      mPlaybackResultsStream.close();
   
   // open streams
   mPlaybackResultsStream.open(playbackResultsFile.c_str());
   
   if (!mPlaybackResultsStream.is_open())
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to error opening the file '%s'\n",
          playbackResultsFile.c_str());
      return;
   }
   
   std::ifstream actionsInStream(actionsInFile.c_str());
   
   if (!actionsInStream.is_open())
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to error opening the file '%s'\n",
          actionsInFile.c_str());
      return;
   }
   
   //-----------------------------------------------------------------
   // read in lines
   //-----------------------------------------------------------------
   char buffer[1024];
   StringArray lines;
   while (!actionsInStream.eof())
   {
      actionsInStream.getline(buffer, 1023);
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage("   <%s>\n", buffer);
      #endif
      
      if (buffer[0] != '\0')
         lines.push_back(buffer);
   }
   
   actionsInStream.close();
   
   //--------------------------------------------------------------
   // Find the first item
   //--------------------------------------------------------------
   wxTreeItemId firstItemId = GetFirstVisibleItem();
   
   if (firstItemId.IsOk())
   {
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      WriteNode(1, "   ", false, "firstItemId", firstItemId);
      #endif
   }
   else
   {
      MessageInterface::ShowMessage
         ("\n*** ERROR *** Playback stopped due to first item not found\n");
      return;
   }
   
   //-----------------------------------------------------------------
   // parse lines into actions
   //-----------------------------------------------------------------
   int lineCount = lines.size();
   StringTokenizer stk;
   StringArray actions;
   
   for (int i=0; i<lineCount; i++)
   {
      stk.Set(lines[i], " ");
      actions = stk.GetAllTokens();
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage("\n");
      for (UnsignedInt i=0; i<actions.size(); i++)
         MessageInterface::ShowMessage("<%s>", actions[i].c_str());
      MessageInterface::ShowMessage("\n");
      #endif
      
      //------------------------------------------
      // Sample actions:
      // Append Optimize to Mission Sequence
      // Append If to Optimize1
      // Append Equation to If1
      // Append While to Optimize1
      // Insert Maneuver after While1
      // Delete Stop1 from Mission Sequence
      // Delete Report2 from Optimize1
      //------------------------------------------
      
      //--------------------------------------------------------------
      // Find select item
      //--------------------------------------------------------------
      wxString selCmd = actions[3].c_str();
      wxString cmd = actions[1].c_str();
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      MessageInterface::ShowMessage
         ("   selCmd = '%s', cmd = '%s'\n", selCmd.c_str(), cmd.c_str());
      #endif
      
      bool itemFound = false;
      wxTreeItemId itemId;
      
      //--------------------------------------------------------------
      // Select item
      //--------------------------------------------------------------
      if (actions[0] == "Delete")
      {
         itemId = FindChild(firstItemId, cmd);
      }
      else
      {
         if (selCmd == "Mission")
            itemId = firstItemId;
         else
            itemId = FindChild(firstItemId, selCmd);
      }
      
      if (itemId.IsOk() && GetItemText(itemId) != "")
      {
         SelectItem(itemId);
         itemFound = true;
      }
      
      #ifdef DEBUG_MISSION_TREE_ACTIONS
      wxTreeItemId selId = GetSelection();
      WriteNode(1, "   ", false, "GetSelection()", selId);
      #endif
      
      
      //--------------------------------------------------------------
      // Do actions
      //--------------------------------------------------------------
      if (itemFound)
      {
         if (actions[0] == "Append")
         {
            Append(cmd);
         }
         else if (actions[0] == "Insert")
         {
            if (actions[2] == "before")
               InsertBefore(cmd);
            else
               InsertAfter(cmd);
         }
         else if (actions[0] == "Delete")
         {
            DeleteCommand(cmd);
         }
         else
         {
            MessageInterface::ShowMessage
               ("\n*** ERROR *** Playback stopped due to unknown action \"%s\"\n",
                actions[0].c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("\n*** ERROR *** Playback stopped due to '%s' not found\n",
             selCmd.c_str());
         return;
      }
   }
   
   // close playback results stream
   mPlaybackResultsStream.close();
   
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   ShowCommands("After Playback");
   #endif
}


//------------------------------------------------------------------------------
// void WriteActions(const wxString &str)
//------------------------------------------------------------------------------
void MissionTree::WriteActions(const wxString &str)
{
   #ifdef DEBUG_MISSION_TREE_ACTIONS
   MessageInterface::ShowMessage("\n..........%s", str.c_str());
   #endif
   
   // write actions
   mActionsOutStream << str;
}


//------------------------------------------------------------------------------
// void WriteResults()
//------------------------------------------------------------------------------
void MissionTree::WriteResults()
{
   // write results
   GmatCommand *cmd = theGuiInterpreter->GetFirstCommand();
   
   if (mSaveActions)
      mResultsStream << GmatCommandUtil::GetCommandSeqString(cmd, false);
   else if (mPlaybackActions)
      mPlaybackResultsStream << GmatCommandUtil::GetCommandSeqString(cmd, false);
}
#endif

