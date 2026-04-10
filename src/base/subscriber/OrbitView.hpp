//$Id$
//------------------------------------------------------------------------------
//                                  OrbitView
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
// Created: 2010/04/19
//
/**
 * Declares OrbitView class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitView_hpp
#define OrbitView_hpp

#include "OrbitPlot.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include <map>

class GMAT_API OrbitView : public OrbitPlot
{
public:
   OrbitView(const std::string &name);
   OrbitView(const OrbitView &ogl);
   OrbitView& operator=(const OrbitView&);
   virtual ~OrbitView(void);
   
   Rvector3             GetViewVector(const std::string &which);
   void                 SetVector(const std::string &which, const Rvector3 &value);
   
   // methods inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
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
   virtual bool         IsSquareBracketAllowedInSetting(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
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
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
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
   
   virtual void         SetSegmentOrbitColor(GmatBase *originator,
                                             bool overrideColor,
                                             UnsignedInt orbitColor);
   
protected:

   void     PutRvector3Value(Rvector3 &rvec3, Integer id,
                             const std::string &sval, Integer index = -1);
   void     WriteDeprecatedMessage(Integer id) const;
   bool     UpdateSolverData();
   
   bool       mShowLabels;
   
   CoordinateSystem *mViewUpCoordSystem;
   SpacePoint *mViewCoordSysOrigin;
   SpacePoint *mViewUpCoordSysOrigin;
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointObj;
   SpacePoint *mViewDirectionObj;
   
   std::string mEclipticPlane;
   std::string mXYPlane;
   std::string mWireFrame;
   std::string mOverlapPlot;
   std::string mUseInitialView;
   std::string mAxes;
   std::string mGrid;
   std::string mSunLine;
   
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

   // stars
   std::string mEnableStars;
   std::string mEnableConstellations;
   Integer mStarCount;
   
   // FOV - currentrly not used and will be removed later
   Integer mMinFOV;
   Integer mMaxFOV;
   Integer mInitialFOV;

   enum
   {
      SHOW_LABELS = OrbitPlotParamCount,
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
      VIEW_UP_COORD_SYSTEM,
      VIEW_UP_AXIS,
      CELESTIAL_PLANE,
      ECLIPTIC_PLANE,
      XY_PLANE,
      WIRE_FRAME,
      AXES,
      GRID,
      EARTH_SUN_LINES,
      SUN_LINE,
      OVERLAP_PLOT,
      USE_INITIAL_VIEW,
      STAR_COUNT,
      ENABLE_STARS,
      ENABLE_CONSTELLATIONS,
      MIN_FOV,
      MAX_FOV,
      INITIAL_FOV,
      OrbitViewParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OrbitViewParamCount - OrbitPlotParamCount];
   static const std::string
      PARAMETER_TEXT[OrbitViewParamCount - OrbitPlotParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
      
};

#endif
