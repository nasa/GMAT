//$Id$
//------------------------------------------------------------------------------
//                         StateConversionUtil
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy Shoan, GSFC/GSSB
// Created: 2011.11.02
//
/**
 * Implementation of the namespace containing methods to convert between
 * orbit state representations.
 *
 */
//------------------------------------------------------------------------------

#include <sstream>                 // for <<
#include "utildefs.hpp"
#include "StateConversionUtil.hpp"
#include "GmatDefaults.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"

//#define DEBUG_EQUINOCTIAL
//#define DEBUG_MODEQUINOCTIAL
//#define DEBUG_STATE_CONVERSION
//#define DEBUG_STATE_CONVERSION_SQRT
//#define DEBUG_KEPLERIAN
//#define DEBUG_KEPLERIAN_TA
//#define DEBUG_KEPLERIAN_EA
//#define DEBUG_KEPLERIAN_HA
//#define DEBUG_KEPLERIAN_SMA
//#define DEBUG_KEPLERIAN_ECC
//#define DEBUG_KEPLERIAN_INC
//#define DEBUG_KEPLERIAN_RAAN
//#define DEBUG_KEPLERIAN_AOP
//#define DEBUG_KEPLERIAN_AM
//#define DEBUG_KEPLERIAN_ECC_VEC
//#define DEBUG_CART_TO_KEPL
//#define DEBUG_MOD_KEPLERIAN
//#define DEBUG_ANOMALY
//#define DEBUG_ANOMALY_MA
//#define DEBUG_MODKEP_TO_KEP
//#define DEBUG_CONVERT_ERRORS
//#define DEBUG_SC_CONVERT_VALIDATE
//#define DEBUG_BROUWER_SHORT
//#define DEBUG_BROUWER_LONG

using namespace GmatMathUtil;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real         StateConversionUtil::ORBIT_TOL          = 1.0E-10;
const Real         StateConversionUtil::ORBIT_TOL_SQ       = 1.0E-20;
const Real         StateConversionUtil::SINGULAR_TOL       = .001;
const Real         StateConversionUtil::INFINITE_TOL       = 1.0E-30;
const Real         StateConversionUtil::PARABOLIC_TOL      = 1.0E-7;
const Real         StateConversionUtil::MU_TOL             = 1.0E-15;
const Real         StateConversionUtil::EQUINOCTIAL_TOL    = 1.0E-5;
const Real         StateConversionUtil::ANGLE_TOL          = 0.0;

const Integer      StateConversionUtil::MAX_ITERATIONS     = 75;

const Real         StateConversionUtil::DEFAULT_MU         =
                   GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];

const std::string  StateConversionUtil::STATE_TYPE_TEXT[StateTypeCount] =
{
   "Cartesian",
   "Keplerian",
   "ModifiedKeplerian",
   "SphericalAZFPA",
   "SphericalRADEC",
   "Equinoctial",
   "ModifiedEquinoctial",  // Modified by M.H.
   "AlternateEquinoctial", // Alternate Equinoctial by HYKim
   "Delaunay",
   "Planetodetic",
   "OutgoingAsymptote",
   "IncomingAsymptote",
   "BrouwerMeanShort",
   "BrouwerMeanLong",
};

const bool         StateConversionUtil::REQUIRES_CB_ORIGIN[StateTypeCount] =
{
   false, // "Cartesian"
   true,  // "Keplerian"
   true,  // "ModifiedKeplerian"
   false, // "SphericalAZFPA"
   false, // "SphericalRADEC"
   true,  // "Equinoctial"
   true,  // "ModifiedEquinoctial"
   false, // "AlternateEquinoctial"
   true,  // "Delaunay"
   true,  // "Planetodetic"
   true,  // "OutgoingAsymptote"
   true,  // "IncomingAsymptote"
   true,  // "BrouwerMeanShort"
   true,  // "BrouwerMeanLong"
};

const bool         StateConversionUtil::REQUIRES_FIXED_CS[StateTypeCount] =
{
   false, // "Cartesian"
   false, // "Keplerian"
   false, // "ModifiedKeplerian"
   false, // "SphericalAZFPA"
   false, // "SphericalRADEC"
   false, // "Equinoctial"
   false, // "ModifiedEquinoctial"
   false, // "AlternateEquinoctial"
   false, // "Delaunay"
   true,  // "Planetodetic"
   false, // "OutgoingAsymptote"
   false, // "IncomingAsymptote"
   false, // "BrouwerMeanShort"
   false, // "BrouwerMeanLong"
};


const std::string StateConversionUtil::ANOMALY_LONG_TEXT[AnomalyTypeCount] =
{
   "True Anomaly", "Mean Anomaly", "Eccentric Anomaly", "Hyperbolic Anomaly",
};

const std::string StateConversionUtil::ANOMALY_SHORT_TEXT[AnomalyTypeCount] =
{
   "TA", "MA", "EA", "HA",
};


bool StateConversionUtil::apsidesForIncomingAsymptoteWritten  = false;
bool StateConversionUtil::apsidesForOutgoingAsymptoteWritten  = false;
bool StateConversionUtil::brouwerNotConvergingShortWritten    = false;
bool StateConversionUtil::brouwerNotConvergingLongWritten     = false;
bool StateConversionUtil::brouwerSmallEccentricityWritten     = false;
bool StateConversionUtil::criticalInclinationWritten          = false;
bool StateConversionUtil::possibleInaccuracyShortWritten      = false;
bool StateConversionUtil::possibleInaccuracyLongWritten       = false;
bool StateConversionUtil::inaccuracyCriticalAngleWritten      = false;



//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Rvector6 Convert(Real mu,
//                   Real *state,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   Real mu         = GmatSolarSystemDefaults::PLANET_MU[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[
//                                     GmatSolarSystemDefaults::EARTH],
//                   const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
/**
 * Converts from fromType to toType.
 *
 * @param <state>       state to convert
 * @param <fromType>    state type to convert from
 * @param <toType>      state type to convert to
 * @param <mu>          gravitational constant for the central body
 * @param <flattening>  flattening coefficient for the central body
 * @param <eqRadius>    equatorial radius for the central body
 * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
 *
 * @return Converted states from the specific element type
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::Convert(const Real *state,
                              const std::string &fromType, const std::string &toType,
                              Real mu,
                              Real flattening,
                              Real eqRadius,
                              const std::string &anomalyType)
{
   Rvector6 newState;
   newState.Set(state[0], state[1], state[2], state[3], state[4], state[5]);

   if (fromType == toType)
      return newState;

   return Convert(newState, fromType, toType, mu, flattening, eqRadius, anomalyType);
}


//---------------------------------------------------------------------------
//  Rvector6 Convert(const Rvector6 &state,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   const std::string &fromType,
//                   const std::string &toType,
//                   Real mu         = GmatSolarSystemDefaults::PLANET_MU[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[
//                                     GmatSolarSystemDefaults::EARTH],
//                   Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[
//                                     GmatSolarSystemDefaults::EARTH],
//                   const std::string &anomalyType = "TA")
//                   const std::string &anomalyType = "TA")
//---------------------------------------------------------------------------
/**
 * Converts state from fromType to toType.
 *
 * @param <state>       state to convert
 * @param <fromType>    state type to convert from
 * @param <toType>      state type to convert to
 * @param <mu>          gravitational constant for the central body
 * @param <flattening>  flattening coefficient for the central body
 * @param <eqRadius>    equatorial radius for the central body
 * @param <anomalyType> anomaly type string if toType is Mod/Keplerian
 *
 * @return Converted states from the specific element type
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::Convert(const Rvector6 &state,
                              const std::string &fromType, const std::string &toType,
                              Real mu,
                              Real flattening,
                              Real eqRadius,
                              const std::string &anomalyType)
{
   #ifdef DEBUG_STATE_CONVERSION
   MessageInterface::ShowMessage
      ("StateConversionUtil::Convert() fromType=%s, toType=%s, anomalyType=%s\n"
       "   state=%s\n", fromType.c_str(), toType.c_str(), anomalyType.c_str(),
       state.ToString(13).c_str());
   #endif

   if (fromType == toType)
      return state;
   
   Rvector6 outState;
   
   try
   {
      // Determine the input state type
      if (fromType == "Cartesian")
         outState = ConvertFromCartesian(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "Keplerian")
         outState = ConvertFromKeplerian(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "ModifiedKeplerian")
         outState = ConvertFromModKeplerian(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "SphericalAZFPA")
         outState = ConvertFromSphericalAZFPA(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "SphericalRADEC")
         outState = ConvertFromSphericalRADEC(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "Equinoctial")
         outState = ConvertFromEquinoctial(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "ModifiedEquinoctial")
         outState = ConvertFromModEquinoctial(toType, state, mu, anomalyType, flattening, eqRadius);
	   else if (fromType == "AlternateEquinoctial")
         outState = ConvertFromAltEquinoctial(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "Delaunay")
         outState = ConvertFromDelaunay(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "Planetodetic")
         outState = ConvertFromPlanetodetic(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "OutgoingAsymptote")
         outState = ConvertFromOutgoingAsymptote(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "IncomingAsymptote")
         outState = ConvertFromIncomingAsymptote(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "BrouwerMeanShort")
         outState = ConvertFromBrouwerMeanShort(toType, state, mu, anomalyType, flattening, eqRadius);
      else if (fromType == "BrouwerMeanLong")
         outState = ConvertFromBrouwerMeanLong(toType, state, mu, anomalyType, flattening, eqRadius);
      else
      {
         throw UtilityException
            ("StateConversionUtil::Convert() Cannot convert the state \"" +
             fromType + "\" to \"" + toType + "\". \"" + fromType +
             " is an unknown State Type\n");
      }
   }
   catch(UtilityException &ue)
   {
      throw ue;
   }
   
   #ifdef DEBUG_STATE_CONVERSION
   MessageInterface::ShowMessage
      ("StateConversionUtil::Convert() returning \n   %s\n", outState.ToString(13).c_str());
   #endif
   
   return outState;
} // Convert()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromCartesian(const std::string &toType, const Rvector6 &state,
//                              Real mu, const std::string &anomalyType,
//                              Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromCartesian(const std::string &toType, const Rvector6 &state, 
                                                   Real mu, const std::string &anomalyType,
                                                   Real flattening, Real eqRadius)
{
   Rvector6 outState;
   
   if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState = KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState = CartesianToSphericalAZFPA(state);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(state);
   }
   else if (toType == "Equinoctial")
   {
      outState = CartesianToEquinoctial(state, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(state, mu);
   }
   else if (toType == "AlternateEquinoctial") // Alternate Equinoctial by HYKim 
   {
      Rvector6 equinoctial = CartesianToEquinoctial(state, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage("   CartesianToKeplerian = %s", kepl.ToString().c_str());
      #endif
      outState = KeplerianToDelaunay(kepl, mu); 
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(state, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, state);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, state);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      outState = CartesianToBrouwerMeanShort(mu, state);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      outState = CartesianToBrouwerMeanLong(mu, state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Cartesian\" to \"" + toType +
          "\". \"" + toType + "\" is an unknown State Type\n");
   }
   
   return outState;
   
} // ConvertFromCartesian()

//------------------------------------------------------------------------------
//Rvector6 ConvertFromKeplerian(const std::string &toType, const Rvector6 &state,
//                              Real mu, const std::string &anomalyType,
//                              Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromKeplerian(const std::string &toType, const Rvector6 &state,
                                                   Real mu, const std::string &anomalyType,
                                                   Real flattening, Real eqRadius)
{
   Rvector6 outState;
   
   if (toType == "Cartesian")
   {
      outState = KeplerianToCartesian(mu, state, anomalyType);
   }
   else if (toType == "ModifiedKeplerian")
   {
      outState = KeplerianToModKeplerian(state);
   }
   else if (toType == "SphericalAZFPA")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToSphericalAZFPA(cartesian);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToSphericalRADEC(cartesian);
   }
   else if (toType == "Equinoctial")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToEquinoctial(cartesian, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToModEquinoctial(cartesian, mu);
   }
   else if (toType == "AlternateEquinoctial")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      Rvector6 equinoctial = CartesianToEquinoctial(cartesian, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if ( toType == "Delaunay" ) // Modified by M.H.
   {
      outState = KeplerianToDelaunay(state, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToPlanetodetic(cartesian, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToOutgoingAsymptote(mu, cartesian);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToIncomingAsymptote(mu, cartesian);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanShort(mu, cartesian);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanLong(mu, cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Keperian\" to \"" + toType +
          "\". \"" + toType + " is an unknown State Type\n");
   }
   
   return outState;
} // ConvertFromKeplerian()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromModKeplerian(const std::string &toType, const Rvector6 &state, 
//                                 Real mu, const std::string &anomalyType,
//                                 Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromModKeplerian(const std::string &toType,const Rvector6 &state,
                                                      Real mu, const std::string &anomalyType,
                                                      Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 keplerian = ModKeplerianToKeplerian(state);
   
   if (toType == "Cartesian")
   {
      outState = KeplerianToCartesian(mu, keplerian, anomalyType);
   }
   else if (toType == "Keplerian")
   {
      outState = keplerian;
   }
   else if (toType == "SphericalAZFPA")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
      outState = CartesianToSphericalAZFPA(cartesian);
   }
   else if (toType == "SphericalRADEC")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
      outState = CartesianToSphericalRADEC(cartesian);
   }
   else if (toType == "Equinoctial")
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
      outState = CartesianToEquinoctial(cartesian, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
      outState = CartesianToModEquinoctial(cartesian, mu);
   }
   else if (toType == "AlternateEquinoctial") // Alternate Equinoctial by HYKim 
   {
      Rvector6 cartesian   = KeplerianToCartesian(mu, state, anomalyType);
      Rvector6 equinoctial = CartesianToEquinoctial(cartesian, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      outState = KeplerianToDelaunay(keplerian, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, keplerian, anomalyType);
      outState = CartesianToPlanetodetic(cartesian, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToOutgoingAsymptote(mu, cartesian);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToIncomingAsymptote(mu, cartesian);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanShort(mu, cartesian);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanLong(mu, cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"ModKeplerian\" to \"" + toType +
          "\". \"" + toType + " is an unknown State Type\n");
   }
   
   return outState;
} // ConvertFromModKeperian()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromSphericalAZFPA(const std::string &toType, const Rvector6 &state, 
//                                   Real mu, const std::string &anomalyType,
//                                   Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromSphericalAZFPA(const std::string &toType, const Rvector6 &state,
                                                        Real mu, const std::string &anomalyType,
                                                        Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartesian = SphericalAZFPAToCartesian(state);
   
   if (toType == "Cartesian")
   {
      outState = cartesian;
   }
   else if (toType == "Keplerian")
   {
      outState = CartesianToKeplerian(mu, cartesian, anomalyType);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
      outState = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartesian);
   }
   else if (toType == "Equinoctial")
   {
      outState = CartesianToEquinoctial(cartesian, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(cartesian,mu);
   }
   else if (toType == "AlternateEquinoctial") // Alternate Equinoctial by HYKim 
   {
      Rvector6 equinoctial = CartesianToEquinoctial(cartesian, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
      outState = KeplerianToDelaunay(keplerian,mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cartesian, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToOutgoingAsymptote(mu, cartesian);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToIncomingAsymptote(mu, cartesian);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanShort(mu, cartesian);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      Rvector6 cartesian = KeplerianToCartesian(mu, state, anomalyType);
      outState = CartesianToBrouwerMeanLong(mu, cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalAZFPA\" to \"" + toType +
          "\". \"" + toType + " is an unknown State Type\n");
   }
   return outState;
} // ConvertFromSphericalAZFPA()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromSphericalRADEC(const std::string &toType, const Rvector6 &state, 
//                                   Real mu, const std::string &anomalyType,
//                                   Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromSphericalRADEC(const std::string &toType, const Rvector6 &state,
                                                        Real mu, const std::string &anomalyType,
                                                        Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartesian = SphericalRADECToCartesian(state);
   
   if (toType == "Cartesian")
   {
      outState = cartesian;
   }
   else if (toType == "Keplerian")
   {
      outState = CartesianToKeplerian(mu, cartesian, anomalyType);
   }
   else if (toType == "ModifiedKeplerian")
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
      outState = KeplerianToModKeplerian(keplerian);
   }
   else if (toType == "SphericalAZFPA")
   {
      outState = CartesianToSphericalAZFPA(cartesian);
   }
   else if (toType == "Equinoctial")
   {
      outState = CartesianToEquinoctial(cartesian, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(cartesian,mu);
   }
   else if (toType == "AlternateEquinoctial") // Alternate Equinoctial by HYKim 
   {
      Rvector6 equinoctial = CartesianToEquinoctial(cartesian, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartesian, anomalyType);
      outState = KeplerianToDelaunay(keplerian, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cartesian, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, cartesian);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, cartesian);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      outState = CartesianToBrouwerMeanShort(mu, cartesian);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      outState = CartesianToBrouwerMeanLong(mu, cartesian);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"SphericalRADEC\" to \"" + toType +
          "\". \"" + toType + " is an unknown State Type\n");
   }
   
   return outState;
} // ConvertFromSphericalRADEC()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromEquinoctial(const std::string &toType, const Rvector6 &state, 
//                                Real mu, const std::string &anomalyType,
//                                Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromEquinoctial(const std::string &toType, const Rvector6 &state,
                                                     Real mu, const std::string &anomalyType,
                                                     Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = EquinoctialToCartesian(state, mu);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(cartState, mu);
   }
   else if (toType == "AlternateEquinoctial") // Alternate Equinoctial by HYKim
   {
      outState = EquinoctialToAltEquinoctial(state);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartState, anomalyType);
      outState = KeplerianToDelaunay(keplerian, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cartState, flattening, eqRadius);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, state);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, state);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      outState = CartesianToBrouwerMeanShort(mu, state);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      outState = CartesianToBrouwerMeanLong(mu, state);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromEquinoctial()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromModEquinoctial(const std::string &toType, const Rvector6 &state, 
//                                   Real mu, const std::string &anomalyType,
//                                   Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromModEquinoctial(const std::string &toType, const Rvector6 &state,
                                                        Real mu, const std::string &anomalyType,
                                                        Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = ModEquinoctialToCartesian(state, mu);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "Equinoctial") 
   {
      outState = CartesianToEquinoctial(cartState, mu);
   }
   else if (toType == "AlternateEquinoctial") 
   {
      Rvector6 equinoctial = CartesianToEquinoctial(state, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartState, anomalyType);
      outState = KeplerianToDelaunay(keplerian, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cartState, flattening, eqRadius);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"ModifiedEquinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromModEquinoctial()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromAltEquinoctial(const std::string &toType, const Rvector6 &state, 
//                                   Real mu, const std::string &anomalyType,
//                                   Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromAltEquinoctial(const std::string &toType, const Rvector6 &state,
                                                        Real mu, const std::string &anomalyType,
                                                        Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 equinoctial = AltEquinoctialToEquinoctial(state);
   Rvector6 cartState = EquinoctialToCartesian(equinoctial, mu);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, cartState, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState = CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "Equinoctial") 
   {
      outState = CartesianToEquinoctial(cartState, mu);
   }
   else if (toType == "ModifiedEquinoctial") 
   {
      outState = CartesianToModEquinoctial(cartState, mu);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 keplerian = CartesianToKeplerian(mu, cartState, anomalyType);
      outState = KeplerianToDelaunay(keplerian, mu);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cartState, flattening, eqRadius);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"AlternateEquinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromAltEquinoctial()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromDelaunay(const std::string &toType, const Rvector6 &state, 
//                             Real mu, const std::string &anomalyType,
//                             Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromDelaunay(const std::string &toType, const Rvector6 &state,
                                                  Real mu, const std::string &anomalyType,
                                                  Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 kepl = DelaunayToKeplerian(state, mu);
   
#ifdef DEBUG_STATE_CONVERSION
   MessageInterface::ShowMessage("   DelaunayToKeplerian = %s", kepl.ToString().c_str());
#endif
   //It should call KeplerianToCartesian() (LOJ: 2013.11.12)
   //Rvector6 cart = CartesianToKeplerian(mu, state, anomalyType);
   Rvector6 cart = KeplerianToCartesian(mu, kepl, anomalyType);
   if (toType == "Cartesian")
   {
      outState = cart;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cart);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cart);
   }
   else if (toType == "Equinoctial") 
   {
      outState = CartesianToEquinoctial(cart, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(cart, mu);
   }
   else if (toType == "AlternateEquinoctial")
   {
      Rvector6 equinoctial = CartesianToEquinoctial(state, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Planetodetic") // Modified by M.H.
   {
      outState = CartesianToPlanetodetic(cart, flattening, eqRadius);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Delaunay\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromDelaunay()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromPlanetodetic(const std::string &toType, const Rvector6 &state, 
//                                 Real mu, const std::string &anomalyType,
//                                 Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromPlanetodetic(const std::string &toType, const Rvector6 &state,
                                                      Real mu, const std::string &anomalyType,
                                                      Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cart = PlanetodeticToCartesian(state, flattening, eqRadius);
   
   if (toType == "Cartesian")
   {
      outState = cart;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, cart, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cart);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cart);
   }
   else if (toType == "Equinoctial") 
   {
      outState = CartesianToEquinoctial(cart, mu);
   }
   else if (toType == "ModifiedEquinoctial") // Modified by M.H.
   {
      outState = CartesianToModEquinoctial(cart, mu);
   }
   else if (toType == "AlternateEquinoctial")
   {
      Rvector6 equinoctial = CartesianToEquinoctial(state, mu);
      outState = EquinoctialToAltEquinoctial(equinoctial);
   }
   else if (toType == "Delaunay") // Modified by M.H.
   {
      Rvector6 kepl = CartesianToKeplerian(mu, cart, anomalyType);
      outState = KeplerianToDelaunay(kepl, mu);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Planetodetic\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromPlanetodetic()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromIncomingAsymptote(const std::string &toType, const Rvector6 &state, 
//                                      Real mu, const std::string &anomalyType,
//                                      Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromIncomingAsymptote(const std::string &toType, const Rvector6 &state,
                                                           Real mu, const std::string &anomalyType,
                                                           Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = IncomingAsymptoteToCartesian(mu, state);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, cartState);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromIncomingAsymptote()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromOutgoingAsymptote(const std::string &toType, const Rvector6 &state, 
//                                      Real mu, const std::string &anomalyType,
//                                      Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromOutgoingAsymptote(const std::string &toType, const Rvector6 &state,
                                                           Real mu, const std::string &anomalyType,
                                                           Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = OutgoingAsymptoteToCartesian(mu, state);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, cartState);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"Equinoctial\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromOutgoingAsymptote()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromBrouwerMeanShort(const std::string &toType, const Rvector6 &state, 
//                                     Real mu, const std::string &anomalyType,
//                                     Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromBrouwerMeanShort(const std::string &toType, const Rvector6 &state,
                                                          Real mu, const std::string &anomalyType,
                                                          Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = BrouwerMeanShortToCartesian(mu, state);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, cartState);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, cartState);
   }
   else if (toType == "BrouwerMeanLong") // YK
   {
      outState = CartesianToBrouwerMeanLong(mu, cartState);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"BrouwerMeanShort\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromBrouwerMeanShort()


//------------------------------------------------------------------------------
//Rvector6 ConvertFromBrouwerMeanLong(const std::string &toType, const Rvector6 &state, 
//                                    Real mu, const std::string &anomalyType,
//                                    Real flattening, Real eqRadius)
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::ConvertFromBrouwerMeanLong(const std::string &toType, const Rvector6 &state,
                                                         Real mu, const std::string &anomalyType,
                                                         Real flattening, Real eqRadius)
{
   Rvector6 outState;
   Rvector6 cartState = BrouwerMeanLongToCartesian(mu, state);
   
   if (toType == "Cartesian")
   {
      outState = cartState;
   }
   else if (toType == "Keplerian" || toType == "ModifiedKeplerian")
   {
      Rvector6 kepl = CartesianToKeplerian(mu, state, anomalyType);
      
      if (toType == "ModifiedKeplerian")
         outState =  KeplerianToModKeplerian(kepl);
      else
         outState = kepl;
   }
   else if (toType == "SphericalAZFPA")
   {
      outState =  CartesianToSphericalAZFPA(cartState);
   }
   else if (toType == "SphericalRADEC")
   {
      outState = CartesianToSphericalRADEC(cartState);
   }
   else if (toType == "OutgoingAsymptote") // YK
   {
      outState = CartesianToOutgoingAsymptote(mu, cartState);
   }
   else if (toType == "IncomingAsymptote") // YK
   {
      outState = CartesianToIncomingAsymptote(mu, cartState);
   }
   else if (toType == "BrouwerMeanShort") // YK
   {
      outState = CartesianToBrouwerMeanShort(mu, cartState);
   }
   else
   {
      throw UtilityException
         ("Cannot convert the state from \"BrouwerMeanLong\" to \"" + toType +
          "\". \"" + toType + " is Unknown State Type\n");
   }
   
   return outState;
} // ConvertFromBrouwerMeanLong()


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                               const Rvector3 &vel,
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel,
                                                   AnomalyType anomalyType)
{
   #ifdef DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("CartesianToKeplerian() ");
   MessageInterface::ShowMessage("                   pos = %s and  vel = %s\n", pos.ToString().c_str(), vel.ToString().c_str());
   #endif

   Real tfp, ma;
   Real p[3], v[3];
   for (unsigned int ii = 0; ii < 3; ii++)
   {
      p[ii] = pos[ii];
      v[ii] = vel[ii];
   }

   Real     kepOut[6];
   kepOut[0] = kepOut[1] = kepOut[2] = kepOut[3] = kepOut[4] = kepOut[5] = 0.0;
   Integer retval = ComputeCartToKepl(mu, p, v, &tfp, kepOut, &ma);                    // It return keplerian state in TA form
   if (retval != 0)
   {
      // only non-zero retval is 2, which did indicate a zero mu; ignore for now, as we did before
   }

   Real anomaly = kepOut[5];
   Real sma = kepOut[0];
   Real ecc = kepOut[1];
   Real ta  = kepOut[5];                // unit: degree

   if (anomalyType != TA)
   {
      anomaly = ConvertFromTrueAnomaly(anomalyType, ta*GmatMathConstants::RAD_PER_DEG, ecc)*GmatMathConstants::DEG_PER_RAD;   // unit: degree
   }
   Rvector6 kep(sma, ecc, kepOut[2], kepOut[3], kepOut[4], anomaly);

   #ifdef DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("returning %s\n", kep.ToString().c_str());
   #endif

   return kep;
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                               const Rvector3 &vel,
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
 Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel,
                                                   const std::string &anomalyType)
{
   AnomalyType type = GetAnomalyType(anomalyType);
   return CartesianToKeplerian(mu, pos, vel, type);
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
 /**
  * Converts from Cartesian to Keplerian.
  *
  * @param <mu>            Gravitational constant for the central body
  * @param <state>         Cartesian state
  * @param <anomalyType>   Anomaly type
  *
  * @return Spacecraft orbit state converted from Cartesian to Keplerian
  */
 //---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                                   AnomalyType anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Cartesian state
 * @param <anomalyType>   Anomaly type
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                                   const std::string &anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state, Real *ma)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Keplerian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Cartesian state
 * @param <ma>            Mean Anomaly
 *
 * @return Spacecraft orbit state converted from Cartesian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToKeplerian(Real mu, const Rvector6 &state, Real *ma)
{
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage("StateConversionUtil::CartesianToKeplerian called ... \n");
   #endif

   Real     kepl[6];
   Real     r[3];
   Real     v[3];
   Real     tfp;
   //Integer  ret;
   Integer  errorCode;

   for (int i=0; i<6; i++)
      kepl[i] = 0.0;

   if(mu < MU_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
      errmsg << " from Keplerian to Cartesian state." << std::endl;
      throw UtilityException(errmsg.str());
   }
   else
   {
      state.GetR(r);
      state.GetV(v);


      if (IsRvValid(r,v))
      {
         errorCode = ComputeCartToKepl(mu, r, v, &tfp, kepl, ma);

         switch (errorCode)
         {
         case 0: // no error
            //ret = 1;
            break;
         case 2:
            throw UtilityException
               ("Gravity constant too small for conversion to Keplerian elements\n");
         default:
            throw UtilityException
               ("Unable to convert Cartesian elements to Keplerian\n");
         }
      }
      else
      {
         std::stringstream ss;
         ss << state;
         throw UtilityException
            ("Invalid Cartesian elements:\n" +
             ss.str());
      }
   }

   Rvector6 keplVec = Rvector6(kepl[0], kepl[1], kepl[2], kepl[3], kepl[4], kepl[5]);
   return keplVec;
}


//------------------------------------------------------------------------------
// Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
//                               AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Cartesian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Keplerian state
 * @param <anomalyType>   Anomaly Type
 *
 * @return Spacecraft orbit state converted from Keplerian to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToCartesian(Real mu, const Rvector6 &state,
                                                   AnomalyType anomalyType)
{
   Integer   ret = 1;
   Real      temp_r[3];
   Real      temp_v[3];
   Real      kepl[6];
   Rvector6  cartVec;

   for (int i=0; i<6; i++)
      kepl[i] = state[i];

   //  These checks test for invalid combination of ECC and SMA. -SPH
   if (kepl[1] < 0.0)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << kepl[1] << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());

      kepl[1] *= -1.0;
   }
   if ((kepl[0] > 0.0) && (kepl[1] > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << kepl[0] << ") cannot be positive if"; 
      errmsg << " eccentricity (" << kepl[1] << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      kepl[0] *= -1.0;
   }
   if ((kepl[0] < 0.0) && (kepl[1] < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << kepl[0] << ") cannot be negative if ";
      errmsg << " eccentricity (" << kepl[1] << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      kepl[0] *= -1.0;
      ret = 1;
   }

   //  These checks test for invalid mu, singular conic sections,
   //  or numerical edge conditions -SPH
   if (ret)
   {
      //  Test that mu is not too small to avoid divide by zero.
      if (mu < MU_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
         errmsg << " from Keplerian to Cartesian state." << std::endl;
         throw UtilityException(errmsg.str());
      }
      else
      {
         // Test that radius of periapsis is not too small.
         Real absA1E = Abs(kepl[0] * (1.0 - kepl[1]));
         if (absA1E < SINGULAR_TOL)
         {
            std::stringstream errmsg("");
            errmsg.precision(16);
            errmsg << "A nearly singular conic section was encountered while converting from"; 
            errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis("; 
            errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
            throw UtilityException(errmsg.str());
         }
         //  Verify that orbit is not too close to a parabola which results in undefined SMA
         Real oneMinusE = Abs(1.0 - kepl[1]);
         if (oneMinusE < PARABOLIC_TOL)
         {
            std::stringstream errmsg("");
            errmsg.precision(16);
            errmsg << "A nearly parabolic orbit";
            errmsg << " (ECC = " << kepl[1] << ") was encountered";
            errmsg << " while converting from the Keplerian elements to"; 
            errmsg << " the Cartesian state.";
            errmsg << " The Keplerian elements are";
            errmsg << " undefined for a parabolic orbit." << std::endl;
            throw UtilityException(errmsg.str());
         }

         ////  Verify that if orbit is hyperbolic, TA is realistic.
         //if (kepl[1] > 1.0)
         //{
         //   #ifdef DEBUG_CONVERT_ERRORS
         //      MessageInterface::ShowMessage("Attempting to test for impossible TA:  ecc = %12.10f\n", kepl[1]);
         //   #endif
         //   Real possible  = PI - ACos(1.0 / kepl[1]);
         //   Real taM       = kepl[5]  * RAD_PER_DEG;
         //   while (taM > PI)   taM -= TWO_PI;
         //   while (taM < -PI)  taM += TWO_PI;
         //   if (Abs(taM) >= possible)
         //   {
         //      possible *= DEG_PER_RAD;
         //      std::stringstream errmsg;
         //      errmsg.precision(12);
         //      errmsg << "\nError: The TA value is not physically possible for a hyperbolic orbit ";
         //      errmsg << "with the input values of SMA and ECC (or RadPer and RadApo).\nThe allowed values are: ";
         //      errmsg << "[" << -possible << " < TA < " << possible << " (degrees)]\nor equivalently: ";
         //      errmsg << "[TA < " << possible << " or TA > " << (360.0 - possible) << " (degrees)]\n";
         //      throw UtilityException(errmsg.str());
         //   }
         //}
         
         ////  Verify that position is not too large for the machine
         //Real infCheck  = 1.0 + kepl[1] * Cos(kepl[5] * RAD_PER_DEG);
         //if (infCheck < INFINITE_TOL)
         //{
         //   std::string errmsg = "A near infinite radius was encountered while converting from ";
         //   errmsg += "the Keplerian elements to the Cartesian state.\n";
         //   throw UtilityException(errmsg);
         //}
        
         // if the return code from a call to compute_kepl_to_cart is greater than zero, there is an error
         Integer errorCode = ComputeKeplToCart(mu, kepl, temp_r, temp_v, anomalyType);
         if (errorCode > 0)
         {
            if (errorCode == 2)
            {
               std::stringstream errmsg("");
               errmsg.precision(16);
               errmsg << "A nearly parabolic orbit (ECC = " << kepl[1] << ") was encountered";
               errmsg << " while converting from the Keplerian elements to the Cartesian state.";
               errmsg << " The Keplerian elements are undefined for a parabolic orbit." << std::endl;
               throw UtilityException(errmsg.str());
            }
            else
            {
               //  Hope we never hit this.  This is a last resort, something went wrong.
               throw UtilityException
               ("Unable to convert Keplerian elements to Cartesian state.\n");
            }
         }
         else
         {
            // build the state vector
            cartVec = Rvector6(temp_r[0], temp_r[1], temp_r[2],
                               temp_v[0], temp_v[1], temp_v[2]);
         }
      }
   }

   return cartVec;
}

//------------------------------------------------------------------------------
// Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
//                               const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Cartesian.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <state>         Keplerian state
 * @param <anomalyType>   Anomaly Type
 *
 * @return Spacecraft orbit state converted from Keplerian to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToCartesian(Real mu, const Rvector6 &state,
                                                   const std::string &anomalyType)
{
   AnomalyType type = GetAnomalyType(anomalyType);
   return KeplerianToCartesian(mu, state, type);
}




//------------------------------------------------------------------------------
// Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to SphericalAZFPA.
 *
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to SphericalAZFPA
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToSphericalAZFPA(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();

   if (rMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalAZFPA: ";
      errmsg << "Spherical elements are undefined because RMAG (" << rMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   if (vMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalAZFPA: ";
      errmsg << "Spherical elements are undefined because VMAG (" << vMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Calculate the vertical flight path angle. rMag = 0 or vMag = 0 is trapped above. 
   Real   psi     = ACos((pos * vel) / (rMag * vMag));

   //Calculate the azimuth angle
   // First, calculate basis (column) vectors of Fl expressed in Fi
   Rvector3 x(Cos(delta) * Cos(lambda),  Cos(delta) * Sin(lambda), Sin(delta));
   Rvector3 y(Cos(lambda + PI_OVER_TWO), Sin(lambda + PI_OVER_TWO), 0.0);
   Rvector3 z(-Sin(delta) * Cos(lambda), -Sin(delta) * Sin(lambda), Cos(delta));
   // Create the transformation matrix from Fi (the frame in which the cartesian state is expressed)
   // to Fl (local frame, where z is a unit vector that points north); Rli is the tranpose of the
   // matrix created by the three column vectors    Rli = [x y z]T
   Rmatrix33 Rli( x[0], x[1], x[2],
                  y[0], y[1], y[2],
                  z[0], z[1], z[2] );

   // Compute the velocity in the local frame
   Rvector3  vLocal = Rli * vel;

   //Compute the flight path azimuth angle
   Real alphaF     = ATan2(vLocal[1], vLocal[2]);

   return Rvector6(rMag, lambda * DEG_PER_RAD, delta  * DEG_PER_RAD,
         vMag, alphaF * DEG_PER_RAD, psi    * DEG_PER_RAD);  // existing code expects this order
}

//------------------------------------------------------------------------------
// Rvector6 SphericalAZFPAToCartesian(const Rvector6& spherical)
//------------------------------------------------------------------------------
/**
 * Converts from SphericalAZFPA to Cartesian.
 *
 * @param <spherical>     SphericalAZFPA state
 *
 * @return Spacecraft orbit state converted from SphericalAZFPA to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::SphericalAZFPAToCartesian(const Rvector6& spherical)
{
   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage("Entering SphericalAZFPAToCartesian: spherical = %s\n",
            spherical.ToString().c_str());
   #endif
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
   Real     alphaF  = spherical[4] * RAD_PER_DEG; // flight path azimuth   *** existing code expects this order
   Real     psi     = spherical[5] * RAD_PER_DEG; // vertical flight path angle

   // Compute the position
   Rvector3 pos(rMag * Cos(delta) * Cos(lambda),
                rMag * Cos(delta) * Sin(lambda),
                rMag * Sin(delta) );

   Real     sinDelta  = Sin(delta);
   Real     cosDelta  = Cos(delta);
   Real     sinLambda = Sin(lambda);
   Real     cosLambda = Cos(lambda);
   Real     sinPsi    = Sin(psi);
   Real     cosPsi    = Cos(psi);
   Real     sinAlphaF = Sin(alphaF);
   Real     cosAlphaF = Cos(alphaF);

   Real vx, vy, vz;

   vx = vMag * ( (cosPsi * cosDelta * cosLambda) -
        sinPsi * ((sinAlphaF * sinLambda) + (cosAlphaF * sinDelta * cosLambda)) );
   vy = vMag * ( (cosPsi * cosDelta * sinLambda) +
        sinPsi * ((sinAlphaF * cosLambda) - (cosAlphaF * sinDelta * sinLambda)) );
   vz = vMag * ( (cosPsi * sinDelta) + (sinPsi * cosAlphaF * cosDelta) );
   Rvector3 vel(vx, vy, vz);

   return Rvector6(pos, vel);
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to SphericalRADEC.
 *
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to SphericalRADEC
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToSphericalRADEC(const Rvector6& cartesian)
{
   // Calculate the magnitude of the position vector, right ascension, and declination
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real    rMag   = pos.GetMagnitude();

   if (rMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalRADEC: ";
      errmsg << "Spherical elements are undefined because RMAG (" << rMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   Real    lambda = ATan2(pos[1], pos[0]);
   Real    delta  = ASin(pos[2] / rMag);

   // Calculate magnitude of the velocity vector
   Real   vMag    = vel.GetMagnitude();

   if (vMag < 1e-10)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Cartesian to SphericalRADEC: ";
      errmsg << "Spherical elements are undefined because VMAG (" << vMag;
      errmsg << ") is less than 1e-10." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Compute right ascension of velocity
   Real   lambdaV = ATan2(vel[1], vel[0]);

   // Compute the declination of velocity
   Real   deltaV  = ASin(vel[2] / vMag);

   return Rvector6(rMag, lambda  * DEG_PER_RAD, delta  * DEG_PER_RAD,
                   vMag, lambdaV * DEG_PER_RAD, deltaV * DEG_PER_RAD);
}

//------------------------------------------------------------------------------
// Rvector6 SphericalRADECToCartesian(const Rvector6& spherical)
//------------------------------------------------------------------------------
/**
 * Converts from SphericalRADEC to Cartesian.
 *
 * @param <spherical>     SphericalRADEC state
 *
 * @return Spacecraft orbit state converted from SphericalRADEC to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::SphericalRADECToCartesian(const Rvector6& spherical)
{
   Real     rMag    = spherical[0]; // magnitude of the position vector
   Real     lambda  = spherical[1] * RAD_PER_DEG; // right ascension
   Real     delta   = spherical[2] * RAD_PER_DEG; // declination
   Real     vMag    = spherical[3]; // magnitude of the velocity vector
   Real     lambdaV = spherical[4] * RAD_PER_DEG; // right ascension of velocity
   Real     deltaV  = spherical[5] * RAD_PER_DEG; // declination of velocity

   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage(
            "Entering SphericalRADECToCartesian with state %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            spherical[0],spherical[1],spherical[2],spherical[3],spherical[4],spherical[5]);
   #endif
   // Compute the position
   Rvector3 pos(rMag * Cos(delta) * Cos(lambda),
                rMag * Cos(delta) * Sin(lambda),
                rMag * Sin(delta) );

   // Compute the velocity
   Real     vx = vMag   * Cos(lambdaV) * Cos(deltaV);
   Real     vy = vx     * Tan(lambdaV);
   Real     vz = vMag   * Sin(deltaV);
   Rvector3 vel(vx, vy, vz);

   #ifdef DEBUG_STATE_CONVERSION
      MessageInterface::ShowMessage(
            "Exiting SphericalRADECToCartesian and returning state %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            pos[0],pos[1],pos[2],vel[0],vel[1],vel[2]);
   #endif
   return Rvector6(pos, vel);
}

//------------------------------------------------------------------------------
//  Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Modified Keplerian.
 *
 * @param <keplerian>     Keplerian state
 *
 * @return Spacecraft orbit state converted from Keplerian to Modified Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToModKeplerian(const Rvector6& keplerian)
{
   #ifdef DEBUG_MOD_KEPLERIAN
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() keplerian =\n   %s\n", keplerian.ToString().c_str());
   #endif

   Real a = keplerian[0];    // Semi-major axis
   Real e = keplerian[1];    // eccentricity

   // Check for  exactly parabolic orbit or infinite semi-major axis
   // then send the error message
   if ( a == 1 || GmatMathUtil::IsInf(a))
      throw UtilityException("StateConversionUtil::KeplerianToModKeplerian: "
                             "Parabolic orbits cannot be entered in Keplerian "
                             "or Modified Keplerian format");
   
   // Check for invalid eccentricity then send the error message
   if (e < 0.0)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << e << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());
      
      e *= -1.0;
   }
   
   // Check for inconsistent semi-major axis and  eccentricity
   // then send the error message
   if ((a > 0.0) && (e > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << a << ") cannot be positive if"; 
      errmsg << " eccentricity (" << a << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      a *= -1.0;
   }
   if ((a < 0.0) && (e < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << a << ") cannot be negative if ";
      errmsg << " eccentricity (" << e << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      a *= -1.0;
   }
   
   // Test that radius of periapsis is not too small.
   Real absA1E = Abs(a * (1.0 - e));
   if (absA1E < SINGULAR_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "A nearly singular conic section was encountered while converting from"; 
      errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis("; 
      errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
      throw UtilityException(errmsg.str());
   }
   //  Verify that orbit is not too close to a parabola which results in undefined SMA
   Real oneMinusE = Abs(1.0 - e);
   if (oneMinusE < PARABOLIC_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << e << ") was encountered";
      errmsg << " while converting from the Keplerian elements to"; 
      errmsg << " the Cartesian state.";
      errmsg << " The Keplerian elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   
   // Check for parabolic orbit to machine precision
   // then send the error message
   if ( GmatMathUtil::Abs(e - 1) < 2*GmatRealConstants::REAL_EPSILON)
   {
      std::string errmsg =
            "Error in conversion from Keplerian to ModKeplerian state: ";
      errmsg += "The state results in an orbit that is nearly parabolic.\n";
      throw UtilityException(errmsg);
   }
   // Check for a singular conic section
   if (GmatMathUtil::Abs(a*(1 - e) < .001))
   {
      throw UtilityException
         ("StateConversionUtil: Error in conversion from Keplerian to ModKeplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }

   // Convert into radius of periapsis and apoapsis
   Real radPer = a*(1.0 - e);
   Real radApo = a*(1.0 + e);


   #ifdef DEBUG_MOD_KEPLERIAN
   Rvector6 modkepl = Rvector6(radPer, radApo, keplerian[2], keplerian[3],
                               keplerian[4], keplerian[5]);
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() returning\n   %s\n", modkepl.ToString().c_str());
   #endif

   // return new Modified Keplerian
   return Rvector6(radPer, radApo, keplerian[2], keplerian[3],
                   keplerian[4], keplerian[5]);
}


//------------------------------------------------------------------------------
//  Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian)
//------------------------------------------------------------------------------
/**
 * Converts from Modified Keplerian to Keplerian.
 *
 * @param <modKeplerian>     Modified Keplerian state
 *
 * @return Spacecraft orbit state converted from Modified Keplerian to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::ModKeplerianToKeplerian(const Rvector6& modKeplerian)
{
   #ifdef DEBUG_MODKEP_TO_KEP
      MessageInterface::ShowMessage("Entering ModKepToKep, radPer = %12.10f, radApo = %12.10f\n",
            modKeplerian[0], modKeplerian[1]);
   #endif
   Real radPer = modKeplerian[0];     // Radius of Periapsis
   Real radApo = modKeplerian[1];     // Radius of Apoapsis

   // Check validity
   if (IsEqual(radApo, 0.0, .001))
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Radius of Apoapsis must not be zero");

   if (radApo < radPer && radApo > 0.0)
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: If RadApo < RadPer then RadApo must be negative.  "
                             "If setting Modified Keplerian State, set RadApo before RadPer to avoid this issue.  "
                             "If setting the hyperbolic asymptote, set RadPer last.");

   if (radPer <= 0.0)
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Radius of Periapsis must be greater than zero");

   if (IsEqual(radPer, 0.0, .001))
      throw UtilityException("StateConversionUtil::ModKeplerianToKeplerian: "
                             "Parabolic orbits are not currently supported."
                             "RadPer must be greater than zero");

   // Compute the division between them
   Real rpbyra = radPer/radApo;

   // compute the eccentricity and semi-major axis
   Real e = (1.0 - rpbyra)/(1.0 + rpbyra);
   Real a = radPer/(1.0 - e);

   // Return the classic Keplerian
   return Rvector6(a, e, modKeplerian[2], modKeplerian[3],
                   modKeplerian[4], modKeplerian[5]);
}


//------------------------------------------------------------------------------
// Rvector6 CartesianToEquinoctial(const Rvector6& cartesian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Equinoctial.
 *
 * @param <cartesian>     Cartesian state
 * @param <mu>            Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Cartesian to Equinoctial
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToEquinoctial(const Rvector6& cartesian, const Real& mu)
{
   #ifdef DEBUG_EQUINOCTIAL
      MessageInterface::ShowMessage("Converting from Cartesian to Equinoctial: \n");
      MessageInterface::ShowMessage("   input Cartesian is: %s\n", cartesian.ToString().c_str());
      MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   #endif
   Real sma, h, k, p, q, lambda; // equinoctial elements

   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real r = pos.GetMagnitude();
   Real v = vel.GetMagnitude();

   if (r <= 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Equinoctial - position vector is zero vector.\n");
   }
   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Cartesian to Equinoctial - gravitational constant is zero.\n");
   }

   Rvector3 eVec = ( ((v*v - mu/r) * pos) - ((pos * vel) * vel) ) / mu;
   Real e = eVec.GetMagnitude();

   // Check for a near parabolic or hyperbolic orbit.
   if ( e > 1.0 - GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg =
            "Cannot convert from Cartesian to Equinoctial - the orbit is either parabolic or hyperbolic.\n";
      throw UtilityException(errmsg);
   }

   Real xi  = (v * v / 2.0) - (mu / r);
   sma      = - mu / (2.0 * xi);

   // Check to see if the conic section is nearly singular
   if (Abs(sma * (1.0 - e)) < .001)
   {
      #ifdef DEBUG_EQUINOCTIAL
         MessageInterface::ShowMessage(
               "Equinoctial ... failing check for singular conic section ... e = %12.10f, sma = %12,10f\n",
               e, sma);
      #endif
      std::string errmsg =
         "Cannot convert from Cartesian to Equinoctial: The state results in a singular conic section with radius of periapsis less than 1 m.\n";
      throw UtilityException(errmsg);
   }

   Rvector3 am = Cross(pos, vel).GetUnitVector();
   Real inc = ACos((am[2]), GmatOrbitConstants::KEP_TOL);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Equinoctial elements: "
          "Equinoctial state does not currently support orbits with inclination of 180 degrees.\n");
   }

   Integer j = 1;  // always 1, unless inclination is exactly 180 degrees

   // Define equinoctial coordinate system
   Rvector3 f;
   f[0]      =   1.0 - ((am[0] * am[0]) / (1.0 + Pow(am[2], j)));
   f[1]      = - (am[0] * am[1]) / (1.0 + Pow(am[2], j));
   f[2]      = - Pow(am[0], j);
   f         = f.GetUnitVector();

   Rvector3 g = Cross(am,f).GetUnitVector();

   h =   eVec * g;
   k =   eVec * f;
   p =   am[0] / (1.0 + Pow(am[2], j));
   q = - am[1] / (1.0 + Pow(am[2], j));

   // Calculate mean longitude
   // First, calculate true longitude
   Real X1      = pos * f;
   Real Y1      = pos * g;
   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from CartesianToEquinoctial\n");
      MessageInterface::ShowMessage("h = %12.10f,  k = %12.10f\n", h, k);
   #endif
   Real tmpSqrt = Sqrt(1.0 - (h * h) - (k * k));
   Real beta    = 1.0 / (1.0 + tmpSqrt);
   Real cosF    = k + ((1.0 - k*k*beta) * X1 - (h * k * beta * Y1)) /
                  (sma * tmpSqrt);
   Real sinF    = h + ((1.0 - h * h * beta) * Y1 - (h * k * beta * X1)) /
                  (sma * tmpSqrt);
   Real F       = ATan2(sinF, cosF);
   // limit F to a positive value
   while (F < 0.0) F += TWO_PI;
   lambda       = (F + (h * cosF) - (k * sinF)) * DEG_PER_RAD;

   return Rvector6(sma, h, k, p, q, lambda);
}

//------------------------------------------------------------------------------
// Rvector6 EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Equinoctial to Cartesian.
 *
 * @param <equinoctial>     Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Equinoctial to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu)
{
   Real sma    = equinoctial[0];   // semi major axis
   Real h      = equinoctial[1];   // projection of eccentricity vector onto y
   Real k      = equinoctial[2];   // projection of eccentricity vector onto x
   Real p      = equinoctial[3];   // projection of N onto y
   Real q      = equinoctial[4];   // projection of N onto x
   Real lambda = equinoctial[5]*RAD_PER_DEG;   // mean longitude

   // Check for eccentricity out-of-range
   Real e = Sqrt((h * h) + (k * k));
   Real oneMinusEps = 1.0 - GmatOrbitConstants::ECC_RANGE_TOL;
   if (e > oneMinusEps )
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Cartesian elements: ";
      errmsg << "Values of EquinoctialH and EquinoctialK result in eccentricity of ";
      errmsg << e << " and eccentricity must be less than " << oneMinusEps << std::endl;
      throw UtilityException(errmsg.str());
   }
   
   // Use mean longitude to find true longitude
   Real prevF;
   Real fF;
   Real fPrimeF;
   Real F = lambda;      // first guess is mean longitude
   do {
      prevF   = F;
      fF      = F + h * Cos(F) - k * Sin(F) - lambda;
      fPrimeF = 1.0 - h * Sin(F) - k * Cos(F);
      F       = prevF - (fF/fPrimeF);
   } while (Abs(F-prevF) >= ORBIT_TOL);

   // Adjust true longitude to be between 0 and two-pi
   while (F < 0) F += TWO_PI;

   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from EquinoctialToCartesian\n");
      MessageInterface::ShowMessage("h = %12.10f,  k = %12.10f\n", h, k);
   #endif
   Real tmpSqrt;
   try
   {
      tmpSqrt = Sqrt(1.0 - (h * h) - (k * k));
   }
   catch (UtilityException &ue)
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Cartesian elements: ";
      errmsg << "Values of EquinoctialH and EquinoctialK result in eccentricity of ";
      errmsg << e << " and eccentricity must be less than " << oneMinusEps << std::endl;
      throw UtilityException(errmsg.str());
   }
   Real beta    = 1.0 / (1.0 + tmpSqrt);

   #ifdef DEBUG_STATE_CONVERSION_SQRT
      MessageInterface::ShowMessage("About to call Sqrt from EquinoctialToCartesian (2)\n");
      MessageInterface::ShowMessage("mu = %12.10f,  sma = %12.10f\n", mu, sma);
   #endif
   Real n    = Sqrt(mu/(sma * sma * sma));
   Real cosF = Cos(F);
   Real sinF = Sin(F);
   Real r    = sma * (1.0 - (k * cosF) - (h * sinF));

   if (r <= 0.0)
   {
      throw UtilityException("Error in conversion from Equinoctial to Cartesian elements: Cannot convert state because RMAG <= 0.\n");
   }

   // Calculate the cartesian components expressed in the equinoctial coordinate system

   Real X1    = sma * (((1.0 - (h * h * beta)) * cosF) + (h * k * beta * sinF) - k);
   Real Y1    = sma * (((1.0 - (k * k * beta)) * sinF) + (h * k * beta * cosF) - h);
   Real X1Dot = ((n * sma * sma) / r) * ((h * k * beta * cosF) -
                (1.0 - (h * h * beta)) * sinF);
   Real Y1Dot = ((n * sma * sma) / r) * ((1.0 - (k * k * beta)) * cosF -
                (h * k * beta * sinF));

   // assumption in conversion from equinoctial to cartesian
   Integer j = 1;  // always 1, unless inclination is exactly 180 degrees

   // Compute Q matrix
   Rmatrix33 Q(1.0 - (p * p) + (q * q),   2.0 * p * q * j,                2.0 * p,
               2.0 * p * q,               (1.0 + (p * p) - (q * q)) * j, -2.0 * q,
              -2.0 * p * j,               2.0 * q,                       (1.0 - (p * p) - (q * q)) * j);

   Rmatrix33 Q2 = (1.0 / (1.0 + (p * p) + (q * q))) * Q;
   Rvector3  f(Q2(0,0), Q2(1,0), Q2(2,0));
   Rvector3  g(Q2(0,1), Q2(1,1), Q2(2,1));
   f = f.GetUnitVector();
   g = g.GetUnitVector();

   Rvector3 pos = (X1 * f) + (Y1 * g);
   Rvector3 vel = (X1Dot * f) + (Y1Dot * g);

   return Rvector6(pos, vel);
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 CartesianToModEquinoctial(const Rvector6& cartesian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to ModifiedEquinoctial.
 *
 * @param <cartesian>     Cartesian state
 * @param <mu>            Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Cartesian to ModifiedEquinoctial
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToModEquinoctial(const Rvector6& cartesian, const Real& mu)
{
   #ifdef DEBUG_MODEQUINOCTIAL
   MessageInterface::ShowMessage("Converting from Cartesian to ModEquinoctial: \n");
   MessageInterface::ShowMessage("   input Cartesian is: %s\n", cartesian.ToString().c_str());
   MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   #endif
   
   Real p_mee, f_mee, g_mee, h_mee, k_mee, L_mee; // modequinoctial elements
   Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
   Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
   Real rMag = pos.GetMagnitude();
   //Real vMag = vel.GetMagnitude();
   
   if (rMag <= 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Modified Equinoctial - position vector is zero vector.\n");
   }
   
   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Cartesian to Modified Equinoctial - gravitational constant is zero.\n");
   }
   
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();
   
   Rvector3 rHat, vHat, hHat;
   if ( rMag == 0 )
   {
      rHat[0] = 0; rHat[1] = 0; rHat[2] = 0;
   }
   else 
   {
      rHat = pos.GetUnitVector();
   }
   
   if ( hMag == 0 )
   {
      hHat[0] = 0; hHat[1] = 0; hHat[2] = 0;
      vHat[0] = 0; vHat[1] = 0; vHat[2] = 0;
   }
   else
   {
      hHat = hVec.GetUnitVector();
      Real dotpv = pos[0]*vel[0] + pos[1]*vel[1] + pos[2]*vel[2];
      vHat = ( rMag*vel - dotpv/rMag*pos )/hMag;
   }
   
   Rvector3 eVec=   CartesianToEccVector(mu, pos, vel);
   
   p_mee = (hMag * hMag) /mu;
   
   if ( p_mee < 0 )
   {
      throw UtilityException("Semi-latus rectum has to be greater than 0.\n");
   }
   
   Real j = 1;
   Real denom = ( 1 + hHat[2]*j );
   
   // Throw an exception when singularity condition (Fix for GMT-4174)
   if ( Abs(denom) < 1.0E-7 )
   {
      //std::string warn = "Warning: Singularity may occur during calculate Modified Equinoctial element h and k.";
      //MessageInterface::PopupMessage(Gmat::WARNING_, warn);
      throw UtilityException("Singularity occurs during calculate Modified Equinoctial element h and k.\n");
   }
   //else if ( Abs(denom ) < 1.0E-16 )
   //{
   //   throw UtilityException("Singularity occurs during calculate Modified Equinoctial element h and k.\n");
   //}
   
   // Define modequinoctial coordinate system
   Rvector3 f;
   f[0]      =   1.0 - ((hHat[0] * hHat[0]) / denom);
   f[1]      = - (hHat[0] * hHat[1]) / denom;
   f[2]      = - hHat[0]*j ;
   f         = f.GetUnitVector();
   
   Rvector3 g = Cross(hHat,f).GetUnitVector();
   
   f_mee =   eVec * f;
   g_mee =   eVec * g;
   k_mee =   hHat[0] / denom ;
   h_mee = - hHat[1] / denom ;

   // Calculate True longitude
   Real sinl = rHat[1] - vHat[0];
   Real cosl = rHat[0] + vHat[1];

   L_mee = atan2(sinl,cosl);
   
   while ( L_mee > TWO_PI )
   {
      L_mee -= TWO_PI;
   }
   while ( L_mee < 0 )
   {
      L_mee += TWO_PI;
   }
   
   L_mee = L_mee * DEG_PER_RAD;
   
   return Rvector6(p_mee, f_mee, g_mee, h_mee, k_mee, L_mee);
   
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 ModEquinoctialToCartesian(const Rvector6& modequinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from ModifiedEquinoctial to Cartesian.
 *
 * @param <modequinoctial>     Modified Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Modified Equinoctial to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::ModEquinoctialToCartesian(const Rvector6& modequinoctial, const Real& mu)
{
   #ifdef DEBUG_MODEQUINOCTIAL
   MessageInterface::ShowMessage("Converting from Modified Equinoctial to Cartesian: \n");
   MessageInterface::ShowMessage("   input Modified Equinoctial is: %s\n", modequinoctial.ToString().c_str());
   MessageInterface::ShowMessage("   mu is:              %12.10f\n", mu);
   #endif
   
   Rvector3 pos;
   Rvector3 vel; // Cartesian elements

   Real p_mee= modequinoctial[0];   // Semi-latus rectum
   Real f_mee= modequinoctial[1];   // projection of eccentricity vector onto x
   Real g_mee= modequinoctial[2];   // projection of eccentricity vector onto y
   Real h_mee= modequinoctial[3];   // projection of N onto x
   Real k_mee= modequinoctial[4];   // projection of N onto y
   Real L_mee= modequinoctial[5]*RAD_PER_DEG;// True longitude

   Real j = 1; // regrograde factor

   if (mu < MU_TOL)
   {
      throw UtilityException("Cannot convert from Modified Equinoctial to Cartesian - gravitational constant is zero.\n");
   }
   
   if ( p_mee < 0 )
   {
      throw UtilityException("Cannot convert from Modified Equinoctial to Cartesian: Semi-latus rectum has to be greater than 0");
   }
   
   Real r= p_mee / (1 + f_mee*Cos(L_mee) + g_mee * Sin(L_mee));
   Real X1= r * Cos(L_mee);
   Real Y1= r * Sin(L_mee);
   
   Real dotX1, dotY1;
   if ( p_mee == 0)
   {
      dotX1= 0;
      dotY1= 0;
   }
   else
   {
      dotX1= -Sqrt(mu/p_mee) * ( g_mee + Sin(L_mee) );
      dotY1= Sqrt(mu/p_mee) * ( f_mee + Cos(L_mee) );
   }
   
   Real alpha2= h_mee*h_mee - k_mee*k_mee;
   Real s2= 1 + h_mee*h_mee + k_mee*k_mee;
   
   Rvector3 f_hat, g_hat;
   f_hat[0] = (1 + alpha2)/s2;
   f_hat[1] = (2 * k_mee * h_mee)/s2;
   f_hat[2] = (-2 * k_mee * j)/s2;
   
   g_hat[0] = ( 2 * k_mee * h_mee * j ) /s2;
   g_hat[1] = ( (1 - alpha2) * j )/s2;
   g_hat[2] = ( 2 * h_mee ) /s2;
   
   pos = X1 * f_hat + Y1 * g_hat;
   vel = dotX1 * f_hat + dotY1 * g_hat;
   
   return Rvector6(pos[0], pos[1], pos[2], vel[0], vel[1], vel[2] );
}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 KeplerianToDelaunay(const Rvector6& keplerian, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Keplerian to Delaunay
 *
 * @param <keplerian>Keplerian state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Keplerian to Delaunay
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::KeplerianToDelaunay(const Rvector6& keplerian, const Real& mu)
{
	Real sma= keplerian[0];
	Real ecc= keplerian[1];
	Real inc= keplerian[2]*RAD_PER_DEG;
	Real ta= keplerian[5]*RAD_PER_DEG;

	if ( ecc < 0.0)
	{
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Eccentricity (" << ecc << ") cannot be less than 0.0.";
      errmsg << " The sign of the eccentricity has been changed.\n";
      MessageInterface::ShowMessage(errmsg.str());

      ecc *= -1.0;
	}

   if ((sma > 0.0) && (ecc > 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << sma << ") cannot be positive if"; 
      errmsg << " eccentricity (" << ecc << ") is greater than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from hyperbolic to elliptic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      sma *= -1.0;
   }
   
   if ((sma < 0.0) && (ecc < 1.0))
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "*** Warning *** Semimajor axis (" << sma << ") cannot be negative if ";
      errmsg << " eccentricity (" << ecc << ") is less than 1.0.";
      errmsg << " The sign of the semimajor axis has been changed.";
      errmsg << " If changing orbit from elliptic to hyperbolic, set eccentricity first.\n" << std::endl;
      MessageInterface::ShowMessage(errmsg.str());
      
      sma *= -1.0;
   }
   
   if (mu < MU_TOL)
   {
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Gravitational constant (" << mu << ") is too small to convert"; 
      errmsg << " from Keplerian to Cartesian state." << std::endl;
      throw UtilityException(errmsg.str());
   }
   else
   {
      // Test that radius of periapsis is not too small.
      Real absA1E = Abs(sma * (1.0 - ecc));
      if (absA1E < SINGULAR_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "A nearly singular conic section was encountered while converting from" ;
         errmsg << "  the Keplerian elements to the Cartesian state. The radius of periapsis(" ;
         errmsg << absA1E << ") must be greater than 1 meter." << std::endl;
         throw UtilityException(errmsg.str());
      }
      //  Verify that orbit is not too close to a parabola which results in undefined SMA
      Real oneMinusE = Abs(1.0 - ecc);
      if (oneMinusE < PARABOLIC_TOL)
      {
         std::stringstream errmsg("");
         errmsg.precision(16);
         errmsg << "A nearly parabolic orbit";
         errmsg << " (ECC = " << ecc << ") was encountered";
         errmsg << " while converting from the Keplerian elements to" ;
         errmsg << " the Cartesian state.";
         errmsg << " The Keplerian elements are";
         errmsg << " undefined for a parabolic orbit." << std::endl;
         throw UtilityException(errmsg.str());
      }
   }
   
   // Check for hyperbolic orbit
   if ( ecc > 1.0 + GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg =
            "Cannot convert from Keplerian to Delaunay - the orbit is hyperbolic.\n";
      throw UtilityException(errmsg);

//      #ifdef DEBUG_CONVERT_ERRORS
//      MessageInterface::ShowMessage("Attempting to test for impossible TA:  ecc = %12.10f\n", ecc);
//      #endif
//
//      Real possible = PI - ACos(1.0/ecc);
//
//      while ( ta > PI )ta -= TWO_PI;
//      while ( ta < -PI)ta += TWO_PI;  // was +PI
//
//      if ( Abs(ta ) >= possible )
//      {
//         possible *= DEG_PER_RAD;
//         std::stringstream errmsg;
//           errmsg.precision(12);
//           errmsg << "\nError: The TA value is not physically possible for a hyperbolic orbit ";
//           errmsg << "with the input values of SMA and ECC (or RadPer and RadApo).\nThe allowed values are: ";
//           errmsg << "[" << -possible << " < TA < " << possible << " (degrees)]\nor equivalently: ";
//           errmsg << "[TA < " << possible << " or TA > " << (360.0 - possible) << " (degrees)]\n";
//           throw UtilityException(errmsg.str());
//      }
   }
   
   Real L_dela= Sqrt(mu * sma);
   Real G_dela= L_dela * Sqrt(1 - ecc*ecc);
   Real H_dela= G_dela * Cos(inc);
   Real ll_dela= TrueToMeanAnomaly(ta,ecc) * DEG_PER_RAD;
   Real gg_dela= keplerian[4];
   Real hh_dela= keplerian[3];

   return Rvector6( ll_dela, gg_dela, hh_dela, L_dela, G_dela, H_dela );

}

// Modified by M.H.
//------------------------------------------------------------------------------
// Rvector6 DelaunayToKeplerian(const Rvector6& delaunay, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Delaunay to Keplerian
 *
 * @param <keplerian>Delaunay state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Delaunay to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::DelaunayToKeplerian(const Rvector6& delaunay, const Real& mu)
{
   Real L_dela= delaunay[3];
   Real G_dela= delaunay[4];
   Real H_dela= delaunay[5];
   Real ll_dela= delaunay[0]*RAD_PER_DEG;
   
   if (GmatMathUtil::Abs(H_dela) > GmatMathUtil::Abs(G_dela))
   {
      std::string errmsg = "The magnitude of DelaunayH must be less than or ";
      errmsg += "equal to the magnitude of DelaunayG.  If setting the Delaunay ";
      errmsg += "state, set DelaunayG before DelaunayH to avoid this ";
      errmsg += "issue.\n";
      throw UtilityException(errmsg);
   }
   if ((G_dela / L_dela) > 1.0)
   {
      std::string errmsg = "It is required that (DelaunayG / DelaunayL) <= 1.  ";
      errmsg += "If setting the Delaunay state, ";
      errmsg += "set DelaunayL before DelaunayG to avoid this ";
      errmsg += "issue.\n";
      throw UtilityException(errmsg);
   }

   Real sma= L_dela*L_dela / mu;
   Real ecc= Sqrt ( 1 - (G_dela/L_dela)*(G_dela/L_dela) );
   Real inc= ACos(H_dela / G_dela) * DEG_PER_RAD;
   Real aop= delaunay[1];
   Real raan= delaunay[2];
   Real ta= MeanToTrueAnomaly(ll_dela, ecc) * DEG_PER_RAD;
   
   return Rvector6( sma, ecc, inc, raan, aop, ta );
}



// Modified by M.H. 2014.01.08
//------------------------------------------------------------------------------
// Rvector6 CartesianToPlanetodetic(const Rvector6& cartesian, Real flattening,
//                                  Real eqRadius)
//------------------------------------------------------------------------------
/**
 * Converts from Planetocentric to Cartesian.
 *
 * @param <cartesian>   Cartesian state
 * @param <flattening>  flattening coefficient for the central body
 * @param <eqRadius>    equatorial radius for the central body
 *
 * @return Spacecraft orbit state converted from Cartesian to Planetodetic
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToPlanetodetic(const Rvector6& cartesian,
                                                      Real flattening, Real eqRadius)
{
   // Convert Cartesian state to Planetocentric state
   Rvector6 planetocentric= CartesianToSphericalAZFPA(cartesian);
   
   //Real Req = 6378.1363; // equatorial radius
   //Real f = 0.0033527; // flattening coefficients
   Real Req = eqRadius; // equatorial radius
   Real f = flattening; // flattening coefficients
   
   Real rMag = planetocentric[0];
   Real lon = planetocentric[1]; // longitude
   Real latg = planetocentric[2] * RAD_PER_DEG; // planetocentric latitude
   Real vMag = planetocentric[3];
   Real azi = planetocentric[4];
   Real vfpa = planetocentric[5]; // vertical flight path angle
   Real hfpa = 90 - vfpa;// horizontal flight path angle
   
   
   // Convert planetocentric latitude to Planetodetic latitude
   Real r_z = cartesian[2];
   Real r_xy = Sqrt(cartesian[0]*cartesian[0] + cartesian[1]*cartesian[1]);
   Real latd = latg;
   Real e2 = 2*f - f*f;
   Real tol = 1;
   
   while( tol >= 1e-13)
   {
	   // If latitude is close to 90 deg
	   /*if ( Abs(latd - PI/2) < (0.001*RAD_PER_DEG) )
	   {
		   MessageInterface::ShowMessage("Latitude is close to 90 deg within 1e-3 deg \n");
		   MessageInterface::ShowMessage("Therefore Planetodetic latitude is equal to Planetocentric latitude \n");
		   break;
	   }*/
      Real latd_old = latd;
      Real C = Req / Sqrt( 1 - e2*Sin(latd_old)*Sin(latd_old) );
      latd = ATan( (r_z + C*e2*Sin(latd_old))/r_xy );
      
      tol = Abs(latd - latd_old);
   }
   
   return Rvector6(rMag, lon, latd * DEG_PER_RAD, vMag, azi, hfpa);
   
} // CartesianToPlanetodetic()




// Mod made by YKK 2014.03.26
//------------------------------------------------------------------------------
// Rvector6 PlanetodeticToCartesian(const Rvector6& planetodetic, Real flattening,
//                                  Real eqRadius)
//------------------------------------------------------------------------------
/**
 * Converts from Planetodetic to Cartesian.
 *
 * @param <planetodetic>  Planetodetic state
 * @param <flattening>    flattening coefficient for the central body
 * @param <eqRadius>      equatorial radius for the central body
 *
 * @return Spacecraft orbit state converted from Planetodetic to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::PlanetodeticToCartesian(const Rvector6& planetodetic,
                                                      Real flattening, Real eqRadius)
{
   //Real Req = 6378.1363;
   //Real f = 0.0033527;
   Real Req = eqRadius;
   Real f = flattening;
   
   Real rMag = planetodetic[0];
   Real lon  = planetodetic[1];
   Real latd = planetodetic[2];
   Real vMag = planetodetic[3];
   Real azi  = planetodetic[4];
   Real hfpa = planetodetic[5];

// Check input value

   // 2) -90 deg <= latd <= 90
   while ( (latd < -90) || (latd > 90) )
   {
	   if (latd > 90)
	   {
		   std::stringstream errmsg("");
		   errmsg << "Converting from Planetodetic to Cartesian: ";
		   errmsg << "Input Planetodetic latitude value has to be smaller than 90 deg"<< std::endl;
		   throw UtilityException(errmsg.str());
	   }
	   else
	   {
		   std::stringstream errmsg("");
		   errmsg << "Converting from Planetodetic to Cartesian: ";
		   errmsg << "Input Planetodetic latitude value has to be bigger than -90 deg"<< std::endl;
		   throw UtilityException(errmsg.str());
	   }
   }

   // 3) -90 deg <= hfpa <= 90
   while ( (hfpa < -90 ) || (hfpa > 90) )
   {
	   if (hfpa > 90)
	   {
		   std::stringstream errmsg("");
		   errmsg << "Converting from Planetodetic to Cartesian: ";
		   errmsg << "Input Horizontal FPA value has to be smaller than 90 deg"<< std::endl;
		   throw UtilityException(errmsg.str());
	   }
	   else
	   {
		   std::stringstream errmsg("");
		   errmsg << "Converting from Planetodetic to Cartesian: ";
		   errmsg << "Input Horizontal FPA value has to be bigger than -90 deg"<< std::endl;
		   throw UtilityException(errmsg.str());
	   }
   }
   
   lon  = lon * RAD_PER_DEG;
   latd = latd * RAD_PER_DEG;

   Real vfpa = 90 - hfpa;
   
// convert plantodetic latitude to planetocentric latitude
   Real e2   = 2*f - f*f;
   Real tol  = 1;
   Real latg = latd;
   
   while(tol >= 1e-13)
   {
	   // If latitude is close to 90 deg
	   if ( Abs(latg - PI/2) < (0.001*RAD_PER_DEG) || Abs(latg + PI/2) < (0.001*RAD_PER_DEG)) // modified by YKK
	   {
		   MessageInterface::ShowMessage("Latitude is close to 90 deg within 1e-3 deg \n");
		   MessageInterface::ShowMessage("Therefore Planetocentric latitude is equal to Planetodetic latitude \n");
		   break;
	   }
      Real latg_old = latg;
      
      Real x = rMag * Cos(latg_old) * Cos(lon);
      Real y = rMag * cos(latg_old) * Sin(lon);
      
      Real r_xy = Sqrt(x*x + y*y);
      Real alt = r_xy/Cos(latd) - Req/Sqrt(1-e2*Sin(latd)*Sin(latd));
      
      Real Sin2 = Sin(2*latd);
      Real Sin4 = Sin(4*latd);
      Real h_hat = alt/Req;
      Real denom = ( h_hat + 1);
      
      latg = latd + (-Sin2/denom)*f + ( (-Sin2)/(2*denom*denom) + (1/(4*denom*denom) + 1/(4*denom))*Sin4 )*f*f;
      
      tol = Abs(latg - latg_old);
   }
   
   Rvector6 planetocentric;
   planetocentric[0] =  rMag; 
   planetocentric[1] = lon*DEG_PER_RAD;
   planetocentric[2] = latg*DEG_PER_RAD;
   planetocentric[3] = vMag;
   planetocentric[4] = azi;
   planetocentric[5] = vfpa;
   
   // convert planetocentric to cartesian state
   Rvector6 cartesian = SphericalAZFPAToCartesian(planetocentric);
   
   return cartesian;
} // PlanetodeticToCartesian()


// modified by YK (2014.04.09)
//------------------------------------------------------------------------------
// Rvector6 CartesianToIncomingAsymptote(Real mu, const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Incoming Asymptote.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to Incoming Asymptote
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToIncomingAsymptote(Real mu, const Rvector6& cartesian)
{
   #ifdef DEBUG_INCOMING_ASYMPTOTE
   MessageInterface::ShowMessage
      ("StateConversionUtil::CartesianToIncomingAsymptote() entered\n   mu = %.12f\n   "
       "cartesian = %s", mu, cartesian.ToString().c_str());
   #endif
   
	// Calculate the magnitude of the position vector, right ascension, and declination
	Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
	Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);

	Real     rMag    =  pos.GetMagnitude();
	Real     vMag    =  vel.GetMagnitude();

	Rvector3 hVec    =  Cross( pos, vel );
	Real	   hMag    =  hVec.GetMagnitude();
	Rvector3 eccVec  =  CartesianToEccVector(mu, pos, vel);
	Real	   ecc	  =  eccVec.GetMagnitude();
   
	Real     c3      =  vMag * vMag - 2 * mu / rMag;

   #ifdef DEBUG_INCOMING_ASYMPTOTE
   MessageInterface::ShowMessage("   ecc = %.12f, c3 = %.12f\n", ecc, c3);
   #endif
   
	if (Abs(c3) < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(17); // what is it precision(16) ?
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << ecc << ") was encountered";
      errmsg << " while converting from the Cartesian to"; 
      errmsg << " the Incoming Asymptote elements.";
      errmsg << " The Incoming Asymptote elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
   	if (vMag < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(17); // what is it precision(16) ?
      errmsg << " The Incoming Asymptote elements are";
      errmsg << " undefined for zero-velocity orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
   if (ecc <= 1E-7)
	{
		std::stringstream errmsg("");
		errmsg.precision(16);
		errmsg << "A nearly circular orbit";
		errmsg << " (ECC = " << ecc << ") was encountered";
		errmsg << " while converting from the Cartesian to"; 
		errmsg << " the Incoming Asymptote elements.";
		errmsg << " The Incoming Asymptote elements are";
		errmsg << " undefined for a circular orbit." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
	Real     sma    =   -mu/c3;
	Real     radPer =   sma*(1 - ecc);
	Real	   fac1	 = 1/(1 + c3*hMag*hMag/mu/mu); 
	Rvector3	sVHat; 
   // sVHat is the asymptote vector of position 
   if (c3 > 1E-7)
   {
      // Is spec correct? GMAT IncomingDHA does not match with STK, it is 180 deg off
      sVHat = fac1*(-Sqrt(c3)/mu*Cross(hVec, eccVec) - eccVec); 
   }
   else if (c3 < -1E-7)
   {
      std::string warn = "Warning: Orbit is elliptic so using Apsides vector for asymptote.\n";
      MessageInterface::ShowMessage(warn);
      
      sVHat = -eccVec/ecc;
      //
      //  The two options below are really hacks, the asymptote doesn't exist.  However,
      //  having a definition that is unique and has an inverse computation is
      //  useful, especially when numeric solvers change regimes during
      //  iterative processes. - SPH
      //
      //  If orbit is elliptic, use apsides vector	
   }
   
   #ifdef DEBUG_INCOMING_ASYMPTOTE
   MessageInterface::ShowMessage("   sVHat = %s", sVHat.ToString().c_str());
   #endif
   
	Rvector3 uz(0.0, 0.0, 1.0); // inertial north direction of the coordinate system
	if (ACos(Abs(sVHat*uz)) < 1E-7) // 1e-7, 1e-11, criteria??
	{
		std::stringstream errmsg("");
		errmsg.precision(18);
		errmsg << " Error in Cart2IncomingAsymptote:";
		errmsg << " Cannot convert from  Cartestion to Incoming Asymptote Elements";
		errmsg << " because Incoming Asymptote vector is aligned with z-direction." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
	Rvector3  eaVec	=	Cross(uz,sVHat); 
	Rvector3  eaVhat	=	eaVec/eaVec.GetMagnitude(); 
	Rvector3  noVhat	=	Cross(sVHat,eaVhat); 
	Rvector3  bVec    =  Cross( hVec, sVHat );
	Real      sinBVA	=	(bVec*eaVhat)/hMag;
	Real      cosBVA	=	(bVec*-noVhat)/hMag;
	Real	    bva		=	ATan2(sinBVA,cosBVA);

	if (bva < 0.0)
		bva   =   bva + TWO_PI;
  
	Real dha  =   ASin(sVHat[2]);
	Real rha  =   ATan2(sVHat[1],sVHat[0]);
	if (rha < 0.0)
		rha   =   rha + TWO_PI;
 
	Real trueAnom = ACos( (eccVec*pos)/(ecc*rMag) );
	if (pos*vel < 0)
		trueAnom = TWO_PI - trueAnom;
   
	Rvector6 elem(radPer, c3, rha*DEG_PER_RAD, dha*DEG_PER_RAD, bva*DEG_PER_RAD, trueAnom*DEG_PER_RAD);
   
   #ifdef DEBUG_INCOMING_ASYMPTOTE
   MessageInterface::ShowMessage
      ("StateConversionUtil::CartesianToIncomingAsymptote() returning\n   %s\n", elem.ToString().c_str());
   #endif
   
	return elem;
}
//------------------------------------------------------------------------------
// Rvector6 IncomingAymptoteToCartesian(Real mu, const Rvector6& inasym)
//------------------------------------------------------------------------------
/**
 * Converts from Incoming Aymptote to Cartesian.
 *
 * @param <mu>         Gravitational constant for the central body
 * @param <inasym>     Incoming Asymptote state
 *
 * @return Spacecraft orbit state converted from Incoming Aymptote to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::IncomingAsymptoteToCartesian(Real mu, const Rvector6& inasym)
{
	Real radPer   = inasym[0]; 
	Real c3       = inasym[1];
	Real rha      = inasym[2] * RAD_PER_DEG; 
	Real dha      = inasym[3] * RAD_PER_DEG; 
	Real bva      = inasym[4] * RAD_PER_DEG; 
	Real trueAnom = inasym[5] * RAD_PER_DEG; 
   
	if (c3 < 1E-7)
	{
		std::string warn = "Warning: Orbit is elliptic so using Apsides vector for asymptote.\n";
		MessageInterface::ShowMessage(warn);
	}
   
	Real sma =  -mu/c3;
	Real ecc =  1 - radPer/sma;
   
	if (Abs(c3) < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(19); // what is it precision(16) ?
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << ecc << ") was encountered";
      errmsg << " while converting from the Cartesian to"; 
      errmsg << " the Incoming Asymptote elements.";
      errmsg << " The Incoming Asymptote elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
	if (ecc < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(20); // what is it precision(16) ?
      errmsg << "A nearly circular orbit";
      errmsg << " (ECC = " << ecc << ") was encountered";
      errmsg << " while converting from the the Incoming Asymptote elements to"; 
      errmsg << " the Cartesian.";
      errmsg << " The Incoming Asymptote elements are";
      errmsg << " undefined for a circular orbit." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
	Real sVHatx  =   Cos(dha)*Cos(rha);
	Real sVHaty  =   Cos(dha)*Sin(rha);
	Real sVHatz  =   Sin(dha);
	Rvector3 sVHat(sVHatx,sVHaty,sVHatz);
	Rvector3 uz(0.0,0.0,1.0);
	Rvector3 ux(1.0,0.0,0.0);
   
	if (ACos(Abs(sVHat*uz)) < 1E-7) // 1e-7, 1e-11, criteria??
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the the Incoming Asymptote elements to";
		errmsg << " the Cartesian.";
		errmsg << " The Incoming Asymptote vector is aligned with z-direction." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
	Rvector3  eaVec	=	Cross(uz,sVHat); 
	Rvector3  eaVhat	=	eaVec/eaVec.GetMagnitude(); 
	Rvector3  noVhat	=	Cross(sVHat,eaVhat); 
	Real	    ami	=	TWO_PI/4 - bva; // angular azimuth at infinity 
	Rvector3  hVHat	=	Sin(ami)*eaVhat + Cos(ami)*noVhat;
    Rvector3  nodeVec =   Cross(uz, hVHat);
	Real      nMag    =   nodeVec.GetMagnitude();
	Rvector3  eccVHat;
   
	if (c3 <= -1E-7)
	{
		eccVHat = -sVHat;
	}
	else if (c3 >= 1E-7)
	{
		Real     taMax   =   ACos(-1/ecc);
		Rvector3 oVHat   =   Cross(hVHat,sVHat);
		eccVHat =   Sin(taMax)*oVHat + Cos(taMax)*sVHat; 
	}

	Real inc = ACos(uz*hVHat);
	Real raan;
	Real argPeriapsis;
   
	if ( ecc >= 1E-11 && inc >= 1E-11 )  // CASE 1: Non-circular, Inclined Orbit
	{
		if (nMag == 0.0)
		{
			throw UtilityException("Cannot convert from Incoming asymptote elements to "
                                "Cartesian elements - line-of-nodes vector is a zero vector.\n");
		}
		raan = ACos( nodeVec.Get(0)/nMag );
		if (nodeVec.Get(1) < 0)
			raan = TWO_PI - raan;
      
		argPeriapsis = ACos( (nodeVec*eccVHat)/nMag );
		if (eccVHat.Get(2) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;
	}
   
	if ( ecc >= 1E-11 && inc < 1E-7 )  // CASE 2: Non-circular, Equatorial Orbit
	{
		raan = 0;
		argPeriapsis = ACos(eccVHat.Get(0));
		if (eccVHat.Get(1) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;

	}
   
	if ( ecc > 1E-11 && inc >= TWO_PI/2 - 1E-7 )  // CASE 3: Non-circular, Equatorial Retrograde Orbit
	{
		raan = 0 ;
		argPeriapsis = -ACos(eccVHat.Get(0));
		if (eccVHat.Get(1) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;
	}
   
	Real kepl[6];
	kepl[0]	=	sma;
	kepl[1]	=	ecc;
	kepl[2]	=	inc*DEG_PER_RAD;
	kepl[3]	=	raan*DEG_PER_RAD;
	kepl[4]	=	argPeriapsis*DEG_PER_RAD;
	kepl[5]	=	trueAnom*DEG_PER_RAD;
	Real pos[3];
	Real vel[3];
   
	// if the return code from a call to compute_kepl_to_cart is greater than zero, there is an error
	AnomalyType type = GetAnomalyType("TA");
	Rvector6 cart=KeplerianToCartesian(mu, kepl, type);
	return cart;
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToOutgoingAsymptote(Real mu, const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Cartesian to Outgoing Asymptote.
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <cartesian>     Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to Outgoing Asymptote
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToOutgoingAsymptote(Real mu, const Rvector6& cartesian)
{
	// Calculate the magnitude of the position vector, right ascension, and declination
	Rvector3 pos(cartesian[0], cartesian[1], cartesian[2]);
	Rvector3 vel(cartesian[3], cartesian[4], cartesian[5]);
	Real     rMag    =   pos.GetMagnitude();
	Real     vMag    =   vel.GetMagnitude();
   
	Rvector3 hVec    =   Cross( pos, vel);
	Real	   hMag    =   hVec.GetMagnitude();
           
	Rvector3 eccVec  =   CartesianToEccVector(mu, pos, vel);
	Real	   ecc	  =	eccVec.GetMagnitude();
	Real     c3      =   vMag * vMag - 2 * mu / rMag;
   
	if (Abs(c3) < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(17); // what is it precision(16) ?
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << ecc << ") was encountered";
      errmsg << " while converting from the Cartesian to"; 
      errmsg << " the Outgoing Asymptote elements.";
      errmsg << " The Outgoing Asymptote elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
    if (vMag < 1E-7)
	{
      std::stringstream errmsg("");
      errmsg.precision(17); // what is it precision(16) ?
      errmsg << " The Outgoing Asymptote elements are";
      errmsg << " undefined for zero-velocity orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
   if (ecc <= 1E-7)
	{
		std::stringstream errmsg("");
		errmsg.precision(16);
		errmsg << " A nearly circular orbit";
		errmsg << " (ECC = " << ecc << ") was encountered";
		errmsg << " while converting from the Cartesian to"; 
		errmsg << " the Outgoing asymptote elements.";
		errmsg << " The Outgoing asymptote elements are";
		errmsg << " undefined for a circular orbit." << std::endl;
		throw UtilityException(errmsg.str());
	}

	Real     sma     =   -mu/c3;
	Real     radPer  =   sma*(1 - ecc);

	Real fac1  = 1/(1 + c3*(hMag*hMag)/(mu*mu));
	Rvector3	sVHat;
   
   if (c3 > 1E-7)
      sVHat = fac1*(Sqrt(c3)/mu*Cross(hVec,eccVec) - eccVec); 
   else if (c3 < -1E-7)
   {
      
      std::string warn = "Warning: Orbit is elliptic so using Apsides vector for asymptote.\n";
      MessageInterface::ShowMessage(warn);
      sVHat = -eccVec/ecc;
      //
      //  The two options below are really hacks, the asymptote doesn't exist.  However,
      //  having a definition that is unique and has an inverse computation is
      //  useful, especially when numeric solvers change regimes during
      //  iterative processes. - SPH
      //
      //  If orbit is elliptic, use apsides vector
   }
   
   
	Rvector3 uz(0.0, 0.0, 1.0); // inertial north direction of the coordinate system
	if (ACos(Abs(sVHat*uz)) < 1E-7) // 1e-7, 1e-11, criteria??
	{
		std::stringstream errmsg("");
		errmsg.precision(18);
		errmsg << " Error in Cart2OutgoingAsymptote:";
		errmsg << " Cannot convert from  Cartestion to Outgoing Asymptote Elements";
		errmsg << " because Outgoing Asymptote vector is aligned with z-direction." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
   
	Rvector3  eaVec	=	Cross(uz,sVHat); 
	Rvector3  eaVhat	=	eaVec/eaVec.GetMagnitude(); 
	Rvector3  noVhat	=	Cross(sVHat,eaVhat); 
	Rvector3  bVec    = Cross( hVec, sVHat );
	Real      sinBVA	=	(bVec*eaVhat)/hMag;
	Real      cosBVA	=	(bVec*-noVhat)/hMag;
	Real		 bva		=	ATan2(sinBVA,cosBVA);
   
	if (bva < 0.0)
		bva   =   bva + TWO_PI;
	Real dha  =   ASin(sVHat[2]);
	Real rha  =   ATan2(sVHat[1],sVHat[0]);
	if (rha < 0.0)
		rha   =   rha + TWO_PI;
   
	Real trueAnom = ACos( (eccVec*pos)/(ecc*rMag) );
	if (pos*vel < 0)
		trueAnom = TWO_PI - trueAnom;
	Rvector6 elem(radPer, c3, rha*DEG_PER_RAD, dha*DEG_PER_RAD, bva*DEG_PER_RAD, trueAnom*DEG_PER_RAD);
   
	return elem;
}

//------------------------------------------------------------------------------
// Rvector6 OutgoingAymptoteToCartesian(Real mu, const Rvector6& outasym)
//------------------------------------------------------------------------------
/**
 * Converts from Outgoing Aymptote to Cartesian.
 *
 * @param <mu>          Gravitational constant for the central body
 * @param <outasym>     Outgoing Asymptote state
 *
 * @return Spacecraft orbit state converted from Outgoing Aymptote to  Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::OutgoingAsymptoteToCartesian(Real mu, const Rvector6& outasym)
{ 
	Real     radPer   = outasym[0]; 
	Real     c3       = outasym[1];
	Real     rha      = outasym[2] * RAD_PER_DEG; 
	Real     dha      = outasym[3] * RAD_PER_DEG; 
	Real     bva      = outasym[4] * RAD_PER_DEG; 
	Real     trueAnom = outasym[5] * RAD_PER_DEG; 
   
	if (c3 < -1E-7)
	{
		std::string warn = "Warning: Orbit is elliptic so using Apsides vector for asymptote.\n";
		MessageInterface::ShowMessage(warn);
	}

	Real     sma      =   -mu/c3;
	Real     ecc      =   1 - radPer/sma;
   
   
	if (Abs(c3) < 1E-7)
	{
		std::stringstream errmsg("");
      errmsg.precision(19); // what is it precision(16) ?
      errmsg << "A nearly parabolic orbit";
      errmsg << " (ECC = " << ecc << ") was encountered";
      errmsg << " while converting from the Cartesian to"; 
      errmsg << " the Outgoing Asymptote elements.";
      errmsg << " The Outgoing Asymptote elements are";
      errmsg << " undefined for a parabolic orbit." << std::endl;
      throw UtilityException(errmsg.str());
	}
   
	if (ecc < 1E-7)
	{
        std::stringstream errmsg("");
        errmsg.precision(20); // what is it precision(16) ?
        errmsg << "A nearly circular orbit";
        errmsg << " (ECC = " << ecc << ") was encountered";
        errmsg << " while converting from the the Outgoing Asymptote elements to"; 
        errmsg << " the Cartesian.";
        errmsg << " The Outgoing Asymptote elements are";
        errmsg << " undefined for a circular orbit." << std::endl;
        throw UtilityException(errmsg.str());
	}
   
	Real sVHatx  =   Cos(dha)*Cos(rha);
	Real sVHaty  =   Cos(dha)*Sin(rha);
	Real sVHatz  =   Sin(dha);
	Rvector3 sVHat(sVHatx,sVHaty,sVHatz);
	Rvector3 uz(0.0,0.0,1.0);
	Rvector3 ux(1.0,0.0,0.0);
   
	if (ACos(Abs(sVHat*uz)) < 1E-7) // 1e-7, 1e-11, criteria??
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the Outgoing Asymptote elements to";
		errmsg << " the Cartesian.";
		errmsg << " The Outgoing Asymptote vector is aligned with z-direction." << std::endl;
		throw UtilityException(errmsg.str());
	}
   
	Rvector3  eaVec	=	Cross(uz,sVHat); 
	Rvector3  eaVhat	=	eaVec/eaVec.GetMagnitude(); 
	Rvector3  noVhat	=	Cross(sVHat,eaVhat); 
	Real	    ami	   =	TWO_PI/4 - bva; // angular azimuth at infinity 
	Rvector3  hVHat	=	sin(ami)*eaVhat + cos(ami)*noVhat;
   Rvector3  nodeVec =  Cross(uz, hVHat);
	Real      nMag    =  nodeVec.GetMagnitude();
	Rvector3  eccVHat;
   
	if (c3 <= -1E-7)
		eccVHat = -sVHat;
	else if (c3 >= 1E-7)
	{
		Real     taMax   =   ACos(-1/ecc);
		Rvector3 oVHat   =   Cross(hVHat,sVHat);
		eccVHat =  -Sin(taMax)*oVHat + Cos(taMax)*sVHat; 
	}

	Real inc = ACos(uz*hVHat);
	Real raan;
	Real argPeriapsis;
   
	if ( ecc >= 1E-11 && inc >= 1E-11 )  // CASE 1: Non-circular, Inclined Orbit
	{
		if (nMag == 0.0)
		{
			throw UtilityException("Cannot convert from Outgoing asymptote elements to "
                                "Cartesian elements - line-of-nodes vector is a zero vector.\n");
		}
		raan = ACos( nodeVec.Get(0)/nMag );
		if (nodeVec.Get(1) < 0)
			raan = TWO_PI - raan;
      
		argPeriapsis = ACos( (nodeVec*eccVHat)/nMag );
		if (eccVHat.Get(2) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;
	}
	if ( ecc >= 1E-11 && inc < 1E-7 )  // CASE 2: Non-circular, Equatorial Orbit
	{
		raan = 0;
		argPeriapsis = ACos(eccVHat.Get(0));
		if (eccVHat.Get(1) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;
	}
	if ( ecc > 1E-11 && inc >= TWO_PI/2 - 1E-7 )  // CASE 3: Non-circular, Equatorial Retrograde Orbit
	{
		raan = 0 ;
		argPeriapsis = -ACos(eccVHat.Get(0));
		if (eccVHat.Get(1) < 0)
			argPeriapsis = TWO_PI - argPeriapsis;
	}
   
	Real kepl[6];
	kepl[0]	=	sma;
	kepl[1]	=	ecc;
	kepl[2]	=	inc*DEG_PER_RAD;
	kepl[3]	=	raan*DEG_PER_RAD;
	kepl[4]	=	argPeriapsis*DEG_PER_RAD;
	kepl[5]	=	trueAnom*DEG_PER_RAD;
	Real pos[3];
	Real vel[3];         
   
	// if the return code from a call to compute_kepl_to_cart is greater than zero, there is an error
	
	AnomalyType type = GetAnomalyType("TA");
	Rvector6 cart=KeplerianToCartesian(mu, kepl, type);
	return cart;
}


// Mod made by YKK 2014.03.29
//------------------------------------------------------------------------------
// Rvector6 CartesianToBrouwerMeanShort(Real mu, const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Outgoing Aymptote to Cartesian.
 *
 * @param <mu>           Gravitational constant for the central body
 * @param <cartesian>    Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to BrouwerMeanShort
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToBrouwerMeanShort(Real mu, const Rvector6& cartesian)
{ 
	//Real	 mu_Earth = 398600.4418;
	Real	 mu_Earth = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];

	if (Abs(mu - mu_Earth) > 1.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the Cartesian to";
		errmsg << " the BrouwerMeanShort, an error has been encountered.";
		errmsg << " Currently, BrouwerMeanShort is applicable only to the Earth." << std::endl;
		throw UtilityException(errmsg.str());
	}
	
	Real	tol		= 1.0E-8;
	Integer	maxiter	= 75;
	Rvector6 cart = cartesian;
	AnomalyType type = GetAnomalyType("TA");
	AnomalyType type2= GetAnomalyType("MA");

	Rvector6 kep = CartesianToKeplerian(mu, cart, type);
	
	#ifdef DEBUG_BrouwerMeanShortToOsculatingElements
		MessageInterface::ShowMessage("conversion to keplerian is successful");
	#endif

	#ifdef DEBUG_BrouwerMeanShortToOsculatingElements
		MessageInterface::ShowMessage("mu_earth : %12.10f \n", mu);
	#endif
	if (kep[2] > 180.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanShort, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if inclination is smaller than 180.0 DEG." << std::endl;
		throw UtilityException(errmsg.str());
	}
	if ((kep[1] > 0.99) || ((kep[1] < 0.0) ))
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanShort, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if 0.0 < ECC < 0.99."<< std::endl;
		throw UtilityException(errmsg.str());
	}
	Real radper = (kep[0]*(1.0-kep[1]));
	if (radper < 3000.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanShort, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if RadPer is larger than 3000km." << std::endl;
		throw UtilityException(errmsg.str());
	}
    if (radper < 6378.0)
	{
       if (!possibleInaccuracyShortWritten)
       {
           MessageInterface::ShowMessage(
                 "Warning: For BrouwerMeanShort, when RadPer < 6378km, there is a "
                 "possible inaccuracy due to singularity related with "
                 "inside-of-earth orbit.\n");
           possibleInaccuracyShortWritten = true;
       }
	}
	kep[5]= kep[5] * RAD_PER_DEG;
	kep[5]=TrueToMeanAnomaly(kep[5],kep[1]);
	kep[5]= kep[5] * DEG_PER_RAD;
		
	Integer pseudostate=0;
	if (kep[2] > 175.0)
	{
		kep[2] = 180.0 - kep[2]; // INC = 180 - INC
		kep[3] = -kep[3]; // RAAN = - RAAN
		cart = KeplerianToCartesian(mu, kep, type);	
		pseudostate = 1;
	}
	Rvector6 blmean = kep;
	Rvector6 kep2 = BrouwerMeanShortToOsculatingElements(mu, kep);
	
	#ifdef DEBUG_BrouwerMeanShortToOsculatingElements
		MessageInterface::ShowMessage("kep : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep[0], kep[1], kep[2], 
			kep[3], kep[4], kep[5]);
	#endif
		
	#ifdef DEBUG_BrouwerMeanShortToOsculatingElements
		MessageInterface::ShowMessage("kep2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep2[0], kep2[1], kep2[2], 
			kep2[3], kep2[4], kep2[5]);
	#endif
	Rvector6 blmean2;
	
	// blmean2= blmean + (kep - kep2);
	Rvector6 aeq;
	Rvector6 aeq2;
	Rvector6 aeqmean;
	Rvector6 aeqmean2;

	aeq[0]=kep[0];
	aeq[1]=kep[1]*Sin((kep[4]+kep[3])*RAD_PER_DEG);
	aeq[2]=kep[1]*Cos((kep[4]+kep[3])*RAD_PER_DEG);
	aeq[3]=Sin(kep[2]/2.0*RAD_PER_DEG)*Sin(kep[3]*RAD_PER_DEG);
	aeq[4]=Sin(kep[2]/2.0*RAD_PER_DEG)*Cos(kep[3]*RAD_PER_DEG);
	aeq[5]=kep[3]+kep[4]+kep[5];
	
	aeq2[0]=kep2[0];
	aeq2[1]=kep2[1]*Sin((kep2[4]+kep2[3])*RAD_PER_DEG);
	aeq2[2]=kep2[1]*Cos((kep2[4]+kep2[3])*RAD_PER_DEG);
	aeq2[3]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Sin(kep2[3]*RAD_PER_DEG);
	aeq2[4]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Cos(kep2[3]*RAD_PER_DEG);
	aeq2[5]=kep2[3]+kep2[4]+kep2[5];

	aeqmean[0]=blmean[0];
	aeqmean[1]=blmean[1]*Sin((blmean[4]+blmean[3])*RAD_PER_DEG);
	aeqmean[2]=blmean[1]*Cos((blmean[4]+blmean[3])*RAD_PER_DEG);
	aeqmean[3]=Sin(blmean[2]/2.0*RAD_PER_DEG)*Sin(blmean[3]*RAD_PER_DEG);
	aeqmean[4]=Sin(blmean[2]/2.0*RAD_PER_DEG)*Cos(blmean[3]*RAD_PER_DEG);
	aeqmean[5]=blmean[3]+blmean[4]+blmean[5];

	aeqmean2= aeqmean + (aeq-aeq2);
	#ifdef DEBUG_BrouwerMeanShortToOsculatingElements
		MessageInterface::ShowMessage("blmean2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", blmean2[0], blmean2[1], blmean2[2], 
			blmean2[3], blmean2[4], blmean2[5]);
	#endif
	Real emag = 0.9;
	Real emag_old = 1.0;
	Integer ii = 0;
	Rvector6	tmp;
	Rvector6	cart2;
	while (emag > tol)
	{
		
		blmean2[0]=aeqmean2[0];
		blmean2[1]=Sqrt(aeqmean2[1]*aeqmean2[1]+aeqmean2[2]*aeqmean2[2]);
		if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) <= 1.0)
			blmean2[2]=ACos(1.0-2.0*(aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]))*DEG_PER_RAD;
		if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) > 1.0)
			blmean2[2]=ACos(1.0-2.0*1.0)*DEG_PER_RAD;

		blmean2[3]=ATan2(aeqmean2[3],aeqmean2[4])*DEG_PER_RAD;
		if (blmean2[3] < 0.0)
		{	
			blmean2[3] = blmean2[3]+360.0;
		}
		blmean2[4]=ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD-blmean2[3];
		if (blmean2[4] < 0.0)
		{	
			blmean2[4] = blmean2[4]+360.0;
		}
		blmean2[5]=aeqmean2[5]-ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD;

		kep2	= BrouwerMeanShortToOsculatingElements(mu, blmean2);

        #ifdef DEBUG_BrouwerMeanShortToOsculatingElements
            MessageInterface::ShowMessage("kep2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep2[0], kep2[1], kep2[2], 
                kep2[3], kep2[4], kep2[5]);
        #endif
		cart2	= KeplerianToCartesian(mu, kep2, type2);
		
		tmp= cart - cart2;
		
		
		emag	= Sqrt(pow(tmp[0],2.0)+pow(tmp[1],2.0)+pow(tmp[2],2.0)+pow(tmp[3],2.0)+pow(tmp[4],2.0)+pow(tmp[5],2.0))/Sqrt(pow(cart[0],2.0) + pow(cart[1],2.0) 
			+ pow(cart[2],2.0)+pow(cart[3],2.0) + pow(cart[4],2.0) + pow(cart[5],2.0));

		aeq2[0]=kep2[0];
		aeq2[1]=kep2[1]*Sin((kep2[4]+kep2[3])*RAD_PER_DEG);
		aeq2[2]=kep2[1]*Cos((kep2[4]+kep2[3])*RAD_PER_DEG);
		aeq2[3]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Sin(kep2[3]*RAD_PER_DEG);
		aeq2[4]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Cos(kep2[3]*RAD_PER_DEG);
		aeq2[5]=kep2[3]+kep2[4]+kep2[5];

		if (emag_old > emag)
		{	
			emag_old= emag;
			//blmean	= blmean2;
			
			//blmean2= blmean + (kep - kep2);
			aeqmean= aeqmean2;
			aeqmean2= aeqmean + (aeq-aeq2);

            #ifdef DEBUG_BrouwerMeanShortToOsculatingElements
                MessageInterface::ShowMessage("blmean2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n",
                    blmean2[0], blmean2[1], blmean2[2],
                    blmean2[3], blmean2[4], blmean2[5]);
            #endif
		}
		else
		{
		   if (!brouwerNotConvergingShortWritten)
		   {
            MessageInterface::ShowMessage(
                  "Warning:  the iterative algorithm converting from Cartesian "
                  "to BrouwerMeanShort is not converging. So, it has been "
                  "interrupted. The current relative error is %12.10f .\n",emag_old);
            brouwerNotConvergingShortWritten = true;
		   }
			
			break;
		}


		if (ii > maxiter)
		{		
			MessageInterface::ShowMessage(
			      "Warning: Maximum iteration number has been reached. "
			      "There is a possible inaccuracy.\n");
			break;
		}
		ii = ii + 1;
	}	
	
	blmean[0]=aeqmean2[0];
	blmean[1]=Sqrt(aeqmean2[1]*aeqmean2[1]+aeqmean2[2]*aeqmean2[2]);	
	if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) <= 1.0)
		blmean[2]=ACos(1.0-2.0*(aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]))*DEG_PER_RAD;
	if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) > 1.0)
		blmean[2]=ACos(1.0-2.0*1.0)*DEG_PER_RAD;
	blmean[3]=ATan2(aeqmean2[3],aeqmean2[4])*DEG_PER_RAD;
	blmean[4]=ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD-blmean[3];
	blmean[5]=aeqmean2[5]-ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD;

	
	if (blmean[1] < 0.0)
	{
		blmean[1] = -blmean[1];
		blmean[4] = blmean[4] + 180.0;
		blmean[5] = blmean[5] - 180.0;
	}
	
	if (pseudostate != 0)
	{
		blmean[2] = 180.0 - blmean[2];
		blmean[3] = - blmean[3];
	}

	blmean[3] = Mod(blmean[3], 360.0);
	blmean[4] = Mod(blmean[4], 360.0);
	blmean[5] = Mod(blmean[5], 360.0);
	if (blmean[3] <0.0)
	{
		blmean[3]=blmean[3] + 360.0;
	}
	if (blmean[4] <0.0)
	{
		blmean[4]=blmean[4] + 360.0;
	}	
	if (blmean[5] <0.0)
	{
		blmean[5]=blmean[5] + 360.0;
	}

	return blmean;
}

//------------------------------------------------------------------------------
// Rvector6 BrouwerMeanShortToOsculatingElements(Real mu, const Rvector6& blms)
//------------------------------------------------------------------------------
/**
 * Converts from Brouwer-Lyddane Mean Elements (short period terms only) to 
 * Osculating Keplerian Elements.
 *
 * @param <blms>    Brouwer-Lyddane Mean Elements (short period terms only)
 *
 * @return Spacecraft orbit state converted from BrouwerMeanShort to Keplerian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::BrouwerMeanShortToOsculatingElements(Real mu, const Rvector6& blms)
{ 
	//Real	 mu_Earth = 398600.4415;
	Real	 mu_Earth = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];
   
	if (Abs(mu - mu_Earth) > 1.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the BrouwerMeanShort to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " Currently, BrouwerMeanShort is applicable only to the Earth." << std::endl;
		throw UtilityException(errmsg.str());
	}
	
	//Real	 re		 = 6378.1363; // Earth radius
	Real	 re		 = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH];
	Real	 j2		 = 1.082626925638815E-03;
	Real	 ae		 = 1.0;
	Real     smap    = blms[0] / re; 
	Real     eccp    = blms[1];
	Real     incp    = blms[2] * RAD_PER_DEG; 
	Real     raanp   = blms[3] * RAD_PER_DEG; 
	Real     aopp    = blms[4] * RAD_PER_DEG; 
	Real     meanAnom= blms[5] * RAD_PER_DEG;  
	
   	if ((incp < 0.0) || (incp > 180 * RAD_PER_DEG))
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the BrouwerMeanShort to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if 0.0 DEG. < MeanINC < 180 DEG." << std::endl;
		throw UtilityException(errmsg.str());
	}
	Real radper= blms[0] * (1.0 - blms[1]);
	if (radper < 3000.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the BrouwerMeanShort to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if mean RadPer is larger than 3000km." << std::endl;
		throw UtilityException(errmsg.str());
	}
	if (radper < 6378.0)
	{
	   if (!possibleInaccuracyShortWritten)
	   {
         MessageInterface::ShowMessage(
              "Warning: For BrouwerMeanShort, when RadPer < 6378km, there is a "
              "possible inaccuracy due to singularity "
              "related with inside-of-earth orbit.\n");
         possibleInaccuracyShortWritten = true;
	   }
	}
	if (eccp < 0.0)
	{
		eccp=-1.0*eccp;
		meanAnom= meanAnom-TWO_PI/2;
		aopp= aopp+TWO_PI/2;
		if (!brouwerSmallEccentricityWritten)
		{
         MessageInterface::ShowMessage(
               "Warning: Because eccentricity is smaller than 0.0, "
               "the current apoapsis will be taken to be new periapsis.\n");
         brouwerSmallEccentricityWritten = true;
		}
	}
	if (eccp > 0.99)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the BrouwerMeanShort to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanShort is applicable";
		errmsg << " only if mean ECC is smaller than 0.99." << std::endl;
		throw UtilityException(errmsg.str());
	}
	Integer pseudostate=0;

	if (incp > 175.0* RAD_PER_DEG)
	{
		incp = TWO_PI/2-incp;// INC = 180 - INC
		raanp = -raanp; // RAAN = -RAAN
		pseudostate = 1;
	}
	raanp	= Mod(raanp,TWO_PI);
	aopp	= Mod(aopp,TWO_PI);
	meanAnom= Mod(meanAnom,TWO_PI);
	
	if (raanp < 0.0)
	{
		raanp	= raanp + TWO_PI;
	}
	if (aopp < 0.0)
	{
		aopp	= aopp + TWO_PI;
	}	
	if (meanAnom < 0.0)
	{
		meanAnom= meanAnom + TWO_PI;
	}

	Real	eta		= Sqrt(1.0-pow(eccp,2.0));
	Real	theta	= Cos(incp);
	Real	p		= smap*pow(eta,2.0);
	Real	k2		= 1.0/2.0*j2; // pow(ae,2) = 0 ?? wierd  
	Real	gm2		= k2/pow(smap,2.0);
	Real	gm2p	= gm2/pow(eta,4.0);

	Real	tap		= MeanToTrueAnomaly(meanAnom, eccp, 1.0E-8);
	if (tap < 0.0)
	{	
		tap=tap+TWO_PI;
	}
	Real	rp		= p/(1.0 + eccp*Cos(tap));
	Real	adr		= smap/rp;
	
	Real	sma1	=	smap + smap*gm2*((pow(adr,3.0)-1.0/pow(eta,3.0))*(-1.0+3.0*pow(theta,2.0))+3.0*(1.0-pow(theta,2.0))*pow(adr,3.0)*Cos(2.0*aopp+2.0*tap));
	
	Real	decc	=	pow(eta,2.0)/2.0*((3.0*(1.0/pow(eta,6.0))*gm2*(1.0-pow(theta,2.0))*Cos(2.0*aopp+2.0*tap)*(3.0*eccp*pow(Cos(tap),2.0)+3.0*Cos(tap)+pow(eccp,2.0)*pow(Cos(tap),3.0)+eccp))
						- gm2p*(1.0-pow(theta,2.0))*(3.0*Cos(2.0*aopp+tap)+Cos(3.0*tap+2.0*aopp))+(3.0*pow(theta,2.0)-1.0)*gm2/pow(eta,6.0)*(eccp*eta+eccp/(1+eta)+3.0*eccp*pow(Cos(tap),2.0)
						+ 3.0*Cos(tap)+pow(eccp,2.0)*pow(Cos(tap),3.0)));
      
	Real	dinc	=	gm2p/2.0*theta*Sin(incp)*(3.0*Cos(2.0*aopp+2.0*tap)+3.0*eccp*Cos(2.0*aopp+tap)+eccp*Cos(2.0*aopp+3.0*tap));

	Real	draan	=	-gm2p/2.0*theta*(6.0*(tap-meanAnom+eccp*Sin(tap)) - 3.0*Sin(2.0*aopp+2.0*tap) - 3.0*eccp*Sin(2.0*aopp+tap) - eccp*Sin(2.0*aopp+3.0*tap));
	
	Real	aop1	=	aopp+ 3.0*j2/2.0/pow(p,2.0)*((2.0-5.0/2.0*pow(Sin(incp),2.0))*(tap-meanAnom+eccp*Sin(tap)) 
						+ (1.0-3.0/2.0*pow(Sin(incp),2.0))*(1.0/eccp*(1.0-1.0/4.0*pow(eccp,2.0))*Sin(tap)+1.0/2.0*Sin(2.0*tap)+eccp/12.0*Sin(3.0*tap)) 
						- 1.0/eccp*(1.0/4.0*pow(Sin(incp),2.0) + (1.0/2.0-15.0/16.0*pow(Sin(incp),2.0))*pow(eccp,2.0))*Sin(tap+2.0*aopp) 
						+ eccp/16.0*pow(Sin(incp),2.0)*Sin(tap-2.0*aopp) -1.0/2.0*(1.0-5.0/2.0*pow(Sin(incp),2.0))*Sin(2.0*tap+2.0*aopp) 
						+ 1.0/eccp*(7.0/12.0*pow(Sin(incp),2.0)-1.0/6.0*(1.0-19.0/8.0*pow(Sin(incp),2.0))*pow(eccp,2.0))*Sin(3.0*tap+2.0*aopp) 
					    + 3.0/8.0*pow(Sin(incp),2.0)*Sin(4.0*tap+2.0*aopp) + eccp/16.0*pow(Sin(incp),2.0)*Sin(5.0*tap+2.0*aopp));
	
	Real	ma1		=	meanAnom + 3.0*j2*eta/2.0/eccp/pow(p,2)*(-(1.0-3.0/2.0*pow(Sin(incp),2.0))*((1.0-pow(eccp,2.0)/4.0)*Sin(tap)+eccp/2.0*Sin(2.0*tap)+pow(eccp,2.0)/12.0*Sin(3.0*tap)) 
						+ pow(Sin(incp),2.0)*(1.0/4.0*(1.0+5.0/4.0*pow(eccp,2.0))*Sin(tap+2.0*aopp)-pow(eccp,2.0)/16.0*Sin(tap-2.0*aopp) -7.0/12.0*(1.0-pow(eccp,2.0)/28.0)*Sin(3.0*tap+2.0*aopp) 
						-3.0*eccp/8.0*Sin(4.0*tap+2.0*aopp)-pow(eccp,2.0)/16.0*Sin(5.0*tap+2.0*aopp)));
	
	Real	lgh		=	raanp+aopp+meanAnom+gm2p/4.0*(6.0*(-1.0-2.0*theta+5.0*pow(theta,2.0))*(tap-meanAnom+eccp*Sin(tap)) 
						+ (3.0 + 2.0*theta- 5.0*pow(theta,2.0))*(3.0*Sin(2.0*aopp+2.0*tap)+3.0*eccp*Sin(2.0*aopp+tap)+eccp*Sin(2.0*aopp+3.0*tap))) 
						+ gm2p/4.0*pow(eta,2.0)/(eta+1.0)*eccp*(3.0*(1.0-pow(theta,2.0))*(Sin(3.0*tap+2.0*aopp)*(1.0/3.0+pow(adr,2.0)*pow(eta,2.0)+adr)
						+ Sin(2.0*aopp+tap)*(1.0-pow(adr,2.0)*pow(eta,2.0)-adr))+2.0*Sin(tap)*(3.0*pow(theta,2.0)-1.0)*(1.0+pow(adr,2.0)*pow(eta,2.0)+adr));



	Real	eccpdl	=	-pow(eta,3.0)/4.0*gm2p*(2.0*(-1.0+3.0*pow(theta,2.0))*(pow(adr,2.0)*pow(eta,2.0)+adr+1.0)*Sin(tap) 
						+ 3.0*(1.0-pow(theta,2.0))*((-pow(adr,2.0)*pow(eta,2.0)-adr+1.0)*Sin(2.0*aopp+tap) + (pow(adr,2.0)*pow(eta,2.0)+adr+1.0/3.0)*Sin(2.0*aopp+3.0*tap)));
	Real	ecosl	=	(eccp+decc)*Cos(meanAnom)-eccpdl*Sin(meanAnom);
	Real	esinl	=	(eccp+decc)*Sin(meanAnom)+eccpdl*Cos(meanAnom);
	Real	ecc1	=	sqrt(pow(ecosl,2.0)+pow(esinl,2.0));
	if (ecc1 < 1.0E-11)
	{
		ma1=0.0;
	} else
	{
		ma1=ATan2(esinl,ecosl);
		if (ma1 < 0.0)
		{
			ma1 = ma1 + TWO_PI;
		}
	}

	Real	sinhalfisinh	=	(Sin(0.5*incp)+Cos(0.5*incp)*0.5*dinc)*Sin(raanp)+1.0/2.0*Sin(incp)/Cos(incp/2.0)*draan*Cos(raanp);
	Real	sinhalficosh	=	(Sin(0.5*incp)+Cos(0.5*incp)*0.5*dinc)*Cos(raanp)-1.0/2.0*Sin(incp)/Cos(incp/2.0)*draan*Sin(raanp);
	Real	inc1;
	if (Sqrt(pow(sinhalfisinh,2.0)+pow(sinhalficosh,2.0)) > 1.0)
	{
		inc1 =	2.0*ASin(1);
	}
	else if (Sqrt(pow(sinhalfisinh,2.0)+pow(sinhalficosh,2.0)) < -1.0)
	{
		
		inc1 =	2.0*ASin(-1.0);
	}
	else
	{
		inc1 =	2.0*ASin(Sqrt(pow(sinhalfisinh,2.0)+pow(sinhalficosh,2.0)));
	}
    Real raan1;

	if ((inc1 ==0.0) || (inc1 == 180.0))
	{
		raan1=0.0;
		aop1=lgh-ma1-raan1;
	} else 
	{
		raan1=ATan2(sinhalfisinh,sinhalficosh);
		if (raan1 < 0.0)
		{
			raan1 = raan1 + TWO_PI;
		}
		aop1=lgh-ma1-raan1;
	}
	
	

	aop1=Mod(aop1,TWO_PI);
	if (aop1 < 0.0)
	{
		aop1=aop1+TWO_PI;
	}

	if (raan1 < 0.0)
	{
		raan1 = raan1 + TWO_PI;
	}

	Real kepl[6];
	kepl[0]	=	sma1*re;
	kepl[1]	=	ecc1;
	kepl[2]	=	inc1*DEG_PER_RAD;
	kepl[3]	=	raan1*DEG_PER_RAD;
	kepl[4]	=	aop1*DEG_PER_RAD;
	kepl[5]	=	ma1*DEG_PER_RAD;
		
	if (pseudostate != 0)
	{
		kepl[2] = 180.0 - kepl[2];
		kepl[3] = 360.0 - kepl[3];
	}
	
	return kepl;
}

//------------------------------------------------------------------------------
// Rvector6 BrouwerMeanShortToCartersian(Real mu, const Rvector6& blms)
//------------------------------------------------------------------------------
/**
 * Converts from Brouwer-Lyddane Mean Elements (short period terms only) to Cartesian.
 *
 * @param <blms>    Brouwer-Lyddane Mean Elements (short period terms only)
 *
 * @return Spacecraft orbit state converted from BrouwerMeanShort to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::BrouwerMeanShortToCartesian(Real mu, const Rvector6& blms)
{ 
	Rvector6 kepl=BrouwerMeanShortToOsculatingElements(mu, blms);

	AnomalyType type = GetAnomalyType("MA");
	Rvector6 cart=KeplerianToCartesian(mu, kepl, type);
	return cart;
}
//------------------------------------------------------------------------------
// Rvector6 CartesianToBrouwerMeanLong(Real mu, const Rvector6& cartesian)
//------------------------------------------------------------------------------
/**
 * Converts from Brouwer-Lyddane Mean Elements (short and long period terms) to Cartesian.
 *
 * @param <cartesian> Cartesian state
 *
 * @return Spacecraft orbit state converted from Cartesian to BrouwerMeanLong
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToBrouwerMeanLong(Real mu, const Rvector6& cartesian)
{ 
	//Real	 mu_Earth = 398600.4415;
	Real	 mu_Earth = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];
   
	if (Abs(mu - mu_Earth) > 1.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the Cartesian to";
		errmsg << " the BrouwerMeanLong, an error has been encountered.";
		errmsg << " Currently, BrouwerMeanLong is applicable only to the Earth." << std::endl;
		throw UtilityException(errmsg.str());
	}
	
	Real	tol=1.0E-8;
	Integer	maxiter=75;
	Rvector6 cart = cartesian;
	AnomalyType type = GetAnomalyType("TA");
	AnomalyType type2= GetAnomalyType("MA");

	Rvector6 kep = CartesianToKeplerian(mu, cart, type);

	if ((kep[1] > 0.99) || (kep[1] < 0.0))
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanLong, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if 0.0 < ECC < 0.99." << std::endl;
		throw UtilityException(errmsg.str());
	}
	Real radper = (kep[0]*(1.0-kep[1]));
	if (radper < 3000.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanLong, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if RadPer is larger than  3000km." << std::endl;
		throw UtilityException(errmsg.str());
	}
    if (radper < 6378.0)
	{
       if (!possibleInaccuracyLongWritten)
       {
           MessageInterface::ShowMessage(
                 "Warning: For BrouwerMeanLong, when RadPer < 6378km, there is a "
                 "possible inaccuracy due to singularity "
                 "related with inside-of-earth orbit.\n");
           possibleInaccuracyLongWritten = true;
       }
	}
	if (kep[2] > 180.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the Cartesian to";
		errmsg << " the BrouwerMeanLong, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if inclination is smaller than 180 DEG." << std::endl;
		throw UtilityException(errmsg.str());
	}

	if ((58.80 < kep[2] && kep[2] < 65.78) || (114.22 < kep[2] && kep[2] < 121.2))
	{
	   if (!inaccuracyCriticalAngleWritten)
	   {
         MessageInterface::ShowMessage(
              "Warning: For BrouwserMeanLong, when 58.80 DEG < "
               "INC < 65.78 DEG, or 114.22 DEG "
              "< INC < 121.2DEG, there is a possible inaccuracy due to "
              "singularity related with critical angle.\n");
         inaccuracyCriticalAngleWritten = true;
	   }
	}

	kep[5]= kep[5] * RAD_PER_DEG;
	kep[5]=TrueToMeanAnomaly(kep[5],kep[1]);
	kep[5]= kep[5] * DEG_PER_RAD;

	Integer pseudostate=0;
	if (kep[2] > 175.0)
	{
		kep[2] = 180.0 - kep[2]; // INC = 180 - INC
		kep[3] = -kep[3]; // RAAN = - RAAN
		cart = KeplerianToCartesian(mu, kep, type);	
		pseudostate = 1;
	}
	
	Rvector6 blmean ;
	blmean= kep;
	Rvector6 kep2	;
	kep2= BrouwerMeanLongToOsculatingElements(mu, kep);
	
	#ifdef DEBUG_BrouwerMeanLongToOsculatingElements
		MessageInterface::ShowMessage("kep : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep[0], kep[1], kep[2], 
			kep[3], kep[4], kep[5]);
	#endif
		
	#ifdef DEBUG_BrouwerMeanLongToOsculatingElements
		MessageInterface::ShowMessage("kep2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep2[0], kep2[1], kep2[2], 
			kep2[3], kep2[4], kep2[5]);
	#endif
	Rvector6 blmean2;
	
	// blmean2= blmean + (kep - kep2);
	Rvector6 aeq;
	Rvector6 aeq2;
	Rvector6 aeqmean;
	Rvector6 aeqmean2;

	aeq[0]=kep[0];
	aeq[1]=kep[1]*Sin((kep[4]+kep[3])*RAD_PER_DEG);
	aeq[2]=kep[1]*Cos((kep[4]+kep[3])*RAD_PER_DEG);
	aeq[3]=Sin(kep[2]/2.0*RAD_PER_DEG)*Sin(kep[3]*RAD_PER_DEG);
	aeq[4]=Sin(kep[2]/2.0*RAD_PER_DEG)*Cos(kep[3]*RAD_PER_DEG);
	aeq[5]=kep[3]+kep[4]+kep[5];
	
	aeq2[0]=kep2[0];
	aeq2[1]=kep2[1]*Sin((kep2[4]+kep2[3])*RAD_PER_DEG);
	aeq2[2]=kep2[1]*Cos((kep2[4]+kep2[3])*RAD_PER_DEG);
	aeq2[3]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Sin(kep2[3]*RAD_PER_DEG);
	aeq2[4]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Cos(kep2[3]*RAD_PER_DEG);
	aeq2[5]=kep2[3]+kep2[4]+kep2[5];
	
	aeqmean[0]=blmean[0];
	aeqmean[1]=blmean[1]*Sin((blmean[4]+blmean[3])*RAD_PER_DEG);
	aeqmean[2]=blmean[1]*Cos((blmean[4]+blmean[3])*RAD_PER_DEG);
	aeqmean[3]=Sin(blmean[2]/2.0*RAD_PER_DEG)*Sin(blmean[3]*RAD_PER_DEG);
	aeqmean[4]=Sin(blmean[2]/2.0*RAD_PER_DEG)*Cos(blmean[3]*RAD_PER_DEG);
	aeqmean[5]=blmean[3]+blmean[4]+blmean[5];

	aeqmean2= aeqmean + (aeq-aeq2);
	#ifdef DEBUG_BrouwerMeanLongToOsculatingElements
		MessageInterface::ShowMessage("blmean2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", blmean2[0], blmean2[1], blmean2[2], 
			blmean2[3], blmean2[4], blmean2[5]);
	#endif
	Real emag = 0.9;
	Real emag_old = 1.0;
	Integer ii = 0;
	Rvector6	tmp;
	Rvector6	cart2;
	while (emag > tol)
	{
		blmean2[0]=aeqmean2[0];
		blmean2[1]=Sqrt(aeqmean2[1]*aeqmean2[1]+aeqmean2[2]*aeqmean2[2]);
		if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) <= 1.0)
			blmean2[2]=ACos(1.0-2.0*(aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]))*DEG_PER_RAD;
		if ((aeqmean2[3]*aeqmean2[3]+aeqmean2[4]*aeqmean2[4]) > 1.0)
			blmean2[2]=ACos(1.0-2.0*1.0)*DEG_PER_RAD;

		blmean2[3]=ATan2(aeqmean2[3],aeqmean2[4])*DEG_PER_RAD;
		if (blmean2[3] < 0.0)
		{	
			blmean2[3] = blmean2[3]+360.0;
		}
		blmean2[4]=ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD-blmean2[3];
		blmean2[5]=aeqmean2[5]-ATan2(aeqmean2[1],aeqmean2[2])*DEG_PER_RAD;

        #ifdef DEBUG_BrouwerMeanLongToOsculatingElements
            MessageInterface::ShowMessage("blmean2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", blmean2[0], blmean2[1], blmean2[2], 
                blmean2[3], blmean2[4], blmean2[5]);
        #endif

		kep2	= BrouwerMeanLongToOsculatingElements(mu, blmean2);

        #ifdef DEBUG_BrouwerMeanLongToOsculatingElements
            MessageInterface::ShowMessage("kep2 : %12.10f, %12.10f, %12.10f, %12.10f, %12.10f, %12.10f\n", kep2[0], kep2[1], kep2[2], 
                kep2[3], kep2[4], kep2[5]);
        #endif
		cart2	= KeplerianToCartesian(mu, kep2, type2);
		
		tmp= cart - cart2;
		
		
		emag	= Sqrt(pow(tmp[0],2.0)+pow(tmp[1],2.0)+pow(tmp[2],2.0)+pow(tmp[3],2.0)+pow(tmp[4],2.0)+pow(tmp[5],2.0))/Sqrt(pow(cart[0],2.0) + pow(cart[1],2.0) 
			+ pow(cart[2],2.0)+pow(cart[3],2.0) + pow(cart[4],2.0) + pow(cart[5],2.0));

		if (emag_old > emag)
		{	
			emag_old= emag;
			//blmean	= blmean2;
			
			//blmean2= blmean + (kep - kep2);
			
			aeq2[0]=kep2[0];
			aeq2[1]=kep2[1]*Sin((kep2[4]+kep2[3])*RAD_PER_DEG);
			aeq2[2]=kep2[1]*Cos((kep2[4]+kep2[3])*RAD_PER_DEG);
			aeq2[3]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Sin(kep2[3]*RAD_PER_DEG);
			aeq2[4]=Sin(kep2[2]/2.0*RAD_PER_DEG)*Cos(kep2[3]*RAD_PER_DEG);
			aeq2[5]=kep2[3]+kep2[4]+kep2[5];

			aeqmean= aeqmean2;
			aeqmean2= aeqmean + (aeq-aeq2);
		}
		else
		{
		   if (!brouwerNotConvergingLongWritten)
		   {
            MessageInterface::ShowMessage(
                  "Warning: the iterative algorithm converting from Cartesian "
                  "to BrouwerMeanLong is not converging. So, it has been "
                  "interrupted. The current relative error is %12.10f . \n",emag_old);
            brouwerNotConvergingLongWritten = true;
		   }
			break;
		}
		if (ii > maxiter)
		{		
			MessageInterface::ShowMessage("Warning: Maximum iteration number has been reached. There is a possible inaccuracy.\n");
			break;
		}
		ii = ii + 1;
	}	
	
	blmean[0]=aeqmean[0];
	blmean[1]=Sqrt(aeqmean[1]*aeqmean[1]+aeqmean[2]*aeqmean[2]);
	if ((aeqmean[3]*aeqmean[3]+aeqmean[4]*aeqmean[4]) <= 1.0)
		blmean[2]=ACos(1.0-2.0*(aeqmean[3]*aeqmean[3]+aeqmean[4]*aeqmean[4]))*DEG_PER_RAD;
	if ((aeqmean[3]*aeqmean[3]+aeqmean[4]*aeqmean[4]) > 1.0)
		blmean[2]=ACos(1.0-2.0*1.0)*DEG_PER_RAD;

	blmean[3]=ATan2(aeqmean[3],aeqmean[4])*DEG_PER_RAD;
	if (blmean[3] < 0.0)
	{	
		blmean[3] = blmean[3]+360.0;
	}
	blmean[4]=ATan2(aeqmean[1],aeqmean[2])*DEG_PER_RAD-blmean[3];
	blmean[5]=aeqmean[5]-ATan2(aeqmean[1],aeqmean[2])*DEG_PER_RAD;

	
	if (pseudostate != 0)
	{
		blmean[2] = 180.0 - blmean[2];
		blmean[3] = - blmean[3];
	}

	blmean[3] = Mod(blmean[3], 360.0);
	blmean[4] = Mod(blmean[4], 360.0);
	blmean[5] = Mod(blmean[5], 360.0);
	if (blmean[3] <0.0)
	{
		blmean[3]=blmean[3] + 360.0;
	}
	if (blmean[4] <0.0)
	{
		blmean[4]=blmean[4] + 360.0;
	}	
	if (blmean[5] <0.0)
	{
		blmean[5]=blmean[5] + 360.0;
	}
	return blmean;
}

//------------------------------------------------------------------------------
// Rvector6 BrouwerMeanLongToOsculatingElements(Real mu, const Rvector6& blml)
//------------------------------------------------------------------------------
/**
 * Converts from BrouwerMeanLong to Osculating Keplerian Elements.
 *
 * @param <blml> Brouwer-Lyddane Mean Elements (Long and long period terms)
 *
 * @return Spacecraft orbit state converted from Keplerian to BrouwerMeanLong
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::BrouwerMeanLongToOsculatingElements(Real mu, const Rvector6 &blml)
{
	//Real	 mu_Earth = 398600.4418;
	Real	 mu_Earth = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH];
   
	if (Abs(mu - mu_Earth) > 1.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the BrouwerMeanLong to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " Currently, BrouwerMeanLong is applicable only to the Earth." << std::endl;
		throw UtilityException(errmsg.str());
	}

	Integer pseudostate = 0;
	
	//Real	 re		 = 6378.1363; // Earth radius
	Real	 re		 = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH];
	Real	 j2		 = 1.082626925638815E-03;
	Real	 j3     = -0.2532307818191774E-5;
	Real	 j4     = -0.1620429990000000E-5;
	Real	 j5     = -0.2270711043920343E-6;
	Real	 ae		 = 1.0;
	Real     smadp    = blml[0] / re; 
	Real     eccdp    = blml[1];
	Real     incdp    = blml[2] * RAD_PER_DEG; 
	Real     raandp   = blml[3] * RAD_PER_DEG; 
	Real     aopdp    = blml[4] * RAD_PER_DEG; 
	Real     meanAnom = blml[5] * RAD_PER_DEG;  
	
	if (incdp > 175.0* RAD_PER_DEG)
	{
		incdp = TWO_PI/2-incdp;// INC = 180 - INC
		raandp = -raandp; // RAAN = -RAAN
		pseudostate = 1;
	}
	// negative eccentricity aviodance lines
	if (eccdp > 0.99)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " while converting from the BrouwerMeanLong to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if mean ECC is smaller than 0.99." << std::endl;
		throw UtilityException(errmsg.str());
	}
	
	Real radper = (blml[0]*(1.0-blml[1]));
	if (radper < 3000.0)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the BrouwerMeanLong to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if RadPer is larger than 3000km." << std::endl;
		throw UtilityException(errmsg.str());
	}
    if (radper < 6378.0)
	{
       if (!possibleInaccuracyLongWritten)
       {
          MessageInterface::ShowMessage(
              "Warning: For BrouwerMeanLong, when RadPer < 6378km, there "
              "is a possible inaccuracy due to singularity related "
              "with inside-of-earth orbit.\n");
          possibleInaccuracyLongWritten = true;
       }
	}
    
	if (blml[2] > 180)
	{
		std::stringstream errmsg("");
		errmsg.precision(21);
		errmsg << " While converting from the BrouwerMeanLong to";
		errmsg << " the Cartesian, an error has been encountered.";
		errmsg << " BrouwerMeanLong is applicable";
		errmsg << " only if inclination is smaller than 180 DEG." << std::endl;
		throw UtilityException(errmsg.str());
	}
    
	raandp	= Mod(raandp,TWO_PI);
	aopdp	= Mod(aopdp,TWO_PI);
	meanAnom= Mod(meanAnom,TWO_PI);
	
	if (raandp < 0.0)
	{
		raandp	= raandp + TWO_PI;
	}
	if (aopdp < 0.0)
	{
		aopdp	= aopdp + TWO_PI;
	}	
	if (meanAnom < 0.0)
	{
		meanAnom= meanAnom + TWO_PI;
	}
	
    Real    bk2=(1.0/2.0)*(j2*ae*ae);
    Real    bk3=-j3*pow(ae,3.0);
    Real    bk4=-(3.0/8.0)*j4*pow(ae,4.0);
    Real    bk5=-j5*pow(ae,5.0);
    Real    eccdp2=eccdp*eccdp;
    Real    cn2=1.0-eccdp2;
    Real    cn=Sqrt(cn2);
    Real    gm2=bk2/pow(smadp,2.0);
    Real    gmp2=gm2/(cn2*cn2);
    Real    gm4=bk4/pow(smadp,4.0);
    Real    gmp4=gm4/pow(cn,8.0);
    Real    theta=Cos(incdp);
    Real    theta2=theta*theta;
    Real    theta4=theta2*theta2;

	Real    gm3=bk3/pow(smadp,3.0);
    Real    gmp3=gm3/(cn2*cn2*cn2);
    Real    gm5=bk5/pow(smadp,5.0);
    Real    gmp5=gm5/pow(cn,10.0);
	
    Real    g3dg2=gmp3/gmp2;
    Real    g4dg2=gmp4/gmp2;
    Real    g5dg2=gmp5/gmp2;

	Real    sinMADP=Sin(meanAnom);
    Real    cosMADP=Cos(meanAnom);
    Real    sinraandp=Sin(raandp);
    Real    cosraandp=Cos(raandp);
	
//-------------------------------------I
// COMPUTE TRUE ANOMALY(DOUBLE PRIMED) I
//-------------------------------------I
    // Need to loosen tolerance to 1e-12 for GMT-4446 fix (LOJ: 2014.03.18)
    //Real	tadp		= MeanToTrueAnomaly(meanAnom, eccdp, 0.5E-15);
   Real	tadp		= MeanToTrueAnomaly(meanAnom, eccdp, 1e-12);
    
	Real	rp		= smadp*(1.0-eccdp*eccdp)/(1.0 + eccdp*Cos(tadp));
	Real	adr		= smadp/rp;
    Real    sinta=Sin(tadp);
    Real    costa=Cos(tadp);
    Real    cs2gta=Cos(2.0*aopdp+2.0*tadp);
    Real    adr2=adr*adr;
    Real    adr3=adr2*adr;
    Real    costa2=costa*costa;
	
    Real    a1=((1.0/8.0)*gmp2*cn2)*(1.0-11.0*theta2-((40.0*theta4)/(1.0-5.0*theta2)));
    Real    a2=((5.0/12.0)*g4dg2*cn2)*(1.0-((8.0*theta4)/(1.0-5.0*theta2))-3.0*theta2);
    Real    a3=g5dg2*((3.0*eccdp2)+4.0);
    Real    a4=g5dg2*(1.0-(24.0*theta4)/(1.0-5.0*theta2)-9.0*theta2);
    Real    a5=(g5dg2*(3.0*eccdp2+4.0))*(1.0-(24.0*theta4)/(1.0-5.0*theta2)-9.0*theta2);
    Real    a6=g3dg2*(1.0/4.0);
    Real    sinI=Sin(incdp);
    Real    a10=cn2*sinI;
    Real    a7=a6*a10;
    Real    a8p=g5dg2*eccdp*(1.0-(16.0*theta4)/(1.0-5.0*theta2)-5.0*theta2);
    Real    a8=a8p*eccdp;
	
    Real    b13=eccdp*(a1-a2);
    Real    b14=a7+(5.0/64.0)*a5*a10;
    Real    b15=a8*a10*(35.0/384.0); 

    Real    a11=2.0+eccdp2;
    Real    a12=3.0*eccdp2+2.0;
    Real    a13=theta2*a12;
    Real    a14=(5.0*eccdp2+2.0)*(theta4/(1.0-5.0*theta2));
    Real    a17=theta4/((1.0-5.0*theta2)*(1.0-5.0*theta2));
    Real    a15=(eccdp2*theta4*theta2)/((1.0-5.0*theta2)*(1.0-5.0*theta2));
    Real    a16=theta2/(1.0-5.0*theta2);
    Real    a18=eccdp*sinI;
    Real    a19=a18/(1.0+cn);
    Real    a21=eccdp*theta;
    Real    a22=eccdp2*theta;
    Real    sinI2=Sin(incdp/2.0);
    Real    cosI2=Cos(incdp/2.0);
    Real    tanI2=Tan(incdp/2.0);
    Real    a26=16.0*a16+40.0*a17+3.0;
    Real    a27=a22*(1.0/8.0)*(11.0+200.0*a17+80.0*a16);
	
    Real    b1=cn*(a1-a2)-((a11-400.0*a15-40.0*a14-11.0*a13)*(1.0/16.0)+(11.0+200.0*a17+80.0*a16)*a22*(1.0/8.0))*gmp2+((-80.0*a15-8.0*a14-3.0*a13+a11)*(5.0/24.0)+(5.0/12.0)*a26*a22)*g4dg2;
    Real    b2=a6*a19*(2.0+cn-eccdp2)+(5.0/64.0)*a5*a19*cn2-(15.0/32.0)*a4*a18*cn*cn2+((5.0/64.0)*a5+a6)*a21*tanI2+(9.0*eccdp2+26.0)*(5.0/64.0)*a4*a18+(15.0/32.0)*a3*a21*a26*sinI*(1.0-theta);
    Real    b3=((80.0*a17+5.0+32.0*a16)*a22*sinI*(theta-1.0)*(35.0/576.0)*g5dg2*eccdp)-((a22*tanI2+(2.0*eccdp2+3.0*(1.0-cn2*cn))*sinI)*(35.0/1152.0)*a8p);
    Real    b4=cn*eccdp*(a1-a2);
    Real    b5=((9.0*eccdp2+4.0)*a10*a4*(5.0/64.0)+a7)*cn;
    Real    b6=(35.0/384.0)*a8*cn2*cn*sinI;
    Real    b7=((cn2*a18)/(1.0-5.0*theta2))*((1.0/8.0)*gmp2*(1.0-15.0*theta2)+(1.0-7.0*theta2)*g4dg2*(-(5.0/12.0)));
    Real    b8=(5.0/64.0)*(a3*cn2*(1.0-9.0*theta2-(24.0*theta4/(1.0-5.0*theta2))))+a6*cn2;
    Real    b9=a8*(35.0/384.0)*cn2;
    Real    b10=sinI*(a22*a26*g4dg2*(5.0/12.0)-a27*gmp2);
    Real    b11=a21*(a5*(5.0/64.0)+a6+a3*a26*(15.0/32.0)*sinI*sinI);
    Real    b12=-((80.0*a17+32.0*a16+5.0)*(a22*eccdp*sinI*sinI*(35.0/576.0)*g5dg2)+(a8*a21*(35.0/1152.0)));
	
//----------------------------I
// COMPUTE (SEMI-MAJOR AXIS)  I
//----------------------------I
    Real    sma=smadp*(1.0+gm2*((3.0*theta2-1.0)*(eccdp2/(cn2*cn2*cn2))*(cn+(1.0/(1.0+cn)))+((3.0*theta2-1.0)/(cn2*cn2*cn2))*(eccdp*costa)*(3.0+3.0*eccdp*costa+eccdp2*costa2)+3.0*(1.0-theta2)*adr3*cs2gta));
    Real    sn2gta=Sin(2.0*aopdp+2.0*tadp);
    Real    snf2gd=Sin(2.0*aopdp+tadp);
    Real    csf2gd=Cos(2.0*aopdp+tadp);
    Real    sn2gd=Sin(2.0*aopdp);
    Real    cs2gd=Cos(2.0*aopdp);
    Real    sin3gd=Sin(3.0*aopdp);
    Real    cs3gd=Cos(3.0*aopdp);
    Real    sn3fgd=Sin(3.0*tadp+2.0*aopdp);
    Real    cs3fgd=Cos(3.0*tadp+2.0*aopdp);
    Real    sinGD=Sin(aopdp);
    Real    cosGD=Cos(aopdp);
	
//------------------------I
// COMPUTE (L+G+H) PRIMED I
//------------------------I
	Real	bisubc = pow((1.0-5.0*theta2),-2.0)*((25.0*theta4*theta)*(gmp2*eccdp2));
    Real	blghp;
	Real    eccdpdl;
	Real    dltI;
	Real    sinDH;
	Real    dlt1e;
	if (bisubc >= 0.001)
	{	// modifications for critical inclination
	   if (!criticalInclinationWritten)
	   {
         MessageInterface::ShowMessage(
               "Warning: Mean inclination is close to critical "
               "inclination 63 or 117 DEG. There is a possible inaccuracy.\n");
         criticalInclinationWritten = true;
	   }
		dlt1e = 0.0;
		blghp = 0.0;
		eccdpdl= 0.0;
		dltI = 0.0;
		sinDH = 0.0;
	} 
	else
	{
		blghp=raandp+aopdp+meanAnom+b3*cs3gd+b1*sn2gd+b2*cosGD;
		blghp = Mod(blghp,(TWO_PI));
		if (blghp < 0.0)
		{ 
			blghp=blghp+(TWO_PI);
		}
		dlt1e=b14*sinGD+b13*cs2gd-b15*sin3gd;
		eccdpdl=b4*sn2gd-b5*cosGD+b6*cs3gd-(1.0/4.0)*cn2*cn*gmp2*(2.0*(3.0*theta2-1.0)*(adr2*cn2+adr+1.0)*sinta+3.0*(1.0-theta2)*((-adr2*cn2-adr+1.0)*snf2gd+(adr2*cn2+adr+(1.0/3.0))*sn3fgd));
		dltI=(1.0/2.0)*theta*gmp2*sinI*(eccdp*cs3fgd+3.0*(eccdp*csf2gd+cs2gta))-(a21/cn2)*(b8*sinGD+b7*cs2gd-b9*sin3gd);
		sinDH=(1.0/cosI2)*((1.0/2.0)*(b12*cs3gd+b11*cosGD+b10*sn2gd-((1.0/2.0)*gmp2*theta*sinI*(6.0*(eccdp*sinta-meanAnom+tadp)-(3.0*(sn2gta+eccdp*snf2gd)+eccdp*sn3fgd)))));
	}
    
//-----------------I
// COMPUTE (L+G+H) I
//-----------------I
	Real    blgh=blghp+((1.0/(cn+1.0))*(1.0/4.0)*eccdp*gmp2*cn2*(3.0*(1.0-theta2)*(sn3fgd*((1.0/3.0)+adr2*cn2+adr)+snf2gd*(1.0-(adr2*cn2+adr)))+2.0*sinta*(3.0*theta2-1.0)*(adr2*cn2+adr+1.0)))
            +gmp2*(3.0/2.0)*((-2.0*theta-1.0+5.0*theta2)*(eccdp*sinta+tadp-meanAnom))+(3.0+2.0*theta-5.0*theta2)*(gmp2*(1.0/4.0)*(eccdp*sn3fgd+3.0*(sn2gta+eccdp*snf2gd)));
    blgh = Mod(blgh,(TWO_PI));
    if (blgh < 0.0)
	{
		blgh=blgh+(TWO_PI);
	}
    
    Real    dlte=dlt1e+((1.0/2.0)*cn2*((3.0*(1.0/(cn2*cn2*cn2))*gm2*(1.0-theta2)*cs2gta*(3.0*eccdp*costa2+3.0*costa+eccdp2*costa*costa2+eccdp))
            -(gmp2*(1.0-theta2)*(3.0*csf2gd+cs3fgd))+(3.0*theta2-1.0)*gm2*(1.0/(cn2*cn2*cn2))*(eccdp*cn+(eccdp/(1.0+cn))+3.0*eccdp*costa2+3.0*costa+eccdp2*costa*costa2)));
    Real    eccdpdl2=eccdpdl*eccdpdl;
    Real    eccdpde2=(eccdp+dlte)*(eccdp+dlte);

//-------------------------I
// COMPUTE ECC             I
//-------------------------I
    Real    ecc=sqrt(eccdpdl2+eccdpde2);
    Real    sinDH2=sinDH*sinDH;
    Real    squar=(dltI*cosI2*(1.0/2.0)+sinI2)*(dltI*cosI2*(1.0/2.0)+sinI2);
    Real    sqrI=sqrt(sinDH2+squar);
	
//--------------------------I
// COMPUTE (INCLINATION) I
//--------------------------I
    Real    inc=2*ASin(sqrI);
    inc = Mod(inc,(TWO_PI));
	
//-------------------------I
// COMPUTE (MEAN ANOMALY), (LONGITUDE OF ASCENDING NODE) and (ARGUMENT  OF PERIGEE) I
//-------------------------I
	Real ma;
	Real	raan;
	Real	aop;
	if (ecc <= 1.0E-11)
	{
		aop=0.0;
		if (inc <= 1.0E-7) 
		{    
			raan=0.0;
			ma = blgh;
		} 
			else
		{
			Real    arg1=sinDH*cosraandp+sinraandp*((1.0/2.0)*dltI*cosI2+sinI2);
			Real    arg2=cosraandp*((1.0/2.0)*dltI*cosI2+sinI2)-(sinDH*sinraandp);
			raan=ATan2(arg1,arg2);
			ma = blgh-aop-raan;
		}
	} 
	else
	{
		Real    arg1=eccdpdl*cosMADP+(eccdp+dlte)*sinMADP;
		Real    arg2=(eccdp+dlte)*cosMADP-(eccdpdl*sinMADP);
		ma =ATan2(arg1,arg2);
		ma=Mod(ma,(TWO_PI));
		
		if (inc <= 1.0E-7) 
		{    
			raan=0.0;
			aop = blgh - raan - ma;
		} 
			else
		{
			Real    arg1=sinDH*cosraandp+sinraandp*((1.0/2.0)*dltI*cosI2+sinI2);
			Real    arg2=cosraandp*((1.0/2.0)*dltI*cosI2+sinI2)-(sinDH*sinraandp);
			raan=ATan2(arg1,arg2);
			aop = blgh - ma - raan;
		}
	}
    if (ma < 0.0)
	{
		ma = ma + TWO_PI;
	}

    raan=Mod(raan,(TWO_PI));
    if (raan < 0)
	{
		raan=raan+(TWO_PI);
	}
		 
    aop=Mod(aop,(TWO_PI));
	if (aop < 0.0)
	{
		aop = aop + TWO_PI;
	}

	Real kepl[6];
	kepl[0]	=	sma*re;
	kepl[1]	=	ecc;
	kepl[2]	=	inc*DEG_PER_RAD;
	kepl[3]	=	raan*DEG_PER_RAD;
	kepl[4]	=	aop*DEG_PER_RAD;
	kepl[5]	=	ma*DEG_PER_RAD;

	if (pseudostate != 0)
	{
		kepl[2] = 180.0 - kepl[2];
		kepl[3] = 360.0 - kepl[3];
	}

	return kepl;
}

//------------------------------------------------------------------------------
// Rvector6 BrouwerMeanLongToCartersian(Real mu, const Rvector6& blms)
//------------------------------------------------------------------------------
/**
 * Converts from Brouwer-Lyddane Mean Elements to Cartesian.
 *
 * @param <blms>    Brouwer-Lyddane Mean Elements
 *
 * @return Spacecraft orbit state converted from BrouwerMeanLong to Cartesian
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::BrouwerMeanLongToCartesian(Real mu, const Rvector6& blms)
{ 
	Rvector6 kepl=BrouwerMeanLongToOsculatingElements(mu, blms);

	AnomalyType type = GetAnomalyType("MA");
	Rvector6 cart=KeplerianToCartesian(mu, kepl, type);
	return cart;
}
// End of new code by YK 2014.03.29



// Alternate Equinoctial by HYKim 
//------------------------------------------------------------------------------
// Rvector6 EquinoctialToAlternateEquinoctial(const Rvector6& equinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from Equinoctial to AlternateEquinoctial.
 *
 * @param <equinoctial>     Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Equinoctial to AlternateEquinoctial
 */
//---------------------------------------------------------------------------
 Rvector6 StateConversionUtil::EquinoctialToAltEquinoctial(const Rvector6& equinoctial)
{
   Real sma    = equinoctial[0];   // semi major axis
   Real h      = equinoctial[1];   // equinoctial h
   Real k      = equinoctial[2];   // equinoctial k
   Real p      = equinoctial[3];   // equinoctial p
   Real q      = equinoctial[4];   // equinoctial q
   Real lambda = equinoctial[5]*RAD_PER_DEG;   // mean longitude

   // Find the inclination
   Real i = 2 * ATan (sqrt( p * p + q * q));
   if (GmatMathUtil::IsEqual(i,PI) ) 
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Alternate Equinoctial elements: ";
      errmsg << "Conversion result is near singularity that occurs when i=180deg ";
      throw UtilityException(errmsg.str());
   }
   

   // Compute Alternate Equinoctial elements

   /*
   Real a    = sma;            // semimajor axis
   Real e1   = h;              // Equinoctial H
   Real e2   = k;              // Equinoctial K
   */

   Real altp   = p * Cos(i/2);   // variation of Equinoctial P
   Real altq   = q * Cos(i/2);   // variation of Equinoctial Q
 
   /* 
   Real e5   = lambda;         // mena longitude      
   */
   
   // Return the Alternate Equinoctial elements
  return Rvector6(equinoctial[0], equinoctial[1], equinoctial[2], altp, altq, equinoctial[5]);
}

  
// Alternate Equinoctial by HYKim 
//------------------------------------------------------------------------------
// Rvector6 AlternateEquinoctialToEquinoctial(const Rvector6& equinoctial, const Real& mu)
//------------------------------------------------------------------------------
/**
 * Converts from AlternateEquinoctial to Equinoctial.
 *
 * @param <Altequinoctial>  Alternate Equinoctial state
 * @param <mu>              Gravitational constant for the central body
 *
 * @return Spacecraft orbit state converted from Equinoctial to AlternateEquinoctial
 */
//---------------------------------------------------------------------------
Rvector6 StateConversionUtil::AltEquinoctialToEquinoctial(const Rvector6& Altequinoctial)
{
   Real sma    = Altequinoctial[0];                // semi major axis
   Real h      = Altequinoctial[1];                // equinoctial H
   Real k      = Altequinoctial[2];                // equinoctial K
   Real altp   = Altequinoctial[3];                // alternate equinoctial P
   Real altq   = Altequinoctial[4];                // alternate equinoctial Q
   Real lambda = Altequinoctial[5]*RAD_PER_DEG;    // mean longitude

   // Find the inclination
   Real i = 2 * ASin (sqrt( altp * altp + altq * altq));
   if (GmatMathUtil::IsEqual(i,PI))
   {
      std::stringstream errmsg("");
      errmsg.precision(15);
      errmsg << "Error in conversion from Equinoctial to Alternate Equinoctial elements: ";
      errmsg << "Conversion result is near singularity that occurs when i=180deg ";
      throw UtilityException(errmsg.str());
   }
   

   // Compute Alternate Equinoctial elements

   /*
   Real sma    = a;             // semimajor axis
   Real h      = e1;            // Equinoctial H
   Real k      = e2;            // Equinoctial K
   */

   Real p      = altp/Cos(i/2);   // Equinoctial P
   Real q      = altq/Cos(i/2);   // Equinoctial Q

   /*
   Real lambda = e5;            // mena longitude      
   */
   // Return the Alternate Equinoctial elements
   return Rvector6(Altequinoctial[0], Altequinoctial[1], Altequinoctial[2], p, q, Altequinoctial[5]);
}


//------------------------------------------------------------------------------
// anomaly conversion methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real TrueToMeanAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes mean anomaly from true anomaly
 *
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return Mean anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::TrueToMeanAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToMeanAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ma = 0.0;

   if (ecc < (1.0 - GmatOrbitConstants::KEP_TOL))
   {
      Real ea = TrueToEccentricAnomaly(taRadians, ecc);
      ma = ea - ecc * Sin(ea);
      // Only mod it to be between 0 and twopi when the orbit is elliptical
      if (ma < 0.0)
         ma = ma + TWO_PI;
      if (modBy2Pi)
      {
         while (ma > TWO_PI)
            ma -= TWO_PI;
      }
   }
   else if (ecc > (1.0 + GmatOrbitConstants::KEP_TOL))
   {
      Real ha = TrueToHyperbolicAnomaly(taRadians, ecc);
      ma = ecc * Sinh(ha) - ha;
   }
   else
   {
      std::string warn =
         "Warning: Orbit is near parabolic in mean anomaly calculation.  ";
      warn += "Setting MA = 0\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
      ma = 0.0;
   }

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToMeanAnomaly() returning %f\n", ma);
   #endif

   return ma;
}


