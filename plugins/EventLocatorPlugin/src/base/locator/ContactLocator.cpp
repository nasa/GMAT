//$Id: ContactLocator.cpp 1979 2012-01-07 00:34:06Z  $
//------------------------------------------------------------------------------
//                           ContactLocator
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 7, 2011
// Updated: 2015  Wendy Shoan / GSFC and Yeerang Lim/KAIST
//
/**
 * Definition of the ContactLocator
 * Updates based on prototype by Joel Parker / GSFC
 */
//------------------------------------------------------------------------------


#include "ContactLocator.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"
#include "GroundstationInterface.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "EventException.hpp"
#include "EphemManager.hpp"
#include "StringUtil.hpp"
#include "ContactEvent.hpp"
#include "FileUtil.hpp"
#include "Imager.hpp"
#include "SpiceInterface.hpp"
#include "Moderator.hpp"
#include "PlanetographicRegion.hpp"
#include <iomanip>

//#define DEBUG_SET
//#define DEBUG_SETREF
//#define DEBUG_CONTACT_LOCATOR_WRITE
//#define DEBUG_CONTACT_EVENTS
//#define DEBUG_INIT_FINALIZE
//#define DEBUG_CONTACTLOCATOR_INIT

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string ContactLocator::PARAMETER_TEXT[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
   "Observers",               // STATIONS
   "LightTimeDirection",
   "LeftJustified",
   "ReportPrecision",
   "ReportFormat",
   "IntervalStepSize",
   "ReportTimeFormat",
};

const Gmat::ParameterType ContactLocator::PARAMETER_TYPE[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,    // STATIONS
   Gmat::ENUMERATION_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRING_TYPE, // REPORT_TEMPLATE_FORMAT
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE, // REPORT_TIME_FORMAT
};

const std::string ContactLocator::LT_DIRECTIONS[2] =
{
   "Transmit",
   "Receive",
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ContactLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ContactLocator::ContactLocator(const std::string &name) :
   EventLocator         ("ContactLocator", name),
   spice(NULL),
   lightTimeDirection   ("Transmit")
{
   objectTypeNames.push_back("ContactLocator");
   parameterCount = ContactLocatorParamCount;

   observerNames.clear();
   observers.clear();
   contactResults.clear();

   // Override default stepSize for the ContactLocator
   stepSize = 10;

   // Set default occulting bodies  2015.09.21 removing Luna as a default (GMT-5070)
//   defaultOccultingBodies.push_back("Luna");

   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("CREATED new ContactLocator %s<%p>\n",
            instanceName.c_str(), this);
   #endif
}

//------------------------------------------------------------------------------
// ~ContactLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ContactLocator::~ContactLocator()
{
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("DESTRUCTING ContactLocator %s<%p>\n",
            instanceName.c_str(), this);
   #endif
   TakeAction("Clear");
   // Remove the temp files
   if (spice)
   {
      for (Integer i = 0; i < fkFileName.size(); i++)
      {
         spice->UnloadKernel(fkFileName[i]);
         remove(fkFileName[i].c_str());
      }
      for (Integer i = 0; i < ikFileName.size(); i++)
      {
         spice->UnloadKernel(ikFileName[i]);
         remove(ikFileName[i].c_str());
      }
   }
}

//------------------------------------------------------------------------------
// ContactLocator(const ContactLocator &cl)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param cl The original being copied
 */
//------------------------------------------------------------------------------
ContactLocator::ContactLocator(const ContactLocator &cl) :
   EventLocator            (cl),
   observerNames           (cl.observerNames),
   lightTimeDirection      (cl.lightTimeDirection),
   leftJustified           (cl.leftJustified),
   reportPrecision         (cl.reportPrecision),
   reportColumnsInOrder    (cl.reportColumnsInOrder),
   reportIntervals         (cl.reportIntervals),
   intervalStep            (cl.intervalStep),
   reportTimeFormat        (cl.reportTimeFormat),
   reportTemplateFormat    (cl.reportTemplateFormat),
   spice(NULL)
{
   // Observers
   observerNames.clear();
   observers.clear();
   for (Integer jj = 0; jj < cl.observerNames.size(); jj++)
      observerNames.push_back(cl.observerNames.at(jj));
   for (Integer jj = 0; jj < cl.observers.size(); jj++)
      observers.push_back(cl.observers.at(jj));
   // ContactResults
   TakeAction("Clear", "Events");
   ContactResult *toCopy   = NULL;
   ContactResult *newEvent = NULL;
   for (Integer ii = 0; ii < cl.contactResults.size(); ii++)
   {
      toCopy   = cl.contactResults.at(ii);
      newEvent = new ContactResult(*toCopy);
      contactResults.push_back(newEvent);
   }
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage(
            "CREATED new ContactLocator %s<%p> by copying %s<%p>\n",
            instanceName.c_str(), this, cl.instanceName.c_str(), &cl);
   #endif
}


//------------------------------------------------------------------------------
// ContactLocator& operator=(const ContactLocator &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param c The ContactLocator providing parameters for this one
 *
 * @return This ContactLocator
 */
//------------------------------------------------------------------------------
ContactLocator& ContactLocator::operator=(const ContactLocator &c)
{
   if (this != &c)
   {
      EventLocator::operator=(c);

//      observerNames       = c.observerNames;
      lightTimeDirection = c.lightTimeDirection;
     reportPrecision = c.reportPrecision;
     reportColumnsInOrder = c.reportColumnsInOrder;
     leftJustified = c.leftJustified;
     reportIntervals = c.reportIntervals;
     intervalStep = c.intervalStep;
     reportTimeFormat = c.reportTimeFormat;
     reportTemplateFormat = c.reportTemplateFormat;

      // Observers
      observerNames.clear();
      observers.clear();
      for (Integer jj = 0; jj < c.observerNames.size(); jj++)
         observerNames.push_back(c.observerNames.at(jj));
      for (Integer jj = 0; jj < c.observers.size(); jj++)
         observers.push_back(c.observers.at(jj));
      // ContactResults
      TakeAction("Clear", "Events");
      ContactResult *toCopy   = NULL;
      ContactResult *newEvent = NULL;
      for (Integer ii = 0; ii < c.contactResults.size(); ii++)
      {
         toCopy   = c.contactResults.at(ii);
         newEvent = new ContactResult(*toCopy);
         contactResults.push_back(newEvent);
      }
   }
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage(
            "COPIED to ContactLocator %s<%p> from %s<%p>\n",
            instanceName.c_str(), this, c.instanceName.c_str(), &c);
   #endif

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
GmatBase *ContactLocator::Clone() const
{
   return new ContactLocator(*this);
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
void ContactLocator::Copy(const GmatBase* orig)
{
   operator=(*((ContactLocator *)(orig)));
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
std::string ContactLocator::GetParameterText(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < ContactLocatorParamCount)
      return PARAMETER_TEXT[id - EventLocatorParamCount];

   if (id == SATNAME)  return "Target";

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
Integer ContactLocator::GetParameterID(const std::string & str) const
{
   for (Integer i = EventLocatorParamCount; i < ContactLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EventLocatorParamCount])
         return i;
   }
   if (str == "Target")
      return SATNAME;
   else if (str == "Spacecraft")
   {
      std::string errmsg = "\"Spacecraft\" not a valid field for a Contact Locator.  ";
      errmsg += "Please use \"Target\".\n";
      throw EventException(errmsg);
   }
   else
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
Gmat::ParameterType ContactLocator::GetParameterType(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < ContactLocatorParamCount)
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
std::string ContactLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Integer ContactLocator::GetIntegerParameter(const Integer id) const
{
   if (id == REPORT_PRECISION)
      return reportPrecision;

   return EventLocator::GetIntegerParameter(id);
}

//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
 //------------------------------------------------------------------------------
Integer ContactLocator::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == REPORT_PRECISION)
   {
      if (value < 0)
      {
         std::string errmsg = "*** Error *** The value" +
            GmatStringUtil::ToString(value) + "for field ReportPrecision on "
            "object \"" + instanceName + "\" is not an allowed value. "
            "Allowed values are [0 < Integer]";
         throw EventException(errmsg);
      }

      reportPrecision = value;
      return reportPrecision;
   }

   return EventLocator::SetIntegerParameter(id, value);
}

//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Integer ContactLocator::GetIntegerParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetIntegerParameter(id);
}

//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
 //------------------------------------------------------------------------------
Integer ContactLocator::SetIntegerParameter(const std::string &label,
   const Integer value)
{
   Integer id = GetParameterID(label);
   return SetIntegerParameter(id, value);
}


