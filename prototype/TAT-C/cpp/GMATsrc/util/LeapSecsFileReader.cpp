//$Id: LeapSecsFileReader.cpp 10525 2012-05-24 14:42:59Z wendys-dev $
//------------------------------------------------------------------------------
//                              LeapSecsFileReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Allison R. Greene
// Created: 2005/01/26
//
/**
 * Reads time coefficent file, creates a table of coefficients and converts
 * to the utc.
 *
 * File found at : ftp://maia.usno.navy.mil/ser7/tai-utc.dat
 *
 * @note The MJD-JD offset used is GmatTimeConstants::JD_NOV_17_1858
 */
//------------------------------------------------------------------------------

#include "LeapSecsFileReader.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstdlib>			// Required for GCC 4.3

//#define DEBUG_READ_LEAP_SECS_FILE
//#define DEBUG_IN_LEAP_SECOND
//#define DEBUG_DUMP_DATA

//---------------------------------
// static data
//---------------------------------
// hard coded for now - need to allow user to set later
//std::string LeapSecsFileReader::withFileName = "tai-utc.dat";

//------------------------------------------------------------------------------
// public
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  LeapSecsFileReader()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::LeapSecsFileReader(const std::string &fileName) :
withFileName     (fileName)
{
   isInitialized = false;
}

