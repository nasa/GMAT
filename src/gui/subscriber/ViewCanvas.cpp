//$Id$
//------------------------------------------------------------------------------
//                              ViewCanvas
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
 * Implements ViewCanvas for 3D visualization.
 */
//------------------------------------------------------------------------------
#include "ViewCanvas.hpp"
#include "gmatwxdefs.hpp"          // for WX_TO_STD_STRING, STD_TO_WX_STRING macros
#include "MdiGlPlotData.hpp"       // for GmatPlot::MAX_SCS
#include "Spacecraft.hpp"
#include "ModelManager.hpp"
#include "Rendering.hpp"           // for GlColorType
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "FileManager.hpp"         // for GetFullPathname()
#include "GmatOpenGLSupport.hpp"   // for InitGL()
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"       // for GetMainFrameStatusBar()
#include "GuiInterpreter.hpp"
#include "SubscriberException.hpp"
#include "MessageInterface.hpp"
#include "AttitudeConversionUtility.hpp"

// Libpng-1.6 is more stringent about checking ICC profiles than previous versions.
// You can ignore the warning. To get rid of it, remove the iCCP chunk from the PNG image.
// For now just ignore warning (LOJ: 2014.09.23)
#define __IGNORE_PNG_WARNING__

//http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
// When you desire high-quality texture mapping, you will typically specify a
// mipmapped texture filter. Mipmapping lets you specify multiple levels of
// detail for a texture image. Each level of detail is half the size of the
// previous level of detail in each dimension. So if your initial texture
// image is an image of size 32x32, the lower levels of detail will be of
// size 16x16, 8x8, 4x4, 2x2, and 1x1. Typically, you use the gluBuild2DMipmaps
// routine to automatically construct the lower levels of details from you
// original image. This routine re-samples the original image at each level
// of detail so that the image is available at each of the various smaller sizes.
#define __ENABLE_MIPMAPS__

//#define DEBUG_GL_INFO
//#define DEBUG_INIT 1
//#define DEBUG_CS 1
//#define DEBUG_LOAD_IMAGE 1
//#define DEBUG_TEXTURE 1
//#define DEBUG_LOAD_MODEL 1
//#define DEBUG_GL_CONTEXT 1
//#define DEBUG_DRAWING_MODE
//#define DEBUG_OBJECT 2
//#define DEBUG_DATA_BUFFERRING 1
//#define DEBUG_UPDATE 2
//#define DEBUG_UPDATE_OBJECT 1
//#define DEBUG_UPDATE_VIEW 1
//#define DEBUG_DRAW 1
//#define DEBUG_SOLVER_DATA 1
//#define DEBUG_ECLIPTIC_PLANE
//#define DEBUG_ROT_MAT
//#define DEBUG_ATTITUDE
//#define DEBUG_ROTATE_BODY 2

