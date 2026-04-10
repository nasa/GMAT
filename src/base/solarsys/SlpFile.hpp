//$Id$
//------------------------------------------------------------------------------
//                                  SlpFile
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
// Author: Wendy C. Shoan
// Created: 2004/02/10
//
/**
 * This is the class that handles access to an SLP file.
 *
 * @note This class was created from Swingby code.  For list of major mods,
 *       see the .cpp file.
 * @note For Build 2, only Sun/Earth/Moon are assumed - later, change MAX_BODIES!
 */
//------------------------------------------------------------------------------
#ifndef SlpFile_hpp
#define SlpFile_hpp

#include "gmatdefs.hpp"
#include "PlanetaryEphem.hpp"
#include "A1Mjd.hpp"

class GMAT_API SlpFile : public PlanetaryEphem
{
public:

   // default constructor
   SlpFile(std::string withFileName);
   // copy constructor
   SlpFile(const SlpFile& slpf);
   // operator=
   SlpFile& operator=(const SlpFile& slpf);
   // destructor
   ~SlpFile();

   // method to return the body ID number, given the name of the body
   Integer GetBodyID(std::string bodyName);

   // method to return the position and velocity of the specified body
   // at the specified time
   Real* GetPosVel(Integer forBody, A1Mjd atTime, 
                   bool overrideTimeSystem = false);

   // method to return the day-of-year and year of the start time of the
   // SLP file.
   Integer* GetStartDayAndYear();

   // from slp_data_types.h
   // length of SLP header records
   static const Integer LRECLHD;
   // length of SLP data records
   static const Integer LRECLDT;
   // ??????
   static const Integer IBEPM;
   // ??????
   static const Integer SLP_LEN;
   // ??????
   static const Integer TIM_LEN;

   static const Integer SUN_ID;
   static const Integer MERCURY_ID;
   static const Integer VENUS_ID;
   static const Integer EARTH_ID;
   static const Integer MOON_ID;
   static const Integer MARS_ID;
   static const Integer JUPITER_ID;
   static const Integer SATURN_ID;
   static const Integer URANUS_ID;
   static const Integer NEPTUNE_ID;
   static const Integer PLUTO_ID;

   static const Real    JD_MJD_OFFSET;// = GmatTimeConstants::JD_JAN_5_1941;  // NOTE - SLP code uses this as a long int!!!!

   // maximum length of path name
   static const Integer MAX_PATH_LEN;

   // from Swingby bodies_types.h
   // Max number of bodies that can be modeled
   static const Integer MAX_BODIES = 3;
   // Max number of zonal values that are enterable
   static const Integer MAX_ZONALS;
   // Max length of the name of a potential field name
   static const Integer MAX_POTENTIAL_NAME;
   // The number of bodies normally found on the planetary ephemeris file
   static const Integer NUM_STANDARD_BODIES;

protected:
   //------------------------------------------------------------------------------
   //  SLP_HEADER:
   //     Define the structure of the Code 550 Solar/Lunar/Planetary
   //     ephemeris file.
   //
   //  Reference:
   //     Data Set Layouts for the Goddard Trajectory Determination System
   //     (GTDS), Revision 2.  1988.  Aaronson, K. and D. Squier.  Pg. FRN-14.
   //     CSC/SD-88/6110
   //------------------------------------------------------------------------------
   struct slp_header
   {
      long   iday1;           // day number of start of file (DDD)
      long   iyear;           // year of start of file (YYYY)
      long   ispan;           // number of curve-fit intervals
      long   nbepm[9];        // body indicators:
                              //   nbepm[0] = central body (typically earth)
                              //   nbepm[1] = fast-moving noncentral body
                              //       (typically moon)
                              //   nbepm[2] to nbepm[8] = slow-moving 
                              //       noncentral bodies (typically planets
                              //       omitting mercury and venus; this can
                              //       change)
      long   ndegre[4];       // degree of curve-fit polynomials for:
                              //   ndegre[0] = rotation matrices
                              //   ndegre[1] = fast-moving noncentral body
                              //       position
                              //   ndegre[2] = fast-moving noncentral body
                              //       velocity
                              //   ndegre[3] = slow-moving noncentral body
                              //       positions
      long   ncfday;          // number of days per curve-fit interval
      long   islp50;          // coordinate system indicator:
                              //   1 = Mean-equator-and-equinox-of-B1950.0
                              //   2 = True-of-date using theory and constants
                              //       for mean-equator-and-equinox-of-B1950.0
                              //   3 = Mean-equator-and-equinox-of-J2000.0
                              //   4 = True-of-date using theory and constants
                              //       for mean-equator-and-equinox-of-J2000.0
      long   nbslp;           // number of bodies in the curve-fit
      long   icdate[5];       // file generation date:
                              //   icdate[0] = Year   (YY)
                              //   icdate[1] = Month  (MM)
                              //   icdate[2] = Day    (DD)
                              //   icdate[3] = Hour   (HH)
                              //   icdate[4] = Minute (MM)
      double rsec;            // seconds part of file generation date (SS.SSS)
      char   spare[2160];     // spares

   };


   
private:

   // from Swingby slp_file.h
   // method to open the SLP file and read the header
   int open_slp (int slp_header[]);
   // method to read the SLp file and return two data records
   int read_slp (double time, double elapsec,
                 int islphdo[], int islpdto[], double *eptime);
   // method to return the position of the requested body at the requested time
   int slp_pos (double time,  double elapst, int nbody, int ibody[],
                double posslp[MAX_BODIES][3]);
   // method to return the velocity of the requested body at the requested time
   int slp_vel (double time, double elapst, int nbody, int ibody[],
                double velslp[MAX_BODIES][3]);
   // do we need these two?
//   char *get_slp_error(int error, char *string);
//   char *get_a1_utc_offset_error(int error, char *string);

   
   // from time_util.h - we can switch to using GMAT time utilities later
   void time_array_to_utcmjd(double starray[6], double *utcmjd);
   int a1_utc_offset(double mjd, double *a1utc, double *ut1utc, double *tdtutc);

   void InitializeSlpFile();

   std::string itsName;

   // from slp_data_types.h
   // length of planetary ephemeris header records
   Integer lengthOfHeaderRecord;
   // length of planetary ephemeris data records
   Integer lengthOfDataRecord;
   // ??????
   Integer ibepm;

   // NOTE: these were externs in Swingby; should use constants instead of
   // 566 and 1132 here
   // header record
   int g_slp_header[566];
   //int g_slp_data[LRECLDT];
   // two data records
   int g_slp_data[1132];


};
#endif // SlpFile_hpp
