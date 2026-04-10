//$Id: EventManager.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EventManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/09
//
/**
 * The event manager used to monitor events
 */
//------------------------------------------------------------------------------


#include "EventManager.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_EVENTMAN_FINDROOT


//-----------------------------------------------------------------------------
// EventManager()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
EventManager::EventManager() :
   TriggerManager       ()
{
   triggerType = Gmat::EVENT;
   triggerTypeString = "EventManager";
}


//-----------------------------------------------------------------------------
// ~EventManager()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
EventManager::~EventManager()
{
   CleanUp();
}


void EventManager::CleanUp()
{
   eventNames.clear();
   
   // clean up ObjectArray events;
   for (Integer i = 0; i < events.size(); ++i)
   {
      if (events[i])
         delete events[i];
   }
   events.clear();

   // clean up EstimationRootFinder locater;
   locater.CleanUp();

   eventStatus.clear();
   activated.clear();
}


//-----------------------------------------------------------------------------
// EventManager(const EventManager& em)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param em The EventManager that supplies attributes for this new one
 */
//-----------------------------------------------------------------------------
EventManager::EventManager(const EventManager& em) :
   TriggerManager          (em)
{
}


//-----------------------------------------------------------------------------
// EventManager& operator=(const EventManager& em)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param em The EventManager that supplies attributes for this instance
 *
 * @return This instance, configured to match em
 */
//-----------------------------------------------------------------------------
EventManager& EventManager::operator=(const EventManager& em)
{
   if (this != &em)
   {
      TriggerManager::operator=(em);
   }

   return *this;
}


//-----------------------------------------------------------------------------
// void SetObject(GmatBase* obj)
//-----------------------------------------------------------------------------
/**
 * Passes a referenced object into the EventManager
 *
 * This method is used to set propagators (via a PropSetup pointer) and Events
 * on the EventManager.
 *
 * @param obj The referenced object
 */
//-----------------------------------------------------------------------------
void EventManager::SetObject(GmatBase* obj)
{
   UnsignedInt type = obj->GetType();

   switch (type)
   {
      case Gmat::PROP_SETUP:
         locater.SetPropSetup((PropSetup*)obj);
         break;

      case Gmat::EVENT:
         if (find(events.begin(), events.end(), obj) == events.end())
            events.push_back(obj);
         break;

      default:
         break;
   }
}


//-----------------------------------------------------------------------------
// void ClearObject(GmatBase* obj)
//-----------------------------------------------------------------------------
/**
 * Clears a single object or all of the object pointers in the EventManager
 *
 * @param obj The object that gets cleared, or NULL to clear all objects
 */
//-----------------------------------------------------------------------------
void EventManager::ClearObject(GmatBase* obj)
{
   if (obj != NULL)
   {
      UnsignedInt type = obj->GetType();

      switch (type)
      {
         case Gmat::PROP_SETUP:
            locater.SetPropSetup(NULL);
            break;

         case Gmat::EVENT:
            {
               ObjectArray::iterator object;
               object = find(events.begin(), events.end(), obj);
               if (object != events.end())
               {
                  #ifdef DEBUG_EVENT
                     MessageInterface::ShowMessage("EventManager removing a "
                           "%s event\n", obj->GetTypeName().c_str());
                  #endif
                  events.erase(object);
               }
            }
            break;

         default:
            break;
      }
   }
   else // Clear all if input was NULL
   {
      locater.SetPropSetup(NULL);
      events.clear();
   }
}


//-----------------------------------------------------------------------------
// bool CheckForTrigger()
//-----------------------------------------------------------------------------
/**
 * Determines if a managed event has been triggered
 *
 * This method is used to detect root crossings or extrema crossings in the
 * EventManager.  The method as provided here is a place holder for the full
 * implementation.  There is no trigger detection code in place yet.
 *
 * @return true if an event was triggered, false if not.  The current
 *         implementation always returns false.
 */
