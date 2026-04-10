//$Id$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
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
// Author: Allison Greene
// Created: 2004/12/15
//
/**
 * Declares FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef FunctionSetupPanel_hpp
#define FunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Function.hpp"

#ifdef __USE_STC_EDITOR__
#include "ScriptEditor.hpp"
#endif

class FunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   FunctionSetupPanel(wxWindow *parent, const wxString &name); 
   ~FunctionSetupPanel();
   
   virtual void SetEditorModified(bool flag);
   
   wxTextCtrl *mFileContentsTextCtrl;
   
#ifdef __USE_STC_EDITOR__
   ScriptEditor* GetEditor() { return mEditor; };
#endif
   
private:
   Function *theGmatFunction;
   wxString mFullFunctionPath;
   wxString mFunctionName;
   
#ifdef __USE_STC_EDITOR__
   ScriptEditor *mEditor;
#endif
   
   bool mEnableLoad;
   bool mEnableSave;
   bool mIsNewFunction;
   wxString mFilename;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   void OnSaveAs(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
