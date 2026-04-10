//$Id: MeasurementManager.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementManager
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
// Created: 2009/06/24
//
/**
 * MeasurementManager declaration used in GMAT's estimators and simulator
 *
 * This class acts as a mediator between the estimators and simulator and the
 * measurement models.
 */
//------------------------------------------------------------------------------

#ifndef MeasurementManager_hpp
#define MeasurementManager_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "EstimationDefs.hpp"
#include "MeasurementData.hpp"
#include "ObservationData.hpp"
#include "Rmatrix.hpp"
#include "MeasurementModelBase.hpp"
#include "DataFile.hpp"

// Extensions for tracking data adapters
#include "TrackingFileSet.hpp"
#include "TrackingDataAdapter.hpp"
#include "Event.hpp"

#include "SignalDataCache.hpp"

class PropSetup;


class ESTIMATION_API MeasurementManager
{
public:
   MeasurementManager();
   virtual ~MeasurementManager();
   virtual void CleanUp();
   MeasurementManager(const MeasurementManager &mm);
   MeasurementManager& operator=(const MeasurementManager &mm);

   bool                    SetPropagators(std::vector<PropSetup*> *props,
                                 std::map<std::string, StringArray> *satPropMap);
   bool                    Initialize();
   bool                    PrepareForProcessing(bool simulating = false);
   bool                    ProcessingComplete();
   bool                    Finalize();

   bool                    ValidateDuplicationOfGroundStationID(std::string& errorMsg);
// bool                    CalculateMeasurements(bool withEvents = false);                     // made changes for Bug 8 in ticket GMT-4314
   bool                    CalculateMeasurements(bool forSimulation = false, bool withEvents = false, bool addNoise = false);   // made changes for Bug 8 in ticket GMT-4314
   const std::vector<RealArray>&
                           CalculateDerivatives(GmatBase *obj, Integer wrt,
                                                Integer forMeasurement);
   bool                    MeasurementHasEvents();
   ObjectArray&            GetActiveEvents();
   bool                    ProcessEvent(Event *locatedEvent);
   bool                    WriteMeasurements();

   Integer                 AddMeasurement(TrackingDataAdapter *adapter);  // Needed?
   Integer                 AddMeasurement(TrackingFileSet* tfs);
   void                    AddMeasurementName(std::string measName);
   GmatBase*               GetClone(GmatBase *obj);
   const StringArray&      GetMeasurementNames() const;
   const IntegerArray      GetMeasurementId(const std::string &modelName) const;
   const StringArray&      GetParticipantList();
   std::vector<StringArray> GetSignalPathList();
   Integer                 Calculate(const Integer measurementToCalc,
                                     bool withEvents = false);
   Integer                 CountFeasibleMeasurements(const Integer measurementToCalc) const;
   const MeasurementData*  GetMeasurement(const Integer measurementToGet);
   MeasurementModelBase*   GetMeasurementObject(const Integer measurementToGet);
   Integer                 GetEventCount(const Integer forMeasurement = -1);
   const StringArray&      GetStreamList();
   void                    SetStreamObject(DataFile *newStream);
   bool                    WriteMeasurement(const Integer measurementToWrite);

///// TBD: Do we want something more generic here?
   const StringArray&      GetRampTableDataStreamList();
   void                    SetRampTableDataStreamObject(DataFile *newStream);

   IntegerArray&           GetValidMeasurementList();

   // Observation reader methods needed for estimation
   UnsignedInt             LoadObservations();

///// TBD: Do we want something more generic here?
   // Ramp tables reader method needed for simulator
   void                    LoadRampTables();

   const std::vector<TrackingFileSet*>&  GetAllTrackingFileSets();
   const std::vector<TrackingDataAdapter*>& GetAllTrackingDataAdapters();
   virtual void            SetTransientForces(std::vector<PhysicalModel*> *tf);

   UnsignedInt             GetMeasurementSize();
   UnsignedInt             GetCurrentRecordNumber();
   
   GmatTime                GetEpochGT();
   GmatTime                GetNextEpochGT();

