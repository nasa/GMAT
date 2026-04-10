//$Id$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
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
// Created: 2004/02/02
/**
 * Declares XyPlotSetupPanel class. This class allows user to setup XY Plot.
 */
//------------------------------------------------------------------------------
#ifndef XyPlotSetupPanel_hpp
#define XyPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "XyPlot.hpp"

class XyPlotSetupPanel: public GmatPanel
{
public:
   XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   ~XyPlotSetupPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(UnsignedInt type,
                                  const wxString &oldName,
                                  const wxString &newName);
protected:
   XyPlot *mXyPlot;
   
   int  mNumXParams;
   int  mNumYParams;
   bool mXParamChanged;
   bool mYParamChanged;
   bool mUseUserParam;
   
   std::string mSelYName;
   wxArrayString mObjectTypeList;
   wxArrayString mXvarWxStrings;
   wxArrayString mYvarWxStrings;
   
   wxStaticText *mCoordSysLabel;
   wxComboBox *mSolverIterComboBox;   
   wxListBox *mXSelectedListBox;
   wxListBox *mYSelectedListBox;   
   wxButton *mViewXButton;
   wxButton *mViewYButton;
   wxCheckBox *showPlotCheckBox;
   wxCheckBox *showGridCheckBox;
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 92000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_LISTBOX,
      ID_BUTTON,
   };
   
};
#endif
