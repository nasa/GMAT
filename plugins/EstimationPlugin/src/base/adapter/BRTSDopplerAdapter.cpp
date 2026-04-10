//------------------------------------------------------------------------------
//                           BRTSDopplerAdapter
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

#include "BRTSDopplerAdapter.hpp"
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
BRTSDopplerAdapter::PARAMETER_TEXT[BRTSDopplerAdapterParamCount - BRTSRangeAdapterParamCount] =
{
   "DopplerCountInterval",
   "ServiceAccess",
   "Node4Frequency",
   "Node4Band",
   "SmarId",
   "DataFlag",
};


const Gmat::ParameterType
BRTSDopplerAdapter::PARAMETER_TYPE[BRTSDopplerAdapterParamCount - BRTSRangeAdapterParamCount] =
{
   Gmat::REAL_TYPE,                    // DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,             // SERVICE_ACCESS
   Gmat::REAL_TYPE,                    // NODE4_FREQUENCY
   Gmat::INTEGER_TYPE,                 // NODE4_BAND
   Gmat::INTEGER_TYPE,                 // SMAR_ID
   Gmat::INTEGER_TYPE,                 // DATA_FLAG
};




//------------------------------------------------------------------------------
// BRTSDopplerAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
BRTSDopplerAdapter::BRTSDopplerAdapter(const std::string& name) :
   BRTSRangeAdapter       (name),
   adapterSL              (NULL),
   adapterSS              (NULL),
   adapterES              (NULL),
   dopplerCountInterval   (1.0),                     // 1 second
   node4Freq              (2000.0),                  // 2000 Mhz
   node4FreqBand          (1),                       // 0: undefined, 1: S-band, 2: X-band, 3: K-band
   smarId                 (0),
   dataFlag               (0)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BRTSDopplerAdapter default constructor <%p>\n", this);
#endif
   typeName = "BRTS_Doppler";
   parameterCount = BRTSDopplerAdapterParamCount;
}


//------------------------------------------------------------------------------
// ~BRTSDopplerAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BRTSDopplerAdapter::~BRTSDopplerAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BRTSDopplerAdapter default destructor <%p>\n", this);
#endif

   if (adapterSL)
      delete adapterSL;
   if (adapterSS)
      delete adapterSS;
   if (adapterES)
      delete adapterES;
}


//------------------------------------------------------------------------------
// BRTSDopplerAdapter(const BRTSDopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param da The adapter copied to make this one
 */
//------------------------------------------------------------------------------
BRTSDopplerAdapter::BRTSDopplerAdapter(const BRTSDopplerAdapter& da) :
   BRTSRangeAdapter       (da),
   adapterSL              (NULL),
   adapterSS              (NULL),
   adapterES              (NULL),
   dopplerCountInterval   (da.dopplerCountInterval),
   serviceAccessList      (da.serviceAccessList),
   node4Freq              (da.node4Freq),
   node4FreqBand          (da.node4FreqBand),
   smarId                 (da.smarId),
   dataFlag               (da.dataFlag)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BRTSDopplerAdapter copy constructor   from <%p> to <%p>\n", &da, this);
#endif
   if (da.adapterES)
      adapterES = (RangeAdapterKm*)(da.adapterES->Clone());
   if (da.adapterSL)
      adapterSL = (BRTSRangeAdapter*)(da.adapterSL->Clone());
   if (da.adapterSS)
      adapterSS = (RangeAdapterKm*)(da.adapterSS->Clone());

}


//------------------------------------------------------------------------------
// BRTSDopplerAdapter& operator=(const BRTSDopplerAdapter& da)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param da The adapter copied to make this one match it
 *
 * @return This adapter made to look like da
 */
