//$Id: RFHardware.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             RFHardware
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
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the RFHardware class
 */
//------------------------------------------------------------------------------
#include "RFHardware.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script RFHardware properties
const std::string
RFHardware::PARAMETER_TEXT[RFHardwareParamCount - SensorParamCount] =
   {
      "PrimaryAntenna",
   };

/// Integer IDs associated with the RFHardware properties
const Gmat::ParameterType
RFHardware::PARAMETER_TYPE[RFHardwareParamCount - SensorParamCount] =
   {
      Gmat::OBJECT_TYPE,
   };

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// RFHardware(const std::string &type, const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param type The type of RFHardware subclass that is constructed.
 * @param name The name of the RDHardware object.
 */
//-----------------------------------------------------------------------------
RFHardware::RFHardware(const std::string &type, const std::string &name) :
   Sensor             (type, name),
   primaryAntenna     (NULL),
   primaryAntennaName ("")
{
   objectTypes.push_back(Gmat::RF_HARDWARE);
   objectTypeNames.push_back("RFHardware");
   parameterCount = RFHardwareParamCount;

   for (Integer i = SensorParamCount; i < RFHardwareParamCount; i++)
      parameterWriteOrder.push_back(i);
}


//-----------------------------------------------------------------------------
// ~RFHardware()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
RFHardware::~RFHardware()
{
}


//-----------------------------------------------------------------------------
// RFHardware(const RFHardware & rfh) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rfh The RFHardware element that is copied into this new instance.
 */
//-----------------------------------------------------------------------------
RFHardware::RFHardware(const RFHardware & rfh) :
   Sensor               (rfh),
   primaryAntenna       (NULL),
   primaryAntennaName   (rfh.primaryAntennaName)
{
   for (Integer i = SensorParamCount; i < RFHardwareParamCount; i++)
      parameterWriteOrder.push_back(i);
}


//-----------------------------------------------------------------------------
// RFHardware& operator=(const RFHardware& rfh)
//-----------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param rfh The RFHardware element that provides the configuration for this
 * instance.
 *
 * @return This object, configured to look like rfh.
 */
//-----------------------------------------------------------------------------
RFHardware & RFHardware::operator=(const RFHardware & rfh)
{
   if (this != &rfh)
   {
      Sensor::operator=(rfh);

      primaryAntenna     = rfh.primaryAntenna;
      primaryAntennaName = rfh.primaryAntennaName;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * RFHardware object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer RFHardware::GetParameterID(const std::string & str) const
{
   for (Integer i = SensorParamCount; i < RFHardwareParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SensorParamCount])
         return i;
   }

   return Sensor::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with an RFHardware object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetParameterText(const Integer id) const
{
   if (id >= SensorParamCount && id < RFHardwareParamCount)
      return PARAMETER_TEXT[id - SensorParamCount];
   return Sensor::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the RFHardware property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetParameterTypeString(const Integer id) const
{
   return Sensor::PARAM_TYPE_STRING[GetParameterType(id)];
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
Gmat::ParameterType RFHardware::GetParameterType(const Integer id) const
{
   if (id >= SensorParamCount && id < RFHardwareParamCount)
      return PARAMETER_TYPE[id - SensorParamCount];

   return Sensor::GetParameterType(id);
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
 * data for the RFHardware properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetParameterUnit(const Integer id) const
{
   if (PRIMARY_ANTENNA)
      return "";

   return Sensor::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case PRIMARY_ANTENNA:
      return primaryAntennaName;
   default:
      return Sensor::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool RFHardware::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case PRIMARY_ANTENNA:
      if (value == "")
         throw HardwareException("Error: Name of primary antenna set to " + GetName() + "is an empty string.\n");

      if (!GmatStringUtil::IsValidIdentity(value))
         throw HardwareException("Error: '" + value + "' set to " + GetName() + ".PrimaryAntenna parameter is an invalid name.\n");

      primaryAntennaName = value;
      return true;
   default:
      return Sensor::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool RFHardware::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string RFHardware::GetStringParameter(const Integer id, const Integer index) const
{
   return Sensor::GetStringParameter(id, index);
}


bool RFHardware::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   return Sensor::SetStringParameter(id, value, index);
}


std::string RFHardware::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool RFHardware::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return A pointer to the object, or NULL if the object is not set
 */
//------------------------------------------------------------------------------
GmatBase* RFHardware::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   // return primary antenna when it is requested:
   if ((type == Gmat::HARDWARE)&&(name == primaryAntennaName))
   {
      return (GmatBase*)primaryAntenna;
   }

   return Sensor::GetRefObject(type, name);
}


GmatBase* RFHardware::GetRefObject(const UnsignedInt type, const std::string &name, const Integer index)
{
   return Sensor::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object
 *
 * @param obj The reference object
 * @param type The type of the object
 * @param name The object's name
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool RFHardware::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   if ((type == Gmat::HARDWARE)&&(name == primaryAntennaName))
   {
      primaryAntenna = (Antenna *)obj;
      return true;
   }

   return Sensor::SetRefObject(obj, type, name);
}


bool RFHardware::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string &name, const Integer index)
{
   return Sensor::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns a name of the referenced objects.
 *
 * @return a name of objects of the requested type.
 */
//------------------------------------------------------------------------------
std::string RFHardware::GetRefObjectName(const UnsignedInt type) const
{
   if ((type == Gmat::HARDWARE)||(type == Gmat::UNKNOWN_OBJECT))
   {
      if (primaryAntennaName == "")
         throw HardwareException("Error: value of " + GetName() + ".PrimaryAntenna parameter was not set in GMAT script.\n");

      return primaryAntennaName;
   }

   return Sensor::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectNameArray(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns an array of names of the referenced objects.
 *
 * @return an array of names of the objects of the requested type.
 */
//------------------------------------------------------------------------------
const StringArray& RFHardware::GetRefObjectNameArray(const UnsignedInt type)
{
   Sensor::GetRefObjectNameArray(type);

   switch(type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::HARDWARE:
      //refObjectNames.clear();
      if (primaryAntennaName == "")
         throw HardwareException("Error: value of " + GetName() + ".PrimaryAntenna was not set in GMAT script.\n");

      refObjectNames.push_back(primaryAntennaName);
      //return refObjectNames;
        break;

   default:
      break;
   }

   //return Sensor::GetRefObjectNameArray(type);
   return refObjectNames;
}


//------------------------------------------------------------------------------
const ObjectTypeArray& RFHardware::GetRefObjectTypeArray()
{
   // Primary antenna is a referenced object of RFHardware. Primary antenna
   // has type of Gmat:HARDWARE. We define it in refObjectTypes as shown below:
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::HARDWARE);
   return refObjectTypes;

//      return Hardware::GetRefObjectTypeArray();
}


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool RFHardware::HasRefObjectTypeArray()
{
   // It has the referenced object type array as it was defined in
   // RFHardware::GetRefObjectTypeArray() function.
   return true;
}


UnsignedInt RFHardware::GetPropertyObjectType(const Integer id) const
{
   if (id == PRIMARY_ANTENNA)
      return Gmat::ANTENNA;
   else
      return Gmat::UNKNOWN_OBJECT;
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
bool RFHardware::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Initializing RFHardware object \"%s\"\n",
            instanceName.c_str());
   #endif
   bool retval = false;

   if (Sensor::Initialize())
   {
      if (primaryAntenna != NULL)
         retval = true;
      else
         MessageInterface::ShowMessage("Primary antenna not set for the %s \"%s\"\n",
            typeName.c_str(), instanceName.c_str());
   }

   return retval;
}

