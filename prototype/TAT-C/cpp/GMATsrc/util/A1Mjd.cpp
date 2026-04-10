//$Id$
//------------------------------------------------------------------------------
//                                    A1Mjd
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
// Created: 2003/09/15
// Modified: 2004/05/06  J. Gurganus - See A1Mjd.hpp for details.
//
/**
 * This class provides A1 Modified Julian Date(MJD). The zero date of the MJD
 * 12 noon on January 5th, 1941.
 */
//------------------------------------------------------------------------------
#include <sstream>                 // for stringstream
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "A1Date.hpp"
#include "TimeTypes.hpp"           // for TimeConst::, UtcMjd
#include "RealUtilities.hpp"       // for Round(), IsEqual(), Floor()
#include "MessageInterface.hpp"

//#define DEBUG_LEAP_SECONDS
//#define DEBUG_TOA1DATE

using namespace GmatTimeUtil;
using namespace GmatTimeConstants;
using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const std::string A1Mjd::DATA_DESCRIPTIONS[NUM_DATA] = 
{
   "A1 Modified Julian Date"
};

const A1Mjd A1Mjd::J2000      = A1Mjd(A1MJD_OF_J2000);
const Real  A1Mjd::mTolerance = GmatRealConstants::REAL_EPSILON;


//------------------------------------------------------------------------------
// A1Mjd()
//------------------------------------------------------------------------------
/**
 * Default constructor. Creates A1Mjd time of J2000.
 */
//------------------------------------------------------------------------------
A1Mjd::A1Mjd()
{
   mMjd = A1MJD_OF_J2000;
}

//------------------------------------------------------------------------------
// A1Mjd(const Real &val)
//------------------------------------------------------------------------------
/**
 * Constructor. Creates A1Mjd time from Real value.
 *
 * @param <val> the value to create an object.
 */
//------------------------------------------------------------------------------
A1Mjd::A1Mjd(const Real &val)
{
   mMjd = val;
}

//------------------------------------------------------------------------------
// A1Mjd(const Integer year, const Integer month, const Integer day,
//       const Real &secsOfDay)
//------------------------------------------------------------------------------
//  A1Mjd::A1Mjd(const Integer year, const Integer month, const Integer day,
//               const Real &secsOfDay)
//  {
//     Integer hour, minute;
//     Real second;

//     hour = (Integer)(secondsOfDay / GmatMathConstants::SECS_PER_DAY);
//     minute = (Integer)(Rem(secondsOfDay, GmatMathConstants::SECS_PER_DAY) / 60.0);
//     second = Rem(Rem(secondsOfDay, GmatMathConstants::SECS_PER_DAY), 60.0);

//     // Compute seconds from reference
//     secondsFromReferenceD =
//        SecondsSinceReference (year, month, day, hour, minute, second);
//  }

//------------------------------------------------------------------------------
// A1Mjd(const A1Mjd &a1mjd)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <a1mjd> the object to be copied.
 */
//------------------------------------------------------------------------------
A1Mjd::A1Mjd(const A1Mjd &a1mjd)
{
   mMjd = a1mjd.mMjd;
}

//------------------------------------------------------------------------------
// A1Mjd& operator= (const A1Mjd &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object to be copied.
 * @return reference to this object.
 */
//------------------------------------------------------------------------------
A1Mjd& A1Mjd::operator= (const A1Mjd &right)
{
   if (this != &right)
      mMjd = right.mMjd;

   return *this;
}

//------------------------------------------------------------------------------
// A1Mjd* Clone() const
//------------------------------------------------------------------------------
A1Mjd* A1Mjd::Clone() const
{
   return (new A1Mjd(*this));
}

//------------------------------------------------------------------------------
// ~A1Mjd()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
A1Mjd::~A1Mjd()
{
}

//------------------------------------------------------------------------------
// A1Mjd operator+ (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Adds an object and return a new object.
 *
 * @return a new object.
 */
//------------------------------------------------------------------------------
A1Mjd A1Mjd::operator+ (const A1Mjd &right) const
{
   return A1Mjd(mMjd + right.mMjd);
}

