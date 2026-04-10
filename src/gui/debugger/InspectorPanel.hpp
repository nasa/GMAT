//$Id$
//------------------------------------------------------------------------------
//                           InspectorPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract, Task Order 112
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Apr 20, 2021
/**
 * The GMAT debugger panel, used to view run time objects and step through a
 * control sequence.
 *
 * Parts of this code are derived from Thinking Systems' Astrodynamics
 * Workbench, and used by permission
 */
//------------------------------------------------------------------------------

#ifndef SRC_GUI_DEBUGGER_INSPECTORPANEL_HPP_
#define SRC_GUI_DEBUGGER_INSPECTORPANEL_HPP_

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"

class Moderator;
class ScriptInterpreter;
class GmatMainFrame;
class GmatCommand;


/**
 * The Inspector panel is the GMAT debugger panel that drives script debugging.
 */
class InspectorPanel : public wxDialog
{
public:
   InspectorPanel();
//   virtual ~InspectorPanel();

   void SetCommand(GmatCommand *cmd);

   DECLARE_EVENT_TABLE()

private:
   Moderator *theModerator;
   ScriptInterpreter *theInterpreter;
   GmatMainFrame *theMainFrame;

   GmatCommand *currentcmd;

   wxRadioButton *scrb;
   wxRadioButton *allrb;
   wxStaticText *cmdText;
   wxComboBox *choices;
   Gmat::RunState theRunState;
   wxButton *stepButton;
   wxButton *closeButton;
   wxButton *endButton;
   wxTextCtrl *theScript;

   // Arrays used for the selection combobox
   wxArrayString satList;
   wxArrayString allObjects;
   wxArrayString currentObjects;

   // Current settings
   bool showingSpacraft;
   wxString currentObject;
   wxString text;
   GmatBase *theObject;
   std::string generatingString;

   // Event handlers
   void SelectObject(wxCommandEvent& WXUNUSED(event));
   void Step(wxCommandEvent& WXUNUSED(event));
   void End(wxCommandEvent& WXUNUSED(event));
   void Close(wxCommandEvent& WXUNUSED(event));
   void SetSelectionList(wxCommandEvent& WXUNUSED(event));
   void Close(wxCloseEvent& WXUNUSED(event));

   void PopulateLists();
   void UpdateText();
   void BuildText();

   enum
   {
      // declares an id which will be used to call our button
      RADIO_Spacecraft = wxID_HIGHEST + 1,
      RADIO_All,
      CMD_Text,
      COMBO_Selection,
      BUTTON_Step,
      BUTTON_End,
      BUTTON_Close
   };
};

#endif /* SRC_GUI_DEBUGGER_INSPECTORPANEL_HPP_ */
