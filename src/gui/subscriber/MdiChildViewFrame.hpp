//$Id$
//------------------------------------------------------------------------------
//                              MdiChildViewFrame
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
// Created: 2010/04/19
/**
 * Declares MdiChildViewFrame for 3D visualization.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildViewFrame_hpp
#define MdiChildViewFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "MdiGlPlotData.hpp"      // for wxStringColorMap
#include "gmatdefs.hpp"
#include "SolarSystem.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class ViewCanvas;

class MdiChildViewFrame: public GmatMdiChildFrame
{
public:
   MdiChildViewFrame(wxMDIParentFrame *parent, const wxString& plotName,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style,
                     const wxString &viewType,
                     const GmatTree::ItemType itemType);
   virtual ~MdiChildViewFrame();
   
   // getters
   virtual bool GetOverlapPlot();
   virtual ViewCanvas* GetGlCanvas();
   virtual bool GetUseViewPointInfo();
   virtual bool GetDrawWireFrame();
   virtual bool GetDrawXyPlane();
   virtual bool GetDrawEcPlane();
   virtual bool GetDrawSunLine();
   virtual bool GetDrawAxes();
   virtual bool GetDrawGrid();
   virtual UnsignedInt GetXyPlaneColor();
   virtual UnsignedInt GetSunLineColor();
   virtual bool IsAnimationRunning();
   virtual Integer GetAnimationUpdateInterval();
   virtual Integer GetAnimationFrameIncrement();
   
   // setters
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetUserInterrupt();
   
   virtual void SetPlotName(const wxString &name);
   virtual void ResetShowViewOption();
   virtual void SetOverlapPlot(bool overlap);
   virtual void SetUseInitialViewDef(bool flag);
   virtual void SetAnimationUpdateInterval(Integer value);
   virtual void SetAnimationFrameIncrement(Integer value);
   virtual void SetDrawWireFrame(bool flag);
   virtual void SetDrawStars(bool flag);
   virtual void SetDrawConstellations(bool flag);
   virtual void SetStarCount(int count);
   virtual void SetDrawXyPlane(bool flag);
   virtual void SetDrawEcPlane(bool flag);
   virtual void SetDrawSunLine(bool flag);
   virtual void SetDrawAxes(bool flag);
   virtual void SetDrawGrid(bool flag);
   virtual void SetIsNewFrame(bool flag);
   virtual void SetXyPlaneColor(UnsignedInt color);
   virtual void SetSunLineColor(UnsignedInt color);
   //virtual void SetObjectColors(const wxStringColorMap &objectColorMap);
   virtual void SetShowObjects(const wxStringBoolMap &showObjMap);
   virtual void SetNumPointsToRedraw(Integer numPoints);
   
   // actions
   virtual void DrawInOtherCoordSystem(const wxString &csName);
   virtual void RedrawPlot(bool viewAnimation);
   
   // menu actions
   virtual void OnClearPlot(wxCommandEvent& event);
   virtual void OnChangeTitle(wxCommandEvent& event);
   virtual void OnShowDefaultView(wxCommandEvent& event);
   
   // from GL option dialog
   virtual void OnDrawWireFrame(wxCommandEvent& event);
   virtual void OnDrawXyPlane(wxCommandEvent& event);
   
   virtual void OnQuit(wxCommandEvent& event);
   
   virtual void OnActivate(wxActivateEvent& event);
   virtual void OnPlotSize(wxSizeEvent& event);
   virtual void OnMove(wxMoveEvent& event);
   virtual void OnPlotClose(wxCloseEvent &event);
   virtual void OnClose(wxCloseEvent &event);
   
   // drawing
   virtual void SetGlObject(const StringArray &objNames,
                        //const UnsignedIntArray &objOrbitColors,
                        const std::vector<SpacePoint*> &objArray);
   
   virtual void SetGlCoordSystem(CoordinateSystem *internalCs,
                        CoordinateSystem *viewCs,
                        CoordinateSystem *viewUpCs);
   
   virtual void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
   virtual void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                        bool drawXyPlane, bool drawWireFrame, bool drawAxes,
                        bool drawGrid, bool drawSunLine, bool overlapPlot,
                        bool usevpInfo, bool drawStars, bool drawConstellations,
                        Integer starCount);
   
   virtual void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   virtual void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   virtual void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   virtual void SetGlUpdateFrequency(Integer updFreq);
   
   virtual void UpdatePlot(const StringArray &scNames, const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        //const UnsignedIntArray &scColors,
                        const ColorMap &orbitColorMap,
                        const ColorMap &targetColorMap,
                        bool solving, Integer solverOption, bool updateCanvas,
                        bool drawing, bool inFunction = false);
   virtual void TakeAction(const std::string &action);
   virtual void InitializePlot();
   virtual void RefreshPlot();
   virtual void DeletePlot();
   virtual void SetEndOfRun();

   //Data points
   virtual void SetMaxDataPoints(Integer);
      
protected:
   
   void CheckFrame();
   
   ViewCanvas *mCanvas;
   
   wxString mPlotTitle;
   wxString mViewType;
   bool mOverlapPlot;
   bool mInFunction;
   
   wxArrayString mBodyNames;
   UnsignedIntArray mBodyColors;
   
   DECLARE_EVENT_TABLE()
};
#endif