//------------------------------------------------------------------------------
BRTSDopplerAdapter& BRTSDopplerAdapter::operator=(const BRTSDopplerAdapter& da)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BRTSDopplerAdapter operator =   set <%p> = <%p>\n", this, &da);
#endif

   if (this != &da)
   {
      BRTSRangeAdapter::operator=(da);

      dopplerCountInterval = da.dopplerCountInterval;
      serviceAccessList    = da.serviceAccessList;
      node4Freq            = da.node4Freq;
      node4FreqBand        = da.node4FreqBand;
      smarId               = da.smarId;
      dataFlag             = da.dataFlag;

      if (adapterSL)
      {
         delete adapterSL;
         adapterSL = NULL;
      }
      if (da.adapterSL)
         adapterSL = (BRTSRangeAdapter*)da.adapterSL->Clone();

      if (adapterSS)
      {
         delete adapterSS;
         adapterSS = NULL;
      }
      if (da.adapterSS)
         adapterSS = (RangeAdapterKm*)da.adapterSS->Clone();

      if (adapterES)
      {
         delete adapterES;
         adapterES = NULL;
      }
      if (da.adapterES)
         adapterES = (RangeAdapterKm*)da.adapterES->Clone();

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
void BRTSDopplerAdapter::SetSolarSystem(SolarSystem *ss)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("BRTSDopplerAdapter<%p>::SetSolarSystem('%s')\n", this, ss->GetName().c_str()); 
#endif

   adapterSL->SetSolarSystem(ss);
   adapterSS->SetSolarSystem(ss);
   adapterES->SetSolarSystem(ss);

   BRTSRangeAdapter::SetSolarSystem(ss);
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
GmatBase* BRTSDopplerAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BRTSDopplerAdapter::Clone()   clone this <%p>\n", this);
#endif

   return new BRTSDopplerAdapter(*this);
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
std::string BRTSDopplerAdapter::GetParameterText(const Integer id) const
{
   if (id >= BRTSRangeAdapterParamCount && id < BRTSDopplerAdapterParamCount)
      return PARAMETER_TEXT[id - BRTSRangeAdapterParamCount];
   return BRTSRangeAdapter::GetParameterText(id);
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
Integer BRTSDopplerAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = BRTSRangeAdapterParamCount; i < BRTSDopplerAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BRTSRangeAdapterParamCount])
         return i;
   }
   return BRTSRangeAdapter::GetParameterID(str);
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
Gmat::ParameterType BRTSDopplerAdapter::GetParameterType(const Integer id) const
{
   if (id >= BRTSRangeAdapterParamCount && id < BRTSDopplerAdapterParamCount)
      return PARAMETER_TYPE[id - BRTSRangeAdapterParamCount];

   return BRTSRangeAdapter::GetParameterType(id);
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
std::string BRTSDopplerAdapter::GetParameterTypeString(const Integer id) const
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
bool BRTSDopplerAdapter::SetStringParameter(const Integer id, const std::string& value)
{
   bool retval = true;
   if ((id != MEASUREMENT_TYPE)&&(id != SIGNAL_PATH))
   {
      retval = adapterSL->SetStringParameter(id, value) && retval;
      retval = adapterSS->SetStringParameter(id, value) && retval;
      retval = adapterES->SetStringParameter(id, value) && retval;
   }

   retval = BRTSRangeAdapter::SetStringParameter(id, value) && retval;

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
bool BRTSDopplerAdapter::SetStringParameter(const Integer id,
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
   retval = adapterSL->SetStringParameter(id, value, index) && retval;
   retval = adapterSS->SetStringParameter(id, value, index) && retval;
   retval = adapterES->SetStringParameter(id, value, index) && retval;

   retval = BRTSRangeAdapter::SetStringParameter(id, value, index) && retval;

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
std::string BRTSDopplerAdapter::GetStringParameter(const Integer id,
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

   return BRTSRangeAdapter::GetStringParameter(id, index);
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
bool BRTSDopplerAdapter::SetStringParameter(const std::string& label,
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
bool BRTSDopplerAdapter::SetStringParameter(const std::string& label,
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
std::string BRTSDopplerAdapter::GetStringParameter(const std::string& label,
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
Integer BRTSDopplerAdapter::SetIntegerParameter(const Integer id, const Integer value)
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


   adapterSL->SetIntegerParameter(id, value);
   adapterSS->SetIntegerParameter(id, value);
   adapterES->SetIntegerParameter(id, value);

   return BRTSRangeAdapter::SetIntegerParameter(id, value);
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
Integer BRTSDopplerAdapter::GetIntegerParameter(const Integer id) const
{
   if (id == NODE4_BAND)
      return node4FreqBand;

   if (id == SMAR_ID)
      return smarId;

   if (id == DATA_FLAG)
      return dataFlag;

   return BRTSRangeAdapter::GetIntegerParameter(id);
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
Integer BRTSDopplerAdapter::SetIntegerParameter(const std::string &label, const Integer value)
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
Integer BRTSDopplerAdapter::GetIntegerParameter(const std::string &label) const
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
Real BRTSDopplerAdapter::GetRealParameter(const Integer id) const
{
   if (id == DOPPLER_COUNT_INTERVAL)
      return dopplerCountInterval;

   if (id == NODE4_FREQUENCY)
      return node4Freq;

   return BRTSRangeAdapter::GetRealParameter(id);
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
Real BRTSDopplerAdapter::SetRealParameter(const Integer id, const Real value)
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

   adapterSL->SetRealParameter(id, value);
   adapterSS->SetRealParameter(id, value);
   adapterES->SetRealParameter(id, value);

   Real retval = BRTSRangeAdapter::SetRealParameter(id, value);

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
Real BRTSDopplerAdapter::GetRealParameter(const std::string &label) const
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
Real BRTSDopplerAdapter::SetRealParameter(const std::string &label, const Real value)
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
bool BRTSDopplerAdapter::SetBooleanParameter(const Integer id, const bool value)
{
   // Note: for SL, SS, and ES paths, AddNoise always is set to false due to it calculation
   if (id == ADD_NOISE)
   {
      adapterSL->SetBooleanParameter(id, false);
      adapterSS->SetBooleanParameter(id, false);
      adapterES->SetBooleanParameter(id, false);
   }
   else
   {
      adapterSL->SetBooleanParameter(id, value);
      adapterSS->SetBooleanParameter(id, value);
      adapterES->SetBooleanParameter(id, value);
   }

   return BRTSRangeAdapter::SetBooleanParameter(id, value);
}


bool BRTSDopplerAdapter::SetBooleanParameter(const std::string &label, const bool value)
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
const StringArray& BRTSDopplerAdapter::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SERVICE_ACCESS)
      return serviceAccessList;

   return BRTSRangeAdapter::GetStringArrayParameter(id);
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
bool BRTSDopplerAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   // Handle additional renames specific to this adapter
   bool retval = true;
   retval = adapterSL->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterSS->RenameRefObject(type, oldName, newName) && retval;
   retval = adapterES->RenameRefObject(type, oldName, newName) && retval;

   retval = BRTSRangeAdapter::RenameRefObject(type, oldName, newName) && retval;

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
bool BRTSDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   bool retval = true;
   retval = adapterSL->SetRefObject(obj, type, name) && retval;
   retval = adapterSS->SetRefObject(obj, type, name) && retval;
   retval = adapterES->SetRefObject(obj, type, name) && retval;

   retval = BRTSRangeAdapter::SetRefObject(obj, type, name) && retval;

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
bool BRTSDopplerAdapter::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   bool retval = true;
   retval = adapterSL->SetRefObject(obj, type, name, index) && retval;
   retval = adapterSS->SetRefObject(obj, type, name, index) && retval;
   retval = adapterES->SetRefObject(obj, type, name, index) && retval;

   retval = BRTSRangeAdapter::SetRefObject(obj, type, name, index) && retval;

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
bool BRTSDopplerAdapter::SetMeasurement(MeasureModel* meas)
{
   return BRTSRangeAdapter::SetMeasurement(meas);
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
void BRTSDopplerAdapter::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting propagator to %p in "
            "BRTSDopplerAdapter\n", ps);
   #endif

   adapterSL->SetPropagators(ps, spMap);
   adapterSS->SetPropagators(ps, spMap);
   adapterES->SetPropagators(ps, spMap);

   BRTSRangeAdapter::SetPropagators(ps, spMap);
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
void BRTSDopplerAdapter::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    GetMeasurementModel()->SetTransientForces(tf);
    adapterSL->SetTransientForces(tf);
    adapterSS->SetTransientForces(tf);
    adapterES->SetTransientForces(tf);
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
bool BRTSDopplerAdapter::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("BRTSDopplerAdapter::Initialize() <%p> start\n", this);
#endif

   bool retval = false;

   if (BRTSRangeAdapter::Initialize())
   {
      // @todo: initialize all needed variables
      //if (serviceAccessList.size() == 0)
      //   throw MeasurementException("Error: no TDRS service access was set for measurement\n");

      serviceAccessIndex = 0;

      retval = adapterSL->Initialize();
      retval = adapterSS->Initialize() && retval;
      retval = adapterES->Initialize() && retval;
   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("BRTSDopplerAdapter::Initialize() <%p> exit\n", this);
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
const MeasurementData& BRTSDopplerAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable,
      bool forSimulation)
{
   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("BRTSDopplerAdapter::CalculateMeasurement(%s, "
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

   SetForTdrs();

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
            throw MeasurementException("Error: Simulation TDRS service access list is empty.\n");

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

      cMeasurement.tdrsNode4Freq = node4Freq*1.0e6;        // unit Hz
      cMeasurement.tdrsNode4Band = node4FreqBand;
      cMeasurement.tdrsDataFlag  = dataFlag;
      cMeasurement.tdrsSMARID    = smarId;

      if (serviceAccessList.empty())
         throw MeasurementException("Error: No service access is specified for the measurement.\n");
      cMeasurement.tdrsServiceID = serviceAccessList[serviceAccessIndex];
      //MessageInterface::ShowMessage("service access = %s   node 4 frequency = %le Hz   node 4 band = %d    TDRS data flag = %d    SMAR id = %d\n",
      //   cMeasurement.tdrsServiceID.c_str(), cMeasurement.tdrsNode4//Freq, cMeasurement.tdrsNode4Band, cMeasurement.tdrsDataFlag, cMeasurement.tdrsSMARID);


      // 2. Compute range for End path
      // 2.1. Propagate all space objects to time tm
      // This step is not needed due to measurement time tm is set to t3RE
      // 2.2. Compute range in km for End Path
      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("Compute range for End-Paths...\n");
      #endif
      bool addNoiseOption  = addNoise;
      bool addBiasOption   = addBias;
      bool rangeOnlyOption = rangeOnly;
      bool withMedia = withMediaCorrection;

      // 2.2.1. Compute range in km w/o any noise or bias for End-Long path
      addNoise  = false;
      addBias   = false;
      rangeOnly = true;
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for End Long-Path...\n");
#endif
      BRTSRangeAdapter::CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for End Long-Path.\n");
#endif

      // reset back to their orginal values
      addNoise  = addNoiseOption;
      addBias   = addBiasOption;
      rangeOnly = rangeOnlyOption;
      withMediaCorrection = withMedia;

      // 2.2.2. Specify uplink frequency
      // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0
      uplinkFreq        = calcData->GetUplinkFrequency(0, NULL);                   // No ramp table is used for BRTS measurement   // calcData->GetUplinkFrequency(0, rampTB);
      uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, NULL);    // No ramp table is used for BRTS measurement   // calcData->GetUplinkFrequency(0, rampTB);
      freqBand          = calcData->GetUplinkFrequencyBand(0, NULL);               // No ramp table is used for BRTS measurement   // calcData->GetUplinkFrequencyBand(0, rampTB);

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
      // For End-Short path, range calculation does not add bias and noise to calculated value
      adapterES->AddBias(false);
      adapterES->AddNoise(false);
      adapterES->SetRangeOnly(true);
      adapterES->AddMediaCorrection(withMedia);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for End Short-Path...    adapterES = <%p>\n", adapterES);
#endif
      adapterES->SetForTdrs();
      adapterES->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for End Short-Path.\n");
#endif
   
      // 3. Compute for Start-Long and Start-Short paths w/o any noise and bias
      #ifdef DEBUG_DOPPLER_CALCULATION
         MessageInterface::ShowMessage("Compute range for Start-Paths...\n");
      #endif

      // 3.1. Set doppler count interval to MeasureModel object due to
      // Start-Long path and Start-Short path are measured earlier by number of
      // seconds shown in doppler count interval
      adapterSL->GetMeasurementModel()->SetCountInterval(dopplerCountInterval);
      adapterSS->GetMeasurementModel()->SetCountInterval(dopplerCountInterval);

      // 3.2. For Start-path, range calculation does not add bias and noise to calculated value
      // Note that: default option is no adding noise
      adapterSL->AddBias(false);
      adapterSL->AddNoise(false);
      adapterSL->SetRangeOnly(true);
      adapterSL->AddMediaCorrection(withMedia);
      adapterSS->AddBias(false);
      adapterSS->AddNoise(false);
      adapterSS->SetRangeOnly(true);
      adapterSS->AddMediaCorrection(withMedia);
   
      // 3.3. Compute range for Start long and short paths
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for Start Long-Path...\n");
#endif
      adapterSL->SetForTdrs();
      adapterSL->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for Start Long-Path.\n");
#endif
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Compute range for Start Short-Path...\n");
#endif
      adapterSS->SetForTdrs();
      adapterSS->CalculateMeasurement(withEvents, obData, rampTB, forSimulation);
#ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("Complete calculation range for Start Short-Path.\n");
#endif
      adapterES->AddMediaCorrection(withMedia);
      adapterSL->AddMediaCorrection(withMedia);
      adapterSS->AddMediaCorrection(withMedia);

      // 3.4. Remove obData object when it is not used
      if (obData)
      {
         delete obData;
         obData = NULL;
      }
      Real speedoflightkm = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;

      // 4. Calculate Doopler shift frequency (unit: Hz) based on range (unit: km) and store result to cMeasurement:
      std::vector<SignalBase*> paths = calcData->GetSignalPaths();
      for (UnsignedInt i = 0; i < paths.size(); ++i)          // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths.
      {
         // 4.0. Get legs: 1, 2, 3, and 4:
         SignalBase* leg1 = paths[i];
         SignalBase* leg2 = leg1->GetNext();
         SignalBase* leg3 = leg2->GetNext();
         SignalBase* leg4 = leg3->GetNext();

         // 4.1. Specify TDRS and Sat transponder turn around ratios:
         SignalData* sd2 = leg2->GetSignalDataObject();
         SignalData* sd3 = leg3->GetSignalDataObject();
         Real tdrsTAR = sd2->transmitFreq / sd2->arriveFreq;       // TDRS transponder turn around ratio
         Real satTAR   = sd3->transmitFreq / sd3->arriveFreq;      // BRTS transponder turn around ratio
         Real totalTAR = tdrsTAR * satTAR;                         // combination turn around ratio

         // 4.2. Specify TDRS id
         std::string scId = sd2->tNode->GetStringParameter("Id");
         if (!GetTdrsIdFromSpacecraftId(scId))
         {
            if (warnTdrsId)
            {
               warnTdrsId = false;
               MessageInterface::ShowMessage("Warning: The TDRS ID for Spacecraft %s could not be determined, the default ID of %s will be used. Some Doppler measurements may be computed incorrectly.\n", sd2->tNode->GetName().c_str(), TDRSId.c_str());
            }
         }

         // 4.3. Specify effective frequency
         Real effFreq = node4Freq;                                                          // it is effective transmit frequency A from BRTS station or user spacecraft:  A = f0 * Ratio * a

         // 4.4. Calculate uplink frequency based on Node 4 frequency:
         uplinkFreq = node4Freq / totalTAR;                                                // It is transmit frequency from ground station:  f0 = A/(Ratio*a) with assumtion a = 1.0.
         //MessageInterface::ShowMessage("new uplinkFreq = %.12le   \n", uplinkFreq);

         // Step 4.5 is incorrect. We use transmit frequency setting for ground station. It is not derived from Node4Frequency due to we do not know value a of frequency mixer.
         // We have A = f0*TAR*a. Therefor frequency mixer physical property a = A/(f0*TAR)
         /// @Todo: It needs to remove this step in TDRSDopplerAdapter as well.
         ////// 4.5. Recalculate frequency and media correction for each leg from the value of uplink frequency
         ////ReCalculateFrequencyAndMediaCorrection(i, uplinkFreq, rampTB);
         ////adapterES->ReCalculateFrequencyAndMediaCorrection(i, uplinkFreq, rampTB);
         ////adapterSL->ReCalculateFrequencyAndMediaCorrection(i, uplinkFreq, rampTB);
         ////adapterSS->ReCalculateFrequencyAndMediaCorrection(i, uplinkFreq, rampTB);

         // 4.6. Get measurement data for each path
         measDataEL = GetMeasurement();
         measDataES = adapterES->GetMeasurement();
         measDataSL = adapterSL->GetMeasurement();
         measDataSS = adapterSS->GetMeasurement();

         // 4.7. Specify pilot frequency
         Real pilotFreq = 0.0;
         try
         {
            pilotFreq = CalculatePilotFrequency(effFreq, node4FreqBand, serviceAccessList[serviceAccessIndex], TDRSId, smarId, dataFlag);                        // unit: MHz
         }
         catch (MeasurementException &e)
         {
            std::string errmsg = "Error calculating TDRS Pilot frequency for TDRS: \"" + sd2->tNode->GetName() + "\". Error message found: \n   " + e.GetFullMessage().substr(30);
            throw MeasurementException(errmsg);
         }
         //Real dBdA = CalculatePilotFrequencyDerivativeWRTEffectiveFrequency(effFreq, node4FreqBand, serviceAccessList[serviceAccessIndex], tdrsId, smarId, dataFlag);


         //MessageInterface::ShowMessage("effect freq = %le MHz     pivot freq = %le MHz   TDRS TAR = %f     Sat TAR = %f\n", effFreq, pilotFreq, tdrsTAR, satTAR);
         // 4.8. Specify multiplier for SL-path, SS-path, EL-path, and ES-path
         multiplierSL = (effFreq*1.0e6) / (dopplerCountInterval*speedoflightkm);                                // unit: Hz/Km
         multiplierSS = (pilotFreq*1.0e6)/(dopplerCountInterval*speedoflightkm);                                // unit: Hz/Km
         multiplierEL = -(effFreq*1.0e6) / (dopplerCountInterval*speedoflightkm);                               // unit: Hz/Km
         multiplierES = -(pilotFreq*1.0e6)/(dopplerCountInterval*speedoflightkm);                               // unit: Hz/Km
         //MessageInterface::ShowMessage("multiplierEL = %.12le   multiplierES = %.12le   multiplierSL = %.12le   multiplierSS = %.12le   \n", multiplierEL, multiplierES, multiplierSL, multiplierSS);
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
         //Real dopplerCountCorrection = dopplerCountInterval * speedoflightkm;
         //cMeasurement.value[i] = multiplierEL * (measDataEL.value[i] / GetMultiplierFactor())
         //                        + multiplierSL * (measDataSL.value[i] / adapterSL->GetMultiplierFactor() - dopplerCountCorrection)
         //                        + multiplierES * (measDataES.value[i] / adapterES->GetMultiplierFactor())
         //                        + multiplierSS * (measDataSS.value[i] / adapterSS->GetMultiplierFactor() - dopplerCountCorrection);                // unit: Hz    //(Equation 7-92 GTDS MathSpec)

         //cMeasurement.correction[i] = multiplierEL * (measDataEL.correction[i] / GetMultiplierFactor())
         //   + multiplierSL * (measDataSL.correction[i] / adapterSL->GetMultiplierFactor() - dopplerCountCorrection)
         //   + multiplierES * (measDataES.correction[i] / adapterES->GetMultiplierFactor())
         //   + multiplierSS * (measDataSS.correction[i] / adapterSS->GetMultiplierFactor() - dopplerCountCorrection);                                // unit: Hz    //(Equation 7-92 GTDS MathSpec)

         // Note that: doppler count correction was done in BRTS range measuremnt
         cMeasurement.value[i] = multiplierEL * (measDataEL.value[i] / GetMultiplierFactor())
                                 + multiplierSL * (measDataSL.value[i] / adapterSL->GetMultiplierFactor())
                                 + multiplierES * (measDataES.value[i] / adapterES->GetMultiplierFactor())
                                 + multiplierSS * (measDataSS.value[i] / adapterSS->GetMultiplierFactor());          // unit: Hz

         cMeasurement.correction[i] = multiplierEL * (measDataEL.correction[i] / GetMultiplierFactor())
            + multiplierSL * (measDataSL.correction[i] / adapterSL->GetMultiplierFactor())
            + multiplierES * (measDataES.correction[i] / adapterES->GetMultiplierFactor())
            + multiplierSS * (measDataSS.correction[i] / adapterSS->GetMultiplierFactor());                          // unit: Hz

         // Update media corrections
         cMeasurement.ionoCorrectValue = multiplierEL * GetIonoCorrection() + multiplierSL * adapterSL->GetIonoCorrection()
            + multiplierES * adapterES->GetIonoCorrection() + multiplierSS * adapterSS->GetIonoCorrection();

         cMeasurement.tropoCorrectValue = multiplierEL * GetTropoCorrection() + multiplierSL * adapterSL->GetTropoCorrection()
            + multiplierES * adapterES->GetTropoCorrection() + multiplierSS * adapterSS->GetTropoCorrection();


         // 4.11. Specify measurement feasibility
         if (!measDataEL.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataEL.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataEL.feasibilityValue;
         }
         else if (!measDataES.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataES.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataES.feasibilityValue;
         }
         else if (!measDataSL.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataSL.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataSL.feasibilityValue;
         }
         else if (!measDataSS.isFeasible)
         {
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = measDataSS.unfeasibleReason;
            cMeasurement.feasibilityValue = measDataSS.feasibilityValue;
         }

         // 4.12. Add noise and bias
         Real C_idealVal = cMeasurement.value[i];

         if (measurementType == "BRTS_Doppler")
         {
            // Compute bias
            ComputeMeasurementBias("Bias", "BRTS_Doppler", 2);
            // Compute noise sigma
            ComputeMeasurementNoiseSigma("NoiseSigma", "BRTS_Doppler", 2);
            // Compute measurement error covariance matrix
            ComputeMeasurementErrorCovarianceMatrix();

            // Calculate measurement covariance
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

               // Add bias to measurement value only after noise had been added in order to avoid adding bias' noise
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
            MessageInterface::ShowMessage("====  BRTSDopplerAdapter: Range Calculation for Measurement Data %dth  \n", i);
            MessageInterface::ShowMessage("===================================================================\n");
            MessageInterface::ShowMessage("      . Measurement epoch          : %.12lf\n", cMeasurement.epochGT.GetMjd());
            MessageInterface::ShowMessage("      . Measurement type            : <%s>\n", measurementType.c_str());
            MessageInterface::ShowMessage("      . Noise adding option         : %s\n", (addNoise?"true":"false"));
            MessageInterface::ShowMessage("      . Doppler count interval      : %.12lf seconds\n", dopplerCountInterval);
            MessageInterface::ShowMessage("      . Real travel time for SL-path : %.12lf Km\n", measDataSL.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for SS-path : %.12lf Km\n", measDataSS.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for EL-path : %.12lf Km\n", measDataEL.value[i]);
            MessageInterface::ShowMessage("      . Real travel time for ES-path : %.12lf Km\n", measDataES.value[i]);
            MessageInterface::ShowMessage("      . Service access               : %s\n", serviceAccessList[serviceAccessIndex].c_str());
            MessageInterface::ShowMessage("      . Node 4 (effect) frequency    : %.12lf MHz\n", effFreq);
            MessageInterface::ShowMessage("      . Node 4 frequency band        : %d\n", node4FreqBand);
            MessageInterface::ShowMessage("      . TDRS id                      : %s\n", tdrsId.c_str());
            MessageInterface::ShowMessage("      . SMAR id                      : %d\n", smarId);
            MessageInterface::ShowMessage("      . Data Flag                    : %d\n", dataFlag);
            MessageInterface::ShowMessage("      . Pilot frequency              : %.12lf MHz\n", pilotFreq);
            MessageInterface::ShowMessage("      . TDRS transponder turn around       ratio: %lf\n", tdrsTAR);
            MessageInterface::ShowMessage("      . Spacecraft transponder turn around ratio: %lf\n", satTAR);
            MessageInterface::ShowMessage("      . Multiplier factor for SL-path: %.12lf Hz/Km\n", multiplierSL);
            MessageInterface::ShowMessage("      . Multiplier factor for SS-path: %.12lf Hz/Km\n", multiplierSS);
            MessageInterface::ShowMessage("      . Multiplier factor for EL-path: %.12lf Hz/Km\n", multiplierEL);
            MessageInterface::ShowMessage("      . Multiplier factor for ES-path: %.12lf Hz/Km\n", multiplierES);
            MessageInterface::ShowMessage("      . C-value w/o noise and bias  : %.12lf Hz\n", C_idealVal);
            //if (measurementType == "BRTSDoppler_HZ")
            if (measurementType == "BRTS_Doppler")
            {
               MessageInterface::ShowMessage("      . BRTSDoppler noise sigma  : %.12lf Hz \n", noiseSigma[i]);
               MessageInterface::ShowMessage("      . BRTSDoppler bias         : %.12lf Hz \n", measurementBias[i]);
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
   else // The requested epoch is not supported - likely not on a BRTS ephem
   {
      epochValid = false;
   }

   if (!epochValid)
   {
      cMeasurement.isFeasible = false;
      cMeasurement.unfeasibleReason = "EGAP";
   }



   #ifdef DEBUG_DOPPLER_CALCULATION
      MessageInterface::ShowMessage("BRTSDopplerAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
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
const std::vector<RealArray>& BRTSDopplerAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   Integer parmId = GetParmIdFromEstID(id, obj);
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Enter BRTSDopplerAdapter::CalculateMeasurementDerivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n", obj->GetFullName().c_str(), id, parmId, cMeasurement.epoch);
   #endif
   
   // Get parameter name specified by id
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; // GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
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

      if (((ErrorModel*)obj)->GetStringParameter("Type") == "BRTS_Doppler" && checker > 1)
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
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "BRTS_Doppler")
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
   else
   {

      // Perform the calculations
      // Derivative for End-Long path
      const std::vector<RealArray> *derivativeDataEL =
         &(calcData->CalculateMeasurementDerivatives(obj, id)); 
      // Derivative for End-Short path
      const std::vector<RealArray> *derivativeDataES =
         &(adapterES->CalculateMeasurementDerivatives(obj, id));
      // Derivative for Start-Long path
      const std::vector<RealArray> *derivativeDataSL =
         &(adapterSL->CalculateMeasurementDerivatives(obj, id));
      // Derivative for Start-Short path
      const std::vector<RealArray> *derivativeDataSS =
         &(adapterSS->CalculateMeasurementDerivatives(obj, id));

      // copy SL, SS, EL, and ES paths' derivatives
      UnsignedInt size = derivativeDataEL->at(0).size();
      std::vector<RealArray> derivativesEL;
      for (UnsignedInt i = 0; i < derivativeDataEL->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesEL.push_back(oneRow);

         if (derivativeDataEL->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesEL[i][j] = (derivativeDataEL->at(i))[j];
         }
      }
   
      size = derivativeDataES->at(0).size();
      std::vector<RealArray> derivativesES;
      for (UnsignedInt i = 0; i < derivativeDataES->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesES.push_back(oneRow);

         if (derivativeDataES->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesES[i][j] = (derivativeDataES->at(i))[j];
         }
      }

      size = derivativeDataSL->at(0).size();
      std::vector<RealArray> derivativesSL;
      for (UnsignedInt i = 0; i < derivativeDataSL->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesSL.push_back(oneRow);

         if (derivativeDataSL->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesSL[i][j] = (derivativeDataSL->at(i))[j];
         }
      }

      size = derivativeDataSS->at(0).size();
      std::vector<RealArray> derivativesSS;
      for (UnsignedInt i = 0; i < derivativeDataSS->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         derivativesSS.push_back(oneRow);

         if (derivativeDataSS->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            derivativesSS[i][j] = (derivativeDataSS->at(i))[j];
         }
      }


      #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives End-Long path: [");
      for (UnsignedInt i = 0; i < derivativeDataEL->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataEL->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataEL->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives End-Short path: [");
      for (UnsignedInt i = 0; i < derivativeDataES->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataES->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataES->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives Start-Long path: [");
      for (UnsignedInt i = 0; i < derivativeDataSL->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataSL->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataSL->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      MessageInterface::ShowMessage("   Derivatives Start-Short path: [");
      for (UnsignedInt i = 0; i < derivativeDataSS->size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < derivativeDataSS->at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le", (derivativeDataSS->at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
      #endif

      // Now assemble the derivative data into the requested derivative
      size = derivativesEL[0].size();               // This is the size of derivative vector for signal path 0
      
      for (UnsignedInt i = 0; i < derivativesEL.size(); ++i)
      {
         // For each signal path, do the following steps:
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);

         if (derivativesEL[i].size() != size)
            throw MeasurementException("Derivative data size for End-Long path is a different size "
               "than expected");
         if (derivativesES[i].size() != size)
            throw MeasurementException("Derivative data size for End-Short path is a different size "
               "than expected");
         if (derivativesSL[i].size() != size)
            throw MeasurementException("Derivative data size for Start-Long path is a different size "
               "than expected");
         if (derivativesSS[i].size() != size)
            throw MeasurementException("Derivative data size for Start-Short path is a different size "
               "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            if ((paramName == "Position")||(paramName == "Velocity")||(paramName == "CartesianX"))
            {
               // Convert measurement derivatives from km/s to Hz for velocity and position 
               theDataDerivatives[i][j] = + derivativesEL[i][j] * multiplierEL
                                          + derivativesES[i][j] * multiplierES
                                          + derivativesSL[i][j] * multiplierSL
                                          + derivativesSS[i][j] * multiplierSS;
            }
            else
            {
               // set the same End-Long path's derivatives for Bias an other solve-for variables
               theDataDerivatives[i][j] = derivativesEL[i][j];
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
      MessageInterface::ShowMessage("Exit BRTSDopplerAdapter::CalculateMeasurementDerivatives():\n");
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
bool BRTSDopplerAdapter::WriteMeasurements()
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
bool BRTSDopplerAdapter::WriteMeasurement(Integer id)
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
Integer BRTSDopplerAdapter::HasParameterCovariances(Integer parameterId)
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
Integer BRTSDopplerAdapter::GetEventCount()
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
void BRTSDopplerAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   adapterSL->SetCorrection(correctionName, correctionType);
   adapterSS->SetCorrection(correctionName, correctionType);
   adapterES->SetCorrection(correctionName, correctionType);
   BRTSRangeAdapter::SetCorrection(correctionName, correctionType);
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
Real BRTSDopplerAdapter::GetTurnAroundRatio(Integer freqBand)
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

