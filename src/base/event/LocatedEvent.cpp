//$Id$
//------------------------------------------------------------------------------
//                           LocatedEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Heavily Modified: Wendy Shoan/GSFC  2015.01.09
//
/**
 * Implementation for the LocatedEvent base class.
 */
//------------------------------------------------------------------------------

#include "LocatedEvent.hpp"
#include "GmatConstants.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

LocatedEvent::LocatedEvent() :
   start     (0.0),
   end       (0.0),
   duration  (0.0)
{
}

LocatedEvent::LocatedEvent(Real startEpoch, Real endEpoch) :
   start     (startEpoch),
   end       (endEpoch),
   duration  (0.0)
{
}

LocatedEvent::~LocatedEvent()
{
   // nothing to do here
}

LocatedEvent::LocatedEvent(const LocatedEvent& copy) :
   start        (copy.start),
   end          (copy.end),
   duration     (copy.duration)
{
}

LocatedEvent& LocatedEvent::operator=(const LocatedEvent& copy)
{
   if (&copy != this)
   {
      start     = copy.start;
      end       = copy.end;
      duration  = copy.duration;
   }

   return *this;
}


Real LocatedEvent::GetDuration()
{
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}

Real LocatedEvent::GetStart()
{
   return start;
}

Real LocatedEvent::GetEnd()
{
   return end;
}

//std::string LocatedEvent::GetReportString()
//{
//   // Abstract for now
//}

void LocatedEvent::SetStart(Real epoch1)
{
   start = epoch1;
}

void LocatedEvent::SetEnd(Real epoch2)
{
   end = epoch2;
}

