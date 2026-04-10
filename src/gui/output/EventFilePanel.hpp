//$Id$
//------------------------------------------------------------------------------
//                              EventFilePanel
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
// Author: Allison Greene
// Created: 2005/04/18
//
/**
 * Declares EventFilePanel class.
 */
//------------------------------------------------------------------------------

#ifndef EventFilePanel_hpp
#define EventFilePanel_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "EventLocator.hpp"

class EventFilePanel : public wxPanel
{
public:
   // constructors
   EventFilePanel( wxWindow *parent, wxString reportName);

   void OnClose(wxCommandEvent &event);
   void OnHelp();

   wxTextCtrl *mFileContentsTextCtrl;

protected:
   // member functions
   void Create();
   void Show();
   void LoadData();

   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxString mLocatorName;
   
   wxWindow *theParent;
    
   wxBoxSizer *thePanelSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;

   wxButton *theCloseButton;
   wxButton *theHelpButton;

   EventLocator *theLocator;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_BUTTON_HELP,
      ID_TEXTCTRL,
   };

};

#endif // EventFilePanel_hpp
