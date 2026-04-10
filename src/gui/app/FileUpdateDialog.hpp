//$Id$
//------------------------------------------------------------------------------
//                              FileUpdateDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Thomas G. Grubb, NASA/GSFC
// Created: 2016/01/04
//
/**
 * Declares FileUpdateDialog class.
 */
//------------------------------------------------------------------------------

#ifndef FileUpdateDialog_hpp
#define FileUpdateDialog_hpp

#include "gmatwxdefs.hpp"
#include "IFileUpdater.hpp"
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/checklst.h>
#include <wx/msgdlg.h>

class FileUpdateDialog : public wxDialog
{
public:
   
   // constructors
   FileUpdateDialog(wxWindow *parent, wxWindowID id, const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCLOSE_BOX);
   
   void InitializeFiles();
   bool IsEmpty();
   bool GenerateBatchFile();
   void OnSelect(wxCommandEvent &event);
   void OnOK(wxCommandEvent &event);
   void OnCancel(wxCommandEvent &event);
   
protected:
   
   wxButton       *mSelectButton;
   wxButton       *mOkButton;
   wxButton       *mCancelButton;
   wxGrid         *mGrid;
   IFileUpdater   *updateUtil;
   bool           mIsEmpty;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8100,
      ID_BUTTON_CANCEL,
      ID_BUTTON_SELECT,
      ID_CHECKLISTBOX,
   };

};

#endif // FileUpdateDialog_hpp
