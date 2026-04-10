//------------------------------------------------------------------------------
//                           TrackingFileSet
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
// Created: Mar 11, 2014
/**
 * The class of objects that define GMAT measurement models and tracking data
 */
//------------------------------------------------------------------------------

#include "TrackingFileSet.hpp"
#include "TFSMagicNumbers.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <sstream>                  // For magic number mapping code
/// Temporarily here; needs to move into a factory
#include "GNRangeAdapter.hpp"
#include "DSNRangeAdapter.hpp"
#include "DSNPNRangeAdapter.hpp"
#include "DopplerAdapter.hpp"
#include "GNDopplerAdapter.hpp"
#include "TDRSRangeAdapter.hpp"
#include "TDRSDopplerAdapter.hpp"
#include "TDRS3LReturnDopplerAdapter.hpp"
#include "TDRSDOWDAdapter.hpp"
#include "BRTSRangeAdapter.hpp"
#include "BRTSDopplerAdapter.hpp"
//#include "RangeRateAdapterKps.hpp"
#include "PointRangeRateAdapterKps.hpp"
#include "GPSAdapter.hpp"
#include "GPSPointMeasureModel.hpp"
#include "AzimuthAdapter.hpp"
#include "ElevationAdapter.hpp"
#include "XEastAdapter.hpp"
#include "YNorthAdapter.hpp"
#include "XSouthAdapter.hpp"
#include "YEastAdapter.hpp"
#include "RightAscAdapter.hpp"
#include "DeclinationAdapter.hpp"
#include "RangeSkinAdapter.hpp"
#include "PropSetup.hpp"
#include "PhysicalSignal.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_INITIALIZE
//#define DEBUG_GET_PARAMETER
//#define DEBUG_BUILD_ADAPTER

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

/// Strings describing the BatchEstimator parameters
const std::string TrackingFileSet::PARAMETER_TEXT[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =
{
   "AddTrackingConfig",             // TRACKINGCONFIG
   "FileName",                      // FILENAME
   "RampTable",                     // RAMPED_TABLE
   "UseLightTime",                  // USELIGHTTIME
   "UseRelativityCorrection",       // USE_RELATIVITY
   "UseETminusTAI",                 // USE_ETMINUSTAI
   "AberrationCorrection",          // ABERRATION_CORRECTION
   "SimRangeModuloConstant",        // RANGE_MODULO
   "SimDopplerCountInterval",       // DOPPLER_COUNT_INTERVAL
   "SimTDRSServiceAccessList",      // TDRS_SERVICE_ACCESS
   "SimTDRSSmarId",                 // TDSR_SMAR_ID
   "SimTDRSTrackerType",            // TDRS_TRACKER_TYPE

   "TimeGapForPassBreak",           // TIMEGAP_FOR_PASSBREAK

   "MinHeightOfRayPath",            // HORP_HEIGHT
   "MaxCentralAngleOfRayPath",      // HORP_CENTRAL_ANGLE
   "DataFilters",                   // DATA_FILTERS
   // Additions to allow exposure to the API via a GmatBase pointer
   "ApiGetConfigCount",             // API_TRACKINGCONFIGCOUNT,
   "ApiGetCalculated",              // API_GET_C_VALUE,
};

/// Types of the BatchEstimator parameters
const Gmat::ParameterType TrackingFileSet::PARAMETER_TYPE[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =
{
   Gmat::STRINGARRAY_TYPE,          // TRACKINGCONFIG
   Gmat::STRINGARRAY_TYPE,          // FILENAME, but it's a list of names...
   Gmat::STRINGARRAY_TYPE,          // RAMPED_TABLE, but it's a list of names...
   Gmat::BOOLEAN_TYPE,              // USELIGHTTIME
   Gmat::BOOLEAN_TYPE,              // USE_RELATIVITY
   Gmat::BOOLEAN_TYPE,              // USE_ETMINUSTAI
   Gmat::STRING_TYPE,               // ABERRATION_CORRECTION
   Gmat::REAL_TYPE,                 // RANGE_MODULO
   Gmat::REAL_TYPE,                 // DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,          // TDRS_SERVICE_ACCESS
   Gmat::INTEGER_TYPE,              // TDRS_SMAR_ID
   Gmat::INTEGER_TYPE,              // TDRS_DATA_FLAG

   Gmat::REAL_TYPE,                 // TIMEGAP_FOR_PASSBREAK   // Time gap for pass break is used for pass biases

   Gmat::REAL_TYPE,                 // HORP_HEIGHT
   Gmat::REAL_TYPE,                 // HORP_CENTRAL_ANGLE
   Gmat::OBJECTARRAY_TYPE,          // DATA_FILLTERS
   // Additions to allow exposure to the API via a GmatBase pointer
   Gmat::INTEGER_TYPE,              // API_TRACKINGCONFIGCOUNT,
   Gmat::REAL_TYPE                 // API_GET_C_VALUE,
};


//------------------------------------------------------------------------------
// TrackingFileSet()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
TrackingFileSet::TrackingFileSet(const std::string &name) :
   MeasurementModelBase          (name, "TrackingFileSet"),
   useLighttime                  (true),
   solarsystem                   (NULL),
   thePropagators                (NULL),
   satPropagatorMap              (NULL),
   useRelativityCorrection       (false),
   useETminusTAICorrection       (false),
   aberrationCorrection          ("None"),
   rangeModulo                   (1.0e18),
   dopplerCountInterval          (1.0),
   tdrsServiceAccessList         ({}),
   tdrsSMARID                    (1),
   tdrsDataFlag                  (0),
   tdrsMeasurementFrequencies    ({}),
   tdrsMeasurementFrequencyBands ({}),

   timeGapForPassBreak         (1.0e70),                 // unit: second

   useHorp                   (true),                // Sat to Sat always uses HORP with this set true
   forceHorp                 (false),
   horpOblate                (false),
   horpHeight                (500.0),               // Default to 500 km above the body's surface
   horpAngle                 (70.0)                 // 70 deg
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> default construction \n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::MEASUREMENT_MODEL);
   objectTypes.push_back(GmatType::GetTypeId("TrackingFileSet"));
   objectTypeNames.push_back("TrackingFileSet");
   parameterCount = TrackingFileSetParamCount;
}


//------------------------------------------------------------------------------
// ~TrackingFileSet()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::~TrackingFileSet()
{
   filenames.clear();
   rampedTablenames.clear();
   tdrsServiceAccessList.clear();
   dataFilterNames.clear();
   mesg.clear();

   // clean up std::vector<MeasurementDefinition> trackingConfigs;
   for (Integer i = 0; i < trackingConfigs.size(); ++i)
   {
      for (Integer j = 0; j < trackingConfigs[i].strands.size(); ++j)
         trackingConfigs[i].strands[j].clear();
      trackingConfigs[i].strands.clear();

      for (Integer j = 0; j < trackingConfigs[i].sensors.size(); ++j)
         trackingConfigs[i].sensors[j].clear();
      trackingConfigs[i].sensors.clear();

      trackingConfigs[i].types.clear();

   }
   trackingConfigs.clear();

   // clean up std::vector<TrackingDataAdapter*> measurements;
   // the delete of TrackingDataADapter objects in measurements is handled by the delete of createdObjects
   //for (Integer i = 0; i < measurements.size(); ++i)
   //{
   //   if (measurements[i])
   //      delete measurements[i];            // It deletes TrackingDataAdapter objects for this TrackingFileSet
   //}
   measurements.clear();
   
   // clean up ObjectArray dataFilters;
   // the delete of DataFilter objects is handled by the delete of createdObjects
   dataFilters.clear();
   
   tdrsMeasurementFrequencies.clear();
   tdrsMeasurementFrequencyBands.clear();
   // clean up ObjectArray references;
   //for (Integer i = 0; i < references.size(); ++i)
   //{
   //   if (references[i])
   //      delete references[i];                            // It cannot delete these objects because they waere not created in this class
   //}
   references.clear();

   ionosphereCache.clear();
}

//------------------------------------------------------------------------------
// TrackingFileSet(const TrackingFileSet& tfs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tfs The TrackingFileSet copied to make a new one
 */
//------------------------------------------------------------------------------
TrackingFileSet::TrackingFileSet(const TrackingFileSet& tfs) :
   MeasurementModelBase          (tfs),
   filenames                     (tfs.filenames),
   rampedTablenames              (tfs.rampedTablenames),
   useLighttime                  (tfs.useLighttime),
   solarsystem                   (tfs.solarsystem),
   thePropagators                (tfs.thePropagators),
   satPropagatorMap              (tfs.satPropagatorMap),
   references                    (tfs.references),
   useRelativityCorrection       (tfs.useRelativityCorrection),
   useETminusTAICorrection       (tfs.useETminusTAICorrection),
   aberrationCorrection          (tfs.aberrationCorrection),
   rangeModulo                   (tfs.rangeModulo),
   dopplerCountInterval          (tfs.dopplerCountInterval),
   tdrsServiceAccessList         (tfs.tdrsServiceAccessList),
   tdrsSMARID                    (tfs.tdrsSMARID),
   tdrsDataFlag                  (tfs.tdrsDataFlag),
   tdrsMeasurementFrequencies    (tfs.tdrsMeasurementFrequencies),
   tdrsMeasurementFrequencyBands (tfs.tdrsMeasurementFrequencyBands),

   timeGapForPassBreak       (tfs.timeGapForPassBreak),
   useHorp                   (tfs.useHorp),
   forceHorp                 (tfs.forceHorp),
   horpOblate                (tfs.horpOblate),
   horpHeight                (tfs.horpHeight),
   horpAngle                 (tfs.horpAngle),
   dataFilterNames           (tfs.dataFilterNames)

{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> copy construction from <%s,%p>\n", GetName().c_str(), this, tfs.GetName().c_str(), &tfs);
#endif
   
   for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
      trackingConfigs.push_back(tfs.trackingConfigs[i]);

   // Each tracking file set contains a set of filters in which they have their own thinning frequency counters.
   // Therefore, it needs to have seperate set of data filter objects for each tracking file set object. 
   // Note that: If 2 different tracking file set objects share the same set of data filter objects, values 
   // of frequency counters are mixed up by both tracking file set objects. 
   for (UnsignedInt i = 0; i < tfs.dataFilters.size(); ++i)
   {
      if (tfs.dataFilters[i])
      {
         GmatBase* cloneObj = tfs.dataFilters[i]->Clone();
         dataFilters.push_back(cloneObj);
         createdObjects.push_back(cloneObj);        // It needs to bookkeeping for delete
      }
   }

   isInitialized = false;
}


//------------------------------------------------------------------------------
// TrackingFileSet& operator=(const TrackingFileSet& tfs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tfs The TrackingFileSet that provides parameter updates to this one
 *
 * @return This TrackingFileSet, configured to match tfs
 */
