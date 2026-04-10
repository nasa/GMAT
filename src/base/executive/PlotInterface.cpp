//$Id$
//------------------------------------------------------------------------------
//                             PlotInterface
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/12/18
//
/**
 * Implements PlotInterface class.
 * This class updates OpenGL canvas, XY plot window
 */
//------------------------------------------------------------------------------

#include "PlotInterface.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PLOTIF_GL 1
//#define DEBUG_PLOTIF_GL_CREATE 1
//#define DEBUG_PLOTIF_GL_DELETE 1
//#define DEBUG_PLOTIF_GL_UPDATE 1
//#define DEBUG_PLOTIF_XY 1
//#define DEBUG_PLOTIF_XY_UPDATE 1
//#define DEBUG_RENAME 1

//---------------------------------
//  static data
//---------------------------------
PlotReceiver *PlotInterface::thePlotReceiver = NULL;

//---------------------------------
//  public functions
//---------------------------------


//------------------------------------------------------------------------------
// void SetPlotReceiver(PlotReceiver *pr)
//------------------------------------------------------------------------------
void PlotInterface::SetPlotReceiver(PlotReceiver *pr)
{
   thePlotReceiver = pr;
}


//------------------------------------------------------------------------------
//  PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::PlotInterface()
{
}


//------------------------------------------------------------------------------
//  ~PlotInterface()
//------------------------------------------------------------------------------
PlotInterface::~PlotInterface()
{
}


//------------------------------------------------------------------------------
//  bool CreateGlPlotWindow(const std::string &plotName, ...)
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 * @param <oldName>  old plot name, this is needed for renaming plot
 * @param <x, y>     position of the plot in screen coordinates
 * @param <w, h>     size of the plot in screen coordinates
 * @param <drawEcPlane>  true if draw ecliptic plane
 * @param <drawXyPlane>  true if draw XY plane
 * @param <drawWirePlane>  true if draw wire frame
 * @param <drawAxes>  true if draw axes
 * @param <drawGrid>  true if draw grid
 * @param <drawSunLine>  true if draw earth sun lines
 * @param <overlapPlot>  true if overlap plot without clearing the plot
 * @param <usevpInfo>  true if use viewpoint info to draw plot
 * @param <numPtsToRedraw>  number of points to redraw during the run
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGlPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       Real positionX, Real positionY,
                                       Real width, Real height, bool isMaximized,
                                       Integer numPtsToRedraw)
{
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PI::CreateGlPlotWindow() %s entered, x = %12.10f, y = %12.10f, w = %12.10f, h = %12.10f,"
            "thePlotReceiver=<%p>\n", plotName.c_str(), positionX, positionY, width, height,
       thePlotReceiver);
   #endif
   
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateGlPlotWindow(plotName, oldName, positionX, positionY, width, height, isMaximized, numPtsToRedraw);
   
   return false;
}


//------------------------------------------------------------------------------
// void SetViewType(GmatPlot::ViewType view)
//------------------------------------------------------------------------------
void PlotInterface::SetViewType(GmatPlot::ViewType view)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetViewType(view);
}


//------------------------------------------------------------------------------
// void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
//------------------------------------------------------------------------------
void PlotInterface::SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlSolarSystem(plotName, ss);
}


//------------------------------------------------------------------------------
// void SetGlObject(const std::string &plotName,  ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlObject(const std::string &plotName,
                                const StringArray &objNames,
                                //const UnsignedIntArray &objOrbitColors,
                                const std::vector<SpacePoint*> &objArray)
{
   if (thePlotReceiver != NULL)
      //thePlotReceiver->SetGlObject(plotName, objNames, objOrbitColors, objArray);
      thePlotReceiver->SetGlObject(plotName, objNames, objArray);
}


//------------------------------------------------------------------------------
// static void SetGlCoordSystem(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlCoordSystem(const std::string &plotName,
                                     CoordinateSystem *internalCs,
                                     CoordinateSystem *viewCs,
                                     CoordinateSystem *viewUpCs)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlCoordSystem(plotName, internalCs, viewCs, viewUpCs);
}


