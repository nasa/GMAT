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
// Author: Wendy C. Shoan
// Created: 2004/02/10
//
/**
 * Implementation of the SlpFile class.
 *
 * @note This class was created from Swingby code.  Major mods were:
 * - A1_MJD type changed to double (as typedef'ed in swingby)
 * - use bool. true, false instead of C_BOOL C_TRUE, C_FALSE
 * - changed #defines to static const parameters
 * - removed _MAX_PATH (not standard on platforms) - use new static
 *   const MAX_PATH_LEN instead
 * - replaced call to SECS_TO_DAYS with division by 86400.00
 */
//------------------------------------------------------------------------------

#include <memory.h>          // for memcpy()
#include <stdio.h>           // for fopen()
#include <math.h>            // for floor()
#include "SolarSystem.hpp"   // for body names
#include "SlpFile.hpp"
#include "PlanetaryEphem.hpp"
#include "PlanetaryEphemException.hpp"
#include "TimeSystemConverter.hpp"
#include "TimeTypes.hpp"
#include "StringUtil.hpp"    // for ToString()

// length of SLP header records
const Integer SlpFile::LRECLHD             = 566;  // if change these, change SlpFile.h private data too
// length of SLP data records
const Integer SlpFile::LRECLDT             = 1132;
// ??????
const Integer SlpFile::IBEPM               = 9;
// ??????
const Integer SlpFile::SLP_LEN             = 2264;
// ??????
const Integer SlpFile::TIM_LEN             = 248;

const Integer SlpFile::SUN_ID              = 3;
const Integer SlpFile::MERCURY_ID          = 10;
const Integer SlpFile::VENUS_ID            = 11;
const Integer SlpFile::EARTH_ID            = 1;
const Integer SlpFile::MOON_ID             = 2;
const Integer SlpFile::MARS_ID             = 4;
const Integer SlpFile::JUPITER_ID          = 5;
const Integer SlpFile::SATURN_ID           = 6;
const Integer SlpFile::URANUS_ID           = 7;
const Integer SlpFile::NEPTUNE_ID          = 8;
const Integer SlpFile::PLUTO_ID            = 9;

// maximum length of path name
const Integer SlpFile::MAX_PATH_LEN        = 260;

// Max number of bodies that can be modeled
//const Integer PlanetaryEphem::MAX_BODIES          = 3;             // increase later -> 20;
// Max number of zonal values that are enterable
const Integer SlpFile::MAX_ZONALS          =  5;
// Max length of the name of a potential field name
const Integer SlpFile::MAX_POTENTIAL_NAME  = 72;
// The number of bodies normally found on the SLP file
const Integer SlpFile::NUM_STANDARD_BODIES = 11;

