//$Id$
//------------------------------------------------------------------------------
//                                SQPSetupPanel
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
// Author:
// Created:
//
/**
 * Declares SQPSetupPanel.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "SQPSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"  // for ToReal()

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SQPSetupPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBOBOX, SQPSetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, SQPSetupPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, SQPSetupPanel::OnCheckboxChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, SQPSetupPanel::OnBrowse)
END_EVENT_TABLE()

//const wxString							// made a change here
//SQPSetupPanel::DISPLAY_SCHEMES[4] =
//{
//   "Iter",
//   "Off",
//   "Notify",
//   "Final"
//};

//------------------------------------------------------------------------------
// SQPSetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
SQPSetupPanel::SQPSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   fileDialog = NULL;

   theSolver =
      (Solver*)theGuiInterpreter->GetConfiguredObject(name.c_str());

   isTextModified = true;

   if (theSolver != NULL)
   {
      Create();
      Show();
   }
   else
   {
      // show error message
   }
}


//------------------------------------------------------------------------------
// ~SQPSetupPanel()
//------------------------------------------------------------------------------
SQPSetupPanel::~SQPSetupPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void SQPSetupPanel::Create()
//------------------------------------------------------------------------------
void SQPSetupPanel::Create()
{
    Setup(this);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void SQPSetupPanel::LoadData()
{
   // load data from the core engine
   try
   {
      mObject = theSolver;

	  std::string valueStr;

// GradObj, GradConstr, DerivativeCheck, Diagnostics, and Display are no longer been used:
//      std::string valueStr = theSolver->GetStringParameter("GradObj");
//      if (valueStr == "On")
//         gradObjCB->SetValue(true);
//      else
//         gradObjCB->SetValue(false);
//
//      valueStr = theSolver->GetStringParameter("GradConstr");
//      if (valueStr == "On")
//         gradConstrCB->SetValue(true);
//      else
//         gradConstrCB->SetValue(false);
//
//      valueStr = theSolver->GetStringParameter("DerivativeCheck");
//      if (valueStr == "On")
//         derivativeCheckCB->SetValue(true);
//      else
//         derivativeCheckCB->SetValue(false);
//
//      valueStr = theSolver->GetStringParameter("Diagnostics");
//      if (valueStr == "On")
//         diagnosticsCB->SetValue(true);
//      else
//         diagnosticsCB->SetValue(false);

      valueStr = theSolver->GetStringParameter("TolFun");
      tolFunTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = theSolver->GetStringParameter("TolCon");
      tolConTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = theSolver->GetStringParameter("TolX");
      tolXTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = theSolver->GetStringParameter("MaxFunEvals");
      maxFunEvalsTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = wxString::Format("%d",theSolver->GetIntegerParameter(theSolver->GetParameterID("MaximumIterations")));		//valueStr = theSolver->GetStringParameter("MaxIter");
      maxIterTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = theSolver->GetStringParameter("DiffMinChange");
      diffMinChangeTextCtrl->SetValue(wxString(valueStr.c_str()));

      valueStr = theSolver->GetStringParameter("DiffMaxChange");
      diffMaxChangeTextCtrl->SetValue(wxString(valueStr.c_str()));

//      valueStr = theSolver->GetStringParameter("Display");
//      displayComboBox->SetValue(wxString(valueStr.c_str()));

  	  Integer id;
	  id = theSolver->GetParameterID("ShowProgress");
	  showProgressCheckBox->SetValue(theSolver->GetBooleanParameter(id));

	  id = theSolver->GetParameterID("ReportStyle");
	  styleComboBox->SetValue(theSolver->GetStringParameter(id).c_str());

	  id = theSolver->GetParameterID("ReportFile");
	  reportfileTextCtrl->SetValue(theSolver->GetStringParameter(id).c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("SQPSetupPanel:LoadData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
   }

   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void SQPSetupPanel::SaveData()
{
   canClose = true;
   Real tolFun, tolCon, tolX, diffMinChange, diffMaxChange;
   Integer maxIter, maxFunEvals;
   std::string tolFunStr, tolConStr, tolXStr, maxIterStr, maxFunEvalsStr;
   std::string diffMinChangeStr, diffMaxChangeStr;
   std::string str;

   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (isTextModified)
   {
      str = tolFunTextCtrl->GetValue();
      CheckReal(tolFun, str, "TolFun", "Real Number > 0");
      tolFunStr = str;

      str = tolConTextCtrl->GetValue();
      CheckReal(tolCon, str, "TolCon", "Real Number > 0");
      tolConStr = str;

      str = tolXTextCtrl->GetValue();
      CheckReal(tolX, str, "TolX", "Real Number > 0");
      tolXStr = str;

      str = maxIterTextCtrl->GetValue();
      CheckInteger(maxIter, str, "MaximumIterations", "Integer Number > 0");
      maxIterStr = str;

      str = maxFunEvalsTextCtrl->GetValue();
      CheckInteger(maxFunEvals, str, "MaxFunEvals", "Integer Number > 0");
      maxFunEvalsStr = str;

      str = diffMinChangeTextCtrl->GetValue();
      CheckReal(diffMinChange, str, "DiffMinChange", "Real Number > 0");
      diffMinChangeStr = str;

      str = diffMaxChangeTextCtrl->GetValue();
      CheckReal(diffMaxChange, str, "DiffMaxChange", "Real Number > 0");
      diffMaxChangeStr = str;
   }

   if (!canClose)
      return;

   try
   {

      // Note: It will catch one error at a time
      // Is this acceptable?

      if (isTextModified)
      {
         theSolver->SetStringParameter("TolFun", tolFunStr);
         theSolver->SetStringParameter("TolCon", tolConStr);
         theSolver->SetStringParameter("TolX", tolXStr);
         theSolver->SetIntegerParameter(theSolver->GetParameterID("MaximumIterations"), maxIter);
         theSolver->SetStringParameter("MaxFunEvals", maxFunEvalsStr);
         theSolver->SetStringParameter("DiffMinChange", diffMinChangeStr);
         theSolver->SetStringParameter("DiffMaxChange", diffMaxChangeStr);
         isTextModified = false;
      }

// GradObj, GradConstr, DerivativeCheck, Diagnostics, and Display parameters are no loner been used:
//      if (gradObjCB->IsChecked())
//         theSolver->SetStringParameter("GradObj", "On");
//      else
//         theSolver->SetStringParameter("GradObj", "Off");
//
//      if (gradConstrCB->IsChecked())
//         theSolver->SetStringParameter("GradConstr", "On");
//      else
//         theSolver->SetStringParameter("GradConstr", "Off");
//
//      if (derivativeCheckCB->IsChecked())
//         theSolver->SetStringParameter("DerivativeCheck", "On");
//      else
//         theSolver->SetStringParameter("DerivativeCheck", "Off");
//
//      if (diagnosticsCB->IsChecked())
//         theSolver->SetStringParameter("Diagnostics", "On");
//      else
//         theSolver->SetStringParameter("Diagnostics", "Off");
//
//      theSolver->SetStringParameter("Display", displayComboBox->GetValue().c_str());

	  Integer id;
      id = theSolver->GetParameterID("ShowProgress");
      theSolver->SetBooleanParameter(id, showProgressCheckBox->GetValue());

	  id = theSolver->GetParameterID("ReportStyle");
	  theSolver->SetStringParameter(id, styleComboBox->GetValue().c_str());

      id = theSolver->GetParameterID("ReportFile");
      theSolver->SetStringParameter(id, reportfileTextCtrl->GetValue().c_str());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void SQPSetupPanel::Setup( wxWindow *parent)
{
	wxString *styleArray = new wxString[4];
	styleArray[0] = "Normal";
	styleArray[1] = "Concise";
	styleArray[2] = "Verbose";
	styleArray[3] = "Debug";

    Integer bsize = 2;

	// 1. Create Options box:
	wxStaticBoxSizer *optionsStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Options");
	wxFlexGridSizer *grid1 = new wxFlexGridSizer( 2, 0, 0 );
	grid1->AddGrowableCol(1);

	// Max Iter
	maxIterStaticText = new wxStaticText(parent, ID_TEXT, wxT("Max. Iterations"));
	maxIterTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
	// Max Fun Evals
	maxFunEvalsStaticText = new wxStaticText(parent, ID_TEXT, wxT("Max. Func. Evals."));
	maxFunEvalsTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
   // Tol X
   tolXStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol X"));
   tolXTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
   // Tol Con
   tolConStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol Con"));
   tolConTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
   // Tol Fun
   tolFunStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol Fun"));
   tolFunTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
   // Diff Max Change
   diffMaxChangeStaticText = new wxStaticText(parent, ID_TEXT, wxT("Diff Max Change"));
   diffMaxChangeTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));
   // Diff Min Change
   diffMinChangeStaticText = new wxStaticText(parent, ID_TEXT, wxT("Diff Min Change"));
   diffMinChangeTextCtrl = new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                      wxSize(100,-1));
    grid1->Add( maxIterStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( maxIterTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( maxFunEvalsStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( maxFunEvalsTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid1->Add( tolXStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( tolXTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid1->Add( tolConStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( tolConTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid1->Add( tolFunStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( tolFunTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid1->Add( diffMaxChangeStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( diffMaxChangeTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( diffMinChangeStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid1->Add( diffMinChangeTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );

	optionsStaticBoxSizer->Add( grid1, 0, wxALIGN_LEFT|wxALL, bsize );


   	// 2. Create Output Box:
	wxStaticBoxSizer *outputStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Output");
	wxFlexGridSizer *grid2 = new wxFlexGridSizer( 3, 0, 0 );
	grid2->AddGrowableCol(1);

    showProgressCheckBox = new wxCheckBox( parent, ID_CHECKBOX, wxT("Show Progress"),wxDefaultPosition, wxDefaultSize, 0 );
    reportStyleStaticText = new wxStaticText( parent, ID_TEXT, wxT("Report Style"), wxDefaultPosition,wxDefaultSize, 0);
    styleComboBox = new wxComboBox( parent, ID_COMBOBOX, wxT("Normal"), wxDefaultPosition,
         wxSize(200,-1), 4, styleArray, wxCB_DROPDOWN|wxCB_READONLY );
    reportfileStaticText = new wxStaticText( parent, ID_TEXT, wxT("Report File"), wxDefaultPosition,wxDefaultSize, 0);
    reportfileTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );

	browseButton = new wxButton(parent, ID_BUTTON_BROWSE, wxT("Browse"));

	grid2->Add( showProgressCheckBox, 0, wxALIGN_LEFT|wxALL, bsize );
    grid2->Add( 0, 0, wxALIGN_CENTRE|wxALL, bsize);
	grid2->Add( 0, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid2->Add( reportStyleStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid2->Add( styleComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
	grid2->Add( 0, 0, wxALIGN_CENTRE|wxALL, bsize);
    grid2->Add( reportfileStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
    grid2->Add( reportfileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
	grid2->Add( browseButton, 0, wxALIGN_LEFT|wxALL, bsize);

	outputStaticBoxSizer->Add( grid2, 0, wxALIGN_LEFT|wxALL, bsize );


	// 3. Add Options box and Output box to theMiddleSizer:
	theMiddleSizer->Add(optionsStaticBoxSizer, 0, wxGROW, bsize);
	theMiddleSizer->Add(outputStaticBoxSizer, 0, wxGROW, bsize);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnTextChange(wxCommandEvent& event)
{
   isTextModified = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckboxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnCheckboxChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnBrowse()
// This function is used to open file dialog.
//------------------------------------------------------------------------------
void SQPSetupPanel::OnBrowse(wxCommandEvent &event)
{
	if (fileDialog == NULL)
	{
		// create fileDialog object when it does not exist
		wxString caption = wxT("Choose a File");
		wxString defaultDir = wxT("C:/DevelGMAT/scripts/");
		wxString defaultFile = wxEmptyString;
		//wxString wildcard = wxT("*.script");
		wxString wildcard = wxT("Data files (*.data)|*.data|All files (*.*)|*.*");
		fileDialog = new wxFileDialog(this, caption,
                                    defaultDir, defaultFile,
                                    wildcard,
                                    gmatFD_OPEN);
	}
	else
	{
		// show fileDialog when it exists
		fileDialog->Show();
	}

	if (fileDialog->ShowModal() == wxID_OK)
	{
		// change reportFile when a new file is chosen
		wxString filename = fileDialog->GetPath(); // To get full path to file
		reportfileTextCtrl->SetValue(filename);
		isTextModified = true;
	}
	else
	{
		// only hide fileDialog when clik on Cancel button
		fileDialog->Hide();
	}

}


