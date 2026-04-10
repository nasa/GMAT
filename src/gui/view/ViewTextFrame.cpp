//$Id$
//------------------------------------------------------------------------------
//                                 ViewTextFrame
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
// Created: 2003/11/03
//
/**
 * Shows text.
 */
//------------------------------------------------------------------------------
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"
#include "FileManager.hpp"
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(ViewTextFrame, wxFrame)
   EVT_MENU(VIEW_TEXT_CLEAR, ViewTextFrame::OnClear)
   EVT_MENU(VIEW_TEXT_SAVE_AS, ViewTextFrame::OnSaveAs)
   EVT_MENU(VIEW_TEXT_EXIT, ViewTextFrame::OnExit)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ViewTextFrame(wxFrame *frame, const wxString& title,
//              int x, int y, int w, int h, const wxString &mode)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param frame			Parent frame
 * @param title			Title for the frame
 * @param x				window position x
 * @param y				window position y
 * @param w				window width
 * @param h				window height
 * @param mode			window mode ("Permanent", "Temporary")
 * @param type			text type ("Output", "Script")
 */
ViewTextFrame::ViewTextFrame(wxFrame *frame, const wxString& title,
                             int x, int y, int w, int h, const wxString &mode,
                             const wxString &type)
   : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h),
             wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
   CreateStatusBar(2);
   mWindowMode = mode;
   mTextType = type;

   // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
   // increase text size(loj: 2009.02.05)
   mTextCtrl = new wxTextCtrl(this, -1, _T(""), wxPoint(0, 0), wxSize(0, 0),
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH);
   mTextCtrl->SetMaxLength(320000);
   mTextCtrl->SetFont(GmatAppData::Instance()->GetFont());

#if wxUSE_MENUS
   // create a menu bar
   SetMenuBar(CreateMainMenu());
#endif // wxUSE_MENUS

   // Set GMAT main icon
   GmatAppData::Instance()->SetIcon(this, "ViewTextFrame");
   
   CenterOnScreen(wxBOTH);
}


//------------------------------------------------------------------------------
// ~ViewTextFrame()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
ViewTextFrame::~ViewTextFrame()
{
   if (mWindowMode != "Temporary")
   {
      GmatAppData::Instance()->SetCompareWindow(NULL);
   }
}


//------------------------------------------------------------------------------
// void AppendText(const wxString& text)
//------------------------------------------------------------------------------
/**
 * Appends specified text to the text control
 *
 * @param text			text to append
 */
void ViewTextFrame::AppendText(const wxString& text)
{
   mTextCtrl->AppendText(text);
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// wxMenuBar* CreateMainMenu()
//------------------------------------------------------------------------------
/**
 * Creates the main menu bar for the frame
 *
 * @return	wxMenuBar created
 */
wxMenuBar* ViewTextFrame::CreateMainMenu()
{
   // Make a menubar
   wxMenuBar *menuBar = new wxMenuBar;

   wxMenu *menuFile = new wxMenu;
   menuFile->Append(VIEW_TEXT_CLEAR, _T("&Clear"));
   menuFile->AppendSeparator();
   menuFile->Append(VIEW_TEXT_SAVE_AS, _T("&Save As..."));
   menuFile->AppendSeparator();
   menuFile->Append(VIEW_TEXT_EXIT, _T("E&xit"));
   menuBar->Append(menuFile, _T("&File"));

   return menuBar;
}


//------------------------------------------------------------------------------
// void OnClear(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Clear text event handler
 *
 * @param event		command event of control
 */
void ViewTextFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
   mTextCtrl->Clear();
}


//------------------------------------------------------------------------------
// void OnSaveAs(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Save As event handler
 *
 * @param event		command event of control
 */
void ViewTextFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{

   wxString filename;
   if (mTextType == "Script")
   {
      filename =
         wxFileSelector("Choose a file to save", "", "", "script",
                        "Script files (*.script)|*.script", gmatFD_SAVE);
   }
   else
   {
      filename =
         wxFileSelector("Choose a file to save", "", "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        gmatFD_SAVE);
   }

   if (!filename.empty())
      mTextCtrl->SaveFile(filename);

}


//------------------------------------------------------------------------------
// void OnExit(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
/**
 * Exit window event handler
 *
 * @param event		command event of control
 */
void ViewTextFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
   if (mWindowMode == "Temporary")
      Close(true);
   else
      Show(false);
}

