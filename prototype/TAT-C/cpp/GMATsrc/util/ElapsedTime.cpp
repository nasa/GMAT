//$Id$
//------------------------------------------------------------------------------
//                              ElapsedTime
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
// Created: 2003/09/22
//
/**
 * Defines elapsed time operations. Internal elapsed time is in seconds.
 */
//------------------------------------------------------------------------------
#include <sstream>
#include "ElapsedTime.hpp"
#include "RealUtilities.hpp"

using namespace GmatTimeConstants;  // for SECS_PER_DAY, SECS_PER_HOUR, etc.
using namespace GmatMathUtil;  // for Rem(), IsEqual()
using namespace GmatMathConstants;  // for Rem(), IsEqual()

//---------------------------------
// static variables
//---------------------------------

const std::string ElapsedTime::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Elapsed Time in Seconds"
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ElapsedTime::ElapsedTime(const Real &secs, const Real tol)
//------------------------------------------------------------------------------
ElapsedTime::ElapsedTime(const Real &secs, const Real tol)
   : seconds(secs), tolerance(tol)
{
}

//------------------------------------------------------------------------------
// ElapsedTime::ElapsedTime(const ElapsedTime &elapsedTime, const Real tol)
//------------------------------------------------------------------------------
ElapsedTime::ElapsedTime(const ElapsedTime &elapsedTime, const Real tol)
   : seconds(elapsedTime.seconds), tolerance(tol)
{
}

//------------------------------------------------------------------------------
// ElapsedTime& ElapsedTime::operator= (const ElapsedTime &right)
//------------------------------------------------------------------------------
ElapsedTime& ElapsedTime::operator= (const ElapsedTime &right)
{
   seconds = right.seconds;
   return *this;
}

//------------------------------------------------------------------------------
// ElapsedTime::~ElapsedTime()
//------------------------------------------------------------------------------
ElapsedTime::~ElapsedTime()
{
}

//------------------------------------------------------------------------------
// ElapsedTime ElapsedTime::operator+ (const Real &right) const
//------------------------------------------------------------------------------
ElapsedTime ElapsedTime::operator+ (const Real &right) const
{
   return ElapsedTime(seconds + right);
}

//------------------------------------------------------------------------------
// ElapsedTime ElapsedTime::operator- (const Real &right) const
//------------------------------------------------------------------------------
ElapsedTime ElapsedTime::operator- (const Real &right) const
{
   return ElapsedTime(seconds - right);
}

//------------------------------------------------------------------------------
// const ElapsedTime& ElapsedTime::operator+= (const Real &right)
//------------------------------------------------------------------------------
const ElapsedTime& ElapsedTime::operator+= (const Real &right)
{
   seconds += right;
   return *this;
}

//------------------------------------------------------------------------------
// const ElapsedTime& ElapsedTime::operator-= (const Real &right)
//------------------------------------------------------------------------------
const ElapsedTime& ElapsedTime::operator-= (const Real &right)
{
   seconds -= right;
   return *this;
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator< (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator< (const ElapsedTime &right) const
{
   return seconds < right.seconds;
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator> (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator> (const ElapsedTime &right) const
{
   return seconds > right.seconds;
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator== (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator== (const ElapsedTime &right) const
{
   return IsEqual(seconds, right.seconds, tolerance);
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator!= (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator!= (const ElapsedTime &right) const
{
   return !(IsEqual(seconds, right.seconds, tolerance));
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator>= (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator>= (const ElapsedTime &right) const
{
   return seconds >= right.seconds;
}

//------------------------------------------------------------------------------
// bool ElapsedTime::operator<= (const ElapsedTime &right) const
//------------------------------------------------------------------------------
bool ElapsedTime::operator<= (const ElapsedTime &right) const
{
   return seconds <= right.seconds;
}

//------------------------------------------------------------------------------
// Real ElapsedTime::Get() const
//------------------------------------------------------------------------------
Real ElapsedTime::Get() const
{
   return seconds;
}

//------------------------------------------------------------------------------
// void ElapsedTime::Set(Real secs)
//------------------------------------------------------------------------------
void ElapsedTime::Set(Real secs)
{
   seconds = secs;
}

//------------------------------------------------------------------------------
// GmatTimeUtil::ElapsedDate ElapsedTime::ToElapsedDate() const
//------------------------------------------------------------------------------
GmatTimeUtil::ElapsedDate ElapsedTime::ToElapsedDate() const
{
   Real secs = seconds;
   
   // Compute sign
   if (secs < 0)
      secs =  - secs;
   
   //Compute elapsed days, hours, minutes, seconds 
   Integer days = (Integer)(secs / SECS_PER_DAY); 
   Integer hours = (Integer)(Rem(secs, SECS_PER_DAY) / SECS_PER_HOUR); 
   Integer minutes = (Integer)(Rem(Rem(secs, SECS_PER_DAY), 
                                   SECS_PER_HOUR) / SECS_PER_MINUTE); 
   secs = Rem(Rem(Rem(secs, SECS_PER_DAY), SECS_PER_HOUR), SECS_PER_MINUTE); 
   
   return GmatTimeUtil::ElapsedDate(days, hours, minutes, secs);
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer ElapsedTime::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* ElapsedTime::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* ElapsedTime::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
// std::string* ElapsedTime::ToValueStrings()
//------------------------------------------------------------------------------
std::string* ElapsedTime::ToValueStrings()
{
   std::stringstream ss("");

   ss << seconds;
   stringValues[0] = ss.str();
   return stringValues;
}

