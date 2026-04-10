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
 * Implementation for the IntrusionEvent base class.
 */
 //------------------------------------------------------------------------------

#include <sstream>
#include "IntrusionEvent.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REPORT_STRING

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IntrusionEvent()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
IntrusionEvent::IntrusionEvent() :
   LocatedEvent()
{
}

//------------------------------------------------------------------------------
// IntrusionEvent(Real startEpoch, Real endEpoch, RealArray epochArray,
//    const std::string theBody, RealArray angularDiameter,
//    RealArray coordAngle1, RealArray coordAngle2, RealArray illuminations)
//------------------------------------------------------------------------------
/**
* Constructor where data for event is immediately provided
*
* @param type The intrusion event type, possible types are intrusion, 
         occultation or transit
* @param startEpoch The starting epoch of the event in A1ModJulian
* @param endEpoch The ending epoch of the event in A1ModJulian
* @param epochArray The intermediate time steps of the intrusion between and
*        including startEpoch and endEpoch in A1ModJulian
* @param theBody The intruding body of this event
* @param angularDiameter The angular diameter of the intruding body at each 
*        step in the event in degrees
* @param coordAngle1 The first angular coordinate (x-coord) of the intruding
*        body from the sensor's coordinate frame in degrees
* @param coordAngle2 The first angular coordinate (y-coord) of the intruding 
*        body from the sensor's coordinate frame in degrees
* @param illuminations The amount of illumination the intruding body has at 
*        each step from the sensor's point of view
*/
//------------------------------------------------------------------------------
IntrusionEvent::IntrusionEvent(const std::string &type, Real startEpoch, Real endEpoch,
   RealArray epochArray, const std::string theBody, RealArray angularDiameter,
   RealArray coordAngle1, RealArray coordAngle2, RealArray illuminations) :
   LocatedEvent(startEpoch, endEpoch),
   intrusionType(type),
   intrudingBody(theBody),
   skipFirstLine(false),
   skipLastLine(false)
{
   epochs.clear();
   for (Integer ii = 0; ii < epochArray.size(); ++ii)
      epochs.push_back(epochArray.at(ii));
   angDiameter.clear();
   for (Integer ii = 0; ii < angularDiameter.size(); ++ii)
      angDiameter.push_back(angularDiameter.at(ii));
   angLoc1.clear();
   for (Integer ii = 0; ii < coordAngle1.size(); ++ii)
      angLoc1.push_back(coordAngle1.at(ii));
   angLoc2.clear();
   for (Integer ii = 0; ii < coordAngle2.size(); ++ii)
      angLoc2.push_back(coordAngle2.at(ii));
   illmnVals.clear();
   for (Integer ii = 0; ii < illuminations.size(); ++ii)
      illmnVals.push_back(illuminations.at(ii));
}

//------------------------------------------------------------------------------
// ~IntrusionEvent()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
IntrusionEvent::~IntrusionEvent()
{
   // nothing to do here
}

//------------------------------------------------------------------------------
// IntrusionEvent(const IntrusionEvent& copy)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
IntrusionEvent::IntrusionEvent(const IntrusionEvent& copy) :
   LocatedEvent(copy),
   intrusionType(copy.intrusionType),
   intrudingBody(copy.intrudingBody),
   skipFirstLine(copy.skipFirstLine),
   skipLastLine(copy.skipLastLine)
{
   epochs.clear();
   for (Integer ii = 0; ii < copy.epochs.size(); ++ii)
      epochs.push_back(copy.epochs.at(ii));
   angDiameter.clear();
   for (Integer ii = 0; ii < copy.angDiameter.size(); ++ii)
      angDiameter.push_back(copy.angDiameter.at(ii));
   angLoc1.clear();
   for (Integer ii = 0; ii < copy.angLoc1.size(); ++ii)
      angLoc1.push_back(copy.angLoc1.at(ii));
   angLoc2.clear();
   for (Integer ii = 0; ii < copy.angLoc2.size(); ++ii)
      angLoc2.push_back(copy.angLoc2.at(ii));
   illmnVals.clear();
   for (Integer ii = 0; ii < copy.illmnVals.size(); ++ii)
      illmnVals.push_back(copy.illmnVals.at(ii));
}

IntrusionEvent& IntrusionEvent::operator=(const IntrusionEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      intrudingBody = copy.intrudingBody;
      skipFirstLine = copy.skipFirstLine;
      skipLastLine = copy.skipLastLine;

      epochs.clear();
      for (Integer ii = 0; ii < copy.epochs.size(); ++ii)
         epochs.push_back(copy.epochs.at(ii));
      angDiameter.clear();
      for (Integer ii = 0; ii < copy.angDiameter.size(); ++ii)
         angDiameter.push_back(copy.angDiameter.at(ii));
      angLoc1.clear();
      for (Integer ii = 0; ii < copy.angLoc1.size(); ++ii)
         angLoc1.push_back(copy.angLoc1.at(ii));
      angLoc2.clear();
      for (Integer ii = 0; ii < copy.angLoc2.size(); ++ii)
         angLoc2.push_back(copy.angLoc2.at(ii));
      illmnVals.clear();
      for (Integer ii = 0; ii < copy.illmnVals.size(); ++ii)
         illmnVals.push_back(copy.illmnVals.at(ii));
   }

   return *this;
}

