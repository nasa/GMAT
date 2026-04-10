//$Id$
//------------------------------------------------------------------------------
//                              TSPlotCurve
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotCurves used on TsPlotCanvas's.
 */
//------------------------------------------------------------------------------
#ifndef TsPlotCurve_hpp
#define TsPlotCurve_hpp

#include <wx/wx.h>

// Forward reference for friend class
class TsPlotCanvas;

// Interpolators are currently turned off in this code
//#include "Interpolator.hpp"

#include <vector>
#include "TsPlotDefs.hpp"


class TsPlotCurve
{
public:
   TsPlotCurve();
   virtual ~TsPlotCurve();
   
   void AddData(double x, double y, double high = 0.0, double low = 0.0);
   bool RegionSizeChanged(bool reset = true);
   void SetColour(wxColour rgb, int where = 0);
   wxColour GetColour(int whichOne = 0);
   void DarkenColour(int darkeningFactor = 1);
   void LightenColour(int lighteningFactor = 1);
   void SetWidth(int w);
   int  GetWidth();
   void SetStyle(int ls);
   int  GetStyle();
   void HighlightPoint(int index);
   
   bool UseLine();
   bool UseLine(bool tf);
   bool AddBreak(int where);
   void BreakAndDiscard(int startBreakIndex, int endBreakIndex = -1);
   bool UseMarker();
   bool UseMarker(bool tf);
   int  GetMarker(int whichOne = 0);
   void SetMarker(MarkerType newType, int where = 0);
   int  GetMarkerSize();
   void SetMarkerSize(int newSize);

   bool UseHiLow();
   bool UseHiLow(bool useHiLow);

   virtual void   Clear();

   virtual double GetMinX();
   virtual double GetMaxX();
   virtual double GetY(double x);
   virtual double GetMinY();
   virtual double GetMaxY();

   virtual void PenUp();
   virtual void PenDown();
   
   const std::vector<int>* GetPenUpLocations();
   const std::vector<int>* GetColorChangeLocations();
   const std::vector<int>* GetMarkerChangeLocations();
   const std::vector<int>* GetHighlightPoints();

   virtual void Rescale();

protected:
   double minX;
   double maxX;
   double minY; 
   double maxY;
   
   bool   rangeChanged;
   bool   domainChanged;
   
   bool   useLines;
   bool   useMarkers;
   int    currentMarkerStyle;
   int    markerSize;
   bool   showHiLow;

   // Interpolator *mInterp;
   
   /// Independent data
   std::vector<double>  abscissa;
   /// Dependent data
   std::vector<double>  ordinate;
   /// Data used for top of error bars
   std::vector<double>  highError;
   /// Data used for bottom of error bars
   std::vector<double>  lowError;
   /// Location for "pen up" commands
   std::vector<int>     penUpIndex;
   /// Current base line color (for darkening and lightening)
   wxColour             baseColor;
   /// vector of line colors
   std::vector<wxColour> linecolor;
   /// Location for color changes
   std::vector<int>     colorIndex;
   /// Location for line breaks (points where data my be discarded)
   std::vector<int>     breakIndex;
   /// List of marker styles used; first entry is style on the legend
   std::vector<MarkerType> markerStyles;
   /// Location for marker style changes
   std::vector<int>     markerIndex;
   /// Location for highlighted points
   std::vector<int>     highlightIndex;
   /// Line width.  This parameter is set for the entire line
   int                  lineWidth;
   /// Line style.  This parameter is set for the entire line
   int                  lineStyle;
   /// Flag indicating that the curve is accepting data
   bool                 penIsDown;

   /// Index of the last point that was plotted
   unsigned int         lastPointPlotted;
   
   friend class TsPlotCanvas; 
   friend class TsPlotXYCanvas; 
   friend class TsPlotTimelineCanvas; 
   friend class TsPlotPolarCanvas; 
};

#endif
