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
 * Implements MdiChildGroundTrackFrame for ground track plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChildGroundTrackFrame.hpp"
#include "GroundTrackCanvas.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"            // for theMdiChildren->Append
#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(MdiChildGroundTrackFrame, MdiChildViewFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildViewFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildViewFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildViewFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildViewFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildViewFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildViewFrame::OnDrawXyPlane)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildGroundTrackFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildGroundTrackFrame::MdiChildGroundTrackFrame(wxMDIParentFrame *parent,
                             const wxString& plotName, const wxString& title,
                             const wxPoint& pos, const wxSize& size,
                             const long style)
   : MdiChildViewFrame(parent, plotName, title, pos, size, style, "2D",
                       GmatTree::OUTPUT_GROUND_TRACK_PLOT)
{
   // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   GroundTrackCanvas *canvas =
      new GroundTrackCanvas(this, -1, wxPoint(0, 0), wxSize(width, height), plotName);
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
}


//------------------------------------------------------------------------------
// ~MdiChildGroundTrackFrame()
//------------------------------------------------------------------------------
MdiChildGroundTrackFrame::~MdiChildGroundTrackFrame()
{   
}


//------------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &textureMap, Integer footPrintOption)
//------------------------------------------------------------------------------
void MdiChildGroundTrackFrame::SetGl2dDrawingOption(const std::string &centralBodyName,
                                                    const std::string &textureMap,
                                                    Integer footPrintOption)
{
   if (mCanvas)
   {
      mCanvas->SetGl2dDrawingOption(centralBodyName, textureMap, footPrintOption);
   }
}


