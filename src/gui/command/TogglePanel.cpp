//$Id$
//------------------------------------------------------------------------------
//                              TogglePanel
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
// Created: 2004/10/20
//
/**
 * This class contains the Toggle setup window.
 */
//------------------------------------------------------------------------------

#include "TogglePanel.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_TOGGLE_PANEL 1


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TogglePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_RADIOBUTTON(ID_RADIOBUTTON, TogglePanel::OnRadioButtonChange)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, TogglePanel::OnCheckListBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TogglePanel(wxWindow *parent, GmatCommand *cmd, bool forXyPlotOnly, ...)
//------------------------------------------------------------------------------
/**
 * Constructs TogglePanel object. This panel is used for Toggle and XYPlot
 * specific commands such as MarkPoint and ClearPlot.
 *
 * @param  parent input parent.
 * @param  cmd    Toggle command pointer
 * @param  forXyPlotOnly  true if this panel is for XYPlot only.
 * @param  showToggleState  true if toggle state should be shown
 */
//------------------------------------------------------------------------------
TogglePanel::TogglePanel(wxWindow *parent, GmatCommand *cmd, bool forXyPlotOnly,
                         bool showToggleState)
   : GmatPanel(parent)
{
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::TogglePanel() entered, parent=<%p>, cmd=<%p>, forXyPlotOnly=%d\n",
       parent, cmd, forXyPlotOnly);
   #endif
   
   theCommand = cmd;
   isForXyPlotOnly = forXyPlotOnly;
   mShowToggleState = showToggleState;
   
   if (theCommand != NULL)
   {
      Create();
      Show();
   }
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage("TogglePanel::TogglePanel() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~TogglePanel()
//------------------------------------------------------------------------------
TogglePanel::~TogglePanel()
{
   if (isForXyPlotOnly)
      theGuiManager->UnregisterCheckListBox("XYPlot", mSubsCheckListBox);
   else
      theGuiManager->UnregisterCheckListBox("Subscriber", mSubsCheckListBox);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const wxString &action)
//------------------------------------------------------------------------------
bool TogglePanel::TakeAction(const wxString &action)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("TogglePanel::TakeAction() for '%s' entered, action = '%s'\n",
       mCmdTypeName.c_str(), action.c_str());
   #endif
   
   if (action == "EnableUpdate")
      EnableUpdate(true);
   
   return true;
}


//---------------------------------
// protected methods
//---------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel for the Toggle command
 */