//---------------------------------
// static data
//---------------------------------
const Real ViewCanvas::RADIUS_ZOOM_RATIO = 2.2;
int ViewCanvas::GmatGLCanvasAttribs[4] = { WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
// color
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// ViewCanvas(wxWindow *parent, wxWindowID id, ...)
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
ViewCanvas::ViewCanvas(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       const wxString& name, long style)
   // @note
   // I want to use wxFULL_REPAINT_ON_RESIZE for style so when window is resized
   // it automatically repaints, but it flickers too much. I tried to use
   // wxBufferedPaintDC() to reduce the flickering, but I see no difference.
   // Currently it repaints when I make window size larger, but not when I
   // reduce the size. (LOJ: 2011.07.01)
   // Repainting issue when reducing size was resolved by adding Refresh(false)
   // and Update(). (LOJ: 2012.05.03 for GMT-2582 fix)
//   #ifdef __WXMSW__
//   // Constructor with explicit wxGLContext with default GL attributes
//   // It is getting pixel format error with GmatGLCanvasAttribs
//   : wxGLCanvas(parent, id, 0, pos, size, style, name)
//   #elif __WXMAC__
//   // Constructor with implicit wxGLContext with default GL attributes
//   : wxGLCanvas(parent, id, pos, size, style, name)
//   #elif __linux
   // Constructor with explicit wxGLContext
   // Double buffer activation needed in Linux (Patch from Tristan Moody)
   : wxGLCanvas(parent, id, ViewCanvas::GmatGLCanvasAttribs, pos, size, style, name)
//   #else
//   // Constructor with explicit wxGLContext with default GL attributes
//   : wxGLCanvas(parent, id, 0, pos, size, style, name)
//   #endif
   // Fixed unitialized value error     
   , mTime( NULL ),
     mIsDrawing( NULL ),
     mObjectRadius( NULL ),
     mObjMaxZoomIn( NULL ),
     mObjLastFrame( NULL ),
     mDrawOrbitFlag( NULL ),
     mObjectOrbitColor( NULL ),
     mObjectGciPos( NULL ),
     mObjectViewPos( NULL ),
     mObjectViewVel( NULL ),
     mObjectQuat( NULL ),
     mBodyRotAngle( NULL ),
     mBodyRotAxis( NULL )
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("ViewCanvas() constructor entered, name='%s'\n", name.WX_TO_C_STRING);
   #endif
   
   // Initialize pointers
   mParent = parent;
   GmatAppData *gmatAppData = GmatAppData::Instance();
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();
   theStatusBar = gmatAppData->GetMainFrame()->GetMainFrameStatusBar();
   mCanvasSize = size;
   
   // Initialization
   mPlotName = name;
   mGlInitialized = false;
   mViewPointInitialized = false;
   mSatModelsAreLoaded = false;
   mOtherModelsAreLoaded = false;
   mIsNewFrame = true;
   
   // Performance
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   mNumPointsToRedraw = 0;
   mRedrawLastPointsOnly = false;
   mUpdateFrequency = 50;
   
   // Ring buffer index
   mBeginIndex1 = 0;
   mBeginIndex2 = -1;
   mEndIndex1 = -1;
   mEndIndex2 = -1;
   mRealBeginIndex1 = 0;
   mRealBeginIndex2 = -1;
   mRealEndIndex1 = -1;
   mRealEndIndex2 = -1;
   mLastIndex = 0;
   mCurrIndex = -1;
   
   // Data points
   mNumData = 0;
   mTotalPoints = 0;
   maxData = 20000;
   
   // Data control flags
   mOverCounter = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mWriteWarning = true;
   mNeedVelocity = false;
   mNeedAttitude = false;
   mNeedEcliptic = false;
   mIgnoreTimeSequence = false;
   
   // drawing options
   mDrawWireFrame = false;
   mDrawGrid = false;
   mShowLabels = false;
   
   // Light source
   // The base subscriber OrbitView always adds Sun to the object list
   mSunPresent = true;
   // If light source is enabled, it will use Sun as light source,
   // otherwise it will use ambient light
   mEnableLightSource = true;
   // This flag allows to use Sun as light source only if Sun is drawing
   mEnableLightSourceOnlyIfSunIsDrawing = false;
   
   // View control
   mUseInitialViewPoint = true;
   mCurrViewDistance = 30000.0;
   
   // Animation
   mIsAnimationRunning = false;
   mHasUserInterrupted = false;
   mUpdateInterval = 1;
   mFrameInc = 1;
   
   // Message
   mShowMaxWarning = true;
   mOverCounter = 0;
   
   // Solver data
   mDrawSolverData = false;
   mIsSolving = false;
   
   // Error handling and function mode
   mFatalErrorFound = false;
   mWriteRepaintDisalbedInfo = false;
   mInFunction = false;
   
   // Solar system
   pSolarSystem = NULL;
   
   // Spacecraft
   mScCount = 0;
   mScImage = NULL;
   
   // GroundStatjion
   mGsImage = NULL;
   
   // Objects
   mObjectCount = 0;
   mObjectDefaultRadius = 200; //km: make big enough to see
   mOriginRadius = mObjectDefaultRadius;
   
   // Coordinate System
   pInternalCoordSystem = theGuiInterpreter->GetInternalCoordinateSystem();
   mInternalCoordSysName = wxString(pInternalCoordSystem->GetName().c_str());
   mViewCoordSysName = "";
   pViewCoordSystem = NULL;
   mViewCsIsInternalCs = true;
   pMJ2000EcCoordSystem = NULL;
   
   // Initialize dynamic arrays to NULL
   ClearObjectArrays(false);
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("ViewCanvas() constructor leaving, name='%s'\n", name.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// ~ViewCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ViewCanvas::~ViewCanvas()
{
   #ifdef DEBUG_VIEWCANVAS
   MessageInterface::ShowMessage("ViewCanvas::~ViewCanvas() '%s' entered\n", mPlotName.c_str());
   #endif
      
   // Cleanup textures
   for (std::map<wxString, GLuint>::iterator i = mTextureIdMap.begin();
        i != mTextureIdMap.end(); ++i)
   {
      GLuint texId = i->second;
      #ifdef DEBUG_VIEWCANVAS
      MessageInterface::ShowMessage("   texId = %3d, obj = '%s'\n", texId, i->first.c_str());
      #endif
      if (i->first != "" && texId != UNINIT_TEXTURE)
      {
         glDeleteTextures(1, &texId);
      }
   }
   
   // delete image data
   if (mScImage)
      delete [] mScImage;
   
   if (mGsImage)
      delete [] mGsImage;
   
   if (pMJ2000EcCoordSystem != nullptr)
   {
      delete pMJ2000EcCoordSystem;
      pMJ2000EcCoordSystem = nullptr;
   }
   #ifdef DEBUG_VIEWCANVAS
   MessageInterface::ShowMessage("ViewCanvas::~ViewCanvas() '%s' leaving\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool SetGLContext(const wxString &msg)
//------------------------------------------------------------------------------
/**
 * Sets GL context to ModelManager which stores shared GL context pointer.
 *
 * @return true if GL context is not NULL, false otherwise
 */
//------------------------------------------------------------------------------
bool ViewCanvas::SetGLContext(const wxString &msg)
{
   #ifdef DEBUG_GL_CONTEXT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGLContext() entered, theContext=<%p>\n", theContext);
   #endif
   
   bool retval = false;
   
   #ifndef __WXMAC__
      ModelManager *mm = ModelManager::Instance();
      if (!mm->GetSharedGLContext())
      {
         wxGLContext *glContext = new wxGLContext(this);
         #ifdef DEBUG_GL_CONTEXT
         MessageInterface::ShowMessage
            ("   Setting new wxGLContext(this)<%p> to ModelManager::theContext\n", glContext);
         #endif
         mm->SetSharedGLContext(glContext);
      }
      
      // Use the shared context from the ModelManager
      theContext = mm->GetSharedGLContext();
      if (theContext)
      {
         theContext->SetCurrent(*this);
         SetCurrent(*theContext);
         retval = true;
      }
      else
      {
         #ifdef DEBUG_GL_CONTEXT
         MessageInterface::ShowMessage("**** ERROR **** Cannot set GL context %s\n", msg.c_str());
         #endif
      }
   #else
      // Use implicit GL context on Mac
//      theContext = GetContext();
//      SetCurrent();
      retval = true;
   #endif
   
   #ifdef DEBUG_GL_CONTEXT
   MessageInterface::ShowMessage
      ("VisualModelCanvas::SetGLContext() returning %d, theContext=<%p>\n",
       retval, theContext);
   #endif
   
   return retval;   
}


//------------------------------------------------------------------------------
// bool InitializePlot()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::InitializePlot()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("\nViewCanvas::InitializePlot() '%s' entered, theContext=<%p>, mIsNewFrame=%d, "
       "mUseInitialViewPoint=%d\n", mPlotName.WX_TO_C_STRING, theContext, mIsNewFrame,
       mUseInitialViewPoint);
   #endif
   
   // Add things to do here
   wxPaintDC dc(this);
   
   if (!SetGLContext("in ViewCanvas::InitializePlot()"))
      return false;
   
	// initialize opengl
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("   Calling InitOpenGL()\n");
   #endif
   InitOpenGL();
   
   // If starting in new frame or using initial setup then initialize view
   // else just stay in the last view
   if (mIsNewFrame || mUseInitialViewPoint)
      mViewPointInitialized = false;
   else
      mViewPointInitialized = true;
   
   // load body textures
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("   Calling LoadBodyTextures()\n");
   #endif
   LoadBodyTextures();
   
   // load spacecraft models
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("   Calling LoadSpacecraftModels()\n");
   #endif
   LoadSpacecraftModels(true);
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitializePlot() '%s' leaving\n\n", mPlotName.WX_TO_C_STRING);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool InitOpenGL()
//------------------------------------------------------------------------------
/**
 * Initializes GL and IL.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::InitOpenGL()
{   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitOpenGL() '%s' entered, mGlInitialized = %d\n",
       mPlotName.WX_TO_C_STRING, mGlInitialized);
   #endif
   
   #ifdef DEBUG_GL_INFO
   char *GL_version = (char*)glGetString(GL_VERSION);
   MessageInterface::ShowMessage("   GL version  = '%s'\n", GL_version);
   char *GL_vendor = (char*)glGetString(GL_VENDOR);
   MessageInterface::ShowMessage("   GL vendor   = '%s'\n", GL_vendor);
   char *GL_renderer = (char*)glGetString(GL_RENDERER);
   MessageInterface::ShowMessage("   GL renderer = '%s'\n", GL_renderer);
   char *GLU_version = (char*)gluGetString(GLU_VERSION);
   MessageInterface::ShowMessage("   GLU version = '%s'\n", GLU_version);
   #endif
   
   #ifdef DEBUG_GL_MORE_INFO
   char *GL_ext = (char*)glGetString(GL_EXTENSIONS);
   char *GLU_ext = (char*)gluGetString(GLU_EXTENSIONS);
   MessageInterface::ShowMessage("   GL extensions = '%s'\n", GL_ext);
   MessageInterface::ShowMessage("   GLU extensions = '%s'\n", GLU_ext);
   #endif
	
	if (mGlInitialized)
	{
      #ifdef DEBUG_INIT
		MessageInterface::ShowMessage
			("ViewCanvas::InitOpenGL() '%s', GL already initialized so returning true\n",
			 mPlotName.WX_TO_C_STRING);
      #endif
		return true;
	}
   
   if (!SetGLContext("in ViewCanvas::InitOpenGL()"))
      return false;
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("   Calling InitGL()\n");
   #endif
	
   InitGL();
	
   mShowMaxWarning = true;
   mIsAnimationRunning = false;
   mGlInitialized = true;
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::InitOpenGL() '%s' returning true\n", mPlotName.WX_TO_C_STRING);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// virtual void SetObjectOrbitColors(const ColorMap &orbitColorMap)
//------------------------------------------------------------------------------
void ViewCanvas::SetObjectOrbitColors(const ColorMap &orbitColorMap)
{
   mObjectOrbitColorMap = orbitColorMap;
}

//------------------------------------------------------------------------------
// virtual void SetObjectTargetColors(const ColorMap &targetColorMap)
//------------------------------------------------------------------------------
void ViewCanvas::SetObjectTargetColors(const ColorMap &targetColorMap)
{
   mObjectTargetColorMap = targetColorMap;
}

//------------------------------------------------------------------------------
// virtual void SetShowObjects(const wxStringBoolMap &objectColorMap)
//------------------------------------------------------------------------------
void ViewCanvas::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   mShowObjectMap = showObjMap;
}

//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames,
//                  const std::vector<SpacePoint*> &objArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlObject(const StringArray &objNames,
                             const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlObject() '%s' entered, objCount=%d, objArrayCount=%d\n",
       mPlotName.WX_TO_C_STRING, objNames.size(), objArray.size());
   #endif
   
   mObjectArray = objArray;
   wxArrayString tempList;
   int scCount = 0;
   
   if (objNames.size() == objArray.size())
   {      
      for (UnsignedInt i=0; i<objNames.size(); i++)
      {
         tempList.Add(objNames[i].c_str());
         
         // Set spacecraft names and count
         if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            mScNameArray.push_back(objNames[i]);
            scCount++;
         }
         
         #if DEBUG_OBJECT > 1
         MessageInterface::ShowMessage
            ("   objNames[%2d]=%s, objPtr=<%p>%s\n", i, objNames[i].c_str(),
             mObjectArray[i], mObjectArray[i]->GetName().c_str());
         #endif
      }
      
      AddObjectList(tempList);
   }
   else
   {
      MessageInterface::ShowMessage("ViewCanvas::SetGlObject() object sizes "
                                    "are not the same. No objects added.\n");
   }
   
   mScCount = scCount;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlObject() '%s' leaving, mScCount=%d\n",
      mPlotName.WX_TO_C_STRING, mScCount);
   #endif
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void ViewCanvas::SetSolarSystem(SolarSystem *ss)
{
   pSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetGlCoordSystem(CoordinateSystem *internalCs,CoordinateSystem *viewCs,
//                       CoordinateSystem *viewUpCs)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlCoordSystem(CoordinateSystem *internalCs,
                                  CoordinateSystem *viewCs,
                                  CoordinateSystem *viewUpCs)
{
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlCoordSystem() '%s' entered, internalCs=<%p>, viewCs=<%p>, "
       " viweUpCs=%p\n",  mPlotName.c_str(), internalCs, viewCs, viewUpCs);
   #endif
   
   if (internalCs == NULL || viewCs == NULL || viewUpCs == NULL)
   {
      throw SubscriberException
         ("Internal or View or View Up CoordinateSystem is NULL\n"); 
   }
   
   pInternalCoordSystem = internalCs;
   mInternalCoordSysName = internalCs->GetName().c_str();
   
   pViewCoordSystem = viewCs;
   mViewCoordSysName = viewCs->GetName().c_str();
   
   pViewUpCoordSystem = viewUpCs;
   mViewUpCoordSysName = viewUpCs->GetName().c_str();
   
   // see if we need data conversion
   if (mViewCoordSysName.IsSameAs(mInternalCoordSysName))
      mViewCsIsInternalCs = true;
   else
      mViewCsIsInternalCs = false;
   
   // set view center object
   mOriginName = viewCs->GetOriginName().c_str();
   mOriginId = GetObjectId(mOriginName);
   mOriginRadius = mObjectRadius[mOriginId];
   
   mViewObjName = mOriginName;
   mViewObjId = mOriginId;
      
   #if DEBUG_CS
   MessageInterface::ShowMessage
      ("   mViewCoordSysName=%s, pViewCoordSystem=%p, mOriginName=%s, "
       "mOriginId=%d, mOriginRadius=%f\n", mViewCoordSysName.c_str(), pViewCoordSystem,
       mOriginName.c_str(),  mOriginId, mOriginRadius);
   MessageInterface::ShowMessage
      ("   mViewUpCoordSysName=%s, mViewObjName=%s, mViewObjId=%d\n",
       mViewUpCoordSysName.c_str(), mViewObjName.c_str(), mViewObjId);
   #endif
   
} // end SetGlCoordSystem()


//---------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &centralBodyName,
//         const std::string &textureMap, bool drawFootPrints)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl2dDrawingOption(const std::string &centralBodyName,
                                      const std::string &textureMap,
                                      Integer footPrintOption)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane, bool drawXyPlane, ...)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                                      bool drawXyPlane, bool drawWireFrame,
                                      bool drawAxes, bool drawGrid, bool drawSunLine,
                                      bool usevpInfo, bool drawStars,
                                      bool drawConstellations,
                                      Integer starCount)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj, ...)
//---------------------------------------------------------------------------
void ViewCanvas::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                   SpacePoint *vdObj, Real vscaleFactor,
                                   const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                   const Rvector3 &vdVec, const std::string &upAxis,
                                   bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   // do nothing here
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   mDrawOrbitArray = drawArray;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawOrbitFlag() mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d\n", mDrawOrbitArray.size(), mObjectCount);
   
   bool draw;
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? true : false;      
      MessageInterface::ShowMessage
         ("ViewCanvas::SetGlDrawOrbitFlag() i=%2d, mDrawOrbitArray[%s]=%d\n",
          i, mObjectNames[i].WX_TO_C_STRING, draw);
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void ViewCanvas::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   mShowObjectArray = showArray;

   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawObjectFlag() entered, mDrawOrbitArray.size()=%d, "
       "mObjectCount=%d, mEnableLightSourceOnlyIfSunIsDrawing=%d\n",
       mShowObjectArray.size(), mObjectCount, mEnableLightSourceOnlyIfSunIsDrawing);
   #endif
   
   bool show;
   if (mEnableLightSourceOnlyIfSunIsDrawing)
      mEnableLightSource = false;
   else
      mEnableLightSource = true;
   
   for (int i=0; i<mObjectCount; i++)
   {
      show = mShowObjectArray[i] ? true : false;
      mShowObjectMap[mObjectNames[i]] = show;
      
      if (mEnableLightSourceOnlyIfSunIsDrawing)
      {
         if (mObjectNames[i] == "Sun" && mShowObjectMap["Sun"])
            mEnableLightSource = true;
      }
      
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::SetGlShowObjectFlag() i=%2d, mShowObjectMap[%s]=%d\n",
          i, mObjectNames[i].WX_TO_C_STRING, show);
      #endif
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::SetGlDrawObjectFlag() leaving, mEnableLightSource=%d, mSunPresent=%d\n",
       mEnableLightSource, mSunPresent);
   #endif
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void ViewCanvas::SetNumPointsToRedraw(Integer numPoints)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ViewCanvas::SetNumPointsToRedraw() entered, numPoints=%d\n", numPoints);
   #endif
   
   mNumPointsToRedraw = numPoints;
   mRedrawLastPointsOnly = false;
   
   // if mNumPointsToRedraw =  0 It redraws whole plot
   // if mNumPointsToRedraw = -1 It does not clear GL_COLOR_BUFFER
   if (mNumPointsToRedraw > 0)
      mRedrawLastPointsOnly = true;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ViewCanvas::SetNumPointsToRedraw() leaving, mRedrawLastPointsOnly=%d\n",
       mRedrawLastPointsOnly);
   #endif
}


//------------------------------------------------------------------------------
// void SetUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void ViewCanvas::SetUpdateFrequency(Integer updFreq)
{
   mUpdateFrequency = updFreq;
}


//---------------------------------------------------------------------------
// wxString GetGotoObjectName()
//---------------------------------------------------------------------------
wxString ViewCanvas::GetGotoObjectName()
{
   // return blank name
   return "";
}


//---------------------------------------------------------------------------
// void GotoObject(const wxString &objName)
//---------------------------------------------------------------------------
void ViewCanvas::GotoObject(const wxString &objName)
{
   // do nothing here
}


//------------------------------------------------------------------------------
// void SetEndOfRun(bool flag = true)
//------------------------------------------------------------------------------
void ViewCanvas::SetEndOfRun(bool flag)
{
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ViewCanvas::SetEndOfRun() '%s' entered, flag=%d, mNumData=%d\n",
       mPlotName.WX_TO_C_STRING, flag, mNumData);
   #endif
   
   mIsEndOfRun = flag;
   mIsEndOfData = flag;
   
   if (mNumData < 1)
   {
      Refresh(false);
      Update();
      return;
   }
}


//---------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//---------------------------------------------------------------------------
/**
 * Perform any actions for GL plot
 */
//---------------------------------------------------------------------------
void ViewCanvas::TakeAction(const std::string &action)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("ViewCanvas::TakeAction() '%s' entered, action = '%s'\n", mPlotName.c_str(),
       action.c_str());
   #endif
   
   if (action == "ClearSolverData")
   {
      mSolverAllPosX.clear();
      mSolverAllPosY.clear();
      mSolverAllPosZ.clear();
      mSolverIterColorArray.clear();
      mSolverIterScNameArray.clear();
   }
   else if (action == "ClearObjects")
   {
      mObjectCount = 0;
      mObjectArray.clear();
   }
   else if (action == "PenUp")
   {
      mIsDrawing[mLastIndex] = true;
   }
   else if (action == "PenDown")
   {
      // We don't want to connect to new point so set it to false
      mIsDrawing[mLastIndex] = false;
   }
   else if (action == "IgnoreTimeSequence")
   {
      // This is indicates not to check for time in order
      // This will allow showing unsynchronized propagation
      mIgnoreTimeSequence = true;
   }
   
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("ViewCanvas::TakeAction() '%s' leaving, action = '%s'\n", mPlotName.c_str(),
       action.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, const Real &time, ...
//------------------------------------------------------------------------------
/**
 * Updates spacecraft trajectory data buffer. Incoming spacecraft position and
 * velocity should be in view coordinate system as this canvas does not convert
 * the state in view coordinate system. It buffers incoming spacecraft data in
 * UpdateSpacecraftData(). All other celestial object data are updated by
 * calling GetMJ2000State(time) in UpdateOtherData()
 *
 * @param <scNames> spacecraft name array
 * @param <time> time
 * @param <posX> position x array
 * @param <posY> position y array
 * @param <posZ> position z array
 * @param <velX> velocity x array
 * @param <velY> velocity y array
 * @param <velZ> velocity z array
 * @param <scColors> orbit color array
 * @param <solving> true if GMAT is running solver
 * @param <solverOption> solver iteration drawing option can be one of following:
 *           0 (Subscriber::SI_ALL)     = Draw all iteration
 *           1 (Subscriber::SI_CURRENT) = Draw current iteration only
 *           2 (Subscriber::SI_NONE)    = Draw no iteration
 * @param <drawing> true if drawing is enabled
 * @param <inFunction> true if data is published inside a function
 */
//------------------------------------------------------------------------------
void ViewCanvas::UpdatePlot(const StringArray &scNames, const Real &time,
                            const RealArray &posX, const RealArray &posY,
                            const RealArray &posZ, const RealArray &velX,
                            const RealArray &velY, const RealArray &velZ,
                            const ColorMap &orbitColorMap,
                            const ColorMap &targetColorMap,
                            bool solving, Integer solverOption,
                            bool drawing, bool inFunction)
{
   if (IsFrozen())
      Thaw();
   
   SetGLContext();
   
   // reset the error flag (since the user could fix something then rerun
   mFatalErrorFound = false;
   
   mTotalPoints++;
   mInFunction = inFunction;   
   mDrawSolverData = false;
   mIsSolving = solving;
   mScCount = scNames.size();
   mScNameArray = scNames;
   UnsignedIntArray scColors;
   
   // Build spacecraft colors
   for (int i = 0; i < mScCount; i++)
   {
      //@todo If color not found, throw an exception?
      UnsignedInt intColor = 0XFFFFFF; // Temporarily set to white
      if (solving)
         intColor = targetColorMap.at(scNames[i]);
      else
         intColor = orbitColorMap.at(scNames[i]);
      scColors.push_back(intColor);
   }
   
   // Set object color map
   SetObjectOrbitColors(orbitColorMap);
   SetObjectTargetColors(targetColorMap);
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("===========================================================================\n");
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdatePlot() plot='%s', time=%f, mNumData=%d, mScCount=%d, "
       "solving=%d, solverOption=%d, drawing=%d, inFunction=%d\n", GetName().WX_TO_C_STRING,
       time, mNumData, mScCount, solving, solverOption, drawing, inFunction);
   ColorMap::const_iterator iter = orbitColorMap.begin();
   #endif
   #if DEBUG_UPDATE > 1
   for (int i = 0; i < mScCount; i++)
      MessageInterface::ShowMessage
         ("   posX:%12.5f, posY:%12.5f, posZ:%12.5f, scColor:%06X\n", posX[i], posY[i],
          posZ[i], scColors[i]);
   #endif
   
   #if DEBUG_COLOR
   while (iter != orbitColorMap.end())
   {
      MessageInterface::ShowMessage
         ("orbitColorMap[%s] = %06X\n", (iter->first).c_str(), iter->second);
      iter++;
   }
   #endif
   
   
   //-----------------------------------------------------------------
   // If showing current iteration only, handle solver iteration data
   // separately here since it will be shown temporarily during the run
   //-----------------------------------------------------------------
   if (solverOption == 1)
      UpdateSolverData(scNames, posX, posY, posZ, scColors, solving);
   
   // If drawing solver's current iteration and no run data has been
   // buffered up, save up to 2 points so that it will still go through
   // view projection transformation to show other objects.
   if (solverOption == 1 && solving && mNumData > 1)
      return;
   
   if (mNumData < maxData)
      mNumData++;
   
   #if 0
   if (mScCount > GmatPlot::MAX_SCS)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** The number of spacecraft to plot is greater than "
          "maximum of %d, so showing first %d spacrafts\n");
      mScCount = GmatPlot::MAX_SCS;
   }
   #endif
   
   //-----------------------------------------------------------------
   // Buffer data for plot
   //-----------------------------------------------------------------
   ComputeRingBufferIndex();
   mTime[mLastIndex] = time;
   mIsDrawing[mLastIndex] = drawing;
   
   // update spacecraft position
   UpdateSpacecraftData(time, posX, posY, posZ, velX, velY, velZ, scColors,
                        solverOption);
   
   // update non-spacecraft objects position
   UpdateOtherData(time);
   
   #if DEBUG_UPDATE_VIEW
   MessageInterface::ShowMessage
      ("   mViewPointInitialized=%d, mGlInitialized=%d, mUseInitialViewPoint=%d\n",
       mViewPointInitialized, mGlInitialized, mUseInitialViewPoint);
   #endif
   
   // Initialize view point if not already initialized
   // We want users to change the view point during the run,
   // so mUseInitialViewPoint is removed (LOJ: 2011.02.08)
   //if (!mViewPointInitialized || mUseInitialViewPoint)
   if (!mViewPointInitialized)
   {
      #if DEBUG_UPDATE_VIEW
      MessageInterface::ShowMessage
         ("ViewCanvas::UpdatePlot() Calling InitializeViewPoint()\n");
      #endif
      if (!mViewPointInitialized ||
          (mGlInitialized && mUseInitialViewPoint))
      {
         #if DEBUG_UPDATE_VIEW
         MessageInterface::ShowMessage("===> Initializing view\n");
         #endif
         InitializeViewPoint();
      }
      else
      {
         #if DEBUG_UPDATE_VIEW
         MessageInterface::ShowMessage("===> Using current view\n");
         #endif
      }
      
      mViewPointInitialized = true;
   }
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage("ViewCanvas::UpdatePlot() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void AddObjectList(wxArrayString &objNames, bool clearList=true)
//---------------------------------------------------------------------------
void ViewCanvas::AddObjectList(const wxArrayString &objNames, bool clearList)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::AddObjectList() '%s' entered, object count=%d, clearList=%d\n",
       mPlotName.WX_TO_C_STRING, objNames.GetCount(), clearList);
   #endif
   
   // clear bodies
   if (clearList)
   {
      mObjectNames.Empty();
   }
   
   RgbColor rgb;
   
   mObjectCount = objNames.GetCount();
   ClearObjectArrays();
   
   if (!CreateObjectArrays())
      throw SubscriberException("There is not enough memory to allocate OrbitView data\n");
   
   for (int i=0; i<mObjectCount; i++)
   {
      // add object names
      mObjectNames.Add(objNames[i]);
      mTextureIdMap[objNames[i]] = UNINIT_TEXTURE;
      
      // initialize show object
      mShowObjectMap[objNames[i]] = true;
      
      // set real object radius, if it is CelestialBody
      if (mObjectArray[i]->IsOfType(Gmat::CELESTIAL_BODY))
      {
         mObjectRadius[i] = ((CelestialBody*)(mObjectArray[i]))->GetEquatorialRadius();
         mObjMaxZoomIn[i] = mObjectRadius[i] * RADIUS_ZOOM_RATIO;
      }
      else
      {
         mObjectRadius[i] = mObjectDefaultRadius;
         mObjMaxZoomIn[i] = mObjectDefaultRadius * RADIUS_ZOOM_RATIO;
      }
      
      #if DEBUG_OBJECT > 1
      if (i == 0) MessageInterface::ShowMessage("   In AddObjectList()\n");
      MessageInterface::ShowMessage
         ("   mObjectNames[%2d]=%s, mObjectRadius=%f\n", i,
         mObjectNames[i].WX_TO_C_STRING, mObjectRadius[i]);
      #endif
   }
   
   ResetPlotInfo();
   ClearPlot();
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::AddObjectList() '%s' leaving\n", mPlotName.WX_TO_C_STRING);
   #endif
   
} //AddObjectList()

//-----------------------
// protected methods
//-----------------------

//------------------------------------------------------------------------------
// virtual void SetDrawingMode()
//------------------------------------------------------------------------------
void ViewCanvas::SetDrawingMode()
{
   #ifdef DEBUG_DRAWING_MODE
   MessageInterface::ShowMessage
      ("SetDrawingMode() '%s' entered, mDrawWireFrame=%d\n", mPlotName.c_str(),
       mDrawWireFrame);
   #endif
   
   // Set OpenGL to recognize the counter clockwise defined side of a polygon
   // as its 'front' for lighting and culling purposes
   glFrontFace(GL_CCW);
   
   // Disable face culling so that backwards models (like Mir) will work
   glDisable (GL_CULL_FACE);
   
   // Enable OpenGL to use glColorMaterial() to get material properties
   glEnable(GL_COLOR_MATERIAL);
   
   // Set the front face's ambient and diffuse components
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   
   // Set the ambient lighting
   GLfloat ambient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   
   // Set viewport size
   int nWidth, nHeight;
   GetClientSize(&nWidth, &nHeight);
   glViewport(0, 0, nWidth, nHeight);
   
   // Set plygon drawng mode
   if (mDrawWireFrame)
   {
      glPolygonMode(GL_FRONT, GL_LINE);
      glPolygonMode(GL_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT, GL_FILL);
      glPolygonMode(GL_BACK, GL_FILL);
   }
   
   #ifdef DEBUG_DRAWING_MODE
   MessageInterface::ShowMessage("SetDrawingMode() '%s' leaving\n", mPlotName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandleLightSource()
//------------------------------------------------------------------------------
void ViewCanvas::HandleLightSource()
{
   // do nothing here
}


//------------------------------------------------------------------------------
// void ResetPlotInfo()
//------------------------------------------------------------------------------
/**
 * Resets plotting information.
 */
//------------------------------------------------------------------------------
void ViewCanvas::ResetPlotInfo()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("ViewCanvas::ResetPlotInfo() entered for %s\n",
   mPlotName.WX_TO_C_STRING);
   #endif
   
   mCurrIndex = -1;
   mNumData = 0;
   mTotalPoints = 0;
   
   mBeginIndex1 = 0;
   mBeginIndex2 = -1;
   mEndIndex1 = -1;
   mEndIndex2 = -1;
   mRealBeginIndex1 = 0;
   mRealBeginIndex2 = -1;
   mRealEndIndex1 = -1;
   mRealEndIndex2 = -1;
   mLastIndex = 0;
   
   mOverCounter = 0;
   mIsEndOfData = false;
   mIsEndOfRun = false;
   mWriteWarning = true;
   mInFunction = false;
   
   mWriteRepaintDisalbedInfo = true;
   mSatModelsAreLoaded = false;
   mOtherModelsAreLoaded = false;
   
   // Initialize view
   if (mUseInitialViewPoint)
   {
      #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("   Calling SetDefaultView()\n");
      #endif
      SetDefaultView();
   }
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("ViewCanvas::ResetPlotInfo() leaving\n");
   #endif
}

//---------------------------------------------------------------------------
// int GetObjectId(const wxString &name)
//---------------------------------------------------------------------------
int ViewCanvas::GetObjectId(const wxString &name)
{
   for (int i=0; i<mObjectCount; i++)
      if (mObjectNames[i] == name)
         return i;
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::GetObjectId() obj name: " + name +
       " not found in the object list\n");
   #endif
   
   return UNKNOWN_OBJ_ID;
}


//------------------------------------------------------------------------------
// void ClearObjectArrays(bool deleteArrays = true)
//------------------------------------------------------------------------------
void ViewCanvas::ClearObjectArrays(bool deleteArrays)
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("ViewCanvas::ClearObjectArrays() entered\n");
   #endif

   if (deleteArrays)
   {
      if (mTime)
         delete [] mTime;
      
      if (mIsDrawing)
         delete [] mIsDrawing;
      
      if (mObjectRadius)
         delete [] mObjectRadius;
      
      if (mObjMaxZoomIn)
         delete [] mObjMaxZoomIn;
      
      if (mObjLastFrame)
         delete [] mObjLastFrame;
      
      if (mDrawOrbitFlag)
         delete [] mDrawOrbitFlag;
      
      if (mObjectOrbitColor)
         delete [] mObjectOrbitColor;
      
      if (mObjectGciPos)
         delete [] mObjectGciPos;
      
      if (mObjectViewPos)
         delete [] mObjectViewPos;
      
      if (mObjectViewVel)
         delete [] mObjectViewVel;
      
      if (mObjectQuat)
         delete [] mObjectQuat;
      
      if (mBodyRotAngle)
         delete [] mBodyRotAngle;

      if (mBodyRotAxis)
         delete [] mBodyRotAxis;
      
      if (mRotMatViewToInternal)
         delete [] mRotMatViewToInternal;
      
      if (mRotMatViewToEcliptic)
         delete [] mRotMatViewToEcliptic;
   }
   
   mTime = NULL;
   mIsDrawing = NULL;
   mObjectRadius = NULL;
   mObjMaxZoomIn = NULL;
   mObjLastFrame = NULL;
   mDrawOrbitFlag = NULL;
   mObjectOrbitColor = NULL;
   mObjectGciPos = NULL;
   mObjectViewPos = NULL;
   mObjectViewVel = NULL;
   mObjectQuat = NULL;
   mBodyRotAngle = NULL;
   mBodyRotAxis = NULL;
   mRotMatViewToInternal = NULL;
   mRotMatViewToEcliptic = NULL;
      
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("ViewCanvas::ClearObjectArrays() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool CreateObjectArrays()
//------------------------------------------------------------------------------
/*
 * Allocates arrays for objects.
 */
//------------------------------------------------------------------------------
bool ViewCanvas::CreateObjectArrays()
{
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::CreateObjectArrays() allocating object arrays with %d\n",
       mObjectCount);
   #endif
   
   if ((mTime = new Real[maxData]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mTime, so returning false\n");
      #endif
      return false;
   }
   
   if ((mIsDrawing = new bool[maxData]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mIsDrawing, so returning false\n");
      #endif
      return false;
   }
   
   if ((mObjectRadius = new Real[mObjectCount]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectRadius, so returning false\n");
      #endif
      return false;
   }
   
   for (int i=0; i<mObjectCount; i++)
      mObjectRadius[i] = 0.0;
   
   if ((mObjMaxZoomIn = new Real[mObjectCount]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjMaxZoomIn, so returning false\n");
      #endif
      return false;
   }
   
   for (int i=0; i<mObjectCount; i++)
      mObjMaxZoomIn[i] = 0.0;
   
   if ((mObjLastFrame = new int[mObjectCount]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjLastFrame, so returning false\n");
      #endif
      return false;
   }
   
   if ((mDrawOrbitFlag = new bool[mObjectCount*maxData]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mDrawOrbitFlag, so returning false\n");
      #endif
      return false;
   }
   
   if ((mObjectOrbitColor = new UnsignedInt[mObjectCount*maxData]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectOrbitColor, so returning false\n");
      #endif
      return false;
   }
   
   if ((mObjectGciPos = new Real[mObjectCount*maxData*3]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectGciPos, so returning false\n");
      #endif
      return false;
   }
   
   if ((mObjectViewPos = new Real[mObjectCount*maxData*3]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectViewPos, so returning false\n");
      #endif
      return false;
   }
   
   if (mNeedVelocity)
      if ((mObjectViewVel = new Real[mObjectCount*maxData*3]) == NULL)
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectViewVel, so returning false\n");
         #endif
         return false;
      }
   
   if (mNeedAttitude)
   {
      if ((mObjectQuat = new Real[mObjectCount*maxData*4]) == NULL)
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mObjectQuat, so returning false\n");
         #endif
         return false;
      }
      
      if ((mBodyRotAngle = new Real[mObjectCount*maxData]) == NULL)
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mBodyRotAngle, so returning false\n");
         #endif
         return false;
      }
      
      if ((mBodyRotAxis = new Real[mObjectCount*maxData*3]) == NULL)
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mBodyRotAxis, so returning false\n");
         #endif
         return false;
      }
   }
   
   if ((mRotMatViewToInternal = new Real[maxData*16]) == NULL)
   {
      #if DEBUG_OBJECT
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mRotMatViewToInternal, so returning false\n");
      #endif
      return false;
   }
   
   if (mNeedEcliptic)
   {
      if ((mRotMatViewToEcliptic = new Real[maxData*16]) == NULL)
      {
         #if DEBUG_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::CreateObjectArrays() cannot allocate enough memory for mRotMatViewToEcliptic, so returning false\n");
         #endif
         return false;
      }
   }
   
   #if DEBUG_OBJECT
   MessageInterface::ShowMessage("ViewCanvas::CreateObjectArrays() exiting\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void ComputeRingBufferIndex(bool writeWarning)
//------------------------------------------------------------------------------
/**
 * Computes begin and end index of the data in the ring buffer. When it is filled
 * with maxData points, the begin index goes back to 0.
 */
//------------------------------------------------------------------------------
void ViewCanvas::ComputeRingBufferIndex()
{
   mCurrIndex++;
   
   if (mCurrIndex < maxData)
   {
      mEndIndex1 = mNumData - 1;
      if (mEndIndex2 != -1)
      {
         mBeginIndex1++;
         if (mBeginIndex1 + 1 > maxData)
            mBeginIndex1 = 0;
         
         mEndIndex2++;
         if (mEndIndex2 + 1 > maxData)
            mEndIndex2 = 0;
      }
   }
   else
   {
      // Write buffer maxed out message only once
      if (mWriteWarning)
      {
         Real toMjd = -999;
         std::string utcGregorian;
         TimeSystemConverter::Instance()->Convert("A1ModJulian", mTime[mCurrIndex-1], "",
                                    "UTCGregorian", toMjd, utcGregorian, 1);
         MessageInterface::ShowMessage
            ("*** WARNING *** %s: '%s' exceed the maximum data points, now "
             "showing %d most recent data points.\n", utcGregorian.c_str(),
             mPlotName.WX_TO_C_STRING, maxData);
         mWriteWarning = false;
      }
      
      mBeginIndex1++;
      if (mBeginIndex1 + 1 > maxData)
         mBeginIndex1 = 0;
      
      mEndIndex1 = maxData - 1;
      
      mBeginIndex2 = 0;
      mEndIndex2++;
      if (mEndIndex2 + 1 > maxData)
         mEndIndex2 = 0;
      mCurrIndex = 0;
   }
   
   // Compute last buffer index
   mLastIndex = mEndIndex1;
   if (mEndIndex2 != -1)
      mLastIndex = mEndIndex2;
   
   #ifdef DEBUG_DATA_BUFFERRING
   MessageInterface::ShowMessage
      ("===> mTotalPoints=%4d, mNumData=%3d, mCurrIndex=%3d, mLastIndex=%3d\n   "
       "mBeginIndex1=%3d, mEndIndex1=%3d, mBeginIndex2=%3d, mEndIndex2=%3d\n",
       mTotalPoints, mNumData, mCurrIndex, mLastIndex, mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   #endif
   
}


//------------------------------------------------------------------------------
// void ComputeActualIndex()
//------------------------------------------------------------------------------
void ViewCanvas::ComputeActualIndex()
{
   #ifdef DEBUG_COMPUTE_ACTUAL_INDEX
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeActualIndex() mRedrawLastPointsOnly=%d, mIsEndOfRun=%d\n"
       "   mBeginIndex1=%d, mEndIndex1=%d, mBeginIndex2=%d, mEndIndex2=%d\n",
       mRedrawLastPointsOnly, mIsEndOfRun, mBeginIndex1, mEndIndex1, mBeginIndex2,
       mEndIndex2);
   #endif
   
   mRealBeginIndex1 = mBeginIndex1;
   mRealEndIndex1   = mEndIndex1;
   mRealBeginIndex2 = mBeginIndex2;
   mRealEndIndex2   = mEndIndex2;
   
   // if re-drawing last few points only
   if (mRedrawLastPointsOnly && !mIsEndOfRun)
   {
      // if ring buffer not over run
      if (mEndIndex2 == -1)
      {
         mRealBeginIndex1 = mEndIndex1 - mNumPointsToRedraw;
         if (mRealBeginIndex1 < 0)
            mRealBeginIndex1 = 0;
      }
      else
      {
         mRealBeginIndex1 = mEndIndex2 - mNumPointsToRedraw;
         if (mRealBeginIndex1 >= 0)
         {
            mRealEndIndex1 = mEndIndex2;
            mRealBeginIndex2 = -1;
            mRealEndIndex2 = -1;
         }
         else
         {
            mRealBeginIndex1 = maxData + mRealBeginIndex1;
            mRealEndIndex1 = maxData - 1;
            mRealBeginIndex2 = 0;
            mRealEndIndex2 = mEndIndex2;
         }
      }
   }
   
   #ifdef DEBUG_COMPUTE_ACTUAL_INDEX
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeActualIndex()     mBeginIndex1=%3d,     mEndIndex1=%3d, "
       "    mBeginIndex2=%3d,     mEndIndex2=%3d\n", mBeginIndex1, mEndIndex1,
       mBeginIndex2, mEndIndex2);
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeActualIndex() mRealBeginIndex1=%3d, mRealEndIndex1=%3d, "
       "mRealBeginIndex2=%3d, mRealEndIndex2=%3d\n", mRealBeginIndex1, mRealEndIndex1,
       mRealBeginIndex2,  mRealEndIndex2);
   #endif
}


//------------------------------------------------------------------------------
// virtual bool LoadBodyTextures()
//------------------------------------------------------------------------------
bool ViewCanvas::LoadBodyTextures()
{
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadBodyTextures() '%s' entered, mObjectCount=%d\n",
       mPlotName.WX_TO_C_STRING, mObjectCount);
   #endif
   
   //--------------------------------------------------
   // load object texture if used
   //--------------------------------------------------
   for (int i=0; i<mObjectCount; i++)
   {
      // Spacecraft is using icon for showing image on the plot
      // so texture mapping is needed (LOJ: 2011.12.10)
		// We are not using texture for spacecraft so skip
      //if (mObjectArray[i]->IsOfType(Gmat::SPACECRAFT))
      //   continue;
      
      #if DEBUG_TEXTURE
		MessageInterface::ShowMessage
			("   object = '%s', map id = %d\n", mObjectNames[i].WX_TO_C_STRING,
			 mTextureIdMap[mObjectNames[i]]);
      #endif
		
      if (mTextureIdMap[mObjectNames[i]] == UNINIT_TEXTURE)
      {
         #if DEBUG_TEXTURE > 1
         MessageInterface::ShowMessage
            ("ViewCanvas::LoadBodyTextures() object=<%p>'%s'\n",
             mObjectArray[i], mObjectNames[i].WX_TO_C_STRING);
         #endif
         
         mTextureIdMap[mObjectNames[i]] =
            BindTexture(mObjectArray[i], mObjectNames[i]);
      }
   }
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadBodyTextures() '%s' leaving, mObjectCount=%d\n",
       mPlotName.WX_TO_C_STRING, mObjectCount);
   #endif
   
   return true;   
}


//------------------------------------------------------------------------------
// virtual GLuint BindTexture(SpacePoint *obj, const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Binds texture map for the body. It uses texture file set for this plot.
 * If it was not set then it uses texture file from the body.
 */
//------------------------------------------------------------------------------
GLuint ViewCanvas::BindTexture(SpacePoint *obj, const wxString &objName)
{
   #ifdef __IGNORE_PNG_WARNING__
   wxLogLevel logLevel = wxLog::GetLogLevel();
   wxLog::SetLogLevel(0);
   #endif
   
   GLuint texId = UNINIT_TEXTURE;
   std::string textureFile;
   
   // If texture file specified then use it
   if (mTextureFileMap.find(objName) != mTextureFileMap.end())
      textureFile = mTextureFileMap[objName];
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::BindTexture() '%s' entered, objName='%s'\n   textureFile = '%s'\n",
       mPlotName.WX_TO_C_STRING, objName.WX_TO_C_STRING, textureFile.c_str());
   #endif
   
   try
   {
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
      {
         // Use texture map set by user for this plot, if not set use from the celestial body
         if (textureFile == "" || !GmatFileUtil::DoesFileExist(textureFile))
         {
            std::string oldTextureFile = textureFile;
            CelestialBody *body = (CelestialBody*) obj;
            textureFile = body->GetStringParameter(body->GetParameterID("TextureMapFullPath"));
            #if DEBUG_TEXTURE
            MessageInterface::ShowMessage
               ("   texture fullpath from the body = '%s'\n", textureFile.c_str());
            #endif
            if (oldTextureFile != "")
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** The texture file '%s' does not exist, \n"
                   "    so using the texture file '%s' from the body '%s'.\n",
                   oldTextureFile.c_str(), textureFile.c_str(), body->GetName().c_str());
            }
            
            FileManager *fm = FileManager::Instance();
            // Changed to use FileManager::FindPath() (LOJ: 2014.07.08)
            std::string textureLoc = fm->FindPath(textureFile, "TEXTURE_PATH", true, false, false);
            if (textureLoc == "")
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** The texture file '%s' does not exist\n", textureFile.c_str());
            }
            
            // // If texture file does not exist, try with default path from the startup file
            // if (!GmatFileUtil::DoesFileExist(textureFile.c_str()))
            // {
            //    oldTextureFile = textureFile;
            //    FileManager *fm = FileManager::Instance();
            //    std::string textureLoc = fm->GetFullPathname("TEXTURE_PATH");
            //    textureFile = textureLoc + textureFile;
            //    if (oldTextureFile != "")
            //    {
            //       MessageInterface::ShowMessage
            //          ("*** WARNING *** The texture file '%s' does not exist, \n"
            //           "    so using the file '%s' \n    from the path specified in the startup file.\n",
            //           oldTextureFile.c_str(), textureFile.c_str(), body->GetName().c_str());
            //    }
            // }
         }
      }
      else if (obj->IsOfType(Gmat::SPACECRAFT) ||
               obj->IsOfType(Gmat::GROUND_STATION))
      {
         FileManager *fm = FileManager::Instance();
         std::string iconLoc = fm->GetFullPathname("ICON_PATH");
         
         #ifdef DEBUG_TEXTURE
         MessageInterface::ShowMessage("   iconLoc = '%s'\n", iconLoc.c_str());
         #endif
         
         // Check if icon file directory exist
         if (GmatFileUtil::DoesDirectoryExist(iconLoc.c_str(), false))
         {
            if (obj->IsOfType(Gmat::SPACECRAFT))
               textureFile = iconLoc + "Spacecraft.png";
            else
               textureFile = iconLoc + "rt_GroundStation.png";
         }
      }
      
      #if DEBUG_TEXTURE
      MessageInterface::ShowMessage
         ("   theContext=<%p>, textureFile='%s'\n", theContext, textureFile.c_str());
      #endif
      
      if (!SetGLContext("in ViewCanvas::BindTexture()"))
         return -1;
      
      // Generate text id and bind it before loading image
      glGenTextures(1, &texId);
      glBindTexture(GL_TEXTURE_2D, texId);
      
      #if DEBUG_TEXTURE
      GLenum error = glGetError();
      MessageInterface::ShowMessage
         ("   texId = %3d, error = %d, GL_INVALID_VALUE = %d, UNINIT_TEXTURE = %d\n",
          texId, error, GL_INVALID_VALUE, UNINIT_TEXTURE);
      #endif
      
      // Save image data if object is spacecraft or ground station
      int objUsingIcon = -99;
      if (obj->IsOfType(Gmat::SPACECRAFT))
         objUsingIcon = 1;
      else if (obj->IsOfType(Gmat::GROUND_STATION))
         objUsingIcon = 2;
      
      if (!LoadImage(textureFile, objUsingIcon))
      {
         if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** ViewCanvas::BindTexture() Cannot load texture "
                "image for '%s' from '%s'\n", objName.WX_TO_C_STRING, textureFile.c_str());
         }
         texId = UNINIT_TEXTURE;
      }
   }
   catch (BaseException &e)
   {
      // Give warning for missing texture file for only CelestialBody object
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** ViewCanvas::BindTexture() Cannot bind texture "
             "image for %s.\n%s\n", objName.WX_TO_C_STRING, e.GetFullMessage().c_str());
      }
   }
   
   #if DEBUG_TEXTURE
   MessageInterface::ShowMessage
      ("ViewCanvas::BindTexture() '%s' leaving, objName='%s', texId=%d\n",
		 mPlotName.WX_TO_C_STRING, objName.WX_TO_C_STRING, texId);
   #endif
   
   #ifdef __IGNORE_PNG_WARNING__
   wxLog::SetLogLevel(logLevel);
   #endif
   
   return texId;
}


