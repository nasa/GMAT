//$Id$
//------------------------------------------------------------------------------
//                           InspectorPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract, Task Order 112
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Apr 20, 2021
/**
 * The GMAT debugger panel, used to view run time objects and step through a
 * control sequence.
 *
 * Parts of this code are derived from Thinking Systems' Astrodynamics
 * Workbench, and used by permission
 */
//------------------------------------------------------------------------------

#include "InspectorPanel.hpp"
#include "Moderator.hpp"
#include "ScriptInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"
#include "GmatCommand.hpp"
#include "Breakpoint.hpp"


BEGIN_EVENT_TABLE ( InspectorPanel, wxDialog )
   EVT_BUTTON(BUTTON_Step,  InspectorPanel::Step)
   EVT_BUTTON(BUTTON_Close, InspectorPanel::Close)
   EVT_BUTTON(BUTTON_End, InspectorPanel::End)
   EVT_RADIOBUTTON(RADIO_Spacecraft, InspectorPanel::SetSelectionList)
   EVT_RADIOBUTTON(RADIO_All, InspectorPanel::SetSelectionList)
   EVT_COMBOBOX(COMBO_Selection, InspectorPanel::SelectObject)
   EVT_CLOSE(InspectorPanel::Close)
END_EVENT_TABLE()


