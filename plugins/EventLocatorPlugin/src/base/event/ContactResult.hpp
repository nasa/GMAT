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
 * Definition for the ContactResult class.  This class is a holder of
 * ContactEvents.
 */
//------------------------------------------------------------------------------

#ifndef ContactResult_hpp
#define ContactResult_hpp

#include "gmatdefs.hpp"
#include "ContactEvent.hpp"

/**
 * ContactResult class
 */
class LOCATOR_API ContactResult : public LocatedEvent
{
public:

   ContactResult();
   virtual ~ContactResult();
   ContactResult(const ContactResult& copy);
   ContactResult& operator=(const ContactResult& copy);

   // Add events to the list
   virtual bool         AddEvent(ContactEvent* newEvent);
   //virtual bool         SetEvent(Integer atIndex, ContactEvent *toEvent, bool deleteExisting = false);
   virtual Integer      NumberOfEvents();
   /// Set the no-events string
   virtual void         SetNoEvents(const std::string &noEv);
   /// Set the name of the observer
   virtual void         SetObserverName(const std::string &itsName);

   /// Return computed quantities
   virtual Real         GetDuration();

//   /// Set index
//   void                 SetIndex(Integer i);

   /// Return a specific Event
   ContactEvent*        GetEvent(Integer atIndex);

   // Get the report string
   virtual std::string  GetReportString();

   // Clear the event list
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// The eclipse events
   std::vector<ContactEvent*> theEvents;
   /// Number of events
   Integer                    numEvents;
   /// The name of the observer
   std::string                observerName;
   /// No-events string
   std::string                noEvents;
};

#endif // ContactResult_hpp