//------------------------------------------------------------------------------
// Integer GetNumIndividualEvents()
//------------------------------------------------------------------------------
/**
* Get the number of individual events within this total event
*
* @return The number of indiviual events
*/
//------------------------------------------------------------------------------
Integer IntrusionEvent::GetNumIndividualEvents()
{
   Integer retVal = epochs.size();
   if (skipFirstLine)
      --retVal;
   if (skipLastLine)
      --retVal;

   if (retVal < 0)
      retVal = 0;

   return retVal;
}

//------------------------------------------------------------------------------
// Integer GetNumIndividualEvents()
//------------------------------------------------------------------------------
/**
* Return the type of intrusion event this is. Possible options are intrusion,
* transit, or occultation
*
* @return The intrusion type
*/
//------------------------------------------------------------------------------
std::string IntrusionEvent::GetIntrusionType()
{
   return intrusionType;
}

//------------------------------------------------------------------------------
// std::string GetReportString()
//------------------------------------------------------------------------------
/**
* Builds and returns the reporting string for this total event, used to print
* to the users output file for an IntrusionLocator
*
* @return The report string
*/
//------------------------------------------------------------------------------
std::string IntrusionEvent::GetReportString()
{
   #ifdef DEBUG_REPORT_STRING
      MessageInterface::ShowMessage("In EcEv::GetReportString\n");
   #endif
   std::stringstream eventString("");
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string startGregorian, endGregorian;
   Real        resultMjd;
   std::string blanks = "    ";

   TimeSystemConverter::Instance()->Convert("A1ModJulian", start, "",
      outputFormat, resultMjd, startGregorian);
   TimeSystemConverter::Instance()->Convert("A1ModJulian", end, "",
      outputFormat, resultMjd, endGregorian);

   StringArray epochsGregorian;
   std::string tempEpoch;
   for (Integer ii = 0; ii < epochs.size(); ++ii)
   {
      TimeSystemConverter::Instance()->Convert("A1ModJulian", epochs.at(ii), "",
         outputFormat, resultMjd, tempEpoch);
      epochsGregorian.push_back(tempEpoch);
   }

   #ifdef DEBUG_REPORT_STRING
      MessageInterface::ShowMessage("In EcEv::GetReportString, about to get Duration\n");
   #endif
   Real currentDuration = GetDuration();

   Integer ii = 0;
   Integer dataSize = epochs.size();
   if (skipFirstLine)
      ++ii;
   if (skipLastLine)
      --dataSize;

   for (ii; ii < dataSize; ++ii)
   {
      if (ii == (epochs.size() - 1))
      {
         if (intrusionType == "Occultation")
            eventString << "\n";
      }

      eventString << epochsGregorian.at(ii) << blanks <<
         GmatStringUtil::PadWithBlanks(intrudingBody, 12) << blanks <<
         GmatStringUtil::BuildNumber(angDiameter.at(ii), false, 14) <<
         blanks << GmatStringUtil::BuildNumber(angLoc1.at(ii), false, 14) <<
         blanks << GmatStringUtil::BuildNumber(angLoc2.at(ii), false, 14) <<
         blanks << GmatStringUtil::BuildNumber(illmnVals.at(ii), false, 14) <<
         blanks;

      std::string intrusionDesc;
      if (ii == 0)
         intrusionDesc = intrusionType + " Start";
      else if (ii == (epochs.size() - 1))
         intrusionDesc = intrusionType + " End";
      else
         intrusionDesc = intrusionType;

      eventString << GmatStringUtil::PadWithBlanks(intrusionDesc, 12) << "\n";
   }
   return eventString.str();
}

//------------------------------------------------------------------------------
// void SetSkipFirstLine(bool skip)
//------------------------------------------------------------------------------
/**
* Set whether to skip the first line written in this event. Used when 
* transitioning between types of intrusions to avoid printing the same set of 
* data twice (i.e. transit -> intrusion, occulatation -> intrusion)
*
* @param skip Whether or not to skip writing the first line of this event
*/
//------------------------------------------------------------------------------
void IntrusionEvent::SetSkipFirstLine(bool skip)
{
   skipFirstLine = skip;
}

//------------------------------------------------------------------------------
// void SetSkipLastLine(bool skip)
//------------------------------------------------------------------------------
/**
* Set whether to skip the last line written in this event. Used when
* transitioning between types of intrusions to avoid printing the same set of
* data twice (i.e. intrusion -> transit, intrusion -> occultation)
*
* @param skip Whether or not to skip writing the las line of this event
*/
//------------------------------------------------------------------------------
void IntrusionEvent::SetSkipLastLine(bool skip)
{
   skipLastLine = skip;
}
