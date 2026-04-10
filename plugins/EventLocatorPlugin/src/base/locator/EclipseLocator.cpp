//$Id: EclipseLocator.cpp 2251 2012-04-03 23:16:37Z  $
//------------------------------------------------------------------------------
//                           EclipseLocator
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
// Created: Sep 2, 2011
// Updated: 2015  Wendy Shoan / GSFC
//
/**
 * Implementation of the EclipseLocator
 * Updates based on prototype by Yeerang Lim/KAIST
 */
//------------------------------------------------------------------------------

#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "EclipseLocator.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"
#include "EphemManager.hpp"
#include "EclipseEvent.hpp"
#include "StringUtil.hpp"


//#define DEBUG_TYPELIST
//#define DEBUG_LOCATOR_DESTRUCTOR
//#define DEBUG_EVENT_INITIALIZATION
//#define DEBUG_ECLIPSE_EVENTS
//#define DEBUG_ECLIPSE_INIT
//#define DEBUG_ECLIPSE_FIND_EVENTS
//#define DEBUG_ECLIPSE_FIND_EVENTS_COUNT
//#define DEBUG_ECLIPSE_ACTION
//#define DEBUG_ECLIPSE_LOCATOR_WRITE
//#define DEBUG_ECLIPSE_SET
//#define DEBUG_TIME_SPENT
//#define DEBUG_NESTED_EVENTS

#ifdef DEBUG_TIME_SPENT
#include <time.h>
#endif

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string EclipseLocator::PARAMETER_TEXT[
      EclipseLocatorParamCount - EventLocatorParamCount] =
{
      "EclipseTypes",           // ECLIPSE_TYPES
};

const Gmat::ParameterType EclipseLocator::PARAMETER_TYPE[
      EclipseLocatorParamCount - EventLocatorParamCount] =
{
   Gmat::STRINGARRAY_TYPE     // OCCULTERS
};

//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EclipseLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the EclipseLocator
 */
//------------------------------------------------------------------------------
EclipseLocator::EclipseLocator(const std::string &name) :
   EventLocator         ("EclipseLocator", name),
   maxIndex             (-1),
   maxDuration          (-1.0)
{
   objectTypeNames.push_back("EclipseLocator");
   parameterCount = EclipseLocatorParamCount;

   defaultEclipseTypes.push_back("Umbra");
   defaultEclipseTypes.push_back("Penumbra");
   defaultEclipseTypes.push_back("Antumbra");

   TakeAction("Clear", "Events");

   // Set default occulting bodies
   defaultOccultingBodies.push_back("Earth");
   defaultOccultingBodies.push_back("Luna");

   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p>\n",
            name.c_str(), this);
   #endif
}

//------------------------------------------------------------------------------
// ~EclipseLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EclipseLocator::~EclipseLocator()
{
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Deleting EclipseLocator at <%p>\n", this);
   #endif
   TakeAction("Clear", "Events");
   defaultEclipseTypes.clear();
}

//------------------------------------------------------------------------------
// EclipseLocator(const EclipseLocator & el)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param el The EclipseLocator being copied
 */
//------------------------------------------------------------------------------
EclipseLocator::EclipseLocator(const EclipseLocator & el) :
   EventLocator         (el),
//   eclipseTypes         (el.eclipseTypes),
   sun                  (NULL),
   maxIndex             (el.maxIndex),
   maxDuration          (el.maxDuration)
{
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p> "
            "using the Copy constructor\n", instanceName.c_str(), this);
   #endif
   defaultEclipseTypes.clear();
   // copy the list of default eclipse types
   for (unsigned int i = 0; i < (el.defaultEclipseTypes).size(); i++)
      defaultEclipseTypes.push_back((el.defaultEclipseTypes).at(i));

   // Eclipse Types
   eclipseTypes.clear();
   for (Integer jj = 0; jj < (el.eclipseTypes).size(); jj++)
      eclipseTypes.push_back(el.eclipseTypes.at(jj));
   // Events
   TakeAction("Clear", "Events");
   EclipseTotalEvent *toCopy   = NULL;
   EclipseTotalEvent *newEvent = NULL;
   for (Integer ii = 0; ii < el.theEvents.size(); ii++)
   {
      toCopy   = el.theEvents.at(ii);
      newEvent = new EclipseTotalEvent(*toCopy);
      theEvents.push_back(newEvent);
   }

   isInitialized = false;
}

//------------------------------------------------------------------------------
// EclipseLocator& operator=(const EclipseLocator & el)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param el The EclipseLocator providing data for this one
 *
 * @return this instance, configured to match el
 */
//------------------------------------------------------------------------------
EclipseLocator& EclipseLocator::operator=(const EclipseLocator & el)
{
   if (this != &el)
   {
      EventLocator::operator=(el);

      eclipseTypes = el.eclipseTypes;
      sun          = NULL;
      maxIndex     = el.maxIndex;
      maxDuration  = el.maxDuration;

      defaultEclipseTypes.clear();
      // copy the list of default eclipse types
      for (unsigned int i = 0; i < (el.defaultEclipseTypes).size(); i++)
         defaultEclipseTypes.push_back((el.defaultEclipseTypes).at(i));

      eclipseTypes.clear();
      // copy the list of eclipse types
      for (Integer jj = 0; jj < (el.eclipseTypes).size(); jj++)
         eclipseTypes.push_back(el.eclipseTypes.at(jj));

      // copy the events
      TakeAction("Clear", "Events");
      EclipseTotalEvent *toCopy   = NULL;
      EclipseTotalEvent *newEvent = NULL;
      for (Integer ii = 0; ii < el.theEvents.size(); ii++)
      {
         toCopy   = el.theEvents.at(ii);
         newEvent = new EclipseTotalEvent(*toCopy);
         theEvents.push_back(newEvent);
      }

      isInitialized = false;
   }

   return *this;
}

// Inherited (GmatBase) methods for parameters

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
std::string EclipseLocator::GetParameterText(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < EclipseLocatorParamCount)
      return PARAMETER_TEXT[id - EventLocatorParamCount];
   return EventLocator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter
 *
 * @param str The script string for the parameter
 *
 * @return The parameter's id
 */
//------------------------------------------------------------------------------
Integer EclipseLocator::GetParameterID(const std::string &str) const
{
   for (Integer i = EventLocatorParamCount; i < EclipseLocatorParamCount; i++)
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
Gmat::ParameterType EclipseLocator::GetParameterType(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < EclipseLocatorParamCount)
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
std::string EclipseLocator::GetParameterTypeString(const Integer id) const
{
   return EventLocator::PARAM_TYPE_STRING[GetParameterType(id)];
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
const StringArray& EclipseLocator::GetPropertyEnumStrings(
                                   const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case ECLIPSE_TYPES:
      enumStrings.clear();
      enumStrings.push_back("Umbra");
      enumStrings.push_back("Penumbra");
      enumStrings.push_back("Antumbra");
      return enumStrings;
   default:
      return EventLocator::GetPropertyEnumStrings(id);
   }
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
std::string EclipseLocator::GetStringParameter(const Integer id) const
{
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
bool EclipseLocator::SetStringParameter(const Integer id,
                                        const std::string &value)
{
   if (id == ECLIPSE_TYPES)
   {
      if ((value != "Umbra") && (value != "Penumbra") && (value != "Antumbra"))
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EclipseTypes",
                       "1 or more of [Umbra, Penumbra, Antumbra]");
         throw ee;
      }
      if (find(eclipseTypes.begin(), eclipseTypes.end(), value) ==
          eclipseTypes.end())
      {
         eclipseTypes.push_back(value);
      }
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
std::string EclipseLocator::GetStringParameter(const Integer id,
                                               const Integer index) const
{
   if (id == ECLIPSE_TYPES)
   {
      if ((index >= 0) && (index < (Integer)eclipseTypes.size()))
         return eclipseTypes.at(index);
      else
         throw EventException(
               "Index out of range when trying to access eclipse type list "
               "for " + instanceName);
   }

   return EventLocator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//                         const Integer index)
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
bool EclipseLocator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   #ifdef DEBUG_ECLIPSE_SET
      MessageInterface::ShowMessage(
            "Entering EclipseLocator::SetStringParameter (index) ... ");
      MessageInterface::ShowMessage(
            "id = %d, value = %s, index = %d\n", id, value.c_str(), index);
   #endif
   if (id == ECLIPSE_TYPES)
   {
      if (index < 0)
      {
         std::string errmsg = "Eclipse type(s) must be set for an Eclipse ";
         errmsg += "Locator.  ";
         errmsg += "Empty braces are not allowed.\n";
         throw EventException(errmsg);
      }
      if ((value != "Umbra") && (value != "Penumbra") && (value != "Antumbra"))
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EclipseTypes",
                       "1 or more of [Umbra, Penumbra, Antumbra]");
         throw ee;
      }
      if (index < (Integer) eclipseTypes.size())
      {
         eclipseTypes.at(index) = value;
         return true;
      }
      else
      {
         eclipseTypes.push_back(value);
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
const StringArray& EclipseLocator::GetStringArrayParameter(const Integer id) const
{
   if (id == ECLIPSE_TYPES)
   {
      //If there have been eclipse types set, return those names
      // Otherwise, return the default set of eclipse types
      if (eclipseTypes.empty()) return defaultEclipseTypes;
      return eclipseTypes;
   }

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
const StringArray& EclipseLocator::GetStringArrayParameter(const Integer id,
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
std::string EclipseLocator::GetStringParameter(const std::string &label) const
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
bool EclipseLocator::SetStringParameter(const std::string &label,
      const std::string &value)
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
std::string EclipseLocator::GetStringParameter(const std::string &label,
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
bool EclipseLocator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
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
const StringArray& EclipseLocator::GetStringArrayParameter(
      const std::string &label) const
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
const StringArray& EclipseLocator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
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
bool EclipseLocator::TakeAction(const std::string &action,
                                const std::string &actionData)
{
   #ifdef DEBUG_ECLIPSE_ACTION
      MessageInterface::ShowMessage(
                    "Entering EclipseLocator::TakeAction with action = %s and "
                    "actionData = %s\n",
            action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "EclipseTypes") || (actionData == ""))
      {
         eclipseTypes.clear();
         retval = true;
      }
      else if ((actionData == "Events") || (actionData == ""))
      {
      #ifdef DEBUG_ECLIPSE_ACTION
         MessageInterface::ShowMessage(
               "In EclipseLocator::TakeAction, about to clear %d events\n",
               (Integer) theEvents.size());
      #endif
         for (Integer ii = 0; ii < theEvents.size(); ii++)
         {
            theEvents.at(ii)->TakeAction("Clear", "Events");
            delete theEvents.at(ii);
         }
         theEvents.clear();
         retval = true;
      }

      return (EventLocator::TakeAction(action, actionData) || retval);
   }

   return EventLocator::TakeAction(action, actionData);
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
const ObjectTypeArray& EclipseLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();
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
const ObjectTypeArray& EclipseLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
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
GmatBase *EclipseLocator::Clone() const
{
   return new EclipseLocator(*this);
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
void EclipseLocator::Copy(const GmatBase* orig)
{
   operator=(*((EclipseLocator *)(orig)));
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
bool EclipseLocator::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_ECLIPSE_INIT
      MessageInterface::ShowMessage("Initializing %s\n", instanceName.c_str());
   #endif

   if (solarSys)
   {
      sun = (Star*) solarSys->GetBody(GmatSolarSystemDefaults::SUN_NAME);
   }
   if (eclipseTypes.size() < 1)
   {
      // Use default list here
      for (Integer ii = 0; ii < 3; ii++)
         eclipseTypes.push_back(defaultEclipseTypes[ii]);
   }

   // NOW initialize the base class
   retval = EventLocator::Initialize();

   SetLocatingString("EclipseLocator");

   return retval;
}

//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
bool EclipseLocator::ReportEventData(const std::string &reportNotice)
{
   #ifdef DEBUG_ECLIPSE_LOCATOR_WRITE
      MessageInterface::ShowMessage("EclipseLocator::ReportEventData ... \n");
   #endif

   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
      // TBD - throw an exception or just continue without writing?
      return false;
   }

   std::string    itsName   = sat->GetName();
   theReport << "Spacecraft: " << itsName << "\n\n";

   Integer sz = (Integer) theEvents.size();
   if (sz == 0)
   {
      theReport << GetNoEventsString("eclipse") << "\n";
   }
   else
   {
      theReport << "Start Time (UTC)            Stop Time (UTC)               Duration (s)    ";
      theReport << "Occ Body        Type        Event Number  Total Duration (s)\n";

      // Loop over the total events list
      for (Integer ii = 0; ii < sz; ii++)
      {
         EclipseTotalEvent* ev = theEvents.at(ii);
         std::string eventString = ev->GetReportString();
         theReport << eventString; //  << "\n";
      }
      Integer numIndividual = 0;
      for (unsigned int jj = 0; jj < sz; jj++)
         numIndividual += theEvents.at(jj)->NumberOfEvents();

      theReport << "\nNumber of individual events : " << numIndividual << "\n";
      theReport << "Number of total events      : "   << sz            << "\n";
      theReport << "Maximum duration (s)        : "   << GmatStringUtil::BuildNumber(maxDuration, false, 14)   << "\n";
      theReport << "Maximum duration at the "         <<
                   GmatStringUtil::ToOrdinal(maxIndex + 1) << " eclipse.\n\n\n";
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
void EclipseLocator::FindEvents()
{
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage(
                        "Entering EclipseLocator::FindEvents with these "
                        "occultingBodies:\n");
      if (!occultingBodies.empty())
      {
         for (Integer ii = 0; ii < occultingBodies.size(); ii++)
            MessageInterface::ShowMessage("   %s\n", (occultingBodies.at(ii))->GetName().c_str());
      }
      else
      {
         MessageInterface::ShowMessage("No occulting bodies set!!!!\n");
      }
      MessageInterface::ShowMessage("and these eclipseTypes:\n");
      for (Integer ii = 0; ii < eclipseTypes.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", eclipseTypes.at(ii).c_str());
   #endif

   #ifdef DEBUG_TIME_SPENT
   clock_t t = clock();
   #endif

   // Clear old events
   TakeAction("Clear", "Events");

   // Set up data for the calls to CSPICE
   std::string      theFront  = "";  // we will loop over occultingBodies
   std::string      theFShape = "ELLIPSOID";
   std::string      theFFrame = ""; // we will loop over occultingBodies
   std::string      theBack   = "SUN";
   std::string      theBShape = "ELLIPSOID";
   std::string      theBFrame = "IAU_SUN";
   std::string      theAbCorr = GetAbcorrString();
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage("---- theAbCorr  = %s\n",
                                    theAbCorr.c_str());
   #endif

   EclipseTotalEvent  *rawList = new EclipseTotalEvent();
   Integer            numEclipse = 0;
   RealArray          starts;
   RealArray          ends;
   std::string        bodyName;

   #ifdef DEBUG_TIME_SPENT
   t = clock();
   #endif
   for (Integer ii = 0; ii < occultingBodies.size(); ii++)
   {
      CelestialBody *body = (CelestialBody*) occultingBodies.at(ii);
      Integer bodyNaifId  = body->GetIntegerParameter(body->GetParameterID(
                                                      "NAIFId"));
      theFront  = GmatStringUtil::Trim(GmatStringUtil::ToString(bodyNaifId));
      bodyName  = body->GetName();
      theFFrame = body->GetStringParameter(
                        body->GetParameterID("SpiceFrameId"));

      for (Integer jj = 0; jj < eclipseTypes.size(); jj++)
      {
         starts.clear();
         ends.clear();

         em->GetOccultationIntervals(eclipseTypes.at(jj), theFront, theFShape,
                                     theFFrame, theBack, theBShape, theBFrame,
                                     theAbCorr, initialEp, finalEp,
                                     useEntireInterval, stepSize,
                                     numEclipse, starts, ends);

         #ifdef DEBUG_ECLIPSE_FIND_EVENTS
//            MessageInterface::ShowMessage("After gfoclt_c:\n");
//            MessageInterface::ShowMessage("  numEclipse = %d\n", numEclipse);
         #endif
         // Create an event from the result
         for (Integer kk = 0; kk < numEclipse; kk++)
         {
            Real s1 = starts.at(kk);
            Real e1 = ends.at(kk);
            EclipseEvent *newEvent = new EclipseEvent(s1, e1,
                                         eclipseTypes.at(jj), bodyName);
            rawList->AddEvent(newEvent);
         }
      }
   }
   #ifdef DEBUG_TIME_SPENT
   timeSpent = (Real) (clock() - t);
   MessageInterface::ShowMessage(
                     " --- TOTAL time spent in getting occultation intervals "
                     "= %12.10f (sec)\n",
         (timeSpent / CLOCKS_PER_SEC));
   #endif
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage("rawList has been set up ...\n");
   #endif

   Integer numEventsInTotal = rawList->NumberOfEvents();
   if (numEventsInTotal == 0)
   {
      delete rawList;
      return;
   }

//   // Clear old events
//   TakeAction("Clear", "Events");
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage(
                        "Events have been cleared ... numEventsInTotal = %d\n",
                        numEventsInTotal);
   #endif

   #ifdef DEBUG_TIME_SPENT
   t = clock();
   #endif
   // Rearrange the events into the proper start time order
   EclipseEvent *a, *b;
   // @todo Check this algorithm for correctness and performance
   for (Integer yy = 0; yy < numEventsInTotal; yy++)
   {
      for (Integer zz = 0; zz < numEventsInTotal; zz++)
      {
         a = rawList->GetEvent(yy);
         b = rawList->GetEvent(zz);
         if (a->GetStart() < b->GetStart())
         {
            // switch these
            rawList->SetEvent(yy, b);
            rawList->SetEvent(zz, a);
         }
      }
   }
   a = NULL;
   b = NULL;
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage("Events have been ordered\n");
   #endif
   #ifdef DEBUG_TIME_SPENT
   timeSpent = (Real) (clock() - t);
   MessageInterface::ShowMessage(
                     " --- time spent in ordering the raw list of events "
                     "= %12.10f (sec)\n",
         (timeSpent / CLOCKS_PER_SEC));
   t = clock();
   #endif


   Integer currentIndex = 0;
   EclipseTotalEvent *currentTotal = NULL;
   EclipseEvent* currentEvent = rawList->GetEvent(0);

   EclipseTotalEvent *aTotal = new EclipseTotalEvent();
   aTotal->AddEvent(currentEvent);
   aTotal->SetStart(currentEvent->GetStart());
   aTotal->SetEnd(currentEvent->GetEnd());
   aTotal->SetIndex(currentIndex);
   theEvents.push_back(aTotal); // first one on the list, at position 0
   #ifdef DEBUG_ECLIPSE_FIND_EVENTS
      MessageInterface::ShowMessage("First total event has been set up\n");
   #endif
   
   for (Integer qq = 1; qq < numEventsInTotal; qq++)
   {
      currentEvent  = rawList->GetEvent(qq);
      Real itsStart = currentEvent->GetStart();
      Real itsEnd   = currentEvent->GetEnd();
      std::string bodyName = currentEvent->GetBodyName();
      #ifdef DEBUG_ECLIPSE_FIND_EVENTS_COUNT
         MessageInterface::ShowMessage(
                           "------------------------------------\n");
         MessageInterface::ShowMessage("FindEvents:: Event info:\n");
         MessageInterface::ShowMessage("   start: %12.10f\n", itsStart);
         MessageInterface::ShowMessage("   end:   %12.10f\n", itsEnd);
         MessageInterface::ShowMessage("   body:  %s\n", bodyName.c_str());
      #endif
      if (itsStart > (theEvents.at(currentIndex))->GetEnd())
      {
         #ifdef DEBUG_ECLIPSE_FIND_EVENTS_COUNT
            MessageInterface::ShowMessage(
                              "--- (1) setting currentIndex to %d and "
                              "closing out totalEvent\n",
                              (currentIndex+1));
         #endif
         // Check for nested event(s) in the current one, which would need to
         // be added after the last/current one and before we start a new one
         std::vector<EclipseTotalEvent*> nested = GetNestedEvents(currentIndex,
                                                  theEvents.at(currentIndex));
         #ifdef DEBUG_ECLIPSE_FIND_EVENTS_COUNT
            MessageInterface::ShowMessage("SIZE OF NESTED = %d\n",
                                          (Integer) nested.size());
         #endif
         if (!nested.empty())
         {
            // Remove the original event from the list
            theEvents.pop_back();  // Is this correct?
            currentIndex--;
            // add the nested events created from the original event
            for (Integer ii = 0; ii < nested.size(); ii++)
            {
               #ifdef DEBUG_NESTED_EVENTS
               MessageInterface::ShowMessage(" ++++ nested(%d):\n", ii);
               Integer ne = nested.at(ii)->NumberOfEvents();
               for (Integer qq = 0; qq < ne; qq++)
               {
                  EclipseEvent *ee = nested.at(ii)->GetEvent(qq);
                  MessageInterface::ShowMessage("   start: %12.10f\n",
                                                ee->GetStart());
                  MessageInterface::ShowMessage("   end:   %12.10f\n",
                                                ee->GetEnd());
                  MessageInterface::ShowMessage("   body:  %s\n",
                                                ee->GetBodyName().c_str());
               }
               #endif
               theEvents.push_back(nested.at(ii));
               currentIndex++;
            }
         }
         // Now, start the next one
         currentIndex++;
         currentTotal = new EclipseTotalEvent();
         currentTotal->AddEvent(currentEvent);
         currentTotal->SetStart(itsStart);
         currentTotal->SetEnd(itsEnd);
         currentTotal->SetIndex(currentIndex);
         theEvents.push_back(currentTotal);
      }

      else
      {
         #ifdef DEBUG_ECLIPSE_FIND_EVENTS_COUNT
            MessageInterface::ShowMessage(
                              "--- (2) adding event to current ...\n");
         #endif
         if (itsEnd > (theEvents.at(currentIndex))->GetEnd())
         {
            theEvents.at(currentIndex)->SetEnd(itsEnd);
         }
         theEvents.at(currentIndex)->AddEvent(currentEvent);
      }
   }

   #ifdef DEBUG_TIME_SPENT
   timeSpent = (Real) (clock() - t);
   MessageInterface::ShowMessage(
                     " --- time spent in creating lists of umbra/penumbra "
                     "= %12.10f (sec)\n",
         (timeSpent / CLOCKS_PER_SEC));
   #endif

   // Compute the maximum duration of the events
   maxIndex    = -1;
   maxDuration = -1.0;
   for (Integer rr = 0; rr < theEvents.size(); rr++)
   {
      Real itsTotalDuration = theEvents.at(rr)->GetDuration();
      if (itsTotalDuration > maxDuration)
      {
         maxDuration = itsTotalDuration;
         maxIndex = rr;
      }
   }
//   delete rawList;
}

//------------------------------------------------------------------------------
// std::vector<EclipseTotalEvent*> GetNestedEvents(Integer idx,
//                                                 EclipseTotalEvent *inEvent)
//------------------------------------------------------------------------------
/**
 * Find and pull out any nested eclipse events included in the input
 * EclipseTotalEvent.
 */
//------------------------------------------------------------------------------
std::vector<EclipseTotalEvent*> EclipseLocator::GetNestedEvents(Integer idx,
                                                EclipseTotalEvent *inEvent)
{
   #ifdef DEBUG_NESTED_EVENTS
         MessageInterface::ShowMessage(
                           "--- CALLING GetNestedEvent with index %d\n", idx);
         MessageInterface::ShowMessage("    input events: \n");
         for (Integer ii = 0; ii < inEvent->NumberOfEvents(); ii++)
         {
            EclipseEvent *e = inEvent->GetEvent(ii);
            MessageInterface::ShowMessage("    (%d) %s   %12.10f   %12.10f\n",
                     ii, e->GetBodyName().c_str(), e->GetStart(), e->GetEnd());
         }
   #endif
   
   StringArray                     bodyNames;
   std::vector<EclipseTotalEvent*> eventList;
   
   // How many individual events are there?
   Integer numEvents   = inEvent->NumberOfEvents();
   
   //' If there are zero or one events in this total event, just return
   if ((numEvents == 0) || (numEvents == 1))
      return eventList;   // return empty list
   
   // We will put the nested events, by occulting body, in the order that they
   // appear in this event
   // Start with the first event
   EclipseEvent *event0   = inEvent->GetEvent(0);
   std::string firstBody  = event0->GetBodyName();
   Real        firstStart = event0->GetStart();
   Real        firstEnd   = event0->GetEnd();
   
   // if we only have one body in this event, just return
   bool multiBody = false;
   for (Integer mm = 1; mm < numEvents; mm++)
   {
      if ((inEvent->GetEvent(mm))->GetBodyName() != firstBody)
      {
         multiBody = true;
         break;
      }
   }
   if (!multiBody)
      return eventList;  // return empty list
   
   // Keep track of the index numbers, stat with the current one
   Integer currentIdx = idx;

   // Find any nested events in the input one, by looking for the
   // occulting body names of each EclipseEvent
   EclipseTotalEvent *firstEvent = new EclipseTotalEvent();
   firstEvent->AddEvent(event0);
   firstEvent->SetStart(firstStart);
   firstEvent->SetEnd(firstEnd); // may be updated by later event
   firstEvent->SetIndex(currentIdx++);

   // Add this first one to the event and body name arrays
   eventList.push_back(firstEvent);
   bodyNames.push_back(firstBody);
   
   // Info we will obtain for each event in the total event list
   EclipseEvent  *eachEvent;
   std::string   eventBody = "";
   Real          itsStart = 0.0;
   Real          itsEnd   = 0.0;

   // See if any of the events have a different occulting body; if so,
   // create a new total event
   for (Integer ii = 1; ii < numEvents; ii++)
   {
      eachEvent  = inEvent->GetEvent(ii);
      eventBody  = eachEvent->GetBodyName();
      itsStart   = eachEvent->GetStart();
      itsEnd     = eachEvent->GetEnd();
      
      // Find out if we've already seen this body name
      // (Indexing into eventList and bodyNames is the same)
      bool found = false;
      for (Integer nn = 0; nn < eventList.size(); nn++)
      {
         if (eventBody == bodyNames.at(nn))
         {
            // BUT if there is more than one event for this body, they may
            // be separate events - in that case, don't add it to the
            // current event  (a second difference?)
            Real lastEnd = eventList.at(nn)->GetEnd();
            if (GmatMathUtil::Abs(itsStart - lastEnd) <= 1.0e-7)
            {
               found = true;
               eventList.at(nn)->AddEvent(eachEvent);
               #ifdef DEBUG_NESTED_EVENTS
                     MessageInterface::ShowMessage(
                                       "---***--- Added event for body %s with end time %12.10f\n", eventBody.c_str(), itsEnd);
               #endif
               if (itsEnd > lastEnd)
               {
                  (eventList.at(nn))->SetEnd(itsEnd);
               }
               break;
            }
         }
      }
      if (!found)
      {
         // Add a new name
         bodyNames.push_back(eventBody);
         // Add a new eclipseTotalEvent
         EclipseTotalEvent *newEvent = new EclipseTotalEvent();
         newEvent->AddEvent(eachEvent);
         newEvent->SetStart(itsStart);
         newEvent->SetEnd(itsEnd); // may be updated by later event
         newEvent->SetIndex(currentIdx++);
         eventList.push_back(newEvent);
         #ifdef DEBUG_NESTED_EVENTS
               MessageInterface::ShowMessage(
                                 "---***--- CREATED new event for body %s with end time %12.10f\n", eventBody.c_str(), itsEnd);
         #endif
      }
   }
   
   return eventList;
}
