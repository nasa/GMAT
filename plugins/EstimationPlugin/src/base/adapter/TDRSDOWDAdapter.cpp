//------------------------------------------------------------------------------
//                           TDRSDOWDAdapter
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
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: Jan 4, 2021
/**
 * A measurement adapter for TDRS DOWD (unit: Hz)
 */
//------------------------------------------------------------------------------

#include "TDRSDOWDAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "Transponder.hpp"
#include "PropSetup.hpp"
#include "RandomNumber.hpp"
#include "ErrorModel.hpp"
#include <sstream>
#include <time.h>

#include "SpaceObject.hpp"


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_DERIVATIVE_CALCULATION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE
//#define DEBUG_DOPPLER_CALCULATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
TDRSDOWDAdapter::PARAMETER_TEXT[TDRSDOWDAdapterParamCount - AdapterParamCount] =
{
   "DopplerCountIntervalTDRSComp",
   "ServiceAccessTDRSComp",
   "Node4FrequencyTDRSComp",
   "Node4BandTDRSComp",
   "SmarIdTDRSComp",
   "DataFlagTDRSComp",
   "DopplerCountIntervalTDRSRef",
   "ServiceAccessTDRSRef",
   "Node4FrequencyTDRSRef",
   "Node4BandTDRSRef",
   "SmarIdTDRSRef",
   "DataFlagTDRSRef",
};


const Gmat::ParameterType
TDRSDOWDAdapter::PARAMETER_TYPE[TDRSDOWDAdapterParamCount - AdapterParamCount] =
{
   Gmat::REAL_TYPE,                    // TDRS_COMP_DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,             // TDRS_COMP_SERVICE_ACCESS
   Gmat::REAL_TYPE,                    // TDRS_COMP_NODE4_FREQUENCY
   Gmat::INTEGER_TYPE,                 // TDRS_COMP_NODE4_BAND
   Gmat::INTEGER_TYPE,                 // TDRS_COMP_SMAR_ID
   Gmat::INTEGER_TYPE,                 // TDRS_COMP_DATA_FLAG
   Gmat::REAL_TYPE,                    // TDRS_REF_DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,             // TDRS_REF_SERVICE_ACCESS
   Gmat::REAL_TYPE,                    // TDRS_REF_NODE4_FREQUENCY
   Gmat::INTEGER_TYPE,                 // TDRS_REF_NODE4_BAND
   Gmat::INTEGER_TYPE,                 // TDRS_REF_SMAR_ID
   Gmat::INTEGER_TYPE,                 // TDRS_REF_DATA_FLAG
};




//------------------------------------------------------------------------------
// TDRSDOWDAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
TDRSDOWDAdapter::TDRSDOWDAdapter(const std::string& name) :
   TrackingDataAdapter         ("SN_DOWD", name),
   adapterTDRSComp             (NULL),
   dopplerCountIntervalTDRSComp(1.0),                     // 1 second
   node4FreqTDRSComp           (2000.0),                  // 2000 Mhz
   node4FreqBandTDRSComp       (1),                       // 0: undefined, 1: S-band, 2: X-band, 3: K-band
   smarIdTDRSComp              (0),
   dataFlagTDRSComp            (0),
   adapterTDRSRef              (NULL),
   dopplerCountIntervalTDRSRef (1.0),                     // 1 second
   node4FreqTDRSRef            (2000.0),                  // 2000 Mhz
   node4FreqBandTDRSRef        (1),                       // 0: undefined, 1: S-band, 2: X-band, 3: K-band
   smarIdTDRSRef               (0),
   dataFlagTDRSRef             (0)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSDOWDAdapter default constructor <%p>\n", this);
#endif
   typeName = "SN_DOWD";
   parameterCount = TDRSDOWDAdapterParamCount;

   // TDRS DOWD measurement model (Its tracking config is {{userSC, TDRSRef, GSRef, TDRSComp, GSComp}, 'SN_DOWD'}) 
   // has 2 TDRS 3L return doppler measurements {{userSC, TDRSRef, GSRef}, 'SN_Doppler_Rtn'} and 
   // {{userSC, TDRSComp, GSComp}, 'SN_Doppler_Rtn'}
   // The measuring participant is ground station GSComp. 
   // Its index in participant list {userSC, TDRSRef, GSRef, TDRSComp, GSComp} is 4.
   measParticipantIndex = 4;

   multiplierTDRSComp = 1.0;
   multiplierTDRSRef = -1.0;
}


//------------------------------------------------------------------------------
// ~TDRSDOWDAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TDRSDOWDAdapter::~TDRSDOWDAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSDOWDAdapter default destructor <%p>\n", this);
#endif

   if (adapterTDRSComp)
      delete adapterTDRSComp;

   if (adapterTDRSRef)
      delete adapterTDRSRef;
}


//------------------------------------------------------------------------------
// TDRSDOWDAdapter(const TDRSDOWDAdapter& da)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param da The adapter copied to make this one
 */
//------------------------------------------------------------------------------
TDRSDOWDAdapter::TDRSDOWDAdapter(const TDRSDOWDAdapter& da) :
   TrackingDataAdapter           (da),
   adapterTDRSComp               (NULL),
   dopplerCountIntervalTDRSComp  (da.dopplerCountIntervalTDRSComp),
   serviceAccessListTDRSComp     (da.serviceAccessListTDRSComp),
   node4FreqTDRSComp             (da.node4FreqTDRSComp),
   node4FreqBandTDRSComp         (da.node4FreqBandTDRSComp),
   smarIdTDRSComp                (da.smarIdTDRSComp),
   dataFlagTDRSComp              (da.dataFlagTDRSComp),
   adapterTDRSRef                (NULL),
   dopplerCountIntervalTDRSRef   (da.dopplerCountIntervalTDRSRef),
   serviceAccessListTDRSRef      (da.serviceAccessListTDRSRef),
   node4FreqTDRSRef              (da.node4FreqTDRSRef),
   node4FreqBandTDRSRef          (da.node4FreqBandTDRSRef),
   smarIdTDRSRef                 (da.smarIdTDRSRef),
   dataFlagTDRSRef               (da.dataFlagTDRSRef)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSDOWDAdapter copy constructor   from <%p> to <%p>\n", &da, this);
#endif
   if (da.adapterTDRSComp)
      adapterTDRSComp = (TDRS3LReturnDopplerAdapter*)(da.adapterTDRSComp->Clone());
   if (da.adapterTDRSRef)
      adapterTDRSRef = (TDRS3LReturnDopplerAdapter*)(da.adapterTDRSRef->Clone());

   multiplierTDRSComp = 1.0;
   multiplierTDRSRef = -1.0;
}


