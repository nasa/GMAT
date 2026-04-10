//$Id: MeasurementManager.cpp 1398 2011-04-21 20:39:37Z  $
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
 * MeasurementManager implementation used in GMAT's estimators and simulator
 */
//------------------------------------------------------------------------------

#include "MeasurementManager.hpp"
#include "MeasurementException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "RealUtilities.hpp"
#include <sstream>            // To build DataStream for a TrackingFileSet

#include "DataFileAdapter.hpp"
#include "PropSetup.hpp"
#include "AcceptFilter.hpp"
#include "RejectFilter.hpp"
#include "Spacecraft.hpp"
#include "Receiver.hpp"

// Temporary to get Adapters hooked up
#include "GmatObType.hpp"
#include "RampTableType.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FLOW
//#define DEBUG_CALCULATE_MEASUREMENTS
//#define DEBUG_GET_ACTIVE_EVENTS
//#define DEBUG_LOAD_OBSERVATIONS
//#define DEBUG_RAMP_TABLE
//#define DEBUG_LOAD_FREQUENCY_RAMP_TABLE
//#define DEBUG_INITIALIZE
//#define DEBUG_MODEL_MAPPING
//#define DEBUG_CALCULATE
//#define DEBUG_ADVANCE_OBSERVATION
//#define DEBUG_EXECUTION
//#define DEBUG_ADAPTERS
//#define DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS

// Selects between old datafile classes and the classes in the DataFile plugin
//#define USE_DATAFILE_PLUGINS


//------------------------------------------------------------------------------
// MeasurementManager()
//------------------------------------------------------------------------------
/**
 * Constructor for the measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager::MeasurementManager() :
   thePropagators    (NULL),
   satPropagatorMap  (NULL),
   anchorEpochGT     (GmatTimeConstants::MJD_OF_J2000),
   currentEpochGT    (GmatTimeConstants::MJD_OF_J2000),
   obsIndex          (0),
   idBase            (10000),
   largestId         (10000),
   eventCount        (0),
   inSimulationMode  (false),
   isForward         (true),
   transientForces   (NULL)
{
}

//------------------------------------------------------------------------------
// ~MeasurementManager()
//------------------------------------------------------------------------------
/**
 * Destructor for the measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager::~MeasurementManager()
{
   //MessageInterface::ShowMessage("MeasurementMaganger destruction!!!!!!!!!!!!\n");
   CleanUp();
}


void MeasurementManager::CleanUp()
{

   modelNames.clear();
   participants.clear();

   // clean up std::vector<TrackingFileSet*>    trackingSets;
   // It cannot delete these objects because they were not created in this class. TrackingFileSet objects are created by Factory and they will be deleted in Sandbox
   trackingSets.clear();

   // clean up std::map<TrackingFileSet*, StringArray> adapterFromTFSMap;
   for (std::map<TrackingFileSet*, StringArray>::iterator i = adapterFromTFSMap.begin(); i != adapterFromTFSMap.end(); ++i)
      (*i).second.clear();
   adapterFromTFSMap.clear();

   // clean up std::vector<TrackingDataAdapter*> adapters;
   // It cannot delete objects in adapters because they were not created in this class. 
   // TrackingDataAdapter objects (belong to a given TrackingFileSet) will be deleted by TrackingFileSet object 
   adapters.clear();

   // clean up std::vector<PhysicalModel *>     *transientForces;
   // This vector are passed by pointer from Spacecraft object. The clean up task has to be handled by Spacecraft objects. 
   //if (transientForces)
   //{
   //   for (Integer i = 0; i < (*transientForces).size(); ++i)
   //   {
   //      if ((*transientForces)[i])
   //         delete (*transientForces)[i];
   //   }
   //   (*transientForces).clear();
   //}

   // clean up PropSetup *thePropagator;
   // It cannot delete because this object was not created in this class
   //if (thePropagator)
   //{
   //   delete thePropagator;
   //   thePropagator = NULL;
   //}
   thePropagators = NULL;
   satPropagatorMap = NULL;

   // clean up std::vector<MeasurementData>     measurements;
   // MeasurementData object in measurements did not exist after TrackingDataAdapter object was deleted. Therefor, it cannot run measurements[i].CleanUp().
   for (Integer i = 0; i < measurements.size(); ++i)
      measurements[i].CleanUp();
   measurements.clear();

   // clean up std::vector<ObservationData>     observations;
   for (Integer i = 0; i < observations.size(); ++i)
      observations[i].Clear();
   observations.clear();
   
   derivatives.clear();
   streamNames.clear();

   // clean up std::vector<DataFile*> streamList;
   for (Integer i = 0; i < streamList.size(); ++i)
   {
      if (streamList[i])
      {
         streamList[i]->CloseStream();
         delete streamList[i];
      }
   }
   streamList.clear();

   rampTableDataStreamNames.clear();

   // clean up std::vector<DataFile*> rampTableDataStreamList;
   for (Integer i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      if (rampTableDataStreamList[i])
      {
         rampTableDataStreamList[i]->CloseStream();
         delete rampTableDataStreamList[i];
      }
   }
   rampTableDataStreamList.clear();

   // clean up std::map<Integer, DataFile*>      idToRampTableStreamMap;
   idToRampTableStreamMap.clear();

   // clean up std::map<std::string, std::vector<RampTableData> > rampTables;
   for (std::map<std::string, std::vector<RampTableData>>::iterator i = rampTables.begin(); i != rampTables.end(); ++i)
   {
      for (Integer j = 0; j < (*i).second.size(); ++j)
      {
         (*i).second[j].Clear();
      }
      (*i).second.clear();
   }
   rampTables.clear();

   //// clean up ObjectArray activeEvents;
   //for (Integer i = 0; i < activeEvents.size(); ++i)
   //{
   //   if (activeEvents[i])
   //      delete activeEvents[i];
   //}
   //activeEvents.clear();

   // clean up std::map<Integer, DataFile*> idToStreamMap;
   idToStreamMap.clear();

   activeMeasurements.clear();

   // clean up std::map<UnsignedInt, StringArray> trackingConfigsMap;
   for (std::map<UnsignedInt, StringArray>::iterator i = trackingConfigsMap.begin(); i != trackingConfigsMap.end(); ++i)
   {
      (*i).second.clear();
   }
   trackingConfigsMap.clear();
   
}


//------------------------------------------------------------------------------
// MeasurementManager(const MeasurementManager &mm)
//------------------------------------------------------------------------------
/**
 * Copy constructor for the measurement manager
 *
 * @param mm The manager that gets copied
 */
//------------------------------------------------------------------------------
MeasurementManager::MeasurementManager(const MeasurementManager &mm) :
   thePropagators    (mm.thePropagators),
   satPropagatorMap  (mm.satPropagatorMap),
   anchorEpochGT     (mm.anchorEpochGT),
   currentEpochGT    (mm.currentEpochGT),
   obsIndex          (mm.obsIndex),
   idBase            (mm.idBase),
   largestId         (mm.largestId),
   eventCount        (mm.eventCount),
   inSimulationMode  (mm.inSimulationMode),
   isForward         (mm.isForward),
   modelNames        (mm.modelNames),
   adapters          (mm.adapters),
   trackingSets      (mm.trackingSets),
   
   transientForces   (NULL)
{
}

//------------------------------------------------------------------------------
// MeasurementManager& operator=(const MeasurementManager &mm)
//------------------------------------------------------------------------------
/**
 * Measurement manager assignment operator
 *
 * @param mm The manager that gets copied
 *
 * @return This measurement manager
 */
//------------------------------------------------------------------------------
MeasurementManager& MeasurementManager::operator=(const MeasurementManager &mm)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasurementManager::operator= enter: <%p> assign from <%p>\n", this, &mm);  
#endif
   if (&mm != this)
   {
      thePropagators   = mm.thePropagators;
      satPropagatorMap = mm.satPropagatorMap;
      anchorEpochGT    = mm.anchorEpochGT;
      currentEpochGT   = mm.currentEpochGT;
      obsIndex         = mm.obsIndex;
      modelNames       = mm.modelNames;
      eventCount       = mm.eventCount;
      inSimulationMode = mm.inSimulationMode;
      isForward        = mm.isForward;
      transientForces  = NULL;

      adapters         = mm.adapters;
      trackingSets     = mm.trackingSets;

      //// Clone the measurements, adapters, and tracking file sets
      //for (std::vector<TrackingDataAdapter*>::iterator i = adapters.begin();
      //      i != adapters.end(); ++i)
      //   delete (*i);
      //adapters.clear();

      //for (std::vector<TrackingFileSet*>::iterator i = trackingSets.begin();
      //      i != trackingSets.end(); ++i)
      //   delete (*i);
      //trackingSets.clear();

      measurements.clear();

      ////for (std::vector<TrackingDataAdapter*>::const_iterator i = mm.adapters.begin();
      ////      i != mm.adapters.end(); ++i)
      ////   adapters.push_back((TrackingDataAdapter*)(*i)->Clone());

      ////for (std::vector<TrackingFileSet*>::const_iterator i = mm.trackingSets.begin();
      ////      i != mm.trackingSets.end(); ++i)
      ////   trackingSets.push_back((TrackingFileSet*)((*i)->Clone()));

      // Should measurements be rebuilt here?
   }

#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasurementManager::operator= exit: <%p> assign from <%p>\n", this, &mm);  
#endif
   return *this;
}


//------------------------------------------------------------------------------
// bool SetPropagator(std::vector<PropSetup*> *props,
//       std::map<std::string, StringArray> *satPropMap)
//------------------------------------------------------------------------------
/**
 * Sets the propagator needed by the tracking data adapters
 *
 * @param props The propagators
 * @param satPropMap The spacecraft/propagator map
 *
 * @return true if the propagator vector pointer is set; false if it is NULL
 */
//------------------------------------------------------------------------------
bool MeasurementManager::SetPropagators(std::vector<PropSetup*> *props,
      std::map<std::string, StringArray> *satPropMap)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Setting the propagator in the measurement "
            "manager to %p\n", props);
   #endif

   thePropagators = props;
   satPropagatorMap = satPropMap;

   return (thePropagators != NULL);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Verifies that the measurement models are ready to calculate measuremetns,
 * and builds internal data structures needed to manage these calculations.
 *
 * @return true is ready to go, false if not
 */
//------------------------------------------------------------------------------
bool MeasurementManager::Initialize()
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::Initialize() method\n");
   #endif

   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::Initialize()\n");
   #endif

   bool retval = true;

   measurements.clear();
   
   for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
   {
      // For each tracking set, It needs to do the following steps to process

      // Step 1: Initialize trackingSets[i]
      if (trackingSets[i]->Initialize() == false)
         return false;

      // Step 2: Load data adapters from tracking file sets to measurement manager
      std::vector<TrackingDataAdapter*> *setAdapters =
            trackingSets[i]->GetAdapters();
      StringArray names;
      for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
      {
         AddMeasurement((*setAdapters)[j]);
         MeasurementData md;
         measurements.push_back(md);
         names.push_back((*setAdapters)[j]->GetName());

         // Set retval?
      }
      adapterFromTFSMap[trackingSets[i]] = names;

      // Step 3: Set stream objects and data filters for all observation data files in trackingSet[i]
      // 3.1. Create DataFile object for each file name
      StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");
      for (UnsignedInt k = 0; k < filenames.size(); ++k)
      {
         // 3.1.1 Create DataFile object
         DataFile *newStream = new DataFile(filenames[k]);
         newStream->SetStringParameter("Filename", filenames[k]);

         // 3.1.2 Create and set a data stream associated with the DataFile object
         GmatObType *got = new GmatObType();                      // ??? what happen for GMAT_OD and GMAT_ODDoppler???   // In new design, GMATInteral data file contains data records with different measurement type
         newStream->SetStream(got);
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("   Adding %s DataFile %s <%p>\n",
                  (newStream->IsInitialized() ? "initialized" :
                  "not initialized"), newStream->GetName().c_str(), newStream);
         #endif

         SetStreamObject(newStream);

         // Associate the adapters with the stream
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Associating %d with %s\n",
                     (*setAdapters)[j]->GetModelID(),
                     newStream->GetName().c_str());
            #endif
            idToStreamMap[(*setAdapters)[j]->GetModelID()] = newStream;
         }// for j loop
         
         if (inSimulationMode)
         {
            if (newStream->OpenStream(inSimulationMode) == false)
               throw MeasurementException("The stream " + filenames[k] + 
                     " failed to open in simulation mode");
         }
      }// for k loop

      // 3.2. Set data filters to data file (only set data filters in estimation mode)
      if (!inSimulationMode)
         SetStatisticsDataFiltersToDataFiles(i);

      // Step 4: Set stream objects for all ramp tables in trackingSet[i] 
      StringArray rampedTablenames = trackingSets[i]->GetStringArrayParameter("RampTable");
      for (UnsignedInt k1 = 0; k1 < rampedTablenames.size(); ++k1)
      {
         DataFile *newStream = new DataFile(rampedTablenames[k1]);
         newStream->SetStringParameter("Filename", rampedTablenames[k1]);

#ifdef DEBUG_RAMP_TABLE
         MessageInterface::ShowMessage("MeasurementManager::Initialize():  Ramp table name = %s\n", rampedTablenames[k1].c_str());
#endif
         RampTableType *rpt = new RampTableType();
         newStream->SetStream(rpt);
         newStream->SetStringParameter("Format", "GMAT_RampTable");

         #ifdef DEBUG_RAMP_TABLE
            MessageInterface::ShowMessage("   Adding %s DataFile %s <%p>\n",
                  (newStream->IsInitialized() ? "initialized" :
                  "not initialized"), newStream->GetName().c_str(), newStream);
         #endif

         SetRampTableDataStreamObject(newStream);

         // Associate the adapters with the stream
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Associating %d with %s\n",
                     (*setAdapters)[j]->GetModelID(),
                     newStream->GetName().c_str());
            #endif
            idToRampTableStreamMap[(*setAdapters)[j]->GetModelID()] = newStream;
         }// for j loop

         if (inSimulationMode)
         {
            if (newStream->OpenStream(inSimulationMode) == false)
               throw MeasurementException("The stream " + rampedTablenames[k1] + 
                     " failed to open in simulation mode");
         }
      }// for k1 loop

   }// for i loop

   // 5. Initialize all data file objects 
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      if (streamList[i]->IsInitialized() == false)
         streamList[i]->Initialize();
   }