//---------------------------------------------------------------------------
// bool LoadImage(const std::string &fileName, int objUsingIcon)
//---------------------------------------------------------------------------
/**
 * Loads image from a file.
 *
 * @param  fileName  The name of image file
 * @objUsingIcon  The id of the object using icon
 *                   1 = spacecraft, 2 = ground station)
 */
//---------------------------------------------------------------------------
bool ViewCanvas::LoadImage(const std::string &fileName, int objUsingIcon)
{
   #if DEBUG_LOAD_IMAGE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadImage() '%s' entered\n   file='%s', objUsingIcon=%d\n",
       mPlotName.WX_TO_C_STRING, fileName.c_str(), objUsingIcon);
   #endif
   
   if (fileName == "")
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() blank filename, so returning false\n");
      #endif
      return false;
   }
   
   wxImage image = wxImage(fileName.c_str());
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() empty data, so returning false\n");
      #endif
      return false;
   }
   
   #if DEBUG_LOAD_IMAGE
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d, hasAlpha=%d\n", width, height, size,
       image.HasAlpha());
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   glEnable(GL_TEXTURE_2D);
   
   //=======================================================
   // Do we want to use mipmaps?
   // When you desire high-quality texture mapping, you will typically specify a
   // mipmapped texture filter. Mipmapping lets you specify multiple levels of
   // detail for a texture image. Each level of detail is half the size of the
   // previous level of detail in each dimension. So if your initial texture
   // image is an image of size 32x32, the lower levels of detail will be of
   // size 16x16, 8x8, 4x4, 2x2, and 1x1. Typically, you use the gluBuild2DMipmaps
   // routine to automatically construct the lower levels of details from you
   // original image. This routine re-samples the original image at each level
   // of detail so that the image is available at each of the various smaller sizes.
   //=======================================================================
   #ifdef __ENABLE_MIPMAPS__
   //=======================================================================
   
   //used for min and magnifying texture
   int mipmapsStatus = 0;
   
   //pass image to opengl
   
   //==============================================
   // gluBuild2DMipmaps() crashes on Linux
   #ifndef __WXGTK__
   //==============================================
   
   // If small icon, set background color alpha value to 0 transparent
   // so that it will not be shown when drawing.
   if (width <= 16 && height <= 16)
   {
      mipmapsStatus = AddAlphaToTexture(mirror, objUsingIcon, true);
   }
   else
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      
      // This call crashes GMAT on Linux, so it is excluded here. 
      mipmapsStatus =
         gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                           GL_UNSIGNED_BYTE, data1);
   }
   
   //==============================================
   #else
   //==============================================
   
   if (width <= 16 && height <= 16)
   {
      mipmapsStatus = AddAlphaToTexture(mirror, objUsingIcon, false);
   }
   else
   {
      // Try this on Linux
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data1);
   }
   
   //==============================================
   #endif
   //==============================================
   
   if (mipmapsStatus == 0)
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() '%s' returning true, mipmapsStatus=%d\n",
          mPlotName.WX_TO_C_STRING, mipmapsStatus);
      #endif
      
      return true;
   }
   else
   {
      #if DEBUG_LOAD_IMAGE
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadImage() '%s' returning false, mipmapsStatus=%d\n",
          mPlotName.WX_TO_C_STRING, mipmapsStatus);
      #endif
      return false;
   }
   
   //=======================================================================
   // Not using mipmaps
   #else
   //=======================================================================
   
   //used for min and magnifying texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                GL_UNSIGNED_BYTE, data1);
   
   #if DEBUG_LOAD_IMAGE
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadImage() '%s' returning true, using glTexImage2D\d\n",
       mPlotName.c_str());
   #endif
   
   return true;
   
   //=======================================================================
   #endif
   //=======================================================================
}