InspectorPanel::InspectorPanel() :
   wxDialog(NULL, -1, "Object Inspector", wxDefaultPosition, wxDefaultSize,
         wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
   currentcmd     (NULL)
{
   theModerator = Moderator::Instance();
   theInterpreter = theModerator->GetUiInterpreter();
   theMainFrame = GmatAppData::Instance()->GetMainFrame();

   // Retrieve the object lists
   PopulateLists();

   // Object layout

   // Everything sits inside of topPanel
   wxBoxSizer *topPanel = new wxBoxSizer(wxVERTICAL);

   // Breakpoint Layout contains the command text and the data layout
   wxBoxSizer *breakpointLayoutBox = new wxBoxSizer(wxVERTICAL);

   // Data layout has 2 subpanels, with the radio buttons, combo box, and data
   wxBoxSizer *dataLayoutBox = new wxBoxSizer(wxHORIZONTAL);

   wxBoxSizer *leftVertPanel = new wxBoxSizer(wxVERTICAL);
   dataLayoutBox->Add(leftVertPanel, 0, wxEXPAND);


   // The left subpanel has radio buttons and a combo box
   wxPanel *leftPanel = new wxPanel(this, -1);
   wxStaticBox *objectSelectBox = new wxStaticBox(leftPanel, -1,
         wxT(""),wxPoint(5, 5), wxSize(200, 80));
   leftVertPanel->Add(leftPanel, 1);

   // Text of current command
   wxBoxSizer *cmdLayoutBox = new wxBoxSizer(wxHORIZONTAL);
   cmdLayoutBox->AddSpacer(5);
   wxPanel *cmdPanel = new wxPanel(this, -1);
   cmdText = new wxStaticText(cmdPanel, wxID_ANY, "InspectorPanel: Command Not Set\n");
//   breakpointLayoutBox->Add(cmdPanel);
   cmdLayoutBox->Add(cmdPanel);
   topPanel->AddSpacer(5);
   topPanel->Add(cmdLayoutBox);
   topPanel->AddSpacer(5);

   // Object grouping radio buttons
   scrb = new wxRadioButton(objectSelectBox, RADIO_Spacecraft,
       wxT("Spacecraft Only"), wxPoint(15, 5), wxDefaultSize, wxRB_GROUP);
   allrb = new wxRadioButton(objectSelectBox, RADIO_All,
      wxT("All Objects"), wxPoint(15, 32), wxDefaultSize);

   //showingSpacraft = false;
   if (showingSpacraft)
      scrb->SetValue(true);
   else
      allrb->SetValue(true);

   wxStaticBox *theSelectBox = new wxStaticBox(leftPanel, -1,
         wxT(""),wxPoint(5, 105), wxSize(240, 600));

   choices = new wxComboBox(theSelectBox, COMBO_Selection, currentObject,
         wxDefaultPosition, wxSize(180, -1), currentObjects, wxCB_READONLY);

   wxBoxSizer *rightVertPanel = new wxBoxSizer(wxVERTICAL);
   dataLayoutBox->Add(rightVertPanel, 1, wxEXPAND);

   wxPanel    *rightPanel = new wxPanel(this, -1);
   rightVertPanel->Add(rightPanel);


   theScript = new wxTextCtrl(rightPanel, -1, text, wxPoint(0,0), wxSize(450,700),
                              wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);
   theScript->SetFont( GmatAppData::Instance()->GetFont() );
   //rightVertPanel->Add(theScript);

   // Buttons, at the bottom of the dialog
   wxBoxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);
   stepButton = new wxButton(this, BUTTON_Step, wxT("Step"),
       wxDefaultPosition, wxSize(120, 30));
   closeButton = new wxButton(this, BUTTON_Close, wxT("Resume Run"),
       wxDefaultPosition, wxSize(120, 30));
   endButton = new wxButton(this, BUTTON_End, wxT("End Run"),
       wxDefaultPosition, wxSize(120, 30));

   buttonBox->Add(stepButton, 1);
   buttonBox->Add(closeButton, 1, wxLEFT, 5);
   buttonBox->Add(endButton, 1, wxLEFT, 5);

   breakpointLayoutBox->Add(dataLayoutBox, 1, wxEXPAND);

   topPanel->Add(breakpointLayoutBox, 1, wxEXPAND);
   topPanel->Add(buttonBox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
   SetSizerAndFit(topPanel);

   theRunState = theModerator->GetRunState();
   theInterpreter->ChangeRunState("Pause");
}

void InspectorPanel::SetCommand(GmatCommand *cmd)
{
   currentcmd = cmd;
   cmdText->SetLabel(currentcmd->GetGeneratingString(Gmat::NO_COMMENTS));
   //cmdText->GetParent()->Layout();
}


//---------------------------------------------------------------------
// Event handlers
//---------------------------------------------------------------------

void InspectorPanel::SelectObject(wxCommandEvent& WXUNUSED(event))
{
   currentObject = choices->GetValue();
   UpdateText();
}

void InspectorPanel::Step(wxCommandEvent& event)
{
   if(currentcmd != NULL)
   {
      if (currentcmd->IsOfType("BranchCommand"))
      {
         if (currentcmd->IsExecuting())
         {
            currentcmd = currentcmd->GetNext();
         }
         else
            currentcmd = currentcmd->GetNext();
      }
      else
         currentcmd = currentcmd->GetNext();
   }

   if(currentcmd == NULL)
   {
      MessageInterface::ShowMessage("End of Script.\n");
      theMainFrame->OnStop(event);
      Destroy();
   }
   else
   {
      while(currentcmd != NULL)
      {
         if (currentcmd->GetGeneratingString(Gmat::NO_COMMENTS) != "")
            break;
         currentcmd = currentcmd->GetNext();
      }

      if(currentcmd == NULL)
      {
         MessageInterface::ShowMessage("End of Script.\n");
         theMainFrame->OnStop(event);
         Destroy();
      }
      else
      {
         // Skip breakpoints since we are already debugging
         if (currentcmd->GetTypeName() == "Breakpoint")
            currentcmd = currentcmd->GetNext();

         if(currentcmd == NULL)
         {
            MessageInterface::ShowMessage("End of Script.\n");
            theMainFrame->OnStop(event);
            Destroy();
         }
         else
         {
            SetCommand(currentcmd);
            currentcmd->Execute();

            UpdateText();
         }
      }
   }
}

void InspectorPanel::End(wxCommandEvent& event)
{
   theMainFrame->OnStop(event);
   Destroy();
}


void InspectorPanel::Close(wxCommandEvent& event)
{
   theMainFrame->OnRun(event);
   Breakpoint::ClearDebugger();
   Destroy();
}

//------------------------------------------------------------------------------
// void InspectorPanel::Close(wxCloseEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler used to trap closing with the X in the window frame
 *
 * @param event The close event - unused here
 */
//------------------------------------------------------------------------------
void InspectorPanel::Close(wxCloseEvent& event)
{
   theMainFrame->ResumeRun();
   Breakpoint::ClearDebugger();
   Destroy();
}

void InspectorPanel::SetSelectionList(wxCommandEvent& WXUNUSED(event))
{
   if (scrb->GetValue())
   {
      // Only show spacecraft
      currentObjects = satList;
      if (currentObjects.Index(currentObject) == wxNOT_FOUND)
      {
         if (currentObjects.GetCount() > 0)
            currentObject = currentObjects[0];
         else
            currentObject = "";
      }
   }
   else
   {
      // Show all objects in the Sandbox
      currentObjects = allObjects;
   }

   choices->Set(currentObjects);

   choices->SetValue(currentObject);
   currentObject = choices->GetStringSelection();
   UpdateText();
}


//---------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------


void InspectorPanel::PopulateLists()
{
   StringArray objects = theModerator->GetListOfObjects(Gmat::SPACECRAFT);
   satList.Clear();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
      satList.Add(objects[i].c_str());

   objects = theModerator->GetListOfObjects(Gmat::UNKNOWN_OBJECT);
   allObjects.Clear();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
      allObjects.Add(objects[i].c_str());

   if (satList.GetCount() > 0)
   {
      showingSpacraft = true;
      currentObject = satList[0];
      currentObjects = satList;
   }
   else
   {
      showingSpacraft = false;
      if (allObjects.GetCount() > 0)
         currentObject = allObjects[0];
      else
         currentObject = "";
      currentObjects = allObjects;
   }

   if (currentObject != "")
      BuildText();
}

void InspectorPanel::UpdateText()
{
   BuildText();
   theScript->Clear();
   theScript->ChangeValue(text);
}

void InspectorPanel::BuildText()
{
   theObject = theModerator->GetInternalObject(currentObject.ToStdString());
   if (theObject != NULL)
   {
      generatingString = theObject->GetGeneratingString(Gmat::DEBUG_INSPECT);
      text = generatingString.c_str();

      if (theObject->IsOfType("Parameter"))
      {
         std::string contents =
               "\n\n------- Current Value -------\n\n" +
               ((Parameter*)theObject)->ToString();
         text += contents.c_str();
      }
   }
   else
      text = "Got nothing to say here\n";
}
