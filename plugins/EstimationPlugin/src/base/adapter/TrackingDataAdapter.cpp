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

#include "TrackingDataAdapter.hpp"
#include "MeasurementException.hpp"
#include "TextParser.hpp"
#include "MessageInterface.hpp"
#include "PropSetup.hpp"
#include "SignalBase.hpp"
#include "ErrorModel.hpp" 
#include "GroundstationInterface.hpp"
#include "Spacecraft.hpp"
#include "Receiver.hpp"
#include <sstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_TAYLOR_SERIES


//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
TrackingDataAdapter::PARAMETER_TEXT[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   "SignalPath",
   "ObservationData",
   "RampTables",
   "MeasurementType",
   "AddNoise",
   "UplinkFrequency",
   "UplinkBand",
   "TimeGapForPassBreak",
};


const Gmat::ParameterType
TrackingDataAdapter::PARAMETER_TYPE[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,          // SIGNAL_PATH
   Gmat::OBJECTARRAY_TYPE,          // OBS_DATA
   Gmat::OBJECTARRAY_TYPE,          // RAMPED_TABLE
   Gmat::STRING_TYPE,               // MEASUREMENT_TYPE
   Gmat::BOOLEAN_TYPE,              // ADD_NOISE
   Gmat::REAL_TYPE,                 // UPLINK_FREQUENCY
   Gmat::INTEGER_TYPE,              // UPLINK_BAND
   Gmat::REAL_TYPE,                 // TIMEGAP_FOR_PASSBREAK
};


//------------------------------------------------------------------------------
// TrackingDataAdapter(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr The string type of the adapter
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::TrackingDataAdapter(const std::string &typeStr,
      const std::string &name) :
   MeasurementModelBase (name, typeStr),
   measurementType      (""),
   calcData             (NULL),
   navLog               (NULL),
   logLevel             (0),
   solarsys             (NULL),
   modelID              (-1),
   modelTypeID          (-1),
   modelType            ("UnknownType"),
   multiplier           (1.0),
   withLighttime        (false),

   timeGapForPassBreak  (1.0e70),
   
   useHorp              (false),
   horpHeight           (500.0),
   horpAngle            (70.0),
   horpOblateness       (false),
   thePropagators       (NULL),
   satPropagatorMap     (NULL),
   uplinkFreq           (1.0e3),         // unit: Mhz
   uplinkFreqAtRecei    (1.0e3),         // unit: Mhz
   freqBand             (1),
   obsData              (NULL),
   addNoise             (false),
   addBias              (true),
   rangeOnly            (false),
   rampTB               (NULL),
   beginIndex           (0),
   endIndex             (0),
   withMediaCorrection  (true),
   errMsg               (""),
   ionosphereCache      (NULL),
   measParticipantIndex (-1),
   measErrorModel       (NULL),
   biasType             (BIASTYPE_IS_UNDEFINED)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter default constructor <%p>\n", this);
#endif
   
   parameterCount = AdapterParamCount;
}


//------------------------------------------------------------------------------
// ~TrackingDataAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::~TrackingDataAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  <%p>\n", this);
#endif

   dimNames.clear();
   noiseSigma.clear();
   measurementBias.clear();
   rampTableNames.clear();
   
   //clean up ProgressReporter *navLog;
   navLog = NULL;
   
   // clean up cMeasurement.CleanUp();              // This step does not need

   // clean up std::vector<RealArray>    theDataDerivatives;
   for (Integer i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();
   
   // clean up std::vector<StringArray*> participantLists;
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      if (participantLists[i])
      {
         participantLists[i]->clear();
         delete participantLists[i];
      }
   }
   participantLists.clear();

   // clean up MeasureModel *calcData;
   if (calcData)
      delete calcData;

   // clean up ObjectArray               refObjects;
   refObjects.clear();

   // clean up ObservationData*          obsData;
   obsData = NULL;

   // clean up std::vector<RampTableData>*rampTB;
   // rampTB were not created inside this class. Therefore, we cannot clean it up here. 
   //if (rampTB)
   //{
   //   for (Integer i = 0; i < rampTB->size(); ++i)
   //   {
   //      (*rampTB)[i].Clear();
   //   }
   //   rampTB->clear();
   //}
   rampTB = NULL;

   // clean up ObjectArray forObjects;
   forObjects.clear();
   
}


//------------------------------------------------------------------------------
// TrackingDataAdapter(const TrackingDataAdapter& ma)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ma The adapter copied to make this new one
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::TrackingDataAdapter(const TrackingDataAdapter& ma) :
   MeasurementModelBase (ma),
   measurementType      (ma.measurementType),
   dimNames             (ma.dimNames),
   participantLists     (ma.participantLists),
   calcData             (NULL),
   navLog               (ma.navLog),
   logLevel             (ma.logLevel),
   solarsys             (ma.solarsys),
   modelID              (ma.modelID),     // Should this be -1?
   modelTypeID          (ma.modelTypeID),
   modelType            (ma.modelType),
   multiplier           (ma.multiplier),
   withLighttime        (ma.withLighttime),
   timeGapForPassBreak  (ma.timeGapForPassBreak),

   thePropagators       (NULL),
   satPropagatorMap     (NULL),
   uplinkFreq           (ma.uplinkFreq),
   uplinkFreqAtRecei    (ma.uplinkFreqAtRecei),
   freqBand             (ma.freqBand),
   obsData              (ma.obsData),
   addNoise             (ma.addNoise),
   addBias              (ma.addBias),
   rangeOnly            (ma.rangeOnly),
   rampTB               (ma.rampTB),
   beginIndex           (ma.beginIndex),
   endIndex             (ma.endIndex),
   rampTableNames       (ma.rampTableNames),
   forObjects           (ma.forObjects),
   withMediaCorrection  (ma.withMediaCorrection),
   errMsg               (ma.errMsg),
   ionosphereCache      (NULL),
   measParticipantIndex (ma.measParticipantIndex),
   measErrorModel       (NULL),
   biasType             (BIASTYPE_IS_UNDEFINED)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter copy constructor  from <%p> to <%p>\n", &ma, this);
#endif
   isInitialized = false;
}


//------------------------------------------------------------------------------
// TrackingDataAdapter& operator=(const TrackingDataAdapter& ma)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ma The adapter copied into this one
 *
 * @return This adapter, configured to match ma
 */
