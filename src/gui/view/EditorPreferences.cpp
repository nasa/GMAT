//$Id$
//------------------------------------------------------------------------------
//                              EditorPreferences
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
// Created: 2009/01/15
//
/**
 * Implements EditorPreferences.
 */
//------------------------------------------------------------------------------

#include "EditorPreferences.hpp"

//---------------------------------
// static data
//---------------------------------

//----------------------------------------------------------------------------
//! language types
const GmatEditor::CommonInfoType GmatEditor::globalCommonPrefs =
{
    // editor functionality prefs
    true,  // syntaxEnable
    true,  // foldEnable
    true,  // indentEnable
    // display defaults prefs
    false, // overTypeInitial
    false, // readOnlyInitial
    false, // wrapModeInitial
    false, // displayEOLEnable
    false, // IndentGuideEnable
    true,  // lineNumberEnable
    false, // longLineOnEnable
    false, // whiteSpaceEnable
};

//----------------------------------------------------------------------------
// keywordlists
// GMAT
// @note
// These are GMAT default commands and types. Actual commands and types are
// retrieved via the GuiInterpreter::GetStringOfAllFactoryItemsExcept()
// and set to GMAT language preference in Editor constructor.
//
wxChar* GmatCommands =
   _T("GMAT Create Global Maneuver Propagate Report Save Stop Toggle ")
   _T("Achieve Vary Target Optimize Minimize PenDown PenUp ")
   _T("For EndFor If Else EndIf While EndWhile Target EndTarget ")
   _T("BeginFiniteBurn EndFiniteBurn BeginScript EndScript " )
   _T("Spacecraft ForceModel Propagator FuelTank Thruster SolarSystem ")
   _T("CoordinateSystem Variable Array String ReportFile XYPlot OpenGLPlot " )
   _T("ImpulsiveBurn FiniteBurn DifferentialCorrector Optimizer MatlabFunction" );
wxChar* GmatObjectTypes =
   _T("Spacecraft ForceModel Propagator FuelTank Thruster SolarSystem ")
   _T("CoordinateSystem Variable Array String ReportFile XYPlot OpenGLPlot " )
   _T("ImpulsiveBurn FiniteBurn DifferentialCorrector Optimizer MatlabFunction" );
wxChar* GmatComments =
   _T("%");
// C++
wxChar* CppWordlist1 =
   _T("asm auto bool break case catch char class const const_cast ")
   _T("continue default delete do double dynamic_cast else enum explicit ")
   _T("export extern false float for friend goto if inline int long ")
   _T("mutable namespace new operator private protected public register ")
   _T("reinterpret_cast return short signed sizeof static static_cast ")
   _T("struct switch template this throw true try typedef typeid ")
   _T("typename union unsigned using virtual void volatile wchar_t ")
   _T("while");
wxChar* CppWordlist2 =
   _T("file");
wxChar* CppWordlist3 =
   _T("a addindex addtogroup anchor arg attention author b brief bug c ")
   _T("class code date def defgroup deprecated dontinclude e em endcode ")
   _T("endhtmlonly endif endlatexonly endlink endverbatim enum example ")
   _T("exception f$ f[ f] file fn hideinitializer htmlinclude ")
   _T("htmlonly if image include ingroup internal invariant interface ")
   _T("latexonly li line link mainpage name namespace nosubgrouping note ")
   _T("overload p page par param post pre ref relates remarks return ")
   _T("retval sa section see showinitializer since skip skipline struct ")
   _T("subsection test throw todo typedef union until var verbatim ")
   _T("verbinclude version warning weakgroup $ @ \"\" & < > # { }");

// Python
wxChar* PythonWordlist1 =
   _T("and assert break class continue def del elif else except exec ")
   _T("finally for from global if import in is lambda None not or pass ")
   _T("print raise return try while yield");
wxChar* PythonWordlist2 =
   _T("ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN ")
   _T("BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS ")
   _T("COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX ")
   _T("DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE ")
   _T("LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON ")
   _T("RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 ")
   _T("STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY");


//----------------------------------------------------------------------------
//! languages
//------------------------------------------------
// Lexical states for SCLEX_MATLAB (from stc.h)
//------------------------------------------------
//#define wxSTC_MATLAB_DEFAULT 0
//#define wxSTC_MATLAB_COMMENT 1
//#define wxSTC_MATLAB_COMMAND 2
//#define wxSTC_MATLAB_NUMBER  3
//#define wxSTC_MATLAB_KEYWORD 4
// single quoted string
//#define wxSTC_MATLAB_STRING 5
//#define wxSTC_MATLAB_OPERATOR 6
//#define wxSTC_MATLAB_IDENTIFIER 7
//#define wxSTC_MATLAB_DOUBLEQUOTESTRING 8
//------------------------------------------------