//------------------------------------------------------------------------------
// int AddAlphaToTexture(const wxImage &image, int objUsingIcon, bool useMipmaps)
//------------------------------------------------------------------------------
/**
 * Adds alpha value to texture image
 */
//------------------------------------------------------------------------------
int ViewCanvas::AddAlphaToTexture(const wxImage &image, int objUsingIcon,
                                  bool useMipmaps)
{
   int width = image.GetWidth();
   int height = image.GetHeight();
   unsigned char red, green, blue;
   int status = 0;
   
   // Create new texture image to be used with glTexImage2
   // Use GlRgbColorType to fix GMT-4337 (LOJ: 2014.03.07)
   //GlColorType *tex32 = new GlColorType[width * height];
   GlRgbColorType *tex32 = new GlRgbColorType[width * height];
   
   for (int x = 0; x < width; x++)
   {
      for (int y = 0; y < height; y++)
      {
         red = image.GetRed(x, y);
         green = image.GetGreen(x, y);
         blue = image.GetBlue(x, y);
         
         #ifdef DEBUG_ADD_ALPHA
         MessageInterface::ShowMessage
            ("   image(%2d,%2d), red=%3d, green=%3d, blue=%3d\n",
             x, y, red, green, blue);
         #endif
         
         int index = y * width + x;
         tex32[index].red = red;
         tex32[index].green = green;
         tex32[index].blue = blue;
         
         // Assumes white background for now
         if (red == 255 && green == 255 && blue == 255)            
            tex32[index].alpha = 0;
         else
            tex32[index].alpha = 255;
      }
   }
   
   if (useMipmaps)
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      
      // Set internal format and data type to GL_RGBA
      status =
         gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA,
                           GL_UNSIGNED_BYTE, tex32);
   }
   else
   {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, tex32);
   }
   
   if (objUsingIcon == 1)
      mScImage = (GLubyte*)tex32;
   else if (objUsingIcon == 2)
      mGsImage = (GLubyte*)tex32;
   else
      delete[] tex32;
   
   return status;
}


