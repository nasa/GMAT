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
 * Definition for the IntrusionResult class.  This class is a holder of
 * IntrusionEvents.
 */
 //------------------------------------------------------------------------------

#ifndef IntrusionResult_hpp
#define IntrusionResult_hpp

#include "gmatdefs.hpp"
#include "IntrusionEvent.hpp"

/**
 * IntrusionResult class
 */
class LOCATOR_API IntrusionResult : public LocatedEvent
{
public:

   IntrusionResult();
   virtual ~IntrusionResult();
   IntrusionResult(const IntrusionResult& copy);
   IntrusionResult& operator=(const IntrusionResult& copy);

   // Add events to the list
   virtual bool         AddEvent(IntrusionEvent* newEvent);
   //virtual bool         SetEvent(Integer atIndex, ContactEvent *toEvent, bool deleteExisting = false);
   virtual Integer      NumberOfTotalEvents();
   virtual Integer      NumberOfIndividualEvents();
   /// Set the no-events string
   virtual void         SetNoEvents(const std::string &noEv);
   /// Set the spacecraft name the sensor is on
   virtual void         SetSpacecraftName(const std::string &nameOfSC);
   /// Set the name of the sensor
   virtual void         SetSensorName(const std::string &nameOfSensor);
   /// Set the coordinate type used for reporting
   virtual void         SetCoordinateType(const std::string &coordinateType);
   virtual void         SetCoordinateNames(const std::string &firstCoordName, const std::string &secondCoordName);

   /// Return computed quantities
   virtual Real         GetDuration();

   //   /// Set index
   //   void                 SetIndex(Integer i);

      /// Return a specific Event
   IntrusionEvent*      GetEvent(Integer atIndex);

   // Get the report string
   virtual std::string  GetReportString();

   // Clear the event list
   virtual bool         TakeAction(const std::string &action,
      const std::string &actionData = "");

protected:
   /// The eclipse events
   std::vector<IntrusionEvent*> theEvents;
   /// Number of events
   Integer                    numEvents;
   /// No-events string
   std::string                noEvents;
   /// The name of the first coordinate
   std::string                coord1Name;
   /// The name of the second coordinate
   std::string                coord2Name;
   /// The name of the spacecraft
   std::string                scName;
   /// The name of the sensor
   std::string                sensorName;
   /// The type of coordinate system used for reporting
   std::string                coordType;
};

#endif // IntrusionResult_hpp