//------------------------------------------------------------------------------
TrackingDataAdapter& TrackingDataAdapter::operator=(
      const TrackingDataAdapter& ma)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter operator =  set <%p> = <%p>\n", this, &ma);
#endif

   if (this != &ma)
   {
      MeasurementModelBase::operator=(ma);

      measurementType    = ma.measurementType;
      dimNames           = ma.dimNames;
      navLog             = ma.navLog;
      logLevel           = ma.logLevel;
      solarsys           = ma.solarsys;
      modelID            = ma.modelID;          // Should this be -1?
      modelTypeID        = ma.modelTypeID;
      modelType          = ma.modelType;
      multiplier         = ma.multiplier;
      withLighttime      = ma.withLighttime;

      timeGapForPassBreak= ma.timeGapForPassBreak;

      uplinkFreq         = ma.uplinkFreq;
      uplinkFreqAtRecei  = ma.uplinkFreqAtRecei;
      freqBand           = ma.freqBand;
      obsData            = ma.obsData;
      addNoise           = ma.addNoise;
      addBias            = ma.addBias;
      rangeOnly          = ma.rangeOnly;
      rampTB             = ma.rampTB;
      beginIndex         = ma.beginIndex;
      endIndex           = ma.endIndex;
      rampTableNames     = ma.rampTableNames;
      forObjects         = ma.forObjects;
      withMediaCorrection = ma.withMediaCorrection;
      errMsg             = ma.errMsg;
      
      measParticipantIndex = ma.measParticipantIndex;
      
      measErrorModel     = ma.measErrorModel;
      biasType           = ma.biasType;

      // We require that these be set after a copy
      thePropagators     = NULL;
      satPropagatorMap   = NULL;

      if (calcData)
      {
         delete calcData;
         calcData = NULL;
      }
      if (ma.calcData)
         calcData = (MeasureModel*)ma.calcData->Clone();

      refObjects.clear();
      isInitialized = false;
   }

   ionosphereCache = NULL;

   return *this;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The pointer
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetSolarSystem(SolarSystem *ss)
{
   solarsys = ss;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetParameterText(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < AdapterParamCount)
      return PARAMETER_TEXT[id - MeasurementModelBaseParamCount];
   return MeasurementModelBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = MeasurementModelBaseParamCount; i < AdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - MeasurementModelBaseParamCount])
         return i;
   }
   return MeasurementModelBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingDataAdapter::GetParameterType(
      const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < AdapterParamCount)
      return PARAMETER_TYPE[id - MeasurementModelBaseParamCount];

   return MeasurementModelBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
Integer TrackingDataAdapter::GetIntegerParameter(const Integer id) const
{
   if (id == UPLINK_BAND)
      return freqBand;

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
Integer TrackingDataAdapter::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == UPLINK_BAND)
   {
      if (value <= 0)
         throw MeasurementException("Error: uplink frequency band has invalid value\n");

      freqBand = value;
      return freqBand;
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
Integer TrackingDataAdapter::GetIntegerParameter(const std::string &label) const
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
Integer TrackingDataAdapter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const Integer id) const
{
   if (id == UPLINK_FREQUENCY)
      return uplinkFreq;                        // unit: Mhz

   if (id == TIMEGAP_FOR_PASSBREAK)
      return timeGapForPassBreak;               // unit: second

   return MeasurementModelBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter (unit: Mhz for uplink frequency)
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == UPLINK_FREQUENCY)
   {
      if (value < 0)
         throw MeasurementException("Error: uplink frequency has a negative value\n");

      uplinkFreq = value;             // unit: Mhz
      return uplinkFreq;
   }

   if (id == TIMEGAP_FOR_PASSBREAK)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: TimeGapForPassBreak has an invalid value. It has to be a positive number\n");
      timeGapForPassBreak = value;     // unit: second
      return timeGapForPassBreak;
   }

   return MeasurementModelBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an element in a real parameter
 *
 * @param id      The ID for the parameter
 * @param index   The index of an element in a real array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const Integer id, const Integer index) const
{
   return MeasurementModelBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for an element in a real array parameter
 *
 * @param id      The ID for the parameter
 * @param value   The value for the parameter
 * @oaram index   The index of an element in a real array
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   return MeasurementModelBase::SetRealParameter(id, value, index);
}

   
//------------------------------------------------------------------------------
// std::string GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const std::string & label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a Real parameter in an array.
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 * @param index The index of the parameter
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const std::string & label,
      const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets a bool parameter.
 *
 * @param id      The ID for the parameter
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::GetBooleanParameter(const Integer id) const
{
   if (id == ADD_NOISE)
      return addNoise;

   return MeasurementModelBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets value to a bool parameter.
 *
 * @param id      The ID for the parameter
 * @param value   bool value used to set
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == ADD_NOISE)
   {
      addNoise = value;
      return addNoise;
   }

   return MeasurementModelBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method gets a bool parameter.
 *
 * @param label   name of the parameter
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets value to a bool parameter.
 *
 * @param label   name of the parameter
 * @param value   bool value used to set
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const Integer id) const
{
   if (id == MEASUREMENT_TYPE)
      return measurementType;

   return MeasurementModelBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == MEASUREMENT_TYPE)
   {
      measurementType = value;
      return true;
   }

   if (id == SIGNAL_PATH)
   {
      StringArray *partList = DecomposePathString(value);
      if (partList->size() > 0)
      {
         participantLists.push_back(partList);

         // Make sure the measurement model has been set
         if (!calcData)
            throw MeasurementException("Unable to set parameter data on the "
            "measurement because the associated model has not yet been set.");

         // Pass the ordered participant lists to the measurement model
         Integer whichOne = participantLists.size() - 1;
         for (UnsignedInt i = 0; i < partList->size(); ++i)
            calcData->SetStringParameter("SignalPath", partList->at(i),
                  whichOne);

         return true;
      }

      // The list is empty
      delete partList;
      return false;
   }

   return MeasurementModelBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The ID for the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == RAMPTABLES)
   {
      if ((0 <= index)&&(index < rampTableNames.size()))
         return rampTableNames[index];
      else
      {
         std::stringstream ss;
         ss << "Error: index (" << index << ") is out of bound (" << rampTableNames.size() << ")\n";
         throw MeasurementException(ss.str());
      }
   }

   return MeasurementModelBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if (id == RAMPTABLES)
   {
      if (value == "")
         throw MeasurementException("Error: Name of ramped frequency table is an empty string\n");

      if ((0 <= index)&&(index < rampTableNames.size()))
         rampTableNames[index] = value;
      else
         rampTableNames.push_back(value);
      return true;
   }

   return MeasurementModelBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const Integer id) const
{
   if (id == RAMPTABLES)
      return rampTableNames;

   return MeasurementModelBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param id The ID for the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const Integer id, const Integer index) const
{
   if (id == SIGNAL_PATH)
   {
      if ((index >= 0) && ((Integer)participantLists.size() > index))
         return *(participantLists[index]);
      throw MeasurementException("Index out of bounds when accessing the "
            "signal path");
   }

   return MeasurementModelBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The scriptable name of the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param label The scriptable name of the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}



