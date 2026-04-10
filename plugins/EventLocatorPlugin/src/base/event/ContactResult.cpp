//$Id$
//------------------------------------------------------------------------------
//                               ContactResult
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
// Author: Wendy C. Shoan
// Created: 2015.04.24
//
/**
 * Implementation for the ContactResult base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include "ContactResult.hpp"
#include "EventException.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

ContactResult::ContactResult() :
   LocatedEvent  (),
   numEvents     (0)
{
   theEvents.clear();
}

ContactResult::~ContactResult()
{
   TakeAction("Clear");
}

ContactResult::ContactResult(const ContactResult& copy) :
   LocatedEvent  (copy)
{
   TakeAction("Clear");
   ContactEvent *toCopy   = NULL;
   ContactEvent *newEvent = NULL;
   for (Integer ii = 0; ii < copy.theEvents.size(); ii++)
   {
      toCopy   = copy.theEvents.at(ii);
      newEvent = new ContactEvent(*toCopy);
      theEvents.push_back(newEvent);
   }
}

ContactResult& ContactResult::operator=(const ContactResult& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      TakeAction("Clear");
      ContactEvent *toCopy   = NULL;
      ContactEvent *newEvent = NULL;
      for (Integer ii = 0; ii < copy.theEvents.size(); ii++)
      {
         toCopy   = copy.theEvents.at(ii);
         newEvent = new ContactEvent(*toCopy);
         theEvents.push_back(newEvent);
      }
   }

   return *this;
}

bool ContactResult::AddEvent(ContactEvent* newEvent)
{
   // #todo check here first to make sure the event is not
   // already in the list
   theEvents.push_back(newEvent);
   numEvents++;
   return true;
}

Integer ContactResult::NumberOfEvents()
{
   return theEvents.size();
}

void ContactResult::SetNoEvents(const std::string &noEv)
{
   noEvents = noEv;
}

void ContactResult::SetObserverName(const std::string &itsName)
{
   observerName = itsName;
}


Real ContactResult::GetDuration()
{
   // recompute start and end here from the events in the list <<<< TBD
   // for now, events are ordered as they are put into the list and
   // start and end are set by FindEvents
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}


ContactEvent* ContactResult::GetEvent(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for ContactResult.\n");

   return theEvents.at(atIndex);
}

std::string ContactResult::GetReportString()
{
   std::stringstream totalString("");
   Integer sz = theEvents.size();

   totalString << "Observer: " << observerName << "\n";

   if (sz > 0)
   {
      totalString << "Start Time (UTC)            Stop Time (UTC)";
      totalString << "               Duration (s)         ";
      totalString << "\n";

      // Loop over the total events list
      for (Integer ii = 0; ii < sz; ii++)
      {
         ContactEvent* ev          = theEvents.at(ii);
         std::string   eventString = ev->GetReportStringLegacy();
         totalString <<  eventString << "    \n";
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
bool ContactResult::TakeAction(const std::string &action,
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

