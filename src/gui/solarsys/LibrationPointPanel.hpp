//$Id$
//------------------------------------------------------------------------------
//                              LibrationPointPanel
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
// Created: 2005/04/21
/**
 * This class allows user to configure a LibrationPoint.
 */
//------------------------------------------------------------------------------
#ifndef LibrationPointPanel_hpp
#define LibrationPointPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "LibrationPoint.hpp"


class LibrationPointPanel: public GmatPanel
{
public:
   LibrationPointPanel(wxWindow *parent, const wxString &name);
   ~LibrationPointPanel();
   virtual bool RefreshObjects(UnsignedInt type = Gmat::UNKNOWN_OBJECT);
   
   
private:
   // member data
   LibrationPoint *theLibrationPt;
   LibrationPoint *theClonedLibPoint;
   
   wxComboBox *primaryBodyCB;
   wxComboBox *secondaryBodyCB;
   wxComboBox *librationPtCB;
   
   // member functions
   void OnComboBoxChange(wxCommandEvent& event);
	void UpdateComboBoxes();
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 6150,
      ID_COMBOBOX
   };
};
#endif



