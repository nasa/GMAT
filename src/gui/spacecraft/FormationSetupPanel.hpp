//$Id$
//------------------------------------------------------------------------------
//                              FormationSetupPanel
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
// Created: 2004/02/12 
// Modified: 
//    2010.03.22 Thomas Grubb 
//      - Added AVL_LISTBOX window id to use with Available ListBox double-click
/**
 * Declares FormationSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef FormationSetupPanel_hpp
#define FormationSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "FormationInterface.hpp"

class FormationSetupPanel: public GmatPanel
{
public:
   FormationSetupPanel(wxWindow *parent, const wxString &formationName);
   ~FormationSetupPanel();
   
protected:
   
   std::string mFormationName;
   FormationInterface *mFormation;
   FormationInterface *mClonedFormation;
   
   wxArrayString mSoExcList;
   
   wxListBox *mSoAvailableListBox;
   wxListBox *mSoSelectedListBox;

   void OnAddSpaceObject(wxCommandEvent& event);
   void OnRemoveSpaceObject(wxCommandEvent& event);
   void OnClearSpaceObject(wxCommandEvent& event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ADD_BUTTON = 99000,
      REMOVE_BUTTON,
      CLEAR_BUTTON,
      SEL_LISTBOX,
      AVL_LISTBOX
   };
};
#endif
