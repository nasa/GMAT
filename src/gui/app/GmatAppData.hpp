//$Id$
//------------------------------------------------------------------------------
//                              GmatAppData
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
// Created: 2003/10/29
//
/**
 * Declares GuiInterpreter and other GUI compoment pointers.
 */
//------------------------------------------------------------------------------
#ifndef GmatAppData_hpp
#define GmatAppData_hpp

#if !defined __CONSOLE_APP__

//#include "ViewTextFrame.hpp"
//#include "ResourceTree.hpp"
//#include "MissionTree.hpp"
//#include "OutputTree.hpp"
//#include "GmatMainFrame.hpp"

class ViewTextFrame;
class ResourceTree;
class MissionTree;
class OutputTree;
class GmatMainFrame;

#include "gmatwxdefs.hpp"
#include <wx/print.h>         // for wxPrintData
#include <wx/printdlg.h>      // for wxPageSetupDialogData
#include <wx/confbase.h>
#include <wx/config.h>


#ifdef __USE_STC_EDITOR__
#include <wx/cmndata.h>
#endif

#endif

#include "GuiInterpreter.hpp"

class GmatAppData
{
public:
   
   static GmatAppData* Instance();
   
   void SetGuiInterpreter(GuiInterpreter *guiInterp);
   GuiInterpreter* GetGuiInterpreter();
   
#if !defined __CONSOLE_APP__
   
   void SetMainFrame(GmatMainFrame *mainFrame);
   GmatMainFrame* GetMainFrame();
   
   void SetResourceTree(ResourceTree *resourceTree);
   ResourceTree* GetResourceTree();
   
   void SetMissionTree(MissionTree *missionTree);
   MissionTree* GetMissionTree();
   
   void SetOutputTree(OutputTree *outputTree);
   OutputTree* GetOutputTree();
   
   void SetMessageWindow(ViewTextFrame *frame);
   ViewTextFrame* GetMessageWindow();
   
   void SetCompareWindow(ViewTextFrame *frame);
   ViewTextFrame* GetCompareWindow();
   
   void SetMessageTextCtrl(wxTextCtrl *msgTextCtrl);
   wxTextCtrl* GetMessageTextCtrl();
   
   void SetFont(wxFont font);
   wxFont GetFont();
   
   void SetScriptFont(wxFont font);
   wxFont GetScriptFont();
   
   void SetFontSize(Integer size);
   Integer GetFontSize();
   
   void SetScriptFontSize(Integer size);
   Integer GetScriptFontSize();
   
   void SetTempScriptName(const wxString &tempName);
   wxString GetTempScriptName();
   
   wxConfigBase* GetPersonalizationConfig();
   
   void ResetIconFile();
   void SetIconFile();
   wxString GetIconFile();
   
   bool SetIcon(wxTopLevelWindow *topWindow, const std::string &calledFrom);
   
#endif
    
private:
   
   GuiInterpreter *theGuiInterpreter;
   
   /// The singleton instance
   static GmatAppData *theGmatAppData;
   
   GmatAppData();
   ~GmatAppData();
   
#if !defined __CONSOLE_APP__
   GmatMainFrame *theMainFrame;
   ResourceTree  *theResourceTree;
   MissionTree   *theMissionTree;
   OutputTree    *theOutputTree;
   ViewTextFrame *theMessageWindow;
   ViewTextFrame *theCompareWindow;
   wxTextCtrl    *theMessageTextCtrl;
   wxFont        theFont;
   wxFont        theScriptFont;
   Integer       theFontSize;
   Integer       theScriptFontSize;
   wxString      theTempScriptName;
   wxConfigBase  *thePersonalizationConfig;
   wxString      theIconFile;
   bool          theIconFileSet;
   
   #ifdef __USE_STC_EDITOR__
   wxPageSetupDialogData *thePageSetupDialogData;
   #endif
#endif
   
};

#if !defined __CONSOLE_APP__
   #if wxUSE_PRINTING_ARCHITECTURE
   // global print data, to remember settings during the session
   extern wxPrintData *globalPrintData;
   extern wxPageSetupData *globalPageSetupData;
   #endif // wxUSE_PRINTING_ARCHITECTURE
#endif

#endif // GmatAppData_hpp
