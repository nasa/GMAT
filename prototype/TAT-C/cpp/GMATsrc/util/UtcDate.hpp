//$Id$
//------------------------------------------------------------------------------
//                                UtcDate
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
// Created: 1995/10/18 for GSS project
// Modified: 2003/09/12 Linda Jun - Added member data: descs, stringValues
//           2004/05/06 J. Gurganus - Added ToA1Mjd method for converting to
//                                    A1 Modified Julian date. 
//
/**
 * Provides conversions among various ways representing UTC calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef UtcDate_hpp
#define UtcDate_hpp

#include "utildefs.hpp"
#include "TimeTypes.hpp"
#include "Date.hpp"
//#include "A1Date.hpp"
#include "DateUtil.hpp"

 class DateUtil;
//class A1Date;

class GMATUTIL_API UtcDate : public Date
{
public:
    UtcDate();
    UtcDate(Integer year, Integer month, Integer day, Integer hour, 
            Integer minute, Real second);
    UtcDate(Integer year, Integer doy, Integer hour, Integer minute,
            Real second);
    UtcDate(Integer year, Integer month, Integer day, Real secondsOfDay);
    UtcDate(const GmatTimeUtil::CalDate &date);
    UtcDate(const std::string& time);            // "YYYYMMDD.HHMMSSmmm"
    UtcDate(const UtcDate &date);
    UtcDate operator= (const UtcDate &date);
    ~UtcDate();

//loj: Do this later
//      Real     operator- (const UtcDate &date) const;
//      UtcDate  operator+ (const Real seconds) const;
//      UtcDate& operator+=(const Real seconds);
//      UtcDate  operator- (const Real seconds) const;
//      UtcDate& operator-=(const Real seconds);
//      A1Date ToA1Date();

    Real  ToA1Mjd() const;

protected:
private:
};
#endif // UtcDate_hpp
