//$Id$
//------------------------------------------------------------------------------
//                              GmatPanel
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
// Modified:
//    2010.03.26 Thomas Grubb
//       - Reorganized bottom buttons
//       - Changed Show Script and Command Summary buttons to be bitmap buttons
//    2010.03.05 Thomas Grubb
//       - Enabled help button to launch wiki page from configuration file (GMAT.ini)
//       - Added accelerator keys to Apply, Help, Show Script and Command Summary buttons
//
/**
 * Implements GmatPanel class.
 */
//------------------------------------------------------------------------------

#include <wx/utils.h>
#include <wx/config.h>
#include "GmatPanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "GmatMdiChildFrame.hpp"
#include "FileUtil.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "bitmaps/NewScript.xpm"
#include "bitmaps/report.xpm"

#include "ShowScriptDialog.hpp"
#include "ShowSummaryDialog.hpp"
#include "CommandUtil.hpp"

//#define DEBUG_GMATPANEL
//#define DEBUG_GMATPANEL_SAVE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_SUMMARY, GmatPanel::OnSummary)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatPanel(wxWindow *parent, bool showBottomSizer, bool showScriptButton)
//------------------------------------------------------------------------------
/**
 * Constructs GmatPanel object.
 *
 * @param <parent> parent window.
 * @param <showBottomSizer> if true, shows bottom OK, Apply, Cancel buttons (true)
 * @param <showScriptButton> if true, shows Show Script button (true)
 *
 */
