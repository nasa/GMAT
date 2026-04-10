//$Id$
//------------------------------------------------------------------------------
//                                  OpenGlPlot
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
// Created: 2003/12/16
//
/**
 * Implements OpenGlPlot class.
 */
//------------------------------------------------------------------------------

#include "OpenGlPlot.hpp"
#include "PlotInterface.hpp"       // for UpdateGlPlot()
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "SubscriberException.hpp" // for SubscriberException()
#include "MessageInterface.hpp"    // for ShowMessage()
#include "TextParser.hpp"          // for SeparateBrackets()
#include "StringUtil.hpp"          // for ToReal()
#include "CoordinateConverter.hpp" // for Convert()
#include <algorithm>               // for find(), distance()

#define __REMOVE_OBJ_BY_SETTING_FLAG__

//#define DBGLVL_OPENGL_INIT 1
//#define DBGLVL_OPENGL_DATA 1
//#define DBGLVL_OPENGL_DATA_LABELS 1
//#define DBGLVL_OPENGL_ADD 1
//#define DBGLVL_OPENGL_OBJ 2
//#define DBGLVL_OPENGL_PARAM 1
//#define DEBUG_OPENGL_PUT
//#define DBGLVL_OPENGL_PARAM_STRING 2
//#define DBGLVL_OPENGL_PARAM_RVEC3 1
//#define DBGLVL_OPENGL_UPDATE 2
//#define DBGLVL_TAKE_ACTION 1
//#define DBGLVL_REMOVE_SP 1
//#define DBGLVL_RENAME 1
//#define DBGLVL_SOLVER_CURRENT_ITER 2

//---------------------------------
// static data
//---------------------------------
const std::string
OpenGlPlot::PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount] =
{
   "Add",
   "OrbitColor",
   "TargetColor",
   "CoordinateSystem",
   "ViewPointRef",
   "ViewPointReference",
   "ViewPointRefType",
   "ViewPointRefVector",
   "ViewPointVector",
   "ViewPointVectorType",
   "ViewPointVectorVector",
   "ViewDirection",
   "ViewDirectionType",
   "ViewDirectionVector",
   "ViewScaleFactor",
   "FixedFovAngle",
   "ViewUpCoordinateSystem",
   "ViewUpAxis",
   "CelestialPlane",
   "XYPlane",
   "WireFrame",
   "Axes",
   "Grid",
   "EarthSunLines",
   "SunLine",
   "Overlap",
   "UseInitialView",
   "PerspectiveMode",
   "UseFixedFov",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
   "NumPointsToRedraw",
   "ShowPlot",
        "StarCount",
        "EnableStars",
        "EnableConstellations",
        "MinFOV",
        "MaxFOV",
        "InitialFOV",
}; 


const Gmat::ParameterType
OpenGlPlot::PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,       //"Add"
   Gmat::UNSIGNED_INTARRAY_TYPE, //"OrbitColor",
   Gmat::UNSIGNED_INTARRAY_TYPE, //"TargetColor",
   Gmat::OBJECT_TYPE,            //"CoordinateSystem"
   Gmat::OBJECT_TYPE,            //"ViewPointRef",
   Gmat::OBJECT_TYPE,            //"ViewPointReference",
   Gmat::STRING_TYPE,            //"ViewPointRefType"
   Gmat::RVECTOR_TYPE,           //"ViewPointRefVector",
   //Gmat::STRING_TYPE,            //"ViewPointVector",
   Gmat::OBJECT_TYPE,            //"ViewPointVector",
   Gmat::STRING_TYPE,            //"ViewPointVectorType",
   Gmat::RVECTOR_TYPE,           //"ViewPointVectorVector",
   Gmat::OBJECT_TYPE,            //"ViewDirection",
   Gmat::STRING_TYPE,            //"ViewDirectionType",
   Gmat::RVECTOR_TYPE,           //"ViewDirectionVector",
   Gmat::REAL_TYPE,              //"ViewScaleFactor",
   Gmat::REAL_TYPE,              //"FixedFovAngle",
   Gmat::OBJECT_TYPE,            //"ViewUpCoordinaetSystem"
   Gmat::ENUMERATION_TYPE,       //"ViewUpAxis"
   
   Gmat::ON_OFF_TYPE,            //"CelestialPlane"
   Gmat::ON_OFF_TYPE,            //"XYPlane"
   Gmat::ON_OFF_TYPE,            //"WireFrame"
   Gmat::ON_OFF_TYPE,            //"Axes"
   Gmat::ON_OFF_TYPE,            //"Grid"
   Gmat::ON_OFF_TYPE,            //"EarthSunLines"   
   Gmat::ON_OFF_TYPE,            //"SunLine"   
   Gmat::ON_OFF_TYPE,            //"Overlap"
   Gmat::ON_OFF_TYPE,            //"LockView"
   Gmat::ON_OFF_TYPE,            //"PerspectiveMode"
   Gmat::ON_OFF_TYPE,            //"UseFixedFov"
   
   Gmat::INTEGER_TYPE,           //"DataCollectFrequency"
   Gmat::INTEGER_TYPE,           //"UpdatePlotFrequency"
   Gmat::INTEGER_TYPE,           //"NumPointsToRedraw"
   
   Gmat::BOOLEAN_TYPE,           //"ShowPlot"

        Gmat::INTEGER_TYPE,                             //"StarCount"
        Gmat::ON_OFF_TYPE,                              //"EnableStars"
        Gmat::ON_OFF_TYPE,                              //"EnableConstellations"
        Gmat::INTEGER_TYPE,                             //"MinFOV"
        Gmat::INTEGER_TYPE,                             //"MaxFOV"
        Gmat::INTEGER_TYPE,                             //"InitialFOV"
};


const UnsignedInt
OpenGlPlot::DEFAULT_ORBIT_COLOR[MAX_SP_COLOR] =
{
   GmatColor::RED32,       GmatColor::LIME32,    GmatColor::YELLOW32,
   GmatColor::AQUA32,      GmatColor::PINK32,    GmatColor::L_BLUE32,
   GmatColor::L_GRAY32,    GmatColor::BLUE32,    GmatColor::FUCHSIA32,
   GmatColor::BEIGE32,     GmatColor::RED32,     GmatColor::LIME32,
   GmatColor::YELLOW32,    GmatColor::AQUA32,    GmatColor::PINK32
};


//------------------------------------------------------------------------------
// OpenGlPlot(const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const std::string &name)
   : Subscriber("OpenGLPlot", name)
{
   // GmatBase data
   parameterCount = OpenGlPlotParamCount;
   objectTypes.push_back(Gmat::ORBIT_VIEW);
   objectTypeNames.push_back("OpenGLPlot");
   
   mEclipticPlane = "Off";
   mXYPlane = "On";
   mWireFrame = "Off";
   mAxes = "On";
   mGrid = "Off";
   mSunLine = "Off";
   mOverlapPlot = "Off";
   mUseInitialView = "On";
   mPerspectiveMode = "Off";
   mUseFixedFov = "Off";

        // stars
        mEnableStars = "On";
        mEnableConstellations = "On";
        mStarCount = 46000;

        // FOV
        mMinFOV = 0;
        mMaxFOV = 90;
        mInitialFOV = 45;
   
   mOldName = instanceName;
   mViewCoordSysName = "EarthMJ2000Eq";
   mViewUpCoordSysName = "EarthMJ2000Eq";
   mViewUpAxisName = "Z";
   
   // viewpoint
   mViewPointRefName = "Earth";
   mViewPointRefType = "Object";
   mViewPointVecName = "[ 0 0 30000 ]";
   mViewPointVecType = "Vector";
   mViewDirectionName = "Earth";
   mViewDirectionType= "Object";
   mViewScaleFactor = 1.0;
   mFixedFovAngle = 45.0;
   mViewPointRefVector.Set(0.0, 0.0, 0.0);
   mViewPointVecVector.Set(0.0, 0.0, 30000.0);
   mViewDirectionVector.Set(0.0, 0.0, -1.0);
   
   mViewCoordSystem = NULL;
   mViewUpCoordSystem = NULL;
   mViewCoordSysOrigin = NULL;
   mViewUpCoordSysOrigin = NULL;
   mViewPointRefObj = NULL;
   mViewPointObj = NULL;
   mViewDirectionObj = NULL;
   
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 50;
   mNumPointsToRedraw = 0;
   mNumData = 0;
   mNumCollected = 0;
   mScNameArray.clear();
   mObjectNameArray.clear();
   mAllSpNameArray.clear();
   mAllRefObjectNames.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mShowObjectArray.clear();
   mAllSpArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mDrawOrbitMap.clear();
   mShowObjectMap.clear();
   
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
   mNonStdBodyCount = 0;
   
   // default planet color
   mOrbitColorMap["Earth"] = GmatColor::GREEN32;
   mOrbitColorMap["Luna"] = GmatColor::SILVER32;
   mOrbitColorMap["Sun"] = GmatColor::ORANGE32;
   mOrbitColorMap["Mercury"] = GmatColor::GRAY32;
   mOrbitColorMap["Venus"] = GmatColor::BEIGE32;
   mOrbitColorMap["Mars"] = GmatColor::L_GRAY32;
   mOrbitColorMap["Jupiter"] = GmatColor::L_BROWN32;
   mOrbitColorMap["Saturn"] = GmatColor::D_BROWN32;
   mOrbitColorMap["Uranus"] = GmatColor::BLUE32;
   mOrbitColorMap["Neptune"] = GmatColor::NAVY32;
   mOrbitColorMap["Pluto"] = GmatColor::PURPLE32;
   
}


//------------------------------------------------------------------------------
// OpenGlPlot(const OpenGlPlot &ogl)
//------------------------------------------------------------------------------
/**
 * The copy consturctor
 */
