//------------------------------------------------------------------------------
//                           AbsoluteDate
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
// Created: 2016.05.05
//
/**
 * Implementation of the AbsoluteDate class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "DateUtil.hpp"
#include "AbsoluteDate.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "TATCException.hpp" // need new specific exception class?
#include "MessageInterface.hpp"

//#define DEBUG_GREGORIAN_DATE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer AbsoluteDate::DAYS_PER_MONTH[12] =
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   
const Real AbsoluteDate::JD_1900 = 2415019.5;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  AbsoluteDate()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//---------------------------------------------------------------------------
AbsoluteDate::AbsoluteDate() :
   currentDate         (GmatTimeConstants::JD_OF_J2000)
{
}

//------------------------------------------------------------------------------
//  AbsoluteDate(const AbsoluteDate &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param copy the object to copy
 */
//---------------------------------------------------------------------------
AbsoluteDate::AbsoluteDate(const AbsoluteDate &copy) :
   currentDate         (copy.currentDate)
{
}

//------------------------------------------------------------------------------
//  AbsoluteDate & operator=(const AbsoluteDate &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the AbsolutDate class.
 * 
 * @param copy the object to copy
 */
//---------------------------------------------------------------------------
AbsoluteDate& AbsoluteDate::operator=(const AbsoluteDate &copy)
{
   if (&copy == this)
      return *this;
   
   currentDate         = copy.currentDate;
   
   return *this;
}

//------------------------------------------------------------------------------
//  ~AbsoluteDate()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//---------------------------------------------------------------------------
AbsoluteDate::~AbsoluteDate()
{
}

//------------------------------------------------------------------------------
//  void SetGregorianDate(Integer year, Integer month,  Integer day,
//                        Integer hour, Integer minute, Real second)
//------------------------------------------------------------------------------
/**
 * Sets the Gregorian date.
 * 
 * @param year   the year
 * @param month  the month
 * @param day    the day
 * @param hour   the hour
 * @param minute the minute
 * @param second the seconds 
 */
//---------------------------------------------------------------------------
void  AbsoluteDate::SetGregorianDate(Integer year, Integer month,  Integer day,
                                     Integer hour, Integer minute, Real second)
{
   // Sets Gregorian date from input
   if (year < 1900 || year > 2100)
      throw TATCException("Invalid Year provided to AbsoluteDate()\n");

   if (month < 0 || month > 12)
      throw TATCException("Invalid Month provided to AbsoluteDate()\n");

   bool isLeap = IsLeapYear(year); // DateUtil
   
   if (day < 0 || day >= DAYS_PER_MONTH[month-1])
      if (isLeap && month == 2 && day != 29)
         throw TATCException("Invalid Month provided to AbsoluteDate()\n");

   if (hour < 0 || hour >= 24)
      throw TATCException("Invalid Hour provided to AbsoluteDate()\n");

   if (minute < 0 || minute >= 60)
      throw TATCException("Invalid Hour provided to AbsoluteDate()\n");

   if (second < 0.0 || second >= 60.0)  // what about leap seconds??
      throw TATCException("Invalid Seconds provided to AbsoluteDate()\n");

   currentDate = GregorianToJulianDate(year,month,day,hour,minute,second);
}

//------------------------------------------------------------------------------
// void SetJulianDate(Real jd)
//------------------------------------------------------------------------------
/**
 * Sets the Julian date.
 * 
 * @param jd   the Julian date
 */
//---------------------------------------------------------------------------
void AbsoluteDate::SetJulianDate(Real jd)
{
   currentDate = jd;
}

//------------------------------------------------------------------------------
// Real GetJulianDate() const
//------------------------------------------------------------------------------
/**
 * Returns the Julian date.
 * 
 * @return   the Julian date
 */
//---------------------------------------------------------------------------
Real AbsoluteDate::GetJulianDate() const
{
   return currentDate;
}

//------------------------------------------------------------------------------
// Rvector6 GetGregorianDate()
//------------------------------------------------------------------------------
/**
 * Returns the Gregorian date.
 * 
 * @return   the Gregorian date
 */