//------------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &plotName, Integer footPrintOption)
//------------------------------------------------------------------------------
void PlotInterface::SetGl2dDrawingOption(const std::string &plotName,
                                         const std::string &centralBodyName,
                                         const std::string &textureMap,
                                         Integer footPrintOption)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->
         SetGl2dDrawingOption(plotName, centralBodyName, textureMap, footPrintOption);
}


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(const std::string &plotName, bool drawEcPlane, ...)
//------------------------------------------------------------------------------
void PlotInterface::SetGl3dDrawingOption(const std::string &plotName,
                                         bool showLabels, bool drawEcPlane,
                                         bool drawXyPlane, bool drawWireFrame,
                                         bool drawAxes, bool drawGrid, bool drawSunLine,
                                         bool overlapPlot, bool usevpInfo,
                                         bool drawStars, bool drawConstellations,
                                         Integer starCount)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->
         SetGl3dDrawingOption(plotName, showLabels, drawEcPlane, drawXyPlane,
                              drawWireFrame, drawAxes, drawGrid, drawSunLine,
                              overlapPlot, usevpInfo, drawStars, drawConstellations,
                              starCount);
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(const std::string &plotName, SpacePoint *vpRefObj, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGl3dViewOption(const std::string &plotName,
                                      SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                      SpacePoint *vdObj, Real vsFactor,
                                      const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                      const Rvector3 &vdVec, const std::string &upAxis,
                                      bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGl3dViewOption(plotName, vpRefObj, vpVecObj, vdObj, 
            vsFactor, vpRefVec, vpVec, vdVec, upAxis, usevpRefVec, usevpVec, 
            usevdVec);
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlDrawOrbitFlag(const std::string &plotName,
                                       const std::vector<bool> &drawArray)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlDrawOrbitFlag(plotName, drawArray);
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void PlotInterface::SetGlShowObjectFlag(const std::string &plotName,
                                        const std::vector<bool> &showArray)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlShowObjectFlag(plotName, showArray);
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq)
//------------------------------------------------------------------------------
void PlotInterface::SetGlUpdateFrequency(const std::string &plotName,
                                         Integer updFreq)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGlUpdateFrequency(plotName, updFreq);
}


//------------------------------------------------------------------------------
//  bool IsThere(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Checks if OpenGlPlot exist.
 */
