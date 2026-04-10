//$Id$
//------------------------------------------------------------------------------
//                                     ScriptEditor
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
// Created: 2009/01/05
//
/**
 * Implements ScriptEditor class.
 */
// Renamed Editor to CodeEditor to fix name collision with wxWidget's Scintilla lib
//------------------------------------------------------------------------------

#include "ScriptEditor.hpp"
#include "GmatMainFrame.hpp"
#include "GmatSavePanel.hpp"
#include "ScriptEventPanel.hpp"
#include "FunctionSetupPanel.hpp"
#include "FindReplaceDialog.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <wx/filename.h>           // for wxFileName
#include <wx/numdlg.h>             // for wxGetNumberFromUser

//#define DEBUG_EDITOR
//#define DEBUG_GMAT_KEYWORDS
//#define DEBUG_EDITOR_PREF
//#define DEBUG_EDITOR_FIND
//#define DEBUG_EDITOR_COMMENT

BEGIN_EVENT_TABLE (ScriptEditor, wxStyledTextCtrl)
   // common
   EVT_SIZE (                         ScriptEditor::OnSize)
   // edit
   EVT_MENU (wxID_CLEAR,              ScriptEditor::OnClear)
   EVT_MENU (wxID_CUT,                ScriptEditor::OnCut)
   EVT_MENU (wxID_COPY,               ScriptEditor::OnCopy)
   EVT_MENU (wxID_PASTE,              ScriptEditor::OnPaste)
   EVT_MENU (STC_ID_INDENTMORE,       ScriptEditor::OnIndentMore)
   EVT_MENU (STC_ID_INDENTLESS,       ScriptEditor::OnIndentLess)
   EVT_MENU (wxID_SELECTALL,          ScriptEditor::OnSelectAll)
   EVT_MENU (STC_ID_SELECTLINE,       ScriptEditor::OnSelectLine)
   EVT_MENU (wxID_REDO,               ScriptEditor::OnRedo)
   EVT_MENU (wxID_UNDO,               ScriptEditor::OnUndo)
   // find, replace, goto
   EVT_MENU (wxID_FIND,               ScriptEditor::OnFind)
   EVT_MENU (STC_ID_FINDNEXT,         ScriptEditor::OnFindNext)
   EVT_MENU (STC_ID_REPLACENEXT,      ScriptEditor::OnReplaceNext)
   EVT_MENU (STC_ID_REPLACEALL,       ScriptEditor::OnReplaceAll)
   EVT_MENU (STC_ID_BRACEMATCH,       ScriptEditor::OnBraceMatch)
   EVT_MENU (STC_ID_GOTO,             ScriptEditor::OnGoToLine)
   // view
   EVT_MENU_RANGE (STC_ID_HILIGHTFIRST, STC_ID_HILIGHTLAST,
                                      ScriptEditor::OnHilightLang)
   EVT_MENU (STC_ID_DISPLAYEOL,       ScriptEditor::OnDisplayEOL)
   EVT_MENU (STC_ID_INDENTGUIDE,      ScriptEditor::OnIndentGuide)
   EVT_MENU (STC_ID_LINENUMBER,       ScriptEditor::OnLineNumber)
   EVT_MENU (STC_ID_LONGLINEON,       ScriptEditor::OnLongLineOn)
   EVT_MENU (STC_ID_WHITESPACE,       ScriptEditor::OnWhiteSpace)
   EVT_MENU (STC_ID_FOLDTOGGLE,       ScriptEditor::OnFoldToggle)
   EVT_MENU (STC_ID_OVERTYPE,         ScriptEditor::OnSetOverType)
   EVT_MENU (STC_ID_READONLY,         ScriptEditor::OnSetReadOnly)
   EVT_MENU (STC_ID_WRAPMODEON,       ScriptEditor::OnWrapmodeOn)
   EVT_MENU (STC_ID_CHARSETANSI,      ScriptEditor::OnUseCharset)
   EVT_MENU (STC_ID_CHARSETMAC,       ScriptEditor::OnUseCharset)
   // extra
   EVT_MENU (STC_ID_CHANGELOWER,      ScriptEditor::OnChangeCase)
   EVT_MENU (STC_ID_CHANGEUPPER,      ScriptEditor::OnChangeCase)
   EVT_MENU (STC_ID_CONVERTCR,        ScriptEditor::OnConvertEOL)
   EVT_MENU (STC_ID_CONVERTCRLF,      ScriptEditor::OnConvertEOL)
   EVT_MENU (STC_ID_CONVERTLF,        ScriptEditor::OnConvertEOL)
   // stc
   EVT_STC_MARGINCLICK (wxID_ANY,     ScriptEditor::OnMarginClick)
   EVT_STC_CHANGE      (wxID_ANY,     ScriptEditor::OnTextChange)
   EVT_STC_CHARADDED   (wxID_ANY,     ScriptEditor::OnCharAdded)
   EVT_STC_STYLENEEDED (wxID_ANY,     ScriptEditor::OnStyleNeeded)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ScriptEditor(wxWindow *parent, bool notifyChange, wxWindowID id,
//        const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param  parent		Parent window pointer
 * @param  notifyChange	Set this to true if parent class to be notified of change
 * @param  id			wxWidgets ID to assign to the Editor
 * @param  pos			Position of the editor 
 * @param  size			Size of the editor
 * @param  style		Editor Window style
 *
 */
//------------------------------------------------------------------------------
ScriptEditor::ScriptEditor(wxWindow *parent, bool notifyChange, wxWindowID id,
               const wxPoint &pos, const wxSize &size, long style)
   : wxStyledTextCtrl(parent, id, pos, size, style)
{
   #ifdef DEBUG_EDITOR
   MessageInterface::ShowMessage
      ("ScriptEditor::ScriptEditor() entered, parent = <%p>'%s', notifyChange = %d\n",
       parent, parent->GetName().c_str(), notifyChange);
   #endif
   
   mParent = (GmatPanel*)parent;
   mNotifyChange = notifyChange;
   mFileName = wxEmptyString;
   mFindReplaceDialog = NULL;
   
   mLineNumberID = 0;
   mDividerID = 1;
   mFoldingID = 2;
   mPrevLineNumber = 1;
   mLastSelectPos = -1;
   mLastFindPos = -1;

   mInitializeHighlights = true;
   mIsStringBlock = false;
   mStringBlockLines = 0;

   mPrevLineCount = GetNumberOfLines();
   
   // initialize language
   mLanguage = NULL;
   
   // Set EOL mode to wxSTC_EOL_LF. Why wxSTC_EOL_CRLF keep adding blank line?
   // wxSTC_EOL_CRLF (Windows), wxSTC_EOL_CR (Macintosh) and wxSTC_EOL_LF (Linux)
   SetEOLMode(wxSTC_EOL_LF);

   // default font for all styles
   SetViewEOL(GmatEditor::globalCommonPrefs.displayEOLEnable);
   SetIndentationGuides(GmatEditor::globalCommonPrefs.indentGuideEnable);
   SetEdgeMode(GmatEditor::globalCommonPrefs.longLineOnEnable?
               wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
   SetViewWhiteSpace(GmatEditor::globalCommonPrefs.whiteSpaceEnable?
                     wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
   SetOvertype(GmatEditor::globalCommonPrefs.overTypeInitial);
   SetReadOnly(GmatEditor::globalCommonPrefs.readOnlyInitial);
   SetWrapMode(GmatEditor::globalCommonPrefs.wrapModeInitial?
               wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
   wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
   StyleSetFont(wxSTC_STYLE_DEFAULT, font);
   StyleSetForeground(wxSTC_STYLE_DEFAULT, *wxBLACK);
   StyleSetBackground(wxSTC_STYLE_DEFAULT, *wxWHITE);
   StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(_T("DARK GREY")));
   StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
   StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(_T("DARK GREY")));
      
   // set visibility
   SetVisiblePolicy(wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
   SetXCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
   SetYCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
   
   // markers
   MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_DOTDOTDOT, _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN, _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_DOTDOTDOT, _T("BLACK"), _T("WHITE"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, _T("BLACK"), _T("WHITE"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   
   // miscelaneous
   mLineNumberMargin = TextWidth(wxSTC_STYLE_LINENUMBER, _T("_999999"));
   mFoldingMargin = 16;
   // TGG: 2012-08-14, GMT-1854, Script Editor Tab Key does nothing
   // Commented out CmdKeyClear(wxSTC_KEY_TAB, 0); // this is done by the menu accelerator key
   SetLayoutCache(wxSTC_CACHE_PAGE);
   
   // Get object type namses to be used as keywords
   GuiInterpreter *guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   ObjectTypeArray excList;
   excList.push_back(Gmat::PARAMETER);
   excList.push_back(Gmat::MATH_NODE);
   excList.push_back(Gmat::COMMAND);
   
   std::string objCreatables = guiInterpreter->GetStringOfAllFactoryItemsExcept(excList);
   objCreatables = objCreatables + "Propagator ";
   mGmatObjectTypes = objCreatables.c_str();
   
   std::string cmdCreatables = guiInterpreter->GetStringOfAllFactoryItems(Gmat::COMMAND);
   mGmatCommandTypes = cmdCreatables.c_str();
   std::string keywords = objCreatables + cmdCreatables;
   mGmatKeywords = keywords.c_str();

   mObjCreatablesArray = guiInterpreter->GetListOfAllFactoryItemsExcept(excList);
   mObjCreatablesArray.push_back("Propagator");
   mCmdCreatablesArray = guiInterpreter->GetListOfFactoryItems(Gmat::COMMAND);
   
   #ifdef DEBUG_GMAT_KEYWORDS
   MessageInterface::ShowMessage
      ("==> Here is the list of object types:\n%s\n", mGmatObjectTypes.c_str());
   MessageInterface::ShowMessage
      ("==> Here is the list of command types:\n%s\n", mGmatCommandTypes.c_str());
   #endif
   
   // set GMAT language keywords
   bool found = false;
   GmatEditor::LanguageInfoType *curInfo;
   
   #ifdef DEBUG_GMAT_KEYWORDS
   MessageInterface::ShowMessage
      ("There are %d languages defined\n", GmatEditor::globalLanguagePrefsSize);
   #endif
   
   for (int index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = (GmatEditor::LanguageInfoType*)&GmatEditor::globalLanguagePrefs[index];
      wxString name = curInfo->name;
      
      if (name == "GMAT")
      {
         #ifdef DEBUG_GMAT_KEYWORDS
         MessageInterface::ShowMessage
            ("   ==> Found GMAT language, so setting new keywords for matlab "
             "style (see EditorPreference.cpp)\n");
         #endif
         //@note LEX language for GMAT is set in globalLanguagePrefs in EditorPreference.cpp,
         // it is currently set to MATLAB
         #if 1
         // wxSTC_LEX_MATLAB (Matlab style) (Why this doesn't show colors for keywords)
         //curInfo->styles[2].words = mGmatCommandTypes.c_str(); // COMMAND
         //curInfo->styles[4].words = mGmatObjectTypes.c_str();  // KEYWORDS
         curInfo->styles[4].words = mGmatKeywords.c_str();  // KEYWORDS
         #endif
         #if 0
         // wxSTC_LEX_PYTHON (Python style)
         // Why I cannot set two types of keywords?
         //curInfo->styles[5].words = mGmatCommandTypes.c_str(); // WORD
         //curInfo->styles[9].words = mGmatObjectTypes.c_str();  // DEFNAME
         curInfo->styles[5].words = mGmatKeywords.c_str(); // WORD
         #endif
         found = true;
         break;
      }
   }
   
   #ifdef DEBUG_GMAT_KEYWORDS
   if (!found)
      MessageInterface::ShowMessage("==> GMAT language not found!!\n");
   #endif
   
   // Let's set it to GMAT preference
   InitializePrefs("GMAT");
   
   #ifdef DEBUG_EDITOR
   MessageInterface::ShowMessage("ScriptEditor::ScriptEditor() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~ScriptEditor()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
ScriptEditor::~ScriptEditor()
{
   if (mFindReplaceDialog)
      delete mFindReplaceDialog;
   
   mFindReplaceDialog = NULL;
}


//------------------------------------------------------------------------------
// void SetFindText(const wxString &text)
//------------------------------------------------------------------------------
/**
 * Sets the Find (Search) Text
 * 
 * @param text	Text to search for
 */
void ScriptEditor::SetFindText(const wxString &text)
{
   mFindText = text;
}


//------------------------------------------------------------------------------
// void SetReplaceText(const wxString &text)
//------------------------------------------------------------------------------
/**
 * Sets the Replace Text
 * 
 * @param text	Text to use for replacement
 */
void ScriptEditor::SetReplaceText(const wxString &text)
{
   mReplaceText = text;
}


//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for when ScriptEditor size is changed
 *
 * @param event		holds information about the size change event
 */
void ScriptEditor::OnSize(wxSizeEvent& event)
{
   int w = 0;
   int maxw = 0;
   // this code assumes no font size changes
   for (int line=0;line<GetLineCount();line++)
   {
	   w = TextWidth(0, GetLine(line));
	   if (w > maxw)
		   maxw = w;
   }
   // TGG: This code is wrong as GetClientSize has nothing to do with line width
   // Unfortunately, the SetScrollWidthTracking method was not exposed
   //int x = GetClientSize().x +
   //  (GmatEditor::globalCommonPrefs.lineNumberEnable? mLineNumberMargin: 0) +
   //  (GmatEditor::globalCommonPrefs.foldEnable? mFoldingMargin: 0);
   
   if (maxw > 0)
      SetScrollWidth(maxw);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnFont(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for when font is changed
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnFont(wxCommandEvent& event)
{
   //@todo - need implementation
}


// edit event handlers
//------------------------------------------------------------------------------
// void OnRedo(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to redo changes
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnRedo(wxCommandEvent &WXUNUSED(event))
{
   if (!CanRedo())
      return;
   
   Redo();
}


//------------------------------------------------------------------------------
// void OnUndo(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to undo changes
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnUndo(wxCommandEvent &WXUNUSED(event))
{
   if (!CanUndo())
      return;
   Undo();
}

//------------------------------------------------------------------------------
// void OnClear(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to be cleared
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnClear(wxCommandEvent &WXUNUSED(event))
{
   if (GetReadOnly())
      return;
   
   Clear();
}


//------------------------------------------------------------------------------
// void OnCut(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to cut selected text to clipboard
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnCut(wxCommandEvent &WXUNUSED(event))
{
   if (GetReadOnly() ||(GetSelectionEnd()-GetSelectionStart() <= 0))
      return;
   
   Cut();
}


//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to copy selected text to clipboard
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnCopy(wxCommandEvent &WXUNUSED(event))
{
   if (GetSelectionEnd()-GetSelectionStart() <= 0)
      return;
   
   Copy();
}


//------------------------------------------------------------------------------
// void OnPaste(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to paste clipboard contents into the
 * editor at the selection point
 *
 * @param event		command event originated by the control
 */
void ScriptEditor::OnPaste(wxCommandEvent &WXUNUSED(event))
{
   if (!CanPaste())
      return;
   
   Paste();
}


//------------------------------------------------------------------------------
// void OnFind(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to find text within editor contents
 *
 * @param event		command event originated by the control
 *
 * @note The find operation is not performed here.  Rather the find dialog is 
 * displayed
 */
void ScriptEditor::OnFind(wxCommandEvent &WXUNUSED(event))
{
   if (mFindReplaceDialog == NULL)
   {
      mFindReplaceDialog = new FindReplaceDialog(this, -1, "Find and Replace");
      
      #ifdef DEBUG_EDITOR_FIND
      MessageInterface::ShowMessage
         ("ScriptEditor::OnFind() FindReplaceDialog <%p> created\n", mFindReplaceDialog);
      #endif
      
      // show modeless dialog
      mFindReplaceDialog->Show(true);
      
      // set this as Editor
      mFindReplaceDialog->SetEditor(this);
      
      // set the current caret position to the position of the search anchor. 
      SearchAnchor();
   }
   else
   {
      // show modeless dialog
      mFindReplaceDialog->Show(true);
   }
}


//------------------------------------------------------------------------------
// void OnFindNext(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to find next occurence of the search
 * string from the current position
 *
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnFindNext(wxCommandEvent &WXUNUSED(event))
{
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("ScriptEditor::OnFindNext() entered, mFindReplaceDialog=<%p>, mFindText='%s'\n",
       mFindReplaceDialog, mFindText.c_str());
   #endif
   
   int i, cPos, ePos;
   cPos = GetCurrentPos();
   ePos = GetAnchor();
   if (cPos > ePos)
   {
	   SetCurrentPos(cPos);
	   SetAnchor(cPos);
   }
   else
   {
	   SetCurrentPos(ePos);
	   SetAnchor(ePos);
   }
   SearchAnchor();
      
   #ifdef DEBUG_EDITOR_FIND
   int anchorPos = GetAnchor();
   MessageInterface::ShowMessage("   anchorPos=%d\n", anchorPos);
   #endif
   
   // Find some text starting at the search anchor.
   // This does not ensure the selection is visible.
   mLastFindPos = SearchNext(0, mFindText);
   // make sure caret is at end of selection
   cPos = GetCurrentPos();
   ePos = GetAnchor();
   if (cPos < ePos) 
   {
	   i = cPos;
	   SetCurrentPos(ePos);
	   SetAnchor(i);
   }

   int line = GetCurrentLine();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("   mLastFindPos=%d, line=%d\n", mLastFindPos, line);
   #endif
   
   // Scroll to line to make line with selection visible
   int scrollOffset = LinesOnScreen();
   ScrollToLine(line - (int)(scrollOffset/2));
   
   // Save selection end for the next search
   mLastSelectPos = GetSelectionEnd();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage("mLastSelectPos=%d\n", mLastSelectPos);
   #endif

   MarkerDefine(1, wxSTC_MARK_BACKGROUND, _T("YELLOW"), _T("YELLOW"));
   //MarkerDefine(2, wxSTC_MARK_CIRCLE, _T("RED"), _T("RED"));
   MarkerAdd(3, 1);
   //MarkerAdd(3, 2);
}


//------------------------------------------------------------------------------
// void OnFindPrev(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when when the editor is requested to find previous occurence of the search
 * string from the current position
 *
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnFindPrev(wxCommandEvent &WXUNUSED(event))
{
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("ScriptEditor::OnFindPrev() entered, mFindReplaceDialog=<%p>, mFindText='%s'\n",
       mFindReplaceDialog, mFindText.c_str());
   #endif
   
   // make sure caret is at beginning of selection
   int cPos = GetCurrentPos();
   int ePos = GetAnchor();
   if (cPos > ePos) 
   {
	   int i = cPos;
	   SetCurrentPos(ePos);
	   SetAnchor(i);
   }
   SearchAnchor();
   
   #ifdef DEBUG_EDITOR_FIND
   int anchorPos = GetAnchor();
   MessageInterface::ShowMessage("   anchorPos=%d\n", anchorPos);
   #endif
   
   // Find some text starting at the search anchor and moving backwards.
   // This does not ensure the selection is visible.
   mLastFindPos = SearchPrev(0, mFindText);
   int line = GetCurrentLine();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("    mLastFindPos=%d, line=%d\n", mLastFindPos, line);
   #endif
   
   // Scroll to line to make line with selection visible
   int scrollOffset = LinesOnScreen();
   ScrollToLine(line - (int)(scrollOffset/2));
   
   // Save selection start for the next search
   mLastSelectPos = GetSelectionStart();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage("mLastSelectPos=%d\n", mLastSelectPos);
   #endif
}


//------------------------------------------------------------------------------
// void OnReplaceNext(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to replace next occurence of the search
 * string (from the current position) with the replacement string
 *
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnReplaceNext(wxCommandEvent &event)
{
	if (mFindText != GetSelectedText())
      OnFindNext(event);

   // if text to replace not found, return
   if (mFindText != GetSelectedText())
      return;
   
   // Replace the selected text with the argument text.
   ReplaceSelection(mReplaceText);
   OnFindNext(event);
}


//------------------------------------------------------------------------------
// void OnReplaceAll(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to replace ALL occurences of the search
 * string with the replacement string within the editor contents
 *
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnReplaceAll(wxCommandEvent &event)
{
   GotoPos(0);
   OnFindNext(event);
   while (mFindText.IsSameAs(GetSelectedText(), false))
   {
      ReplaceSelection(mReplaceText);
      OnFindNext(event);
   }
}


//------------------------------------------------------------------------------
// void OnBraceMatch(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to find a matching brace
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnBraceMatch(wxCommandEvent &WXUNUSED(event))
{
   int min = GetCurrentPos();
   int max = BraceMatch(min);
   if (max >(min+1))
   {
      BraceHighlight(min+1, max);
      SetSelection(min+1, max);
   }
   else
   {
      BraceBadLight(min);
   }
}


//------------------------------------------------------------------------------
// void OnGoToLine(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to go to a specific line number
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnGoToLine(wxCommandEvent &WXUNUSED(event))
{
   long lineNumber = wxGetNumberFromUser
      ("", "Line Number", "Go To", mPrevLineNumber, 1, 100000, this);
   
   if (lineNumber != -1)
   {
      mPrevLineNumber = lineNumber;      
      GotoLine(lineNumber-1);
   }
}


//------------------------------------------------------------------------------
// void OnIndentMore(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to indent the selected text/current
 * line
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnIndentMore(wxCommandEvent &WXUNUSED(event))
{
   // TGG: 2012-08-14, bug GMT-2979, Script editor comment/indent commands don't work in a standard way
   // fix for when cursor/selection on same line, indent doesn't work
   int cPos = GetCurrentPos();
   int ePos = GetAnchor();

   int sLine = LineFromPosition(cPos);
   int eLine = LineFromPosition(ePos);

   if (sLine == eLine)
   {
	   Home();
   }

   CmdKeyExecute(wxSTC_CMD_TAB);
}


//------------------------------------------------------------------------------
// void OnIndentLess(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to unindent the selected text/current
 * line
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnIndentLess(wxCommandEvent &WXUNUSED(event))
{
   // TGG: 2012-08-14, bug GMT-2979, Script editor comment/indent commands don't work in a standard way
   // fix for when cursor/selection on same line, indent doesn't work
   int cPos = GetCurrentPos();
   int ePos = GetAnchor();

   int sLine = LineFromPosition(cPos);
   int eLine = LineFromPosition(ePos);

   if (sLine == eLine)
   {
	   Home();
   }

   CmdKeyExecute(wxSTC_CMD_BACKTAB);
}


//------------------------------------------------------------------------------
// void OnSelectAll(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to select the entire contents
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnSelectAll(wxCommandEvent &WXUNUSED(event))
{
   SetSelection(0, GetTextLength());
}


//------------------------------------------------------------------------------
// void OnSelectLine(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to select current line
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnSelectLine(wxCommandEvent &WXUNUSED(event))
{
   int lineStart = PositionFromLine(GetCurrentLine());
   int lineEnd = PositionFromLine(GetCurrentLine() + 1);
   SetSelection(lineStart, lineEnd);
}


//------------------------------------------------------------------------------
// void OnComment(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to comment selected lines/
 * current line
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnComment(wxCommandEvent &event)
{
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnComment() entered\n");
   #endif
   
   // TGG: 2012-08-14, bug GMT-2979, Script editor comment/indent commands don't work in a standard way
   // set up selection so it selects entire lines so replace works
   int cPos = GetCurrentPos();
   int ePos = GetAnchor();
   int i;
   if (cPos > ePos) 
   {
	   LineEnd();
	   i = GetCurrentPos();
	   SetCurrentPos(ePos);
	   ePos = i;
	   Home();
	   cPos = GetCurrentPos();
   }
   else
   {
	   Home();
	   cPos = GetCurrentPos();
	   SetCurrentPos(ePos);
	   LineEnd();
	   ePos = GetCurrentPos();
	
   }
   SetSelection(cPos, ePos);
   // Retrieve the selected text.
   wxString selString = GetSelectedText();

   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnComment() SelText='%s'\n", selString.c_str());
   #endif

   wxString newString = "% ";
   size_t n = 0;
   while ( n < selString.Length() )
   {
	   if ((selString.GetChar(n) == '\n') || (selString.GetChar(n) == '\r'))
	   {
		   newString.Append(selString.GetChar(n));
		   if (((n+1) < selString.Length()) && ((selString.GetChar(n+1) == '\n') || (selString.GetChar(n+1) == '\r')))
		   {
			   n = n + 1;
			   newString.Append(selString.GetChar(n));
		   }
		   newString.Append("% ");
		   n = n + 1;
	   }
	   else
	   {
		   newString.Append(selString.GetChar(n));
		   n = n + 1;
	   }
   }
   
   if ( (newString.Length() > 1)  && (newString.Last() == '%') )
      newString = newString.Mid(0, newString.Length()-1);
  
   // Replace the selected text with the argument text.
   ReplaceSelection(newString);

   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnComment() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnUncomment(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is requested to uncomment selected lines/
 * current line
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnUncomment(wxCommandEvent &event)
{
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnUncomment() entered\n");
   #endif
   
   // TGG: 2012-08-14, bug GMT-2979, Script editor comment/indent commands don't work in a standard way
   // set up selection so it selects entire lines so replace works
   int cPos = GetCurrentPos();
   int ePos = GetAnchor();
   int i;
   if (cPos > ePos) 
   {
	   LineEnd();
	   i = GetCurrentPos();
	   SetCurrentPos(ePos);
	   ePos = i;
	   Home();
	   cPos = GetCurrentPos();
   }
   else
   {
	   Home();
	   cPos = GetCurrentPos();
	   SetCurrentPos(ePos);
	   LineEnd();
	   ePos = GetCurrentPos();
	
   }
   SetSelection(cPos, ePos);

   // Retrieve the selected text.
   wxString selString = GetSelectedText();

   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnComment() SelText='%s'\n", selString.c_str());
   #endif

   wxString newString;
   bool lookForComment = true;
   size_t n = 0;
   while ( n < selString.Length() )
   {
	   if ((selString.GetChar(n) == '\n') || (selString.GetChar(n) == '\r'))
	   {
		   newString.Append(selString.GetChar(n));
		   if (((n+1) < selString.Length()) && ((selString.GetChar(n+1) == '\n') || (selString.GetChar(n+1) == '\r')))
		   {
			   n = n + 1;
			   newString.Append(selString.GetChar(n));
		   }
		   lookForComment = true;
		   n = n + 1;
	   }
	   else if (lookForComment && (selString.GetChar(n) == '%'))
	   {
		   lookForComment = false;
		   n = n + 1;
		   if ((n < selString.Length()) && (selString.GetChar(n) == ' '))
			   n = n + 1;
	   }
	   else
	   {
		   if (lookForComment && ((selString.GetChar(n) != ' ') && (selString.GetChar(n) != '\t')))
			   lookForComment = false;
		   newString.Append(selString.GetChar(n));
		   n = n + 1;
	   }
   }
   
   // Replace the selected text with the argument text.
   ReplaceSelection(newString);
   
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("ScriptEditor::OnUncomment() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnHilightLang(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler 
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnHilightLang(wxCommandEvent &event)
{
   InitializePrefs(GmatEditor::globalLanguagePrefs[event.GetId() -
                                                   STC_ID_HILIGHTFIRST].name);
}


//--------------------------------------------------------------------------------
// void OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
//--------------------------------------------------------------------------------
/**
 * Event handler for when the editor should turn on/off displaying end-of-line
 * characters
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
{
   SetViewEOL(!GetViewEOL());
}


//------------------------------------------------------------------------------
// void OnIndentGuide(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should toggle displaying indentation
 * guides
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnIndentGuide(wxCommandEvent &WXUNUSED(event))
{
   SetIndentationGuides(!GetIndentationGuides());
}


//------------------------------------------------------------------------------
// void OnLineNumber(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor handle displaying/not displaying line numbers
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnLineNumber(wxCommandEvent &WXUNUSED(event))
{
   SetMarginWidth(mLineNumberID,
                  GetMarginWidth(mLineNumberID) == 0 ? mLineNumberMargin : 0);
}


//------------------------------------------------------------------------------
// void OnLongLineOn(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should handle long line display
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnLongLineOn(wxCommandEvent &WXUNUSED(event))
{
   SetEdgeMode(GetEdgeMode() == 0? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
}


//------------------------------------------------------------------------------
// void OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should handle display of whitespace characters
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
{
   SetViewWhiteSpace(GetViewWhiteSpace() == 0?
                      wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
}


//------------------------------------------------------------------------------
// void OnFoldToggle(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should perform code folding
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnFoldToggle(wxCommandEvent &WXUNUSED(event))
{
   ToggleFold(GetFoldParent(GetCurrentLine()));
}


//------------------------------------------------------------------------------
// void OnSetOverType(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor responds to overtype mode being set
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnSetOverType(wxCommandEvent &WXUNUSED(event))
{
   SetOvertype(!GetOvertype());
}


//------------------------------------------------------------------------------
// void OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor is set to read-only or editable
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
{
   SetReadOnly(!GetReadOnly());
}


//------------------------------------------------------------------------------
// void OnWrapmodeOn(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should handle wrapping mode changed
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnWrapmodeOn(wxCommandEvent &WXUNUSED(event))
{
   SetWrapMode(GetWrapMode() == 0? wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
}


//------------------------------------------------------------------------------
// void OnUseCharset(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should use different character set
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnUseCharset(wxCommandEvent &event)
{
   int Nr;
   int charset = GetCodePage();
   
   for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++)
      StyleSetCharacterSet(Nr, charset);
   
   SetCodePage(charset);
}


//------------------------------------------------------------------------------
// void OnChangeCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should change case of text
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnChangeCase(wxCommandEvent &event)
{
}


//------------------------------------------------------------------------------
// void OnConvertEOL(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor should convert end of line character mode
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnConvertEOL(wxCommandEvent &event)
{
   int eolMode = GetEOLMode();
   ConvertEOLs(eolMode);
   SetEOLMode(eolMode);
}


//------------------------------------------------------------------------------
// void ScriptEditor::OnMarginClick (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor's margin is clicked
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnMarginClick (wxStyledTextEvent &event)
{
   if (event.GetMargin() == 2)
   {
      int lineClick = LineFromPosition (event.GetPosition());
      int levelClick = GetFoldLevel (lineClick);
      if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
         ToggleFold (lineClick);
   }
}


//------------------------------------------------------------------------------
// void OnTextChange (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor's text is changed
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnTextChange (wxStyledTextEvent &event)
{
   if (mNotifyChange)
   {
      if (IsModified())
      {
         bool setChildDirty = true;
         wxString parentName = mParent->GetName();
         #ifdef DEBUG_TEXT_CHANGE
         MessageInterface::ShowMessage
            ("In ScriptEditor::OnTextChange() text changed, mParent='%s'\n",
             parentName.WX_TO_C_STRING);
         #endif
         if (parentName == "ScriptEventPanel")
            mParent->SetEditorModified(true);
         else if (parentName == "SashScriptEventPanel")
            ((ScriptEventPanel*)(mParent->GetParent()))->SetEditorModified(true);
         else if (parentName == "GmatSavePanel")
            ((GmatSavePanel*)mParent)->SetEditorModified(true);
         else if (parentName == "FunctionSetupPanel")
            ((FunctionSetupPanel*)mParent)->SetEditorModified(true);
         else
            setChildDirty = false;
         
         if (setChildDirty)
            GmatAppData::Instance()->GetMainFrame()->SetActiveChildDirty(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnCharAdded (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
/**
 * Event handler for when the editor has a character added
 * 
 * @param event		command event originated by the control
 *
 */
void ScriptEditor::OnCharAdded (wxStyledTextEvent &event)
{
   char chr = (char)event.GetKey();
   int currentLine = GetCurrentLine();
   // Change this if support for mac files with \r is needed
   if (chr == '\n')
   {
      int lineInd = 0;
      if (currentLine > 0)
         lineInd = GetLineIndentation(currentLine - 1);
      if (lineInd == 0)
         return;
      SetLineIndentation (currentLine, lineInd);
      // if UseTabs is on, use LineEnd(), if off, use GotoPos
      LineEnd();
      //GotoPos(PositionFromLine (currentLine) + lineInd);
   }
}

//------------------------------------------------------------------------------
// void OnStyleNeeded(wxStyledTextEvent &event)
//------------------------------------------------------------------------------
/**
* Event handler for when the editor needs a style for a line
*
* @param event		command event originated by the control
*/
//------------------------------------------------------------------------------
void ScriptEditor::OnStyleNeeded(wxStyledTextEvent &event)
{
   int endStyled = GetEndStyled();
   int startLine = 0;
   int lineCount = GetNumberOfLines();
   int lastPos = GetLastPosition();
   // If script is freshly loaded, style entire document
   if (mInitializeHighlights)
   {
      while (startLine < lineCount - 1)
      {
         startLine = LineFromPosition(GetEndStyled() + 2);

         int startPos = PositionFromLine(startLine);
         int endPos = GetLineEndPosition(startLine);

         // If this is a blank line, set default style and move on
         if (startPos == endPos)
         {
            StartStyling(startPos, 255);
            SetStyling(0, 0);
            endStyled = GetEndStyled();
            continue;
         }

         ApplySyntaxHighlight(startPos, endPos);
         ApplyFoldLevels(startLine, false);
      }
      mInitializeHighlights = false;
   }
   // Style only the required lines
   else
   {
      endStyled = GetEndStyled();
      startLine = LineFromPosition(GetEndStyled());
      int selectedLine = LineFromPosition(GetCurrentPos());
      int startPos = PositionFromLine(startLine);
      int endPos = GetLineEndPosition(startLine);

      if (startPos == endPos)
      {
         StartStyling(startPos, 255);
         SetStyling(0, 0);
         endStyled = GetEndStyled();
         ApplyFoldLevels(startLine, true);
      }
      // If multiple lines need to be styles (i.e. when pasting in text) go
      // through all the necessary lines up to the currently selected line
      else if (selectedLine > startLine)
      {
         endPos = GetLineEndPosition(selectedLine);
         ApplySyntaxHighlight(startPos, endPos);
         for (int i = startLine; i <= selectedLine; ++i)
            ApplyFoldLevels(i, true);
      }
      // Only style one line
      else
      {
         ApplySyntaxHighlight(startPos, endPos);
         ApplyFoldLevels(startLine, true);
      }

      // If a string block was shortened, reset the styles of the text
      // previously within the block
      while (GetStyleAt(PositionFromLine(startLine + 1)) == 5 ||
         GetLine(startLine + 1) == "\r\n")
      {
         startLine = LineFromPosition(GetEndStyled() + 2);

         int startPos = PositionFromLine(startLine);
         int endPos = GetLineEndPosition(startLine);
         if (startPos == endPos)
         {
            StartStyling(startPos, 255);
            SetStyling(0, 0);
            endStyled = GetEndStyled();
            continue;
         }

         ApplySyntaxHighlight(startPos, endPos);
      }

      // If a string block is created, set all text within the block to the
      // string style (can include multiple lines)
      while (mIsStringBlock)
      {
         startLine = LineFromPosition(GetEndStyled() + 2);

         int startPos = PositionFromLine(startLine);
         int endPos = GetLineEndPosition(startLine);
         if (startPos == lastPos)
         {
            mIsStringBlock = false;
            break;
         }
         if (startPos == endPos)
         {
            StartStyling(startPos, 255);
            SetStyling(0, 0);
            endStyled = GetEndStyled();
            continue;
         }

         ApplySyntaxHighlight(startPos, endPos);
         mStringBlockLines++;
      }

      StartStyling(GetEndStyled(), 255);
      SetStyling(2, 0);
   }
   return;
}

//----------------------------
// private methods
//----------------------------

//------------------------------------------------------------------------------
// wxString DeterminePrefs(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Determine Preferences for editor based on file extension
 * 
 * @param filename		filename to find preferences for
 * @return string detailing the detected preference
 *
 */
wxString ScriptEditor::DeterminePrefs(const wxString &filename)
{
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage
     ("ScriptEditor::DeterminePrefs() entered, filename='%s'\n", filename.c_str());
   #endif
   
   GmatEditor::LanguageInfoType const* curInfo;
   
   // determine language from filepatterns
   int index;
   for (index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = &GmatEditor::globalLanguagePrefs [index];
      wxString filepattern = curInfo->filepattern;
      filepattern.Lower();
      while(!filepattern.empty())
      {
         wxString cur = filepattern.BeforeFirst(';');
         if ((cur == filename) ||
            (cur ==(filename.BeforeLast('.') + _T(".*"))) ||
            (cur ==(_T("*.") + filename.AfterLast('.'))))
         {
            #ifdef DEBUG_EDITOR_PREF
            MessageInterface::ShowMessage
              ("ScriptEditor::DeterminePrefs() exiting with '%s'\n", curInfo->name);
            #endif
            
            return curInfo->name;
         }
         filepattern = filepattern.AfterFirst(';');
      }
   }
   
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage("ScriptEditor::DeterminePrefs() exiting with empty string\n");
   #endif
   
   return wxEmptyString;
}


//------------------------------------------------------------------------------
// bool InitializePrefs(const wxString &name)
//------------------------------------------------------------------------------
/**
 * Initialize preferences based on preference name
 * 
 * @param name		preference name to load/init
 * @return true if prefernce name is found in globalLanguagePrefs
 *
 */
bool ScriptEditor::InitializePrefs(const wxString &name)
{
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage
     ("ScriptEditor::InitializePrefs() entered, name='%s'\n", name.c_str());
   #endif
   
   // initialize styles
   StyleClearAll();
   GmatEditor::LanguageInfoType const* curInfo = NULL;
   
   // determine language
   bool found = false;
   for (int index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = &GmatEditor::globalLanguagePrefs[index];
      if (curInfo->name == name)
      {
         found = true;
         break;
      }
   }
   
   if (!found)
   {
      #ifdef DEBUG_EDITOR_PREF
      MessageInterface::ShowMessage("ScriptEditor::InitializePrefs() returning false\n");
      #endif
      return false;
   }
   
   // set lexer and language
   SetLexer(curInfo->lexer);
   mLanguage = curInfo;
   
   // set margin for line numbers
   SetMarginType(mLineNumberID, wxSTC_MARGIN_NUMBER);
   StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(_T("DARK GREY")));
   StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
   
   //SetMarginWidth(mLineNumberID, 0); // start out not visible
   SetMarginWidth(mLineNumberID, mLineNumberMargin); // show line numbers
   
   // default fonts for all styles!
   int index;
   for (index = 0; index < wxSTC_STYLE_LASTPREDEFINED; index++)
   {
      wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
      StyleSetFont(index, font);
   }
   
   // set common styles
   StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(_T("DARK GREY")));
   StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(_T("DARK GREY")));
   
   // initialize settings
   if (GmatEditor::globalCommonPrefs.syntaxEnable)
   {
      int keywordIndex = 0;
      for (index = 0; index < STYLE_TYPES_COUNT; index++)
      {
         int styleType = curInfo->styles[index].type;
         if (styleType == -1)
            continue;
         
         const GmatEditor::StyleInfoType &curType =
            GmatEditor::globalStylePrefs[styleType];
         wxFont font(curType.fontsize, wxMODERN, wxNORMAL, wxNORMAL, false,
                     curType.fontname);
         StyleSetFont(index, font);
         
         if (curType.foreground)
         {
            // Check if a custom RGB value was used
            std::string customName = "Custom";
            if (curType.foreground != customName)
               StyleSetForeground(index, wxColour(curType.foreground));
            else
               StyleSetForeground(index, wxColour(curType.foregroundRGB));
         }
         
         if (curType.background)
            StyleSetBackground(index, wxColour(curType.background));
         
         StyleSetBold(index,(curType.fontstyle & GMAT_STC_STYLE_BOLD) > 0);
         StyleSetItalic(index,(curType.fontstyle & GMAT_STC_STYLE_ITALIC) > 0);
         StyleSetUnderline(index,(curType.fontstyle & GMAT_STC_STYLE_UNDERL) > 0);
         StyleSetVisible(index,(curType.fontstyle & GMAT_STC_STYLE_HIDDEN) == 0);
         StyleSetCase(index, curType.lettercase);
         
         const wxChar *svalue = curInfo->styles[index].words;
         if (svalue)
         {
            #ifdef DEBUG_EDITOR_PREF
            MessageInterface::ShowMessage
               ("==> styleType = %d, keywordIndex = %d, styles[%d].words = '%s'\n",
                styleType, keywordIndex, index, svalue);
            #endif

            SetKeyWords(keywordIndex, svalue);
            //SetKeyWords(index, svalue);
            keywordIndex += 1;
         }
      }
   }
   
   // set margin as unused
   SetMarginType(mDividerID, wxSTC_MARGIN_SYMBOL);
   SetMarginWidth(mDividerID, 0);
   SetMarginSensitive(mDividerID, false);
   
   // folding
   SetMarginType(mFoldingID, wxSTC_MARGIN_SYMBOL);
   SetMarginMask(mFoldingID, wxSTC_MASK_FOLDERS);
   StyleSetBackground(mFoldingID, *wxWHITE);
   SetMarginWidth(mFoldingID, 0);
   SetMarginSensitive(mFoldingID, false);
   if (GmatEditor::globalCommonPrefs.foldEnable)
   {
      SetMarginWidth(mFoldingID, curInfo->folds != 0? mFoldingMargin: 0);
      SetMarginSensitive(mFoldingID, curInfo->folds != 0);
      SetProperty(_T("fold"), curInfo->folds != 0? _T("1"): _T("0"));
      SetProperty(_T("fold.comment"),
                  (curInfo->folds & GMAT_STC_FOLD_COMMENT) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.compact"),
                  (curInfo->folds & GMAT_STC_FOLD_COMPACT) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.preprocessor"),
                  (curInfo->folds & GMAT_STC_FOLD_PREPROC) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.html"),
                  (curInfo->folds & GMAT_STC_FOLD_HTML) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.html.preprocessor"),
                  (curInfo->folds & GMAT_STC_FOLD_HTMLPREP) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.comment.python"),
                  (curInfo->folds & GMAT_STC_FOLD_COMMENTPY) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.quotes.python"),
                  (curInfo->folds & GMAT_STC_FOLD_QUOTESPY) > 0? _T("1"): _T("0"));
   }
   SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
   
   // set spaces and indention
   SetTabWidth(3);

   // GMT-3336  Preserve tabs
   //SetUseTabs(false);
   SetUseTabs(true);

   SetTabIndents(true);
   SetBackSpaceUnIndents(true);
   SetIndent(GmatEditor::globalCommonPrefs.indentEnable ? 3 : 0);
   
   // others
   SetViewEOL(GmatEditor::globalCommonPrefs.displayEOLEnable);
   SetIndentationGuides(GmatEditor::globalCommonPrefs.indentGuideEnable);
   SetEdgeColumn(80);
   SetEdgeMode(GmatEditor::globalCommonPrefs.longLineOnEnable? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
   SetViewWhiteSpace(GmatEditor::globalCommonPrefs.whiteSpaceEnable?
                     wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
   SetOvertype(GmatEditor::globalCommonPrefs.overTypeInitial);
   SetReadOnly(GmatEditor::globalCommonPrefs.readOnlyInitial);
   SetWrapMode(GmatEditor::globalCommonPrefs.wrapModeInitial?
               wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
   
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage("ScriptEditor::InitializePrefs() returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// void ApplySyntaxHighlight(int fromPos, int toPos)
//------------------------------------------------------------------------------
/**
* Apply syntax highilighting to keywords and special characters
*
* @param fromPos  The starting position of the current line
* @param toPos    The position of the end of the current line
*/
//------------------------------------------------------------------------------
void ScriptEditor::ApplySyntaxHighlight(int fromPos, int toPos)
{
   int previousPos = fromPos;
   int currentPos = fromPos;
   std::string currentCharSet;

   // Clear the current style and set it to the default style
   StartStyling(fromPos, 255);
   SetStyling(toPos - fromPos, 0);
   int lastPos = GetLastPosition();
   bool isCurrCharNum = false;
   bool isCharSetNumber = false;
   bool endOfString = false;

   while (currentPos < toPos)
   {
      for (int i = currentPos; i <= toPos; ++i)
      {
         char currentChar = GetCharAt(i);
         if (currentChar < 0)
         {
            currentCharSet += currentChar;
            currentPos = i + 1;
            previousPos = i;
            break;
         }
         std::stringstream charToString;
         std::string currentString;
         charToString << currentChar;
         charToString >> currentString;
         isCurrCharNum = GmatStringUtil::IsNumber(currentString);
         char prevChar;
         if (i > fromPos)
            prevChar = GetCharAt(i - 1);
         if (mIsStringBlock)
         {
            currentCharSet += currentChar;
            currentPos = i + 1;
            if (currentChar == '\'' || currentPos == lastPos)
            {
               endOfString = true;
               break;
            }
         }
         else if (currentChar == '%')
         {
            currentPos = GetLineEndPosition(LineFromPosition(i));
            previousPos = i;
            currentCharSet = currentChar;
            break;
         }
         else if (currentChar == '=')
         {
            currentCharSet = currentChar;
            currentPos = i + 1;
            break;
         }
         else if (currentChar == '\'' && currentCharSet.size() == 0 &&
            prevChar != ')' && prevChar != '}'  && prevChar != ']')
         {
            currentCharSet += currentChar;
            currentPos = i + 1;
            mIsStringBlock = true;
            endOfString = false;
         }
         else if (isCharSetNumber && (isCurrCharNum ||
            currentChar == 'e' || currentChar == '.' ||
            (currentChar == '+' && currentCharSet.back() == 'e') ||
            (currentChar == '-' && currentCharSet.back() == 'e')))
         {
            currentCharSet += currentChar;
            currentPos = i + 1;
         }
         else if (isCurrCharNum && (currentCharSet[0] == '.' ||
            currentCharSet.size() == 0))
         {
            currentCharSet += currentChar;
            currentPos = i + 1;
            isCharSetNumber = true;
         }
         else if (!isalpha(currentChar) && currentChar != '.' &&
            currentChar != '_' && !isCurrCharNum)
         {
            currentPos = i + 1;
            break;
         }
         else
         {
            if (isCharSetNumber)
            {
               currentPos = i;
               break;
            }
            currentCharSet += currentChar;
         }
      }

      // Check for a keyword to highlight
      bool styleFound = false;
      int currentStyle = 0;
      if (currentCharSet == "%")
      {
         styleFound = true;
         currentStyle = 1;
      }
      if (!styleFound && mIsStringBlock)
      {
         styleFound = true;
         currentStyle = 5;
         if (endOfString)
            mIsStringBlock = false;
      }
      if (!styleFound && isCharSetNumber)
      {
         styleFound = true;
         currentStyle = 3;
         isCharSetNumber = false;
      }
      if (!styleFound)
      {
         for (int i = 0; i < mCmdCreatablesArray.size(); ++i)
         {
            if (currentCharSet == mCmdCreatablesArray[i])
            {
               currentStyle = 2;
               styleFound = true;
               break;
            }
         }
      }
      if (!styleFound)
      {
         for (int i = 0; i < mObjCreatablesArray.size(); ++i)
         {
            if (currentCharSet == mObjCreatablesArray[i])
            {
               currentStyle = 4;
               styleFound = true;
               break;
            }
         }
      }

      StartStyling(previousPos, 255);
      if (currentStyle == 1)
         SetStyling(currentPos - previousPos + 1, currentStyle);
      else
         SetStyling(currentCharSet.length(), currentStyle);
      previousPos = currentPos;
      currentCharSet = "";
   }

   if (currentPos == toPos)
   {
      StartStyling(currentPos, 255);
      SetStyling(0, 0);
   }

   if (mIsStringBlock)
   {
      if (LineFromPosition(currentPos) == GetNumberOfLines() - 1)
         mIsStringBlock = false;
   }
}

//------------------------------------------------------------------------------
// void ApplyFoldLevels(int fromLine, bool checkForEnds)
//------------------------------------------------------------------------------
/**
* Set folding levels for the script
*
* @param fromLine Line to start fold checking from
* @param checkForEnds Boolean of whether or not to check the current line for a
*        new end statement for a fold
*/
//------------------------------------------------------------------------------
void ScriptEditor::ApplyFoldLevels(int fromLine, bool checkForEnds)
{
   int currLine = fromLine;
   IntegerArray startFoldRemoval;
   IntegerArray endFoldRemoval;
   bool checkCurrentFoldSegments = false;

   // Check if new lines were added or removed and adjust the fold locations
   // accordingly
   int locationToSkip = -1;
   if (mPrevLineCount < GetNumberOfLines())
   {
      checkCurrentFoldSegments = true;
      int numLinesAdded = GetNumberOfLines() - mPrevLineCount;
      mPrevLineCount = GetNumberOfLines();
      for (int foldIdx = 0; foldIdx < mFoldLocations.size(); ++foldIdx)
      {
         if (currLine < mFoldLocations[foldIdx][0])
         {
            mFoldLocations[foldIdx][0] += numLinesAdded;
            locationToSkip = foldIdx;
         }
         if (currLine < mFoldLocations[foldIdx][1])
         {
            mFoldLocations[foldIdx][1] += numLinesAdded;
            locationToSkip = foldIdx;
         }
      }
   }
   else if (mPrevLineCount > GetNumberOfLines())
   {
      checkCurrentFoldSegments = true;
      int numLinesRemoved = mPrevLineCount - GetNumberOfLines();
      mPrevLineCount = GetNumberOfLines();
      for (int foldIdx = 0; foldIdx < mFoldLocations.size(); ++foldIdx)
      {
         if (currLine < mFoldLocations[foldIdx][0])
         {
            if (currLine + numLinesRemoved >= mFoldLocations[foldIdx][0])
               mFoldLocations[foldIdx][0] = currLine;
            else
               mFoldLocations[foldIdx][0] -= numLinesRemoved;
            locationToSkip = foldIdx;
         }
         if (currLine < mFoldLocations[foldIdx][1])
         {
            if (currLine + numLinesRemoved >= mFoldLocations[foldIdx][1])
               mFoldLocations[foldIdx][1] = currLine;
            else
               mFoldLocations[foldIdx][1] -= numLinesRemoved;
            locationToSkip = foldIdx;
         }
      }
   }

   std::string currCommand = "";
   std::string currCommand2 = "";
   std::string currText = "";

   // Check if the current fold locations have lost the start or end of the
   // section and, if so, remove the fold levels
   int currSelectedLine = LineFromPosition(GetCurrentPos());
   for (int i = 0; i < mFoldLocations.size(); ++i)
   {
      if (mFoldLocations[i][0] == currSelectedLine ||
         (checkCurrentFoldSegments && mFoldLocations[i][0] != -1))
      {
         currCommand = "";
         currText = GetLine(mFoldLocations[i][0]);
         for (int j = 0; j < currText.size(); ++j)
         {
            char currChar = currText[j];
            if (currCommand == "")
            {
               if (currChar == ' ' || currChar == '\t')
                  continue;
               else
                  currCommand += currChar;
            }
            else
            {
               if (currChar == ' ' || currChar == '\t' || currChar == '\n' ||
                  currChar == '\r' || currChar == ';')
               {
                  break;
               }
               else
                  currCommand += currChar;
            }
         }

         if (currCommand == "BeginScript")
            currCommand = "Script";

         if (currCommand != mFoldTypes[i] && mFoldLocations[i][1] != -1)
         {
            startFoldRemoval.push_back(mFoldLocations[i][0]);
            endFoldRemoval.push_back(mFoldLocations[i][1]);
            mFoldLocations[i][0] = -1;
         }
         else if (currCommand != mFoldTypes[i] &&
            mFoldLocations[i][1] == -1)
         {
            mFoldLocations.erase(mFoldLocations.begin() + i);
            mFoldTypes.erase(mFoldTypes.begin() + i);
            --i;
            continue;
         }
      }

      if (mFoldLocations[i][1] == currSelectedLine ||
         (checkCurrentFoldSegments && mFoldLocations[i][1] != -1))
      {
         currCommand2 = "";
         currText = GetLine(mFoldLocations[i][1]);
         for (int j = 0; j < currText.size(); ++j)
         {
            char currChar = currText[j];
            if (currCommand2 == "")
            {
               if (currChar == ' ' || currChar == '\t')
                  continue;
               else
                  currCommand2 += currChar;
            }
            else
            {
               if (currChar == ' ' || currChar == '\t' || currChar == '\n' ||
                  currChar == '\r' || currChar == ';')
               {
                  break;
               }
               else
                  currCommand2 += currChar;
            }
         }

         if (currCommand2 != "End" + mFoldTypes[i] &&
            mFoldLocations[i][0] != -1)
         {
            startFoldRemoval.push_back(mFoldLocations[i][0]);
            endFoldRemoval.push_back(mFoldLocations[i][1]);
            mFoldLocations[i][1] = -1;
         }
         else if (currCommand2 != "End" + mFoldTypes[i] &&
            mFoldLocations[i][0] == -1)
         {
            if (endFoldRemoval.size() > 0)
            {
               if (endFoldRemoval.back() == mFoldLocations[i][1])
               {
                  startFoldRemoval.pop_back();
                  endFoldRemoval.pop_back();
               }
            }
            mFoldLocations.erase(mFoldLocations.begin() + i);
            mFoldTypes.erase(mFoldTypes.begin() + i);
            --i;
         }
      }
   }

   // Build a string containing the command of the current line to be used
   // to check for a new start or end of a fold section
   currCommand = "";
   currText = GetLine(fromLine);
   for (int i = 0; i < currText.size(); ++i)
   {
      char currChar = currText[i];
      if (currCommand == "")
      {
         if (currChar == ' ' || currChar == '\t')
            continue;
         else
            currCommand += currChar;
      }
      else
      {
         if (currChar == ' ' || currChar == '\t' || currChar == '\n' ||
            currChar == '\r' || currChar == ';')
         {
            break;
         }
         else
            currCommand += currChar;
      }
   }

   // If there is no longer the start or end of a fold where it used to be,
   // reduce the fold level of the lines in this section by one
   if (startFoldRemoval.size() > 0)
   {
      for (int removeIdx = 0; removeIdx < startFoldRemoval.size(); ++removeIdx)
      {
         for (int lineIdx = startFoldRemoval[removeIdx] + 1;
            lineIdx <= endFoldRemoval[removeIdx]; ++lineIdx)
         {
            int testLevel = GetFoldLevel(lineIdx);
            SetFoldLevel(lineIdx, GetFoldLevel(lineIdx) - 1);
         }
      }
   }

   // Check for a new start or end of a fold section
   int startLine = currLine;
   int endLine = GetNumberOfLines();
   int endFoldLine = 0;
   bool endFound = false;
   int foldDistTest = INT_MAX;
   int minDistIdx = -1;
   int testFoldLevelMultiple = -1;

   if (currCommand == "If" || currCommand == "For" ||
      currCommand == "While" || currCommand == "Target" ||
      currCommand == "Optimize")
   {
      int currFoldLevel = GetFoldLevel(startLine);
      SetFoldLevel(startLine, currFoldLevel | wxSTC_FOLDLEVELHEADERFLAG);
      int newFoldLevel = currFoldLevel + 1;
      int testFoldLevel = GetFoldLevel(startLine + 1);

      // Search for a valid ending statement that needs a header statement,
      // if one isn't found, add this header as a new possible fold location
      // and do not modify fold levels
      for (int i = 0; i < mFoldLocations.size(); ++i)
      {
         if (mFoldLocations[i][0] == -1)
         {
            // Fold levels will sometimes be automatically set to multiples
            // of the base fold level 1024 (for example, 1024 can be
            // represented as 9216, so then 1026 would be 9218), so check that
            // the possible end statement divided by the base value has the
            // same remainder as the header
            if (mFoldLocations[i][1] - startLine > 0 &&
               mFoldLocations[i][1] - startLine < foldDistTest &&
               mFoldTypes[i] == currCommand &&
               testFoldLevel % 1024 ==
               GetFoldLevel(mFoldLocations[i][1]) % 1024)
            {
               foldDistTest = startLine - mFoldLocations[i][0];
               minDistIdx = i;
               endFound = true;
            }
         }
         else if (mFoldLocations[i][0] == startLine)
            return;
      }

      if (!endFound)
      {
         mFoldLocations.resize(mFoldLocations.size() + 1);
         mFoldLocations[mFoldLocations.size() - 1].push_back(startLine);
         mFoldLocations[mFoldLocations.size() - 1].push_back(-1);
         mFoldTypes.push_back(currCommand);
      }
      else
      {
         mFoldLocations[minDistIdx][0] = startLine;
         endFoldLine = mFoldLocations[minDistIdx][1];
         int compareFold = GetFoldLevel(endFoldLine);

         for (int i = startLine + 1; i <= endFoldLine; ++i)
         {
            SetFoldLevel(i, GetFoldLevel(i) + 1);
         }
      }
   }
   else if (currCommand == "BeginScript")
   {
      int currFoldLevel = GetFoldLevel(startLine);
      SetFoldLevel(startLine, currFoldLevel | wxSTC_FOLDLEVELHEADERFLAG);
      int newFoldLevel = currFoldLevel + 1;
      int testFoldLevel = GetFoldLevel(startLine + 1);

      // Search for a valid ending statement that needs a header statement,
      // if one isn't found, add this header as a new possible fold location
      // and do not modify fold levels
      for (int i = 0; i < mFoldLocations.size(); ++i)
      {
         if (mFoldLocations[i][0] == -1)
         {
            // Fold levels will sometimes be automatically set to multiples
            // of the base fold level 1024 (for example, 1024 can be
            // represented as 9216, so then 1026 would be 9218), so check that
            // the possible end statement divided by the base value has the
            // same remainder as the header
            if (mFoldLocations[i][1] - startLine > 0 &&
               mFoldLocations[i][1] - startLine < foldDistTest &&
               mFoldTypes[i] == "Script" &&
               testFoldLevel % 1024 ==
               GetFoldLevel(mFoldLocations[i][1]) % 1024)
            {
               foldDistTest = startLine - mFoldLocations[i][0];
               minDistIdx = i;
               endFound = true;
            }
         }
         else if (mFoldLocations[i][0] == startLine)
            return;
      }

      if (!endFound)
      {
         mFoldLocations.resize(mFoldLocations.size() + 1);
         mFoldLocations[mFoldLocations.size() - 1].push_back(startLine);
         mFoldLocations[mFoldLocations.size() - 1].push_back(-1);
         mFoldTypes.push_back("Script");
      }
      else
      {
         mFoldLocations[minDistIdx][0] = startLine;
         endFoldLine = mFoldLocations[minDistIdx][1];

         for (int i = startLine + 1; i <= endFoldLine; ++i)
         {
            SetFoldLevel(i, GetFoldLevel(i) + 1);
         }
      }
   }
   else if (currCommand == "EndIf" || currCommand == "EndFor" ||
      currCommand == "EndWhile" || currCommand == "EndTarget" ||
      currCommand == "EndOptimize" || currCommand == "EndScript")
   {
      int currFoldLevel = GetFoldLevel(startLine);
      int newFoldLevel = currFoldLevel + 1;
      int testFoldLevel = GetFoldLevel(startLine);

      std::string commandName = currCommand;
      commandName.erase(0, 3);

      // Search for a valid header statement that needs an ending statement,
      // if one isn't found, add this ending as a new possible fold location
      // and do not modify fold levels
      for (int i = 0; i < mFoldLocations.size(); ++i)
      {
         if (mFoldLocations[i][1] == -1)
         {
            // Fold levels will sometimes be automatically set to multiples
            // of the base fold level 1024 (for example, 1024 can be
            // represented as 9216, so then 1026 would be 9218), so check that
            // the possible header statement divided by the base value has the
            // same remainder as the end statement
            if (startLine - mFoldLocations[i][0] > 0 &&
               startLine - mFoldLocations[i][0] < foldDistTest &&
               mFoldTypes[i] == commandName &&
               testFoldLevel % 1024 ==
               GetFoldLevel(mFoldLocations[i][0] + 1) % 1024)
            {
               foldDistTest = startLine - mFoldLocations[i][0];
               minDistIdx = i;
               endFound = true;
            }
         }
         else if (mFoldLocations[i][1] == startLine)
            return;
      }

      if (!endFound)
      {
         mFoldLocations.resize(mFoldLocations.size() + 1);
         mFoldLocations[mFoldLocations.size() - 1].push_back(-1);
         mFoldLocations[mFoldLocations.size() - 1].push_back(startLine);
         mFoldTypes.push_back(commandName);
      }
      else
      {
         mFoldLocations[minDistIdx][1] = startLine;
         endFoldLine = mFoldLocations[minDistIdx][0];

         for (int i = startLine; i > endFoldLine; --i)
         {
            SetFoldLevel(i, GetFoldLevel(i) + 1);
         }
      }
   }
}

//------------------------------------------------------------------------------
// wxString GetLine(int lineNumber)
//------------------------------------------------------------------------------
/**
 * Return the line at the specified line number
 * 
 * @param lineNumber		0-based line number
 * @return string representation of the line
 *
 */
wxString ScriptEditor::GetLine(int lineNumber)
{
   return wxStyledTextCtrl::GetLine(lineNumber);
}


//------------------------------------------------------------------------------
// wxString GetText()
//------------------------------------------------------------------------------
/**
 * Return the text in the editor
 * 
 * @return string representation of the editor contents
 *
 */
wxString ScriptEditor::GetText()
{
   return wxStyledTextCtrl::GetText();
}


//------------------------------------------------------------------------------
// bool LoadFile()
//------------------------------------------------------------------------------
/**
 * Load a file (from internal filename) into the editor.  If filename is not defined
 * show a dialog requesting filename from user
 * 
 * @return true if file loaded
 *
 */
bool ScriptEditor::LoadFile()
{
#if wxUSE_FILEDLG
   // get filname
   if (!mFileName) {
      wxFileDialog dlg(this, _T("Open file"), wxEmptyString, wxEmptyString,
                        _T("Any file(*)|*"), gmatFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
      if (dlg.ShowModal() != wxID_OK) return false;
      mFileName = dlg.GetPath();
   }
   
   // load file
   return LoadFile(mFileName);
#else
   return false;
#endif // wxUSE_FILEDLG
}


//------------------------------------------------------------------------------
// bool LoadFile(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Load the specified file into the editor and initialize editor
 * 
 * @param filename	file to be loaded
 * @return true if file loaded
 *
 */
bool ScriptEditor::LoadFile(const wxString &filename)
{
   #ifdef DEBUG_EDITOR_FILE
   MessageInterface::ShowMessage
     ("ScriptEditor::LoadFile() entered, this=<%p>, filename='%s'\n", this, filename.c_str());
   #endif
   
   // load file in edit and clear undo
   if (!filename.empty())
      mFileName = filename;
   
   ClearAll();
   mInitializeHighlights = true;
   mFoldLocations.clear();
   mFoldTypes.clear();
   mPrevLineCount = GetNumberOfLines();

   wxStyledTextCtrl::LoadFile(mFileName);
   EmptyUndoBuffer();
   
   // determine lexer language
   wxFileName fname(mFileName);
   InitializePrefs(DeterminePrefs(fname.GetFullName()));
   
   #ifdef DEBUG_EDITOR_FILE
   MessageInterface::ShowMessage("ScriptEditor::LoadFile() exiting with true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool SaveFile()
//------------------------------------------------------------------------------
/**
 * Save a file from the editor using internal filename.  If filename is not defined
 * show a dialog requesting filename from user
 * 
 * @return true if file saved
 *
 */
bool ScriptEditor::SaveFile()
{
#if wxUSE_FILEDLG
   // return if no change
   if (!IsModified()) return true;

   // get filname
   if (!mFileName)
   {
      wxFileDialog dlg(this, _T("Save file"), wxEmptyString, wxEmptyString,
                        _T("Any file(*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
      if (dlg.ShowModal() != wxID_OK) return false;
      mFileName = dlg.GetPath();
   }
   
   // save file
   return SaveFile(mFileName);
#else
   return false;
#endif // wxUSE_FILEDLG
}


//------------------------------------------------------------------------------
// bool SaveFile(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Save a file from the editor using specified filename.  
 * 
 * @param filename	file to be saved
 * @return true if file saved
 *
 */
bool ScriptEditor::SaveFile(const wxString &filename)
{
   ConvertEOLs(GetEOLMode());
   return wxStyledTextCtrl::SaveFile(filename);
}


//------------------------------------------------------------------------------
// bool IsModified()
//------------------------------------------------------------------------------
/**
 * Return whether editor contents have been modified
 * 
 * @return true if editor modified
 *
 */
bool ScriptEditor::IsModified()
{
   bool state = (GetModify() && !GetReadOnly());
   
   #ifdef DEBUG_EDITOR_MODIFIED
   MessageInterface::ShowMessage("ScriptEditor::IsModified() returning %d\n", state);
   #endif
   
   return state;
}