//------------------------------------------------------------------------------
OpenGlPlot::OpenGlPlot(const OpenGlPlot &ogl)
   : Subscriber(ogl)
{
   mEclipticPlane = ogl.mEclipticPlane;
   mXYPlane = ogl.mXYPlane;
   mWireFrame = ogl.mWireFrame;
   mAxes = ogl.mAxes;
   mGrid = ogl.mGrid;
   mSunLine = ogl.mSunLine;
   mOverlapPlot = ogl.mOverlapPlot;
   mUseInitialView = ogl.mUseInitialView;
   mPerspectiveMode = ogl.mPerspectiveMode;
   mUseFixedFov = ogl.mUseFixedFov;
   
   mOldName = ogl.mOldName;;
   mViewCoordSysName = ogl.mViewCoordSysName;
   
   // viewpoint
   mViewPointRefName = ogl.mViewPointRefName;
   mViewPointRefType = ogl.mViewPointRefType;
   mViewPointVecName = ogl.mViewPointVecName;
   mViewPointVecType = ogl.mViewPointVecType;
   mViewDirectionName = ogl.mViewDirectionName;
   mViewDirectionType = ogl.mViewDirectionType;
   mViewScaleFactor = ogl.mViewScaleFactor;
   mFixedFovAngle = ogl.mFixedFovAngle;
   mViewPointRefVector = ogl.mViewPointRefVector;
   mViewPointVecVector = ogl.mViewPointVecVector;
   mViewDirectionVector = ogl.mViewDirectionVector;
   mViewUpCoordSysName = ogl.mViewUpCoordSysName;
   mViewUpAxisName = ogl.mViewUpAxisName;
   
   mViewCoordSystem = ogl.mViewCoordSystem;
   mViewUpCoordSystem = ogl.mViewCoordSystem;
   mViewCoordSysOrigin = ogl.mViewCoordSysOrigin;
   mViewUpCoordSysOrigin = ogl.mViewUpCoordSysOrigin;
   mViewPointRefObj = ogl.mViewPointRefObj;
   mViewPointObj = ogl.mViewPointObj;
   mViewDirectionObj = ogl.mViewDirectionObj;

        // stars
        mStarCount = ogl.mStarCount;
        mEnableStars = ogl.mEnableStars;
        mEnableConstellations = ogl.mEnableConstellations;

        // FOV
        mMinFOV = ogl.mMinFOV;
        mMaxFOV = ogl.mMaxFOV;
        mInitialFOV = ogl.mInitialFOV;
   
   mDataCollectFrequency = ogl.mDataCollectFrequency;
   mUpdatePlotFrequency = ogl.mUpdatePlotFrequency;
   mNumPointsToRedraw = ogl.mNumPointsToRedraw;
   
   mAllSpCount = ogl.mAllSpCount;
   mScCount = ogl.mScCount;
   mObjectCount = ogl.mObjectCount;
   mNonStdBodyCount = ogl.mNonStdBodyCount;
   
   mObjectArray = ogl.mObjectArray;
   mDrawOrbitArray = ogl.mDrawOrbitArray;
   mShowObjectArray = ogl.mShowObjectArray;
   mAllSpArray = ogl.mAllSpArray;
   mScNameArray = ogl.mScNameArray;
   mObjectNameArray = ogl.mObjectNameArray;
   mAllSpNameArray = ogl.mAllSpNameArray;
   mAllRefObjectNames = ogl.mAllRefObjectNames;
   mScXArray = ogl.mScXArray;
   mScYArray = ogl.mScYArray;
   mScZArray = ogl.mScZArray;
   mScVxArray = ogl.mScVxArray;
   mScVyArray = ogl.mScVyArray;
   mScVzArray = ogl.mScVzArray;
   mScOrbitColorArray = ogl.mScOrbitColorArray;
   mScTargetColorArray = ogl.mScTargetColorArray;
   mOrbitColorArray = ogl.mOrbitColorArray;
   mTargetColorArray = ogl.mTargetColorArray;
   
   mOrbitColorMap = ogl.mOrbitColorMap;
   mTargetColorMap = ogl.mTargetColorMap;
   mDrawOrbitMap = ogl.mDrawOrbitMap;
   mShowObjectMap = ogl.mShowObjectMap;
   
   mNumData = ogl.mNumData;
   mNumCollected = ogl.mNumCollected;
}


//------------------------------------------------------------------------------
// OpenGlPlot& operator=(const OpenGlPlot&)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
OpenGlPlot& OpenGlPlot::operator=(const OpenGlPlot& ogl)
{
   if (this == &ogl)
      return *this;
   
   Subscriber::operator=(ogl);
   
   mEclipticPlane = ogl.mEclipticPlane;
   mXYPlane = ogl.mXYPlane;
   mWireFrame = ogl.mWireFrame;
   mAxes = ogl.mAxes;
   mGrid = ogl.mGrid;
   mSunLine = ogl.mSunLine;
   mOverlapPlot = ogl.mOverlapPlot;
   mUseInitialView = ogl.mUseInitialView;
   mPerspectiveMode = ogl.mPerspectiveMode;
   mUseFixedFov = ogl.mUseFixedFov;
   
   mOldName = ogl.mOldName;;
   mViewCoordSysName = ogl.mViewCoordSysName;
   
   // viewpoint
   mViewPointRefName = ogl.mViewPointRefName;
   mViewPointRefType = ogl.mViewPointRefType;
   mViewPointVecName = ogl.mViewPointVecName;
   mViewPointVecType = ogl.mViewPointVecType;
   mViewDirectionName = ogl.mViewDirectionName;
   mViewDirectionType = ogl.mViewDirectionType;
   mViewScaleFactor = ogl.mViewScaleFactor;
   mFixedFovAngle = ogl.mFixedFovAngle;
   mViewPointRefVector = ogl.mViewPointRefVector;
   mViewPointVecVector = ogl.mViewPointVecVector;
   mViewDirectionVector = ogl.mViewDirectionVector;
   mViewUpCoordSysName = ogl.mViewUpCoordSysName;
   mViewUpAxisName = ogl.mViewUpAxisName;
   
   mViewCoordSystem = ogl.mViewCoordSystem;
   mViewUpCoordSystem = ogl.mViewCoordSystem;
   mViewCoordSysOrigin = ogl.mViewCoordSysOrigin;
   mViewUpCoordSysOrigin = ogl.mViewUpCoordSysOrigin;
   mViewPointRefObj = ogl.mViewPointRefObj;
   mViewPointObj = ogl.mViewPointObj;
   mViewDirectionObj = ogl.mViewDirectionObj;
   
   mDataCollectFrequency = ogl.mDataCollectFrequency;
   mUpdatePlotFrequency = ogl.mUpdatePlotFrequency;
   mNumPointsToRedraw = ogl.mNumPointsToRedraw;
   
   mAllSpCount = ogl.mAllSpCount;
   mScCount = ogl.mScCount;
   mObjectCount = ogl.mObjectCount;
   mNonStdBodyCount = ogl.mNonStdBodyCount;
   
   mObjectArray = ogl.mObjectArray;
   mDrawOrbitArray = ogl.mDrawOrbitArray;
   mShowObjectArray = ogl.mShowObjectArray;
   mAllSpArray = ogl.mAllSpArray;
   mScNameArray = ogl.mScNameArray;
   mObjectNameArray = ogl.mObjectNameArray;
   mAllSpNameArray = ogl.mAllSpNameArray;
   mAllRefObjectNames = ogl.mAllRefObjectNames;
   mScXArray = ogl.mScXArray;
   mScYArray = ogl.mScYArray;
   mScZArray = ogl.mScZArray;
   mScVxArray = ogl.mScVxArray;
   mScVyArray = ogl.mScVyArray;
   mScVzArray = ogl.mScVzArray;
   mScOrbitColorArray = ogl.mScOrbitColorArray;
   mScTargetColorArray = ogl.mScTargetColorArray;
   mOrbitColorArray = ogl.mOrbitColorArray;
   mTargetColorArray = ogl.mTargetColorArray;
   
   mOrbitColorMap = ogl.mOrbitColorMap;
   mTargetColorMap = ogl.mTargetColorMap;
   mDrawOrbitMap = ogl.mDrawOrbitMap;
   mShowObjectMap = ogl.mShowObjectMap;
   
   mNumData = ogl.mNumData;
   mNumCollected = ogl.mNumCollected;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OpenGlPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 * @note This destructor does not delete OpenGL plot window, but clears data.
 *       OpenGL plot window is deleted when it is closed by the user or GMAT
 *       shuts down.
 */
//------------------------------------------------------------------------------
OpenGlPlot::~OpenGlPlot()
{
   PlotInterface::TakeGlAction(instanceName, "ClearObjects");
}


//------------------------------------------------------------------------------
// const StringArray& GetSpacePointList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetSpacePointList()
{
   return mAllSpNameArray;
}


//------------------------------------------------------------------------------
// const StringArray& GetSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetSpacecraftList()
{
   return mScNameArray;
}


//------------------------------------------------------------------------------
// const StringArray& GetNonSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetNonSpacecraftList()
{
   return mObjectNameArray;
}


//------------------------------------------------------------------------------
// UnsignedInt GetColor(const std::string &item, const std::string &name)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::GetColor(const std::string &item,
                                 const std::string &name)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetColor() item=%s, name=%s\n",
       item.c_str(), name.c_str());
   #endif
   
   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(name) != mOrbitColorMap.end())
         return mOrbitColorMap[name];
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(name) != mTargetColorMap.end())
         return mTargetColorMap[name];
   }
   
   return GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED;
}


//------------------------------------------------------------------------------
// bool SetColor(const std::string &item, const std::string &name,
//               UnsignedInt value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetColor(const std::string &item, const std::string &name,
                          UnsignedInt value)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetColor() item=%s, name=%s, value=%u\n",
       item.c_str(), name.c_str(), value);
   #endif
   
   if (item == "Orbit")
   {
      if (mOrbitColorMap.find(name) != mOrbitColorMap.end())
      {
         mOrbitColorMap[name] = value;
         
         for (int i=0; i<mAllSpCount; i++)
            if (mAllSpNameArray[i] == name)
               mOrbitColorArray[i] = value;
         
         return true;
      }
   }
   else if (item == "Target")
   {
      if (mTargetColorMap.find(name) != mTargetColorMap.end())
      {
         mTargetColorMap[name] = value;
         
         for (int i=0; i<mAllSpCount; i++)
            if (mAllSpNameArray[i] == name)
               mTargetColorArray[i] = value;
         
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetShowObject(const std::string &name)
//------------------------------------------------------------------------------
bool OpenGlPlot::GetShowObject(const std::string &name)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetShowObject() name=%s returning %d\n",
       name.c_str(), mDrawOrbitMap[name]);
   #endif
   
   return mShowObjectMap[name];
}


//------------------------------------------------------------------------------
// void SetShowObject(const std::string &name, bool value)
//------------------------------------------------------------------------------
void OpenGlPlot::SetShowObject(const std::string &name, bool value)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetShowObject() name=%s setting %d\n", name.c_str(), value);
   #endif
   
   mShowObjectMap[name] = value;
   if (value)
      mDrawOrbitMap[name] = value;
}


//------------------------------------------------------------------------------
// Rvector3 GetVector(const std::string &which)
//------------------------------------------------------------------------------
Rvector3 OpenGlPlot::GetVector(const std::string &which)
{
   if (which == "ViewPointReference")
      return mViewPointRefVector;
   else if (which == "ViewPointVector")
      return mViewPointVecVector;
   else if (which == "ViewDirection")
      return mViewDirectionVector;
   else
      throw SubscriberException(which + " is unknown OpenGlPlot parameter\n");
}


//------------------------------------------------------------------------------
// void SetVector(const std::string &which, const Rvector3 &value)
//------------------------------------------------------------------------------
void OpenGlPlot::SetVector(const std::string &which, const Rvector3 &value)
{
   #if DBGLVL_OPENGL_SET
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetVector() which=%s, value=%s\n", which.c_str(),
       value.ToString().c_str());
   #endif
   
   if (which == "ViewPointReference")
      mViewPointRefVector = value;
   else if (which == "ViewPointVector")
      mViewPointVecVector = value;
   else if (which == "ViewDirection")
      mViewDirectionVector = value;
   else
      throw SubscriberException(which + " is unknown OpenGlPlot parameter\n");
}


