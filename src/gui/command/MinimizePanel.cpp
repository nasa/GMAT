//$Id$
//------------------------------------------------------------------------------
//                              MinimizePanel
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
// Created: 2006/09/20
/**
 * This class contains the Minimize command setup window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "MinimizePanel.hpp"
#include "Array.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MINIMIZE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MinimizePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, MinimizePanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, MinimizePanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, MinimizePanel::OnSolverSelection)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MinimizePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
MinimizePanel::MinimizePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mMinimizeCommand = (Minimize *)cmd;
   theGuiManager = GuiItemManager::GetInstance();
   
   solverName = "";
   minParam = NULL;
   mVarNameChanged = false;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~MinimizePanel()
//------------------------------------------------------------------------------
MinimizePanel::~MinimizePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void MinimizePanel::Create()
{
   int bsize = 2; // bordersize

   // Optimizer
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Optimizer"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mSolverComboBox = theGuiManager->GetOptimizerComboBox(this, ID_COMBO,
                        wxSize(120,-1));
   
   // Variable to be Minimized
   wxStaticText *variableStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable to be Minimized"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mVariableTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   
   // Choose button
   mChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
 
   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *solverSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *variableSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *variableInterfaceSizer = new wxBoxSizer(wxHORIZONTAL);
   
   solverSizer->Add(solverStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   solverSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   variableInterfaceSizer->Add(mVariableTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   variableInterfaceSizer->Add(mChooseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   variableSizer->Add(variableStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   variableSizer->Add(variableInterfaceSizer, 0, wxALIGN_CENTER|wxALL, bsize);

   panelSizer->Add(solverSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(variableSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void MinimizePanel::LoadData()
{
   #if DEBUG_MINIMIZE_PANEL
   MessageInterface::ShowMessage("MinimizePanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mMinimizeCommand->GetTypeName().c_str());
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mMinimizeCommand;
      
      std::string loadedSolverName = mMinimizeCommand->
         GetStringParameter(mMinimizeCommand->GetParameterID("OptimizerName"));

      std::string loadedVariableName = mMinimizeCommand->
         GetStringParameter(mMinimizeCommand->GetParameterID("ObjectiveName"));

      #if DEBUG_MINIMIZE_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", loadedSolverName.c_str());
      MessageInterface::ShowMessage("variable=%s\n", loadedVariableName.c_str());
      #endif
      
      solverName = wxString(loadedSolverName.c_str());      
      variableName = wxString(loadedVariableName.c_str());
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   ShowGoalSetup();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void MinimizePanel::SaveData()
{   
   #if DEBUG_MINIMIZE_PANEL
   MessageInterface::ShowMessage("MinimizePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check input value - Variable, Array element, Spacecraft Parameter
   //-----------------------------------------------------------------
   
   if (mVarNameChanged)
   {
      std::string varName = variableName.WX_TO_STD_STRING;
      
      ObjectTypeArray objTypes;
      objTypes.push_back(Gmat::SPACE_POINT);
      objTypes.push_back(Gmat::IMPULSIVE_BURN);

	  canClose = CheckVariable(varName, objTypes,
	        "Variable to be Minimized",
	        "Variable, Array element, Spacecraft parameter", false);
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      mMinimizeCommand->SetStringParameter
         (mMinimizeCommand->GetParameterID("OptimizerName"),
          std::string(solverName.c_str()));
      
      mMinimizeCommand->SetStringParameter
         (mMinimizeCommand->GetParameterID("ObjectiveName"),
          std::string(variableName.c_str()));
          
      theGuiInterpreter->ValidateCommand(mMinimizeCommand);
      mVarNameChanged = !canClose;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void ShowGoalSetup()
//------------------------------------------------------------------------------
void MinimizePanel::ShowGoalSetup()
{
   wxString str;
   
   if (solverName == "")
   {
      mSolverComboBox->SetSelection(0);
      solverName = mSolverComboBox->GetStringSelection();
   }
   else
      mSolverComboBox->SetStringSelection(solverName);
      
   mVariableTextCtrl->SetValue(variableName);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MinimizePanel::OnTextChange(wxCommandEvent& event)
{
   if (mVariableTextCtrl->IsModified())
   {
      variableName = mVariableTextCtrl->GetValue();
      mVarNameChanged = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MinimizePanel::OnSolverSelection(wxCommandEvent &event)
{
   solverName = mSolverComboBox->GetStringSelection();
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MinimizePanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mChooseButton)
         {
            mVariableTextCtrl->SetValue(newParamName);
            variableName = newParamName;
         }
         
         EnableUpdate(true);
      }
   }
}

