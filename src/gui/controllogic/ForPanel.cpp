//$Id$
//------------------------------------------------------------------------------
//                              ForPanel
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
// Author: Allison Greene
// Created: 2004/05/17
//
/**
 * Implements the For loop setup window.
 */
//------------------------------------------------------------------------------
#include "ForPanel.hpp"
#include "ParameterSelectDialog.hpp"

//#define DEBUG_FOR_PANEL_LOAD 1
//#define DEBUG_FOR_PANEL_SAVE 1
//#define DEBUG_FOR_PANEL_CELL 1


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ForPanel, GmatPanel)
   EVT_GRID_CELL_LEFT_CLICK(ForPanel::OnCellLeftClick)   
   EVT_GRID_CELL_RIGHT_CLICK(ForPanel::OnCellRightClick)   
   EVT_GRID_CELL_CHANGE(ForPanel::OnCellValueChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ForPanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Creates a ForPanel (constructor).
 *
 * @param parent   the parent window
 * @param cmd      the command object
 */
//------------------------------------------------------------------------------
ForPanel::ForPanel(wxWindow *parent, GmatCommand *cmd)
    : GmatPanel(parent)
{
   theForCommand = (For *)cmd;
   
   mStartValue   = 0;
   mIncrValue    = 0;
   mEndValue     = 0;
   
   mIndexIsParam = false;
   mStartIsParam = false;
   mEndIsParam   = false;
   mIncrIsParam  = false;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~ForPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the ForPanel (destructor).
 */
//------------------------------------------------------------------------------
ForPanel::~ForPanel()
{
   mObjectTypeList.Clear();
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the ForPanel widgets.
 */
//------------------------------------------------------------------------------
void ForPanel::Create()
{
   // wxGrid
   #if __WXMAC__
      conditionGrid =
         new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,65), wxWANTS_CHARS);
   #else
      conditionGrid =
         new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(600,65), wxWANTS_CHARS);
   #endif
   
   conditionGrid->CreateGrid(1, MAX_COL, wxGrid::wxGridSelectCells);
   conditionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
   conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
 
   #if __WXMAC__
      conditionGrid->SetColSize(INDEX_SEL_COL, 25);
      conditionGrid->SetColSize(INDEX_COL, 155);
      conditionGrid->SetColSize(START_SEL_COL, 25);
      conditionGrid->SetColSize(START_COL, 155);
      conditionGrid->SetColSize(INCR_SEL_COL, 25);
      conditionGrid->SetColSize(INCR_COL, 155);
      conditionGrid->SetColSize(END_SEL_COL, 25);
      conditionGrid->SetColSize(END_COL, 155);
   #else
      conditionGrid->SetColSize(INDEX_SEL_COL, 25);
      conditionGrid->SetColSize(INDEX_COL, 125);
      conditionGrid->SetColSize(START_SEL_COL, 25);
      conditionGrid->SetColSize(START_COL, 125);
      conditionGrid->SetColSize(INCR_SEL_COL, 25);
      conditionGrid->SetColSize(INCR_COL, 125);
      conditionGrid->SetColSize(END_SEL_COL, 25);
      conditionGrid->SetColSize(END_COL, 125);
   #endif

   conditionGrid->SetColLabelValue(INDEX_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(INDEX_COL, _T("Index"));
   conditionGrid->SetColLabelValue(START_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(START_COL, _T("Start"));
   conditionGrid->SetColLabelValue(INCR_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(INCR_COL, _T("Increment"));
   conditionGrid->SetColLabelValue(END_SEL_COL, _T(""));
   conditionGrid->SetColLabelValue(END_COL, _T("End"));
   
   conditionGrid->SetRowLabelSize(0);
   
   conditionGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);   
   
   // Set ... before each column for selection
   conditionGrid->SetCellBackgroundColour(0, INDEX_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, START_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, INCR_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellBackgroundColour(0, END_SEL_COL, *wxLIGHT_GREY);
   conditionGrid->SetCellValue(0, INDEX_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, START_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, INCR_SEL_COL, _T("  ... "));
   conditionGrid->SetCellValue(0, END_SEL_COL, _T("  ... "));
   conditionGrid->SetReadOnly(0, INDEX_SEL_COL, true);
   conditionGrid->SetReadOnly(0, START_SEL_COL, true);
   conditionGrid->SetReadOnly(0, INCR_SEL_COL, true);
   conditionGrid->SetReadOnly(0, END_SEL_COL, true);
   #if wxCHECK_VERSION(3, 0, 0)
   conditionGrid->SetTabBehaviour(wxGrid::Tab_Leave);
   #endif
   
   // wx*Sizers
   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
   item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 0);
   
   theMiddleSizer->Add(item0, 0, wxGROW, 0);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data into the ForPanel widgets.
 */
//------------------------------------------------------------------------------
void ForPanel::LoadData()
{
   #if DEBUG_FOR_PANEL_LOAD
   MessageInterface::ShowMessage
      ("ForPanel::LoadData() entered, theForCommand=<%p>\n", theForCommand);
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theForCommand;
   
   if (theForCommand == NULL)
   {
      #if DEBUG_FOR_PANEL_LOAD
      MessageInterface::ShowMessage("ForPanel::LoadData() leaving, the command is NULL\n");
      #endif
      return;
   }
   
   try
   {
      Integer paramId;
      wxString str;

      paramId = theForCommand->GetParameterID("IndexName");
      mIndexString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("StartName");
      mStartString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("EndName");
      mEndString = theForCommand->GetStringParameter(paramId).c_str();

      paramId = theForCommand->GetParameterID("IncrementName");
      mIncrString = theForCommand->GetStringParameter(paramId).c_str();
      
      conditionGrid->SetCellValue(0, INDEX_COL, mIndexString);
      conditionGrid->SetCellValue(0, START_COL, mStartString); 
      conditionGrid->SetCellValue(0, END_COL, mEndString);
      conditionGrid->SetCellValue(0, INCR_COL, mIncrString); 
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_FOR_PANEL_LOAD
   MessageInterface::ShowMessage("ForPanel::LoadData() leaving\n");
   #endif

}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the ForPanel widgets to the For command.
 */
//------------------------------------------------------------------------------
void ForPanel::SaveData()
{
   #if DEBUG_FOR_PANEL_SAVE
   MessageInterface::ShowMessage("ForPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   ObjectTypeArray objTypes;
   objTypes.push_back(Gmat::SPACE_POINT);
   objTypes.push_back(Gmat::IMPULSIVE_BURN);
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   CheckVariable(mIndexString.c_str(), objTypes, "Index",
                 "Variable", false);
   CheckVariable(mStartString.c_str(), objTypes, "Start",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mIncrString.c_str(), objTypes, "Increment",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mEndString.c_str(), objTypes, "End",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   
   if (!canClose)
   {
      #if DEBUG_FOR_PANEL_SAVE
      MessageInterface::ShowMessage("ForPanel::SaveData() leaving, error encountered\n");
      #endif
      return;
   }

   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // Since validation is not done until element wrappers are created,
      // need to create a clone
      For *clonedForCommand = (For*)theForCommand->Clone();
      
      Integer paramId;
      paramId = clonedForCommand->GetParameterID("IndexName");
      clonedForCommand->SetStringParameter(paramId, mIndexString.c_str());
      
      paramId = clonedForCommand->GetParameterID("StartName");
      clonedForCommand->SetStringParameter(paramId, mStartString.c_str());
      
      paramId = clonedForCommand->GetParameterID("EndName");
      clonedForCommand->SetStringParameter(paramId, mEndString.c_str());
      
      paramId = clonedForCommand->GetParameterID("IncrementName");
      clonedForCommand->SetStringParameter(paramId, mIncrString.c_str());
      
      bool contOnError = theGuiInterpreter->GetContinueOnError();
      theGuiInterpreter->SetContinueOnError(false);
      if (!theGuiInterpreter->ValidateCommand(clonedForCommand))
         canClose = false;
      theGuiInterpreter->SetContinueOnError(contOnError);
      
      // Copy cloned command to original and validate if cloned validation was successful
      if (canClose)
      {
         #if DEBUG_FOR_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   Copying cloned <%p> to original <%p>, and validating command to create wrppers\n",
             clonedForCommand, theForCommand);
         #endif
         theForCommand->Copy(clonedForCommand);
         theGuiInterpreter->ValidateCommand(theForCommand);
      }
      
      delete clonedForCommand;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #if DEBUG_FOR_PANEL_SAVE
   MessageInterface::ShowMessage("ForPanel::SaveData() leaving, canClose=%d\n", canClose);
   #endif
}

//------------------------------------------------------------------------------
// void GetNewValue(Integer row, Integer col)
//------------------------------------------------------------------------------
/**
 * Gets the new value at the row and column specified.
 *
 * @param row    specified row
 * @param col    specified column
 */
//------------------------------------------------------------------------------
void ForPanel::GetNewValue(Integer row, Integer col)
{
   bool settableOnly = (col == INDEX_COL);

   wxArrayString mParamObjectTypeList;
   wxString DefaultType = "Variable";
   if (!settableOnly)
   {
	   DefaultType = "Spacecraft";
	   mParamObjectTypeList = mObjectTypeList;
   }
   ParameterSelectDialog paramDlg(this, mParamObjectTypeList,
			 GuiItemManager::SHOW_PLOTTABLE, 0, false, false,
			 !settableOnly, true, !settableOnly, !settableOnly, DefaultType, true, settableOnly);
   paramDlg.ShowModal();
   
   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      
      if (newParamName == conditionGrid->GetCellValue(row, col))
         return;
      
      conditionGrid->SetCellValue(row, col, newParamName);
      
      if (col == INDEX_COL)
         mIndexString = newParamName;
      else if (col == START_COL)
         mStartString = newParamName;
      else if (col == INCR_COL)
         mIncrString = newParamName;
      else if (col == END_COL)
         mEndString = newParamName;
      
      EnableUpdate(true);
   }  
}

//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user left-clicks on the cell.
 *
 * @param  event   grid event to handle
 */
//------------------------------------------------------------------------------
void ForPanel::OnCellLeftClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   
   conditionGrid->SelectBlock(row, col, row, col);
   conditionGrid->SetGridCursor(row, col);
   
   if (col == INDEX_SEL_COL || col == START_SEL_COL || col == INCR_SEL_COL ||
       col == END_SEL_COL)
      GetNewValue(row, col + 1);
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user right-clicks on the cell.
 *
 * @param  event   grid event to handle
 */
//------------------------------------------------------------------------------
void ForPanel::OnCellRightClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();

   GetNewValue(row, col);
}     


//------------------------------------------------------------------------------
// void OnCellValueChange(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user changes the value in the cell.
 *
 * @param  event   grid event to handle
 */
//------------------------------------------------------------------------------
void ForPanel::OnCellValueChange(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   wxString valStr = conditionGrid->GetCellValue(row, col);
   
   #if DEBUG_FOR_PANEL_CELL
   MessageInterface::ShowMessage
      ("ForPanel::OnCellValueChange() row=%d, col=%d, value=%s\n", row, col,
       valStr.c_str());
   #endif
   
   if (col == INDEX_COL)
      mIndexString = valStr;
   else if (col == START_COL)
      mStartString = valStr;
   else if (col == INCR_COL)
      mIncrString = valStr;
   else if (col == END_COL)
      mEndString = valStr;
   
   EnableUpdate(true);  
}
