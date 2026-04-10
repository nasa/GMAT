//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
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
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef FiniteBurnSetupPanel_hpp
#define FiniteBurnSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "FiniteBurn.hpp"

class FiniteBurnSetupPanel : public GmatPanel
{
public:
   FiniteBurnSetupPanel( wxWindow *parent, const wxString &burnName);
   ~FiniteBurnSetupPanel();

private:
   static const int MAX_PROP_ROW = 5;

   wxArrayString mExcludedThrusterList;
   wxButton *selectButton;
   wxButton *removeButton;
   wxButton *selectAllButton;
   wxButton *removeAllButton;

   wxListBox *availableThrusterListBox;
   wxListBox *selectedThrusterListBox;
   
   bool dataChanged;

   // member data
   FiniteBurn *theBurn;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Event handling
   DECLARE_EVENT_TABLE();
   void OnButtonClick(wxCommandEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
       ID_TEXT = 81000,
       ID_COMBOBOX,
       ID_LISTBOX,
       ID_BUTTON
   };
};

#endif // FiniteBurnSetupPanel_hpp