///// TBD: Do we want something more generic here?
   // 6. Initialize all ramp table data objects
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      if (rampTableDataStreamList[i]->IsInitialized() == false)
         rampTableDataStreamList[i]->Initialize();
   }


   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage(
            "Exit MeasurementManager::Initialize()\n");
   #endif
#ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Exit MeasurementManager::Initialize() method\n");
#endif

   retval = true;
   return retval;
}


//-----------------------------------------------------------------------------------
// ObjectArray MeasurementManager::GetStatisticsDataFilters(TrackingFileSet* tfs)
//-----------------------------------------------------------------------------------
/** This function is used to get all statistics data filters set to a given 
*      TrackingFileSet.
* @param tfs      given tracking file set. If it is NULL, that means all tracking 
*                 file sets
*
* @return         a list of data filter objects
*/
//-----------------------------------------------------------------------------------
ObjectArray MeasurementManager::GetStatisticsDataFilters(TrackingFileSet* tfs)
{
   
   ObjectArray objList;
   if (tfs == NULL)
   {
      for (Integer i = 0; i < trackingSets.size(); ++i)
      {
         // Get list of data filter objects from trackingSet[i]
         ObjectArray objs = trackingSets[i]->GetRefObjectArray(Gmat::DATA_FILTER);
         
         // Add them to objList
         for (Integer j = 0; j < objs.size(); ++j)
         {
            // Get name of objs[j]
            std::string name = objs[j]->GetName();

            // if it is not already in objList, then add it to the list
            bool found = false;
            for (Integer k = 0; k < objList.size(); ++k)
            {
               if (name == objList[k]->GetName())
               {
                  found = true;
                  break;
               }
            }
            
            if (!found)
               objList.push_back(objs[j]);
         }
      }
   }
   else
   {
      objList = tfs->GetRefObjectArray("DataFilters");
   }
   
   return objList;
}

//-----------------------------------------------------------------------------------
// bool SetStatisticsDataFiltersToDataFiles(Integer option)
//-----------------------------------------------------------------------------------
/** This function is used to set statistics data filters to approriated data file
* as specified in tracking file set objects.
*
* @param i      TrackingFileSet index
*
*/
//-----------------------------------------------------------------------------------
bool MeasurementManager::SetStatisticsDataFiltersToDataFiles(UnsignedInt i)
{
      // 1. Get tracking configs in ID from TrackingFileSet trackingSet[i]
      StringArray tkconfigs;
      std::stringstream ss;
      std::vector<TrackingDataAdapter*>* ads = trackingSets[i]->GetAdapters();
      for (UnsignedInt k = 0; k < ads->size(); ++k)
      {
         //MeasureModel* model = ads->at(k)->GetMeasurementModel();
         //std::vector<ObjectArray*> objArrList = model->GetParticipantObjectLists();
         //for (UnsignedInt k1 = 0; k1 < objArrList.size(); ++k1)
         //{
         //   ss.str("");
         //   ss << "{{";
         //   for (UnsignedInt k2 = 0; k2 < objArrList[k1]->size(); ++k2)
         //   {
         //      ss << objArrList[k1]->at(k2)->GetStringParameter("Id");
         //      if (k2 < objArrList[k1]->size()-1)
         //         ss << ",";
         //   }
         //   ss << "}" << ads->at(k)->GetStringParameter("MeasurementType") << "}";
         //   tkconfigs.push_back(ss.str());
         //   //MessageInterface::ShowMessage("tkconfig: <%s>\n", ss.str().c_str());
         //}
         tkconfigs.push_back(ads->at(k)->GetTrackingConfig());
      }

      // 2. Get all data filters defined in tracking file set object trackingSet[i]
      ObjectArray dataFilterObjects = trackingSets[i]->GetRefObjectArray(Gmat::DATA_FILTER);
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(" &&&&   There are %d data filter objects defined in %s.\n", dataFilterObjects.size(), trackingSets[i]->GetName().c_str());
         for(UnsignedInt j = 0; j < dataFilterObjects.size(); ++j)
            MessageInterface::ShowMessage("filter %d: <%s>\n", j, dataFilterObjects[j]->GetName().c_str());
      #endif

      // 3. Set tracking configs to statistics data filters in order to filter data based on them
      for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
      {
         // set tracking configs to statistics accept filter
         if (dataFilterObjects[k]->IsOfType("AcceptFilter"))
            ((AcceptFilter*)dataFilterObjects[k])->SetTrackingConfigs(tkconfigs);

         // set tracking configs to statistics reject filter
         if (dataFilterObjects[k]->IsOfType("RejectFilter"))
            ((RejectFilter*)dataFilterObjects[k])->SetTrackingConfigs(tkconfigs);
      }

      // 4. Get list of all file names defined in trackingSet[i]
      StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(" &&&&   There are %d filenames defined in %s.\n", filenames.size(), trackingSets[i]->GetName().c_str());
         for(UnsignedInt j = 0; j < filenames.size(); ++j)
            MessageInterface::ShowMessage("file %d: <%s>\n", j, filenames[j].c_str());
      #endif
      
      // 5. Set data filters to the DataFile objects
      for (UnsignedInt j = 0; j < filenames.size(); ++j)
      {
         std::string fileName = filenames[j];
         // 5.1. Specify DataFile object having filename[j]
         DataFile* fileObj = NULL;
         for (UnsignedInt k = 0; k < streamList.size(); ++k)
         {
            if (streamList[k]->GetName() == fileName)
            {
               fileObj = streamList[k];
               break;
            }
         }
         if (fileObj == NULL)
            throw MeasurementException("Error: DataFile object with name '" + filenames[j] + "' was not set in MeasurementManager.\n");
         
         // 5.2. Set data filters of type "Statistics" filter to DataFile object
         for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
         {
            GmatBase*  datafilter = dataFilterObjects[k];

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage(" &&&&   Set data filter <%s,%p> to data file <%s,%p>.\n", datafilter->GetName().c_str(), datafilter, fileObj->GetName().c_str(), fileObj);
            #endif
            // Set statistic filter to DataFile object. 
            // Filter will handle the selection of data record based on file names; therefore, the follwoing code is not needed.
            fileObj->SetDataFilter((DataFilter*)datafilter);
         }// for k loop
      }// for j loop

      // clean up
      tkconfigs.clear();
      ss.clear();
      dataFilterObjects.clear();

   return true;
}



//------------------------------------------------------------------------------
// bool PrepareForProcessing(bool simulating, PropSetup *propagator)
//------------------------------------------------------------------------------
/**
 * This method sets up measurement models for use in simulation or estimation
 *
 * @param simulating Flag to tell the MM that the system is only simulating, so
 *                   derivative calculations are not necessary.
 *
 * @return true unless the stream could not be opened to receive simulated data;
 *         in that case, returns false.
 */
//------------------------------------------------------------------------------
bool MeasurementManager::PrepareForProcessing(bool simulating)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::PrepareForProcessing() method\n");
      MessageInterface::ShowMessage(
            "Working with %d streams\n", streamList.size());
   #endif

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::PrepareForProcessing(%s)\n",
         (simulating ? "true" : "false"));
   #endif
   
   // Verify no pair of streams having the same file name but different data format:
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      std::string fname      = streamList[i]->GetStringParameter("Filename");
      std::string dataformat = streamList[i]->GetStringParameter("Format");
      for(UnsignedInt j = 0; j < streamList.size(); ++j)
      {
         if (i == j)
            continue;
         if ((fname == streamList[j]->GetStringParameter("Filename"))&&(dataformat != streamList[j]->GetStringParameter("Format")))
            throw MeasurementException("Error: DataFile objects '"+ streamList[i]->GetName() +"' and '" + streamList[j]->GetName() + "' have the same file name but different data format\n");
      }

      for(UnsignedInt j = 0; j < rampTableDataStreamList.size(); ++j)
      {
         if ((fname == rampTableDataStreamList[j]->GetStringParameter("Filename"))&&(dataformat != rampTableDataStreamList[j]->GetStringParameter("Format")))
            throw MeasurementException("Error: DataFile objects '"+ streamList[i]->GetName() +"' and '" + rampTableDataStreamList[j]->GetName() + "' have the same file name but different data format\n");
      }
   }
   
   // Pass the propagator settings to the tracking data adapters
   for (UnsignedInt i = 0; i < adapters.size(); ++i) {
      adapters[i]->SetPropagators(thePropagators, satPropagatorMap);
      if (adapters[i]->GetMeasurementModel() != NULL)
         adapters[i]->SetTransientForces(transientForces);
   }
   
   // Open all streams in streamList
   bool retval = true;
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Opening the data stream to file %s\n",
               streamList[i]->GetName().c_str());
      #endif

      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         streamList[i]->SetStringParameter(
               streamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (streamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (streamList[i]->OpenStream(simulating) == false)
         {
            MessageInterface::ShowMessage("Open failed\n");
            retval = false;
         }
      #endif
   }
   
///// TBD: Do we want something more generic here?
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         rampTableDataStreamList[i]->SetStringParameter(
               rampTableDataStreamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (rampTableDataStreamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (rampTableDataStreamList[i]->OpenStream(simulating) == false)
            retval = false;
      #endif
   }
   
   inSimulationMode = simulating;

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::PrepareForProcessing() method: retval = %s\n", (retval?"true":"false"));
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool MeasurementManager::ProcessingComplete()
//------------------------------------------------------------------------------
/**
 * Closes the measurement streams when processing is finished
 *
 * @return true if the streams closed, false if any fail to close
 */
//------------------------------------------------------------------------------
bool MeasurementManager::ProcessingComplete()
{
   #ifdef DEBUG_FLOW
   MessageInterface::ShowMessage(
         "Entered MeasurementManager::ProcessingComplete() method\n");
   #endif

   bool retval = true;

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         if (streamList[i]->CloseFile() == false)
            retval = false;
      #else
         if (streamList[i]->CloseStream() == false)
            retval = false;
      #endif
   }

///// TBD: Do we want something more generic here?
   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      #ifdef USE_DATAFILE_PLUGINS
         if (rampTableDataStreamList[i]->CloseFile() == false)
            retval = false;
      #else
         if (rampTableDataStreamList[i]->CloseStream() == false)
            retval = false;
      #endif
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::ProcessingComplete() method\n");
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * This method performs the final clean-up for a MeasurementManager prior to
 * deletion
 *
 * @return true if the object was cleaned up, false is a problem was encountered
 */
//------------------------------------------------------------------------------
bool MeasurementManager::Finalize()
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::Finalize() method\n");
   #endif

   bool retval = true;
   return retval;
}


//------------------------------------------------------------------------------
// Integer Calculate(const Integer measurementToCalc, bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculate the selected measurement for the current state
 *
 * @param measurementToCalc The ID of the measurement that gets calculated.  If
 *                          the ID is -1, all measurements are attempted
 * @param withEvents Flag used to perform calculation with event data
 *
 * @return The number of measurements that were calculated
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::Calculate(const Integer measurementToCalc,
      bool withEvents)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::Calculate(%d, %s) method\n",
            measurementToCalc, (withEvents?"true":"false"));
   #endif

   Integer successCount = 0;
   eventCount = 0;

   if (measurementToCalc == -1)
   {
      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         // Specify ramp table associated with measurement model models[j]:
         // Note: Only one ramp table is used for a measurement model
///// TBD: Do we want something more generic here?
         StringArray sr = adapters[j]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it
         // passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(observations[obsIndex]);
         
         measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od, rt);

         if (measurements[j].isFeasible)
         {
            ++successCount;
            eventCount += measurements[j].eventCount;
         }
      }
   }
   else
   {
      if ((measurementToCalc < (Integer)adapters.size()) && (measurementToCalc >= 0))
      {
///// TBD: Do we want something more generic here?
         // Specify ramp table associated with tracking data adapter adapters[j]:
         // Note: Only one ramp table is used for a tracking data adapter
         // StringArray sr = models[measurementToCalc]->GetStringArrayParameter("RampTables");
         StringArray sr = adapters[measurementToCalc]->GetStringArrayParameter("RampTables");
         std::vector<RampTableData>* rt = NULL;
         if (sr.size() > 0)
            rt = &(rampTables[sr[0]]);

         // Specify current observation data. If no observation data used, it passes a NULL pointer.
         ObservationData* od = NULL;
         if (!observations.empty())
            od = &(observations[obsIndex]);

         #ifdef DEBUG_CALCULATE
            MessageInterface::ShowMessage("****** adapters[%d]name = '%s'\n",
                  measurementToCalc,
                  adapters[measurementToCalc]->GetName().c_str());
            MessageInterface::ShowMessage("****** observations.size() = %d\n", observations.size());
            if (od == NULL)
               MessageInterface::ShowMessage("****** Observation data is not used in calculation\n");
            else
               MessageInterface::ShowMessage("****** currentObs: epoch = %.12lf, participants: %s  %s,  meas value = %.12lf\n", currentObs->epoch, currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), currentObs->value[0]);
         #endif
         
         measurements[measurementToCalc] =
            adapters[measurementToCalc]->CalculateMeasurement(withEvents, od, rt);
         
         #ifdef DEBUG_CALCULATE
            MessageInterface::ShowMessage("****** measurements[%d] = <%p>,   "
                  ".epoch = %.12lf,   .participants: %s  %s,   "
                  ".value[0] = %.12le\n", measurementToCalc,
                  &measurements[measurementToCalc],
                  measurements[measurementToCalc].epoch,
                  measurements[measurementToCalc].participantIDs[0].c_str(),
                  measurements[measurementToCalc].participantIDs[1].c_str(),
                  measurements[measurementToCalc].value[0]);
         #endif

         if ((measurements[measurementToCalc].isFeasible) || 
            (measurements[measurementToCalc].unfeasibleReason.substr(0, 1) == "B" || measurements[measurementToCalc].unfeasibleReason.substr(0, 1) == "H"))
         {
            successCount = 1;
            eventCount = measurements[measurementToCalc].eventCount;
         }
      }
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "   Found %d events to process\n", eventCount);
      MessageInterface::ShowMessage(
            "Exit MeasurementManager::Calculate(%d, %s) method\n",
            measurementToCalc, (withEvents?"true":"false"));
   #endif

   return successCount;
}


//------------------------------------------------------------------------------
// Integer CountFeasibleMeasurements(const Integer measurementToCalc)
//------------------------------------------------------------------------------
/**
 * Count the number of feasible measurements which were last calculated
 *
 * @param measurementToCalc The ID of the measurement that was calculated.  If
 *                          the ID is -1, all measurements are checked
 *
 * @return The number of calculated measurements that were feasible
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::CountFeasibleMeasurements(const Integer measurementToCalc) const
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::CountFeasibleMeasurements(%d, %s) method\n",
            measurementToCalc, (withEvents?"true":"false"));
   #endif

   const MeasurementData *measurement;
   Integer successCount = 0;

   if (measurementToCalc == -1)
   {
      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         measurement = &adapters[j]->GetMeasurement();

         if (measurement->isFeasible)
         {
            ++successCount;
         }
      }
   }
   else
   {
      if ((measurementToCalc < (Integer)adapters.size()) && (measurementToCalc >= 0))
      {
         measurement = &adapters[measurementToCalc]->GetMeasurement();

         if ((measurement->isFeasible) || 
            (measurement->unfeasibleReason.substr(0, 1) == "B") || measurements[measurementToCalc].unfeasibleReason.substr(0, 1) == "B" ||
            (measurement->unfeasibleReason.substr(0, 1) == "H") || measurements[measurementToCalc].unfeasibleReason.substr(0, 1) == "H")
         {
            successCount = 1;
         }
      }
   }

   return successCount;
}

//------------------------------------------------------------------------------
// MeasurementData* GetMeasurement(const Integer measurementToGet)
//------------------------------------------------------------------------------
/**
 * Retrieves a calculated measurement
 *
 * @param measurementToGet ID of the desired measurement
 *
 * @return Pointer to the measurement, or NULL if the measuremetn was not
 *         available
 */
//------------------------------------------------------------------------------
const MeasurementData* MeasurementManager::GetMeasurement(
         const Integer measurementToGet)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::GetMeasurement(measurentToGet = %d) method\n", measurementToGet);
   #endif

   MeasurementData *theMeasurement = NULL;
   if ( (measurementToGet >= 0) &&
        (measurementToGet < (Integer)measurements.size()))
      theMeasurement = &measurements[measurementToGet];

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::GetMeasurement() method\n");
   #endif

   return theMeasurement;
}


//------------------------------------------------------------------------------
// MeasurementModel* GetMeasurementObject(const Integer measurementToGet)
//------------------------------------------------------------------------------
/**
 * Accessor for specific measurement model objects
 *
 * @param measurementToGet Index to the requested measurement model
 *
 * @return The model
 */
//------------------------------------------------------------------------------
MeasurementModelBase* MeasurementManager::GetMeasurementObject(
      const Integer measurementToGet)
{
   MeasurementModelBase *retval = NULL;

   if ((measurementToGet >= 0) && (measurementToGet < (Integer)adapters.size()))
      retval = adapters[measurementToGet];


   return retval;
}


//-----------------------------------------------------------------------------
// Integer GetEventCount(const Integer forMeasurement)
//-----------------------------------------------------------------------------
/**
 * Returns the number of events associated with a specific measurement
 *
 * @param forMeasurement The index of the measurement
 *
 * @return The number of associated events
 */
//-----------------------------------------------------------------------------
Integer MeasurementManager::GetEventCount(const Integer forMeasurement)
{
   #ifdef DEBUG_EVENTS
      MessageInterface::ShowMessage("MeasurementManager::GetEventCount(%d) "
            "called", forMeasurement);
   #endif
   Integer retval = 0;

   if (forMeasurement == -1)
      retval = eventCount;
   else if ((forMeasurement >= 0) &&
            (forMeasurement < (Integer)measurements.size()))
      retval = measurements[forMeasurement].eventCount;

   #ifdef DEBUG_EVENTS
      MessageInterface::ShowMessage("; Event count is %d\n", retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(const Integer measurementToWrite)
//------------------------------------------------------------------------------
/**
 * Sends the selected measurement to a MeasurementStream
 *
 * @param measurementToWrite ID of the calculated measurement that is to be
 *                           written
 *
 * @return true if the measurement was sent to a MeasurementStream; false if
 *         not (either because the stream was not available or because the
 *         measurement is not possible)
 */
//------------------------------------------------------------------------------
bool MeasurementManager::WriteMeasurement(const Integer measurementToWrite)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(
            "Entered MeasurementManager::WriteMeasurement() method\n");
   #endif

   bool wasWritten = false;

   return wasWritten;
}



//-----------------------------------------------------------------------------
// UsignedInt LoadObservations()
//-----------------------------------------------------------------------------
/**
 * Opens the observation data sources and reads in all available observations.
 *
 * return   number of observation data
 *
 * @todo Once multiple observations sources are in use, this method will need to
 * sort the observations into time order.
 */
//-----------------------------------------------------------------------------
void MeasurementManager::UpdateObservationContent(ObservationData* odPointer)
{
   if (odPointer == NULL)
      return;

   StringArray scNames = trackingSets[0]->GetListOfObjects(Gmat::SPACECRAFT);

   // Specify spacecraft Id based on sensor Id for GPS Point Solution 
   if (odPointer->typeName == "GPS_PosVec")
   {
      // Get receiver Id
      std::string receiveId = odPointer->sensorIDs[0];

      // Specify spacecraft Id for given receiver Id
      std::string scId = "";
      for (UnsignedInt j = 0; j < scNames.size(); ++j)
      {
         bool found = false;
         Spacecraft* sc = (Spacecraft*)(trackingSets[0]->GetConfiguredObject(scNames[j]));
         ObjectArray objList = sc->GetRefObjectArray(Gmat::HARDWARE);
         for (UnsignedInt k = 0; k < objList.size(); ++k)
         {
            if (objList[k]->IsOfType("Receiver"))
            {
               std::string id = ((Receiver*)objList[k])->GetStringParameter("Id");
               if (id == receiveId)
               {
                  found = true;
                  break;
               }
            }
         }

         // Get spacecraft Id
         if (found)
         {
            scId = sc->GetStringParameter("Id");
            break;
         }
      }

      // Update participantID in ObservationData object
      odPointer->participantIDs[0] = scId;
      
   }
}


UnsignedInt MeasurementManager::LoadObservations()
{
   #ifdef DEBUG_LOAD_OBSERVATIONS
      MessageInterface::ShowMessage(
         "Entered MeasurementManager::LoadObservations() method\n");
   #endif

   #ifdef USE_DATAFILE_PLUGINS
      DataFileAdapter dfa;
   #endif

   // Open all streams in streamList
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      #ifdef DEBUG_LOAD_OBSERVATIONS
         MessageInterface::ShowMessage("Opening the data stream to file %s\n",
               streamList[i]->GetName().c_str()); 
      #endif

      #ifdef USE_DATAFILE_PLUGINS
         std::string writeMode = (simulating ? "w" : "r");
         streamList[i]->SetStringParameter(
               streamList[i]->GetParameterID("ReadWriteMode"), writeMode);

         if (streamList[i]->OpenFile() == false)
            retval = false;
      #else
         if (streamList[i]->OpenStream(false) == false)
            throw MeasurementException("Error: Cannot open file '" + streamList[i]->GetName() + "'.\n");
      #endif
   }
   
   // Initialize trackingConfigsMap
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      StringArray sa;
      trackingConfigsMap[i] = sa;
   }
   

   std::map<std::string, Integer> totalCount;
   totalCount["Invalid measurement value"]        = 0;
   totalCount["Record duplication or time order"] = 0;

   observations.clear();

   std::vector<UnsignedInt> numRec;                    // numRec[i] is number of records of data file specified by streamList[i] 
   std::vector<UnsignedInt> count;                     // count[i] is number of all accepted records associated with file specified by streamList[i] after applying statistic filters
   std::vector<ObservationData*> dataBuffer;           // dataBuffer[i] contains the current data record read from streamList[i]  
   ObservationData* odPointer;
   
   // Get A list of GMAT objects
   if (trackingSets.size() == 0)
      throw MeasurementException("Error: No TrackingFileSet was defined in GMAT script.\n");
   
   for (UnsignedInt i = 0; i < streamList.size(); ++i)
   {
      odPointer = streamList[i]->ReadObservation();
      UpdateObservationContent(odPointer);             // It is only used for GPS Point Solution
      dataBuffer.push_back(odPointer);
      
      if (odPointer == NULL)
         numRec.push_back(0);
      else
         numRec.push_back(1);

      count.push_back(0);
   }
   
   ObservationData od;
   while (true)
   {
      // 1. get a data record in data buffer with smallest value of epoch
      UnsignedInt minIndex = dataBuffer.size();     // point to the outside of data buffer
      for (UnsignedInt i = 0; i < dataBuffer.size(); ++i)
      {
         if (dataBuffer[i] == NULL)
            continue;

         if (minIndex == dataBuffer.size())
            minIndex = i;
         else
         {
            if (dataBuffer[i]->epoch < dataBuffer[minIndex]->epoch)
               minIndex = i;
         }
      }
      
      // 2. if dada buffer contains all NULL data record (that means all streams at EOF), then exit while loop
      if (minIndex == dataBuffer.size())
         break;
      
      // 3. Filter the data record do it for all filters
      Integer rejectedReason = 0;
      ObservationData* obsData = dataBuffer[minIndex];
      // This observation data is belong to data file: streamList[minIndex]. Therefore, it needs to use filters defined in that data file object
      ObservationData* selectedData = streamList[minIndex]->FilteringData(obsData, rejectedReason);
      
      // 4. if it passes all filters then add it to observations
      if (selectedData != NULL)
      {
         od = *selectedData;
         observations.push_back(od);
         count[minIndex] = count[minIndex] + 1;

         //@todo: generate tracking config here
         std::stringstream ss;
         ss << "{";
         for (UnsignedInt i = 0; i < od.participantIDs.size(); ++i)
         {
            if (i != 0)
               ss << ",";
            ss << od.participantIDs[i];
            if (od.sensorIDs[i] != "")
               ss << "." << od.sensorIDs[i];
         }
         ss << "}," << od.typeName;

         if (trackingConfigsMap[minIndex].empty())
            trackingConfigsMap[minIndex].push_back(ss.str());
         else if (find(trackingConfigsMap[minIndex].begin(), trackingConfigsMap[minIndex].end(), ss.str()) == trackingConfigsMap[minIndex].end())
            trackingConfigsMap[minIndex].push_back(ss.str());
      }
      
      // 5. count throw recods based on rejectedReason
      switch (rejectedReason)
      {
         case 1:
            break;
         case 2:
            break;
         case 3:
            ++totalCount["Invalid measurement value"];
            break;
         case 4:
            ++totalCount["Record duplication or time order"];
            break;
         case 5:
            break;
         default:
            {
               if (rejectedReason >=6)
               { 
                  Integer filterIndex = rejectedReason - 6;
                  std::vector<DataFilter*>& filters = streamList[minIndex]->GetFilterList();                  
                  if (filterIndex < filters.size())
                  {
                     std::string filterName = "";
                     if (filters[filterIndex]->IsOfType("AcceptFilter"))
                     {
                        filterName = "All Accept Filter";
                     }
                     else if (filters[filterIndex]->IsOfType("RejectFilter"))
                     {
                        filterName = "RejectFilter ";
                        filterName += filters[filterIndex]->GetName();
                     }
                     ++totalCount[filterName];
                  }
               }
            }
      }
      
      // 6. Read data record from streamList[minIndex] to fill data buffer if the stream is not EOF
      if (dataBuffer[minIndex] != NULL)
      {
         dataBuffer[minIndex] = streamList[minIndex]->ReadObservation();
         UpdateObservationContent(dataBuffer[minIndex]);             // It is only used for GPS Point Solution
         if (dataBuffer[minIndex] != NULL)
            numRec[minIndex] = numRec[minIndex] + 1;        // count up number of read records if it really has one record read from file
      }
   }
   
   // 7. Display all statistic of data records
   //Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   MessageInterface::ShowMessage("Number of thrown records due to:\n");
   for(std::map<std::string,Integer>::iterator i = totalCount.begin(); i != totalCount.end(); ++i)
   {
      MessageInterface::ShowMessage("     .%s : %d\n", i->first.c_str(), i->second); 
   }

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
      MessageInterface::ShowMessage("Data file '%s' has %d of %d records used for estimation.\n", streamList[i]->GetStringParameter("Filename").c_str(), count[i], numRec[i]);

   #ifdef DEBUG_LOAD_OBSERVATIONS
      MessageInterface::ShowMessage(" observations.size() = %d\n", observations.size());
      for (UnsignedInt i = 0; i < observations.size(); ++i)
      {
         if (observations[i].typeName == "GPS_PosVec")
            MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participant: %s  sensor: %s   observation data: %.12lf   %.12lf   %.12lf\n", observations[i].dataFormat.c_str(), observations[i].epoch, observations[i].typeName.c_str(), observations[i].type, observations[i].participantIDs[0].c_str(), observations[i].sensorIDs[0].c_str(), observations[i].value[0], observations[i].value[1], observations[i].value[2]);
         else
            MessageInterface::ShowMessage(" Data type = %s    A1MJD epoch: %.15lf   measurement type = <%s, %d>   participants: %s   %s   observation data: %.12lf\n", observations[i].dataFormat.c_str(), observations[i].epoch, observations[i].typeName.c_str(), observations[i].type, observations[i].participantIDs[0].c_str(), observations[i].participantIDs[1].c_str(), observations[i].value[0]);
      }
   #endif
   
   // Set the current data pointer to the first observation value
   obsIndex = isForward ? 0 : (observations.size()-1);
   MessageInterface::ShowMessage("Total number of load records : %d\n\n", observations.size());

   for(UnsignedInt i = 0; i < trackingConfigsMap.size(); ++i)
   {
      MessageInterface::ShowMessage("List of tracking configurations (present in participant ID) for load records from data file '%s':\n", streamList[i]->GetName().c_str());
      if (trackingConfigsMap[i].size() == 0)
         MessageInterface::ShowMessage("   None\n");
      else
      {
         for (UnsignedInt j = 0; j < trackingConfigsMap[i].size(); ++j)
            MessageInterface::ShowMessage("   Config %d: {%s}\n", j, trackingConfigsMap[i].at(j).c_str());
      }
   }
   MessageInterface::ShowMessage("\n");

   for (UnsignedInt i = 0; i < streamList.size(); ++i)
      streamList[i]->CleanUp();

   #ifdef DEBUG_LOAD_OBSERVATIONS
     MessageInterface::ShowMessage(
         "Exit MeasurementManager::LoadObservations() method\n");
   #endif

   return observations.size(); 
}


//#define APPLY_PASSBREAK_TIMEGAP_TO_DATASET
void MeasurementManager::SetPassBiasesToTrackingDataAdapters()
{
#ifdef APPLY_PASSBREAK_TIMEGAP_TO_DATASET
   // Step 1: Get pass break time gap for each tracking data config
   std::string currentConfig;
   Real timeGapLimit;
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      Real currentTimeGapLimit = adapters[i]->GetRealParameter("TimeGapForPassBreak");
      currentConfig = adapters[i]->GetTrackingConfig();
      timeGapLimit = GmatMathUtil::Max(timeGapLimit, currentTimeGapLimit);
   }

   // Step 2: Specify start epoch for each pass. Start epoches are applied for all tracking data
   std::vector<GmatTime*> passBreakStartEpoches;
   GmatTime prevEpoch(0.0);
   GmatTime currentEpoch;
   for (Integer i = 0; i < observations.size(); ++i)
   {
      currentEpoch = observations[i].epochGT;
      if (prevEpoch.GetMjd() == 0.0)
      {
         GmatTime* startEpoch = new GmatTime(currentEpoch);
         passBreakStartEpoches.push_back(startEpoch);
      }
      else
      {
         // if time gap is greater than time gap limit then adding new pass
         Real timeGap = (currentEpoch - prevEpoch).GetTimeInSec();
         if (timeGap > timeGapLimit)
         {
            GmatTime* startEpoch = new GmatTime(currentEpoch);
            passBreakStartEpoches.push_back(startEpoch);
         }
      }
      prevEpoch = currentEpoch;
   }

   // Step 3: Set pass start epoches to tracking data adapters
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      // Get ErrorModel belonging to this adapter 
      std::string measType = adapters[i]->GetTypeName();
      ErrorModel* errmodel = adapters[i]->GetMeasurementErrorModel(0, measType, 2);
      
      // Set start epoch for each pass in ErrorModel
      errmodel->SetPassBiasStartEpoches(passBreakStartEpoches);

      // Set initial value of each pass bias in ErrorModel
      Real bias = errmodel->GetRealParameter("Bias");
      RealArray passBiases;
      for (Integer j = 0; j < passBreakStartEpoches.size(); ++j)
         passBiases.push_back(bias);
      Rvector passBiasesVec(passBiases);
      errmodel->SetRvectorParameter("PassBiases", passBiasesVec);
   }

#else


   //Build adapterconfig->ErrorModel map:
   std::map<std::string, ErrorModel*> adaptConfigErrModel;
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      adaptConfigErrModel[adapters[i]->GetTrackingConfig()] = adapters[i]->GetMeasurementErrorModel(0, adapters[i]->GetTypeName(), 2);
   }

   // Step 1: Get pass break time gap for each tracking data config
   std::map<std::string, Real> timeGapLimitMap;                  // map between tracking config and pass break time gap  
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      Real timeGapLimit = adapters[i]->GetRealParameter("TimeGapForPassBreak");
      std::string adapterConfig = adapters[i]->GetTrackingConfig();

      if (timeGapLimitMap.empty())
         timeGapLimitMap[adapterConfig] = timeGapLimit;
      else
      {
         std::map<std::string, Real>::iterator j = timeGapLimitMap.begin();
         for (; j != timeGapLimitMap.end(); ++j)
         {
            if ((*j).first == adapterConfig)
            {
               if (timeGapLimit > 0.0)
               {
                  if (timeGapLimitMap[adapterConfig] > 0.0)
                     timeGapLimitMap[adapterConfig] = GmatMathUtil::Min(timeGapLimit, timeGapLimitMap[adapterConfig]);
                  else
                     timeGapLimitMap[adapterConfig] = timeGapLimit;
               }

               break;
            }
         }

         if (j == timeGapLimitMap.end())
            timeGapLimitMap[adapterConfig] = timeGapLimit;
      }
   }

   #ifdef DEBUG_SET_PASSBIASES
   MessageInterface::ShowMessage("Time gap limit map:\n");
   for (std::map<std::string, Real>::iterator i = timeGapLimitMap.begin(); i != timeGapLimitMap.end(); ++i)
      MessageInterface::ShowMessage("**  <%s>  %f\n", (*i).first.c_str(), (*i).second);
   #endif

   // Step 2: Specify start epoch for each pass. Each tracking data has its own pass break start epoches
   std::map<ErrorModel*, std::vector<GmatTime*>> startEpochesMap;   // map between tracking config and pass break start epoches 
   std::map<std::string, GmatTime> prevEpochMap;                    // map between tracking config and its previous observation epoch   
   GmatTime currentEpoch;
   Real currentTimeGapLimit;
   for (Integer i = 0; i < observations.size(); ++i)
   {
      // Specify configure of the current observation
      std::string obDataConfig = observations[i].GetTrackingConfig();
      
      #ifdef DEBUG_SET_PASSBIASES
      MessageInterface::ShowMessage("observation %d: config = <%s>  epoch = %lf\n", i, obDataConfig.c_str(), observations[i].epochGT.GetMjd());
      #endif

      // Specify time gap limit for the current observation's config
      bool configFound = false;
      currentTimeGapLimit = 0.0;
      for (std::map<std::string, Real>::iterator j = timeGapLimitMap.begin(); j != timeGapLimitMap.end(); ++j)
      {
         if ((*j).first == obDataConfig)
         {
            currentTimeGapLimit = (*j).second;
            configFound = true;
            break;
         }
      }
      if (currentTimeGapLimit == 0.0)
         currentTimeGapLimit = 1.0e70;


      // Specify value of startEpochesMap
      if (configFound)
      {
         // This section is used to set value for startEpochesMap
         currentEpoch = observations[i].epochGT;
         bool prevEpochFound = false;
         GmatTime prevEpoch;
         std::map<std::string, GmatTime>::iterator j = prevEpochMap.begin();
         for (; j != prevEpochMap.end(); ++j)
         {
            if ((*j).first == obDataConfig)
            {
               prevEpoch = (*j).second;
               prevEpochFound = true;
               break;
            }
         }

         if (prevEpochFound)
         {
            // Update value of startEpochesMap
            Real currentTimeGap = (currentEpoch - prevEpoch).GetTimeInSec();
            if (currentTimeGap > currentTimeGapLimit)
            {
               startEpochesMap[adaptConfigErrModel[obDataConfig]].push_back(new GmatTime(currentEpoch));
            }
         }
         else
         {
            if (startEpochesMap.count(adaptConfigErrModel[obDataConfig]) > 0)
            {
               //Adds a start epoch in the case that there are two tracking configs with the same error model
               //This can happen if there are 2-way and 3-way measurements in close succession. 
               startEpochesMap[adaptConfigErrModel[obDataConfig]].push_back(new GmatTime(currentEpoch));
            }
            else
            {
               std::vector<GmatTime*> startEpoches;
               startEpoches.push_back(new GmatTime(currentEpoch));
               startEpochesMap[adaptConfigErrModel[obDataConfig]] = startEpoches;
            }
         }         
      }
      else
      {
         std::vector<GmatTime*> startEpoches;
         startEpoches.push_back(new GmatTime(currentEpoch));
         startEpochesMap[adaptConfigErrModel[obDataConfig]] = startEpoches;
      }

      // Update value of prevEpochMap
      prevEpochMap[obDataConfig] = currentEpoch;
   }


   // Step 3: Set pass break start epoches to tracking data adapters
   std::map<std::string, Real> passBiasesMap;
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      // Get the config of this adapter
      std::string adapterConfig = adapters[i]->GetTrackingConfig();
      // Get ErrorModel of this adapter 
      ErrorModel* errmodel = adaptConfigErrModel[adapterConfig];
      
      #ifdef DEBUG_SET_PASSBIASES
      MessageInterface::ShowMessage("adapter[%d]: config = <%s>  time gap limit = %f\n", i, adapterConfig.c_str(), adapters[i]->GetRealParameter("TimeGapForPassBreak"));
      #endif

      // If time gap limit was defined for this adapter, then set up value for pass biases and start epoches, otherwise skip to the nexr adapter
      if (adapters[i]->GetRealParameter("TimeGapForPassBreak") > 0.0)
      {
         if (startEpochesMap.find(errmodel) != startEpochesMap.end())
         {
            // Set start epoch for each pass in ErrorModel
            errmodel->SetPassBiasStartEpoches(startEpochesMap[errmodel]);

#ifdef DEBUG_SET_PASSBIASES
            MessageInterface::ShowMessage("****   start epoches = [");
            for (Integer j = 0; j < startEpochesMap[adapterConfig].size(); ++j)
               MessageInterface::ShowMessage("%lf   ", startEpochesMap[adapterConfig][j]->GetMjd());
            MessageInterface::ShowMessage("]\n");
#endif

            // Set initial value of each pass bias in ErrorModel
            Real bias = errmodel->GetRealParameter("Bias");
            RealArray passBiases;
            for (Integer j = 0; j < startEpochesMap[errmodel].size(); ++j)
               passBiases.push_back(bias);
            Rvector passBiasesVec(passBiases);
            errmodel->SetRvectorParameter("PassBiases", passBiasesVec);

#ifdef DEBUG_SET_PASSBIASES
            MessageInterface::ShowMessage("****   pass biases = [");
            for (Integer j = 0; j < passBiasesVec.GetSize(); ++j)
               MessageInterface::ShowMessage("%f   ", passBiasesVec(j));
            MessageInterface::ShowMessage("]\n");
#endif

         }
      }
   }
#endif
}


//-----------------------------------------------------------------------------
// bool AutoGenerateTrackingDataAdapters()
//-----------------------------------------------------------------------------
/**
* This function is used to generate TrackingDataAdapters automatically based on
* observation data.
*
*/
//-----------------------------------------------------------------------------
bool MeasurementManager::AutoGenerateTrackingDataAdapters()
{
#ifdef DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS
   MessageInterface::ShowMessage("Enter MeasurementManager::AutoGenerateTrackingDataAdapters()\n");
#endif

   // Get a list of TrackingFileSet having no tracking configs
   for(UnsignedInt i = 0; i < trackingSets.size(); ++i)
   {
      StringArray list = trackingSets[i]->GetStringArrayParameter("AddTrackingConfig");
      if (list.empty())
      {
         // For each tracking file set object, It generates tracking data adapters for trackingSets[i]
         StringArray createList;

         // 1. Get list data files
         StringArray fileNamesList = trackingSets[i]->GetStringArrayParameter("FileName");
         for (UnsignedInt j = 0; j < fileNamesList.size(); ++j)
         {
            // Get file index for fileNamesList[j]
            UnsignedInt index;
            for (UnsignedInt k = 0; k < streamList.size(); ++k)
            {
               if (GmatStringUtil::ToUpper(fileNamesList[j]) == GmatStringUtil::ToUpper(streamList[k]->GetName()))
               {
                  index = k;
                  break;
               }
            }

            // Get a list of tracking configurations for this data file
            StringArray tclist = trackingConfigsMap[index];
            for (UnsignedInt k = 0; k < tclist.size(); ++k)
            {
               if (createList.empty())
                  createList.push_back(tclist[k]);
               else if (find(createList.begin(), createList.end(), tclist[k]) == createList.end())
                  createList.push_back(tclist[k]);
            }
         }

         // 2. Create tracking data adapters for trackingSets[i] based on createList
         // 2.0. Get a list of stations and spacecrafts
         ObjectMap objectmap = trackingSets[i]->GetConfiguredObjectMap();
         ObjectArray partList;
         for (ObjectMap::iterator j = objectmap.begin(); j != objectmap.end(); ++j)
         {
            if (((*j).second->IsOfType(Gmat::GROUND_STATION))||
                ((*j).second->IsOfType(Gmat::SPACECRAFT)))
               partList.push_back((*j).second);
         }

         // 2.1. Prepare all inputs
         std::vector<StringArray> strands;
         std::vector<StringArray> sensors;
         StringArray types;

         for (UnsignedInt j = 0; j < createList.size(); ++j)
         {
            StringArray participants;
            StringArray sens;
            std::string participantID;
            StringArray nameList;
            std::string sensorID;
            Integer count;
            GmatBase* obj;

            std::string config = createList[j];
            size_t pos = config.find_last_of(',');                     // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
            std::string type = config.substr(pos+1, config.size()-(pos+1));
            std::string strand = config.substr(1, pos-2);

            while (strand != "")
            {
               pos = strand.find_first_of(',');
               if (pos != strand.npos)
               {
                  nameList = GmatStringUtil::ParseName(strand.substr(0, pos));
                  participantID = nameList[0];
                  if (nameList.size() == 2)
                     sensorID = nameList[1];

                  strand = strand.substr(pos+1);
               }
               else
               {
                  nameList = GmatStringUtil::ParseName(strand);
                  participantID = nameList[0];
                  if (nameList.size() == 2)
                     sensorID = nameList[1];

                  strand = "";
               }

               // Convert participant ID to participant object
               count = 0;
               obj = NULL;
               for (UnsignedInt k = 0; k < partList.size(); ++k)
               {
                  if (partList[k]->GetStringParameter("Id") == participantID)
                  {
                     obj = partList[k];
                     ++count;
                  }
               }
               if (count == 0)
                  throw MeasurementException("Error: Failed to generate tracking configuration due to neither station nor spacecraft defined in your script has Id = '" + participantID + "'\n");
               else if (count > 1)
                  throw MeasurementException("Error: Failed to generate tracking configuation due to 2 or more GMAT objects having the same Id = '" + participantID + "'\n");

               
               GmatBase* receiver = NULL;
               if (sensorID != "")
               {
                  if ((obj!= NULL)&&(obj->IsOfType(Gmat::SPACECRAFT)))
                  {
                     // Get Receiver sensor from spacecraft 
                     ObjectArray hwList = ((Spacecraft*)obj)->GetRefObjectArray("Hardware");
                     for (UnsignedInt hwIndex = 0; hwIndex < hwList.size(); ++hwIndex)
                     {
                        if ((hwList[hwIndex]->GetTypeName() == "Receiver") && (hwList[hwIndex]->GetStringParameter("Id") == sensorID))
                        {
                           receiver = hwList[hwIndex];
                           break;
                        }
                     }
                     if (receiver == NULL)
                        --count;
                  }
               }               
               if (count == 0)
                  throw MeasurementException("Error: Failed to generate tracking configuration due to neither station nor spacecraft defined in your script has Id = '" + participantID + "'\n" );
               
               // Set name
               participants.push_back(obj->GetName());
               if (receiver == NULL)
                  sens.push_back("");
               else
                  sens.push_back(receiver->GetName());

               // Set reference object for tracking file set
               try
               {
                  trackingSets[i]->SetRefObject(obj, obj->GetType(), obj->GetName());
               }
               catch(...)
               {
                  // skip when it fail to set referent object
               }
            }
            
            strands.push_back(participants);
            sensors.push_back(sens);
            types.push_back(type);
         }
         
         // 2.2. Generate tracking configs
         trackingSets[i]->GenerateTrackingConfigs(strands, sensors, types);
         
         // 3. Set stream objects and data filters for all observation data files in trackingSet[i]
         // 3.1. Get list of all data filters defined in trackingSet[i]
         ObjectArray dataFilterObjects = trackingSets[i]->GetRefObjectArray(Gmat::DATA_FILTER);
         // 3.2. Get list of all file names defined in trackingSet[i]
         StringArray filenames = trackingSets[i]->GetStringArrayParameter("FileName");

         for(UnsignedInt j = 0; j < filenames.size(); ++j)
         {
            // 3.3. Set data filter to data file object
            // 3.3.1. Get data file object with name filenames[j]
            DataFile* fileObj = NULL;
            for (UnsignedInt k = 0; k < streamList.size(); ++k)
            {
               if (streamList[k]->GetName() == filenames[j])
               {
                  fileObj = streamList[k];
                  break;
               }
            }

            // 3.3.2. Set data filters of type "Statistics" filter to DataFile object
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage(" &&&&   There are %d data filter objects.\n", dataFilterObjects.size());
            #endif
            for (UnsignedInt k = 0; k < dataFilterObjects.size(); ++k)
            {
               if (dataFilterObjects[k]->IsOfType("AcceptFilter") ||
                   dataFilterObjects[k]->IsOfType("RejectFilter"))
               {
                  // data filter is only set to data file if and only if datafilter.Filenames parameter 
                  // is an empty list or contains the name of data file.
                  StringArray nameList = dataFilterObjects[k]->GetStringArrayParameter("FileNames");
               
                  bool setfilter = false;
                  if (nameList.size() == 0)
                     setfilter = true;
                  else
                  {
                     if (dataFilterObjects[k]->IsOfType("AcceptFilter") &&
                         ((find(nameList.begin(), nameList.end(), "From_AddTrackingConfig") != nameList.end()) ||
                         (find(nameList.begin(), nameList.end(), "All") != nameList.end())))
                        setfilter = true;
                     else
                     {
                        for (UnsignedInt q = 0; q < nameList.size(); ++q)
                        {
                           if (GmatStringUtil::ToUpper(nameList[q]) == GmatStringUtil::ToUpper(filenames[j]))
                           {
                              setfilter = true;
                              break;
                           }
                        }
                     }
                  }

                  if (setfilter)
                  {
                     #ifdef DEBUG_INITIALIZATION
                        MessageInterface::ShowMessage(" &&&&   Set data filter <%s,%p> to data file <%s,%p>.\n", dataFilterObjects[k]->GetName().c_str(), dataFilterObjects[k], fileObj->GetName().c_str(), fileObj);
                     #endif
                     fileObj->SetDataFilter((DataFilter*)dataFilterObjects[k]);
                  }
               }
            }
         }

         // 4. Load data adapters from tracking file sets to measurement manager
         std::vector<TrackingDataAdapter*> *setAdapters = trackingSets[i]->GetAdapters();
         StringArray names;
         for (UnsignedInt j = 0; j < setAdapters->size(); ++j)
         {
            AddMeasurement((*setAdapters)[j]);
            MeasurementData md;
            measurements.push_back(md);
            names.push_back((*setAdapters)[j]->GetName());
         }
         adapterFromTFSMap[trackingSets[i]] = names;
      }
      else
         MessageInterface::ShowMessage("****   No tracking configuration was "
               "generated because the tracking configuration is defined in the "
               "script.\n");
   }// for i loop


#ifdef DEBUG_AUTO_GENERATE_TRACKINGDATAADAPTERS
   MessageInterface::ShowMessage("Exit MeasurementManager::AutoGenerateTrackingDataAdapters()\n");
#endif
   return true;
}


//-----------------------------------------------------------------------------
// std::vector<ObservationData>* GetObservationDataList()
//-----------------------------------------------------------------------------
/**
* Get a list of all observations
*/
//-----------------------------------------------------------------------------
std::vector<ObservationData>* MeasurementManager::GetObservationDataList()
{
   return &observations;
}


//-----------------------------------------------------------------------------
// void LoadRampTables()
//-----------------------------------------------------------------------------
/**
 * Load all frequency ramp tables.
 */
//-----------------------------------------------------------------------------
void MeasurementManager::LoadRampTables()
{
   #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
      MessageInterface::ShowMessage("Entered MeasurementManager::LoadRampTables() method\n");
      MessageInterface::ShowMessage("  rampTableDataStreamList.size() = %d\n", rampTableDataStreamList.size());
   #endif

   #ifdef USE_DATAFILE_PLUGINS
      DataFileAdapter dfa;
   #endif

   rampTables.clear();

   for (UnsignedInt i = 0; i < rampTableDataStreamList.size(); ++i)
   {
      RampTableData *rtd;
      RampTableData rtd_old;
      rtd_old.epoch = -1.0;
      if (rampTableDataStreamList[i]->GetStringParameter("Format") == "GMAT_RampTable")
      {
      #ifdef USE_DATAFILE_PLUGINS
         if (rampTableDataStreamList[i]->GetIsOpen())
         {
            ObType *obs;

            // This part needs some design information from Matt
            ObType *obd = dfa.GetObTypeObject(rampTableDataStreamList[i]);
            if (!rampTableDataStreamList[i]->GetData(obd))
            {
               throw MeasurementException("Could not load ramp table data\n");
            }
            dfa.LoadRampTable(*obd, *rtd);

            while (!rampTableDataStreamList[i]->IsEOF())
            {
               if (rampTableDataStreamList[i]->GetData(obs))
               {
                  // Store the data for processing
               }
               rampTableDataStreamList[i]->AdvanceToNextOb();
            }
         }
      #else
         if (rampTableDataStreamList[i]->IsOpen())
         {
            if (rampTableDataStreamList[i]->GetStringParameter("Format") == "GMAT_RampTable")
            {
               std::map<std::string, RampTableData> ramp_table_map;
               std::vector<RampTableData> ramp_table;
               rtd = rampTableDataStreamList[i]->ReadRampTableData();
               while (rtd != NULL)
               {
                  // Data records containing rampType = 0(snap), 6(invalid/unknown) , and 7(left bank in DSN file) will be removed from ramp table  
                  //if ((rtd_old.epoch < rtd->epoch)&&(rtd->rampType >= 1)&&(rtd->rampType <= 5))
                  if ((rtd->rampType >= 1)&&(rtd->rampType <= 6))
                  {
                     // Specify keyindex
                     std::stringstream ss;
                     ss.precision(21);
                     for (UnsignedInt p = 0; p < rtd->participantIDs.size(); p++)
                     {
                        ss << rtd->participantIDs[p];
                        ss << " ";          // adding a blank between partcipants
                     }
                     ss << rtd->epoch;
                     rtd->indexkey = ss.str();

                     // Store data record into ramp table map if it is not in there 
                     if (ramp_table_map.find(rtd->indexkey) == ramp_table_map.end())
                        ramp_table_map[rtd->indexkey] = *rtd;

                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                     MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le  indexkey = <%s>\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate, rtd->indexkey.c_str());
                     #endif

                     rtd_old = *rtd;
                  }
                  else
                  {
                     #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                        //MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le: throw away this record due to the order of time or rampType = 0, 6, or 7\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                        MessageInterface::ShowMessage(" epoch: %.15lf   participants: %s   %s   frequency band = %d    ramp type = %d    ramp frequency = %.12le     ramp rate = : %.12le: throw away this record due to rampType = 0, 6, or 7\n", rtd->epoch, rtd->participantIDs[0].c_str(), rtd->participantIDs[1].c_str(), rtd->uplinkBand, rtd->rampType, rtd->rampFrequency, rtd->rampRate);
                     #endif
                  }

                  rtd = rampTableDataStreamList[i]->ReadRampTableData();
               }

               // Store data from ramp table map to ramp_table
               for (std::map<std::string, RampTableData>::iterator q = ramp_table_map.begin(); q != ramp_table_map.end(); ++q)
               {
                  ramp_table.push_back(q->second);
               }

               // store ramp_table to rampTables
               rampTables[rampTableDataStreamList[i]->GetName()] = ramp_table;
               #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
                  MessageInterface::ShowMessage("Ramp Table:\n");
                  for (UnsignedInt j = 0; j < ramp_table.size(); ++j)
                  {
                     RampTableData data = ramp_table[j];
                     MessageInterface::ShowMessage("%.12lf  %s  %s  %d  %d  %le  %le\n", data.epoch, data.participantIDs[0].c_str(), data.participantIDs[1].c_str(), data.uplinkBand, data.rampType, data.rampFrequency, data.rampRate);
                  }
                  MessageInterface::ShowMessage("      ^^^^^^ ramp_table[%s] = <%p>\n", rampTableDataStreamList[i]->GetName().c_str(), &rampTables[rampTableDataStreamList[i]->GetName()]);
               #endif

               // Clean up memory
               ramp_table_map.clear();
               ramp_table.clear();
            }
         }
      #endif
      }
   }

   #ifdef DEBUG_LOAD_FREQUENCY_RAMP_TABLE
      MessageInterface::ShowMessage(
         "Exit MeasurementManager::LoadRampTable() method\n");
   #endif
}


