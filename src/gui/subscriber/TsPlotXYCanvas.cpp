// $Id$
//------------------------------------------------------------------------------
//                            TsPlotXYCanvas
//------------------------------------------------------------------------------
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2007/05/26
//
// This code is Copyright (c) 2007, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Implements the TsPlotXYCanvas window.
 */
//------------------------------------------------------------------------------


#include "TsPlotXYCanvas.hpp"
#include <sstream>
#include <algorithm>
#include "MessageInterface.hpp"

//#define DEBUG_INTERFACE
//#define DEBUG_PENUP_PENDOWN


//------------------------------------------------------------------------------
// TsPlotXYCanvas(wxWindow* parent, wxWindowID id,
//      const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
//    TsPlotCanvas         (parent, id, pos, size, style, name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param parent The owner of this canvas
 * @param id The ID for the canvas
 * @param pos The canvas's lower left location
 * @param size The width and height of the canvas
 * @param style The canvas style
 * @param name The name of this plot
 */
//------------------------------------------------------------------------------
TsPlotXYCanvas::TsPlotXYCanvas(wxWindow* parent, wxWindowID id, 
     const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
   TsPlotCanvas         (parent, id, pos, size, style, name)
{
}

//------------------------------------------------------------------------------
// TsPlotXYCanvas::~TsPlotXYCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TsPlotXYCanvas::~TsPlotXYCanvas()
{
}


//------------------------------------------------------------------------------
// void DrawAxes(wxDC &dc)
//------------------------------------------------------------------------------
/**
 * Draws the X and Y axes, tick marks, and axis labels
 *
 * @param dc The drawing device context
 */
//------------------------------------------------------------------------------
void TsPlotXYCanvas::DrawAxes(wxDC &dc)
{
   wxCoord w, h;
   dc.GetSize(&w, &h);
   
   // Draw the frame
   dc.SetPen(*wxBLACK_PEN);
   
   int wid = w-(left+right), 
       ht  = h - (top+bottom), 
       x0  = left, 
       y0  = h-(bottom), 
       xm  = w-right, 
       ym  = top;

   dc.DrawLine(x0, y0, x0, ym);
   dc.DrawLine(xm, y0, xm, ym);
   dc.DrawLine(x0, y0, xm, y0);
   dc.DrawLine(x0, ym, xm, ym);

   
   // Draw the axes with labels  
   int idx, idy;
   double dx, dy, dxval, dyval, fval;

   dx = wid / (double)xticks;
   dy = ht  / (double)yticks;
   dxval = (currentXMax-currentXMin) / (double)xticks;
   dyval = (currentYMax-currentYMin) / (double)yticks;
   
   wxString label;
   std::stringstream labelStream;
   
   //if ((int)xGridLoc.size() != xticks+1)
   //{
   //   xGridLoc.clear();
      //for (int i = 0; i <= xticks; ++i)
      //   xGridLoc.push_back(0);
   //}
   //if ((int)yGridLoc.size() != yticks+1)
   //{
   //   xGridLoc.clear();
      //for (int i = 0; i <= yticks; ++i)
      //   yGridLoc.push_back(0);
   //}

   dc.SetFont(labelFont);

   // x-Axis first
   double delta, step, logStep, factor, div, start;
   delta = currentXMax - currentXMin;

   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage
         ("***XMin = %lf, XMax = %lf, delta=%lf\n", currentXMin, currentXMax, delta);
   #endif

   step = delta / (xticks+1.0);
   if (delta > 0.0)
   {
      logStep = (int)log10(step);
      factor = pow(10.0, logStep-2);
      delta = ((int)(step / factor)) * factor;
      div = (int)(currentXMin / delta);
      if (currentXMin > 0.0)
         div += 1.0;
      start = div * delta;

      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage(
            "step = %lf, logStep = %lf, factor = %lf, div = %lf\n", step, 
            logStep, factor, div);
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start,
            delta);
      #endif
   }
   else
   {
      start = currentXMin;
      delta = step;
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   
   labelStream.precision(xLabelPrecision);
   for (int i = 0; i <= xticks; ++i)
   {
      idx = (int)(i * dx + 0.5);
      fval = start + i * delta;  //currentXMin + i*dxval;
      if (fabs(fval) < 1e-9) fval = 0.0;

      xGridLoc[i] = GetXLocation(fval);  //x0 + idx;
      dc.DrawLine(xGridLoc[i], y0, xGridLoc[i], y0 - tickSize);
      dc.DrawLine(xGridLoc[i], ym, xGridLoc[i], ym + tickSize);
      labelStream.str("");
      labelStream << fval;
      label = labelStream.str().c_str();
      dc.GetTextExtent(label, &w, &h);
      dc.DrawText(label, xGridLoc[i] - w/2, y0 + h/2);
   }

   delta = currentYMax - currentYMin;

   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("***YMin = %lf, YMax = %lf, delta=%g\n", currentYMin,
         currentYMax, delta);
   #endif
   
   step = delta / (yticks+1.0);
   if (delta > 0.0)
   {
      logStep = (int)log10(step);
      factor = pow(10.0, logStep-2);
      delta = ((int)(step / factor)) * factor;
      div = (int)(currentYMin / delta);
      if (currentYMin > 0.0)
         div += 1.0;
      start = div * delta;

      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage(
            "step = %lf, logStep = %lf, factor = %lf, div = %lf\n",
            step, logStep, factor, div);
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   else
   {
      start = currentYMin;
      delta = step;
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   
   labelStream.precision(yLabelPrecision);
   for (int i = 0; i <= yticks; ++i)
   {
      idy = (int)(i * dy + 0.5);
      fval = start + i*delta;
      if (fabs(fval) < 1e-9) fval = 0.0;
      yGridLoc[i] = GetYLocation(fval);
      dc.DrawLine(xm, yGridLoc[i], xm - tickSize, yGridLoc[i]);
      dc.DrawLine(x0, yGridLoc[i], x0 + tickSize, yGridLoc[i]);
      labelStream.str("");
      labelStream << fval;
      label = labelStream.str().c_str();
      dc.GetTextExtent(label, &w, &h);
      dc.DrawText(label, x0 - w - 4, yGridLoc[i] - h/2);
   }
}


//------------------------------------------------------------------------------
// void DrawLabels(wxDC &dc)
//------------------------------------------------------------------------------
/**
 * Labels axes nad places plot titles
 *
 * @param dc The drawing device context
 */
//------------------------------------------------------------------------------
void TsPlotXYCanvas::DrawLabels(wxDC &dc)
{
   wxCoord wid, w, ht, h;
   dc.GetSize(&wid, &ht);
   
   int xCenter = (wid + left - right)/ 2, yCenter = (ht + top - bottom)/ 2;

   // Build default labels if they are not set already
   if ((xLabel == "") && defaultLabels)
      xLabel = "X Data";
      
   if ((yLabel == "") && defaultLabels)
   {
      if (names.size() != 0)
      {
         //for (std::vector<std::string>::iterator i = names.begin(); 
         for (std::vector<wxString>::iterator i = names.begin(); 
              i < names.end(); ++i)
         {
            if (i != names.begin())
               yLabel += ", ";
            yLabel += *i;
         }
      }
      else
      {
         yLabel = "Y Data";
      }
   }
   
   if (showTitle)
   {
      if (plotTitle == "")
         plotTitle = yLabel + " vs " + xLabel;

      // Draw the title
      dc.SetFont(titleFont);
      wxString label;
      wxString title = plotTitle;
      dc.GetTextExtent(title, &w, &h);
      int xloc = xCenter - w / 2;
      int yloc = (top - h) / 2;
      dc.DrawText(title, xloc, yloc);
   }

   if (labelAxes)
   {
      // Add x-axis label
      dc.SetFont(axisFont);
      wxString title = xLabel.c_str();
      dc.GetTextExtent(title, &w, &h);
      int xloc = xCenter - w / 2;
      int yloc = ht - bottom / 2;
      dc.DrawText(title, xloc, yloc);

      // Add y-axis label
      title = yLabel;
      dc.GetTextExtent(title, &w, &h);
      xloc = h / 2;
      yloc = yCenter + w / 2;
      dc.DrawRotatedText(title, xloc, yloc, 90.0);
   }
}


//------------------------------------------------------------------------------
// void PlotData(wxDC &dc)
//------------------------------------------------------------------------------
/**
 * Plots the data in the curves
 *
 * @param dc The drawing device context, used for size data
 */
//------------------------------------------------------------------------------
void TsPlotXYCanvas::PlotData(wxDC &dc)
{
   #ifdef DEBUG_PLOT_DATA
   MessageInterface::ShowMessage("TsPlotXYCanvas::PlotData() entered\n");
   #endif
   wxCoord w, h;
   dc.GetSize(&w, &h);

   dc.SetClippingRegion(left, top, (w-(left+right)), (h-(top+bottom)));
   
   if (data.size() > 0)
   {
      int n = 0;
      int pupLoc = -1, pupIndex = -1, locCount = 0;
      int ccLoc = -1, ccIndex = -1, ccCount = 0;
      int mcLoc = -1, mcIndex = -1, mcCount = 0;
      const std::vector<int> *pups;
      const std::vector<int> *ccs;
      const std::vector<int> *mcs;
      const std::vector<int> *highlights;

      for (std::vector<TsPlotCurve *>::iterator curve = data.begin(); 
           curve != data.end(); ++curve)
      {
         pups = (*curve)->GetPenUpLocations(); // penUpLocations(n);
         ccs =  (*curve)->GetColorChangeLocations();
         mcs =  (*curve)->GetMarkerChangeLocations();
         highlights = (*curve)->GetHighlightPoints();

         #ifdef DEBUG_MARK_POINT
            MessageInterface::ShowMessage("HighlightCount = %d: [", 
               highlights->size());
            for (unsigned int i = 0; i < highlights->size(); ++i)
               MessageInterface::ShowMessage(" %d ", highlights->at(i));
            MessageInterface::ShowMessage("]; curve has %d points\n", 
               (*curve)->abscissa.size());
         #endif
         locCount = (int)pups->size();
         if (locCount > 0)
         {
            #ifdef DEBUG_PENUP_PENDOWN
               MessageInterface::ShowMessage("Penup count: %d\n", locCount);
            #endif
            pupIndex = 0;
            pupLoc = (*pups)[pupIndex];
         }
         
         ccCount = (int)ccs->size();
         if (ccCount > 0)
         {
            ccIndex = 0;
            ccLoc = 0;
         }

         mcCount = (int)mcs->size();
         if (mcCount > 0)
         {
            mcIndex = 0;
            mcLoc = 0;
         }

         if ((*curve)->abscissa.size() > 0)
         {
            plotPens[n].SetColour((*curve)->GetColour(0));
            plotPens[n].SetWidth((*curve)->GetWidth());
            plotPens[n].SetStyle((*curve)->GetStyle());
            dc.SetPen(plotPens[n]);

            if ((unsigned int)pupLoc < (*curve)->lastPointPlotted)
            {
               // Get the next penup
               ++pupIndex;
               if (pupIndex < locCount)
                  pupLoc = (*pups)[pupIndex];

                  #ifdef DEBUG_PENUP_PENDOWN
                     MessageInterface::ShowMessage("Penup location now %d, "
                           "Surrounding y-axis points: [", pupLoc);

                     for (Integer i = -2; i < 3; ++i)
                     {
                        if ((pupLoc + i >= 0) &&
                            (pupLoc + i < (*curve)->ordinate.size()) )
                           MessageInterface::ShowMessage(" %lf ",
                                 (*curve)->ordinate[pupLoc + i]);
                     }
                     MessageInterface::ShowMessage("]\n");
                  #endif
            }
            
            if ((unsigned int)ccLoc <= (*curve)->lastPointPlotted)
            {
               // Get the next color
               ++ccIndex;
               if (ccIndex < ccCount)
                  ccLoc = (*ccs)[ccIndex];
            }

            if ((unsigned int)mcLoc <= (*curve)->lastPointPlotted)
            {
               // Get the next marker
               ++mcIndex;
               if (mcIndex < mcCount)
                  mcLoc = (*mcs)[mcIndex];
            }

            int j;
            int x0, y0, x1, y1, hi = 0, lo = 0;

            bool drawLines   = (*curve)->UseLine();
            bool drawMarker  = (*curve)->UseMarker();
            int  markerStyle = (*curve)->GetMarker();
            int  markerSize  = (*curve)->GetMarkerSize();

            bool showErrorBars = (*curve)->UseHiLow();
            for (j = (*curve)->lastPointPlotted;
                 j < (int)((*curve)->abscissa.size())-1; ++j)
            {
               if (j == ccLoc)
               {
                  plotPens[n].SetColour((*curve)->GetColour(ccIndex));
                  // Get the next color
                  ++ccIndex;
                  if (ccIndex < ccCount)
                     ccLoc = (*ccs)[ccIndex];
                  dc.SetPen(plotPens[n]);
               }

               if (j == mcLoc)
               {
                  markerStyle = (*curve)->GetMarker(mcIndex);
                  // Get the next marker
                  ++mcIndex;
                  if (mcIndex < mcCount)
                     mcLoc = (*mcs)[mcIndex];
               }

               if (j != pupLoc)
               {
                  x0 = int(left +
                        ((*curve)->abscissa[j]-currentXMin) * xScale + 0.5);
                  y0 = int(top +
                        (currentYMax-(*curve)->ordinate[j]) * yScale + 0.5);

                  if (showErrorBars)
                  {
                     if ((Integer)((*curve)->highError.size()) > j)
                     {
                        hi = int((*curve)->highError[j] * yScale + 0.5);
                        if ((Integer)((*curve)->lowError.size()) > j)
                           lo = int((*curve)->lowError[j] * yScale + 0.5);
                        else
                           lo = hi;

                        #ifdef DEBUG_ERROR_BARS
                           MessageInterface::ShowMessage("high = %lf -> "
                                 "hi = %d, lo = %d\n", (*curve)->highError[j],
                                 hi, lo);
                        #endif
                     }
                  }

                  if (drawLines)
                  {
                     x1 = int(left +
                           ((*curve)->abscissa[j+1]-currentXMin)*xScale + 0.5);
                     y1 = int(top +
                           (currentYMax-(*curve)->ordinate[j+1])*yScale + 0.5);

                     dc.DrawLine(x0, y0, x1, y1);
                  }

                  if (drawMarker)
                  {
                     if (find(highlights->begin(), highlights->end(), j) !=
                           highlights->end())
                        DrawMarker(dc, highlightMarker, markerSize, x0, y0,
                              plotPens[n]);
                     else
                     {
                        if (showErrorBars)
                           DrawMarker(dc, sigma13Marker, markerSize, x0, y0,
                                 plotPens[n], hi, lo);
                        DrawMarker(dc, markerStyle, markerSize, x0, y0,
                              plotPens[n]);
                     }
                  }
                  else if (find(highlights->begin(), highlights->end(), j) !=
                        highlights->end())
// Omitted so that marker is ALWAYS placed on last point received (GMT-3478)
//                     if (j > 0)
//                        DrawMarker(dc, highlightMarker, markerSize, x1, y1,
//                              plotPens[n]);
//                     else
                        DrawMarker(dc, highlightMarker, markerSize, x0, y0,
                              plotPens[n]);
               }
               else
               {
                  // Get the next penup
                  ++pupIndex;
                  if (pupIndex < locCount)
                     pupLoc = (*pups)[pupIndex];
                  #ifdef DEBUG_PENUP_PENDOWN
                     MessageInterface::ShowMessage("Penup location now %d, "
                           "Surrounding y-axis points: [", pupLoc);

                     for (Integer i = -2; i < 3; ++i)
                     {
                        if ((pupLoc + i >= 0) &&
                            (pupLoc + i < (*curve)->ordinate.size()) )
                           MessageInterface::ShowMessage(" %lf ",
                                 (*curve)->ordinate[pupLoc + i]);
                     }
                     MessageInterface::ShowMessage("]\n");
                  #endif
               }
            }

            // Catch the marker for the last point
            if (drawMarker)
            {
               int k = (*curve)->abscissa.size() - 1;
               x1 = int(left +
                     ((*curve)->abscissa[k]-currentXMin)*xScale + 0.5);
               y1 = int(top +
                     (currentYMax-(*curve)->ordinate[k])*yScale + 0.5);

               DrawMarker(dc, markerStyle, markerSize, x1, y1, plotPens[n]);
            }
            else if (find(highlights->begin(), highlights->end(), j) !=
                  highlights->end())
               DrawMarker(dc, highlightMarker, markerSize, x1, y1,
                     plotPens[n]);

            (*curve)->lastPointPlotted = j-1;
         }
         ++n;
      }
   }
   dc.DestroyClippingRegion();

//   RefreshRect(wxRect(0,0,w,h));

   #ifdef DEBUG_PLOT_DATA
   MessageInterface::ShowMessage("TsPlotXYCanvas::PlotData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void Rescale(wxDC &dc)
//------------------------------------------------------------------------------
/**
 * Resets the scales for the X and Y axes
 *
 * @param dc The drawing device context, used for size data
 */
//------------------------------------------------------------------------------
void TsPlotXYCanvas::Rescale(wxDC &dc)
{
   bool isNotFirstPoint = false;
   wxCoord w, h;
   dc.GetSize(&w, &h);

   w -= (left+right);
   h -= (top+bottom);

   double delx;
   double dely;

   if (!zoomed)
   {
      #ifdef DEBUG_RESCALING
         MessageInterface::ShowMessage
            ("===> no-zoom: plotXMin=%g, plotXMax=%g, plotYMin=%g, "
             "plotYMax=%g\n", plotXMin, plotXMax, plotYMin, plotYMax);
      #endif
      std::vector<TsPlotCurve *>::iterator i = data.begin();
      if (i != data.end())
      {
         (*i)->Rescale();

         xMin = (*i)->minX;
         xMax = (*i)->maxX;
         yMin = (*i)->minY;
         yMax = (*i)->maxY;

         // Look through the PlotCurves for ranges
         for (++i; i != data.end(); ++i)
         {
            (*i)->Rescale();

            if ((*i)->minX < xMin)
               xMin = (*i)->minX;
            if ((*i)->maxX > xMax)
               xMax = (*i)->maxX;
            if ((*i)->minY < yMin)
               yMin = (*i)->minY;
            if ((*i)->maxY > yMax)
               yMax = (*i)->maxY;

            if ((*i)->abscissa.size() > 1)
            {
               isNotFirstPoint = true;
               #if DEBUG_TS_CANVAS
                  MessageInterface::ShowMessage(
                     "TsPlotCanvas::Rescale() datasize=%d, %s\n", 
                     data.size(),  
                     (isNotFirstPoint?"is not First Point":"is First Point"));
               #endif
            }
         }
      }
      delx = (xMax - xMin)*0.1;
      dely = (yMax - yMin)*0.1;
      rescaled = false;
      
      #ifdef DEBUG_RESCALING
         MessageInterface::ShowMessage
            ("===> no-zoom: xMin=%g, xMax=%g, yMin=%g, yMax=%g\n",
             xMin, xMax, yMin, yMax);
      #endif
      
      if (!overrideXMax && (xMax > plotXMax))
      {
         plotXMax = xMax + delx;
         rescaled = true;
      }

      if (!overrideXMin && (xMin < plotXMin))
      {
         #if DEBUG_TS_CANVAS
            MessageInterface::ShowMessage(
               "TsPlotCanvas::Rescale() xMin(%lf) < plotXMin(%lf) new value: ",
               xMin, plotXMin);
         #endif
         plotXMin = xMin - delx;
         rescaled = true;
         #if DEBUG_TS_CANVAS
            MessageInterface::ShowMessage("%lf\n", plotXMin);
         #endif
      }

      #ifdef DEBUG_RESCALING
         MessageInterface::ShowMessage
            ("===> no-zoom: yMin=%g, yMax=%g, plotYMin=%g, plotYMax=%g\n",
             yMin, yMax, plotYMin, plotYMax);
      #endif

      if (!overrideYMin && (yMin < plotYMin))
      {
         plotYMin = yMin - dely;
         rescaled = true;
      }

      if (!overrideYMax && (yMax > plotYMax))
      {
         plotYMax = yMax + dely;
         rescaled = true;
      }

      #ifdef DEBUG_RESCALING
         MessageInterface::ShowMessage
            ("===> no-zoom: Overrides = %s %s %s %s\n",
             (overrideXMin ? "true" : "false"),
             (overrideXMax ? "true" : "false"),
             (overrideYMin ? "true" : "false"),
             (overrideYMax ? "true" : "false"));
      #endif

      if (rescaled)
      {
         for (i = data.begin(); i != data.end(); ++i)
            (*i)->lastPointPlotted = 0;
      }

      if (plotXMin == plotXMax)
         delx = 1.0;
      else
         delx = plotXMax - plotXMin;

      if (plotYMin == plotYMax)
         dely = 1.0;
      else
         dely = plotYMax - plotYMin;

      currentXMin = plotXMin;
      currentXMax = plotXMax;
      currentYMin = plotYMin;
      currentYMax = plotYMax;
      
      #ifdef DEBUG_RESCALING
         MessageInterface::ShowMessage
            ("===> no-zoom: currentXMin=%g, currentXMax=%g, currentYMin=%g, "
             "currentYMax=%g\n", currentXMin, currentXMax, currentYMin,
             currentYMax);
      #endif
   }
   else
   {
      xMin = zoomXMin;
      xMax = zoomXMax;
      yMin = zoomYMin;
      yMax = zoomYMax;

      delx = (xMax - xMin);
      dely = (yMax - yMin);

      currentXMin = zoomXMin;
      currentXMax = zoomXMax;
      currentYMin = zoomYMin;
      currentYMax = zoomYMax;
      
      //MessageInterface::ShowMessage
      //   ("===>    zoom: currentXMin=%f, currentXMax=%f, currentYMin=%f,"
      //    "currentYMax=%f\n", currentXMin, currentXMax, currentYMin, currentYMax);
      
      rescaled = true;
   }
   
   xScale = w / delx;
   yScale = h / dely;
   
   //MessageInterface::ShowMessage
   //   ("===> xScale=%g, yScale=%g, w=%d, h=%d, delx=%g, dely=%g\n",
   //    xScale, yScale, w, h, delx, dely);
}
