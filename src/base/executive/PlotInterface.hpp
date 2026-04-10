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
 * Declares PlotInterface class.
 */
//------------------------------------------------------------------------------
#ifndef PlotInterface_hpp
#define PlotInterface_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "PlotReceiver.hpp"
#include "DynamicDataStruct.hpp"


/**
 * Interface functions for the OpenGL and XY plot classes.
 */
class GMAT_API PlotInterface
{

public:
   
   static void SetPlotReceiver(PlotReceiver *pr);
   
   // for OpenGL Plot
   static bool CreateGlPlotWindow(const std::string &plotName,
                        const std::string &oldName,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized,
                        Integer numPtsToRedraw);
   
   static void SetViewType(GmatPlot::ViewType view);
   static void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss);
   
   static void SetGlObject(const std::string &plotName,
                        const StringArray &objNames,
                        const std::vector<SpacePoint*> &objArray);
   
   static void SetGlCoordSystem(const std::string &plotName,
                        CoordinateSystem *internalCs,
                        CoordinateSystem *viewCs,
                        CoordinateSystem *viewUpCs);
   
   static void SetGl2dDrawingOption(const std::string &plotName,
                        const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
   static void SetGl3dDrawingOption(const std::string &plotName,
                        bool showLabels, bool drawEcPlane, bool drawXyPlane,
                        bool drawWireFrame, bool drawAxes,
                        bool drawGrid, bool drawSunLine,
                        bool overlapPlot, bool usevpInfo,
                        bool drawStars, bool drawConstellations,
                        Integer starCount);
   
   static void SetGl3dViewOption(const std::string &plotName,
                        SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   static void SetGlDrawOrbitFlag(const std::string &plotName,
                        const std::vector<bool> &drawArray);
   
   static void SetGlShowObjectFlag(const std::string &plotName,
                        const std::vector<bool> &showArray);
   
   static void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq);
   
   static bool IsThere(const std::string &plotName);
   
   static bool InitializeGlPlot(const std::string &plotName);
   static bool RefreshGlPlot(const std::string &plotName);
   static bool DeleteGlPlot(const std::string &plotName);
   static bool SetGlEndOfRun(const std::string &plotName);
   
   static bool UpdateGlPlot(const std::string &plotName,
                        const std::string &oldName,
                        const StringArray &scNames, const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const ColorMap &orbitColorMap,
                        const ColorMap &targetColorMap,
                        bool solving, Integer solverOption, bool updateCanvas,
                        bool drawing, bool inFunction = false);
   
   static bool TakeGlAction(const std::string &plotName,
                        const std::string &action);
   static void SetMaxGlDataPoints(const std::string &plotname, Integer maxDataPoints);
   
   // for XY plot
   static bool CreateXyPlotWindow(const std::string &plotName,
                        const std::string &oldName,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized,
                        const std::string &plotTitle,
                        const std::string &xAxisTitle,
                        const std::string &yAxisTitle,
                        bool drawGrid = false, bool canSave = true);
   static bool DeleteXyPlot(const std::string &plotName);
   static bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
                        const std::string &curveTitle,
                        UnsignedInt penColor);
   static bool DeleteAllXyPlotCurves(const std::string &plotName,
                        const std::string &oldName);
   static bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex);
   static void ClearXyPlotData(const std::string &plotName);
   static void XyPlotPenUp(const std::string &plotName);
   static void XyPlotPenDown(const std::string &plotName);
   static void XyPlotDarken(const std::string &plotName, Integer factor,
                        Integer index = -1, Integer curveNumber = -1);
   static void XyPlotLighten(const std::string &plotName, Integer factor,
                        Integer index = -1, Integer curveNumber = -1);
   static void XyPlotMarkPoint(const std::string &plotName, Integer index = -1,
                        Integer curveNumber = -1);
   static void XyPlotMarkBreak(const std::string &plotName, Integer index = -1,
                        Integer curveNumber = -1);
   static void XyPlotClearFromBreak(const std::string &plotName,
                        Integer startBreakNumber = -1, Integer endBreakNumber = -1,
                        Integer curveNumber = -1);

   static void XyPlotChangeWidth(const std::string &plotName,
                        Integer index = -1, Integer newWidth = 1, int forCurve = -1);
   static void XyPlotChangeStyle(const std::string &plotName,
                        Integer index = -1, Integer newStyle = 100, int forCurve = -1);

   
   static void XyPlotRescale(const std::string &plotName);

   static void XyPlotCurveSettings(const std::string &plotName,
                        bool useLines = true,
                        Integer lineWidth = 1,
                        Integer lineStyle = 100,
                        bool useMarkers = false,
                        Integer markerSize = 3,
                        Integer marker = 1,
                        bool useHiLow = false,
                        Integer forCurve = -1);

   static void SetXyPlotTitle(const std::string &plotName,
                        const std::string &plotTitle);
   static void ShowXyPlotLegend(const std::string &plotName);
   static bool RefreshXyPlot(const std::string &plotName);
   static bool UpdateXyPlot(const std::string &plotName,
                        const std::string &oldName,
                        const Real &xval, const Rvector &yvals,
                        const std::string &plotTitle,
                        const std::string &xAxisTitle,
                        const std::string &yAxisTitle,
                        Integer solverOption,
                        bool updateCanvas, bool drawGrid);
   static bool UpdateXyPlotData(const std::string &plotName,
                        const Real &xval, const Rvector &yvals,
                        const Rvector &hiError,
                        const Rvector &lowError);
   
   static bool UpdateXyPlotCurve(const std::string &plotName,
                        Integer whichCurve, const Real &xval, const Real &yval,
                        const Real hi = 0.0, const Real low = 0.0);

   static bool DeactivateXyPlot(const std::string &plotName);
   static bool ActivateXyPlot(const std::string &plotName);
   static bool TakeXYAction(const std::string &plotName,
                        const std::string &action);

   // Ground track plot
   static bool CreateGroundTrackWindow(const std::string &plotName,
                        const std::string &oldName,
                        Real positionX, Real positionY,
                        Real width, Real height, bool isMaximized);
   static void SetGroundTrackOption(const std::string &plotName,
                        const std::string &optionSetting,
                        const std::string &optionValue);


