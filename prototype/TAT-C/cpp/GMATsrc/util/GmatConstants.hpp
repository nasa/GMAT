//$Id:
//------------------------------------------------------------------------------
//                           GmatConstants
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task Order 28
//
// Author: W. Shoan/NASA/GSFC/583
// Created: 2011.02.15
//
/**
 * Provides declarations for commonly used physical/computation/time/conversion
 * constants.
 */
//------------------------------------------------------------------------------
#include <limits>
#include "utildefs.hpp"

#ifndef GmatConstants_hpp
#define GmatConstants_hpp

// Constants relating to real numbers
namespace GmatRealConstants
{
   const Real    REAL_TOL             = 1.0e-15;
   const Real    REAL_UNDEFINED       = -987654321.0123e-45;
   const Real    REAL_UNDEFINED_LARGE = -9876543210.1234;
   const Integer INTEGER_UNDEFINED    = -987654321;
   
   #ifdef _MSC_VER
      const Integer INTEGER_MAX          = _I32_MAX;
   #else
      const Integer INTEGER_MAX          = std::numeric_limits<Integer>::max();
   #endif
   
   #ifdef __glibcpp_f64_radix
      const Integer SHORT_REAL_RADIX = __glibcpp_f64_radix;
   #else
      const Integer SHORT_REAL_RADIX  = 2;
   #endif

   //loj: Couldn't find corresponding MANT_DIG, do this later
   //const Integer REAL_MANT_DIG;
   #ifdef __glibcpp_f64_epsilon
      const Real    REAL_EPSILON     = __glibcpp_f64_epsilon;
   #else
      const Real    REAL_EPSILON      = 2.2204460492503131e-16;
   #endif

   #ifdef __glibcpp_f64_digits
      const Integer REAL_DIG         = __glibcpp_f64_digits;
   #else
      const Integer REAL_DIG          = 53;
   #endif

   #ifdef __glibcpp_f64_min_exponent
      const Integer REAL_MIN_EXP     = __glibcpp_f64_min_exponent;
   #else
      const Integer REAL_MIN_EXP      = -1021;
   #endif

   #ifdef __glibcpp_f64_min
      const Real    REAL_MIN         = __glibcpp_f64_min;
   #else
      const Real    REAL_MIN          = 2.2250738585072014e-308;
   #endif

   #ifdef __glibcpp_f64_min_exponent10
      const Integer REAL_MIN_10_EXP  = __glibcpp_f64_min_exponent10;
   #else
      const Integer REAL_MIN_10_EXP   = -307;
   #endif

   #ifdef __glibcpp_f64_max_exponent
      const Integer REAL_MAX_EXP     = __glibcpp_f64_max_exponent;
   #else
      const Integer REAL_MAX_EXP      = 1024;
   #endif

   #ifdef __glibcpp_f64_max
      const Real    REAL_MAX         = __glibcpp_f64_max;
   #else
      const Real    REAL_MAX          = 1.7976931348623157e+308;
   #endif

   #ifdef __glibcpp_f64_max_exponent10
      const Integer REAL_MAX_10_EXP  = __glibcpp_f64_max_exponent10;
   #else
      const Integer REAL_MAX_10_EXP   = 308;
   #endif
} // GmatRealConstants

// Physical Constants
namespace GmatPhysicalConstants
{
    //Speed Of Light Constant
    const Real SPEED_OF_LIGHT_VACUUM            = 299792458.0;  // m/s
    const Real c                                = 299792458.0;  // m/s

    // Gravitational constant
    const Real UNIVERSAL_GRAVITATIONAL_CONSTANT = 6.673e-20;     // km^3/(kg s^2)

    //Astronomical Constants
    const Real ASTRONOMICAL_UNIT                = 1.49597870e8;  // km

    // Temperature constants
    const Real ABSOLUTE_ZERO_K                  = 0.0;           // K
    const Real ABSOLUTE_ZERO_C                  = -273.15;       // degrees C
} // GmatPhysicalConstants

// Constants relating to integer numbers
namespace GmatIntegerConstants
{
   const Integer INTEGER_UNDEFINED     = -987654321;
}

// Time Constants
namespace GmatTimeConstants
{
   const Real SECS_PER_DAY             = 86400.0;
   const Real SECS_PER_HOUR            = 3600.0;
   const Real SECS_PER_MINUTE          = 60.0;

   const Real DAYS_PER_YEAR            = 365.25;
   const Real DAYS_PER_JULIAN_CENTURY  = 36525.00;
   const Real DAYS_PER_SEC             = 1.1574074074074074074074074074074e-5;

   const Real TIME_OF_J2000  = 883655990.850000; // 2000/01/01 43167.85
   const Real JD_OF_J2000    = 2451545.0;        // JD of J2000 epoch
   const Real MJD_OF_J2000   = 21545.00000000;   // MJD of J2000 epoch
   const Real A1MJD_OF_J2000 = 21545.00000000;   // 2000/01/01 11:59:27.965622
   const Real JD_MJD_OFFSET  = 2400000.5;        // Vallado page 187 (= JD_NOV_17_1858)
   const Real TT_TAI_OFFSET  = 32.184;           // GMAT Math Spec section 2.3
   const Real A1_TAI_OFFSET  = 0.0343817;        // GMAT Math Spec section 2.1
   const Real JD_JAN_5_1941  = 2430000.0;        // old name JULIAN_DATE_OF_010541
   const Real JD_NOV_17_1858 = 2400000.5;        // old name JD_MJD_OFFSET

   const Integer DAYS_BEFORE_MONTH[12] =
   {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
   const Integer LEAP_YEAR_DAYS_BEFORE_MONTH[12] =
   {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
   const Integer DAYS_IN_MONTH[12] =
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   const Integer LEAP_YEAR_DAYS_IN_MONTH[12] =
   {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   const Integer JULIAN_DATE_OF_010541 = 2430000;

   const Real MJD_EPOCH_PRECISION = 7.27e-12;    // 37 bits for decimal piece

   enum DayName {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY};

   enum MonthName {JANUARY = 1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST,
                   SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};

   static const std::string MONTH_NAME_TEXT[12] =
   {
      "Jan",  "Feb",  "Mar",  "Apr",  "May",  "Jun",
      "Jul",  "Aug",  "Sep",  "Oct",  "Nov",  "Dec"
   };
} // GmatTimeConstants


// Math Constants and Conversion for angles, mass, and length
namespace GmatMathConstants
{
   //Math constants
   static const Real PI_DEG = 180.0;
   static const Real PI_OVER_TWO_DEG = 90.0;
   static const Real TWO_PI_DEG = 360.0;
   static const Real PI = 3.14159265358979323846264338327950288419716939937511;
   static const Real TWO_PI = 6.28318530717958647692528676655900576839433879875022;
   static const Real PI_OVER_TWO = 1.57079632679489661923132169163975144209858469968756;
   static const Real E = 2.71828182845904523536028747135266249775724709369996;

   //Angle conversion
   static const Real RAD_PER_DEG =
      3.14159265358979323846264338327950288419716939937511 / 180.0;
   static const Real DEG_PER_RAD=
      180.0 / 3.14159265358979323846264338327950288419716939937511;

   static const Real ARCSEC_PER_DEGREE = 3600.0;
   static const Real DEG_PER_ARCSEC = 1.0 / 3600.0;
   static const Real RAD_PER_ARCSEC = DEG_PER_ARCSEC * RAD_PER_DEG;

   //Mass conversion
   static const Real LBM_TO_KG = 0.45359237;
   static const Real SLUG_TO_KG = 14.59390294;

   //Length
   static const Real INCH_TO_M = 0.0254;
   static const Real FOOT_TO_M = 0.3048;
   static const Real STATUTE_MILE_TO_M = 1609.344;
   static const Real M_TO_KM = 0.001;
   static const Real KM_TO_M = 1000.0;

   // NaN
   static const Real QUIET_NAN = std::numeric_limits<Real>::quiet_NaN();

   enum SIGN {PLUS = 1, MINUS = -1 };

} // GmatMathConstants

// Constants used in orbital element conversion between Cartesion, Keplerian, ModKeplerian, etc.
namespace GmatOrbitConstants
{
   // Undefined
   const static Real ORBIT_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

   // Tolerance
   const static Real KEP_TOL         = 1.0e-11;
   const static Real KEP_ANOMALY_TOL = 1.0e-12;
   const static Real KEP_ZERO_TOL    = 1.0e-30;
   const static Real KEP_ECC_TOL     = 1.0e-7;  // Determined to using numerical experiments
                                             // after multiple transformations
   const static Real ECC_RANGE_TOL   = 1.0e-5;

} // GmatOrbitConstants

// Constants used in attitude conversion between DCM, Quaternion, etc.
namespace GmatAttitudeConstants
{
   // Undefined
   const static Real ATTITUDE_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

   // Tolerance
   const Real     QUAT_MIN_MAG                 = 1.0e-10;
   const Real     EULER_ANGLE_TOLERANCE        = 1.0E-10;
   const Real     DCM_ORTHONORMALITY_TOLERANCE = 1.0e-14;

} // GmatAttitudeConstants


#endif // GmatConstants_hpp
