//$Id$
//------------------------------------------------------------------------------
//                              GmatCommandPanel
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
// Author: Linda Jun
// Created: 2010/07/15
/**
 * Implements GmatCommandPanel class.
 */
//------------------------------------------------------------------------------
#include "GmatCommandPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_GMATCOMMAND_PANEL
//#define DEBUG_GMATCOMMAND_PANEL_LOAD
//#define DEBUG_GMATCOMMAND_PANEL_SAVE

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(GmatCommandPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_TEXT(ID_TEXT_CTRL, GmatCommandPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
   
//------------------------------------------------------------------------------
// GmatCommandPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs GmatCommandPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the GmatCommandPanel GUI
 */
//------------------------------------------------------------------------------
GmatCommandPanel::GmatCommandPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage
      ("GmatCommandPanel() entered, cmd=<%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   if (cmd != NULL)
   {
      theCommand = cmd;
      mObject = (GmatBase*)cmd;
      Create();
      Show();
      EnableUpdate(false);
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// ~GmatCommandPanel()
//------------------------------------------------------------------------------
GmatCommandPanel::~GmatCommandPanel()
{
}


//-------------------------------
// protected methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void GmatCommandPanel::Create()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel::Create() entering...\n");
   #endif
   
   Integer bsize = 2; // border size
   wxArrayString emptyList;   
   
   //-------------------------------------------------------
   // show get generating string
   //-------------------------------------------------------
   wxBoxSizer *textSizer = new wxBoxSizer(wxVERTICAL);
   
   commandTextCtrl = new
      wxTextCtrl(this, ID_TEXT_CTRL, wxT(""), wxDefaultPosition, wxSize(450, -1));
   
   textSizer->Add(commandTextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(textSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_GMATCOMMAND_PANEL
   MessageInterface::ShowMessage("GmatCommandPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void GmatCommandPanel::LoadData()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL_LOAD
   MessageInterface::ShowMessage("GmatCommandPanel::LoadData() entering...\n");
   #endif
   
   try
   {
      wxString genStr = theCommand->GetGeneratingString(Gmat::NO_COMMENTS).c_str();
      commandTextCtrl->SetValue(genStr);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL_LOAD
   MessageInterface::ShowMessage("GmatCommandPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void GmatCommandPanel::SaveData()
{
   #ifdef DEBUG_GMATCOMMAND_PANEL_SAVE
   MessageInterface::ShowMessage("GmatCommandPanel::SaveData() entered\n");
   #endif
   
   canClose         = true;
//   std::string str;
   bool interpreted = true;
//   Integer width, prec;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   std::string genString = theCommand->GetGeneratingString(Gmat::NO_COMMENTS);
   try
   {
      std::string cmdStr = commandTextCtrl->GetValue().WX_TO_STD_STRING;
      theCommand->SetGeneratingString(cmdStr);
      if (!theCommand->InterpretAction())
         canClose = false;
      
      else if (!theCommand->VerifyObjects())
         canClose = false;
      
      // Now validate command and create element wrappers
      else if (!theGuiInterpreter->ValidateCommand(theCommand))
         canClose = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      theCommand->SetGeneratingString(genString);
      theCommand->InterpretAction();  // objects previously checked
      // Now re-validate command and create element wrappers
      if (!theGuiInterpreter->ValidateCommand(theCommand))
         canClose = false;
      canClose = false;
      return;
   }
   if (!canClose)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_,
                        "Error validating '%s' Command - "
                        "Invalid object or field name",
                        theCommand->GetTypeName().c_str());
      theCommand->SetGeneratingString(genString);
      theCommand->InterpretAction();
      // Now re-validate command and create element wrappers
      if (!theGuiInterpreter->ValidateCommand(theCommand))
         canClose = false;
      return;
   }
   
   #ifdef DEBUG_GMATCOMMAND_PANEL_SAVE
   MessageInterface::ShowMessage("GmatCommandPanel::SaveData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void GmatCommandPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}


