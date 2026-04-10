//$Id$
//------------------------------------------------------------------------------
//                                  Date
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
// Author: Linda Jun, L. Cisney
// Created: 1995/10/18 for GSS project (originally GSSDate)
// Modified: 2003/09/12 Linda Jun - See Date.hpp
//
/**
 * This class is abstrace base class which provides conversions among various
 * ways of representing calendar dates and times.
 */
//------------------------------------------------------------------------------
#include <sstream>              // for stringstream and ostringstream
#include "utildefs.hpp"
#include "TimeTypes.hpp"        // for Time constants
#include "Date.hpp"
#include "A1Mjd.hpp"
#include "DateUtil.hpp"         // for ToHMSFromSecondsOfDay()
#include "StringTokenizer.hpp"  // for calendar in string

#include <cstdlib>			// Required for GCC 4.3

//---------------------------------
// static data
//---------------------------------
const std::string Date::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Year", "Month", "Day", "Hour", "Minute", "Second"
};

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Integer GetYear() const
//------------------------------------------------------------------------------
Integer Date::GetYear() const
{
   return yearD;
}

//------------------------------------------------------------------------------
//  Integer GetMonth() const
//------------------------------------------------------------------------------
Integer Date::GetMonth() const
{
   return monthD;
}

//------------------------------------------------------------------------------
//  Integer GetDay() const
//------------------------------------------------------------------------------
Integer Date::GetDay() const
{
   return dayD;
}

//------------------------------------------------------------------------------
//  Real GetSecondsOfDay() const
//------------------------------------------------------------------------------
Real Date::GetSecondsOfDay() const
{
   return secondsOfDayD;
}

//------------------------------------------------------------------------------
// Integer Date::GetHour() const
//------------------------------------------------------------------------------
Integer Date::GetHour() const
{
   Integer hour;
   Integer min;
   Real sec;

   ToHMSFromSecondsOfDay(secondsOfDayD, hour, min, sec);
   return hour;
}

//------------------------------------------------------------------------------
// Integer Date::GetMinute() const
//------------------------------------------------------------------------------
Integer Date::GetMinute() const
{
   Integer hour;
   Integer min;
   Real sec;

   ToHMSFromSecondsOfDay(secondsOfDayD, hour, min, sec);
   return min;
}

//------------------------------------------------------------------------------
// Real Date::GetSecond() const
//------------------------------------------------------------------------------
Real Date::GetSecond() const
{
   Integer hour;
   Integer min;
   Real sec;

   ToHMSFromSecondsOfDay(secondsOfDayD, hour, min, sec);
   return sec;
}

//------------------------------------------------------------------------------
//  GmatTimeConstants::DayName GetDayName() const
//------------------------------------------------------------------------------
GmatTimeConstants::DayName Date::GetDayName() const
{
   const Integer JD_OF_010172 = 2441318;
   const GmatTimeConstants::DayName DAY_NAME_OF_010172  = GmatTimeConstants::SATURDAY;
   Integer dayNumber;

   dayNumber = (Integer)(DateUtil::JulianDay(yearD, monthD, dayD))
      - JD_OF_010172;

   dayNumber = (dayNumber + DAY_NAME_OF_010172) % 7;

   return (GmatTimeConstants::DayName)dayNumber;
}

//------------------------------------------------------------------------------
//  Integer GetDaysPerMonth() const
//------------------------------------------------------------------------------
Integer Date::GetDaysPerMonth() const
{
   if(IsLeapYear(yearD))
      return GmatTimeConstants::LEAP_YEAR_DAYS_IN_MONTH[monthD - 1];
   else
      return GmatTimeConstants::DAYS_IN_MONTH[monthD - 1];
}

//------------------------------------------------------------------------------
//  GmatTimeConstants::MonthName GetMonthName() const
//------------------------------------------------------------------------------
GmatTimeConstants::MonthName Date::GetMonthName() const
{
   return (GmatTimeConstants::MonthName)monthD;
}

//------------------------------------------------------------------------------
//  Real ToPackedCalendarReal() const
//------------------------------------------------------------------------------
/*
 * @return time in Real in the format of YYYYMMDD.HHMMSSmmm
 */
//------------------------------------------------------------------------------
Real Date::ToPackedCalendarReal() const
{
   Real ymd;
   Real hms;

   ToYearMonthDayHourMinSec(ymd, hms);
   
   return ymd + hms;
}

//------------------------------------------------------------------------------
//  Real ToPackedYYYMMDD() const
//------------------------------------------------------------------------------
/*
 * Converts time to YYYY(year - 1900) MM (month) and DD (day).
 *
 * @return time in Real in the format of YYYMMDD.0
 */
//------------------------------------------------------------------------------
Real Date::ToPackedYYYMMDD() const
{
   Real ymd;
   Real hms;
   
   ToYearMonthDayHourMinSec(ymd, hms);
   return ymd - 19000000.0;
}


