//$Id$
//------------------------------------------------------------------------------
//                              MdiChildTrajFrame
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
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTrajFrame_hpp
#define MdiChildTrajFrame_hpp

#include "MdiChildViewFrame.hpp"
#include "OpenGlOptionDialog.hpp"
#include "gmatdefs.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class MdiChildTrajFrame: public MdiChildViewFrame
{
public:
   MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& plotName,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);
   virtual ~MdiChildTrajFrame();
   
   virtual OpenGlOptionDialog* GetOptionDialog();
   virtual void EnableAnimation(bool enable);
   
   virtual void OnShowOptionDialog(wxCommandEvent& event);
   virtual void OnDrawWireFrame(wxCommandEvent& event);
   
protected:
   
   OpenGlOptionDialog *mOptionDialog;
   DECLARE_EVENT_TABLE();
};
#endif
