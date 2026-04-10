//$Id$
//------------------------------------------------------------------------------
//                              EditorPrintout
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
// Created: 2008/01/06
//
/**
 * Implements EditorPrintout class.
 */
//------------------------------------------------------------------------------

#include "EditorPrintout.hpp"
#include "GmatAppData.hpp"         // for globalPageSetupData
#include "MessageInterface.hpp"

//#define DEBUG_PRINTOUT

//------------------------------------------------------------------------------
// EditorPrintout(Editor *editor, wxChar *title)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * @param editor	the editor that supplies the contents
 * @param title		title of the printout
 */
EditorPrintout::EditorPrintout(ScriptEditor *editor, wxChar *title)
   : wxPrintout(title)
{
   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage
      ("EditorPrintout::EditorPrintout() entered, editor=<%p>\n", editor);
   #endif

   mEditor = editor;
   mPagePrinted = 0;
}


//------------------------------------------------------------------------------
// bool OnPrintPage(int page)
//------------------------------------------------------------------------------
/**
 * Event handler for when printing a single page 
 * 
 * @param page	page number to print
 * @return		status of the print (true if printed)
 */
bool EditorPrintout::OnPrintPage(int page)
{
   wxDC *dc = GetDC();
   if(!dc)
      return false;

   // scale DC
   PrintScaling(dc);

   // print page
   if(page == 1)
      mPagePrinted = 0;

   mPagePrinted = mEditor->FormatRange(1, mPagePrinted, mEditor->GetLength(),
                                       dc, dc, mPrintRect, mPageRect);

   return true;
}


//------------------------------------------------------------------------------
// bool OnBeginDocument(int startPage, int endPage)
//------------------------------------------------------------------------------
/**
 * Event handler for when printing a range of pages
 * 
 * @param startPage	page number for first page to print
 * @param endPage	page number for last page to print
 * @return			status of the print (true if printed)
 */
bool EditorPrintout::OnBeginDocument(int startPage, int endPage)
{
   if(!wxPrintout::OnBeginDocument(startPage, endPage))
      return false;

   return true;
}


//------------------------------------------------------------------------------
// void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
//------------------------------------------------------------------------------
/**
 * Gets Page information 
 * 
 * @param minPage		returns the minimum number of pages that can be printed
 * @param minPage		returns the maximum number of pages that can be printed
 * @param selPageFrom	returns the first page of the selection
 * @param selPageTo		returns the last page of the selection
 */
void EditorPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
                                 int *selPageTo)
{
   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage("EditorPrintout::GetPageInfo() entered\n");
   #endif

   // initialize values
   *minPage = 0;
   *maxPage = 0;
   *selPageFrom = 0;
   *selPageTo = 0;

   // scale DC if possible
   wxDC *dc = GetDC();
   if(!dc)
      return;

   PrintScaling(dc);

   // get print page informations and convert to printer pixels
   wxSize ppiScr;
   GetPPIScreen(&ppiScr.x, &ppiScr.y);
   wxSize page = globalPageSetupData->GetPaperSize();
   page.x = static_cast<int>(page.x * ppiScr.x / 25.4);
   page.y = static_cast<int>(page.y * ppiScr.y / 25.4);
   mPageRect = wxRect(0, 0, page.x, page.y);

   // get margins informations and convert to printer pixels
   wxPoint pt = globalPageSetupData->GetMarginTopLeft();
   int left = pt.x;
   int top = pt.y;
   pt = globalPageSetupData->GetMarginBottomRight();
   int right = pt.x;
   int bottom = pt.y;

   top = static_cast<int>(top * ppiScr.y / 25.4);
   bottom = static_cast<int>(bottom * ppiScr.y / 25.4);
   left = static_cast<int>(left * ppiScr.x / 25.4);
   right = static_cast<int>(right * ppiScr.x / 25.4);

   mPrintRect = wxRect(left, top,
                       page.x -(left + right), page.y -(top + bottom));

   // count pages
   //while (HasPage(*maxPage)) <== causing infinite loop (loj)
   if (HasPage(*maxPage))
   {
      mPagePrinted =
         mEditor->FormatRange(0, mPagePrinted, mEditor->GetLength(),
                              dc, dc, mPrintRect, mPageRect);

      #ifdef DEBUG_PRINTOUT
      MessageInterface::ShowMessage("   mPagePrinted=%d\n", mPagePrinted);
      #endif

      *maxPage += 1;
   }

   if(*maxPage > 0)
      *minPage = 1;

   *selPageFrom = *minPage;
   *selPageTo = *maxPage;

   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage("EditorPrintout::GetPageInfo() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool HasPage(int WXUNUSED(page))
//------------------------------------------------------------------------------
/**
 * Returns whether the page exists
 * 
 * @return			true if page has been printed
 */
bool EditorPrintout::HasPage(int WXUNUSED(page))
{
   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage
      ("EditorPrintout::HasPage() mPagePrinted=%d, mEditor->GetLength()=%d\n",
       mPagePrinted, mEditor->GetLength());
   #endif

   bool hasPage = mPagePrinted < mEditor->GetLength();

   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage
      ("EditorPrintout::HasPage() returning %d\n", hasPage);
   #endif

   return hasPage;
}


//------------------------------------------------------------------------------
// bool PrintScaling(wxDC *dc)
//------------------------------------------------------------------------------
/**
 * Scales Print Device context
 * 
 * @return			true if scaled
 */
bool EditorPrintout::PrintScaling(wxDC *dc)
{
   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage("EditorPrintout::PrintScaling() entered\n");
   #endif

   // check for dc, return if none
   if(!dc)
      return false;

   // get printer and screen sizing values
   wxSize ppiScr;
   GetPPIScreen(&ppiScr.x, &ppiScr.y);

   if(ppiScr.x == 0)
   { // most possible guess 96 dpi
      ppiScr.x = 96;
      ppiScr.y = 96;
   }

   wxSize ppiPrt;
   GetPPIPrinter(&ppiPrt.x, &ppiPrt.y);

   if(ppiPrt.x == 0)
   { // scaling factor to 1
      ppiPrt.x = ppiScr.x;
      ppiPrt.y = ppiScr.y;
   }

   wxSize dcSize = dc->GetSize();
   wxSize pageSize;
   GetPageSizePixels(&pageSize.x, &pageSize.y);

   // set user scale
   float scale_x =(float)(ppiPrt.x * dcSize.x) / (float)(ppiScr.x * pageSize.x);
   float scale_y =(float)(ppiPrt.y * dcSize.y) / (float)(ppiScr.y * pageSize.y);
   dc->SetUserScale(scale_x, scale_y);

   #ifdef DEBUG_PRINTOUT
   MessageInterface::ShowMessage
      ("EditorPrintout::PrintScaling() returning true\n");
   #endif

   return true;
}
