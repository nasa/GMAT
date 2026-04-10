//$Id$
//------------------------------------------------------------------------------
//                                  DeFile
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
// Created: 2004/03/01
//
/**
 * Implementation of the class that handles access to a DE file.
 *
 * @note The actual code that reads the DE file data, interpolates, etc.
 *       is from JPL/JSC (written by David Hoffman of JSC); I have modified
 *       it to not use macros, not print warning messages to the screen,
 *       use MAX_ARRAY_SIZE and arraySize, etc.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "Rvector6.hpp"
#include "SolarSystem.hpp"
#include "PlanetaryEphem.hpp"
#include "PlanetaryEphemException.hpp"
#include "TimeTypes.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define __UNIT_TEST__

#if defined (__UNIT_TEST__)
#include <fstream> //for debug output
#endif

//#define DEBUG_DEFILE
//#define DEBUG_DEFILE_READ
//#define DEBUG_DEFILE_INIT
//#define DEBUG_DEFILE_GET



///@todo: Option to use the acceleration calculated in DeFile currently disabled.
//        It will be turned on when it is approved after it is reviewed and tested.
//#define USE_CALCULATE_ACCELERATION


// DE file code from JPL/JSC (Hoffman) includes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include <iostream>


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Initialized in the header to fix swig link error:
//Error	849	error LNK2019: unresolved external symbol "public: static int const DeFile::VENUS_ID" (?VENUS_ID@DeFile@@2HB) referenced in function _CSharp_DeFile_VENUS_ID_get@0	C:\Projects\GmatAPI\gmat\build\windows-VS2013-CMake-32\swig\gmat_sharpCSHARP_wrap.obj

// const Integer DeFile::SUN_ID              = 10;
// const Integer DeFile::MERCURY_ID          = 0;
// const Integer DeFile::VENUS_ID            = 1;
// const Integer DeFile::EARTH_ID            = 2;     // Earth-Moon barycenter
// const Integer DeFile::MOON_ID             = 9;     // relative to geocenter
// const Integer DeFile::MARS_ID             = 3;
// const Integer DeFile::JUPITER_ID          = 4;
// const Integer DeFile::SATURN_ID           = 5;
// const Integer DeFile::URANUS_ID           = 6;
// const Integer DeFile::NEPTUNE_ID          = 7;
// const Integer DeFile::PLUTO_ID            = 8;

// const Integer DeFile::SS_BARY_ID          = 11;
// const Integer DeFile::EM_BARY_ID          = 12;
// const Integer DeFile::NUTATIONS_ID        = 13;
// const Integer DeFile::LIBRATIONS_ID       = 14;

const Real DeFile::JD_MJD_OFFSET  = GmatTimeConstants::JD_JAN_5_1941;
const Real DeFile::TT_OFFSET      = GmatTimeConstants::TT_TAI_OFFSET;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  DeFile(Gmat::DeFileType ofType, std::string fileName = "",
//         Gmat::DeFileFormat fmt = DE_BINARY)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the DeFile class
 * (default constructor).
 *
 * @param <ofType>   parameter indicating the type of De File.
 * @param <fileName> parameter indicating the full path name of the file.
 * @param <fmt>      parameter indicating the format of the file.
 *
 * @note if an ASCII file is input on creation, it will be converted to
 *       a file in native binary format; the binary file will be read, for
 *       efficiency.
 */
//------------------------------------------------------------------------------
DeFile::DeFile(Gmat::DeFileType ofType, std::string fileName,
               Gmat::DeFileFormat fmt) :
   PlanetaryEphem(fileName)
{
   defType       = ofType;
   theFileFormat = fmt;
   theFileName   = fileName;
   
   baseEpoch     = GmatTimeConstants::JD_JAN_5_1941;

   Initialize();
}

//------------------------------------------------------------------------------
//  DeFile(const DeFile& def)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the DeFile class using the values of the
 * input De File (copy constructor).
 *
 * @param <def> De file whose values to use to create this DE File.
 */
//------------------------------------------------------------------------------
DeFile::DeFile(const DeFile& def) :
   PlanetaryEphem(def),
   theFileName    (def.theFileName),
   theFileFormat  (def.theFileFormat),
   asciiFileName  (def.asciiFileName),
   binaryFileName (def.binaryFileName),
   defType        (def.defType),
   arraySize      (def.arraySize),
   Ephemeris_File (def.Ephemeris_File),
   T_beg          (def.T_beg),
   T_end          (def.T_end),
   T_span         (def.T_span),
   baseEpoch      (def.baseEpoch),
   mFileBeg       (def.mFileBeg),
   mA1FileBeg     (def.mA1FileBeg),
   EPHEMERIS      (def.EPHEMERIS)
{
   /// data from JPL/JSC code (Hoffman) ephem_read.c
   H1.data        = (def.H1).data;
   strcpy(H1.pad, (def.H1).pad);
   H2.data        = (def.H2).data;
   strcpy(H2.pad, (def.H2).pad);
   R1             = def.R1;

   int i;
   for (i=0;i<MAX_ARRAY_SIZE;i++)  Coeff_Array[i] = def.Coeff_Array[i];
}

//------------------------------------------------------------------------------
//  DeFile& operator= (const DeFile& def)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SlpFile class.
 *
 * @param <def> the DeFile object whose data to assign to "this"
 *            DE File.
 *
 * @return "this" DeFile with data of input DeFile def.
 */
//------------------------------------------------------------------------------
DeFile& DeFile::operator=(const DeFile& def)
{
   if (this == &def) return *this;
   PlanetaryEphem::operator=(def);

   theFileName    = def.theFileName;
   theFileFormat  = def.theFileFormat;

   asciiFileName  = def.asciiFileName;
   binaryFileName = def.binaryFileName;
   defType        = def.defType;
   arraySize      = def.arraySize;

   /// data from JPL/JSC code (Hoffman) ephem_read.c
   H1.data        = (def.H1).data;
   strcpy(H1.pad, (def.H1).pad);
   H2.data        = (def.H2).data;
   strcpy(H2.pad, (def.H2).pad);
   R1             = def.R1;
   Ephemeris_File = def.Ephemeris_File;
   int i;
   for (i=0;i<MAX_ARRAY_SIZE;i++)  Coeff_Array[i] = def.Coeff_Array[i];
   T_beg          = def.T_beg;
   T_end          = def.T_end;
   T_span         = def.T_span;
   baseEpoch      = def.baseEpoch;
   mFileBeg       = def.mFileBeg;
   mA1FileBeg     = def.mA1FileBeg;

   EPHEMERIS      = def.EPHEMERIS;
   return *this;
}

//------------------------------------------------------------------------------
//  ~DeFile()
//------------------------------------------------------------------------------
/**
 * Destructor for the DeFile class.
 */
