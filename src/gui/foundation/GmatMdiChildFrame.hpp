//$Id$
//------------------------------------------------------------------------------
//                              GmatMdiChildFrame
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
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatMdiChildFrame class.
 */
//------------------------------------------------------------------------------

#ifndef GmatMdiChildFrame_hpp
#define GmatMdiChildFrame_hpp

#include "gmatwxdefs.hpp"
#include "GmatMenuBar.hpp"
#include "GuiInterpreter.hpp"
#include "GmatTreeItemData.hpp"  // for namespace GmatTree::
#include "PluginWidget.hpp"

#ifdef __USE_STC_EDITOR__
#include "ScriptEditor.hpp"
#endif

class GmatMdiChildFrame : public wxMDIChildFrame
{
public:
   // constructors
   GmatMdiChildFrame(wxMDIParentFrame *parent, 
                     const wxString &name = "",
                     const wxString &title = "", 
                     const GmatTree::ItemType type = GmatTree::UNKNOWN_ITEM,
                     wxWindowID id = -1, 
                     const wxPoint &pos = wxDefaultPosition, 
                     const wxSize &size = wxDefaultSize, 
                     long style = wxDEFAULT_FRAME_STYLE);
   ~GmatMdiChildFrame();
   
#ifdef __WXMAC__
   wxString GetTitle();
   void SetTitle(wxString newTitle);
#endif

   virtual wxString   GetPlotName();
   virtual void       SetPlotName(const wxString &name);
   virtual void       SetSavedConfigFlag(bool isPreset);
   virtual bool       GetSavedConfigFlag();

   wxMenuBar*         GetMenuBar();
   GmatTree::ItemType GetItemType();
   void               SetDataType(GmatTree::ItemType type);
   wxTextCtrl*        GetScriptTextCtrl();
   void               SetScriptTextCtrl(wxTextCtrl *textCtrl);
   void               SetActiveChild(bool active);
   bool               IsActiveChild();
   void               UpdateGuiItem(int updateEdit, int updateAnimation);
   void               UpdateActiveChild();

   bool               ChildIsClosing();
   
#ifdef __USE_STC_EDITOR__
   // Renamed Editor to ScriptEditor to fix name collision with wxWidget's Scintilla lib
   ScriptEditor* GetEditor();
   void SetEditor(ScriptEditor *editor);
#endif
   
   void SetDirty(bool dirty) { mDirty = dirty; }
   bool IsDirty() const { return mDirty; }
   void OverrideDirty(bool flag) { mOverrideDirty = flag; }
   void SetCanBeDeleted(bool flag) { mCanBeDeleted = flag; }
   bool CanClose() const { return mCanClose; }
   
   wxWindow* GetMdiParent();
   
   virtual wxWindow* GetAssociatedWindow();
   virtual void SetAssociatedWindow(wxWindow *win);
   
   virtual void OnActivate(wxActivateEvent &event);
   virtual void OnIconize(wxIconizeEvent &event);
   virtual void OnClose(wxCloseEvent &event);
   virtual void UpdateScriptActiveStatus(bool isActive);
   
   virtual void SetSaveLocationFlag(bool tf = true);
   virtual void SaveChildPositionAndSize();

   void SetPluginWidget(PluginWidget *widget);
   PluginWidget *GetPluginWidget();

protected:
   
#ifdef __WXMAC__
   wxString childTitle;
#endif
   
   static Integer maxZOrder;
   Integer        relativeZOrder;
   bool           usingSavedConfiguration;
   
   wxString mChildName;
   
   bool mDirty;
   bool mOverrideDirty;
   bool mCanClose;
   bool mCanSaveLocation;
   bool mIsActiveChild;
   bool mCanBeDeleted;
   GmatTree::ItemType mItemType;
   wxTextCtrl *theScriptTextCtrl;
   GmatMenuBar *theMenuBar;
   wxMDIParentFrame *theParent;
   wxWindow *theAssociatedWin;
   
   // GuiInterpreter
   GuiInterpreter *theGuiInterpreter;

   /// Plugin widget used for windows that remain open between runs
   PluginWidget *thePluginWidget;

   bool childIsClosing;


#ifdef __USE_STC_EDITOR__
   ScriptEditor *theEditor;
#endif

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
      
};


#endif // GmatMdiChildFrame_hpp
