//$Id$
//------------------------------------------------------------------------------
//                              GroundTrackCanvas
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
// Created: 2011/06/06
/**
 * Declares GroundTrackCanvas for drawing ground track plot using OpenGL.
 */
//------------------------------------------------------------------------------
#ifndef GroundTrackCanvas_hpp
#define GroundTrackCanvas_hpp

#include "ViewCanvas.hpp"
#include "gmatwxdefs.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "GLStars.hpp"
#include "Rendering.hpp"           // for GlColorType
#include <map>

class GroundTrackCanvas: public ViewCanvas
{
public:
   GroundTrackCanvas(wxWindow *parent, const wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, 
                     const wxString& name = wxT("GroundTrackCanvas"),
                     long style = 0);
   virtual ~GroundTrackCanvas();
   
   
   void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
   // actions
   void ClearPlot();
   void RedrawPlot(bool viewAnimation);
   void ShowDefaultView();
   void ZoomIn();
   void ZoomOut();
   void DrawWireFrame(bool flag);
   
   void OnDrawGrid(bool flag);
   void DrawInOtherCoordSystem(const wxString &csName);
   void ViewAnimation(int interval, int frameInc = 30);
   
   // user actions
   void TakeAction(const std::string &action);

   //Max data point option
   virtual int GetMaxDataPoints();
   virtual void SetMaxDataPoints(int);
   
protected:
   
   // events
   void OnPaint(wxPaintEvent &event);
   void OnSize(wxSizeEvent &event);
   void OnMouse(wxMouseEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   
   DECLARE_EVENT_TABLE();

private:
   
   static const int LAST_STD_BODY_ID;// = 10;
   static const int MAX_COORD_SYS;// = 10;
   //static const std::string BODY_NAME[GmatPlot::MAX_BODIES];
   static const Real MAX_ZOOM_IN;// = 3700.0;
   static const Real RADIUS_ZOOM_RATIO;// = 2.2;
   static const Real DEFAULT_DIST;// = 30000.0;
      
   // camera
   Camera mCamera;
   
   // light source
   Light mLight;   
   
   // Data members used by the mouse
   GLfloat mfStartX, mfStartY;
   
   // Actual params of the window
   GLfloat mfLeftPos, mfRightPos, mfBottomPos, mfTopPos;
   
   // central body
   std::string mCentralBodyName;
   std::string mCentralBodyTextureFile;
   
   // warning flags
   bool        mWriteCentralBodyWarning;
   
   // foot print option
   Integer mFootPrintOption;
   Integer mFootPrintDrawFrequency;
   
   // mouse rotating
   bool mRotateXy;
   bool mRotateEarthToEnd;
   Real lastLongitudeD;
   float mQuat[4];     // orientation of object
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   
   // Control Modes
   int mControlMode;  // 0 = rotate around center, 1 = 6DOF movement, 2 = 6DOF movement NASA/Dunn style
   int mInversion;    // 1 is true movement, -1 is inverted
   
   // initial viewpoint
   std::string mViewPointRefObjName;
   std::string mViewUpAxisName;
   
   // spacecraft
   GLuint mGlList;
   
   // view
   wxSize  mCanvasSize;
   
   // view point
   void SetDefaultViewPoint();
   void InitializeViewPoint();
   void SetDefaultView();
   
   // view objects
   void SetupWorld();

   // drawing objects
   void DrawFrame();
   void DrawPlot();
   void DrawObjectOrbit();
   void DrawObjectTexture(const wxString &objName, int obj, int objId);
   void DrawObject(const wxString &objName, int obj);
   void DrawOrbitLines(int i, const wxString &objName, int obj, int objId);
   void DrawGroundTrackLines(Rvector3 &r1, Rvector3 &v1,
                             Rvector3 &r2, Rvector3 &v2);
   void DrawGridLines();
   void DrawCentralBodyTexture();
   void DrawCircleAt(GlColorType *color, double lon, double lat, double radius,
                     bool enableTransparency = true);
   void DrawCircleAtCurrentPosition(int objId, int index, double radius,
                                    bool enableTransparency = true);
   void DrawImage(const wxString &objName, float lon, float lat, float imagePos, GLubyte *image);
   void DrawSpacecraft(const wxString &objName, int objId, int index);
   void DrawGroundStation(const wxString &objName, int objId, int index);
   
   // for rotation
   void RotateBodyUsingAttitude(const wxString &objName, int objId);
   void RotateBody(const wxString &objName, int frame, int objId);
      
   // for coordinate system
   bool ConvertObjectData();
   void ConvertObject(int objId, int index);
   
   // Linux specific fix
   #ifdef __WXGTK__
      bool hasBeenPainted;
   #endif
};

#endif
