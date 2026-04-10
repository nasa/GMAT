//$Id$
//------------------------------------------------------------------------------
//                         ShowScriptDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show the command summary for individual
 * components.
 */
//------------------------------------------------------------------------------

#include "GuiItemManager.hpp"
#include "GmatDialog.hpp"
#include "GmatCommand.hpp"

#ifndef ShowSummaryDialog_hpp
#define ShowSummaryDialog_hpp


/** The modal dialog used to display scripting for a component. 
 *
 * At user request, this dialog was constructed so that the user can select text 
 * and copy it to the clipboard for pasting elsewhere. 
 */
class ShowSummaryDialog : public GmatDialog
{
public:
   // constructors
   ShowSummaryDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
                    GmatCommand *obj, bool summaryForMission = false, bool physicsOnly = false);
 
   void OnSaveAs(wxCommandEvent &event);

   ~ShowSummaryDialog();

protected:
   /// The object that provides the generating string for display.
   GmatCommand *theObject;
   /// The component on the dialog that shows the scripting.
   wxTextCtrl *theSummary;
   wxButton   *theSaveAsButton;
          
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnComboBoxChange(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_CS_TEXT = 46000,
      ID_COMBOBOX,
      ID_BUTTON_SAVEAS
   };



   bool           isCoordSysModified;
   std::string    currentCoordSysName;
   GuiItemManager *theGuiManager;
   bool           summaryForEntireMission;
   bool           physicsBasedOnly;

   wxComboBox     *coordSysComboBox;

   void BuildValidCoordinateSystemList();
};

#endif //ShowScriptDialog_hpp
