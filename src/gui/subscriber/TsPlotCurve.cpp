//$Id$
//------------------------------------------------------------------------------
//                              TsPlotCurve
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Implements the TsPlotCurves used on TsPlotCanvas's.
 */
//------------------------------------------------------------------------------
 

#include "TsPlotCurve.hpp"
//#include "LinearInterpolator.hpp"
#include "MessageInterface.hpp"
#include <algorithm>


//#define DEBUG_PENUP_PENDOWN
//#define DEBUG_MESSSAGE_FLOW
//#define DEBUG_REPEATED_POINTS


// #define TEST_POINT_22  // Tests curve change features starting at point 22

//------------------------------------------------------------------------------
// TsPlotCurve()
//------------------------------------------------------------------------------
TsPlotCurve::TsPlotCurve() :
   minX              (1e99),
   maxX              (-1e99),
   minY              (1e99),
   maxY              (-1e99),
   rangeChanged      (false),
   domainChanged     (false),
   useLines          (true),
   useMarkers        (false),
   currentMarkerStyle(unsetMarker),
   markerSize        (3),
   showHiLow         (false),
   baseColor         (0xff,0x00,0x00),
   lineWidth         (3),
   lineStyle         (wxSOLID),
   penIsDown         (true),
   lastPointPlotted  (0)
{
//   mInterp = new LinearInterpolator();
   ordinate.clear();
   abscissa.clear();
}


//------------------------------------------------------------------------------
// ~TsPlotCurve()
//------------------------------------------------------------------------------
TsPlotCurve::~TsPlotCurve()
{
   ordinate.clear();
   abscissa.clear();

//   delete mInterp;
}


//------------------------------------------------------------------------------
// void AddData(double x, double y)
//------------------------------------------------------------------------------
void TsPlotCurve::AddData(double x, double y, double high, double low)
{
   #ifdef DEBUG_PENUP_PENDOWN
      static Integer counter = 0;
      if (abscissa.size() == 0) 
         counter = 0;
         
      if (abscissa.size() == 80 && counter == 0)
      {
         counter = 1;
         MessageInterface::ShowMessage("Pen up\n");
         PenUp();
      }
      if (counter == 1) 
      {
         ++counter;
         MessageInterface::ShowMessage("Pen down\n");
         PenDown();
      }
   #endif
   
   if (penIsDown)
   {
      // Include error bars in top and bottom values
      Real yt = y + high, yb = y - (low == 0.0 ? high : low);

      double lx, ly;
      int asize = abscissa.size();

      if (asize == 0)
      {
         #ifdef DEBUG_FIRST_POINT
            MessageInterface::ShowMessage("Adding initial data: [%lf, %lf]",
               x, y);
            if (errorDataPresent)
               MessageInterface::ShowMessage(" with error bars: [%le, %le]\n",
                     high, low);
            MessageInterface::ShowMessage("\n");
         #endif
         
         minX = maxX = x;
         maxY = yt;
         minY = yb;
         rangeChanged = true;
         domainChanged = true;
         lx = x - 1.0;
         ly = y - 1.0;
      }
      else
      {
         lx = abscissa[asize-1];
         ly = ordinate[asize-1];
      }

      // Only add points that do not match the previous point
      if (!((x == lx) && (y == ly)))
      {
         abscissa.push_back(x);
         ordinate.push_back(y);
         if (high > 0.0)
         {
            highError.push_back(high);
            if (low > 0.0)
               lowError.push_back(low);
         }

         if (x < minX)
         {
            minX = x;
            domainChanged = true;
         }
         if (x > maxX)
         {
            maxX = x;
            domainChanged = true;
         }
         if (yb < minY)
         {
            minY = yb;
            rangeChanged = true;
         }
         if (yt > maxY)
         {
            maxY = yt;
            rangeChanged = true;
         }
      }
      #ifdef DEBUG_REPEATED_POINTS
         else
         {
            MessageInterface::ShowMessage("Omitting [%le %le] because it matches "
                  "[%le %le]\n", x, y, lx, ly);
         }
      #endif
      #if DEBUG_XY_PLOT_CURVE_ADD
         MessageInterface::ShowMessage
            ("TsPlotCurve::AddData() size = %d, x = %lf, y = %lf\n",
             abscissa.size(), abscissa.back(), ordinate.back());
      #endif
   }
}

//------------------------------------------------------------------------------
// bool RegionSizeChanged(bool reset)
//------------------------------------------------------------------------------
/**
 * Method queried to determine if the plot needs erasing before redrawing
 *
 * @param reset Resets the flags if desired (default is to reset)
 *
 * @return true if the X or Y axes is rescaled because of new data
 */
