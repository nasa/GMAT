//$Id$
//------------------------------------------------------------------------------
//                         DynamicDataDisplaySetupPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2018/02/20, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines DynamicDataDisplaySetupPanel methods
*/
//------------------------------------------------------------------------------
#include "DynamicDataDisplaySetupPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(DynamicDataDisplaySetupPanel, GmatPanel)
EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
EVT_BUTTON(ID_BUTTON_UPDATE, DynamicDataDisplaySetupPanel::OnUpdate)
EVT_KEY_DOWN(DynamicDataDisplaySetupPanel::OnDelete)
EVT_COLOURPICKER_CHANGED(ID_COLOR, DynamicDataDisplaySetupPanel::OnColorPickerChange)
EVT_GRID_CELL_LEFT_DCLICK(DynamicDataDisplaySetupPanel::OnGridCellDClick)
#if wxCHECK_VERSION(3, 0, 0)
EVT_GRID_TABBING(DynamicDataDisplaySetupPanel::OnGridTabbing)
#endif
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DynamicDataDisplaySetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
DynamicDataDisplaySetupPanel::DynamicDataDisplaySetupPanel(wxWindow *parent,
   const wxString &name) : GmatPanel(parent)
{
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(name.c_str());

   mDisplay = (DynamicDataDisplay*)subscriber;

   mDisplayName = name;
   mIsCellValueChanged = false;
   mNumRows = 0;
   mNumCols = 0;

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::Create()
{
   int bsize = 2;

   // Setup for display sizing
   wxStaticText *displayRowStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Row",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *displayColStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Column",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *displayTimesSignStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "x",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *warnColorStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Warning Color",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *critColorStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Critical Color",
      wxDefaultPosition, wxDefaultSize, 0);

   mDisplayRowTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT("2"),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mDisplayColTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT("2"),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mWarnColorPickerCtrl = new wxColourPickerCtrl(this, ID_COLOR);
   mCritColorPickerCtrl = new wxColourPickerCtrl(this, ID_COLOR);
   mUpdateButton = new wxButton(this, ID_BUTTON_UPDATE, wxT("Update"));

   wxFlexGridSizer *displayFlexGridSizer = new wxFlexGridSizer(2, 4, 0, 0);

   // 1st row
   displayFlexGridSizer->Add(displayRowStaticText, 0, wxALIGN_CENTER | wxALL,
      bsize);
   displayFlexGridSizer->Add(0, 0, 0, bsize);
   displayFlexGridSizer->Add(displayColStaticText, 0, wxALIGN_CENTER | wxALL,
      bsize);
   displayFlexGridSizer->Add(0, 0, 0, bsize);

   // 2nd row
   displayFlexGridSizer->Add(mDisplayRowTextCtrl, 0, wxALIGN_CENTER | wxALL,
      bsize);
   displayFlexGridSizer->Add(displayTimesSignStaticText, 0, wxALIGN_CENTER |
      wxALL, bsize);
   displayFlexGridSizer->Add(mDisplayColTextCtrl, 0, wxALIGN_CENTER | wxALL,
      bsize);
   displayFlexGridSizer->Add(mUpdateButton, 0, wxALIGN_CENTER | wxLEFT, 50);

   // Create the display grid
   mDisplayDataGrid = new wxGrid(this, -1, wxDefaultPosition, wxSize(500,
      130));

   mDisplayDataGrid->SetRowLabelSize(40);
   mDisplayDataGrid->SetColLabelSize(20);
   mDisplayDataGrid->EnableEditing(false);
   mDisplayDataGrid->EnableDragGridSize(false);
   #if wxCHECK_VERSION(3, 0, 0)
   mDisplayDataGrid->SetTabBehaviour(wxGrid::Tab_Wrap);
   #endif

   GmatStaticBoxSizer *displayStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Data Table");
   displayStaticBoxSizer->Add(displayFlexGridSizer, 0, wxGROW |
      wxALIGN_CENTRE | wxALL, bsize);
   displayStaticBoxSizer->Add(mDisplayDataGrid, 1, wxGROW | wxALIGN_CENTER |
      wxALL, bsize);

   wxBoxSizer *colorsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   colorsBoxSizer->Add(warnColorStaticText, 0, wxALIGN_CENTRE | wxALL, bsize);
   colorsBoxSizer->Add(mWarnColorPickerCtrl, 0, wxALIGN_CENTRE | wxALL, bsize);
   colorsBoxSizer->Add(20, 20);
   colorsBoxSizer->Add(critColorStaticText, 0, wxALIGN_CENTRE | wxALL, bsize);
   colorsBoxSizer->Add(mCritColorPickerCtrl, 0, wxALIGN_CENTRE | wxALL, bsize);

   wxFlexGridSizer *colorsGridSizer = new wxFlexGridSizer(3);
   colorsGridSizer->Add(20, 20);
   colorsGridSizer->Add(colorsBoxSizer, 0, wxGROW | wxALL, bsize);
   colorsGridSizer->Add(20, 20);

   GmatStaticBoxSizer *colorsStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Condition Colors");
   colorsStaticBoxSizer->Add(colorsGridSizer, 0, wxALIGN_CENTER | wxALL,
      bsize);

   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(displayStaticBoxSizer, 1, wxGROW | wxALIGN_CENTRE | wxALL,
      bsize);
   pageBoxSizer->Add(colorsStaticBoxSizer, 0, wxGROW | wxALIGN_CENTRE | wxALL,
      bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 1, wxGROW | wxALIGN_CENTRE | wxALL,
      bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::LoadData()
{
   mObject = mDisplay;
   if (mDisplay != NULL)
   {
      displayDataStruct = mDisplay->GetDynamicDataStruct();

      // First check for empty cells to assign default values to
      Integer maxColCount = 0;
      for (Integer i = 0; i < displayDataStruct.size(); ++i)
      {
         if (displayDataStruct[i].size() > mNumCols)
            mNumCols = displayDataStruct[i].size();
      }

      for (Integer i = 0; i < displayDataStruct.size(); ++i)
      {
         if (displayDataStruct[i].size() < mNumCols)
         {
            Integer oldNumCols = displayDataStruct[i].size();
            displayDataStruct[i].resize(mNumCols);
            for (Integer j = oldNumCols; j < mNumCols; ++j)
               SetParamDefaultValues(displayDataStruct[i][j]);
         }
      }

      // Set text control values based on struct size
      mNumRows = displayDataStruct.size();

      mDisplayRowTextCtrl->SetValue(theGuiManager->ToWxString(mNumRows));
      mDisplayColTextCtrl->SetValue(theGuiManager->ToWxString(mNumCols));

      mDisplayDataGrid->CreateGrid(mNumRows, mNumCols);

      for (Integer i = 0; i < mNumRows; i++)
      {
         mDisplayDataGrid->SetRowLabelValue(i,
            theGuiManager->ToWxString(i + 1));
      }

      for (Integer j = 0; j < mNumCols; j++)
      {
         mDisplayDataGrid->SetColLabelValue(j,
            theGuiManager->ToWxString(j + 1));
      }

      for (Integer i = 0; i < mNumRows; ++i)
      {
         for (Integer j = 0; j < displayDataStruct[i].size(); ++j)
         {
            mDisplayDataGrid->SetCellValue(
               i, j, displayDataStruct[i][j].paramName);
            if (displayDataStruct[i][j].paramName != "")
               mDisplayDataGrid->AutoSizeColumn(j);
         }
      }

      for (Integer j = 0; j < mNumCols; ++j)
      {
         wxString colLabel = theGuiManager->ToWxString(j + 1);
         mDisplayDataGrid->SetColLabelValue(j, colLabel);
      }
   }

   mIntWarnColor =
      RgbColor::ToIntColor(mDisplay->GetStringParameter("WarnColor"));
   RgbColor rgbWarnColor = RgbColor(mIntWarnColor);
   wxColour rgbWxWarnColor = wxColour(rgbWarnColor.Red(), rgbWarnColor.Green(),
      rgbWarnColor.Blue());
   mWarnColorPickerCtrl->SetColour(rgbWxWarnColor);

   mIntCritColor =
      RgbColor::ToIntColor(mDisplay->GetStringParameter("CritColor"));
   RgbColor rgbCritColor = RgbColor(mIntCritColor);
   wxColour rgbWxCritColor = wxColour(rgbCritColor.Red(), rgbCritColor.Green(),
      rgbCritColor.Blue());
   mCritColorPickerCtrl->SetColour(rgbWxCritColor);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::SaveData()
{
   canClose = true;

   mDisplay->SetParamSettings(displayDataStruct);

   wxColour wxWarnColor = mWarnColorPickerCtrl->GetColour();
   RgbColor rgbWarnColor =
      RgbColor(wxWarnColor.Red(), wxWarnColor.Green(), wxWarnColor.Blue());
   std::string warnColor = RgbColor::ToRgbString(rgbWarnColor.GetIntColor());
   mDisplay->SetStringParameter("WarnColor", warnColor);

   wxColour waxCritColor = mCritColorPickerCtrl->GetColour();
   RgbColor rgbCritColor =
      RgbColor(waxCritColor.Red(), waxCritColor.Green(), waxCritColor.Blue());
   std::string critColor = RgbColor::ToRgbString(rgbCritColor.GetIntColor());
   mDisplay->SetStringParameter("CritColor", critColor);

   GmatBase *clonedObj = mDisplay->Clone();
   theGuiInterpreter->ValidateSubscriber(clonedObj);
   mDisplay->Copy(clonedObj);
   delete clonedObj;
}

//------------------------------------------------------------------------------
// void OnUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
* Method to handle the event when the update button is clicked to add or remove
* rows and columns from the DynamicDataDisplay
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::OnUpdate(wxCommandEvent& event)
{
   // Modify rows
   std::string newRowCountString = mDisplayRowTextCtrl->GetValue().WX_TO_C_STRING;
   Real newRowCount;
   GmatStringUtil::ToReal(newRowCountString, newRowCount);
   if (newRowCount < 0)
   {
      std::string errMsg = "The grid cannot contain a negative number of rows "
         "or columns.";
      MessageInterface::PopupMessage(Gmat::ERROR_, errMsg);
      return;
   }

   if (newRowCount > mNumRows)
   {
      mDisplayDataGrid->AppendRows(newRowCount - mNumRows);

      // Initialize new structs for the new rows
      displayDataStruct.resize(newRowCount);
      for (Integer i = mNumRows; i < newRowCount; ++i)
      {
         displayDataStruct[i].resize(mNumCols);
         for (Integer j = 0; j < mNumCols; ++j)
            SetParamDefaultValues(displayDataStruct[i][j]);
      }
      mNumRows = newRowCount;
   }
   else if (newRowCount < mNumRows)
   {
      mDisplayDataGrid->DeleteRows(newRowCount, mNumRows - newRowCount);
      displayDataStruct.resize(newRowCount);
      mNumRows = newRowCount;
   }

   // Modify columns
   std::string newColCountString = mDisplayColTextCtrl->GetValue().WX_TO_C_STRING;
   Real newColCount;
   GmatStringUtil::ToReal(newColCountString, newColCount);
   if (newColCount < 0)
   {
      std::string errMsg = "The grid cannot contain a negative number of rows "
         "or columns.";
      MessageInterface::PopupMessage(Gmat::ERROR_, errMsg);
      return;
   }

   if (newColCount > mNumCols)
   {
      mDisplayDataGrid->AppendCols(newColCount - mNumCols);

      // Set the labels of the new columns
      for (Integer j = mNumCols; j < newColCount; ++j)
      {
         wxString colLabel = theGuiManager->ToWxString(j + 1);
         mDisplayDataGrid->SetColLabelValue(j, colLabel);
      }

      // Initialize new structs for the new columns
      for (Integer i = 0; i < mNumRows; ++i)
      {
         displayDataStruct[i].resize(newColCount);
         for (Integer j = mNumCols; j < newColCount; ++j)
            SetParamDefaultValues(displayDataStruct[i][j]);
      }
      mNumCols = newColCount;
   }
   else if (newColCount < mNumCols)
   {
      mDisplayDataGrid->DeleteCols(newColCount, mNumCols - newColCount);
      for (Integer i = 0; i < displayDataStruct.size(); ++i)
         displayDataStruct[i].resize(newColCount);
      mNumCols = newColCount;
   }

   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnGridCellDClick(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
* Method to handle the event when a cell in the grid is double left clicked
* which leads the user to the dialog window to select parameter options for
* that cell
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::OnGridCellDClick(wxGridEvent& event)
{
   Integer rowIdx = event.GetRow();
   Integer colIdx = event.GetCol();

   DynamicDataSettingsDialog dataSettings(this, displayDataStruct[rowIdx][colIdx]);
   dataSettings.ShowModal();

   if (dataSettings.WasDataSaved())
   {
      displayDataStruct[rowIdx][colIdx] = dataSettings.GetParamSettings();
      mDisplayDataGrid->SetCellValue(rowIdx, colIdx, displayDataStruct[rowIdx][colIdx].paramName);
      if (displayDataStruct[rowIdx][colIdx].paramName != "")
         mDisplayDataGrid->AutoSizeColumn(colIdx);
      EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnGridTabbing(wxGridEvent& event)
//------------------------------------------------------------------------------
/**
* Method to handle tabbing directions in the grid
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::OnGridTabbing(wxGridEvent& event)
{
   Integer row = event.GetRow();
   Integer col = event.GetCol();
   if (!event.ShiftDown() &&
      (row == (mDisplayDataGrid->GetNumberRows() - 1)) &&
      (col == (mDisplayDataGrid->GetNumberCols() - 1)))
   {
      mDisplayDataGrid->Navigate(wxNavigationKeyEvent::IsForward);
   }
   else if (event.ShiftDown() &&
      (row == 0) &&
      (col == 0))
   {
      mDisplayDataGrid->Navigate(wxNavigationKeyEvent::IsBackward);
   }
   else
      event.Skip();
}

//------------------------------------------------------------------------------
// void OnColorPickerChange(wxColourPickerEvent& event)
//------------------------------------------------------------------------------
/**
* When the color pickers for warning and critical conditions are changed, allow
* the user to apply the changes
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::OnColorPickerChange(wxColourPickerEvent& event)
{
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnDelete(wxKeyEvent& event)
//------------------------------------------------------------------------------
/**
* Method called when a key is pressed, if it is the delete key the selected
* parameter cell is set to an empty parameter name with default settings.  Any
* other key is sent to be handled elsewhere.
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::OnDelete(wxKeyEvent& event)
{
   if (event.GetKeyCode() == WXK_DELETE)
   {
      Integer selectedRow = mDisplayDataGrid->GetGridCursorRow();
      Integer selectedCol = mDisplayDataGrid->GetGridCursorCol();

      SetParamDefaultValues(displayDataStruct[selectedRow][selectedCol]);
      mDisplayDataGrid->SetCellValue(selectedRow, selectedCol,
    		  displayDataStruct[selectedRow][selectedCol].paramName);

      EnableUpdate(true);
   }

   event.Skip();
}

//------------------------------------------------------------------------------
// void SetParamDefaultValues(DDD &paramSettings)
//------------------------------------------------------------------------------
/**
* Method to set the input DynamicDataDisplay struct to the default settings
*/
//------------------------------------------------------------------------------
void DynamicDataDisplaySetupPanel::SetParamDefaultValues(DDD &paramSettings)
{
   Real inf = 9.999e300;

   paramSettings.paramName = "";
   paramSettings.paramRef = NULL;
   paramSettings.refObjectName = "";
   paramSettings.paramTextColor =
      ColorDatabase::Instance()->GetIntColor("Black");
   paramSettings.paramBackgroundColor =
      ColorDatabase::Instance()->GetIntColor("White");
   paramSettings.warnLowerBound = -inf;
   paramSettings.warnUpperBound = inf;
   paramSettings.critLowerBound = -inf;
   paramSettings.critUpperBound = inf;
   paramSettings.isTextColorUserSet = false;
}
