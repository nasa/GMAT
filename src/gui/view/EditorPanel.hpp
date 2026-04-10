//$Id$
//------------------------------------------------------------------------------
//                              EditorPanel
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
// Created: 2009/01/15
//
/**
 * Declares EditorPanel class.
 */
//------------------------------------------------------------------------------

#ifndef EditorPanel_hpp
#define EditorPanel_hpp

#include "GmatSavePanel.hpp"
#include "ScriptEditor.hpp"
#include <wx/fontdlg.h>

class EditorPanel: public GmatSavePanel
{
public:
   // default constructor
   EditorPanel(wxWindow *parent, const wxString &name, bool activeScript = false);
   // destructor
   ~EditorPanel();
   
   // method to retrieve the Editor
   ScriptEditor* GetEditor() { return mEditor; };
   void ClickButton( bool run = false );
   
private:
   ScriptEditor   *mEditor;
   
   // methods inherited from GmatSavePanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextOverMaxLen(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   void OnRunButton(wxCommandEvent& event);
   void OnTabNext(wxCommandEvent& event);
   void OnTabPrev(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 9000,
      ID_SYNC_BUTTON,
      ID_SYNC_RUN_BUTTON,
	  ID_TAB_NEXT,
	  ID_TAB_PREV
   };
};

#endif