//-----------------------------------------------------------------------------
// GmatTime GetEpoch()
//-----------------------------------------------------------------------------
/**
 * Retrieves the epoch of the current observation
 *
 * @return The (a.1 modified Julian) epoch of the observation.
 */
//-----------------------------------------------------------------------------
GmatTime MeasurementManager::GetEpochGT()
{
   if (obsIndex == observations.size() || obsIndex == -1)
      return 0.0;
   return observations[obsIndex].epochGT;
}


//-----------------------------------------------------------------------------
// GmatTime GetNextEpoch()
//-----------------------------------------------------------------------------
/**
 * Retrieves the epoch for the next available observation
 *
 * @return The (a.1 modified Julian) epoch of the next observation.
 */
//-----------------------------------------------------------------------------
GmatTime MeasurementManager::GetNextEpochGT()
{
   Integer nextIndex;

   if (isForward)
   {
      nextIndex = obsIndex + 1;
      if (nextIndex >= observations.size())
         return 0.0;
   }
   else
   {
      nextIndex = obsIndex - 1;
      if (nextIndex <= 0)
         return 0.0;
   }

#ifdef DEBUG_FLOW
   MessageInterface::ShowMessage("Next epoch: %s\n", observations[nextIndex].epochGT.ToString().c_str());
#endif
   return observations[nextIndex].epochGT;
}


//-----------------------------------------------------------------------------
// const ObservationData *GetObsData(const Integer observationToGet)
//-----------------------------------------------------------------------------
/**
 * Access method for the observation data
 *
 * This method is used to access observations.  If the input parameter,
 * observationToGet, is -1, the current observation is returned.  If it is set
 * to a non-negative number, the observation at that index in the observations
 * vector is returned.  If neither case is valid, a NULL pointer is returned.
 *
 * @param observationToGet Index of the desired observation, or -1 for the
 *                         current observation.
 *
 * @return A pointer to the observation
 */
//-----------------------------------------------------------------------------
const ObservationData *MeasurementManager::GetObsData(
      const Integer observationToGet)
{
   if (observationToGet == -1)
   {
      if (observations.size() == 0)
         return NULL;
      else if ((obsIndex < 0) || (obsIndex >= (Integer)observations.size()))
         return NULL;
      else
         return &(observations[obsIndex]);
   }
   else
   {
      if (observations.size() == 0)
         return NULL;
      else if ((observationToGet < 0) || (observationToGet >= (Integer)observations.size()))
         return NULL;
      else
         return &(observations[observationToGet]);
   }
}


//-----------------------------------------------------------------------------
// ObservationData* GetObsDataObject(const Integer observationToGet)
//-----------------------------------------------------------------------------
/**
 * This method is used to get an observation data object.  If the input parameter,
 * observationToGet, is -1, the current observation data object is returned.  If it is set
 * to a non-negative number, the observation data object at that index in the observations
 * vector is returned.  If neither case is valid, a NULL pointer is returned.
 *
 * @param observationToGet Index of the desired observation data object, or -1 for the
 *                         current observation data object.
 *
 * @return A pointer to the observation data object
 */
//-----------------------------------------------------------------------------
ObservationData* MeasurementManager::GetObsDataObject(const Integer observationToGet)
{
   if (observationToGet == -1)
   {
      return &(observations[obsIndex]);
   }

   if ((observationToGet < 0) ||
       (observationToGet >= (Integer)observations.size()))
      return NULL;

   return &(observations[observationToGet]);
}



//-----------------------------------------------------------------------------
// bool MeasurementManager::AdvanceObservation()
//-----------------------------------------------------------------------------
/**
 * Advances the current observation pointer to the next observation in the
 * observations vector
 *
 * @return   true when it is at the end of observations table otherwise return false
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::AdvanceObservation()
{
   if (isForward)
   {
      if (obsIndex != observations.size())
         ++obsIndex;
   }
   else
   {
      if (obsIndex != -1)
         --obsIndex;
   }


#ifdef DEBUG_ADVANCE_OBSERVATION
   if (obsIndex == observations.size() || obsIndex == -1)
      MessageInterface::ShowMessage("MeasurementManager::AdanceObservation():   current observation is at the end\n");
   else
      MessageInterface::ShowMessage("MeasurementManager::AdanceObservation():   current observation: epoch = %s   measurement type = <%s>   value.size() = %d\n", observations[obsIndex].epochGT.ToString().c_str(), observations[obsIndex].typeName.c_str(), observations[obsIndex].value.size());
#endif

   if (obsIndex == observations.size() || obsIndex == -1)
      return true;
   else
     return false;
}


//-----------------------------------------------------------------------------
// bool MeasurementManager::RemoveObservation(const Integer observationToRemove)
//-----------------------------------------------------------------------------
/**
 * Removes the current or selected observation from the observations vector
 *
 * @param observationToRemove Index of the desired observation data object to remove,
 *                            or -1 for the current observation data object.
 *
 * @return true when it is at the end of observations table otherwise return false
 */
 //-----------------------------------------------------------------------------
bool MeasurementManager::RemoveObservation(const Integer observationToRemove)
{
   Integer removeIndex = observationToRemove;
   if (observationToRemove == -1)
      removeIndex = obsIndex;

   observations.erase(observations.begin() + removeIndex);

   if (obsIndex > removeIndex)
      --obsIndex;

   if (obsIndex == observations.size() || obsIndex == -1)
      return true;
   else
      return false;
}


////------------------------------------------------------------------------------
//// Integer AddMeasurement(MeasurementModel *meas)
////------------------------------------------------------------------------------
///**
// * Sets a new MeasurementModel on the MeasurementManager
// *
// * @param meas The new MeasurementModel
// *
// * @return The ID for the model during this run
// */
////------------------------------------------------------------------------------
//Integer MeasurementManager::AddMeasurement(MeasurementModel *meas)
//{
//   Integer retval = -1;
//   if (meas->IsOfType("TrackingFileSet"))
//   {
//      retval = AddMeasurement((TrackingFileSet*)meas);
//   }
//   else
//   {
//      meas->SetModelID(largestId++);
//      models.push_back(meas);
//      retval = meas->GetModelID();
//
//      #ifdef DEBUG_INITIALIZATION
//         MessageInterface::ShowMessage(
//            "Added measurement of type %s with unique ID %d\n",
//            meas->GetStringParameter("Type").c_str(),
//            meas->GetModelID());
//      #endif
//   }
//
//   return retval;
//}


////------------------------------------------------------------------------------
//// Integer AddMeasurement(MeasurementModel *meas)
////------------------------------------------------------------------------------
///**
// * Sets a TrackingSystem and associated collection of MeasurementModels on the
// * MeasurementManager
// *
// * @param system The new TrackingSystem
// *
// * @return The ID for the first model in this system
// */
////------------------------------------------------------------------------------
//Integer MeasurementManager::AddMeasurement(TrackingSystem *system)
//{
////   Integer firstModel = largestId;
////
////   MeasurementModel *meas = NULL;
////
////   return firstModel;
////   meas->SetModelID(largestId++);
//   systems.push_back(system);
//
//   #ifdef DEBUG_INITIALIZATION
//      MessageInterface::ShowMessage(
//            "Added tracking system named %s of type %s with unique ID %d\n",
//            system->GetName().c_str(), system->GetTypeName().c_str(),
//            -1 /*meas->GetModelID()*/);
//   #endif
//
//   return -1; //meas->GetModelID();
//}


//------------------------------------------------------------------------------
// const StringArray& GetParticipantList()
//------------------------------------------------------------------------------
/**
 * Accesses the complete list of measurement participants, avoiding duplicates.
 *
 * @return The list.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementManager::GetParticipantList()
{
   participants.clear();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager knows about %d "
            "TrackingDataSets\n", trackingSets.size());
      MessageInterface::ShowMessage("MeasurementManager knows about %d "
         "measurement model names\n", modelNames.size());
      for (UnsignedInt i = 0; i < modelNames.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", modelNames[i].c_str());
   #endif

   // Walk through the collection of measurement models...
   for (std::vector<TrackingFileSet*>::iterator i =  trackingSets.begin();
         i !=  trackingSets.end(); ++i)
   {
      /// @todo: This part will need changes
      // Walk through the participant list for the model
      StringArray parts = (*i)->GetParticipants();
      for (UnsignedInt j = 0; j < parts.size(); ++j)
      {
         // If the participant is not in the list yet, add it
         if (find(participants.begin(), participants.end(), parts[j]) ==
               participants.end())
            participants.push_back(parts[j]);
      }
   }

   return participants;
}


//------------------------------------------------------------------------------
// std::vector<StringArray> GetSignalPathList()
//------------------------------------------------------------------------------
/**
 * Accesses the complete list of all signal paths.
 *
 * @return The list.
 */
//------------------------------------------------------------------------------
std::vector<StringArray> MeasurementManager::GetSignalPathList()
{
   std::vector<StringArray> signalPathList;
   signalPathList.clear();

   // Walk through the collection of measurement models...
   for (std::vector<TrackingDataAdapter*>::iterator i =  adapters.begin();
         i !=  adapters.end(); ++i)
   {
      StringArray parts = (*i)->GetParticipants(0);
      signalPathList.push_back(parts);
   }

   return signalPathList;
}


//-----------------------------------------------------------------------------
// void AddMeasurementName(std::string measName)
//-----------------------------------------------------------------------------
/**
 * Adds a named measurement model to the list of managed models.
 *
 * This method is idempotent: the same model can be added multiple times, and
 * only one copy of the name will appear in the list.
 *
 * @param measName The name of the measurement model
 */
//-----------------------------------------------------------------------------
void MeasurementManager::AddMeasurementName(std::string measName)
{
   if (find(modelNames.begin(), modelNames.end(), measName) == modelNames.end())
      modelNames.push_back(measName);
}

//-----------------------------------------------------------------------------
// const StringArray& GetMeasurementNames() const
//-----------------------------------------------------------------------------
/**
 * Retrieves the list of measurement models registered with this
 * MeasurementManager
 *
 * @return The list of models
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetMeasurementNames() const
{
   return modelNames;
}


//-----------------------------------------------------------------------------
// const std::vector<TrackingDataAdapter*>& GetAllTrackingDataAdapters()
//-----------------------------------------------------------------------------
/**
* Get a list of all TrackingAdapter objects
*/
//-----------------------------------------------------------------------------
const std::vector<TrackingDataAdapter*>& MeasurementManager::GetAllTrackingDataAdapters()
{
   return adapters;
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the measurement manager
*
* The transient force vector is a set of models used in GMAT's ODEModel for
* affects that are turned on and off over the course of a mission.  An example
* of a transient force is a finite burn, which is toggled by the
* BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
* by commands that need them.  Typical usage is found in the propagation
* enabled commands.
*
* @param tf The vector of transient forces
*/
//------------------------------------------------------------------------------
void MeasurementManager::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    transientForces = tf;
}


//-----------------------------------------------------------------------------
// const Integer GetMeasurementId(const std::string &modelName) const
//-----------------------------------------------------------------------------
/**
 * Retrieves the model ID (or IDs) for a named measurement model or a named tracking file set
 *
 * @param modelName The name of the measurement model or the tracking file set
 *
 * @return The ID (or IDs) for model or tracking file set
 */
//-----------------------------------------------------------------------------
const IntegerArray MeasurementManager::GetMeasurementId(
      const std::string &modelName) const
{
   IntegerArray arrayId;
   Integer foundId = -1;

   // Check modelName to be a name of trackingSet 
   UnsignedInt i;
   for (i = 0; i < trackingSets.size(); ++i)
   {
      if (trackingSets[i]->GetName() == modelName)
         break;
   }

   if (i < trackingSets.size())
   {
      // Get a name list of TrackingDataAdapter object associated the TrackingFileSet specified by modelName
      StringArray tfsAdapterList;
      if (adapterFromTFSMap.find(trackingSets[i]) != adapterFromTFSMap.end())
         tfsAdapterList = adapterFromTFSMap.find(trackingSets[i])->second;

      for (UnsignedInt k = 0; k < tfsAdapterList.size(); ++k)
      {
         // Find an adapter having the same name as tfsAdapterList[i]
         foundId = -1;
         for (UnsignedInt j = 0; j < adapters.size(); ++j)
         {
            if (adapters[j]->GetName() == tfsAdapterList[k])
            {
               foundId = adapters[j]->GetModelID();
               break;
            }
         }

         // Add founId to ID
         if (foundId != -1)
            arrayId.push_back(foundId);
      }
   }
   else
      arrayId.push_back(-1);

   return arrayId;

   /// @todo: Handle multiple measurements inside of a TrackingFileSet

   //for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
   //{
   //   if (trackingSets[i]->GetName() == modelName)
   //   {
   //      if (adapterFromTFSMap.find(trackingSets[i]) != adapterFromTFSMap.end())
   //      {
   //         /// @todo: Make measurementID accessor work for > 1 adapter in a TFS
   //         StringArray tfsAdapterList =
   //               adapterFromTFSMap.find(trackingSets[i])->second;

   //         std::string modelAdapterName = (tfsAdapterList.size() > 0 ?
   //               tfsAdapterList[0] : "Range");

   //         for (UnsignedInt j = 0; j < adapters.size(); ++j)
   //            if (adapters[j]->GetName() == modelAdapterName)
   //               foundId = adapters[j]->GetModelID();
   //      }
   //   }
   //}

   //return foundId;
}


