// $Id$
//------------------------------------------------------------------------------
//                              TsPlotCanvas
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotCanvas window.
 */
//------------------------------------------------------------------------------

#include "TsPlotCanvas.hpp"
#include "MessageInterface.hpp"
#include "TsPlotOptionsDialog.hpp"

#include <wx/dcmemory.h>
#include <wx/dcprint.h>
#include <wx/bitmap.h>

#include <cmath>
#include <fstream>
#include <algorithm>


// Define the static data
int  TsPlotCanvas::xSensitivity = 5;
int  TsPlotCanvas::ySensitivity = 5;
int  TsPlotCanvas::defaultLineWidth = 1;
bool TsPlotCanvas::defaultLabels = false;

//#define DEBUG_TS_CANVAS 1
//#define DEBUG_INTERFACE 1


BEGIN_EVENT_TABLE(TsPlotCanvas, wxWindow)
   EVT_PAINT         (TsPlotCanvas::OnPaint)
//   EVT_SET_FOCUS     (TsPlotCanvas::OnRefresh)
   EVT_SIZE          (TsPlotCanvas::OnSize)
   EVT_MOUSE_EVENTS  (TsPlotCanvas::OnMouseEvent)
   EVT_MENU          (ID_TOGGLE_GRID, TsPlotCanvas::ToggleGrid)
   EVT_MENU          (ID_TOGGLE_LEGEND, TsPlotCanvas::ToggleLegend)
   EVT_MENU          (ID_PLOT_DETAILS, TsPlotCanvas::SetOptions)
   EVT_MENU          (ID_PLOT_SAVE, TsPlotCanvas::SaveData)
   EVT_MENU          (ID_PLOT_SAVEIMAGE, TsPlotCanvas::SaveImage)
END_EVENT_TABLE()


// Define a constructor for my canvas
TsPlotCanvas::TsPlotCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                           const wxSize& size, long style,
                           const wxString& name) :
   wxWindow       (parent, -1, pos, size, style),
   runState       (10000),
   left           (80),
   right          (30),
   top            (20),
   bottom         (50),
   titleSize      (10),
   labelSize      (10),
   legendSize     (8),
   axisLabelSize  (12),
   plotPens       (NULL),
   plotDependent  (NULL),
   //xDataName      ("X Data"),
   filename       ("PlotData.txt"),
   plotTitle      (name),
   xLabel         (""),
   yLabel         (""),
   plotXMin       (1e99),
   plotXMax       (-1e99),
   plotYMin       (1e99),
   plotYMax       (-1e99),
   xticks         (6),
   yticks         (4),
   xMinorTicks    (5),
   yMinorTicks    (5),
   tickSize       (10),
   minorTickSize  (6),
   xMin           (1e99),
   xMax           (-1e99),
   yMin           (1e99),
   yMax           (-1e99),
   xName          ("X"),
   yName          ("Y"),
   hasData        (false),
   rescaled       (true),
   zoomed         (false),
   zooming        (false),
   movingLegend   (false),
   dataUpdated    (false),
   overrideXMin   (false),
   overrideXMax   (false),
   overrideYMin   (false),
   overrideYMax   (false),
//   automaticXMin  (true),
//   automaticXMax  (true),
//   automaticYMin  (true),
//   automaticYMax  (true),
   userXMin       (0.0),
   userXMax       (8600.0),
   userYMin       (-4000.0),
   userYMax       (7000.0),
   showsigmaLine  (true),
   show1sigmaTick (true),
   show2sigmaTick (true),
   show3sigmaTick (true),
   showTitle      (false),
   labelAxes      (false),
   hasGrid        (true),
   hasLegend      (true),
   allowPlotOptions(false),			// Change to true to show options dialog
   initializeLegendLoc (true),
   alwaysDraw     (false),
   xLabelPrecision(8),
   yLabelPrecision(6),
   plotArea(),  // Fixed unitialized value error
   mouseRect(),
   legendRect(),
   legendColumns  (1),
   resized        (false),
   drawAllCounter (0)
{
   wxPaintDC dc(this);

   labelFont = dc.GetFont();
   labelFont.SetPointSize(labelSize);

   titleFont = *wxSWISS_FONT; //dc.GetFont();
   titleFont.SetPointSize(titleSize);
   titleFont.SetWeight(wxBOLD);

   axisFont = *wxSWISS_FONT;
   axisFont.SetPointSize(axisLabelSize);

#ifdef __WXMAC__
   backgnd = 0xcfcfcf;
#else
   backgnd = GetBackgroundColour();
#endif

   plotColor.Set(0xff, 0xff, 0xff);

   #ifdef __WXMAC__
      gridColor = 0x7f7f7f;
   #else
      // backgnd was too light; darken it
      // gridColor = backgnd;
      gridColor.Set(backgnd.Red()<<2, backgnd.Green()<<2, backgnd.Blue()<<2);
   #endif
   gridPen.SetStyle(wxDOT);
   gridPen.SetColour(gridColor);

   legendColor.Set(0xff, 0xff, 0xef);
   legendFont = *wxSMALL_FONT;
   legendFont.SetPointSize(legendSize);

   legendRect.x = left-5;
   legendRect.y = top-5;
}


//==============================================================================
// wx Message Handlers
//==============================================================================


