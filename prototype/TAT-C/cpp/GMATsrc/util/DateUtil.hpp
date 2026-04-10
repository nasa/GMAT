//$Id$
//------------------------------------------------------------------------------
//                                    DateUtil
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
// Created: 1995/10/18
//
/**
 * This class provides conversions among various ways of representing calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef DateUtil_hpp
#define DateUtil_hpp

#include "TimeTypes.hpp" // for ElapsedDays, YearNumber, MonthOfYear, DayOfMonth
#include "GmatConstants.hpp"
#include "GmatTime.hpp"

class GMATUTIL_API DateUtil
{
public:
   // --------------------------------------------------------------------------
   // static data
   // --------------------------------------------------------------------------
   // These are the lower and upper bounds for epoch.
   // GMAT's algorithms are only valid over this time range.
   // --------------------------------------------------------------------------
   static const std::string EARLIEST_VALID_GREGORIAN;
   static const std::string LATEST_VALID_GREGORIAN;
   static const std::string EARLIEST_VALID_MJD;
   static const std::string LATEST_VALID_MJD;

   static const Real        EARLIEST_VALID_MJD_VALUE;
   static const Real        LATEST_VALID_MJD_VALUE;

   // --------------------------------------------------------------------------
   // static methods
   // --------------------------------------------------------------------------
   static Integer       JulianDay(YearNumber year, MonthOfYear month,
                                  DayOfMonth day);
   
   static std::string   FormatGregorian(YearNumber year, MonthOfYear month,
                                        DayOfMonth day, Integer hour, Integer minute,
                                        Real second);

   static bool          IsValidGregorian(const std::string &str, bool checkDate = false);

   friend Real GMATUTIL_API JulianDate(YearNumber year, MonthOfYear month,
                          DayOfMonth day, Integer hour, Integer minute,
                          Real second);
   
   friend Real GMATUTIL_API ModifiedJulianDate(YearNumber year, MonthOfYear month,
                                  DayOfMonth day, Integer hour, Integer minute,
                                  Real second, Real refEpochJD);  // =
                                  // GmatTimeConstants::JULIAN_DATE_OF_010541)
   
   friend GmatTime GMATUTIL_API ModifiedJulianDateGT(YearNumber year, MonthOfYear month,
                                  DayOfMonth day, Integer hour, Integer minute,
                                  Real second, Real refEpochJD);  // =
                                  // GmatTimeConstants::JULIAN_DATE_OF_010541)
   
   friend void GMATUTIL_API UnpackDate(Real packedDate, Integer& year, Integer& month, 
                          Integer& day);        // input in YYYYMMDD. 
   friend void GMATUTIL_API UnpackDateWithDOY(Real packedDate, Integer& year, Integer& day);
   
   // input in YYYYDDD. 
   friend void GMATUTIL_API UnpackTime(Real packedTime, Integer& hour, Integer& minute,
                          Real& second);        // input in hhmmssnnn.
   friend void GMATUTIL_API ToMonthDayFromYearDOY(Integer year, Integer dayOfYear,
                                     Integer& month, Integer& day);
   friend Integer GMATUTIL_API ToDOYFromYearMonthDay(Integer year, Integer month,
                                        Integer day);
   friend Real GMATUTIL_API ToSecondsOfDayFromHMS(Integer hour, Integer minute,
                                     Real second);
   friend void GMATUTIL_API ToHMSFromSecondsOfDay(Real secsOfDay, Integer& hour,
                                     Integer& minute, Real& second);
   friend bool GMATUTIL_API IsValidTime(Integer year, Integer month, Integer day,
                           Integer hour, Integer minute, Real second);

   friend bool GMATUTIL_API IsLeapYear(Integer year);


protected:

   // --------------------------------------------------------------------------
   // static data
   // --------------------------------------------------------------------------
   // These must match the values of the static const strings/values above
   // --------------------------------------------------------------------------
   static const Integer     MIN_YEAR;
   static const Integer     MIN_MONTH;
   static const Integer     MIN_DAY;
   static const Integer     MIN_HOUR;
   static const Integer     MIN_MINUTE;
   static const Real        MIN_SEC;

   static const Integer     MAX_YEAR;
   static const Integer     MAX_MONTH;
   static const Integer     MAX_DAY;
   static const Integer     MAX_HOUR;
   static const Integer     MAX_MINUTE;
   static const Real        MAX_SEC;
  
};

// Inject the friend function names into the namespace
Real GMATUTIL_API JulianDate(YearNumber year, MonthOfYear month,
                       DayOfMonth day, Integer hour, Integer minute,
                       Real second);
Real GMATUTIL_API ModifiedJulianDate(YearNumber year, MonthOfYear month,
                               DayOfMonth day, Integer hour, Integer minute,
                               Real second, Real refEpochJD = GmatTimeConstants::JULIAN_DATE_OF_010541);
GmatTime GMATUTIL_API ModifiedJulianDateGT(YearNumber year, MonthOfYear month,
                               DayOfMonth day, Integer hour, Integer minute,
                               Real second, Real refEpochJD = GmatTimeConstants::JULIAN_DATE_OF_010541);
void GMATUTIL_API UnpackDate(Real packedDate, Integer& year, Integer& month,
                       Integer& day);        // input in YYYYMMDD.
void GMATUTIL_API UnpackDateWithDOY(Real packedDate, Integer& year, Integer& day);
void GMATUTIL_API UnpackTime(Real packedTime, Integer& hour, Integer& minute,
                       Real& second);        // input in hhmmssnnn.
void GMATUTIL_API ToMonthDayFromYearDOY(Integer year, Integer dayOfYear,
                                  Integer& month, Integer& day);
Integer GMATUTIL_API ToDOYFromYearMonthDay(Integer year, Integer month,
                                     Integer day);
Real GMATUTIL_API ToSecondsOfDayFromHMS(Integer hour, Integer minute,
                                  Real second);
void GMATUTIL_API ToHMSFromSecondsOfDay(Real secsOfDay, Integer& hour,
                                  Integer& minute, Real& second);
bool GMATUTIL_API IsValidTime(Integer year, Integer month, Integer day,
                        Integer hour, Integer minute, Real second);
bool GMATUTIL_API IsLeapYear(Integer year);

#endif