//------------------------------------------------------------------------------
// A1Mjd operator- (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Subtracts a object and return a new object.
 *
 * @return a new object.
 */
//------------------------------------------------------------------------------
A1Mjd A1Mjd::operator- (const A1Mjd &right) const
{
   return A1Mjd(mMjd - right.mMjd);
}

//------------------------------------------------------------------------------
// const A1Mjd& operator+= (const A1Mjd &right)
//------------------------------------------------------------------------------
/**
 * Adds an object and return the same object.
 */
//------------------------------------------------------------------------------
const A1Mjd& A1Mjd::operator+= (const A1Mjd &right)
{
   mMjd += right.mMjd;
   return *this;
}

//------------------------------------------------------------------------------
// const A1Mjd& operator-= (const A1Mjd &right)
//------------------------------------------------------------------------------
/**
 * Subtracts an object and return the same object.
 */
//------------------------------------------------------------------------------
const A1Mjd& A1Mjd::operator-= (const A1Mjd &right)
{
   mMjd -= right.mMjd;
   return *this;
}

//------------------------------------------------------------------------------
// A1Mjd operator+ (const Real &right) const
//------------------------------------------------------------------------------
/**
 * Adds a Real value and return a new object.
 */
//------------------------------------------------------------------------------
A1Mjd A1Mjd::operator+ (const Real &right) const
{
   return A1Mjd(mMjd + right);
}

//------------------------------------------------------------------------------
// A1Mjd operator- (const Real &right) const
//------------------------------------------------------------------------------
/**
 * Subtracts a Real value and return a new object.
 */
//------------------------------------------------------------------------------
A1Mjd A1Mjd::operator- (const Real &right) const
{
   return A1Mjd(mMjd - right);
}

//------------------------------------------------------------------------------
// const A1Mjd& operator+= (const Real &right)
//------------------------------------------------------------------------------
/**
 * Adds a Real value and return the same object.
 */
//------------------------------------------------------------------------------
const A1Mjd& A1Mjd::operator+= (const Real &right)
{
   mMjd += right;
   return *this;
}

//------------------------------------------------------------------------------
// const A1Mjd& operator-= (const Real &right)
//------------------------------------------------------------------------------
/**
 * Subtracts a Real value and return the same object.
 */
//------------------------------------------------------------------------------
const A1Mjd& A1Mjd::operator-= (const Real &right)
{
   mMjd -= right;
   return *this;
}

//------------------------------------------------------------------------------
// bool operator< (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is less than another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is less than other object value; false otherwise.
 */
//------------------------------------------------------------------------------
// bool A1Mjd::operator< (const A1Mjd &right) const
// {
//    return mMjd < right.mMjd;
// }

//------------------------------------------------------------------------------
// bool operator> (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is greater than another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is greater than other object value; false otherwise.
 */
//------------------------------------------------------------------------------
// bool A1Mjd::operator> (const A1Mjd &right) const
// {
//    return mMjd > right.mMjd;
// }

//------------------------------------------------------------------------------
// bool operator== (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is equal to another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is equal to another object value;
 *         false otherwise.
 */
//------------------------------------------------------------------------------
bool A1Mjd::operator== (const A1Mjd &right) const
{
   return IsEqual(mMjd, right.mMjd, mTolerance);
}

//------------------------------------------------------------------------------
// bool operator!= (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is not equal to another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is not equal to another object value;
 *         false otherwise.
 */
//------------------------------------------------------------------------------
bool A1Mjd::operator!= (const A1Mjd &right) const
{
   return !(IsEqual(mMjd, right.mMjd, mTolerance));
}

//------------------------------------------------------------------------------
// bool operator>= (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is greater than or equal to another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is greater than or equal to another object value;
 *         false otherwise.
 */
//------------------------------------------------------------------------------
// bool A1Mjd::operator>= (const A1Mjd &right) const
// {
//    return mMjd >= right.mMjd;
// }