void TsPlotCanvas::OnPaint(wxPaintEvent& ev)
{
   #ifdef DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("==> TsPlotCanvas::OnPaint() entered, dataUpdated = %d\n", dataUpdated);
   #endif
   
   // On linux, this line floods the processor with messages.  So for
   // platforms that are not using GTK, refresh here
   #ifndef __WXGTK__
      wxWindow::Refresh(false);
   #endif
   bool ownedPlotCanvas = true;

   wxRegionIterator upd(GetUpdateRegion());
   if (upd && dataUpdated == false)
   {
      #ifdef DEBUG_REGIONUPDATES
         MessageInterface::ShowMessage("Update Regions:\n");
      #endif
      ++upd;
      if (upd)
         resized = true;
   }

   // Always refresh all in idle state (10000)
   if (runState == 10000)
      resized = true;

   wxPaintDC dc(this);
   wxCoord w, h;
   dc.GetSize(&w, &h);

   if (drawAllCounter > 0)
   {
      resized = true;
      --drawAllCounter;
   }

   // If the legend is turned on, be sure it can be seen
   if (legendRect.x > w)
      legendRect.x = w - 5;
   if (legendRect.y > h)
      legendRect.y = h - 5;
   bool drawAll = (resized ? true : false);
   
   // wxRegionIterator is not used here so commented out (LOJ: 2014.10.10)
   //wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
   #ifdef __WXMAC__
   if (!dataUpdated)
      drawAll = true;
   #endif
   if (alwaysDraw)
      drawAll = true;
   
   if (w > 0 && h > 0)
      Refresh(dc, drawAll);
   
   #ifdef DEBUG_TS_CANVAS
   MessageInterface::ShowMessage("==> TsPlotCanvas::OnPaint() leaving\n");
   #endif
}


void TsPlotCanvas::OnSize(wxSizeEvent& ev)
{
   resized = true;
   initializeLegendLoc = true;

   wxWindow::Refresh(true);
}


//------------------------------------------------------------------------------
// void TsPlotCanvas::OnMouseEvent(wxMouseEvent& event)
//------------------------------------------------------------------------------
/**
 * Code that responds to mouse events
 *
 * @param event The mouse event
 */
//------------------------------------------------------------------------------
void TsPlotCanvas::OnMouseEvent(wxMouseEvent& event)
{
   bool changed = false;

   wxClientDC dc(this);
   PrepareDC(dc);

   wxCoord w, h;
   dc.GetSize(&w, &h);

   static int oldX, oldY;

   wxPoint pt(event.GetLogicalPosition(dc));

   // Provide options to export data and control the grid and legend
   if (event.RightDown())
   {
      wxMenu menu;
      menu.AppendCheckItem(ID_TOGGLE_GRID, "Toggle Grid");
      menu.AppendCheckItem(ID_TOGGLE_LEGEND, "Toggle Legend");
      menu.AppendSeparator();
      if (allowPlotOptions)
      {
         menu.Append(ID_PLOT_DETAILS, "Set Plot Options...");
         menu.AppendSeparator();
      }
      menu.Append(ID_PLOT_SAVE, "Export Data...");
      menu.Append(ID_PLOT_SAVEIMAGE, "Save Image...");

      menu.Check(ID_TOGGLE_GRID, hasGrid);
      menu.Check(ID_TOGGLE_LEGEND, hasLegend);

      #ifdef __WXMSW__
         // Windows seems to need a location here.
         PopupMenu(&menu, pt);
      #else
         PopupMenu(&menu);
      #endif
   }
   // Double click removes zoom setting
   else if (event.LeftDClick())
   {
      changed = true;
      UnZoom();
   }
   else
   {
      // Select starting point for region selection or legend moving
      if (event.LeftDown())
      {
         mouseRect.x = pt.x;
         mouseRect.y = pt.y;
         oldX = pt.x;
         oldY = pt.y;

         // First check to see if the user is dragging the legend
         if (hasLegend)
         {
            if (legendRect.wxRECT_CONTAINS(pt))
               movingLegend = true;
         }
         if (!movingLegend)
         {
            #ifdef DEBUG_INTERFACE
               MessageInterface::ShowMessage("Down at [%ld  %ld]\n",
                  mouseRect.x, mouseRect.y);
            #endif

            if (plotArea.wxRECT_CONTAINS(pt))
               zooming = true;
         }
      }
      // Region selection/legend motion
      if (event.Dragging())
      {
         #if wxCHECK_VERSION(3, 0, 0)
         wxRasterOperationMode logfun = dc.GetLogicalFunction();
         #else
         int logfun = dc.GetLogicalFunction();
         #endif
         
         dc.SetLogicalFunction(wxINVERT);
         if (zooming)
         {
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
            oldX = pt.x;
            oldY = pt.y;
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
         }
         else if (movingLegend)
         {
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, oldY);
            oldX = pt.x;
            oldY = pt.y;
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, oldY);
         }
         dc.SetLogicalFunction(logfun);
      }
      // Completion of motion on the plot
      if (event.LeftUp() && (movingLegend || zooming))
      {
         bool wentRight = true, wentDown = true;

         if (mouseRect.x <= pt.x)
            mouseRect.width = pt.x - mouseRect.x;
         else
         {
            mouseRect.width = mouseRect.x - pt.x;
            mouseRect.x = pt.x;
            wentRight = false;
         }
         if (mouseRect.y <= pt.y)
            mouseRect.height = pt.y - mouseRect.y;
         else
         {
            mouseRect.height = mouseRect.y - pt.y;
            mouseRect.y = pt.y;
            wentDown = false;
         }

         #ifdef DEBUG_INTERFACE
            MessageInterface::ShowMessage("Up at [%ld  %ld]; ", pt.x, pt.y);
            MessageInterface::ShowMessage("Rect is %ld by %ld\n",
               mouseRect.width, mouseRect.height);
            MessageInterface::ShowMessage("Rectangle properties: [%ld %ld], "
               "%ld by %ld\n", region.x, region.y, region.width, region.height);
         #endif

         if (movingLegend)
         {
            if (wentRight)
               legendRect.x += mouseRect.width;
            else
               legendRect.x -= mouseRect.width;

            if (wentDown)
               legendRect.y += mouseRect.height;
            else
               legendRect.y -= mouseRect.height;

            changed = true;
         }

         if (zooming)
         {
            // First clear the rubberband, in case no zoom is made
            #if wxCHECK_VERSION(3, 0, 0)
            wxRasterOperationMode logfun = dc.GetLogicalFunction();
            #else
            int logfun = dc.GetLogicalFunction();
            #endif
            dc.SetLogicalFunction(wxINVERT);
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
            dc.SetLogicalFunction(logfun);

            if ((mouseRect.width > xSensitivity) &&
                (mouseRect.height > ySensitivity))
            {
               Zoom(mouseRect);
               changed = true;
            }
         }
      }
   }

   if (event.LeftUp() && (movingLegend || zooming))
   {
      zooming = false;
      movingLegend = false;
      wxWindow::Refresh(true);
   }

   if (changed)
   {
      // Tell curves to draw everything
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
         (*i)->lastPointPlotted = 0;

      Refresh(dc, true);
   }
}


