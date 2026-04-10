//$Id: SeqEstimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         SeqEstimator
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/09/03
//
/**
 * Base class for sequential estimation
 */
//------------------------------------------------------------------------------


#ifndef SeqEstimator_hpp
#define SeqEstimator_hpp

#include "kalman_defs.hpp"
#include "Estimator.hpp"
#include "ProcessNoiseModel.hpp"

/**
 * Provides core functionality used in sequential estimation.
 *
 * This class provides the data structures and interfaces used when performing
 * sequential estimation.  The class includes an implementation of a finite
 * state machine that represents the typical state transitions encountered when
 * performing sequential estimation in, for example, an extended Kalman filter.
 */
class KALMAN_API SeqEstimator: public Estimator
{
public:
   SeqEstimator(const std::string &type, const std::string &name);
   virtual ~SeqEstimator();
   SeqEstimator(const SeqEstimator &se);
   SeqEstimator&    operator=(const SeqEstimator &se);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual void         StateCleanUp();
   virtual bool         Finalize();

   virtual bool         IsFinalPass();

   // methods overridden from GmatBase
// EKF mod 12/16
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
// End EKF mod

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   // Statistics information for Filter measurements
   struct FilterMeasurementInfoType : MeasurementInfoType
   {
      RealArray state;
      Rmatrix   cov;
      Rmatrix   sqrtCov;
      Rmatrix   covVNB;
      RealArray scaledResid;
      Rmatrix   kalmanGain;

      // Need to size Rmatrix
      FilterMeasurementInfoType()
      {
         cov.SetSize(0, 0);
         sqrtCov.SetSize(0, 0);
         covVNB.SetSize(0, 0);
         kalmanGain.SetSize(0, 0);
      };

      // Need copy assignment defined to set size of Rmatrix
      FilterMeasurementInfoType& operator=(const FilterMeasurementInfoType& info)
      {
         MeasurementInfoType::operator=(info);

         cov.SetSize(info.cov.GetNumRows(), info.cov.GetNumColumns());
         sqrtCov.SetSize(info.sqrtCov.GetNumRows(), info.sqrtCov.GetNumColumns());
         covVNB.SetSize(info.covVNB.GetNumRows(), info.covVNB.GetNumColumns());
         kalmanGain.SetSize(info.kalmanGain.GetNumRows(), info.kalmanGain.GetNumColumns());

         state       = info.state;
         cov         = info.cov;
         sqrtCov     = info.sqrtCov;
         covVNB = info.covVNB;
         scaledResid = info.scaledResid;
         kalmanGain  = info.kalmanGain;

         return *this;
      }
   };

   // Covariance information for measurements and time-updates
   struct UpdateInfoType
   {
      GmatTime    epoch;
      bool        isObs;
      FilterMeasurementInfoType measStat;
      RealArray   state;
      Rmatrix     cov;
      Rmatrix     sqrtCov;
      Rmatrix     covVNB;
      Rmatrix     processNoise;
      Rmatrix     stm;

      // Need to size Rmatrix
      UpdateInfoType()
      {
         cov.SetSize(0, 0);
         sqrtCov.SetSize(0, 0);
         covVNB.SetSize(0, 0);
         processNoise.SetSize(0, 0);
         stm.SetSize(0, 0);
      };

      // If there is a need for copy assignment, it will need to be
      // added explicitly here to size the Rmatrix struct members.
      // See FilterMeasurementInfoType above for an example.
   };

   // Functions added for smoothing
   virtual std::vector<UpdateInfoType> GetUpdateStats();
   virtual void         GetObjectBuffer(ObjectArray &buffer);
   virtual void         SetAnchorEpoch(const GmatTime& epoch, bool noiseBetween);
   virtual void         SetCovariance(const Rmatrix& cov);
   virtual bool         UpdateInitialConditions();

protected:
   /// Smoother instances access SeqEstimator data directly
   friend class SmootherBase;

   /// particicpants column length for Filter Covariance section of the report file.
   Integer                 pcolumnCovLen;

// EKF mod 12/16
   static const UnsignedInt truthStateSize;
   static const UnsignedInt stdColLen;
   static const UnsignedInt minPartSize;
   static const UnsignedInt strSizeMin;

   /// The step size used for updating the process noise
   Real        processNoiseStep;
   /// The next epoch desired for updating the process noise
   GmatTime    nextNoiseUpdateGT;
   /// The time duration to delay rectifying the reference trajectory
   Real        delayRectifySpan;
   /// The position sigma threshold for measurement deweighting (km)
   Real        deweightThreshold;
   /// The deweighting coefficient for measurment weighting (beta for Lear's method)
   Real        deweightCoeff;
   /// If an anchor epoch has been set
   bool        hasAnchorEpoch;
   /// If the estimator is to overwrite the edit flags with the calculated flags
   bool        overwriteEditFlags;

   Real constMult;
// End EKF mod

   /// The file to read data from to initialize the SeqEstimator
   std::string inputDataFile;
   /// The epoch to initialize the SeqEstimator from based on the inputDataFile
   std::string restartEpochFormat;
   /// The epoch to initialize the SeqEstimator from based on the inputDataFile
   std::string restartEpoch;
   /// The file to write SeqEstimator data to
   std::string outputDataFile;
   /// The output data file
   std::ofstream dataFile;