//------------------------------------------------------------------------------
// Real TrueToEccentricAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes eccentric anomaly from true anomaly
 *
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return Eccentric anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::TrueToEccentricAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToEccentricAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ea = 0.0;

   if (ecc <= (1.0 - GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real cosTa = Cos(taRadians);
      Real eccCosTa = ecc * cosTa;

      Real sinEa = (Sqrt(1.0 - ecc*ecc) * Sin(taRadians)) / (1.0 + eccCosTa);
      Real cosEa = (ecc + cosTa) / (1.0 + eccCosTa);
      ea = ATan2(sinEa, cosEa);
   }

   if (ea < 0.0)
      ea = ea + TWO_PI;
   if (modBy2Pi)
   {
      while (ea > TWO_PI)
         ea -= TWO_PI;
   }

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToEccentricAnomaly() returning %f\n", ea);
   #endif

   return ea;
}


//------------------------------------------------------------------------------
// Real TrueToHyperbolicAnomaly(Real taRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/**
 * Converts true anomaly to hyperbolic anomaly.
 *
 * @param  taRadians  True anomaly in radians
 * @param  ecc        Eccentricity
 * @param  modBy2Pi   Flag specifying whether or not to mod the result by two pi
 *
 * @return  Hyperbolic anomaly in radians, 0.0 if eccentricity is less than 1.0 + KEP_TOL
 *
 * @exception  UtilityException is throw if "eccentricity * cos(trueAnomaly)" is -1.
 */
