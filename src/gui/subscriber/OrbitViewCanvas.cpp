//$Id$
//------------------------------------------------------------------------------
//                              OrbitViewCanvas
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
// Developed jointly by NASA/GSFC, Thinking Systems, Inc., and Schafer Corp.,
// under AFRL NOVA Contract #FA945104D03990003
//
// Author:   Linda Jun (wrote TrajPlotCanvas)
// Created:  2003/11/25
// Modified: Phillip Silvia, Jr. and Dunning Idle 5th - Schafer Corp.
// Date:     Summer 2010
// Changes:  Took out all minus signs from trajectories and vectors so attitude
//           would work correctly.  Added attitude calls to draw spacecraft in
//           the proper orientation.  Changed the ECI axis color convention to
//           Red-Green-Blue.
/**
* These classes and methods implement OpenGL display of the earth and
* orbit trajectories.  The upgrade by Schafer allows 3D spacecraft
* models to be drawn in the correct position and attitude.
 */
//------------------------------------------------------------------------------

#include "OrbitViewCanvas.hpp"
#include "Camera.hpp"
#include "ModelObject.hpp"
#include "ModelManager.hpp"
#include "GmatAppData.hpp"         // for GetGuiInterpreter()
#include "GmatMainFrame.hpp"       // for EnableAnimation()
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "Rvector3.hpp"            // for Rvector3::GetMagnitude()
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"           // for ComputeAngleInDeg()
#include "CelestialBody.hpp"
#include "MdiGlPlotData.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "TimeSystemConverter.hpp" 
#include "GmatDefaults.hpp"
#include "BodyFixedAxes.hpp"
#include "Rendering.hpp"
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include "AttitudeConversionUtility.hpp"
#include <string.h>                // for strlen( )

#include "GuiPublisher.hpp"

#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#  else
#    include <gl.h>
#    include <glu.h>
#  endif
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

// always define USE_TRACKBALL, it works better for rotation
#define USE_TRACKBALL
#ifdef USE_TRACKBALL
#include "AttitudeUtil.hpp"
using namespace FloatAttUtil;
#endif

// Now using wxMilliSleep() so removed (LOJ: 2012.07.18)
// If Sleep in not defined (on unix boxes)
//#ifndef Sleep 
//#ifndef __WXMSW__
//#include <unistd.h>
//#define Sleep(t) usleep((t))
//#endif
//#endif

// For the newer (wx 2.7.x+) method, create a wxGLCanvas window using the
// constructor that does not create an implicit rendering context, create
// an explicit instance of a wxGLContext that is initialized with the
// wxGLCanvas yourself (highly recommended for future compatibility with
// wxWidgets and the flexibility of your own program!) then use either
// wxGLCanvas::SetCurrent with the instance of the wxGLContext or
// wxGLContext::SetCurrent with the instance of the wxGLCanvas to bind the
// OpenGL state that is represented by the rendering context to the canvas,
// and then call wxGLCanvas::SwapBuffers to swap the buffers of the OpenGL
// canvas and thus show your current output.

// Skip data over the max position difference
// Define this for GMT-3209 Fix (LOJ: 2013.01.25)
// Actually this is not a robust fix. We need to think about better fix for
// incorrectly connecting lines
//#define SKIP_DATA_OVER_LIMIT

// Currently local plots are deleted when function run completes (2015.07.09)
// So this macro has no effects. But if plot is global we need to check if 
// all drawing objects are global as well since plot access object pointers.
//#define DISABLE_REPAINT_IN_FUNCTION

// debug
//#define DEBUG_INIT 1
//#define DEBUG_ACTION 1
//#define DEBUG_CONVERT 1
//#define DEBUG_DRAW 2
//#define DEBUG_ORBIT_LINES
//#define DEBUG_DRAW_STARS 1
//#define DEBUG_ZOOM 1
//#define DEBUG_OBJECT 2
//#define DEBUG_VIEW_OPTIONS 1
//#define DEBUG_DRAWING_OPTIONS
//#define DEBUG_CS 1
//#define DEBUG_ANIMATION 1
//#define DEBUG_LONGITUDE 1
//#define DEBUG_SHOW_SKIP 1
//#define DEBUG_ROTATE_BODY 1
//#define DEBUG_DATA_BUFFERRING
//#define DEBUG_DRAW_3D_BODY
//#define DEBUG_ON_PAINT 2

#define MODE_CENTERED_VIEW 0
#define MODE_FREE_FLYING 1
#define MODE_ASTRONAUT_6DOF 2

BEGIN_EVENT_TABLE(OrbitViewCanvas, ViewCanvas)
   EVT_SIZE(OrbitViewCanvas::OnSize)
   EVT_PAINT(OrbitViewCanvas::OnPaint)
   EVT_MOUSE_EVENTS(OrbitViewCanvas::OnMouse)
   EVT_KEY_DOWN(OrbitViewCanvas::OnKeyDown)
END_EVENT_TABLE()

using namespace GmatPlot;
using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const int OrbitViewCanvas::LAST_STD_BODY_ID = 10;
const int OrbitViewCanvas::MAX_COORD_SYS = 10;
const Real OrbitViewCanvas::MAX_ZOOM_IN = 3700.0;
const Real OrbitViewCanvas::RADIUS_ZOOM_RATIO = 2.2;
const Real OrbitViewCanvas::DEFAULT_DISTANCE = 30000.0;

// color
static unsigned int *sIntColor = new unsigned int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// OrbitViewCanvas(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <parent>   parent window pointer
 * @param <id>       window id
 * @param <pos>      position (top, left) where the window to be placed 
 * @param <size>     size of the window
 * @param <name>     title of window
 * @param <style>    style of window
 *
 */
