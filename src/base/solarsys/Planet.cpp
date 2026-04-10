//$Id$
//------------------------------------------------------------------------------
//                                  Planet
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
 * Implementation of the Planet class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "FileManager.hpp"
#include "Rmatrix.hpp"
#include "Planet.hpp"
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "SolarSystemException.hpp"
#include "TimeSystemConverter.hpp"
#include "StateConversionUtil.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::

//#define DEBUG_PLANET 1
//#define DEBUG_PLANET_TWO_BODY
//#define DEBUG_PLANET_NUTATION_INTERVAL
//#define DEBUG_PLANET_CONSTRUCT
//#define DEBUG_PLANET_SET_STRING
//#define DEBUG_PLANET_INIT

using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
Planet::PARAMETER_TEXT[PlanetParamCount - CelestialBodyParamCount] =
{
   "NutationUpdateInterval",
   "EopFileName",
};

const Gmat::ParameterType
Planet::PARAMETER_TYPE[PlanetParamCount - CelestialBodyParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Planet(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "Earth").
 */
//------------------------------------------------------------------------------
Planet::Planet(std::string name) :
   CelestialBody     ("Planet",name),
   nutationUpdateInterval    (60.0),
   eopFileName               ("")
{   
   // @todo This constructor should call the other one, setting Sun as central body!!!
   #ifdef DEBUG_PLANET_CONSTRUCT
      MessageInterface::ShowMessage("In Planet constructor for %s\n", name.c_str());
   #endif
   objectTypeNames.push_back("Planet");
   parameterCount      = PlanetParamCount;
   
   // Set default colors
   SetDefaultColors(GmatColor::ORCHID, GmatColor::DARK_GRAY);
   
   theCentralBodyName  = GmatSolarSystemDefaults::SUN_NAME;
   bodyType            = Gmat::PLANET;
   bodyNumber          = 1;
   referenceBodyNumber = 3;
   rotationSrc         = Gmat::IAU_SIMPLIFIED;
   if (name == GmatSolarSystemDefaults::EARTH_NAME) 
      rotationSrc      = Gmat::FK5_IAU_1980;
   else if (name == GmatSolarSystemDefaults::NEPTUNE_NAME)
      rotationSrc      = Gmat::IAU_2002;


   if (name == GmatSolarSystemDefaults::EARTH_NAME)
   {
      #ifdef DEBUG_PLANET_CONSTRUCT
         MessageInterface::ShowMessage("In Planet constructor, setting default PCKs.\n");
      #endif
      FileManager *fm  = FileManager::Instance();
      std::string path = fm->GetFullPathname(FileManager::PLANETARY_COEFF_PATH);
      std::string earthLatest  = fm->GetFilename(FileManager::EARTH_LATEST_PCK_FILE);
      std::string earthPredict = fm->GetFilename(FileManager::EARTH_PCK_PREDICTED_FILE);
      std::string earthCurrent = fm->GetFilename(FileManager::EARTH_PCK_CURRENT_FILE);
      attitudeSpiceKernelNames.push_back(path+earthPredict);
      attitudeSpiceKernelNames.push_back(path+earthCurrent);
      attitudeSpiceKernelNames.push_back(path+earthLatest);
//      attitudeSpiceKernelNames.push_back(path+"earth_070425_370426_predict.bpc");
//      attitudeSpiceKernelNames.push_back(path+"earth_720101_070426.bpc");
//      attitudeSpiceKernelNames.push_back(path+"earth_latest_high_prec.bpc");
   }

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Planet(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class
 * (constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Planet::Planet(std::string name, const std::string &cBody) :
   CelestialBody     ("Planet",name),
   nutationUpdateInterval    (60.0),
   eopFileName               ("")
{
#ifdef DEBUG_PLANET_CONSTRUCT
   MessageInterface::ShowMessage("In Planet constructor for %s, with central body %s\n",
         name.c_str(), cBody.c_str());
#endif
   objectTypeNames.push_back("Planet");
   parameterCount      = PlanetParamCount;
   
   theCentralBodyName  = cBody;
   bodyType            = Gmat::PLANET;
   bodyNumber          = 1;
   referenceBodyNumber = 3;
   rotationSrc         = Gmat::IAU_SIMPLIFIED;
   if (name == GmatSolarSystemDefaults::EARTH_NAME) 
      rotationSrc      = Gmat::FK5_IAU_1980;
   else if (name == GmatSolarSystemDefaults::NEPTUNE_NAME)
      rotationSrc      = Gmat::IAU_2002;
   
   if (name == GmatSolarSystemDefaults::EARTH_NAME)
   {
      #ifdef DEBUG_PLANET_CONSTRUCT
         MessageInterface::ShowMessage("In Planet constructor, setting default PCKs.\n");
      #endif
      FileManager *fm  = FileManager::Instance();
      std::string path = fm->GetFullPathname(FileManager::PLANETARY_COEFF_PATH);
      std::string earthLatest  = fm->GetFilename(FileManager::EARTH_LATEST_PCK_FILE);
      std::string earthPredict = fm->GetFilename(FileManager::EARTH_PCK_PREDICTED_FILE);
      std::string earthCurrent = fm->GetFilename(FileManager::EARTH_PCK_CURRENT_FILE);
      attitudeSpiceKernelNames.push_back(path+earthPredict);
      attitudeSpiceKernelNames.push_back(path+earthCurrent);
      attitudeSpiceKernelNames.push_back(path+earthLatest);
//      attitudeSpiceKernelNames.push_back(path+"earth_070425_370426_predict.bpc");
//      attitudeSpiceKernelNames.push_back(path+"earth_720101_070426.bpc");
//      attitudeSpiceKernelNames.push_back(path+"earth_latest_high_prec.bpc");
   }

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Planet(const Planet &pl)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class as a copy of the
 * specified Planet class (copy constructor).
 *
 * @param <pl> Planet object to copy.
 */
//------------------------------------------------------------------------------
Planet::Planet(const Planet &pl) :
   CelestialBody  (pl),
   nutationUpdateInterval         (pl.nutationUpdateInterval),
   eopFileName                    (pl.eopFileName),
   default_nutationUpdateInterval (pl.default_nutationUpdateInterval)
{
}

//------------------------------------------------------------------------------
//  Planet& operator= (const Planet& pl)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Planet class.
 *
 * @param <pl> the Planet object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Planet with data of input Planet pl.
 */
//------------------------------------------------------------------------------
Planet& Planet::operator=(const Planet &pl)
{
   if (&pl == this)
      return *this;

   CelestialBody::operator=(pl);
   nutationUpdateInterval          = pl.nutationUpdateInterval;
   default_nutationUpdateInterval  = pl.default_nutationUpdateInterval;
   
   eopFileName                     = pl.eopFileName;
   
   return *this;
}

//------------------------------------------------------------------------------
//  ~Planet()
//------------------------------------------------------------------------------
/**
 * Destructor for the Planet class.
 */
//------------------------------------------------------------------------------
Planet::~Planet()
{
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the CelestialBody class.
 *
 * @return  success flag
 */
//------------------------------------------------------------------------------
bool Planet::Initialize()
{
   #ifdef DEBUG_PLANET_INIT
   MessageInterface::ShowMessage
      ("Planet::Initialize() this=<%p> %10s\n",
       this, GetName().c_str());
   #endif
   if (eopFileName != "")
      GmatGlobal::Instance()->GetEopFile()->ResetEopFile(eopFileName);
   return CelestialBody::Initialize();
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
 * @note currently only implemented for the (currently known) planets of our
 *       Solar System.  See "Report of the IAU/IAG Working Group on
 *       Cartographic Coordinates and Rotational Elements of the Planets
 *       and Satellites: 2000"
 */
//------------------------------------------------------------------------------
Rvector Planet::GetBodyCartographicCoordinates(const A1Mjd &forTime) const
{
   // Neptune is the special case for the planets
   if (instanceName == GmatSolarSystemDefaults::NEPTUNE_NAME)
   {
      if (rotationSrc == Gmat::IAU_2002)
      {
      Real d = GetJulianDaysFromTDBEpoch(forTime); // interval in Julian days
      Real T = d / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;  // interval in Julian centuries
      Real N    = 357.85 + 52.316 * T;
      //Real NDot = 52.316 * CelestialBody::TDot;
	  //NDot = 52.316 (deg/century) * (1 century/36525 days)
      Real NDot      = 0.0014323;   // fixed conversion error from spec, (SPH) 02 Nov. 2012
      Real alpha     = orientation[0] + orientation[1] * Sin(Rad(N));
      Real delta     = orientation[2] + orientation[3] * Cos(Rad(N));
      Real W         = orientation[4] + orientation[5] * d - 0.48 * Sin(Rad(N));
      Real Wdot      = orientation[5] * CelestialBody::dDot - 0.48 * NDot * Cos(Rad(N));
      return Rvector(4, alpha, delta, W, Wdot);
      }
      else if (rotationSrc == Gmat::IAU_SIMPLIFIED)
      {
         return CelestialBody::GetBodyCartographicCoordinates(forTime);
      }
      else
      {
         std::string errmsg = "Error computing cartographic coordinates for Neptune - ";
         errmsg += "unknown or invalid rotation data source\n";
         throw SolarSystemException(errmsg);
      }
   }
   else if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      // TBD - actually, the FK5 stuff is handled in the AxesSystem class(es) for 
      // which it is appropriate (e.g. BodyFixedAxes)
      return CelestialBody::GetBodyCartographicCoordinates(forTime);
   }

   // by default, call the method that handles the IAU_SIMPLIFIED method
   return CelestialBody::GetBodyCartographicCoordinates(forTime);
}

//------------------------------------------------------------------------------
//  Real GetHourAngle(A1Mjd atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the hour angle for the body, referenced from the
 * Prime Meridian, measured westward
 *
 * @param <atTime> time for which to compute the hour angle
 *
 * @return hour angle for the body, in degrees, from the Prime Meridian
 *
 * @note algorithm 15, Vallado p. 192
 */
//------------------------------------------------------------------------------
Real Planet::GetHourAngle(A1Mjd atTime)
{
   if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      // Convert the time to a UT1 MJD
      // 20.02.06 - arg: changed to use enum types instead of strings
      Real mjdUT1 = theTimeConverter->Convert(atTime.Get(),
                                 TimeSystemConverter::A1MJD, TimeSystemConverter::UT1MJD,
                                 GmatTimeConstants::JD_JAN_5_1941);
      Real jdUT1    = mjdUT1 + GmatTimeConstants::JD_JAN_5_1941; // right?
                                                            // Compute elapsed Julian centuries (UT1)
      Real tUT1     = (jdUT1 - GmatTimeConstants::JD_OF_J2000) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
      
      // compute mean sidereal time, in degrees
      // according to Vallado Eq. 3-45, converted to degrees, where
      // 1 hour = 15 degrees and 1 second = 1/240 of a second
      
      Real mst        = (67310.54841 / 240) +
         (((876600 * 15) + (8640184.812866 / 240)) * tUT1) +
         ((0.093104 / 240) * tUT1 * tUT1) -
         ((6.2e-06 / 240) * tUT1 * tUT1 * tUT1);
      
      // reduce to a quantity within one day (GmatTimeConstants::SECS_PER_DAY seconds, 360.0 degrees)
      hourAngle = AngleUtil::PutAngleInDegRange(mst,0.0,360.0);
      return hourAngle;
   }
   return CelestialBody::GetHourAngle(atTime);
}


//------------------------------------------------------------------------------
//  bool SetTwoBodyEpoch(const A1Mjd &toTime)
//------------------------------------------------------------------------------
/**
 * This method sets the epoch to be used for Low Fidelity analytic modeling.
 *
 * @param <toTime> epoch
 *
 * @return success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetTwoBodyEpoch(const A1Mjd &toTime)
{
   #ifdef DEBUG_PLANET_TWO_BODY
      MessageInterface::ShowMessage(
      "In Planet::SetTwoBodyEpoch with time = %.12f\n", toTime.Get());
   #endif
   if (!CelestialBody::SetTwoBodyEpoch(toTime)) return false;
   bool OK = true;
   // For the Earth, send the information to the Sun
   if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      if (!theCentralBody) 
         throw SolarSystemException("Central body must be set for " 
                                    + instanceName);
      #ifdef DEBUG_PLANET_TWO_BODY
         MessageInterface::ShowMessage(
         "-------- and setting central body's epoch time to %.12f\n", 
         toTime.Get());
      #endif
      OK = theCentralBody->SetTwoBodyEpoch(toTime);
   }
   return OK;
}

//------------------------------------------------------------------------------
//  bool SetTwoBodyElements(const Rvector6 &kepl)
//------------------------------------------------------------------------------
/**
 * This method sets the elements to be used for Low Fidelity analytic modeling.
 *
 * @param <kepl> initial keplerian elements.
 *
 * @return success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetTwoBodyElements(const Rvector6 &kepl)
{
   #ifdef DEBUG_PLANET_TWO_BODY
      MessageInterface::ShowMessage(
      "In Planet::SetTwoBodyElements, kepl = \n%.12f %.12f %.12f %.12f %.12f %.12f\n", 
      kepl[0], kepl[1], kepl[2], kepl[3], kepl[4], kepl[5]);
   #endif
   if (!CelestialBody::SetTwoBodyElements(kepl)) return false;
   bool OK = true;
   // For the Earth, send the information to the Sun
   if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      if (!theCentralBody) 
         throw SolarSystemException("Central body must be set for " 
                                    + instanceName);
      Real     ma;
      Real     totalMu = mu + theCentralBody->GetGravitationalConstant();
      Rvector6 cart    = - (StateConversionUtil::KeplerianToCartesian(totalMu, kepl, "TA"));
      Rvector6 sunKepl = StateConversionUtil::CartesianToKeplerian(totalMu, cart, &ma);

      #ifdef DEBUG_PLANET_TWO_BODY
         MessageInterface::ShowMessage(
         "-------- and setting central body's elements to \n %.12f %.12f %.12f %.12f %.12f %.12f\n", 
         sunKepl[0], sunKepl[1], sunKepl[2], 
         sunKepl[3], sunKepl[4], sunKepl[5]);
      #endif
      OK = theCentralBody->SetTwoBodyElements(sunKepl);
   }
   return OK;
}

//------------------------------------------------------------------------------
//  Real GetNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * This method returns the nutation update interval.
 *
 * @return nutation update interval (seconds)
 *
 */
//------------------------------------------------------------------------------
Real Planet::GetNutationUpdateInterval() const 
{
   return nutationUpdateInterval;
}

//------------------------------------------------------------------------------
//  bool SetNutationUpdateInterval(Real val)
//------------------------------------------------------------------------------
/**
 * This method sets the nutation update interval.
 *
 * @param <val> nutation update interval vlaue to use (seconds)
 *
 * @return success flag for the operation
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetNutationUpdateInterval(Real val)
{
   #ifdef DEBUG_PLANET_NUTATION_INTERVAL
      MessageInterface::ShowMessage("Setting nutation interval for body %s to %12.10f\n",
            instanceName.c_str(), val);
   #endif
   if (val < 0.0)
   {
      SolarSystemException sse;
      sse.SetDetails(errorMessageFormat.c_str(),
                     GmatStringUtil::ToString(val, GetDataPrecision()).c_str(),
                     "NutationUpdateInterval", "Real Number >= 0.0");
      throw sse;
   }
   
   nutationUpdateInterval = val;
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Planet.
 *
 * @return clone of the Planet.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Planet::Clone() const
{
   return (new Planet(*this));
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
void Planet::Copy(const GmatBase* orig)
{
   operator=(*((Planet *)(orig)));
}

//---------------------------------------------------------------------------
//  bool NeedsOnlyMainSPK()
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the default SPK file contains
 * sufficient data for this Planet.
 *
 * @return flag indicating whether or not an additional SPK file is needed
 *         for this Planet; true, if only the default one is needed; false
 *         if an additional file is needed.
 */
//---------------------------------------------------------------------------
bool Planet::NeedsOnlyMainSPK()
{
   // If the planet data is included in our "built-in" SPK file,
   // we don't need another one
   if ((instanceName == GmatSolarSystemDefaults::MERCURY_NAME) ||
       (instanceName == GmatSolarSystemDefaults::VENUS_NAME)   ||
       (instanceName == GmatSolarSystemDefaults::EARTH_NAME)   ||
       (instanceName == GmatSolarSystemDefaults::MARS_NAME)    ||
       (instanceName == GmatSolarSystemDefaults::JUPITER_NAME) ||
       (instanceName == GmatSolarSystemDefaults::SATURN_NAME)  ||
       (instanceName == GmatSolarSystemDefaults::URANUS_NAME)  ||
       (instanceName == GmatSolarSystemDefaults::NEPTUNE_NAME) ||
       (instanceName == GmatSolarSystemDefaults::PLUTO_NAME)      )
              return true;
   // Otherwise, we need an additional SPK file
   return false;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Planet::GetParameterText(const Integer id) const
{
   if (id >= CelestialBodyParamCount && id < PlanetParamCount)
      return PARAMETER_TEXT[id - CelestialBodyParamCount];
   return CelestialBody::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Planet::GetParameterID(const std::string &str) const
{
   for (Integer i = CelestialBodyParamCount; i < PlanetParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CelestialBodyParamCount])
         return i;
   }
   
   return CelestialBody::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Planet::GetParameterType(const Integer id) const
{
   if (id >= CelestialBodyParamCount && id < PlanetParamCount)
      return PARAMETER_TYPE[id - CelestialBodyParamCount];
   
   return CelestialBody::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Planet::GetParameterTypeString(const Integer id) const
{
   return CelestialBody::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified parameter is
 * read-only.
 *
 * @param <id> Id of the parameter
 *
 * @return flag indicating whether or not the specified parameter is
 *         read-only.
 */
//---------------------------------------------------------------------------
bool Planet::IsParameterReadOnly(const Integer id) const
{
   if (id == NUTATION_UPDATE_INTERVAL)
   {
      if (instanceName == GmatSolarSystemDefaults::EARTH_NAME) return false;
      else                                         return true;
   }
   if (id == EOP_FILE_NAME)
   {
      if (instanceName == GmatSolarSystemDefaults::EARTH_NAME) return false;
      else                                         return true;
   }
   return CelestialBody::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::GetRealParameter(const Integer id) const
{
   if (id == NUTATION_UPDATE_INTERVAL) return nutationUpdateInterval;
   return CelestialBody::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::SetRealParameter(const Integer id, const Real value)
{
   if (id == NUTATION_UPDATE_INTERVAL)
   {
      if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
      {
         SetNutationUpdateInterval(value);
         return nutationUpdateInterval;
      }
   }
   return CelestialBody::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Planet::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_GET_STRING
   MessageInterface::ShowMessage
      ("Planet::GetStringParameter() '%s' entered, id = %d\n",
       GetName().c_str(), id);
   #endif
   
   if (id == EOP_FILE_NAME)         return eopFileName;
   
   return CelestialBody::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @exception <SolarSystemException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetStringParameter(const Integer id,
                                const std::string &value)
{
   #ifdef DEBUG_PLANET_SET_STRING
   std::string idString = GetParameterText(id);
   MessageInterface::ShowMessage
      ("Planet::SetStringP:: id = %d (%s), value = %s\n",
       id, idString.c_str(), value.c_str());
   #endif

   if (id == EOP_FILE_NAME)
   {
      if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
      {
         eopFileName = value;
         if (eopFileName != "")
            GmatGlobal::Instance()->GetEopFile()->ResetEopFile(eopFileName);
         return true;
      }
   }
   
   return CelestialBody::SetStringParameter(id, value);
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
Integer Planet::SetIntegerParameter(const Integer id,
                                    const Integer value)
{
   if (instanceName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      if ((id == NAIF_ID) &&
          (value != GmatSolarSystemDefaults::PLANET_NAIF_IDS[2]))
      {
         std::string errmsg = "NAIF ID for body Earth may not be modified.\n";
         throw SolarSystemException(errmsg);
      }
   }
   return CelestialBody::SetIntegerParameter(id,value);

}



//---------------------------------------------------------------------------
//  bool IsParameterCloaked(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified parameter is
 * cloaked, i.e. not written out unless the value is changed by the user.
 *
 * @param <id> Id of the parameter
 *
 * @return flag indicating whether or not the specified parameter is
 *         cloaked.
 */
//---------------------------------------------------------------------------
bool Planet::IsParameterCloaked(const Integer id) const
{
   if (!cloaking) return false;
   // if it's read-only, we'll cloak it
   if (IsParameterReadOnly(id)) return true;

   if (id >= CelestialBodyParamCount && id < PlanetParamCount)
      return IsParameterEqualToDefault(id);
   
   return CelestialBody::IsParameterCloaked(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterEqualToDefault(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the current value of the
 * specified parameter is equal to its default value.
 *
 * @param <id> Id of the parameter
 *
 * @return flag indicating whether or not the current value of the
 * specified parameter is equal to its default value.
 */
//---------------------------------------------------------------------------
bool Planet::IsParameterEqualToDefault(const Integer id) const
{
   if (id == NUTATION_UPDATE_INTERVAL)
   {
      if (default_nutationUpdateInterval == nutationUpdateInterval)     return true;
      else                                                              return false;
   }
   if (id == EOP_FILE_NAME)
   {
      if (eopFileName == "")  return true;
      else                   return false;
   }

   return CelestialBody::IsParameterEqualToDefault(id);
}

//---------------------------------------------------------------------------
//  bool SaveAllAsDefault()
//---------------------------------------------------------------------------
/**
 * Saves all current values to be the default values.
 *
 * @return success flag for the operation
 */
//---------------------------------------------------------------------------
bool Planet::SaveAllAsDefault()
{
   CelestialBody::SaveAllAsDefault();
   default_nutationUpdateInterval = nutationUpdateInterval;
   return true;
}

//---------------------------------------------------------------------------
//  bool SaveParameterAsDefault(const Integer id)
//---------------------------------------------------------------------------
/**
 * Saves the current value of the specified parameter to be the default value.
 *
 * @return success flag for the operation
 */
//---------------------------------------------------------------------------
bool Planet::SaveParameterAsDefault(const Integer id)
{
   if (id == NUTATION_UPDATE_INTERVAL)  
   {
      default_nutationUpdateInterval = nutationUpdateInterval;
      return true;
   }
   return CelestialBody::SaveParameterAsDefault(id);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
