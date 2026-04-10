//------------------------------------------------------------------------------
//                           TrackingDataAdapter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 7, 2014
/**
 * Base class for the tracking data adapters
 */
//------------------------------------------------------------------------------

#ifndef TrackingDataAdapter_hpp
#define TrackingDataAdapter_hpp

#include "MeasurementModelBase.hpp"
#include "MeasureModel.hpp"
#include "MeasurementData.hpp"
#include "ProgressReporter.hpp"
#include "RampTableData.hpp"
#include "ObservationData.hpp"
#include "SignalDataCache.hpp"
#include "ErrorModel.hpp"

// Forward reference
class SolarSystem;
class PropSetup;
class PhysicalModel;

/**
 * Base class for the tracking data adapters
 *
 * The tracking data adapters convert the raw data computed in measurement
 * models into the formatted data needed for the estimation and simulation
 * processes.  For many measurement types, this conversion is a very lightweight
 * process.
 *
 * The adapters also assemble the derivative data needed by these processes.
 */

#define BIASTYPE_IS_UNDEFINED    0
#define BIASTYPE_IS_NONE         1
#define BIASTYPE_IS_BIAS         2
#define BIASTYPE_IS_PASSBIAS     3

class ESTIMATION_API TrackingDataAdapter: public MeasurementModelBase
{
public:
   TrackingDataAdapter(const std::string &typeStr, const std::string &name);
   virtual ~TrackingDataAdapter();
   TrackingDataAdapter(const TrackingDataAdapter& ma);
   TrackingDataAdapter& operator=(const TrackingDataAdapter& ma);

   virtual void SetSolarSystem(SolarSystem *ss);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;


   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);


   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
//   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);     // ^^^^
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);


   virtual void         SetPropagators(std::vector<PropSetup*> *ps,
                           std::map<std::string, StringArray> *spMap);
   virtual bool         Initialize();
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);

   DEFAULT_TO_NO_CLONES

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL,
                              bool forSimulation = false) = 0;
   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id) = 0;
   
   std::string          GetTrackingConfig();

   virtual bool         SetMeasurement(MeasureModel *meas);
   void                 SetModelID(Integer newID);
   Integer              GetModelID();
   void                 SetModelTypeID(const Integer theID,
                                       const std::string type, Real mult = 1.0);
   Integer              GetModelTypeID();

   std::string          GetModelType();

   virtual const MeasurementData&
                        GetMeasurement();
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();

   virtual void         SetCorrection(const std::string& correctionName,
                                      const std::string& correctionType);

   // Measurement Model Settings
   virtual bool         SetProgressReporter(ProgressReporter* reporter);
   virtual void         UsesLightTime(const bool tf);

   // Multiplier Factor
   virtual void         SetMultiplierFactor(Real mult);
   virtual Real         GetMultiplierFactor();

   // Get measurement model object
   virtual MeasureModel * GetMeasurementModel();

   virtual void         AddMediaCorrection(bool isAdd) {withMediaCorrection = isAdd;}
   virtual void         AddBias(bool isAdd) {addBias = isAdd;}
   virtual void         AddNoise(bool isAdd) {addNoise = isAdd;}
   virtual void         SetRangeOnly(bool isRangeOnly) {rangeOnly = isRangeOnly;}
   
   // Set solve-for and consider objects
   virtual bool         SetUsedForObjects(ObjectArray objArray) {forObjects = objArray; return true;};

   virtual StringArray  GetParticipants(Integer forPathIndex);

   std::string          GetErrorMessage(){ return errMsg; };

   Real                 ApiGetDerivativeValue(Integer row,Integer column);

   StringArray          GetMeasurementDimension() { return dimNames;};

   void                 SetIonosphereCache(SignalDataCache::SimpleSignalDataCache *cache);

   void                 SetupHorp(Real height, Real angle, bool useOblateness = false, bool force = false);

   ErrorModel*          GetMeasurementErrorModel(Integer measIndex, const std::string measType, Integer numTrip);

   virtual bool         IsCrossLink();