//---------------------------------------------------------------------------
//  Integer GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real ContactLocator::GetRealParameter(const Integer id) const
{
   if (id == REPORT_PRECISION)
      return reportPrecision;
   if (id == INTERVAL_STEP)
      return intervalStep;

   return EventLocator::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Integer SetRealParameter(const Integer id, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
 //------------------------------------------------------------------------------
Real ContactLocator::SetRealParameter(const Integer id, const Real value)
{
   if (id == INTERVAL_STEP)
   {
      if (value < 0)
      {
         std::string errmsg = "*** Error *** The value" +
            GmatStringUtil::ToString(value) + "for field intervalStep on "
            "object \"" + instanceName + "\" is not an allowed value. "
            "Allowed values are greater than 0.";
         throw EventException(errmsg);
      }

      intervalStep = value;
      if (value != 0)
      {
         reportIntervals = true;
      }
      return intervalStep;
   }

   return EventLocator::SetRealParameter(id, value);
}

//---------------------------------------------------------------------------
//  Integer SetRealParameter(const Integer id, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
 //------------------------------------------------------------------------------
Real ContactLocator::SetRealParameter(const Integer id, const Integer value)
{
   if (id == INTERVAL_STEP)
   {
      if (value < 0)
      {
         std::string errmsg = "*** Error *** The value" +
            GmatStringUtil::ToString(value) + "for field intervalStep on "
            "object \"" + instanceName + "\" is not an allowed value. "
            "Allowed values are greater than 0.";
         throw EventException(errmsg);
      }

      intervalStep = value;
      if (value != 0)
      {
         reportIntervals = true;
      }
      return intervalStep;
   }

   return EventLocator::SetRealParameter(id, value);
}

//---------------------------------------------------------------------------
//  Integer GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real ContactLocator::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Integer SetRealParameter(const std::string &label, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
 //------------------------------------------------------------------------------
Real ContactLocator::SetRealParameter(const std::string &label,
   const Integer value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
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
std::string ContactLocator::GetStringParameter(const Integer id) const
{
   if (id == LIGHT_TIME_DIRECTION)
      return lightTimeDirection;
   if (id == REPORT_TIME_FORMAT)
      return reportTimeFormat;
   if (id == REPORT_TEMPLATE_FORMAT)
      return reportTemplateFormat;

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
bool ContactLocator::SetStringParameter(const Integer id,
      const std::string &value)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("In SetStringParameter with id = %d, value = %s\n",
            id, value.c_str());
   #endif
   if (id == LIGHT_TIME_DIRECTION)
   {
      std::string lightTimeDirectionList = "Transmit, Receive";
      if ((value == "Transmit") || (value == "Receive"))
      {
         lightTimeDirection = value;
         return true;
      }
      else
      {
         std::string errmsg =
            "The value of \"" + value + "\" for field \"LightTimeDirection\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + lightTimeDirectionList + " ]. ";
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("ERROR message is: \"%s\"\n", errmsg.c_str()); // *******
         #endif
         EventException ee;
         ee.SetDetails(errmsg);
         throw ee;
      }
   }
   if (id == STATIONS)
   {
      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("--- Attempting to add a GS to list of ground observers ...\n");
      #endif
      if (find(observerNames.begin(), observerNames.end(), value) ==
            observerNames.end())
      {
         observerNames.push_back(value);
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("--- Just added \"%s\" to list of ground observers ...\n",
                  value.c_str());
         #endif
      }
      return true;
   }
   if (id == REPORT_TIME_FORMAT)
   {
      if (value == "UTCGregorian" || value == "UTCMJD" || value == "ISOYD")
      {
         reportTimeFormat = value;
         return true;
      }
      else
         throw EventException("ReportTimeFormat must be set to UTC, UTCMJD, or ISOYD");
   }
   if (id == REPORT_TEMPLATE_FORMAT)
   {
      if (value == "AzimuthElevationRangeReport" || value == "AzimuthElevationRangeRangeRateReport" || value == "ContactRangeReport" || value == "SiteViewMaxElevationRangeReport" || value == "SiteViewMaxElevationReport" || value == "Legacy")
      {
         reportTemplateFormat = value;
         return true;
      }
      else
         throw EventException("ReportFormat options must be set to one of the following: AzimuthElevationRangeReport, AzimuthElevationRangeRangeRateReport, ContactRangeReport,SiteViewMaxElevationReport, SiteViewMaxElevationRangeReport, or Legacy");
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
std::string ContactLocator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == STATIONS)
    {
       if (index < (Integer)observerNames.size())
          return observerNames[index];
       else
          throw EventException(
                "Index out of range when trying to access observer list for " +
                instanceName);
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
bool ContactLocator::SetStringParameter(const Integer id,
      const std::string & value, const Integer index)
{
   if (id == STATIONS)
   {
      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("--- Attempting to add a GS (index = %d) to list of ground observers ...\n", index);
      #endif
      if (find(observerNames.begin(), observerNames.end(), value) == observerNames.end())
      {
         if (index < (Integer)observerNames.size())
         {
            observerNames[index] = value;
            return true;
         }
         else
         {
            observerNames.push_back(value);
            return true;
         }
      }
      else
      {
         // ignore duplicate observer names, for now
         return true;
//         std::string errmsg = "Observer ";
//         errmsg += value + " is already in list for ContactLocator ";
//         errmsg += instanceName + ".  Each observer must be listed only once.\n";
//         throw EventException(errmsg);
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
const StringArray & ContactLocator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == STATIONS)
      return observerNames;

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
const StringArray& ContactLocator::GetStringArrayParameter(const Integer id,
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
std::string ContactLocator::GetStringParameter(const std::string & label) const
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
bool ContactLocator::SetStringParameter(const std::string & label,
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
std::string ContactLocator::GetStringParameter(const std::string &label,
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
bool ContactLocator::SetStringParameter(const std::string & label,
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
const StringArray & ContactLocator::GetStringArrayParameter(
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
const StringArray & ContactLocator::GetStringArrayParameter(
      const std::string & label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
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
bool ContactLocator::GetBooleanParameter(const Integer id) const
{
   if (id == LEFT_JUSTIFIED)
      return leftJustified;

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
bool ContactLocator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == LEFT_JUSTIFIED)
   {
      leftJustified = value;
      return leftJustified;
   }

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
bool ContactLocator::GetBooleanParameter(const std::string &label) const
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
bool ContactLocator::SetBooleanParameter(const std::string &label,
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
UnsignedInt ContactLocator::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case STATIONS:
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
const StringArray& ContactLocator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case LIGHT_TIME_DIRECTION:
      enumStrings.clear();
      for (Integer ii = 0; ii < 2; ii++)
         enumStrings.push_back(LT_DIRECTIONS[ii]);

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
const ObjectTypeArray& ContactLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();  // ??
   if (id == STATIONS)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::GROUND_STATION) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::GROUND_STATION);
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
const ObjectTypeArray& ContactLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to a reference object contained in a vector of
 * objects in the BodyFixedPoint class.
 *
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* ContactLocator::GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index)
{
   switch (type)
   {
      case Gmat::GROUND_STATION:
         for (UnsignedInt ii = 0; ii < observerNames.size(); ii++)
         {
            if (name == observerNames.at(ii))
            {
               return observers.at(ii);
            }
         }
         break;
      case Gmat::SPACECRAFT:
         if (targetName != name)
         {
            for (UnsignedInt ii = 0; ii < observerNames.size(); ii++)
            {
               if (name == observerNames.at(ii))
               {
                  return observers.at(ii);
               }
            }
         }
         break;

      default:
         break;
   }

   return EventLocator::GetRefObject(type, name, index);
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
bool ContactLocator::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   #ifdef DEBUG_SETREF
      MessageInterface::ShowMessage("CL::SetRefObject, obj = %s, type = %d (%s), name= %s\n",
            (obj? "NOT NULL" : "NULL"), (Integer) type, OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT].c_str(),
            name.c_str());
      MessageInterface::ShowMessage("observer names are:\n");
      for (Integer ii = 0; ii < observerNames.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", observerNames.at(ii).c_str());
   #endif
   switch (type)
   {
      case Gmat::GROUND_STATION:
         for (UnsignedInt ii = 0; ii < observerNames.size(); ii++)
         {
            #ifdef DEBUG_SETREF
               MessageInterface::ShowMessage(
                     "Is of type GROUND_STATION, checking name %s ...\n",
                     observerNames.at(ii).c_str());
            #endif
            if (name == observerNames.at(ii))
//            if (obj->GetName() == observerNames.at(ii))
            {
               #ifdef DEBUG_SETREF
                  MessageInterface::ShowMessage(
                        "it matched!!! so setting it ...\n");
               #endif
               observers.push_back(obj);
//               observers.at(ii) = (GroundstationInterface*) obj;
               return true;
            }
         }
         break;
      case Gmat::SPACECRAFT:
         if (targetName != name)
         {
            if (targetIsRegion)
            {
               if (!sat)
               {
                  sat = (Spacecraft*)obj;
               }
               observers.push_back(obj);
               
               return true;
            }
            for (UnsignedInt ii = 0; ii < observerNames.size(); ii++)
            {
                  #ifdef DEBUG_SETREF
               MessageInterface::ShowMessage(
                  "Is of type GROUND_STATION, checking name %s ...\n",
                  observerNames.at(ii).c_str());
                  #endif
               if (name == observerNames.at(ii))
                  //            if (obj->GetName() == observerNames.at(ii))
               {
                  #ifdef DEBUG_SETREF
                  MessageInterface::ShowMessage(
                     "it matched!!! so setting it ...\n");
                  #endif
                  observers.push_back(obj);
                  //               observers.at(ii) = (GroundstationInterface*) obj;
                  return true;
               }
            }
         }
         break;
      default:
         break;
   }

   #ifdef DEBUG_SETREF
      MessageInterface::ShowMessage(
            "--- DIDN'T match anthing!!! so calling parent ...\n");
   #endif
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
bool ContactLocator::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
const StringArray& ContactLocator::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_BF_REF
      MessageInterface::ShowMessage("In BFP::GetRefObjectNameArray, requesting type %d (%s)\n",
            (Integer) type, (GmatBase::OBJECT_TYPE_STRING[type]).c_str());
   #endif

   refObjectNames = EventLocator::GetRefObjectNameArray(type);

  if ((type == Gmat::GROUND_STATION) || (type == Gmat::SPACE_POINT) || (type == Gmat::UNKNOWN_OBJECT))
  {
     refObjectNames.insert(refObjectNames.begin(), observerNames.begin(),
           observerNames.end());
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
const ObjectTypeArray& ContactLocator::GetRefObjectTypeArray()
{
   refObjectTypes = EventLocator::GetRefObjectTypeArray();
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
bool ContactLocator::RenameRefObject(const UnsignedInt type,
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
      case Gmat::GROUND_STATION:
      case Gmat::UNKNOWN_OBJECT:
         for (UnsignedInt i = 0; i < observerNames.size(); ++i)
         {
            if (observerNames[i] == oldName)
            {
               observerNames[i] = newName;
               retval = true;
            }
         }
         break;
      default:
         ;        // Intentional drop-through
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
bool ContactLocator::TakeAction(const std::string &action,
                                const std::string &actionData)
{
   #ifdef DEBUG_ECLIPSE_ACTION
      MessageInterface::ShowMessage(
            "Entering EclipseLocator::TakeAction (%s<%p>) with action = %s and actionData = %s\n",
            instanceName.c_str(), this, action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "Observers") || (actionData == ""))
      {
         observerNames.clear();
         observers.clear();
         retval = true;
      }
      else if ((actionData == "Events") || (actionData == ""))
      {
         #ifdef DEBUG_ECLIPSE_ACTION
            MessageInterface::ShowMessage(
                  "In ContactLocator::TakeAction, about to clear %d events\n",
                  (Integer) contactResults.size());
         #endif
         for (Integer ii = 0; ii < contactResults.size(); ii++)
         {
            contactResults.at(ii)->TakeAction("Clear", "Events");
            delete contactResults.at(ii);
         }
         contactResults.clear();
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
bool ContactLocator::Initialize()
{
   bool retval = false;
   #ifdef DEBUG_CONTACTLOCATOR_INIT
      MessageInterface::ShowMessage("In CL::Init for %s\n", instanceName.c_str());
   #endif

   Moderator *theModerator = Moderator::Instance();
   if (EventLocator::Initialize())
   {

      if (targetIsRegion && observers.size() > 1)
      {
         std::string errmsg = "Error setting observers on ContactLocator ";
         errmsg += instanceName + ".\n If the target is a PlanetographicRegion, only one observer is allowed, and it must be a spacecraft.";
         throw EventException(errmsg);
      }
      if (observerNames.size() != observers.size())
      {
         std::string errmsg = "Error setting observers on ContactLocator ";
         errmsg += instanceName + ". All observers must be either ground stations or spacecraft.\n";
         throw EventException(errmsg);
      }
      for (UnsignedInt ii= 0; ii < observers.size(); ii++)
      {
         if (observers.at(ii) == NULL)
         {
            std::string errmsg = "Error setting observers on ContactLocator ";
            errmsg += instanceName + "\n";
            throw EventException(errmsg);
         }
         if (!observers[ii]->IsOfType(Gmat::GROUND_STATION) && !observers[ii]->IsOfType(Gmat::SPACECRAFT))
         {
            throw EventException("The observer " + observers[ii]->GetName() + " for ContactLocator " + instanceName + 
               " must be a ground station or spacecraft for Contact Location");
         }
         // Add Imagers with FOV to directObserversList, else add a default ground stations
         bool hasFovAttached = false;
         for (UnsignedInt j = 0; j < observers[ii]->GetRefObjectArray(Gmat::HARDWARE).size(); j++)
         {
            Hardware* hardwareOnBoard = ((Hardware*)(observers[ii]->GetRefObjectArray(Gmat::HARDWARE)[j]));
            if (hardwareOnBoard->HasFOV())
            {
               /* Developer note: The following section is required due to errors in the initialization order of Hardware in relation 
               to Groundstations and Spacecraft. Currently, Hardware is initialized after both, so the Imager object 
               is cloned onto the GS and SC without having been initialized, meaning the FOV is not cloned onto the
               Imager and is seen as NULL. Calling the moderator for the FOV is a workaround used presently until the initialization issue
               is resolved, but will cause issues down the road when FOV or imagers are accessed during mission sequence. 
               Once fixed, just the lines in the else statment will be necessary
               
               */

               // Check if the central body is among those covered by the test system
               std::string centralBodyName = observers[ii]->GetStringParameter(observers[ii]->GetParameterID("CentralBody"));
               std::string centralBodyType = ((BodyFixedPoint *)observers[ii])->GetCentralBody()->GetTypeName();
               bool testedCB = ((centralBodyName == "Luna") || (centralBodyType == "Planet"));
               if (!testedCB)
               {
                  //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING && (centralBodyType == "Moon"))        // made changes by TUAN NGUYEN
                  if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING && (centralBodyType == "Moon"))   // made changes by TUAN NGUYEN
                  {
                     MessageInterface::ShowMessage("Testing Mode WARNING: Use of moons that are not Luna for central bodies by the ContactLocator has not been officially tested with FOV.\n");
                  }
                  else
                  {
                     throw EventException("Usage of FOV is only permitted for observers with central bodies that are \"Luna\" or a planet.");
                  }
               }
               if (((Imager *)hardwareOnBoard)->GetFieldOfView() == NULL)
               {
                  std::string fovName = hardwareOnBoard->GetRefObjectName(Gmat::FIELD_OF_VIEW);
                  GmatBase *fovObj = theModerator->GetConfiguredObject(fovName);
                  ((Imager *)hardwareOnBoard)->SetRefObject(fovObj, Gmat::FIELD_OF_VIEW, fovName);
                  directObservers.push_back(hardwareOnBoard);
                  directObserversNames.push_back(hardwareOnBoard->GetName());
                  directObserversHostIndex.push_back(ii);
                  hasFovAttached = true;
               }
               else
               {
                  directObservers.push_back(hardwareOnBoard);
                  directObserversNames.push_back(hardwareOnBoard->GetName());
                  directObserversHostIndex.push_back(ii);
                  hasFovAttached = true;
               }
            }
         }
         
         if (!hasFovAttached)
         {
            // Check if the central body is among those covered by the test system
            if (!targetIsRegion)
            {
               std::string centralBodyName = observers[ii]->GetStringParameter(observers[ii]->GetParameterID("CentralBody"));
               std::string centralBodyType = ((BodyFixedPoint *)observers[ii])->GetCentralBody()->GetTypeName();
               bool testedCB = ((centralBodyName == "Luna") || (centralBodyType == "Planet"));
               if (reportTemplateFormat != "Legacy" && !testedCB)
               {

                  //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING && (centralBodyType == "Moon"))        // made changes by TUAN NGUYEN
                  if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING && (centralBodyType == "Moon"))   // made changes by TUAN NGUYEN
                  {
                     MessageInterface::ShowMessage("Testing Mode WARNING: Use of moons that are not Luna for central bodies by the ContactLocator has not been officially tested with Non-Legacy Report formats.\n");
                  }
                  else
                  {
                     throw EventException("Usage of Non-Legacy Report formats is only permitted for observers with central bodies on \"Earth\", \"Luna\", and \"Mars.\"");
                  }
               }
               else if (centralBodyType != "Planet" && centralBodyType != "Moon")
               {
                  throw EventException("The central bodies of Ground stations used in contact location must be a Planet or a Moon. Asteroids and Comets are not permitted.");
               }
            }
            // Add groundstations without FOV imagers for legacy users
            if (observers[ii]->GetType() == Gmat::GROUND_STATION)
            {
               directObservers.push_back(observers[ii]);
               directObserversNames.push_back(observers[ii]->GetName());
               directObserversHostIndex.push_back(ii);
            }
            else if (observers[ii]->GetType() == Gmat::SPACECRAFT)
            {
               if (!targetIsRegion)
               {
                  std::string errmsg = "Error setting observers on ContactLocator ";
                  errmsg += instanceName + ", all spacecraft observers must have an imager with a FieldOfView object attached if the target is a Spacecraft\n";
                  throw EventException(errmsg);
               }
            }
            else if (observers[ii]->GetType() == Gmat::REGION)
            {
               observedRegions.push_back(observers[ii]);
            }
         }
      }

      if (runMode != "Disabled")
      {
         // Set up the observers so that we can do Contact Location
         for (UnsignedInt ii = 0; ii < observers.size(); ii++)
         {
            if (observers[ii]->GetType() == Gmat::GROUND_STATION)
            {
               GroundstationInterface *gsi = (GroundstationInterface*)observers.at(ii);
               if (!gsi->InitializeForContactLocation(true))  // use false for testing resulting files
               {
                  std::string errmsg = "Error writing SPK or FK kernel for Ground Station ";
                  errmsg += observerNames.at(ii) + " used by ContactLocator ";
                  errmsg += instanceName + "\n";
                  throw EventException(errmsg);
               }
            }
            // Add Once Spacecraft CK and FK implemented
            /*
            if (observers[ii]->GetType() == Gmat::SPACECRAFT)
            {
               if (!observers[ii]->InitializeForContactLocation(true))  // use false for testing resulting files
               {
                  std::string errmsg = "Error writing SPK or FK kernel for Ground Station ";
                  errmsg += observerNames.at(ii) + " used by ContactLocator ";
                  errmsg += instanceName + "\n";
                  throw EventException(errmsg);
               }
            }
            */
         }
         for (UnsignedInt ii = 0; ii < directObservers.size(); ii++)
         {
            if (directObservers[ii]->GetType() != Gmat::GROUND_STATION)
            {
               if (!SetUpImagerSPICEFKandIK((Imager*)directObservers[ii], ii))  // use false for testing resulting files
               {
                  std::string errmsg = "Error writing IK or FK kernel for Imager ";
                  errmsg += observerNames.at(ii) + " used by ContactLocator ";
                  errmsg += instanceName + "\n";
                  throw EventException(errmsg);
               }
            }
         }
      }

      // Set Up Column Orders based on the templates
      StringArray tempColumns;
      if (reportTemplateFormat == "AzimuthElevationRangeReport" || reportTemplateFormat == "AzimuthElevationRangeRangeRateReport")
      {
         if (reportIntervals)
         {
            tempColumns.push_back("PassNumber");
            tempColumns.push_back("Observer");
            tempColumns.push_back("IntervalTime" + reportTimeFormat);
            tempColumns.push_back("IntervalAzimuth");
            tempColumns.push_back("IntervalElevation");
            tempColumns.push_back("IntervalRange");
            if (reportTemplateFormat == "AzimuthElevationRangeRangeRateReport")
               tempColumns.push_back("IntervalRangeRate");
         }
         else
         {
            throw EventException("ReportIntervals must be non-zero to create a \"" +
               reportTemplateFormat + "\"");
         }
      }
      else if (reportTemplateFormat == "ContactRangeReport")
      {
         tempColumns.push_back("Observer");
         tempColumns.push_back("Duration");
         tempColumns.push_back("StartTime" + reportTimeFormat);
         tempColumns.push_back("EndTime" + reportTimeFormat);
         tempColumns.push_back("StartRange");
         tempColumns.push_back("EndRange");
         reportIntervals = false;
         intervalStep = 0;
      }
      else if (reportTemplateFormat == "SiteViewMaxElevationReport")
      {
         tempColumns.push_back("Observer");
         tempColumns.push_back("StartTime" + reportTimeFormat);
         tempColumns.push_back("EndTime" + reportTimeFormat);
         tempColumns.push_back("Duration");
         tempColumns.push_back("MaxElevation");
         tempColumns.push_back("MaxElevationTime" + reportTimeFormat);
         reportIntervals = false;
         intervalStep = 0;
      }
      else if (reportTemplateFormat == "SiteViewMaxElevationRangeReport")
      {
         tempColumns.push_back("Observer");
         tempColumns.push_back("StartTime" + reportTimeFormat);
         tempColumns.push_back("EndTime" + reportTimeFormat);
         tempColumns.push_back("Duration");
         tempColumns.push_back("MaxElevation");
         tempColumns.push_back("MaxElevationTime" + reportTimeFormat);
         tempColumns.push_back("StartRange");
         tempColumns.push_back("EndRange");
         reportIntervals = false;
         intervalStep = 0;
      }
      else if (reportTemplateFormat == "Legacy")
      {
         tempColumns.clear();
         reportIntervals = false;
         intervalStep = 0;
      }

      reportColumnsInOrder.resize(tempColumns.size());
      reportColumnsInOrder = tempColumns;
      // Initialize the member event functions
      retval = true;
   }

   #ifdef DEBUG_CONTACTLOCATOR_INIT
      MessageInterface::ShowMessage("In CL::Init  about to set locatingString\n");
   #endif
   SetLocatingString("ContactLocator");

   return retval;
}

//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
 //------------------------------------------------------------------------------
bool ContactLocator::ReportEventDataLegacy(const std::string &reportNotice)
{
#ifdef DEBUG_CONTACT_LOCATOR_WRITE
   MessageInterface::ShowMessage("ContactLocator::ReportEventData ... \n");
#endif

   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return false;
   }

   std::string    itsName = targetName;
   theReport << "Target: " << itsName << "\n\n";

   Integer     sz = (Integer)contactResults.size();
   std::string noEvents = GetNoEventsString("contact");

#ifdef DEBUG_CONTACT_LOCATOR_WRITE
   MessageInterface::ShowMessage("attempting to write out %d events\n",
      (Integer)sz);
#endif
   Integer numIndividual = 0;

   if (sz == 0)
   {
      theReport << "\n" << noEvents << "\n";
   }
   // Loop over the total events list
   for (Integer ii = 0; ii < sz; ii++)
   {
      ContactResult* ev = contactResults.at(ii);
      numIndividual = ev->NumberOfEvents();
      ev->SetNoEvents(noEvents);

      std::string eventString = ev->GetReportString();
      theReport << eventString << "\n";

      theReport << "\nNumber of events : " << numIndividual << "\n\n\n";
   }

   //   for (unsigned int jj = 0; jj < sz; jj++)
   //      numIndividual += contactResults.at(jj)->NumberOfEvents();

   //   theReport << "\nNumber of events : " << numIndividual << "\n\n\n";

   theReport.close();
   return true;
}


//------------------------------------------------------------------------------
// bool FormatReportData(Integer &maxColumnWidth, Integer ii, Integer jj, Integer kk, Integer iii, std::string s)
//------------------------------------------------------------------------------
/**
 * Saves the data to a cell for outputting and checks the data width against the
 * column width. If it is larger, set maxColumnWidth to the new column width.
 *
 * @param maxColumnWidth Defines the width for the column to be set at. Used for formatting
 *
 * @param ii - column number
 *
 * @param jj - observer number
 *
 * @param kk - event number
 *
 * @param iii - iteration of aer section
 *
 * @param s - data in the form of a string to output to the report
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
void ContactLocator::FormatReportData(Integer &maxColumnWidth, Integer ii, Integer jj, Integer kk, Integer iii, std::string s)
{
   if (contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth == 0)
   {
      contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth = contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle.size();
   }
   contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).cellData.push_back(s);
   if (s.size() > contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth)
   {
      maxColumnWidth = s.size();
   }
   else if (maxColumnWidth < contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth)
   {
      maxColumnWidth = contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth;
   }
   return;
}

//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
bool ContactLocator::ReportEventData(const std::string &reportNotice)
{
   if (reportColumnsInOrder.size() == 0)
   {
      ReportEventDataLegacy(reportNotice);
      return true;
   }

#ifdef DEBUG_CONTACT_LOCATOR_WRITE
   MessageInterface::ShowMessage("ContactLocator::ReportEventData ... \n");
#endif

   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return false;
   }

   std::string    itsName = targetName;
   theReport << "Target: " << itsName << "\n\n";

   Integer     sz = (Integer)contactResults.size();
   std::string noEvents = GetNoEventsString("contact");

#ifdef DEBUG_CONTACT_LOCATOR_WRITE
   MessageInterface::ShowMessage("attempting to write out %d events\n",
      (Integer)sz);
#endif

   Integer totalNumEvents = 0;

   for (Integer ii = 0; ii < sz; ii++)
   {
      totalNumEvents += contactResults.at(ii)->NumberOfEvents();
   }

   /// Find an event with a non-zero number of passes
   Integer nz = -1;
   for (Integer ii = 0; ii < contactResults.size(); ii++)
   {
      if (contactResults.at(ii)->NumberOfEvents() != 0)
      {
         nz = ii;
      }
   }
   if(nz != -1)
   {
      for (Integer ii = 0; ii < (contactResults.at(nz))->GetEvent(0)->reportColumns.size(); ii++)
      {
         Integer maxColumnWidth = 0;
         for (Integer jj = 0; jj < contactResults.size(); jj++)
         {
            for (Integer kk = 0; kk < contactResults.at(jj)->NumberOfEvents(); kk++)
            {
               std::stringstream s("");
               std::string columnToUse = contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnName;
               if (columnToUse == "StartTimeUTCGregorian")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Time (UTCGregorian)";
                  std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
                  std::string startGregorian;
                  Real        resultMjd;

                  TimeSystemConverter::Instance()->Convert("A1ModJulian", contactResults.at(jj)->GetEvent(kk)->GetStart(), "",
                     outputFormat, resultMjd, startGregorian);
                  s << startGregorian;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "StartTimeISOYD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Time (ISO-YD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->GetStart(), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  A1Mjd a1Mjd = A1Mjd(utcMjd);
                  A1Date dateTime = a1Mjd.ToA1Date();
                  Integer dayOfYear = (Integer)dateTime.ToDayOfYear();
                  std::string daySTR = "";
                  if (dayOfYear < 10)
                  {
                     daySTR = "00";
                  }
                  else if (dayOfYear < 100)
                  {
                     daySTR = "0";
                  }
                  Integer year = dateTime.GetYear();
                  Integer hour = dateTime.GetHour();
                  Integer minute = dateTime.GetMinute();
                  Real second = dateTime.GetSecond();

                  s << year << "-" << daySTR << dayOfYear << "T" << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":";
                  if (second < 10)
                  {
                     s << "0" << std::fixed << std::setprecision(3) << second;
                  }
                  else
                  {
                     s << std::fixed << std::setprecision(3) << second;
                  }
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "StartTimeUTCMJD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Time (UTC-MJD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->GetStart(), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  s << std::fixed << std::setprecision(8) << utcMjd;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndTimeUTCGregorian")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Time (UTCGregorian)";
                  std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
                  std::string endGregorian;
                  Real        resultMjd;

                  TimeSystemConverter::Instance()->Convert("A1ModJulian", contactResults.at(jj)->GetEvent(kk)->GetEnd(), "",
                     outputFormat, resultMjd, endGregorian);
                  s << endGregorian;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndTimeISOYD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Time (ISO-YD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->GetEnd(), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  A1Mjd a1Mjd = A1Mjd(utcMjd);
                  A1Date dateTime = a1Mjd.ToA1Date();
                  Integer dayOfYear = (Integer)dateTime.ToDayOfYear();
                  std::string daySTR = "";
                  if (dayOfYear < 10)
                  {
                     daySTR = "00";
                  }
                  else if (dayOfYear < 100)
                  {
                     daySTR = "0";
                  }
                  Integer year = dateTime.GetYear();
                  Integer hour = dateTime.GetHour();
                  Integer minute = dateTime.GetMinute();
                  Real second = dateTime.GetSecond();

                  s << year << "-" << daySTR << dayOfYear << "T" << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":";
                  if (second < 10)
                  {
                     s << "0" << std::fixed << std::setprecision(3) << second;
                  }
                  else
                  {
                     s << std::fixed << std::setprecision(3) << second;
                  }
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndTimeUTCMJD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Time (UTC-MJD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->GetEnd(), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  s << std::fixed << std::setprecision(8) << utcMjd;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "SignalPath")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Signal Path";
                  s << contactResults.at(jj)->GetEvent(kk)->eventSignalPath;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "Duration")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Duration (s)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->GetDuration();
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "StartRange")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Range (km)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventRanges.at(0);
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndRange")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Range (km)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventRanges.back();
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "StartAzimuth")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Azimuth (deg)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventAzimuths.at(0);
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndAzimuth")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Azimuth (deg)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventAzimuths.back();
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "StartElevation")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Start Elevation (deg)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventElevations.at(0);
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "EndElevation")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Stop Elevation (deg)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventElevations.back();
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "MaxElevation")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Maximum Elevation (deg)";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventMaxElevation;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "MaxElevationTimeUTCGregorian")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Max Elevation Time (UTCGregorian)";
                  std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
                  std::string endGregorian;
                  Real        resultMjd;

                  TimeSystemConverter::Instance()->Convert("A1ModJulian", contactResults.at(jj)->GetEvent(kk)->eventMaxElevationEpoch, "",
                     outputFormat, resultMjd, endGregorian);
                  s << endGregorian;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "MaxElevationTimeISOYD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Max Elevation Time (ISO-YD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->eventMaxElevationEpoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  A1Mjd a1Mjd = A1Mjd(utcMjd);
                  A1Date dateTime = a1Mjd.ToA1Date();
                  Integer dayOfYear = (Integer)dateTime.ToDayOfYear();
                  std::string daySTR = "";
                  if (dayOfYear < 10)
                  {
                     daySTR = "00";
                  }
                  else if (dayOfYear < 100)
                  {
                     daySTR = "0";
                  }
                  Integer year = dateTime.GetYear();
                  Integer hour = dateTime.GetHour();
                  Integer minute = dateTime.GetMinute();
                  Real second = dateTime.GetSecond();

                  s << year << "-" << daySTR << dayOfYear << "T" << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":";
                  if (second < 10)
                  {
                     s << "0" << std::fixed << std::setprecision(3) << second;
                  }
                  else
                  {
                     s << std::fixed << std::setprecision(3) << second;
                  }
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "MaxElevationTimeUTCMJD")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Max Elevation Time (UTC-MJD)";

                  TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                  Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->eventMaxElevationEpoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                  s << std::fixed << std::setprecision(8) << utcMjd;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "PassNumber")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Pass Number";
                  s << std::fixed << std::setprecision(reportPrecision) << kk + 1;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (columnToUse == "Observer")
               {
                  contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Observer";
                  s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventStation;
                  FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
               }
               else if (reportIntervals)
               {
                  if (columnToUse == "IntervalTimeISOYD")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventTimes.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Time (ISO-YD)";

                        TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                        Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->eventTimes.at(iii), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                        A1Mjd a1Mjd = A1Mjd(utcMjd);
                        A1Date dateTime = a1Mjd.ToA1Date();
                        Integer dayOfYear = (Integer)dateTime.ToDayOfYear();
                        std::string daySTR = "";
                        if (dayOfYear < 10)
                        {
                           daySTR = "00";
                        }
                        else if (dayOfYear < 100)
                        {
                           daySTR = "0";
                        }
                        Integer year = dateTime.GetYear();
                        Integer hour = dateTime.GetHour();
                        Integer minute = dateTime.GetMinute();
                        Real second = dateTime.GetSecond();

                        s << year << "-" << daySTR << dayOfYear << "T" << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":";
                        if (second < 10)
                        {
                           s << "0" << std::fixed << std::setprecision(3) << second;
                        }
                        else
                        {
                           s << std::fixed << std::setprecision(3) << second;
                        }
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalTimeUTCGregorian")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventTimes.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Time (UTCGregorian)";
                        std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
                        std::string timeGregorian;
                        Real        resultMjd;

                        TimeSystemConverter::Instance()->Convert("A1ModJulian", contactResults.at(jj)->GetEvent(kk)->eventTimes.at(iii), "",
                           outputFormat, resultMjd, timeGregorian);
                        s << timeGregorian;
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalTimeUTCMJD")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventTimes.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Time (UTC-MJD)";

                        TimeSystemConverter *tcv = TimeSystemConverter::Instance();
                        Real utcMjd = tcv->Convert(contactResults.at(jj)->GetEvent(kk)->eventTimes.at(iii), TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD);

                        s << std::fixed << std::setprecision(8) << utcMjd;
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalAzimuth")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventAzimuths.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Azimuth (deg)";
                        s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventAzimuths.at(iii);
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalElevation")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventElevations.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Elevation (deg)";
                        s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventElevations.at(iii);
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalRange")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventRanges.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Range (km)";
                        s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventRanges.at(iii);
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
                  else if (columnToUse == "IntervalRangeRate")
                  {
                     contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).aerColumn = true;
                     for (Integer iii = 0; iii < contactResults.at(jj)->GetEvent(kk)->eventRangeRates.size(); iii++)
                     {
                        s.str(std::string());
                        contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnTitle = "Range Rate (km/s)";
                        s << std::fixed << std::setprecision(reportPrecision) << contactResults.at(jj)->GetEvent(kk)->eventRangeRates.at(iii);
                        FormatReportData(maxColumnWidth, ii, jj, kk, 0, s.str());
                     }
                  }
               }
               else
               {
                  //inappropriate input

                  std::string errmsg = "Error: The Column Name " + columnToUse + " is not supported as a column type";
                  throw EventException(errmsg);
                  break;
               }
            }
         }

         for (Integer jj = 0; jj < contactResults.size(); jj++)
         {
            for (Integer kk = 0; kk < contactResults.at(jj)->NumberOfEvents(); kk++)
            {
               contactResults.at(jj)->GetEvent(kk)->reportColumns.at(ii).columnWidth = maxColumnWidth;
            }
         }
      }

      if (sz == 0)
      {
         theReport << "\n" << noEvents << "\n";
      }
      else
      {
         std::stringstream totalString("\n");
         std::stringstream dashesString("");


         for (Integer ii = 0; ii < (contactResults.at(nz))->GetEvent(0)->reportColumns.size(); ii++)
         {
            std::string nameString;
            nameString = contactResults.at(nz)->GetEvent(0)->reportColumns.at(ii).columnTitle;
            if (nameString.size() < contactResults.at(nz)->GetEvent(0)->reportColumns.at(ii).columnWidth)
            {
               std::string nameSpaces("");
               for (Integer i = 0; i < (contactResults.at(nz)->GetEvent(0)->reportColumns.at(ii).columnWidth - nameString.size()); i++)
               {
                  nameSpaces += " ";
               }

               nameString += nameSpaces;
            }
            totalString << nameString << contactResults.at(nz)->GetEvent(0)->columnSpacing;
            for (Integer jj = 0; jj < nameString.size(); jj++)
            {
               dashesString << "-";
            }
            dashesString << contactResults.at(nz)->GetEvent(0)->columnSpacing;
         }

         totalString << "\n" << dashesString.str() << "\n";
         theReport << totalString.str();
      }
   }

   // Rearrange data by time, then report
   for (Integer ii = 0; ii < totalNumEvents; ii++)
   {
      Real compTime = -1;
      std::string currentReportString;
      ContactEvent *reportEvent;
      for (Integer jj = 0; jj < sz; jj++)
      {
         for (Integer kk = 0; kk < (contactResults.at(jj))->NumberOfEvents(); kk++)
         {
            if (contactResults.at(jj)->GetEvent(kk)->eventNumber == -1)
            {
               if (compTime == -1)
               {
                  reportEvent = contactResults.at(jj)->GetEvent(kk);
                  compTime = contactResults.at(jj)->GetEvent(kk)->GetStart();
               }
               if (contactResults.at(jj)->GetEvent(kk)->GetStart() < compTime)
               {
                  reportEvent = contactResults.at(jj)->GetEvent(kk);
                  compTime = contactResults.at(jj)->GetEvent(kk)->GetStart();
               }
            }
         }
      }
      reportEvent->eventNumber = ii;
      // Correct the pass number for multiple observers
      if (contactResults.size() > 1)
      {
         for (Integer jj = 0; jj < reportEvent->reportColumns.size(); jj++)
         {
            if (reportEvent->reportColumns.at(jj).columnName == "PassNumber")
            {
               reportEvent->reportColumns.at(jj).cellData[0] = GmatStringUtil::ToString(ii + 1);
            }
         }
      }
      if (reportIntervals)
      {
         theReport << reportEvent->GetAERString() << "\n";
      }
      else
      {
         theReport << reportEvent->GetReportString() << "\n";
      }
   }
   if (nz == -1)
   {
      theReport << "No contacts detected" << "\n";
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
void ContactLocator::FindEvents()
{
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("Entering ContactLocator::FindEvents ...\n");
      MessageInterface::ShowMessage("Stations (%d) are:\n", observers.size());
      for (Integer ii = 0; ii < observers.size(); ii++)
         MessageInterface::ShowMessage("    %d     %s<%p>\n",
               ii, (observers.at(ii)->GetName()).c_str(), observers.at(ii));
   #endif


   // Set up data for the calls to CSPICE

   // @YRL
   std::string      theObsrvr = ""; // we will loop over observers for this
   // up to this line
//   std::string      theFront  = "";  // we will loop over occultingBodies
//   std::string      theFShape = "ELLIPSOID";
//   std::string      theFFrame = ""; // we will loop over occultingBodies for this
//   std::string      theBack   = "SUN";
//   std::string      theBShape = "ELLIPSOID";
//   std::string      theBFrame = "IAU_SUN";
   std::string      theAbCorr = GetAbcorrString();
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("---- theAbCorr  = %s\n", theAbCorr.c_str());
   #endif

   Integer        numContacts = 0;
   RealArray      starts;
   RealArray      ends;
   RealArray      maxElevationTimes;

   // Need to set findStart and findStop somewhere in here!!!!

   // Clear old events
   TakeAction("Clear", "Events");
   // @YRL

   if (!targetIsRegion)
   {
      for (Integer j = 0; j < directObservers.size(); j++)
      {
         bool isImagerObserver = false;
         // We want a ContactResult for each station whether or not there are events
         ContactResult *evList = new ContactResult();
         if (directObservers[j]->GetType() == Gmat::GROUND_STATION)
         {
            evList->SetObserverName(observers.at(j)->GetName());
         }
         else
         {
            evList->SetObserverName((observers.at(directObserversHostIndex.at(j))->GetName()) + "." + directObservers.at(j)->GetName());
            isImagerObserver = true;
         }

         starts.clear();
         ends.clear();
         maxElevationTimes.clear();
         Integer obsNaifId;
         std::string imagerNAIFId;
         if (!isImagerObserver)
         {
            obsNaifId = directObservers.at(j)->GetIntegerParameter(directObservers.at(j)->GetParameterID("NAIFId"));
         }
         else
         {
            obsNaifId = observers.at(directObserversHostIndex[j])->GetIntegerParameter(observers.at(directObserversHostIndex[j])->GetParameterID("NAIFId"));

            imagerNAIFId = GmatStringUtil::ToString(directObservers.at(j)->GetIntegerParameter("NAIFId"));
         }
         theObsrvr = GmatStringUtil::ToString(obsNaifId);
         std::string obsFrame = observers.at(directObserversHostIndex[j])->GetStringParameter("SpiceFrameId");
         Real  minElAngle;
         if (observers.at(directObserversHostIndex[j])->GetType() == Gmat::GROUND_STATION)
         {
            minElAngle = observers.at(directObserversHostIndex[j])->GetRealParameter("MinimumElevationAngle");
         }
         else
         {
            minElAngle = 0;
         }

         // The ground station's central body should not be an occulting body (Expand for SC observers)
         StringArray bodiesToUse;
         std::string currentBody;
         std::string centralBody = observers.at(directObserversHostIndex[j])->GetStringParameter(
            observers.at(directObserversHostIndex[j])->GetParameterID("CentralBody"));

         for (unsigned int ii = 0; ii < occultingBodyNames.size(); ii++)
         {
            currentBody = occultingBodyNames.at(ii);
            if (currentBody == centralBody)
            {
               //            if (!centralBodyWarningWritten)
               //            {
               MessageInterface::ShowMessage(
                  "*** WARNING *** Body %s is the central body for "
                  "GroundStation %s and so will not be considered an occulting body "
                  "for contact location.\n", centralBody.c_str(),
                  (observers.at(j)->GetName()).c_str());
               //               centralBodyWarningWritten = true;
               //            }
            }
            else
            {
               bodiesToUse.push_back(currentBody);
            }
         }


#ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("Calling GetContactIntervals with: \n");
         MessageInterface::ShowMessage("   theObsrvr         = %s(%s)\n",
            (observers.at(j))->GetName().c_str(), theObsrvr.c_str());
         MessageInterface::ShowMessage("   occultingBodies   = \n");
         for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
            MessageInterface::ShowMessage("      %d     %s\n", ii, occultingBodyNames.at(ii).c_str());
         MessageInterface::ShowMessage("   bodiesToUse   = \n");
         for (Integer ii = 0; ii < bodiesToUse.size(); ii++)
            MessageInterface::ShowMessage("      %d     %s\n", ii, bodiesToUse.at(ii).c_str());
         MessageInterface::ShowMessage("   theAbCorr         = %s\n", theAbCorr.c_str());
         MessageInterface::ShowMessage("   initialEp         = %12.10f\n", initialEp);
         MessageInterface::ShowMessage("   finalEp           = %12.10f\n", finalEp);
         MessageInterface::ShowMessage("   useEntireInterval = %s\n", (useEntireInterval ? "true" : "false"));
         MessageInterface::ShowMessage("   stepSize          = %12.10f\n", stepSize);
#endif
         bool transmit = (GmatStringUtil::ToUpper(lightTimeDirection) == "TRANSMIT");
         em->GetContactIntervals(theObsrvr, minElAngle, obsFrame, bodiesToUse, theAbCorr,
            initialEp, finalEp, useEntireInterval, useLightTimeDelay, transmit, stepSize, numContacts,
            starts, ends, isImagerObserver, imagerNAIFId, maxElevationTimes);
#ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("After GetContactIntervals: \n");
         MessageInterface::ShowMessage("   numContacts       = %d\n", numContacts);
#endif
         if (numContacts > 0)
         {
            // Insert the events into the array
            for (Integer kk = 0; kk < numContacts; kk++)
            {
               Real s1 = starts.at(kk);
               Real e1 = ends.at(kk);
               Real maxElevationTime = maxElevationTimes.at(kk);
               ContactEvent *newEvent = new ContactEvent(s1, e1, reportColumnsInOrder, leftJustified);

               SpiceInt target = this->sat->GetIntegerParameter("NAIFId");
               SpiceInt obs = obsNaifId;

               ConstSpiceChar *obsFrameConstSpiceChar = obsFrame.c_str();
               ConstSpiceChar *theAbCorrConstSpiceChar = theAbCorr.c_str();

               SpiceDouble azimuthEpoch, elevationEpoch, radiusEpoch, radialVelEpoch;
               SpiceDouble maxAzimuthEpoch, maxElevationEpoch, maxRadiusEpoch;

               newEvent->eventTimes.push_back(s1);
               newEvent->eventTimes.push_back(e1);

               if (reportColumnsInOrder.size() > 0)
               {
                  SpiceDouble startState[6];
                  em->GetTargetState(target,
                     s1,
                     obsFrameConstSpiceChar,
                     theAbCorrConstSpiceChar,
                     obs,
                     startState);

                  SpiceDouble startPos[3];
                  startPos[0] = startState[0];
                  startPos[1] = startState[1];
                  startPos[2] = startState[2];

                  // Move this to Ephem Manager
                  reclat_c(startPos, &radiusEpoch, &azimuthEpoch, &elevationEpoch);

                  if (radiusEpoch != 0)
                     radialVelEpoch = (startState[3] * startState[0] +
                        startState[4] * startState[1] +
                        startState[5] * startState[2]) / radiusEpoch;
                  else
                     radialVelEpoch = 0;

                  azimuthEpoch = -azimuthEpoch;
                  if (azimuthEpoch < 0)
                  {
                     azimuthEpoch = 2 * GmatMathConstants::PI + azimuthEpoch;
                  }

                  newEvent->eventRanges.push_back(radiusEpoch);
                  newEvent->eventRangeRates.push_back(radialVelEpoch);
                  newEvent->eventAzimuths.push_back(azimuthEpoch * GmatMathConstants::DEG_PER_RAD);
                  newEvent->eventElevations.push_back(elevationEpoch * GmatMathConstants::DEG_PER_RAD);

                  SpiceDouble maxElState[6];
                  em->GetTargetState(target,
                     maxElevationTime,
                     obsFrameConstSpiceChar,
                     theAbCorrConstSpiceChar,
                     obs,
                     maxElState);

                  SpiceDouble maxElPos[3];
                  maxElPos[0] = maxElState[0];
                  maxElPos[1] = maxElState[1];
                  maxElPos[2] = maxElState[2];

                  // Move this to Ephem Manager
                  reclat_c(maxElPos, &maxRadiusEpoch, &maxAzimuthEpoch, &maxElevationEpoch);

                  newEvent->eventMaxElevation = maxElevationEpoch * GmatMathConstants::DEG_PER_RAD;
                  newEvent->eventMaxElevationEpoch = maxElevationTime;

                  if (reportIntervals)
                  {
                     Real intervalToReport = e1 - s1;
                     Integer numToReport = (Integer)floor(intervalToReport / (intervalStep / 86400.0));

                     for (int i = 0; i <= numToReport - 1; i++)
                     {
                        Real epochMod = s1 + (intervalStep / 86400.0) * (i + 1);
                        SpiceDouble epochState[6];
                        em->GetTargetState(target,
                           epochMod,
                           obsFrameConstSpiceChar,
                           theAbCorrConstSpiceChar,
                           obs,
                           epochState);

                        SpiceDouble epochPos[3];
                        epochPos[0] = epochState[0];
                        epochPos[1] = epochState[1];
                        epochPos[2] = epochState[2];

                        reclat_c(epochPos, &radiusEpoch, &azimuthEpoch, &elevationEpoch);

                        if (radiusEpoch != 0)
                           radialVelEpoch = (epochState[3] * epochState[0] +
                              epochState[4] * epochState[1] +
                              epochState[5] * epochState[2]) / radiusEpoch;
                        else
                           radialVelEpoch = 0;

                        azimuthEpoch = -azimuthEpoch;
                        if (azimuthEpoch < 0)
                        {
                           azimuthEpoch = 2 * GmatMathConstants::PI + azimuthEpoch;
                        }

                        newEvent->eventTimes.insert(newEvent->eventTimes.end() - 1, s1 + (i + 1)*(intervalStep / 86400.0));
                        newEvent->eventRanges.push_back(radiusEpoch);
                        newEvent->eventRangeRates.push_back(radialVelEpoch);
                        newEvent->eventAzimuths.push_back(azimuthEpoch * GmatMathConstants::DEG_PER_RAD);
                        newEvent->eventElevations.push_back(elevationEpoch * GmatMathConstants::DEG_PER_RAD);
                     }
                  }
                  SpiceDouble endState[6];
                  em->GetTargetState(target,
                     e1,
                     obsFrameConstSpiceChar,
                     theAbCorrConstSpiceChar,
                     obs,
                     endState);

                  SpiceDouble endPos[3];
                  endPos[0] = endState[0];
                  endPos[1] = endState[1];
                  endPos[2] = endState[2];

                  reclat_c(endPos, &radiusEpoch, &azimuthEpoch, &elevationEpoch);

                  if (radiusEpoch != 0)
                     radialVelEpoch = (endState[3] * endState[0] +
                        endState[4] * endState[1] +
                        endState[5] * endState[2]) / radiusEpoch;
                  else
                     radialVelEpoch = 0;

                  azimuthEpoch = -azimuthEpoch;
                  if (azimuthEpoch < 0)
                  {
                     azimuthEpoch = 2 * GmatMathConstants::PI + azimuthEpoch;
                  }

                  newEvent->eventRanges.push_back(radiusEpoch);
                  newEvent->eventRangeRates.push_back(radialVelEpoch);
                  newEvent->eventAzimuths.push_back(azimuthEpoch * GmatMathConstants::DEG_PER_RAD);
                  newEvent->eventElevations.push_back(elevationEpoch * GmatMathConstants::DEG_PER_RAD);
               }

               if (directObserversNames.at(j) != observers.at(directObserversHostIndex.at(j))->GetName())
               {
                  newEvent->eventSignalPath = sat->GetName() + "->" + observers.at(directObserversHostIndex.at(j))->GetName() + "." + (directObservers.at(j))->GetName();
                  newEvent->eventStation = (observers.at(directObserversHostIndex.at(j))->GetName()) + "." + (directObservers.at(j))->GetName();
               }
               else
               {
                  newEvent->eventSignalPath = sat->GetName() + "->" + (directObservers.at(j))->GetName();
                  newEvent->eventStation = (directObservers.at(j))->GetName();
               }

               evList->AddEvent(newEvent);
            }
         }
         // One result array for each station whether or not there are events
         contactResults.push_back(evList);
      }
   }
   else //targetIsRegion
   {
         ContactResult *regionEvList = new ContactResult();
         regionEvList->SetObserverName(sat->GetName());

         starts.clear();
         ends.clear();
         maxElevationTimes.clear();


         numContacts = 0;

         //Fixed interpolation tolerance for now. Half a millisecond as GMAT reports to the millisecond so this will be accurate to the reported value.
         Real tolerance = 0.0005 / GmatTimeConstants::SECS_PER_DAY;
         Real totalStepTaken = 0.0;

         // Conversions are required because Spice uses A1MJD and the ephemeris was created in TAIMJD
         Real previousTime = findStart;

         //Flags for current and previous 
         bool wasWithinRegion = false;
         bool isWithinRegion  = false;

#ifdef DEBUG_CONTACT_EVENTS

         // Entry and exit positions are for diagnostic purpose 
         RealArray entryLatitudes;
         RealArray entryLongitudes;
         RealArray entryHeights;

         RealArray exitLatitudes;
         RealArray exitLongitudes;
         RealArray exitHeights;
         RealArray epochs;
#endif

         //Loops through from findStart to findStop by the stepSize
         // Calculate the number of loops required
         if (stepSize <= 0.0)
            stepSize = 10.0;

         //Target Sats NAIFId
         targetSat = sat->GetIntegerParameter("NAIFId");
         std::string satBodyName = sat->GetOriginName();
         std::string regionBodyName = region->GetCentralBody()->GetName();

         //if (satBodyName != regionBodyName)
         //{
         //   throw EventException("The central body for region "+ region->GetFullName() + 
         //      " [" + regionBodyName + "] must match the central body for spacecraft " + 
         //      sat->GetFullName() + " [" + satBodyName + "].");
         //}

         //Target Body NAIFId
         obs = region->GetCentralBody()->GetIntegerParameter("NAIFId");
         std::string targetFrame = region->GetCentralBody()->GetStringParameter("SpiceFrameId");
         obsFrameConstSpiceChar = targetFrame.c_str();
         Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();

         std::string regionCentralBodyName = region->GetCentralBody()->GetName();
         if (runmode != GmatGlobal::TESTING)
         {
            if (satBodyName != "Earth" || regionCentralBodyName != "Earth")
               throw EventException("Central body other than \"Earth\" found for use with a PlanetographicRegion ContactLocator event. The only central body currently allowed is \"Earth\".\n Central bodies found:\n Observer: \"" + 
                  sat->GetName() + "\", Central body: \"" + satBodyName +
                  "\"\n Target: \"" + region->GetName() + "\", Central body: \"" + regionBodyName + "\"\n");
         }
         
         if (satBodyName != regionCentralBodyName)
         {
            throw EventException("The central body for spacecraft " +
               sat->GetName() + ": " + satBodyName +
               " doesn't match the central body for planetographic region " +
               region->GetName() + ": " + regionCentralBodyName + ".\n");
         }



         //Aberration correction flag
         //Set to NONE as we should not adjust for light time or aberrations in this context.
         std::string abcorrString = "NONE";
         theAbCorrConstSpiceChar = abcorrString.c_str();


         Integer numberLoops = (Integer)((findStop - findStart) * GmatTimeConstants::SECS_PER_DAY / stepSize);
         for (int count = 0; count <= numberLoops; ++count)
         {
            Rvector3 latLongHeight;
            Real distance;
            Rvector3 cartesian;
            Real m = (Real)count / numberLoops; // 0.0 <= m <= 1.0 
            Real currentTime = findStart + m * (findStop - findStart);
            if (count == numberLoops)
            {
               currentTime = findStop;
            }

            isWithinRegion = IsSatWithinRegion(currentTime, latLongHeight, distance);

            //Handles a change of state either in or out of the region
            if (isWithinRegion != wasWithinRegion)
            {
               Real crossingTime = findStart;
               if (count > 0)
               {
                  crossingTime = InterpolateRegionCrossing(previousTime, currentTime,
                     isWithinRegion, tolerance, latLongHeight);
               }

#ifdef DEBUG_CONTACT_EVENTS
               // debugging.
               Real latitude = latLongHeight[0] * GmatMathConstants::DEG_PER_RAD;
               Real longitude = latLongHeight[1] * GmatMathConstants::DEG_PER_RAD;
               Real height = latLongHeight[2];

               if (longitude > 180.0)
                  longitude -= 360.0;
#endif
               if (isWithinRegion)
               {
                  starts.push_back(crossingTime);
#ifdef DEBUG_CONTACT_EVENTS
                  entryLatitudes.push_back(latitude);
                  entryLongitudes.push_back(longitude);
                  entryHeights.push_back(height);
#endif
               }
               else
               {
                  ends.push_back(crossingTime);
#ifdef DEBUG_CONTACT_EVENTS
                  exitLatitudes.push_back(latitude);
                  exitLongitudes.push_back(longitude);
                  exitHeights.push_back(height);
#endif
               }
               wasWithinRegion = isWithinRegion;
            }
            previousTime = currentTime;
         }

         // Ended within the region
         if (starts.size() > ends.size())
            ends.push_back(findStop);

         //This loops through the contacts found reports the info out
         for (int j = 0; j < starts.size(); j++)
         {
            Real s1 = starts[j];
            Real e1 = ends[j];
            //Real maxElevationTime = maxElevationTimes.at(i);
            ContactEvent *newEvent = new ContactEvent(s1, e1, reportColumnsInOrder, leftJustified);

            newEvent->eventTimes.push_back(s1);
            newEvent->eventTimes.push_back(e1);

            //The following additions are a rough WIP which will be updated with the correct data when available.
            newEvent->eventSignalPath = targetName + "->" + sat->GetName();

            //eventStaion is just the observer. In this context, that means a spacecraft, in the original CL implementation, this was only a ground station.
            newEvent->eventStation = sat->GetName();
            regionEvList->AddEvent(newEvent);
         }
         // One result array for each station whether or not there are events
         contactResults.push_back(regionEvList);
   }


   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("ContactLocator::FindEvents leaving ... \n");
   #endif
   // @YRL, upto this line
}


//------------------------------------------------------------------------------
// Real ContactLocator::InterpolateRegionCrossing(Real low, Real high, bool isWithin, Real tolerance)
//------------------------------------------------------------------------------
/**
 * Interpolates across the region boundary to find a more accurate crossing epoch
 *
 * @param low,  A Real epoch that represents the low end of the range to check
 * @param high, A Real epoch that represents the high end of the range to check
 * @param isWithin, whether or not the sat is in or out of the region at the current high epoch
 * @param tolerance, the Real tolerance that the search will get to
 *
 * @return A real epoch that the sat crossed the region boundary
 */
 //------------------------------------------------------------------------------
Real ContactLocator::InterpolateRegionCrossing(Real low, Real high, bool isWithin, Real tolerance, Rvector3 & latLongHeight)
{
   //A binary search to find a more accurate region crossing time.

   Real checkEpoch = 0.0;

   bool isInRegion = isWithin;
   bool highIsIn   = isWithin;
   Real distance;
   Real lowDistance;
   Real highDistance;

#ifdef DEBUG_CONTACT_EVENTS
   static Integer totalLoop(0);
   static Real sumErrorSquared(0.0);
   RealArray distances;
   RealArray lats;
   RealArray longs;
   RealArray alts;
#endif

   for (int i = 0; i < 30; ++i)
   {
      checkEpoch = (high + low) / 2.0;
      Rvector3 latLongHeight;
      isInRegion = IsSatWithinRegion(checkEpoch, latLongHeight, distance);
#ifdef DEBUG_CONTACT_EVENTS
      ++totalLoop;
      distances.push_back(distance);
      lats.push_back(latLongHeight[0]);
      longs.push_back(latLongHeight[1]);
#endif

      if (isInRegion == highIsIn)
      {
         high = checkEpoch;
      }
      else
      {
         low = checkEpoch;
      }
   }
#ifdef DEBUG_CONTACT_EVENTS
   sumErrorSquared += (distance*distance);
#endif
   checkEpoch = (high + low) / 2.0;
   IsSatWithinRegion(checkEpoch, latLongHeight, distance);
   return checkEpoch;
}


//------------------------------------------------------------------------------
// bool ContactLocator::IsSatWithinRegion(Real epoch)
//------------------------------------------------------------------------------
/**
 * Finds out if the assigned sat is within the assigned planetographic region at
 * a given epoch
 *
 * @param epoch, the Real epoch to check
 *
 * @return true if the sat is in the region, false if not
 */
 //------------------------------------------------------------------------------
bool ContactLocator::IsSatWithinRegion(Real epoch, Rvector3 & latLongHeight, Real & distance)
{
   bool isWithinRegion = false;

   //Gets the body fixed position of the targetSat
   SpiceDouble epochState[6];
   em->GetTargetState(obs,
      epoch,
      obsFrameConstSpiceChar,
      theAbCorrConstSpiceChar,
      targetSat,
      epochState);

   //Currently hardcoded for earth body fixed coordinate system "ITRF93". A conversion to the user defined bodyFixed position should be done.
   //Spice should be able to handle that for us if we make sure to pass in the correct frame as "obsFrameConstSpiceChar"
   //and make sure it is defined in spice.

   //I am not sure of the definition of latitudinal coordinates for bodies other than Earth. Need to check

   //Gets the latitude, longitude, height, position of the targetSat given the body fixed position.
   
   BodyFixedPoint * centralBody = (BodyFixedPoint *)(region->GetCentralBody());
   Real equatorialRadius = centralBody->GetRealParameter("EquatorialRadius");
   Real flattening = centralBody->GetRealParameter("Flattening");
   Rvector3 cartesian = Rvector3(-epochState[0], -epochState[1], -epochState[2]);

   latLongHeight = PlanetographicRegion::CartesianToEllipsoid(cartesian, flattening, equatorialRadius);

   Real lat = latLongHeight[0] * GmatMathConstants::DEG_PER_RAD;
   Real lon = latLongHeight[1] * GmatMathConstants::DEG_PER_RAD;
   Real height = latLongHeight[2];

   //Call to planetographic region function that returns if a given state is inside a region. 
   isWithinRegion = region->isWithin(lat, lon, height, distance);
   return isWithinRegion;
}

//------------------------------------------------------------------------------
// std::string GetAbcorrString()
//------------------------------------------------------------------------------
/**
 * Returns the aberration correction for use in CSPICE calls
 *
 * @return string representing the selected aberration corrections
 */
//------------------------------------------------------------------------------
std::string ContactLocator::GetAbcorrString()
{
   std::string correction =  EventLocator::GetAbcorrString();
   if (useLightTimeDelay && (lightTimeDirection == "Transmit"))
   {
      correction = 'X' + correction;
   }
   return correction;
}

std::string ContactLocator::GetSpiceFrame(const std::string & bodyName)
{
   // Check for the Sun
   if (bodyName == GmatSolarSystemDefaults::SUN_NAME)
      return GmatSolarSystemDefaults::STAR_SPICE_FRAME_ID;

   // Check for valid planets
   for (Integer planetIndex = 0; planetIndex < GmatSolarSystemDefaults::NumberOfDefaultPlanets; ++planetIndex)
   {
      if (bodyName == GmatSolarSystemDefaults::PLANET_NAMES[planetIndex])
      {
         return GmatSolarSystemDefaults::PLANET_SPICE_FRAME_ID[planetIndex];
      }
   }

   // Check for valid moons
   for (Integer moonIndex = 0; moonIndex < GmatSolarSystemDefaults::NumberOfDefaultMoons; ++moonIndex)
   {
      if (bodyName == GmatSolarSystemDefaults::MOON_NAMES[moonIndex])
      {
         return GmatSolarSystemDefaults::MOON_SPICE_FRAME_ID[moonIndex];
      }
   }
   return std::string();
}


//------------------------------------------------------------------------------
// void SetUpImagerSPICEFKandIK(Imager * imagerInUse, std::string hostObjName, Integer indxNum)
//------------------------------------------------------------------------------
/**
 * Function used to set up the Instrument and frame kernal of an imager
 *
 * @param imagerInUse The imager of current concern
 * @param hostObjName The name of the ground station or spacecraft the imager is attached to
 * @param The numer of the observer relative to the other observers
 *
 * @return true set up completed
 */
 //------------------------------------------------------------------------------
bool ContactLocator::SetUpImagerSPICEFKandIK(Imager * imagerInUse, Integer indxNum)
{
   // Now set up the imagers (currently only works for GroundStations)

   if (!spice)
      spice = new SpiceInterface();

   // Get data for the FOV from GMAT
   std::string fovShape;
   FieldOfView *theFOV = imagerInUse->GetFieldOfView();
   if (theFOV->IsOfType(Gmat::CONICAL_FOV))
   {
      // Remove the exception below to enable ConicalFOV
      throw EventException("A ConicalFov may not be used on an Antenna attached to a GroundStation. Remove " + theFOV->GetName() + " from " + imagerInUse->GetName() + " or replace it with a CustomFOV object.");
      fovShape = "CIRCLE";
   }
   else if (theFOV->IsOfType(Gmat::RECTANGULAR_FOV))
   {
      // Remove the exception below to enable RectangularFOV
      throw EventException("A RectangularFOV may not be used on an Antenna attached to a GroundStation. Remove " + theFOV->GetName() + " from "+imagerInUse->GetName() +" or replace it with a CustomFOV object.");
      fovShape = "RECTANGLE";
   }
   else if (theFOV->IsOfType(Gmat::CUSTOM_FOV))
      fovShape = "POLYGON";
   else
   {
      // Should never hit this currently, but possible if new FOV types added later
      std::string errmsg = "Error setting imagers on IntrusionLocator ";
      errmsg += instanceName + ", Imager " +
         imagerInUse->GetName() + " is using an unsupported "
         "FOV shape. Allowable shapes are [Custom]";
      throw EventException(errmsg);
   }

   // Get the Host object

   GmatBase *hostObj = observers[directObserversHostIndex[indxNum]];
   std::string hostObjName = hostObj->GetName();
   std::string spiceFilePath = GmatFileUtil::GetTemporaryDirectory();
   std::string now = GmatTimeUtil::FormatCurrentTime(4);

   // Store spacecraft NAIF Id
   Integer observerNAIFId = hostObj->GetIntegerParameter("NAIFId");
   std::string observerNAIFIdString = GmatStringUtil::ToString(observerNAIFId);
   if (observerNAIFId == 0)
   {
      std::string errMsg = "The observer \"" + hostObj->GetName() +
         "\" for ContactLocator \"" + instanceName + "\" has a NAIF ID of 0.\n";
      throw EventException(errMsg);
   }
   if (GmatMathUtil::Abs(observerNAIFId * 100.0) >= 2147483647.0)
   {
      std::string errmsg = "The NAIF ID on observer " + hostObj->GetName() +
         " is too large. The ID must satisfy the relation "
         "(abs(NAIFID * 100) < 2147483647) in order to "
         "create the associated frame kernels with "
         "proper IDs for ContactLocator ";
      errmsg += instanceName + "\n";
      throw EventException(errmsg);
   }

   // Create the instrument definition file for the FOV's

   // To get a unique id, check which piece of hardware the current sensor is
   StringArray allHardwareNames = sat->GetRefObjectNameArray(Gmat::HARDWARE);

   // Prepare an id for this hardware, recommended by SPICE documents
   // to use s/c ID * 1000 - instrument number
   // Id can't be larger than 9 digits, or SPICE will fail
   Integer frameId = 0;
   if (observerNAIFId > 0)
      frameId = observerNAIFId * 100 + indxNum;
   else
      frameId = observerNAIFId * 100 - indxNum;
   imagerInUse->SetIntegerParameter("NAIFId", frameId);
   std::string frameIdStr = GmatStringUtil::ToString(frameId);
   std::string frameName = GmatStringUtil::ToUpper(hostObjName) +
      "_" + GmatStringUtil::ToUpper(imagerInUse->GetName());

   if (frameName.size() > 26)
      frameName = frameName.substr(0, 25);

   ikFileName.push_back(spiceFilePath + hostObjName + "ImagerFOVs_" + now + ".tf");
   if (GmatFileUtil::DoesFileExist(ikFileName.back()))
      remove(ikFileName.back().c_str());
   std::ofstream ikStream;
   ikStream.open(ikFileName.back());

   fkFileName.push_back(spiceFilePath + hostObjName + "ImagerFrames_" + now + ".tf");
   if (GmatFileUtil::DoesFileExist(fkFileName.back()))
      remove(fkFileName.back().c_str());
   std::ofstream fkStream;
   fkStream.open(fkFileName.back());

   // Get the rotation matrix
   Rmatrix33 rotMat = imagerInUse->GetRotationMatrix();
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



   if (hostObj->GetType() == Gmat::SPACECRAFT)
   {
      //Complete once Spacecraft CK and FK are implemented
      /*
      reportCoordinates = "SpacecraftOrigin";
      // Generate the sensor frame
      if (reportCoordinates == "SpacecraftOrigin")
      {
         fkStream << "\\begindata\n";
         fkStream << "NAIF_BODY_NAME += ('" << imagerInUse->GetName() << "')\n";
         fkStream << "NAIF_BODY_CODE += (" << frameIdStr << ")\n";
         fkStream << "FRAME_" << frameName << " = " << frameIdStr << "\n";
         fkStream << "FRAME_" << frameIdStr << "_NAME = '" << frameName << "'\n";
         fkStream << "FRAME_" << frameIdStr << "_CLASS = 4\n";
         fkStream << "FRAME_" << frameIdStr << "_CLASS_ID = " << frameIdStr << "\n";
         //This will NOT WORK until CK and FK are added to the Spacecraft
         fkStream << "FRAME_" << frameIdStr << "_CENTER = " << GmatStringUtil::ToString(hostObj->GetIntegerParameter("NAIFId")) << "\n\n";
         fkStream << "OBJECT_" << frameIdStr << "_FRAME = '" << frameName << "'\n\n";
         fkStream << "TKFRAME_" << frameIdStr << "_RELATIVE = '" << hostObj->GetStringParameter("SpiceFrameId") << "'\n";
         fkStream << "TKFRAME_" << frameIdStr << "_SPEC = 'MATRIX'\n";
         fkStream << "TKFRAME_" << frameIdStr << "_MATRIX = (" << rotMatColumn << ")\n\n";

      }

      */

   }
   else //Build FK for Imager
   {


      fkStream << "\\begindata\n";
      fkStream << "NAIF_BODY_NAME += ('" << imagerInUse->GetName() << "')\n";
      fkStream << "NAIF_BODY_CODE += (" << frameIdStr << ")\n";
      fkStream << "FRAME_" << frameName << " = " << frameIdStr << "\n";
      fkStream << "FRAME_" << frameIdStr << "_NAME = '" << frameName << "'\n";
      fkStream << "FRAME_" << frameIdStr << "_CLASS = 4\n";
      fkStream << "FRAME_" << frameIdStr << "_CLASS_ID = " << frameIdStr << "\n";
      fkStream << "FRAME_" << frameIdStr << "_CENTER = " << GmatStringUtil::ToString(hostObj->GetIntegerParameter("NAIFId")) << "\n\n";
      fkStream << "OBJECT_" << frameIdStr << "_FRAME = '" << frameName << "'\n\n";
      fkStream << "TKFRAME_" << frameIdStr << "_RELATIVE = '" << hostObj->GetStringParameter("SpiceFrameId") << "'\n";
      fkStream << "TKFRAME_" << frameIdStr << "_SPEC = 'MATRIX'\n";
      fkStream << "TKFRAME_" << frameIdStr << "_MATRIX = (" << rotMatColumn << ")\n\n";

   }
   fkStream.close();
   spice->LoadKernel(fkFileName.back());


   // Generate the Imager IK
   spiceInstNames.push_back("INS" + frameIdStr);

   ikStream << "\\begindata\n";
   if (fovShape == "POLYGON")
   {
      ikStream << "INS" << frameIdStr << "_FOV_CLASS_SPEC = 'CORNERS'\n";
   }
   else
   {
      ikStream << "INS" << frameIdStr << "_FOV_CLASS_SPEC = 'ANGLES'\n";
   }
   ikStream << "INS" << frameIdStr << "_FOV_SHAPE = '" << fovShape << "'\n";
   ikStream << "INS" << frameIdStr << "_FOV_FRAME = '" << frameName << "'\n";
   imagerInUse->GetDirection()[0];
   ikStream << "INS" << frameIdStr << "_BORESIGHT = ("<< GmatStringUtil::RealToString(imagerInUse->GetDirection()[0])<< " " << GmatStringUtil::RealToString(imagerInUse->GetDirection()[1]) << " " << GmatStringUtil::RealToString(imagerInUse->GetDirection()[2]) <<")\n";
   if (fovShape == "CIRCLE")
   {
      ikStream << "INS" << frameIdStr << "_FOV_REF_VECTOR = (" << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[0]) << " " << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[1]) << " " << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[2]) << ")\n";
      ikStream << "INS" << frameIdStr << "_FOV_REF_ANGLE = (" <<
         GmatStringUtil::ToString(theFOV->GetRealParameter("FieldOfViewAngle")) << ")\n";
      ikStream << "INS" << frameIdStr << "_FOV_ANGLE_UNITS = 'DEGREES'\n";
   }
   else if (fovShape == "RECTANGLE")
   {
      ikStream << "INS" << frameIdStr << "_FOV_REF_VECTOR = (" << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[0]) << " " << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[1]) << " " << GmatStringUtil::RealToString(imagerInUse->GetSecondDirection()[2]) << ")\n";
      ikStream << "INS" << frameIdStr << "_FOV_REF_ANGLE = " <<
         GmatStringUtil::ToString(theFOV->GetRealParameter("AngleHeight")) << "\n";
      ikStream << "INS" << frameIdStr << "_FOV_CROSS_ANGLE = " <<
         GmatStringUtil::ToString(theFOV->GetRealParameter("AngleWidth") / 2.0) << "\n";
      ikStream << "INS" << frameIdStr << "_FOV_ANGLE_UNITS = 'DEGREES'\n";
   }
   else if (fovShape == "POLYGON")
   {
      Rvector coneAngles = theFOV->GetRvectorParameter("ConeAngles");
      Rvector clockAngles = theFOV->GetRvectorParameter("ClockAngles");
      std::string lengthSpaces = "INS" + frameIdStr + "_FOV_BOUNDARY_CORNERS = ( ";
      lengthSpaces = std::string(" ",lengthSpaces.size());
      ikStream << "INS" << frameIdStr << "_FOV_BOUNDARY_CORNERS = ( ";
      for (Integer i = 0; i < coneAngles.GetSize(); i++)
      {
         Real az = GmatMathUtil::DegToRad(-clockAngles.GetElement(i));
         Real el = GmatMathUtil::DegToRad(coneAngles.GetElement(i));
         Real x = GmatMathUtil::Sin(el)*GmatMathUtil::Cos(az); // Normally this would be X, but X is north in SPICE, so we call this y
         Real y = GmatMathUtil::Sin(el)*GmatMathUtil::Sin(az); //
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

   // Close the files
   ikStream.close();

   spice->LoadKernel(ikFileName.back());

   return true;
}
