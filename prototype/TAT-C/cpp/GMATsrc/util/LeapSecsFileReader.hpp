//$Id$
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
 */
//------------------------------------------------------------------------------

#ifndef LeapSecsFileReader_hpp
#define LeapSecsFileReader_hpp

#include "utildefs.hpp"
#include "TimeTypes.hpp"

/** 
 * Structure defining internal leap second information.
 *
 * Moved here from inside of the LeapSecsFileReader class to clean up import/
 * export issues with Visual Studio
 */
struct LeapSecondInformation
{
   Real julianDate;         // arg: 2/1/05 assumed to be utc
   Real taiMJD;
   Real offset1;
   Real offset2;
   Real offset3;
};    

// Required for Visual Studio dll exports
#ifdef EXPORT_TEMPLATES
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<LeapSecondInformation>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<LeapSecondInformation>;
#endif

class GMATUTIL_API LeapSecsFileReader
{
public:
   LeapSecsFileReader(const std::string &fileName = "tai-utc.dat");
   virtual ~LeapSecsFileReader();
   LeapSecsFileReader(const LeapSecsFileReader& lsfr);
   LeapSecsFileReader& operator=(const LeapSecsFileReader& lsfr);
   
   bool Initialize();
   Real NumberOfLeapSecondsFrom(UtcMjd utcMjd);
   Real GetFirstLeapSecondMJD(Real fromUtcMjd, Real toUtcMjd);
   
   /// Determines whether or not the input time (in TAI MJD, referenced to
   /// GmatTimeConstants::JD_MJD_OFFSET) is in a leap second
   bool IsInLeapSecond(Real theTaiMjd);
   
private:

   bool Parse(std::string line);

   // member data
   bool isInitialized;
   std::vector<LeapSecondInformation> lookUpTable;
   std::string withFileName;
};

#endif // LeapSecsFileReader_hpp