//------------------------------------------------------------------------------
DeFile::~DeFile()
{
   // close the file, if it's open
   if (Ephemeris_File != NULL)
   {
      fclose(Ephemeris_File);

      // Prevent double closing of the file in the base class destructor
      if (Ephemeris_File == g_pef_dcb.fptr)
         g_pef_dcb.fptr = NULL;
   }
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void DeFile::Initialize()
{
   #ifdef DEBUG_DEFILE_INIT
   MessageInterface::ShowMessage("DeFile::Initialize() entered\n");
   #endif
  
   try
   {
      InitializeDeFile(theFileName, theFileFormat);
   }
   catch (PlanetaryEphemException &pe)
   {
      throw PlanetaryEphemException(pe.GetFullMessage());
   }

   #ifdef DEBUG_DEFILE_INIT
   MessageInterface::ShowMessage("DeFile::Initialize() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
//  Integer GetBodyID(std::string bodyName)
//------------------------------------------------------------------------------
/**
 * This method returns the ID for the requested body.
 *
 * @param <bodyName> name of body whose ID is requested.
 *
 * @return the ID for the requested body. Returns -1 if invalid body name.
 *
 */
//------------------------------------------------------------------------------
Integer  DeFile::GetBodyID(std::string bodyName)
{
   if (bodyName == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)     return DeFile::SS_BARY_ID;

   if (bodyName == GmatSolarSystemDefaults::SUN_NAME)     return DeFile::SUN_ID;
   if (bodyName == GmatSolarSystemDefaults::MERCURY_NAME) return DeFile::MERCURY_ID;
   if (bodyName == GmatSolarSystemDefaults::VENUS_NAME)   return DeFile::VENUS_ID;
   if (bodyName == GmatSolarSystemDefaults::EARTH_NAME)   return DeFile::EARTH_ID;
   if (bodyName == GmatSolarSystemDefaults::MOON_NAME)    return DeFile::MOON_ID;
   if (bodyName == GmatSolarSystemDefaults::MARS_NAME)    return DeFile::MARS_ID;
   if (bodyName == GmatSolarSystemDefaults::JUPITER_NAME) return DeFile::JUPITER_ID;
   if (bodyName == GmatSolarSystemDefaults::SATURN_NAME)  return DeFile::SATURN_ID;
   if (bodyName == GmatSolarSystemDefaults::URANUS_NAME)  return DeFile::URANUS_ID;
   if (bodyName == GmatSolarSystemDefaults::NEPTUNE_NAME) return DeFile::NEPTUNE_ID;
   if (bodyName == GmatSolarSystemDefaults::PLUTO_NAME)   return DeFile::PLUTO_ID;

   return -1;
}

Gmat::DeFileType DeFile::GetDeFileType() const
{
   return defType;
}


//------------------------------------------------------------------------------
//  Real* GetPosVel(Integer forBody, A1Mjd atTime, bool overrideTimeSystem)
//------------------------------------------------------------------------------
/**
 * This method returns the position and velocity of the specified body at the
 * requested time.
 *
 * @param <forbody>            the requested body (ID number on DE file).
 * @param <atTime>             time for which state of the body is requested.
 * @param <overrideTimeSystem> override the TDB time and use TT?
 *
 * @return state of the body at the requested time.
 *
 * @exception <PlanetaryEphemException> thrown if the position or velocity cannot
 *            be obtained due to problems opening or reading the DE File;
 *            message is based on error
 */
//------------------------------------------------------------------------------
Real* DeFile::GetPosVel(Integer forBody, A1Mjd atTime, bool overrideTimeSystem)
{
   #ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVel() entered, forBody=%d, atTime=%f, overrideTimeSystem=%d, reading file %s\n",
       forBody, atTime.GetReal(), overrideTimeSystem, itsName.c_str());
   #endif
   
   if (atTime.Get() < mA1FileBeg)
   {
	   std::stringstream ss;
	   ss << "Attempting to read data for an epoch (" << atTime.Get()
		   << ") earlier than the beginning of the current DE File (" << mA1FileBeg
		   << "); exiting.\n";
	   throw PlanetaryEphemException(ss.str());
//      throw PlanetaryEphemException("Attempting to read data for an epoch "
//            "earlier than the beginning of the current DE File; exiting.\n");
   }

   //static Real      result[6];
   // Adding 3 elements for acceleration
   static Real      result[9];

   // if we're asking for the Earth state, return 0.0 (since we're
   // currently assuming Earth-Centered Equatorial
   if (forBody == DeFile::EARTH_ID) // this should check for the J2000Body <<<<
   {
      result[0] = 0.0;
      result[1] = 0.0;
      result[2] = 0.0;
      result[3] = 0.0;
      result[4] = 0.0;
      result[5] = 0.0;

      // acceleration
      result[6] = 0.0;
      result[7] = 0.0;
      result[8] = 0.0;

      return result;
   }
   
   stateType rv;
   double absJD = 0.0;
   if (overrideTimeSystem)
   {
      double mjdTT = (double) theTimeConverter->Convert(atTime.Get(),
                      TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                      GmatTimeConstants::JD_JAN_5_1941);

      absJD = mjdTT;

      #ifdef DEBUG_DEFILE_GET
         MessageInterface::ShowMessage
            ("DeFile::GetPosVel() : mjdTT = %12.10f\n", mjdTT);
         MessageInterface::ShowMessage
            ("DeFile::GetPosVel() : absJD = %12.10f\n", absJD);
      #endif
   }
   else
   {
      double mjdTDB = (double) theTimeConverter->Convert(atTime.Get(),
                      TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
                      GmatTimeConstants::JD_JAN_5_1941);

      absJD = mjdTDB;

      #ifdef DEBUG_DEFILE_GET
         MessageInterface::ShowMessage
            ("DeFile::GetPosVel() : mjdTDB = %12.10f\n", mjdTDB);
         MessageInterface::ShowMessage
            ("DeFile::GetPosVel() : absJD = %12.10f\n", absJD);
      #endif
   }
   
   
   // if we're asking for the moon state, just get it and return it, as
   // it is supposed to be a geocentric state from the DE file  << should check for Moon w.r.t J2000Body here?
   
   // interpolate the data to get the state
   Interpolate_State(absJD, forBody, &rv);
   #ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel()  state from DE file = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
               rv.Position[0], rv.Position[1], rv.Position[2], rv.Velocity[0], rv.Velocity[1], rv.Velocity[2]);
   #endif

   if (forBody == DeFile::MOON_ID)
   {
      result[0] = (Real) rv.Position[0];
      result[1] = (Real) rv.Position[1];
      result[2] = (Real) rv.Position[2];
      result[3] = (Real) rv.Velocity[0] ;
      result[4] = (Real) rv.Velocity[1] ;
      result[5] = (Real) rv.Velocity[2] ;

      // acceleration
#ifdef USE_CALCULATE_ACCELERATION
      result[6] = (Real) rv.Acceleration[0];
      result[7] = (Real) rv.Acceleration[1];
      result[8] = (Real) rv.Acceleration[2];
#else
      result[6] = 0.0;
      result[7] = 0.0;
      result[8] = 0.0;
#endif

      return result;
   }

   // otherwise, we must take the body's state (in Solar System Barycentric
   // coordinates), then get the Earth-Moon state in SSBarycentric,
   // then get the Earth state from that (using the Moon state in
   // geocentric), then figure out the body's state wrt the Earth  << should be checking wrt the J2000Body here??
   stateType emrv, mrv;
   // earth-moon barycenter relative to solar system barycenter
   Interpolate_State(absJD,(int)DeFile::EARTH_ID, &emrv);
   // moon state (geocentric)
   Interpolate_State(absJD,(int)DeFile::MOON_ID, &mrv);
   #ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Earth-Moon barycenter state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
               emrv.Position[0], emrv.Position[1], emrv.Position[2], emrv.Velocity[0], emrv.Velocity[1], emrv.Velocity[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Moon (geocentric) state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
               mrv.Position[0], mrv.Position[1], mrv.Position[2], mrv.Velocity[0], mrv.Velocity[1], mrv.Velocity[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() R1.EMRAT = %12.10f\n", R1.EMRAT);
   #endif

   result[0] = rv.Position[0] -(emrv.Position[0] - (mrv.Position[0] / (R1.EMRAT + 1.0)));
   result[1] = rv.Position[1] -(emrv.Position[1] - (mrv.Position[1] / (R1.EMRAT + 1.0)));
   result[2] = rv.Position[2] -(emrv.Position[2] - (mrv.Position[2] / (R1.EMRAT + 1.0)));
   result[3] = rv.Velocity[0] -(emrv.Velocity[0] - (mrv.Velocity[0] / (R1.EMRAT + 1.0)));
   result[4] = rv.Velocity[1] -(emrv.Velocity[1] - (mrv.Velocity[1] / (R1.EMRAT + 1.0)));
   result[5] = rv.Velocity[2] -(emrv.Velocity[2] - (mrv.Velocity[2] / (R1.EMRAT + 1.0)));

   // acceleartion
#ifdef USE_CALCULATE_ACCELERATION
   result[6] = rv.Acceleration[0] - (emrv.Acceleration[0] - (mrv.Acceleration[0] / (R1.EMRAT + 1.0)));
   result[7] = rv.Acceleration[1] - (emrv.Acceleration[1] - (mrv.Acceleration[1] / (R1.EMRAT + 1.0)));
   result[8] = rv.Acceleration[2] - (emrv.Acceleration[2] - (mrv.Acceleration[2] / (R1.EMRAT + 1.0)));
#else
   result[6] = 0.0;
   result[7] = 0.0;
   result[8] = 0.0;
#endif

   #ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVel() returning %f, %f, %f, %f, %f, %f\n",
       result[0], result[1], result[2], result[3], result[4], result[5]);
   #endif
   
   //if (forBody == 11)
   //{
   //   MessageInterface::ShowMessage
   //      ("DeFile::GetPosVel(forBody = %d)  state from DE file = %.12f  %.12f  %.12f  %.12f  %.12f  %.12f\n", forBody,
   //      result[0], result[1], result[2], result[3], result[4], result[5]);
   //}

   return result;
}


Real* DeFile::GetPosVel(Integer forBody, GmatTime atTime, bool overrideTimeSystem)
{
#ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVel() entered, forBody=%d, atTime=%.12lf, overrideTimeSystem=%d, reading file %s\n",
      forBody, atTime.GetMjd(), overrideTimeSystem, itsName.c_str());
#endif

   if (atTime < mA1FileBeg)
   {
      std::stringstream ss;
      ss << "Attempting to read data for an epoch (" << atTime.GetMjd()
         << ") earlier than the beginning of the current DE File (" << mA1FileBeg
         << "); exiting.\n";
      throw PlanetaryEphemException(ss.str());
      //      throw PlanetaryEphemException("Attempting to read data for an epoch "
      //            "earlier than the beginning of the current DE File; exiting.\n");
   }
   
   //static Real      result[6];
   // Adding 3 elements for acceleration
   static Real      result[9];

   // if we're asking for the Earth state, return 0.0 (since we're
   // currently assuming Earth-Centered Equatorial
   if (forBody == DeFile::EARTH_ID) // this should check for the J2000Body <<<<
   {
      result[0] = 0.0;
      result[1] = 0.0;
      result[2] = 0.0;
      result[3] = 0.0;
      result[4] = 0.0;
      result[5] = 0.0;

      // acceleration
      result[6] = 0.0;
      result[7] = 0.0;
      result[8] = 0.0;

      return result;
   }

   stateType rv;
   GmatTime absJD = 0.0;
   if (overrideTimeSystem)
   {
      GmatTime mjdTT = theTimeConverter->Convert(atTime,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
         GmatTimeConstants::JD_JAN_5_1941);

      absJD = mjdTT;

#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : mjdTT = %.12lf\n", mjdTT.GetMjd());
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : absJD = %.12lf\n", absJD.GetMjd());
#endif
   }
   else
   {
      GmatTime mjdTDB = theTimeConverter->Convert(atTime,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
         GmatTimeConstants::JD_JAN_5_1941);

      absJD = mjdTDB;

#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : mjdTDB = %.12f\n", mjdTDB.GetMjd());
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : absJD = %.12f\n", absJD.GetMjd());
#endif
   }
   //MessageInterface::ShowMessage("A1 = [%d  %d  %.12lf]  to  absJD = [%d  %d  %.12lf]\n", atTime.GetDays(), atTime.GetSec(), atTime.GetFracSec(), absJD.GetDays(), absJD.GetSec(), absJD.GetFracSec());

   // if we're asking for the moon state, just get it and return it, as
   // it is supposed to be a geocentric state from the DE file  << should check for Moon w.r.t J2000Body here?

   // interpolate the data to get the state
   Interpolate_State(absJD, forBody, &rv);
#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel()  state from DE file = %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
         rv.Position[0], rv.Position[1], rv.Position[2], rv.Velocity[0], rv.Velocity[1], rv.Velocity[2]);
#endif
   
   if (forBody == DeFile::MOON_ID)
   {
      result[0] = (Real)rv.Position[0];
      result[1] = (Real)rv.Position[1];
      result[2] = (Real)rv.Position[2];
      result[3] = (Real)rv.Velocity[0];
      result[4] = (Real)rv.Velocity[1];
      result[5] = (Real)rv.Velocity[2];

      // acceleration
#ifdef USE_CALCULATE_ACCELERATION
      result[6] = (Real)rv.Acceleration[0];
      result[7] = (Real)rv.Acceleration[1];
      result[8] = (Real)rv.Acceleration[2];
#else
      result[6] = 0.0;
      result[7] = 0.0;
      result[8] = 0.0;
#endif
      return result;
   }

   // otherwise, we must take the body's state (in Solar System Barycentric
   // coordinates), then get the Earth-Moon state in SSBarycentric,
   // then get the Earth state from that (using the Moon state in
   // geocentric), then figure out the body's state wrt the Earth  << should be checking wrt the J2000Body here??
   stateType emrv, mrv;
   // earth-moon barycenter relative to solar system barycenter
   Interpolate_State(absJD, (int)DeFile::EARTH_ID, &emrv);
   // moon state (geocentric)
   Interpolate_State(absJD, (int)DeFile::MOON_ID, &mrv);
#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Earth-Moon barycenter state = %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
         emrv.Position[0], emrv.Position[1], emrv.Position[2], emrv.Velocity[0], emrv.Velocity[1], emrv.Velocity[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Moon (geocentric) state = %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
         mrv.Position[0], mrv.Position[1], mrv.Position[2], mrv.Velocity[0], mrv.Velocity[1], mrv.Velocity[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() R1.EMRAT = %.15f\n", R1.EMRAT);
#endif
   
   result[0] = rv.Position[0] - (emrv.Position[0] - (mrv.Position[0] / (R1.EMRAT + 1.0)));
   result[1] = rv.Position[1] - (emrv.Position[1] - (mrv.Position[1] / (R1.EMRAT + 1.0)));
   result[2] = rv.Position[2] - (emrv.Position[2] - (mrv.Position[2] / (R1.EMRAT + 1.0)));
   result[3] = rv.Velocity[0] - (emrv.Velocity[0] - (mrv.Velocity[0] / (R1.EMRAT + 1.0)));
   result[4] = rv.Velocity[1] - (emrv.Velocity[1] - (mrv.Velocity[1] / (R1.EMRAT + 1.0)));
   result[5] = rv.Velocity[2] - (emrv.Velocity[2] - (mrv.Velocity[2] / (R1.EMRAT + 1.0)));

   // acceleartion
#ifdef USE_CALCULATE_ACCELERATION
   result[6] = rv.Acceleration[0] - (emrv.Acceleration[0] - (mrv.Acceleration[0] / (R1.EMRAT + 1.0)));
   result[7] = rv.Acceleration[1] - (emrv.Acceleration[1] - (mrv.Acceleration[1] / (R1.EMRAT + 1.0)));
   result[8] = rv.Acceleration[2] - (emrv.Acceleration[2] - (mrv.Acceleration[2] / (R1.EMRAT + 1.0)));
#else
   result[6] = 0.0;
   result[7] = 0.0;
   result[8] = 0.0;
#endif

#ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVel() returning %.12lf, %.12lf, %.12lf, %.12lf, %.12lf, %.12lf\n",
      result[0], result[1], result[2], result[3], result[4], result[5]);
#endif


   //if (forBody == 11)
   //{
   //   MessageInterface::ShowMessage
   //      ("DeFile::GetPosVel(forBody = %d)  state from DE file = %.12f  %.12f  %.12f  %.12f  %.12f  %.12f\n", forBody,
   //      result[0], result[1], result[2], result[3], result[4], result[5]);
   //}
   return result;
}


//------------------------------------------------------------------------------
//  Real* GetPosDelta(Integer forBody, const GmatTime &atTime1, const GmatTime &atTime2, bool overrideTimeSystem)
//------------------------------------------------------------------------------
/**
 * This method returns the change in the position of the specified
 * body between the requested times.
 *
 * @param <forbody>            the requested body (ID number on DE file).
 * @param <atTime1>            first time for which state of the body is requested.
 * @param <atTime2>            second time for which state of the body is requested.
 * @param <overrideTimeSystem> override the TDB time and use TT?
 *
 * @return state of the body at the requested time.
 *
 * @exception <PlanetaryEphemException> thrown if the position cannot
 *            be obtained due to problems opening or reading the DE File;
 *            message is based on error
 */
//------------------------------------------------------------------------------
Real* DeFile::GetPosDelta(Integer forBody,
                          const GmatTime &atTime1,
                          const GmatTime &atTime2,
                          bool overrideTimeSystem)
{
#ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVelDelta() entered, forBody=%d, atTime1=%.12lf, atTime2=%.12lf, overrideTimeSystem=%d, reading file %s\n",
      forBody, atTime1.GetMjd(), atTime2.GetMjd(), overrideTimeSystem, itsName.c_str());
#endif

   if (atTime1 < mA1FileBeg)
   {
      std::stringstream ss;
      ss << "Attempting to read data for an epoch (" << atTime1.GetMjd()
         << ") earlier than the beginning of the current DE File (" << mA1FileBeg
         << "); exiting.\n";
      throw PlanetaryEphemException(ss.str());
      //      throw PlanetaryEphemException("Attempting to read data for an epoch "
      //            "earlier than the beginning of the current DE File; exiting.\n");
   }
   static Real      result[3];
   // if we're asking for the Earth state, return 0.0 (since we're
   // currently assuming Earth-Centered Equatorial
   if (forBody == DeFile::EARTH_ID) // this should check for the J2000Body <<<<
   {
      result[0] = 0.0;
      result[1] = 0.0;
      result[2] = 0.0;
      return result;
   }

   stateType rDelta;
   GmatTime absJD1 = 0.0;
   GmatTime absJD2 = 0.0;
   if (overrideTimeSystem)
   {
      GmatTime mjdTT1 = theTimeConverter->Convert(atTime1,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
         GmatTimeConstants::JD_JAN_5_1941);
      GmatTime mjdTT2 = theTimeConverter->Convert(atTime2,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
         GmatTimeConstants::JD_JAN_5_1941);

      absJD1 = mjdTT1;
      absJD2 = mjdTT2;

#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : mjdTT1 = %.12lf\n", mjdTT1.GetMjd());
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : absJD1 = %.12lf\n", absJD1.GetMjd());
#endif
   }
   else
   {
      GmatTime mjdTDB1 = theTimeConverter->Convert(atTime1,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
         GmatTimeConstants::JD_JAN_5_1941);
      GmatTime mjdTDB2 = theTimeConverter->Convert(atTime2,
         TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
         GmatTimeConstants::JD_JAN_5_1941);

      absJD1 = mjdTDB1;
      absJD2 = mjdTDB2;

#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : mjdTDB = %.12f\n", mjdTDB1.GetMjd());
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() : absJD = %.12f\n", absJD1.GetMjd());
#endif
   }
   //MessageInterface::ShowMessage("A1 = [%d  %d  %.12lf]  to  absJD = [%d  %d  %.12lf]\n", atTime.GetDays(), atTime.GetSec(), atTime.GetFracSec(), absJD.GetDays(), absJD.GetSec(), absJD.GetFracSec());

   // if we're asking for the moon state, just get it and return it, as
   // it is supposed to be a geocentric state from the DE file  << should check for Moon w.r.t J2000Body here?

   // interpolate the data to get the state
   Interpolate_State_Delta(absJD1, absJD2, forBody, &rDelta);
#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel()  state delta from DE file = %.12lf  %.12lf  %.12lf\n",
         rDelta.Position[0], rDelta.Position[1], rDelta.Position[2]);