//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Adjusts names for objects referenced by this one when they are renamed
 *
 * @param type The type of the object that is renamed
 * @param oldName The name of the object before it was changed
 * @param newName The new name for the object
 *
 * @return true if a reference name was changed, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;

   // Build the list
   if (calcData)
   {
      retval = calcData->RenameRefObject(type, oldName, newName);
   }

   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of the reference objects used in the model
 *
 * @param type The type of object requested (not used)
 *
 * @return The list of names of reference objects
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();

   // Build the list
   if (calcData)
   {
      refObjectNames = calcData->GetRefObjectNameArray(type);
   }

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets pointers to the model's reference objects
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   bool retval = false;

   if (calcData)
   {
      retval = calcData->SetRefObject(obj, type, name);
   }

   if (find(refObjects.begin(), refObjects.end(), obj) == refObjects.end())
   {
      refObjects.push_back(obj);
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the pointers for the reference object
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 * @param index Index for the object's location
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   bool retval = false;

   if (calcData)
   {
      retval = calcData->SetRefObject(obj, type, name, index);
   }

   if ((index >= 0) && (index < (Integer)refObjects.size()))
   {
      refObjects[index] = obj;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetMeasurement(MeasureModel* meas)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model pointer
 *
 * @param meas The pointer
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetMeasurement(MeasureModel* meas)
{
   bool retval = false;

   if (meas)
   {
      calcData = meas;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& GetMeasurement()
//------------------------------------------------------------------------------
/**
 * Retrieves computed measurement data
 *
 * @return The measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& TrackingDataAdapter::GetMeasurement()
{
   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");

   return cMeasurement;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write the measurement data
 *
 * @todo: Implement this method
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::WriteMeasurements()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a single measurement
 *
 * @param id ID for the measurement that is written
 *
 * @todo: Implement this method
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::WriteMeasurement(Integer id)
{
   bool retval = false;

   return retval;
}

//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Checks to see if adapter has covariance data for a specified parameter ID
 *
 * @param paramID The parameter ID
 *
 * @return Size of the covariance data that is available
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::HasParameterCovariances(Integer parameterId)
{
   return 0;
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of light time solution events in the measurement
 *
 * @return The event count
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetEventCount()
{
   return 0;
}


//------------------------------------------------------------------------------
// void SetPropagators(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Passes a propagator to the adapter for use in light time iterations.  The
 * propagator is cloned so that propagation of single spacecraft can be
 * performed.
 *
 * @param ps The PropSetup that is being set
 *
 * @todo The current call takes a single propagator.  Once the estimation system
 *       supports multiple propagators, this should be changed to a vector of
 *       PropSetup objects.
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetPropagator(ps = <%p>)\n", this, ps);
   #endif

   thePropagators = ps;
   satPropagatorMap = spMap;
   if (ps != NULL)
      calcData->SetPropagators(thePropagators, satPropagatorMap);
   else
      thePropagators = NULL;
}


//------------------------------------------------------------------------------
// MeasureModel* GetMeasurementModel()
//------------------------------------------------------------------------------
/**
 * Get MeasureModel object for this adapter
 */
//------------------------------------------------------------------------------
MeasureModel* TrackingDataAdapter::GetMeasurementModel()
{
   return calcData;
}


//------------------------------------------------------------------------------
// StringArray GetParticipants(Integer forPathIndex)
//------------------------------------------------------------------------------
/**
* Get a name list of participants for a signal path
*
* @param forPathIndex         Index of a given path
* @return                     a name list of participants in a given signal path
*
*/
//------------------------------------------------------------------------------
StringArray TrackingDataAdapter::GetParticipants(Integer forPathIndex)
{
   StringArray list = *(participantLists[0]);
   return list;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start Initializing a Tracking Data Adapter <%p>\n", this);
   #endif

   bool retval = false;

   if (MeasurementModelBase::Initialize())
   {
      if (calcData == NULL)
         throw MeasurementException("TrackingDataAdapter Initialization failed "
               "because the measurement model is not yet set");

      if (thePropagators)
         calcData->SetPropagators(thePropagators, satPropagatorMap);

      calcData->SetSolarSystem(solarsys);
      calcData->SetProgressReporter(navLog);
      calcData->UsesLightTime(withLighttime);

      retval = calcData->Initialize();

      calcData->UseIonosphereCache(ionosphereCache);

      if (useHorp)
      {
         std::vector<SignalBase*> paths = calcData->GetSignalPaths();
         for (UnsignedInt i = 0; i < paths.size(); ++i)
         {
            paths[i]->SetupHorp(horpHeight, horpAngle, horpOblateness, forceHorp);
         }
      }
   }
   
   cMeasurement.type = modelTypeID;
   cMeasurement.typeName = modelType;
   cMeasurement.uniqueID = modelID;

   if (cMeasurement.covariance == NULL)
      cMeasurement.covariance = new Covariance;

   // Default to a 1x1 identity covariance
   cMeasurement.covariance->SetDimension(1);
   (*(cMeasurement.covariance))(0,0) = 1.0;

   if (navLog != NULL)
      logLevel = navLog->GetLogLevel("Adapter");
   else
      logLevel = 32767;

   if (!retval && navLog && (logLevel == 0))
      navLog->WriteData("Error initializing the measurement adapter " +
            instanceName);

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End Initializing a Tracking Data Adapter <%p>\n", this);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the adapter
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
void TrackingDataAdapter::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    GetMeasurementModel()->SetTransientForces(tf);
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string correctionName,
//       const std::string correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @todo This method isn't yet hooked up, and might change as corrections are
 *       incorporated into the system
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   calcData->SetCorrection(correctionName, correctionType);
}


//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the reporter for the Adapter
 *
 * @param reporter The reporter that should be used
 *
 * @return true if set, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      logLevel = reporter->GetLogLevel("Adapter");
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void SetModelID(const Integer newID)
//------------------------------------------------------------------------------
/**
 * Sets the model's unique ID number
 *
 * @param newID the model ID used in the current run
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetModelID(const Integer newID)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting ModelID to %d\n", newID);
   #endif

   modelID = newID;
   cMeasurement.uniqueID = modelID;
}


//------------------------------------------------------------------------------
// Integer GetModelID()
//------------------------------------------------------------------------------
/**
 * Retrieves the unique model ID for the current run
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetModelID()
{
   return modelID;
}


//------------------------------------------------------------------------------
// Integer GetModelTypeID()
//------------------------------------------------------------------------------
/**
 * Returns the model's magic number
 *
 * @return The type ID, used to speed lookups and to identify model subtypes
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetModelTypeID()
{
   return modelTypeID;
}


//------------------------------------------------------------------------------
// std::string GetModelType()
//------------------------------------------------------------------------------
/**
 * Returns the Models Type
 *
 * @return The model type as a string
 */
 //------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetModelType()
{
   return modelType;
}

//------------------------------------------------------------------------------
// void SetModelTypeID(const Integer theID, const std::string type, Real mult)
//------------------------------------------------------------------------------
/**
 * Sets fields used for all of teh Adapters of a given type and subtype
 *
 * @param theID The measuremetn's magic number used internally and in .gmd files
 * @param type The typ descriptor for the model
 * @param mult A multiplicative factor that some models use (defaults to 1.0)
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetModelTypeID(const Integer theID,
      const std::string type, Real mult)
{
   modelTypeID = theID;
   modelType = type;
   multiplier = mult;
}

//------------------------------------------------------------------------------
// void UsesLightTime(const bool tf)
//------------------------------------------------------------------------------
/**
 * Method used to set or clear the light time solution flag
 *
 * @param tf true to find light time solutions, false to omit them
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::UsesLightTime(const bool tf)
{
   withLighttime = tf;
}

//------------------------------------------------------------------------------
// StringArray* DecomposePathString(const std::string &value)
//------------------------------------------------------------------------------
/**
 * Breaks apart a signal path string into a participant list
 *
 * @param value The string
 *
 * @return The ordered vector of participant names
 */
//------------------------------------------------------------------------------
StringArray* TrackingDataAdapter::DecomposePathString(const std::string &value)
{
   TextParser theParser;
   StringArray *partList = new StringArray;

   (*partList)  = theParser.SeparateBy(value, ", ");

   if (navLog)
   {
      if (logLevel == 0)
      {
         std::stringstream msg;
         msg << "Path string " << value << " has members:\n";
         for (UnsignedInt i = 0; i < partList->size(); ++i)
            msg << "   " + partList->at(i) + "\n";

         navLog->WriteData(msg.str());
      }
   }

   return partList;
}


//------------------------------------------------------------------------------
// void SetMultiplierFactor(Real mult)
//------------------------------------------------------------------------------
/**
 * Set value to multiplier
 *
 * @param mult     Value used to set to multiplier
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetMultiplierFactor(Real mult)
{
   multiplier = mult;
}


//------------------------------------------------------------------------------
// Real GetMultiplierFactor()
//------------------------------------------------------------------------------
/**
 * Get value of multiplier
 *
 * @return     Value of multiplier
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetMultiplierFactor()
{
   return multiplier;
}


//------------------------------------------------------------------------------
// ErrorModel* TrackingDataAdapter::GetMeasurementErrorModel(Integer measIndex, 
//                      const std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
* Get the measurement ErrorModel.
*
* @param measIndex           Index of measurement's element
* @param measType            Measurement type of this tracking data
* @param numTrip             Number of ways signal travel such as 1-way, 2-ways,
*                            or 3-ways
*/
//------------------------------------------------------------------------------
ErrorModel* TrackingDataAdapter::GetMeasurementErrorModel(Integer measIndex, const std::string measType, Integer numTrip)
{
   ErrorModel* errmodel = NULL;

   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();

   // Get first and last signal data
   SignalData* first = data[measIndex];
   SignalData* last = data[measIndex];
   if (measParticipantIndex == -1)
   {
      while (last->next != NULL)
         last = last->next;
   }
   else
   {
      // This part is used for special case when measuring participant is not at the end of participant list. 
      // Example: TDRS 3L Return Doppler with its tracking config: {{userSC, TDRS, GS2, GS1}, 'SN_Doppler_Rtn'} 
      // has measuring participant is ground station GS2 is not at the end of list {userSC, TDRS, GS2, GS1}
      Integer legIndex = 0;
      while (legIndex < measParticipantIndex - 1)
      {
         if (last != NULL)
         {
            last = last->next;
            ++legIndex;
         }
         else
            break;
      }
      if (legIndex < measParticipantIndex - 1)
         throw MeasurementException("Error: No index of measuring participant was set.\n");
      if (last == NULL)
         throw MeasurementException("Error: Measuring participant is NULL object.\n");
   }
   
   // Specify ground station
   SpacePoint* gs = NULL;            // ground station 
   GmatBase* sc = NULL;              // spacecraft being measured
   GmatBase* scSignaler = NULL;      // space performing measurement for SC to SC measurement
   if (measType == "GPS_PosVec")
   {
      // Process for 1 participant measurment
      scSignaler = first->rNode;
   }
   else
   {
      // Process for 2 or more particpants measurment 
      if ((first->tNode->IsOfType(Gmat::GROUND_STATION)) && (last->rNode->IsOfType(Gmat::GROUND_STATION) == false))
      {
         gs = first->tNode;
         sc = first->rNode;
      }
      else if (last->rNode->IsOfType(Gmat::GROUND_STATION))
      {
         gs = last->rNode;
         sc = last->tNode;
      }
      else
      {
         scSignaler = last->rNode;
         sc = last->tNode;
      }
   }
   
   //// Search for ErrorModel associated with measType, numTrip,  and spacecraft (It has to search in GroundStation::errorModelMap)
   // Search for ErrorModel associated with measType and spacecraft (It has to search in GroundStation::errorModelMap)
   if (gs)
   {
      std::map<std::string, ObjectArray> errmodelMap = ((GroundstationInterface*)gs)->GetErrorModelMap();
      for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin(); mapIndex != errmodelMap.end(); ++mapIndex)
      {
         if (sc->GetName() == mapIndex->first)
         {
            for (UnsignedInt j = 0; j < mapIndex->second.size(); ++j)
            {
               if (mapIndex->second.at(j)->GetStringParameter("Type") == measType)
               {
                  errmodel = (ErrorModel*)mapIndex->second.at(j);
                  break;
               }
            }

            if (errmodel != NULL)
               break;
         }
      }
   }
   else if (scSignaler)
   {
      ObjectArray hardwareList = ((Spacecraft*)scSignaler)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      ObjectArray errModels;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Receiver"))
         {
            errModels = ((Receiver*)hardwareList[i])->GetRefObjectArray(Gmat::ERROR_MODEL);
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << ((Spacecraft*)scSignaler)->GetName() << " does not have a receiver to receive signal\n";
         throw MeasurementException(ss.str());
      }

      for (i = 0; i < errModels.size(); ++i)
      {
         if (errModels[i]->GetStringParameter("Type") == (measType))
         {
            errmodel = (ErrorModel*)errModels[i];
            break;
         }
         if (errmodel != NULL)
            break;
      }

      // if not found, throw an error message
      if (errmodel == NULL)
      {
         std::stringstream ss;
         ss << "Error: ErrorModel mismatched. No error model with Type = '" << measType << "' was set to the Spacecraft receiver on" << ((Spacecraft*)scSignaler)->GetName() << "\n";
         throw MeasurementException(ss.str());
      }

   }
   
   // if not found, throw an error message
   if (errmodel == NULL)
   {
      std::stringstream ss;
      ss << "Error: ErrorModel mismatched. No error model with Type = '" << measType << "' was set to GroundStation " << gs->GetName() << ".ErrorModels\n";
      throw MeasurementException(ss.str());
   }
   
   // Clean up memory
   data.clear();

   return errmodel;
}


//------------------------------------------------------------------------------
// bool TrackingDataAdapter::IsCrossLink()
//------------------------------------------------------------------------------
/**
 * Returns true if the measurement is crosslink tracking, ie spacecraft to spacecraft
 *
 * @return true if the measurement is crosslink tracking, false if not
 *
 */
 //------------------------------------------------------------------------------
bool TrackingDataAdapter::IsCrossLink()
{
    return calcData->IsCrossLink();
}


//------------------------------------------------------------------------------
// void ComputeMeasurementBias(const std::string biasName, 
//                             const std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
* Get measurement bias
*
* @param biasName      Name of the measurement bias. In this case is "Bias"      
* @param measType      Measurement type of this tracking data
* @param numTrip       Number of ways signal travel such as 1-way, 2-ways, 
*                      or 3-ways 
*
* @return              Value of bias
*/
//------------------------------------------------------------------------------
void TrackingDataAdapter::ComputeMeasurementBias(const std::string biasName, const std::string measType, Integer numTrip)
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate bias for each measurement (signal path)
   measurementBias.clear();
   if (rangeOnly)
   {
      for (UnsignedInt i = 0; i < measurementSize; ++i)
         measurementBias.push_back(0.0);

      data.clear();
      return;
   }

	for (UnsignedInt i = 0; i < measurementSize; ++i)
	{
      if (measErrorModel == NULL)
         measErrorModel = GetMeasurementErrorModel(i, measType, numTrip);

      Real bias = 0.0;
      if (forObjects.size() > 0)
      {
         // This is case for running estimation: solve-for objects are stored in forObjects array
         // Search for object with the same name with errmodels[k]
         UnsignedInt j;
         for (j = 0; j < forObjects.size(); ++j)
         {
            if (forObjects[j]->GetFullName() == measErrorModel->GetFullName())
               break;
         }

         // Get Bias from that error model
         if (j >= forObjects.size())
            bias = measErrorModel->GetRealParameter(biasName);        // bias is a consider parameter
         else
            bias = forObjects[j]->GetRealParameter(biasName);         // bias is a solve-for parameter

         if (biasType == BIASTYPE_IS_UNDEFINED)
         {
            biasType = BIASTYPE_IS_NONE;
            StringArray solvefors = measErrorModel->GetStringArrayParameter("SolveFors");
            for (Integer i = 0; i < solvefors.size(); ++i)
            {
               if (solvefors[i] == "Bias")
               {
                  biasType = BIASTYPE_IS_BIAS;
                  break;
               }
               if (solvefors[i] == "PassBiases")
               {
                  biasType = BIASTYPE_IS_PASSBIAS;
                  break;
               }
            }
         }
      }
      else
      {
         // This is case for running simulation. no solve-for objects are stored in forObjects
         // For this case, bias value is gotten from GroundStation.ErrorModels parameter
         bias = measErrorModel->GetRealParameter(biasName);           // bias is consider parameter
      }

      measurementBias.push_back(bias);
   }

   // Clean up memory
   data.clear();
}


//------------------------------------------------------------------------------
// void ComputeMeasurementNoiseSigma(const std::string noiseSigmaName, 
//                                   const std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
* Get measurement noise sigma
*
* @param noisSigmaName      Name of the measurement noise sigma. In this case is 
*                           "NoisSigma"
* @param measType           Measurement type of this tracking data
* @param numTrip            NumTrip shown number of ways signal travel such as 
*                           1-way, 2-ways, or 3-ways
*
* @return                   Value of noise sigma
*/
//------------------------------------------------------------------------------
void TrackingDataAdapter::ComputeMeasurementNoiseSigma(const std::string noiseSigmaName, const std::string measType, Integer numTrip)
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate noise sigma for each signal path
   noiseSigma.clear();
   if (rangeOnly)
   {
      for (UnsignedInt i = 0; i < measurementSize; ++i)
         noiseSigma.push_back(0.0);
      return;
   }

   for (UnsignedInt i = 0; i < measurementSize; ++i)
   {
      if (measErrorModel == NULL)
         measErrorModel = GetMeasurementErrorModel(i, measType, numTrip);

      Real noise = measErrorModel->GetRealParameter(noiseSigmaName);
		noiseSigma.push_back(noise);
   }

   // Clean up memory
   data.clear();
}


//----------------------------------------------------------------------------------
// void ComputeMeasurementErrorCovarianceMatrix()
//----------------------------------------------------------------------------------
/**
* This function is used to set value to covariance matrix associated to this 
* tracking data.
*
*/
//----------------------------------------------------------------------------------
void TrackingDataAdapter::ComputeMeasurementErrorCovarianceMatrix()
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate measurement error covariance matrix
   measErrorCovariance.SetDimension(measurementSize);
   for (UnsignedInt i = 0; i < measurementSize; ++i)
   {
      for (Integer j = 0; j < measurementSize; ++j)
      {
         if (i != j)
            measErrorCovariance(i,j) = 0.0;
         else
         {
            if (noiseSigma[i] != 0.0)
               measErrorCovariance(i,j) =  noiseSigma[i] * noiseSigma[i];
            else
               measErrorCovariance(i,j) =  1.0;          // if no noise setting, set it to 1.0
         }
      }
   }// for i

   // Clean up memory
   data.clear();
}


//----------------------------------------------------------------------------------
// void BeginEndIndexesOfRampTable(Integer & err)
//----------------------------------------------------------------------------------
/**
* This function is used to specify the indexes of the first and the last record 
* associated to this tracking data.
*
* @param err      Error number
*/
//----------------------------------------------------------------------------------
void TrackingDataAdapter::BeginEndIndexesOfRampTable(Integer & err)
{
   // 1. Get search key
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   SignalBase* leg = paths[0];
   SignalData* sd = leg->GetSignalDataObject();

   std::string gsName, scName, gsID, scID, searchkey;
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      gsName = sd->tNode->GetName();
      gsID   = sd->tNode->GetStringParameter("Id");
      scName = sd->rNode->GetName();
      scID   = sd->rNode->GetStringParameter("Id");
   }
   else if (sd->rNode->IsOfType(Gmat::SPACECRAFT) && sd->rNode->IsOfType(Gmat::SPACECRAFT))
   {
      gsName = sd->tNode->GetName();
      gsID = sd->tNode->GetStringParameter("Id");
      scName = sd->rNode->GetName();
      scID = sd->rNode->GetStringParameter("Id");
   }
   else
   {
      gsName = sd->rNode->GetName();
      gsID   = sd->rNode->GetStringParameter("Id");
      scName = sd->tNode->GetName();
      scID   = sd->tNode->GetStringParameter("Id");
   }
   searchkey = gsID + " " + scID + " ";

   // 2. Search for the beginning index
   if (rampTB == NULL)
   {
      err = 2;
      errMsg = "Error: No ramp table was set for " + GetName() + "\n";
      throw MeasurementException("Error: No ramp table was set for " + GetName() + "\n");
   }
   else if ((*rampTB).size() == 0)
   {
      err = 3;
      errMsg = "Error: Ramp table has no data record.\n";
      throw MeasurementException("Error: Ramp table has no data record.\n");
   }

   beginIndex = 0;
   for(; beginIndex < (*rampTB).size(); ++beginIndex)
   {
      if (searchkey == (*rampTB)[beginIndex].indexkey.substr(0,searchkey.size()))
         break;
   }

   // 3. Search for the ending index
   endIndex = beginIndex; 
   for(; endIndex < (*rampTB).size(); ++endIndex)
   {
      if (searchkey != (*rampTB)[endIndex].indexkey.substr(0,searchkey.size()))
         break;
   }

   // 4. Verify number of data records
   if ((endIndex - beginIndex) == 0)
   {
      err = 4;
      std::stringstream ss;
      ss << "Error: Ramp table has no frequency data records for uplink signal from " << gsName << " to " << scName << ". It needs at least 1 record.\n";
      errMsg = ss.str();
      throw MeasurementException(ss.str());
   }
}


