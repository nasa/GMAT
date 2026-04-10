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
//
/**
 * Implements ScriptEventPanel class.
 */
//------------------------------------------------------------------------------

#include "ScriptEventPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"       // for GetCommandSeqString()
#include "StringUtil.hpp"        // for MakeCommentLines()
#include "NoOp.hpp"
#include <algorithm>             // for sort(), set_difference()
#include <sstream>               // for std::stringstream

// This needs more working on size, so commented out
//#define __USE_SASH_WINDOW__

//#define DEBUG_CREATE 1
//#define DEBUG_COMMENTS
//#define DBGLVL_SEPANEL_CREATE 1
//#define DBGLVL_SEPANEL_LOAD 1
//#define DBGLVL_SEPANEL_SAVE 1
//#define DBGLVL_SEPANEL_REPLACE_CMDS 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ScriptEventPanel, GmatPanel)
   EVT_TEXT(ID_COMMENT_CTRL, ScriptEventPanel::OnCommentChange)
   EVT_TEXT(ID_SCRIPT_CTRL, ScriptEventPanel::OnCommentChange)
   EVT_SASH_DRAGGED (ID_SASH_WINDOW, ScriptEventPanel::OnSashDrag)
   EVT_SIZE (ScriptEventPanel::OnSize)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ScriptEventPanel()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ScriptEventPanel::ScriptEventPanel(wxWindow *parent, MissionTreeItemData *item)
   : GmatPanel(parent)
{
   theItem = item;
   theCommand = item->GetCommand();
   SetName("ScriptEventPanel");
   
   #if DBGLVL_SEPANEL
   ShowCommand("ScriptEventPanel() theCommand=", theCommand);
   #endif
   
   mPrevCommand = NULL;
   mNextCommand = NULL;
   mNewCommand = NULL;
   
   #ifdef __USE_STC_EDITOR__
   mEditor = NULL;
   #endif
   
   if (theCommand != NULL)
   {
     Create();
     Show();
   }
}


//------------------------------------------------------------------------------
// ~ScriptEventPanel()
//------------------------------------------------------------------------------
/**
 * The destructor.
 */
