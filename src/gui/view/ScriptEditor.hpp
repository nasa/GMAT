//$Id$
//------------------------------------------------------------------------------
//                                   ScriptEditor
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
 * Declares ScriptEditor class.
 */
//------------------------------------------------------------------------------
#ifndef ScriptEditor_hpp
#define ScriptEditor_hpp

#include "gmatdefs.hpp"
#include "EditorPreferences.hpp" // for GmatEditor::
#include "wx/stc/stc.h"          // for wxStyledTextCtrl

class GmatPanel;
class FindReplaceDialog;

class ScriptEditor: public wxStyledTextCtrl
{
   friend class EditorProperties;
   friend class EditorPrint;
   
public:
   // default constructor
   ScriptEditor(wxWindow *parent, bool notifyChange = false,
          wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          long style = wxSUNKEN_BORDER|wxVSCROLL);
   
   // destructor
   ~ScriptEditor();
   
   // find/replace dialog text
   void SetFindText(const wxString &text);
   void SetReplaceText(const wxString &text);
   
   // event handlers
   // common event handlers
   void OnSize(wxSizeEvent &event);
   // font event handlers
   void OnFont(wxCommandEvent &event);
   // edit event handlers
   void OnRedo(wxCommandEvent &event);
   void OnUndo(wxCommandEvent &event);
   void OnClear(wxCommandEvent &event);
   void OnCut(wxCommandEvent &event);
   void OnCopy(wxCommandEvent &event);
   void OnPaste(wxCommandEvent &event);
   // find, replace, goto, select event handlers
   void OnFind(wxCommandEvent &event);
   void OnFindNext(wxCommandEvent &event);
   void OnFindPrev(wxCommandEvent &event);
   void OnReplaceNext(wxCommandEvent &event);
   void OnReplaceAll(wxCommandEvent &event);
   void OnBraceMatch(wxCommandEvent &event);
   void OnGoToLine(wxCommandEvent &event);
   void OnIndentMore(wxCommandEvent &event);
   void OnIndentLess(wxCommandEvent &event);
   void OnSelectAll(wxCommandEvent &event);
   void OnSelectLine(wxCommandEvent &event);
   // comment, uncomment event handlers
   void OnComment(wxCommandEvent &event);
   void OnUncomment(wxCommandEvent &event);
   // view event handlers
   void OnHilightLang(wxCommandEvent &event);
   void OnDisplayEOL(wxCommandEvent &event);
   void OnIndentGuide(wxCommandEvent &event);
   void OnLineNumber(wxCommandEvent &event);
   void OnLongLineOn(wxCommandEvent &event);
   void OnWhiteSpace(wxCommandEvent &event);
   void OnFoldToggle(wxCommandEvent &event);
   void OnSetOverType(wxCommandEvent &event);
   void OnSetReadOnly(wxCommandEvent &event);
   void OnWrapmodeOn(wxCommandEvent &event);
   void OnUseCharset(wxCommandEvent &event);
   // extra event handlers
   void OnChangeCase(wxCommandEvent &event);
   void OnConvertEOL(wxCommandEvent &event);
   // stc event handlers
   void OnMarginClick(wxStyledTextEvent &event);
   void OnTextChange(wxStyledTextEvent &event);
   void OnCharAdded(wxStyledTextEvent &event);
   void OnStyleNeeded(wxStyledTextEvent &event);
   
   // language/lexer
   bool UserSettings(const wxString &filename);
   GmatEditor::LanguageInfoType const* GetLanguageInfo() {return mLanguage;};
   
   // text
   wxString GetLine(int lineNumber);
   wxString GetText();
   
   // file operations
   bool LoadFile();
   bool LoadFile(const wxString &filename);
   bool SaveFile();
   bool SaveFile(const wxString &filename);
   bool IsModified();
   wxString GetFilename() {return mFileName;};
   void SetFilename(const wxString &filename) {mFileName = filename;};
   
private:

   GmatPanel *mParent;
   bool      mNotifyChange;
   
   // language/lexer
   wxString DeterminePrefs(const wxString &filename);
   bool InitializePrefs(const wxString &filename);
   void ApplySyntaxHighlight(int fromPos, int toPos);
   void ApplyFoldLevels(int fromLine, bool checkForEnds);
   
   // find/replace text
   FindReplaceDialog *mFindReplaceDialog;
   wxString mFindText;
   wxString mReplaceText;
   int mLastSelectPos;
   int mLastFindPos;
   
   // file
   wxString mFileName;
   wxString mGmatCommandTypes;
   wxString mGmatObjectTypes;
   
   // lanugage properties
   GmatEditor::LanguageInfoType const* mLanguage;
   
   // margin variables
   int  mLineNumberID;
   int  mLineNumberMargin;
   int  mFoldingID;
   int  mFoldingMargin;
   int  mDividerID;

   // syntax highlighting
   StringArray mObjCreatablesArray;
   StringArray mCmdCreatablesArray;
   wxString mGmatKeywords;
   bool mInitializeHighlights;
   bool mIsStringBlock;
   int  mStringBlockLines;
   
   // fold text
   std::vector<std::vector<int>> mFoldLocations;
   StringArray mFoldTypes;
   int mPrevLineCount;

   // goto line
   long mPrevLineNumber;
   
   DECLARE_EVENT_TABLE()
};

// IDs for STC editor
enum
{
   // menu IDs
   STC_ID_PROPERTIES = 18000,
   STC_ID_INDENTMORE,
   STC_ID_INDENTLESS,
   STC_ID_FINDNEXT,
   STC_ID_REPLACE,
   STC_ID_REPLACENEXT,
   STC_ID_REPLACEALL,
   STC_ID_BRACEMATCH,
   STC_ID_GOTO,
   STC_ID_PAGEACTIVE,
   STC_ID_DISPLAYEOL,
   STC_ID_INDENTGUIDE,
   STC_ID_LINENUMBER,
   STC_ID_LONGLINEON,
   STC_ID_WHITESPACE,
   STC_ID_FOLDTOGGLE,
   STC_ID_OVERTYPE,
   STC_ID_READONLY,
   STC_ID_WRAPMODEON,
   STC_ID_CHANGECASE,
   STC_ID_CHANGELOWER,
   STC_ID_CHANGEUPPER,
   STC_ID_HILIGHTLANG,
   STC_ID_HILIGHTFIRST,
   STC_ID_HILIGHTLAST = STC_ID_HILIGHTFIRST + 99,
   STC_ID_CONVERTEOL,
   STC_ID_CONVERTCR,
   STC_ID_CONVERTCRLF,
   STC_ID_CONVERTLF,
   STC_ID_USECHARSET,
   STC_ID_CHARSETANSI,
   STC_ID_CHARSETMAC,
   STC_ID_PAGEPREV,
   STC_ID_PAGENEXT,
   STC_ID_SELECTLINE,
   
   // other IDs
   STC_ID_STATUSBAR,
   STC_ID_TITLEBAR,
   STC_ID_ABOUTTIMER,
   STC_ID_UPDATETIMER,
   
   // dialog find IDs
   STC_ID_DLG_FIND_TEXT,
   
   // preferences IDs
   STC_ID_PREFS_LANGUAGE,
   STC_ID_PREFS_STYLETYPE,
   STC_ID_PREFS_KEYWORDS,
   
   // comment IDs
   STC_ID_COMMENT,
   STC_ID_UNCOMMENT,
};

#endif
