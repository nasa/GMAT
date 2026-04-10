//$Id$
//------------------------------------------------------------------------------
//                         DynamicDataSettingsDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2018/02/20, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines DynamicDataSettingsDialog methods
*/
//------------------------------------------------------------------------------
#include "DynamicDataSettingsDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "ParameterSelectDialog.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(DynamicDataSettingsDialog, GmatDialog)
EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
EVT_BUTTON(ID_BUTTON_SELECT, DynamicDataSettingsDialog::OnSelect)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DynamicDataSettingsDialog()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
DynamicDataSettingsDialog::DynamicDataSettingsDialog(wxWindow *parent,
   DDD currDataStruct) : GmatDialog(parent, -1, wxString(_T("New Data")))
{
   currParamSettings = currDataStruct;
   mWasDataSaved = false;

   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("SpacePoint");
   mObjectTypeList.Add("ImpulsiveBurn");
   mObjectTypeList.Add("FiniteBurn");

   Create();
   ShowData();
}

//------------------------------------------------------------------------------
// DDD GetParamSettings()
//------------------------------------------------------------------------------
/**
* Method to get struct of the new parameter settings
*
* @return The new struct of parameter settings
*/
//------------------------------------------------------------------------------
DDD DynamicDataSettingsDialog::GetParamSettings()
{
   return newParamSettings;
}