//------------------------------------------------------------------------------
TrackingFileSet& TrackingFileSet::operator=(const TrackingFileSet& tfs)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> operator =  from <%s,%p>\n", GetName().c_str(), this, tfs.GetName().c_str(), &tfs);
#endif

   if (this != &tfs)
   {
      MeasurementModelBase::operator=(tfs);

      // clean up TrackingDataAdapter objects
      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         if (measurements[i])
            delete measurements[i];
      }
      measurements.clear();

      // clean up trackingConfigs
      for (Integer i = 0; i < trackingConfigs.size(); ++i)
      {
         for (Integer j = 0; j < trackingConfigs[i].strands.size(); ++j)
            trackingConfigs[i].strands[j].clear();
         trackingConfigs[i].strands.clear();

         for (Integer j = 0; j < trackingConfigs[i].sensors.size(); ++j)
            trackingConfigs[i].sensors[j].clear();
         trackingConfigs[i].sensors.clear();

         trackingConfigs[i].types.clear();
      }
      trackingConfigs.clear();

      for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
         trackingConfigs.push_back(tfs.trackingConfigs[i]);

      filenames                     = tfs.filenames;
      rampedTablenames              = tfs.rampedTablenames;
      useLighttime                  = tfs.useLighttime;
      solarsystem                   = tfs.solarsystem;
      thePropagators                = tfs.thePropagators;
      satPropagatorMap              = tfs.satPropagatorMap;
      references                    = tfs.references;
      useRelativityCorrection       = tfs.useRelativityCorrection;
      useETminusTAICorrection       = tfs.useETminusTAICorrection;
      aberrationCorrection          = tfs.aberrationCorrection;
      rangeModulo                   = tfs.rangeModulo;
      dopplerCountInterval          = tfs.dopplerCountInterval;
      tdrsServiceAccessList         = tfs.tdrsServiceAccessList;
      tdrsSMARID                    = tfs.tdrsSMARID;
      tdrsDataFlag                  = tfs.tdrsDataFlag;
      tdrsMeasurementFrequencies    = tfs.tdrsMeasurementFrequencies;
      tdrsMeasurementFrequencyBands = tfs.tdrsMeasurementFrequencyBands;

      timeGapForPassBreak          = tfs.timeGapForPassBreak;

      useHorp                      = tfs.useHorp;
      forceHorp                    = tfs.forceHorp;
      horpOblate                   = tfs.horpOblate;
      horpHeight                   = tfs.horpHeight;
      horpAngle                    = tfs.horpAngle;
      dataFilterNames              = tfs.dataFilterNames;
      

      // Remove all dataFilters
      //for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      //{
      //   if (dataFilters[i])
      //      delete dataFilters[i];
      //}
      dataFilters.clear();

      // Create clones of data filters
      for(UnsignedInt i = 0; i < tfs.dataFilters.size(); ++i)
      {
         if (tfs.dataFilters[i])
         {
            GmatBase* clonedObj = tfs.dataFilters[i]->Clone();
            dataFilters.push_back(clonedObj);
            createdObjects.push_back(clonedObj);
         }
      }

      isInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a replica of this TrackingFileSet
 *
 * @return The replica
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::Clone() \n", GetName().c_str(), this);
#endif

   return new TrackingFileSet(*this);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id ID for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterText(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < TrackingFileSetParamCount)
      return PARAMETER_TEXT[id - MeasurementModelBaseParamCount];
   return MeasurementModelBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter unit, given the input parameter ID.
 *
 * @param id ID for the requested parameter text.
 *
 * @return parameter unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterUnit(const Integer id) const
{
   if (id == RANGE_MODULO)
      return "RU";
   if (id == DOPPLER_COUNT_INTERVAL)
      return "sec";
   if (id == HORP_HEIGHT)
      return "km";

   /// @todo: it needs to add unit for HORP_VETRAL_ANGLE
   if (id == HORP_CENTRAL_ANGLE)
      return "";

   return MeasurementModelBase::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str String for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::GetParameterID(const std::string& str) const
{
   for (Integer i = MeasurementModelBaseParamCount; i < TrackingFileSetParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - MeasurementModelBaseParamCount])
         return i;
   }

   return MeasurementModelBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingFileSet::GetParameterType(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < TrackingFileSetParamCount)
      return PARAMETER_TYPE[id - MeasurementModelBaseParamCount];

   return MeasurementModelBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterTypeString(const Integer id) const
{
   return PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// bool TrackingFileSet::IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter field is part of the read/write scripting
 *
 * @param id Field ID value
 *
 * @return false for read/write fields, true if not part of the scripting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::IsParameterReadOnly(const Integer id) const
{
   if ((id == API_TRACKINGCONFIGCOUNT) || (id == API_GET_C_VALUE))
      return true;

   if (useHorp == false)
      if ((id == HORP_HEIGHT) || (id == HORP_CENTRAL_ANGLE))
         return true;

   return MeasurementModelBase::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter field is part of the read/write scripting
 *
 * @param label Script value for the field
 *
 * @return false for read/write fields, true if not part of the scripting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::GetIntegerParameter(const Integer id) const
{
   if (id == TDRS_SMAR_ID)
      return tdrsSMARID;
   if (id == TDRS_TRACKER_TYPE)
      return tdrsDataFlag;

   if (id == API_TRACKINGCONFIGCOUNT)
      return measurements.size();

   return MeasurementModelBase::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == TDRS_SMAR_ID)
   {
      if (value < 1 || value > 30)
         throw MeasurementException("Error: Parameter "+GetName()+"."+GetParameterText(id)+" has an invalid value. It must be within range [1,30]\n");

      tdrsSMARID = value;
      return tdrsSMARID;
   }

   if (id == TDRS_TRACKER_TYPE)
   {
      if ((value != 0)&&(value != 1))
         throw MeasurementException("Error: Parameter "+GetName()+"."+GetParameterText(id)+" has invalid value. Its value has to be 0 or 1.\n");

      tdrsDataFlag = value;
      return tdrsDataFlag;
   }

   return MeasurementModelBase::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const Integer id) const
{
   if (id == RANGE_MODULO)
      return rangeModulo;
   if (id == DOPPLER_COUNT_INTERVAL)
      return dopplerCountInterval;

   if (id == TIMEGAP_FOR_PASSBREAK)
      return timeGapForPassBreak;

   if (id == HORP_HEIGHT)
      return horpHeight;
   if (id == HORP_CENTRAL_ANGLE)
      return horpAngle;

   return MeasurementModelBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a Real parameter from an array of such data
 *
 * Used here in API calls to evaluate measurement C values
 *
 * @param id ID for the field
 * @param index Index into the array.  For measurements, this identifies which
 *              measurement needs to be calculated
 *
 * @return The field value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const Integer id, const Integer index) const
{
   if (id == API_GET_C_VALUE)
   {
      Real retval = -987654321.012345;

      if ((index >= 0) && (index < measurements.size()))
      {
         try
         {
            MeasurementData md = measurements[index]->CalculateMeasurement();
            if (md.isFeasible)
               retval = md.value[0];
            else
               retval = 0.0;
         }
         catch (...)
         {
            retval = -1.0;
         }
      }

      return retval;     // Temporary retval for flow
   }

   return MeasurementModelBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value was not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::SetRealParameter(const Integer id, const Real value)
{
   if (id == RANGE_MODULO)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+"."+GetParameterText(id)+" has an invalid value. It has to be a positive number\n");

      rangeModulo = value;
      return rangeModulo;
   }

   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+"."+GetParameterText(id)+" has an invalid value. It has to be a positive number\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
   }

   if (id == TIMEGAP_FOR_PASSBREAK)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: " + GetName() + "." + GetParameterText(id) + " has an invalid value. It has to be a positive number\n");

      timeGapForPassBreak = value;
      return timeGapForPassBreak;
   }

   if (id == HORP_HEIGHT)
   {
      if (value < 0.0)
         throw MeasurementException("Error: " + GetName() + "." + GetParameterText(id)+
               " has an invalid value. The value must be non-negative.\n");
      useHorp = true;
      forceHorp = true;
      horpHeight = value;
      return horpHeight;
   }

   if (id == HORP_CENTRAL_ANGLE)
   {
      if ((value < 0.0) || (value > 180.0))
         throw MeasurementException("Error: " + GetName() + "." + GetParameterText(id)+
               " has an invalid value. The value must in the range 0.0 <= value <= 180.0.\n");
      useHorp = true;
      forceHorp = true;
      horpAngle = value;
      return horpAngle;
   }

   return MeasurementModelBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a Real parameter from an array of such data
 *
 * Used here in API calls to evaluate measurement C values
 *
 * @param label Script value for the field
 * @param index Index into the array.  For measurements, this identifies which
 *              measurement needs to be calculated
 *
 * @return The field value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const std::string &label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value wwas not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::SetRealParameter(const std::string & label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const Integer id) const
{
   if (id == ABERRATION_CORRECTION)
   {
      return aberrationCorrection;
   }

   return MeasurementModelBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value
 *
 * @param id The ID of the parameter
 * @param value The value
 *
 * @return true is the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const Integer id,
      const std::string& value)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::SetStringParameter(id = %d, "
            "value = '%s') called\n", GetName().c_str(), this, id, value.c_str());
   #endif

   if (id == TRACKINGCONFIG)
   {
      throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".AddTrackingConfig parameter has a syntax error.\n");
   }

   if (id == FILENAME)
   {
      // if it is an empty list, then clear file name list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         filenames.clear();
         return true;
      }

      // check for a valid file name
      Integer err = 0;
      if (!GmatStringUtil::IsValidFullFileName(value, err))
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".FileName parameter is an invalid file name.\n");

      // add name to the list
      if (find(filenames.begin(), filenames.end(), value) == filenames.end())
      {
         filenames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: File name is replicated ('" + value + "')\n");
   }

   if (id == RAMPED_TABLENAME)
   {
      // if it is an empty list, then clear ramp table name list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         rampedTablenames.clear();
         return true;
      }

      // check for a valid file name
      Integer err = 0;
      if (!GmatStringUtil::IsValidFullFileName(value, err))
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".RampTable parameter is an invalid file name.\n");

      // add name to the list
      if (find(rampedTablenames.begin(), rampedTablenames.end(), value) == rampedTablenames.end())
      {
         rampedTablenames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: ramp table name is replicated ('" + value + "')\n");

   }

   if (id == ABERRATION_CORRECTION)
   {
      if (value != "None" && value != "Annual" && value != "Diurnal" && value != "AnnualAndDiurnal")
         throw MeasurementException("Error: TrackingFileSet's AberrationCorrection value must be Annual, "
            "Diurnal, AnnualAndDiurnal, or None.  '" + value + "' is not a valid value.\n");
      aberrationCorrection = value;
      return true;
   }
   
   if (id == DATA_FILTERS)
   {
      // if it is an empty list, then set clear the list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         dataFilterNames.clear();
         return true;
      }
      else if (!GmatStringUtil::IsValidIdentity(value))
         throw MeasurementException("Error: '" + value + " set to " + GetName() + ".DataFilters parameter is an invalid object name.\n");

      if (find(dataFilterNames.begin(), dataFilterNames.end(), value) == dataFilterNames.end())
      {
         dataFilterNames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: name of data filter is replicated ('" + value + "')\n");      
   }

   return MeasurementModelBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value from a StringArray.
 *
 * @param id ID for the requested parameter
 * @param index The index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == TRACKINGCONFIG)
   {
      if (((Integer)trackingConfigs.size() > index) && (index >= 0))
         return trackingConfigs[index].GetDefinitionString();
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a tracking data configuration");
   }

   if (id == FILENAME)
   {
      if (((Integer)filenames.size() > index) && (index >= 0))
         return filenames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a tracking data file name");
   }

   if (id == RAMPED_TABLENAME)
   {
      if (((Integer)rampedTablenames.size() > index) && (index >= 0))
         return rampedTablenames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a ramp table file name");
   }

   if (id == ABERRATION_CORRECTION)
   {
      return aberrationCorrection;
   }
   
   if (id == DATA_FILTERS)
   {
      if (((Integer)dataFilterNames.size() > index) && (index >= 0))
         return dataFilterNames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a data filter name");
   }

   if (id == TDRS_SERVICE_ACCESS)
   {
      if (((Integer)tdrsServiceAccessList.size() > index) && (index >= 0))
         return tdrsServiceAccessList[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a service access list");
   }

   return MeasurementModelBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value in a StringArray.
 *
 * @param id ID for the requested parameter
 * @param value The new value
 * @param index The index into the array
 *
 * @return The string.
 */
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
* This function is used to add a particpant name to the list of signal path's
* particicpants list.
*
* @param participantName      Name of a partcipant along the signal path
*
* @return                     true if a valid participant name, false otherwise
*/
//------------------------------------------------------------------------------
bool TrackingFileSet::AddToSignalPath(std::string participantName, Integer configIndex, Integer strandIndex)
{
   // validate participant name
   if (GmatStringUtil::IsValidExtendedIdentity(participantName) == false)        // It is for GPS Point Solution
      throw MeasurementException("Error: Invalid participant name '" + participantName + "' was set to " + GetName() + ".AddTrackingConfig parameter.\n");
   
   if (strandIndex > 0)
      throw MeasurementException("Error: Syntax error when setting value to " + GetName() + ".AddTrackingConfig parameter. In current version, GMAT does not allow two or more participant lists in tracking configuration.\n");
   
   StringArray names = GmatStringUtil::ParseName(participantName);
   trackingConfigs[configIndex].strands[strandIndex].push_back(names[0]);
   trackingConfigs[configIndex].sensors[strandIndex].push_back(names[1]);
   names.clear();

   return true;
}


//------------------------------------------------------------------------------
// bool ParseTrackingConfig(const std::string value, Integer configIndex)
//------------------------------------------------------------------------------
/**
* This function is used to parse a strand {part1,...,partn} or a tracking config 
* {{part1,..., partn}, datatype1, ..., datatypem}
*
* @param value          a string containing a strand or a tracking config
* @param configIndex    index of this tracking config
* @param start          flag indicates the start of a configuration
*
* @return               true if no syntax error, false otherwise
*/
//------------------------------------------------------------------------------
//std::string TrackingFileSet::CheckTypeDeprecation(const std::string datatype)
//{
//   std::map<std::string, std::string> depTypeMap;
//   TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
//   depTypeMap = mn->GetDeprecatedTypeMap();
//
//   std::string typeName = datatype;
//   for (std::map<std::string, std::string>::iterator i = depTypeMap.begin();
//      i != depTypeMap.end(); ++i)
//   {
//      if ((*i).first == datatype)
//      {
//         std::stringstream ss;
//         ss << "Warning: " << GetName() << ".AddTrackingConfig type name '" 
//            << datatype << "' is deprecated and will be removed in a future release. Use '" 
//            << (*i).second << "' instead.\n";
//
//         if (find(mesg.begin(), mesg.end(), ss.str()) == mesg.end())
//         {
//            MessageInterface::ShowMessage(ss.str());
//            mesg.push_back(ss.str());
//         }
//         typeName = (*i).second;
//         break;
//      }
//   }
//
//   return typeName;
//}


bool TrackingFileSet::ParseTrackingConfig(std::string value, Integer& configIndex, bool& start)
{
   // case 1: parse a strand {part1, ..., partn}
   // case 2: parse a tracking config syntax: {{part1, ..., partn}, ..., {part1, ..., partn}, datatype1, ..., datatypem}

   //MessageInterface::ShowMessage("TrackingFileSet::ParseTrackingConfig('%s', %d)     for TrackingFileSet %s\n", value.c_str(), configIndex, GetName().c_str());

   bool retval = true;

   // Get all known measurement types
   StringArray knownTypes;                                                                  // GMT-5701
   TFSMagicNumbers *mn = TFSMagicNumbers::Instance();                                       // GMT-5701
   knownTypes = mn->GetAvailableTypes();                                                    // GMT-5701

   // Remove opened and closed curly brackets at the begin and end of string
   std::string value1 = GmatStringUtil::Trim(value.substr(1, value.size() - 2));

   // Specify the syntax is a strand or a tracking config
   if (value1.at(0) == '{')
   {
      // case 2: parse a tracking config syntax: {part1, ..., partn}, ..., {part1, ..., partn}, datatype1, ..., datatypem

      std::string::size_type pos;
      // 1. Parse tracking config and get a list of strands
      while (value1.at(0) == '{')
      {
         pos = value1.find_first_of('}');
         if (pos == value1.npos)
            throw MeasurementException("Error: strand '" + value1 + "' missed a closed curly bracket.\n");
         else
         {
            // Get list of strands
            std::string strand = value1.substr(0, pos + 1);

            // Add a slot to strands array in order to store a new strand 
            StringArray trackList, sensorList;
            trackingConfigs[configIndex].strands.push_back(trackList);
            trackingConfigs[configIndex].sensors.push_back(sensorList);
            Integer strandIndex = trackingConfigs[configIndex].strands.size() - 1;

            // clean up
            trackList.clear();
            sensorList.clear();

            // Parse the strand
            retval = ParseStrand(strand, configIndex, strandIndex);
            if (retval == false)
               return retval;

            // Get the remain unparsed string
            value1 = GmatStringUtil::Trim(value1.substr(pos+1));
            if (value1.at(0) != ',')
               throw MeasurementException("Error: syntax error - missing ',' after '" + strand + "'.\n");
            value1 = GmatStringUtil::Trim(value1.substr(1));
         }
      }

      // 2. Parse tracking config and get a list of data types: datatype1, ..., datatypem
      std::string datatype;
      pos = value1.find_first_of(',');
      while (pos != value1.npos)
      {
         // Get data type
         datatype = value1.substr(0, pos);
         
         //// Check deprecated types
         //datatype = CheckTypeDeprecation(datatype);

         // Validate and add data type to data type array
         if (find(knownTypes.begin(), knownTypes.end(), datatype) == knownTypes.end())
         {
            std::stringstream ss;
            ss << "Error: In current version, GMAT does not have measurement type '" + datatype + "'.\n This is a list of all available types:\n";
            for (Integer i = 0; i < knownTypes.size(); ++i)
            {
               if (i < knownTypes.size() - 1)
                  ss << " " << knownTypes[i] << ",";
               else
                  ss << " " << knownTypes[i] << ".\n";
            }
            throw MeasurementException(ss.str());
         }
         else
         {
            if ((!trackingConfigs[configIndex].types.empty()) &&
               (find(trackingConfigs[configIndex].types.begin(), trackingConfigs[configIndex].types.end(), datatype) != trackingConfigs[configIndex].types.end()))
               throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
            else
            {
               trackingConfigs[configIndex].types.push_back(datatype);
            }
         }

         // Get the remain of unparsed string
         value1 = GmatStringUtil::Trim(value1.substr(pos + 1));
         pos = value1.find_first_of(',');
      }

      // Add the last data type to data type array
      datatype = value1;

      //// Check deplicated types
      //datatype = CheckTypeDeprecation(datatype);

      if (find(knownTypes.begin(), knownTypes.end(), datatype) == knownTypes.end())
      {
         std::stringstream ss;
         ss << "Error: In current version, GMAT does not have measurement type '" + datatype + "'.\n This is a list of all available types:\n";
         for (Integer i = 0; i < knownTypes.size(); ++i)
         {
            if (i < knownTypes.size() - 1)
               ss << " " << knownTypes[i] << ",";
            else
               ss << " " << knownTypes[i] << ".\n";
         }
         throw MeasurementException(ss.str());
      }
      else
      {
         if ((!trackingConfigs[configIndex].types.empty()) &&
            (find(trackingConfigs[configIndex].types.begin(), trackingConfigs[configIndex].types.end(), datatype) != trackingConfigs[configIndex].types.end()))
            throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
         else
         {
            trackingConfigs[configIndex].types.push_back(datatype);
         }
      }

      // start a new tracking config
      start = true;
   }
   else
   {
      //case 1: parse a strand syntax: part1, ..., partn
      // Add a slot to strands array in order to store a new strand 
      StringArray trackList, sensorList;
      trackingConfigs[configIndex].strands.push_back(trackList);
      trackingConfigs[configIndex].sensors.push_back(sensorList);
      Integer strandIndex = trackingConfigs[configIndex].strands.size() - 1;

      // clean up
      trackList.clear();
      sensorList.clear();

      // Parse the strand
      return ParseStrand(value, configIndex, strandIndex);
   }
   
   // clean up
   knownTypes.clear();

   return retval;
}


//------------------------------------------------------------------------------
// bool ParsingStrand(const std::string value)
//------------------------------------------------------------------------------
/**
* This function is used to parse strand with syntax {part1,..., partn}
*
* @param value    a string containing a list of partcipants a long signal path
*
* @return         true if no syntax error, false otherwise
*/
//------------------------------------------------------------------------------
bool TrackingFileSet::ParseStrand(std::string value, Integer configIndex, Integer strandIndex)
{
   // Remove open and close curly brackets at the begin and end of string
   value = value.substr(1, value.size() - 2);

   // processing each participant in participants list
   std::string participantName;
   size_t pos = value.find_first_of(',');
   while (pos != std::string::npos)
   {
      participantName = GmatStringUtil::Trim(value.substr(0, pos));
      if (AddToSignalPath(participantName, configIndex, strandIndex) == false)
         return false;

      value = value.substr(pos + 1);
      pos = value.find_first_of(',');
   }

   participantName = GmatStringUtil::Trim(value);
   return AddToSignalPath(participantName, configIndex, strandIndex);
}


bool TrackingFileSet::SetStringParameter(const Integer id,
      const std::string& value1, const Integer index)
{
   std::string value = value1;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::SetStringParameter(%d, "
            "'%s', %d) called\n", GetName().c_str(), this, id, value.c_str(), index);
   #endif

   if (id == TRACKINGCONFIG)
   {
      // Nothing was set to tracking configuration when an empty list was set to AddTrackingConfig parameter
      if (index == -1)
         return true;

      if (value == "")
         throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");


      // Get all known measurement types
      StringArray knownTypes;                                                                  // GMT-5701
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();                                       // GMT-5701
      knownTypes = mn->GetAvailableTypes();                                                    // GMT-5701


      // Create a new tracking config
      if (index == 0)                    // it starts a tracking configuration when index = 0
      {
         // reset open bracket count and new strand flag
         openBracketCount = 0;
         start = true;
      }

      if (start)
      {
         // Starting a new definition
         MeasurementDefinition newDef;
         trackingConfigs.push_back(newDef);
         start = false;
      }
      Integer defIndex = trackingConfigs.size() - 1;

      if ((value.size() > 1) && (value.c_str()[0] == '{') && (value.c_str()[value.size() - 1] == '}'))
      {
         // parsing syntax for: 
         // case1: {part1,..., partn}
         // case2: {{part1,..., partn}, datatype1,...,datatypem}
         if (openBracketCount != 0)
            throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

         return ParseTrackingConfig(value, defIndex, start);
      }
      else
      {
         // parsing the follwoing cases:
         // case 3: '{<part>'
         // case 4: '<part>}'
         // case 5: '<part>'
         // case 6: '<typename>'

         std::string rawName = GmatStringUtil::Trim(value);
         if (rawName.size() == 0)
            throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

         // case 3: '{<part>'
         if (rawName.at(0) == '{')
         {
            if (openBracketCount != 0)
               throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

            ++openBracketCount;
            rawName = GmatStringUtil::Trim(rawName.substr(1));                          // trim open curly bracket and all white space
            
            StringArray trackList, sensorList;
            trackingConfigs[defIndex].strands.push_back(trackList);
            trackingConfigs[defIndex].sensors.push_back(trackList);
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);

            // clean up
            trackList.clear();
            sensorList.clear();

            return true;
         }


         // case 4: '<part>}'
         if (rawName.at(rawName.size() - 1) == '}')
         {
            if (openBracketCount == 0)
               throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

            --openBracketCount;
            rawName = GmatStringUtil::Trim(rawName.substr(0, rawName.size() - 1));
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);

            return true;
         }

         // case 5: '<part>'
         if (openBracketCount > 0)
         {
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);
            
            return true;
         }

         // case 6:
         //// Check deplicated types
         //rawName = CheckTypeDeprecation(rawName);

         if (find(knownTypes.begin(), knownTypes.end(), rawName) == knownTypes.end())
         {
            std::stringstream ss;
            ss << "Error: In current version, GMAT does not have measurement type '" + rawName + "'.\n This is a list of all available types:\n";
            for (Integer i = 0; i < knownTypes.size(); ++i)
            {
               if (i < knownTypes.size() - 1)
                  ss << " " << knownTypes[i] << ",";
               else
                  ss << " " << knownTypes[i] << ".\n";
            }
            throw MeasurementException(ss.str());
         }
         else
         {
            if ((!trackingConfigs[defIndex].types.empty()) &&
                (find(trackingConfigs[defIndex].types.begin(), trackingConfigs[defIndex].types.end(), rawName) != trackingConfigs[defIndex].types.end()))
               throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
            else
            {
               trackingConfigs[defIndex].types.push_back(rawName);
               return true;
            }
         }
      }

      // clean up
      knownTypes.clear();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               trackingConfigs.size());
         for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", trackingConfigs[i].GetDefinitionString().c_str());
      #endif
      return true;
   }

   if (id == FILENAME)
   {
      // throw an error message when it is an empty list
      if (index == -1)
         throw MeasurementException("Error: No file name was set to " + GetName() + ".Filenames parameter.\n");

      if ((!filenames.empty())&&
          (find (filenames.begin(), filenames.end(), value) != filenames.end()))
         throw MeasurementException("Error: replication of file name ('" + value + "').\n");

      if (((Integer)filenames.size() > index) && (index >= 0))
         filenames[index] = value;
      else if ((Integer)filenames.size() == index)
         filenames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a tracking data file name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               filenames.size());
         for (UnsignedInt i = 0; i < filenames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", filenames[i].c_str());
      #endif
      return true;
   }

   if (id == RAMPED_TABLENAME)
   {
      // an empty list is set to RampTables parameter when index == -1
      if (index == -1)
      {
         rampedTablenames.clear();
         return true;
      }

      if ((!rampedTablenames.empty())&&
          (find(rampedTablenames.begin(), rampedTablenames.end(), value) != rampedTablenames.end()))
         throw MeasurementException("Error: replication of ramp table name ('" + value + "').\n");

      if (((Integer)rampedTablenames.size() > index) && (index >= 0))
         rampedTablenames[index] = value;
      else if ((Integer)rampedTablenames.size() == index)
         rampedTablenames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a ramp table file name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               rampedTablenames.size());
         for (UnsignedInt i = 0; i < rampedTablenames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", rampedTablenames[i].c_str());
      #endif
      return true;
   }

   if (id == DATA_FILTERS)
   {
      // return true when it is an empty list
      if (index == -1)
      {
         dataFilterNames.clear();
         return true;
      }

      if ((!dataFilterNames.empty())&&
          (find(dataFilterNames.begin(), dataFilterNames.end(), value) != dataFilterNames.end()))
         throw MeasurementException("Error: replication of data filter name ('" + value + "').\n");
      
      if (((Integer)dataFilterNames.size() > index) && (index >= 0))
         dataFilterNames[index] = value;
      else if ((Integer)dataFilterNames.size() == index)
         dataFilterNames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a data filter name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in DataFilters:\n",
               dataFilterNames.size());
         for (UnsignedInt i = 0; i < dataFilterNames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", dataFilterNames[i].c_str());
      #endif
      return true;
   }

   if (id == TDRS_SERVICE_ACCESS)
   {
      // an empty list is set to TDARSServiceAccess parameter when index == -1
      if (index == -1)
      {
         tdrsServiceAccessList.clear();
         return true;
      }

      if ((!tdrsServiceAccessList.empty()) &&
         (find(tdrsServiceAccessList.begin(), tdrsServiceAccessList.end(), value) != tdrsServiceAccessList.end()))
         throw MeasurementException("Error: replication of service access name ('" + value + "').\n");

      if (((Integer)tdrsServiceAccessList.size() > index) && (index >= 0))
         tdrsServiceAccessList[index] = value;
      else if ((Integer)tdrsServiceAccessList.size() == index)
         tdrsServiceAccessList.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
            "set a service access name");

#ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("%d names in TDRS service access list:\n",
         tdrsServiceAccessList.size());
      for (UnsignedInt i = 0; i < tdrsServiceAccessList.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", tdrsServiceAccessList[i].c_str());
#endif
      return true;
   }

   return MeasurementModelBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array
 *
 * @param id The ID of the array
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const Integer id) const
{
#ifdef DEBUG_GET_PARAMETER
   MessageInterface::ShowMessage("TrackingFileSet::GetStringArrayParameter(id = %d).\n", id);
#endif

   static StringArray tconfigs;

   if (id == TRACKINGCONFIG)
   {
      tconfigs.clear();
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         tconfigs.push_back(trackingConfigs[i].GetDefinitionString());
      return tconfigs;
   }

   if (id == FILENAME)
      return filenames;

   if (id == RAMPED_TABLENAME)
      return rampedTablenames;

   if (id == TDRS_SERVICE_ACCESS)
      return tdrsServiceAccessList;

   if (id == DATA_FILTERS)
      return dataFilterNames;
   
   return MeasurementModelBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array from a vector of string arrays
 *
 * @param id The ID for the string array vector
 * @param index The string array in the vector that is wanted
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   /// todo: Build decomposed reference stuff if we need external access to it
   if (id == TRACKINGCONFIG)
      if ((index >= 0) && (index < (Integer)trackingConfigs.size()))
         return trackingConfigs[index].types;

   return MeasurementModelBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value, given the input parameter ID.
 *
 * @param label Scripted string for the requested parameter.
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value
 *
 * @param label Scripted string for the requested parameter.
 * @param value The value
 *
 * @return true is the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value from a StringArray.
 *
 * @param label Scripted string for the requested parameter.
 * @param index The index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value in a StringArray.
 *
 * @param label Scripted string for the requested parameter.
 * @param value The new value
 * @param index The index into the array
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array
 *
 * @param label Scripted string for the requested parameter.
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array from a vector of string arrays
 *
 * @param label Scripted string for the requested parameter.
 * @param index The string array in the vector that is wanted
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const Integer id) const
{
   if (id == USE_RELATIVITY)
      return useRelativityCorrection;

   if (id == USE_ETMINUSTAI)
      return useETminusTAICorrection;

   if (id == USELIGHTTIME)
      return useLighttime;

   return MeasurementModelBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param id ID for the parameter
 * @param value The new value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_RELATIVITY)
   {
      useRelativityCorrection = value;
      return useRelativityCorrection;
   }

   if (id == USE_ETMINUSTAI)
   {
      useETminusTAICorrection = value;
      return useETminusTAICorrection;
   }

   if (id == USELIGHTTIME)
   {
      useLighttime = value;
      return useLighttime;
   }

   return MeasurementModelBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param id The ID for the parameter
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const Integer id,
      const Integer index) const
{
   return MeasurementModelBase::GetBooleanParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter from an array of Boolean values
 *
 * @param id The ID for the parameter
 * @param value The new value
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const Integer id, const bool value,
      const Integer index)
{
   return MeasurementModelBase::SetBooleanParameter(id, value, index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const std::string& label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string& label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param label Script label for the parameter
 * @param value The new value
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const std::string& label,
      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const std::string& label,
      const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string& label, const bool value,
//      const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 * @param value The new value
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const std::string& label,
      const bool value, const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param id ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt TrackingFileSet::GetPropertyObjectType(const Integer id) const
{
   if (id == DATA_FILTERS)
      return Gmat::DATA_FILTER;

   return MeasurementModelBase::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a single reference object
 *
 * @param type The type of the desired object
 *
 * @return The name of the object
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetRefObjectName(const UnsignedInt type) const
{
   return MeasurementModelBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types the object expects
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& TrackingFileSet::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   /// @todo: remove when they are autogenerated
   refObjectTypes.push_back(Gmat::DATA_FILE);
   refObjectTypes.push_back(Gmat::DATA_FILTER);

   return refObjectTypes;
}

//------------------------------------------------------------------------------
// const StringArray& TrackingFileSet::GetRefObjectNameArray(
//       const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the names of reference objects
 *
 * @param type The object type, or Gmat::UNKNOWN_OBJECT for all ref objects
 *
 * @return The list of reference object names
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetRefObjectNameArray(
      const UnsignedInt type)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "GetRefObjectNameArray(%d)\n", GetName().c_str(), this, type);
   #endif

   refObjectNames.clear();

   // @todo Make more robust by dropping the types rather than assuming only
   //       one at the end.  Basically, get the bracket parsing working
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT))
   {
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
            for (UnsignedInt k=0; k < trackingConfigs[i].strands[j].size(); ++k)
         {
            if (find(refObjectNames.begin(), refObjectNames.end(),
                  trackingConfigs[i].strands[j][k]) == refObjectNames.end())
               refObjectNames.push_back(trackingConfigs[i].strands[j][k]);
         }
   }

   //// @todo This loop statement is temporary code.  Remove when datafile objects
   //// are autogenerated.  The current code sets DataFile object names in the
   //// filenames array
   //if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATA_FILE))
   //{
   //   for (UnsignedInt i = 0; i <  filenames.size(); ++i)
   //      refObjectNames.push_back(filenames[i]);
   //}

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATA_FILTER))
   {
      for (UnsignedInt i = 0; i <  dataFilterNames.size(); ++i)
         refObjectNames.push_back(dataFilterNames[i]);
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Reference objects:  ");
      for (UnsignedInt i = 0; i < refObjectNames.size(); ++i)
         MessageInterface::ShowMessage("'%s'  ", refObjectNames[i].c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets names for reference objects
 *
 * @param type The type of object being named
 * @param name The object name
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObjectName(const UnsignedInt type,
      const std::string& name)
{
   bool retval = false;
   return (retval ? retval:MeasurementModelBase::SetRefObjectName(type, name));
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//      const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Renames object references based on a rename in the user interface
 *
 * @param type The type of the object that is renamed
 * @param oldName The old name of the object
 * @param newName The new name
 *
 * @return true if a name was changed, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "RenameRefObject(type = %d, oldName = '%s', newName = '%s')\n", GetName().c_str(), this, type, oldName.c_str(), newName.c_str());
   #endif

   if ((type == Gmat::DATA_FILE) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < filenames.size(); ++i)
      {
         if (filenames[i] == oldName)
         {
            filenames[i] = newName;
            return true;
         }
      }
   }

   if ((type == Gmat::SPACE_POINT) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      {
         for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
         {
            for (UnsignedInt k = 0; k < trackingConfigs[i].strands[j].size(); ++k)
            {
               if (trackingConfigs[i].strands[j][k] == oldName)
               {
                  trackingConfigs[i].strands[j][k] = newName;
                  return true;
               }
            }
         }
      }
   }

   if ((type == Gmat::DATA_FILTER) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < dataFilterNames.size(); ++i)
      {
         if (dataFilterNames[i] == oldName)
         {
            dataFilterNames[i] = newName;
            return true;
         }
      }
   }

   return MeasurementModelBase::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object, given its type and name
 *
 * @param type The object type
 * @param name The object name
 *
 * @return The object pointer, or NULL if it is not set
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::GetRefObject(const UnsignedInt type,
      const std::string& name)
{
   if ((type == Gmat::DATA_FILTER)||(type == Gmat::UNKNOWN_OBJECT))
   {
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      {
         if (dataFilters[i]->GetName() == name)
            return dataFilters[i];
      }
   }

   return MeasurementModelBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string& name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object from an array of objects
 *
 * @param type The object type
 * @param name The object name
 * @param index The index of the object in the object array
 *
 * @return The object pointer, or NULL if it is not set
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::GetRefObject(const UnsignedInt type,
      const std::string& name, const Integer index)
{
   if (type == Gmat::SPACE_POINT)
   {
      if ((index < (Integer)references.size()) && (index >= 0))
         if (references[index]->GetName() == name)
            return references[index];
   }

   if (type == Gmat::DATA_FILTER)
   {
      if ((index < (Integer)dataFilters.size()) && (index >= 0))
         if (dataFilters[index]->GetName() == name)
            return dataFilters[index];
   }

   return MeasurementModelBase::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object pointer
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true if a pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "SetRefObject(obj = %p, type = %d, name = '%s')\n", GetName().c_str(), this, obj, type, name.c_str());
   #endif


   bool retval = false;

   if (obj->IsOfType(Gmat::DATA_FILTER))
   {
      bool found = false;
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      {
         if (obj->GetName() == dataFilters[i]->GetName())
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding data filter %s\n", name.c_str());
         #endif
            GmatBase* clonedObj = obj->Clone();
            dataFilters.push_back(clonedObj);
            createdObjects.push_back(clonedObj);
            return true;
      }
   }
   else
   {
      if (find(references.begin(), references.end(), obj) == references.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding node %s\n", name.c_str());
         #endif
         references.push_back(obj);
         return true;
      }
   }

   return (retval || MeasurementModelBase::SetRefObject(obj, type, name));
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a reference object pointer in an object array
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The name of the object
 * @param index The array index for the object
 *
 * @return true if a pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name, const Integer index)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "SetRefObject(obj = %p, type = %d, name = '%s', index = %d)\n", GetName().c_str(), this, obj, type, name.c_str(), index);
   #endif

   bool retval = false;

   if (type == Gmat::DATA_FILTER)
   {
      if ((index < (Integer)dataFilters.size()) && (index >= 0))
      {
         // The delete of DataFilter objects is handled when delete createdObjects
         //if (dataFilters[index])
         //   delete dataFilters[index];

         // The new one has to be cloned object to avoid error in destructor
         GmatBase* clonedObj = obj->Clone();
         dataFilters[index] = clonedObj;
         createdObjects.push_back(clonedObj);
         retval = true;
      }
   }
   else
   {
      if (find(references.begin(), references.end(), obj) == references.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding node %s\n", name.c_str());
         #endif
         references.push_back(obj);
         retval = true;
      }
   }

   return (retval || MeasurementModelBase::SetRefObject(obj,type,name,index));
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingFileSet::GetRefObjectArray(const UnsignedInt type)
{
   static ObjectArray objectList;
   objectList.clear();

   objectList = MeasurementModelBase::GetRefObjectArray(type);
   
   if ((type == Gmat::SPACE_POINT) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < references.size(); ++i)
         objectList.push_back(references[i]);
   }
   
   if ((type == Gmat::DATA_FILTER) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
         objectList.push_back(dataFilters[i]);
   }

   return objectList;
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects
 *
 * @param type The label for the type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingFileSet::GetRefObjectArray(const std::string& typeString)
{
   return GetRefObjectArray(GetObjectType(typeString));
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem* ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system for the measurements
 *
 * @param ss The solar system
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetSolarSystem(SolarSystem* ss)
{
   solarsystem = ss;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator pointer used for light time computations
 *
 * This is a method provided for API users that need to configure tracking
 * file sets without a driving Simulator or Estimator.  GMAT Application coders
 * should not call into this method.
 *
 * @param ps The propagator
 *
 * @todo: Generalize for multiple propagators once that support is implemented
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetPropagator(PropSetup *ps)
{
   if (thePropagators == NULL)
   {
      if (propvec.size() == 0)
      {
         propvec.push_back(ps);
         // Add an empty sat list to the API prop-sat map
         StringArray sats;
         spm[ps->GetName()] = sats;
      }
      else
         propvec[0] = ps;
   }
   else
   {
      throw MeasurementException("SetPropagator should only be called when "
            "there is no Solver supplying propagator data");
   }
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps, const std::string &forSpacecraft)
//------------------------------------------------------------------------------
/**
 * Sets the propagator used for light time iterations for a specific spacecraft
 *
 * This is a method provided for API users that need to configure tracking
 * file sets without a driving Simulator or Estimator.  GMAT Application coders
 * should not call into this method.
 *
 * @param ps The propagator
 * @param forSpacecraft The spacecraft that uses this propagator
 *
 * @todo: Generalize for multiple propagators once that support is implemented
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetPropagator(PropSetup *ps, const std::string &forSpacecraft)
{
   if (thePropagators == NULL)
   {
      if (find(propvec.begin(), propvec.end(), ps) == propvec.end())
         propvec.push_back(ps);

      std::string propName = ps->GetName();
      if (spm.find(propName) == spm.end())
      {
         StringArray sats;
         spm[propName] = sats;
      }

      if (forSpacecraft != "")
      {
         if (find(spm[propName].begin(), spm[propName].end(), forSpacecraft) == spm[propName].end())
            spm[propName].push_back(forSpacecraft);
      }
   }
   else
   {
      throw MeasurementException("SetPropagator should only be called when "
            "there is no Solver supplying propagator data");
   }
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator pointer used for light time computations
 *
 * @param ps The propagator
 *
 * @todo: Generalize for multiple propagators once that support is implemented
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_PROPAGATORS
      MessageInterface::ShowMessage("Setting propagator in the tracking file set "
            "%s to <%p>\n", instanceName.c_str(), ps);
   #endif
   thePropagators = ps;
   satPropagatorMap = spMap;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the tracking file set for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::Initialize()
{
   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("Entered TrackingFileSet::Initialize() <%s,%p>\n", GetName().c_str(), this);
   #endif
   
   if (this->IsInitialized())
      return true;

   bool retval = false;
   if (MeasurementModelBase::Initialize())
   {
      // Count the adapters needed
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
      //StringArray knownTypes = mn->GetKnownTypes();
      StringArray knownTypes = mn->GetAvailableTypes();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Tracking Configuration:\n");
         for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         {
            MessageInterface::ShowMessage("  %3d: ", i);
            MessageInterface::ShowMessage("%s\n",
                  trackingConfigs[i].GetDefinitionString().c_str());
         }
      #endif
      if (tdrsServiceAccessList.empty())
         tdrsServiceAccessList.push_back("SA1");

      // Check for observation data file 
      if (filenames.size() == 0)
         throw MeasurementException("No observation data file is set to " + GetName() + " object.\n");

      // Build the adapters
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      {
         // Check to make sure only one strand defined in TrackingConfig
         if (trackingConfigs[i].strands.size() != 1)
            throw MeasurementException("Multiple strands and empty strands "
                     "are not yet implemented");
         // Check to make sure at least one measurement type defined in TrackingConfig
         if (trackingConfigs[i].types.size() == 0)
            throw MeasurementException("No measurement type is defined in " + GetName() + ".AddTrackingConfig.\n");

         for (UnsignedInt j = 0; j < trackingConfigs[i].types.size(); ++j)
         {
            // Processing for all measurement types using Receiver in a strand such as GPS_PosVec
            if (trackingConfigs[i].types[j] == "GPS_PosVec")
            {
               // Verify participant is a reveiver
               for (UnsignedInt k = 0; k < trackingConfigs[i].sensors.size(); ++k)
               {
                  if (trackingConfigs[i].sensors[k].at(0) == "")
                     throw MeasurementException("Error: No GPS Receiver was set to the strand in "
                     + GetName() + ".AddTrackingConfig parameter for GPS_PosVec measurement type.\n");
               }
            }

            // Build a tracking data adapter for a given tracking configuration
            TrackingDataAdapter *tda =
                  BuildAdapter(trackingConfigs[i].strands[0], 
                        trackingConfigs[i].sensors[0], 
                        trackingConfigs[i].types[j], i);
            createdObjects.push_back((GmatBase*)tda);
            
            if (tda == NULL)
            {
               throw MeasurementException("Unable to build the " +
                     trackingConfigs[i].types[j] + " measurement");
            }

            // Set time gap for pass break to tracking data adapter
            tda->SetRealParameter("TimeGapForPassBreak", timeGapForPassBreak);


            // Push tracking data adapter to measurement model list
            measurements.push_back(tda);
            if (useHorp || forceHorp)
               tda->SetupHorp(horpHeight, horpAngle, horpOblate, forceHorp);
         }
      }

      // Pass in ref objects
      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         StringArray refObjects = measurements[i]->GetRefObjectNameArray(
               Gmat::UNKNOWN_OBJECT);

         for (UnsignedInt j = 0; j < refObjects.size(); ++j)
         {
            GmatBase *obj = NULL;
            for (UnsignedInt k = 0; k < references.size(); ++k)
            {
               if (references[k]->GetName() == refObjects[j])
               {
                  obj = references[k];
                  break;
               }
            }
            if (obj)
            {
               if (measurements[i]->SetRefObject(obj, obj->GetType(),
                     obj->GetName()) == false)
                  MessageInterface::ShowMessage("The reference object %s was "
                        "requested but not set in a measurement referenced "
                        "by %s\n", obj->GetName().c_str(),
                        instanceName.c_str());
            }
            else
            {
               throw MeasurementException("The reference object " +
                     refObjects[j] + " was requested but not available in the "
                     "tracking file set " + instanceName);
            }
         }

         // clean up memmory
         refObjects.clear();
      }

      retval = true;

      // Initialize the Adapters
      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         measurements[i]->SetSolarSystem(solarsystem);
         if (thePropagators)
            measurements[i]->SetPropagators(thePropagators, satPropagatorMap);
         else if (propvec.size() > 0)
            measurements[i]->SetPropagators(&propvec, &spm);

         // Initialize frequency lists
         tdrsMeasurementFrequencies.push_back(0.0);
         tdrsMeasurementFrequencyBands.push_back("None");

         // Set measurement corrections to TrackingDataAdapter
         if (useRelativityCorrection)
            measurements[i]->SetCorrection("Moyer","Relativity");
         if (useETminusTAICorrection)
            measurements[i]->SetCorrection("Moyer","ET-TAI");
         measurements[i]->SetCorrection("Aberration", "Aberration-" + aberrationCorrection);

         std::string measType = measurements[i]->GetStringParameter("MeasurementType");

         // Set ramped frequency tables to TrackingDataAdapter
         for (UnsignedInt k = 0; k < rampedTablenames.size(); ++k)
         {
            measurements[i]->SetStringParameter("RampTables", rampedTablenames[k], k);
         }

         // Set range modulo constant for DSN_SeqRange
         if (measType == "DSN_SeqRange" || measType == "DSN_PNRange")
         {
            measurements[i]->SetRealParameter("RangeModuloConstant", rangeModulo);
         }

         // Set service access, node 4 frequency and band, SMAR id for TDRSDoppler
         if ((measType == "SN_Doppler") || (measType == "BRTS_Doppler") || (measType == "SN_Doppler_Rtn"))
         {
            // set the list of service access to TDRSDopplerAdapter object
            for (UnsignedInt accIndex = 0; accIndex < tdrsServiceAccessList.size(); ++accIndex) 
               measurements[i]->SetStringParameter("ServiceAccess", tdrsServiceAccessList[accIndex], accIndex);

            //if (measType != "SN_Doppler_Rtn")
            //{
               // set node 4 frequency and frequency band to TDRSDopplerAdapter object
            measurements[i]->SetRealParameter("Node4Frequency", GetTransmitterFrequency(i, measType));
            measurements[i]->SetIntegerParameter("Node4Band", GetFrequencyBand(i));
            //}
            
            // set SMARD Id and TDRS data flag to TDRSDopplerAdapter object
            measurements[i]->SetIntegerParameter("SmarId", tdrsSMARID);
            measurements[i]->SetIntegerParameter("DataFlag", tdrsDataFlag);
         }

         // Set doppler count interval for Doppler
         if ((measType == "DSN_TCP")||(measType == "RangeRate")||(measType == "SN_Doppler") || (measType == "BRTS_Doppler") || (measType == "SN_Doppler_Rtn"))
         {
            measurements[i]->SetRealParameter("DopplerCountInterval", dopplerCountInterval);
         }

         // Set parameters for TDRS DOWD measurement type
         if (measType == "SN_DOWD")
         {
            // set SMARD Id and TDRS data flag to the reference TDRS
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("SmarIdTDRSRef", tdrsSMARID);
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("DataFlagTDRSRef", tdrsDataFlag);

            // set doppler count interval to the reference TDRS
            ((TDRSDOWDAdapter*)measurements[i])->SetRealParameter("DopplerCountIntervalTDRSRef", dopplerCountInterval);


            // Set parameters to the comparison TDRS
            // set the list of service access to the comparison TDRS
            for (UnsignedInt accIndex = 0; accIndex < tdrsServiceAccessList.size(); ++accIndex)
               ((TDRSDOWDAdapter*)measurements[i])->SetStringParameter("ServiceAccessTDRSComp", tdrsServiceAccessList[accIndex], accIndex);

            // set node 4 frequency and frequency band to the comparison TDRS
            ((TDRSDOWDAdapter*)measurements[i])->SetRealParameter("Node4FrequencyTDRSComp", GetTransmitterFrequency(i, measType));
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("Node4BandTDRSComp", GetFrequencyBand(i));
            ((TDRSDOWDAdapter*)measurements[i])->SetRealParameter("Node4FrequencyTDRSRef", GetTransmitterFrequency(i, measType));
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("Node4BandTDRSRef", GetFrequencyBand(i));

            // set SMARD Id and TDRS data flag to the comparison TDRS
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("SmarIdTDRSComp", tdrsSMARID);
            ((TDRSDOWDAdapter*)measurements[i])->SetIntegerParameter("DataFlagTDRSComp", tdrsDataFlag);

            // set doppler count interval to the comparison TDRS
            ((TDRSDOWDAdapter*)measurements[i])->SetRealParameter("DopplerCountIntervalTDRSComp", dopplerCountInterval);
         }

         /// This code for RnageRateAdapterKps is no longer in use. It was done in GNDopplerAdapter class (the lines right above)
         //// Set DopplerInterval for RangeRate
         //if (measType == "RangeRate")
         //{
         //   measurements[i]->SetRealParameter("DopplerInterval", dopplerCountInterval);      // unit: second
         //}

         if (measType == "GPS_PosVec")
         {
            // set value to parameters for GPS position vector measurement
            // Currently nothing needs to set
         }

         // Initialize TrackingDataAdapter
         retval = retval && measurements[i]->Initialize();

         if (measType == "DSN_SeqRange" || measType == "DSN_TCP" || measType == "DSN_PNRange")
         {
             if (!GmatGlobal::Instance()->InTestingMode())
             {
                 if (measurements[i]->IsCrossLink())
                 {
                     std::stringstream ss;
                     ss << "Error: Cannot add a tracking config that uses cross-link or inter-spacecraft tracking for " << measType <<".\n";
                     throw MeasurementException(ss.str());
                 }
             }
         }

      }
      
      // Initialize data filters
      for (UnsignedInt i = 0; i < dataFilters.size(); ++i)
         dataFilters[i]->Initialize();

      // Verify referent objects setting
      StringArray nameList = GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      ObjectArray objList = GetRefObjectArray(Gmat::UNKNOWN_OBJECT);
      for (UnsignedInt i = 0; i < nameList.size(); ++i)
      {
         std::string objName = nameList[i];
         bool found = false;
         for (UnsignedInt j = 0; j < objList.size(); ++j)
         {
            if (objName == objList[j]->GetName())
               found = true;
         }
         if (!found)
         {
            throw MeasurementException("Error: GMAT object with name '" + objName + "' was not defined in script.\n");
         }
      }

      // clean up
      nameList.clear();
      objList.clear();

      isInitialized = true;
   }
   
   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("dataFilters.size() = %d\n", dataFilters.size());
      for (UnsignedInt i = 0; i < dataFilters.size(); ++i)
         MessageInterface::ShowMessage("filter %d: name = '%s'   pointer = <%p>\n", i, dataFilters[i]->GetName().c_str(), dataFilters[i]);

      MessageInterface::ShowMessage("Exit TrackingFileSet::Initialize() <%s,%p>\n", GetName().c_str(), this);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const STringArray& GetParticipants() const
//------------------------------------------------------------------------------
/**
 * Retrieves the participants specified in the tracking file set
 *
 * @return  a string array containing names of all participants listed in tracking file set
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetParticipants() const
{                                                                                         
#ifdef DEBUG_GET_PARTICIPANT
   MessageInterface::ShowMessage("TrackingFileSet::GetParticipants().\n");
#endif

   static StringArray tconfigs;
   tconfigs.clear();

   for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
         for (UnsignedInt k=0; k < trackingConfigs[i].strands[j].size(); ++k)
            if (find(tconfigs.begin(), tconfigs.end(),
                  trackingConfigs[i].strands[j][k]) == tconfigs.end())
               tconfigs.push_back(trackingConfigs[i].strands[j][k]);

   return tconfigs;
}


//------------------------------------------------------------------------------
// TrackingDataAdapter* GetAdapter(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves the tracking data adapters specified in the tracking file set
 *
 * @return The adapters, in an std::vector
 */
//------------------------------------------------------------------------------
TrackingDataAdapter *TrackingFileSet::GetAdapter(Integer index)
{
   if ((index < measurements.size()) && (index >= 0))
      return measurements[index];
   return NULL;
}


//------------------------------------------------------------------------------
// std::vector<TrackingDataAdapter*> *GetAdapters()
//------------------------------------------------------------------------------
/**
 * Retrieves the tracking data adapters specified in the tracking file set
 *
 * @return The adapters, in an std::vector
 */
//------------------------------------------------------------------------------
std::vector<TrackingDataAdapter*> *TrackingFileSet::GetAdapters()
{
   return &measurements;
}


//------------------------------------------------------------------------------
// TrackingDataAdapter* BuildAdapter(const StringArray& strand,
//       const std::string& type)
//------------------------------------------------------------------------------
/**
 * Builds a tracking data adapter for the tracking file set
 *
 * @param strand          The ordered list of nodes in the strands used in the adapter
 * @param sensors         The ordered list of sensors associated to spacecrafts  
 *                           (using for GPS Point Solution)
 * @param type            The type of adapter needed
 *
 * @return                The adapter
 *
 * @todo This implementation is not the final code.  The adapter should be built
 *       in a Factory so that users can add new models without changing core nav
 *       code.  In addition, it is currently coded for only one strand.
 */
//------------------------------------------------------------------------------
TrackingDataAdapter* TrackingFileSet::BuildAdapter(const StringArray& strand, const StringArray& sensors,
      const std::string& type, Integer configIndex)
{
#ifdef DEBUG_BUILD_ADAPTER
   MessageInterface::ShowMessage("TrackingFileSet::BuildAdapter(, type = <%s>, configIndex = %d)   enter\n", type.c_str(), configIndex);
#endif

   TrackingDataAdapter *retval = NULL;
   std::vector<StringArray> nodelist;
   StringArray currentStrand;
   std::map<std::string,std::string> designators;
   bool isOneWay = false;
   std::stringstream designator;
   // Counts for spacecraft and stations identified in GEODYN format
   Integer sCount = 0, tCount = 0;

   // Use for signals that should omit spacecraft transponder delays
   bool passiveMeasureModel = false;

   /// @todo: Check to see if this code works for multi-strand measurements.
   ///        Original code assumed a single strand, so it may need modification
   ///        here and wrapping in an outer loop.  Hence the indentation.
   
   // 1. Set value for designators map:
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      //designator.str() = "";
      // Search for a referent object with the same name of node strand[i]
      UnsignedInt j = 0;
      for (; j < references.size(); ++j)
      {
         if (references[j]->GetName() == strand[i])
            break;
      }

      // If found then add value to designators map
      if (j < references.size())
      {
         UnsignedInt pType = references[j]->GetType();

         switch(pType)
         {
            case Gmat::SPACECRAFT:
               if (designators.find(strand[i]) == designators.end())
               {
                  ++sCount;
                  designator.str("");
                  designator << "S" << sCount;
                  designators[strand[i]] = designator.str();
                  if (i == 0)
                     isOneWay = true;
               }
               break;

            case Gmat::GROUND_STATION:
               if (designators.find(strand[i]) == designators.end())
               {
                  ++tCount;
                  designator.str("");
                  designator << "T" << tCount;
                  designators[strand[i]] = designator.str();
               }
               break;

            default:
               throw MeasurementException("Object type not recognized in "
                           "the tracking file set strand mapping code");
               break;  // No effect, but it avoids a warning message
         }
      }
   }
   
   // And now build the node list
   /// @todo: Check as described above.
   // 2. Add all spacecraft and station IDs to currentStrand
   currentStrand.clear();
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      currentStrand.push_back(designators[strand[i]]);
   }
   nodelist.push_back(currentStrand);

   
   /// @todo Move this into a Factory so that plugin adapters work
   // 3. Create TrackingDataAdapter for a given measurement type
   std::stringstream ss;
   //ss << instanceName << "_Cfig" << configIndex+1 << "_" << type;
   ss << instanceName << "_{";
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      ss << strand[i];
      if (i < strand.size()-1)
         ss << ",";
   }
   ss << "}_" << type;
   std::string adapterName = ss.str();          // tracking adapter name contains TrackingFileSet name following tracking configuration index and type 
   //MessageInterface::ShowMessage("Adapter name: <%s>\n", adapterName.c_str());

   if (type == "Range")                                    // It is GN (or USN) Range
   {
      //retval = new RangeAdapterKm(adapterName);
      retval = new GNRangeAdapter(adapterName);
   }
   else if (type == "Range_Skin")
   {
      retval = new RangeSkinAdapter(adapterName);
      passiveMeasureModel = true;
   }
   else if (type == "SN_Range")                                    // It is SN Range     (TDRS Range measurement)
   {
      retval = new TDRSRangeAdapter(adapterName);
   }
   else if (type == "BRTS_Range")                                 // It is SN Range     (BRTS Range measurement)
   {
      retval = new BRTSRangeAdapter(adapterName);
   }
   else if (type == "DSN_PNRange")                               // It is DSN Range
   {
       retval = new DSNPNRangeAdapter(adapterName);
   }
   else if (type == "DSN_SeqRange")                               // It is DSN Range
   {
      retval = new DSNRangeAdapter(adapterName);
   }
   else if (type == "DSN_TCP")                               // It is DSN Doppler
   {
      retval = new DopplerAdapter(adapterName);
      if (retval)
      {
          ((DopplerAdapter*)retval)->adapterS = (RangeAdapterKm*)BuildAdapter(strand, sensors, "Range", configIndex);
          retval->UsesLightTime(useLighttime);
          retval->SetStringParameter("MeasurementType", type);
      }
   }                                                                  
   else if (type == "RangeRate")
   {
      retval = new GNDopplerAdapter(adapterName);
      if (retval)
      {
         ((GNDopplerAdapter*)retval)->adapterS = (GNRangeAdapter*)BuildAdapter(strand, sensors, "Range", configIndex);
      }
   }
   else if (type == "SN_Doppler")                            // It is 2-ways TDRS Doppler
   {
      retval = new TDRSDopplerAdapter(adapterName);
      if (retval)
      {
         // Add code to specify short strand here
         StringArray shortStrand;
         StringArray shortSensors;
         if (strand.size() != 5)
            throw MeasurementException("Error: Signal path does not contain 5 participants.\n");
         shortStrand.push_back(strand[0]);
         shortStrand.push_back(strand[3]);
         shortStrand.push_back(strand[4]);
         shortSensors.push_back(sensors[0]);
         shortSensors.push_back(sensors[3]);
         shortSensors.push_back(sensors[4]);
         ((TDRSDopplerAdapter*)retval)->adapterES = (RangeAdapterKm*)BuildAdapter(shortStrand, shortSensors, "Range", configIndex);
         ((TDRSDopplerAdapter*)retval)->adapterSL = (RangeAdapterKm*)BuildAdapter(strand, sensors, "Range", configIndex);
         ((TDRSDopplerAdapter*)retval)->adapterSS = (RangeAdapterKm*)BuildAdapter(shortStrand, shortSensors, "Range", configIndex);

         // clean up 
         shortStrand.clear();
         shortSensors.clear();
      }
   }
   else if (type == "SN_Doppler_Rtn")                            // It is TDRS 3L Return Doppler
   {
      retval = new TDRS3LReturnDopplerAdapter(adapterName);
      if (retval)
      {
         if (strand.size() == 3)
         {
            StringArray scPathStrand;
            StringArray scPathSensors;
            StringArray gsPathStrand;
            StringArray gsPathSensors;

            scPathStrand.push_back(strand[0]);
            scPathStrand.push_back(strand[1]);
            scPathStrand.push_back(strand[2]);
            scPathSensors.push_back(sensors[0]);
            scPathSensors.push_back(sensors[1]);
            scPathSensors.push_back(sensors[2]);

            gsPathStrand.push_back(strand[2]);
            gsPathStrand.push_back(strand[1]);
            gsPathStrand.push_back(strand[2]);
            gsPathSensors.push_back(sensors[2]);
            gsPathSensors.push_back(sensors[1]);
            gsPathSensors.push_back(sensors[2]);
            ((TDRS3LReturnDopplerAdapter*)retval)->adapterESC = (RangeAdapterKm*)BuildAdapter(scPathStrand, scPathSensors, "Range", configIndex);
            ((TDRS3LReturnDopplerAdapter*)retval)->adapterEGS = (RangeAdapterKm*)BuildAdapter(gsPathStrand, gsPathSensors, "Range", configIndex);
            ((TDRS3LReturnDopplerAdapter*)retval)->adapterSSC = (RangeAdapterKm*)BuildAdapter(scPathStrand, scPathSensors, "Range", configIndex);
            ((TDRS3LReturnDopplerAdapter*)retval)->adapterSGS = (RangeAdapterKm*)BuildAdapter(gsPathStrand, gsPathSensors, "Range", configIndex);

            // clean up 
            scPathStrand.clear();
            scPathSensors.clear();
            gsPathStrand.clear();
            gsPathSensors.clear();

         }
         else
            throw MeasurementException("Tracking config: \n\"" +  trackingConfigs[configIndex].GetDefinitionString() + "\" \nfor SN_Doppler_Rtn tracking must contain exactly three participants.\n");
      }
   }
   else if (type == "SN_DOWD")                            // It is TDRS DOWD
   {
      retval = new TDRSDOWDAdapter(adapterName);
      if (retval)
      {
         if (strand.size() == 5)
         {
            StringArray compTDRSPathStrand;
            StringArray compTDRSPathSensors;
            StringArray refTDRSPathStrand;
            StringArray refTDRSPathSensors;

            refTDRSPathStrand.push_back(strand[0]);
            refTDRSPathStrand.push_back(strand[1]);
            refTDRSPathStrand.push_back(strand[2]);
            refTDRSPathSensors.push_back(sensors[0]);
            refTDRSPathSensors.push_back(sensors[1]);
            refTDRSPathSensors.push_back(sensors[2]);

            compTDRSPathStrand.push_back(strand[0]);
            compTDRSPathStrand.push_back(strand[3]);
            compTDRSPathStrand.push_back(strand[4]);
            compTDRSPathSensors.push_back(sensors[0]);
            compTDRSPathSensors.push_back(sensors[3]);
            compTDRSPathSensors.push_back(sensors[4]);
            ((TDRSDOWDAdapter*)retval)->adapterTDRSComp = (TDRS3LReturnDopplerAdapter*)BuildAdapter(compTDRSPathStrand, compTDRSPathSensors, "SN_Doppler_Rtn", configIndex);
            ((TDRSDOWDAdapter*)retval)->adapterTDRSRef = (TDRS3LReturnDopplerAdapter*)BuildAdapter(refTDRSPathStrand, refTDRSPathSensors, "SN_Doppler_Rtn", configIndex);

            // clean up 
            compTDRSPathStrand.clear();
            compTDRSPathSensors.clear();
            refTDRSPathStrand.clear();
            refTDRSPathSensors.clear();

         }
         else
            throw MeasurementException("Error: Tracking config does not contain 5 participants.\n");
      }
   }
   else if (type == "BRTS_Doppler")                // It is 2-ways BRTS Doppler
   {
      retval = new BRTSDopplerAdapter(adapterName);
      if (retval)
      {

         // Add code to specify short strand here
         StringArray shortStrand;
         StringArray shortSensors;
         if (strand.size() != 5)
            throw MeasurementException("Error: Signal path does not contain 5 participants.\n");
         shortStrand.push_back(strand[0]);
         shortStrand.push_back(strand[3]);
         shortStrand.push_back(strand[4]);
         shortSensors.push_back(sensors[0]);
         shortSensors.push_back(sensors[3]);
         shortSensors.push_back(sensors[4]);
         ((BRTSDopplerAdapter*)retval)->adapterES = (RangeAdapterKm*)BuildAdapter(shortStrand, shortSensors, "Range", configIndex);
         ((BRTSDopplerAdapter*)retval)->adapterSL = (BRTSRangeAdapter*)BuildAdapter(strand, sensors, "Range", configIndex);
         ((BRTSDopplerAdapter*)retval)->adapterSS = (RangeAdapterKm*)BuildAdapter(shortStrand, shortSensors, "Range", configIndex);

         // clean up
         shortStrand.clear();
         shortSensors.clear();
      }
   }
   /// Calcalation of GN Doppler (km/s) is in GNRangeAdapter class. RangeRateAdapterKps is no longer in use
   //else if (type == "RangeRate")
   //{                                                                  
   //   retval = new RangeRateAdapterKps(instanceName + type);              
   //   if (retval)                                                     
   //   {
   //      retval->UsesLightTime(useLighttime);                         
   //      retval->SetStringParameter("MeasurementType", type);       
   //   }
   //}
   else if (type == "PointRangeRate")
   {
      retval = new PointRangeRateAdapterKps(instanceName + type);
   }
   else if (type == "GPS_PosVec")
   {
      retval = new GPSAdapter(instanceName + type);
      if (retval)
      {
         ((GPSAdapter*)retval)->SetGPSReceiverName(strand[0] + "." + sensors[0]);
      }
   }
   else if (type == "Azimuth")                                // It is Azimuth angle
   {
      retval = new AzimuthAdapter(adapterName);
   }
   else if (type == "Elevation")                              // It is Elevation angle
   {
      retval = new ElevationAdapter(adapterName);
   }
   else if (type == "XEast")                                  // It is XEast angle
   {
      retval = new XEastAdapter(adapterName);
   }
   else if (type == "YNorth")                                 // It is YNorth angle
   {
      retval = new YNorthAdapter(adapterName);
   }
   else if (type == "XSouth")                                 // It is XSouth angle
   {
      retval = new XSouthAdapter(adapterName);
   }
   else if (type == "YEast")                                  // It is YEast angle
   {
      retval = new YEastAdapter(adapterName);
   }
   else if (type == "RightAscension")                         // It is RightAscension angle
   {
      retval = new RightAscAdapter(adapterName);
   }
   else if (type == "Declination")                            // It is Declination angle
   {
      retval = new DeclinationAdapter(adapterName);
   }
   else                                                               
      throw MeasurementException("Error: '"+ type +"' measurement type was "
            "not implemented in this version of EstimationPlugin.\n");
   
   if (retval)
   {
       retval->UsesLightTime(useLighttime);
       retval->SetStringParameter("MeasurementType", type);
   }


   if (retval)
   {
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
      //MeasureModel *mm = new MeasureModel(instanceName + type + "Measurement");

      MeasureModel *mm;
      if (type == "GPS_PosVec")
         mm = (MeasureModel*)(new GPSPointMeasureModel(adapterName + "Measurement"));
      else
         mm = new MeasureModel(adapterName + "Measurement", passiveMeasureModel);

      retval->SetMeasurement(mm);
      Integer magicNumber = mn->GetMagicNumber(nodelist, type);
      Real multiplier = mn->GetMNMultiplier(magicNumber);
      retval->SetModelTypeID(magicNumber, type, multiplier);
      retval->SetIonosphereCache(&ionosphereCache);

      // Pass in the signal paths
      std::string theStrand;
      for (UnsignedInt k = 0; k < strand.size(); ++k)
      {
         if (k > 0)
            theStrand += ", ";
         theStrand += strand[k];
      }
      retval->SetStringParameter("SignalPath", theStrand);
      //MessageInterface::ShowMessage("Add signal path <%s> to %s adapter\n", theStrand.c_str(), retval->GetName().c_str());
   }

   // clean up
   for (Integer i = 0; i < nodelist.size(); ++i)
      nodelist[i].clear();
   nodelist.clear();
   currentStrand.clear();
   designators.clear();
   designator.clear();

   //Warning for 3-way not using ET-TAI
   if (useETminusTAICorrection == false)
   {
       if (warnThreeWayETTAI && !isOneWay)
       {
           if (strand.size() == 3 )
           {
               //Three way as recieve station != transmit station
               if (strand[0] != strand[2])
               {
                   std::string warn = "WARNING: Tracking file set ";
                   warn += instanceName;
                   warn += " includes three-way tracking strands. It is strongly recommended that UseETminusTAI be set to True for this tracking file set.\n";
                   MessageInterface::ShowMessage(warn);
                   warnThreeWayETTAI = false;
               }
           }
       }
       if (warnOneWayETTAI && isOneWay)
       {
          //Three way as recieve station != transmit station
          std::string warn = "WARNING: Tracking file set ";
          warn += instanceName;
          warn += " includes one-way tracking strands. It is strongly recommended that UseETminusTAI be set to True for this tracking file set.\n";
          MessageInterface::ShowMessage(warn);
          warnOneWayETTAI = false;
       }
   }