const GmatEditor::LanguageInfoType GmatEditor::globalLanguagePrefs [] =
{
   //-------------------------------------------------------
   // GMAT script, function, Matlab scripts (style 0)
   //-------------------------------------------------------
   {_T("GMAT"),
    _T("*.script;*.m;*.gmf"),
    #if 1
    // using matlab style - the order is defined in stc.h
    wxSTC_LEX_CONTAINER, // Shows GMAT comments, but no commands in predefined color
    {{GMAT_STC_TYPE_DEFAULT, NULL},            //  0 DEFAULT
     {GMAT_STC_TYPE_COMMENT, NULL},            //  1 COMMENT
     {GMAT_STC_TYPE_COMMAND, NULL},			   //  2 COMMAND
//     {GMAT_STC_TYPE_COMMAND, GmatCommands},    //  2 COMMAND
     {GMAT_STC_TYPE_NUMBER, NULL},             //  3 NUMBER
     {GMAT_STC_TYPE_WORD1, NULL},			   //  4 KEYWORDS
//     {GMAT_STC_TYPE_WORD1, GmatObjectTypes},   //  4 KEYWORDS
     {GMAT_STC_TYPE_STRING, NULL},             //  5 STRING
     {GMAT_STC_TYPE_OPERATOR, NULL},           //  6 OPERATOR
     {GMAT_STC_TYPE_IDENTIFIER, NULL},         //  7 IDENTIFIER
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENT | GMAT_STC_FOLD_COMPACT |
    GMAT_STC_FOLD_PREPROC},
   #endif
    // using python style - the order is defined in stc.h
    #if 0
    wxSTC_LEX_PYTHON, // Shows GMAT commands and folds, but no comments, no object types
    {{GMAT_STC_TYPE_DEFAULT, NULL},               //  0 DEFAULT
     {GMAT_STC_TYPE_COMMENT_LINE, NULL},          //  1 COMMENTLINE
     {GMAT_STC_TYPE_NUMBER, NULL},                //  2 NUMBER
     {GMAT_STC_TYPE_STRING, NULL},                //  3 STRING
     {GMAT_STC_TYPE_CHARACTER, NULL},             //  4 CHARACTER
     {GMAT_STC_TYPE_WORD1, GmatCommands},         //  5 WORD
     {GMAT_STC_TYPE_DEFAULT, NULL},               //  6 TRIPLE
     {GMAT_STC_TYPE_DEFAULT, NULL},               //  7 TRIPLEDOUBLE
     {GMAT_STC_TYPE_DEFAULT, NULL},               //  8 CLASSNAME
     {GMAT_STC_TYPE_WORD2, GmatObjectTypes},      //  9 DEFNAME
     {GMAT_STC_TYPE_OPERATOR, NULL},              // 10 OPERATOR
     {GMAT_STC_TYPE_IDENTIFIER, NULL},            // 11 IDENTIFIER
     {GMAT_STC_TYPE_COMMENT_DOC, NULL},           // 12 COMMENTBLOCK
     {GMAT_STC_TYPE_STRING_EOL, NULL},            // 13 STRINGEOL
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENT | GMAT_STC_FOLD_COMPACT |
    GMAT_STC_FOLD_PREPROC},
   #endif
    // cpp style
    #if 0
    wxSTC_LEX_CPP, // Shows GMAT commands, object types, but no comments, no folds
    {{GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_COMMENT, NULL},
     {GMAT_STC_TYPE_COMMENT_LINE, NULL},
     {GMAT_STC_TYPE_COMMENT_DOC, NULL},
     {GMAT_STC_TYPE_NUMBER, NULL},
     {GMAT_STC_TYPE_WORD1, GmatCommands},   // KEYWORDS
     {GMAT_STC_TYPE_STRING, NULL},
     {GMAT_STC_TYPE_CHARACTER, NULL},
     {GMAT_STC_TYPE_UUID, NULL},
     {GMAT_STC_TYPE_PREPROCESSOR, NULL},
     {GMAT_STC_TYPE_OPERATOR, NULL},
     {GMAT_STC_TYPE_IDENTIFIER, NULL},
     {GMAT_STC_TYPE_STRING_EOL, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},         // VERBATIM
     {GMAT_STC_TYPE_REGEX, NULL},
     {GMAT_STC_TYPE_COMMENT_SPECIAL, NULL}, // DOXY
     {GMAT_STC_TYPE_WORD2, GmatObjectTypes},// EXTRA WORDS
     {GMAT_STC_TYPE_WORD3, GmatCommands},   // DOXY KEYWORDS
     {GMAT_STC_TYPE_ERROR, NULL},           // KEYWORDS ERROR
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENT | GMAT_STC_FOLD_COMPACT |
    GMAT_STC_FOLD_PREPROC},
    #endif
   //======================================================================
   // MATLAB
   //======================================================================
   {_T("MATLAB"),
    _T("*.m;"),
    wxSTC_LEX_MATLAB, // Shows GMAT comments, but no commands
    {{GMAT_STC_TYPE_DEFAULT, NULL},            //  0 DEFAULT
     {GMAT_STC_TYPE_COMMENT, NULL},            //  1 COMMENT
     {GMAT_STC_TYPE_COMMAND, GmatCommands},    //  2 COMMAND
     {GMAT_STC_TYPE_NUMBER, NULL},             //  3 NUMBER
     {GMAT_STC_TYPE_WORD1, GmatObjectTypes},   //  4 KEYWORDS
     {GMAT_STC_TYPE_STRING, NULL},             //  5 STRING
     {GMAT_STC_TYPE_OPERATOR, NULL},           //  6 OPERATOR
     {GMAT_STC_TYPE_IDENTIFIER, NULL},         //  7 IDENTIFIER
     {-1, NULL},              //  8 KEYWORDS
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENT | GMAT_STC_FOLD_COMPACT |
    GMAT_STC_FOLD_PREPROC},
   //======================================================================
   // C++
   //======================================================================
   {_T("C++"),
    _T("*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma"),
    wxSTC_LEX_CPP,
    {{GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_COMMENT, NULL},
     {GMAT_STC_TYPE_COMMENT_LINE, NULL},
     {GMAT_STC_TYPE_COMMENT_DOC, NULL},
     {GMAT_STC_TYPE_NUMBER, NULL},
     {GMAT_STC_TYPE_WORD1, CppWordlist1},   // KEYWORDS
     {GMAT_STC_TYPE_STRING, NULL},
     {GMAT_STC_TYPE_CHARACTER, NULL},
     {GMAT_STC_TYPE_UUID, NULL},
     {GMAT_STC_TYPE_PREPROCESSOR, NULL},
     {GMAT_STC_TYPE_OPERATOR, NULL},
     {GMAT_STC_TYPE_IDENTIFIER, NULL},
     {GMAT_STC_TYPE_STRING_EOL, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},         // VERBATIM
     {GMAT_STC_TYPE_REGEX, NULL},
     {GMAT_STC_TYPE_COMMENT_SPECIAL, NULL}, // DOXY
     {GMAT_STC_TYPE_WORD2, CppWordlist2},   // EXTRA WORDS
     {GMAT_STC_TYPE_WORD3, CppWordlist3},   // DOXY KEYWORDS
     {GMAT_STC_TYPE_ERROR, NULL},           // KEYWORDS ERROR
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENT | GMAT_STC_FOLD_COMPACT |
    GMAT_STC_FOLD_PREPROC},
   //======================================================================
   // Python
   //======================================================================
   {_T("Python"),
    _T("*.py;*.pyw"),
    wxSTC_LEX_PYTHON,
    {{GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_COMMENT_LINE, NULL},
     {GMAT_STC_TYPE_NUMBER, NULL},
     {GMAT_STC_TYPE_STRING, NULL},
     {GMAT_STC_TYPE_CHARACTER, NULL},
     {GMAT_STC_TYPE_WORD1, PythonWordlist1},   // KEYWORDS
     {GMAT_STC_TYPE_DEFAULT, NULL},            // TRIPLE
     {GMAT_STC_TYPE_DEFAULT, NULL},            // TRIPLEDOUBLE
     {GMAT_STC_TYPE_DEFAULT, NULL},            // CLASSNAME
     {GMAT_STC_TYPE_DEFAULT, PythonWordlist2}, // DEFNAME
     {GMAT_STC_TYPE_OPERATOR, NULL},
     {GMAT_STC_TYPE_IDENTIFIER, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},            // COMMENT_BLOCK
     {GMAT_STC_TYPE_STRING_EOL, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    GMAT_STC_FOLD_COMMENTPY | GMAT_STC_FOLD_QUOTESPY},
   //======================================================================
   // * (any)
   //======================================================================
   #ifdef _UNICODE
   {(wxChar *)(wxString(DEFAULT_LANGUAGE).wc_str()),
   #else
   {(wxChar *)DEFAULT_LANGUAGE,
   #endif
    _T("*.*"),
    wxSTC_LEX_PROPERTIES,
    {{GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},
     {GMAT_STC_TYPE_DEFAULT, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL},
     {-1, NULL}},
    0},
};

const int GmatEditor::globalLanguagePrefsSize = WXSIZEOF(GmatEditor::globalLanguagePrefs);

//----------------------------------------------------------------------------
//! style types
const GmatEditor::StyleInfoType GmatEditor::globalStylePrefs [] =
{
   // GMAT_STC_TYPE_DEFAULT 0
   {_T("Default"),
    _T("BLACK"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_WORD1 (with wxSTC_LEX_PYTHON, GMAT KeyWords shows in this color)
   {_T("Keyword1"),
    _T("Custom"), _T("WHITE"),
    //_T(""), 10, GMAT_STC_STYLE_BOLD, 0},
    _T(""), 10, 0, 0, 11505963},

   // GMAT_STC_TYPE_WORD2
   {_T("Keyword2"),
    _T("RED"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_WORD3
   {_T("Keyword3"),
    _T("CORNFLOWER BLUE"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_WORD4
   {_T("Keyword4"),
    _T("CYAN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_WORD5
   {_T("Keyword5"),
    _T("DARK GREY"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_WORD6
   {_T("Keyword6"),
    _T("GREY"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_COMMENT 7
   {_T("Comment"),
    _T("FOREST GREEN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_COMMENT_DOC 8
   {_T("Comment (Doc)"),
    _T("FOREST GREEN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_COMMENT_LINE 9
   {_T("Comment line"),
    _T("FOREST GREEN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_COMMENT_SPECIAL 10
   {_T("Special comment"),
    _T("FOREST GREEN"), _T("WHITE"),
    _T(""), 10, GMAT_STC_STYLE_ITALIC, 0, 0},

   // GMAT_STC_TYPE_CHARACTER (with wxSTC_LEX_PYTHON, string inside single quote) 11
   {_T("Character"),
    _T("PURPLE"), _T("WHITE"),
    //_T("KHAKI"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_CHARACTER_EOL 12
   {_T("Character (EOL)"),
    _T("PURPLE"), _T("WHITE"),
    //_T("KHAKI"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_STRING (with wxSTC_LEX_PYTHON, string inside double quote) 13
   {_T("String"),
    _T("BROWN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_STRING_EOL 14
   {_T("String (EOL)"),
    _T("BROWN"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_DELIMITER 15
   {_T("Delimiter"),
    _T("ORANGE"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_PUNCTUATION 16
   {_T("Punctuation"),
    _T("ORANGE"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_OPERATOR (with wxSTC_LEX_PYTHON, () [] math operators shown in this color) 17
   {_T("Operator"),
    _T("BLACK"), _T("WHITE"),
    //_T(""), 10, GMAT_STC_STYLE_BOLD, 0},
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_BRACE 18
   {_T("Label"),
    _T("VIOLET"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_COMMAND 19
   {_T("Command"),
    _T("BLUE"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_IDENTIFIER (with wxSTC_LEX_PYTHON, statesments showns in this color) 20
   {_T("Identifier"),
    _T("BLACK"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_LABEL 21
   {_T("Label"),
    _T("VIOLET"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_NUMBER 22
   {_T("Number"),
    _T("SIENNA"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_PARAMETER 23
   {_T("Parameter"),
    _T("VIOLET"), _T("WHITE"),
    _T(""), 10, GMAT_STC_STYLE_ITALIC, 0, 0},

   // GMAT_STC_TYPE_REGEX 24
   {_T("Regular expression"),
    _T("ORCHID"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_UUID 25
   {_T("UUID"),
    _T("ORCHID"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_VALUE 26
   {_T("Value"),
    _T("ORCHID"), _T("WHITE"),
    _T(""), 10, GMAT_STC_STYLE_ITALIC, 0, 0},

   // GMAT_STC_TYPE_PREPROCESSOR 27
   {_T("Preprocessor"),
    _T("GREY"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_SCRIPT 28
   {_T("Script"),
    _T("DARK GREY"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_ERROR 29
   {_T("Error"),
    _T("RED"), _T("WHITE"),
    _T(""), 10, 0, 0, 0},

   // GMAT_STC_TYPE_UNDEFINED 30
   {_T("Undefined"),
    _T("ORANGE"), _T("WHITE"),
    _T(""), 10, 0, 0, 0}

};

const int GmatEditor::globalStylePrefsSize = WXSIZEOF(GmatEditor::globalStylePrefs);
