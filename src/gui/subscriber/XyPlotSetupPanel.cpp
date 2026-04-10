//$Id$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
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
// Created: 2004/02/02
/**
 * Implements XyPlotSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "XyPlotSetupPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>

//#define DEBUG_XYPLOT_PANEL 1
//#define DEBUG_XYPLOT_PANEL_LOAD 1
//#define DEBUG_XYPLOT_PANEL_SAVE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BUTTON, XyPlotSetupPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBOBOX, XyPlotSetupPanel::OnComboBoxChange)
   EVT_CHECKBOX(ID_CHECKBOX, XyPlotSetupPanel::OnCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs XyPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the XyPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
XyPlotSetupPanel::XyPlotSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
   : GmatPanel(parent)
{
   #if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage("XyPlotSetupPanel() entering...\n");
   MessageInterface::ShowMessage("XyPlotSetupPanel() subscriberName = " +
                                 std::string(subscriberName.c_str()) + "\n");
   #endif
   
   Subscriber *subscriber = (Subscriber*)
      theGuiInterpreter->GetConfiguredObject(std::string(subscriberName.c_str()));
   
   mXyPlot = (XyPlot*)subscriber;
   
   mXParamChanged = false;
   mYParamChanged = false;
   mUseUserParam = false;
   mNumXParams = 0;
   mNumYParams = 0;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   mObjectTypeList.Add("FiniteBurn");
   
   Create();
   Show();
   
   // force saving data
   if (mNumXParams == 0 || mNumYParams <= 0)
   {
      EnableUpdate(true);
      mXParamChanged = true;
      mYParamChanged = true;
      showPlotCheckBox->SetValue(true);
   }
   
   // Listen for Spacecraft and Parameter name change
   theGuiManager->AddToResourceUpdateListeners(this);
   
   #if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage
      ("   mNumXParams=%d, mNumYParams=%d, mXParamChanged=%d, mYParamChanged=%d\n",
       mNumXParams, mNumYParams, mXParamChanged, mYParamChanged);
   #endif
}


//------------------------------------------------------------------------------
// ~XyPlotSetupPanel()
//------------------------------------------------------------------------------
XyPlotSetupPanel::~XyPlotSetupPanel()
{
   mObjectTypeList.Clear() ;   
   theGuiManager->RemoveFromResourceUpdateListeners(this);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool XyPlotSetupPanel::PrepareObjectNameChange()
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
void XyPlotSetupPanel::ObjectNameChanged(UnsignedInt type,
                                         const wxString &oldName,
                                         const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("XyPlotPanel::ObjectNameChanged() type=%d, oldName=<%s>, "
       "newName=<%s>, mDataChanged=%d\n", type, oldName.c_str(), newName.c_str(),
       mDataChanged);
   #endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::PARAMETER)
      return;
   
   mXParamChanged = false;
   mYParamChanged = false;
   mUseUserParam = false;
   mNumXParams = 0;
   mNumYParams = 0;
   
   LoadData();
   
   // force saving data
   if (mNumXParams == 0 || mNumYParams <= 0)
   {
      EnableUpdate(true);
      mXParamChanged = true;
      mYParamChanged = true;
      showPlotCheckBox->SetValue(true);
   }
   
   // We don't need to save data if object name changed from the resouce tree
   // while this panel is opened, since base code already has new name
   EnableUpdate(false);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewXButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE, 0, false);
      
      paramDlg.SetParamNameArray(mXvarWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mXParamChanged = true;
         EnableUpdate(true);
         mXvarWxStrings = paramDlg.GetParamNameArray();
         
         if (mXvarWxStrings.Count() > 0)
         {
            mXSelectedListBox->Clear();
            for (unsigned int i=0; i<mXvarWxStrings.Count(); i++)
               mXSelectedListBox->Append(mXvarWxStrings[i]);
         }
         else // no selections
         {
            mXSelectedListBox->Clear();
         }
      }
   }
   else if (event.GetEventObject() == mViewYButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_PLOTTABLE, 0, true);
      
      paramDlg.SetParamNameArray(mYvarWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mYParamChanged = true;
         EnableUpdate(true);
         mYvarWxStrings = paramDlg.GetParamNameArray();
         
         if (mYvarWxStrings.Count() > 0)
         {
            mYSelectedListBox->Clear();
            for (unsigned int i=0; i<mYvarWxStrings.Count(); i++)
               mYSelectedListBox->Append(mYvarWxStrings[i]);
         }
         else // no selections
         {
            mYSelectedListBox->Clear();
         }
      }
   }
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::Create()
{
   wxArrayString emptyList;
   Integer bsize = 2; // border size
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/XY Plot"));
   
   //------------------------------------------------------
   // plot option
   //------------------------------------------------------
   showPlotCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, gmatwxT("Show " GUI_ACCEL_KEY "Plot"),
                     wxDefaultPosition, wxDefaultSize, 0);
   showPlotCheckBox->SetToolTip(pConfig->Read(_T("ShowPlotHint")));
   
   showGridCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, gmatwxT("Show " GUI_ACCEL_KEY "Grid"),
                     wxDefaultPosition, wxDefaultSize, 0);
   showGridCheckBox->SetToolTip(pConfig->Read(_T("ShowGridHint")));
   
   //----- Solver Iteration ComboBox
   wxStaticText *solverIterLabel =
      new wxStaticText(this, -1, gmatwxT(GUI_ACCEL_KEY"Solver Iterations"),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   mSolverIterComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize,
                     emptyList, wxCB_READONLY);
   mSolverIterComboBox->SetToolTip(pConfig->Read(_T("SolverIterationsHint")));
   
   // Get Solver Iteration option list from the Subscriber
   const std::string *solverIterList = Subscriber::GetSolverIterOptionList();
   int count = Subscriber::GetSolverIterOptionCount();
   for (int i=0; i<count; i++)
      mSolverIterComboBox->Append(solverIterList[i].c_str());
   
   wxFlexGridSizer *option2Sizer = new wxFlexGridSizer(2);
   option2Sizer->Add(showPlotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(showGridCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(20, 20);
   option2Sizer->Add(solverIterLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   option2Sizer->Add(mSolverIterComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *optionSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionSizer->Add(option2Sizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   
   //------------------------------------------------------
   // X selected
   //------------------------------------------------------
   
   mXSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(170, 200),
                    emptyList, wxLB_SINGLE|wxLB_HSCROLL);
   mXSelectedListBox->SetToolTip(pConfig->Read(_T("SelectedXHint")));
   
   mViewXButton = new wxButton(this, ID_BUTTON, "Edit " GUI_ACCEL_KEY "X",
                               wxDefaultPosition, wxDefaultSize, 0);
   mViewXButton->SetToolTip(pConfig->Read(_T("SelectXHint")));
   
   GmatStaticBoxSizer *xSelectedBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Selected X");
   
   xSelectedBoxSizer->Add(mXSelectedListBox, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, bsize);
   xSelectedBoxSizer->Add(mViewXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // Y selected
   //------------------------------------------------------
   mYSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(170,200),
                    emptyList, wxLB_SINGLE|wxLB_HSCROLL);
   mYSelectedListBox->SetToolTip(pConfig->Read(_T("SelectedYHint")));
   
   mViewYButton = new wxButton(this, ID_BUTTON, "Edit " GUI_ACCEL_KEY "Y",
                               wxDefaultPosition, wxDefaultSize, 0);
   mViewYButton->SetToolTip(pConfig->Read(_T("SelectYHint")));
   
   GmatStaticBoxSizer *ySelectedBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Selected Y");
   
   ySelectedBoxSizer->Add(mYSelectedListBox, 1, wxALIGN_CENTRE| wxEXPAND |wxALL, bsize);
   ySelectedBoxSizer->Add(mViewYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------
   wxBoxSizer *plotSizer = new wxBoxSizer(wxHORIZONTAL);
   plotSizer->Add(optionSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   plotSizer->Add(xSelectedBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   plotSizer->Add(ySelectedBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(plotSizer, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, bsize);
   
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::LoadData()
{
   #if DEBUG_XYPLOT_PANEL_LOAD
   MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() entered\n");
   #endif
   
   // load data from the core engine
   
   try
   {   
      // Set the pointer for the "Show Script" button
      mObject = mXyPlot;
      
      showPlotCheckBox->SetValue(mXyPlot->IsActive());
      showGridCheckBox->SetValue(mXyPlot->GetBooleanParameter(XyPlot::SHOW_GRID));
      mSolverIterComboBox->
         SetValue(mXyPlot->GetStringParameter(Subscriber::SOLVER_ITERATIONS).c_str());
      
      // Get X parameter
      wxString *xParamNames = new wxString[1];
      xParamNames[0] = mXyPlot->GetStringParameter(XyPlot::XVARIABLE).c_str();
      mXvarWxStrings.Add(xParamNames[0]);
      
      #if DEBUG_XYPLOT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("   xParamNames[0] = %s\n",  xParamNames[0].c_str());
      #endif
      
      if (!xParamNames[0].IsSameAs(""))
      {
         mNumXParams = 1;
         mXSelectedListBox->Set(1, xParamNames);
      }
      
      // get Y parameters
      StringArray yParamList = mXyPlot->GetStringArrayParameter(XyPlot::YVARIABLES);
      mNumYParams = yParamList.size();
      
      #if DEBUG_XYPLOT_PANEL_LOAD
      MessageInterface::ShowMessage("   mNumYParams = %d\n", mNumYParams);
      #endif
      
      delete [] xParamNames;
      
      if (mNumYParams > 0)
      {
         wxString *yParamNames = new wxString[mNumYParams];
         
         for (int i=0; i<mNumYParams; i++)
         {
            #if DEBUG_XYPLOT_PANEL_LOAD
               MessageInterface::ShowMessage
                  ("   yParamList[%d] = %s\n", i, yParamList[i].c_str());
            #endif
            yParamNames[i] = yParamList[i].c_str();
            mYvarWxStrings.Add(yParamNames[i]);
         }
         
         mYSelectedListBox->Set(mNumYParams, yParamNames);
         mYSelectedListBox->SetSelection(0);
         delete [] yParamNames;
         
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_XYPLOT_PANEL_LOAD
   MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::SaveData()
{
   #if DEBUG_XYPLOT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("XyPlotSetupPanel::SaveData() entered, mNumXParams=%d, mNumYParams=%d, "
       "mXParamChanged=%d, mYParamChanged=%d\n",
       mNumXParams, mNumYParams, mXParamChanged, mYParamChanged);
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Subscriber *clonedObj = (Subscriber*)mXyPlot->Clone();
      
      clonedObj->Activate(showPlotCheckBox->IsChecked());
      clonedObj->SetBooleanParameter(XyPlot::SHOW_GRID, showGridCheckBox->IsChecked());
      clonedObj->SetStringParameter(Subscriber::SOLVER_ITERATIONS,
                                    mSolverIterComboBox->GetValue().c_str());
      
      // set X parameter
      if (mXParamChanged)
      {
         if (mXSelectedListBox->GetCount() == 0 && showPlotCheckBox->IsChecked())
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "X parameter not selected. "
                "The plot will not be activated.");
            clonedObj->Activate(false);
         }
         else
         {
            std::string selXName = std::string(mXSelectedListBox->GetString(0).c_str());
            clonedObj->SetStringParameter(XyPlot::XVARIABLE, selXName);
         }
      }
      
      // set Y parameters
      if (mYParamChanged)
      {
         int numYParams = mYSelectedListBox->GetCount();
         
         mNumYParams = numYParams;
         if (mNumYParams == 0 && showPlotCheckBox->IsChecked())
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,"Y parameters not selected. "
                "The plot will not be activated.");
            clonedObj->Activate(false);
         }
         else
         {
            mNumYParams = numYParams;
         }
         
         if (mNumYParams >= 0) // >=0 because the list needs to be cleared
         {
            clonedObj->TakeAction("Clear");
            for (int i=0; i<mNumYParams; i++)
            {
               #if DEBUG_XYPLOT_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("XyPlotSetupPanel::SaveData() yVar = %s\n",
                   mYSelectedListBox->GetString(i).c_str());
               #endif
               std::string selYName = std::string(mYSelectedListBox->GetString(i).c_str());
               clonedObj->SetStringParameter(XyPlot::YVARIABLES, selYName, i);
            }
         }
      }
      
      if (mXParamChanged || mYParamChanged)
      {
         #if DEBUG_XYPLOT_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   Calling theGuiInterpreter->ValidateSubscriber()\n");
         #endif
         theGuiInterpreter->ValidateSubscriber(clonedObj);
         mXParamChanged = false;
         mYParamChanged = false;
      }
      
      mXyPlot->Copy(clonedObj);
      delete clonedObj;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #if DEBUG_XYPLOT_PANEL_SAVE
   MessageInterface::ShowMessage("XyPlotSetupPanel::SaveData() leaving\n");
   #endif
}

