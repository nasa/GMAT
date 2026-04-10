// Author: Darrel Conway
// Created: 2019/09/26
//
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


#include "TLEData.hpp"


TLEData::TLEData() :
   satName        (""),
   secFromJ2k     (0.0)
{
   tleLines[0] = tleLines[1] = tleLines[2] = "";
   for (int i = 0; i < 10; ++i)
      elements[i] = 0.0;
}


TLEData::~TLEData()
{
}

TLEData::TLEData(const TLEData& tle) :
   satName        (tle.satName),
   secFromJ2k     (tle.secFromJ2k)
{
   for (int i = 0; i < 3; ++i)
      tleLines[i] = tle.tleLines[i];
   for (int i = 0; i < 10; ++i)
      elements[i] = tle.elements[i];
}

TLEData& TLEData::operator=(const TLEData& tle)
{
   if (this != &tle)
   {
      satName = tle.satName;
      secFromJ2k = tle.secFromJ2k;
      for (int i = 0; i < 3; ++i)
         tleLines[i] = tle.tleLines[i];
      for (int i = 0; i < 10; ++i)
         elements[i] = tle.elements[i];
   }

   return *this;
}