//------------------------------------------------------------------------------
// Real RampedFrequencyIntergration(Real t0, Real delta_t, Integer& err)
//------------------------------------------------------------------------------
/**
* Calculate the integration of ramped frequency in range from time t0 to time t1
*
* @param t1         The end time for integration (unit: A1Mjd)
* @param delta_t    Elapse time (unit: second)
* @param err        Error number
*
* @return The integration of ramped frequency.
* Assumptions: ramp table had been sorted by epoch
*/
//------------------------------------------------------------------------------
//Real TrackingDataAdapter::IntegralRampedFrequency(Real t1, Real delta_t, Integer& err)
//{
//#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
//   MessageInterface::ShowMessage("Enter PhysicalMeasurement::IntegralRampedFrequency()\n");
//#endif
//
//   // Verify ramp table and elpase time
//   err = 0;
//   if (delta_t < 0)
//   {
//      err = 1;
//      errMsg = "Error: Elapse time has to be a non negative number\n";
//      throw MeasurementException("Error: Elapse time has to be a non negative number\n");
//   }
//
//   if (rampTB == NULL)
//   {
//      err = 2;
//      errMsg = "Error: No ramp table available for measurement calculation\n";
//      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
//   }
//   
//   if ((*rampTB).size() == 0)
//   {
//      err = 3;
//      std::stringstream ss;
//      ss << "Error: Ramp table has no data record. It needs at least 1 record.\n";
//      errMsg = ss.str();
//      throw MeasurementException(ss.str());
//   }
//   
//   // Get the beginning index and the ending index for frequency data records for this measurement model 
//   BeginEndIndexesOfRampTable(err);
//
//   Real t0 = t1 - delta_t/GmatTimeConstants::SECS_PER_DAY; 
//   Real time_min = (*rampTB)[beginIndex].epoch;
//
//   // Verify t0 and t1 are not out of range of ramp table
//   if (t1 < time_min)
//   {
//      // Convert t1 and time_min from A1Mjd to TAIMjd
//      Real t1TAI, tminTAI;
//      std::string tais;
//      Real a1Time = t1;
//      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t1TAI, tais);
//      a1Time = time_min;
//      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);
//
//      // Generate error message
//      char s[200];
//      sprintf(&s[0], "Error: End epoch t3R = %.12lf is out of range [%.12lf , +%c) of ramp table\n", t1TAI, tminTAI);
//      std::string st(&s[0]);
//      err = 4;
//      errMsg = st;
//      throw MeasurementException(st);
//   }
//
//   if (t0 < time_min)
//   {
//      // Convert t0 and time_min from A1Mjd to TAIMjd
//      Real t0TAI, tminTAI;
//      std::string tais;
//      Real a1Time = t0;
//      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t0TAI, tais);
//      a1Time = time_min;
//      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);
//
//      // Generate error message
//      char s[200];
//      sprintf(&s[0], "Error: Start epoch t1T = %.12lf is out of range [%.12lf , +Inf) of ramp table\n", t0TAI, tminTAI);
//      std::string st(&s[0]);
//      err = 5;
//      errMsg = st;
//      throw MeasurementException(st);
//   }
//
//#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
//   MessageInterface::ShowMessage(" Start epoch t1 = %.15lf A1Mjd\n", t0);
//   MessageInterface::ShowMessage(" End epoch t3   = %.15lf A1Mjd\n", t1);
//   MessageInterface::ShowMessage(" elapse time   = %.15lf s\n", delta_t);
//#endif
//
//   // Search for start index of the interval containing t1
//   UnsignedInt end_interval = beginIndex;
//   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
//   {
//      if (t1 >= (*rampTB)[i].epoch)
//         end_interval = i;
//      else
//         break;
//   }
//
//   Real basedFreq = (*rampTB)[end_interval].rampFrequency; 
//
//#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
//   MessageInterface::ShowMessage("\n End interval: i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n", end_interval, (*rampTB)[end_interval].epoch, (*rampTB)[end_interval].rampFrequency, (*rampTB)[end_interval].rampRate);
//   MessageInterface::ShowMessage("               i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n\n", end_interval+1, (*rampTB)[end_interval+1].epoch, (*rampTB)[end_interval+1].rampFrequency, (*rampTB)[end_interval+1].rampRate);
//   MessageInterface::ShowMessage(" Based frequency = %.15le\n", basedFreq);
//#endif
//
//   // Integration of the frequency from t0 to t1:
//   Real f0, f1, f_dot;
//   Real value1;
//   Real interval_len;
//
//   Real value = 0.0;
//   Real dt = delta_t;
//   for (Integer i = end_interval; dt > 0.0; --i)
//   {
//      // Specify lenght of the "current" interval, f0, and f_dot
//      if (i == end_interval)
//         interval_len = (t1 - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;
//      else
//         interval_len = ((*rampTB)[i+1].epoch - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;
//
//      f0    = (*rampTB)[i].rampFrequency;
//      f_dot = (*rampTB)[i].rampRate;
//      if (dt < interval_len)
//      {
//         f0 = f0 + f_dot*(interval_len - dt);
//         interval_len = dt;
//      }
//
//      // Specify f1
//      f1 = f0 + f_dot*interval_len;
//
//      // Take integral for the current interval
//      value1 = ((f0 + f1)/2 - basedFreq) * interval_len;
//      value  = value + value1;
//
//#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
//      MessageInterface::ShowMessage("interval i = %d:    value1 = %.12lf    f0 = %.12lf   f1 = %.12lf     f_ave = %.12lfHz   width = %.12lfs \n", i, value1, f0, f1, (f0+f1)/2, interval_len);
//      MessageInterface::ShowMessage("interval i = %d: epoch = %.12lf     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n", i,
//      (*rampTB)[i].epoch,  (*rampTB)[i].uplinkBand, (*rampTB)[i].rampType, (*rampTB)[i].rampFrequency, (*rampTB)[i].rampRate);
//#endif
//
//      // Specify dt 
//      dt = dt - interval_len;
//   }
//   Real rel_val = value;
//   value = value + basedFreq*delta_t;
//
//#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
//   MessageInterface::ShowMessage(" value = %.15lf     relative value = %.15lf    based value = %.15lf\n", value, rel_val, basedFreq*delta_t);
//   MessageInterface::ShowMessage("Exit PhysicalMeasurement::IntegralRampedFrequency()\n");
//#endif
//
//   return value;
//}


