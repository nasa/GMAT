//$Id$
//------------------------------------------------------------------------------
//                              ReportPanel
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
// Created: 2006/08/10
/**
 * Implements ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#include "ReportPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "ReportFile.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REPORTPANEL
//#define DEBUG_REPORTPANEL_LOAD
//#define DEBUG_REPORTPANEL_SAVE

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BUTTON, ReportPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBOBOX, ReportPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportPanel(wxWindow *parent, const GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs ReportPanel object.
 *
 * @param <parent> input parent.
 * @param <cmd> input command.
 */
//------------------------------------------------------------------------------
ReportPanel::ReportPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #ifdef DEBUG_REPORTPANEL
   MessageInterface::ShowMessage("ReportPanel() entering...\n");
   MessageInterface::ShowMessage("ReportPanel() cmd = " +
                                 cmd->GetTypeName() + "\n");
   #endif
   
   theCommand = cmd;
   mHasReportFileChanged = false;
   mHasParameterChanged = false;
   
   if (theCommand != NULL)
   {
      mObjectTypeList.Add("Spacecraft");
      mObjectTypeList.Add("SpacePoint");
      mObjectTypeList.Add("ImpulsiveBurn");
      mObjectTypeList.Add("FiniteBurn");
      Create();
      Show();
      EnableUpdate(false);
   }
}


//------------------------------------------------------------------------------
// ~ReportPanel()
//------------------------------------------------------------------------------
ReportPanel::~ReportPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("ReportFile", mReportFileComboBox);   
}


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
void ReportPanel::Create()
{
   #ifdef DEBUG_REPORTPANEL
   MessageInterface::ShowMessage("ReportPanel::Create() entering...\n");
   #endif
   
   Integer bsize = 2; // border size
   wxArrayString emptyList;   
   
   //------------------------------------------------------
   // report file
   //------------------------------------------------------
   wxStaticText *reportFileLabel =
      new wxStaticText(this, -1, wxT("ReportFile: "), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mReportFileComboBox = theGuiManager->
      GetReportFileComboBox(this, ID_COMBOBOX, wxSize(200, -1));
   
   wxBoxSizer *reportFileSizer = new wxBoxSizer(wxHORIZONTAL);
   reportFileSizer->Add(reportFileLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   reportFileSizer->Add(mReportFileComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // selected parameter list
   //-------------------------------------------------------   
   mSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(200, 150),
                    emptyList, wxLB_SINGLE);
   
   mViewButton = new wxButton( this, ID_BUTTON, wxT("View"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   GmatStaticBoxSizer *selectedSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Parameter List");
   
   selectedSizer->Add(mSelectedListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(mViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *panelSizer = new wxFlexGridSizer(2, 1, 0, 0);
   panelSizer->Add(reportFileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   panelSizer->Add(20, 10, 0, wxALIGN_CENTRE|wxALL, bsize);
   panelSizer->Add(selectedSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(panelSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_REPORTPANEL
   MessageInterface::ShowMessage("ReportPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportPanel::LoadData()
{
   #ifdef DEBUG_REPORTPANEL_LOAD
   MessageInterface::ShowMessage("ReportPanel::LoadData() entering...\n");
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      // Get ReportFile name
      std::string rfName = theCommand->GetRefObjectName(Gmat::SUBSCRIBER);
      mReportFileComboBox->SetValue(rfName.c_str());
      
      #ifdef DEBUG_REPORTPANEL_LOAD
      MessageInterface::ShowMessage("   ReportFile name='%s'\n", rfName.c_str());
      #endif
      
      // Get parameters to report
      //StringArray parameterList = theCommand->GetRefObjectNameArray(Gmat::PARAMETER);
      StringArray parameterList = theCommand->GetStringArrayParameter("Add");
      mNumParameters = parameterList.size();
      
      #ifdef DEBUG_REPORTPANEL_LOAD
      MessageInterface::ShowMessage("   mNumParameters=%d\n", mNumParameters);
      #endif
      
      if (mNumParameters > 0)
      {      
         Parameter *param;
         wxString paramName;
         
         for (int i=0; i<mNumParameters; i++)
         {
            param = theGuiInterpreter->GetParameter(parameterList[i]);
            paramName = parameterList[i].c_str();
            mSelectedListBox->Append(paramName);
            mReportWxStrings.Add(paramName);
            #ifdef DEBUG_REPORTPANEL_LOAD
            MessageInterface::ShowMessage("   added %s\n", paramName.c_str());
            #endif
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_REPORTPANEL_LOAD
   MessageInterface::ShowMessage("ReportPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportPanel::SaveData()
{
   #ifdef DEBUG_REPORTPANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() entering...\n");
   #endif
   
   canClose = true;
   
   std::string rfName = mReportFileComboBox->GetValue().WX_TO_STD_STRING;
   
   ReportFile *reportFile =
      (ReportFile*)theGuiInterpreter->GetConfiguredObject(rfName);
   
   #ifdef DEBUG_REPORTPANEL_SAVE
   MessageInterface::ShowMessage
      ("   ReportFile name='%s', addr=%p\n", rfName.c_str(), reportFile);
   #endif
   
   if (reportFile == NULL)
      return;
   
   try
   {
      // save ReportFile
      if (mHasReportFileChanged)
      {
         #ifdef DEBUG_REPORTPANEL_SAVE
         MessageInterface::ShowMessage
            ("    rfName=%s, reportFile=%p\n", rfName.c_str(), reportFile);
         #endif
         
         theCommand->SetRefObject(reportFile, Gmat::SUBSCRIBER, rfName, 0);
         mHasReportFileChanged = false;
      }
      
      // save parameters
      if (mHasParameterChanged)
      {
         mHasParameterChanged = false;
         theCommand->TakeAction("Clear");
         Parameter *param = NULL;
         mNumParameters = mSelectedListBox->GetCount();
         
         #ifdef DEBUG_REPORTPANEL_SAVE
         MessageInterface::ShowMessage("   mNumParameters=%d\n", mNumParameters);
         #endif
         
         for (int i=0; i<mNumParameters; i++)
         {
            std::string selName =
               std::string(mSelectedListBox->GetString(i).c_str());
            
            #ifdef DEBUG_REPORTPANEL_SAVE
            MessageInterface::ShowMessage("   selName='%s'\n", selName.c_str());
            #endif
            
            param = theGuiInterpreter->GetParameter(selName);
            theCommand->SetStringParameter("Add", selName);
            theCommand->SetRefObject(param, Gmat::PARAMETER, selName, i);
         }
         
         // Now validate command and create element wrappers
         if (!theGuiInterpreter->ValidateCommand(theCommand))
            canClose = false;
         
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_REPORTPANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void ReportPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewButton)
   {
      // Note:: 2008.01.23
      // The Report command cannot take Arrays yet, so set allowArray to false
      // 2012.06.05 enabling Arrays since Report command can handle it now.
      // Allow whole array or array element
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_REPORTABLE, 2, true,
                                     true, true, true, true);
      
      paramDlg.SetParamNameArray(mReportWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mHasParameterChanged = true;
         EnableUpdate(true);
         mReportWxStrings = paramDlg.GetParamNameArray();
         
         if (mReportWxStrings.Count() > 0)
         {
            mSelectedListBox->Clear();
            for (unsigned int i=0; i<mReportWxStrings.Count(); i++)
               mSelectedListBox->Append(mReportWxStrings[i]);
         }
         else // no selections
         {
            mSelectedListBox->Clear();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnComboBoxChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (obj == mReportFileComboBox)
   {
      mHasReportFileChanged = true;
      EnableUpdate(true);
   }
}