//------------------------------------------------------------------------------
// virtual bool LoadSpacecraftModels(bool writeWarning)
//------------------------------------------------------------------------------
/**
 * Loads spacecraft model specified in the spacecraft object.
 *
 * @param  writeWarning  If true, writes warning if failed to load models
 */
//------------------------------------------------------------------------------
bool ViewCanvas::LoadSpacecraftModels(bool writeWarning)
{
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadSpacecraftModels() '%s' entered, writeWarning = %d, mGlInitialized = %d, "
       "mSatModelsAreLoaded = %d, mScCount = %d\n", mPlotName.WX_TO_C_STRING, writeWarning,
       mGlInitialized, mSatModelsAreLoaded, mScCount);
   #endif
   
   // Add this here to see if it works on Linux loading spacecraft 3ds model (LOJ: 2012.08.01)
   wxPaintDC dc(this);
   if (!SetGLContext("in ViewCanvas::LoadSpacecraftModels()"))
   {
      #if DEBUG_LOAD_MODEL
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadSpacecraftModels() '%s' returning false, SetGLContext() failed\n");
      #endif
      return false;
   }
   
   if (mGlInitialized)
   {
      if (!mSatModelsAreLoaded)
      {
         ModelManager *mm = ModelManager::Instance();
			
			int numModelLoaded = 0;
         for (int sc = 0;  sc < mScCount; sc++)
         {
				std::string satName = mScNameArray[sc];
            int satId = GetObjectId(satName.c_str());
				
            #ifdef DEBUG_LOAD_MODEL
				MessageInterface::ShowMessage
					("   satName = '%s', satId = %d\n", satName.c_str(), satId);
				#endif
				
				if (satId == UNKNOWN_OBJ_ID)
				{
               #ifdef DEBUG_LOAD_MODEL
					MessageInterface::ShowMessage("   sat id is UNKNOWN so skipping\n");
				   #endif
				}
				else
            {
               Spacecraft *sat = (Spacecraft*)mObjectArray[satId];
					if (sat == NULL)
					{
                  #ifdef DEBUG_LOAD_MODEL
						MessageInterface::ShowMessage("   spacecraft pointer is NULL so skipping\n");
						#endif
					}
					else
					{
                  std::string modelFullPath = sat->GetModelFileFullPath();
                  #ifdef DEBUG_LOAD_MODEL
						MessageInterface::ShowMessage
							("   Loading model file from the spacecraft <%p>'%s'\n   modelFile = '%s', "
							 "modelID = %d\n",  sat, sat->GetName().c_str(), modelFullPath.c_str(),
							 sat->GetModelId());
				      #endif
                  
                  // If fullpaht is blank, try with model file name
                  if (modelFullPath == "")
                  {
                     modelFullPath = sat->GetModelFile();
                  }
                  
						if (modelFullPath != "" && sat->GetModelId() == -1)
						{
							if (GmatFileUtil::DoesFileExist(modelFullPath))
							{                        
                        int modelId = mm->LoadModel(modelFullPath);
                        #ifdef DEBUG_LOAD_MODEL
								MessageInterface::ShowMessage
                           ("   modelId %d returned from mm->LoadModel(), mm=<%p>\n", modelId, mm);
                        #endif
                        
                        sat->SetModelId(modelId);
                        numModelLoaded++;
                        
                        #ifdef DEBUG_LOAD_MODEL
								MessageInterface::ShowMessage
									("   Successfully loaded model '%s', numModelLoaded = %d\n", modelFullPath.c_str(),
                            numModelLoaded);
                        #endif
							}
							else
							{
                        if (writeWarning)
                        {
                           MessageInterface::ShowMessage
                              ("*** WARNING *** Cannot load the model file for spacecraft '%s'.\n    "
                               "The file '%s' does not exist.\n", sat->GetName().c_str(),
                               modelFullPath.c_str());
                        }
							}
						}
               }
				}
			}
			
         #ifdef DEBUG_LOAD_MODEL
			MessageInterface::ShowMessage
            ("   numModelLoaded = %d, mScCount = %d\n", numModelLoaded, mScCount);
			#endif
         
			// Set mSatModelsAreLoaded to true if it went through all models
			if (numModelLoaded == mScCount)
				mSatModelsAreLoaded = true;
      }
   }
   
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadSpacecraftModels() '%s' leaving, mGlInitialized = %d, "
       "mSatModelsAreLoaded = %d\n", mPlotName.WX_TO_C_STRING, mGlInitialized,
       mSatModelsAreLoaded);
   #endif
   
   return mSatModelsAreLoaded;
}