Real TrackingDataAdapter::IntegralRampedFrequency(GmatTime t1, Real delta_t, Integer& err)
{
#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("Enter PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

   // Verify ramp table and elpase time
   err = 0;
   if (delta_t < 0)
   {
      err = 1;
      errMsg = "Error: Elapse time has to be a non negative number\n";
      throw MeasurementException("Error: Elapse time has to be a non negative number\n");
   }

   if (rampTB == NULL)
   {
      err = 2;
      errMsg = "Error: No ramp table available for measurement calculation\n";
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   }

   if ((*rampTB).size() == 0)
   {
      err = 3;
      std::stringstream ss;
      ss << "Error: Ramp table has no data record. It needs at least 1 record.\n";
      errMsg = ss.str();
      throw MeasurementException(ss.str());
   }

   // Get the beginning index and the ending index for frequency data records for this measurement model 
   BeginEndIndexesOfRampTable(err);

   GmatTime t0 = t1;
   t0.SubtractSeconds(delta_t);

   GmatTime time_min = (*rampTB)[beginIndex].epoch;

   // Verify t0 and t1 are not out of range of ramp table
   if (t1 < time_min)
   {
      // Convert t1 and time_min from A1Mjd to TAIMjd
      GmatTime t1TAI, tminTAI;
      std::string tais;
      GmatTime a1Time = t1;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t1TAI, tais);
      a1Time = time_min;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);

      // Generate error message
      char s[200];
      sprintf(&s[0], "Error: End epoch t3R = %s is out of range [%s , +Inf) of ramp table\n", t1TAI.ToString().c_str(), tminTAI.ToString().c_str());
      std::string st(&s[0]);
      err = 4;
      errMsg = st;
      throw MeasurementException(st);
   }

   if (t0 < time_min)
   {
      // Convert t0 and time_min from A1Mjd to TAIMjd
      GmatTime t0TAI, tminTAI;
      std::string tais;
      GmatTime a1Time = t0;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t0TAI, tais);
      a1Time = time_min;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);

      // Generate error message
      char s[200];
      sprintf(&s[0], "Error: Start epoch t1T = %s is out of range [%s , +Inf) of ramp table\n", t0TAI.ToString().c_str(), tminTAI.ToString().c_str());
      std::string st(&s[0]);
      err = 5;
      errMsg = st;
      throw MeasurementException(st);
   }

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage(" Start epoch t1 = %s A1Mjd\n", t0.ToString().c_str());
   MessageInterface::ShowMessage(" End epoch t3   = %s A1Mjd\n", t1.ToString().c_str());
   MessageInterface::ShowMessage(" elapse time   = %.15lf s\n", delta_t);
