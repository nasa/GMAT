//$Id: Sensor.cpp 211 2010-06-07 11:38:00  $
//------------------------------------------------------------------------------
//                             Sensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Tuan Nguyen
// Created: 2010/06/07 by Tuan Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Sensor class
 */
//------------------------------------------------------------------------------
#include "Sensor.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SET_REAL_PARA

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Sensor properties
const std::string
Sensor::PARAMETER_TEXT[SensorParamCount - ImagerParamCount] =
{
   "SensorID",
   "HardwareDelay",
};

/// Integer IDs associated with the RFHardware properties
const Gmat::ParameterType
Sensor::PARAMETER_TYPE[SensorParamCount - ImagerParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
};

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sensor(const std::string &type, const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param type The type of Sensor subclass that is constructed.
 * @param name The name of the Sensor object.
 */
//-----------------------------------------------------------------------------
Sensor::Sensor(const std::string &type, const std::string &name) :
   Imager         (Gmat::HARDWARE, type, name),
   signal1        (NULL),
   signal2        (NULL),
   hardwareDelay1 (0.0),
   hardwareDelay2 (0.0),
   isTransmitted1 (false),
   isTransmitted2 (false),
   sensorID       ("")
{
   objectTypes.push_back(Gmat::SENSOR);
   objectTypeNames.push_back("Sensor");
   parameterCount = SensorParamCount;

   for (Integer i = ImagerParamCount; i < SensorParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//-----------------------------------------------------------------------------
// ~Sensor()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
Sensor::~Sensor()
{
}


//-----------------------------------------------------------------------------
// Sensor(const Sensor & sensor) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param senser The sensor element that is copied into this new instance.
 */
//-----------------------------------------------------------------------------
Sensor::Sensor(const Sensor & sensor) :
   Imager        (sensor),
   hardwareDelay1  (sensor.hardwareDelay1),
   hardwareDelay2  (sensor.hardwareDelay2),
   isTransmitted1  (sensor.isTransmitted1),
   isTransmitted2  (sensor.isTransmitted2),
   sensorID        ("")
{
   signal1 = NULL;
   if (sensor.signal1 != NULL)
   {
      signal1 = new Signal();
      signal1->SetEpoch(sensor.signal1->GetEpoch());
      signal1->SetValue(sensor.signal1->GetValue());
   }

   signal2 = NULL;
   if (sensor.signal2 != NULL)
   {
      signal2 = new Signal();
      signal2->SetEpoch(sensor.signal2->GetEpoch());
      signal2->SetValue(sensor.signal2->GetValue());
   }

   for (Integer i = ImagerParamCount; i < SensorParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//-----------------------------------------------------------------------------
// Sensor& operator=(const Sensor& sensor)
//-----------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param sensor The Sensor element that provides the configuration for this
 * instance.
 *
 * @return This object, configured to look like sensor.
 */
//-----------------------------------------------------------------------------
Sensor & Sensor::operator=(const Sensor & sensor)
{
   if (this != &sensor)
   {
      Imager::operator=(sensor);

      hardwareDelay1 = sensor.hardwareDelay1;
      hardwareDelay2 = sensor.hardwareDelay2;
      isTransmitted1 = sensor.isTransmitted1;
      isTransmitted2 = sensor.isTransmitted2;

      if (sensor.signal1 != NULL)
      {
         if (signal1 != NULL)
            *signal1 = *(sensor.signal1);
         else
            signal1 = new Signal(*(sensor.signal1));
      }
      else
      {
         if (signal1 != NULL)
         {
            delete signal1;
            signal1 = NULL;
         }
      }

      if (sensor.signal2 != NULL)
      {
         if (signal2 != NULL)
            *signal2 = *(sensor.signal2);
         else
            signal2 = new Signal(*(sensor.signal2));
      }
      else
      {
         if (signal2 != NULL)
         {
            delete signal2;
            signal2 = NULL;
         }
      }

      sensorID       = "";
   }

   return *this;
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Sensor object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Sensor::GetParameterID(const std::string & str) const
{
   for (Integer i = ImagerParamCount; i < SensorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ImagerParamCount])
      {
         if (IsParameterReadOnly(i))
            throw HardwareException("Error: '" + str + "' parameter was not defined in GMAT " + typeName + "'s syntax.\n");
         
         return i;
      }
   }

   return Imager::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with an Sensor object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Sensor::GetParameterText(const Integer id) const
{
   if (id >= ImagerParamCount && id < SensorParamCount)
      return PARAMETER_TEXT[id - ImagerParamCount];
   return Imager::GetParameterText(id);
}



//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Sensor property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Sensor::GetParameterTypeString(const Integer id) const
{
   return Imager::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the enumerated parameter type for a specified property
 *
 * @param id The property's ID
 *
 * @return The ParameterType of the object.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Sensor::GetParameterType(const Integer id) const
{
   if (id >= ImagerParamCount && id < SensorParamCount)
      return PARAMETER_TYPE[id - ImagerParamCount];

   return Imager::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units for a property
 *
 * @param id The property's ID
 *
 * @return The units for the property
 *
 * @note: This method is not yet used in GMAT.  Once the code uses it, the unit
 * data for the Sensor properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Sensor::GetParameterUnit(const Integer id) const
{
   if (id == HARDWARE_DELAY)
      return "s";               // Unit of hardware delay is (s) second
   if (id == SENSOR_ID)
      return "";                // Sensor ID has no unit

   return Imager::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param label The scriptable label of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Sensor::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are not placed in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Sensor::IsParameterReadOnly(const Integer id) const
{
   if ((id == HARDWARE_DELAY)||(id == SENSOR_ID))
      return true; 

   return Imager::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Sensor::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case SENSOR_ID:
         return sensorID;

      default:
        break;
   }

   return Imager::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer property.
 *
 * @param label The label of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Sensor::GetStringParameter(const std::string &label)const
{
   return GetStringParameter(GetParameterID(label));
}



//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an integer property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return true if the value is set, false otherwise
 */
//------------------------------------------------------------------------------
bool Sensor::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
      case SENSOR_ID:
            sensorID = value;
         return true;

      default:
         break;
   }

   return Imager::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an integer property.
 *
 * @param label The label of the property
 * @param value The new value for the property
 *
 * @return true if the value is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Sensor::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string Sensor::GetStringParameter(const Integer id, const Integer index) const
{
   return Imager::GetStringParameter(id, index);
}


bool Sensor::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   return Imager::SetStringParameter(id, value, index);
}


std::string Sensor::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool Sensor::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 * @param index The index number of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Sensor::GetRealParameter(const Integer id, const Integer index) const
{
   switch (id)
   {
      case HARDWARE_DELAY:
         if (index == 0)
                return hardwareDelay1;
         else
         {
            if(index == 1)
               return hardwareDelay2;
            else
               throw HardwareException("Index is out of bound\n");
         }
      default:
         break;
   }

   return Imager::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Sensor::GetRealParameter(const Integer id) const
{
   if (id == HARDWARE_DELAY)
      return hardwareDelay1;
   return Imager::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 * @param index The index number of the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Sensor::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   #ifdef DEBUG_SET_REAL_PARA
      MessageInterface::ShowMessage("Sensor::SetRealParameter(id = %d, value = %le, index = %d)\n",id, value, index);
   #endif

   switch (id)
   {
      case HARDWARE_DELAY:
         if (index == 0)
         {
            if (value < 0.0)
               throw HardwareException("Error: A negative number was set to " + GetName() + ".HardwareDelay parameter.\n");

            hardwareDelay1 = value;
            return hardwareDelay1;
         }
         else if(index == 1)
         {
            if (value < 0.0)
               throw HardwareException("Error: A negative number was set to " + GetName() + ".HardwareDelay parameter.\n");

            hardwareDelay2 = value;
            return hardwareDelay2;
         }
         else
            throw HardwareException("Index is out of bound\n");

      default:
         break;
   }

   return Imager::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Sensor::SetRealParameter(const Integer id, const Real value)
{
   // Without an index, use delay 1
   if (id == HARDWARE_DELAY)
   {
      if (value < 0.0)
         throw HardwareException("Error: A negative number was set for the " +
               instanceName + ".HardwareDelay parameter.\n");

      hardwareDelay1 = value;
      return hardwareDelay1;
   }

   return Imager::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param index The index number of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Sensor::GetRealParameter(const std::string & label, const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Sensor::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 * @param index The index number of the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Sensor::SetRealParameter(const std::string & label, const Real value,
      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Sensor::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Verifies that the object is ready for use in the Mission Control Sequence
 *
 * @return true if ready, false if not
 */
//------------------------------------------------------------------------------
bool Sensor::Initialize()
{
   return true;
}


//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to get hardware delay.
 *
 * @return hardware delay
 */
//------------------------------------------------------------------------------
Real Sensor::GetDelay(Integer whichOne)
{
   switch(whichOne)
   {
     case 0:
        return hardwareDelay1;
     case 1:
        return hardwareDelay2;
     default:
        return 0.0;
   }
}


//------------------------------------------------------------------------------
// bool SetDelay(Real delay, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to set hardware delay.
 *
 * @return true if the delay is set successfully, false otherwise
 */
//------------------------------------------------------------------------------
bool Sensor::SetDelay(Real delay, Integer whichOne)
{
   switch(whichOne)
   {
   case 0:
      hardwareDelay1 = delay;
      return true;
   case 1:
      hardwareDelay2 = delay;
      return true;
   default:
      return false;
   }
}

//------------------------------------------------------------------------------
// bool IsFeasible(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal is feasible or not.
 *
 * @param whichOne      The index specifying a given signal.
 *                      for Transponder; 1 for input signal, 2 for output signal
 *                      for Transmitter or Receiver; only one signal is used
 *
 * @return      true if it is feasible, false otherwise.
 */
//------------------------------------------------------------------------------
bool Sensor::IsFeasible(Integer whichOne)
{
   return false;
}

//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals.
 *
 * @return the number of signals.
 */
//------------------------------------------------------------------------------
Integer Sensor::GetSignalCount()
{
   return 0;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne      The index specifying a given signal.
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Sensor::IsTransmitted(Integer whichOne)
{
   switch(whichOne)
   {
      case 0:
         return isTransmitted1;
      case 1:
         return isTransmitted2;
      default:
         return false;
   }
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne      The index specifying a given signal.
 *
 * @return      a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Sensor::GetSignal(Integer whichOne)
{
   switch(whichOne)
   {
      case 0:
         return signal1;
      case 1:
         return signal2;
      default:
         return NULL;
   }
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s             The signal needed to set to
 * @param whichOne      The index specifying a given signal.
 *
 * @return      true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Sensor::SetSignal(Signal* s, Integer whichOne)
{
   switch(whichOne)
   {
      case 0:
         signal1 = s;
         break;
      case 1:
         signal2 = s;
         break;
      default:
         return false;
   }
   return true;
}