//==============================================================================
// Core plotting methods
//==============================================================================

//------------------------------------------------------------------------------
// void TsPlotCanvas::Update()
//------------------------------------------------------------------------------
/**
 * Update the invalidated window regions
 *
 * Override of the base wxWindow method to allow region invalidation before
 * updating.
 *
 * @note: If the plot scaling has changed, this method refreshes the entire
 * window
 */
//------------------------------------------------------------------------------
void TsPlotCanvas::Update()
{
   bool refreshNeeded = false;

   for (std::vector<TsPlotCurve*>::iterator i = data.begin(); i != data.end(); ++i)
   {
      if ((*i)->RegionSizeChanged())
         if ((plotXMin > (*i)->GetMinX()) || (plotXMax < (*i)->GetMaxX()) ||
            (plotYMin > (*i)->GetMinY()) || (plotYMax < (*i)->GetMaxY()))
            refreshNeeded = true;
   }

   if (refreshNeeded || rescaled)
      wxWindow::Refresh(true);
   else
      wxWindow::Update();
}



void TsPlotCanvas::Refresh(wxDC &dc, bool drawAll)
{
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::Refresh() datasize=%d, drawAll=%d\n", data.size(), drawAll);
   #endif

   wxCoord w, h;
   dc.GetSize(&w, &h);

   // If the size is invalid, do nothing
   if (w <= 0 || h <= 0)
      return;

   if (showTitle)
      top = 30;
   else
      top = 20;
   
   Rescale(dc);
   
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::Refresh() rescaled = %d\n", rescaled);
   #endif
   
   // Set region colors
   if (rescaled || drawAll)
   {
      #ifdef __WXMAC__
         dc.SetBackground(wxBrush(backgnd, wxSOLID));
      #else
         dc.SetBackground(wxBrush(backgnd, wxTRANSPARENT));
      #endif
      dc.Clear();

      dc.SetClippingRegion(left, top, (w-(left+right)), (h-(top+bottom)));
      #ifdef __WXMAC__
         dc.SetBackground(wxBrush(plotColor, wxSOLID));
      #else
         dc.SetBackground(wxBrush(plotColor, wxTRANSPARENT));
      #endif
      dc.Clear();
      dc.DestroyClippingRegion();

      plotArea.x      = left;
      plotArea.y      = top;
      plotArea.width  = w - (left + right);
      plotArea.height = h - (top + bottom);
   }

   // Set curves to plot all data when needed
   if ((!rescaled && drawAll) || zoomed)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
         (*i)->lastPointPlotted = 0;

   // Now draw everything
   wxBEGIN_DRAWING

   if (rescaled || drawAll)
   {
      DrawAxes(dc);
      DrawLabels(dc);
   }

   if (hasGrid && (rescaled || drawAll))
      DrawGrid(dc);
   
   PlotData(dc);

   if (rescaled || drawAll)
   {
      if (hasLegend)
         DrawLegend(dc);
   }

   wxEND_DRAWING
   dataUpdated = false;
   resized = false;
   
//   wxWindow::Refresh();

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage("TsPlotCanvas::Refresh() leaving\n");
   #endif
}


