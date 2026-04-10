//$Id$
//------------------------------------------------------------------------------
//                              TankAndMixDialog
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
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares TankAndMixDialog class. This class shows a dialog window on
 * which celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef TankAndMixDialog_hpp
#define TankAndMixDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "gmatdefs.hpp"
#include <wx/string.h> // for wxArrayString

class TankAndMixDialog : public GmatDialog
{
public:
    
   TankAndMixDialog(wxWindow *parent, wxArrayString &currentTanks,
         wxArrayDouble &currentMixes);
   ~TankAndMixDialog();
   
   wxArrayString& GetTankNames()
      { return tankNames; }
   wxArrayDouble& GetMixValues()
      { return mixValues; }
      
   bool UpdateTankAndMixArrays(wxArrayString &currentTanks,
         wxArrayDouble &currentMixes);

private:
   wxArrayString selectedTanks;
   wxArrayString tankNames;
   wxArrayDouble mixValues;

   wxButton *addTankButton;
   wxButton *removeTankButton;
   wxButton *clearTankButton;
   
   wxListBox *tankListBox;
   wxGrid    *tankMixGrid;
//   wxListBox *tankSelectedListBox;
//   wxListBox *mixValueListBox;
   
   wxString selectedTankName;

   wxGridCellFloatRenderer *theRenderer;
   wxGridCellFloatEditor *theMixEditor;

   bool hasTankChanged;

   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnButton(wxCommandEvent& event);
   void OnSelectTank(wxCommandEvent& event);
   void OnListBoxDoubleClick(wxCommandEvent& event);
   
   void ShowTankOption(const wxString &name, bool show);

   DECLARE_EVENT_TABLE()
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif
