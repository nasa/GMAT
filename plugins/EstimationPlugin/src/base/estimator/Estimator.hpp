//$Id: Estimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Estimator
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
// Created: 2009/08/03
//
/**
 * Definition of the Estimator base class
 */
//------------------------------------------------------------------------------


#ifndef Estimator_hpp
#define Estimator_hpp

#include "Solver.hpp"
#include "MeasurementManager.hpp"
#include "EstimationStateManager.hpp"
#include "PropSetup.hpp"
#include "DataWriter.hpp"
#include "DataBucket.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton
#include "CholeskyFactorization.hpp"
#include "QRFactorization.hpp"
#include <map>

/**
 * The base class used for GMAT's estimators
 */

//#define ESTTIME_ROUNDOFF 1e-10
#define ESTTIME_ROUNDOFF 1e-9      //1e-10 causes infinitive loop in propagation steps

class ESTIMATION_API Estimator : public Solver
{
public:
   Estimator(const std::string &type, const std::string &name);
   virtual ~Estimator() = 0;
   Estimator(const Estimator& est);
   Estimator& operator=(const Estimator& est);

   virtual void         SetSolarSystem(SolarSystem *ss);

   virtual bool         Initialize();
   virtual bool         Reinitialize();
   virtual void         CompleteInitialization();
   virtual bool         Finalize();

   virtual bool         IsFinalPass() = 0;

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                        const Real value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label,
                                          const std::string &value);

   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        GetBooleanParameter(const std::string &label) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
   virtual bool        SetBooleanParameter(const std::string &label,
                                           const bool value);

   virtual const StringArray& GetPropertyEnumStrings(const Integer id) const;

   virtual UnsignedInt  GetPropertyObjectType(const Integer id) const;
   virtual UnsignedInt  GetPropertyObjectType(const Integer id,
         const Integer index) const;

   // Access methods for the reference objects
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
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
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   virtual Real         GetTimeStep();

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   virtual void         UpdateCurrentEpoch(GmatTime newEpoch);
   virtual GmatTime     GetCurrentEpoch();
   virtual void         BeginPredicting(Real predictSpan);
   virtual UnsignedInt  TrimObsByEpoch(const GmatTime& epoch, bool removeObsAtEpoch);
   virtual bool         UpdateInitialConditions();

   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual std::map<std::string, std::vector<Real *>>* GetForceDerivativeCache();

   PropSetup*           GetPropagator(const std::string &forSpacecraft);
   MeasurementManager*  GetMeasurementManager();
   EstimationStateManager*
                        GetEstimationStateManager();

   virtual bool         ResetState();
   virtual void         ResetSTM();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);

   virtual void        SetDelayInitialization(bool delay);

   virtual bool		   SetPropagatorSatMap(std::string propName, std::string satName);

   virtual void         UpdateCov();

   std::map<std::string, StringArray> GetPropSatMap();

   std::map<std::string, PropSetup*>  GetSatPropMap();

   void                   BuildSatPropMap();

   bool                   DoesSatUseEphemerisPropagator(std::string satName);
   
protected:
///// TBD: Do we need to separate TS and MM like this going forward?
   /// Names of measurements and tracking systems used in the estimation
   StringArray             measurementNames;
   /// Names of the measurement models used in the estimation
   StringArray             modelNames;
   /// The solve for parameters in the estimation problem
   StringArray             solveForStrings;
   /// The consider parameters in the estimation problem
   StringArray             considerStrings;

   /// Names of default PropSetup and spacecraft propagators used to evolve the system
   std::vector<std::string> propagatorNames;
   /// Map of spacecraft propagators that override the default propagator
   std::map<std::string, StringArray> propagatorSatMap;
   /// Propagator name used for sat prop filling
   std::string             currentPropagator;

   /// Flag used to check for a state load from the objects prior to propagation
   bool                    resetState;


   //---------------------------------------------------------------------------
   // Internal Data
   //---------------------------------------------------------------------------

   /**
    *  The timestep that gets returned for the next propagation
    *
    *  timeStep will be the same as estimationStep when the state machine is in
    *  the PROPAGATING state.  When in the LOCATING state, timeStep is the time
    *  from the base epoch to the next attempt at finding the event that is
    *  being located.
    */
   Real                    timeStep;
   /// Local storage element for ref object names
   StringArray             refObjectList;

   /// The estimator's measurement manager
   MeasurementManager      measManager;
   /// The estimator's state manager
   EstimationStateManager  esm;
   /// Indexes of rows/columns of 0s that were removed from the normal matrix
   IntegerArray            removedNormalMatrixIndexes;
   /// Transient forces used in estimation
   std::vector<PhysicalModel *>     *transientForces;
   /// Cached force derivatives
   std::map<std::string, std::vector<Real *>>     forceDerivativeCache;

   /// The propagators configured for simulation
   std::vector<PropSetup*> propagators;
   /// A mapping from a spacecraft name to the associated propagator
   std::map<std::string, PropSetup*> satPropMap;
   /// Flag indicating if the satellite propagator map needs to be built
   bool                    needsSatPropMap;

   /// Time system used to specify the estimation epoch
   std::string             estEpochFormat;
   /// The estimation epoch.  "FromParticipants" means use the spacecraft epoch.
   std::string             estEpoch;
   /// Epoch of the current estimation state
   GmatTime                estimationEpochGT;
   /// Currrrent simulation epoch
   GmatTime                currentEpochGT;
   /// The next epoch desired from propagation
   GmatTime                nextMeasurementEpochGT;
   /// How long to propagate past the last measurement in seconds
   Real                    predictTimeSpan;
   /// Epoch to predict to
   GmatTime                finalEpochGT;
   /// If the estimator is predicting past the last measurement
   bool                    isPredicting;
   /// Flag if the data from the predictive span should be added to the MATLAB output
   bool                    addPredictToMatlab;

   /// The measurement derivatives at the measurement epoch, \tilde H
   std::vector<RealArray>  hTilde;
   
   /// The indices for the MeasurementModels with observations at current epoch
   IntegerArray            modelsToAccess;

   /// The estimation state transition matrix
   Rmatrix                 *stm;
   /// The estimation state covariance matrix
   Covariance              *stateCovariance;
   /// The estimated state in GMAT internal coordinate system (in Cartesian coordinate system)
   GmatState               *estimationState;
   /// The estimated state in solve-for state type (solve-for state could be CartesianState or KeplerianState)
   GmatState               estimationStateS;

   /// Cartesian to Solve-for state conversion derivative matrix [dX/dS]
   Rmatrix                 cart2SolvMatrix;
   /// Solve-for state to Keplerian conversion derivative matrix [dS/dK]
   Rmatrix                 solv2KeplMatrix;

   /// Apriori estimation state presenting in MJ2000Eq cooridnate system and solve-for state type
   GmatState aprioriMJ2000EqSolveForState;

   /// Apriori estimation state presenting in participants' cooridnate systems and state type specified by spacecraft's DisplayStateType
   GmatState aprioriSolveForState;
   /// Previous estimation state presenting in participants' coordinate systems and state type specified by spacecraft's DisplayStateType
   GmatState previousSolveForState;
   /// Current estimation state presenting in participants' coordinate systems and state type specified by spacecraft's DisplayStateType
   GmatState currentSolveForState;
   /// Apriori estimation state presenting in participants' cooridnate systems and state type specified by spacecraft's DisplayStateType
   /// Its anomaly is in form of MA 
   GmatState aprioriSolveForStateMA;

   /// Apriori estimation state presenting in participants' Cartesian coordinate system
   GmatState aprioriSolveForStateC;
   /// Previous estimation state presenting in participants' Cartesian coordinate system
   GmatState previousSolveForStateC;
   /// Current estimation state presenting in participants' Cartesian coordinate systems
   GmatState currentSolveForStateC;

   /// Apriori estimation state presenting in participants' Keplerian coordinate system
   GmatState aprioriSolveForStateK;
   /// Previous estimation state presenting in participants' Keplerian coordinate system
   GmatState previousSolveForStateK;
   /// Current estimation state presenting in participants' Keplerian coordinate systems
   GmatState currentSolveForStateK;


   /// Size of the estimation state vector
   UnsignedInt             stateSize;

   /// The estimated state in solve-for state type (solve-for state could be CartesianState or KeplerianState)
   GmatState               initialEstimationStateS;


   /// The information matrix, $\Lambda$
   Rmatrix                 information;
   /// The inverse of the information matrix
   Rmatrix                 informationInverse;

   /// The residual vector, N in Tapley, Schutz and Born
   Rvector                 residuals;
   /// The a priori state, \bar x_0 in Tapley, Schutz and Born
   Rvector                 x0bar;

   // Factorization class instances
   CholeskyFactorization   cf;
   QRFactorization         qr;

   std::map<GmatBase*, CoordinateSystem*> vnbFrames; // VNB frames for report output

   /// Flag set to wait writing Measurement residuals until after estimation
   bool                    writeMeasurmentsAtEnd;
   /// Flag set to show all residuals during processing
   bool                    showAllResiduals;
   /// Flag used to only display a specific set of residuals
   bool                    showSpecificResiduals;
   /// Toggle for displayign error bars
   bool                    showErrorBars;
   /// List of the residuals plot names
   StringArray             addedPlots;

   /// The owned residuals plots used by this estimator
   std::vector<OwnedPlot*> residualPlots;
   /// The data used to generate error bars on the residuals plots
   std::vector<RealArray*> hiLowData;

   /// Vector of the current active Event objects
   ObjectArray             activeEvents;
   /// Flag indicating is an Event is currently being located
   bool                    locatingEvent;

   /// Number of removed observation data records
   std::map<std::string, UnsignedInt> numRemovedRecords;

   /// A string as a line/lines buffer to store a line/lines for writing to report file
   std::string linesBuff;

   std::string linesDataBuff;

   /// particicpants column length. It is used for writing report file
   Integer                 pcolumnLen;

   /// The .mat DataWriter object used to write data for MATLAB
   DataWriter              *matWriter;
   /// Flag indicating is the .mat file should be written
   bool                    writeMatFile;
   /// .mat data file name
   std::string             matFileName;
   /// Flag indicating if the JSON file should be written
   bool                    writeJsonFile;
   /// JSON data file name
   std::string             jsonFileName;
   /// Data file style
   std::string             dataFileStyle;
   /// Data container used during accumulation
   DataBucket              matData;
   /// Data container used for observations
   DataBucket              matObsData;
   /// Data container for estimation config data
   DataBucket              matConfigData;

   /// Indexing for the .mat data elements
   std::map<std::string, Integer> matIndex;
   /// Indexing for the .mat observation elements
   std::map<std::string, Integer> matObsIndex;
   /// Indexing for the .mat estimation config elements
   std::map<std::string, Integer> matConfigIndex;

   bool dontWriteDataInUpdate;

   /// Solar system      // It needs to display information of central bodies in estimation report file
   SolarSystem *solarSystem;

   // EOP time range. It needs to check measurement epoch to be inside that range.
   Real      eopTimeMin;        // EOP time min
   Real      eopTimeMax;        // EOP time max
   Integer   warningCount;      // count number of warning message when check measurement epoch to be inside or outside EOP time range

   // Media correction QA
   StringArray ionoWarningList;   // list contains all measurement passes with ionosphere correction to be outside acceptable range [0m, 20m]
   StringArray tropoWarningList;  // list contains all measurement passes with troposphere correction to be outside acceptable range [0m, 60m]

   /// List of estimation data filters
   StringArray dataFilterStrings;
   ObjectArray dataFilterObjs;
   IntegerArray editedRecords;             // flag indicating an observation data record used for calculating estimation

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   //Write TDRS stats to matfile when true
   bool addTdrsStats;

   // Statistics information for measurements
   struct MeasurementInfoType
   {
      GmatTime    epoch;
      UnsignedInt recNum;
      Integer     modelSize;
      Integer     editFlag;
      bool        isCalculated;
      std::string removedReason;
      std::string station;
      std::string type;
      Integer     uniqueID;
      Real        frequency;
      Real        feasibilityValue;
      RealArray   measValue;
      RealArray   residual;
      RealArray   weight;
      std::vector<RealArray>  hAccum;
      Real        tropoCorrectValue;
      Real        ionoCorrectValue;
      Real        horpHeight;
      Real        horpAngle;
   };

   std::vector<MeasurementInfoType> measStats;

   StringArray  stationsList;            // station
   StringArray  measTypesList;           // measurement type

   // Calculated measurement information for Json output
   struct JsonComputedMeas
   {
      Real                        recNum;
      GmatTime                    epoch;
      StringArray                 participants;
      std::string                 type;
      RealArray                   residual;
      std::string                 editFlag;
      RealArray                   measValue;
      Real                        elevation;
      Real                        ionoCorrectValue;
      Real                        tropoCorrectValue;
      RealArray                   partials;
   };

   /// Parameters associated with the Estimators
   enum
   {
      MEASUREMENTS  = SolverParamCount,
      SOLVEFORS,
      PROPAGATOR,
      ESTIMATION_EPOCH_FORMAT,
      ESTIMATION_EPOCH,
      PREDICT_TIME_SPAN,
      ADD_PREDICT_TO_MATLAB,
      SHOW_RESIDUALS,
      ADD_RESIDUAL_PLOT,
      DATA_FILTERS,
      MATLAB_OUTPUT_FILENAME,
      JSON_OUTPUT_FILENAME,
      DATA_FILE_STYLE,
      EstimatorParamCount
   };

   /// String identifiers for the parameters
   static const std::string    PARAMETER_TEXT[EstimatorParamCount -
                                              SolverParamCount];
   /// Types of the parameters
   static const Gmat::ParameterType
                               PARAMETER_TYPE[EstimatorParamCount -
                                              SolverParamCount];

   static const Integer NORMAL_FLAG;
   static const Integer BLOCKED_FLAG;
   static const Integer U_FLAG;
   static const Integer RAMP_FLAG;
   static const Integer IRMS_FLAG;
   static const Integer OLSE_FLAG;
   static const Integer USER_FLAG;
   static const Integer ILSE_FLAG;
   static const Integer HORP_FLAG;
   static const Integer EGAP_FLAG;

   /// Conversion from ModJulian to MATLAB datenum
   static const Real MATLAB_DATE_CONVERSION;

   /// Tolerance for covariance matrix inversion
   static const Real COV_INV_TOL;

   virtual Integer         TestForConvergence(std::string &reason);

   GmatTime                ConvertToGmatTimeEpoch(const std::string &theEpoch, 
                                                  const std::string &theFormat);

   virtual void            BuildResidualPlot(const std::string &plotName,
                              const StringArray &measurementNames);
   virtual void            PlotResiduals();
   virtual void            EnhancePlot();
   virtual void            CalculateResiduals(MeasurementInfoType &measStat);
   virtual bool            OverwriteEditFlag(const std::string &editFlag);
   virtual bool            WriteEditFlag();
   virtual void            EstimationPartials(std::vector<RealArray> &hMeas);
   virtual bool            DataFilter() = 0;
   virtual void            BuildMeasurementLine(const MeasurementInfoType &measStat);
   virtual void            WriteMeasurementLine(Integer recNum);
   virtual void            AddMatlabData(const MeasurementInfoType &measStat);
   virtual void            AddMatlabData(const MeasurementInfoType &measStat,
                                         DataBucket &matData, IntegerMap &matIndex);
   virtual void            AddMatlabIterationData();
   virtual void            AddMatlabConfigData();

   bool                    AddMatData(DataBucket &matData, Integer index = 0);
   virtual bool            WriteMatData();
   StringArray             GetMatDataDescription(DataBucket &data);

   virtual Integer         SetSolverResults(Real*, const std::string&,
                                            const std::string&);
   virtual void            SetResultValue(Integer, Real, const std::string&);

   virtual ObservationData*
                           FilteringData(ObservationData* obsData, Integer obDataId, Integer& filterIndex);

   /// Abstract method that performs the estimation in derived classes
   virtual void            Estimate() = 0;

   void                    Symmetrize(Covariance& mat);
   void                    Symmetrize(Rmatrix& mat);
   virtual Rmatrix         thinQR(const Rmatrix &mat1, const Rmatrix &mat2);
   Covariance*             GetMeasurementCovariance();
   Real                    GetMeasurementCovariance(const UnsignedInt r, const UnsignedInt c);
   virtual Real            GetMeasurementWeight(const UnsignedInt index);
   Real                    GetEpsilonConversion(const UnsignedInt stateIndex);
   void                    CovarianceEpsilonConversion(Rmatrix& cov);
   void                    CovarianceEpsilonConversionReverse(Rmatrix& cov);
   void                    CartesianCovarianceRotation(Rmatrix& cov, bool allStatesCart);
   Rmatrix                 GetCovarianceVNB(const Rmatrix& inCov);

   virtual bool           SetNominalValues();
   virtual void           UpdateCovarianceNominalValues(RealArray prevEpsilonConversions);
   virtual Real           GetStateValue(const Integer id, const Integer stateIndex);
   virtual Real           SetStateValue(const Integer id, const Integer stateIndex, const Real value);

   Real                    ObservationDataCorrection(Real cValue, Real oValue, Real moduloConstant);
   void                    ValidateMediaCorrection(const MeasurementData* measData);

   /// Estimation status contains all status of an estimation
   enum EstimationStatus
   {
      UNKNOWN,
      ABSOLUTETOL_CONVERGED,
      RELATIVETOL_CONVERGED,
      ABS_AND_REL_TOL_CONVERGED,
      MAX_CONSECUTIVE_DIVERGED,
      MAX_ITERATIONS_DIVERGED,
      CONVERGING,
      DIVERGING,
	   RUN_INITIAL_GUESS,
   };

   std::stringstream textFile0;
   std::stringstream textFile1;
   std::stringstream textFile1_1;
   std::stringstream textFile2;
   std::stringstream textFile3;
   std::stringstream textFile4;
   std::stringstream textFile5;

   virtual void            WriteToTextFile(Solver::SolverState state =
                                              Solver::UNDEFINED_STATE);

   // Report File functions
   virtual void           WriteReportFileHeaderPart1();
   virtual void           WriteReportFileHeaderPart2();
   virtual void           WriteReportFileHeaderPart2b();
   virtual void           WriteReportFileHeaderPart2c();
   virtual void           WriteReportFileHeaderPart3();
   virtual void           WriteReportFileHeaderPart4_1();
   virtual void           WriteReportFileHeaderPart4_2();
   virtual void           WriteReportFileHeaderPart4_3();
   virtual void           WriteReportFileHeaderPart4_3b();
   virtual void           WriteReportFileHeaderPart4_4();
   virtual void           WriteReportFileHeaderPart4();
   virtual void           WriteReportFileHeaderPart5();
   virtual void           WriteReportFileHeaderPart6() = 0; // Contains estimator-specific reporting
   virtual void           WriteReportFileHeader();

   virtual bool           IsIterative() = 0; // Estimator specific if it iterates

   StringArray            ColumnBreak(std::string content, Integer colLenMax, StringArray seperators);

   virtual void           WriteIterationHeader();
   virtual void           WriteNotationHeader();
   virtual void           WriteMeasurementHeader();
   virtual void           WritePageHeader();
   virtual std::string    GetHeaderName();
   virtual std::string    CenterAndPadHeader(std::string text);

   virtual void           WriteReportFileSummaryPart0(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart1(Solver::SolverState sState) = 0; // Contains estimator-specific reporting
   virtual void           WriteReportFileSummaryPart2(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart3(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart4(Solver::SolverState sState);
   virtual void           WriteReportFileSummaryPart5(Solver::SolverState sState);
   virtual void           WriteReportFileSummary(Solver::SolverState sState);

   StringArray                      jsonObsData;
   StringArray                      jsonIterData;
   std::vector<JsonComputedMeas>    jsonMeasData;

   mutable bool                   matDeprecatedFlag;

   virtual bool           WriteJsonData();
   virtual void           AddJsonObservationData(const MeasurementInfoType &measStat);
   virtual void           AddJsonIterationData();
   virtual void           AddJsonIterationData(const MeasurementInfoType &measStat);

   std::string            GetElementName(ListItem* infor, bool isInternalCS,
                                         std::string stateType = "", std::string anomalyType = "TA") const;
   std::string            GetElementNameMJ2000Eq(ListItem* infor, bool isInternalCS,
                                                 std::string stateType = "", std::string anomalyType = "TA") const;
   std::string            GetElementFullName(ListItem* infor, bool isInternalCS,
                                             std::string stateType = "", std::string anomalyType = "TA") const;
   std::string            GetElementFullNameID(ListItem* infor, bool isInternalCS,
                                               std::string stateType = "", std::string anomalyType = "TA") const;
   std::string            GetElementFullNameMJ2000Eq(ListItem* infor, bool isInternalCS,
                                                     std::string stateType = "", std::string anomalyType = "TA") const;

   std::string            GetElementUnit(ListItem* infor, std::string stateType) const;
   Integer                GetElementPrecision(std::string unit) const;

   enum ObsFilterType
   {
      ALL,
      STATION,
      DATATYPE,
   };

   std::string            WriteObservationSummary(ObsFilterType filter1 = ALL, ObsFilterType filter2 = ALL);
   UnsignedIntArray       FilterObservations(std::string groundStation, std::string measType) const;
   std::string            SummarizeObservationValues(UnsignedIntArray indicies) const;

   std::string            WriteMatrixReport(const Rmatrix &matrix, bool scientific = false) const;

   std::map<GmatBase*, Rvector6>
                          CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state);
   std::map<GmatBase*, Rvector6>
                          CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state);
   std::map<GmatBase*, RealArray>
                          CalculateAncillaryElements(const std::vector<ListItem*> *map, GmatState state);


   std::string            GetFileCreateTime(std::string fileName);
   std::string            CTime(const time_t* time);
   std::string            GetGMATBuildDate();
   std::string            GetDayOfWeek(Integer day, Integer month, Integer year);
   std::string            GetOperatingSystemName();
   std::string            GetOperatingSystemVersion();
   std::string            GetHostName();
   std::string            GetUserID();

   std::string GetUnit(const std::string type) const;

   virtual void           WriteStringArrayValue(Gmat::WriteMode mode,
                                                std::string &prefix,
                                                Integer id, bool writeQuotes,
                                                std::stringstream &stream);

 
   virtual std::string GetMeasurementResidualTitles();

private:
   bool      delayInitialization;

   std::stringstream formatter;

};

#endif /* Estimator_hpp */