//------------------------------------------------------------------------------
ScriptEventPanel::~ScriptEventPanel()
{
   #ifdef __USE_STC_EDITOR__
   if (mEditor)
   {
      delete mEditor;
      mEditor = NULL;
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetEditorModified(bool isModified)
//------------------------------------------------------------------------------
/**
 * Sets the mEditorModified flag.
 *
 * @param <isModified>  value to set the mEditorModified flag to.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::SetEditorModified(bool isModified)
{
   #ifdef DEBUG_EDITOR_MODIFIED
   MessageInterface::ShowMessage
      ("ScriptEventPanel::SetEditorModified() entered, isModified=%d\n", isModified);
   #endif
   
   EnableUpdate(isModified);
   mEditorModified = isModified;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the ScriptEventPanel's GUI components.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::Create()
{
   #if DBGLVL_SEPANEL_CREATE
   MessageInterface::ShowMessage("ScriptEventPanel::Create() entered\n");
   #endif
   
   int bsize = 3; // border size
   
   //=================================================================
   #ifdef __USE_SASH_WINDOW__
   //=================================================================
   
   int w, h;
   
   // A window w/sash for comments
   theCommentsWin =
      new wxSashLayoutWindow(this, ID_SASH_WINDOW,
                             wxDefaultPosition, wxSize(-1, -1), wxSW_3D);
   // A window w/sash for scripts
   theScriptsWin =
      new wxSashLayoutWindow(this, -1,
                             wxDefaultPosition, wxSize(-1, -1), wxSW_3D);
   
   theCommentsWin->SetDefaultSize(wxSize(20, 70));
   theCommentsWin->SetMinimumSizeY(50);
   theCommentsWin->SetMaximumSizeY(350);
   theCommentsWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   theCommentsWin->SetAlignment(wxLAYOUT_TOP);
   theCommentsWin->SetSashVisible(wxSASH_BOTTOM, TRUE);
   
   theScriptsWin->SetMinimumSizeY(100);
   theScriptsWin->SetMaximumSizeY(500);
   theScriptsWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   theScriptsWin->SetAlignment(wxLAYOUT_BOTTOM);
   //theScriptsWin->SetSashVisible(wxSASH_TOP, TRUE);
   theScriptsWin->SetName("SashScriptEventPanel");
   
   // Comment label
   wxStaticText *commentText =
      new wxStaticText(this, ID_TEXT,
                       wxT("Comments"), wxDefaultPosition, wxDefaultSize, 0);
   // Comment text
   mCommentTextCtrl =
      new wxTextCtrl(theCommentsWin, ID_COMMENT_CTRL, wxT(""), wxDefaultPosition,
                     wxSize(-1, 70), wxTE_MULTILINE | wxTE_DONTWRAP);
   
   // BeginScript label
   wxStaticText *beginScriptText =
      new wxStaticText(this, ID_TEXT,
                       wxT("BeginScript;"), wxDefaultPosition, wxDefaultSize, 0);
   // EndScript label
   wxStaticText *endScriptText =
      new wxStaticText(this, ID_TEXT,
                       wxT("EndScript;"), wxDefaultPosition, wxDefaultSize, 0);
   // Change font color to blue
   beginScriptText->SetForegroundColour(*wxBLUE);
   endScriptText->SetForegroundColour(*wxBLUE);
   
#ifdef __USE_STC_EDITOR__
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage
      ("ScriptEventPanel::Create() Using sash window\n   Creating Editor from "
       "parent = <%p>'%s', theScriptsWin = <%s>'%s'\n", this, this->GetName().c_str(),
       theScriptsWin, theScriptsWin->GetName().c_str());
   #endif
   mEditor = new ScriptEditor(theScriptsWin, true, ID_STC);
#else
   // We don't want TextCtrl to wrap text, so add wxTE_DONTWRAP to style
   mFileContentsTextCtrl =
      new wxTextCtrl(theScriptsWin, ID_SCRIPT_CTRL, wxT(""), wxDefaultPosition,
                     wxDefaultSize, wxTE_MULTILINE | wxTE_DONTWRAP);
   
   mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
#endif
   
   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mPageSizer->Add(commentText, 0);
   mPageSizer->Add(theCommentsWin, 0, wxGROW);
   mPageSizer->Add(beginScriptText, 0);
   mPageSizer->Add(theScriptsWin, 1, wxGROW);
   mPageSizer->Add(endScriptText, 0);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   
   // Not using sash window
   //=================================================================
   #else
   //=================================================================
   
   theCommentsWin = NULL;
   theScriptsWin  = NULL;
   
   // Comment label
   wxStaticText *commentText =
      new wxStaticText(this, ID_TEXT,
                       wxT("Comments"), wxDefaultPosition, wxDefaultSize, 0);
   // Comment text
   mCommentTextCtrl =
      new wxTextCtrl(this, ID_COMMENT_CTRL, wxT(""), wxDefaultPosition,
                     wxSize(-1, 70), wxTE_MULTILINE | wxTE_DONTWRAP);
   
   // BeginScript label
   wxStaticText *beginScriptText =
      new wxStaticText(this, ID_TEXT,
                       wxT("BeginScript;"), wxDefaultPosition, wxDefaultSize, 0);
   // EndScript label
   wxStaticText *endScriptText =
      new wxStaticText(this, ID_TEXT,
                       wxT("EndScript;"), wxDefaultPosition, wxDefaultSize, 0);
   // Change font color to blue
   beginScriptText->SetForegroundColour(*wxBLUE);
   endScriptText->SetForegroundColour(*wxBLUE);
   
   #ifdef __USE_STC_EDITOR__
      #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage
         ("ScriptEventPanel::Create() Not using sash window\n   Creating Editor from "
          "parent = <%p>'%s'\n", this, this->GetName().c_str());
      #endif
      mEditor = new ScriptEditor(this, true, ID_STC);
   #else
      // We don't want TextCtrl to wrap text, so add wxTE_DONTWRAP to style
      mFileContentsTextCtrl =
         new wxTextCtrl(this, ID_SCRIPT_CTRL, wxT(""), wxDefaultPosition,
                        wxDefaultSize, wxTE_MULTILINE | wxTE_DONTWRAP);

      mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
   #endif
   
   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mPageSizer->Add(commentText, 0, wxALIGN_LEFT | wxALL, bsize);
   mPageSizer->Add(mCommentTextCtrl, 0, wxALIGN_LEFT | wxGROW | wxALL, bsize);
   mPageSizer->Add(beginScriptText, 0, wxALIGN_LEFT | wxALL, bsize);

   #ifdef __USE_STC_EDITOR__
      mPageSizer->Add(mEditor, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #else
      mPageSizer->Add(mFileContentsTextCtrl, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #endif

   mPageSizer->Add(endScriptText, 0, wxALIGN_LEFT | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   
   //=================================================================
   #endif
   //=================================================================
   
   #if DBGLVL_SEPANEL_CREATE
   MessageInterface::ShowMessage("ScriptEventPanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data into the panel's GUI widgets.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::LoadData()
{
   
   // Set the pointer for the "Show Script" button
   mObject = theCommand;
   
   #if DBGLVL_SEPANEL_LOAD
   MessageInterface::ShowMessage("ScriptEventPanel::LoadData()\n");
   MessageInterface::ShowMessage("     theCommand=%s\n", theCommand->GetTypeName().c_str());
   std::string cmdString = GmatCommandUtil::GetCommandSeqString(theCommand);
   MessageInterface::ShowMessage("     theCommand seuqence=%s\n", cmdString.c_str());
   
   GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
   cmdString = GmatCommandUtil::GetCommandSeqString(firstCmd);
   MessageInterface::ShowMessage("===> mission seuqence=%s\n", cmdString.c_str());
   
   mPrevCommand = theCommand->GetPrevious();
   ShowCommand("     mPrevCommand = ", mPrevCommand);
   #endif
   
   std::stringstream text;
   
   // We don't want to include Begin/EndScript,
   // so pass Gmat::GUI_EDITOR to GetGeneratingString()
   text << theCommand->GetGeneratingString(Gmat::GUI_EDITOR);
   // TGG: Fix for GMT-2982 
   // add comment symbols
   
   wxString comments = theCommand->GetCommentLine().c_str();
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage("LoadData() initial comments = \n'%s'\n", comments.c_str());
   #endif
   if (comments.StartsWith("% "))  // gets rid of first %
     comments = comments.Mid(2, comments.Length()-1);
   
   comments.Replace("\n% ", "\n");

   // Remove last \n since it adds extra blank line as part of comments when saved
   if (comments.EndsWith("\n"))
       comments = comments.Mid(0, comments.Length()-1);
   
   mCommentTextCtrl->AppendText(comments.c_str());
   mCommentTextCtrl->SetModified(false);
   
   wxString scriptText = text.str().c_str();
   // Remove last \n since it adds extra blank line as part of script when saved
   if (scriptText.EndsWith("\n"))
      scriptText = scriptText.Mid(0, scriptText.Length()-1);
   
   #ifdef __USE_STC_EDITOR__
      mEditor->AppendText(scriptText);
      mEditor->EmptyUndoBuffer();
      mEditor->SetSavePoint();
   #else
      mFileContentsTextCtrl->AppendText(scriptText);
      mFileContentsTextCtrl->SetModified(false);
   #endif
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/* Saves the data from the GUI widgets to the ScriptEvent.
 *
 * In order for ScriptEvent to save new scripts, it creates a new sequence of
 * commands and replaces the old sequence of commands via command->ForceSetNext()
 * and command->ForceSetPrevious().
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::SaveData()
{
   #if DBGLVL_SEPANEL_SAVE
   MessageInterface::ShowMessage("ScriptEventPanel::SaveData() Entered\n");
   #endif
   
   canClose = false;
   std::stringstream scriptText1;
   std::string oldName = theCommand->GetName();
   
   // If only the comment changed, just set the comment and return
   #ifdef __USE_STC_EDITOR__
   if (!mEditor->IsModified() && mCommentTextCtrl->IsModified())
   #else
   if (!mFileContentsTextCtrl->IsModified() && mCommentTextCtrl->IsModified())
   #endif
   {
      SaveComments();     
      
      mCommentTextCtrl->SetModified(false);
      EnableUpdate(false);
      canClose = true;
      #if DBGLVL_SEPANEL_SAVE
      MessageInterface::ShowMessage
         ("ScriptEventPanel::SaveData() only comment saved, just returning\n");
      #endif
      return;
   }
   
   //-----------------------------------------------------------------
   // Add lines to stringstream
   //-----------------------------------------------------------------
   scriptText1 << "BeginScript;" << "\n";
   //=======================================================
   #ifdef __USE_STC_EDITOR__
   //=======================================================
   
   #if DBGLVL_SEPANEL_SAVE
   int numLines = mEditor->GetLineCount();
   MessageInterface::ShowMessage("   number of lines=%d\n", numLines);
   #endif
   
   wxString text = mEditor->GetText();
   
   // Add EOL if not there.
   size_t length = text.Len();
   wxChar lastChar = text[length-1];
   
   if (lastChar != '\n' && lastChar != '\r' && lastChar != '\0')
      text = text + "\n";
   
   scriptText1 << text;
   
   //=======================================================
   #else
   //=======================================================
   
   int numLines = mFileContentsTextCtrl->GetNumberOfLines();
   wxString line;
   for (int i=0; i<numLines; i++)
   {      
      line = mFileContentsTextCtrl->GetLineText(i);
      scriptText1 << line << "\n";
   }
   
   //=======================================================
   #endif
   //=======================================================
   
   scriptText1 << "EndScript;" << "\n";
   
   
   //-----------------------------------------------------------------
   // Get previous command
   //-----------------------------------------------------------------
   std::string cmdString = GmatCommandUtil::GetCommandSeqString(theCommand);
   
   #if DBGLVL_SEPANEL_SAVE
   MessageInterface::ShowMessage
      ("   ==> Current BeginScript sequence=%s\n", cmdString.c_str());
   #endif
   
   // Get previous command
   mPrevCommand = theCommand->GetPrevious();
   
   #if DBGLVL_SEPANEL_SAVE
   ShowCommand("   mPrevCommand = ", mPrevCommand);
   ShowCommand("   theCommand->GetPrevious() = ", theCommand->GetPrevious());   
   #endif
   
   //-----------------------------------------------------------------
   // If previous command is NULL, just return
   //-----------------------------------------------------------------
   if (mPrevCommand == NULL)
   {      
      #if DBGLVL_SEPANEL_SAVE
      GmatCommand *firstCmd = theGuiInterpreter->GetFirstCommand();
      ShowCommand("   firstCmd = ", firstCmd);
      #endif
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "ScriptEventPanal::SaveData() *** Internal Error "
          "Occurred ***\nthe previous command is empty. Cannot continue.\n");
      
      return;
   }
   
   #if DBGLVL_SEPANEL_SAVE
   ShowCommand("   mPrevCommand = ", mPrevCommand);
   #endif
   
   //-----------------------------------------------------------------
   // Create temporary NoOp command, so that commands can be appended
   //-----------------------------------------------------------------
   GmatCommand *tempNoOp = new NoOp;
   GmatCommand *inCommand = tempNoOp;
   
   // Save old function list, so that new functions can be deleted if there
   // are script errors
   StringArray oldFunctions = theGuiInterpreter->GetListOfObjects(Gmat::FUNCTION);
   
   #if DBGLVL_SEPANEL_SAVE
   for (UnsignedInt i=0; i<oldFunctions.size(); i++)
      MessageInterface::ShowMessage
         ("   ==> oldFunctions[%d] = '%s'\n", i, oldFunctions[i].c_str());
   #endif
   
   try
   {
      //--------------------------------------------------------------
      // Set text lines to istringstream and interpret
      //--------------------------------------------------------------
      #if DBGLVL_SEPANEL_SAVE
      MessageInterface::ShowMessage
         ("   scriptText1 =\n%s\n", scriptText1.str().c_str());
      #endif
      
      std::istringstream *inStringStream = new std::istringstream;
      inStringStream->str(scriptText1.str());
      
      #if DBGLVL_SEPANEL_SAVE
      MessageInterface::ShowMessage
         ("====================> ScriptEvent calling "
          "theGuiInterpreter->Interpret(%p)\n", inCommand);
      #endif
      
      canClose = theGuiInterpreter->Interpret(inCommand, inStringStream);
      
      #if DBGLVL_SEPANEL_SAVE
      ShowCommand("   inCommand = ", inCommand);
      cmdString = GmatCommandUtil::GetCommandSeqString(inCommand);
      MessageInterface::ShowMessage
         ("   ==> temp noOp seuqence=%s\n", cmdString.c_str());
      MessageInterface::ShowMessage("   canClose = %d\n", canClose);
      #endif
      
      //--------------------------------------------------------------
      // If error occurred during interpretation, handle and return
      //--------------------------------------------------------------
      bool ignoreErrors = canClose;
      if (!canClose)
      {         
		  MessageInterface::PopupMessage
           (Gmat::ERROR_,"Errors were found in the ScriptEvent.  "
            "The script cannot be saved until all errors are fixed");
      }
      
      if (!ignoreErrors)
      {
         #if DBGLVL_SEPANEL_SAVE
         MessageInterface::ShowMessage
            ("   Deleting tempNoOp->GetNext()=%p\n", tempNoOp->GetNext());
         #endif
         
         //--------------------------------------------
         // Delete newly created GmatFunctions first
         // since any non-command text will create as
         // GmatFunctions, such as aaaa; or bbbb;
         //--------------------------------------------
         StringArray allFunctions = theGuiInterpreter->GetListOfObjects(Gmat::FUNCTION);
         #if DBGLVL_SEPANEL_SAVE
         for (UnsignedInt i=0; i<allFunctions.size(); i++)
            MessageInterface::ShowMessage
               ("   ==> allFunctions[%d] = '%s'\n", i, allFunctions[i].c_str());
         #endif
         
         sort(oldFunctions.begin(), oldFunctions.end());
         sort(allFunctions.begin(), allFunctions.end());
         
         StringArray newFunctions;
         // Make list of new functions
         set_difference(allFunctions.begin(), allFunctions.end(), oldFunctions.begin(),
                        oldFunctions.end(), back_inserter(newFunctions));
         
         for (UnsignedInt i=0; i<newFunctions.size(); i++)
         {
            #if DBGLVL_SEPANEL_SAVE
            MessageInterface::ShowMessage
               ("   ==> removing newFunctions[%d] = '%s'\n", i, newFunctions[i].c_str());
            #endif
            theGuiInterpreter->RemoveObjectIfNotUsed(Gmat::FUNCTION, newFunctions[i]);
         }
         
         //--------------------------------------------
         // Delete commands appended to BeginScript
         //--------------------------------------------
         GmatCommand *temp =
            theGuiInterpreter->DeleteCommand(tempNoOp->GetNext());
         
         if (temp != NULL)
         {
            temp->ForceSetNext(NULL);
            delete temp;
            temp = NULL;
         }
         
         #if DBGLVL_SEPANEL_SAVE
         MessageInterface::ShowMessage
            ("   New Script not saved. Deleting tempNoOp=%p\n", tempNoOp);
         #endif
         
         if (tempNoOp != NULL)
         {
            tempNoOp->ForceSetNext(NULL);
            delete tempNoOp;
            tempNoOp = NULL;
         }
         
         return;
      }
      
      //--------------------------------------------------------------
      // Everything is good to go, so delete temporary NoOp
      //--------------------------------------------------------------
      mNewCommand = tempNoOp->GetNext();
      
      #if DBGLVL_SEPANEL_SAVE
      ShowCommand("   saving  mNewCommand = ", mNewCommand);
      ShowCommand("   deleting tempNoOp   = ", tempNoOp);
      #endif
      
      tempNoOp->ForceSetNext(NULL);
      delete tempNoOp;
      tempNoOp = NULL;
      
      //--------------------------------------------------------------
      // Replace current ScriptEvent with new one
      //--------------------------------------------------------------
      ReplaceScriptEvent();
      mObject = mNewCommand;
      theCommand = mNewCommand;
      theCommand->SetName(oldName);
      
      #if DBGLVL_SEPANEL_SAVE
      ShowCommand("   After saving theCommand = ", theCommand);
      #endif
      
      // We want to save comments regardless it is modified or not since
      // new BeginScript command was created.
      // Fix for GMT-2611 (LOJ: 2012.10.05)
      SaveComments();
      
      // Since GmatMdiChildFrame uses this TextCtrl for checking
      // modified flag, set the flag to false
      SetEditorModified(false);
      #ifdef __USE_STC_EDITOR__
      // Do we need to do anything here?
      #else
      mFileContentsTextCtrl->SetModified(false);
      #endif
      
      mCommentTextCtrl->SetModified(false);
      EnableUpdate(false);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage() + 
         "\nError parsing the ScriptEvent; please correct the text");
      canClose = false;
   }
   
   #if DBGLVL_SEPANEL_SAVE
   MessageInterface::ShowMessage("ScriptEventPanel::SaveData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnCommentChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles an OnCommentChange event.
 *
 * @param <event>  event being handled.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::OnCommentChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnScriptChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles an OnScriptChange event.
 *
 * @param <event>  event being handled.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::OnScriptChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSashDrag(wxSashEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles an OnSashDrag event.
 *
 * @param <event>  event being handled.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::OnSashDrag(wxSashEvent &event)
{
   int w, h;
   GetClientSize(&w, &h);

   #ifdef DEBUG_SASH_DRAG
   MessageInterface::ShowMessage
      ("ScriptEventPanel::OnSashDrag() entered, clientW=%d, clientH=%d\n", w, h);
   #endif
   
   if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
      return;
   
   int newH = event.GetDragRect().height;
   
   #ifdef DEBUG_SASH_DRAG
   MessageInterface::ShowMessage("Setting comments new height to %d\n", newH);
   #endif
   
   // Sets size of comment window
   theCommentsWin->SetDefaultSize(wxSize(w, newH));
   
   wxLayoutAlgorithm layout;
   // Sets the rest of area to scripts window
   layout.LayoutWindow(this, theScriptsWin);
   
   theMiddleSizer->Layout();
   
   #ifdef DEBUG_SASH_DRAG
   MessageInterface::ShowMessage("ScriptEventPanel::OnSashDrag() leaving\n");
   #endif
}


// ------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles resizing of the window
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::OnSize(wxSizeEvent& event)
{
   int w, h;
   GetClientSize(&w, &h);

   #ifdef DEBUG_SIZE
   MessageInterface::ShowMessage("ScriptEventPanel::OnSize() entered\n");
   MessageInterface::ShowMessage("   client size w=%d, h=%d\n", w, h);
   #endif
   
   // adjust new maximum SashWindow size
   //@todo - Fix this to work (Why comments window disappears?)
   if (theCommentsWin != NULL && theScriptsWin != NULL)
   {
      int minH = 50;
      int maxH = 350;
      
      #ifdef DEBUG_SIZE
      int minComH = theCommentsWin->GetMinimumSizeY();
      int maxcomH = theCommentsWin->GetMaximumSizeY();
      int minScrH = theScriptsWin->GetMinimumSizeY();
      int maxScrH = theScriptsWin->GetMaximumSizeY();
      MessageInterface::ShowMessage("minComH=%d, maxcomH=%d\n", minComH, maxcomH);
      MessageInterface::ShowMessage("minScrH=%d, maxScrH=%d\n", minScrH, maxScrH);
      MessageInterface::ShowMessage("Setting comments min height to %d\n", minH);
      MessageInterface::ShowMessage("Setting comments max height to %d\n", maxH);
      #endif
      
      theCommentsWin->SetMinimumSizeY(minH);
      theCommentsWin->SetMaximumSizeY(maxH);
      
      wxLayoutAlgorithm layout;
      layout.LayoutWindow(this, theScriptsWin);
      
      theMiddleSizer->Layout();
   }
   
   event.Skip(true);
   
   #ifdef DEBUG_SIZE
   MessageInterface::ShowMessage("ScriptEventPanel::OnSize() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveComments()
//------------------------------------------------------------------------------
/**
 * Saves preface comments of BeginScript command.
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::SaveComments()
{   
   // TGG: Fix for GMT-2982 
   // add comment symbols
   wxString comments = mCommentTextCtrl->GetValue();
   std::string stdCmts = comments.WX_TO_STD_STRING;
   
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage
      ("SaveComments() comments from ctrl = \n'%s'\n", comments.c_str());
   #endif
   
   //if (comments.StartsWith("% "))  // gets rid of first %
   //   comments = comments.Mid(2, comments.Length()-1);
   //comments.Replace("\n% ", "\n");
   //comments.Replace("\n", "\n% ");
   //comments = "% " + comments;
   
   // Prepend % for non-blank lines only (LOJ: 2012.10.10)
   stdCmts = GmatStringUtil::MakeCommentLines(stdCmts.c_str());
   
   theCommand->SetCommentLine(stdCmts);
   
   #ifdef DEBUG_COMMENTS
   MessageInterface::ShowMessage
      ("SaveComments() comments saved = \n'%s'\n", stdCmts.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void ReplaceScriptEvent()
//------------------------------------------------------------------------------
/**
 * If a new command sequence was created, it replaces the old sequence of commands
 * with the new sequence via command->ForceSetNext() and command->ForceSetPrevious().
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::ReplaceScriptEvent()
{
   #if DBGLVL_SEPANEL_REPLACE_CMDS
   MessageInterface::ShowMessage("ScriptEventPanel::ReplaceScriptEvent() Entered\n");
   ShowCommand("   theCommand   = ", theCommand);
   ShowCommand("   mNewCommand  = ", mNewCommand);
   ShowCommand("   mPrevCommand = ", mPrevCommand);
   #endif
   
   if (theCommand != NULL && mPrevCommand != NULL && mNewCommand != NULL)
   {
      // Get first command from the mission sequence
      GmatCommand *first = theGuiInterpreter->GetFirstCommand();
      // Get parent of current ScriptEvent
      GmatCommand *parent = GmatCommandUtil::GetParentCommand(first, theCommand);
      // Find matching EndScript for new ScriptEvent
      GmatCommand *endScript = GmatCommandUtil::GetMatchingEnd(mNewCommand);
      
      if (parent == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
             "*** INTERNAL ERROR ***  parent is NULL\n");
         return;
      }
      
      #if DBGLVL_SEPANEL_REPLACE_CMDS
      ShowCommand("   first     = ", first);
      ShowCommand("   parent    = ", parent);
      ShowCommand("   endScript = ", endScript);
      #endif
      
      // If endScript found continue, otherwise internal error found      
      if (endScript)
      {
         //-----------------------------------------------------------
         // Get command after EndScript from current ScriptEvent
         //-----------------------------------------------------------
         mNextCommand = GmatCommandUtil::GetNextCommand(theCommand);
         
         #if DBGLVL_SEPANEL_REPLACE_CMDS
         ShowCommand("   mNextCommand = ", mNextCommand);
         #endif
         
         //-----------------------------------------------------------
         // Delete old ScriptEvent
         //-----------------------------------------------------------
         #if DBGLVL_SEPANEL_REPLACE_CMDS
         ShowCommand("   ==> calling gui->DeleteCommand() ", theCommand);
         #endif
         
         //Note: DeleteCommand() does not delete theCommand
         GmatCommand *temp = theGuiInterpreter->DeleteCommand(theCommand);
         
         if (temp != NULL)
         {
            temp->ForceSetNext(NULL);
            delete temp;
            temp = NULL;
         }
         
         #if DBGLVL_SEPANEL_REPLACE_CMDS
         MessageInterface::ShowMessage
            ("   ==> after Calling gui->DeleteCommand()\n");
         #endif
         
         //-----------------------------------------------------------
         // If this ScriptEvent is within BranchCommand, Insert Command
         //-----------------------------------------------------------
         if (parent->IsOfType("BranchCommand"))
         {
            #if DBGLVL_SEPANEL_REPLACE_CMDS
            ShowCommand("   insert commands to BranchCommand = ", parent);
            #endif
            
            GmatCommand *prevCmd = mPrevCommand;
            GmatCommand *current = mNewCommand;
            GmatCommand *next = NULL;
            
            // if previous command is BranchCommand or BeginScript,
            // find matching end command, and set it to previous command
            GmatCommand *realPrevCmd = prevCmd;
            if (prevCmd->GetTypeName() == "BeginScript")
            {
               realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               ShowCommand("   realPrevCmd = ", realPrevCmd);
               #endif
               
               prevCmd = realPrevCmd;
            }
            
            // insert commands in the new script event
            while (current != mNextCommand && current != NULL && prevCmd != NULL)
            {
               next = current->GetNext();
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               ShowCommand("   Inserting current = ", current);
               ShowCommand("   after     prevCmd = ", prevCmd);
               ShowCommand("             next    = ", next);
               #endif
               
               current->ForceSetNext(NULL);
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               MessageInterface::ShowMessage("   ==> calling gui->InsertCommand()\n");
               #endif
               
               realPrevCmd = prevCmd;
               
               //----------------------------------------------------------
               // Handle nested BranchCommand inside Begin/EndScript.
               // Since Insert BranchCommand inserts whole branch to command
               // sequence, if previous command is BranchCommand, and current
               // command is not BeginScript, find matching End BranchCommand
               // and insert after it.
               //----------------------------------------------------------
               if ((prevCmd->IsOfType("BranchCommand") &&
                    current->GetTypeName() != "BeginScript"))
               {
                  realPrevCmd = GmatCommandUtil::GetMatchingEnd(prevCmd);
                  
                  #if DBGLVL_SEPANEL_REPLACE_CMDS
                  ShowCommand("   realPrevCmd = ", realPrevCmd);
                  #endif

                  prevCmd = realPrevCmd;
               }
               
               theGuiInterpreter->InsertCommand(current, prevCmd);               
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               MessageInterface::ShowMessage
                  ("   ==> after calling gui->InsertCommand()\n");
               ShowCommand("   prevCmd->next = ", prevCmd->GetNext());
               
               std::string cmdString = GmatCommandUtil::GetCommandSeqString(first);
               MessageInterface::ShowMessage
                  ("===> new sequence=%s\n", cmdString.c_str());
               #endif
               
               prevCmd = current;
               current = next;
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               ShowCommand("   Inserting current = ", current);
               ShowCommand("   after     prevCmd = ", prevCmd);
               #endif
            } // end while
            
            mNewCommand->ForceSetPrevious(mPrevCommand);
         }
         //-----------------------------------------------------------
         // Else, just set previous/next pointer
         //-----------------------------------------------------------
         else
         {
            #if DBGLVL_SEPANEL_REPLACE_CMDS
            MessageInterface::ShowMessage
               ("   Set previous/next command of BeginScript\n");
            #endif
            // Set previous/next command of BeginScript
            if (mPrevCommand->IsOfType("BranchEnd"))
            {
               // If previous command is BranchEnd, find matching parent of
               // BranchEnd and set BranchEnd's next command to matching parent, then
               // set next command of matching parent to new command
               GmatCommand *subParent =
                  GmatCommandUtil::GetParentCommand(first, mPrevCommand);
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               ShowCommand("   Setting ", subParent, " as next of ", mPrevCommand);
               #endif
               mPrevCommand->ForceSetNext(subParent);
               
               #if DBGLVL_SEPANEL_REPLACE_CMDS
               ShowCommand("   Setting ", mNewCommand, " as next of ", subParent);
               #endif
               subParent->ForceSetNext(mNewCommand);
            }
            else
            {
               mPrevCommand->ForceSetNext(mNewCommand);
               mNewCommand->ForceSetPrevious(mPrevCommand);
            }
            
            // Set previous/next command of EndScript
            if (mNextCommand != NULL)
               mNextCommand->ForceSetPrevious(endScript);
            
            endScript->ForceSetNext(mNextCommand);
         }
         
         //-----------------------------------------------------------
         // Set new ScriptEvent to item command
         //-----------------------------------------------------------
         theItem->SetCommand(mNewCommand);
         
         //-----------------------------------------------------------
         // Some sanity check
         //-----------------------------------------------------------
         if (mNewCommand->GetPrevious() != mPrevCommand)
         {
            if (!mPrevCommand->IsOfType("BranchEnd"))
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
                   "*** INTERNAL ERROR ***  \nmNewCommand->GetPrevious() != "
                   "mPrevCommand\n");
               //ShowCommand("mPrevCommand = ", mPrevCommand);
            }
         }
         
         if (mNewCommand->GetNext() == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
                "*** INTERNAL ERROR ***  mNewCommand->GetNext() == NULL\n");
         }
         else
         {
            #if DBGLVL_SEPANEL_REPLACE_CMDS
            std::string cmdString = GmatCommandUtil::GetCommandSeqString(first);    
            MessageInterface::ShowMessage
               ("===> final new sequence=%s\n", cmdString.c_str());
            #endif
         }
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "ScriptEventPanel::ReplaceScriptEvent() "
             "*** INTERNAL ERROR ***  last command should not be NULL\n");
      }
   }
   
   #if DBGLVL_SEPANEL_REPLACE_CMDS
   MessageInterface::ShowMessage
      ("ScriptEventPanel::ReplaceScriptEvent() Leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// void ShowCommand(const std::string &title1, GmatCommand *cmd1,
//                  const std::string &title2, GmatCommand *cmd2)
//------------------------------------------------------------------------------
/**
 * Shows the command information.
 *
 * @param <title1>  title of first command
 * @param <cmd1>    first command
 * @param <title2>  title of second command
 * @param <cmd2>    second command
 */
//------------------------------------------------------------------------------
void ScriptEventPanel::ShowCommand(const std::string &title1, GmatCommand *cmd1,
                                   const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage("%s(%p)NULL\n", title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str());
   }
   else
   {
      if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s(%p)NULL%s(%p)NULL\n", title1.c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str(),
             title2.c_str(), cmd2, cmd2->GetTypeName().c_str());
   }
}