protected:
   /// Measurement dimension
   StringArray               dimNames;
   /// The ordered list of participants in the measurement
   std::vector<StringArray*> participantLists;
   /// The measurement model that holds the raw data processed in this adapter
   MeasureModel              *calcData;
   /// Most recently computed measurement data
   MeasurementData           cMeasurement;
   /// Measurement derivatives
   std::vector<RealArray>    theDataDerivatives;

   // Noise model parameters
   /// Noise sigma (noiseSigma[i]) associated to measurement (cMeasurement.value[i])
   RealArray                 noiseSigma;
   /// Measurement bias (measurementBias[i]) associated to measurement (cMeasurement.value[i])
   RealArray                 measurementBias;

   /// Reporter for the adapter
   ProgressReporter          *navLog;
   /// Current logging level for adapters
   UnsignedInt               logLevel;
   /// Solar system used in the modeling
   SolarSystem               *solarsys;
   /// Array of reference objects
   ObjectArray               refObjects;
   /// Unique measurement ID used during a run
   Integer                   modelID;
   /// Measurement type ID
   Integer                   modelTypeID;
   /// Text name of the model type
   std::string               modelType;
   /// Scaling factor used in some measurements -- e.g. 2-way range divide by 2
   Real                      multiplier;

   /// Flags controlling the internal computations and corrections
   bool                      withLighttime;

   /// Flag used to activate the height of ray path algorithm
   bool useHorp;
   /// Flag used to force the height of ray path algorithm for all bodies
   bool forceHorp;
   /// The minimum height allowed for a signal to be valid
   Real horpHeight;
   /// The central angle used to determine if a signal is behind the body
   Real horpAngle;
   /// Toggle to calculate HORP based on an oblate body rather than spherical
   bool horpOblateness;


   /*
    *  Propagator components are passed through to the objects that need
    *  them.  We do not make local clones of these objects - cloning is not
    *  needed.  Instead, the pointers to the propagator management components,
    *  owned by the calling code, are just set as local pointers, and are never
    *  destroyed.  The calling code is responsible for that task.
    */

   /// Propagators used by adapters for light time solution
   std::vector<PropSetup*>          *thePropagators;
   /// Mapping for propagator overrides for specific spacecraft
   std::map<std::string, StringArray> *satPropagatorMap;


   /// Constant frequency value used in a physical measurement when needed (In DSNDoppler, it is used as uplink frequency for S path)
   Real                      uplinkFreq;                           // unit is MHz
   /// Frequency band   (In DSNDoppler, it is used for S path)
   Integer                   freqBand;
   /// uplink frequency at recieved epoch (It is defferent from uplink frequency at transmit epoch uplinkFreq)
   Real                      uplinkFreqAtRecei;                    // unit is MHz
   /// Observation data object containing an observation data record
   ObservationData*          obsData;

   /// Ramped frequency table used to calculate ramped frequency measurements
   std::vector<RampTableData>*rampTB;
   UnsignedInt               beginIndex;        // specify index of the first element in ramp table used for this tracking data adapter 
   UnsignedInt               endIndex;          // specify index of the last element in ramp table used for this tracking data adapter

   /// Name of the frequency ramp table that supplied or receives data
   StringArray               rampTableNames;
   /// Add noise to measurement (used only for simulation)
   bool                      addNoise;
   /// Add bias to measurement
   bool                      addBias;
   /// Flag indicating to compute range only (w/o bias and noise model).
   /// It is used in Doppler measurement for E and S-paths before adding noise and bias
   bool                      rangeOnly;

   /// Flag indicating to compute media correction as seting by GMAT script
   bool                      withMediaCorrection;

   /// Measurement error covariance matrix
   Covariance                measErrorCovariance;

   /// Measurement type
   std::string               measurementType;  // Accepted values: "Range", "SN_Range", "BRTS_Range", "DSN_PNRange", "DSN_SeqRange", "DSN_TCP", "RangeRate", "SN_Doppler", "SN_Doppler_Rtn", "BRTS_Doppler", "SN_DOWD", etc

   /// A list of all objects used for measurement calculation (specifically it contains solve-for objects and consider objects)
   ObjectArray               forObjects;

   /// Store the error message whenever an error occurs during measurement calculation
   std::string               errMsg;

   ///  Ionosphere cache
   SignalDataCache::SimpleSignalDataCache *ionosphereCache;

   /// Index of measuring participant. 
   /// Example: TDRS 3L Return Doppler's tracking config has participants {userSC, TDRS, GS2, GS1}.
   /// Its signal paths are userSC-->TDRS-->GS2 and GS1-->TDRS-->GS2. 
   /// Both paths have measuring participant to be GS2. 
   /// The index of GS2 in participant list {userSC, TDRS, GS2, GS1} is stored in measParticipantIndex
   /// For any measurement having measuring participant at the end of participant list, value of measParticipantIndex is -1 
   Integer                   measParticipantIndex;


   /// Time gap used to specify a pass break between to adjacent pass biases
   /// Example: {ob(1), ob(2), ..., ob(n)} are a set of n obsevations of a given measurement type. 
   /// They are sorted in order of measurement time tag ob(i).epoch. 
   /// Time gap between ob(i) and ob(i+1) is gap = ob(i+1).epoch - ob(i).epoch  
   /// When the gap is greater or equal timeGap. It shows the start point of the next pass bias at ob(i+1).epoch
   Real                      timeGapForPassBreak;                       // unit: second. 

   ErrorModel*               measErrorModel;

   /// Bias type is used to specify either bias or pass biases used as a solve-for variables.
   /// Available values are 
   ///        BIASTYPE_IS UNDEFINED :     bias type is not denied
   ///        BIASTYPE_IS_NONE:           no bias or pass biases in the solve-for list
   ///        BIASTYPE_IS_BIAS:           bias is in the list of solve-for variables
   ///        BIASTYPE_IS_PASSBIAS:       pass bias is in the list of solve-for variables 
   Integer                   biasType;


   /// Parameter IDs for the TrackingDataAdapter
   enum
   {
      SIGNAL_PATH  = MeasurementModelBaseParamCount,
      OBS_DATA,
      RAMPTABLES,
      MEASUREMENT_TYPE,
      ADD_NOISE,
      UPLINK_FREQUENCY,
      UPLINK_BAND,
      TIMEGAP_FOR_PASSBREAK,
      AdapterParamCount,
   };


   /// Strings describing the TrackingDataAdapter parameters
   static const std::string PARAMETER_TEXT[AdapterParamCount -
                                           MeasurementModelBaseParamCount];
   /// Types of the TrackingDataAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[AdapterParamCount -
                                                   MeasurementModelBaseParamCount];
   StringArray*         DecomposePathString(const std::string &value);
   
   virtual void         ComputeMeasurementBias(const std::string biasName, const std::string measType, Integer numTrip);
   virtual void         ComputeMeasurementNoiseSigma(const std::string noiseSigmaName, const std::string measType, Integer numTrip);
   void                 ComputeMeasurementErrorCovarianceMatrix();

   void                 BeginEndIndexesOfRampTable(Integer & err);
   virtual Real         IntegralRampedFrequency(GmatTime t1, Real delta_t, Integer& err);
};

#endif /* TrackingDataAdapter_hpp */