//----------------------------------
// inherited methods from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool OpenGlPlot::Initialize()
{
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)
      return true;
   
   Subscriber::Initialize();
   
   // theInternalCoordSys is used only by OpenGL plot so check. (2008.06.16)
   if (theInternalCoordSystem == NULL)
   {
      active = false;
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "*** WARNING *** The OpenGL plot named \"%s\" will be turned off. "
          "It has a NULL internal coordinate system pointer.\n", GetName().c_str());
      return false;
   }
   
   #if DBGLVL_OPENGL_INIT
   MessageInterface::ShowMessage
      ("OpenGlPlot::Initialize() this=<%p>'%s', active=%d, isInitialized=%d, "
       "isEndOfReceive=%d, mAllSpCount=%d\n", this, GetName().c_str(), active,
       isInitialized, isEndOfReceive, mAllSpCount);
   #endif
   
   bool foundSc = false;
   bool retval = false;
   Integer nullCounter = 0;
   
   if (mAllSpCount == 0)
   {
      active = false;
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "*** WARNING *** The OpenGL plot named \"%s\" will be turned off. "
          "No SpacePoints were added to plot.\n", GetName().c_str());
      return false;
   }
   
   // check for spacecaft is included in the plot
   for (int i=0; i<mAllSpCount; i++)
   {
      #if DBGLVL_OPENGL_INIT > 1
      MessageInterface::ShowMessage
         ("OpenGlPlot::Initialize() mAllSpNameArray[%d]=%s, addr=%d\n",
          i, mAllSpNameArray[i].c_str(), mAllSpArray[i]);
      #endif
      
      if (mAllSpArray[i])
      {                  
         if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
         {
            foundSc = true;
            break;
         }
      }
      else
         nullCounter++;
   }
   
   if (nullCounter == mAllSpCount)
   {
      active = false;
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "*** WARNING *** The OpenGL plot named \"%s\" will be turned off. "
          "%d SpaceObjects have NULL pointers.\n", GetName().c_str(), nullCounter);
      return false;
   }
   
   if (!foundSc)
   {
      active = false;
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "*** WARNING *** The OpenGL plot named \"%s\" will be turned off. "
          "No Spacecraft was added to plot.\n", GetName().c_str());
      return false;
   }
   
   
   //--------------------------------------------------------
   // start initializing for OpenGL plot
   //--------------------------------------------------------
   if (active && !isInitialized)
   {
      #if DBGLVL_OPENGL_INIT
      MessageInterface::ShowMessage
         ("OpenGlPlot::Initialize() CreateGlPlotWindow() theSolarSystem=%p\n",
          theSolarSystem);
      #endif
      
      if (PlotInterface::CreateGlPlotWindow
          (instanceName, mOldName, (mEclipticPlane == "On"), (mXYPlane == "On"),
           (mWireFrame == "On"), (mAxes == "On"), (mGrid == "On"),
           (mSunLine == "On"), (mOverlapPlot == "On"), (mUseInitialView == "On"),
           (mPerspectiveMode == "On"), mNumPointsToRedraw, 
			  (mEnableStars == "On"), (mEnableConstellations == "On"), mStarCount))
      {
         #if DBGLVL_OPENGL_INIT
         MessageInterface::ShowMessage
            ("   mViewPointRefObj=%p, mViewScaleFactor=%f\n",
             mViewPointRefObj, mViewScaleFactor);
         #endif
         
         //--------------------------------------------------------
         // Set Spacecraft and non-Spacecraft objects.
         // If non-Spacecraft, position has to be computed in the
         // TrajPlotCanvas, so need to pass those object pointers.
         //--------------------------------------------------------
         
         ClearDynamicArrays();
         
         // add non-spacecraft plot objects to the list
         for (int i=0; i<mAllSpCount; i++)
         {
            #if DBGLVL_OPENGL_INIT > 1
            MessageInterface::ShowMessage
               ("OpenGlPlot::Initialize() mAllSpNameArray[%d]=%s, addr=%d\n",
                i, mAllSpNameArray[i].c_str(), mAllSpArray[i]);
            #endif
            
            if (mAllSpArray[i])
            {
               //add all objects to object list
               mObjectNameArray.push_back(mAllSpNameArray[i]);                  
               mDrawOrbitArray.push_back(mDrawOrbitMap[mAllSpNameArray[i]]);
               mShowObjectArray.push_back(mShowObjectMap[mAllSpNameArray[i]]);
               mOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
               mTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
               mObjectArray.push_back(mAllSpArray[i]);
               
               if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
               {
                  mScNameArray.push_back(mAllSpNameArray[i]);
                  mScOrbitColorArray.push_back(mOrbitColorMap[mAllSpNameArray[i]]);
                  mScTargetColorArray.push_back(mTargetColorMap[mAllSpNameArray[i]]);
                  mScXArray.push_back(0.0);
                  mScYArray.push_back(0.0);
                  mScZArray.push_back(0.0);
                  mScVxArray.push_back(0.0);
                  mScVyArray.push_back(0.0);
                  mScVzArray.push_back(0.0);
               }
            }
            else
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "The SpacePoint name: %s has NULL pointer.\n"
                   "It will be removed from the OpenGL plot.\n",
                   mAllSpNameArray[i].c_str());
            }
         }
         
         mScCount = mScNameArray.size();
         mObjectCount = mObjectNameArray.size();
         
         // check ViewPoint info to see if any objects need to be
         // included in the non-spacecraft list
         if (mViewCoordSystem == NULL)
            throw SubscriberException
               ("OpenGlPlot::Initialize() CoordinateSystem: " + mViewCoordSysName +
                " not set\n");
         
         if (mViewUpCoordSystem == NULL)
            throw SubscriberException
               ("OpenGlPlot::Initialize() CoordinateSystem: " + mViewUpCoordSysName +
                " not set\n");               
         
         // Get View CoordinateSystem Origin pointer
         mViewCoordSysOrigin = mViewCoordSystem->GetOrigin();
         
         if (mViewCoordSysOrigin != NULL)
            UpdateObjectList(mViewCoordSysOrigin);
         
         // Get View Up CoordinateSystem Origin pointer
         mViewUpCoordSysOrigin = mViewUpCoordSystem->GetOrigin();
         
         if (mViewUpCoordSysOrigin != NULL)
            UpdateObjectList(mViewUpCoordSysOrigin);
         
         // Get ViewPointRef object pointer from the current SolarSystem
         if (mViewPointRefObj != NULL)
            UpdateObjectList(mViewPointRefObj);
         
         // Get ViewPoint object pointer from the current SolarSystem
         if (mViewPointObj != NULL)
            UpdateObjectList(mViewPointObj);
         
         // Get ViewDirection object pointer from the current SolarSystem
         if (mViewDirectionObj != NULL)
            UpdateObjectList(mViewDirectionObj);
         
         #if DBGLVL_OPENGL_INIT > 1
         MessageInterface::ShowMessage
            ("   mScNameArray.size=%d, mScOrbitColorArray.size=%d\n",
             mScNameArray.size(), mScOrbitColorArray.size());
         MessageInterface::ShowMessage
            ("   mObjectNameArray.size=%d, mOrbitColorArray.size=%d\n",
             mObjectNameArray.size(), mOrbitColorArray.size());
         
         bool draw, show;
         for (int i=0; i<mObjectCount; i++)
         {
            draw = mDrawOrbitArray[i] ? true : false;
            show = mShowObjectArray[i] ? true : false;
            MessageInterface::ShowMessage
               ("   mObjectNameArray[%d]=%s, draw=%d, show=%d, color=%d\n",
                i, mObjectNameArray[i].c_str(), draw, show, mOrbitColorArray[i]);
         }
         #endif
         
         #if DBGLVL_OPENGL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlSolarSystem(%p)\n", theSolarSystem);
         #endif
         
         // set SolarSystem
         PlotInterface::SetGlSolarSystem(instanceName, theSolarSystem);
         
         #if DBGLVL_OPENGL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlObject()\n");
         for (UnsignedInt i=0; i<mObjectArray.size(); i++)
            MessageInterface::ShowMessage
               ("      mObjectArray[%d]=<%p>'%s'\n", i, mObjectArray[i],
                mObjectArray[i]->GetName().c_str());
         #endif
         
         // set all object array and pointers
         PlotInterface::SetGlObject(instanceName, mObjectNameArray,
                                    mOrbitColorArray, mObjectArray);
         
         //--------------------------------------------------------
         // set CoordinateSystem
         //--------------------------------------------------------
         #if DBGLVL_OPENGL_INIT
         MessageInterface::ShowMessage
            ("   theInternalCoordSystem = <%p>, origin = <%p>'%s'\n"
             "   theDataCoordSystem     = <%p>, origin = <%p>'%s'\n"
             "   mViewCoordSystem       = <%p>, origin = <%p>'%s'\n"
             "   mViewUpCoordSystem     = <%p>, origin = <%p>'%s'\n"
             "   mViewPointRefObj       = <%p>'%s'\n"
             "   mViewPointObj          = <%p>'%s'\n"
             "   mViewDirectionObj      = <%p>'%s'\n",
             theInternalCoordSystem, theInternalCoordSystem->GetOrigin(),
             theInternalCoordSystem->GetOriginName().c_str(),
             theDataCoordSystem, theDataCoordSystem->GetOrigin(),
             theDataCoordSystem->GetOriginName().c_str(),
             mViewCoordSystem, mViewCoordSystem->GetOrigin(),
             mViewCoordSystem->GetOriginName().c_str(),
             mViewUpCoordSystem, mViewUpCoordSystem->GetOrigin(),
             mViewUpCoordSystem->GetOriginName().c_str(),
             mViewPointRefObj,
             mViewPointRefObj ? mViewPointRefObj->GetName().c_str() : "NULL",
             mViewPointObj,
             mViewPointObj ? mViewPointObj->GetName().c_str() : "NULL",
             mViewDirectionObj,
             mViewDirectionObj ? mViewDirectionObj->GetName().c_str() : "NULL");
         
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlCoordSystem()\n");
         #endif
         
         PlotInterface::SetGlCoordSystem(instanceName, theInternalCoordSystem,
                                         mViewCoordSystem, mViewUpCoordSystem);
         
         //--------------------------------------------------------
         // set viewpoint info
         //--------------------------------------------------------
         #if DBGLVL_OPENGL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlViewOption()\n");
         #endif
         
         PlotInterface::SetGlViewOption
            (instanceName, mViewPointRefObj, mViewPointObj,
             mViewDirectionObj, mViewScaleFactor, mViewPointRefVector,
             mViewPointVecVector, mViewDirectionVector, mViewUpAxisName,
             (mViewPointRefType == "Vector"), (mViewPointVecType == "Vector"),
             (mViewDirectionType == "Vector"), (mUseFixedFov == "On"),
             mFixedFovAngle);
         
         PlotInterface::SetGlUpdateFrequency(instanceName, mUpdatePlotFrequency);
         
         //--------------------------------------------------------
         // set drawing object flag
         //--------------------------------------------------------
         PlotInterface::SetGlDrawOrbitFlag(instanceName, mDrawOrbitArray);
         PlotInterface::SetGlShowObjectFlag(instanceName, mShowObjectArray);
         
         isInitialized = true;
         retval = true;
      }
      else
      {
         retval = false;
      }
   }
   else
   {
      #if DBGLVL_OPENGL_INIT
      MessageInterface::ShowMessage
         ("OpenGlPlot::Initialize() Plot is active and initialized, "
          "so calling DeleteGlPlot()\n");
      #endif
      
      // Why do we want to delete plot if active and initialized?
      // This causes Global OpenGL plot not to show, so commented out (loj: 2008.10.08)
      //retval =  PlotInterface::DeleteGlPlot(instanceName);
   }
   
   #if DBGLVL_OPENGL_INIT
   MessageInterface::ShowMessage("OpenGlPlot::Initialize() exiting\n");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void Activate(bool state)