//---------------------------------------------------------------------------
Rvector6 AbsoluteDate::GetGregorianDate()
{
   
   Integer year, month, day, hour, min;
   Real    second;
   Real    minute;
   
   Real    jd      = currentDate;
   Real    t1900   = (jd - JD_1900) / GmatTimeConstants::DAYS_PER_YEAR;
   year            = 1900 + GmatMathUtil::Fix(t1900);
   Integer leapYrs = GmatMathUtil::Fix((year - 1900 - 1)*0.25);
   Real days       = (jd - JD_1900) - ((year - 1900) * 365 + leapYrs);
   
   // Handle leap year
   if (days < 1)
   {
      year--;
      leapYrs = GmatMathUtil::Fix((year - 1900 - 1)*0.25);
      days    = (jd - JD_1900) - ((year - 1900) * 365 + leapYrs);
   }

   Real    dayOfYr    = GmatMathUtil::Fix(days);
   Integer dayOfYrInt = (Integer) dayOfYr;  
   
   ToMonthDayFromYearDOY(year, dayOfYrInt, month, day);  // DateUtil
   #ifdef DEBUG_GREGORIAN_DATE
      MessageInterface::ShowMessage(
            "So far, year = %d, month = %d, day = %d, and (dayOfYr, dayOfYrInt,days) = (%12.10f %d %12.10f)\n",
            year, month, day, dayOfYr, dayOfYrInt, days);
   #endif
   Real tau = (days - dayOfYr) * 24;
   hour     = GmatMathUtil::Fix(tau);
   minute   = GmatMathUtil::Fix((tau - hour) * 60);
   min      = (Integer) minute;
   second   = (tau - hour - minute/60) * GmatTimeConstants::SECS_PER_HOUR;
   #ifdef DEBUG_GREGORIAN_DATE
      MessageInterface::ShowMessage(
            "   and then, tau = %12.10f, hour = %d, min = %d, second = %16.14f\n",
            tau, hour, min, second);
   #endif
   
   Rvector6 result(year, month, day, hour, min, second);
   return result;
}

//------------------------------------------------------------------------------
// void Advance(Real stepInSec)
//------------------------------------------------------------------------------
/**
 * Advances the date by the number of seconds specified.
 * 
 * @param  stepInSec   stepsize (seconds)
 */
//---------------------------------------------------------------------------
void AbsoluteDate::Advance(Real stepInSec)
{
   currentDate += stepInSec / GmatTimeConstants::SECS_PER_DAY;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real GregorianToJulianDate(Integer year, Integer month,  Integer day,
//                            Integer hour, Integer minute, Real second)
//------------------------------------------------------------------------------
/**
 * Converts a Gregorian date to a Julian date.
 * 
 * @param year   the year
 * @param month  the month
 * @param day    the day
 * @param hour   the hour
 * @param minute the minute
 * @param second the seconds 
 * 
 * @return the Julian date
 */
//---------------------------------------------------------------------------
Real AbsoluteDate::GregorianToJulianDate(Integer year, Integer month,  Integer day,
                                         Integer hour, Integer minute, Real second)
{
//   // Converts a Gregorian date to a Julian date
//   Real jDay = 367 * year - GmatMathUtil::Fix(7 *
//               (year + GmatMathUtil::Fix((month + 9) / 12)) / 4) +
//               GmatMathUtil::Fix(275 * month / 9) + day + 1721013.5;
//   Real partOfDay = ((second/60 + minute) / 60 + hour) / 24;
//   Real julianDate = (jDay) + partOfDay;
//   return julianDate;
   return JulianDate(year, month, day, hour, minute, second);
}

//------------------------------------------------------------------------------
//  AbsoluteDate* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the AbsoluteDate.
 *
 * @return clone of the AbsoluteDate.
 */
//------------------------------------------------------------------------------
AbsoluteDate* AbsoluteDate::Clone() const
{
   return (new AbsoluteDate(*this));
}