//------------------------------------------------------------------------------
void TogglePanel::Create()
{
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage("TogglePanel::Create() entered\n");
   #endif
   
   int bsize = 2;
   mCmdTypeName = theCommand->GetTypeName().c_str();
   
   // create object label
   wxStaticText *objectLabel =
      new wxStaticText(this, ID_TEXT, wxT("Select Subscribers to " + mCmdTypeName),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // create subscriber check list box
   if (isForXyPlotOnly)
      mSubsCheckListBox =
         theGuiManager->GetXyPlotCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,-1));
   else
   {
	   if ((theCommand->GetTypeName() == "PenUp") ||
		   (theCommand->GetTypeName() == "PenDown"))
		  mSubsCheckListBox =
			 theGuiManager->GetSubscriberCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,-1), true);
	   else
		  mSubsCheckListBox =
			 theGuiManager->GetSubscriberCheckListBox(this, ID_CHECKLISTBOX, wxSize(150,-1));
   }
   
   // On or Off button
   if (mShowToggleState)
   {
      mOnRadioButton =
         new wxRadioButton(this, ID_RADIOBUTTON, wxT("On"),
                           wxDefaultPosition, wxDefaultSize, 0);
      
      mOffRadioButton =
         new wxRadioButton(this, ID_RADIOBUTTON, wxT("Off"),
                           wxDefaultPosition, wxDefaultSize, 0);
   }
   
   // create sizers
   wxBoxSizer *buttonSizer = NULL;
   wxBoxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
   if (mShowToggleState)
   {
      buttonSizer = new wxBoxSizer(wxHORIZONTAL);
      buttonSizer->Add(mOnRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
      buttonSizer->Add(mOffRadioButton, 0, wxALIGN_CENTER | wxALL, bsize);
   }
   
   pageSizer->Add(objectLabel, 0, wxALIGN_CENTER | wxALL, bsize);
   pageSizer->Add(mSubsCheckListBox, 0, wxALIGN_CENTER | wxALL, bsize);
   if (mShowToggleState)
      pageSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   
   // add to middle sizer
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage("TogglePanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void TogglePanel::LoadData()
{
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::LoadData() '%s' entered, mShowToggleState=%d\n",
       theCommand->GetTypeName().c_str(), mShowToggleState);
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   try
   {
      // get subscriber names 
      StringArray subNames = theCommand->GetRefObjectNameArray(Gmat::SUBSCRIBER);
      int subsize = subNames.size();
      #if DEBUG_TOGGLE_PANEL
      MessageInterface::ShowMessage("   There are %d subscribers\n", subsize);
      for (unsigned int i=0; i<subNames.size(); i++)
         MessageInterface::ShowMessage
            ("      subNames[%d]='%s'\n", i, subNames[i].c_str());
      #endif
      
      std::string toggleState;
      if (mShowToggleState)
      {
         toggleState = theCommand->GetStringParameter
            (theCommand->GetParameterID("ToggleState"));
         
         #if DEBUG_TOGGLE_PANEL
         for (unsigned int i=0; i<subNames.size(); i++)
            MessageInterface::ShowMessage
               ("   subName[%d]=%s, toggleState=%s\n", i,
                subNames[i].c_str(), toggleState.c_str());
         #endif
      }
      
      // Initialize check list box
      std::string name;
      for (UnsignedInt i=0; i<mSubsCheckListBox->GetCount(); i++)
      {
         name = mSubsCheckListBox->GetString(i).c_str();
         #if DEBUG_TOGGLE_PANEL
         MessageInterface::ShowMessage("   available name = '%s'\n", name.c_str());
         #endif
         for (int j=0; j<subsize; j++)
         {
            if (name == subNames[j])
            {
               mSubsCheckListBox->Check(i);
               break;
            }
         }
      }
      
      if (mShowToggleState)
      {
         if (toggleState == "On")
            mOnRadioButton->SetValue(true);
         else
            mOffRadioButton->SetValue(true);
      }
   }
   catch (GmatBaseException &ex)
   {
      MessageInterface::ShowMessage(ex.GetFullMessage() + "\n");
   }
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage("TogglePanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void TogglePanel::SaveData()
{
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage
      ("TogglePanel::SaveData() '%s' entered, mShowToggleState=%d\n",
       theCommand->GetTypeName().c_str(), mShowToggleState);
   #endif
   
   int count = mSubsCheckListBox->GetCount();
   int checkedCount = 0;
   std::string onOff = "Off";
   
   if (mShowToggleState)
      onOff = mOnRadioButton->GetValue() ? "On" : "Off";
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check for number of subscribers checked
   //-----------------------------------------------------------------
   for (int i=0; i<count; i++)
      if (mSubsCheckListBox->IsChecked(i))
         checkedCount++;
   
   if (checkedCount == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Please select one or more subscribers to %s.", theCommand->GetTypeName().c_str());
      canClose = false;
      return;
   }
   
   
   //-----------------------------------------------------------------
   // save values to base, base code should do any range checking
   //-----------------------------------------------------------------
   
   try
   {
      theCommand->TakeAction("Clear");
      
      std::string subName;
      //std::string cmdStr = theCommand->GetTypeName();
      for (int i=0; i<count; i++)
      {
         subName = mSubsCheckListBox->GetString(i);
         #if DEBUG_TOGGLE_PANEL
         MessageInterface::ShowMessage("   name = '%s' and ", subName.c_str());
         #endif
         if (mSubsCheckListBox->IsChecked(i))
         {
            #if DEBUG_TOGGLE_PANEL
            MessageInterface::ShowMessage("it is checked\n");
            #endif
            
            //cmdStr = cmdStr + " " + subName;            
            theCommand->SetStringParameter(theCommand->GetParameterID("Subscriber"), subName);
         }
         else
         {
            #if DEBUG_TOGGLE_PANEL
            MessageInterface::ShowMessage("it is not checked\n");
            #endif
         }
      }

      #if 0
      if (mShowToggleState)
         cmdStr = cmdStr + " " + onOff;
      cmdStr = cmdStr + ";";
      
      #if DEBUG_TOGGLE_PANEL
      MessageInterface::ShowMessage("   cmdStr = '%s'\n", cmdStr.c_str());
      #endif
      #endif
      
      if (mShowToggleState)
         theCommand->SetStringParameter(theCommand->GetParameterID("ToggleState"), onOff);
      
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   
   #if DEBUG_TOGGLE_PANEL
   MessageInterface::ShowMessage("TogglePanel::SaveData() leaving\n");
   #endif
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnRadioButtonChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnRadioButtonChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void TogglePanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


