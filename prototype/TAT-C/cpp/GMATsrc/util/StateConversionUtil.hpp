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
 * Definition of the static class containing methods to convert between
 * orbit state representations.
 * This is a static class: No instances of this class may be declared.
 *
 * @Note for BodyFixed states:
 *        Cartesian states are (x,y,z)
 *        Spherical and Spherical-Ellipsoid states are (latitude, longitude, height)
 */
//------------------------------------------------------------------------------


#ifndef StateConversionUtil_hpp
#define StateConversionUtil_hpp

#include "utildefs.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "Rmatrix33.hpp"
#include "Rmatrix66.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"

class GMATUTIL_API StateConversionUtil
{
public:
//------------------------------------------------------------------------------
// type declarations
//------------------------------------------------------------------------------
enum StateType
{
   CARTESIAN,
   KEPLERIAN,
   MOD_KEPLERIAN,
   SPH_AZFPA,
   SPH_RADEC,
   EQUINOCTIAL,
   MOD_EQUINOCTIAL,  // Modifed by M.H.
   ALT_EQUINOCTIAL,  // Alternate Equinoctial by HYKim
   DELAUNAY,
   PLANETODETIC,
   OUT_ASYM,
   IN_ASYM,
   BROLYD_SHORT,
   BROLYD_LONG,
   StateTypeCount
};

enum AnomalyType
{
   TA,
   MA,
   EA,
   HA,
   AnomalyTypeCount
};

#define EARTH_MU GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]
#define EARTH_FLATTENING GmatSolarSystemDefaults::PLANET_FLATTENING[GmatSolarSystemDefaults::EARTH]
#define EARTH_EQ_RADIUS GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]

//------------------------------------------------------------------------------
// general state conversion methods
//------------------------------------------------------------------------------

static Rvector6 Convert(const Real *state,
                        const std::string &fromType, const std::string &toType,
                        Real mu         = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH],
                        Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[GmatSolarSystemDefaults::EARTH],
                        Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH],
                        const std::string &anomalyType = "TA");
static Rvector6 Convert(const Rvector6 &state,
                        const std::string &fromType, const std::string &toType,
                        Real mu         = GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH],
                        Real flattening = GmatSolarSystemDefaults::PLANET_FLATTENING[GmatSolarSystemDefaults::EARTH],
                        Real eqRadius   = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH],
                        const std::string &anomalyType = "TA");

