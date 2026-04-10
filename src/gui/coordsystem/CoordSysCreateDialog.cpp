//$Id$
//------------------------------------------------------------------------------
//                              CoordSysCreateDialog
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
// Created: 2005/03/03
//
/**
 * Implements CoordSysCreateDialog class. This class shows dialog window where a
 * coordinate system can be created.
 * 
 */
//------------------------------------------------------------------------------

#include "CoordSysCreateDialog.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystem.hpp"
#include "AxisSystem.hpp"
#include "DateUtil.hpp"
#include "StringUtil.hpp"            // for GetInvalidNameMessageFormat()
#include <wx/config.h>

//#define DEBUG_COORD_DIALOG
//#define DEBUG_COORD_DIALOG_CREATE
//#define DEBUG_COORD_DIALOG_LOAD
//#define DEBUG_COORD_DIALOG_SAVE
//#define DEBUG_COORD_DIALOG_XYZ
//#define DEBUG_TEXT_MODS

//------------------------------------------------------------------------------
// event tables and other macros for wxWidgets
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CoordSysCreateDialog, GmatDialog)
   EVT_TEXT(ID_TEXTCTRL, CoordSysCreateDialog::OnTextUpdate)
   EVT_COMBOBOX(ID_COMBO, CoordSysCreateDialog::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CoordSysCreateDialog(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param  parent   the parent window
 */
//------------------------------------------------------------------------------
CoordSysCreateDialog::CoordSysCreateDialog(wxWindow *parent)
: GmatDialog(parent, -1, wxString(_T("New Coordinate System")))
{
   mIsCoordCreated    = false;
   mIsTextModified    = false;
   mIsLACTextModified = false;
   
   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel.
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::Create()
{
   #ifdef DEBUG_COORDDIALOG_CREATE
   MessageInterface::ShowMessage("CoordSysCreateDialog::Create() entered\n");
   #endif
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Coordinate System"));
   
   // wxStaticText
   nameStaticText =
      new wxStaticText( this, ID_TEXT,
                        "Coordinate System " GUI_ACCEL_KEY "Name",
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxTextCtrl
   nameTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                                  wxDefaultPosition, wxSize(160,-1), 0 );
   nameTextCtrl->SetToolTip(pConfig->Read(_T("NameHint")));
   
   mCoordPanel = new CoordPanel(this, true);
   
   // wx*Sizers
   wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );
   int bsize = 2;
   
   boxsizer1->Add( nameStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer1->Add( nameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   
   theMiddleSizer->Add(boxsizer1, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   theMiddleSizer->Add( mCoordPanel, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_COORDDIALOG_CREATE
   MessageInterface::ShowMessage("CoordSysCreateDialog::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data onto the panel.
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::LoadData()
{
   #ifdef DEBUG_COORDDIALOG_LOAD
   MessageInterface::ShowMessage("CoordSysCreateDialog::LoadData() entered\n");
   #endif
   
   try
   {
      epochTextCtrl = mCoordPanel->GetEpochTextCtrl();

      originComboBox       = mCoordPanel->GetOriginComboBox();
      typeComboBox         = mCoordPanel->GetTypeComboBox();
      primaryComboBox      = mCoordPanel->GetPrimaryComboBox();
      secondaryComboBox    = mCoordPanel->GetSecondaryComboBox();
      refObjectComboBox    = mCoordPanel->GetReferenceObjectComboBox();
      constraintCSComboBox = mCoordPanel->GetConstraintCSComboBox();

      xComboBox = mCoordPanel->GetXComboBox();
      yComboBox = mCoordPanel->GetYComboBox();
      zComboBox = mCoordPanel->GetZComboBox();

      mCoordPanel->SetDefaultAxis();
            
      mCoordPanel->EnableOptions();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordSysCreateDialog:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
   
   #ifdef DEBUG_COORDDIALOG_LOAD
   MessageInterface::ShowMessage("CoordSysCreateDialog::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel to the coordinate system object.
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::SaveData()
{
   #ifdef DEBUG_COORD_DIALOG_SAVE
   MessageInterface::ShowMessage
      ("CoordSysCreateDialog::SaveData() entered, mIsTextModified=%d, mIsLACTextModified = %d\n",
       mIsTextModified, mIsLACTextModified);
   #endif
   
   canClose = true;
   
   std::string coordName = std::string(nameTextCtrl->GetValue().Trim().c_str());
   if (coordName == "")
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Please enter Coordinate System name");
      canClose = false;
      return;
   }
   
   // Check if name starts with number or contains non-alphanumeric character
   // Warning message is displayed from UserInputValidator::IsValidName()
   if (!IsValidName(coordName.c_str()))
      return;
   
   // Check if name is any command type name
   if (theGuiInterpreter->IsCommandType(coordName.c_str()))
   {
      std::string format = GmatStringUtil::GetInvalidNameMessageFormat();
      MessageInterface::PopupMessage
         (Gmat::ERROR_, format.c_str(), coordName.c_str());
      canClose = false;
      return;
   }
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      Real epoch;
      std::string str = mCoordPanel->GetEpochTextCtrl()->GetValue().WX_TO_STD_STRING;
      #ifdef DEBUG_COORD_DIALOG_SAVE
      MessageInterface::ShowMessage
         ("CoordSysCreateDialog::SaveData() epoch value = %s\n",
          str.c_str());
      #endif
      bool isValid = CheckReal(epoch, str, "Epoch", "Real Number >= 0");
      #ifdef DEBUG_COORD_DIALOG_SAVE
      MessageInterface::ShowMessage
         ("CoordSysCreateDialog::SaveData() isValid = %s, and epoch real value = %12.10f\n",
          (isValid? "true" : "false"), epoch);
      #endif
      
      // check range here
      
      if (isValid)
         CheckRealRange(str, epoch, "Epoch", DateUtil::EARLIEST_VALID_MJD_VALUE,
                        DateUtil::LATEST_VALID_MJD_VALUE, true, true, true, true);
   }
   
   if (mIsLACTextModified)
   {
      #ifdef DEBUG_TEXT_MODS
         MessageInterface::ShowMessage("calling coordPanel->CheckAlign ...\n");
      #endif
      canClose = canClose && mCoordPanel->CheckAlignmentConstraintValues();
   }

   if (!canClose)
      return;
   
   mIsTextModified    = false;
   mIsLACTextModified = false;
   
   CoordinateSystem *coord =
      (CoordinateSystem*)theGuiInterpreter->GetConfiguredObject(coordName);
   
   if (mIsCoordCreated)
   {
      AxisSystem *axis =
         (AxisSystem *)coord->GetRefObject(Gmat::AXIS_SYSTEM, "");
      #ifdef DEBUG_COORD_DIALOG_SAVE
       MessageInterface::ShowMessage
          ("CoordSysCreateDialog::SaveData() About to call SaveData for mIsCoordCreated\n");
       #endif

      canClose = mCoordPanel->SaveData(coordName, axis, wxFormatName);
   }
   else
   {
      if (coord == NULL)
      {
         // create AxisSystem
         AxisSystem *axis = mCoordPanel->CreateAxis();
         #ifdef DEBUG_COORD_DIALOG_XYZ
            MessageInterface::ShowMessage("CoordSysCreateDialog:: CreateAxis returned %s\n",
                  (axis? "not NULL" : "NULL"));
         #endif
         
         if (axis != NULL)
         {
            #ifdef DEBUG_COORD_DIALOG_XYZ
               MessageInterface::ShowMessage("CoordSysCreateDialog:: executing axis not equal to NULL part ...\n");
            #endif
            canClose        = mCoordPanel->SaveData(coordName, axis, wxFormatName);
            if (!canClose)
            {
               #ifdef DEBUG_COORD_DIALOG_SAVE
               MessageInterface::ShowMessage
                  ("CoordSysCreateDialog::SaveData() Removing coordinate system %s due to error\n", coordName.c_str());
               #endif
               // Remove the object if there was an error creating or initializing it
               theGuiInterpreter->RemoveObjectIfNotUsed(Gmat::COORDINATE_SYSTEM, coordName);
            }
            else
            {
               mCoordName      = wxString(coordName.c_str());
               mIsCoordCreated = true;
            }
         }
         else
         {
            #ifdef DEBUG_COORD_DIALOG_XYZ
               MessageInterface::ShowMessage("CoordSysCreateDialog:: executing axis EQUAL to NULL part ...\n");
            #endif
               MessageInterface::ShowMessage
                  ("CoordSysCreateDialog::SaveData() Cannot create AxisSystem.\n");
            canClose = false;
         }
      }
      else
      {
         wxLogError("\"" + wxString(coordName.c_str()) +
                    "\" already exists. Please enter a different name.\n");
         canClose = false;
      }
   }
   #ifdef DEBUG_COORD_DIALOG_SAVE
   MessageInterface::ShowMessage("CoordSysCreateDialog::SaveData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
/**
 * Resets the data
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::ResetData()
{
   mIsCoordCreated = false;
   mCoordName = "";
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when text is updated.
 *
 * @param  event   command event to handle
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::OnTextUpdate(wxCommandEvent& event)
{
   if (nameTextCtrl->GetValue().Trim() != "")
   {
      EnableUpdate(true);
   }
   
   if (mCoordPanel->GetEpochTextCtrl()->IsModified() )
   {
      EnableUpdate(true);
      mIsTextModified = true;
   }

   if (mCoordPanel->IsAlignmentConstraintTextModified())
   {
      #ifdef DEBUG_TEXT_MODS
         MessageInterface::ShowMessage(" .... setting mIsLACTextModified to true ...\n");
      #endif
      EnableUpdate(true);
      mIsLACTextModified = true;
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when a combo box is updated.
 *
 * @param  event   command event to handle
 */
//------------------------------------------------------------------------------
void CoordSysCreateDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == originComboBox)
   {
   }
   else if (event.GetEventObject() == typeComboBox)
   {
      mCoordPanel->EnableOptions();
   }
   
   if (nameTextCtrl->GetValue().Trim() != "")
      EnableUpdate(true);
}

