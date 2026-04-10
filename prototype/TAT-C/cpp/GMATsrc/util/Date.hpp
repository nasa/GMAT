//$Id$
//------------------------------------------------------------------------------
//                                    Date
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
// Created: 1995/10/18 for GSS project (originally GSSDate)
// Modified: 2003/09/12 Linda Jun - Added new methods and member data to return
//           data descriptions and values.
/**
 * This class is abstrace base class which provides conversions among various
 * ways of representing calendar dates and times.
 */
//------------------------------------------------------------------------------
#ifndef Date_hpp
#define Date_hpp

#include "utildefs.hpp"
#include "BaseException.hpp"
#include "GmatConstants.hpp"
#include "TimeTypes.hpp"

class GMATUTIL_API Date
{
public:
   
    // exception(s)
   class TimeRangeError : public BaseException
         { public:  TimeRangeError(const std::string& message =
          "Date error: date or time out of specified range")
          : BaseException(message) {}; };

   class LeapYearError : public BaseException
         { public:  LeapYearError(const std::string& message =
          "Date error: day number is invalid for specified year")
          : BaseException(message) {}; };

    Integer GetYear() const;
    Integer GetMonth() const;
    Integer GetDay() const;
    Real    GetSecondsOfDay() const;
    
    Integer GetHour() const;
    Integer GetMinute() const;
    Real    GetSecond() const;
    
    GmatTimeConstants::DayName GetDayName() const;
    Integer GetDaysPerMonth() const;
    GmatTimeConstants::MonthName GetMonthName() const;
    
    Real ToPackedCalendarReal() const;      // YYYYMMDD.HHMMSSmmm
    Real ToPackedYYYMMDD() const;           // YYYMMDD.0
    Real ToPackedHHMMSS() const;            // HHMMSS.mmm
    Real ToDayOfYear() const;               // Day count of year
    std::string& ToPackedCalendarString();  // "YYYYMMDD.HHMMSSmmm"
    
    
    void ToYearDOYHourMinSec(Integer& year, Integer& dayOfYear, Integer& hour, 
                             Integer& minute, Real& second) const;
    void ToYearMonthDayHourMinSec(Integer& year, Integer& month, Integer& day, 
                                  Integer& hour, Integer& minute, Real& second) const;
    void ToYearMonthDayHourMinSec(Real& year, Real& month, Real& day, 
                                  Real& hour, Real& minute, Real& second) const;
    void ToYearMonthDayHourMinSec(Real& ymd, Real& hms) const;
    
    bool IsValid() const;
    
    Integer GetNumData() const;
    const std::string* GetDataDescriptions() const;
    std::string* ToValueStrings();
    
protected:
   
    Date();
    Date(Integer year, Integer month, Integer day, Integer hour, 
             Integer minute, Real second);
    Date(Integer year, Integer dayOfYear, Integer hour, Integer minute,
             Real second);
    Date(Integer year, Integer month, Integer day, Real secondsOfDay);
    Date(const GmatTimeUtil::CalDate &date);
    Date(const std::string &time); // "YYYYMMDD.HHMMSSmmm"
    Date(const Date &date);
    ~Date();
    
    bool  operator>  (const Date &date) const;
    bool  operator<  (const Date &date) const;
    
    Integer  yearD;
    Integer  monthD;
    Integer  dayD;
    Real     secondsOfDayD;
    
    std::string mPackedString;
    
    static const Integer NUM_DATA = 6;
    static const std::string DATA_DESCRIPTIONS[NUM_DATA];
    std::string stringValues[NUM_DATA];

private:
};
#endif //Date_hpp