//------------------------------------------------------------------------------
GmatPanel::GmatPanel(wxWindow *parent, bool showBottomSizer, bool showScriptButton)
   : wxPanel(parent)
{
   // Set default font
   SetFont(GmatAppData::Instance()->GetFont());
   
   theOkButton = NULL;
   theApplyButton = NULL;
   theCancelButton = NULL;
   theHelpButton = NULL;
   theScriptButton = NULL;
   theSummaryButton = NULL;

   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   UserInputValidator::SetGuiManager(theGuiManager);
   UserInputValidator::SetWindow(this);

   canClose = true;
   mDataChanged = false;

   mShowBottomSizer = showBottomSizer;
   mShowScriptButton = showScriptButton;

   theParent = parent;

   #ifdef DEBUG_GMATPANEL
   MessageInterface::ShowMessage
      ("GmatPanel::GmatPanel() entered. theGuiInterpreter=<%p>\n   "
       "showBottomSizer=%d, showScriptButton=%d\n", theGuiInterpreter,
       showBottomSizer, showScriptButton);
   #endif

   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);

   #ifdef __SHOW_TOP_SIZER__
   theTopSizer = new wxStaticBoxSizer(wxVERTICAL, this );
   #endif

   if (showBottomSizer)
      theMiddleSizer = (wxSizer*)(new wxStaticBoxSizer(wxVERTICAL, this));
   else
      theMiddleSizer = (wxSizer*)(new wxBoxSizer(wxVERTICAL));

   theBottomSizer = new wxStaticBoxSizer(wxVERTICAL, this );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   #ifdef __SHOW_TOP_SIZER__
   wxBoxSizer *theTopButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   #endif

   if (showBottomSizer)
   {
      // create bottom buttons
      wxBitmap *bitmap = new wxBitmap(NewScript_xpm);
      theScriptButton = new wxBitmapButton
         (this, ID_BUTTON_SCRIPT, *bitmap, wxDefaultPosition, wxDefaultSize, 4);
      theScriptButton->SetToolTip("Show Script (F7)");
      bitmap = new wxBitmap(report_xpm);
      theSummaryButton = new wxBitmapButton
         (this, ID_BUTTON_SUMMARY, *bitmap, wxDefaultPosition, wxDefaultSize, 4);
      theSummaryButton->SetToolTip("Command Summary");
      //theScriptButton = new wxButton
      //   (this, ID_BUTTON_SCRIPT, "Show "GUI_ACCEL_KEY"Script", wxDefaultPosition, wxDefaultSize, 0);
      //theSummaryButton = new wxButton
      //   (this, ID_BUTTON_SUMMARY, GUI_ACCEL_KEY"Command Summary", wxDefaultPosition, wxDefaultSize, 0);

      theOkButton = new wxButton
         (this, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
      theApplyButton = new wxButton
         (this, ID_BUTTON_APPLY, GUI_ACCEL_KEY"Apply", wxDefaultPosition, wxDefaultSize, 0);
      theCancelButton = new wxButton
                  (this, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);

      theHelpButton = new wxButton
         (this, ID_BUTTON_HELP, GUI_ACCEL_KEY"Help", wxDefaultPosition, wxDefaultSize, 0);
      theHelpButton->SetToolTip("Panel-specific Help (F1)");


      // set the Apply button as the default button, T. Grubb
      theApplyButton->SetDefault();

      // use different color for Show Script, and Command Summary for now
      theScriptButton->SetForegroundColour(*wxBLUE);
      theSummaryButton->SetForegroundColour(*wxBLUE);
   }

   int borderSize = 3;

   // add items to top sizer
   #ifdef __SHOW_TOP_SIZER__
   theTopButtonSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   theTopButtonSizer->Add(theSummaryButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   theTopSizer->Add(theTopButtonSizer, 0, wxALIGN_RIGHT | wxALL, borderSize);
   #endif

   // adds the buttons to button sizer
   if (showBottomSizer)
   {
      theButtonSizer->Add(theScriptButton, 0, wxALIGN_CENTER | wxALL, borderSize);
      theButtonSizer->Add(theSummaryButton, 0, wxALIGN_CENTER | wxALL, borderSize);

      theButtonSizer->AddSpacer(10);
      theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
      theButtonSizer->Add(theApplyButton, 0, wxALIGN_CENTER | wxALL, borderSize);
      theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);

      theButtonSizer->Add(0, 1, wxALIGN_RIGHT);
      theButtonSizer->Add(theHelpButton, 0, wxALIGN_RIGHT | wxALL, borderSize);

      theBottomSizer->Add(theButtonSizer, 0, wxGROW | wxALL, borderSize);
   }

   #ifdef __SHOW_TOP_SIZER__
   topStaticBox->Show(mShowScriptButton);
   #endif

   mObject = NULL;
      
   // shortcut keys
   wxAcceleratorEntry entries[3];
   entries[0].Set(wxACCEL_NORMAL,  WXK_F1, ID_BUTTON_HELP);
   entries[1].Set(wxACCEL_NORMAL,  WXK_F7, ID_BUTTON_SCRIPT);
   entries[2].Set(wxACCEL_CTRL,  (int) 'W', ID_BUTTON_CANCEL);
   wxAcceleratorTable accel(3, entries);
   this->SetAcceleratorTable(accel);

   #ifdef DEBUG_GMATPANEL
   MessageInterface::ShowMessage("GmatPanel::GmatPanel() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter* GmatPanel::GetGuiInterpreter()
{
   return theGuiInterpreter;
}


//------------------------------------------------------------------------------
// wxPanel* GetPanel()
//------------------------------------------------------------------------------
wxPanel* GmatPanel::GetPanel()
{
   return this;
}


//------------------------------------------------------------------------------
// virtual void EnableUpdate(bool enable = true)
//------------------------------------------------------------------------------
void GmatPanel::EnableUpdate(bool enable)
{
   #ifdef DEBUG_GMATPANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::EnableUpdate() enable=%d\n", enable);
   #endif

   GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();

   if (enable)
   {
      mDataChanged = true;
      mdichild->SetDirty(true);
   }
   else
   {
      mDataChanged = false;
      mdichild->SetDirty(false);
   }
   // toggle the Apply button based on modifications (T Grubb)
   if (theApplyButton != NULL) theApplyButton->Enable(mDataChanged);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const wxString &action)
//------------------------------------------------------------------------------
bool GmatPanel::TakeAction(const wxString &action)
{
   return true;
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool GmatPanel::PrepareObjectNameChange()
{
   return canClose;
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(UnsignedInt type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
void GmatPanel::ObjectNameChanged(UnsignedInt type, const wxString &oldName,
                                  const wxString &newName)
{
   // Do we need anything here?
}

//------------------------------------------------------------------------------
// virtual bool RefreshObjects(UnsignedInt type = Gmat::UNKNOWN_OBJECT)
//------------------------------------------------------------------------------
bool GmatPanel::RefreshObjects(UnsignedInt type)
{
   // child classes will implement this as needed
   return true;
}


//------------------------------------------------------------------------------
// void SetCanClose(bool flag)
//------------------------------------------------------------------------------
void GmatPanel::SetCanClose(bool flag)
{
   canClose = flag;
}


//------------------------------------------------------------------------------
// void SetEditorModified(bool isModified)
//------------------------------------------------------------------------------
void GmatPanel::SetEditorModified(bool isModified)
{
   mEditorModified = isModified;
}


//------------------------------------------------------------------------------
// bool IsEditorModified()
//------------------------------------------------------------------------------
bool GmatPanel::IsEditorModified()
{
   return mEditorModified;
}


//------------------------------------------------------------------------------
// virtual void OnApply()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnApply(wxCommandEvent &event)
{
   #ifdef DEBUG_GMATPANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::OnApply() entered, mDataChanged=%d\n", mDataChanged);
   #endif

   if (mDataChanged)
   {
      SaveData();
      GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();
      if (canClose)
      {
         mdichild->SetDirty(false);
         #ifdef DEBUG_GMATPANEL_SAVE
         MessageInterface::ShowMessage
            ("   ==> Setting configuration changed to true for the object <%s>\n",
             mObject->GetTypeName().c_str());
         #endif
         theGuiInterpreter->ConfigurationChanged(mObject, true);
         EnableUpdate(false);
		 GmatAppData::Instance()->GetMainFrame()->PanelObjectChanged(mObject);

      }
   }
   #ifdef DEBUG_GMATPANEL_SAVE
   MessageInterface::ShowMessage("GmatPanel::OnApply() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void OnOk()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatPanel::OnOK(wxCommandEvent &event)
{
   #ifdef DEBUG_GMATPANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::OnOK() entered, mDataChanged=%d\n", mDataChanged);
   #endif
   
   if (mDataChanged)
   {
      SaveData();
      GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();

      if (canClose)
      {
         mdichild->SetDirty(false);
         #ifdef DEBUG_GMATPANEL_SAVE
         MessageInterface::ShowMessage
            ("   ==> Setting configuration changed to true for the object <%s>\n",
             mObject->GetTypeName().c_str());
         #endif
         theGuiInterpreter->ConfigurationChanged(mObject, true);
		 GmatAppData::Instance()->GetMainFrame()->PanelObjectChanged(mObject);

      }
   }
   
   if (canClose)
      GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
   
   #ifdef DEBUG_GMATPANEL_SAVE
   MessageInterface::ShowMessage("GmatPanel::OnOK() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnCancel(wxCommandEvent &event)
{
   GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();
   mdichild->SetDirty(false);
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatPanel::OnHelp(wxCommandEvent &event)
{
   wxString s;
   wxString sHTML;
   wxString baseHelpLink;
   char msgBuffer[255];

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("/Help"));
   if (mObject != NULL)
     s = mObject->GetTypeName().c_str();
   else
     s = GetName().c_str();

	// get base help link if available
   FileManager* fm = FileManager::Instance();
   std::string helpPath = fm->GetFullPathname("HELP_PATH");
   helpPath = helpPath + _T("html/%s.html");
   
	sprintf( msgBuffer, helpPath.c_str(), s.WX_TO_C_STRING);
	#ifdef DEBUG_GMATPANEL
	  MessageInterface::ShowMessage
		 ("GmatPanel::OnHelp() Default Help Link=%s\n", msgBuffer);
	#endif
	// open separate window to show help
   s = pConfig->Read(s, wxString(&msgBuffer[0]));
	#ifdef DEBUG_GMATPANEL
	  MessageInterface::ShowMessage
		 ("GmatPanel::OnHelp() Web Page=<%s>\n",
		  s.WX_TO_C_STRING);
	#endif
   
   //The following adds in the absolute path instead of the ..
   size_t start_pos = s.find("file://.."); 
   if (start_pos != std::string::npos) 
   { 
      std::string workingDir = GmatFileUtil::GetApplicationPath();
      std::string sepType = GmatFileUtil::GetPathSeparator();
      Integer end = workingDir.find_last_of(sepType);
      if (end != std::string::npos)
         workingDir = workingDir.substr(0, end);
      end = workingDir.find_last_of(sepType);
      if (end != std::string::npos)
         workingDir = workingDir.substr(0, end);
      s.replace(start_pos, 9, workingDir.c_str());
   }
	wxLaunchDefaultApplication(s);
    
}


//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void GmatPanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL)
   {
      title = "Scripting for ";
      if (mObject->IsOfType("GmatCommand"))
         title += mObject->GetTypeName().c_str();
      else
         title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject);
   ssd.ShowModal();
}


//------------------------------------------------------------------------------
// virtual void OnSummary()
//------------------------------------------------------------------------------
/**
 * Shows command summary
 */
//------------------------------------------------------------------------------
void GmatPanel::OnSummary(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL)
   {
      title = "Command Summary for ";
      if (mObject->GetName() != "")
         title += mObject->GetName().c_str();
      else
      {
         std::string cmdType = mObject->GetTypeName();
         if (cmdType == "BeginScript") cmdType = "ScriptEvent";
         title += cmdType.c_str();
      }
   }
   GmatCommand *cmdObj = (GmatCommand*) mObject;
   // Handle special case of ScriptEvent - we need the Command Summary of the EndScript
   if (cmdObj->GetTypeName() == "BeginScript")
   {
      GmatCommand *endCmd = GmatCommandUtil::GetMatchingEnd(cmdObj);
      std::string endName = endCmd->GetSummaryName();
      endCmd->SetSummaryName(cmdObj->GetSummaryName());
      ShowSummaryDialog ssd(this, -1, title, endCmd);
      ssd.ShowModal();
      // Set the EndScript command summary name back to what it was
      endCmd->SetSummaryName(endName);
   }
   else
   {
      ShowSummaryDialog ssd(this, -1, title, cmdObj);
      ssd.ShowModal();
   }
}



//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj)
//------------------------------------------------------------------------------
bool GmatPanel::SetObject(GmatBase *obj)
{
   if (obj == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The panel cannot be populated, the object named \"" +
          mObjectName + "\" is NULL\n");
      return false;
   }
   else
   {
      mObject = obj;
      UserInputValidator::SetObject(obj);
      return true;
   }
}


//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void GmatPanel::Show()
{
   // add items to panel sizer

   #ifdef __SHOW_TOP_SIZER__
   thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
   #endif

   thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);

   if (mShowBottomSizer)
      thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);

   // displays the script button
   #ifdef __SHOW_TOP_SIZER__
   thePanelSizer->Show(theTopSizer, mShowScriptButton);
   #endif

   if (mShowBottomSizer)
      theScriptButton->Show(mShowScriptButton);

   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );

   SetSizer(thePanelSizer); //use the sizer for layout

   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size

   LoadData();

   EnableUpdate(false);

   if (mShowBottomSizer)
      if ((mObject == NULL) || (!mObject->IsOfType(Gmat::COMMAND)))
         theSummaryButton->Hide();

   // call Layout() to force layout of the children anew
   thePanelSizer->Layout();
}


