//$Id$
//------------------------------------------------------------------------------
//                           OrbitDesignerTime      
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Evelyn Hull
// Created: 2011/07/25
//
/**
 * Definition of the Orbit Designer time class
 */
//------------------------------------------------------------------------------

#ifndef OrbitDesignerTime_hpp
#define OrbitDesignerTime_hpp

#include "StringUtil.hpp"

class GMATUTIL_API OrbitDesignerTime
{
public:
   OrbitDesignerTime(std::string epochFormat = "UTCGregorian",
                     std::string epoch = "01 Jan 2000 11:59:28.000",
                     Real RAAN = 306.6148021947984100,
                     std::string startTime = "12:00:00.0");
   OrbitDesignerTime(std::string epoch, std::string epochFormatStr, 
                     bool raanVal, Real RAAN, bool startTimeVal, 
                     std::string startTime);
   ~OrbitDesignerTime();

   Real FindRAAN();
   std::string FindStartTime(bool flag = false, Real lon = 0);

   void SetEpoch(std::string val);
   void SetStart(std::string val);
   void SetRAAN(Real val);

   //accessor methods
   Real GetRAAN();
   std::string GetStartTime();
   std::string GetEpoch();
   std::string GetEpochFormat();

   bool IsError();
   std::string GetError();

private:
   std::string epoch;
   std::string epochFormat;
   Real RAAN;
   std::string startTime;
   std::string errormsg;
   bool isError;
};

#endif