const Real    SlpFile::JD_MJD_OFFSET = GmatTimeConstants::JD_JAN_5_1941;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SlpFile(std::string withFileName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SlpFile class
 * (default constructor).
 *
 * @param <name> parameter indicating the full path name of the SLP File.
 */
//------------------------------------------------------------------------------
SlpFile::SlpFile(std::string withFileName) :
PlanetaryEphem(withFileName)
{
   InitializeSlpFile();
}

//------------------------------------------------------------------------------
//  SlpFile(const SlpFile& slpf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SlpFile class
 * (constructor).
 *
 * @param <slpf> SlpFIle object whose values to copy to the new SLP File.
 */
//------------------------------------------------------------------------------
SlpFile::SlpFile(const SlpFile& slpf) :
PlanetaryEphem(slpf)
{
   InitializeSlpFile();  // is it correct to do this?  or do I want to copy all of the stuff?
   int i;
   for (i=0;i<lengthOfHeaderRecord;i++)
      g_slp_header[i] = slpf.g_slp_header[i];
   for (i=0;i<lengthOfDataRecord;i++)
      g_slp_data[i] = slpf.g_slp_data[i];
   ibepm            = slpf.ibepm;
}

//------------------------------------------------------------------------------
//  SlpFile& operator= (const SlpFile& slpf)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SlpFile class.
 *
 * @param <slpf> the SlpFile object whose data to assign to "this"
 *            SLP File.
 *
 * @return "this" SlpFile with data of input SlpFile slpf.
 */
//------------------------------------------------------------------------------
SlpFile& SlpFile::operator=(const SlpFile& slpf)
{
   if (this == &slpf) return *this;
   PlanetaryEphem::operator=(slpf);
   int i;
   for (i=0;i<lengthOfHeaderRecord;i++)
      g_slp_header[i] = slpf.g_slp_header[i];
   for (i=0;i<lengthOfDataRecord;i++)
      g_slp_data[i] = slpf.g_slp_data[i];
   ibepm            = slpf.ibepm;
   // set class data
   return *this;
}

//------------------------------------------------------------------------------
//  ~SlpFile()
//------------------------------------------------------------------------------
/**
 * Destructor for the SlpFile class.
 */
//------------------------------------------------------------------------------
SlpFile::~SlpFile()
{
}

//------------------------------------------------------------------------------
//  Integer GetBodyID(std::string bodyName)
//------------------------------------------------------------------------------
/**
* This method returns the body ID for the specified body.
 *
 * @param <bodyName> body whose ID is requested.
 *
 * @return body ID for the specified body. Returns -1 if invalid body name.
 *
 */
//------------------------------------------------------------------------------
Integer SlpFile::GetBodyID(std::string bodyName)
{
   if (bodyName == GmatSolarSystemDefaults::SUN_NAME)     return SlpFile::SUN_ID;
   if (bodyName == GmatSolarSystemDefaults::MERCURY_NAME) return SlpFile::MERCURY_ID;
   if (bodyName == GmatSolarSystemDefaults::VENUS_NAME)   return SlpFile::VENUS_ID;
   if (bodyName == GmatSolarSystemDefaults::EARTH_NAME)   return SlpFile::EARTH_ID;
   if (bodyName == GmatSolarSystemDefaults::MOON_NAME)    return SlpFile::MOON_ID;
   if (bodyName == GmatSolarSystemDefaults::MARS_NAME)    return SlpFile::MARS_ID;
   if (bodyName == GmatSolarSystemDefaults::JUPITER_NAME) return SlpFile::JUPITER_ID;
   if (bodyName == GmatSolarSystemDefaults::SATURN_NAME)  return SlpFile::SATURN_ID;
   if (bodyName == GmatSolarSystemDefaults::URANUS_NAME)  return SlpFile::URANUS_ID;
   if (bodyName == GmatSolarSystemDefaults::NEPTUNE_NAME) return SlpFile::NEPTUNE_ID;
   if (bodyName == GmatSolarSystemDefaults::PLUTO_NAME)   return SlpFile::PLUTO_ID;

   return -1;
}

//------------------------------------------------------------------------------
//  Real* GetPosVel(Integer forBody, A1Mjd atTime, overrideTimeSystem)
//------------------------------------------------------------------------------
/**
 * This method returns the position and velocity of the specified body at the
 * requested time.
 *
 * @param <forbody>  the requested body (number on SLP file).
 * @param <atTime>   time for which state of the body is requested.
 * @param <overrideTimeSystem> currently ignored for SLP file ******
 *
 * @return state of the body at the requested time.
 *
 * @exception <PlanetaryEphemException> thrown if the position or velocity cannot
 *            be obtained due to problems opening or reading the SLP Fle;
 *            message is based on error
 */
//------------------------------------------------------------------------------
Real* SlpFile::GetPosVel(Integer forBody, A1Mjd atTime, bool overrideTimeSystem)
{
   double    forTime            = (double) atTime.Get();

   int       iBody[1]           = {forBody};
   double    pos[MAX_BODIES][3];
   double    vel[MAX_BODIES][3];
   static Real      result[6];
   
   int posCode     = slp_pos(forTime, 0.0, 1, iBody, pos);
   //      0 = No error
   //      1 = Invalid number of bodies
   //      3 = Requested body not on SLP file
   //     10 = Modified julian date not on time coefficients file
   //     11 = Invalid slp record number, time requested may not be on file
   //     12 = Error returned from reading slp file
   //     13 = Error opening slp file
   //     14 = Error reading time coefficients file
   //     23 = SLP file is not J2000
   switch (posCode)
   {
      case 1:
         throw PlanetaryEphemException("Invalid number of bodies\n");
         break;
      case 3:
         //throw PlanetaryEphemException("Requested body not on SLP File\n");
         throw PlanetaryEphemException
            ("Requested body: " + GmatStringUtil::ToString(forBody) +
             " not on SLP File\n");
         break;
      case 10:
         throw PlanetaryEphemException(
               "Modified Julian Date not on timing coefficients file\n");
         break;
      case 11:
         throw PlanetaryEphemException(
               "Invalid SLP record number - time may not be on file\n");
         break;
      case 12:
         throw PlanetaryEphemException("Error reading SLP File\n");
         break;
      case 13:
         throw PlanetaryEphemException("Error opening SLP File\n");
         break;
      case 14:
         throw PlanetaryEphemException("Error reading timing coefficients file\n");
         break;
      case 23:
         throw PlanetaryEphemException("SLP File is not J2000\n");
         break;
      default:
         break;
   }
   if (posCode > 0)
      throw PlanetaryEphemException("Unknown error from slp_pos\n");

   int velCode     = slp_vel(forTime, 0.0, 1, iBody, vel);
   switch (velCode)
   {
      case 1:
         throw PlanetaryEphemException("Invalid number of bodies\n");
         break;
      case 3:
         //throw PlanetaryEphemException("Requested body not on SLP File\n");
         throw PlanetaryEphemException
            ("Requested body: " + GmatStringUtil::ToString(forBody) +
             " not on SLP File\n");
         break;
      case 10:
         throw PlanetaryEphemException(
                                "Modified Julian Date not on timing coefficients file\n");
         break;
      case 11:
         throw PlanetaryEphemException(
                                "Invalid SLP record number - time may not be on file\n");
         break;
      case 12:
         throw PlanetaryEphemException("Error reading SLP File\n");
         break;
      case 13:
         throw PlanetaryEphemException("Error opening SLP File\n");
         break;
      case 14:
         throw PlanetaryEphemException("Error reading timing coefficients file\n");
         break;
      case 23:
         throw PlanetaryEphemException("SLP File is not J2000\n");
         break;
      default:
         break;
   }
   if (velCode > 0)
      throw PlanetaryEphemException("Unknown error from slp_pos\n");


   result[0] = (Real) pos[0][0];
   result[1] = (Real) pos[0][1];
   result[2] = (Real) pos[0][2];
   result[3] = (Real) vel[0][0];
   result[4] = (Real) vel[0][1];
   result[5] = (Real) vel[0][2];
   return result;
}

//------------------------------------------------------------------------------
//  Integer* GetStartDayAndYear()
//------------------------------------------------------------------------------
/**
 * This method returns the start day-of-year and year of the SLP file.
 *
 * @return start day-of-year and year of the SLP file.
 *
 * @exception <PlanetaryEphemException> thrown if there is an error opening or
 *                                      reading the SLP File, or if it is not
 *                                      J2000
 */
//------------------------------------------------------------------------------
Integer* SlpFile::GetStartDayAndYear()
{
   int err = open_slp(g_slp_header);
   //     0 = no error
   //    12 = error reading slp file
   //    13 = error opening the SLP file  (WCS: added comment 2004/02/17)
   //    23 = slp file is not j2000
   if (err == 12)
      throw PlanetaryEphemException(
                             "There is an error reading the SLP File, on opening\n");
   if (err == 13)
      throw PlanetaryEphemException(
                             "There is an error opening the SLP File\n");
   if (err == 23)
      throw PlanetaryEphemException(
                        "SLP File is not J2000\n");
   if (err > 0)
      throw PlanetaryEphemException(
                             "Unknown error opening the SLP File\n");
   
   Integer* res = new Integer[2];
   if (err == 0)
   {
      res[0] = g_slp_header[0];
      res[1] = g_slp_header[1];
   }
   else
   {
      res[0] = 0;
      res[1] = 0;
   }
   return res;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  int open_slp (int slp_header[])
//------------------------------------------------------------------------------
//  Purpose:
//     Opens the SLP file and reads the file header.
//
//  Reference:
//     j2000 coordinate conversion software mathematical background and system
//     description (csc/sd-89/6148)
//
//  Arguments:
//     <slp_header>   o   SLP header record (the size is contaied in 'slp_data.h')
//
//  Returns:
//     0 = no error
//    12 = error reading slp file
//    13 = error opening the SLP file  (WCS: added comment 2004/02/17)
//    23 = slp file is not j2000
//
//  Modifications:
//     06/28/90  E. Graham - Added to rsl
//     05/14/91  M. Woolsey - New version added to rsl
//     01/28/92  J. Carrico - Converted open() to PC for Swingby
//     12/03/97  D. Gates - Converted to C
//     07/26/01  L. Jun - Changed openslp() to open_slp()
//------------------------------------------------------------------------------
int SlpFile::open_slp (int slp_header[])
{

   //-----------------------------------
   //  External variables
   //-----------------------------------
   //extern DCB_TYPE g_pef_dcb;          / WCS - changed to dcb_type & 
   //extern int g_slp_header[LRECLHD];   / moved to header as private data

   int ierr;
   ierr = 0;

   // open only if not alread open.
   if (g_pef_dcb.fptr == NULL)
   {

      g_pef_dcb.fptr = fopen(g_pef_dcb.full_path, "rb");

      if (g_pef_dcb.fptr != NULL)
         g_pef_dcb.recl = SLP_LEN;
      else
         ierr = 13;

      if (ierr == 0)
      {
         // CALL fread TO READ THE SLP HEADER FILE
         if (fread((void*)g_slp_header,sizeof(g_slp_header),1,g_pef_dcb.fptr))
         {
            // IF INVALID COORDINATE SYSTEM OF SLP HEADER FILE THEN
            if ((g_slp_header[17] != 3) && (g_slp_header[17] != 4))
            {
               ierr = 23;
            }
            else
            {
               // COPY HEADER RECORD TO OUTPUT
               memcpy(slp_header, g_slp_header, lengthOfHeaderRecord * sizeof(int));
            }
         }
         else
         {
            ierr = 12;
         }
      }
   }
   else
   {
      memcpy(slp_header, g_slp_header, lengthOfHeaderRecord * sizeof(int));
   }

   return ierr;
}


//------------------------------------------------------------------------------
//  int read_slp (A1_MJD time, double elapsec, int islphdo[], int g_slp_datao[],
//                double *eptime)
//------------------------------------------------------------------------------
//  Purpose:
//     Given an A1 modified julian date, this function returns polynomials to
//     propagate SLP data for it.
//
//  Arguments:
//     <time>       i  A1 modified julian date of base epoch, days
//     <elapsec>    i  A1 elapsed seconds from base epoch, seconds
//     <islphdo[]>  o  slp file header reacord
//     <islpdto[]>  o  two slp data records
//     <eptime>     o  time difference between requested time and coefficients
//                     A1 days
//
//  Returns:
//      0 = No error
//     10 = Modified julian date not on time coefficients file
//     11 = Invalid slp record number, time requested may not be on file
//     12 = Error returned from reading slp file
//     13 = Error opening slp file
//     14 = Error reading time coefficients file
//     23 = slp file is not j2000
//
//  Notes:
//     The SLP file accounts for tiems in an unusual way. The data record contains
//     a variable TSEC, which is the UTC number of elapsed seconds from the
//     starting epoch plus the A1 - UTC offset.  So adding TSEC to the UTC
//     modified julian date (converting units), you get out the atomic MJD of
//     the timie on the data record.
//
//  Modifications:
//     06/28/90  E. Graham   - Added to rsl
//     05/14/91  M. Woolsey  - New version added to rsl
//     01/29/92  J. Carrico  - Moved DATA statement (SLPYR) to be after declarations
//     12/03/97  D. Gates    - Converted to C
//     06/08/01  L. Jun      - Used renamed function open_slp()
//------------------------------------------------------------------------------
int SlpFile::read_slp (double time, double elapsec, int islphdo[], int islpdto[],
              double *eptime)
{
   //-----------------------------------
   //  External variables
   //-----------------------------------
   //extern int g_slp_header[LRECLHD];
   //extern int g_slp_data[LRECLDT];
   //extern DCB_TYPE g_pef_dcb;

   int iret;

   // DECLARE LOCAL VARIABLES
   int idayr;
   int irec, ierr, i2;
   static double slpyr [6] = {0.0,1.0,1.0,0.0,0.0,0.0};
   // UNPACKED FORMAT OF THE SLP FILE BASE EPOCH
   int *iyear = g_slp_header + 1;
   int *iday1 = g_slp_header;
   int *ncfday = g_slp_header + 16;
   int *ispan = g_slp_header + 2;
   int *iday = g_slp_data + 1126;
   double *tsec = (double*)(g_slp_data+566);
   int *islpdt1 = g_slp_data + lengthOfHeaderRecord;

   double slptpt;
   double tme;
   static double djulsps;                   // LAST EPOCH MJD OF FILE
   static double slpzer;                    // A1 - UTC OFFSET
   //static C_BOOL called_diffutc = C_FALSE;
   static bool called_diffutc = false;
   double ut1utc;                    // NOT USED
   double tdtutc;                    // NOT USED
   int ircmap[3] = {14, 10, 10};     // RETURN CODE MAP BETWEEN
                                     // DIFFUTC AND J2K PACKAGE

   // INITIALIZE ERROR FLAG
   iret = 0;
   ierr = 0;

   // IF SLP FILE HAS NOT BEEN OPENED BEFORE
   if (g_pef_dcb.fptr == NULL)
   {

      // OPEN THE SLP FILE
      if ((iret = open_slp (g_slp_header)) == 0)
      {
        // called_diffutc = C_FALSE;
         called_diffutc = false;

         // RESET THE LAST SAVED RECORD NUMBER
         *iday = -1 - *ncfday;
      }
   }
   if ((!called_diffutc) && (iret==0))
   {

      // CALCULATE THE MODIFIED JULIAN DATE OF THE STARTING EPOCH
      slpyr[0] = (double)*iyear;
      //loj: calmjd (slpyr, &djulsps);
      time_array_to_utcmjd(slpyr, &djulsps);

      // COMPUTE THE MODIFIED JULIAN DATE OF THE STARTING RECORD ON THE SLP FILE
      slptpt = djulsps + (double)*iday1;

      // CALCULATE THE DIFFERENCE IN SECONDS BETWEEN A.1 AND UTC TIME SYSTEMS
      //loj: ierr = diffutc ((A1_MJD)slptpt, &slpzer, &ut1utc, &tdtutc);
      //ierr = a1_utc_offset((A1_MJD)slptpt, &slpzer, &ut1utc, &tdtutc);
      ierr = a1_utc_offset((double)slptpt, &slpzer, &ut1utc, &tdtutc);

      // IF ERROR RETURNED FROM DIFFUTC THEN RETURN THE J2000 ERROR CODE
      if (ierr != 0)
         iret = ircmap[ierr-1];
      else
         //called_diffutc = C_TRUE;
         called_diffutc = true;

   }

   if (iret == 0)
   {

      // COMPUTE THE NUMBER OF DAYS TO READ INTO THE SLP FILE
      tme = (double)time - djulsps;
      //idayr = (int)floor(tme + SECS_TO_DAYS(elapsec - slpzer)) + 1;
      idayr = (int)floor(tme + ((elapsec - slpzer)/GmatTimeConstants::SECS_PER_DAY)) + 1;

      // IF A NEW RECORD IS NEEDED THEN
      if (idayr < *iday || idayr > (*iday + *ncfday))
      {

         // COMPUTE RECORD NUMBER TO READ FROM THE SLP FILE
         irec = ((idayr - *iday1) / *ncfday) + 1;

         // IF THE SLP FILE RECORD NUMBER IS INVALID THEN
         if (irec <= 0 || irec > *ispan)
         {
            iret = 11;
         }

         if (iret == 0)
         {
            i2 = 2 * irec - 1;

            // READ THE FIRST RECORD OF SLP DATA
            iret = fseek(g_pef_dcb.fptr,(long)(i2*lengthOfHeaderRecord*sizeof(int)),0);

            if (iret == 0)
            {
               if (fread((void*)islpdt1,lengthOfHeaderRecord*sizeof(int),1,g_pef_dcb.fptr))
               {
                  if (!fread((void*)g_slp_data,lengthOfHeaderRecord*sizeof(int),1,g_pef_dcb.fptr))
                  {
                     // CLEAR LAST RECORD READ
                     *iday = -1 - *ncfday;

                     // SET ERROR RETURN CODE
                     iret = 12;
                  }
               }
               else
               {
                  // CLEAR LAST RECORD READ
                  *iday = -1 - *ncfday;

                  // SET ERROR RETURN CODE
                  iret = 12;
               }
            }
            else
            {
               // CLEAR LAST RECORD READ
               *iday = -1 - *ncfday;

               // SET ERROR RETURN CODE
               iret = 12;
            }
         }
      }

      if (iret == 0)
      {

         // COPY THE LAST READ SLP DATA INTO OUTPUT BUFFER
         memcpy(islpdto, g_slp_data, lengthOfDataRecord*sizeof(int));

         // COPY THE LAST READ SLP HEADER DATA INTO OUTPUT BUFFER
         memcpy(islphdo, g_slp_header, lengthOfHeaderRecord*sizeof(int));

         // CALCULATE THE TIME DIFFERENCE BETWEEN THE REQUESTED TIME AND
         // THE TIME OF THE COEFFICIENTS
         //*eptime = tme + SECS_TO_DAYS(elapsec - *tsec);
         *eptime = tme + ((elapsec - *tsec)/GmatTimeConstants::SECS_PER_DAY);
      }
   }

   return iret;
}


//------------------------------------------------------------------------------
//  int slp_pos (A1_MJD time,  double elapst, int nbody, int ibody[],
//               double posslp[MAX_BODIES][3])
//------------------------------------------------------------------------------
//  Purpose:
//     Gets the position of requested SLP bodies at the requested time.
//
//  Reference:
//     J2000 coordinate conversion software mathematical background and system
//     description (csc/sd-89/6148)
//
//  Arguments:
//     <time>     i   epoch time in A1 modified julian date(days)
//     <elapst>   i   elapsed atomic time requested (seconds)
//     <nbody>    i   number of SLP bodies
//     <ibody[]>  i   SLP bodies requested
//                    1 = Earth,   2 = Moon,    3 = Sun,    4 = Mars,
//                    5 = Jupiter, 6 = Saturn,  7 = Uranus, 8 = Neptune,
//                    9 = Pluto,  10 = Mercury, 11 = Venus
//
//     <posslp[MAX_BODIES][3]>  o  position vectors (km)
//
//  Returns:
//      0 = No error
//      1 = Invalid number of bodies
//      3 = Requested body not on SLP file
//     10 = Modified julian date not on time coefficients file
//     11 = Invalid slp record number, time requested may not be on file
//     12 = Error returned from reading slp file
//     13 = Error opening slp file
//     14 = Error reading time coefficients file
//     23 = SLP file is not J2000
//
//  Notes:
//     This function uses a merging index: it doesn't identify a slow moving
//     body by simple sequential search.  The Index is reset on each call.  This
//     makes the function faster if the SLP bodies are requested in the same
//     order as on the SLP file (normally ascending).  The function will still
//     solver for the positions if the bodies aren't in this order, just not as
//     quick.
//
//  Modifications:
//     02/25/91  E. Graham  - Entered into RSL
//     01/29/92  J. Carrico - Re-ordered argument declarations for NBODY
//     12/08/92  D. Gates   - Converted to C
//------------------------------------------------------------------------------
//int slp_pos (A1_MJD time,  double elapst, int nbody, int ibody[],
//             double posslp[MAX_BODIES][3])
int SlpFile::slp_pos (double time,  double elapst, int nbody, int ibody[],
             double posslp[MAX_BODIES][3])
{
   //---------------------------------------------------------------------------
   // local variables
   // ppoly[3][20]      polynomial coeffcients for fast-moving non-central body
   //                   positions
   // ppoly2[3][13][7]  polynomial coeffcients for slow-moving non-central body
   //                   positions
   // ndegre[4]         degrees of curve-fit polynomials
   //                      1 = ratation matrices
   //                      2 = fast-moving non-central body position
   //                      3 = fast-moving non-central body velocity
   //                      4 = slow-moving non-central body position number of days
   //                          per curve-fit
   //---------------------------------------------------------------------------
   int ircode;
   int  mtim, ii, jj, kk, ll;
   int  ndxslow;
   int  ndxmrge;
   double eptime;
   double temp;
   int islphd[SlpFile::LRECLHD], islpdt[SlpFile::LRECLDT];
   int working;
   int *nbepm = islphd+3;
   int *ndegre = islphd+12;
   double *ppoly1=(double*)((int*)(islpdt+568));
   double *ppoly2=(double*)islpdt;

   ircode= 0;
   ndxmrge = 0;

   if (nbody <= 0)
   {
      ircode= 1;
   }
   else
   {

      // READ THE SLP FILE
      //if ((ircode = readslp (time, elapst, islphd, islpdt, &eptime)) == 0)
      if ((ircode = read_slp (time, elapst, islphd, islpdt, &eptime)) == 0)
      {
         
         // compute the position vector matrix of the slp file for each requsted
         // body
         for (jj = 0; (ircode == 0) && (jj<nbody); jj++)
         {

            // do for each unchecked body on the slp file
            working = 1;

            for (ll=ndxmrge; working && ll<IBEPM; ll++)
            {

               // if slp body requested is the current body
               if (ibody[jj] == nbepm[ll])
               {

                  // set the merging index to the current index
                  ndxmrge = ll;

                  // break to calculate the position of the slp body
                  working = 0;

               }
            }

            // do for each body on the slp file
            for (ll=0; working && ll<ndxmrge; ll++)
            {

               // if slp body requested is the current body
               if (ibody[jj] == nbepm[ll])
               {

                  // set the merging index to the current index
                  ndxmrge = ll;

                  // break to calculate the position of the slp body
                  working = 0;

               }               
            }

            // no planet was found with the rueqsted id
            // set error code to 3
            if (working)
            {
               ircode = 3;
            }
            else
            {
               // compute position of the identified body

               // if the slp body is a slow-moving body
               if (ndxmrge >= 2)
               {
                  // compute the number of polynomial coefficients
                  mtim = ndegre[3];

                  // compute the position with respect to the central body
                  ndxslow = ndxmrge - 2;
                  for (ii=0; ii<3; ii++)
                  {
                     temp = 0.0;
                     for (kk = mtim; kk>0; kk--)
                     {
                        temp = (temp + ppoly2[(ndxslow*39)+(kk*3)+ii]) * eptime;
                     }
                     posslp[jj][ii] = temp + ppoly2[(ndxslow*39)+ii];
                  }
               }

               // else if the slp body is the central body
               else if (ndxmrge == 0)
               {
                  posslp[jj][0] = 0.0;
                  posslp[jj][1] = 0.0;
                  posslp[jj][2] = 0.0;
               }

               //else the slp body is fast-moving non-central body velocity
               else
               {

                  // compute the number of polynomial coefficients
                  mtim = ndegre[1];

                  // compute the velocity with respect to the central body
                  for (ii=0; ii<3; ii++)
                  {
                     temp = 0.0;
                     for (kk=mtim; kk>0; kk--)
                     {
                        temp = (temp + ppoly1[(kk*3)+ii]) * eptime;
                     }
                     posslp[jj][ii] = temp + ppoly1[ii];
                  }

               } //if (ndxmrge >= 2)
            } //if (working)
         } //for (jj = 0; (ircode == 0) && (jj<nbody); jj++)
      } //if ((ircode = readslp (time, elapst, islphd, islpdt, &eptime)) == 0)
      
   } //if (nbody <= 0)

   return ircode;
}

//------------------------------------------------------------------------------
//  int slp_vel (A1_MJD time, double elapst, int nbody, int ibody[],
//               double velslp[MAX_BODIES][3])
//------------------------------------------------------------------------------
//  Purpose:
//     Gets the velocity of requested SLP bodies at the requested time.
//
//  Reference:
//     J2000 coordinate conversion software mathematical background and system
//     description (csc/sd-89/6148)
//
//  Arguments:
//     <time>     i   epoch time in A1 modified julian date(days)
//     <elapst>   i   elapsed atomic time requested (seconds)
//     <nbody>    i   number of SLP bodies
//     <ibody[]>  i   SLP bodies requested
//                    1 = Earth,   2 = Moon,    3 = Sun,    4 = Mars,
//                    5 = Jupiter, 6 = Saturn,  7 = Uranus, 8 = Neptune,
//                    9 = Pluto,  10 = Mercury, 11 = Venus
//
//     <velslp[MAX_BODIES][3]>  o  velocity vectors (km)
//
//  Returns:
//      0 = No error
//      1 = Invalid number of bodies
//      3 = Requested body not on SLP file
//     10 = Modified julian date not on time coefficients file
//     11 = Invalid slp record number, time requested may not be on file
//     12 = Error returned from reading slp file
//     13 = Error opening slp file
//     14 = Error reading time coefficients file
//     23 = SLP file is not J2000
//
//  Notes:
//     This function uses a merging index: it doesn't identify a slow moving
//     body by simple sequential search.  The Index is reset on each call.  This
//     makes the function faster if the SLP bodies are requested in the same
//     order as on the SLP file (normally ascending).  The function will still
//     solver for the positions if the bodies aren't in this order, just not as
//     quick.
//
//  Modifications:
//     02/25/91  E. Graham - Entered into RSL
//     01/29/92  J. Carrico - Re-ordered argument declarations for NBODY
//     12/08/92  D. Gates - Converted to C
//     08/15/01  L. Jun - Renamed slpvel()
//------------------------------------------------------------------------------
//int slp_vel (A1_MJD time, double elapst, int nbody, int ibody[],
//             double velslp[MAX_BODIES][3])
int SlpFile::slp_vel (double time, double elapst, int nbody, int ibody[],
             double velslp[MAX_BODIES][3])
{
   //---------------------------------------------------------------------------
   // local variables
   //---------------------------------------------------------------------------
   //
   // ppoly2[3][13][7]  polynomial coeffcients for slow-moving non-central body
   //                   positions
   // ndegre[4]         degrees of curve-fit polynomials
   //                      1 = ratation matrices
   //                      2 = fast-moving non-central body position
   //                      3 = fast-moving non-central body velocity
   //                      4 = slow-moving non-central body position number of days
   //                          per curve-fit
   //---------------------------------------------------------------------------

   int ircode;

   int mtim, ii, jj, kk, ll;
   int ndxslow, ndxmrge;
   double dkk, temp, eptime;
   int islphd[SlpFile::LRECLHD], islpdt[SlpFile::LRECLDT];
   int working;
   int *nbepm = islphd+3;
   int *ndegre = islphd+12;
   double *vpoly1=(double*)((int*)(islpdt+688));
   double *ppoly2=(double*)((int*)islpdt);

   ircode= 0;
   ndxmrge = 0;

   if (nbody <= 0)
   {
      ircode= 1;
   }

   // read the SLP file
   //else if ((ircode = readslp(time, elapst, islphd, islpdt, &eptime)) == 0)
   else if ((ircode = read_slp(time, elapst, islphd, islpdt, &eptime)) == 0)
   {
      // compute the position vector matrix of the slp file for each requsted
      // body
      for (jj = 0; (ircode == 0) && (jj<nbody); jj++)
      {
         // do for each unchecked body on the slp file
         working = 1;
         for (ll=ndxmrge; working && ll<IBEPM; ll++)
         {
            // if slp body requested is the current body
            if (ibody[jj] == nbepm[ll])
            {
               // set the merging index to the current index
               ndxmrge = ll;

               // break to calculate the position of the slp body
               working = 0;
            }
         }

         // do for each body on the slp file
         for (ll=0; working && ll<ndxmrge; ll++)
         {
            // if slp body requested is the current body
            if (ibody[jj] == nbepm[ll])
            {

               // set the merging index to the current index
               ndxmrge = ll;

               // break to calculate the position of the slp body
               working = 0;
            }
         }

         // no planet was found with the rueqsted id
         // set error code to 3
         if (working)
         {
            ircode = 3;
         }
         else
         {
            // if the slp body is a slow-moving body
            if (ndxmrge >= 2)
            {
               // compute the number of polynomial coefficients
               mtim = ndegre[3];

               // compute the velocity with respect to the central body
               ndxslow = ndxmrge - 2;
               for (ii=0; ii<3; ii++)
               {
                  temp = 0.0;
                  dkk = (double)(mtim+1);
                  for (kk = mtim; kk>1; kk--)
                  {

                     dkk -= 1.0;
                     temp = (temp + dkk * ppoly2[(ndxslow*39)+(kk*3)+ii]) * eptime;
                  }
                 // velslp[jj][ii] = SECS_TO_DAYS(temp + ppoly2[(ndxslow*39)+3+ii]);
                  velslp[jj][ii] = ((temp + ppoly2[(ndxslow*39)+3+ii]))/GmatTimeConstants::SECS_PER_DAY;

               }
            }

            // else if the slp body is the central body
            else if (ndxmrge == 0)
            {
               velslp[jj][0] = 0.0;
               velslp[jj][1] = 0.0;
               velslp[jj][2] = 0.0;
            }

            //else the slp body is fast-moving non-central body velocity
            else
            {
               // compute the number of polynomial coefficients
               mtim = ndegre[2];

               // compute the velocity with respect to the central body
               for (ii=0; ii<3; ii++)
               {
                  temp = 0.0;
                  for (kk=mtim; kk>0; kk--)
                  {
                     temp = (temp + vpoly1[(kk*3)+ii]) * eptime;
                  }
                  velslp[jj][ii] = temp + vpoly1[ii];
               }

            } //if (ndxmrge >= 2)
         } //if (working)
      } //for (jj = 0; (ircode == 0) && (jj<nbody); jj++)
   } //if (nbody <= 0)

   return ircode;
}

//------------------------------------------------------------------------------
// "helper" functions from Swingby time_util.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  int a1_utc_offset(A1_MJD refmjd, double *a1utc, double *ut1utc, double *tdtutc)
//------------------------------------------------------------------------------
//  Purpose:
//     Computes the offsets in seconds between the utc, ut1, tdt, and a.1 time
//     systems.  The three offsets returned by this subroutine are the most
//     commonly used and are all based on the utc system.  They are calculated
//     by combining other offsets which are either derived from the timing
//     coefficients file or are defined constants like that used in the conversion
//     from a.1 to tdt.
//
//  References:
//     date and time handling package (dth) system description and
//     users guide (fdd/553-90/03, sd-90/6119)
//
//  Arguments:
//     <refmjd>   i   the utc modified julian date for which time offsets
//                    are calculated (mjd.fff)  
//                    ****************<< NOTE - refmjd is UTC, not A1!!!!!
//     <a1utc>    o   the number of offset seconds to be added to a utc time to
//                    derive the a.1 time (ss.sss)
//     <ut1utc>   o   the number of offset seconds to be added to a utc time to
//                    derive the ut1 time (ss.sss)
//     <tdtutc>   o   the number of offset seconds to be added to a utc time to
//                    derive the tdt time (ss.sss)
//
//  Returns:
//     0 = no errors
//     1 = file read error
//     2 = request time is out of file directory range
//     3 = request time is out of range the timing coefficents file
//
//  External variables:
//     <g_tcf_dcb>  i  DCB_TYPE
//
//  Error Handling: None
//
//  Notes:
//     The timing coefficient record read by tcf has 5 sets of conversion
//     coeficients (coef) and 5 time intervals (jarg) associated with the
//     coefficients.
//
//     Note the naming convention: 'a1utc' stands for 'a.1 minus utc'
//
//  Modifications:
//     05/20/90  C. Anderson - Added to rsl
//     06/27/91  M. Woolsey  - Added an open statement for the time
//     01/28/92  J. Carrico  - Converted open() to pc for swingby
//     01/29/98  R. Feiertag - Converted to C, reset error code on first return
//               from tcf.c.
//     06/01/01  L. Jun - Renamed diffutc() to a1_utc_offset().  Renamed
//               external varialbe TCF_DCB to g_tcf_dcb
//------------------------------------------------------------------------------
//int a1_utc_offset(A1_MJD refmjd, double *a1utc, double *ut1utc, double *tdtutc)
int SlpFile::a1_utc_offset(double refmjd, double *a1utc, double *ut1utc, double *tdtutc)
{
   //-------------------------
   // external variables
   //-------------------------
   //extern  DCB_TYPE  g_tcf_dcb;
  /*   - ***************** replace all of this Swingby code with GMAT code ***********
   double time, a1ut1;
   long   julian, i;
   int    error;

   // ensure that contents of coef and jarg are saved after return
   // save coef, jarg
   // initialize coef, jarg 

   //static float   coef [6][5] = {0.0};
   //static int    jarg [6] = {0};
   static float   coef [6][5] = {{0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 0.0}};
   static int    jarg [6] = {0, 0, 0, 0, 0, 0};

   error = 0;

   // initialize time offset in case of error return
   *a1utc  = 0.0;
   *ut1utc = 0.0;
   *tdtutc = 0.0;

   // adjust refmjd (which uses a 2430000.0 modification factor) by a
   // half day to match the 2430000.5 modification factor used by
   // the timing coefficients file, and integerize it.

   julian = (long)(refmjd - 0.5);

   // -- *************************
   //future_build: read_tcf()
   // -- *************************
   //     if (g_tcf_dcb.fptr == NULL)
   //     {
   //        // call tcf to open time coefficients file, imput request time
   //        // for contents of proper record of timing coefficients
   //        error = read_tcf(refmjd, coef, jarg);

   //        if (error == 3)
   //        {
   //           error = 1; // reset error code
   //        }
   //     }
   //     else
   //     {
   //        // if new refmjd is outside time range of previous record read
   //        if (!(julian >= jarg[0] && julian < jarg[5]))
   //        {
   //           // call tcf to read and return a new record
   //           error = read_tcf(refmjd, coef, jarg);
   //        }
   //     }

   if (error == 0)
   {

      for(i=0; i<5; i++)
      {
         // if the request time falls within the interval, stop searching
         if (julian >= jarg[i] && julian < jarg[i+1])
         {
            break;
         }
      }

      // if upper bound of interval is 99999, a sentinel time has been
      // reached, and the request time is out of range of the file

      if (jarg[5] >= 99999)
      {
         error = 3;
      }

      if (error == 0)
      {
         // set time argument of interpolation as time since start of interval
         time = (refmjd - .50) - (double)(jarg[i]) + 1.0;

         // evaluate a1utc conversion coefficients
         *a1utc = coef[0][i] + coef[1][i]*time + coef[2][i]*time*time;

         // evaluate a1ut1 conversion coefficients
         a1ut1 = coef[3][i] + coef[4][i]*time + coef[5][i]*time*time;

         // calculate the remaining conversion offsets
         *ut1utc = *a1utc - a1ut1;
         *tdtutc = GmatTimeConstants::TT_TAI_OFFSET - GmatTimeUtil::A1_TAI_OFFSET  + *a1utc;
      }
   }

   return error;
   */

   // the tdt-tai and a.1-tai values used  are constant offsets

 
   // interpolations
   // 20.02.06 - arg: changed to use enum types instead of strings
//   Real mjdA1  = theTimeConverter->Convert(refmjd,
//                 "UtcMjd", "A1Mjd", GmatTimeConstants::JD_JAN_5_1941);
   Real mjdA1  = theTimeConverter->Convert(refmjd,
                 TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD,
                 GmatTimeConstants::JD_JAN_5_1941);
   *a1utc      = (mjdA1 - refmjd) * GmatTimeConstants::SECS_PER_DAY;
//   Real mjdUT1 = theTimeConverter->Convert(refmjd,
//                 "UtcMjd", "Ut1Mjd", GmatTimeConstants::JD_JAN_5_1941);
   Real mjdUT1 = theTimeConverter->Convert(refmjd,
                 TimeSystemConverter::UTCMJD, TimeSystemConverter::UT1MJD,
                 GmatTimeConstants::JD_JAN_5_1941);
   *ut1utc     = (mjdUT1 - refmjd) * GmatTimeConstants::SECS_PER_DAY;
//   Real mjdTT  = theTimeConverter->Convert(refmjd,
//                 "UtcMjd", "TtMjd", GmatTimeConstants::JD_JAN_5_1941);
   Real mjdTT  = theTimeConverter->Convert(refmjd,
                 TimeSystemConverter::UTCMJD, TimeSystemConverter::TTMJD,
                 GmatTimeConstants::JD_JAN_5_1941);
   *tdtutc     = (mjdTT - refmjd) * GmatTimeConstants::SECS_PER_DAY;
   return 0;

   
} // end a1_utc_offset()

//------------------------------------------------------------------------------
//  void time_array_to_utcmjd(double starray[6], double *utcmjd)
//------------------------------------------------------------------------------
//  Purpose:
//     Converts standard time array to utc modified julian date.
//
//  Arguments:
//     <starray[6]>  i  time array
//     <*utcmjd>     o  utc mjd
//
//  Modifications:
//     05/20/90  C. Anderson  - Added to rsl
//     11/12/97  R. Feiertag  - Converted to C
//     06/11/01  L. Jun - Renamed calmjd() to time_array_to_utcmjd()
//------------------------------------------------------------------------------
void SlpFile::time_array_to_utcmjd(double starray[6], double *utcmjd)
{
   double        fraction;
   long          i, j, k, julday;

   // integerize input year, month and day
   i = (long)starray[0];
   j = (long)starray[1];
   k = (long)starray[2];

   // calculate full (integer) julian date using fliegel and
   //    van flandern algorithm
   julday=k-32075+1461*(i+4800+(j-14)/12)/4+367*(j-2-(j-14)/12*12)/
      12-3*((i+4900+(j-14)/12)/100)/4;

   // subtract standard modification factor
   //julday -= GmatTimeConstants::JD_JAN_5_1941;
   julday -= (long int) jdMjdOffset;    // wcs

   // remove 12 hour gregorian to julian offset and calculate fractional
   //    part of day
   //fraction = SECS_TO_DAYS((starray[3] - 12.0) * GmatTimeConstants::SECS_PER_HOUR +
   //                        starray[4] * 60.0  + starray[5]);
   fraction = (((starray[3] - 12.0) * GmatTimeConstants::SECS_PER_HOUR +
                           starray[4] * 60.0  + starray[5]))/GmatTimeConstants::SECS_PER_DAY;

   // float modified julian day and add fractional day
   *utcmjd = (double)julday + fraction;

} // time_array_to_utcmjd()

//------------------------------------------------------------------------------
// void InitializeSlpFile()
//------------------------------------------------------------------------------
/**
 * This method initializes the data for the SLP file.
 *
 */
//------------------------------------------------------------------------------
void SlpFile::InitializeSlpFile()
{
   lengthOfHeaderRecord = SlpFile::LRECLHD;
   lengthOfDataRecord   = SlpFile::LRECLDT;
   ibepm                = SlpFile::IBEPM;
   jdMjdOffset          = (double) SlpFile::JD_MJD_OFFSET;
   int i;
   for (i=0;i<lengthOfHeaderRecord;i++)
      g_slp_header[i] = 0;
   for (i=0;i<lengthOfDataRecord;i++)
      g_slp_data[i] = 0;
}
