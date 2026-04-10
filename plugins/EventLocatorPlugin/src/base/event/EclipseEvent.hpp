//$Id$
//------------------------------------------------------------------------------
//                               EclipseEvent
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
// Created: 2015.01.08
//
/**
 * Definition for the EclipseEvent class.
 */
//------------------------------------------------------------------------------

#ifndef EclipseEvent_hpp
#define EclipseEvent_hpp

#include "gmatdefs.hpp"
#include "EventLocatorDefs.hpp"
#include "LocatedEvent.hpp"

/**
 * EclipseEvent class
 */
class LOCATOR_API EclipseEvent : public LocatedEvent
{
public:

   EclipseEvent();
   EclipseEvent(Real startEpoch, Real endEpoch, const std::string itsType, const std::string theBody);
   virtual ~EclipseEvent();
   EclipseEvent(const EclipseEvent& copy);
   EclipseEvent& operator=(const EclipseEvent& copy);

   /// Return computed quantities
   
   virtual std::string GetBodyName();

   // Get the report string
   virtual std::string  GetReportString();

protected:
   /// The eclipse type
   std::string  eclipseType;
   ///The name of the occulting body (or bodies)
   std::string  occultingBody;
};

#endif // EclipseEvent_hpp