#endif
   
   if (forBody == DeFile::MOON_ID)
   {
      result[0] = (Real)rDelta.Position[0];
      result[1] = (Real)rDelta.Position[1];
      result[2] = (Real)rDelta.Position[2];
      return result;
   }

   // otherwise, we must take the body's state (in Solar System Barycentric
   // coordinates), then get the Earth-Moon state in SSBarycentric,
   // then get the Earth state from that (using the Moon state in
   // geocentric), then figure out the body's state wrt the Earth  << should be checking wrt the J2000Body here??
   stateType emrDelta, mrDelta;
   // earth-moon barycenter relative to solar system barycenter
   Interpolate_State_Delta(absJD1, absJD2, (int)DeFile::EARTH_ID, &emrDelta);
   // moon state (geocentric)
   Interpolate_State_Delta(absJD1, absJD2, (int)DeFile::MOON_ID, &mrDelta);
#ifdef DEBUG_DEFILE_GET
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Earth-Moon barycenter state delta = %.12lf  %.12lf  %.12lf\n",
         emrDelta.Position[0], emrDelta.Position[1], emrDelta.Position[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() Moon (geocentric) state delta = %.12lf  %.12lf  %.12lf\n",
         mrDelta.Position[0], mrDelta.Position[1], mrDelta.Position[2]);
      MessageInterface::ShowMessage
         ("DeFile::GetPosVel() R1.EMRAT = %.15f\n", R1.EMRAT);
#endif
   
   result[0] = rDelta.Position[0] - (emrDelta.Position[0] - (mrDelta.Position[0] / (R1.EMRAT + 1.0)));
   result[1] = rDelta.Position[1] - (emrDelta.Position[1] - (mrDelta.Position[1] / (R1.EMRAT + 1.0)));
   result[2] = rDelta.Position[2] - (emrDelta.Position[2] - (mrDelta.Position[2] / (R1.EMRAT + 1.0)));

#ifdef DEBUG_DEFILE_GET
   MessageInterface::ShowMessage
      ("DeFile::GetPosVel() returning %.12lf, %.12lf, %.12lf\n",
      result[0], result[1], result[2]);
#endif


   //if (forBody == 11)
   //{
   //   MessageInterface::ShowMessage
   //      ("DeFile::GetPosVel(forBody = %d)  state from DE file = %.12f  %.12f  %.12f\n", forBody,
   //      result[0], result[1], result[2]);
   //}
   return result;
}


//------------------------------------------------------------------------------
// void GetAnglesAndRates(A1Mjd atTime, Real* angles, Real* rates, 
//                        bool overrideTimeSystem)
//------------------------------------------------------------------------------
void  DeFile::GetAnglesAndRates(A1Mjd atTime, Real* angles, Real* rates, 
                                bool overrideTimeSystem)
{
   // assume Luna for now - check for name later
   double absJD = 0.0;
   if (overrideTimeSystem)
   {
       double mjdTT = (double) theTimeConverter->Convert(atTime.Get(),
                       TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                       GmatTimeConstants::JD_JAN_5_1941);
       absJD        = mjdTT;
   }
   else
   {
       double mjdTDB = (double) theTimeConverter->Convert(atTime.Get(),
                       TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
                       GmatTimeConstants::JD_JAN_5_1941);
       absJD         = mjdTDB;
   }

   #ifdef DEBUG_DEFILE_LIB
   MessageInterface::ShowMessage
      ("DeFile::GetAnglesAndRates() Calling Interpolate_Libration(%.9f)\n", absJD);
   #endif
   
   Interpolate_Libration(absJD, 12, angles, rates);
}


void  DeFile::GetAnglesAndRates(const GmatTime &atTime, Real* angles, Real* rates, 
                                bool overrideTimeSystem)
{
   // assume Luna for now - check for name later
   GmatTime absJD(0);
   if (overrideTimeSystem)
   {
       GmatTime mjdTT = theTimeConverter->Convert(atTime,
                        TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                        GmatTimeConstants::JD_JAN_5_1941);
       absJD          = mjdTT;
   }
   else
   {
       GmatTime mjdTDB = theTimeConverter->Convert(atTime,
                         TimeSystemConverter::A1MJD, TimeSystemConverter::TDBMJD,
                         GmatTimeConstants::JD_JAN_5_1941);
       absJD           = mjdTDB;
   }

   #ifdef DEBUG_DEFILE_LIB
   MessageInterface::ShowMessage
      ("DeFile::GetAnglesAndRates() Calling Interpolate_Libration(%.9f)\n", absJD);
   #endif
   
   Interpolate_Libration(absJD, 12, angles, rates);
}

//------------------------------------------------------------------------------
//  Integer* GetStartDayAndYear()
//------------------------------------------------------------------------------
/**
 * This method returns the start day-of-year and year of the DE file.
 *
 * @return start day-of-year and year of the DE file.
 */
//------------------------------------------------------------------------------
Integer* DeFile::GetStartDayAndYear()
{
   Integer y, doy, h, min;
   Real    sec;
   Integer* res  = new Integer[2];
   // first time on header is start time of file in absolute Julian Days
   Real mjd      = (Real) R1.timeData[0] - jdMjdOffset;
   A1Mjd *a      = new A1Mjd(mjd);
   UtcDate uTime = a->ToUtcDate();
   uTime.ToYearDOYHourMinSec(y,doy,h,min,sec);
   res[0]        = doy;
   res[1]        = y;
   
   delete a;
   return res;
}


//------------------------------------------------------------------------------
//  std::string Convert(std::string deFileNameAscii)
//------------------------------------------------------------------------------
/**
 * This method converts the ASCII DE file (input at creation) to a native
 * binary format.  It assumes that an appropriate header file, with the name
 * header.FMT, where FMT is the type of DE file (e.g. 421), exists in the
 * same directory as the ASCII DE file.
 *
 * @param <deFileNameAscii> ASCII file name (pull path name).
 *
 * @return the full path name of the binary file; to name it, this method will
 *        take the input ASCII file name and add 'Bin' at the end (before the
 *        .FMT).
 */
