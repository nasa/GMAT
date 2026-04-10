//------------------------------------------------------------------------------
//                           GroundTrackWindow
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Sep 24, 2025
/**
 * Window containing the ground track plotting area
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#ifndef SRC_GUI_SUBSCRIBER_GroundTrackWindow_HPP_
#define SRC_GUI_SUBSCRIBER_GroundTrackWindow_HPP_

#include "GmatMdiChildFrame.hpp"
#include "GroundTrack.hpp"
#include "GroundTrackArea.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class GroundTrackWindow : public GmatMdiChildFrame
{
public:

   GroundTrackWindow(wxMDIParentFrame *parent,
         const wxString& plotName,
         const wxString& title,
         const GmatTree::ItemType type,
         const wxPoint& pos,
         const wxSize& size,
         const long style);
   virtual ~GroundTrackWindow();

   // menu actions
   virtual void OnChangeTitle(wxCommandEvent& event);
   virtual void OnQuit(wxCommandEvent& event);

   virtual void OnActivate(wxActivateEvent& event);
   virtual void OnMove(wxMoveEvent& event);
   virtual void OnClose(wxCloseEvent &event);
   virtual void OnSize(wxSizeEvent& event);

   virtual void SetSolarSystem(SolarSystem *ss);

   void SetOption(const std::string &optionSetting, const std::string &optionValue = "");
   void SetColor(const int forCurve, const int red, const int green,
         const int blue, const int atIndex = 0);
   virtual void SetLineWidth(int width);
   virtual void LabelData(StringArray dataNames);

   virtual bool AddData(const double epoch, const double *longlat,
         const int satcount);

   void UpdatePlot();
   void ResetForNewRun();
   bool TakeAction(const std::string &theAction);

protected:
   virtual void Create(const std::string &title = "");

   void OnPaint(wxPaintEvent& event);

   /// The title of the display window
   wxString mPlotTitle;

   /// The display widget
   GroundTrackArea *theMap;

   /// Solar system for data lookups
   SolarSystem *theSS;
};

#endif /* SRC_GUI_SUBSCRIBER_GroundTrackWindow_HPP_ */