//------------------------------------------------------------------------------
//  Real ToPackedHHMMSS() const
//------------------------------------------------------------------------------
/*
 * Converts time to calendar format and returns hour, minute, seconds, and
 * milliseconds.
 *
 * @return time in Real in the format of HHMMSS.mmm
 */
//------------------------------------------------------------------------------
Real Date::ToPackedHHMMSS() const
{
   Real ymd;
   Real hms;
   
   ToYearMonthDayHourMinSec(ymd, hms);
   return hms;
}


//------------------------------------------------------------------------------
// Real ToDayOfYear() const
//------------------------------------------------------------------------------
Real Date::ToDayOfYear() const
{
   Real dayOfYear = ToDOYFromYearMonthDay(yearD, monthD, dayD);
   return dayOfYear;
}


//------------------------------------------------------------------------------
//  std::string& ToPackedCalendarString()
//------------------------------------------------------------------------------
std::string& Date::ToPackedCalendarString()
{
   std::ostringstream ss("");
   ss.precision(9);
   ss.setf(std::ios::fixed);

   Real ymd;
   Real hms;

   ToYearMonthDayHourMinSec(ymd, hms);

   // Get date in YMD
   ss << ymd;
   StringTokenizer stringToken(ss.str(), ".");
   std::string tempString = stringToken.GetToken(0);

   // Get time in HMS
   ss.str("");
   ss << hms;
   stringToken.Set(ss.str(), ".");
   tempString = tempString + "." + stringToken.GetToken(1);

//    ss << (ymd + hms);
//   mPackedString = ss.str();
   mPackedString = tempString;

   return mPackedString;
}

//------------------------------------------------------------------------------
//  void ToYearDOYHourMinSec(Integer& year, Integer& dayOfYear,
//                           Integer& hour, Integer& minute, Real& second) const
//------------------------------------------------------------------------------
void Date::ToYearDOYHourMinSec(Integer& year, Integer& dayOfYear,
                               Integer& hour, Integer& minute, Real& second) const
{
   year = yearD;
   dayOfYear = ToDOYFromYearMonthDay(yearD, monthD, dayD);
   ToHMSFromSecondsOfDay(secondsOfDayD, hour, minute, second);
}

//------------------------------------------------------------------------------
//  void ToYearMonthDayHourMinSec(Integer& year, Integer& month, Integer& day,
//                              Integer& hour, Integer& minute, Real& second) const
//------------------------------------------------------------------------------
void Date::ToYearMonthDayHourMinSec(Integer& year, Integer& month, Integer& day,
                                  Integer& hour, Integer& minute, Real& second) const
{
   year = yearD;
   month = monthD;
   day = dayD;
   ToHMSFromSecondsOfDay(secondsOfDayD, hour, minute, second);
}

//------------------------------------------------------------------------------
//  void ToYearMonthDayHourMinSec(Real& year, Real& month, Real& day,
//                              Real& hour, Real& minute, Real& second) const
//------------------------------------------------------------------------------
void Date::ToYearMonthDayHourMinSec(Real& year, Real& month, Real& day,
                                  Real& hour, Real& minute, Real& second) const
{
   Integer iyear, imonth, iday, ihour, iminute;
   
   ToYearMonthDayHourMinSec(iyear, imonth, iday, ihour, iminute, second);
   year = (Real)iyear;
   month = (Real)imonth;
   day = (Real)iday;
   hour = (Real)ihour;
   minute = (Real)iminute;
}

//------------------------------------------------------------------------------
//  void ToYearMonthDayHourMinSec(Real& ymd, Real& hms) const
//------------------------------------------------------------------------------
/**
 * Returns time in YYYYMMDD.0 and HHMMSS.mmm format
 */
//------------------------------------------------------------------------------
void Date::ToYearMonthDayHourMinSec(Real& ymd, Real& hms) const
{
   Integer h;
   Integer m;
   Real    s;

   ToHMSFromSecondsOfDay(secondsOfDayD, h, m, s);
   ymd = (Real) (yearD * 10000.0 + monthD * 100.0 + dayD);
   hms = (Real) (h * 1.0e+07 + m * 100000.0) +  s * 1000.0;

   hms = hms/1.0e+09;

}

//------------------------------------------------------------------------------
// bool IsValid() const
//------------------------------------------------------------------------------
bool Date::IsValid() const
{
   return(IsValidTime(yearD,monthD,dayD,GetHour(),GetMinute(),GetSecond()));
}


//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Date::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Date::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
// std::string* ToValueStrings()
//------------------------------------------------------------------------------
std::string* Date::ToValueStrings()
{
   Integer hour;
   Integer min;
   Real sec;

   ToHMSFromSecondsOfDay(secondsOfDayD, hour, min, sec);
   std::stringstream ss("");

   ss << yearD;
   stringValues[0] = ss.str();

   ss.str("");
   ss << monthD;
   stringValues[1] = ss.str();

   ss.str("");
   ss << dayD;
   stringValues[2] = ss.str();

   ss.str("");
   ss << hour;
   stringValues[3] = ss.str();

   ss.str("");
   ss << min;
   stringValues[4] = ss.str();

   ss.str("");
   ss << sec;
   stringValues[5] = ss.str();

   return stringValues;
}

