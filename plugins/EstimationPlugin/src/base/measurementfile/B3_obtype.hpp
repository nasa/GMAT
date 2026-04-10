//------------------------------------------------------------------------------
//                         B3_obtype
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
//         Based on code by Matthew P. Wilkins, Shafer Corporation
// Created: 2009/07/06
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#ifndef B3_obtype_hpp
#define B3_obtype_hpp

#include "estimation_defs.hpp"
#include "ObType.hpp"


/**
 * B3 specific extensions to the base ObType structure
 */
class ESTIMATION_API B3_obtype : public ObType
{
public:
   B3_obtype();
   virtual ~B3_obtype();


   // Possible obtype values and their meaning
   // 0 - Range rate only
   // 1 - Azimuth and elevation
   // 2 - Range, azimuth and elevation
   // 3 - Range, azimuth, elevation, and range rate
   // 4 - Range, azimuth, eelcation, and range rate
   //    (extra measurements for azimuth rate, elevation rate, etc are ignored)
   // 5 - Right Ascension and Declination
   // 6 - Range only
   // 8 - Azimuth, elevation, sometimes range and ECF position of the sensor
   // 9 - Right ascension, declination, sometimes range and
   //     ECF position of the sensor
   //
   // The variables below will only be specified according to the b3Type
   // otherwise they will be NULL valued.

   Integer b3Type;

   // Typeically "U" but may be other characters as well
   std::string securityClassification;

   // SSSSS integer satellite ID number corresponding to the SSC number
   Integer satelliteID;

   // sss integer sensor ID
   Integer sensorID;

   // YY year (assumes 20YY if Y <= 50, 19YY if YY > 50)
   Integer year;

   // DDD Day of year where January 1st is day 1.
   Integer dayOfYear;

   // Hours, minutes, and seconds of observation
   Integer hour;
   Integer minute;
   Real seconds;

   // Elevation, declination are specified in degrees.
   Real elevation;
   Real declination;

// Right Ascension specified in hours
   Real rightAscension;

   // Azimuth is specified in degrees and is always positive.
   Real azimuth;

   //  Range in km
   Real range;

   // Slant range rate in km/s
   Real rangeRate;

   // Earth Centered Fixed (ECF) sensor position in kilometers
   Real ecf_X;
   Real ecf_Y;
   Real ecf_Z;

};

#endif /* B3_obtype_hpp */
