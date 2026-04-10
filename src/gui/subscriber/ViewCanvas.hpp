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
 * Declares ViewCanvas for 3D visualization.
 */
//------------------------------------------------------------------------------
#ifndef ViewCanvas_hpp
#define ViewCanvas_hpp

#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "GuiInterpreter.hpp"
#include "SolarSystem.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "ModelObject.hpp"
#include <map>

class ViewCanvas: public wxGLCanvas
{
public:
   ViewCanvas(wxWindow *parent, const wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, 
              const wxString& name = wxT("ViewCanvas"),
              long style = 0);
   virtual ~ViewCanvas();
   
   // GL context
   virtual bool SetGLContext(const wxString &msg = "");
   
   // initialization
   virtual bool InitializePlot();
   virtual bool InitOpenGL();
   virtual void SetIsNewFrame(bool flag) {mIsNewFrame = flag; }
   
   // user interrupt
   virtual void SetUserInterrupt() { mHasUserInterrupted = true; }
   
   //Max data point option
   virtual void SetMaxDataPoints(Integer);

   // drawing options
   virtual bool GetUseInitialViewDef() { return mUseInitialViewPoint; }   
   virtual void SetUseInitialViewDef(bool flag) { mUseInitialViewPoint = flag; }
   virtual bool GetDrawWireFrame() { return mDrawWireFrame; }
   virtual void SetDrawWireFrame(bool flag) { mDrawWireFrame = flag; }
   virtual bool GetDrawGrid() { return mDrawGrid; }
   virtual void SetDrawGrid(bool flag) { mDrawGrid = flag; }
   
   // animation
   virtual int  GetAnimationUpdateInterval() { return mUpdateInterval; }
   virtual void SetAnimationUpdateInterval(int value) { mUpdateInterval = value; }
   virtual int  GetAnimationFrameIncrement() { return mFrameInc; }
   virtual void SetAnimationFrameIncrement(int value) { mFrameInc = value; }
   virtual bool IsAnimationRunning() { return mIsAnimationRunning; }
   
   // objects
   virtual const wxStringBoolMap& GetShowObjectMap() { return mShowObjectMap; }
   virtual const ColorMap& GetObjectOrbitColorMap() { return mObjectOrbitColorMap; }
   virtual const ColorMap& GetObjectTargetColorMap() { return mObjectTargetColorMap; }
   virtual const wxArrayString& GetObjectNames() { return mObjectNames; }
   virtual const wxArrayString& GetValidCSNames() { return mValidCSNames; }
   virtual void SetObjectOrbitColors(const ColorMap &orbitColorMap);
   virtual void SetObjectTargetColors(const ColorMap &targetColorMap);
   virtual void SetShowObjects(const wxStringBoolMap &showObjMap);  
   virtual wxString GetGotoObjectName();
   
   // coordinate system
   virtual wxString GetViewCoordSysName() { return mViewCoordSysName; }
   virtual CoordinateSystem* GetViewCoordSystem() { return pViewCoordSystem; }
   
   // view distance
   virtual float GetCurrentViewDistance() { return mCurrViewDistance; }
   virtual void SetCurrentViewDistance(float dist) { mCurrViewDistance = dist; }
   
   // mission run
   virtual void SetEndOfData(bool flag = true) { mIsEndOfData = flag; }
   virtual void SetEndOfRun(bool flag = true);   
   
   
   // actions
   virtual void ClearPlot() = 0;
   virtual void RedrawPlot(bool viewAnimation) = 0;
   virtual void ShowDefaultView() = 0;
   virtual void DrawWireFrame(bool flag) = 0;
   
   virtual void OnDrawGrid(bool flag) = 0;
   
   virtual void DrawInOtherCoordSystem(const wxString &csName) = 0;
   virtual void GotoObject(const wxString &objName);
   virtual void ViewAnimation(int interval, int frameInc = 30) = 0;
   