//------------------------------------------------------------------------------
Real StateConversionUtil::TrueToHyperbolicAnomaly(Real taRadians, Real ecc, bool modBy2Pi)
{
   #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() ta=%f, ecc=%f\n", taRadians, ecc);
   #endif

   Real ha = 0.0;

   if (ecc >= (1.0 + GmatOrbitConstants::KEP_TOL))
   {
      Real tanhHa2 = Tan(taRadians / 2) * Sqrt((ecc - 1.0) / (ecc + 1.0));
      ha = 2 * ATanh(tanhHa2);

      //Real cosTa = Cos(taRadians);
      //Real eccCosTa = ecc * cosTa;
      //if (eccCosTa == -1.0)
      //{
      //   throw UtilityException("StateConversionUtil::TrueToEccentricAnomaly - error converting - divide by zero.\n");
      //}

      //Real sinhHa = (Sin(taRadians) * Sqrt(ecc*ecc - 1.0)) / (1.0 + eccCosTa);
      //ha = ASinh(sinhHa);
   }
#ifdef DEBUG_ANOMALY
   else
   {
      MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() ecc < (1.0 + KEP_TOL), returning ha = 0.0\n");
   }
#endif

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() returning %f\n", ha);
   #endif

   //if (modBy2Pi)
   //{
   //   while (ha > TWO_PI)
   //      ha -= TWO_PI;
   //   while (ha < 0.0)
   //      ha += TWO_PI;
   //}

   return ha;
}


//------------------------------------------------------------------------------
// Real MeanToTrueAnomaly(Real maRadians, Real ecc, Real tol = 1.0e-08)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from mean anomaly
 *
 * @param <maRadians>     Mean anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <tol>           Tolerance value
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::MeanToTrueAnomaly(Real maRadians, Real ecc, Real tol)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("MeanToTrueAnomaly() maRadians=%.10f, ecc=%.10f, tol=%.15f\n", maRadians, ecc, tol);
   #endif
   
   Real ta;
   Integer iter;
   Integer ret;

   ret = ComputeMeanToTrueAnomaly(maRadians, ecc, tol, &ta, &iter);
   
   if (ret == 0)
   {
      #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("MeanToTrueAnomaly() returning %f\n", ta);
      #endif

      return ta;
   }
   
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("MeanToTrueAnomaly() Throwing a exception - error converting MA to TA\n");
   #endif
   throw UtilityException("MeanToTrueAnomaly() Error converting "
                          " Mean Anomaly to True Anomaly\n");
}

//------------------------------------------------------------------------------
// Real EccentricToTrueAnomaly(Real eaRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from eccentric anomaly
 *
 * @param <eaRadians>     Eccentric anomaly in radians
 * @param <ecc>           Eccentricity
 * @param  <modBy2Pi>     Flag specifying whether or not to mod the result by two pi
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::EccentricToTrueAnomaly(Real eaRadians, Real ecc, bool modBy2Pi)
{
   Real e2            = ecc * ecc;
   Real cosE          = Cos(eaRadians);
   Real oneMinusECosE = 1.0 - ( ecc * cosE);
   if (oneMinusECosE == 0.0)
   {
      throw UtilityException("StateConversionUtil::EccentricToTrueAnomaly - error converting - divide by zero.\n");
   }

   Real sinTA = (Sqrt(1.0 - e2) * Sin(eaRadians)) / oneMinusECosE;
   Real cosTA = (cosE - ecc) / oneMinusECosE;
   Real ta    =  ATan2(sinTA, cosTA);
   if (modBy2Pi)
   {
      while (ta)
         ta -= TWO_PI;
   }
   return ta;
}

//------------------------------------------------------------------------------
// Real HyperbolicToTrueAnomaly(Real haRadians, Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from hyperbolic anomaly
 *
 * @param <eaRadians>     Hyperbolic anomaly in radians
 * @param <ecc>           Eccentricity
 * @param  <modBy2Pi>     Flag specifying whether or not to mod the result by two pi
 *
 * @return True anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::HyperbolicToTrueAnomaly(Real haRadians, Real ecc, bool modBy2Pi)
{
   Real e2             = ecc * ecc;
   Real coshH          = Cosh(haRadians);
   Real oneMinusECoshH = 1.0 - (ecc * coshH);
   if (oneMinusECoshH == 0.0)
   {
      throw UtilityException("StateConversionUtil::HyperbolicToTrueAnomaly - error converting - divide by zero.\n");
   }

   Real sinTA = (Sqrt(e2 - 1.0) * Sinh(haRadians)) / oneMinusECoshH;
   Real cosTA = (coshH - ecc) / oneMinusECoshH;
   Real ta    = ATan2(sinTA, cosTA);

   if (modBy2Pi)
   {
      while (ta)
         ta -= TWO_PI;
   }
   return ta;
}

//------------------------------------------------------------------------------
// Real ConvertFromTrueAnomaly(const std::string &toType, Real taRadians,
//                                    Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts from true anomaly to specified anomaly type
 *
 * @param <toType>        Anomaly type to convert to
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  anomaly of specified type
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertFromTrueAnomaly(const std::string &toType, Real taRadians, Real ecc, bool modBy2Pi)
{
   AnomalyType anomType = GetAnomalyType(toType);
   return ConvertFromTrueAnomaly(anomType, taRadians, ecc, modBy2Pi);
}

//------------------------------------------------------------------------------
// Real ConvertFromTrueAnomaly(AnomalyType toType, Real taRadians,
//                                    Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts from true anomaly to specified anomaly type
 *
 * @param <toType>        Anomaly type to convert to
 * @param <taRadians>     True anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  anomaly of specified type
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertFromTrueAnomaly(AnomalyType toType,
                          Real taRadians, Real ecc, bool modBy2Pi)
{
   switch (toType)
   {
   case TA:
      return taRadians;
   case MA:
      return TrueToMeanAnomaly(taRadians, ecc, modBy2Pi);
   case EA:
      return TrueToEccentricAnomaly(taRadians, ecc, modBy2Pi);
   case HA:
      return TrueToHyperbolicAnomaly(taRadians, ecc, modBy2Pi);
   default:
      throw UtilityException("ERROR converting to true anomaly - unknown anomaly type.\n");
   }
}

//------------------------------------------------------------------------------
// Real ConvertToTrueAnomaly(const std::string &fromType, Real taRadians,
//                                  Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts to true anomaly from input anomaly type
 *
 * @param <fromType>      Anomaly type to convert from
 * @param <taRadians>     Anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertToTrueAnomaly(const std::string &fromType, Real taRadians, Real ecc, bool modBy2Pi)
{
   AnomalyType anomType = GetAnomalyType(fromType);
   return ConvertToTrueAnomaly(anomType, taRadians, ecc, modBy2Pi);
}

//------------------------------------------------------------------------------
// Real ConvertToTrueAnomaly(AnomalyType fromType, Real taRadians,
//                                  Real ecc, bool modBy2Pi = false)
//------------------------------------------------------------------------------
/*
 * Converts to true anomaly from input anomaly type
 *
 * @param <fromType>      Anomaly type to convert from
 * @param <taRadians>     Anomaly in radians
 * @param <ecc>           Eccentricity
 * @param <modBy2Pi>      Flag specifying whether or not to mod the result by two pi
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::ConvertToTrueAnomaly(AnomalyType fromType, Real taRadians, Real ecc, bool modBy2Pi)
{
   switch (fromType)
   {
   case TA:
      return taRadians;
   case MA:
      return MeanToTrueAnomaly(taRadians, ecc);
   case EA:
      return EccentricToTrueAnomaly(taRadians, ecc, modBy2Pi);
   case HA:
      return HyperbolicToTrueAnomaly(taRadians, ecc, modBy2Pi);
   default:
      throw UtilityException("ERROR converting to true anomaly - unknown anomaly type.\n");
   }
}


//------------------------------------------------------------------------------
// Real CartesianToTA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  true anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToTA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage
      ("CartesianToTA() mu=%f, inRadian=%d\n   pos =%s\n   vel =%s\n", mu, inRadian,
       pos.ToString().c_str(), vel.ToString().c_str());
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   
   // if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   // {
   //    throw UtilityException
   //       ("Error in conversion to Keplerian state: "
   //        "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   // }
   
   Real ecc = eVec.GetMagnitude();
   Real rMag = pos.GetMagnitude();
   Real ta = 0.0;

   if (rMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to TA - position vector is a zero vector\n");
   }

   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage
      ("   eVec=%s,\n   inc=%f, ecc=%f, rMag=%f, ta=%f\n",
       eVec.ToString().c_str(), inc, ecc, rMag, ta);
   #endif

   // Case 1:  Non-circular, Inclined Orbit
   //if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 1:  Non-circular, Inclined Orbit\n");
      #endif

      Real temp = (eVec*pos) / (ecc*rMag);
      ta = ACos(temp, GmatOrbitConstants::KEP_TOL);

      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ACos(temp, GmatOrbitConstants::KEP_TOL));
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ta);
      #endif

      // Fix quadrant
      if ((pos*vel) < 0.0)
      {
         ta = TWO_PI - ta;
         #ifdef DEBUG_KEPLERIAN_TA
         MessageInterface::ShowMessage("   after quadrant fix, ta=%f\n", ta);
         #endif
      }
   }
   // Case 2: Non-circular, Equatorial Orbit
   //else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 2: Non-circular, Equatorial Orbit\n");
      #endif

      //ta = ACos((eVec*pos) / (ecc*rMag));
      ta = ACos(((eVec*pos) / (ecc*rMag)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos*vel) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 3: Circular, Inclined Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 3: Circular, Inclined Orbit\n");
      #endif

      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to TA - n vector is a zero vector.\n");
      }
      //ta = ACos((nVec*pos) / (nMag*rMag));
      ta = ACos(((nVec*pos) / (nMag*rMag)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos[2]) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 4: Circular, Equatorial Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 4: Circular, Equatorial Orbit\n");
      #endif

      //ta = ACos(pos[0] / rMag);
      ta = ACos((pos[0] / rMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if ((pos[1]) < 0.0)
         ta = TWO_PI - ta;
   }

   // Convert 2pi to 0
   ta = Mod(ta, TWO_PI);

   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage("CartesianToTA() returning %f\n", ta);
   #endif

   if (inRadian)
      return ta;
   else
      return ta * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Real CartesianToMA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes mean anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  mean anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToMA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("CartesianToMA() ");
   #endif

   Real ta  = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ma  = TrueToMeanAnomaly(ta, ecc);

   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("returning %f\n", ma);
   #endif

   if (inRadian)
      return ma;
   else
      return ma * DEG_PER_RAD;
}



//------------------------------------------------------------------------------
// Real CartesianToEA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes eccentric anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  eccentric anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToEA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("CartesianToEA() ");
   #endif

   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ea = TrueToEccentricAnomaly(ta, ecc);

   #ifdef DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("returning %f\n", ea);
   #endif

   if (inRadian)
      return ea;
   else
      return ea * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Real CartesianToHA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes hyperbolic anomaly from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  hyperbolic anomaly
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToHA(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("CartesianToHA() ");
   #endif

   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ha = TrueToHyperbolicAnomaly(ta, ecc);

   #ifdef DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("returning %f\n", ha);
   #endif

   if (inRadian)
      return ha;
   else
      return ha * DEG_PER_RAD;

}


//------------------------------------------------------------------------------
// other conversion methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real CartesianToSMA(Real mu, const Rvector3 &pos,
//                     const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes semimajor axis from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  SMA
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToSMA(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("CartesianToSMA() ");
   #endif

   Real rMag = pos.GetMagnitude(); // ||r||
   Real vMag = vel.GetMagnitude(); // ||v||

   if (rMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to SMA - position vector is a zero vector.\n");
   }

   Real zeta = 0.5*(vMag*vMag) - mu/rMag;

   if (zeta == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to SMA - computed zeta is zero.\n");
   }

   // check if the orbit is near parabolic
   Real ecc = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - ecc)) <= GmatOrbitConstants::KEP_ECC_TOL)
   {

      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in an orbit that is nearly parabolic.\n");
   }

   Real sma = -mu/(2*zeta);

   // Check for a singular conic section
   if (Abs(sma*(1 - ecc) < .001))
   {
      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }


   #ifdef DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("returning %f\n", sma);
   #endif

   return sma;
}

//------------------------------------------------------------------------------
// Real CartesianToECC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes eccentricity from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  Eccentricity
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToECC(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("CartesianToECC() ");
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real eMag = eVec.GetMagnitude(); // ||e||

   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("returning %f\n", eMag);
   #endif

   return eMag;
}


//------------------------------------------------------------------------------
// Real CartesianToINC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes inclination from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  inclination
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToINC(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("CartesianToINC() ");
   #endif

   // check if the orbit is near parabolic
   Real eMag = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - eMag)) <= GmatOrbitConstants::KEP_ZERO_TOL)
   {

      throw UtilityException
      ("Error in conversion from Cartesian to Keplerian state: "
       "The state results in an orbit that is nearly parabolic.\n");
   }

   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();

   if (hMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to INC - angular momentum is a zero vector.\n");
   }

   Real inc = ACos((hVec[2] / hMag), GmatOrbitConstants::KEP_TOL);
   
   // if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   // {
   //    throw UtilityException
   //       ("Error in conversion to Keplerian state: "
   //        "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   // }
   
   #ifdef DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("returning %f\n", inc);
   #endif

   if (inRadian)
      return inc;
   else
      return inc * DEG_PER_RAD;
}

//------------------------------------------------------------------------------
// Real CartesianToRAAN(Real mu, const Rvector3 &pos,
//                             const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes right ascension of the ascending node from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  RAAN
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToRAAN(Real mu, const Rvector3 &pos,
                                          const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("CartesianToRAAN() ");
   #endif

   Real ecc = CartesianToECC(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   // if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   // {
   //    throw UtilityException
   //       ("Error in conversion to Keplerian state: "
   //        "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   // }
   Real raan = 0.0;

   // Case 1:  Non-circular, Inclined Orbit
   //if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to RAAN - n vector is a zero vector.\n");
      }
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;
   }
   // Case 2: Non-circular, Equatorial Orbit
   //else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      raan = 0.0;
   }
   // Case 3: Circular, Inclined Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if (nMag == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to RAAN - n vector is a zero vector.\n");
      }
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;
   }
   // Case 4: Circular, Equatorial Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      raan = 0.0;
   }

   #ifdef DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("returning %f\n", raan);
   #endif

   // Convert 2pi to 0
   raan = Mod(raan, TWO_PI);

   if (inRadian)
      return raan;
   else
      return raan * DEG_PER_RAD;

}


//------------------------------------------------------------------------------
// Real CartesianToAOP(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
/*
 * Computes argument of periapsis from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 * @param <inRadian>      Flag specifying whether the result should be in
 *                        radians (true) or degrees (false)
 *
 * @return  AOP
 */
