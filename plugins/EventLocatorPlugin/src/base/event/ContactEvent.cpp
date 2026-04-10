//$Id$
//------------------------------------------------------------------------------
//                               ContactEvent
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
 * Implementation for the ContactEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include "ContactEvent.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

ContactEvent::ContactEvent(Real startEpoch, Real endEpoch) :
   LocatedEvent(startEpoch, endEpoch)
{
}

ContactEvent::ContactEvent(Real startEpoch, Real endEpoch, StringArray columns, bool leftJust) :
   LocatedEvent(startEpoch, endEpoch)
{
   leftJustified = leftJust;

   for (Integer i = 0; i < columns.size(); i++)
   {
      reportColumns.push_back(column(columns.at(i).c_str(), i));
   }
}

ContactEvent::~ContactEvent()
{
   // nothing to do here
}

ContactEvent::ContactEvent(const ContactEvent& copy) :
   LocatedEvent(copy)
{
}

ContactEvent& ContactEvent::operator=(const ContactEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
   }

   return *this;
}

std::string ContactEvent::GetReportStringLegacy()
{
   std::stringstream eventString("");
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string startGregorian, endGregorian;
   Real        resultMjd;
   std::string blanks = "    ";

   TimeSystemConverter::Instance()->Convert("A1ModJulian", start, "",
      outputFormat, resultMjd, startGregorian);
   TimeSystemConverter::Instance()->Convert("A1ModJulian", end, "",
      outputFormat, resultMjd, endGregorian);

   Real currentDuration = GetDuration();

   eventString << startGregorian << blanks << endGregorian << blanks <<
      GmatStringUtil::BuildNumber(currentDuration, false, 14);

   return eventString.str();
}

std::string ContactEvent::GetReportString()
{
   std::stringstream eventString("");

   for (Integer i = 0; i < reportColumns.size(); i++)
   {
      if (reportColumns.at(i).columnNumber == i)
      {
         std::stringstream valueString("");
         valueString << reportColumns.at(i).cellData.at(0);
         if (valueString.str().size() < reportColumns.at(i).columnWidth)
         {
            std::string valueSpaces("");
            for (Integer j = 0; j < (reportColumns.at(i).columnWidth - valueString.str().size()); j++)
            {
               valueSpaces += " ";
            }
            if (leftJustified)
            {
               valueString << valueSpaces;
            }
            else
            {
               std::string tempString = valueSpaces + valueString.str();
               valueString.str(std::string());
               valueString << tempString;
            }
         }
         eventString << valueString.str() << columnSpacing;
      }
   }

   return eventString.str();
}

std::string ContactEvent::GetAERString()
{
   std::stringstream eventString("");

   for (Integer i = 0; i < eventTimes.size(); i++)
   {
      for (Integer j = 0; j < reportColumns.size(); j++)
      {
         if (reportColumns.at(j).columnNumber == j)
         {
            std::stringstream valueString("");
            if (reportColumns.at(j).aerColumn)
            {
               valueString << reportColumns.at(j).cellData.at(i);
            }
            else
            {
               valueString << reportColumns.at(j).cellData.at(0);
            }

            if (valueString.str().size() < reportColumns.at(j).columnWidth)
            {
               std::string valueSpaces("");
               for (Integer k = 0; k < (reportColumns.at(j).columnWidth - valueString.str().size()); k++)
               {
                  valueSpaces += " ";
               }
               if (leftJustified)
               {
                  valueString << valueSpaces;
               }
               else
               {
                  std::string tempString = valueSpaces + valueString.str();
                  valueString.str(std::string());
                  valueString << tempString;
               }
            }
            eventString << valueString.str() << columnSpacing;
         }
      }
      eventString << "\n";
   }

   return eventString.str();
}