   virtual void SetGlObject(const StringArray &objNames,
                        const std::vector<SpacePoint*> &objectArray);
   
   // SolarSystem
   virtual void SetSolarSystem(SolarSystem *ss);
   
   // CoordinateSystem
   virtual void SetGlCoordSystem(CoordinateSystem *internalCs,
                        CoordinateSystem *viewCs,
                        CoordinateSystem *viewUpCs);
   
   // drawing options
   virtual void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   virtual void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                        bool drawXyPlane, bool drawWireFrame,
                        bool drawAxes, bool drawGrid, bool drawSunLine,
                        bool usevpInfo, bool drawStars,
                        bool drawConstellations,
                        Integer starCount);
   
   // viewpoint
   virtual void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vscaleFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, const std::string &upAxis,
                        bool usevpRefVec, bool usevpVec, bool usevdVec);
   
   // drawing toggle switch
   virtual void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray);
   virtual void SetGlShowObjectFlag(const std::vector<bool> &showArray);
   
   // performance
   virtual void SetUpdateFrequency(Integer updFreq);
   virtual void SetNumPointsToRedraw(Integer numPoints);
   
   // actions
   virtual void TakeAction(const std::string &action);
   
   // update
   virtual void UpdatePlot(const StringArray &scNames, const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const ColorMap &orbitColorMap,
                        const ColorMap &targetColorMap,
                        bool solving, Integer solverOption, bool drawing,
                        bool inFunction = false);
   
   // object
   virtual void AddObjectList(const wxArrayString &objNames, bool clearList = true);
   
   // Double buffer activation needed in Linux (Patch from Tristan Moody)
   // Moved from gmatwxdefs.hpp to remove compiler warning: defined but not used (LOJ: 2012.05.29)
   static int GmatGLCanvasAttribs[4]; // = {WX_GL_DOUBLEBUFFER, 0};

