//$Id$
//------------------------------------------------------------------------------
//                              ScriptEventPanel
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
// Created: 2005/1/12
// Modified:
//    2006.12.04 Linda Jun
//       - Implemented save changes by replacing old command sequence with new
//
/**
 * Declares ScriptEventPanel class.
 */
//------------------------------------------------------------------------------

#ifndef ScriptEventPanel_hpp
#define ScriptEventPanel_hpp

#include "GmatPanel.hpp"
#include "Parameter.hpp"
#include "MissionTreeItemData.hpp"
#include <wx/laywin.h>

#ifdef __USE_STC_EDITOR__
#include "ScriptEditor.hpp"
#endif

class ScriptEventPanel: public GmatPanel
{
public:
   // constructors
   ScriptEventPanel(wxWindow *parent, MissionTreeItemData *item);
   ~ScriptEventPanel();
   
   virtual void SetEditorModified(bool flag);
   
   wxTextCtrl *mFileContentsTextCtrl;
   
#ifdef __USE_STC_EDITOR__
   ScriptEditor* GetEditor() { return mEditor; };
#endif
   
private:
   // member data
#ifdef __USE_STC_EDITOR__
   ScriptEditor *mEditor;
#endif
   
   MissionTreeItemData *theItem;
   GmatCommand *theCommand;
   GmatCommand *mPrevCommand;
   GmatCommand *mNextCommand;
   GmatCommand *mNewCommand;
   
   wxSashLayoutWindow* theCommentsWin;
   wxSashLayoutWindow* theScriptsWin;
   
   wxTextCtrl  *mCommentTextCtrl;
   wxGridSizer *mBottomSizer;
   wxBoxSizer  *mPageSizer;
   
   void SaveComments();
   void ReplaceScriptEvent();
   
   // for Debug
   void ShowCommand(const std::string &title1, GmatCommand *cmd1,
                    const std::string &title2 = "", GmatCommand *cmd2 = NULL);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnCommentChange(wxCommandEvent& event);
   void OnScriptChange(wxCommandEvent& event);
   void OnSashDrag(wxSashEvent &event);
   void OnSize(wxSizeEvent &event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_SASH_WINDOW = 9000,
      ID_TEXT,
      ID_COMMENT_CTRL,
      ID_SCRIPT_CTRL,
      ID_STC,
   };
};

#endif
