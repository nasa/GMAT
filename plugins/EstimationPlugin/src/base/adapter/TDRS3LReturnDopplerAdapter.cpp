//------------------------------------------------------------------------------
//                           TDRS3LReturnDopplerAdapter
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
// Created: August 13, 2015
/**
 * A measurement adapter for BRTS Doppler   (unit: Hz)
 */
//------------------------------------------------------------------------------

#include "TDRS3LReturnDopplerAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
//#include "Transponder.hpp"
#include "Transmitter.hpp"
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
TDRS3LReturnDopplerAdapter::PARAMETER_TEXT[TDRS3LReturnDopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   "DopplerCountInterval",
   "ServiceAccess",
   "Node4Frequency",
   "Node4Band",
   "SmarId",
   "DataFlag",
};


const Gmat::ParameterType
TDRS3LReturnDopplerAdapter::PARAMETER_TYPE[TDRS3LReturnDopplerAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,                    // DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,             // SERVICE_ACCESS
   Gmat::REAL_TYPE,                    // NODE4_FREQUENCY
   Gmat::INTEGER_TYPE,                 // NODE4_BAND
   Gmat::INTEGER_TYPE,                 // SMAR_ID
   Gmat::INTEGER_TYPE,                 // DATA_FLAG
};




//------------------------------------------------------------------------------
// TDRS3LReturnDopplerAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
TDRS3LReturnDopplerAdapter::TDRS3LReturnDopplerAdapter(const std::string& name) :
   RangeAdapterKm         (name),
   adapterSSC             (NULL),
   adapterSGS             (NULL),
   adapterESC             (NULL),
   adapterEGS             (NULL),
   dopplerCountInterval   (1.0),                     // 1 second
   node4Freq              (2000.0),                  // 2000 Mhz
   node4FreqBand          (1),                       // 0: undefined, 1: S-band, 2: X-band, 3: K-band
   smarId                 (0),
   serviceAccessIndex     (0),
   frequencyMixer_a       (1.0),
   dataFlag               (0)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter default constructor <%p>\n", this);
#endif
   typeName = "SN_Doppler_Rtn";
   parameterCount = TDRS3LReturnDopplerAdapterParamCount;
   
   // TDRS 3L Return Doppler measurement model (Its tracking config is {{userSC, TDRS, GS1}, 'SN_Doppler_Rtn'}) has 2 signal paths:
   // First signal path: userSC-->TDRS-->GS1
   // Second signal path: GS1-->TDRS-->GS1
   measParticipantIndex = 2;
}


//------------------------------------------------------------------------------
// ~TDRS3LReturnDopplerAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TDRS3LReturnDopplerAdapter::~TDRS3LReturnDopplerAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter default destructor <%p>\n", this);
#endif

   if (adapterSSC)
      delete adapterSSC;
   if (adapterSGS)
      delete adapterSGS;
   if (adapterESC)
      delete adapterESC;
   if (adapterEGS)
      delete adapterEGS;
}


//------------------------------------------------------------------------------
// TDRS3LReturnDopplerAdapter(const TDRS3LReturnDopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param da The adapter copied to make this one
 */
//------------------------------------------------------------------------------
TDRS3LReturnDopplerAdapter::TDRS3LReturnDopplerAdapter(const TDRS3LReturnDopplerAdapter& da) :
   RangeAdapterKm         (da),
   adapterSSC             (NULL),
   adapterSGS             (NULL),
   adapterESC             (NULL),
   adapterEGS             (NULL),
   dopplerCountInterval   (da.dopplerCountInterval),
   serviceAccessList      (da.serviceAccessList),
   node4Freq              (da.node4Freq),
   node4FreqBand          (da.node4FreqBand),
   smarId                 (da.smarId),
   dataFlag               (da.dataFlag)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter copy constructor   from <%p> to <%p>\n", &da, this);
#endif
   if (da.adapterESC)
      adapterESC = (RangeAdapterKm*)(da.adapterESC->Clone());
   if (da.adapterEGS)
      adapterEGS = (RangeAdapterKm*)(da.adapterEGS->Clone());
   if (da.adapterSSC)
      adapterSSC = (RangeAdapterKm*)(da.adapterSSC->Clone());
   if (da.adapterSGS)
      adapterSGS = (RangeAdapterKm*)(da.adapterSGS->Clone());
   serviceAccessIndex = da.serviceAccessIndex;
   frequencyMixer_a = 1.0;
}


//------------------------------------------------------------------------------
// TDRS3LReturnDopplerAdapter& operator=(const TDRS3LReturnDopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param da The adapter copied to make this one match it
 *
 * @return This adapter made to look like da
 */