//------------------------------------------------------------------------------
//  ~LeapSecsFileReader()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::~LeapSecsFileReader()
{
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool LeapSecsFileReader::Initialize()
{
   std::ifstream instream;
   try
   {
      if (!isInitialized)
      {
         instream.open(withFileName.c_str());
         
         //if (instream == NULL)
         if (!instream.is_open())
         {
            std::string errMsg = "Unable to locate leap second file "
               + withFileName + "\n";
            throw UtilityException(errMsg);
         }
         
         bool    isOK           = true;
         Integer numLinesParsed = 0;
         while (!instream.eof())
         {
            std::string line;
            getline(instream,line);
            if (!GmatStringUtil::IsBlank(line, true))
            {
               if (!(isOK = Parse(line))) break;
               numLinesParsed++;
            }
         }
         
         // Why personalization data written to this file?
         // Try closing before throwing an exception (LOJ: 2014.06.18)
         // Moved from below 
         #ifdef DEBUG_LEAP_SECOND_FILE
         MessageInterface::ShowMessage
            ("LeapSecsFileReader::Initialize() 1 Closing leap second file\n");
         #endif
         instream.close();
         
         if (!isOK)
         {
            std::string errMsg = "Unable to read leap second file "
                                 + withFileName + " - file is malformed\n";
            throw UtilityException(errMsg);
         }
         if (numLinesParsed == 0)
         {
            std::string errMsg = "Unable to read leap second file "
                                 + withFileName + " - file contains no data\n";
            throw UtilityException(errMsg);
         }
         //instream.close();
      }
   }
   catch (...)
   {
      if (instream.is_open())
      {
         #ifdef DEBUG_LEAP_SECOND_FILE
         MessageInterface::ShowMessage
            ("LeapSecsFileReader::Initialize() 2 Closing leap second file\n");
         #endif
         instream.close();
      }
      
      //MessageInterface::PopupMessage(Gmat::WARNING_,
      //                               "Unknown Error in LeapSecondFileReader");
      // re-throw the exception
      throw;
   }

   isInitialized = true;
   return isInitialized;
}


//------------------------------------------------------------------------------
// Real NumberOfLeapSecondsFrom(UtcMjd utcMjd)
//------------------------------------------------------------------------------
/**
 * Converts utcmjd to utcjd and then looks it up from the table.  If file is not
 * read, 0 is returned.
 *
 * @return number of leap seconds
 * @note Assumes that JD from table is utcjd.
 */
//------------------------------------------------------------------------------
Real LeapSecsFileReader::NumberOfLeapSecondsFrom(UtcMjd utcMjd)
{
   if (isInitialized)
   {
      Real jd = utcMjd + GmatTimeConstants::JD_MJD_OFFSET;
      
      // look up each entry in the table to see if value is greater then the
      // julian date
      std::vector<LeapSecondInformation>::iterator info;
      for (std::vector<LeapSecondInformation>::iterator i = lookUpTable.end();
           i > lookUpTable.begin(); i--)
      {
         info = i-1;
         if (jd >= info->julianDate)
         {
            return (info->offset1 + ((utcMjd - info->offset2) * info->offset3));
         }
      }
      
      return 0.0;
   }
   else
      return 0.0;
}


//------------------------------------------------------------------------------
// Real GetFirstLeapSecondMJD(Real fromUtcMjd, Real toUtcMjd)
//------------------------------------------------------------------------------
/**
 * Returns UTCMJD of first leap seconds occurred between fromUtcMjd and toUtcMjd.
 * If file is not read or fromUtcMjd is greater than toUtcMjd, or no leap seconds
 * occurred between input dates, -1 is returned.
 *
 * @return First date of leap seconds occurred between two input dates
 * @note Assumes that JD from table is utcjd.
 */
//------------------------------------------------------------------------------
Real LeapSecsFileReader::GetFirstLeapSecondMJD(Real fromUtcMjd, Real toUtcMjd)
{
   #ifdef DEBUG_LEAP_SECOND
   MessageInterface::ShowMessage
      ("LeapSecsFileReader::GetFirstLeapSecondMJD() entered, fromUtcMjd=%.12f, "
       "toUtcMjd=%.12f, isInitialized=%d\n", fromUtcMjd, toUtcMjd, isInitialized);
   #endif
   Real firstMjd = -1.0;
   
   if (isInitialized && (toUtcMjd >= fromUtcMjd))
   {
      Real fromJd = fromUtcMjd + GmatTimeConstants::JD_MJD_OFFSET;
      Real toJd = toUtcMjd + GmatTimeConstants::JD_MJD_OFFSET;
      Real currJd = 0.0;
      RealArray utcMjdArray;
      
      // look up each entry in the table to see if value is greater then the
      // julian date
      std::vector<LeapSecondInformation>::iterator info;
      for (std::vector<LeapSecondInformation>::iterator i = lookUpTable.end();
           i > lookUpTable.begin(); i--)
      {
          info = i-1;
          currJd = info->julianDate;
          if ((currJd >= fromJd) && (currJd <= toJd))
          {
             utcMjdArray.push_back(currJd);
          }
          else if (currJd < fromJd)
          {
             break;
          }
      }
      
      // Compute MJD of first JD in the array
      if (utcMjdArray.empty())
      {
         firstMjd = -1.0;
      }
      else
      {
         //Since iterator goes backward in the above for loop, larger jd is moved to front
         firstMjd = utcMjdArray.back() - GmatTimeConstants::JD_MJD_OFFSET;
      }
   }
   
   #ifdef DEBUG_LEAP_SECOND
   MessageInterface::ShowMessage
      ("LeapSecsFileReader::GetFirstLeapSecondMJD() returning %.12f\n", firstMjd);
   #endif
   return firstMjd;
}


//------------------------------------------------------------------------------
// bool IsInLeapSecond(Real theTaiMjd)
// theTaiMjd is referenced to GmatTimeConstants::JD_NOV_17_1858
//------------------------------------------------------------------------------
bool LeapSecsFileReader::IsInLeapSecond(Real theTaiMjd)
{
   bool isLeap = false;

   // Find rhe nearest leap second to the input tai mjd time
   Real nearestLeapSecond = 0.0;
   
   // Is the time before or after the lookup table?
   Integer sz    = lookUpTable.size();
   Real    tai0  = lookUpTable.at(0).taiMJD;
   Real    taif  = lookUpTable.at(sz-1).taiMJD;

   #ifdef DEBUG_IN_LEAP_SECOND
      MessageInterface::ShowMessage("In IsInLS, theTaiMjd = %12.10f\n", theTaiMjd);
      MessageInterface::ShowMessage("           tai0      = %12.10f\n", tai0);
      MessageInterface::ShowMessage("           taif      = %12.10f\n", taif);
      MessageInterface::ShowMessage(" one sec = %12.10f\n", 1.0 /GmatTimeConstants::SECS_PER_DAY);
   #endif

   // is this correct? or should it be 0.0 before the earliest time on the file?
   if (theTaiMjd <= tai0)
      nearestLeapSecond = tai0;
   else if (theTaiMjd >= taif)
      nearestLeapSecond = taif;
   else
   {
      Real diff        = GmatRealConstants::REAL_MAX;
      Real currDiff    = 0.0;
      Real currTai     = 0.0;
      Real previousTai = taif;
      
      Integer lookupSize = lookUpTable.size();
      for (Integer ii = lookupSize - 1; ii >= 0; ii--)
      {
         currTai  = lookUpTable.at(ii).taiMJD;
         currDiff = GmatMathUtil::Abs(currTai - theTaiMjd);
         // added '=' because if it's exactly in-between, we want the later one
         // - is that correct?
         if (currDiff >= diff)
         {
            nearestLeapSecond = previousTai;
            break;
         }
         else // currDiff < diff   so save it as the smallest diff so far
         {
            diff        = currDiff;
            previousTai = currTai;
            if (ii == 0)  // we've reached the end (beginning!) of the table
            {
               nearestLeapSecond = currTai;
            }
         }
      }
   }
   if (nearestLeapSecond == 0.0) // something went wrong
   {
      std::string errMsg = "ERROR finding nearest leap second\n";
      throw UtilityException(errMsg);
   }
   
   Real nearestMinusOne = nearestLeapSecond - (1.0/GmatTimeConstants::SECS_PER_DAY);
   // Determine if the input time is within the leap second
   if ((theTaiMjd >= nearestMinusOne) && (theTaiMjd < nearestLeapSecond))
      isLeap = true;
   
   #ifdef DEBUG_IN_LEAP_SECOND
      MessageInterface::ShowMessage("---------- nearestLeapSecond = %12.10f\n", nearestLeapSecond);
      MessageInterface::ShowMessage("---------- nearestMinusOne   = %12.10f\n", nearestMinusOne);
      MessageInterface::ShowMessage("---------- isLeap = %s\n", (isLeap? " true" : "false"));
   #endif
   return isLeap;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool Parse()
//------------------------------------------------------------------------------
/**
 * Parses each line to add leap second information to the table
 *
 * Format of the line is:
 *       YYYY MMM  D =JD jDate  TAI-UTC= off1 S + (MJD - off2) X off3 S
 * @return true if the file parses successfully
 */
//------------------------------------------------------------------------------
bool LeapSecsFileReader::Parse(std::string line)
{
#ifdef DEBUG_READ_LEAP_SECS_FILE
   MessageInterface::ShowMessage("Entering LeapSecsFileReader::Parse with line = %s\n", line.c_str());
#endif
   Real jDate, off1, off2, off3;
   
   // ignore blank lines
   if (GmatStringUtil::IsBlank(line, true))  return true;
   
   std::istringstream ss(line);
   Integer year, day;
   std::string month, equalsJD, tai_utc, S, plus, mjd, minus, closeParen, X, S2;
   // clear error flags
   ss.clear();
   try
   {
      ss >> year >> month >> day >> equalsJD >> jDate >> tai_utc >> off1 >> S >> plus >> mjd >> minus >> off2 >> closeParen >> X >> off3 >> S2;
#ifdef DEBUG_READ_LEAP_SECS_FILE
      MessageInterface::ShowMessage("Entering LeapSecsFileReader::Parse - data read from line are: \n");
      MessageInterface::ShowMessage("         year       = %d\n", year);
      MessageInterface::ShowMessage("         month      = %s\n", month.c_str());
      MessageInterface::ShowMessage("         day        = %d\n", day);
      MessageInterface::ShowMessage("         equalsJD   = %s\n", equalsJD.c_str());
      MessageInterface::ShowMessage("   >>>jDate = %12.10f\n", jDate);
      MessageInterface::ShowMessage("         tai_utc    = %s\n", tai_utc.c_str());
      MessageInterface::ShowMessage("   >>>off1  = %12.10f\n", off1);
      MessageInterface::ShowMessage("         S          = %s\n", S.c_str());
      MessageInterface::ShowMessage("         plus       = %s\n", plus.c_str());
      MessageInterface::ShowMessage("         mjd        = %s\n", mjd.c_str());
      MessageInterface::ShowMessage("         minus      = %s\n", minus.c_str());
      MessageInterface::ShowMessage("   >>>off2  = %12.10f\n", off2);
      MessageInterface::ShowMessage("         closeParen = %s\n", closeParen.c_str());
      MessageInterface::ShowMessage("         X          = %s\n", X.c_str());
      MessageInterface::ShowMessage("   >>>off3  = %12.10f\n", off3);
      MessageInterface::ShowMessage("         S2         = %s\n", S2.c_str());
      if (ss.fail()) MessageInterface::ShowMessage(" ------ fail is true ------\n");
      if (ss.bad())  MessageInterface::ShowMessage(" ------ bad  is true ------\n");
      if (ss.eof())  MessageInterface::ShowMessage(" ------ eof  is true ------\n");
#endif
      // if there was an error reading all of the items from the stream, return false
      // don't check for eof here, as it will return true when it gets to the end of the line
      if (ss.bad() || ss.fail()) return false;
      // Convert to TAI time (MJD) here and store that as well
      Real numLeapSeconds = off1 + ((jDate - off2) * off3);
      Real taiDate        = jDate - GmatTimeConstants::JD_MJD_OFFSET +
                            (numLeapSeconds/GmatTimeConstants::SECS_PER_DAY);  
#ifdef DEBUG_DUMP_DATA
      MessageInterface::ShowMessage(" LookUpTable(%d)::\n", (Integer) lookUpTable.size());
      MessageInterface::ShowMessage("     jDate   =============== %12.10f\n", jDate);
      MessageInterface::ShowMessage("     taiDate =============== %12.10f\n", taiDate);
      MessageInterface::ShowMessage("     off1    =============== %12.10f\n", off1);
      MessageInterface::ShowMessage("     off2    =============== %12.10f\n", off2);
      MessageInterface::ShowMessage("     off3    =============== %12.10f\n", off3);
#endif
      LeapSecondInformation leapSecInfo = {jDate, taiDate, off1, off2, off3};
      lookUpTable.push_back(leapSecInfo);
      return true;
   }
   catch (BaseException &be)
   {
      return false;
   }
}