#ifdef DEBUG_BUILD_ADAPTER
   MessageInterface::ShowMessage("TrackingFileSet::BuildAdapter(, type = <%s>, configIndex = %d)   exit\n", type.c_str(), configIndex);
#endif

   return retval;
}


bool TrackingFileSet::GenerateTrackingConfigs(std::vector<StringArray> strandsList, std::vector<StringArray> sensorsList, StringArray typesList)
{
   if (strandsList.empty())
      return true;

   // Generate a list of tracking configs
   MessageInterface::ShowMessage("Total of %d tracking configurations are generated for tracking file set %s:\n", strandsList.size(), GetName().c_str());
   for(UnsignedInt i = 0; i < strandsList.size(); ++i)
   {
      MeasurementDefinition md;
      md.SetDefinitionString(strandsList[i], sensorsList[i], typesList[i]);
      trackingConfigs.push_back(md);
      trackingConfigs[i].GetDefinitionString();
      
      std::string measType = typesList[i];
      std::string trackingConfigLog = md.GetDefinitionString();
      
      // if non-DSN and ramped table supplied then it will be ignored
      if ((measType != "DSN_SeqRange" && measType != "DSN_TCP" && measType != "DSN_PNRange") && rampedTablenames.size() > 0) {
         trackingConfigLog += " (Ramp table will be ignored for this strand)";
      }

      MessageInterface::ShowMessage("   Tracking config %d: %s\n", i, trackingConfigLog.c_str());
   }

   isInitialized = false;
   
   return Initialize();
}

