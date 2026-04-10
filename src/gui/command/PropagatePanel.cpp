//$Id$
//------------------------------------------------------------------------------
//                              PropagatePanel
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
// Author: Waka Waktola
// Created: 2003/08/29
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropgateCommandPanel
/**
 * This class contains the Propagate Command Setup window.
 */
//------------------------------------------------------------------------------

#include <wx/variant.h> 
#include "PropagatePanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "SpaceObjectSelectDialog.hpp"
#include "PropagatorSelectDialog.hpp"
#include "GmatColorPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatAppData.hpp"
#include "MissionTree.hpp"
#include "StringUtil.hpp"               // for SeparateBy()
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATE_PANEL
//#define DEBUG_PROPAGATE_PANEL_LOAD
//#define DEBUG_PROPAGATE_PANEL_SAVE
//#define DEBUG_PROPAGATE_PANEL_STOPCOND
//#define DEBUG_RENAME
//#define DEBUG_COLOR

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatePanel, GmatPanel)
   EVT_GRID_CELL_LEFT_CLICK(PropagatePanel::OnCellLeftClick)
   EVT_GRID_CELL_RIGHT_CLICK(PropagatePanel::OnCellRightClick)
   EVT_GRID_CELL_CHANGE(PropagatePanel::OnCellValueChange)
   EVT_CHECKBOX(ID_CHECKBOX, PropagatePanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, PropagatePanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, PropagatePanel::OnTextChange)
   #if wxCHECK_VERSION(3, 0, 0)
   EVT_GRID_TABBING(PropagatePanel::OnGridTabbing)
   #endif
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagatePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagatePanel::PropagatePanel(wxWindow *parent, GmatCommand *cmd, wxTreeItemId nodeId)
   : GmatPanel(parent)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage ("PropagatePanel::PropagatePanel() entered\n");
   #endif
   
   thePropCmd = (Propagate *)cmd;
   theNodeId = nodeId;
   
   InitializeData();   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   EnableUpdate(false);
   
   // Default values
   mPropModeChanged = false;
   mPropDirChanged = false;
   mPropSatChanged = false;
   mStopCondChanged = false;
   mStopTolChanged = false;
   isPropGridDisabled = false;
   canClose = true;
   
   // Listen for Propagator or Spacecraft name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~PropagatePanel()
