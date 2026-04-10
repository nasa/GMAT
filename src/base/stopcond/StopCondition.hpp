//$Id$
//------------------------------------------------------------------------------
//                              StopCondition
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
// Created: 2003/10/09
//
/**
 * Declares StopCondition class.
 */
//------------------------------------------------------------------------------
#ifndef StopCondition_hpp
#define StopCondition_hpp

#include "gmatdefs.hpp"

#include "paramdefs.hpp"
#include "GmatBase.hpp"
#include "Interpolator.hpp"
#include "SolarSystem.hpp"
#include "Parameter.hpp"
#include "Spacecraft.hpp"
#include "Variable.hpp"
#include "ElementWrapper.hpp"

class GMAT_API StopCondition : public GmatBase
{
public:

   static const Real STOP_COND_TOL;

   StopCondition(const std::string &name = "",
                 const std::string &desc = "",
                 Parameter *epochParam = NULL, 
                 Parameter *stopParam = NULL,
                 const Real &goal = GmatBase::REAL_PARAMETER_UNDEFINED,
                 const Real &tol = STOP_COND_TOL,
                 const Integer repeatCount = 1,
                 Interpolator *interp = NULL);
   StopCondition(const StopCondition &copy);
   StopCondition& operator= (const StopCondition &right); 
   virtual ~StopCondition();
   
   bool Initialize();
   virtual bool Validate();
   virtual bool Evaluate();
   virtual bool IsTimeCondition();
   virtual bool AddToBuffer(bool isInitialPoint);
   virtual Real GetStopEpoch();
   void Reset();
   virtual Real GetStopInterval();
   
   bool IsInitialized();
   Integer GetBufferSize();
   std::string& GetDescription();
   Parameter* GetEpochParameter();
   Parameter* GetStopParameter();
   Parameter* GetGoalParameter();
   Interpolator* GetInterpolator();
   
   void SetDescription(const std::string &desc);
   void SetPropDirection(Real dir);
   void SetSolarSystem(SolarSystem *solarSystem);
   bool SetInterpolator(Interpolator *interp);
   bool SetEpochParameter(Parameter *param);
   bool SetStopParameter(Parameter *param);
   bool SetGoalParameter(Parameter *param);
   
   void SetLhsString(const std::string &str);
   void SetRhsString(const std::string &str);
   
   std::string GetLhsString();
   std::string GetRhsString();
   
   bool SetLhsWrapper(ElementWrapper* toWrapper);
   bool SetRhsWrapper(ElementWrapper* toWrapper);

   bool ClearWrappers();
   
   //virtual bool SetSpacecraft(SpaceObject *sc);
   //bool SetSpacecrafts(const std::vector<SpaceObject *> stopSats);
   bool SetSpacecrafts(const ObjectArray &propSats,
                               const std::vector<SpaceObject *> stopSats);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name = "");
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id,
                                       const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
   // Accessors for the last condition evaluated
   virtual Real GetStopValue();
   virtual Real GetStopDifference();
   virtual Real GetStopGoal();
   virtual Real GetStopTime();
   virtual Real GetTimeMultiplier();
   virtual bool IsCyclicParameter();
   virtual bool GetRange(Real &min, Real &max);
   virtual Real PutInRange(const Real value, const Real min, const Real max, 
                           const  bool isReflection = false);
   virtual void SkipEvaluation(bool shouldSkip);
   virtual void UpdateBuffer();
   
   DEFAULT_TO_NO_CLONES

protected:
   
   StringArray mAllRefObjectNames;
   
   Real mBaseEpoch;
   Real internalEpoch;
   Real currentGoalValue;
   Integer mRepeatCount;
   SolarSystem *mSolarSystem;
   
   Interpolator *mInterpolator;
   std::string mDescription;
   std::string mStopParamType;
   std::string mStopParamName;
   std::string mEpochParamName;
   std::string lhsString;
   std::string rhsString;
   
   /// left hand side Parameter of stopping condition
   Parameter *mStopParam;
   /// right hand side Parameter of stopping condition
   Parameter *mGoalParam;
   Parameter *mEpochParam;
   Parameter *mEccParam;
   Parameter *mRmagParam;
   
   ElementWrapper *lhsWrapper;
   ElementWrapper *rhsWrapper;
   
   /// ring buffer for epochs
   RealArray mEpochBuffer;
   /// ring buffer for associated LHS values
   RealArray lhsValueBuffer;
   /// ring buffer for associated RHS values
   RealArray rhsValueBuffer;
   
   Integer mNumValidPoints;
   Integer mBufferSize;
   Real mStopEpoch;
   Real mStopInterval;
   
   // History data used instead of ring buffer for general propagation (before
   // a stopping condition triggers
   Real previousEpoch;
   Real previousAchievedValue;
   Real previousGoalValue;
   
   bool mUseInternalEpoch;
   bool mInitialized;
   bool mNeedInterpolator;
   bool mAllowGoalParam;
   bool mBackwardsProp;
   bool activated;
   
   // Flags used to mark special cases
   bool isLhsCyclicCondition;
   bool isRhsCyclicCondition;
   bool isPeriapse;
   bool isApoapse;
   bool isCyclicTimeCondition;      // Used for Elapsed... time conditions
   Real startValue;
   Real initialGoalValue;
   
   // The CycleType moved to GmatParam namespace defined in Parameter.hpp
   //enum CycleType
   //{
   //   NOT_CYCLIC,
   //   ZERO_90,
   //   ZERO_180,
   //   ZERO_360,
   //   PLUS_MINUS_90,
   //   PLUS_MINUS_180,
   //   OTHER_CYCLIC
   //};
   
   // To handle stopping condition of Parameters in both sides
   // such as Sat1.TA = Sat2.TA (future implementation)
   GmatParam::CycleType lhsCycleType;
   GmatParam::CycleType rhsCycleType;
   
   enum TimeType
   {
      NOT_TIME_PARAM,
      SECOND_PARAM,
      MINUTE_PARAM,
      HOUR_PARAM,
      DAY_PARAM,
      EPOCH_PARAM,
      UNKNOWN_PARAM_TIME_TYPE
   };
   
   TimeType stopParamTimeType;
   
   enum
   {
      BASE_EPOCH = GmatBaseParamCount,
      EPOCH,
      EPOCH_VAR,
      STOP_VAR,
      GOAL,
      REPEAT_COUNT,
      StopConditionParamCount,
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[StopConditionParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[StopConditionParamCount - GmatBaseParamCount];

   bool CheckOnPeriapsis();
   bool CheckOnApoapsis();
   bool CheckCyclicCondition(Real &value);

private:
   void CopyDynamicData(const StopCondition &stopCond);
};

#endif // StopCondition_hpp
