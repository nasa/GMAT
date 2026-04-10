//$Id$
//------------------------------------------------------------------------------
//                              FindEventsPanel
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
// Author: Wendy Shoan
// Created: 2005.03.27
//
/**
 * This class contains the FindEvents Setup window.
 */
//------------------------------------------------------------------------------

#include "FindEventsPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FINDEVENTS_PANEL 1
//#define DEBUG_FINDEVENTS_LOAD

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FindEventsPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_LOCATOR_COMBOBOX, FindEventsPanel::OnLocatorComboBoxChange)
   EVT_CHECKBOX(ID_APPEND_CHECKBOX, FindEventsPanel::OnAppendCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// FindEventsPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs FindEventsPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
FindEventsPanel::FindEventsPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)

{
   theCommand = cmd;

   if (theCommand != NULL)
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~FindEventsPanel()
//------------------------------------------------------------------------------
FindEventsPanel::~FindEventsPanel()
{
   theGuiManager->UnregisterComboBox("EventLocator", locatorCB);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnLocatorComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FindEventsPanel::OnLocatorComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnAppendCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FindEventsPanel::OnAppendCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 *
 * @note Creates the panel for the FindEvents Command
 */
//------------------------------------------------------------------------------
void FindEventsPanel::Create()
{
   #if DEBUG_FINDEVENTS_PANEL
   MessageInterface::ShowMessage("FindEventsPanel::Create() command=%s\n",
                                 theCommand->GetTypeName().c_str());
   #endif

   StringArray items;
   Integer bsize = 5; // border size

   // create sizers
   wxFlexGridSizer *pageFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );

   //----------------------------------------------------------------------
   // Locators
   //----------------------------------------------------------------------
   // create locator label
   wxStaticText *locatorLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("Event Locator"), wxDefaultPosition, wxDefaultSize, 0);

   #if DEBUG_FINDEVENTS_PANEL
   MessageInterface::ShowMessage
      ("FindEventsPanel::Create() Calling theGuiManager->GetLocatorComboBox()\n");
   #endif

   // create locator combo box
   locatorCB = theGuiManager->GetLocatorComboBox(this, ID_LOCATOR_COMBOBOX, wxSize(180,-1));


   //----------------------------------------------------------------------
   // Append flag
   //----------------------------------------------------------------------
   appendCheckBox =
      new wxCheckBox(this, ID_APPEND_CHECKBOX, gmatwxT(GUI_ACCEL_KEY"Append"),
      wxDefaultPosition, wxSize(-1, -1), bsize);
   appendCheckBox->SetToolTip(_T("Append data to existing file"));

   // add locator label and combobox to sizer
   pageFlexGridSizer->Add( locatorLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   pageFlexGridSizer->Add( locatorCB, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   // Add the append toggle
   pageFlexGridSizer->Add(appendCheckBox, 0, wxGROW | wxALIGN_LEFT | wxALL, bsize);

   // add to middle sizer
   theMiddleSizer->Add(pageFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void FindEventsPanel::LoadData()
{
   // load data from the core engine
   Integer id;
   int index;

   // Set the pointer for the "Show Script" button
   mObject = theCommand;

   // locator
   id = theCommand->GetParameterID("EventLocator");
   std::string locator     = theCommand->GetStringParameter(id);
   StringArray locatorList = theGuiInterpreter->GetListOfObjects(Gmat::EVENT_LOCATOR);
   index = 0;
   #ifdef DEBUG_FINDEVENTS_LOAD
      MessageInterface::ShowMessage("In FindEvents::LoadData, locator = **%s**\n", locator.c_str());
      MessageInterface::ShowMessage("     and list of event locators is:\n");
      for (Integer ii = 0; ii < locatorList.size(); ii++)
         MessageInterface::ShowMessage("    -- %s\n", locatorList.at(ii).c_str());
   #endif
   for (StringArray::iterator iter = locatorList.begin();
        iter != locatorList.end(); ++iter)
   {
      #ifdef DEBUG_FINDEVENTS_LOAD
         MessageInterface::ShowMessage("     iter = %s\n", (*iter).c_str());
      #endif
      if (locator == *iter)
      {
         locatorCB->SetSelection(index);
         #ifdef DEBUG_FINDEVENTS_LOAD
            MessageInterface::ShowMessage("Set selection to %d\n", index);
         #endif
      }
      else
         ++index;
   }

   if (!locatorCB->SetStringSelection(locator.c_str()))
   {
          locatorCB->Append("Select an event locator");
          locatorCB->SetStringSelection("Select an event locator");
   }

   appendCheckBox->SetValue((wxVariant(theCommand->GetBooleanParameter("Append"))));
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void FindEventsPanel::SaveData()
{
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
        // save data to core engine
        Integer id;
        wxString elemString;

        // save burn
        wxString locatorString = locatorCB->GetStringSelection();
        id = theCommand->GetParameterID("EventLocator");
        std::string locator = std::string (locatorString.c_str());
        theCommand->SetStringParameter(id, locator);

        if (appendCheckBox->IsChecked())
           theCommand->SetBooleanParameter("Append", true);
        else
           theCommand->SetBooleanParameter("Append", false);

   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