//------------------------------------------------------------------------------
std::string DeFile::Convert(std::string deFileNameAscii)
{
   // TBD
   return deFileNameAscii;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void InitializeDeFile(std::string name)
//------------------------------------------------------------------------------
/**
 * This method initializes the data for the DE file.
 *
 * @exception <PlanetaryEphemException> thrown if there is an error initializing
 *            the DE file.
 */
//------------------------------------------------------------------------------
void DeFile::InitializeDeFile(std::string fName, Gmat::DeFileFormat fileFmt)
{
   #ifdef DEBUG_DEFILE_INIT
   MessageInterface::ShowMessage
      ("DeFile::InitializeDeFile() entered\n   file=%s, fileFormat=%d\n",
       fName.c_str(), fileFmt);
   #endif
   
   if (fileFmt == Gmat::DE_ASCII)
   {
      asciiFileName  = fName;
      binaryFileName = Convert(asciiFileName);
   }
   else
   {
      asciiFileName  = "unknown";
      binaryFileName = fName;
   }

   if (defType == Gmat::DE_DE405)
   {
      arraySize = DeFile::ARRAY_SIZE_405;
      EPHEMERIS = 405;
   }
   else if (defType == Gmat::DE_DE421)
   {
      arraySize = DeFile::ARRAY_SIZE_421;
      EPHEMERIS = 421;
   }
   else if (defType == Gmat::DE_DE424)
   {
      arraySize = DeFile::ARRAY_SIZE_424;
      EPHEMERIS = 424;
   }
   else if (defType == Gmat::DE_DE430)
   {
      arraySize = DeFile::ARRAY_SIZE_430;
      EPHEMERIS = 430;
   }
   else
   {
      // ERROR!  Other formats not currently supported!!!
   }
   int worked = Initialize_Ephemeris((char*)binaryFileName.c_str());
   
   if (worked == FAILURE)
   {
      throw PlanetaryEphemException("DE file is not of specified format!!"
                                    "DE file not able to be initialized!");
   }
   itsName           = binaryFileName;
   strcpy(g_pef_dcb.full_path,binaryFileName.c_str());
   g_pef_dcb.recl    = arraySize;
   if (Ephemeris_File)
      g_pef_dcb.fptr = Ephemeris_File;
   jdMjdOffset       = (double) DeFile::JD_MJD_OFFSET;
   
   // store file begin time
   mFileBeg          = T_beg - baseEpoch;
   mA1FileBeg        = T_beg;

   #ifdef DEBUG_DEFILE_INIT
   MessageInterface::ShowMessage("   T_beg=%.9f, addr=%p\n", T_beg, &T_beg);
   MessageInterface::ShowMessage("   T_end=%.9f, addr=%p\n", T_end, &T_end);
   MessageInterface::ShowMessage("DeFile::InitializeDeFile() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// private methods from JPL/JSC code ephem_read.c
//------------------------------------------------------------------------------

/******************************************************************************/
/**                                                                          **/
/**  SOURCE FILE: ephem_read.c                                               **/
/**                                                                          **/
/**     This file contains a set of functions and global variables that      **/
/**     implements an ephemeris server program. Client programs can use      **/
/**     use this server to access ephemeris data by calling one of the       **/
/**     following functions:                                                 **/
/**                                                                          **/
/**        Interpolate_Libration -- returns lunar libration angles           **/
/**        Interpolate_Nutation  -- returns (terrestrial) nutation angles    **/
/**        Interpolate_Position  -- returns position of planet               **/
/**        Interpolate_State     -- returns position and velocity of planet  **/
/**                                                                          **/
/**     Note that client programs must make one, and only one, call to the   **/
/**     function Initialize_Ephemeris before any of these other functions    **/
/**     be used. After this call any of the above functions can be called    **/
/**     in any sequence.                                                     **/
/**                                                                          **/
/**  Programmer: David Hoffman/EG5                                           **/
/**              NASA, Johnson Space Center                                  **/
/**              Houston, TX 77058                                           **/
/**              e-mail: david.a.hoffman1@jsc.nasa.gov                       **/
/**                                                                          **/
/**  History:                                                                **/
/**                                                                          **/
/**    8/11/99  Changed made to function Read_Coefficients to ensure that    **/
/**             Offset has a negative value when requested time comes        **/
/**             before the beginning of the current record. Thanks to Gary   **/
/**             Bernstien (garyb@astro.lsa.umich.edu) for pointing out this  **/
/**             serious bug.                                                 **/
/**                                                                          **/
/******************************************************************************/

/**==========================================================================**/
/**  Read_Coefficients                                                       **/
/**                                                                          **/
/**     This function is used by the functions below to read an array of     **/
/**     Tchebeychev coefficients from a binary ephemeris data file.          **/
/**                                                                          **/
/**  Input: Desired record time.                                             **/
/**                                                                          **/
/**  Output: None.                                                           **/
/**                                                                          **/
/**==========================================================================**/
void DeFile::Read_Coefficients( double Time )
{
   #ifdef DEBUG_DEFILE_READ
      MessageInterface::ShowMessage("DeFile::Read_Coefficients() Time=%.9f)\n", Time);
      MessageInterface::ShowMessage(" DE filename = '%s'\n",theFileName.c_str());
   #endif
   
   double  T_delta = 0.0;
   int     Offset  =  0 ;

   #ifdef DEBUG_DEFILE_READ
      MessageInterface::ShowMessage
         ("DeFile::Read_Coefficients() T_beg=%f, T_end=%f\n", T_beg, T_end);
   #endif

   /*--------------------------------------------------------------------------*/
   /*  Find ephemeris data that record contains input time. Note that one, and */
   /*  only one, of the following conditional statements will be true (if both */
   /*  were false, this function would not have been called).                  */
   /*--------------------------------------------------------------------------*/

   if ( Time < T_beg )                    /* Compute backwards location offset */
   {
      T_delta = T_beg - Time;
      Offset  = (int) - ceil(T_delta/T_span);
   }

   if ( Time > T_end )                    /* Compute forwards location offset */
   {
      T_delta = Time - T_end;
      Offset  = (int) ceil(T_delta/T_span);
   }

   /*--------------------------------------------------------------------------*/
   /*  Retrieve ephemeris data from new record.                                */
   /*--------------------------------------------------------------------------*/

   #ifdef DEBUG_DEFILE_READ
      MessageInterface::ShowMessage
         ("DeFile::Read_Coefficients() Offset=%d\n", Offset);
   #endif

   // if time is less than file begin time, do not update time info.
   if (Time > mFileBeg) //loj: 9/15/05 Added
   {
      fseek(Ephemeris_File,(Offset-1)*arraySize*sizeof(double),SEEK_CUR);

      // Intentionally get the return and then ignore it to move warning from
      // system libraries to GMAT code base.  The "unused variable" warning here
      // can be safely ignored.
      size_t len = fread(&Coeff_Array,sizeof(double),arraySize,Ephemeris_File);
      if ((Integer)len != arraySize)
      {
         // Write detaild message (LOJ: 2015.10.03)
         //throw PlanetaryEphemException("Requested epoch is not on the DE file");
         PlanetaryEphemException ex;
         ex.SetDetails("Requested epoch %.9f is not on the DE file '%s'.\n", Time,
                       theFileName.c_str());
         throw ex;
      }
      
      T_beg  = Coeff_Array[0] - baseEpoch;
      T_end  = Coeff_Array[1] - baseEpoch;
      T_span = T_end - T_beg;
   }
}


void DeFile::Read_Coefficients(GmatTime Time)
{
#ifdef DEBUG_DEFILE_READ
   MessageInterface::ShowMessage("DeFile::Read_Coefficients() Time=%.9f)\n", Time.GetMjd());
   MessageInterface::ShowMessage(" DE filename = '%s'\n", theFileName.c_str());
#endif

   double  T_delta = 0.0;
   int     Offset = 0;

#ifdef DEBUG_DEFILE_READ
   MessageInterface::ShowMessage
      ("DeFile::Read_Coefficients() T_beg=%f, T_end=%f\n", T_beg, T_end);
#endif

   /*--------------------------------------------------------------------------*/
   /*  Find ephemeris data that record contains input time. Note that one, and */
   /*  only one, of the following conditional statements will be true (if both */
   /*  were false, this function would not have been called).                  */
   /*--------------------------------------------------------------------------*/

   if (Time < T_beg)                    /* Compute backwards location offset */
   {
      T_delta = (GmatTime(T_beg) - Time).GetTimeInSec();
      Offset = (int)-ceil(T_delta / (T_span*GmatTimeConstants::SECS_PER_DAY));
   }

   if (Time > T_end)                    /* Compute forwards location offset */
   {
      T_delta = (Time - GmatTime(T_end)).GetTimeInSec();
      Offset = (int)ceil(T_delta / (T_span*GmatTimeConstants::SECS_PER_DAY));
   }

   /*--------------------------------------------------------------------------*/
   /*  Retrieve ephemeris data from new record.                                */
   /*--------------------------------------------------------------------------*/

#ifdef DEBUG_DEFILE_READ
   MessageInterface::ShowMessage
      ("DeFile::Read_Coefficients() Offset=%d\n", Offset);
#endif

   // if time is less than file begin time, do not update time info.
   if (Time > mFileBeg) //loj: 9/15/05 Added
   {
      fseek(Ephemeris_File, (Offset - 1)*arraySize*sizeof(double), SEEK_CUR);

      // Intentionally get the return and then ignore it to move warning from
      // system libraries to GMAT code base.  The "unused variable" warning here
      // can be safely ignored.
      size_t len = fread(&Coeff_Array, sizeof(double), arraySize, Ephemeris_File);
      if ((Integer)len != arraySize)
      {
         // Write detaild message (LOJ: 2015.10.03)
         //throw PlanetaryEphemException("Requested epoch is not on the DE file");
         PlanetaryEphemException ex;
         ex.SetDetails("Requested epoch %.9f is not on the DE file '%s'.\n", Time.GetMjd(),
            theFileName.c_str());
         throw ex;
      }

      T_beg = Coeff_Array[0] - baseEpoch;
      T_end = Coeff_Array[1] - baseEpoch;
      T_span = T_end - T_beg;
   }
}


/**==========================================================================**/
/**  Initialize_Ephemeris                                                    **/
/**                                                                          **/
/**     This function must be called once by any program that accesses the   **/
/**     ephemeris data. It opens the ephemeris data file, reads the header   **/
/**     data, loads the first coefficient record into a global array, then   **/
/**     returns a status code that indicates whether or not all of this was  **/
/**     done successfully.                                                   **/
/**                                                                          **/
/**  Input: A character string giving the name of an ephemeris data file.    **/
/**                                                                          **/
/**  Returns: An integer status code.                                        **/
/**                                                                          **/
/**==========================================================================**/
int DeFile::Initialize_Ephemeris( char *fileName )
{
   #ifdef DEBUG_DEFILE_INIT
   MessageInterface::ShowMessage("DeFile::Initialize_Ephemeris() entered\n");
   MessageInterface::ShowMessage
      ("   fileName=%s, arraySize=%d\n", fileName, arraySize);
   #endif
   
   int headerID;
  
   /*--------------------------------------------------------------------------*/
   /*  Open ephemeris file.                                                    */
   /*--------------------------------------------------------------------------*/

   Ephemeris_File = fopen(fileName,"rb");

   /*--------------------------------------------------------------------------*/
   /*  Read header & first coefficient array, then return status code.         */
   /*--------------------------------------------------------------------------*/
   if ( Ephemeris_File == NULL ) /*........................No need to continue */
   {
      return FAILURE;
   }
   else  /*.................Read first three header records from ephemeris file */
   {
      size_t len = fread(&H1,sizeof(double),arraySize,Ephemeris_File);
      len       += fread(&H2,sizeof(double),arraySize,Ephemeris_File);
      len       += fread(&Coeff_Array,sizeof(double),arraySize,Ephemeris_File);
            
      /*...............................Store header data in global variables */
      R1 = H1.data;
              
      /*..........................................Set current time variables */
      T_beg  = Coeff_Array[0] - baseEpoch;
      T_end  = Coeff_Array[1] - baseEpoch;
      T_span = T_end - T_beg;

      /*..............................Convert header ephemeris ID to integer */
      headerID = (int) R1.DENUM;

      #if defined (__UNIT_TEST__)
         std::ofstream fout;
         fout.open("TestDeFile.txt");

         fout << " In: Initialize_Ephemeris" << std::endl;
         fout << "     ARRAY_SIZE     " << arraySize << std::endl;
         fout << "     R1.timeData[0] " << R1.timeData[0] << std::endl;
         fout << "     R1.timeData[1] " << R1.timeData[1] << std::endl;
         fout << "     R1.timeData[2] " << R1.timeData[2] << std::endl;
         fout << "     R1.numConst    " << R1.numConst << std::endl;
         fout << "     R1.AU          " << R1.AU << std::endl;
         fout << "     R1.EMRAT       " <<  R1.EMRAT << std::endl;
         fout << "     R1.coeffPtr[0][0] " << R1.coeffPtr[0][0] << std::endl;
         fout << "     R1.coeffPtr[11][2]" << R1.coeffPtr[11][2] << std::endl;

         fout << "     R1.DENUM   " << R1.DENUM << std::endl;
         fout << "     headerID   " << headerID << std::endl;
         fout << "     EPHEMERIS  " << EPHEMERIS << std::endl;
         fout << "     T_Beg      " << T_beg << std::endl;
         fout << "     T_End      " << T_end << std::endl;
         fout << "     T_Span     " << T_span << std::endl;

         fout.close();
      #endif
  
      /*..................................................Return status code */
      if ( headerID == EPHEMERIS )
      {
         return SUCCESS;
      }
      else
      {
         return FAILURE;
      }
   }
}

/**==========================================================================**/
/**  Interpolate_Libration                                                   **/
/**                                                                          **/
/**     This function computes an array of libration angles from Chebyshev   **/
/**     coefficients read in from an ephemeris data file. The coefficients   **/
/**     are read in from a file by the function Read_Coefficients (when      **/
/**     necessary).                                                          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Nutation -- Pointer to external array to receive the answer.         **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**                                                                          **/
/**  2006.05.19  W. Shoan/GSFC/MAB  added code to compute and return rates   **/
/**              (cut-and-paste from Interpolate_State & modify )            **/
/**==========================================================================**/
void DeFile::Interpolate_Libration( double Time , int Target , 
                                    double Libration[3], double rates[3] )
{
   double    A[50] , Cp[50]  , Up[50], sum[3] , rateSum[3];
   double    T_break , T_seg = 0.0 , T_sub = 0.0 , Tc = 0.0;
   int       i , j;
   long int  C , G , N , offset = 0;

   #ifdef DEBUG_DEFILE_LIB
      MessageInterface::ShowMessage
         ("DeFile::Interpolate_Libration(%.9f, %d)\n", Time, Target);
      MessageInterface::ShowMessage("   addr T_beg=%p\n", &T_beg);
      MessageInterface::ShowMessage("   addr T_end=%p\n", &T_end);
   #endif
  
   /*--------------------------------------------------------------------------*/
   /* This function only computes librations.                                  */
   /*--------------------------------------------------------------------------*/
   if ( Target != 12 )             /* Also protects against weird input errors */
   {
      // printf("\n This function only computes librations.\n");
      return;
   }
 
   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<50 ; i++ )
   {
      A[i] = 0.0;
   }
  
   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input (if so, get it).             */
   /*--------------------------------------------------------------------------*/
   if ( Time < T_beg || Time > T_end )
   {
      #ifdef DEBUG_DEFILE_LIB
         MessageInterface::ShowMessage
            ("DeFile::Interpolate_Libration() Calling Read_Coefficients(%.9f)\n",
             Time);
      #endif
     
      Read_Coefficients(Time);
   }
  
   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/
   C = R1.libratPtr[0] - 1;                   /* Coefficient array entry point */
   N = R1.libratPtr[1];                       /*        Number of coefficients */
   G = R1.libratPtr[2];                       /*    Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the libration angles is used   */
   /*  to load the coefficients.                                               */
   /*--------------------------------------------------------------------------*/
   if ( G == 1 )
   {
      Tc = 2.0*(Time - T_beg) / T_span - 1.0;
      for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
   }
   else if ( G > 1 )
   {
      T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
      for ( j=G ; j>0 ; j-- )
      {
         T_break = T_beg + ((double) j-1) * T_sub;
         if ( Time > T_break )
         {
            T_seg  = T_break;
            offset = j-1;
            break;
            }
         }
            
      Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
      C  = C + 3 * offset * N;
       
      for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      //printf("\n Number of granules must be >= 1: check header data.\n");
   }

   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the libration angles and rates                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<3 ; i++ )
   {
      Cp[0] = 1.0;
      Cp[1] = Tc;
      Cp[2] = 2.0 * Tc*Tc - 1.0;
        
      Up[0] = 0.0;
      Up[1] = 1.0;
      Up[2] = 4.0 * Tc;

      for ( j=3 ; j<N ; j++ )
      {
         Cp[j] = 2.0 * Tc * Cp[j-1] - Cp[j-2];
         Up[j] = 2.0 * Tc * Up[j-1] + 2.0 * Cp[j-1] - Up[j-2];
      }

      sum[i]     = 0.0;
      rateSum[i] = 0.0;

      for ( j=N-1 ; j>-1 ; j-- )  sum[i]     = sum[i] + A[j+i*N] * Cp[j];
      for ( j=N-1 ; j>0  ; j-- )  rateSum[i] = rateSum[i] + A[j+i*N] * Up[j];
      Libration[i] = sum[i];
      rates[i]     = rateSum[i] * 2.0 * ((double) G) / (T_span * GmatTimeConstants::SECS_PER_DAY);
   }
   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the rates.                                          */
   /*--------------------------------------------------------------------------*/
      
   return;
}


void DeFile::Interpolate_Libration( const GmatTime &Time , int Target , 
                                    double Libration[3], double rates[3] )
{
   double    A[50] , Cp[50]  , Up[50], sum[3] , rateSum[3];
   double    T_break , T_seg = 0.0 , T_sub = 0.0 , Tc = 0.0;
   int       i , j;
   long int  C , G , N , offset = 0;

   #ifdef DEBUG_DEFILE_LIB
      MessageInterface::ShowMessage
         ("DeFile::Interpolate_Libration(%.9f, %d)\n", Time, Target);
      MessageInterface::ShowMessage("   addr T_beg=%p\n", &T_beg);
      MessageInterface::ShowMessage("   addr T_end=%p\n", &T_end);
   #endif
  
   /*--------------------------------------------------------------------------*/
   /* This function only computes librations.                                  */
   /*--------------------------------------------------------------------------*/
   if ( Target != 12 )             /* Also protects against weird input errors */
   {
      // printf("\n This function only computes librations.\n");
      return;
   }
 
   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<50 ; i++ )
   {
      A[i] = 0.0;
   }
  
   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input (if so, get it).             */
   /*--------------------------------------------------------------------------*/
   if ( Time < T_beg || Time > T_end )
   {
      #ifdef DEBUG_DEFILE_LIB
         MessageInterface::ShowMessage
            ("DeFile::Interpolate_Libration() Calling Read_Coefficients(%.9f)\n",
             Time);
      #endif
     
      Read_Coefficients(Time);
   }
  
   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/
   C = R1.libratPtr[0] - 1;                   /* Coefficient array entry point */
   N = R1.libratPtr[1];                       /*        Number of coefficients */
   G = R1.libratPtr[2];                       /*    Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the libration angles is used   */
   /*  to load the coefficients.                                               */
   /*--------------------------------------------------------------------------*/
   if ( G == 1 )
   {
      Tc = 2.0*(Time - T_beg).GetMjd() / T_span - 1.0;
      for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
   }
   else if ( G > 1 )
   {
      T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
      for ( j=G ; j>0 ; j-- )
      {
         T_break = T_beg + ((double) j-1) * T_sub;
         if ( Time > T_break )
         {
            T_seg  = T_break;
            offset = j-1;
            break;
            }
         }
            
      Tc = 2.0*(Time - T_seg).GetMjd() / T_sub - 1.0;
      C  = C + 3 * offset * N;
       
      for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      //printf("\n Number of granules must be >= 1: check header data.\n");
   }

   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the libration angles and rates                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<3 ; i++ )
   {
      Cp[0] = 1.0;
      Cp[1] = Tc;
      Cp[2] = 2.0 * Tc*Tc - 1.0;
        
      Up[0] = 0.0;
      Up[1] = 1.0;
      Up[2] = 4.0 * Tc;

      for ( j=3 ; j<N ; j++ )
      {
         Cp[j] = 2.0 * Tc * Cp[j-1] - Cp[j-2];
         Up[j] = 2.0 * Tc * Up[j-1] + 2.0 * Cp[j-1] - Up[j-2];
      }

      sum[i]     = 0.0;
      rateSum[i] = 0.0;

      for ( j=N-1 ; j>-1 ; j-- )  sum[i]     = sum[i] + A[j+i*N] * Cp[j];
      for ( j=N-1 ; j>0  ; j-- )  rateSum[i] = rateSum[i] + A[j+i*N] * Up[j];
      Libration[i] = sum[i];
      rates[i]     = rateSum[i] * 2.0 * ((double) G) / (T_span * GmatTimeConstants::SECS_PER_DAY);
   }
   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the rates.                                          */
   /*--------------------------------------------------------------------------*/
      
   return;
}


