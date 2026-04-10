//$Id$
//------------------------------------------------------------------------------
//                               IntrusionEvent
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
 * Definition for the IntrusionEvent class.
 */
 //------------------------------------------------------------------------------

#ifndef IntrusionEvent_hpp
#define IntrusionEvent_hpp

#include "gmatdefs.hpp"
#include "EventLocatorDefs.hpp"
#include "LocatedEvent.hpp"

/**
 * IntrusionEvent class
 */
class LOCATOR_API IntrusionEvent : public LocatedEvent
{
public:

   IntrusionEvent();
   IntrusionEvent(const std::string &type, Real startEpoch, Real endEpoch, RealArray epochArray,
      const std::string theBody, RealArray angularDiameter,
      RealArray coordAngle1, RealArray coordAngle2, RealArray illuminations);
   virtual ~IntrusionEvent();
   IntrusionEvent(const IntrusionEvent& copy);
   IntrusionEvent& operator=(const IntrusionEvent& copy);

   /// Return computed quantities
   // Get the number of individual events stepped through
   Integer GetNumIndividualEvents();
   // Get the type of intrusion event
   virtual std::string GetIntrusionType();
   // Get the report string
   virtual std::string GetReportString();
   virtual void SetSkipFirstLine(bool skip);
   virtual void SetSkipLastLine(bool skip);

protected:
   ///The name of the intruding body
   std::string intrudingBody;
   ///The type of intrusion between a typical intrusion, an occultation, or a transit
   std::string intrusionType;
   /// Epochs during the intrusion event
   RealArray epochs;
   /// The angular diameter of the intruding body
   RealArray angDiameter;
   /// First value of angular location of intruding body
   RealArray angLoc1;
   /// Second value of angular location of intruding body
   RealArray angLoc2;
   /// The illumination values of each intrusion
   RealArray illmnVals;
   bool skipFirstLine;
   bool skipLastLine;
};

#endif // IntrusionEvent_hpp