//-----------------------------------------------------------------------------
bool EventManager::CheckForTrigger()
{
   bool retval = false;

   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
//      Event *current = (Event*)events[i];
//      if (current->Evaluate())
//      {
//
//      }
   }

   return retval;
}


//-----------------------------------------------------------------------------
// Real LocateTrigger()
//-----------------------------------------------------------------------------
/**
 * Launches the event location process once an event trigger has been detected
 *
 * @return The epoch of the event
 *
 * @note This method is not yet implemented
 */
//-----------------------------------------------------------------------------
Real EventManager::LocateTrigger()
{
   return -1;
}


//-----------------------------------------------------------------------------
// TriggerManager* Clone()
//-----------------------------------------------------------------------------
/**
 * Creates a new EventManager that matches this one
 *
 * @return The new EventManager
 */
//-----------------------------------------------------------------------------
TriggerManager* EventManager::Clone()
{
   return new EventManager(*this);
}


//-----------------------------------------------------------------------------
// void SetFixedState(Event *thisOne)
//-----------------------------------------------------------------------------
/**
 * Fixes the state data for an Event
 *
 * @param thisOne The Event supplying the state data
 */
//-----------------------------------------------------------------------------
void EventManager::SetFixedState(Event *thisOne)
{
   locater.FixState(thisOne);
}


//-----------------------------------------------------------------------------
// Real FindRoot(Integer whichOne)
//-----------------------------------------------------------------------------
/**
 * Locates the zero-crossing for an Event
 *
 * @param whichOne Index of the event to be processed
 *
 * @return The epoch of the event, or -1.0 if no zero crossing was found
 */
//-----------------------------------------------------------------------------
Real EventManager::FindRoot(Integer whichOne)
{
   #ifdef DEBUG_EVENTMAN_FINDROOT
      MessageInterface::ShowMessage("EventManager::FindRoot looking for "
            "root %d out of %d known events\n", whichOne, events.size());
   #endif
   Real rootTime = -1.0;

   ObjectArray eventList;
   eventList.clear();

   // For now, just find one root in this call
   if ((whichOne >= 0) && (whichOne < (Integer)events.size()))
   {
      #ifdef DEBUG_EVENTMAN_FINDROOT
         MessageInterface::ShowMessage("   Placing %s on the stack\n",
               events[whichOne]->GetName().c_str());
      #endif

      eventList.push_back(events[whichOne]);
      #ifdef DEBUG_EVENTMAN_FINDROOT
         MessageInterface::ShowMessage("   Calling RootFinder to check the "
               "current event list\n");
      #endif

      rootTime = locater.Locate(eventList);

      ((Event*)events[whichOne])->Evaluate();
      // Now calculate the next time step estimate based on propagated states
      ((Event*)events[whichOne])->EstimateTimestep();

      #ifdef DEBUG_EVENTMAN_FINDROOT
         MessageInterface::ShowMessage("   Evaluated event function; status is "
               "now %d\n", ((Event*)events[whichOne])->CheckStatus());
      #endif
   }

   return rootTime;
}


//-----------------------------------------------------------------------------
// void ProcessResults()
//-----------------------------------------------------------------------------
/**
 * Processes the results of event location
 *
 * @note: This method is not yet implemented
 */
//-----------------------------------------------------------------------------
void EventManager::ProcessResults()
{
}


//-----------------------------------------------------------------------------
// void SaveEventData()
//-----------------------------------------------------------------------------
/**
 * Stores the event data for later processing
 *
 * @note: This method is not yet implemented
 */
//-----------------------------------------------------------------------------
void EventManager::SaveEventData()
{
}


//-----------------------------------------------------------------------------
// const IntegerArray& GetStatus()
//-----------------------------------------------------------------------------
/**
 * Retrieves the overall status of the managed Events.
 *
 * @return The status
 */
//-----------------------------------------------------------------------------
const IntegerArray& EventManager::GetStatus()
{
   return eventStatus;
}
