//$Id$
//------------------------------------------------------------------------------
//                              GmatStaticBoxSizer
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
// Author: Linda Jun, NASA/GSFC
// Created: 2006/12/05
//
/**
 * Implements GmatStaticBoxSizer class.
 */
//------------------------------------------------------------------------------

#include "GmatStaticBoxSizer.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatStaticBoxSizer(wxWindow *parent, const wxString& label = wxEmptyString,
//                    long style = 0)
//------------------------------------------------------------------------------
/**
 * Constructs GmatStaticBoxSizer object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
#if wxCHECK_VERSION(3, 0, 0) || !defined( __WXMAC__)
   GmatStaticBoxSizer::GmatStaticBoxSizer(int orient, wxWindow *parent,
                                          const wxString& label, long style)
      : wxStaticBoxSizer(orient, parent, label)
   {
   }

   void GmatStaticBoxSizer::SetLabel(const wxString& label)
   {
      GetStaticBox()->SetLabel(label);
   }
#else
   GmatStaticBoxSizer::GmatStaticBoxSizer(int orient, wxWindow *parent,
                                          const wxString& label, long style)
      : wxBoxSizer(orient)
   {
      labelText =
         new wxStaticText(parent, -1, label, wxDefaultPosition, wxSize(220,-1), style);
      labelText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                false, _T(""), wxFONTENCODING_SYSTEM));
      Add(labelText);
   }

   void GmatStaticBoxSizer::SetLabel(const wxString& label)
   {
      labelText->SetLabel(label);
   }
#endif