//------------------------------------------------------------------------------
// TDRSDOWDAdapter& operator=(const TDRSDOWDAdapter& da)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param da The adapter copied to make this one match it
 *
 * @return This adapter made to look like da
 */
//------------------------------------------------------------------------------
TDRSDOWDAdapter& TDRSDOWDAdapter::operator=(const TDRSDOWDAdapter& da)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSDOWDAdapter operator =   set <%p> = <%p>\n", this, &da);
#endif

   if (this != &da)
   {
      TrackingDataAdapter::operator=(da);

      serviceAccessListTDRSComp = da.serviceAccessListTDRSComp;
      node4FreqTDRSComp         = da.node4FreqTDRSComp;
      node4FreqBandTDRSComp     = da.node4FreqBandTDRSComp;
      smarIdTDRSComp            = da.smarIdTDRSComp;
      dataFlagTDRSComp          = da.dataFlagTDRSComp;

      if (adapterTDRSComp)
      {
         delete adapterTDRSComp;
         adapterTDRSComp = NULL;
      }
      if (da.adapterTDRSComp)
         adapterTDRSComp = (TDRS3LReturnDopplerAdapter*)da.adapterTDRSComp->Clone();

      serviceAccessListTDRSRef    = da.serviceAccessListTDRSRef;
      node4FreqTDRSRef            = da.node4FreqTDRSRef;
      node4FreqBandTDRSRef        = da.node4FreqBandTDRSRef;
      smarIdTDRSRef               = da.smarIdTDRSRef;
      dataFlagTDRSRef             = da.dataFlagTDRSRef;

      if (adapterTDRSRef)
      {
         delete adapterTDRSRef;
         adapterTDRSRef = NULL;
      }
      if (da.adapterTDRSRef)
         adapterTDRSRef = (TDRS3LReturnDopplerAdapter*)da.adapterTDRSRef->Clone();
   }

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
void TDRSDOWDAdapter::SetSolarSystem(SolarSystem *ss)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TDRSDOWDAdapter<%p>::SetSolarSystem('%s')\n", this, ss->GetName().c_str()); 
#endif

   adapterTDRSComp->SetSolarSystem(ss);
   adapterTDRSRef->SetSolarSystem(ss);

   TrackingDataAdapter::SetSolarSystem(ss);
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new adapter that matches this one
 *
 * @return A new adapter set to match this one
 */
//------------------------------------------------------------------------------
GmatBase* TDRSDOWDAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRSDOWDAdapter::Clone()   clone this <%p>\n", this);
#endif

   return new TDRSDOWDAdapter(*this);
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
std::string TDRSDOWDAdapter::GetParameterText(const Integer id) const
{
   if (id >= AdapterParamCount && id < TDRSDOWDAdapterParamCount)
      return PARAMETER_TEXT[id - AdapterParamCount];
   return TrackingDataAdapter::GetParameterText(id);
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
Integer TDRSDOWDAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = AdapterParamCount; i < TDRSDOWDAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AdapterParamCount])
         return i;
   }
   return TrackingDataAdapter::GetParameterID(str);
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
Gmat::ParameterType TDRSDOWDAdapter::GetParameterType(const Integer id) const
{
   if (id >= AdapterParamCount && id < TDRSDOWDAdapterParamCount)
      return PARAMETER_TYPE[id - AdapterParamCount];

   return TrackingDataAdapter::GetParameterType(id);
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
std::string TDRSDOWDAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool TDRSDOWDAdapter::SetStringParameter(const Integer id, const std::string& value)
{
   bool retval = true;
   if ((id != MEASUREMENT_TYPE)&&(id != SIGNAL_PATH))
   {
      retval = adapterTDRSComp->SetStringParameter(id, value) && retval;
      retval = adapterTDRSRef->SetStringParameter(id, value) && retval;
   }

   retval = TrackingDataAdapter::SetStringParameter(id, value) && retval;

   return retval; 
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
bool TDRSDOWDAdapter::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if (id == TDRS_COMP_SERVICE_ACCESS)
   {
      if (((Integer)serviceAccessListTDRSComp.size() > index) && (index >= 0))
         serviceAccessListTDRSComp[index] = value;
      else if ((Integer)serviceAccessListTDRSComp.size() == index)
         serviceAccessListTDRSComp.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
            "set a service access name");

      return true;
   }

   if (id == TDRS_REF_SERVICE_ACCESS)
   {
      if (((Integer)serviceAccessListTDRSRef.size() > index) && (index >= 0))
         serviceAccessListTDRSRef[index] = value;
      else if ((Integer)serviceAccessListTDRSRef.size() == index)
         serviceAccessListTDRSRef.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
            "set a service access name");

      return true;
   }

   bool retval = true;
   //retval = adapterTDRSRef->SetStringParameter(id, value, index) && retval;
   //retval = adapterTDRSRef->SetStringParameter(id, value, index) && retval;
 
   retval = TrackingDataAdapter::SetStringParameter(id, value, index) && retval;

   return retval;
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
std::string TDRSDOWDAdapter::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == TDRS_COMP_SERVICE_ACCESS)
   {
      if ((0 <= index)&&(index < serviceAccessListTDRSComp.size()))
         return serviceAccessListTDRSComp[index];
      else
      {
         std::stringstream ss;
         ss << "Error: index (" << index << ") is out of bound (" << serviceAccessListTDRSComp.size() << ")\n";
         throw MeasurementException(ss.str());
      }
   }

   if (id == TDRS_REF_SERVICE_ACCESS)
   {
      if ((0 <= index) && (index < serviceAccessListTDRSRef.size()))
         return serviceAccessListTDRSRef[index];
      else
      {
         std::stringstream ss;
         ss << "Error: index (" << index << ") is out of bound (" << serviceAccessListTDRSRef.size() << ")\n";
         throw MeasurementException(ss.str());
      }
   }

   return TrackingDataAdapter::GetStringParameter(id, index);
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
bool TDRSDOWDAdapter::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
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
bool TDRSDOWDAdapter::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
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
std::string TDRSDOWDAdapter::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
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
Integer TDRSDOWDAdapter::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == TDRS_COMP_NODE4_BAND)
   {
      // @todo: verify a valid frequency band
      node4FreqBandTDRSComp = value;
      return adapterTDRSComp->SetIntegerParameter("Node4Band", value);
   }
   if (id == TDRS_REF_NODE4_BAND)
   {
      // @todo: verify a valid frequency band
      node4FreqBandTDRSRef = value;
      return adapterTDRSRef->SetIntegerParameter("Node4Band", value);
   }

   if (id == TDRS_COMP_SMAR_ID)
   {
      // @todo: verify a valid SMAR id
      smarIdTDRSComp = value;
      return adapterTDRSComp->SetIntegerParameter("SmarId", value);
   }
   if (id == TDRS_REF_SMAR_ID)
   {
      // @todo: verify a valid SMAR id
      smarIdTDRSRef = value;
      return adapterTDRSRef->SetIntegerParameter("SmarId", value);
   }

   if (id == TDRS_COMP_DATA_FLAG)
   {
      // @todo: verify a valid data flag
      dataFlagTDRSComp = value;
      return adapterTDRSComp->SetIntegerParameter("DataFlag", value);
   }
   if (id == TDRS_REF_DATA_FLAG)
   {
      // @todo: verify a valid data flag
      dataFlagTDRSRef = value;
      return adapterTDRSRef->SetIntegerParameter("DataFlag", value);
   }

   return TrackingDataAdapter::SetIntegerParameter(id, value);
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
Integer TDRSDOWDAdapter::GetIntegerParameter(const Integer id) const
{
   if (id == TDRS_COMP_NODE4_BAND)
      return node4FreqBandTDRSComp;
   if (id == TDRS_REF_NODE4_BAND)
      return node4FreqBandTDRSRef;

   if (id == TDRS_COMP_SMAR_ID)
      return smarIdTDRSComp;
   if (id == TDRS_REF_SMAR_ID)
      return smarIdTDRSRef;

   if (id == TDRS_COMP_DATA_FLAG)
      return dataFlagTDRSComp;
   if (id == TDRS_REF_DATA_FLAG)
      return dataFlagTDRSRef;

   return TrackingDataAdapter::GetIntegerParameter(id);
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
Integer TDRSDOWDAdapter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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
Integer TDRSDOWDAdapter::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
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
Real TDRSDOWDAdapter::GetRealParameter(const Integer id) const
{
   if (id == TDRS_COMP_DOPPLER_COUNT_INTERVAL)
      return dopplerCountIntervalTDRSComp;
   if (id == TDRS_REF_DOPPLER_COUNT_INTERVAL)
      return dopplerCountIntervalTDRSRef;

   if (id == TDRS_COMP_NODE4_FREQUENCY)
      return node4FreqTDRSComp;
   if (id == TDRS_REF_NODE4_FREQUENCY)
      return node4FreqTDRSRef;

   return TrackingDataAdapter::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TDRSDOWDAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == TDRS_COMP_DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler count interval for comparison TDRS one way doppler measurement has a nonpositive value\n");

      dopplerCountIntervalTDRSComp = value;
      adapterTDRSComp->SetRealParameter("DopplerCountInterval", value);
      return dopplerCountIntervalTDRSComp;
   }
   if (id == TDRS_REF_DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler count interval for reference TDRS One Way Doppler measurement has a nonpositive value\n");

      dopplerCountIntervalTDRSRef = value;
      adapterTDRSRef->SetRealParameter("DopplerCountInterval", value);
      return dopplerCountIntervalTDRSRef;
   }

   if (id == TDRS_COMP_NODE4_FREQUENCY)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: TDRS node 4 frequency for comparison TDRS One Way Doppler measurement has a nonpositive number\n");
      node4FreqTDRSComp = value;
      adapterTDRSComp->SetRealParameter("Node4Frequency", value);
      return node4FreqTDRSComp;
   }
   if (id == TDRS_REF_NODE4_FREQUENCY)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: TDRS node 4 frequency for reference TDRS One Way Doppler measurement has a nonpositive number\n");
      node4FreqTDRSRef = value;
      adapterTDRSRef->SetRealParameter("Node4Frequency", value);
      return node4FreqTDRSRef;
   }

   return TrackingDataAdapter::SetRealParameter(id, value);
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
Real TDRSDOWDAdapter::GetRealParameter(const std::string &label) const
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
Real TDRSDOWDAdapter::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 */
 //------------------------------------------------------------------------------