//   static bool DeleteGroundTrack(const std::string &plotName);
//   static bool AddGroundTrackCurve(const std::string &plotName, int curveIndex,
//                        const std::string &curveTitle,
//                        UnsignedInt penColor);
//   static void ClearGroundTrackData(const std::string &plotName);
//   static bool RefreshGroundTrack(const std::string &plotName);

   static bool UpdateGroundTrackData(const std::string &plotName,
                          const double epoch, const double *longlat,
                          const int satcount);

//   static bool UpdateGroundTrack(const std::string &plotName,
//                        const std::string &oldName,
//                        const Real &xval, const Rvector &yvals,
//                        Integer solverOption,
//                        bool updateCanvas);
//   static bool UpdateGroundTrackCurve(const std::string &plotName,
//                        Integer whichCurve, const Real &xval, const Real &yval);
   static bool TakeGroundTrackAction(const std::string &plotName,
                          const std::string &action);



   // for dynamic data display
   static bool CreateDynamicDataDisplay(const std::string &plotName,
                        const std::string &oldName,
                        const std::string &plotTitle, Real positionX, 
                        Real positionY, Real width, Real height);
   static bool SetDynamicDataTableSize(const std::string &plotName,
                        Integer maxRowCount, Integer maxColCount);
   static bool UpdateDynamicDataDisplay(const std::string &plotName,
                        std::vector<std::vector<DDD>> newData);
   static bool DeleteDynamicData(const std::string &plotName,
                        const std::string &oldName);
   static bool SetDynamicDataTextColor(const std::string &plotName,
                        std::vector<std::vector<DDD>> newColors);

private:

   PlotInterface();
   ~PlotInterface();
   
   static PlotReceiver *thePlotReceiver;

};

#endif
