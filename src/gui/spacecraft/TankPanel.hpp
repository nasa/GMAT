//$Id$
//------------------------------------------------------------------------------
//                            TankPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef TankPanel_hpp
#define TankPanel_hpp

#include "gmatdefs.hpp"
#include "FuelTank.hpp"
#include "Spacecraft.hpp"
#include "Hardware.hpp"

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"

class TankPanel: public wxPanel
{
public:
   TankPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~TankPanel();
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged; }
   
private:    
   
   bool dataChanged;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnButtonClick(wxCommandEvent &event);
   
   Spacecraft *theSpacecraft;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;

   wxArrayString mExcludedTankList;

   GmatPanel *theScPanel;
   
   wxButton *selectButton;
   wxButton *removeButton;
   wxButton *selectAllButton;
   wxButton *removeAllButton;
   
   wxListBox *availableTankListBox;
   wxListBox *selectedTankListBox;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_LISTBOX = 30200,
      ID_BUTTON,
   };
};
#endif