bool TDRSDOWDAdapter::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == ADD_NOISE)
   {
      // comparison and reference TDRS One Way Doppler measurement have to be calculate w/o noise and bias
      // TDRSDOWD = (comparion - reference) + TDRSDOWD's noise + TDRSDOWD's bias
      adapterTDRSComp->SetBooleanParameter(id, false);
      adapterTDRSRef->SetBooleanParameter(id, false);
   }
   else
   {
      adapterTDRSComp->SetBooleanParameter(id, value);
      adapterTDRSRef->SetBooleanParameter(id, value);
   }

   // It needs to set ADD_NOISE in TDRSDOWD object too
   return TrackingDataAdapter::SetBooleanParameter(id, value);
}


bool TDRSDOWDAdapter::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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
const StringArray& TDRSDOWDAdapter::GetStringArrayParameter(
      const Integer id) const
{
   if (id == TDRS_COMP_SERVICE_ACCESS)
      return serviceAccessListTDRSComp;
   if (id == TDRS_REF_SERVICE_ACCESS)
      return serviceAccessListTDRSRef;

   return TrackingDataAdapter::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Method used to rename reference objects
 *
 * @param type The type of the object that is renamed
 * @param oldName The old object name
 * @param newName The new name
 *
 * @return true if a rename happened, false if not
 */
//------------------------------------------------------------------------------
bool TDRSDOWDAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   // Handle additional renames specific to this adapter
   bool retval = true;
   retval = adapterTDRSComp->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterTDRSRef->RenameRefObject(type, oldName, newName) && retval;
 
   retval = TrackingDataAdapter::RenameRefObject(type, oldName, newName) && retval;

   return retval;
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
bool TDRSDOWDAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   bool retval = true;
   retval = adapterTDRSComp->SetRefObject(obj, type, name) && retval;
   retval = adapterTDRSRef->SetRefObject(obj, type, name) && retval;
 
   retval = TrackingDataAdapter::SetRefObject(obj, type, name) && retval;

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
bool TDRSDOWDAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   bool retval = true;
   retval = adapterTDRSComp->SetRefObject(obj, type, name, index) && retval;
   retval = adapterTDRSRef->SetRefObject(obj, type, name, index) && retval;

   retval = TrackingDataAdapter::SetRefObject(obj, type, name, index) && retval;

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
bool TDRSDOWDAdapter::SetMeasurement(MeasureModel* meas)
{
   return TrackingDataAdapter::SetMeasurement(meas);
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
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
void TDRSDOWDAdapter::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting propagator to %p in "
            "TDRSDOWDAdapter\n", ps);
   #endif

   adapterTDRSComp->SetPropagators(ps, spMap);
   adapterTDRSRef->SetPropagators(ps, spMap);

   TrackingDataAdapter::SetPropagators(ps, spMap);
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
void TDRSDOWDAdapter::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    GetMeasurementModel()->SetTransientForces(tf);
    adapterTDRSComp->SetTransientForces(tf);
    adapterTDRSRef->SetTransientForces(tf);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool TDRSDOWDAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("TDRSDOWDAdapter::Initialize() <%p> start\n", this);
#endif

   bool retval = false;

   calcData->measParticipantIndex = measParticipantIndex;

   if (TrackingDataAdapter::Initialize())
   {
      // @todo: initialize all needed variables
      //if (serviceAccessList.size() == 0)
      //   throw MeasurementException("Error: no TDRS service access was set for measurement\n");
      
      if (useHorp || forceHorp)
      {
         adapterTDRSComp->SetupHorp(horpHeight, horpAngle, horpOblateness, forceHorp);
         adapterTDRSRef->SetupHorp(horpHeight, horpAngle, horpOblateness, forceHorp);
      }

      serviceAccessIndexTDRSComp = 0;
      serviceAccessIndexTDRSRef = 0;

      // Set service access to comparison and reference TDRS one way doppler measurement objects
      for (Integer i = 0; i < serviceAccessListTDRSComp.size(); ++i)
         adapterTDRSComp->SetStringParameter("ServiceAccess", serviceAccessListTDRSComp[i], i);
      for (Integer i = 0; i < serviceAccessListTDRSRef.size(); ++i)
         adapterTDRSRef->SetStringParameter("ServiceAccess", serviceAccessListTDRSRef[i], i);

      retval = adapterTDRSComp->Initialize();
      retval = adapterTDRSRef->Initialize() && retval;

      if (participantLists.size() > 1)
         MessageInterface::ShowMessage("Warning: .gmd files do not support "
            "multiple strands\n");

      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
         {
            std::string theId;

            for (UnsignedInt k = 0; k < refObjects.size(); ++k)
            {
               if (refObjects[k]->GetName() == participantLists[i]->at(j))
               {
                  theId = refObjects[k]->GetStringParameter("Id");
                  break;
               }
            }
            cMeasurement.participantIDs.push_back(theId);
         }
      }

     

      isInitialized = true;
   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("TDRSDOWDAdapter::Initialize() <%p> exit\n", this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents,
//       ObservationData* forObservation, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Computes the measurement associated with this adapter
 *
 * @note: The parameters associated with this call will probably be removed;
 * they are here to support compatibility with the old measurement models
 *
 * @param withEvents Flag indicating is the light time solution should be
 *                   included
 * @param forObservation The observation data associated with this measurement
 * @param rampTB Ramp table for a ramped measurement
 *
 * @return The computed measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& TDRSDOWDAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable,
      bool forSimulation)
{
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("TDRSDOWDAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   bool epochValid = true;

   GmatTime computeEpoch;
   if (forObservation)
      computeEpoch = forObservation->epochGT;
   else
   {
      for (UnsignedInt i = 0; i < refObjects.size(); ++i)
      {
         if (refObjects[i]->IsOfType(Gmat::SPACEOBJECT))
         {
            computeEpoch = ((SpaceObject*)refObjects[i])->GetEpochGT();
            break;
         }
      }
   }

   // Check for epoch validity on the ephemeris propagators
   if (calcData->CheckEpochValidity(computeEpoch))
   {
      // 1.1. Set value for local variables
      obsData = forObservation;
      // 1.2. Reset value for Doppler count interval
      if (obsData)
      {
         dopplerCountIntervalTDRSComp = obsData->dopplerCountInterval;          // unit: second
         serviceAccessListTDRSComp.clear();
         serviceAccessListTDRSComp.push_back(obsData->tdrsServiceID);           // SA1, SA2, or MA
         serviceAccessIndexTDRSComp = 0;
         node4FreqTDRSComp     = obsData->tdrsNode4Freq/1.0e6;                  // unit: MHz
         node4FreqBandTDRSComp = obsData->tdrsNode4Band;                        // 0: unspecified, 1: S-band, 2: X-band, 3: K-band
         dataFlagTDRSComp      = obsData->tdrsDataFlag;
         smarIdTDRSComp        = obsData->tdrsSMARID;

         dopplerCountIntervalTDRSRef = obsData->dopplerCountIntervalTDRSRef;    // unit: second
         serviceAccessListTDRSRef.clear();
         serviceAccessListTDRSRef.push_back(obsData->tdrsServiceIDTDRSRef);     // SA1, SA2, or MA
         serviceAccessIndexTDRSRef = 0;
         node4FreqTDRSRef          = obsData->tdrsNode4FreqTDRSRef / 1.0e6;              // unit: MHz
         node4FreqBandTDRSRef      = obsData->tdrsNode4BandTDRSRef;                  // 0: unspecified, 1: S-band, 2: X-band, 3: K-band
         dataFlagTDRSRef           = obsData->tdrsDataFlagTDRSRef;
         smarIdTDRSRef             = obsData->tdrsSMARIDTDRSRef;
      }
      else
      {
         // obsData == NULL That means simulation
         if (serviceAccessListTDRSComp.empty())
            throw MeasurementException("The simulator TDRS service access list for TDRS DOWD measurements is empty. Please add a valid service (SA1, SA2, or MA) to the list.\n");
         if (serviceAccessListTDRSRef.empty())
         {
            serviceAccessListTDRSRef = serviceAccessListTDRSComp;
            for (Integer i = 0; i < serviceAccessListTDRSComp.size(); ++i)
               adapterTDRSRef->SetStringParameter("ServiceAccess", serviceAccessListTDRSRef[i], i);
         }
      }

      if (forSimulation)
      {
         srand(time(NULL));
         serviceAccessIndexTDRSComp = rand() % serviceAccessListTDRSComp.size();
         serviceAccessIndexTDRSRef = serviceAccessIndexTDRSComp;
         if (serviceAccessListTDRSComp[serviceAccessIndexTDRSComp] == "MA")
         {
            if (smarIdTDRSComp == 0)
               smarIdTDRSComp = 1;
         }
         else if (serviceAccessListTDRSComp[serviceAccessIndexTDRSComp] == "SA1" || serviceAccessListTDRSComp[serviceAccessIndexTDRSComp] == "SA2")
         {
            smarIdTDRSComp = 0;
         }

         if (serviceAccessListTDRSRef[serviceAccessIndexTDRSRef] == "MA")
         {
            if (smarIdTDRSRef == 0)
               smarIdTDRSRef = 1;
         }
         else if (serviceAccessListTDRSRef[serviceAccessIndexTDRSRef] == "SA1" || serviceAccessListTDRSRef[serviceAccessIndexTDRSRef] == "SA2")
         {
            smarIdTDRSRef = 0;
         }
      }
      cMeasurement.tdrsNode4Freq = node4FreqTDRSComp*1.0e6;        // unit Hz
      cMeasurement.tdrsNode4Band = node4FreqBandTDRSComp;
      cMeasurement.tdrsDataFlag  = dataFlagTDRSComp;
      cMeasurement.tdrsSMARID    = smarIdTDRSComp;
      cMeasurement.tdrsNode4FreqTDRSRef = node4FreqTDRSRef * 1.0e6;        // unit Hz
      cMeasurement.tdrsNode4BandTDRSRef = node4FreqBandTDRSRef;
      cMeasurement.tdrsDataFlagTDRSRef = dataFlagTDRSRef;
      cMeasurement.tdrsSMARIDTDRSRef = smarIdTDRSRef;

      if (serviceAccessListTDRSComp.empty())
         throw MeasurementException("Error: No TDRS service access of comparison TDRS DOWD measurement is specified for the measurement.\n");
      if (serviceAccessListTDRSRef.empty())
         throw MeasurementException("Error: No TDRS service access of reference TDRS DOWD measurement is specified for the measurement.\n");
      cMeasurement.tdrsServiceID = serviceAccessListTDRSComp[serviceAccessIndexTDRSComp];
      cMeasurement.tdrsServiceIDTDRSRef = serviceAccessListTDRSRef[serviceAccessIndexTDRSRef];

      //MessageInterface::ShowMessage("Comparison TDRS: service access = %s   node 4 frequency = %le Hz   node 4 band = %d    TDRS data flag = %d    SMAR id = %d\n",
      //   cMeasurement.tdrsServiceID.c_str(), cMeasurement.tdrsNode4Freq, cMeasurement.tdrsNode4Band, cMeasurement.tdrsDataFlag, cMeasurement.tdrsSMARID);
      //MessageInterface::ShowMessage("Reference TDRS: service access = %s   node 4 frequency = %le Hz   node 4 band = %d    TDRS data flag = %d    SMAR id = %d\n",
      //   cMeasurement.tdrsServiceIDTDRSRef.c_str(), cMeasurement.tdrsNode4FreqTDRSRef, cMeasurement.tdrsNode4BandTDRSRef, cMeasurement.tdrsDataFlagTDRSRef, cMeasurement.tdrsSMARIDTDRSRef);


      // 2. Compute comparison TDRS 3L Return Doppler measurement 
      // 2.1. Propagate all space objects to time tm
      // This step is not needed due to measurement time tm is set to t3RE
      // 2.2. Specify comparison TDRS 3L Return Doppler measurement in Hz
      bool addNoiseOption  = addNoise;
      bool addBiasOption   = addBias;
      bool rangeOnlyOption = rangeOnly;
      bool withMedia = withMediaCorrection;

      // 2.2.1. Compute range in km w/o any noise or bias for End-Long path
      addNoise  = false;
      addBias   = false;
      rangeOnly = true;

#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute the comparison TDRS One Way Doppler measurement...    adapterTDRSComp = <%p>\n", adapterTDRSComp);
#endif
      adapterTDRSComp->AddBias(false);
      adapterTDRSComp->AddNoise(false);
      adapterTDRSComp->SetRangeOnly(true);
      adapterTDRSComp->CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);
      measDataTDRSComp = adapterTDRSComp->GetMeasurement();
      cMeasurement.epochGT = measDataTDRSComp.epochGT;
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation of the comparison TDRS One Way Doppler measurement.\n");
#endif
      
      
      // reset back to their orginal values
      addNoise  = addNoiseOption;
      addBias   = addBiasOption;
      rangeOnly = rangeOnlyOption;
      withMediaCorrection = withMedia;

      ////// 2.3.1. Measurement time is the same as the one for reference TDRS
      ////GmatTime tm = cMeasurement.epochGT;                              // Get measurement time
      ////ObservationData* obData = NULL;
      ////if (forObservation)
      ////   obData = new ObservationData(*forObservation);
      ////else
      ////   obData = new ObservationData();
      ////obData->epochGT = tm;
      ////obData->epoch   = tm.GetMjd();

      // 2.3.2. Compute TDRS oneway doppler w/o any noise and bias for reference TDRS
      // This calculation does not add bias and noise to calculated value
      adapterTDRSRef->AddBias(false);
      adapterTDRSRef->AddNoise(false);
      adapterTDRSRef->SetRangeOnly(true);
      adapterTDRSRef->AddMediaCorrection(withMedia);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute the reference TDRS One Way Doppler measurement...    adapterTDRSRef = <%p>\n", adapterTDRSRef);
#endif
      if (forObservation)
      {
         ObservationData* obData1 = new ObservationData(*forObservation);
         obData1->dopplerCountInterval = obData1->dopplerCountIntervalTDRSRef;
         obData1->tdrsNode4Freq = obData1->tdrsNode4FreqTDRSRef;
         obData1->tdrsNode4Band = obData1->tdrsNode4BandTDRSRef;
         obData1->tdrsServiceID = obData1->tdrsServiceIDTDRSRef;
         obData1->tdrsDataFlag = obData1->tdrsDataFlagTDRSRef;
         obData1->tdrsSMARID = obData1->tdrsSMARIDTDRSRef;
         adapterTDRSRef->CalculateMeasurement(withEvents, obData1, rampTB, forSimulation);
      }
      else
         adapterTDRSRef->CalculateMeasurement(withEvents, forObservation,rampTB, forSimulation);
      measDataTDRSRef = adapterTDRSRef->GetMeasurement();
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation of the reference TDRS One Way Doppler measurement.\n");
#endif
      
      ////// 3.4. Remove obData object when it is not used
      ////if (obData)
      ////{
      ////   delete obData;
      ////   obData = NULL;
      ////}
      ////Real speedoflightkm = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;
      

      // 4. Set uplink frequency, uplink frequency band, node4 frequency, node4 fequency band for reference TDRS oneway doppler
      cMeasurement.uplinkFreq = measDataTDRSComp.uplinkFreq;         // unit is Hz
      cMeasurement.uplinkFreqAtRecei = measDataTDRSComp.uplinkFreqAtRecei;  // unit is Hz
      cMeasurement.uplinkBand = measDataTDRSComp.uplinkBand;
      cMeasurement.tdrsNode4Freq = measDataTDRSComp.tdrsNode4Freq;      // unit is Hz
      cMeasurement.tdrsNode4Band = measDataTDRSComp.tdrsNode4Band;
      cMeasurement.tdrsSMARID = measDataTDRSComp.tdrsSMARID;
      cMeasurement.tdrsServiceID = measDataTDRSComp.tdrsServiceID;
      cMeasurement.dopplerCountInterval = measDataTDRSComp.dopplerCountInterval;

      cMeasurement.uplinkFreqTDRSRef = measDataTDRSRef.uplinkFreq;         // unit is Hz
      cMeasurement.uplinkFreqAtReceiTDRSRef = measDataTDRSRef.uplinkFreqAtRecei;  // unit is Hz
      cMeasurement.uplinkBandTDRSRef = measDataTDRSRef.uplinkBand;
      cMeasurement.tdrsNode4FreqTDRSRef = measDataTDRSRef.tdrsNode4Freq;      // unit is Hz
      cMeasurement.tdrsNode4BandTDRSRef = measDataTDRSRef.tdrsNode4Band;
      cMeasurement.tdrsSMARIDTDRSRef = measDataTDRSRef.tdrsSMARID;
      cMeasurement.tdrsServiceIDTDRSRef = measDataTDRSRef.tdrsServiceID;
      cMeasurement.dopplerCountIntervalTDRSRef = measDataTDRSRef.dopplerCountInterval;

      //5. Calculate measurement value
      cMeasurement.value.clear();
      cMeasurement.correction.clear();

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      for (UnsignedInt i = 0; i < paths.size(); ++i)          // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths.
      {
         // 4.1. Get legs: 1 and 2:
         SignalBase* leg1 = paths[i];
         SignalBase* leg2 = leg1->GetNext();

         ////// 4.2. Specify TDRS id
         
         
         // 4.3. Calculate TDRS DOWD value and correction (unit: Hz)         
         cMeasurement.value.push_back(multiplierTDRSComp*measDataTDRSComp.value[i] + multiplierTDRSRef*measDataTDRSRef.value[i]);
         cMeasurement.correction.push_back(multiplierTDRSComp*measDataTDRSComp.correction[i] + multiplierTDRSRef*measDataTDRSRef.correction[i]);

         // 4.4. Update media corrections
         cMeasurement.ionoCorrectValue = multiplierTDRSComp*adapterTDRSComp->GetIonoCorrection() + multiplierTDRSRef*adapterTDRSRef->GetIonoCorrection();
         cMeasurement.tropoCorrectValue = multiplierTDRSComp*adapterTDRSComp->GetTropoCorrection() + multiplierTDRSRef*adapterTDRSRef->GetTropoCorrection();

         // 4.5. Specify measurement feasibility
         cMeasurement.isFeasible = true;
         cMeasurement.unfeasibleReason = "N";
         cMeasurement.feasibilityValue = measDataTDRSComp.feasibilityValue;
         
         if (!measDataTDRSRef.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataTDRSRef.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataTDRSRef.feasibilityValue;
         }
         if (!measDataTDRSComp.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataTDRSComp.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataTDRSComp.feasibilityValue;
         }
         
         //Handle HORP
         cMeasurement.horpHeight = GmatMathUtil::Min(cMeasurement.horpHeight, measDataTDRSComp.horpHeight);
         cMeasurement.horpHeight = GmatMathUtil::Min(cMeasurement.horpHeight, measDataTDRSRef.horpHeight);
         cMeasurement.horpAngle = GmatMathUtil::Max(cMeasurement.horpAngle, measDataTDRSComp.horpAngle);
         cMeasurement.horpAngle = GmatMathUtil::Max(cMeasurement.horpAngle, measDataTDRSRef.horpAngle);

         // 4.6. Add noise and bias
         Real C_idealVal = cMeasurement.value[i];

         if (measurementType == "SN_DOWD")
         {
            // Compute bias
            ComputeMeasurementBias("Bias", "SN_DOWD", 2);
            // Compute noise sigma
            ComputeMeasurementNoiseSigma("NoiseSigma", "SN_DOWD", 2);
            // Compute measurement error covariance matrix
            ComputeMeasurementErrorCovarianceMatrix();

            cMeasurement.covariance = &measErrorCovariance;

            // if need range value only, skip this section, otherwise add noise and bias as possible
            if (!rangeOnly)
            {
               // Add noise to measurement value
               if (addNoise)
               {
                  // Add noise here
                  if (cMeasurement.unfeasibleReason != "R")
                  {
                     RandomNumber* rn = RandomNumber::Instance();
                     Real val = rn->Gaussian(0.0, noiseSigma[i]);
                     cMeasurement.value[i] = cMeasurement.value[i] + val;
                     cMeasurement.correction[i] = cMeasurement.correction[i] + val;
                  }
               }

               //Add bias to measurement value only after noise had been added in order to avoid adding bias' noise
               if (addBias)
               {
                  #ifdef DEBUG_RANGE_CALCULATION
                     MessageInterface::ShowMessage("      . Add bias...\n");
                  #endif
                  if (biasType == BIASTYPE_IS_PASSBIAS) //(timeGapForPassBreak > 0.0)
                  {
                     // add pass bias
                     // specify pass number associate to the current observation
                     Real passBias = measErrorModel->GetPassBias(cMeasurement.epochGT);
                     cMeasurement.value[i] = cMeasurement.value[i] + passBias;                       // pass bias unit: Km/s
                     cMeasurement.correction[i] = cMeasurement.correction[i] + passBias;             // pass bias unit: Km/s
                  }
                  else
                  {
                     // add bias
                     cMeasurement.value[i] = cMeasurement.value[i] + measurementBias[i];             // bias unit: Km/s
                     cMeasurement.correction[i] = cMeasurement.correction[i] + measurementBias[i];   // bias unit: Km/s
                  }
               }
            }
         }


         #ifdef DEBUG_DOPPLER_CALCULATION
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("====  TDRSDOWDAdapter: Range Calculation for Measurement Data %dth  \n", i);
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("      . Measurement epoch             : %.12lf\n", cMeasurement.epochGT.GetMjd());
            MessageInterface::ShowMessage("      . Measurement type              : <%s>\n", measurementType.c_str());
            MessageInterface::ShowMessage("      . Noise adding option           : %s\n", (addNoise?"true":"false"));
            MessageInterface::ShowMessage("      . For comparison TDRS One Way Doppler measurement:\n");
            MessageInterface::ShowMessage("           *Doppler count interval    : %.12lf seconds\n", measDataTDRSComp.dopplerCountInterval);
            MessageInterface::ShowMessage("           *Service access            : %s\n", measDataTDRSComp.tdrsServiceID.c_str()); // serviceAccessListTDRSComp[adapterTDRSComp->serviceAccessIndex].c_str());
            MessageInterface::ShowMessage("           *Node 4 (effect) frequency : %.12lf MHz\n", measDataTDRSComp.tdrsNode4Freq/1.0e6);
            MessageInterface::ShowMessage("           *Node 4 frequency band     : %d\n", measDataTDRSComp.tdrsNode4Band);
            //MessageInterface::ShowMessage("           *TDRS id                   : %s\n", measDataTDRSComp.tdrsId.c_str());
            MessageInterface::ShowMessage("           *SMAR id                   : %d\n", measDataTDRSComp.tdrsSMARID);
            MessageInterface::ShowMessage("           *Data Flag                 : %d\n", measDataTDRSComp.tdrsDataFlag);
            //MessageInterface::ShowMessage("           *Pilot frequency           : %.12lf MHz\n", pilotFreq);
            MessageInterface::ShowMessage("           *TDRS one way doppler      : %.12lf Km\n", measDataTDRSComp.value[i]);

            MessageInterface::ShowMessage("      . For reference TDRS One Way Doppler measurement:\n");
            MessageInterface::ShowMessage("           *Doppler count interval    : %.12lf seconds\n", measDataTDRSRef.dopplerCountInterval);
            MessageInterface::ShowMessage("           *Service access            : %s\n", measDataTDRSRef.tdrsServiceID.c_str()); // serviceAccessListTDRSComp[adapterTDRSComp->serviceAccessIndex].c_str());
            MessageInterface::ShowMessage("           *Node 4 (effect) frequency : %.12lf MHz\n", measDataTDRSRef.tdrsNode4Freq / 1.0e6);
            MessageInterface::ShowMessage("           *Node 4 frequency band     : %d\n", measDataTDRSRef.tdrsNode4Band);
            //MessageInterface::ShowMessage("           *TDRS id                   : %s\n", measDataTDRSRef.tdrsId.c_str());
            MessageInterface::ShowMessage("           *SMAR id                   : %d\n", measDataTDRSRef.tdrsSMARID);
            MessageInterface::ShowMessage("           *Data Flag                 : %d\n", measDataTDRSRef.tdrsDataFlag);
            //MessageInterface::ShowMessage("           *Pilot frequency           : %.12lf MHz\n", pilotFreq);
            MessageInterface::ShowMessage("           *TDRS one way doppler      : %.12lf Km\n", measDataTDRSComp.value[i]);

            MessageInterface::ShowMessage("      . C-value w/o noise and bias    : %.12lf Hz\n", C_idealVal);

            if (measurementType == "SN_DOWD")
            {
               MessageInterface::ShowMessage("      . TDRSS DOWD noise sigma        : %.12lf Hz \n", noiseSigma[i]);
               MessageInterface::ShowMessage("      . TDRSS DOWD bias               : %.12lf Hz \n", measurementBias[i]);
            }

            MessageInterface::ShowMessage("      . C-value with noise and bias : %.12lf Hz\n", cMeasurement.value[i]);
            MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.12lf\n", cMeasurement.epoch);
            MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
            MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
            MessageInterface::ShowMessage("      . Elevation angle             : %.12lf degree\n", cMeasurement.feasibilityValue);
            MessageInterface::ShowMessage("      . Covariance matrix           : <%p>\n", cMeasurement.covariance);
            if (cMeasurement.covariance)
            {
               MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
               MessageInterface::ShowMessage("     [ ");
               for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
               {
                  if ( i > 0)
                     MessageInterface::ShowMessage("\n");
                  for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
                     MessageInterface::ShowMessage("%lf   ", cMeasurement.covariance->GetCovariance()->GetElement(i,j));
               }
               MessageInterface::ShowMessage("]\n");
            }
            MessageInterface::ShowMessage("===================================================================\n");
         #endif

      }
	  if (!calcData->CheckEpochValidity(computeEpoch))
	  {
		  epochValid = false;
	  }
   }
   else // The requested epoch is not supported - likely not on a TDRSS 3L Return Doppler ephem
   {
      epochValid = false;
   }

   if (!epochValid)
   {
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "EGAP";
   }



   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage(
         "TDRSDOWDAdapter::CalculateMeasurement(%s, <%p>, <%p>)"
         " exit\n", (withEvents ? "true" : "false"), forObservation, rampTable);
   #endif

   return cMeasurement;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//       Integer id)
//------------------------------------------------------------------------------
/**
 * Computes measurement derivatives for a given parameter on a given object
 *
 * @param obj The object that has the w.r.t. parameter
 * @param id  The ID of the w.r.t. parameter
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& 
TDRSDOWDAdapter::CalculateMeasurementDerivatives(GmatBase* obj, 
   Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   Integer parmId = GetParmIdFromEstID(id, obj);
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage(
         "Enter TDRSDOWDAdapter::CalculateMeasurementDerivatives"
         "(%s, %d) called; parm ID is %d; Epoch %.12lf\n", 
         obj->GetFullName().c_str(), id, parmId, cMeasurement.epoch);
   #endif
   
   // Get parameter name specified by id
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; // GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", 
         paramName.c_str());
   #endif

   // Clear derivative variable:
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();
   
   if (paramName == "Bias")
   {
      // Check added for multi-SC bias estimation. Checker sees if participants in the measurement are in the bias being solved for, and returns the value if so.
      Integer checker = 0;
      std::string name = obj->GetFullName();

      // For example, if solve-for object's name is 'P0.P1_ErrorModelName'.                                         // made changes by TUAN NGUYEN
      // That means: solver-for variable Bias' is related to the signal path ('P0', 'P1', ... 'Pn').                // made changes by TUAN NGUYEN
      // Ideally, the name should be 'P0.P1_P2_...._Pn_ErrorModelName'. It should be contain the name               // made changes by TUAN NGUYEN
      // of all participants in the signal path. It is a long name. Therefore, we need to make it                   // made changes by TUAN NGUYEN
      // shorter by take the names of first 2 participants. As a result, the name is 'P0.P1_ErrorModelName'.        // made changes by TUAN NGUYEN

      // We need to take derivative the measurement value of that path w.r.t. 'Bias' parameter in                   // made changes by TUAN NGUYEN
      // 'ErrorModelName' object. Before taking derivative, it needs to check participant[0] to be 'P0' and         // made changes by TUAN NGUYEN
      // participant[1] = 'P1' and so on, then calculate derivative. If its is not matched set 0 to derivative      // made changes by TUAN NGUYEN
      // because the measurement value of that signal path is independent of the solver-for bias.                   // made changes by TUAN NGUYEN

      StringArray *nameList = participantLists[0];
      for (Integer indx = 0; indx < nameList->size(); indx++)
      {
         size_t pos = name.find(nameList->at(indx));                                                                // made changes by TUAN NGUYEN
         //if (name.rfind(nameList->at(indx)) != std::string::npos)                                                 // made changes by TUAN NGUYEN
         if (pos != std::string::npos)
         {
            //std::string fullCheck = name.substr(name.rfind(nameList->at(indx)) + nameList->at(indx).length(),1);  // made changes by TUAN NGUYEN
            std::string fullCheck = name.substr(pos + nameList->at(indx).length(), 1);                              // made changes by TUAN NGUYEN
            if (fullCheck == "_" || fullCheck == ".")
            {
               // name.erase(name.rfind(nameList->at(indx)), nameList->at(indx).length());                          // made changes by TUAN NGUYEN
               name.erase(pos, nameList->at(indx).length() + 1);     // It needs to remove "_" or "." as well       // made changes by TUAN NGUYEN
               checker++;
            }
         }
      }

      if (((ErrorModel*)obj)->GetStringParameter("Type") == "SN_DOWD" && checker > 1)
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id);
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }
   }
   else if (paramName == "PassBiases")
   {
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "SN_DOWD")
      {
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id, &cMeasurement.epochGT);
      }
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }
   }
   else if (paramName == "FrequencyBias")
   {
      Integer size = obj->GetEstimationParameterSize(id);
      RealArray oneRow;
      oneRow.assign(size, 0.0);
      theDataDerivatives.push_back(oneRow);
   }
   else
   {

      // Perform the calculations
      // Derivative for comparison TDRS One Way Doppler measuremnt
      const std::vector<RealArray> *derivativeDataTDRSComp =
         &(adapterTDRSComp->CalculateMeasurementDerivatives(obj, id));
      // Derivative for reference TDRS One Way Doppler measurement
      const std::vector<RealArray> *derivativeDataTDRSRef =
         &(adapterTDRSRef->CalculateMeasurementDerivatives(obj, id));

      // copy SSC, SGS, ESC, and EGS paths' derivatives
      UnsignedInt size = derivativeDataTDRSComp->at(0).size();
      std::vector<RealArray> derivativesTDRSComp;
      for (UnsignedInt i = 0; i < derivativeDataTDRSComp->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesTDRSComp.push_back(oneRow);

         if (derivativeDataTDRSComp->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesTDRSComp[i][j] = (derivativeDataTDRSComp->at(i))[j];
         }
      }
   
      size = derivativeDataTDRSRef->at(0).size();
      std::vector<RealArray> derivativesTDRSRef;
      for (UnsignedInt i = 0; i < derivativeDataTDRSRef->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesTDRSRef.push_back(oneRow);

         if (derivativeDataTDRSRef->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesTDRSRef[i][j] = (derivativeDataTDRSRef->at(i))[j];
         }
      }

      #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives of comparison TDRS One Way Doppler measurement: [");
      for (UnsignedInt i = 0; i < derivativeDataTDRSComp->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataTDRSComp->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataTDRSComp->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives of reference TDRS One Way Doppler measurement: [");
      for (UnsignedInt i = 0; i < derivativeDataTDRSRef->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataTDRSRef->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataTDRSRef->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      #endif

      // Now assemble the derivative data into the requested derivative
      size = derivativesTDRSComp[0].size();               // This is the size of derivative vector for signal path 0
      
      for (UnsignedInt i = 0; i < derivativesTDRSComp.size(); ++i)
      {
         // For each signal path, do the following steps:
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);

         if (derivativesTDRSComp[i].size() != size)
            throw MeasurementException("Derivative data size for comparison TDRS One Way Doppler measurement is a different size "
               "than expected");
         if (derivativesTDRSRef[i].size() != size)
            throw MeasurementException("Derivative data size for reference TDRS One Way Doppler measurement is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            if ((paramName == "Position")||(paramName == "Velocity")||(paramName == "CartesianX"))
            {
               // Convert measurement derivatives from km/s to Hz for velocity and position 
               theDataDerivatives[i][j] = + derivativesTDRSComp[i][j] * multiplierTDRSComp
                                          + derivativesTDRSRef[i][j] * multiplierTDRSRef;
            }
            else
            {
               // set the same End-Spacecraft path's derivatives for Bias an other solve-for variables
               theDataDerivatives[i][j] = derivativesTDRSComp[i][j];
            }
         }
      }
   }

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         MessageInterface::ShowMessage("Derivative for path %dth:\n", i);
         MessageInterface::ShowMessage("[");
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            MessageInterface::ShowMessage("    %.12lf", theDataDerivatives[i][j]);
            MessageInterface::ShowMessage("%s", ((j == theDataDerivatives[i].size()-1)?"":","));
         }
         MessageInterface::ShowMessage("]\n");
      }
      
   #endif

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Exit TDRSDOWDAdapter::CalculateMeasurementDerivatives():\n");
   #endif

   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo Implement this method
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool TDRSDOWDAdapter::WriteMeasurements()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a specific measurement
 *
 * @todo Implement this method
 *
 * @param id ID of the parameter to write
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool TDRSDOWDAdapter::WriteMeasurement(Integer id)
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
Integer TDRSDOWDAdapter::HasParameterCovariances(Integer parameterId)
{
   Integer retval = 0;

   return retval;
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
Integer TDRSDOWDAdapter::GetEventCount()
{
   Integer retval = 0;

   return retval;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string& correctionName,
//       const std::string& correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @note This information originally was not passed via SetStringParameter
 *       because it wasn't managed by scripting on MeasurementModels.  It was
 *       managed in the TrackingSystem code.  Since it is now scripted on the
 *       measurement -- meaning on the adapter -- this code should move into the
 *       Get/SetStringParameter methods.  It is included here !!TEMPORARILY!!
 *       to get a scripted adapter functioning in GMAT Nav.
 */
//------------------------------------------------------------------------------
void TDRSDOWDAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   adapterTDRSComp->SetCorrection(correctionName, correctionType);
   adapterTDRSRef->SetCorrection(correctionName, correctionType);
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);
}


//------------------------------------------------------------------------------
// Real GetTurnAroundRatio(Integer freqBand)
//------------------------------------------------------------------------------
/**
 * Retrieves turn around ratio
 *
 * @param freqBand   frequency band
 *
 * return   the value of trun around ratio associated with frequency band 
 */
//------------------------------------------------------------------------------
Real TDRSDOWDAdapter::GetTurnAroundRatio(Integer freqBand)
{
   switch (freqBand)
   {
      case 1:            // for S-band, turn around ratio is 240/221
         return 240.0/221.0;
      case 2:            // for X-band, turn around ratio is 880/749
         return 880.0/749.0;
   }

   // Display an error message when frequency band is not specified 
   std::stringstream ss;
   ss << "Error: frequency band " << freqBand << " is not specified.\n";
   throw MeasurementException(ss.str());
}