void TsPlotCanvas::DrawMarker(wxDC &dc, int style, int markerSize, int x0,
      int y0, wxPen &thePen, int hi, int low)
{
   wxPen currentPen = dc.GetPen();
   dc.SetPen(thePen);

   // Manage int overflows
   int leftX = - markerSize - 1, rightX = 32000;
   int topY = - markerSize - 1, bottomY = 32000;

   if (x0 < leftX)
      x0 = leftX;
   if (x0 > rightX)
      x0 = rightX;

   if (y0 < topY)
      y0 = topY;
   if (y0 > bottomY)
      y0 = bottomY;

   switch (style)
   {
      case xMarker:
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize+1, y0+markerSize+1);
         dc.DrawLine(x0+markerSize, y0-markerSize,
               x0-markerSize-1, y0+markerSize+1);
         break;

      case circleMarker:
         dc.DrawCircle(x0, y0, markerSize);
         break;

      case plusMarker:
         dc.DrawLine(x0-markerSize, y0, x0+markerSize, y0);
         dc.DrawLine(x0, y0-markerSize, x0, y0+markerSize);
         break;

      case starMarker:
         dc.DrawLine(x0-markerSize, y0, x0+markerSize, y0);
         dc.DrawLine(x0, y0-markerSize, x0, y0+markerSize);
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0+markerSize);
         dc.DrawLine(x0-markerSize, y0+markerSize,
               x0+markerSize, y0-markerSize);
         break;

      case squareMarker:
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0-markerSize);
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0-markerSize, y0+markerSize);
         dc.DrawLine(x0+markerSize, y0-markerSize,
               x0+markerSize, y0+markerSize);
         dc.DrawLine(x0-markerSize, y0+markerSize,
               x0+markerSize, y0+markerSize);
         break;

      case diamondMarker:
         dc.DrawLine(x0-markerSize, y0, x0, y0-markerSize);
         dc.DrawLine(x0, y0-markerSize, x0+markerSize, y0);
         dc.DrawLine(x0+markerSize, y0, x0, y0+markerSize);
         dc.DrawLine(x0, y0+markerSize, x0-markerSize, y0);
         break;

      case xsquareMarker:
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0-markerSize);
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0+markerSize);
         dc.DrawLine(x0+markerSize, y0-markerSize,
               x0-markerSize, y0+markerSize);
         dc.DrawLine(x0-markerSize, y0+markerSize,
               x0+markerSize, y0+markerSize);
         break;

      case triangleMarker:
         dc.DrawLine(x0-markerSize, y0+markerSize,
               x0+markerSize, y0+markerSize);
         dc.DrawLine(x0-markerSize, y0+markerSize, x0,
               y0-markerSize);
         dc.DrawLine(x0+markerSize, y0+markerSize, x0,
               y0-markerSize);
         break;

      case nablaMarker:
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0-markerSize);
         dc.DrawLine(x0-markerSize, y0-markerSize, x0,
               y0+markerSize);
         dc.DrawLine(x0+markerSize, y0-markerSize, x0,
               y0+markerSize);
         break;

      case gunsightMarker:
         dc.DrawCircle(x0, y0, markerSize);
         dc.DrawLine(x0-markerSize, y0, x0+markerSize, y0);
         dc.DrawLine(x0, y0-markerSize, x0, y0+markerSize);
         break;

      case highlightMarker:
         { // Scoping
            wxPen highlightPen = dc.GetPen();
            highlightPen.SetWidth(highlightPen.GetWidth() + 1);
            dc.SetPen(highlightPen);
            dc.DrawLine(x0-markerSize-1, y0-markerSize-1, x0+markerSize+2, y0+markerSize+2);
            dc.DrawLine(x0-markerSize-1, y0+markerSize+1, x0+markerSize+2, y0-markerSize-2);
            dc.SetPen(thePen);
         }
         break;

      case hilowMarker:
         if (hi > 0)
         { // Scoping
            if (low < 0)
               low = hi;
            dc.DrawLine(x0, y0-hi, x0, y0+low);
            dc.DrawLine(x0-markerSize, y0-hi, x0+markerSize+1, y0-hi);
            dc.DrawLine(x0-markerSize, y0+low, x0+markerSize+1, y0+low);
         }
         break;

      case sigma13Marker:
         if (hi > 0)
         { // Scoping
            int yt, yb, y2t, y2b, y3t, y3b;

            if (low < 0)
               low = hi;

            // Manage int overflows
            yt  = y0 - hi;
            yb  = y0 + low;
            y2t = y0 - 2*hi;
            y2b = y0 + 2*low;
            y3t = y0 - 3*hi;
            y3b = y0 + 3*low;
            if (yt < topY)
               yt = topY;
            if (yb > bottomY)
               yb = bottomY;
            if (y2t < topY)
               y2t = topY;
            if (y2b > bottomY)
               y2b = bottomY;
            if (y3t < topY)
               y3t = topY;
            if (y3b > bottomY)
               y3b = bottomY;

            if (showsigmaLine)
            {
               dc.DrawLine(x0, yt, x0, y3t);
               dc.DrawLine(x0, yb, x0, y3b);
            }
            if (show1sigmaTick)
            {
               dc.DrawLine(x0-markerSize, yt, x0+markerSize+1, yt);
               dc.DrawLine(x0-markerSize, yb, x0+markerSize+1, yb);
            }
            if (show2sigmaTick)
            {
               dc.DrawLine(x0-markerSize, y2t, x0+markerSize+1, y2t);
               dc.DrawLine(x0-markerSize, y2b, x0+markerSize+1, y2b);
            }
            if (show3sigmaTick)
            {
               dc.DrawLine(x0-markerSize, y3t, x0+markerSize+1, y3t);
               dc.DrawLine(x0-markerSize, y3b, x0+markerSize+1, y3b);
            }
         }

      default: // Same as xMarker
         dc.DrawLine(x0-markerSize, y0-markerSize,
               x0+markerSize, y0+markerSize);
         dc.DrawLine(x0+markerSize, y0-markerSize,
               x0-markerSize, y0+markerSize);
         break;
   }

}

void TsPlotCanvas::DrawGrid(wxDC &dc)
{
   wxCoord w, h;
   dc.GetSize(&w, &h);

   int x0  = left,
       y0  = h-(bottom),
       xm  = w-right,
       ym  = top;

   dc.SetPen(gridPen);

//   if (xGridLoc.size() > 0)
//      for (unsigned int i = 0; i <= xGridLoc.size(); ++i)
   if (xticks > 0)
      for (int i = 0; i <= xticks; ++i)
         if ((xGridLoc[i] != x0) && (xGridLoc[i] != xm))
            dc.DrawLine(xGridLoc[i], y0 - tickSize, xGridLoc[i], ym + tickSize);
//   if (yGridLoc.size() > 0)
//      for (unsigned int i = 0; i <= yGridLoc.size(); ++i)
   if (yticks > 0)
      for (int i = 0; i <= yticks; ++i)
         if ((yGridLoc[i] != y0) && (yGridLoc[i] != ym))
            dc.DrawLine(xm - tickSize, yGridLoc[i], x0+tickSize, yGridLoc[i]);
}