//------------------------------------------------------------------------------
bool OpenGlPlot::Activate(bool state)
{
   #ifdef DEBUG_OPENGL_ACTIVATE
   MessageInterface::ShowMessage
      ("OpenGlPlot::Activate() this=<%p>'%s' entered, state=%d, isInitialized=%d\n",
       this, GetName().c_str(), state, isInitialized);
   #endif
   
   return Subscriber::Activate(state);
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the OpenGlPlot.
 *
 * @return clone of the OpenGlPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* OpenGlPlot::Clone() const
{
   return (new OpenGlPlot(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void OpenGlPlot::Copy(const GmatBase* orig)
{
   operator=(*((OpenGlPlot *)(orig)));
}


//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName = "")
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @see GmatBase
 *
 */
//------------------------------------------------------------------------------
bool OpenGlPlot::SetName(const std::string &who, const std::string &oldName)
{
   #if DBGLVL_RENAME
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetName() newName=%s, oldName=%s\n", who.c_str(),
       oldName.c_str());
   #endif
   
   if (oldName == "")
      mOldName = instanceName;
   else
      mOldName = oldName;
   
   return GmatBase::SetName(who);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool OpenGlPlot::TakeAction(const std::string &action,
                            const std::string &actionData)
{
   #if DBGLVL_TAKE_ACTION
   MessageInterface::ShowMessage
      ("OpenGlPlot::TakeAction() '%s' entered, action='%s', actionData='%s'\n",
       GetName().c_str(), action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      return ClearSpacePointList();
   }
   else if (action == "Remove")
   {
      return RemoveSpacePoint(actionData);
   }
   else if (action == "Finalize")
   {
      PlotInterface::DeleteGlPlot(instanceName);
   }
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OpenGlPlot::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #if DBGLVL_RENAME
   MessageInterface::ShowMessage
      ("OpenGlPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   if (type == Gmat::SPACECRAFT)
   {
      // for spacecraft name
      for (int i=0; i<mAllSpCount; i++)
         if (mAllSpNameArray[i] == oldName)
            mAllSpNameArray[i] = newName;
      
      //----------------------------------------------------
      // Since spacecraft name is used as key for spacecraft
      // color map, I can't change the key name, so it is
      // removed and inserted with new name
      //----------------------------------------------------
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      std::map<std::string, bool>::iterator drawOrbitPos, showObjectPos;
      orbColorPos = mOrbitColorMap.find(oldName);
      targColorPos = mTargetColorMap.find(oldName);
      drawOrbitPos = mDrawOrbitMap.find(oldName);
      showObjectPos = mShowObjectMap.find(oldName);
   
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // add new spacecraft name key and delete old
         mOrbitColorMap[newName] = mOrbitColorMap[oldName];
         mTargetColorMap[newName] = mTargetColorMap[oldName];
         mDrawOrbitMap[newName] = mDrawOrbitMap[oldName];
         mShowObjectMap[newName] = mShowObjectMap[oldName];
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);
         mDrawOrbitMap.erase(drawOrbitPos);
         mShowObjectMap.erase(showObjectPos);
         
         #if DBGLVL_RENAME
         MessageInterface::ShowMessage("---After rename\n");
         for (orbColorPos = mOrbitColorMap.begin();
              orbColorPos != mOrbitColorMap.end(); ++orbColorPos)
         {
            MessageInterface::ShowMessage
               ("sc=%s, color=%d\n", orbColorPos->first.c_str(), orbColorPos->second);
         }
         #endif
      }
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (mViewCoordSysName == oldName)
         mViewCoordSysName = newName;

      if (mViewUpCoordSysName == oldName)
         mViewUpCoordSysName = newName;      
   }
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<OpenGlPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType OpenGlPlot::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < OpenGlPlotParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - SubscriberParamCount)];
   else
      return Subscriber::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool OpenGlPlot::IsParameterReadOnly(const Integer id) const
{
   //Note: We can remove PERSPECTIVE_MODE, USE_FIXED_FOV, FIXED_FOV_ANGLE
   //      when perspective mode is working.
   
   if (id == OVERLAP_PLOT ||
       id == PERSPECTIVE_MODE || id == USE_FIXED_FOV || id == FIXED_FOV_ANGLE ||
       id == EARTH_SUN_LINES || id == VIEWPOINT_REF || id == VIEWPOINT_REF_VECTOR ||
       id == VIEWPOINT_VECTOR_VECTOR || id == VIEW_DIRECTION_VECTOR ||
       id == VIEWPOINT_REF_TYPE || id == VIEWPOINT_VECTOR_TYPE ||
       id == VIEW_DIRECTION_TYPE)
      return true;
   
   return Subscriber::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      return mDataCollectFrequency;
   case UPDATE_PLOT_FREQUENCY:
      return mUpdatePlotFrequency;
   case NUM_POINTS_TO_REDRAW:
      return mNumPointsToRedraw;
        case STAR_COUNT:
                return mStarCount;
        case MIN_FOV:
                return mMinFOV;
        case MAX_FOV:
                return mMaxFOV;
        case INITIAL_FOV:
                return mInitialFOV;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer OpenGlPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer OpenGlPlot::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      if (value > 0)
      {
         mDataCollectFrequency = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "DataCollectFrequency", "Integer Number > 0");
         throw se;
      }
   case UPDATE_PLOT_FREQUENCY:
      if (value > 0)
      {
         mUpdatePlotFrequency = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "UpdatePlotFrequency", "Integer Number > 0");
         throw se;
      }
   case NUM_POINTS_TO_REDRAW:
      if (value >= 0)
      {
         mNumPointsToRedraw = value;
         return value;
      }
      else
      {
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(),
                       GmatStringUtil::ToString(value, 1).c_str(),
                       "NumPointsToRedraw", "Integer Number >= 0");
         throw se;
      }
        case STAR_COUNT:
                if (value >= 0)
                {
                        mStarCount = value;
                        return value;
                }
                else
                {
                        SubscriberException se;
                        se.SetDetails(errorMessageFormat.c_str(),
                                                        GmatStringUtil::ToString(value, 1).c_str(),
                                                        "StarCount", "Integer Value >= 0");
                }
        case MIN_FOV:
                mMinFOV = value;
                return value;
        case MAX_FOV:
                mMaxFOV = value;
                return value;
        case INITIAL_FOV:
                mInitialFOV = value;
                return value;
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer OpenGlPlot::SetIntegerParameter(const std::string &label,
                                        const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real OpenGlPlot::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case VIEW_SCALE_FACTOR:
      return mViewScaleFactor;
   case FIXED_FOV_ANGLE:
      return mFixedFovAngle;
   default:
      return Subscriber::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real OpenGlPlot::GetRealParameter(const std::string &label) const
{
   #if DBGLVL_OPENGL_PARAM
     MessageInterface::ShowMessage
        ("OpenGlPlot::GetRealParameter() label = %s\n", label.c_str());
   #endif
   
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real OpenGlPlot::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case VIEW_SCALE_FACTOR:
      mViewScaleFactor = value;
      return value;
   case FIXED_FOV_ANGLE:
      mFixedFovAngle = value;
      return value;
   default:
      return Subscriber::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real OpenGlPlot::SetRealParameter(const std::string &label, const Real value)
{
   #if DBGLVL_OPENGL_PARAM
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetRealParameter() label = %s, value = %f \n",
          label.c_str(), value);
   #endif
   
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
Real OpenGlPlot::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      WriteDeprecatedMessage(id);
      return mViewPointRefVector[index];
      
   case VIEWPOINT_VECTOR_VECTOR:
      WriteDeprecatedMessage(id);
      return mViewPointVecVector[index];
      
   case VIEW_DIRECTION_VECTOR:
      WriteDeprecatedMessage(id);
      return mViewDirectionVector[index];
      
   default:
      return Subscriber::GetRealParameter(id, index);
   }
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
Real OpenGlPlot::SetRealParameter(const Integer id, const Real value,
                                  const Integer index)
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      WriteDeprecatedMessage(id);
      mViewPointRefVector[index] = value;
      return value;
      
   case VIEWPOINT_VECTOR_VECTOR:
      WriteDeprecatedMessage(id);
      mViewPointVecVector[index] = value;
      return value;
      
   case VIEW_DIRECTION_VECTOR:
      WriteDeprecatedMessage(id);
      mViewDirectionVector[index] = value;
      return value;
      
   default:
      return Subscriber::SetRealParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::GetRvectorParameter(const Integer id) const
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      //WriteDeprecatedMessage(id);
      return mViewPointRefVector;
   case VIEWPOINT_VECTOR_VECTOR:
      {
         //WriteDeprecatedMessage(id);
         #if DBGLVL_OPENGL_PARAM
         Rvector vec = mViewPointVecVector;
         MessageInterface::ShowMessage
            ("OpenGlPlot::GetRvectorParameter() returning = %s\n",
             vec.ToString().c_str());
         #endif
         return mViewPointVecVector;
      }
   case VIEW_DIRECTION_VECTOR:
      //WriteDeprecatedMessage(id);
      return mViewDirectionVector;
   default:
      return Subscriber::GetRvectorParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::GetRvectorParameter(const std::string &label) const
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRvectorParameter() label = %s\n", label.c_str());
   #endif
   
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual const Rvector& SetRvectorParameter(const Integer id,
//                                            const Rvector &value)
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::SetRvectorParameter(const Integer id,
                                               const Rvector &value)
{
   switch (id)
   {
   case VIEWPOINT_REF_VECTOR:
      WriteDeprecatedMessage(id);
      mViewPointRefVector[0] = value[0];
      mViewPointRefVector[1] = value[1];
      mViewPointRefVector[2] = value[2];
      return value;
      
   case VIEWPOINT_VECTOR_VECTOR:
      mViewPointVecVector[0] = value[0];
      mViewPointVecVector[1] = value[1];
      mViewPointVecVector[2] = value[2];
      return value;
      
   case VIEW_DIRECTION_VECTOR:
      WriteDeprecatedMessage(id);
      mViewDirectionVector[0] = value[0];
      mViewDirectionVector[1] = value[1];
      mViewDirectionVector[2] = value[2];
      return value;
      
   default:
      return Subscriber::SetRvectorParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual const Rvector& SetRvectorParameter(const std::string &label,
//                                            const Rvector &value)
//------------------------------------------------------------------------------
const Rvector& OpenGlPlot::SetRvectorParameter(const std::string &label,
                                               const Rvector &value)
{
   #if DBGLVL_OPENGL_PARAM
   Rvector val = value;
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetRvectorParameter() label = %s, "
       "value = %s \n", label.c_str(), val.ToString().c_str());
   #endif
   
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const Integer id) const
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetStringParameter()<%s> id=%d<%s>\n",
       instanceName.c_str(), id, GetParameterText(id).c_str());
   #endif
   
   switch (id)
   {
   case COORD_SYSTEM:
      return mViewCoordSysName;
   case VIEWPOINT_REF:      
      WriteDeprecatedMessage(id);
      if (mViewPointRefType == "Vector")
         return ("[ " + mViewPointRefVector.ToString(16) + " ]");
      else
         return mViewPointRefName;
   case VIEWPOINT_REFERENCE:      
      if (mViewPointRefType == "Vector")         
         return ("[ " + mViewPointRefVector.ToString(16) + " ]");
      else
         return mViewPointRefName;
   case VIEWPOINT_REF_TYPE:
      return mViewPointRefType;
   case VIEWPOINT_VECTOR:
      if (mViewPointVecType == "Vector")
         return ("[ " + mViewPointVecVector.ToString(16) + " ]");
      else
         return mViewPointVecName;
   case VIEWPOINT_VECTOR_TYPE:
      return mViewPointVecType;
   case VIEW_DIRECTION:
      if (mViewDirectionType == "Vector")
         return ("[ " + mViewDirectionVector.ToString(16) + " ]");
      else
         return mViewDirectionName;
   case VIEW_DIRECTION_TYPE:
      return mViewDirectionType;
   case VIEW_UP_COORD_SYSTEM:
      return mViewUpCoordSysName;
   case VIEW_UP_AXIS:
      return mViewUpAxisName;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetStringParameter(const std::string &label) const
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetStringParameter() this=<%p>'%s', id=%d<%s>, value='%s'\n",
       this, instanceName.c_str(), id, GetParameterText(id).c_str(), value.c_str());
   #endif
   
   switch (id)
   {
   case ADD:
      return AddSpacePoint(value, mAllSpCount);
   case ORBIT_COLOR:
   case TARGET_COLOR:
      if (value[0] == '[')
         PutUnsignedIntValue(id, value);
      return true;
   case COORD_SYSTEM:
      mViewCoordSysName = value;
      return true;
   case VIEWPOINT_REF:
      WriteDeprecatedMessage(id);
      mViewPointRefName = value;
      mViewPointRefType = "Object";
      
      // Handle deprecated value "Vector"
      if (value == "Vector" || GmatStringUtil::IsNumber(value))
         mViewPointRefType = "Vector";
      
      if (value[0] == '[')
      {
         PutRvector3Value(mViewPointRefVector, id, value);
         mViewPointRefType = "Vector";
      }
      return true;
   case VIEWPOINT_REFERENCE:
      mViewPointRefName = value;
      mViewPointRefType = "Object";
      
      // Handle deprecated value "Vector"
      if (value == "Vector" || GmatStringUtil::IsNumber(value))
         mViewPointRefType = "Vector";
      
      if (value[0] == '[')
      {
         PutRvector3Value(mViewPointRefVector, id, value);
         mViewPointRefType = "Vector";
      }
   case VIEWPOINT_REF_TYPE:
      mViewPointRefType = value;
      return true;
   case VIEWPOINT_VECTOR:
      mViewPointVecName = value;
      mViewPointVecType = "Object";
      
      // Handle deprecated value "Vector"
      if (value == "Vector" || GmatStringUtil::IsNumber(value))
         mViewPointVecType = "Vector";
      
      if (value[0] == '[')
      {
         PutRvector3Value(mViewPointVecVector, id, value);
         mViewPointVecType = "Vector";
      }
      return true;
   case VIEWPOINT_VECTOR_TYPE:
      mViewPointVecType = value;
      return true;
   case VIEW_DIRECTION:
      mViewDirectionName = value;
      mViewDirectionType = "Object";
      
      // Handle deprecated value "Vector"
      if (value == "Vector" || GmatStringUtil::IsNumber(value))
         mViewDirectionType = "Vector";
      
      if (value[0] == '[')
      {
         PutRvector3Value(mViewDirectionVector, id, value);
         mViewDirectionType = "Vector";
      }
      return true;
   case VIEW_DIRECTION_TYPE:
      mViewDirectionType = value;
      return true;
   case VIEW_UP_COORD_SYSTEM:
      mViewUpCoordSysName = value;
      return true;
   case VIEW_UP_AXIS:
      mViewUpAxisName = value;
      return true;
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetStringParameter()<%s> label=%s, value=%s \n",
       instanceName.c_str(), label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetStringParameter()<%s> id=%d<%s>, value=%s, index= %d\n",
       instanceName.c_str(), id, GetParameterText(id).c_str(), value.c_str(), index);
   #endif
   
   switch (id)
   {
   case ADD:
      return AddSpacePoint(value, index);
   case VIEWPOINT_REF:
      WriteDeprecatedMessage(id);
      mViewPointRefType = "Vector";
      PutRvector3Value(mViewPointRefVector, id, value, index);
      return true;
   case VIEWPOINT_REFERENCE:
      mViewPointRefType = "Vector";
      PutRvector3Value(mViewPointRefVector, id, value, index);
      return true;
   case VIEWPOINT_VECTOR:
      mViewPointVecType = "Vector";
      PutRvector3Value(mViewPointVecVector, id, value, index);
      return true;
   case VIEW_DIRECTION:
      mViewDirectionType = "Vector";
      PutRvector3Value(mViewDirectionVector, id, value, index);
      return true;
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetStringParameter(const std::string &label,
                                    const std::string &value,
                                    const Integer index)
{
   #if DBGLVL_OPENGL_PARAM_STRING
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetStringParameter() label = %s, value = %s, index = %d\n",
       label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// virtual const UnsignedIntArray&
// GetUnsignedIntArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const UnsignedIntArray&
OpenGlPlot::GetUnsignedIntArrayParameter(const Integer id) const
{   
   switch (id)
   {
   case ORBIT_COLOR:
      return mOrbitColorArray;
   case TARGET_COLOR:
      return mTargetColorArray;
   default:
      return Subscriber::GetUnsignedIntArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                             const UnsignedInt value,
//                                             const Integer index)
//------------------------------------------------------------------------------
UnsignedInt OpenGlPlot::SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetUnsignedIntParameter() this=%s\n   id=%d, value=%u, index=%d, "
       "mAllSpCount=%d, mOrbitColorArray.size()=%d, mTargetColorArray.size()=%d\n",
       instanceName.c_str(), id, value, index, mAllSpCount, mOrbitColorArray.size(),
       mTargetColorArray.size());
   #endif
   
   switch (id)
   {
   case ORBIT_COLOR:
      {
         Integer size = mAllSpNameArray.size();
         if (index >= size)
            throw SubscriberException
               ("index out of bounds for " + GetParameterText(id));
         
         for (int i=0; i<size; i++)
         {
            if (index == i)
               mOrbitColorMap[mAllSpNameArray[i]] = value;
            
            if (index < size)
               mOrbitColorArray[index] = value;
            else
               mOrbitColorArray.push_back(value);
         }
         return value;
      }
   case TARGET_COLOR:
      {
         Integer size = mAllSpNameArray.size();
         if (index >= size)
            throw SubscriberException
               ("index out of bounds for " + GetParameterText(id));
         
         for (int i=0; i<size; i++)
         {
            if (index == i)
               mTargetColorMap[mAllSpNameArray[i]] = value;
            
            if (index < size)
               mTargetColorArray[index] = value;
            else
               mTargetColorArray.push_back(value);
         }
         return value;
      }
   default:
      return Subscriber::SetUnsignedIntParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD:
      return mAllSpNameArray;
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool OpenGlPlot::GetBooleanParameter(const Integer id) const
{
   if (id == SHOW_PLOT)
      return active;
   return Subscriber::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetBooleanParameter(const Integer id, const bool value)
{
   #if DBGLVL_OPENGL_PARAM
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetBooleanParameter()<%s> id=%d, value=%d\n",
       instanceName.c_str(), id, value);
   #endif
   
   if (id == SHOW_PLOT)
   {
      active = value;
      return active;
   }
   return Subscriber::SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string OpenGlPlot::GetOnOffParameter(const Integer id) const
{
   switch (id)
   {
   case CELESTIAL_PLANE:
      return mEclipticPlane;
   case XY_PLANE:
      return mXYPlane;
   case WIRE_FRAME:
      return mWireFrame;
   case AXES:
      return mAxes;
   case GRID:
      return mGrid;
   case EARTH_SUN_LINES:
      return mSunLine;
   case SUN_LINE:
      return mSunLine;
   case OVERLAP_PLOT:
      return mOverlapPlot;
   case USE_INITIAL_VIEW:
      return mUseInitialView;
   case PERSPECTIVE_MODE:
      return mPerspectiveMode;
   case USE_FIXED_FOV:
      return mUseFixedFov;
        case ENABLE_STARS:
                return mEnableStars;
        case ENABLE_CONSTELLATIONS:
                return mEnableConstellations;
   default:
      return Subscriber::GetOnOffParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string OpenGlPlot::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool OpenGlPlot::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case CELESTIAL_PLANE:
      mEclipticPlane = value;
      return true;
   case XY_PLANE:
      mXYPlane = value;
      return true;
   case WIRE_FRAME:
      mWireFrame = value;
      return true;
   case AXES:
      mAxes = value;
      return true;
   case GRID:
      mGrid = value;
      return true;
   case EARTH_SUN_LINES:
      WriteDeprecatedMessage(id);
      mSunLine = value;
      return true;
   case SUN_LINE:
      mSunLine = value;
      return true;
   case OVERLAP_PLOT:
      mOverlapPlot = value;
      return true;
   case USE_INITIAL_VIEW:
      mUseInitialView = value;
      return true;
   case PERSPECTIVE_MODE:
      mPerspectiveMode = value;
      return true;
   case USE_FIXED_FOV:
      mUseFixedFov = value;
      return true;
        case ENABLE_STARS:
                mEnableStars = value;
                return true;
        case ENABLE_CONSTELLATIONS:
                mEnableConstellations = value;
                return true;
   default:
      return Subscriber::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool OpenGlPlot::SetOnOffParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string OpenGlPlot::GetRefObjectName(const UnsignedInt type) const
{
   #if DBGLVL_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectName() type: %s\n",
       GmatBase::GetObjectTypeString(type).c_str());
   #endif
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      return mViewCoordSysName; //just return this
   }
   
   #if DBGLVL_OPENGL_OBJ
   std::string msg = "type: " + GmatBase::GetObjectTypeString(type) + " not found";
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectName() %s\n", msg.c_str());
   #endif
   
   return Subscriber::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool OpenGlPlot::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& OpenGlPlot::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& OpenGlPlot::GetRefObjectNameArray(const UnsignedInt type)
{
   mAllRefObjectNames.clear();
   
   // if Draw Earth-Sun lines is on, add Earth and Sun
   if (mSunLine == "On")
   {
      AddSpacePoint("Earth", mAllSpCount, false);
      AddSpacePoint("Sun", mAllSpCount, false);
   }
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      mAllRefObjectNames.push_back(mViewCoordSysName);
      mAllRefObjectNames.push_back(mViewUpCoordSysName);
   }
   else if (type == Gmat::SPACE_POINT)
   {
      mAllRefObjectNames = mAllSpNameArray;
      
      if (mViewPointRefType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointRefName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointRefName);
      }
      
      if (mViewPointVecType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointVecName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointVecName);
      }
      
      if (mViewDirectionType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewDirectionName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewDirectionName);
      }
   }
   else if (type == Gmat::UNKNOWN_OBJECT)
   {
      #ifdef DEBUG_OPENGL_OBJ
      MessageInterface::ShowMessage
         ("mViewPointRefType=%s, mViewPointVecType=%s, mViewDirectionType=%s\n",
          mViewPointRefType.c_str(), mViewPointVecType.c_str(), mViewDirectionType.c_str());
      #endif
      
      mAllRefObjectNames = mAllSpNameArray;
      
      mAllRefObjectNames.push_back(mViewCoordSysName);
      
      if (mViewCoordSysName != mViewUpCoordSysName)
         mAllRefObjectNames.push_back(mViewUpCoordSysName);
      
      if (mViewPointRefType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointRefName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointRefName);
      }
      
      if (mViewPointVecType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewPointVecName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewPointVecName);
      }
      
      if (mViewDirectionType != "Vector")
      {
         if (find(mAllRefObjectNames.begin(), mAllRefObjectNames.end(),
                  mViewDirectionName) == mAllRefObjectNames.end())
            mAllRefObjectNames.push_back(mViewDirectionName);
      }
   }
   
   #if DBGLVL_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::GetRefObjectNameArray() returning for type:%d\n", type);
   for (unsigned int i=0; i<mAllRefObjectNames.size(); i++)
      MessageInterface::ShowMessage("   %s\n", mAllRefObjectNames[i].c_str());
   #endif
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* OpenGlPlot::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (name == mViewCoordSysName)
         return mViewCoordSystem;
      if (name == mViewUpCoordSysName)
         return mViewUpCoordSystem;
   }
   else if (type == Gmat::SPACE_POINT)
   {
      if (name == mViewPointRefName)
         return mViewPointRefObj;
      else if (name == mViewPointVecName)
         return mViewPointObj;
      else if (name == mViewDirectionName)
         return mViewDirectionObj;
   }
   
   return Subscriber::GetRefObject(type, name);
   
//    throw SubscriberException("OpenGlPlot::GetRefObject() the object name: " + name +
//                            "not found\n");
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Set reference object pointer.
 *
 * @param <obj>  Reference object pointer to set to given object type and name
 * @param <type> Reference object type
 * @param <name> Reference object name
 */
//------------------------------------------------------------------------------
bool OpenGlPlot::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   #if DBGLVL_OPENGL_OBJ
   MessageInterface::ShowMessage
      ("OpenGlPlot::SetRefObject() this=<%p>'%s', obj=<%p>'%s', type=%d[%s], name='%s'\n",
       this, GetName().c_str(), obj, obj->GetName().c_str(), type,
       obj->GetTypeName().c_str(), name.c_str());
   #endif
   
   std::string realName = name;
   if (name == "")
      realName = obj->GetName();
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (realName == mViewCoordSysName)
         mViewCoordSystem = (CoordinateSystem*)obj;
      if (realName == mViewUpCoordSysName)
         mViewUpCoordSystem = (CoordinateSystem*)obj;
      return true;
   }
   else if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      #if DBGLVL_OPENGL_OBJ
      MessageInterface::ShowMessage("   mAllSpCount=%d\n", mAllSpCount);
      #endif
      
      for (Integer i=0; i<mAllSpCount; i++)
      {
         #if DBGLVL_OPENGL_OBJ
         MessageInterface::ShowMessage
            ("   mAllSpNameArray[%d]='%s'\n", i, mAllSpNameArray[i].c_str());
         #endif
         
         if (mAllSpNameArray[i] == realName)
         {
            #if DBGLVL_OPENGL_OBJ > 1
            MessageInterface::ShowMessage
               ("   Setting object to '%s'\n", mAllSpNameArray[i].c_str());
            #endif
            
            mAllSpArray[i] = (SpacePoint*)(obj);
         }
      }
      
      #if DBGLVL_OPENGL_OBJ
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetRefObject() realName='%s', mViewPointRefName='%s', "
          "mViewPointVecName='%s', mViewDirectionName='%s'\n", realName.c_str(),
          mViewPointRefName.c_str(), mViewPointVecName.c_str(),
          mViewDirectionName.c_str());
      #endif
      
      // ViewPoint info
      if (realName == mViewPointRefName)
         mViewPointRefObj = (SpacePoint*)obj;
      
      if (realName == mViewPointVecName)
         mViewPointObj = (SpacePoint*)obj;
      
      if (realName == mViewDirectionName)
         mViewDirectionObj = (SpacePoint*)obj;
      
      #if DBGLVL_OPENGL_OBJ
      MessageInterface::ShowMessage
         ("OpenGlPlot::SetRefObject() mViewPointRefObj=<%p>, mViewPointObj=<%p>, "
          "mViewDirectionObj=<%p>\n", mViewPointRefObj, mViewPointObj,
          mViewDirectionObj);
      #endif
      return true;
   }
   
   return Subscriber::SetRefObject(obj, type, realName);
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddSpacePoint(const std::string &name, Integer index, bool show = true)
//------------------------------------------------------------------------------
bool OpenGlPlot::AddSpacePoint(const std::string &name, Integer index, bool show)
{
   #if DBGLVL_OPENGL_ADD
   MessageInterface::ShowMessage
      ("OpenGlPlot::AddSpacePoint()<%s> name=%s, index=%d, show=%d, mAllSpCount=%d\n",
       instanceName.c_str(), name.c_str(), index, show, mAllSpCount);
   #endif
   
   // if name not in the list, add
   if (find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name) ==
       mAllSpNameArray.end())
   {
      if (name != "" && index == mAllSpCount)
      {
         mAllSpNameArray.push_back(name);
         mAllSpArray.push_back(NULL);
         mAllSpCount = mAllSpNameArray.size();
         
         mDrawOrbitMap[name] = show;
         mShowObjectMap[name] = show;
         
         if (mAllSpCount < MAX_SP_COLOR)
         {
            // If object is non-standard-body, use mNonStdBodyCount.
            // So that spacecraft color starts from DEFAULT_ORBIT_COLOR
            if (mOrbitColorMap.find(name) == mOrbitColorMap.end())
            {
               mOrbitColorMap[name] = DEFAULT_ORBIT_COLOR[mNonStdBodyCount];
               mTargetColorMap[name] = GmatColor::TEAL32;
               mOrbitColorArray.push_back(DEFAULT_ORBIT_COLOR[mNonStdBodyCount]);
               mTargetColorArray.push_back(GmatColor::TEAL32);
               mNonStdBodyCount++;
            }
            else
            {
               mOrbitColorArray.push_back(mOrbitColorMap[name]);
               mTargetColorArray.push_back(mTargetColorMap[name]);
            }
         }
         else
         {
            mOrbitColorMap[name] = GmatColor::RED32;
            mTargetColorMap[name] = GmatColor::TEAL32;
            mOrbitColorArray.push_back(GmatColor::RED32);
            mTargetColorArray.push_back(GmatColor::TEAL32);
         }
      }
   }
   
   #if DBGLVL_OPENGL_ADD   
   std::string objName;
   for (int i=0; i<mAllSpCount; i++)
   {
      objName = mAllSpNameArray[i];
      MessageInterface::ShowMessage
         ("   mAllSpNameArray[%d]=%s, draw=%d, show=%d "
          "orbColor=%u, targColor=%u\n", i, objName.c_str(), mDrawOrbitMap[objName],
          mShowObjectMap[objName], mOrbitColorMap[objName], mTargetColorMap[objName]);
      MessageInterface::ShowMessage
         ("   mOrbitColorArray[%d]=%u, mTargetColorArray[%d]=%u\n", i, mOrbitColorArray[i],
          i, mTargetColorArray[i]);
   }
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ClearSpacePointList()
//------------------------------------------------------------------------------
bool OpenGlPlot::ClearSpacePointList()
{
   //MessageInterface::ShowMessage("OpenGlPlot::ClearSpacePointList()\n");
   
   mAllSpNameArray.clear();
   mAllSpArray.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mShowObjectArray.clear();
   mScNameArray.clear();
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
   mOrbitColorMap.clear();
   mTargetColorMap.clear();
   mAllSpCount = 0;
   mScCount = 0;
   mObjectCount = 0;
   mNonStdBodyCount = 0;
   
   return true;
}