//-----------------------------------------------------------------------------
// GmatBase* GetClone(GmatBase *obj)
//-----------------------------------------------------------------------------
/**
 * Finds a local clone of an object if one is available
 *
 * @param obj The object whose clone is needed
 *
 * @return A pointer to the clone, if one is available
 */
//-----------------------------------------------------------------------------
GmatBase* MeasurementManager::GetClone(GmatBase *obj)
{
   GmatBase *retval = NULL;
   // Look for an object named same as obj
   std::string objname = obj->GetName();

   if (objname != "")
   {
      // Check the tracking file sets
      for (UnsignedInt i = 0; i < trackingSets.size(); ++i)
         if (trackingSets[i]->GetName() == objname)
         {
            retval = trackingSets[i];
            break;
         }
   }

   return retval;
}


//--------------------------------------------------------------------------------------
// bool ValidateDouplicationOfGroundStationID()
//--------------------------------------------------------------------------------------
/**
* This function is used to verify 2 different ground stations having the same Id
*
* @return      true if each ground station has a unique ID, false otherwise 
*/
//--------------------------------------------------------------------------------------
bool MeasurementManager::ValidateDuplicationOfGroundStationID(std::string& errorMsg)
{
   std::map<std::string, std::string> gsNameIdMap;
   
   for (Integer i = 0; i < adapters.size(); ++i)
   {
      std::vector<ObjectArray*> participantObjLists = adapters[i]->GetMeasurementModel()->GetParticipantObjectLists();
      ObjectArray* oa = participantObjLists[0];

      for (Integer j = 0; j < (*oa).size(); ++j)
      {
         if ((*oa)[j]->IsOfType(Gmat::GROUND_STATION))
         {
            if (gsNameIdMap.size() == 0)
               gsNameIdMap[(*oa)[j]->GetName()] = (*oa)[j]->GetStringParameter("Id");
            else
            {
               for (std::map<std::string, std::string>::iterator objPtr = gsNameIdMap.begin(); objPtr != gsNameIdMap.end(); ++objPtr)
               {
                  if (objPtr->first == (*oa)[j]->GetName())
                     continue;
                  else
                  {
                     if (objPtr->second != (*oa)[j]->GetStringParameter("Id"))
                     {
                        // Add to gsNameIdMap
                        gsNameIdMap[(*oa)[j]->GetName()] = (*oa)[j]->GetStringParameter("Id");
                        break;
                     }
                     else
                     {
                        errorMsg = "Both ground stations '" + objPtr->first + "' and '" + (*oa)[j]->GetName() + "' have the same Id '" + objPtr->second + "'";
                        return false;
                     }
                  }
               }
            }
         }
      }
   }

   gsNameIdMap.clear();

   return true;
}


//------------------------------------------------------------------------------
// bool CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)   
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel
 *
 * @param  forSimulation   true for simulation calculation and false for 
 *                          estimation calculation. Default value is false
 * @param  withEvents       flag to indicate calculation with ro without events
 * @param  addNoise         flag to indicate noise added to measurement
 *
 * @return True if at least one measurement is feasible and calculated; false
 *         otherwise
 */
