//$Id$
//------------------------------------------------------------------------------
//                              AssignmentPanel
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
// Created: 2004/12/23
/**
 * This class contains the Assignment Setup window.
 */
//------------------------------------------------------------------------------

#include "AssignmentPanel.hpp"
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "MessageInterface.hpp"
#include "FileManager.hpp"          // GetGmatFunctionPath()
#include <wx/config.h>

//#define DEBUG_ASSIGNMENT_PANEL 1
//#define DEBUG_ASSIGNMENT_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AssignmentPanel, GmatPanel)
   EVT_TEXT(ID_TEXTCTRL, AssignmentPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// AssignmentPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
AssignmentPanel::AssignmentPanel( wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel() entered\n");
   #endif
   
   theCommand = (Assignment *)cmd;
   mIsTextModified = false;
   
   if (theCommand != NULL)
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~AssignmentPanel()
//------------------------------------------------------------------------------
AssignmentPanel::~AssignmentPanel()
{
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AssignmentPanel::Create()
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel::Create() entered\n");
   #endif
   
   int bsize = 2; // bordersize
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Equation"));
   
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(3, 0, 0);
   
   wxStaticText *mLhsLabel =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Left-Hand Side" );
   mLhsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0);
   mLhsTextCtrl->SetToolTip(pConfig->Read(_T("LeftHandSideHint")));
   
   wxStaticText *equalSign =
      new wxStaticText(this, ID_TEXT, wxT(" = "),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *mRhsLabel =
      new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Right-Hand Side" );
   mRhsTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(400,-1), 0);
   mRhsTextCtrl->SetToolTip(pConfig->Read(_T("RightHandSideHint")));
   
   pageSizer->Add(mLhsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(0, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(mRhsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(mLhsTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   pageSizer->Add(equalSign, 0, wxALIGN_CENTRE|wxALL, bsize);
   pageSizer->Add(mRhsTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AssignmentPanel::LoadData()
{
   #if DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage("AssignmentPanel::LoadData() entered\n");
   #endif
   
   mObject = theCommand;
   
   lhs = theCommand->GetLHS();
   rhs = theCommand->GetRHS();
   
   mLhsTextCtrl->SetValue(lhs.c_str());
   mRhsTextCtrl->SetValue(rhs.c_str());
   
   if (lhs == "" || rhs == "")
   {
      // force to call SaveData() for empty LHS and RHS checking
      mIsTextModified = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AssignmentPanel::SaveData()
{
   #if DEBUG_ASSIGNMENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("\nAssignmentPanel::SaveData() entered, mIsTextModified=%d, lhs='%s', rhs='%s'\n",
       mIsTextModified, lhs.c_str(), rhs.c_str());
   #endif
   
   canClose = true;
   if (mIsTextModified)
   {
		mLhsTextCtrl->SetValue(mLhsTextCtrl->GetValue().Trim(true).Trim(false));
		mRhsTextCtrl->SetValue(mRhsTextCtrl->GetValue().Trim(true).Trim(false));
   }
   std::string newLhs = mLhsTextCtrl->GetValue().WX_TO_STD_STRING;
   std::string newRhs = mRhsTextCtrl->GetValue().WX_TO_STD_STRING;
   
   #if DEBUG_ASSIGNMENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("   newLhs='%s', newRhs='%s'\n", newLhs.c_str(), newRhs.c_str());
   #endif
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      Real rval;
      
      // check if it has blank lhs or rhs
      if (newLhs == "" || newRhs == "")
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "LHS or RHS cannot be blank");
         canClose = false;
         return;
      }
      
      // lhs cannot be a number
      if (GmatStringUtil::ToReal(newLhs, rval))
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Left hand side cannot be a number. %f", rval);
         canClose = false;
      }
      
      bool isCallFunction = false;
      
      // Check if LHS [] for GmatFunction output
      if (GmatStringUtil::IsEnclosedWithBrackets(newLhs))
      {
         // Parse function name
         std::string argStr;
         std::string fName = GmatStringUtil::ParseFunctionName(newRhs, argStr);
         #if DEBUG_ASSIGNMENT_PANEL_SAVE
         MessageInterface::ShowMessage("   fName = '%s'\n", fName.c_str());
         #endif
         
         if (fName != "")
         {
            #if DEBUG_ASSIGNMENT_PANEL_SAVE
            MessageInterface::ShowMessage
               ("   Checking if '%s' is a configured function\n", fName.c_str());
            #endif
            GmatBase *func = theGuiInterpreter->GetConfiguredObject(fName);
            if (func && func->IsOfType(Gmat::FUNCTION))
            {
               isCallFunction = true;
               #if DEBUG_ASSIGNMENT_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   '%s' is a function, func = <%p>'%s'\n", fName.c_str(),
                   func, func ? func->GetName().c_str() : "NULL");
               #endif
            }
            else
            {
               // Call FileManager if function is in the GmatFunction path
               #if DEBUG_ASSIGNMENT_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("   Checking if '%s' is in the GmatFunction path\n", fName.c_str());
               #endif
               std::string funcPath = FileManager::Instance()->GetGmatFunctionPath(fName);
               if (funcPath != "")
               {
                  isCallFunction = true;
                  #if DEBUG_ASSIGNMENT_PANEL_SAVE
                  MessageInterface::ShowMessage
                     ("   '%s' is a function found from the start_up GmatFunction path\n",
                      fName.c_str());
                  #endif
               }
            }
         }
      }
      
      // CallFunction is not allowed in Assignment command
      // @todo
      // If CallFunction, this Assignment command should be replaced with a
      // CallFunction when panel is saved and exit (future work)
      if (isCallFunction)
      {
         // Check for only one output since [] is optional
         StringArray lhsParts = GmatStringUtil::SeparateBy(newLhs, "[],");
         
         #if DEBUG_ASSIGNMENT_PANEL_SAVE
         MessageInterface::ShowMessage("   lhsParts.size() = %d\n", lhsParts.size());
         for (unsigned int i = 0; i < lhsParts.size(); i++)
            MessageInterface::ShowMessage("   lhsParts[%d] = %s\n", i, lhsParts[i].c_str());
         #endif
         
         std::string newGenStr = newLhs + " = " + newRhs;
         if (lhsParts.size() == 1)
         {
            // Set string without [] to LHS
            newLhs = lhsParts[0];
            mLhsTextCtrl->SetValue(newLhs.c_str());
            MessageInterface::ShowMessage
               ("*** WARNING *** [] is removed from the LHS of the equation: %s\n",
                newGenStr.c_str());
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Cannot switch to CallFunction in the equation \"" +
                newGenStr + ".\" Please create CallGmatFunction or CallMatlabFunction from the MissionTree.");
            canClose = false;
         }
      }
      
      if (!isCallFunction)
      {
         #if DEBUG_ASSIGNMENT_PANEL_SAVE
         MessageInterface::ShowMessage("   Calling CheckVariable() to validate LHS\n");
         #endif
         ObjectTypeArray objTypes;
         objTypes.push_back(Gmat::UNKNOWN_OBJECT);
         
         // lhs should be an existing variable or valid object property
         CheckVariable(newLhs, objTypes, "Left hand side",
                       "Variable, Array, Array element, Object property", false,
                       true, true, true);
      }
   }
   
   if (!canClose)
   {
      #if DEBUG_ASSIGNMENT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("AssignmentPanel::SaveData() leaving, user input error encountered\n");
      #endif
      return;
   }
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the validation
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      std::string newGenStr = newLhs + " = " + newRhs;
      
      #if DEBUG_ASSIGNMENT_PANEL_SAVE
      MessageInterface::ShowMessage("   newGenStr = '%s'\n", newGenStr.c_str());
      #endif
      
      try
      {
         // Interpret assignment string         
         theCommand->SetGeneratingString(newGenStr);
         theCommand->InterpretAction();
         
         // Create element wrappers
         if (!theGuiInterpreter->ValidateCommand(theCommand))
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "Error found in the equation \"" + newGenStr + "\"");
            // Set saved lhs and rhs values
            theCommand->SetLHS(lhs);
            theCommand->SetRHS(rhs);
            canClose = false;
         }
         else if (!theCommand->Validate())
         {
            std::string errmsg = theCommand->GetLastErrorMessage();
            theCommand->SetLastErrorMessage("");
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "\"" + newGenStr + "\" failed validation. " + errmsg);
            
            theCommand->SetLHS(lhs);
            theCommand->SetRHS(rhs);
            canClose = false;
         }
         else
         {
            // update lhs and rhs
            lhs = newLhs;
            rhs = newRhs;
            mIsTextModified = false;
         }
      }
      catch (BaseException &e)
      {
         // Set saved lhs and rhs values
         theCommand->SetLHS(lhs);
         theCommand->SetRHS(rhs);
         std::string msg = e.GetFullMessage();
         // remove duplicate exception message
         msg = GmatStringUtil::Replace(msg, "Interpreter Exception: Interpreter Exception: ",
                                       "Interpreter Exception: ");
         
         MessageInterface::PopupMessage(Gmat::ERROR_, msg);
         canClose = false;
      }
   }
   
   #if DEBUG_ASSIGNMENT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("AssignmentPanel::SaveData() leaving, canClose=%d\n", canClose);
   #endif
}

//------------------------------------------------------------------------------
// virtual void OnCancel()
//------------------------------------------------------------------------------
/**
 * Closes panel without saving new input.
 */
//------------------------------------------------------------------------------
void AssignmentPanel::OnCancel(wxCommandEvent &event)
{
   #ifdef DEBUG_ASSIGNMENT_PANEL
   MessageInterface::ShowMessage
      ("AssignmentPanel::OnCancel() Setting lhs = '%s', rhs = '%s' to command\n",
       lhs.c_str(), rhs.c_str());
   #endif
   
   // set saved lhs and rhs values
   theCommand->SetLHS(lhs);
   theCommand->SetRHS(rhs);
   GmatPanel::OnCancel(event);
}

//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AssignmentPanel::OnTextChange(wxCommandEvent& event)
{
   if (mLhsTextCtrl->IsModified() || mRhsTextCtrl->IsModified())
   {
      mIsTextModified = true;
      EnableUpdate(true);
   }
}