protected:

   enum
   {
      UNINIT_TEXTURE = 999,
      UNKNOWN_BODY = -1,
      UNKNOWN_OBJ_ID = -999,
   };
   
   static const Real RADIUS_ZOOM_RATIO;// = 2.2;
   
   // Parent frame
   wxWindow *mParent;
   
   // OpenGL Context
   wxGLContext *theContext;
   wxSize  mCanvasSize;
   
   // GuiInterpreter
   GuiInterpreter *theGuiInterpreter;
   wxStatusBar *theStatusBar;
   
   // Initialization
   wxString mPlotName;
   bool mGlInitialized;
   bool mViewPointInitialized;
   bool mSatModelsAreLoaded;
   bool mOtherModelsAreLoaded;
   bool mIsNewFrame;
   
   // Performance
   bool mRedrawLastPointsOnly;
   int  mNumPointsToRedraw;
   int  mUpdateFrequency;
   
   // Ring buffer index
   int  mBeginIndex1;
   int  mBeginIndex2;
   int  mEndIndex1;
   int  mEndIndex2;
   int  mRealBeginIndex1;
   int  mRealBeginIndex2;
   int  mRealEndIndex1;
   int  mRealEndIndex2;
   int  mLastIndex;
   int  mCurrIndex;
   
   // Data points
   int  mNumData;
   int  mTotalPoints;
   int  maxData;
   
   // Data control flags
   bool mIsEndOfData;
   bool mIsEndOfRun;
   bool mIsFirstRun;
   bool mWriteWarning;
   bool mNeedVelocity;
   bool mNeedAttitude;
   bool mNeedEcliptic;
   bool mIgnoreTimeSequence;
   
   // Drawing option
   bool mDrawWireFrame;
   bool mDrawGrid;   
   bool mShowLabels;
   
   // Light source
   bool mSunPresent;
   bool mEnableLightSource;
   bool mEnableLightSourceOnlyIfSunIsDrawing;
   
   // Texture
   std::map<wxString, GLuint> mTextureIdMap;
   std::map<wxString, std::string> mTextureFileMap;
   
   // View control
   bool mUseInitialViewPoint;
   float mCurrViewDistance;
   
   // Animation
   bool mIsAnimationRunning;
   bool mHasUserInterrupted;
   int  mUpdateInterval;
   int  mFrameInc;
   
   // Message
   bool mShowMaxWarning;
   int  mOverCounter;
   
   // Error handling and function mode
   bool mFatalErrorFound;
   bool mWriteRepaintDisalbedInfo;
   bool mInFunction;
   
   // Solar system
   SolarSystem *pSolarSystem;
   
   // Coordinate system
   wxString mInternalCoordSysName;
   wxString mViewCoordSysName;
   wxString mViewUpCoordSysName;
   wxString mOriginName;
   wxString mViewObjName;   
   CoordinateSystem *pInternalCoordSystem;
   CoordinateSystem *pViewCoordSystem;
   CoordinateSystem *pViewUpCoordSystem;
   
   // For drawing ecliptic plane
   CoordinateSystem *pMJ2000EcCoordSystem;
   
   int mOriginId;
   int mViewObjId;
   float mOriginRadius;
   
   // Coordinate sytem conversion
   bool mViewCsIsInternalCs;
   CoordinateConverter mCoordConverter;
   
   // Spacecraft data
   int   mScCount;
   float mScRadius;
   StringArray mScNameArray;
   GLubyte *mScImage;
   
   // GroundStation data
   GLubyte *mGsImage;
   
   // Celestial objects
   wxArrayString mObjectNames;
   wxArrayString mShowObjectNames;
   wxArrayString mValidCSNames;
   ColorMap mObjectOrbitColorMap;
   ColorMap mObjectTargetColorMap;
   wxStringBoolMap  mShowObjectMap;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   int mObjectCount;
   Real mObjectDefaultRadius;
   
   // Data buffers
   Real *mTime;                    // [MAX_DATA]
   bool *mIsDrawing;               // [MAX_DATA]
   Real *mObjectRadius;            // [mObjectCount]
   Real *mObjMaxZoomIn;            // [mObjectCount]
   int  *mObjLastFrame;            // [mObjectCount]
   bool *mDrawOrbitFlag;           // [mObjectCount][MAX_DATA]
   UnsignedInt *mObjectOrbitColor; // [mObjectCount][MAX_DATA]
   
   // Space object positions
   Real *mObjectGciPos;            // [mObjectCount][MAX_DATA][3]
   Real *mObjectViewPos;           // [mObjectCount][MAX_DATA][3]
   
   // Space object velocities
   Real *mObjectViewVel;           // [mObjectCount][MAX_DATA][3]
   
   // Space object attitude
   Real *mObjectQuat;              // [mObjectCount][MAX_DATA][4]
   
   // Space object rotation angle and axis
   Real *mBodyRotAngle;            // [mObjectCount][MAX_DATA]
   Real *mBodyRotAxis;             // [mObjectCount][MAX_DATA[[3]
   
   // Rotation matrices from view to internal(MJ2000Eq) axes
   Real *mRotMatViewToInternal;    // [MAX_DATA][16]
   // Rotation matrices from view to ecliptic (MJ2000Ec) axes
   Real *mRotMatViewToEcliptic;    // [MAX_DATA][16]
   
   // Solver data
   bool mDrawSolverData;
   bool mIsSolving;
   std::vector<RealArray> mSolverAllPosX; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosY; // [numPoints][numSC]
   std::vector<RealArray> mSolverAllPosZ; // [numPoints][numSC]
   UnsignedIntArray mSolverIterColorArray;
   StringArray mSolverIterScNameArray;
   
   // Viewing
   // Coordinate Transformation Matrices
   Rmatrix mCoordMatrix;
   
   // Events
   virtual void OnPaint(wxPaintEvent &event) = 0;
   virtual void OnSize(wxSizeEvent &event) = 0;
   virtual void OnMouse(wxMouseEvent &event) = 0;
   virtual void OnKeyDown(wxKeyEvent &event) = 0;
	
   // Initialization
   virtual void ResetPlotInfo();   
   virtual void InitializeViewPoint() = 0;
   virtual void SetDefaultView() = 0;
   
   // Data ring buffer indexing
   virtual void ComputeRingBufferIndex();
   virtual void ComputeActualIndex();
   
   // Texture
   virtual bool LoadBodyTextures();
   virtual GLuint BindTexture(SpacePoint *obj, const wxString &objName);
   virtual int  AddAlphaToTexture(const wxImage &image, int objUsingIcon,
                                  bool useMipmaps);
   
   // Model
   virtual bool LoadSpacecraftModels(bool writeWarning);
   virtual bool LoadOtherObjectModels(bool writeWarning);
   
   // View objects
   virtual void SetupProjection();
   virtual void SetupWorld() = 0;
   
   // Drawing mode
   virtual void SetDrawingMode();
   
   // Light source
   virtual void HandleLightSource();
   
   // Drawing objects
   virtual void DrawFrame() = 0;
   virtual void DrawPlot() = 0;
   virtual void DrawObjectOrbit() = 0;
   virtual void DrawObjectTexture(const wxString &objName, int obj, int objId) = 0;
   virtual void DrawObject(const wxString &objName, int obj) = 0;
   virtual void DrawOrbit(const wxString &objName, int obj, int objId);
   virtual void DrawOrbitLines(int i, const wxString &objName, int obj, int objId) = 0;
   
   virtual void DrawSolverData();
   
   virtual void DrawStatus(const wxString &label1, unsigned int textColor,
                        const wxString &label2, double time,  int xpos = 0,
                        int ypos = 0, bool showCS = true, const wxString &label3 = "");
      
   // For object
   int  GetObjectId(const wxString &name);
   void ClearObjectArrays(bool deleteArrays = true);
   bool CreateObjectArrays();
   
   // For data update
   void UpdateSolverData(const StringArray &scNames, const RealArray &posX,
                        const RealArray &posY, const RealArray &posZ,
                        const UnsignedIntArray &scColors, bool solving);
   void UpdateSpacecraftData(const Real &time,
                        const RealArray &posX, const RealArray &posY,
                        const RealArray &posZ, const RealArray &velX,
                        const RealArray &velY, const RealArray &velZ,
                        const UnsignedIntArray &scColors, Integer solverOption);
   void UpdateOtherData(const Real &time);
   void UpdateSpacecraftAttitude(Real time, Spacecraft *sat, int satId);   
   void UpdateOtherObjectAttitude(Real time, SpacePoint *sp, int objId);
   void UpdateBodyRotationData(const wxString &objName, int objId);
   void GetBodyRotationData(int objId, Real &angInDeg, Rvector3 &eAxis);
   void ComputeRotMatForView(Real time, Rvector6 &state);
   void ComputeRotMatForEclipticPlane(Real time, Rvector6 &state);
   void SaveBodyRotationData(int objId, Real angInDeg, const Rvector3 &eAxis);
   void SaveRotMatViewToInternal(Rmatrix &rotMat);
   void SaveRotMatViewToEcliptic(Rmatrix &rotMat);
   
   // For drawing ecliptic plane
   void CreateMJ2000EcCoordSystem();
   
   // For coordinate system
   virtual bool ConvertObjectData() = 0;
   virtual void ConvertObject(int objId, int index) = 0;
   
   // For copy
   void CopyVector3(Real to[3], Real from[3]);
   
   // For loading image
   virtual bool LoadImage(const std::string &fileName, int objUsingIcon);
   
   // For debug
   void DrawDebugMessage(const wxString &msg, unsigned int textColor, int xpos, int ypos);
   
};

#endif