//------------------------------------------------------------------------------
// specific state conversion methods
//------------------------------------------------------------------------------
static Rvector6 ConvertFromCartesian(const std::string &toType, const Rvector6 &state,
                                     Real mu = EARTH_MU,
                                     const std::string &anomalyType = "TA",
                                     Real flattening =  EARTH_FLATTENING,
                                     Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromKeplerian(const std::string &toType, const Rvector6 &state, 
                                     Real mu = EARTH_MU,
                                     const std::string &anomalyType = "TA",
                                     Real flattening =  EARTH_FLATTENING,
                                     Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromModKeplerian(const std::string &toType, const Rvector6 &state,
                                        Real mu = EARTH_MU,
                                        const std::string &anomalyType = "TA",
                                        Real flattening =  EARTH_FLATTENING,
                                        Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromSphericalAZFPA(const std::string &toType, const Rvector6 &state, 
                                          Real mu = EARTH_MU,
                                          const std::string &anomalyType = "TA",
                                          Real flattening =  EARTH_FLATTENING,
                                          Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromSphericalRADEC(const std::string &toType, const Rvector6 &state,
                                          Real mu = EARTH_MU,
                                          const std::string &anomalyType = "TA",
                                          Real flattening =  EARTH_FLATTENING,
                                          Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromEquinoctial(const std::string &toType, const Rvector6 &state,
                                       Real mu = EARTH_MU,
                                       const std::string &anomalyType = "TA",
                                       Real flattening =  EARTH_FLATTENING,
                                       Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromModEquinoctial(const std::string &toType, const Rvector6 &state,
                                          Real mu = EARTH_MU,
                                          const std::string &anomalyType = "TA",
                                          Real flattening =  EARTH_FLATTENING,
                                          Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromAltEquinoctial(const std::string &toType, const Rvector6 &state,
                                          Real mu = EARTH_MU,
                                          const std::string &anomalyType = "TA",
                                          Real flattening =  EARTH_FLATTENING,
                                          Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromDelaunay(const std::string &toType, const Rvector6 &state,
                                    Real mu = EARTH_MU,
                                    const std::string &anomalyType = "TA",
                                    Real flattening =  EARTH_FLATTENING,
                                    Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromPlanetodetic(const std::string &toType, const Rvector6 &state,
                                        Real mu = EARTH_MU,
                                        const std::string &anomalyType = "TA",
                                        Real flattening =  EARTH_FLATTENING,
                                        Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromIncomingAsymptote(const std::string &toType, const Rvector6 &state,
                                             Real mu = EARTH_MU,
                                             const std::string &anomalyType = "TA",
                                             Real flattening =  EARTH_FLATTENING,
                                             Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromOutgoingAsymptote(const std::string &toType, const Rvector6 &state,
                                             Real mu = EARTH_MU,
                                             const std::string &anomalyType = "TA",
                                             Real flattening =  EARTH_FLATTENING,
                                             Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromBrouwerMeanShort(const std::string &toType, const Rvector6 &state,
                                            Real mu = EARTH_MU,
                                            const std::string &anomalyType = "TA",
                                            Real flattening =  EARTH_FLATTENING,
                                            Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 ConvertFromBrouwerMeanLong(const std::string &toType, const Rvector6 &state,
                                           Real mu = EARTH_MU,
                                           const std::string &anomalyType = "TA",
                                           Real flattening =  EARTH_FLATTENING,
                                           Real eqRadius = EARTH_EQ_RADIUS);

static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                     const Rvector3 &vel,
                                     AnomalyType anomalyType);
static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                     const Rvector3 &vel,
                                     const std::string &anomalyType = "TA");
static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state,
                                     AnomalyType anomalyType);
static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state,
                                     const std::string &anomalyType = "TA");
static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state, Real *ma);

static Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
                                     AnomalyType anomalyType);
static Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
                                     const std::string &anomalyType = "TA");

static Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartesian);
static Rvector6 SphericalAZFPAToCartesian(const Rvector6& spherical);

static Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian);
static Rvector6 SphericalRADECToCartesian(const Rvector6& spherical);

static Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian);
static Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian);

static Rvector6 CartesianToEquinoctial(const Rvector6& cartesian,   const Real& mu);
static Rvector6 EquinoctialToCartesian(const Rvector6& equinoctial, const Real& mu);

static Rvector6 CartesianToModEquinoctial(const Rvector6& cartesian,   const Real& mu); // MODIFIED BY M.H.
static Rvector6 ModEquinoctialToCartesian(const Rvector6& modequinoctial, const Real& mu);

static Rvector6 KeplerianToDelaunay(const Rvector6& keplerian, const Real& mu); // MODIFIED BY M.H.
static Rvector6 DelaunayToKeplerian(const Rvector6& delaunay, const Real& mu);

static Rvector6 CartesianToPlanetodetic(const Rvector6& cartesian, Real flattening, Real eqRadius); // MODIFIED BY M.H.
static Rvector6 PlanetodeticToCartesian(const Rvector6& planetodetic, Real flattening, Real eqRadius);

// modified by YK
static Rvector6 CartesianToIncomingAsymptote(Real mu, const Rvector6& cartesian);
static Rvector6 IncomingAsymptoteToCartesian(Real mu, const Rvector6& inasym);

static Rvector6 CartesianToOutgoingAsymptote(Real mu, const Rvector6& cartesian);
static Rvector6 OutgoingAsymptoteToCartesian(Real mu, const Rvector6& outasym);

static Rvector6 CartesianToBrouwerMeanShort(Real mu, const Rvector6& cartesian);
static Rvector6 BrouwerMeanShortToOsculatingElements(Real mu, const Rvector6 &blms); // Anomaly Type is fixed to "MA"
static Rvector6 BrouwerMeanShortToCartesian(Real mu, const Rvector6 &blms);

static Rvector6 CartesianToBrouwerMeanLong(Real mu, const Rvector6& cartesian);
static Rvector6 BrouwerMeanLongToOsculatingElements(Real mu, const Rvector6 &blml); // Anomaly Type is fixed to "MA"
static Rvector6 BrouwerMeanLongToCartesian(Real mu, const Rvector6 &blml);

// Alternate Equinoctial by HYKim 
static Rvector6 EquinoctialToAltEquinoctial(const Rvector6& equinoctial);
static Rvector6 AltEquinoctialToEquinoctial(const Rvector6& altequinoctial);


//------------------------------------------------------------------------------
// anomaly conversion methods
//------------------------------------------------------------------------------
static Real     TrueToMeanAnomaly(Real taRadians,        Real ecc, bool modBy2Pi = false);
static Real     TrueToEccentricAnomaly(Real taRadians,   Real ecc, bool modBy2Pi = false);
static Real     TrueToHyperbolicAnomaly(Real taRadians,  Real ecc, bool modBy2Pi = false);

static Real     MeanToTrueAnomaly(Real maRadians,        Real ecc, Real tol      = 1.0e-08);
static Real     EccentricToTrueAnomaly(Real eaRadians,   Real ecc, bool modBy2Pi = false);
static Real     HyperbolicToTrueAnomaly(Real haRadians,  Real ecc, bool modBy2Pi = false);

static Real     ConvertFromTrueAnomaly(const std::string &toType, Real taRadians, Real ecc, bool modBy2Pi = false);
static Real     ConvertFromTrueAnomaly(AnomalyType toType,        Real taRadians, Real ecc, bool modBy2Pi = false);
static Real     ConvertToTrueAnomaly(const std::string &fromType, Real taRadians, Real ecc, bool modBy2Pi = false);
static Real     ConvertToTrueAnomaly(AnomalyType fromType,        Real taRadians, Real ecc, bool modBy2Pi = false);

static Real     CartesianToTA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);
static Real     CartesianToMA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);
static Real     CartesianToEA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);
static Real     CartesianToHA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);

