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


#ifndef _TSPLOTCANVAS_HPP_
#define _TSPLOTCANVAS_HPP_

#include <wx/wx.h>
#include <vector>
#include <string>

#include "TsPlotDefs.hpp"
#include "TsPlotCurve.hpp"


class TsPlotCanvas : public wxWindow
{
public:
   enum PlotComponents {
      PLOT_TITLE = 1024,
      X_LABEL,
      Y_LABEL,
      PLOT_AREA,
      XAXIS,
      YAXIS,
      YAXIS2,
      ID_TOGGLE_GRID,
      ID_TOGGLE_LEGEND,
      ID_PLOT_DETAILS,
      ID_PLOT_SAVE,
      ID_PLOT_SAVEIMAGE
   };

public:
   TsPlotCanvas(wxWindow* parent, wxWindowID id = -1, 
                const wxPoint& pos = wxPoint(1,1),
                const wxSize& size = wxDefaultSize, 
                long style = wxTAB_TRAVERSAL,
                const wxString& name = "");

   virtual void OnPaint(wxPaintEvent& ev);
   virtual void OnSize(wxSizeEvent& ev);
   void OnMouseEvent(wxMouseEvent& event);
   
   // Override of base wx method to allow region invalidation before updating
   virtual void Update ();

   void Refresh(wxDC &dc, bool drawAll);

   void DumpData(const std::string &fn);
   void SetLabel(const wxString &dataName, const PlotComponents which);
   void SetDataName(const wxString &dataName);
   void AddData(TsPlotCurve *curve, wxColour startColor = *wxWHITE);
   void DataUpdate(bool tf = true);
   int  GetCurveCount();
   void ShowGrid(bool show);
   void ShowLegend(bool show = true);
   
   void ClearAllCurveData();

   TsPlotCurve*   GetPlotCurve(int index);
   void DeletePlotCurve(int index);
   
   void PenUp(int index = -1);
   void PenDown(int index = -1);
   void Darken(int factor, int index, int forCurve = -1);
   void Lighten(int factor, int index, int forCurve);
   void MarkPoint(int index, int forCurve = -1);
   void MarkBreak(int index, int forCurve);
   void ClearFromBreak(int breakNumber, int index, int forCurve);
   void ChangeColor(int index, unsigned long newColor = 0xffffff,
         int forCurve = -1);
   void ChangeMarker(int index, int newMarker = -1, int forCurve = -1);
   
   void SetLineWidth(int w,  int lineId = -1);
   void SetLineStyle(int ls, int lineId = -1);
   void SetAxisLimit(std::string axisEnd, bool automatic = false, 
                     double value = 0.0);
                     
   void Rescale();

   void SetTickCount(int count, bool isXAxis = true);
   int  GetTickCount(bool isXAxis = true);
   void SetMinorTickCount(int count, bool isXAxis = true);
   int  GetMinorTickCount(bool isXAxis = true);
   void AlwaysDraw(bool tf);
   void SaveImage(const char *filename);

protected:
   /// Run state
   int runState;
   /// borders
   int left, right, top, bottom;
   /// fonts
   wxFont titleFont, labelFont, axisFont, legendFont;
   /// font sizes
   int titleSize, labelSize, legendSize;
   /// Colors
   wxColour backgnd, plotColor, legendColor;
   wxColour gridColor;
   wxPen gridPen;
   
   int axisLabelSize;
   
   std::vector<TsPlotCurve *> data;
   std::vector<const std::vector<int>*> penUpLocations;
   
   wxPen *plotPens;
   std::vector <int> *plotDependent;
   std::vector <wxString> names;
   std::string filename;
   wxString plotTitle;
   wxString xLabel;
   wxString yLabel;
   double plotXMin;
   double plotXMax;
   double plotYMin;
   double plotYMax;
   double currentXMin;
   double currentXMax;
   double currentYMin;
   double currentYMax;
   double zoomXMin;
   double zoomXMax;
   double zoomYMin;
   double zoomYMax;
   double xScale;
   double yScale;
   int xticks;
   int yticks;
   int xMinorTicks;
   int yMinorTicks;
   // This construct doesn't work in Visual C++???  So switched to static arrays
   //std::vector <int> xGridLoc;
   //std::vector <int> yGridLoc;
   int xGridLoc[64];
   int yGridLoc[64];
   int tickSize;
   int minorTickSize;
   int varCount;
   
   double xMin;
   double xMax;
   double yMin;
   double yMax;
   std::string xName, yName;
   
   int currentId;
   bool hasData;
   bool rescaled;
   bool zoomed;
   bool zooming;
   bool movingLegend;
   bool dataUpdated;
   
   bool overrideXMin;
   bool overrideXMax;
   bool overrideYMin;
   bool overrideYMax;
   
   double userXMin;
   double userXMax;
   double userYMin;
   double userYMax;
   
   // Error bar properties for the 13sigma error bars
   bool showsigmaLine;
   bool show1sigmaTick;
   bool show2sigmaTick;
   bool show3sigmaTick;

   // Appearance parms
   bool showTitle;
   bool labelAxes;
   bool hasGrid;
   bool hasLegend;
   bool allowPlotOptions;
   bool initializeLegendLoc;
   bool alwaysDraw;

   long zoomLeft, zoomTop, zoomWidth, zoomHeight;
   int  xLabelPrecision, yLabelPrecision;
   
   virtual void DrawAxes(wxDC &dc) = 0;
   virtual void DrawLabels(wxDC &dc) = 0;
   virtual void PlotData(wxDC &dc) = 0;
   virtual void Rescale(wxDC &dc) = 0;
   
   virtual void DrawMarker(wxDC &dc, int style, int markerSize, int x0, int y0,
         wxPen &thePen, int hi = -1, int low = -1);

   virtual void DrawGrid(wxDC &dc);
   virtual void ToggleGrid(wxCommandEvent& event);
   virtual void DrawLegend(wxDC &dc);
   virtual void ToggleLegend(wxCommandEvent& event);

   virtual void SetOptions(wxCommandEvent& event);
   virtual void ResetRanges();
   void SaveData(wxCommandEvent& event);
   void SaveImage(wxCommandEvent& event);

   virtual void Zoom(wxRect &region);
   virtual void UnZoom();
   
   virtual int GetXLocation(double val, double val2 = 0.0);
   virtual int GetYLocation(double val, double val2 = 0.0);
   virtual double GetActualXValue(int x, int y = 0);
   virtual double GetActualYValue(int y, int x = 0);

   // Statics to let us set global properties
   static int  xSensitivity, ySensitivity;
   static int  defaultLineWidth;
   static bool defaultLabels;

   /// Rectangle with screen coordinates of the plot area
   wxRect  plotArea;
   /// Rectangle used when dragging the mouse
   wxRect  mouseRect;
   /// Legend
   wxRect  legendRect;
   /// Number of columns in the legend (0 to make it 1 row)
   int legendColumns;
   /// Flag triggered by a size event
   bool resized;
   /// Counter for drawall calls used when repainting a hidden window
   int drawAllCounter;

//private:
   DECLARE_EVENT_TABLE()
};

#endif //_TSPLOTCANVAS_HPP_
