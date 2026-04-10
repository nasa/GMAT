//$Id$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
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
 * Implements ParameterSetupPanel class. This panel is used for setting
 * values for user Variables and Strings.
 */
//------------------------------------------------------------------------------

#include "ParameterSetupPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ParameterSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, ParameterSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ParameterSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ParameterSetupPanel::ParameterSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   mVarName = name;
   mIsStringVar = false;
   mIsValueChanged = false;
   
   mParam =
      (Parameter*)theGuiInterpreter->GetConfiguredObject(mVarName.c_str());
   if (mParam->GetTypeName() == "String")
      mIsStringVar = true;
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ParameterSetupPanel::Create()
{
   int bsize = 2; // border size
   
   //-------------------------------------------------------
   // for Variable Setup
   //-------------------------------------------------------
   
   wxStaticText *nameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *emptyStaticText =
      new wxStaticText(this, ID_TEXT, wxT("  "),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *equalSignStaticText =
      new wxStaticText(this, ID_TEXT, wxT("="),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *valueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Value"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(150,-1), 0);
   mValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                                 wxDefaultPosition, wxSize(300,-1), 0);
      
   // wxSizers
   mPageBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxFlexGridSizer *top1FlexGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *detailsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   wxStaticBox *variableStaticBox;
   
   if (mIsStringVar)
      variableStaticBox = new wxStaticBox(this, -1, wxT("String"));
   else
      variableStaticBox = new wxStaticBox(this, -1, wxT("Variable"));
   
   mVarStaticBoxSizer = new wxStaticBoxSizer(variableStaticBox, wxVERTICAL);
   
   // Add to wx*Sizers
   // for Variable Setup
   // 1st row
   top1FlexGridSizer->Add(nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(emptyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   top1FlexGridSizer->Add(valueStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   // 1st row
   top1FlexGridSizer->Add(mVarNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(equalSignStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   top1FlexGridSizer->Add(mValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   mVarStaticBoxSizer->Add(top1FlexGridSizer, 0, wxALIGN_TOP|wxALL, bsize);
   mVarStaticBoxSizer->Add(detailsBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mPageBoxSizer->Add(mVarStaticBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(mPageBoxSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);

}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::LoadData()
{
   mParam =
      (Parameter*)theGuiInterpreter->GetConfiguredObject(mVarName.c_str());
   
   // Set the pointer for the "Show Script" button
   mObject = mParam;
   if (mParam == NULL)
      return;

         
   #if DEBUG_PARAM_PANEL
   MessageInterface::ShowMessage
      ("ParameterSetupPanel::LoadData() paramName=%s\n", mParam->GetName().c_str());
   #endif
   
   try
   {
      // show expression
      std::string varExp = mParam->GetStringParameter("Expression");
      mVarNameTextCtrl->SetValue(mVarName);
      mValueTextCtrl->SetValue(varExp.c_str());
            
      if (!mIsStringVar)
      {
         // if expression is just a number, enable editing
         double realVal;
         if (mValueTextCtrl->GetValue().ToDouble(&realVal))
            mValueTextCtrl->Enable();
         else
            mValueTextCtrl->Disable();
      }
      else
      {
         mValueTextCtrl->SetValue(mParam->GetStringParameter("Expression").c_str());
      }
      
      mVarNameTextCtrl->Disable();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ParameterSetupPanel::SaveData()
{
   canClose = true;
   std::string expr;
   
   //-----------------------------------------------------------------
   // check values from text field if variable
   //-----------------------------------------------------------------
   if (mIsValueChanged)
   {
      expr = mValueTextCtrl->GetValue().c_str();
      Real rval;
      if (mParam->GetTypeName() == "Variable")
         CheckReal(rval, expr, "Expression", "Real Number");
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      if (mIsValueChanged)
      {
         mIsValueChanged = false;
         mParam->SetStringParameter("Expression", expr);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ParameterSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (mValueTextCtrl->IsModified())
   {
      mIsValueChanged = true;
      EnableUpdate(true);
   }
}