void TsPlotCanvas::DrawLegend(wxDC &dc)
{
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::DrawLegend() names.size=%d\n", names.size());
   #endif

   int j = 0, h = 16, w = 16, labelCount = (int)names.size();
   wxString label;
   int xloc, yloc, rowCount = 1, colCount, markerReserve = 0;

   if (legendColumns > 0)
   {
      rowCount = (labelCount + legendColumns - 1) / legendColumns;
      colCount = legendColumns;
   }
   else
      colCount = labelCount;

   for (unsigned int j = 0; j < data.size(); ++j)
      if (data[j]->UseMarker())
      {
         markerReserve = 8;
         break;
      }

   wxColour textFore = dc.GetTextForeground();

   dc.SetFont(legendFont);

   // Find widest text extent
   int maxw = 0, maxh = 0;
   for (j = 0; j < labelCount; ++j)
   {
      dc.GetTextExtent(names[j].c_str(), &w, &h);
      if (w > maxw)
         maxw = w;
      if (h > maxh)
         maxh = h;
   }

   legendRect.height = (h+1)*rowCount + 8;
   legendRect.width = (maxw + markerReserve + 10) * colCount;

   if ((maxw > 0) && (initializeLegendLoc))
   {
      legendRect.x = plotArea.x + plotArea.width - legendRect.width + 5;
      legendRect.y = plotArea.y - 5;
      initializeLegendLoc = false;  // Turn off static legend location
   }

   dc.SetClippingRegion(legendRect);
   dc.SetBackground(wxBrush(legendColor, wxSOLID /*wxTRANSPARENT*/));
   dc.Clear();

   dc.SetPen(*wxBLACK_PEN);
   dc.DrawLine(legendRect.x + 1, legendRect.y + 1,
               legendRect.x + 1, legendRect.y + legendRect.height-2);
   dc.DrawLine(legendRect.x + 1, legendRect.y + 1,
               legendRect.x + legendRect.width - 2, legendRect.y + 1);
   dc.DrawLine(legendRect.x + legendRect.width - 2, legendRect.y + 1,
               legendRect.x + legendRect.width - 2,
               legendRect.y + legendRect.height - 2);
   dc.DrawLine(legendRect.x + 1, legendRect.y + legendRect.height - 2,
               legendRect.x + legendRect.width - 2,
               legendRect.y + legendRect.height - 2);

   for (j = 0; j < labelCount; ++j)
   {
      dc.SetTextForeground(data[j]->GetColour(0));
      label = names[j];
      xloc = legendRect.x + markerReserve + 6;
      yloc = legendRect.y + (h+1)*j + 4;
      dc.DrawText(label, xloc, yloc);

      if (data[j]->UseMarker())
      {
         int markerStyle = data[j]->GetMarker(0);
         wxPen mPen = plotPens[j];
         mPen.SetColour(data[j]->GetColour(0));
         DrawMarker(dc, markerStyle, 3, xloc - markerReserve + 2, yloc + maxh/2, mPen);
      }
   }

   dc.SetTextForeground(textFore);
   dc.DestroyClippingRegion();
}


//==============================================================================
// Data manipulation methods
//==============================================================================

void TsPlotCanvas::SetDataName(const wxString &dataName)
{
   #ifdef DEBUG_INTERFACE
   MessageInterface::ShowMessage("Adding data named %s\n", dataName.c_str());
   #endif
   
   names.push_back(dataName);
}


void TsPlotCanvas::SetLabel(const wxString &dataName,
                            const PlotComponents which)
{
   switch (which)
   {
      case PLOT_TITLE:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("Plot title is %s\n",
               dataName.c_str());
         #endif
         plotTitle = dataName;
         // yLabel = "";
         // xLabel = "";
         break;

      case X_LABEL:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("X label is %s\n", dataName.c_str());
         #endif
         xLabel = dataName;
         break;

      case Y_LABEL:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("Y label is %s\n", dataName.c_str());
         #endif
         yLabel = dataName;
         break;

      default:
         break;
   }
}


void TsPlotCanvas::AddData(TsPlotCurve *curve, wxColour startColor)
{
   data.push_back(curve);
   penUpLocations.push_back(curve->GetPenUpLocations());

   unsigned int varCount = (unsigned int)data.size();
   wxPen *newPens = new wxPen[varCount];

   for (unsigned int i = 0; i < varCount-1; ++i)
      newPens[i] = plotPens[i];

   // Defaults for the first 6 (if there are that many)
   int penID = varCount - 1;

   switch (penID)
   {
      case 0:     // Red
         newPens[0].SetColour(220, 0, 0);
         break;

      case 1:     // Green
         newPens[1].SetColour(0, 220, 0);
         break;

      case 2:     // Blue
         newPens[2].SetColour(0, 0, 220);
         break;

      case 3:     // Yellow
         newPens[3].SetColour(220, 220, 0);
         break;

      case 4:     // Fuchsia
         newPens[4].SetColour(220, 0, 220);
         break;

      case 5:     // Aqua
         newPens[5].SetColour(0, 220, 220);
         break;

      case 6:     // Maroon
         newPens[6].SetColour(110, 0, 0);
         break;

      case 7:     // Dk Green
         newPens[7].SetColour(0, 110, 0);
         break;

      case 8:     // Dk Blue
         newPens[8].SetColour(0, 0, 110);
         break;

      case 9:     // Gray
         newPens[9].SetColour(128, 128, 128);
         break;

      case 10:    // Brown
         newPens[10].SetColour(139, 88, 27);
         break;

      case 11:    // Dk Gray
         newPens[11].SetColour(64, 64, 64);
         break;

// More slots if needed
//      case 12:
//         newPens[12].SetColour(220, 0, 0);
//         break;
//
//      case 13:
//         newPens[13].SetColour(0, 220, 0);
//         break;
//
//      case 14:
//         newPens[14].SetColour(0, 0, 220);
//         break;
//
//      case 15:
//         newPens[15].SetColour(220, 220, 0);
//         break;
//
//      case 16:
//         newPens[16].SetColour(220, 0, 220);
//         break;
//
//      case 17:
//         newPens[17].SetColour(0, 220, 220);
//         break;
//
//      case 18:
//         newPens[18].SetColour(220, 0, 0);
//         break;
//
//      case 19:
//         newPens[19].SetColour(0, 220, 0);
//         break;
//
//      case 20:
//         newPens[20].SetColour(0, 0, 220);
//         break;
//
//      case 21:
//         newPens[21].SetColour(220, 220, 0);
//         break;
//
//      case 22:
//         newPens[22].SetColour(220, 0, 220);
//         break;
//
//      case 23:
//         newPens[].SetColour(0, 220, 220);
//         break;

      default:
         newPens[penID].SetColour(0, 0, 0);  // Black for the rest
   }

   newPens[penID].SetWidth(defaultLineWidth);

   if (plotPens)
      delete [] plotPens;

   if (startColor != *wxWHITE)
   {
      #ifdef ENABLE_AUTODARKEN
         // Check for a color match and if there, darken the new one
         for (UnsignedInt i = 0; i < varCount-1; ++i)
         {
            if (startColor == newPens[i].GetColour())
            {
               unsigned char r, g, b;
               r = startColor.Red() / 2;
               g = startColor.Green() / 2;
               b = startColor.Blue() / 2;


            }
         }
      #endif

      newPens[penID].SetColour(startColor);
   }

   plotPens = newPens;

   curve->SetColour(plotPens[penID].GetColour());
   if (curve->GetMarker() == unsetMarker)
      curve->SetMarker((MarkerType)(data.size() % MarkerCount));
}


