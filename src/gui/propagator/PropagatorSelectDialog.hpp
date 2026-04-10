//$Id$
//------------------------------------------------------------------------------
//                              PropagatorSelectDialog
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
// Created: 2004/10/19
//
/**
 * Declares PropagatorSelectDialog class. This class shows dialog window where
 * propagator can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef PropagatorSelectDialog_hpp
#define PropagatorSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h>    // for wxArrayString

class PropagatorSelectDialog : public GmatDialog
{
public:
   
   PropagatorSelectDialog(wxWindow *parent, const wxString &propName);
   
   bool HasSelectionChanged();
   wxString GetPropagatorName();
   
private:

   bool mHasSelectionChanged;
   wxString mPropName;
   wxString mNewPropName;
   
   wxButton *mAddButton;
   wxButton *mCancelButton;
   
   wxListBox *mPropagatorListBox;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif



