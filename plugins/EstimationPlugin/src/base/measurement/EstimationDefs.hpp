//$Id: EstimationDefs.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationDefs
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
// Created: 2009/06/24
//
/**
 * Measurement model definitions used in GMAT's estimation subsystem
 *
 * This file adds structures and enumerations to the Gmat namespace that are
 * used in the estimation subsystem for measurement modeling, both during
 * simulation and during estimation.
 */
//------------------------------------------------------------------------------


#ifndef EstimationDefs_hpp
#define EstimationDefs_hpp

#include "estimation_defs.hpp"      // DLL import/export definitions
#include "gmatdefs.hpp"


/// Extensions to the Gmat namespace used in the estimation subsystem
namespace Gmat
{
   /// Identifiers for the known measurement data types
   enum MeasurementType
   {
      // Geometric Measurements
      GEOMETRIC_FIRST_MEASUREMENT = 7000,
      GEOMETRIC_RANGE = GEOMETRIC_FIRST_MEASUREMENT,
      GEOMETRIC_RANGE_RATE,
      GEOMETRIC_AZ_EL,
      GEOMETRIC_RA_DEC,
      // Add new Geometric measurements here
      GEOMETRIC_MAX_MEASUREMENT = GEOMETRIC_RANGE + 49,// Room for 50 geometric types

      // Deep Space Network measurements
      DSN_FIRST_MEASUREMENT,                      // 7050
      DSN_TWOWAYRANGE = DSN_FIRST_MEASUREMENT,
      DSN_TWOWAYDOPPLER,
      // Add new DSN measurements here
      DSN_MAX_MEASUREMENT = DSN_TWOWAYRANGE + 49, // Room for 50 DSN types

      // United Space Network measurements
      USN_FIRST_MEASUREMENT,                      // 7100
      USN_TWOWAYRANGE = USN_FIRST_MEASUREMENT,
      USN_TWOWAYRANGERATE,
      // Add new USN measurements here
      USN_MAX_MEASUREMENT = USN_TWOWAYRANGE + 49, // Room for 50 USN types

      // Space Network measurements
      SN_FIRST_MEASUREMENT,                       // 7150
      SN_TWOWAYRANGE = SN_FIRST_MEASUREMENT,
      SN_TWOWAYRANGERATE,
      // Add new SN measurements here
      SN_MAX_MEASUREMENT = SN_TWOWAYRANGE + 49,   // Room for 50 SN types

      // United Space Network measurements
      TDRSS_FIRST_MEASUREMENT,                    // 7200
      TDRSS_TWOWAYRANGE = TDRSS_FIRST_MEASUREMENT,
      TDRSS_TWOWAYRANGERATE,
      // Add new USN measurements here
      TDRSS_MAX_MEASUREMENT = TDRSS_TWOWAYRANGE + 49, // Room for 50 USN types

      // New tracking system measurement types that are part of the estimation
      // plugin start here

      UNKNOWN_MEASUREMENT,                        // 7250

      // More general measurement types -- room for up to 100
      GENERAL_FIRST_MEASUREMENT = 7600,
      OPTICAL_AZEL = GENERAL_FIRST_MEASUREMENT,
      OPTICAL_RADEC,
      // Add new general purpose measurements here
      GENERAL_MAX_MEASUREMENT = 7699,

      /// Allow for user defined measurements
      CUSTOM_MEASUREMENT_START = 7700,
      // Base custom measurement IDs on CUSTOM_MEASUREMENT_START
      CUSTOM_MEASUREMENT_END   = 7999,
      UNKNOWN_CUSTOM_MEASUREMENT
   };

   /// Identifiers for the known data sources
   enum MeasurementSource
   {
      GMAT_INTERNAL_DATA = 3000,
      // B3_DATA,
      // RINEX_DATA,
      // SLR_DATA,
      // SP3_DATA,
      // TLE_DATA,
      UNKNOWN_SOURCE,
      /// Allow for user defined sources
      CUSTOM_SOURCE_START = 3500,
      CUSTOM_SOURCE_END   = 3800
   };

}


#endif /* EstimationDefs_hpp */
