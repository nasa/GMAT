//$Id$
//------------------------------------------------------------------------------
//                              MdiChildGroundTrackFrame
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
// Created: 2011/06/06
/**
 * Declares MdiChildGroundTrackFrame for ground track plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildGroundTrackFrame_hpp
#define MdiChildGroundTrackFrame_hpp

#include "MdiChildViewFrame.hpp"
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

class MdiChildGroundTrackFrame: public MdiChildViewFrame
{
public:
   MdiChildGroundTrackFrame(wxMDIParentFrame *parent, const wxString& plotName,
                            const wxString& title, const wxPoint& pos,
                            const wxSize& size, const long style);
   virtual ~MdiChildGroundTrackFrame();
   
   virtual void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
protected:
   
   DECLARE_EVENT_TABLE();
};
#endif
