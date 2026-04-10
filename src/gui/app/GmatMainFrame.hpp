//$Id$
//------------------------------------------------------------------------------
//                              GmatMainFrame
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
// Created: 2003/08/05
//
// Modified: 2003/08/28 - Allison Greene
//         : Updated to include full menubar and splitter window.
//
//         : 2003/09/18 - Allison Greene
//         : Updated to include tool bar.
//
//         : 2003/10/03 - Allison Greene
//         : Updated to include tabs for right side of window and left side.
//
/**
 * This class provides the main frame for GMAT.
 */
//------------------------------------------------------------------------------
#ifndef GmatMainFrame_hpp
#define GmatMainFrame_hpp

#include "gmatwxdefs.hpp"

#include <stdio.h>

#include "ViewTextFrame.hpp"
#include "GuiInterpreter.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatServer.hpp"
#include "GmatMdiChildFrame.hpp"
#include "GmatNotebook.hpp"

// Plugin GUI elements
#include "GmatWidget.hpp"
#include "GuiFactory.hpp"

#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/docview.h>
#include <wx/laywin.h>
#include <wx/textctrl.h>
#include <wx/hyperlink.h>
#include <wx/help.h>        // (wxWidgets chooses the appropriate help controller class)

class MdiChildViewFrame;
class MdiChildTsFrame;
class WelcomePanel;

namespace GmatGui
{
   enum ScriptOption
   {
      OPEN_SCRIPT_ON_ERROR,
      ALWAYS_OPEN_SCRIPT,
      DO_NOT_OPEN_SCRIPT
   };
};