//------------------------------------------------------------------------------
// ClearIonosphereCache()
//------------------------------------------------------------------------------
/**
 * Clears the ionosphere cache
 */
 //------------------------------------------------------------------------------
void TrackingFileSet::ClearIonosphereCache()
{
   ionosphereCache.clear();
}


//------------------------------------------------------------------------------
// TrackingFileSet::UsesHorp()
//------------------------------------------------------------------------------
/**
 * Returns bool useHorp
 */
 //------------------------------------------------------------------------------
 bool TrackingFileSet::UsesHorp()
 {
    return useHorp;
 }


// Internal class definition

//------------------------------------------------------------------------------
// TrackingFileSet::MeasurementDefinition::MeasurementDefinition()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::MeasurementDefinition()
{
}


//------------------------------------------------------------------------------
// MeasurementDefinition::~MeasurementDefinition()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::~MeasurementDefinition()
{
   for (UnsignedInt i = 0; i < strands.size(); ++i)
      strands[i].clear();
   strands.clear();

   for (UnsignedInt i = 0; i < sensors.size(); ++i)
      sensors[i].clear();
   sensors.clear();
   
   types.clear();
}

//------------------------------------------------------------------------------
// MeasurementDefinition::MeasurementDefinition(const MeasurementDefinition& md)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param md The definition copied to this one
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::MeasurementDefinition(
      const MeasurementDefinition& md) :
            types       (md.types)
{
   for (UnsignedInt i = 0; i < md.strands.size(); ++i)
   {
      strands.push_back(md.strands[i]);
   }

   for (UnsignedInt i = 0; i < md.sensors.size(); ++i)
   {
      sensors.push_back(md.sensors[i]);
   }
}


//------------------------------------------------------------------------------
// MeasurementDefinition& MeasurementDefinition::operator=(
//       const MeasurementDefinition& md)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param md The definition copied into this one
 *
 * @return This MeasurementDefinition, set to match md
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition&
      TrackingFileSet::MeasurementDefinition::operator=(
            const MeasurementDefinition& md)
{
   if (this != &md)
   {
      types = md.types;

      strands.clear();
      for (UnsignedInt i = 0; i < md.strands.size(); ++i)
      {
         strands.push_back(md.strands[i]);
      }

      sensors.clear();
      for (UnsignedInt i = 0; i < md.sensors.size(); ++i)
      {
         strands.push_back(md.sensors[i]);
      }
   }

   return *this;
}


bool TrackingFileSet::MeasurementDefinition::SetDefinitionString(StringArray strand, StringArray sens, std::string measType)
{
   strands.push_back(strand);
   sensors.push_back(sens);
   types.push_back(measType);

   return true;
}


//------------------------------------------------------------------------------
// std::string MeasurementDefinition::GetDefinitionString() const
//------------------------------------------------------------------------------
/**
 * Generates the string describing a measurement definition
 *
 * @return The string, as it could appear in script
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::MeasurementDefinition::GetDefinitionString() const
{
   std::string configstring = "{";
   for (UnsignedInt i = 0; i < strands.size(); ++i)
   {
      if (i > 0)
         configstring += ",";
      configstring += "{";
      for (UnsignedInt j = 0; j < strands[i].size(); ++j)
      {
         if (j > 0)
            configstring += ",";
         configstring += strands[i][j];

         if (sensors[i][j] != "")
            configstring += ("." + sensors[i][j]);
      }
      configstring += "}";
   }
   
   for (UnsignedInt i = 0; i < types.size(); ++i)
   {
      configstring += ",";
      configstring += types[i];
   }
   configstring += "}";
   
   return configstring;
}

//------------------------------------------------------------------------------
// Real TrackingFileSet::GetTransmitterFrequency(Integer measurementNumber)
//------------------------------------------------------------------------------
/**
 * Gets the transmit frequency from the spacecraft (one-way) or ground station (two-way)
 * from the transmitting object in the measurement list or returns a default value
 * Also pulls the frequency band from the same transmitter
 *
 * @param measurementNumber The index of the desired measurement from the measurements list
 *
 * @return Real Frequency in MHz
 */
 //------------------------------------------------------------------------------