void TsPlotCanvas::DataUpdate(bool tf)
{
   dataUpdated = tf;
}


TsPlotCurve* TsPlotCanvas::GetPlotCurve(int index)
{
   if ((index < (int)data.size()) && (index >= 0))
      return data[index];
   return NULL;
}

void TsPlotCanvas::DeletePlotCurve(int index)
{
   if ((index < (int)data.size()) && (index >= 0))
   {
      TsPlotCurve *curve = GetPlotCurve(index);
      std::vector<TsPlotCurve*>::iterator iter =
         find(data.begin(), data.end(), curve);
      data.erase(iter);
      delete curve;
      
      // We need to keep track of legend names associated with each curve
      std::vector<wxString>::iterator iter1 = find(names.begin(), names.end(), names[index]);
      names.erase(iter1);
   }

   // We need to keep track of legend names associated with each curve
   ////loj: 6/15/05
   //// We dont keep track of deleted names, so just clear
   //names.clear();

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::DeletePlotCurve() datasize=%d, index=%d\n",
       index, data.size());
   #endif
}

int TsPlotCanvas::GetCurveCount()
{
   return (int)data.size();
}


void TsPlotCanvas::DumpData(const std::string &fn)
{
   std::ofstream outfile;
   std::string outname = filename;
   if (fn != "")
      outname = fn;

   if (outname != "")
   {
      outfile.open(outname.c_str());
      outfile.precision(15);

      outfile << plotTitle << std::endl
              << xLabel << "   " << yLabel << std::endl;

      /// @todo Determine a better format to write out the plot data
      int curveNum = 0;
      for (std::vector<TsPlotCurve*>::iterator i = data.begin();
           i != data.end(); ++i)
      {
         int j;
         outfile << names[curveNum++] << "\n";
         for (j = 0; j < (int)((*i)->abscissa.size()); ++j)
            outfile << (*i)->abscissa[j] << ", " << (*i)->ordinate[j] << "\n";
         outfile << "\n";
      }
      outfile.close();
   }
}


//------------------------------------------------------------------------------
// void wxPlotWindow::ClearAllCurveData()
//------------------------------------------------------------------------------
void TsPlotCanvas::ClearAllCurveData()
{
   for (std::vector<TsPlotCurve*>::iterator i = data.begin();
        i != data.end(); ++i)
      (*i)->Clear();

   if (!userXMin)
      plotXMin =  1e99;
   if (!userXMax)
      plotXMax = -1e99;
   if (!userYMin)
      plotYMin =  1e99;
   if (!userYMax)
      plotYMax = -1e99;
   xMin     =  1e99;
   xMax     = -1e99;
   yMin     =  1e99;
   yMax     = -1e99;

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage(
      "TsPlotCanvas::ClearAllCurveData() clearing dc\n");
   #endif

   //loj: 6/15/05 Added
   // Clear plot area and redraw other area
   wxClientDC dc(this);

   dc.SetClippingRegion(plotArea.x, plotArea.y, plotArea.width,
      plotArea.height);
   dc.SetBackground(wxBrush(plotColor, wxTRANSPARENT));
   dc.Clear();
   dc.DestroyClippingRegion();

   // Now draw other area
   wxBEGIN_DRAWING

   DrawAxes(dc);
   DrawLabels(dc);

   if (hasGrid)
      DrawGrid(dc);

   if (hasLegend)
      DrawLegend(dc);

   wxEND_DRAWING
   dataUpdated = false;

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage(
      "TsPlotCanvas::ClearAllCurveData() leaving\n");
   #endif
}


//==============================================================================
// Interface manipulation methods
//==============================================================================

void TsPlotCanvas::ShowGrid(bool show)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Toggling hasGrid to %s\n",
         (show ? "true" : "false"));
   #endif
   hasGrid = show;
}


void TsPlotCanvas::ToggleGrid(wxCommandEvent& event)
{
   hasGrid = (hasGrid ? false : true);
   wxWindow::Refresh(true);
}


void TsPlotCanvas::ShowLegend(bool show)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Toggling hasLegend to %s\n",
         (show ? "true" : "false"));
   #endif
   hasLegend = show;
}


void TsPlotCanvas::ToggleLegend(wxCommandEvent& event)
{
   hasLegend = (hasLegend ? false : true);
   wxWindow::Refresh(true);
}


void TsPlotCanvas::SetOptions(wxCommandEvent& event)
{
   TsPlotOptionsDialog dlg(xName.c_str(),yName.c_str(),this, -1, "Plot options");
   dlg.SetPlotTitle(plotTitle);
   dlg.SetXLabel(xLabel);
   dlg.SetYLabel(yLabel);
   if (data.size() > 0)
      dlg.SetWidth(data[0]->GetWidth());
   else
      dlg.SetWidth(1);

   dlg.SetXPrecision(xLabelPrecision);
   dlg.SetYPrecision(yLabelPrecision);

   dlg.SetXMin(userXMin);
   dlg.SetXMax(userXMax);
   dlg.SetYMin(userYMin);
   dlg.SetYMax(userYMax);

   dlg.SetXMinState(overrideXMin);
   dlg.SetXMaxState(overrideXMax);
   dlg.SetYMinState(overrideYMin);
   dlg.SetYMaxState(overrideYMax);

   dlg.SetXTickCount(xticks);
   dlg.SetYTickCount(yticks);

   if (dlg.ShowModal() == wxID_OK)
   {
      // Plot title
      plotTitle = dlg.GetPlotTitle();
      showTitle = (plotTitle != "");

      // Plot x- and y-axis label
      xLabel = dlg.GetXLabel();
      yLabel = dlg.GetYLabel();
      labelAxes = ((xLabel != "") | (yLabel != ""));

      // Line properties
      SetLineWidth(dlg.GetWidth());

      xLabelPrecision = dlg.GetXPrecision();
      yLabelPrecision = dlg.GetYPrecision();

      overrideXMin = dlg.GetXMinState();
      overrideXMax = dlg.GetXMaxState();
      overrideYMin = dlg.GetYMinState();
      overrideYMax = dlg.GetYMaxState();

      if (overrideXMin)
         userXMin = dlg.GetXMin();
      else
         plotXMin = 1e99;

      if (overrideXMax)
         userXMax = dlg.GetXMax();
      else
         plotXMax = -1e99;

      if (overrideYMin)
         userYMin = dlg.GetYMin();
      else
         plotYMin = 1e99;

      if (overrideYMax)
         userYMax = dlg.GetYMax();
      else
         plotYMax = -1e99;

      xticks = dlg.GetXTickCount();
      yticks = dlg.GetYTickCount();

      ResetRanges();

      wxClientDC dc(this);
      Refresh(dc, true);
   }
}


