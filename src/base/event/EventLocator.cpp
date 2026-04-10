//$Id$
//------------------------------------------------------------------------------
//                           EventLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Implementation of the the event locator base class
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>

#include "EventLocator.hpp"
#include "EventException.hpp"
#include "Spacecraft.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "FileUtil.hpp"
#include "GmatConstants.hpp"
#include "StringUtil.hpp"
#include "TimeSystemConverter.hpp"
#include "GregorianDate.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "EphemManager.hpp"


//#define DEBUG_DUMPEVENTDATA
//#define DEBUG_LOCATE_EVENTS
//#define DEBUG_EVENT_INITIALIZATION
//#define DEBUG_OBJECTS
//#define DEBUG_DATE_FORMAT
//#define DEBUG_EVENTLOCATOR_OBJECT
//#define DEBUG_EVENTLOCATOR_WRITE
//#define DEBUG_EVENTLOCATOR_DATA
//#define DEBUG_EVENTLOCATOR_SET
//#define DEBUG_EVENTLOCATOR_GET
//#define DEBUG_EVENTLOCATOR_COPY
//#define DEBUG_EVENT_EPOCH_STRING

#ifdef DEBUG_DUMPEVENTDATA
   #include <fstream>
   std::ofstream dumpfile("LocatorData.txt");
#endif

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
EventLocator::PARAMETER_TEXT[EventLocatorParamCount - GmatBaseParamCount] =
{
   "Spacecraft",           // SATNAME
   "Filename",             // EVENT_FILENAME
   "OccultingBodies",      // OCCULTING_BODIES
   "InputEpochFormat",     // INPUT_EPOCH_FORMAT
   "InitialEpoch",         // INITIAL_EPOCH
   "StepSize",             // STEP_SIZE
   "FinalEpoch",           // FINAL_EPOCH
   "UseLightTimeDelay",    // USE_LIGHT_TIME_DELAY
   "UseStellarAberration", // USE_STELLAR_ABERRATION
   "WriteReport",          // WRITE_REPORT
   "RunMode",              // RUN_MODE
   "UseEntireInterval",    // USE_ENTIRE_INTERVAL
};

const Gmat::ParameterType
EventLocator::PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,      // SATNAME
   Gmat::FILENAME_TYPE,    // EVENT_FILENAME
   Gmat::OBJECTARRAY_TYPE, // OCCULTING_BODIES
   Gmat::STRING_TYPE,      // INPUT_EPOCH_FORMAT
   Gmat::STRING_TYPE,      // INITIAL_EPOCH
   Gmat::REAL_TYPE,        // STEP_SIZE
   Gmat::STRING_TYPE,      // FINAL_EPOCH
   Gmat::BOOLEAN_TYPE,     // USE_LIGHT_TIME_DELAY
   Gmat::BOOLEAN_TYPE,     // USE_STELLAR_ABERRATION
   Gmat::BOOLEAN_TYPE,     // WRITE_REPORT
   Gmat::ENUMERATION_TYPE, // RUN_MODE
   Gmat::BOOLEAN_TYPE,     // USE_ENTIRE_INTERVAL
};

const std::string EventLocator::RUN_MODES[3] =
{
      "Automatic",
      "Manual",
      "Disabled",
};

const Integer EventLocator::numModes = 3;

const std::string EventLocator::defaultFormat        = "TAIModJulian";
const Real        EventLocator::defaultInitialEpoch  = 21545;
const Real        EventLocator::defaultFinalEpoch    = 21545.138;

// Used for light-time calculations
const Real EventLocator::STEP_MULTIPLE = 0.5;

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// EventLocator(const std::string &typeStr, const std::string &nomme) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr Type name of the locator
 * @param nomme Name of this instance of the locator
 */
//------------------------------------------------------------------------------
EventLocator::EventLocator(const std::string &typeStr,
                           const std::string &nomme) :
   GmatBase                (Gmat::EVENT_LOCATOR, typeStr, nomme),
   filename                (""),
   fileWasWritten          (false),
   useLightTimeDelay       (true),
   useStellarAberration    (true),
   writeReport             (true),
   locatingString          (""),
   runMode                 ("Automatic"),
   useEntireInterval       (true),
   appendReport            (false),
   epochFormat             ("TAIModJulian"),
   initialEpoch            ("21545"),        // MUST match initialEp
   finalEpoch              ("21545.138"),    // MUST match finalEp
   stepSize                (10.0),
   initialEp               (defaultInitialEpoch),
   finalEp                 (defaultFinalEpoch),
   fromEpoch               (0.0),
   toEpoch                 (0.0),
   findStart               (0.0),
   findStop                (0.0),
   scStart                 (0.0),
   scNow                   (0.0),
   targetName              (""),
   sat                     (NULL),
   region                  (NULL),
   solarSys                (NULL),
   em                      (NULL),
   initialEpochSet         (false),
   finalEpochSet           (false)
{
   objectTypes.push_back(Gmat::EVENT_LOCATOR);
   objectTypeNames.push_back("EventLocator");

   filename = instanceName + ".txt";  // default filename

   blockCommandModeAssignment = false;

   occultingBodyNames.clear();
   occultingBodies.clear();
   defaultOccultingBodies.clear();
   // default occulting bodies are set in the leaf classes

   theTimeConverter = TimeSystemConverter::Instance();
}


//------------------------------------------------------------------------------
// ~EventLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventLocator::~EventLocator()
{
   #ifdef DEBUG_EVENT_INITIALIZATION
      MessageInterface::ShowMessage("Deleting event locator %s <%p>\n",
            instanceName.c_str(), this);
   #endif

   if (!occultingBodyNames.empty())
      occultingBodyNames.clear();
   if (!occultingBodies.empty())
      occultingBodies.clear();
   defaultOccultingBodies.clear();
}


//------------------------------------------------------------------------------
// EventLocator(const EventLocator& el)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param el The locator being copied here
 */
//------------------------------------------------------------------------------
EventLocator::EventLocator(const EventLocator& el) :
   GmatBase                (el),
   filename                (el.filename),
   fileWasWritten          (false),
   useLightTimeDelay       (el.useLightTimeDelay),
   useStellarAberration    (el.useStellarAberration),
   writeReport             (el.writeReport),
   runMode                 (el.runMode),
   locatingString          (el.locatingString),
   useEntireInterval       (el.useEntireInterval),
   appendReport            (el.appendReport),
   epochFormat             (el.epochFormat),
   initialEpoch            (el.initialEpoch),
   finalEpoch              (el.finalEpoch),
   stepSize                (el.stepSize),
   initialEp               (el.initialEp),
   finalEp                 (el.finalEp),
   fromEpoch               (el.fromEpoch),
   toEpoch                 (el.toEpoch),
   findStart               (el.findStart),
   findStop                (el.findStop),
   scStart                 (el.scStart),
   scNow                   (el.scNow),
   targetName              (el.targetName),
   sat                     (NULL),
   region                  (NULL),
   solarSys                (el.solarSys),
   em                      (NULL),
   initialEpochSet         (el.initialEpochSet),
   finalEpochSet           (el.finalEpochSet)
{
   occultingBodyNames.clear();
   occultingBodies.clear();
   defaultOccultingBodies.clear();

   UnsignedInt sz = el.occultingBodyNames.size();
   for (UnsignedInt ii = 0; ii < sz; ii++)
      occultingBodyNames.push_back(el.occultingBodyNames.at(ii));
   // copy the list of default occulting body names
   for (unsigned int i = 0; i < (el.defaultOccultingBodies).size(); i++)
   {
      defaultOccultingBodies.push_back((el.defaultOccultingBodies).at(i));
   }

   theTimeConverter = TimeSystemConverter::Instance();
   isInitialized    = false;
}


//------------------------------------------------------------------------------
// EventLocator& EventLocator::operator=(const EventLocator& el)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param el The locator being copied here
 *
 * @return this instance, set to match el.
 */
//------------------------------------------------------------------------------
EventLocator& EventLocator::operator=(const EventLocator& el)
{
   if (this != &el)
   {
      GmatBase::operator =(el);

      filename             = el.filename;
      fileWasWritten       = el.fileWasWritten;

      useLightTimeDelay    = el.useLightTimeDelay;
      useStellarAberration = el.useStellarAberration;
      writeReport          = el.writeReport;
      runMode              = el.runMode;
      locatingString       = el.locatingString;
      useEntireInterval    = el.useEntireInterval;
      appendReport         = el.appendReport;
      epochFormat          = el.epochFormat;
      initialEpoch         = el.initialEpoch;
      finalEpoch           = el.finalEpoch;
      stepSize             = el.stepSize;
      initialEp            = el.initialEp;
      finalEp              = el.finalEp;
      fromEpoch            = el.fromEpoch;
      toEpoch              = el.toEpoch;
      findStart            = el.findStart;
      findStop             = el.findStop;
      scStart              = el.scStart;
      scNow                = el.scNow;
      targetName           = el.targetName;
      sat                  = el.sat;     // NULL;
      region               = el.region;
      solarSys             = el.solarSys;
      em                   = NULL;
      initialEpochSet      = el.initialEpochSet;
      finalEpochSet        = el.finalEpochSet;

      occultingBodyNames.clear();
      occultingBodies.clear();
      defaultOccultingBodies.clear();
      #ifdef DEBUG_EVENTLOCATOR_SET
      MessageInterface::ShowMessage("In EventLocator::operator=, current body names:\n");
      for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
         MessageInterface::ShowMessage("    %s\n", occultingBodyNames.at(ii).c_str());
      MessageInterface::ShowMessage("In EventLocator::operator=, body names of input are:\n");
      for (Integer ii = 0; ii < (el.occultingBodyNames).size(); ii++)
         MessageInterface::ShowMessage("    %s\n", (el.occultingBodyNames.at(ii)).c_str());
      #endif
      UnsignedInt sz = el.occultingBodyNames.size();
      for (UnsignedInt ii = 0; ii < sz; ii++)
         occultingBodyNames.push_back(el.occultingBodyNames.at(ii));

      sz = el.occultingBodies.size();
      for (UnsignedInt ii = 0; ii < sz; ii++)
         occultingBodies.push_back(el.occultingBodies.at(ii));

      // copy the list of default occulting body names
      for (unsigned int i = 0; i < (el.defaultOccultingBodies).size(); i++)
      {
         defaultOccultingBodies.push_back((el.defaultOccultingBodies).at(i));
      }
}

   return *this;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text description of the object parameter with input id
 *
 * @param id The ID for the parameter
 *
 * @return The string used to script the parameter
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter ID associated with a string
 *
 * @param str The string associated with the ID
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer EventLocator::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < EventLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type of a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EventLocator::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description of the type of a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type description
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if the parameter is read only or writable when saved
 *
 * @param id The ID for the parameter
 *
 * @return true if the parameter is not writable, false if it is
 */
//------------------------------------------------------------------------------
bool EventLocator::IsParameterReadOnly(const Integer id) const
{

   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Determines if the parameter is read only or writable when saved
 *
 * @param label The string description of the parameter
 *
 * @return true if the parameter is not writable, false if it is
 */
//------------------------------------------------------------------------------
bool EventLocator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool EventLocator::IsParameterCommandModeSettable(const Integer id) const
{
   if ((id == EVENT_FILENAME)      || (id == INPUT_EPOCH_FORMAT)  ||
       (id == INITIAL_EPOCH)       || (id == FINAL_EPOCH)         ||
       (id == USE_ENTIRE_INTERVAL) || (id == WRITE_REPORT)          )
      return true;

   return false;
}



//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const Integer id) const
{
   if (id == STEP_SIZE)
      return stepSize;

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const Integer id, const Real value)
{
   if (id == STEP_SIZE)
   {
      if (value > 0.0)
      {
         stepSize = value;
         return stepSize;
      }
      else
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
               "StepSize", "Real number > 0");
         throw ee;
      }
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const Integer id) const
{
   if (id == SATNAME)
      return targetName;
   if (id == EVENT_FILENAME)
      return filename;
   if (id == INPUT_EPOCH_FORMAT)
      return epochFormat;
   if (id == INITIAL_EPOCH)
   {
//      return initialEpoch;
      return GetEpochString("INITIAL", epochFormat);
   }
   if (id == FINAL_EPOCH)
   {
//      return finalEpoch;
      return GetEpochString("FINAL", epochFormat);
   }
   if (id == RUN_MODE)
      return runMode;

   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const Integer id,
      const std::string &value)
{
   if (id == SATNAME)
   {
      if (value != "")
      {
         targetName = value;
         return true;
      }
      return false;
   }
   if (id == EVENT_FILENAME)
   {
      if (value != "")
      {
         filename = value;
         return true;
      }
      return false;
   }
   if (id == INPUT_EPOCH_FORMAT)
   {
      if (theTimeConverter->IsValidTimeSystem(value))
      {
         epochFormat = value;
         return true;
      }
      else
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EpochFormat", "Valid Time Format");
         throw ee;
      }
   }
   if ((id == INITIAL_EPOCH) || (id == FINAL_EPOCH))
   {
      SetEpoch(value, id);
      return true;
   }

   if (id == RUN_MODE)
   {
      for (Integer jj = 0; jj < numModes; jj++)
      {
         if (GmatStringUtil::ToUpper(value) == GmatStringUtil::ToUpper(RUN_MODES[jj]))
         {
            runMode = value;
            return true;
         }
      }
      EventException ee("");
      std::string allowed = "One of ";
      for (Integer jj = 0; jj < numModes; jj++)
      {
         allowed += RUN_MODES[jj];
         if (jj != (numModes -1))
            allowed += ", ";
      }
      ee.SetDetails(errorMessageFormat.c_str(), value.c_str(),
            "RunMode", allowed.c_str());
      throw ee;
   }
   if (id == OCCULTING_BODIES)
   {
      #ifdef DEBUG_EVENTLOCATOR_SET
         MessageInterface::ShowMessage("About to set occulting body on EventLocator %s\n",
               instanceName.c_str());
         MessageInterface::ShowMessage("Current occulting bodies list:\n");
         for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
            MessageInterface::ShowMessage("   %s\n", occultingBodyNames.at(ii).c_str());
      #endif
      if (value != targetName)
      {
         if (value != "Sun")
         {
            if (find(occultingBodyNames.begin(), occultingBodyNames.end(),
                  value) == occultingBodyNames.end())
            occultingBodyNames.push_back(value);
         }
         else
         {
            MessageInterface::ShowMessage("The Sun was set as an occulting "
            "body, but Stars cannot occult their own light.  The Sun is being "
            "excluded from the list of occulters.\n");
            return false;
         }
      }
      #ifdef DEBUG_EVENTLOCATOR_SET
         MessageInterface::ShowMessage("And THEN occulting bodies list:\n");
         for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
            MessageInterface::ShowMessage("   %s\n", occultingBodyNames.at(ii).c_str());
      #endif
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter from a vector of strings
 *
 * @param id The ID of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const Integer id,
      const Integer index) const
{

   if (id == OCCULTING_BODIES)
   {
      if ((index >= 0) && (index < (Integer) occultingBodyNames.size()))
         return occultingBodyNames.at(index);
      else
         throw EventException(
               "Index out of range when trying to access occulting body name for " +
               instanceName);
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter in a vector of strings
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == OCCULTING_BODIES)
   {
      if (find(occultingBodyNames.begin(), occultingBodyNames.end(), value) == occultingBodyNames.end())
      {
         if (index < (Integer) occultingBodyNames.size())
         {
            occultingBodyNames.at(index) = value;
            return true;
         }
         else
         {
            occultingBodyNames.push_back(value);
            return true;
         }
      }
      else
      {
         // ignore duplicate occulting body names, for now
         return true;
//         std::string errmsg = "Occulting Body ";
//         errmsg += value + " is already in list for EventLocator ";
//         errmsg += instanceName + ".  Each body must be listed only once.\n";
//         throw EventException(errmsg);
      }
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_EVENTLOCATOR_GET
      MessageInterface::ShowMessage("In EL::GetStringArrayP, occulting bodies are:\n");
      for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
         MessageInterface::ShowMessage("    %s\n", occultingBodyNames.at(ii).c_str());
   #endif
   if (id == OCCULTING_BODIES)
   {
      if (occultingBodyNames.empty())
         return defaultOccultingBodies;
      return occultingBodyNames;
   }

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter from a vector of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the StringArray in the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(const Integer id,
                                             const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter from a vector of strings
 *
 * @param label The script string of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter in a vector of strings
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter from a vector of StringArrays
 *
 * @param label The script string of the parameter
 * @param index The index of the StringArray in the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
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
bool EventLocator::GetBooleanParameter(const Integer id) const
{
   if (id == USE_LIGHT_TIME_DELAY)
      return useLightTimeDelay;
   if (id == USE_STELLAR_ABERRATION)
      return useStellarAberration;
   if (id == WRITE_REPORT)
      return writeReport;
   if (id == USE_ENTIRE_INTERVAL)
      return useEntireInterval;

   return GmatBase::GetBooleanParameter(id);
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
bool EventLocator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_LIGHT_TIME_DELAY)
   {
      useLightTimeDelay = value;
      return useLightTimeDelay;
   }
   if (id == USE_STELLAR_ABERRATION)
   {
      useStellarAberration = value;
      return useStellarAberration;
   }
   if (id == WRITE_REPORT)
   {
      writeReport = value;
      return writeReport;
   }
   if (id == USE_ENTIRE_INTERVAL)
   {
      useEntireInterval = value;
      return useEntireInterval;
   }

   return GmatBase::SetBooleanParameter(id, value);
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
bool EventLocator::GetBooleanParameter(const std::string &label) const
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
bool EventLocator::SetBooleanParameter(const std::string &label,
      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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
bool EventLocator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   #ifdef DEBUG_EVENTLOCATOR_SET
      MessageInterface::ShowMessage("In EL::TakeAction, action = %s and actionData = %s\n",
            action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      if ((actionData == "OccultingBodies") || (actionData == ""))
      {
         occultingBodyNames.clear();
         occultingBodies.clear();
      }
      return true;
   }

   return GmatBase::TakeAction(action, actionData);
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
UnsignedInt EventLocator::GetPropertyObjectType(const Integer id) const
{
   #ifdef DEBUG_OBJECTS
      MessageInterface::ShowMessage(
            "EventLocator:: Asking now for the type of object %d\n", id);
   #endif
   switch (id)
   {
   case SATNAME:
      return Gmat::UNKNOWN_OBJECT;
   case OCCULTING_BODIES:
      return Gmat::CELESTIAL_BODY;
   case Gmat::REGION:
      return Gmat::REGION;
   default:
      return GmatBase::GetPropertyObjectType(id);
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
const StringArray& EventLocator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case RUN_MODE:
      enumStrings.clear();
      for (Integer ii = 0; ii < numModes; ii++)
         enumStrings.push_back(RUN_MODES[ii]);

      return enumStrings;
   default:
      return GmatBase::GetPropertyEnumStrings(id);
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
const ObjectTypeArray& EventLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();  // ??
   if (id == SATNAME)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::SPACECRAFT) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::SPACECRAFT);
      return listedTypes;
   }
   if (id == OCCULTING_BODIES)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::CELESTIAL_BODY) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::CELESTIAL_BODY);
      return listedTypes;
   }

   return GmatBase::GetTypesForList(id);
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
const ObjectTypeArray& EventLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The solar system
 */
//------------------------------------------------------------------------------
void EventLocator::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
}

//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool EventLocator::HasRefObjectTypeArray()
{
   return true;
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
const ObjectTypeArray& EventLocator::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   refObjectTypes.push_back(Gmat::CELESTIAL_BODY);
   refObjectTypes.push_back(Gmat::REGION);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  void SetEpoch(std::string ep)
//------------------------------------------------------------------------------
/**
 * Set the epoch.
 *
 * @param <ep> The new epoch.
 *
 * @return the epoch as an A1MJD Real
 */
//------------------------------------------------------------------------------
void EventLocator::SetEpoch(const std::string &ep, Integer id)
{
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Setting %s epoch (%d) for spacecraft %s to \"%s\"\n",
       (id == INITIAL_EPOCH? "Initial ":"Final "), id, instanceName.c_str(), ep.c_str());
   #endif

   std::string epochString = "";
   Real        epochAsReal = 0.0;
   std::string timeSystem;
   std::string timeFormat;
   theTimeConverter->GetTimeSystemAndFormat(epochFormat, timeSystem, timeFormat);
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("epochFormat = %s\n", epochFormat.c_str());
      MessageInterface::ShowMessage("timeSystem  = %s\n", timeSystem.c_str());
      MessageInterface::ShowMessage("timeFormat  = %s\n", timeFormat.c_str());
   #endif
   if (timeFormat == "ModJulian") // numeric - save and output without quotes
      epochString = GmatStringUtil::RemoveEnclosingString(ep, "'");
   else // "Gregorian" - not numeric - save and output with quotes
   {
//      if (!GmatStringUtil::IsEnclosedWith(ep, "'"))
//         epochString = GmatStringUtil::AddEnclosingString(ep, "'");
//      else
         epochString = ep;
   }

   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Calling EpochToReal with %s\n",
            epochString.c_str());
   #endif
   Real epochIn = -999.999;
   std::string noQuotes = GmatStringUtil::RemoveEnclosingString(epochString, "'");
   std::string outStr;
   theTimeConverter->Convert(epochFormat, epochIn, noQuotes,
                             "A1ModJulian", epochAsReal, outStr);
   if (id == INITIAL_EPOCH)
   {
      initialEpoch    = epochString;
      initialEp       = epochAsReal;
      initialEpochSet = true;
   }
   else  // FINAL_EPOCH
   {
      finalEpoch    = epochString;
      finalEp       = epochAsReal;
      finalEpochSet = true;
   }
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Setting epoch (A1Mjd) to %12.15f\n",
      epochAsReal);
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Setting epoch (as string) to %s\n",
            epochString.c_str());
   #endif
}

//------------------------------------------------------------------------------
// std::string GetEpochString(const std::string &whichOne  = "INITIAL",
//                            const std::string &outFormat = "UTCGregorian")
//------------------------------------------------------------------------------
std::string EventLocator::GetEpochString(const std::string &whichOne,
                                         const std::string &outFormat) const
{
#ifdef DEBUG_EVENT_EPOCH_STRING
   MessageInterface::ShowMessage("In GetEpochString, initialEpoch = %s\n",
         initialEpoch.c_str());
   MessageInterface::ShowMessage("In GetEpochString, finalEpoch = %s\n",
         finalEpoch.c_str());
#endif
   Real outMjd = -999.999;
   std::string outStr;
   std::string outputFormat = outFormat;
//   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?

   if (whichOne == "INITIAL")
   {
      if ((outFormat == defaultFormat) && GmatMathUtil::IsEqual(initialEp, defaultInitialEpoch))
         outStr =  initialEpoch;
      else
         theTimeConverter->Convert("A1ModJulian", initialEp, "",
                                     outputFormat, outMjd, outStr);
   }
   else if (whichOne == "FINAL")
   {
      if ((outFormat == defaultFormat) && GmatMathUtil::IsEqual(finalEp, defaultFinalEpoch))
         outStr =  finalEpoch;
      else
         theTimeConverter->Convert("A1ModJulian", finalEp, "",
                                     outputFormat, outMjd, outStr);
   }
   else
      return "";

   return outStr;
}

void EventLocator::SetAppend(bool appendIt)
{
   appendReport = appendIt;
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the reference object
 *
 * @param type The type of the requested object name; UNKNOWN_OBJECT for any
 *
 * @return Object name
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      return targetName;
   }
   return GmatBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference objects
 *
 * @param type The type of the requested objects; UNKNOWN_OBJECT for all
 *
 * @return A StringArray of object names
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();
   // Get ref. objects for requesting type from the parent class
   GmatBase::GetRefObjectNameArray(type);

   if (type == Gmat::UNKNOWN_OBJECT)
   {
      refObjectNames.push_back(targetName);

      #ifdef DEBUG_EVENTLOCATOR_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::GetRefObjectNameArray() this=<%p>'%s' returning %d "
             "ref. object names\n", this, GetName().c_str(),
             refObjectNames.size());
         for (UnsignedInt i=0; i<refObjectNames.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n",
                  refObjectNames[i].c_str());
      #endif
   }
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::CELESTIAL_BODY)
   {
      if (occultingBodyNames.empty())
         // Add ref. objects for requesting type from this class
         refObjectNames.insert(refObjectNames.begin(), defaultOccultingBodies.begin(),
               defaultOccultingBodies.end());
      else
         // Add ref. objects for requesting type from this class
         refObjectNames.insert(refObjectNames.begin(), occultingBodyNames.begin(),
               occultingBodyNames.end());

      #ifdef DEBUG_EVENTLOCATOR_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::GetRefObjectNameArray() this=<%p>'%s' returning %d "
             "ref. object names\n", this, GetName().c_str(),
             refObjectNames.size());
         for (UnsignedInt i=0; i<refObjectNames.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n",
                  refObjectNames[i].c_str());
      #endif
   }
   #ifdef DEBUG_EVENTLOCATOR_SET
      MessageInterface::ShowMessage("refObjectNames = \n");
      for (Integer ii = 0; ii < refObjectNames.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", refObjectNames.at(ii).c_str());
   #endif

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object on this object
 *
 * @param obj The reference object
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EventLocator::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                const std::string &name)
{
   if ((targetName == name) && obj->IsOfType(Gmat::REGION))
   {
      targetIsRegion = true;
      region = (PlanetographicRegion*) obj;
      return true;
   }
   if ((targetName == name) && (obj->IsOfType(Gmat::SPACECRAFT)))
   {
      targetIsRegion = false;
      sat = (Spacecraft*) obj;
      return true;
   }
   if (obj->IsOfType(Gmat::CELESTIAL_BODY))
   {
      // check to see if it's already in the list
      std::vector<CelestialBody*>::iterator pos =
         find(occultingBodies.begin(), occultingBodies.end(), obj);
      if (pos != occultingBodies.end())
      {
         #ifdef DEBUG_CP_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::SetRefObject() the body <%p> '%s' already exist, so "
             "returning true\n", (*pos), name.c_str());
         #endif
         return true;
      }

      // If ref object has the same name, reset it
      pos = occultingBodies.begin();
      std::string bodyName;
      bool bodyFound = false;
      while (pos != occultingBodies.end())
      {
         bodyName = (*pos)->GetName();
         if (bodyName == name)
         {
            #ifdef DEBUG_CP_OBJECT
            MessageInterface::ShowMessage
               ("EventLocator::SetRefObject() resetting the pointer of body '%s' <%p> to "
                "<%p>\n", bodyName.c_str(), (*pos), (CelestialBody*)obj);
            #endif

            (*pos) = (CelestialBody*)obj;
            bodyFound = true;
         }
         ++pos;
      }

      // If ref object not found, add it
      if (!bodyFound)
      {
         #ifdef DEBUG_CP_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::SetRefObject() this=<%p> '%s', adding <%p> '%s' "
             "to bodyList for object %s\n", this, GetName().c_str(), obj, name.c_str(),
             instanceName.c_str());
         #endif

         occultingBodies.push_back((CelestialBody*) obj);
      }

      return true;
   }
   return GmatBase::SetRefObject(obj, type, name);
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
bool EventLocator::RenameRefObject(const UnsignedInt type,
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
      case Gmat::SPACEOBJECT:
      case Gmat::UNKNOWN_OBJECT:
         if (targetName == oldName)
         {
            targetName = newName;
            retval = true;
         }
         break;
      case Gmat::CELESTIAL_BODY:
         for (UnsignedInt ii = 0; ii < occultingBodyNames.size(); ii++)
         {
            if (occultingBodyNames.at(ii) == oldName)
            {
               occultingBodyNames.at(ii) = newName;
               retval = true;
            }
         }
         for (UnsignedInt ii = 0; ii < defaultOccultingBodies.size(); ii++)
         {
            if (defaultOccultingBodies.at(ii) == oldName)
            {
               defaultOccultingBodies.at(ii) = newName;
               retval = true;
            }
         }
         break;

      default:
         ;        // Intentional drop-through
   }
   
   return retval;
}


////------------------------------------------------------------------------------
//// Integer GetOwnedObjectCount()
////------------------------------------------------------------------------------
///**
// * Retrieves the number of owned objects in the event locator
// *
// * @return The owned object count
// */
////------------------------------------------------------------------------------
//Integer EventLocator::GetOwnedObjectCount()
//{
//   return 1 + eventTable.GetOwnedObjectCount();
//}
//
//
////------------------------------------------------------------------------------
//// GmatBase* GetOwnedObject(Integer whichOne)
////------------------------------------------------------------------------------
///**
// * Retrieves an owned objects by index
// *
// * @param whichOne The index of the owned object
// *
// * @return The owned object
// */
////------------------------------------------------------------------------------
//GmatBase* EventLocator::GetOwnedObject(Integer whichOne)
//{
//   GmatBase *retval = NULL;
//   if (whichOne == 0)
//      retval = &eventTable;
//   else
//      retval = eventTable.GetOwnedObject(whichOne - 1);
//
//   return retval;
//}
//

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the locator for use
 *
 * @return true if initialization succeeds, false on failure
 */