//------------------------------------------------------------------------------
OrbitViewCanvas::OrbitViewCanvas(wxWindow *parent, wxWindowID id,
                                 const wxPoint& pos, const wxSize& size,
                                 const wxString& name, long style)
   : ViewCanvas(parent, id, pos, size, name, style)
{
   // extra data need
   mNeedAttitude = true;
	mNeedEcliptic = true;
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = false;
   #endif
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas() entered, name = '%s', size.X = %d, size.Y = %d\n",
       name.WX_TO_C_STRING, size.GetWidth(), size.GetHeight());
   #endif
   
   mStars = GLStars::Instance();
   mStars->SetDesiredStarCount(mStarCount);
   
   mCamera.Reset();
   mCamera.Relocate(DEFAULT_DISTANCE, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   mGlList = 0;
   mIsFirstRun = true;
   
   ResetPlotInfo();
   
   // projection
   mControlMode = MODE_CENTERED_VIEW;
   mInversion = 1;
   mUseInitialViewPoint = true;
   
   // viewpoint
   SetDefaultViewPoint();
   
   // default view
   mCanvasSize = size;
   mDefaultRotXAngle = 90.0;
   mDefaultRotYAngle = 0.0;
   mDefaultRotZAngle = 0.0;
   mDefaultViewDistance = DEFAULT_DISTANCE;
   
   mCurrViewDistance = DEFAULT_DISTANCE;
   
   mOriginName = "";
   mOriginId = 0;
   
   //original value
   mZoomAmount = 300.0;
   
   // projection
   ChangeProjection(size.x, size.y, mCurrViewDistance);
   
   // Note from Dunn.  Size of space object vs. spacecraft models will take lots of
   // work in the future.  Models need to be drawn in meters.  Cameras need to
   // be placed near models to "make big enough to see", and if camera is beyond
   // about 2000 meters, model should be drawn as dot.  More discussion to follow!
   mScRadius = 200;        //km: make big enough to see
   
   // drawing options
   mDrawXyPlane = false;
   mDrawEclipticPlane = false;
   mDrawAxes = false;
   mPolygonMode = GL_FILL;
   
   mXyPlaneColor = GmatColor::NAVY;
   mEcPlaneColor = GmatColor::MAROON;
   mSunLineColor = GmatColor::YELLOW;
   
   // animation
   mIsAnimationRunning = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 1;
   mFrameInc = 1;
   
   // solver data
   mDrawSolverData = false;
   
   // message
   mShowMaxWarning = true;
   mOverCounter = 0;
   
   // solar system
   pSolarSystem = NULL;
   
   // objects
   mObjectCount         = 0;
   mObjectDefaultRadius = 200; //km: make big enough to see

   // Initialize arrays to NULL
   ClearObjectArrays(false);
   
   // Zoom
   mMaxZoomIn = MAX_ZOOM_IN;
   
   // Spacecraft
   mScCount = 0;
    
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("   pInternalCoordSystem=<%p>'%s', pViewCoordSystem=<%p>'%s'\n",
       pInternalCoordSystem, pInternalCoordSystem ? pInternalCoordSystem->GetName().c_str() : "NULL",
       pViewCoordSystem, pViewCoordSystem ? pViewCoordSystem->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("OrbitViewCanvas() constructor exiting, theContext=<%p>\n\n", theContext);
   #endif
}


//------------------------------------------------------------------------------
// ~OrbitViewCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitViewCanvas::~OrbitViewCanvas()
{
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::~OrbitViewCanvas() '%s' entered, mHasUserInterrupted=%d\n",
       mPlotName.WX_TO_C_STRING, mHasUserInterrupted);
   #endif
   
   // Patch from Tristan Moody
   //   This patch moves the modelContext deletion to the ModelManager
   //   destructor from the OrbitViewCanvas and GroundTrackCanvas destructors.
   //   As long as the ModelManager destructor is called after all other
   //   OpenGL-related code is finished (to be verified), this should fix the
   //   problem reported in Bug 2591.
   //
   // Old code:
//   // Note:
//   // deleting m_glContext is handled in wxGLCanvas
//
//   #ifndef __WXMAC__
//      ModelManager *mm = ModelManager::Instance();
//      if (!mm->modelContext)
//      {
//         // delete modelContext since it was created in the constructor
//         delete mm->modelContext;
//         mm->modelContext = NULL;
//      }
//   #endif
   
   ClearObjectArrays();
   
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::~OrbitViewCanvas() '%s' exiting\n", mPlotName.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void ClearPlot()
//------------------------------------------------------------------------------
/**
 * Clears plot.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ClearPlot()
{
   SetGLContext(); // Ensure context is valid; placate some GL drivers

   //loj: black for now.. eventually it will use background color
   glClearColor(0.0, 0.0, 0.0, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glFlush();

   #ifndef __linux
      SwapBuffers();
   #endif
}


//------------------------------------------------------------------------------
// void RedrawPlot(bool viewAnimation)
//------------------------------------------------------------------------------
/**
 * Redraws plot.
 *
 * @param <viewAnimation> true if animation is viewed
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_REDRAW
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::RedrawPlot() entered, viewAnimation=%d\n", viewAnimation);
   #endif
   
   if (mCurrViewDistance < mMaxZoomIn)
   {
      mCurrViewDistance = mMaxZoomIn;
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::RedrawPlot() distance < max zoom in. distance set to %f\n",
          mCurrViewDistance);
   }
   
   if (viewAnimation)
      ViewAnimation(mUpdateInterval, mFrameInc);
   else
      Refresh(false);
   
}


//------------------------------------------------------------------------------
// void ShowDefaultView()
//------------------------------------------------------------------------------
/**
 * Shows default view.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ShowDefaultView()
{
   int clientWidth, clientHeight;
   GetClientSize(&clientWidth, &clientHeight);

   SetDefaultView();
   ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
   ChangeProjection(clientWidth, clientHeight, mCurrViewDistance);
   Refresh(false);
}

//------------------------------------------------------------------------------
// void DrawWireFrame(bool flag)
//------------------------------------------------------------------------------
/**
 * Shows objects in wire frame.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawWireFrame(bool flag)
{
   mDrawWireFrame = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawXyPlane(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws equatorial plane
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawXyPlane(bool flag)
{
   mDrawXyPlane = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawEcPlane(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawEcPlane(bool flag)
{
   mDrawEclipticPlane = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void OnDrawAxes(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws axes.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnDrawAxes(bool flag)
{
   mDrawAxes = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void OnDrawGrid(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws axes.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnDrawGrid(bool flag)
{
   mDrawGrid = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
/**
 * Draws objects in other coordinate system.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawInOtherCoordSystem(const wxString &csName)
{
   #if DEBUG_ACTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawInOtherCoordSysName() viewCS=%s, newCS=%s\n",
       mViewCoordSysName.c_str(), csName.WX_TO_C_STRING);
   #endif

   if (csName == "")
      return;
   
   
   // if current view CS name is different from the new CS name
   if (!mViewCoordSysName.IsSameAs(csName))
   {
      mViewCoordSysName = csName;
      
      pViewCoordSystem =
         (CoordinateSystem*)theGuiInterpreter->GetConfiguredObject(csName.c_str());
      
      if (pViewCoordSystem->GetName() == pInternalCoordSystem->GetName())
         mViewCsIsInternalCs = true;
      else
         mViewCsIsInternalCs = false;
      
      wxString oldOriginName = mOriginName;
      mOriginName = pViewCoordSystem->GetOriginName().c_str();
      mOriginId = GetObjectId(mOriginName);
      
      UpdateRotateFlags();
      
      if (!mOriginName.IsSameAs(oldOriginName))
         GotoObject(mOriginName);
      
      ConvertObjectData();
      Refresh(false);
   }
}


//---------------------------------------------------------------------------
// void GotoObject(const wxString &objName)
//---------------------------------------------------------------------------
void OrbitViewCanvas::GotoObject(const wxString &objName)
{
   int objId = GetObjectId(objName);
   
   mViewObjId = objId;
   mMaxZoomIn = mObjMaxZoomIn[objId];
   
   // if goto Object is center(0,0,0), zoom out to see the object,
   // otherwise, set to final position of the object
   if (objName == mViewObjName)
   {
      mCurrViewDistance = mMaxZoomIn;
   }
   else
   {
      //int index = objId * maxData * 3 + (mNumData-1) * 3;
      int index = objId * maxData * 3 + mLastIndex * 3;

      // compute mCurrViewDistance
      Rvector3 pos(mObjectViewPos[index+0], mObjectViewPos[index+1],
                   mObjectViewPos[index+2]);
      
      mCurrViewDistance = pos.GetMagnitude();
      
      if (mCurrViewDistance == 0.0)
         mCurrViewDistance = mMaxZoomIn;
   }
   
   #ifdef DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::GotoObject() objName=%s, mViewObjId=%d, mMaxZoomIn=%f\n"
       "   mCurrViewDistance=%f\n", objName.WX_TO_C_STRING, mViewObjId, mMaxZoomIn, mCurrViewDistance);
   #endif

   mIsEndOfData = true;
   mIsEndOfRun = true;

}


//---------------------------------------------------------------------------
// void ViewAnimation(int interval, int frameInc)
//---------------------------------------------------------------------------
void OrbitViewCanvas::ViewAnimation(int interval, int frameInc)
{
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ViewAnimation() entered, interval=%d, frameInc=%d\n",
       interval, frameInc);
   #endif
   
   #ifdef DISABLE_REPAINT_IN_FUNCTION
   if (mIsEndOfData && mInFunction)
   {
      wxString msg = "*** WARNING *** This plot data was published inside a "
         "function, so repainting or drawing animation is disabled.\n";
      MessageInterface::ShowMessage(msg.WX_TO_C_STRING);
      return;
   }
   #endif
   
   this->SetFocus(); // so that it can get key interrupt
   mIsAnimationRunning = true;
   mUpdateInterval = interval;
   mFrameInc = frameInc;
   mHasUserInterrupted = false;
   // Since current solver iteration is drawn only during the run,
   // turn off drawing solver data. (LOJ: for GMT-2526 fix)
   mDrawSolverData = false;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(false, false, true);
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   DrawFrame();
   
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(true, false, true);   
   
   mIsAnimationRunning = false;
   
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ViewAnimation() leaving, mIsAnimationRunning=%d\n",
       mIsAnimationRunning);
   #endif
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames,
//                  const UnsignedIntArray &objOrbitColors,
//                  const std::vector<SpacePoint*> &objArray)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGlObject(const StringArray &objNames,
                                  //const UnsignedIntArray &objOrbitColors,
                                  const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlObject() '%s' entered\n", mPlotName.WX_TO_C_STRING);
   #endif
   
   //ViewCanvas::SetGlObject(objNames, objOrbitColors, objArray);
   ViewCanvas::SetGlObject(objNames, objArray);
   
   // We don't want do set default view here since SetDefaultViewPoint() resets
   // mViewPointInitialized to false which makes mUseInitialViewPoint flag useless
   // Initialize objects used in view
   //SetDefaultViewPoint();
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlObject() '%s' leaving\n", mPlotName.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *internalCs,CoordinateSystem *viewCs,
//                       CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGlCoordSystem(CoordinateSystem *internalCs,
                                       CoordinateSystem *viewCs,
                                       CoordinateSystem *viewUpCs)
{
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlCoordSystem() '%s' entered\n", mPlotName.WX_TO_C_STRING);
   #endif
   
   ViewCanvas::SetGlCoordSystem(internalCs, viewCs, viewUpCs);
   
   // if view coordinate system origin is spacecraft, make spacecraft radius smaller.
   // So that spacecraft won't overlap each other.
   //@todo: need better way to scale spacecraft size.  See Dunn's comments above.
   if (viewCs->GetOrigin()->IsOfType(Gmat::SPACECRAFT))
      mScRadius = 30;
   else if (viewCs->GetOrigin()->IsOfType(Gmat::CELESTIAL_BODY))
      mScRadius = mObjectRadius[mOriginId] * 0.03;
   
   mMaxZoomIn = mObjMaxZoomIn[mOriginId];
   
   if (mUseInitialViewPoint)
      mCurrViewDistance = mMaxZoomIn;
   
   UpdateRotateFlags();
   
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlCoordSystem() '%s' leaving\n", mPlotName.WX_TO_C_STRING);
   #endif
   
} // end SetGlCoordSystem()


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane, bool drawXyPlane, ...)
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                                           bool drawXyPlane, bool drawWireFrame,
                                           bool drawAxes, bool drawGrid, bool drawSunLine,
                                           bool usevpInfo, bool drawStars,
                                           bool drawConstellations,
                                           Integer starCount)
{
   #ifdef DEBUG_DRAWING_OPTIONS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGl3dDrawingOption() entered, showLabels=%d, drawEcPlane=%d, "
       "drawXyPlane=%d, drawWireFrame=%d\n   drawAxes=%d, drawGrid=%d, drawSunLine=%d, "
       "usevpInfo=%d, drawStars=%d, drawConstellations=%d, starCount=%d\n", showLabels,
       drawEcPlane, drawXyPlane, drawWireFrame, drawAxes, drawGrid, drawSunLine,
       usevpInfo, drawStars, drawConstellations, starCount);
   #endif
   
   mShowLabels = showLabels;
   mDrawEclipticPlane = drawEcPlane;
   mDrawXyPlane = drawXyPlane;
   mDrawWireFrame = drawWireFrame;
   mDrawAxes = drawAxes;
   mDrawGrid = drawGrid;
   mDrawSunLine = drawSunLine;
   mUseInitialViewPoint = usevpInfo;
   mDrawStars = drawStars;
   mDrawConstellations = drawConstellations;
   mStarCount = starCount;
      
   if (mDrawWireFrame)
      mPolygonMode = GL_LINE;
   else
      mPolygonMode = GL_FILL;
   
   #ifdef DEBUG_DRAWING_OPTIONS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGl3dDrawingOption() mUseInitialViewPoint=%d, "
       "mPolygonMode=%d, GL_LINE=%d, GL_FILL=%d\n", mUseInitialViewPoint,
       mPolygonMode, GL_LINE, GL_FILL);
   #endif
   
   Refresh(false);
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
//                      SpacePoint *vdObj, Real vsFactor,
//                      const Rvector3 &vpRefVec, const Rvector3 &vpVec,
//                      const Rvector3 &vdVec, bool usevpRefVec,
//                      bool usevpVec, bool usevdVec)
//------------------------------------------------------------------------------
/*
 * Sets OpenGL view options
 *
 * @param <vpRefObj> Viewpoint reference object pointer
 * @param <vpVecObj> Viewpoint vector object pointer
 * @param <vdObj>  View direction object pointer
 * @param <vsFactor> Viewpoint scale factor
 * @param <vpRefVec> 3 element vector for viewpoint ref. vector (use if usevpVec is true)
 * @param <vpVec> 3 element vector for viewpoint vector (use if usevpVec is true)
 * @param <vdVec> 3 element vector for view direction (use if usevdVec is true)
 * @param <usevpRefVec> true if use vector for viewpoint reference vector
 * @param <usevpVec> true if use vector for viewpoint vector
 * @param <usevdVec> true if use vector for view direction
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                        SpacePoint *vdObj, Real vsFactor,
                                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                        const Rvector3 &vdVec, const std::string &upAxis,
                                        bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   pViewPointRefObj = vpRefObj;
   pViewPointVectorObj = vpVecObj;
   pViewDirectionObj = vdObj;
      
   mViewScaleFactor = vsFactor;
   mViewPointRefVector = vpRefVec;
   mViewPointVector = vpVec;
   mViewDirectionVector = vdVec;
   mViewUpAxisName = upAxis;
   mUseViewPointRefVector = usevpRefVec;
   mUseViewPointVector = usevpVec;
   mUseViewDirectionVector = usevdVec;
   
   Rvector3 lvpRefVec(vpRefVec);
   Rvector3 lvpVec(vpVec);
   Rvector3 lvdVec(vdVec);
   
   #if DEBUG_VIEW_OPTIONS
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::SetGlViewOption() pViewPointRefObj=%p, "
       "pViewPointVectorObj=%p\n   pViewDirectionObj=%p, mViewScaleFactor=%f   "
       "mViewPointRefVector=%s\n   mViewPointVector=%s, mViewDirectionVector=%s, "
       "mViewUpAxisName=%s\n   mUseViewPointRefVector=%d, mUseViewDirectionVector=%d\n",
       pViewPointRefObj, pViewPointVectorObj, pViewDirectionObj, mViewScaleFactor,
       lvpRefVec.ToString(10).c_str(), lvpVec.ToString(10).c_str(),
       lvdVec.ToString(10).c_str(), mViewUpAxisName.c_str(), mUseViewPointRefVector,
       mUseViewDirectionVector);
   #endif
   
   // Set viewpoint ref. object id
   if (!mUseViewPointRefVector && pViewPointRefObj)
   {
      mViewPointRefObjName = pViewPointRefObj->GetName();
      mVpRefObjId = GetObjectId(mViewPointRefObjName.c_str());
      
      if (mVpRefObjId == UNKNOWN_BODY)
      {
         mUseViewPointRefVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
             "pViewPointRefObj name=%s, so using vector=%s\n",
             pViewPointRefObj->GetName().c_str(),
             mViewPointRefVector.ToString().c_str());
      }
   }
   else
   {
      mViewPointRefObjName = "Earth";
      
      if (!mUseViewPointRefVector)
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewPointRefObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
   // Set viewpoint vector object id
   if (!mUseViewPointVector && pViewPointVectorObj)
   {
      mVpVecObjId = GetObjectId(pViewPointVectorObj->GetName().c_str());
      
      if (mVpVecObjId == UNKNOWN_BODY)
      {
         mUseViewPointVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
             "pViewPointVectorObj name=%s, so using vector=%s\n",
             pViewPointVectorObj->GetName().c_str(),
             mViewPointVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewPointVector)
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewPointVectorObject is NULL, "
             "so will use default Vector instead.\n");
   }
   
   // Set view direction object id
   if (!mUseViewDirectionVector && pViewDirectionObj)
   {
      mViewObjName = pViewDirectionObj->GetName().c_str();
      mVdirObjId = GetObjectId(mViewObjName.c_str());
      
      if (mVdirObjId == UNKNOWN_BODY)
      {
         mUseViewDirectionVector = true;
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() Cannot find "
             "pViewDirectionObj name=%s, so using vector=%s\n",
             pViewDirectionObj->GetName().c_str(),
             mViewDirectionVector.ToString().c_str());
      }
   }
   else
   {
      if (!mUseViewDirectionVector)
         MessageInterface::ShowMessage
            ("*** Warning *** OrbitViewCanvas::SetGlViewOption() "
             "ViewDirectionObject is NULL,"
             "so will use default Vector instead.\n");
   }
   
} //end SetGl3dViewOption()


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
void OrbitViewCanvas::TakeAction(const std::string &action)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::TakeAction() entered, action = '%s'\n", action.c_str());
   #endif
   
   // Any actions to handle in this plot?
   
   ViewCanvas::TakeAction(action);
}


//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxPaintEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnPaint(wxPaintEvent& event)
{
   #ifdef DEBUG_ON_PAINT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::OnPaint() '%s' entered, theContext=<%p>, mFatalErrorFound=%s, mGlInitialized=%d, mObjectCount=%d\n",
       mPlotName.WX_TO_C_STRING, theContext,
       (mFatalErrorFound? "true" : "false"), mGlInitialized, mObjectCount);
   #endif
   
   // must always be here
   wxPaintDC dc(this);
   
   // Check for any error condition
   if (mFatalErrorFound)
   {
      #ifdef DEBUG_ON_PAINT
      MessageInterface::ShowMessage("**** ERROR **** fatal error found\n");
      #endif
      return;
   }
   
   if (!SetGLContext("in OrbitViewCanvas::OnPaint()"))
      return;
   
   if (!mGlInitialized && mObjectCount > 0)
   {
      #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("OrbitViewCanvas::OnPaint() Calling InitializePlot()\n");
      #endif
      InitializePlot();
      mGlInitialized = true;
   }
   
   SetDrawingMode();
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = true;
   #endif

   
   #ifdef DISABLE_REPAINT_IN_FUNCTION
   if (mIsEndOfRun && mInFunction)
   {
      if (mWriteRepaintDisalbedInfo)
      {
         Freeze();
         wxString msg = "*** WARNING *** This plot data was published inside a "
            "function, so repainting or drawing animation is disabled.\n";
         MessageInterface::ShowMessage(msg.c_str());
         GmatAppData::Instance()->GetMainFrame()->EnableAnimation(false);
         
         mWriteRepaintDisalbedInfo = false;
      }
      return;
   }
   #endif
   
   DrawPlot();
   
}


//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxSizeEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnSize(wxSizeEvent& event)
{
   // Linux specific handler for sizing
   #ifdef __WXGTK__
      if (hasBeenPainted == false)
         return;
   #endif
   
   // this is also necessary to update the context on some platforms
//   wxGLCanvas::OnSize(event);
   
   // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
   int nWidth, nHeight;
   GetClientSize(&nWidth, &nHeight);
   mCanvasSize.x = nWidth;
   mCanvasSize.y = nHeight;
   
   if (!SetGLContext("in OrbitViewCanvas::OnSize()"))
      return;
   
   // Need this to make picture not to stretch to canvas
   ChangeProjection(nWidth, nHeight, mCurrViewDistance);
   glViewport(0, 0, (GLint) nWidth, (GLint) nHeight);
   
   Refresh(false);
   Update();
}


//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxMouseEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnMouse(wxMouseEvent& event)
{
   #ifdef DEBUG_ON_MOUSE
   MessageInterface::ShowMessage
      ("===> OnMouse() mUseInitialViewPoint=%d, mIsEndOfData=%d\n",
       mUseInitialViewPoint, mIsEndOfData);
   #endif
   
   #ifdef DISABLE_REPAINT_IN_FUNCTION
   if (mIsEndOfData && mInFunction)
   {
      if (mWriteRepaintDisalbedInfo)
      {
         //Freeze();
         wxString msg = "*** WARNING *** This plot data was published inside a "
            "function, so repainting or drawing animation is disabled.\n";
         MessageInterface::ShowMessage(msg.c_str());
         GmatAppData::Instance()->GetMainFrame()->EnableAnimation(false);
         
         mWriteRepaintDisalbedInfo = false;
      }
      return;
   }
   #endif
   
   int flippedY;
   int width, height;
   int mouseX, mouseY;
   
   GetClientSize(&width, &height);
   
   mouseX = event.GetX();
   mouseY = event.GetY();
   
   // First, flip the mouseY value so it is oriented right (bottom left is 0,0)
   flippedY = height - mouseY;
   
   GLfloat fEndX = mfLeftPos + ((GLfloat)mouseX /(GLfloat)width) *
      (mfRightPos - mfLeftPos);
   GLfloat fEndY = mfBottomPos + ((GLfloat)flippedY /(GLfloat)height)*
      (mfTopPos - mfBottomPos);
   
   //if mouse dragging
   if (event.Dragging())
   {
      //------------------------------
      // translating
      //------------------------------
      if ((mControlMode != MODE_ASTRONAUT_6DOF && event.ShiftDown() && event.LeftIsDown()) ||
         (mControlMode == MODE_ASTRONAUT_6DOF && event.LeftIsDown()))
      {
         // Do a X/Y Translate of the camera
         mfCamTransX = (fEndX - mfStartX) * mInversion;
         mfCamTransY = (fEndY - mfStartY) * mInversion;

         mCamera.Translate(mfCamTransX, mfCamTransY, 0.0, true);
         
         // repaint
         Refresh(false);
      }
      //------------------------------
      // rotating
      //------------------------------
      else if ((mControlMode != MODE_ASTRONAUT_6DOF && event.LeftIsDown()) ||
         (mControlMode == MODE_ASTRONAUT_6DOF && event.RightIsDown()))
      {
         // PS - Attempting a new form of view rotation
         //   Rather than apply a rotation based on quaternions and all of that
         //   complication, we move the camera position based
         //   on the mouse movement. 
         // The angles used are based on how far the mouse moved
         float angleX = (mLastMouseX - mouseX) / 200.0 * mInversion,
            angleY = (mLastMouseY - mouseY) / 200.0 * mInversion;
         if (mControlMode == MODE_CENTERED_VIEW)
         {
            mCamera.Rotate(angleX, angleY, 0.0, false, true);
         }
         else
         {
            mCamera.Rotate(angleX, angleY, 0.0, false, false);
         }
         
         // repaint
         Refresh(false); 
         
      }
      //------------------------------
      // translating forward and backward
      //------------------------------
      /*else if (mControlMode != MODE_ASTRONAUT_6DOF && event.ShiftDown() && event.RightIsDown())
      {
         // find the length
         Real length = mLastMouseY - mouseY;
         length *= 100;
         mCamera.Translate(0.0, 0.0, length, true);

         Refresh(false);
      }*/
      //------------------------------
      // FOV Zoom
      //------------------------------
      else if (event.ShiftDown() && event.RightIsDown())
      {
         // Find the zoom amount
//         Real x2 = (mLastMouseX - mouseX) * (mLastMouseX - mouseX);
//         Real y2 = (mouseY - mLastMouseY) * (mouseY - mLastMouseY);
//         Real length = sqrt(x2 + y2);
         Real length = abs(mouseY - mLastMouseY);
         Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
         
//         mZoomAmount = length * distance / 1000000;
         Real maxLimiter = (mCamera.fovDeg < 30.0 ? mCamera.fovDeg : 30.0);
         mZoomAmount = length * distance / 5000000.0 * maxLimiter;
         
         if (mouseY > mLastMouseY)
            mCamera.ZoomOut(mZoomAmount);
         else
            mCamera.ZoomIn(mZoomAmount);
         
         Refresh(false);
      }
      //------------------------------
      // "zooming"
      //------------------------------
      else if (mControlMode != MODE_ASTRONAUT_6DOF && event.RightIsDown())
      {            
         // if end-of-run compute new mfCamRotXYZAngle by calling ChangeView()
         if (mIsEndOfRun)
            ChangeView(mCurrRotXAngle, mCurrRotYAngle, mCurrRotZAngle);
         
         // Find the zoom amount
//         Real x2 = (mLastMouseX - mouseX) * (mLastMouseX - mouseX);
//         Real y2 = (mouseY - mLastMouseY) * (mouseY - mLastMouseY);
//         Real length = sqrt(x2 + y2);
         Real length = abs(mouseY - mLastMouseY);
         Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
         
         mZoomAmount = length * distance / 500;
         
         #ifdef DEBUG_ON_MOUSE
         MessageInterface::ShowMessage
            ("   mCamera.view_center = %s   mCamera.position    = %s",
             mCamera.view_center.ToString().c_str(), mCamera.position.ToString().c_str());
         MessageInterface::ShowMessage
            ("   mouse length=%f, distance=%f, mZoomAmount=%f\n", length, distance, mZoomAmount);
         #endif
         
         // For zoom in out, move camera forward or backward in z direction
         // If dragging from upper right corner to lower left corner (zoom in)
         //if (mouseX < mLastMouseX && mouseY > mLastMouseY)

         // If dragging from top to bottom, zoom in
         if (mouseY > mLastMouseY)
         {
            // In the future, if we want to limit the zoom in, we can check for
            // the distance, ie. if (distance < (mOriginRadius * 1.2)) return
            mCamera.Translate(0, 0, mZoomAmount, false);
         }
         // If dragging from lower left corner to upper right corner (zoom out)
         //else if (mouseX > mLastMouseX && mouseY < mLastMouseY)

         // If dragging from bottom to top, zoom out
         else if (mouseY < mLastMouseY)
         {
            mCamera.Translate(0, 0, -mZoomAmount, false);
         }

         // per GMT-3600, left/right motion does not apply zooming
//         else
//         {
//            // If mouse moves toward left then zoom in
//            if (mouseX < mLastMouseX || mouseY < mLastMouseY)
//            {
//               // In the future, if we want to limit the zoom in, we can check for
//               // the distance, ie. if (distance < (mOriginRadius * 1.2)) return
//               mCamera.Translate(0, 0, mZoomAmount, false);
//            }
//            // Else mouse moves toward right so zoom out
//            else
//               mCamera.Translate(0, 0, -mZoomAmount, false);
//         }
         
         Refresh(false);
      }
      //------------------------------
      // roll
      //------------------------------
      else if (event.MiddleIsDown())
      {
         float roll = (mouseY - mLastMouseY) / 400.0 * mInversion;
         if (mControlMode == MODE_CENTERED_VIEW)
         {
            mCamera.Rotate(0.0, 0.0, roll, false, true);
         }
         else
            mCamera.Rotate(0.0, 0.0, roll, false, false);
         Refresh(false);
      }
   }
   // Mousewheel movements
   else if (event.GetWheelRotation() != 0 && mControlMode == MODE_ASTRONAUT_6DOF)
   {
      float rot = event.GetWheelRotation();
      Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
      Real movement = rot * distance / 3000;

      if (event.ShiftDown() && rot > 0)
      {
         mCamera.ZoomIn(1);
      }
      else if (event.ShiftDown() && rot < 0)
      {
         mCamera.ZoomOut(1);
      }
      else if (rot > 0)
      {
         mCamera.Translate(0.0, 0.0, movement, true);
      }
      else if (rot < 0)
      {
         mCamera.Translate(0.0, 0.0, movement, true);
      }
      Refresh(false);
   } // end if (event.Dragging())
   
   // ensures the directional vectors for the viewpoint are still orthogonal
   mCamera.ReorthogonalizeVectors();
   
   // save last position
   mLastMouseX = mouseX;
   mLastMouseY = mouseY;
   
   mfStartX = fEndX;
   mfStartY = fEndY;
   
   #ifdef __WRITE_GL_MOUSE_POS__
   wxString mousePosStr;
   //mousePosStr.Printf("X = %g Y = %g", fEndX, fEndY);
   mousePosStr.Printf("X = %g Y = %g mouseX = %d, mouseY = %d",
                      fEndX, fEndY, mouseX, mouseY);   
   theStatusBar->SetStatusText(mousePosStr, 2);
   //wxLogStatus(MdiGlPlot::mdiParentGlFrame,
   //            wxT("X = %d Y = %d lastX = %f lastY = %f Zoom amount = %f Distance = %f"),
   //            event.GetX(), event.GetY(), mfStartX, mfStartY, mZoomAmount, mCurrViewDistance);
   #endif
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes wxKeyEvent.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::OnKeyDown(wxKeyEvent &event)
{
   int keyDown = event.GetKeyCode();
   if (keyDown == 'w' || keyDown == 'W')
   {
      mCamera.Translate(0.0, 0.0, 300, true);
   }
   else if (keyDown == 's' || keyDown == 'S')
   {
      mCamera.Translate(0.0, 0.0, -300, true);
   }
   else if (keyDown == 'a' || keyDown == 'A')
   {
      mCamera.Translate(-300, 0.0, 0.0, true);
   }
   else if (keyDown == 'd' || keyDown == 'D')
   {
      mCamera.Translate(300, 0.0, 0.0, true);
   }
   else if (keyDown == 'z' || keyDown == 'Z')
   {
      if (event.ShiftDown())
      {
         mControlMode = MODE_ASTRONAUT_6DOF;
      }
      else
      {
         if (mControlMode == MODE_ASTRONAUT_6DOF)
            mControlMode = MODE_FREE_FLYING;
         else
         {
            mControlMode = 1 - mControlMode;
         }
      }
   }
   else if (keyDown == 'i' || keyDown == 'I')
   {
      mInversion *= -1;
   }
   /*else if (keyDown == 't' || keyDown == 'T')
    * {
      if (mCamera.TrackingMode() == 1)
         mCamera.Untrack();
      else
         mCamera.TrackStill(0);
   }
   else if (keyDown == 'f' || keyDown == 'F')
   {
      if (mCamera.TrackingMode() == 2)
         mCamera.Untrack();
      else
         mCamera.TrackFollow(0);
   }*/
   else if (keyDown == WXK_ESCAPE)
      mHasUserInterrupted = true;
   
   // ensures the directional vectors for the viewpoint are still orthogonal
   mCamera.ReorthogonalizeVectors();
   Refresh(false);
}