//------------------------------------------------------------------------------
bool MeasurementManager::CalculateMeasurements(bool forSimulation, bool withEvents, bool addNoise)
{
   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(" Entered bool MeasurementManager::CalculateMeasurements(simulation = %s, withEvents = %s, addNoise = %s)\n", (forSimulation?"true":"false"), (withEvents?"true":"false"), (addNoise?"true":"false"));
   #endif
   
   // Specify observation data for measurement
   ObservationData* od = NULL;
   if (!observations.empty())
      od = &(observations[obsIndex]);

   bool retval = false;
   eventCount = 0;
   
   if (forSimulation)
   {  // This section is used for simulation only:
      #ifdef DEBUG_CALCULATE_MEASUREMENTS
      MessageInterface::ShowMessage("adapters size = %d\n", adapters.size());
      #endif
      for (UnsignedInt i = 0; i < adapters.size(); ++i)
      {
         // Specify ramp table associated with measurement adapters[i]:
         // Note: Only one ramp table is used for a measurement model
         std::vector<RampTableData>* rt = GetRampTableForAdapter(*adapters[i]);

         // Set AddNoise to measuement apdater
         adapters[i]->SetBooleanParameter("AddNoise", addNoise);

         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            if (withEvents)
               MessageInterface::ShowMessage(" Simulation: measurement adapter %s with events\n", adapters[i]->GetName().c_str());
            else
               MessageInterface::ShowMessage(" Simulation: measurement adapter %s without events\n", adapters[i]->GetName().c_str());
         #endif
         measurements[i] = adapters[i]->CalculateMeasurement(withEvents, od, rt, forSimulation);
         if (measurements[i].unfeasibleReason == "R")
            throw MeasurementException(adapters[i]->GetErrorMessage());

         if (measurements[i].isFeasible)
         {
            if (!withEvents)
               eventCount += measurements[i].eventCount;
            retval = true;
         }
         
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage(" Adapter Measurement is %s."
               , (measurements[i].isFeasible?"feasible":" not feasible"));
            MessageInterface::ShowMessage(" Its feasible value = %lf  reason = <%s>.   ", measurements[i].feasibilityValue, measurements[i].unfeasibleReason.c_str());
            MessageInterface::ShowMessage(" Its value is [");
            for (Integer k = 0; k < measurements[i].value.size(); ++k)
               MessageInterface::ShowMessage("%lf  ", measurements[i].value[k]);
            MessageInterface::ShowMessage("]km\n");
         #endif
      } // for i loop
   }
   else
   {  // This section is used for estimation only:
      if (od == NULL)
      {
         #ifdef DEBUG_CALCULATE_MEASUREMENTS
            MessageInterface::ShowMessage("    observation data is NULL\n");
         #endif
         return retval;               // no calculation measurement value whenever no observation data is available
      }

      #ifdef DEBUG_CALCULATE_MEASUREMENTS
         MessageInterface::ShowMessage("    %s observation data: %.12lf  %s  %d  %s  %s   %.12lf\n", od->dataFormat.c_str(), od->epoch, od->typeName.c_str(), od->type, od->participantIDs[0].c_str(), od->participantIDs[1].c_str(),od->value[0]);
         MessageInterface::ShowMessage(" processing for tracking data adapters ...\n");
         MessageInterface::ShowMessage("adapters.size() = %d:\n", adapters.size());
      #endif

      // Now do the tracking data adapters. Obsevation data od belongs to adapters[j] 
      // when their measurement type and signal path are the same. The measurement[j] associated
      // to adapter has valid value when they are belong to each other. Otherwise, measurement[j]
      // will have feasibale to be false.
      for (UnsignedInt j = 0; j < adapters.size(); ++j)
      {
         // Code to verify observation data belonging to the measurement model jth
         bool isbelong = false;
         if ((adapters[j]->GetStringParameter("MeasurementType") == od->typeName))
         {
            std::vector<ObjectArray*> participantObjLists = adapters[j]->GetMeasurementModel()->GetParticipantObjectLists();
            UnsignedInt num = participantObjLists[0]->size();                                       // participants in measurement model
            if (num == od->participantIDs.size())
            {
               isbelong = true;
               for (UnsignedInt i1 = 0; i1 < num; ++i1)
               {
                  // when observation data's signal path and measurement model's signal
                  // path are different, they do not belong each other
                  if (od->participantIDs[i1] != participantObjLists[0]->at(i1)->GetStringParameter("Id"))
                  {
                     isbelong = false;
                     break;
                  }
               }
            }
         }

         if (isbelong == false)
         {
            measurements[j].typeName         = adapters[j]->GetStringParameter("MeasurementType");
            measurements[j].epochGT          = od->epochGT;
            measurements[j].epoch            = od->epoch;
            measurements[j].epochSystem      = od->epochSystem;
            measurements[j].isFeasible       = false;
            measurements[j].covariance       = NULL;
            measurements[j].eventCount       = 0;
            measurements[j].feasibilityValue = 0.0;
            measurements[j].unfeasibleReason = "U";
            measurements[j].value.clear();  
         }
         else
         {
///// TBD: Do we want something more generic here?
            // Specify ramp table associated with tracking data adapter adapters[j]:
            // Note: Only one ramp table is used for a tracking data adapter
            std::vector<RampTableData>* rt = GetRampTableForAdapter(*adapters[j]);

            //MessageInterface::LogMessage(" calculating %s \n", adapters[j]->GetName().c_str());
            measurements[j] = adapters[j]->CalculateMeasurement(withEvents, od, rt, forSimulation);
            
            if (measurements[j].isFeasible)
            {
               if (!withEvents)
                  eventCount += measurements[j].eventCount;
               retval = true;
            }

            #ifdef DEBUG_ADAPTERS
               MessageInterface::ShowMessage("   Measurement %d computed; first "
                  "value: %lf\n", j, measurements[j].value[0]);
            #endif
         }
      }// for j loop
   }

   #ifdef DEBUG_FLOW
      MessageInterface::ShowMessage(" Returning %s from bool MeasurementManager"
            "::CalculateMeasurements(%s,%s)\n", (retval ? "true" : "false"),
            (forSimulation?"true":"false"), (withEvents?"true":"false"));
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// bool MeasurementHasEvents()
//-----------------------------------------------------------------------------
/**
 * Check to see if any registered measurements have events
 *
 * @return true if any of the measurements have events
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::MeasurementHasEvents()
{
   bool retval = false;

   for (UnsignedInt i = 0; i < measurements.size(); ++i)
      if (measurements[i].eventCount > 0)
         retval = true;

   return retval;
}


//-----------------------------------------------------------------------------
// ObjectArray& GetActiveEvents()
//-----------------------------------------------------------------------------
/**
 * Retrieves a vector of all events associated with currently feasible
 * measurements in the MeasurementManager
 *
 * @return The vector of events for the feasible measurements
 */
//-----------------------------------------------------------------------------
ObjectArray& MeasurementManager::GetActiveEvents()
{
#ifdef DEBUG_GET_ACTIVE_EVENTS
   MessageInterface::ShowMessage("MeasurementManager::GetActiveEvents()   entered: \n");
#endif

   activeEvents.clear();

#ifdef DEBUG_GET_ACTIVE_EVENTS
   MessageInterface::ShowMessage("MeasurementManager::GetActiveEvents()   exit: \n");
#endif

   return activeEvents;
}


//-----------------------------------------------------------------------------
// bool ProcessEvent(Event *locatedEvent)
//-----------------------------------------------------------------------------
/**
 * Passes an event to the owning measurement so that measurement can process the
 * result from that event.
 *
 * This method is called after the RootFinder has located an event, so that the
 * measurement that depends on that event can retrieve the result of the event
 * location process for use in the measurement evaluation process.
 *
 * @param locatedEvent Pointer to the event that is to be processed
 *
 * @return true if the event was passed to a measurement model, false if not
 */
//-----------------------------------------------------------------------------
bool MeasurementManager::ProcessEvent(Event *locatedEvent)
{
   bool retval = false;

   return retval;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& MeasurementManager::CalculateDerivatives(
//                         GmatBase *obj, Integer wrt, Integer forMeasurement)
//------------------------------------------------------------------------------
/**
 * Fires the calculation method of each owned MeasurementModel, and on success
 * (i.e. if the measurement was feasible) then fires the derivative calculator.
 *
 * @param obj The object holding the with-respect-to parameters
 * @param wrt The with-respect-to parameters
 * @param forMeasurement Indix of the measurement that is being differentiated
 *
 * @return The derivative data
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& MeasurementManager::CalculateDerivatives(
                           GmatBase *obj, Integer wrt, Integer forMeasurement)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered MeasurementManager::"
            "CalculateDerivatives(%s <%p>, %d, %d)\n", obj->GetName().c_str(),
            obj, wrt, forMeasurement);
   #endif

   return adapters[forMeasurement]->CalculateMeasurementDerivatives(obj,wrt);
}


//------------------------------------------------------------------------------
// bool MeasurementManager::WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Writes the calculated data to a file.
 *
 * This method is used during measurement simulation to generate simulated data.
 *
 * @return  true when at least one measurement successed for writing, 
 *          false when all measurement are unfeasible
 */
//------------------------------------------------------------------------------
bool MeasurementManager::WriteMeasurements()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("Entered MeasurementManager::"
            "WriteMeasurements()\n");
   #endif

   bool retval = false;

   for (UnsignedInt i = 0; i < measurements.size(); ++i)
   {
      if (measurements[i].isFeasible)
      {
         #ifdef DEBUG_FILE_WRITE
            MessageInterface::ShowMessage("Feasible\n   id = %d\n",
                  measurements[i].uniqueID);
            MessageInterface::ShowMessage("   Stream at <%p>\n",
                  idToStreamMap[measurements[i].uniqueID]);
         #endif
         if (idToStreamMap[measurements[i].uniqueID])
         {
            idToStreamMap[measurements[i].uniqueID]->
               WriteMeasurement(&(measurements[i]));
         }
         else
            throw MeasurementException("Error: No data file is defined in TrackingFileSet or MeasurementModel\n");

         retval = true;
      }
   }

   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("Exit MeasurementManager::"
            "WriteMeasurements()\n");
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// const StringArray& GetStreamList()
//-----------------------------------------------------------------------------
/**
 * Retrieves a list of the observation data streams available to this
 * MeasurementManager.
 *
 * The current implementation queries the measurement models for the list of
 * streams in those models.  That dependency may change with later builds, based
 * on changes in the scripting to the location of the stream identifiers.
 *
 * @return The list of streams
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetStreamList()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetStreamList() "
            "Entered\n   %d adapters registered\n", adapters.size());
   #endif

   // Run through the measurements and build the list
   streamNames.clear();
   for (UnsignedInt i = 0; i < adapters.size(); ++i)
   {
      StringArray names = adapters[i]->GetStringArrayParameter("ObservationData");
      for (UnsignedInt j = 0; j < names.size(); ++j)
      {
         if(find(streamNames.begin(), streamNames.end(), names[j]) ==
               streamNames.end())
            streamNames.push_back(names[j]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetStreamList() Exit \n");
   #endif

   return streamNames;
}


///// TBD: Do we want something more generic here?
//-----------------------------------------------------------------------------
// const StringArray& GetRampTableDataStreamList()
//-----------------------------------------------------------------------------
/**
 * Retrieves a list of the ramp table data streams available to this
 * MeasurementManager.
 *
 * The current implementation queries the measurement models for the list of
 * streams in those models.  That dependency may change with later builds, based
 * on changes in the scripting to the location of the stream identifiers.
 *
 * @return The list of ramp table data streams
 */
//-----------------------------------------------------------------------------
const StringArray& MeasurementManager::GetRampTableDataStreamList()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetRampTableDataStreamList() "
            "Entered\n   %d adapters registered\n", adapters.size());
   #endif

   // Run through the measurements and build the list
   rampTableDataStreamNames.clear();
   for (UnsignedInt i = 0; i < adapters.size(); ++i)
   {
      StringArray names = adapters[i]->GetStringArrayParameter("RampTables");
      for (UnsignedInt j = 0; j < names.size(); ++j)
      {
         if(find(rampTableDataStreamNames.begin(), rampTableDataStreamNames.end(), names[j]) ==
               rampTableDataStreamNames.end())
            rampTableDataStreamNames.push_back(names[j]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("MeasurementManager::GetRampTableDataStreamList() Exit\n");
   #endif

   return rampTableDataStreamNames;
}


//-----------------------------------------------------------------------------
// void SetStreamObject(DataFile *newStream)
//-----------------------------------------------------------------------------
/**
 * Passes an observation stream into the MeasurementManager.
 *
 * This method passes an observation stream to the MeasurementManager.  The
 * MeasurementManager, in turn, passes the stream pointer to each
 * MeasurementModel that uses it.
 *
 * This method is idempotent.  Multiple calls with the same observation stream
 * have the same result as passing in the stream once.
 *
 * @param newStream The stream that is being set
 */
//-----------------------------------------------------------------------------
void MeasurementManager::SetStreamObject(DataFile *newStream)
{
   if (find(streamList.begin(), streamList.end(), newStream) ==
         streamList.end())
   {
      streamList.push_back(newStream);
      std::string streamName = newStream->GetName();
   }
}


///// TBD: Do we want something more generic here?
//-----------------------------------------------------------------------------
// void SetRampTableDataStreamObject(DataFile *newStream)
//-----------------------------------------------------------------------------
/**
 * Passes a ramp table data stream into the MeasurementManager.
 *
 * This method passes a ramp table data stream to the MeasurementManager.  The
 * MeasurementManager, in turn, passes the stream pointer to each
 * MeasurementModel that uses it.
 *
 * This method is idempotent.  Multiple calls with the same ramp table data stream
 * have the same result as passing in the stream once.
 *
 * @param newStream The stream that is being set
 */
//-----------------------------------------------------------------------------
void MeasurementManager::SetRampTableDataStreamObject(DataFile *newStream)
{
   if (find(rampTableDataStreamList.begin(), rampTableDataStreamList.end(), newStream) ==
         rampTableDataStreamList.end())
   {
      rampTableDataStreamList.push_back(newStream);
      std::string streamName = newStream->GetName();
   }
}


//-----------------------------------------------------------------------------
// IntegerArray& GetValidMeasurementList()
//-----------------------------------------------------------------------------
/**
 * Retrieves the indices of the current active measurement models
 *
 * @return A vector of indices for the active measurements
 */
//-----------------------------------------------------------------------------
IntegerArray& MeasurementManager::GetValidMeasurementList()
{
   activeMeasurements.clear();

   FindModelForObservation();

   #ifdef DEBUG_MODEL_MAPPING
      MessageInterface::ShowMessage("Found %d potential models for the current "
            "observation\n", activeMeasurements.size());
   #endif

   return activeMeasurements;
}


//-----------------------------------------------------------------------------
// Integer FindModelForObservation()
//-----------------------------------------------------------------------------
/**
 * Finds the MeasurementModel associated with the current observation
 * Does so by matching both type and path of the observation with the measurement
 *
 * @return The index for the measurement model
 */
//-----------------------------------------------------------------------------
Integer MeasurementManager::FindModelForObservation()
{
   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("Entered MeasurementManager::"
             "FindModelForObservation()\n");
   #endif
   Integer retval = 0;

   Integer type = observations[obsIndex].type;

   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("   Current observation type: %d\n", type);
   #endif

   for (UnsignedInt i = 0; i < adapters.size(); ++i)
   {
      MeasurementData theMeas = adapters[i]->GetMeasurement();

      #ifdef DEBUG_MODEL_MAPPING
            MessageInterface::ShowMessage("   Current model type: %d\n",
                  theMeas.type);
      #endif

      if (theMeas.type == type)
      {
         #ifdef DEBUG_MODEL_MAPPING
               MessageInterface::ShowMessage("   Found a model of this type; "
                     "checking participants\n");
         #endif
         StringArray parts = observations[obsIndex].participantIDs;
         StringArray measParts = theMeas.participantIDs;
         bool missingParticipant = false;
         if (measParts.size() != parts.size())
            missingParticipant = true;
         else
         {
            for (UnsignedInt j = 0; j < parts.size(); ++j)
            {
               if (parts[j] != measParts[j])
               {
                  missingParticipant = true;
                  break;
               }
            }
         }

         //Old way of checking, just checked if all participants were present, not the order. 
         //for (UnsignedInt j = 0; j < parts.size(); ++j)
            //if (find(measParts.begin(), measParts.end(), parts[j]) ==
               //measParts.end())
               //missingParticipant = true;

         if (!missingParticipant)
         {
            activeMeasurements.push_back(i);
            ++retval;
         }
      }
   }

   #ifdef DEBUG_MODEL_MAPPING
       MessageInterface::ShowMessage("Exit MeasurementManager::"
             "FindModelForObservation()\n");
   #endif

   return retval;
}


//-----------------------------------------------------------------------------
// void Reset()
//-----------------------------------------------------------------------------
/**
 * Resets the data pointers in the MeasurementManager.
 *
 * This method is used to set the MeasurementManager to the start of the
 * observation data so that an Estimator can start processing with the first
 * observation available to the MeasurementManager.
 */
//-----------------------------------------------------------------------------
void  MeasurementManager::Reset()
{
   if (isForward)
      obsIndex = 0;
   else
      obsIndex = observations.size() - 1;
}


//-----------------------------------------------------------------------------
// void SetDirection(bool forwards)
//-----------------------------------------------------------------------------
/**
 * Set the direction to advance through the observations in MeasurementManager.
 *
 * This setting affects which direction AdvanceObservation() will travel through
 * the vector of observations.
 *
 * @param forwards The direction to set. True means to advance forwads through
 *                 the vector of observations, false means to advance in reverse
 */
 //-----------------------------------------------------------------------------
void MeasurementManager::SetDirection(bool forwards)
{
   isForward = forwards;
}


//-----------------------------------------------------------------------------
// bool IsForward()
//-----------------------------------------------------------------------------
/**
 * Gets the direction to advance through the observations in MeasurementManager.
 *
 * @return True if the direction is forwards, false if backwards
 */
 //-----------------------------------------------------------------------------
bool MeasurementManager::IsForward()
{
   return isForward;
}


//------------------------------------------------------------------------------
// Integer AddMeasurement(TrackingFileSet* tfs)
//------------------------------------------------------------------------------
/**
 * Adds a tracking file set to the manager
 *
 * @param tfs The TrackingFileSet object that is managed
 *
 * @return TBD
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(TrackingFileSet* tfs)
{
   Integer retval = -1;

   if (find(trackingSets.begin(),trackingSets.end(), tfs) == trackingSets.end())
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Adding %s TrackingFileSet %s <%p>\n",
               (tfs->IsInitialized() ? "initialized" : "not initialized"),
               tfs->GetName().c_str(), tfs);
      #endif
      trackingSets.push_back(tfs);
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer AddMeasurement(TrackingDataAdapter* adapter)
//------------------------------------------------------------------------------
/**
 * Adds a TrackingDataAdapter to the measurement manager
 *
 * @param adapter The adapter
 *
 * @return TBD
 */
//------------------------------------------------------------------------------
Integer MeasurementManager::AddMeasurement(TrackingDataAdapter* adapter)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("   Adding %s TrackingDataAdapter %s "
            "<%p>\n", (adapter->IsInitialized() ? "initialized" :
            "not initialized"), adapter->GetName().c_str(), adapter);
   #endif
   adapter->SetModelID(largestId++);
   if (thePropagators)
      adapter->SetPropagators(thePropagators, satPropagatorMap);
   adapters.push_back(adapter);
   return -1;
}


////------------------------------------------------------------------------------
//// const std::vector<MeasurementModel*>& GetAllMeasurementModels()
////------------------------------------------------------------------------------
///**
//* Get a list of all MeasurementModel objects
//*/
////------------------------------------------------------------------------------
//const std::vector<MeasurementModel*>& MeasurementManager::GetAllMeasurementModels()
//{
//   return models;
//}


////------------------------------------------------------------------------------
//// const std::vector<TrackingSystem*>& GetAllTrackingSystems()
////------------------------------------------------------------------------------
///**
//* Get a list of all TrackingSystem objects
//*/
////------------------------------------------------------------------------------
//const std::vector<TrackingSystem*>& MeasurementManager::GetAllTrackingSystems()
//{
//   return systems;
//}


//------------------------------------------------------------------------------
// const std::vector<TrackingFileSet*>& GetAllTrackingFileSets()
//------------------------------------------------------------------------------
/**
* Get a list of all TrackingFileSet objects
*/
//------------------------------------------------------------------------------
const std::vector<TrackingFileSet*>& MeasurementManager::GetAllTrackingFileSets()
{
   return trackingSets;
}


//------------------------------------------------------------------------------
// UnsignedInt GetMeasurementSize()
//------------------------------------------------------------------------------
/**
* Get the size of the measurement vector
*/
//------------------------------------------------------------------------------
UnsignedInt MeasurementManager::GetMeasurementSize()
{
   return observations.size();
}


//------------------------------------------------------------------------------
// UnsignedInt GetCurrentRecordNumber()
//------------------------------------------------------------------------------
/**
* Get the current observation record number
*/
//------------------------------------------------------------------------------
UnsignedInt MeasurementManager::GetCurrentRecordNumber()
{
   return obsIndex;
}

//------------------------------------------------------------------------------
// GetRampTableForAdapter(TrackingDataAdapter& adapter)
//------------------------------------------------------------------------------
/**
 * @param adapter The adapter to get the ramp table from
 */
 //------------------------------------------------------------------------------
std::vector<RampTableData>* MeasurementManager::GetRampTableForAdapter(TrackingDataAdapter& adapter)
{
   StringArray sr = adapter.GetStringArrayParameter("RampTables");
   std::string measType = adapter.GetStringParameter("MeasurementType");

   std::vector<RampTableData>* rt = NULL;
   if( (measType == "DSN_SeqRange" || measType == "DSN_TCP" || measType == "DSN_PNRange") && sr.size() > 0 )
      rt = &(rampTables[sr[0]]);

   return rt;
}

//------------------------------------------------------------------------------
// ClearIonosphereCache()
//------------------------------------------------------------------------------
/**
 * Clears the ionosphere cache
 */
 //------------------------------------------------------------------------------
void MeasurementManager::ClearIonosphereCache()
{
   // Make sure previous cache is cleared
   for (auto it = trackingSets.begin(); it != trackingSets.end(); ++it) {
      (*it)->ClearIonosphereCache();
   }
}