//---------------------------------------------------------------------------
Real StateConversionUtil::CartesianToAOP(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("CartesianToAOP() ");
   #endif

   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   // if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   // {
   //    throw UtilityException
   //       ("Error in conversion to Keplerian state: "
   //        "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   // }
   Real ecc = eVec.GetMagnitude();
   Real aop = 0.0;

   // Case 1:  Non-circular, Inclined Orbit
   //if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      if ((nMag == 0.0) || (ecc == 0.0))
      {
         throw UtilityException("Cannot convert from Cartesian to AOP - n vector is a zero vector or eccentricity is zero.\n");
      }
      aop = ACos(((nVec*eVec) / (nMag*ecc)), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (eVec[2] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 2: Non-circular, Equatorial Orbit
   //else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      if (ecc == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to AOP - eccentricity is zero.\n");
      }
      aop = ACos((eVec[0] / ecc), GmatOrbitConstants::KEP_TOL);

      // Fix quadrant
      if (eVec[1] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 3: Circular, Inclined Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc >= GmatOrbitConstants::KEP_TOL) && (inc <= PI - GmatOrbitConstants::KEP_TOL)))
   {
      aop = 0.0;
   }
   // Case 4: Circular, Equatorial Orbit
   //else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && ((inc < GmatOrbitConstants::KEP_TOL) || (inc > PI - GmatOrbitConstants::KEP_TOL)))
   {
      aop = 0.0;
   }

   #ifdef DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("returning %f\n", aop);
   #endif

   // Convert 2pi to 0
   aop = Mod(aop, TWO_PI);

   if (inRadian)
      return aop;
   else
      return aop * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// Rvector3 CartesianToEccVector(Real mu, const Rvector3 &pos,
//                                      const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes eccentricity vector from input cartesian state
 *
 * @param <mu>            Gravitational constant for the central body
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  ECC vector
 */
//---------------------------------------------------------------------------
Rvector3 StateConversionUtil::CartesianToEccVector(Real mu, const Rvector3 &pos,
                                                   const Rvector3 &vel)
{
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();
   if ((mu == 0.0) || (rMag == 0.0))
   {
      throw UtilityException("Cannot convert from Cartesian to EccVector - position vector is a zero vector or mu is zero.\n");
   }

   Rvector3 eVec = ((vMag*vMag - mu/rMag)*pos - (pos*vel)*vel) / mu;

   #ifdef DEBUG_ECC_VEC
   MessageInterface::ShowMessage
      ("CartesianToEccVector() returning\n   %s\n", eVec.ToString().c_str());
   #endif

   return eVec;
}

//------------------------------------------------------------------------------
// Rvector3 CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
/*
 * Computes direction of line of nodes from input cartesian state
 *
 * @param <pos>           Cartesian position
 * @param <vel>           Cartesian velocity
 *
 * @return  Dir of line of nodes
 */
//---------------------------------------------------------------------------
Rvector3 StateConversionUtil::CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
{
   Rvector3 hVec = Cross(pos, vel);
   Rvector3 nVec = Cross(Rvector3(0, 0, 1), hVec);
   return nVec;
}

//------------------------------------------------------------------------------
// Rvector6 CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
//                                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * CartesianTos angular momentum, its magnitude, and other related data.
 *
 * @param <mu>  gravitational constant of the central body
 * @param <pos> input position vector
 * @param <vel> input velocity vector
 *
 * @return [0] Angular momentum x unit vector
 *         [1] Angular momentum y unit vector
 *         [2] Angular momentum z unit vector
 *         [3] Angular momentum magnitude
 *         [4] velocity magnitude squared
 *         [5] Orbit parameter
 */
//------------------------------------------------------------------------------
Rvector6 StateConversionUtil::CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
                                                         const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("CartesianToAngularMomentum() ");
   #endif

   Real vMag = vel.GetMagnitude();
   Real vMagSq = vMag*vMag;
   if (mu == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Angular Momentum - mu is zero.\n");
   }

   Rvector3 hVec = Cross(pos, vel);
   Real hMag = Sqrt(hVec * hVec);
   Real orbParam = (hMag*hMag) / mu;
   hVec.Normalize();

   Rvector6 h(hVec[0], hVec[1], hVec[2], hMag, vMagSq, orbParam);

   #ifdef DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("returning %s\n", h.ToString().c_str());
   #endif

   return h;
}


//------------------------------------------------------------------------------
// derivative conversion methods
//------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianDerivativeConversion(Real mu, const Rvector6 &cartesianState)
//--------------------------------------------------------------------------------------
/**
* This function is used to calculate Cartesian to Keplerian derivative state conversion matrix.
* Only apply for Keplerian state presented in mean anomaly only. (GTDS MathSpec Equation 3-229)
*
* @param mu                 mu value of primary body
* @param cartesianState     state presented in Cartesian coordiante system
*
* return                    derivative state conversion matrix [dX/dK] where X is 
*                           Cartesian state and K is Keplerian state
*/
//--------------------------------------------------------------------------------------
Rmatrix66 StateConversionUtil::CartesianToKeplerianDerivativeConversion(
   Real mu, const Rvector6 &cartesianState)
{
   static Rmatrix66 result;

   // 1. Convert Cartesian state to Keplerian state (angles in degree)
   Rvector6 keplerState = StateConversionUtil::CartesianToKeplerian(mu, cartesianState, StateConversionUtil::MA);

   // 2. Convert INC, RAAN, AOP, MA from degree to radian
   for (UnsignedInt i = 2; i < 6; ++i)
      keplerState[i] = keplerState[i] * GmatMathConstants::RAD_PER_DEG;

   // 3. Calculate derivative conversion matrix
   if ((keplerState[1] >= 0.0) && (keplerState[1] < 1.0))
   {
      // Use analytical solution for Ellipse case
      result = CartesianToKeplerianDerivativeConversionWithKeplInput(mu, keplerState);        // It is the matrix [dX/dK] where X is Cartesian state and K is Keplerian state
   }
   else if (keplerState[1] >= 1.0)
   {
      /// Use finite difference method for Hyperbolic and Parabolic case.
      // The step to convert from degree to radian for all angles is not needed due to 
      // the all angles of the input state fo the function KeplerianToCartesianDerivativeConversion_FiniteDiff() is in degree
      //result = CartesianToKeplerianDerivativeConversion_FiniteDiff(mu, cartesianState);
      result = CartesianToKeplerianDerivativeConversionWithKeplInput_FiniteDiff(mu, keplerState);
   }
   else
      throw UtilityException("Error: Eccentricity (" + GmatStringUtil::ToString(keplerState[1]) + ") is out of range [0,inf)\n");
   
   return result;
}


//--------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianDerivativeConversion_FiniteDiff(Real mu, const Rvector6 &cartesianState)
//--------------------------------------------------------------------------------------
/**
* This function is used to calculate derivative conversion matrix [dX/dK] where X is
* Cartesian state and K is Keplerian state using finite difference method
*
* @param mu                 mu value of primary body
* @param cartesianState     state presented in Cartesian coordiante system
*
* return                    derivative state conversion matrix [dX/dK] where X is
*                           Cartesian state and K is Keplerian state
*/
//--------------------------------------------------------------------------------------
Rmatrix66 StateConversionUtil::CartesianToKeplerianDerivativeConversion_FiniteDiff(
   Real mu, const Rvector6 &cartesianState)
{
   static Rmatrix66 result;
   Rmatrix66 dKdX;                   // It is matrix [dK/dX]
   
   // 1. Convert Cartesian state to Keplerian state (angles in degree)
   Rvector6 K0 = StateConversionUtil::CartesianToKeplerian(mu, cartesianState, StateConversionUtil::MA);

   // 2. Calculate derivative conversion matrix
   if (K0[1] >= 0.0)
   {
      //result = KeplerianToCartesianDerivativeConversion_FiniteDiff(mu, keplerState);            // It is the matrix [dX/dK] where X is Cartesian state and K is Keplerian state

      for (Integer col = 0; col < 6; ++col)
      {
         Rvector6 X0 = cartesianState;
         Rvector6 X1 = cartesianState;
         //if (col == 1)
         //   K1[col] = K1[col] * 1.000001;
         //else
         //   K1[col] = K1[col] * 1.00000001;
         X1[col] = X1[col] * 1.000001;

         Rvector6 K1 = CartesianToKeplerian(mu, X1, StateConversionUtil::MA);
         Rvector6 dK = K1 - K0;

         for (Integer row = 0; row < 6; ++row)
         {
            //if (col == 1)
            //   dKdX.SetElement(row, col, dK(row) / (cartesianState[col] * 0.000001));
            //else
            //   dKdX.SetElement(row, col, dK(row) / (cartesianState[col] * 0.00000001));
            dKdX.SetElement(row, col, dK(row) / (cartesianState[col] * 0.000001));
         }
      }

      result = dKdX.Inverse();       // It is [dX/dK] = inv([dK/dX])
   }
   else
      throw UtilityException("Error: Eccentricity (" + GmatStringUtil::ToString(K0[1]) + ") is out of range [0,inf)\n");

   return result;
}


//--------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianDerivativeConversionWithKeplInput_FiniteDiff(Real mu, 
//                     const Rvector6 &keplerianState)
//--------------------------------------------------------------------------------------
/**
* This function is used to calculate derivative conversion matrix [dX/dK] where X is
* Cartesian state and K is Keplerian state using finite difference method. Keplerian 
* anomaly is in MA form.
*
* @param mu                 mu value of primary body
* @param keplerianState     state presented in Keplerian coordiante system with anomaly 
*                           in MA form
*
* return                    derivative state conversion matrix [dX/dK] where X is
*                           Cartesian state and K is Keplerian state. Keplerian anomaly 
*                           element is in MA form.
*/
//--------------------------------------------------------------------------------------
Rmatrix66 StateConversionUtil::CartesianToKeplerianDerivativeConversionWithKeplInput_FiniteDiff(
   Real mu, const Rvector6 &keplerianState)
{
   static Rmatrix66 result;

   // 1. Convert Keplerian state (angles in degree) to Cartesian state
   Rvector6 X0 = StateConversionUtil::KeplerianToCartesian(mu, keplerianState, StateConversionUtil::MA);

   // 2. Calculate derivative conversion matrix
   for (Integer col = 0; col < 6; ++col)
   {
      Rvector6 K0 = keplerianState;
      Rvector6 K1 = keplerianState;
      if (col == 1)
         K1[col] = K1[col] * 1.000001;
      else
         K1[col] = K1[col] * 1.00000001;

      Rvector6 X1 = KeplerianToCartesian(mu, K1, StateConversionUtil::MA);
      Rvector6 dX = X1 - X0;

      for (Integer row = 0; row < 6; ++row)
      {
         if (col == 1)
            result.SetElement(row, col, dX(row) / (K0[col] * 0.000001));
         else
            result.SetElement(row, col, dX(row) / (K0[col] * 0.00000001));
      }
   }

   return result;
}


//-------------------------------------------------------------------------------------
// Real CalculateEccentricAnomaly(Real e, Real M)
//-------------------------------------------------------------------------------------
/**
* This function is used to calculate eccentric anomaly E based on values of 
* eccentricity and mean anomaly
*
* @param e       eccentricity
* @param M       mean anomaly
* return         value of ecentric anomaly
*
*/
//-------------------------------------------------------------------------------------
Real StateConversionUtil::CalculateEccentricAnomaly(Real e, Real M)
{
   Real val = 0.0;
   if ((0.0 <= e) && (e < 1.0))
      val = CalculateEccentricAnomalyEllipse(e, M);
   else if (e > 1.0)
      val = CalculateEccentricAnomalyHyperbola(e, M);
   else if (e == 1.0)
   {
      val = CalculateEccentricAnomalyParabola(e, M);
   }

   return val;
}

Real StateConversionUtil::CalculateEccentricAnomalyEllipse(Real e, Real M)
{
   Real tolerance = 1.0e-12;
   Real E, newE, D, f;
   
   newE = M + e*Sin(M);                                  // Equation 3-182 GTDS MathSpec
   E = newE + 1;
   while (Abs(newE - E) > tolerance)
   {
      E = newE;
      f = E - e*Sin(E) - M;                              // Equation 3-179 GTDS MathSpec
      D = 1 - e*Cos(E - 0.5*f);                          // Equation 3-180 GTDS MathSpec
      newE = E - f/D;                                    // Equation 3-181 GTDS MathSpec
   }

   return newE;
}

Real StateConversionUtil::CalculateEccentricAnomalyHyperbola(Real e, Real M)
{
   Real tolerance = 1.0e-12;
   Real E, newE;

   E = M / 2.0;
   newE = E - (e*Sinh(E) - E - M)/(e*Cosh(E) -1);                 // Equation 3-186 GTDS MathSpec
   while (Abs(newE - E) > tolerance)
   {
      E = newE;
      newE = E - (e*Sinh(E) - E - M) / (e*Cosh(E) - 1);           // Equation 3-186 GTDS MathSpec
   }

   return newE;
}


Real StateConversionUtil::CalculateEccentricAnomalyParabola(Real e, Real M)
{
   Real tolerance = 1.0e-12;
   Real E, newE;

   // It is not implemented yet
   newE = 0.0;
   E = 0.0;
   return newE;
}

//--------------------------------------------------------------------------------------
// Rmatrix66 KeplerianToCartesianDerivativeConversion(Real mu, const Rvector6 &keplerState)
// Rmatrix66 CartesianToKeplerianDerivativeConversionWithKeplInput(
//   Real mu, const Rvector6 &keplerState)

//--------------------------------------------------------------------------------------
/**
* This function is used to calculate [dX/dK] derivative conversion matrix for a given Keplerian state.
* Only apply for Keplerian state presented in mean anomaly 
*
* @param mu             mu value of primary body
* @param keplerState    state presented in Keplerian coordiante system
*
* return                derivative state conversion matrix
*/
//--------------------------------------------------------------------------------------
//Rmatrix66 StateConversionUtil::KeplerianToCartesianDerivativeConversion(
//   Real mu, const Rvector6 &keplerState)
Rmatrix66 StateConversionUtil::CartesianToKeplerianDerivativeConversionWithKeplInput(
   Real mu, const Rvector6 &keplerState)
{
   static Rmatrix66 result;
   
   // Specify a,e,M, Ohm, omega, i
   Real a, e, M, Ohm, omega, iAngle;
   a      = keplerState(0);                  // SMA
   e      = keplerState(1);                  // ECC
   iAngle = keplerState(2);                  // INC            // unit: radian
   Ohm    = keplerState(3);                  // RAAN           // unit: radian
   omega  = keplerState(4);                  // AOP            // unit: radian
   M      = keplerState(5);                  // MA             // unit: radian

   if ((e < 0.0)|| e >= 1.0)
      throw UtilityException("Error: Eccentricity (" + GmatStringUtil::ToString(e) + ") is out of range [0,1)\n");

   // Specify E from e and M
   Real E;         // eccentric anomaly
   E = CalculateEccentricAnomaly(e,M);                                     // Equation 3-178 GTDS MathSpec   

   Rvector3 rpVec;
   Rvector3 rpdotVec;
   if ((0.0 <= e) && (e < 1.0))            // Ellipse case
   {
      // Specify rp:                                                          // Equation 3-176 GTDS MathSpec
   rpVec[0] = a*(Cos(E) - e);
   rpVec[1] = a*Sin(E)*Sqrt(1 - e*e);
   rpVec[2] = 0;
   
   // Specify rpdot:                                                       // Equation 3-177 GTDS MathSpec 
   rpdotVec[0] = -Sin(E);
   rpdotVec[1] = Cos(E)*Sqrt(1 - e*e);
   rpdotVec[2] = 0;
   rpdotVec = (Sqrt(mu/a)/(1 - e*Cos(E))) * rpdotVec;
   }
   else if (e > 1.0)                            // Hyperbola case
   {
      // Specify rp:                                                          // Equation 3-183 GTDS MathSpec
      rpVec[0] = a*(Cosh(E) - e);
      rpVec[1] = -a*Sinh(E)*Sqrt(e*e - 1);
      rpVec[2] = 0;

      // Specify rpdot:                                                       // Equation 3-184 GTDS MathSpec 
      rpdotVec[0] = Sinh(E);
      rpdotVec[1] = -Cosh(E)*Sqrt(e*e - 1);
      rpdotVec[2] = 0;
      rpdotVec = (Sqrt(-mu / a) / (e*Cosh(E) - 1)) * rpdotVec;
   }
   
   // Specify P
   Rmatrix33 P;
   P(0,0) = Cos(Ohm)*Cos(omega) - Sin(Ohm)*Cos(iAngle)*Sin(omega);         // Equation 3-192a GTDS MathSpec
   P(0,1) = -Cos(Ohm)*Sin(omega) - Sin(Ohm)*Cos(iAngle)*Cos(omega);        // Equation 3-192b GTDS MathSpec
   P(0,2) = Sin(Ohm)*Sin(iAngle);                                          // Equation 3-192c GTDS MathSpec
   P(1,0) = Sin(Ohm)*Cos(omega) + Cos(Ohm)*Cos(iAngle)*Sin(omega);         // Equation 3-192d GTDS MathSpec
   P(1,1) = -Sin(Ohm)*Sin(omega) + Cos(Ohm)*Cos(iAngle)*Cos(omega);        // Equation 3-192e GTDS MathSpec
   P(1,2) = -Cos(Ohm)*Sin(iAngle);                                         // Equation 3-192f GTDS MathSpec
   P(2,0) = Sin(iAngle)*Sin(omega);                                        // Equation 3-192g GTDS MathSpec
   P(2,1) = Sin(iAngle)*Cos(omega);                                        // Equation 3-192h GTDS MathSpec
   P(2,2) = Cos(iAngle);                                                   // Equation 3-192i GTDS MathSpec
   
   // Specify rVec and rdotVec
   Rvector3 rVec = P * rpVec;                                              // Equation 3-193 GTDS MathSpec
   Rvector3 rdotVec = P * rpdotVec;                                        // Equation 3-194 GTDS MathSpec
   
   // Mean motion n:
   Real n = (1 / a)*Sqrt(mu / a);                                              // Equation 3-201 GTDS MathSpec
   
   // Conversion matrix
   Rmatrix33 c1, c2, A, B;
   // Partial derivative of rp w.r.t. semi-major axis a, eccentricity e, and mean anomaly M (Equation 3-199 GTDS MathSpec)
   c1(0,0) = rpVec[0]/a;
   c1(1,0) = rpVec[1]/a;
   c1(2,0) = 0.0;
   c1(0,1) = -a - rpVec[1] * rpVec[1] / (rVec.Norm()*(1 - e*e));        
   c1(1,1) = rpVec[0] * rpVec[1] / (rVec.Norm()*(1 - e*e));
   c1(2,1) = 0.0;
   c1(0,2) = -a*rpVec[1] / (rVec.Norm()*Sqrt(1 - e*e));
   c1(1,2) = a*Sqrt(1 - e*e)*(rpVec[0] + a*e) / rVec.Norm();
   c1(2,2) = 0.0;
   A = P*c1;                                                               // Equation 3-195 GTDS MatSpec

   // Partial derivative of rpdot w.r.t. semi-major axis a, eccentricity e, and mean anomaly M  (Equation 3-200 GTDS MathSpec)
   c2(0,0) = -rpdotVec[0]/(2*a);
   c2(1,0) = -rpdotVec[1]/(2*a);
   c2(2,0) = 0.0;
   c2(0,1) = rpdotVec[0] * Pow(a / rVec.Norm(), 2)*(2 * rpVec[0] / a + e*Pow(rpVec[1] / a, 2) / (1 - e*e));
   c2(1,1) = n*Pow(a / rVec.Norm(), 2)*(rpVec[0] * rpVec[0] / rVec.Norm() - rpVec[1] * rpVec[1] / a / (1 - e*e)) / Sqrt(1 - e*e);
   c2(2,1) = 0.0;
   c2(0,2) = -n*Pow(a / rVec.Norm(), 3)*rpVec[0];
   c2(1,2) = -n*Pow(a / rVec.Norm(), 3)*rpVec[1];
   c2(2,2) = 0.0;
   B = P*c2;                                                               // Equation 3-196 GTDS MatSpec

   // Partial derivative w.r.t. Ohm  (Equation 3-202 GTDS MathSpec)
   Rmatrix33 dPdOhm;
   dPdOhm(0,0) = -P(1,0);
   dPdOhm(0,1) = -P(1,1);
   dPdOhm(0,2) = -P(1,2);         // 0.0;  // Equation 3-202 GTDS MatSpec is incorrect at here
   dPdOhm(1,0) = P(0,0);
   dPdOhm(1,1) = P(0,1);
   dPdOhm(1,2) = P(0,2);          // 0.0;  // Equation 3-202 GTDS MatSpec is incorrect at here
   dPdOhm(2,0) = 0.0;
   dPdOhm(2,1) = 0.0;
   dPdOhm(2,2) = 0.0;
   Rvector3 drdOhm = dPdOhm * rpVec;                                       // Equation 3-197 GTDS MathSpec
   Rvector3 drdotdOhm = dPdOhm * rpdotVec;                                 // Equation 3-198 GTDS MathSpec

   // Partial derivative w.r.t. omega  (Equation 3-203 GTDS MathSpec)
   Rmatrix33 dPdomega;
   dPdomega(0,0) = P(0,1);
   dPdomega(0,1) = -P(0,0);
   dPdomega(0,2) = 0.0;
   dPdomega(1,0) = P(1,1);
   dPdomega(1,1) = -P(1,0);
   dPdomega(1,2) = 0.0;
   dPdomega(2,0) = P(2,1);
   dPdomega(2,1) = -P(2,0);
   dPdomega(2,2) = 0.0;
   Rvector3 drdomega = dPdomega * rpVec;                                   // Equation 3-197 GTDS MathSpec
   Rvector3 drdotdomega = dPdomega * rpdotVec;                             // Equation 3-198 GTDS MathSpec

   // Partial derivative w.r.t. i  (Equation 3-204 GTDS MathSpec)
   Rmatrix33 dPdi;
   dPdi(0,0) = Sin(Ohm)*Sin(iAngle)*Sin(omega);
   dPdi(0,1) = Sin(Ohm)*Sin(iAngle)*Cos(omega);
   dPdi(0,2) = Sin(Ohm)*Cos(iAngle);                   // 0.0;    // Equation 3-204 GTDS MathSpec is incorrect here
   dPdi(1,0) = -Cos(Ohm)*Sin(iAngle)*Sin(omega);
   dPdi(1,1) = -Cos(Ohm)*Sin(iAngle)*Cos(omega);
   dPdi(1,2) = -Cos(Ohm)*Cos(iAngle);                  // 0.0;    // Equation 3-204 GTDS MathSpec is incorrect here
   dPdi(2,0) = Cos(iAngle)*Sin(omega);
   dPdi(2,1) = Cos(iAngle)*Cos(omega);
   dPdi(2,2) = -Sin(iAngle);                           // 0.0;    // Equation 3-204 GTDS MathSpec is incorrect here
   Rvector3 drdi = dPdi * rpVec;                                           // Equation 3-197 GTDS MathSpec
   Rvector3 drdotdi = dPdi * rpdotVec;                                     // Equation 3-198 GTDS MathSpec

   result(0,0) = A(0,0); result(0,1) = A(0,1); result(0,5) = A(0,2);
   result(1,0) = A(1,0); result(1,1) = A(1,1); result(1,5) = A(1,2);
   result(2,0) = A(2,0); result(2,1) = A(2,1); result(2,5) = A(2,2); //A(0,2);
   result(3,0) = B(0,0); result(3,1) = B(0,1); result(3,5) = B(0,2);
   result(4,0) = B(1,0); result(4,1) = B(1,1); result(4,5) = B(1,2);
   result(5,0) = B(2,0); result(5,1) = B(2,1); result(5,5) = B(2,2); //B(0,2);
   
   result(0,2) = drdi[0];    result(0,3) = drdOhm[0];    result(0,4) = drdomega[0];
   result(1,2) = drdi[1];    result(1,3) = drdOhm[1];    result(1,4) = drdomega[1];
   result(2,2) = drdi[2];    result(2,3) = drdOhm[2];    result(2,4) = drdomega[2];
   result(3,2) = drdotdi[0]; result(3,3) = drdotdOhm[0]; result(3,4) = drdotdomega[0];
   result(4,2) = drdotdi[1]; result(4,3) = drdotdOhm[1]; result(4,4) = drdotdomega[1];
   result(5,2) = drdotdi[2]; result(5,3) = drdotdOhm[2]; result(5,4) = drdotdomega[2];


   // convert radian to degree INC, RAAN, AOP, and MA from radian to degree
   for(UnsignedInt row = 0; row < 6; ++row)
      for (UnsignedInt col = 2; col < 6; ++col)
         result(row,col) = result(row,col)*GmatMathConstants::RAD_PER_DEG;      // Note that: di, dOhm, domega, and dM are in denominator 
   
   return result;
}