class GmatMainFrame : public wxMDIParentFrame
{
public:
   // constructors
   GmatMainFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
                 const wxPoint& pos, const wxSize& size, const long style);
   ~GmatMainFrame();
   
   void GetActualClientSize(Integer *w, Integer *h, bool ignoreMissionTree);
   GmatMdiChildFrame* CreateChild(GmatTreeItemData *item, bool restore = true);
   GmatMdiChildFrame* GetChild(const wxString &name);
   wxHelpController* GetHelpController();
   wxList* GetListOfMdiChildren();
   
   Integer GetNumberOfChildOpen(bool scriptsOnly = false, bool incPlots = false,
                                bool incScripts = false, bool incMissionTree = false);
   Integer GetNumberOfActivePlots();

   bool IsMissionRunning();
   bool IsAnimatable();
   bool IsAnimationRunning();
   bool IsMissionTreeUndocked();
   bool IsMissionTreeUndocked(Integer &xPos, Integer &yPos, Integer &width);
   bool IsScriptEditorOpen(Integer &xPos, Integer &yPos, Integer &width, Integer &height);
   void IconizeUndockedMissionTree();
   bool IsChildOpen(GmatTree::ItemType itemType);
   bool IsChildOpen(GmatTreeItemData *item, bool restore = true);
   bool IsChildOpen(const wxString &name, GmatTree::ItemType itemType,
                    bool restore = true);
   bool RenameChild(GmatTreeItemData *item, wxString newName);
   bool RenameChild(const wxString &oldName, const wxString &newName);
   bool RenameActiveChild(const wxString &newName);
   bool RemoveChild(const wxString &name, GmatTree::ItemType itemType,
                    bool deleteChild = true);
   void SetAutoExitAfterRun(bool autoExit);
   void RemoveOutput(const wxString &name);
   void CloseChild(const wxString &name, GmatTree::ItemType itemType);
   void CloseChild(GmatMdiChildFrame *child);
   void CloseWelcomePanel();
   void CloseActiveChild();
   bool CloseAllChildren(bool closeScriptWindow = true, bool closePlots = true,
                         bool closeReports = true, bool closeUndockedMissionTree = true,
                         bool closingGmat = false, bool closeWelcomePanel = true,
                         bool closeSolverWindows = true);
   void MinimizeChildren();
   void ComputeReportPositionAndSize(const wxString &name, Integer &x,
                                     Integer &y, Integer &w, Integer &h,
                                     bool &isUsingSaved);
   void PositionNewChild(GmatMdiChildFrame *newChild, int numChildren);
   void RepositionChildren(int xOffset);
   void SetActiveChildDirty(bool dirty);
   void OverrideActiveChildDirty(bool override);
   void CloseCurrentProject();
   bool InterpretScript(const wxString &filename,
                        Integer scriptOpenOpt = GmatGui::OPEN_SCRIPT_ON_ERROR,
                        bool closeScript = false, bool readBack = false,
                        const wxString &savePath = "", bool multScripts = false,
                        bool closeWelcomePanel = true);
   bool BuildScript(const wxString &filename, bool addToResourceTree = false);
   Integer RunCurrentScript();
   Integer BuildAndRunScript(const wxString &filename, bool addToResourceTree = false);
   Integer RunCurrentMission();
   void StopRunningMission();
   void StopAnimation();
   void NotifyRunCompleted();
   void ProcessPendingEvent();
   void StartMatlabServer();
   void StopMatlabServer();
   void UpdateRecentMenu(wxArrayString files);
   Integer GetToolBarHeight();
   wxToolBar* GetMainFrameToolBar();
   wxStatusBar* GetMainFrameStatusBar();
   
   void EnableAnimation(bool enable = true);
   void ManageMissionTree();
   
   // event handling
   void PanelObjectChanged( GmatBase *obj );
   void OnClose(wxCloseEvent& event);
   void OnProjectNew(wxCommandEvent &event);
   void OnClearCurrentMission(wxCommandEvent &event);
   void OnLoadDefaultMission(wxCommandEvent &event);
   void OnSaveScriptAs(wxCommandEvent &event);
   void OnSaveScript(wxCommandEvent &event);
   void OnPrintSetup(wxCommandEvent &event);
   void OnPrint(wxCommandEvent &event);
   void OnProjectExit(wxCommandEvent &event);
   void OnRun(wxCommandEvent &event);
   void OnPause(wxCommandEvent &event);
   void OnPause(); //Used by the Breakpoint command.
   void OnStop(wxCommandEvent &event);
   void OnHelpAbout(wxCommandEvent &event);
   void OnHelpWelcome(wxCommandEvent &event);
   void OnHelpHelp(wxCommandEvent &event);
   void OnHelpSearch(wxCommandEvent &event);
   void OnHelpUsingGmat(wxCommandEvent &event);
   void OnHelpTutorial(wxCommandEvent &event);
   void OnHelpIssue(wxCommandEvent &event);
   void OnHelpUpdate(wxCommandEvent &event);
   void OnHelpFeedback(wxCommandEvent &event);
   void OnHyperLinkClick(wxHyperlinkEvent &event);
   
   void OnNewScript(wxCommandEvent &event);
   void OnOpenScript(wxCommandEvent &event);
   void OpenRecentScript(size_t index, wxCommandEvent &event);
   void OpenRecentScript(wxString filename, wxCommandEvent &event, bool closeWelcomePanel = true);
   void OnOpenRecentScript1(wxCommandEvent &event);  // I know this is retarded but the event returns the MDI frame
   void OnOpenRecentScript2(wxCommandEvent &event);  // as its event object.  There doesn't seem to be a way
   void OnOpenRecentScript3(wxCommandEvent &event);  // to figure out which menu item called an event handler
   void OnOpenRecentScript4(wxCommandEvent &event);
   void OnOpenRecentScript5(wxCommandEvent &event);
   void OnSetPath(wxCommandEvent &event);
   
   void OnScreenshot(wxCommandEvent &event);
  
   void OnUndo(wxCommandEvent& event);
   void OnRedo(wxCommandEvent& event);
   void OnCut(wxCommandEvent& event);
   void OnCopy(wxCommandEvent& event);
   void OnPaste(wxCommandEvent& event);
   void OnComment(wxCommandEvent& event);
   void OnUncomment(wxCommandEvent& event);
   void OnSelectAll(wxCommandEvent& event);
   
   void OnMsgWinCopy(wxCommandEvent& event);
   void OnMsgWinRightMouseDown(wxMouseEvent& event);
   void OnMsgWinSelectAll(wxCommandEvent& event);

   void OnFind(wxCommandEvent& event);
   void OnFindNext(wxCommandEvent& event);
   void OnReplace(wxCommandEvent& event);
   void OnReplaceNext(wxCommandEvent& event);
   void OnGoToLine(wxCommandEvent& event);
   void OnLineNumber(wxCommandEvent& event);
   void OnIndentMore(wxCommandEvent& event);
   void OnIndentLess(wxCommandEvent& event);
   
   void OnFont(wxCommandEvent& event);
   
   void OnOpenMatlab(wxCommandEvent& event);
   void OnCloseMatlab(wxCommandEvent& event);
   
   void OnMatlabServerStart(wxCommandEvent& event);
   void OnMatlabServerStop(wxCommandEvent& event);
   
   void OnFileCompare(wxCommandEvent& event);
   void OnGenerateTextEphemFile(wxCommandEvent& event);
   
   void OnSashDrag(wxSashEvent &event);
   void OnMsgSashDrag(wxSashEvent &event);
   void OnMainFrameSize(wxSizeEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   void OnCycleTab(wxCommandEvent &event);
   
   void OnAnimation(wxCommandEvent& event);
   
   void UpdateMenus(bool openOn);
   void EnableMenuAndToolBar(bool enable, bool missionRunning = false,
                             bool forAnimation = false);
   void EnableNotebookAndMissionTree(bool enable);
   
   void OnScriptBuildObject(wxCommandEvent& WXUNUSED(event));
   void OnScriptBuildAndRun(wxCommandEvent& event);
   void OnScriptRun(wxCommandEvent& WXUNUSED(event));
   void OnCloseAll(wxCommandEvent &event);
   void OnCloseActive(wxCommandEvent &event);
   
   bool SetScriptFileName(const char *filename);
   bool SetScriptFileName(const std::string &filename);
   bool IsActiveScriptModified();
   void RefreshActiveScript(const wxString &filename, bool reloadFile = true);
   std::string GetActiveScriptFileName();
   void UpdateGuiScriptSyncStatus(int guiStatus, int scriptStatus);
   void UpdateAdvancedGuiMode(int status);
   
   virtual bool Show(bool show = true);
   
   void ResumeRun();


   MdiChildViewFrame *viewSubframe;
   MdiChildTsFrame *tsSubframe;
   wxList *theMdiChildren;

   // Plugin Window Creator
   GmatMdiChildFrame* CreatePluginChild(const std::string &title,
                                    const std::string &name,
                                    const std::string panelType,
                                    GmatTree::ItemType itemType,
                                    GmatBase *forObject,
                                    GmatWidget **returnedWidget);

protected:

private:
   wxString mAnimationTitle;
   int  mAnimationFrameInc;
   bool mAnimationEnabled;
   bool mIsMissionRunning;
   bool mRunPaused;
   bool mRunCompleted;
   bool mAutoExitAfterRun;
   bool mInterpretFailed;
   bool mExitWithoutConfirm;
   bool mUndockedMissionTreePresized;
   Integer mRunStatus;

   bool useChildDelete;
   
   GmatServer *mMatlabServer;
   std::string mScriptFilename;
   std::string mTempScriptName;
   GuiInterpreter *theGuiInterpreter;
   
   wxSashLayoutWindow* theMainWin;
   wxSashLayoutWindow* theMessageWin;
   GmatNotebook *theNotebook;
   
   ViewTextFrame *mTextFrame;
   WelcomePanel *mWelcomePanel;
   wxStatusBar *theStatusBar;
   wxMenuBar *theMenuBar;
   wxToolBar *theToolBar;
   wxMenu *mMsgWinPopupMenu;
   wxTextCtrl *msgTextCtrl;
   
   wxHelpController *theHelpController;
   
   GmatMdiChildFrame* CreateNewResource(const wxString &title,
                                        const wxString &name,
                                        GmatTree::ItemType itemType,
                                        GmatBase *forObject = NULL);    // Added for GUI plugins
   GmatMdiChildFrame* CreateNewCommand(GmatTree::ItemType itemType,
                                       GmatTreeItemData *item);
   GmatMdiChildFrame* CreateNewControl(const wxString &title,
                                       const wxString &name,
                                       GmatTree::ItemType itemType,
                                       GmatCommand *cmd);
   GmatMdiChildFrame* CreateNewOutput(const wxString &title,
                                      const wxString &name,
                                      GmatTree::ItemType itemType);
   GmatMdiChildFrame* CreateUndockedMissionPanel(const wxString &title,
                                                 const wxString &name,
                                                 GmatTree::ItemType itemType);
   void RestoreUndockedMissionPanel();
   
   bool ShowScriptOverwriteMessage();
   bool ShowSaveMessage();
   bool SaveScriptAs();
   void OpenScript(bool restore = true);
   void UpdateTitle(const wxString &filename = "");
   void SaveGuiToActiveScript();
   
   void SaveChildPositionsAndSizes();
   bool GetConfigurationData(const std::string &forItem, Integer &x, Integer &y,
                             Integer &w, Integer &h, bool &isMinimized, bool &isMaximized);
   
   void ComputeAnimationSpeed(Integer &frameInc, Integer &updateIntervalInMilSec,
                              bool slower);
   
   void CompareFiles();
   void GetBaseFilesToCompare(Integer compareType, const wxString &baseDir,
                              const wxString &basePrefix, wxArrayString &baseFileNameArray,
                              wxArrayString &noPrefixNameArray);
   
   // GUI Plugin additions
   std::vector <GuiFactory*> guiFactories;
   GmatWidget *GetPluginWidget(const std::string type, GmatBase *forObject,
         wxWindow *withParent = NULL);

   // IDs for the controls
   enum
   {
      ID_SASH_WINDOW = 100,
      ID_MSGWIN_MENU_COPY,
      ID_MSGWIN_MENU_SELECTALL,
      ID_MSG_SASH_WINDOW,
   };
   
   // event handling
   DECLARE_EVENT_TABLE();
   
};

#endif // GmatMainFrame_hpp