//------------------------------------------------------------------------------
// bool RemoveSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes spacecraft from the spacecraft list
 *
 * @param <name> spacecraft name to be removed from the list
 *
 * @return true if spacecraft was removed from the list, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool OpenGlPlot::RemoveSpacePoint(const std::string &name)
{
   //-----------------------------------------------------------------
   #ifdef __REMOVE_OBJ_BY_SETTING_FLAG__
   //-----------------------------------------------------------------

   for (UnsignedInt i=0; i<mObjectNameArray.size(); i++)
   {
      if (mObjectNameArray[i] == name)
      {
         mDrawOrbitArray[i] = false;
         PlotInterface::SetGlDrawOrbitFlag(instanceName, mDrawOrbitArray);
         return true;
      }
   }
   
   return false;
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   bool removedFromScArray = false;
   bool removedFromAllSpArray = false;
   
   //-------------------------------------------------------
   // remove from mScNameArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mScNameArray:\n", name.c_str());
   MessageInterface::ShowMessage("mScCount=%d\n", mScCount);
   for (int i=0; i<mScCount; i++)
   {
      MessageInterface::ShowMessage
         ("mScNameArray[%d]=%s\n", i, mScNameArray[i].c_str());
   }
   #endif
   
   StringArray::iterator scPos = 
      find(mScNameArray.begin(), mScNameArray.end(), name);
   
   if (scPos != mScNameArray.end())
   {
      MessageInterface::ShowMessage("sc to be erased=%s\n", (*scPos).c_str());
      
      // erase given spacecraft from the arrays
      mScNameArray.erase(scPos);
      
      // just reduce the size of array
      mScOrbitColorArray.erase(mScOrbitColorArray.begin());
      mScTargetColorArray.erase(mScTargetColorArray.begin());
      mScXArray.erase(mScXArray.begin());
      mScYArray.erase(mScYArray.begin());
      mScZArray.erase(mScZArray.begin());
      mScVxArray.erase(mScVxArray.begin());
      mScVyArray.erase(mScVyArray.begin());
      mScVzArray.erase(mScVzArray.begin());
      
      mScCount = mScNameArray.size();
      
      // update color array
      for (int i=0; i<mScCount; i++)
      {
         mScOrbitColorArray[i] = mOrbitColorMap[mScNameArray[i]];
         mScTargetColorArray[i] = mTargetColorMap[mScNameArray[i]];
      }
      
      #if DBGLVL_REMOVE_SP
      MessageInterface::ShowMessage("---After remove from mScNameArray:\n");
      MessageInterface::ShowMessage("mScCount=%d\n", mScCount);
      for (int i=0; i<mScCount; i++)
      {
         MessageInterface::ShowMessage
            ("mScNameArray[%d]=%s\n", i, mScNameArray[i].c_str());
      }
      #endif
      
      removedFromScArray = true;
      //return true;
   }
   
   
   //-------------------------------------------------------
   // remove from mAllSpNameArray and mObjectNameArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mAllSpNameArray:\n", name.c_str());
   MessageInterface::ShowMessage("mAllSpCount=%d\n", mAllSpCount);
   for (int i=0; i<mAllSpCount; i++)
   {
      MessageInterface::ShowMessage
         ("mAllSpNameArray[%d]=%s\n", i, mAllSpNameArray[i].c_str());
   }
   #endif
   
   StringArray::iterator spPos = 
      find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name);
   StringArray::iterator objPos = 
      find(mObjectNameArray.begin(), mObjectNameArray.end(), name);
   
   if (spPos != mAllSpNameArray.end() && objPos != mObjectNameArray.end())
   {
      std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
      orbColorPos = mOrbitColorMap.find(name);
      targColorPos = mTargetColorMap.find(name);
      
      if (orbColorPos != mOrbitColorMap.end() &&
          targColorPos != mTargetColorMap.end())
      {
         // erase given spacecraft name
         mAllSpNameArray.erase(spPos);
         mObjectNameArray.erase(objPos);
         mOrbitColorMap.erase(orbColorPos);
         mTargetColorMap.erase(targColorPos);
         
         // reduce the size of array
         mOrbitColorArray.erase(mOrbitColorArray.begin());
         mTargetColorArray.erase(mTargetColorArray.begin());
         
         mAllSpCount = mAllSpNameArray.size();
         
         // update color array
         for (int i=0; i<mAllSpCount; i++)
         {
            mOrbitColorArray[i] = mOrbitColorMap[mAllSpNameArray[i]];
            mTargetColorArray[i] = mTargetColorMap[mAllSpNameArray[i]];
         }
         
         #if DBGLVL_REMOVE_SP
         MessageInterface::ShowMessage("---After remove from mAllSpNameArray\n");
         MessageInterface::ShowMessage("mAllSpCount=%d\n", mAllSpCount);
         for (int i=0; i<mAllSpCount; i++)
         {
            MessageInterface::ShowMessage
               ("mAllSpNameArray[%d]=%s\n", i, mAllSpNameArray[i].c_str());
         }
         #endif
         
         removedFromAllSpArray = true;
      }
   }

   //-------------------------------------------------------
   // remove from mObjectArray
   //-------------------------------------------------------
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacePoint() name=%s\n--- Before remove from "
       "mObjectArray:\n", name.c_str());
   MessageInterface::ShowMessage("size=%d\n", mObjectArray.size());
   #endif
   
   for (std::vector<SpacePoint*>::iterator objptPos = mObjectArray.begin();
        objptPos != mObjectArray.end(); ++objptPos)
   {
      MessageInterface::ShowMessage
         ("mObjectArray=%s\n", (*objptPos)->GetName().c_str());
      if ((*objptPos)->GetName() == name)
      {
         mObjectArray.erase(objptPos);
         break;
      }
   }
   
   #if DBGLVL_REMOVE_SP
   MessageInterface::ShowMessage
      ("OpenGlPlot::RemoveSpacePoint() name=%s\n--- After remove from "
       "mObjectArray:\n", name.c_str());
   MessageInterface::ShowMessage("size=%d\n", mObjectArray.size());
   #endif
   
   if (removedFromScArray && removedFromAllSpArray)
      // set all object array and pointers
      PlotInterface::SetGlObject(instanceName, mObjectNameArray,
                                 mOrbitColorArray, mObjectArray);
   
   return (removedFromScArray && removedFromAllSpArray);
   
   #endif
}