void TsPlotCanvas::ResetRanges()
{
   if (overrideXMin)
      plotXMin = userXMin;
   if (overrideXMax)
      plotXMax = userXMax;
   if (overrideYMin)
      plotYMin = userYMin;
   if (overrideYMax)
      plotYMax = userYMax;
}

void TsPlotCanvas::SaveData(wxCommandEvent& event)
{
   wxFileDialog dlg(this, "Select save file name", "", "PlotData.txt", "*.*",
      wxFILE_SAVE_FLAG | wxFILE_OVERWRITE_FLAG);

   if (dlg.ShowModal() == wxID_OK)
   {
      std::string filename;
      filename = dlg.GetPath().c_str();
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Selected file name: %s",
            filename.c_str());
      #endif
      DumpData(filename);
   }
}


void TsPlotCanvas::SaveImage(const char *filename)
{
    // Create a memory DC
   wxMemoryDC temp_dc;
   wxCoord w, h;
   GetSize(&w, &h);
   wxBitmap test_bitmap(w * 2, h * 2);
   temp_dc.SelectObject(test_bitmap);
   bool titleVisible = showTitle;
   showTitle = true;
    // draw to memory dc bitmap
   Refresh(temp_dc, true);
   showTitle = titleVisible;
   // save bitmap
   test_bitmap.SaveFile(filename, wxBITMAP_TYPE_BMP);

}

void TsPlotCanvas::SaveImage(wxCommandEvent& event)
{
   wxFileDialog dlg(this, "Select save file name", "", "PlotImage.bmp", "*.*",
      wxFILE_SAVE_FLAG | wxFILE_OVERWRITE_FLAG);

   if (dlg.ShowModal() == wxID_OK)
   {
      std::string filename;
      filename = dlg.GetPath().c_str();
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Selected file name: %s",
            filename.c_str());
      #endif
      SaveImage(filename.c_str());
   }
}


void TsPlotCanvas::Zoom(wxRect &region)
{
   wxClientDC dc(this);
   wxCoord w, h;
   dc.GetSize(&w, &h);
   wxCoord wid = w - right, ht = h - bottom;

   if (region.GetX() < left)
      region.SetX(left);
   if (region.GetX()+region.GetWidth() > wid)
      region.SetWidth(wid-region.GetX());

   if (region.GetY() < top)
      region.SetY(top);
   if (region.GetY()+region.GetHeight() > ht)
      region.SetHeight(ht-region.GetY());

   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Zoom to [%d %d; %d %d]!\n"
                                    "BoundBox is [%d %d; %d %d]",
         region.GetX(), region.GetY(), region.GetX()+region.GetWidth(),
         region.GetY()+region.GetHeight(), left, top, wid, ht);
   #endif

   zoomXMin = GetActualXValue(region.x);
   zoomXMax = GetActualXValue(region.x + region.width);
   zoomYMin = GetActualYValue(region.y + region.height);
   zoomYMax = GetActualYValue(region.y);

   zoomed = true;
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Zoom region is [%lf %lf; %lf %lf]!\n",
         zoomXMin, zoomXMax, zoomYMin, zoomYMax);
   #endif

   wxWindow::Refresh(true);
}


void TsPlotCanvas::UnZoom()
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Unzoom!\n");
   #endif
   zoomed = false;

   wxWindow::Refresh(true);
}


void TsPlotCanvas::PenUp(int index)
{
   if (index == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin();
           i != data.end(); ++i)
         (*i)->PenUp();
   else
      if ((index >= 0) && (data.size() < (unsigned int)index))
         data[index]->PenUp();

   for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
        curve != data.end(); ++curve)
      (*curve)->lastPointPlotted = 0;

   // Finally, refresh the display
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::PenDown(int index)
{
   if (index == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin();
           i != data.end(); ++i)
         (*i)->PenDown();
   else
      if ((index >= 0) && (data.size() < (unsigned int)index))
         data[index]->PenDown();

   for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
        curve != data.end(); ++curve)
      (*curve)->lastPointPlotted = 0;

   // Finally, refresh the display
   wxClientDC dc(this);
   Refresh(dc, true);
}

void TsPlotCanvas::Darken(int factor, int index, int forCurve)
{
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         data[forCurve]->DarkenColour(factor);
         data[forCurve]->lastPointPlotted = 0;
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         (*curve)->DarkenColour(factor);
         (*curve)->lastPointPlotted = 0;
      }
   }
}

void TsPlotCanvas::Lighten(int factor, int index, int forCurve)
{
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         data[forCurve]->LightenColour(factor);
         data[forCurve]->lastPointPlotted = 0;
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         (*curve)->LightenColour(factor);
         (*curve)->lastPointPlotted = 0;
      }
   }
}

void TsPlotCanvas::MarkPoint(int index, int forCurve)
{
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         data[forCurve]->HighlightPoint(index);
         data[forCurve]->lastPointPlotted = 0;
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         (*curve)->HighlightPoint(index);
         (*curve)->lastPointPlotted = 0;
      }
   }
}


void TsPlotCanvas::MarkBreak(int index, int forCurve)
{
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         data[forCurve]->AddBreak(index);
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         (*curve)->AddBreak(index);
      }
   }
}

