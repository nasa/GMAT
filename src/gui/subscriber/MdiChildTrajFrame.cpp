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
 * Implements MdiChildTrajFrame for trajectory opengl plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChildTrajFrame.hpp"
#include "TrajPlotCanvas.hpp"
#include "GmatAppData.hpp"

//#define DEBUG_TRAJ_FRAME
//#define DEBUG_MDI_CHILD_FRAME_CLOSE

BEGIN_EVENT_TABLE(MdiChildTrajFrame, MdiChildViewFrame)
   EVT_MENU(GmatPlot::MDI_GL_CHILD_QUIT, MdiChildTrajFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_GL_CHANGE_TITLE, MdiChildTrajFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_GL_CLEAR_PLOT, MdiChildTrajFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, MdiChildTrajFrame::OnShowOptionDialog)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, MdiChildTrajFrame::OnDrawWireFrame)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, MdiChildTrajFrame::OnDrawXyPlane)
   EVT_ACTIVATE(MdiChildTrajFrame::OnActivate)
   EVT_SIZE(MdiChildTrajFrame::OnPlotSize)
   EVT_MOVE(MdiChildTrajFrame::OnMove)
   EVT_CLOSE(MdiChildTrajFrame::OnPlotClose) 
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildTrajFrame::MdiChildTrajFrame(wxMDIParentFrame *parent,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style)
   : MdiChildViewFrame(parent, plotName, title, pos, size, style)
{
   // use this if we want option dialog to be modeless
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
   // Create GLCanvas
   int width, height;
   GetClientSize(&width, &height);
   TrajPlotCanvas *canvas =
      new TrajPlotCanvas(this, -1, wxPoint(0, 0), wxSize(width, height), plotName);
   
   mCanvas = canvas;
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
}


//------------------------------------------------------------------------------
// ~MdiChildTrajFrame()
//------------------------------------------------------------------------------
MdiChildTrajFrame::~MdiChildTrajFrame()
{
   if (mOptionDialog)
      delete mOptionDialog;
   
   mOptionDialog = (OpenGlOptionDialog*)NULL;
   
}


//------------------------------------------------------------------------------
// OpenGlOptionDialog* GetOptionDialog()
//------------------------------------------------------------------------------
OpenGlOptionDialog* MdiChildTrajFrame::GetOptionDialog()
{
   return mOptionDialog;
}


//------------------------------------------------------------------------------
// void EnableAnimationButton(bool enable)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::EnableAnimation(bool enable)
{
   if (mOptionDialog)
      mOptionDialog->EnableAnimation(enable);
}


//------------------------------------------------------------------------------
// void OnShowOptionDialog(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowOptionDialog(wxCommandEvent& event)
{
   #ifdef DEBUG_TRAJ_FRAME
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::OnShowOptionDialog() entered\n");
   #endif
   
   if (mOptionDialog == NULL)
      mOptionDialog = new OpenGlOptionDialog(this, mPlotName, mBodyNames,
                                             mBodyColors);
   
   mOptionDialog->UpdateObjectList(mCanvas->GetObjectNames(),
                                   mCanvas->GetValidCSNames(),
                                   mCanvas->GetShowObjectMap(),
                                   mCanvas->GetObjectColorMap());
   
   int x, y, newX;
   GmatAppData::Instance()->GetMainFrame()->GetPosition(&x, &y);
   newX = x-20;
   
   if (newX < 0)
      newX = 5;
   
   mOptionDialog->Move(newX, y+100);
   mOptionDialog->Show(true); // modeless dialog
}


//------------------------------------------------------------------------------
// void OnDrawWireFrame(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnDrawWireFrame(wxCommandEvent& event)
{
   if (mOptionDialog)
      mOptionDialog->SetDrawWireFrame(event.IsChecked());

   MdiChildViewFrame::OnDrawWireFrame(event);
}