//------------------------------------------------------------------------------
bool TsPlotCurve::RegionSizeChanged(bool reset)
{
   bool retval = false;
   if (rangeChanged || domainChanged)
      retval = true;
   if (retval && reset)
   {
      rangeChanged = false;
      domainChanged = false;
   }
   return retval;
}

////------------------------------------------------------------------------------
//// void SetInterpolator(Interpolator *interp)
////------------------------------------------------------------------------------
//void TsPlotCurve::SetInterpolator(Interpolator *interp)
//{
//   if (mInterp == NULL)
//      mInterp = interp;
//}


//------------------------------------------------------------------------------
// double GetStartX()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMinX()
{
      return minX;
}

//------------------------------------------------------------------------------
// double GetEndX()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMaxX()
{
      return maxX;
}


//------------------------------------------------------------------------------
// virtual double GetY(wxInt32 x)
//------------------------------------------------------------------------------
double TsPlotCurve::GetY(double x)
{
   double yVal = 0.0;
   unsigned int i;
    
   for (i=0; i < abscissa.size(); i++)
   {
      if (abscissa[i] == x)
      {
         yVal = ordinate[i];
         break;
      }
      else if (abscissa[i] > x)
      {
         if (i == 0)
            ++i;
         yVal = ordinate[i-1];
         // interpolate
//         mInterp->Clear();
//         mInterp->AddPoint(mXdata[i-1], &mYdata[i-1]);
//         mInterp->AddPoint(mXdata[i], &mYdata[i]);
//         mInterp->Interpolate(x, &yVal);
         break;
      }
   }
    
   #if DEBUG_XY_PLOT_CURVE_GET
      MessageInterface::ShowMessage
         ("TsPlotCurve::GetY() size = %d, x = %lf, y = %lf\n",
          abscissa.size(), x, yVal);
   #endif
   
   return yVal;
}


//------------------------------------------------------------------------------
// virtual void Clear()
//------------------------------------------------------------------------------
void TsPlotCurve::Clear()
{
   abscissa.clear();
   ordinate.clear();
   penUpIndex.clear();
   colorIndex.clear();
   markerIndex.clear();

   highlightIndex.clear();


   wxColour def = linecolor[0];
   linecolor.clear();
   linecolor.push_back(def);
   colorIndex.push_back(0);

   #ifdef TEST_POINT_22
      // Go cyan from point 22 to 32
      SetColour(*wxCYAN, 22);
      SetColour(def, 32);
      // Mark these points as a check
      HighlightPoint(22);
      HighlightPoint(32);
   #endif

   currentMarkerStyle = markerStyles[0];
   markerStyles.clear();
   markerStyles.push_back((MarkerType)currentMarkerStyle);
   markerIndex.push_back(0);

   #ifdef TEST_POINT_22
      // Make an x from 22 - 27
      markerStyles.push_back(xMarker);
      markerIndex.push_back(22);
      markerStyles.push_back((MarkerType)currentMarkerStyle);
      markerIndex.push_back(27);
   #endif
}


//------------------------------------------------------------------------------
// virtual double GetYMin()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMinY()
{
   return minY;
}


//------------------------------------------------------------------------------
// virtual double GetYMax()
//------------------------------------------------------------------------------
double TsPlotCurve::GetMaxY()
{
#if DEBUG_XY_PLOT_CURVE_MAX
   MessageInterface::ShowMessage
      ("TsPlotCurve::GetYMax() size = %d, max=%f\n",
       ordinate.size(), maxY);
#endif
   
   return maxY;
}


//------------------------------------------------------------------------------
// void PenUp()
//------------------------------------------------------------------------------
void TsPlotCurve::PenUp()
{
   #ifdef DEBUG_PENUP_PENDOWN
      MessageInterface::ShowMessage("Penup at %d\n", (int)abscissa.size() - 1);
   #endif

   // Avoid repeated indices
   int lastPup = (penUpIndex.size() > 0 ? penUpIndex[penUpIndex.size()-1] : -1);
   if (lastPup != (int)abscissa.size()-1)
      penUpIndex.push_back((int)abscissa.size() - 1);

   #ifdef DEBUG_PENUP_PENDOWN
      MessageInterface::ShowMessage("PenUp buffer: [");
      for (unsigned int i = 0; i < penUpIndex.size(); ++i)
         MessageInterface::ShowMessage(" %d ", penUpIndex[i]);
      MessageInterface::ShowMessage("]\n");
   #endif
   penIsDown = false;
}

//------------------------------------------------------------------------------
// void PenDown()
//------------------------------------------------------------------------------
void TsPlotCurve::PenDown()
{
   #ifdef DEBUG_PENUP_PENDOWN
      MessageInterface::ShowMessage("Pendown at %d\n", (int)abscissa.size() - 1);
   #endif
   penIsDown = true;
}


