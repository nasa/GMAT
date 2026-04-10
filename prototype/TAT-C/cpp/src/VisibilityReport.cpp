//------------------------------------------------------------------------------
//                           VisibilityReport
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
 * Implementation of the visibility report base class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "VisibilityReport.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  VisibilityReport()
//------------------------------------------------------------------------------
/**
 * Constructs VisibilityReport instance (default constructor).
 *
 */
//------------------------------------------------------------------------------
VisibilityReport::VisibilityReport()
{
   // by default, startDate and endDate are the AbsoluteDate default values
}

//------------------------------------------------------------------------------
//  VisibilityReport(const VisibilityReport &be)
//------------------------------------------------------------------------------
/**
 * Constructs VisibilityReport instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
VisibilityReport::VisibilityReport( const VisibilityReport &copy) :
   startDate   (copy.startDate),
   endDate     (copy.endDate)
{
}

//------------------------------------------------------------------------------
//  VisibilityReport& operator=(const VisibilityReport &copy)
//------------------------------------------------------------------------------
/**
 * VisibilityReport operator=.
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
VisibilityReport& VisibilityReport::operator=(const VisibilityReport &copy)
{
   if (&copy == this)
      return *this;
   
   startDate  = copy.startDate;
   endDate    = copy.endDate;
   
   return *this;
}


//------------------------------------------------------------------------------
//  ~VisibilityReport()
//------------------------------------------------------------------------------
/**
 * Destructs VisibilityReport instance
 */
//------------------------------------------------------------------------------
VisibilityReport::~VisibilityReport()
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
void VisibilityReport::SetStartDate(const AbsoluteDate &toDate)
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
void VisibilityReport::SetEndDate(const AbsoluteDate &toDate)
{
   endDate = toDate;
}



//------------------------------------------------------------------------------
// const AbsoluteDate& VisibilityReport::GetStartDate()
//------------------------------------------------------------------------------
/**
 * Returns the start date for the report.
 *
 * @return  The start time for the report.
 */
//------------------------------------------------------------------------------
const AbsoluteDate& VisibilityReport::GetStartDate()
{
   return startDate;
}

//------------------------------------------------------------------------------
// const AbsoluteDate& VisibilityReport::GetEndDate()
//------------------------------------------------------------------------------
/**
 * Returns the end date for the report.
 *
 * @return  The end time for the report.
 */
//------------------------------------------------------------------------------
const AbsoluteDate& VisibilityReport::GetEndDate()
{
   return endDate;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
