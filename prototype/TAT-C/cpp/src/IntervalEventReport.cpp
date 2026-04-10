//------------------------------------------------------------------------------
//                           IntervalEventReport
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.04.29
//
/**
 * Implementation of the Interval Event Report class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "IntervalEventReport.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  IntervalEventReport(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs IntervalEventReport instance (default constructor).
 *
 * @param <details> A message providing the details of the exception.
 */
//------------------------------------------------------------------------------
IntervalEventReport::IntervalEventReport():
poiIndex(-1)
{
    //VisiblePOIReport emtpyReport;
}

//------------------------------------------------------------------------------
//  IntervalEventReport(const IntervalEventReport &be)
//------------------------------------------------------------------------------
/**
 * Constructs IntervalEventReport instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
IntervalEventReport::IntervalEventReport( const IntervalEventReport &copy) :
   poiIndex    (copy.poiIndex),
   startDate   (copy.startDate),
   endDate     (copy.endDate),
   discretePOIEvents(copy.discretePOIEvents)
{
}

//------------------------------------------------------------------------------
//  IntervalEventReport& operator=(const IntervalEventReport &copy)
//------------------------------------------------------------------------------
/**
 * IntervalEventReport operator=.
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
IntervalEventReport& IntervalEventReport::operator=(const IntervalEventReport &copy)
{
   if (&copy == this)
      return *this;

   poiIndex          = copy.poiIndex;
   startDate         = copy.startDate;
   endDate           = copy.endDate;
   discretePOIEvents = copy.discretePOIEvents;
   return *this;
}

//------------------------------------------------------------------------------
//  ~IntervalEventReport()
//------------------------------------------------------------------------------
/**
 * Destructs IntervalEventReport instance
 */
//------------------------------------------------------------------------------
IntervalEventReport::~IntervalEventReport()
{
}

//------------------------------------------------------------------------------
//  void SetStartDate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
/**
 * Sets the start date for the report.
 *
 * @param toDate The start time for the report.
 */
//------------------------------------------------------------------------------
void IntervalEventReport::SetStartDate(const AbsoluteDate &toDate)
{
   startDate = toDate;
}

//------------------------------------------------------------------------------
//  void SetEndDate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
/**
 * Sets the end date for the report.
 *
 * @param toDate The end time for the report.
 */
//------------------------------------------------------------------------------
void IntervalEventReport::SetEndDate(const AbsoluteDate &toDate)
{
   endDate = toDate;
}


//------------------------------------------------------------------------------
// const AbsoluteDate& IntervalEventReport::GetStartDate()
//------------------------------------------------------------------------------
/**
 * Returns the start date for the report.
 *
 * @return  The start time for the report.
 */
//------------------------------------------------------------------------------
const AbsoluteDate& IntervalEventReport::GetStartDate()
{
   return startDate;
}

//------------------------------------------------------------------------------
// const AbsoluteDate& IntervalEventReport::GetEndDate()
//------------------------------------------------------------------------------
/**
 * Returns the end date for the report.
 *
 * @return  The end time for the report.
 */
//------------------------------------------------------------------------------
const AbsoluteDate& IntervalEventReport::GetEndDate()
{
   return endDate;
}

//------------------------------------------------------------------------------
// void SetPOIIndex(Integer toIdx)
//---------------------------------------------------------------------------
/**
* Sets the POI Index for the report.
*
* @param <toIdx> Index for the report
*/
//---------------------------------------------------------------------------
void IntervalEventReport::SetPOIIndex(Integer toIdx)
{
    poiIndex = toIdx;
}

//------------------------------------------------------------------------------
// Integer GetPOIIndex()
//---------------------------------------------------------------------------
/**
* Returns the POI Index for the report.
*
* @return <toIdx> Index for the report
*/
//---------------------------------------------------------------------------
Integer IntervalEventReport::GetPOIIndex()
{
    return poiIndex;
}

//------------------------------------------------------------------------------
// void IntervalEventReport::AddPOIEvent(const VisiblePOIReport theReport)
//------------------------------------------------------------------------------
/**
* Appends a VisiblePOIReport to the discrtePOIEvents std::vector
*
* @param VisiblePOIReport report.
*/
//------------------------------------------------------------------------------
void IntervalEventReport::AddPOIEvent(const VisiblePOIReport &theReport)
{
    discretePOIEvents.push_back(theReport);
}

//------------------------------------------------------------------------------
// std::vector<VisiblePOIReport> IntervalEventReport::GetPOIEvents()
//------------------------------------------------------------------------------
/**
 * Returns the vector of VisiblePOIReports
 *
 * @return  The vector of VisiblePOIReports
 */
//------------------------------------------------------------------------------
std::vector<VisiblePOIReport> IntervalEventReport::GetPOIEvents()
{
   return discretePOIEvents;
}

//------------------------------------------------------------------------------
// void IntervalEventReport::SetAllPOIEvents(const std::vector<VisiblePOIReport> theEvents)
//------------------------------------------------------------------------------
/**
* Sets the entire discrtePOIEvents std::vector
*
* @param std::vector<VisiblePOIReport>.
*/
//------------------------------------------------------------------------------
void IntervalEventReport::SetAllPOIEvents(const std::vector<VisiblePOIReport> theEvents)
{
    discretePOIEvents = theEvents;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
