//$Id$
//------------------------------------------------------------------------------
//                                 TimeSystemConverter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Allison Greene
// Created: 2005/02/03
//
/**
 * Definition of the TimeSystemConverter class
 */
//------------------------------------------------------------------------------

#ifndef TimeSystemConverter_hpp
#define TimeSystemConverter_hpp

//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include "BaseException.hpp"
#include "EopFile.hpp"
#include "LeapSecsFileReader.hpp"
//#include "GmatConstants.hpp"

#include "GmatTime.hpp"


class GMATUTIL_API UnimplementedException : public BaseException
{
   public:
      UnimplementedException(const std::string &message =
      "TimeSystemConverter: Conversion not implemented: ")
      : BaseException(message) {};
};
class GMATUTIL_API TimeFileException : public BaseException
{
   public:
      TimeFileException(const std::string &message =
      "TimeSystemConverter: File is unknown: ")
      : BaseException(message) {};
};
class GMATUTIL_API TimeFormatException : public BaseException
{
   public:
      TimeFormatException(const std::string &message =
      "TimeSystemConverter: Requested format not implemented: ")
      : BaseException(message) {};
};
class GMATUTIL_API InvalidTimeException : public BaseException
{
   public:
      InvalidTimeException(const std::string &message =
      "TimeSystemConverter: Requested time is invalid: ")
      : BaseException(message) {};
};



class GMATUTIL_API TimeSystemConverter
{
public:
   // Most of the time, GMAT uses the singleton
   static TimeSystemConverter *Instance();

   // Specified in Math Spec section 2.3
   const Real TDB_COEFF1;                  //  = 0.001658;
   const Real TDB_COEFF2;                  //  = 0.00001385;
   const Real M_E_OFFSET;                  //  = 357.5277233;
   const Real M_E_COEFF1;                  //  = 35999.05034;
   const Real T_TT_OFFSET;                 //  = GmatTimeConstants::JD_OF_J2000;
   const Real T_TT_COEFF1;                 //  = GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   const Real L_B;                         //  = 1.550505e-8;
   const Real NUM_SECS;                    //  = GmatTimeConstants::SECS_PER_DAY;

   enum TimeSystemTypes
   {
      A1MJD = 0,
      TAIMJD,
      UTCMJD,
      UT1MJD,
      TDBMJD,
      TTMJD,
      A1,
      TAI,
      UTC,
      UT1,
      TDB,
      TT,
      TimeSystemCount
   };

   static const std::string TIME_SYSTEM_TEXT[TimeSystemCount];
   
   Integer     GetTimeTypeID(std::string &str);
   
   Real        Convert(const Real origValue,
                         const Integer fromType,
                         const Integer toType,
                         Real refJd = GmatTimeConstants::JD_JAN_5_1941,
                         bool *insideLeapSec = NULL);
   Real        ConvertToTaiMjd(Integer fromType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858,
                         bool *insideLeapSec = NULL);
   Real        ConvertFromTaiMjd(Integer toType, Real origValue,
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858,
                         bool *insideLeapSec = NULL);

   GmatTime    Convert(const GmatTime origValue, 
                         const Integer fromType, 
                         const Integer toType, 
                         Real refJd = GmatTimeConstants::JD_JAN_5_1941,
                         bool *insideLeapSec = NULL);
   GmatTime    ConvertToTaiMjd(Integer fromType, GmatTime origValue, 
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858,
                         bool *insideLeapSec = NULL);
   GmatTime    ConvertFromTaiMjd(Integer toType, GmatTime origValue, 
                         Real refJd = GmatTimeConstants::JD_NOV_17_1858,
                         bool *insideLeapSec = NULL);

   Real        NumberOfLeapSecondsFrom(Real utcMjd,
                         Real jdOfMjdRef = GmatTimeConstants::JD_JAN_5_1941);
   Real        GetFirstLeapSecondMJD(Real fromUtcMjd, Real toUtcMjd,
                         Real jdOfMjdRef = GmatTimeConstants::JD_JAN_5_1941);
   
   void        SetEopFile(EopFile *eopFile);
   void        SetLeapSecsFileReader(LeapSecsFileReader *leapSecsFileReader);
   
   void        GetTimeSystemAndFormat(const std::string &type, std::string &system,
                         std::string &format);
   
   std::string ConvertMjdToGregorian(const Real mjd, bool handleLeapSecond = false,
                                              Integer format = 1);
   Real        ConvertGregorianToMjd(const std::string &greg);
   GmatTime    ConvertGregorianToMjdGT(const std::string &greg);
   
   void        Convert(const char *fromType, Real fromMjd,
                         const char *fromStr, const char *toType,
                         Real &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);
   void        Convert(const char *fromType, Real fromMjd,
                         const std::string &fromStr, const std::string &toType,
                         Real &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);
   void        Convert(const std::string &fromType, Real fromMjd,
                         const std::string &fromStr, const std::string &toType,
                         Real &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);
   
   void        Convert(const char *fromType, GmatTime fromMjd, 
                         const char *fromStr, const char *toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);
   void        Convert(const char *fromType, GmatTime fromMjd, 
                         const std::string &fromStr, const std::string &toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);
   void        Convert(const std::string &fromType, GmatTime fromMjd, 
                         const std::string &fromStr, const std::string &toType, 
                         GmatTime &toMjd, std::string &toStr, Integer format = 1,
                         bool *insideLeapSec = NULL);

   bool        ValidateTimeSystem(std::string sys);   
   bool        ValidateTimeFormat(const std::string &format, const std::string &value,
                                    bool checkValue = true);
   StringArray GetValidTimeRepresentations();
   
   bool        IsValidTimeSystem(const std::string& system);
   
protected:

   bool        IsInLeapSecond(Real theTaiMjd);
   bool        IsInLeapSecond(GmatTime theTaiMjd);
   
//   bool        HandleLeapSecond();
   

//bool isInLeapSecond;
   EopFile              *theEopFile;
   LeapSecsFileReader   *theLeapSecsFileReader;


   TimeSystemConverter();
   TimeSystemConverter(const TimeSystemConverter &tcu);

   // The singleton
   static TimeSystemConverter *theTimeConverter;
};

#endif // TimeSystemConverter_hpp
