//$Id: NRLMsise00Atmosphere.cpp 9485 2011-07-18 00:11:14Z tdnguye2 $
//------------------------------------------------------------------------------
//                              NRLMsise00Atmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Tuan Dang Nguyen
// Created: 2011/07/18
//
/**
 * The NRLMSISE00 atmosphere
 */
//------------------------------------------------------------------------------

#include "NRLMsise00Atmosphere.hpp"
#include "MessageInterface.hpp"
#include "TimeSystemConverter.hpp"

#include "f2c.h"

#ifndef __SKIP_NRLMSISE00__
extern "C"
{
   int gtd7_(integer *iyd, real *sec, real *alt, real *glat,
        real *glong, real *stl, real *f107a, real *f107, real *ap, integer *
        mass, real *d__, real *t);
}
#endif

//#define DEBUG_NRLMSISE00_ATMOSPHERE
//#define DEBUG_GEODETICS
//#define DEBUG_FIRSTCALL

//#define DEBUG_SHOW_FLUX


#ifdef DEBUG_NRLMSISE00_ATMOSPHERE
static FILE *logFile;  // Temp log file
#endif


//------------------------------------------------------------------------------
//  NRLMsise00Atmosphere(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
NRLMsise00Atmosphere::NRLMsise00Atmosphere(const std::string &typeStr,
                const std::string &name) :
   AtmosphereModel     (typeStr, name),
   fileData            (false),
   fluxfilename        ("")
{
//	MessageInterface::PopupMessage(Gmat::WARNING_, "Enter NRLMsise00Atmosphere constructor");
	#ifdef DEBUG_NRLMSISE00_ATMOSPHERE
	logFile = fopen("GMAT-NRLMSISE00.txt", "w");
	#endif

//	MessageInterface::PopupMessage(Gmat::WARNING_, "Leave NRLMsise00Atmosphere constructor");
}


//------------------------------------------------------------------------------
//  ~NRLMsise00Atmosphere()
//------------------------------------------------------------------------------
/**
 *   Destructor.
 */
//------------------------------------------------------------------------------
NRLMsise00Atmosphere::~NRLMsise00Atmosphere()
{
}


//------------------------------------------------------------------------------
//  NRLMsise00Atmosphere(const NRLMsise00Atmosphere& msise)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 *
 * @param <msise> NRLMSISE00 object to copy in creating the new one.
 */
//------------------------------------------------------------------------------
NRLMsise00Atmosphere::NRLMsise00Atmosphere(const NRLMsise00Atmosphere& msise) :
AtmosphereModel     (msise),
fileData            (false),  // is this correct?
fluxfilename        (msise.fluxfilename),
mass                (msise.mass)
{
//	MessageInterface::PopupMessage(Gmat::WARNING_, "Enter NRLMsise00Atmosphere copy constructor");

   for (Integer i = 0; i < 7; i++)
      ap[i] = msise.ap[i];

//   MessageInterface::PopupMessage(Gmat::WARNING_, "Leave NRLMsise00Atmosphere copy constructor");
}

//------------------------------------------------------------------------------
//  NRLMsise00Atmosphere& operator= (const NRLMsise00Atmosphere& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NRLMsise00Atmosphere class.
 *
 * @param <m> the NRLMsise00Atmosphere object whose data to assign to "this"
 *            AtmosphereModel.
 *
 * @return "this" NRLMsise00Atmosphere with data of input NRLMsise00Atmosphere st.
 */
//------------------------------------------------------------------------------
NRLMsise00Atmosphere& NRLMsise00Atmosphere::operator=(const NRLMsise00Atmosphere& msise)
{
//	MessageInterface::PopupMessage(Gmat::WARNING_, "Enter operator =");
   if (&msise == this)
      return *this;

   AtmosphereModel::operator=(msise);

   fileData     = false;  // is this correct?
   fluxfilename = msise.fluxfilename;
   mass         = msise.mass;

//   MessageInterface::PopupMessage(Gmat::WARNING_, "Leave operator =");
   return *this;
}


//------------------------------------------------------------------------------
//  bool Density(Real *pos, Real *density, Real epoch, Integer count)
//------------------------------------------------------------------------------
/**
 *  Calculates the density at each of the states in the input vector, using the
 *  NRLMSISE00 atmosphere model.
 *
 *  @param pos       The input vector of spacecraft states
 *  @param density   The array of output densities
 *  @param epoch     The current TAIJulian epoch
 *  @param count     The number of spacecraft contained in pos
 */
