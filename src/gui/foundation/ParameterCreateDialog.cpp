//$Id$
//------------------------------------------------------------------------------
//                              ParameterCreateDialog
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
 * Implements ParameterCreateDialog class. This class shows dialog window where a
 * user parameter can be created.
 * 
 */
//------------------------------------------------------------------------------

#include "ParameterCreateDialog.hpp"
#include "ParameterSelectDialog.hpp"
#include "GmatMainFrame.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GmatAppData.hpp"              // for GetResourceTree()
#include "ResourceTree.hpp"
#include "RgbColor.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "StringUtil.hpp"               // for GmatStringUtil::
#include "MessageInterface.hpp"
#include "StringTokenizer.hpp"
#include "Array.hpp"
#include "gmatdefs.hpp"
#include "ArraySetupDialog.hpp"
#include "bitmaps/Erase.xpm"

#include <wx/help.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/config.h>
#include <wx/variant.h>                 // for wxVariant()

//#define DEBUG_PARAM_CREATE
//#define DEBUG_PARAM_CREATE_VAR
//#define DEBUG_PARAM_CREATE_LOAD
//#define DEBUG_PARAM_CREATE_SAVE
//#define DEBUG_PAGE_CHANGED

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterCreateDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, ParameterCreateDialog::OnOK)
   EVT_BUTTON(ID_CREATE_BUTTON, ParameterCreateDialog::OnCreateButton)
   EVT_BUTTON(ID_SELECT_BUTTON, ParameterCreateDialog::OnSelectButtonClick)
   EVT_BUTTON(ID_EDITARRAY_BUTTON, ParameterCreateDialog::OnEditArrayButtonClick)
   EVT_BUTTON(ID_CLEAR_VAR_BUTTON, ParameterCreateDialog::OnClearButtonClick)
   EVT_BUTTON(ID_CLEAR_ARR_BUTTON, ParameterCreateDialog::OnClearButtonClick)
   EVT_BUTTON(ID_CLEAR_STR_BUTTON, ParameterCreateDialog::OnClearButtonClick)
   EVT_TEXT(ID_VARTEXTCTRL, ParameterCreateDialog::OnVarTextUpdate)
   EVT_TEXT_ENTER(ID_VARTEXTCTRL, ParameterCreateDialog::OnTextEnter)
   EVT_TEXT(ID_ARYTEXTCTRL, ParameterCreateDialog::OnAryTextUpdate)
   EVT_TEXT_ENTER(ID_ARYTEXTCTRL, ParameterCreateDialog::OnTextEnter)
   EVT_TEXT(ID_STRTEXTCTRL, ParameterCreateDialog::OnStrTextUpdate)
   EVT_TEXT_ENTER(ID_STRTEXTCTRL, ParameterCreateDialog::OnTextEnter)
   EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, ParameterCreateDialog::OnPageChanged)
   EVT_LISTBOX(ID_LISTBOX, ParameterCreateDialog::OnListboxClick)
END_EVENT_TABLE()
   
//------------------------------------------------------------------------------
// ParameterCreateDialog(wxWindow *parent, int paramType)
//------------------------------------------------------------------------------
/*
 * @param paramType 1 = Variable, 2 = Array, 3 = String
 */
//------------------------------------------------------------------------------
ParameterCreateDialog::ParameterCreateDialog(wxWindow *parent, ParameterType paramType)
   : GmatDialog(parent, -1, wxString(_T("New Variable, Array, or String")))
{
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog() entered, paramType=%d\n", paramType);
   #endif
   
   mParamType = paramType;
   mCurrParam = NULL;
   mParamNames.Clear();
   mIsParamCreated = false;
   mPageChangedByUser = false;
   mArrayChanged = false;
   mVariableChanged = false;
   mStringChanged = false;
   mSelectVarStrings.Add("Spacecraft");
   mSelectVarStrings.Add("SpacePoint");
   mSelectVarStrings.Add("ImpulsiveBurn");
   
   Create(); 
   SetParameterType( paramType );
   ShowData();
   mPageChangedByUser = true;
}


//------------------------------------------------------------------------------
// ParameterCreateDialog(wxWindow *parent, string paramName)
//------------------------------------------------------------------------------
/*
 */
//------------------------------------------------------------------------------
ParameterCreateDialog::ParameterCreateDialog(wxWindow *parent, const wxString paramName)
   : GmatDialog(parent, -1, wxString(_T("New Variable, Array, or String")))
{
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog() entered, paramName='%s'\n", paramName.c_str());
   #endif
   
   mObjectName = paramName.c_str();
   mCurrParam = (Parameter*)theGuiInterpreter->GetConfiguredObject(mObjectName);
   if (!mCurrParam)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot find the parameter object named " + mObjectName);
   }
   else
   {
      mParamNames.Clear();
      mIsParamCreated = false;
      mPageChangedByUser = false;
      mSelectVarStrings.Add("Spacecraft");
      mSelectVarStrings.Add("SpacePoint");
      mSelectVarStrings.Add("ImpulsiveBurn");
      
      Create(); 
      std::string s = mCurrParam->GetTypeName();
      if (s == "String")
         mParamType = STRING;
      else if (s == "Array")
         mParamType = ARRAY;
      else
         mParamType = VARIABLE;
      SetParameterType( mParamType );
      ShowData();
      mPageChangedByUser = true;
   }
}


//------------------------------------------------------------------------------
// ~ParameterCreateDialog()
//------------------------------------------------------------------------------
ParameterCreateDialog::~ParameterCreateDialog()
{
   mSelectVarStrings.Clear();   
}


