//$Id$
//------------------------------------------------------------------------------
//                               IntrusionResult
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
// Created: July 16, 2021
//
/**
 * Implementation for the IntrusionResult base class.
 */
 //------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include "IntrusionResult.hpp"
#include "EventException.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IntrusionEvent()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
IntrusionResult::IntrusionResult() :
   LocatedEvent(),
   numEvents(0),
   scName(""),
   sensorName(""),
   coordType(""),
   coord1Name("X (deg)"),
   coord2Name("Y (deg)")
{
   theEvents.clear();
}

//------------------------------------------------------------------------------
// ~IntrusionEvent()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
IntrusionResult::~IntrusionResult()
{
   TakeAction("Clear");
}

//------------------------------------------------------------------------------
// IntrusionEvent(const IntrusionResult& copy)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
IntrusionResult::IntrusionResult(const IntrusionResult& copy) :
   LocatedEvent(copy),
   numEvents(copy.numEvents),
   scName(copy.scName),
   sensorName(copy.sensorName),
   coordType(copy.coordType),
   coord1Name(copy.coord1Name),
   coord2Name(copy.coord2Name)
{
   TakeAction("Clear");
   IntrusionEvent *toCopy = NULL;
   IntrusionEvent *newEvent = NULL;
   for (Integer ii = 0; ii < copy.theEvents.size(); ii++)
   {
      toCopy = copy.theEvents.at(ii);
      newEvent = new IntrusionEvent(*toCopy);
      theEvents.push_back(newEvent);
   }
}

//------------------------------------------------------------------------------
// IntrusionEvent(const IntrusionResult& copy)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
IntrusionResult& IntrusionResult::operator=(const IntrusionResult& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      numEvents = copy.numEvents;
      scName = copy.scName;
      sensorName = copy.sensorName;
      coordType = copy.coordType;
      coord1Name = copy.coord1Name;
      coord2Name = copy.coord2Name;

      TakeAction("Clear");
      IntrusionEvent *toCopy = NULL;
      IntrusionEvent *newEvent = NULL;
      for (Integer ii = 0; ii < copy.theEvents.size(); ii++)
      {
         toCopy = copy.theEvents.at(ii);
         newEvent = new IntrusionEvent(*toCopy);
         theEvents.push_back(newEvent);
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool AddEvent(IntrusionEvent* newEvent)
//------------------------------------------------------------------------------
/**
* Adds an intrusion total event to the overall list of intrusion events in 
* a mission.
*
* @return Boolean of whether or not adding the event was successful
*/
//------------------------------------------------------------------------------
bool IntrusionResult::AddEvent(IntrusionEvent* newEvent)
{
   // #todo check here first to make sure the event is not
   // already in the list
   theEvents.push_back(newEvent);
   numEvents++;
   return true;
}

//------------------------------------------------------------------------------
// Integer NumberOfTotalEvents()
//------------------------------------------------------------------------------
/**
* The number of total events (i.e. IntrusionEvent objects) currently stored in
* the overall event list.
*
* @return The number of total events
*/
//------------------------------------------------------------------------------
Integer IntrusionResult::NumberOfTotalEvents()
{
   Integer totalNumEvents = 0;
   if (theEvents.size() > 0)
   {
      ++totalNumEvents;
      for (Integer ii = 0; ii < theEvents.size() - 1; ++ii)
      {
         if (theEvents.at(ii)->GetIntrusionType() == "Occultation")
            continue;
         else if (theEvents.at(ii)->GetEnd() == theEvents.at(ii + 1)->GetStart() &&
            (theEvents.at(ii)->GetIntrusionType() == "Transit" ||
               theEvents.at(ii + 1)->GetIntrusionType() == "Transit"))
            continue;
         ++totalNumEvents;
      }
   }
   return totalNumEvents;
}

//------------------------------------------------------------------------------
// Integer NumberOfIndividualEvents()
//------------------------------------------------------------------------------
/**
* The number of individual events (i.e. each timestep a body is intruding)
* currently stored in the overall event list.
*
* @return The number of individual events
*/
//------------------------------------------------------------------------------
Integer IntrusionResult::NumberOfIndividualEvents()
{
   Integer numIndEvents = 0;
   for (Integer ii = 0; ii < theEvents.size(); ++ii)
   {
      numIndEvents += theEvents.at(ii)->GetNumIndividualEvents();
   }
   return numIndEvents;
}

//------------------------------------------------------------------------------
// void SetNoEvents(const std::string &noEv)
//------------------------------------------------------------------------------
/**
* Set the string that describes the case where no intrusion events are
* detected.
*
* @param noEv The no event description string
*/
//------------------------------------------------------------------------------
void IntrusionResult::SetNoEvents(const std::string &noEv)
{
   noEvents = noEv;
}

//------------------------------------------------------------------------------
// void SetSpacecraftName(const std::string &nameOfSC)
//------------------------------------------------------------------------------
/**
* Set the name of the spacecraft with the sensor performing the intrusion 
* detection.
*
* @param nameOfSC The name of the spacecraft
*/
//------------------------------------------------------------------------------
void IntrusionResult::SetSpacecraftName(const std::string &nameOfSC)
{
   scName = nameOfSC;
}

//------------------------------------------------------------------------------
// void SetSensorName(const std::string &nameOfSensor)
//------------------------------------------------------------------------------
/**
* Set the name of the sensor performing the intrusion detection in these events.
*
* @param nameOfSC The name of the sensor
*/
//------------------------------------------------------------------------------
void IntrusionResult::SetSensorName(const std::string &nameOfSensor)
{
   sensorName = nameOfSensor;
}

//------------------------------------------------------------------------------
// void SetCoordinateType(const std::string &coordinateType)
//------------------------------------------------------------------------------
/**
* Set the coordinate type that the event data is being store in.
*
* @param coordinateType The coordinate system type
*/
//------------------------------------------------------------------------------
void IntrusionResult::SetCoordinateType(const std::string &coordinateType)
{
   coordType = coordinateType;
}

//------------------------------------------------------------------------------
// void SetCoordinateNames(const std::string &firstCoordName,
//                         const std::string &secondCoordName)
//------------------------------------------------------------------------------
/**
* Set the names of the coordinates used to override the generic x-coord and 
* y-coord names in the output report.
*
* @param coordinateType The reporting coordinate axis names
*/
//------------------------------------------------------------------------------
void IntrusionResult::SetCoordinateNames(const std::string &firstCoordName,
                                         const std::string &secondCoordName)
{
   coord1Name = firstCoordName + " (deg)";
   coord2Name = secondCoordName + " (deg)";
}

//------------------------------------------------------------------------------
// Real GetDuration()
//------------------------------------------------------------------------------
/**
* Get the time duration of all the events combined in seconds.
*
* @return The time duration in seconds
*/
//------------------------------------------------------------------------------
Real IntrusionResult::GetDuration()
{
   // recompute start and end here from the events in the list <<<< TBD
   // for now, events are ordered as they are put into the list and
   // start and end are set by FindEvents
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}

//------------------------------------------------------------------------------
// IntrusionEvent* GetEvent(Integer atIndex)
//------------------------------------------------------------------------------
/**
* Get an IntrusionEvent object (a single total event) at the selected index
*
* @return The IntrusionEvent pointer
*/
//------------------------------------------------------------------------------
IntrusionEvent* IntrusionResult::GetEvent(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for IntrusionResult.\n");

   return theEvents.at(atIndex);
}

//------------------------------------------------------------------------------
// std::string GetReportString()
//------------------------------------------------------------------------------
/**
* Get the overall report string of a set of total events with meta data listed.
*
* @return The report string
*/
//------------------------------------------------------------------------------
std::string IntrusionResult::GetReportString()
{
   std::stringstream totalString("");
   Integer sz = theEvents.size();
   
   // Check whether all information was provided

   totalString << "Spacecraft:   " << scName << "\n";
   totalString << "Sensor:       " << sensorName << "\n";
   totalString << "Coordinates:  " << coordType << "\n\n";

   if (sz > 0)
   {
      totalString << "Epoch (UTC)                 Body              Diameter(deg)";
      totalString << "     " << GmatStringUtil::PadWithBlanks(coord1Name, 17);
      totalString << GmatStringUtil::PadWithBlanks(coord2Name, 19);
      totalString << "Illumination    Event\n";

      // Loop over the total events list
      bool isNewSegment = true, writeOcc = false;
      for (Integer ii = 0; ii < sz; ii++)
      {
         // First check if this event to next event is continuous
         isNewSegment = true;
         if (ii < (sz - 1))
         {
            if (theEvents.at(ii)->GetEnd() == theEvents.at(ii + 1)->GetStart())
            {
               if (theEvents.at(ii)->GetIntrusionType() == "Intrusion")
                  theEvents.at(ii)->SetSkipLastLine(true);
               else if (theEvents.at(ii + 1)->GetIntrusionType() == "Intrusion")
                  theEvents.at(ii + 1)->SetSkipFirstLine(true);
               isNewSegment = false;
            }
         }

         IntrusionEvent* ev = theEvents.at(ii);
         std::string   eventString = ev->GetReportString();
         totalString << eventString;
         if (isNewSegment)
            totalString << "    \n";
         totalString.setf(std::ios::left);
         totalString.width(5);
      }
   }
   else
      totalString << noEvents << "\n";

   return totalString.str();
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
bool IntrusionResult::TakeAction(const std::string &action,
   const std::string &actionData)
{
   if (action == "Clear")
   {
      for (Integer ii = 0; ii < theEvents.size(); ii++)
      {
         delete theEvents.at(ii);
      }
      theEvents.clear();
   }
   return true;
}
