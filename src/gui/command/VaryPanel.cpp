//$Id$
//------------------------------------------------------------------------------
//                                   VaryPanel
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
 * This class contains the setup for Target Vary command.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "VaryPanel.hpp"
#include "Solver.hpp"
#include "GmatBaseException.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "CommandUtil.hpp"         // for GetParentCommand()

//#define DEBUG_VARYPANEL_LOAD
//#define DEBUG_VARYPANEL_SAVE
//#define DEBUG_VARYPANEL_SOLVER
//#define DEBUG_SET_CONTROL

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(VaryPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, VaryPanel::OnButton)
   EVT_TEXT(ID_TEXTCTRL, VaryPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, VaryPanel::OnSolverSelection)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// VaryPanel(wxWindow *parent, GmatCommand *cmd, bool inOptimize)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
VaryPanel::VaryPanel(wxWindow *parent, GmatCommand *cmd, bool inOptimize)
   : GmatPanel(parent)
{
   #ifdef DEBUG_VARYPANEL
   MessageInterface::ShowMessage
      ("VaryPanel::VaryPanel() entered, cmd=<%p><%s>, inOptimize=%d\n",
       cmd, cmd ? cmd->GetTypeName().c_str() : "NULL", inOptimize);
   #endif
   
   mVaryCommand = (Vary *)cmd;
   inOptimizeCmd = inOptimize;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   solverChanged = false;
   variableChanged = false;
   EnableUpdate(false);
   
   #ifdef DEBUG_VARYPANEL
   MessageInterface::ShowMessage("VaryPanel::VaryPanel() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// ~VaryPanel()
//------------------------------------------------------------------------------
VaryPanel::~VaryPanel()
{
   mObjectTypeList.Clear();
   if (inOptimizeCmd)
      theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
   else
      theGuiManager->UnregisterComboBox("BoundarySolver", mSolverComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void VaryPanel::Create()
{
   int bsize = 2; // bordersize
   
   // Targeter
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   // Show all user-defined Solvers
   if (inOptimizeCmd)
      mSolverComboBox =
         theGuiManager->GetOptimizerComboBox(this, ID_COMBO, wxSize(180,-1));
   else
      mSolverComboBox =
         theGuiManager->GetBoundarySolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   // Variable
   wxStaticText *varStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable"), 
                       wxDefaultPosition, wxSize(55, -1), 0);
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mViewVarButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
   
   // Initial Value
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial Value"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mInitialTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(100,-1), 0);
   
   // Perturbation
   pertStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Perturbation"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mPertTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                  wxDefaultPosition, wxSize(100,-1), 0);
   
   // Lower
   lowerValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Lower"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mLowerValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Upper
   upperValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Upper"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mUpperValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Max Step
   maxStepStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Max Step"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMaxStepTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(100,-1), 0);

   // Additive Scale Factor
   additiveStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Additive Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mAdditiveTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Multiplicative Scale Factor
   multiplicativeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Multiplicative Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMultiplicativeTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);
   
   // wx*Sizers
   wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   GmatStaticBoxSizer *varSetupSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Variable Setup");
   wxFlexGridSizer *valueGridSizer = new wxFlexGridSizer(6, 0, 0);
   wxBoxSizer *solverBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *variableBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer *scaleGridSizer = new wxFlexGridSizer(2, 0, 0);
   
   // Add to wx*Sizers
   
   solverBoxSizer->Add(solverStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   solverBoxSizer->Add(mSolverComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   variableBoxSizer->Add(varStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   variableBoxSizer->Add(mVarNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   variableBoxSizer->Add(mViewVarButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   valueGridSizer->Add(initialStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(pertStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(lowerValueStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(upperValueStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(maxStepStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mInitialTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mPertTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mLowerValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mUpperValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mMaxStepTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   scaleGridSizer->Add(additiveStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(mAdditiveTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(multiplicativeStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(mMultiplicativeTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   varSetupSizer->Add(variableBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   varSetupSizer->Add(valueGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   panelSizer->Add(solverBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(varSetupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(scaleGridSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void VaryPanel::LoadData()
{
   #ifdef DEBUG_VARYPANEL_LOAD
   MessageInterface::ShowMessage("VaryPanel::LoadData() entered\n");
   MessageInterface::ShowMessage
      ("   Command=<%p>'%s'\n", mVaryCommand, mVaryCommand ?
       mVaryCommand->GetTypeName().c_str() : "NULL");
   #endif
   
   mVarNameTextCtrl->Disable(); // we don't want user to edit this box
   mViewVarButton->Enable();
   
   if (mVaryCommand == NULL)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, "The Vary command is NULL\n");
      return;
   }
   
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mVaryCommand;
      
      solverName =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("SolverName"));
      
      #ifdef DEBUG_VARYPANEL_LOAD
      MessageInterface::ShowMessage("   solverName=%s\n", solverName.c_str());
      #endif
      
      variableName =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Variable"));
      
      #ifdef DEBUG_VARYPANEL_LOAD
      MessageInterface::ShowMessage("   variableName=%s\n", variableName.c_str());
      #endif
      
      wxString initValStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("InitialValue")).c_str();
      wxString pertStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Perturbation")).c_str();
      wxString lowerStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Lower")).c_str();
      wxString upperStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Upper")).c_str();      
      wxString maxStepStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("MaxStep")).c_str();
      wxString addSfStr = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("AdditiveScaleFactor")).c_str();
      wxString multCfStr =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("MultiplicativeScaleFactor")).c_str();
      
      mSolverComboBox->SetStringSelection(solverName.c_str());
      mVarNameTextCtrl->SetValue(variableName.c_str());
      
      mInitialTextCtrl->SetValue(initValStr);
      mPertTextCtrl->SetValue(pertStr);
      mLowerValueTextCtrl->SetValue(lowerStr);
      mUpperValueTextCtrl->SetValue(upperStr);
      mMaxStepTextCtrl->SetValue(maxStepStr);
      mAdditiveTextCtrl->SetValue(addSfStr);
      mMultiplicativeTextCtrl->SetValue(multCfStr);
      
      //  Enalbe or disable fields depends on the solver type
      GmatBase *solver = theGuiInterpreter->GetConfiguredObject(solverName);
      if (solver != NULL)
      {
//         mVaryCommand->SetRefObject(solver, Gmat::SOLVER, solverName);
         solver->SetStringParameter
            (solver->GetParameterID("Variables"), variableName);
         SetControlEnabling(solver);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_VARYPANEL_LOAD
   MessageInterface::ShowMessage("VaryPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void VaryPanel::SaveData()
{   
   #ifdef DEBUG_VARYPANEL_SAVE
   MessageInterface::ShowMessage("VaryPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string strInitVal, strPert, strLower, strUpper, strMaxStep;
   bool checkVariableRange = false;
   bool changed = false;
   std::string strAddSf, strMultSf;
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   
   std::string expRange = "Real Number, Variable, Array element, Plottable Parameter";
   ObjectTypeArray objTypes;
   objTypes.push_back(Gmat::UNKNOWN_OBJECT);
   
   // Any plottable Parameters allowed, so use UNKNOWN_OBJECT
   if (mInitialTextCtrl->IsModified())
   {
      strInitVal = mInitialTextCtrl->GetValue().c_str();
      CheckVariable(strInitVal, objTypes,
                    "InitialValue", expRange, true);
      checkVariableRange = true;
   }
   
   if (mPertTextCtrl->IsModified())
   {
      strPert = mPertTextCtrl->GetValue().c_str();
      CheckVariable(strPert, objTypes,
                    "Perturbation", expRange, true);
   }
   
   if (mLowerValueTextCtrl->IsModified())
   {
      strLower = mLowerValueTextCtrl->GetValue().c_str();
      CheckVariable(strLower, objTypes,
                    "Lower", expRange, true);
      checkVariableRange = true;
   }
   
   if (mUpperValueTextCtrl->IsModified())
   {
      strUpper = mUpperValueTextCtrl->GetValue().c_str();
      CheckVariable(strUpper.c_str(), objTypes,
                    "Upper", expRange, true);
      checkVariableRange = true;
   }
   
   if (mMaxStepTextCtrl->IsModified())
   {
      strMaxStep = mMaxStepTextCtrl->GetValue().c_str();
      CheckVariable(strMaxStep.c_str(), objTypes,
                    "MaxStep", expRange, true);
   }
   
   if (mAdditiveTextCtrl->IsModified())
   {
      strAddSf = mAdditiveTextCtrl->GetValue().c_str();
      CheckVariable(strAddSf.c_str(), objTypes,
                    "AdditiveScaleFactor", expRange, true);
   }
   
   if (mMultiplicativeTextCtrl->IsModified())
   {
      strMultSf = mMultiplicativeTextCtrl->GetValue().c_str();
      CheckVariable(strMultSf.c_str(), objTypes,
                    "MultiplicativeScaleFactor", expRange, true);
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   
   #ifdef DEBUG_VARYPANEL_SAVE
   MessageInterface::ShowMessage("   solverName=%s, variableName=%s\n",
       solverName.c_str(), variableName.c_str());
   #endif
   
   Solver *solver = (Solver*)theGuiInterpreter->GetConfiguredObject(solverName);
   
   if (solver == NULL)
      throw GmatBaseException("Cannot find the solver: " + solverName);
   
   bool validateCommand = false;
   
   // GMT-3867  Check that numerical entries satisfy range settings
   if (checkVariableRange)
   {
      Real initial, lower, upper;
      bool initReal, lowerReal, upperReal, badValue = false;
      // At least one range field changes, so
      changed = true;

      std::string panelInitial = mInitialTextCtrl->GetValue().ToStdString();
      std::string panelLower   = mLowerValueTextCtrl->GetValue().ToStdString();
      std::string panelUpper   = mUpperValueTextCtrl->GetValue().ToStdString();

      if (GmatStringUtil::IsNumber(panelInitial))
      {
         initReal = true;
         std::stringstream temp;
         temp << panelInitial;
         temp >> initial;
      }

      if (GmatStringUtil::IsNumber(panelLower))
      {
         lowerReal = true;
         std::stringstream temp;
         temp << panelLower;
         temp >> lower;
      }

      if (GmatStringUtil::IsNumber(panelUpper))
      {
         upperReal = true;
         std::stringstream temp;
         temp << panelUpper;
         temp >> upper;
      }

      // Trap issues in ranges for numbers on the panel
      if (lowerReal && upperReal)
         if (lower > upper)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_,
                  "Lower bound (%lf) is greater than upper bound (%lf).  Please adjust "
                  "the bounds to a valid range.\n", lower, upper);
            canClose = false;
            badValue = true;
         }
      if (initReal && lowerReal)
         if (initial < lower)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, "The initial value (%lf) is "
                  "lower than lower bound (%lf).  Please adjust "
                  "the initial value accordingly.\n", initial, lower);
            canClose = false;
            badValue = true;
         }

      if (initReal && upperReal)
         if (initial > upper)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_,"The initial value (%lf) is "
                  "greater than upper bound (%lf).  Please adjust the initial "
                  "value accordingly.\n", initial, upper);
            canClose = false;
            badValue = true;
         }

      // Exit here so the user can fix things
      if (badValue)
         return;
   }

   try
   {
      if (solverChanged)
      {
         #ifdef DEBUG_VARYPANEL_SAVE
         MessageInterface::ShowMessage
            ("   Solver changed, solver=<%p>'%s'\n", solver, solver->GetName().c_str());
         #endif
         mVaryCommand->SetStringParameter("SolverName", solverName);
         mVaryCommand->SetRefObject(solver, Gmat::SOLVER, solverName);
         solverChanged = false;
         changed = true;
      }
      
      if (variableChanged)
      {
         #ifdef DEBUG_VARYPANEL_SAVE
         MessageInterface::ShowMessage
            ("   Variable changed, variableName='%s'\n", variableName.c_str());
         #endif
         validateCommand = true;
         mVaryCommand->SetStringParameter("Variable", variableName);
         solver->SetStringParameter("Variables", variableName);
         variableChanged = false;
         changed = true;
      }
      
      if (mInitialTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("InitialValue", strInitVal.c_str());
         mInitialTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mPertTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("Perturbation", strPert.c_str());
         mPertTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mLowerValueTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("Lower", strLower.c_str());
         mLowerValueTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mUpperValueTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("Upper", strUpper.c_str());
         mUpperValueTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mMaxStepTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("MaxStep", strMaxStep.c_str());
         mMaxStepTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mAdditiveTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("AdditiveScaleFactor", strAddSf.c_str());
         mAdditiveTextCtrl->DiscardEdits();
         changed = true;
      }
      
      if (mMultiplicativeTextCtrl->IsModified())
      {
         validateCommand = true;
         mVaryCommand->SetStringParameter("MultiplicativeScaleFactor", strMultSf.c_str());
         mMultiplicativeTextCtrl->DiscardEdits();
         changed = true;
      }

      if (changed)
         mVaryCommand->SetRefObject(solver, Gmat::SOLVER, solverName);

      // avoid unnecessary validation since it clears all wrappers and recreates them
      if (validateCommand)
      {
         if (!theGuiInterpreter->ValidateCommand(mVaryCommand))
            canClose = false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_VARYPANEL_SAVE
   MessageInterface::ShowMessage("VaryPanel::SaveData() leaving\n");
   #endif
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void VaryPanel::OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void VaryPanel::OnTextChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void VaryPanel::OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void VaryPanel::OnSolverSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_VARYPANEL_SOLVER
   MessageInterface::ShowMessage("VaryPanel::OnSolverSelection() entered\n");
   #endif
   
   solverName = mSolverComboBox->GetStringSelection().c_str();
   
   GmatBase *slvr = theGuiInterpreter->GetConfiguredObject(solverName);
   
   #ifdef DEBUG_VARYPANEL_SOLVER
   MessageInterface::ShowMessage
      ("   solverName='%s', solver=<%p>'%s'\n", solverName.c_str(), slvr,
       slvr ? slvr->GetName().c_str() : "NULL");
   #endif
   
   if (slvr == NULL)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, "The solver " + solverName + " is NULL");
   }
   else
   {
      solverChanged = true;
      SetControlEnabling(slvr);
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void SetControlEnabling(GmatBase *slvr)
//------------------------------------------------------------------------------
/**
 * Enables and disables variable controls based on what the solver supports.
 * 
 * @param slvr The Solver that the Vary command configured from this panel uses.
 */
//------------------------------------------------------------------------------
void VaryPanel::SetControlEnabling(GmatBase *slvr)
{
   #ifdef DEBUG_SET_CONTROL
   MessageInterface::ShowMessage
      ("VaryPanel::SetControlEnabling() entered, solver=<%p><%s>'%s'\n", slvr,
       slvr->GetTypeName().c_str(), slvr->GetName().c_str());
   #endif
   if (slvr->GetBooleanParameter(slvr->GetParameterID("AllowScaleSetting")))
   {
      additiveStaticText->Enable(true);
      multiplicativeStaticText->Enable(true);
      mAdditiveTextCtrl->Enable(true);
      mMultiplicativeTextCtrl->Enable(true);
   }
   else
   {
      additiveStaticText->Enable(false);
      multiplicativeStaticText->Enable(false);
      mAdditiveTextCtrl->Enable(false);
      mMultiplicativeTextCtrl->Enable(false);
   }
   
   if (slvr->GetBooleanParameter(slvr->GetParameterID("AllowRangeSettings")))
   {
      lowerValueStaticText->Enable(true);
      mLowerValueTextCtrl->Enable(true);
      upperValueStaticText->Enable(true);
      mUpperValueTextCtrl->Enable(true);
   }
   else // in target
   {
      lowerValueStaticText->Enable(false);
      mLowerValueTextCtrl->Enable(false);
      upperValueStaticText->Enable(false);
      mUpperValueTextCtrl->Enable(false);
   }      
   
   if (slvr->GetBooleanParameter(slvr->GetParameterID("AllowStepsizeSetting")))
   {
      maxStepStaticText->Enable(true);
      mMaxStepTextCtrl->Enable(true);
   }
   else
   {
      maxStepStaticText->Enable(false);
      mMaxStepTextCtrl->Enable(false);
   }
   
   if (slvr->GetBooleanParameter(slvr->GetParameterID("AllowVariablePertSetting")))
   {
      pertStaticText->Enable(true);
      mPertTextCtrl->Enable(true);
   }
   else
   {
      pertStaticText->Enable(false);
      mPertTextCtrl->Enable(false);
   }
   #ifdef DEBUG_SET_CONTROL
   MessageInterface::ShowMessage
      ("VaryPanel::SetControlEnabling() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void VaryPanel::OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void VaryPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewVarButton)  
   {
      wxString objType = "ImpulsiveBurn";
      if (theGuiManager->GetNumImpulsiveBurn() == 0)
          objType = "Spacecraft";
      
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE, 0, false,
                                     false, true, true, true, true, objType,
                                     true, true, true);
      
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         mVarNameTextCtrl->SetValue(newParamName);
         variableName = newParamName.c_str();
         variableChanged = true;
         EnableUpdate(true);
      }
   }
   else
      event.Skip();
}