//------------------------------------------------------------------------------
bool PlotInterface::IsThere(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->IsThere(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
//  bool InitializeGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot by plot name.
 *
 * @param <plotName> name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::InitializeGlPlot(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->InitializeGlPlot(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
//  bool RefreshGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool PlotInterface::RefreshGlPlot(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->RefreshGlPlot(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
//  bool DeleteGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot by plot name.
 *
 * @param <plotName> name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteGlPlot(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteGlPlot(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
// bool SetGlEndOfRun(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Sets end of run flag to OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool PlotInterface::SetGlEndOfRun(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->SetGlEndOfRun(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
//  bool UpdateGlPlot(const std::string &plotName, ...
//------------------------------------------------------------------------------
/*
 * Buffers data and updates OpenGL plow window if updateCanvas is true
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateGlPlot(const std::string &plotName,
                                 const std::string &oldName,
                                 const StringArray &scNames, const Real &time,
                                 const RealArray &posX, const RealArray &posY,
                                 const RealArray &posZ, const RealArray &velX,
                                 const RealArray &velY, const RealArray &velZ,
                                 //const UnsignedIntArray &scColors,
                                 const ColorMap &orbitColorMap,
                                 const ColorMap &targetColorMap,
                                 bool solving, Integer solverOption,
                                 bool updateCanvas, bool drawing, bool inFunction)
{
   #if DEBUG_PLOTIF_GL_UPDATE
   MessageInterface::ShowMessage
      ("PI::UpdateGlPlot() '%s' entered, thePlotReceiver=<%p>\n", plotName.c_str(),
       thePlotReceiver);
   #endif
   
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateGlPlot(plotName, oldName, scNames, time,
                                           posX, posY, posZ, velX, velY, velZ,
                                           //scColors,
                                           orbitColorMap, targetColorMap,
                                           solving, solverOption, 
                                           updateCanvas, drawing, inFunction);
   
   return false;
} // end UpdateGlPlot()


//------------------------------------------------------------------------------
// bool TakeGlAction(const std::string &plotName, const std::string &action)
//------------------------------------------------------------------------------
bool PlotInterface::TakeGlAction(const std::string &plotName,
                                 const std::string &action)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->TakeGlAction(plotName, action);
   
   return false;
}


//------------------------------------------------------------------------------
// void PlotInterface::SetMaxGlDataPoints(const std::string &plotName,
//                                        Integer maxDataPoints)
//------------------------------------------------------------------------------
/**
* Passes a custom amount of data points to plot
*
* @param PlotName Name of the plot receiving the action
* @param maxDataPoints The custom max setting of data points
*/
//------------------------------------------------------------------------------
void PlotInterface::SetMaxGlDataPoints(const std::string &plotName,
                                       Integer maxDataPoints)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetMaxGlDataPoints(plotName, maxDataPoints);
}


//------------------------------------------------------------------------------
//  bool CreateXyPlotWindow(const std::string &plotName,
//                          const std::string &oldName,
//                          const std::string &plotTitle,
//                          const std::string &xAxisTitle,
//                          const std::string &yAxisTitle,
//                          bool drawGrid = false)
//------------------------------------------------------------------------------
/*
 * Creates a XyPlot window.
 *
 * @param plotName Name of the plot
 * @param oldName Former name of the plot
 * @param x, y  position of the plot in screen coordinates
 * @param w, h  size of the plot in screen coordinates
 * @param plotTitle Title of the plot
 * @param xAxisTitle X-axis label for the plot
 * @param yAxisTitle Y-axis label for the plot
 * @param drawGrid Flag indicating if the grid lines should be drawn
 *
 * @return true on success, false is no plot was created
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateXyPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       Real positionX, Real positionY,
                                       Real width, Real height, bool isMaximized,
                                       const std::string &plotTitle,
                                       const std::string &xAxisTitle,
                                       const std::string &yAxisTitle,
                                       bool drawGrid, bool canSave)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateXyPlotWindow(plotName, oldName, positionX, positionY, width, height, isMaximized, plotTitle,
            xAxisTitle, yAxisTitle, drawGrid, canSave);
   
   return false;
}


//------------------------------------------------------------------------------
//  bool DeleteXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes a XyPlot by plot name.
 *
 * @param plotName name of plot to be deleted
 *
 * @return true on success, false is no plot was deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlot(const std::string &plotName)
{    
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteXyPlot(plotName);
   
   return false;
}


//------------------------------------------------------------------------------
// bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
//                     const std::string &curveTitle,
//                     UnsignedInt penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to an XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param curveIndex The index for the curve
 * @param curveTitle Label for the curve
 * @param penColor Default color for the curve
 *
 * @return true on success, false is no curve was added
 */
//------------------------------------------------------------------------------
bool PlotInterface::AddXyPlotCurve(const std::string &plotName, int curveIndex,
                                   const std::string &curveTitle,
                                   UnsignedInt penColor)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->AddXyPlotCurve(plotName, curveIndex, curveTitle, penColor);
   
   return false;
}

//------------------------------------------------------------------------------
// bool DeleteAllXyPlotCurves(const std::string &plotName,
//                            const std::string &oldName)
//------------------------------------------------------------------------------
/*
 * Deletes all plot curves in XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param oldName The previous name of the plot that receives the new curve
 *
 * @return true on success, false if no action was taken
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteAllXyPlotCurves(const std::string &plotName,
                                          const std::string &oldName)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteAllXyPlotCurves(plotName, oldName);
      
   return false;
}


//------------------------------------------------------------------------------
// bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
//------------------------------------------------------------------------------
/*
 * Deletes a plot curve to XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param curveIndex Index of the curve that is to be deleted
 *
 * @return true on success, false if no curve was deleted
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteXyPlotCurve(plotName, curveIndex);
      
   return false;
}


//------------------------------------------------------------------------------
// void ClearXyPlotData(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Removes all data from the plot curves, leaving the curve containers in place
 * but empty.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void PlotInterface::ClearXyPlotData(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->ClearXyPlotData(plotName);
}


//------------------------------------------------------------------------------
// void XyPlotPenUp(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Tells a plot to stop drawing received data.  This method is idempotent.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotPenUp(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotPenUp(plotName);
}

//------------------------------------------------------------------------------
// void XyPlotPenDown(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Tells a plot to resume drawing received data.  This method is idempotent.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotPenDown(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotPenDown(plotName);
}


//------------------------------------------------------------------------------
// void XyPlotDarken(const std::string &plotName, Integer factor,
//          Integer index = -1, Integer curveNumber = -1)
//------------------------------------------------------------------------------
/**
 * Darkens a curve or plot by a specified amount
 *
 * @param plotName The plot that has the curves that need to darken
 * @param factor The darkening factor
 * @param index The starting point on the curve or curves that need darkening
 *              (-1 for the next point)
 * @param curveNumber When darkening a single curve, its index (-1 for all
 *                    curves on the plot)
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotDarken(const std::string &plotName, Integer factor,
         Integer index, Integer curveNumber)
{
   #ifdef DEBUG_XYSTYLE_CHANGES
         MessageInterface::ShowMessage("XyPlotDarken(%s, %d, %d, %d) called\n",
               plotName.c_str(), factor, index, curveNumber);
   #endif

   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotDarken(plotName, factor, index, curveNumber);
}

//------------------------------------------------------------------------------
// void XyPlotLighten(const std::string &plotName, Integer factor,
//          Integer index = -1, Integer curveNumber = -1)
//------------------------------------------------------------------------------
/**
 * Lightens a curve or plot by a specified amount
 *
 * @param plotName The plot that has the curves that need to lighten
 * @param factor The lightening factor
 * @param index The starting point on the curve or curves that need lightening
 *              (-1 for the next point)
 * @param curveNumber When lightening a single curve, its index (-1 for all
 *                    curves on the plot)
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotLighten(const std::string &plotName, Integer factor,
         Integer index, Integer curveNumber)
{
   #ifdef DEBUG_XYSTYLE_CHANGES
         MessageInterface::ShowMessage("XyPlotLighten(%s, %d, %d, %d) called\n",
               plotName.c_str(), factor, index, curveNumber);
   #endif

   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotLighten(plotName, factor, index, curveNumber);
}

//------------------------------------------------------------------------------
// void XyPlotChangeWidth(const std::string &plotName, Integer index = -1,
//       Integer newWidth = 1, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Changes the line width for a curve.
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new width (currently
 *              not used)
 * @param newWidth The new width
 * @param forCurve The index of the curve that is changing width.
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotChangeWidth(const std::string &plotName,
      Integer index, Integer newWidth, int forCurve)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotChangeWidth(plotName, index, newWidth, forCurve);
}

//------------------------------------------------------------------------------
// void XyPlotChangeStyle(const std::string &plotName, Integer index = -1,
//       Integer newStyle = 100, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Changes the line style for a curve.
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new width (currently
 *              not used)
 * @param newStyle The new line style
 * @param forCurve The index of the curve that is changing style.
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotChangeStyle(const std::string &plotName,
      Integer index, Integer newStyle, int forCurve)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotChangeStyle(plotName, index, newStyle, forCurve);
}

//------------------------------------------------------------------------------
// void XyPlotMarkPoint(const std::string &plotName, Integer index,
//       Integer curveNumber)
//------------------------------------------------------------------------------
/**
 * Marks a specific point on a specific curve of a XyPlot with an oversized X
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the point that gets marked
 * @param curveNumber The index of the curve containing the point to mark.  Set
 *                    curveNumber to -1 to mark all curves.
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotMarkPoint(const std::string &plotName, Integer index,
      Integer curveNumber)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotMarkPoint(plotName, index, curveNumber);
}


//------------------------------------------------------------------------------
// void XyPlotMarkBreak(const std::string &plotName, Integer index = -1,
//          Integer curveNumber = -1)
//------------------------------------------------------------------------------
/**
 * Marks a specific point on a specific curve as a point where the curve may be
 * broken
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the break point (-1 for the current point)
 * @param curveNumber The index of the curve containing the point to break
 *                    (-1 to mark all curves)
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotMarkBreak(const std::string &plotName, Integer index,
         Integer curveNumber)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotMarkBreak(plotName, index, curveNumber);
}


//------------------------------------------------------------------------------
// void XyPlotClearFromBreak(const std::string &plotName,
//          Integer startBreakNumber, Integer endBreakNumber = -1,
//          Integer curveNumber = -1)
//------------------------------------------------------------------------------
/**
 * Breaks a curve at a break point, discarding the data between that break point
 * and a subsequent break point
 *
 * @param plotName The plot that contains the curve
 * @param startBreakNumber The index of the starting break point (-1 to start at
 *                         the current point)
 * @param endBreakNumber The index of the end break point (-1 to break to the
 *                       end of the curve)
 * @param curveNumber The index of the curve containing the point to break
 *                    (-1 to mark all curves)
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotClearFromBreak(const std::string &plotName,
      Integer startBreakNumber, Integer endBreakNumber, Integer curveNumber)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotClearFromBreak(plotName, startBreakNumber,
            endBreakNumber, curveNumber);
}


//------------------------------------------------------------------------------
// void XyPlotRescale(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Sends a rescale message to the plot
 *
 * @param plotName The plot that is to be rescaled
 */
//------------------------------------------------------------------------------
void PlotInterface::XyPlotRescale(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotRescale(plotName);
}


//------------------------------------------------------------------------------
// void XyPlotCurveSettings(const std::string &plotName, bool useLines,
//       Integer lineWidth, Integer lineStyle, bool useMarkers,
//       Integer markerSize, Integer marker, bool useHiLow, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Sets the default settings for a curve
 *
 * @param plotName The name of the plot that contains the curve
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
void PlotInterface::XyPlotCurveSettings(const std::string &plotName,
      bool useLines, Integer lineWidth, Integer lineStyle, bool useMarkers,
      Integer markerSize, Integer marker, bool useHiLow, Integer forCurve)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->XyPlotCurveSettings(plotName, useLines, lineWidth,
            lineStyle, useMarkers, markerSize, marker, useHiLow, forCurve);
}


//------------------------------------------------------------------------------
// void SetXyPlotTitle(const std::string &plotName,
//       const std::string &plotTitle)
//------------------------------------------------------------------------------
/**
 * Sets the title for a plot
 *
 * @param plotName The name of the plot
 * @param plotTitle The new title for the plot
 */
//------------------------------------------------------------------------------
void PlotInterface::SetXyPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetXyPlotTitle(plotName, plotTitle);
}


//------------------------------------------------------------------------------
// void ShowXyPlotLegend(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Turns on display of the plot legend
 *
 * @param plotName The name of the plot
 */
//------------------------------------------------------------------------------
void PlotInterface::ShowXyPlotLegend(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->ShowXyPlotLegend(plotName);
}


//------------------------------------------------------------------------------
// bool RefreshXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes all plot curves on a plot
 *
 * @param plotName The name of the plot
 *
 * @return true on success, false if nothing was refreshed
 */
//------------------------------------------------------------------------------
bool PlotInterface::RefreshXyPlot(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->RefreshXyPlot(plotName);
      
   return false;
}


//------------------------------------------------------------------------------
// bool UpdateXyPlot(const std::string &plotName, const std::string &oldName,
//                   const Real &xval, const Rvector &yvals,
//                   const std::string &plotTitle,
//                   const std::string &xAxisTitle,
//                   const std::string &yAxisTitle, bool updateCanvas,
//                   bool drawGrid)
//------------------------------------------------------------------------------
/*
 * Updates an XY plot window.
 *
 * @param plotName name of the xy plot
 * @param oldName Former name of the plot, or an empty string
 * @param xval x value
 * @param yvals y values, should be in the order of curve added
 * @param plotTitle The plot's Title
 * @param xAxisTitle The plot's X axis title
 * @param yAxisTitle The plot's Y axis title
 * @param updateCanvas Flag indicating if the canvas should update immediately
 * @param drawGrid flag indicating if the grid should be drawn
 *
 * @return true if an update occurred, false otherwise
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateXyPlot(const std::string &plotName,
                                 const std::string &oldName,
                                 const Real &xval, const Rvector &yvals,
                                 const std::string &plotTitle,
                                 const std::string &xAxisTitle,
                                 const std::string &yAxisTitle,
                                 Integer solverOption, bool updateCanvas,
                                 bool drawGrid)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateXyPlot(plotName, oldName, xval, yvals,
            plotTitle, xAxisTitle, yAxisTitle, updateCanvas, drawGrid);
      
   return false;
}


//------------------------------------------------------------------------------
// bool UpdateXyPlotData(const std::string &plotName, const Real &xval,
//       const Rvector &yvals, const Rvector &hiError, const Rvector &lowError)
//------------------------------------------------------------------------------
/**
 * Updates the data on a plot, passing in a set of y values for a given x, and
 * optionally the data used to draw error bars
 *
 * @param plotName The name of the plot receiving the data
 * @param xval The X value associated with the points
 * @param yvals The Y values associated with the points; these are assigned to
 *              the curves indexed in the order contained in the array
 * @param hiError +sigma error data for the error bars
 * @param lowError -sigma error for the error bars; if no data is contained in
 *                 this array, the low error is assumed to have the same
 *                 magnitude as the high error
 *
 * @return true if the data was processed, false if not
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateXyPlotData(const std::string &plotName,
                             const Real &xval, const Rvector &yvals,
                             const Rvector &hiError, const Rvector &lowError)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateXyPlotData(plotName, xval, yvals, &hiError,
            &lowError);

   return false;
}

//------------------------------------------------------------------------------
// bool UpdateXyPlotCurve(const std::string &plotName, Integer whichCurve,
//       const Real &xval, const Real &yval, const Real hi, const Real low)
//------------------------------------------------------------------------------
/**
 * Adds a point to the plot data for a specific curve on a plot
 *
 * @param plotName The name of the plot receiving the data
 * @param whichCurve Index of the curve receiving the data
 * @param xval The X value associated with the point
 * @param yval The Y value associated with the points
 * @param hi +sigma error data for the point's error bar; only used if hi > 0.0
 * @param low -sigma error for the point's error bar; if <= 0.0, the low error
 *            is assumed to have the same magnitude as the high error
 *
 * @return true if the data was processed, false if not
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateXyPlotCurve(const std::string &plotName,
                             Integer whichCurve, const Real &xval,
                             const Real &yval, const Real hi, const Real low)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateXyPlotCurve(plotName, whichCurve, xval,
            yval, hi, low);

   return false;
}

//------------------------------------------------------------------------------
// bool DeactivateXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Disables redrawing for a plot.  This method is used when a plot is receiving
 * a large amount of data all at once, so that the update performance doesn't
 * degrade.
 *
 * @param plotName The name of the plot receiving the data
 *
 * @return true is a plot received the message, false if not
 */
//------------------------------------------------------------------------------
bool PlotInterface::DeactivateXyPlot(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeactivateXyPlot(plotName);

   return false;
}


//------------------------------------------------------------------------------
// bool ActivateXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Enables redrawing for a plot, and forces an immediate update.  This method is
 * used to redraw a plot after it has been disabled and  received a large amount
 * of data all at once.
 *
 * @param plotName The name of the plot receiving the data
 *
 * @return true is a plot received the message, false if not
 */
//------------------------------------------------------------------------------
bool PlotInterface::ActivateXyPlot(const std::string &plotName)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->ActivateXyPlot(plotName);

   return false;
}


//------------------------------------------------------------------------------
// bool TakeXYAction(const std::string& plotName, const std::string& action)
//------------------------------------------------------------------------------
/**
 * Passes a custom acton to the XY plot
 *
 * @param PlotName Name of the plot receiving the action
 * param action The invoked action
 *
 * @return true if an action was taken, false if no action was invoked
 */
//------------------------------------------------------------------------------
bool PlotInterface::TakeXYAction(const std::string& plotName,
      const std::string& action)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->TakeXYAction(plotName, action);

   return false;
}