/**==========================================================================**/
/**  Interpolate_Nutation                                                    **/
/**                                                                          **/
/**     This function computes an array of nutation angles from Chebyshev    **/
/**     coefficients read in from an ephemeris data file. The coefficients   **/
/**     are read in from a file by the function Read_Coefficients (when      **/
/**     necessary).                                                          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Nutation -- Pointer to external array to receive the angles.         **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**                                                                          **/
/**==========================================================================**/
void DeFile::Interpolate_Nutation( double Time , int Target , double Nutation[2] )
{
   double    A[50] , Cp[50]  , sum[3] , T_break , T_seg = 0.0 , T_sub , Tc = 0.0;
   int       i , j;
   long int  C , G , N , offset = 0;

   /*--------------------------------------------------------------------------*/
   /* This function only computes nutations.                                   */
   /*--------------------------------------------------------------------------*/
   if ( Target != SS_BARY_ID )             /* Also protects against weird input errors */
   {
      // printf("\n This function only computes nutations.\n");
      return;
   }
 
   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<50 ; i++ )
   {
      A[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input (if so, get it).             */
   /*--------------------------------------------------------------------------*/
   if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/
   C = R1.coeffPtr[Target][0] - 1;            /* Coefficient array entry point */
   N = R1.coeffPtr[Target][1];                /*        Number of coefficients */
   G = R1.coeffPtr[Target][2];                /*    Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the nutation angles is used    */
   /*  to load the coefficients.                                               */
   /*--------------------------------------------------------------------------*/
   if ( G == 1 )
   {
      Tc = 2.0*(Time - T_beg) / T_span - 1.0;
      for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
   }
   else if ( G > 1 )
   {
      T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
      for ( j=G ; j>0 ; j-- )
      {
         T_break = T_beg + ((double) j-1) * T_sub;
         if ( Time > T_break )
         {
            T_seg  = T_break;
            offset = j-1;
            break;
         }
      }
            
      Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
      C  = C + 3 * offset * N;
       
      for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
       //printf("\n Number of granules must be >= 1: check header data.\n");
    }

   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the nutation.                                       */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<2 ; i++ )
   {
      Cp[0]  = 1.0;                                 /* Begin polynomial sum */
      Cp[1]  = Tc;
      sum[i] = A[i*N] + A[1+i*N]*Tc;

      for ( j=2 ; j<N ; j++ )                                  /* Finish it */
      {
         Cp[j]  = 2.0 * Tc * Cp[j-1] - Cp[j-2];
         sum[i] = sum[i] + A[j+i*N] * Cp[j];
      }
      Nutation[i] = sum[i];
   }
      
   return;
}