//------------------------------------------------------------------------------
// bool operator<= (const A1Mjd &right) const
//------------------------------------------------------------------------------
/**
 * Determins if this object value is less than or equal to another object value.
 *
 * @param <right> another object.
 *
 * @return true if this object value is less than or equal to another object value;
 *         false otherwise.
 */
//------------------------------------------------------------------------------
// bool A1Mjd::operator<= (const A1Mjd &right) const
// {
//    return mMjd <= right.mMjd;
// }

// //------------------------------------------------------------------------------
// // Real Subtract(const A1Mjd &right) const
// //------------------------------------------------------------------------------
// /**
//  * Subtracts a object and return a Real value.
//  */
// //------------------------------------------------------------------------------
// Real A1Mjd::Subtract(const A1Mjd &right) const
// {
//    return (mMjd - right.mMjd);
// }

// //------------------------------------------------------------------------------
// // Real Subtract(const Real &right) const
// //------------------------------------------------------------------------------
// Real A1Mjd::Subtract(const Real &right) const
// {
//    return (mMjd - right);
// }

// //------------------------------------------------------------------------------
// // Real Get() const
// //------------------------------------------------------------------------------
// Real A1Mjd::Get() const
// {
//    return mMjd;
// }

// //------------------------------------------------------------------------------
// // void Set(Real val)
// //------------------------------------------------------------------------------
// void A1Mjd::Set(Real val)
// {
//    mMjd = val;
// }

//------------------------------------------------------------------------------
// UtcDate ToUtcDate()
//------------------------------------------------------------------------------
UtcDate A1Mjd::ToUtcDate()
{
   CalDate calDate;
   Real utcmMjd = ToUtcMjd();
   calDate = UtcMjdToCalDate(utcmMjd);
   return UtcDate(calDate);
}

//------------------------------------------------------------------------------
// A1Date ToA1Date(bool handleLeapSecond = false)
//------------------------------------------------------------------------------
/**
 * Converts from a A1 modified Julian date to A1Date (no leap seconds)
 */
//------------------------------------------------------------------------------
A1Date A1Mjd::ToA1Date(bool handleLeapSecond)
{
   CalDate calDate;
   calDate = UtcMjdToCalDate(mMjd, handleLeapSecond); // No leap seconds applied
   #ifdef DEBUG_TOA1DATE
   MessageInterface::ShowMessage
      ("year=%d, month=%d, day=%d, hour=%d, minute=%d, second=%f, handleLeapSecond = %s\n", calDate.year,
       calDate.month, calDate.day, calDate.hour, calDate.minute, calDate.second, (handleLeapSecond? "true" : "false"));
   #endif
   return A1Date(calDate);
}

//------------------------------------------------------------------------------
// Real UtcMjdToA1Mjd(const Real utcMjd)
//------------------------------------------------------------------------------
/**
 * Converts from UTC modified Julian date to a A1 modified Julian date
 */