Real TrackingFileSet::GetTransmitterFrequency(Integer measurementNumber, std::string measType)
{
   std::string transmitterName;
   int referenceIndex = -1;

   // 2000.0 MHz is default
   Real returnFrequency = 2000.0;
   std::string band = "None";
   tdrsMeasurementFrequencies[measurementNumber] = returnFrequency;
   tdrsMeasurementFrequencyBands[measurementNumber] = band;

   int measSize = measurements[measurementNumber]->GetParticipants(0).size();
   if (measSize == 0)
      return returnFrequency; //returns default if participant list is empty

   if ((measType == "SN_Doppler") || (measType == "BRTS_Doppler") || (measType == "SN_DOWD") || (measType == "SN_Doppler_Rtn"))
   {
      if((measType == "SN_DOWD") || (measType == "SN_Doppler_Rtn"))
         transmitterName = measurements[measurementNumber]->GetParticipants(0)[0];
      else if (measSize >= 3)
      {
         transmitterName = measurements[measurementNumber]->GetParticipants(0)[measSize - 3];
      }
      for (int i = 0; i < references.size(); i++)
      {
         if (references[i]->GetName() == transmitterName)
         {
            if (measType == "BRTS_Doppler")
            {
               referenceIndex = i;
               break;
            }
            else if (references[i]->IsOfType(Gmat::SPACECRAFT))
            {
               referenceIndex = i;
               break;
            }
         }
      }
   }
   else
      throw MeasurementException("Unsupported measurement type for GetTransmitterFrequency(). Supported types are SN_Doppler, SN_Doppler_Rtn, BRTS_Doppler, and SN_DOWD.\n");

   if (referenceIndex == -1)
      return returnFrequency; //returns default if no transmitter is found

   //Finds transmitter on the object in measurement list
   for (int i = 0; i < references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE).size(); i++)
   {
      if (references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE)[i]->IsOfType("Transmitter"))
      {
         Integer id = references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE)[i]->GetParameterID("Frequency");
         Integer bandId = references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE)[i]->GetParameterID("FrequencyBand");
         returnFrequency = references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE)[i]->GetRealParameter(id);
         band = references[referenceIndex]->GetRefObjectArray(Gmat::HARDWARE)[i]->GetStringParameter(bandId);
         break;
      }
   }

   tdrsMeasurementFrequencies[measurementNumber] = returnFrequency;
   tdrsMeasurementFrequencyBands[measurementNumber] = band;
   return returnFrequency;
}

