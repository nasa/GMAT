//$Id$
//------------------------------------------------------------------------------
//                              ArraySetupPanel
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
// Created: 2004/12/30
//
/**
 * Implements ArraySetupPanel class. This panel enables user to see and modify
 * array values.
 */
//------------------------------------------------------------------------------

#include "ArraySetupPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "Array.hpp"
#include "StringUtil.hpp"          // for ToString()
#include "MessageInterface.hpp"

//#define DEBUG_ARRAY_PANEL 1
//#define DEBUG_ARRAY_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ArraySetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON, ArraySetupPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBO, ArraySetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, ArraySetupPanel::OnTextUpdate)
   EVT_TEXT_ENTER(ID_TEXTCTRL, ArraySetupPanel::OnTextEnter)
   EVT_GRID_CELL_CHANGE(ArraySetupPanel::OnGridCellChange)
   #if wxCHECK_VERSION(3, 0, 0)
   EVT_GRID_TABBING(ArraySetupPanel::OnGridTabbing)
   #endif
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ArraySetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ArraySetupPanel::ArraySetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   mVarName = name;
   mIsArrValChanged = false;
   mNumRows = 0;
   mNumCols = 0;
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ArraySetupPanel::Create()
{
   int bsize = 2; // border size
   
   wxString arrValArray[] = { wxT("") };
   
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   // We can't add emptyStaticText to two layouts, so I duplicated it
   // When the window is deleted, it deletes it's children: if the control is added twice, it is deleted twice
   wxStaticText *emptyStaticText2 =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   //-------------------------------------------------------
   // for Array Setup
   //-------------------------------------------------------
   wxStaticText *arrNameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1RowStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Row"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1ColStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Column"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrEqualSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" = "), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrTimesStaticText =
      new wxStaticText(this, ID_TEXT, wxT(" X "), wxDefaultPosition, wxDefaultSize, 0);
   
   mArrNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0);
   mArrRowTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35,-1), 0);
   mArrColTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(35,-1), 0);
   
   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   
   // 1st row
   arr1FlexGridSizer->Add(arrNameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1RowStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(emptyStaticText2, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arr1ColStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   // 2nd row
   arr1FlexGridSizer->Add(mArrNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrEqualSignStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrRowTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(arrTimesStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrColTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   // show single element
   wxStaticText *commaStaticText =
      new wxStaticText(this, ID_TEXT, wxT(","),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   
   mRowComboBox = 
      new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(40,-1),
                     0, arrValArray, wxCB_DROPDOWN|wxCB_READONLY);
   mColComboBox = 
      new wxComboBox(this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(40,-1), 
                     0, arrValArray, wxCB_DROPDOWN|wxCB_READONLY);
   
   mArrValTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(100,-1), 0);
   mUpdateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                   wxDefaultPosition, wxDefaultSize, 0);
   
   wxBoxSizer *singleValBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   singleValBoxSizer->Add(mRowComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(commaStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mColComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(equalStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mArrValTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   singleValBoxSizer->Add(mUpdateButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // use wxGrid show array values
   mArrGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(300,157), wxWANTS_CHARS);
   
   mArrGrid->SetRowLabelSize(40);
   mArrGrid->SetColLabelSize(20);
   mArrGrid->SetScrollbars(5, 8, 15, 15);
   mArrGrid->EnableEditing(true);
   #if wxCHECK_VERSION(3, 0, 0)
   mArrGrid->SetTabBehaviour(wxGrid::Tab_Wrap);
   #endif
   wxBoxSizer *arrValBoxSizer = new wxBoxSizer(wxVERTICAL);
   arrValBoxSizer->Add(singleValBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   arrValBoxSizer->Add(mArrGrid, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   GmatStaticBoxSizer *arrStaticBoxSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Array");
   arrStaticBoxSizer->Add(arr1FlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   arrStaticBoxSizer->Add(arrValBoxSizer, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(arrStaticBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ArraySetupPanel::LoadData()
{
   mParam = theGuiInterpreter->GetParameter(std::string(mVarName.c_str()));

   // Set the pointer for the "Show Script" button
   mObject = mParam;

   if (mParam != NULL)
   {
      
      #if DEBUG_ARRAY_PANEL
      MessageInterface::ShowMessage
         ("ArraySetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
      #endif

      try
      {
         mNumRows = mParam->GetIntegerParameter("NumRows");
         mNumCols = mParam->GetIntegerParameter("NumCols");
         
         mArrNameTextCtrl->SetValue(mVarName);
         wxString str;
         str << mNumRows;
         mArrRowTextCtrl->SetValue(str);
         str = "";
         str << mNumCols;
         mArrColTextCtrl->SetValue(str);
         
         // append row index to ComboBox
         for (int i=0; i<mNumRows; i++)
         {
            wxString intStr;
            intStr << i+1;
            mRowComboBox->Append(intStr);
         }
         mRowComboBox->SetSelection(0);
         
         // append column index to ComboBox
         for (int i=0; i<mNumCols; i++)
         {
            wxString intStr;
            intStr << i+1;
            mColComboBox->Append(intStr);
         }
         mColComboBox->SetSelection(0);
         
         Array *arrParam = (Array*)mParam;
         
         // set value (wxTextCtrl)
         Real val = arrParam->GetRealParameter("SingleValue", 0, 0);
         mArrValTextCtrl->SetValue(theGuiManager->ToWxString(val));
         
         #ifdef DEBUG_ARRAY_PANEL
         MessageInterface::ShowMessage("mNumRows=%d, mNumCols=%d\n",
                                       mNumRows, mNumCols);
         #endif
         
         // set value (wxGrid)
         mArrGrid->CreateGrid(mNumRows, mNumCols);
         
         int row, col;
         Real rval;
         
         for (row=0; row<mNumRows; row++)
         {
            str.Printf("%d", row+1);
            mArrGrid->SetRowLabelValue(row, str);
         }
         
         for (col=0; col<mNumCols; col++)
         {
            str.Printf("%d", col+1);
            mArrGrid->SetColLabelValue(col, str);
         }
         
         // set size to Rmatrix and initialize
         mRmat.SetSize(mNumRows, mNumCols);
         for (row=0; row<mNumRows; row++)
         {
            for (col=0; col<mNumCols; col++)
            {
               mRmat.SetElement(row, col, arrParam->
                                GetRealParameter("SingleValue", row, col));
               
               #ifdef DEBUG_ARRAY_PANEL
               MessageInterface::ShowMessage
                  ("==> val(%d,%d)=%f\n", row, col, mRmat.GetElement(row, col));
               #endif
               
               rval = mRmat.GetElement(row, col);
               mArrGrid->SetCellValue(row, col, theGuiManager->ToWxString(rval));
            }
         }
      }
      catch (BaseException &e)
      {
         wxLogError(wxString(e.GetFullMessage().c_str()));
         wxLog::FlushActive();
      }
   }
   
   mArrNameTextCtrl->Disable();
   mArrRowTextCtrl->Disable();
   mArrColTextCtrl->Disable();
   mUpdateButton->Disable();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ArraySetupPanel::SaveData()
{
   #if DEBUG_ARRAY_SAVE
   MessageInterface::ShowMessage
      ("ArraySetupPanel::SaveData() mIsArrValChanged=%d\n", mIsArrValChanged);
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // Check text field and cell value
   //-----------------------------------------------------------------
   if (mIsArrValChanged)
   {
      wxString str;
      Real rval;
      
      if (mUpdateButton->IsEnabled())
         UpdateCellValue();
      
      for (int i=0; i<mNumRows; i++)
      {
         for (int j=0; j<mNumCols; j++)
         {
            str = mArrGrid->GetCellValue(i, j);
            CheckCellValue(rval, i, j, str.c_str());
         }
      }
   }
   
   #if DEBUG_ARRAY_SAVE
   MessageInterface::ShowMessage
      ("   mIsArrValChanged=%d, canClose=%d\n", mIsArrValChanged, canClose);
   #endif
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // Save cell value
   //-----------------------------------------------------------------
   
   mIsArrValChanged = false;
   Array *arrParam = (Array*)mParam;
   
   try
   {
      for (int i=0; i<mNumRows; i++)
         for (int j=0; j<mNumCols; j++)
            arrParam->SetRealParameter("SingleValue", mRmat.GetElement(i,j), i, j);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == mArrValTextCtrl &&
       mArrValTextCtrl->IsModified())
   {
      EnableUpdate(true);
      mIsArrValChanged = true;
      mUpdateButton->Enable();
   }
}


//------------------------------------------------------------------------------
// void OnTextEnter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnTextEnter(wxCommandEvent& event)
{
   if (event.GetEventObject() == mArrValTextCtrl)
   {
      UpdateCellValue();
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mRowComboBox ||
       event.GetEventObject() == mColComboBox)
   {
      Real rval = mRmat.GetElement(mRowComboBox->GetSelection(),
                                   mColComboBox->GetSelection());
      
      mArrValTextCtrl->SetValue(theGuiManager->ToWxString(rval));
   }
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnButtonClick(wxCommandEvent& event)
{    
   if (event.GetEventObject() == mUpdateButton)
   {
      UpdateCellValue();
   }
}


//------------------------------------------------------------------------------
// void OnGridCellChange(wxGridEvent& event)
//------------------------------------------------------------------------------
void ArraySetupPanel::OnGridCellChange(wxGridEvent& event)
{
   int row = mArrGrid->GetGridCursorRow();
   int col = mArrGrid->GetGridCursorCol();   
   wxString str = mArrGrid->GetCellValue(row, col);
   Real rval;

   mIsArrValChanged = true;
   CheckCellValue(rval, row, col, str.c_str());
}


//------------------------------------------------------------------------------
// void UpdateCellValue()
//------------------------------------------------------------------------------
void ArraySetupPanel::UpdateCellValue()
{
   int row = mRowComboBox->GetSelection();
   int col = mColComboBox->GetSelection();
   wxString str = mArrValTextCtrl->GetValue();
   Real rval;
   
   mIsArrValChanged = true;
   if (CheckCellValue(rval, row, col, str.c_str()))
   {
      mArrGrid->SetCellValue(row, col, str);
      mUpdateButton->Disable();
   }   
}

//------------------------------------------------------------------------------
// bool CheckCellValue(Real &rval, int row, int col, const char *str)
//------------------------------------------------------------------------------
bool ArraySetupPanel::CheckCellValue(Real &rval, int row, int col,
                                     const char *str)
{
   return CheckCellValue(rval, row, col, std::string(str));
}

//------------------------------------------------------------------------------
// bool CheckCellValue(Real &rval, int row, int col, const std::string &str)
//------------------------------------------------------------------------------
bool ArraySetupPanel::CheckCellValue(Real &rval, int row, int col,
                                     const std::string &str)
{
   std::string field = "(" + GmatStringUtil::ToString(row+1, 1) +
      "," + GmatStringUtil::ToString(col+1, 1) + ")";
   
   EnableUpdate(true);
   
   if (CheckReal(rval, str.c_str(), field, "Real Number"))
   {
      mRmat.SetElement(row, col, rval);
      return true;
   }
   else
   {
      return false;
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
void ArraySetupPanel::OnGridTabbing(wxGridEvent& event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    if (!event.ShiftDown() &&
        (row == (mArrGrid->GetNumberRows() - 1)) &&
        (col == (mArrGrid->GetNumberCols() - 1)))
    {
        mArrGrid->Navigate( wxNavigationKeyEvent::IsForward );
    }
    else if (event.ShiftDown() &&
        (row == 0) &&
        (col == 0))
    {
        mArrGrid->Navigate( wxNavigationKeyEvent::IsBackward );
    }
    else
        event.Skip();
} 

