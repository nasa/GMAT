//$Id$
//------------------------------------------------------------------------------
//                              Msise90Atmosphere
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
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * The MSISE90 atmosphere
 */
//------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "Msise90Atmosphere.hpp"
#include "MessageInterface.hpp"
#include "CelestialBody.hpp"
#include "TimeSystemConverter.hpp"
#include "TimeTypes.hpp"


//#define DEBUG_NAN_CONDITIONS
//#define DEBUG_FIRSTCALL

//#define DUMP_FLUX_DATA

#ifndef __SKIP_MSISE90__
extern "C" 
{ 
   #ifdef USE_64_BIT_LONGS
      void gtd6_(long int*,float*,float*,float*,float*,float*,float*,float*,float*,long int*,float*,float*);
   #else
      void gtd6_(int*,float*,float*,float*,float*,float*,float*,float*,float*,int*,float*,float*);
   #endif
}
#endif

//#define DEBUG_MSISE90_ATMOSPHERE

#ifdef DEBUG_MSISE90_ATMOSPHERE
static FILE *logFile;  // Temp log file
#endif

//------------------------------------------------------------------------------
//  Msise90Atmosphere(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 *  Constructor.
 *
 *  @param name   Name of the Msise90Atmosphere (default is blank)
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::Msise90Atmosphere(const std::string &name) :
    AtmosphereModel     ("MSISE90", name),
    mass                (0)
{
    #ifdef DEBUG_MSISE90_ATMOSPHERE
    logFile = NULL;
    #endif
    for (Integer i = 0; i < 7; i++)
       ap[i] = 0.0;

}

//------------------------------------------------------------------------------
//  ~Msise90Atmosphere()
//------------------------------------------------------------------------------
/**
 *   Destructor.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::~Msise90Atmosphere()
{ 
}

//------------------------------------------------------------------------------
//  Msise90Atmosphere(const Msise90Atmosphere& msise)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 *
 * @param <msise> MSISE90 object to copy in creating the new one.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::Msise90Atmosphere(const Msise90Atmosphere& msise) :
   AtmosphereModel     (msise),
   mass                (msise.mass)
{
   for (Integer i = 0; i < 7; i++)
      ap[i] = msise.ap[i];
}

//------------------------------------------------------------------------------
//  Msise90Atmosphere& operator= (const Msise90Atmosphere& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Msise90Atmosphere class.
 *
 * @param <m> the Msise90Atmosphere object whose data to assign to "this"
 *            AtmosphereModel.
 *
 * @return "this" Msise90Atmosphere with data of input Msise90Atmosphere st.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere& Msise90Atmosphere::operator=(const Msise90Atmosphere& msise)
{
   if (&msise == this)
      return *this;

   AtmosphereModel::operator=(msise);
   
   mass         = msise.mass;
   
   for (Integer i = 0; i < 7; i++)
      ap[i] = msise.ap[i];
      
   return *this;
}

//------------------------------------------------------------------------------
//  bool Density(Real *pos, Real *density, Real epoch, Integer count)
//------------------------------------------------------------------------------
/**
 *  Calculates the density at each of the states in the input vector, using the 
 *  MSISE90 atmosphere model.
 * 
 *  @param pos       The input vector of spacecraft states
 *  @param density   The array of output densities
 *  @param epoch     The current TAIJulian epoch
 *  @param count     The number of spacecraft contained in pos 
 */