//------------------------------------------------------------------------------
// void ClearDynamicArrays()
//------------------------------------------------------------------------------
void OpenGlPlot::ClearDynamicArrays()
{
   mObjectNameArray.clear();
   mOrbitColorArray.clear();
   mTargetColorArray.clear();
   mObjectArray.clear();
   mDrawOrbitArray.clear();
   mShowObjectArray.clear();
   mScNameArray.clear();
   mScOrbitColorArray.clear();
   mScTargetColorArray.clear();
   mScXArray.clear();
   mScYArray.clear();
   mScZArray.clear();
   mScVxArray.clear();
   mScVyArray.clear();
   mScVzArray.clear();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(SpacePoint *sp, bool show = false)
//------------------------------------------------------------------------------
/**
 * Add non-spacecraft object to the list.
 */
//------------------------------------------------------------------------------
void OpenGlPlot::UpdateObjectList(SpacePoint *sp, bool show)
{   
   // Add all spacepoint objects
   std::string name = sp->GetName();
   StringArray::iterator pos = 
      find(mObjectNameArray.begin(), mObjectNameArray.end(), name);
   
   // if name not found, add to arrays
   if (pos == mObjectNameArray.end())
   {
      mObjectNameArray.push_back(name);
      mOrbitColorArray.push_back(mOrbitColorMap[name]);
      mTargetColorArray.push_back(mTargetColorMap[name]);
      mObjectArray.push_back(sp);
      mDrawOrbitMap[name] = show;
      mShowObjectMap[name] = show;
      mDrawOrbitArray.push_back(show);
      mShowObjectArray.push_back(show);
      mObjectCount = mObjectNameArray.size();
   }
   
   #if DBGLVL_OPENGL_INIT > 1
   Integer draw, showObj;
   MessageInterface::ShowMessage
      ("OpenGlPlot::UpdateObjectList() instanceName=%s\n", instanceName.c_str());
   for (int i=0; i<mObjectCount; i++)
   {
      draw = mDrawOrbitArray[i] ? 1 : 0;
      showObj = mShowObjectArray[i] ? 1 : 0;
      MessageInterface::ShowMessage
         ("   mObjectNameArray[%d]=%s, draw=%d, show=%d, color=%d\n", i,
          mObjectNameArray[i].c_str(), draw, showObj, mOrbitColorArray[i]);
   }
   #endif
}


//------------------------------------------------------------------------------
// void PutRvector3Value(Rvector3 &rvec3, Integer id,
//                       const std::string &sval, Integer index = -1);
//------------------------------------------------------------------------------
/*
 * Converts input string to Real and store as Rvector3 element at idnex.
 *
 * @param rvec3  input Rvector3 where value to be stored
 * @param id     input Parameter ID used for formating error message
 * @param sval   input string value
 * @param index  input index to be used for storing a Rvector3 element
 *               if index is -1, whole 3 elements are converted and stored
 *               from a string format of "[element1 element2 element3]"
 */
//------------------------------------------------------------------------------
void OpenGlPlot::PutRvector3Value(Rvector3 &rvec3, Integer id,
                                  const std::string &sval, Integer index)
{
   #if DBGLVL_OPENGL_PARAM_RVEC3
   MessageInterface::ShowMessage
      ("OpenGlPlot::PutRvector3Value() id=%d, sval=%s, index=%d\n",
       id, sval.c_str(), index);
   #endif
   
   std::string badVal;
   bool isValid = true;
   std::string field = GetParameterText(id);
   
   // Check index, throw exception if out of bound
   if (index < -1 || index > 2)
   {
      badVal = sval;
      isValid = false;
   }
   
   if (isValid)
   {
      // Convert string value to Real
      if (index != -1)
      {
         Real rval;
         if (GmatStringUtil::ToReal(sval, rval))
            rvec3[index] = rval;
         else
         {
            isValid = false;
            badVal = sval;
         }
      }
      else if (index == -1)
      {
         StringArray valArray;
         std::string svalue = sval;
         svalue = GmatStringUtil::Trim(svalue);
         std::string::size_type index1 = svalue.find_first_of("[");
         if (index1 != svalue.npos)
         {
            std::string::size_type index2 = svalue.find_last_of("]");
            if (index2 != svalue.npos)
            {
               svalue = svalue.substr(index1+1, index2-index1-1);
            }
            else
            {
               isValid = false;
               badVal = sval;
            }
         }
         
         valArray = GmatStringUtil::SeparateBy(svalue, " ,");         
         int arraySize = valArray.size();
         
         if (arraySize == 3)
         {
            Real rvals[3];
            bool rvalsOk[3];
            rvalsOk[0] = GmatStringUtil::ToReal(valArray[0], rvals[0]);
            rvalsOk[1] = GmatStringUtil::ToReal(valArray[1], rvals[1]);
            rvalsOk[2] = GmatStringUtil::ToReal(valArray[2], rvals[2]);
            
            // Detects first invalid input and throw exception
            if (!rvalsOk[0])
               badVal = valArray[0];
            else if (!rvalsOk[1])
               badVal = valArray[1];
            else if (!rvalsOk[2])
               badVal = valArray[2];
            
            if (rvalsOk[0] && rvalsOk[1] && rvalsOk[2])
               rvec3.Set(rvals[0], rvals[1], rvals[2]);
            else
               isValid = false;
         }
         else
         {
            isValid = false;
            badVal = sval;
         }
      }
   }
   
   if (!isValid)
   {
      SubscriberException se;
      se.SetDetails(errorMessageFormat.c_str(), badVal.c_str(), field.c_str(),
                    "SpacecraftName, CelestialBodyName, LibrationPointName, "
                    "BarycenterName, or a 3-vector of numerical values");
      throw se;
   }
}


//------------------------------------------------------------------------------
// void PutUnsignedIntValue(Integer id, const std::string &sval)
//------------------------------------------------------------------------------
void OpenGlPlot::PutUnsignedIntValue(Integer id, const std::string &sval)
{
   #ifdef DEBUG_OPENGL_PUT
   MessageInterface::ShowMessage
      ("PutUnsignedIntValue() id=%d, sval='%s'\n", id, sval.c_str());
   #endif
   
   UnsignedIntArray vals = GmatStringUtil::ToUnsignedIntArray(sval);
   for (UnsignedInt i=0; i<vals.size(); i++)
      SetUnsignedIntParameter(id, vals[i], i);
}


//------------------------------------------------------------------------------
// void WriteDeprecatedMessage(Integer id) const
//------------------------------------------------------------------------------
void OpenGlPlot::WriteDeprecatedMessage(Integer id) const
{
   // Write only one message per session
   static bool writeEarthSunLines = true;
   static bool writeViewpointRef = true;
   static bool writeViewpointRefVector = true;
   static bool writeViewpointVectorVector = true;
   static bool writeViewDirectionVector = true;
   
   switch (id)
   {
   case EARTH_SUN_LINES:
      if (writeEarthSunLines)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"EarthSunLines\" is deprecated and will be "
             "removed from a future build; please use \"SunLine\" "
             "instead.\n");
         writeEarthSunLines = false;
      }
      break;
   case VIEWPOINT_REF:
      if (writeViewpointRef)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"ViewPointRef\" is deprecated and will be "
             "removed from a future build; please use \"ViewPointReference\" "
             "instead.\n");
         writeViewpointRef = false;
      }
      break;
   case VIEWPOINT_REF_VECTOR:
      if (writeViewpointRefVector)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"ViewPointRefVector\" is deprecated and will be "
             "removed from a future build.\n");
         writeViewpointRefVector = false;
      }
      break;
   case VIEWPOINT_VECTOR_VECTOR:
      if (writeViewpointVectorVector)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"ViewPointVectorVector\" is deprecated and will be "
             "removed from a future build.\n");
         writeViewpointVectorVector = false;
      }
      break;
   case VIEW_DIRECTION_VECTOR:
      if (writeViewDirectionVector)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"ViewDirectionVector\" is deprecated and will be "
             "removed from a future build.\n");
         writeViewDirectionVector = false;
      }
      break;
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// bool UpdateSolverData()
//------------------------------------------------------------------------------
bool OpenGlPlot::UpdateSolverData()
{
   int size = mCurrEpochArray.size();
   int last = size - 1;
   
   #if DBGLVL_SOLVER_CURRENT_ITER
   MessageInterface::ShowMessage("===> num buffered data = %d\n", size);
   MessageInterface::ShowMessage("==========> now update solver plot\n");
   #endif
   
   if (size == 0)
      return true;
   
   UnsignedIntArray colorArray = mScOrbitColorArray;
   if (runstate == Gmat::SOLVING)
      colorArray = mScTargetColorArray;
   else
      colorArray = mScOrbitColorArray;
   
   // Update plot with last iteration data
   for (int i=0; i<size-1; i++)
   {
      #if DBGLVL_SOLVER_CURRENT_ITER > 1
      for (int sc=0; sc<mScCount; sc++)
         MessageInterface::ShowMessage
            ("   i=%d, sc=%d, solver epoch = %f, X,Y,Z = %f, %f, %f\n", i, sc,
             mCurrEpochArray[i], mCurrXArray[i][sc], mCurrYArray[i][sc],
             mCurrZArray[i][sc]);
      #endif
      
      // Just buffer data up to last point - 1
      PlotInterface::
         UpdateGlPlot(instanceName, mOldName, mCurrScArray[i],
                      mCurrEpochArray[i], mCurrXArray[i], mCurrYArray[i],
                      mCurrZArray[i], mCurrVxArray[i], mCurrVyArray[i],
                      mCurrVzArray[i], colorArray, true, mSolverIterOption, false);
   }
   
   // Buffer last point and Update the plot
   PlotInterface::
      UpdateGlPlot(instanceName, mOldName, mCurrScArray[last],
                   mCurrEpochArray[last], mCurrXArray[last], mCurrYArray[last],
                   mCurrZArray[last], mCurrVxArray[last], mCurrVyArray[last],
                   mCurrVzArray[last], colorArray, true, mSolverIterOption, true);
   
   // clear arrays
   mCurrScArray.clear();
   mCurrEpochArray.clear();
   mCurrXArray.clear();
   mCurrYArray.clear();
   mCurrZArray.clear();
   mCurrVxArray.clear();
   mCurrVyArray.clear();
   mCurrVzArray.clear();
   
   if (runstate == Gmat::SOLVING)
      PlotInterface::TakeGlAction(instanceName, "ClearSolverData");
   
   return true;
}