   const ObservationData * GetObsData(const Integer observationToGet = -1);
   ObservationData*        GetObsDataObject(const Integer observationToGet = -1);
   bool                    AdvanceObservation();                                    // made changes for Bug 8 in ticket GMT-4314
   bool                    RemoveObservation(const Integer observationToRemove = -1);
   void                    Reset();
   void                    SetDirection(bool forwards);
   bool                    IsForward();
   
   std::vector<ObservationData>* GetObservationDataList();

   //const std::vector<TrackingFileSet*>     GetTrackingSets() const ;              // It is the same as GetAllTrackingFileSets() function

   /// This function is used to generate tracking data adapters for tracking file set objects having no tracking configs
   bool                    AutoGenerateTrackingDataAdapters();

   bool                    SetStatisticsDataFiltersToDataFiles(UnsignedInt index);

   ObjectArray             GetStatisticsDataFilters(TrackingFileSet* tfs = NULL);

   void                    ClearIonosphereCache();

   void                    SetPassBiasesToTrackingDataAdapters();

protected:
   /// List of the managed measurement models
   StringArray                      modelNames;
   /// List of all participants referenced in the measurement models
   StringArray                      participants;
   /// Pointers to the measurements
   std::vector<TrackingFileSet*>    trackingSets;
   /// Mapping from TrackingFileSets to Adapter names
   std::map<TrackingFileSet*,StringArray> adapterFromTFSMap;
   /// Pointers to the measurements
   std::vector<TrackingDataAdapter*> adapters;
   /// transient forces to pass to the MeasureModels
   std::vector<PhysicalModel *>     *transientForces;

   /// Propagators used by adapters for light time solution
   std::vector<PropSetup*>          *thePropagators;
   /// Mapping for propagator overrides for specific spacecraft
   std::map<std::string, StringArray> *satPropagatorMap;


   /// Current measurement epoch, ignoring event searching
   GmatTime                         anchorEpochGT;
   /// Current measurement epoch, including event searching
   GmatTime                         currentEpochGT;

   /// Measurement calculations
   std::vector<MeasurementData>     measurements;

   /// Observation data from all of the input observation data files
   std::vector<ObservationData>     observations;
   /// The index of the current observation from the vector of observations
   Integer                          obsIndex;

   /// Measurement derivatives
   std::vector<Rmatrix>             derivatives;
   /// Measurement stream objects
   StringArray                      streamNames;
   /// Measurement stream objects
   std::vector<DataFile*>           streamList;


///// TBD: Do we want something more generic here?
   /// ramp table data stream names
   StringArray                      rampTableDataStreamNames;
   /// Ramp table data stream objects
   std::vector<DataFile*>           rampTableDataStreamList;
   /// Maping between unique IDs and the associated data files
   std::map<Integer,DataFile*>      idToRampTableStreamMap;
   /// Association between name of DataFile objects and frequency ramp tables
   std::map<std::string,std::vector<RampTableData> >
                                    rampTables;


   /// Temporary element used to manage events that are ready for processing
   ObjectArray                      activeEvents;
   ///// Association between Events and the MeasurementModels that provided them
   //std::map<Event*,MeasurementModel*>
   //                                 eventMap;

   /// Starting ID for measurement models; used to provide unique IDs in a run
   Integer                          idBase;
   /// One more than the largest assigned ID
   Integer                          largestId;

   /// Maping between unique IDs and the associated data files
   std::map<Integer,DataFile*>      idToStreamMap;

   /// Indices of measurement models that match observations at current epoch
   IntegerArray                     activeMeasurements;
   /// Total number of events that must be evaluated
   Integer                          eventCount;
   /// Flag to indicate simulation mode
   bool                             inSimulationMode;
   /// Flag to indicate direction of measurements
   bool                             isForward;

   Integer                          FindModelForObservation();

private:
   /// Maping between data file index and a list of tracking configuration
   std::map<UnsignedInt, StringArray> trackingConfigsMap;

   void UpdateObservationContent(ObservationData* odPointer);

   std::vector<RampTableData>* GetRampTableForAdapter(TrackingDataAdapter& adapter);

};

#endif /*MeasurementManager_hpp*/