/**==========================================================================**/
/**  Interpolate_Position                                                    **/
/**                                                                          **/
/**     This function computes a position vector for a selected planetary    **/
/**     body from Chebyshev coefficients read in from an ephemeris data      **/
/**     file. These coefficients are read from the data file by calling      **/
/**     the function Read_Coefficients (when necessary).                     **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Position -- Pointer to external array to receive the position.       **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**                                                                          **/
/**==========================================================================**/
void DeFile::Interpolate_Position( double Time , int Target , double Position[3] )
{
   double    A[50] , Cp[50]  , sum[3] , T_break , T_seg = 0.0 , T_sub , Tc = 0.0;
   int       i , j;
   long int  C , G , N , offset = 0;

   /*--------------------------------------------------------------------------*/
   /* This function doesn't "do" nutations or librations.                      */
   /*--------------------------------------------------------------------------*/
   if ( Target >= SS_BARY_ID )             /* Also protects against weird input errors */
   {
      //printf("\n This function does not compute nutations or librations.\n");
      return;
   }
 
   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<50 ; i++ )
   {
      A[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input (if so, get it).             */
   /*--------------------------------------------------------------------------*/
   if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/
   C = R1.coeffPtr[Target][0] - 1;          /*   Coefficient array entry point */
   N = R1.coeffPtr[Target][1];              /* Number of coeff's per component */
   G = R1.coeffPtr[Target][2];              /*      Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the ephemeris body is used to  */
   /*  load the coefficients.                                                  */
   /*--------------------------------------------------------------------------*/
   if ( G == 1 )
   {
      Tc = 2.0*(Time - T_beg) / T_span - 1.0;
      for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
   }
   else if ( G > 1 )
   {
      T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
      for ( j=G ; j>0 ; j-- )
      {
         T_break = T_beg + ((double) j-1) * T_sub;
         if ( Time > T_break )
         {
            T_seg  = T_break;
            offset = j-1;
            break;
         }
      }
            
      Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
      C  = C + 3 * offset * N;
       
      for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      // printf("\n Number of granules must be >= 1: check header data.\n");
   }


   /*--------------------------------------------------------------------------*/
   /* Compute interpolated the position.                                       */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<3 ; i++ )
   {
      Cp[0]  = 1.0;                                 /* Begin polynomial sum */
      Cp[1]  = Tc;
      sum[i] = A[i*N] + A[1+i*N]*Tc;

      for ( j=2 ; j<N ; j++ )                                  /* Finish it */
      {
         Cp[j]  = 2.0 * Tc * Cp[j-1] - Cp[j-2];
         sum[i] = sum[i] + A[j+i*N] * Cp[j];
      }
      Position[i] = sum[i];
   }

   return;
}

/**==========================================================================**/
/**  Interpolate_State                                                       **/
/**                                                                          **/
/**     This function computes position and velocity vectors for a selected  **/
/**     planetary body from Chebyshev coefficients that are read in from an  **/
/**     ephemeris data file. These coefficients are read from the data file  **/
/**     by calling the function Read_Coefficients (when necessary).          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Position -- Pointer to external array to receive the position.       **/
/**                                                                          **/
/**  Returns: Nothing (explicitly)                                           **/
/**==========================================================================**/
void DeFile::Interpolate_State(double Time , int Target, stateType *p)
{
   double    A[50]   , /*B[50] ,*/ Cp[50] , P_Sum[3] , V_Sum[3] , Up[50] ,
             Wp[50], A_Sum[3], 
                      T_break , T_seg = 0.0 , T_sub  , Tc = 0.0;
   int       i , j;
   long int  C , G , N , offset = 0;
   stateType X;

   /*--------------------------------------------------------------------------*/
   /* This function doesn't "do" nutations or librations.                      */
   /*--------------------------------------------------------------------------*/
   if ( Target >= SS_BARY_ID )             /* Also protects against weird input errors */
   {
      //printf("\n This function does not compute nutations or librations.\n");
      return;
   }

   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<50 ; i++ )
   {
      A[i] = 0.0;
//      B[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input.                             */
   /*--------------------------------------------------------------------------*/
   #ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State() before Read_Coefficients()\nTime=%f, T_beg=%f, "
       "T_end=%f T_span=%f\n", Time, T_beg, T_end, T_span);
   #endif
  
   if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);
  
   #ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State() after  Read_Coefficients()\nTime=%f, T_beg=%f, "
       "T_end=%f T_span=%f\n", Time, T_beg, T_end, T_span);
   #endif
  
   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/
  
   C = R1.coeffPtr[Target][0] - 1;               /*    Coeff array entry point */
   N = R1.coeffPtr[Target][1];                   /*          Number of coeff's */
   G = R1.coeffPtr[Target][2];                   /* Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the ephemeris body is used to  */
   /*  load the coefficients.                                                  */
   /*--------------------------------------------------------------------------*/
   if ( G == 1 )
   {
      Tc = 2.0*(Time - T_beg) / T_span - 1.0;
      for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
   }
   else if ( G > 1 )
   {
      T_sub = T_span / ((double) G);          /* Compute subgranule interval */
      for ( j=G ; j>0 ; j-- )
      {
         T_break = T_beg + ((double) j-1) * T_sub;
         if ( Time > T_break )
         {
            T_seg  = T_break;
            offset = j-1;
            break;
         }
      }

      Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
      C  = C + 3 * offset * N;
       
      for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      //printf("\n Number of granules must be >= 1: check header data.\n");
   }

   /*--------------------------------------------------------------------------*/
   /* Compute the interpolated position & velocity                             */
   /*--------------------------------------------------------------------------*/
   for ( i=0 ; i<3 ; i++ )                /* Compute interpolating polynomials */
   {
      Cp[0] = 1.0;
      Cp[1] = Tc;
      Cp[2] = 2.0 * Tc*Tc - 1.0;

      // Up[j] = d[Cp[j]]/dTc
      Up[0] = 0.0;
      Up[1] = 1.0;
      Up[2] = 4.0 * Tc;

      // Wp[j] = d[Up[j]]/dTc
      Wp[0] = 0.0;
      Wp[1] = 0.0;
      Wp[2] = 4.0;

      for ( j=3 ; j<N ; j++ )
      {
         Cp[j] = 2.0 * Tc * Cp[j-1] - Cp[j-2];
         Up[j] = 2.0 * Tc * Up[j-1] + 2.0 * Cp[j-1] - Up[j-2];
         Wp[j] = 2.0 * Tc * Wp[j - 1] + 4.0 * Up[j - 1] - Wp[j - 2];
      }

      P_Sum[i] = 0.0;           /* Compute interpolated position & velocity */
      V_Sum[i] = 0.0;
      A_Sum[i] = 0.0;

      for ( j=N-1 ; j>-1 ; j-- )  P_Sum[i] = P_Sum[i] + A[j+i*N] * Cp[j];
      for ( j=N-1 ; j>0  ; j-- )  V_Sum[i] = V_Sum[i] + A[j+i*N] * Up[j];
      for ( j=N-1 ; j> 0 ; j-- )  A_Sum[i] = A_Sum[i] + A[j+i*N] * Wp[j];

      X.Position[i] = P_Sum[i];
      X.Velocity[i] = V_Sum[i] * 2.0 * ((double) G) / (T_span * GmatTimeConstants::SECS_PER_DAY);
      // Calculate d[Tc]/dt
      double dTcdt = 2.0 * ((double) G) / (T_span * GmatTimeConstants::SECS_PER_DAY);
      // Reverse to the origin code to calculate velocity in order to prevent a tiny change in result
      //X.Velocity[i] = V_Sum[i] * dTcdt;
      X.Acceleration[i] = A_Sum[i] * dTcdt * dTcdt;
   }

   /*--------------------------------------------------------------------------*/
   /*  Return computed values.                                                 */
   /*--------------------------------------------------------------------------*/

   *p = X;
   //MessageInterface::ShowMessage("target = %d  epoch = %.12lf  State: [%.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf]\n", Target, Time, p->Position[0], p->Position[1], p->Position[2], p->Velocity[0], p->Velocity[1], p->Velocity[2]);
   return;
}


void DeFile::Interpolate_State(GmatTime Time, int Target, stateType *p)
{
   double    A[50], /*B[50] ,*/ Cp[50], P_Sum[3], V_Sum[3], Up[50],
      Wp[50], A_Sum[3], 
      T_break, T_seg = 0.0, T_sub, Tc = 0.0;
   int       i, j;
   long int  C, G, N, offset = 0;
   stateType X;

   /*--------------------------------------------------------------------------*/
   /* This function doesn't "do" nutations or librations.                      */
   /*--------------------------------------------------------------------------*/
   if (Target >= SS_BARY_ID)             /* Also protects against weird input errors */
   {
      //MessageInterface::ShowMessage("DeFile::Interpolate_State() function does not compute nutations or librations.\n");
      return;
   }

   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for (i = 0; i<50; i++)
   {
      A[i] = 0.0;
      //      B[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input.                             */
   /*--------------------------------------------------------------------------*/
#ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State() before Read_Coefficients()\nTime=%f, T_beg=%f, "
      "T_end=%f T_span=%f\n", Time, T_beg, T_end, T_span);
#endif

   if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

#ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State() after  Read_Coefficients()\nTime=%f, T_beg=%f, "
      "T_end=%f T_span=%f\n", Time, T_beg, T_end, T_span);
