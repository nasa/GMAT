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
// Developed jointly by NASA/GSFC, Thinking Systems, Inc
//
// Author:   Linda Jun
// Created:  2011/06/06
//
/**
 * Implements GroundTrackCanvas for drawing ground track plot using OpenGL.
 */
//------------------------------------------------------------------------------

#include "GroundTrackCanvas.hpp"
#include "Camera.hpp"
#include "ModelObject.hpp"
#include "ModelManager.hpp"
#include "GmatMainFrame.hpp"       // for EnableAnimation()
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "MessageInterface.hpp"
#include "Rendering.hpp"           // for DrawStringAt(), DrawSquare()
#include "MdiChildGroundTrackFrame.hpp"
#include "AttitudeConversionUtility.hpp"
#include "BodyFixedPoint.hpp"

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
//#define SKIP_DATA_OVER_LIMIT

// Currently local plots are deleted when function run completes (2015.07.09)
// So this macro has no effects. But if plot is global we need to check if 
// all drawing objects are global as well since plot access object pointers.
//#define DISABLE_REPAINT_IN_FUNCTION

// debug
//#define DEBUG_INIT 1
//#define DEBUG_UPDATE 1
//#define DEBUG_ACTION 1
//#define DEBUG_DRAW 1
//#define DEBUG_DRAW_IMAGE 1
//#define DEBUG_DRAW_LINE 2
//#define DEBUG_OBJECT 2
//#define DEBUG_PROJECTION 3
//#define DEBUG_CS 1
//#define DEBUG_ANIMATION 1
//#define DEBUG_LONGITUDE 1
//#define DEBUG_SHOW_SKIP 1
//#define DEBUG_ROTATE_BODY 2
//#define DEBUG_DATA_BUFFERRING
//#define DEBUG_ORBIT_LINES 1
//#define DEBUG_DRAW_DEBUG 1
//#define DEBUG_DRAW_SPACECRAFT 1
//#define DEBUG_ONPAINT 2

#define MODE_CENTERED_VIEW 0
#define MODE_FREE_FLYING 1
#define MODE_ASTRONAUT_6DOF 2

BEGIN_EVENT_TABLE(GroundTrackCanvas, ViewCanvas)
   EVT_SIZE(GroundTrackCanvas::OnSize)
   EVT_PAINT(GroundTrackCanvas::OnPaint)
   EVT_MOUSE_EVENTS(GroundTrackCanvas::OnMouse)
   EVT_KEY_DOWN(GroundTrackCanvas::OnKeyDown)
END_EVENT_TABLE()

using namespace GmatPlot;
using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const int GroundTrackCanvas::LAST_STD_BODY_ID = 10;
const int GroundTrackCanvas::MAX_COORD_SYS = 10;
const Real GroundTrackCanvas::MAX_ZOOM_IN = 3700.0;
const Real GroundTrackCanvas::RADIUS_ZOOM_RATIO = 2.2;
const Real GroundTrackCanvas::DEFAULT_DIST = 30000.0;

// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// GroundTrackCanvas(wxWindow *parent, wxWindowID id, ...)
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
GroundTrackCanvas::GroundTrackCanvas(wxWindow *parent, wxWindowID id,
                                     const wxPoint& pos, const wxSize& size,
                                     const wxString& name, long style)
   : ViewCanvas(parent, id, pos, size, name, style)
{
   // velocity is needed for computing direction of the ground track
   mNeedVelocity = true;
   
   // Linux specific
   #ifdef __WXGTK__
      hasBeenPainted = false;
   #endif
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("GroundTrackCanvas() name=%s, size.X=%d, size.Y=%d\n",
       name.c_str(), size.GetWidth(), size.GetHeight());
   #endif
   
   
   //@todo - Remove 3d related stuff
   
   mCamera.Reset();
   mCamera.Relocate(DEFAULT_DIST, 0.0, 0.0, 0.0, 0.0, 0.0);
   
   // initialize data members   
   mGlList = 0;
   mIsFirstRun = true;
   
   ResetPlotInfo();
   
   // projection
   mControlMode = MODE_CENTERED_VIEW;
   mInversion = 1;
   
   // viewpoint
   SetDefaultViewPoint();
   
   mOriginName = "";
   mOriginId = 0;
   
   mRotateXy = true;
   mZoomAmount = 300.0;
   
   mFootPrintOption = 0; // 0 = Draw NONE, 1 = Draw ALL
   mFootPrintDrawFrequency = 10; // every 100th plot data
   
   // write warning flags
   mWriteCentralBodyWarning = true;
   
   // Zoom
   mMaxZoomIn = MAX_ZOOM_IN;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("   pInternalCoordSystem=%p, pViewCoordSystem=%p\n", pInternalCoordSystem,
       pViewCoordSystem);
   if (pInternalCoordSystem)
      MessageInterface::ShowMessage
         ("   pInternalCoordSystem=%s\n", pInternalCoordSystem->GetName().c_str());
   if (pViewCoordSystem)
      MessageInterface::ShowMessage
         ("   pViewCoordSystem=%s\n", pViewCoordSystem->GetName().c_str());
   MessageInterface::ShowMessage("GroundTrackCanvas() constructor exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~GroundTrackCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GroundTrackCanvas::~GroundTrackCanvas()
{
   #ifdef DEBUG_RESOURCE_CLEARING
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::~GroundTrackCanvas() '%s' entered\n", mPlotName.c_str());
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
      ("GroundTrackCanvas::~GroundTrackCanvas() '%s' exiting\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &centralBodyName,
//         const std::string &textureMap, Integer footPrintOption)
//------------------------------------------------------------------------------
void GroundTrackCanvas::SetGl2dDrawingOption(const std::string &centralBodyName,
                                             const std::string &textureMap,
                                             Integer footPrintOption)
{
   #ifdef DEBUG_DRAWING_OPTION
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::SetGl2dDrawingOption() entered, centralBodyName='%s', "
       "textureMap='%s', footPrintOption=%d\n", centralBodyName.c_str(),
       textureMap.c_str(), footPrintOption);
   #endif
   mCentralBodyName = centralBodyName;
   mCentralBodyTextureFile = textureMap;
   mTextureFileMap[mCentralBodyName.c_str()] = textureMap;
   mFootPrintOption = footPrintOption;
}


//------------------------------------------------------------------------------
// void ClearPlot()
//------------------------------------------------------------------------------
/**
 * Clears plot.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::ClearPlot()
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
void GroundTrackCanvas::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_REDRAW
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::RedrawPlot() entered, viewAnimation=%d\n", viewAnimation);
   #endif
      
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
void GroundTrackCanvas::ShowDefaultView()
{
   SetDefaultView();
   Refresh(false);
}


//------------------------------------------------------------------------------
// void DrawWireFrame(bool flag)
//------------------------------------------------------------------------------
/**
 * Shows objects in wire frame.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawWireFrame(bool flag)
{
   mDrawWireFrame = flag;
   Refresh(false);
}


//------------------------------------------------------------------------------
// void OnDrawGrid(bool flag)
//------------------------------------------------------------------------------
/**
 * Draws axes.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::OnDrawGrid(bool flag)
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
void GroundTrackCanvas::DrawInOtherCoordSystem(const wxString &csName)
{
   #if DEBUG_ACTION
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawInOtherCoordSysName() viewCS=%s, newCS=%s\n",
       mViewCoordSysName.c_str(), csName.c_str());
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
            
      if (!mOriginName.IsSameAs(oldOriginName))
         GotoObject(mOriginName);
      
      ConvertObjectData();
      Refresh(false);
   }
}


//---------------------------------------------------------------------------
// void ViewAnimation(int interval, int frameInc)
//---------------------------------------------------------------------------
void GroundTrackCanvas::ViewAnimation(int interval, int frameInc)
{
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::ViewAnimation() interval=%d, frameInc=%d\n",
       interval, frameInc);
   #endif

   
   #ifdef DISABLE_REPAINT_IN_FUNCTION
   if (mIsEndOfData && mInFunction)
   {
      wxString msg = "*** WARNING *** This plot data was published inside a "
         "function, so repainting or drawing animation is disabled.\n";
      MessageInterface::ShowMessage(msg.c_str());
      return;
   }
   #endif
   
   this->SetFocus(); // so that it can get key interrupt
   mIsAnimationRunning = true;
   mUpdateInterval = interval;
   mFrameInc = frameInc;
   mHasUserInterrupted = false;
   // Since current solver iteration is drawn only during the run,
   // turn off drawing solver data.
   mDrawSolverData = false;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(false, false, true);
   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   DrawFrame();
   
   gmatAppData->GetMainFrame()->EnableMenuAndToolBar(true, false, true);   
   
   mIsAnimationRunning = false;
   
}


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
void GroundTrackCanvas::TakeAction(const std::string &action)
{
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
void GroundTrackCanvas::OnPaint(wxPaintEvent& event)
{
   #ifdef DEBUG_ONPAINT
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::OnPaint() entered, mFatalErrorFound=%s, mGlInitialized=%d, "
       "mObjectCount=%d\n", (mFatalErrorFound? "true" : "false"),
       mGlInitialized, mObjectCount);
   #endif
   
   // must always be here
   wxPaintDC dc(this);
      
   // Check for any error condition
   if (mFatalErrorFound)
   {
      #ifdef DEBUG_ONPAINT
      MessageInterface::ShowMessage("**** ERROR **** fatal error found\n");
      #endif
      return;
   }
   
   if (!SetGLContext("in GroundTrackCanvas::OnPaint()"))
      return;
   
   if (!mGlInitialized && mObjectCount > 0)
   {
      #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundTrackCanvas::OnPaint() Calling InitOpenGL()\n");
      #endif
      InitOpenGL();
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
void GroundTrackCanvas::OnSize(wxSizeEvent& event)
{
   #ifdef DEBUG_ONSIZE
   MessageInterface::ShowMessage("GroundTrackCanvas::OnSize() entered\n");
   #endif
   
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
   
   if (!SetGLContext("in GroundTrackCanvas::OnSize()"))
      return;
   
   glViewport(0, 0, (GLint) nWidth, (GLint) nHeight);
   mCurrViewDistance = (float)(sqrt((Real)(nWidth*nWidth + nHeight*nHeight)));
   
   Refresh(false);
   Update();
   
   #ifdef DEBUG_ONSIZE
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::OnSize() leaving, w = %d, h = %d, mCurrViewDistance = %f\n",
       nWidth, nHeight, mCurrViewDistance);
   #endif
}


//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent& event)
//------------------------------------------------------------------------------
/**
 * Processes wxMouseEvent.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::OnMouse(wxMouseEvent& event)
{
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
   
   
   #ifndef __SKIP_WRITE_MOUSE_POS__
   
   if (event.Leaving())
      theStatusBar->SetStatusText("", 2);

   
   if (!((MdiChildGroundTrackFrame*)mParent)->IsActiveChild())
      return;
   
   int width, height;
   int mouseX = event.GetX();
   int mouseY = event.GetY();
   GetClientSize(&width, &height);
   
   double left    = -180.0;
   double right   =  180.0;
   double bottom  = -90.0;
   double top     =  90.0;
   
   double canvasAspect = (double)width / (double)height;
   
   // The aspect ratio of texture map for the ground track is always 2.0
   if (canvasAspect >= 2.0)
   {
      left  = canvasAspect * bottom;
      right = canvasAspect * top;
   }
   else
   {
      bottom = left  / canvasAspect;
      top    = right / canvasAspect;
   }
   
   // Flip mouseY so it is oriented bottom left is 0,0
   mouseY = height - mouseY;
   double lon = (mouseX * (right*2) / width) - right;
   double lat = (mouseY * (top*2) / height) - top;

   // if the mouse is within the texture map, show the latitude and longitude
   // in the status bar (LOJ: 2011.01.09 for bug 
   if ((lon >= -180.0 && lon <= 180.0) && (lat >= -90.0 && lat <= 90.0))
   {
      wxString mousePosStr;
      mousePosStr.Printf("Latitude:%g  Longitude:%g", lat, lon);
      theStatusBar->SetStatusText(mousePosStr, 2);
   }
   else
      theStatusBar->SetStatusText("", 2);
   
   #endif // #ifndef __SKIP_WRITE_MOUSE_POS__
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes wxKeyEvent.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::OnKeyDown(wxKeyEvent &event)
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
void GroundTrackCanvas::SetDefaultViewPoint()
{
}


//------------------------------------------------------------------------------
// void InitializeViewPoint()
//------------------------------------------------------------------------------
void GroundTrackCanvas::InitializeViewPoint()
{
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::InitializeViewPoint() entered\n");
   #endif
   
   mViewPointInitialized = true;
   
   #if DEBUG_INIT
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::InitializeViewPoint() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetDefaultView()
//------------------------------------------------------------------------------
void GroundTrackCanvas::SetDefaultView()
{
}


//------------------------------------------------------------------------------
//  void SetupWorld()
//------------------------------------------------------------------------------
/**
 * Sets view projection.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::SetupWorld()
{
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("GroundTrackCanvas::SetupWorld() entered\n");
   #endif
   
   double left    = -180.0;
   double right   =  180.0;
   double bottom  = -90.0;
   double top     =  90.0;
   
   double canvasAspect = (double)mCanvasSize.x / (double)mCanvasSize.y;
   
   // The aspect ratio of texture map for the ground track is always 2.0
   if (canvasAspect >= 2.0)
   {
      left  = canvasAspect * bottom;
      right = canvasAspect * top;
   }
   else
   {
      bottom = left  / canvasAspect;
      top    = right / canvasAspect;
   }
   
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage
      ("   left = %f, right = %f, bottom = %f, top = %f\n", left, right, bottom, top);
   #endif
   
   // Set left, right, bottom, top
   gluOrtho2D(left, right, bottom, top);
   
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("GroundTrackCanvas::SetupWorld() leaving\n");
   #endif
} // end SetupWorld()


//------------------------------------------------------------------------------
//  void DrawFrame()
//------------------------------------------------------------------------------
/**
 * Draws whole picture.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawFrame()
{
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawFrame() entered, mNumData=%d, mUsenitialViewPoint=%d\n"
       "   mViewCoordSysName=%s\n", mNumData, mUseInitialViewPoint,
       mViewCoordSysName.c_str());
   #endif
   
   if (mUseInitialViewPoint)
   {
      #ifdef USE_TRACKBALL
         ToQuat(mQuat, 0.0f, 0.0f, 0.0f, 0.0);
      #endif
         
      SetDefaultView();            
      
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
   for (int frame = 1; frame < numberOfData; frame+=mFrameInc)
   {
      mIsAnimationRunning = true;

      // Yield periodically
      publisher->Ping();

      if (mHasUserInterrupted)
         break;
      
      wxMilliSleep(mUpdateInterval);
      mNumData = frame;
      
      ComputeRingBufferIndex();
      
      Refresh(false);
   }
   
   // Draw final point if not drawn (GMT-3264 fix)
   if (mNumData < numberOfData)
   {
      mNumData = numberOfData;
      #if DEBUG_ANIMATION
      MessageInterface::ShowMessage("   ==> Drawing final point\n");
      #endif
      ComputeRingBufferIndex();
   }
   
   // Final refresh, in case number of points is less than 50
   Refresh(false);
   
   mNumData = numberOfData;
   mIsEndOfData = true;
   mIsEndOfRun = true;
   
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage("GroundTrackCanvas::DrawFrame() leaving\n");
   #endif
} // end DrawFrame()


//------------------------------------------------------------------------------
//  void DrawPlot()
//------------------------------------------------------------------------------
/**
 * Draws whole plot.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawPlot()
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawPlot() entered, mTotalPoints=%d\n",
       mTotalPoints);
   #endif

   if (mTotalPoints == 0)
      return;
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawPlot() entered, mTotalPoints=%d, mNumData=%d, mTime[%d]=%f\n",
       mTotalPoints, mNumData, mLastIndex, mTime[mLastIndex]);
   #endif
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("   mRedrawLastPointsOnly=%d, mNumPointsToRedraw=%d, mViewCsIsInternalCs=%d, "
       "mUseInitialViewPoint=%d\n", mRedrawLastPointsOnly, mNumPointsToRedraw,
       mViewCsIsInternalCs, mUseInitialViewPoint);
   MessageInterface::ShowMessage
      ("   mUseInitialViewPoint=%d, mIsEndOfData=%d, mIsEndOfRun=%d, mDrawSolverData=%d\n",
       mUseInitialViewPoint, mIsEndOfData, mIsEndOfRun, mDrawSolverData);
   #endif
   
   // Set background color to grey
   glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
   //glClearColor(0.20, 0.50, 0.50, 1.0); // green blue
   
   if (mRedrawLastPointsOnly || mNumPointsToRedraw == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_DEPTH_BUFFER_BIT);
   
   // Plot is not refreshed when another panel is opened, so add glFlush()
   // and SwapBuffers() (loj: 4/5/06)
   if (mNumData < 1 && !mDrawSolverData) // to avoid 0.0 time
   {
      glFlush();
      SwapBuffers();
      return;
   }
   
   // Do not use light source
   glDisable(GL_LIGHTING);

   // Set viewing projection
   SetupProjection();
   
   // Draw central body texture map
   DrawCentralBodyTexture();
   
   // Always draw grid lines for now, will draw on user option later
   DrawGridLines();
   
   // Draw object orbit
   DrawObjectOrbit();
   
   if (mDrawSolverData)
      DrawSolverData();
   
   DrawStatus("", GmatColor::RED, "  Epoch: ", mTime[mLastIndex], 0, 5, false);   
   
   glFlush();
   SwapBuffers();
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage("GroundTrackCanvas::DrawPlot() leaving\n");
   #endif
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
void GroundTrackCanvas::DrawObjectOrbit()
{
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("\n==========> DrawObjectOrbit() entered, mLastIndex=%d, mObjectCount=%d\n",
       mLastIndex, mObjectCount);
   #endif
   
   int objId;
   wxString objName;
   
   #if 0
   if (mEnableLightSource && mSunPresent)
   {
      // we don't want the orbit paths lit
      glDisable(GL_LIGHTING);
   }
   #endif
   
   ComputeActualIndex();
   
   for (int obj=0; obj<mObjectCount; obj++)
   {
      objName = mObjectNames[obj];
      objId = GetObjectId(objName);
      mObjLastFrame[objId] = 0;
      
      #if DEBUG_DRAW
      MessageInterface::ShowMessage
         ("DrawObjectOrbit() obj=%d, objId=%d, objName='%s'\n",
          obj, objId, objName.WX_TO_C_STRING);
      #endif
      
      #if DEBUG_DRAW_DEBUG
      DrawDebugMessage(" Before DrawOrbit --- ", GmatColor::RED, 0, 100);
      #endif
      
      // always draw spacecraft orbit trajectory
      DrawOrbit(objName, obj, objId);
      
      #if DEBUG_DRAW_DEBUG
      DrawDebugMessage(" After DrawOrbit  --- ", GmatColor::RED, 0, 120);
      #endif
      
      //---------------------------------------------------------
      // draw object with texture
      //---------------------------------------------------------      
      if (mShowObjectMap[objName])
      {
         if (objName != mCentralBodyName.c_str())
            DrawObjectTexture(objName, obj, objId);
      }      
   }
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage("==========> DrawObjectOrbit() leaving\n");
   #endif
} // end DrawObjectOrbit()


//------------------------------------------------------------------------------
// void DrawObjectTexture(const wxString &objName, int obj, int objId, int frame)
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawObjectTexture(const wxString &objName, int obj,
                                          int objId)
{
   if (mNumData < 1)
      return;
   
   int frame = mObjLastFrame[objId];
   int index2 = objId * maxData * 3 + frame * 3;
   
   #if DEBUG_DRAW
   MessageInterface::ShowMessage
      ("DrawObjectTexture() entered, objName='%s', obj=%d, objId=%d, frame=%d, "
       "index2=%d, mObjLastFrame[%d]=%d\n", objName.WX_TO_C_STRING,
       obj, objId, frame,
       index2, objId, mObjLastFrame[objId]);
   MessageInterface::ShowMessage
      ("   mObjectViewPos=%f, %f, %f\n", mObjectViewPos[index2+0],
       mObjectViewPos[index2+1], mObjectViewPos[index2+2]);
   #endif
   
   glPushMatrix();
   
   // first disable GL_TEXTURE_2D to show lines clearly
   // without this, lines are drawn dim (loj: 2007.06.11)
   glDisable(GL_TEXTURE_2D);
   
   #if 0
   //-------------------------------------------------------
   // enable light source on option
   //-------------------------------------------------------
   if (mEnableLightSource && mSunPresent)
   {
      int sunId = GetObjectId("Sun");
      int index;
      if (sunId == UNKNOWN_OBJ_ID)
      {
         mLight.SetPosition(0.01f, 1.0f, 0.3f);
         mLight.SetDirectional(true);
      }
      else
      {
         index = sunId * maxData * 3 + frame * 3;
         mLight.SetPosition(mObjectViewPos[index+0], mObjectViewPos[index+1], mObjectViewPos[index+2]);
         mLight.SetDirectional(false);
      }
      
      // Dunn is setting sunlight to be a little dimmer.
      mLight.SetColor(0.8f,0.8f,0.8f,1.0f);
      
      // reset the light position to reflect the transformations
      float lpos[4], *color = mLight.GetColor();
      mLight.GetPositionf(lpos);
      glLightfv(GL_LIGHT0, GL_POSITION, lpos);
      glLightfv(GL_LIGHT0, GL_SPECULAR, color);
      
      // enable the lighting
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
   }
   #endif
   
   
   // Draw spacecraft model
   if (mObjectArray[obj]->IsOfType(Gmat::SPACECRAFT))
   {     
      // If drawing at current position is on
      if (mIsDrawing[frame])
      {
         #if DEBUG_DRAW
         MessageInterface::ShowMessage("==> Drawing spacecraft image '%s'\n",
         objName.WX_TO_C_STRING);
         #endif
         DrawSpacecraft(objName, objId, index2);
      }
   }
   else if (mObjectArray[obj]->IsOfType(Gmat::GROUND_STATION))
   {
      // If drawing at current position is on
      if (mIsDrawing[frame])
      {
         #if DEBUG_DRAW
         MessageInterface::ShowMessage("   Drawing ground station image '%s'\n",
         objName.WX_TO_C_STRING);
         #endif
         DrawGroundStation(objName, objId, index2);
      }
   }
   else
   {
      #if DEBUG_DRAW
      MessageInterface::ShowMessage("==> Drawing body '%s'\n",
      objName.WX_TO_C_STRING);
      #endif
      
      //put object at final position
      #if 0
      glTranslatef(mObjectViewPos[index2+0],mObjectViewPos[index2+1],mObjectViewPos[index2+2]);
      #endif
      
      #if 0
      DrawObject(objName, obj);
      #endif
   }
   
   if (mEnableLightSource && mSunPresent)
   {
      glDisable(GL_LIGHTING);
   }
   
   glPopMatrix();
} // DrawObjectTexture


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
void GroundTrackCanvas::DrawObject(const wxString &objName, int obj)
{   
   int objId = GetObjectId(objName);
   
   #if DEBUG_DRAW > 1
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawObject() drawing:%s, obj=%d, objId:%d, frame:%d\n",
       objName.c_str(), obj, objId, frame);
   #endif
   
   #if 0
   //-------------------------------------------------------
   // enable light source on option
   //-------------------------------------------------------
   if (mEnableLightSource && mSunPresent)
   {
      int sunId = GetObjectId("Sun");
      int index = sunId * maxData * 3 + frame * 3;
      
      if (sunId == UNKNOWN_OBJ_ID)
      {
         mLight.SetPosition(0.01f, 1.0f, 0.3f);
      }
      else
      {
         index = sunId * maxData * 3 + frame * 3;
         mLight.SetPosition(mObjectViewPos[index+0],mObjectViewPos[index+1],mObjectViewPos[index+2]);
      }
      mLight.SetDirectional(true);
      
      // Dunn is setting sun level a little dimmer to avoid washing out the models.
      mLight.SetColor(0.8f,0.8f,0.8f,1.0f);  
      // If 4th value is zero, the light source is directional one, and
      // (x,y,z) values describes its direction.
      // If 4th value is nonzero, the light is positional, and the (x,y,z) values
      // specify the location of the light in homogeneous object coordinates.
      // By default, a positional light radiates in all directions.
      
      float lpos[4];
      mLight.GetPositionf(lpos);
      glLightfv(GL_LIGHT0, GL_POSITION, lpos);
      glLightfv(GL_LIGHT0, GL_SPECULAR, mLight.GetColor());
      
      // enable the lighting
      glEnable(GL_LIGHTING);
   }
   #endif
   
   
   #if 0
   if (mEnableLightSource && mSunPresent)
   {
      glDisable(GL_LIGHTING);
   }
   #endif
   
} // end DrawObject(const wxString &objName)


//------------------------------------------------------------------------------
// void DrawOrbitLines(int i, const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawOrbitLines(int i, const wxString &objName, int obj,
                                       int objId)
{
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Entered DrawOrbitLines  --- ", GmatColor::RED, 0, 300);
   #endif
   
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("\nGroundTrackCanvas::DrawOrbitLines() entered, i=%d, objName='%s', "
       "obj=%d, objId=%d, mTime[%3d]=%f, mTime[%3d]=%f, mIsDrawing[%3d]=%d, "
       "mIsDrawing[%3d]=%d\n", i, objName.c_str(), obj, objId, i, mTime[i],
       i-1, mTime[i-1], i, mIsDrawing[i], i-1, mIsDrawing[i-1]);
   #endif
   
   // Draw orbit lines for spacecraft only
   if (!(mObjectArray[objId]->IsOfType(Gmat::SPACECRAFT)))
   {
      // We are not drawing trajectory other than spacecraft.
      // Just settinig color here for label
      //*sIntColor = mObjectOrbitColorMap[objName].GetIntColor(); //LOJ: 2013.11.25
      // Changed to use WX_TO_STD_STRING macro for wx3.0 (LOJ: 2014.08.07)
      //*sIntColor = mObjectOrbitColorMap[objName.c_str()];
      *sIntColor = mObjectOrbitColorMap[objName.WX_TO_STD_STRING];
      
      #if DEBUG_ORBIT_LINES > 1
      MessageInterface::ShowMessage
         ("GroundTrackCanvas::DrawOrbitLines() leaving, not drawing '%s'\n", objName.c_str());
      #endif
      return;
   }
   
   // If current or previous points are not drawing, just return
   if (!mIsDrawing[i] || !mIsDrawing[i-1])
   {
      #if DEBUG_ORBIT_LINES > 1
      MessageInterface::ShowMessage
         ("GroundTrackCanvas::DrawOrbitLines() just returning, current or previous lines are not drawn\n");
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
      
      // if object position magnitude is 0, skip
      if (r1.GetMagnitude() == 0.0 || r2.GetMagnitude() == 0.0)
      {
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage
            ("GroundTrackCanvas::DrawOrbitLines() just returning, current or previous position are zero\n");
         #endif
         return;
      }
      
      // if object position diff is over limit, skip (ScriptEx_TargetHohmann)
      #ifdef SKIP_DATA_OVER_LIMIT
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("   => Skiiping data over 100000 is on\n");
      #endif
      static Real sMaxDiffDist = 100000.0;
      // if difference is more than sMaxDiffDist skip
      if ((Abs(r2[0]- r1[0]) > sMaxDiffDist && (SignOf(r2[0]) != SignOf(r1[0]))) ||
          (Abs(r2[1]- r1[1]) > sMaxDiffDist && (SignOf(r2[1]) != SignOf(r1[1]))) ||
          (Abs(r2[2]- r1[2]) > sMaxDiffDist && (SignOf(r2[2]) != SignOf(r1[2]))))
      {
         #if DEBUG_SHOW_SKIP
         MessageInterface::ShowMessage
            ("   plot=%s, i1=%d, i2=%d, time1=%f, time2=%f\n   r1=%s, r2=%s\n",
             GetName().c_str(), i-1, i, mTime[i-1], mTime[i], r1.ToString().c_str(),
             r2.ToString().c_str());
         #endif
         return;
      }
      #endif
      
      // If drawing orbit lines
      int colorIndex = objId * maxData + i;
      if (mDrawOrbitFlag[colorIndex])
      {
         // We are drawing a spacecraft orbit.  This includes solver passes.
         *sIntColor = mObjectOrbitColor[colorIndex];
         
         Rvector3 v1(mObjectViewVel[index1+0], mObjectViewVel[index1+1],
                     mObjectViewVel[index1+2]);
         
         Rvector3 v2(mObjectViewVel[index2+0], mObjectViewVel[index2+1],
                     mObjectViewVel[index2+2]);
         
         #ifdef DEBUG_ORBIT_LINES
         MessageInterface::ShowMessage("   Drawing ground track lines for '%s'\n", objName.c_str());
         #endif
         
         DrawGroundTrackLines(r1, v1, r2, v2);
         
         // draw foot print on option
         if (mFootPrintOption == 1)
         {
            // Plot shows incosistent gap between foot prints, so commented out
            //if (i % mFootPrintDrawFrequency == 0)
            DrawCircleAtCurrentPosition(objId, index2, 5.0);
         }
      }
      
      
      // save last valid frame to show object at final frame
      mObjLastFrame[objId] = i;
   }
   else
   {
      #ifdef DEBUG_ORBIT_LINES
      MessageInterface::ShowMessage("   Not drawing ground track lines for '%s'\n", objName.c_str());
      #endif
   }
   
   #ifdef DEBUG_ORBIT_LINES
   MessageInterface::ShowMessage
      ("DrawOrbitLines() leaving, mObjLastFrame[%d] = %d\n", objId, i);
   #endif
   
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Leaving DrawOrbitLines  --- ", GmatColor::RED, 0, 420);
   #endif
}


//------------------------------------------------------------------------------
// void DrawGroundTrackLines(Rvector3 &r1, Rvector3 &v1, Rvector3 &r2, Rvector3 &v2)
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawGroundTrackLines(Rvector3 &r1, Rvector3 &v1,
                                             Rvector3 &r2, Rvector3 &v2)
{
   #if DEBUG_DRAW_LINE
   MessageInterface::ShowMessage("DrawGroundTrackLines() entered\n");
   #endif
   
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Entered DrawGroundTrackLines --- ", GmatColor::RED, 0, 500);
   #endif
   
   // Compute latitude and longitude
   Real lon1, lat1, lon2, lat2;
   r1.ComputeLongitudeLatitude(lon1, lat1);
   r2.ComputeLongitudeLatitude(lon2, lat2);
   
   // Comvert to degree
   lon1 *= GmatMathConstants::DEG_PER_RAD;
   lat1 *= GmatMathConstants::DEG_PER_RAD;
   lon2 *= GmatMathConstants::DEG_PER_RAD;
   lat2 *= GmatMathConstants::DEG_PER_RAD;
   Real latM = (lat1 + lat2) / 2.0;
   
   #if DEBUG_DRAW_LINE
   MessageInterface::ShowMessage
      ("   lon1=%f, lat1=%f, lon2=%f, lat2=%f\n", lon1, lat1, lon2, lat2);
   MessageInterface::ShowMessage
      ("   color = [%u, %u, %u]\n", sGlColor->red, sGlColor->green, sGlColor->blue);
   #endif
   
   // Turn on TEXTURE_2D to dim the color, alpha doen't seem to work!!
   glEnable(GL_TEXTURE_2D);
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   // Why  it doesn't use alpha?
   //glColor4ub(sGlColor->red, sGlColor->green, sGlColor->blue, 255);
   glLineWidth(0.5);
   
   Real plusLon1 = GmatMathUtil::Mod(lon1, GmatMathConstants::TWO_PI_DEG);
   Real plusLon2 = GmatMathUtil::Mod(lon2, GmatMathConstants::TWO_PI_DEG);
   Real minusLon1 = GmatMathUtil::Mod(lon1, -GmatMathConstants::TWO_PI_DEG);
   Real minusLon2 = GmatMathUtil::Mod(lon2, -GmatMathConstants::TWO_PI_DEG);
   #if DEBUG_DRAW_LINE
   MessageInterface::ShowMessage
      ("   plusLon1=%f, plusLon2=%f, minusLon1=%f, minusLon2=%f\n",
       plusLon1, plusLon2, minusLon1, minusLon2);
   #endif
   
   bool atLhsBorder = false;
   bool atRhsBorder = false;
   Real m1, lat3;
   if ((GmatMathUtil::SignOf(lon1) == -1 && GmatMathUtil::SignOf(lon2) == 1) &&
       ((lon2 - lon1) > 180))
      atLhsBorder = true;
   if ((GmatMathUtil::SignOf(lon1) == 1 && GmatMathUtil::SignOf(lon2) == -1) &&
       ((lon1 - lon2) > 180))
      atRhsBorder = true;
   
   #if DEBUG_DRAW_LINE
   MessageInterface::ShowMessage
      ("   atLhsBorder=%d, atRhsBorder=%d\n", atLhsBorder, atRhsBorder);
   #endif
   
   if (atLhsBorder)
   {
      m1 = (lat2 - lat1) / (minusLon2 - minusLon1);
      lat3 = m1 * (-GmatMathConstants::PI_DEG - minusLon2) + lat2;
      
      #if DEBUG_DRAW_LINE
      MessageInterface::ShowMessage("------> at LHS border, m1=%f, lat3=%f\n", m1, lat3);
      #endif
      
      DrawLine(lon1, lat1, -GmatMathConstants::PI_DEG, lat3);
      DrawLine(GmatMathConstants::PI_DEG, lat3, lon2, lat2);
   }
   else if (atRhsBorder)
   {
      m1 = (lat2 - lat1) / (plusLon2 - plusLon1);
      lat3 = m1 * (GmatMathConstants::PI_DEG - plusLon2) + lat2;
            
      #if DEBUG_DRAW_LINE
      MessageInterface::ShowMessage("------> at RHS border, m1=%f, lat3=%f\n", m1, lat3);
      #endif
      
      DrawLine(lon1, lat1, GmatMathConstants::PI_DEG, lat3);
      DrawLine(-GmatMathConstants::PI_DEG, lat3, lon2, lat2);
   }
   else
   {
      #if DEBUG_DRAW_LINE
      MessageInterface::ShowMessage("   ------> at normal drawing\n");
      #endif
      
      DrawLine(lon1, lat1, lon2, lat2);
   }
   
   // Turn off TEXTURE_2D to go back to normal light
   glDisable(GL_TEXTURE_2D);
   
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Leaving DrawGroundTrackLines --- ", GmatColor::RED, 0, 500);
   #endif
   
   #if DEBUG_DRAW_LINE
   MessageInterface::ShowMessage("DrawGroundTrackLines() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void DrawGridLines()
//---------------------------------------------------------------------------
void GroundTrackCanvas::DrawGridLines()
{
   glEnable(GL_TEXTURE_2D);
   glColor4f(1.0, 1.0, 1.0, 1.0);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   Real lat1 = +90.0;
   Real lat2 = -90.0;
   Real lon1 = 0.0;
   Real lon2 = 0.0;
   wxString str;
   
   // Draw longitudes
   for (int i = -180; i <= +180; i+=30)
   {
      lon1 = (Real)i;
      lon2 = lon1;
      DrawLine(lon1, lat1, lon2, lat2);

      // Draw labels
      if (i != -180 && i != 180)
      {
         str.Printf("%d", i);
         DrawStringAt(str, lon1+1, lat2+1, 0, 1);
      }
   }
   
   // Draw latitudes
   lon1 = -180.0;
   lon2 = +180.0;
   for (int i = -90; i <= +90; i+=30)
   {
      lat1 = (Real)i;
      lat2 = lat1;
      DrawLine(lon1, lat1, lon2, lat2);
      
      // Draw labels
      if (i != -90 && i != 90)
      {
         str.Printf("%d", i);
         DrawStringAt(str, lon1+1, lat2+1, 0, 1);
      }
   }
   
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
}


//---------------------------------------------------------------------------
// void DrawCentralBodyTexture()
//---------------------------------------------------------------------------
void GroundTrackCanvas::DrawCentralBodyTexture()
{
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage("DrawCentralBodyTexture() entered\n");
   #endif
   
   if (mTextureIdMap[mCentralBodyName.c_str()] != UNINIT_TEXTURE)
   {
      glLoadIdentity();
      glColor3f(1.0, 1.0, 1.0);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mTextureIdMap[mCentralBodyName.c_str()]);
      glBegin(GL_QUADS);
      // Texture coordinate points go counter clockwise from the bottom left corner
      glTexCoord2f(0.0, 0.0);  glVertex2f(-180.0, -90.0);
      glTexCoord2f(1.0, 0.0);  glVertex2f(+180.0, -90.0);
      glTexCoord2f(1.0, 1.0);  glVertex2f(+180.0, +90.0);
      glTexCoord2f(0.0, 1.0);  glVertex2f(-180.0, +90.0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      
      // reset to drawing mode
      SetupProjection();
   }
   
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage("DrawCentralBodyTexture() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void DrawCircleAtCurrentPosition(int objId, int index, double radius, ...)
//------------------------------------------------------------------------------
/**
 * Draws a circle at current position with given radius.
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawCircleAtCurrentPosition(int objId, int index,
                                                    double radius,
                                                    bool enableTransparency)
{
   // Compute current position
   Real lon2, lat2;
   Rvector3 r2(mObjectViewPos[index+0], mObjectViewPos[index+1],
               mObjectViewPos[index+2]);

   //======================================================================
   // Actualy I can't use CartesianToSpherical() from BodyFixedStateConverter as is
   // since longitude has to be between -180 and 180. Also it constructs Rvector3 when
   // it returns the value which will slow down the performance. I need a new function
   // CartesianToLatLong(const Rvector3 &cart, Real &lat, Real &lon);
   // I don't need height, so pass 0.0 for mean radius, flattening is not used so pass 0.0
   #if 0
   Rvector3 latLonHgt =
      BodyFixedStateConverterUtil::CartesianToSpherical(r2, 0.0, 0.0);
   Real lon3 = latLonHgt[1] * GmatMathConstants::DEG_PER_RAD;
   Real lat3 = latLonHgt[0] * GmatMathConstants::DEG_PER_RAD;
   MessageInterface::ShowMessage("   lon3 = %f, lat3 = %f\n", lon3, lat3);
   #endif
   //======================================================================
   
   r2.ComputeLongitudeLatitude(lon2, lat2);
   lon2 *= GmatMathConstants::DEG_PER_RAD;
   lat2 *= GmatMathConstants::DEG_PER_RAD;
   
   #ifdef DEBUG_DRAW_SPACECRAFT
   MessageInterface::ShowMessage("   lon2 = %f, lat2 = %f\n", lon2, lat2);
   #endif
   
   // Set color
   *sIntColor = mObjectOrbitColor[objId*maxData+mObjLastFrame[objId]];
   
   // Draw circle
   DrawCircleAt(sGlColor, lon2, lat2, radius, enableTransparency);
   
}


//------------------------------------------------------------------------------
// void DrawCircleAt(GlColorType *color, double lon, double lat, double radius,
//                   bool enableTransparency = true)
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawCircleAt(GlColorType *color, double lon, double lat,
                                     double radius, bool enableTransparency)
{
   if (enableTransparency)
   {
      // Make the color transparent before drawing, set alpha value to 128
      glColor4ub(color->red, color->green, color->blue, 128);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else
      glColor3ub(color->red, color->green, color->blue);
   
   glEnable(GL_TEXTURE_2D);
   DrawCircle(lon, lat, radius, true);
   glDisable(GL_TEXTURE_2D);
   if (enableTransparency)
      glDisable(GL_BLEND);
}


//------------------------------------------------------------------------------
// void DrawImage(const wxString &objName, float lon, float lat, float imagePos)
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawImage(const wxString &objName, float lon, float lat,
                                  float imagePos, GLubyte *image)
{   
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawImage() entered, objName='%s', lon=%f, lat=%f, "
       "imagePos=%f, image=<%p>\n", objName.WX_TO_C_STRING, lon, lat, imagePos, image);
   #endif
   
   //=======================================================
   #ifdef __DRAW_WITH_BLENDING__
   //=======================================================
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage("   Drawing image with blending\n");
   #endif
   
   // This code block draws image with blending
   glPushMatrix();
   glLoadIdentity();
   glColor4f(1.0, 1.0, 1.0, 1.0);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mTextureIdMap[objName.c_str()]);
   
   // Need to enable blending in order to remove background color of png file.
   // Actually it works without enabling GL_ALPHA_TEST
   glEnable (GL_BLEND);
   //glEnable(GL_ALPHA_TEST);
   //glAlphaFunc(GL_GREATER, 0.0f); // works
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // works
   //glBlendFunc (GL_DST_COLOR, GL_ZERO); // blends with black
   //glBlendFunc (GL_SRC_ALPHA, GL_ONE); // blends with white
   
   glBegin(GL_QUADS);
   // Texture coordinate points go counter clockwise from the bottom left corner
   glTexCoord2f(0.0, 0.0);  glVertex2f(lon-imagePos, lat-imagePos);
   glTexCoord2f(1.0, 0.0);  glVertex2f(lon+imagePos, lat-imagePos);
   glTexCoord2f(1.0, 1.0);  glVertex2f(lon+imagePos, lat+imagePos);
   glTexCoord2f(0.0, 1.0);  glVertex2f(lon-imagePos, lat+imagePos);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   
   // Restore old projection matrix
   glPopMatrix();
   
   //=======================================================
   #else
   //=======================================================
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage("   Drawing image without blending\n");
   #endif
   
   // This code block draws image without blending
   GLint texW, texH;
   glBindTexture(GL_TEXTURE_2D, mTextureIdMap[objName.c_str()]);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texW);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);
   
   // Handle crossing boundary
   //@todo There should be a better way to adjust for drawing at the edge
   float newPos = imagePos;
   if (lon == 180.0 || lon == -180.0 ||  lat == 90.0 || lat == -90.0)
      newPos = 0.0;
   
   float lonPos = lon - newPos;
   float latPos = lat - newPos;
   
   glRasterPos3f(lonPos, latPos, 0.0);
   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0.0f);
   
   if (image == NULL)
   {
      GLubyte *tempImage = new GLubyte[texW * texH * 4];
      glGetTexImage(GL_TEXTURE_2D,  0,  GL_RGBA, GL_UNSIGNED_BYTE,  tempImage);
      glDrawPixels(texW, texH, GL_RGBA, GL_UNSIGNED_BYTE,  tempImage);
      delete[] tempImage;
   }
   else
   {
      glDrawPixels(texW, texH, GL_RGBA, GL_UNSIGNED_BYTE, image);
   }
   
   //=======================================================
   #endif
   //=======================================================
   
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage("GroundTrackCanvas::DrawImage() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void DrawSpacecraft(const wxString &objName, int objId, int index)
//------------------------------------------------------------------------------
/**
 * Draws a spacecraft at current position
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawSpacecraft(const wxString &objName, int objId, int index)
{
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawSpacecraft() entered, objName='%s', objId=%d, "
       "index=%d\n", objName.WX_TO_C_STRING, objId, index);
   #endif
   
   // Compute current position
   Real lon2, lat2;
   Rvector3 r2(mObjectViewPos[index+0], mObjectViewPos[index+1],
               mObjectViewPos[index+2]);
   r2.ComputeLongitudeLatitude(lon2, lat2);
   lon2 *= GmatMathConstants::DEG_PER_RAD;
   lat2 *= GmatMathConstants::DEG_PER_RAD;
   
   // Use defaultCanvasAxis to draw spacecraft image and label in proper position
   float defaultCanvasAxis = 1800.0;
   float imagePos = 2.0 * (defaultCanvasAxis / mCurrViewDistance);
   
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage
      ("   lon2 = %f, lat2 = %f, imagePos = %f\n", lon2, lat2, imagePos);
   #endif
   
   if (mTextureIdMap[objName.c_str()] != UNINIT_TEXTURE)
   {
      #if DEBUG_DRAW_IMAGE
      MessageInterface::ShowMessage("   drawing with texture map\n");
      #endif
      
      DrawImage(objName, lon2, lat2, imagePos, mScImage);
   }
   else
   {
      #if DEBUG_DRAW_IMAGE
      MessageInterface::ShowMessage("   drawing with circle\n");
      #endif
      
      // Set color
      *sIntColor = mObjectOrbitColor[objId*maxData+mObjLastFrame[objId]];
      // Draw circle for now
      DrawCircleAt(sGlColor, lon2, lat2, 3.0);
   }
   
   // Draw spacecraft label
   //@todo Need better algorithm for positioning label (LOJ: 2012.08.24)
   float lonPos = lon2 + imagePos;
   if (lon2 >= 120.0 && lon2 <= 180.0)
      lonPos = lon2 - ((imagePos - 0.3) * objName.Len());
   
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   DrawStringAt(objName, lonPos, lat2+imagePos, 0, 1);
}


//------------------------------------------------------------------------------
// void DrawGroundStation(const wxString &objName, int objId, int index)
//------------------------------------------------------------------------------
/**
 * Draws a square at current position
 */
//------------------------------------------------------------------------------
void GroundTrackCanvas::DrawGroundStation(const wxString &objName, int objId,
                                          int index)
{
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::DrawGroundStation() entered, objName='%s', objId=%d, "
       "index=%d\n", objName.WX_TO_C_STRING, objId, index);
   #endif
   
   // Don't draw stations that are not on the central body for the plot
   int gsID = GetObjectId(objName.c_str());
   BodyFixedPoint *gs = (BodyFixedPoint*)mObjectArray[gsID];
   if (gs)
   {
      std::string gsBodyName = gs->GetStringParameter("CentralBody");
      if (gsBodyName != mCentralBodyName)
      {
         // Write wrong central body message only once
         if (mWriteCentralBodyWarning)
         {
            MessageInterface::ShowMessage
            ("*** WARNING ***: GroundStation %s has central body %s, "
             "but central body for Ground Track plot is %s, so "
             "GroundStation is not displayed.\n", gs->GetName().c_str(),
             gsBodyName.c_str(), mCentralBodyName.c_str());
            mWriteCentralBodyWarning = false;
         }
         return;
      }
   }
   
   
   // Compute current position
   Real lon2, lat2;
   Rvector3 r2(mObjectViewPos[index+0], mObjectViewPos[index+1],
               mObjectViewPos[index+2]);
   
   r2.ComputeLongitudeLatitude(lon2, lat2);
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage("   r2   = %s", r2.ToString().c_str());
   MessageInterface::ShowMessage("   lon2 = %f, lat2 = %f\n", lon2, lat2);
   #endif
   
   lon2 *= GmatMathConstants::DEG_PER_RAD;
   lat2 *= GmatMathConstants::DEG_PER_RAD;
   
   // Use defaultCanvasAxis to draw spacecraft image and label in proper position
   float defaultCanvasAxis = 1800.0;
   float imagePos = 2.0 * (defaultCanvasAxis / mCurrViewDistance);
   
   #if DEBUG_DRAW_IMAGE
   MessageInterface::ShowMessage
      ("   lon2 = %f, lat2 = %f, imagePos = %f\n", lon2, lat2, imagePos);
   #endif
   
   if (mTextureIdMap[objName.c_str()] != UNINIT_TEXTURE)
   {
      #if DEBUG_DRAW_IMAGE
      MessageInterface::ShowMessage("   drawing with texture map\n");
      #endif
      
      DrawImage(objName, lon2, lat2, imagePos, mGsImage);
   }
   else
   {
      #if DEBUG_DRAW_IMAGE
      MessageInterface::ShowMessage("   drawing with square\n");
      #endif
      
      // Set color to yellow
      GlColorType *objColor = (GlColorType*)&GmatColor::YELLOW;
      //GlColorType *objColor = (GlColorType*)&GmatColor::RED;
      
      // Make the color transparent before drawing, set alpha value to 128
      glColor4ub(objColor->red, objColor->green, objColor->blue, 128);
      glEnable(GL_TEXTURE_2D);
      // If blending image is enabled
      #ifdef __ENABLE_BLEND_IMAGE__
      glEnable (GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      #endif
      DrawSquare(lon2, lat2, 2.0, true);
      glDisable(GL_TEXTURE_2D);
      glDisable (GL_BLEND); 
   }
   
   // Draw ground station label
   //@todo Need better algorithm for positioning label (LOJ: 2012.08.24)
   float lonPos = lon2 + imagePos;
   if (lon2 >= 120.0 && lon2 <= 180.0)
      lonPos = lon2 - ((imagePos - 0.3) * objName.Len());
   
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
   DrawStringAt(objName, lonPos, lat2+imagePos, 0, 1);
}


//---------------------------------------------------------------------------
// void RotateBodyUsingAttitude(const wxString &objName, int objId)
//---------------------------------------------------------------------------
void GroundTrackCanvas::RotateBodyUsingAttitude(const wxString &objName, int objId)
{
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s' entered, objId=%d, mLastIndex=%d, time=%f\n",
       objName.WX_TO_C_STRING, objId, mLastIndex, mTime[mLastIndex]);
   #endif
   
   if (mTime[mLastIndex] == 0.0)
      return;
   
   // Any object that has an attitude (spacecraft, celestial sphere, and celestial bodies)
   // needs to be oriented correctly in the  coordinate system in which the Orbit View is drawn.    
   //
   // Define the following matrices:
   // R_IP:  the rotation matrix from the coordinate system of the plot to the inertial coordinate system 
   // R_IB:  the rotation matrix from celestial body fixed to inertial, for the body to be drawn in the Orbit View
   //
   // We can calculate the rotation matrix from celestial body fixed to the plot coordinate system, R_BP, using:
   //
   // R_BP = R_IB^T*R_IP;
   //
   // R_PB defines the rotation that must be applied to the celestial body before drawing in the Orbit View.
   // I don't know what OpenGL method is being used for this so we can talk about that this afternoon.
   // We may need to convert R_PB to quaternion or Euler angles  but the most efficient way would be to
   // just pass in R_PB.
   
   // Rotate body
   int attIndex = objId * maxData * 4 + mLastIndex * 4;
   
   Rvector quat = Rvector(4, mObjectQuat[attIndex+0], mObjectQuat[attIndex+1],
                          mObjectQuat[attIndex+2], mObjectQuat[attIndex+3]);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==================================\n");
   MessageInterface::ShowMessage
      ("==> attIndex=%d, quat=%s\n", attIndex, quat.ToString(16, 1).c_str());
   #endif
   
   if (quat.IsZeroVector())
      return;
   
   // the rotation matrix from celestial body fixed to inertial
   Rmatrix33 matIB = AttitudeConversionUtility::ToCosineMatrix(quat);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matIB=\n%s", matIB.ToString(16, 25).c_str());
   #endif
   
   // Get the rotation matrix from the coordinate system of the plot to the inertial coordinate system
   Rvector6 inState, outState;
   int posIndex = objId * maxData * 3 + mLastIndex * 3;
   inState.Set(mObjectGciPos[posIndex+0], mObjectGciPos[posIndex+1],
               mObjectGciPos[posIndex+2], 0.0, 0.0, 0.0);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("   posIndex=%d, inState=%s\n", posIndex, inState.ToString(16, 1).c_str());
   MessageInterface::ShowMessage
      ("   pViewCoordSystem=<%p>'%s', pInternalCoordSystem=<%p>'%s'\n",
       pViewCoordSystem,
       pViewCoordSystem ? pViewCoordSystem->GetName().c_str() : "NULL",
       pInternalCoordSystem,
       pInternalCoordSystem ? pInternalCoordSystem->GetName().c_str() : "NULL");
   #endif

   mCoordConverter.Convert(mTime[mLastIndex], inState, pViewCoordSystem,
                           outState, pInternalCoordSystem);
   
   Rmatrix33 matIP = mCoordConverter.GetLastRotationMatrix();
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matIP=\n%s", matIP.ToString(16, 25).c_str());
   #endif
   
   Rmatrix33 matBP = matIB.Transpose() * matIP;
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage("==> matPB=\n%s", matBP.ToString(16, 25).c_str());
   #endif
   
   // Compute angle and axis
   Rvector3 eAxis;
   Real eAngle;
   AttitudeConversionUtility::DCMToEulerAxisAndAngle(matBP, eAxis, eAngle);
   
   // Convert to degree
   Real angInDeg = GmatMathUtil::RadToDeg(eAngle, true);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("RotateBodyUsingAttitude() '%s', epoch=%f, attIndex=%d, eAngle=%f, "
       "angInDeg=%f\n   eAxis=%s\n", objName.WX_TO_C_STRING, mTime[mLastIndex],
        attIndex, eAngle, angInDeg, eAxis.ToString().c_str());
   #endif
   
   // Now rotate
   glRotated(angInDeg, eAxis[0], eAxis[1], eAxis[2]);
}


//---------------------------------------------------------------------------
// void RotateBody(const wxString &objName, int frame, int objId)
//---------------------------------------------------------------------------
void GroundTrackCanvas::RotateBody(const wxString &objName, int frame, int objId)
{
   #ifdef DEBUG_ROTATE_BODY
   MessageInterface::ShowMessage
      ("RotateBody() '%s' entered, objName='%s', objId=%d, mOriginId=%d, "
       "mOriginName='%s'\n", mPlotName.WX_TO_C_STRING, objName.WX_TO_C_STRING,
       objId, mOriginId, mOriginName.WX_TO_C_STRING);
   #endif
   
   // Rotate other body
   RotateBodyUsingAttitude(objName, objId);
}


//---------------------------------------------------------------------------
// bool ConvertObjectData()
//---------------------------------------------------------------------------
bool GroundTrackCanvas::ConvertObjectData()
{
   if (pInternalCoordSystem == NULL || pViewCoordSystem == NULL)
      return false;
   
   Rvector6 inState, outState;
   
   #if DEBUG_CONVERT
   MessageInterface::ShowMessage
      ("GroundTrackCanvas::ConvertObjectData() internalCS=%s, viewCSName=%s, viewCS=%d\n",
       pInternalCoordSystem->GetName().c_str(), pViewCoordSystem->GetName().c_str(),
       pViewCoordSystem);
   #endif
   
   // do not convert if view CS is internal CS
   if (mViewCsIsInternalCs)
   {
      #if DEBUG_CONVERT
      MessageInterface::ShowMessage
         ("GroundTrackCanvas::ConvertObjectData() No conversion is needed. "
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
            ("GroundTrackCanvas::ConvertObjectData() mObjectNames[%d]=%s\n", objId,
             mObjectNames[i].c_str());
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
void GroundTrackCanvas::ConvertObject(int objId, int index)
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


//---------------------------------------------------------------------------
// int GetMaxDataPoints()
//---------------------------------------------------------------------------
int GroundTrackCanvas::GetMaxDataPoints()
{
   return maxData;
}


//---------------------------------------------------------------------------
// void SetMaxDataPoints(int maxPlotPoints)
//---------------------------------------------------------------------------
void GroundTrackCanvas::SetMaxDataPoints(int maxPlotPoints)
{
   maxData = maxPlotPoints;
}
