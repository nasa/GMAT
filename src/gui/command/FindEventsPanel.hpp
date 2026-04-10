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
// Created: 2015.03/27
//
/**
 * This class contains the FindEvents Setup window.
 */
//------------------------------------------------------------------------------

#ifndef FindEventsPanel_hpp
#define FindEventsPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class FindEventsPanel : public GmatPanel
{
public:
   // constructors
   FindEventsPanel(wxWindow *parent, GmatCommand *cmd);
   ~FindEventsPanel();

protected:
   // member data
   GmatCommand *theCommand;

   wxComboBox *locatorCB;
   wxCheckBox *appendCheckBox;

   // member functions
   void OnLocatorComboBoxChange(wxCommandEvent& event);
   void OnAppendCheckBoxChange(wxCommandEvent &event);

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
      ID_LOCATOR_COMBOBOX,
      ID_APPEND_CHECKBOX
   };
};

#endif // FindEventsPanel_hpp
