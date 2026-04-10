//$Id$
//------------------------------------------------------------------------------
//                              NonlinearConstraintPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2004/9/15
/**
 * This implements setting up NonlinearConstraint command.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "NonlinearConstraintPanel.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"  // for ToDouble()

//#define DEBUG_NLC_PANEL 1


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NonlinearConstraintPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, NonlinearConstraintPanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, NonlinearConstraintPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, NonlinearConstraintPanel::OnSolverSelection)
END_EVENT_TABLE()


//------------------------------------------------------------------------------
// NonlinearConstraintPanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
NonlinearConstraintPanel::NonlinearConstraintPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mNonlinearConstraintCommand = (NonlinearConstraint *)cmd;      
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");

   theGuiManager = GuiItemManager::GetInstance();
   
   Create();
   Show();
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// ~NonlinearConstraintPanel()
//------------------------------------------------------------------------------
NonlinearConstraintPanel::~NonlinearConstraintPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::Create()
{
   int bsize = 2; // bordersize

   // Optimizer
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Optimizer Name"),
                       wxDefaultPosition, wxSize(70, -1), 0);
   mSolverComboBox = theGuiManager->GetOptimizerComboBox(this, ID_COMBO,
                        wxSize(120,-1));

   // Constraint Variable
   wxStaticText *lhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Constraint Variable Name"), 
                       wxDefaultPosition, wxSize(80, -1), 0);
   mLHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(120,-1), 0);

   // Left choose button
   mLeftChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, 
      wxSize(50,-1), 0);
      
   // Comparison
   wxStaticText *blankStaticText =
      new wxStaticText(this, ID_TEXT, wxT(""), 
                       wxDefaultPosition, wxSize(60, -1), 0);
   wxString comparisons[] = { wxT("<="), wxT(">="), wxT("=") };
   mComparisonComboBox =
         new wxComboBox(this, ID_COMBO, wxString(comparisons[0]), wxDefaultPosition,
                        wxSize(45,-1), 3, comparisons,
                        wxCB_DROPDOWN|wxCB_READONLY);

   // Constraint Value
   wxStaticText *rhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Constraint Value"), 
                       wxDefaultPosition, wxSize(80, -1), 0);
   mRHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(120,-1), 0);

   // Right choose button
   mRightChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, 
      wxSize(50,-1), 0);

   // Tolerence
//   wxStaticText *toleranceStaticText =
//      new wxStaticText(this, ID_TEXT, wxT("Tolerance"), 
//                       wxDefaultPosition, wxSize(60, -1), 0);
//   mTolTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
//                                     wxDefaultPosition, wxSize(40,-1), 0);
   
   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *solverSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *rhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *rhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *conditionSizer = new wxBoxSizer(wxVERTICAL);
   //wxBoxSizer *toleranceSizer = new wxBoxSizer(wxVERTICAL);
   
   solverSizer->Add(solverStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   solverSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   lhsInterSizer->Add(mLHSTextCtrl, 0, wxALIGN_CENTER|wxALL, 0);
   lhsInterSizer->Add(mLeftChooseButton, 0, wxALIGN_CENTER|wxALL, 0);
   
   lhsSizer->Add(lhsStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   lhsSizer->Add(lhsInterSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   rhsInterSizer->Add(mRHSTextCtrl, 0, wxALIGN_CENTER|wxALL, 0);
   rhsInterSizer->Add(mRightChooseButton, 0, wxALIGN_CENTER|wxALL, 0);
   
   rhsSizer->Add(rhsStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   rhsSizer->Add(rhsInterSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   conditionSizer->Add(blankStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   conditionSizer->Add(mComparisonComboBox, 0, wxALIGN_CENTER|wxALL, bsize);

   //toleranceSizer->Add(toleranceStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   //toleranceSizer->Add(mTolTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   panelSizer->Add(solverSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(lhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(conditionSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(rhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   //panelSizer->Add(toleranceSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("NonlinearConstraintPanel::LoadData() entered\n");
   MessageInterface::ShowMessage
      ("Command=%s\n", mNonlinearConstraintCommand->GetTypeName().c_str());
   #endif
   
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mNonlinearConstraintCommand;
      
      std::string loadedSolverName = mNonlinearConstraintCommand->
         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("OptimizerName"));
      
      if (loadedSolverName == "")
         mSolverComboBox->SetSelection(0);
      else
         mSolverComboBox->SetStringSelection(wxString(loadedSolverName.c_str()));

      std::string loadedVariableName = mNonlinearConstraintCommand->
         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("ConstraintArg1"));

      mLHSTextCtrl->SetValue(wxString(loadedVariableName.c_str()));

      std::string operatorStr = mNonlinearConstraintCommand->
         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("Operator"));
      
      mComparisonComboBox->SetStringSelection(wxString(operatorStr.c_str()));

      std::string loadedValue = mNonlinearConstraintCommand->
         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("ConstraintArg2"));

      mRHSTextCtrl->SetValue(wxString(loadedValue.c_str()));
      
      //Real tolValue = mNonlinearConstraintCommand->
       //  GetRealParameter(mNonlinearConstraintCommand->GetParameterID("Tolerance"));

      //mTolTextCtrl->SetValue(theGuiManager->ToWxString(tolValue));
     
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::SaveData()
{   
   #if DEBUG_NLC_PANEL
   MessageInterface::ShowMessage("NonlinearConstraintPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string inputString;
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   ObjectTypeArray objTypes;
   objTypes.push_back(Gmat::SPACE_POINT);
   objTypes.push_back(Gmat::IMPULSIVE_BURN);

   inputString = mLHSTextCtrl->GetValue().c_str();
   CheckVariable(inputString, objTypes, "Constraint",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   inputString = mRHSTextCtrl->GetValue().c_str();
   CheckVariable(inputString, objTypes, "Constraint Value",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      mNonlinearConstraintCommand->SetStringParameter
         (mNonlinearConstraintCommand->GetParameterID("OptimizerName"),
          mSolverComboBox->GetValue().c_str());
      
      mNonlinearConstraintCommand->SetStringParameter
         (mNonlinearConstraintCommand->GetParameterID("ConstraintArg1"),
          mLHSTextCtrl->GetValue().c_str());
      
      mNonlinearConstraintCommand->SetStringParameter
         (mNonlinearConstraintCommand->GetParameterID("Operator"),
          mComparisonComboBox->GetValue().c_str());
      
      #if DEBUG_NLC_PANEL
      MessageInterface::ShowMessage("   about to save ConstraintArg2 value\n");
      #endif
      mNonlinearConstraintCommand->SetStringParameter
         (mNonlinearConstraintCommand->GetParameterID("ConstraintArg2"),
          mRHSTextCtrl->GetValue().c_str());
      #if DEBUG_NLC_PANEL
      MessageInterface::ShowMessage("   finished saving ConstraintArg2 value\n");
      #endif
      
      theGuiInterpreter->ValidateCommand(mNonlinearConstraintCommand);
      EnableUpdate(false);
   
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #if DEBUG_NLC_PANEL
   MessageInterface::ShowMessage("NonlinearConstraintPanel::SaveData() exited\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::OnTextChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::OnSolverSelection(wxCommandEvent &event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void NonlinearConstraintPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mLeftChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mLeftChooseButton)
         {
            mLHSTextCtrl->SetValue(newParamName);
         }
         
         EnableUpdate(true);
      }
   }
   else if (event.GetEventObject() == mRightChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mRightChooseButton)
         {
            mRHSTextCtrl->SetValue(newParamName);
         }
         
         EnableUpdate(true);
      }
   }
   else
   {
      event.Skip();
   }
}