//------------------------------------------------------------------------------
bool EventLocator::Initialize()
{
   bool retval = true;

   #ifdef DEBUG_EVENT_INITIALIZATION
      MessageInterface::ShowMessage("Initializing event locator at <%p>\n", this);
      MessageInterface::ShowMessage("    Initial epoch is \"%s\"\n", initialEpoch.c_str());
      MessageInterface::ShowMessage("    Final   epoch is \"%s\"\n", finalEpoch.c_str());
   #endif

////      // Make sure the epochs are set and Real epochs computed
////   SetEpoch(initialEpoch, INITIAL_EPOCH);
////   SetEpoch(finalEpoch,   FINAL_EPOCH);
////   #ifdef DEBUG_EVENT_INITIALIZATION
////      MessageInterface::ShowMessage("Epochs Initialized!!!!\n");
////   #endif
//
//   // Validate inputs here
//   if (initialEp > finalEp)
//   {
//      std::string errmsg = "Initial Epoch must be earlier than Final Epoch for EventLocator ";
//      errmsg += instanceName + ".\n";
//      throw EventException(errmsg);
//   }
//
//   fromEpoch = initialEp;
//   toEpoch   = finalEp;

   if (!sat && !region)
   {
      std::string errmsg = "Target for EventLocator ";
      errmsg += instanceName + " must be set to either a Spacecraft or a PlanetographicRegion.\n";
      throw EventException(errmsg);
   }
   if (!targetIsRegion)
   {
      if (!sat->IsOfType("Spacecraft"))
      {
         std::string errmsg = "Target must be of type Spacecraft or PlanetographicRegion for EventLocator ";
         errmsg += instanceName + ".\n";
         throw EventException(errmsg);
      }
   }
   else if (!region->IsOfType("PlanetographicRegion"))
   {
      std::string errmsg = "Target must be of type Spacecraft or PlanetographicRegion for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }
   else if (!sat)
   {
      std::string errmsg = "Observer for EventLocator " + instanceName + " must be set as a Spacecraft when the target is a PlanetographicRegion.\n";
      throw EventException(errmsg);
   }
   
   Integer satNAIFId = sat->GetIntegerParameter("NAIFId");
   if (satNAIFId >= 0)
   {
      std::string errmsg = "Error in EventLocator::Initialize, "
         "the spacecraft NAIF ID for spacecraft \"" + sat->GetName() +
         "\" must be negative in order to properly associate it with "
         "the generated SCLK file for event detection.\n";
      throw EventException(errmsg);
   }

   if (filename == "")
   {
      std::string errmsg = "No filename specified for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }
   // StellarAberration can only be set if LightTimeCorrection is also set
   if (useStellarAberration && !useLightTimeDelay)
   {
      std::string errmsg = "UseLightTimeDelay must be set to true ";
      errmsg += "when UseStellarAberration is set to true for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }

   /// Get the occulting bodies from the solar system??? OR put them on the ref object list ??  TBD
   // @todo <<<<<<<<<<  Currently have them in the ref object list
   if (occultingBodyNames.size() < 1)
   {
      // Use default list here
      for (Integer ii = 0; ii < defaultOccultingBodies.size(); ii++)
         occultingBodyNames.push_back(defaultOccultingBodies[ii]);
   }

   if (occultingBodyNames.size() > occultingBodies.size())
   {
      std::string errmsg = "Specified occulting bodies not set for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }

//   // For now, call this method here to load the planetary PCKs
//   #ifdef __USE_SPICE__
//      solarSys->LoadPCKs();
//   #endif

   #ifdef DEBUG_EVENT_INITIALIZATION
      MessageInterface::ShowMessage(">>> About to tell spacecraft to record its data ...\n");
   #endif
   if (runMode != "Disabled")
   {
      // Tell the spacecraft to start recording its data
      sat->RecordEphemerisData();
   }

   fileWasWritten = false;
   isInitialized  = true;

   scStart = sat->GetEpoch();

   return retval;
}


//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 * NOTE: this may be able to be an abstract method, when it is implemented in
 * all of the derived classes (unless there are commonalities that can
 * be handled here) *******
 */
//------------------------------------------------------------------------------
bool EventLocator::ReportEventData(const std::string &reportNotice)
{
   // renameFile set to false here because renaming does not work on Windows
   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return false;
   }

   // Write the data here
   theReport << "Here is the report for the EventLocator ";
   theReport << instanceName << "!!!!!!\n";

   theReport.close();
   return true;
}

//------------------------------------------------------------------------------
// void LocateEvents(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Locates events in the selected time range
 *
 * @param appendToFile flag specifying whether to append the data to the
 *                     file (true) or erase the current file and start
 *                     over (false)
 * @note  this method assumes that the appendReport flag has been set
 *        before this call (using SetAppend method)
 *
 */
//------------------------------------------------------------------------------
void EventLocator::LocateEvents(const std::string &reportNotice)
{
   #ifdef DEBUG_EVENTLOCATOR_DATA
      MessageInterface::ShowMessage("In EL::LocateEvents, about to call ProvideEphemerisData\n");
   #endif

   if (runMode != "Disabled")
   {
      // First, need to validate initial and final epochs here ...
      //      // Make sure the epochs are set and Real epochs computed
      //   SetEpoch(initialEpoch, INITIAL_EPOCH);
      //   SetEpoch(finalEpoch,   FINAL_EPOCH);
      //   #ifdef DEBUG_EVENT_INITIALIZATION
      //      MessageInterface::ShowMessage("Epochs Initialized!!!!\n");
      //   #endif

      /// Get the ephem manager from the spacecraft, if we don't have it already
      if (!em)
      {
         em  = sat->GetEphemManager();
         if (!em)
         {
            std::string errmsg = "ERROR - no EphemManager available for spacecraft ";
            errmsg += sat->GetName() + "!!\n";
            throw EventException(errmsg);
         }
      }

      // Stop the data recording so that the kernel will be loaded
      sat->ProvideEphemerisData();

      Real coverageBegin;
      Real coverageEnd;
      scNow = sat->GetEpoch();
      em->GetCoverage(initialEp, finalEp, useEntireInterval, true,
                      findStart, findStop, coverageBegin, coverageEnd);
      #ifdef DEBUG_TIME_SPENT
      Real timeSpent = (Real) (clock() - t);
      MessageInterface::ShowMessage(" --- time spent in GetCoverage = %12.10f (sec)\n",
            (timeSpent / CLOCKS_PER_SEC));
      #endif
      #ifdef DEBUG_LOCATE_EVENTS
         MessageInterface::ShowMessage("---- findStart (from ephemManager)  = %12.10f\n", findStart);
         MessageInterface::ShowMessage("---- findStop (from ephemManager)   = %12.10f\n", findStop );
      #endif
      if (GmatMathUtil::IsEqual(findStart,0.0) && GmatMathUtil::IsEqual(findStop,0.0))
      {
         // ... in case there were no files to read from, we'll just use the
         // beginning and current spacecraft times
         findStart     = scStart;
         findStop      = scNow;
         coverageBegin = scStart;
         coverageEnd   = scNow;
      }
      #ifdef DEBUG_LOCATE_EVENTS
      MessageInterface::ShowMessage("---- findStart      = %12.10f\n", findStart);
      MessageInterface::ShowMessage("---- findStop       = %12.10f\n", findStop );
      MessageInterface::ShowMessage("---- coverageBegin  = %12.10f\n", coverageBegin);
      MessageInterface::ShowMessage("---- coverageEnd    = %12.10f\n", coverageEnd );
      MessageInterface::ShowMessage("---- initialEp      = %12.10f\n", initialEp);
      MessageInterface::ShowMessage("---- finalEp        = %12.10f\n", finalEp );
      #endif

      // Validate inputs here
      if (initialEp > finalEp)
      {
         std::string errmsg = "Initial Epoch must be earlier than Final Epoch for EventLocator ";
         errmsg += instanceName + ".\n";
         throw EventException(errmsg);
      }
      // Validate initialEpoch and finalEpoch against available range of data (coverageBegin:coverageEnd)
      Real oneMillisecond = (1.0 / GmatTimeConstants::SECS_PER_DAY) / 1000.00;
      if (!useEntireInterval)
      {
         if (initialEpochSet && (initialEp < (coverageBegin - oneMillisecond)))
         {
            std::string errmsg = "Initial Epoch must be after the beginning of ";
            errmsg += "coverage for spacecraft ";
            errmsg += sat->GetName() + ", for EventLocator ";
            errmsg += instanceName + ".\n";
            throw EventException(errmsg);
         }
         if (initialEpochSet && (initialEp > (coverageEnd + oneMillisecond)))
         {
            std::string errmsg = "Initial Epoch must be before the end of ";
            errmsg += "coverage for spacecraft ";
            errmsg += sat->GetName() + ", for EventLocator ";
            errmsg += instanceName + ".\n";
            throw EventException(errmsg);
         }
         if (finalEpochSet && (finalEp < (coverageBegin - oneMillisecond)))
         {
            std::string errmsg = "Final Epoch must be after the beginning of ";
            errmsg += "coverage for spacecraft ";
            errmsg += sat->GetName() + ", for EventLocator ";
            errmsg += instanceName + ".\n";
            throw EventException(errmsg);
         }
         if (finalEpochSet && (finalEp > (coverageEnd + oneMillisecond)))
         {
            std::string errmsg = "Final Epoch must be before the end of ";
            errmsg += "coverage for spacecraft ";
            errmsg += sat->GetName() + ", for EventLocator ";
            errmsg += instanceName + ".\n";
            throw EventException(errmsg);
         }
      }

      fromEpoch = initialEp;
      toEpoch   = finalEp;
//
//      // Stop the data recording so that the kernel will be loaded
//      sat->ProvideEphemerisData();

      // write the 'running'message each time location is performed
      MessageInterface::ShowMessage(locatingString);

      // Locate events in derived class and store them as you have decided to do so
      FindEvents();
      #ifdef DEBUG_EVENTLOCATOR_DATA
         MessageInterface::ShowMessage("In EL::LocateEvents, after FindEvents.  About to write the report (%s) ...\n",
               (writeReport? "true":"false"));
      #endif

      // Write the report
      // appendReport should already have been set by a call to SetAppend
      if (writeReport)
      {
         bool wasOK = ReportEventData(reportNotice);
         #ifdef DEBUG_EVENTLOCATOR_DATA
            MessageInterface::ShowMessage("In EL::LocateEvents, after ReportEventData (wasOK = %s) ...\n",
                  (wasOK? "true":"false"));
         #endif
         if (wasOK) fileWasWritten = true;
      }
   }
   #ifdef DEBUG_EVENTLOCATOR_DATA
      MessageInterface::ShowMessage("LEAVING EL::LocateEvents ...\n");
   #endif
}

//------------------------------------------------------------------------------
// bool FileWasWritten()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating if the EventLocation data was written to a file
 *
 * @return The fileWasWritten flag.
 */
//------------------------------------------------------------------------------
bool EventLocator::FileWasWritten()
{
   return fileWasWritten;
}

//------------------------------------------------------------------------------
// bool IsInAutomaticMode()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating if the EventLocation run mode is "Automatic"
 *
 * @return The flag.
 */
//------------------------------------------------------------------------------
bool EventLocator::IsInAutomaticMode()
{
   return (runMode == "Automatic");
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Real EpochToReal(const std::string &ep)
//------------------------------------------------------------------------------
Real EventLocator::EpochToReal(const std::string &ep)
{
   Real fromMjd = -999.999;
   Real outMjd = -999.999;
   std::string outStr;

   // remove enclosing quotes for the validation and conversion
   std::string epNoQuote = GmatStringUtil::RemoveEnclosingString(ep, "'");

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("EventLocator::EpochToReal() Converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (epochFormat.find("Gregorian") != std::string::npos)
   {
      if (!GregorianDate::IsValid(epNoQuote))
      {
         std::string errmsg = "EventLocator error: epoch ";
         errmsg += ep + " is not a valid Gregorian date.\n";
         throw EventException(errmsg);
      }
   }

   theTimeConverter->Convert(epochFormat, fromMjd, epNoQuote, "A1ModJulian", outMjd,
                              outStr);
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("EventLocator::EpochToReal() Done converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (outMjd == -999.999)
   {
      #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("EventLocator::EpochToReal() oops!  outMjd = -999.999!!\n");
      #endif
   }
   return outMjd;
}

//------------------------------------------------------------------------------
//  bool OpenReportFile(bool renameOld = true)
//------------------------------------------------------------------------------
bool EventLocator::OpenReportFile(bool renameOld)
{
   std::string fullFileName;

   if ((filename.find('/', 0) == std::string::npos) &&
       (filename.find('\\', 0) == std::string::npos))
   {
      FileManager *fm = FileManager::Instance();
      std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH);

      // Check for terminating '/' and add if needed
      Integer len = outPath.length();
      if ((outPath.c_str()[len-1] != '/') && (outPath.c_str()[len-1] != '\\'))
         outPath = outPath + "/";

      fullFileName = outPath + filename;
   }
   else
      fullFileName = filename;

   std::string withoutExt = "";
   // Get the file extension (with prepended '.')
   std::string fileExt = GmatFileUtil::ParseFileExtension(fullFileName, true);
   if (fileExt == "")
   {
      withoutExt = fullFileName;
   }
   else
   {
      std::string::size_type lastDot = fullFileName.find_last_of(".");
      withoutExt = fullFileName.substr(0, lastDot);
   }

   bool fileDoesExist = GmatFileUtil::DoesFileExist(fullFileName);

   #ifdef DEBUG_EVENTLOCATOR_WRITE
      MessageInterface::ShowMessage("In ReportEventData, fullFileName  = %s\n", fullFileName.c_str());
      MessageInterface::ShowMessage("                    withoutExt    = %s\n", withoutExt.c_str());
      MessageInterface::ShowMessage("                    fileDoesExist = %s\n", (fileDoesExist? "true" : "false"));
   #endif

   if (appendReport)
      theReport.open(fullFileName.c_str(), std::fstream::out | std::fstream::app);
   else
   {
      // Rename any old existing files here if necessary
      if (renameOld) // renaming does NOT WORK on Windows, so we will overwrite all the time
      {
         if (fileDoesExist)
         {
            // Get the FileManager pointer
            FileManager *fm = FileManager::Instance();
            Integer     fileCounter = 0;
            bool        done        = false;
            std::stringstream fileRename("");
            Integer     retCode = 0;
            while (!done)
            {
               fileRename.str("");
               fileRename << withoutExt << "__" << fileCounter << fileExt;
               #ifdef DEBUG_EVENTLOCATOR_WRITE
                  MessageInterface::ShowMessage("------>>> Renaming %s to %s\n",
                        fullFileName.c_str(), fileRename.str().c_str());
               #endif
               if (fm->RenameFile(fullFileName, fileRename.str(), retCode))
               {
                  done = true;
               }
               else
               {
                  if (retCode == 0) // if no error from system, but not allowed to overwrite
                  {
      //               if (fileCounter < MAX_FILE_RENAMES) // no MAX for now
                        fileCounter++;
      //               else
      //               {
      //                  reset_c(); // reset failure flag in SPICE
      //                  std::string errmsg = "Error renaming existing Event Report file  \"";
      //                  errmsg += fullFileName + "\".  Maximum number of renames exceeded.\n";
      //                  throw UtilityException(errmsg);
      //               }
                  }
                  else
                  {
      //               reset_c(); // reset failure flag in SPICE
                     std::string errmsg =
                           "Unknown system error occurred when attempting to "
                           "rename existing Event Report file \"";
                     errmsg += fullFileName + "\".\n";
                     throw EventException(errmsg);
                  }
               }

            }
         }
      }
      else // if not renaming, if it already exists, delete it
      {
         if (fileDoesExist)
            remove(fullFileName.c_str());
      }
      theReport.open(fullFileName.c_str(), std::fstream::out);
   }

   if (!theReport.is_open())
   {
      std::string errmsg = "Error creating or opening report file ";
      errmsg += fullFileName + ", for object \"";
      errmsg += instanceName + "\".  No event data will be added to the file.\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, errmsg);
      return false;
   }
   return true;
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
std::string EventLocator::GetAbcorrString()
{
   std::string correction =  "NONE";
   if (useLightTimeDelay)
   {
      correction = "CN";
      if (useStellarAberration)
      {
         correction = correction + "+S";
      }
   }
   return correction;
}

CelestialBody* EventLocator::GetCelestialBody(const std::string &withName)
{
   Integer sz = (Integer) occultingBodies.size();
   for (Integer ii = 0; ii < sz; ii++)
      if (occultingBodies.at(ii)->GetName() == withName)
         return occultingBodies.at(ii);
   return NULL;
}

std::string EventLocator::GetNoEventsString(const std::string &forType)
{
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string fromGregorian, toGregorian;
   Real        resultMjd;
   std::string noEvents;


   theTimeConverter->Convert("A1ModJulian", findStart, "",
                              outputFormat, resultMjd, fromGregorian);
   theTimeConverter->Convert("A1ModJulian", findStop, "",
                              outputFormat, resultMjd, toGregorian);

   noEvents  = "There are no ";
   noEvents += forType;
   noEvents += " events in the time interval ";
   noEvents += fromGregorian + " to " + toGregorian;

   return noEvents;
}

void EventLocator::SetLocatingString(const std::string &forType)
{
   locatingString  = "Finding events for ";
   locatingString += forType + " ";
   locatingString += instanceName + " ...\n";
   locatingString += "Celestial body properties are provided by SPICE kernels.\n";
}

