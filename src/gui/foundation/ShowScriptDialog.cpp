//$Id$
//------------------------------------------------------------------------------
//                         ShowScriptDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/03/08
//
/**
 * Implements the dialog used to show scripting for individual components. 
 */
//------------------------------------------------------------------------------


#include "ShowScriptDialog.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
//  ShowScriptDialog(wxWindow *parent, wxWindowID id, const wxString& title, 
//                   GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * FiniteBurn constructor (default constructor).
 * 
 * @param parent Parent wxWindow for the dialog.
 * @param id wxWidget identifier for the dialog.
 * @param title Title for the dialog.
 * @param obj The object that provides the script text.
 */
//------------------------------------------------------------------------------
ShowScriptDialog::ShowScriptDialog(wxWindow *parent, wxWindowID id, 
                                    const wxString& title, GmatBase *obj, 
                                    bool isSingleton) :
   GmatDialog(parent, id, title, NULL, wxDefaultPosition, wxDefaultSize, 
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
   theObject(obj),
   showAsSingleton(isSingleton)
{
   Create();
   ShowData();
   theOkButton->Enable(true);
   theDialogSizer->Hide(theCancelButton, true);
   theDialogSizer->Layout();
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and populates the dialog.
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::Create()
{
   Integer w, h;
   wxString text = "Script not yet available for this panel";
   // Find the height of a line of test, to use when sizing the text control
   GetTextExtent(text, &w, &h);

   wxSize scriptPanelSize(500, 32);

   try
   {      
      if (theObject != NULL)
      {
         std::string genstring =
            theObject->GetGeneratingString(Gmat::SHOW_SCRIPT).c_str();
         if (showAsSingleton)
         {
            // remove the "Create" line
            Integer loc = genstring.find("\n");
            genstring = genstring.substr(loc+1);
         }
         text = genstring.c_str();   
         StringArray sar = theObject->GetGeneratingStringArray(Gmat::SHOW_SCRIPT);
         Integer size = sar.size();
         // set max height to 550(loj: 2008.11.25)
         Integer height = 32 + (size+1) * h;
         height = height > 550 ? 550 : height;
         scriptPanelSize.Set(500, height);
      }
      
      // Set additional style wxTE_RICH to Ctrl + mouse scroll wheel to decrease or
      // increase text size on Windows (loj: 2009.02.05)
      theScript = new wxTextCtrl(this, -1, text, wxPoint(0,0), scriptPanelSize, 
                                 wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL | wxTE_RICH);
      theScript->SetFont( GmatAppData::Instance()->GetFont() );
      theMiddleSizer->Add(theScript, 1, wxGROW|wxALL, 3);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::LoadData()
{
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::SaveData()
{
}


//------------------------------------------------------------------------------
// void ResetData()
//------------------------------------------------------------------------------
/**
 * For the ShowScriptDialog, does nothing..
 */
//------------------------------------------------------------------------------
void ShowScriptDialog::ResetData()
{
}