   /// Changes in the state vector
   Rvector                 dx;
   /// Array used to track the one-sigma deviation for each measurement point
   RealArray               sigma;

   /// The time updates covariance matrix
   Rmatrix                 pBar;
   /// The process noise
   Rmatrix                 Q;

   Rmatrix                 H; // measurement matrix, EKF mod 12/16
   Rmatrix                 I; // identity matrix of size stateSize, EKF mod 12/16

// EKF mod 12/16   
/// Size of the measurement
   UnsignedInt             measSize;
   /// meas count
   UnsignedInt             measCount;

   /// Process noise
   std::map<GmatBase*, ProcessNoiseModel*>
                           processNoiseModels;

   /// Data container for filter data
   DataBucket              matFilterData;

   /// Indexing for the .mat filter elements
   std::map<std::string, Integer> matFilterIndex;

   /// previous update epoch
   GmatTime                 prevUpdateEpochGT;

   /// Previous Cartesian to Solve-for state conversion derivative matrix [dX/dS]
   Rmatrix                 cart2SolvMatrixPrev;
   /// Previous Solve-for state to Keplerian conversion derivative matrix [dS/dK]
   Rmatrix                 solv2KeplMatrixPrev;
   // Is true when TakeAction("RunBackwards") is called, false when TakeAction("RunForwards") is called
   bool					   isSmoothing;
   // The forward pass data from a smoother run. Used when smoothing to ensure forward and back smoother epochs match
   std::vector<SeqEstimator::UpdateInfoType>* forwardFilterInfo;
   /// Parameter IDs for the BatchEstimators
   enum
   {
      PROCESS_NOISE_TIME_STEP = EstimatorParamCount,
      CONSTANT_MULTIPLIER,
      DELAY_RECTIFY_TIME,
      MEAS_DEWEIGHT_THRESHOLD,
      MEAS_DEWEIGHT_COEFF,
      INPUT_DATA_FILE,
      RESTART_EPOCH_FORMAT,
      RESTART_EPOCH,
      OUTPUT_DATA_FILE,

      SeqEstimatorParamCount
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string
                           PARAMETER_TEXT[SeqEstimatorParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[SeqEstimatorParamCount -
                                              EstimatorParamCount];
// End EKF mod

   // Methods used in the finite state machine
   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   virtual void            PrepareForStep();
   virtual void            FilterUpdate();

   // Additional methods used by the EKF
   virtual void            UpdateProcessNoise();
   virtual void            AddEstimatedParameterNoise(Rmatrix &noise, Real dt);
   virtual void            TimeUpdate();

   virtual bool            OverwriteEditFlag(const std::string &editFlag);
   virtual bool            WriteEditFlag();
   virtual void            EstimationPartials(std::vector<RealArray> &hMeas);
   virtual bool            DataFilter();

   virtual void            GetEstimationState(GmatState& outputState);

   bool                    ConvertToPartCoordSys(ListItem* infor, GmatTime epoch,
                                 Real inputStateElement,
                                 Real* outputStateElement);

   Rmatrix                 CovarianceConvertionMatrix(std::map<GmatBase*, Rvector6> stateMap);

   Rmatrix66               CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state);

   virtual std::string     GetProgressString();

   virtual void           WriteReportFileHeaderPart6();

   virtual void           WriteNotationHeader();

   virtual void           WriteReportFileSummaryPart1(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart5(Solver::SolverState sState);

   virtual void           WriteCovariancePageHeader();

   std::vector<UpdateInfoType> updateStats;

   virtual void           FillUpdateInfo(UpdateInfoType &updateStat);
   virtual void           BuildCovarianceLine(const UpdateInfoType &updateStat);

   ObjectArray            esmObjBuffer;

   virtual bool           IsIterative();

   virtual std::string    GetHeaderName();

   void                   OpenDataFile();
   virtual std::string    DataFileStateHeader(Integer index);
   virtual std::string    DataFileCovHeader() const;
   virtual void           WriteDataFile();
   virtual bool           RestartFromDataFile(const GmatTime &epoch, bool onlyPriorEpochs);
   virtual void           WriteCovarianceToDataFile();
   virtual void           ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
                                                     UnsignedInt firstStateIndex, IntegerArray stateColumnNum);

   virtual bool           WriteMatData();
   virtual void           AddMatlabData(const FilterMeasurementInfoType &measStat);
   virtual void           AddMatlabData(const FilterMeasurementInfoType &measStat,
                                        DataBucket &matData, IntegerMap &matIndex);
   virtual void           AddMatlabFilterData(const UpdateInfoType &updateStat);
   virtual void           AddMatlabFilterData(const UpdateInfoType &updateStat,
                                              DataBucket &matFilterData, IntegerMap &matFilterIndex);
   virtual void           AddMatlabConfigData();

   virtual bool			  VerifySmoothTimeStep();
   virtual Real			  MatchSmoothTimeStep();

   virtual std::string GetMeasurementResidualTitles();
};

#endif /* SeqEstimator_hpp */
