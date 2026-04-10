//$Id$
//------------------------------------------------------------------------------
//                               AchievePanel
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
// Created: 2004/10/12
//
/**
 * This class contains the setup for Target Achive command.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "ParameterSelectDialog.hpp"
#include "AchievePanel.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ACHIEVE_PANEL_LOAD 1
//#define DEBUG_ACHIEVE_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AchievePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, AchievePanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, AchievePanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, AchievePanel::OnSolverSelection)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// AchievePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
AchievePanel::AchievePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mAchieveCommand = (Achieve *)cmd;

   mIsTextModified = false;
   mSolverName = "";
   mGoalName = "";
   mGoalValue = "";
   mTolerance = "1.0e-6";
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~AchievePanel()
//------------------------------------------------------------------------------
AchievePanel::~AchievePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("BoundarySolver", mSolverComboBox);
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AchievePanel::Create()
{
   int bsize = 2; // bordersize

   // Solver name
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   mSolverComboBox =
      theGuiManager->GetBoundarySolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   // Goal
   wxStaticText *goalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Goal"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mGoalNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mViewGoalButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
   
   // Value
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Value"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mGoalValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(250,-1), 0);
   mViewGoalValueButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);

   // Tolerence
   wxStaticText *toleranceStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mToleranceTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(250,-1), 0);   
   mViewToleranceButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
   
   
   // wx*Sizers
   wxFlexGridSizer *goalGridSizer = new wxFlexGridSizer(3, 0, 0);
   
   // Add to wx*Sizers  
   goalGridSizer->Add(solverStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mSolverComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(goalStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mGoalNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewGoalButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(initialStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mGoalValueTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewGoalValueButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(toleranceStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mToleranceTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewToleranceButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // add page sizer to middle sizer
   theMiddleSizer->Add(goalGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AchievePanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL_LOAD
   MessageInterface::ShowMessage("AchievePanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mAchieveCommand->GetTypeName().c_str());
   #endif
   
   // We don't want user to edit this box
   mGoalNameTextCtrl->Disable();
      
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mAchieveCommand;
      
      std::string solverName = mAchieveCommand->
         GetStringParameter(mAchieveCommand->GetParameterID("TargeterName"));
      
      std::string goalName = mAchieveCommand->
         GetStringParameter(mAchieveCommand->GetParameterID("Goal"));
      
      std::string goalValue = mAchieveCommand->GetStringParameter
         (mAchieveCommand->GetParameterID("GoalValue"));
      
      std::string toleranceValue = mAchieveCommand->GetStringParameter
         (mAchieveCommand->GetParameterID("Tolerance"));
      
      #if DEBUG_ACHIEVE_PANEL_LOAD
      MessageInterface::ShowMessage("   solverName=%s\n", solverName.c_str());
      MessageInterface::ShowMessage("   goalName=%s\n", goalName.c_str());
      MessageInterface::ShowMessage("   goalValue=%s\n", goalValue.c_str());
      MessageInterface::ShowMessage("   tolerance=%s\n", toleranceValue.c_str());
      #endif
      
      mSolverName = wxString(solverName.c_str());      
      mGoalName = wxString(goalName.c_str());
      mGoalValue = wxString(goalValue.c_str());
      mTolerance = wxString(toleranceValue.c_str());
      
      mSolverComboBox->SetValue(mSolverName);
      mGoalNameTextCtrl->SetValue(mGoalName);
      mGoalValueTextCtrl->SetValue(mGoalValue);   
      mToleranceTextCtrl->SetValue(mTolerance);
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   #if DEBUG_ACHIEVE_PANEL_LOAD
   MessageInterface::ShowMessage("AchievePanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AchievePanel::SaveData()
{   
   #if DEBUG_ACHIEVE_PANEL_SAVE
   MessageInterface::ShowMessage
      ("AchievePanel::SaveData() entered, mIsTextModified=%d\n", mIsTextModified);
   #endif
   
   canClose = true;
   std::string inputString;
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      ObjectTypeArray objTypes;
      objTypes.push_back(Gmat::SPACE_POINT);
      objTypes.push_back(Gmat::IMPULSIVE_BURN);

      inputString = mGoalValue.c_str();
      CheckVariable(mGoalValue.c_str(), objTypes, "GoalValue",
                    "Real Number, Variable, Array element, plottable Parameter", true);
      
      inputString = mToleranceTextCtrl->GetValue();
      CheckVariable(inputString.c_str(), objTypes, "Tolerance",
                    "Real Number, Variable, Array element, plottable Parameter", true);
   }
   
   if (!canClose)
      return;
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
   try
   {
      #if DEBUG_ACHIEVE_PANEL_SAVE
      MessageInterface::ShowMessage("   Setting Solver to '%s'\n", mSolverName.c_str());
      #endif
      
      mAchieveCommand->SetStringParameter
         (mAchieveCommand->GetParameterID("TargeterName"), mSolverName.c_str());
      
      if (mIsTextModified)
      {
         #if DEBUG_ACHIEVE_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   Setting Goal to '%s', Value to '%s', Tolerance to '%s'\n",
             mGoalName.c_str(), mGoalValue.c_str(), mTolerance.c_str());
         #endif
         
         mAchieveCommand->SetStringParameter
            (mAchieveCommand->GetParameterID("Goal"), mGoalName.c_str());
         
         mAchieveCommand->SetStringParameter
            (mAchieveCommand->GetParameterID("GoalValue"), mGoalValue.c_str());
         
         mTolerance = mToleranceTextCtrl->GetValue();      
         mAchieveCommand->SetStringParameter
            (mAchieveCommand->GetParameterID("Tolerance"), mTolerance.c_str());
         
         mIsTextModified = false;
         
         #if DEBUG_ACHIEVE_PANEL_SAVE
         MessageInterface::ShowMessage("   Calling theGuiInterpreter->ValidateCommand()\n");
         #endif
         
         if (!theGuiInterpreter->ValidateCommand(mAchieveCommand))
         {
            #if DEBUG_ACHIEVE_PANEL_SAVE
            MessageInterface::ShowMessage("   Failed to validate command\n");
            #endif
            canClose = false;
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #if DEBUG_ACHIEVE_PANEL_SAVE
   MessageInterface::ShowMessage
      ("AchievePanel::SaveData() leaving, canClose=%d\n", canClose);
   #endif
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void AchievePanel::OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Sets mIsTextModified to true if text field for Real value is modified.
 */