#endif

   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/

   C = R1.coeffPtr[Target][0] - 1;               /*    Coeff array entry point */
   N = R1.coeffPtr[Target][1];                   /*          Number of coeff's */
   G = R1.coeffPtr[Target][2];                   /* Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the ephemeris body is used to  */
   /*  load the coefficients.                                                  */
   /*--------------------------------------------------------------------------*/
   Real dTc;
   if (G == 1)
   {
      //Tc = 2.0*(Time.GetMjd() - T_beg) / T_span - 1.0;
      //dTc = 2.0*(Time - GmatTime(Time.GetMjd())).GetTimeInSec() / T_span / GmatTimeConstants::SECS_PER_DAY;
      //Tc = Tc + dTc;
      
      Tc = 2.0*(Time - T_beg).GetMjd() / T_span - 1.0;

      for (i = C; i<(C + 3 * N); i++)  A[i - C] = Coeff_Array[i];
   }
   else if (G > 1)
   {
      T_sub = T_span / ((double)G);          /* Compute subgranule interval */
      for (j = G; j>0; j--)
      {
         T_break = T_beg + ((double)j - 1) * T_sub;
         if (Time > T_break)
         {
            T_seg = T_break;
            offset = j - 1;
            break;
         }
      }

      //Tc = 2.0*(Time.GetMjd() - T_seg) / T_sub - 1.0;
      //dTc = 2.0*(Time - GmatTime(Time.GetMjd())).GetTimeInSec() / T_span / GmatTimeConstants::SECS_PER_DAY;
      //Tc = Tc + dTc;

      Tc = 2.0*(Time - T_seg).GetMjd() / T_sub - 1.0;
      C = C + 3 * offset * N;

      for (i = C; i<(C + 3 * N); i++) A[i - C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      //printf("\n Number of granules must be >= 1: check header data.\n");
   }

   /*--------------------------------------------------------------------------*/
   /* Compute the interpolated position & velocity                             */
   /*--------------------------------------------------------------------------*/
   for (i = 0; i<3; i++)                /* Compute interpolating polynomials */
   {
      Cp[0] = 1.0;
      Cp[1] = Tc;
      Cp[2] = 2.0 * Tc*Tc - 1.0;

      // Up[j] := d[Cp[j]]/dTc  for any j >= 0 
      Up[0] = 0.0;
      Up[1] = 1.0;
      Up[2] = 4.0 * Tc;

      // Wp[j] := d[Up[j]]/dTc  for any j >= 0 
      Wp[0] = 0.0;
      Wp[1] = 0.0;
      Wp[2] = 4.0;

      for (j = 3; j<N; j++)
      {
         Cp[j] = 2.0 * Tc * Cp[j - 1] - Cp[j - 2];
         Up[j] = 2.0 * Tc * Up[j - 1] + 2.0 * Cp[j - 1] - Up[j - 2];
         Wp[j] = 2.0 * Tc * Wp[j - 1] + 4.0 * Up[j - 1] - Wp[j - 2];
      }

      P_Sum[i] = 0.0;           /* Compute interpolated position & velocity */
      V_Sum[i] = 0.0;
      A_Sum[i] = 0.0;

      for (j = N - 1; j>-1; j--)  P_Sum[i] = P_Sum[i] + A[j + i*N] * Cp[j];
      for (j = N - 1; j>0; j--)  V_Sum[i] = V_Sum[i] + A[j + i*N] * Up[j];
      for (j = N - 1; j>0; j--)  A_Sum[i] = A_Sum[i] + A[j + i*N] * Wp[j];

      X.Position[i] = P_Sum[i];
      //X.Velocity[i] = V_Sum[i] * 2.0 * ((double)G) / (T_span * GmatTimeConstants::SECS_PER_DAY);
      X.Velocity[i] = V_Sum[i] * 2.0 * ((double)G) / T_span / GmatTimeConstants::SECS_PER_DAY;
      // Calculate d[Tc]/dt
      double dTcdt = 2.0 * ((double)G) / T_span / GmatTimeConstants::SECS_PER_DAY;
      // Reverse to the origin code to calculate velocity in order to prevent a tiny change in result
      //X.Velocity[i] = V_Sum[i] * dTcdt; 
      X.Acceleration[i] = A_Sum[i] * dTcdt *dTcdt;
   }

   /*--------------------------------------------------------------------------*/
   /*  Return computed values.                                                 */
   /*--------------------------------------------------------------------------*/

   *p = X;
   //MessageInterface::ShowMessage("target = %d  epoch = %.12lf  State: [%.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf]\n",Target, Time.GetMjd(), p->Position[0], p->Position[1], p->Position[2], p->Velocity[0], p->Velocity[1], p->Velocity[2]);
   return;
}


/**==========================================================================**/
/**  Interpolate_State_Delta                                                 **/
/**                                                                          **/
/**     This function computes position delta vectors for a                  **/
/**     selected planetary body from Chebyshev coefficients that are read in **/
/**     from an ephemeris data file. These coefficients are read from the    **/
/**     data file by calling the function Read_Coefficients (when necessary).**/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time1    -- Time for which position is desired (Julian Date).        **/
/**     Time2    -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Position -- Pointer to external array to receive the position.       **/
/**                                                                          **/
/**  Returns: Nothing (explicitly)                                           **/
/**==========================================================================**/
void DeFile::Interpolate_State_Delta(const GmatTime &Time, const GmatTime &Time2,
                                     int Target, stateType *p)
{
   double    A[50], /*B[50] ,*/ Cp[50], P_Sum[3], Cp_Delta[50],
      T_break, T_seg = 0.0, T_sub, Tc = 0.0, Tc2 = 0.0, dt = 0.0;
   int       i, j;
   long int  C, G, N, offset = 0;
   stateType X;

   /*--------------------------------------------------------------------------*/
   /* This function doesn't "do" nutations or librations.                      */
   /*--------------------------------------------------------------------------*/
   if (Target >= SS_BARY_ID)             /* Also protects against weird input errors */
   {
      //MessageInterface::ShowMessage("DeFile::Interpolate_State_Delta() function does not compute nutations or librations.\n");
      return;
   }

   /*--------------------------------------------------------------------------*/
   /* Initialize local coefficient array.                                      */
   /*--------------------------------------------------------------------------*/
   for (i = 0; i<50; i++)
   {
      A[i] = 0.0;
      //      B[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /* Determine if a new record needs to be input.                             */
   /*--------------------------------------------------------------------------*/
#ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State_Delta() before Read_Coefficients()\nTime=%f, Time2=%f, T_beg=%f, "
      "T_end=%f T_span=%f\n", Time, Time2, T_beg, T_end, T_span);
#endif

   if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

#ifdef DEBUG_DEFILE_INTERPOLATE
   MessageInterface::ShowMessage
      ("DeFile::Interpolate_State_Delta() after  Read_Coefficients()\nTime=%f, Time2=%f, T_beg=%f, "
      "T_end=%f T_span=%f\n", Time, Time2, T_beg, T_end, T_span);
#endif

   /*--------------------------------------------------------------------------*/
   /* Read the coefficients from the binary record.                            */
   /*--------------------------------------------------------------------------*/

   C = R1.coeffPtr[Target][0] - 1;               /*    Coeff array entry point */
   N = R1.coeffPtr[Target][1];                   /*          Number of coeff's */
   G = R1.coeffPtr[Target][2];                   /* Granules in current record */

   /*--------------------------------------------------------------------------*/
   /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
   /*  into array A[]. If T_span is covered by a single granule this is easy.  */
   /*  If not, the granule that contains the interpolation time is found, and  */
   /*  an offset from the array entry point for the ephemeris body is used to  */
   /*  load the coefficients.                                                  */
   /*--------------------------------------------------------------------------*/
   Real dTc;
   if (G == 1)
   {
      //Tc = 2.0*(Time.GetMjd() - T_beg) / T_span - 1.0;
      //dTc = 2.0*(Time - GmatTime(Time.GetMjd())).GetTimeInSec() / T_span / GmatTimeConstants::SECS_PER_DAY;
      //Tc = Tc + dTc;
      
      Tc = 2.0*(Time - T_beg).GetMjd() / T_span - 1.0;
      Tc2 = 2.0*(Time2 - T_beg).GetMjd() / T_span - 1.0;
      dt = 2.0*(Time2 - Time).GetMjd() / T_span;

      for (i = C; i<(C + 3 * N); i++)  A[i - C] = Coeff_Array[i];
   }
   else if (G > 1)
   {
      T_sub = T_span / ((double)G);          /* Compute subgranule interval */
      for (j = G; j>0; j--)
      {
         T_break = T_beg + ((double)j - 1) * T_sub;
         if (Time > T_break)
         {
            T_seg = T_break;
            offset = j - 1;
            break;
         }
      }

      //Tc = 2.0*(Time.GetMjd() - T_seg) / T_sub - 1.0;
      //dTc = 2.0*(Time - GmatTime(Time.GetMjd())).GetTimeInSec() / T_span / GmatTimeConstants::SECS_PER_DAY;
      //Tc = Tc + dTc;

      Tc = 2.0*(Time - T_seg).GetMjd() / T_sub - 1.0;
      Tc2 = 2.0*(Time2 - T_seg).GetMjd() / T_sub - 1.0;
      dt = 2.0*(Time2 - Time).GetMjd() / T_sub;
      C = C + 3 * offset * N;

      for (i = C; i<(C + 3 * N); i++) A[i - C] = Coeff_Array[i];
   }
   else                                   /* Something has gone terribly wrong */
   {
      //printf("\n Number of granules must be >= 1: check header data.\n");
   }

   /*--------------------------------------------------------------------------*/
   /* Compute the interpolated position & velocity                             */
   /*--------------------------------------------------------------------------*/
   for (i = 0; i<3; i++)                /* Compute interpolating polynomials */
   {
      Cp[0] = 1.0;
      Cp[1] = Tc;
      Cp[2] = 2.0 * Tc*Tc - 1.0;

      Cp_Delta[0] = 0.0;
      Cp_Delta[1] = dt;
      Cp_Delta[2] = 2.0 * Tc2*dt + 2.0*dt*Tc;

      for (j = 3; j<N; j++)
      {
         Cp[j] = 2.0 * Tc * Cp[j - 1] - Cp[j - 2];
         Cp_Delta[j] = 2.0 * Tc2 * Cp_Delta[j - 1] + 2.0 * dt * Cp[j - 1] - Cp_Delta[j - 2];
      }

      P_Sum[i] = 0.0;           /* Compute interpolated position */

      for (j = N - 1; j>-1; j--)  P_Sum[i] = P_Sum[i] + A[j + i*N] * Cp_Delta[j];

      X.Position[i] = P_Sum[i];
      X.Velocity[i] = 0.0;
   }

   /*--------------------------------------------------------------------------*/
   /*  Return computed values.                                                 */
   /*--------------------------------------------------------------------------*/

   *p = X;
   //MessageInterface::ShowMessage("target = %d  epoch = %.12lf  State: [%.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf]\n",Target, Time.GetMjd(), p->Position[0], p->Position[1], p->Position[2], p->Velocity[0], p->Velocity[1], p->Velocity[2]);
   return;
}