//------------------------------------------------------------------------------
// bool CreateGroundTrackWindow(const std::string &plotName, const std::string &oldName, Real positionX, Real positionY, Real width, Real height, bool isMaximized)
//------------------------------------------------------------------------------
/**
 * Creates a new GroundTrackWindow
 *
 * @param plotName The name of the plot receiving the data
 * @param oldName The previous name of the plot
 * @param positionX, positionY  The position of the plot in screen coordinates
 * @param width, height  The dimensions of the window
 * @param isMaximized Flag for maximized or not
 *
 * @return true if creation was successful, false otherwise
 */
//------------------------------------------------------------------------------
bool PlotInterface::CreateGroundTrackWindow(const std::string &plotName,
                     const std::string &oldName,
                     Real positionX, Real positionY,
                     Real width, Real height, bool isMaximized)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateGroundTrackWindow(plotName, oldName,
            plotName, positionX, positionY, width, height, isMaximized);

   return false;
}

void PlotInterface::SetGroundTrackOption(const std::string &plotName,
                     const std::string &optionSetting,
                     const std::string &optionValue)
{
   if (thePlotReceiver != NULL)
      thePlotReceiver->SetGroundTrackOption(plotName, optionSetting,
            optionValue);
}

//------------------------------------------------------------------------------
// bool UpdateGroundTrackData(const std::string &plotName, const double epoch, const double *longlat, const int satcount)
//------------------------------------------------------------------------------
/**
 * Updates the data on a plot, passing in a set of y values for a given x, and
 * optionally the data used to draw error bars
 *
 * @param plotName The name of the plot receiving the data
 * @param epoch The epoch of the data
 * @param longlat A set of longitutde & latitude coordinates
 * @param satcount The number of satellites
 *
 * @return true if the data was processed, false if not
 */
