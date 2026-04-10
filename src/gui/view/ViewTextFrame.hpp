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
#ifndef ViewTextFrame_hpp
#define ViewTextFrame_hpp

#include "gmatwxdefs.hpp"

class ViewTextFrame: public wxFrame
{
public:

   // constructors
   ViewTextFrame(wxFrame *frame, const wxString& title, int x, int y, int w, int h,
                 const wxString &mode = "Temporary", const wxString &type = "Output");
   // destructor
   ~ViewTextFrame();

   // operations
   void AppendText(const wxString& text);
   void SetMaxLength(unsigned long len)
      { mTextCtrl->SetMaxLength(len); };
   int GetNumberOfLines()
      { return mTextCtrl->GetNumberOfLines(); };
   void ClearText()
      { mTextCtrl->Clear(); };
   void SetWindowMode(const wxString &mode)
      { mWindowMode = mode; };
   wxString GetWindowMode()
      { return mWindowMode; };
   wxTextCtrl *GetTextCtrl()
      { return mTextCtrl; };

private:

   wxMenuBar *CreateMainMenu();

   DECLARE_EVENT_TABLE();

   // event handlers
   void OnClear(wxCommandEvent& event);
   void OnSaveAs(wxCommandEvent& event);
   void OnExit(wxCommandEvent& event);

   // IDs for the controls and the menu commands
   enum
   {
      VIEW_TEXT_CLEAR = 8000,
      VIEW_TEXT_SAVE_AS,
      VIEW_TEXT_EXIT
   };

   /// just some place to put our messages in
   wxTextCtrl *mTextCtrl;

   /// window mode ("Permanent", "Temporary")
   wxString mWindowMode;

   /// text type ("Output", "Script")
   wxString mTextType;
};

#endif
