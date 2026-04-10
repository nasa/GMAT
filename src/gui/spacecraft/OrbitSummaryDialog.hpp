//
//------------------------------------------------------------------------------
//                         OrbitSummaryDialog
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
// Author: Evelyn Hull
// Created: 2011/10/08
//
/**
 * Implements the dialog used to show a summary of the calculated orbit
 * parameters
 */
//------------------------------------------------------------------------------


#include "GmatDialog.hpp"

#ifndef OrbitSummaryDialog_hpp
#define OrbitSummaryDialog_hpp

class OrbitSummaryDialog : public GmatDialog
{
public:
   // constructors
   OrbitSummaryDialog(wxWindow *parent, wxString &summary);
   ~OrbitSummaryDialog();

private:
   wxTextCtrl *orbitSummary;
   wxString orbitSummaryString;

   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   DECLARE_EVENT_TABLE();

   enum
   {
	  ID_TEXTCTRL = 8000
   };
};

#endif //OrbitSummaryDialog_hpp