//------------------------------------------------------------------------------
// Integer GetFrequencyBand(Integer measurementNumber)
//------------------------------------------------------------------------------
/**
 * Get frequency band corresponding to a given frequency for a given measurement
 *
 * @param measurementNumber  index of the desired measurement in measurements list
 *
 * @return an integer corresponding to frequency band
 */
 //------------------------------------------------------------------------------
Integer TrackingFileSet::GetFrequencyBand(Integer measurementNumber)
{
   if (tdrsMeasurementFrequencies.size() > measurementNumber)
   {
      if (tdrsMeasurementFrequencies[measurementNumber] == 0.0)
      {
         std::string measType = measurements[measurementNumber]->GetStringParameter("MeasurementType");
         GetTransmitterFrequency(measurementNumber, measType);
      }
   }
   else
      throw MeasurementException("TrackingFileSet has not been initialized, initialize TrackingFileSet first.");

   Integer returnBand = ConvertBandStringToInt(tdrsMeasurementFrequencyBands[measurementNumber]);
   
   if(returnBand == 0)
      return FrequencyBand(tdrsMeasurementFrequencies[measurementNumber] * 1.0e6);

   return returnBand; 
   
}


//------------------------------------------------------------------------------
// Integer ConvertBandStringToInt(std::string frequencyBand)
//------------------------------------------------------------------------------
/**
 * Converts the frequency band from a string to the internal integer number.
 *
 * @param frequencyBand  string representation of the band
 *
 * @return an integer corresponding to frequency band
 */
 //------------------------------------------------------------------------------
Integer TrackingFileSet::ConvertBandStringToInt(std::string frequencyBand)
{

   //Currently only S, C, X, and K bands are supported
   Integer returnBand = 0;
   
   if (frequencyBand == "None")
      returnBand = 0;
   else if (frequencyBand == "S") 
      returnBand = 1;
   else if (frequencyBand == "X")
      returnBand = 2;
   else if (frequencyBand == "K")
      returnBand = 3;
   else if (frequencyBand == "C")
      returnBand = 4;
   
   return returnBand;
}


//------------------------------------------------------------------------------
// Integer FrequencyBand(Real frequency)
//------------------------------------------------------------------------------
/**
 * Get frequency band corresponding to a given frequency
 *
 * @param frequency   frequency in Hz
 *
 * @return an integer corresponding to frequency band
 */
 //------------------------------------------------------------------------------
Integer TrackingFileSet::FrequencyBand(Real frequency)
{
   Integer freqBand = 0;

   // S-band
   if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      freqBand = 1;               // 1 for S-band

   // X-band 
   if ((frequency >= 8000000000.0) && (frequency <= 12000000000.0))
      freqBand = 2;               // 2 for X-band

   // K-Band
   if ((frequency >= 12000000000.0) && (frequency <= 40000000000.0))
      freqBand = 3;               // 3 for K-band

   // C-band
   if ((frequency > 4000000000.0) && (frequency < 8000000000.0))
      freqBand = 4;

   if (freqBand == 0)
   {
      std::stringstream strs;
      strs << "Error in TrackingFileSet::FrequencyBand():  GMAT cannot specify frequency band for frequency = " << frequency << " Hz\n";
      throw MeasurementException(strs.str());
   }

   return freqBand;
}