//------------------------------------------------------------------------------
// virtual bool LoadOtherObjectModels(bool writeWarning)
//------------------------------------------------------------------------------
/**
 * Loads celestial body model specified in the body object.
 *
 * @param  writeWarning  If true, writes warning if failed to load models
 */
//------------------------------------------------------------------------------
bool ViewCanvas::LoadOtherObjectModels(bool writeWarning)
{
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadOtherObjectModels() '%s' entered, writeWarning = %d, "
       "mGlInitialized = %d, mOtherModelsLoaded = %d\n", mPlotName.WX_TO_C_STRING,
       writeWarning, mGlInitialized, mOtherModelsAreLoaded);
   #endif
   
   // Add this here to see if it works on Linux loading spacecraft 3ds model (LOJ: 2012.08.01)
   wxPaintDC dc(this);
   if (!SetGLContext("in ViewCanvas::LoadOtherObjectModels()"))
   {
      #if DEBUG_LOAD_MODEL
      MessageInterface::ShowMessage
         ("ViewCanvas::LoadOtherObjectModels() '%s' returning false, SetGLContext() failed\n");
      #endif
      return false;
   }
   
   if (mGlInitialized)
   {
      if (!mOtherModelsAreLoaded)
      {
         ModelManager *mm = ModelManager::Instance();
			
			int numModelLoaded = 0;
         
         for (int obj = 0; obj < mObjectCount; obj++)
         {
            CelestialBody *body = (CelestialBody*)mObjectArray[obj];
            
            // If object pointer is not NULL and is a CelestialBody, check for 3D model
            if (mObjectArray[obj] != NULL && mObjectArray[obj]->IsOfType(Gmat::CELESTIAL_BODY))
            {
               CelestialBody *body = (CelestialBody*)mObjectArray[obj];
               
               std::string modelFullPath = body->Get3dViewModelFileFullPath();
               #ifdef DEBUG_LOAD_MODEL
               MessageInterface::ShowMessage
                  ("   Loading model file from the body <%p>'%s'\n   modelFile = '%s', "
                   "modelID = %d\n",  body, body->GetName().c_str(), modelFullPath.c_str(),
                   body->Get3dViewModelId());
				   #endif
               
               // If model file is not empty and model has not already loaded
               if (modelFullPath != "" && body->Get3dViewModelId() == -1)
               {
                  if (GmatFileUtil::DoesFileExist(modelFullPath))
                  {                        
                     int modelId = mm->LoadModel(modelFullPath);
                     #ifdef DEBUG_LOAD_MODEL
                     MessageInterface::ShowMessage
                        ("   modelId %d returned from mm->LoadModel(), mm=<%p>\n", modelId, mm);
                     #endif
                     
                     body->Set3dViewModelId(modelId);
                     numModelLoaded++;
                     
                     #ifdef DEBUG_LOAD_MODEL
                     MessageInterface::ShowMessage
                        ("   Successfully loaded model '%s', numModelLoaded = %d\n", modelFullPath.c_str(),
                         numModelLoaded);
                     #endif
                  }
                  else
                  {
                     if (writeWarning)
                     {
                        MessageInterface::ShowMessage
                           ("*** WARNING *** Cannot load the model file for spacecraft '%s'.\n    "
                            "The file '%s' does not exist.\n", body->GetName().c_str(),
                            modelFullPath.c_str());
                     }
                  }
               }
            }
         }
         
			// Set mOtherModelsAreLoaded to true since some bodies do not have models
         mOtherModelsAreLoaded = true;
      }
   }
   
   #if DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadOtherObjectModels() '%s' leaving, mGlInitialized = %d, "
       "mOtherModelsAreLoaded = %d\n", mPlotName.WX_TO_C_STRING, mGlInitialized,
       mOtherModelsAreLoaded);
   #endif
   
   return mOtherModelsAreLoaded;
}


//------------------------------------------------------------------------------
//  void SetupProjection()
//------------------------------------------------------------------------------
/**
 * Sets up how object is viewed
 */
