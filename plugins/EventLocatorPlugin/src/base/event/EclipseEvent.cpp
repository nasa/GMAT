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
 * Implementation for the EclipseEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "EclipseEvent.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REPORT_STRING

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

EclipseEvent::EclipseEvent() :
   LocatedEvent  ()
{
}

EclipseEvent::EclipseEvent(Real startEpoch, Real endEpoch,
                           const std::string itsType, const std::string theBody) :
   LocatedEvent  (startEpoch, endEpoch),
   eclipseType   (itsType),
   occultingBody (theBody)
{
}

EclipseEvent::~EclipseEvent()
{
   // nothing to do here
}

EclipseEvent::EclipseEvent(const EclipseEvent& copy) :
   LocatedEvent  (copy),
   eclipseType   (copy.eclipseType),
   occultingBody (copy.occultingBody)
{
}

EclipseEvent& EclipseEvent::operator=(const EclipseEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      eclipseType   = copy.eclipseType;
      occultingBody = copy.occultingBody;
   }

   return *this;
}

std::string EclipseEvent::GetBodyName()
{
   return occultingBody;
}

std::string EclipseEvent::GetReportString()
{
   #ifdef DEBUG_REPORT_STRING
      MessageInterface::ShowMessage("In EcEv::GetReportString\n");
   #endif
   std::stringstream eventString("");
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string startGregorian, endGregorian;
   Real        resultMjd;
   std::string blanks       = "    ";

   TimeSystemConverter::Instance()->Convert("A1ModJulian", start, "",
                              outputFormat, resultMjd, startGregorian);
   TimeSystemConverter::Instance()->Convert("A1ModJulian", end, "",
                              outputFormat, resultMjd, endGregorian);

   #ifdef DEBUG_REPORT_STRING
      MessageInterface::ShowMessage("In EcEv::GetReportString, about to get Duration\n");
   #endif
   Real currentDuration = GetDuration();

   eventString << startGregorian << blanks << endGregorian         << blanks <<
                  GmatStringUtil::BuildNumber(currentDuration, false, 14)    <<
                  blanks <<
                  GmatStringUtil::PadWithBlanks(occultingBody, 12) << blanks <<
                  GmatStringUtil::PadWithBlanks(eclipseType, 8);
   return eventString.str();
}

