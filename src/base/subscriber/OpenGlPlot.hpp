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
 * Declares OpenGlPlot class.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlot_hpp
#define OpenGlPlot_hpp

#include "Subscriber.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include <map>

class GMAT_API OpenGlPlot : public Subscriber
{
public:
   OpenGlPlot(const std::string &name);
   OpenGlPlot(const OpenGlPlot &ogl);
   OpenGlPlot& operator=(const OpenGlPlot&);
   virtual ~OpenGlPlot(void);
   
   const StringArray&   GetSpacePointList();
   const StringArray&   GetSpacecraftList();
   const StringArray&   GetNonSpacecraftList();
   
   UnsignedInt          GetColor(const std::string &item, const std::string &scName);
   bool                 SetColor(const std::string &item, const std::string &name,
                                 UnsignedInt value);
   bool                 GetShowObject(const std::string &name);
   void                 SetShowObject(const std::string &name, bool value);
   
   Rvector3             GetViewVector(const std::string &which);
   void                 SetVector(const std::string &which, const Rvector3 &value);
   
   // methods inherited from Subscriber
   virtual bool         Initialize();
   virtual bool         Activate(bool state = true);
   
   // methods inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label, const Real value);
   
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index);
   
   virtual const UnsignedIntArray&
                        GetUnsignedIntArrayParameter(const Integer id) const;   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                  const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
protected:

   bool     AddSpacePoint(const std::string &name, Integer index,
                          bool show = true);
   bool     ClearSpacePointList();
   bool     RemoveSpacePoint(const std::string &name);
   void     ClearDynamicArrays();
   void     UpdateObjectList(SpacePoint *sp, bool show = false);
   void     PutRvector3Value(Rvector3 &rvec3, Integer id,
                             const std::string &sval, Integer index = -1);
   void     PutUnsignedIntValue(Integer id, const std::string &sval);
   void     WriteDeprecatedMessage(Integer id) const;
   bool     UpdateSolverData();
   
   CoordinateSystem *mViewCoordSystem;
   CoordinateSystem *mViewUpCoordSystem;
   SpacePoint *mViewCoordSysOrigin;
   SpacePoint *mViewUpCoordSysOrigin;
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointObj;
   SpacePoint *mViewDirectionObj;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<SpacePoint*> mAllSpArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   
   std::string mEclipticPlane;
   std::string mXYPlane;
   std::string mWireFrame;
   std::string mOverlapPlot;
   std::string mUseInitialView;
   std::string mPerspectiveMode;
   std::string mUseFixedFov;
   std::string mAxes;
   std::string mGrid;
   std::string mSunLine;
   
   std::string mOldName;
   std::string mViewCoordSysName;
   std::string mViewPointRefName;
   std::string mViewPointRefType;
   std::string mViewPointVecName;
   std::string mViewPointVecType;
   std::string mViewDirectionName;
   std::string mViewDirectionType;
   std::string mViewUpCoordSysName;
   std::string mViewUpAxisName;
   
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVecVector;
   Rvector3 mViewDirectionVector;
   
   Real mViewScaleFactor;
   Real mFixedFovAngle;
   
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   Integer mNumPointsToRedraw;
   Integer mNumData;
   Integer mNumCollected;
   
   Integer mAllSpCount;
   Integer mScCount;
   Integer mObjectCount;
   Integer mNonStdBodyCount;

	std::string mEnableStars;
	std::string mEnableConstellations;
	Integer mStarCount;

	Integer mMinFOV;
	Integer mMaxFOV;
	Integer mInitialFOV;
   
   StringArray mScNameArray;
   StringArray mObjectNameArray;
   StringArray mAllSpNameArray;
   StringArray mAllRefObjectNames;
   
   // arrays for holding distrubuted data
   RealArray mScXArray;
   RealArray mScYArray;
   RealArray mScZArray;
   RealArray mScVxArray;
   RealArray mScVyArray;
   RealArray mScVzArray;
   UnsignedIntArray mScOrbitColorArray;
   UnsignedIntArray mScTargetColorArray;
   UnsignedIntArray mOrbitColorArray;
   UnsignedIntArray mTargetColorArray;
   
   // arrays for holding solver current data
   std::vector<StringArray> mCurrScArray;
   std::vector<Real> mCurrEpochArray;
   std::vector<RealArray> mCurrXArray;
   std::vector<RealArray> mCurrYArray;
   std::vector<RealArray> mCurrZArray;
   std::vector<RealArray> mCurrVxArray;
   std::vector<RealArray> mCurrVyArray;
   std::vector<RealArray> mCurrVzArray;
   
   std::map<std::string, UnsignedInt> mOrbitColorMap;
   std::map<std::string, UnsignedInt> mTargetColorMap;
   std::map<std::string, bool> mDrawOrbitMap;
   std::map<std::string, bool> mShowObjectMap;
   
   enum
   {
      ADD = SubscriberParamCount,
      ORBIT_COLOR,
      TARGET_COLOR,
      COORD_SYSTEM,
      VIEWPOINT_REF,
      VIEWPOINT_REFERENCE,
      VIEWPOINT_REF_TYPE,
      VIEWPOINT_REF_VECTOR,
      VIEWPOINT_VECTOR,
      VIEWPOINT_VECTOR_TYPE,
      VIEWPOINT_VECTOR_VECTOR,
      VIEW_DIRECTION,
      VIEW_DIRECTION_TYPE,
      VIEW_DIRECTION_VECTOR,
      VIEW_SCALE_FACTOR,
      FIXED_FOV_ANGLE,
      VIEW_UP_COORD_SYSTEM,
      VIEW_UP_AXIS,
      CELESTIAL_PLANE,
      XY_PLANE,
      WIRE_FRAME,
      AXES,
      GRID,
      EARTH_SUN_LINES,
      SUN_LINE,
      OVERLAP_PLOT,
      USE_INITIAL_VIEW,
      PERSPECTIVE_MODE,
      USE_FIXED_FOV,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      NUM_POINTS_TO_REDRAW,
      SHOW_PLOT,
		STAR_COUNT,
		ENABLE_STARS,
		ENABLE_CONSTELLATIONS,
		MIN_FOV,
		MAX_FOV,
		INITIAL_FOV,
      OpenGlPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   
   static const int MAX_SP_COLOR = 15;
   static const UnsignedInt DEFAULT_ORBIT_COLOR[MAX_SP_COLOR];
   
};

#endif