//------------------------------------------------------------------------------
// bool WasDataSaved()
//------------------------------------------------------------------------------
/**
* Boolean method showing whether or not the data was saved by clicking Ok or
* discarded by clicking cancel
*
* @return Whether the data settings were saved or not
*/
//------------------------------------------------------------------------------
bool DynamicDataSettingsDialog::WasDataSaved()
{
   return mWasDataSaved;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void DynamicDataSettingsDialog::Create()
{
   int bsize = 5;

   wxStaticText *paramNameStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Name",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *textColorStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Text color",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *cellBackgroundColorStaticText = 
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Cell background color",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *warnBoundsStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Warning bounds",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *critBoundsStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Critical bounds",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *lowerBoundsStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Lower",
      wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *upperBoundsStaticText =
      new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY "Upper",
      wxDefaultPosition, wxDefaultSize, 0);

   mParamNameTextCtrl = new wxTextCtrl(this, ID_DISPLAYTEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(130, -1), wxTE_READONLY);
   mParamTextColorCtrl = new wxColourPickerCtrl(this, ID_COLOR);
   mParamBackgroundColorCtrl = new wxColourPickerCtrl(this, ID_COLOR);
   mWarnLowerBoundTextCtrl = new wxTextCtrl(this, ID_DISPLAYTEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mWarnUpperBoundTextCtrl = new wxTextCtrl(this, ID_DISPLAYTEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mCritLowerBoundTextCtrl = new wxTextCtrl(this, ID_DISPLAYTEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mCritUpperBoundTextCtrl = new wxTextCtrl(this, ID_DISPLAYTEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(130, -1), wxTE_PROCESS_ENTER,
      wxTextValidator(wxGMAT_FILTER_NUMERIC));

   mSelectButton = new wxButton(this, ID_BUTTON_SELECT, wxT("Select"));

   wxFlexGridSizer *optionsSizer = new wxFlexGridSizer(3);
   optionsSizer->Add(paramNameStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mParamNameTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mSelectButton, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(textColorStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mParamTextColorCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(0, 0, 0, bsize);
   optionsSizer->Add(cellBackgroundColorStaticText, 0, wxALIGN_CENTER | wxALL,
      bsize);
   optionsSizer->Add(mParamBackgroundColorCtrl, 0, wxALIGN_CENTER | wxALL,
      bsize);
   optionsSizer->Add(0, 0, 0, bsize);
   optionsSizer->Add(0, 0, 0, bsize);
   optionsSizer->Add(20, 40);
   optionsSizer->Add(20, 40);
   optionsSizer->Add(20, 40);
   optionsSizer->Add(lowerBoundsStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(upperBoundsStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(warnBoundsStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mWarnLowerBoundTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mWarnUpperBoundTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(critBoundsStaticText, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mCritLowerBoundTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   optionsSizer->Add(mCritUpperBoundTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);

   GmatStaticBoxSizer *optionListSizer = new GmatStaticBoxSizer(wxVERTICAL,
      this, "Data Options");
   optionListSizer->Add(optionsSizer, 0, wxALIGN_CENTER | wxGROW, 0);

   theMiddleSizer->Add(optionListSizer, 0, wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void DynamicDataSettingsDialog::LoadData()
{
   mParamNameTextCtrl->SetValue(currParamSettings.paramName);

   RgbColor paramTextColor = RgbColor(currParamSettings.paramTextColor);
   wxColour paramWxTextColor = wxColour(paramTextColor.Red(),
      paramTextColor.Green(), paramTextColor.Blue());
   mParamTextColorCtrl->SetColour(paramWxTextColor);

   RgbColor paramBackgroundColor =
      RgbColor(currParamSettings.paramBackgroundColor);
   wxColour paramWxBackgroundColor = wxColour(paramBackgroundColor.Red(),
      paramBackgroundColor.Green(), paramBackgroundColor.Blue());
   mParamBackgroundColorCtrl->SetColour(paramWxBackgroundColor);

   wxString boundValue;
   boundValue = GmatStringUtil::ToString(currParamSettings.warnLowerBound);
   mWarnLowerBoundTextCtrl->SetValue(boundValue);
   boundValue = GmatStringUtil::ToString(currParamSettings.warnUpperBound);
   mWarnUpperBoundTextCtrl->SetValue(boundValue);
   boundValue = GmatStringUtil::ToString(currParamSettings.critLowerBound);
   mCritLowerBoundTextCtrl->SetValue(boundValue);
   boundValue = GmatStringUtil::ToString(currParamSettings.critUpperBound);
   mCritUpperBoundTextCtrl->SetValue(boundValue);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void DynamicDataSettingsDialog::SaveData()
{
   canClose = true;

   newParamSettings.paramName = mParamNameTextCtrl->GetValue();

   wxColour wxcolor = mParamTextColorCtrl->GetColour();
   RgbColor color = RgbColor(wxcolor.Red(), wxcolor.Green(), wxcolor.Blue());
   std::string colorStr = RgbColor::ToRgbString(color.GetIntColor());
   newParamSettings.paramTextColor = RgbColor::ToIntColor(colorStr);

   if (color.GetIntColor() == 0)
      newParamSettings.isTextColorUserSet = false;
   else
      newParamSettings.isTextColorUserSet = true;

   wxcolor = mParamBackgroundColorCtrl->GetColour();
   color = RgbColor(wxcolor.Red(), wxcolor.Green(), wxcolor.Blue());
   colorStr = RgbColor::ToRgbString(color.GetIntColor());
   newParamSettings.paramBackgroundColor = RgbColor::ToIntColor(colorStr);

   GmatStringUtil::ToReal(mWarnLowerBoundTextCtrl->GetValue(),
      newParamSettings.warnLowerBound);
   GmatStringUtil::ToReal(mWarnUpperBoundTextCtrl->GetValue(),
      newParamSettings.warnUpperBound);
   GmatStringUtil::ToReal(mCritLowerBoundTextCtrl->GetValue(),
      newParamSettings.critLowerBound);
   GmatStringUtil::ToReal(mCritUpperBoundTextCtrl->GetValue(),
      newParamSettings.critUpperBound);

   try
   {
      if (newParamSettings.warnLowerBound > newParamSettings.warnUpperBound)
      {
         throw SubscriberException("The new lower warning bound set for \"" +
            newParamSettings.paramName + "\" is greater than the new upper "
            "warning bound");
      }
      if (newParamSettings.critLowerBound > newParamSettings.critUpperBound)
      {
         throw SubscriberException("The new lower critical bound set for \"" +
            newParamSettings.paramName + "\" is greater than the new upper "
            "critical bound");
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }

   mWasDataSaved = true;
}

//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
void DynamicDataSettingsDialog::ResetData()
{
   mWasDataSaved = false;
}

//------------------------------------------------------------------------------
// void OnSelect()
//------------------------------------------------------------------------------
/**
* Method for event when user clicks the "select" button.  This brings up the
* parameter selection window for the user to select a parameter for their
* selected cell in the DynamicDataDisplay GUI.
*/
//------------------------------------------------------------------------------
void DynamicDataSettingsDialog::OnSelect(wxCommandEvent& event)
{
   ParameterSelectDialog paramSelect(this, mObjectTypeList,
      GuiItemManager::SHOW_REPORTABLE, 0, false, true, true, true, true, true);
   paramSelect.ShowModal();
   mParamNameTextCtrl->SetValue(paramSelect.GetParamName());
}