//------------------------------------------------------------------------------
bool NRLMsise00Atmosphere::Density(Real *pos, Real *density, Real epoch,
                                Integer count)
{
   #ifdef DEBUG_FIRSTCALL
      static bool firstcall = true;
   #endif

   Integer i, i6;
   Real    alt;

   Real    lst;     // Local apparent solar time (Hrs)
   Real    den[9], temp[2];


   if (mCentralBody == NULL)
      throw AtmosphereException(
         "Central body pointer not set in NRLMSISE00 model.");

   Real utcEpoch = theTimeConverter->Convert(epoch, TimeSystemConverter::A1MJD,
         TimeSystemConverter::UTCMJD, GmatTimeConstants::JD_JAN_5_1941);
   GetInputs(utcEpoch);

   #ifdef DEBUG_SHOW_FLUX
      MessageInterface::ShowMessage("%lf:      %lf   %lf      [%lf %lf %lf %lf "
            "%lf %lf %lf]\n", epoch, f107, f107a, ap[0], ap[1], ap[2], ap[3],
            ap[4], ap[5], ap[6]);
   #endif

   int xyd = yd;
   float xsod = (float)sod;
   float xalt; //alt;
   float xlat; // Geodetic Latitude
   float xlon; //lon;
   float xlst;
   float xf107a = (float)f107a;
   float xf107 = (float)f107;
   int xmass;
   float xap[7];
   float xden[9];
   float xtemp[2];

   Integer j;
   for (j = 0; j < 7; j++)
      xap[j]   = (float)ap[j];
   for (j = 0; j < 9; j++)
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

      alt = CalculateGeodetics(&pos[i6], epoch, true);
      lst = sod/3600.0 + geoLong/15.0;


      #ifdef DEBUG_GEODETICS
         MessageInterface::ShowMessage("Diffs:\n");
         MessageInterface::ShowMessage("   Height:    %.12lf vs %.12lf\n", geoHeight, alt);
         MessageInterface::ShowMessage("   Latitude:  %.12lf vs %.12lf\n", geoLat, geolat);
         MessageInterface::ShowMessage("   Longitude: %.12lf vs %.12lf\n", geoLong, lon);
      #endif

      #ifdef DEBUG_NRLMSISE00_ATMOSPHERE
         MessageInterface::ShowMessage(
               "   GeodeticLat = %lf\n", geoLat);
      #endif

      #ifdef DEBUG_NRLMSISE00_ATMOSPHERE
         MessageInterface::ShowMessage(
            "Calculating NRLMSISE00 Density from parameters:\n   "
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

      #ifdef DEBUG_NRLMSISE00_ATMOSPHERE
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
      #endif

      #ifndef __SKIP_NRLMSISE00__
        gtd7_((integer*)&xyd,&xsod,&xalt,&xlat,&xlon,&xlst,&xf107a,&xf107,&xap[0],(integer*)&xmass,
                  &xden[0], &xtemp[0]);
      #endif

      #ifdef DEBUG_NRLMSISE00_ATMOSPHERE
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

      density[i] = ((double)xden[5] * 1000.0);	// convert unit from cg/cm**3 to kg/m**3

      #ifdef DEBUG_FIRSTCALL
         if (firstcall)
         {
            MessageInterface::ShowMessage("==================================\n");
            MessageInterface::ShowMessage("NRLMSISE00 Model, First call data:\n");
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

      #ifdef DEBUG_NRLMSISE00_ATMOSPHERE
         MessageInterface::ShowMessage(
            "   Density = %15.9le\n", density[i]);
      #endif
   }

//   MessageInterface::PopupMessage(Gmat::WARNING_, "Leave Density()");
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
GmatBase* NRLMsise00Atmosphere::Clone() const
{
//	MessageInterface::PopupMessage(Gmat::WARNING_, "Enter and Leave Clone()");
   return (new NRLMsise00Atmosphere(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void NRLMsise00Atmosphere::Copy(const GmatBase* orig)
{
//	MessageInterface::PopupMessage(Gmat::WARNING_, "Enter and Leave Copy()");
   operator=(*((NRLMsise00Atmosphere *)(orig)));
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool NRLMsise00Atmosphere::Initialize()
{
   return AtmosphereModel::Initialize();
}

