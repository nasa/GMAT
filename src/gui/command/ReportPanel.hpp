//$Id$
//------------------------------------------------------------------------------
//                              ReportPanel
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
// Created: 2006/08/10
/**
 * Declares ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#ifndef ReportPanel_hpp
#define ReportPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ReportPanel: public GmatPanel
{
public:
   ReportPanel(wxWindow *parent, GmatCommand *cmd);
   ~ReportPanel();
   
protected:
   GmatCommand *theCommand;
   
   int  mNumParameters;
   bool mHasReportFileChanged;
   bool mHasParameterChanged;
   
   wxArrayString mObjectTypeList;
   wxArrayString mReportWxStrings;
   
   wxComboBox *mReportFileComboBox;   
   wxListBox  *mSelectedListBox;
   wxButton   *mViewButton;   
   
   void OnButtonClick(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 93000,
      ID_BUTTON,
      ID_LISTBOX,
      ID_COMBOBOX,
   };

private:
   
};
#endif