#endif

   // Search for start index of the interval containing t1
   UnsignedInt end_interval = beginIndex;
   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
   {
      if (t1 >= (*rampTB)[i].epochGT)
         end_interval = i;
      else
         break;
   }

   Real basedFreq = (*rampTB)[end_interval].rampFrequency;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("\n End interval: i = %d    epoch = %s A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n", end_interval, (*rampTB)[end_interval].epochGT.ToString().c_str(), (*rampTB)[end_interval].rampFrequency, (*rampTB)[end_interval].rampRate);
   MessageInterface::ShowMessage("               i = %d    epoch = %s A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n\n", end_interval + 1, (*rampTB)[end_interval + 1].epochGT.ToString().c_str(), (*rampTB)[end_interval + 1].rampFrequency, (*rampTB)[end_interval + 1].rampRate);
   MessageInterface::ShowMessage(" Based frequency = %.15le\n", basedFreq);
#endif

   // Integration of the frequency from t0 to t1:
   Real f0, f1, f_dot;
   Real value1;
   Real interval_len;

   Real value = 0.0;
   Real dt = delta_t;
   for (Integer i = end_interval; dt > 0.0; --i)
   {
      // Specify lenght of the "current" interval, f0, and f_dot
      if (i == end_interval)
         interval_len = (t1 - (*rampTB)[i].epochGT).GetTimeInSec();
      else
         interval_len = ((*rampTB)[i + 1].epochGT - (*rampTB)[i].epochGT).GetTimeInSec();

      f0 = (*rampTB)[i].rampFrequency;
      f_dot = (*rampTB)[i].rampRate;
      if (dt < interval_len)
      {
         f0 = f0 + f_dot*(interval_len - dt);
         interval_len = dt;
      }

      // Specify f1
      f1 = f0 + f_dot*interval_len;

      // Take integral for the current interval
      value1 = ((f0 + f1) / 2 - basedFreq) * interval_len;
      value = value + value1;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
      MessageInterface::ShowMessage("interval i = %d:    value1 = %.12lf    f0 = %.12lf   f1 = %.12lf     f_ave = %.12lfHz   width = %.12lfs \n", i, value1, f0, f1, (f0 + f1) / 2, interval_len);
      MessageInterface::ShowMessage("interval i = %d: epoch = %s     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n", i,
         (*rampTB)[i].epochGT.ToString().c_str(), (*rampTB)[i].uplinkBand, (*rampTB)[i].rampType, (*rampTB)[i].rampFrequency, (*rampTB)[i].rampRate);
#endif

      // Specify dt 
      dt = dt - interval_len;
   }
   Real rel_val = value;
   value = value + basedFreq*delta_t;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage(" value = %.15lf     relative value = %.15lf    based value = %.15lf\n", value, rel_val, basedFreq*delta_t);
   MessageInterface::ShowMessage("Exit PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

   return value;
}


