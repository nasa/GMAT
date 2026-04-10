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
// Author: Linda Jun
// Created: 2004/01/20
/**
 * Implements MdiChildXyFrame class for xy plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"          // for WX_TO_STD_STRING macro
#include "gmatwxrcs.hpp"
#include "MdiChildTsFrame.hpp"
#include "Subscriber.hpp"
#include "SubscriberException.hpp"

#include "MdiTsPlotData.hpp"
#include "TsPlotCurve.hpp"
#include "TsPlotXYCanvas.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"       // for theMdiChildren->Append
#include "RealUtilities.hpp"       // for Abs(), Min(), Max()
#include <fstream>                 // for ifstream (plot input file)

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/gdicmn.h"             // for color

#include "RgbColor.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MDI_TS_FRAME
//#define DEBUG_RENAME
//#define DEBUG_REDRAW_CURVE

BEGIN_EVENT_TABLE(MdiChildTsFrame, GmatMdiChildFrame)
   EVT_MENU(GmatPlot::MDI_TS_OPEN_PLOT_FILE, MdiChildTsFrame::OnOpenXyPlotFile)
   EVT_MENU(GmatPlot::MDI_TS_CHILD_QUIT, MdiChildTsFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_TS_CHANGE_TITLE, MdiChildTsFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_TS_CLEAR_PLOT, MdiChildTsFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_TS_SHOW_DEFAULT_VIEW,
         MdiChildTsFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_TS_DRAW_GRID, MdiChildTsFrame::OnDrawGrid)
   EVT_MENU(GmatPlot::MDI_TS_DRAW_DOTTED_LINE,
         MdiChildTsFrame::OnDrawDottedLine)
   EVT_MENU(GmatPlot::MDI_TS_HELP_VIEW, MdiChildTsFrame::OnHelpView)

//   EVT_PLOT_CLICKED(-1, MdiChildTsFrame::OnPlotClick)
   EVT_ACTIVATE(MdiChildTsFrame::OnActivate)
   EVT_SIZE(MdiChildTsFrame::OnSize)
   EVT_MOVE(MdiChildTsFrame::OnMove)
   EVT_CLOSE(MdiChildTsFrame::OnPlotClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTsFrame(wxMDIParentFrame *parent, bool isMainFrame,
//                 const wxString &plotName, const wxString& plotTitle,
//                 const wxString& xAxisTitle, const wxString& yAxisTitle,
//                 const wxPoint& pos, const wxSize& size, const long style)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MdiChildTsFrame(wxMDIParentFrame *parent, bool isMainFrame,
//       const wxString &plotName, const wxString& plotTitle,
//       const wxString& xAxisTitle, const wxString& yAxisTitle,
//       const wxPoint& pos, const wxSize& size, const long style)
//------------------------------------------------------------------------------
/**
 * Constructor for the frame
 *
 * @param parent Window that owns this one
 * @param isMainFrame Flag indicating if this is the main frame
 * @param plotName Name of the plot
 * @param plotTitle Title of the plot
 * @param xAxisTitle X-axis label
 * @param yAxisTitle Y-axis label
 * @param pos Position for the frame on the screen
 * @param size Size of the frame
 * @param style Style used for drawing the frame
 */
