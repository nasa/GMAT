//$Id$
//------------------------------------------------------------------------------
//                             GuiPlotReceiver
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
 * Declares GuiPlotReceiver class.
 */
//------------------------------------------------------------------------------
#ifndef GuiPlotReceiver_hpp
#define GuiPlotReceiver_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "PlotReceiver.hpp"
#include "DynamicDataStruct.hpp"


/**
 * The PlotReceiver used in GMAT's wxWidgets based GUI
 */
class GuiPlotReceiver : public PlotReceiver
{

public:
   static GuiPlotReceiver* Instance();
   
   // for OpenGL Plot
   virtual bool CreateGlPlotWindow(const std::string &plotName,
                        const std::string &oldName,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized,
                        Integer numPtsToRedraw);
   
   virtual void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss);
   
   virtual void SetGlObject(const std::string &plotName,
                        const StringArray &objNames,
                        const std::vector<SpacePoint*> &objArray);
   
   virtual void SetGlCoordSystem(const std::string &plotName,
                        CoordinateSystem *internalCs,
                        CoordinateSystem *viewCs,
                        CoordinateSystem *viewUpCs);
   
   virtual void SetGl2dDrawingOption(const std::string &plotName,
                        const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
   virtual void SetGl3dDrawingOption(const std::string &plotName,
                        bool showLabels, bool drawEcPlane,
                        bool drawXyPlane, bool drawWireFrame,
                        bool drawAxes, bool drawGrid, bool drawSunLine,
                        bool overlapPlot, bool usevpInfo,
                        bool drawStars, bool drawConstellations,
                        Integer starCount);
   
   virtual void SetGl3dViewOption(const std::string &plotName,
                        SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   virtual void SetGlDrawOrbitFlag(const std::string &plotName,
                        const std::vector<bool> &drawArray);
   
   virtual void SetGlShowObjectFlag(const std::string &plotName,
                        const std::vector<bool> &showArray);
   
   virtual void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq);
   
   virtual bool IsThere(const std::string &plotName);
   
   virtual bool InitializeGlPlot(const std::string &plotName);
   virtual bool RefreshGlPlot(const std::string &plotName);
   virtual bool DeleteGlPlot(const std::string &plotName);
   virtual bool SetGlEndOfRun(const std::string &plotName);
   
   virtual bool UpdateGlPlot(const std::string &plotName,
                        const std::string &oldName,
                        const StringArray &scNames, const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const ColorMap &orbitColorMap, const ColorMap &targetColorMap,
                        bool solving, Integer solverOption, bool updateCanvas,
                        bool drawing, bool inFunction = false);
   
   virtual bool TakeGlAction(const std::string &plotName,
                        const std::string &action);
   virtual void SetMaxGlDataPoints(const std::string &plotName, Integer maxDataPoints);
   
   // for XY plot
   virtual bool CreateXyPlotWindow(const std::string &plotName,
                        const std::string &oldName,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized,
                        const std::string &plotTitle,
                        const std::string &xAxisTitle,
                        const std::string &yAxisTitle,
                        bool drawGrid = false,
                        bool canSaveLocation = true);
   virtual bool DeleteXyPlot(const std::string &plotName);
   virtual bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
                        const std::string &curveTitle, UnsignedInt penColor);
   virtual bool DeleteAllXyPlotCurves(const std::string &plotName,
                        const std::string &oldName);
   virtual bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex);
   virtual void ClearXyPlotData(const std::string &plotName);
   virtual void XyPlotPenUp(const std::string &plotName);
   virtual void XyPlotPenDown(const std::string &plotName);
   virtual void XyPlotDarken(const std::string &plotName, Integer factor,
                        Integer index = -1, Integer forCurve = -1);
   virtual void XyPlotLighten(const std::string &plotName, Integer factor,
                        Integer index = -1, Integer forCurve = -1);
   virtual void XyPlotMarkPoint(const std::string &plotName, Integer index = -1,
                        Integer forCurve = -1);
   virtual void XyPlotMarkBreak(const std::string &plotName, Integer index = -1,
                        Integer forCurve = -1);
   virtual void XyPlotClearFromBreak(const std::string &plotName,
                        Integer startBreakNumber, Integer endBreakNumber = -1,
                        Integer forCurve = -1);

   virtual void XyPlotChangeColor(const std::string &plotName,
                        Integer index = -1, UnsignedInt newColor = 0xffffff,
                        Integer forCurve = -1);
   virtual void XyPlotChangeMarker(const std::string &plotName,
                        Integer index = -1, Integer newMarker = -1, Integer forCurve = -1);
   virtual void XyPlotChangeWidth(const std::string &plotName,
                        Integer index = -1, Integer newWidth = 1, int forCurve = -1);
   virtual void XyPlotChangeStyle(const std::string &plotName,
                        Integer index = -1, Integer newStyle = 100, int forCurve = -1);

   virtual void XyPlotRescale(const std::string &plotName);
   virtual void XyPlotCurveSettings(const std::string &plotName,
                        bool useLines = true,
                        Integer lineWidth = 1,
                        Integer lineStyle = 100,
                        bool useMarkers = false,
                        Integer markerSize = 3,
                        Integer marker = 1,
                        bool useHiLow = false,
                        Integer forCurve = -1);
   
   virtual void SetXyPlotTitle(const std::string &plotName,
                        const std::string &plotTitle);
   virtual void ShowXyPlotLegend(const std::string &plotName);
   virtual bool RefreshXyPlot(const std::string &plotName);
   virtual bool UpdateXyPlot(const std::string &plotName,
                        const std::string &oldName,
                        const Real &xval, const Rvector &yvals,
                        const std::string &plotTitle,
                        const std::string &xAxisTitle,
                        const std::string &yAxisTitle,
                        bool updateCanvas, bool drawGrid);
   virtual bool UpdateXyPlotData(const std::string &plotName, const Real &xval,
                        const Rvector &yvals, const Rvector *yhis = NULL,
                        const Rvector *ylows = NULL);
   virtual bool UpdateXyPlotCurve(const std::string &plotName,
                        const Integer whichCurve, const Real xval,
                        const Real yval, const Real yhi = 0.0,
                        const Real ylow = 0.0);

   virtual bool DeactivateXyPlot(const std::string &plotName);
   virtual bool ActivateXyPlot(const std::string &plotName);
   virtual bool TakeXYAction(const std::string &plotName,
                        const std::string &action);

   // for dynamic data display
   virtual bool CreateDynamicDataDisplay(const std::string &plotName,
                        const std::string &oldName,
                        const std::string &plotTitle, Real positionX,
                        Real positionY, Real width, Real height);
   virtual bool SetDynamicDataTableSize(const std::string &plotName,
                        Integer maxRowCount, Integer maxColCount);
   virtual bool UpdateDynamicDataDisplay(const std::string &plotName,
                        std::vector<std::vector<DDD>> newData);
   virtual bool DeleteDynamicData(const std::string &plotName,
                        const std::string &oldName);
   virtual bool SetDynamicDataTextColor(const std::string &plotName,
                        std::vector<std::vector<DDD>> newColors);

   //GroundTrackWindow
   virtual bool CreateGroundTrackWindow(const std::string &trackName,
                        const std::string &oldName,
                        const std::string &title,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized);
   virtual void SetGroundTrackOption(const std::string &plotName,
                        const std::string &optionSetting,
                        const std::string &optionValue);
   virtual bool UpdateGroundTrackData(const std::string &plotName,
                        const double epoch, const double *longlat,
                        const int satcount);
   virtual bool TakeGroundTrackAction(const std::string &plotName,
                        const std::string &action);


protected:
   bool ComputePlotPositionAndSize(bool isGLPlot, Real positionX,
                                   Real positionY, Real width, Real height,
                                   Integer &x, Integer &y, Integer &w, Integer &h,
                                   bool usingSaved = false);
   
private:
   GuiPlotReceiver();
   virtual ~GuiPlotReceiver();

   static GuiPlotReceiver* theGuiPlotReceiver;
};

#endif