//------------------------------------------------------------------------------
// other conversion methods
//------------------------------------------------------------------------------
static Real     CartesianToSMA(Real mu, const Rvector3 &pos,
                           const Rvector3 &vel);
static Real     CartesianToECC(Real mu, const Rvector3 &pos,
                           const Rvector3 &vel);
static Real     CartesianToINC(Real mu, const Rvector3 &pos,
                           const Rvector3 &vel, bool inRadian = false);
static Real     CartesianToRAAN(Real mu, const Rvector3 &pos,
                            const Rvector3 &vel, bool inRadian = false);
static Real     CartesianToAOP(Real mu, const Rvector3 &pos,
                           const Rvector3 &vel, bool inRadian = false);
static Rvector3 CartesianToEccVector(Real mu, const Rvector3 &pos,
                                     const Rvector3 &vel);
static Rvector3 CartesianToDirOfLineOfNode(const Rvector3 &pos,
                                           const Rvector3 &vel);
static Rvector6 CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
                                           const Rvector3 &vel);

//------------------------------------------------------------------------------
// derivative conversion methods
//------------------------------------------------------------------------------
static Real CalculateEccentricAnomaly(Real e, Real M);
static Rmatrix66 CartesianToKeplerianDerivativeConversion(Real mu, const Rvector6 &cartesianState);
static Rmatrix66 CartesianToKeplerianDerivativeConversionWithKeplInput(Real mu, const Rvector6 &keplerState);

