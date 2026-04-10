//$Id$
//------------------------------------------------------------------------------
//                              CoordSysCreateDialog
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
// Author: Allison Greene
// Created: 2005/03/03
//
/**
 * Declares CoordSysCreateDialog class. This class shows dialog window where a
 * coordinate system can be created.
 * 
 */
//------------------------------------------------------------------------------
#ifndef CoordSysCreateDialog_hpp
#define CoordSysCreateDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "CoordPanel.hpp"

class CoordSysCreateDialog : public GmatDialog
{
public:
    
   CoordSysCreateDialog(wxWindow *parent);

   wxString& GetCoordName()
      { return mCoordName; }
   bool IsCoordCreated()
      { return mIsCoordCreated; }

protected:
   bool mIsCoordCreated;
   bool mIsTextModified;
   bool mIsLACTextModified;
   
   wxString mCoordName;
   CoordPanel *mCoordPanel;
   
   wxStaticText *nameStaticText;
   wxTextCtrl *nameTextCtrl;
   
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 46000,
      ID_COMBO,
      ID_TEXT,
      ID_BUTTON
   };
   
private:
   wxString wxFormatName;
   
   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *intervalTextCtrl;

   wxComboBox *originComboBox;
   wxComboBox *typeComboBox;
   wxComboBox *primaryComboBox;
   wxComboBox *formatComboBox;
   wxComboBox *secondaryComboBox;
   wxComboBox *refObjectComboBox;
   wxComboBox *constraintCSComboBox;
   
   wxComboBox *xComboBox;
   wxComboBox *yComboBox;
   wxComboBox *zComboBox;
   
};

#endif
