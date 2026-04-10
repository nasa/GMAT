//$Id$
//------------------------------------------------------------------------------
//                                TimeTypes
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
// Author: K. Mueller, L. Jun
// Created: 1995/08/28 for GSS project
// Modified:
//   2003/09/12 Linda Jun - Defined namespace GmatTimeUtil. Added A1MJD_OF_J2000.
//
/**
 * Provides declarations for date & time types.
 */
//------------------------------------------------------------------------------
#ifndef TimeTypes_hpp
#define TimeTypes_hpp

#include "utildefs.hpp"

typedef Real     UtcMjd;
typedef Real     Ut1Mjd;
typedef Integer  YearNumber;

typedef Integer  DayOfYear;
typedef Integer  MonthOfYear;
typedef Integer  DayOfMonth;
typedef Integer  HourOfDay;
typedef Integer  MinuteOfHour;

namespace GmatTimeUtil
{
   class GMATUTIL_API CalDate
   {
      public:
      CalDate(YearNumber y, MonthOfYear mo, DayOfMonth d,
              HourOfDay h, MinuteOfHour m, Real s)
         : year(y), month(mo), day(d), hour(h), minute(m), second(s)
         {};

      CalDate() : year(1941), month(1), day(5), hour(0), minute(0), second(0.0) {};
      
      YearNumber   year;
      MonthOfYear  month;
      DayOfMonth   day;
      HourOfDay    hour;
      MinuteOfHour minute;
      Real         second;
   };
   
   class GMATUTIL_API ElapsedDate
   {
      public:
      ElapsedDate(Integer d, Integer h, Integer m, Real s)
         : days(d), hours(h), minutes(m), seconds(s)
         {};

      ElapsedDate() : days(0), hours(0), minutes(0), seconds(0.0) {};

      Integer days;
      Integer hours;
      Integer minutes;
      Real    seconds;
   };
   
   bool GMATUTIL_API IsValidMonthName(const std::string &str);
   std::string GMATUTIL_API GetMonthName(Integer month);
   Integer GMATUTIL_API GetMonth(const std::string &monthName);
   std::string GMATUTIL_API FormatCurrentTime(Integer format = 1);
   std::string GMATUTIL_API GetGregorianFormat();
}
#endif // TimeTypes_hpp
