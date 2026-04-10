//$Id$
//------------------------------------------------------------------------------
//                                  Moon
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
// Created: 2004/01/29
//
/**
 * Implementation of the Moon class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Moon.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::

//#define DEBUG_MOON 1

using namespace GmatMathUtil;


// initialize static default values

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Moon(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Moon class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "Luna").
 */
//------------------------------------------------------------------------------
Moon::Moon(std::string name) :
   CelestialBody     ("Moon",name)
{
   objectTypeNames.push_back("Moon");
   parameterCount      = MoonParamCount;
   
   // Set default colors
   SetDefaultColors(GmatColor::TAN, GmatColor::DARK_GRAY);
   
   theCentralBodyName  = GmatSolarSystemDefaults::EARTH_NAME; // by default, the Moon is Luna 
   bodyType            = Gmat::MOON;
   bodyNumber          = 2;
   referenceBodyNumber = 3;
   if (name == GmatSolarSystemDefaults::MOON_NAME) rotationSrc         = Gmat::DE_FILE;
   else                                rotationSrc         = Gmat::IAU_SIMPLIFIED;
   

   if (name == GmatSolarSystemDefaults::MOON_NAME)  
   {
      FileManager *fm = FileManager::Instance();
      std::string path = fm->GetFullPathname(FileManager::PLANETARY_COEFF_PATH);
      std::string lunaCurrent = fm->GetFilename(FileManager::LUNA_PCK_CURRENT_FILE);
      std::string lunaFrame   = fm->GetFilename(FileManager::LUNA_FRAME_KERNEL_FILE);
      attitudeSpiceKernelNames.push_back(path+lunaCurrent);
      frameSpiceKernelNames.push_back(path+lunaFrame);
//      attitudeSpiceKernelNames.push_back(path+"moon_pa_de421_1900-2050.bpc");
//      frameSpiceKernelNames.push_back(path+"moon_080317.tf");
   }

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Moon(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Moon class
 * (constructor).
 *
 * @param <name>  optional parameter indicating the name of the celestial
 *                body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Moon::Moon(std::string name, const std::string &cBody) :
   CelestialBody     ("Moon",name)
{
   objectTypeNames.push_back("Moon");
   parameterCount      = MoonParamCount;

   theCentralBodyName  = cBody; 
   bodyType            = Gmat::MOON;
   bodyNumber          = 2;
   referenceBodyNumber = 3;
   if (name == GmatSolarSystemDefaults::MOON_NAME) rotationSrc         = Gmat::DE_FILE;
   else                                rotationSrc         = Gmat::IAU_SIMPLIFIED;

   if (name == GmatSolarSystemDefaults::MOON_NAME)  
   {
      FileManager *fm = FileManager::Instance();
      std::string path = fm->GetFullPathname(FileManager::PLANETARY_COEFF_PATH);
      std::string lunaCurrent = fm->GetFilename(FileManager::LUNA_PCK_CURRENT_FILE);
      std::string lunaFrame   = fm->GetFilename(FileManager::LUNA_FRAME_KERNEL_FILE);
      attitudeSpiceKernelNames.push_back(path+lunaCurrent);
      frameSpiceKernelNames.push_back(path+lunaFrame);
//      attitudeSpiceKernelNames.push_back(path+"moon_pa_de421_1900-2050.bpc");
//      frameSpiceKernelNames.push_back(path+"moon_080317.tf");
   }

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Moon(const Moon &m)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Moon class as a copy of the
 * specified Moon class (copy constructor).
 *
 * @param <m> Moon object to copy.
 */
//------------------------------------------------------------------------------
Moon::Moon(const Moon &m) :
   CelestialBody (m)
{
}

//------------------------------------------------------------------------------
//  Moon& operator= (const Moon& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Moon class.
 *
 * @param <m> the Moon object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Moon with data of input Moon m.
 */
//------------------------------------------------------------------------------
Moon& Moon::operator=(const Moon &m)
{
   if (&m == this)
      return *this;

   CelestialBody::operator=(m);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Moon()
//------------------------------------------------------------------------------
/**
 * Destructor for the Moon class.
 */
//------------------------------------------------------------------------------
Moon::~Moon()
{
}

//------------------------------------------------------------------------------
//  Rvector GetBodyCartographicCoordinates(const A1Mjd &forTime) const
//------------------------------------------------------------------------------
/**
 * This method returns the cartographic coordinates for the planet.
 * 
 * @param <forTime> time for which to compute the cartographic coordinates.
 *
 * @return vector containing alpha, delta, W, Wdot.
 *
 * @note currently only implemented for the moons (listed in IAU doc) of our
 *       Solar System.  See "Report of the IAU/IAG Working Group on
 *       Cartographic Coordinates and Rotational Elements of the Planets
 *       and Satellites: 2000"
 */
//------------------------------------------------------------------------------
Rvector Moon::GetBodyCartographicCoordinates(const A1Mjd &forTime) const
{
   Real alpha = 0;
   Real delta = 0;
   Real W     = 0;
   Real Wdot  = 0.0;
   // intermediate angle computations, in radians (for trig functions)
   Real p1  = 0.0, p2  = 0.0, p3  = 0.0, p4  = 0.0;
   Real p5  = 0.0, p6  = 0.0, p7  = 0.0, p8  = 0.0;
   Real p9  = 0.0, p10 = 0.0, p11 = 0.0, p12 = 0.0;
   Real p13 = 0.0;
   // Real p14 = 0.0, p15 = 0.0, p16 = 0.0;
   Real d = GetJulianDaysFromTDBEpoch(forTime); // interval in Julian days
   Real T = d / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY; // interval in Julian centuries
   // Compute for Earth's Moon
   if (theCentralBodyName == GmatSolarSystemDefaults::EARTH_NAME) 
   {
//      if (rotationSrc == Gmat::IAU_2002)
//      {
         p1  = Rad(125.045 -  0.0529921 * d);
         p2  = Rad(250.089 -  0.1059842 * d);
         p3  = Rad(260.008 + 13.0120009 * d);
         p4  = Rad(176.625 + 13.3407154 * d);
         p5  = Rad(357.529 +  0.9856003 * d);
         p6  = Rad(311.589 + 26.4057084 * d);
         p7  = Rad(134.963 + 13.0649930 * d);
         p8  = Rad(276.617 +  0.3287146 * d);
         p9  = Rad( 34.226 +  1.7484877 * d);
         p10 = Rad( 15.134 -  0.1589763 * d);
         p11 = Rad(119.743 +  0.0036096 * d);
         p12 = Rad(239.961 +  0.1643573 * d);
         p13 = Rad( 25.053 + 12.9590088 * d);
         
         alpha = 269.9949  
                +  0.0031 * T        -  3.8787 * Sin(p1)  -  0.1204 * Sin(p2)
                +  0.0700 * Sin(p3)  -  0.0172 * Sin(p4)  +  0.0072 * Sin(p6)
                -  0.0052 * Sin(p10) +  0.0043 * Sin(p13);
         delta =  66.5392   
                +  0.0130 * T        +  1.5419 * Cos(p1)  +  0.0239 * Cos(p2)
                -  0.0278 * Cos(p3)  +  0.0068 * Cos(p4)  -  0.0029 * Cos(p6)
                +  0.0009 * Cos(p7)  +  0.0008 * Cos(p10) -  0.0009 * Cos(p13);
         W     =  38.3213 
                + 13.17635815 * d    -  1.4E-12 * d * d   +  3.5610 * Sin(p1)
                +  0.1208 * Sin(p2)  -  0.0642 * Sin(p3)  +  0.0158 * Sin(p4)
                +  0.0252 * Sin(p5)  -  0.0066 * Sin(p6)  -  0.0047 * Sin(p7)
                -  0.0046 * Sin(p8)  +  0.0028 * Sin(p9)  +  0.0052 * Sin(p10)
                +  0.0040 * Sin(p11) +  0.0019 * Sin(p12) -  0.0044 * Sin(p13);
         Wdot  =  13.17635815
                -  2.8e-12 *    d     -  0.18870 * Cos(p1)
                -  0.01280 * Cos(p2)  -  0.835   * Cos(p3) +  0.211   * Cos(p4)
                +  0.0248  * Cos(p5)  -  0.17    * Cos(p6) -  0.061   * Cos(p7)
                -  0.0015  * Cos(p8)  +  0.0049  * Cos(p9) -  0.00083 * Cos(p10)
                +  0.00001 * Cos(p11) +  0.00031 * Cos(p12) - 0.057 * Cos(p13);
//      }
//      else if (rotationSrc == Gmat::DE_FILE)
//      {
//         DeFile *deFile = (DeFile*) theSourceFile;
//         if (!deFile) // assuming no SLP any more here, only DE405
//         {
//            throw  SolarSystemException(
//               "No DE file specified - cannot get/compute Moon data");
//         }
//         Real librationAngles[3], andRates[3];
//         deFile->GetAnglesAndRates(forTime, librationAngles, andRates, overrideTime);
//          // TBD ****************** how to use librationAngles & andRates
//         std::string errmsg = "Unable to compute cartographic coordinates for ";
//         errmsg += instanceName + " - DE_FILE not implemented in Moon class.\n";
//         throw SolarSystemException(errmsg);
//      }
   }
   // Compute for Mars' moons
   else if (theCentralBodyName == GmatSolarSystemDefaults::MARS_NAME)
   {
      if (rotationSrc == Gmat::IAU_2002)
      {
         p1  = Rad(169.51  -    0.4357640 * d);
         p2  = Rad(192.93  + 1128.4096700 * d +  8.864 * T * T);
         p3  = Rad( 53.47  -    0.0181510 * d);
         if (instanceName == GmatSolarSystemDefaults::PHOBOS_NAME)
         {
            alpha = 317.68           -    0.108 * T     +  1.79 * Sin(p1);
            delta =  52.90           -    0.061 * T     -  1.08 * Cos(p1);
            W     =  35.06           + 1128.8445850 * d +  8.864 * T * T  
                    - 1.42 * Sin(p1) -    0.78 * Sin(p2);
            Wdot  = 0.0;
            MessageInterface::ShowMessage("Wdot not yet computed for Phobos\n");
         }
         else  // Deimos
         {
            alpha = 316.65           -    0.108 * T     +  2.98 * Sin(p3);
            delta =  53.52           -    0.061 * T     -  1.78 * Cos(p3);
            W     =  79.41           +  285.1618970 * d -  0.520 * T * T  
                    - 2.58 * Sin(p3) +    0.19 * Cos(p3);
            Wdot  = 0.0;
            MessageInterface::ShowMessage("Wdot not yet computed for Deimos\n");
         }
      }
      else if (rotationSrc == Gmat::IAU_SIMPLIFIED)
      {
         return CelestialBody::GetBodyCartographicCoordinates(forTime);
      }
   }
   /// @todo add other moons to cartographic coordinate computations, when needed 
   else
   {
      return CelestialBody::GetBodyCartographicCoordinates(forTime);
   }
   
   return Rvector(4, alpha, delta, W, Wdot);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Moon.
 *
 * @return clone of the Moon.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Moon::Clone() const
{
   return (new Moon(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 *
 * @param <orig> The object that is being copied.
 */
//---------------------------------------------------------------------------
void Moon::Copy(const GmatBase* orig)
{
   operator=(*((Moon *)(orig)));
}

//---------------------------------------------------------------------------
//  bool NeedsOnlyMainSPK()
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the default SPK file contains
 * sufficient data for this Moon.
 *
 * @return flag indicating whether or not an additional SPK file is needed
 *         for this Moon; true, if only the default one is needed; false
 *         if an additional file is needed.
 */
//---------------------------------------------------------------------------
bool Moon::NeedsOnlyMainSPK()
{
   if (instanceName == GmatSolarSystemDefaults::MOON_NAME)  return true;
   return false;
}

//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Moon::SetIntegerParameter(const Integer id,
                                  const Integer value)
{
   if (instanceName == GmatSolarSystemDefaults::MOON_NAME)
   {
      if ((id == NAIF_ID) &&
          (value != GmatSolarSystemDefaults::MOON_NAIF_IDS[0]))
      {
         std::string errmsg = "NAIF ID for body Luna may not be modified.\n";
         throw SolarSystemException(errmsg);
      }
   }
   return CelestialBody::SetIntegerParameter(id,value);
}




//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