//------------------------------------------------------------------------------
PropagatePanel::~PropagatePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool PropagatePanel::PrepareObjectNameChange()
{
   // Save GUI data
   wxCommandEvent event;
   OnApply(event);
   
   return GmatPanel::PrepareObjectNameChange();
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(UnsignedInt type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
/*
 * Reflects resource name change to this panel.
 * By the time this method is called, the base code already changed reference
 * object name, so all we need to do is re-load the data.
 */
//------------------------------------------------------------------------------
void PropagatePanel::ObjectNameChanged(UnsignedInt type,
                                       const wxString &oldName,
                                       const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("PropagatePanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::PROP_SETUP && type != Gmat::SPACECRAFT &&
       type != Gmat::PARAMETER)
      return;
   
   // Initialize GUI data and re-load from base
   InitializeData();
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagatePanel::Create()
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::Create() entered\n");
   #endif
   
   Integer bsize = 2; // bordersize
   
   // Propagate Mode
   wxStaticText *synchStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Propagate Mode:  "), 
         wxDefaultPosition, wxDefaultSize, 0);
   
   StringArray propModes = thePropCmd->GetStringArrayParameter
      (thePropCmd->GetParameterID("AvailablePropModes"));
   
   mPropModeCount = propModes.size();
   
   wxString *propModeList = new wxString[mPropModeCount];
   for (Integer i=0; i<mPropModeCount; i++)
   {
      propModeList[i] = propModes[i].c_str();
   }
   
   if (strcmp(propModeList[0].c_str(), "") == 0)
      propModeList[0] = "None";

   mPropModeComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxString(propModeList[0].c_str()), 
         wxDefaultPosition, wxSize(150,-1), mPropModeCount, propModeList,
         wxCB_DROPDOWN|wxCB_READONLY);
   
   mPropModeComboBox->Enable(true);
   
   // Backwards propagate
   backPropCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Backwards Propagation"),
                      wxDefaultPosition, wxDefaultSize, 0);

   // STM and A-matrix propagate
   stmPropCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Propagate STM"),
                      wxDefaultPosition, wxDefaultSize, 0);
   aMatrixCalcCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Compute A-Matrix"),
                      wxDefaultPosition, wxDefaultSize, 0);
   
   // Propagator Grid
   propGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(750,100), 
         wxWANTS_CHARS);
   
   propGrid->CreateGrid(MAX_PROP_ROW, MAX_PROP_COL, wxGrid::wxGridSelectCells);
   wxColour gridColor = wxTheColourDatabase->Find("DIM GREY");
   propGrid->SetGridLineColour(gridColor);
   
   propGrid->SetColLabelValue(PROP_NAME_SEL_COL, _T(""));
   propGrid->SetColLabelValue(PROP_NAME_COL, _T("Propagator"));
   propGrid->SetColLabelValue(PROP_SOS_SEL_COL, _T(""));
   propGrid->SetColLabelValue(PROP_SOS_COL, _T("Spacecraft List"));
   propGrid->SetColSize(PROP_NAME_SEL_COL, 25);
   propGrid->SetColSize(PROP_NAME_COL, 340);
   propGrid->SetColSize(PROP_SOS_SEL_COL, 25);
   propGrid->SetColSize(PROP_SOS_COL, 340);
   #if wxCHECK_VERSION(3, 0, 0)
   propGrid->SetTabBehaviour(wxGrid::Tab_Wrap);
   #endif
   
   propGrid->SetMargins(0, 0);
   propGrid->SetRowLabelSize(0);
   // Grubb 2014-12-15: Commented out, scroll area will be calculated automatically
   // based on size
 //  propGrid->SetScrollbars(5, 8, 15, 15);
   
   for (Integer i = 0; i < MAX_PROP_ROW; i++)
   {
      propGrid->SetReadOnly(i, PROP_NAME_SEL_COL, true);
      propGrid->SetReadOnly(i, PROP_SOS_SEL_COL, true);
      propGrid->SetCellValue(i, PROP_NAME_SEL_COL, "  ... ");
      propGrid->SetCellValue(i, PROP_SOS_SEL_COL, "  ... ");
      propGrid->SetCellBackgroundColour(i, PROP_NAME_SEL_COL, *wxLIGHT_GREY);
      propGrid->SetCellBackgroundColour(i, PROP_SOS_SEL_COL, *wxLIGHT_GREY);
   }
   
   // Stop tolerance
   wxStaticText *stopTolStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Stop Tolerance: "), 
                       wxDefaultPosition, wxSize(-1, -1), 0);
   mStopTolTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(150,-1), 0);
   wxBoxSizer *stopTolSizer = new wxBoxSizer(wxHORIZONTAL);
   
   stopTolSizer->Add(stopTolStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   stopTolSizer->Add(mStopTolTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
 
   // Stopping Condition Grid
   stopCondGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(750,100), 
         wxWANTS_CHARS);
   
   stopCondGrid->CreateGrid(MAX_STOPCOND_ROW, MAX_STOPCOND_COL, wxGrid::wxGridSelectCells);
   stopCondGrid->SetGridLineColour(gridColor);
   
   stopCondGrid->SetColLabelValue(STOPCOND_LEFT_SEL_COL, _T(""));
   stopCondGrid->SetColLabelValue(STOPCOND_LEFT_COL, _T("Parameter"));
   stopCondGrid->SetColLabelValue(STOPCOND_RELOPER_COL, _T(""));
   stopCondGrid->SetColLabelValue(STOPCOND_RIGHT_SEL_COL, _T(""));
   stopCondGrid->SetColLabelValue(STOPCOND_RIGHT_COL, _T("Condition"));
   
   stopCondGrid->SetColSize(STOPCOND_LEFT_SEL_COL, 25);
   stopCondGrid->SetColSize(STOPCOND_LEFT_COL, 325);
   stopCondGrid->SetColSize(STOPCOND_RELOPER_COL, 30);
   stopCondGrid->SetColSize(STOPCOND_RIGHT_SEL_COL, 25);
   stopCondGrid->SetColSize(STOPCOND_RIGHT_COL, 325);
   
   for (Integer i = 0; i < MAX_STOPCOND_ROW; i++)
   {
      stopCondGrid->SetReadOnly(i, STOPCOND_LEFT_SEL_COL, true);
      stopCondGrid->SetReadOnly(i, STOPCOND_RELOPER_COL, true);
      stopCondGrid->SetReadOnly(i, STOPCOND_RIGHT_SEL_COL, true);
      stopCondGrid->SetReadOnly(i, STOPCOND_RIGHT_COL, true);
      stopCondGrid->SetCellValue(i, STOPCOND_LEFT_SEL_COL, "  ...");
      stopCondGrid->SetCellValue(i, STOPCOND_RIGHT_SEL_COL, "  ...");
      stopCondGrid->SetCellBackgroundColour(i, STOPCOND_LEFT_SEL_COL, *wxLIGHT_GREY);
      stopCondGrid->SetCellBackgroundColour(i, STOPCOND_RIGHT_SEL_COL, *wxLIGHT_GREY);
   }
   
   stopCondGrid->SetMargins(0, 0);
   stopCondGrid->SetRowLabelSize(0);
   #if wxCHECK_VERSION(3, 0, 0)
   stopCondGrid->SetTabBehaviour(wxGrid::Tab_Wrap);
   #endif
   
   // Grubb 2014-12-15: Commented out, scroll area will be calculated automatically
   // based on size
//   stopCondGrid->SetScrollbars(5, 8, 15, 15);
   
   wxFlexGridSizer *propModeSizer = new wxFlexGridSizer(6, 0, 0);
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   GmatStaticBoxSizer *propSizer = 
      new GmatStaticBoxSizer(wxVERTICAL, this, "Propagators and Spacecraft");
   
   //Adding objects to sizers
   propModeSizer->Add(synchStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(mPropModeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(200, 20, wxALIGN_CENTRE|wxALL, bsize);
   propModeSizer->Add(backPropCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(stmPropCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(aMatrixCalcCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   propSizer->Add(propModeSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   propSizer->Add(propGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // Stopping conditions
   GmatStaticBoxSizer *stopSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Stopping Conditions");
   
   stopSizer->Add(stopTolSizer, 0, wxALIGN_LEFT|wxALL, 0);
   stopSizer->Add(stopCondGrid, 0, wxALIGN_CENTER|wxALL, 0);
   
   // Get first SpaceObject color
   std::string objName = thePropCmd->GetFirstSpaceObjectName();
   bool overrideColor = thePropCmd->GetOverrideSegmentColor();
   UnsignedInt segmentColor = thePropCmd->GetSegmentOrbitColor();
   
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("PropagatePanel Loading... overrideColor = %d, segmentColor = %06X\n", overrideColor, segmentColor);
   #endif
   
   // Create color panel
   mColorPanel = new GmatColorPanel(this, this, NULL, true, overrideColor, true, true, objName, segmentColor);
   
   pageSizer->Add(propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 0);
   pageSizer->Add(stopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, 0);
   pageSizer->Add(mColorPanel, 0, wxGROW|wxALIGN_CENTER|wxALL, 0);
   
   theMiddleSizer->Add(pageSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
void PropagatePanel::InitializeData()
{
   mPropModeCount = 1;
   mPropCount     = 0;
   mStopCondCount = 0;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName = "";
      mTempProp[i].soNames = "";
      mTempProp[i].soNameList.Clear();
   }
   
   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      mTempStopCond[i].isChanged = false;
      mTempStopCond[i].name = "";
      mTempStopCond[i].desc = "";
      mTempStopCond[i].varName = "";
      mTempStopCond[i].relOpStr = "";
      mTempStopCond[i].goalStr = "";
      mTempStopCond[i].stopCondPtr = NULL;
   }
}


//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayPropagator()
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayPropagator() entered\n");
   #endif
   
   wxString name;
   for (Integer i=0; i<mPropCount; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SOS_COL, mTempProp[i].soNames);
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void DisplayStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayStopCondition()
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayStopCondition() \n");
   #endif
   
   //----- for stopCondGrid - show all
   for (Integer i=0; i<mStopCondCount; i++)
   {
      stopCondGrid->
         SetCellValue(i, STOPCOND_LEFT_COL, mTempStopCond[i].varName);

      if (mTempStopCond[i].varName.Contains(".Periapsis") ||
           mTempStopCond[i].varName.Contains(".Apoapsis")  ||
           strcmp(mTempStopCond[i].varName, "") == 0)
      {
              stopCondGrid->SetCellValue(i, STOPCOND_RELOPER_COL, "");
              stopCondGrid->SetCellValue(i, STOPCOND_RIGHT_COL, "");
              stopCondGrid->SetReadOnly(i, STOPCOND_RIGHT_COL, true);
      }
      else
      {
         stopCondGrid->SetCellValue(i, STOPCOND_RELOPER_COL, "   = ");
         stopCondGrid->
            SetCellValue(i, STOPCOND_RIGHT_COL, mTempStopCond[i].goalStr);
         stopCondGrid->SetReadOnly(i, STOPCOND_RIGHT_COL, false);
      }   
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::UpdateStopCondition(Integer stopRow)
{
   #ifdef DEBUG_PROPAGATE_PANEL_STOPCOND
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() entered, stopRow = %d\n", stopRow);
   #endif
   
   wxString oldStopName = mTempStopCond[stopRow].name;
   mTempStopCond[stopRow].name = "StopOn" + 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_LEFT_COL);
   mTempStopCond[stopRow].varName = 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_LEFT_COL);
   mTempStopCond[stopRow].relOpStr = 
      stopCondGrid->GetCellValue(stopRow, STOPCOND_RELOPER_COL);
   
   // if Apoapsis or Periapsis, disable goal
   if (mTempStopCond[stopRow].varName.Contains(".Periapsis") ||
        mTempStopCond[stopRow].varName.Contains(".Apoapsis"))
   {
      mTempStopCond[stopRow].goalStr = "0.0";
   }
   else
   {
      mTempStopCond[stopRow].goalStr = 
         stopCondGrid->GetCellValue(stopRow, STOPCOND_RIGHT_COL);
   }

   std::string nameStr = mTempStopCond[stopRow].name.WX_TO_STD_STRING;
   std::string stopStr = mTempStopCond[stopRow].varName.WX_TO_STD_STRING;
   std::string goalStr = mTempStopCond[stopRow].goalStr.WX_TO_STD_STRING;
   
   #ifdef DEBUG_PROPAGATE_PANEL_STOPCOND
   MessageInterface::ShowMessage
      ("   old name = '%s'\n   new name = '%s'\n   stop str = '%s'\n   goal str = '%s'\n",
       oldStopName.c_str(), nameStr.c_str(), stopStr.c_str(), goalStr.c_str());
   #endif
   
   wxString str = FormatStopCondDesc(mTempStopCond[stopRow].varName,
                                     mTempStopCond[stopRow].relOpStr,
                                     mTempStopCond[stopRow].goalStr);
   mTempStopCond[stopRow].desc = str;
   
   #ifdef DEBUG_PROPAGATE_PANEL_STOPCOND
   MessageInterface::ShowMessage("   str='%s'\n", str.c_str());
   #endif
   
   
   //-----------------------------------------------------------------
   // create StopCondition if new StopCondition
   //-----------------------------------------------------------------
   if (oldStopName.IsSameAs(""))
   {
      #ifdef DEBUG_PROPAGATE_PANEL_STOPCOND
      MessageInterface::ShowMessage("   Creating new stop condition\n");
      #endif
      StopCondition *stopCond = 
         (StopCondition*)theGuiInterpreter-> CreateStopCondition
         ("StopCondition", stopStr);
      mTempStopCond[stopRow].stopCondPtr = stopCond;
      
      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagatePanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", mTempStopCond[stopRow].name.WX_TO_C_STRING);
      }
   }
   
   mTempStopCond[stopRow].isChanged = true;
   
   mStopCondChanged = true;
   EnableUpdate(true);
   
   #ifdef DEBUG_PROPAGATE_PANEL_STOPCOND
   MessageInterface::ShowMessage("PropagatePanel::UpdateStopCondition() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void GetNewPropagatorName(Integer row, Integer col)
//------------------------------------------------------------------------------
void PropagatePanel::GetNewPropagatorName(Integer row, Integer col)
{
   PropagatorSelectDialog propDlg(this, "");
   propDlg.ShowModal();
   
   if (propDlg.HasSelectionChanged())
   {
      wxString newPropName = propDlg.GetPropagatorName();
      #ifdef DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::GetNewPropagatorName() newPropName = %s\n",
          newPropName.c_str());
      #endif
      
      propGrid->SetCellValue(row, col, newPropName);
      mPropSatChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void GetNewSpaceObjectList(Integer row, Integer col)
//------------------------------------------------------------------------------
void PropagatePanel::GetNewSpaceObjectList(Integer row, Integer col)
{
   wxArrayString soExcList;
   Integer soCount = 0;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      soCount = mTempProp[i].soNameList.Count();
      
      for (Integer j=0; j<soCount; j++)
         soExcList.Add(mTempProp[i].soNameList[j]);
   }
   
   SpaceObjectSelectDialog soDlg(this, mTempProp[row].soNameList, soExcList);
   soDlg.ShowModal();
   
   if (soDlg.HasSelectionChanged())
   {
      wxArrayString &newNames = soDlg.GetSpaceObjectNames();
      mTempProp[row].isChanged = true;
      mTempProp[row].soNames = "";
      soCount = newNames.GetCount();
      
      #ifdef DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::GetNewSpaceObjectList() new soCount=%d\n", soCount);
      #endif
      
      mTempProp[row].soNameList.Clear();
      for (Integer j=0; j<soCount; j++)
      {
         mTempProp[row].soNameList.Add(newNames[j]);
         
         #ifdef DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::GetNewSpaceObjectList() soNameList[%d]='%s'\n",
             j, mTempProp[row].soNameList[j].c_str());
         #endif
      }
      
      if (soCount > 0)
      {
         for(Integer j=0; j < soCount-1; j++)
         {
            mTempProp[row].soNames += newNames[j].c_str();
            mTempProp[row].soNames += ", ";
         }
         
         mTempProp[row].soNames += newNames[soCount-1].c_str();
      }
      
      mTempProp[row].soCount = soCount;
      
      propGrid->SetCellValue(row, col, mTempProp[row].soNames);
      
      mPropSatChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void GetNewStopCondLeftValue(Integer row, Integer col)
//------------------------------------------------------------------------------
void PropagatePanel::GetNewStopCondLeftValue(Integer row, Integer col)
{
   // show dialog to select parameter
   // we cannot allow Variables
   ParameterSelectDialog paramDlg(this, mObjectTypeList, GuiItemManager::SHOW_PLOTTABLE,
                                  0, false, false, true, true, true, true, "Spacecraft",
                                  true, false, false, true);
   wxArrayString paramNames;
   paramNames.Add(stopCondGrid->GetCellValue(row,STOPCOND_LEFT_COL));
   paramDlg.SetParamNameArray(paramNames);
   paramDlg.ShowModal();
   
   if (paramDlg.HasSelectionChanged())
   {
      wxString newParamName = paramDlg.GetParamName();
      stopCondGrid->SetCellValue(row,STOPCOND_LEFT_COL,newParamName);
      
      // if Apoapsis or Periapsis, disable goal
      if (newParamName.Contains(".Periapsis") ||
          newParamName.Contains(".Apoapsis"))
      {
         stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
         stopCondGrid->SetCellValue(row, STOPCOND_RIGHT_COL, "");
         stopCondGrid->SetReadOnly(row, STOPCOND_RIGHT_COL, true);
      }
      else
      {
         stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "   = ");
         if (stopCondGrid->GetCellValue(row, STOPCOND_RIGHT_COL) == "")
            stopCondGrid->SetCellValue(row, STOPCOND_RIGHT_COL, "0.0");
         stopCondGrid->SetReadOnly(row, STOPCOND_RIGHT_COL, false);
      }
      
      mStopCondChanged = true;
      UpdateStopCondition(row);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void GetNewStopCondRightValue(Integer row, Integer col)
//------------------------------------------------------------------------------
void PropagatePanel::GetNewStopCondRightValue(Integer row, Integer col)
{
   wxString paramName = stopCondGrid->GetCellValue(row, STOPCOND_LEFT_COL);
   
   // do nothing if parameter is empty
   if (paramName == "")
      return;
   
   // do nothing if parameter contains Apoapsis or Periapsis
   if (paramName.Contains(".Periapsis") ||
       paramName.Contains(".Apoapsis"))
      return;
   
   // show dialog to select parameter
   // we can allow Variables
   ParameterSelectDialog paramDlg(this, mObjectTypeList, GuiItemManager::SHOW_PLOTTABLE,
                                  0, false, false, true, true, true, true, "Spacecraft",
                                  true, false, false, true);
   paramDlg.ShowModal();
   
   if (paramDlg.HasSelectionChanged())
   {
      wxString newParamName = paramDlg.GetParamName();
      stopCondGrid->SetCellValue(row, STOPCOND_RIGHT_COL, newParamName);
      mStopCondChanged = true;
      UpdateStopCondition(row);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// wxString FormatStopCondDesc(const wxString &varName, 
//                             const wxString &relOpStr,
//                             const wxString &goalStr)
//------------------------------------------------------------------------------
wxString PropagatePanel::FormatStopCondDesc(const wxString &varName,
                                            const wxString &relOpStr,
                                            const wxString &goalStr)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::FormatStopCondDesc() entered\n");
   #endif

   wxString goalTempStr = goalStr;
   wxString desc;
   wxString opStr = relOpStr;
   
   if (varName.Contains("Apoapsis") || varName.Contains("Periapsis"))
   {
      opStr = "";
      goalTempStr = "";
   }
   
   desc = varName + " " + opStr + " " + goalTempStr;
    
   return desc;
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnTextChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mStopTolTextCtrl)
   {
      mStopTolChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles CheckBox event.
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnCheckBoxChange(wxCommandEvent& event)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCheckBoxChange() entered\n");
   #endif

   if (event.GetEventObject() == backPropCheckBox)
   {
      mPropDirChanged = true;
      EnableUpdate(true);
   }

   if (event.GetEventObject() == stmPropCheckBox)
   {
      mPropStmChanged = true;
      EnableUpdate(true);
   }

   if (event.GetEventObject() == aMatrixCalcCheckBox)
   {
      mCalcAmatrixChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnComboBoxChange(wxCommandEvent& event)
{
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnComboBoxChange() entered\n");
   #endif
   
   if (event.GetEventObject() == mPropModeComboBox)
   {
      mPropModeChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellLeftClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellLeftClick() isPropGridDisabled=%d, row = %d, col = %d\n",
       isPropGridDisabled, row, col);
   #endif
   
   // Propagate grid
   if (event.GetEventObject() == propGrid)
   {
      if (isPropGridDisabled)
         return;
      
      propGrid->SelectBlock(row, col, row, col);
      propGrid->SetGridCursor(row, col);
      
      if (col == PROP_NAME_SEL_COL)
         GetNewPropagatorName(row, col + 1);
      else if (col == PROP_SOS_SEL_COL)
         GetNewSpaceObjectList(row, col + 1);
   }
   // Stopping Condition grid
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectBlock(row, col, row, col);
      stopCondGrid->SetGridCursor(row, col);
      
      if (col == STOPCOND_LEFT_SEL_COL)
         GetNewStopCondLeftValue(row, col + 1);
      else if (col == STOPCOND_RIGHT_SEL_COL)
         GetNewStopCondRightValue(row, col + 1);
   }
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellRightClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellRightClick() row = %d, col = %d\n", row, col);
   #endif
   
   // Propagate grid
   if (event.GetEventObject() == propGrid)
   {
      if (isPropGridDisabled)
         return;
      
      propGrid->SelectBlock(row, col, row, col);
      propGrid->SetGridCursor(row, col);
      
      if (col == PROP_NAME_COL)
         GetNewPropagatorName(row, col);
      else if (col == PROP_SOS_COL)
         GetNewSpaceObjectList(row, col);
   } 
   
   // Stopping Condition grid
   else if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectBlock(row, col, row, col);
      stopCondGrid->SetGridCursor(row, col);
      
      if (col == STOPCOND_LEFT_COL)
         GetNewStopCondLeftValue(row, col);
      else if (col == STOPCOND_RIGHT_COL)
         GetNewStopCondRightValue(row, col);
   }
}


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellValueChange(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   #ifdef DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::OnCellValueChange() row=%d, col=%d, ", row, col);
   #endif

   // Manage user typing directly into the grids
   if (event.GetEventObject() == propGrid)
   {
      wxString propName = propGrid->GetCellValue(row, PROP_NAME_COL);
      wxString satNames = propGrid->GetCellValue(row, PROP_SOS_COL);
      
      #ifdef DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("propName=<%s>, satList=<%s>\n", propName.c_str(), satNames.c_str());
      #endif
      
      if ((col == PROP_NAME_COL) || (col == PROP_SOS_COL))
      {
         mPropSatChanged = true;
         EnableUpdate(true);
      }
   }
   
   if (event.GetEventObject() == stopCondGrid)
   {
      wxString paramName = stopCondGrid->GetCellValue(row, STOPCOND_LEFT_COL);
      wxString condValue = stopCondGrid->GetCellValue(row, STOPCOND_RIGHT_COL);
      
      #ifdef DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("paramName=<%s>, condValue=<%s>\n", paramName.c_str(), condValue.c_str());
      #endif
      
      if (col == STOPCOND_LEFT_COL)
      {
         // if Apoapsis or Periapsis, disable goal
         if (paramName.Contains(".Periapsis") || paramName.Contains(".Apoapsis"))
         {
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
            stopCondGrid->SetCellValue(row, STOPCOND_RIGHT_COL, "");
            stopCondGrid->SetReadOnly(row, STOPCOND_RIGHT_COL, true);
         }
         else if (paramName == "")
         {
            // do not show = sign if codition is empty
            if (condValue == "")
               stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
         }
         else
         {
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "   = ");
            if (stopCondGrid->GetCellValue(row, STOPCOND_RIGHT_COL) == "")
               stopCondGrid->SetCellValue(row, STOPCOND_RIGHT_COL, "0.0");
            stopCondGrid->SetReadOnly(row, STOPCOND_RIGHT_COL, false);
         }
      }
      else if (col == STOPCOND_RIGHT_COL)
      {
         // do not show = sign if pameter and codition is empty
         if (paramName == "" && condValue == "")
            stopCondGrid->SetCellValue(row, STOPCOND_RELOPER_COL, "");
      }
      
      mStopCondChanged = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagatePanel::LoadData()
{
   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage("PropagatePanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = thePropCmd;
   
   //----------------------------------
   // propagation mode
   //----------------------------------
   std::string mode =
      thePropCmd->GetStringParameter(thePropCmd->GetParameterID("PropagateMode"));
   mPropModeComboBox->SetStringSelection(mode.c_str());
   
   //----------------------------------
   // Backwards propagation
   //----------------------------------
   Integer PropDirectionId = thePropCmd->GetParameterID("PropForward");
   bool backProp = !thePropCmd->GetBooleanParameter(PropDirectionId);
   backPropCheckBox->SetValue(backProp);
   
   //----------------------------------
   // STM propagation and A-Matrix Calculation
   //----------------------------------
   Integer PropSTMId = thePropCmd->GetParameterID("AllSTMs");
   bool stmProp = thePropCmd->GetBooleanParameter(PropSTMId);
   stmPropCheckBox->SetValue(stmProp);

   Integer calcAMatrixId = thePropCmd->GetParameterID("AllAMatrices");
   bool amatCalc = thePropCmd->GetBooleanParameter(calcAMatrixId);
   aMatrixCalcCheckBox->SetValue(amatCalc);

   //----------------------------------
   // propagator
   //----------------------------------
   Integer propId = thePropCmd->GetParameterID("Propagator");
   
   // Get the list of propagators (aka the PropSetups)
   StringArray propNames = thePropCmd->GetStringArrayParameter(propId);
   mPropCount = propNames.size();
   
   StringArray soList;
   wxString name;
   
   Integer scId = thePropCmd->GetParameterID("Spacecraft");

   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mPropCount=%d\n", mPropCount);
   #endif
   
   Integer soCount = 0;
   
   if (mPropCount > MAX_PROP_ROW)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "There are more propagators (%d) than GMAT can manage "
          "to show (%d).\nSo the propagator grid is set to uneditable.\n",
          mPropCount, MAX_PROP_ROW);
      mPropCount = MAX_PROP_ROW;
      propGrid->EnableEditing(false);
      isPropGridDisabled = true;
   }
   
   for (Integer i=0; i<mPropCount; i++)
   {
      mTempProp[i].propName = wxString(propNames[i].c_str());
      
      // Get the list of spacecraft and formations
      soList = thePropCmd->GetStringArrayParameter(scId, i);
      soCount = soList.size();
      
      #ifdef DEBUG_PROPAGATE_PANEL_LOAD
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() propName=%s, soCount=%d\n",
          propNames[i].c_str(), soCount);
      #endif
      
      Integer actualSoCount = 0;
      for (Integer j=0; j<soCount; j++)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_LOAD
         MessageInterface::ShowMessage
            ("PropagatePanel::LoadData() soNameList[%d]='%s'\n",
             j, soList[j].c_str());
         #endif
         
         // verify space object actually exist
         if (theGuiInterpreter->GetConfiguredObject(soList[j]))
         {
            actualSoCount++;
            mTempProp[i].soNameList.Add(soList[j].c_str());
         }
         else
         {
            // No warning for STM or AMatrix
            if ((soList[j] != "STM") && (soList[j] != "AMatrix"))
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "The SpaceObject named '%s' was not created,"
                   " so removed from the display list\n", soList[j].c_str());
         }
      }
      
      #ifdef DEBUG_PROPAGATE_PANEL_LOAD
      MessageInterface::ShowMessage("   actualSoCount=%d\n", actualSoCount);
      MessageInterface::ShowMessage("      Names:\n");
      for (UnsignedInt m = 0; m < mTempProp[i].soNameList.size(); ++m)
         MessageInterface::ShowMessage("         %s\n",
               mTempProp[i].soNameList[m].c_str());
      #endif
      
      soCount = actualSoCount;
      mTempProp[i].soCount = actualSoCount;
      
      if (soCount > 0)
      {
         for (Integer j=0; j<soCount-1; j++)
         {
            mTempProp[i].soNames += mTempProp[i].soNameList[j].c_str();
            mTempProp[i].soNames += ", ";
         }
         
         mTempProp[i].soNames += mTempProp[i].soNameList[soCount-1].c_str();
      }
      
   } // for (Integer i=0; i<mPropCount; i++)
   
   backPropCheckBox->SetValue(backProp);     // Why repeated here?
   
   //----------------------------------
   // stopping conditions
   //----------------------------------
   
   Real stopTol = thePropCmd->GetRealParameter("StopTolerance");
   mStopTolTextCtrl->SetValue(theGuiManager->ToWxString(stopTol));
   
   ObjectArray &stopArray =
      thePropCmd->GetRefObjectArray(Gmat::STOP_CONDITION);
   mStopCondCount = stopArray.size();
   
   #ifdef DEBUG_PROPAGATE_PANEL_LOAD
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mStopCondCount=%d\n", mStopCondCount);
   #endif
   
   StopCondition  *stopCond;
   for (Integer i=0; i<mStopCondCount; i++)
   {
      stopCond = (StopCondition *)stopArray[i]; 
      
      #ifdef DEBUG_PROPAGATE_PANEL_LOAD
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() stopCond=%p, stopArray[%d]=%s\n",
          stopCond, i, stopCond->GetName().c_str());
      #endif
      
      // StopCondition created from the script might not have been 
      // configured (unnamed)
      if (stopCond != NULL)
      {
         mTempStopCond[i].stopCondPtr = stopCond;
         mTempStopCond[i].name = wxString(stopCond->GetName().c_str());
         mTempStopCond[i].varName = 
            wxString(stopCond->GetStringParameter("StopVar").c_str());
         mTempStopCond[i].goalStr = 
            stopCond->GetStringParameter("Goal").c_str();
         wxString str = FormatStopCondDesc(mTempStopCond[i].varName,
                                           mTempStopCond[i].relOpStr,
                                           mTempStopCond[i].goalStr);
         mTempStopCond[i].desc = str;
      }
   }
   
   DisplayPropagator();
   DisplayStopCondition();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagatePanel::SaveData()
{
   #ifdef DEBUG_PROPAGATE_PANEL_SAVE
   MessageInterface::ShowMessage("PropagatePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   Integer blankProps = 0;
   wxArrayString emptyProps, emptySos;
   wxString propName, soNames;
   
   //-----------------------------------------------------------------
   // check valid propagators and space objects
   //-----------------------------------------------------------------
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      propName = propGrid->GetCellValue(i, PROP_NAME_COL);
      soNames = propGrid->GetCellValue(i, PROP_SOS_COL);
      
      if (propName == "" && soNames == "")
         ++blankProps;
      else if (propName != "" && soNames == "")
         emptySos.Add(propName);
      else if (propName == "" && soNames != "")
         emptyProps.Add(soNames);
   }

   // Validate that the propagator and spacecraft lists are valid data entries
   std::string validationMessage;
   StringArray validationErrors;

   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      std::string propagator = propGrid->GetCellValue(i, PROP_NAME_COL).WX_TO_STD_STRING;
      std::string satNames = propGrid->GetCellValue(i, PROP_SOS_COL).WX_TO_STD_STRING;
      StringArray sats;

      if (propagator != "")
      {
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
            MessageInterface::ShowMessage("Validating propagator %s\n", propagator.c_str());
         #endif

         GmatBase *theThing = theGuiInterpreter->GetConfiguredObject(propagator);

         if (theThing == NULL)
         {
            validationMessage = "The propagator " + propagator + " cannot be found\n";
            validationErrors.push_back(validationMessage);
         }
         else if (theThing->IsOfType("PropSetup") == false)
         {
            validationMessage = "The object " + propagator + " is not a Propagator\n";
            validationErrors.push_back(validationMessage);
         }

         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
            MessageInterface::ShowMessage("Validating SpaceObjects %s\n", satNames.c_str());
         #endif

         // Split up the satellite string and validate its pieces
         sats = GmatStringUtil::SeparateByComma(satNames);
         for (UnsignedInt i = 0; i < sats.size(); ++i)
         {
            theThing = theGuiInterpreter->GetConfiguredObject(sats[i]);

            if (theThing == NULL)
            {
               validationMessage = "The SpaceObject " + sats[i] + " cannot be found\n";
               validationErrors.push_back(validationMessage);
            }
            else 
            {
               if (theThing->IsOfType("SpaceObject") == false)
               {
                  validationMessage = "The object " + sats[i] + " is not a Spacecraft or Formation\n";
                  validationErrors.push_back(validationMessage);
               }
               else
               {
                  bool stmOrAmatrix = stmPropCheckBox->IsChecked() || aMatrixCalcCheckBox->IsChecked();
                  if (theThing->IsOfType("Formation") && stmOrAmatrix)
                  {
                     validationMessage = "The object " + sats[i] + " is a Formation; Formations cannot "
                        "be propagated when propagating the State Transition Matrix or calculating the "
                        "A-matrix\n";
                     validationErrors.push_back(validationMessage);
                  }
               }
            }
         }
      }
   }

   if (validationErrors.size() > 0)
   {
      validationMessage = "Propagate command configuration error:\n";
      for (UnsignedInt i = 0; i < validationErrors.size(); ++i)
         validationMessage += validationErrors[i];
      MessageInterface::PopupMessage
         (Gmat::ERROR_, validationMessage);
      canClose = false;
   }

   // check to see if there is at least one propagator
   if (blankProps == MAX_PROP_ROW)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Propagate command must have at least one propagator.\n");
      canClose = false;
   }
   
   // check to see if there are any missing propagators for space objects
   if (emptyProps.GetCount() > 0)
   {
      for (UnsignedInt i=0; i<emptyProps.GetCount(); i++)
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Please select a Propagator for "
             "Spacecraft \"%s\"\n", emptyProps[i].WX_TO_C_STRING);
      
      canClose = false;
   }
   
   // check to see if there are any missing space objects to propagate
   if (emptySos.GetCount() > 0)
   {
      for (UnsignedInt i=0; i<emptySos.GetCount(); i++)
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Please select Spacecraft for "
             "Propagator \"%s\"\n", emptySos[i].WX_TO_C_STRING);
      
      canClose = false;
   }
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   Real stopTol;
   if (mStopTolChanged)
   {
      CheckReal(stopTol, mStopTolTextCtrl->GetValue().c_str(), "StopTolerance",
                "Real Number > 0", false, true);
   }
   
   if (mStopCondChanged)
   {
      for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
      {
         if ((stopCondGrid->GetCellValue(i, STOPCOND_LEFT_COL) != "") ||
              (stopCondGrid->GetCellValue(i, STOPCOND_RIGHT_COL) != ""))
         {
            ObjectTypeArray objTypes;
            objTypes.push_back(Gmat::SPACE_POINT); // is this right? or should it only be SPACECRAFT?
            objTypes.push_back(Gmat::IMPULSIVE_BURN);

            CheckVariable(stopCondGrid->GetCellValue
                          (i, STOPCOND_LEFT_COL).c_str(), objTypes, "Parameter",
                          "Variable, Array element, plottable Parameter", true, true);
            
            // check condition if parameter is not Periapsis nor Apoapsis
            wxString paramName = stopCondGrid->GetCellValue(i, STOPCOND_LEFT_COL);
            if (!paramName.Contains(".Periapsis") && !paramName.Contains(".Apoapsis"))
            {
               CheckVariable(stopCondGrid->GetCellValue
                             (i, STOPCOND_RIGHT_COL).c_str(), objTypes, "Condition",
                             "Variable, Array element, plottable Parameter", true, true);
            }
         }
      }
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      //-------------------------------------------------------
      // Save propagation mode
      //-------------------------------------------------------
      if (mPropModeChanged)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save propagation mode \n");
         #endif
         
         mPropModeChanged = false;
         wxString str = mPropModeComboBox->GetStringSelection();
         if (str.CmpNoCase("None") == 0)
            str = "";
         thePropCmd->SetStringParameter
            (thePropCmd->GetParameterID("PropagateMode"), str.c_str());
      }
      
      //---------------------------------------
      // Save propagator and spacecraft
      //---------------------------------------
      if (mPropSatChanged)
      {         
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save propagator and spacecraft \n");
         #endif
         
         mPropSatChanged = false;
         Integer propId = thePropCmd->GetParameterID("Propagator");
         Integer scId = thePropCmd->GetParameterID("Spacecraft");
         
         // Clear propagator and spacecraft list
         thePropCmd->TakeAction("Clear", "Propagator");
         
         Integer soCount = 0;
         mPropCount = 0;
         for (Integer i=0; i<MAX_PROP_ROW; i++)
         {
            #ifdef DEBUG_PROPAGATE_PANEL_SAVE
            MessageInterface::ShowMessage
               ("PropagatePanel::SaveData() "
                "propagator name[%2d] = '%s', spacecraft name[%2d] = '%s'\n",
                i, propGrid->GetCellValue(i, PROP_NAME_COL).c_str(),
                i, propGrid->GetCellValue(i, PROP_SOS_COL).c_str());
            #endif
            
            
            if ((propGrid->GetCellValue(i, PROP_NAME_COL) != "") || 
                 (propGrid->GetCellValue(i, PROP_SOS_COL) != ""))
            {
               mTempProp[mPropCount].propName = 
                  propGrid->GetCellValue(i, PROP_NAME_COL).c_str(); 
            
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   propName[%d] = '%s'\n", mPropCount,
                   mTempProp[mPropCount].propName.c_str());
               #endif
               
               // saving propagator
               thePropCmd->SetStringParameter 
                  (propId, mTempProp[mPropCount].propName.c_str());
               
               // saving spacecraft
               std::string spacecraftStr = 
                  propGrid->GetCellValue(i, PROP_SOS_COL).WX_TO_STD_STRING;
               StringArray parts = 
                  GmatStringUtil::SeparateBy(spacecraftStr, ", ");
               
               soCount = parts.size();                    
               for (Integer j=0; j<soCount; j++)
               {
                  #ifdef DEBUG_PROPAGATE_PANEL_SAVE
                  MessageInterface::ShowMessage
                     ("     scList[%d] = '%s'\n", j, parts[j].c_str());
                  #endif
                  
                  thePropCmd->SetStringParameter
                     (scId, std::string(parts[j].c_str()), mPropCount);
               }
               ++mPropCount;
            } // if
         } // for MAX_PROP_ROW
      } // if (mPropSatChanged)
      
      
      //---------------------------------------
      // Save the prop direction
      //---------------------------------------
      if (mPropDirChanged)
      {
         mPropDirChanged = false;
         thePropCmd->SetBooleanParameter("PropForward",
                                         !(backPropCheckBox->IsChecked()));
      }
      
      //---------------------------------------
      // Save STM and A-Matrix settings
      //---------------------------------------
      if (mPropStmChanged)
      {
         mPropStmChanged = false;
         thePropCmd->SetBooleanParameter("AllSTMs",
               stmPropCheckBox->IsChecked());
      }

      if (mCalcAmatrixChanged)
      {
         mCalcAmatrixChanged = false;
         thePropCmd->SetBooleanParameter("AllAMatrices",
               aMatrixCalcCheckBox->IsChecked());
      }

      //---------------------------------------
      // Save stop tolerance
      //---------------------------------------
      if (mStopTolChanged)
      {
         mStopTolChanged = false;
         thePropCmd->SetRealParameter("StopTolerance", stopTol);
      }
      
      //---------------------------------------
      // Save stopping condition
      //---------------------------------------
      
      #ifdef DEBUG_PROPAGATE_PANEL_SAVE
      MessageInterface::ShowMessage
         ("PropagatePanel::SaveData() mPropCount=%d\n", mPropCount);
      #endif
      
      if (mStopCondChanged)
      {
         #ifdef DEBUG_PROPAGATE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() Save stopping conditions\n");
         #endif
         
         mStopCondChanged = false;
         thePropCmd->TakeAction("Clear", "StopCondition");
         
         mStopCondCount = 0;
         // count number of stopping conditions
         for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
         {
            if (stopCondGrid->GetCellValue(i, STOPCOND_LEFT_COL) != "")
            {
               UpdateStopCondition(i);

               StopCondition *currStop = mTempStopCond[mStopCondCount].stopCondPtr;
               std::string nameStr = mTempStopCond[i].name.WX_TO_STD_STRING;
               std::string stopStr = mTempStopCond[i].varName.WX_TO_STD_STRING;
               std::string goalStr = mTempStopCond[i].goalStr.WX_TO_STD_STRING;
               
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   Saving stop condition, name='%s', stop='%s', goal='%s'\n",
                   nameStr.c_str(), stopStr.c_str(), goalStr.c_str());
               #endif
               
               currStop->SetName(nameStr);
               currStop->SetStringParameter("StopVar", stopStr);
               currStop->SetStringParameter("Goal", goalStr);
               
               thePropCmd->
                  SetRefObject(mTempStopCond[mStopCondCount].stopCondPtr, 
                               Gmat::STOP_CONDITION, "", mStopCondCount);
               
               mStopCondCount++;
               
               #ifdef DEBUG_PROPAGATE_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() mStopCondCount=%d\n", 
                   mStopCondCount);
               #endif
            } // STOPCOND_LEFT_COL != ""
         } // for MAX_STOPCOND_ROW
         
         // Validate command to create stop condition wrappers
         theGuiInterpreter->ValidateCommand(thePropCmd);
         
      } // if (mStopCondChanged)
      
      // Save new colors if changed
      if (mColorPanel->HasColorChanged() || mColorPanel->HasOverrideColorChanged())
      {
         bool overrideColor = mColorPanel->GetOverrideColor();
         UnsignedInt newOrbColor = mColorPanel->GetOrbitColor();
         UnsignedInt newTargColor = mColorPanel->GetTargetColor();
         #ifdef DEBUG_COLOR
         MessageInterface::ShowMessage
            ("PropagatePanel Saving... overrideColor=%d, newOrbColor=%06X, newTargColor=%06X\n",
             overrideColor, newOrbColor, newTargColor);
         #endif
         thePropCmd->SetSegmentOrbitColor(newOrbColor);
         thePropCmd->SetOverrideSegmentColor(overrideColor);
         
         // Update Propagate item text color in the MissionTree
         (GmatAppData::Instance()->GetMissionTree())->ChangeNodeColor(theNodeId, newOrbColor);
      }
      #ifdef DEBUG_PROPAGATE_PANEL_SAVE
      MessageInterface::ShowMessage("PropagatePanel::SaveData() leaving\n");
      #endif
   } // try
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      #ifdef DEBUG_PROPAGATE_PANEL_SAVE
      MessageInterface::ShowMessage("PropagatePanel::SaveData() leaving\n");
      #endif
      return;
   }
}

//------------------------------------------------------------------------------
// void OnGridTabbing(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user tabs in the grid
 *
 * @param  event   grid event to handle
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnGridTabbing(wxGridEvent& event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    if (event.GetEventObject() == propGrid)
    {
        if (!event.ShiftDown() &&
            (row == (propGrid->GetNumberRows() - 1)) &&
            (col == (propGrid->GetNumberCols() - 1)))
        {
            propGrid->Navigate( wxNavigationKeyEvent::IsForward );
        }
        else if (event.ShiftDown() &&
            (row == 0) &&
            (col == 0))
        {
            propGrid->Navigate( wxNavigationKeyEvent::IsBackward );
        }
        else
            event.Skip();
    }
    else if (event.GetEventObject() == stopCondGrid)
    {
        if (!event.ShiftDown() &&
            (row == (stopCondGrid->GetNumberRows() - 1)) &&
            (col == (stopCondGrid->GetNumberCols() - 1)))
        {
            stopCondGrid->Navigate( wxNavigationKeyEvent::IsForward );
        }
        else if (event.ShiftDown() &&
            (row == 0) &&
            (col == 0))
        {
            stopCondGrid->Navigate( wxNavigationKeyEvent::IsBackward );
        }
        else
            event.Skip();
    }
} 
   

