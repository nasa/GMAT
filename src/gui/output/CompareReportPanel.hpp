//$Id$
//------------------------------------------------------------------------------
//                              CompareReportPanel
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
// Created: 2004/02/02
//
/**
 * Declares CompareReportPanel class.
 */
//------------------------------------------------------------------------------

#ifndef CompareReportPanel_hpp
#define CompareReportPanel_hpp

#include "gmatwxdefs.hpp"

class CompareReportPanel : public wxPanel
{
public:
   // constructors
   CompareReportPanel( wxWindow *parent, const wxString &name);

   void AppendText(const wxString &text);
   void SetMaxLength(unsigned long len)
      { mTextCtrl->SetMaxLength(len); };
   int GetNumberOfLines()
      { return mTextCtrl->GetNumberOfLines(); };
   void ClearText()
      { mTextCtrl->Clear(); };
   wxTextCtrl *GetTextCtrl()
      { return mTextCtrl; };
   
   void OnClose(wxCommandEvent &event);

protected:

   // member data
   wxString mName;
   wxTextCtrl *mTextCtrl;
   
   wxButton *theCloseButton;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_TEXTCTRL,
   };

};

#endif // CompareReportPanel_hpp
