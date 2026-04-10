//$Id$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
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
 * This class contains the Matlab Command Setup window.
 */
//------------------------------------------------------------------------------

#include "CallFunctionPanel.hpp"
#include "MessageInterface.hpp"

#include "Function.hpp"
#include <wx/colour.h>
#include "Parameter.hpp"
#include "ParameterSelectDialog.hpp"

//#define DEBUG_CALLFUNCTION_PANEL 1
//#define DEBUG_CALLFUNCTION_PANEL_LOAD 1
//#define DEBUG_CALLFUNCTION_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CallFunctionPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBO, CallFunctionPanel::OnComboChange)
   EVT_BUTTON(ID_BUTTON, CallFunctionPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CallFunctionPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CallFunctionPanel::CallFunctionPanel( wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   theCommand = (CallFunction *)cmd;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   if (theCommand != NULL)
   {
      Create();
      Show();

      // If function name is blank, set update flag to ask for function
      // name when save
      if (theFunctionComboBox->GetValue() == "")
         EnableUpdate(true);
   }
   
   // Listen for Spacecraft and Parameter name change
   theGuiManager->AddToResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// ~CallFunctionPanel()
//------------------------------------------------------------------------------
CallFunctionPanel::~CallFunctionPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Function", theFunctionComboBox);   
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool CallFunctionPanel::PrepareObjectNameChange()
{
   // Save GUI data
   wxCommandEvent event;
   OnApply(event);
   
   return GmatPanel::PrepareObjectNameChange();
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(UnsignedInt type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
/*
 * Reflects resource name change to this panel.
 * By the time this method is called, the base code already changed reference
 * object name, so all we need to do is re-load the data.
 */
//------------------------------------------------------------------------------
void CallFunctionPanel::ObjectNameChanged(UnsignedInt type,
                                          const wxString &oldName,
                                          const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ReportFilePanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::FUNCTION)
      return;
   
   LoadData();
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CallFunctionPanel::Create()
{
   //MessageInterface::ShowMessage("CallFunctionPanel::Create() entered\n");
   
   int bsize = 3; // bordersize
   
   wxStaticText *outLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("[  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *outRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  ]"),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *inLeftBracket =
                     new wxStaticText( this, ID_TEXT, wxT("(  "),
                     wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticText *inRightBracket =
                     new wxStaticText( this, ID_TEXT, wxT("  )"),
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
   
   theOutputTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(300, -1),
                      wxTE_MULTILINE|wxTE_READONLY);
   theOutputTextCtrl->SetBackgroundColour( *wxLIGHT_GREY );
   
   theOutputViewButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
   
   // Get available function ComboBox from theGuiManager
   theFunctionComboBox =
      theGuiManager->GetFunctionComboBox(this, ID_COMBO, wxSize(300, -1));

   theInputTextCtrl =
      new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(300, -1),
                      wxTE_MULTILINE|wxTE_READONLY);
   theInputTextCtrl->SetBackgroundColour( *wxLIGHT_GREY );
   
   theInputViewButton = new
      wxButton(this, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0);
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer( 5, 0, 0 );
   pageSizer->Add(outLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theOutputTextCtrl, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(outStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theOutputViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   pageSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theFunctionComboBox, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(5, 5);
   pageSizer->Add(functionStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(5, 5);
   
   pageSizer->Add(inLeftBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theInputTextCtrl, 0, wxEXPAND|wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inRightBracket, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(inStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(theInputViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CallFunctionPanel::LoadData()
{
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage("CallFunctionPanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   int id = theCommand->GetParameterID("FunctionName");
   std::string functionName = theCommand->GetStringParameter(id);
   
   // If function name is not in the ComboBox list, add blank
   if (!theFunctionComboBox->SetStringSelection(wxString(functionName.c_str())))
   {
      theFunctionComboBox->Append("");
      theFunctionComboBox->SetStringSelection("");
   }
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Function name is: <%s>\n", functionName.c_str());
   #endif
   
   // Get input parameters
   StringArray inputList = theCommand->GetStringArrayParameter("AddInput");
   mNumInput = inputList.size();
   mInputWxStrings.Clear();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Found %d input parameters:\n", mNumInput);
   for (StringArray::iterator i = inputList.begin(); i != inputList.end(); ++i)
      MessageInterface::ShowMessage("      '%s'\n", i->c_str());
   #endif
   
   if (mNumInput > 0)
   {
      wxString *inputNames = new wxString[mNumInput];
      wxString inputText = "";
      wxString delimiter = ", ";
      
      for (int i=0; i<mNumInput; i++)
      {         
         inputNames[i] = inputList[i].c_str();
         
         if (i == mNumInput-1)
            delimiter = "";
         
         inputText = inputText + inputList[i].c_str() + delimiter;
         mInputWxStrings.Add(inputList[i].c_str());
      }
      
      theInputTextCtrl->SetValue(inputText);
      delete [] inputNames;
   }
   
   // get output parameters
   StringArray outputList = theCommand->GetStringArrayParameter("AddOutput");
   mNumOutput = outputList.size();
   mOutputWxStrings.Clear();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_LOAD
   MessageInterface::ShowMessage
      ("   Found %d output parameters:\n", mNumOutput);
   for (StringArray::iterator i = outputList.begin(); i != outputList.end(); ++i)
      MessageInterface::ShowMessage("      '%s'\n", i->c_str());
   #endif
   
   if (mNumOutput > 0)
   {
      wxString *outputNames = new wxString[mNumOutput];
      wxString outputText = "";
      wxString delimiter = ", ";
      
      for (int i=0; i<mNumOutput; i++)
      {         
         outputNames[i] = outputList[i].c_str();
         
         if (i == mNumOutput-1)
            delimiter = "";
         
         outputText = outputText + outputList[i].c_str() + delimiter;
         mOutputWxStrings.Add(outputList[i].c_str());
      }
      
      theOutputTextCtrl->SetValue(outputText);
      delete [] outputNames;
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CallFunctionPanel::SaveData()
{
   wxString functionName = theFunctionComboBox->GetValue();
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CallFunctionPanel::SaveData() functionName=%s\n", functionName.c_str());
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check function name
   //-----------------------------------------------------------------
   if (functionName == "" || functionName.Find(" ") != wxNOT_FOUND)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, + "Please select function name.\n");
      canClose = false;
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      theCommand->SetStringParameter("FunctionName", functionName.c_str());
      
      mNumInput = mInputWxStrings.Count();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
      MessageInterface::ShowMessage("   Found %d input parameters:\n", mNumInput);
      #endif
      
      if (mNumInput >= 0) // >=0 because the list needs to be cleared
      {
         theCommand->TakeAction("ClearInput");
         for (int i=0; i<mNumInput; i++)
         {
            std::string selInName = std::string(mInputWxStrings[i]);
            theCommand->SetStringParameter("AddInput", selInName, i);
            
            #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
            MessageInterface::ShowMessage("      '%s'\n", selInName.c_str());
            #endif
         }
      }
      
      mNumOutput = mOutputWxStrings.Count();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
      MessageInterface::ShowMessage("   Found %d output parameters:\n", mNumOutput);
      #endif
      
      if (mNumOutput >= 0) // >=0 because the list needs to be cleared
      {
         theCommand->TakeAction("ClearOutput");
         for (int i=0; i<mNumOutput; i++)
         {
            std::string selOutName = std::string(mOutputWxStrings[i]);
            theCommand->SetStringParameter("AddOutput", selOutName, i);
            
            #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
            MessageInterface::ShowMessage("      '%s'\n", selOutName.c_str());
            #endif
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #ifdef DEBUG_CALLFUNCTION_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CallFunctionPanel::SaveData() theCommand=<%s>\n",
       theCommand->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void OnComboChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void CallFunctionPanel::OnComboChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CallFunctionPanel::OnButtonClick(wxCommandEvent& event)
{
   #ifdef DEBUG_CALLFUNCTION_PANEL
   MessageInterface::ShowMessage
      ("CallFunctionPanel::OnButtonClick() entered\n");
   #endif
   
   wxString value = "";
   wxString delimiter = ", ";
   
   if (event.GetEventObject() == theInputViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_REPORTABLE,
                                     2, true, true, true, true, true, true);
      
      paramDlg.SetParamNameArray(mInputWxStrings);
      paramDlg.ShowModal();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage
         ("   Has new input selection made? %d\n", paramDlg.HasSelectionChanged());
      #endif
      
      if (paramDlg.HasSelectionChanged())
      {
         EnableUpdate(true);
         mInputWxStrings = paramDlg.GetParamNameArray();
         
         if (mInputWxStrings.Count() > 0)
         {
            value = value + mInputWxStrings[0];
         
            for (unsigned int i=1; i<mInputWxStrings.Count(); i++)
               value = value + delimiter + mInputWxStrings[i];
         
            theInputTextCtrl->SetValue(value);
         }
         else     // no selections
         {
            theInputTextCtrl->SetValue(value);
         }
      }
   }
   else if (event.GetEventObject() == theOutputViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_REPORTABLE,
                                     2, true, true, true, true, true, true, "Spacecraft", true, true);
      
      paramDlg.SetParamNameArray(mOutputWxStrings);
      paramDlg.ShowModal();
      
      #ifdef DEBUG_CALLFUNCTION_PANEL
      MessageInterface::ShowMessage
         ("   Has new output selection made? %d\n", paramDlg.HasSelectionChanged());
      #endif
      
      if (paramDlg.HasSelectionChanged())
      {
         EnableUpdate(true);
         mOutputWxStrings = paramDlg.GetParamNameArray();
         
         if (mOutputWxStrings.Count() > 0)
         {
            value = value + mOutputWxStrings[0];
            
            for (unsigned int i=1; i<mOutputWxStrings.Count(); i++)
               value = value + delimiter + mOutputWxStrings[i];
            
            theOutputTextCtrl->SetValue(value);
         }
         else     // no selections
         {
            theOutputTextCtrl->SetValue(value);
         }
      }
   }
}
