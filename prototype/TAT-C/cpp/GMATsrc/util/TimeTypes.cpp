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
// Author: Linda Jun, NASA/GSFC
// Created: 2006/05/05
//
/**
 * Provides declarations for date & time types.
 */
//------------------------------------------------------------------------------

#include "TimeTypes.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include <ctime>                   // for time()
#include <sstream>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/time.h>    // not available for MSVC?
#endif


//------------------------------------------------------------------------------
// bool IsValidMonthName(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if input string is valid month short name. Input month name should
 *   be month name abbreviation.  For example, "Jan", "Feb", etc.
 *
 * @param  str  input month string
 *
 * @return return true if input string is valid month name
 */
//------------------------------------------------------------------------------ 
bool GmatTimeUtil::IsValidMonthName(const std::string &str)
{
   for (int i=0; i<12; i++)
   {
      if (str == GmatTimeConstants::MONTH_NAME_TEXT[i])
         return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// std::string GetMonthName(Integer month)
//------------------------------------------------------------------------------
/*
 * Return corresponding Month Name for integer month.
 *
 * @param  month  input month
 *
 * @return corresponding Month Name for integer month.
 * 
 * @exception <TimeException> thrown if input month is out of range
 */
//------------------------------------------------------------------------------
std::string GmatTimeUtil::GetMonthName(Integer month)
{
   if (month < 1 || month > 12) 
   {
      throw TimeException
         ("Cannot get Month Name for " + GmatStringUtil::ToString(month));
   }
   
   return GmatTimeConstants::MONTH_NAME_TEXT[month-1];
}


//------------------------------------------------------------------------------
// Integer GetMonth(const std::string &monthName)
//------------------------------------------------------------------------------
/*
 * Return corresponding month for input Month Name. Input month name should
 *   be month name abbreviation.  For example, "Jan", "Feb", etc.
 *
 * @param  monthName  input month name
 *
 * @return corresponding Integer month for Month Name.
 *         -1, if name not found
 * 
 * @exception <TimeException> thrown if input month name is out of range
 */
//------------------------------------------------------------------------------
Integer GmatTimeUtil::GetMonth(const std::string &monthName)
{
   for (int i=0; i<12; i++)
   {
      if (monthName == GmatTimeConstants::MONTH_NAME_TEXT[i])
         return i + 1;
   }
   
   return -1;
}


//------------------------------------------------------------------------------
// std::string FormatCurrentTime(Integer format = 1)
//------------------------------------------------------------------------------
/*
 * Returns the current time in specified format.
 *
 * @param  format  Used in formating current time (1)
 *                 1 = "Wed Apr 16 12:30:22 2008"
 *                 2 = "2008-04-16T12:30:22"
 *                 3 = "2008-04-16 12:30:22"
 *                 4 = 1234567890_34565  (seconds and milliseconds since Jan 1, 1970)
 *
 */
//------------------------------------------------------------------------------
std::string GmatTimeUtil::FormatCurrentTime(Integer format)
{
   time_t currTime = time(NULL);

   if (format == 1)
   {
      char *currTimeStr = ctime(&currTime);
      return currTimeStr;
   }
   else if (format == 4)
   {
      std::stringstream ss("");
      ss << (Integer) currTime;
      #ifdef _MSC_VER
         // QueryPerformanceCounter() Retrieves the current value of the
         // performance counter, which is a high resolution (<1us) time stamp
         // that can be used for time-interval measurements.
         LARGE_INTEGER performanceCount;
         QueryPerformanceCounter(&performanceCount);
         ss << "_";
         ss << performanceCount.HighPart;
         ss << performanceCount.LowPart;
      #else
         /// Add microseconds of the current time
         timeval now;
         gettimeofday(&now, NULL);
         ss << "_" << (Integer) now.tv_usec;
      #endif
      return ss.str();
   }
   else
   {
      tm *loctime = localtime(&currTime);
      char timeBuf[20];
      if (format == 2)
         strftime(timeBuf, 20, "%Y-%m-%dT%H:%M:%S", loctime);
      else
         strftime(timeBuf, 20, "%Y-%m-%d %H:%M:%S", loctime);
      return timeBuf;
   }
}

//------------------------------------------------------------------------------
// std::string GetGregorianFormat()
//------------------------------------------------------------------------------
/**
 * Return gregorian time format
 */
//------------------------------------------------------------------------------
std::string GmatTimeUtil::GetGregorianFormat()
{
   return "DD MMM YYYY HH:MM:SS.mmm";
}