//------------------------------------------------------------------------------
TDRS3LReturnDopplerAdapter& TDRS3LReturnDopplerAdapter::operator=(const TDRS3LReturnDopplerAdapter& da)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter operator =   set <%p> = <%p>\n", this, &da);
#endif

   if (this != &da)
   {
      RangeAdapterKm::operator=(da);

      dopplerCountInterval = da.dopplerCountInterval;
      serviceAccessList    = da.serviceAccessList;
      node4Freq            = da.node4Freq;
      node4FreqBand        = da.node4FreqBand;
      smarId               = da.smarId;
      dataFlag             = da.dataFlag;
      serviceAccessIndex   = da.serviceAccessIndex;
      if (adapterSSC)
      {
         delete adapterSSC;
         adapterSSC = NULL;
      }
      if (da.adapterSSC)
         adapterSSC = (RangeAdapterKm*)da.adapterSSC->Clone();

      if (adapterSGS)
      {
         delete adapterSGS;
         adapterSGS = NULL;
      }
      if (da.adapterSGS)
         adapterSGS = (RangeAdapterKm*)da.adapterSGS->Clone();

      if (adapterESC)
      {
         delete adapterESC;
         adapterESC = NULL;
      }
      if (da.adapterESC)
         adapterESC = (RangeAdapterKm*)da.adapterESC->Clone();

      if (adapterEGS)
      {
         delete adapterEGS;
         adapterEGS = NULL;
      }
      if (da.adapterEGS)
         adapterEGS = (RangeAdapterKm*)da.adapterEGS->Clone();

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
void TDRS3LReturnDopplerAdapter::SetSolarSystem(SolarSystem *ss)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter<%p>::SetSolarSystem('%s')\n", this, ss->GetName().c_str()); 
#endif

   adapterSSC->SetSolarSystem(ss);
   adapterSGS->SetSolarSystem(ss);
   adapterESC->SetSolarSystem(ss);
   adapterEGS->SetSolarSystem(ss);

   RangeAdapterKm::SetSolarSystem(ss);
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
GmatBase* TDRS3LReturnDopplerAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter::Clone()   clone this <%p>\n", this);
#endif

   return new TDRS3LReturnDopplerAdapter(*this);
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
std::string TDRS3LReturnDopplerAdapter::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < TDRS3LReturnDopplerAdapterParamCount)
      return PARAMETER_TEXT[id - RangeAdapterKmParamCount];
   return RangeAdapterKm::GetParameterText(id);
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
Integer TDRS3LReturnDopplerAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < TDRS3LReturnDopplerAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RangeAdapterKmParamCount])
         return i;
   }
   return RangeAdapterKm::GetParameterID(str);
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
Gmat::ParameterType TDRS3LReturnDopplerAdapter::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < TDRS3LReturnDopplerAdapterParamCount)
      return PARAMETER_TYPE[id - RangeAdapterKmParamCount];

   return RangeAdapterKm::GetParameterType(id);
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
std::string TDRS3LReturnDopplerAdapter::GetParameterTypeString(const Integer id) const
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
bool TDRS3LReturnDopplerAdapter::SetStringParameter(const Integer id, const std::string& value)
{
   bool retval = true;
   if ((id != MEASUREMENT_TYPE)&&(id != SIGNAL_PATH))
   {
      retval = adapterSSC->SetStringParameter(id, value) && retval;
      retval = adapterSGS->SetStringParameter(id, value) && retval;
      retval = adapterESC->SetStringParameter(id, value) && retval;
      retval = adapterEGS->SetStringParameter(id, value) && retval;
   }

   retval = RangeAdapterKm::SetStringParameter(id, value) && retval;

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
bool TDRS3LReturnDopplerAdapter::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if (id == SERVICE_ACCESS)
   {
      if (((Integer)serviceAccessList.size() > index) && (index >= 0))
         serviceAccessList[index] = value;
      else if ((Integer)serviceAccessList.size() == index)
         serviceAccessList.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
            "set a service access name");

      return true;
   }

   bool retval = true;
   retval = adapterSSC->SetStringParameter(id, value, index) && retval;
   retval = adapterSGS->SetStringParameter(id, value, index) && retval;
   retval = adapterESC->SetStringParameter(id, value, index) && retval;
   retval = adapterEGS->SetStringParameter(id, value, index) && retval;

   retval = RangeAdapterKm::SetStringParameter(id, value, index) && retval;

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
std::string TDRS3LReturnDopplerAdapter::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == SERVICE_ACCESS)
   {
      if ((0 <= index)&&(index < serviceAccessList.size()))
         return serviceAccessList[index];
      else
      {
         std::stringstream ss;
         ss << "Error: index (" << index << ") is out of bound (" << serviceAccessList.size() << ")\n";
         throw MeasurementException(ss.str());
      }
   }

   return RangeAdapterKm::GetStringParameter(id, index);
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
bool TDRS3LReturnDopplerAdapter::SetStringParameter(const std::string& label,
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
bool TDRS3LReturnDopplerAdapter::SetStringParameter(const std::string& label,
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
std::string TDRS3LReturnDopplerAdapter::GetStringParameter(const std::string& label,
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
Integer TDRS3LReturnDopplerAdapter::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == NODE4_BAND)
   {
      // @todo: verify a valid frequency band
      node4FreqBand = value;
      return true;
   }

   if (id == SMAR_ID)
   {
      // @todo: verify a valid SMAR id
      smarId = value;
      return true;
   }

   if (id == DATA_FLAG)
   {
      // @todo: verify a valid data flag
      dataFlag = value;
      return true;
   }


   adapterSSC->SetIntegerParameter(id, value);
   adapterSGS->SetIntegerParameter(id, value);
   adapterESC->SetIntegerParameter(id, value);
   adapterEGS->SetIntegerParameter(id, value);

   return RangeAdapterKm::SetIntegerParameter(id, value);
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
Integer TDRS3LReturnDopplerAdapter::GetIntegerParameter(const Integer id) const
{
   if (id == NODE4_BAND)
      return node4FreqBand;

   if (id == SMAR_ID)
      return smarId;

   if (id == DATA_FLAG)
      return dataFlag;

   return RangeAdapterKm::GetIntegerParameter(id);
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
Integer TDRS3LReturnDopplerAdapter::SetIntegerParameter(const std::string &label, const Integer value)
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
Integer TDRS3LReturnDopplerAdapter::GetIntegerParameter(const std::string &label) const
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
Real TDRS3LReturnDopplerAdapter::GetRealParameter(const Integer id) const
{
   if (id == DOPPLER_COUNT_INTERVAL)
      return dopplerCountInterval;

   if (id == NODE4_FREQUENCY)
      return node4Freq;

   return RangeAdapterKm::GetRealParameter(id);
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
Real TDRS3LReturnDopplerAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler count interval has a nonpositive value\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
   }

   if (id == NODE4_FREQUENCY)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: TDRS node 4 frequency has to be a positive number\n");
      node4Freq = value;
      return node4Freq;
   }

   adapterSSC->SetRealParameter(id, value);
   adapterSGS->SetRealParameter(id, value);
   adapterESC->SetRealParameter(id, value);
   adapterEGS->SetRealParameter(id, value);

   Real retval = RangeAdapterKm::SetRealParameter(id, value);

   return retval;
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
Real TDRS3LReturnDopplerAdapter::GetRealParameter(const std::string &label) const
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
Real TDRS3LReturnDopplerAdapter::SetRealParameter(const std::string &label, const Real value)
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
bool TDRS3LReturnDopplerAdapter::SetBooleanParameter(const Integer id, const bool value)
{
   // Note: for SL, SS, and ES paths, AddNoise always is set to false due to it calculation
   if (id == ADD_NOISE)
   {
      adapterSSC->SetBooleanParameter(id, false);
      adapterSGS->SetBooleanParameter(id, false);
      adapterESC->SetBooleanParameter(id, false);
      adapterEGS->SetBooleanParameter(id, false);
   }
   else
   {
      adapterSSC->SetBooleanParameter(id, value);
      adapterSGS->SetBooleanParameter(id, value);
      adapterESC->SetBooleanParameter(id, value);
      adapterEGS->SetBooleanParameter(id, value);
   }

   return RangeAdapterKm::SetBooleanParameter(id, value);

}


bool TDRS3LReturnDopplerAdapter::SetBooleanParameter(const std::string &label, const bool value)
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
const StringArray& TDRS3LReturnDopplerAdapter::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SERVICE_ACCESS)
      return serviceAccessList;

   return RangeAdapterKm::GetStringArrayParameter(id);
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
bool TDRS3LReturnDopplerAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   // Handle additional renames specific to this adapter
   bool retval = true;
   retval = adapterSSC->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterSGS->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterESC->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterEGS->RenameRefObject(type, oldName, newName) && retval;

   retval = RangeAdapterKm::RenameRefObject(type, oldName, newName) && retval;

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
bool TDRS3LReturnDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   bool retval = true;
   retval = adapterSSC->SetRefObject(obj, type, name) && retval;
   retval = adapterSGS->SetRefObject(obj, type, name) && retval;
   retval = adapterESC->SetRefObject(obj, type, name) && retval;
   retval = adapterEGS->SetRefObject(obj, type, name) && retval;

   retval = RangeAdapterKm::SetRefObject(obj, type, name) && retval;

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
bool TDRS3LReturnDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   bool retval = true;
   retval = adapterSSC->SetRefObject(obj, type, name, index) && retval;
   retval = adapterSGS->SetRefObject(obj, type, name, index) && retval;
   retval = adapterESC->SetRefObject(obj, type, name, index) && retval;
   retval = adapterEGS->SetRefObject(obj, type, name, index) && retval;

   retval = RangeAdapterKm::SetRefObject(obj, type, name, index) && retval;

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
bool TDRS3LReturnDopplerAdapter::SetMeasurement(MeasureModel* meas)
{
   return RangeAdapterKm::SetMeasurement(meas);
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
void TDRS3LReturnDopplerAdapter::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting propagator to %p in "
            "TDRS3LReturnDopplerAdapter\n", ps);
   #endif

   adapterSSC->SetPropagators(ps, spMap);
   adapterSGS->SetPropagators(ps, spMap);
   adapterESC->SetPropagators(ps, spMap); 
   adapterEGS->SetPropagators(ps, spMap);

   RangeAdapterKm::SetPropagators(ps, spMap);
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
void TDRS3LReturnDopplerAdapter::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    GetMeasurementModel()->SetTransientForces(tf);
    adapterSSC->SetTransientForces(tf);
    adapterSGS->SetTransientForces(tf);
    adapterESC->SetTransientForces(tf);
    adapterEGS->SetTransientForces(tf);
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
bool TDRS3LReturnDopplerAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter::Initialize() <%p> start\n", this);
#endif

   bool retval = false;

   // measParticipantIndex is index of the participant which is used for measurement. 
   // The time tag associated to that participant is the measurement time tag tm.
   // For TDRS 3L Return Doppler measurement, tracking config is {{userSC, TDRS, GS1, GS2}, 'SN_Doppler_Rtn'}
   // It has 2 paths: userSC-->TDRS-->GS1 and GS2-->TDRS-->GS1. 
   // In the above tracking config, index of participants are 0 for userSC, 1 for TDRS, 2 for GS1, and 3 for GS2.
   // The ground station GS1 is used to for measurement. Therefore, measParticipantIndex = 2 for this case.
   // If the measuring participant is the last participant in the config list, measParticipantIndex is set to -1.
   calcData->measParticipantIndex = measParticipantIndex;

   if (RangeAdapterKm::Initialize())
   {
      // @todo: initialize all needed variables
      //if (serviceAccessList.size() == 0)
      //   throw MeasurementException("Error: no TDRS service access was set for measurement\n");

      //Set up HORP for spacecraft legs
      serviceAccessIndex = 0;
      if (useHorp || forceHorp)
      {
         adapterSSC->SetupHorp(horpHeight, horpAngle, horpOblateness, forceHorp);
         adapterESC->SetupHorp(horpHeight, horpAngle, horpOblateness, forceHorp);
      }

      retval = adapterSSC->Initialize();
      retval = adapterSGS->Initialize() && retval;
      retval = adapterESC->Initialize() && retval;
      retval = adapterEGS->Initialize() && retval;

      //One way data requires ET-TAI for spacecraft legs, set even if User has not
      adapterSSC->GetMeasurementModel()->AddCorrection("Moyer", "ET-TAI");
      adapterESC->GetMeasurementModel()->AddCorrection("Moyer", "ET-TAI");

   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter::Initialize() <%p> exit\n", this);
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
const MeasurementData& TDRS3LReturnDopplerAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable,
      bool forSimulation)
{
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("TDRS3LReturnDopplerAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   bool epochValid = true;

   GmatTime computeEpoch;
   if (forObservation)
   {
      computeEpoch = forObservation->epochGT;
   }
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
         dopplerCountInterval = obsData->dopplerCountInterval;          // unit: second
         serviceAccessList.clear();
         serviceAccessList.push_back(obsData->tdrsServiceID);           // SA1, SA2, or MA
         serviceAccessIndex = 0;

         node4Freq     = obsData->tdrsNode4Freq/1.0e6;                  // unit: MHz
         node4FreqBand = obsData->tdrsNode4Band;                        // 0: unspecified, 1: S-band, 2: X-band, 3: K-band

         dataFlag      = obsData->tdrsDataFlag;
         smarId        = obsData->tdrsSMARID;
      }
      else
      {
         // obsData == NULL That means simulation
         if (serviceAccessList.empty())
            throw MeasurementException("The simulator TDRS service access list for TDRS measurements is empty. Please add a valid service (SA1, SA2, or MA) to the list.\n");
         // Specify node 4 frequency and band

         //node4Freq = adapterESC->GetMeasurementModel()->GetTransmitFrequency(0, 0);      // unit: MHz
         //if (forObservation)
            //node4FreqBand = obsData->tdrsNode4Band;
         //else
            //node4FreqBand = adapterESC->GetMeasurementModel()->GetTransmitFrequencyBand(0, 0);      // 0: unspecified, 1: S-band, 2: X-band, 3: K-band
         srand(time(NULL));
         serviceAccessIndex = rand() % serviceAccessList.size();
      }

      if (forSimulation)
      {
         if (serviceAccessList[serviceAccessIndex] == "MA")
         {
            if (smarId == 0)
               smarId = 1;
         }
         else if (serviceAccessList[serviceAccessIndex] == "SA1" || serviceAccessList[serviceAccessIndex] == "SA2")
         {
            smarId = 0;
         }
      }
      // 2. Compute range for End paths
      // 2.1. Propagate all space objects to time tm
      // This step is not needed due to measurement time tm is set to t3RE
      // 2.2. Compute range in km for End paths
      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("Compute range for End Paths...\n");
      #endif
      bool addNoiseOption  = addNoise;
      bool addBiasOption   = addBias;
      bool rangeOnlyOption = rangeOnly;
      bool withMedia = withMediaCorrection;

      // 2.2.1. Compute range in km w/o any noise or bias for End-Long path
      //addNoise  = false;
      //addBias   = false;
      //rangeOnly = true;
      adapterESC->AddBias(false);
      adapterESC->AddNoise(false);
      adapterESC->SetRangeOnly(true);
      //adapterEGS->AddMediaCorrection(withMedia);
      adapterESC->AddMediaCorrection(withMedia);         // It has to be End Spacecarft (ESC) path

#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for End Spacecraft-Path...\n");
#endif
      //RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);
      adapterESC->CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for End Spacecraft-Path.\n");
#endif
      cMeasurement.epochGT = adapterESC->GetMeasurement().epochGT;

      // reset back to their orginal values
      addNoise  = addNoiseOption;
      addBias   = addBiasOption;
      rangeOnly = rangeOnlyOption;
      withMediaCorrection = withMedia;

      // 2.2.2. Specify uplink frequency
      // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0
      if (obsData)
      {
         uplinkFreq = node4Freq;       
         uplinkFreqAtRecei = node4Freq;    
         freqBand = node4FreqBand;            
      }
      else
      {
         // No ramp table is used for TDRSS 3L Return Doppler measurement  
         uplinkFreq = calcData->GetUplinkFrequency(0, NULL);                   
         uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, NULL);    
         freqBand = calcData->GetUplinkFrequencyBand(0, NULL);             
      }
      // 2.3.1. Measurement time is the same as the one for End-path
      GmatTime tm = cMeasurement.epochGT;                              // Get measurement time
      ObservationData* obData = NULL;
      if (forObservation)
         obData = new ObservationData(*forObservation);
      else
         obData = new ObservationData();
      obData->epochGT = tm;
      obData->epoch   = tm.GetMjd();

      // 2.3.2. Compute range in Km w/o any noise and bias for End-Short path
      // For End-GroundStation path, range calculation does not add bias and noise to calculated value
      adapterEGS->AddBias(false);
      adapterEGS->AddNoise(false);
      adapterEGS->SetRangeOnly(true);
      adapterEGS->AddMediaCorrection(withMedia);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for End GroundStation-Path...    adapterEGS = <%p>\n", adapterEGS);
#endif
      adapterEGS->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for End GroundStation-Path.\n");
#endif
   
      // 3. Compute for Start-Long and Start-Short paths w/o any noise and bias
      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("Compute range for Start Paths...\n");
      #endif

      // 3.1. Set doppler count interval to MeasureModel object due to
      // Start Spacecraft-path and Start GroundStation-path are measured earlier by number of
      // seconds shown in doppler count interval
      adapterSSC->GetMeasurementModel()->SetCountInterval(dopplerCountInterval);
      adapterSGS->GetMeasurementModel()->SetCountInterval(dopplerCountInterval);

      // 3.2. For Start Spacecraft-path, range calculation does not add bias and noise to calculated value
      // Note that: default option is no adding noise
      adapterSSC->AddBias(false);
      adapterSSC->AddNoise(false);
      adapterSSC->SetRangeOnly(true);
      adapterSSC->AddMediaCorrection(withMedia);
      adapterSGS->AddBias(false);
      adapterSGS->AddNoise(false);
      adapterSGS->SetRangeOnly(true);
      adapterSGS->AddMediaCorrection(withMedia);
   
      // 3.3. Compute range for Start Spacecraft-path and Start GroundStation-path
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for Start Spacecraft-Path...\n");
#endif
      adapterSSC->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for Start Spacecraft-Path.\n");
#endif
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for Start GroundStation-Path...\n");
#endif
      adapterSGS->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for Start GroundStation-Path.\n");
#endif

      adapterESC->AddMediaCorrection(withMedia);
      adapterEGS->AddMediaCorrection(withMedia);
      adapterSSC->AddMediaCorrection(withMedia);
      adapterSGS->AddMediaCorrection(withMedia);

      



      cMeasurement.tdrsNode4Freq = node4Freq * 1.0e6;        // unit Hz
      cMeasurement.tdrsNode4Band = node4FreqBand;
      cMeasurement.tdrsDataFlag = dataFlag;
      cMeasurement.tdrsSMARID = smarId;

      if (serviceAccessList.empty())
         throw MeasurementException("Error: No service access is specified for the measurement.\n");
      cMeasurement.tdrsServiceID = serviceAccessList[serviceAccessIndex];
      //MessageInterface::ShowMessage("service access = %s   node 4 frequency = %le Hz   node 4 band = %d    TDRS data flag = %d    SMAR id = %d\n",
      //   cMeasurement.tdrsServiceID.c_str(), cMeasurement.tdrsNode4//Freq, cMeasurement.tdrsNode4Band, cMeasurement.tdrsDataFlag, cMeasurement.tdrsSMARID);

      // 3.4. Remove obData object when it is not used
      if (obData)
      {
         delete obData;
         obData = NULL;
      }
      Real speedoflightkm = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;
      
      // 4. Calculate Doopler shift frequency (unit: Hz) based on range (unit: km) and store result to cMeasurement:
      cMeasurement.value.clear();
      cMeasurement.correction.clear();

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      for (UnsignedInt i = 0; i < paths.size(); ++i)          // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths.
      {
         // 4.1. Get legs: 1 and 2:
         SignalBase* leg1 = paths[i];
         SignalBase* leg2 = leg1->GetNext();

         // 4.2. Specify TDRS id
         SignalData* sd2 = leg2->GetSignalDataObject();
         std::string scId = sd2->tNode->GetStringParameter("Id");
         if (!GetTdrsIdFromSpacecraftId(scId))
         {
            if (warnTdrsId)
            {
               warnTdrsId = false;
               MessageInterface::ShowMessage("Warning: The TDRS ID for Spacecraft %s could not be determined, the default ID of %s will be used. Some Doppler measurements may be computed incorrectly.\n", sd2->tNode->GetName().c_str(), TDRSId.c_str());
            }
         }
         //MessageInterface::ShowMessage("TDRS id = %s\n", tdrsId.c_str());
         
         // 4.3. Specify effective frequency
         Real effFreq = node4Freq;
         
         // 4.4. Calculate uplink frequency based on Node 4 frequency:
         //uplinkFreq = node4Freq / totalTAR;                                                // It is transmit frequency from ground station:  f0 = A/(Ratio*a) with assumtion a = 1.0.
         //MessageInterface::ShowMessage("new uplinkFreq = %.12le   \n", uplinkFreq);

         // 4.6. Get measurement data for each path
         measDataESC = adapterESC->GetMeasurement();
         measDataEGS = adapterEGS->GetMeasurement();
         measDataSSC = adapterSSC->GetMeasurement();
         measDataSGS = adapterSGS->GetMeasurement();
         
         // 4.7. Specify pilot frequency
         Real pilotFreq;
         try
         {
            pilotFreq = CalculatePilotFrequency(effFreq, node4FreqBand, serviceAccessList[serviceAccessIndex], TDRSId, smarId, dataFlag);                        // unit: MHz
            dBdA = CalculatePilotFrequencyDerivativeWRTEffectiveFrequency(effFreq, node4FreqBand, serviceAccessList[serviceAccessIndex], TDRSId, smarId, dataFlag);
         }
         catch (MeasurementException &e)
         {
            std::string errmsg = "Error calculating TDRS Pilot frequency for TDRS: \"" + sd2->tNode->GetName() + "\". Error message found: \n   " + e.GetFullMessage().substr(30);
            throw MeasurementException(errmsg);
         }

         // 4.8. Specify multiplier for SSC-path, SGS-path, ESC-path, and EGS-path
         multiplierSSC = (effFreq*1.0e6) / (dopplerCountInterval*speedoflightkm);                                // unit: Hz/Km
         multiplierSGS = (pilotFreq*1.0e6)/(dopplerCountInterval*speedoflightkm);                                // unit: Hz/Km
         multiplierESC = -(effFreq*1.0e6) / (dopplerCountInterval*speedoflightkm);                               // unit: Hz/Km
         multiplierEGS = -(pilotFreq*1.0e6)/(dopplerCountInterval*speedoflightkm);                               // unit: Hz/Km
         //MessageInterface::ShowMessage("multiplierESC = %.12le   multiplierEGS = %.12le   multiplierSSC = %.12le   multiplierSGS = %.12le   \n", multiplierESC, multiplierEGS, multiplierSSC, multiplierSGS);

         // 4.9. Set uplink frequency, uplink frequency band, node4 frequency, node4 fequency band
         cMeasurement.uplinkFreq           = uplinkFreq*1.0e6;         // convert Mhz to Hz due cMeasurement.uplinkFreq's unit is Hz
         cMeasurement.uplinkFreqAtRecei    = uplinkFreqAtRecei*1.0e6;  // convert Mhz to Hz due cMeasurement.uplinkFreqAtRecei's unit is Hz
         cMeasurement.uplinkBand           = freqBand;
         cMeasurement.tdrsNode4Freq        = node4Freq*1.0e6;          // convert Mhz to Hz due cMeasurement.tdrsNode4Freq's unit is Hz
         cMeasurement.tdrsNode4Band        = node4FreqBand;
         cMeasurement.tdrsSMARID           = smarId;
         cMeasurement.tdrsServiceID        = serviceAccessList[serviceAccessIndex];
         cMeasurement.dopplerCountInterval = dopplerCountInterval;

         // 4.10. Calculate Frequency Doppler Shift
         // No doppler count correction here, so this code was changed to fix this bug
         //Real dopplerCountCorrection = dopplerCountInterval * speedoflightkm;
         //cMeasurement.value.push_back(
         //   multiplierESC * (measDataESC.value[i] / adapterESC->GetMultiplierFactor())
         //   + multiplierSSC * (measDataSSC.value[i] / adapterSSC->GetMultiplierFactor() - dopplerCountCorrection)
         //   + multiplierEGS * (measDataEGS.value[i] / adapterEGS->GetMultiplierFactor())
         //   + multiplierSGS * (measDataSGS.value[i] / adapterSGS->GetMultiplierFactor() - dopplerCountCorrection)
         //   );                                                                                                                   // unit: Hz    //(Equation 7-92 GTDS MathSpec)

         //cMeasurement.correction.push_back(
         //   multiplierESC * (measDataESC.correction[i] / adapterESC->GetMultiplierFactor())
         //   + multiplierSSC * (measDataSSC.correction[i] / adapterSSC->GetMultiplierFactor() - dopplerCountCorrection)
         //   + multiplierEGS * (measDataEGS.correction[i] / adapterEGS->GetMultiplierFactor())
         //   + multiplierSGS * (measDataSGS.correction[i] / adapterSGS->GetMultiplierFactor() - dopplerCountCorrection)
         //   );                                                                                                                  // unit: Hz    //(Equation 7-92 GTDS MathSpec)
         cMeasurement.value.push_back(
            multiplierESC * (measDataESC.value[i] / adapterESC->GetMultiplierFactor())
            + multiplierSSC * (measDataSSC.value[i] / adapterSSC->GetMultiplierFactor())
            + multiplierEGS * (measDataEGS.value[i] / adapterEGS->GetMultiplierFactor())
            + multiplierSGS * (measDataSGS.value[i] / adapterSGS->GetMultiplierFactor())
            );                                                                                                                   // unit: Hz    //(Equation 7-92 GTDS MathSpec)

         cMeasurement.correction.push_back(
            multiplierESC * (measDataESC.correction[i] / adapterESC->GetMultiplierFactor())
            + multiplierSSC * (measDataSSC.correction[i] / adapterSSC->GetMultiplierFactor())
            + multiplierEGS * (measDataEGS.correction[i] / adapterEGS->GetMultiplierFactor())
            + multiplierSGS * (measDataSGS.correction[i] / adapterSGS->GetMultiplierFactor())
            );                                                                                                                  // unit: Hz    //(Equation 7-92 GTDS MathSpec)


         // Negative of ratio between range rate for SC-path and speed of light: [-1/(c*Delta_t)]*Delta_rhoSC   (unit: (km/s)/(km/s) = 1)
         // Equations 2.14.1 and 2.14.2 in TDRSS and BTRS Math Specification 
         rangeRateSCRatio =  (-1.0/(dopplerCountInterval*speedoflightkm))* 
            (measDataESC.value[i] / adapterESC->GetMultiplierFactor() -
             measDataSSC.value[i] / adapterSSC->GetMultiplierFactor());

         // Negative of ratio between range rate for GS-path and speed of light: [-1/(c*Delta_t)]*Delta_rhoGS   (unit: (km/s)/(km/s) = 1)
         // Equations 2.14.1 and 2.14.2 in TDRSS and BTRS Math Specification 
         rangeRateGSRatio = (-1.0 / (dopplerCountInterval*speedoflightkm))*
            (measDataEGS.value[i] / adapterEGS->GetMultiplierFactor() -
               measDataSGS.value[i] / adapterSGS->GetMultiplierFactor());


         // Update media corrections
         cMeasurement.ionoCorrectValue = multiplierESC * adapterESC->GetIonoCorrection()
            + multiplierSSC * adapterSSC->GetIonoCorrection()
            + multiplierEGS * adapterEGS->GetIonoCorrection() 
            + multiplierSGS * adapterSGS->GetIonoCorrection();
         
         cMeasurement.tropoCorrectValue = multiplierESC * adapterESC->GetTropoCorrection()
            + multiplierSSC * adapterSSC->GetTropoCorrection()
            + multiplierEGS * adapterEGS->GetTropoCorrection() 
            + multiplierSGS * adapterSGS->GetTropoCorrection();


         // 4.11. Specify measurement feasibility
         cMeasurement.isFeasible = true;
         cMeasurement.unfeasibleReason = "N";
         cMeasurement.feasibilityValue = measDataEGS.feasibilityValue;
         if (!measDataESC.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataESC.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataESC.feasibilityValue;
         }
         else if (!measDataEGS.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataEGS.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataEGS.feasibilityValue;
         }
         else if (!measDataSSC.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataSSC.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataSSC.feasibilityValue;
         }
         else if (!measDataSGS.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataSGS.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataSGS.feasibilityValue;
         }
         

         cMeasurement.horpHeight = GmatMathUtil::Min(cMeasurement.horpHeight, measDataSSC.horpHeight);
         cMeasurement.horpHeight = GmatMathUtil::Min(cMeasurement.horpHeight, measDataESC.horpHeight);
         cMeasurement.horpAngle = GmatMathUtil::Max(cMeasurement.horpAngle, measDataSSC.horpAngle);
         cMeasurement.horpAngle = GmatMathUtil::Max(cMeasurement.horpAngle, measDataESC.horpAngle);

         
         // 4.12. Add noise and bias
         Real C_idealVal = cMeasurement.value[i];

         if (measurementType == "SN_Doppler_Rtn")
         {
            // Compute bias
            //ComputeMeasurementBias("Bias", "BRTSDoppler_HZ", 2);
            ComputeMeasurementBias("Bias", "SN_Doppler_Rtn", 2);
            // Compute noise sigma
            //ComputeMeasurementNoiseSigma("NoiseSigma", "BRTSDoppler_HZ", 2);
            ComputeMeasurementNoiseSigma("NoiseSigma", "SN_Doppler_Rtn", 2);
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

                  //Add in Oscillatorbias
                  Real oscBias = adapterESC->GetMeasurementModel()->GetTransmitFrequencyOscillatorBias(0, 0);
                  cMeasurement.value[i] = cMeasurement.value[i] + oscBias;                       // pass bias unit: Km/s
                  cMeasurement.correction[i] = cMeasurement.correction[i] + oscBias;             // pass bias unit: Km/s
                  
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
            MessageInterface::ShowMessage("====  TDRS3LReturnDopplerAdapter: Range Calculation for Measurement Data %dth  \n", i);
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("      . Measurement epoch             : %.12lf\n", cMeasurement.epochGT.GetMjd());
            MessageInterface::ShowMessage("      . Measurement type              : <%s>\n", measurementType.c_str());
            MessageInterface::ShowMessage("      . Noise adding option           : %s\n", (addNoise?"true":"false"));
            MessageInterface::ShowMessage("      . Doppler count interval        : %.12lf seconds\n", dopplerCountInterval);
            MessageInterface::ShowMessage("      . Real travel time for SSC-path : %.12lf Km\n", measDataSSC.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for SGS-path : %.12lf Km\n", measDataSGS.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for ESC-path : %.12lf Km\n", measDataESC.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for EGS-path : %.12lf Km\n", measDataEGS.value[i]);
            MessageInterface::ShowMessage("      . Service access                : %s\n", serviceAccessList[serviceAccessIndex].c_str());
            MessageInterface::ShowMessage("      . Node 4 (effect) frequency     : %.12lf MHz\n", effFreq);
            MessageInterface::ShowMessage("      . Node 4 frequency band         : %d\n", node4FreqBand);
            MessageInterface::ShowMessage("      . TDRS id                       : %s\n", tdrsId.c_str());
            MessageInterface::ShowMessage("      . SMAR id                       : %d\n", smarId);
            MessageInterface::ShowMessage("      . Data Flag                     : %d\n", dataFlag);
            MessageInterface::ShowMessage("      . Pilot frequency               : %.12lf MHz\n", pilotFreq);
            MessageInterface::ShowMessage("      . Multiplier factor for SSC-path: %.12lf Hz/Km\n", multiplierSSC);
            MessageInterface::ShowMessage("      . Multiplier factor for SGS-path: %.12lf Hz/Km\n", multiplierSGS);
            MessageInterface::ShowMessage("      . Multiplier factor for ESC-path: %.12lf Hz/Km\n", multiplierESC);
            MessageInterface::ShowMessage("      . Multiplier factor for EGS-path: %.12lf Hz/Km\n", multiplierEGS);
            MessageInterface::ShowMessage("      . C-value w/o noise and bias    : %.12lf Hz\n", C_idealVal);

            if (measurementType == "SN_Doppler_Rtn")
            {
               MessageInterface::ShowMessage("      . TDRSS 3L Return Doppler noise sigma : %.12lf Hz \n", noiseSigma[i]);
               MessageInterface::ShowMessage("      . TDRSS 3L Return Doppler bias        : %.12lf Hz \n", measurementBias[i]);
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
         "TDRS3LReturnDopplerAdapter::CalculateMeasurement(%s, <%p>, <%p>)"
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
TDRS3LReturnDopplerAdapter::CalculateMeasurementDerivatives(GmatBase* obj, 
   Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   Integer parmId = GetParmIdFromEstID(id, obj);
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage(
         "Enter TDRS3LReturnDopplerAdapter::CalculateMeasurementDerivatives"
         "(%s, %d) called; parm ID is %d; Epoch %.12lf\n", 
         obj->GetFullName().c_str(), id, parmId, cMeasurement.epochGT.GetMjd());
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

      if (((ErrorModel*)obj)->GetStringParameter("Type") == "SN_Doppler_Rtn" && checker > 1)
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
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "SN_Doppler_Rtn")
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
      RealArray dOscFreq_dPolyCoeffs = ((Transmitter*)obj)->GetFrequencyBiasDerivative();
      theDataDerivatives.push_back(dOscFreq_dPolyCoeffs);
   }
   else
   {
      // Derivative for End-Spacecraft path
      const std::vector<RealArray> *derivativeDataESC =
         &(adapterESC->CalculateMeasurementDerivatives(obj, id));
      // Derivative for End-GroundStation path
      const std::vector<RealArray> *derivativeDataEGS =
         &(adapterEGS->CalculateMeasurementDerivatives(obj, id));
      // Derivative for Start-Spacecraft path
      const std::vector<RealArray> *derivativeDataSSC =
         &(adapterSSC->CalculateMeasurementDerivatives(obj, id));
      // Derivative for Start-GroungStation path
      const std::vector<RealArray> *derivativeDataSGS =
         &(adapterSGS->CalculateMeasurementDerivatives(obj, id));
      // copy SSC, SGS, ESC, and EGS paths' derivatives
      UnsignedInt size = derivativeDataESC->at(0).size();
      std::vector<RealArray> derivativesESC;
      for (UnsignedInt i = 0; i < derivativeDataESC->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesESC.push_back(oneRow);

         if (derivativeDataESC->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesESC[i][j] = (derivativeDataESC->at(i))[j];
         }
      }
   
      size = derivativeDataEGS->at(0).size();
      std::vector<RealArray> derivativesEGS;
      for (UnsignedInt i = 0; i < derivativeDataEGS->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesEGS.push_back(oneRow);

         if (derivativeDataEGS->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesEGS[i][j] = (derivativeDataEGS->at(i))[j];
         }
      }

      size = derivativeDataSSC->at(0).size();
      std::vector<RealArray> derivativesSSC;
      for (UnsignedInt i = 0; i < derivativeDataSSC->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesSSC.push_back(oneRow);

         if (derivativeDataSSC->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesSSC[i][j] = (derivativeDataSSC->at(i))[j];
         }
      }

      size = derivativeDataSGS->at(0).size();
      std::vector<RealArray> derivativesSGS;
      for (UnsignedInt i = 0; i < derivativeDataSGS->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesSGS.push_back(oneRow);

         if (derivativeDataSGS->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesSGS[i][j] = (derivativeDataSGS->at(i))[j];
         }
      }

      #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives End-Spacecraft path: [");
      for (UnsignedInt i = 0; i < derivativeDataESC->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataESC->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataESC->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives End-GroundStation path: [");
      for (UnsignedInt i = 0; i < derivativeDataEGS->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataEGS->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataEGS->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives Start-Spacecraft path: [");
      for (UnsignedInt i = 0; i < derivativeDataSSC->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataSSC->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataSSC->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives Start-GroundStation path: [");
      for (UnsignedInt i = 0; i < derivativeDataSGS->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataSGS->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataSGS->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      #endif

      // Now assemble the derivative data into the requested derivative
      size = derivativesESC[0].size();               // This is the size of derivative vector for signal path 0
      
      for (UnsignedInt i = 0; i < derivativesESC.size(); ++i)
      {
         // For each signal path, do the following steps:
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);

         if (derivativesESC[i].size() != size)
            throw MeasurementException("Derivative data size for End-Spacecarft path is a different size "
               "than expected");
         if (derivativesEGS[i].size() != size)
            throw MeasurementException("Derivative data size for End-GroundStation path is a different size "
               "than expected");
         if (derivativesSSC[i].size() != size)
            throw MeasurementException("Derivative data size for Start-Spacecraft path is a different size "
               "than expected");
         if (derivativesSGS[i].size() != size)
            throw MeasurementException("Derivative data size for Start-GroundStation path is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            if ((paramName == "Position")||(paramName == "Velocity")||(paramName == "CartesianX"))
            {
               // Convert measurement derivatives from km/s to Hz for velocity and position 
               theDataDerivatives[i][j] = + derivativesESC[i][j] * multiplierESC
                                          + derivativesEGS[i][j] * multiplierEGS
                                          + derivativesSSC[i][j] * multiplierSSC
                                          + derivativesSGS[i][j] * multiplierSGS;
            }
            else
            {
               // set the same End-Spacecraft path's derivatives for Bias an other solve-for variables
               theDataDerivatives[i][j] = derivativesESC[i][j];
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
      MessageInterface::ShowMessage("Exit TDRS3LReturnDopplerAdapter::CalculateMeasurementDerivatives():\n");
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
bool TDRS3LReturnDopplerAdapter::WriteMeasurements()
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
bool TDRS3LReturnDopplerAdapter::WriteMeasurement(Integer id)
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
Integer TDRS3LReturnDopplerAdapter::HasParameterCovariances(Integer parameterId)
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
Integer TDRS3LReturnDopplerAdapter::GetEventCount()
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
void TDRS3LReturnDopplerAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   adapterSSC->SetCorrection(correctionName, correctionType);
   adapterSGS->SetCorrection(correctionName, correctionType);
   adapterESC->SetCorrection(correctionName, correctionType);
   adapterEGS->SetCorrection(correctionName, correctionType);
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
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
Real TDRS3LReturnDopplerAdapter::GetTurnAroundRatio(Integer freqBand)
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