//------------------------------------------------------------------------------
// void SetDefaultViewPoint()
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetDefaultViewPoint()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("OrbitViewCanvas::SetDefaultViewPoint() entered\n");
   #endif
   
   mViewPointInitialized = false;
   mViewPointRefObjName = "UNKNOWN";
   
   pViewPointRefObj = NULL;
   pViewPointVectorObj = NULL;
   pViewDirectionObj = NULL;
   
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVector.Set(DEFAULT_DISTANCE, 0.0, 0.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   //mCamera.Reset();
   //mCamera.Relocate(DEFAULT_DISTANCE, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   mViewScaleFactor = 1.0;
   mUseViewPointRefVector = true;
   mUseViewPointVector = true;
   mUseViewDirectionVector = true;
   mVpRefObjId = UNKNOWN_OBJ_ID;
   mVpVecObjId = UNKNOWN_OBJ_ID;
   mVdirObjId = UNKNOWN_OBJ_ID;
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("OrbitViewCanvas::SetDefaultViewPoint() leavng\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeViewPoint()
//------------------------------------------------------------------------------
void OrbitViewCanvas::InitializeViewPoint()
{
   #if DEBUG_INIT
   MessageInterface::ShowMessage("==========> OrbitViewCanvas::InitializeViewPoint() entered\n");
   #endif
   
   // Dunn took out minus signs below to position vectors correctly in the 
   // ECI reference frame.

   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitializeViewPoint() entered\n");
   #endif
   
   wxString name;
   int objId;
   int index;
   Rvector3 refVec, viewpoint, direction;
   
   if (mUseViewPointRefVector)
   {
      refVec = mViewPointRefVector;
   }
   else
   {
      name = pViewPointRefObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * maxData * 3 + (mLastIndex*3);
      refVec = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   
   if (mUseViewPointVector)
   {
      viewpoint = mViewPointVector;
   }
   else
   {
      name = pViewPointVectorObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * maxData * 3 + (mLastIndex*3);
      viewpoint = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   viewpoint *= mViewScaleFactor;
      
   if (mUseViewDirectionVector)
   {
      direction = mViewDirectionVector;
   }
   else
   {
      name = pViewDirectionObj->GetName().c_str();
      objId = GetObjectId(name);
      index = objId * maxData * 3 + (mLastIndex*3);
      direction = Rvector3(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
   }
   
   mCamera.Reset();
   
   // Set the direction of up vector
   if (mViewUpAxisName == "X")
      mCamera.up = Rvector3(1.0, 0.0, 0.0);
   else if (mViewUpAxisName == "-X")
      mCamera.up = Rvector3(-1.0, 0.0, 0.0);
   else if (mViewUpAxisName == "Y")
      mCamera.up = Rvector3(0.0, 1.0, 0.0);
   else if (mViewUpAxisName == "-Y")
      mCamera.up = Rvector3(0.0, -1.0, 0.0);
   else if (mViewUpAxisName == "Z")
      mCamera.up = Rvector3(0.0, 0.0, 1.0);
   else if (mViewUpAxisName == "-Z")
      mCamera.up = Rvector3(0.0, 0.0, -1.0);

   Rvector3 viewPos = refVec + viewpoint;
   Rvector3 viewDiff = viewPos - direction;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage("   mCamera.up = %s", mCamera.up.ToString().c_str());
   MessageInterface::ShowMessage("   refVec     = %s", refVec.ToString().c_str());
   MessageInterface::ShowMessage("   viewpoint  = %s", viewpoint.ToString().c_str());
   MessageInterface::ShowMessage("   viewPos    = %s", viewPos.ToString().c_str());
   MessageInterface::ShowMessage("   direction  = %s", viewPos.ToString().c_str());
   MessageInterface::ShowMessage("   viewDiff   = %s", viewDiff.ToString().c_str());
   #endif
   
   // If view difference is not zero then relocate camera
   if (!viewDiff.IsZeroVector())
   {
      #if DEBUG_INIT
      MessageInterface::ShowMessage
         ("=====> relocating camera to\n    position = %s   direction = %s",
          viewPos.ToString().c_str(), direction.ToString().c_str());
      #endif
      
      mCamera.Relocate(viewPos, direction);
   }

   // Moved to ViewCanvas
   //mViewPointInitialized = true;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::InitializeViewPoint() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetDefaultView()
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetDefaultView()
{
   mCurrRotXAngle = mDefaultRotXAngle;
   mCurrRotYAngle = mDefaultRotYAngle;
   mCurrRotZAngle = mDefaultRotZAngle;
   mCurrViewDistance = mDefaultViewDistance;
   mfCamTransX = 0;
   mfCamTransY = 0;
   mfCamTransZ = 0;
   mfCamRotXAngle = 0;
   mfCamRotYAngle = 0;
   mfCamRotZAngle = 0;
   
   #ifdef USE_TRACKBALL
   ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
   #endif   
}


//------------------------------------------------------------------------------
//  void SetupProjection()
//------------------------------------------------------------------------------
/**
 * Sets up how object is viewed
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetupProjection()
{
   // Setup the world view
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glLoadIdentity();
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
}


//------------------------------------------------------------------------------
//  void SetupWorld()
//------------------------------------------------------------------------------
/**
 * Sets world view as perspective projection. gluPerspective specifies a viewing
 * frustum into the world coordinate system. In general, the aspect ratio in
 * gluPerspective should match the aspect ratio of the associated viewport.
 * For example, aspect = 2.0 means the viewer's angle of view is twice as wide
 * in x as it is in y. If the viewport is twice as wide as it is tall, it displays
 * the image without distortion.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::SetupWorld()
{
   // Setup how we view the world
   GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
   
   // PS - Greatly simplified. Uses the FOV from the active camera, the aspect ratio of the 
   //    screen, and a constant near-far plane
   // LOJ - Changed to use variable near plane distance as it affects plot appearance with objet
   //    size and distance (2012.06.28)
   float nearDist = (mCamera.position - mCamera.view_center).GetMagnitude() * 0.001;
   float farDist = (mCamera.position - mCamera.view_center).GetMagnitude() * 2;
   if (nearDist < 0.001f)
      nearDist = 0.001f;
   if (farDist < 500000000.0f)
      farDist = 500000000.0f;
   
   //-----------------------------------------------------------------
   // void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
   // fovy   : Specifies the field of view angle, in degrees, in the y direction.
   // aspect : Specifies the aspect ratio that determines the field of view in the x direction.
   //          The aspect ratio is the ratio of x (width) to y (height).
   // zNear  : Specifies the distance from the viewer to the near clipping plane
   //          (always positive).
   // zFar   : Specifies the distance from the viewer to the far clipping plane
   //          (always positive).
   //-----------------------------------------------------------------
   
   #ifdef DEBUG_SETUP_WORLD
   MessageInterface::ShowMessage
      ("SetupWorld(), fov = %f, aspect = %f, nearDist = %f, farDist = %f\n",
       mCamera.fovDeg, aspect, nearDist, farDist);
   #endif
   
   // Changed zNear from 50 to 0.1 to fix GMT-2386 (LOJ: 2012.06.27)
   // Changed to use variable near plane distance as it affects plot with objet
   // size and distance (LOJ: 2012.06.28)
   //gluPerspective(mCamera.fovDeg, aspect, 0.1f, farDist);
   gluPerspective(mCamera.fovDeg, aspect, nearDist, farDist);
   
   //-----------------------------------------------------------------
   // Note: mouse rotation is applied in TransformView as MODELVIEW mode
   //-----------------------------------------------------------------
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   
   int index = mViewObjId * maxData * 3 + mLastIndex * 3;
   glTranslatef(mObjectViewPos[index+0], mObjectViewPos[index+1],
                -mObjectViewPos[index+2]);
   
} // end SetupWorld()


//------------------------------------------------------------------------------
//  void ChangeView(float viewX, float viewY, float viewZ)
//------------------------------------------------------------------------------
/**
 * Changes view by rotating the camera.
 *
 * @param <viewX> rotation angle of X component.
 * @param <viewY> rotation angle of Y component.
 * @param <viewZ> rotation angle of Z component.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ChangeView(float viewX, float viewY, float viewZ)
{

   mfCamRotXAngle = (int)(viewX) % 360 + 270;
   mfCamRotYAngle = (int)(viewY) % 360;
   mfCamRotZAngle = (int)(viewZ) % 360;

   #ifdef DEBUG_CHANGE_VIEW
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ChangeView()entered,  mfCamRotXYZAngle = %f %f %f\n",
       mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
   #endif
   
   // don't let the rotation angles build up to some insane size
   if (mfCamRotYAngle > 360)
      mfCamRotYAngle -= 360;
   else if (mfCamRotYAngle < 0)
      mfCamRotYAngle += 360;
   
   // don't let the rotation angles build up to some insane size
   if (mfCamRotXAngle > 450)
      mfCamRotXAngle -= 360;
   else if (mfCamRotXAngle < 90)
      mfCamRotXAngle += 360;
   
   // don't let the rotation angles build up to some insane size
   if (mfCamRotZAngle > 360)
      mfCamRotZAngle -= 360;
   else if (mfCamRotZAngle < 0)
      mfCamRotZAngle += 360;
   
   #ifdef DEBUG_CHANGE_VIEW
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ChangeView() levaing,  mfCamRotXYZAngle = %f %f %f\n",
       mfCamRotXAngle, mfCamRotYAngle, mfCamRotZAngle);
   #endif
} // end ChangeView()


//------------------------------------------------------------------------------
//  void ChangeProjection(int width, int height, float axisLength)
//------------------------------------------------------------------------------
/**
 * Changes view projection by viewing area in pixel and axis length.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::ChangeProjection(int width, int height, float axisLength)
{    
   GLfloat fAspect = (GLfloat) height / (GLfloat) width;
   
   GLfloat fViewLeft   = -axisLength/2;
   GLfloat fViewRight  =  axisLength/2;
   GLfloat fViewTop    =  axisLength/2;
   GLfloat fViewBottom = -axisLength/2;
   
   // This makes texture maps upside down
   //fViewTop    = -axisLength/2;
   //fViewBottom =  axisLength/2;
   
   // Save the size we are setting the projection for later use
   if (width <= height)
   {
      mfLeftPos = fViewLeft;
      mfRightPos = fViewRight;
      mfBottomPos = fViewBottom * fAspect;
      mfTopPos = fViewTop * fAspect;
   }
   else
   {
      mfLeftPos = fViewLeft / fAspect;
      mfRightPos = fViewRight / fAspect;
      mfBottomPos = fViewBottom;
      mfTopPos = fViewTop;
   }
}


//------------------------------------------------------------------------------
// void TransformView()
//------------------------------------------------------------------------------
/**
 * Calls gluLookAt() to display the view using the camera.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::TransformView()
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==> OrbitViewCanvas::TransformView() mIsEndOfData=%d, mIsEndOfRun=%d\n",
       mIsEndOfData, mIsEndOfRun);
   #endif
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   camera position    = %s\n   camera view_center = %s\n   camera up vector   = %s\n",
       mCamera.position.ToString().c_str(), mCamera.view_center.ToString().c_str(),
       mCamera.up.ToString().c_str());
   #endif
   
   glLoadIdentity();
   
   //-----------------------------------------------------------------
   //gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez,
   //          GLdouble centerx, GLdouble centery, GLdouble centerz,
   //          GLdouble upx, GLdouble upy, GLdouble upz);
   // eyex    : The position of the eye point.
   // eyey    : The position of the eye point.
   // eyez    : The position of the eye point.
   // centerx : The position of the reference point.
   // centery : The position of the reference point.
   // centerz : The position of the reference point.
   // upx     : The direction of the up vector.
   // upy     : The direction of the up vector.
   // upz     : The direction of the up vector.
   //-----------------------------------------------------------------
   
   gluLookAt(mCamera.position[0], mCamera.position[1], mCamera.position[2],
             mCamera.view_center[0], mCamera.view_center[1], mCamera.view_center[2],
             mCamera.up[0], mCamera.up[1], mCamera.up[2]);
   
} // end TransformView()


//------------------------------------------------------------------------------
// virtual void HandleLightSource()
//------------------------------------------------------------------------------
/**
 * Enables to use Sun as the light source.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::HandleLightSource()
{
   int sunId = GetObjectId("Sun");
   int frame = mLastIndex;
   
   if (sunId == UNKNOWN_OBJ_ID)
   {
      mLight.SetPosition(0.01f, 1.0f, 0.3f);
      mLight.SetDirectional(true);
   }
   else
   {
      int index = sunId * maxData * 3 + frame * 3;
      mLight.SetPosition(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
      mLight.SetDirectional(false);
   }
   
   // Dunn is setting sunlight to be a little dimmer.
   mLight.SetColor(0.8f, 0.8f, 0.8f, 1.0f);
   
   // If 4th value is zero, the light source is directional one, and
   // (x,y,z) values describes its direction.
   // If 4th value is nonzero, the light is positional, and the (x,y,z) values
   // specify the location of the light in homogeneous object coordinates.
   // By default, a positional light radiates in all directions.
   
   // Reset the light position to reflect the transformations
   float lightPos[4], *lightColor = mLight.GetColor();
   mLight.GetPositionf(lightPos);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
   
   // Enable the lighting
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}


//------------------------------------------------------------------------------
//  void DrawFrame()
//------------------------------------------------------------------------------
/**
 * Draws saved frames for viewing animation.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawFrame()
{
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawFrame() entered, mNumData=%d, mUsenitialViewPoint=%d\n"
       "   mViewCoordSysName=%s, mFrameInc=%d\n", mNumData, mUseInitialViewPoint,
       mViewCoordSysName.c_str(), mFrameInc);
   #endif
   
   if (mUseInitialViewPoint)
   {
      #ifdef USE_TRACKBALL
         ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
      #endif
         
      SetDefaultView();
      UpdateRotateFlags();
      
      // set view center object
      mOriginName = wxString(pViewCoordSystem->GetOriginName().c_str());
      mOriginId = GetObjectId(mOriginName);
      
      mViewObjName = mOriginName;
      GotoObject(mViewObjName);
   }
   
   int numberOfData = mNumData;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mCurrIndex = 0;

   Publisher *publisher = GuiPublisher::Instance();

   // refresh every 50 points (Allow user to set frame this increment?)
   for (int frame = 1; frame <= numberOfData; frame+=mFrameInc)
   {
      mIsAnimationRunning = true;

      // Yield periodically
      publisher->Ping();

      if (mHasUserInterrupted)
         break;
      
      //Sleep(mUpdateInterval);
      wxMilliSleep(mUpdateInterval);
      
      mNumData = frame;
      ComputeRingBufferIndex();
      
      Refresh(false);
   }
   
   // final refresh, in case number of points is less than 50
   Refresh(false);
   
   mNumData = numberOfData;
   mIsEndOfData = true;
   mIsEndOfRun = true;
   mIsAnimationRunning = false;
   
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage("OrbitViewCanvas::DrawFrame() leaving\n");
   #endif
} // end DrawFrame()


//------------------------------------------------------------------------------
//  void DrawPlot()
//------------------------------------------------------------------------------
/**
 * Draws whole plot.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawPlot()
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawPlot() mTotalPoints=%d at beginning\n",
       mTotalPoints);
   #endif

   if (mTotalPoints == 0)
      return;
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   mRedrawLastPointsOnly=%d, mNumPointsToRedraw=%d, mViewCsIsInternalCs=%d, "
       "mUseInitialViewPoint=%d, mCurrViewDistance=%f\n", mRedrawLastPointsOnly, mNumPointsToRedraw,
       mViewCsIsInternalCs, mUseInitialViewPoint, mCurrViewDistance);
   MessageInterface::ShowMessage
      ("   mIsEndOfData=%d, mIsEndOfRun=%d, mDrawSolverData=%d, mIsAnimationRunning=%d\n",
       mIsEndOfData, mIsEndOfRun, mDrawSolverData, mIsAnimationRunning);
   #endif
   
   // Set background color to black
   glClearColor(0.0, 0.0, 0.0, 1.0);
   
   if (mRedrawLastPointsOnly || mNumPointsToRedraw == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_DEPTH_BUFFER_BIT);
   
   #if DEBUG_SOLVER_STATUS
   if (mIsSolving && !mIsEndOfRun)
      DrawDebugMessage(" Solving", GmatColor::YELLOW, 0, 100);
   else
      DrawDebugMessage("        ", GmatColor::YELLOW, 0, 100);
   #endif
   
   // Draw epoch at the bottom of the screen
   DrawStatus("", GmatColor::YELLOW, "  Epoch: ", mTime[mLastIndex], 0, 5);
   
   // Plot is not refreshed when another panel is opened, so add glFlush()
   // and SwapBuffers() 
   if (mNumData < 1 && !mDrawSolverData) // to avoid 0.0 time
   {
      glFlush();
      SwapBuffers();
      return;
   }
   
   ChangeProjection(mCanvasSize.x, mCanvasSize.y, mCurrViewDistance);
   
   glDisable(GL_LIGHTING);
   
   // draw stars
   if (mDrawStars)
      DrawStars();
   
   SetupProjection();
   TransformView();
   
   // draw axes
   if (mDrawAxes)
      DrawAxes();
   
   // draw equatorial plane
   if (mDrawXyPlane)
      DrawXYPlane(mXyPlaneColor);
   
   // draw ecliptic plane
   if (mDrawEclipticPlane)
   {
      if (pMJ2000EcCoordSystem == NULL)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Cannot compute MJ2000Ec coordinate system, so drawing "
             "ecliptic plane is turned off\n");
         mDrawEclipticPlane = false;
      }
      else
         DrawEclipticPlane(mEcPlaneColor);
   }
   
   // draw object orbit
   DrawObjectOrbit();
   
   if (mDrawSolverData)
      DrawSolverData();
   
   // draw Earth-Sun line
   if (mDrawSunLine)
      DrawSunLine();
   
   glFlush();
   SwapBuffers();
   
} // end DrawPlot()


//------------------------------------------------------------------------------
//  void DrawObjectOrbit()
//------------------------------------------------------------------------------
/**
 * Draws object orbit and object at the frame number. The frame is the index
 * of the data buffer which starts at 0.
 *
 * @param  frame  Frame number to be used for drawing
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawObjectOrbit()
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==========> DrawObjectOrbit() entered, mLastIndex=%d\n", mLastIndex);
   #endif
   
   int objId;
   wxString objName;
   
   // we don't want the orbit paths lit
   if (mEnableLightSource)
      glDisable(GL_LIGHTING);
   
   ComputeActualIndex();
   
   for (int obj=0; obj<mObjectCount; obj++)
   {
      objName = mObjectNames[obj];
      objId = GetObjectId(objName);
      mObjLastFrame[objId] = 0;
      
      int colorIndex = objId * maxData + mLastIndex;
      
      #if DEBUG_DRAW
      MessageInterface::ShowMessage
         ("DrawObjectOrbit() obj=%d, objId=%d, objName='%s', colorIndex=%d, mDrawOrbitFlag[%d]=%d\n",
          obj, objId, objName.WX_TO_C_STRING, colorIndex, colorIndex, mDrawOrbitFlag[colorIndex]);
      #endif
      
      // If not showing orbit just draw object, continue to next one
      if (!mDrawOrbitFlag[colorIndex])
      {
         #if DEBUG_DRAW
         MessageInterface::ShowMessage
            ("==> Not drawing orbit of '%s', so skip\n", objName.WX_TO_C_STRING);
         #endif
         
         // just draw object texture and continue
         if (mShowObjectMap[objName])
            DrawObjectTexture(objName, obj, objId);
         
         continue;
      }
      
      // always draw orbit trajectory
      DrawOrbit(objName, obj, objId);
      
      //---------------------------------------------------------
      //draw object with texture
      //---------------------------------------------------------      
      if (mShowObjectMap[objName])
      {            
         DrawObjectTexture(objName, obj, objId);
      }
   }
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage("==========> DrawObjectOrbit() leaving\n");
   #endif
} // end DrawObjectOrbit()


//------------------------------------------------------------------------------
// void DrawObjectTexture(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawObjectTexture(const wxString &objName, int obj,
                                        int objId)
{
   if (mNumData < 1)
      return;
   
   int frame = mObjLastFrame[objId];
   int index1 = objId * maxData * 3 + frame * 3;
   bool drawingSpacecraft = false;
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("DrawObjectTexture() entered, objName='%s', obj=%d, objId=%d, frame=%d, "
       "index1=%d, mObjLastFrame[%d]=%d\n", objName.WX_TO_C_STRING, obj, objId, frame,
       index1, objId, mObjLastFrame[objId]);
   MessageInterface::ShowMessage
      ("   mObjectViewPos=%f, %f, %f\n", mObjectViewPos[index1+0],
       mObjectViewPos[index1+1], mObjectViewPos[index1+2]);
   #endif
   
   glPushMatrix();
   
   // First disable GL_TEXTURE_2D to show lines clearly
   // without this, lines are drawn dim (loj: 2007.06.11)
   glDisable(GL_TEXTURE_2D);

   // Enable light source on option
   if (mEnableLightSource)
      HandleLightSource();
   
   // Draw spacecraft
   if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
   {
      drawingSpacecraft = true;
      // If drawing at current position is on
      if (mIsDrawing[frame])
      {
         Spacecraft *sat = (Spacecraft*)mObjectArray[obj];
         
         #if DEBUG_DRAW
         MessageInterface::ShowMessage
            ("   Drawing spacecraft '%s' with modelId: %d\n", objName.WX_TO_C_STRING,
             sat->GetModelId());
         #endif
         
         if (sat->GetModelId() != -1)
         {
            DrawSpacecraft3dModel(sat, objId, frame);
         }
         else
         {
            // Dunn took out old minus signs to make attitude correct.
            glTranslatef(mObjectViewPos[index1+0],
                         mObjectViewPos[index1+1],
                         mObjectViewPos[index1+2]);
            GlColorType *yellow = (GlColorType*)&GmatColor::YELLOW;
            //GlColorType *red = (GlColorType*)&GmatColor::RED;
            *sIntColor = mObjectOrbitColor[objId * maxData + mObjLastFrame[objId]];
            // We want to differenciate spacecraft by orbit color so pass sGlColor (LOJ: 2011.02.16)
            //DrawSpacecraft(mScRadius, yellow, red);
            
            DrawSpacecraft(mScRadius, yellow, sGlColor, false);
         }
      }
   }
   else
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage("   Drawing body '%s'\n", objName.WX_TO_C_STRING);
      #endif
      
      GmatBase* tmpObj = mObjectArray[obj];
      CelestialBody *body = NULL;
      if (tmpObj->IsOfType(Gmat::CELESTIAL_BODY))
         body = (CelestialBody*)tmpObj;
      
      // put object at final position
      // Dunn took out minus signs
      glTranslatef(mObjectViewPos[index1+0],mObjectViewPos[index1+1],mObjectViewPos[index1+2]);
      
      // If body has 3d model file, use it
      if (body != NULL && body->Get3dViewModelId() != -1)
      {
         DrawCelestialBody3dModel(body, objName, objId, frame);
      }
      else
      {
         DrawObject(objName, obj);
      }
   }
   
   if (mEnableLightSource)
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
   }
   
   //============================================================
   // Draw space object name (GMT-3854)
   // Disable lighting before rendering text
   if (mShowLabels)
   {
      glPushMatrix();
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      
      // Now colors for all other bodies are also saved (LOJ: 2013.11.25)
      *sIntColor = mObjectOrbitColor[objId * maxData + frame];
      
      glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
      DrawStringAt(objName, 0, 0, 0, 1);
      glPopMatrix();
   }
   //============================================================
   
   glPopMatrix();
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("DrawObjectTexture() leaving, objName='%s'\n", objName.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
//  void DrawObject(const wxString &objName, int obj)
//------------------------------------------------------------------------------
/**
 * Draws object sphere and maps texture image.
 *
 * @param  objName  Name of the object
 * @param  obj      Index of the object for mObjectArray
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawObject(const wxString &objName, int obj)
{
   int objId = GetObjectId(objName);
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawObject() drawing:%s, obj=%d, objId:%d, mLastIndex:%d\n",
       objName.WX_TO_C_STRING, obj, objId, mLastIndex);
   MessageInterface::ShowMessage
      ("   mTextureIdMap[%s]=%d\n", objName.WX_TO_C_STRING, mTextureIdMap[objName]);
   #endif
   
   // Rotate body before drawing texture
   if (mObjectArray[obj]->IsOfType(Gmat::CELESTIAL_BODY))
      RotateBodyUsingAttitude(objName, objId);
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   After rotate body, mDrawAxes=%d, objId=%d, mOriginId=%d, mCanRotateAxes=%d\n",
       mDrawAxes, objId, mOriginId, mCanRotateAxes);
   #endif
   
   // Rotate to line up wrt the texture map
   glRotatef(90.0, 0.0, 0.0, 1.0);
   
   //-------------------------------------------------------
   // draw object with texture on option
   //-------------------------------------------------------
   if (mTextureIdMap[objName] != UNINIT_TEXTURE)
   {
      //glColor4f(1.0, 1.0, 1.0, 1.0);
      glColor3f(1.0, 1.0, 1.0);
      
      // LOJ: 2012.02.23 (Fix for Bug 2465)
      // Because the camera is moving and we are using gluLookAt(),
      // we don't need to rotate the world system here, so commented out.
      //glMultMatrixd(mCoordMatrix.GetDataVector());
      
      glBindTexture(GL_TEXTURE_2D, mTextureIdMap[objName]);
      glEnable(GL_TEXTURE_2D);
      
      if (objName == "Sun")
      {
         glDisable(GL_LIGHTING);
         // Why passing mPolygonMode doesn't work?
         //DrawSphere(mObjectRadius[objId], 50, 50, mPolygonMode, GLU_INSIDE);     
         if (mDrawWireFrame)
            DrawSphere(mObjectRadius[objId], 50, 50, GLU_LINE, GLU_INSIDE);
         else
            DrawSphere(mObjectRadius[objId], 50, 50, GLU_FILL, GLU_INSIDE);
         glEnable(GL_LIGHTING);
      }
      else
      {
         // Why passing mPolygonMode doesn't work?
         //DrawSphere(mObjectRadius[objId], 50, 50, mPolygonMode);     
         if (mDrawWireFrame)
            DrawSphere(mObjectRadius[objId], 50, 50, GLU_LINE);
         else
            DrawSphere(mObjectRadius[objId], 50, 50, GLU_FILL);
      }
      
      glDisable(GL_TEXTURE_2D);
      
      //----------------------------------------------------
      // draw grid on option
      //----------------------------------------------------
      if (mDrawGrid && objName == "Earth")
         DrawGridLines(objId);
   }
   else
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage
         ("*** WARNING *** OrbitViewCanvas::DrawObject() %s texture not found.\n",
          objName.WX_TO_C_STRING);
      #endif
      
      // Just draw a wireframe sphere if we get here
      glColor3f(0.20f, 0.20f, 0.50f);
      DrawSphere(mObjectRadius[objId], 50, 50, GLU_LINE);      
      glDisable(GL_TEXTURE_2D);
   }
   
   if (mEnableLightSource)
      glDisable(GL_LIGHTING);
   
} // end DrawObject(const wxString &objName)


//------------------------------------------------------------------------------
// void DrawOrbitLines(int i, const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawOrbitLines(int i, const wxString &objName, int obj,
                                     int objId)
{
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("\nOrbitViewCanvas::DrawOrbitLines() entered, objName='%s'\n   i=%2d, obj=%d, "
       "objId=%d, mTime[%3d]=%f, mTime[%3d]=%f, mIsDrawing[%3d]=%d, mIsDrawing[%3d]=%d\n",
       objName.WX_TO_C_STRING, i, obj, objId, i, mTime[i], i-1, mTime[i-1], i, mIsDrawing[i], i-1,
       mIsDrawing[i-1]);
   #endif
   
   // If current or previous points are not drawing, just return
   if (!mIsDrawing[i] || !mIsDrawing[i-1])
   {
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("DrawOrbitLines() leaving, =====> object is not drawing\n");
      #endif
      return;
   }
   
   int index1 = 0, index2 = 0;
   bool drawLine = false;
   
   // Check if line can be be drawn
   if (mIgnoreTimeSequence)
   {
      drawLine = true;
   }
   else
   {
      // Draw object orbit line based on time
      if ((mTime[i] > mTime[i-1]) ||
          ((i>2) && (mTime[i] <= mTime[i-1]) && (mTime[i-1] <= mTime[i-2]))) //for backprop
      {
         drawLine = true;
      }
   }
   
   // Draw object orbit line
   if (drawLine)
   {
      index1 = objId * maxData * 3 + (i-1) * 3;
      index2 = objId * maxData * 3 + i * 3;
      
      Rvector3 r1(mObjectViewPos[index1+0], mObjectViewPos[index1+1],
                  mObjectViewPos[index1+2]);
      
      Rvector3 r2(mObjectViewPos[index2+0], mObjectViewPos[index2+1],
                  mObjectViewPos[index2+2]);
      
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("   r1 = %s\n   r2 = %s\n", r1.ToString().c_str(), r2.ToString().c_str());
      #endif
      
      // if object position magnitude is 0, skip
      if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
      {
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage("DrawOrbitLines() leaving, ===> position is zero\n");
         #endif

         // Update the frame index so the attitude will update
         mObjLastFrame[objId] = i;
         return;
      }
      
      // if object position diff is over limit, skip (ScriptEx_TargetHohmann)
      #ifdef SKIP_DATA_OVER_LIMIT
      static Real sMaxDiffDist = 100000000.0;
      // if difference is more than sMaxDiffDist skip
      if ((Abs(r2[0]- r1[0]) > sMaxDiffDist && (SignOf(r2[0]) != SignOf(r1[0]))) ||
          (Abs(r2[1]- r1[1]) > sMaxDiffDist && (SignOf(r2[1]) != SignOf(r1[1]))) ||
          (Abs(r2[2]- r1[2]) > sMaxDiffDist && (SignOf(r2[2]) != SignOf(r1[2]))))
      {
         #if DEBUG_SHOW_SKIP
         MessageInterface::ShowMessage
            ("   plot=%s, i1=%d, i2=%d, time1=%f, time2=%f\n   r1=%s   r2=%s\n",
             GetName().c_str(), i-1, i, mTime[i-1], mTime[i], r1.ToString().c_str(),
             r2.ToString().c_str());
         #endif
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage
            ("DrawOrbitLines() leaving, ===> position difference is over the limit\n");
         #endif
         return;
      }
      #endif
      
      // If drawing orbit lines
      int colorIndex = objId * maxData + i;
      if (mDrawOrbitFlag[colorIndex])
      {
         // Now colors for all drawing objects are saved (LOJ: 2014.01.24)
         *sIntColor = mObjectOrbitColor[colorIndex];
         
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage
            ("   colorIndex=%4d, sIntColor=%06X, sGlColor=%06X\n", colorIndex,
             *sIntColor, sGlColor);
         #endif
         
         // PS - Rendering.cpp
         DrawLine(sGlColor, r1, r2);
      }
      
      // save last valid frame to show object at final frame
      mObjLastFrame[objId] = i;
      
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage
         ("DrawOrbitLines() leaving, mObjLastFrame[%d] = %d\n", objId, i);
      #endif
   }
   else
   {
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("DrawOrbitLines() leaving, ===> time is not in order\n");
      #endif
   }
}


//------------------------------------------------------------------------------
//  void DrawXYPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws XY plane circles.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawXYPlane(UnsignedInt color)
{
   glPushMatrix();
   
   DrawCircles(color);
   
   glPopMatrix();
   
} // end DrawXYPlane()


//------------------------------------------------------------------------------
//  void DrawEclipticPlane(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws ecliptic plane circles.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawEclipticPlane(UnsignedInt color)
{
   glPushMatrix();
   
   // Get view to ecliptic frame rotation matrix
   int coordIndex = mLastIndex*16;
   mCoordMatrix = Rmatrix(4,4);
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         mCoordMatrix.SetElement(i, j, mRotMatViewToEcliptic[coordIndex + i*4 + j]);
   
   // Multiply rotation matrix to current matrix
   glMultMatrixd(mCoordMatrix.GetDataVector());
   
   // Then draw circles
   DrawCircles(color);
   
   glPopMatrix();
} // end DrawEclipticPlane()


//------------------------------------------------------------------------------
// void DrawCircles(UnsignedInt color)
//------------------------------------------------------------------------------
/**
 * Draws circles.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawCircles(UnsignedInt color)
{
   int i;
   float endPos[3];
   float distance;
   Real angle;
   
   glDisable(GL_LIGHTING);
   glDisable(GL_LINE_SMOOTH);
   glLineWidth(1.0f);
   
   distance = (mCamera.position - mCamera.view_center).GetMagnitude();
   
   // set color
   *sIntColor = color;
   Rvector3 start, end;
   start.Set(0, 0, 0);
   
   //-----------------------------------
   // draw lines
   //-----------------------------------
   for (i=7; i<368; i+=15)
   {
      angle = GmatMathConstants::RAD_PER_DEG * ((Real)i);

      endPos[0] = distance * cos(angle);
      endPos[1] = distance * sin(angle);
      endPos[2] = 0.0;

      end.Set(endPos[0], endPos[1], endPos[2]);
      // PS - See Rendering.cpp
      DrawLine(sGlColor, start, end); 
   }
   
   //-----------------------------------
   // draw circles
   //-----------------------------------
   glPushMatrix();
   
   GLUquadricObj *qobj = gluNewQuadric();

   //==============================================================
   // Argosy code
   //==============================================================
   Real orthoDepth = distance;
   
   Real ort = orthoDepth * 8;
   Real pwr = Floor(Log10(ort));
   Real size = Exp10(pwr)/100;
   Real imax = orthoDepth/size;
   
   //------------------------------------------
   // Draw MAJOR circles
   //------------------------------------------
   //SetCelestialColor (GlOptions.EquatorialPlaneColor);
   for (int i=1; i<=(int)imax; ++i)
      if (i%10==0)
         DrawCircle(qobj, i*size);
   
   //------------------------------------------
   // Draw MINOR circles
   //------------------------------------------
   imax = GmatMathUtil::Min(imax, 100);
   Real factor = (size*100)/(ort);
   
   GLubyte ubfactor = (GLubyte)(factor * 255);
   //MessageInterface::ShowMessage("===> ubfactor=%d, factor=%f\n", ubfactor, factor);
   
   // Why does alpha value have no effects?
   glColor4ub(sGlColor->red, sGlColor->green, sGlColor->blue, ubfactor);
   
   for (int i=1; i<=(int)imax; ++i)
      if (i%10 == 0 || (factor > 0.5))
         DrawCircle(qobj, i*size);
   
   gluDeleteQuadric(qobj);
   sGlColor->alpha = 255;
   
   glPopMatrix();
   glLineWidth(1.0f);
   glEnable(GL_LINE_SMOOTH);
}


//------------------------------------------------------------------------------
//  void DrawSunLine()
//------------------------------------------------------------------------------
/**
 * Draws Origin to Sun lines.
 */
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawSunLine()
{
   //int frame = mNumData - 1;
   int frame = mLastIndex;
   if (frame <= 0)
      return;
   
   int sunId = GetObjectId("Sun");
   
   // check for Sun
   if (sunId == UNKNOWN_OBJ_ID)
      return;
   
   Rvector3 originPos, sunPos;
   Real distance = (Real)mCurrViewDistance;
   Real mag;
   
   //--------------------------------
   // draw sun line
   //--------------------------------
   
   // set color
   *sIntColor = mSunLineColor;
   int index = 0;
   
   // draw one line from origin to Sun
   // Dunn took out old minus signs to make attitude correct.
   index = mOriginId * maxData * 3 + frame * 3;
   originPos.Set(mObjectViewPos[index+0], 
                 mObjectViewPos[index+1], 
                 mObjectViewPos[index+2]);
   
   index = sunId * maxData * 3 + frame * 3;
   sunPos.Set(mObjectViewPos[index+0], 
              mObjectViewPos[index+1], 
              mObjectViewPos[index+2]);
   
   // show lines between Sun and Earth and to -Sun
   // Dunn set it so sunline is only from origin out from earth in direction of
   // sun.
   // PS - See Rendering.cpp
   DrawLine(sGlColor, originPos, sunPos);
   //DrawLine(sGlColor, originPos, -sunPos);
   
   // Show Sun direction text
   glColor3f(1.0, 1.0, 0.0); // yellow
   
   // get sun unit vector and multiply by distance
   // Dunn changed the division factor from 2.2 to 2.0
   mag = sqrt(sunPos[0]*sunPos[0] + sunPos[1]*sunPos[1] + sunPos[2]*sunPos[2]);
   DrawStringAt(" +S", sunPos[0]/mag*distance/2.0,
                       sunPos[1]/mag*distance/2.0,
                       sunPos[2]/mag*distance/2.0,
                1.0);
   
} // end DrawSunLine()

//---------------------------------------------------------------------------
// void DrawAxes()
//---------------------------------------------------------------------------
void OrbitViewCanvas::DrawAxes()
{
   glPushMatrix();
   
   // Rotate axis before drawing
   if (mCanRotateAxes)
      RotateUsingOriginAttitude();
   
   float distance = (mCamera.position - mCamera.view_center).GetMagnitude();
   //GLfloat viewDist = mCurrViewDistance;
   GLfloat viewDist = mCurrViewDistance + distance * 0.3;
   Rvector3 xAxis(viewDist, 0.0, 0.0);
   Rvector3 yAxis(0.0, viewDist, 0.0);
   Rvector3 zAxis(0.0, 0.0, viewDist);
   
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   wxString axisLabel;
   
   //glLineWidth(2.0f);
   glLineWidth(1.0f);
   
   //-----------------------------------
   // draw axes
   //-----------------------------------
   
   Rvector3 origin;
   origin.Set(0, 0, 0);
   
   // PS - See Rendering.cpp
   DrawLine(1, 0, 0, origin, xAxis);
   DrawLine(0, 1, 0, origin, yAxis);
   DrawLine(0, 0, 1, origin, zAxis);
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------
   // Dunn took out old minus signs to get axis labels at the correct end of
   // each axis and thus make attitude correct.
   glColor3f(1, 0, 0);     // red
   axisLabel = "+X ";
   DrawStringAt(axisLabel, xAxis);
   
   glColor3f(0, 1, 0);     // green
   axisLabel = "+Y ";
   DrawStringAt(axisLabel, yAxis);
   
   glColor3f(0, 0, 1);     // blue
   axisLabel = "+Z ";
   DrawStringAt(axisLabel, zAxis);
   
   glLineWidth(1.0);
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   glPopMatrix();
}


//------------------------------------------------------------------------------
// void DrawGridLines(int objId)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawGridLines(int objId)
{
   // This makes lines thicker
   //glEnable(GL_LINE_SMOOTH);
   //glLineWidth(1.5);
   
   // Just draw a wireframe sphere little bigger to show grid
   //glColor3f(0.20, 0.20, 0.50); // dark blue
   glColor3f(0.0, 0.0, 0.0);      // black
   //glColor3f(0.50, 0.10, 0.20); // maroon
   GLdouble radius = mObjectRadius[objId] + mObjectRadius[objId] * 0.03;
   DrawSphere(radius, 36, 18, GLU_LINE, GLU_OUTSIDE, GL_NONE, GL_FALSE);
}


//------------------------------------------------------------------------------
// void DrawSpacecraft3dModel(Spacecraft *sc, int objId, int frame)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawSpacecraft3dModel(Spacecraft *sc, int objId, int frame)
{
   #ifdef DEBUG_DRAW_SPACECRAFT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawSpacecraft3dModel() entered, sc=<%p>'%s', objId=%d, "
       "frame=%d\n", sc, sc ? sc->GetName().c_str() : "NULL", objId, frame);
   #endif
   
   ModelManager *mm = ModelManager::Instance();
   //ModelObject *scModel = mm->GetModel(sc->modelID);
   ModelObject *scModel = mm->GetModel(sc->GetModelId());
   
   float RTD = (float)GmatMathConstants::DEG_PER_RAD;
   
   int index1 = objId * maxData * 3 + frame * 3;
   int attIndex = objId * maxData * 4 + mObjLastFrame[objId] * 4;
   
   Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
                          mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);

   #ifdef DEBUG_ATTITUDE_DISPLAY
      MessageInterface::ShowMessage("Quat[%d - %d]: [%lf  %lf  %lf  %lf]\n", 
         attIndex, attIndex+3, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
         mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   #endif

   Rvector3 EARad = AttitudeConversionUtility::ToEulerAngles(quat, 1,2,3);
   
   #ifdef DEBUG_SC_ATTITUDE
   MessageInterface::ShowMessage
      ("DrawSpacecraft3dModel(), '%s', model=<%p>, modelId=%d, EARad=%s",
       sc->GetName().c_str(), model, sc->GetModelId(),  EARad.ToString().c_str());
   #endif
   
   float EAng1Deg = float(EARad(0)) * RTD;
   float EAng2Deg = float(EARad(1)) * RTD;
   float EAng3Deg = float(EARad(2)) * RTD;

   // Get offset rotation and scale from Spacecraft
   float     offset[3];
   float     rotation[3];
   float     scale;
   offset[0]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetX"));
   offset[1]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetY"));
   offset[2]   = sc->GetRealParameter(sc->GetParameterID("ModelOffsetZ"));
   rotation[0] = sc->GetRealParameter(sc->GetParameterID("ModelRotationX"));
   rotation[1] = sc->GetRealParameter(sc->GetParameterID("ModelRotationY"));
   rotation[2] = sc->GetRealParameter(sc->GetParameterID("ModelRotationZ"));
   scale = sc->GetRealParameter(sc->GetParameterID("ModelScale"));
   scModel->SetBodyPosition(offset[0], offset[1], offset[2]);
   scModel->SetRotation(true, rotation[0], rotation[1], rotation[2]);
   scModel->SetAttitude(true, 0, 0, 0);
   scModel->SetScale(scale);
   
   #ifdef DEBUG_ATTITUDE_DISPLAY
      MessageInterface::ShowMessage("Model angles: [%lf  %lf  %lf]\n", 
         EAng1Deg, EAng2Deg, EAng3Deg);
   #endif

   // Dunn's new attitude call.  Need to change to quaternions.  Also need
   // to concatenate with BaseRotation.  Also need this to work for replay
   // animation buttons.
   scModel->SetAttitude(true, EAng1Deg, EAng2Deg, EAng3Deg);
   
   // The line above is where the object model gets its orientation.  This
   // also seems to be a good place to give the model its ECI position.
   // That call is actually in ModelObject.cpp on line 682.
   
   // Draw model
   glTranslatef(mObjectViewPos[index1+0],
                mObjectViewPos[index1+1],
                mObjectViewPos[index1+2]);
   
   //-----------------------------------------------------------------
   // glMultMatrixd(const GLdouble *m)
   // Multiplies the current matrix with the one specified using m, and
   // replaces the current matrix with the product.
   // m Points to 16 consecutive values that are used as the elements of a
   // 4x4 column-major matrix
   //-----------------------------------------------------------------

   // Get view to internal coordinate frame rotation matrix
   int coordIndex = mLastIndex*16;
   mCoordMatrix = Rmatrix(4,4);
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         mCoordMatrix.SetElement(i, j, mRotMatViewToInternal[coordIndex + i*4 + j]);

   // Multiply rotation matrix to current matrix
   glMultMatrixd(mCoordMatrix.GetDataVector());

   // Set isLit to true
   scModel->DrawAsSpacecraft(true);
      
   // Old code that may be worth saving
   // SetCurrent(*theContext);
   // ModelManager *mm = ModelManager::Instance();
   // SetCurrent(*mm->modelContext);
   // mm->modelContext->SetCurrent(*this);
   
   // Maybe delete this
   // Rvector3  eulersRad   = scAttitude->GetEulerAngles(mTime[frame],1,2,3);  // Uses the 123 Euler Sequence
   
   // Save line below for when Phil modifies scModel->Rotate to accept quats
   // Rvector   quaternion  = scAttitude->GetQuaternion(mTime[frame]);
   
   // Save line below for when I need to transpose a matrix.
   // Rmatrix33 TMat        = AMat.Transpose();
   
   // Neither of the calls below works.  The model is being positioned somewhere else!
   // PosX = 0.0;
   // PosY = 0.0;
   // PosZ = 15000.0;
   // scModel->Reposition(PosX,PosY,PosZ);
   // scModel->TranslateW(PosX,PosY,PosZ);
   
   #ifdef DEBUG_DRAW_SPACECRAFT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawSpacecraft3dModel() leaving, sc=<%p>'%s', objId=%d, "
       "frame=%d\n", sc, sc ? sc->GetName().c_str() : "NULL", objId, frame);
   #endif
}


//------------------------------------------------------------------------------
// void DrawCelestialBody3dModel(CelestialBody *body, int objId, int frame)
//------------------------------------------------------------------------------
void OrbitViewCanvas::DrawCelestialBody3dModel(CelestialBody *body, const wxString &objName,
                                               int objId, int frame)
{
   #ifdef DEBUG_DRAW_3D_BODY
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawCelestialBody3dModel() entered, body=<%p>'%s', objName='%s', "
       "objId=%d, frame=%d\n", body, body ? body->GetName().c_str() : "NULL", objName.WX_TO_C_STRING,
       objId, frame);
   #endif
   
   // Rotate body before drawing model
   RotateBodyUsingAttitude(objName, objId);
   
   ModelManager *mm = ModelManager::Instance();
   ModelObject *bodyModel = mm->GetModel(body->Get3dViewModelId());

   // Comment out attitude for now. Add it later if needed (LOJ: 2015.01.30)
   float RTD = (float)GmatMathConstants::DEG_PER_RAD;
   
   int index1 = objId * maxData * 3 + frame * 3;
   // int attIndex = objId * maxData * 4 + mObjLastFrame[objId] * 4;
   
   // Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
   //                        mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   
   // #ifdef DEBUG_ATTITUDE_DISPLAY
   // MessageInterface::ShowMessage
   //    ("Quat[%d - %d]: [%lf  %lf  %lf  %lf]\n", 
   //     attIndex, attIndex+3, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
   //     mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   // #endif
   
   // Rvector3 EARad = AttitudeConversionUtility::ToEulerAngles(quat, 1,2,3);
   Rvector3 EARad;
   
   #ifdef DEBUG_BODY_ATTITUDE
   MessageInterface::ShowMessage
      ("DrawCelestialBody3dModel(), '%s', model=<%p>, 3DModelId=%d, EARad=%s",
       body->GetName().c_str(), model, body->GetModelId(),  EARad.ToString().c_str());
   #endif
   
   float EAng1Deg = float(EARad(0)) * RTD;
   float EAng2Deg = float(EARad(1)) * RTD;
   float EAng3Deg = float(EARad(2)) * RTD;
   
   // Get offset rotation and scale from CelestialBody.
   float     offset[3];
   float     rotation[3];
   float     scale = 1.0;
   for (int i = 0; i < 3; i++)
   {
      offset[i] = 0.0;
      rotation[i] = 0.0;
   }
   
   offset[0]   = body->GetRealParameter(body->GetParameterID("3DModelOffsetX"));
   offset[1]   = body->GetRealParameter(body->GetParameterID("3DModelOffsetY"));
   offset[2]   = body->GetRealParameter(body->GetParameterID("3DModelOffsetZ"));
   rotation[0] = body->GetRealParameter(body->GetParameterID("3DModelRotationX"));
   rotation[1] = body->GetRealParameter(body->GetParameterID("3DModelRotationY"));
   rotation[2] = body->GetRealParameter(body->GetParameterID("3DModelRotationZ"));
   scale = body->GetRealParameter(body->GetParameterID("3DModelScale"));
   bodyModel->SetBodyPosition(offset[0], offset[1], offset[2]);
   bodyModel->SetRotation(true, rotation[0], rotation[1], rotation[2]);
   bodyModel->SetAttitude(true, 0, 0, 0);
   bodyModel->SetScale(scale);
   
   #ifdef DEBUG_BODY_ATTITUDE
   MessageInterface::ShowMessage
      ("Model angles: [%lf  %lf  %lf]\n", EAng1Deg, EAng2Deg, EAng3Deg);
   #endif

   // Dunn's new attitude call.  Need to change to quaternions.  Also need
   // to concatenate with BaseRotation.  Also need this to work for replay
   // animation buttons.
   bodyModel->SetAttitude(true, EAng1Deg, EAng2Deg, EAng3Deg);
   
   // Set isLit to true
   bodyModel->DrawAsCelestialBody(true);
   
   #ifdef DEBUG_DRAW_3D_BODY
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::DrawCelestialBody3dModel() leaving, body=<%p>'%s', objId=%d, "
       "frame=%d\n", body, body ? body->GetName().c_str() : "NULL", objId, frame);
   #endif
}


//---------------------------------------------------------------------------
// void DrawStars()
//---------------------------------------------------------------------------
void OrbitViewCanvas::DrawStars()
{
   #if DEBUG_DRAW_STARS
   MessageInterface::ShowMessage("OrbitViewCanvas::DrawStars() entered\n");
   #endif
   
   // drawing the stars at infinity requires them to have their own projection
   glMatrixMode(GL_PROJECTION); 
   glLoadIdentity();
   GLfloat aspect = (GLfloat)mCanvasSize.x / (GLfloat)mCanvasSize.y;
   glMatrixMode(GL_MODELVIEW);
   gluPerspective(mCamera.fovDeg, aspect, 0.1f, 50000000.0f);
   // The stars also need to be drawn in their own world view to be drawn at infinity
   Rvector3 starPosition = mCamera.position;
   Rvector3 starCenter = mCamera.view_center - starPosition;
   Rvector3 starUp = mCamera.up;
   
   #if DEBUG_DRAW_STARS
   MessageInterface::ShowMessage
      ("   starCenter   = %s   starPosition = %s", starCenter.ToString().c_str(),
       starPosition.ToString().c_str());
   #endif
   
   // if star position is not zero vector then normalize (bug 2367 fix)
   if (!starPosition.IsZeroVector())
      starPosition.Normalize();
   starCenter += starPosition;
   
   gluLookAt(starPosition[0], starPosition[1], starPosition[2],
             starCenter[0], starCenter[1], starCenter[2],
             starUp[0], starUp[1], starUp[2]);
   
   //-----------------------------------------------------------------
   // glMultMatrixd(const GLdouble *m)
   // Multiplies the current matrix with the one specified using m, and
   // replaces the current matrix with the product.
   // m Points to 16 consecutive values that are used as the elements of a        
   // 4x4 column-major matrix
   //-----------------------------------------------------------------
   
   // Get view to internal coordinate frame rotation matrix
   int coordIndex = mLastIndex*16;
   mCoordMatrix = Rmatrix(4,4);
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         mCoordMatrix.SetElement(i, j, mRotMatViewToInternal[coordIndex + i*4 + j]);
   
   // Multiply rotation matrix to current matrix
   glMultMatrixd(mCoordMatrix.GetDataVector());
   
   #if DEBUG_COORD_MATRIX
   MessageInterface::ShowMessage
      ("\n===============> mLastIndex = %d, mCoordMatrix = \n%s\n", mLastIndex,
       mCoordMatrix.ToString(12).c_str());
   #endif
   
   // draw the stars
   mStars->DrawStarsVA(1.0f, mStarCount, mDrawConstellations);
   
   #if DEBUG_DRAW_STARS
   MessageInterface::ShowMessage("OrbitViewCanvas::DrawStars() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void RotateBodyUsingAttitude(const wxString &objName, int objId)
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateBodyUsingAttitude(const wxString &objName, int objId)
{
   #if DEBUG_ROTATE_BODY > 0
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s' entered, objId=%d, mLastIndex=%d, time=%f\n",
       objName.WX_TO_C_STRING, objId, mLastIndex, mTime[mLastIndex]);
   #endif
   
   if (mTime[mLastIndex] == 0.0)
      return;
   
   Real angInDeg = 0.0;
   Rvector3 eAxis;
   
   // Use saved rotation angle and axis
   GetBodyRotationData(objId, angInDeg, eAxis);
   #if DEBUG_ROTATE_BODY > 0
      MessageInterface::ShowMessage("Rotate %s %lf deg about axis [%lf %lf %lf]\n", 
         objName.WX_TO_C_STRING, angInDeg, eAxis[0], eAxis[1], eAxis[2]);
   #endif
   // Now rotate
   if (angInDeg != 0.0)
      glRotated(angInDeg, eAxis[0], eAxis[1], eAxis[2]);
   
   #if DEBUG_ROTATE_BODY > 0
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s' leaving, objId=%d, mLastIndex=%d, time=%f, "
       "angInDeg=%f, eAxis=%s\n", objName.WX_TO_C_STRING, objId, mLastIndex, mTime[mLastIndex],
       angInDeg, eAxis.ToString(12).c_str());
   #endif
}


//---------------------------------------------------------------------------
// void RotateUsingOriginAttitude()
//---------------------------------------------------------------------------
void OrbitViewCanvas::RotateUsingOriginAttitude()
{
   #if DEBUG_ROTATE_WORLD > 1
   MessageInterface::ShowMessage
      ("RotateUsingOriginAttitude() '%s' entered, mLastIndex=%d, time=%f\n",
       mOriginName.c_str(), mLastIndex, mTime[mLastIndex]);
   #endif
   
   if (mTime[mLastIndex] == 0.0)
      return;
   
   Real angInDeg = 0.0;
   Rvector3 eAxis;
   
   // Use saved rotation angle and axis
   GetBodyRotationData(mOriginId, angInDeg, eAxis);
   
   // Now rotate
   if (angInDeg != 0.0)
      glRotated(angInDeg, eAxis[0], eAxis[1], eAxis[2]);
   
   #if DEBUG_ROTATE_WORLD > 0
   MessageInterface::ShowMessage
      ("RotateUsingOriginAttitude() '%s' leaving, mOriginId=%d, mLastIndex=%d, time=%f, "
       "angInDeg=%f, eAxis=%s\n", mOriginName.c_str(), mOriginId, mLastIndex, mTime[mLastIndex],
       angInDeg, eAxis.ToString(12).c_str());
   #endif
}


//---------------------------------------------------------------------------
// void UpdateRotateFlags()
//---------------------------------------------------------------------------
/**
 * Updates flag for rotating axes when body rotates.
 */
//---------------------------------------------------------------------------
void OrbitViewCanvas::UpdateRotateFlags()
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateRotateFlags() entered, pViewCoordSystem=<%p>'%s'\n",
       pViewCoordSystem, pViewCoordSystem->GetName().c_str());
   #endif
   
   AxisSystem *axis =
      (AxisSystem*)pViewCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "");
   
   if (axis->IsOfType("BodyFixedAxes") &&
       (mOriginName.IsSameAs(axis->GetStringParameter("Origin").c_str())))
   {
      mCanRotateAxes = true;
   }
   else
   {
      mCanRotateAxes = false;
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::UpdateRotateFlags() mCanRotateAxes=%d\n", mCanRotateAxes);
   #endif
}