//------------------------------------------------------------------------------
void ViewCanvas::SetupProjection()
{
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("ViewCanvas::SetupProjection() entered\n");
   #endif
   
   // Setup the world view
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glLoadIdentity();            // clear all previous information
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();            // clear all previous information
   
   #if DEBUG_PROJECTION > 2
   MessageInterface::ShowMessage("ViewCanvas::SetupProjection() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateSolverData(const StringArray &scNames, const RealArray posX, ...)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateSolverData(const StringArray &scNames, const RealArray &posX,
                       const RealArray &posY, const RealArray &posZ,
                       const UnsignedIntArray &scColors, bool solving)
{
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateSolverData() entered, solving=%d\n", solving);
   #endif
   
   //-----------------------------------------------------------------
   // If showing current iteration only, handle solver iteration data
   // separately here since it will be shown temporarily during the run
   //-----------------------------------------------------------------
   if (solving)
   {
      mDrawSolverData = true;
      RealArray tempSolverX, tempSolverY, tempSolverZ;
      StringArray tempScName;
      for (int sc=0; sc<mScCount; sc++)
      {
         int satId = GetObjectId(mScNameArray[sc].c_str());
         if (satId != UNKNOWN_OBJ_ID)
         {
            // if we are not drawing this spacecraft, skip
            if (!mDrawOrbitArray[satId])
               continue;
            
            tempSolverX.push_back(posX[sc]);
            tempSolverY.push_back(posY[sc]);
            tempSolverZ.push_back(posZ[sc]);
         }
      }
      
      mSolverAllPosX.push_back(tempSolverX);
      mSolverAllPosY.push_back(tempSolverY);
      mSolverAllPosZ.push_back(tempSolverZ);
      mSolverIterColorArray = scColors;
      mSolverIterScNameArray = scNames;
   }
   else
   {
      mSolverAllPosX.clear();
      mSolverAllPosY.clear();
      mSolverAllPosZ.clear();
      mSolverIterColorArray.clear();
      mSolverIterScNameArray.clear();
   }
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateSolverData() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void UpdateSpacecraftData(const RealArray &posX, const RealArray &posY, ...)
//---------------------------------------------------------------------------
void ViewCanvas::UpdateSpacecraftData(const Real &time,
                       const RealArray &posX, const RealArray &posY,
                       const RealArray &posZ, const RealArray &velX,
                       const RealArray &velY, const RealArray &velZ,
                       const UnsignedIntArray &scColors,
                       Integer solverOption)
{
   #if DEBUG_UPDATE
   static int sNumDebugOutput = 1000;
   #endif
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateSpacecraftData() entered, time=%f, mNumData=%d, mScCount=%d\n"
       "   mIsSolving=%d, solverOption=%d, mDrawSolverData=%d, mGlInitialized=%d, "
       "mSatModelsAreLoaded=%d\n", time, mNumData, mScCount, mIsSolving, solverOption,
       mDrawSolverData, mGlInitialized, mSatModelsAreLoaded);
   #endif
   
   // Load spacecraft models
   if (!mSatModelsAreLoaded)
      LoadSpacecraftModels(false);
   
   //-------------------------------------------------------
   // update spacecraft position
   //-------------------------------------------------------
   for (int sc = 0; sc < mScCount; sc++)
   {
      int satId = GetObjectId(mScNameArray[sc].c_str());
      
      #if DEBUG_UPDATE
      MessageInterface::ShowMessage
         ("   ==> sc=%d, mScCount=%d, satId=%d, scName=%s, mDrawOrbitArray[%d]=%d\n",
          sc, mScCount, satId, mObjectNames[satId].WX_TO_C_STRING, satId,
          satId != UNKNOWN_OBJ_ID ? mDrawOrbitArray[satId] : int(UNKNOWN_OBJ_ID));
      #endif
      
      if (satId != UNKNOWN_OBJ_ID)
      {
         Spacecraft *sat = (Spacecraft*)mObjectArray[satId];
         int colorIndex = satId * maxData + mLastIndex;
         #if DEBUG_UPDATE
         MessageInterface::ShowMessage
            ("   sat=<%p>'%s', mLastIndex=%d, colorIndex=%d\n", sat, sat->GetName().c_str(),
             mLastIndex, colorIndex);
         #endif
         
         if (!mDrawOrbitArray[satId])
         {
            mDrawOrbitFlag[colorIndex] = false;
            #ifdef DEBUG_UPDATE
            MessageInterface::ShowMessage("===> mDrawOrbitArray[satId] is false\n");
            #endif
            continue;
         }
         
         // Now set flags, colors, position, and velocity
         mDrawOrbitFlag[colorIndex] = true;
         
         // If drawing solver's current iteration only, we don't want to draw
         // first 3 points since these points have solver data.
         if (mIsSolving && (mDrawSolverData || (solverOption == 1 && mNumData == 2)))
         {
            #ifdef DEBUG_UPDATE
            MessageInterface::ShowMessage
               ("===> Drawing solver's current iteration only, so setting mDrawOrbitFlag to false\n");
            #endif
            mDrawOrbitFlag[colorIndex] = false;
         }
         
         mObjectOrbitColor[colorIndex] = scColors[sc];
         
         int posIndex = satId * maxData * 3 + (mLastIndex*3);
         mObjectViewPos[posIndex+0] = posX[sc];
         mObjectViewPos[posIndex+1] = posY[sc];
         mObjectViewPos[posIndex+2] = posZ[sc];
         
         // Buffer velocity if needed
         if (mNeedVelocity)
         {
            mObjectViewVel[posIndex+0] = velX[sc];
            mObjectViewVel[posIndex+1] = velY[sc];
            mObjectViewVel[posIndex+2] = velZ[sc];            
         }
         
         #if DEBUG_UPDATE
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId:%d, object=%s, colorIndex=%u, drawOrbit=%d, color=%06X\n",
                satId, mObjectNames[satId].WX_TO_C_STRING, colorIndex, mDrawOrbitFlag[colorIndex],
                mObjectOrbitColor[colorIndex]);
            MessageInterface::ShowMessage
               ("   satId:%d, posIndex=%d, gcipos = %f, %f, %f\n", satId,
                posIndex, mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
         }
         #endif
         
         // if need to convert to internal coordinate system (EarthMJ2000Eq)
         if (mViewCsIsInternalCs)
         {
            CopyVector3(&mObjectGciPos[posIndex], &mObjectViewPos[posIndex]);
         }
         else
         {
            Rvector6 satState(posX[sc], posY[sc], posZ[sc], velX[sc], velY[sc], velZ[sc]);
            Rvector6 outState;
            
            mCoordConverter.Convert(time, satState, pViewCoordSystem,
                                    outState, pInternalCoordSystem);
            
            mObjectGciPos[posIndex+0] = outState[0];
            mObjectGciPos[posIndex+1] = outState[1];
            mObjectGciPos[posIndex+2] = outState[2];                  
         }
         
         #if DEBUG_UPDATE
         if (mNumData < sNumDebugOutput)
         {
            MessageInterface::ShowMessage
               ("   satId:%d, posIndex=%d, tmppos = %f, %f, %f\n", satId, posIndex,
                mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
         }
         #endif
         
         // Update spacecraft attitude
         #if DEBUG_UPDATE
         MessageInterface::ShowMessage
            ("   Now updating attitude of spacecraft id: %d\n", satId);
         #endif
         
         if (mNeedAttitude)
            UpdateSpacecraftAttitude(time, sat, satId);
      }
   }
   
   #if DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateSpacecraftData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateOtherData(const Real &time)
//------------------------------------------------------------------------------
/**
 * Updates celetial body state by calling GetMJ2000State() with time.
 */
//------------------------------------------------------------------------------
void ViewCanvas::UpdateOtherData(const Real &time)
{
   #if DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateOtherData() entered, time = %f\n", time);
   #endif
   
   bool viewRotMatComputed = false;
   bool eclipticRotMatComputed = false;
   
   // Load celestial body models
   if (!mOtherModelsAreLoaded)
      LoadOtherObjectModels(false);
   
   //-------------------------------------------------------
   // update celestial body's position
   //-------------------------------------------------------
   for (int obj = 0; obj < mObjectCount; obj++)
   {
      SpacePoint *otherObj = mObjectArray[obj];
      
      // if object pointer is not NULL and not a spacecraft
      if (otherObj != NULL && otherObj->GetType() != Gmat::SPACECRAFT)
      {
         int objId = GetObjectId(mObjectNames[obj]);
         wxString objName = mObjectNames[objId].c_str();
         
         #if DEBUG_UPDATE_OBJECT
         MessageInterface::ShowMessage
            ("ViewCanvas::UpdateOtherData() objId=%d, objName=%s, mDrawOrbitArray[%d]=%d\n",
             objId, mObjectNames[objId].WX_TO_C_STRING, objId,
             objId != UNKNOWN_OBJ_ID ? mDrawOrbitArray[objId] : int(UNKNOWN_OBJ_ID));
         #endif
         
         // if object id found
         if (objId != UNKNOWN_OBJ_ID)
         {
            int colorIndex = objId * maxData + mLastIndex;
            
            if (!mDrawOrbitArray[objId])
               mDrawOrbitFlag[colorIndex] = false;
            else
               mDrawOrbitFlag[colorIndex] = true;
            
            #if DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage
               ("   mLastIndex=%d, colorIndex=%d, drawOrbit=%d\n",
                mLastIndex, colorIndex, mDrawOrbitFlag[colorIndex]);
            #endif
            
            //LOJ: 2013.11.25
            // Set orbit or target color
            if (mIsSolving)
               mObjectOrbitColor[colorIndex] = mObjectTargetColorMap[objName.WX_TO_STD_STRING];
            else
               mObjectOrbitColor[colorIndex] = mObjectOrbitColorMap[objName.WX_TO_STD_STRING];
            
            Rvector6 objMjEqState;
            try
            {
               objMjEqState = otherObj->GetMJ2000State(time);
            }
            catch (BaseException &)
            {
               #if DEBUG_UPDATE_OBJECT
                  MessageInterface::ShowMessage(
                        "   fatal error found in UpdateOtherData "
                        "for %s (otherObj = %s)!!!!\n",
                        mPlotName.WX_TO_C_STRING, otherObj->GetName().c_str());
               #endif
               mFatalErrorFound = true;
               throw;
            }
            
            int posIndex = objId * maxData * 3 + (mLastIndex*3);
            mObjectGciPos[posIndex+0] = objMjEqState[0];
            mObjectGciPos[posIndex+1] = objMjEqState[1];
            mObjectGciPos[posIndex+2] = objMjEqState[2];
            
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("ViewCanvas::UpdateOtherData() %s, posIndex=%d, objMjEqState=%s\n",
                mObjectNames[obj].c_str(), posIndex, objMjEqState.ToString().c_str());
            #endif
            
            // Convert objects to view CoordinateSystem if needed
            if (mViewCsIsInternalCs)
            {
               CopyVector3(&mObjectViewPos[posIndex], &mObjectGciPos[posIndex]);
            }
            else
            {
               Rvector6 outState;
               
               // Convert to view coordinate system since planets are in MJ2000Eq
               mCoordConverter.Convert(time, objMjEqState, pInternalCoordSystem,
                                       outState, pViewCoordSystem);
               
               mObjectViewPos[posIndex+0] = outState[0];
               mObjectViewPos[posIndex+1] = outState[1];
               mObjectViewPos[posIndex+2] = outState[2];
            }
            
            // Compute internal to view rotation matrix and save
            // We need to compute only once here, not for all objects
            if (!viewRotMatComputed)
            {
               ComputeRotMatForView(time, objMjEqState);               
               viewRotMatComputed = true;
            }
            
            // If drawing ecliptic plane, convert and save rotation matrix
            if (mNeedEcliptic)
            {
               // We need to compute only once here, not for all objects
               if (!eclipticRotMatComputed)
               {
                  if (pMJ2000EcCoordSystem == NULL)
                     CreateMJ2000EcCoordSystem();
                  
                  // If MJ2000Ec coordinate system is available, then compute
                  if (pMJ2000EcCoordSystem)
                     ComputeRotMatForEclipticPlane(time, objMjEqState);
                  
                  eclipticRotMatComputed = true;      
               }
            }
            
            // Update object's attitude and rotation data
            if (mNeedAttitude)
               UpdateOtherObjectAttitude(time, otherObj, objId);
            
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("    %s posIndex=%d, tmppos = %f, %f, %f\n", mObjectNames[obj].c_str(),
                posIndex, mObjectViewPos[posIndex+0], mObjectViewPos[posIndex+1],
                mObjectViewPos[posIndex+2]);
            #endif
         }
         else
         {
            #if DEBUG_UPDATE_OBJECT > 1
            MessageInterface::ShowMessage
               ("ViewCanvas::UpdateOtherData() Cannot Add data. Invalid objId=%d\n",
                objId);
            #endif
         }
      }
      else
      {
         #if DEBUG_UPDATE_OBJECT > 1
         if (mObjectArray[obj] == NULL)
         {
            MessageInterface::ShowMessage
               ("ViewCanvas::UpdateOtherData() Cannot add data. %s is NULL\n",
                mObjectNames[obj].c_str());
         }
         #endif
      }
   }
   
   #if DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("ViewCanvas::UpdateOtherData() leaving, time = %f\n", time);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId)
{
   if (sat == NULL)
      return;
   
   int attIndex = satId * maxData * 4 + (mLastIndex*4);
   
   Rmatrix33 cosMat = sat->GetAttitude(time);
   Rvector quat = AttitudeConversionUtility::ToQuaternion(cosMat);

   #ifdef DEBUG_ATTITUDE
   MessageInterface::ShowMessage("Attitude quaternion for %s: [%lf  %lf  %lf  %lf]\n", 
      sat->GetName().c_str(), quat[0], quat[1], quat[2], quat[3]);
   #endif

   mObjectQuat[attIndex+0] = quat[0];
   mObjectQuat[attIndex+1] = quat[1];
   mObjectQuat[attIndex+2] = quat[2];
   mObjectQuat[attIndex+3] = quat[3];
}


//------------------------------------------------------------------------------
// void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId)
//------------------------------------------------------------------------------
void ViewCanvas::UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId)
{
   if (sp == NULL)
      return;
   
   int attIndex = objId * maxData * 4 + (mLastIndex*4);
   wxString objName = sp->GetName().c_str();
   
   // Get attitude matrix   
   Rmatrix33 cosMat = sp->GetAttitude(time);
   Rvector quat = AttitudeConversionUtility::ToQuaternion(cosMat);
   #ifdef DEBUG_ATTITUDE
   MessageInterface::ShowMessage
      ("UpdateOtherObjectAttitude() '%s', attIndex=%d, quat=%s", 
       objName.WX_TO_C_STRING,
       attIndex, quat.ToString().c_str());
   #endif
   mObjectQuat[attIndex+0] = quat[0];
   mObjectQuat[attIndex+1] = quat[1];
   mObjectQuat[attIndex+2] = quat[2];
   mObjectQuat[attIndex+3] = quat[3];
   
   // Compute and save body rotation angle and axis
   UpdateBodyRotationData(objName, objId);
   
}


//---------------------------------------------------------------------------
// void UpdateBodyRotationData(const wxString &objName, int objId)
//---------------------------------------------------------------------------
/**
 * Computes and stores body rotation angle and axis using the following algorithm.
 *
 * @parameter  objName  The name of the body to rotate
 * @parameter  objId    The id of the body to rotate
 * @parameter  angInDeg  Computed rotation angle in degrees
 * @parameter  eAxis  Computed rotation axis
 *
 * Any object that has an attitude (spacecraft, celestial sphere, and celestial bodies)
 * needs to be oriented correctly in the  coordinate system in which the Orbit View is drawn.    
 *
 * Define the following matrices:
 * R_IP:  the rotation matrix from the coordinate system of the plot to the inertial coordinate system 
 * R_IB:  the rotation matrix from celestial body fixed to inertial, for the body to be drawn in the Orbit View
 *
 * We can calculate the rotation matrix from celestial body fixed to the plot coordinate system, R_BP, using:
 *
 * R_BP = R_IB^T*R_IP;
 *
 * R_PB defines the rotation that must be applied to the celestial body before drawing in the Orbit View.
 * I don't know what OpenGL method is being used for this so we can talk about that this afternoon.
 * We may need to convert R_PB to quaternion or Euler angles  but the most efficient way would be to
 * just pass in R_PB.
 */
 //-----------------------------------------------------------------------------
void ViewCanvas::UpdateBodyRotationData(const wxString &objName, int objId)
{
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("\nUpdateBodyRotationData() entered, '%s', objId=%d, mLastIndex=%d, epoch=%f\n",
       objName.WX_TO_C_STRING, objId, mLastIndex, mTime[mLastIndex]);
   #endif
   
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
   Real eAngle;
   Rvector3 eAxis;
   AttitudeConversionUtility::DCMToEulerAxisAndAngle(matBP, eAxis, eAngle);
   
   // Convert to degree
   Real angInDeg = GmatMathUtil::RadToDeg(eAngle, true);
   
   // Now save rotation angle ans axis for use during animation
   SaveBodyRotationData(objId, angInDeg, eAxis);
   
   #if DEBUG_ROTATE_BODY > 1
   MessageInterface::ShowMessage
      ("ComputeBodyRotationData() leaving, '%s', epoch=%f, attIndex=%d, eAngle=%f, "
       "angInDeg=%f\n   eAxis=%s\n", objName.WX_TO_C_STRING, mTime[mLastIndex],
       attIndex, eAngle, angInDeg, eAxis.ToString(12).c_str());
   #endif
}

//---------------------------------------------------------------------------
// void GetBodyRotationData(Real angInDeg, const Rvector3 &eAxis)
//---------------------------------------------------------------------------
/**
 * Retrieves body rotation angle and axis
 *
 * @parameter  angInDeg  Rotation angle in degrees
 * @parameter  eAxis  Rotation axis vector
 */
//---------------------------------------------------------------------------
void ViewCanvas::GetBodyRotationData(int objId, Real &angInDeg, Rvector3 &eAxis)
{
   int angIndex = objId * maxData + mLastIndex;
   int axisIndex = objId * maxData * 3 + (mLastIndex * 3);
   angInDeg = mBodyRotAngle[angIndex];
   eAxis(0) = mBodyRotAxis[axisIndex];
   eAxis(1) = mBodyRotAxis[axisIndex + 1];
   eAxis(2) = mBodyRotAxis[axisIndex + 2];
}


//------------------------------------------------------------------------------
// void ComputeRotMatForView(Real time, Rvector6 &state)
//------------------------------------------------------------------------------
void ViewCanvas::ComputeRotMatForView(Real time, Rvector6 &state)
{
   // state is in MJ2000Eq internal coordinate system
   Rvector6 outState;
   mCoordConverter.Convert(time, state, pInternalCoordSystem,
                           outState, pViewCoordSystem);
   Rmatrix rotMatEqToView = mCoordConverter.GetLastRotationMatrix();
   
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeRotMatForView() rotMatEqToView=\n%s",
       rotMatEqToView.ToString(12, 20).c_str());
   #endif
   
   rotMatEqToView = rotMatEqToView.Transpose();
   
   // Save rotation matrix for animation
   SaveRotMatViewToInternal(rotMatEqToView);
}


//---------------------------------------------------------------------------
// void ComputeRotMatForEclipticPlane(Real time, Rvector6 &state)
//---------------------------------------------------------------------------
/**
 * Here is math for how to rotate the ecliptic plane for OrbitView.
 * Here are some definitions first:
 * 
 * - Q means MJ2000Eq axes
 * - C means MJ2000Ec axes
 * - P means plot axes
 * - R_QC = Rotation matrix from MJ2000Ec to MJ2000Eq.  
 * - R_PQ = Rotation from MJ2000Eq to Plot coordinate axes.
 * 
 * By definition, the ecliptic plane lies in the XY plane of MJ2000Ec.
 * So, we need to determine how to rotate from MJ2000Ec to the plot system.
 * That is done like this:
 * 
 * R_PC = R_PQ * R_QC;
 *
 */
 //---------------------------------------------------------
void ViewCanvas::ComputeRotMatForEclipticPlane(Real time, Rvector6 &state)
{
   // state is in MJ2000Eq internal coordinate system, so
   // convert it from MJ2000Eq to MJ2000Ec then transpose it
   Rvector6 outState1, outState2;
   mCoordConverter.Convert(time, state, pInternalCoordSystem,
                           outState1, pMJ2000EcCoordSystem);
   Rmatrix rotMatEcToEq = mCoordConverter.GetLastRotationMatrix();
   rotMatEcToEq = rotMatEcToEq.Transpose();
   
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeRotMatForEclipticPlane() rotMatEcToEq=\n%s",
       rotMatEcToEq.ToString(12, 20).c_str());
   #endif
   
   mCoordConverter.Convert(time, outState1, pInternalCoordSystem,
                           outState2, pViewCoordSystem);
   Rmatrix rotMatEqToView = mCoordConverter.GetLastRotationMatrix();
   
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeRotMatForEclipticPlane() rotMatEqToView=\n%s",
       rotMatEqToView.ToString(12, 20).c_str());
   #endif
   
   Rmatrix rotMatEcToView = rotMatEqToView * rotMatEcToEq;
   rotMatEcToView = rotMatEcToView.Transpose();
   
   // Save rotation matrix for animation
   SaveRotMatViewToEcliptic(rotMatEcToView);
}


