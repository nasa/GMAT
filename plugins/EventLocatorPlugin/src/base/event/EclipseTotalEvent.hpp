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
// Created: 2015.01.09
//
/**
 * Definition for the EclipseTotalEvent class.  This class is a holder of
 * EclipseEvents.
 */
//------------------------------------------------------------------------------

#ifndef EclipseTotalEvent_hpp
#define EclipseTotalEvent_hpp

#include "gmatdefs.hpp"
#include "EclipseEvent.hpp"

/**
 * EclipseTotalEvent class
 */
class LOCATOR_API EclipseTotalEvent : public LocatedEvent
{
public:

   EclipseTotalEvent();
   virtual ~EclipseTotalEvent();
   EclipseTotalEvent(const EclipseTotalEvent& copy);
   EclipseTotalEvent& operator=(const EclipseTotalEvent& copy);

   // Add events to the list
   virtual bool         AddEvent(EclipseEvent* newEvent);
   virtual bool         SetEvent(Integer atIndex, EclipseEvent *toEvent,
                                 bool deleteExisting = false);

   virtual Integer      NumberOfEvents();

   /// Return computed quantities
   virtual Real         GetDuration();

   /// Set index
   void                 SetIndex(Integer i);

   /// Return a specific Event
   EclipseEvent*        GetEvent(Integer atIndex);

   // Get the report string
   virtual std::string  GetReportString();

   // Clear the event list
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// The eclipse events
   std::vector<EclipseEvent*> theEvents;
   /// Number of events
   Integer                    numEvents;
   /// Index of the total event
   Integer                    theIndex;
};

#endif // EclipseTotalEvent_hpp