//---------------------------------------------------------------------------
// bool ConvertObjectData()
//---------------------------------------------------------------------------
bool OrbitViewCanvas::ConvertObjectData()
{
   if (pInternalCoordSystem == NULL || pViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_CONVERT
   MessageInterface::ShowMessage
      ("OrbitViewCanvas::ConvertObjectData() internalCS=%s, viewCSName=%s, viewCS=%d\n",
       pInternalCoordSystem->GetName().c_str(), pViewCoordSystem->GetName().c_str(),
       pViewCoordSystem);
   #endif
   
   // do not convert if view CS is internal CS
   if (mViewCsIsInternalCs)
   {
      #if DEBUG_CONVERT
      MessageInterface::ShowMessage
         ("OrbitViewCanvas::ConvertObjectData() No conversion is needed. "
          "Just copy MJ2000 pos\n");
      #endif
      for (int obj=0; obj<mObjectCount; obj++)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         int index = 0;
         
         // Draw first part from the ring buffer
         for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
         {
            index = objId * maxData * 3 + i * 3;
            CopyVector3(&mObjectViewPos[index], &mObjectGciPos[index]);
         }
         
         // Draw second part from the ring buffer
         if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
         {
            for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
            {
               index = objId * maxData * 3 + i * 3;
               CopyVector3(&mObjectViewPos[index], &mObjectGciPos[index]);
            }
         }
      }
   }
   else
   {
      for (int obj=0; obj<mObjectCount; obj++)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         
         #if DEBUG_CONVERT
         MessageInterface::ShowMessage
            ("OrbitViewCanvas::ConvertObjectData() mObjectNames[%d]='%s'\n", objId,
             mObjectNames[obj].c_str());
         #endif         
         
         // Draw first part from the ring buffer
         for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
            ConvertObject(objId, i);
         
         // Draw second part from the ring buffer
         if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
         {
            for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
               ConvertObject(objId, i);
         }
      }
   }
   
   return true;
} //ConvertObjectData()


//---------------------------------------------------------------------------
// void ConvertObject(int objId, int index)
//---------------------------------------------------------------------------
void OrbitViewCanvas::ConvertObject(int objId, int index)
{
   Rvector6 inState, outState;
   int start = objId*maxData*3+index*3;
   inState.Set(mObjectGciPos[start+0], mObjectGciPos[start+1],
               mObjectGciPos[start+2], 0.0, 0.0, 0.0);
   
   mCoordConverter.Convert(mTime[index], inState, pInternalCoordSystem,
                           outState, pViewCoordSystem);
   
   mObjectViewPos[index+0] = outState[0];
   mObjectViewPos[index+1] = outState[1];
   mObjectViewPos[index+2] = outState[2];
   
   #if DEBUG_CONVERT
   if (index < 10)
   {
      MessageInterface::ShowMessage
         ("   index=%d, inState=%16.9f, %16.9f, %16.9f\n"
          "           outState=%16.9f, %16.9f, %16.9f\n", index, inState[0],
          inState[1], inState[2], outState[0], outState[1], outState[2]);
   }
   #endif
}


