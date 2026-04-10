//$Id$
//------------------------------------------------------------------------------
//                            PowerSystemPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// Created: 2014.05.07
/**
 * This class contains information needed to setup users spacecraft power
 * system through GUI
 *
 */
//------------------------------------------------------------------------------
#ifndef PowerSystemPanel_hpp
#define PowerSystemPanel_hpp

#include "gmatwxdefs.hpp"
#include "Spacecraft.hpp"
#include "GmatPanel.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"


class PowerSystemPanel: public wxPanel
{
public:
   PowerSystemPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~PowerSystemPanel();

   void SaveData();
   void LoadData();

   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }

private:
   bool dataChanged;
   bool canClose;

   bool powerSystemChanged;

   void Create();

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnComboBoxChange(wxCommandEvent& event);

   Spacecraft     *theSpacecraft;
   GuiItemManager *theGuiManager;
   GuiInterpreter *theGuiInterpreter;

   GmatPanel    *theScPanel;

   wxComboBox   *powerSystemComboBox;

   std::string  thePowerSystem;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30220,
      ID_COMBOBOX
   };
};
#endif
