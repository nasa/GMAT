//$Id$
//------------------------------------------------------------------------------
//                           SolverCreatePanel
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
// Author: Waka Waktola
// Created: 2004/01/21
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "DCSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"
#include "DifferentialCorrector.hpp"
#include "StringUtil.hpp"  // for ToInteger()
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DCSetupPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, DCSetupPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBOBOX, DCSetupPanel::OnComboBoxChange)
    EVT_CHECKBOX(ID_CHECKBOX, DCSetupPanel::OnCheckBoxChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, DCSetupPanel::OnBrowse)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DCSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
DCSetupPanel::DCSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   fileDialog = NULL;

   theSolver =
      (Solver*)theGuiInterpreter->GetConfiguredObject(std::string(name.c_str()));
   
   theDC = (DifferentialCorrector *)theSolver;
   isTextModified = false;
   
   if (theDC != NULL)
   {
      Create();
      LoadData();        // made a change
      Show();
   }
}

//------------------------------------------------------------------------------
// void ~DCSetupPanel()
//------------------------------------------------------------------------------
DCSetupPanel::~DCSetupPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void DCSetupPanel::Create()
{        
    Setup(this);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void DCSetupPanel::LoadData()
{
   mObject = theDC;
    
   Integer id;
         
   id = theDC->GetParameterID("MaximumIterations");
   maxTextCtrl->SetValue(wxVariant((long)theDC->GetIntegerParameter(id)));
  
   id = theDC->GetParameterID("ReportStyle");
   styleComboBox->SetValue(theDC->GetStringParameter(id).c_str());
   // reportStyle = theDC->GetStringParameter(id).c_str();
   // styleComboBox->SetValue(reportStyle.c_str());
   
   id = theDC->GetParameterID("ReportFile");
   //id = theDC->GetParameterID("TargeterTextFile");
   reportfileTextCtrl->SetValue(theDC->GetStringParameter(id).c_str());
   // textfileTextCtrl->SetValue(theDC->GetStringParameter(id).c_str());
   
   id = theDC->GetParameterID("ShowProgress");
   showProgressCheckBox->SetValue(theDC->GetBooleanParameter(id));
   
   id = theDC->GetParameterID("Algorithm");
   algorithmComboBox->SetValue(theDC->GetStringParameter(id).c_str());

   id = theDC->GetParameterID("DerivativeMethod");
   // id = theDC->GetParameterID("UseCentralDifferences");
   derivativeMethodComboBox->SetValue(theDC->GetStringParameter(id).c_str());
   // centralDifferencesCheckBox->SetValue(theDC->GetBooleanParameter(id));
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void DCSetupPanel::SaveData()
{   
   canClose = true;
   std::string str;
   Integer maxIter;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (isTextModified)
   {
      str = maxTextCtrl->GetValue();
      CheckInteger(maxIter, str, "Maximum Iterations", "Integer Number > 0");
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;

      if (isTextModified)
      {
         // save maximum iterations
         id = theDC->GetParameterID("MaximumIterations");      
         theDC->SetIntegerParameter(id, maxIter);
         isTextModified = false;
      }
      
      id = theDC->GetParameterID("ReportStyle");
      theDC->SetStringParameter(id, styleComboBox->GetValue().c_str());
      // theDC->SetStringParameter(id, reportStyle.c_str());
      
      id = theDC->GetParameterID("ReportFile");
      // id = theDC->GetParameterID("TargeterTextFile");
      theDC->SetStringParameter(id, reportfileTextCtrl->GetValue().c_str());
      // theDC->SetStringParameter(id, textfileTextCtrl->GetValue().c_str());
      
      id = theDC->GetParameterID("ShowProgress");
      theDC->SetBooleanParameter(id, showProgressCheckBox->GetValue());

      id = theDC->GetParameterID("Algorithm");
      theDC->SetStringParameter(id, algorithmComboBox->GetValue().c_str());
    
      id = theDC->GetParameterID("DerivativeMethod");
      // id = theDC->GetParameterID("UseCentralDifferences");
      theDC->SetStringParameter(id, derivativeMethodComboBox->GetValue().c_str());
      // theDC->SetBooleanParameter(id, centralDifferencesCheckBox->GetValue());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}

//------------------------------------------------------------------------------
// void Setup()
//------------------------------------------------------------------------------
void DCSetupPanel::Setup( wxWindow *parent)
{   
   wxString *styleArray = new wxString[4];
   styleArray[0] = "Normal";
   styleArray[1] = "Concise";
   styleArray[2] = "Verbose";
   styleArray[3] = "Debug";
   
   wxString *derivativeMethodArray = new wxString[3];
   derivativeMethodArray[0] = "CentralDifference";
   derivativeMethodArray[1] = "ForwardDifference";
   derivativeMethodArray[2] = "BackwardDifference";
   
   wxString *algorithmArray = new wxString[3];
   algorithmArray[0] = "NewtonRaphson";
   algorithmArray[1] = "Broyden";
   algorithmArray[2] = "ModifiedBroyden";
   

    wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
    #if __WXMAC__
    int buttonWidth = 40;
    #else
    int buttonWidth = 25;
    #endif

    Integer bsize = 2;                      // box size
   // 1. Create Options Box:
   wxStaticBoxSizer *optionsStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Options");
   wxFlexGridSizer *grid1 = new wxFlexGridSizer( 2, 0, 0 );
   grid1->AddGrowableCol(1);
   
   algorithmStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Algorithm"), wxDefaultPosition,wxDefaultSize, 0);
   algorithmComboBox =
      new wxComboBox( parent, ID_COMBOBOX, wxT("NewtonRaphson"), wxDefaultPosition,
                      wxSize(200,-1), 3, algorithmArray, wxCB_DROPDOWN|wxCB_READONLY );
   maxStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Iterations"), wxDefaultPosition, wxDefaultSize, 0 );
   maxTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
   derivativeMethodStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Derivative Method"), wxDefaultPosition,wxDefaultSize, 0);
   derivativeMethodComboBox =
      new wxComboBox( parent, ID_COMBOBOX, wxT("CentralDifference"), wxDefaultPosition, 
                      wxSize(200,-1), 3, derivativeMethodArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   grid1->Add( algorithmStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add( algorithmComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   grid1->Add( maxStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add( maxTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add( derivativeMethodStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   grid1->Add( derivativeMethodComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   optionsStaticBoxSizer->Add( grid1, 0, wxALIGN_LEFT|wxALL, bsize );
   
   
   // 2. Create Output Box:
   wxStaticBoxSizer *outputStaticBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Output");
   wxFlexGridSizer *grid2 = new wxFlexGridSizer( 3, 0, 0 );
   grid2->AddGrowableCol(1);
   
   showProgressCheckBox =
      new wxCheckBox( parent, ID_CHECKBOX, wxT("Show Progress"),wxDefaultPosition, wxDefaultSize, 0 );
   reportStyleStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Report Style"), wxDefaultPosition,wxDefaultSize, 0);
   styleComboBox =
      new wxComboBox( parent, ID_COMBOBOX, wxT("Normal"), wxDefaultPosition, 
                      wxSize(200,-1), 4, styleArray, wxCB_DROPDOWN|wxCB_READONLY );
   reportfileStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Report File"), wxDefaultPosition,wxDefaultSize, 0);
   reportfileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
   
   browseButton = new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
	                            wxSize(buttonWidth, -1));
   
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
   
   
   // add Options box and Output box to theMiddleSizer:
   theMiddleSizer->Add(optionsStaticBoxSizer, 0, wxGROW, bsize);
   theMiddleSizer->Add(outputStaticBoxSizer, 0, wxGROW, bsize);
   
}


/*
void DCSetupPanel::Setup( wxWindow *parent)
{   
    // wxStaticText
    maxStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Iterations"), 
                     wxDefaultPosition, wxDefaultSize, 0 );
    textfileStaticText = new wxStaticText( parent, ID_TEXT, wxT("Targeter Text File"),
                            wxDefaultPosition,wxDefaultSize, 0);
    reportStyleStaticText = new wxStaticText( parent, ID_TEXT, wxT("Report Style"),
                            wxDefaultPosition,wxDefaultSize, 0);
                            
    // wxTextCtrl
    maxTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
                  wxDefaultPosition, wxSize(50,-1), 0 );
    textfileTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(200,-1), 0 );
                            
    // wxCheckBox
    showProgressCheckBox = new wxCheckBox( parent, ID_CHECKBOX, 
                               wxT("Show Progress"),wxDefaultPosition, 
                               wxDefaultSize, 0 );
    centralDifferencesCheckBox = new wxCheckBox( parent, ID_CHECKBOX, 
                               wxT("Use Central Differences"),wxDefaultPosition, 
                               wxDefaultSize, 0 );
                               
    // wxString
   wxString *styleArray = new wxString[4];
   styleArray[0] = "Normal";
   styleArray[1] = "Concise";
   styleArray[2] = "Verbose";
   styleArray[3] = "Debug";

    // wxComboBox
    styleComboBox =
      new wxComboBox( this, ID_COMBOBOX, wxT(reportStyle), wxDefaultPosition, 
         wxSize(200,-1), 4, styleArray, wxCB_DROPDOWN|wxCB_READONLY );
                                
    // wx*Sizer
    wxBoxSizer *boxsizerMain = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    flexGridSizer1->AddGrowableCol( 1 );
 
    // Add to wx*Sizer
    flexGridSizer1->Add( maxStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( maxTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( textfileStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( textfileTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( showProgressCheckBox, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( 0, 0, wxALIGN_CENTRE|wxALL, 5);
    flexGridSizer1->Add( centralDifferencesCheckBox, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( 0, 0, wxALIGN_CENTRE|wxALL, 5);
    flexGridSizer1->Add( reportStyleStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer1->Add( styleComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
    
    boxsizer1->Add( flexGridSizer1, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizerMain->Add( boxsizer1, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(boxsizerMain, 0, wxGROW, 5);
}
*/

//------------------------------------------------------------------------------
// void OnTextUpdate()
//------------------------------------------------------------------------------
void DCSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   isTextModified = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void DCSetupPanel::OnComboBoxChange(wxCommandEvent &event)
{
//   reportStyle = styleComboBox->GetStringSelection().c_str();
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void DCSetupPanel::OnCheckBoxChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}    


//------------------------------------------------------------------------------
// void OnBrowse()
/**
 * This function is used to open file dialog.
 */
//------------------------------------------------------------------------------
void DCSetupPanel::OnBrowse(wxCommandEvent &event)
{
   if (fileDialog == NULL)
   {
      // create fileDialog object when it does not exist
      wxString caption = wxT("Choose a File");
      wxString defaultDir = wxT("C:/DevelGMAT/scripts/");
      wxString defaultFile = wxEmptyString;
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
      wxString filename = fileDialog->GetPath().c_str();
      reportfileTextCtrl->SetValue(filename);
      isTextModified = true;
   }
   else
   {
      // only hide fileDialog when clik on Cancel button
      fileDialog->Hide();
   }
   
}
