//$Id$
//------------------------------------------------------------------------------
//                                  A1Date
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 1995/10/05 for GSS project
// Modified: 2003/09/12 Linda Jun - See A1Date.hpp
//
/**
 * Provides conversions among various ways representing A1 calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#include "A1Date.hpp"
#include "Date.hpp"
#include "TimeTypes.hpp"
#include "UtcDate.hpp"
#include "A1Mjd.hpp"

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  A1Date()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @note Calls A1Mjd default constructor which creates an object with 0 
 *       second from reference.
 */
//------------------------------------------------------------------------------
A1Date::A1Date()
  : Date()
{
}

//------------------------------------------------------------------------------
//  A1Date (Integer year, Integer month, Integer day, Integer hour, 
//          Integer minute, Real second)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @note Assumes input date is in A1 time system.
 */
//------------------------------------------------------------------------------
A1Date::A1Date (Integer year, Integer month, Integer day, Integer hour, 
                Integer minute, Real second)
  : Date(year, month, day, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  A1Date (Integer year, Integer dayOfYear, Integer hour, Integer minute,
//          Real second)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @note Assumes input date is in A1 time system.
 */
//------------------------------------------------------------------------------
A1Date::A1Date (Integer year, Integer dayOfYear, Integer hour, Integer minute,
                Real second)
  : Date(year, dayOfYear, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  A1Date (Integer year, Integer month, Integer day, Real secondsOfDay)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @note Assumes input date is in A1 time system.
 */
//------------------------------------------------------------------------------
A1Date::A1Date (Integer year, Integer month, Integer day, Real secondsOfDay)
  : Date(year, month, day, secondsOfDay)
{
}

//------------------------------------------------------------------------------
// A1Date(const GmatTimeUtil::CalDate &date)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @note assumes input date is in A1 time system.
 */
//------------------------------------------------------------------------------
A1Date::A1Date(const GmatTimeUtil::CalDate &date)
   : Date(date)
{
}

//------------------------------------------------------------------------------
//  A1Date (const std::string& date)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <time> Time in "YYMMDD.hhmmssnnn" format
 *
 * @note Assumes input date is in A1 time system.
 */
//------------------------------------------------------------------------------
A1Date::A1Date (const std::string& dateString)
  : Date(dateString)
{
}

//------------------------------------------------------------------------------
//  A1Date (const A1Date &a1date)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
A1Date::A1Date (const A1Date &a1date)
  : Date(a1date)
{
}

//------------------------------------------------------------------------------
//  A1Date operator= (const A1Date &a1date)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
A1Date A1Date::operator= (const A1Date &a1date)
{
   if (this != &a1date)
      Date::operator=(a1date);
   
   return *this;
}

//------------------------------------------------------------------------------
//  bool operator> (const A1Date &a1date) const
//------------------------------------------------------------------------------
/**
 * Comparison operator >
 */
//------------------------------------------------------------------------------
bool A1Date::operator> (const A1Date &a1date) const
{
   return Date::operator>(a1date);
}


//------------------------------------------------------------------------------
//  bool operator< (const A1Date &a1date) const
//------------------------------------------------------------------------------
/**
 * Comparison operator <
 */
//------------------------------------------------------------------------------
bool A1Date::operator< (const A1Date &a1date) const
{
   return Date::operator<(a1date);
}

//------------------------------------------------------------------------------
//  A1Date ()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
A1Date::~A1Date ()
{
}

//loj: Do this later.
//  //------------------------------------------------------------------------------
//  //  Real operator- (const A1Date &date) const
//  //------------------------------------------------------------------------------
//  Real A1Date::operator- (const A1Date &date) const
//  {
//     Real offset;
   
//     A1Mjd t1(yearD, monthD, dayD, secondsOfDayD);
//     A1Mjd t2(date.yearD, date.monthD, date.dayD, date.secondsOfDayD);
   
//     offset = t1 - t2;
//     return offset;
//  }

//  //------------------------------------------------------------------------------
//  //  A1Date operator+ (const Real seconds) const
//  //------------------------------------------------------------------------------
//  A1Date A1Date::operator+ (const Real seconds) const
//  {
//     A1Mjd tempTime(*this);
   
//     tempTime = tempTime + seconds;
//     return A1Date(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  A1Date& operator+= (const Real seconds)
//  //------------------------------------------------------------------------------
//  A1Date& A1Date::operator+= (const Real seconds)
//  {
//      A1Date a1date;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime + seconds;
//      a1date = A1Date(tempTime);
//      yearD = a1date.Year();
//      monthD = a1date.Month();
//      dayD = a1date.Day();
//      secondsOfDayD = a1date.SecondsOfDay();
//      return *this;
//  }

//  //------------------------------------------------------------------------------
//  //  A1Date operator- (const Real seconds) const
//  //
//  //  Notes: none
//  //------------------------------------------------------------------------------
//  A1Date
//  A1Date::operator- (const Real seconds) const
//  {
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      return A1Date(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  A1Date& operator-= (const Real seconds)
//  //
//  //  Notes: none
//  //------------------------------------------------------------------------------
//  A1Date&
//  A1Date::operator-= (const Real seconds)
//  {
//      A1Date a1date;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      a1date = A1Date(tempTime);
//      yearD = a1date.Year();
//      monthD = a1date.Month();
//      dayD = a1date.Day();
//      secondsOfDayD = a1date.SecondsOfDay();
//      return *this;
//  }

//------------------------------------------------------------------------------
//  GmatTimeUtil::CalDate ToUtcDate() const
//------------------------------------------------------------------------------
/**
 * @note The two time systems label time differently.  At any given moment,
 *       the A.1 system is several seconds ahead of the UTC system.  This offset
 *       is constant between leap insertions.  For example, the instant of time
 *       labeled July 1, 1992, 12:00:27.0343817 in the A.1 system will be labeled
 *       July 1, 1992, 12:00:00 (Noon) in the UTC system.
 */
//------------------------------------------------------------------------------
//  GmatTimeUtil::CalDate A1Date::ToUtcDate() const
//  {
//     UtcDate utcDate;
//     GmatTimeUtil::CalDate tempDate;
   
//     // convert A1 date to equivalent UTC date
//     //loj: work on this
//     A1Mjd a1Time(*this);
//     utcDate = UtcDate(a1Time);
//     utcDate.YearMonDayHourMinSec(tempDate.year, tempDate.month,  tempDate.day, 
//                                  tempDate.hour, tempDate.minute, tempDate.second);
//     return tempDate;
//  }
