//$Id$
//------------------------------------------------------------------------------
//                                  OrbitPlot
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
// number X-XXXX-X
//
// Author: Linda Jun
// Created: 2011/05/23
//
/**
 * Declares OrbitPlot class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitPlot_hpp
#define OrbitPlot_hpp

#include "Subscriber.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include <map>

class GMAT_API OrbitPlot : public Subscriber
{
public:
   OrbitPlot(const std::string &type, const std::string &name);
   OrbitPlot(const OrbitPlot &op);
   OrbitPlot& operator=(const OrbitPlot&);
   virtual ~OrbitPlot();
   
   const StringArray&   GetSpacePointList();
   const StringArray&   GetSpacecraftList();
   const StringArray&   GetNonSpacecraftList();
   
   bool                 GetShowObject(const std::string &name);
   void                 SetShowObject(const std::string &name, bool value);
   
   // methods inherited from Subscriber
   virtual bool         Activate(bool state = true);
   
   // methods inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   
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
   
   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const Integer id) const;
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const std::string &label) const;
   virtual bool         SetBooleanArrayParameter(const Integer id,
                                                 const BooleanArray &valueArray);
   virtual bool         SetBooleanArrayParameter(const std::string &label,
                                                 const BooleanArray &valueArray);
   
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
   
   virtual void         SetOrbitColorChanged(GmatBase *originator,
                                             const std::string &newColor,
                                             const std::string &objName,
                                             const std::string &desc);
   virtual void         SetTargetColorChanged(GmatBase *originator,
                                              const std::string &newColor,
                                              const std::string &objName,
                                              const std::string &desc);
   virtual void         SetSegmentOrbitColor(GmatBase *originator,
                                             bool overrideColor,
                                             UnsignedInt orbitColor,
                                             const StringArray &objNames);
   // for GUI population
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   
protected:
   
   CoordinateSystem *mViewCoordSystem;
   
   std::string mOldName;
   std::string mViewCoordSysName;
   
   // object names and arrays
   std::vector<SpacePoint*> mObjectArray;
   std::vector<SpacePoint*> mAllSpArray;
   BooleanArray mDrawOrbitArray;
   BooleanArray mDrawObjectArray;
   
   StringArray mScNameArray;
   StringArray mObjectNameArray;
   StringArray mAllSpNameArray;
   StringArray mAllRefObjectNames;
   
   Integer mAllSpCount;
   Integer mScCount;
   Integer mObjectCount;
   Integer mNonStdBodyCount;
      
   // for data control
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   Integer mNumPointsToRedraw;
   Integer mNumData;
   Integer mNumCollected;
   Integer mDataAbsentWarningCount;
   Integer mMaxData;

   // arrays for holding distributed data
   RealArray mScXArray;
   RealArray mScYArray;
   RealArray mScZArray;
   RealArray mScVxArray;
   RealArray mScVyArray;
   RealArray mScVzArray;
   
   // arrays for holding previous distributed data
   BooleanArray mScPrevDataPresent;
   RealArray mScPrevEpoch;
   RealArray mScPrevX;
   RealArray mScPrevY;
   RealArray mScPrevZ;
   RealArray mScPrevVx;
   RealArray mScPrevVy;
   RealArray mScPrevVz;
   
   // arrays for holding solver current data
   std::vector<StringArray> mCurrScArray;
   std::vector<Real> mCurrEpochArray;
   std::vector<RealArray> mCurrXArray;
   std::vector<RealArray> mCurrYArray;
   std::vector<RealArray> mCurrZArray;
   std::vector<RealArray> mCurrVxArray;
   std::vector<RealArray> mCurrVyArray;
   std::vector<RealArray> mCurrVzArray;
   
   // maps for object and color
   std::map<std::string, UnsignedInt> mDefaultOrbitColorMap;
   std::map<std::string, UnsignedInt> mDefaultTargetColorMap;
   std::map<std::string, UnsignedInt> mCurrentOrbitColorMap;
   std::map<std::string, UnsignedInt> mCurrentTargetColorMap;
   
   // maps for object, flag, and epoch
   std::map<std::string, bool> mDrawOrbitMap;
   std::map<std::string, bool> mShowObjectMap;
   std::map<std::string, Real> mScInitialEpochMap;
   
   /// Calls PlotInterface for plotting non-solver data  
   virtual bool         UpdateData(const Real *dat, Integer len);
   
   /// Calls PlotInterface for plotting solver data
   virtual bool         UpdateSolverData();
   
   /// Buffers published spacecraft orbit data
   void                 BufferZeroData(Integer scIndex);
   void                 BufferPreviousData(Integer scIndex);
   virtual Integer      BufferOrbitData(const Real *dat, Integer len);
   
   /// Handle absent spacecraft when data is published
   void                 HandleAbsentData(const std::string &scName,
                                         Integer scIndex, Real currEpoch);
   
   /// Returns object string list
   virtual std::string  GetObjectStringList() const;
   
   /// Adds Spacecraft and other objects to object arrays
   bool                 AddSpacePoint(const std::string &name, Integer index,
                                      bool show = true);
   /// Clears all object arrays called from TakeAction("Clear")
   bool                 ClearSpacePointList();
   /// Removes SpacePoint object from the object called from TakeAction("Remove")
   bool                 RemoveSpacePoint(const std::string &name);
   /// Finds the index of the element label from the element label array.
   Integer              FindIndexOfElement(StringArray &labelArray,
                                           const std::string &label);
   /// Builds dynamic arrays to pass to plotting canvas
   void                 BuildDynamicArrays();
   /// Clears dynamic arrays such as object name array, etc.
   void                 ClearDynamicArrays();
   /// Updates plotting object information such as plotting object pointer, option flags, etc.
   void                 UpdateObjectList(SpacePoint *sp, bool show = false);
   
   /// Debug message
   void                 WriteCoordinateSystem(CoordinateSystem *cs,
                                              const std::string &label = "");
   
   enum
   {
      ADD = SubscriberParamCount,
      COORD_SYSTEM,
      DRAW_OBJECT,
      ORBIT_COLOR,
      TARGET_COLOR,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      NUM_POINTS_TO_REDRAW,
      SHOW_PLOT,
      MAX_DATA,
      OrbitPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OrbitPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[OrbitPlotParamCount - SubscriberParamCount];

};

#endif
