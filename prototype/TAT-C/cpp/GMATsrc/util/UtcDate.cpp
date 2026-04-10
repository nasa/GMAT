//$Id$
//------------------------------------------------------------------------------
//                                UtcDate
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
// Created: 1995/10/18 for GSS project
// Modified:
//   2003/09/12 Linda Jun - Added member data: descs, stringValues.
//              Replaced GSSString with std::string.
//   2004/05/06 J. Gurganus - See UtcDate.hpp for details.
//
/**
 * Provides conversions among various ways representing UTC calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#include <cmath>
#include "utildefs.hpp"
#include "RealUtilities.hpp"   // for Round() 
#include "A1Mjd.hpp"     // for A1Mjd
#include "UtcDate.hpp"   // for UtcDate
#include "TimeSystemConverter.hpp"
//  #include "A1Date.hpp"    // for A1Date

using namespace GmatTimeConstants;
using namespace GmatMathUtil;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  UtcDate()
//------------------------------------------------------------------------------
/**
 * @note Calls A1Mjd default constructor which creates an object with 0 
 *       second from reference.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate()
  : Date()
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer month, Integer day, Integer hour, 
//           Integer minute, Real second)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer month, Integer day, Integer hour, 
                  Integer minute, Real second)
  : Date(year, month, day, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer dayOfYear, Integer hour, Integer minute,
//           Real second)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer dayOfYear, Integer hour, Integer minute,
                  Real second)
  : Date(year, dayOfYear, hour, minute, second)
{
}

//------------------------------------------------------------------------------
//  UtcDate (Integer year, Integer month, Integer day, Real secondsOfDay);
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (Integer year, Integer month, Integer day, Real secondsOfDay)
  : Date(year, month, day, secondsOfDay)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const GmatTimeUtil::CalDate &date)
//------------------------------------------------------------------------------
/**
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (const GmatTimeUtil::CalDate &date)
  : Date(date)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const std::string& time)
//------------------------------------------------------------------------------
/**
 * @param <time> Time in "YYMMDD.hhmmssnnn" format
 *
 * @note Assumes input date is in UTC time system.
 */
//------------------------------------------------------------------------------
UtcDate::UtcDate (const std::string& time)
  : Date(time)
{
}

//------------------------------------------------------------------------------
//  UtcDate (const UtcDate &date)
//------------------------------------------------------------------------------
UtcDate::UtcDate (const UtcDate &date)
  : Date(date)
{
}

//------------------------------------------------------------------------------
//  UtcDate operator= (const UtcDate &date)
//------------------------------------------------------------------------------
UtcDate UtcDate::operator= (const UtcDate &date)
{
    yearD = date.yearD;
    monthD = date.monthD;
    dayD = date.dayD;
    secondsOfDayD = date.secondsOfDayD;
    return *this;
}

//------------------------------------------------------------------------------
//  ~UtcDate ()
//------------------------------------------------------------------------------
UtcDate::~UtcDate ()
{
}

//  //------------------------------------------------------------------------------
//  //  Real operator- (const UtcDate &date) const
//  //------------------------------------------------------------------------------
//  Real UtcDate::operator- (const UtcDate &date) const
//  {
//      Real offset;
   
//      A1Mjd t1(yearD, monthD, dayD, secondsOfDayD);
//      A1Mjd t2(date.yearD, date.monthD, date.dayD, date.secondsOfDayD);
   
//      //loj:offset = t1 - t2;
//      offset = t1.Subtract(t2);
//      return offset;
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate operator+ (const Real seconds) const
//  //------------------------------------------------------------------------------
//  UtcDate UtcDate::operator+ (const Real seconds) const
//  {   
//      A1Mjd tempTime(*this);

//      tempTime = tempTime + seconds;
//      return UtcDate(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate& operator+= (const Real seconds)
//  //------------------------------------------------------------------------------
//  UtcDate& UtcDate::operator+= (const Real seconds)
//  {
//      UtcDate utcDate;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime + seconds;
//      utcDate = UtcDate(tempTime);
//      *this = utcDate;
//      return *this;
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate operator- (const Real seconds) const
//  //------------------------------------------------------------------------------
//  UtcDate UtcDate::operator- (const Real seconds) const
//  {
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      return UtcDate(tempTime);
//  }

//  //------------------------------------------------------------------------------
//  //  UtcDate& operator-= (const Real seconds)
//  //------------------------------------------------------------------------------
//  UtcDate& UtcDate::operator-= (const Real seconds)
//  {
//      UtcDate utcDate;
//      A1Mjd tempTime(*this);

//      tempTime = tempTime - seconds;
//      utcDate = UtcDate(tempTime);
//      *this = utcDate;
//      return *this;
//  }
//------------------------------------------------------------------------------
//  A1Date ToA1Date()
//------------------------------------------------------------------------------
/**
 * @note The two time systems label time differently.  At any given moment,
 *       the A.1 system is several seconds ahead of the UTC system.  This offset
 *       is constant between leap insertions.  For example, the instant of time
 *       labeled July 1, 1992, 12:00:27.0343817 in the A.1 system will be labeled
 *       July 1, 1992, 12:00:00 (Noon) in the UTC system.
 */
//------------------------------------------------------------------------------
//  A1Date UtcDate::ToA1Date()
//  {
//      A1Date a1Date;
//      CalDate tempDate;
   
//      // convert UTC date to equivalent A1 date
//      A1Mjd utcMjd(*this); //loj: should subtract leap seconds when A1Mjd is constructed
//      //a1Date = A1Date(utcMjd); //loj: compile error

//      return utcMjd.ToA1Date();
    
//  }


//------------------------------------------------------------------------------
//  Real ToA1Mjd() const
//------------------------------------------------------------------------------
/**
 * Converts from UtcDate to A1 Modified Julian date. 
 *
 * @return A1 Modified Julian Date
 */
//------------------------------------------------------------------------------
Real UtcDate::ToA1Mjd() const
{
   Integer year   = GetYear();
   Integer month  = GetMonth();
   Integer day    = GetDay();
   Integer hour   = GetHour();
   Integer minute = GetMinute();
   Real second    = GetSecond();

   // Convert to Modified Julian date
   Real utcmjd = ModifiedJulianDate(year,month,day,hour,minute,second);
   // Convert to A1Mjd
   Real a1Mjd = TimeSystemConverter::Instance()->Convert(utcmjd, TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);
   return a1Mjd;
}
