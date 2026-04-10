//$Id$
//------------------------------------------------------------------------------
//                           IntrusionLocator
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: May 10, 2021
//
/**
 * Definition of the IntrusionLocator
 */
 //------------------------------------------------------------------------------


#include "IntrusionLocator.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "EventException.hpp"
#include "EphemManager.hpp"
#include "StringUtil.hpp"
#include "Imager.hpp"
#include "SpiceInterface.hpp"
#include "SpiceFrameKernelWriter.hpp"
#include "FileUtil.hpp"
#include "IntrusionEvent.hpp"

//#define DEBUG_SET
//#define DEBUG_SETREF
//#define DEBUG_CONTACT_LOCATOR_WRITE
//#define DEBUG_CONTACT_EVENTS
//#define DEBUG_INIT_FINALIZE
//#define DEBUG_IntrusionLocator_INIT

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string IntrusionLocator::PARAMETER_TEXT[
   IntrusionLocatorParamCount - EventLocatorParamCount] =
   {
      "Sensors",            // IMAGERS
      "CentralBody",
      "IntrudingBodies",
      "MinimumPhase",
      "ReportCoordinates",
      "SpiceGridFrameFile"
   };

const Gmat::ParameterType IntrusionLocator::PARAMETER_TYPE[
   IntrusionLocatorParamCount - EventLocatorParamCount] =
   {
      Gmat::OBJECTARRAY_TYPE, // IMAGERS
      Gmat::OBJECT_TYPE,
      Gmat::OBJECTARRAY_TYPE,
      Gmat::REAL_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
   };


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IntrusionLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
IntrusionLocator::IntrusionLocator(const std::string &name) :
   EventLocator("IntrusionLocator", name),
   centralBodyName("Earth"),
   centralBody(NULL),
   minimumPhase(0.0),
   reportCoordinates("SensorFrame"),
   spice(NULL),
   gridFrameFile(""),
   scFKFileName("")
{
   objectTypeNames.push_back("IntrusionLocator");
   parameterCount = IntrusionLocatorParamCount;

   sensorNames.clear();
   sensors.clear();

   intrudingBodyNames.clear();
   intrudingBodies.clear();

   // Override default stepSize for the IntrusionLocator
   stepSize = 10;

   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("CREATED new IntrusionLocator %s<%p>\n",
         instanceName.c_str(), this);
   #endif
}

//------------------------------------------------------------------------------
// ~IntrusionLocator()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
IntrusionLocator::~IntrusionLocator()
{
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("DESTRUCTING IntrusionLocator %s<%p>\n",
         instanceName.c_str(), this);
   #endif

   // Remove the temp files
   if (spice)
   {
      for (Integer fileIdx = 0; fileIdx < instFKFileNames.size(); ++fileIdx)
         spice->UnloadKernel(instFKFileNames.at(fileIdx));

      for (Integer fileIdx = 0; fileIdx < ikFileNames.size(); ++fileIdx)
         spice->UnloadKernel(ikFileNames.at(fileIdx));

      spice->UnloadKernel(scFKFileName);
   }

   for (Integer fileIdx = 0; fileIdx < instFKFileNames.size(); ++fileIdx)
      remove(instFKFileNames.at(fileIdx).c_str());

   for (Integer fileIdx = 0; fileIdx < ikFileNames.size(); ++fileIdx)
      remove(ikFileNames.at(fileIdx).c_str());

   remove(scFKFileName.c_str());

   TakeAction("Clear");
}

//------------------------------------------------------------------------------
// IntrusionLocator(const IntrusionLocator &cl)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param cl The original being copied
*/
//------------------------------------------------------------------------------
IntrusionLocator::IntrusionLocator(const IntrusionLocator &cl) :
   EventLocator(cl),
   centralBodyName(cl.centralBodyName),
   centralBody(cl.centralBody),
   minimumPhase(cl.minimumPhase),
   reportCoordinates(cl.reportCoordinates),
   spice(NULL),
   gridFrameFile(cl.gridFrameFile),
   instFKFileNames(cl.instFKFileNames),
   scFKFileName(cl.scFKFileName),
   ikFileNames(cl.ikFileNames)
{
   sensorNames.clear();
   sensors.clear();
   for (Integer jj = 0; jj < cl.sensorNames.size(); jj++)
      sensorNames.push_back(cl.sensorNames.at(jj));
   for (Integer jj = 0; jj < cl.sensors.size(); jj++)
      sensors.push_back(cl.sensors.at(jj));

   intrudingBodyNames.clear();
   intrudingBodies.clear();
   for (Integer jj = 0; jj < cl.intrudingBodyNames.size(); jj++)
      intrudingBodyNames.push_back(cl.intrudingBodyNames.at(jj));
   for (Integer jj = 0; jj < cl.intrudingBodies.size(); jj++)
      intrudingBodies.push_back(cl.intrudingBodies.at(jj));

   TakeAction("Clear", "Events");
   IntrusionResult *toCopy = NULL;
   IntrusionResult *newEvent = NULL;
   for (Integer ii = 0; ii < cl.intrusionResults.size(); ii++)
   {
      toCopy = cl.intrusionResults.at(ii);
      newEvent = new IntrusionResult(*toCopy);
      intrusionResults.push_back(newEvent);
   }

   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage(
         "CREATED new IntrusionLocator %s<%p> by copying %s<%p>\n",
         instanceName.c_str(), this, cl.instanceName.c_str(), &cl);
   #endif
}


//------------------------------------------------------------------------------
// IntrusionLocator& operator=(const IntrusionLocator &c)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param c The IntrusionLocator providing parameters for this one
*
* @return This IntrusionLocator
*/
//------------------------------------------------------------------------------
IntrusionLocator& IntrusionLocator::operator=(const IntrusionLocator &c)
{
   if (this != &c)
   {
      EventLocator::operator=(c);

      centralBodyName = c.centralBodyName;
      centralBody = c.centralBody;
      minimumPhase = c.minimumPhase;
      reportCoordinates = c.reportCoordinates;
      spice = NULL;
      gridFrameFile = c.gridFrameFile;
      instFKFileNames = c.instFKFileNames;
      scFKFileName = c.scFKFileName;
      ikFileNames = c.ikFileNames;

      sensorNames.clear();
      sensors.clear();
      for (Integer jj = 0; jj < c.sensorNames.size(); jj++)
         sensorNames.push_back(c.sensorNames.at(jj));
      for (Integer jj = 0; jj < c.sensors.size(); jj++)
         sensors.push_back(c.sensors.at(jj));

      intrudingBodyNames.clear();
      intrudingBodies.clear();
      for (Integer jj = 0; jj < c.intrudingBodyNames.size(); jj++)
         intrudingBodyNames.push_back(c.intrudingBodyNames.at(jj));
      for (Integer jj = 0; jj < c.intrudingBodies.size(); jj++)
         intrudingBodies.push_back(c.intrudingBodies.at(jj));

      TakeAction("Clear", "Events");
      IntrusionResult *toCopy = NULL;
      IntrusionResult *newEvent = NULL;
      for (Integer ii = 0; ii < c.intrusionResults.size(); ii++)
      {
         toCopy = c.intrusionResults.at(ii);
         newEvent = new IntrusionResult(*toCopy);
         intrusionResults.push_back(newEvent);
      }

      #ifdef DEBUG_INIT_FINALIZE
            MessageInterface::ShowMessage(
               "COPIED to IntrusionLocator %s<%p> from %s<%p>\n",
               instanceName.c_str(), this, c.instanceName.c_str(), &c);
      #endif
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
* Creates a replica of this instance
*
* @return A pointer to the new instance
*/
//------------------------------------------------------------------------------
GmatBase *IntrusionLocator::Clone() const
{
   return new IntrusionLocator(*this);
}

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
* Sets this object to match another one.
*
* @param orig The original that is being copied.
*/
//---------------------------------------------------------------------------
void IntrusionLocator::Copy(const GmatBase* orig)
{
   operator=(*((IntrusionLocator *)(orig)));
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves the scripted name for a parameter
*
* @param id The parameter's id
*
* @return The script string
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetParameterText(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < IntrusionLocatorParamCount)
      return PARAMETER_TEXT[id - EventLocatorParamCount];

   return EventLocator::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//------------------------------------------------------------------------------
/**
* Retrieves the ID for a parameter
*
* @param str The script string for the parameter
*
* @return The parameter's id
*/
//------------------------------------------------------------------------------
Integer IntrusionLocator::GetParameterID(const std::string & str) const
{
   for (Integer i = EventLocatorParamCount; i < IntrusionLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EventLocatorParamCount])
         return i;
   }

   return EventLocator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves the type for a parameter
*
* @param id The parameter's id
*
* @return The parameter type
*/
//------------------------------------------------------------------------------
Gmat::ParameterType IntrusionLocator::GetParameterType(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < IntrusionLocatorParamCount)
      return PARAMETER_TYPE[id - EventLocatorParamCount];

   return EventLocator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a string describing a parameter's type
*
* @param id The parameter's id
*
* @return The type description
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter
*
* @param id The parameter's id
*
* @return The parameter data, a string
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetStringParameter(const Integer id) const
{
   if (id == REPORT_COORDINATES)
      return reportCoordinates;
   if (id == CENTRAL_BODY)
      return centralBodyName;
   if (id == SPICE_GRID_FRAME_FILE)
      return gridFrameFile;

   return EventLocator::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
* Sets the contents of a string parameter
*
* @param id The parameter's id
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::SetStringParameter(const Integer id,
   const std::string &value)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("In SetStringParameter with id = %d, value = %s\n",
         id, value.c_str());
   #endif
   if (id == REPORT_COORDINATES)
   {
      std::string reportCoordList = "SensorFrame, FixedGrid";
      if ((value == "SensorFrame") || (value == "FixedGrid"))
      {
         reportCoordinates = value;
         return true;
      }
      else
      {
         std::string errmsg =
            "The value of \"" + value + "\" for field \"ReportCoordinates\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + reportCoordList + " ]\n. ";
         #ifdef DEBUG_SET
                  MessageInterface::ShowMessage("ERROR message is: \"%s\"\n", errmsg.c_str()); // *******
         #endif
         EventException ee;
         ee.SetDetails(errmsg);
         throw ee;
      }
   }
   if (id == SENSORS)
   {
      #ifdef DEBUG_SET
            MessageInterface::ShowMessage("--- Attempting to add an Imager to list of sensors ...\n");
      #endif
      if (find(sensorNames.begin(), sensorNames.end(), value) ==
         sensorNames.end())
      {
         sensorNames.push_back(value);
         #ifdef DEBUG_SET
                  MessageInterface::ShowMessage("--- Just added \"%s\" to list of sensors ...\n",
                     value.c_str());
         #endif
      }
      return true;
   }
   if (id == CENTRAL_BODY)
   {
      centralBodyName = value;
      return true;
   }
   if (id == INTRUDING_BODIES)
   {
      if (find(intrudingBodyNames.begin(), intrudingBodyNames.end(), value) ==
         intrudingBodyNames.end())
      {
         intrudingBodyNames.push_back(value);
      }
      return true;
   }
   if (id == SPICE_GRID_FRAME_FILE)
   {
      gridFrameFile = value;
      return true;
   }

   return EventLocator::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter from a string array
*
* @param id The parameter's id
* @param index The index into the array
*
* @return The parameter string
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetStringParameter(const Integer id,
   const Integer index) const
{
   if (id == SENSORS)
   {
      if (index < (Integer)sensorNames.size())
         return sensorNames[index];
      else
         throw EventException(
            "Index out of range when trying to access sensor list for " +
            instanceName);
   }
   if (id == INTRUDING_BODIES)
   {
      if (index < (Integer)intrudingBodyNames.size())
         return intrudingBodyNames[index];
      else
         throw EventException(
            "Index out of range when trying to access intruding body list "
            "for " + instanceName);
   }

   return EventLocator::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
* Sets a parameter value in a string array
*
* @param id The parameter's id
* @param value The new parameter value
* @param index The index of the parameter in the array
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::SetStringParameter(const Integer id,
   const std::string & value, const Integer index)
{
   if (id == SENSORS)
   {
      #ifdef DEBUG_SET
            MessageInterface::ShowMessage("--- Attempting to add an Imager "
               "(index = %d) to list of imagers ...\n", index);
      #endif
      if (find(sensorNames.begin(), sensorNames.end(), value) ==
            sensorNames.end())
      {
         if (index < (Integer)sensorNames.size())
         {
            sensorNames[index] = value;
            return true;
         }
         else
         {
            sensorNames.push_back(value);
            return true;
         }
      }
      else
      {
         // ignore duplicate imager names, for now
         return true;
      }
   }
   if (id == INTRUDING_BODIES)
   {
      #ifdef DEBUG_SET
            MessageInterface::ShowMessage("--- Attempting to add an intruding "
               "body (index = %d) to list of intuding bodies ...\n", index);
      #endif
      if (find(intrudingBodyNames.begin(), intrudingBodyNames.end(), value) ==
            intrudingBodyNames.end())
      {
         if (index < (Integer)intrudingBodyNames.size())
         {
            intrudingBodyNames[index] = value;
            return true;
         }
         else
         {
            intrudingBodyNames.push_back(value);
            return true;
         }
      }
      else
      {
         // ignore duplicate intuding body names, for now
         return true;
      }
   }

   return EventLocator::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray & GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a StringArray parameter
*
* @param id The parameter's id
*
* @return The StringArray
*/
//------------------------------------------------------------------------------
const StringArray & IntrusionLocator::GetStringArrayParameter(
   const Integer id) const
{
   if (id == SENSORS)
      return sensorNames;
   if (id == INTRUDING_BODIES)
      return intrudingBodyNames;

   return EventLocator::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a StringArray parameter from an array of StringArrays
*
* @param id The parameter's id
* @param index The index into the array
*
* @return The StringArray
*/
//------------------------------------------------------------------------------
const StringArray& IntrusionLocator::GetStringArrayParameter(const Integer id,
   const Integer index) const
{
   return EventLocator::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter
*
* @param label The parameter's scripted identifier
*
* @return The parameter data, a string
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
* Sets the contents of a string parameter
*
* @param label The parameter's scripted identifier
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::SetStringParameter(const std::string & label,
   const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter from a string array
*
* @param label The parameter's scripted identifier
* @param index The index into the array
*
* @return The parameter string
*/
//------------------------------------------------------------------------------
std::string IntrusionLocator::GetStringParameter(const std::string &label,
   const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
* Sets a parameter value in a string array
*
* @param label The parameter's scripted identifier
* @param value The new parameter value
* @param index The index of the parameter in the array
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::SetStringParameter(const std::string & label,
   const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
* Retrieves a StringArray parameter
*
* @param label The parameter's scripted identifier
*
* @return The StringArray
*/
//------------------------------------------------------------------------------
const StringArray & IntrusionLocator::GetStringArrayParameter(
   const std::string & label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a StringArray parameter from an array of StringArrays
*
* @param label The parameter's scripted identifier
* @param index The index into the array
*
* @return The StringArray
*/
//------------------------------------------------------------------------------
const StringArray & IntrusionLocator::GetStringArrayParameter(
   const std::string & label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real IntrusionLocator::GetRealParameter(const Integer id) const
{
   if (id == MINIMUM_PHASE)
      return minimumPhase;

   return EventLocator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real IntrusionLocator::SetRealParameter(const Integer id,
   const Real value)
{
   if (id == MINIMUM_PHASE)
   {
      if (value < 0.0 || value > 1.0)
      {
         std::string errmsg = "*** Error *** The value" +
            GmatStringUtil::ToString(value) + "for field MinimumPhase on "
            "object \"" + instanceName + "\" is not an allowed value. "
            "Allowed values are [0.0 < Real < 1.0]";
         throw EventException(errmsg);
      }

      minimumPhase = value;
      return minimumPhase;
   }

   return EventLocator::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real IntrusionLocator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real IntrusionLocator::SetRealParameter(const std::string &label,
   const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
 //------------------------------------------------------------------------------
bool IntrusionLocator::GetBooleanParameter(const Integer id) const
{
   return EventLocator::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
 //------------------------------------------------------------------------------
bool IntrusionLocator::SetBooleanParameter(const Integer id, const bool value)
{
   return EventLocator::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
 //------------------------------------------------------------------------------
bool IntrusionLocator::GetBooleanParameter(const std::string& label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
 //------------------------------------------------------------------------------
bool IntrusionLocator::SetBooleanParameter(const std::string& label,
   const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieves object type of parameter of given id.
*
* @param <id> ID for the parameter.
*
* @return parameter ObjectType
*/
//---------------------------------------------------------------------------
UnsignedInt IntrusionLocator::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case SENSORS:
      return Gmat::HARDWARE;
   case CENTRAL_BODY:
      return Gmat::CELESTIAL_BODY;
   case INTRUDING_BODIES:
      return Gmat::SPACE_POINT;
   default:
      return EventLocator::GetPropertyObjectType(id);
   }
}

//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieves eumeration symbols of parameter of given id.
*
* @param <id> ID for the parameter.
*
* @return list of enumeration symbols
*/
//---------------------------------------------------------------------------
const StringArray& IntrusionLocator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case REPORT_COORDINATES:
      enumStrings.clear();
      enumStrings.push_back("SensorFrame");
      enumStrings.push_back("FixedGrid");
      return enumStrings;
   default:
      return EventLocator::GetPropertyEnumStrings(id);
   }
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const Integer id)
//------------------------------------------------------------------------------
/**
* Retrieves a list of types that need to be shown on a GUI for a parameter
*
* @param id The parameter ID
*
* @return The list of types
*/
//------------------------------------------------------------------------------
const ObjectTypeArray& IntrusionLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();  // ??
   if (id == SENSORS)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::HARDWARE) ==
         listedTypes.end())
         listedTypes.push_back(Gmat::HARDWARE);
      return listedTypes;
   }
   if (id == CENTRAL_BODY)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::CELESTIAL_BODY) ==
         listedTypes.end())
         listedTypes.push_back(Gmat::CELESTIAL_BODY);
      return listedTypes;
   }
   if (id == INTRUDING_BODIES)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::CELESTIAL_BODY) ==
         listedTypes.end())
         listedTypes.push_back(Gmat::CELESTIAL_BODY);
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::SPACECRAFT) ==
         listedTypes.end())
         listedTypes.push_back(Gmat::SPACECRAFT);
      return listedTypes;
   }

   return EventLocator::GetTypesForList(id);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const std::string &label)
//------------------------------------------------------------------------------
/**
* Retrieves a list of types that need to be shown on a GUI for a parameter
*
* @param label The parameter's identifying string
*
* @return The list of types
*/
//------------------------------------------------------------------------------
const ObjectTypeArray& IntrusionLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
* This method returns a pointer to a reference object contained in a vector of
* objects in the BodyFixedPoint class.
*
* @param type type of the reference object requested
* @param name name of the reference object requested
*
* @return pointer to the reference object requested.
*/
//------------------------------------------------------------------------------
GmatBase* IntrusionLocator::GetRefObject(const UnsignedInt type,
   const std::string &name)
{
   switch (type)
   {
   case Gmat::HARDWARE:
      for (UnsignedInt ii = 0; ii < sensorNames.size(); ii++)
      {
         if (name == sensorNames.at(ii))
         {
            return sensors.at(ii);
         }
      }
      break;

   case Gmat::CELESTIAL_BODY:
   case Gmat::SPACECRAFT:
      if (name == centralBodyName)
         return centralBody;

      for (UnsignedInt ii = 0; ii < intrudingBodyNames.size(); ii++)
      {
         if (name == intrudingBodyNames.at(ii))
         {
            return intrudingBodies.at(ii);
         }
      }
      break;

   default:
      break;
   }
   return EventLocator::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
* This method sets a pointer to a reference object in a vector of objects in
* the BodyFixedPoint class.
*
* @param obj The reference object.
* @param type type of the reference object requested
* @param name name of the reference object requested
*
* @return true if successful; otherwise, false.
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string &name)
{
#ifdef DEBUG_SETREF
   MessageInterface::ShowMessage("CL::SetRefObject, obj = %s, type = %d (%s), name= %s\n",
      (obj ? "NOT NULL" : "NULL"), (Integer)type, OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT].c_str(),
      name.c_str());
   MessageInterface::ShowMessage("station names are:\n");
   for (Integer ii = 0; ii < stationNames.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", stationNames.at(ii).c_str());
#endif
   switch (type)
   {
   case Gmat::HARDWARE:
      for (UnsignedInt ii = 0; ii < sensorNames.size(); ii++)
      {
         if (name == sensorNames.at(ii))
         {
            if (obj->GetTypeName() == "Imager")
            {
               sensors.push_back(obj);
               return true;
            }
            else
            {
               std::string errmsg = "*** Error *** The object" +
                  name + "for field Sensors on "
                  "object \"" + instanceName + "\" is not an allowed value. "
                  "Allowed values are Imager objects.\n";;
               throw EventException(errmsg);
            }
         }
      }
      break;

   case Gmat::CELESTIAL_BODY:
   case Gmat::SPACECRAFT:
      {
         bool bodyFound = false;
         if (name == centralBodyName)
         {
            centralBody = (CelestialBody*)obj;
            bodyFound = true;
         }

         for (UnsignedInt ii = 0; ii < intrudingBodyNames.size(); ii++)
         {
            if (name == intrudingBodyNames.at(ii))
            {
               bodyFound = false;
               for (Integer jj = 0; jj < intrudingBodies.size(); ++jj)
               {
                  if (name == intrudingBodies.at(jj)->GetName())
                  {
                     bodyFound = true;
                     break;
                  }
               }
               if (!bodyFound)
               {
                  intrudingBodies.push_back(obj);
                  bodyFound = true;
               }
               break;
            }
         }

         if (bodyFound)
            return true;
         break;
      }
   default:
      break;
   }

   // Call parent class to add objects to bodyList
   return EventLocator::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
* @see GmatBase
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
* Returns array of names of reference objects of the provided type, or all if 
* unknown
*/
//------------------------------------------------------------------------------
const StringArray& IntrusionLocator::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_BF_REF
      MessageInterface::ShowMessage("In BFP::GetRefObjectNameArray, requesting type %d (%s)\n",
         (Integer)type, (GmatBase::OBJECT_TYPE_STRING[type]).c_str());
   #endif

   refObjectNames = EventLocator::GetRefObjectNameArray(type);

   if (type == Gmat::HARDWARE)
   {
      refObjectNames.insert(refObjectNames.begin(), sensorNames.begin(),
         sensorNames.end());
   }
   if (type == Gmat::SPACE_POINT)
   {
      refObjectNames.push_back(centralBodyName);
      refObjectNames.insert(refObjectNames.begin(), intrudingBodyNames.begin(),
         intrudingBodyNames.end());
   }
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      refObjectNames.insert(refObjectNames.begin(), sensorNames.begin(),
         sensorNames.end());
      refObjectNames.push_back(centralBodyName);
      refObjectNames.insert(refObjectNames.begin(), intrudingBodyNames.begin(),
         intrudingBodyNames.end());
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
* Retrieves the list of ref object types used by this class.
*
* @return the list of object types.
*
*/
//------------------------------------------------------------------------------
const ObjectTypeArray& IntrusionLocator::GetRefObjectTypeArray()
{
   refObjectTypes = EventLocator::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::HARDWARE);
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
* Interface used to support user renames of object references.
*
* @param type reference object type.
* @param oldName object name to be renamed.
* @param newName new object name.
*
* @return true if object name changed, false if not.
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::RenameRefObject(const UnsignedInt type,
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
   case Gmat::HARDWARE:
      for (UnsignedInt i = 0; i < sensorNames.size(); ++i)
      {
         if (sensorNames[i] == oldName)
         {
            sensorNames[i] = newName;
            retval = true;
            return retval;
         }
      }
      break;
   case Gmat::CELESTIAL_BODY:
   case Gmat::SPACECRAFT:
      if (centralBodyName == oldName)
      {
         centralBodyName = newName;
         retval = true;
         return retval;
      }

      for (UnsignedInt i = 0; i < intrudingBodyNames.size(); ++i)
      {
         if (intrudingBodyNames[i] == oldName)
         {
            intrudingBodyNames[i] = newName;
            retval = true;
            return retval;
         }
      }
      break;
   case Gmat::UNKNOWN_OBJECT:
      for (UnsignedInt i = 0; i < sensorNames.size(); ++i)
      {
         if (sensorNames[i] == oldName)
         {
            sensorNames[i] = newName;
            retval = true;
            return retval;
         }
      }

      if (centralBodyName == oldName)
      {
         centralBodyName = newName;
         retval = true;
         return retval;
      }

      for (UnsignedInt i = 0; i < intrudingBodyNames.size(); ++i)
      {
         if (intrudingBodyNames[i] == oldName)
         {
            intrudingBodyNames[i] = newName;
            retval = true;
            return retval;
         }
      }
      break;
   default:
      ;     // Intentional drop-through
   }

   retval = (retval || EventLocator::RenameRefObject(type, oldName, newName));

   return retval;
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
* Performs a custom action on the object.
*
* Event locators use this method to clear arrays in the locator.
*
* @param action The string specifying the action to be taken
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::TakeAction(const std::string &action,
   const std::string &actionData)
{

   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "Sensors") || (actionData == ""))
      {
         sensorNames.clear();
         sensors.clear();
         retval = true;
      }
      else if ((actionData == "IntrudingBodies") || (actionData == ""))
      {
         intrudingBodyNames.clear();
         intrudingBodies.clear();
         retval = true;
      }
      else if ((actionData == "Events") || (actionData == ""))
      {
         #ifdef DEBUG_ECLIPSE_ACTION
                  MessageInterface::ShowMessage(
                     "In ContactLocator::TakeAction, about to clear %d events\n",
                     (Integer)contactResults.size());
         #endif
         for (Integer ii = 0; ii < intrusionResults.size(); ii++)
         {
            intrusionResults.at(ii)->TakeAction("Clear", "Events");
            delete intrusionResults.at(ii);
         }
         intrusionResults.clear();
         retval = true;
      }

      return (EventLocator::TakeAction(action, actionData) || retval);
   }

   return EventLocator::TakeAction(action, actionData);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Prepares the locator for use
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::Initialize()
{
   bool retval = false;
#ifdef DEBUG_IntrusionLocator_INIT
   MessageInterface::ShowMessage("In CL::Init for %s\n", instanceName.c_str());
#endif

   for (Integer i = 0; i < intrudingBodyNames.size(); ++i)
   {
      if (intrudingBodyNames.at(i) == targetName)
      {
         std::string errmsg = "Error in IntrusionLocator::Initialize(), "
            "the spacecraft \"" + targetName + "\" is being used as both a "
            "target and observer in IntrusionLocator \"";
         errmsg += instanceName + "\".\n";
         throw EventException(errmsg);
      }
   }

   if (EventLocator::Initialize())
   {
      if (sensorNames.size() != sensors.size())
      {
         std::string errmsg = "Error setting imagers on IntrusionLocator ";
         errmsg += instanceName + "\n";
         throw EventException(errmsg);
      }
      for (UnsignedInt ii = 0; ii < sensors.size(); ++ii)
      {
         if (sensors.at(ii) == NULL)
         {
            std::string errmsg = "Error setting imagers on IntrusionLocator ";
            errmsg += instanceName + "\n";
            throw EventException(errmsg);
         }
      }

      for (UnsignedInt ii = 0; ii < intrudingBodies.size(); ++ii)
      {
         if (intrudingBodyNames.at(ii) == centralBodyName)
         {
            MessageInterface::ShowMessage("*** WARNING *** The target "
               "intruding body \"" + intrudingBodyNames.at(ii) + "\" on "
               "locator \"" + instanceName + "\" is the same as the "
               "selected central body. This body will be skipped when "
               "checking for intrusion events.\n");
            intrudingBodyNames.erase(intrudingBodyNames.begin() + ii);
            for (Integer jj = 0; jj < intrudingBodies.size(); ++jj)
            {
               if (intrudingBodies.at(jj)->GetName() == centralBodyName)
               {
                  intrudingBodies.erase(intrudingBodies.begin() + jj);
                  break;
               }
            }
            break;
         }
      }

      if (runMode != "Disabled")
      {
         if (!spice)
            spice = new SpiceInterface();

         // If we have any spacecraft targets, they need an SPK
         for (Integer i = 0; i < intrudingBodies.size(); ++i)
         {
            if (intrudingBodies.at(i)->IsOfType(Gmat::SPACECRAFT))
               ((Spacecraft*)intrudingBodies.at(i))->RecordEphemerisData();
         }

         //spiceInstNames.clear();

         // Store spacecraft NAIF Id
         Integer satNAIFId = sat->GetIntegerParameter("NAIFId");
         std::string satNAIFIdString = GmatStringUtil::ToString(satNAIFId);

         // Create the frame definition file for the imagers
         std::string spiceFilePath = GmatFileUtil::GetTemporaryDirectory();
         std::string now = GmatTimeUtil::FormatCurrentTime(4);

         // First create the spacecraft frame
         SpiceFrameKernelWriter frameWriter = SpiceFrameKernelWriter();

         if (reportCoordinates == "SensorFrame")
         {
            if (GmatMathUtil::Abs(satNAIFId * 1000.0) >= 2147483647.0)
            {
               std::string errmsg = "Error in IntrusionLocator::Initialize(),"
                  " the NAIF ID on spacecraft " + sat->GetName() +
                  " is too large. The ID must satisfy the relation "
                  "(abs(NAIFID * 1000) < 2147483647) in order to "
                  "create the associated attitude and frame kernels with "
                  "proper IDs for IntrusionLocator ";
               errmsg += instanceName + "\n";
               throw EventException(errmsg);
            }

            std::string scIdString = GmatStringUtil::ToString(satNAIFId);
            std::string scFrameIdString = GmatStringUtil::ToString(satNAIFId * 1000);
            std::string scBodyFrameName = GmatStringUtil::ToUpper(sat->GetName());
            scFKFileName = spiceFilePath + sat->GetName() + now + ".tf";
            frameWriter.WriteCKBasedFrame(scFKFileName, scBodyFrameName,
               scFrameIdString, scIdString);
            frameWriter.LoadKernel(scFKFileName);
         }

         for (Integer sensIdx = 0; sensIdx < sensors.size(); ++sensIdx)
         {
            // To get a unique id, check which piece of hardware the current sensor is
            StringArray allHardwareNames = sat->GetRefObjectNameArray(Gmat::HARDWARE);
            Integer hardwareIdx = -1;
            for (Integer ii = 0; ii < allHardwareNames.size(); ++ii)
            {
               if (sensors.at(sensIdx)->GetName() == allHardwareNames.at(ii))
               {
                  hardwareIdx = ii;
                  break;
               }
            }

            if (hardwareIdx == -1)
            {
               std::string errmsg = "Error setting imagers on IntrusionLocator ";
               errmsg += instanceName + ", Imager " + sensors.at(sensIdx)->GetName() +
                  " not attached to spacecraft " + sat->GetName() + "\n";
               throw EventException(errmsg);
            }

            // Prepare an id for this hardware, recommended by SPICE documents
            // to use s/c ID * 1000 - instrument number
            Integer frameId = 0;
            if (satNAIFId < 0)
               frameId = satNAIFId * 1000 - hardwareIdx - 1;
            else
            {
               std::string errmsg = "Error in IntrusionLocator::Initialize, "
                  "the spacecraft NAIF ID for spacecraft \"" + sat->GetName() +
                  "\" must be negative in order to properly associate it with "
                  "the generated SCLK file for intrusion.\n";
               throw EventException(errmsg);
            }

            std::string frameIdStr = GmatStringUtil::ToString(frameId);
            std::string frameName = GmatStringUtil::ToUpper(sat->GetName()) +
               "_" + GmatStringUtil::ToUpper(sensors.at(sensIdx)->GetName());

            // Get the rotation matrix
            Rmatrix33 rotMat = ((Imager*)sensors.at(sensIdx))->GetRotationMatrix();
            // Needs to be transposed to be from sensor frame to SC frame for SPICE
            rotMat = rotMat.Transpose();
            // Create column of elements in string
            std::string rotMatColumn = "";
            for (Integer jj = 0; jj < 3; ++jj)
            {
               for (Integer ii = 0; ii < 3; ++ii)
               {
                  rotMatColumn += GmatStringUtil::ToString(rotMat(ii, jj), 1);
                  if (!(jj == 2 && ii == 2))
                     rotMatColumn += "\n";
               }
            }

            // Generate the sensor frame
            instFKFileNames.push_back(spiceFilePath + sensors.at(sensIdx)->GetName() + "ImagerFrame_" + now + ".tf");
            if (GmatFileUtil::DoesFileExist(instFKFileNames.back()))
               remove(instFKFileNames.back().c_str());
            if (reportCoordinates == "SensorFrame")
            {
               std::string scIdString = GmatStringUtil::ToString(satNAIFId);
               std::string scFrameIdString = GmatStringUtil::ToString(satNAIFId * 1000);
               std::string scBodyFrameName = GmatStringUtil::ToUpper(sat->GetName());
               // Now create the sensor's frame
               frameWriter.WriteFixedOffsetFrame(instFKFileNames.back(), frameName, frameIdStr,
                  scIdString, scBodyFrameName, rotMatColumn);
               frameWriter.LoadKernel(instFKFileNames.back());
            }
            else if (reportCoordinates == "FixedGrid")
            {
               // Load the provided file and get its class ID to generate the 
               // Imager frame
               spice->LoadKernel(gridFrameFile);
               std::ifstream gridFixedSpiceFrame;
               gridFixedSpiceFrame.open(gridFrameFile);
               std::string line, fixedGridClassID, centerID, centerPos,
                  gridCenterBodyID, gridFrameName, customFrameName;
               bool classIDFound = false, centerIDFound = false, centerPosFound = false;

               while (!gridFixedSpiceFrame.eof())
               {
                  std::getline(gridFixedSpiceFrame, line);
                  if (line.find("CLASS_ID") != std::string::npos)
                  {
                     fixedGridClassID = line.substr(line.find("=") + 1);
                     fixedGridClassID = GmatStringUtil::RemoveAllBlanks(fixedGridClassID);
                     classIDFound = true;
                  }
                  if (line.find("GRID_ORIGIN_FRAME") != std::string::npos)
                  {
                     gridFrameName = line.substr(line.find("=") + 1);
                     gridFrameName = GmatStringUtil::RemoveAllBlanks(gridFrameName);
                     gridFrameName = GmatStringUtil::RemoveAll(gridFrameName, "'");
                  }
                  if (line.find("GRID_ORIGIN_CENTER") != std::string::npos)
                  {
                     gridCenterBodyID = line.substr(line.find("=") + 1);
                     gridCenterBodyID = GmatStringUtil::RemoveAllBlanks(gridCenterBodyID);
                  }
                  if (line.find("GRID_ORIGIN_IDCODE") != std::string::npos)
                  {
                     centerID = line.substr(line.find("=") + 1);
                     centerID = GmatStringUtil::RemoveAllBlanks(centerID);
                     centerIDFound = true;
                  }
                  if (line.find("GRID_ORIGIN_XYZ") != std::string::npos)
                  {
                     centerPos = line.substr(line.find("=") + 1);
                     centerPos = GmatStringUtil::Trim(centerPos);
                     // Make GmatStringUtil friendly by switching out bracket type
                     centerPos = GmatStringUtil::RemoveOuterString(centerPos, "(", ")");
                     centerPos.insert(0, "[");
                     centerPos += "]";
                     centerPosFound = true;
                  }
                  if (line.find("FRAME_") != std::string::npos &&
                     line.find("_NAME") != std::string::npos)
                  {
                     customFrameName = line.substr(line.find("=") + 1);
                     customFrameName = GmatStringUtil::Trim(customFrameName);
                     customFrameName = GmatStringUtil::RemoveAll(customFrameName, "'");
                  }
               }

               gridFixedSpiceFrame.close();

               if (!classIDFound)
               {
                  std::string errmsg = "Error in IntrusionLocator::Initialize, "
                     "the provided fixed grid frame file " + gridFrameFile +
                     " does not contain a class ID.\n";
                  throw EventException(errmsg);
               }

               Integer classIDInt, centerIDInt, gridCenterBodyIDInt;
               GmatStringUtil::ToInteger(gridCenterBodyID, gridCenterBodyIDInt);
               GmatStringUtil::ToInteger(fixedGridClassID, classIDInt);
               GmatStringUtil::ToInteger(centerID, centerIDInt);
               RealArray sensorState = GmatStringUtil::ToRealArray(centerPos);
               WriteFixedGridSPK(sensorNames.at(sensIdx), gridFrameName,
                  gridCenterBodyIDInt, centerIDInt, sensorState);

               //if (sensors.at(sensIdx)->GetStringParameter("Coordinates") != "FixedGrid")
               //{
               //   std::string errmsg = "Error in IntrusionLocator::Initialize, "
               //      "the attached Imager named " + sensorNames.at(sensIdx) +
               //      " does not match the selected coordinate frame " + 
               //      reportCoordinates + " for intrusion locator " +
               //      instanceName + ".\n";
               //   throw EventException(errmsg);
               //}

               // Generate the frame file for the sensor
               frameWriter.WriteFixedOffsetFrame(instFKFileNames.back(), frameName, frameIdStr,
                  centerID, customFrameName, rotMatColumn);
               frameWriter.LoadKernel(instFKFileNames.back());
            }
            else
            {
               std::string errmsg = "Error in IntrusionLocator::Initialize, "
                  "unknown coordinate frame " + reportCoordinates + " being "
                  "used to generate a SPICE frame for imager.\n";
               throw EventException(errmsg);
            }

            // Get data for the FOV from GMAT
            std::string fovShape;
            FieldOfView *theFOV = ((Imager*)sensors.at(sensIdx))->GetFieldOfView();
            if (!theFOV)
            {
               std::string errmsg = "Error in IntrusionLocator::Initialize, "
                  "the imager " + sensors.at(sensIdx)->GetName() + " does "
                  "not have a field of view assigned.\n";
               throw EventException(errmsg);
            }
            if (theFOV->IsOfType(Gmat::CONICAL_FOV))
               fovShape = "CIRCLE";
            else if (theFOV->IsOfType(Gmat::RECTANGULAR_FOV))
               fovShape = "RECTANGLE";
            else if (theFOV->IsOfType(Gmat::CUSTOM_FOV))
               fovShape = "POLYGON";
            else
            {
               std::string errmsg = "Error setting imagers on IntrusionLocator ";
               errmsg += instanceName + ", Imager " +
                  sensors.at(sensIdx)->GetName() + " is using an unsupported "
                  "FOV shape. Allowable shapes are [Conical, Rectangular, Custom]";
               throw EventException(errmsg);
            }

            // Create the instrument definition file for the FOV's
            ikFileNames.push_back(spiceFilePath + sensors.at(sensIdx)->GetName() + "ImagerFOV_" + now + ".ti");
            if (GmatFileUtil::DoesFileExist(ikFileNames.back()))
               remove(ikFileNames.back().c_str());
            std::ofstream ikStream;
            ikStream.open(ikFileNames.back());

            ikStream << "\\begindata\n";
            if (fovShape == "POLYGON")
               ikStream << "INS" << frameIdStr << "_FOV_CLASS_SPEC = 'CORNERS'\n";
            else
               ikStream << "INS" << frameIdStr << "_FOV_CLASS_SPEC = 'ANGLES'\n";
            ikStream << "INS" << frameIdStr << "_FOV_SHAPE = '" << fovShape << "'\n";
            ikStream << "INS" << frameIdStr << "_FOV_FRAME = '" << frameName << "'\n";
            ikStream << "INS" << frameIdStr << "_BORESIGHT = (0.0 0.0 1.0)\n";
            if (fovShape == "CIRCLE")
            {
               ikStream << "INS" << frameIdStr << "_FOV_REF_VECTOR = (0.0 1.0 0.0)\n";
               ikStream << "INS" << frameIdStr << "_FOV_REF_ANGLE = (" <<
                  GmatStringUtil::ToString(theFOV->GetRealParameter("FieldOfViewAngle")) << ")\n";
               ikStream << "INS" << frameIdStr << "_FOV_ANGLE_UNITS = 'DEGREES'\n";
            }
            else if (fovShape == "RECTANGLE")
            {
               ikStream << "INS" << frameIdStr << "_FOV_REF_VECTOR = (0.0 1.0 0.0)\n";
               ikStream << "INS" << frameIdStr << "_FOV_REF_ANGLE = " <<
                  GmatStringUtil::ToString(theFOV->GetRealParameter("AngleHeight")) << "\n";
               ikStream << "INS" << frameIdStr << "_FOV_CROSS_ANGLE = " <<
                  GmatStringUtil::ToString(theFOV->GetRealParameter("AngleWidth")) << "\n";
               ikStream << "INS" << frameIdStr << "_FOV_ANGLE_UNITS = 'DEGREES'\n";
            }
            else if (fovShape == "POLYGON")
            {
               Rvector coneAngles = theFOV->GetRvectorParameter("ConeAngles");
               Rvector clockAngles = theFOV->GetRvectorParameter("ClockAngles");
               std::string lengthSpaces = "INS" + frameIdStr + "_FOV_BOUNDARY_CORNERS = ( ";
               lengthSpaces = std::string(" ", lengthSpaces.size());
               ikStream << "INS" << frameIdStr << "_FOV_BOUNDARY_CORNERS = ( ";
               for (Integer i = 0; i < coneAngles.GetSize(); i++)
               {
                  Real az = GmatMathUtil::DegToRad(-clockAngles.GetElement(i));
                  Real el = GmatMathUtil::DegToRad(coneAngles.GetElement(i));
                  Real x = GmatMathUtil::Sin(el) * GmatMathUtil::Cos(az); // Normally this would be X, but X is north in SPICE, so we call this y
                  Real y = GmatMathUtil::Sin(el) * GmatMathUtil::Sin(az); //
                  Real z = GmatMathUtil::Cos(el);
                  // Normally this would be X, but X is north in SPICE, so we go (-y,x,z)
                  if (i == 0)
                  {
                     ikStream << GmatStringUtil::RealToString(-y) << " " << GmatStringUtil::RealToString(x) << " " << GmatStringUtil::RealToString(z);
                  }
                  else
                  {
                     ikStream << "\n" << GmatStringUtil::RealToString(-y) << " " << GmatStringUtil::RealToString(x) << " " << GmatStringUtil::RealToString(z);
                  }
               }
               ikStream << ")\n";
            }
            ikStream << "NAIF_BODY_NAME += ('" << frameName << "')\n";
            ikStream << "NAIF_BODY_CODE += (" << frameIdStr << ")\n";

            ikStream.close();
            spice->LoadKernel(ikFileNames.back());
         }

         if (sensors.size() == 0)
         {
            MessageInterface::ShowMessage("*** WARNING *** IntrusionLocator " +
               instanceName + " does not have any imagers selected, no "
               "events will be detected.\n");
         }
      }

      retval = true;
   }

   #ifdef DEBUG_IntrusionLocator_INIT
      MessageInterface::ShowMessage("In IntrusionLocator::Initialize  about "
         "to set locatingString\n");
   #endif

   SetLocatingString("IntrusionLocator");

   return retval;
}

//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
* Writes the event data to file and optionally displays the event data plot.
*/
//------------------------------------------------------------------------------
bool IntrusionLocator::ReportEventData(const std::string &reportNotice)
{
   #ifdef DEBUG_CONTACT_LOCATOR_WRITE
      MessageInterface::ShowMessage("IntrusionLocator::ReportEventData ... \n");
   #endif

   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
   // TBD - do we want to throw an exception or just continue without writing?
      return false;
   }


   Integer     sz = (Integer)intrusionResults.size();
   std::string noEvents = GetNoEventsString("intrusion");

   #ifdef DEBUG_CONTACT_LOCATOR_WRITE
      MessageInterface::ShowMessage("attempting to write out %d events\n",
         (Integer)sz);
   #endif

   Integer numIndividual = 0;
   Integer numTotal = 0;

   if (sz == 0)
   {
      theReport << "\n" << noEvents << "\n";
   }

   // Loop over the total events list
   for (Integer ii = 0; ii < sz; ii++)
   {
      IntrusionResult* ev = intrusionResults.at(ii);
      ev->SetNoEvents(noEvents);

      std::string eventString = ev->GetReportString();
      theReport << eventString << "\n";


      numIndividual = ev->NumberOfIndividualEvents();
      numTotal = ev->NumberOfTotalEvents();
      theReport << "\nNumber of individual events : " << numIndividual;
      theReport << "\nNumber of total events      : " << numTotal << "\n\n\n";
   }

   theReport.close();
   return true;
}

