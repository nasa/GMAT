// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TLEData_hpp
#define TLEData_hpp

#include <string>

/**
 * Structure used to hold the TLE data for a single spacecraft
 */
class TLEData
{
public:
   TLEData();
   ~TLEData();
   TLEData(const TLEData& tle);
   TLEData& operator=(const TLEData& tle);

   /// Name of the spacecraft
   std::string satName;
   /// The raw TLE, with header line
   std::string tleLines[3];

   /// Epoch of the element set (sec from J2000: Noon 1/1/2000 TT)
   double secFromJ2k;
   /// Array of the data in CSPICE form
   double elements[10];
};

#endif