//------------------------------------------------------------------------------
bool PlotInterface::UpdateGroundTrackData(const std::string &plotName,
      const double epoch, const double *longlat, const int satcount)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateGroundTrackData(plotName,
            epoch, longlat, satcount);

   return false;
}

//------------------------------------------------------------------------------
// bool TakeGroundTrackAction(const std::string& plotName, const std::string& action)
//------------------------------------------------------------------------------
/**
 * Passes a custom acton to the GroundTrack plot
 *
 * @param PlotName Name of the plot receiving the action
 * param action The invoked action
 *
 * @return true if an action was taken, false if no action was invoked
 */
//------------------------------------------------------------------------------
bool PlotInterface::TakeGroundTrackAction(const std::string &plotName,
                          const std::string &action)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->TakeGroundTrackAction(plotName, action);

   #ifdef DEBUG_GROUNDTRACK_ACTION
      else
         MessageInterface::ShowMessage("Action %s: The plot receiver is NULL\n",
               action.c_str());
   #endif

   return false;
}

//------------------------------------------------------------------------------
// bool CreateDynamicDataDisplay(const std::string &plotName, 
//                               const std::string &oldName,
//                               const std::string &plotTitle,
//                               Real positionX, Real positionY,
//                               Real width, Real height)
//------------------------------------------------------------------------------
/**
* Creates a DynamicDataDisplay window.
*
* @param plotName Name of the table
* @param oldName Former name of the table
* @param positionX, positionY  position of the table in screen coordinates
* @param w, h  size of the table in screen coordinates
* @param plotTitle Title of the table
*
* @return true on success, false when no table was created
*/
//------------------------------------------------------------------------------
bool PlotInterface::CreateDynamicDataDisplay(const std::string &plotName,
                             const std::string &oldName,
                             const std::string &plotTitle, Real positionX,
                             Real positionY, Real width, Real height)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->CreateDynamicDataDisplay(plotName, oldName,
         plotTitle, positionX, positionY, width, height);

   return false;
}

