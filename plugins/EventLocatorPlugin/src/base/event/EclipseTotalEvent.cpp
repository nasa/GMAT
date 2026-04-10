//$Id$
//------------------------------------------------------------------------------
//                               EclipseTotalEvent
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
// Created: 2015.01.12
//
/**
 * Implementation for the EclipseTotalEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include "EclipseTotalEvent.hpp"
#include "EventException.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ECLIPSE_TOTAL_EVENT
//#define DEBUG_CLEAR_EVENTS
//#define DEBUG_ADD_ECLIPSE_EVENT

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

EclipseTotalEvent::EclipseTotalEvent() :
   LocatedEvent  (),
   numEvents     (0),
   theIndex      (-1)
{
}

EclipseTotalEvent::~EclipseTotalEvent()
{
   TakeAction("Clear");
}

EclipseTotalEvent::EclipseTotalEvent(const EclipseTotalEvent& copy) :
   LocatedEvent  (copy),
   theIndex      (copy.theIndex)
{
   TakeAction("Clear");
   EclipseEvent *toCopy   = NULL;
   EclipseEvent *newEvent = NULL;
   for (Integer ii = 0; ii < (copy.theEvents).size(); ii++)
   {
      toCopy   = copy.theEvents.at(ii);
      newEvent = new EclipseEvent(*toCopy);
      theEvents.push_back(newEvent);
   }
}

EclipseTotalEvent& EclipseTotalEvent::operator=(const EclipseTotalEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      theIndex      = copy.theIndex;

      TakeAction("Clear");
      EclipseEvent *toCopy   = NULL;
      EclipseEvent *newEvent = NULL;
      for (Integer ii = 0; ii < (copy.theEvents).size(); ii++)
      {
         toCopy   = copy.theEvents.at(ii);
         newEvent = new EclipseEvent(*toCopy);
         theEvents.push_back(newEvent);
      }
   }

   return *this;
}

bool EclipseTotalEvent::AddEvent(EclipseEvent* newEvent)
{
#ifdef DEBUG_ADD_ECLIPSE_EVENT
   MessageInterface::ShowMessage("Adding eclipse event to total:\n");
   MessageInterface::ShowMessage("   Body:       %s\n",      newEvent->GetBodyName().c_str());
   MessageInterface::ShowMessage("   Start:      %12.10f\n", newEvent->GetStart());
   MessageInterface::ShowMessage("   End:        %12.10f\n", newEvent->GetEnd());
   MessageInterface::ShowMessage("   Duration:   %12.10f\n", newEvent->GetDuration());
#endif
   // #todo check here first to make sure the event is not
   // already in the list
   theEvents.push_back(newEvent);
   numEvents++;
   return true;
}

bool EclipseTotalEvent::SetEvent(Integer atIndex, EclipseEvent *toEvent,
                                 bool deleteExisting)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for EclipseTotalEvent.\n");

   if (theEvents.at(atIndex) != toEvent)
   {
      // don't want to do this deletion in most cases
      if (deleteExisting) delete theEvents.at(atIndex);
      theEvents.at(atIndex) = toEvent;
   }
   return true;
}

Integer EclipseTotalEvent::NumberOfEvents()
{
   return theEvents.size();
}

Real EclipseTotalEvent::GetDuration()
{
   // recompute start and end here from the events in the list <<<< TBD
   // for now, events are ordered as they are put into the list and
   // start and end are set by FindEvents
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}


void EclipseTotalEvent::SetIndex(Integer i)
{
   theIndex = i;
}

EclipseEvent* EclipseTotalEvent::GetEvent(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for EclipseTotalEvent.\n");

   return theEvents.at(atIndex);
}

std::string EclipseTotalEvent::GetReportString()
{
   std::stringstream totalString("");

   Integer sz = theEvents.size();
   #ifdef DEBUG_ECLIPSE_TOTAL_EVENT
      MessageInterface::ShowMessage("In ETE::GetReportString, numEvents = %d\n",
            sz);
   #endif

   // Loop over the total events list
   for (Integer ii = 0; ii < sz; ii++)
   {
      EclipseEvent* ev = theEvents.at(ii);
      #ifdef DEBUG_ECLIPSE_TOTAL_EVENT
         MessageInterface::ShowMessage("In ETE::GetReportString, event %d is %sNULL\n",
               ii, (ev? "NOT " : ""));
      #endif
      std::string eventString = ev->GetReportString();
      Real        totalD      = GetDuration();
      totalString <<  eventString << "    ";
      totalString.setf(std::ios::left);
      totalString.width(5);
      totalString << (theIndex + 1)   << "       " <<
                     GmatStringUtil::BuildNumber(totalD, false, 14) << "\n";
   }
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
bool EclipseTotalEvent::TakeAction(const std::string &action,
                                   const std::string &actionData)
{
   if (action == "Clear")
   {
      #ifdef DEBUG_CLEAR_EVENTS
            MessageInterface::ShowMessage("Now clearing %d events in the EclipseTotalEvent\n",
                                         (Integer) theEvents.size());
      #endif
      for (Integer ii = 0; ii < theEvents.size(); ii++)
      {
         delete theEvents.at(ii);
      }
      theEvents.clear();
   }
   return true;
}