//------------------------------------------------------------------------------
// Real ApiGetDerivativeValue(Integer row,Integer column)
//------------------------------------------------------------------------------
/**
 * Method used in the API calls to access derivative data
 *
 * @param row Index for the row.  Set to -1  with column = 0 to get the number of rows.
 * @param column Index for the column.  Set to -1  with row = 0 to get the number of rows.
 *
 * @return The derivative value, or the row/column count as spec'd above
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::ApiGetDerivativeValue(Integer row,Integer column)
{
   Real retval = -987654321.012345;

   if ((row == -1) && (column == 0))
      retval = theDataDerivatives.size();
   else if ((row == 0) && (column == -1))
   {
      if (theDataDerivatives.size() > 0)
         retval = theDataDerivatives[0].size();
      else
         retval = 0.0;
   }
   else if ((row < theDataDerivatives.size()) && (row >= 0))
      if ((column < theDataDerivatives[row].size()) && (column >= 0))
         retval = theDataDerivatives[row][column];

   return retval;
}

//------------------------------------------------------------------------------
// void SetIonosphereCache(SignalDataCache::SimpleSignalDataCache * cache)
//------------------------------------------------------------------------------
/**
 * Sets a reference to the ionosphere cache that will be passed in to the measure model and that
 * can subsequently be used to store calculated corrections that can be reused
 *
 * @param cache the ionosphere cache started by the tracking file set
 *
 */
 //------------------------------------------------------------------------------
void TrackingDataAdapter::SetIonosphereCache(SignalDataCache::SimpleSignalDataCache * cache)
{
   ionosphereCache = cache;
}

//------------------------------------------------------------------------------
// void SetupHorp(Real height, Real angle, bool useOblateness)
//------------------------------------------------------------------------------
/**
 * Sets the height of ray path parameters for spacecraft to spacecraft signals
 *
 * @param height The minimum height allowing a signal to be valid
 * @param angle The central angle used when determining if a signal in behind a body
 * @param useOblateness Toggle to use oblate bodies rather than spherical
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetupHorp(Real height, Real angle, bool useOblateness, bool force)
{
   useHorp = true;
   forceHorp = force;
   horpHeight = height;
   horpAngle = angle;
   horpOblateness = useOblateness;
}


//------------------------------------------------------------------------------------------
// std::string GetTrackingConfig()
//------------------------------------------------------------------------------------------
/**
* This function is used to get tracking data configure
* Example: {{65,328,65}Range} or {{64,328,64}DSN_TCP}
*/
//------------------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetTrackingConfig()
{
   std::stringstream ss;
   ss << "{";
   for (Integer i = 0; i < participantLists.size(); ++i)
   {
      ss << "{";
      for (Integer k = 0; k < participantLists[i]->size(); ++k)
      {

         for (Integer j = 0; j < refObjects.size(); ++j)
         {
            if (refObjects[j]->GetName() == participantLists[i]->at(k))
            {
               std::string id;
               if (refObjects[j]->IsOfType(Gmat::SPACECRAFT))
               {
                  id = ((Spacecraft*)refObjects[j])->GetStringParameter("Id");
                  ss << id;
                  if (typeName == "GPS_PosVec")
                  {
                     // Get sensor ID
                     ObjectArray objList = ((Spacecraft*)refObjects[j])->GetRefObjectArray(Gmat::HARDWARE);
                     for (UnsignedInt k = 0; k < objList.size(); ++k)
                     {
                        if (objList[k]->IsOfType("Receiver"))
                        {
                           ss << "." << ((Receiver*)objList[k])->GetStringParameter("Id");
                           break;
                        }
                     }
                  }
               }
               else if (refObjects[j]->IsOfType(Gmat::GROUND_STATION))
               {
                  id = ((GroundstationInterface*)refObjects[j])->GetStringParameter("Id");
                  ss << id;
               }

               break;
            }
         }

         if (k < (*participantLists[i]).size() - 1)
            ss << ",";
      }
      ss << "}";
   }
   ss << typeName << "}";
   
   return ss.str();
}