//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void FindEvents()
//------------------------------------------------------------------------------
/**
* Find the eclipse events requested in the time range requested.
*
*/
//------------------------------------------------------------------------------
void IntrusionLocator::FindEvents()
{
   // Clear old events
   TakeAction("Clear", "Events");

   // Check if we need to load any target spacecraft SPKs
   for (Integer targetIdx = 0; targetIdx < intrudingBodyNames.size(); ++targetIdx)
   {
      if (intrudingBodies.at(targetIdx)->IsOfType(Gmat::SPACECRAFT))
         ((Spacecraft*)intrudingBodies.at(targetIdx))->ProvideEphemerisData();
   }

   // Loop through each of the imagers to run through intrusion detection
   for (Integer sensIdx = 0; sensIdx < sensors.size(); ++sensIdx)
   {
      std::string instName = GmatStringUtil::ToUpper(sat->GetName()) +
         "_" + GmatStringUtil::ToUpper(sensors.at(sensIdx)->GetName());

      std::string targetShape, targetFrame;
      Integer targetNaifId = 0;
      std::string abCorr = GetAbcorrString();
      StringArray occultBods;
      if (centralBodyName != "")
         occultBods.push_back(centralBodyName); // For now, just accepting the 
                                                // single input from user
      Integer numIntervals = 0;
      StringArray intervalTypes;
      RealArray starts, ends;

      // Set up report data for a single sensor
      IntrusionResult *singleSensorResult = new IntrusionResult();
      singleSensorResult->SetSpacecraftName(sat->GetName());
      singleSensorResult->SetSensorName(sensors.at(sensIdx)->GetName());
      singleSensorResult->SetCoordinateType(reportCoordinates);

      // Loop through intruding targets for this imager
      for (Integer targetIdx = 0; targetIdx < intrudingBodyNames.size(); ++targetIdx)
      {
         intervalTypes.clear();
         starts.clear();
         ends.clear();
         Real maxPhaseAngle = GmatMathConstants::PI * (1.0 - minimumPhase);

         if (intrudingBodies.at(targetIdx)->IsOfType(Gmat::SPACECRAFT))
         {
            targetShape = "POINT";
            // We leave this empty since it is just a point
            targetFrame = "";
         }
         else
         {
            targetShape = "ELLIPSOID";
            targetFrame = intrudingBodies.at(targetIdx)->GetStringParameter(
               intrudingBodies.at(targetIdx)->GetParameterID("SpiceFrameId"));
         }
         targetNaifId = intrudingBodies.at(targetIdx)->GetIntegerParameter(
            intrudingBodies.at(targetIdx)->GetParameterID("NAIFId"));

         em->GetIntrusionIntervals(instName, intrudingBodyNames.at(targetIdx),
            targetShape, targetFrame, targetNaifId, abCorr, occultBods, maxPhaseAngle, initialEp, finalEp,
            useEntireInterval, useLightTimeDelay, stepSize,
            numIntervals, intervalTypes, starts, ends);

         for (Integer intervalIdx = 0; intervalIdx < numIntervals; ++intervalIdx)
         {
            // For the intrusion report, we also want intermediate data within intervals
            Real intermediateTime = starts.at(intervalIdx);
            RealArray epochArray, angDiameters, coordAng1, coordAng2, phases;
            bool isOcculting = false;
            if (intervalTypes.at(intervalIdx) == "Occultation")
               isOcculting = true;

            while (intermediateTime <= ends.at(intervalIdx))
            {
               epochArray.push_back(intermediateTime);

               // First get the position of the target w.r.t. the sensor
               std::string frameName = GmatStringUtil::ToUpper(sat->GetName()) +
                  "_" + GmatStringUtil::ToUpper(sensors.at(sensIdx)->GetName());
               Rvector3 targPos;
               Real lightTime = 0.0;

               em->GetTargetPosition(intermediateTime, targetNaifId, frameName,
                  abCorr, targPos, lightTime);

               // Determine the angular diameter of the target
               Real angDiam = 0;
               if (intrudingBodies.at(targetIdx)->IsOfType(Gmat::CELESTIAL_BODY))
               {
                  Real dist = GmatMathUtil::Sqrt(targPos(0) * targPos(0) +
                     targPos(1) * targPos(1) + targPos(2) * targPos(2));
                  Real targDiam = 2.0 *
                     ((CelestialBody*)intrudingBodies.at(targetIdx))->
                     GetEquatorialRadius();
                  angDiam = 2.0 *
                     GmatMathUtil::ASin(targDiam / (2.0 * dist)) *
                     180.0 / GmatMathConstants::PI;
               }

               angDiameters.push_back(angDiam);

               // Determine the angular coordinates of the center of the target
               Real xAngle, yAngle;

               if (reportCoordinates == "FixedGrid")
               {
                  xAngle = GmatMathUtil::Abs(GmatMathUtil::ATan(targPos(0) / targPos(2)) *
                     180.0 / GmatMathConstants::PI);
                  if (targPos(0) >= 0.0)
                     xAngle = 90.0 - xAngle;
                  else
                     xAngle = 90.0 + xAngle;
                  yAngle = GmatMathUtil::ATan(targPos(1) / targPos(2)) *
                     180.0 / GmatMathConstants::PI;
               }
               else
               {
                  xAngle = GmatMathUtil::ATan(targPos(0) / targPos(2)) *
                     180.0 / GmatMathConstants::PI;
                  yAngle = GmatMathUtil::ATan(targPos(1) / targPos(2)) *
                     180.0 / GmatMathConstants::PI;
               }

               coordAng1.push_back(xAngle);
               coordAng2.push_back(yAngle);

               // Now get the phase angle to determine the percent illumination
               Real phasePercent;
               if (intrudingBodyNames.at(targetIdx) != "Sun")
               {
                  CelestialBody* illmnBody = solarSys->GetBody("Sun");
                  Integer illmnNaifId = illmnBody->GetIntegerParameter(illmnBody->GetParameterID("NAIFId"));
                  Real phaseAngle = em->GetPhaseAngle(intermediateTime,
                     targetNaifId, illmnNaifId, abCorr);
                  phasePercent = 1.0 - phaseAngle / GmatMathConstants::PI;
               }
               else
                  phasePercent = 1.0;

               phases.push_back(phasePercent);

               if (isOcculting)
               {
                  if (intermediateTime == ends.at(intervalIdx))
                     intermediateTime += stepSize / 86400.0; // Used to end the loop
                  else
                     intermediateTime = ends.at(intervalIdx);
               }
               else if ((intermediateTime + (stepSize / 86400.0)) <=
                  ends.at(intervalIdx))
                  intermediateTime += stepSize / 86400.0;
               else if (intermediateTime == ends.at(intervalIdx))
                  intermediateTime += stepSize / 86400.0; // Used to end the loop
               else
                  intermediateTime = ends.at(intervalIdx);
            }

            // Store the data
            std::string intervalType = intervalTypes.at(intervalIdx);
            Real s1 = starts.at(intervalIdx);
            Real e1 = ends.at(intervalIdx);
            IntrusionEvent *newEvent = new IntrusionEvent(intervalType,
               s1, e1, epochArray, intrudingBodyNames.at(targetIdx),
               angDiameters, coordAng1,coordAng2, phases);
            singleSensorResult->AddEvent(newEvent);
         }
      }
      intrusionResults.push_back(singleSensorResult);
   }
}