//------------------------------------------------------------------------------
bool Msise90Atmosphere::Density(Real *pos, Real *density, Real epoch, 
                                Integer count)
{
   #ifdef DEBUG_FIRSTCALL
      static bool firstcall = true;
   #endif

   Integer i, i6;

   #ifdef DEBUG_GEODETICS
      Real    alt;
   #endif

   Real    lst;     // Local apparent solar time (Hrs)
   Real    den[8], temp[2];
   
   #ifdef DEBUG_MSISE90_ATMOSPHERE
   logFile = fopen("GMAT-MSISE90.txt", "w");
   #endif
   
   if (mCentralBody == NULL)
      throw AtmosphereException(
         "Central body pointer not set in MSISE90 model.");

   Real utcEpoch = theTimeConverter->Convert(epoch, TimeSystemConverter::A1MJD,
		   theTimeConverter->UTCMJD, GmatTimeConstants::JD_JAN_5_1941);

   GetInputs(utcEpoch);


   #ifdef DUMP_FLUX_DATA
      MessageInterface::ShowMessage("%.12lf  %lf  %lf  [%lf  %lf  %lf  %lf  %lf  %lf  %lf]\n",
         epoch, f107, f107a, ap[0], ap[1], ap[2], ap[3], ap[4], ap[5], ap[6]);
   #endif

   int xyd    = yd;
   float xsod = (float)sod;
   float xalt; //alt;
   float xlat; // Geodetic Latitude
   float xlon; //lon;
   float xlst;
   float xf107a = (float)f107a;
   float xf107  = (float)f107;
   int xmass;
   float xap[7];
   float xden[8];
   float xtemp[2];

   Integer j;
   for (j = 0; j < 7; j++)  
      xap[j]   = (float)ap[j];
   for (j = 0; j < 8; j++)  
   {
      den[j]  = 0.0;
      xden[j] = (float)den[j];
   }
   for (j = 0; j < 2; j++)
   {
      temp[j]  = 0.0;
      xtemp[j] = (float)temp[j];
   }
   for (i = 0; i < count; ++i) 
   {
      i6 = i*6;
      mass = 48;

      #ifdef DEBUG_GEODETICS
         alt =
      #endif
      CalculateGeodetics(&pos[i6], epoch, true);
      lst = sod/3600.0 + geoLong/15.0;

      #ifdef DEBUG_GEODETICS
         MessageInterface::ShowMessage("Diffs:\n");
         MessageInterface::ShowMessage("   Height:    %.12lf vs %.12lf\n", geoHeight, alt);
         MessageInterface::ShowMessage("   Latitude:  %.12lf vs %.12lf\n", geoLat, geolat);
         MessageInterface::ShowMessage("   Longitude: %.12lf vs %.12lf\n", geoLong, lon);
      #endif

      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
               "   GeodeticLat = %lf\n", geoLat);
      #endif
      
      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "Calculating MSISE90 Density from parameters:\n   "
            "yd = %d\n   sod = %.12lf\n   alt = %.12lf\n   lat = %.12lf\n   "
            "lon = %.12lf\n   lst = %.12lf\n   f107a = %.12lf\n   "
            "f107 = %.12lf\n   ap = [%.12lf %.12lf %.12lf %.12lf %.12lf "
            "%.12lf %.12lf]\n   w = [%.12le %.12le %.12le]\n", yd, sod,
            geoHeight, geoLat, geoLong, lst, f107a, f107, ap[0], ap[1], ap[2],
            ap[3], ap[4], ap[5], ap[6], angVel[0], angVel[1], angVel[2]);
      #endif
      
      xalt = (float)geoHeight;
      xlat = (float)geoLat;
      xlon = (float)geoLong;
      xlst = (float)lst;
      xmass = mass;

      #ifdef DEBUG_MSISE90_ATMOSPHERE
      MessageInterface::ShowMessage("Writing Pre-GTDS6 MSISE90 data to log file ...\n");
      fprintf(logFile, "Pre-GTDS6() \n");
      fprintf(logFile, "=========== \n");
      fprintf(logFile, "Epoch                  = %le \n", epoch);
      fprintf(logFile, "Year & Days            = %d \n", xyd);
      fprintf(logFile, "Seconds                = %le \n", xsod);
      fprintf(logFile, "Altitude               = %le \n", xalt);
      fprintf(logFile, "Latitude               = %le \n", xlat);
      fprintf(logFile, "Longitude              = %le \n", xlon);
      fprintf(logFile, "Solar Time             = %le \n", xlst);
      fprintf(logFile, "F107 Average           = %le \n", xf107a);
      fprintf(logFile, "F107                   = %le \n", xf107);
      fprintf(logFile, "Geomagnetic index[0]   = %le \n", xap[0]);
      fprintf(logFile, "Geomagnetic index[1]   = %le \n", xap[1]);
      fprintf(logFile, "Geomagnetic index[2]   = %le \n", xap[2]);
      fprintf(logFile, "Geomagnetic index[3]   = %le \n", xap[3]);
      fprintf(logFile, "Geomagnetic index[4]   = %le \n", xap[4]);
      fprintf(logFile, "Geomagnetic index[5]   = %le \n", xap[5]);
      fprintf(logFile, "Geomagnetic index[6]   = %le \n", xap[6]);
      fprintf(logFile, "Mass                   = %d \n", xmass);
      fprintf(logFile, "HE Number Density      = %le \n", xden[0]);
      fprintf(logFile, "O Number Density       = %le \n", xden[1]);
      fprintf(logFile, "N2 Number Density      = %le \n", xden[2]);
      fprintf(logFile, "O2 Number Density      = %le \n", xden[3]);
      fprintf(logFile, "AR Number Density      = %le \n", xden[4]);
      fprintf(logFile, "Total Mass Density     = %le \n", xden[5]);
      fprintf(logFile, "H Number Density       = %le \n", xden[7]);
      fprintf(logFile, "EXOSPHERIC Temperature = %le \n", xtemp[0]);
      fprintf(logFile, "Temperature at Alt     = %le \n", xtemp[1]);
      fprintf(logFile, "\n");
      fprintf(logFile, "\n");
      MessageInterface::ShowMessage("DONE Writing Pre-GTDS6 MSISE90 data to log file ...\n");
      #endif
      
      #ifndef __SKIP_MSISE90__
         #ifdef DEBUG_MSISE90_ATMOSPHERE
            MessageInterface::ShowMessage("About to call gtd6_ ...\n");
         #endif
         #ifdef USE_64_BIT_LONGS
            long int xydLong = (long int) xyd;
            long int xmassLong = (long int) xmass;
            gtd6_(&xydLong,&xsod,&xalt,&xlat,&xlon,&xlst,&xf107a,&xf107,&xap[0],&xmassLong,
                  &xden[0],&xtemp[0]);
         #else
            gtd6_(&xyd,&xsod,&xalt,&xlat,&xlon,&xlst,&xf107a,&xf107,&xap[0],&xmass,
                  &xden[0],&xtemp[0]);
         #endif
         #ifdef DEBUG_MSISE90_ATMOSPHERE
            MessageInterface::ShowMessage("DONE calling gtd6_ ...\n");
         #endif
      #endif
      
      #ifdef DEBUG_MSISE90_ATMOSPHERE
      fprintf(logFile, "Post-GTDS6() \n");
      fprintf(logFile, "=========== \n");
      fprintf(logFile, "Epoch                  = %le \n", epoch);
      fprintf(logFile, "Year & Days            = %d \n", xyd);
      fprintf(logFile, "Seconds                = %le \n", xsod);
      fprintf(logFile, "Altitude               = %le \n", xalt);
      fprintf(logFile, "Latitude               = %le \n", xlat);
      fprintf(logFile, "Longitude              = %le \n", xlon);
      fprintf(logFile, "Solar Time             = %le \n", xlst);
      fprintf(logFile, "F107 Average           = %le \n", xf107a);
      fprintf(logFile, "F107                   = %le \n", xf107);
      fprintf(logFile, "Geomagnetic index[0]   = %le \n", xap[0]);
      fprintf(logFile, "Geomagnetic index[1]   = %le \n", xap[1]);
      fprintf(logFile, "Geomagnetic index[2]   = %le \n", xap[2]);
      fprintf(logFile, "Geomagnetic index[3]   = %le \n", xap[3]);
      fprintf(logFile, "Geomagnetic index[4]   = %le \n", xap[4]);
      fprintf(logFile, "Geomagnetic index[5]   = %le \n", xap[5]);
      fprintf(logFile, "Geomagnetic index[6]   = %le \n", xap[6]);
      fprintf(logFile, "Mass                   = %d \n", xmass);
      fprintf(logFile, "HE Number Density      = %le \n", xden[0]);
      fprintf(logFile, "O Number Density       = %le \n", xden[1]);
      fprintf(logFile, "N2 Number Density      = %le \n", xden[2]);
      fprintf(logFile, "O2 Number Density      = %le \n", xden[3]);
      fprintf(logFile, "AR Number Density      = %le \n", xden[4]);
      fprintf(logFile, "Total Mass Density     = %le \n", xden[5]);
      fprintf(logFile, "H Number Density       = %le \n", xden[6]);
      fprintf(logFile, "N Number Density       = %le \n", xden[7]);
      fprintf(logFile, "EXOSPHERIC Temperature = %le \n", xtemp[0]);
      fprintf(logFile, "Temperature at Alt     = %le \n", xtemp[1]);
      fprintf(logFile, "\n");
      fprintf(logFile, "\n");
      #endif
      
      density[i] = xden[5] * 1000.0;

      #ifdef DEBUG_FIRSTCALL
         if (firstcall)
         {
            MessageInterface::ShowMessage("==================================\n");
            MessageInterface::ShowMessage("MSISE90 Model, First call data:\n");
            MessageInterface::ShowMessage("   Year/DOY:    %d\n", xyd);
            MessageInterface::ShowMessage("   SOD:         %.12lf\n", xsod);
            MessageInterface::ShowMessage("   MJD:         %.12lf\n", epoch);
            MessageInterface::ShowMessage("   Altitude:    %.12lf\n", xalt);
            MessageInterface::ShowMessage("   Density:     %.12le\n", density[0]*1e9);
            MessageInterface::ShowMessage("   F10.7:       %.12lf\n", xf107);
            MessageInterface::ShowMessage("   F10.7a:      %.12lf\n", xf107a);
            MessageInterface::ShowMessage("   Ap:          [%lf %lf %lf %lf "
                  "%lf %lf %lf]\n", xap[0], xap[1], xap[2], xap[3], xap[4],
                  xap[5], xap[6]);
            MessageInterface::ShowMessage("==================================\n");

            firstcall = false;
         }
      #endif

      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "   Altitude = %15.9lf  Density = %15.9le\n", alt, density[i]);
      #endif

      #ifdef DEBUG_NAN_CONDITIONS
         if (GmatMathUtil::IsNaN(xden[5]))
         {
            MessageInterface::ShowMessage("NAN found in MSISE90 "
                  "density element %d, Value is %lf at epoch %.12lf\n", j,
                  xden[5], epoch);
            MessageInterface::ShowMessage(
               "   Position:   %16.9le  %16.9le  %16.9le\n",
               pos[i6], pos[i6+1], pos[i6+2]);
            MessageInterface::ShowMessage("   Epoch                  = %le \n", epoch);
            MessageInterface::ShowMessage("   Year & Days            = %d \n", xyd);
            MessageInterface::ShowMessage("   Seconds                = %le \n", xsod);
            MessageInterface::ShowMessage("   Altitude               = %le \n", xalt);
            MessageInterface::ShowMessage("   Latitude               = %le \n", xlat);
            MessageInterface::ShowMessage("   Longitude              = %le \n", xlon);
            MessageInterface::ShowMessage("   Solar Time             = %le \n", xlst);
            MessageInterface::ShowMessage("   F107 Average           = %le \n", xf107a);
            MessageInterface::ShowMessage("   F107                   = %le \n", xf107);
            MessageInterface::ShowMessage("   Geomagnetic index[0]   = %le \n", xap[0]);
            MessageInterface::ShowMessage("   Geomagnetic index[1]   = %le \n", xap[1]);
            MessageInterface::ShowMessage("   Geomagnetic index[2]   = %le \n", xap[2]);
            MessageInterface::ShowMessage("   Geomagnetic index[3]   = %le \n", xap[3]);
            MessageInterface::ShowMessage("   Geomagnetic index[4]   = %le \n", xap[4]);
            MessageInterface::ShowMessage("   Geomagnetic index[5]   = %le \n", xap[5]);
            MessageInterface::ShowMessage("   Geomagnetic index[6]   = %le \n", xap[6]);
            MessageInterface::ShowMessage("   Mass                   = %d \n", xmass);
            MessageInterface::ShowMessage("   HE Number Density      = %le \n", xden[0]);
            MessageInterface::ShowMessage("   O Number Density       = %le \n", xden[1]);
            MessageInterface::ShowMessage("   N2 Number Density      = %le \n", xden[2]);
            MessageInterface::ShowMessage("   O2 Number Density      = %le \n", xden[3]);
            MessageInterface::ShowMessage("   AR Number Density      = %le \n", xden[4]);
            MessageInterface::ShowMessage("   Total Mass Density     = %le \n", xden[5]);
            MessageInterface::ShowMessage("   H Number Density       = %le \n", xden[6]);
            MessageInterface::ShowMessage("   N Number Density       = %le \n", xden[7]);
            MessageInterface::ShowMessage("   EXOSPHERIC Temperature = %le \n", xtemp[0]);
            MessageInterface::ShowMessage("   Temperature at Alt     = %le \n", xtemp[1]);
            MessageInterface::ShowMessage("\n");
         }
      #endif
   }
   
   #ifdef DEBUG_MSISE90_ATMOSPHERE
   fflush(logFile);
   fclose(logFile);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone the object (inherited from GmatBase).
 *
 * @return a clone of "this" object.
 */
//------------------------------------------------------------------------------
GmatBase* Msise90Atmosphere::Clone() const
{
   return (new Msise90Atmosphere(*this));
}
