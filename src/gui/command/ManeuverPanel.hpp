//$Id$
//------------------------------------------------------------------------------
//                              ManeuverPanel
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
// Author: LaMont Ruley
// Created: 2003/12/01
// Modification: 2004/10/20 Renamed from ManeuverSetupPanel
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ManeuverPanel_hpp
#define ManeuverPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class ManeuverPanel : public GmatPanel
{
public:
   // constructors
   ManeuverPanel(wxWindow *parent, GmatCommand *cmd);
   ~ManeuverPanel();
   
protected:
   // member data
   GmatCommand *theCommand;

   wxComboBox *burnCB;
   wxComboBox *satCB;
   wxCheckBox *backpropCheckBox;

    
   // member functions
   void OnBurnComboBoxChange(wxCommandEvent& event);
   void OnSatComboBoxChange(wxCommandEvent& event);
   void OnBackpropCheckBoxChange(wxCommandEvent &event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,
        
      ID_BURN_COMBOBOX,
      ID_SAT_COMBOBOX,
      ID_BACKPROP_CHECKBOX
   };
};

#endif // ManeuverPanel_hpp