//------------------------------------------------------------------------------
// void WriteFixedGridSPK(std::string sensorName, Integer gridId,
//                        RealArray gridOrigin)
//------------------------------------------------------------------------------
/**
* Generate an spk file for the fixed grid frame being used in intrusion
* detection/reporting. This is required for CSpice functionality. This is done 
* by providing coordinate frame details and origin location at a start and end
* epoch. These epochs are set to an extremely wide timespan to cover any time
* the mission could potentially utilize.
*
* @param sensorName The name of the sensor that is having its SPK generated
* @param gridId The SPICE ID of the fixed grid frame being used for the sensor
* @param gridPosition The position of the fixed grid's origin provided by its frame 
*        kernel
*
*/
//------------------------------------------------------------------------------
void IntrusionLocator::WriteFixedGridSPK(const std::string &sensorName,
   const std::string &frameName, const Integer centerID, const Integer gridId,
   const RealArray gridPosition)
{
   // Get the time (seconds since January 1, 1970), to make the temporary file name unique
   std::string now = GmatTimeUtil::FormatCurrentTime(4);
   std::string kernelBaseName = GmatFileUtil::GetTemporaryDirectory() + "tmp_" + sensorName;
   std::string spkName = kernelBaseName + "_" + now + ".bsp";

   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("In WriteSPK, spkName = \"%s\"\n", spkName.c_str());
   #endif

   // We are not renaming here - just remove the existing file
   if (GmatFileUtil::DoesFileExist(spkName))
      remove(spkName.c_str());

   if (!spice)
      spice = new SpiceInterface();

   SpiceInt        maxChar = 4000; // need static const for this?
   std::string     internalFileName = "GMAT-generated SPK file for " + instanceName;
   ConstSpiceChar  *internalSPKName = internalFileName.c_str();
   ConstSpiceChar  *spkNameSPICE = spkName.c_str();
   SpiceInt        handle;

   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("In WriteSPK, about to open SPK ...\n");
   #endif
   // CSPICE method to create and open an SPK kernel
   spkopn_c(spkNameSPICE, internalSPKName, maxChar, &handle);
   if (failed_c()) // CSPICE method to detect failure of previous call to CSPICE
   {
      ConstSpiceChar option[] = "LONG"; // retrieve long error message
      SpiceInt       numErrChar = MAX_LONG_MESSAGE_VALUE;
      //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numErrChar, err);
      std::string errStr(err);
      std::string errmsg = "Error getting file handle for GroundStation SPK file \"";
      errmsg += spkName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      delete[] err;
      throw EventException(errmsg);
   }

   // Write the data to the Fixed Grid SPK
   SpiceInt       spiceId = gridId;
   SpiceInt       spiceCentral = centerID;
   ConstSpiceChar *bFrame = frameName.c_str();

   SpiceDouble    theMax = GmatRealConstants::REAL_MAX - 10.0;
   SpiceDouble    first = -theMax / 2.0;
   SpiceDouble    last = theMax / 2.0;
   SpiceDouble    epoch1 = first;
   SpiceDouble    step = last - first;
   std::string    segmentId = "Segment 1 for Asset " + instanceName;
   ConstSpiceChar *segId = segmentId.c_str();
   // put state into a SpiceDouble array

   if (gridPosition.size() != 3)
   {
      std::string errmsg = "Error in IntrusionLocator::WriteFixedGridSPK, "
         "the provided fixed grid origin position array must contain 3 "
         "elements.\n";
      throw EventException(errmsg);
   }

   SpiceDouble  *stateArray;
   stateArray = new SpiceDouble[6 * 2];
   stateArray[0] = gridPosition.at(0);
   stateArray[1] = gridPosition.at(1);
   stateArray[2] = gridPosition.at(2);
   stateArray[3] = 0.0;
   stateArray[4] = 0.0;
   stateArray[5] = 0.0;
   stateArray[6] = gridPosition.at(0);
   stateArray[7] = gridPosition.at(1);
   stateArray[8] = gridPosition.at(2);
   stateArray[9] = 0.0;
   stateArray[10] = 0.0;
   stateArray[11] = 0.0;

   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("In WriteSPK, about to write SPK ...\n");
      MessageInterface::ShowMessage("spiceId = %d, spiceCentral = %d\n",
         naifId, bodyNaif);
   #endif

   SpiceInt originFrameID;
   namfrm_c(bFrame, &originFrameID);

   /// @TODO: This currently only allows Earth centered reference frames for
   /// setting up a fixed grid. Should be made generic when other frames can
   /// be properly tested
   SpiceInt frameCenter, frameClass, frameClassID;
   SpiceBoolean frameFound;
   frinfo_c(originFrameID, &frameCenter, &frameClass, &frameClassID, &frameFound);
   if (frameCenter != 399 || centerID != 399)
   {
      std::string errMsg = "Error in fixed grid file of IntrusionLocator \"" +
         instanceName + "\", only fixed grids originating from an Earth "
         "centered reference frame are accepted.";
      throw EventException(errMsg);
   }

   spkw08_c(handle, spiceId, spiceCentral, bFrame, first, last, segId, 1, 2,
      stateArray, epoch1, step);

   spkcls_c(handle);

   spice->LoadKernel(spkName);

   delete[] stateArray;
}