//---------------------------------
//  protected
//---------------------------------

//------------------------------------------------------------------------------
//  Date()
//------------------------------------------------------------------------------
Date::Date()
   : yearD(1941), monthD(1), dayD(5), secondsOfDayD(43167.85)
{
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer month, Integer day, Integer hour,
//       Integer minute, Real second)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer month, Integer day, Integer hour,
           Integer minute, Real second)
{
   // check time
   if(!IsValidTime(year, month, day, hour, minute, second))
   {
      throw TimeRangeError();
   }

   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer dayOfYear, Integer hour, Integer minute,
//       Real second)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer dayOfYear, Integer hour, Integer minute,
           Real second)
{
   yearD = year;
   ToMonthDayFromYearDOY(year, dayOfYear, monthD, dayD);

   // check time
   if(!IsValidTime(yearD, monthD, dayD, hour, minute, second))
   {
      throw TimeRangeError();
   }
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(Integer year, Integer month, Integer day, Real secondsOfDay)
//------------------------------------------------------------------------------
Date::Date(Integer year, Integer month, Integer day, Real secondsOfDay)
{
   Real    seconds;
   Integer hour, minute;

   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = secondsOfDay;

   // check time
   ToHMSFromSecondsOfDay(secondsOfDay, hour, minute, seconds);
   if(!IsValidTime(yearD, monthD, dayD, hour, minute, seconds))
   {
      throw TimeRangeError();
   }
}

//------------------------------------------------------------------------------
// Date(const GmatTimeUtil::CalDate &date)
//------------------------------------------------------------------------------
Date::Date(const GmatTimeUtil::CalDate &date)
{
   yearD = date.year;
   monthD = date.month;
   dayD = date.day;
   secondsOfDayD = date.hour * GmatTimeConstants::SECS_PER_HOUR +
      date.minute * GmatTimeConstants::SECS_PER_MINUTE + date.second;
}

//------------------------------------------------------------------------------
//  Date(const std::string& time)
//------------------------------------------------------------------------------
/**
 * @param <time> time in string form of "YYYYMMDD.hhmmssnnn"
 */
//------------------------------------------------------------------------------
Date::Date(const std::string& time)
{
   const char *tempTime = time.c_str();
   Integer datePart;
   Integer timePart;
   Integer year, month, day, hour, minute;
   Real    second;

   datePart = atoi(tempTime);
   tempTime = strstr(tempTime, ".");

   if (tempTime != NULL)
      timePart = atoi(tempTime + 1);
   else
      timePart = 1;

   try
   {
      UnpackDate(datePart, year, month, day);
      UnpackTime(timePart, hour, minute, second);
   }
   catch(TimeRangeError& tre)
   {
      throw tre;
   }
   yearD = year;
   monthD = month;
   dayD = day;
   secondsOfDayD = ToSecondsOfDayFromHMS(hour, minute, second);
}

//------------------------------------------------------------------------------
//  Date(const Date &date)
//------------------------------------------------------------------------------
Date::Date(const Date &date)
{
   yearD = date.yearD;
   monthD = date.monthD;
   dayD = date.dayD;
   secondsOfDayD = date.secondsOfDayD;
}

//------------------------------------------------------------------------------
//  ~Date()
//------------------------------------------------------------------------------
Date::~Date()
{
}

//------------------------------------------------------------------------------
//  bool operator> (const Date &date) const
//------------------------------------------------------------------------------
/**
 * Comparison operator >
 */
//------------------------------------------------------------------------------
bool Date::operator> (const Date &date) const
{
    if (yearD > date.yearD)
	return true;
    else if ( yearD == date.yearD && monthD > date.monthD )
	return true;
    else if ( yearD == date.yearD && monthD == date.monthD && dayD > date.dayD)
	return true;
    else if ( yearD == date.yearD && monthD == date.monthD && dayD == date.dayD
	      && secondsOfDayD > date.secondsOfDayD)
	return true;
    else
	return false;
}


//------------------------------------------------------------------------------
//  bool operator< (const Date &date) const
//------------------------------------------------------------------------------
/**
 * Comparison operator <
 */
//------------------------------------------------------------------------------
bool Date::operator< (const Date &date) const
{
    if (yearD < date.yearD)
	return true;
    else if ( yearD == date.yearD && monthD < date.monthD )
	return true;
    else if ( yearD == date.yearD && monthD == date.monthD && dayD < date.dayD)
	return true;
    else if ( yearD == date.yearD && monthD == date.monthD && dayD == date.dayD
	      && secondsOfDayD < date.secondsOfDayD)
	return true;
    else
	return false;
}

