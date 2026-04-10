//$Id$
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
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
// Author: Waka Waktola
// Created: 2004/10/26
/**
 * This class contains the Coordinate System Setup Panel.
 */
//------------------------------------------------------------------------------
#ifndef CoordSystemConfigPanel_hpp
#define CoordSystemConfigPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CoordinateSystem.hpp"
#include "CoordPanel.hpp"

class CoordSystemConfigPanel : public GmatPanel
{
public:
   // constructors
   CoordSystemConfigPanel(wxWindow *parent, const wxString &coordName);
   ~CoordSystemConfigPanel();  
   
private:
   CoordinateSystem *theCoordSys;
   CoordPanel       *mCoordPanel;
   wxString         mEpochFormat;
   bool             mOriginChanged;
   bool             mObjRefChanged;
   bool             mEpochChanged;
   
   wxString   previousType;
   wxString   previousOrigin;

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
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   virtual void ResetFlags();

   // data handling methods
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboUpdate(wxCommandEvent& event);

   // event handling
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
       ID_TEXTCTRL = 46000,
       ID_COMBO,
       ID_TEXT,
   };
};

#endif // CoordSystemConfigPanel_hpp
