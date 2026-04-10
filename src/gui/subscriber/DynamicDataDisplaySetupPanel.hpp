//$Id$
//------------------------------------------------------------------------------
//                         DynamicDataDisplaySetupPanel
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
* Declares DynamicDataDisplaySetupPanel methods
*/
//------------------------------------------------------------------------------
#ifndef DynamicDataDisplaySetupPanel_hpp
#define DynamicDataDisplaySetupPanel_hpp

#include "GmatPanel.hpp"
#include "DynamicDataDisplay.hpp"
#include "DynamicDataStruct.hpp"
#include "DynamicDataSettingsDialog.hpp"
#include "GuiInterpreter.hpp"
#include <wx/clrpicker.h> // for wxColorPickerCtrl

class DynamicDataDisplaySetupPanel : public GmatPanel
{
public:
   DynamicDataDisplaySetupPanel(wxWindow *parent, const wxString &name);

private:
   DynamicDataDisplay *mDisplay;
   std::vector<std::vector<DDD>> displayDataStruct;
   wxString mDisplayName;

   Integer mNumRows;
   Integer mNumCols;
   UnsignedInt mIntWarnColor;
   UnsignedInt mIntCritColor;
   bool mIsCellValueChanged;

   wxTextCtrl *mDisplayRowTextCtrl;
   wxTextCtrl *mDisplayColTextCtrl;
   wxColourPickerCtrl *mWarnColorPickerCtrl;
   wxColourPickerCtrl *mCritColorPickerCtrl;

   wxButton *mUpdateButton;

   wxGrid *mDisplayDataGrid;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // event handling
   void OnUpdate(wxCommandEvent& event);
   void OnGridCellDClick(wxGridEvent& event);
   void OnGridTabbing(wxGridEvent& event);
   void OnColorPickerChange(wxColourPickerEvent& event);
   void OnDelete(wxKeyEvent& event);

   void SetParamDefaultValues(DDD &paramSettings);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON_UPDATE,
      ID_TEXTCTRL,
      ID_COLOR
   };
};
#endif