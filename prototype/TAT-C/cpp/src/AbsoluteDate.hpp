//------------------------------------------------------------------------------
//                           AbsoluteDate
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.05
//
/**
 * Definition of the AbsoluteDate class.  This class represents an epoch.
 */
//------------------------------------------------------------------------------
#ifndef AbsoluteDate_hpp
#define AbsoluteDate_hpp

#include "gmatdefs.hpp"
#include "Rvector6.hpp"

class AbsoluteDate
{
public:
   
   /// class construction/destruction
   AbsoluteDate();
   AbsoluteDate( const AbsoluteDate &copy);
   AbsoluteDate& operator=(const AbsoluteDate &copy);
   
   virtual ~AbsoluteDate();
   
   /// Set the Gregorian date
   virtual void     SetGregorianDate(Integer year, Integer month,  Integer day,
                                     Integer hour, Integer minute, Real second);
   /// Set the Julian Date
   virtual void     SetJulianDate(Real jd);
   /// Return the Julian Date
   virtual Real     GetJulianDate() const;
   /// Return the Gregorian date
   virtual Rvector6 GetGregorianDate();
   /// Advance the time by stepInSec seconds
   virtual void     Advance(Real stepInSec);
   /// Clone the AbsoluteDate
   virtual AbsoluteDate* Clone() const;
   
protected:
   
   /// Current date in Julian Day format
   Real  currentDate;
   
   /// Compute the Julian date from the input Gregorian date
   Real  GregorianToJulianDate(Integer year, Integer month,  Integer day,
                               Integer hour, Integer minute, Real second);
   
   /// <static> days-per-month constant
   static const Integer DAYS_PER_MONTH[12];
   /// <static> Julian date of 1900 constant
   static const Real    JD_1900;
};
#endif // AbsoluteDate_hpp