//------------------------------------------------------------------------------
// const std::vector<int>* GetPenUpLocations()
//------------------------------------------------------------------------------
const std::vector<int>* TsPlotCurve::GetPenUpLocations()
{
   return &penUpIndex;
}

const std::vector<int>* TsPlotCurve::GetColorChangeLocations()
{
   return &colorIndex;
}

const std::vector<int>* TsPlotCurve::GetMarkerChangeLocations()
{
   return &markerIndex;
}

const std::vector<int>* TsPlotCurve::GetHighlightPoints()
{
   return &highlightIndex;
}

void TsPlotCurve::Rescale()
{
   #ifdef DEBUG_RESCALING
      MessageInterface::ShowMessage("In TsPlotCurve::Rescale, showHiLow = %s\n",
            (showHiLow == true ? "true" : "false"));
   #endif
   if (abscissa.size() > 0)
   {
      if ((!showHiLow) || (highError.size() == 0))
      {
         minX = maxX = abscissa[0];
         minY = maxY = ordinate[0];

         for (unsigned int i=1; i < abscissa.size(); i++)
         {
            if (abscissa[i] < minX)
               minX = abscissa[i];
            if (abscissa[i] > maxX)
               maxX = abscissa[i];
            if (ordinate[i] < minY)
               minY = ordinate[i];
            if (ordinate[i] > maxY)
               maxY = ordinate[i];
         }
      }
      else
      {
         minX = maxX = abscissa[0];
         minY = ordinate[0] - lowError[0];
         maxY = ordinate[0] + highError[0];

         Real hi, lo;

         for (unsigned int i=1; i < abscissa.size(); i++)
         {
            if (abscissa[i] < minX)
               minX = abscissa[i];
            if (abscissa[i] > maxX)
               maxX = abscissa[i];

            if (lowError.size() > i)
               lo = lowError[i];
            else
               lo = 0.0;
            if (highError.size() > i)
               hi = highError[i];
            else
               hi = 0.0;

            if (ordinate[i] - lo < minY)
               minY = ordinate[i] - lo;
            if (ordinate[i] + hi > maxY)
               maxY = ordinate[i] + hi;
         }
      }
   }

   #ifdef DEBUG_RESCALING
      MessageInterface::ShowMessage("ShowHiLow: %s, yMin = %le, yMax = %le\n",
            (showHiLow ? "true" : "false"), minY, maxY);
   #endif
}


void TsPlotCurve::SetColour(wxColour rgb, int where)
{
//   MessageInterface::ShowMessage("Color %s starts at %d",
//		   rgb.GetAsString(wxC2S_CSS_SYNTAX).c_str(), where);

   if (where == -1)
      where = abscissa.size();
   if ((where != 0) || (linecolor.size() == 0))
   {
      linecolor.push_back(rgb);
      colorIndex.push_back(where);
   }
   else
   {
      linecolor[0] = rgb;
   }

   baseColor = rgb;
}


wxColour TsPlotCurve::GetColour(int whichOne)
{
   if (((int)(linecolor.size()) > whichOne) && (whichOne >= 0))
   {
      wxColour thisOne = linecolor[whichOne];
   }
//   else
//   {
//      MessageInterface::ShowMessage("Getting color %d unavailable on %p\n",
//            whichOne, this);
//      MessageInterface::ShowMessage("   linecolor size = %d\n", linecolor.size());
//   }

   if (linecolor.size() == 0)
      return *wxRED;

   if (whichOne == 0)
      return linecolor[0];

   if (whichOne < (int)linecolor.size())
      return linecolor[whichOne];

   return linecolor[0];
}

void TsPlotCurve::DarkenColour(int darkeningFactor)
{
   if (darkeningFactor < 1)
      darkeningFactor = 1;

   wxColour startColor = baseColor;
   unsigned char r,g,b;

   r = startColor.Red() / darkeningFactor;
   g = startColor.Green() / darkeningFactor;
   b = startColor.Blue() / darkeningFactor;

   wxColour theColor(r, g, b);
   SetColour(theColor, -1);
   baseColor = startColor;
}

void TsPlotCurve::LightenColour(int lighteningFactor)
{
   if (lighteningFactor < 1)
      lighteningFactor = 1;

   wxColour startColor = baseColor;
   unsigned char r,g,b, rdiff, gdiff, bdiff;

   rdiff = 255 - startColor.Red();
   gdiff = 255 - startColor.Green();
   bdiff = 255 - startColor.Blue();

   r = startColor.Red() + (int)(rdiff * (1.0 - 1.0 / lighteningFactor));
   g = startColor.Green() + (int)(gdiff * (1.0 - 1.0 / lighteningFactor));
   b = startColor.Blue() + (int)(bdiff * (1.0 - 1.0 / lighteningFactor));

   wxColour theColor(r, g, b);
   SetColour(theColor, -1);
   baseColor = startColor;
}