static Rmatrix66 CartesianToKeplerianDerivativeConversion_FiniteDiff(Real mu, const Rvector6 &cartesianState);
static Rmatrix66 CartesianToKeplerianDerivativeConversionWithKeplInput_FiniteDiff(Real mu, const Rvector6 &keplerianState);


//------------------------------------------------------------------------------
// other methods
//------------------------------------------------------------------------------
static bool               ValidateValue(const std::string &label,          Real value,
                                        const std::string &errorMsgFmt,    Integer dataPrecision,
                                        const std::string &compareTo = "", Real compareValue = 0.0);

static Integer            GetTypeCount() { return StateTypeCount; }
static const std::string* GetStateTypeList();
static bool               RequiresCelestialBodyOrigin(const std::string &type);
static bool               RequiresFixedCoordinateSystem(const std::string &type);
static AnomalyType        GetAnomalyType(const std::string &typeStr);
static bool               IsValidAnomalyType(const std::string &anomType);
static bool               IsRvValid(Real *r, Real *v);
static const std::string* GetLongTypeNameList();
static std::string        GetAnomalyShortText(const std::string &anomalyType);
static std::string        GetAnomalyLongText(const std::string &anomalyType);

private:
//------------------------------------------------------------------------------
// private static methods
//------------------------------------------------------------------------------
static Integer ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
                                 Real elem[6], Real *ma);
static Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                 Real v[3], AnomalyType anomalyType);
static Integer ComputeMeanToTrueAnomaly(Real maRadians, Real ecc, Real tol,
                                        Real *ta,       Integer *iter);

static Real CalculateEccentricAnomalyEllipse(Real e, Real M);
static Real CalculateEccentricAnomalyHyperbola(Real e, Real M);
static Real CalculateEccentricAnomalyParabola(Real e, Real M);

//------------------------------------------------------------------------------
// private static data
//------------------------------------------------------------------------------
static const Real         ORBIT_TOL;        // = 1.0E-10;
static const Real         ORBIT_TOL_SQ;     // = 1.0E-20;
static const Real         SINGULAR_TOL;     // = .001
static const Real         INFINITE_TOL;     // = 1.0E-30
static const Real         PARABOLIC_TOL;    // = 1.0E-7;
static const Real         MU_TOL;           // = 1.0E-15
static const Real         EQUINOCTIAL_TOL;  // = 1.0E-5
static const Real         ANGLE_TOL;        // = 0.0 for now

static const Integer      MAX_ITERATIONS; // 75

static const Real         DEFAULT_MU;  // km^3/s^2
static const std::string  STATE_TYPE_TEXT[StateTypeCount];
static const bool         REQUIRES_CB_ORIGIN[StateTypeCount];
static const bool         REQUIRES_FIXED_CS[StateTypeCount];

static const std::string  ANOMALY_LONG_TEXT[AnomalyTypeCount];
static const std::string  ANOMALY_SHORT_TEXT[AnomalyTypeCount];

static       bool         apsidesForIncomingAsymptoteWritten;
static       bool         apsidesForOutgoingAsymptoteWritten;
static       bool         brouwerNotConvergingShortWritten;
static       bool         brouwerNotConvergingLongWritten;
static       bool         brouwerSmallEccentricityWritten;
static       bool         criticalInclinationWritten;
static       bool         possibleInaccuracyShortWritten;
static       bool         possibleInaccuracyLongWritten;
static       bool         inaccuracyCriticalAngleWritten;

//------------------------------------------------------------------------------
// private constructors, destructor, operator=
//------------------------------------------------------------------------------
StateConversionUtil();
StateConversionUtil(const StateConversionUtil &copy);
~StateConversionUtil();
StateConversionUtil& operator=(const StateConversionUtil &copy);

};

#endif  //  StateConversionUtil_hpp