void TsPlotCanvas::ClearFromBreak(int startBreakNumber, int endBreakNumber,
      int forCurve)
{
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         data[forCurve]->BreakAndDiscard(startBreakNumber, endBreakNumber);
      }
   }
   else // Discard points from all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         (*curve)->BreakAndDiscard(startBreakNumber, endBreakNumber);
      }
   }
}

void TsPlotCanvas::ChangeColor(int index, unsigned long newColor, int forCurve)
{
   wxColour useColor;
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         if (newColor == 0xffffff)
            useColor = data[forCurve]->GetColour(0);
         else
            useColor.Set(newColor);
         data[forCurve]->SetColour(useColor, index);
         data[forCurve]->lastPointPlotted = 0;
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         if (newColor == 0xffffff)
            useColor = data[forCurve]->GetColour(0);
         else
            useColor.Set(newColor);
         (*curve)->SetColour(useColor, index);
         (*curve)->lastPointPlotted = 0;
      }
   }
}


void TsPlotCanvas::ChangeMarker(int index, int newMarker, int forCurve)
{
   MarkerType useMarker;
   if (forCurve >= 0)
   {
      if (forCurve < (int)data.size())
      {
         if (newMarker == -1)
            useMarker = (MarkerType)data[forCurve]->GetMarker(0);
         else
            useMarker = (MarkerType)(newMarker % MarkerCount);

         data[forCurve]->SetMarker(useMarker, index);
         data[forCurve]->lastPointPlotted = 0;
      }
   }
   else // Mark the point on all curves
   {
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin();
           curve != data.end(); ++curve)
      {
         if (newMarker == -1)
            useMarker = (MarkerType)data[forCurve]->GetMarker(0);
         else
            useMarker = (MarkerType)(newMarker % MarkerCount);

         (*curve)->SetMarker(useMarker, index);
         (*curve)->lastPointPlotted = 0;
      }
   }
}


void TsPlotCanvas::SetLineWidth(int w,  int lineId)
{
   if (lineId == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin();
           i != data.end(); ++i)
         (*i)->SetWidth(w);
   else
      if (lineId < (int)data.size())
         data[lineId]->SetWidth(w);

   // Finally, refresh the display
   for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
      (*i)->lastPointPlotted = 0;
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::SetLineStyle(int ls, int lineId)
{
   if (lineId == -1)
   {
      for (std::vector<TsPlotCurve *>::iterator i = data.begin();
           i != data.end(); ++i)
         (*i)->SetStyle(ls);
   }
   else
      if (lineId < (int)data.size())
         data[lineId]->SetStyle(ls);

   // Finally, refresh the display
   for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
      (*i)->lastPointPlotted = 0;
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::SetAxisLimit(std::string axisEnd, bool automatic,
                                double value)
{
   if (axisEnd == "xMinimum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideXMin)
         {
            overrideXMin = false;
            plotXMin = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideXMin = true;
         plotXMin = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "xMaximum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideXMax)
         {
            overrideXMax = false;
            plotXMax = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideXMax = true;
         plotXMax = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "yMinimum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideYMin)
         {
            overrideYMin = false;
            plotYMin = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideYMin = true;
         plotYMin = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "yMaximum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideYMax)
         {
            overrideYMax = false;
            plotYMax = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideYMax = true;
         plotYMax = value;
         ResetRanges();
      }
   }
   else
   {
      MessageInterface::ShowMessage("Axis override failed; specify axes using "
         "the names \"xMinimum\", \"xMaximum\", \"yMinimum\", or "
         "\"yMaximum\".\n");
   }
}


//------------------------------------------------------------------------------
// void Rescale()
//------------------------------------------------------------------------------
/**
 * Sets/resets the minimum and maximum scale settings for the plot
 *
 * @note This method is not called during the run.
 */
//------------------------------------------------------------------------------
void TsPlotCanvas::Rescale()
{
   overrideXMin = false;
   overrideXMax = false;
   overrideYMin = false;
   overrideYMax = false;
   plotXMin = 1e99;
   plotXMax = -1e99;
   plotYMin = 1e99;
   plotYMax = -1e99;
   xMin = 1e99;
   xMax = -1e99;
   yMin = 1e99;
   yMax = -1e99;

   #ifdef DEBUG_RESCALING
      MessageInterface::ShowMessage
         ("!!! plotXMin=%g, plotXMax=%g, plotYMin=%g, plotYMax=%g\n",
          plotXMin, plotXMax, plotYMin, plotYMax);
   #endif

   wxClientDC dc(this);
   Rescale(dc);

   wxWindow::Refresh(true);
}

void TsPlotCanvas::SetTickCount(int count, bool isXAxis)
{
   if (isXAxis)
      xticks = count;
   yticks = count;
}


int  TsPlotCanvas::GetTickCount(bool isXAxis)
{
   if (isXAxis)
      return xticks;
   return yticks;
}


void TsPlotCanvas::SetMinorTickCount(int count, bool isXAxis)
{
   if (isXAxis)
      xMinorTicks = count;
   yMinorTicks = count;
}


int TsPlotCanvas::GetMinorTickCount(bool isXAxis)
{
   if (isXAxis)
      return xMinorTicks;
   return yMinorTicks;
}


//==============================================================================
// Helper methods
//==============================================================================

int TsPlotCanvas::GetXLocation(double val, double val2)
{
   return (int)(left + (val - currentXMin) * xScale);
}


int TsPlotCanvas::GetYLocation(double val, double val2)
{
   return (int)(top + (currentYMax - val) * yScale);
}


double TsPlotCanvas::GetActualXValue(int x, int y)
{
   return currentXMin + (x - left) / xScale;
}


double TsPlotCanvas::GetActualYValue(int y, int x)
{
   return currentYMax - (y - top) / yScale;
}

void TsPlotCanvas::AlwaysDraw(bool tf)
{
   alwaysDraw = tf;
   runState = alwaysDraw ? 10000 : 10001;
}