//------------------------------------------------------------------------------
void AchievePanel::OnTextChange(wxCommandEvent& event)
{
   if (mGoalValueTextCtrl->IsModified())
   {
      mGoalValue = mGoalValueTextCtrl->GetValue();
      mIsTextModified = true;
   }
   
   if (mToleranceTextCtrl->IsModified())
   {
      mIsTextModified = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void AchievePanel::OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AchievePanel::OnSolverSelection(wxCommandEvent &event)
{
   mSolverName = mSolverComboBox->GetValue();
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void AchievePanel::OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AchievePanel::OnButtonClick(wxCommandEvent& event)
{
   // show dialog to select parameter
   ParameterSelectDialog paramDlg(this, mObjectTypeList);
   paramDlg.ShowModal();
      
   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      if (event.GetEventObject() == mViewGoalButton)
      {
         mGoalNameTextCtrl->SetValue(newParamName);
         mGoalName = newParamName;
         mIsTextModified = true;
      }
      else if (event.GetEventObject() == mViewGoalValueButton)
      {
         mGoalValueTextCtrl->SetValue(newParamName);
         mGoalValue = newParamName;
         mIsTextModified = true;
      }
      else if (event.GetEventObject() == mViewToleranceButton)
      {
         mToleranceTextCtrl->SetValue(newParamName);
         mTolerance = newParamName;
         mIsTextModified = true;
      }
      
      EnableUpdate(true);
   }
}

