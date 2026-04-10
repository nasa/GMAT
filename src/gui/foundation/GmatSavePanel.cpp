//$Id$
//------------------------------------------------------------------------------
//                              GmatSavePanel
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
//
/**
 * Implements GmatSavePanel class.
 */
//------------------------------------------------------------------------------

#include "GmatSavePanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "ShowScriptDialog.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SAVE
//#define DEBUG_ACTIVE_SCRIPT
//#define DEBUG_TEXT_MODIFIED

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatSavePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_SAVE, GmatSavePanel::OnSave)
   EVT_BUTTON(ID_BUTTON_SAVE_AS, GmatSavePanel::OnSaveAs)
   EVT_BUTTON(ID_BUTTON_CLOSE, GmatSavePanel::OnClosePanel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatSavePanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatSavePanel(wxWindow *parent,  bool showScriptButton, ...)
//------------------------------------------------------------------------------
/**
 * Constructs GmatSavePanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
GmatSavePanel::GmatSavePanel(wxWindow *parent, bool showScriptButton,
                             wxString filename, bool showScriptActiveStatus,
                             bool isScriptActive)
   : GmatPanel(parent, false, showScriptButton)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   canClose = true;
   mEditorModified = false;
   hasFileLoaded = false;
   mShowScriptButton = showScriptButton;
   mFilename = filename;
   if (mFilename == "")
      mFilename = GmatAppData::Instance()->GetTempScriptName().c_str();
   mShowScriptActiveStatus = showScriptActiveStatus;
   mIsScriptActive = isScriptActive;
   mSyncGui = false;
   mDelayBuild = false;
   mSaveCanceled = false;
   theParent = parent;
   SetName("GmatSavePanel");
   
   int borderSize = 2;
   wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );
   
   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   theTopSizer = new wxStaticBoxSizer( topStaticBox, wxVERTICAL );
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   
   // create script button
   theScriptButton = new wxButton(this, ID_BUTTON_SCRIPT, "Show Script",
                                  wxDefaultPosition, wxDefaultSize, 0);
   
   // create bottom buttons
   theSaveButton =
      new wxButton(this, ID_BUTTON_SAVE, "Save", wxDefaultPosition, wxDefaultSize, 0);
   theSaveAsButton =
      new wxButton(this, ID_BUTTON_SAVE_AS, "Save As", wxDefaultPosition, wxDefaultSize, 0);
   theCloseButton =
      new wxButton(this, ID_BUTTON_CLOSE, "Close", wxDefaultPosition, wxDefaultSize, 0);
   
   // add items to top sizer
   theTopSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   
   //------------------------------------------------------
   // for active or inactive script indicator
   //------------------------------------------------------
   wxFlexGridSizer *bottomGridSizer = new wxFlexGridSizer(5);
   
   if (mShowScriptActiveStatus)
   {
      mScriptActiveLabel = new wxStaticText
         (this, -1, wxT(""), wxDefaultPosition, wxSize(110, -1), wxALIGN_CENTRE);
      mScriptActiveLabel->Centre();
      // Make font bold face
      wxFont font = mScriptActiveLabel->GetFont();
      font.SetWeight(wxFONTWEIGHT_BOLD);
      // Make font size little bigger
      int fontSize = font.GetPointSize();
      font.SetPointSize(fontSize + 1);
      mScriptActiveLabel->SetFont(font);
      
      RefreshScriptActiveStatus(mIsScriptActive);
      bottomGridSizer->Add(mScriptActiveLabel, 0, wxALIGN_LEFT | wxALL, borderSize*2);
      bottomGridSizer->Add(30, 20);
   }
   
   // Add script status label
   mScriptDirtyLabel = new wxStaticText
      (this, -1, wxT(""), wxDefaultPosition, wxSize(80, -1), wxALIGN_CENTRE);
   bottomGridSizer->Add(mScriptDirtyLabel, 0, wxALIGN_LEFT | wxALL, borderSize*2);
   bottomGridSizer->Add(50, 20);
   
   // adds the buttons to button sizer
   theButtonSizer->Add(theSaveButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theSaveAsButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCloseButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   ////if (mIsScriptActive)
   ////   theButtonSizer->Hide(theSaveButton);
   
   bottomGridSizer->Add(theButtonSizer, 0, wxALIGN_RIGHT | wxALL, borderSize);
   theBottomSizer->Add(bottomGridSizer, 0, wxALIGN_LEFT | wxALL, borderSize);
   
   topStaticBox->Show(mShowScriptButton);
   
   mObject = NULL;
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::Show()
{
   // add items to middle sizer
   thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
   thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // displays the script button
   thePanelSizer->Show(theTopSizer, mShowScriptButton);
   theScriptButton->Show(mShowScriptButton);
   thePanelSizer->Layout();
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   LoadData();
   mScriptDirtyLabel->SetLabel("");

}


//------------------------------------------------------------------------------
// void OnSave()
//------------------------------------------------------------------------------
/**
 * Saves the data to disk. No syntax checking is performed and no GUI
 * synchronization is performed.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnSave(wxCommandEvent &event)
{
   // scriptStatus: 1 = clean, 2 = dirty, 3 = error
   int scriptStatus = theGuiManager->GetActiveScriptStatus();
   int guiStatus = theGuiManager->GetGuiStatus();
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::OnSave() entered, mSyncGui=%d, mFilename='%s'\n   tempFileName='%s', "
       "scriptStatus=%d, guiStatus=%d, mIsScriptActive=%d\n", mSyncGui, mFilename.WX_TO_C_STRING,
       GmatAppData::Instance()->GetTempScriptName().WX_TO_C_STRING, scriptStatus, guiStatus,
       mIsScriptActive );
   #endif
   
   // If it is active script and both script and GUI is clean, there is nothing to save
   if (mIsScriptActive && scriptStatus == 1 && guiStatus == 1)
   {
      #ifdef DEBUG_SAVE
      MessageInterface::ShowMessage
         ("GmatSavePanel::OnSave() leaving, both script and GUI is clean\n",
          scriptStatus);
      #endif
      return;
   }
   
   mSaveCanceled = false;
   // if it is temp script file, call OnSaveAs() to bring up file dialog to save
   if (mFilename == GmatAppData::Instance()->GetTempScriptName())
   {
      OnSaveAs(event);
      if (mSaveCanceled)
      {
         #ifdef DEBUG_SAVE
         MessageInterface::ShowMessage
            ("GmatSavePanel::OnSave() leaving, user canceled SaveAs, mFilename='%s'\n",
             mFilename.WX_TO_C_STRING);
         #endif
         return;
      }
   }
   else if (!mSyncGui)
   {
      // Save inactive script and return
      SaveScript();
      mSyncGui = false;
      #ifdef DEBUG_SAVE
      MessageInterface::ShowMessage
         ("GmatSavePanel::OnSave() leaving, not synchronizing with GUI, %s script "
          "saved,\n   mFilename='%s'\n", mIsScriptActive ? "active" : "inactive",
          mFilename.WX_TO_C_STRING);
      #endif
      return;
   }
   
   // Confirm user if user wants to refresh GUI with script
   bool saveScript = false;
   if (guiStatus == 1)
   {
      saveScript = true;
   }
   // If GUI is dirty, prompt user to select an action
   else if (guiStatus == 2)
   {
      #ifdef DEBUG_SAVE
      MessageInterface::ShowMessage("   ==> Showing GUI overwrite confirm message\n");
      #endif
      wxString guiOverwriteMsg = "You will lose changes made in the GUI if the script "
         "is saved.  Do you want to save the script, discard the GUI changes, and reload "
         "the GUI with the saved script";
      if (event.GetEventObject() == mSaveSyncButton)
         guiOverwriteMsg = guiOverwriteMsg + "?";
      else if (event.GetEventObject() == mSaveSyncRunButton)
         guiOverwriteMsg = guiOverwriteMsg + " and run?";
      
      wxMessageDialog *msgDlg = new wxMessageDialog
         (this, guiOverwriteMsg, "Save script...",
          wxYES_NO | wxCANCEL |wxICON_QUESTION, wxDefaultPosition);
      
      int result = msgDlg->ShowModal();
      if (result == wxID_YES)
         saveScript = true;
      else
         mSyncGui = false;
      
      delete msgDlg;
   }
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("   saveScript=%d, mSyncGui=%d\n", saveScript, mSyncGui);
   #endif
   
   if (saveScript)
      SaveAndBuildScript(event);
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::OnSave() leaving, mFilename='%s', mSyncGui=%d\n",
       mFilename.WX_TO_C_STRING, mSyncGui);
   #endif
}


//------------------------------------------------------------------------------
// void OnSaveAs()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnSaveAs(wxCommandEvent &event)
{
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::OnSaveAs() entered, script = '%s', mIsScriptActive = %d\n",
       mFilename.WX_TO_C_STRING, mIsScriptActive);
   #endif
   
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
                       "Script files (*.script, *.m)|*.script;*.m|"
                       "Text files (*.txt, *.text)|*.txt;*.text|"
                       "All files (*.*)|*.*", wxFD_SAVE);
   
   bool saveScript = false;
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString path = dialog.GetPath().c_str();
      mSaveCanceled = false;
      
      if(DoesFileExist(path.c_str()))
      {
         if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
                          _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
         {
            mFilename = path;
            saveScript = true;
         }
		 else
			mSaveCanceled = true;
      }
      else
      {
         mFilename = path;
         saveScript = true;
      }
   }
   else
      mSaveCanceled = true;
   
   if (saveScript)      
      SaveScript();
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::OnSaveAs() leaving, saveScript=%d, mSaveCanceled=%d, script = '%s'\n",
       saveScript, mSaveCanceled, mFilename.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void OnClosePanel()
//------------------------------------------------------------------------------
/**
 * Closes panel.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnClosePanel(wxCommandEvent &event)
{
   #ifdef DEBUG_CLOSE_PANEL
   MessageInterface::ShowMessage
      ("GmatSavePanel::OnClosePanel() '%s' entered, mEditorModified=%d\n",
       mFilename.WX_TO_C_STRING, mEditorModified);
   #endif

   // We don't want to show duplicate save message when GmatMdiChildFrame is closing,
   // so set override dirty flag to false
   ((GmatMdiChildFrame*)
    (GmatAppData::Instance()->GetMainFrame()->GetActiveChild()))->OverrideDirty(false);
   
   if (mEditorModified)
      ((GmatMdiChildFrame*)
       (GmatAppData::Instance()->GetMainFrame()->GetActiveChild()))->SetDirty(true);
   else
      ((GmatMdiChildFrame*)
       (GmatAppData::Instance()->GetMainFrame()->GetActiveChild()))->SetDirty(false);
   
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void GmatSavePanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL)
   {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject);
   ssd.ShowModal();
}


//------------------------------------------------------------------------------
// void UpdateScriptActiveStatus(bool isActive)
//------------------------------------------------------------------------------
void GmatSavePanel::UpdateScriptActiveStatus(bool isActive)
{
   #ifdef DEBUG_UPDATE_ACTIVE_STATUS
   MessageInterface::ShowMessage
      ("GmatSavePanel::UpdateScriptActiveStatus() entered, isActive=%d, "
       "mFilename='%s'\n", isActive, mFilename.WX_TO_C_STRING);
   #endif
   
   mIsScriptActive = isActive;
   
   if (mShowScriptActiveStatus)
      RefreshScriptActiveStatus(mIsScriptActive);
}


//------------------------------------------------------------------------------
// bool UpdateStatusOnClose()
//
// @ return TRUE if panel can close
//------------------------------------------------------------------------------
bool GmatSavePanel::UpdateStatusOnClose()
{
      // For active script, update sync status
      if (mIsScriptActive & !mIsFunction)
      {
		  if (GmatAppData::Instance()->GetMainFrame()->IsMissionRunning())
		  {
           wxMessageBox("GMAT is running the mission.\n"
								"Please stop the mission first.",
                        wxT("Warning"), wxOK);
           return false;
		  }
		  //else
		  {
			 // clean up the status since we are going away
			 theGuiManager->SetActiveScriptStatus(1);
			 GmatAppData::Instance()->GetMainFrame()->
				 UpdateGuiScriptSyncStatus(theGuiManager->GetGuiStatus(),
                                       theGuiManager->GetActiveScriptStatus());
		  }
      }
	  return true;
}


//------------------------------------------------------------------------------
// void ReloadFile()
//------------------------------------------------------------------------------
/**
 * Reloads file contents.
 */
//------------------------------------------------------------------------------
void GmatSavePanel::ReloadFile()
{
   LoadData();
   mScriptDirtyLabel->SetLabel("");
}


//------------------------------------------------------------------------------
// void SetEditorModified(bool modified, updateSyncStatus)
//------------------------------------------------------------------------------
/**
 * Updates script modified status and GUI/Script sync status if updateSyncStatus
 * is true
 */
//------------------------------------------------------------------------------
void GmatSavePanel::SetEditorModified(bool modified, bool updateSyncStatus)
{
   #ifdef DEBUG_TEXT_MODIFIED
   MessageInterface::ShowMessage
      ("GmatSavePanel::SetEditorModified() entered, modified=%d, mEditorModified=%d, "
       "hasFileLoaded=%d, mIsScriptActive=%d\n", modified, mEditorModified, hasFileLoaded,
       mIsScriptActive);
   #endif
   
   // Update script modified status
   if (hasFileLoaded)
   {
      int scriptStatus = modified ? 2 : 1; // 1 = clean, 2 = dirty
      if (mEditorModified != modified)
      {
         if (scriptStatus == 1)
            mScriptDirtyLabel->SetLabel("");
         else
            mScriptDirtyLabel->SetLabel("**modified**");
      }
      
      // For active script, update sync status
      if (mIsScriptActive && updateSyncStatus)
      {
         theGuiManager->SetActiveScriptStatus(scriptStatus);
         GmatAppData::Instance()->GetMainFrame()->
            UpdateGuiScriptSyncStatus(0, scriptStatus);
      }
      
      mEditorModified = modified;
   }
   
   #ifdef DEBUG_TEXT_MODIFIED
   MessageInterface::ShowMessage
      ("GmatSavePanel::SetEditorModified() leaving, mEditorModified=%d\n", mEditorModified);
   #endif
}

//------------------------------------------------------------------------------
// bool DoesFileExist(const char *scriptFilename)
//------------------------------------------------------------------------------
bool GmatSavePanel::DoesFileExist(const char *scriptFilename)
{
   return DoesFileExist(std::string(scriptFilename));
}

//------------------------------------------------------------------------------
// bool DoesFileExist(const std::string &scriptFilename)
//------------------------------------------------------------------------------
bool GmatSavePanel::DoesFileExist(const std::string &scriptFilename)
{
   FILE * pFile;
   pFile = fopen (scriptFilename.c_str(),"rt+");
   if (pFile!=NULL)
   {
      fclose (pFile);
      return true;
   }
   else
      return false;
   
}


//------------------------------------------------------------------------------
// void MakeScriptActive(wxCommandEvent &event, bool isScriptModified)
//------------------------------------------------------------------------------
void GmatSavePanel::MakeScriptActive(wxCommandEvent &event, bool isScriptModified)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("GmatSavePanel::MakeScriptActive() entered, isScriptModified=%d\n",
       isScriptModified);
   #endif
   
   bool saveScript = true;
   mSyncGui = false;
   mDelayBuild = false;
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
   if (event.GetEventObject() == mSaveSyncButton ||
       event.GetEventObject() == mSaveSyncRunButton)
   {
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   Event is SaveSyncButton or SaveSyncRunButton\n");
      #endif
      if (gmatAppData->GetMainFrame()->IsAnimationRunning())
      {
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage("   Animation is running\n");
         #endif
         // @todo - Figure out why following happens. (LOJ: 2012.07.18)
         // It always runs mission before updating the resource tree.
         // It sometimes crashes when I tried SaveSyncRun again.
         // @note - Prompt user to stop animation first until issue is resolved.
         // Is there a timing issue? wxYield() didn't seem to solve the problem.
         // Use wxTimer?
         //===========================================================
         #if 0
         //===========================================================
         wxString action = "reload the GUI with the saved script?";
         if (event.GetEventObject() == mSaveSyncRunButton)
            action = "reload the GUI with the saved script and run?";
         
         int answer =
            wxMessageBox(wxT("GMAT is running the animation.\n"
                             "Are you sure you want to stop the animation and " + action),
                         wxT("Please confirm"), wxYES_NO);
         
         if (answer == wxYES)
         {
            // Stop animation
            gmatAppData->GetMainFrame()->StopAnimation();
         }
         else
            return;
         
         //===========================================================
         #else
         //===========================================================
        
         wxMessageBox("GMAT is running the animation.\n"
                      "Please stop the animation first.",
                      wxT("Warning"), wxOK);
         return;
         
         //===========================================================
         #endif
         //===========================================================
      }
      else
      {
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage("   Animation is not running\n");
         #endif
      }
      mSyncGui = true;
      mDelayBuild = true;
   }
   
   // This yield and the next one seem superfluous; leaving for now
   wxYield();
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("   mIsScriptActive=%d\n", mIsScriptActive);
   #endif
   
   // If this is not an active script, prompt the user for setting active
   if (!mIsScriptActive)
   {
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   ==> Showing active script confirm message\n");
      #endif
      wxString makeActiveMsg = "The script file \"" + mFilename + "\" is not the active script.  "
         "Would you like to save, make it the active script, and load it into the GUI";
      if (event.GetEventObject() == mSaveSyncButton)
         makeActiveMsg = makeActiveMsg + "?";
      else if (event.GetEventObject() == mSaveSyncRunButton)
         makeActiveMsg = makeActiveMsg + " and run?";
      
      wxMessageDialog *msgDlg = new wxMessageDialog
         (this, makeActiveMsg, "Save active...", wxYES_NO | wxICON_QUESTION, wxDefaultPosition);
      int result = msgDlg->ShowModal();
      
      if (result == wxID_YES)
      {
         mSyncGui = true;
         saveScript = true;
         // If inactive script is modified, set it dirty so that change can be saved
         // before build (Fix for GMT-2843, LOJ: 2012.08.22)
         if (isScriptModified)
            theGuiManager->SetActiveScriptStatus(2);
      }
      else
      {
         mSyncGui = false;
         saveScript = false;
      }
   }
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("   mSyncGui=%d, saveScript=%d, mScriptFilename='%s'\n", mSyncGui, saveScript,
       mScriptFilename.WX_TO_C_STRING);
   #endif
   
   // No action is performed if user said no to sync GUI
   // See GMAT Software Specification, Script Editor section (LOJ: 2012.02.22)
   if (saveScript)
   {
      OnSave(event);
      if (mSaveCanceled)
      {
         #ifdef DEBUG_SAVE
         MessageInterface::ShowMessage
            ("GmatSavePanel::OnSave() leaving, user canceled SaveAs, mFilename='%s'\n", mFilename.WX_TO_C_STRING);
         #endif
         return;
      }
   }
   
   // This yield and the previous one seem superfluous; leaving for now
   wxYield();
   
   // If continue building, set script file name and build script
   if (mSyncGui)
   {
      if (event.GetEventObject() == mSaveSyncButton)
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
         {
            #ifdef DEBUG_ACTIVE_SCRIPT
            MessageInterface::ShowMessage
               ("   ==> Calling GmatMainFrame::OnScriptBuildObject()\n");
            #endif
            gmatAppData->GetMainFrame()->OnScriptBuildObject(event);
         }
      }
      else if (event.GetEventObject() == mSaveSyncRunButton)
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
         {
            #ifdef DEBUG_ACTIVE_SCRIPT
            MessageInterface::ShowMessage
               ("   ==> Calling GmatMainFrame::OnScriptBuildAndRun()\n");
            #endif
            gmatAppData->GetMainFrame()->OnScriptBuildAndRun(event);
         }
      }
      // Make current script active script (Fix for GMT-206, LOJ: 2012.02.09)
      UpdateScriptActiveStatus(true);
      mSyncGui = false;
   }
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("GmatSavePanel::MakeScriptActive() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void RefreshScriptActiveStatus(bool isActive)
//------------------------------------------------------------------------------
void GmatSavePanel::RefreshScriptActiveStatus(bool isActive)
{
   if (mIsScriptActive)
   {
      ////theButtonSizer->Hide(theSaveButton);
      theButtonSizer->Layout();
      
      mScriptActiveLabel->SetLabel(" Active Script ");
      wxColour bgcolor = wxTheColourDatabase->Find("DIM GREY");
      mScriptActiveLabel->SetBackgroundColour(bgcolor);
      mScriptActiveLabel->SetForegroundColour(*wxGREEN);
   }
   else
   {
      ////theButtonSizer->Show(theSaveButton);
      theButtonSizer->Layout();
      
      mScriptActiveLabel->SetLabel(" Inactive Script ");
      wxColour bgcolor = wxTheColourDatabase->Find("LIGHT GREY");
      mScriptActiveLabel->SetBackgroundColour(bgcolor);
      mScriptActiveLabel->SetForegroundColour(*wxRED);
   }
}


//------------------------------------------------------------------------------
// void SaveScript()
//------------------------------------------------------------------------------
void GmatSavePanel::SaveScript()
{
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::SaveScript() entered\n   mFilename='%s'\n",
       mFilename.WX_TO_C_STRING);
   MessageInterface::ShowMessage("   Calling SaveData()\n");
   #endif
   
   SaveData();
   mScriptDirtyLabel->SetLabel("");
   SetEditorModified(false);
   
   if (mIsScriptActive)
   {
      GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
      mainFrame->SetActiveChildDirty(false);
      mainFrame->SetScriptFileName(mFilename.c_str());
      mainFrame->RefreshActiveScript(mFilename.c_str(), false);
      mainFrame->UpdateGuiScriptSyncStatus(2, 2);
   }
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::SaveScript() leaving\n   mFilename='%s'\n",
       mFilename.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void SaveAndBuildScript(wxCommandEvent &event)
//------------------------------------------------------------------------------
void GmatSavePanel::SaveAndBuildScript(wxCommandEvent &event)
{
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::SaveAndBuildScript() entered\n   mFilename='%s'\n",
       mFilename.WX_TO_C_STRING);
   #endif
   
   SaveData();
   mScriptDirtyLabel->SetLabel("");
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   SetEditorModified(false);
   gmatAppData->GetMainFrame()->SetActiveChildDirty(false);
   
   // If script is active, build the script 
   if (mIsScriptActive & !mDelayBuild)
   {
      if (gmatAppData->GetMainFrame()->SetScriptFileName(mFilename.c_str()))
         gmatAppData->GetMainFrame()->OnScriptBuildObject(event);
   }
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage
      ("GmatSavePanel::SaveAndBuildScript() leaving\n   mFilename='%s'\n",
       mFilename.WX_TO_C_STRING);
   #endif
}