//------------------------------------------------------------------------------
Real A1Mjd::UtcMjdToA1Mjd(const Real utcMjd)
{
   Real a1mjd;
   
   // Get A1-UTC offset in seconds
   a1mjd = GetA1UtcDiff(utcMjd);
   
   return (utcMjd + a1mjd/SECS_PER_DAY);
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
/**
 * @return number of data elements.
 */
//------------------------------------------------------------------------------
Integer A1Mjd::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
/**
 * @return data description pointer.
 */
//------------------------------------------------------------------------------
const std::string* A1Mjd::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
// std::string* ToValueStrings()
//------------------------------------------------------------------------------
/**
 * @return data value string pointer.
 */
//------------------------------------------------------------------------------
std::string* A1Mjd::ToValueStrings()
{
   std::stringstream ss("");
   ss << mMjd;
   stringValues[0] = ss.str();
   return stringValues;   
}

//------------------------------------------------------------------------------
// Real ToUtcMjd()
//------------------------------------------------------------------------------
UtcMjd A1Mjd::ToUtcMjd()
{
   Real a1mjd = mMjd;
   UtcMjd utcmjd;
   Real   a1utc1;
   Real   a1utc2;
   Real   a1utc3;

   // Get the A.1-UTC offset at this A.1 time... 
   a1utc1 = GetA1UtcDiff(a1mjd);
   
   utcmjd = a1mjd - a1utc1/SECS_PER_DAY;

   // Test the offset at this new time... 
   a1utc2 = GetA1UtcDiff(utcmjd);

   // If the offset changed, subtract the new offset... 
   if (a1utc2 != a1utc1)
   {
      utcmjd = a1mjd - a1utc2/SECS_PER_DAY;

      // Test the offset at this new time... 
      a1utc3 = GetA1UtcDiff(utcmjd); //get_a1_utc_diff(utcmjd, &a1utc3);

      // If the offset changed, subtract the new offset... 
      if (a1utc3 != a1utc2)
      {
         utcmjd = a1mjd - a1utc3/SECS_PER_DAY;
      }
   }

   return utcmjd;
}


//---------------------------------
// private
//---------------------------------


//------------------------------------------------------------------------------
// Real GetA1UtcDiff(const UtcMjd &utcmjd)
//------------------------------------------------------------------------------
Real A1Mjd::GetA1UtcDiff(const UtcMjd &utcmjd)
{
   //loj: analytic for build1
   
   return GetA1UtcDiffAnalytic(utcmjd);
}

//------------------------------------------------------------------------------
// Real GetA1UtcDiffAnalytic(const UtcMjd &utcmjd)
//------------------------------------------------------------------------------
/**
 * Compute the A.1-UTC offset analytically.
 * This function was developed by taking the modified julian dates and the a1utc
 * offset for these dates and fitting a line to the data.  The data was obtained
 * from the Timing Coefficents File by using the PC-RSL routine DIFFUTC
 *
 * @param <utcmjd> input UTC modified julian date
 * @return A1 and UTC offset
 * @note Reused from Swingby get_a1_utc_diff_analytic()
 */
//------------------------------------------------------------------------------
Real A1Mjd::GetA1UtcDiffAnalytic(const UtcMjd &utcmjd)
{
   Real mjd;    // date used for line fit
   Real a1utc;  // difference in seconds between A.1 and UTC
                // (add to UTC to give A.1)
   Real diff;

   // Round to nearest day...
   mjd = Round(utcmjd);

   // Calculate integer number of seconds...
   a1utc = Floor(-9.24696 + 0.001925*mjd);

   // Add TAI minus A.1 offset...
   diff = a1utc + 0.03437805175781120;
   return diff;
}

//------------------------------------------------------------------------------
// CalDate A1MjdToCalDate(const A1Mjd &a1mjd) const
//------------------------------------------------------------------------------
CalDate A1Mjd::A1MjdToCalDate(const A1Mjd &a1mjd)
{
   Real  utcmjd;
   Real  a1utc1;
   Real  a1utc2;
   Real  a1utc3;
   Real  leapSecond = 0.0;
   CalDate calDate;
   
   a1utc1 = GetA1UtcDiff(a1mjd.mMjd);

   utcmjd = a1mjd.mMjd - a1utc1/SECS_PER_DAY;

   // Test the offset at this new time... 
   a1utc2 = GetA1UtcDiff(utcmjd);

   // If the offset changed, subtract the new offset... 
   if(a1utc2 != a1utc1)
   {
      utcmjd = a1mjd.mMjd - a1utc2/SECS_PER_DAY;

      // Test the offset at this new time... 
      a1utc3 = GetA1UtcDiff(utcmjd);

      // If the offset changed, subtract the new offset... 
      if(a1utc3 != a1utc2)
      {
         utcmjd = a1mjd.mMjd - a1utc3/SECS_PER_DAY;

         // We are on the leap second! 
         leapSecond = 1.0;
      }
   }

   calDate = UtcMjdToCalDate(utcmjd);
   calDate.second += leapSecond;
   return calDate;    
}

//------------------------------------------------------------------------------
// CalDate UtcMjdToCalDate(const UtcMjd &utcmjd, bool handleLeapSecond = false)
//------------------------------------------------------------------------------
// modification:
//    2003/09/24 Linda Jun - Modified Swingby utcmjd_to_time_array() to return
//               CalDate object.
//------------------------------------------------------------------------------
/**
 * Converts a utc modified julian date to a calendar date in the date/time
 * handling standard format using the fliegel & van flandern algorithm.
 * Reference:
 *   'communications of the acm', volume 2, number 10, october 1968,
 *   p. 657, (the fliegel & van flandern algorithm)
 *
 * @param <utcmjd> input utc modified julian date, days
 * @return UtcDate object
 * @note The modified julian date is the full julian day modified by
 *       subtracting JD_JAN_5_1941.
 *
 *       A day containing a leap second will appear as though it has only
 *       GmatTimeConstants::SECS_PER_DAY seconds, though the utc (universal time coordinate)
 *       modified julian date covers the entire day.
 */
//------------------------------------------------------------------------------
CalDate A1Mjd::UtcMjdToCalDate(const UtcMjd &utcmjd, bool handleLeapSecond)
{
   Real timeArray[6];
   Real seconds, modjul;
   long mjd, l, n, i, j;
   
   // Used in converting mjd seconds to whole number (moved from TimeTypes.hpp)
   // Changed 1.0e6 to 1.0e4 to avoid 59.99999999999 (LOJ: 2009.11.09)
   // A1MJD of 25131.171227249783 gives 26 Oct 2009 16:05:60.000 instead of
   //    16:06:00.000
   // Changed to 1.0e3 for Bug 1631 (LOJ: 2009.11.13)
   //static const Real MJDSEC_TO_WHOLE_NUM = 1.0e4;
   static const Real MJDSEC_TO_WHOLE_NUM = 1.0e3;
   
   // add a half day (julian to gregorian) offset to the input utcmjd
   if (handleLeapSecond)
      modjul = (utcmjd + 0.50) - 1.0/SECS_PER_DAY;
   else
      modjul = (utcmjd + 0.50);
   
   // obtain the offset modified number of julian days
   mjd = (long)modjul;

#ifdef DEBUG_LEAP_SECONDS
   MessageInterface::ShowMessage("In UtcMjdToCalDate ... utcmjd = %12.10f\n", utcmjd);
   MessageInterface::ShowMessage("                       modjul = %12.10f\n", modjul);
   MessageInterface::ShowMessage("                       mjd    = %12.10f\n", mjd);
   MessageInterface::ShowMessage("                       handleLeapSecond = %s\n", (handleLeapSecond? "true" : "false"));
#endif
   // convert fractional part of day to seconds
   seconds = (modjul - (Real)mjd) * SECS_PER_DAY;
   
#ifdef DEBUG_LEAP_SECONDS
   MessageInterface::ShowMessage("                       seconds = %12.10f\n", seconds);
#endif
   // round fractional seconds at their accuracy limit to
   // prevent truncation errors during decomposition
   seconds = (NearestInt(seconds * MJDSEC_TO_WHOLE_NUM)) / MJDSEC_TO_WHOLE_NUM;
#ifdef DEBUG_LEAP_SECONDS
   MessageInterface::ShowMessage("                       seconds (adjusted) = %12.10f\n", seconds);
   MessageInterface::ShowMessage(" NOW checking for seconds >= SECS_PER_DAY(%12.10f)\n", SECS_PER_DAY);
#endif
   
   // if modjul is negative (pre 1/5/1941) and seconds are negative
   // (measured backwards from the end of the day); turn them around.
   if (modjul < 0.0 && seconds < 0.0)
      seconds += SECS_PER_DAY;
   
   // if the number of seconds exceeds the number of seconds in a day
   if (seconds >= SECS_PER_DAY)
   {
      seconds -= SECS_PER_DAY;
      mjd += 1;
#ifdef DEBUG_LEAP_SECONDS
      MessageInterface::ShowMessage(" **** setting seconds = %12.10f\n", seconds);
      MessageInterface::ShowMessage("      setting mjd     = %12.10f\n", mjd);
#endif
   }
   
   // divide hours out of total seconds
   timeArray[3] = Floor(seconds / SECS_PER_HOUR);
   
   // subtract hours out of total seconds
   seconds -= timeArray[3] * SECS_PER_HOUR;
   
   // divide minutes out of remaining seconds
   timeArray[4] = Floor(seconds / SECS_PER_MINUTE);

   // subtract out minutes to leave seconds
   seconds -= timeArray[4] * SECS_PER_MINUTE;
   
   // once more, round fractional seconds at accuracy limit
   timeArray[5] = NearestInt(seconds * MJDSEC_TO_WHOLE_NUM) / MJDSEC_TO_WHOLE_NUM;
   
   // calculate day, month, and year using the fliegel & van flandern algorithm
   l = (long)(mjd + (68569 + GmatTimeConstants::JD_JAN_5_1941));
   n = 4 * l / 146097;
   l -= (146097 * n + 3) / 4;
   i = 4000 * (l + 1) / 1461001;
   l = l - 1461 * i / 4 + 31;
   j = 80 * l / 2447;
   timeArray[2] = l - 2447 * j / 80;
   l = j / 11;
   timeArray[1] = j + 2 - 12 * l;
   timeArray[0] = 100 * (n - 49) + i + l;
   
   if (handleLeapSecond)
      timeArray[5] += 1.0;

#ifdef DEBUG_LEAP_SECONDS
   MessageInterface::ShowMessage(" Resulting time:\n");
   MessageInterface::ShowMessage("   year:    %12.10f\n", timeArray[0]);
   MessageInterface::ShowMessage("   month:   %12.10f\n", timeArray[1]);
   MessageInterface::ShowMessage("   day:     %12.10f\n", timeArray[2]);
   MessageInterface::ShowMessage("   hour:    %12.10f\n", timeArray[3]);
   MessageInterface::ShowMessage("   minute:  %12.10f\n", timeArray[4]);
   MessageInterface::ShowMessage("   seconds: %12.10f\n", timeArray[5]);
#endif
   return CalDate((YearNumber)timeArray[0], (MonthOfYear)timeArray[1],
                  (DayOfMonth)timeArray[2], (HourOfDay)timeArray[3],
                  (MinuteOfHour)timeArray[4], timeArray[5]);
}

//------------------------------------------------------------------------------
// Real SecondsSinceReference(Integer year, MonthOfYear month,
//                            DayOfMonth day, HourOfDay hour,
//                            MinuteOfHour minute, Real second)
//------------------------------------------------------------------------------
/*
 * Static function to computes the seconds since the reference time of the
 * time specified by the inputs
 *
 * @param <year> input year 
 * @param <month> input month of the year
 * @param <day> input day of the month
 * @param <hour> input hour of the day 
 * @param <minute> input minute of the hour 
 * @param <second> input seconds of the hour 
 *
 * @return seconds from the reference time
 */
//------------------------------------------------------------------------------
//  Real A1Mjd::SecondsSinceReference(Integer year, MonthOfYear month,
//                                    DayOfMonth day, HourOfDay hour,
//                                    MinuteOfHour minute, Real second)
//  {
//loj: 3/12/04 Changed ElapsedDays to Integer
//     Integer julianDay;
//     Integer daysSinceRef;
//     Real leapSecondsBtw;
//     Real seconds;

//     // Compute julian day of input date
//     julianDay = JulianDay(year, month, day);

//     // Compute leap seconds between
//     leapSecondsBtw = 
//        LeapSecondsBetween(UTCJulianDateOfReferenceD.dayNumber, julianDay);

//     // Compute days since reference
//     daysSinceRef = julianDay - UTCJulianDateOfReferenceD.dayNumber;

//     // Compute seconds from reference
//     seconds = (daysSinceRef * TimeConstants::secondsPerDay) 
//        + (hour * GmatMathConstants::SECS_PER_DAY)
//        + (minute * 60) + second + leapSecondsBtw;
//     return seconds;
//  }
