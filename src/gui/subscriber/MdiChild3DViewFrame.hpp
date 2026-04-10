//$Id$
//------------------------------------------------------------------------------
//                              MdiChild3DViewFrame
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
// Created: 2003/11/25
/**
 * Declares MdiChild3DViewFrame for 3D visualization
 */
//------------------------------------------------------------------------------
#ifndef MdiChild3DViewFrame_hpp
#define MdiChild3DViewFrame_hpp

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

class MdiChild3DViewFrame: public MdiChildViewFrame
{
public:
   MdiChild3DViewFrame(wxMDIParentFrame *parent, const wxString& plotName,
                       const wxString& title, const wxPoint& pos,
                       const wxSize& size, const long style);
   virtual ~MdiChild3DViewFrame();
      
   virtual void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                        bool drawXyPlane, bool drawWireFrame, bool drawAxes,
                        bool drawGrid, bool drawSunLine, bool overlapPlot,
                        bool usevpInfo, bool drawStars, bool drawConstellations,
                        Integer starCount);
   
   virtual void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
protected:
   
   DECLARE_EVENT_TABLE();
};
#endif