//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(int len)
{
   //loj: How do I convert data to Real data?
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool OpenGlPlot::Distribute(const Real *dat, Integer len)
{
   #if DBGLVL_OPENGL_UPDATE
   MessageInterface::ShowMessage
      ("OpenGlPlot::Distribute() instanceName=%s, active=%d, isEndOfRun=%d, "
       "isEndOfReceive=%d\n   mAllSpCount=%d, mScCount=%d, len=%d, runstate=%d\n",
       instanceName.c_str(), active, isEndOfRun, isEndOfReceive, mAllSpCount,
       mScCount, len, runstate);
   #endif
   
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)
      return true;
   
   if (!active || mScCount <= 0)
      return true;
   
   // test isEndOfRun first
   if (isEndOfRun)
      return PlotInterface::SetGlEndOfRun(instanceName);
   
   if (isEndOfReceive)
   {
      if ((mSolverIterOption == SI_CURRENT) &&
          (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS))
      {
         UpdateSolverData();
      }
      else
      {
         return PlotInterface::RefreshGlPlot(instanceName);
      }
   }
   
   
   if (len <= 0)
      return true;
   
   
   #if DBGLVL_OPENGL_DATA
   MessageInterface::ShowMessage("%s, len=%d\n", GetName().c_str(), len);
   for (int i=0; i<len; i++)
      MessageInterface::ShowMessage("%.11f  ", dat[i]);
   MessageInterface::ShowMessage("\n");
   #endif
   
   //------------------------------------------------------------
   // if targeting and draw target is None, just return
   //------------------------------------------------------------
   if ((mSolverIterOption == SI_NONE) && (runstate == Gmat::SOLVING))
   {
      #if DBGLVL_OPENGL_UPDATE > 1
      MessageInterface::ShowMessage
         ("   Just returning: SolverIterations is %d and runstate is %d\n",
          mSolverIterOption, runstate);
      #endif
      
      return true;
   }
   
   //------------------------------------------------------------
   // update plot data
   //------------------------------------------------------------
   
   CoordinateConverter coordConverter;
   mNumData++;
   
   #if DBGLVL_OPENGL_UPDATE > 1
   MessageInterface::ShowMessage
      ("   mNumData=%d, mDataCollectFrequency=%d, currentProvider=<%p>\n",
       mNumData, mDataCollectFrequency, currentProvider);
   #endif
   
   if ((mNumData % mDataCollectFrequency) == 0)
   {
      mNumData = 0;
      mNumCollected++;
      bool update = (mNumCollected % mUpdatePlotFrequency) == 0;
      
      #if DBGLVL_OPENGL_UPDATE > 1
      MessageInterface::ShowMessage
         ("   currentProvider=%d, theDataLabels.size()=%d\n",
          currentProvider, theDataLabels.size());
      #endif
      
      #if DBGLVL_OPENGL_UPDATE > 2
      MessageInterface::ShowMessage
         ("OpenGlPlot::Distribute() Using new Publisher code\n");
      #endif
      
      // @note
      // New Publisher code doesn't assign currentProvider anymore,
      // it just copies current labels. There was an issue with
      // provider id keep incrementing if data is regisgered and
      // published inside a GmatFunction
      StringArray dataLabels = theDataLabels[0];
            
      #if DBGLVL_OPENGL_DATA_LABELS
      MessageInterface::ShowMessage("   Data labels for %s =\n   ", GetName().c_str());
      for (int j=0; j<(int)dataLabels.size(); j++)
         MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
      MessageInterface::ShowMessage("\n");
      #endif
      
      Integer idX, idY, idZ;
      Integer idVx, idVy, idVz;
      Integer scIndex = -1;
      
      for (int i=0; i<mScCount; i++)
      {
         idX = FindIndexOfElement(dataLabels, mScNameArray[i]+".X");
         idY = FindIndexOfElement(dataLabels, mScNameArray[i]+".Y");
         idZ = FindIndexOfElement(dataLabels, mScNameArray[i]+".Z");
         
         idVx = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vx");
         idVy = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vy");
         idVz = FindIndexOfElement(dataLabels, mScNameArray[i]+".Vz");
         
         #if DBGLVL_OPENGL_DATA_LABELS
         MessageInterface::ShowMessage
            ("   mScNameArray[%d]=%s, idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d\n",
             i, mScNameArray[i].c_str(), idX, idY, idZ, idVx, idVy, idVz);
         #endif
         
         // if any of index not found, continue with next spacecraft name
         if (idX  == -1 || idY  == -1 || idZ  == -1 ||
             idVx == -1 || idVy == -1 || idVz == -1)
            continue;
         
         scIndex++;
         
         // buffer data
         for (int sc=0; sc<mScCount; sc++)
         {
            // If distributed data coordinate system is different from view
            // coordinate system, convert data here.
            // if we convert after current epoch, it will not give correct
            // results, if origin is spacecraft,
            // ie, sat->GetMJ2000State(epoch) will not give correct results.
            
            #if DBGLVL_OPENGL_DATA
            MessageInterface::ShowMessage
               ("   %s, %.11f, X,Y,Z = %f, %f, %f\n", GetName().c_str(), dat[0],
                dat[idX], dat[idY], dat[idZ]);
            #endif
            
            if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
                (mViewCoordSystem != theDataCoordSystem))
            {
               Rvector6 inState, outState;
               
               // convert position and velocity
               inState.Set(dat[idX], dat[idY], dat[idZ],
                           dat[idVx], dat[idVy], dat[idVz]);
               
               coordConverter.Convert(dat[0], inState, theDataCoordSystem,
                                      outState, mViewCoordSystem);
               
               mScXArray[scIndex] = outState[0];
               mScYArray[scIndex] = outState[1];
               mScZArray[scIndex] = outState[2];
               mScVxArray[scIndex] = outState[3];
               mScVyArray[scIndex] = outState[4];
               mScVzArray[scIndex] = outState[5];
            }
            else
            {
               mScXArray[scIndex] = dat[idX];
               mScYArray[scIndex] = dat[idY];
               mScZArray[scIndex] = dat[idZ];
               mScVxArray[scIndex] = dat[idVx];
               mScVyArray[scIndex] = dat[idVy];
               mScVzArray[scIndex] = dat[idVz];
            }
            
            #if DBGLVL_OPENGL_DATA
            MessageInterface::ShowMessage
               ("   after buffering, scNo=%d, scIndex=%d, X,Y,Z = %f, %f, %f\n",
                i, scIndex, mScXArray[scIndex], mScYArray[scIndex], mScZArray[scIndex]);
            #endif
            
            #if DBGLVL_OPENGL_DATA > 1
            MessageInterface::ShowMessage
               ("   Vx,Vy,Vz = %f, %f, %f\n",
                mScVxArray[scIndex], mScVyArray[scIndex], mScVzArray[scIndex]);
            #endif
         }
      }
      
      // if only showing current iteration, buffer data and return
      if (mSolverIterOption == SI_CURRENT)
      {
         // save data when targeting or last iteration
         if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
         {
            mCurrScArray.push_back(mScNameArray);
            mCurrEpochArray.push_back(dat[0]);
            mCurrXArray.push_back(mScXArray);
            mCurrYArray.push_back(mScYArray);
            mCurrZArray.push_back(mScZArray);
            mCurrVxArray.push_back(mScVxArray);
            mCurrVyArray.push_back(mScVyArray);
            mCurrVzArray.push_back(mScVzArray);
         }
         
         if (runstate == Gmat::SOLVING)
         {
            //MessageInterface::ShowMessage
            //   ("=====> num buffered = %d\n", mCurrEpochArray.size());
            return true;
         }
      }
      
      
      #if DBGLVL_OPENGL_UPDATE > 0
      MessageInterface::ShowMessage("==========> now update GL plot\n");
      #endif
      
      bool solving = false;
      UnsignedIntArray colorArray = mScOrbitColorArray;
      if (runstate == Gmat::SOLVING)
      {
         solving = true;
         colorArray = mScTargetColorArray;
      }
      
      PlotInterface::
         UpdateGlPlot(instanceName, mOldName, mScNameArray,
                      dat[0], mScXArray, mScYArray, mScZArray,
                      mScVxArray, mScVyArray, mScVzArray,
                      colorArray, solving, mSolverIterOption, update);
      
      if (update)
         mNumCollected = 0;
   }
   
   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}

