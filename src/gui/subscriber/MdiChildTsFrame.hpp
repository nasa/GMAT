//$Id$
//------------------------------------------------------------------------------
//                              MdiChildTsFrame
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
// Author: Darrel Conway
// Created: 2005/05/05
/**
 * Declares MdiChildTsFrame class for ts plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTsFrame_hpp
#define MdiChildTsFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "TsPlotXYCanvas.hpp"

// // For compilers that support precompilation, includes "wx/wx.h".
// #include "wx/wxprec.h"

// #ifdef __BORLANDC__
// #pragma hdrstop
// #endif

// #ifndef WX_PRECOMP
// #include "wx/wx.h"
// #include "wx/mdi.h"
// #endif

#include "gmatdefs.hpp"     // for UnsignedInt


/**
 * The frame used to XY plots based on the TsPlot library
 */
class MdiChildTsFrame: public GmatMdiChildFrame
{
public:
   TsPlotCanvas   *mXyPlot;
   wxTextCtrl     *mLogTextCtrl;
   wxMenu         *mViewOptionMenu;
   
   bool mIsMainFrame;
   
   MdiChildTsFrame(wxMDIParentFrame *parent, bool isMainFrame,
                   const wxString &plotName, const wxString& plotTitle,
                   const wxString& xAxisTitle, const wxString& yAxisTitle,
                   const wxPoint& pos, const wxSize& size, const long style);
   virtual ~MdiChildTsFrame();
   
   int  ReadXyPlotFile(const wxString &filename);
   bool DeletePlot();
   void SetPlotTitle(const wxString &title);
   void ShowPlotLegend();
   void AddPlotCurve(int curveIndex, const wxString &curveTitle,
                     UnsignedInt penColor);
   void DeleteAllPlotCurves();
   void DeletePlotCurve(int curveIndex);
   void AddDataPoints(int curveIndex, double xData, double yData, Real hi = 0.0,
         Real lo = 0.0);
   void ClearPlotData();
   void PenUp();
   void PenDown();
   void Darken(int factor, int index, int forCurve = -1);
   void Lighten(int factor, int index, int forCurve = -1);
   void MarkPoint(int index, int forCurve = -1);
   void MarkBreak(int index, int forCurve = -1);
   void ClearFromBreak(int startBreakNumber, int endBreakNumber = -1,
         int forCurve = -1);
   void ChangeColor(int index, unsigned long newColor = 0xffffff,
         int forCurve = -1);
   void ChangeMarker(int index, int newMarker, int forCurve = -1);
   void SetLineWidth(int w,  int lineId = -1);
   void SetLineStyle(int ls, int lineId = -1);

   void Rescale();
   bool IsActive();
   void IsActive(bool yesno);

   void CurveSettings(bool useLines = true, Integer lineWidth = 1,
         Integer lineStyle = wxSOLID, bool useMarkers = false,
         Integer markerSize = 3, Integer marker = circleMarker,
         bool useHiLow = false, Integer forCurve = -1);
   void RedrawCurve();
    
   // getter
   wxString GetPlotTitle() {return mPlotTitle;}
   wxString GetXAxisTitle() {return mXAxisTitle;}
   wxString GetYAxisTitle() {return mYAxisTitle;}
   int GetCurveCount() {return mXyPlot->GetCurveCount();}
   //bool GetShowGrid() {return mXyPlot->GetShowGrid();}
   
   // setter
   void SetPlotName(const wxString &name);
   void SetXAxisTitle(const wxString &title) {mXAxisTitle = title;}
   void SetYAxisTitle(const wxString &title) {mYAxisTitle = title;}
   void SetShowGrid(bool show);
   void ResetZoom();
   
   // menu events
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnDrawGrid(wxCommandEvent& event);
   void OnDrawDottedLine(wxCommandEvent& event);
   void OnHelpView(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
   void OnOpenXyPlotFile(wxCommandEvent& event);

   // window events
   //void OnPlotClick(wxPlotEvent &event);
   void OnActivate(wxActivateEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnPlotClose(wxCloseEvent& event);
   void OnClose(wxCloseEvent& event);
   void TakeAction(const std::string &action);

protected:
   
   void AdjustYScale();
   double GetYMin();
   double GetYMax();
   
   // TS plot does not have curve size limit.  Setting one here is artificial,
   // and only needed because of the mHasFirstXSet[] array below.  That array
   // should be replaced by a std::vector<bool> and this artifice removed.
   static const int MAX_NUM_CURVE = 20;      // Changed from 6 for GMT-2411
    
   wxString mPlotTitle;
   wxString mXAxisTitle;
   wxString mYAxisTitle;
   bool mHasFirstXSet[MAX_NUM_CURVE];
   bool isActive;
    
   DECLARE_EVENT_TABLE()
};
#endif