//------------------------------------------------------------------------------
// private methods from JPL/JSC code ephem_util.c
//------------------------------------------------------------------------------

/******************************************************************************/
/**                                                                          **/
/**  SOURCE FILE: ephem_util.c                                               **/
/**                                                                          **/
/**    This file contains source code for the utility functions used in the  **/
/**    ephemeris manipulation programs.                                      **/
/**                                                                          **/
/**  Programmer: David Hoffman/EG5                                           **/
/**              NASA, Johnson Space Center                                  **/
/**              Houston, TX 77058                                           **/
/**              e-mail: david.a.hoffman1@jsc.nasa.gov                       **/
/**                                                                          **/
/******************************************************************************/

/**==========================================================================**/
/**  Find_Value                                                              **/
/**                                                                          **/
/**    Given a string, an array of strings, and an array of double precision **/
/**    numbers (both arrays having the same dimension) this function will    **/
/**    scan the string array to find a match and, if a match is found, it    **/
/**    will return the value of the double precision number whose position   **/
/**    in the double precision array corresponds to the location of the      **/
/**    match in the string array (follow all that?).                         **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**    name        - a string                                                **/
/**    name_array  - an array of strings                                     **/
/**    value_array - a double precision array                                **/
/**                                                                          **/
/**  Output:                                                                 **/
/**    A double precision number                                             **/
/**==========================================================================**/
double DeFile::Find_Value( char    name[], 
                   char    name_array[400][6] , 
                   double  value_array[400]  ) 
{
   double value = 0.0;
   //int    found = FALSE;
   char   target[7];
   int    i, j;

   for ( i=0 ; i<400 ; i++ )
   {
      /*.....................Convert current name array element to a string */
      for ( j=0 ; j<6 ; j++ )  target[j] = name_array[i][j];
      target[6] = '\0';

      /*................................See if it matches the string sought */
      if ( !strcmp(name,target) )
      {
         value = value_array[i];
         //found = TRUE;
         break;                                /* No need to keep looking */
      }                                       /*   once a match is found */
   }

   //if ( (i==399) & (found==FALSE) ) Warning(20);             /* Didn't find it */

   return value;
}

/**==========================================================================**/
/**  Gregorian_to_Julian                                                     **/
/**                                                                          **/
/**     This function takes a time,  specified in terms of a Gregorian date  **/
/**     (i.e. a modern calander date) plus a time of day (given in terms of  **/
/**     hours, minutes, and seconds since midnight) and  returns the corre-  **/
/**     sponding Julian date.  The algorithm used to do this calculation is  **/
/**     based on pp 60-61 of: Astronomical Algorithms, Jean Meeus, Willman-  **/
/**     Bell, Richmond VA, 1991.  It has been modified  here to allow users  **/
/**     to input hours, minutes, and seconds instead of fractional days.     **/
/**                                                                          **/
/**     IMPORTANT NOTE: This algorithm, despite the claim made in Meeus, is  **/
/**     only valid for years later than 0 A.D. (or ACE for the politically   **/
/**     correct).                                                            **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**      year (integer: positive for AD, negative for BC)                    **/
/**     month (integer: 1 through 12)                                        **/
/**       day (integer: 1 through 31)                                        **/
/**      hour (integer: 0 through 24)                                        **/
/**       min (integer: 0 through 60)                                        **/
/**       sec ( double: less than 60.0)                                      **/
/**                                                                          **/
/**  Output:                                                                 **/
/**     Julian date (double)                                                 **/
/**==========================================================================**/
double DeFile::Gregorian_to_Julian( int     year ,  int     month   , 
                            int     day  ,  int     hour    , 
                            int     min  ,  double  seconds )
{
   double  A, B, D, H, JD, M, N, Y;

   /*--------------------------------------------------------------------------*/
   /*  Adjust year & month values (if necessary).                              */
   /*--------------------------------------------------------------------------*/

   if ( month < 3 )
   {                                      /* See Muess, p. 61 */
      month = month + 12;
      year  = year - 1;
   }

   /*--------------------------------------------------------------------------*/
   /*  Perform type casts and adjust the day count.                            */
   /*--------------------------------------------------------------------------*/

   Y = (double) year;
   M = (double) month;
   D = (double) day;
   H = (double) hour;
   N = (double) min;

   /*--------------------------------------------------------------------------*/
   /*  Compute the day fraction:                                               */
   /*--------------------------------------------------------------------------*/
   D = D  +  (H / 24.0)  +  (N / 1440.0)  +  (seconds / GmatTimeConstants::SECS_PER_DAY);

   /*--------------------------------------------------------------------------*/
   /*  Compute the Julian date.                                                */
   /*--------------------------------------------------------------------------*/
   A  = floor(Y/100.0);
   B  = 2.0 - A + floor(A/4.0);
   JD = floor(GmatTimeConstants::DAYS_PER_YEAR*(Y+4716.0)) + floor(30.6001*(M+1.0)) + D + B - 1524.5;
  
   return JD;
}
                         

/**==========================================================================**/
/**  Integer modulo function (self-explanatory: returns x mod y)             **/
/**==========================================================================**/
int mod(int x, int y)
{
   double rf, rx, ry;
   int f;
  
   /*--------------------------------------------------------------------------*/
   /* The basic formula can be found on p.38 of Knuth's The Art of Computer    */
   /* Programming, volume 1. I still can't believe that C lacks this function, */
   /* but I can find no mention of it in any of my C books...                  */
   /*--------------------------------------------------------------------------*/
   if ( y != 0 )
   {
      rx = (double) x;
      ry = (double) y;
      rf = floor(rx/ry);
      f  = (int) rf;
      return x - y*f;
   }
   else
      return x;
}

/**==========================================================================**/
/**  Read_File_Line                                                          **/
/**                                                                          **/
/**     Reads in one line from an ASCII file. Specifically, it reads in 80   **/
/**     characters unless it encounters an end-of-line or an end-of-file.    **/
/**     If requested, it also converts FORTRAN scientific notation to the    **/
/**     equivalent C representation (in which case it converts any 'D' in    **/
/**     the line to an 'E').                                                 **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     inFile     - pointer to an ASCII file                                **/
/**     filter     - Boolean flag (OK, an integer)                           **/
/**     lineBuffer - an externally defined character array                   **/
/**                                                                          **/
/**  Returns:                                                                **/
/**     Nothing.                                                             **/
/**==========================================================================**/
int DeFile::Read_File_Line( FILE *inFile, int filter, char lineBuffer[82])
{
   char ignore[40];
   char *code;
   int  i, status = 0;

   /*--------------------------------------------------------------------------*/
   /* Read one line (81 characters max) from header file.                      */
   /*--------------------------------------------------------------------------*/
   code = fgets(lineBuffer,82,inFile);

   /*--------------------------------------------------------------------------*/
   /*  Protect against lines over 80 characters long.                          */
   /*--------------------------------------------------------------------------*/
   if ( (strlen(lineBuffer) == 81) && (lineBuffer[80] != '\n') )
   {
      // Intentionally get the return and then ignore it to move warning from
      // system libraries to GMAT code base.  The "unused variable" warning
      // here can be safely ignored.
      char* ch = fgets(ignore,40,inFile);      /* Read past next end of line */
      if (ch == NULL)
         throw PlanetaryEphemException("Unable to read line from the DE file");
      lineBuffer[81] = '\0';
   }

   /*--------------------------------------------------------------------------*/
   /* Convert FORTRAN exponential representation to C representation.          */
   /*--------------------------------------------------------------------------*/
   if ( filter == TRUE )
   {
      for ( i=0 ; i<82 ; i++ )
      {
         if (lineBuffer[i] == '\0') break;
         if (lineBuffer[i] ==  'D') lineBuffer[i] = 'E';
      }
   }

   /*--------------------------------------------------------------------------*/
   /* Quit when EOF encountered.                                               */
   /*--------------------------------------------------------------------------*/

   if ( code == NULL ) status = EOF;

   return status;
}

/**==========================================================================**/
/**  Read_Group_Header                                                       **/
/**                                                                          **/
/**    Each data group in the header file is preceded by a group header      **/
/**    which consists of a blank line, a line of text, and another blank     **/
/**    line. The line of text consists of the string: GROUP   10XX (where    **/
/**    XX could be 10, 30, 40, 41, or 50). This function reads the group     **/
/**    header and returns an integer that signifies which group was read.    **/
/**                                                                          **/
/**  Input:                                                                  **/
/**    inFile - File pointer                                                 **/
/**                                                                          **/
/**  Returns:                                                                **/
/**    Integer = 1 for GROUP 1010                                            **/
/**            = 2 for GROUP 1030                                            **/
/**            = 3 for GROUP 1040                                            **/
/**            = 4 for GROUP 1041                                            **/
/**            = 5 for GROUP 1050                                            **/
/**==========================================================================**/
int DeFile::Read_Group_Header(FILE *inFile)
{
   int   count   = 0;                               /* Initial values required */
   int   tripCnt = 1;
   char  appStr[2], charIn, headStr[10];
   int   group;

   /*--------------------------------------------------------------------------*/
   /* Build the group header string.                                           */
   /*--------------------------------------------------------------------------*/
   headStr[0] = '\0';                               /* Start with empty string */

   do
   {
      /*................................................. Read one character */
       
      charIn = fgetc(inFile);
       
      /*..........................................Do the right thing with it */
       
      if ( isgraph(charIn) )          /* Build sting of non-blank characters */
      {
         appStr[0] = charIn;
         appStr[1] = '\0';
         strcat(headStr,appStr);
      }
      else                                              /* Count line breaks */
      {
         if (charIn == '\n') count = count + 1;
      }
            
      /*.......................................Protect against strange input */
      if (feof(inFile)) break;

      if ( ++tripCnt > 247 )
      {
         //Warning(21);
         break;
      }
       
   } while (count < 3);

   /*--------------------------------------------------------------------------*/
   /* Determine the group whose data follows.                                  */
   /*--------------------------------------------------------------------------*/
   if (!strcmp(headStr,"GROUP1010")) group = 1;
   else if (!strcmp(headStr,"GROUP1030")) group = 2;
   else if (!strcmp(headStr,"GROUP1040")) group = 3;
   else if (!strcmp(headStr,"GROUP1041")) group = 4;
   else if (!strcmp(headStr,"GROUP1050")) group = 5;
   else                                   group = 0;
  
   return group;
}
