//$Id$
//------------------------------------------------------------------------------
//                                  A1Date
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
// Created: 1995/10/05 for GSS project
// Modified: 2003/09/12 Linda Jun - Added a new constructor taking
//           GmatTimeUtil::CalDate
//
/**
 * Provides conversions among various ways representing A1 calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef A1Date_hpp
#define A1Date_hpp

#include "utildefs.hpp"
#include "TimeTypes.hpp"
#include "Date.hpp"


class UtcDate;

class GMATUTIL_API A1Date : public Date
{
public:
    A1Date ();
    A1Date (Integer year, Integer month, Integer day, Integer hour, 
            Integer minute, Real second);
    A1Date (Integer year, Integer doy, Integer hour, Integer minute,
            Real second);
    A1Date (Integer year, Integer month, Integer day, Real secondsOfDay);
    A1Date(const GmatTimeUtil::CalDate &date);
    A1Date (const std::string &dateString);      // "YYYYMMDD.hhmmssnnn"
    A1Date (const A1Date &a1date);               // copy constructor
    A1Date  operator=  (const A1Date &a1date);
    bool  operator>  (const A1Date &a1date) const;
    bool  operator<  (const A1Date &a1date) const;
    ~A1Date ();

//loj: Do this later if we need.
//      Real    operator-  (const A1Date &date) const;
//      A1Date  operator+  (const Real seconds) const;
//      A1Date& operator+= (const Real seconds);
//      A1Date  operator-  (const Real seconds) const;
//      A1Date& operator-= (const Real seconds);
   
//     GmatTimeUtil::CalDate ToUtcDate() const;

protected:
private:
};

#endif // A1Date_hpp
