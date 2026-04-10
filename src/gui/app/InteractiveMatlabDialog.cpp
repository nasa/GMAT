//$Id$
//------------------------------------------------------------------------------
//                              InteractiveMatlabDialog
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
// Author: Allison Greene
// Created: 2004/11/12
//
/**
 * Declares InteractiveMatlabDialog class. This class shows dialog window where
 * parameters can be passed to/from matlab.
 *
 */
//------------------------------------------------------------------------------

#include "InteractiveMatlabDialog.hpp"
#include "ParameterSelectDialog.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(InteractiveMatlabDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON, InteractiveMatlabDialog::OnButton)

   EVT_GRID_CELL_LEFT_CLICK(InteractiveMatlabDialog::OnCellClick)
   EVT_GRID_CELL_RIGHT_CLICK(InteractiveMatlabDialog::OnCellClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// InteractiveMatlabDialog(wxWindow *parent)
//------------------------------------------------------------------------------
InteractiveMatlabDialog::InteractiveMatlabDialog(wxWindow *parent)
   : wxDialog(parent, -1, wxString(_T("InteractiveMatlabDialog")))
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   theParent = parent;

   mObjectTypeList.Add("Spacecraft");
   
   // reset array of strings
   inputStrings.Clear();
   outputStrings.Clear();
   
   // create command
   theCmd = new CallFunction("CallMatlabFunction");
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~InteractiveMatlabDialog()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
InteractiveMatlabDialog::~InteractiveMatlabDialog()
{
   mObjectTypeList.Clear();
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void InteractiveMatlabDialog::Create()
{
   int borderSize = 2;

   wxStaticBox *topStaticBox = new wxStaticBox(this, -1, wxT(""));
   wxStaticBox *middleStaticBox = new wxStaticBox(this, -1, wxT("Results"));
   wxStaticBox *bottomStaticBox = new wxStaticBox(this, -1, wxT(""));

    // create sizers
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   theTopSizer = new wxStaticBoxSizer(topStaticBox, wxVERTICAL);
   theMiddleSizer = new wxStaticBoxSizer(middleStaticBox, wxVERTICAL);
   theBottomSizer = new wxStaticBoxSizer(bottomStaticBox, wxVERTICAL);
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL); //loj: 10/19/04 Made theButtonSizer member data

    // create bottom buttons
   theEvaluateButton =
      new wxButton(this, ID_BUTTON, "Evaluate", wxDefaultPosition, wxDefaultSize, 0);

   theClearButton =
      new wxButton(this, ID_BUTTON, "Clear", wxDefaultPosition, wxDefaultSize, 0);

   theCloseButton =
      new wxButton(this, ID_BUTTON, "Close", wxDefaultPosition, wxDefaultSize, 0);

   // adds the buttons to button sizer
   theButtonSizer->Add(theEvaluateButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theClearButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);

   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);

   // fill in top sizer
   int bsize = 5; // bordersize

   wxFlexGridSizer *mflexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *outputSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *inputSizer = new wxBoxSizer(wxHORIZONTAL);

   wxStaticText *outLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("[  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *outRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  ]"),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *inLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("[  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *inRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  ]"),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *equalSign =
                     new wxStaticText( this, ID_TEXT, wxT("  =  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *outStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Output  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *inStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Input  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   wxStaticText *functionStaticText =
                     new wxStaticText( this, ID_TEXT, wxT("  Function  "),
                     wxDefaultPosition, wxDefaultSize, 0 );

   StringArray list = theGuiInterpreter->GetListOfObjects(Gmat::FUNCTION);
   int size = list.size();
   wxString *choices = new wxString[size];

   for (int i=0; i<size; i++)
   {
        choices[i] = list[i].c_str();
   }

    // combo box for the date type
   functionComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
             wxDefaultPosition, wxSize(130,-1), size, choices,
             wxCB_DROPDOWN | wxCB_READONLY );

   // wxGrid
   inputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(290, 23), wxWANTS_CHARS );

   inputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   inputGrid->SetColSize(0, 290);
   inputGrid->SetRowSize(0, 23);
   inputGrid->SetColLabelSize(0);
   inputGrid->SetRowLabelSize(0);
   inputGrid->SetMargins(0, 0);
   inputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   inputGrid->EnableEditing(false);

   outputGrid =
      new wxGrid( this, -1, wxDefaultPosition, wxSize(290, 23), wxWANTS_CHARS );

   outputGrid->CreateGrid( 1, 1, wxGrid::wxGridSelectRows );
   outputGrid->SetColSize(0, 290);
   outputGrid->SetRowSize(0, 23);
   outputGrid->SetColLabelSize(0);
   outputGrid->SetRowLabelSize(0);
   outputGrid->SetMargins(0, 0);
   outputGrid->SetScrollbars(0, 0, 0, 0, 0, 0, FALSE);
   outputGrid->EnableEditing(false);

   outputSizer->Add(outLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(outputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);
   outputSizer->Add(outRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);

   inputSizer->Add(inLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(inputGrid, 0, wxALIGN_CENTRE|wxALL, bsize);
   inputSizer->Add(inRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);

//   horizontalSizer->Add(nameStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   horizontalSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   horizontalSizer->Add(functionComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);

   mflexGridSizer->Add(outputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(outStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(horizontalSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(functionStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(inputSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mflexGridSizer->Add(inStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);

   theTopSizer->Add(mflexGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);

   // fill in middle sizer
   outputTextCtrl = new wxTextCtrl( this, ID_TEXT, wxT(""),
                            wxDefaultPosition, wxSize(350,175),
                            wxTE_MULTILINE | wxTE_READONLY);
   theMiddleSizer->Add(outputTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void InteractiveMatlabDialog::Show()
{
   // add items to middle sizer
   theDialogSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
   theDialogSizer->Add(theMiddleSizer, 0, wxGROW | wxALL, 1);
   theDialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer); //use the sizer for layout
   theDialogSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   theDialogSizer->SetSizeHints(this); //set size hints to honour minimum size

   CenterOnScreen(wxBOTH);
}

//---------------------------------
// private methods
//---------------------------------
void InteractiveMatlabDialog::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == theEvaluateButton)
   {
      delete(theCmd);
      theCmd = new CallFunction("CallMatlabFunction");
      SetupCommand();
      SetResults();
   }
   else if (event.GetEventObject() == theClearButton)
   {
      OnClear();
   }
   else if (event.GetEventObject() == theCloseButton)
   {
      OnClear();
      delete(theCmd);
      Close();
   }
   else
   {
      //Error - unknown object
   }
}

//------------------------------------------------------------------------------
// void OnCellClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void InteractiveMatlabDialog::OnCellClick(wxGridEvent& event)
{
   unsigned int row = event.GetRow();
   unsigned int col = event.GetCol();

   if (event.GetEventObject() == inputGrid)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE,
                                     true, false, true);
      paramDlg.SetParamNameArray(inputStrings);
      paramDlg.ShowModal();
      
      inputStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";

      if (inputStrings.Count() > 0)
      {
         cellValue = cellValue + inputStrings[0];

         for (unsigned int i=1; i<inputStrings.Count(); i++)
         {
            cellValue = cellValue + ", " + inputStrings[i];
         }

         inputGrid->SetCellValue(row, col, cellValue);
      }
      else     // no selections
         inputGrid->SetCellValue(row, col, "");
   }
   else if (event.GetEventObject() == outputGrid)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE,
                                     true, false, true);
      paramDlg.SetParamNameArray(outputStrings);
      paramDlg.ShowModal();

      outputStrings = paramDlg.GetParamNameArray();
      wxString cellValue = "";

      if (outputStrings.Count() > 0)
      {
         cellValue = cellValue + outputStrings[0];

         for (unsigned int i=1; i<outputStrings.Count(); i++)
         {
            cellValue = cellValue + ", " + outputStrings[i];
         }

         outputGrid->SetCellValue(row, col, cellValue);
      }
      else     // no selections
         outputGrid->SetCellValue(row, col, "");
   }
}

void InteractiveMatlabDialog::SetupCommand()
{
//   MessageInterface::ShowMessage("SetupCommand() Entered \n");
   wxString functionName = functionComboBox->GetStringSelection();

   // arg: for now to avoid a crash
   if (functionName != "")
   {
      Function *function = (Function *)theGuiInterpreter->GetConfiguredObject(
               std::string(functionName));

      if (function != NULL)
      {
         theCmd->SetRefObject(function, Gmat::FUNCTION, function->GetName());
      }
   }
   else
      throw ("No Function Name Given");

   // clear out previous parameters
   theCmd->TakeAction("Clear");

   // set input parameters
   for (unsigned int i=0; i<inputStrings.Count(); i++)
   {
      std::string selInName = std::string(inputStrings[i]);
      theCmd->SetStringParameter("AddInput", selInName, i);
   }

   // set output parameters
   for (unsigned int i=0; i<outputStrings.Count(); i++)
   {
       std::string selOutName = std::string(outputStrings[i]);
       theCmd->SetStringParameter("AddOutput", selOutName, i);
   }

}

void InteractiveMatlabDialog::SetResults()
{
//   MessageInterface::ShowMessage("SetResults() Entered \n");

   // need to execute the command
//   theCmd->Initialize();
   theCmd->Execute();

//   MessageInterface::ShowMessage("executed command \n");

   outputTextCtrl->AppendText(wxT("\nSent to Matlab:  "));
   std::string evaluationString = theCmd->FormEvalString();
   outputTextCtrl->AppendText(evaluationString.c_str());

//   MessageInterface::ShowMessage("got eval string \n");

   outputTextCtrl->AppendText(wxT("\n\n"));

   // output the results
   for (unsigned int i=0; i<outputStrings.Count(); i++)
   {
      Parameter *param = (Parameter *)theGuiInterpreter->GetConfiguredObject(
            std::string(outputStrings[i]));

      if (param->GetTypeName() == "Array")
      {
//         MessageInterface::ShowMessage("parameter is an array \n");

         Array *array = (Array *)param;
         int numRows = array->GetIntegerParameter("NumRows");
         int numCols = array->GetIntegerParameter("NumCols");

         // create rmatrix
         Rmatrix rmatrix = array->GetRmatrixParameter("RmatValue");
//         MessageInterface::ShowMessage("got the array values\n");

         std::ostringstream os;
         os << array->GetName().c_str() << " = \n" ;

         for (int j=0; j<numRows; j++)
         {
           for (int k=0; k<numCols; k++)
             os << "\t" << rmatrix(j, k);
           os << "\n";
         }

         std::string paramString = os.str();
         outputTextCtrl->AppendText(paramString.c_str());

      }
      else if (param->GetTypeName() == "String")
      {
         wxString paramString;
         StringVar *stringVar = (StringVar *)param;
         paramString.Printf("%s = %s\n", param->GetName().c_str(),
                                           stringVar->GetString().c_str());
         outputTextCtrl->AppendText(paramString);
      }
      else
//      if (param->GetTypeName() == "Variable")
      {
         wxString paramString;
         paramString.Printf("%s = %f\n", param->GetName().c_str(),
                                           param->EvaluateReal());
         outputTextCtrl->AppendText(paramString);
      }
   }
}

void InteractiveMatlabDialog::OnClear()
{
      // set gui to empty string
      inputGrid->SetCellValue(0, 0, "");
      outputGrid->SetCellValue(0, 0, "");
      outputTextCtrl->SetValue("");
      functionComboBox->SetValue("");

      // reset array of strings
      inputStrings.Clear();
      outputStrings.Clear();

      // clear out previous parameters
      theCmd->TakeAction("Clear");
}
