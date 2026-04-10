//$Id$
//------------------------------------------------------------------------------
//                         SmootherBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 * Base class for smoother
 */
//------------------------------------------------------------------------------


#ifndef SmootherBase_hpp
#define SmootherBase_hpp

#include "kalman_defs.hpp"
#include "Estimator.hpp"
#include "EstimatorException.hpp"
#include "SeqEstimator.hpp"

/**
 * Provides core functionality used in smoothing.
 *
 * This class provides the data structures and interfaces used when performing
 * smoothing.  It uses the settings from the specified filter
 */
class KALMAN_API SmootherBase: public Estimator
{
public:
   /// Enumeration defining the states in the state machine
   enum SmootherState
   {
      FILTERING,
      SMOOTHING,
      PREDICTING,
      UNDEFINED_STATE         // This one should stay at the end of the list.
   };

   SmootherBase(const std::string &type, const std::string &name);
   virtual ~SmootherBase();
   SmootherBase(const SmootherBase &sb);
   SmootherBase&    operator=(const SmootherBase &sb);

   virtual SmootherState   GetSmootherState();

   virtual bool         Initialize();
   virtual bool         Reinitialize();
   virtual SolverState  AdvanceState();
   virtual void         StateCleanUp();
   virtual bool         Finalize();

   virtual bool         IsFinalPass();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                        const Real value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);

   virtual UnsignedInt
      GetPropertyObjectType(const Integer id) const;

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
      const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
      const std::string &oldName,
      const std::string &newName);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
      const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name = "");

   virtual Real         GetTimeStep();

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual std::map<std::string, std::vector<Real *>>* GetForceDerivativeCache();

   SeqEstimator*        GetFilter();
   void                 PrepareFilter();
   void                 SetForwardFilterInfo(std::vector<SeqEstimator::UpdateInfoType> filterInfo);
   void                 SetObjectBuffer(const ObjectArray &buffer);

   virtual bool         ResetState();
   virtual void         MoveToNext(bool includeUpdate);

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);

   virtual void        SetDelayInitialization(bool delay);

   virtual bool        UpdateInitialConditions();

   virtual void         UpdateCov();

protected:
   /// Filter
   SeqEstimator *filter;
   std::string  filterName;

   // Filter info
   std::vector<SeqEstimator::UpdateInfoType> forwardFilterInfo;
   std::vector<SeqEstimator::UpdateInfoType> backwardFilterInfo;

   // Filter info index
   UnsignedInt filterIndex;

   // Buffer of objects from the filter's esm at final time
   ObjectArray esmObjBuffer;

   /// The time duration to delay rectifying the reference trajectory in the filter
   Real delayFilterRectifySpan;
   /// The position sigma threshold for measurement deweighting in the filter (km)
   Real deweightThresholdFilter;
   /// The deweighting coefficient for measurment weighting in the filter (beta for Lear's method)
   Real deweightCoeffFilter;

   /// Current state for the smoother
   SmootherState smootherState;

   /// Data container for smoother data
   DataBucket              matSmootherData;

   /// Indexing for the .mat smoother elements
   std::map<std::string, Integer> matSmootherIndex;

   /// Parameter IDs for the Smoother
   enum
   {
      FILTER = EstimatorParamCount,
      DELAY_FILTER_RECTIFY_TIME,
      MEAS_DEWEIGHT_THRESHOLD_FILTER,
      MEAS_DEWEIGHT_COEFF_FILTER,

      SmootherBaseParamCount
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string
                           PARAMETER_TEXT[SmootherBaseParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[SmootherBaseParamCount -
                                              EstimatorParamCount];

   // Methods used in the finite state machine
   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            Estimate();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   virtual void            SmootherUpdate();

   virtual bool            OverwriteEditFlag(const std::string &editFlag);
   virtual bool            WriteEditFlag();
   virtual bool            DataFilter();

   virtual std::string     GetProgressString();

   virtual void           WriteReportFileHeaderPart6();

   virtual void           WriteNotationHeader();

   virtual void           WriteReportFileSummaryPart1(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart5(Solver::SolverState sState);

   virtual void           WriteCovariancePageHeader();

   // Statistics information for Smoother measurements
   struct SmootherMeasurementInfoType : MeasurementInfoType
   {
      RealArray state;
      Rmatrix   cov;
      Rmatrix   covVNB;
      RealArray scaledResid;

      // Need to size Rmatrix
      SmootherMeasurementInfoType()
      {
         cov.SetSize(0, 0);
         covVNB.SetSize(0, 0);
      };
   };

   // Covariance information for measurements and time-updates
   struct SmootherInfoType
   {
      GmatTime    epoch;
      bool        isObs;
      SmootherMeasurementInfoType measStat;
      RealArray   state;
      Rmatrix     cov;
      Rmatrix66   covVNB;

      // Need to size Rmatrix
      SmootherInfoType()
      {
         cov.SetSize(0, 0);
         covVNB.SetSize(0, 0);
      };

      // If there is a need for copy assignment, it will need to be
      // added explicitly here to size the Rmatrix struct members.
      // See FilterMeasurementInfoType above for an example.
   };

   std::vector<SmootherInfoType> smootherStats;

   virtual void           SmoothState(SmootherInfoType &smootherStat, bool useUpdate) = 0;
   virtual void           AdvanceEpoch();

   virtual void           BuildCovarianceLine(const SmootherInfoType &updateStat);

   virtual bool           IsIterative();

   virtual std::string    GetHeaderName();

   virtual bool           WriteMatData();
   virtual bool           WriteAdditionalMatData() = 0;
   virtual void           AddMatlabData(const SmootherMeasurementInfoType &measStat);
   virtual void           AddMatlabData(const SmootherMeasurementInfoType &measStat,
                                        DataBucket &matData, IntegerMap &matIndex);
   virtual void           AddMatlabSmootherData(const SmootherInfoType &smootherStat);
   virtual void           AddMatlabSmootherData(const SmootherInfoType &smootherStat,
                                                DataBucket &matSmootherData, IntegerMap &matSmootherIndex);

   // Wrapper for functions called from SeqEstimator
   virtual void           AddMatlabData(const SeqEstimator::FilterMeasurementInfoType &measStat,
                                        DataBucket &matData, IntegerMap &matIndex);
   virtual void           AddMatlabFilterData(const SeqEstimator::UpdateInfoType &updateStat,
                                              DataBucket &matFilterData, IntegerMap &matFilterIndex);
   virtual void           AddMatlabConfigData();

   virtual EstimatorException BackFilterExceptionMessage(BaseException &ex);

   virtual std::string GetMeasurementResidualTitles();
};

#endif /* SmootherBase_hpp */