//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Closes the page
 */
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnOK(wxCommandEvent &event)
{
   Close();
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void ParameterCreateDialog::Create()
{
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage("ParameterCreateDialog::Create() entered\n");
   #endif
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   int bsize = 2;
   std::string CreateLabel = "=" GUI_ACCEL_KEY ">";
   wxBitmap clearBitmap = wxBitmap(Erase_xpm);
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Parameter"));

   notebook = new wxNotebook(this, ID_NOTEBOOK);
   wxPanel *varPanel = new wxPanel(notebook);
   wxPanel *arrPanel = new wxPanel(notebook);
   wxPanel *strPanel = new wxPanel(notebook);
   //wxStaticText
   wxStaticText *varNameStaticText =
      new wxStaticText(varPanel, ID_TEXT, "Variable " GUI_ACCEL_KEY "Name",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *expStaticText =
      new wxStaticText(varPanel, ID_TEXT, "Variable " GUI_ACCEL_KEY "Value",
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *varEqualSignStaticText =
      new wxStaticText(varPanel, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrNameStaticText =
      new wxStaticText(arrPanel, ID_TEXT, "Array " GUI_ACCEL_KEY "Name",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1RowStaticText =
      new wxStaticText(arrPanel, ID_TEXT, GUI_ACCEL_KEY"Row",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arr1ColStaticText =
      new wxStaticText(arrPanel, ID_TEXT, GUI_ACCEL_KEY"Column",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrEqualSignStaticText =
      new wxStaticText(arrPanel, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *arrTimesStaticText =
      new wxStaticText(arrPanel, ID_TEXT, wxT(" X"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *stringNameLabel =
      new wxStaticText(strPanel, ID_TEXT, "String " GUI_ACCEL_KEY "Name",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *stringEqualSignStaticText =
      new wxStaticText(strPanel, ID_TEXT, wxT("="),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *stringValueLabel =
      new wxStaticText(strPanel, ID_TEXT, "String " GUI_ACCEL_KEY "Value",
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *configStringLabel =
      new wxStaticText(strPanel, ID_TEXT, GUI_ACCEL_KEY"Strings",
                        wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarClearButton =
      new wxBitmapButton(varPanel, ID_CLEAR_VAR_BUTTON, clearBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mVarClearButton->SetToolTip(pConfig->Read(_T("ClearVariableHint"), "Clear Variable Fields"));
   
   mVarNameTextCtrl = new wxTextCtrl(varPanel, ID_VARTEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(130,-1), wxTE_PROCESS_ENTER);
   mVarNameTextCtrl->SetToolTip(pConfig->Read(_T("VariableNameHint")));

   // Only numeric value is allowed (LOJ: 2010.11.24)
   mVarValueTextCtrl = new wxTextCtrl(varPanel, ID_VARTEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(280,-1), wxTE_PROCESS_ENTER,
                                  wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mVarValueTextCtrl->SetToolTip(pConfig->Read(_T("VariableValueHint")));
   
   mArrClearButton =
      new wxBitmapButton(arrPanel, ID_CLEAR_ARR_BUTTON, clearBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mArrClearButton->SetToolTip(pConfig->Read(_T("ClearArrayHint"), "Clear Array Fields"));
   
   mArrNameTextCtrl = new wxTextCtrl(arrPanel, ID_ARYTEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(102,-1), wxTE_PROCESS_ENTER);
   mArrNameTextCtrl->SetToolTip(pConfig->Read(_T("ArrayNameHint")));
   mArrRowTextCtrl = new wxTextCtrl(arrPanel, ID_ARYTEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(50,-1), wxTE_PROCESS_ENTER, 
                                    wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mArrRowTextCtrl->SetToolTip(pConfig->Read(_T("ArrayRowValueHint")));
   mArrColTextCtrl = new wxTextCtrl(arrPanel, ID_ARYTEXTCTRL, wxT(""),
                                    wxDefaultPosition, wxSize(50,-1), wxTE_PROCESS_ENTER,
                                    wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mArrColTextCtrl->SetToolTip(pConfig->Read(_T("ArrayColumnValueHint")));

   mStrClearButton =
      new wxBitmapButton(strPanel, ID_CLEAR_STR_BUTTON, clearBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mStrClearButton->SetToolTip(pConfig->Read(_T("ClearStringHint"), "Clear String Fields"));
   
   mStringNameTextCtrl = new wxTextCtrl(strPanel, ID_STRTEXTCTRL, wxT(""),
                                        wxDefaultPosition, wxSize(80,-1), wxTE_PROCESS_ENTER);
   mStringNameTextCtrl->SetToolTip(pConfig->Read(_T("StringNameHint")));
   mStringValueTextCtrl = new wxTextCtrl(strPanel, ID_STRTEXTCTRL, wxT(""),
                                     wxDefaultPosition, wxSize(110,-1), wxTE_PROCESS_ENTER);
   mStringValueTextCtrl->SetToolTip(pConfig->Read(_T("StringValueHint")));
   
   // wxButton
   mCreateVariableButton = new wxButton(varPanel, ID_CREATE_BUTTON, wxString(CreateLabel.c_str()),
                                        wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   mCreateVariableButton->SetToolTip(pConfig->Read(_T("CreateVariableHint")));
   mCreateVariableButton->Disable();
   mSelectButton = new wxButton(varPanel, ID_SELECT_BUTTON, wxT("Select"),
                                     wxDefaultPosition, wxDefaultSize, 0);
   mSelectButton->SetToolTip(pConfig->Read(_T("SelectHint")));

   mCreateArrayButton = new wxButton(arrPanel, ID_CREATE_BUTTON, wxString(CreateLabel.c_str()),
                                     wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   mCreateArrayButton->SetToolTip(pConfig->Read(_T("CreateArrayHint")));
   mCreateArrayButton->Disable();
   mEditArrayButton = new wxButton(arrPanel, ID_EDITARRAY_BUTTON, wxT("Edit"),
                                     wxDefaultPosition, wxDefaultSize, 0);
   mEditArrayButton->Disable();
   mEditArrayButton->SetToolTip(pConfig->Read(_T("EditArrayHint")));

   mCreateStringButton = new wxButton(strPanel, ID_CREATE_BUTTON, wxString(CreateLabel.c_str()),
                                      wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   mCreateStringButton->SetToolTip(pConfig->Read(_T("CreateStringHint")));
   mCreateStringButton->Disable();
   
   //wxArrayString
   wxArrayString emptyArray;
   
   // wxListBox
   mUserVarListBox =
      theGuiManager->GetUserVariableListBox(varPanel, ID_LISTBOX, wxSize(170, 125), "");
   mUserVarListBox->SetToolTip(pConfig->Read(_T("VariableListHint")));
   mUserArrayListBox =
      theGuiManager->GetUserArrayListBox(arrPanel, ID_LISTBOX, wxSize(170, 125), "");
   mUserArrayListBox->SetToolTip(pConfig->Read(_T("ArrayListHint")));
   mUserStringListBox =
      theGuiManager->GetUserStringListBox(strPanel, ID_LISTBOX, wxSize(170, 125), "");
   mUserStringListBox->SetToolTip(pConfig->Read(_T("StringListHint")));
          
   // wxSizers
   mDetailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);   
   
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(5, 0, 0);
   wxFlexGridSizer *objPropertyFlexGridSizer = new wxFlexGridSizer(4, 0, 0);
   wxFlexGridSizer *arr1FlexGridSizer = new wxFlexGridSizer(7, 0, 0);
   wxFlexGridSizer *stringFlexGridSizer = new wxFlexGridSizer(6, 0, 0);


   GmatStaticBoxSizer *variableStaticBoxSizer =
      new GmatStaticBoxSizer(wxHORIZONTAL, varPanel);
   
   GmatStaticBoxSizer *arrayStaticBoxSizer =
      new GmatStaticBoxSizer(wxHORIZONTAL, arrPanel);
   
   GmatStaticBoxSizer *stringStaticBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, strPanel);
   
   // Add to wx*Sizers
   //-------------------------------------------------------
   // for Variable
   //-------------------------------------------------------
   top1FlexGridSizer->Add(0, 0, 0, bsize);
   top1FlexGridSizer->Add(varNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(0, 0, 0, bsize);
   top1FlexGridSizer->Add(expStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(0, 0, 0, bsize);
   
   top1FlexGridSizer->Add(mVarClearButton, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mVarNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(varEqualSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mVarValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mCreateVariableButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   top1FlexGridSizer->Add(0, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(0, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(0, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(mSelectButton, 0, wxALIGN_LEFT|wxALL, bsize);
   top1FlexGridSizer->Add(0, 0, wxALIGN_CENTRE|wxALL, bsize);
   objPropertyFlexGridSizer->Add(mUserVarListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   variableStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   variableStaticBoxSizer->Add(objPropertyFlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   
   stringFlexGridSizer->Add(0, 0, 0, bsize);
   stringFlexGridSizer->Add(stringNameLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   stringFlexGridSizer->Add(0, 0, 0, bsize);
   stringFlexGridSizer->Add(stringValueLabel, 1, wxALIGN_CENTER|wxALL, bsize);
   stringFlexGridSizer->Add(0, 0, 0, bsize);
   stringFlexGridSizer->Add(configStringLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   
   stringFlexGridSizer->Add(mStrClearButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   stringFlexGridSizer->Add(mStringNameTextCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   stringFlexGridSizer->Add(stringEqualSignStaticText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   stringFlexGridSizer->Add(mStringValueTextCtrl, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   stringFlexGridSizer->Add(mCreateStringButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   stringFlexGridSizer->Add(mUserStringListBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   stringStaticBoxSizer->Add(stringFlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   
   //-------------------------------------------------------
   // for Array Creation
   //-------------------------------------------------------
   // 1st row
   arr1FlexGridSizer->Add(0, 0, 0, bsize);
   arr1FlexGridSizer->Add(arrNameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(0, 0, 0, bsize);
   arr1FlexGridSizer->Add(arr1RowStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(0, 0, 0, bsize);
   arr1FlexGridSizer->Add(arr1ColStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(0, 0, 0, bsize);
   //arr1FlexGridSizer->Add(configArrStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 2nd row
   arr1FlexGridSizer->Add(mArrClearButton, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(arrEqualSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrRowTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(arrTimesStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(mArrColTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   arr1FlexGridSizer->Add(mCreateArrayButton, 0, wxALIGN_CENTER|wxALL, bsize);
   //arr1FlexGridSizer->Add(mUserArrayListBox, 0, wxALIGN_CENTER|wxALL, bsize);

   arr1FlexGridSizer->Add(0, 0, 0, bsize);
   arr1FlexGridSizer->Add(mEditArrayButton, 0, wxALIGN_LEFT, bsize);

   arrayStaticBoxSizer->Add(arr1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   arrayStaticBoxSizer->Add(mUserArrayListBox, 0, wxALIGN_TOP|wxALL, bsize);
   
   varPanel->SetSizer(variableStaticBoxSizer);
   arrPanel->SetSizer(arrayStaticBoxSizer);
   strPanel->SetSizer(stringStaticBoxSizer);

   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   notebook->AddPage(varPanel, GUI_ACCEL_KEY"Variable", true);
   notebook->AddPage(arrPanel, GUI_ACCEL_KEY"Array", false);
   notebook->AddPage(strPanel, GUI_ACCEL_KEY"String", false);
   
   theMiddleSizer->Add(notebook, 0, wxALIGN_LEFT|wxGROW, 0);
   
   theCancelButton->SetLabel("Cancel");
   theOkButton->SetLabel("Close"); // OK button acts like Close   
   // Only numbers and string literals are allowed for initial values, so hide
   mSelectButton->Hide();
   
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage("ParameterCreateDialog::Create() exiting\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::LoadData()
{
   #ifdef DEBUG_PARAM_CREATE_LOAD
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::LoadData() entering, mObjectName='%s'\n", mObjectName.c_str());
   #endif
   
   wxString str;
   int mNumRows;
   int mNumCols;
//   Real rval;
   
   if (mObjectName != "")
   {
      mCurrParam =
         (Parameter*)theGuiInterpreter->GetConfiguredObject(mObjectName.c_str());
      
      // Set the pointer for the "Show Script" button
      mObject = mCurrParam;
      if (mCurrParam == NULL)
         return;
      
      switch (mParamType)
      {
         case VARIABLE:
            mVarNameTextCtrl->SetValue(mObjectName.c_str());
            // We no longer allow expression (LOJ: 2010.11.24)
            //mVarValueTextCtrl->SetValue(mCurrParam->GetStringParameter("Expression").c_str());
            mVarValueTextCtrl->SetValue(wxVariant(mCurrParam->GetRealParameter("Value")));
            mCreateVariableButton->Disable();
            mUserVarListBox->SetStringSelection(mObjectName.c_str());
            mVariableChanged = false;
            break;
         case ARRAY:
            mArrNameTextCtrl->SetValue(mObjectName.c_str());
            mNumRows = mCurrParam->GetIntegerParameter("NumRows");
            mNumCols = mCurrParam->GetIntegerParameter("NumCols");
            str << mNumRows;
            mArrRowTextCtrl->SetValue(str);
            str = "";
            str << mNumCols;
            mArrColTextCtrl->SetValue(str);
            mCreateArrayButton->Disable();
            mEditArrayButton->Enable(mCurrParam != NULL);
            mUserArrayListBox->SetStringSelection(mObjectName.c_str());
            mArrayChanged = false;
            break;
         case STRING:
            mStringNameTextCtrl->SetValue(mObjectName.c_str());
            mStringValueTextCtrl->SetValue(mCurrParam->GetStringParameter("Expression").c_str());
            mCreateStringButton->Disable();
            mUserStringListBox->SetStringSelection(mObjectName.c_str());
            mStringChanged = false;
            break;
      }
   }
   
   #ifdef DEBUG_PARAM_CREATE_LOAD
   MessageInterface::ShowMessage("ParameterCreateDialog::LoadData() exiting\n");
   #endif   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::SaveData()
{
   std::string s;
   Integer mNumCols;
   Integer mNumRows;

   canClose = true;
   wxString paramName;
   
   #ifdef DEBUG_PARAM_CREATE_SAVE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::SaveData() entered, mParamType=%d, mCurrParam=<%p>\n"
       "   mArrayChanged=%d, mVariableChanged=%d, mStringChanged=%d\n", mParamType,
       mCurrParam, mArrayChanged, mVariableChanged, mStringChanged);
   #endif
   
   // Check for the existing name
   
   switch (mParamType)
   {
      case VARIABLE:
         paramName = mVarNameTextCtrl->GetValue();
         if ((mCurrParam == NULL) || (mObjectName.c_str() != paramName))
         {
            CreateVariable();
         }
         else
         {
            std::string expr = mVarValueTextCtrl->GetValue().WX_TO_STD_STRING;
            Real rval;
            CheckReal(rval, expr, "Expression", "Real Number");
            #ifdef DEBUG_PARAM_CREATE_SAVE
            MessageInterface::ShowMessage
               ("   Setting %s to variable '%s'\n", expr.c_str(), mCurrParam->GetName().c_str());
            #endif
            mCurrParam->SetStringParameter("Expression", expr);
            ResetControls();
         }
         break;
      case ARRAY:
         paramName = mArrNameTextCtrl->GetValue();
         if ((mCurrParam == NULL) || (mObjectName.c_str() != paramName))
         {
            #ifdef DEBUG_PARAM_CREATE_SAVE
            MessageInterface::ShowMessage
               ("   Creating new Array '%s'\n", paramName.c_str());
            #endif
            CreateArray();
         }
         else
         {
            #ifdef DEBUG_PARAM_CREATE_SAVE
            MessageInterface::ShowMessage
               ("   Modifying existing Array '%s'\n", paramName.c_str());
            #endif
            
            s = mArrRowTextCtrl->GetValue().c_str();
            CheckIntegerRange(mNumRows, s, "Rows", 1, 1000, true, true, true, true);
            s = mArrColTextCtrl->GetValue().c_str();
            CheckIntegerRange(mNumCols, s, "Columns", 1, 1000, true, true, true, true);
            
            // Reset size if columns and rows are valid
            if (canClose)
            {
               #ifdef DEBUG_PARAM_CREATE_SAVE
               MessageInterface::ShowMessage
                  ("   Resetting size of Array '%s' to rows=%d, cols=%d\n",
                   paramName.c_str(), mNumRows, mNumCols);
               #endif
               ((Array *) mCurrParam)->SetSize(mNumRows, mNumCols, false);
            }
            
            if (canClose)
               ResetControls();
         }
         break;
      case STRING:
         paramName = mStringNameTextCtrl->GetValue();
         if ((mCurrParam == NULL) || (mObjectName.c_str() != paramName))
         {
            CreateString();
         }
         else
         {
            std::string expr = mStringValueTextCtrl->GetValue().WX_TO_STD_STRING;
            mCurrParam->SetStringParameter("Expression", expr);
            ResetControls();
         }
         break;
   }
   
   if (!canClose) return;
   
   EnableUpdate( mCreateVariableButton->IsEnabled() || 
                 mCreateArrayButton->IsEnabled() || 
                 mCreateStringButton->IsEnabled() );
   
   #ifdef DEBUG_PARAM_CREATE_SAVE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::SaveData() leaving, mParamType=%d, mCurrParam=<%p>\n"
       "   mArrayChanged=%d, mVariableChanged=%d, mStringChanged=%d\n", mParamType,
       mCurrParam, mArrayChanged, mVariableChanged, mStringChanged);
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ResetData()
{
   mIsParamCreated = false;
}


//------------------------------------------------------------------------------
// virtual void ResetControls()
//------------------------------------------------------------------------------
void ParameterCreateDialog::ResetControls()
{
   switch (mParamType)
   {
      case VARIABLE:
         mCreateVariableButton->Disable();
         mVarValueTextCtrl->SetValue("");
         mVarNameTextCtrl->SetValue("");
         mVariableChanged = false;
         mUserVarListBox->Deselect(mUserVarListBox->GetSelection());
         break;
      case ARRAY:
         mCreateArrayButton->Disable();
         mArrNameTextCtrl->SetValue("");
         mArrRowTextCtrl->SetValue("");
         mArrColTextCtrl->SetValue("");
         mEditArrayButton->Disable();
         mUserArrayListBox->Deselect(mUserArrayListBox->GetSelection());
         mArrayChanged = false;
         break;
      case STRING:
         mCreateStringButton->Disable();
         mStringNameTextCtrl->SetValue("");
         mStringValueTextCtrl->SetValue("");
         mUserStringListBox->Deselect(mUserStringListBox->GetSelection());
         mStringChanged = false;
         break;
   }
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnVarTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnVarTextUpdate(wxCommandEvent& event)
{
   mCreateVariableButton->Disable();

   if ((mVarNameTextCtrl->IsModified() &&
        mVarNameTextCtrl->GetValue().Trim() != "") ||
       (mVarValueTextCtrl->IsModified() &&
        mVarValueTextCtrl->GetValue().Trim() != ""))
   {
      mCreateVariableButton->Enable();
      EnableUpdate(true);
      mVariableChanged = true;
   }
   
}


//------------------------------------------------------------------------------
// void OnTextEnter(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnTextEnter(wxCommandEvent& event)
{
	((wxWindow *) event.GetEventObject())->GetNextSibling()->SetFocus();
}
 
//------------------------------------------------------------------------------
// void OnAryTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnAryTextUpdate(wxCommandEvent& event)
{
   mCreateArrayButton->Disable();
   
   if ((mArrNameTextCtrl->IsModified() &&
        mArrNameTextCtrl->GetValue().Trim() != "") ||
       (mArrRowTextCtrl->IsModified() &&
        mArrRowTextCtrl->GetValue().Trim() != "") ||
       (mArrColTextCtrl->IsModified() &&
        mArrColTextCtrl->GetValue().Trim() != ""))
   {
      mCreateArrayButton->Enable();
      EnableUpdate(true);
      mArrayChanged = true;
   }

}


//------------------------------------------------------------------------------
// void OnStrTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnStrTextUpdate(wxCommandEvent& event)
{
   mCreateStringButton->Disable();
   
   if ((mStringNameTextCtrl->IsModified() &&
        mStringNameTextCtrl->GetValue().Trim() != "") ||
       mStringValueTextCtrl->IsModified())
   {
      mCreateStringButton->Enable();
      EnableUpdate(true);
      mStringChanged = true;
   }
   
}


//------------------------------------------------------------------------------
// void OnCreateButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnCreateButton(wxCommandEvent& event)
{    
   SaveData();
   switch (mParamType)
   {
      case VARIABLE:
         mVarNameTextCtrl->SetFocus();
         break;
      case ARRAY:
         mArrNameTextCtrl->SetFocus();
         break;
      case STRING:
         mStringNameTextCtrl->SetFocus();
         break;
   }
}

//------------------------------------------------------------------------------
// void SetParameterType(ParameterType paramType)
//------------------------------------------------------------------------------
void ParameterCreateDialog::SetParameterType( ParameterType paramType )
{
   mParamType = paramType;

   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage
      ("SetParameterType() entered, mParamType=%d, mObjectName='%s'\n",
       mParamType, mObjectName.c_str());
   #endif
   
   // This SetSelection() is deprecated and should not be used in new code.
   // So used the ChangeSelection() function instead. (LOJ: 2010.11.29)
   notebook->ChangeSelection( (size_t) mParamType );
}


//------------------------------------------------------------------------------
// void OnPageChanged(wxNotebookEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnPageChanged(wxNotebookEvent& event)
{
   #ifdef DEBUG_PAGE_CHANGED
   MessageInterface::ShowMessage
      ("OnPageChanged() entered, mPageChangedByUser=%d, mArrayChanged=%d, "
       "mVariableChanged=%d, mStringChanged=%d\n", mPageChangedByUser, mArrayChanged,
       mVariableChanged, mStringChanged);
   #endif
   
   
   if (mPageChangedByUser)
   {
      // Show current selection data when page chaged by user
      mParamType = (ParameterType) event.GetSelection();
      switch (mParamType)
      {
      case VARIABLE:
         mObjectName = mUserVarListBox->GetStringSelection();
         if (!mVariableChanged)
            LoadData();
         break;
      case ARRAY:
         mObjectName = mUserArrayListBox->GetStringSelection();
         if (!mArrayChanged)
            LoadData();
         break;
      case STRING:
         mObjectName = mUserStringListBox->GetStringSelection();
         if (!mStringChanged)
            LoadData();
         break;
      }
   }
   
   // Show current selection data when page chages
   switch (mParamType)
   {
      case VARIABLE:
         mVarNameTextCtrl->SetFocus();
         break;
      case ARRAY:
         mArrNameTextCtrl->SetFocus();
         break;
      case STRING:
         mStringNameTextCtrl->SetFocus();
         break;
   }
}


//------------------------------------------------------------------------------
// void OnClearButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnClearButtonClick(wxCommandEvent& event)
{    
   switch (mParamType)
   {
      case VARIABLE:
         mVarNameTextCtrl->Clear();
         mVarValueTextCtrl->Clear();
         mUserVarListBox->Deselect(mUserVarListBox->GetSelection());
         break;
      case ARRAY:
         mArrNameTextCtrl->Clear();
         mArrRowTextCtrl->Clear();
         mArrColTextCtrl->Clear();
         mUserArrayListBox->Deselect(mUserArrayListBox->GetSelection());
         break;
      case STRING:
         mStringNameTextCtrl->Clear();
         mStringValueTextCtrl->Clear();
         mUserStringListBox->Deselect(mUserStringListBox->GetSelection());
         break;
   }
}


//------------------------------------------------------------------------------
// void OnEditArrayButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnEditArrayButtonClick(wxCommandEvent& event)
{
   #ifdef DEBUG_EDIT_ARRAY
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::OnEditArrayButtonClick() paramName='%s'\n",
       mArrNameTextCtrl->GetValue().c_str());
   #endif
   
   ArraySetupDialog paramDlg(this, mArrNameTextCtrl->GetValue());
   paramDlg.ShowModal();
}


//------------------------------------------------------------------------------
// void OnSelectButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnSelectButtonClick(wxCommandEvent& event)
{    

   ParameterSelectDialog paramDlg(this, mSelectVarStrings,
                                  GuiItemManager::SHOW_PLOTTABLE, false, true);
   
   paramDlg.SetParamNameArray(mSelectVarStrings);
   paramDlg.ShowModal();
   
   if (paramDlg.HasSelectionChanged())
   {
      wxArrayString selectVarStrings = paramDlg.GetParamNameArray();
      if (selectVarStrings.Count() > 0)
      {
         mVarValueTextCtrl->Clear();
         for (unsigned int i=0; i<selectVarStrings.Count(); i++)
            mVarValueTextCtrl->AppendText(selectVarStrings[i]);
      }
      else // no selections
      {
         mVarValueTextCtrl->Clear();
      }
   }
}


//------------------------------------------------------------------------------
// void OnHelp(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnHelp(wxCommandEvent &event)
{
   #ifdef DEBUG_GMAT_DIALOG_HELP
   MessageInterface::ShowMessage
      ("GmatDialog::OnHelp() entered, mObject=<%p><%s>'%s'\n", mObject,
       mObject ? mObject->GetTypeName().c_str() : "NULL",
       mObject ? mObject->GetName().c_str() : "NULL");
   #endif
   
   wxString objLink;
   wxString sHTML;
   wxString baseHelpLink;
   char msgBuffer[255];
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("/Help"));
   objLink = "VariableArrayString";
   
   FileManager* fm = FileManager::Instance();
   std::string helpPath = fm->GetFullPathname("HELP_PATH");
   helpPath = helpPath + _T("html/%s.html");

   sprintf(msgBuffer, helpPath.c_str(), objLink.WX_TO_C_STRING);
#ifdef DEBUG_GMATPANEL
   MessageInterface::ShowMessage
   ("GmatPanel::OnHelp() Default Help Link=%s\n", msgBuffer);
#endif
   // open separate window to show help
   objLink = pConfig->Read(objLink, wxString(&msgBuffer[0]));
#ifdef DEBUG_GMATPANEL
   MessageInterface::ShowMessage
   ("GmatPanel::OnHelp() Web Page=<%s>\n",
      s.WX_TO_C_STRING);
#endif

   //The following adds in the absolute path instead of the ..
   size_t start_pos = objLink.find("file://..");
   if (start_pos != std::string::npos)
   {

      std::string workingDir = GmatFileUtil::GetApplicationPath();
      std::string sepType = GmatFileUtil::GetPathSeparator();
      Integer end = workingDir.find_last_of(sepType);
      if (end != std::string::npos)
         workingDir = workingDir.substr(0, end);
      end = workingDir.find_last_of(sepType);
      if (end != std::string::npos)
         workingDir = workingDir.substr(0, end);
      objLink.replace(start_pos, 9, workingDir.c_str());
   }

   wxLaunchDefaultApplication(objLink);

   
   #ifdef DEBUG_GMAT_DIALOG_HELP
   MessageInterface::ShowMessage("GmatDialog::OnHelp() leaving\n");
   #endif

}


//------------------------------------------------------------------------------
// void OnListboxClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterCreateDialog::OnListboxClick(wxCommandEvent& event)
{
   wxString currObject = mObjectName.c_str();
   wxString nextObject = event.GetString();
   mObjectName = event.GetString();

   #ifdef DEBUG_LIST_BOX
   MessageInterface::ShowMessage
      ("OnListboxClick() entered, currObject='%s', nextObject='%s'\n   "
       "mArrayChanged=%d, mVariableChanged=%d, mStringChanged=%d\n", currObject.c_str(),
       nextObject.c_str(), mArrayChanged, mVariableChanged, mStringChanged);
   #endif

   #if 0
   bool objectChanged = false;
   
   if (event.GetEventObject() == mUserVarListBox)
      objectChanged = mVariableChanged;
   else if (event.GetEventObject() == mUserArrayListBox)
      objectChanged = mArrayChanged;
   else if (event.GetEventObject() == mUserStringListBox)
      objectChanged = mStringChanged;
   #endif
   
   // Prompt user for saving current object before switching to other of the same type
   //if (objectChanged)
   if (mVariableChanged || mArrayChanged || mStringChanged)
   {
      if (currObject != nextObject)
      {
         wxString paramTypeName = "Variable";
         if (mArrayChanged)
            paramTypeName = "Array";
         else if (mStringChanged)
            paramTypeName = "String";

         //wxString msg = "The value change made to will be lost, do you want to save it first?";
         wxString msg = "Do you want to save the new value before switching to "
            " other " + paramTypeName + "?";
         wxMessageDialog *msgDlg = new wxMessageDialog
            (this, msg, "Save...", wxYES_NO |wxICON_QUESTION, wxDefaultPosition);
         
         int result = msgDlg->ShowModal();
         if (result == wxID_YES)
         {
            // Save current object before switching to other
            mObjectName = currObject;
            SaveData();
            mObjectName = nextObject;
         }
         else if (result == wxID_NO)
         {
            if (mVariableChanged)
               mVariableChanged = false;
            else if (mArrayChanged)
               mArrayChanged = false;
            else if (mStringChanged)
               mStringChanged = false;
         }
      }
   }
   
   LoadData();
   
   #ifdef DEBUG_LIST_BOX
   MessageInterface::ShowMessage
      ("OnListboxClick() leaving, currObject='%s', mObjectName='%s'\n   "
       "mArrayChanged=%d, mVariableChanged=%d, mStringChanged=%d\n", currObject.c_str(),
       mObjectName.c_str(), mArrayChanged, mVariableChanged, mStringChanged);
   #endif
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const wxString &paramName)
//------------------------------------------------------------------------------
/*
 * @return newly created parameter pointer if it does not exist,
 *         return existing parameter pointer otherwise
 */
//------------------------------------------------------------------------------
Parameter* ParameterCreateDialog::CreateParameter(const wxString &name)
{
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::CreateParameter() name:%s\n", name.c_str());
   #endif
   
   std::string paramName(name.c_str());
   //std::string ownerName(mObjectListBox->GetStringSelection().c_str());
   //std::string propName(mPropertyListBox->GetStringSelection().c_str());
   //std::string depObjName = "";

   //if (mCentralBodyComboBox->IsShown())
   //{
   //   depObjName = std::string(mCentralBodyComboBox->GetStringSelection().c_str());
   //}
   //else if (mCoordSysComboBox->IsShown())
   //{
   //   depObjName = std::string(mCoordSysComboBox->GetStringSelection().c_str());
   //}
   
   Parameter *param = theGuiInterpreter->GetParameter(paramName);
   
   // create a parameter if it does not exist
   if (param == NULL)
   {
      //param = theGuiInterpreter->CreateParameter(propName, paramName);
      //param->SetRefObjectName(Gmat::SPACECRAFT, ownerName);
      
      //if (depObjName != "")
      //   param->SetStringParameter("DepObject", depObjName);
   }
   
   #ifdef DEBUG_PARAM_CREATE
   MessageInterface::ShowMessage("ParameterCreateDialog::CreateParameter() exiting\n");
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// void CreateVariable()
//------------------------------------------------------------------------------
/*
 * This method creates a variable after going through validation.
 */
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateVariable()
{
   wxString wxvarName = mVarNameTextCtrl->GetValue().Trim();
   std::string varName = std::string(wxvarName.c_str());
   wxString wxvarExpr = mVarValueTextCtrl->GetValue().Trim();
   std::string varExpr = std::string(wxvarExpr.c_str());
   Real realNum;
   bool isRealNumber = true;
   
   #ifdef DEBUG_PARAM_CREATE_VAR
   MessageInterface::ShowMessage
      ("ParameterCreateDialog::CreateVariable() entered, varName = "  + varName +
       " varExpr = " + varExpr + "\n");
   #endif
   
   // check if it has blank variable name or expression
   if (varName == "" || varExpr == "")
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Variable name or value cannot be blank");
      canClose = false;
      return;
   }
   
   // Trim blank spaces
   varName = GmatStringUtil::Trim(varName);
   
   // Check if name starts with number or contains non-alphanumeric character
   // Warning message is displayed from UserInputValidator::IsValidName()
   if (!IsValidName(varName.c_str()))
      return;
   
   // Check if name is any command type name
   if (theGuiInterpreter->IsCommandType(varName.c_str()))
   {
      std::string format = GmatStringUtil::GetInvalidNameMessageFormat();
      MessageInterface::PopupMessage
         (Gmat::ERROR_, format.c_str(), varName.c_str());
      canClose = false;
      return;
   }
   
   // check if rhs is a number
   if (!GmatStringUtil::ToReal(varExpr, realNum))
      isRealNumber = false;
   
   Parameter *param = NULL;
   
   // check if variable name already exist
   //if (theGuiInterpreter->GetParameter(varName) != NULL)
   if (theGuiInterpreter->GetConfiguredObject(varName) != NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The variable: \"%s\" cannot be created. "
          "The name already exists.", varName.c_str());
      canClose = false;
      return;
   }
   
   try
   {
      // create a variable if rhs is a number
      if (isRealNumber)
      {
         param = theGuiInterpreter->CreateParameter("Variable", varName);  
         param->SetStringParameter("Expression", varExpr);
      }
      else
      {
         #ifdef __ALLOW_SETTING_TO_ANOTHER_OBJECT__
            SetVariableToAnotherObject(varExpr);
         #else
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "\"%s\" is not a valid number", varExpr.c_str());
            canClose = false;
            return;
         #endif
      }
      
      #ifdef DEBUG_PARAM_CREATE_VAR
      MessageInterface::ShowMessage
         ("ParameterCreateDialog::CreateVariable() The variable \"%s\" added\n",
          varName.c_str());
      #endif
      
      mParamNames.Add(varName.c_str());
      mIsParamCreated = true;
      theGuiManager->UpdateParameter();
      
      GmatAppData::Instance()->GetResourceTree()->UpdateVariable();
      mUserVarListBox->Append(varName.c_str());
      
      for (unsigned int i=0; i<mUserVarListBox->GetCount(); i++)
      {
         if (mUserVarListBox->GetString(i).IsSameAs(varName.c_str()))
         {
            mUserVarListBox->SetSelection(i);
            break;
         }
      }
      
      // reset values 
      ResetControls();
      
      #ifdef DEBUG_PARAM_CREATE_VAR
      MessageInterface::ShowMessage
         ("ParameterCreateDialog::CreateVariable() leaving\n");
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void CreateString()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateString()
{
   wxString wxstrName = mStringNameTextCtrl->GetValue().Trim();
   std::string strName = std::string(wxstrName);
   std::string strValue = std::string(mStringValueTextCtrl->GetValue().c_str());

   try
   {
      // if new user string to create
      if (theGuiInterpreter->GetConfiguredObject(strName) == NULL)
      {
         // check if it has blank variable name
         if (strName == "")
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "String name cannot be blank");
            canClose = false;
            return;
         }
         
         // Trim blank spaces
         strName = GmatStringUtil::Trim(strName);
         
         // check if it has valid variable name
         if (!GmatStringUtil::IsValidName(strName))
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Invalid string name: \"%s.\" String name must "
                "follow GMAT variable name rules (start with an alphabetic character, "
                "only alphanumerics and underscores, no reserved words)", strName.c_str());
            canClose = false;
            return;
         }
         
         Parameter *param;
         
         param = theGuiInterpreter->CreateParameter("String", strName);
         param->SetStringParameter("Expression", strValue);
         
         mParamNames.Add(strName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();
         
         GmatAppData::Instance()->GetResourceTree()->UpdateVariable();
         mUserStringListBox->Append(strName.c_str());
         
         for (unsigned int i=0; i<mUserStringListBox->GetCount(); i++)
         {
            if (mUserStringListBox->GetString(i).IsSameAs(strName.c_str()))
            {
               mUserStringListBox->SetSelection(i);
               break;
            }
         }
         
         EnableUpdate(true);
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The string: \"%s\" cannot be created. "
             "The name already exists.", strName.c_str());
      }
      
      ResetControls();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
}


//------------------------------------------------------------------------------
// void CreateArray()
//------------------------------------------------------------------------------
void ParameterCreateDialog::CreateArray()
{
   std::string s;
   long row, col;
   Integer mNumCols, mNumRows;

   s = mArrRowTextCtrl->GetValue().c_str();
   CheckIntegerRange(mNumRows, s, "Rows", 1, 1000, true, false, true, true);
   s = mArrColTextCtrl->GetValue().c_str();
   CheckIntegerRange(mNumCols, s, "Columns", 1, 1000, true, false, true, true);
   
   if (!(mArrRowTextCtrl->GetValue().ToLong(&row)) ||
       !(mArrColTextCtrl->GetValue().ToLong(&col)))
   {
      wxLogError(wxT("Row or Column is not a number"));
      wxLog::FlushActive();
      canClose = false;
      return;
   }
   
   if (!canClose)
      return;
   
   try
   {
      wxString wxarrName = mArrNameTextCtrl->GetValue().Trim();
      std::string arrName = std::string(wxarrName.c_str());
      
      // if new user array to create
      if (theGuiInterpreter->GetConfiguredObject(arrName) == NULL)
      {
         // check if it has blank variable name or expression
         if (arrName == "")
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Array name cannot be blank");
            canClose = false;
            return;
         }
         
         // Trim blank spaces
         arrName = GmatStringUtil::Trim(arrName);
         
         // check if it has valid variable name
         if (!GmatStringUtil::IsValidName(arrName))
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Invalid array name: \"%s.\" Array name must "
                "follow GMAT variable name rules (start with an alphabetic character, "
                "only alphanumerics and underscores, no reserved words)", arrName.c_str());
            canClose = false;
            return;
         }
         
         Parameter *param;
         
         param = theGuiInterpreter->CreateParameter("Array", arrName);
         param->SetIntegerParameter("NumRows", row);
         param->SetIntegerParameter("NumCols", col);
         
         mParamNames.Add(arrName.c_str());
         mIsParamCreated = true;
         theGuiManager->UpdateParameter();
         
         GmatAppData::Instance()->GetResourceTree()->UpdateVariable();
         mUserArrayListBox->Append(arrName.c_str());
         
         for (unsigned int i=0; i<mUserArrayListBox->GetCount(); i++)
         {
            if (mUserArrayListBox->GetString(i).IsSameAs(arrName.c_str()))
            {
               mUserArrayListBox->SetSelection(i);
               break;
            }
         }
         
         EnableUpdate(true);
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The array: \"%s\" cannot be created. "
             "The name already exists.", arrName.c_str());
      }
      ResetControls();      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SetVariableToAnotherObject(const std::string &varName,
//                                 const std::string &varExpr)
//------------------------------------------------------------------------------
void ParameterCreateDialog::SetVariableToAnotherObject(const std::string &varName,
                                                       const std::string &varExpr)
{
   // Parse the Parameter
   //StringTokenizer st(varExpr, "()*/+-^ ");
   // tokenize nothing, we want no expressions, 04/2010 TGG
   StringTokenizer st(varExpr, "");
   StringArray tokens = st.GetAllTokens();
   StringArray paramArray;
   Real realNum;
   Parameter *param = NULL;
   
   // Check if unexisting varibles used in expression
   for (unsigned int i=0; i<tokens.size(); i++)
   {
      #ifdef DEBUG_PARAM_CREATE_VAR
      MessageInterface::ShowMessage("   token:<%s> \n", tokens[i].c_str());
      #endif
      
      if (!GmatStringUtil::ToReal(tokens[i], realNum))
      {
         // Check for the valid name
         if (!GmatStringUtil::IsValidName(tokens[i]))
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "\"%s\" is not a valid number or variable name",
                tokens[i].c_str());
            canClose = false;
            return;
         }
         
         // create system parameter if it is NULL
         if (theGuiInterpreter->GetParameter(tokens[i]) == NULL)
         {
            // check if it is system parameter
            std::string type, owner, depObj;
            GmatStringUtil::ParseParameter(tokens[i], type, owner, depObj);
            if (theGuiInterpreter->IsParameter(type))
            {
               #ifdef DEBUG_PARAM_CREATE_VAR
               MessageInterface::ShowMessage
                  ("type:%s is a system parameter\n", type.c_str());
               #endif
               
               Parameter *sysParam = 
                  theGuiInterpreter->CreateParameter(type, tokens[i]);
               
               // set ref. object name
               sysParam->SetRefObjectName(sysParam->GetOwnerType(), owner);
               
               // set dependent object name
               if (depObj != "")
                  sysParam->SetStringParameter("DepObject", depObj);
               
            }
            else
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "The variable \"%s\" does not exist. "
                   "It must be created first.", tokens[i].c_str());
               canClose = false;
               return;
            }
         }
         
         // create a variable
         param = theGuiInterpreter->CreateParameter("Variable", varName);
         param->SetStringParameter("Expression", varExpr);
         
         // set parameter names used in expression
         param->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
         
      }
   }      
}