void TsPlotCurve::SetWidth(int w)
{
   lineWidth = w;
}

int TsPlotCurve::GetWidth()
{
   return lineWidth;
}


/**
 * Line styles defined in wxWidgets
 *    wxSOLID  Solid style.
 *    wxTRANSPARENT  No pen is used.
 *    wxDOT    Dotted style.
 *    wxLONG_DASH    Long dashed style.
 *    wxSHORT_DASH   Short dashed style.
 *    wxDOT_DASH  Dot and dash style.
 *    wxSTIPPLE   Use the stipple bitmap.
 *    wxUSER_DASH    Use the user dashes: see wxPen::SetDashes.
 */
void TsPlotCurve::SetStyle(int ls)
{
   lineStyle = ls;
}


int TsPlotCurve::GetStyle()
{
   return lineStyle;
}

void TsPlotCurve::HighlightPoint(int index)
{
   if (index == -1)
      index = (abscissa.size() == 0 ? 0 : abscissa.size()-1);
   highlightIndex.push_back(index);
}


bool TsPlotCurve::UseLine()
{
   return useLines;
}

bool TsPlotCurve::UseLine(bool tf)
{
   return useLines = tf;
}

bool TsPlotCurve::AddBreak(int where)
{
   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("Adding breakpoint; input location: %d\n",
            where);
   #endif
   if (where == -1)
      where = abscissa.size();
   // Make it idempotent
   if (find(breakIndex.begin(), breakIndex.end(), where) == breakIndex.end())
      breakIndex.push_back(where);

   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("   Breakpoint list: \n");
      for (UnsignedInt i = 0; i < breakIndex.size(); ++i)
         MessageInterface::ShowMessage("      %d: %d\n", i, breakIndex[i]);
   #endif

   return true;
}

void TsPlotCurve::BreakAndDiscard(int startBreakIndex, int endBreakIndex)
{
   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("Discarding data for break %d through %d\n",
            startBreakIndex, endBreakIndex);
      MessageInterface::ShowMessage("   Pre-discard ordinate size: %d  "
            "abscissa: %d\n", ordinate.size(), abscissa.size());
   #endif

   // Clear the penup buffer
   if ((startBreakIndex == -1) && (endBreakIndex == -1))
      penUpIndex.clear();

   if (breakIndex.size() == 0)
      return;

   if (startBreakIndex == -1)
      startBreakIndex = breakIndex.size()-1;

   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("StartBreakIndex is now %d; index size "
            "is %d", startBreakIndex, breakIndex.size());
   #endif
   if (startBreakIndex < (int)breakIndex.size())
   {
      // Remove points from here to endBreakIndex, or to end
      if (endBreakIndex == -1)
      {
         std::vector<double>::iterator start = ordinate.begin();
         advance(start, breakIndex[startBreakIndex]);
         ordinate.erase(start, ordinate.end());
         start = abscissa.begin();
         advance(start, breakIndex[startBreakIndex]);
         abscissa.erase(start, abscissa.end());
      }
      else
      {

      }
   }

   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("   Postdiscard ordinate size: %d  "
            "abscissa: %d\n", ordinate.size(), abscissa.size());
   #endif
}

bool TsPlotCurve::UseMarker()
{
   return useMarkers;
}


bool TsPlotCurve::UseMarker(bool tf)
{
   return useMarkers = tf;
}


int TsPlotCurve::GetMarker(int whichOne)
{
   if (markerStyles.size() == 0)
      return unsetMarker;

   if (whichOne == 0)
      return markerStyles[0];

   if (whichOne < (int)markerStyles.size())
      currentMarkerStyle = markerStyles[whichOne];
   return currentMarkerStyle;
}

void TsPlotCurve::SetMarker(MarkerType newType, int where)
{
   if (where > 0)
   {
      markerIndex.push_back(where);
      markerStyles.push_back(newType);
   }
   else
   {
      if (markerIndex.size() == 0)
      {
         markerIndex.push_back(0);
         markerStyles.push_back(newType);
         currentMarkerStyle = newType;
      }
      else
      {
         markerIndex[0]  = 0;
         markerStyles[0] = newType;
         currentMarkerStyle = newType;
      }
   }
}

int TsPlotCurve::GetMarkerSize()
{
   return markerSize;
}

void TsPlotCurve::SetMarkerSize(int newSize)
{
   if (newSize > 0)
      markerSize = newSize;

   if (newSize > 12)
      markerSize = 12;
}

bool TsPlotCurve::UseHiLow()
{
   return showHiLow;
}

bool TsPlotCurve::UseHiLow(bool useHiLow)
{
   showHiLow = useHiLow;
   return showHiLow;
}