//---------------------------------------------------------------------------
// void SaveBodyRotationData(int objId, Real angInDeg, const Rvector3 &eAxis)
//---------------------------------------------------------------------------
/**
 * Saves body rotation angle and axis for use in animation
 *
 * @parameter  objId  Id of the body
 * @parameter  angInDeg  Rotation angle in degrees
 * @parameter  eAxis  Rotation axis vector
 */
//---------------------------------------------------------------------------
void ViewCanvas::SaveBodyRotationData(int objId, Real angInDeg, const Rvector3 &eAxis)
{
   int angIndex = objId * maxData + mLastIndex;
   int axisIndex = objId * maxData * 3 + (mLastIndex * 3);
   mBodyRotAngle[angIndex] = angInDeg;
   mBodyRotAxis[axisIndex] = eAxis(0);
   mBodyRotAxis[axisIndex + 1] = eAxis(1);
   mBodyRotAxis[axisIndex + 2] = eAxis(2);
}


//---------------------------------------------------------------------------
// void SaveRotMatViewToInternal(Rmatrix &rotMat)
//---------------------------------------------------------------------------
void ViewCanvas::SaveRotMatViewToInternal(Rmatrix &rotMat)
{
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::SaveRotMatViewToInternal() entered\n%s",
       rotMat.ToString(12, 20).c_str());
   #endif
   
   // Save internal frame to view coordiante rotation matrix
   // for drawing stars during animation
   int cIndex = mLastIndex * 16;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
         if (j < 3 && i < 3)
            mRotMatViewToInternal[cIndex+(i*4)+j] = rotMat.GetElement(i,j);
         else
            mRotMatViewToInternal[cIndex+(i*4)+j] = 0;
      }
   }
   mRotMatViewToInternal[cIndex+15] = 1;
   
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::SaveRotMatViewToInternal() leaving, cIndex = %d\n", cIndex);
   #endif
}


//---------------------------------------------------------------------------
// void SaveRotMatViewToEcliptic(Rmatrix &rotMat)
//---------------------------------------------------------------------------
void ViewCanvas::SaveRotMatViewToEcliptic(Rmatrix &rotMat)
{
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::SaveRotMatViewToEcliptic() entered\n%s",
       rotMat.ToString(12, 20).c_str());
   #endif
   
   // Save rotation matrix to draw ecliptic plane
   // during animation
   int cIndex = mLastIndex * 16;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
         if (j < 3 && i < 3)
            mRotMatViewToEcliptic[cIndex+(i*4)+j] = rotMat.GetElement(i,j);
         else
            mRotMatViewToEcliptic[cIndex+(i*4)+j] = 0;
      }
   }
   mRotMatViewToEcliptic[cIndex+15] = 1;
   
   #ifdef DEBUG_ROT_MAT
   MessageInterface::ShowMessage
      ("ViewCanvas::SaveRotMatViewToEcliptic() leaving, cIndex = %d\n", cIndex);
   #endif

}


//---------------------------------------------------------------------------
// void CreateMJ2000EcCoordSystem()
//---------------------------------------------------------------------------
void ViewCanvas::CreateMJ2000EcCoordSystem()
{
   #ifdef DEBUG_ECLIPTIC_PLANE
   MessageInterface::ShowMessage
      ("ViewCanvas::CreateMJ2000EcCoordSystem() entered, mOriginName='%s', "
       "pMJ2000EcCoordSystem=<%p>'%s'\n", mOriginName.c_str(), pMJ2000EcCoordSystem,
       pMJ2000EcCoordSystem ? pMJ2000EcCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   if (pMJ2000EcCoordSystem != NULL)
   {
      #ifdef DEBUG_ECLIPTIC_PLANE
      MessageInterface::ShowMessage
         ("ViewCanvas::CreateMJ2000EcCoordSystem() leaving, no need to create pMJ2000EcCoordSystem\n");
      #endif
      return;
   }
   
   std::string originName = GmatSolarSystemDefaults::EARTH_NAME;
   std::string axesType = "MJ2000Ec";
   std::string csName = originName + axesType;
   SpacePoint *origin = (SpacePoint*)pSolarSystem->GetBody(originName.c_str());
   SpacePoint *j2000Body = (SpacePoint*)pSolarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);
   
   // Create coordinate system with Earth origin and MJ2000Ec axis
   pMJ2000EcCoordSystem = CoordinateSystem::CreateLocalCoordinateSystem
      (csName, axesType, origin, NULL, NULL, j2000Body, pSolarSystem);
   
   #ifdef DEBUG_ECLIPTIC_PLANE
   MessageInterface::ShowMessage
      ("ViewCanvas::CreateMJ2000EcCoordSystem() leaving, pMJ2000EcCoordSystem=<%p>'%s'\n",
       pMJ2000EcCoordSystem, pMJ2000EcCoordSystem ? pMJ2000EcCoordSystem->GetName().c_str() : "NULL");
   #endif
}


//---------------------------------------------------------------------------
//  void CopyVector3(Real to[3], Real from[3])
//---------------------------------------------------------------------------
void ViewCanvas::CopyVector3(Real to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//------------------------------------------------------------------------------
// void DrawOrbit(const wxString &objName, int obj, int objId)
//------------------------------------------------------------------------------
void ViewCanvas::DrawOrbit(const wxString &objName, int obj, int objId)
{
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage
      ("==========> DrawOrbit() objName='%s', drawing first part, mRealBeginIndex1=%d, "
       "mRealEndIndex1=%d\n", objName.c_str(), mRealBeginIndex1, mRealEndIndex1);
   #endif
   
   // Draw first part from the ring buffer
   for (int i = mRealBeginIndex1 + 1; i <= mRealEndIndex1; i++)
   {
      DrawOrbitLines(i, objName, obj, objId);
   }
   
   // Draw second part from the ring buffer
   if (mEndIndex2 != -1 && mBeginIndex1 != mBeginIndex2)
   {
      #ifdef DEBUG_DRAW
      MessageInterface::ShowMessage
         ("==========> DrawOrbit() objName='%s', drawing second part\n",
          objName.c_str());
      #endif
      
      for (int i = mRealBeginIndex2 + 1; i <= mRealEndIndex2; i++)
      {
         DrawOrbitLines(i, objName, obj, objId);
      }
   }
   
   #if DEBUG_DRAW_DEBUG
   DrawDebugMessage(" Leaving DrawOrbit  --- ", GmatColor::RED32, 0, 240);
   #endif
}


//------------------------------------------------------------------------------
//  void DrawSolverData()
//------------------------------------------------------------------------------
/**
 * Draws current solver iteration data during the run.
 * This is only called when drawing "current" solver data.  For drawing all
 * solver passes at the same time, see UpdatePlot()
 */
//------------------------------------------------------------------------------
void ViewCanvas::DrawSolverData()
{
   Rvector3 start, end;
   int numPoints = mSolverAllPosX.size();
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage
      ("==========> DrawSolverData() '%s' entered, solver points = %d\n",
       mPlotName.c_str(), numPoints);
   #endif
   
   if (numPoints == 0)
      return;
   
   // Note that we're starting at 2 here rather than at 1.  There is a bug that
   // looks like a bad pointer when starting from 1 when the plot running in
   // "Current" mode.  We need to investigate this issue after the 2011a release
   // is out the door.  This TEMPORARY fix is in place so that the Mac, Linux
   // and Visual Studio builds won't crash for the "Current" setting.
   for (int i=2; i<numPoints; i++)
   {
      int numSc = mSolverAllPosX[i].size();      
      //MessageInterface::ShowMessage("==========> sc count = %d\n", numSc);
      
      //---------------------------------------------------------
      // draw lines
      //---------------------------------------------------------
      for (int sc=0; sc<numSc; sc++)
      {
         *sIntColor = mSolverIterColorArray[sc];
         wxString scName = mSolverIterScNameArray[sc].c_str();
         
         // Dunn took out old minus signs to make attitude correct.
         // Examining GMAT functionality in the debugger, this is only to show
         // the current solver iteration.  Somewhere else the multiple iterations 
         // are drawn.
         start.Set(mSolverAllPosX[i-1][sc], mSolverAllPosY[i-1][sc], mSolverAllPosZ[i-1][sc]);
         end.Set  (mSolverAllPosX[i]  [sc], mSolverAllPosY[i]  [sc], mSolverAllPosZ[i]  [sc]);
         
         // PS - See Rendering.cpp
         DrawLine(sGlColor, start, end);
         
         // Draw space object name at the last point(LOJ: 2013.09.12 GMT-3854)
         if (mShowLabels)
         {
            if (i == numPoints - 1)
            {
               glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
               DrawStringAt(scName, end);
            }
         }
      }
   }
   
   #if DEBUG_SOLVER_DATA
   MessageInterface::ShowMessage("==========> DrawSolverData() leaving\n");
   #endif
}


//---------------------------------------------------------------------------
// void DrawStatus(const wxString &label1, unsigned int textColor, ...)
//---------------------------------------------------------------------------
/*
 * Writes status at the bottom of the frame
 */
//---------------------------------------------------------------------------
void ViewCanvas::DrawStatus(const wxString &label1, unsigned int textColor,
                            const wxString &label2, double time, int xpos,
                            int ypos, bool showCS, const wxString &label3)
{   
   #ifdef DEBUG_DRAW_STATUS
   int frame = mTotalPoints;
   MessageInterface::ShowMessage
      ("DrawStatus() entered, frame=%d, time=%.9f\n", frame, time);
   #endif
   
   //----------------------------------------------------
   // draw current frame number and time
   //----------------------------------------------------
   //loj: 5/23/05 I want to use glWindowPos2f but it is available in version 1.4
   // then I'll not need to set GL_PROJECTION mode
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLfloat)mCanvasSize.x, 0.0, (GLfloat)mCanvasSize.y);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   wxString str, str1;
   wxString text;
   
   #ifdef DEBUG_DRAW_STATUS
   str.Printf("%d", frame);
   text = label1 + str;
   str1.Printf("%f - ", time);
   #endif
   
   if (label1 != "")
      text = label1;      
   
   if (time > 0)
   {
      Real toMjd = -999;
      std::string utcGregorian;
      TimeSystemConverter::Instance()->Convert("A1ModJulian", time, "", "UTCGregorian",
                                 toMjd, utcGregorian, 1);
      str = utcGregorian.c_str();
      #ifdef DEBUG_DRAW_STATUS
      MessageInterface::ShowMessage("  after convert time=%s\n", str.c_str());
      #endif
   }
   
   text = text + label2 + str1 + str;
   
   //glColor3f(1, 0, 0); //red
   GlColorType *color = (GlColorType*)&textColor;
   glColor3ub(color->red, color->green, color->blue);
   glRasterPos2i(xpos, ypos);
   //glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.c_str());
   glCallLists(strlen(text.c_str()), GL_BYTE, (GLubyte*)text.WX_TO_C_STRING);
   
   if (label3 != "")
   {
      glRasterPos2i(xpos, 50);
      glCallLists(strlen(label3.c_str()), GL_BYTE, (GLubyte*)label3.WX_TO_C_STRING);
   }
   
   if (showCS)
   {
      // Prepend space before coordinate system name (Bug 2318 fix)
      wxString viewCsName = "  " + mViewCoordSysName;
      glRasterPos2i(xpos, ypos+20);
      glCallLists(strlen(viewCsName.c_str()), GL_BYTE, (GLubyte*)viewCsName.WX_TO_C_STRING);
   }
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   #ifdef DEBUG_DRAW_STATUS
   MessageInterface::ShowMessage("DrawStatus() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void DrawDebugMessage(const wxString &msg, unsigned int textColor, int xpos, int ypos)
//------------------------------------------------------------------------------
void ViewCanvas::DrawDebugMessage(const wxString &msg, unsigned int textColor,
                                  int xpos, int ypos)
{
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLfloat)mCanvasSize.x, 0.0, (GLfloat)mCanvasSize.y);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   GlColorType *color = (GlColorType*)&textColor;
   glColor3ub(color->red, color->green, color->blue);
   glRasterPos2i(xpos, ypos);
   glCallLists(strlen(msg.c_str()), GL_BYTE, (GLubyte*)msg.WX_TO_C_STRING);
   
   SetupProjection();
}

void ViewCanvas::SetMaxDataPoints(Integer maxPlotPoints)
{
   maxData = maxPlotPoints;
}
