//------------------------------------------------------------------------------
//                           MeasureModel
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
// Created: Jan 31, 2014
/**
 * The Measurement Model base class for estimation
 */
//------------------------------------------------------------------------------

#ifndef MeasureModel_hpp
#define MeasureModel_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "SignalData.hpp"
#include "SignalDataCache.hpp"

// Forward references
class SignalBase;
class ProgressReporter;

class ObservationData;
class RampTableData;
class MeasurementData;
class PropSetup;
class PhysicalModel;
class ODEModel;
class PropagationStateManager;


/**
 * The estimation measurement model
 *
 * This class is the reworked measurement model for GMAT's estimation subsystem.
 * It uses the signal classes to model the path of a measurement.  The output
 * resulting from the modeling is built by an Adapter that uses the raw data to
 * generate measurement information.
 */
class ESTIMATION_API MeasureModel : public GmatBase
{
public:
   MeasureModel(const std::string &name, const bool usesPassiveSignal = false);
   virtual ~MeasureModel();

   virtual void CleanUp();

   MeasureModel(const MeasureModel& mm);
   MeasureModel& operator=(const MeasureModel& mm);

   virtual GmatBase* Clone() const;
   virtual void SetSolarSystem(SolarSystem *ss);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
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
//   virtual bool         SetRefObjectName(const UnsignedInt type,
//                                         const std::string &name);
   virtual bool RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
//   virtual GmatBase*    GetRefObject(const UnsignedInt type,
//                                     const std::string &name);
//   virtual GmatBase*    GetRefObject(const UnsignedInt type,
//                                     const std::string &name,
//                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   DEFAULT_TO_NO_CLONES

   virtual void         SetPropagators(std::vector<PropSetup*> *ps,
         std::map<std::string, StringArray> *spMap);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool         Initialize();

   virtual bool CalculateMeasurement(bool withEvents = false,
                       bool withMediaCorrection = true,
                       ObservationData* forObservation = NULL,
                       std::vector<RampTableData>* rampTB = NULL,
                       bool forSimulation = false, 
                       Real atTimeOffset = 0.0, 
                       Integer forStrand = -1);
   virtual bool ReCalculateFrequencyAndMediaCorrection(
                       UnsignedInt pathIndex, Real uplinkFrequency,
                       std::vector<RampTableData>* rampTB);

   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                                                        Integer id,
                                                        GmatTime *measTime = NULL,
                                                        bool forRangeRate = false,
                                                        Integer forStrand = -1);

   const std::vector<SignalData*>& GetSignalData();
   const std::vector<SignalBase*>& GetSignalPaths();
   bool                 CheckEpochValidity(const GmatTime &theEpoch);
   bool                 IsMeasurementFeasible();
   virtual void         UsesLightTime(const bool tf);

   /// Set measurement (relativity, ET-TAI, ionosphere, or troposphere) correction to measurement model
   virtual void         SetCorrection(const std::string& correctionName,
                           const std::string& correctionType);
   virtual void         AddCorrection(const std::string& correctionName,
                           const std::string& correctionType);
   /// Set flag to indicate measurement time tag is at the end of signal path
   void                 SetTimeTagFlag(const bool flag);
   /// Get flag to indicate measurement time tag
   bool                 GetTimeTagFlag();

   /// Get uplink frequency and uplink band for a given signal path
   Real                 GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB);
   Real                 GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB);
   Integer              GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB);

   /// Get uplink frequency and uplink band for a given signal path, these are used if GMD data is required
   Real                 GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData);
   Real                 GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData);
   Integer              GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData);

   /// Get arrival frequency and frequency band at a given node. It is used in TDRS measurement to specify node 4 frequency and band
   Real                 GetTransmitFrequency(const Integer atNode, const Integer pathIndex);
   Integer              GetTransmitFrequencyBand(const Integer atNode, const Integer pathIndex);
   Real                 GetTransmitFrequencyOscillatorBias(const Integer atNode, const Integer pathIndex);
   /// Get frequency mixer's properties a and b from a node 4 TDRS spacecraft
   RealArray            GetFrequencyMixerProperty(const Integer atNode, const Integer pathIndex);

   void                 AddTransientForce(GmatBase *spacePoint, ODEModel *odeModel, PropagationStateManager *propMan);

   /// Measurement Model Settings
   virtual bool         SetProgressReporter(ProgressReporter* reporter);

   /// Set value for Doppler count interval. It is used to calculate measurement for Start path
   void                 SetCountInterval(Real timeInterval);

   /// Get paticipant objects lists
   virtual const std::vector<ObjectArray*>&
                        GetParticipantObjectLists();

   /// Save and restore states of objects being propagated
   virtual void         SaveState(std::vector<bool>& precTimeVec, std::vector<GmatEpoch>& epochVec,
                                  std::vector<GmatTime>& epochGTVec, std::vector<Real>& valsVec);
   virtual void         RestoreState(std::vector<bool>& precTimeVec, std::vector<GmatEpoch>& epochVec,
                                     std::vector<GmatTime>& epochGTVec, std::vector<Real>& valsVec);

   /// Uses ionosphere cache
   virtual void         UseIonosphereCache(SignalDataCache::SimpleSignalDataCache* cache);

   virtual bool         IsCrossLink();

   Integer measParticipantIndex;

protected:
   /// The ordered list of participants in the signal path
   std::vector<StringArray*> participantLists;
   /// Participant pointers.  This list is 1:1 with participantList
   std::vector<ObjectArray*> participants;

   /// Mapping of participants to (cloned) propagators
   std::map<SpacePoint*,PropSetup*> propMap;

   /// Collection of the potential participants
   ObjectArray candidates;
   /// The list of Signals, path by path
   std::vector<SignalBase*> signalPaths;
   /// Measurement data from the Signals
   std::vector<SignalData*> theData;         // theData[i] points to theData of the head of signalPaths[i]
   /// Most recently calculated measurement derivatives gathered from Signals
   std::vector<RealArray> theDataDerivatives;
   /// transient forces to pass to the ODEModel
   std::vector<PhysicalModel *> *transientForces;

   /// Flag checking if the last measurement computed as feasible
   bool feasible;
   /// Flag used to control light time solution generation
   bool withLighttime;
   /// Flag used to indicate that the propagators need initialization
   bool propsNeedInit;
   /// A list of measurement correction types
   StringArray correctionTypeList;
   /// A list of measurement correction models
   StringArray correctionModelList;

   /// Flag to indicate measurerment time tag to be at the end of signal path
   bool epochIsAtEnd;

   /// Count interval is time interval (in seconds) between Start-path and End-path
   Real countInterval;

   /// The reporter for status information
   ProgressReporter *navLog;
   /// The log level for measurement models
   UnsignedInt logLevel;

   /// Flag used to indicate if the model is physical or single point
   bool isPhysical;

   /// Flag used to indicate if the model uses passive signals that do not include hardware delays
   bool isPassive;

   /// The solar system
   SolarSystem *solarsys;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      SIGNAL_PATH  = GmatBaseParamCount,
      MeasurementParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string PARAMETER_TEXT[MeasurementParamCount -
                                           GmatBaseParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType PARAMETER_TYPE[MeasurementParamCount -
                                                   GmatBaseParamCount];

   void PrepareToPropagate();
};

#endif /* MeasureModel_hpp */