//------------------------------------------------------------------------------
// bool SetDynamicDataTableSize(std::string &plotName, Integer maxRowCount,
//                              Integer maxColCount)
//------------------------------------------------------------------------------
/**
* Sets the number of rows and columns of the DynamicDataDisplay
*
* @param plotName Name of the table
* @param maxRowCount The number of rows required
* @param maxColCount The number of columns required
*
* @return true on success, false when the table was not sized
*/
//------------------------------------------------------------------------------
bool PlotInterface::SetDynamicDataTableSize(const std::string &plotName,
                                            Integer maxRowCount,
                                            Integer maxColCount)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->SetDynamicDataTableSize(plotName, maxRowCount,
         maxColCount);

   return false;
}

//------------------------------------------------------------------------------
// bool UpdateDynamicDataDisplay(std::string &plotName, StringARray &paramNames
//                               Rvector newData, Rvector rowIdxs)
//------------------------------------------------------------------------------
/**
* Updates the data being watched to their current values
*
* @param plotName Name of the table
* @param paramNames The names of the parameters being watched
* @param newData The new data used to update the parameter values being
*        displayed
* @param rowIdxs The row indices for each of the parameters, used to place
*        the parameters in the desired locations
*
* @return true on success, false when the table was not sized
*/
//------------------------------------------------------------------------------
bool PlotInterface::UpdateDynamicDataDisplay(const std::string &plotName,
                                             std::vector<std::vector<DDD>> newData)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->UpdateDynamicDataDisplay(plotName, newData);

   return false;
}

//------------------------------------------------------------------------------
// bool DeleteDynamicData(const std::string &plotName,
//                        const std::string &oldname)
//------------------------------------------------------------------------------
/**
* Deletes the grid of the specified DynamicDataDisplay
*
* @param plotName Name of the table
* @param oldName Former name of the table
*
* @return true on success, false when the grid was not deleted
*/
//------------------------------------------------------------------------------
bool PlotInterface::DeleteDynamicData(const std::string &plotName,
                                      const std::string &oldName)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->DeleteDynamicData(plotName, oldName);

   return false;
}

bool PlotInterface::SetDynamicDataTextColor(const std::string &plotName,
                                            std::vector<std::vector<DDD>> newColors)
{
   if (thePlotReceiver != NULL)
      return thePlotReceiver->SetDynamicDataTextColor(plotName, newColors);

   return false;
}