//------------------------------------------------------------------------------
MdiChildTsFrame::MdiChildTsFrame(wxMDIParentFrame *parent, bool isMainFrame,
      const wxString &plotName, const wxString& plotTitle,
      const wxString& xAxisTitle, const wxString& yAxisTitle,
      const wxPoint& pos, const wxSize& size, const long style) :
   GmatMdiChildFrame(parent, plotName, plotTitle, GmatTree::OUTPUT_XY_PLOT, -1,
                       pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mXyPlot = (TsPlotCanvas *) NULL;
   mIsMainFrame = isMainFrame;
   mPlotTitle = plotTitle;
   mXAxisTitle = xAxisTitle;
   mYAxisTitle = yAxisTitle;

   for (int i=0; i<MAX_NUM_CURVE; i++)
      mHasFirstXSet[i] = false;
   
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::MdiChildTsFrame()\n   X Axis Title = '%s'\n"
       "   Y Axis Title = '%s'\n   isMainFrame = %d\n", xAxisTitle.c_str(),
       yAxisTitle.c_str(), isMainFrame);
   #endif
   
   MdiTsPlot::mdiChildren.Append(this);
    
   // Give it an icon
   #ifdef __WXMSW__
      SetIcon(wxIcon(_T("chrt_icn")));
   #else
      SetIcon(wxIcon( mondrian_xpm ));
   #endif

   // Create XyPlotFrame
   int width, height;
   GetClientSize(&width, &height);
   
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage("   Creating TsPlotCanvas\n");
   #endif
   
   TsPlotCanvas *frame =
      new TsPlotXYCanvas(this, -1, wxPoint(0, 0), wxSize(width, height),
                       wxTAB_TRAVERSAL,//wxPLOT_DEFAULT,
                       plotTitle);
   
   frame->SetLabel(xAxisTitle, TsPlotCanvas::X_LABEL);
   frame->SetLabel(yAxisTitle, TsPlotCanvas::Y_LABEL);
   
   mXyPlot = frame;
   
   wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
   
   topSizer->Add(mXyPlot, 1, wxALIGN_CENTER |wxEXPAND);
   
   SetAutoLayout( TRUE ); //loj: this is called implicitly by SetSizer()
   SetSizer( topSizer );
   
   // this should work for MDI frames as well as for normal ones
   SetSizeHints(100, 100);
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
   
   isActive = true;


   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage("MdiChildTsFrame::MdiChildTsFrame() "
         "leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~MdiChildTsFrame()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
MdiChildTsFrame::~MdiChildTsFrame()
{
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage
      ("~MdiChildTsFrame() mPlotName=%s\n", mPlotName.c_str());
   #endif

   // It needs to keepbokking the XYPlot frames list when a frame is removed.  // Fix bug GMT-7301
   MdiTsPlot::mdiChildren.DeleteObject(this);                                  // Fix bug GMT-7301
   MdiTsPlot::numChildren--;                                                   // Fix bug GMT-7301
   
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage("~MdiChildTsFrame() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// int ReadXyPlotFile(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Reads data for an XY plot and passes those data to the plot component
 *
 * @param filename The file that is to be read
 *
 * @return The number of data points read from the file
 */
//------------------------------------------------------------------------------
int MdiChildTsFrame::ReadXyPlotFile(const wxString &filename)
{
   std::ifstream inStream;  // input data stream
   double tempData[7]; //loj: time, x, y, z, vx, vy, vz for build 2
   int numData = 0;
   double startTime;
   
   if (filename != "")
   {
      // Changed to use WX_TO_STD_STRING macro for wx3.0 (LOJ: 2014.08.07)
      //inStream.open(filename.c_str());
      inStream.open(filename.WX_TO_C_STRING);
      if (inStream.is_open())
      {
         TsPlotCurve *xCurve = new TsPlotCurve();
         TsPlotCurve *yCurve = new TsPlotCurve();
         TsPlotCurve *zCurve = new TsPlotCurve();
         
         // read 1st line to get start time
         for (int i=0; i<7; i++)
            inStream >> tempData[i];
         
         startTime = tempData[0];
         
         // set time, X, Y, Z
         xCurve->AddData(0.0, tempData[1]); // time, X
         yCurve->AddData(0.0, tempData[2]); // time, Y
         zCurve->AddData(0.0, tempData[3]); // time, Z
         numData++;
         
         while(!inStream.eof())
         {
            // read time, X, Y, Z, Vx, Vy, Vz
            for (int i=0; i<7; i++)
               inStream >> tempData[i];
            
            // set time, X, Y, Z
            xCurve->AddData(tempData[0]-startTime, tempData[1]); // time, X
            yCurve->AddData(tempData[0]-startTime, tempData[2]); // time, Y
            zCurve->AddData(tempData[0]-startTime, tempData[3]); // time, Z
            
            numData++;
         }
      }
      
      Update();
   }
   
   return numData;
}

//------------------------------------------------------------------------------
// bool DeletePlot()
//------------------------------------------------------------------------------
/**
 * Prepares the plot frame for deletion
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool MdiChildTsFrame::DeletePlot()
{
   MessageInterface::ShowMessage("MdiChildTsFrame::DeletePlot()\n");
   Close(TRUE);

   return true;
}


//------------------------------------------------------------------------------
// void SetPlotTitle(const wxString &title)
//------------------------------------------------------------------------------
/**
 * Set the plot title
 *
 * @param title The plot title
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::SetPlotTitle(const wxString &title)
{
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage("MdiChildTsFrame::SetPlotTitle() title = %s\n",
                                 title.c_str());
   #endif
   
   mPlotTitle = title;
   
   if (mXyPlot)
      mXyPlot->SetLabel(title, TsPlotCanvas::PLOT_TITLE);
}


//------------------------------------------------------------------------------
// void ShowPlotLegend()
//------------------------------------------------------------------------------
/**
 * Turn on the plot legend
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ShowPlotLegend()
{
   #ifdef DEBUG_MDI_TS_FRAME
      MessageInterface::ShowMessage("MdiChildTsFrame::ShowLegend() entered\n");
   #endif
      
   if (mXyPlot)
      mXyPlot->ShowLegend();
}


//------------------------------------------------------------------------------
// void AddPlotCurve(Integer curveIndex, Integer const wxString &curveTitle,
//                   UnsignedInt penColor)
//------------------------------------------------------------------------------
/**
 * Adds a plot curve to XY plow window.
 *
 * @param curveIndex The index for the curve
 * @param curveTitle Label for the curve
 * @param penColor Default color for the curve
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::AddPlotCurve(Integer curveIndex, const wxString &curveTitle,
                                   UnsignedInt penColor)
{
   #ifdef DEBUG_MDI_TS_FRAME
      MessageInterface::ShowMessage
         ("MdiChildTsFrame::AddPlotCurve() curveIndex='%s', curveTitle='%s', penColor=%u\n",
          curveIndex, curveTitle.c_str(), penColor);
   #endif

   if (curveIndex > MAX_NUM_CURVE)
   {
      std::stringstream num;
      num << MAX_NUM_CURVE;
      throw SubscriberException("XY Plots are limited to " + num.str() +
            " curves in GMAT");
   }
   
   if (mXyPlot != NULL)
   {
      mHasFirstXSet[curveIndex] = false;
      
      // Create XyPlotCurve
      TsPlotCurve *curve = new TsPlotCurve();
      
      #ifdef DEBUG_MDI_TS_FRAME
         MessageInterface::ShowMessage(
            "MdiChildTsFrame::AddPlotCurve() curve title = %s\n",
            curveTitle.c_str());
      #endif
         
      mXyPlot->AddData(curve, penColor);
      mXyPlot->SetDataName(curveTitle);
      
      #ifdef DEBUG_MDI_TS_FRAME
         MessageInterface::ShowMessage
            ("MdiChildTsFrame::AddPlotCurve() curve count = %d added\n",
             mXyPlot->GetCurveCount());
      #endif
   }
   else
   {
      MessageInterface::ShowMessage("MdiChildTsFrame::AddPlotCurve() mXyPlot "
            "is NULL... \n");
   }
}


//------------------------------------------------------------------------------
// void DeleteAllPlotCurves()
//------------------------------------------------------------------------------
/**
 * Deletes all plot curves in the plow window.
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::DeleteAllPlotCurves()
{
   if (mXyPlot != NULL)
   {
      #ifdef DEBUG_MDI_TS_FRAME
      MessageInterface::ShowMessage
         ("MdiChildTsFrame::DeleteAllPlotCurve() curve count=%d \n",
          mXyPlot->GetCurveCount());
      #endif
      
      while (mXyPlot->GetCurveCount() > 0)
         DeletePlotCurve(0);
   }
   else
   {
      MessageInterface::ShowMessage("MdiChildTsFrame::DeletePlotCurve() "
            "mXyPlot is NULL... \n");
   }
}

//------------------------------------------------------------------------------
// void DeletePlotCurve(Integer curveIndex)
//------------------------------------------------------------------------------
/**
 * Deletes a plot curve to XY plow window.
 *
 * @param curveIndex Index of the curve that is to be deleted
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::DeletePlotCurve(Integer curveIndex)
{
   #ifdef DEBUG_MDI_TS_FRAME
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::DeletePlotCurve() curveIndex = %d\n", curveIndex);
   #endif
   
   if (mXyPlot != NULL)
   {
      TsPlotCurve* curve = mXyPlot->GetPlotCurve(curveIndex);
      if (curve != NULL)
         mXyPlot->DeletePlotCurve(curveIndex);
      mHasFirstXSet[curveIndex] = false;
   }
   else
   {
      MessageInterface::ShowMessage("MdiChildTsFrame::DeletePlotCurve() "
            "mXyPlot is NULL... \n");
   }
}

//------------------------------------------------------------------------------
// void AddDataPoints(Integer curveIndex, Real xData, Real yData, Real hi,
//       Real lo)
//------------------------------------------------------------------------------
/*
 * Updates XY plot curve.
 *
 * @param curveIndex curve number
 * @param xData x value to be updated
 * @param yData y value to be updated
 * @param hi +sigma value used for error bars
 * @param lo -sigma valuused for error bars
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::AddDataPoints(Integer curveIndex, Real xData, Real yData,
      Real hi, Real lo)
{
   #ifdef DEBUG_POINT_ADD
      MessageInterface::ShowMessage(
         "MdiChildTsFrame::AddDataPoints(%d, %lf, %lf )\n", curveIndex, xData, 
         yData);
   #endif
   
   if (mXyPlot)
   {
      TsPlotCurve *curve = mXyPlot->GetPlotCurve(curveIndex);
      if (curve)
         curve->AddData(xData, yData, hi, lo);
   } 
}

//------------------------------------------------------------------------------
// void ClearPlotData()
//------------------------------------------------------------------------------
/*
 * Clears XY plot data.
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ClearPlotData()
{
   if (mXyPlot)
   {
      mXyPlot->ClearAllCurveData();
   }
}


//------------------------------------------------------------------------------
// void PenUp()
//------------------------------------------------------------------------------
/*
 * Temporarily stops drawing to the plot.
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::PenUp()
{
   if (mXyPlot)
   {
      mXyPlot->PenUp();
   }
}


//------------------------------------------------------------------------------
// void PenDown()
//------------------------------------------------------------------------------
/*
 * Resumes drawing to the plot.
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::PenDown()
{
   if (mXyPlot)
   {
      mXyPlot->PenDown();
   }
}

//------------------------------------------------------------------------------
// void Darken(int factor, int index, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Darkens a curve or plot by a user specified factor
 *
 * @param factor The darkening factor
 * @param index Index of the starting point
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::Darken(int factor, int index, int forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->Darken(factor, index, forCurve);
   }
}

//------------------------------------------------------------------------------
// void Lighten(int factor, int index, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Lightens a curve or plot by a user specified factor
 *
 * @param factor The darkening factor
 * @param index Index of the starting point
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::Lighten(int factor, int index, int forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->Lighten(factor, index, forCurve);
   }
}

//------------------------------------------------------------------------------
// void MarkPoint(Integer index, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Marks a specific point on a specific curve with an X
 *
 * @param index Index for the point
 * @param forCurve Index of the curve containing the point
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::MarkPoint(Integer index, Integer forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->MarkPoint(index, forCurve);
   }
}

//------------------------------------------------------------------------------
// void MarkBreak(int index, int forCurve)
//------------------------------------------------------------------------------
/**
 * Sets a marker for a possible break and discard point on a curve or plot
 *
 * @param index Index of the point where the break occurs
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::MarkBreak(int index, int forCurve)
{
   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("MdiChildTsFrame::MarkBreak(%d, %d) called "
            "for %s\n", index, forCurve, mPlotName.c_str());
   #endif
   if (mXyPlot)
   {
      mXyPlot->MarkBreak(index, forCurve);
   }
}

//------------------------------------------------------------------------------
// void ClearFromBreak(int startBreakNumber, int endBreakNumber, int forCurve)
//------------------------------------------------------------------------------
/**
 * Discard data on a curve or plot between 2 break points
 *
 * @param startBreakNumber Index of the point where the break occurs
 * @param endBreakNumber Index of the end of the break, or -1 to discard the
 *                       rest of the curve data
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ClearFromBreak(int startBreakNumber, int endBreakNumber,
      int forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->ClearFromBreak(startBreakNumber, endBreakNumber, forCurve);
   }
}

//------------------------------------------------------------------------------
// void ChangeColor(Integer index, unsigned long newColor, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Changes the color of a curve at the specified point
 *
 * @param index Index of the point
 * @param newColor The new curve color
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ChangeColor(Integer index, unsigned long newColor,
      Integer forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->ChangeColor(index, newColor, forCurve);
   }
}

//------------------------------------------------------------------------------
// void ChangeMarker(Integer index, Integer newMarker, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Changes the marker of a curve at the specified point
 *
 * @param index Index of the point
 * @param newMarker The new curve marker
 * @param forCurve Index of the curve that received the color change
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ChangeMarker(Integer index, Integer newMarker,
      Integer forCurve)
{
   if (mXyPlot)
   {
      mXyPlot->ChangeMarker(index, newMarker, forCurve);
   }
}


//------------------------------------------------------------------------------
// void SetLineWidth(int w, int lineId)
//------------------------------------------------------------------------------
/**
 * Sets the line width for a curve or plot
 *
 * @param w The new width
 * @param lineId The ID of the curve (-1 for all)
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::SetLineWidth(int w, int lineId)
{
   if (mXyPlot)
   {
      mXyPlot->SetLineWidth(w, lineId);
   }
}


//------------------------------------------------------------------------------
// void SetLineStyle(int ls, int lineId)
//------------------------------------------------------------------------------
/**
 * Sets the line style for a curve or plot
 *
 * @param ls The new line style
 * @param lineId The ID of the curve (-1 for all)
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::SetLineStyle(int ls, int lineId)
{
   if (mXyPlot)
   {
      mXyPlot->SetLineStyle(ls, lineId);
   }
}


//------------------------------------------------------------------------------
// void Rescale()
//------------------------------------------------------------------------------
/**
 * Causes the plot to perform a rescale so all of the data can be displayed
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::Rescale()
{
   if (mXyPlot)
   {
      mXyPlot->Rescale();
   }
}


//------------------------------------------------------------------------------
// bool MdiChildTsFrame::IsActive()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating if the plot is active (meaning that the plot is
 * receiving data and updating as the data comes in.
 *
 * @return true if the plot is active, false if not
 */
//------------------------------------------------------------------------------
bool MdiChildTsFrame::IsActive()
{
   return isActive;
}


//------------------------------------------------------------------------------
// void MdiChildTsFrame::IsActive(bool yesno)
//------------------------------------------------------------------------------
/**
 * Used to activate and deactivate the plot
 *
 * @param yesno Flag setting (true) or clearing (false) the active state of the
 *              plot
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::IsActive(bool yesno)
{
   isActive = yesno;
}


//------------------------------------------------------------------------------
// void CurveSettings(bool useLines, Integer lineWidth, Integer lineStyle,
//       bool useMarkers, Integer markerSize, Integer marker, bool useHiLow,
//       Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Sets the default settings for a curve
 *
 * @param useLines Flag that is set if the curve should have lines connecting
 *                 the curve points
 * @param lineWidth The width, in pixels, of all drawn lines
 * @param lineStyle The style of the lines
 * @param useMarkers Flag used to toggle on markers at each point on the curve
 * @param markerSize The size of the marker
 * @param marker The marker to be used
 * @param useHiLow Flag used to turn error bars on
 * @param forCurve The index of the curve receiving the settings
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::CurveSettings(bool useLines, Integer lineWidth,
      Integer lineStyle, bool useMarkers, Integer markerSize, Integer marker,
      bool useHiLow, Integer forCurve)
{
   if (forCurve < -1)
      return;

   if (mXyPlot)
   {
      Integer count = mXyPlot->GetCurveCount();

      if (forCurve == -1)
      {
         for (Integer i = 0; i < count; ++i)
         {
            TsPlotCurve *curve = mXyPlot->GetPlotCurve(i);
            curve->UseLine(useLines);
            curve->SetWidth(lineWidth);
            curve->SetStyle(lineStyle);
            curve->UseMarker(useMarkers);
            curve->SetMarkerSize(markerSize);
            if (marker == -1)
               curve->SetMarker((MarkerType)(i % MarkerCount), 0);
            else
               curve->SetMarker((MarkerType)marker, 0);
            curve->UseHiLow(useHiLow);
         }
      }
      else
         if (forCurve < count)
         {
            TsPlotCurve *curve = mXyPlot->GetPlotCurve(forCurve);
            curve->UseLine(useLines);
            curve->SetWidth(lineWidth);
            curve->SetStyle(lineStyle);
            curve->UseMarker(useMarkers);
            curve->SetMarkerSize(markerSize);
            if (marker == -1)
               curve->SetMarker((MarkerType)(forCurve % MarkerCount), 0);
            else
               curve->SetMarker((MarkerType)marker, 0);
            curve->UseHiLow(useHiLow);
         }
   }
}

//------------------------------------------------------------------------------
// void RedrawCurve()
//------------------------------------------------------------------------------
/*
 * Redraws the XY plot.
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::RedrawCurve()
{
   #ifdef DEBUG_REDRAW_CURVE
   MessageInterface::ShowMessage("==========> MdiChildTsFrame::RedrawCurve() entered\n");
   #endif
   if (mXyPlot)
   {
//      Refresh();
      Update(); // need Update to show plot as it runs

      mXyPlot->DataUpdate(true);
//      wxPaintEvent pvt;
//      mXyPlot->OnPaint(pvt);
      wxClientDC dc(mXyPlot);
      PrepareDC(dc);
      mXyPlot->Refresh(dc, true);
      
      // Turning this OFF for Mac for now; this modification makes the XY Plot
      // go grey at the end of the run
      #ifndef __WXMAC__
      // Why OnPaint() is called twice with wx3?
      // Set the data update flag to true so that OnPaint() will not redraw plots
      // (LOJ: 2014.10.10 To improve performance with wx3. See GMT-4722)
      #if wxCHECK_VERSION(3, 0, 0)
      mXyPlot->DataUpdate(true);
      #endif
      #endif
      
      #ifdef DEBUG_REDRAW_CURVE
      MessageInterface::ShowMessage("MdiChildTsFrame::RedrawCurve() calling mXyPlot->Update()\n");
      #endif
      
      mXyPlot->Update();
   }
   #ifdef DEBUG_REDRAW_CURVE
   MessageInterface::ShowMessage("==========> MdiChildTsFrame::RedrawCurve() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildTsFrame::SetPlotName(const wxString &name)
{
   #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("MdiChildTsFrame::SetPlotName() name=%s\n",
                                    name.c_str());
   #endif
   GmatMdiChildFrame::SetPlotName(name);
   SetTitle(name);
}

//------------------------------------------------------------------------------
// void SetShowGrid(bool show)
//------------------------------------------------------------------------------
/*
 * Sets show grid menu option
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::SetShowGrid(bool show)
{
   if (mXyPlot)
   {
      theMenuBar->Check(GmatPlot::MDI_TS_DRAW_GRID, show);
      mXyPlot->ShowGrid(show);
   }
}

//------------------------------------------------------------------------------
// void ResetZoom()
//------------------------------------------------------------------------------
/*
 * Resets plot zoom value
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::ResetZoom()
{
   if (mXyPlot)
   {
//      mXyPlot->ResetZoom();
   }
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}

//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   if (mXyPlot)
   {
      //static wxString s_title = _T("Plot Frame");
      wxString oldTitle = _T("Fred"); // mXyPlot->GetPlotTitle();

      wxString newTitle = wxGetTextFromUser(_T("Enter the new title"),
                                         _T(""),
                                         oldTitle,
                                         GetParent()->GetParent());
      if ( !newTitle )
         return;
      
      mXyPlot->SetLabel(newTitle, TsPlotCanvas::PLOT_TITLE);
   }
}

//------------------------------------------------------------------------------
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
//   if (mXyPlot)
//      mXyPlot->ClearPlot();
}

//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnShowDefaultView(wxCommandEvent& event)
{
//   if (mXyPlot)
//      mXyPlot->ShowDefaultView();
}

//------------------------------------------------------------------------------
// void OnDrawGrid(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnDrawGrid(wxCommandEvent& event)
{
   if (mXyPlot)
   {
      mXyPlot->ShowGrid(event.IsChecked());
   }
}

//------------------------------------------------------------------------------
// void OnDrawDottedLine(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnDrawDottedLine(wxCommandEvent& event)
{
//   if (mXyPlot)
//      mXyPlot->DrawDottedLine(event.IsChecked());
}

//------------------------------------------------------------------------------
// void OnHelpView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnHelpView(wxCommandEvent& event)
{
}

////------------------------------------------------------------------------------
//// void OnPlotClick(wxPlotEvent &event)
////------------------------------------------------------------------------------
//void MdiChildTsFrame::OnPlotClick(wxPlotEvent &event)
//{
//   if (mXyPlot)
//   {
//      XyPlotCurve *curve = (XyPlotCurve*)event.GetCurve();
//      double x = (event.GetPosition() * mXyPlot->GetUnitsPerValue()) +
//         curve->GetFirstX();
//      double y = event.GetCurve()->GetY( event.GetPosition() );
//        
//      //MessageInterface::ShowMessage("MdiChildTsFrame::OnPlotClick() xpos = %d  "
//      //                              "firstx = %f  unitspervalue = %g\n", event.GetPosition(),
//      //                              curve->GetFirstX(), mXyPlot->GetUnitsPerValue());
//        
//      wxString info;
//      //info.Printf("%s: %5.3f  %s: %f\n", GetXAxisTitle().c_str(), x,
//      //            curve->GetCurveTitle().c_str(), y);
//      info.Printf("%s: %5.3f  %s: %g\n", GetXAxisTitle().c_str(), x, //loj: 12/10/04 changed to %g
//                  curve->GetCurveTitle().c_str(), y);
//        
//      //loj: 2/26/04 changed to wxLogStatus
//      wxLogStatus(GmatAppData::Instance()->GetMainFrame(), info);
//        
//      //mLogTextCtrl->AppendText(info);
//        
//      //wxLogMessage(GetXAxisTitle() + ": %5.3f, " +
//      //             curve->GetCurveTitle() + ": %f", x, y);
//   }
//}

//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnActivate(wxActivateEvent& event)
{
   if ( event.GetActive() && mXyPlot )
   {
      mXyPlot->SetFocus();
   }
   
   GmatMdiChildFrame::OnActivate(event);
}

//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnMove(wxMoveEvent& event)
{
   // VZ: here everything is totally wrong under MSW, the positions are
   //     different and both wrong (pos2 is off by 2 pixels for me which seems
   //     to be the width of the MDI canvas border)
    
   //      wxPoint pos1 = event.GetPosition(),
   //              pos2 = GetPosition();
   //      wxLogStatus(MdiXyPlot::MdiChildTsFrame,
   //                  wxT("position from event: (%d, %d), from frame (%d, %d)"),
   //                  pos1.x, pos1.y, pos2.x, pos2.y);

   event.Skip();
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
    
   //      wxSize size1 = event.GetSize(),
   //             size2 = GetSize(),
   //             size3 = GetClientSize();
   //      wxLogStatus(MdiXyPlot::MdiChildTsFrame,
   //                  wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
   //                  size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);

   event.Skip();
}


//------------------------------------------------------------------------------
// void OnPlotClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnPlotClose(wxCloseEvent& event)
{
   #ifdef DEBUG_PLOT_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::OnPlotClose() mPlotName='%s' entered\n", mPlotName.c_str());
   #endif
   
   // Add any check before closing
   
   // remove from list of frames but do not delete
   if (GmatAppData::Instance()->GetMainFrame()->RemoveChild(GetName(), mItemType, false))
   {
      event.Skip();
   }
   else
   {
      event.Veto();
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "**** Internal error occurred, Please close from the ToolBar");
   }
   
   #ifdef DEBUG_PLOT_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::OnPlotClose() mPlotName='%s' exiting\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnClose(wxCloseEvent& event)
{
   // Add any check before closing
   
   #ifdef DEBUG_PLOT_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::OnClose() GetPlotName() ='%s' entered\n", GetPlotName().c_str().AsChar());
   #endif
   
   GmatMdiChildFrame::OnClose(event);
   event.Skip();
   
   /// After running OnClose(). wx will delete this object.     // Fix bug GMT-7301
   /// In the distructor of this object, it has this code.      // Fix bug GMT-7301
   /// Therefore, this section has to be removed.               // Fix bug GMT-7301
   // MdiTsPlot::mdiChildren.DeleteObject(this);                // Fix bug GMT-7301
   // MdiTsPlot::numChildren--;                                 // Fix bug GMT-7301
   
   #ifdef DEBUG_PLOT_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildTsFrame::OnClose() GetPlotName() ='%s' exiting\n", GetPlotName().c_str().AsChar());
   #endif
}


void MdiChildTsFrame::TakeAction(const std::string &action)
{
   if (action == "AlwaysRedraw")
   {
      mXyPlot->AlwaysDraw(true);
   }
   if (action == "RunModeRedraw")
   {
      mXyPlot->AlwaysDraw(false);
   }
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void AdjustYScale()
//------------------------------------------------------------------------------
/*
 * Automatically adjusts y scale to y minimum and maximum value
 *
 * @note This method currently does nothing
 */
//------------------------------------------------------------------------------
void MdiChildTsFrame::AdjustYScale()
{
   using namespace GmatMathUtil;
   
//   double ymin = GetYMin();
//   double ymax = GetYMax();
//   double yMaxScale = Max(Abs(ymin), Abs(ymax));
//   double yMargin = yMaxScale * 0.1;
   
// #ifdef DEBUG_MDI_TS_FRAME
//    MessageInterface::ShowMessage
//       ("MdiChildTsFrame::AdjustYScale() ymin=%f ymax=%f yMaxScale=%f yMargin=%f\n",
//        ymin, ymax, yMaxScale, yMargin);
// #endif

//   for (unsigned int i=0; i<mXyPlot->GetCount(); i++)
//   {
//      mXyPlot->GetAt(i)->SetStartY(-yMaxScale - yMargin);
//      mXyPlot->GetAt(i)->SetEndY(yMaxScale + yMargin);
//   }
}

//------------------------------------------------------------------------------
// double GetYMin()
//------------------------------------------------------------------------------
/*
 * Returns minimum y value of all curves
 */
//------------------------------------------------------------------------------
double MdiChildTsFrame::GetYMin()
{
   double minVal = -123456789.0; //loj: return some other value?
   
   if (mXyPlot)
   {
      std::vector<double> yMinVals;
      std::vector<double>::iterator pos;
   
      #ifdef DEBUG_MDI_TS_FRAME
         MessageInterface::ShowMessage
            ("MdiChildTsFrame::GetYMin() yMinVals.size()=%d\n",
             yMinVals.size());;
      #endif

      if (yMinVals.size() == 1)
      {
         minVal = yMinVals[0];
      }
      else if (yMinVals.size() >= 2)
      {
         pos = min_element(yMinVals.begin(), yMinVals.end());
         minVal = *pos;
      }
   }

   return minVal;
}

//------------------------------------------------------------------------------
// double GetYMax()
//------------------------------------------------------------------------------
/*
 * Returns minimum y value of all curves
 */
//------------------------------------------------------------------------------
double MdiChildTsFrame::GetYMax()
{
   double maxVal = 123456789.0; //loj: return some other value?
   
   if (mXyPlot)
   {
      std::vector<double> yMaxVals;
      std::vector<double>::iterator pos;
      
      #ifdef DEBUG_MDI_TS_FRAME
         MessageInterface::ShowMessage
            ("MdiChildTsFrame::GetYMax() yMaxVals.size()=%d\n",
            yMaxVals.size());;
      #endif
      
      if (yMaxVals.size() == 1)
      {
         maxVal = yMaxVals[0];
      }
      else if (yMaxVals.size() >= 2)
      {
         pos = max_element(yMaxVals.begin(), yMaxVals.end());
         maxVal = *pos;
      }
   }

   return maxVal;
}

//------------------------------------------------------------------------------
// void OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void MdiChildTsFrame::OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
{
   wxFileDialog fileDialog(this, _T("Open Text XY Plot File"),
                           _T(""), _T(""), _T("text XY Plot file (*.txt)|*.txt"));
   
   fileDialog.SetDirectory(wxGetCwd());
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
   if (fileDialog.ShowModal() == wxID_OK)
   {
      wxString xyPlotFileName = fileDialog.GetPath();
      
      ++MdiTsPlot::numChildren;
      gmatAppData->GetMainFrame()->tsSubframe->SetPlotName(
                        "XYPlotFile" + std::to_string(MdiTsPlot::numChildren));
      gmatAppData->GetMainFrame()->tsSubframe->SetTitle(xyPlotFileName);
      
      //-----------------------------------
      // Read text XY Plot file
      //-----------------------------------
      int dataPoints =
         gmatAppData->GetMainFrame()->tsSubframe->ReadXyPlotFile(xyPlotFileName);
      
      if (dataPoints > 0)
      {
         gmatAppData->GetMainFrame()->tsSubframe->Show(TRUE);
         wxLogStatus(gmatAppData->GetMainFrame(),
                     wxT("Number of lines read: %d"), dataPoints);
      }
   }
}