//------------------------------------------------------------------------------
// other methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool ValidateValue(const std::string& label,          Real value,
//                    const std::string &compareTo = "", Real compareValue = 0.0)
//------------------------------------------------------------------------------
/**
 * Validates an input value for the specified orbit element.  When requested, also
 * validates the value against another input value, for coupled quantities.
 *
 * @param <label>        Orbit element label
 * @param <value>        Input value
 * @param <compareTo>    Name of coupled orbit element to validate against
 * @param <compareValue> Value of coupled orbit element to validate against
 *
 * @return true if value is allowed/valid; false otherwise
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::ValidateValue(const std::string &label,       Real value,
                                        const std::string &errorMsgFmt, Integer dataPrecision,
                                        const std::string &compareTo,   Real compareValue)
{
   #ifdef DEBUG_SC_CONVERT_VALIDATE
      MessageInterface::ShowMessage(
         "SCU::ValidateValue() entered with label = %s, value = %le\n   "
         "dataPrecision = %d, compareTo = %s, compareValue = %le\n",
         label.c_str(), value, dataPrecision, compareTo.c_str(), compareValue);
   #endif
      
   // NOTE - the labels for Delaunay will be checked without conversion to
   // all upper case, as element names are identical except for the last
   // character being lower or upper case, e.g. Delaunayl vs. DelaunayL
   std::string labelUpper   = GmatStringUtil::ToUpper(label);
   std::string compareUpper = GmatStringUtil::ToUpper(compareTo);
   #ifdef DEBUG_SC_CONVERT_VALIDATE
      MessageInterface::ShowMessage(
            "SCU::ValidateValue:  labelUpper = %s, compareUpper = %s\n", labelUpper.c_str(), compareUpper.c_str());
   #endif

   // These are only limited by the upper and lower limits of the compiler's Real type
   // Now handled in else part so that we don't need to keep updating this (LOJ: 2014.01.17)
   // if ((labelUpper == "X")              || (labelUpper == "Y")           || (labelUpper == "Z")                 ||
   //     (labelUpper == "VX")             || (labelUpper == "VY")          || (labelUpper == "VZ")                ||
   //     (labelUpper == "AOP")            || (labelUpper == "AZI")         || (labelUpper == "EQUINOCTIALP")      ||
   //     (labelUpper == "EQUINOCTIALQ")   || (labelUpper == "RA")          || (labelUpper == "RAAN")              ||
   //     (labelUpper == "RAV")            || (labelUpper == "TA"))
   // {
   //    return true;
   // }
   // else if (labelUpper == "RADAPO")
   
   // Add here if any real number is not allowed
   if (labelUpper == "RADAPO")
   {
      if (GmatMathUtil::Abs(value) < .001)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "RadApo", "Real Number >= 1 meter");
         throw ue;
      }
      if (compareUpper == "RADPER")
      {
         if ((value > 0.0) && (value < compareValue))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "RadApo", "Real Number < 0.0 if RadApo < RadPer");
            throw ue;
         }
      }
   }
   else if (labelUpper == "RADPER")
   {
      if (GmatMathUtil::Abs(value) < .001)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "RadPer", "Real Number >= 1 meter");
         throw ue;
      }
      else if (IsEqual(value, 0.0, .001))
      {
         UtilityException ue;
         ue.SetDetails("Parabolic orbits are not currently supported.  Radius of Periapsis must be greater than zero");
         throw ue;
      }
   }
   else if (labelUpper == "ECC")
   {
      if (IsEqual(value, 1.0, PARABOLIC_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "Real Number != 1";
         if (PARABOLIC_TOL != 0.0)
            rangeMsg << " (tolerance = " << PARABOLIC_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "ECC", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "SMA")
      {
         if (((value > 1.0 + PARABOLIC_TOL) || (value < 0.0 - PARABOLIC_TOL)) && (compareValue > 0.0))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "ECC", "0 < Real Number < 1 when SMA > 0");
            throw ue;
         }
         if ((value <= 1.0 - PARABOLIC_TOL) && (compareValue < 0.0))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "ECC", "Real Number > 1 when SMA < 0");
            throw ue;
         }
      }
   }
   else if (label == "BrouwerShortECC" || label == "BrouwerLongECC")
   {
      // range: 0.99 > BrouwerECC >= 0
      if (value < 0 || value >= 0.99)
      {
         std::string range = "0 <= Real Number < 0.99";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), range.c_str());
         throw ue;
      }
   }
   else if (labelUpper == "SMA")
   {
      if (IsEqual(value, 0.0, SINGULAR_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "Real Number != 0";
         if (SINGULAR_TOL != 0.0)
            rangeMsg << " (tolerance = " << SINGULAR_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "SMA", rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "ECC")
      {
         if ((value < 0.0) && (compareValue < 1.0 - SINGULAR_TOL) && (compareValue > 0.0 + SINGULAR_TOL))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "SMA", "Real Number > 0 when 0 < ECC < 1");
            throw ue;
         }
         else if ((value > 0.0) && (compareValue > 1.0 + SINGULAR_TOL))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "SMA", "Real Number < 0 when ECC > 1");
            throw ue;
         }
      }
   }
   else if (label == "BrouwerShortSMA" || label == "BrouwerLongSMA")
   {
      if (GmatStringUtil::EndsWith(compareTo, "ECC"))
      {
         // range: SMA >= 1000 / (1 - ECC);
         if (value < (1000 / (1 - compareValue)))
         {
            std::string rangeMsg = "Real Number >= 1000 / (1 - " + compareTo;
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          label.c_str(), rangeMsg.c_str());
            throw ue;
         }
      }
   }
   else if (labelUpper == "INC" || labelUpper == "FPA" ||
            label == "BrouwerShortINC" || label == "BrouwerLongINC") // 0 =< value <= 180
   {
      if ((value < 0.0 - ANGLE_TOL) || (value > 180.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "0.0 <= Real Number <= 180.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       "INC", rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "RMAG" || label == "PlanetodeticRMAG" ||
            labelUpper == "VMAG" || label == "PlanetodeticVMAG") // value >= 1.0e-10
   {
      if (value < 1.0e-10)
      {
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), "Real Number > 1.0e-10");
         throw ue;
      }
   }
   else if (labelUpper == "DEC" || label == "PlanetodeticHFPA" ||
            labelUpper == "DECV" || label == "PlanetodeticLAT") // -90 <= value <= 90
   {
      if ((value < -90.0 - ANGLE_TOL) || (value > 90.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-90.0 <= Real Number <= 90.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "EQUINOCTIALK" || label == "ModEqunoctialK")
   {
      if ((value < -1.0 + EQUINOCTIAL_TOL) || (value > 1.0 - EQUINOCTIAL_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-1 < Real Number < 1";
         if (EQUINOCTIAL_TOL != 0.0)
            rangeMsg << " (tolerance = " << EQUINOCTIAL_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "EQUINOCTIALH" || label == "ModEqunoctialH")
      {
         if (GmatMathUtil::Sqrt(value * value + compareValue * compareValue) > 1.0 - EQUINOCTIAL_TOL)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          label.c_str(), "Sqrt(EquinoctialH^2 + EquinoctialK^2) < 0.99999");
            throw ue;
         }
      }
   }
   else if (labelUpper == "EQUINOCTIALH")
   {
      if ((value < -1.0 + EQUINOCTIAL_TOL) || (value > 1.0 - EQUINOCTIAL_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-1 < Real Number < 1";
         if (EQUINOCTIAL_TOL != 0.0)
            rangeMsg << " (tolerance = " << EQUINOCTIAL_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
      if (compareUpper == "EQUINOCTIALK")
      {
         if (GmatMathUtil::Sqrt(value * value + compareValue * compareValue) > 1.0 - EQUINOCTIAL_TOL)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          label.c_str(), "Sqrt(EquinoctialH^2 + EquinoctialK^2) < 0.99999");
            throw ue;
         }
      }
   }
   else if (labelUpper == "MLONG") // -360 <= value <= 360
   {
      if ((value < -360.0 - ANGLE_TOL) || (value > 360.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-360.0 <= Real Number <= 360.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "TLONG" || label == "Delaunayl" ||
            label == "Delaunayg" || label == "Delaunayh") // 0 <= value <= 360
   {
      if ((value < -ANGLE_TOL) || (value > 360.0 + ANGLE_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "0 <= Real Number <= 360.0";
         if (ANGLE_TOL != 0.0)
            rangeMsg << " (tolerance = " << ANGLE_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (labelUpper == "SEMILATUSRECTUM") // value >= 1.e-7
   {
      if (value < 1.E-7)
      {
         std::stringstream rangeMsg;
         rangeMsg << "1.0e-7 <= Real Number";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (label == "AltEquinoctialP" || label == "AltEquinoctialQ") // -1 <= x <= 1
   {
      // Fix for GMT-4446 (LOJ: 2014.03.20)
      if ((value < -1.0 + EQUINOCTIAL_TOL) || (value > 1.0 - EQUINOCTIAL_TOL))
      {
         std::stringstream rangeMsg;
         rangeMsg << "-1 < Real Number < 1";
         if (EQUINOCTIAL_TOL != 0.0)
            rangeMsg << " (tolerance = " << EQUINOCTIAL_TOL << ")";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
   }
   else if (label == "DelaunayL") // value >= 0
   {
      if (value < 0)
      {
         std::stringstream rangeMsg;
         rangeMsg << "0 <= Real Number";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
      if (compareTo == "DelaunayG")
      {
         if ((compareValue / value) > 1)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "DelaunayL", "(DelaunayG / DelaunayL) <= 1");
            throw ue;
         }
      }
   }
   else if (label == "DelaunayG")
   {
      if (value < 0)
      {
         std::stringstream rangeMsg;
         rangeMsg << "0 <= Real Number";
         UtilityException ue;
         ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                       label.c_str(), rangeMsg.str().c_str());
         throw ue;
      }
      if (compareTo == "DelaunayH")
      {
         if (GmatMathUtil::Abs(value) < GmatMathUtil::Abs(compareValue))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "DelaunayG", "| DelaunayH | <= | DelaunayG | ");
            throw ue;
         }
      }
      if (compareTo == "DelaunayL")
      {
         if ((value / compareValue) > 1)
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "DelaunayG", "(DelaunayG / DelaunayL) <= 1");
            throw ue;
         }
      }
   }
   else if (label == "DelaunayH")
   {
      if (compareTo == "DelaunayG")
      {
         if (GmatMathUtil::Abs(value) > GmatMathUtil::Abs(compareValue))
         {
            UtilityException ue;
            ue.SetDetails(errorMsgFmt.c_str(), GmatStringUtil::ToString(value, dataPrecision).c_str(),
                          "DelaunayH", "| DelaunayH | <= | DelaunayG | ");
            throw ue;
         }
      }
   }
   else
   {
      #ifdef DEBUG_SC_CONVERT_VALIDATE
      MessageInterface::ShowMessage
         ("SCU::ValidateValue() state label '%s' can be any real number\n", label.c_str());
      #endif
   }
   
   #ifdef DEBUG_SC_CONVERT_VALIDATE
   MessageInterface::ShowMessage("SCU::ValidateValue() returning true for label: %s\n", label.c_str());
   #endif
   return true;
}

//------------------------------------------------------------------------------
// const std::string* GetStateTypeList()
//------------------------------------------------------------------------------
/**
 * Returns the list of state types
 *
 * @return list of valid state types
 */
//------------------------------------------------------------------------------
const std::string* StateConversionUtil::GetStateTypeList()
{
   return STATE_TYPE_TEXT;
}

//------------------------------------------------------------------------------
// bool RequiresCelestialBodyOrigin(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified state type
 * requires a celestial body origin
 *
 * @param <type> State type
 *
 * @return flag indicating whether or not the specified state type
 * requires a celestial body origin
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::RequiresCelestialBodyOrigin(const std::string &type)
{
   for (unsigned int ii = 0; ii < StateTypeCount; ii++)
   {
      if (type == STATE_TYPE_TEXT[ii])  return REQUIRES_CB_ORIGIN[ii];
   }
   return false;  // by default
}

//------------------------------------------------------------------------------
// bool RequiresFixedCoordinateSystem(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the specified state type
 * requires a fixed coordinate system
 *
 * @param <type> State type
 *
 * @return flag indicating whether or not the specified state type
 * requires a fixed coordinate system
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::RequiresFixedCoordinateSystem(const std::string &type)
{
   for (unsigned int ii = 0; ii < StateTypeCount; ii++)
   {
      if (type == STATE_TYPE_TEXT[ii])  return REQUIRES_FIXED_CS[ii];
   }
   return false;  // by default
}

//------------------------------------------------------------------------------
// AnomalyType GetAnomalyType(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Returns the AnomalyType corresponding to the specified string
 *
 * @param <typeStr> Anomaly type string
 *
 * @return anomaly type
 */
//------------------------------------------------------------------------------
StateConversionUtil::AnomalyType StateConversionUtil::GetAnomalyType(const std::string &typeStr)
{
   #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("GetAnomalyType (%s) called.\n", typeStr.c_str());
   #endif
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_LONG_TEXT[i])
         return (AnomalyType) i;
   }

   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_SHORT_TEXT[i])
         return (AnomalyType) i;
   }

   throw UtilityException
      ("Invalid Anomaly Type \"" + typeStr + "\"\nAllowed "
       "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
       "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
}

//------------------------------------------------------------------------------
// bool IsValidAnomalyType(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not the input string is a valid anomaly type
 *
 * @param <typeStr> Anomaly type string
 *
 * @return flag indicating whether or not the input string is a valid anomaly type
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::IsValidAnomalyType(const std::string &anomType)
{
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (anomType == ANOMALY_LONG_TEXT[i])
         return true;
   }

   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (anomType == ANOMALY_SHORT_TEXT[i])
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// bool IsRvValid(Real r[3], Real v[3])
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not the input position/velocity is valid
 *
 * @param <r>   Position
 * @param <v>   Velocity
 *
 * @return flag indicating whether or not the input position/velocity is valid
 */
//------------------------------------------------------------------------------
bool StateConversionUtil::IsRvValid(Real r[3], Real v[3])
{
   if ( ((r[0]*r[0] + r[1]*r[1] + r[2]*r[2]) > ORBIT_TOL_SQ) &&
        ((v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > ORBIT_TOL_SQ) )
      return true;
   else
      return false;

}

//------------------------------------------------------------------------------
// const std::string* GetLongTypeNameList()
//------------------------------------------------------------------------------
/**
 * Returns list of long anomaly names
 *
 * @return list of long anomaly names
 */
//------------------------------------------------------------------------------
const std::string* StateConversionUtil::GetLongTypeNameList()
{
   return ANOMALY_LONG_TEXT;
}

//------------------------------------------------------------------------------
// std::string GetAnomalyShortText(const std::string &anomalyType)
//------------------------------------------------------------------------------
/**
 * Returns the short anomaly name for the specified anomaly type
 *
 * @param <anomalyType>  anomaly type
 *
 * @return corresponding short anomaly name
 */
//------------------------------------------------------------------------------
std::string StateConversionUtil::GetAnomalyShortText(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "TA";
   else if (type == "Mean Anomaly" || type == "MA")
      return "MA";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "EA";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "HA";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
}

//------------------------------------------------------------------------------
// std::string GetAnomalyLongText(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns the long anomaly name for the specified anomaly type
 *
 * @param <anomalyType>  anomaly type
 *
 * @return corresponding long anomaly name
 */
//------------------------------------------------------------------------------
std::string StateConversionUtil::GetAnomalyLongText(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "True Anomaly";
   else if (type == "Mean Anomaly" || type == "MA")
      return "Mean Anomaly";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "Eccentric Anomaly";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "Hyperbolic Anomaly";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");

}


//------------------------------------------------------------------------------
// Integer ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
//                           Real elem[6], Real *ma)
//------------------------------------------------------------------------------
/*
 * @param <grav> input gravitational constant
 * @param <r>    input position vector in cartesian coordinate
 * @param <v>    input velocity vector in cartesian coordinate
 * @param <tfp>  output time from periapsis
 * @param <elem> output six keplerian elements
 *               (1) = semimajor axis
 *               (2) = eccentricity
 *               (3) = inclination (deg)
 *               (4) = longitude of ascending node (deg)
 *               (5) = argument of periapsis (deg)
 *               (6) = true anomaly (deg)
 * @param <ma>   output mean anomaly (deg)
 *
 * @return       Error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeCartToKepl(Real grav,    Real r[3], Real v[3], Real *tfp,
                                               Real elem[6], Real *ma)
{
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage("StateConversionUtil::ComputeCartToKepl called ... \n");
      MessageInterface::ShowMessage("   grav = %12.10f\n", grav);
      MessageInterface::ShowMessage("   KEP_ECC_TOL = %12.10f\n", GmatOrbitConstants::KEP_ECC_TOL);
   #endif

   if (Abs(grav) < 1E-30)
      return(2);

   Rvector3 pos(r[0], r[1], r[2]);
   Rvector3 vel(v[0], v[1], v[2]);

   // eqn 4.1
   Rvector3 angMomentum = Cross(pos, vel);

   // eqn 4.2
   Real h = angMomentum.GetMagnitude();
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, pos = %12.14f  %12.14f  %12.14f \n", pos[0], pos[1], pos[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, vel = %12.14f  %12.14f  %12.14f \n", vel[0], vel[1], vel[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, angMomentum = %12.10f  %12.10f  %12.10f\n",
            angMomentum[0], angMomentum[1], angMomentum[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, h = %12.10f\n", h);
   #endif

   // eqn 4.3
   Rvector3 v3(0.0,0.0,1.0);
   Rvector3 nodeVec = Cross(v3, angMomentum);

   // eqn 4.4
   Real n = nodeVec.GetMagnitude();

   // eqn 4.5 - 4.6
   Real posMag = pos.GetMagnitude();
   Real velMag = vel.GetMagnitude();
   if (posMag == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - position vector is a zero vector.\n");
   }
   if (grav == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - mu is zero.\n");
   }

   // eqn 4.7 - 4.8
   Rvector3 eccVec = (1/grav)*((velMag*velMag - grav/posMag)*pos - (pos * vel) * vel);
   Real e = eccVec.GetMagnitude();

   // eqn 4.9
   Real zeta = 0.5*velMag*velMag - (grav/posMag);
   if (zeta == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - computed zeta is zero.\n");
   }

   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, nodeVec = %12.10f  %12.10f  %12.10f \n",
            nodeVec[0], nodeVec[1], nodeVec[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, n = %12.10f\n", n);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, posMag = %12.10f\n", posMag);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, velMag = %12.10f\n", velMag);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, eccVec = %12.10f  %12.10f  %12.10f \n", eccVec[0], eccVec[1], eccVec[2]);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, e = %12.10f\n", e);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, zeta = %12.10f\n", zeta);
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, Abs(1.0 - e) = %12.10f\n", Abs(1.0 - e));
   #endif

   if ((Abs(1.0 - e)) <= GmatOrbitConstants::KEP_ECC_TOL)
   {
      std::string errmsg = "Warning: GMAT does not support parabolic orbits ";
      errmsg += "in conversion from Cartesian to Keplerian state.\n";
      throw UtilityException(errmsg);
   }

   // eqn 4.10
   Real sma = -grav/(2*zeta);
   #ifdef DEBUG_CART_TO_KEPL
      MessageInterface::ShowMessage(
            "   in ComputeCartToKepl, sma = %12.10f\n", sma);
   #endif


   if (Abs(sma*(1 - e)) < .001)
   {
      throw UtilityException
      ("Error in conversion from Cartesian to Keplerian state: "
       "The state results in a singular conic section with radius of periapsis less than 1 m.\n");

   }
   // eqn 4.11
   if (h == 0.0)
   {
      throw UtilityException("Cannot convert from Cartesian to Keplerian - angular momentum is zero.\n");
   }
   Real i = ACos( angMomentum.Get(2)/h );
   
   // For GMT-4169 fix (LOJ: 2014.03.18)
   #if 0
   if (i >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   }
   #endif
   
   Real raan         = 0.0;
   Real argPeriapsis = 0.0;
   Real trueAnom     = 0.0;
   
   // For GMT-4169 fix (LOJ: 2014.03.18)
   //if ( e >= 1E-11 && i >= 1E-11 )  // CASE 1: Non-circular, Inclined Orbit
   if ( e >= 1E-11 && (i >= 1E-11 && i <= PI-1E-11) )  // CASE 1: Non-circular, Inclined Orbit
   {
      if (n == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - line-of-nodes vector is a zero vector.\n");
      }
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;

      argPeriapsis = ACos( (nodeVec*eccVec)/(n*e) );
      if (eccVec.Get(2) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;

      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   // For GMT-4169 fix (LOJ: 2014.03.18)
   //if ( e >= 1E-11 && i < 1E-11 )  // CASE 2: Non-circular, Equatorial Orbit
   if ( e >= 1E-11 && (i < 1E-11 || i > PI-1E-11) )  // CASE 2: Non-circular, Equatorial Orbit
   {
      if (e == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - eccentricity is zero.\n");
      }
      raan = 0;
      argPeriapsis = ACos(eccVec.Get(0)/e);
      if (eccVec.Get(1) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;
      
      // For GMT-4446 fix (LOJ: 2014.03.21)
      if (i > PI-1E-11)
         argPeriapsis= argPeriapsis * -1.0;
      if (argPeriapsis < 0.0)
         argPeriapsis = argPeriapsis + TWO_PI;
      
      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   // For GMT-4169 fix (LOJ: 2014.03.18)
   //if ( e < 1E-11 && i >= 1E-11 )  // CASE 3: Circular, Inclined Orbit
   if ( e < 1E-11 && (i >= 1E-11 && i <= PI-1E-11) )  // CASE 3: Circular, Inclined Orbit
   {
      if (n == 0.0)
      {
         throw UtilityException("Cannot convert from Cartesian to Keplerian - line-of-nodes vector is a zero vector.\n");
      }
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;

      argPeriapsis = 0;

      trueAnom = ACos( (nodeVec*pos)/(n*posMag) );
      if (pos.Get(2) < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   // For GMT-4169 fix (LOJ: 2014.03.18)
   //if ( e < 1E-11 && i < 1E-11 )  // CASE 4: Circular, Equatorial Orbit
   if ( e < 1E-11 && (i < 1E-11 || i > PI-1E-11) )  // CASE 4: Circular, Equatorial Orbit
   {
      raan = 0;
      argPeriapsis = 0;
      trueAnom = ACos(pos.Get(0)/posMag);
      if (pos.Get(1) < 0)
         trueAnom = TWO_PI - trueAnom;
      
      // For GMT-4446 fix (LOJ: 2014.03.21)
      if (i > PI-1E-11)
         trueAnom = trueAnom * -1.0;
      if (trueAnom < 0.0)
         trueAnom = trueAnom + TWO_PI;
   }
   
   elem[0] = sma;
   elem[1] = e;
   elem[2] = i*DEG_PER_RAD;
   elem[3] = raan*DEG_PER_RAD;
   elem[4] = argPeriapsis*DEG_PER_RAD;
   elem[5] = trueAnom*DEG_PER_RAD;
   
   return 0;
}


//------------------------------------------------------------------------------
// Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
//                           Real v[3], AnomalyType anomalyType)
//------------------------------------------------------------------------------
/**
 * Converts Kelperian to Cartesian
 *
 * @param <grav>        Gravitational constant for the central body
 * @param <elem>        Keplerian elements
 * @param <r>           Output Cartesian position
 * @param <v>           Output Cartesian velocity
 * @param <anomalyType> Anomaly type
 *
 * @return Error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                               Real v[3], AnomalyType anomalyType)
{
   Real sma = elem[0],
        ecc = elem[1],
        inc = elem[2]*RAD_PER_DEG,
       raan = elem[3]*RAD_PER_DEG,
        per = elem[4]*RAD_PER_DEG,
       anom = elem[5]*RAD_PER_DEG;

   // **************
   // taken from old code above; necessary to avoid crash, but not in spec
   // if input keplerian anomaly is MA, convert to TA  <<<< what about HA or EA?????   wcs 2011.11.23 ????
   if (anomalyType == MA)
   {
      Real ta;
      Integer iter;
      Integer ret = ComputeMeanToTrueAnomaly(anom, ecc, 1E-8, &ta, &iter);

      if (ret > 0)
         return ret;
      else
         anom = ta;
   }
   // ***************

   // radius near infinite
   #ifdef DEBUG_KEPL_TO_CART
      MessageInterface::ShowMessage("ecc = %12.10f, anom = %12.10f, Cos(anom) = %12.10f, 1+ecc*Cos(anom) = %12.10f\n",
            ecc, anom, Cos(anom), (1+ecc*Cos(anom)));
   #endif

   // eqn 4.24; semilatus rectum
   Real p = sma*(1 - ecc*ecc);

   // orbit parabolic
   if (Abs(p) < INFINITE_TOL)
   {
        return 2;
   }

   Real onePlusECos = 1 + ecc*Cos(anom);
   if (onePlusECos < ORBIT_TOL)
   {
      std::string warn = "Warning: The orbital radius is large in the conversion from ";
      warn += "Keplerian elements to Cartesian state and the state may be near a singularity, ";
      warn += "causing numerical errors in the conversion.\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
   }
   // eqn 4.25; radius
   Real rad = p/onePlusECos;

   // eqn 4.26 - 4.28
   Real cosPerAnom    = Cos(per + anom);
   Real sinPerAnom    = Sin(per + anom);
   Real cosInc        = Cos(inc);
   Real sinInc        = Sin(inc);
   Real cosRaan       = Cos(raan);
   Real sinRaan       = Sin(raan);
   Real sqrtGravP     = Sqrt(grav/p);
   Real cosAnomPlusE  = Cos(anom) + ecc;
   Real sinAnom       = Sin(anom);
   Real cosPer        = Cos(per);
   Real sinPer        = Sin(per);

   r[0] = rad * (cosPerAnom * cosRaan - cosInc * sinPerAnom * sinRaan),
   r[1] = rad * (cosPerAnom * sinRaan + cosInc * sinPerAnom * cosRaan),
   r[2] = rad * sinPerAnom  * sinInc;

   v[0] = sqrtGravP * cosAnomPlusE*(-sinPer*cosRaan-cosInc*sinRaan*cosPer)
                    - sqrtGravP*sinAnom*(cosPer*cosRaan-cosInc*sinRaan*sinPer);

   v[1] = sqrtGravP * cosAnomPlusE*(-sinPer*sinRaan+cosInc*cosRaan*cosPer)
                    - sqrtGravP*sinAnom*(cosPer*sinRaan+cosInc*cosRaan*sinPer);

   v[2] = sqrtGravP * (cosAnomPlusE*sinInc*cosPer - sinAnom*sinInc*sinPer);

   return 0;
}

//------------------------------------------------------------------------------
// Integer ComputeMeanToTrueAnomaly(Real maRadians, Real ecc, Real tol,
//                                  Real *ta, Integer *iter)
//------------------------------------------------------------------------------
/*
 * @param <maRadians>   input elliptical, hyperbolic or mean anomaly in radians
 * @param <ecc>         input eccentricity
 * @param <tol>         input tolerance for accuracy
 * @param <ta>          output true anomaly in radians
 * @param <iter>        output number of iterations
 *
 * @return              error code
 */
//------------------------------------------------------------------------------
Integer StateConversionUtil::ComputeMeanToTrueAnomaly(Real maRadians, Real ecc, Real tol,
                                                      Real *ta, Integer *iter)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("StateConversionUtil::ComputeMeanToTrueAnomaly() entered, maRadians=%.10f, ecc=%.10f, tol=%.15f\n",
       maRadians, ecc, tol);
   #endif
   
   Real temp, temp2;
   Real rm,   e,     e1,  e2,  c,  f,  f1,   f2,    g;
   Real ztol = 1.0e-30;
   bool done = false;
   
   rm = maRadians;
   *iter = 0;

   if (ecc <= 1.0)
   {
      //---------------------------------------------------------
      // elliptical orbit
      //---------------------------------------------------------

      e2 = rm + ecc * Sin(rm);                               // GTDS MathSpec Equation 3-182 
      done = false;
      
      #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("   e2 = %.15f\n", ecc);
      #endif
      
      while (!done)
      {
         *iter = *iter + 1;
         if (*iter > 1000)
         {
            throw UtilityException
               ("ComputeMeanToTrueAnomaly() Stuck in infinite loop in ellitical " 
               "orbit computation using tolerance of " + GmatStringUtil::ToString(tol, 16) + 
               ". Stopped at iteration: " + GmatStringUtil::ToString(*iter) + "\n");
         }

         temp = 1.0 - ecc * Cos(e2);                         // GTDS MathSpec Equation 3-180  Note: a little difference here is that it uses Cos(E) instead of Cos(E-0.5*f) 
                  
         if (temp == 0.0)
         {
            throw UtilityException("Cannot convert Mean to True Anomaly - computed temp is zero.\n");
         }
         
         if (Abs(temp) < ztol)
            return (3);
         
         e1 = e2 - (e2 - ecc * Sin(e2) - rm)/temp;           // GTDS MathSpec Equation 3-181
         
         if (Abs(e2-e1) < tol)
         {
            done = true;
         }
         
         e2 = e1;
      } // while (!done)
      
      #ifdef DEBUG_ANOMALY
      MessageInterface::ShowMessage("   After While (!done), e2 = %.15f\n", e2);
      #endif
      
      e = e2;

      if (e < 0.0)
         e = e + TWO_PI;

      c = Abs(e - GmatMathConstants::PI);

      if (c >= 1.0e-08)
      {
         temp  = 1.0 - ecc;

         if (Abs(temp)< ztol)
            return (5);

         temp2 = (1.0 + ecc)/temp;         // temp2 = (1+ecc)/(1-ecc)

         if (temp2 <0.0)
            return (6);

         f = Sqrt(temp2);
         g = Tan(e/2.0);
         *ta = 2.0 * ATan(f*g);            // tan(TA/2) = Sqrt[(1+ecc)/(1-ecc)] * tan(E/2)
      }
      else
      {
         *ta = e;
      }

      if( *ta < 0.0)
         *ta = *ta + GmatMathConstants::TWO_PI;

   }
   else
   {
      //---------------------------------------------------------
      // hyperbolic orbit
      //---------------------------------------------------------

      // For hyperbolic orbit, anomaly is nolonger to be an angle so we cannot use mod of 2*PI to mean anomaly. 
      // We need to keep its original value for calculation.
      //if (rm > PI)                       // incorrect
      //   rm = rm - TWO_PI;               // incorrect

      //f2 = ecc * Sinh(rm) - rm;          // incorrect
      //f2 = rm / 2;                       // incorrect  // GTDS MathSpec Equation 3-186
      f2 = 0;                              // This is the correct initial value for hyperbolic eccentric anomaly.
                                           // It is converged.
      done = false;      
      while (!done)
      {
         *iter = *iter + 1;
         if (*iter > 1000)
         {
            throw UtilityException
               ("ComputeMeanToTrueAnomaly() "
               "Caught in infinite loop numerical argument "
               "out of domain for sinh() and cosh()\n");
         }

         temp = ecc * Cosh(f2) - 1.0;

         if (Abs(temp) < ztol)
            return (7);

         f1 = f2 - (ecc * Sinh(f2) - f2 - rm) / temp;        // GTDS MathSpec Equation 3-186
         if (Abs(f2-f1) < tol)
         {
            done = true;
         }
         f2 = f1;
      }

      f = f2;
      temp = ecc - 1.0;

      if (Abs(temp) < ztol)
         return (9);

      temp2 = (ecc + 1.0) / temp;                    // temp2 = (ecc+1)/(ecc-1)

      if (temp2 <0.0)
         return (10);

      e = Sqrt(temp2);
      g = Tanh(f/2.0);
      *ta = 2.0 * ATan(e*g);                         // tan(TA/2) = Sqrt[(ecc+1)/(ecc-1)] * Tanh(F/2)    where: F is hyperbolic centric anomaly 

      if (*ta < 0.0)
         *ta = *ta + GmatMathConstants::TWO_PI;
   }

   return (0);

} // end ComputeMeanToTrueAnomaly()
