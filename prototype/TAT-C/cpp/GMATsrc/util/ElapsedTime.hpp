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
 * Declares elapsed time in operations. Internal elapsed time is in seconds.
 */
//------------------------------------------------------------------------------
#ifndef ElapsedTime_hpp
#define ElapsedTime_hpp

#include "utildefs.hpp"
#include "TimeTypes.hpp"
#include "GmatConstants.hpp"

class GMATUTIL_API ElapsedTime
{
public:

//ElapsedTime();
   ElapsedTime(const Real &secs = 0.0, const Real tol = GmatRealConstants::REAL_EPSILON);
   ElapsedTime(const ElapsedTime &elapsedTime, const Real tol = GmatRealConstants::REAL_EPSILON);
   ElapsedTime& operator=(const ElapsedTime &right); 
   virtual ~ElapsedTime();

   ElapsedTime operator+ (const Real &right) const;
   ElapsedTime operator- (const Real &right) const;
   const ElapsedTime& operator+= (const Real &right);
   const ElapsedTime& operator-= (const Real &right);

   bool operator<  (const ElapsedTime &right) const;
   bool operator>  (const ElapsedTime &right) const;
   bool operator== (const ElapsedTime &right) const;
   bool operator!= (const ElapsedTime &right) const;
   bool operator>= (const ElapsedTime &right) const;
   bool operator<= (const ElapsedTime &right) const;

   Real Get() const;
   void Set(Real secs);

   GmatTimeUtil::ElapsedDate ToElapsedDate() const;

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();

protected:
private:

   Real seconds;
   Real tolerance; // Used for time comparison (==, !=)

   static const Integer NUM_DATA = 1;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};

#endif // ElapsedTime_hpp
