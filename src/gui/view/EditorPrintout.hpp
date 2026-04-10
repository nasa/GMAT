//$Id$
//------------------------------------------------------------------------------
//                              EditorPrint
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
 * Declares EditorPrintout class.
 */
//------------------------------------------------------------------------------
#ifndef EditorPrintout_hpp
#define EditorPrintout_hpp

#include "ScriptEditor.hpp"
#include <wx/print.h>

class EditorPrintout: public wxPrintout
{
public:

   // constructor
   EditorPrintout (ScriptEditor *editor, wxChar *title = NULL);

   // event handlers
   bool OnPrintPage (int page);
   bool OnBeginDocument (int startPage, int endPage);

   // print functions
   bool HasPage (int page);
   void GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

private:
   ScriptEditor *mEditor;
   int mPagePrinted;
   wxRect mPageRect;
   wxRect mPrintRect;

   bool PrintScaling (wxDC *dc);
};

#endif
