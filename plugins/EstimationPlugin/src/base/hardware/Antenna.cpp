//$Id: Antenna.cpp 2010-03-31 $
//------------------------------------------------------------------------------
//                             Antenna
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
// Author: Tuan Dang Nguyen
// Created: 2010/03/31 by Tuan Dang Nguyen
//
/**
 * Implementation for the Antenna class
 */
//------------------------------------------------------------------------------

#include "Antenna.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script RFHardware properties
const std::string
Antenna::PARAMETER_TEXT[AntennaParamCount - ImagerParamCount] =
   {
      "Delay",
      "PhaseCenterLocation1",
      "PhaseCenterLocation2",
      "PhaseCenterLocation3",
   };

/// Integer IDs associated with the RFHardware properties
const Gmat::ParameterType
Antenna::PARAMETER_TYPE[AntennaParamCount - ImagerParamCount] =
   {
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
   };

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Antenna(const std::string &type, const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param type The type of Antenna subclass that is constructed.
 * @param name The name of the Antenna object.
 */
//-----------------------------------------------------------------------------
Antenna::Antenna(const std::string &type, const std::string &name) :
   Imager               (Gmat::HARDWARE, type, name),
   antennaDelay         (0.0),         // (1e-6),
   phaseCenterLocation1 (0.0),
   phaseCenterLocation2 (0.0),
   phaseCenterLocation3 (0.0)
{
   // TODO Auto-generated constructor stub
   objectTypes.push_back(Gmat::ANTENNA);
   objectTypeNames.push_back("Antenna");
   parameterCount = AntennaParamCount;

   for (Integer i = ImagerParamCount; i < AntennaParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//-----------------------------------------------------------------------------
// ~Antenna()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
Antenna::~Antenna()
{
}

//-----------------------------------------------------------------------------
// Antenna(const Antenna & ant) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ant The Antenna element that is copied into this new instance.
 */
//-----------------------------------------------------------------------------
Antenna::Antenna(const Antenna & ant) :
   Imager                  (ant),
   antennaDelay            (ant.antennaDelay),
   phaseCenterLocation1    (ant.phaseCenterLocation1),
   phaseCenterLocation2    (ant.phaseCenterLocation2),
   phaseCenterLocation3    (ant.phaseCenterLocation3)
{
   for (Integer i = ImagerParamCount; i < AntennaParamCount; ++i)
      parameterWriteOrder.push_back(i);
}


//-----------------------------------------------------------------------------
// Antenna& operator=(const Antenna& ant)
//-----------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param ant The Antenna element that provides the configuration for this
 * instance.
 *
 * @return This object, configured to look like ant.
 */
//-----------------------------------------------------------------------------
Antenna & Antenna::operator=(const Antenna & ant)
{
   if (this != &ant)
   {
      Imager::operator=(ant);

      antennaDelay         = ant.antennaDelay;
      phaseCenterLocation1 = ant.phaseCenterLocation1;
      phaseCenterLocation2 = ant.phaseCenterLocation2;
      phaseCenterLocation3 = ant.phaseCenterLocation3;
   }

   return *this;
}

GmatBase* Antenna::Clone() const
{
   return new Antenna(*this);
}

//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Antenna::Copy(const GmatBase* orig)
{
   operator=(*((Antenna *)(orig)));
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Antenna object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Antenna::GetParameterID(const std::string & str) const
{
   for (Integer i = ImagerParamCount; i < AntennaParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ImagerParamCount])
      {
         if (IsParameterReadOnly(i))
            throw HardwareException("Error: Parameter '" + str + "' was not defined in GMAT Antenna's syntax.\n");

         return i;
      }
   }

   return Imager::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with an Antenna object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterText(const Integer id) const
{
   if (id >= ImagerParamCount && id < AntennaParamCount)
      return PARAMETER_TEXT[id - ImagerParamCount];
   return Imager::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Antenna property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterTypeString(const Integer id) const
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
Gmat::ParameterType Antenna::GetParameterType(const Integer id) const
{
   if (id >= ImagerParamCount && id < AntennaParamCount)
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
 * data for the Antenna properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
      case ANTENNA_DELAY:
         return "s";                // Unit of antenna delay is (s) second
      case PHASE_CENTER_LOCATION1:
      case PHASE_CENTER_LOCATION2:
      case PHASE_CENTER_LOCATION3:
         return "";                  // It has no unit
      default:
         break;
   }

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
bool Antenna::IsParameterReadOnly(const std::string& label) const
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
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Antenna::IsParameterReadOnly(const Integer id) const
{
   // Inactive fields: Delay, PhaseCenterLocation1,  PhaseCenterLocation2, and PhaseCenterLocation3
   if ((id == ANTENNA_DELAY)||(id == PHASE_CENTER_LOCATION1)||(id == PHASE_CENTER_LOCATION2)||(id == PHASE_CENTER_LOCATION3))
      return true;

   return Imager::IsParameterReadOnly(id);
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
Real Antenna::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case ANTENNA_DELAY:
         return antennaDelay;

      case PHASE_CENTER_LOCATION1:
         return phaseCenterLocation1;

      case PHASE_CENTER_LOCATION2:
         return phaseCenterLocation2;

      case PHASE_CENTER_LOCATION3:
         return phaseCenterLocation3;

//      default:
//         break;
   }

   return Imager::GetRealParameter(id);
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
Real Antenna::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
      case ANTENNA_DELAY:
         if (value >= 0.0)
            antennaDelay = value;
         return antennaDelay;

      case PHASE_CENTER_LOCATION1:
            phaseCenterLocation1 = value;
         return phaseCenterLocation1;

      case PHASE_CENTER_LOCATION2:
            phaseCenterLocation2 = value;
         return phaseCenterLocation2;

      case PHASE_CENTER_LOCATION3:
            phaseCenterLocation3 = value;
         return phaseCenterLocation3;

      default:
         break;
   }

   return Imager::SetRealParameter(id, value);
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
Real Antenna::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
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
Real Antenna::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

